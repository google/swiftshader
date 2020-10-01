// Copyright 2016 The SwiftShader Authors. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "PixelProcessor.hpp"

#include "Primitive.hpp"
#include "Pipeline/Constants.hpp"
#include "Pipeline/PixelProgram.hpp"
#include "System/Debug.hpp"
#include "Vulkan/VkImageView.hpp"
#include "Vulkan/VkPipelineLayout.hpp"

#include <cstring>

namespace sw {

uint32_t PixelProcessor::States::computeHash()
{
	uint32_t *state = reinterpret_cast<uint32_t *>(this);
	uint32_t hash = 0;

	for(unsigned int i = 0; i < sizeof(States) / sizeof(uint32_t); i++)
	{
		hash ^= state[i];
	}

	return hash;
}

bool PixelProcessor::State::operator==(const State &state) const
{
	if(hash != state.hash)
	{
		return false;
	}

	return *static_cast<const States *>(this) == static_cast<const States &>(state);
}

PixelProcessor::PixelProcessor()
{
	setRoutineCacheSize(1024);
}

void PixelProcessor::setBlendConstant(const float4 &blendConstant)
{
	// TODO(b/140935644): Check if clamp is required
	factor.blendConstant4W[0] = word4(static_cast<uint16_t>(iround(0xFFFFu * blendConstant.x)));
	factor.blendConstant4W[1] = word4(static_cast<uint16_t>(iround(0xFFFFu * blendConstant.y)));
	factor.blendConstant4W[2] = word4(static_cast<uint16_t>(iround(0xFFFFu * blendConstant.z)));
	factor.blendConstant4W[3] = word4(static_cast<uint16_t>(iround(0xFFFFu * blendConstant.w)));

	factor.invBlendConstant4W[0] = word4(0xFFFFu - factor.blendConstant4W[0][0]);
	factor.invBlendConstant4W[1] = word4(0xFFFFu - factor.blendConstant4W[1][0]);
	factor.invBlendConstant4W[2] = word4(0xFFFFu - factor.blendConstant4W[2][0]);
	factor.invBlendConstant4W[3] = word4(0xFFFFu - factor.blendConstant4W[3][0]);

	factor.blendConstant4F[0] = float4(blendConstant.x);
	factor.blendConstant4F[1] = float4(blendConstant.y);
	factor.blendConstant4F[2] = float4(blendConstant.z);
	factor.blendConstant4F[3] = float4(blendConstant.w);

	factor.invBlendConstant4F[0] = float4(1 - blendConstant.x);
	factor.invBlendConstant4F[1] = float4(1 - blendConstant.y);
	factor.invBlendConstant4F[2] = float4(1 - blendConstant.z);
	factor.invBlendConstant4F[3] = float4(1 - blendConstant.w);
}

void PixelProcessor::setRoutineCacheSize(int cacheSize)
{
	routineCache = std::make_unique<RoutineCacheType>(clamp(cacheSize, 1, 65536));
}

const PixelProcessor::State PixelProcessor::update(const vk::GraphicsState &pipelineState, const sw::SpirvShader *fragmentShader, const sw::SpirvShader *vertexShader, const vk::Attachments &attachments, bool occlusionEnabled) const
{
	State state;

	state.numClipDistances = vertexShader->getNumOutputClipDistances();
	state.numCullDistances = vertexShader->getNumOutputCullDistances();

	if(fragmentShader)
	{
		state.shaderID = fragmentShader->getSerialID();
		state.pipelineLayoutIdentifier = pipelineState.getPipelineLayout()->identifier;
	}
	else
	{
		state.shaderID = 0;
		state.pipelineLayoutIdentifier = 0;
	}

	state.alphaToCoverage = pipelineState.hasAlphaToCoverage();
	state.depthWriteEnable = pipelineState.depthWriteActive(attachments);

	if(pipelineState.stencilActive(attachments))
	{
		state.stencilActive = true;
		state.frontStencil = pipelineState.getFrontStencil();
		state.backStencil = pipelineState.getBackStencil();
	}

	if(pipelineState.depthBufferActive(attachments))
	{
		state.depthTestActive = true;
		state.depthCompareMode = pipelineState.getDepthCompareMode();
		state.depthFormat = attachments.depthBuffer->getFormat();

		state.depthBias = (pipelineState.getConstantDepthBias() != 0.0f) || (pipelineState.getSlopeDepthBias() != 0.0f);

		// "For fixed-point depth buffers, fragment depth values are always limited to the range [0,1] by clamping after depth bias addition is performed.
		//  Unless the VK_EXT_depth_range_unrestricted extension is enabled, fragment depth values are clamped even when the depth buffer uses a floating-point representation."
		state.depthClamp = !state.depthFormat.isFloatFormat() || !pipelineState.hasDepthRangeUnrestricted();
	}

	state.occlusionEnabled = occlusionEnabled;

	bool fragmentContainsKill = (fragmentShader && fragmentShader->getModes().ContainsKill);
	for(int i = 0; i < RENDERTARGETS; i++)
	{
		state.colorWriteMask |= pipelineState.colorWriteActive(i, attachments) << (4 * i);
		state.targetFormat[i] = attachments.renderTargetInternalFormat(i);
		state.blendState[i] = pipelineState.getBlendState(i, attachments, fragmentContainsKill);
	}

	state.multiSampleCount = static_cast<unsigned int>(pipelineState.getSampleCount());
	state.multiSampleMask = pipelineState.getMultiSampleMask();
	state.enableMultiSampling = (state.multiSampleCount > 1) &&
	                            !(pipelineState.isDrawLine(true) && (pipelineState.getLineRasterizationMode() == VK_LINE_RASTERIZATION_MODE_BRESENHAM_EXT));

	if(state.enableMultiSampling && fragmentShader)
	{
		state.centroid = fragmentShader->getModes().NeedsCentroid;
	}

	state.frontFace = pipelineState.getFrontFace();

	state.hash = state.computeHash();

	return state;
}

PixelProcessor::RoutineType PixelProcessor::routine(const State &state,
                                                    const vk::PipelineLayout *pipelineLayout,
                                                    const SpirvShader *pixelShader,
                                                    const vk::DescriptorSet::Bindings &descriptorSets)
{
	auto routine = routineCache->lookup(state);

	if(!routine)
	{
		QuadRasterizer *generator = new PixelProgram(state, pipelineLayout, pixelShader, descriptorSets);
		generator->generate();
		routine = (*generator)("PixelRoutine_%0.8X", state.shaderID);
		delete generator;

		routineCache->add(state, routine);
	}

	return routine;
}

}  // namespace sw
