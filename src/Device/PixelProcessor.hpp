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

#include "Context.hpp"
#include "RoutineCache.hpp"

namespace sw
{
	class PixelShader;
	class Rasterizer;
	struct Texture;
	struct DrawData;

	class PixelProcessor
	{
	public:
		struct States
		{
			unsigned int computeHash();

			int shaderID;

			VkCompareOp depthCompareMode;
			bool depthWriteEnable;
			bool quadLayoutDepthBuffer;

			bool stencilActive;
			bool twoSidedStencil;
			VkStencilOpState frontStencil;
			VkStencilOpState backStencil;

			bool depthTestActive;
			bool occlusionEnabled;
			bool perspective;
			bool depthClamp;

			bool alphaBlendActive;
			VkBlendFactor sourceBlendFactor;
			VkBlendFactor destBlendFactor;
			VkBlendOp blendOperation;
			VkBlendFactor sourceBlendFactorAlpha;
			VkBlendFactor destBlendFactorAlpha;
			VkBlendOp blendOperationAlpha;

			unsigned int colorWriteMask;
			VkFormat targetFormat[RENDERTARGETS];
			bool writeSRGB;
			unsigned int multiSample;
			unsigned int multiSampleMask;
			TransparencyAntialiasing transparencyAntialiasing;
			bool centroid;
			bool frontFaceCCW;
			VkFormat depthFormat;

			Sampler::State sampler[TEXTURE_IMAGE_UNITS];
		};

		struct State : States
		{
			State();

			bool operator==(const State &state) const;

			int colorWriteActive(int index) const
			{
				return (colorWriteMask >> (index * 4)) & 0xF;
			}

			bool alphaTestActive() const
			{
				return transparencyAntialiasing != TRANSPARENCY_NONE;
			}

			unsigned int hash;
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
		typedef void (*RoutinePointer)(const Primitive *primitive, int count, int thread, DrawData *draw);

		PixelProcessor(Context *context);

		virtual ~PixelProcessor();

		void setRenderTarget(int index, vk::ImageView *renderTarget, unsigned int layer = 0);
		void setDepthBuffer(vk::ImageView *depthBuffer, unsigned int layer = 0);
		void setStencilBuffer(vk::ImageView *stencilBuffer, unsigned int layer = 0);

		void setWriteSRGB(bool sRGB);
		void setDepthBufferEnable(bool depthBufferEnable);
		void setDepthCompare(VkCompareOp depthCompareMode);
		void setDepthWriteEnable(bool depthWriteEnable);
		void setCullMode(CullMode cullMode, bool frontFacingCCW);
		void setColorWriteMask(int index, int rgbaMask);

		void setColorLogicOpEnabled(bool colorLogicOpEnabled);
		void setLogicalOperation(VkLogicOp logicalOperation);

		void setBlendConstant(const Color<float> &blendConstant);

		void setAlphaBlendEnable(bool alphaBlendEnable);
		void setSourceBlendFactor(VkBlendFactor sourceBlendFactor);
		void setDestBlendFactor(VkBlendFactor destBlendFactor);
		void setBlendOperation(VkBlendOp blendOperation);

		void setSeparateAlphaBlendEnable(bool separateAlphaBlendEnable);
		void setSourceBlendFactorAlpha(VkBlendFactor sourceBlendFactorAlpha);
		void setDestBlendFactorAlpha(VkBlendFactor destBlendFactorAlpha);
		void setBlendOperationAlpha(VkBlendOp blendOperationAlpha);

		void setPerspectiveCorrection(bool perspectiveCorrection);

		void setOcclusionEnabled(bool enable);

	protected:
		const State update() const;
		Routine *routine(const State &state);
		void setRoutineCacheSize(int routineCacheSize);

		// Other semi-constants
		Factor factor;

	private:
		Context *const context;

		RoutineCache<State> *routineCache;
	};
}

#endif   // sw_PixelProcessor_hpp
