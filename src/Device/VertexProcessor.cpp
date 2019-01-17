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
#include "Pipeline/VertexShader.hpp"
#include "Pipeline/PixelShader.hpp"
#include "Pipeline/Constants.hpp"
#include "System/Math.hpp"
#include "System/Debug.hpp"

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

	VertexProcessor::TransformFeedbackInfo::TransformFeedbackInfo()
	{
		buffer = nullptr;
		offset = 0;
		reg = 0;
		row = 0;
		col = 0;
		stride = 0;
	}

	VertexProcessor::UniformBufferInfo::UniformBufferInfo()
	{
		buffer = nullptr;
		offset = 0;
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

	void VertexProcessor::setFloatConstant(unsigned int index, const float value[4])
	{
		if(index < VERTEX_UNIFORM_VECTORS)
		{
			c[index][0] = value[0];
			c[index][1] = value[1];
			c[index][2] = value[2];
			c[index][3] = value[3];
		}
		else ASSERT(false);
	}

	void VertexProcessor::setIntegerConstant(unsigned int index, const int integer[4])
	{
		if(index < 16)
		{
			i[index][0] = integer[0];
			i[index][1] = integer[1];
			i[index][2] = integer[2];
			i[index][3] = integer[3];
		}
		else ASSERT(false);
	}

	void VertexProcessor::setBooleanConstant(unsigned int index, int boolean)
	{
		if(index < 16)
		{
			b[index] = boolean != 0;
		}
		else ASSERT(false);
	}

	void VertexProcessor::setUniformBuffer(int index, sw::Resource* buffer, int offset)
	{
		uniformBufferInfo[index].buffer = buffer;
		uniformBufferInfo[index].offset = offset;
	}

	void VertexProcessor::lockUniformBuffers(byte** u, sw::Resource* uniformBuffers[])
	{
		for(int i = 0; i < MAX_UNIFORM_BUFFER_BINDINGS; ++i)
		{
			u[i] = uniformBufferInfo[i].buffer ? static_cast<byte*>(uniformBufferInfo[i].buffer->lock(PUBLIC, PRIVATE)) + uniformBufferInfo[i].offset : nullptr;
			uniformBuffers[i] = uniformBufferInfo[i].buffer;
		}
	}

	void VertexProcessor::setTransformFeedbackBuffer(int index, sw::Resource* buffer, int offset, unsigned int reg, unsigned int row, unsigned int col, unsigned int stride)
	{
		transformFeedbackInfo[index].buffer = buffer;
		transformFeedbackInfo[index].offset = offset;
		transformFeedbackInfo[index].reg = reg;
		transformFeedbackInfo[index].row = row;
		transformFeedbackInfo[index].col = col;
		transformFeedbackInfo[index].stride = stride;
	}

	void VertexProcessor::lockTransformFeedbackBuffers(byte** t, unsigned int* v, unsigned int* r, unsigned int* c, unsigned int* s, sw::Resource* transformFeedbackBuffers[])
	{
		for(int i = 0; i < MAX_TRANSFORM_FEEDBACK_INTERLEAVED_COMPONENTS; ++i)
		{
			t[i] = transformFeedbackInfo[i].buffer ? static_cast<byte*>(transformFeedbackInfo[i].buffer->lock(PUBLIC, PRIVATE)) + transformFeedbackInfo[i].offset : nullptr;
			transformFeedbackBuffers[i] = transformFeedbackInfo[i].buffer;
			v[i] = transformFeedbackInfo[i].reg;
			r[i] = transformFeedbackInfo[i].row;
			c[i] = transformFeedbackInfo[i].col;
			s[i] = transformFeedbackInfo[i].stride;
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

	void VertexProcessor::setTransformFeedbackQueryEnabled(bool enable)
	{
		context->transformFeedbackQueryEnabled = enable;
	}

	void VertexProcessor::enableTransformFeedback(uint64_t enable)
	{
		context->transformFeedbackEnabled = enable;
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

		state.fixedFunction = !context->vertexShader && context->pixelShaderModel() < 0x0300;
		state.textureSampling = context->vertexShader ? context->vertexShader->containsTextureSampling() : false;
		state.positionRegister = context->vertexShader ? context->vertexShader->getPositionRegister() : Pos;
		state.pointSizeRegister = context->vertexShader ? context->vertexShader->getPointSizeRegister() : Pts;

		state.multiSampling = context->getMultiSampleCount() > 1;

		state.transformFeedbackQueryEnabled = context->transformFeedbackQueryEnabled;
		state.transformFeedbackEnabled = context->transformFeedbackEnabled;

		// Note: Quads aren't handled for verticesPerPrimitive, but verticesPerPrimitive is used for transform feedback,
		//       which is an OpenGL ES 3.0 feature, and OpenGL ES 3.0 doesn't support quads as a primitive type.
		DrawType type = static_cast<DrawType>(static_cast<unsigned int>(drawType) & 0xF);
		state.verticesPerPrimitive = 1 + (type >= DRAW_LINELIST) + (type >= DRAW_TRIANGLELIST);

		for(int i = 0; i < MAX_VERTEX_INPUTS; i++)
		{
			state.input[i].type = context->input[i].type;
			state.input[i].count = context->input[i].count;
			state.input[i].normalized = context->input[i].normalized;
			state.input[i].attribType = context->vertexShader ? context->vertexShader->getAttribType(i) : SpirvShader::ATTRIBTYPE_FLOAT;
		}

		for(unsigned int i = 0; i < VERTEX_TEXTURE_IMAGE_UNITS; i++)
		{
			if(context->vertexShader->usesSampler(i))
			{
				state.sampler[i] = context->sampler[TEXTURE_IMAGE_UNITS + i].samplerState();
			}
		}

		if(context->vertexShader)   // FIXME: Also when pre-transformed?
		{
			for(int i = 0; i < MAX_VERTEX_OUTPUTS; i++)
			{
				state.output[i].xWrite = context->vertexShader->getOutput(i, 0).active();
				state.output[i].yWrite = context->vertexShader->getOutput(i, 1).active();
				state.output[i].zWrite = context->vertexShader->getOutput(i, 2).active();
				state.output[i].wWrite = context->vertexShader->getOutput(i, 3).active();
			}
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
			routine = (*generator)(L"VertexRoutine_%0.8X", state.shaderID);
			delete generator;

			routineCache->add(state, routine);
		}

		return routine;
	}
}
