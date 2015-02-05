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

#include "QuadRasterizer.hpp"

#include "Math.hpp"
#include "Primitive.hpp"
#include "Renderer.hpp"
#include "Constants.hpp"
#include "Debug.hpp"

namespace sw
{
	extern bool veryEarlyDepthTest;
	extern bool complementaryDepthBuffer;
	extern bool fullPixelPositionRegister;

	extern int clusterCount;

	QuadRasterizer::Registers::Registers()
	{
		occlusion = 0;

#if PERF_PROFILE
		for(int i = 0; i < PERF_TIMERS; i++)
		{
			cycles[i] = 0;
		}
#endif
	}

	QuadRasterizer::QuadRasterizer(const PixelProcessor::State &state, const PixelShader *pixelShader) : Rasterizer(state), shader(pixelShader)
	{
	}

	QuadRasterizer::~QuadRasterizer()
	{
	}

	void QuadRasterizer::generate()
	{
		Function<Void(Pointer<Byte>, Int, Int, Pointer<Byte>)> function;
		{
			#if PERF_PROFILE
				Long pixelTime = Ticks();
			#endif

			Pointer<Byte> primitive(function.arg(0));
			Int count(function.arg(1));
			Int cluster(function.arg(2));
			Pointer<Byte> data(function.arg(3));

			Registers& r = *createRegisters(shader);
			r.constants = *Pointer<Pointer<Byte> >(data + OFFSET(DrawData,constants));
			r.cluster = cluster;
			r.data = data;
			
			Do
			{
				r.primitive = primitive;

				Int yMin = *Pointer<Int>(primitive + OFFSET(Primitive,yMin));
				Int yMax = *Pointer<Int>(primitive + OFFSET(Primitive,yMax));

				Int cluster2 = r.cluster + r.cluster;
				yMin += clusterCount * 2 - 2 - cluster2;
				yMin &= -clusterCount * 2;
				yMin += cluster2;

				If(yMin < yMax)
				{
					rasterize(r, yMin, yMax);
				}

				primitive += sizeof(Primitive) * state.multiSample;
				count--;
			}
			Until(count == 0)

			if(state.occlusionEnabled)
			{
				UInt clusterOcclusion = *Pointer<UInt>(data + OFFSET(DrawData,occlusion) + 4 * cluster);
				clusterOcclusion += r.occlusion;
				*Pointer<UInt>(data + OFFSET(DrawData,occlusion) + 4 * cluster) = clusterOcclusion;
			}

			#if PERF_PROFILE
				r.cycles[PERF_PIXEL] = Ticks() - pixelTime;

				for(int i = 0; i < PERF_TIMERS; i++)
				{
					*Pointer<Long>(data + OFFSET(DrawData,cycles[i]) + 8 * cluster) += r.cycles[i];
				}
			#endif

			Return();

			delete &r;
		}

		routine = function(L"PixelRoutine_%0.8X", state.shaderID);
	}

	void QuadRasterizer::rasterize(Registers &r, Int &yMin, Int &yMax)
	{
		Pointer<Byte> cBuffer[RENDERTARGETS];
		Pointer<Byte> zBuffer;
		Pointer<Byte> sBuffer;

		for(int index = 0; index < RENDERTARGETS; index++)
		{
			if(state.colorWriteActive(index))
			{
				cBuffer[index] = *Pointer<Pointer<Byte> >(r.data + OFFSET(DrawData,colorBuffer[index])) + yMin * *Pointer<Int>(r.data + OFFSET(DrawData,colorPitchB[index]));
			}
		}

		if(state.depthTestActive)
		{
			zBuffer = *Pointer<Pointer<Byte> >(r.data + OFFSET(DrawData,depthBuffer)) + yMin * *Pointer<Int>(r.data + OFFSET(DrawData,depthPitchB));
		}

		if(state.stencilActive)
		{
			sBuffer = *Pointer<Pointer<Byte> >(r.data + OFFSET(DrawData,stencilBuffer)) + yMin * *Pointer<Int>(r.data + OFFSET(DrawData,stencilPitchB));
		}

		Int y = yMin;
		
		Do
		{
			Int x0a = Int(*Pointer<Short>(r.primitive + OFFSET(Primitive,outline->left) + (y + 0) * sizeof(Primitive::Span)));
			Int x0b = Int(*Pointer<Short>(r.primitive + OFFSET(Primitive,outline->left) + (y + 1) * sizeof(Primitive::Span)));
			Int x0 = Min(x0a, x0b);
			
			for(unsigned int q = 1; q < state.multiSample; q++)
			{
				x0a = Int(*Pointer<Short>(r.primitive + q * sizeof(Primitive) + OFFSET(Primitive,outline->left) + (y + 0) * sizeof(Primitive::Span)));
				x0b = Int(*Pointer<Short>(r.primitive + q * sizeof(Primitive) + OFFSET(Primitive,outline->left) + (y + 1) * sizeof(Primitive::Span)));
				x0 = Min(x0, Min(x0a, x0b));
			}
			
			x0 &= 0xFFFFFFFE;

			Int x1a = Int(*Pointer<Short>(r.primitive + OFFSET(Primitive,outline->right) + (y + 0) * sizeof(Primitive::Span)));
			Int x1b = Int(*Pointer<Short>(r.primitive + OFFSET(Primitive,outline->right) + (y + 1) * sizeof(Primitive::Span)));
			Int x1 = Max(x1a, x1b);

			for(unsigned int q = 1; q < state.multiSample; q++)
			{
				x1a = Int(*Pointer<Short>(r.primitive + q * sizeof(Primitive) + OFFSET(Primitive,outline->right) + (y + 0) * sizeof(Primitive::Span)));
				x1b = Int(*Pointer<Short>(r.primitive + q * sizeof(Primitive) + OFFSET(Primitive,outline->right) + (y + 1) * sizeof(Primitive::Span)));
				x1 = Max(x1, Max(x1a, x1b));
			}

			Float4 yyyy = Float4(Float(y)) + *Pointer<Float4>(r.primitive + OFFSET(Primitive,yQuad), 16);

			if(interpolateZ())
			{
				for(unsigned int q = 0; q < state.multiSample; q++)
				{
					Float4 y = yyyy;

					if(state.multiSample > 1)
					{
						y -= *Pointer<Float4>(r.constants + OFFSET(Constants,Y) + q * sizeof(float4));
					}

					r.Dz[q] = *Pointer<Float4>(r.primitive + OFFSET(Primitive,z.C), 16) + y * *Pointer<Float4>(r.primitive + OFFSET(Primitive,z.B), 16);
				}
			}

			if(veryEarlyDepthTest && state.multiSample == 1)
			{
				if(!state.stencilActive && state.depthTestActive && (state.depthCompareMode == DEPTH_LESSEQUAL || state.depthCompareMode == DEPTH_LESS))   // FIXME: Both modes ok?
				{
					Float4 xxxx = Float4(Float(x0)) + *Pointer<Float4>(r.primitive + OFFSET(Primitive,xQuad), 16);

					Pointer<Byte> buffer;
					Int pitch;

					if(!state.quadLayoutDepthBuffer)
					{
						buffer = zBuffer + 4 * x0;
						pitch = *Pointer<Int>(r.data + OFFSET(DrawData,depthPitchB));
					}
					else
					{	
						buffer = zBuffer + 8 * x0;
					}

					For(Int x = x0, x < x1, x += 2)
					{
						Float4 z = interpolate(xxxx, r.Dz[0], z, r.primitive + OFFSET(Primitive,z), false, false);

						Float4 zValue;
						
						if(!state.quadLayoutDepthBuffer)
						{
							// FIXME: Properly optimizes?
							zValue.xy = *Pointer<Float4>(buffer);
							zValue.zw = *Pointer<Float4>(buffer + pitch - 8);
						}
						else
						{
							zValue = *Pointer<Float4>(buffer, 16);
						}

						Int4 zTest;

						if(complementaryDepthBuffer)
						{
							zTest = CmpLE(zValue, z);
						}
						else
						{
							zTest = CmpNLT(zValue, z);
						}

						Int zMask = SignMask(zTest);

						If(zMask == 0)
						{
							x0 += 2;
						}
						Else
						{
							x = x1;
						}

						xxxx += Float4(2);

						if(!state.quadLayoutDepthBuffer)
						{
							buffer += 8;
						}
						else
						{
							buffer += 16;
						}
					}
				}
			}

			If(x0 < x1)
			{
				if(interpolateW())
				{
					r.Dw = *Pointer<Float4>(r.primitive + OFFSET(Primitive,w.C), 16) + yyyy * *Pointer<Float4>(r.primitive + OFFSET(Primitive,w.B), 16);
				}

				for(int interpolant = 0; interpolant < 10; interpolant++)
				{
					for(int component = 0; component < 4; component++)
					{
						if(state.interpolant[interpolant].component & (1 << component))
						{
							r.Dv[interpolant][component] = *Pointer<Float4>(r.primitive + OFFSET(Primitive,V[interpolant][component].C), 16);

							if(!(state.interpolant[interpolant].flat & (1 << component)))
							{
								r.Dv[interpolant][component] += yyyy * *Pointer<Float4>(r.primitive + OFFSET(Primitive,V[interpolant][component].B), 16);
							}
						}
					}
				}

				if(state.fog.component)
				{
					r.Df = *Pointer<Float4>(r.primitive + OFFSET(Primitive,f.C), 16);

					if(!state.fog.flat)
					{
						r.Df += yyyy * *Pointer<Float4>(r.primitive + OFFSET(Primitive,f.B), 16);
					}
				}

				Short4 xLeft[4];
				Short4 xRight[4];

				for(unsigned int q = 0; q < state.multiSample; q++)
				{
					xLeft[q] = *Pointer<Short4>(r.primitive + q * sizeof(Primitive) + OFFSET(Primitive,outline) + y * sizeof(Primitive::Span));
					xRight[q] = xLeft[q];

					xLeft[q] = Swizzle(xLeft[q], 0xA0) - Short4(1, 2, 1, 2);
					xRight[q] = Swizzle(xRight[q], 0xF5) - Short4(0, 1, 0, 1);
				}

				For(Int x = x0, x < x1, x += 2)
				{
					Short4 xxxx = Short4(x);
					Int cMask[4];

					for(unsigned int q = 0; q < state.multiSample; q++)
					{
						Short4 mask = CmpGT(xxxx, xLeft[q]) & CmpGT(xRight[q], xxxx);
						cMask[q] = SignMask(Pack(mask, mask)) & 0x0000000F;
					}

					quad(r, cBuffer, zBuffer, sBuffer, cMask, x, y);
				}
			}

			for(int index = 0; index < RENDERTARGETS; index++)
			{
				if(state.colorWriteActive(index))
				{
					cBuffer[index] += *Pointer<Int>(r.data + OFFSET(DrawData,colorPitchB[index])) << (1 + sw::log2(clusterCount));   // FIXME: Precompute
				}
			}

			if(state.depthTestActive)
			{
				zBuffer += *Pointer<Int>(r.data + OFFSET(DrawData,depthPitchB)) << (1 + sw::log2(clusterCount));   // FIXME: Precompute
			}

			if(state.stencilActive)
			{
				sBuffer += *Pointer<Int>(r.data + OFFSET(DrawData,stencilPitchB)) << (1 + sw::log2(clusterCount));   // FIXME: Precompute
			}

			y += 2 * clusterCount;
		}
		Until(y >= yMax)
	}

	Float4 QuadRasterizer::interpolate(Float4 &x, Float4 &D, Float4 &rhw, Pointer<Byte> planeEquation, bool flat, bool perspective)
	{
		Float4 interpolant = D;

		if(!flat)
		{
			interpolant += x * *Pointer<Float4>(planeEquation + OFFSET(PlaneEquation, A), 16);

			if(perspective)
			{
				interpolant *= rhw;
			}
		}

		return interpolant;
	}

	bool QuadRasterizer::interpolateZ() const
	{
		return state.depthTestActive || state.pixelFogActive() || (shader && shader->vPosDeclared && fullPixelPositionRegister);
	}

	bool QuadRasterizer::interpolateW() const
	{
		return state.perspective || (shader && shader->vPosDeclared && fullPixelPositionRegister);
	}
}
