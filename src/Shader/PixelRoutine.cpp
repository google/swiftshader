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

#include "PixelRoutine.hpp"

#include "Renderer.hpp"
#include "PixelShader.hpp"
#include "QuadRasterizer.hpp"
#include "Surface.hpp"
#include "Primitive.hpp"
#include "CPUID.hpp"
#include "SamplerCore.hpp"
#include "Constants.hpp"
#include "Debug.hpp"

#include <assert.h>

extern bool localShaderConstants;

namespace sw
{
	extern bool complementaryDepthBuffer;
	extern bool postBlendSRGB;
	extern bool exactColorRounding;

	PixelRoutine::PixelRoutine(const PixelProcessor::State &state, const PixelShader *pixelShader) : Rasterizer(state), pixelShader(pixelShader)
	{
		perturbate = false;
		luminance = false;
		previousScaling = false;

		returns = false;
		ifDepth = 0;
		loopRepDepth = 0;
		breakDepth = 0;

		for(int i = 0; i < 2048; i++)
		{
			labelBlock[i] = 0;
		}
	}

	PixelRoutine::~PixelRoutine()
	{
		for(int i = 0; i < 16; i++)
		{
			delete sampler[i];
		}
	}

	void PixelRoutine::quad(Registers &r, Pointer<Byte> cBuffer[4], Pointer<Byte> &zBuffer, Pointer<Byte> &sBuffer, Int cMask[4], Int &x, Int &y)
	{
		#if PERF_PROFILE
			Long pipeTime = Ticks();
		#endif

		for(int i = 0; i < 16; i++)
		{
			sampler[i] = new SamplerCore(r.constants, state.sampler[i]);
		}

		const bool earlyDepthTest = !state.depthOverride && !state.alphaTestActive();
		const bool integerPipeline = pixelShaderVersion() <= 0x0104;

		Int zMask[4];   // Depth mask
		Int sMask[4];   // Stencil mask

		for(unsigned int q = 0; q < state.multiSample; q++)
		{
			zMask[q] = cMask[q];
			sMask[q] = cMask[q];
		}

		for(unsigned int q = 0; q < state.multiSample; q++)
		{
			stencilTest(r, sBuffer, q, x, sMask[q], cMask[q]);
		}

		Float4 f;

		Color4i &current = r.ri[0];
		Color4i &diffuse = r.vi[0];
		Color4i &specular = r.vi[1];

		Float4 (&z)[4] = r.z;
		Float4 &rhw = r.rhw;
		Float4 rhwCentroid;

		Float4 xxxx = Float4(Float(x)) + *Pointer<Float4>(r.primitive + OFFSET(Primitive,xQuad), 16);
		Float4 yyyy = Float4(Float(y)) + *Pointer<Float4>(r.primitive + OFFSET(Primitive,yQuad), 16);

		if(state.depthTestActive || state.pixelFogActive())
		{
			for(unsigned int q = 0; q < state.multiSample; q++)
			{
				Float4 x = xxxx;
			
				if(state.multiSample > 1)
				{
					x -= *Pointer<Float4>(r.constants + OFFSET(Constants,X) + q * sizeof(float4));
				}

				z[q] = interpolate(x, r.Dz[q], z[q], r.primitive + OFFSET(Primitive,z), false, false);
			}
		}

		Bool depthPass = false;

		if(earlyDepthTest)
		{
			for(unsigned int q = 0; q < state.multiSample; q++)
			{
				depthPass = depthPass || depthTest(r, zBuffer, q, x, z[q], sMask[q], zMask[q], cMask[q]);
			}
		}

		If(depthPass || Bool(!earlyDepthTest))
		{
			#if PERF_PROFILE
				Long interpTime = Ticks();
			#endif

			// Centroid locations
			Float4 XXXX = Float4(0.0f);
			Float4 YYYY = Float4(0.0f);

			if(state.centroid)
			{
				Float4 WWWW(1.0e-9f);

				for(unsigned int q = 0; q < state.multiSample; q++)
				{
					XXXX += *Pointer<Float4>(r.constants + OFFSET(Constants,sampleX[q]) + 16 * cMask[q]);
					YYYY += *Pointer<Float4>(r.constants + OFFSET(Constants,sampleY[q]) + 16 * cMask[q]);
					WWWW += *Pointer<Float4>(r.constants + OFFSET(Constants,weight) + 16 * cMask[q]);
				}

				WWWW = Rcp_pp(WWWW);
				XXXX *= WWWW;
				YYYY *= WWWW;

				XXXX += xxxx;
				YYYY += yyyy;
			}

			if(state.perspective)
			{
				rhw = reciprocal(interpolate(xxxx, r.Dw, rhw, r.primitive + OFFSET(Primitive,w), false, false));

				if(state.centroid)
				{
					rhwCentroid = reciprocal(interpolateCentroid(XXXX, YYYY, rhwCentroid, r.primitive + OFFSET(Primitive,w), false, false));
				}
			}

			for(int interpolant = 0; interpolant < 10; interpolant++)
			{
				for(int component = 0; component < 4; component++)
				{
					Array<Float4> *pv;

					switch(component)
					{
					case 0: pv = &r.vx; break;
					case 1: pv = &r.vy; break;
					case 2: pv = &r.vz; break;
					case 3: pv = &r.vw; break;
					}

					Array<Float4> &v = *pv;

					if(state.interpolant[interpolant].component & (1 << component))
					{
						if(!state.interpolant[interpolant].centroid)
						{
							v[interpolant] = interpolate(xxxx, r.Dv[interpolant][component], rhw, r.primitive + OFFSET(Primitive,V[interpolant][component]), state.interpolant[interpolant].flat & (1 << component), state.perspective);
						}
						else
						{
							v[interpolant] = interpolateCentroid(XXXX, YYYY, rhwCentroid, r.primitive + OFFSET(Primitive,V[interpolant][component]), state.interpolant[interpolant].flat & (1 << component), state.perspective);
						}
					}
				}

				Float4 rcp;

				switch(state.interpolant[interpolant].project)
				{
				case 0:
					break;
				case 1:
					rcp = reciprocal(Float4(r.vy[interpolant]));
					r.vx[interpolant] = r.vx[interpolant] * rcp;
					break;
				case 2:
					rcp = reciprocal(Float4(r.vz[interpolant]));
					r.vx[interpolant] = r.vx[interpolant] * rcp;
					r.vy[interpolant] = r.vy[interpolant] * rcp;
					break;
				case 3:
					rcp = reciprocal(Float4(r.vw[interpolant]));
					r.vx[interpolant] = r.vx[interpolant] * rcp;
					r.vy[interpolant] = r.vy[interpolant] * rcp;
					r.vz[interpolant] = r.vz[interpolant] * rcp;
					break;
				}
			}

			if(state.fog.component)
			{
				f = interpolate(xxxx, r.Df, rhw, r.primitive + OFFSET(Primitive,f), state.fog.flat & 0x01, state.perspective);
			}

			if(integerPipeline)
			{
				if(state.color[0].component & 0x1) diffuse.x = convertFixed12(Float4(r.vx[0])); else diffuse.x = Short4(0x1000, 0x1000, 0x1000, 0x1000);
				if(state.color[0].component & 0x2) diffuse.y = convertFixed12(Float4(r.vy[0])); else diffuse.y = Short4(0x1000, 0x1000, 0x1000, 0x1000);
				if(state.color[0].component & 0x4) diffuse.z = convertFixed12(Float4(r.vz[0])); else diffuse.z = Short4(0x1000, 0x1000, 0x1000, 0x1000);
				if(state.color[0].component & 0x8) diffuse.w = convertFixed12(Float4(r.vw[0])); else diffuse.w = Short4(0x1000, 0x1000, 0x1000, 0x1000);

				if(state.color[1].component & 0x1) specular.x = convertFixed12(Float4(r.vx[1])); else specular.x = Short4(0x0000, 0x0000, 0x0000, 0x0000);
				if(state.color[1].component & 0x2) specular.y = convertFixed12(Float4(r.vy[1])); else specular.y = Short4(0x0000, 0x0000, 0x0000, 0x0000);
				if(state.color[1].component & 0x4) specular.z = convertFixed12(Float4(r.vz[1])); else specular.z = Short4(0x0000, 0x0000, 0x0000, 0x0000);
				if(state.color[1].component & 0x8) specular.w = convertFixed12(Float4(r.vw[1])); else specular.w = Short4(0x0000, 0x0000, 0x0000, 0x0000);
			}
			else if(pixelShaderVersion() >= 0x0300)
			{
				if(pixelShader->vPosDeclared)
				{
					r.vPos.x = Float4(Float(x)) + Float4(0, 1, 0, 1);
					r.vPos.y = Float4(Float(y)) + Float4(0, 0, 1, 1);
				}

				if(pixelShader->vFaceDeclared)
				{
					Float4 area = *Pointer<Float>(r.primitive + OFFSET(Primitive,area));
					
					r.vFace.x = area;
					r.vFace.y = area;
					r.vFace.z = area;
					r.vFace.w = area;
				}
			}

			#if PERF_PROFILE
				r.cycles[PERF_INTERP] += Ticks() - interpTime;
			#endif

			Bool alphaPass = true;

			if(colorUsed())
			{
				#if PERF_PROFILE
					Long shaderTime = Ticks();
				#endif

				if(pixelShader)
				{
				//	pixelShader->print("PixelShader-%0.16llX.txt", state.shaderHash);

					if(pixelShader->getVersion() <= 0x0104)
					{
						ps_1_x(r, cMask);
					}
					else
					{
						ps_2_x(r, cMask);
					}
				}
				else
				{
					current = diffuse;
					Color4i temp(0x0000, 0x0000, 0x0000, 0x0000);

					for(int stage = 0; stage < 8; stage++)
					{
						if(state.textureStage[stage].stageOperation == TextureStage::STAGE_DISABLE)
						{
							break;
						}

						Color4i texture;

						if(state.textureStage[stage].usesTexture)
						{
							sampleTexture(r, texture, stage, stage);
						}

						blendTexture(r, current, temp, texture, stage);
					}

					specularPixel(current, specular);
				}

				#if PERF_PROFILE
					r.cycles[PERF_SHADER] += Ticks() - shaderTime;
				#endif

				if(integerPipeline)
				{
					current.r = Min(current.r, Short4(0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF)); current.r = Max(current.r, Short4(0x0000, 0x0000, 0x0000, 0x0000));
					current.g = Min(current.g, Short4(0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF)); current.g = Max(current.g, Short4(0x0000, 0x0000, 0x0000, 0x0000));
					current.b = Min(current.b, Short4(0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF)); current.b = Max(current.b, Short4(0x0000, 0x0000, 0x0000, 0x0000));
					current.a = Min(current.a, Short4(0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF)); current.a = Max(current.a, Short4(0x0000, 0x0000, 0x0000, 0x0000));

					alphaPass = alphaTest(r, cMask, current);
				}
				else
				{
					clampColor(r.oC);

					alphaPass = alphaTest(r, cMask, r.oC[0]);
				}

				if((pixelShader && pixelShader->containsTexkill()) || state.alphaTestActive())
				{
					for(unsigned int q = 0; q < state.multiSample; q++)
					{
						zMask[q] &= cMask[q];
						sMask[q] &= cMask[q];
					}
				}
			}

			If(alphaPass)
			{
				if(!earlyDepthTest)
				{
					for(unsigned int q = 0; q < state.multiSample; q++)
					{
						depthPass = depthPass || depthTest(r, zBuffer, q, x, z[q], sMask[q], zMask[q], cMask[q]);
					}
				}

				#if PERF_PROFILE
					Long ropTime = Ticks();
				#endif

				If(depthPass || Bool(earlyDepthTest))
				{
					for(unsigned int q = 0; q < state.multiSample; q++)
					{
						if(state.multiSampleMask & (1 << q))
						{
							writeDepth(r, zBuffer, q, x, z[q], zMask[q]);

							if(state.occlusionEnabled)
							{
								r.occlusion += *Pointer<UInt>(r.constants + OFFSET(Constants,occlusionCount) + 4 * (zMask[q] & sMask[q]));
							}
						}
					}

					if(colorUsed())
					{
						#if PERF_PROFILE
							AddAtomic(Pointer<Long>(&profiler.ropOperations), Long(4));
						#endif

						if(integerPipeline)
						{
							rasterOperation(current, r, f, cBuffer[0], x, sMask, zMask, cMask);
						}
						else
						{
							rasterOperation(r.oC, r, f, cBuffer, x, sMask, zMask, cMask);
						}
					}
				}

				#if PERF_PROFILE
					r.cycles[PERF_ROP] += Ticks() - ropTime;
				#endif
			}
		}

		for(unsigned int q = 0; q < state.multiSample; q++)
		{
			if(state.multiSampleMask & (1 << q))
			{
				writeStencil(r, sBuffer, q, x, sMask[q], zMask[q], cMask[q]);
			}
		}

		#if PERF_PROFILE
			r.cycles[PERF_PIPE] += Ticks() - pipeTime;
		#endif
	}

	Float4 PixelRoutine::interpolate(Float4 &x, Float4 &D, Float4 &rhw, Pointer<Byte> planeEquation, bool flat, bool perspective)
	{
		Float4 interpolant = D;

		if(!flat)
		{
			interpolant += x * *Pointer<Float4>(planeEquation + OFFSET(PlaneEquation,A), 16);

			if(perspective)
			{
				interpolant *= rhw;
			}
		}

		return interpolant;
	}

	Float4 PixelRoutine::interpolateCentroid(Float4 &x, Float4 &y, Float4 &rhw, Pointer<Byte> planeEquation, bool flat, bool perspective)
	{
		Float4 interpolant = *Pointer<Float4>(planeEquation + OFFSET(PlaneEquation,C), 16);

		if(!flat)
		{
			interpolant += x * *Pointer<Float4>(planeEquation + OFFSET(PlaneEquation,A), 16) +
			               y * *Pointer<Float4>(planeEquation + OFFSET(PlaneEquation,B), 16);

			if(perspective)
			{
				interpolant *= rhw;
			}
		}

		return interpolant;
	}

	void PixelRoutine::stencilTest(Registers &r, Pointer<Byte> &sBuffer, int q, Int &x, Int &sMask, Int &cMask)
	{
		if(!state.stencilActive)
		{
			return;
		}

		// (StencilRef & StencilMask) CompFunc (StencilBufferValue & StencilMask)

		Pointer<Byte> buffer = sBuffer + 2 * x;

		if(q > 0)
		{
			buffer += q * *Pointer<Int>(r.data + OFFSET(DrawData,stencilSliceB));
		}

		Byte8 value = As<Byte8>(Long1(*Pointer<UInt>(buffer)));
		Byte8 valueCCW = value;

		if(!state.noStencilMask)
		{
			value &= *Pointer<Byte8>(r.data + OFFSET(DrawData,stencil[0].testMaskQ));
		}

		stencilTest(r, value, (Context::StencilCompareMode)state.stencilCompareMode, false);

		if(state.twoSidedStencil)
		{
			if(!state.noStencilMaskCCW)
			{
				valueCCW &= *Pointer<Byte8>(r.data + OFFSET(DrawData,stencil[1].testMaskQ));
			}

			stencilTest(r, valueCCW, (Context::StencilCompareMode)state.stencilCompareModeCCW, true);

			value &= *Pointer<Byte8>(r.primitive + OFFSET(Primitive,clockwiseMask));
			valueCCW &= *Pointer<Byte8>(r.primitive + OFFSET(Primitive,invClockwiseMask));
			value |= valueCCW;
		}

		sMask = SignMask(value) & cMask;
	}

	void PixelRoutine::stencilTest(Registers &r, Byte8 &value, Context::StencilCompareMode stencilCompareMode, bool CCW)
	{
		Byte8 equal;

		switch(stencilCompareMode)
		{
		case Context::STENCIL_ALWAYS:
			value = Byte8(0xFFFFFFFFFFFFFFFF);
			break;
		case Context::STENCIL_NEVER:
			value = Byte8(0x0000000000000000);
			break;
		case Context::STENCIL_LESS:			// a < b ~ b > a
			value += Byte8(0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80);
			value = CmpGT(As<SByte8>(value), *Pointer<SByte8>(r.data + OFFSET(DrawData,stencil[CCW].referenceMaskedSignedQ)));
			break;
		case Context::STENCIL_EQUAL:
			value = CmpEQ(value, *Pointer<Byte8>(r.data + OFFSET(DrawData,stencil[CCW].referenceMaskedQ)));
			break;
		case Context::STENCIL_NOTEQUAL:		// a != b ~ !(a == b)
			value = CmpEQ(value, *Pointer<Byte8>(r.data + OFFSET(DrawData,stencil[CCW].referenceMaskedQ)));
			value ^= Byte8(0xFFFFFFFFFFFFFFFF);
			break;
		case Context::STENCIL_LESSEQUAL:	// a <= b ~ (b > a) || (a == b)
			equal = value;
			equal = CmpEQ(equal, *Pointer<Byte8>(r.data + OFFSET(DrawData,stencil[CCW].referenceMaskedQ)));
			value += Byte8(0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80);
			value = CmpGT(As<SByte8>(value), *Pointer<SByte8>(r.data + OFFSET(DrawData,stencil[CCW].referenceMaskedSignedQ)));
			value |= equal;
			break;
		case Context::STENCIL_GREATER:		// a > b
			equal = *Pointer<Byte8>(r.data + OFFSET(DrawData,stencil[CCW].referenceMaskedSignedQ));
			value += Byte8(0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80);
			equal = CmpGT(As<SByte8>(equal), As<SByte8>(value));
			value = equal;
			break;
		case Context::STENCIL_GREATEREQUAL:	// a >= b ~ !(a < b) ~ !(b > a)
			value += Byte8(0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80);
			value = CmpGT(As<SByte8>(value), *Pointer<SByte8>(r.data + OFFSET(DrawData,stencil[CCW].referenceMaskedSignedQ)));
			value ^= Byte8(0xFFFFFFFFFFFFFFFF);
			break;
		default:
			ASSERT(false);
		}
	}

	Bool PixelRoutine::depthTest(Registers &r, Pointer<Byte> &zBuffer, int q, Int &x, Float4 &z, Int &sMask, Int &zMask, Int &cMask)
	{
		if(!state.depthTestActive)
		{
			return true;
		}

		Float4 Z = z;

		if(pixelShader && pixelShader->depthOverride())
		{
			if(complementaryDepthBuffer)
			{
				Z = Float4(1, 1, 1, 1) - r.oDepth;
			}
			else
			{
				Z = r.oDepth;
			}
		}

		Pointer<Byte> buffer;
		Int pitch;

		if(!state.quadLayoutDepthBuffer)
		{
			buffer = zBuffer + 4 * x;
			pitch = *Pointer<Int>(r.data + OFFSET(DrawData,depthPitchB));
		}
		else
		{
			buffer = zBuffer + 8 * x;
		}

		if(q > 0)
		{
			buffer += q * *Pointer<Int>(r.data + OFFSET(DrawData,depthSliceB));
		}

		Float4 zValue;

		if(state.depthCompareMode != Context::DEPTH_NEVER || (state.depthCompareMode != Context::DEPTH_ALWAYS && !state.depthWriteEnable))
		{
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
		}

		Int4 zTest;

		switch(state.depthCompareMode)
		{
		case Context::DEPTH_ALWAYS:
			// Optimized
			break;
		case Context::DEPTH_NEVER:
			// Optimized
			break;
		case Context::DEPTH_EQUAL:
			zTest = CmpEQ(zValue, Z);
			break;
		case Context::DEPTH_NOTEQUAL:
			zTest = CmpNEQ(zValue, Z);
			break;
		case Context::DEPTH_LESS:
			if(complementaryDepthBuffer)
			{
				zTest = CmpLT(zValue, Z);
			}
			else
			{
				zTest = CmpNLE(zValue, Z);
			}
			break;
		case Context::DEPTH_GREATEREQUAL:
			if(complementaryDepthBuffer)
			{
				zTest = CmpNLT(zValue, Z);
			}
			else
			{
				zTest = CmpLE(zValue, Z);
			}
			break;
		case Context::DEPTH_LESSEQUAL:
			if(complementaryDepthBuffer)
			{
				zTest = CmpLE(zValue, Z);
			}
			else
			{
				zTest = CmpNLT(zValue, Z);
			}
			break;
		case Context::DEPTH_GREATER:
			if(complementaryDepthBuffer)
			{
				zTest = CmpNLE(zValue, Z);
			}
			else
			{
				zTest = CmpLT(zValue, Z);
			}
			break;
		default:
			ASSERT(false);
		}

		switch(state.depthCompareMode)
		{
		case Context::DEPTH_ALWAYS:
			zMask = cMask;
			break;
		case Context::DEPTH_NEVER:
			zMask = 0x0;
			break;
		default:
			zMask = SignMask(zTest) & cMask;
			break;
		}
		
		if(state.stencilActive)
		{
			zMask &= sMask;
		}

		return zMask != 0;
	}

	void PixelRoutine::blendTexture(Registers &r, Color4i &current, Color4i &temp, Color4i &texture, int stage)
	{
		Color4i *arg1;
		Color4i *arg2;
		Color4i *arg3;
		Color4i res;

		Color4i constant;
		Color4i tfactor;

		const TextureStage::State &textureStage = state.textureStage[stage];

		if(textureStage.firstArgument == TextureStage::SOURCE_CONSTANT ||
		   textureStage.firstArgumentAlpha == TextureStage::SOURCE_CONSTANT ||
		   textureStage.secondArgument == TextureStage::SOURCE_CONSTANT ||
		   textureStage.secondArgumentAlpha == TextureStage::SOURCE_CONSTANT ||
		   textureStage.thirdArgument == TextureStage::SOURCE_CONSTANT ||
		   textureStage.thirdArgumentAlpha == TextureStage::SOURCE_CONSTANT)
		{
			constant.r = *Pointer<Short4>(r.data + OFFSET(DrawData,textureStage[stage].constantColor4[0]));
			constant.g = *Pointer<Short4>(r.data + OFFSET(DrawData,textureStage[stage].constantColor4[1]));
			constant.b = *Pointer<Short4>(r.data + OFFSET(DrawData,textureStage[stage].constantColor4[2]));
			constant.a = *Pointer<Short4>(r.data + OFFSET(DrawData,textureStage[stage].constantColor4[3]));
		}

		if(textureStage.firstArgument == TextureStage::SOURCE_TFACTOR ||
		   textureStage.firstArgumentAlpha == TextureStage::SOURCE_TFACTOR ||
		   textureStage.secondArgument == TextureStage::SOURCE_TFACTOR ||
		   textureStage.secondArgumentAlpha == TextureStage::SOURCE_TFACTOR ||
		   textureStage.thirdArgument == TextureStage::SOURCE_TFACTOR ||
		   textureStage.thirdArgumentAlpha == TextureStage::SOURCE_TFACTOR)
		{
			tfactor.r = *Pointer<Short4>(r.data + OFFSET(DrawData,factor.textureFactor4[0]));
			tfactor.g = *Pointer<Short4>(r.data + OFFSET(DrawData,factor.textureFactor4[1]));
			tfactor.b = *Pointer<Short4>(r.data + OFFSET(DrawData,factor.textureFactor4[2]));
			tfactor.a = *Pointer<Short4>(r.data + OFFSET(DrawData,factor.textureFactor4[3]));
		}

		// Premodulate
		if(stage > 0 && textureStage.usesTexture)
		{
			if(state.textureStage[stage - 1].stageOperation == TextureStage::STAGE_PREMODULATE)
			{
				current.r = MulHigh(current.r, texture.r) << 4;
				current.g = MulHigh(current.g, texture.g) << 4;
				current.b = MulHigh(current.b, texture.b) << 4;
			}

			if(state.textureStage[stage - 1].stageOperationAlpha == TextureStage::STAGE_PREMODULATE)
			{
				current.a = MulHigh(current.a, texture.a) << 4;
			}
		}

		if(luminance)
		{
			texture.r = MulHigh(texture.r, r.L) << 4;
			texture.g = MulHigh(texture.g, r.L) << 4;
			texture.b = MulHigh(texture.b, r.L) << 4;

			luminance = false;
		}

		switch(textureStage.firstArgument)
		{
		case TextureStage::SOURCE_TEXTURE:	arg1 = &texture;		break;
		case TextureStage::SOURCE_CONSTANT:	arg1 = &constant;		break;
		case TextureStage::SOURCE_CURRENT:	arg1 = &current;		break;
		case TextureStage::SOURCE_DIFFUSE:	arg1 = &r.diffuse;		break;
		case TextureStage::SOURCE_SPECULAR:	arg1 = &r.specular;		break;
		case TextureStage::SOURCE_TEMP:		arg1 = &temp;			break;
		case TextureStage::SOURCE_TFACTOR:	arg1 = &tfactor;		break;
		default:
			ASSERT(false);
		}

		switch(textureStage.secondArgument)
		{
		case TextureStage::SOURCE_TEXTURE:	arg2 = &texture;		break;
		case TextureStage::SOURCE_CONSTANT:	arg2 = &constant;		break;
		case TextureStage::SOURCE_CURRENT:	arg2 = &current;		break;
		case TextureStage::SOURCE_DIFFUSE:	arg2 = &r.diffuse;		break;
		case TextureStage::SOURCE_SPECULAR:	arg2 = &r.specular;		break;
		case TextureStage::SOURCE_TEMP:		arg2 = &temp;			break;
		case TextureStage::SOURCE_TFACTOR:	arg2 = &tfactor;		break;
		default:
			ASSERT(false);
		}

		switch(textureStage.thirdArgument)
		{
		case TextureStage::SOURCE_TEXTURE:	arg3 = &texture;		break;
		case TextureStage::SOURCE_CONSTANT:	arg3 = &constant;		break;
		case TextureStage::SOURCE_CURRENT:	arg3 = &current;		break;
		case TextureStage::SOURCE_DIFFUSE:	arg3 = &r.diffuse;		break;
		case TextureStage::SOURCE_SPECULAR:	arg3 = &r.specular;		break;
		case TextureStage::SOURCE_TEMP:		arg3 = &temp;			break;
		case TextureStage::SOURCE_TFACTOR:	arg3 = &tfactor;		break;
		default:
			ASSERT(false);
		}

		Color4i mod1;
		Color4i mod2;
		Color4i mod3;

		switch(textureStage.firstModifier)
		{
		case TextureStage::MODIFIER_COLOR:
			break;
		case TextureStage::MODIFIER_INVCOLOR:
			{
				mod1.r = SubSat(Short4(0x1000, 0x1000, 0x1000, 0x1000), arg1->r);
				mod1.g = SubSat(Short4(0x1000, 0x1000, 0x1000, 0x1000), arg1->g);
				mod1.b = SubSat(Short4(0x1000, 0x1000, 0x1000, 0x1000), arg1->b);
				mod1.a = SubSat(Short4(0x1000, 0x1000, 0x1000, 0x1000), arg1->a);

				arg1 = &mod1;
			}
			break;
		case TextureStage::MODIFIER_ALPHA:
			{
				mod1.r = arg1->a;
				mod1.g = arg1->a;
				mod1.b = arg1->a;
				mod1.a = arg1->a;

				arg1 = &mod1;
			}
			break;
		case TextureStage::MODIFIER_INVALPHA:
			{
				mod1.r = SubSat(Short4(0x1000, 0x1000, 0x1000, 0x1000), arg1->a);
				mod1.g = SubSat(Short4(0x1000, 0x1000, 0x1000, 0x1000), arg1->a);
				mod1.b = SubSat(Short4(0x1000, 0x1000, 0x1000, 0x1000), arg1->a);
				mod1.a = SubSat(Short4(0x1000, 0x1000, 0x1000, 0x1000), arg1->a);

				arg1 = &mod1;
			}
			break;
		default:
			ASSERT(false);
		}

		switch(textureStage.secondModifier)
		{
		case TextureStage::MODIFIER_COLOR:
			break;
		case TextureStage::MODIFIER_INVCOLOR:
			{
				mod2.r = SubSat(Short4(0x1000, 0x1000, 0x1000, 0x1000), arg2->r);
				mod2.g = SubSat(Short4(0x1000, 0x1000, 0x1000, 0x1000), arg2->g);
				mod2.b = SubSat(Short4(0x1000, 0x1000, 0x1000, 0x1000), arg2->b);
				mod2.a = SubSat(Short4(0x1000, 0x1000, 0x1000, 0x1000), arg2->a);

				arg2 = &mod2;
			}
			break;
		case TextureStage::MODIFIER_ALPHA:
			{
				mod2.r = arg2->a;
				mod2.g = arg2->a;
				mod2.b = arg2->a;
				mod2.a = arg2->a;

				arg2 = &mod2;
			}
			break;
		case TextureStage::MODIFIER_INVALPHA:
			{
				mod2.r = SubSat(Short4(0x1000, 0x1000, 0x1000, 0x1000), arg2->a);
				mod2.g = SubSat(Short4(0x1000, 0x1000, 0x1000, 0x1000), arg2->a);
				mod2.b = SubSat(Short4(0x1000, 0x1000, 0x1000, 0x1000), arg2->a);
				mod2.a = SubSat(Short4(0x1000, 0x1000, 0x1000, 0x1000), arg2->a);

				arg2 = &mod2;
			}
			break;
		default:
			ASSERT(false);
		}

		switch(textureStage.thirdModifier)
		{
		case TextureStage::MODIFIER_COLOR:
			break;
		case TextureStage::MODIFIER_INVCOLOR:
			{
				mod3.r = SubSat(Short4(0x1000, 0x1000, 0x1000, 0x1000), arg3->r);
				mod3.g = SubSat(Short4(0x1000, 0x1000, 0x1000, 0x1000), arg3->g);
				mod3.b = SubSat(Short4(0x1000, 0x1000, 0x1000, 0x1000), arg3->b);
				mod3.a = SubSat(Short4(0x1000, 0x1000, 0x1000, 0x1000), arg3->a);

				arg3 = &mod3;
			}
			break;
		case TextureStage::MODIFIER_ALPHA:
			{
				mod3.r = arg3->a;
				mod3.g = arg3->a;
				mod3.b = arg3->a;
				mod3.a = arg3->a;

				arg3 = &mod3;
			}
			break;
		case TextureStage::MODIFIER_INVALPHA:
			{
				mod3.r = SubSat(Short4(0x1000, 0x1000, 0x1000, 0x1000), arg3->a);
				mod3.g = SubSat(Short4(0x1000, 0x1000, 0x1000, 0x1000), arg3->a);
				mod3.b = SubSat(Short4(0x1000, 0x1000, 0x1000, 0x1000), arg3->a);
				mod3.a = SubSat(Short4(0x1000, 0x1000, 0x1000, 0x1000), arg3->a);

				arg3 = &mod3;
			}
			break;
		default:
			ASSERT(false);
		}

		switch(textureStage.stageOperation)
		{
		case TextureStage::STAGE_DISABLE:
			break;
		case TextureStage::STAGE_SELECTARG1:					// Arg1
			{
				res.r = arg1->r;
				res.g = arg1->g;
				res.b = arg1->b;
			}
			break;
		case TextureStage::STAGE_SELECTARG2:					// Arg2
			{
				res.r = arg2->r;
				res.g = arg2->g;
				res.b = arg2->b;
			}
			break;
		case TextureStage::STAGE_SELECTARG3:					// Arg3
			{
				res.r = arg3->r;
				res.g = arg3->g;
				res.b = arg3->b;
			}
			break;
		case TextureStage::STAGE_MODULATE:					// Arg1 * Arg2
			{
				res.r = MulHigh(arg1->r, arg2->r) << 4;
				res.g = MulHigh(arg1->g, arg2->g) << 4;
				res.b = MulHigh(arg1->b, arg2->b) << 4;
			}
			break;
		case TextureStage::STAGE_MODULATE2X:					// Arg1 * Arg2 * 2
			{
				res.r = MulHigh(arg1->r, arg2->r) << 5;
				res.g = MulHigh(arg1->g, arg2->g) << 5;
				res.b = MulHigh(arg1->b, arg2->b) << 5;
			}
			break;
		case TextureStage::STAGE_MODULATE4X:					// Arg1 * Arg2 * 4
			{
				res.r = MulHigh(arg1->r, arg2->r) << 6;
				res.g = MulHigh(arg1->g, arg2->g) << 6;
				res.b = MulHigh(arg1->b, arg2->b) << 6;
			}
			break;
		case TextureStage::STAGE_ADD:						// Arg1 + Arg2
			{
				res.r = AddSat(arg1->r, arg2->r);
				res.g = AddSat(arg1->g, arg2->g);
				res.b = AddSat(arg1->b, arg2->b);
			}
			break;
		case TextureStage::STAGE_ADDSIGNED:					// Arg1 + Arg2 - 0.5
			{
				res.r = AddSat(arg1->r, arg2->r);
				res.g = AddSat(arg1->g, arg2->g);
				res.b = AddSat(arg1->b, arg2->b);

				res.r = SubSat(res.r, Short4(0x0800, 0x0800, 0x0800, 0x0800));
				res.g = SubSat(res.g, Short4(0x0800, 0x0800, 0x0800, 0x0800));
				res.b = SubSat(res.b, Short4(0x0800, 0x0800, 0x0800, 0x0800));
			}
			break;
		case TextureStage::STAGE_ADDSIGNED2X:				// (Arg1 + Arg2 - 0.5) << 1
			{
				res.r = AddSat(arg1->r, arg2->r);
				res.g = AddSat(arg1->g, arg2->g);
				res.b = AddSat(arg1->b, arg2->b);

				res.r = SubSat(res.r, Short4(0x0800, 0x0800, 0x0800, 0x0800));
				res.g = SubSat(res.g, Short4(0x0800, 0x0800, 0x0800, 0x0800));
				res.b = SubSat(res.b, Short4(0x0800, 0x0800, 0x0800, 0x0800));

				res.r = AddSat(res.r, res.r);
				res.g = AddSat(res.g, res.g);
				res.b = AddSat(res.b, res.b);
			}
			break;
		case TextureStage::STAGE_SUBTRACT:					// Arg1 - Arg2
			{
				res.r = SubSat(arg1->r, arg2->r);
				res.g = SubSat(arg1->g, arg2->g);
				res.b = SubSat(arg1->b, arg2->b);
			}
			break;
		case TextureStage::STAGE_ADDSMOOTH:					// Arg1 + Arg2 - Arg1 * Arg2
			{
				Short4 tmp;

				tmp = MulHigh(arg1->r, arg2->r) << 4; res.r = AddSat(arg1->r, arg2->r); res.r = SubSat(res.r, tmp);
				tmp = MulHigh(arg1->g, arg2->g) << 4; res.g = AddSat(arg1->g, arg2->g); res.g = SubSat(res.g, tmp);
				tmp = MulHigh(arg1->b, arg2->b) << 4; res.b = AddSat(arg1->b, arg2->b); res.b = SubSat(res.b, tmp);
			}
			break;
		case TextureStage::STAGE_MULTIPLYADD:				// Arg3 + Arg1 * Arg2
			{
				res.r = MulHigh(arg1->r, arg2->r) << 4; res.r = AddSat(res.r, arg3->r);
				res.g = MulHigh(arg1->g, arg2->g) << 4; res.g = AddSat(res.g, arg3->g);
				res.b = MulHigh(arg1->b, arg2->b) << 4; res.b = AddSat(res.b, arg3->b);
			}
			break;
		case TextureStage::STAGE_LERP:						// Arg3 * (Arg1 - Arg2) + Arg2
			{
				res.r = SubSat(arg1->r, arg2->r); res.r = MulHigh(res.r, arg3->r) << 4; res.r = AddSat(res.r, arg2->r);
				res.g = SubSat(arg1->g, arg2->g); res.g = MulHigh(res.g, arg3->g) << 4; res.g = AddSat(res.g, arg2->g);
				res.b = SubSat(arg1->b, arg2->b); res.b = MulHigh(res.b, arg3->b) << 4; res.b = AddSat(res.b, arg2->b);
			}
			break;
		case TextureStage::STAGE_DOT3:						// 2 * (Arg1.r - 0.5) * 2 * (Arg2.r - 0.5) + 2 * (Arg1.g - 0.5) * 2 * (Arg2.g - 0.5) + 2 * (Arg1.b - 0.5) * 2 * (Arg2.b - 0.5)
			{
				Short4 tmp;

				res.r = SubSat(arg1->r, Short4(0x0800, 0x0800, 0x0800, 0x0800)); tmp = SubSat(arg2->r, Short4(0x0800, 0x0800, 0x0800, 0x0800)); res.r = MulHigh(res.r, tmp);
				res.g = SubSat(arg1->g, Short4(0x0800, 0x0800, 0x0800, 0x0800)); tmp = SubSat(arg2->g, Short4(0x0800, 0x0800, 0x0800, 0x0800)); res.g = MulHigh(res.g, tmp);
				res.b = SubSat(arg1->b, Short4(0x0800, 0x0800, 0x0800, 0x0800)); tmp = SubSat(arg2->b, Short4(0x0800, 0x0800, 0x0800, 0x0800)); res.b = MulHigh(res.b, tmp);

				res.r = res.r << 6;
				res.g = res.g << 6;
				res.b = res.b << 6;

				res.r = AddSat(res.r, res.g);
				res.r = AddSat(res.r, res.b);

				// Clamp to [0, 1]
				res.r = Max(res.r, Short4(0x0000, 0x0000, 0x0000, 0x0000));
				res.r = Min(res.r, Short4(0x1000, 0x1000, 0x1000, 0x1000));

				res.g = res.r;
				res.b = res.r;
				res.a = res.r;
			}
			break;
		case TextureStage::STAGE_BLENDCURRENTALPHA:			// Alpha * (Arg1 - Arg2) + Arg2
			{
				res.r = SubSat(arg1->r, arg2->r); res.r = MulHigh(res.r, current.a) << 4; res.r = AddSat(res.r, arg2->r);
				res.g = SubSat(arg1->g, arg2->g); res.g = MulHigh(res.g, current.a) << 4; res.g = AddSat(res.g, arg2->g);
				res.b = SubSat(arg1->b, arg2->b); res.b = MulHigh(res.b, current.a) << 4; res.b = AddSat(res.b, arg2->b);
			}
			break;
		case TextureStage::STAGE_BLENDDIFFUSEALPHA:			// Alpha * (Arg1 - Arg2) + Arg2
			{
				res.r = SubSat(arg1->r, arg2->r); res.r = MulHigh(res.r, r.diffuse.a) << 4; res.r = AddSat(res.r, arg2->r);
				res.g = SubSat(arg1->g, arg2->g); res.g = MulHigh(res.g, r.diffuse.a) << 4; res.g = AddSat(res.g, arg2->g);
				res.b = SubSat(arg1->b, arg2->b); res.b = MulHigh(res.b, r.diffuse.a) << 4; res.b = AddSat(res.b, arg2->b);
			}
			break;
		case TextureStage::STAGE_BLENDFACTORALPHA:			// Alpha * (Arg1 - Arg2) + Arg2
			{
				res.r = SubSat(arg1->r, arg2->r); res.r = MulHigh(res.r, *Pointer<Short4>(r.data + OFFSET(DrawData,factor.textureFactor4[3]))) << 4; res.r = AddSat(res.r, arg2->r);
				res.g = SubSat(arg1->g, arg2->g); res.g = MulHigh(res.g, *Pointer<Short4>(r.data + OFFSET(DrawData,factor.textureFactor4[3]))) << 4; res.g = AddSat(res.g, arg2->g);
				res.b = SubSat(arg1->b, arg2->b); res.b = MulHigh(res.b, *Pointer<Short4>(r.data + OFFSET(DrawData,factor.textureFactor4[3]))) << 4; res.b = AddSat(res.b, arg2->b);
			}
			break;
		case TextureStage::STAGE_BLENDTEXTUREALPHA:			// Alpha * (Arg1 - Arg2) + Arg2
			{
				res.r = SubSat(arg1->r, arg2->r); res.r = MulHigh(res.r, texture.a) << 4; res.r = AddSat(res.r, arg2->r);
				res.g = SubSat(arg1->g, arg2->g); res.g = MulHigh(res.g, texture.a) << 4; res.g = AddSat(res.g, arg2->g);
				res.b = SubSat(arg1->b, arg2->b); res.b = MulHigh(res.b, texture.a) << 4; res.b = AddSat(res.b, arg2->b);
			}
			break;
		case TextureStage::STAGE_BLENDTEXTUREALPHAPM:		// Arg1 + Arg2 * (1 - Alpha)
			{
				res.r = SubSat(Short4(0x1000, 0x1000, 0x1000, 0x1000), texture.a); res.r = MulHigh(res.r, arg2->r) << 4; res.r = AddSat(res.r, arg1->r);
				res.g = SubSat(Short4(0x1000, 0x1000, 0x1000, 0x1000), texture.a); res.g = MulHigh(res.g, arg2->g) << 4; res.g = AddSat(res.g, arg1->g);
				res.b = SubSat(Short4(0x1000, 0x1000, 0x1000, 0x1000), texture.a); res.b = MulHigh(res.b, arg2->b) << 4; res.b = AddSat(res.b, arg1->b);
			}
			break;
		case TextureStage::STAGE_PREMODULATE:
			{
				res.r = arg1->r;
				res.g = arg1->g;
				res.b = arg1->b;
			}
			break;
		case TextureStage::STAGE_MODULATEALPHA_ADDCOLOR:		// Arg1 + Arg1.a * Arg2
			{
				res.r = MulHigh(arg1->a, arg2->r) << 4; res.r = AddSat(res.r, arg1->r);
				res.g = MulHigh(arg1->a, arg2->g) << 4; res.g = AddSat(res.g, arg1->g);
				res.b = MulHigh(arg1->a, arg2->b) << 4; res.b = AddSat(res.b, arg1->b);
			}
			break;
		case TextureStage::STAGE_MODULATECOLOR_ADDALPHA:		// Arg1 * Arg2 + Arg1.a
			{
				res.r = MulHigh(arg1->r, arg2->r) << 4; res.r = AddSat(res.r, arg1->a);
				res.g = MulHigh(arg1->g, arg2->g) << 4; res.g = AddSat(res.g, arg1->a);
				res.b = MulHigh(arg1->b, arg2->b) << 4; res.b = AddSat(res.b, arg1->a);
			}
			break;
		case TextureStage::STAGE_MODULATEINVALPHA_ADDCOLOR:	// (1 - Arg1.a) * Arg2 + Arg1
			{
				Short4 tmp;

				res.r = AddSat(arg1->r, arg2->r); tmp = MulHigh(arg1->a, arg2->r) << 4; res.r = SubSat(res.r, tmp);
				res.g = AddSat(arg1->g, arg2->g); tmp = MulHigh(arg1->a, arg2->g) << 4; res.g = SubSat(res.g, tmp);
				res.b = AddSat(arg1->b, arg2->b); tmp = MulHigh(arg1->a, arg2->b) << 4; res.b = SubSat(res.b, tmp);
			}
			break;
		case TextureStage::STAGE_MODULATEINVCOLOR_ADDALPHA:	// (1 - Arg1) * Arg2 + Arg1.a
			{
				Short4 tmp;

				res.r = AddSat(arg1->a, arg2->r); tmp = MulHigh(arg1->r, arg2->r) << 4; res.r = SubSat(res.r, tmp);
				res.g = AddSat(arg1->a, arg2->g); tmp = MulHigh(arg1->g, arg2->g) << 4; res.g = SubSat(res.g, tmp);
				res.b = AddSat(arg1->a, arg2->b); tmp = MulHigh(arg1->b, arg2->b) << 4; res.b = SubSat(res.b, tmp);
			}
			break;
		case TextureStage::STAGE_BUMPENVMAP:
			{
				r.du = Float4(texture.r) * Float4(1.0f / 0x0FE0, 1.0f / 0x0FE0, 1.0f / 0x0FE0, 1.0f / 0x0FE0);
				r.dv = Float4(texture.g) * Float4(1.0f / 0x0FE0, 1.0f / 0x0FE0, 1.0f / 0x0FE0, 1.0f / 0x0FE0);
			
				Float4 du2;
				Float4 dv2;

				du2 = r.du;
				dv2 = r.dv;
				r.du *= *Pointer<Float4>(r.data + OFFSET(DrawData,textureStage[stage].bumpmapMatrix4F[0][0]));
				dv2 *= *Pointer<Float4>(r.data + OFFSET(DrawData,textureStage[stage].bumpmapMatrix4F[1][0]));
				r.du += dv2;
				r.dv *= *Pointer<Float4>(r.data + OFFSET(DrawData,textureStage[stage].bumpmapMatrix4F[1][1]));
				du2 *= *Pointer<Float4>(r.data + OFFSET(DrawData,textureStage[stage].bumpmapMatrix4F[0][1]));
				r.dv += du2;

				perturbate = true;

				res.r = r.current.r;
				res.g = r.current.g;
				res.b = r.current.b;
				res.a = r.current.a;
			}
			break;
		case TextureStage::STAGE_BUMPENVMAPLUMINANCE:
			{
				r.du = Float4(texture.r) * Float4(1.0f / 0x0FE0, 1.0f / 0x0FE0, 1.0f / 0x0FE0, 1.0f / 0x0FE0);
				r.dv = Float4(texture.g) * Float4(1.0f / 0x0FE0, 1.0f / 0x0FE0, 1.0f / 0x0FE0, 1.0f / 0x0FE0);
			
				Float4 du2;
				Float4 dv2;

				du2 = r.du;
				dv2 = r.dv;

				r.du *= *Pointer<Float4>(r.data + OFFSET(DrawData,textureStage[stage].bumpmapMatrix4F[0][0]));
				dv2 *= *Pointer<Float4>(r.data + OFFSET(DrawData,textureStage[stage].bumpmapMatrix4F[1][0]));
				r.du += dv2;
				r.dv *= *Pointer<Float4>(r.data + OFFSET(DrawData,textureStage[stage].bumpmapMatrix4F[1][1]));
				du2 *= *Pointer<Float4>(r.data + OFFSET(DrawData,textureStage[stage].bumpmapMatrix4F[0][1]));
				r.dv += du2;

				perturbate = true;

				r.L = texture.b;
				r.L = MulHigh(r.L, *Pointer<Short4>(r.data + OFFSET(DrawData,textureStage[stage].luminanceScale4)));
				r.L = r.L << 4;
				r.L = AddSat(r.L, *Pointer<Short4>(r.data + OFFSET(DrawData,textureStage[stage].luminanceOffset4)));
				r.L = Max(r.L, Short4(0x0000, 0x0000, 0x0000, 0x0000));
				r.L = Min(r.L, Short4(0x1000, 0x1000, 0x1000, 0x1000));

				luminance = true;

				res.r = r.current.r;
				res.g = r.current.g;
				res.b = r.current.b;
				res.a = r.current.a;
			}
			break;
		default:
			ASSERT(false);
		}

		if(textureStage.stageOperation != TextureStage::STAGE_DOT3)
		{
			switch(textureStage.firstArgumentAlpha)
			{
			case TextureStage::SOURCE_TEXTURE:	arg1 = &texture;		break;
			case TextureStage::SOURCE_CONSTANT:	arg1 = &constant;		break;
			case TextureStage::SOURCE_CURRENT:	arg1 = &current;		break;
			case TextureStage::SOURCE_DIFFUSE:	arg1 = &r.diffuse;		break;
			case TextureStage::SOURCE_SPECULAR:	arg1 = &r.specular;		break;
			case TextureStage::SOURCE_TEMP:		arg1 = &temp;			break;
			case TextureStage::SOURCE_TFACTOR:	arg1 = &tfactor;		break;
			default:
				ASSERT(false);
			}

			switch(textureStage.secondArgumentAlpha)
			{
			case TextureStage::SOURCE_TEXTURE:	arg2 = &texture;		break;
			case TextureStage::SOURCE_CONSTANT:	arg2 = &constant;		break;
			case TextureStage::SOURCE_CURRENT:	arg2 = &current;		break;
			case TextureStage::SOURCE_DIFFUSE:	arg2 = &r.diffuse;		break;
			case TextureStage::SOURCE_SPECULAR:	arg2 = &r.specular;		break;
			case TextureStage::SOURCE_TEMP:		arg2 = &temp;			break;
			case TextureStage::SOURCE_TFACTOR:	arg2 = &tfactor;		break;
			default:
				ASSERT(false);
			}

			switch(textureStage.thirdArgumentAlpha)
			{
			case TextureStage::SOURCE_TEXTURE:	arg3 = &texture;		break;
			case TextureStage::SOURCE_CONSTANT:	arg3 = &constant;		break;
			case TextureStage::SOURCE_CURRENT:	arg3 = &current;		break;
			case TextureStage::SOURCE_DIFFUSE:	arg3 = &r.diffuse;		break;
			case TextureStage::SOURCE_SPECULAR:	arg3 = &r.specular;		break;
			case TextureStage::SOURCE_TEMP:		arg3 = &temp;			break;
			case TextureStage::SOURCE_TFACTOR:	arg3 = &tfactor;		break;
			default:
				ASSERT(false);
			}

			switch(textureStage.firstModifierAlpha)   // FIXME: Check if actually used
			{
			case TextureStage::MODIFIER_COLOR:
				break;
			case TextureStage::MODIFIER_INVCOLOR:
				{
					mod1.a = SubSat(Short4(0x1000, 0x1000, 0x1000, 0x1000), arg1->a);

					arg1 = &mod1;
				}
				break;
			case TextureStage::MODIFIER_ALPHA:
				{
					// Redudant
				}
				break;
			case TextureStage::MODIFIER_INVALPHA:
				{
					mod1.a = SubSat(Short4(0x1000, 0x1000, 0x1000, 0x1000), arg1->a);

					arg1 = &mod1;
				}
				break;
			default:
				ASSERT(false);
			}

			switch(textureStage.secondModifierAlpha)   // FIXME: Check if actually used
			{
			case TextureStage::MODIFIER_COLOR:
				break;
			case TextureStage::MODIFIER_INVCOLOR:
				{
					mod2.a = SubSat(Short4(0x1000, 0x1000, 0x1000, 0x1000), arg2->a);

					arg2 = &mod2;
				}
				break;
			case TextureStage::MODIFIER_ALPHA:
				{
					// Redudant
				}
				break;
			case TextureStage::MODIFIER_INVALPHA:
				{
					mod2.a = SubSat(Short4(0x1000, 0x1000, 0x1000, 0x1000), arg2->a);

					arg2 = &mod2;
				}
				break;
			default:
				ASSERT(false);
			}

			switch(textureStage.thirdModifierAlpha)   // FIXME: Check if actually used
			{
			case TextureStage::MODIFIER_COLOR:
				break;
			case TextureStage::MODIFIER_INVCOLOR:
				{
					mod3.a = SubSat(Short4(0x1000, 0x1000, 0x1000, 0x1000), arg3->a);

					arg3 = &mod3;
				}
				break;
			case TextureStage::MODIFIER_ALPHA:
				{
					// Redudant
				}
				break;
			case TextureStage::MODIFIER_INVALPHA:
				{
					mod3.a = SubSat(Short4(0x1000, 0x1000, 0x1000, 0x1000), arg3->a);

					arg3 = &mod3;
				}
				break;
			default:
				ASSERT(false);
			}
		
			switch(textureStage.stageOperationAlpha)
			{
			case TextureStage::STAGE_DISABLE:
				break;
			case TextureStage::STAGE_SELECTARG1:					// Arg1
				{
					res.a = arg1->a;
				}
				break;
			case TextureStage::STAGE_SELECTARG2:					// Arg2
				{
					res.a = arg2->a;
				}
				break;
			case TextureStage::STAGE_SELECTARG3:					// Arg3
				{
					res.a = arg3->a;
				}
				break;
			case TextureStage::STAGE_MODULATE:					// Arg1 * Arg2
				{
					res.a = MulHigh(arg1->a, arg2->a) << 4;
				}
				break;
			case TextureStage::STAGE_MODULATE2X:					// Arg1 * Arg2 * 2
				{
					res.a = MulHigh(arg1->a, arg2->a) << 5;
				}
				break;
			case TextureStage::STAGE_MODULATE4X:					// Arg1 * Arg2 * 4
				{
					res.a = MulHigh(arg1->a, arg2->a) << 6;
				}
				break;
			case TextureStage::STAGE_ADD:						// Arg1 + Arg2
				{
					res.a = AddSat(arg1->a, arg2->a);
				}
				break;
			case TextureStage::STAGE_ADDSIGNED:					// Arg1 + Arg2 - 0.5
				{
					res.a = AddSat(arg1->a, arg2->a);
					res.a = SubSat(res.a, Short4(0x0800, 0x0800, 0x0800, 0x0800));
				}
				break;
			case TextureStage::STAGE_ADDSIGNED2X:					// (Arg1 + Arg2 - 0.5) << 1
				{
					res.a = AddSat(arg1->a, arg2->a);
					res.a = SubSat(res.a, Short4(0x0800, 0x0800, 0x0800, 0x0800));
					res.a = AddSat(res.a, res.a);
				}
				break;
			case TextureStage::STAGE_SUBTRACT:					// Arg1 - Arg2
				{
					res.a = SubSat(arg1->a, arg2->a);
				}
				break;
			case TextureStage::STAGE_ADDSMOOTH:					// Arg1 + Arg2 - Arg1 * Arg2
				{
					Short4 tmp;

					tmp = MulHigh(arg1->a, arg2->a) << 4; res.a = AddSat(arg1->a, arg2->a); res.a = SubSat(res.a, tmp);
				}
				break;
			case TextureStage::STAGE_MULTIPLYADD:				// Arg3 + Arg1 * Arg2
				{
					res.a = MulHigh(arg1->a, arg2->a) << 4; res.a = AddSat(res.a, arg3->a);
				}
				break;
			case TextureStage::STAGE_LERP:						// Arg3 * (Arg1 - Arg2) + Arg2
				{
					res.a = SubSat(arg1->a, arg2->a); res.a = MulHigh(res.a, arg3->a) << 4; res.a = AddSat(res.a, arg2->a);
				}
				break;
			case TextureStage::STAGE_DOT3:
				break;   // Already computed in color channel
			case TextureStage::STAGE_BLENDCURRENTALPHA:			// Alpha * (Arg1 - Arg2) + Arg2
				{
					res.a = SubSat(arg1->a, arg2->a); res.a = MulHigh(res.a, current.a) << 4; res.a = AddSat(res.a, arg2->a);
				}
				break;
			case TextureStage::STAGE_BLENDDIFFUSEALPHA:			// Arg1 * (Alpha) + Arg2 * (1 - Alpha)
				{
					res.a = SubSat(arg1->a, arg2->a); res.a = MulHigh(res.a, r.diffuse.a) << 4; res.a = AddSat(res.a, arg2->a);
				}
				break;
			case TextureStage::STAGE_BLENDFACTORALPHA:
				{
					res.a = SubSat(arg1->a, arg2->a); res.a = MulHigh(res.a, *Pointer<Short4>(r.data + OFFSET(DrawData,factor.textureFactor4[3]))) << 4; res.a = AddSat(res.a, arg2->a);
				}
				break;
			case TextureStage::STAGE_BLENDTEXTUREALPHA:			// Arg1 * (Alpha) + Arg2 * (1 - Alpha)
				{
					res.a = SubSat(arg1->a, arg2->a); res.a = MulHigh(res.a, texture.a) << 4; res.a = AddSat(res.a, arg2->a);
				}
				break;
			case TextureStage::STAGE_BLENDTEXTUREALPHAPM:		// Arg1 + Arg2 * (1 - Alpha)
				{
					res.a = SubSat(Short4(0x1000, 0x1000, 0x1000, 0x1000), texture.a); res.a = MulHigh(res.a, arg2->a) << 4; res.a = AddSat(res.a, arg1->a);
				}
				break;
			case TextureStage::STAGE_PREMODULATE:
				{
					res.a = arg1->a;
				}
				break;
			case TextureStage::STAGE_MODULATEALPHA_ADDCOLOR:
			case TextureStage::STAGE_MODULATECOLOR_ADDALPHA:
			case TextureStage::STAGE_MODULATEINVALPHA_ADDCOLOR:
			case TextureStage::STAGE_MODULATEINVCOLOR_ADDALPHA:
			case TextureStage::STAGE_BUMPENVMAP:
			case TextureStage::STAGE_BUMPENVMAPLUMINANCE:
				break;   // Invalid alpha operations
			default:
				ASSERT(false);
			}
		}

		// Clamp result to [0, 1]

		switch(textureStage.stageOperation)
		{
		case TextureStage::STAGE_DISABLE:
		case TextureStage::STAGE_SELECTARG1:
		case TextureStage::STAGE_SELECTARG2:
		case TextureStage::STAGE_SELECTARG3:
		case TextureStage::STAGE_MODULATE:
		case TextureStage::STAGE_MODULATE2X:
		case TextureStage::STAGE_MODULATE4X:
		case TextureStage::STAGE_ADD:
		case TextureStage::STAGE_MULTIPLYADD:
		case TextureStage::STAGE_LERP:
		case TextureStage::STAGE_BLENDCURRENTALPHA:
		case TextureStage::STAGE_BLENDDIFFUSEALPHA:
		case TextureStage::STAGE_BLENDFACTORALPHA:
		case TextureStage::STAGE_BLENDTEXTUREALPHA:
		case TextureStage::STAGE_BLENDTEXTUREALPHAPM:
		case TextureStage::STAGE_DOT3:   // Already clamped
		case TextureStage::STAGE_PREMODULATE:
		case TextureStage::STAGE_MODULATEALPHA_ADDCOLOR:
		case TextureStage::STAGE_MODULATECOLOR_ADDALPHA:
		case TextureStage::STAGE_MODULATEINVALPHA_ADDCOLOR:
		case TextureStage::STAGE_MODULATEINVCOLOR_ADDALPHA:
		case TextureStage::STAGE_BUMPENVMAP:
		case TextureStage::STAGE_BUMPENVMAPLUMINANCE:
			if(state.textureStage[stage].cantUnderflow)
			{
				break;   // Can't go below zero
			}
		case TextureStage::STAGE_ADDSIGNED:
		case TextureStage::STAGE_ADDSIGNED2X:
		case TextureStage::STAGE_SUBTRACT:
		case TextureStage::STAGE_ADDSMOOTH:
			res.r = Max(res.r, Short4(0x0000, 0x0000, 0x0000, 0x0000));
			res.g = Max(res.g, Short4(0x0000, 0x0000, 0x0000, 0x0000));
			res.b = Max(res.b, Short4(0x0000, 0x0000, 0x0000, 0x0000));
			break;
		default:
			ASSERT(false);
		}

		switch(textureStage.stageOperationAlpha)
		{
		case TextureStage::STAGE_DISABLE:
		case TextureStage::STAGE_SELECTARG1:
		case TextureStage::STAGE_SELECTARG2:
		case TextureStage::STAGE_SELECTARG3:
		case TextureStage::STAGE_MODULATE:
		case TextureStage::STAGE_MODULATE2X:
		case TextureStage::STAGE_MODULATE4X:
		case TextureStage::STAGE_ADD:
		case TextureStage::STAGE_MULTIPLYADD:
		case TextureStage::STAGE_LERP:
		case TextureStage::STAGE_BLENDCURRENTALPHA:
		case TextureStage::STAGE_BLENDDIFFUSEALPHA:
		case TextureStage::STAGE_BLENDFACTORALPHA:
		case TextureStage::STAGE_BLENDTEXTUREALPHA:
		case TextureStage::STAGE_BLENDTEXTUREALPHAPM:
		case TextureStage::STAGE_DOT3:   // Already clamped
		case TextureStage::STAGE_PREMODULATE:
		case TextureStage::STAGE_MODULATEALPHA_ADDCOLOR:
		case TextureStage::STAGE_MODULATECOLOR_ADDALPHA:
		case TextureStage::STAGE_MODULATEINVALPHA_ADDCOLOR:
		case TextureStage::STAGE_MODULATEINVCOLOR_ADDALPHA:
		case TextureStage::STAGE_BUMPENVMAP:
		case TextureStage::STAGE_BUMPENVMAPLUMINANCE:
			if(state.textureStage[stage].cantUnderflow)
			{
				break;   // Can't go below zero
			}
		case TextureStage::STAGE_ADDSIGNED:
		case TextureStage::STAGE_ADDSIGNED2X:
		case TextureStage::STAGE_SUBTRACT:
		case TextureStage::STAGE_ADDSMOOTH:
			res.a = Max(res.a, Short4(0x0000, 0x0000, 0x0000, 0x0000));
			break;
		default:
			ASSERT(false);
		}

		switch(textureStage.stageOperation)
		{
		case TextureStage::STAGE_DISABLE:
		case TextureStage::STAGE_SELECTARG1:
		case TextureStage::STAGE_SELECTARG2:
		case TextureStage::STAGE_SELECTARG3:
		case TextureStage::STAGE_MODULATE:
		case TextureStage::STAGE_SUBTRACT:
		case TextureStage::STAGE_ADDSMOOTH:
		case TextureStage::STAGE_LERP:
		case TextureStage::STAGE_BLENDCURRENTALPHA:
		case TextureStage::STAGE_BLENDDIFFUSEALPHA:
		case TextureStage::STAGE_BLENDFACTORALPHA:
		case TextureStage::STAGE_BLENDTEXTUREALPHA:
		case TextureStage::STAGE_DOT3:   // Already clamped
		case TextureStage::STAGE_PREMODULATE:
		case TextureStage::STAGE_MODULATEINVALPHA_ADDCOLOR:
		case TextureStage::STAGE_MODULATEINVCOLOR_ADDALPHA:
		case TextureStage::STAGE_BUMPENVMAP:
		case TextureStage::STAGE_BUMPENVMAPLUMINANCE:
			break;   // Can't go above one
		case TextureStage::STAGE_MODULATE2X:
		case TextureStage::STAGE_MODULATE4X:
		case TextureStage::STAGE_ADD:
		case TextureStage::STAGE_ADDSIGNED:
		case TextureStage::STAGE_ADDSIGNED2X:
		case TextureStage::STAGE_MULTIPLYADD:
		case TextureStage::STAGE_BLENDTEXTUREALPHAPM:
		case TextureStage::STAGE_MODULATEALPHA_ADDCOLOR:
		case TextureStage::STAGE_MODULATECOLOR_ADDALPHA:
			res.r = Min(res.r, Short4(0x1000, 0x1000, 0x1000, 0x1000));
			res.g = Min(res.g, Short4(0x1000, 0x1000, 0x1000, 0x1000));
			res.b = Min(res.b, Short4(0x1000, 0x1000, 0x1000, 0x1000));
			break;
		default:
			ASSERT(false);
		}

		switch(textureStage.stageOperationAlpha)
		{
		case TextureStage::STAGE_DISABLE:
		case TextureStage::STAGE_SELECTARG1:
		case TextureStage::STAGE_SELECTARG2:
		case TextureStage::STAGE_SELECTARG3:
		case TextureStage::STAGE_MODULATE:
		case TextureStage::STAGE_SUBTRACT:
		case TextureStage::STAGE_ADDSMOOTH:
		case TextureStage::STAGE_LERP:
		case TextureStage::STAGE_BLENDCURRENTALPHA:
		case TextureStage::STAGE_BLENDDIFFUSEALPHA:
		case TextureStage::STAGE_BLENDFACTORALPHA:
		case TextureStage::STAGE_BLENDTEXTUREALPHA:
		case TextureStage::STAGE_DOT3:   // Already clamped
		case TextureStage::STAGE_PREMODULATE:
		case TextureStage::STAGE_MODULATEINVALPHA_ADDCOLOR:
		case TextureStage::STAGE_MODULATEINVCOLOR_ADDALPHA:
		case TextureStage::STAGE_BUMPENVMAP:
		case TextureStage::STAGE_BUMPENVMAPLUMINANCE:
			break;   // Can't go above one
		case TextureStage::STAGE_MODULATE2X:
		case TextureStage::STAGE_MODULATE4X:
		case TextureStage::STAGE_ADD:
		case TextureStage::STAGE_ADDSIGNED:
		case TextureStage::STAGE_ADDSIGNED2X:
		case TextureStage::STAGE_MULTIPLYADD:
		case TextureStage::STAGE_BLENDTEXTUREALPHAPM:
		case TextureStage::STAGE_MODULATEALPHA_ADDCOLOR:
		case TextureStage::STAGE_MODULATECOLOR_ADDALPHA:
			res.a = Min(res.a, Short4(0x1000, 0x1000, 0x1000, 0x1000));
			break;
		default:
			ASSERT(false);
		}

		switch(textureStage.destinationArgument)
		{
		case TextureStage::DESTINATION_CURRENT:
			current.r = res.r;
			current.g = res.g;
			current.b = res.b;
			current.a = res.a;
			break;
		case TextureStage::DESTINATION_TEMP:
			temp.r = res.r;
			temp.g = res.g;
			temp.b = res.b;
			temp.a = res.a;
			break;
		default:
			ASSERT(false);
		}
	}

	void PixelRoutine::alphaTest(Registers &r, Int &aMask, Short4 &alpha)
	{
		Short4 cmp;
		Short4 equal;

		switch(state.alphaCompareMode)
		{
		case Context::ALPHA_ALWAYS:
			aMask = 0xF;
			break;
		case Context::ALPHA_NEVER:
			aMask = 0x0;
			break;
		case Context::ALPHA_EQUAL:
			cmp = CmpEQ(alpha, *Pointer<Short4>(r.data + OFFSET(DrawData,factor.alphaReference4)));
			aMask = SignMask(Pack(cmp, Short4(0x0000, 0x0000, 0x0000, 0x0000)));
			break;
		case Context::ALPHA_NOTEQUAL:		// a != b ~ !(a == b)
			cmp = CmpEQ(alpha, *Pointer<Short4>(r.data + OFFSET(DrawData,factor.alphaReference4))) ^ Short4((short)0xFFFF, (short)0xFFFF, (short)0xFFFF, (short)0xFFFF);   // FIXME
			aMask = SignMask(Pack(cmp, Short4(0x0000, 0x0000, 0x0000, 0x0000)));
			break;
		case Context::ALPHA_LESS:			// a < b ~ b > a
			cmp = CmpGT(*Pointer<Short4>(r.data + OFFSET(DrawData,factor.alphaReference4)), alpha);
			aMask = SignMask(Pack(cmp, Short4(0x0000, 0x0000, 0x0000, 0x0000)));
			break;
		case Context::ALPHA_GREATEREQUAL:	// a >= b ~ (a > b) || (a == b) ~ !(b > a)   // TODO: Approximate
			equal = CmpEQ(alpha, *Pointer<Short4>(r.data + OFFSET(DrawData,factor.alphaReference4)));
			cmp = CmpGT(alpha, *Pointer<Short4>(r.data + OFFSET(DrawData,factor.alphaReference4)));
			cmp |= equal;
			aMask = SignMask(Pack(cmp, Short4(0x0000, 0x0000, 0x0000, 0x0000)));
			break;
		case Context::ALPHA_LESSEQUAL:		// a <= b ~ !(a > b)
			cmp = CmpGT(alpha, *Pointer<Short4>(r.data + OFFSET(DrawData,factor.alphaReference4))) ^ Short4((short)0xFFFF, (short)0xFFFF, (short)0xFFFF, (short)0xFFFF);   // FIXME
			aMask = SignMask(Pack(cmp, Short4(0x0000, 0x0000, 0x0000, 0x0000)));
			break;
		case Context::ALPHA_GREATER:			// a > b
			cmp = CmpGT(alpha, *Pointer<Short4>(r.data + OFFSET(DrawData,factor.alphaReference4)));
			aMask = SignMask(Pack(cmp, Short4(0x0000, 0x0000, 0x0000, 0x0000)));
			break;
		default:
			ASSERT(false);
		}
	}

	void PixelRoutine::alphaToCoverage(Registers &r, Int cMask[4], Float4 &alpha)
	{
		Int4 coverage0 = CmpNLT(alpha, *Pointer<Float4>(r.data + OFFSET(DrawData,a2c0)));
		Int4 coverage1 = CmpNLT(alpha, *Pointer<Float4>(r.data + OFFSET(DrawData,a2c1)));
		Int4 coverage2 = CmpNLT(alpha, *Pointer<Float4>(r.data + OFFSET(DrawData,a2c2)));
		Int4 coverage3 = CmpNLT(alpha, *Pointer<Float4>(r.data + OFFSET(DrawData,a2c3)));

		Int aMask0 = SignMask(coverage0);
		Int aMask1 = SignMask(coverage1);
		Int aMask2 = SignMask(coverage2);
		Int aMask3 = SignMask(coverage3);

		cMask[0] &= aMask0;
		cMask[1] &= aMask1;
		cMask[2] &= aMask2;
		cMask[3] &= aMask3;
	}

	Bool PixelRoutine::alphaTest(Registers &r, Int cMask[4], Color4i &current)
	{
		if(!state.alphaTestActive())
		{
			return true;
		}

		Int aMask;

		if(state.transparencyAntialiasing == Context::TRANSPARENCY_NONE)
		{
			alphaTest(r, aMask, current.a);

			for(unsigned int q = 0; q < state.multiSample; q++)
			{
				cMask[q] &= aMask;
			}
		}
		else if(state.transparencyAntialiasing == Context::TRANSPARENCY_ALPHA_TO_COVERAGE)
		{
			Float4 alpha = Float4(current.a) * Float4(1.0f / 0x1000);

			alphaToCoverage(r, cMask, alpha);
		}
		else ASSERT(false);

		Int pass = cMask[0];

		for(unsigned int q = 1; q < state.multiSample; q++)
		{
			pass = pass | cMask[q];
		}

		return pass != 0x0;
	}

	Bool PixelRoutine::alphaTest(Registers &r, Int cMask[4], Color4f &c0)
	{
		if(!state.alphaTestActive())
		{
			return true;
		}

		Int aMask;

		if(state.transparencyAntialiasing == Context::TRANSPARENCY_NONE)
		{
			Short4 alpha = RoundShort4(c0.a * Float4(0x1000, 0x1000, 0x1000, 0x1000));

			alphaTest(r, aMask, alpha);

			for(unsigned int q = 0; q < state.multiSample; q++)
			{
				cMask[q] &= aMask;
			}
		}
		else if(state.transparencyAntialiasing == Context::TRANSPARENCY_ALPHA_TO_COVERAGE)
		{
			alphaToCoverage(r, cMask, c0.a);
		}
		else ASSERT(false);

		Int pass = cMask[0];

		for(unsigned int q = 1; q < state.multiSample; q++)
		{
			pass = pass | cMask[q];
		}

		return pass != 0x0;
	}

	void PixelRoutine::fogBlend(Registers &r, Color4i &current, Float4 &f, Float4 &z, Float4 &rhw)
	{
		if(!state.fogActive)
		{
			return;
		}

		if(state.pixelFogMode != Context::FOG_NONE)
		{
			pixelFog(r, f, z, rhw);
		}
		
		UShort4 fog = convertFixed16(f, true);

		current.r = As<Short4>(MulHigh(As<UShort4>(current.r), fog));
		current.g = As<Short4>(MulHigh(As<UShort4>(current.g), fog));
		current.b = As<Short4>(MulHigh(As<UShort4>(current.b), fog));

		UShort4 invFog = UShort4(0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF) - fog;

		current.r += As<Short4>(MulHigh(invFog, *Pointer<UShort4>(r.data + OFFSET(DrawData,fog.color4[0]))));
		current.g += As<Short4>(MulHigh(invFog, *Pointer<UShort4>(r.data + OFFSET(DrawData,fog.color4[1]))));
		current.b += As<Short4>(MulHigh(invFog, *Pointer<UShort4>(r.data + OFFSET(DrawData,fog.color4[2]))));
	}

	void PixelRoutine::fogBlend(Registers &r, Color4f &c0, Float4 &fog, Float4 &z, Float4 &rhw)
	{
		if(!state.fogActive)
		{
			return;
		}

		if(state.pixelFogMode != Context::FOG_NONE)
		{
			pixelFog(r, fog, z, rhw);

			fog = Min(fog, Float4(1.0f, 1.0f, 1.0f, 1.0f));
			fog = Max(fog, Float4(0.0f, 0.0f, 0.0f, 0.0f));
		}

		c0.r -= *Pointer<Float4>(r.data + OFFSET(DrawData,fog.colorF[0]));
		c0.g -= *Pointer<Float4>(r.data + OFFSET(DrawData,fog.colorF[1]));
		c0.b -= *Pointer<Float4>(r.data + OFFSET(DrawData,fog.colorF[2]));

		c0.r *= fog;
		c0.g *= fog;
		c0.b *= fog;

		c0.r += *Pointer<Float4>(r.data + OFFSET(DrawData,fog.colorF[0]));
		c0.g += *Pointer<Float4>(r.data + OFFSET(DrawData,fog.colorF[1]));
		c0.b += *Pointer<Float4>(r.data + OFFSET(DrawData,fog.colorF[2]));
	}

	void PixelRoutine::pixelFog(Registers &r, Float4 &visibility, Float4 &z, Float4 &rhw)
	{
		Float4 &zw = visibility;

		if(state.pixelFogMode != Context::FOG_NONE)
		{
			if(state.wBasedFog)
			{
				zw = rhw;
			}
			else
			{
				if(complementaryDepthBuffer)
				{
					zw = Float4(1.0f, 1.0f, 1.0f, 1.0f) - z;
				}
				else
				{
					zw = z;
				}
			}
		}

		switch(state.pixelFogMode)
		{
		case Context::FOG_NONE:
			break;
		case Context::FOG_LINEAR:
			zw *= *Pointer<Float4>(r.data + OFFSET(DrawData,fog.scale));
			zw += *Pointer<Float4>(r.data + OFFSET(DrawData,fog.offset));
			break;
		case Context::FOG_EXP:
			zw *= *Pointer<Float4>(r.data + OFFSET(DrawData,fog.densityE));
			zw = exponential(zw, true);
			break;
		case Context::FOG_EXP2:
			zw *= *Pointer<Float4>(r.data + OFFSET(DrawData,fog.densityE2));
			zw *= zw;
			zw = exponential(zw, true);
			zw = Rcp_pp(zw);
			break;
		default:
			ASSERT(false);
		}
	}

	void PixelRoutine::specularPixel(Color4i &current, Color4i &specular)
	{
		if(!state.specularAdd)
		{
			return;
		}

		current.r = AddSat(current.r, specular.r);
		current.g = AddSat(current.g, specular.g);
		current.b = AddSat(current.b, specular.b);
	}

	void PixelRoutine::writeDepth(Registers &r, Pointer<Byte> &zBuffer, int q, Int &x, Float4 &z, Int &zMask)
	{
		if(!state.depthWriteEnable)
		{
			return;
		}

		Float4 Z = z;

		if(pixelShader && pixelShader->depthOverride())
		{
			if(complementaryDepthBuffer)
			{
				Z = Float4(1, 1, 1, 1) - r.oDepth;
			}
			else
			{
				Z = r.oDepth;
			}
		}

		Pointer<Byte> buffer;
		Int pitch;

		if(!state.quadLayoutDepthBuffer)
		{	
			buffer = zBuffer + 4 * x;
			pitch = *Pointer<Int>(r.data + OFFSET(DrawData,depthPitchB));
		}
		else
		{	
			buffer = zBuffer + 8 * x;
		}

		if(q > 0)
		{
			buffer += q * *Pointer<Int>(r.data + OFFSET(DrawData,depthSliceB));
		}

		Float4 zValue;

		if(state.depthCompareMode != Context::DEPTH_NEVER || (state.depthCompareMode != Context::DEPTH_ALWAYS && !state.depthWriteEnable))
		{
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
		}

		Z = As<Float4>(As<Int4>(Z) & *Pointer<Int4>(r.constants + OFFSET(Constants,maskD4X) + zMask * 16, 16));
		zValue = As<Float4>(As<Int4>(zValue) & *Pointer<Int4>(r.constants + OFFSET(Constants,invMaskD4X) + zMask * 16, 16));
		Z = As<Float4>(As<Int4>(Z) | As<Int4>(zValue));

		if(!state.quadLayoutDepthBuffer)
		{
			// FIXME: Properly optimizes?
			*Pointer<Float2>(buffer) = Float2(Z.xy);
			*Pointer<Float2>(buffer + pitch) = Float2(Z.zw);
		}
		else
		{
			*Pointer<Float4>(buffer, 16) = Z;
		}
	}

	void PixelRoutine::writeStencil(Registers &r, Pointer<Byte> &sBuffer, int q, Int &x, Int &sMask, Int &zMask, Int &cMask)
	{
		if(!state.stencilActive)
		{
			return;
		}

		if(state.stencilPassOperation == Context::OPERATION_KEEP && state.stencilZFailOperation == Context::OPERATION_KEEP && state.stencilFailOperation == Context::OPERATION_KEEP)
		{
			if(!state.twoSidedStencil || (state.stencilPassOperationCCW == Context::OPERATION_KEEP && state.stencilZFailOperationCCW == Context::OPERATION_KEEP && state.stencilFailOperationCCW == Context::OPERATION_KEEP))
			{
				return;
			}
		}

		if(state.stencilWriteMasked && (!state.twoSidedStencil || state.stencilWriteMaskedCCW))
		{
			return;
		}

		Pointer<Byte> buffer = sBuffer + 2 * x;

		if(q > 0)
		{
			buffer += q * *Pointer<Int>(r.data + OFFSET(DrawData,stencilSliceB));
		}

		Byte8 bufferValue = As<Byte8>(Long1(*Pointer<UInt>(buffer)));
	
		Byte8 newValue;
		stencilOperation(r, newValue, bufferValue, (Context::StencilOperation)state.stencilPassOperation, (Context::StencilOperation)state.stencilZFailOperation, (Context::StencilOperation)state.stencilFailOperation, false, zMask, sMask);

		if(!state.noStencilWriteMask)
		{
			Byte8 maskedValue = bufferValue;
			newValue &= *Pointer<Byte8>(r.data + OFFSET(DrawData,stencil[0].writeMaskQ));
			maskedValue &= *Pointer<Byte8>(r.data + OFFSET(DrawData,stencil[0].invWriteMaskQ));
			newValue |= maskedValue;
		}

		if(state.twoSidedStencil)
		{
			Byte8 newValueCCW;

			stencilOperation(r, newValueCCW, bufferValue, (Context::StencilOperation)state.stencilPassOperationCCW, (Context::StencilOperation)state.stencilZFailOperationCCW, (Context::StencilOperation)state.stencilFailOperationCCW, true, zMask, sMask);

			if(!state.noStencilWriteMaskCCW)
			{
				Byte8 maskedValue = bufferValue;
				newValueCCW &= *Pointer<Byte8>(r.data + OFFSET(DrawData,stencil[1].writeMaskQ));
				maskedValue &= *Pointer<Byte8>(r.data + OFFSET(DrawData,stencil[1].invWriteMaskQ));
				newValueCCW |= maskedValue;
			}

			newValue &= *Pointer<Byte8>(r.primitive + OFFSET(Primitive,clockwiseMask));
			newValueCCW &= *Pointer<Byte8>(r.primitive + OFFSET(Primitive,invClockwiseMask));
			newValue |= newValueCCW;
		}

		newValue &= *Pointer<Byte8>(r.constants + OFFSET(Constants,maskB4Q) + 8 * cMask);
		bufferValue &= *Pointer<Byte8>(r.constants + OFFSET(Constants,invMaskB4Q) + 8 * cMask);
		newValue |= bufferValue;

		*Pointer<UInt>(buffer) = UInt(As<Long>(newValue));
	}

	void PixelRoutine::stencilOperation(Registers &r, Byte8 &newValue, Byte8 &bufferValue, Context::StencilOperation stencilPassOperation, Context::StencilOperation stencilZFailOperation, Context::StencilOperation stencilFailOperation, bool CCW, Int &zMask, Int &sMask)
	{
		Byte8 &pass = newValue;
		Byte8 fail;
		Byte8 zFail;

		stencilOperation(r, pass, bufferValue, stencilPassOperation, CCW);

		if(stencilZFailOperation != stencilPassOperation)
		{
			stencilOperation(r, zFail, bufferValue, stencilZFailOperation, CCW);
		}

		if(stencilFailOperation != stencilPassOperation || stencilFailOperation != stencilZFailOperation)
		{
			stencilOperation(r, fail, bufferValue, stencilFailOperation, CCW);
		}

		if(stencilFailOperation != stencilPassOperation || stencilFailOperation != stencilZFailOperation)
		{
			if(state.depthTestActive && stencilZFailOperation != stencilPassOperation)   // zMask valid and values not the same
			{
				pass &= *Pointer<Byte8>(r.constants + OFFSET(Constants,maskB4Q) + 8 * zMask);
				zFail &= *Pointer<Byte8>(r.constants + OFFSET(Constants,invMaskB4Q) + 8 * zMask);
				pass |= zFail;
			}

			pass &= *Pointer<Byte8>(r.constants + OFFSET(Constants,maskB4Q) + 8 * sMask);
			fail &= *Pointer<Byte8>(r.constants + OFFSET(Constants,invMaskB4Q) + 8 * sMask);
			pass |= fail;
		}
	}

	void PixelRoutine::stencilOperation(Registers &r, Byte8 &output, Byte8 &bufferValue, Context::StencilOperation operation, bool CCW)
	{
		switch(operation)
		{
		case Context::OPERATION_KEEP:
			output = bufferValue;
			break;
		case Context::OPERATION_ZERO:
			output = Byte8(0x0000000000000000);
			break;
		case Context::OPERATION_REPLACE:
			output = *Pointer<Byte8>(r.data + OFFSET(DrawData,stencil[CCW].referenceQ));
			break;
		case Context::OPERATION_INCRSAT:
			output = AddSat(bufferValue, Byte8(1, 1, 1, 1, 1, 1, 1, 1));
			break;
		case Context::OPERATION_DECRSAT:
			output = SubSat(bufferValue, Byte8(1, 1, 1, 1, 1, 1, 1, 1));
			break;
		case Context::OPERATION_INVERT:
			output = bufferValue ^ Byte8(0xFFFFFFFFFFFFFFFF);
			break;
		case Context::OPERATION_INCR:
			output = bufferValue + Byte8(1, 1, 1, 1, 1, 1, 1, 1);
			break;
		case Context::OPERATION_DECR:
			output = bufferValue - Byte8(1, 1, 1, 1, 1, 1, 1, 1);
			break;
		default:
			ASSERT(false);
		}
	}

	void PixelRoutine::sampleTexture(Registers &r, Color4i &c, int coordinates, int stage, bool project)
	{
		Float4 u = r.vx[2 + coordinates];
		Float4 v = r.vy[2 + coordinates];
		Float4 w = r.vz[2 + coordinates];
		Float4 q = r.vw[2 + coordinates];

		if(perturbate)
		{
			u += r.du;
			v += r.dv;

			perturbate = false;
		}

		sampleTexture(r, c, stage, u, v, w, q, project);
	}

	void PixelRoutine::sampleTexture(Registers &r, Color4i &c, int stage, Float4 &u, Float4 &v, Float4 &w, Float4 &q, bool project, bool bias, bool fixed12)
	{
		Color4f dsx;
		Color4f dsy;

		sampleTexture(r, c, stage, u, v, w, q, dsx, dsy, project, bias, fixed12, false);
	}

	void PixelRoutine::sampleTexture(Registers &r, Color4i &c, int stage, Float4 &u, Float4 &v, Float4 &w, Float4 &q, Color4f &dsx, Color4f &dsy, bool project, bool bias, bool fixed12, bool gradients, bool lodProvided)
	{
		#if PERF_PROFILE
			Long texTime = Ticks();
		#endif

		Pointer<Byte> texture = r.data + OFFSET(DrawData,mipmap) + stage * sizeof(Texture);

		if(!project)
		{
			sampler[stage]->sampleTexture(texture, c, u, v, w, q, dsx, dsy, bias, fixed12, gradients, lodProvided);
		}
		else
		{
			Float4 rq = reciprocal(q);

			Float4 u_q = u * rq;
			Float4 v_q = v * rq;
			Float4 w_q = w * rq;

			sampler[stage]->sampleTexture(texture, c, u_q, v_q, w_q, q, dsx, dsy, bias, fixed12, gradients, lodProvided);
		}

		#if PERF_PROFILE
			r.cycles[PERF_TEX] += Ticks() - texTime;
		#endif
	}

	void PixelRoutine::sampleTexture(Registers &r, Color4f &c, int stage, Float4 &u, Float4 &v, Float4 &w, Float4 &q, Color4f &dsx, Color4f &dsy, bool project, bool bias, bool gradients, bool lodProvided)
	{
		#if PERF_PROFILE
			Long texTime = Ticks();
		#endif

		Pointer<Byte> texture = r.data + OFFSET(DrawData,mipmap) + stage * sizeof(Texture);

		if(!project)
		{
			sampler[stage]->sampleTexture(texture, c, u, v, w, q, dsx, dsy, bias, gradients, lodProvided);
		}
		else
		{
			Float4 rq = reciprocal(q);

			Float4 u_q = u * rq;
			Float4 v_q = v * rq;
			Float4 w_q = w * rq;

			sampler[stage]->sampleTexture(texture, c, u_q, v_q, w_q, q, dsx, dsy, bias, gradients, lodProvided);
		}

		#if PERF_PROFILE
			r.cycles[PERF_TEX] += Ticks() - texTime;
		#endif
	}

	void PixelRoutine::clampColor(Color4f oC[4])
	{
		for(int index = 0; index < 4; index++)
		{
			if(!state.colorWriteActive(index) && !(index == 0 && state.alphaTestActive()))
			{
				continue;
			}

			switch(state.targetFormat[index])
			{
			case FORMAT_NULL:
				break;
			case FORMAT_A16B16G16R16:
			case FORMAT_A8R8G8B8:
			case FORMAT_X8R8G8B8:
			case FORMAT_G16R16:
				oC[index].r = Max(oC[index].r, Float4(0.0f, 0.0f, 0.0f, 0.0f)); oC[index].r = Min(oC[index].r, Float4(1.0f, 1.0f, 1.0f, 1.0f));
				oC[index].g = Max(oC[index].g, Float4(0.0f, 0.0f, 0.0f, 0.0f)); oC[index].g = Min(oC[index].g, Float4(1.0f, 1.0f, 1.0f, 1.0f));
				oC[index].b = Max(oC[index].b, Float4(0.0f, 0.0f, 0.0f, 0.0f)); oC[index].b = Min(oC[index].b, Float4(1.0f, 1.0f, 1.0f, 1.0f));
				oC[index].a = Max(oC[index].a, Float4(0.0f, 0.0f, 0.0f, 0.0f)); oC[index].a = Min(oC[index].a, Float4(1.0f, 1.0f, 1.0f, 1.0f));
				break;
			case FORMAT_R32F:
			case FORMAT_G32R32F:
			case FORMAT_A32B32G32R32F:
				break;
			default:
				ASSERT(false);
			}
		}
	}

	void PixelRoutine::rasterOperation(Color4i &current, Registers &r, Float4 &fog, Pointer<Byte> &cBuffer, Int &x, Int sMask[4], Int zMask[4], Int cMask[4])
	{
		if(!state.colorWriteActive(0))
		{
			return;
		}

		Color4f oC;

		switch(state.targetFormat[0])
		{
		case FORMAT_X8R8G8B8:
		case FORMAT_A8R8G8B8:
		case FORMAT_G16R16:
		case FORMAT_A16B16G16R16:
			if(!postBlendSRGB && state.writeSRGB)
			{
				linearToSRGB12_16(r, current);
			}
			else
			{
				current.r <<= 4;
				current.g <<= 4;
				current.b <<= 4;
				current.a <<= 4;
			}

			fogBlend(r, current, fog, r.z[0], r.rhw);

			for(unsigned int q = 0; q < state.multiSample; q++)
			{
				Pointer<Byte> buffer = cBuffer + q * *Pointer<Int>(r.data + OFFSET(DrawData,colorSliceB[0]));
				Color4i color = current;

				if(state.multiSampleMask & (1 << q))
				{
					alphaBlend(r, 0, buffer, color, x);
					writeColor(r, 0, buffer, x, color, sMask[q], zMask[q], cMask[q]);
				}
			}
			break;
		case FORMAT_R32F:
		case FORMAT_G32R32F:
		case FORMAT_A32B32G32R32F:
			convertSigned12(oC, current);
			fogBlend(r, oC, fog, r.z[0], r.rhw);
			
			for(unsigned int q = 0; q < state.multiSample; q++)
			{
				Pointer<Byte> buffer = cBuffer + q * *Pointer<Int>(r.data + OFFSET(DrawData,colorSliceB[0]));
				Color4f color = oC;

				if(state.multiSampleMask & (1 << q))
				{
					alphaBlend(r, 0, buffer, color, x);
					writeColor(r, 0, buffer, x, color, sMask[q], zMask[q], cMask[q]);
				}
			}
			break;
		default:
			ASSERT(false);
		}
	}

	void PixelRoutine::rasterOperation(Color4f oC[4], Registers &r, Float4 &fog, Pointer<Byte> cBuffer[4], Int &x, Int sMask[4], Int zMask[4], Int cMask[4])
	{
		for(int index = 0; index < 4; index++)
		{
			if(!state.colorWriteActive(index))
			{
				continue;
			}

			if(!postBlendSRGB && state.writeSRGB)
			{
				oC[index].r = linearToSRGB(oC[index].r);
				oC[index].g = linearToSRGB(oC[index].g);
				oC[index].b = linearToSRGB(oC[index].b);
			}

			if(index == 0)
			{
				fogBlend(r, oC[index], fog, r.z[0], r.rhw);
			}

			switch(state.targetFormat[index])
			{
			case FORMAT_X8R8G8B8:
			case FORMAT_A8R8G8B8:
			case FORMAT_G16R16:
			case FORMAT_A16B16G16R16:
				for(unsigned int q = 0; q < state.multiSample; q++)
				{
					Pointer<Byte> buffer = cBuffer[index] + q * *Pointer<Int>(r.data + OFFSET(DrawData,colorSliceB[index]));
					Color4i color;

					color.r = convertFixed16(oC[index].r, false);
					color.g = convertFixed16(oC[index].g, false);
					color.b = convertFixed16(oC[index].b, false);
					color.a = convertFixed16(oC[index].a, false);

					if(state.multiSampleMask & (1 << q))
					{
						alphaBlend(r, index, buffer, color, x);
						writeColor(r, index, buffer, x, color, sMask[q], zMask[q], cMask[q]);
					}
				}
				break;
			case FORMAT_R32F:
			case FORMAT_G32R32F:
			case FORMAT_A32B32G32R32F:
				for(unsigned int q = 0; q < state.multiSample; q++)
				{
					Pointer<Byte> buffer = cBuffer[index] + q * *Pointer<Int>(r.data + OFFSET(DrawData,colorSliceB[index]));
					Color4f color = oC[index];

					if(state.multiSampleMask & (1 << q))
					{
						alphaBlend(r, index, buffer, color, x);
						writeColor(r, index, buffer, x, color, sMask[q], zMask[q], cMask[q]);
					}
				}
				break;
			default:
				ASSERT(false);
			}
		}
	}

	void PixelRoutine::blendFactor(Registers &r, const Color4i &blendFactor, const Color4i &current, const Color4i &pixel, Context::BlendFactor blendFactorActive)
	{
		switch(blendFactorActive)
		{
		case Context::BLEND_ZERO:
			// Optimized
			break;
		case Context::BLEND_ONE:
			// Optimized
			break;
		case Context::BLEND_SOURCE:
			blendFactor.r = current.r;
			blendFactor.g = current.g;
			blendFactor.b = current.b;
			break;
		case Context::BLEND_INVSOURCE:
			blendFactor.r = Short4(0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF) - current.r;
			blendFactor.g = Short4(0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF) - current.g;
			blendFactor.b = Short4(0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF) - current.b;
			break;
		case Context::BLEND_DEST:
			blendFactor.r = pixel.r;
			blendFactor.g = pixel.g;
			blendFactor.b = pixel.b;
			break;
		case Context::BLEND_INVDEST:
			blendFactor.r = Short4(0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF) - pixel.r;
			blendFactor.g = Short4(0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF) - pixel.g;
			blendFactor.b = Short4(0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF) - pixel.b;
			break;
		case Context::BLEND_SOURCEALPHA:
			blendFactor.r = current.a;
			blendFactor.g = current.a;
			blendFactor.b = current.a;
			break;
		case Context::BLEND_INVSOURCEALPHA:
			blendFactor.r = Short4(0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF) - current.a;
			blendFactor.g = Short4(0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF) - current.a;
			blendFactor.b = Short4(0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF) - current.a;
			break;
		case Context::BLEND_DESTALPHA:
			blendFactor.r = pixel.a;
			blendFactor.g = pixel.a;
			blendFactor.b = pixel.a;
			break;
		case Context::BLEND_INVDESTALPHA:
			blendFactor.r = Short4(0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF) - pixel.a;
			blendFactor.g = Short4(0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF) - pixel.a;
			blendFactor.b = Short4(0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF) - pixel.a;
			break;
		case Context::BLEND_SRCALPHASAT:
			blendFactor.r = Short4(0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF) - pixel.a;
			blendFactor.r = Min(As<UShort4>(blendFactor.r), As<UShort4>(current.a));
			blendFactor.g = blendFactor.r;
			blendFactor.b = blendFactor.r;
			break;
		case Context::BLEND_CONSTANT:
			blendFactor.r = *Pointer<Short4>(r.data + OFFSET(DrawData,factor.blendConstant4W[0]));
			blendFactor.g = *Pointer<Short4>(r.data + OFFSET(DrawData,factor.blendConstant4W[1]));
			blendFactor.b = *Pointer<Short4>(r.data + OFFSET(DrawData,factor.blendConstant4W[2]));
			break;
		case Context::BLEND_INVCONSTANT:
			blendFactor.r = *Pointer<Short4>(r.data + OFFSET(DrawData,factor.invBlendConstant4W[0]));
			blendFactor.g = *Pointer<Short4>(r.data + OFFSET(DrawData,factor.invBlendConstant4W[1]));
			blendFactor.b = *Pointer<Short4>(r.data + OFFSET(DrawData,factor.invBlendConstant4W[2]));
			break;
		case Context::BLEND_CONSTANTALPHA:
			blendFactor.r = *Pointer<Short4>(r.data + OFFSET(DrawData,factor.blendConstant4W[3]));
			blendFactor.g = *Pointer<Short4>(r.data + OFFSET(DrawData,factor.blendConstant4W[3]));
			blendFactor.b = *Pointer<Short4>(r.data + OFFSET(DrawData,factor.blendConstant4W[3]));
			break;
		case Context::BLEND_INVCONSTANTALPHA:
			blendFactor.r = *Pointer<Short4>(r.data + OFFSET(DrawData,factor.invBlendConstant4W[3]));
			blendFactor.g = *Pointer<Short4>(r.data + OFFSET(DrawData,factor.invBlendConstant4W[3]));
			blendFactor.b = *Pointer<Short4>(r.data + OFFSET(DrawData,factor.invBlendConstant4W[3]));
			break;
		default:
			ASSERT(false);
		}
	}
	
	void PixelRoutine::blendFactorAlpha(Registers &r, const Color4i &blendFactor, const Color4i &current, const Color4i &pixel, Context::BlendFactor blendFactorAlphaActive)
	{
		switch(blendFactorAlphaActive)
		{
		case Context::BLEND_ZERO:
			// Optimized
			break;
		case Context::BLEND_ONE:
			// Optimized
			break;
		case Context::BLEND_SOURCE:
			blendFactor.a = current.a;
			break;
		case Context::BLEND_INVSOURCE:
			blendFactor.a = Short4(0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF) - current.a;
			break;
		case Context::BLEND_DEST:
			blendFactor.a = pixel.a;
			break;
		case Context::BLEND_INVDEST:
			blendFactor.a = Short4(0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF) - pixel.a;
			break;
		case Context::BLEND_SOURCEALPHA:
			blendFactor.a = current.a;
			break;
		case Context::BLEND_INVSOURCEALPHA:
			blendFactor.a = Short4(0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF) - current.a;
			break;
		case Context::BLEND_DESTALPHA:
			blendFactor.a = pixel.a;
			break;
		case Context::BLEND_INVDESTALPHA:
			blendFactor.a = Short4(0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF) - pixel.a;
			break;
		case Context::BLEND_SRCALPHASAT:
			blendFactor.a = Short4(0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF);
			break;
		case Context::BLEND_CONSTANT:
		case Context::BLEND_CONSTANTALPHA:
			blendFactor.a = *Pointer<Short4>(r.data + OFFSET(DrawData,factor.blendConstant4W[3]));
			break;
		case Context::BLEND_INVCONSTANT:
		case Context::BLEND_INVCONSTANTALPHA:
			blendFactor.a = *Pointer<Short4>(r.data + OFFSET(DrawData,factor.invBlendConstant4W[3]));
			break;
		default:
			ASSERT(false);
		}
	}

	void PixelRoutine::alphaBlend(Registers &r, int index, Pointer<Byte> &cBuffer, Color4i &current, Int &x)
	{
		if(!state.alphaBlendActive)
		{
			return;
		}
		 
		Pointer<Byte> buffer;

		Color4i pixel;
		Short4 c01;
		Short4 c23;

		// Read pixel
		switch(state.targetFormat[index])
		{
		case FORMAT_A8R8G8B8:
			buffer = cBuffer + 4 * x;
			c01 = *Pointer<Short4>(buffer);
			buffer += *Pointer<Int>(r.data + OFFSET(DrawData,colorPitchB[index]));
			c23 = *Pointer<Short4>(buffer);
			pixel.b = c01;
			pixel.g = c01;
			pixel.b = UnpackLow(As<Byte8>(pixel.b), As<Byte8>(c23));
			pixel.g = UnpackHigh(As<Byte8>(pixel.g), As<Byte8>(c23));
			pixel.r = pixel.b;
			pixel.b = UnpackLow(As<Byte8>(pixel.b), As<Byte8>(pixel.g));
			pixel.r = UnpackHigh(As<Byte8>(pixel.r), As<Byte8>(pixel.g));
			pixel.g = pixel.b;
			pixel.a = pixel.r;
			pixel.r = UnpackLow(As<Byte8>(pixel.r), As<Byte8>(pixel.r));
			pixel.g = UnpackHigh(As<Byte8>(pixel.g), As<Byte8>(pixel.g));
			pixel.b = UnpackLow(As<Byte8>(pixel.b), As<Byte8>(pixel.b));
			pixel.a = UnpackHigh(As<Byte8>(pixel.a), As<Byte8>(pixel.a));
			break;
		case FORMAT_X8R8G8B8:
			buffer = cBuffer + 4 * x;
			c01 = *Pointer<Short4>(buffer);
			buffer += *Pointer<Int>(r.data + OFFSET(DrawData,colorPitchB[index]));
			c23 = *Pointer<Short4>(buffer);
			pixel.b = c01;
			pixel.g = c01;
			pixel.b = UnpackLow(As<Byte8>(pixel.b), As<Byte8>(c23));
			pixel.g = UnpackHigh(As<Byte8>(pixel.g), As<Byte8>(c23));
			pixel.r = pixel.b;
			pixel.b = UnpackLow(As<Byte8>(pixel.b), As<Byte8>(pixel.g));
			pixel.r = UnpackHigh(As<Byte8>(pixel.r), As<Byte8>(pixel.g));
			pixel.g = pixel.b;
			pixel.r = UnpackLow(As<Byte8>(pixel.r), As<Byte8>(pixel.r));
			pixel.g = UnpackHigh(As<Byte8>(pixel.g), As<Byte8>(pixel.g));
			pixel.b = UnpackLow(As<Byte8>(pixel.b), As<Byte8>(pixel.b));
			pixel.a = Short4(0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF);
			break;
		case FORMAT_A8G8R8B8Q:
			UNIMPLEMENTED();
		//	pixel.b = UnpackLow(As<Byte8>(pixel.b), *Pointer<Byte8>(cBuffer + 8 * x + 0));
		//	pixel.r = UnpackHigh(As<Byte8>(pixel.r), *Pointer<Byte8>(cBuffer + 8 * x + 0));
		//	pixel.g = UnpackLow(As<Byte8>(pixel.g), *Pointer<Byte8>(cBuffer + 8 * x + 8));
		//	pixel.a = UnpackHigh(As<Byte8>(pixel.a), *Pointer<Byte8>(cBuffer + 8 * x + 8));
			break;
		case FORMAT_X8G8R8B8Q:
			UNIMPLEMENTED();
		//	pixel.b = UnpackLow(As<Byte8>(pixel.b), *Pointer<Byte8>(cBuffer + 8 * x + 0));
		//	pixel.r = UnpackHigh(As<Byte8>(pixel.r), *Pointer<Byte8>(cBuffer + 8 * x + 0));
		//	pixel.g = UnpackLow(As<Byte8>(pixel.g), *Pointer<Byte8>(cBuffer + 8 * x + 8));
		//	pixel.a = Short4(0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF);
			break;
		case FORMAT_A16B16G16R16:
			buffer  = cBuffer;
			pixel.r = *Pointer<Short4>(buffer + 8 * x);
			pixel.g = *Pointer<Short4>(buffer + 8 * x + 8);
			buffer += *Pointer<Int>(r.data + OFFSET(DrawData,colorPitchB[index]));
			pixel.b = *Pointer<Short4>(buffer + 8 * x);
			pixel.a = *Pointer<Short4>(buffer + 8 * x + 8);
			transpose4x4(pixel.r, pixel.g, pixel.b, pixel.a);
			break;
		case FORMAT_G16R16:
			buffer = cBuffer;
			pixel.r = *Pointer<Short4>(buffer  + 4 * x);
			buffer += *Pointer<Int>(r.data + OFFSET(DrawData,colorPitchB[index]));
			pixel.g = *Pointer<Short4>(buffer  + 4 * x);
			pixel.b = pixel.r;
			pixel.r = As<Short4>(UnpackLow(pixel.r, pixel.g));
			pixel.b = As<Short4>(UnpackHigh(pixel.b, pixel.g));
			pixel.g = pixel.b;
			pixel.r = As<Short4>(UnpackLow(pixel.r, pixel.b));
			pixel.g = As<Short4>(UnpackHigh(pixel.g, pixel.b));
			pixel.b = Short4(0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF);
			pixel.a = Short4(0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF);
			break;
		default:
			ASSERT(false);
		}

		if(postBlendSRGB && state.writeSRGB)
		{
			sRGBtoLinear16_16(r, pixel);	
		}

		// Final Color = ObjectColor * SourceBlendFactor + PixelColor * DestinationBlendFactor
		Color4i sourceFactor;
		Color4i destFactor;

		blendFactor(r, sourceFactor, current, pixel, (Context::BlendFactor)state.sourceBlendFactor);
		blendFactor(r, destFactor, current, pixel, (Context::BlendFactor)state.destBlendFactor);

		if(state.sourceBlendFactor != Context::BLEND_ONE && state.sourceBlendFactor != Context::BLEND_ZERO)
		{
			current.r = MulHigh(As<UShort4>(current.r), As<UShort4>(sourceFactor.r));
			current.g = MulHigh(As<UShort4>(current.g), As<UShort4>(sourceFactor.g));
			current.b = MulHigh(As<UShort4>(current.b), As<UShort4>(sourceFactor.b));
		}
	
		if(state.destBlendFactor != Context::BLEND_ONE && state.destBlendFactor != Context::BLEND_ZERO)
		{
			pixel.r = MulHigh(As<UShort4>(pixel.r), As<UShort4>(destFactor.r));
			pixel.g = MulHigh(As<UShort4>(pixel.g), As<UShort4>(destFactor.g));
			pixel.b = MulHigh(As<UShort4>(pixel.b), As<UShort4>(destFactor.b));
		}

		switch(state.blendOperation)
		{
		case Context::BLENDOP_ADD:
			current.r = AddSat(As<UShort4>(current.r), As<UShort4>(pixel.r));
			current.g = AddSat(As<UShort4>(current.g), As<UShort4>(pixel.g));
			current.b = AddSat(As<UShort4>(current.b), As<UShort4>(pixel.b));
			break;
		case Context::BLENDOP_SUB:
			current.r = SubSat(As<UShort4>(current.r), As<UShort4>(pixel.r));
			current.g = SubSat(As<UShort4>(current.g), As<UShort4>(pixel.g));
			current.b = SubSat(As<UShort4>(current.b), As<UShort4>(pixel.b));
			break;
		case Context::BLENDOP_INVSUB:
			current.r = SubSat(As<UShort4>(pixel.r), As<UShort4>(current.r));
			current.g = SubSat(As<UShort4>(pixel.g), As<UShort4>(current.g));
			current.b = SubSat(As<UShort4>(pixel.b), As<UShort4>(current.b));
			break;
		case Context::BLENDOP_MIN:
			current.r = Min(As<UShort4>(current.r), As<UShort4>(pixel.r));
			current.g = Min(As<UShort4>(current.g), As<UShort4>(pixel.g));
			current.b = Min(As<UShort4>(current.b), As<UShort4>(pixel.b));
			break;
		case Context::BLENDOP_MAX:
			current.r = Max(As<UShort4>(current.r), As<UShort4>(pixel.r));
			current.g = Max(As<UShort4>(current.g), As<UShort4>(pixel.g));
			current.b = Max(As<UShort4>(current.b), As<UShort4>(pixel.b));
			break;
		case Context::BLENDOP_SOURCE:
			// No operation
			break;
		case Context::BLENDOP_DEST:
			current.r = pixel.r;
			current.g = pixel.g;
			current.b = pixel.b;
			break;
		case Context::BLENDOP_NULL:
			current.r = Short4(0x0000, 0x0000, 0x0000, 0x0000);
			current.g = Short4(0x0000, 0x0000, 0x0000, 0x0000);
			current.b = Short4(0x0000, 0x0000, 0x0000, 0x0000);
			break;
		default:
			ASSERT(false);
		}

		blendFactorAlpha(r, sourceFactor, current, pixel, (Context::BlendFactor)state.sourceBlendFactorAlpha);
		blendFactorAlpha(r, destFactor, current, pixel, (Context::BlendFactor)state.destBlendFactorAlpha);

		if(state.sourceBlendFactorAlpha != Context::BLEND_ONE && state.sourceBlendFactorAlpha != Context::BLEND_ZERO)
		{
			current.a = MulHigh(As<UShort4>(current.a), As<UShort4>(sourceFactor.a));
		}
	
		if(state.destBlendFactorAlpha != Context::BLEND_ONE && state.destBlendFactorAlpha != Context::BLEND_ZERO)
		{
			pixel.a = MulHigh(As<UShort4>(pixel.a), As<UShort4>(destFactor.a));
		}

		switch(state.blendOperationAlpha)
		{
		case Context::BLENDOP_ADD:
			current.a = AddSat(As<UShort4>(current.a), As<UShort4>(pixel.a));
			break;
		case Context::BLENDOP_SUB:
			current.a = SubSat(As<UShort4>(current.a), As<UShort4>(pixel.a));
			break;
		case Context::BLENDOP_INVSUB:
			current.a = SubSat(As<UShort4>(pixel.a), As<UShort4>(current.a));
			break;
		case Context::BLENDOP_MIN:
			current.a = Min(As<UShort4>(current.a), As<UShort4>(pixel.a));
			break;
		case Context::BLENDOP_MAX:
			current.a = Max(As<UShort4>(current.a), As<UShort4>(pixel.a));
			break;
		case Context::BLENDOP_SOURCE:
			// No operation
			break;
		case Context::BLENDOP_DEST:
			current.a = pixel.a;
			break;
		case Context::BLENDOP_NULL:
			current.a = Short4(0x0000, 0x0000, 0x0000, 0x0000);
			break;
		default:
			ASSERT(false);
		}
	}

	void PixelRoutine::writeColor(Registers &r, int index, Pointer<Byte> &cBuffer, Int &x, Color4i &current, Int &sMask, Int &zMask, Int &cMask)
	{
		if(!state.colorWriteActive(index))
		{
			return;
		}

		if(postBlendSRGB && state.writeSRGB)
		{
			linearToSRGB16_16(r, current);
		}

		if(exactColorRounding)
		{
			switch(state.targetFormat[index])
			{
			case FORMAT_X8G8R8B8Q:
			case FORMAT_A8G8R8B8Q:
			case FORMAT_X8R8G8B8:
			case FORMAT_A8R8G8B8:
				{
					current.r = current.r - As<Short4>(As<UShort4>(current.r) >> 8) + Short4(0x0080, 0x0080, 0x0080, 0x0080);
					current.g = current.g - As<Short4>(As<UShort4>(current.g) >> 8) + Short4(0x0080, 0x0080, 0x0080, 0x0080);
					current.b = current.b - As<Short4>(As<UShort4>(current.b) >> 8) + Short4(0x0080, 0x0080, 0x0080, 0x0080);
					current.a = current.a - As<Short4>(As<UShort4>(current.a) >> 8) + Short4(0x0080, 0x0080, 0x0080, 0x0080);
				}
				break;
			}
		}

		int rgbaWriteMask = state.colorWriteActive(index);
		int bgraWriteMask = rgbaWriteMask & 0x0000000A | (rgbaWriteMask & 0x00000001) << 2 | (rgbaWriteMask & 0x00000004) >> 2;
		int brgaWriteMask = rgbaWriteMask & 0x00000008 | (rgbaWriteMask & 0x00000001) << 1 | (rgbaWriteMask & 0x00000002) << 1 | (rgbaWriteMask & 0x00000004) >> 2;

		switch(state.targetFormat[index])
		{
		case FORMAT_X8G8R8B8Q:
			UNIMPLEMENTED();
		//	current.r = As<Short4>(As<UShort4>(current.r) >> 8);
		//	current.g = As<Short4>(As<UShort4>(current.g) >> 8);
		//	current.b = As<Short4>(As<UShort4>(current.b) >> 8);

		//	current.b = As<Short4>(Pack(As<UShort4>(current.b), As<UShort4>(current.r)));
		//	current.g = As<Short4>(Pack(As<UShort4>(current.g), As<UShort4>(current.g)));
			break;
		case FORMAT_A8G8R8B8Q:
			UNIMPLEMENTED();
		//	current.r = As<Short4>(As<UShort4>(current.r) >> 8);
		//	current.g = As<Short4>(As<UShort4>(current.g) >> 8);
		//	current.b = As<Short4>(As<UShort4>(current.b) >> 8);
		//	current.a = As<Short4>(As<UShort4>(current.a) >> 8);

		//	current.b = As<Short4>(Pack(As<UShort4>(current.b), As<UShort4>(current.r)));
		//	current.g = As<Short4>(Pack(As<UShort4>(current.g), As<UShort4>(current.a)));
			break;
		case FORMAT_X8R8G8B8:
		case FORMAT_A8R8G8B8:
			if(state.targetFormat[index] == FORMAT_X8R8G8B8 || rgbaWriteMask == 0x7)
			{
				current.r = As<Short4>(As<UShort4>(current.r) >> 8);
				current.g = As<Short4>(As<UShort4>(current.g) >> 8);
				current.b = As<Short4>(As<UShort4>(current.b) >> 8);

				current.b = As<Short4>(Pack(As<UShort4>(current.b), As<UShort4>(current.r)));
				current.g = As<Short4>(Pack(As<UShort4>(current.g), As<UShort4>(current.g)));

				current.r = current.b;
				current.b = UnpackLow(As<Byte8>(current.b), As<Byte8>(current.g));
				current.r = UnpackHigh(As<Byte8>(current.r), As<Byte8>(current.g));
				current.g = current.b;
				current.b = As<Short4>(UnpackLow(current.b, current.r));
				current.g = As<Short4>(UnpackHigh(current.g, current.r));
			}
			else
			{
				current.r = As<Short4>(As<UShort4>(current.r) >> 8);
				current.g = As<Short4>(As<UShort4>(current.g) >> 8);
				current.b = As<Short4>(As<UShort4>(current.b) >> 8);
				current.a = As<Short4>(As<UShort4>(current.a) >> 8);

				current.b = As<Short4>(Pack(As<UShort4>(current.b), As<UShort4>(current.r)));
				current.g = As<Short4>(Pack(As<UShort4>(current.g), As<UShort4>(current.a)));

				current.r = current.b;
				current.b = UnpackLow(As<Byte8>(current.b), As<Byte8>(current.g));
				current.r = UnpackHigh(As<Byte8>(current.r), As<Byte8>(current.g));
				current.g = current.b;
				current.b = As<Short4>(UnpackLow(current.b, current.r));
				current.g = As<Short4>(UnpackHigh(current.g, current.r));
			}
			break;
		case FORMAT_G16R16:
			current.b = current.r;
			current.r = As<Short4>(UnpackLow(current.r, current.g));
			current.b = As<Short4>(UnpackHigh(current.b, current.g));
			current.g = current.b;
			break;
		case FORMAT_A16B16G16R16:
			transpose4x4(current.r, current.g, current.b, current.a);
			break;
		case FORMAT_R32F:
		case FORMAT_G32R32F:
		case FORMAT_A32B32G32R32F:
			{
				Color4f oC;

				oC.r = convertUnsigned16(UShort4(current.r));
				oC.g = convertUnsigned16(UShort4(current.g));
				oC.b = convertUnsigned16(UShort4(current.b));
				oC.a = convertUnsigned16(UShort4(current.a));

				writeColor(r, index, cBuffer, x, oC, sMask, zMask, cMask);
			}
			return;
		default:
			ASSERT(false);
		}

		Short4 c01 = current.b;
		Short4 c23 = current.g;

		Int xMask;   // Combination of all masks

		if(state.depthTestActive)
		{
			xMask = zMask;
		}
		else
		{
			xMask = cMask;
		}

		if(state.stencilActive)
		{
			xMask &= sMask;
		}

		Pointer<Byte> buffer;
		Short4 value;

		switch(state.targetFormat[index])
		{
		case FORMAT_A8G8R8B8Q:
		case FORMAT_X8G8R8B8Q:   // FIXME: Don't touch alpha?
			UNIMPLEMENTED();
		//	value = *Pointer<Short4>(cBuffer + 8 * x + 0);

		//	if((state.targetFormat[index] == FORMAT_A8G8R8B8Q && bgraWriteMask != 0x0000000F) ||
		//	   ((state.targetFormat[index] == FORMAT_X8G8R8B8Q && bgraWriteMask != 0x00000007) &&
		//	    (state.targetFormat[index] == FORMAT_X8G8R8B8Q && bgraWriteMask != 0x0000000F)))   // FIXME: Need for masking when XRGB && Fh?
		//	{
		//		Short4 masked = value;
		//		c01 &= *Pointer<Short4>(r.constants + OFFSET(Constants,maskB4Q[bgraWriteMask][0]));
		//		masked &= *Pointer<Short4>(r.constants + OFFSET(Constants,invMaskB4Q[bgraWriteMask][0]));
		//		c01 |= masked;
		//	}

		//	c01 &= *Pointer<Short4>(r.constants + OFFSET(Constants,maskD01Q) + xMask * 8);
		//	value &= *Pointer<Short4>(r.constants + OFFSET(Constants,invMaskD01Q) + xMask * 8);
		//	c01 |= value;
		//	*Pointer<Short4>(cBuffer + 8 * x + 0) = c01;

		//	value = *Pointer<Short4>(cBuffer + 8 * x + 8);

		//	if((state.targetFormat[index] == FORMAT_A8G8R8B8Q && bgraWriteMask != 0x0000000F) ||
		//	   ((state.targetFormat[index] == FORMAT_X8G8R8B8Q && bgraWriteMask != 0x00000007) &&
		//	    (state.targetFormat[index] == FORMAT_X8G8R8B8Q && bgraWriteMask != 0x0000000F)))   // FIXME: Need for masking when XRGB && Fh?
		//	{
		//		Short4 masked = value;
		//		c23 &= *Pointer<Short4>(r.constants + OFFSET(Constants,maskB4Q[bgraWriteMask][0]));
		//		masked &= *Pointer<Short4>(r.constants + OFFSET(Constants,invMaskB4Q[bgraWriteMask][0]));
		//		c23 |= masked;
		//	}

		//	c23 &= *Pointer<Short4>(r.constants + OFFSET(Constants,maskD23Q) + xMask * 8);
		//	value &= *Pointer<Short4>(r.constants + OFFSET(Constants,invMaskD23Q) + xMask * 8);
		//	c23 |= value;
		//	*Pointer<Short4>(cBuffer + 8 * x + 8) = c23;
			break;
		case FORMAT_A8R8G8B8:
		case FORMAT_X8R8G8B8:   // FIXME: Don't touch alpha?
			buffer = cBuffer + x * 4;
			value = *Pointer<Short4>(buffer);

			if((state.targetFormat[index] == FORMAT_A8R8G8B8 && bgraWriteMask != 0x0000000F) ||
			   ((state.targetFormat[index] == FORMAT_X8R8G8B8 && bgraWriteMask != 0x00000007) &&
			    (state.targetFormat[index] == FORMAT_X8R8G8B8 && bgraWriteMask != 0x0000000F)))   // FIXME: Need for masking when XRGB && Fh?
			{
				Short4 masked = value;
				c01 &= *Pointer<Short4>(r.constants + OFFSET(Constants,maskB4Q[bgraWriteMask][0]));
				masked &= *Pointer<Short4>(r.constants + OFFSET(Constants,invMaskB4Q[bgraWriteMask][0]));
				c01 |= masked;
			}

			c01 &= *Pointer<Short4>(r.constants + OFFSET(Constants,maskD01Q) + xMask * 8);
			value &= *Pointer<Short4>(r.constants + OFFSET(Constants,invMaskD01Q) + xMask * 8);
			c01 |= value;
			*Pointer<Short4>(buffer) = c01;

			buffer += *Pointer<Int>(r.data + OFFSET(DrawData,colorPitchB[index]));
			value = *Pointer<Short4>(buffer);

			if((state.targetFormat[index] == FORMAT_A8R8G8B8 && bgraWriteMask != 0x0000000F) ||
			   ((state.targetFormat[index] == FORMAT_X8R8G8B8 && bgraWriteMask != 0x00000007) &&
			    (state.targetFormat[index] == FORMAT_X8R8G8B8 && bgraWriteMask != 0x0000000F)))   // FIXME: Need for masking when XRGB && Fh?
			{
				Short4 masked = value;
				c23 &= *Pointer<Short4>(r.constants + OFFSET(Constants,maskB4Q[bgraWriteMask][0]));
				masked &= *Pointer<Short4>(r.constants + OFFSET(Constants,invMaskB4Q[bgraWriteMask][0]));
				c23 |= masked;
			}

			c23 &= *Pointer<Short4>(r.constants + OFFSET(Constants,maskD23Q) + xMask * 8);
			value &= *Pointer<Short4>(r.constants + OFFSET(Constants,invMaskD23Q) + xMask * 8);
			c23 |= value;
			*Pointer<Short4>(buffer) = c23;
			break;
		case FORMAT_G16R16:
			buffer = cBuffer + 4 * x;

			value = *Pointer<Short4>(buffer);

			if((rgbaWriteMask & 0x00000003) != 0x00000003)
			{
				Short4 masked = value;
				current.r &= *Pointer<Short4>(r.constants + OFFSET(Constants,maskW01Q[rgbaWriteMask & 0x3][0]));
				masked &= *Pointer<Short4>(r.constants + OFFSET(Constants,invMaskW01Q[rgbaWriteMask & 0x3][0]));
				current.r |= masked;
			}

			current.r &= *Pointer<Short4>(r.constants + OFFSET(Constants,maskD01Q) + xMask * 8);
			value &= *Pointer<Short4>(r.constants + OFFSET(Constants,invMaskD01Q) + xMask * 8);
			current.r |= value;
			*Pointer<Short4>(buffer) = current.r;

			buffer += *Pointer<Int>(r.data + OFFSET(DrawData,colorPitchB[index]));

			value = *Pointer<Short4>(buffer);

			if((rgbaWriteMask & 0x00000003) != 0x00000003)
			{
				Short4 masked = value;
				current.g &= *Pointer<Short4>(r.constants + OFFSET(Constants,maskW01Q[rgbaWriteMask & 0x3][0]));
				masked &= *Pointer<Short4>(r.constants + OFFSET(Constants,invMaskW01Q[rgbaWriteMask & 0x3][0]));
				current.g |= masked;
			}

			current.g &= *Pointer<Short4>(r.constants + OFFSET(Constants,maskD23Q) + xMask * 8);
			value &= *Pointer<Short4>(r.constants + OFFSET(Constants,invMaskD23Q) + xMask * 8);
			current.g |= value;
			*Pointer<Short4>(buffer) = current.g;
			break;
		case FORMAT_A16B16G16R16:
			buffer = cBuffer + 8 * x;

			{
				value = *Pointer<Short4>(buffer);

				if(rgbaWriteMask != 0x0000000F)
				{
					Short4 masked = value;
					current.r &= *Pointer<Short4>(r.constants + OFFSET(Constants,maskW4Q[rgbaWriteMask][0]));
					masked &= *Pointer<Short4>(r.constants + OFFSET(Constants,invMaskW4Q[rgbaWriteMask][0]));
					current.r |= masked;
				}

				current.r &= *Pointer<Short4>(r.constants + OFFSET(Constants,maskQ0Q) + xMask * 8);
				value &= *Pointer<Short4>(r.constants + OFFSET(Constants,invMaskQ0Q) + xMask * 8);
				current.r |= value;
				*Pointer<Short4>(buffer) = current.r;
			}

			{
				value = *Pointer<Short4>(buffer + 8);

				if(rgbaWriteMask != 0x0000000F)
				{
					Short4 masked = value;
					current.g &= *Pointer<Short4>(r.constants + OFFSET(Constants,maskW4Q[rgbaWriteMask][0]));
					masked &= *Pointer<Short4>(r.constants + OFFSET(Constants,invMaskW4Q[rgbaWriteMask][0]));
					current.g |= masked;
				}

				current.g &= *Pointer<Short4>(r.constants + OFFSET(Constants,maskQ1Q) + xMask * 8);
				value &= *Pointer<Short4>(r.constants + OFFSET(Constants,invMaskQ1Q) + xMask * 8);
				current.g |= value;
				*Pointer<Short4>(buffer + 8) = current.g;
			}

			buffer += *Pointer<Int>(r.data + OFFSET(DrawData,colorPitchB[index]));

			{
				value = *Pointer<Short4>(buffer);

				if(rgbaWriteMask != 0x0000000F)
				{
					Short4 masked = value;
					current.b &= *Pointer<Short4>(r.constants + OFFSET(Constants,maskW4Q[rgbaWriteMask][0]));
					masked &= *Pointer<Short4>(r.constants + OFFSET(Constants,invMaskW4Q[rgbaWriteMask][0]));
					current.b |= masked;
				}

				current.b &= *Pointer<Short4>(r.constants + OFFSET(Constants,maskQ2Q) + xMask * 8);
				value &= *Pointer<Short4>(r.constants + OFFSET(Constants,invMaskQ2Q) + xMask * 8);
				current.b |= value;
				*Pointer<Short4>(buffer) = current.b;
			}

			{
				value = *Pointer<Short4>(buffer + 8);

				if(rgbaWriteMask != 0x0000000F)
				{
					Short4 masked = value;
					current.a &= *Pointer<Short4>(r.constants + OFFSET(Constants,maskW4Q[rgbaWriteMask][0]));
					masked &= *Pointer<Short4>(r.constants + OFFSET(Constants,invMaskW4Q[rgbaWriteMask][0]));
					current.a |= masked;
				}

				current.a &= *Pointer<Short4>(r.constants + OFFSET(Constants,maskQ3Q) + xMask * 8);
				value &= *Pointer<Short4>(r.constants + OFFSET(Constants,invMaskQ3Q) + xMask * 8);
				current.a |= value;
				*Pointer<Short4>(buffer + 8) = current.a;
			}
			break;
		default:
			ASSERT(false);
		}
	}

	void PixelRoutine::blendFactor(Registers &r, const Color4f &blendFactor, const Color4f &oC, const Color4f &pixel, Context::BlendFactor blendFactorActive) 
	{
		switch(blendFactorActive)
		{
		case Context::BLEND_ZERO:
			// Optimized
			break;
		case Context::BLEND_ONE:
			// Optimized
			break;
		case Context::BLEND_SOURCE:
			blendFactor.r = oC.r;
			blendFactor.g = oC.g;
			blendFactor.b = oC.b;
			break;
		case Context::BLEND_INVSOURCE:
			blendFactor.r = Float4(1.0f, 1.0f, 1.0f, 1.0f) - oC.r;
			blendFactor.g = Float4(1.0f, 1.0f, 1.0f, 1.0f) - oC.g;
			blendFactor.b = Float4(1.0f, 1.0f, 1.0f, 1.0f) - oC.b;
			break;
		case Context::BLEND_DEST:
			blendFactor.r = pixel.r;
			blendFactor.g = pixel.g;
			blendFactor.b = pixel.b;
			break;
		case Context::BLEND_INVDEST:
			blendFactor.r = Float4(1.0f, 1.0f, 1.0f, 1.0f) - pixel.r;
			blendFactor.g = Float4(1.0f, 1.0f, 1.0f, 1.0f) - pixel.g;
			blendFactor.b = Float4(1.0f, 1.0f, 1.0f, 1.0f) - pixel.b;
			break;
		case Context::BLEND_SOURCEALPHA:
			blendFactor.r = oC.a;
			blendFactor.g = oC.a;
			blendFactor.b = oC.a;
			break;
		case Context::BLEND_INVSOURCEALPHA:
			blendFactor.r = Float4(1.0f, 1.0f, 1.0f, 1.0f) - oC.a;
			blendFactor.g = Float4(1.0f, 1.0f, 1.0f, 1.0f) - oC.a;
			blendFactor.b = Float4(1.0f, 1.0f, 1.0f, 1.0f) - oC.a;
			break;
		case Context::BLEND_DESTALPHA:
			blendFactor.r = pixel.a;
			blendFactor.g = pixel.a;
			blendFactor.b = pixel.a;
			break;
		case Context::BLEND_INVDESTALPHA:
			blendFactor.r = Float4(1.0f, 1.0f, 1.0f, 1.0f) - pixel.a;
			blendFactor.g = Float4(1.0f, 1.0f, 1.0f, 1.0f) - pixel.a;
			blendFactor.b = Float4(1.0f, 1.0f, 1.0f, 1.0f) - pixel.a;
			break;
		case Context::BLEND_SRCALPHASAT:
			blendFactor.r = Float4(1.0f, 1.0f, 1.0f, 1.0f) - pixel.a;
			blendFactor.r = Min(blendFactor.r, oC.a);
			blendFactor.g = blendFactor.r;
			blendFactor.b = blendFactor.r;
			break;
		case Context::BLEND_CONSTANT:
			blendFactor.r = *Pointer<Float4>(r.data + OFFSET(DrawData,factor.blendConstant4F[0]));
			blendFactor.g = *Pointer<Float4>(r.data + OFFSET(DrawData,factor.blendConstant4F[1]));
			blendFactor.b = *Pointer<Float4>(r.data + OFFSET(DrawData,factor.blendConstant4F[2]));
			break;
		case Context::BLEND_INVCONSTANT:
			blendFactor.r = *Pointer<Float4>(r.data + OFFSET(DrawData,factor.invBlendConstant4F[0]));
			blendFactor.g = *Pointer<Float4>(r.data + OFFSET(DrawData,factor.invBlendConstant4F[1]));
			blendFactor.b = *Pointer<Float4>(r.data + OFFSET(DrawData,factor.invBlendConstant4F[2]));
			break;
		default:
			ASSERT(false);
		}
	}

	void PixelRoutine::blendFactorAlpha(Registers &r, const Color4f &blendFactor, const Color4f &oC, const Color4f &pixel, Context::BlendFactor blendFactorAlphaActive) 
	{
		switch(blendFactorAlphaActive)
		{
		case Context::BLEND_ZERO:
			// Optimized
			break;
		case Context::BLEND_ONE:
			// Optimized
			break;
		case Context::BLEND_SOURCE:
			blendFactor.a = oC.a;
			break;
		case Context::BLEND_INVSOURCE:
			blendFactor.a = Float4(1.0f, 1.0f, 1.0f, 1.0f) - oC.a;
			break;
		case Context::BLEND_DEST:
			blendFactor.a = pixel.a;
			break;
		case Context::BLEND_INVDEST:
			blendFactor.a = Float4(1.0f, 1.0f, 1.0f, 1.0f) - pixel.a;
			break;
		case Context::BLEND_SOURCEALPHA:
			blendFactor.a = oC.a;
			break;
		case Context::BLEND_INVSOURCEALPHA:
			blendFactor.a = Float4(1.0f, 1.0f, 1.0f, 1.0f) - oC.a;
			break;
		case Context::BLEND_DESTALPHA:
			blendFactor.a = pixel.a;
			break;
		case Context::BLEND_INVDESTALPHA:
			blendFactor.a = Float4(1.0f, 1.0f, 1.0f, 1.0f) - pixel.a;
			break;
		case Context::BLEND_SRCALPHASAT:
			blendFactor.a = Float4(1.0f, 1.0f, 1.0f, 1.0f);
			break;
		case Context::BLEND_CONSTANT:
			blendFactor.a = *Pointer<Float4>(r.data + OFFSET(DrawData,factor.blendConstant4F[3]));
			break;
		case Context::BLEND_INVCONSTANT:
			blendFactor.a = *Pointer<Float4>(r.data + OFFSET(DrawData,factor.invBlendConstant4F[3]));
			break;
		default:
			ASSERT(false);
		}
	}

	void PixelRoutine::alphaBlend(Registers &r, int index, Pointer<Byte> &cBuffer, Color4f &oC, Int &x)
	{
		if(!state.alphaBlendActive)
		{
			return;
		}

		Pointer<Byte> buffer;
		Color4f pixel;

		Color4i color;
		Short4 c01;
		Short4 c23;

		// Read pixel
		switch(state.targetFormat[index])
		{
		case FORMAT_A8R8G8B8:
			buffer = cBuffer + 4 * x;
			c01 = *Pointer<Short4>(buffer);
			buffer += *Pointer<Int>(r.data + OFFSET(DrawData,colorPitchB[index]));
			c23 = *Pointer<Short4>(buffer);
			color.b = c01;
			color.g = c01;
			color.b = UnpackLow(As<Byte8>(color.b), As<Byte8>(c23));
			color.g = UnpackHigh(As<Byte8>(color.g), As<Byte8>(c23));
			color.r = color.b;
			color.b = UnpackLow(As<Byte8>(color.b), As<Byte8>(color.g));
			color.r = UnpackHigh(As<Byte8>(color.r), As<Byte8>(color.g));
			color.g = color.b;
			color.a = color.r;
			color.r = UnpackLow(As<Byte8>(color.r), As<Byte8>(color.r));
			color.g = UnpackHigh(As<Byte8>(color.g), As<Byte8>(color.g));
			color.b = UnpackLow(As<Byte8>(color.b), As<Byte8>(color.b));
			color.a = UnpackHigh(As<Byte8>(color.a), As<Byte8>(color.a));

			pixel.r = convertUnsigned16(As<UShort4>(color.r));
			pixel.g = convertUnsigned16(As<UShort4>(color.g));
			pixel.b = convertUnsigned16(As<UShort4>(color.b));
			pixel.a = convertUnsigned16(As<UShort4>(color.a));
			break;
		case FORMAT_X8R8G8B8:
			buffer = cBuffer + 4 * x;
			c01 = *Pointer<Short4>(buffer);
			buffer += *Pointer<Int>(r.data + OFFSET(DrawData,colorPitchB[index]));
			c23 = *Pointer<Short4>(buffer);
			color.b = c01;
			color.g = c01;
			color.b = UnpackLow(As<Byte8>(color.b), As<Byte8>(c23));
			color.g = UnpackHigh(As<Byte8>(color.g), As<Byte8>(c23));
			color.r = color.b;
			color.b = UnpackLow(As<Byte8>(color.b), As<Byte8>(color.g));
			color.r = UnpackHigh(As<Byte8>(color.r), As<Byte8>(color.g));
			color.g = color.b;
			color.r = UnpackLow(As<Byte8>(color.r), As<Byte8>(color.r));
			color.g = UnpackHigh(As<Byte8>(color.g), As<Byte8>(color.g));
			color.b = UnpackLow(As<Byte8>(color.b), As<Byte8>(color.b));

			pixel.r = convertUnsigned16(As<UShort4>(color.r));
			pixel.g = convertUnsigned16(As<UShort4>(color.g));
			pixel.b = convertUnsigned16(As<UShort4>(color.b));
			pixel.a = Float4(1.0f, 1.0f, 1.0f, 1.0f);
			break;
		case FORMAT_A8G8R8B8Q:
UNIMPLEMENTED();
		//	UnpackLow(pixel.b, qword_ptr [cBuffer+8*x+0]);
		//	UnpackHigh(pixel.r, qword_ptr [cBuffer+8*x+0]);
		//	UnpackLow(pixel.g, qword_ptr [cBuffer+8*x+8]);
		//	UnpackHigh(pixel.a, qword_ptr [cBuffer+8*x+8]);
			break;
		case FORMAT_X8G8R8B8Q:
UNIMPLEMENTED();
		//	UnpackLow(pixel.b, qword_ptr [cBuffer+8*x+0]);
		//	UnpackHigh(pixel.r, qword_ptr [cBuffer+8*x+0]);
		//	UnpackLow(pixel.g, qword_ptr [cBuffer+8*x+8]);
		//	pixel.a = Short4(0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF);
			break;
		case FORMAT_A16B16G16R16:
			buffer  = cBuffer;
			color.r = *Pointer<Short4>(buffer + 8 * x);
			color.g = *Pointer<Short4>(buffer + 8 * x + 8);
			buffer += *Pointer<Int>(r.data + OFFSET(DrawData,colorPitchB[index]));
			color.b = *Pointer<Short4>(buffer + 8 * x);
			color.a = *Pointer<Short4>(buffer + 8 * x + 8);
			
			transpose4x4(color.r, color.g, color.b, color.a);

			pixel.r = convertUnsigned16(As<UShort4>(color.r));
			pixel.g = convertUnsigned16(As<UShort4>(color.g));
			pixel.b = convertUnsigned16(As<UShort4>(color.b));
			pixel.a = convertUnsigned16(As<UShort4>(color.a));
			break;
		case FORMAT_G16R16:
			buffer = cBuffer;
			color.r = *Pointer<Short4>(buffer  + 4 * x);
			buffer += *Pointer<Int>(r.data + OFFSET(DrawData,colorPitchB[index]));
			color.g = *Pointer<Short4>(buffer  + 4 * x);
			color.b = color.r;
			color.r = As<Short4>(UnpackLow(color.r, color.g));
			color.b = As<Short4>(UnpackHigh(color.b, color.g));
			color.g = color.b;
			color.r = As<Short4>(UnpackLow(color.r, color.b));
			color.g = As<Short4>(UnpackHigh(color.g, color.b));
			
			pixel.r = convertUnsigned16(As<UShort4>(color.r));
			pixel.g = convertUnsigned16(As<UShort4>(color.g));
			pixel.b = Float4(1.0f, 1.0f, 1.0f, 1.0f);
			pixel.a = Float4(1.0f, 1.0f, 1.0f, 1.0f);
			break;
		case FORMAT_R32F:
			buffer = cBuffer;
			// FIXME: movlps
			pixel.r.x = *Pointer<Float>(buffer + 4 * x + 0);
			pixel.r.y = *Pointer<Float>(buffer + 4 * x + 4);
			buffer += *Pointer<Int>(r.data + OFFSET(DrawData,colorPitchB[index]));
			// FIXME: movhps
			pixel.r.z = *Pointer<Float>(buffer + 4 * x + 0);
			pixel.r.w = *Pointer<Float>(buffer + 4 * x + 4);
			pixel.g = Float4(1.0f, 1.0f, 1.0f, 1.0f);
			pixel.b = Float4(1.0f, 1.0f, 1.0f, 1.0f);
			pixel.a = Float4(1.0f, 1.0f, 1.0f, 1.0f);
			break;
		case FORMAT_G32R32F:
			buffer = cBuffer;
			pixel.r = *Pointer<Float4>(buffer + 8 * x, 16);
			buffer += *Pointer<Int>(r.data + OFFSET(DrawData,colorPitchB[index]));
			pixel.g = *Pointer<Float4>(buffer + 8 * x, 16);
			pixel.b = pixel.r;
			pixel.r = ShuffleLowHigh(pixel.r, pixel.g, 0x88);
			pixel.b = ShuffleLowHigh(pixel.b, pixel.g, 0xDD);
			pixel.g = pixel.b;
			pixel.b = Float4(1.0f, 1.0f, 1.0f, 1.0f);
			pixel.a = Float4(1.0f, 1.0f, 1.0f, 1.0f);
			break;
		case FORMAT_A32B32G32R32F:
			buffer = cBuffer;
			pixel.r = *Pointer<Float4>(buffer + 16 * x, 16);
			pixel.g = *Pointer<Float4>(buffer + 16 * x + 16, 16);
			buffer += *Pointer<Int>(r.data + OFFSET(DrawData,colorPitchB[index]));
			pixel.b = *Pointer<Float4>(buffer + 16 * x, 16);
			pixel.a = *Pointer<Float4>(buffer + 16 * x + 16, 16);
			transpose4x4(pixel.r, pixel.g, pixel.b, pixel.a);
			break;
		default:
			ASSERT(false);
		}

		if(postBlendSRGB && state.writeSRGB)
		{
			sRGBtoLinear(pixel.r);
			sRGBtoLinear(pixel.g);
			sRGBtoLinear(pixel.b);
		}

		// Final Color = ObjectColor * SourceBlendFactor + PixelColor * DestinationBlendFactor
		Color4f sourceFactor;
		Color4f destFactor;

		blendFactor(r, sourceFactor, oC, pixel, (Context::BlendFactor)state.sourceBlendFactor);
		blendFactor(r, destFactor, oC, pixel, (Context::BlendFactor)state.destBlendFactor);

		if(state.sourceBlendFactor != Context::BLEND_ONE && state.sourceBlendFactor != Context::BLEND_ZERO)
		{
			oC.r *= sourceFactor.r;
			oC.g *= sourceFactor.g;
			oC.b *= sourceFactor.b;
		}
	
		if(state.destBlendFactor != Context::BLEND_ONE && state.destBlendFactor != Context::BLEND_ZERO)
		{
			pixel.r *= destFactor.r;
			pixel.g *= destFactor.g;
			pixel.b *= destFactor.b;
		}

		switch(state.blendOperation)
		{
		case Context::BLENDOP_ADD:
			oC.r += pixel.r;
			oC.g += pixel.g;
			oC.b += pixel.b;
			break;
		case Context::BLENDOP_SUB:
			oC.r -= pixel.r;
			oC.g -= pixel.g;
			oC.b -= pixel.b;
			break;
		case Context::BLENDOP_INVSUB:
			oC.r = pixel.r - oC.r;
			oC.g = pixel.g - oC.g;
			oC.b = pixel.b - oC.b;
			break;
		case Context::BLENDOP_MIN:
			oC.r = Min(oC.r, pixel.r);
			oC.g = Min(oC.g, pixel.g);
			oC.b = Min(oC.b, pixel.b);
			break;
		case Context::BLENDOP_MAX:
			oC.r = Max(oC.r, pixel.r);
			oC.g = Max(oC.g, pixel.g);
			oC.b = Max(oC.b, pixel.b);
			break;
		case Context::BLENDOP_SOURCE:
			// No operation
			break;
		case Context::BLENDOP_DEST:
			oC.r = pixel.r;
			oC.g = pixel.g;
			oC.b = pixel.b;
			break;
		case Context::BLENDOP_NULL:
			oC.r = Float4(0.0f, 0.0f, 0.0f, 0.0f);
			oC.g = Float4(0.0f, 0.0f, 0.0f, 0.0f);
			oC.b = Float4(0.0f, 0.0f, 0.0f, 0.0f);
			break;
		default:
			ASSERT(false);
		}

		blendFactorAlpha(r, sourceFactor, oC, pixel, (Context::BlendFactor)state.sourceBlendFactorAlpha);
		blendFactorAlpha(r, destFactor, oC, pixel, (Context::BlendFactor)state.destBlendFactorAlpha);

		if(state.sourceBlendFactorAlpha != Context::BLEND_ONE && state.sourceBlendFactorAlpha != Context::BLEND_ZERO)
		{
			oC.a *= sourceFactor.a;
		}
	
		if(state.destBlendFactorAlpha != Context::BLEND_ONE && state.destBlendFactorAlpha != Context::BLEND_ZERO)
		{
			pixel.a *= destFactor.a;
		}

		switch(state.blendOperationAlpha)
		{
		case Context::BLENDOP_ADD:
			oC.a += pixel.a;
			break;
		case Context::BLENDOP_SUB:
			oC.a -= pixel.a;
			break;
		case Context::BLENDOP_INVSUB:
			pixel.a -= oC.a;
			oC.a = pixel.a;
			break;
		case Context::BLENDOP_MIN:	
			oC.a = Min(oC.a, pixel.a);
			break;
		case Context::BLENDOP_MAX:	
			oC.a = Max(oC.a, pixel.a);
			break;
		case Context::BLENDOP_SOURCE:
			// No operation
			break;
		case Context::BLENDOP_DEST:
			oC.a = pixel.a;
			break;
		case Context::BLENDOP_NULL:
			oC.a = Float4(0.0f, 0.0f, 0.0f, 0.0f);
			break;
		default:
			ASSERT(false);
		}
	}

	void PixelRoutine::writeColor(Registers &r, int index, Pointer<Byte> &cBuffer, Int &x, Color4f &oC, Int &sMask, Int &zMask, Int &cMask)
	{
		if(!state.colorWriteActive(index))
		{
			return;
		}

		Color4i color;

		switch(state.targetFormat[index])
		{
		case FORMAT_X8R8G8B8:
		case FORMAT_A8R8G8B8:
		case FORMAT_G16R16:
		case FORMAT_A16B16G16R16:
			convertFixed16(color, oC, true);
			writeColor(r, index, cBuffer, x, color, sMask, zMask, cMask);
			return;
		case FORMAT_R32F:
			break;
		case FORMAT_G32R32F:
			oC.b = oC.r;
			oC.r = UnpackLow(oC.r, oC.g);
			oC.b = UnpackHigh(oC.b, oC.g);
			oC.g = oC.b;
			break;
		case FORMAT_A32B32G32R32F:
			transpose4x4(oC.r, oC.g, oC.b, oC.a);
			break;
		default:
			ASSERT(false);
		}

		int rgbaWriteMask = state.colorWriteActive(index);

		Int xMask;   // Combination of all masks

		if(state.depthTestActive)
		{
			xMask = zMask;
		}
		else
		{
			xMask = cMask;
		}

		if(state.stencilActive)
		{
			xMask &= sMask;
		}

		Pointer<Byte> buffer;
		Float4 value;

		switch(state.targetFormat[index])
		{
		case FORMAT_R32F:
			if(rgbaWriteMask & 0x00000001)
			{
				buffer = cBuffer + 4 * x;

				// FIXME: movlps
				value.x = *Pointer<Float>(buffer + 0);
				value.y = *Pointer<Float>(buffer + 4);

				buffer += *Pointer<Int>(r.data + OFFSET(DrawData,colorPitchB[index]));

				// FIXME: movhps
				value.z = *Pointer<Float>(buffer + 0);
				value.w = *Pointer<Float>(buffer + 4);

				oC.r = As<Float4>(As<Int4>(oC.r) & *Pointer<Int4>(r.constants + OFFSET(Constants,maskD4X) + xMask * 16, 16));
				value = As<Float4>(As<Int4>(value) & *Pointer<Int4>(r.constants + OFFSET(Constants,invMaskD4X) + xMask * 16, 16));
				oC.r = As<Float4>(As<Int4>(oC.r) | As<Int4>(value));

				// FIXME: movhps
				*Pointer<Float>(buffer + 0) = oC.r.z;
				*Pointer<Float>(buffer + 4) = oC.r.w;

				buffer -= *Pointer<Int>(r.data + OFFSET(DrawData,colorPitchB[index]));

				// FIXME: movlps
				*Pointer<Float>(buffer + 0) = oC.r.x;
				*Pointer<Float>(buffer + 4) = oC.r.y;
			}
			break;
		case FORMAT_G32R32F:
			buffer = cBuffer + 8 * x;

			value = *Pointer<Float4>(buffer);

			if((rgbaWriteMask & 0x00000003) != 0x00000003)
			{
				Float4 masked = value;
				oC.r = As<Float4>(As<Int4>(oC.r) & *Pointer<Int4>(r.constants + OFFSET(Constants,maskD01X[rgbaWriteMask & 0x3][0])));
				masked = As<Float4>(As<Int4>(masked) & *Pointer<Int4>(r.constants + OFFSET(Constants,invMaskD01X[rgbaWriteMask & 0x3][0])));
				oC.r = As<Float4>(As<Int4>(oC.r) | As<Int4>(masked));
			}

			oC.r = As<Float4>(As<Int4>(oC.r) & *Pointer<Int4>(r.constants + OFFSET(Constants,maskQ01X) + xMask * 16, 16));
			value = As<Float4>(As<Int4>(value) & *Pointer<Int4>(r.constants + OFFSET(Constants,invMaskQ01X) + xMask * 16, 16));
			oC.r = As<Float4>(As<Int4>(oC.r) | As<Int4>(value));
			*Pointer<Float4>(buffer) = oC.r;

			buffer += *Pointer<Int>(r.data + OFFSET(DrawData,colorPitchB[index]));

			value = *Pointer<Float4>(buffer);

			if((rgbaWriteMask & 0x00000003) != 0x00000003)
			{
				Float4 masked;

				masked = value;
				oC.g = As<Float4>(As<Int4>(oC.g) & *Pointer<Int4>(r.constants + OFFSET(Constants,maskD01X[rgbaWriteMask & 0x3][0])));
				masked = As<Float4>(As<Int4>(masked) & *Pointer<Int4>(r.constants + OFFSET(Constants,invMaskD01X[rgbaWriteMask & 0x3][0])));
				oC.g = As<Float4>(As<Int4>(oC.g) | As<Int4>(masked));
			}

			oC.g = As<Float4>(As<Int4>(oC.g) & *Pointer<Int4>(r.constants + OFFSET(Constants,maskQ23X) + xMask * 16, 16));
			value = As<Float4>(As<Int4>(value) & *Pointer<Int4>(r.constants + OFFSET(Constants,invMaskQ23X) + xMask * 16, 16));
			oC.g = As<Float4>(As<Int4>(oC.g) | As<Int4>(value));
			*Pointer<Float4>(buffer) = oC.g;
			break;
		case FORMAT_A32B32G32R32F:
			buffer = cBuffer + 16 * x;

			{
				value = *Pointer<Float4>(buffer, 16);

				if(rgbaWriteMask != 0x0000000F)
				{
					Float4 masked = value;
					oC.r = As<Float4>(As<Int4>(oC.r) & *Pointer<Int4>(r.constants + OFFSET(Constants,maskD4X[rgbaWriteMask][0])));
					masked = As<Float4>(As<Int4>(masked) & *Pointer<Int4>(r.constants + OFFSET(Constants,invMaskD4X[rgbaWriteMask][0])));
					oC.r = As<Float4>(As<Int4>(oC.r) | As<Int4>(masked));
				}
				
				oC.r = As<Float4>(As<Int4>(oC.r) & *Pointer<Int4>(r.constants + OFFSET(Constants,maskX0X) + xMask * 16, 16));
				value = As<Float4>(As<Int4>(value) & *Pointer<Int4>(r.constants + OFFSET(Constants,invMaskX0X) + xMask * 16, 16));
				oC.r = As<Float4>(As<Int4>(oC.r) | As<Int4>(value));
				*Pointer<Float4>(buffer, 16) = oC.r;
			}

			{
				value = *Pointer<Float4>(buffer + 16, 16);

				if(rgbaWriteMask != 0x0000000F)
				{	
					Float4 masked = value;
					oC.g = As<Float4>(As<Int4>(oC.g) & *Pointer<Int4>(r.constants + OFFSET(Constants,maskD4X[rgbaWriteMask][0])));
					masked = As<Float4>(As<Int4>(masked) & *Pointer<Int4>(r.constants + OFFSET(Constants,invMaskD4X[rgbaWriteMask][0])));
					oC.g = As<Float4>(As<Int4>(oC.g) | As<Int4>(masked));
				}

				oC.g = As<Float4>(As<Int4>(oC.g) & *Pointer<Int4>(r.constants + OFFSET(Constants,maskX1X) + xMask * 16, 16));
				value = As<Float4>(As<Int4>(value) & *Pointer<Int4>(r.constants + OFFSET(Constants,invMaskX1X) + xMask * 16, 16));
				oC.g = As<Float4>(As<Int4>(oC.g) | As<Int4>(value));
				*Pointer<Float4>(buffer + 16, 16) = oC.g;
			}

			buffer += *Pointer<Int>(r.data + OFFSET(DrawData,colorPitchB[index]));

			{
				value = *Pointer<Float4>(buffer, 16);

				if(rgbaWriteMask != 0x0000000F)
				{
					Float4 masked = value;
					oC.b = As<Float4>(As<Int4>(oC.b) & *Pointer<Int4>(r.constants + OFFSET(Constants,maskD4X[rgbaWriteMask][0])));
					masked = As<Float4>(As<Int4>(masked) & *Pointer<Int4>(r.constants + OFFSET(Constants,invMaskD4X[rgbaWriteMask][0])));
					oC.b = As<Float4>(As<Int4>(oC.b) | As<Int4>(masked));
				}

				oC.b = As<Float4>(As<Int4>(oC.b) & *Pointer<Int4>(r.constants + OFFSET(Constants,maskX2X) + xMask * 16, 16));
				value = As<Float4>(As<Int4>(value) & *Pointer<Int4>(r.constants + OFFSET(Constants,invMaskX2X) + xMask * 16, 16));
				oC.b = As<Float4>(As<Int4>(oC.b) | As<Int4>(value));
				*Pointer<Float4>(buffer, 16) = oC.b;
			}

			{
				value = *Pointer<Float4>(buffer + 16, 16);

				if(rgbaWriteMask != 0x0000000F)
				{
					Float4 masked = value;
					oC.a = As<Float4>(As<Int4>(oC.a) & *Pointer<Int4>(r.constants + OFFSET(Constants,maskD4X[rgbaWriteMask][0])));
					masked = As<Float4>(As<Int4>(masked) & *Pointer<Int4>(r.constants + OFFSET(Constants,invMaskD4X[rgbaWriteMask][0])));
					oC.a = As<Float4>(As<Int4>(oC.a) | As<Int4>(masked));
				}

				oC.a = As<Float4>(As<Int4>(oC.a) & *Pointer<Int4>(r.constants + OFFSET(Constants,maskX3X) + xMask * 16, 16));
				value = As<Float4>(As<Int4>(value) & *Pointer<Int4>(r.constants + OFFSET(Constants,invMaskX3X) + xMask * 16, 16));
				oC.a = As<Float4>(As<Int4>(oC.a) | As<Int4>(value));
				*Pointer<Float4>(buffer + 16, 16) = oC.a;
			}
			break;
		default:
			ASSERT(false);
		}
	}

	void PixelRoutine::ps_1_x(Registers &r, Int cMask[4])
	{
		int pad = 0;        // Count number of texm3x3pad instructions
		Color4i dPairing;   // Destination for first pairing instruction

		for(int i = 0; i < pixelShader->getLength(); i++)
		{
			const ShaderInstruction *instruction = pixelShader->getInstruction(i);
			Op::Opcode opcode = instruction->getOpcode();

		//	#ifndef NDEBUG   // FIXME: Centralize debug output control
		//		pixelShader->printInstruction(i, "debug.txt");
		//	#endif

			if(opcode == Op::OPCODE_DCL || opcode == Op::OPCODE_DEF || opcode == Op::OPCODE_DEFI || opcode == Op::OPCODE_DEFB)
			{
				continue;
			}

			const Dst &dst = instruction->getDestinationParameter();
			const Src &src0 = instruction->getSourceParameter(0);
			const Src &src1 = instruction->getSourceParameter(1);
			const Src &src2 = instruction->getSourceParameter(2);
			const Src &src3 = instruction->getSourceParameter(3);

			bool pairing = i + 1 < pixelShader->getLength() && pixelShader->getInstruction(i + 1)->isCoissue();   // First instruction of pair
			bool coissue = instruction->isCoissue();                                                                                // Second instruction of pair

			Color4i d;
			Color4i s0;
			Color4i s1;
			Color4i s2;
			Color4i s3;

			if(src0.type != Src::PARAMETER_VOID) s0 = regi(r, src0);
			if(src1.type != Src::PARAMETER_VOID) s1 = regi(r, src1);
			if(src2.type != Src::PARAMETER_VOID) s2 = regi(r, src2);
			if(src3.type != Src::PARAMETER_VOID) s3 = regi(r, src3);

			switch(opcode)
			{
			case Op::OPCODE_PS_1_0:															break;
			case Op::OPCODE_PS_1_1:															break;
			case Op::OPCODE_PS_1_2:															break;
			case Op::OPCODE_PS_1_3:															break;
			case Op::OPCODE_PS_1_4:															break;

			case Op::OPCODE_DEF:															break;

			case Op::OPCODE_NOP:															break;
			case Op::OPCODE_MOV:			MOV(d, s0);										break;
			case Op::OPCODE_ADD:			ADD(d, s0, s1);									break;
			case Op::OPCODE_SUB:			SUB(d, s0, s1);									break;
			case Op::OPCODE_MAD:			MAD(d, s0, s1, s2);								break;
			case Op::OPCODE_MUL:			MUL(d, s0, s1);									break;
			case Op::OPCODE_DP3:			DP3(d, s0, s1);									break;
			case Op::OPCODE_DP4:			DP4(d, s0, s1);									break;
			case Op::OPCODE_LRP:			LRP(d, s0, s1, s2);								break;
			case Op::OPCODE_TEXCOORD:
				if(pixelShader->getVersion() < 0x0104)
				{
					TEXCOORD(d, Float4(r.vx[2 + dst.index]), Float4(r.vy[2 + dst.index]), Float4(r.vz[2 + dst.index]), dst.index);
				}
				else
				{
					if((src0.swizzle & 0x30) == 0x20)   // .xyz
					{
						TEXCRD(d, Float4(r.vx[2 + src0.index]), Float4(r.vy[2 + src0.index]), Float4(r.vz[2 + src0.index]), src0.index, src0.modifier == ShaderInstruction::SourceParameter::MODIFIER_DZ || src0.modifier == ShaderInstruction::SourceParameter::MODIFIER_DW);
					}
					else   // .xyw
					{
						TEXCRD(d, Float4(r.vx[2 + src0.index]), Float4(r.vy[2 + src0.index]), Float4(r.vw[2 + src0.index]), src0.index, src0.modifier == ShaderInstruction::SourceParameter::MODIFIER_DZ || src0.modifier == ShaderInstruction::SourceParameter::MODIFIER_DW);
					}
				}
				break;
			case Op::OPCODE_TEXKILL:
				if(pixelShader->getVersion() < 0x0104)
				{
					TEXKILL(cMask, Float4(r.vx[2 + dst.index]), Float4(r.vy[2 + dst.index]), Float4(r.vz[2 + dst.index]));
				}
				else if(pixelShader->getVersion() == 0x0104)
				{
					if(dst.type == Dst::PARAMETER_TEXTURE)
					{
						TEXKILL(cMask, Float4(r.vx[2 + dst.index]), Float4(r.vy[2 + dst.index]), Float4(r.vz[2 + dst.index]));
					}
					else
					{
						TEXKILL(cMask, r.ri[dst.index]);
					}
				}
				else ASSERT(false);
				break;
			case Op::OPCODE_TEX:
				if(pixelShader->getVersion() < 0x0104)
				{
					TEX(r, d, Float4(r.vx[2 + dst.index]), Float4(r.vy[2 + dst.index]), Float4(r.vz[2 + dst.index]), dst.index, false);
				}
				else if(pixelShader->getVersion() == 0x0104)
				{
					if(src0.type == Src::PARAMETER_TEXTURE)
					{
						if((src0.swizzle & 0x30) == 0x20)   // .xyz
						{
							TEX(r, d, Float4(r.vx[2 + src0.index]), Float4(r.vy[2 + src0.index]), Float4(r.vz[2 + src0.index]), dst.index, src0.modifier == ShaderInstruction::SourceParameter::MODIFIER_DZ || src0.modifier == ShaderInstruction::SourceParameter::MODIFIER_DW);
						}
						else   // .xyw
						{
							TEX(r, d, Float4(r.vx[2 + src0.index]), Float4(r.vy[2 + src0.index]), Float4(r.vw[2 + src0.index]), dst.index, src0.modifier == ShaderInstruction::SourceParameter::MODIFIER_DZ || src0.modifier == ShaderInstruction::SourceParameter::MODIFIER_DW);
						}
					}
					else
					{
						TEXLD(r, d, s0, dst.index, src0.modifier == ShaderInstruction::SourceParameter::MODIFIER_DZ || src0.modifier == ShaderInstruction::SourceParameter::MODIFIER_DW);
					}
				}
				else ASSERT(false);
				break;
			case Op::OPCODE_TEXBEM:			TEXBEM(r, d, s0, Float4(r.vx[2 + dst.index]), Float4(r.vy[2 + dst.index]), Float4(r.vz[2 + dst.index]), dst.index);	break;
			case Op::OPCODE_TEXBEML:		TEXBEML(r, d, s0, Float4(r.vx[2 + dst.index]), Float4(r.vy[2 + dst.index]), Float4(r.vz[2 + dst.index]), dst.index);	break;
			case Op::OPCODE_TEXREG2AR:		TEXREG2AR(r, d, s0, dst.index);					break;
			case Op::OPCODE_TEXREG2GB:		TEXREG2GB(r, d, s0, dst.index);					break;
			case Op::OPCODE_TEXM3X2PAD:		TEXM3X2PAD(r, Float4(r.vx[2 + dst.index]), Float4(r.vy[2 + dst.index]), Float4(r.vz[2 + dst.index]), s0, 0, src0.modifier == Src::MODIFIER_SIGN);	break;
			case Op::OPCODE_TEXM3X2TEX:		TEXM3X2TEX(r, d, Float4(r.vx[2 + dst.index]), Float4(r.vy[2 + dst.index]), Float4(r.vz[2 + dst.index]), dst.index, s0, src0.modifier == Src::MODIFIER_SIGN);	break;
			case Op::OPCODE_TEXM3X3PAD:		TEXM3X3PAD(r, Float4(r.vx[2 + dst.index]), Float4(r.vy[2 + dst.index]), Float4(r.vz[2 + dst.index]), s0, pad++ % 2, src0.modifier == Src::MODIFIER_SIGN);	break;
			case Op::OPCODE_TEXM3X3TEX:		TEXM3X3TEX(r, d, Float4(r.vx[2 + dst.index]), Float4(r.vy[2 + dst.index]), Float4(r.vz[2 + dst.index]), dst.index, s0, src0.modifier == Src::MODIFIER_SIGN);	break;
			case Op::OPCODE_TEXM3X3SPEC:	TEXM3X3SPEC(r, d, Float4(r.vx[2 + dst.index]), Float4(r.vy[2 + dst.index]), Float4(r.vz[2 + dst.index]), dst.index, s0, s1);		break;
			case Op::OPCODE_TEXM3X3VSPEC:	TEXM3X3VSPEC(r, d, Float4(r.vx[2 + dst.index]), Float4(r.vy[2 + dst.index]), Float4(r.vz[2 + dst.index]), dst.index, s0);		break;
			case Op::OPCODE_CND:			CND(d, s0, s1, s2);								break;
			case Op::OPCODE_TEXREG2RGB:		TEXREG2RGB(r, d, s0, dst.index);				break;
			case Op::OPCODE_TEXDP3TEX:		TEXDP3TEX(r, d, Float4(r.vx[2 + dst.index]), Float4(r.vy[2 + dst.index]), Float4(r.vz[2 + dst.index]), dst.index, s0);	break;
			case Op::OPCODE_TEXM3X2DEPTH:	TEXM3X2DEPTH(r, d, Float4(r.vx[2 + dst.index]), Float4(r.vy[2 + dst.index]), Float4(r.vz[2 + dst.index]), s0, src0.modifier == Src::MODIFIER_SIGN);	break;
			case Op::OPCODE_TEXDP3:			TEXDP3(r, d, Float4(r.vx[2 + dst.index]), Float4(r.vy[2 + dst.index]), Float4(r.vz[2 + dst.index]), s0);				break;
			case Op::OPCODE_TEXM3X3:		TEXM3X3(r, d, Float4(r.vx[2 + dst.index]), Float4(r.vy[2 + dst.index]), Float4(r.vz[2 + dst.index]), s0, src0.modifier == Src::MODIFIER_SIGN); 	break;
			case Op::OPCODE_TEXDEPTH:		TEXDEPTH(r);									break;
			case Op::OPCODE_CMP:			CMP(d, s0, s1, s2);								break;
			case Op::OPCODE_BEM:			BEM(r, d, s0, s1, dst.index);					break;
			case Op::OPCODE_PHASE:															break;
			case Op::OPCODE_END:															break;
			default:
				ASSERT(false);
			}

			if(dst.type != Dst::PARAMETER_VOID && opcode != Op::OPCODE_TEXKILL)
			{
				if(dst.shift > 0)
				{
					if(dst.mask & 0x1) {d.r = AddSat(d.r, d.r); if(dst.shift > 1) d.r = AddSat(d.r, d.r); if(dst.shift > 2) d.r = AddSat(d.r, d.r);}
					if(dst.mask & 0x2) {d.g = AddSat(d.g, d.g); if(dst.shift > 1) d.g = AddSat(d.g, d.g); if(dst.shift > 2) d.g = AddSat(d.g, d.g);}
					if(dst.mask & 0x4) {d.b = AddSat(d.b, d.b); if(dst.shift > 1) d.b = AddSat(d.b, d.b); if(dst.shift > 2) d.b = AddSat(d.b, d.b);}
					if(dst.mask & 0x8) {d.a = AddSat(d.a, d.a); if(dst.shift > 1) d.a = AddSat(d.a, d.a); if(dst.shift > 2) d.a = AddSat(d.a, d.a);}
				}
				else if(dst.shift < 0)
				{
					if(dst.mask & 0x1) d.r = d.r >> -dst.shift;
					if(dst.mask & 0x2) d.g = d.g >> -dst.shift;
					if(dst.mask & 0x4) d.b = d.b >> -dst.shift;
					if(dst.mask & 0x8) d.a = d.a >> -dst.shift;
				}

				if(dst.saturate)
				{
					if(dst.mask & 0x1) {d.r = Min(d.r, Short4(0x1000, 0x1000, 0x1000, 0x1000)); d.r = Max(d.r, Short4(0x0000, 0x0000, 0x0000, 0x0000));}
					if(dst.mask & 0x2) {d.g = Min(d.g, Short4(0x1000, 0x1000, 0x1000, 0x1000)); d.g = Max(d.g, Short4(0x0000, 0x0000, 0x0000, 0x0000));}
					if(dst.mask & 0x4) {d.b = Min(d.b, Short4(0x1000, 0x1000, 0x1000, 0x1000)); d.b = Max(d.b, Short4(0x0000, 0x0000, 0x0000, 0x0000));}
					if(dst.mask & 0x8) {d.a = Min(d.a, Short4(0x1000, 0x1000, 0x1000, 0x1000)); d.a = Max(d.a, Short4(0x0000, 0x0000, 0x0000, 0x0000));}
				}

				if(pairing)
				{
					if(dst.mask & 0x1) dPairing.r = d.r;
					if(dst.mask & 0x2) dPairing.g = d.g;
					if(dst.mask & 0x4) dPairing.b = d.b;
					if(dst.mask & 0x8) dPairing.a = d.a;
				}
			
				if(coissue)
				{
					const Dst &dst = pixelShader->getInstruction(i - 1)->getDestinationParameter();

					writeDestination(r, dPairing, dst);
				}
			
				if(!pairing)
				{
					writeDestination(r, d, dst);
				}
			}
		}
	}

	void PixelRoutine::ps_2_x(Registers &r, Int cMask[4])
	{
		r.enableIndex = 0;
		r.stackIndex = 0;
		
		for(int i = 0; i < pixelShader->getLength(); i++)
		{
			const ShaderInstruction *instruction = pixelShader->getInstruction(i);
			Op::Opcode opcode = instruction->getOpcode();

		//	#ifndef NDEBUG   // FIXME: Centralize debug output control
		//		pixelShader->printInstruction(i, "debug.txt");
		//	#endif

			if(opcode == Op::OPCODE_DCL || opcode == Op::OPCODE_DEF || opcode == Op::OPCODE_DEFI || opcode == Op::OPCODE_DEFB)
			{
				continue;
			}

			const Dst &dst = instruction->getDestinationParameter();
			const Src &src0 = instruction->getSourceParameter(0);
			const Src &src1 = instruction->getSourceParameter(1);
			const Src &src2 = instruction->getSourceParameter(2);
			const Src &src3 = instruction->getSourceParameter(3);

			bool predicate = instruction->isPredicate();
			Control control = instruction->getControl();
			bool pp = dst.partialPrecision;
			bool project = instruction->isProject();
			bool bias = instruction->isBias();

			Color4f d;
			Color4f s0;
			Color4f s1;
			Color4f s2;
			Color4f s3;

			if(opcode == Op::OPCODE_TEXKILL)
			{
				if(dst.type == Dst::PARAMETER_TEXTURE)
				{
					d.x = r.vx[2 + dst.index];
					d.y = r.vy[2 + dst.index];
					d.z = r.vz[2 + dst.index];
					d.w = r.vw[2 + dst.index];
				}
				else
				{
					d = r.rf[dst.index];
				}
			}

			if(src0.type != Src::PARAMETER_VOID) s0 = reg(r, src0);
			if(src1.type != Src::PARAMETER_VOID) s1 = reg(r, src1);
			if(src2.type != Src::PARAMETER_VOID) s2 = reg(r, src2);
			if(src3.type != Src::PARAMETER_VOID) s3 = reg(r, src3);

			switch(opcode)
			{
			case Op::OPCODE_PS_2_0:														break;
			case Op::OPCODE_PS_2_x:														break;
			case Op::OPCODE_PS_3_0:														break;
			case Op::OPCODE_DEF:														break;
			case Op::OPCODE_DCL:														break;
			case Op::OPCODE_NOP:														break;
			case Op::OPCODE_MOV:		mov(d, s0);										break;
			case Op::OPCODE_ADD:		add(d, s0, s1);									break;
			case Op::OPCODE_SUB:		sub(d, s0, s1);									break;
			case Op::OPCODE_MUL:		mul(d, s0, s1);									break;
			case Op::OPCODE_MAD:		mad(d, s0, s1, s2);								break;
			case Op::OPCODE_DP2ADD:		dp2add(d, s0, s1, s2);							break;
			case Op::OPCODE_DP3:		dp3(d, s0, s1);									break;
			case Op::OPCODE_DP4:		dp4(d, s0, s1);									break;
			case Op::OPCODE_CMP:		cmp(d, s0, s1, s2);								break;
			case Op::OPCODE_FRC:		frc(d, s0);										break;
			case Op::OPCODE_EXP:		exp(d, s0, pp);									break;
			case Op::OPCODE_LOG:		log(d, s0, pp);									break;
			case Op::OPCODE_RCP:		rcp(d, s0, pp);									break;
			case Op::OPCODE_RSQ:		rsq(d, s0, pp);									break;
			case Op::OPCODE_MIN:		min(d, s0, s1);									break;
			case Op::OPCODE_MAX:		max(d, s0, s1);									break;
			case Op::OPCODE_LRP:		lrp(d, s0, s1, s2);								break;
			case Op::OPCODE_POW:		pow(d, s0, s1, pp);								break;
			case Op::OPCODE_CRS:		crs(d, s0, s1);									break;
			case Op::OPCODE_NRM:		nrm(d, s0, pp);									break;
			case Op::OPCODE_ABS:		abs(d, s0);										break;
			case Op::OPCODE_SINCOS:		sincos(d, s0, pp);								break;
			case Op::OPCODE_M4X4:		M4X4(r, d, s0, src1);							break;
			case Op::OPCODE_M4X3:		M4X3(r, d, s0, src1);							break;
			case Op::OPCODE_M3X4:		M3X4(r, d, s0, src1);							break;
			case Op::OPCODE_M3X3:		M3X3(r, d, s0, src1);							break;
			case Op::OPCODE_M3X2:		M3X2(r, d, s0, src1);							break;
			case Op::OPCODE_TEX:		TEXLD(r, d, s0, src1, project, bias);			break;
			case Op::OPCODE_TEXLDD:		TEXLDD(r, d, s0, src1, s2, s3, project, bias);	break;
			case Op::OPCODE_TEXLDL:		TEXLDL(r, d, s0, src1, project, bias);			break;
			case Op::OPCODE_TEXKILL:	TEXKILL(cMask, d, dst.mask);					break;
			case Op::OPCODE_DSX:		DSX(d, s0);										break;
			case Op::OPCODE_DSY:		DSY(d, s0);										break;
			case Op::OPCODE_BREAK:		BREAK(r);										break;
			case Op::OPCODE_BREAKC:		BREAKC(r, s0, s1, control);						break;
			case Op::OPCODE_BREAKP:		BREAKP(r, src0);								break;
			case Op::OPCODE_CALL:		CALL(r, dst.index);								break;
			case Op::OPCODE_CALLNZ:		CALLNZ(r, dst.index, src0);						break;
			case Op::OPCODE_ELSE:		ELSE(r);										break;
			case Op::OPCODE_ENDIF:		ENDIF(r);										break;
			case Op::OPCODE_ENDLOOP:	ENDLOOP(r);										break;
			case Op::OPCODE_ENDREP:		ENDREP(r);										break;
			case Op::OPCODE_IF:			IF(r, src0);									break;
			case Op::OPCODE_IFC:		IFC(r, s0, s1, control);						break;
			case Op::OPCODE_LABEL:		LABEL(dst.index);								break;
			case Op::OPCODE_LOOP:		LOOP(r, src1);									break;
			case Op::OPCODE_REP:		REP(r, src0);									break;
			case Op::OPCODE_RET:		RET(r);											break;
			case Op::OPCODE_SETP:		setp(d, s0, s1, control);						break;
			case Op::OPCODE_END:														break;
			default:
				ASSERT(false);
			}

			if(dst.type != Dst::PARAMETER_VOID && dst.type != Dst::PARAMETER_LABEL && opcode != Op::OPCODE_TEXKILL)
			{
				if(dst.saturate)
				{
					if(dst.x) d.r = Max(d.r, Float4(0.0f, 0.0f, 0.0f, 0.0f));
					if(dst.y) d.g = Max(d.g, Float4(0.0f, 0.0f, 0.0f, 0.0f));
					if(dst.z) d.b = Max(d.b, Float4(0.0f, 0.0f, 0.0f, 0.0f));
					if(dst.w) d.a = Max(d.a, Float4(0.0f, 0.0f, 0.0f, 0.0f));

					if(dst.x) d.r = Min(d.r, Float4(1.0f, 1.0f, 1.0f, 1.0f));
					if(dst.y) d.g = Min(d.g, Float4(1.0f, 1.0f, 1.0f, 1.0f));
					if(dst.z) d.b = Min(d.b, Float4(1.0f, 1.0f, 1.0f, 1.0f));
					if(dst.w) d.a = Min(d.a, Float4(1.0f, 1.0f, 1.0f, 1.0f));
				}

				if(pixelShader->containsDynamicBranching())
				{
					Color4f pDst;   // FIXME: Rename

					switch(dst.type)
					{
					case Dst::PARAMETER_TEMP:
						if(dst.x) pDst.x = r.rf[dst.index].x;
						if(dst.y) pDst.y = r.rf[dst.index].y;
						if(dst.z) pDst.z = r.rf[dst.index].z;
						if(dst.w) pDst.w = r.rf[dst.index].w;
						break;
					case Dst::PARAMETER_COLOROUT:
						if(dst.x) pDst.x = r.oC[dst.index].x;
						if(dst.y) pDst.y = r.oC[dst.index].y;
						if(dst.z) pDst.z = r.oC[dst.index].z;
						if(dst.w) pDst.w = r.oC[dst.index].w;
						break;
					case Dst::PARAMETER_PREDICATE:
						if(dst.x) pDst.x = r.p0.x;
						if(dst.y) pDst.y = r.p0.y;
						if(dst.z) pDst.z = r.p0.z;
						if(dst.w) pDst.w = r.p0.w;
						break;
					case Dst::PARAMETER_DEPTHOUT:
						pDst.x = r.oDepth;
						break;
					default:
						ASSERT(false);
					}
				
					Int4 enable = r.enableStack[r.enableIndex] & r.enableBreak;

					Int4 xEnable = enable;
					Int4 yEnable = enable;
					Int4 zEnable = enable;
					Int4 wEnable = enable;

					if(predicate)
					{
						unsigned char pSwizzle = instruction->getPredicateSwizzle();

						Float4 xPredicate = r.p0[(pSwizzle >> 0) & 0x03];
						Float4 yPredicate = r.p0[(pSwizzle >> 2) & 0x03];
						Float4 zPredicate = r.p0[(pSwizzle >> 4) & 0x03];
						Float4 wPredicate = r.p0[(pSwizzle >> 6) & 0x03];

						if(!instruction->isPredicateNot())
						{
							if(dst.x) xEnable = xEnable & As<Int4>(xPredicate);
							if(dst.y) yEnable = yEnable & As<Int4>(yPredicate);
							if(dst.z) zEnable = zEnable & As<Int4>(zPredicate);
							if(dst.w) wEnable = wEnable & As<Int4>(wPredicate);
						}
						else
						{
							if(dst.x) xEnable = xEnable & ~As<Int4>(xPredicate);
							if(dst.y) yEnable = yEnable & ~As<Int4>(yPredicate);
							if(dst.z) zEnable = zEnable & ~As<Int4>(zPredicate);
							if(dst.w) wEnable = wEnable & ~As<Int4>(wPredicate);
						}
					}

					if(dst.x) d.x = As<Float4>(As<Int4>(d.x) & xEnable);
					if(dst.y) d.y = As<Float4>(As<Int4>(d.y) & yEnable);
					if(dst.z) d.z = As<Float4>(As<Int4>(d.z) & zEnable);
					if(dst.w) d.w = As<Float4>(As<Int4>(d.w) & wEnable);

					if(dst.x) d.x = As<Float4>(As<Int4>(d.x) | (As<Int4>(pDst.x) & ~xEnable));
					if(dst.y) d.y = As<Float4>(As<Int4>(d.y) | (As<Int4>(pDst.y) & ~yEnable));
					if(dst.z) d.z = As<Float4>(As<Int4>(d.z) | (As<Int4>(pDst.z) & ~zEnable));
					if(dst.w) d.w = As<Float4>(As<Int4>(d.w) | (As<Int4>(pDst.w) & ~wEnable));
				}

				switch(dst.type)
				{
				case Dst::PARAMETER_TEMP:
					if(dst.x) r.rf[dst.index].x = d.x;
					if(dst.y) r.rf[dst.index].y = d.y;
					if(dst.z) r.rf[dst.index].z = d.z;
					if(dst.w) r.rf[dst.index].w = d.w;
					break;
				case Dst::PARAMETER_COLOROUT:
					if(dst.x) r.oC[dst.index].x = d.x;
					if(dst.y) r.oC[dst.index].y = d.y;
					if(dst.z) r.oC[dst.index].z = d.z;
					if(dst.w) r.oC[dst.index].w = d.w;
					break;
				case Dst::PARAMETER_PREDICATE:
					if(dst.x) r.p0.x = d.x;
					if(dst.y) r.p0.y = d.y;
					if(dst.z) r.p0.z = d.z;
					if(dst.w) r.p0.w = d.w;
					break;
				case Dst::PARAMETER_DEPTHOUT:
					r.oDepth = d.x;
					break;
				default:
					ASSERT(false);
				}
			}
		}

		if(returns)
		{
			Nucleus::setInsertBlock(returnBlock);
		}
	}

	Short4 PixelRoutine::convertFixed12(Float4 &cf)
	{
		return RoundShort4(cf * Float4(0x1000, 0x1000, 0x1000, 0x1000));
	}

	void PixelRoutine::convertFixed12(Color4i &ci, Color4f &cf)
	{
		ci.r = convertFixed12(cf.r);
		ci.g = convertFixed12(cf.g);
		ci.b = convertFixed12(cf.b);
		ci.a = convertFixed12(cf.a);
	}

	UShort4 PixelRoutine::convertFixed16(Float4 &cf, bool saturate)
	{
		return UShort4(cf * Float4(0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF), saturate);
	}

	void PixelRoutine::convertFixed16(Color4i &ci, Color4f &cf, bool saturate)
	{
		ci.r = convertFixed16(cf.r, saturate);
		ci.g = convertFixed16(cf.g, saturate);
		ci.b = convertFixed16(cf.b, saturate);
		ci.a = convertFixed16(cf.a, saturate);
	}

	Float4 PixelRoutine::convertSigned12(Short4 &ci)
	{
		return Float4(ci) * Float4(1.0f / 0x0FFE);
	}

	void PixelRoutine::convertSigned12(Color4f &cf, Color4i &ci)
	{
		cf.r = convertSigned12(ci.r);
		cf.g = convertSigned12(ci.g);
		cf.b = convertSigned12(ci.b);
		cf.a = convertSigned12(ci.a);
	}

	Float4 PixelRoutine::convertUnsigned16(UShort4 ci)
	{
		return Float4(ci) * Float4(1.0f / 0xFFFF, 1.0f / 0xFFFF, 1.0f / 0xFFFF, 1.0f / 0xFFFF);
	}

	void PixelRoutine::sRGBtoLinear16_16(Registers &r, Color4i &c)
	{
		c.r = As<UShort4>(c.r) >> 4;
		c.g = As<UShort4>(c.g) >> 4;
		c.b = As<UShort4>(c.b) >> 4;

		sRGBtoLinear12_16(r, c);
	}

	void PixelRoutine::sRGBtoLinear12_16(Registers &r, Color4i &c)
	{
		Pointer<Byte> LUT = r.constants + OFFSET(Constants,sRGBtoLin12_16);

		c.r = Insert(c.r, *Pointer<Short>(LUT + 2 * Int(Extract(c.r, 0))), 0);
		c.r = Insert(c.r, *Pointer<Short>(LUT + 2 * Int(Extract(c.r, 1))), 1);
		c.r = Insert(c.r, *Pointer<Short>(LUT + 2 * Int(Extract(c.r, 2))), 2);
		c.r = Insert(c.r, *Pointer<Short>(LUT + 2 * Int(Extract(c.r, 3))), 3);

		c.g = Insert(c.g, *Pointer<Short>(LUT + 2 * Int(Extract(c.g, 0))), 0);
		c.g = Insert(c.g, *Pointer<Short>(LUT + 2 * Int(Extract(c.g, 1))), 1);
		c.g = Insert(c.g, *Pointer<Short>(LUT + 2 * Int(Extract(c.g, 2))), 2);
		c.g = Insert(c.g, *Pointer<Short>(LUT + 2 * Int(Extract(c.g, 3))), 3);

		c.b = Insert(c.b, *Pointer<Short>(LUT + 2 * Int(Extract(c.b, 0))), 0);
		c.b = Insert(c.b, *Pointer<Short>(LUT + 2 * Int(Extract(c.b, 1))), 1);
		c.b = Insert(c.b, *Pointer<Short>(LUT + 2 * Int(Extract(c.b, 2))), 2);
		c.b = Insert(c.b, *Pointer<Short>(LUT + 2 * Int(Extract(c.b, 3))), 3);
	}

	void PixelRoutine::linearToSRGB16_16(Registers &r, Color4i &c)
	{
		c.r = As<UShort4>(c.r) >> 4;
		c.g = As<UShort4>(c.g) >> 4;
		c.b = As<UShort4>(c.b) >> 4;

		linearToSRGB12_16(r, c);
	}

	void PixelRoutine::linearToSRGB12_16(Registers &r, Color4i &c)
	{
		Pointer<Byte> LUT = r.constants + OFFSET(Constants,linToSRGB12_16);

		c.r = Insert(c.r, *Pointer<Short>(LUT + 2 * Int(Extract(c.r, 0))), 0);
		c.r = Insert(c.r, *Pointer<Short>(LUT + 2 * Int(Extract(c.r, 1))), 1);
		c.r = Insert(c.r, *Pointer<Short>(LUT + 2 * Int(Extract(c.r, 2))), 2);
		c.r = Insert(c.r, *Pointer<Short>(LUT + 2 * Int(Extract(c.r, 3))), 3);

		c.g = Insert(c.g, *Pointer<Short>(LUT + 2 * Int(Extract(c.g, 0))), 0);
		c.g = Insert(c.g, *Pointer<Short>(LUT + 2 * Int(Extract(c.g, 1))), 1);
		c.g = Insert(c.g, *Pointer<Short>(LUT + 2 * Int(Extract(c.g, 2))), 2);
		c.g = Insert(c.g, *Pointer<Short>(LUT + 2 * Int(Extract(c.g, 3))), 3);

		c.b = Insert(c.b, *Pointer<Short>(LUT + 2 * Int(Extract(c.b, 0))), 0);
		c.b = Insert(c.b, *Pointer<Short>(LUT + 2 * Int(Extract(c.b, 1))), 1);
		c.b = Insert(c.b, *Pointer<Short>(LUT + 2 * Int(Extract(c.b, 2))), 2);
		c.b = Insert(c.b, *Pointer<Short>(LUT + 2 * Int(Extract(c.b, 3))), 3);
	}

	Float4 PixelRoutine::linearToSRGB(const Float4 &x)   // Approximates x^(1.0/2.2)
	{
		Float4 sqrtx = Rcp_pp(RcpSqrt_pp(x));
		Float4 sRGB = sqrtx * Float4(1.14f) - x * Float4(0.14f);

		return Min(Max(sRGB, Float4(0.0f)), Float4(1.0f));
	}

	Float4 PixelRoutine::sRGBtoLinear(const Float4 &x)   // Approximates x^2.2
	{
		Float4 linear = x * x;
		linear = linear * Float4(0.73f) + linear * x * Float4(0.27f);

		return Min(Max(linear, Float4(0.0f)), Float4(1.0f));
	}

	void PixelRoutine::MOV(Color4i &dst, Color4i &src0)
	{
		dst.r = src0.x;
		dst.g = src0.y;
		dst.b = src0.z;
		dst.a = src0.w;
	}

	void PixelRoutine::ADD(Color4i &dst, Color4i &src0, Color4i &src1)
	{
		dst.r = AddSat(src0.x, src1.x);
		dst.g = AddSat(src0.y, src1.y);
		dst.b = AddSat(src0.z, src1.z);
		dst.a = AddSat(src0.w, src1.w);
	}

	void PixelRoutine::SUB(Color4i &dst, Color4i &src0, Color4i &src1)
	{
		dst.r = SubSat(src0.x, src1.x);
		dst.g = SubSat(src0.y, src1.y);
		dst.b = SubSat(src0.z, src1.z);
		dst.a = SubSat(src0.w, src1.w);
	}

	void PixelRoutine::MAD(Color4i &dst, Color4i &src0, Color4i &src1, Color4i &src2)
	{
		// FIXME: Long fixed-point multiply fixup
		{dst.x = MulHigh(src0.x, src1.x); dst.x = AddSat(dst.x, dst.x); dst.x = AddSat(dst.x, dst.x); dst.x = AddSat(dst.x, dst.x); dst.x = AddSat(dst.x, dst.x); dst.x = AddSat(dst.x, src2.x);}
		{dst.y = MulHigh(src0.y, src1.y); dst.y = AddSat(dst.y, dst.y); dst.y = AddSat(dst.y, dst.y); dst.y = AddSat(dst.y, dst.y); dst.y = AddSat(dst.y, dst.y); dst.y = AddSat(dst.y, src2.y);}
		{dst.z = MulHigh(src0.z, src1.z); dst.z = AddSat(dst.z, dst.z); dst.z = AddSat(dst.z, dst.z); dst.z = AddSat(dst.z, dst.z); dst.z = AddSat(dst.z, dst.z); dst.z = AddSat(dst.z, src2.z);}
		{dst.w = MulHigh(src0.w, src1.w); dst.w = AddSat(dst.w, dst.w); dst.w = AddSat(dst.w, dst.w); dst.w = AddSat(dst.w, dst.w); dst.w = AddSat(dst.w, dst.w); dst.w = AddSat(dst.w, src2.w);}
	}

	void PixelRoutine::MUL(Color4i &dst, Color4i &src0, Color4i &src1)
	{
		// FIXME: Long fixed-point multiply fixup
		{dst.x = MulHigh(src0.x, src1.x); dst.x = AddSat(dst.x, dst.x); dst.x = AddSat(dst.x, dst.x); dst.x = AddSat(dst.x, dst.x); dst.x = AddSat(dst.x, dst.x);}
		{dst.y = MulHigh(src0.y, src1.y); dst.y = AddSat(dst.y, dst.y); dst.y = AddSat(dst.y, dst.y); dst.y = AddSat(dst.y, dst.y); dst.y = AddSat(dst.y, dst.y);}
		{dst.z = MulHigh(src0.z, src1.z); dst.z = AddSat(dst.z, dst.z); dst.z = AddSat(dst.z, dst.z); dst.z = AddSat(dst.z, dst.z); dst.z = AddSat(dst.z, dst.z);}
		{dst.w = MulHigh(src0.w, src1.w); dst.w = AddSat(dst.w, dst.w); dst.w = AddSat(dst.w, dst.w); dst.w = AddSat(dst.w, dst.w); dst.w = AddSat(dst.w, dst.w);}
	}

	void PixelRoutine::DP3(Color4i &dst, Color4i &src0, Color4i &src1)
	{
		Short4 t0;
		Short4 t1;

		// FIXME: Long fixed-point multiply fixup
		t0 = MulHigh(src0.x, src1.x); t0 = AddSat(t0, t0); t0 = AddSat(t0, t0); t0 = AddSat(t0, t0); t0 = AddSat(t0, t0); 
		t1 = MulHigh(src0.y, src1.y); t1 = AddSat(t1, t1); t1 = AddSat(t1, t1); t1 = AddSat(t1, t1); t1 = AddSat(t1, t1); 
		t0 = AddSat(t0, t1);
		t1 = MulHigh(src0.z, src1.z); t1 = AddSat(t1, t1); t1 = AddSat(t1, t1); t1 = AddSat(t1, t1); t1 = AddSat(t1, t1); 
		t0 = AddSat(t0, t1);

		dst.r = t0;
		dst.g = t0;
		dst.b = t0;
		dst.a = t0;
	}

	void PixelRoutine::DP4(Color4i &dst, Color4i &src0, Color4i &src1)
	{
		Short4 t0;
		Short4 t1;

		// FIXME: Long fixed-point multiply fixup
		t0 = MulHigh(src0.x, src1.x); t0 = AddSat(t0, t0); t0 = AddSat(t0, t0); t0 = AddSat(t0, t0); t0 = AddSat(t0, t0); 
		t1 = MulHigh(src0.y, src1.y); t1 = AddSat(t1, t1); t1 = AddSat(t1, t1); t1 = AddSat(t1, t1); t1 = AddSat(t1, t1); 
		t0 = AddSat(t0, t1);
		t1 = MulHigh(src0.z, src1.z); t1 = AddSat(t1, t1); t1 = AddSat(t1, t1); t1 = AddSat(t1, t1); t1 = AddSat(t1, t1); 
		t0 = AddSat(t0, t1);
		t1 = MulHigh(src0.w, src1.w); t1 = AddSat(t1, t1); t1 = AddSat(t1, t1); t1 = AddSat(t1, t1); t1 = AddSat(t1, t1); 
		t0 = AddSat(t0, t1);

		dst.r = t0;
		dst.g = t0;
		dst.b = t0;
		dst.a = t0;
	}

	void PixelRoutine::LRP(Color4i &dst, Color4i &src0, Color4i &src1, Color4i &src2)
	{
		// FIXME: Long fixed-point multiply fixup
		{dst.x = SubSat(src1.x, src2.x); dst.x = MulHigh(dst.x, src0.x); dst.x = AddSat(dst.x, dst.x); dst.x = AddSat(dst.x, dst.x); dst.x = AddSat(dst.x, dst.x); dst.x = AddSat(dst.x, dst.x); dst.x = AddSat(dst.x, src2.x);}
		{dst.y = SubSat(src1.y, src2.y); dst.y = MulHigh(dst.y, src0.y); dst.y = AddSat(dst.y, dst.y); dst.y = AddSat(dst.y, dst.y); dst.y = AddSat(dst.y, dst.y); dst.y = AddSat(dst.y, dst.y); dst.y = AddSat(dst.y, src2.y);}
		{dst.z = SubSat(src1.z, src2.z); dst.z = MulHigh(dst.z, src0.z); dst.z = AddSat(dst.z, dst.z); dst.z = AddSat(dst.z, dst.z); dst.z = AddSat(dst.z, dst.z); dst.z = AddSat(dst.z, dst.z); dst.z = AddSat(dst.z, src2.z);}
		{dst.w = SubSat(src1.w, src2.w); dst.w = MulHigh(dst.w, src0.w); dst.w = AddSat(dst.w, dst.w); dst.w = AddSat(dst.w, dst.w); dst.w = AddSat(dst.w, dst.w); dst.w = AddSat(dst.w, dst.w); dst.w = AddSat(dst.w, src2.w);}
	}

	void PixelRoutine::TEXCOORD(Color4i &dst, Float4 &u, Float4 &v, Float4 &s, int coordinate)
	{
		Float4 uw;
		Float4 vw;
		Float4 sw;

		if(state.interpolant[2 + coordinate].component & 0x01)
		{
			uw = Max(u, Float4(0.0f, 0.0f, 0.0f, 0.0f));
			uw = Min(uw, Float4(1.0f, 1.0f, 1.0f, 1.0f));
			dst.r = convertFixed12(uw);
		}
		else
		{
			dst.r = Short4(0x0000, 0x0000, 0x0000, 0x0000);
		}

		if(state.interpolant[2 + coordinate].component & 0x02)
		{
			vw = Max(v, Float4(0.0f, 0.0f, 0.0f, 0.0f));
			vw = Min(vw, Float4(1.0f, 1.0f, 1.0f, 1.0f));
			dst.g = convertFixed12(vw);
		}
		else
		{
			dst.g = Short4(0x0000, 0x0000, 0x0000, 0x0000);
		}

		if(state.interpolant[2 + coordinate].component & 0x04)
		{
			sw = Max(s, Float4(0.0f, 0.0f, 0.0f, 0.0f));
			sw = Min(sw, Float4(1.0f, 1.0f, 1.0f, 1.0f));
			dst.b = convertFixed12(sw);
		}
		else
		{
			dst.b = Short4(0x0000, 0x0000, 0x0000, 0x0000);
		}

		dst.a = Short4(0x1000, 0x1000, 0x1000, 0x1000);
	}

	void PixelRoutine::TEXCRD(Color4i &dst, Float4 &u, Float4 &v, Float4 &s, int coordinate, bool project)
	{
		Float4 uw = u;
		Float4 vw = v;
		Float4 sw = s;

		if(project)
		{
			uw *= Rcp_pp(s);
			vw *= Rcp_pp(s);
		}

		if(state.interpolant[2 + coordinate].component & 0x01)
		{
			uw *= Float4(0x1000, 0x1000, 0x1000, 0x1000);
			uw = Max(uw, Float4(-0x8000, -0x8000, -0x8000, -0x8000));
			uw = Min(uw, Float4(0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF));
			dst.r = RoundShort4(uw);
		}
		else
		{
			dst.r = Short4(0x0000, 0x0000, 0x0000, 0x0000);
		}

		if(state.interpolant[2 + coordinate].component & 0x02)
		{
			vw *= Float4(0x1000, 0x1000, 0x1000, 0x1000);
			vw = Max(vw, Float4(-0x8000, -0x8000, -0x8000, -0x8000));
			vw = Min(vw, Float4(0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF));
			dst.g = RoundShort4(vw);
		}
		else
		{
			dst.g = Short4(0x0000, 0x0000, 0x0000, 0x0000);
		}
		
		if(state.interpolant[2 + coordinate].component & 0x04)
		{
			sw *= Float4(0x1000, 0x1000, 0x1000, 0x1000);
			sw = Max(sw, Float4(-0x8000, -0x8000, -0x8000, -0x8000));
			sw = Min(sw, Float4(0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF));
			dst.b = RoundShort4(sw);
		}
		else
		{
			dst.b = Short4(0x0000, 0x0000, 0x0000, 0x0000);
		}
	}

	void PixelRoutine::TEXDP3(Registers &r, Color4i &dst, Float4 &u, Float4 &v, Float4 &s, Color4i &src)
	{
		TEXM3X3PAD(r, u, v, s, src, 0, false);

		Short4 t0 = RoundShort4(r.u_ * Float4(0x1000, 0x1000, 0x1000, 0x1000));

		dst.r = t0;
		dst.g = t0;
		dst.b = t0;
		dst.a = t0;
	}

	void PixelRoutine::TEXDP3TEX(Registers &r, Color4i &dst, Float4 &u, Float4 &v, Float4 &s, int stage, Color4i &src0)
	{
		TEXM3X3PAD(r, u, v, s, src0, 0, false);

		r.v_ = Float4(0.0f, 0.0f, 0.0f, 0.0f);
		r.w_ = Float4(0.0f, 0.0f, 0.0f, 0.0f);

		sampleTexture(r, dst, stage, r.u_, r.v_, r.w_, r.w_);
	}

	void PixelRoutine::TEXKILL(Int cMask[4], Float4 &u, Float4 &v, Float4 &s)
	{
		Int kill = SignMask(CmpNLT(u, Float4(0, 0, 0, 0))) &
		           SignMask(CmpNLT(v, Float4(0, 0, 0, 0))) &
		           SignMask(CmpNLT(s, Float4(0, 0, 0, 0)));

		for(unsigned int q = 0; q < state.multiSample; q++)
		{
			cMask[q] &= kill;
		}
	}

	void PixelRoutine::TEXKILL(Int cMask[4], Color4i &src)
	{
		Short4 test = src.r | src.g | src.b;
		Int kill = SignMask(Pack(test, test)) ^ 0x0000000F;

		for(unsigned int q = 0; q < state.multiSample; q++)
		{
			cMask[q] &= kill;
		}
	}

	void PixelRoutine::TEX(Registers &r, Color4i &dst, Float4 &u, Float4 &v, Float4 &s, int sampler, bool project)
	{
		sampleTexture(r, dst, sampler, u, v, s, s, project);
	}

	void PixelRoutine::TEXLD(Registers &r, Color4i &dst, Color4i &src, int sampler, bool project)
	{
		Float4 u = Float4(src.r) * Float4(1.0f / 0x0FFE, 1.0f / 0x0FFE, 1.0f / 0x0FFE, 1.0f / 0x0FFE);
		Float4 v = Float4(src.g) * Float4(1.0f / 0x0FFE, 1.0f / 0x0FFE, 1.0f / 0x0FFE, 1.0f / 0x0FFE);
		Float4 s = Float4(src.b) * Float4(1.0f / 0x0FFE, 1.0f / 0x0FFE, 1.0f / 0x0FFE, 1.0f / 0x0FFE);

		sampleTexture(r, dst, sampler, u, v, s, s, project);
	}

	void PixelRoutine::TEXBEM(Registers &r, Color4i &dst, Color4i &src, Float4 &u, Float4 &v, Float4 &s, int stage)
	{
		Float4 du = Float4(src.r) * Float4(1.0f / 0x0FFE, 1.0f / 0x0FFE, 1.0f / 0x0FFE, 1.0f / 0x0FFE);
		Float4 dv = Float4(src.g) * Float4(1.0f / 0x0FFE, 1.0f / 0x0FFE, 1.0f / 0x0FFE, 1.0f / 0x0FFE);

		Float4 du2 = du;
		Float4 dv2 = dv;

		du *= *Pointer<Float4>(r.data + OFFSET(DrawData,textureStage[stage].bumpmapMatrix4F[0][0]));
		dv2 *= *Pointer<Float4>(r.data + OFFSET(DrawData,textureStage[stage].bumpmapMatrix4F[1][0]));
		du += dv2;
		dv *= *Pointer<Float4>(r.data + OFFSET(DrawData,textureStage[stage].bumpmapMatrix4F[1][1]));
		du2 *= *Pointer<Float4>(r.data + OFFSET(DrawData,textureStage[stage].bumpmapMatrix4F[0][1]));
		dv += du2;

		Float4 u_ = u + du;
		Float4 v_ = v + dv;

		sampleTexture(r, dst, stage, u_, v_, s, s);
	}

	void PixelRoutine::TEXBEML(Registers &r, Color4i &dst, Color4i &src, Float4 &u, Float4 &v, Float4 &s, int stage)
	{
		Float4 du = Float4(src.r) * Float4(1.0f / 0x0FFE, 1.0f / 0x0FFE, 1.0f / 0x0FFE, 1.0f / 0x0FFE);
		Float4 dv = Float4(src.g) * Float4(1.0f / 0x0FFE, 1.0f / 0x0FFE, 1.0f / 0x0FFE, 1.0f / 0x0FFE);

		Float4 du2 = du;
		Float4 dv2 = dv;

		du *= *Pointer<Float4>(r.data + OFFSET(DrawData,textureStage[stage].bumpmapMatrix4F[0][0]));
		dv2 *= *Pointer<Float4>(r.data + OFFSET(DrawData,textureStage[stage].bumpmapMatrix4F[1][0]));
		du += dv2;
		dv *= *Pointer<Float4>(r.data + OFFSET(DrawData,textureStage[stage].bumpmapMatrix4F[1][1]));
		du2 *= *Pointer<Float4>(r.data + OFFSET(DrawData,textureStage[stage].bumpmapMatrix4F[0][1]));
		dv += du2;

		Float4 u_ = u + du;
		Float4 v_ = v + dv;

		sampleTexture(r, dst, stage, u_, v_, s, s);

		Short4 L;

		L = src.b;
		L = MulHigh(L, *Pointer<Short4>(r.data + OFFSET(DrawData,textureStage[stage].luminanceScale4)));
		L = L << 4;
		L = AddSat(L, *Pointer<Short4>(r.data + OFFSET(DrawData,textureStage[stage].luminanceOffset4)));
		L = Max(L, Short4(0x0000, 0x0000, 0x0000, 0x0000));
		L = Min(L, Short4(0x1000, 0x1000, 0x1000, 0x1000));

		dst.r = MulHigh(dst.r, L); dst.r = dst.r << 4;
		dst.g = MulHigh(dst.g, L); dst.g = dst.g << 4;
		dst.b = MulHigh(dst.b, L); dst.b = dst.b << 4;
	}

	void PixelRoutine::TEXREG2AR(Registers &r, Color4i &dst, Color4i &src0, int stage)
	{
		Float4 u = Float4(src0.a) * Float4(1.0f / 0x0FFE, 1.0f / 0x0FFE, 1.0f / 0x0FFE, 1.0f / 0x0FFE);
		Float4 v = Float4(src0.r) * Float4(1.0f / 0x0FFE, 1.0f / 0x0FFE, 1.0f / 0x0FFE, 1.0f / 0x0FFE);
		Float4 s = Float4(src0.b) * Float4(1.0f / 0x0FFE, 1.0f / 0x0FFE, 1.0f / 0x0FFE, 1.0f / 0x0FFE);

		sampleTexture(r, dst, stage, u, v, s, s);
	}

	void PixelRoutine::TEXREG2GB(Registers &r, Color4i &dst, Color4i &src0, int stage)
	{
		Float4 u = Float4(src0.g) * Float4(1.0f / 0x0FFE, 1.0f / 0x0FFE, 1.0f / 0x0FFE, 1.0f / 0x0FFE);
		Float4 v = Float4(src0.b) * Float4(1.0f / 0x0FFE, 1.0f / 0x0FFE, 1.0f / 0x0FFE, 1.0f / 0x0FFE);
		Float4 s = v;

		sampleTexture(r, dst, stage, u, v, s, s);
	}

	void PixelRoutine::TEXREG2RGB(Registers &r, Color4i &dst, Color4i &src0, int stage)
	{
		Float4 u = Float4(src0.r) * Float4(1.0f / 0x0FFE, 1.0f / 0x0FFE, 1.0f / 0x0FFE, 1.0f / 0x0FFE);
		Float4 v = Float4(src0.g) * Float4(1.0f / 0x0FFE, 1.0f / 0x0FFE, 1.0f / 0x0FFE, 1.0f / 0x0FFE);
		Float4 s = Float4(src0.b) * Float4(1.0f / 0x0FFE, 1.0f / 0x0FFE, 1.0f / 0x0FFE, 1.0f / 0x0FFE);

		sampleTexture(r, dst, stage, u, v, s, s);
	}

	void PixelRoutine::TEXM3X2DEPTH(Registers &r, Color4i &dst, Float4 &u, Float4 &v, Float4 &s, Color4i &src, bool signedScaling)
	{
		TEXM3X2PAD(r, u, v, s, src, 1, signedScaling);

		// z / w
		r.u_ *= Rcp_pp(r.v_);   // FIXME: Set result to 1.0 when division by zero

		r.oDepth = r.u_;
	}

	void PixelRoutine::TEXM3X2PAD(Registers &r, Float4 &u, Float4 &v, Float4 &s, Color4i &src0, int component, bool signedScaling)
	{
		TEXM3X3PAD(r, u, v, s, src0, component, signedScaling);
	}

	void PixelRoutine::TEXM3X2TEX(Registers &r, Color4i &dst, Float4 &u, Float4 &v, Float4 &s, int stage, Color4i &src0, bool signedScaling)
	{
		TEXM3X2PAD(r, u, v, s, src0, 1, signedScaling);

		r.w_ = Float4(0.0f, 0.0f, 0.0f, 0.0f);

		sampleTexture(r, dst, stage, r.u_, r.v_, r.w_, r.w_);
	}

	void PixelRoutine::TEXM3X3(Registers &r, Color4i &dst, Float4 &u, Float4 &v, Float4 &s, Color4i &src0, bool signedScaling)
	{
		TEXM3X3PAD(r, u, v, s, src0, 2, signedScaling);

		dst.r = RoundShort4(r.u_ * Float4(0x1000, 0x1000, 0x1000, 0x1000));
		dst.g = RoundShort4(r.v_ * Float4(0x1000, 0x1000, 0x1000, 0x1000));
		dst.b = RoundShort4(r.w_ * Float4(0x1000, 0x1000, 0x1000, 0x1000));
		dst.a = Short4(0x1000, 0x1000, 0x1000, 0x1000);
	}

	void PixelRoutine::TEXM3X3PAD(Registers &r, Float4 &u, Float4 &v, Float4 &s, Color4i &src0, int component, bool signedScaling)
	{
		if(component == 0 || previousScaling != signedScaling)   // FIXME: Other source modifiers?
		{
			r.U = Float4(src0.r);
			r.V = Float4(src0.g);
			r.W = Float4(src0.b);

			previousScaling = signedScaling;
		}

		Float4 x = r.U * u + r.V * v + r.W * s;

		x *= Float4(1.0f / 0x1000, 1.0f / 0x1000, 1.0f / 0x1000, 1.0f / 0x1000);

		switch(component)
		{
		case 0:	r.u_ = x; break;
		case 1:	r.v_ = x; break;
		case 2: r.w_ = x; break;
		default: ASSERT(false);
		}
	}

	void PixelRoutine::TEXM3X3SPEC(Registers &r, Color4i &dst, Float4 &u, Float4 &v, Float4 &s, int stage, Color4i &src0, Color4i &src1)
	{
		TEXM3X3PAD(r, u, v, s, src0, 2, false);

		Float4 E[3];   // Eye vector

		E[0] = Float4(src1.r) * Float4(1.0f / 0x0FFE, 1.0f / 0x0FFE, 1.0f / 0x0FFE, 1.0f / 0x0FFE);
		E[1] = Float4(src1.g) * Float4(1.0f / 0x0FFE, 1.0f / 0x0FFE, 1.0f / 0x0FFE, 1.0f / 0x0FFE);
		E[2] = Float4(src1.b) * Float4(1.0f / 0x0FFE, 1.0f / 0x0FFE, 1.0f / 0x0FFE, 1.0f / 0x0FFE);

		// Reflection
		Float4 u__;
		Float4 v__;
		Float4 w__;

		// (u'', v'', w'') = 2 * (N . E) * N - E * (N . N)
		u__ = r.u_ * E[0];
		v__ = r.v_ * E[1];
		w__ = r.w_ * E[2];
		u__ += v__ + w__;
		u__ += u__;
		v__ = u__;
		w__ = u__;
		u__ *= r.u_;
		v__ *= r.v_;
		w__ *= r.w_;
		r.u_ *= r.u_;
		r.v_ *= r.v_;
		r.w_ *= r.w_;
		r.u_ += r.v_ + r.w_;
		u__ -= E[0] * r.u_;
		v__ -= E[1] * r.u_;
		w__ -= E[2] * r.u_;

		sampleTexture(r, dst, stage,  u__, v__, w__, w__);
	}

	void PixelRoutine::TEXM3X3TEX(Registers &r, Color4i &dst, Float4 &u, Float4 &v, Float4 &s, int stage, Color4i &src0, bool signedScaling)
	{
		TEXM3X3PAD(r, u, v, s, src0, 2, signedScaling);

		sampleTexture(r, dst, stage, r.u_, r.v_, r.w_, r.w_);
	}

	void PixelRoutine::TEXM3X3VSPEC(Registers &r, Color4i &dst, Float4 &u, Float4 &v, Float4 &s, int stage, Color4i &src0)
	{
		TEXM3X3PAD(r, u, v, s, src0, 2, false);

		Float4 E[3];   // Eye vector

		E[0] = r.vw[2 + stage - 2];
		E[1] = r.vw[2 + stage - 1];
		E[2] = r.vw[2 + stage - 0];

		// Reflection
		Float4 u__;
		Float4 v__;
		Float4 w__;

		// (u'', v'', w'') = 2 * (N . E) * N - E * (N . N)
		u__ = r.u_ * E[0];
		v__ = r.v_ * E[1];
		w__ = r.w_ * E[2];
		u__ += v__ + w__;
		u__ += u__;
		v__ = u__;
		w__ = u__;
		u__ *= r.u_;
		v__ *= r.v_;
		w__ *= r.w_;
		r.u_ *= r.u_;
		r.v_ *= r.v_;
		r.w_ *= r.w_;
		r.u_ += r.v_ + r.w_;
		u__ -= E[0] * r.u_;
		v__ -= E[1] * r.u_;
		w__ -= E[2] * r.u_;

		sampleTexture(r, dst, stage, u__, v__, w__, w__);
	}

	void PixelRoutine::TEXDEPTH(Registers &r)
	{
		r.u_ = Float4(r.ri[5].r);
		r.v_ = Float4(r.ri[5].g);

		// z / w
		r.u_ *= Rcp_pp(r.v_);   // FIXME: Set result to 1.0 when division by zero

		r.oDepth = r.u_;
	}

	void PixelRoutine::CND(Color4i &dst, Color4i &src0, Color4i &src1, Color4i &src2)
	{
		{Short4 t0; t0 = src0.x; t0 = CmpGT(t0, Short4(0x0800, 0x0800, 0x0800, 0x0800)); Short4 t1; t1 = src1.x; t1 = t1 & t0; t0 = ~t0 & src2.x; t0 = t0 | t1; dst.r = t0;};
		{Short4 t0; t0 = src0.y; t0 = CmpGT(t0, Short4(0x0800, 0x0800, 0x0800, 0x0800)); Short4 t1; t1 = src1.y; t1 = t1 & t0; t0 = ~t0 & src2.y; t0 = t0 | t1; dst.g = t0;};
		{Short4 t0; t0 = src0.z; t0 = CmpGT(t0, Short4(0x0800, 0x0800, 0x0800, 0x0800)); Short4 t1; t1 = src1.z; t1 = t1 & t0; t0 = ~t0 & src2.z; t0 = t0 | t1; dst.b = t0;};
		{Short4 t0; t0 = src0.w; t0 = CmpGT(t0, Short4(0x0800, 0x0800, 0x0800, 0x0800)); Short4 t1; t1 = src1.w; t1 = t1 & t0; t0 = ~t0 & src2.w; t0 = t0 | t1; dst.a = t0;};
	}

	void PixelRoutine::CMP(Color4i &dst, Color4i &src0, Color4i &src1, Color4i &src2)
	{
		{Short4 t0 = CmpGT(Short4(0x0000, 0x0000, 0x0000, 0x0000), src0.x); Short4 t1; t1 = src2.x; t1 &= t0; t0 = ~t0 & src1.x; t0 |= t1; dst.r = t0;};
		{Short4 t0 = CmpGT(Short4(0x0000, 0x0000, 0x0000, 0x0000), src0.y); Short4 t1; t1 = src2.y; t1 &= t0; t0 = ~t0 & src1.y; t0 |= t1; dst.g = t0;};
		{Short4 t0 = CmpGT(Short4(0x0000, 0x0000, 0x0000, 0x0000), src0.z); Short4 t1; t1 = src2.z; t1 &= t0; t0 = ~t0 & src1.z; t0 |= t1; dst.b = t0;};
		{Short4 t0 = CmpGT(Short4(0x0000, 0x0000, 0x0000, 0x0000), src0.w); Short4 t1; t1 = src2.w; t1 &= t0; t0 = ~t0 & src1.w; t0 |= t1; dst.a = t0;};
	}

	void PixelRoutine::BEM(Registers &r, Color4i &dst, Color4i &src0, Color4i &src1, int stage)
	{
		Short4 t0;
		Short4 t1;

		// dst.r = src0.r + BUMPENVMAT00(stage) * src1.r + BUMPENVMAT10(stage) * src1.g
		t0 = MulHigh(src1.x, *Pointer<Short4>(r.data + OFFSET(DrawData,textureStage[stage].bumpmapMatrix4W[0][0]))); t0 = t0 << 4;   // FIXME: Matrix components range? Overflow hazard.
		t1 = MulHigh(src1.y, *Pointer<Short4>(r.data + OFFSET(DrawData,textureStage[stage].bumpmapMatrix4W[1][0]))); t1 = t1 << 4;   // FIXME: Matrix components range? Overflow hazard.
		t0 = AddSat(t0, t1);
		t0 = AddSat(t0, src0.x);
		dst.r = t0;

		// dst.g = src0.g + BUMPENVMAT01(stage) * src1.r + BUMPENVMAT11(stage) * src1.g
		t0 = MulHigh(src1.x, *Pointer<Short4>(r.data + OFFSET(DrawData,textureStage[stage].bumpmapMatrix4W[0][1]))); t0 = t0 << 4;   // FIXME: Matrix components range? Overflow hazard.
		t1 = MulHigh(src1.y, *Pointer<Short4>(r.data + OFFSET(DrawData,textureStage[stage].bumpmapMatrix4W[1][1]))); t1 = t1 << 4;   // FIXME: Matrix components range? Overflow hazard.
		t0 = AddSat(t0, t1);
		t0 = AddSat(t0, src0.y);
		dst.g = t0;
	}

	void PixelRoutine::M3X2(Registers &r, Color4f &dst, Color4f &src0, const Src &src1)
	{
		Color4f row0 = reg(r, src1, 0);
		Color4f row1 = reg(r, src1, 1);

		dst.x = dot3(src0, row0);
		dst.y = dot3(src0, row1);
	}

	void PixelRoutine::M3X3(Registers &r, Color4f &dst, Color4f &src0, const Src &src1)
	{
		Color4f row0 = reg(r, src1, 0);
		Color4f row1 = reg(r, src1, 1);
		Color4f row2 = reg(r, src1, 2);

		dst.x = dot3(src0, row0);
		dst.y = dot3(src0, row1);
		dst.z = dot3(src0, row2);
	}

	void PixelRoutine::M3X4(Registers &r, Color4f &dst, Color4f &src0, const Src &src1)
	{
		Color4f row0 = reg(r, src1, 0);
		Color4f row1 = reg(r, src1, 1);
		Color4f row2 = reg(r, src1, 2);
		Color4f row3 = reg(r, src1, 3);

		dst.x = dot3(src0, row0);
		dst.y = dot3(src0, row1);
		dst.z = dot3(src0, row2);
		dst.w = dot3(src0, row3);
	}

	void PixelRoutine::M4X3(Registers &r, Color4f &dst, Color4f &src0, const Src &src1)
	{
		Color4f row0 = reg(r, src1, 0);
		Color4f row1 = reg(r, src1, 1);
		Color4f row2 = reg(r, src1, 2);

		dst.x = dot4(src0, row0);
		dst.y = dot4(src0, row1);
		dst.z = dot4(src0, row2);
	}

	void PixelRoutine::M4X4(Registers &r, Color4f &dst, Color4f &src0, const Src &src1)
	{
		Color4f row0 = reg(r, src1, 0);
		Color4f row1 = reg(r, src1, 1);
		Color4f row2 = reg(r, src1, 2);
		Color4f row3 = reg(r, src1, 3);

		dst.x = dot4(src0, row0);
		dst.y = dot4(src0, row1);
		dst.z = dot4(src0, row2);
		dst.w = dot4(src0, row3);
	}

	void PixelRoutine::TEXLD(Registers &r, Color4f &dst, Color4f &src0, const Src &src1, bool project, bool bias)
	{
		Color4f tmp;

		sampleTexture(r, tmp, src1.index, src0.u, src0.v, src0.s, src0.t, src0, src0, project, bias);

		dst.x = tmp[(src1.swizzle >> 0) & 0x3];
		dst.y = tmp[(src1.swizzle >> 2) & 0x3];
		dst.z = tmp[(src1.swizzle >> 4) & 0x3];
		dst.w = tmp[(src1.swizzle >> 6) & 0x3];
	}
	
	void PixelRoutine::TEXLDD(Registers &r, Color4f &dst, Color4f &src0, const Src &src1, Color4f &src2,  Color4f &src3, bool project, bool bias)
	{
		Color4f tmp;

		sampleTexture(r, tmp, src1.index, src0.u, src0.v, src0.s, src0.t, src2, src3, project, bias, true);

		dst.x = tmp[(src1.swizzle >> 0) & 0x3];
		dst.y = tmp[(src1.swizzle >> 2) & 0x3];
		dst.z = tmp[(src1.swizzle >> 4) & 0x3];
		dst.w = tmp[(src1.swizzle >> 6) & 0x3];
	}
	
	void PixelRoutine::TEXLDL(Registers &r, Color4f &dst, Color4f &src0, const Src &src1, bool project, bool bias)
	{
		Color4f tmp;

		sampleTexture(r, tmp, src1.index, src0.u, src0.v, src0.s, src0.t, src0, src0, project, bias, false, true);

		dst.x = tmp[(src1.swizzle >> 0) & 0x3];
		dst.y = tmp[(src1.swizzle >> 2) & 0x3];
		dst.z = tmp[(src1.swizzle >> 4) & 0x3];
		dst.w = tmp[(src1.swizzle >> 6) & 0x3];
	}

	void PixelRoutine::TEXKILL(Int cMask[4], Color4f &src, unsigned char mask)
	{
		Int kill = -1;
		
		if(mask & 0x1) kill &= SignMask(CmpNLT(src.x, Float4(0, 0, 0, 0)));
		if(mask & 0x2) kill &= SignMask(CmpNLT(src.y, Float4(0, 0, 0, 0)));
		if(mask & 0x4) kill &= SignMask(CmpNLT(src.z, Float4(0, 0, 0, 0)));
		if(mask & 0x8) kill &= SignMask(CmpNLT(src.w, Float4(0, 0, 0, 0)));

		for(unsigned int q = 0; q < state.multiSample; q++)
		{
			cMask[q] &= kill;
		}
	}

	void PixelRoutine::DSX(Color4f &dst, Color4f &src)
	{
		dst.x = src.x.yyyy - src.x.xxxx;
		dst.y = src.y.yyyy - src.y.xxxx;
		dst.z = src.z.yyyy - src.z.xxxx;
		dst.w = src.w.yyyy - src.w.xxxx;
	}

	void PixelRoutine::DSY(Color4f &dst, Color4f &src)
	{
		dst.x = src.x.zzzz - src.x.xxxx;
		dst.y = src.y.zzzz - src.y.xxxx;
		dst.z = src.z.zzzz - src.z.xxxx;
		dst.w = src.w.zzzz - src.w.xxxx;
	}

	void PixelRoutine::BREAK(Registers &r)
	{
		llvm::BasicBlock *deadBlock = Nucleus::createBasicBlock();
		llvm::BasicBlock *endBlock = loopRepEndBlock[loopRepDepth - 1];

		if(breakDepth == 0)
		{
			Nucleus::createBr(endBlock);
		}
		else
		{
			r.enableBreak = r.enableBreak & ~r.enableStack[r.enableIndex];
			Bool allBreak = SignMask(r.enableBreak) == 0x0;

			branch(allBreak, endBlock, deadBlock);
		}

		Nucleus::setInsertBlock(deadBlock);
	}

	void PixelRoutine::BREAKC(Registers &r, Color4f &src0, Color4f &src1, Control control)
	{
		Int4 condition;

		switch(control)
		{
		case Op::CONTROL_GT: condition = CmpNLE(src0.x,  src1.x);	break;
		case Op::CONTROL_EQ: condition = CmpEQ(src0.x, src1.x);		break;
		case Op::CONTROL_GE: condition = CmpNLT(src0.x, src1.x);	break;
		case Op::CONTROL_LT: condition = CmpLT(src0.x,  src1.x);	break;
		case Op::CONTROL_NE: condition = CmpNEQ(src0.x, src1.x);	break;
		case Op::CONTROL_LE: condition = CmpLE(src0.x, src1.x);		break;
		default:
			ASSERT(false);
		}

		condition &= r.enableStack[r.enableIndex];

		llvm::BasicBlock *continueBlock = Nucleus::createBasicBlock();
		llvm::BasicBlock *endBlock = loopRepEndBlock[loopRepDepth - 1];

		r.enableBreak = r.enableBreak & ~condition;
		Bool allBreak = SignMask(r.enableBreak) == 0x0;

		branch(allBreak, endBlock, continueBlock);
		Nucleus::setInsertBlock(continueBlock);
	}

	void PixelRoutine::BREAKP(Registers &r, const Src &predicateRegister)   // FIXME: Factor out parts common with BREAKC
	{
		Int4 condition = As<Int4>(r.p0[predicateRegister.swizzle & 0x3]);

		if(predicateRegister.modifier == Src::MODIFIER_NOT)
		{
			condition = ~condition;
		}

		condition &= r.enableStack[r.enableIndex];

		llvm::BasicBlock *continueBlock = Nucleus::createBasicBlock();
		llvm::BasicBlock *endBlock = loopRepEndBlock[loopRepDepth - 1];

		r.enableBreak = r.enableBreak & ~condition;
		Bool allBreak = SignMask(r.enableBreak) == 0x0;

		branch(allBreak, endBlock, continueBlock);
		Nucleus::setInsertBlock(continueBlock);
	}

	void PixelRoutine::CALL(Registers &r, int labelIndex)
	{
		if(!labelBlock[labelIndex])
		{
			labelBlock[labelIndex] = Nucleus::createBasicBlock();
		}

		llvm::BasicBlock *retBlock = Nucleus::createBasicBlock();
		callRetBlock.push_back(retBlock);

		r.callStack[r.stackIndex++] = UInt((int)callRetBlock.size() - 1);   // FIXME

		Nucleus::createBr(labelBlock[labelIndex]);
		Nucleus::setInsertBlock(retBlock);
	}

	void PixelRoutine::CALLNZ(Registers &r, int labelIndex, const Src &src)
	{
		if(src.type == Src::PARAMETER_CONSTBOOL)
		{
			CALLNZb(r, labelIndex, src);
		}
		else if(src.type == Src::PARAMETER_PREDICATE)
		{
			CALLNZp(r, labelIndex, src);
		}
		else ASSERT(false);
	}

	void PixelRoutine::CALLNZb(Registers &r, int labelIndex, const Src &boolRegister)
	{
		Bool condition = (*Pointer<Byte>(r.data + OFFSET(DrawData,ps.b[boolRegister.index])) != Byte(0));   // FIXME
		
		if(boolRegister.modifier == Src::MODIFIER_NOT)
		{
			condition = !condition;	
		}

		if(!labelBlock[labelIndex])
		{
			labelBlock[labelIndex] = Nucleus::createBasicBlock();
		}

		llvm::BasicBlock *retBlock = Nucleus::createBasicBlock();
		callRetBlock.push_back(retBlock);

		r.callStack[r.stackIndex++] = UInt((int)callRetBlock.size() - 1);   // FIXME

		branch(condition, labelBlock[labelIndex], retBlock);
		Nucleus::setInsertBlock(retBlock);
	}

	void PixelRoutine::CALLNZp(Registers &r, int labelIndex, const Src &predicateRegister)
	{
		Int4 condition = As<Int4>(r.p0[predicateRegister.swizzle & 0x3]);

		if(predicateRegister.modifier == Src::MODIFIER_NOT)
		{
			condition = ~condition;
		}

		condition &= r.enableStack[r.enableIndex];

		if(!labelBlock[labelIndex])
		{
			labelBlock[labelIndex] = Nucleus::createBasicBlock();
		}

		llvm::BasicBlock *retBlock = Nucleus::createBasicBlock();
		callRetBlock.push_back(retBlock);

		r.callStack[r.stackIndex++] = UInt((int)callRetBlock.size() - 1);   // FIXME

		r.enableIndex++;
		r.enableStack[r.enableIndex] = condition;

		Bool notAllFalse = SignMask(condition & r.enableBreak) != 0;

		branch(notAllFalse, labelBlock[labelIndex], retBlock);
		Nucleus::setInsertBlock(retBlock);

		r.enableIndex--;
	}

	void PixelRoutine::ELSE(Registers &r)
	{
		ifDepth--;

		llvm::BasicBlock *falseBlock = ifFalseBlock[ifDepth];
		llvm::BasicBlock *endBlock = Nucleus::createBasicBlock();

		if(isConditionalIf[ifDepth])
		{
			Int4 condition = ~r.enableStack[r.enableIndex] & r.enableStack[r.enableIndex - 1];
			Bool notAllFalse = SignMask(condition & r.enableBreak) != 0;

			branch(notAllFalse, falseBlock, endBlock);

			r.enableStack[r.enableIndex] = ~r.enableStack[r.enableIndex] & r.enableStack[r.enableIndex - 1];
		}
		else
		{
			Nucleus::createBr(endBlock);
			Nucleus::setInsertBlock(falseBlock);
		}

		ifFalseBlock[ifDepth] = endBlock;

		ifDepth++;
	}

	void PixelRoutine::ENDIF(Registers &r)
	{
		ifDepth--;

		llvm::BasicBlock *endBlock = ifFalseBlock[ifDepth];

		Nucleus::createBr(endBlock);
		Nucleus::setInsertBlock(endBlock);

		if(isConditionalIf[ifDepth])
		{
			breakDepth--;
			r.enableIndex--;
		}
	}

	void PixelRoutine::ENDREP(Registers &r)
	{
		loopRepDepth--;

		llvm::BasicBlock *testBlock = loopRepTestBlock[loopRepDepth];
		llvm::BasicBlock *endBlock = loopRepEndBlock[loopRepDepth];

		Nucleus::createBr(testBlock);
		Nucleus::setInsertBlock(endBlock);

		r.loopDepth--;
		r.enableBreak = Int4(0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF);
	}

	void PixelRoutine::ENDLOOP(Registers &r)
	{
		loopRepDepth--;

		r.aL[r.loopDepth] = r.aL[r.loopDepth] + r.increment[r.loopDepth];   // FIXME: +=

		llvm::BasicBlock *testBlock = loopRepTestBlock[loopRepDepth];
		llvm::BasicBlock *endBlock = loopRepEndBlock[loopRepDepth];

		Nucleus::createBr(testBlock);
		Nucleus::setInsertBlock(endBlock);

		r.loopDepth--;
		r.enableBreak = Int4(0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF);
	}

	void PixelRoutine::IF(Registers &r, const Src &src)
	{
		if(src.type == Src::PARAMETER_CONSTBOOL)
		{
			IFb(r, src);
		}
		else if(src.type == Src::PARAMETER_PREDICATE)
		{
			IFp(r, src);
		}
		else ASSERT(false);
	}

	void PixelRoutine::IFb(Registers &r, const Src &boolRegister)
	{
		Bool condition = (*Pointer<Byte>(r.data + OFFSET(DrawData,ps.b[boolRegister.index])) != Byte(0));   // FIXME

		if(boolRegister.modifier == Src::MODIFIER_NOT)
		{
			condition = !condition;	
		}

		llvm::BasicBlock *trueBlock = Nucleus::createBasicBlock();
		llvm::BasicBlock *falseBlock = Nucleus::createBasicBlock();

		branch(condition, trueBlock, falseBlock);

		isConditionalIf[ifDepth] = false;
		ifFalseBlock[ifDepth] = falseBlock;

		ifDepth++;
	}

	void PixelRoutine::IFp(Registers &r, const Src &predicateRegister)   // FIXME: Factor out parts common with IFC
	{
		Int4 condition = As<Int4>(r.p0[predicateRegister.swizzle & 0x3]);

		if(predicateRegister.modifier == Src::MODIFIER_NOT)
		{
			condition = ~condition;
		}

		condition &= r.enableStack[r.enableIndex];

		r.enableIndex++;
		r.enableStack[r.enableIndex] = condition;

		llvm::BasicBlock *trueBlock = Nucleus::createBasicBlock();
		llvm::BasicBlock *falseBlock = Nucleus::createBasicBlock();

		Bool notAllFalse = SignMask(condition & r.enableBreak) != 0;

		branch(notAllFalse, trueBlock, falseBlock);

		isConditionalIf[ifDepth] = true;
		ifFalseBlock[ifDepth] = falseBlock;

		ifDepth++;
		breakDepth++;
	}

	void PixelRoutine::IFC(Registers &r, Color4f &src0, Color4f &src1, Control control)
	{
		Int4 condition;

		switch(control)
		{
		case Op::CONTROL_GT: condition = CmpNLE(src0.x,  src1.x);	break;
		case Op::CONTROL_EQ: condition = CmpEQ(src0.x, src1.x);		break;
		case Op::CONTROL_GE: condition = CmpNLT(src0.x, src1.x);	break;
		case Op::CONTROL_LT: condition = CmpLT(src0.x,  src1.x);	break;
		case Op::CONTROL_NE: condition = CmpNEQ(src0.x, src1.x);	break;
		case Op::CONTROL_LE: condition = CmpLE(src0.x, src1.x);		break;
		default:
			ASSERT(false);
		}

		condition &= r.enableStack[r.enableIndex];

		r.enableIndex++;
		r.enableStack[r.enableIndex] = condition;

		llvm::BasicBlock *trueBlock = Nucleus::createBasicBlock();
		llvm::BasicBlock *falseBlock = Nucleus::createBasicBlock();

		Bool notAllFalse = SignMask(condition & r.enableBreak) != 0;

		branch(notAllFalse, trueBlock, falseBlock);

		isConditionalIf[ifDepth] = true;
		ifFalseBlock[ifDepth] = falseBlock;

		ifDepth++;
		breakDepth++;
	}

	void PixelRoutine::LABEL(int labelIndex)
	{
		Nucleus::setInsertBlock(labelBlock[labelIndex]);
	}

	void PixelRoutine::LOOP(Registers &r, const Src &integerRegister)
	{
		r.loopDepth++;

		r.iteration[r.loopDepth] = *Pointer<Int>(r.data + OFFSET(DrawData,ps.i[integerRegister.index][0]));
		r.aL[r.loopDepth] = *Pointer<Int>(r.data + OFFSET(DrawData,ps.i[integerRegister.index][1]));
		r.increment[r.loopDepth] = *Pointer<Int>(r.data + OFFSET(DrawData,ps.i[integerRegister.index][2]));

	//	If(r.increment[r.loopDepth] == 0)
	//	{
	//		r.increment[r.loopDepth] = 1;
	//	}

		llvm::BasicBlock *loopBlock = Nucleus::createBasicBlock();
		llvm::BasicBlock *testBlock = Nucleus::createBasicBlock();
		llvm::BasicBlock *endBlock = Nucleus::createBasicBlock();

		loopRepTestBlock[loopRepDepth] = testBlock;
		loopRepEndBlock[loopRepDepth] = endBlock;

		// FIXME: jump(testBlock)
		Nucleus::createBr(testBlock);
		Nucleus::setInsertBlock(testBlock);

		branch(r.iteration[r.loopDepth] > 0, loopBlock, endBlock);
		Nucleus::setInsertBlock(loopBlock);

		r.iteration[r.loopDepth] = r.iteration[r.loopDepth] - 1;   // FIXME: --
		
		loopRepDepth++;
		breakDepth = 0;
	}

	void PixelRoutine::REP(Registers &r, const Src &integerRegister)
	{
		r.loopDepth++;

		r.iteration[r.loopDepth] = *Pointer<Int>(r.data + OFFSET(DrawData,ps.i[integerRegister.index][0]));
		r.aL[r.loopDepth] = r.aL[r.loopDepth - 1];

		llvm::BasicBlock *loopBlock = Nucleus::createBasicBlock();
		llvm::BasicBlock *testBlock = Nucleus::createBasicBlock();
		llvm::BasicBlock *endBlock = Nucleus::createBasicBlock();

		loopRepTestBlock[loopRepDepth] = testBlock;
		loopRepEndBlock[loopRepDepth] = endBlock;

		// FIXME: jump(testBlock)
		Nucleus::createBr(testBlock);
		Nucleus::setInsertBlock(testBlock);

		branch(r.iteration[r.loopDepth] > 0, loopBlock, endBlock);
		Nucleus::setInsertBlock(loopBlock);

		r.iteration[r.loopDepth] = r.iteration[r.loopDepth] - 1;   // FIXME: --

		loopRepDepth++;
		breakDepth = 0;
	}

	void PixelRoutine::RET(Registers &r)
	{
		if(!returns)
		{
			returnBlock = Nucleus::createBasicBlock();
			Nucleus::createBr(returnBlock);

			returns = true;
		}
		else
		{
			// FIXME: Encapsulate
			UInt index = r.callStack[--r.stackIndex];
 
			llvm::BasicBlock *unreachableBlock = Nucleus::createBasicBlock();
			llvm::Value *value = Nucleus::createLoad(index.address);
			llvm::Value *switchInst = Nucleus::createSwitch(value, unreachableBlock, (int)callRetBlock.size());

			for(unsigned int i = 0; i < callRetBlock.size(); i++)
			{
				Nucleus::addSwitchCase(switchInst, i, callRetBlock[i]);
			}

			Nucleus::setInsertBlock(unreachableBlock);
			Nucleus::createUnreachable();
		}
	}

	void PixelRoutine::writeDestination(Registers &r, Color4i &d, const Dst &dst)
	{
		switch(dst.type)
		{
		case Dst::PARAMETER_TEMP:
			if(dst.mask & 0x1) r.ri[dst.index].x = d.x;
			if(dst.mask & 0x2) r.ri[dst.index].y = d.y;
			if(dst.mask & 0x4) r.ri[dst.index].z = d.z;
			if(dst.mask & 0x8) r.ri[dst.index].w = d.w;
			break;
		case Dst::PARAMETER_INPUT:
			if(dst.mask & 0x1) r.vi[dst.index].x = d.x;
			if(dst.mask & 0x2) r.vi[dst.index].y = d.y;
			if(dst.mask & 0x4) r.vi[dst.index].z = d.z;
			if(dst.mask & 0x8) r.vi[dst.index].w = d.w;
			break;
		case Dst::PARAMETER_CONST:			ASSERT(false);	break;
		case Dst::PARAMETER_TEXTURE:
			if(dst.mask & 0x1) r.ti[dst.index].x = d.x;
			if(dst.mask & 0x2) r.ti[dst.index].y = d.y;
			if(dst.mask & 0x4) r.ti[dst.index].z = d.z;
			if(dst.mask & 0x8) r.ti[dst.index].w = d.w;
			break;
		case Dst::PARAMETER_COLOROUT:
			if(dst.mask & 0x1) r.vi[dst.index].x = d.x;
			if(dst.mask & 0x2) r.vi[dst.index].y = d.y;
			if(dst.mask & 0x4) r.vi[dst.index].z = d.z;
			if(dst.mask & 0x8) r.vi[dst.index].w = d.w;
			break;
		default:
			ASSERT(false);
		}
	}

	Color4i PixelRoutine::regi(Registers &r, const Src &src)
	{
		Color4i *reg;
		int i = src.index;

		Color4i c;

		if(src.type == ShaderParameter::PARAMETER_CONST)
		{
			c.r = *Pointer<Short4>(r.data + OFFSET(DrawData,ps.cW[i][0]));
			c.g = *Pointer<Short4>(r.data + OFFSET(DrawData,ps.cW[i][1]));
			c.b = *Pointer<Short4>(r.data + OFFSET(DrawData,ps.cW[i][2]));
			c.a = *Pointer<Short4>(r.data + OFFSET(DrawData,ps.cW[i][3]));
		}

		switch(src.type)
		{
		case Src::PARAMETER_TEMP:			reg = &r.ri[i];	break;
		case Src::PARAMETER_INPUT:			reg = &r.vi[i];	break;
		case Src::PARAMETER_CONST:			reg = &c;		break;
		case Src::PARAMETER_TEXTURE:		reg = &r.ti[i];	break;
		case Src::PARAMETER_VOID:			return r.ri[0];   // Dummy
		case Src::PARAMETER_FLOATLITERAL:	return r.ri[0];   // Dummy
		default:
			ASSERT(false);
		}

		Short4 &x = (*reg)[(src.swizzle >> 0) & 0x3];
		Short4 &y = (*reg)[(src.swizzle >> 2) & 0x3];
		Short4 &z = (*reg)[(src.swizzle >> 4) & 0x3];
		Short4 &w = (*reg)[(src.swizzle >> 6) & 0x3];

		Color4i mod;

		switch(src.modifier)
		{
		case Src::MODIFIER_NONE:
			mod.r = x;
			mod.g = y;
			mod.b = z;
			mod.a = w;
			break;
		case Src::MODIFIER_BIAS:
			mod.r = SubSat(x, Short4(0x0800, 0x0800, 0x0800, 0x0800));
			mod.g = SubSat(y, Short4(0x0800, 0x0800, 0x0800, 0x0800));
			mod.b = SubSat(z, Short4(0x0800, 0x0800, 0x0800, 0x0800));
			mod.a = SubSat(w, Short4(0x0800, 0x0800, 0x0800, 0x0800));
			break;
		case Src::MODIFIER_BIAS_NEGATE:
			mod.r = SubSat(Short4(0x0800, 0x0800, 0x0800, 0x0800), x);
			mod.g = SubSat(Short4(0x0800, 0x0800, 0x0800, 0x0800), y);
			mod.b = SubSat(Short4(0x0800, 0x0800, 0x0800, 0x0800), z);
			mod.a = SubSat(Short4(0x0800, 0x0800, 0x0800, 0x0800), w);
			break;
		case Src::MODIFIER_COMPLEMENT:
			mod.r = SubSat(Short4(0x1000, 0x1000, 0x1000, 0x1000), x);
			mod.g = SubSat(Short4(0x1000, 0x1000, 0x1000, 0x1000), y);
			mod.b = SubSat(Short4(0x1000, 0x1000, 0x1000, 0x1000), z);
			mod.a = SubSat(Short4(0x1000, 0x1000, 0x1000, 0x1000), w);
			break;
		case Src::MODIFIER_NEGATE:
			mod.r = -x;
			mod.g = -y;
			mod.b = -z;
			mod.a = -w;
			break;
		case Src::MODIFIER_X2:
			mod.r = AddSat(x, x);
			mod.g = AddSat(y, y);
			mod.b = AddSat(z, z);
			mod.a = AddSat(w, w);
			break;
		case Src::MODIFIER_X2_NEGATE:
			mod.r = -AddSat(x, x);
			mod.g = -AddSat(y, y);
			mod.b = -AddSat(z, z);
			mod.a = -AddSat(w, w);
			break;
		case Src::MODIFIER_SIGN:
			mod.r = SubSat(x, Short4(0x0800, 0x0800, 0x0800, 0x0800));
			mod.g = SubSat(y, Short4(0x0800, 0x0800, 0x0800, 0x0800));
			mod.b = SubSat(z, Short4(0x0800, 0x0800, 0x0800, 0x0800));
			mod.a = SubSat(w, Short4(0x0800, 0x0800, 0x0800, 0x0800));
			mod.r = AddSat(mod.r, mod.r);
			mod.g = AddSat(mod.g, mod.g);
			mod.b = AddSat(mod.b, mod.b);
			mod.a = AddSat(mod.a, mod.a);
			break;
		case Src::MODIFIER_SIGN_NEGATE:
			mod.r = SubSat(Short4(0x0800, 0x0800, 0x0800, 0x0800), x);
			mod.g = SubSat(Short4(0x0800, 0x0800, 0x0800, 0x0800), y);
			mod.b = SubSat(Short4(0x0800, 0x0800, 0x0800, 0x0800), z);
			mod.a = SubSat(Short4(0x0800, 0x0800, 0x0800, 0x0800), w);
			mod.r = AddSat(mod.r, mod.r);
			mod.g = AddSat(mod.g, mod.g);
			mod.b = AddSat(mod.b, mod.b);
			mod.a = AddSat(mod.a, mod.a);
			break;
		case Src::MODIFIER_DZ:
			mod.r = x;
			mod.g = y;
			mod.b = z;
			mod.a = w;
			// Projection performed by texture sampler
			break;
		case Src::MODIFIER_DW:
			mod.r = x;
			mod.g = y;
			mod.b = z;
			mod.a = w;
			// Projection performed by texture sampler
			break;
		default:
			ASSERT(false);
		}

		if(src.type == ShaderParameter::PARAMETER_CONST && (src.modifier == Src::MODIFIER_X2 || src.modifier == Src::MODIFIER_X2_NEGATE))
		{
			mod.r = Min(mod.r, Short4(0x1000, 0x1000, 0x1000, 0x1000)); mod.r = Max(mod.r, Short4(-0x1000, -0x1000, -0x1000, -0x1000));
			mod.g = Min(mod.g, Short4(0x1000, 0x1000, 0x1000, 0x1000)); mod.g = Max(mod.g, Short4(-0x1000, -0x1000, -0x1000, -0x1000));
			mod.b = Min(mod.b, Short4(0x1000, 0x1000, 0x1000, 0x1000)); mod.b = Max(mod.b, Short4(-0x1000, -0x1000, -0x1000, -0x1000));
			mod.a = Min(mod.a, Short4(0x1000, 0x1000, 0x1000, 0x1000)); mod.a = Max(mod.a, Short4(-0x1000, -0x1000, -0x1000, -0x1000));
		}

		return mod;
	}

	Color4f PixelRoutine::reg(Registers &r, const Src &src, int offset)
	{
		Color4f reg;
		int i = src.index + offset;

		switch(src.type)
		{
		case Src::PARAMETER_TEMP:			reg = r.rf[i];		break;
		case Src::PARAMETER_INPUT:
			{
				if(!src.relative)
				{
					reg.x = r.vx[i];
					reg.y = r.vy[i];
					reg.z = r.vz[i];
					reg.w = r.vw[i];
				}
				else if(src.relativeType == Src::PARAMETER_LOOP)
				{
					Int aL = r.aL[r.loopDepth];

					reg.x = r.vx[i + aL];
					reg.y = r.vy[i + aL];
					reg.z = r.vz[i + aL];
					reg.w = r.vw[i + aL];
				}
				else ASSERT(false);
			}
			break;
		case Src::PARAMETER_CONST:
			{
				reg.r = reg.g = reg.b = reg.a = *Pointer<Float4>(r.data + OFFSET(DrawData,ps.c[i]));

				reg.r = reg.r.xxxx;
				reg.g = reg.g.yyyy;
				reg.b = reg.b.zzzz;
				reg.a = reg.a.wwww;

				if(localShaderConstants)   // Constant may be known at compile time
				{
					for(int j = 0; j < pixelShader->getLength(); j++)
					{
						const ShaderInstruction &instruction = *pixelShader->getInstruction(j);

						if(instruction.getOpcode() == ShaderOperation::OPCODE_DEF)
						{
							if(instruction.getDestinationParameter().index == i)
							{
								reg.r = Float4(instruction.getSourceParameter(0).value);
								reg.g = Float4(instruction.getSourceParameter(1).value);
								reg.b = Float4(instruction.getSourceParameter(2).value);
								reg.a = Float4(instruction.getSourceParameter(3).value);

								break;
							}
						}
					}
				}
			}
			break;
		case Src::PARAMETER_TEXTURE:
			{
				reg.x = r.vx[2 + i];
				reg.y = r.vy[2 + i];
				reg.z = r.vz[2 + i];
				reg.w = r.vw[2 + i];
			}
			break;
		case Src::PARAMETER_MISCTYPE:
			if(src.index == 0)				reg = r.vPos;
			if(src.index == 1)				reg = r.vFace;
			break;
		case Src::PARAMETER_SAMPLER:		return r.rf[0];   // Dummy
		case Src::PARAMETER_PREDICATE:		return r.rf[0];   // Dummy
		case Src::PARAMETER_VOID:			return r.rf[0];   // Dummy
		case Src::PARAMETER_FLOATLITERAL:	return r.rf[0];   // Dummy
		case Src::PARAMETER_CONSTINT:		return r.rf[0];   // Dummy
		case Src::PARAMETER_CONSTBOOL:		return r.rf[0];   // Dummy
		case Src::PARAMETER_LOOP:			return r.rf[0];   // Dummy
		default:
			ASSERT(false);
		}

		Float4 &x = reg[(src.swizzle >> 0) & 0x3];
		Float4 &y = reg[(src.swizzle >> 2) & 0x3];
		Float4 &z = reg[(src.swizzle >> 4) & 0x3];
		Float4 &w = reg[(src.swizzle >> 6) & 0x3];

		Color4f mod;

		switch(src.modifier)
		{
		case Src::MODIFIER_NONE:
			mod.x = x;
			mod.y = y;
			mod.z = z;
			mod.w = w;
			break;
		case Src::MODIFIER_NEGATE:
			mod.x = -x;
			mod.y = -y;
			mod.z = -z;
			mod.w = -w;
			break;
		case Src::MODIFIER_ABS:
			mod.x = Abs(x);
			mod.y = Abs(y);
			mod.z = Abs(z);
			mod.w = Abs(w);
			break;
		case Src::MODIFIER_ABS_NEGATE:
			mod.x = -Abs(x);
			mod.y = -Abs(y);
			mod.z = -Abs(z);
			mod.w = -Abs(w);
			break;
		default:
			ASSERT(false);
		}

		return mod;
	}

	bool PixelRoutine::colorUsed()
	{
		return state.colorWriteMask || state.alphaTestActive() || state.shaderContainsTexkill;
	}

	unsigned short PixelRoutine::pixelShaderVersion() const
	{
		return pixelShader ? pixelShader->getVersion() : 0x0000;
	}
}
