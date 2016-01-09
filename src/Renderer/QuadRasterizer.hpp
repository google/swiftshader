// SwiftShader Software Renderer
//
// Copyright(c) 2005-2011 TransGaming Inc.
//
// All rights reserved. No part of this software may be copied, distributed, transmitted,
// transcribed, stored in a retrieval system, translated into any human or computer
// language by any means, or disclosed to third parties without the explicit written
// agreement of TransGaming Inc. Without such an agreement, no rights or licenses, express
// or implied, including but not limited to any patent rights, are granted to you.
//

#ifndef sw_QuadRasterizer_hpp
#define sw_QuadRasterizer_hpp

#include "Rasterizer.hpp"
#include "ShaderCore.hpp"
#include "PixelShader.hpp"

#include "Types.hpp"

namespace sw
{
	class QuadRasterizer : public Rasterizer
	{
	protected:
		QuadRasterizer(const PixelProcessor::State &state, const PixelShader *shader);

		virtual ~QuadRasterizer();

		struct Registers
		{
			Registers();
			virtual ~Registers() {};

			Pointer<Byte> constants;

			Pointer<Byte> primitive;
			Int cluster;
			Pointer<Byte> data;

			Float4 Dz[4];
			Float4 Dw;
			Float4 Dv[10][4];
			Float4 Df;

			UInt occlusion;

#if PERF_PROFILE
			Long cycles[PERF_TIMERS];
#endif
		};

		virtual void quad(Registers &r, Pointer<Byte> cBuffer[4], Pointer<Byte> &zBuffer, Pointer<Byte> &sBuffer, Int cMask[4], Int &x, Int &y) = 0;
		virtual Registers* createRegisters(const PixelShader *shader) = 0;

		bool interpolateZ() const;
		bool interpolateW() const;
		Float4 interpolate(Float4 &x, Float4 &D, Float4 &rhw, Pointer<Byte> planeEquation, bool flat, bool perspective);

		const PixelShader *const shader;

	private:
		void generate();

		void rasterize(Registers &r, Int &yMin, Int &yMax);
	};
}

#endif   // sw_QuadRasterizer_hpp
