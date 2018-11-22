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

			bool depthOverride                        : 1;   // TODO: Eliminate by querying shader.
			bool shaderContainsKill                   : 1;   // TODO: Eliminate by querying shader.

			VkCompareOp depthCompareMode              : BITS(VK_COMPARE_OP_END_RANGE);
			VkCompareOp alphaCompareMode              : BITS(VK_COMPARE_OP_END_RANGE);
			bool depthWriteEnable                     : 1;
			bool quadLayoutDepthBuffer                : 1;

			bool stencilActive                        : 1;
			VkCompareOp stencilCompareMode            : BITS(VK_COMPARE_OP_END_RANGE);
			VkStencilOp stencilFailOperation          : BITS(VK_STENCIL_OP_END_RANGE);
			VkStencilOp stencilPassOperation          : BITS(VK_STENCIL_OP_END_RANGE);
			VkStencilOp stencilZFailOperation         : BITS(VK_STENCIL_OP_END_RANGE);
			bool noStencilMask                        : 1;
			bool noStencilWriteMask                   : 1;
			bool stencilWriteMasked                   : 1;
			bool twoSidedStencil                      : 1;
			VkCompareOp stencilCompareModeCCW         : BITS(VK_COMPARE_OP_END_RANGE);
			VkStencilOp stencilFailOperationCCW       : BITS(VK_STENCIL_OP_END_RANGE);
			VkStencilOp stencilPassOperationCCW       : BITS(VK_STENCIL_OP_END_RANGE);
			VkStencilOp stencilZFailOperationCCW      : BITS(VK_STENCIL_OP_END_RANGE);
			bool noStencilMaskCCW                     : 1;
			bool noStencilWriteMaskCCW                : 1;
			bool stencilWriteMaskedCCW                : 1;

			bool depthTestActive                      : 1;
			bool occlusionEnabled                     : 1;
			bool perspective                          : 1;
			bool depthClamp                           : 1;

			bool alphaBlendActive                     : 1;
			VkBlendFactor sourceBlendFactor           : BITS(VK_BLEND_FACTOR_END_RANGE);
			VkBlendFactor destBlendFactor             : BITS(VK_BLEND_FACTOR_END_RANGE);
			VkBlendOp blendOperation                  : BITS(VK_BLEND_OP_BLUE_EXT);
			VkBlendFactor sourceBlendFactorAlpha      : BITS(VK_BLEND_FACTOR_END_RANGE);
			VkBlendFactor destBlendFactorAlpha        : BITS(VK_BLEND_FACTOR_END_RANGE);
			VkBlendOp blendOperationAlpha             : BITS(VK_BLEND_OP_BLUE_EXT);

			unsigned int colorWriteMask                       : RENDERTARGETS * 4;   // Four component bit masks
			VkFormat targetFormat[RENDERTARGETS];
			bool writeSRGB                                    : 1;
			unsigned int multiSample                          : 3;
			unsigned int multiSampleMask                      : 4;
			TransparencyAntialiasing transparencyAntialiasing : BITS(TRANSPARENCY_LAST);
			bool centroid                                     : 1;
			bool frontFaceCCW                                 : 1;

			VkLogicOp logicalOperation : BITS(VK_LOGIC_OP_END_RANGE);

			Sampler::State sampler[TEXTURE_IMAGE_UNITS];

			struct Interpolant
			{
				unsigned char component : 4;
				unsigned char flat : 4;
				unsigned char project : 2;
				bool centroid : 1;
			};

			union
			{
				struct
				{
					Interpolant color[2];
					Interpolant texture[8];
					Interpolant fog;
				};

				Interpolant interpolant[MAX_FRAGMENT_INPUTS];
			};
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
				return (alphaCompareMode != VK_COMPARE_OP_ALWAYS) || (transparencyAntialiasing != TRANSPARENCY_NONE);
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

		void setFloatConstant(unsigned int index, const float value[4]);
		void setIntegerConstant(unsigned int index, const int value[4]);
		void setBooleanConstant(unsigned int index, int boolean);

		void setUniformBuffer(int index, sw::Resource* buffer, int offset);
		void lockUniformBuffers(byte** u, sw::Resource* uniformBuffers[]);

		void setRenderTarget(int index, Surface *renderTarget, unsigned int layer = 0);
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
		void setAlphaCompare(VkCompareOp alphaCompareMode);
		void setDepthWriteEnable(bool depthWriteEnable);
		void setAlphaTestEnable(bool alphaTestEnable);
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

		void setAlphaReference(float alphaReference);

		void setPerspectiveCorrection(bool perspectiveCorrection);

		void setOcclusionEnabled(bool enable);

	protected:
		const State update() const;
		Routine *routine(const State &state);
		void setRoutineCacheSize(int routineCacheSize);

		// Shader constants
		float4 c[FRAGMENT_UNIFORM_VECTORS];
		int4 i[16];
		bool b[16];

		// Other semi-constants
		Stencil stencil;
		Stencil stencilCCW;
		Factor factor;

	private:
		struct UniformBufferInfo
		{
			UniformBufferInfo();

			Resource* buffer;
			int offset;
		};
		UniformBufferInfo uniformBufferInfo[MAX_UNIFORM_BUFFER_BINDINGS];

		void setFogRanges(float start, float end);

		Context *const context;

		RoutineCache<State> *routineCache;
	};
}

#endif   // sw_PixelProcessor_hpp
