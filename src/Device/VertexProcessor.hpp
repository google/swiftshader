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

#ifndef sw_VertexProcessor_hpp
#define sw_VertexProcessor_hpp

#include "Matrix.hpp"
#include "Context.hpp"
#include "RoutineCache.hpp"
#include "Pipeline/VertexShader.hpp"
#include "Pipeline/SpirvShader.hpp"

namespace sw
{
	struct DrawData;

	struct VertexCache   // FIXME: Variable size
	{
		void clear();

		Vertex vertex[16][4];
		unsigned int tag[16];

		int drawCall;
	};

	struct VertexTask
	{
		unsigned int vertexCount;
		unsigned int primitiveStart;
		VertexCache vertexCache;
	};

	class VertexProcessor
	{
	public:
		struct States
		{
			unsigned int computeHash();

			uint64_t shaderID;

			bool fixedFunction             : 1;   // TODO: Eliminate by querying shader.
			bool textureSampling           : 1;   // TODO: Eliminate by querying shader.
			unsigned int positionRegister  : BITS(MAX_VERTEX_OUTPUTS);   // TODO: Eliminate by querying shader.
			unsigned int pointSizeRegister : BITS(MAX_VERTEX_OUTPUTS);   // TODO: Eliminate by querying shader.

			bool transformFeedbackQueryEnabled                : 1;
			uint64_t transformFeedbackEnabled                 : 64;
			unsigned char verticesPerPrimitive                : 2; // 1 (points), 2 (lines) or 3 (triangles)

			bool multiSampling  : 1;

			Sampler::State sampler[VERTEX_TEXTURE_IMAGE_UNITS];

			struct Input
			{
				operator bool() const   // Returns true if stream contains data
				{
					return count != 0;
				}

				StreamType type    : BITS(STREAMTYPE_LAST);
				unsigned int count : 3;
				bool normalized    : 1;
				unsigned int attribType : BITS(SpirvShader::ATTRIBTYPE_LAST);
			};

			struct Output
			{
				union
				{
					unsigned char write : 4;

					struct
					{
						unsigned char xWrite : 1;
						unsigned char yWrite : 1;
						unsigned char zWrite : 1;
						unsigned char wWrite : 1;
					};
				};

				union
				{
					unsigned char clamp : 4;

					struct
					{
						unsigned char xClamp : 1;
						unsigned char yClamp : 1;
						unsigned char zClamp : 1;
						unsigned char wClamp : 1;
					};
				};
			};

			Input input[MAX_VERTEX_INPUTS];
			Output output[MAX_VERTEX_OUTPUTS];
		};

		struct State : States
		{
			State();

			bool operator==(const State &state) const;

			unsigned int hash;
		};

		typedef void (*RoutinePointer)(Vertex *output, unsigned int *batch, VertexTask *vertexTask, DrawData *draw);

		VertexProcessor(Context *context);

		virtual ~VertexProcessor();

		void setInputStream(int index, const Stream &stream);
		void resetInputStreams();

		void setFloatConstant(unsigned int index, const float value[4]);
		void setIntegerConstant(unsigned int index, const int integer[4]);
		void setBooleanConstant(unsigned int index, int boolean);

		void setUniformBuffer(int index, sw::Resource* uniformBuffer, int offset);
		void lockUniformBuffers(byte** u, sw::Resource* uniformBuffers[]);

		void setTransformFeedbackBuffer(int index, sw::Resource* transformFeedbackBuffer, int offset, unsigned int reg, unsigned int row, unsigned int col, unsigned int stride);
		void lockTransformFeedbackBuffers(byte** t, unsigned int* v, unsigned int* r, unsigned int* c, unsigned int* s, sw::Resource* transformFeedbackBuffers[]);

		void setInstanceID(int instanceID);

		void setTextureFilter(unsigned int sampler, FilterType textureFilter);
		void setMipmapFilter(unsigned int sampler, MipmapType mipmapFilter);
		void setGatherEnable(unsigned int sampler, bool enable);
		void setAddressingModeU(unsigned int sampler, AddressingMode addressingMode);
		void setAddressingModeV(unsigned int sampler, AddressingMode addressingMode);
		void setAddressingModeW(unsigned int sampler, AddressingMode addressingMode);
		void setReadSRGB(unsigned int sampler, bool sRGB);
		void setMipmapLOD(unsigned int sampler, float bias);
		void setBorderColor(unsigned int sampler, const Color<float> &borderColor);
		void setMaxAnisotropy(unsigned int stage, float maxAnisotropy);
		void setHighPrecisionFiltering(unsigned int sampler, bool highPrecisionFiltering);
		void setSwizzleR(unsigned int sampler, SwizzleType swizzleR);
		void setSwizzleG(unsigned int sampler, SwizzleType swizzleG);
		void setSwizzleB(unsigned int sampler, SwizzleType swizzleB);
		void setSwizzleA(unsigned int sampler, SwizzleType swizzleA);
		void setCompareFunc(unsigned int sampler, CompareFunc compare);
		void setBaseLevel(unsigned int sampler, int baseLevel);
		void setMaxLevel(unsigned int sampler, int maxLevel);
		void setMinLod(unsigned int sampler, float minLod);
		void setMaxLod(unsigned int sampler, float maxLod);

		void setPointSizeMin(float pointSizeMin);
		void setPointSizeMax(float pointSizeMax);

		void setTransformFeedbackQueryEnabled(bool enable);
		void enableTransformFeedback(uint64_t enable);

	protected:
		const State update(DrawType drawType);
		Routine *routine(const State &state);

		void setRoutineCacheSize(int cacheSize);

		// Shader constants
		float4 c[VERTEX_UNIFORM_VECTORS + 1];   // One extra for indices out of range, c[VERTEX_UNIFORM_VECTORS] = {0, 0, 0, 0}
		int4 i[16];
		bool b[16];

		float pointSizeMin;
		float pointSizeMax;

	private:
		struct UniformBufferInfo
		{
			UniformBufferInfo();

			Resource* buffer;
			int offset;
		};
		UniformBufferInfo uniformBufferInfo[MAX_UNIFORM_BUFFER_BINDINGS];

		struct TransformFeedbackInfo
		{
			TransformFeedbackInfo();

			Resource* buffer;
			unsigned int offset;
			unsigned int reg;
			unsigned int row;
			unsigned int col;
			unsigned int stride;
		};
		TransformFeedbackInfo transformFeedbackInfo[MAX_TRANSFORM_FEEDBACK_INTERLEAVED_COMPONENTS];

		Context *const context;

		RoutineCache<State> *routineCache;
	};
}

#endif   // sw_VertexProcessor_hpp
