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

#include "VertexProcessor.hpp"

#include "Pipeline/VertexProgram.hpp"
#include "Pipeline/Constants.hpp"
#include "System/Math.hpp"
#include "Vulkan/VkDebug.hpp"

#include <string.h>

namespace sw
{
	bool precacheVertex = false;

	void VertexCache::clear()
	{
		for(int i = 0; i < 16; i++)
		{
			tag[i] = 0x80000000;
		}
	}

	unsigned int VertexProcessor::States::computeHash()
	{
		unsigned int *state = (unsigned int*)this;
		unsigned int hash = 0;

		for(unsigned int i = 0; i < sizeof(States) / 4; i++)
		{
			hash ^= state[i];
		}

		return hash;
	}

	VertexProcessor::State::State()
	{
		memset(this, 0, sizeof(State));
	}

	bool VertexProcessor::State::operator==(const State &state) const
	{
		if(hash != state.hash)
		{
			return false;
		}

		return memcmp(static_cast<const States*>(this), static_cast<const States*>(&state), sizeof(States)) == 0;
	}

	VertexProcessor::VertexProcessor(Context *context) : context(context)
	{
		routineCache = nullptr;
		setRoutineCacheSize(1024);
	}

	VertexProcessor::~VertexProcessor()
	{
		delete routineCache;
		routineCache = nullptr;
	}

	void VertexProcessor::setInputStream(int index, const Stream &stream)
	{
		context->input[index] = stream;
	}

	void VertexProcessor::resetInputStreams()
	{
		for(int i = 0; i < MAX_VERTEX_INPUTS; i++)
		{
			context->input[i].defaults();
		}
	}

	void VertexProcessor::setInstanceID(int instanceID)
	{
		context->instanceID = instanceID;
	}

	void VertexProcessor::setTextureFilter(unsigned int sampler, FilterType textureFilter)
	{
		if(sampler < VERTEX_TEXTURE_IMAGE_UNITS)
		{
			context->sampler[TEXTURE_IMAGE_UNITS + sampler].setTextureFilter(textureFilter);
		}
		else ASSERT(false);
	}

	void VertexProcessor::setMipmapFilter(unsigned int sampler, MipmapType mipmapFilter)
	{
		if(sampler < VERTEX_TEXTURE_IMAGE_UNITS)
		{
			context->sampler[TEXTURE_IMAGE_UNITS + sampler].setMipmapFilter(mipmapFilter);
		}
		else ASSERT(false);
	}

	void VertexProcessor::setGatherEnable(unsigned int sampler, bool enable)
	{
		if(sampler < VERTEX_TEXTURE_IMAGE_UNITS)
		{
			context->sampler[TEXTURE_IMAGE_UNITS + sampler].setGatherEnable(enable);
		}
		else ASSERT(false);
	}

	void VertexProcessor::setAddressingModeU(unsigned int sampler, AddressingMode addressMode)
	{
		if(sampler < VERTEX_TEXTURE_IMAGE_UNITS)
		{
			context->sampler[TEXTURE_IMAGE_UNITS + sampler].setAddressingModeU(addressMode);
		}
		else ASSERT(false);
	}

	void VertexProcessor::setAddressingModeV(unsigned int sampler, AddressingMode addressMode)
	{
		if(sampler < VERTEX_TEXTURE_IMAGE_UNITS)
		{
			context->sampler[TEXTURE_IMAGE_UNITS + sampler].setAddressingModeV(addressMode);
		}
		else ASSERT(false);
	}

	void VertexProcessor::setAddressingModeW(unsigned int sampler, AddressingMode addressMode)
	{
		if(sampler < VERTEX_TEXTURE_IMAGE_UNITS)
		{
			context->sampler[TEXTURE_IMAGE_UNITS + sampler].setAddressingModeW(addressMode);
		}
		else ASSERT(false);
	}

	void VertexProcessor::setReadSRGB(unsigned int sampler, bool sRGB)
	{
		if(sampler < VERTEX_TEXTURE_IMAGE_UNITS)
		{
			context->sampler[TEXTURE_IMAGE_UNITS + sampler].setReadSRGB(sRGB);
		}
		else ASSERT(false);
	}

	void VertexProcessor::setMipmapLOD(unsigned int sampler, float bias)
	{
		if(sampler < VERTEX_TEXTURE_IMAGE_UNITS)
		{
			context->sampler[TEXTURE_IMAGE_UNITS + sampler].setMipmapLOD(bias);
		}
		else ASSERT(false);
	}

	void VertexProcessor::setBorderColor(unsigned int sampler, const Color<float> &borderColor)
	{
		if(sampler < VERTEX_TEXTURE_IMAGE_UNITS)
		{
			context->sampler[TEXTURE_IMAGE_UNITS + sampler].setBorderColor(borderColor);
		}
		else ASSERT(false);
	}

	void VertexProcessor::setMaxAnisotropy(unsigned int sampler, float maxAnisotropy)
	{
		if(sampler < VERTEX_TEXTURE_IMAGE_UNITS)
		{
			context->sampler[TEXTURE_IMAGE_UNITS + sampler].setMaxAnisotropy(maxAnisotropy);
		}
		else ASSERT(false);
	}

	void VertexProcessor::setHighPrecisionFiltering(unsigned int sampler, bool highPrecisionFiltering)
	{
		if(sampler < TEXTURE_IMAGE_UNITS)
		{
			context->sampler[sampler].setHighPrecisionFiltering(highPrecisionFiltering);
		}
		else ASSERT(false);
	}

	void VertexProcessor::setSwizzleR(unsigned int sampler, SwizzleType swizzleR)
	{
		if(sampler < VERTEX_TEXTURE_IMAGE_UNITS)
		{
			context->sampler[TEXTURE_IMAGE_UNITS + sampler].setSwizzleR(swizzleR);
		}
		else ASSERT(false);
	}

	void VertexProcessor::setSwizzleG(unsigned int sampler, SwizzleType swizzleG)
	{
		if(sampler < VERTEX_TEXTURE_IMAGE_UNITS)
		{
			context->sampler[TEXTURE_IMAGE_UNITS + sampler].setSwizzleG(swizzleG);
		}
		else ASSERT(false);
	}

	void VertexProcessor::setSwizzleB(unsigned int sampler, SwizzleType swizzleB)
	{
		if(sampler < VERTEX_TEXTURE_IMAGE_UNITS)
		{
			context->sampler[TEXTURE_IMAGE_UNITS + sampler].setSwizzleB(swizzleB);
		}
		else ASSERT(false);
	}

	void VertexProcessor::setSwizzleA(unsigned int sampler, SwizzleType swizzleA)
	{
		if(sampler < VERTEX_TEXTURE_IMAGE_UNITS)
		{
			context->sampler[TEXTURE_IMAGE_UNITS + sampler].setSwizzleA(swizzleA);
		}
		else ASSERT(false);
	}

	void VertexProcessor::setCompareFunc(unsigned int sampler, CompareFunc compFunc)
	{
		if(sampler < VERTEX_TEXTURE_IMAGE_UNITS)
		{
			context->sampler[TEXTURE_IMAGE_UNITS + sampler].setCompareFunc(compFunc);
		}
		else ASSERT(false);
	}

	void VertexProcessor::setBaseLevel(unsigned int sampler, int baseLevel)
	{
		if(sampler < VERTEX_TEXTURE_IMAGE_UNITS)
		{
			context->sampler[TEXTURE_IMAGE_UNITS + sampler].setBaseLevel(baseLevel);
		}
		else ASSERT(false);
	}

	void VertexProcessor::setMaxLevel(unsigned int sampler, int maxLevel)
	{
		if(sampler < VERTEX_TEXTURE_IMAGE_UNITS)
		{
			context->sampler[TEXTURE_IMAGE_UNITS + sampler].setMaxLevel(maxLevel);
		}
		else ASSERT(false);
	}

	void VertexProcessor::setMinLod(unsigned int sampler, float minLod)
	{
		if(sampler < VERTEX_TEXTURE_IMAGE_UNITS)
		{
			context->sampler[TEXTURE_IMAGE_UNITS + sampler].setMinLod(minLod);
		}
		else ASSERT(false);
	}

	void VertexProcessor::setMaxLod(unsigned int sampler, float maxLod)
	{
		if(sampler < VERTEX_TEXTURE_IMAGE_UNITS)
		{
			context->sampler[TEXTURE_IMAGE_UNITS + sampler].setMaxLod(maxLod);
		}
		else ASSERT(false);
	}

	void VertexProcessor::setPointSizeMin(float pointSizeMin)
	{
		this->pointSizeMin = pointSizeMin;
	}

	void VertexProcessor::setPointSizeMax(float pointSizeMax)
	{
		this->pointSizeMax = pointSizeMax;
	}

	void VertexProcessor::setRoutineCacheSize(int cacheSize)
	{
		delete routineCache;
		routineCache = new RoutineCache<State>(clamp(cacheSize, 1, 65536), precacheVertex ? "sw-vertex" : 0);
	}

	const VertexProcessor::State VertexProcessor::update(DrawType drawType)
	{
		State state;

		state.shaderID = context->vertexShader->getSerialID();

		// Note: Quads aren't handled for verticesPerPrimitive, but verticesPerPrimitive is used for transform feedback,
		//       which is an OpenGL ES 3.0 feature, and OpenGL ES 3.0 doesn't support quads as a primitive type.
		DrawType type = static_cast<DrawType>(static_cast<unsigned int>(drawType) & 0xF);
		state.verticesPerPrimitive = 1 + (type >= DRAW_LINELIST) + (type >= DRAW_TRIANGLELIST);

		for(int i = 0; i < MAX_VERTEX_INPUTS; i++)
		{
			state.input[i].type = context->input[i].type;
			state.input[i].count = context->input[i].count;
			state.input[i].normalized = context->input[i].normalized;
			// TODO: get rid of attribType -- just keep the VK format all the way through, this fully determines
			// how to handle the attribute.
			state.input[i].attribType = context->vertexShader->inputs[i*4].Type;
		}

		state.hash = state.computeHash();

		return state;
	}

	Routine *VertexProcessor::routine(const State &state)
	{
		Routine *routine = routineCache->query(state);

		if(!routine)   // Create one
		{
			VertexRoutine *generator = new VertexProgram(state, context->vertexShader);
			generator->generate();
			routine = (*generator)("VertexRoutine_%0.8X", state.shaderID);
			delete generator;

			routineCache->add(state, routine);
		}

		return routine;
	}
}
