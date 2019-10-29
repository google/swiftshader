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
#include "Pipeline/PixelProgram.hpp"
#include "Pipeline/Constants.hpp"
#include "Vulkan/VkDebug.hpp"
#include "Vulkan/VkImageView.hpp"

#include <cstring>

namespace sw
{
	uint32_t PixelProcessor::States::computeHash()
	{
		uint32_t *state = reinterpret_cast<uint32_t*>(this);
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

		static_assert(is_memcmparable<State>::value, "Cannot memcmp State");
		return memcmp(static_cast<const States*>(this), static_cast<const States*>(&state), sizeof(States)) == 0;
	}

	PixelProcessor::PixelProcessor()
	{
		routineCache = nullptr;
		setRoutineCacheSize(1024);
	}

	PixelProcessor::~PixelProcessor()
	{
		delete routineCache;
		routineCache = nullptr;
	}

	void PixelProcessor::setBlendConstant(const Color<float> &blendConstant)
	{
		// TODO(b/140935644): Compact into generic function, cheack if clamp is required
		factor.blendConstant4W[0][0] =
		factor.blendConstant4W[0][1] =
		factor.blendConstant4W[0][2] =
		factor.blendConstant4W[0][3] = static_cast<uint16_t>(iround(65535.0f * blendConstant.r));

		factor.blendConstant4W[1][0] =
		factor.blendConstant4W[1][1] =
		factor.blendConstant4W[1][2] =
		factor.blendConstant4W[1][3] = static_cast<uint16_t>(iround(65535.0f * blendConstant.g));

		factor.blendConstant4W[2][0] =
		factor.blendConstant4W[2][1] =
		factor.blendConstant4W[2][2] =
		factor.blendConstant4W[2][3] = static_cast<uint16_t>(iround(65535.0f * blendConstant.b));

		factor.blendConstant4W[3][0] =
		factor.blendConstant4W[3][1] =
		factor.blendConstant4W[3][2] =
		factor.blendConstant4W[3][3] = static_cast<uint16_t>(iround(65535.0f * blendConstant.a));

		factor.invBlendConstant4W[0][0] =
		factor.invBlendConstant4W[0][1] =
		factor.invBlendConstant4W[0][2] =
		factor.invBlendConstant4W[0][3] = 0xFFFFu - factor.blendConstant4W[0][0];

		factor.invBlendConstant4W[1][0] =
		factor.invBlendConstant4W[1][1] =
		factor.invBlendConstant4W[1][2] =
		factor.invBlendConstant4W[1][3] = 0xFFFFu - factor.blendConstant4W[1][0];

		factor.invBlendConstant4W[2][0] =
		factor.invBlendConstant4W[2][1] =
		factor.invBlendConstant4W[2][2] =
		factor.invBlendConstant4W[2][3] = 0xFFFFu - factor.blendConstant4W[2][0];

		factor.invBlendConstant4W[3][0] =
		factor.invBlendConstant4W[3][1] =
		factor.invBlendConstant4W[3][2] =
		factor.invBlendConstant4W[3][3] = 0xFFFFu - factor.blendConstant4W[3][0];

		factor.blendConstant4F[0][0] =
		factor.blendConstant4F[0][1] =
		factor.blendConstant4F[0][2] =
		factor.blendConstant4F[0][3] = blendConstant.r;

		factor.blendConstant4F[1][0] =
		factor.blendConstant4F[1][1] =
		factor.blendConstant4F[1][2] =
		factor.blendConstant4F[1][3] = blendConstant.g;

		factor.blendConstant4F[2][0] =
		factor.blendConstant4F[2][1] =
		factor.blendConstant4F[2][2] =
		factor.blendConstant4F[2][3] = blendConstant.b;

		factor.blendConstant4F[3][0] =
		factor.blendConstant4F[3][1] =
		factor.blendConstant4F[3][2] =
		factor.blendConstant4F[3][3] = blendConstant.a;

		factor.invBlendConstant4F[0][0] =
		factor.invBlendConstant4F[0][1] =
		factor.invBlendConstant4F[0][2] =
		factor.invBlendConstant4F[0][3] = 1 - blendConstant.r;

		factor.invBlendConstant4F[1][0] =
		factor.invBlendConstant4F[1][1] =
		factor.invBlendConstant4F[1][2] =
		factor.invBlendConstant4F[1][3] = 1 - blendConstant.g;

		factor.invBlendConstant4F[2][0] =
		factor.invBlendConstant4F[2][1] =
		factor.invBlendConstant4F[2][2] =
		factor.invBlendConstant4F[2][3] = 1 - blendConstant.b;

		factor.invBlendConstant4F[3][0] =
		factor.invBlendConstant4F[3][1] =
		factor.invBlendConstant4F[3][2] =
		factor.invBlendConstant4F[3][3] = 1 - blendConstant.a;
	}

	void PixelProcessor::setRoutineCacheSize(int cacheSize)
	{
		delete routineCache;
		routineCache = new RoutineCacheType(clamp(cacheSize, 1, 65536));
	}

	const PixelProcessor::State PixelProcessor::update(const Context* context) const
	{
		State state;

		if(context->pixelShader)
		{
			state.shaderID = context->pixelShader->getSerialID();
		}
		else
		{
			state.shaderID = 0;
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
			state.quadLayoutDepthBuffer = context->depthBuffer->getFormat().hasQuadLayout();
			state.depthFormat = context->depthBuffer->getFormat();
		}

		state.occlusionEnabled = context->occlusionEnabled;
		state.depthClamp = (context->depthBias != 0.0f) || (context->slopeDepthBias != 0.0f);

		for(int i = 0; i < RENDERTARGETS; i++)
		{
			state.colorWriteMask |= context->colorWriteActive(i) << (4 * i);
			state.targetFormat[i] = context->renderTargetInternalFormat(i);
			state.blendState[i] = context->getBlendState(i);
		}

		state.multiSample = static_cast<unsigned int>(context->sampleCount);
		state.multiSampleMask = context->multiSampleMask;

		if(state.multiSample > 1 && context->pixelShader)
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
		auto routine = routineCache->query(state);

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
}
