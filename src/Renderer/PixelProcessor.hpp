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

			unsigned int depthOverride            : 1;
			unsigned int shaderContainsKill       : 1;

			unsigned int depthCompareMode         : BITS(Context::DEPTH_LAST);
			unsigned int alphaCompareMode         : BITS(Context::ALPHA_LAST);
			unsigned int depthWriteEnable         : 1;
			unsigned int quadLayoutDepthBuffer    : 1;

			unsigned int stencilActive            : 1;
			unsigned int stencilCompareMode       : BITS(Context::STENCIL_LAST);
			unsigned int stencilFailOperation     : BITS(Context::OPERATION_LAST);
			unsigned int stencilPassOperation     : BITS(Context::OPERATION_LAST);
			unsigned int stencilZFailOperation    : BITS(Context::OPERATION_LAST);
			unsigned int noStencilMask            : 1;
			unsigned int noStencilWriteMask       : 1;
			unsigned int stencilWriteMasked       : 1;
			unsigned int twoSidedStencil          : 1;
			unsigned int stencilCompareModeCCW    : BITS(Context::STENCIL_LAST);
			unsigned int stencilFailOperationCCW  : BITS(Context::OPERATION_LAST);
			unsigned int stencilPassOperationCCW  : BITS(Context::OPERATION_LAST);
			unsigned int stencilZFailOperationCCW : BITS(Context::OPERATION_LAST);
			unsigned int noStencilMaskCCW         : 1;
			unsigned int noStencilWriteMaskCCW    : 1;
			unsigned int stencilWriteMaskedCCW    : 1;

			unsigned int depthTestActive          : 1;
			unsigned int fogActive                : 1;
			unsigned int pixelFogMode             : BITS(Context::FOG_LAST);
			unsigned int specularAdd              : 1;
			unsigned int occlusionEnabled         : 1;
			unsigned int wBasedFog                : 1;
			unsigned int perspective              : 1;

			unsigned int alphaBlendActive         : 1;
			unsigned int sourceBlendFactor        : BITS(Context::BLEND_LAST);
			unsigned int destBlendFactor          : BITS(Context::BLEND_LAST);
			unsigned int blendOperation           : BITS(Context::BLENDOP_LAST);
			unsigned int sourceBlendFactorAlpha   : BITS(Context::BLEND_LAST);
			unsigned int destBlendFactorAlpha     : BITS(Context::BLEND_LAST);
			unsigned int blendOperationAlpha      : BITS(Context::BLENDOP_LAST);
			
			unsigned int colorWriteMask           : 16;   // (four times four component bit mask)
			unsigned char targetFormat[4];
			unsigned int writeSRGB                : 1;
			unsigned int multiSample              : 3;
			unsigned int multiSampleMask          : 4;
			unsigned int transparencyAntialiasing : BITS(Context::TRANSPARENCY_LAST);
			unsigned int centroid                 : 1;

			Sampler::State sampler[16];
			TextureStage::State textureStage[8];

			struct Interpolant
			{
				unsigned char component : 4;
				unsigned char flat : 4;
				unsigned char project : 2;
				unsigned char centroid : 1;
			};

			union
			{
				struct
				{
					Interpolant color[2];
					Interpolant texture[8];
					Interpolant fog;
				};

				Interpolant interpolant[10];
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
				return alphaCompareMode != Context::ALPHA_ALWAYS;
			}

			bool pixelFogActive() const
			{
				return pixelFogMode != Context::FOG_NONE;
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
			float4 densityE2;
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
		virtual void setMaxAnisotropy(unsigned int sampler, unsigned int maxAnisotropy);

		virtual void setWriteSRGB(bool sRGB);
		virtual void setDepthBufferEnable(bool depthBufferEnable);
		virtual void setDepthCompare(Context::DepthCompareMode depthCompareMode);
		virtual void setAlphaCompare(Context::AlphaCompareMode alphaCompareMode);
		virtual void setDepthWriteEnable(bool depthWriteEnable);
		virtual void setAlphaTestEnable(bool alphaTestEnable);
		virtual void setCullMode(Context::CullMode cullMode);
		virtual void setColorWriteMask(int index, int rgbaMask);

		virtual void setStencilEnable(bool stencilEnable);
		virtual void setStencilCompare(Context::StencilCompareMode stencilCompareMode);
		virtual void setStencilReference(int stencilReference);
		virtual void setStencilMask(int stencilMask);
		virtual void setStencilFailOperation(Context::StencilOperation stencilFailOperation);
		virtual void setStencilPassOperation(Context::StencilOperation stencilPassOperation);
		virtual void setStencilZFailOperation(Context::StencilOperation stencilZFailOperation);
		virtual void setStencilWriteMask(int stencilWriteMask);
		virtual void setTwoSidedStencil(bool enable);
		virtual void setStencilCompareCCW(Context::StencilCompareMode stencilCompareMode);
		virtual void setStencilReferenceCCW(int stencilReference);
		virtual void setStencilMaskCCW(int stencilMask);
		virtual void setStencilFailOperationCCW(Context::StencilOperation stencilFailOperation);
		virtual void setStencilPassOperationCCW(Context::StencilOperation stencilPassOperation);
		virtual void setStencilZFailOperationCCW(Context::StencilOperation stencilZFailOperation);
		virtual void setStencilWriteMaskCCW(int stencilWriteMask);

		virtual void setTextureFactor(const Color<float> &textureFactor);
		virtual void setBlendConstant(const Color<float> &blendConstant);

		virtual void setFillMode(Context::FillMode fillMode);
		virtual void setShadingMode(Context::ShadingMode shadingMode);
		
		virtual void setAlphaBlendEnable(bool alphaBlendEnable);
		virtual void setSourceBlendFactor(Context::BlendFactor sourceBlendFactor);
		virtual void setDestBlendFactor(Context::BlendFactor destBlendFactor);
		virtual void setBlendOperation(Context::BlendOperation blendOperation);

		virtual void setSeparateAlphaBlendEnable(bool separateAlphaBlendEnable);
		virtual void setSourceBlendFactorAlpha(Context::BlendFactor sourceBlendFactorAlpha);
		virtual void setDestBlendFactorAlpha(Context::BlendFactor destBlendFactorAlpha);
		virtual void setBlendOperationAlpha(Context::BlendOperation blendOperationAlpha);

		virtual void setAlphaReference(int alphaReference);

		virtual void setGlobalMipmapBias(float bias);

		virtual void setFogStart(float start);
		virtual void setFogEnd(float end);
		virtual void setFogColor(Color<float> fogColor);
		virtual void setFogDensity(float fogDensity);
		virtual void setPixelFogMode(Context::FogMode fogMode);

		virtual void setPerspectiveCorrection(bool perspectiveCorrection);

		virtual void setOcclusionEnabled(bool enable);

	protected:
		const State update() const;
		Routine *routine(const State &state);
		void setRoutineCacheSize(int routineCacheSize);

		// Shader constants
		word4 cW[8][4];
		float4 c[224];
		int4 i[16];
		bool b[16];

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
