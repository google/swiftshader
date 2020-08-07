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

const PixelProcessor::State PixelProcessor::update(const Context *context) const
{
	State state;

	state.numClipDistances = context->vertexShader->getNumOutputClipDistances();
	state.numCullDistances = context->vertexShader->getNumOutputCullDistances();

	if(context->pixelShader)
	{
		state.shaderID = context->pixelShader->getSerialID();
		state.pipelineLayoutIdentifier = context->pipelineLayout->identifier;
	}
	else
	{
		state.shaderID = 0;
		state.pipelineLayoutIdentifier = 0;
	}

	state.alphaToCoverage = context->alphaToCoverage;
	state.depthWriteEnable = context->depthWriteActive();

	if(context->stencilActive())
	{
		state.stencilActive = true;
		state.frontStencil = context->frontStencil;
		state.backStencil = context->backStencil;
	}

	if(context->depthBufferActive())
	{
		state.depthTestActive = true;
		state.depthCompareMode = context->depthCompareMode;
		state.depthFormat = context->depthBuffer->getFormat();

		// "For fixed-point depth buffers, fragment depth values are always limited to the range [0,1] by clamping after depth bias addition is performed.
		//  Unless the VK_EXT_depth_range_unrestricted extension is enabled, fragment depth values are clamped even when the depth buffer uses a floating-point representation."
		state.depthClamp = !state.depthFormat.isFloatFormat() || !context->depthRangeUnrestricted;
	}

	state.occlusionEnabled = context->occlusionEnabled;

	for(int i = 0; i < RENDERTARGETS; i++)
	{
		state.colorWriteMask |= context->colorWriteActive(i) << (4 * i);
		state.targetFormat[i] = context->renderTargetInternalFormat(i);
		state.blendState[i] = context->getBlendState(i);
	}

	state.multiSampleCount = static_cast<unsigned int>(context->sampleCount);
	state.multiSampleMask = context->multiSampleMask;
	state.enableMultiSampling = (state.multiSampleCount > 1) &&
	                            !(context->isDrawLine(true) && (context->lineRasterizationMode == VK_LINE_RASTERIZATION_MODE_BRESENHAM_EXT));

	if(state.enableMultiSampling && context->pixelShader)
	{
		state.centroid = context->pixelShader->getModes().NeedsCentroid;
	}

	state.frontFace = context->frontFace;

	state.hash = state.computeHash();

	return state;
}

PixelProcessor::RoutineType PixelProcessor::routine(const State &state,
                                                    vk::PipelineLayout const *pipelineLayout,
                                                    SpirvShader const *pixelShader,
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
