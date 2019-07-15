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

#ifndef sw_PixelProcessor_hpp
#define sw_PixelProcessor_hpp

#include "Color.hpp"
#include "Context.hpp"
#include "Memset.hpp"
#include "RoutineCache.hpp"

namespace sw
{
	class PixelShader;
	class Rasterizer;
	struct Texture;
	struct DrawData;
	struct Primitive;

	class PixelProcessor
	{
	public:
		struct States : Memset<States>
		{
			States() : Memset(this, 0) {}

			uint32_t computeHash();

			uint64_t shaderID;

			VkCompareOp depthCompareMode;
			bool depthWriteEnable;
			bool quadLayoutDepthBuffer;

			bool stencilActive;
			VkStencilOpState frontStencil;
			VkStencilOpState backStencil;

			bool depthTestActive;
			bool occlusionEnabled;
			bool perspective;
			bool depthClamp;

			BlendState blendState[RENDERTARGETS];

			unsigned int colorWriteMask;
			VkFormat targetFormat[RENDERTARGETS];
			unsigned int multiSample;
			unsigned int multiSampleMask;
			bool alphaToCoverage;
			bool centroid;
			VkFrontFace frontFace;
			VkFormat depthFormat;
		};

		struct State : States
		{
			bool operator==(const State &state) const;

			int colorWriteActive(int index) const
			{
				return (colorWriteMask >> (index * 4)) & 0xF;
			}

			uint32_t hash;
		};

		struct Stencil
		{
			int64_t testMaskQ;
			int64_t referenceMaskedQ;
			int64_t referenceMaskedSignedQ;
			int64_t writeMaskQ;
			int64_t invWriteMaskQ;
			int64_t referenceQ;

			void set(int reference, int testMask, int writeMask)
			{
				referenceQ = replicate(reference);
				testMaskQ = replicate(testMask);
				writeMaskQ = replicate(writeMask);
				invWriteMaskQ = ~writeMaskQ;
				referenceMaskedQ = referenceQ & testMaskQ;
				referenceMaskedSignedQ = replicate(((reference & testMask) + 0x80) & 0xFF);
			}

			static int64_t replicate(int b)
			{
				int64_t w = b & 0xFF;

				return (w << 0) | (w << 8) | (w << 16) | (w << 24) | (w << 32) | (w << 40) | (w << 48) | (w << 56);
			}
		};

		struct Factor
		{
			word4 alphaReference4;

			word4 blendConstant4W[4];
			float4 blendConstant4F[4];
			word4 invBlendConstant4W[4];
			float4 invBlendConstant4F[4];
		};

	public:
		typedef void (*RoutinePointer)(const Primitive *primitive, int count, int cluster, int clusterCount, DrawData *draw);

		PixelProcessor();

		virtual ~PixelProcessor();

		void setBlendConstant(const Color<float> &blendConstant);

	protected:
		const State update(const Context* context) const;
		std::shared_ptr<Routine> routine(const State &state, vk::PipelineLayout const *pipelineLayout,
		                                 SpirvShader const *pixelShader, const vk::DescriptorSet::Bindings &descriptorSets);
		void setRoutineCacheSize(int routineCacheSize);

		// Other semi-constants
		Factor factor;

	private:
		RoutineCache<State> *routineCache;
	};
}

#endif   // sw_PixelProcessor_hpp
