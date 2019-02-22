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
			VkCompareOp stencilCompareMode;
			VkStencilOp stencilFailOperation;
			VkStencilOp stencilPassOperation;
			VkStencilOp stencilZFailOperation;
			bool noStencilMask;
			bool noStencilWriteMask;
			bool stencilWriteMasked;
			bool twoSidedStencil;
			VkCompareOp stencilCompareModeCCW;
			VkStencilOp stencilFailOperationCCW;
			VkStencilOp stencilPassOperationCCW;
			VkStencilOp stencilZFailOperationCCW;
			bool noStencilMaskCCW;
			bool noStencilWriteMaskCCW;
			bool stencilWriteMaskedCCW;

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

			VkLogicOp logicalOperation;

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

		void setRenderTarget(int index, Surface *renderTarget, unsigned int layer = 0);
		Surface *getRenderTarget(int index);
		void setDepthBuffer(Surface *depthBuffer, unsigned int layer = 0);
		void setStencilBuffer(Surface *stencilBuffer, unsigned int layer = 0);

		void setTexCoordIndex(unsigned int stage, int texCoordIndex);
		void setConstantColor(unsigned int stage, const Color<float> &constantColor);
		void setBumpmapMatrix(unsigned int stage, int element, float value);
		void setLuminanceScale(unsigned int stage, float value);
		void setLuminanceOffset(unsigned int stage, float value);

		void setTextureFilter(unsigned int sampler, FilterType textureFilter);
		void setMipmapFilter(unsigned int sampler, MipmapType mipmapFilter);
		void setGatherEnable(unsigned int sampler, bool enable);
		void setAddressingModeU(unsigned int sampler, AddressingMode addressingMode);
		void setAddressingModeV(unsigned int sampler, AddressingMode addressingMode);
		void setAddressingModeW(unsigned int sampler, AddressingMode addressingMode);
		void setReadSRGB(unsigned int sampler, bool sRGB);
		void setMipmapLOD(unsigned int sampler, float bias);
		void setBorderColor(unsigned int sampler, const Color<float> &borderColor);
		void setMaxAnisotropy(unsigned int sampler, float maxAnisotropy);
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

		void setWriteSRGB(bool sRGB);
		void setDepthBufferEnable(bool depthBufferEnable);
		void setDepthCompare(VkCompareOp depthCompareMode);
		void setDepthWriteEnable(bool depthWriteEnable);
		void setCullMode(CullMode cullMode, bool frontFacingCCW);
		void setColorWriteMask(int index, int rgbaMask);

		void setColorLogicOpEnabled(bool colorLogicOpEnabled);
		void setLogicalOperation(VkLogicOp logicalOperation);

		void setStencilEnable(bool stencilEnable);
		void setStencilCompare(VkCompareOp stencilCompareMode);
		void setStencilReference(int stencilReference);
		void setStencilMask(int stencilMask);
		void setStencilFailOperation(VkStencilOp stencilFailOperation);
		void setStencilPassOperation(VkStencilOp stencilPassOperation);
		void setStencilZFailOperation(VkStencilOp stencilZFailOperation);
		void setStencilWriteMask(int stencilWriteMask);
		void setTwoSidedStencil(bool enable);
		void setStencilCompareCCW(VkCompareOp stencilCompareMode);
		void setStencilReferenceCCW(int stencilReference);
		void setStencilMaskCCW(int stencilMask);
		void setStencilFailOperationCCW(VkStencilOp stencilFailOperation);
		void setStencilPassOperationCCW(VkStencilOp stencilPassOperation);
		void setStencilZFailOperationCCW(VkStencilOp stencilZFailOperation);
		void setStencilWriteMaskCCW(int stencilWriteMask);

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
		Stencil stencil;
		Stencil stencilCCW;
		Factor factor;

	private:
		Context *const context;

		RoutineCache<State> *routineCache;
	};
}

#endif   // sw_PixelProcessor_hpp
