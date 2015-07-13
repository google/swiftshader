// SwiftShader Software Renderer
//
// Copyright(c) 2005-2013 TransGaming Inc.
//
// All rights reserved. No part of this software may be copied, distributed, transmitted,
// transcribed, stored in a retrieval system, translated into any human or computer
// language by any means, or disclosed to third parties without the explicit written
// agreement of TransGaming Inc. Without such an agreement, no rights or licenses, express
// or implied, including but not limited to any patent rights, are granted to you.
//

#ifndef sw_PixelRoutine_hpp
#define sw_PixelRoutine_hpp

#include "QuadRasterizer.hpp"

namespace sw
{
	class PixelShader;
	class SamplerCore;

	class PixelRoutine : public sw::QuadRasterizer, public ShaderCore
	{
		friend class PixelProcessor;   // FIXME

	public:
		PixelRoutine(const PixelProcessor::State &state, const PixelShader *shader);

		virtual ~PixelRoutine();

	protected:
		struct Registers : public QuadRasterizer::Registers
		{
			Registers(const PixelShader *shader);

			Float4 z[4]; // Multisampled z
			Float4 w;    // Used as is
			Float4 rhw;  // Reciprocal w

			RegisterArray<4096> rf; // Computation registers
			RegisterArray<10> vf;   // Varying registers

			// Outputs
			Vector4f oC[4];
			Float4 oDepth;
		};


		typedef Shader::SourceParameter Src;
		typedef Shader::DestinationParameter Dst;

		virtual void setBuiltins(Registers &r, Int &x, Int &y, Float4(&z)[4], Float4 &w) = 0;
		virtual void applyShader(Registers &r, Int cMask[4]) = 0;
		virtual Bool alphaTest(Registers &r, Int cMask[4]) = 0;
		virtual void rasterOperation(Registers &r, Float4 &fog, Pointer<Byte> cBuffer[4], Int &x, Int sMask[4], Int zMask[4], Int cMask[4]) = 0;

		virtual void quad(QuadRasterizer::Registers &r, Pointer<Byte> cBuffer[4], Pointer<Byte> &zBuffer, Pointer<Byte> &sBuffer, Int cMask[4], Int &x, Int &y);

		Float4 interpolateCentroid(Float4 &x, Float4 &y, Float4 &rhw, Pointer<Byte> planeEquation, bool flat, bool perspective);
		void stencilTest(Registers &r, Pointer<Byte> &sBuffer, int q, Int &x, Int &sMask, Int &cMask);
		void stencilTest(Registers &r, Byte8 &value, StencilCompareMode stencilCompareMode, bool CCW);
		void stencilOperation(Registers &r, Byte8 &newValue, Byte8 &bufferValue, StencilOperation stencilPassOperation, StencilOperation stencilZFailOperation, StencilOperation stencilFailOperation, bool CCW, Int &zMask, Int &sMask);
		void stencilOperation(Registers &r, Byte8 &output, Byte8 &bufferValue, StencilOperation operation, bool CCW);
		Bool depthTest(Registers &r, Pointer<Byte> &zBuffer, int q, Int &x, Float4 &z, Int &sMask, Int &zMask, Int &cMask);
		void alphaTest(Registers &r, Int &aMask, Short4 &alpha);
		void alphaToCoverage(Registers &r, Int cMask[4], Float4 &alpha);
		void fogBlend(Registers &r, Vector4f &c0, Float4 &fog, Float4 &z, Float4 &rhw);
		void pixelFog(Registers &r, Float4 &visibility, Float4 &z, Float4 &rhw);

		// Raster operations
		void blendFactor(Registers &r, const Vector4s &blendFactor, const Vector4s &current, const Vector4s &pixel, BlendFactor blendFactorActive);
		void blendFactorAlpha(Registers &r, const Vector4s &blendFactor, const Vector4s &current, const Vector4s &pixel, BlendFactor blendFactorAlphaActive);
		void readPixel(Registers &r, int index, Pointer<Byte> &cBuffer, Vector4s &current, Int &x, Vector4s &pixel);
		void alphaBlend(Registers &r, int index, Pointer<Byte> &cBuffer, Vector4s &current, Int &x);
		void logicOperation(Registers &r, int index, Pointer<Byte> &cBuffer, Vector4s &current, Int &x);
		void writeColor(Registers &r, int index, Pointer<Byte> &cBuffer, Int &i, Vector4s &current, Int &sMask, Int &zMask, Int &cMask);
		void blendFactor(Registers &r, const Vector4f &blendFactor, const Vector4f &oC, const Vector4f &pixel, BlendFactor blendFactorActive);
		void blendFactorAlpha(Registers &r, const Vector4f &blendFactor, const Vector4f &oC, const Vector4f &pixel, BlendFactor blendFactorAlphaActive);
		void alphaBlend(Registers &r, int index, Pointer<Byte> &cBuffer, Vector4f &oC, Int &x);
		void writeColor(Registers &r, int index, Pointer<Byte> &cBuffer, Int &i, Vector4f &oC, Int &sMask, Int &zMask, Int &cMask);
		void writeStencil(Registers &r, Pointer<Byte> &sBuffer, int q, Int &x, Int &sMask, Int &zMask, Int &cMask);
		void writeDepth(Registers &r, Pointer<Byte> &zBuffer, int q, Int &x, Float4 &z, Int &zMask);

		UShort4 convertFixed16(Float4 &cf, bool saturate = true);
		void sRGBtoLinear16_12_16(Registers &r, Vector4s &c);
		void sRGBtoLinear12_16(Registers &r, Vector4s &c);
		void linearToSRGB16_12_16(Registers &r, Vector4s &c);
		void linearToSRGB12_16(Registers &r, Vector4s &c);
		Float4 sRGBtoLinear(const Float4 &x);

		bool colorUsed();

		SamplerCore *sampler[TEXTURE_IMAGE_UNITS];
	};
}

#endif   // sw_PixelRoutine_hpp
