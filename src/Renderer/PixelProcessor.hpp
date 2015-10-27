// SwiftShader Software Renderer
//
// Copyright(c) 2005-2012 TransGaming Inc.
//
// All rights reserved. No part of this software may be copied, distributed, transmitted,
// transcribed, stored in a retrieval system, translated into any human or computer
// language by any means, or disclosed to third parties without the explicit written
// agreement of TransGaming Inc. Without such an agreement, no rights or licenses, express
// or implied, including but not limited to any patent rights, are granted to you.
//

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

			bool depthOverride                        : 1;
			bool shaderContainsKill                   : 1;

			DepthCompareMode depthCompareMode         : BITS(DEPTH_LAST);
			AlphaCompareMode alphaCompareMode         : BITS(ALPHA_LAST);
			bool depthWriteEnable                     : 1;
			bool quadLayoutDepthBuffer                : 1;

			bool stencilActive                        : 1;
			StencilCompareMode stencilCompareMode     : BITS(STENCIL_LAST);
			StencilOperation stencilFailOperation     : BITS(OPERATION_LAST);
			StencilOperation stencilPassOperation     : BITS(OPERATION_LAST);
			StencilOperation stencilZFailOperation    : BITS(OPERATION_LAST);
			bool noStencilMask                        : 1;
			bool noStencilWriteMask                   : 1;
			bool stencilWriteMasked                   : 1;
			bool twoSidedStencil                      : 1;
			StencilCompareMode stencilCompareModeCCW  : BITS(STENCIL_LAST);
			StencilOperation stencilFailOperationCCW  : BITS(OPERATION_LAST);
			StencilOperation stencilPassOperationCCW  : BITS(OPERATION_LAST);
			StencilOperation stencilZFailOperationCCW : BITS(OPERATION_LAST);
			bool noStencilMaskCCW                     : 1;
			bool noStencilWriteMaskCCW                : 1;
			bool stencilWriteMaskedCCW                : 1;

			bool depthTestActive                      : 1;
			bool fogActive                            : 1;
			FogMode pixelFogMode                      : BITS(FOG_LAST);
			bool specularAdd                          : 1;
			bool occlusionEnabled                     : 1;
			bool wBasedFog                            : 1;
			bool perspective                          : 1;

			bool alphaBlendActive                     : 1;
			BlendFactor sourceBlendFactor             : BITS(BLEND_LAST);
			BlendFactor destBlendFactor               : BITS(BLEND_LAST);
			BlendOperation blendOperation             : BITS(BLENDOP_LAST);
			BlendFactor sourceBlendFactorAlpha        : BITS(BLEND_LAST);
			BlendFactor destBlendFactorAlpha          : BITS(BLEND_LAST);
			BlendOperation blendOperationAlpha        : BITS(BLENDOP_LAST);
			
			unsigned int colorWriteMask                       : 16;   // (four times four component bit mask)
			Format targetFormat[RENDERTARGETS];
			bool writeSRGB                                    : 1;
			unsigned int multiSample                          : 3;
			unsigned int multiSampleMask                      : 4;
			TransparencyAntialiasing transparencyAntialiasing : BITS(TRANSPARENCY_LAST);
			bool centroid                                     : 1;

			LogicalOperation logicalOperation : BITS(LOGICALOP_LAST);

			Sampler::State sampler[TEXTURE_IMAGE_UNITS];
			TextureStage::State textureStage[8];

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
				};

				Interpolant interpolant[10];
			};

			Interpolant fog;
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
				return alphaCompareMode != ALPHA_ALWAYS;
			}

			bool pixelFogActive() const
			{
				return pixelFogMode != FOG_NONE;
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
				referenceMaskedSignedQ = replicate((reference + 0x80) & 0xFF & testMask);
			}

			static int64_t replicate(int b)
			{
				int64_t w = b & 0xFF;

				return (w << 0) | (w << 8) | (w << 16) | (w << 24) | (w << 32) | (w << 40) | (w << 48) | (w << 56);
			}
		};

		struct Fog
		{
			float4 scale;
			float4 offset;
			word4 color4[3];
			float4 colorF[3];
			float4 densityE;
			float4 density2E;
		};

		struct Factor
		{
			word4 textureFactor4[4];

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

		virtual void setFloatConstant(unsigned int index, const float value[4]);
		virtual void setIntegerConstant(unsigned int index, const int value[4]);
		virtual void setBooleanConstant(unsigned int index, int boolean);

		virtual void setUniformBuffer(int index, sw::Resource* buffer, int offset);
		virtual void lockUniformBuffers(byte** u);
		virtual void unlockUniformBuffers();

		virtual void setRenderTarget(int index, Surface *renderTarget);
		virtual void setDepthStencil(Surface *depthStencil);

		virtual void setTexCoordIndex(unsigned int stage, int texCoordIndex);
		virtual void setStageOperation(unsigned int stage, TextureStage::StageOperation stageOperation);
		virtual void setFirstArgument(unsigned int stage, TextureStage::SourceArgument firstArgument);
		virtual void setSecondArgument(unsigned int stage, TextureStage::SourceArgument secondArgument);
		virtual void setThirdArgument(unsigned int stage, TextureStage::SourceArgument thirdArgument);
		virtual void setStageOperationAlpha(unsigned int stage, TextureStage::StageOperation stageOperationAlpha);
		virtual void setFirstArgumentAlpha(unsigned int stage, TextureStage::SourceArgument firstArgumentAlpha);
		virtual void setSecondArgumentAlpha(unsigned int stage, TextureStage::SourceArgument secondArgumentAlpha);
		virtual void setThirdArgumentAlpha(unsigned int stage, TextureStage::SourceArgument thirdArgumentAlpha);
		virtual void setFirstModifier(unsigned int stage, TextureStage::ArgumentModifier firstModifier);
		virtual void setSecondModifier(unsigned int stage, TextureStage::ArgumentModifier secondModifier);
		virtual void setThirdModifier(unsigned int stage, TextureStage::ArgumentModifier thirdModifier);
		virtual void setFirstModifierAlpha(unsigned int stage, TextureStage::ArgumentModifier firstModifierAlpha);
		virtual void setSecondModifierAlpha(unsigned int stage, TextureStage::ArgumentModifier secondModifierAlpha);
		virtual void setThirdModifierAlpha(unsigned int stage, TextureStage::ArgumentModifier thirdModifierAlpha);
		virtual void setDestinationArgument(unsigned int stage, TextureStage::DestinationArgument destinationArgument);
		virtual void setConstantColor(unsigned int stage, const Color<float> &constantColor);
		virtual void setBumpmapMatrix(unsigned int stage, int element, float value);
		virtual void setLuminanceScale(unsigned int stage, float value);
		virtual void setLuminanceOffset(unsigned int stage, float value);

		virtual void setTextureFilter(unsigned int sampler, FilterType textureFilter);
		virtual void setMipmapFilter(unsigned int sampler, MipmapType mipmapFilter);
		virtual void setGatherEnable(unsigned int sampler, bool enable);
		virtual void setAddressingModeU(unsigned int sampler, AddressingMode addressingMode);
		virtual void setAddressingModeV(unsigned int sampler, AddressingMode addressingMode);
		virtual void setAddressingModeW(unsigned int sampler, AddressingMode addressingMode);
		virtual void setReadSRGB(unsigned int sampler, bool sRGB);
		virtual void setMipmapLOD(unsigned int sampler, float bias);
		virtual void setBorderColor(unsigned int sampler, const Color<float> &borderColor);
		virtual void setMaxAnisotropy(unsigned int sampler, float maxAnisotropy);
		virtual void setSwizzleR(unsigned int sampler, SwizzleType swizzleR);
		virtual void setSwizzleG(unsigned int sampler, SwizzleType swizzleG);
		virtual void setSwizzleB(unsigned int sampler, SwizzleType swizzleB);
		virtual void setSwizzleA(unsigned int sampler, SwizzleType swizzleA);

		virtual void setWriteSRGB(bool sRGB);
		virtual void setDepthBufferEnable(bool depthBufferEnable);
		virtual void setDepthCompare(DepthCompareMode depthCompareMode);
		virtual void setAlphaCompare(AlphaCompareMode alphaCompareMode);
		virtual void setDepthWriteEnable(bool depthWriteEnable);
		virtual void setAlphaTestEnable(bool alphaTestEnable);
		virtual void setCullMode(CullMode cullMode);
		virtual void setColorWriteMask(int index, int rgbaMask);

		virtual void setColorLogicOpEnabled(bool colorLogicOpEnabled);
		virtual void setLogicalOperation(LogicalOperation logicalOperation);

		virtual void setStencilEnable(bool stencilEnable);
		virtual void setStencilCompare(StencilCompareMode stencilCompareMode);
		virtual void setStencilReference(int stencilReference);
		virtual void setStencilMask(int stencilMask);
		virtual void setStencilFailOperation(StencilOperation stencilFailOperation);
		virtual void setStencilPassOperation(StencilOperation stencilPassOperation);
		virtual void setStencilZFailOperation(StencilOperation stencilZFailOperation);
		virtual void setStencilWriteMask(int stencilWriteMask);
		virtual void setTwoSidedStencil(bool enable);
		virtual void setStencilCompareCCW(StencilCompareMode stencilCompareMode);
		virtual void setStencilReferenceCCW(int stencilReference);
		virtual void setStencilMaskCCW(int stencilMask);
		virtual void setStencilFailOperationCCW(StencilOperation stencilFailOperation);
		virtual void setStencilPassOperationCCW(StencilOperation stencilPassOperation);
		virtual void setStencilZFailOperationCCW(StencilOperation stencilZFailOperation);
		virtual void setStencilWriteMaskCCW(int stencilWriteMask);

		virtual void setTextureFactor(const Color<float> &textureFactor);
		virtual void setBlendConstant(const Color<float> &blendConstant);

		virtual void setFillMode(FillMode fillMode);
		virtual void setShadingMode(ShadingMode shadingMode);
		
		virtual void setAlphaBlendEnable(bool alphaBlendEnable);
		virtual void setSourceBlendFactor(BlendFactor sourceBlendFactor);
		virtual void setDestBlendFactor(BlendFactor destBlendFactor);
		virtual void setBlendOperation(BlendOperation blendOperation);

		virtual void setSeparateAlphaBlendEnable(bool separateAlphaBlendEnable);
		virtual void setSourceBlendFactorAlpha(BlendFactor sourceBlendFactorAlpha);
		virtual void setDestBlendFactorAlpha(BlendFactor destBlendFactorAlpha);
		virtual void setBlendOperationAlpha(BlendOperation blendOperationAlpha);

		virtual void setAlphaReference(float alphaReference);

		virtual void setGlobalMipmapBias(float bias);

		virtual void setFogStart(float start);
		virtual void setFogEnd(float end);
		virtual void setFogColor(Color<float> fogColor);
		virtual void setFogDensity(float fogDensity);
		virtual void setPixelFogMode(FogMode fogMode);

		virtual void setPerspectiveCorrection(bool perspectiveCorrection);

		virtual void setOcclusionEnabled(bool enable);

	protected:
		const State update() const;
		Routine *routine(const State &state);
		void setRoutineCacheSize(int routineCacheSize);

		// Shader constants
		word4 cW[8][4];
		float4 c[FRAGMENT_UNIFORM_VECTORS];
		int4 i[16];
		bool b[16];
		Resource* uniformBuffer[MAX_UNIFORM_BUFFER_BINDINGS];
		int uniformBufferOffset[MAX_UNIFORM_BUFFER_BINDINGS];

		// Other semi-constants
		Stencil stencil;
		Stencil stencilCCW;
		Fog fog;
		Factor factor;

	private:
		void setFogRanges(float start, float end);

		Context *const context;

		RoutineCache<State> *routineCache;
	};
}

#endif   // sw_PixelProcessor_hpp
