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

#include "PixelRoutine.hpp"

#include "Renderer.hpp"
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
	extern bool booleanFaceRegister;
	extern bool halfIntegerCoordinates;     // Pixel centers are not at integer coordinates
	extern bool fullPixelPositionRegister;

	PixelRoutine::PixelRoutine(const PixelProcessor::State &state, const PixelShader *shader) : Rasterizer(state), shader(shader)
	{
		perturbate = false;
		luminance = false;
		previousScaling = false;

		ifDepth = 0;
		loopRepDepth = 0;
		breakDepth = 0;
		currentLabel = -1;
		whileTest = false;

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
		const bool integerPipeline = shaderVersion() <= 0x0104;

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

		Float4 (&z)[4] = r.z;
		Float4 &w = r.w;
		Float4 &rhw = r.rhw;
		Float4 rhwCentroid;

		Float4 xxxx = Float4(Float(x)) + *Pointer<Float4>(r.primitive + OFFSET(Primitive,xQuad), 16);

		if(interpolateZ())
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

			Float4 yyyy = Float4(Float(y)) + *Pointer<Float4>(r.primitive + OFFSET(Primitive, yQuad), 16);

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

			if(interpolateW())
			{
				w = interpolate(xxxx, r.Dw, rhw, r.primitive + OFFSET(Primitive,w), false, false);
				rhw = reciprocal(w);

				if(state.centroid)
				{
					rhwCentroid = reciprocal(interpolateCentroid(XXXX, YYYY, rhwCentroid, r.primitive + OFFSET(Primitive,w), false, false));
				}
			}

			for(int interpolant = 0; interpolant < 10; interpolant++)
			{
				for(int component = 0; component < 4; component++)
				{
					if(state.interpolant[interpolant].component & (1 << component))
					{
						if(!state.interpolant[interpolant].centroid)
						{
							r.vf[interpolant][component] = interpolate(xxxx, r.Dv[interpolant][component], rhw, r.primitive + OFFSET(Primitive,V[interpolant][component]), (state.interpolant[interpolant].flat & (1 << component)) != 0, state.perspective);
						}
						else
						{
							r.vf[interpolant][component] = interpolateCentroid(XXXX, YYYY, rhwCentroid, r.primitive + OFFSET(Primitive,V[interpolant][component]), (state.interpolant[interpolant].flat & (1 << component)) != 0, state.perspective);
						}
					}
				}

				Float4 rcp;

				switch(state.interpolant[interpolant].project)
				{
				case 0:
					break;
				case 1:
					rcp = reciprocal(r.vf[interpolant].y);
					r.vf[interpolant].x = r.vf[interpolant].x * rcp;
					break;
				case 2:
					rcp = reciprocal(r.vf[interpolant].z);
					r.vf[interpolant].x = r.vf[interpolant].x * rcp;
					r.vf[interpolant].y = r.vf[interpolant].y * rcp;
					break;
				case 3:
					rcp = reciprocal(r.vf[interpolant].w);
					r.vf[interpolant].x = r.vf[interpolant].x * rcp;
					r.vf[interpolant].y = r.vf[interpolant].y * rcp;
					r.vf[interpolant].z = r.vf[interpolant].z * rcp;
					break;
				}
			}

			if(state.fog.component)
			{
				f = interpolate(xxxx, r.Df, rhw, r.primitive + OFFSET(Primitive,f), state.fog.flat & 0x01, state.perspective);
			}

			if(integerPipeline)
			{
				if(state.color[0].component & 0x1) r.diffuse.x = convertFixed12(r.vf[0].x); else r.diffuse.x = Short4(0x1000);
				if(state.color[0].component & 0x2) r.diffuse.y = convertFixed12(r.vf[0].y); else r.diffuse.y = Short4(0x1000);
				if(state.color[0].component & 0x4) r.diffuse.z = convertFixed12(r.vf[0].z); else r.diffuse.z = Short4(0x1000);
				if(state.color[0].component & 0x8) r.diffuse.w = convertFixed12(r.vf[0].w); else r.diffuse.w = Short4(0x1000);

				if(state.color[1].component & 0x1) r.specular.x = convertFixed12(r.vf[1].x); else r.specular.x = Short4(0x0000, 0x0000, 0x0000, 0x0000);
				if(state.color[1].component & 0x2) r.specular.y = convertFixed12(r.vf[1].y); else r.specular.y = Short4(0x0000, 0x0000, 0x0000, 0x0000);
				if(state.color[1].component & 0x4) r.specular.z = convertFixed12(r.vf[1].z); else r.specular.z = Short4(0x0000, 0x0000, 0x0000, 0x0000);
				if(state.color[1].component & 0x8) r.specular.w = convertFixed12(r.vf[1].w); else r.specular.w = Short4(0x0000, 0x0000, 0x0000, 0x0000);
			}
			else if(shaderVersion() >= 0x0300)
			{
				if(shader->vPosDeclared)
				{
					if(!halfIntegerCoordinates)
					{
						r.vPos.x = Float4(Float(x)) + Float4(0, 1, 0, 1);
						r.vPos.y = Float4(Float(y)) + Float4(0, 0, 1, 1);
					}
					else
					{
						r.vPos.x = Float4(Float(x)) + Float4(0.5f, 1.5f, 0.5f, 1.5f);
						r.vPos.y = Float4(Float(y)) + Float4(0.5f, 0.5f, 1.5f, 1.5f);
					}

					if(fullPixelPositionRegister)
					{
						r.vPos.z = z[0];   // FIXME: Centroid?
						r.vPos.w = w;      // FIXME: Centroid?
					}
				}

				if(shader->vFaceDeclared)
				{
					Float4 area = *Pointer<Float>(r.primitive + OFFSET(Primitive,area));
					Float4 face = booleanFaceRegister ? Float4(As<Float4>(CmpNLT(area, Float4(0.0f)))) : area;

					r.vFace.x = face;
					r.vFace.y = face;
					r.vFace.z = face;
					r.vFace.w = face;
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

				if(shader)
				{
				//	shader->print("PixelShader-%0.8X.txt", state.shaderID);

					if(shader->getVersion() <= 0x0104)
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
					r.current = r.diffuse;
					Vector4i temp(0x0000, 0x0000, 0x0000, 0x0000);

					for(int stage = 0; stage < 8; stage++)
					{
						if(state.textureStage[stage].stageOperation == TextureStage::STAGE_DISABLE)
						{
							break;
						}

						Vector4i texture;

						if(state.textureStage[stage].usesTexture)
						{
							sampleTexture(r, texture, stage, stage);
						}

						blendTexture(r, temp, texture, stage);
					}

					specularPixel(r.current, r.specular);
				}

				#if PERF_PROFILE
					r.cycles[PERF_SHADER] += Ticks() - shaderTime;
				#endif

				if(integerPipeline)
				{
					r.current.x = Min(r.current.x, Short4(0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF)); r.current.x = Max(r.current.x, Short4(0x0000, 0x0000, 0x0000, 0x0000));
					r.current.y = Min(r.current.y, Short4(0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF)); r.current.y = Max(r.current.y, Short4(0x0000, 0x0000, 0x0000, 0x0000));
					r.current.z = Min(r.current.z, Short4(0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF)); r.current.z = Max(r.current.z, Short4(0x0000, 0x0000, 0x0000, 0x0000));
					r.current.w = Min(r.current.w, Short4(0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF)); r.current.w = Max(r.current.w, Short4(0x0000, 0x0000, 0x0000, 0x0000));

					alphaPass = alphaTest(r, cMask, r.current);
				}
				else
				{
					clampColor(r.oC);

					alphaPass = alphaTest(r, cMask, r.oC[0]);
				}

				if((shader && shader->containsKill()) || state.alphaTestActive())
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
							AddAtomic(Pointer<Long>(&profiler.ropOperations), 4);
						#endif

						if(integerPipeline)
						{
							rasterOperation(r.current, r, f, cBuffer[0], x, sMask, zMask, cMask);
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

		if(shader && shader->depthOverride())
		{
			if(complementaryDepthBuffer)
			{
				Z = Float4(1.0f) - r.oDepth;
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

	void PixelRoutine::blendTexture(Registers &r, Vector4i &temp, Vector4i &texture, int stage)
	{
		Vector4i *arg1;
		Vector4i *arg2;
		Vector4i *arg3;
		Vector4i res;

		Vector4i constant;
		Vector4i tfactor;

		const TextureStage::State &textureStage = state.textureStage[stage];

		if(textureStage.firstArgument == TextureStage::SOURCE_CONSTANT ||
		   textureStage.firstArgumentAlpha == TextureStage::SOURCE_CONSTANT ||
		   textureStage.secondArgument == TextureStage::SOURCE_CONSTANT ||
		   textureStage.secondArgumentAlpha == TextureStage::SOURCE_CONSTANT ||
		   textureStage.thirdArgument == TextureStage::SOURCE_CONSTANT ||
		   textureStage.thirdArgumentAlpha == TextureStage::SOURCE_CONSTANT)
		{
			constant.x = *Pointer<Short4>(r.data + OFFSET(DrawData,textureStage[stage].constantColor4[0]));
			constant.y = *Pointer<Short4>(r.data + OFFSET(DrawData,textureStage[stage].constantColor4[1]));
			constant.z = *Pointer<Short4>(r.data + OFFSET(DrawData,textureStage[stage].constantColor4[2]));
			constant.w = *Pointer<Short4>(r.data + OFFSET(DrawData,textureStage[stage].constantColor4[3]));
		}

		if(textureStage.firstArgument == TextureStage::SOURCE_TFACTOR ||
		   textureStage.firstArgumentAlpha == TextureStage::SOURCE_TFACTOR ||
		   textureStage.secondArgument == TextureStage::SOURCE_TFACTOR ||
		   textureStage.secondArgumentAlpha == TextureStage::SOURCE_TFACTOR ||
		   textureStage.thirdArgument == TextureStage::SOURCE_TFACTOR ||
		   textureStage.thirdArgumentAlpha == TextureStage::SOURCE_TFACTOR)
		{
			tfactor.x = *Pointer<Short4>(r.data + OFFSET(DrawData,factor.textureFactor4[0]));
			tfactor.y = *Pointer<Short4>(r.data + OFFSET(DrawData,factor.textureFactor4[1]));
			tfactor.z = *Pointer<Short4>(r.data + OFFSET(DrawData,factor.textureFactor4[2]));
			tfactor.w = *Pointer<Short4>(r.data + OFFSET(DrawData,factor.textureFactor4[3]));
		}

		// Premodulate
		if(stage > 0 && textureStage.usesTexture)
		{
			if(state.textureStage[stage - 1].stageOperation == TextureStage::STAGE_PREMODULATE)
			{
				r.current.x = MulHigh(r.current.x, texture.x) << 4;
				r.current.y = MulHigh(r.current.y, texture.y) << 4;
				r.current.z = MulHigh(r.current.z, texture.z) << 4;
			}

			if(state.textureStage[stage - 1].stageOperationAlpha == TextureStage::STAGE_PREMODULATE)
			{
				r.current.w = MulHigh(r.current.w, texture.w) << 4;
			}
		}

		if(luminance)
		{
			texture.x = MulHigh(texture.x, r.L) << 4;
			texture.y = MulHigh(texture.y, r.L) << 4;
			texture.z = MulHigh(texture.z, r.L) << 4;

			luminance = false;
		}

		switch(textureStage.firstArgument)
		{
		case TextureStage::SOURCE_TEXTURE:	arg1 = &texture;		break;
		case TextureStage::SOURCE_CONSTANT:	arg1 = &constant;		break;
		case TextureStage::SOURCE_CURRENT:	arg1 = &r.current;		break;
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
		case TextureStage::SOURCE_CURRENT:	arg2 = &r.current;		break;
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
		case TextureStage::SOURCE_CURRENT:	arg3 = &r.current;		break;
		case TextureStage::SOURCE_DIFFUSE:	arg3 = &r.diffuse;		break;
		case TextureStage::SOURCE_SPECULAR:	arg3 = &r.specular;		break;
		case TextureStage::SOURCE_TEMP:		arg3 = &temp;			break;
		case TextureStage::SOURCE_TFACTOR:	arg3 = &tfactor;		break;
		default:
			ASSERT(false);
		}

		Vector4i mod1;
		Vector4i mod2;
		Vector4i mod3;

		switch(textureStage.firstModifier)
		{
		case TextureStage::MODIFIER_COLOR:
			break;
		case TextureStage::MODIFIER_INVCOLOR:
			{
				mod1.x = SubSat(Short4(0x1000), arg1->x);
				mod1.y = SubSat(Short4(0x1000), arg1->y);
				mod1.z = SubSat(Short4(0x1000), arg1->z);
				mod1.w = SubSat(Short4(0x1000), arg1->w);

				arg1 = &mod1;
			}
			break;
		case TextureStage::MODIFIER_ALPHA:
			{
				mod1.x = arg1->w;
				mod1.y = arg1->w;
				mod1.z = arg1->w;
				mod1.w = arg1->w;

				arg1 = &mod1;
			}
			break;
		case TextureStage::MODIFIER_INVALPHA:
			{
				mod1.x = SubSat(Short4(0x1000), arg1->w);
				mod1.y = SubSat(Short4(0x1000), arg1->w);
				mod1.z = SubSat(Short4(0x1000), arg1->w);
				mod1.w = SubSat(Short4(0x1000), arg1->w);

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
				mod2.x = SubSat(Short4(0x1000), arg2->x);
				mod2.y = SubSat(Short4(0x1000), arg2->y);
				mod2.z = SubSat(Short4(0x1000), arg2->z);
				mod2.w = SubSat(Short4(0x1000), arg2->w);

				arg2 = &mod2;
			}
			break;
		case TextureStage::MODIFIER_ALPHA:
			{
				mod2.x = arg2->w;
				mod2.y = arg2->w;
				mod2.z = arg2->w;
				mod2.w = arg2->w;

				arg2 = &mod2;
			}
			break;
		case TextureStage::MODIFIER_INVALPHA:
			{
				mod2.x = SubSat(Short4(0x1000), arg2->w);
				mod2.y = SubSat(Short4(0x1000), arg2->w);
				mod2.z = SubSat(Short4(0x1000), arg2->w);
				mod2.w = SubSat(Short4(0x1000), arg2->w);

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
				mod3.x = SubSat(Short4(0x1000), arg3->x);
				mod3.y = SubSat(Short4(0x1000), arg3->y);
				mod3.z = SubSat(Short4(0x1000), arg3->z);
				mod3.w = SubSat(Short4(0x1000), arg3->w);

				arg3 = &mod3;
			}
			break;
		case TextureStage::MODIFIER_ALPHA:
			{
				mod3.x = arg3->w;
				mod3.y = arg3->w;
				mod3.z = arg3->w;
				mod3.w = arg3->w;

				arg3 = &mod3;
			}
			break;
		case TextureStage::MODIFIER_INVALPHA:
			{
				mod3.x = SubSat(Short4(0x1000), arg3->w);
				mod3.y = SubSat(Short4(0x1000), arg3->w);
				mod3.z = SubSat(Short4(0x1000), arg3->w);
				mod3.w = SubSat(Short4(0x1000), arg3->w);

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
				res.x = arg1->x;
				res.y = arg1->y;
				res.z = arg1->z;
			}
			break;
		case TextureStage::STAGE_SELECTARG2:					// Arg2
			{
				res.x = arg2->x;
				res.y = arg2->y;
				res.z = arg2->z;
			}
			break;
		case TextureStage::STAGE_SELECTARG3:					// Arg3
			{
				res.x = arg3->x;
				res.y = arg3->y;
				res.z = arg3->z;
			}
			break;
		case TextureStage::STAGE_MODULATE:					// Arg1 * Arg2
			{
				res.x = MulHigh(arg1->x, arg2->x) << 4;
				res.y = MulHigh(arg1->y, arg2->y) << 4;
				res.z = MulHigh(arg1->z, arg2->z) << 4;
			}
			break;
		case TextureStage::STAGE_MODULATE2X:					// Arg1 * Arg2 * 2
			{
				res.x = MulHigh(arg1->x, arg2->x) << 5;
				res.y = MulHigh(arg1->y, arg2->y) << 5;
				res.z = MulHigh(arg1->z, arg2->z) << 5;
			}
			break;
		case TextureStage::STAGE_MODULATE4X:					// Arg1 * Arg2 * 4
			{
				res.x = MulHigh(arg1->x, arg2->x) << 6;
				res.y = MulHigh(arg1->y, arg2->y) << 6;
				res.z = MulHigh(arg1->z, arg2->z) << 6;
			}
			break;
		case TextureStage::STAGE_ADD:						// Arg1 + Arg2
			{
				res.x = AddSat(arg1->x, arg2->x);
				res.y = AddSat(arg1->y, arg2->y);
				res.z = AddSat(arg1->z, arg2->z);
			}
			break;
		case TextureStage::STAGE_ADDSIGNED:					// Arg1 + Arg2 - 0.5
			{
				res.x = AddSat(arg1->x, arg2->x);
				res.y = AddSat(arg1->y, arg2->y);
				res.z = AddSat(arg1->z, arg2->z);

				res.x = SubSat(res.x, Short4(0x0800, 0x0800, 0x0800, 0x0800));
				res.y = SubSat(res.y, Short4(0x0800, 0x0800, 0x0800, 0x0800));
				res.z = SubSat(res.z, Short4(0x0800, 0x0800, 0x0800, 0x0800));
			}
			break;
		case TextureStage::STAGE_ADDSIGNED2X:				// (Arg1 + Arg2 - 0.5) << 1
			{
				res.x = AddSat(arg1->x, arg2->x);
				res.y = AddSat(arg1->y, arg2->y);
				res.z = AddSat(arg1->z, arg2->z);

				res.x = SubSat(res.x, Short4(0x0800, 0x0800, 0x0800, 0x0800));
				res.y = SubSat(res.y, Short4(0x0800, 0x0800, 0x0800, 0x0800));
				res.z = SubSat(res.z, Short4(0x0800, 0x0800, 0x0800, 0x0800));

				res.x = AddSat(res.x, res.x);
				res.y = AddSat(res.y, res.y);
				res.z = AddSat(res.z, res.z);
			}
			break;
		case TextureStage::STAGE_SUBTRACT:					// Arg1 - Arg2
			{
				res.x = SubSat(arg1->x, arg2->x);
				res.y = SubSat(arg1->y, arg2->y);
				res.z = SubSat(arg1->z, arg2->z);
			}
			break;
		case TextureStage::STAGE_ADDSMOOTH:					// Arg1 + Arg2 - Arg1 * Arg2
			{
				Short4 tmp;

				tmp = MulHigh(arg1->x, arg2->x) << 4; res.x = AddSat(arg1->x, arg2->x); res.x = SubSat(res.x, tmp);
				tmp = MulHigh(arg1->y, arg2->y) << 4; res.y = AddSat(arg1->y, arg2->y); res.y = SubSat(res.y, tmp);
				tmp = MulHigh(arg1->z, arg2->z) << 4; res.z = AddSat(arg1->z, arg2->z); res.z = SubSat(res.z, tmp);
			}
			break;
		case TextureStage::STAGE_MULTIPLYADD:				// Arg3 + Arg1 * Arg2
			{
				res.x = MulHigh(arg1->x, arg2->x) << 4; res.x = AddSat(res.x, arg3->x);
				res.y = MulHigh(arg1->y, arg2->y) << 4; res.y = AddSat(res.y, arg3->y);
				res.z = MulHigh(arg1->z, arg2->z) << 4; res.z = AddSat(res.z, arg3->z);
			}
			break;
		case TextureStage::STAGE_LERP:						// Arg3 * (Arg1 - Arg2) + Arg2
			{
				res.x = SubSat(arg1->x, arg2->x); res.x = MulHigh(res.x, arg3->x) << 4; res.x = AddSat(res.x, arg2->x);
				res.y = SubSat(arg1->y, arg2->y); res.y = MulHigh(res.y, arg3->y) << 4; res.y = AddSat(res.y, arg2->y);
				res.z = SubSat(arg1->z, arg2->z); res.z = MulHigh(res.z, arg3->z) << 4; res.z = AddSat(res.z, arg2->z);
			}
			break;
		case TextureStage::STAGE_DOT3:						// 2 * (Arg1.x - 0.5) * 2 * (Arg2.x - 0.5) + 2 * (Arg1.y - 0.5) * 2 * (Arg2.y - 0.5) + 2 * (Arg1.z - 0.5) * 2 * (Arg2.z - 0.5)
			{
				Short4 tmp;

				res.x = SubSat(arg1->x, Short4(0x0800, 0x0800, 0x0800, 0x0800)); tmp = SubSat(arg2->x, Short4(0x0800, 0x0800, 0x0800, 0x0800)); res.x = MulHigh(res.x, tmp);
				res.y = SubSat(arg1->y, Short4(0x0800, 0x0800, 0x0800, 0x0800)); tmp = SubSat(arg2->y, Short4(0x0800, 0x0800, 0x0800, 0x0800)); res.y = MulHigh(res.y, tmp);
				res.z = SubSat(arg1->z, Short4(0x0800, 0x0800, 0x0800, 0x0800)); tmp = SubSat(arg2->z, Short4(0x0800, 0x0800, 0x0800, 0x0800)); res.z = MulHigh(res.z, tmp);

				res.x = res.x << 6;
				res.y = res.y << 6;
				res.z = res.z << 6;

				res.x = AddSat(res.x, res.y);
				res.x = AddSat(res.x, res.z);

				// Clamp to [0, 1]
				res.x = Max(res.x, Short4(0x0000, 0x0000, 0x0000, 0x0000));
				res.x = Min(res.x, Short4(0x1000));

				res.y = res.x;
				res.z = res.x;
				res.w = res.x;
			}
			break;
		case TextureStage::STAGE_BLENDCURRENTALPHA:			// Alpha * (Arg1 - Arg2) + Arg2
			{
				res.x = SubSat(arg1->x, arg2->x); res.x = MulHigh(res.x, r.current.w) << 4; res.x = AddSat(res.x, arg2->x);
				res.y = SubSat(arg1->y, arg2->y); res.y = MulHigh(res.y, r.current.w) << 4; res.y = AddSat(res.y, arg2->y);
				res.z = SubSat(arg1->z, arg2->z); res.z = MulHigh(res.z, r.current.w) << 4; res.z = AddSat(res.z, arg2->z);
			}
			break;
		case TextureStage::STAGE_BLENDDIFFUSEALPHA:			// Alpha * (Arg1 - Arg2) + Arg2
			{
				res.x = SubSat(arg1->x, arg2->x); res.x = MulHigh(res.x, r.diffuse.w) << 4; res.x = AddSat(res.x, arg2->x);
				res.y = SubSat(arg1->y, arg2->y); res.y = MulHigh(res.y, r.diffuse.w) << 4; res.y = AddSat(res.y, arg2->y);
				res.z = SubSat(arg1->z, arg2->z); res.z = MulHigh(res.z, r.diffuse.w) << 4; res.z = AddSat(res.z, arg2->z);
			}
			break;
		case TextureStage::STAGE_BLENDFACTORALPHA:			// Alpha * (Arg1 - Arg2) + Arg2
			{
				res.x = SubSat(arg1->x, arg2->x); res.x = MulHigh(res.x, *Pointer<Short4>(r.data + OFFSET(DrawData,factor.textureFactor4[3]))) << 4; res.x = AddSat(res.x, arg2->x);
				res.y = SubSat(arg1->y, arg2->y); res.y = MulHigh(res.y, *Pointer<Short4>(r.data + OFFSET(DrawData,factor.textureFactor4[3]))) << 4; res.y = AddSat(res.y, arg2->y);
				res.z = SubSat(arg1->z, arg2->z); res.z = MulHigh(res.z, *Pointer<Short4>(r.data + OFFSET(DrawData,factor.textureFactor4[3]))) << 4; res.z = AddSat(res.z, arg2->z);
			}
			break;
		case TextureStage::STAGE_BLENDTEXTUREALPHA:			// Alpha * (Arg1 - Arg2) + Arg2
			{
				res.x = SubSat(arg1->x, arg2->x); res.x = MulHigh(res.x, texture.w) << 4; res.x = AddSat(res.x, arg2->x);
				res.y = SubSat(arg1->y, arg2->y); res.y = MulHigh(res.y, texture.w) << 4; res.y = AddSat(res.y, arg2->y);
				res.z = SubSat(arg1->z, arg2->z); res.z = MulHigh(res.z, texture.w) << 4; res.z = AddSat(res.z, arg2->z);
			}
			break;
		case TextureStage::STAGE_BLENDTEXTUREALPHAPM:		// Arg1 + Arg2 * (1 - Alpha)
			{
				res.x = SubSat(Short4(0x1000), texture.w); res.x = MulHigh(res.x, arg2->x) << 4; res.x = AddSat(res.x, arg1->x);
				res.y = SubSat(Short4(0x1000), texture.w); res.y = MulHigh(res.y, arg2->y) << 4; res.y = AddSat(res.y, arg1->y);
				res.z = SubSat(Short4(0x1000), texture.w); res.z = MulHigh(res.z, arg2->z) << 4; res.z = AddSat(res.z, arg1->z);
			}
			break;
		case TextureStage::STAGE_PREMODULATE:
			{
				res.x = arg1->x;
				res.y = arg1->y;
				res.z = arg1->z;
			}
			break;
		case TextureStage::STAGE_MODULATEALPHA_ADDCOLOR:		// Arg1 + Arg1.w * Arg2
			{
				res.x = MulHigh(arg1->w, arg2->x) << 4; res.x = AddSat(res.x, arg1->x);
				res.y = MulHigh(arg1->w, arg2->y) << 4; res.y = AddSat(res.y, arg1->y);
				res.z = MulHigh(arg1->w, arg2->z) << 4; res.z = AddSat(res.z, arg1->z);
			}
			break;
		case TextureStage::STAGE_MODULATECOLOR_ADDALPHA:		// Arg1 * Arg2 + Arg1.w
			{
				res.x = MulHigh(arg1->x, arg2->x) << 4; res.x = AddSat(res.x, arg1->w);
				res.y = MulHigh(arg1->y, arg2->y) << 4; res.y = AddSat(res.y, arg1->w);
				res.z = MulHigh(arg1->z, arg2->z) << 4; res.z = AddSat(res.z, arg1->w);
			}
			break;
		case TextureStage::STAGE_MODULATEINVALPHA_ADDCOLOR:	// (1 - Arg1.w) * Arg2 + Arg1
			{
				Short4 tmp;

				res.x = AddSat(arg1->x, arg2->x); tmp = MulHigh(arg1->w, arg2->x) << 4; res.x = SubSat(res.x, tmp);
				res.y = AddSat(arg1->y, arg2->y); tmp = MulHigh(arg1->w, arg2->y) << 4; res.y = SubSat(res.y, tmp);
				res.z = AddSat(arg1->z, arg2->z); tmp = MulHigh(arg1->w, arg2->z) << 4; res.z = SubSat(res.z, tmp);
			}
			break;
		case TextureStage::STAGE_MODULATEINVCOLOR_ADDALPHA:	// (1 - Arg1) * Arg2 + Arg1.w
			{
				Short4 tmp;

				res.x = AddSat(arg1->w, arg2->x); tmp = MulHigh(arg1->x, arg2->x) << 4; res.x = SubSat(res.x, tmp);
				res.y = AddSat(arg1->w, arg2->y); tmp = MulHigh(arg1->y, arg2->y) << 4; res.y = SubSat(res.y, tmp);
				res.z = AddSat(arg1->w, arg2->z); tmp = MulHigh(arg1->z, arg2->z) << 4; res.z = SubSat(res.z, tmp);
			}
			break;
		case TextureStage::STAGE_BUMPENVMAP:
			{
				r.du = Float4(texture.x) * Float4(1.0f / 0x0FE0);
				r.dv = Float4(texture.y) * Float4(1.0f / 0x0FE0);
			
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

				res.x = r.current.x;
				res.y = r.current.y;
				res.z = r.current.z;
				res.w = r.current.w;
			}
			break;
		case TextureStage::STAGE_BUMPENVMAPLUMINANCE:
			{
				r.du = Float4(texture.x) * Float4(1.0f / 0x0FE0);
				r.dv = Float4(texture.y) * Float4(1.0f / 0x0FE0);
			
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

				r.L = texture.z;
				r.L = MulHigh(r.L, *Pointer<Short4>(r.data + OFFSET(DrawData,textureStage[stage].luminanceScale4)));
				r.L = r.L << 4;
				r.L = AddSat(r.L, *Pointer<Short4>(r.data + OFFSET(DrawData,textureStage[stage].luminanceOffset4)));
				r.L = Max(r.L, Short4(0x0000, 0x0000, 0x0000, 0x0000));
				r.L = Min(r.L, Short4(0x1000));

				luminance = true;

				res.x = r.current.x;
				res.y = r.current.y;
				res.z = r.current.z;
				res.w = r.current.w;
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
			case TextureStage::SOURCE_CURRENT:	arg1 = &r.current;		break;
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
			case TextureStage::SOURCE_CURRENT:	arg2 = &r.current;		break;
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
			case TextureStage::SOURCE_CURRENT:	arg3 = &r.current;		break;
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
					mod1.w = SubSat(Short4(0x1000), arg1->w);

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
					mod1.w = SubSat(Short4(0x1000), arg1->w);

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
					mod2.w = SubSat(Short4(0x1000), arg2->w);

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
					mod2.w = SubSat(Short4(0x1000), arg2->w);

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
					mod3.w = SubSat(Short4(0x1000), arg3->w);

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
					mod3.w = SubSat(Short4(0x1000), arg3->w);

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
					res.w = arg1->w;
				}
				break;
			case TextureStage::STAGE_SELECTARG2:					// Arg2
				{
					res.w = arg2->w;
				}
				break;
			case TextureStage::STAGE_SELECTARG3:					// Arg3
				{
					res.w = arg3->w;
				}
				break;
			case TextureStage::STAGE_MODULATE:					// Arg1 * Arg2
				{
					res.w = MulHigh(arg1->w, arg2->w) << 4;
				}
				break;
			case TextureStage::STAGE_MODULATE2X:					// Arg1 * Arg2 * 2
				{
					res.w = MulHigh(arg1->w, arg2->w) << 5;
				}
				break;
			case TextureStage::STAGE_MODULATE4X:					// Arg1 * Arg2 * 4
				{
					res.w = MulHigh(arg1->w, arg2->w) << 6;
				}
				break;
			case TextureStage::STAGE_ADD:						// Arg1 + Arg2
				{
					res.w = AddSat(arg1->w, arg2->w);
				}
				break;
			case TextureStage::STAGE_ADDSIGNED:					// Arg1 + Arg2 - 0.5
				{
					res.w = AddSat(arg1->w, arg2->w);
					res.w = SubSat(res.w, Short4(0x0800, 0x0800, 0x0800, 0x0800));
				}
				break;
			case TextureStage::STAGE_ADDSIGNED2X:					// (Arg1 + Arg2 - 0.5) << 1
				{
					res.w = AddSat(arg1->w, arg2->w);
					res.w = SubSat(res.w, Short4(0x0800, 0x0800, 0x0800, 0x0800));
					res.w = AddSat(res.w, res.w);
				}
				break;
			case TextureStage::STAGE_SUBTRACT:					// Arg1 - Arg2
				{
					res.w = SubSat(arg1->w, arg2->w);
				}
				break;
			case TextureStage::STAGE_ADDSMOOTH:					// Arg1 + Arg2 - Arg1 * Arg2
				{
					Short4 tmp;

					tmp = MulHigh(arg1->w, arg2->w) << 4; res.w = AddSat(arg1->w, arg2->w); res.w = SubSat(res.w, tmp);
				}
				break;
			case TextureStage::STAGE_MULTIPLYADD:				// Arg3 + Arg1 * Arg2
				{
					res.w = MulHigh(arg1->w, arg2->w) << 4; res.w = AddSat(res.w, arg3->w);
				}
				break;
			case TextureStage::STAGE_LERP:						// Arg3 * (Arg1 - Arg2) + Arg2
				{
					res.w = SubSat(arg1->w, arg2->w); res.w = MulHigh(res.w, arg3->w) << 4; res.w = AddSat(res.w, arg2->w);
				}
				break;
			case TextureStage::STAGE_DOT3:
				break;   // Already computed in color channel
			case TextureStage::STAGE_BLENDCURRENTALPHA:			// Alpha * (Arg1 - Arg2) + Arg2
				{
					res.w = SubSat(arg1->w, arg2->w); res.w = MulHigh(res.w, r.current.w) << 4; res.w = AddSat(res.w, arg2->w);
				}
				break;
			case TextureStage::STAGE_BLENDDIFFUSEALPHA:			// Arg1 * (Alpha) + Arg2 * (1 - Alpha)
				{
					res.w = SubSat(arg1->w, arg2->w); res.w = MulHigh(res.w, r.diffuse.w) << 4; res.w = AddSat(res.w, arg2->w);
				}
				break;
			case TextureStage::STAGE_BLENDFACTORALPHA:
				{
					res.w = SubSat(arg1->w, arg2->w); res.w = MulHigh(res.w, *Pointer<Short4>(r.data + OFFSET(DrawData,factor.textureFactor4[3]))) << 4; res.w = AddSat(res.w, arg2->w);
				}
				break;
			case TextureStage::STAGE_BLENDTEXTUREALPHA:			// Arg1 * (Alpha) + Arg2 * (1 - Alpha)
				{
					res.w = SubSat(arg1->w, arg2->w); res.w = MulHigh(res.w, texture.w) << 4; res.w = AddSat(res.w, arg2->w);
				}
				break;
			case TextureStage::STAGE_BLENDTEXTUREALPHAPM:		// Arg1 + Arg2 * (1 - Alpha)
				{
					res.w = SubSat(Short4(0x1000), texture.w); res.w = MulHigh(res.w, arg2->w) << 4; res.w = AddSat(res.w, arg1->w);
				}
				break;
			case TextureStage::STAGE_PREMODULATE:
				{
					res.w = arg1->w;
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
			res.x = Max(res.x, Short4(0x0000, 0x0000, 0x0000, 0x0000));
			res.y = Max(res.y, Short4(0x0000, 0x0000, 0x0000, 0x0000));
			res.z = Max(res.z, Short4(0x0000, 0x0000, 0x0000, 0x0000));
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
			res.w = Max(res.w, Short4(0x0000, 0x0000, 0x0000, 0x0000));
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
			res.x = Min(res.x, Short4(0x1000));
			res.y = Min(res.y, Short4(0x1000));
			res.z = Min(res.z, Short4(0x1000));
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
			res.w = Min(res.w, Short4(0x1000));
			break;
		default:
			ASSERT(false);
		}

		switch(textureStage.destinationArgument)
		{
		case TextureStage::DESTINATION_CURRENT:
			r.current.x = res.x;
			r.current.y = res.y;
			r.current.z = res.z;
			r.current.w = res.w;
			break;
		case TextureStage::DESTINATION_TEMP:
			temp.x = res.x;
			temp.y = res.y;
			temp.z = res.z;
			temp.w = res.w;
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

	Bool PixelRoutine::alphaTest(Registers &r, Int cMask[4], Vector4i &current)
	{
		if(!state.alphaTestActive())
		{
			return true;
		}

		Int aMask;

		if(state.transparencyAntialiasing == Context::TRANSPARENCY_NONE)
		{
			alphaTest(r, aMask, current.w);

			for(unsigned int q = 0; q < state.multiSample; q++)
			{
				cMask[q] &= aMask;
			}
		}
		else if(state.transparencyAntialiasing == Context::TRANSPARENCY_ALPHA_TO_COVERAGE)
		{
			Float4 alpha = Float4(current.w) * Float4(1.0f / 0x1000);

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

	Bool PixelRoutine::alphaTest(Registers &r, Int cMask[4], Vector4f &c0)
	{
		if(!state.alphaTestActive())
		{
			return true;
		}

		Int aMask;

		if(state.transparencyAntialiasing == Context::TRANSPARENCY_NONE)
		{
			Short4 alpha = RoundShort4(c0.w * Float4(0x1000));

			alphaTest(r, aMask, alpha);

			for(unsigned int q = 0; q < state.multiSample; q++)
			{
				cMask[q] &= aMask;
			}
		}
		else if(state.transparencyAntialiasing == Context::TRANSPARENCY_ALPHA_TO_COVERAGE)
		{
			alphaToCoverage(r, cMask, c0.w);
		}
		else ASSERT(false);

		Int pass = cMask[0];

		for(unsigned int q = 1; q < state.multiSample; q++)
		{
			pass = pass | cMask[q];
		}

		return pass != 0x0;
	}

	void PixelRoutine::fogBlend(Registers &r, Vector4i &current, Float4 &f, Float4 &z, Float4 &rhw)
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

		current.x = As<Short4>(MulHigh(As<UShort4>(current.x), fog));
		current.y = As<Short4>(MulHigh(As<UShort4>(current.y), fog));
		current.z = As<Short4>(MulHigh(As<UShort4>(current.z), fog));

		UShort4 invFog = UShort4(0xFFFFu) - fog;

		current.x += As<Short4>(MulHigh(invFog, *Pointer<UShort4>(r.data + OFFSET(DrawData,fog.color4[0]))));
		current.y += As<Short4>(MulHigh(invFog, *Pointer<UShort4>(r.data + OFFSET(DrawData,fog.color4[1]))));
		current.z += As<Short4>(MulHigh(invFog, *Pointer<UShort4>(r.data + OFFSET(DrawData,fog.color4[2]))));
	}

	void PixelRoutine::fogBlend(Registers &r, Vector4f &c0, Float4 &fog, Float4 &z, Float4 &rhw)
	{
		if(!state.fogActive)
		{
			return;
		}

		if(state.pixelFogMode != Context::FOG_NONE)
		{
			pixelFog(r, fog, z, rhw);

			fog = Min(fog, Float4(1.0f));
			fog = Max(fog, Float4(0.0f));
		}

		c0.x -= *Pointer<Float4>(r.data + OFFSET(DrawData,fog.colorF[0]));
		c0.y -= *Pointer<Float4>(r.data + OFFSET(DrawData,fog.colorF[1]));
		c0.z -= *Pointer<Float4>(r.data + OFFSET(DrawData,fog.colorF[2]));

		c0.x *= fog;
		c0.y *= fog;
		c0.z *= fog;

		c0.x += *Pointer<Float4>(r.data + OFFSET(DrawData,fog.colorF[0]));
		c0.y += *Pointer<Float4>(r.data + OFFSET(DrawData,fog.colorF[1]));
		c0.z += *Pointer<Float4>(r.data + OFFSET(DrawData,fog.colorF[2]));
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
					zw = Float4(1.0f) - z;
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
			zw = exponential2(zw, true);
			break;
		case Context::FOG_EXP2:
			zw *= *Pointer<Float4>(r.data + OFFSET(DrawData,fog.densityE2));
			zw *= zw;
			zw = exponential2(zw, true);
			zw = Rcp_pp(zw);
			break;
		default:
			ASSERT(false);
		}
	}

	void PixelRoutine::specularPixel(Vector4i &current, Vector4i &specular)
	{
		if(!state.specularAdd)
		{
			return;
		}

		current.x = AddSat(current.x, specular.x);
		current.y = AddSat(current.y, specular.y);
		current.z = AddSat(current.z, specular.z);
	}

	void PixelRoutine::writeDepth(Registers &r, Pointer<Byte> &zBuffer, int q, Int &x, Float4 &z, Int &zMask)
	{
		if(!state.depthWriteEnable)
		{
			return;
		}

		Float4 Z = z;

		if(shader && shader->depthOverride())
		{
			if(complementaryDepthBuffer)
			{
				Z = Float4(1.0f) - r.oDepth;
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

	void PixelRoutine::sampleTexture(Registers &r, Vector4i &c, int coordinates, int stage, bool project)
	{
		Float4 u = r.vf[2 + coordinates].x;
		Float4 v = r.vf[2 + coordinates].y;
		Float4 w = r.vf[2 + coordinates].z;
		Float4 q = r.vf[2 + coordinates].w;

		if(perturbate)
		{
			u += r.du;
			v += r.dv;

			perturbate = false;
		}

		sampleTexture(r, c, stage, u, v, w, q, project);
	}

	void PixelRoutine::sampleTexture(Registers &r, Vector4i &c, int stage, Float4 &u, Float4 &v, Float4 &w, Float4 &q, bool project, bool bias, bool fixed12)
	{
		Vector4f dsx;
		Vector4f dsy;

		sampleTexture(r, c, stage, u, v, w, q, dsx, dsy, project, bias, fixed12, false);
	}

	void PixelRoutine::sampleTexture(Registers &r, Vector4i &c, int stage, Float4 &u, Float4 &v, Float4 &w, Float4 &q, Vector4f &dsx, Vector4f &dsy, bool project, bool bias, bool fixed12, bool gradients, bool lodProvided)
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

	void PixelRoutine::sampleTexture(Registers &r, Vector4f &c, const Src &sampler, Float4 &u, Float4 &v, Float4 &w, Float4 &q, Vector4f &dsx, Vector4f &dsy, bool project, bool bias, bool gradients, bool lodProvided)
	{
		if(sampler.type == Shader::PARAMETER_SAMPLER && sampler.rel.type == Shader::PARAMETER_VOID)
		{	
			sampleTexture(r, c, sampler.index, u, v, w, q, dsx, dsy, project, bias, gradients, lodProvided);	
		}
		else
		{
			Int index = As<Int>(Float(reg(r, sampler).x.x));

			for(int i = 0; i < 16; i++)
			{
				if(shader->usesSampler(i))
				{
					If(index == i)
					{
						sampleTexture(r, c, i, u, v, w, q, dsx, dsy, project, bias, gradients, lodProvided);
						// FIXME: When the sampler states are the same, we could use one sampler and just index the texture
					}
				}
			}
		}
	}

	void PixelRoutine::sampleTexture(Registers &r, Vector4f &c, int stage, Float4 &u, Float4 &v, Float4 &w, Float4 &q, Vector4f &dsx, Vector4f &dsy, bool project, bool bias, bool gradients, bool lodProvided)
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

	void PixelRoutine::clampColor(Vector4f oC[4])
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
			case FORMAT_A8:
			case FORMAT_G16R16:
				oC[index].x = Max(oC[index].x, Float4(0.0f)); oC[index].x = Min(oC[index].x, Float4(1.0f));
				oC[index].y = Max(oC[index].y, Float4(0.0f)); oC[index].y = Min(oC[index].y, Float4(1.0f));
				oC[index].z = Max(oC[index].z, Float4(0.0f)); oC[index].z = Min(oC[index].z, Float4(1.0f));
				oC[index].w = Max(oC[index].w, Float4(0.0f)); oC[index].w = Min(oC[index].w, Float4(1.0f));
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

	void PixelRoutine::rasterOperation(Vector4i &current, Registers &r, Float4 &fog, Pointer<Byte> &cBuffer, Int &x, Int sMask[4], Int zMask[4], Int cMask[4])
	{
		if(!state.colorWriteActive(0))
		{
			return;
		}

		Vector4f oC;

		switch(state.targetFormat[0])
		{
		case FORMAT_X8R8G8B8:
		case FORMAT_A8R8G8B8:
		case FORMAT_A8:
		case FORMAT_G16R16:
		case FORMAT_A16B16G16R16:
			if(!postBlendSRGB && state.writeSRGB)
			{
				linearToSRGB12_16(r, current);
			}
			else
			{
				current.x <<= 4;
				current.y <<= 4;
				current.z <<= 4;
				current.w <<= 4;
			}

			fogBlend(r, current, fog, r.z[0], r.rhw);

			for(unsigned int q = 0; q < state.multiSample; q++)
			{
				Pointer<Byte> buffer = cBuffer + q * *Pointer<Int>(r.data + OFFSET(DrawData,colorSliceB[0]));
				Vector4i color = current;

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
				Vector4f color = oC;

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

	void PixelRoutine::rasterOperation(Vector4f oC[4], Registers &r, Float4 &fog, Pointer<Byte> cBuffer[4], Int &x, Int sMask[4], Int zMask[4], Int cMask[4])
	{
		for(int index = 0; index < 4; index++)
		{
			if(!state.colorWriteActive(index))
			{
				continue;
			}

			if(!postBlendSRGB && state.writeSRGB)
			{
				oC[index].x = linearToSRGB(oC[index].x);
				oC[index].y = linearToSRGB(oC[index].y);
				oC[index].z = linearToSRGB(oC[index].z);
			}

			if(index == 0)
			{
				fogBlend(r, oC[index], fog, r.z[0], r.rhw);
			}

			switch(state.targetFormat[index])
			{
			case FORMAT_X8R8G8B8:
			case FORMAT_A8R8G8B8:
			case FORMAT_A8:
			case FORMAT_G16R16:
			case FORMAT_A16B16G16R16:
				for(unsigned int q = 0; q < state.multiSample; q++)
				{
					Pointer<Byte> buffer = cBuffer[index] + q * *Pointer<Int>(r.data + OFFSET(DrawData,colorSliceB[index]));
					Vector4i color;

					color.x = convertFixed16(oC[index].x, false);
					color.y = convertFixed16(oC[index].y, false);
					color.z = convertFixed16(oC[index].z, false);
					color.w = convertFixed16(oC[index].w, false);

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
					Vector4f color = oC[index];

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

	void PixelRoutine::blendFactor(Registers &r, const Vector4i &blendFactor, const Vector4i &current, const Vector4i &pixel, Context::BlendFactor blendFactorActive)
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
			blendFactor.x = current.x;
			blendFactor.y = current.y;
			blendFactor.z = current.z;
			break;
		case Context::BLEND_INVSOURCE:
			blendFactor.x = Short4(0xFFFFu) - current.x;
			blendFactor.y = Short4(0xFFFFu) - current.y;
			blendFactor.z = Short4(0xFFFFu) - current.z;
			break;
		case Context::BLEND_DEST:
			blendFactor.x = pixel.x;
			blendFactor.y = pixel.y;
			blendFactor.z = pixel.z;
			break;
		case Context::BLEND_INVDEST:
			blendFactor.x = Short4(0xFFFFu) - pixel.x;
			blendFactor.y = Short4(0xFFFFu) - pixel.y;
			blendFactor.z = Short4(0xFFFFu) - pixel.z;
			break;
		case Context::BLEND_SOURCEALPHA:
			blendFactor.x = current.w;
			blendFactor.y = current.w;
			blendFactor.z = current.w;
			break;
		case Context::BLEND_INVSOURCEALPHA:
			blendFactor.x = Short4(0xFFFFu) - current.w;
			blendFactor.y = Short4(0xFFFFu) - current.w;
			blendFactor.z = Short4(0xFFFFu) - current.w;
			break;
		case Context::BLEND_DESTALPHA:
			blendFactor.x = pixel.w;
			blendFactor.y = pixel.w;
			blendFactor.z = pixel.w;
			break;
		case Context::BLEND_INVDESTALPHA:
			blendFactor.x = Short4(0xFFFFu) - pixel.w;
			blendFactor.y = Short4(0xFFFFu) - pixel.w;
			blendFactor.z = Short4(0xFFFFu) - pixel.w;
			break;
		case Context::BLEND_SRCALPHASAT:
			blendFactor.x = Short4(0xFFFFu) - pixel.w;
			blendFactor.x = Min(As<UShort4>(blendFactor.x), As<UShort4>(current.w));
			blendFactor.y = blendFactor.x;
			blendFactor.z = blendFactor.x;
			break;
		case Context::BLEND_CONSTANT:
			blendFactor.x = *Pointer<Short4>(r.data + OFFSET(DrawData,factor.blendConstant4W[0]));
			blendFactor.y = *Pointer<Short4>(r.data + OFFSET(DrawData,factor.blendConstant4W[1]));
			blendFactor.z = *Pointer<Short4>(r.data + OFFSET(DrawData,factor.blendConstant4W[2]));
			break;
		case Context::BLEND_INVCONSTANT:
			blendFactor.x = *Pointer<Short4>(r.data + OFFSET(DrawData,factor.invBlendConstant4W[0]));
			blendFactor.y = *Pointer<Short4>(r.data + OFFSET(DrawData,factor.invBlendConstant4W[1]));
			blendFactor.z = *Pointer<Short4>(r.data + OFFSET(DrawData,factor.invBlendConstant4W[2]));
			break;
		case Context::BLEND_CONSTANTALPHA:
			blendFactor.x = *Pointer<Short4>(r.data + OFFSET(DrawData,factor.blendConstant4W[3]));
			blendFactor.y = *Pointer<Short4>(r.data + OFFSET(DrawData,factor.blendConstant4W[3]));
			blendFactor.z = *Pointer<Short4>(r.data + OFFSET(DrawData,factor.blendConstant4W[3]));
			break;
		case Context::BLEND_INVCONSTANTALPHA:
			blendFactor.x = *Pointer<Short4>(r.data + OFFSET(DrawData,factor.invBlendConstant4W[3]));
			blendFactor.y = *Pointer<Short4>(r.data + OFFSET(DrawData,factor.invBlendConstant4W[3]));
			blendFactor.z = *Pointer<Short4>(r.data + OFFSET(DrawData,factor.invBlendConstant4W[3]));
			break;
		default:
			ASSERT(false);
		}
	}
	
	void PixelRoutine::blendFactorAlpha(Registers &r, const Vector4i &blendFactor, const Vector4i &current, const Vector4i &pixel, Context::BlendFactor blendFactorAlphaActive)
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
			blendFactor.w = current.w;
			break;
		case Context::BLEND_INVSOURCE:
			blendFactor.w = Short4(0xFFFFu) - current.w;
			break;
		case Context::BLEND_DEST:
			blendFactor.w = pixel.w;
			break;
		case Context::BLEND_INVDEST:
			blendFactor.w = Short4(0xFFFFu) - pixel.w;
			break;
		case Context::BLEND_SOURCEALPHA:
			blendFactor.w = current.w;
			break;
		case Context::BLEND_INVSOURCEALPHA:
			blendFactor.w = Short4(0xFFFFu) - current.w;
			break;
		case Context::BLEND_DESTALPHA:
			blendFactor.w = pixel.w;
			break;
		case Context::BLEND_INVDESTALPHA:
			blendFactor.w = Short4(0xFFFFu) - pixel.w;
			break;
		case Context::BLEND_SRCALPHASAT:
			blendFactor.w = Short4(0xFFFFu);
			break;
		case Context::BLEND_CONSTANT:
		case Context::BLEND_CONSTANTALPHA:
			blendFactor.w = *Pointer<Short4>(r.data + OFFSET(DrawData,factor.blendConstant4W[3]));
			break;
		case Context::BLEND_INVCONSTANT:
		case Context::BLEND_INVCONSTANTALPHA:
			blendFactor.w = *Pointer<Short4>(r.data + OFFSET(DrawData,factor.invBlendConstant4W[3]));
			break;
		default:
			ASSERT(false);
		}
	}

	void PixelRoutine::alphaBlend(Registers &r, int index, Pointer<Byte> &cBuffer, Vector4i &current, Int &x)
	{
		if(!state.alphaBlendActive)
		{
			return;
		}
		 
		Pointer<Byte> buffer;

		Vector4i pixel;
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
			pixel.z = c01;
			pixel.y = c01;
			pixel.z = UnpackLow(As<Byte8>(pixel.z), As<Byte8>(c23));
			pixel.y = UnpackHigh(As<Byte8>(pixel.y), As<Byte8>(c23));
			pixel.x = pixel.z;
			pixel.z = UnpackLow(As<Byte8>(pixel.z), As<Byte8>(pixel.y));
			pixel.x = UnpackHigh(As<Byte8>(pixel.x), As<Byte8>(pixel.y));
			pixel.y = pixel.z;
			pixel.w = pixel.x;
			pixel.x = UnpackLow(As<Byte8>(pixel.x), As<Byte8>(pixel.x));
			pixel.y = UnpackHigh(As<Byte8>(pixel.y), As<Byte8>(pixel.y));
			pixel.z = UnpackLow(As<Byte8>(pixel.z), As<Byte8>(pixel.z));
			pixel.w = UnpackHigh(As<Byte8>(pixel.w), As<Byte8>(pixel.w));
			break;
		case FORMAT_A8:
			buffer = cBuffer + 1 * x;
			pixel.w = Insert(pixel.w, *Pointer<Short>(buffer), 0);
			buffer += *Pointer<Int>(r.data + OFFSET(DrawData,colorPitchB[index]));
			pixel.w = Insert(pixel.w, *Pointer<Short>(buffer), 1);
			pixel.w = UnpackLow(As<Byte8>(pixel.w), As<Byte8>(pixel.w));
			pixel.x = Short4(0x0000);
			pixel.y = Short4(0x0000);
			pixel.z = Short4(0x0000);
			break;
		case FORMAT_X8R8G8B8:
			buffer = cBuffer + 4 * x;
			c01 = *Pointer<Short4>(buffer);
			buffer += *Pointer<Int>(r.data + OFFSET(DrawData,colorPitchB[index]));
			c23 = *Pointer<Short4>(buffer);
			pixel.z = c01;
			pixel.y = c01;
			pixel.z = UnpackLow(As<Byte8>(pixel.z), As<Byte8>(c23));
			pixel.y = UnpackHigh(As<Byte8>(pixel.y), As<Byte8>(c23));
			pixel.x = pixel.z;
			pixel.z = UnpackLow(As<Byte8>(pixel.z), As<Byte8>(pixel.y));
			pixel.x = UnpackHigh(As<Byte8>(pixel.x), As<Byte8>(pixel.y));
			pixel.y = pixel.z;
			pixel.x = UnpackLow(As<Byte8>(pixel.x), As<Byte8>(pixel.x));
			pixel.y = UnpackHigh(As<Byte8>(pixel.y), As<Byte8>(pixel.y));
			pixel.z = UnpackLow(As<Byte8>(pixel.z), As<Byte8>(pixel.z));
			pixel.w = Short4(0xFFFFu);
			break;
		case FORMAT_A8G8R8B8Q:
			UNIMPLEMENTED();
		//	pixel.z = UnpackLow(As<Byte8>(pixel.z), *Pointer<Byte8>(cBuffer + 8 * x + 0));
		//	pixel.x = UnpackHigh(As<Byte8>(pixel.x), *Pointer<Byte8>(cBuffer + 8 * x + 0));
		//	pixel.y = UnpackLow(As<Byte8>(pixel.y), *Pointer<Byte8>(cBuffer + 8 * x + 8));
		//	pixel.w = UnpackHigh(As<Byte8>(pixel.w), *Pointer<Byte8>(cBuffer + 8 * x + 8));
			break;
		case FORMAT_X8G8R8B8Q:
			UNIMPLEMENTED();
		//	pixel.z = UnpackLow(As<Byte8>(pixel.z), *Pointer<Byte8>(cBuffer + 8 * x + 0));
		//	pixel.x = UnpackHigh(As<Byte8>(pixel.x), *Pointer<Byte8>(cBuffer + 8 * x + 0));
		//	pixel.y = UnpackLow(As<Byte8>(pixel.y), *Pointer<Byte8>(cBuffer + 8 * x + 8));
		//	pixel.w = Short4(0xFFFFu);
			break;
		case FORMAT_A16B16G16R16:
			buffer  = cBuffer;
			pixel.x = *Pointer<Short4>(buffer + 8 * x);
			pixel.y = *Pointer<Short4>(buffer + 8 * x + 8);
			buffer += *Pointer<Int>(r.data + OFFSET(DrawData,colorPitchB[index]));
			pixel.z = *Pointer<Short4>(buffer + 8 * x);
			pixel.w = *Pointer<Short4>(buffer + 8 * x + 8);
			transpose4x4(pixel.x, pixel.y, pixel.z, pixel.w);
			break;
		case FORMAT_G16R16:
			buffer = cBuffer;
			pixel.x = *Pointer<Short4>(buffer  + 4 * x);
			buffer += *Pointer<Int>(r.data + OFFSET(DrawData,colorPitchB[index]));
			pixel.y = *Pointer<Short4>(buffer  + 4 * x);
			pixel.z = pixel.x;
			pixel.x = As<Short4>(UnpackLow(pixel.x, pixel.y));
			pixel.z = As<Short4>(UnpackHigh(pixel.z, pixel.y));
			pixel.y = pixel.z;
			pixel.x = As<Short4>(UnpackLow(pixel.x, pixel.z));
			pixel.y = As<Short4>(UnpackHigh(pixel.y, pixel.z));
			pixel.z = Short4(0xFFFFu);
			pixel.w = Short4(0xFFFFu);
			break;
		default:
			ASSERT(false);
		}

		if(postBlendSRGB && state.writeSRGB)
		{
			sRGBtoLinear16_16(r, pixel);	
		}

		// Final Color = ObjectColor * SourceBlendFactor + PixelColor * DestinationBlendFactor
		Vector4i sourceFactor;
		Vector4i destFactor;

		blendFactor(r, sourceFactor, current, pixel, (Context::BlendFactor)state.sourceBlendFactor);
		blendFactor(r, destFactor, current, pixel, (Context::BlendFactor)state.destBlendFactor);

		if(state.sourceBlendFactor != Context::BLEND_ONE && state.sourceBlendFactor != Context::BLEND_ZERO)
		{
			current.x = MulHigh(As<UShort4>(current.x), As<UShort4>(sourceFactor.x));
			current.y = MulHigh(As<UShort4>(current.y), As<UShort4>(sourceFactor.y));
			current.z = MulHigh(As<UShort4>(current.z), As<UShort4>(sourceFactor.z));
		}
	
		if(state.destBlendFactor != Context::BLEND_ONE && state.destBlendFactor != Context::BLEND_ZERO)
		{
			pixel.x = MulHigh(As<UShort4>(pixel.x), As<UShort4>(destFactor.x));
			pixel.y = MulHigh(As<UShort4>(pixel.y), As<UShort4>(destFactor.y));
			pixel.z = MulHigh(As<UShort4>(pixel.z), As<UShort4>(destFactor.z));
		}

		switch(state.blendOperation)
		{
		case Context::BLENDOP_ADD:
			current.x = AddSat(As<UShort4>(current.x), As<UShort4>(pixel.x));
			current.y = AddSat(As<UShort4>(current.y), As<UShort4>(pixel.y));
			current.z = AddSat(As<UShort4>(current.z), As<UShort4>(pixel.z));
			break;
		case Context::BLENDOP_SUB:
			current.x = SubSat(As<UShort4>(current.x), As<UShort4>(pixel.x));
			current.y = SubSat(As<UShort4>(current.y), As<UShort4>(pixel.y));
			current.z = SubSat(As<UShort4>(current.z), As<UShort4>(pixel.z));
			break;
		case Context::BLENDOP_INVSUB:
			current.x = SubSat(As<UShort4>(pixel.x), As<UShort4>(current.x));
			current.y = SubSat(As<UShort4>(pixel.y), As<UShort4>(current.y));
			current.z = SubSat(As<UShort4>(pixel.z), As<UShort4>(current.z));
			break;
		case Context::BLENDOP_MIN:
			current.x = Min(As<UShort4>(current.x), As<UShort4>(pixel.x));
			current.y = Min(As<UShort4>(current.y), As<UShort4>(pixel.y));
			current.z = Min(As<UShort4>(current.z), As<UShort4>(pixel.z));
			break;
		case Context::BLENDOP_MAX:
			current.x = Max(As<UShort4>(current.x), As<UShort4>(pixel.x));
			current.y = Max(As<UShort4>(current.y), As<UShort4>(pixel.y));
			current.z = Max(As<UShort4>(current.z), As<UShort4>(pixel.z));
			break;
		case Context::BLENDOP_SOURCE:
			// No operation
			break;
		case Context::BLENDOP_DEST:
			current.x = pixel.x;
			current.y = pixel.y;
			current.z = pixel.z;
			break;
		case Context::BLENDOP_NULL:
			current.x = Short4(0x0000, 0x0000, 0x0000, 0x0000);
			current.y = Short4(0x0000, 0x0000, 0x0000, 0x0000);
			current.z = Short4(0x0000, 0x0000, 0x0000, 0x0000);
			break;
		default:
			ASSERT(false);
		}

		blendFactorAlpha(r, sourceFactor, current, pixel, (Context::BlendFactor)state.sourceBlendFactorAlpha);
		blendFactorAlpha(r, destFactor, current, pixel, (Context::BlendFactor)state.destBlendFactorAlpha);

		if(state.sourceBlendFactorAlpha != Context::BLEND_ONE && state.sourceBlendFactorAlpha != Context::BLEND_ZERO)
		{
			current.w = MulHigh(As<UShort4>(current.w), As<UShort4>(sourceFactor.w));
		}
	
		if(state.destBlendFactorAlpha != Context::BLEND_ONE && state.destBlendFactorAlpha != Context::BLEND_ZERO)
		{
			pixel.w = MulHigh(As<UShort4>(pixel.w), As<UShort4>(destFactor.w));
		}

		switch(state.blendOperationAlpha)
		{
		case Context::BLENDOP_ADD:
			current.w = AddSat(As<UShort4>(current.w), As<UShort4>(pixel.w));
			break;
		case Context::BLENDOP_SUB:
			current.w = SubSat(As<UShort4>(current.w), As<UShort4>(pixel.w));
			break;
		case Context::BLENDOP_INVSUB:
			current.w = SubSat(As<UShort4>(pixel.w), As<UShort4>(current.w));
			break;
		case Context::BLENDOP_MIN:
			current.w = Min(As<UShort4>(current.w), As<UShort4>(pixel.w));
			break;
		case Context::BLENDOP_MAX:
			current.w = Max(As<UShort4>(current.w), As<UShort4>(pixel.w));
			break;
		case Context::BLENDOP_SOURCE:
			// No operation
			break;
		case Context::BLENDOP_DEST:
			current.w = pixel.w;
			break;
		case Context::BLENDOP_NULL:
			current.w = Short4(0x0000, 0x0000, 0x0000, 0x0000);
			break;
		default:
			ASSERT(false);
		}
	}

	void PixelRoutine::writeColor(Registers &r, int index, Pointer<Byte> &cBuffer, Int &x, Vector4i &current, Int &sMask, Int &zMask, Int &cMask)
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
					current.x = current.x - As<Short4>(As<UShort4>(current.x) >> 8) + Short4(0x0080, 0x0080, 0x0080, 0x0080);
					current.y = current.y - As<Short4>(As<UShort4>(current.y) >> 8) + Short4(0x0080, 0x0080, 0x0080, 0x0080);
					current.z = current.z - As<Short4>(As<UShort4>(current.z) >> 8) + Short4(0x0080, 0x0080, 0x0080, 0x0080);
					current.w = current.w - As<Short4>(As<UShort4>(current.w) >> 8) + Short4(0x0080, 0x0080, 0x0080, 0x0080);
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
		//	current.x = As<Short4>(As<UShort4>(current.x) >> 8);
		//	current.y = As<Short4>(As<UShort4>(current.y) >> 8);
		//	current.z = As<Short4>(As<UShort4>(current.z) >> 8);

		//	current.z = As<Short4>(Pack(As<UShort4>(current.z), As<UShort4>(current.x)));
		//	current.y = As<Short4>(Pack(As<UShort4>(current.y), As<UShort4>(current.y)));
			break;
		case FORMAT_A8G8R8B8Q:
			UNIMPLEMENTED();
		//	current.x = As<Short4>(As<UShort4>(current.x) >> 8);
		//	current.y = As<Short4>(As<UShort4>(current.y) >> 8);
		//	current.z = As<Short4>(As<UShort4>(current.z) >> 8);
		//	current.w = As<Short4>(As<UShort4>(current.w) >> 8);

		//	current.z = As<Short4>(Pack(As<UShort4>(current.z), As<UShort4>(current.x)));
		//	current.y = As<Short4>(Pack(As<UShort4>(current.y), As<UShort4>(current.w)));
			break;
		case FORMAT_X8R8G8B8:
		case FORMAT_A8R8G8B8:
			if(state.targetFormat[index] == FORMAT_X8R8G8B8 || rgbaWriteMask == 0x7)
			{
				current.x = As<Short4>(As<UShort4>(current.x) >> 8);
				current.y = As<Short4>(As<UShort4>(current.y) >> 8);
				current.z = As<Short4>(As<UShort4>(current.z) >> 8);

				current.z = As<Short4>(Pack(As<UShort4>(current.z), As<UShort4>(current.x)));
				current.y = As<Short4>(Pack(As<UShort4>(current.y), As<UShort4>(current.y)));

				current.x = current.z;
				current.z = UnpackLow(As<Byte8>(current.z), As<Byte8>(current.y));
				current.x = UnpackHigh(As<Byte8>(current.x), As<Byte8>(current.y));
				current.y = current.z;
				current.z = As<Short4>(UnpackLow(current.z, current.x));
				current.y = As<Short4>(UnpackHigh(current.y, current.x));
			}
			else
			{
				current.x = As<Short4>(As<UShort4>(current.x) >> 8);
				current.y = As<Short4>(As<UShort4>(current.y) >> 8);
				current.z = As<Short4>(As<UShort4>(current.z) >> 8);
				current.w = As<Short4>(As<UShort4>(current.w) >> 8);

				current.z = As<Short4>(Pack(As<UShort4>(current.z), As<UShort4>(current.x)));
				current.y = As<Short4>(Pack(As<UShort4>(current.y), As<UShort4>(current.w)));

				current.x = current.z;
				current.z = UnpackLow(As<Byte8>(current.z), As<Byte8>(current.y));
				current.x = UnpackHigh(As<Byte8>(current.x), As<Byte8>(current.y));
				current.y = current.z;
				current.z = As<Short4>(UnpackLow(current.z, current.x));
				current.y = As<Short4>(UnpackHigh(current.y, current.x));
			}
			break;
		case FORMAT_A8:
			current.w = As<Short4>(As<UShort4>(current.w) >> 8);
			current.w = As<Short4>(Pack(As<UShort4>(current.w), As<UShort4>(current.w)));
			break;
		case FORMAT_G16R16:
			current.z = current.x;
			current.x = As<Short4>(UnpackLow(current.x, current.y));
			current.z = As<Short4>(UnpackHigh(current.z, current.y));
			current.y = current.z;
			break;
		case FORMAT_A16B16G16R16:
			transpose4x4(current.x, current.y, current.z, current.w);
			break;
		case FORMAT_R32F:
		case FORMAT_G32R32F:
		case FORMAT_A32B32G32R32F:
			{
				Vector4f oC;

				oC.x = convertUnsigned16(UShort4(current.x));
				oC.y = convertUnsigned16(UShort4(current.y));
				oC.z = convertUnsigned16(UShort4(current.z));
				oC.w = convertUnsigned16(UShort4(current.w));

				writeColor(r, index, cBuffer, x, oC, sMask, zMask, cMask);
			}
			return;
		default:
			ASSERT(false);
		}

		Short4 c01 = current.z;
		Short4 c23 = current.y;

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
		case FORMAT_A8:
			if(rgbaWriteMask & 0x00000008)
			{
				buffer = cBuffer + 1 * x;
				Insert(value, *Pointer<Short>(buffer), 0);
				Int pitch = *Pointer<Int>(r.data + OFFSET(DrawData,colorPitchB[index]));
				Insert(value, *Pointer<Short>(buffer + pitch), 1);
				value = UnpackLow(As<Byte8>(value), As<Byte8>(value));

				current.w &= *Pointer<Short4>(r.constants + OFFSET(Constants,maskB4Q) + 8 * xMask);
				value &= *Pointer<Short4>(r.constants + OFFSET(Constants,invMaskB4Q) + 8 * xMask);
				current.w |= value;

				*Pointer<Short>(buffer) = Extract(current.w, 0);
				*Pointer<Short>(buffer + pitch) = Extract(current.w, 1);
			}
			break;
		case FORMAT_G16R16:
			buffer = cBuffer + 4 * x;

			value = *Pointer<Short4>(buffer);

			if((rgbaWriteMask & 0x00000003) != 0x00000003)
			{
				Short4 masked = value;
				current.x &= *Pointer<Short4>(r.constants + OFFSET(Constants,maskW01Q[rgbaWriteMask & 0x3][0]));
				masked &= *Pointer<Short4>(r.constants + OFFSET(Constants,invMaskW01Q[rgbaWriteMask & 0x3][0]));
				current.x |= masked;
			}

			current.x &= *Pointer<Short4>(r.constants + OFFSET(Constants,maskD01Q) + xMask * 8);
			value &= *Pointer<Short4>(r.constants + OFFSET(Constants,invMaskD01Q) + xMask * 8);
			current.x |= value;
			*Pointer<Short4>(buffer) = current.x;

			buffer += *Pointer<Int>(r.data + OFFSET(DrawData,colorPitchB[index]));

			value = *Pointer<Short4>(buffer);

			if((rgbaWriteMask & 0x00000003) != 0x00000003)
			{
				Short4 masked = value;
				current.y &= *Pointer<Short4>(r.constants + OFFSET(Constants,maskW01Q[rgbaWriteMask & 0x3][0]));
				masked &= *Pointer<Short4>(r.constants + OFFSET(Constants,invMaskW01Q[rgbaWriteMask & 0x3][0]));
				current.y |= masked;
			}

			current.y &= *Pointer<Short4>(r.constants + OFFSET(Constants,maskD23Q) + xMask * 8);
			value &= *Pointer<Short4>(r.constants + OFFSET(Constants,invMaskD23Q) + xMask * 8);
			current.y |= value;
			*Pointer<Short4>(buffer) = current.y;
			break;
		case FORMAT_A16B16G16R16:
			buffer = cBuffer + 8 * x;

			{
				value = *Pointer<Short4>(buffer);

				if(rgbaWriteMask != 0x0000000F)
				{
					Short4 masked = value;
					current.x &= *Pointer<Short4>(r.constants + OFFSET(Constants,maskW4Q[rgbaWriteMask][0]));
					masked &= *Pointer<Short4>(r.constants + OFFSET(Constants,invMaskW4Q[rgbaWriteMask][0]));
					current.x |= masked;
				}

				current.x &= *Pointer<Short4>(r.constants + OFFSET(Constants,maskQ0Q) + xMask * 8);
				value &= *Pointer<Short4>(r.constants + OFFSET(Constants,invMaskQ0Q) + xMask * 8);
				current.x |= value;
				*Pointer<Short4>(buffer) = current.x;
			}

			{
				value = *Pointer<Short4>(buffer + 8);

				if(rgbaWriteMask != 0x0000000F)
				{
					Short4 masked = value;
					current.y &= *Pointer<Short4>(r.constants + OFFSET(Constants,maskW4Q[rgbaWriteMask][0]));
					masked &= *Pointer<Short4>(r.constants + OFFSET(Constants,invMaskW4Q[rgbaWriteMask][0]));
					current.y |= masked;
				}

				current.y &= *Pointer<Short4>(r.constants + OFFSET(Constants,maskQ1Q) + xMask * 8);
				value &= *Pointer<Short4>(r.constants + OFFSET(Constants,invMaskQ1Q) + xMask * 8);
				current.y |= value;
				*Pointer<Short4>(buffer + 8) = current.y;
			}

			buffer += *Pointer<Int>(r.data + OFFSET(DrawData,colorPitchB[index]));

			{
				value = *Pointer<Short4>(buffer);

				if(rgbaWriteMask != 0x0000000F)
				{
					Short4 masked = value;
					current.z &= *Pointer<Short4>(r.constants + OFFSET(Constants,maskW4Q[rgbaWriteMask][0]));
					masked &= *Pointer<Short4>(r.constants + OFFSET(Constants,invMaskW4Q[rgbaWriteMask][0]));
					current.z |= masked;
				}

				current.z &= *Pointer<Short4>(r.constants + OFFSET(Constants,maskQ2Q) + xMask * 8);
				value &= *Pointer<Short4>(r.constants + OFFSET(Constants,invMaskQ2Q) + xMask * 8);
				current.z |= value;
				*Pointer<Short4>(buffer) = current.z;
			}

			{
				value = *Pointer<Short4>(buffer + 8);

				if(rgbaWriteMask != 0x0000000F)
				{
					Short4 masked = value;
					current.w &= *Pointer<Short4>(r.constants + OFFSET(Constants,maskW4Q[rgbaWriteMask][0]));
					masked &= *Pointer<Short4>(r.constants + OFFSET(Constants,invMaskW4Q[rgbaWriteMask][0]));
					current.w |= masked;
				}

				current.w &= *Pointer<Short4>(r.constants + OFFSET(Constants,maskQ3Q) + xMask * 8);
				value &= *Pointer<Short4>(r.constants + OFFSET(Constants,invMaskQ3Q) + xMask * 8);
				current.w |= value;
				*Pointer<Short4>(buffer + 8) = current.w;
			}
			break;
		default:
			ASSERT(false);
		}
	}

	void PixelRoutine::blendFactor(Registers &r, const Vector4f &blendFactor, const Vector4f &oC, const Vector4f &pixel, Context::BlendFactor blendFactorActive) 
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
			blendFactor.x = oC.x;
			blendFactor.y = oC.y;
			blendFactor.z = oC.z;
			break;
		case Context::BLEND_INVSOURCE:
			blendFactor.x = Float4(1.0f) - oC.x;
			blendFactor.y = Float4(1.0f) - oC.y;
			blendFactor.z = Float4(1.0f) - oC.z;
			break;
		case Context::BLEND_DEST:
			blendFactor.x = pixel.x;
			blendFactor.y = pixel.y;
			blendFactor.z = pixel.z;
			break;
		case Context::BLEND_INVDEST:
			blendFactor.x = Float4(1.0f) - pixel.x;
			blendFactor.y = Float4(1.0f) - pixel.y;
			blendFactor.z = Float4(1.0f) - pixel.z;
			break;
		case Context::BLEND_SOURCEALPHA:
			blendFactor.x = oC.w;
			blendFactor.y = oC.w;
			blendFactor.z = oC.w;
			break;
		case Context::BLEND_INVSOURCEALPHA:
			blendFactor.x = Float4(1.0f) - oC.w;
			blendFactor.y = Float4(1.0f) - oC.w;
			blendFactor.z = Float4(1.0f) - oC.w;
			break;
		case Context::BLEND_DESTALPHA:
			blendFactor.x = pixel.w;
			blendFactor.y = pixel.w;
			blendFactor.z = pixel.w;
			break;
		case Context::BLEND_INVDESTALPHA:
			blendFactor.x = Float4(1.0f) - pixel.w;
			blendFactor.y = Float4(1.0f) - pixel.w;
			blendFactor.z = Float4(1.0f) - pixel.w;
			break;
		case Context::BLEND_SRCALPHASAT:
			blendFactor.x = Float4(1.0f) - pixel.w;
			blendFactor.x = Min(blendFactor.x, oC.w);
			blendFactor.y = blendFactor.x;
			blendFactor.z = blendFactor.x;
			break;
		case Context::BLEND_CONSTANT:
			blendFactor.x = *Pointer<Float4>(r.data + OFFSET(DrawData,factor.blendConstant4F[0]));
			blendFactor.y = *Pointer<Float4>(r.data + OFFSET(DrawData,factor.blendConstant4F[1]));
			blendFactor.z = *Pointer<Float4>(r.data + OFFSET(DrawData,factor.blendConstant4F[2]));
			break;
		case Context::BLEND_INVCONSTANT:
			blendFactor.x = *Pointer<Float4>(r.data + OFFSET(DrawData,factor.invBlendConstant4F[0]));
			blendFactor.y = *Pointer<Float4>(r.data + OFFSET(DrawData,factor.invBlendConstant4F[1]));
			blendFactor.z = *Pointer<Float4>(r.data + OFFSET(DrawData,factor.invBlendConstant4F[2]));
			break;
		default:
			ASSERT(false);
		}
	}

	void PixelRoutine::blendFactorAlpha(Registers &r, const Vector4f &blendFactor, const Vector4f &oC, const Vector4f &pixel, Context::BlendFactor blendFactorAlphaActive) 
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
			blendFactor.w = oC.w;
			break;
		case Context::BLEND_INVSOURCE:
			blendFactor.w = Float4(1.0f) - oC.w;
			break;
		case Context::BLEND_DEST:
			blendFactor.w = pixel.w;
			break;
		case Context::BLEND_INVDEST:
			blendFactor.w = Float4(1.0f) - pixel.w;
			break;
		case Context::BLEND_SOURCEALPHA:
			blendFactor.w = oC.w;
			break;
		case Context::BLEND_INVSOURCEALPHA:
			blendFactor.w = Float4(1.0f) - oC.w;
			break;
		case Context::BLEND_DESTALPHA:
			blendFactor.w = pixel.w;
			break;
		case Context::BLEND_INVDESTALPHA:
			blendFactor.w = Float4(1.0f) - pixel.w;
			break;
		case Context::BLEND_SRCALPHASAT:
			blendFactor.w = Float4(1.0f);
			break;
		case Context::BLEND_CONSTANT:
			blendFactor.w = *Pointer<Float4>(r.data + OFFSET(DrawData,factor.blendConstant4F[3]));
			break;
		case Context::BLEND_INVCONSTANT:
			blendFactor.w = *Pointer<Float4>(r.data + OFFSET(DrawData,factor.invBlendConstant4F[3]));
			break;
		default:
			ASSERT(false);
		}
	}

	void PixelRoutine::alphaBlend(Registers &r, int index, Pointer<Byte> &cBuffer, Vector4f &oC, Int &x)
	{
		if(!state.alphaBlendActive)
		{
			return;
		}

		Pointer<Byte> buffer;
		Vector4f pixel;

		Vector4i color;
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
			color.z = c01;
			color.y = c01;
			color.z = UnpackLow(As<Byte8>(color.z), As<Byte8>(c23));
			color.y = UnpackHigh(As<Byte8>(color.y), As<Byte8>(c23));
			color.x = color.z;
			color.z = UnpackLow(As<Byte8>(color.z), As<Byte8>(color.y));
			color.x = UnpackHigh(As<Byte8>(color.x), As<Byte8>(color.y));
			color.y = color.z;
			color.w = color.x;
			color.x = UnpackLow(As<Byte8>(color.x), As<Byte8>(color.x));
			color.y = UnpackHigh(As<Byte8>(color.y), As<Byte8>(color.y));
			color.z = UnpackLow(As<Byte8>(color.z), As<Byte8>(color.z));
			color.w = UnpackHigh(As<Byte8>(color.w), As<Byte8>(color.w));

			pixel.x = convertUnsigned16(As<UShort4>(color.x));
			pixel.y = convertUnsigned16(As<UShort4>(color.y));
			pixel.z = convertUnsigned16(As<UShort4>(color.z));
			pixel.w = convertUnsigned16(As<UShort4>(color.w));
			break;
		case FORMAT_X8R8G8B8:
			buffer = cBuffer + 4 * x;
			c01 = *Pointer<Short4>(buffer);
			buffer += *Pointer<Int>(r.data + OFFSET(DrawData,colorPitchB[index]));
			c23 = *Pointer<Short4>(buffer);
			color.z = c01;
			color.y = c01;
			color.z = UnpackLow(As<Byte8>(color.z), As<Byte8>(c23));
			color.y = UnpackHigh(As<Byte8>(color.y), As<Byte8>(c23));
			color.x = color.z;
			color.z = UnpackLow(As<Byte8>(color.z), As<Byte8>(color.y));
			color.x = UnpackHigh(As<Byte8>(color.x), As<Byte8>(color.y));
			color.y = color.z;
			color.x = UnpackLow(As<Byte8>(color.x), As<Byte8>(color.x));
			color.y = UnpackHigh(As<Byte8>(color.y), As<Byte8>(color.y));
			color.z = UnpackLow(As<Byte8>(color.z), As<Byte8>(color.z));

			pixel.x = convertUnsigned16(As<UShort4>(color.x));
			pixel.y = convertUnsigned16(As<UShort4>(color.y));
			pixel.z = convertUnsigned16(As<UShort4>(color.z));
			pixel.w = Float4(1.0f);
			break;
		case FORMAT_A8:
			buffer = cBuffer + 1 * x;
			c01 = Insert(c01, *Pointer<Short>(buffer), 0);
			buffer += *Pointer<Int>(r.data + OFFSET(DrawData,colorPitchB[index]));
			c01 = Insert(c01, *Pointer<Short>(buffer), 1);
			pixel.w = convertUnsigned16(As<UShort4>(UnpackLow(As<Byte8>(c01), As<Byte8>(c01))));
			pixel.x = Float4(0.0f);
			pixel.y = Float4(0.0f);
			pixel.z = Float4(0.0f);
			break;
		case FORMAT_A8G8R8B8Q:
			UNIMPLEMENTED();
		//	UnpackLow(pixel.z, qword_ptr [cBuffer+8*x+0]);
		//	UnpackHigh(pixel.x, qword_ptr [cBuffer+8*x+0]);
		//	UnpackLow(pixel.y, qword_ptr [cBuffer+8*x+8]);
		//	UnpackHigh(pixel.w, qword_ptr [cBuffer+8*x+8]);
			break;
		case FORMAT_X8G8R8B8Q:
			UNIMPLEMENTED();
		//	UnpackLow(pixel.z, qword_ptr [cBuffer+8*x+0]);
		//	UnpackHigh(pixel.x, qword_ptr [cBuffer+8*x+0]);
		//	UnpackLow(pixel.y, qword_ptr [cBuffer+8*x+8]);
		//	pixel.w = Short4(0xFFFFu);
			break;
		case FORMAT_A16B16G16R16:
			buffer  = cBuffer;
			color.x = *Pointer<Short4>(buffer + 8 * x);
			color.y = *Pointer<Short4>(buffer + 8 * x + 8);
			buffer += *Pointer<Int>(r.data + OFFSET(DrawData,colorPitchB[index]));
			color.z = *Pointer<Short4>(buffer + 8 * x);
			color.w = *Pointer<Short4>(buffer + 8 * x + 8);
			
			transpose4x4(color.x, color.y, color.z, color.w);

			pixel.x = convertUnsigned16(As<UShort4>(color.x));
			pixel.y = convertUnsigned16(As<UShort4>(color.y));
			pixel.z = convertUnsigned16(As<UShort4>(color.z));
			pixel.w = convertUnsigned16(As<UShort4>(color.w));
			break;
		case FORMAT_G16R16:
			buffer = cBuffer;
			color.x = *Pointer<Short4>(buffer  + 4 * x);
			buffer += *Pointer<Int>(r.data + OFFSET(DrawData,colorPitchB[index]));
			color.y = *Pointer<Short4>(buffer  + 4 * x);
			color.z = color.x;
			color.x = As<Short4>(UnpackLow(color.x, color.y));
			color.z = As<Short4>(UnpackHigh(color.z, color.y));
			color.y = color.z;
			color.x = As<Short4>(UnpackLow(color.x, color.z));
			color.y = As<Short4>(UnpackHigh(color.y, color.z));
			
			pixel.x = convertUnsigned16(As<UShort4>(color.x));
			pixel.y = convertUnsigned16(As<UShort4>(color.y));
			pixel.z = Float4(1.0f);
			pixel.w = Float4(1.0f);
			break;
		case FORMAT_R32F:
			buffer = cBuffer;
			// FIXME: movlps
			pixel.x.x = *Pointer<Float>(buffer + 4 * x + 0);
			pixel.x.y = *Pointer<Float>(buffer + 4 * x + 4);
			buffer += *Pointer<Int>(r.data + OFFSET(DrawData,colorPitchB[index]));
			// FIXME: movhps
			pixel.x.z = *Pointer<Float>(buffer + 4 * x + 0);
			pixel.x.w = *Pointer<Float>(buffer + 4 * x + 4);
			pixel.y = Float4(1.0f);
			pixel.z = Float4(1.0f);
			pixel.w = Float4(1.0f);
			break;
		case FORMAT_G32R32F:
			buffer = cBuffer;
			pixel.x = *Pointer<Float4>(buffer + 8 * x, 16);
			buffer += *Pointer<Int>(r.data + OFFSET(DrawData,colorPitchB[index]));
			pixel.y = *Pointer<Float4>(buffer + 8 * x, 16);
			pixel.z = pixel.x;
			pixel.x = ShuffleLowHigh(pixel.x, pixel.y, 0x88);
			pixel.z = ShuffleLowHigh(pixel.z, pixel.y, 0xDD);
			pixel.y = pixel.z;
			pixel.z = Float4(1.0f);
			pixel.w = Float4(1.0f);
			break;
		case FORMAT_A32B32G32R32F:
			buffer = cBuffer;
			pixel.x = *Pointer<Float4>(buffer + 16 * x, 16);
			pixel.y = *Pointer<Float4>(buffer + 16 * x + 16, 16);
			buffer += *Pointer<Int>(r.data + OFFSET(DrawData,colorPitchB[index]));
			pixel.z = *Pointer<Float4>(buffer + 16 * x, 16);
			pixel.w = *Pointer<Float4>(buffer + 16 * x + 16, 16);
			transpose4x4(pixel.x, pixel.y, pixel.z, pixel.w);
			break;
		default:
			ASSERT(false);
		}

		if(postBlendSRGB && state.writeSRGB)
		{
			sRGBtoLinear(pixel.x);
			sRGBtoLinear(pixel.y);
			sRGBtoLinear(pixel.z);
		}

		// Final Color = ObjectColor * SourceBlendFactor + PixelColor * DestinationBlendFactor
		Vector4f sourceFactor;
		Vector4f destFactor;

		blendFactor(r, sourceFactor, oC, pixel, (Context::BlendFactor)state.sourceBlendFactor);
		blendFactor(r, destFactor, oC, pixel, (Context::BlendFactor)state.destBlendFactor);

		if(state.sourceBlendFactor != Context::BLEND_ONE && state.sourceBlendFactor != Context::BLEND_ZERO)
		{
			oC.x *= sourceFactor.x;
			oC.y *= sourceFactor.y;
			oC.z *= sourceFactor.z;
		}
	
		if(state.destBlendFactor != Context::BLEND_ONE && state.destBlendFactor != Context::BLEND_ZERO)
		{
			pixel.x *= destFactor.x;
			pixel.y *= destFactor.y;
			pixel.z *= destFactor.z;
		}

		switch(state.blendOperation)
		{
		case Context::BLENDOP_ADD:
			oC.x += pixel.x;
			oC.y += pixel.y;
			oC.z += pixel.z;
			break;
		case Context::BLENDOP_SUB:
			oC.x -= pixel.x;
			oC.y -= pixel.y;
			oC.z -= pixel.z;
			break;
		case Context::BLENDOP_INVSUB:
			oC.x = pixel.x - oC.x;
			oC.y = pixel.y - oC.y;
			oC.z = pixel.z - oC.z;
			break;
		case Context::BLENDOP_MIN:
			oC.x = Min(oC.x, pixel.x);
			oC.y = Min(oC.y, pixel.y);
			oC.z = Min(oC.z, pixel.z);
			break;
		case Context::BLENDOP_MAX:
			oC.x = Max(oC.x, pixel.x);
			oC.y = Max(oC.y, pixel.y);
			oC.z = Max(oC.z, pixel.z);
			break;
		case Context::BLENDOP_SOURCE:
			// No operation
			break;
		case Context::BLENDOP_DEST:
			oC.x = pixel.x;
			oC.y = pixel.y;
			oC.z = pixel.z;
			break;
		case Context::BLENDOP_NULL:
			oC.x = Float4(0.0f);
			oC.y = Float4(0.0f);
			oC.z = Float4(0.0f);
			break;
		default:
			ASSERT(false);
		}

		blendFactorAlpha(r, sourceFactor, oC, pixel, (Context::BlendFactor)state.sourceBlendFactorAlpha);
		blendFactorAlpha(r, destFactor, oC, pixel, (Context::BlendFactor)state.destBlendFactorAlpha);

		if(state.sourceBlendFactorAlpha != Context::BLEND_ONE && state.sourceBlendFactorAlpha != Context::BLEND_ZERO)
		{
			oC.w *= sourceFactor.w;
		}
	
		if(state.destBlendFactorAlpha != Context::BLEND_ONE && state.destBlendFactorAlpha != Context::BLEND_ZERO)
		{
			pixel.w *= destFactor.w;
		}

		switch(state.blendOperationAlpha)
		{
		case Context::BLENDOP_ADD:
			oC.w += pixel.w;
			break;
		case Context::BLENDOP_SUB:
			oC.w -= pixel.w;
			break;
		case Context::BLENDOP_INVSUB:
			pixel.w -= oC.w;
			oC.w = pixel.w;
			break;
		case Context::BLENDOP_MIN:	
			oC.w = Min(oC.w, pixel.w);
			break;
		case Context::BLENDOP_MAX:	
			oC.w = Max(oC.w, pixel.w);
			break;
		case Context::BLENDOP_SOURCE:
			// No operation
			break;
		case Context::BLENDOP_DEST:
			oC.w = pixel.w;
			break;
		case Context::BLENDOP_NULL:
			oC.w = Float4(0.0f);
			break;
		default:
			ASSERT(false);
		}
	}

	void PixelRoutine::writeColor(Registers &r, int index, Pointer<Byte> &cBuffer, Int &x, Vector4f &oC, Int &sMask, Int &zMask, Int &cMask)
	{
		if(!state.colorWriteActive(index))
		{
			return;
		}

		Vector4i color;

		switch(state.targetFormat[index])
		{
		case FORMAT_X8R8G8B8:
		case FORMAT_A8R8G8B8:
		case FORMAT_A8:
		case FORMAT_G16R16:
		case FORMAT_A16B16G16R16:
			convertFixed16(color, oC, true);
			writeColor(r, index, cBuffer, x, color, sMask, zMask, cMask);
			return;
		case FORMAT_R32F:
			break;
		case FORMAT_G32R32F:
			oC.z = oC.x;
			oC.x = UnpackLow(oC.x, oC.y);
			oC.z = UnpackHigh(oC.z, oC.y);
			oC.y = oC.z;
			break;
		case FORMAT_A32B32G32R32F:
			transpose4x4(oC.x, oC.y, oC.z, oC.w);
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

				oC.x = As<Float4>(As<Int4>(oC.x) & *Pointer<Int4>(r.constants + OFFSET(Constants,maskD4X) + xMask * 16, 16));
				value = As<Float4>(As<Int4>(value) & *Pointer<Int4>(r.constants + OFFSET(Constants,invMaskD4X) + xMask * 16, 16));
				oC.x = As<Float4>(As<Int4>(oC.x) | As<Int4>(value));

				// FIXME: movhps
				*Pointer<Float>(buffer + 0) = oC.x.z;
				*Pointer<Float>(buffer + 4) = oC.x.w;

				buffer -= *Pointer<Int>(r.data + OFFSET(DrawData,colorPitchB[index]));

				// FIXME: movlps
				*Pointer<Float>(buffer + 0) = oC.x.x;
				*Pointer<Float>(buffer + 4) = oC.x.y;
			}
			break;
		case FORMAT_G32R32F:
			buffer = cBuffer + 8 * x;

			value = *Pointer<Float4>(buffer);

			if((rgbaWriteMask & 0x00000003) != 0x00000003)
			{
				Float4 masked = value;
				oC.x = As<Float4>(As<Int4>(oC.x) & *Pointer<Int4>(r.constants + OFFSET(Constants,maskD01X[rgbaWriteMask & 0x3][0])));
				masked = As<Float4>(As<Int4>(masked) & *Pointer<Int4>(r.constants + OFFSET(Constants,invMaskD01X[rgbaWriteMask & 0x3][0])));
				oC.x = As<Float4>(As<Int4>(oC.x) | As<Int4>(masked));
			}

			oC.x = As<Float4>(As<Int4>(oC.x) & *Pointer<Int4>(r.constants + OFFSET(Constants,maskQ01X) + xMask * 16, 16));
			value = As<Float4>(As<Int4>(value) & *Pointer<Int4>(r.constants + OFFSET(Constants,invMaskQ01X) + xMask * 16, 16));
			oC.x = As<Float4>(As<Int4>(oC.x) | As<Int4>(value));
			*Pointer<Float4>(buffer) = oC.x;

			buffer += *Pointer<Int>(r.data + OFFSET(DrawData,colorPitchB[index]));

			value = *Pointer<Float4>(buffer);

			if((rgbaWriteMask & 0x00000003) != 0x00000003)
			{
				Float4 masked;

				masked = value;
				oC.y = As<Float4>(As<Int4>(oC.y) & *Pointer<Int4>(r.constants + OFFSET(Constants,maskD01X[rgbaWriteMask & 0x3][0])));
				masked = As<Float4>(As<Int4>(masked) & *Pointer<Int4>(r.constants + OFFSET(Constants,invMaskD01X[rgbaWriteMask & 0x3][0])));
				oC.y = As<Float4>(As<Int4>(oC.y) | As<Int4>(masked));
			}

			oC.y = As<Float4>(As<Int4>(oC.y) & *Pointer<Int4>(r.constants + OFFSET(Constants,maskQ23X) + xMask * 16, 16));
			value = As<Float4>(As<Int4>(value) & *Pointer<Int4>(r.constants + OFFSET(Constants,invMaskQ23X) + xMask * 16, 16));
			oC.y = As<Float4>(As<Int4>(oC.y) | As<Int4>(value));
			*Pointer<Float4>(buffer) = oC.y;
			break;
		case FORMAT_A32B32G32R32F:
			buffer = cBuffer + 16 * x;

			{
				value = *Pointer<Float4>(buffer, 16);

				if(rgbaWriteMask != 0x0000000F)
				{
					Float4 masked = value;
					oC.x = As<Float4>(As<Int4>(oC.x) & *Pointer<Int4>(r.constants + OFFSET(Constants,maskD4X[rgbaWriteMask][0])));
					masked = As<Float4>(As<Int4>(masked) & *Pointer<Int4>(r.constants + OFFSET(Constants,invMaskD4X[rgbaWriteMask][0])));
					oC.x = As<Float4>(As<Int4>(oC.x) | As<Int4>(masked));
				}
				
				oC.x = As<Float4>(As<Int4>(oC.x) & *Pointer<Int4>(r.constants + OFFSET(Constants,maskX0X) + xMask * 16, 16));
				value = As<Float4>(As<Int4>(value) & *Pointer<Int4>(r.constants + OFFSET(Constants,invMaskX0X) + xMask * 16, 16));
				oC.x = As<Float4>(As<Int4>(oC.x) | As<Int4>(value));
				*Pointer<Float4>(buffer, 16) = oC.x;
			}

			{
				value = *Pointer<Float4>(buffer + 16, 16);

				if(rgbaWriteMask != 0x0000000F)
				{	
					Float4 masked = value;
					oC.y = As<Float4>(As<Int4>(oC.y) & *Pointer<Int4>(r.constants + OFFSET(Constants,maskD4X[rgbaWriteMask][0])));
					masked = As<Float4>(As<Int4>(masked) & *Pointer<Int4>(r.constants + OFFSET(Constants,invMaskD4X[rgbaWriteMask][0])));
					oC.y = As<Float4>(As<Int4>(oC.y) | As<Int4>(masked));
				}

				oC.y = As<Float4>(As<Int4>(oC.y) & *Pointer<Int4>(r.constants + OFFSET(Constants,maskX1X) + xMask * 16, 16));
				value = As<Float4>(As<Int4>(value) & *Pointer<Int4>(r.constants + OFFSET(Constants,invMaskX1X) + xMask * 16, 16));
				oC.y = As<Float4>(As<Int4>(oC.y) | As<Int4>(value));
				*Pointer<Float4>(buffer + 16, 16) = oC.y;
			}

			buffer += *Pointer<Int>(r.data + OFFSET(DrawData,colorPitchB[index]));

			{
				value = *Pointer<Float4>(buffer, 16);

				if(rgbaWriteMask != 0x0000000F)
				{
					Float4 masked = value;
					oC.z = As<Float4>(As<Int4>(oC.z) & *Pointer<Int4>(r.constants + OFFSET(Constants,maskD4X[rgbaWriteMask][0])));
					masked = As<Float4>(As<Int4>(masked) & *Pointer<Int4>(r.constants + OFFSET(Constants,invMaskD4X[rgbaWriteMask][0])));
					oC.z = As<Float4>(As<Int4>(oC.z) | As<Int4>(masked));
				}

				oC.z = As<Float4>(As<Int4>(oC.z) & *Pointer<Int4>(r.constants + OFFSET(Constants,maskX2X) + xMask * 16, 16));
				value = As<Float4>(As<Int4>(value) & *Pointer<Int4>(r.constants + OFFSET(Constants,invMaskX2X) + xMask * 16, 16));
				oC.z = As<Float4>(As<Int4>(oC.z) | As<Int4>(value));
				*Pointer<Float4>(buffer, 16) = oC.z;
			}

			{
				value = *Pointer<Float4>(buffer + 16, 16);

				if(rgbaWriteMask != 0x0000000F)
				{
					Float4 masked = value;
					oC.w = As<Float4>(As<Int4>(oC.w) & *Pointer<Int4>(r.constants + OFFSET(Constants,maskD4X[rgbaWriteMask][0])));
					masked = As<Float4>(As<Int4>(masked) & *Pointer<Int4>(r.constants + OFFSET(Constants,invMaskD4X[rgbaWriteMask][0])));
					oC.w = As<Float4>(As<Int4>(oC.w) | As<Int4>(masked));
				}

				oC.w = As<Float4>(As<Int4>(oC.w) & *Pointer<Int4>(r.constants + OFFSET(Constants,maskX3X) + xMask * 16, 16));
				value = As<Float4>(As<Int4>(value) & *Pointer<Int4>(r.constants + OFFSET(Constants,invMaskX3X) + xMask * 16, 16));
				oC.w = As<Float4>(As<Int4>(oC.w) | As<Int4>(value));
				*Pointer<Float4>(buffer + 16, 16) = oC.w;
			}
			break;
		default:
			ASSERT(false);
		}
	}

	void PixelRoutine::ps_1_x(Registers &r, Int cMask[4])
	{
		int pad = 0;        // Count number of texm3x3pad instructions
		Vector4i dPairing;   // Destination for first pairing instruction

		for(int i = 0; i < shader->getLength(); i++)
		{
			const Shader::Instruction *instruction = shader->getInstruction(i);
			Shader::Opcode opcode = instruction->opcode;

		//	#ifndef NDEBUG   // FIXME: Centralize debug output control
		//		shader->printInstruction(i, "debug.txt");
		//	#endif

			if(opcode == Shader::OPCODE_DCL || opcode == Shader::OPCODE_DEF || opcode == Shader::OPCODE_DEFI || opcode == Shader::OPCODE_DEFB)
			{
				continue;
			}

			const Dst &dst = instruction->dst;
			const Src &src0 = instruction->src[0];
			const Src &src1 = instruction->src[1];
			const Src &src2 = instruction->src[2];

			unsigned short version = shader->getVersion();
			bool pairing = i + 1 < shader->getLength() && shader->getInstruction(i + 1)->coissue;   // First instruction of pair
			bool coissue = instruction->coissue;                                                              // Second instruction of pair

			Vector4i d;
			Vector4i s0;
			Vector4i s1;
			Vector4i s2;

			if(src0.type != Shader::PARAMETER_VOID) s0 = regi(r, src0);
			if(src1.type != Shader::PARAMETER_VOID) s1 = regi(r, src1);
			if(src2.type != Shader::PARAMETER_VOID) s2 = regi(r, src2);

			Float4 u = version < 0x0104 ? r.vf[2 + dst.index].x : r.vf[2 + src0.index].x;
			Float4 v = version < 0x0104 ? r.vf[2 + dst.index].y : r.vf[2 + src0.index].y;
			Float4 s = version < 0x0104 ? r.vf[2 + dst.index].z : r.vf[2 + src0.index].z;
			Float4 t = version < 0x0104 ? r.vf[2 + dst.index].w : r.vf[2 + src0.index].w;

			switch(opcode)
			{
			case Shader::OPCODE_PS_1_0:															break;
			case Shader::OPCODE_PS_1_1:															break;
			case Shader::OPCODE_PS_1_2:															break;
			case Shader::OPCODE_PS_1_3:															break;
			case Shader::OPCODE_PS_1_4:															break;

			case Shader::OPCODE_DEF:															break;

			case Shader::OPCODE_NOP:															break;
			case Shader::OPCODE_MOV:			MOV(d, s0);										break;
			case Shader::OPCODE_ADD:			ADD(d, s0, s1);									break;
			case Shader::OPCODE_SUB:			SUB(d, s0, s1);									break;
			case Shader::OPCODE_MAD:			MAD(d, s0, s1, s2);								break;
			case Shader::OPCODE_MUL:			MUL(d, s0, s1);									break;
			case Shader::OPCODE_DP3:			DP3(d, s0, s1);									break;
			case Shader::OPCODE_DP4:			DP4(d, s0, s1);									break;
			case Shader::OPCODE_LRP:			LRP(d, s0, s1, s2);								break;
			case Shader::OPCODE_TEXCOORD:
				if(version < 0x0104)
				{
					TEXCOORD(d, u, v, s, dst.index);
				}
				else
				{
					if((src0.swizzle & 0x30) == 0x20)   // .xyz
					{
						TEXCRD(d, u, v, s, src0.index, src0.modifier == Shader::MODIFIER_DZ || src0.modifier == Shader::MODIFIER_DW);
					}
					else   // .xyw
					{
						TEXCRD(d, u, v, t, src0.index, src0.modifier == Shader::MODIFIER_DZ || src0.modifier == Shader::MODIFIER_DW);
					}
				}
				break;
			case Shader::OPCODE_TEXKILL:
				if(version < 0x0104)
				{
					TEXKILL(cMask, u, v, s);
				}
				else if(version == 0x0104)
				{
					if(dst.type == Shader::PARAMETER_TEXTURE)
					{
						TEXKILL(cMask, u, v, s);
					}
					else
					{
						TEXKILL(cMask, r.ri[dst.index]);
					}
				}
				else ASSERT(false);
				break;
			case Shader::OPCODE_TEX:
				if(version < 0x0104)
				{
					TEX(r, d, u, v, s, dst.index, false);
				}
				else if(version == 0x0104)
				{
					if(src0.type == Shader::PARAMETER_TEXTURE)
					{
						if((src0.swizzle & 0x30) == 0x20)   // .xyz
						{
							TEX(r, d, u, v, s, dst.index, src0.modifier == Shader::MODIFIER_DZ || src0.modifier == Shader::MODIFIER_DW);
						}
						else   // .xyw
						{
							TEX(r, d, u, v, t, dst.index, src0.modifier == Shader::MODIFIER_DZ || src0.modifier == Shader::MODIFIER_DW);
						}
					}
					else
					{
						TEXLD(r, d, s0, dst.index, src0.modifier == Shader::MODIFIER_DZ || src0.modifier == Shader::MODIFIER_DW);
					}
				}
				else ASSERT(false);
				break;
			case Shader::OPCODE_TEXBEM:			TEXBEM(r, d, s0, u, v, s, dst.index);	break;
			case Shader::OPCODE_TEXBEML:		TEXBEML(r, d, s0, u, v, s, dst.index);	break;
			case Shader::OPCODE_TEXREG2AR:		TEXREG2AR(r, d, s0, dst.index);					break;
			case Shader::OPCODE_TEXREG2GB:		TEXREG2GB(r, d, s0, dst.index);					break;
			case Shader::OPCODE_TEXM3X2PAD:		TEXM3X2PAD(r, u, v, s, s0, 0, src0.modifier == Shader::MODIFIER_SIGN);	break;
			case Shader::OPCODE_TEXM3X2TEX:		TEXM3X2TEX(r, d, u, v, s, dst.index, s0, src0.modifier == Shader::MODIFIER_SIGN);	break;
			case Shader::OPCODE_TEXM3X3PAD:		TEXM3X3PAD(r, u, v, s, s0, pad++ % 2, src0.modifier == Shader::MODIFIER_SIGN);	break;
			case Shader::OPCODE_TEXM3X3TEX:		TEXM3X3TEX(r, d, u, v, s, dst.index, s0, src0.modifier == Shader::MODIFIER_SIGN);	break;
			case Shader::OPCODE_TEXM3X3SPEC:	TEXM3X3SPEC(r, d, u, v, s, dst.index, s0, s1);		break;
			case Shader::OPCODE_TEXM3X3VSPEC:	TEXM3X3VSPEC(r, d, u, v, s, dst.index, s0);		break;
			case Shader::OPCODE_CND:			CND(d, s0, s1, s2);								break;
			case Shader::OPCODE_TEXREG2RGB:		TEXREG2RGB(r, d, s0, dst.index);				break;
			case Shader::OPCODE_TEXDP3TEX:		TEXDP3TEX(r, d, u, v, s, dst.index, s0);	break;
			case Shader::OPCODE_TEXM3X2DEPTH:	TEXM3X2DEPTH(r, d, u, v, s, s0, src0.modifier == Shader::MODIFIER_SIGN);	break;
			case Shader::OPCODE_TEXDP3:			TEXDP3(r, d, u, v, s, s0);				break;
			case Shader::OPCODE_TEXM3X3:		TEXM3X3(r, d, u, v, s, s0, src0.modifier == Shader::MODIFIER_SIGN); 	break;
			case Shader::OPCODE_TEXDEPTH:		TEXDEPTH(r);									break;
			case Shader::OPCODE_CMP0:			CMP(d, s0, s1, s2);								break;
			case Shader::OPCODE_BEM:			BEM(r, d, s0, s1, dst.index);					break;
			case Shader::OPCODE_PHASE:															break;
			case Shader::OPCODE_END:															break;
			default:
				ASSERT(false);
			}

			if(dst.type != Shader::PARAMETER_VOID && opcode != Shader::OPCODE_TEXKILL)
			{
				if(dst.shift > 0)
				{
					if(dst.mask & 0x1) {d.x = AddSat(d.x, d.x); if(dst.shift > 1) d.x = AddSat(d.x, d.x); if(dst.shift > 2) d.x = AddSat(d.x, d.x);}
					if(dst.mask & 0x2) {d.y = AddSat(d.y, d.y); if(dst.shift > 1) d.y = AddSat(d.y, d.y); if(dst.shift > 2) d.y = AddSat(d.y, d.y);}
					if(dst.mask & 0x4) {d.z = AddSat(d.z, d.z); if(dst.shift > 1) d.z = AddSat(d.z, d.z); if(dst.shift > 2) d.z = AddSat(d.z, d.z);}
					if(dst.mask & 0x8) {d.w = AddSat(d.w, d.w); if(dst.shift > 1) d.w = AddSat(d.w, d.w); if(dst.shift > 2) d.w = AddSat(d.w, d.w);}
				}
				else if(dst.shift < 0)
				{
					if(dst.mask & 0x1) d.x = d.x >> -dst.shift;
					if(dst.mask & 0x2) d.y = d.y >> -dst.shift;
					if(dst.mask & 0x4) d.z = d.z >> -dst.shift;
					if(dst.mask & 0x8) d.w = d.w >> -dst.shift;
				}

				if(dst.saturate)
				{
					if(dst.mask & 0x1) {d.x = Min(d.x, Short4(0x1000)); d.x = Max(d.x, Short4(0x0000, 0x0000, 0x0000, 0x0000));}
					if(dst.mask & 0x2) {d.y = Min(d.y, Short4(0x1000)); d.y = Max(d.y, Short4(0x0000, 0x0000, 0x0000, 0x0000));}
					if(dst.mask & 0x4) {d.z = Min(d.z, Short4(0x1000)); d.z = Max(d.z, Short4(0x0000, 0x0000, 0x0000, 0x0000));}
					if(dst.mask & 0x8) {d.w = Min(d.w, Short4(0x1000)); d.w = Max(d.w, Short4(0x0000, 0x0000, 0x0000, 0x0000));}
				}

				if(pairing)
				{
					if(dst.mask & 0x1) dPairing.x = d.x;
					if(dst.mask & 0x2) dPairing.y = d.y;
					if(dst.mask & 0x4) dPairing.z = d.z;
					if(dst.mask & 0x8) dPairing.w = d.w;
				}
			
				if(coissue)
				{
					const Dst &dst = shader->getInstruction(i - 1)->dst;

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

		if(shader->containsLeaveInstruction())
		{
			r.enableLeave = Int4(0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF);
		}

		bool out[4][4] = {false};

		// Create all call site return blocks up front
		for(int i = 0; i < shader->getLength(); i++)
		{
			const Shader::Instruction *instruction = shader->getInstruction(i);
			Shader::Opcode opcode = instruction->opcode;

			if(opcode == Shader::OPCODE_CALL || opcode == Shader::OPCODE_CALLNZ)
			{
				const Dst &dst = instruction->dst;

				ASSERT(callRetBlock[dst.label].size() == dst.callSite);
				callRetBlock[dst.label].push_back(Nucleus::createBasicBlock());
			}
		}
		
		for(int i = 0; i < shader->getLength(); i++)
		{
			const Shader::Instruction *instruction = shader->getInstruction(i);
			Shader::Opcode opcode = instruction->opcode;

			if(opcode == Shader::OPCODE_DCL || opcode == Shader::OPCODE_DEF || opcode == Shader::OPCODE_DEFI || opcode == Shader::OPCODE_DEFB)
			{
				continue;
			}

			const Dst &dst = instruction->dst;
			const Src &src0 = instruction->src[0];
			const Src &src1 = instruction->src[1];
			const Src &src2 = instruction->src[2];
			const Src &src3 = instruction->src[3];

			bool predicate = instruction->predicate;
			Control control = instruction->control;
			bool pp = dst.partialPrecision;
			bool project = instruction->project;
			bool bias = instruction->bias;

			Vector4f d;
			Vector4f s0;
			Vector4f s1;
			Vector4f s2;
			Vector4f s3;

			if(opcode == Shader::OPCODE_TEXKILL)   // Takes destination as input
			{
				if(dst.type == Shader::PARAMETER_TEXTURE)
				{
					d.x = r.vf[2 + dst.index].x;
					d.y = r.vf[2 + dst.index].y;
					d.z = r.vf[2 + dst.index].z;
					d.w = r.vf[2 + dst.index].w;
				}
				else
				{
					d = r.rf[dst.index];
				}
			}

			if(src0.type != Shader::PARAMETER_VOID) s0 = reg(r, src0);
			if(src1.type != Shader::PARAMETER_VOID) s1 = reg(r, src1);
			if(src2.type != Shader::PARAMETER_VOID) s2 = reg(r, src2);
			if(src3.type != Shader::PARAMETER_VOID) s3 = reg(r, src3);

			switch(opcode)
			{
			case Shader::OPCODE_PS_2_0:														break;
			case Shader::OPCODE_PS_2_x:														break;
			case Shader::OPCODE_PS_3_0:														break;
			case Shader::OPCODE_DEF:														break;
			case Shader::OPCODE_DCL:														break;
			case Shader::OPCODE_NOP:														break;
			case Shader::OPCODE_MOV:		mov(d, s0);										break;
			case Shader::OPCODE_F2B:		f2b(d, s0);										break;
			case Shader::OPCODE_B2F:		b2f(d, s0);										break;
			case Shader::OPCODE_ADD:		add(d, s0, s1);									break;
			case Shader::OPCODE_SUB:		sub(d, s0, s1);									break;
			case Shader::OPCODE_MUL:		mul(d, s0, s1);									break;
			case Shader::OPCODE_MAD:		mad(d, s0, s1, s2);								break;
			case Shader::OPCODE_DP1:		dp1(d, s0, s1);									break;
			case Shader::OPCODE_DP2:		dp2(d, s0, s1);									break;
			case Shader::OPCODE_DP2ADD:		dp2add(d, s0, s1, s2);							break;
			case Shader::OPCODE_DP3:		dp3(d, s0, s1);									break;
			case Shader::OPCODE_DP4:		dp4(d, s0, s1);									break;
			case Shader::OPCODE_CMP0:		cmp0(d, s0, s1, s2);							break;
			case Shader::OPCODE_ICMP:		icmp(d, s0, s1, control);						break;
			case Shader::OPCODE_SELECT:		select(d, s0, s1, s2);							break;
			case Shader::OPCODE_EXTRACT:	extract(d.x, s0, s1.x);							break;
			case Shader::OPCODE_INSERT:		insert(d, s0, s1.x, s2.x);						break;
			case Shader::OPCODE_FRC:		frc(d, s0);										break;
			case Shader::OPCODE_TRUNC:      trunc(d, s0);                                   break;
			case Shader::OPCODE_FLOOR:      floor(d, s0);                                   break;
			case Shader::OPCODE_CEIL:       ceil(d, s0);                                    break;
			case Shader::OPCODE_EXP2X:		exp2x(d, s0, pp);								break;
			case Shader::OPCODE_EXP2:		exp2(d, s0, pp);								break;
			case Shader::OPCODE_LOG2X:		log2x(d, s0, pp);								break;
			case Shader::OPCODE_LOG2:		log2(d, s0, pp);								break;
			case Shader::OPCODE_EXP:		exp(d, s0, pp);									break;
			case Shader::OPCODE_LOG:		log(d, s0, pp);									break;
			case Shader::OPCODE_RCPX:		rcpx(d, s0, pp);								break;
			case Shader::OPCODE_DIV:		div(d, s0, s1);									break;
			case Shader::OPCODE_MOD:		mod(d, s0, s1);									break;
			case Shader::OPCODE_RSQX:		rsqx(d, s0, pp);								break;
			case Shader::OPCODE_SQRT:		sqrt(d, s0, pp);								break;
			case Shader::OPCODE_RSQ:		rsq(d, s0, pp);									break;
			case Shader::OPCODE_LEN2:		len2(d.x, s0, pp);								break;
			case Shader::OPCODE_LEN3:		len3(d.x, s0, pp);								break;
			case Shader::OPCODE_LEN4:		len4(d.x, s0, pp);								break;
			case Shader::OPCODE_DIST1:		dist1(d.x, s0, s1, pp);							break;
			case Shader::OPCODE_DIST2:		dist2(d.x, s0, s1, pp);							break;
			case Shader::OPCODE_DIST3:		dist3(d.x, s0, s1, pp);							break;
			case Shader::OPCODE_DIST4:		dist4(d.x, s0, s1, pp);							break;
			case Shader::OPCODE_MIN:		min(d, s0, s1);									break;
			case Shader::OPCODE_MAX:		max(d, s0, s1);									break;
			case Shader::OPCODE_LRP:		lrp(d, s0, s1, s2);								break;
			case Shader::OPCODE_STEP:		step(d, s0, s1);								break;
			case Shader::OPCODE_SMOOTH:		smooth(d, s0, s1, s2);							break;
			case Shader::OPCODE_POWX:		powx(d, s0, s1, pp);							break;
			case Shader::OPCODE_POW:		pow(d, s0, s1, pp);								break;
			case Shader::OPCODE_SGN:		sgn(d, s0);										break;
			case Shader::OPCODE_CRS:		crs(d, s0, s1);									break;
			case Shader::OPCODE_FORWARD1:	forward1(d, s0, s1, s2);						break;
			case Shader::OPCODE_FORWARD2:	forward2(d, s0, s1, s2);						break;
			case Shader::OPCODE_FORWARD3:	forward3(d, s0, s1, s2);						break;
			case Shader::OPCODE_FORWARD4:	forward4(d, s0, s1, s2);						break;
			case Shader::OPCODE_REFLECT1:	reflect1(d, s0, s1);							break;
			case Shader::OPCODE_REFLECT2:	reflect2(d, s0, s1);							break;
			case Shader::OPCODE_REFLECT3:	reflect3(d, s0, s1);							break;
			case Shader::OPCODE_REFLECT4:	reflect4(d, s0, s1);							break;
			case Shader::OPCODE_REFRACT1:	refract1(d, s0, s1, s2.x);						break;
			case Shader::OPCODE_REFRACT2:	refract2(d, s0, s1, s2.x);						break;
			case Shader::OPCODE_REFRACT3:	refract3(d, s0, s1, s2.x);						break;
			case Shader::OPCODE_REFRACT4:	refract4(d, s0, s1, s2.x);						break;
			case Shader::OPCODE_NRM2:		nrm2(d, s0, pp);								break;
			case Shader::OPCODE_NRM3:		nrm3(d, s0, pp);								break;
			case Shader::OPCODE_NRM4:		nrm4(d, s0, pp);								break;
			case Shader::OPCODE_ABS:		abs(d, s0);										break;
			case Shader::OPCODE_SINCOS:		sincos(d, s0, pp);								break;
			case Shader::OPCODE_COS:		cos(d, s0, pp);									break;
			case Shader::OPCODE_SIN:		sin(d, s0, pp);									break;
			case Shader::OPCODE_TAN:		tan(d, s0, pp);									break;
			case Shader::OPCODE_ACOS:		acos(d, s0, pp);								break;
			case Shader::OPCODE_ASIN:		asin(d, s0, pp);								break;
			case Shader::OPCODE_ATAN:		atan(d, s0, pp);								break;
			case Shader::OPCODE_ATAN2:		atan2(d, s0, s1, pp);							break;
			case Shader::OPCODE_M4X4:		M4X4(r, d, s0, src1);							break;
			case Shader::OPCODE_M4X3:		M4X3(r, d, s0, src1);							break;
			case Shader::OPCODE_M3X4:		M3X4(r, d, s0, src1);							break;
			case Shader::OPCODE_M3X3:		M3X3(r, d, s0, src1);							break;
			case Shader::OPCODE_M3X2:		M3X2(r, d, s0, src1);							break;
			case Shader::OPCODE_TEX:		TEXLD(r, d, s0, src1, project, bias);			break;
			case Shader::OPCODE_TEXLDD:		TEXLDD(r, d, s0, src1, s2, s3, project, bias);	break;
			case Shader::OPCODE_TEXLDL:		TEXLDL(r, d, s0, src1, project, bias);			break;
			case Shader::OPCODE_TEXKILL:	TEXKILL(cMask, d, dst.mask);					break;
			case Shader::OPCODE_DISCARD:	DISCARD(r, cMask, instruction);					break;
			case Shader::OPCODE_DFDX:		DFDX(d, s0);									break;
			case Shader::OPCODE_DFDY:		DFDY(d, s0);									break;
			case Shader::OPCODE_FWIDTH:		FWIDTH(d, s0);									break;
			case Shader::OPCODE_BREAK:		BREAK(r);										break;
			case Shader::OPCODE_BREAKC:		BREAKC(r, s0, s1, control);						break;
			case Shader::OPCODE_BREAKP:		BREAKP(r, src0);								break;
			case Shader::OPCODE_CONTINUE:	CONTINUE(r);									break;
			case Shader::OPCODE_TEST:		TEST();											break;
			case Shader::OPCODE_CALL:		CALL(r, dst.label, dst.callSite);               break;
			case Shader::OPCODE_CALLNZ:		CALLNZ(r, dst.label, dst.callSite, src0);       break;
			case Shader::OPCODE_ELSE:		ELSE(r);										break;
			case Shader::OPCODE_ENDIF:		ENDIF(r);										break;
			case Shader::OPCODE_ENDLOOP:	ENDLOOP(r);										break;
			case Shader::OPCODE_ENDREP:		ENDREP(r);										break;
			case Shader::OPCODE_ENDWHILE:	ENDWHILE(r);	     							break;
			case Shader::OPCODE_IF:			IF(r, src0);									break;
			case Shader::OPCODE_IFC:		IFC(r, s0, s1, control);						break;
			case Shader::OPCODE_LABEL:		LABEL(dst.index);								break;
			case Shader::OPCODE_LOOP:		LOOP(r, src1);									break;
			case Shader::OPCODE_REP:		REP(r, src0);									break;
			case Shader::OPCODE_WHILE:		WHILE(r, src0);									break;
			case Shader::OPCODE_RET:		RET(r);											break;
			case Shader::OPCODE_LEAVE:		LEAVE(r);										break;
			case Shader::OPCODE_CMP:		cmp(d, s0, s1, control);						break;
			case Shader::OPCODE_ALL:		all(d.x, s0);									break;
			case Shader::OPCODE_ANY:		any(d.x, s0);									break;
			case Shader::OPCODE_NOT:		not(d, s0);										break;
			case Shader::OPCODE_OR:			or(d.x, s0.x, s1.x);							break;
			case Shader::OPCODE_XOR:		xor(d.x, s0.x, s1.x);							break;
			case Shader::OPCODE_AND:		and(d.x, s0.x, s1.x);							break;
			case Shader::OPCODE_END:														break;
			default:
				ASSERT(false);
			}

			if(dst.type != Shader::PARAMETER_VOID && dst.type != Shader::PARAMETER_LABEL && opcode != Shader::OPCODE_TEXKILL && opcode != Shader::OPCODE_NOP)
			{
				if(dst.integer)
				{
					switch(opcode)
					{
					case Shader::OPCODE_DIV:
						if(dst.x) d.x = Trunc(d.x);
						if(dst.y) d.y = Trunc(d.y);
						if(dst.z) d.z = Trunc(d.z);
						if(dst.w) d.w = Trunc(d.w);
						break;
					default:
						break;   // No truncation to integer required when arguments are integer
					}
				}

				if(dst.saturate)
				{
					if(dst.x) d.x = Max(d.x, Float4(0.0f));
					if(dst.y) d.y = Max(d.y, Float4(0.0f));
					if(dst.z) d.z = Max(d.z, Float4(0.0f));
					if(dst.w) d.w = Max(d.w, Float4(0.0f));

					if(dst.x) d.x = Min(d.x, Float4(1.0f));
					if(dst.y) d.y = Min(d.y, Float4(1.0f));
					if(dst.z) d.z = Min(d.z, Float4(1.0f));
					if(dst.w) d.w = Min(d.w, Float4(1.0f));
				}

				if(instruction->isPredicated())
				{
					Vector4f pDst;   // FIXME: Rename

					switch(dst.type)
					{
					case Shader::PARAMETER_TEMP:
						if(dst.rel.type == Shader::PARAMETER_VOID)
						{
							if(dst.x) pDst.x = r.rf[dst.index].x;
							if(dst.y) pDst.y = r.rf[dst.index].y;
							if(dst.z) pDst.z = r.rf[dst.index].z;
							if(dst.w) pDst.w = r.rf[dst.index].w;
						}
						else
						{
							Int a = relativeAddress(r, dst);

							if(dst.x) pDst.x = r.rf[dst.index + a].x;
							if(dst.y) pDst.y = r.rf[dst.index + a].y;
							if(dst.z) pDst.z = r.rf[dst.index + a].z;
							if(dst.w) pDst.w = r.rf[dst.index + a].w;
						}
						break;
					case Shader::PARAMETER_COLOROUT:
						ASSERT(dst.rel.type == Shader::PARAMETER_VOID);
						if(dst.x) pDst.x = r.oC[dst.index].x;
						if(dst.y) pDst.y = r.oC[dst.index].y;
						if(dst.z) pDst.z = r.oC[dst.index].z;
						if(dst.w) pDst.w = r.oC[dst.index].w;
						break;
					case Shader::PARAMETER_PREDICATE:
						if(dst.x) pDst.x = r.p0.x;
						if(dst.y) pDst.y = r.p0.y;
						if(dst.z) pDst.z = r.p0.z;
						if(dst.w) pDst.w = r.p0.w;
						break;
					case Shader::PARAMETER_DEPTHOUT:
						pDst.x = r.oDepth;
						break;
					default:
						ASSERT(false);
					}
				
					Int4 enable = enableMask(r, instruction);

					Int4 xEnable = enable;
					Int4 yEnable = enable;
					Int4 zEnable = enable;
					Int4 wEnable = enable;

					if(predicate)
					{
						unsigned char pSwizzle = instruction->predicateSwizzle;

						Float4 xPredicate = r.p0[(pSwizzle >> 0) & 0x03];
						Float4 yPredicate = r.p0[(pSwizzle >> 2) & 0x03];
						Float4 zPredicate = r.p0[(pSwizzle >> 4) & 0x03];
						Float4 wPredicate = r.p0[(pSwizzle >> 6) & 0x03];

						if(!instruction->predicateNot)
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
				case Shader::PARAMETER_TEMP:
					if(dst.rel.type == Shader::PARAMETER_VOID)
					{
						if(dst.x) r.rf[dst.index].x = d.x;
						if(dst.y) r.rf[dst.index].y = d.y;
						if(dst.z) r.rf[dst.index].z = d.z;
						if(dst.w) r.rf[dst.index].w = d.w;
					}
					else
					{
						Int a = relativeAddress(r, dst);

						if(dst.x) r.rf[dst.index + a].x = d.x;
						if(dst.y) r.rf[dst.index + a].y = d.y;
						if(dst.z) r.rf[dst.index + a].z = d.z;
						if(dst.w) r.rf[dst.index + a].w = d.w;
					}
					break;
				case Shader::PARAMETER_COLOROUT:
					ASSERT(dst.rel.type == Shader::PARAMETER_VOID);
					if(dst.x) {r.oC[dst.index].x = d.x; out[dst.index][0] = true;}
					if(dst.y) {r.oC[dst.index].y = d.y; out[dst.index][1] = true;}
					if(dst.z) {r.oC[dst.index].z = d.z; out[dst.index][2] = true;}
					if(dst.w) {r.oC[dst.index].w = d.w; out[dst.index][3] = true;}
					break;
				case Shader::PARAMETER_PREDICATE:
					if(dst.x) r.p0.x = d.x;
					if(dst.y) r.p0.y = d.y;
					if(dst.z) r.p0.z = d.z;
					if(dst.w) r.p0.w = d.w;
					break;
				case Shader::PARAMETER_DEPTHOUT:
					r.oDepth = d.x;
					break;
				default:
					ASSERT(false);
				}
			}
		}

		if(currentLabel != -1)
		{
			Nucleus::setInsertBlock(returnBlock);
		}

		for(int i = 0; i < 4; i++)
		{
			if((Format)state.targetFormat[i] != FORMAT_NULL)
			{
				if(!out[i][0]) r.oC[i].x = Float4(0.0f);
				if(!out[i][1]) r.oC[i].y = Float4(0.0f);
				if(!out[i][2]) r.oC[i].z = Float4(0.0f);
				if(!out[i][3]) r.oC[i].w = Float4(0.0f);
			}
		}
	}

	Short4 PixelRoutine::convertFixed12(RValue<Float4> cf)
	{
		return RoundShort4(cf * Float4(0x1000));
	}

	void PixelRoutine::convertFixed12(Vector4i &ci, Vector4f &cf)
	{
		ci.x = convertFixed12(cf.x);
		ci.y = convertFixed12(cf.y);
		ci.z = convertFixed12(cf.z);
		ci.w = convertFixed12(cf.w);
	}

	UShort4 PixelRoutine::convertFixed16(Float4 &cf, bool saturate)
	{
		return UShort4(cf * Float4(0xFFFF), saturate);
	}

	void PixelRoutine::convertFixed16(Vector4i &ci, Vector4f &cf, bool saturate)
	{
		ci.x = convertFixed16(cf.x, saturate);
		ci.y = convertFixed16(cf.y, saturate);
		ci.z = convertFixed16(cf.z, saturate);
		ci.w = convertFixed16(cf.w, saturate);
	}

	Float4 PixelRoutine::convertSigned12(Short4 &ci)
	{
		return Float4(ci) * Float4(1.0f / 0x0FFE);
	}

	void PixelRoutine::convertSigned12(Vector4f &cf, Vector4i &ci)
	{
		cf.x = convertSigned12(ci.x);
		cf.y = convertSigned12(ci.y);
		cf.z = convertSigned12(ci.z);
		cf.w = convertSigned12(ci.w);
	}

	Float4 PixelRoutine::convertUnsigned16(UShort4 ci)
	{
		return Float4(ci) * Float4(1.0f / 0xFFFF);
	}

	void PixelRoutine::sRGBtoLinear16_16(Registers &r, Vector4i &c)
	{
		c.x = As<UShort4>(c.x) >> 4;
		c.y = As<UShort4>(c.y) >> 4;
		c.z = As<UShort4>(c.z) >> 4;

		sRGBtoLinear12_16(r, c);
	}

	void PixelRoutine::sRGBtoLinear12_16(Registers &r, Vector4i &c)
	{
		Pointer<Byte> LUT = r.constants + OFFSET(Constants,sRGBtoLin12_16);

		c.x = Insert(c.x, *Pointer<Short>(LUT + 2 * Int(Extract(c.x, 0))), 0);
		c.x = Insert(c.x, *Pointer<Short>(LUT + 2 * Int(Extract(c.x, 1))), 1);
		c.x = Insert(c.x, *Pointer<Short>(LUT + 2 * Int(Extract(c.x, 2))), 2);
		c.x = Insert(c.x, *Pointer<Short>(LUT + 2 * Int(Extract(c.x, 3))), 3);

		c.y = Insert(c.y, *Pointer<Short>(LUT + 2 * Int(Extract(c.y, 0))), 0);
		c.y = Insert(c.y, *Pointer<Short>(LUT + 2 * Int(Extract(c.y, 1))), 1);
		c.y = Insert(c.y, *Pointer<Short>(LUT + 2 * Int(Extract(c.y, 2))), 2);
		c.y = Insert(c.y, *Pointer<Short>(LUT + 2 * Int(Extract(c.y, 3))), 3);

		c.z = Insert(c.z, *Pointer<Short>(LUT + 2 * Int(Extract(c.z, 0))), 0);
		c.z = Insert(c.z, *Pointer<Short>(LUT + 2 * Int(Extract(c.z, 1))), 1);
		c.z = Insert(c.z, *Pointer<Short>(LUT + 2 * Int(Extract(c.z, 2))), 2);
		c.z = Insert(c.z, *Pointer<Short>(LUT + 2 * Int(Extract(c.z, 3))), 3);
	}

	void PixelRoutine::linearToSRGB16_16(Registers &r, Vector4i &c)
	{
		c.x = As<UShort4>(c.x) >> 4;
		c.y = As<UShort4>(c.y) >> 4;
		c.z = As<UShort4>(c.z) >> 4;

		linearToSRGB12_16(r, c);
	}

	void PixelRoutine::linearToSRGB12_16(Registers &r, Vector4i &c)
	{
		Pointer<Byte> LUT = r.constants + OFFSET(Constants,linToSRGB12_16);

		c.x = Insert(c.x, *Pointer<Short>(LUT + 2 * Int(Extract(c.x, 0))), 0);
		c.x = Insert(c.x, *Pointer<Short>(LUT + 2 * Int(Extract(c.x, 1))), 1);
		c.x = Insert(c.x, *Pointer<Short>(LUT + 2 * Int(Extract(c.x, 2))), 2);
		c.x = Insert(c.x, *Pointer<Short>(LUT + 2 * Int(Extract(c.x, 3))), 3);

		c.y = Insert(c.y, *Pointer<Short>(LUT + 2 * Int(Extract(c.y, 0))), 0);
		c.y = Insert(c.y, *Pointer<Short>(LUT + 2 * Int(Extract(c.y, 1))), 1);
		c.y = Insert(c.y, *Pointer<Short>(LUT + 2 * Int(Extract(c.y, 2))), 2);
		c.y = Insert(c.y, *Pointer<Short>(LUT + 2 * Int(Extract(c.y, 3))), 3);

		c.z = Insert(c.z, *Pointer<Short>(LUT + 2 * Int(Extract(c.z, 0))), 0);
		c.z = Insert(c.z, *Pointer<Short>(LUT + 2 * Int(Extract(c.z, 1))), 1);
		c.z = Insert(c.z, *Pointer<Short>(LUT + 2 * Int(Extract(c.z, 2))), 2);
		c.z = Insert(c.z, *Pointer<Short>(LUT + 2 * Int(Extract(c.z, 3))), 3);
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

	void PixelRoutine::MOV(Vector4i &dst, Vector4i &src0)
	{
		dst.x = src0.x;
		dst.y = src0.y;
		dst.z = src0.z;
		dst.w = src0.w;
	}

	void PixelRoutine::ADD(Vector4i &dst, Vector4i &src0, Vector4i &src1)
	{
		dst.x = AddSat(src0.x, src1.x);
		dst.y = AddSat(src0.y, src1.y);
		dst.z = AddSat(src0.z, src1.z);
		dst.w = AddSat(src0.w, src1.w);
	}

	void PixelRoutine::SUB(Vector4i &dst, Vector4i &src0, Vector4i &src1)
	{
		dst.x = SubSat(src0.x, src1.x);
		dst.y = SubSat(src0.y, src1.y);
		dst.z = SubSat(src0.z, src1.z);
		dst.w = SubSat(src0.w, src1.w);
	}

	void PixelRoutine::MAD(Vector4i &dst, Vector4i &src0, Vector4i &src1, Vector4i &src2)
	{
		// FIXME: Long fixed-point multiply fixup
		{dst.x = MulHigh(src0.x, src1.x); dst.x = AddSat(dst.x, dst.x); dst.x = AddSat(dst.x, dst.x); dst.x = AddSat(dst.x, dst.x); dst.x = AddSat(dst.x, dst.x); dst.x = AddSat(dst.x, src2.x);}
		{dst.y = MulHigh(src0.y, src1.y); dst.y = AddSat(dst.y, dst.y); dst.y = AddSat(dst.y, dst.y); dst.y = AddSat(dst.y, dst.y); dst.y = AddSat(dst.y, dst.y); dst.y = AddSat(dst.y, src2.y);}
		{dst.z = MulHigh(src0.z, src1.z); dst.z = AddSat(dst.z, dst.z); dst.z = AddSat(dst.z, dst.z); dst.z = AddSat(dst.z, dst.z); dst.z = AddSat(dst.z, dst.z); dst.z = AddSat(dst.z, src2.z);}
		{dst.w = MulHigh(src0.w, src1.w); dst.w = AddSat(dst.w, dst.w); dst.w = AddSat(dst.w, dst.w); dst.w = AddSat(dst.w, dst.w); dst.w = AddSat(dst.w, dst.w); dst.w = AddSat(dst.w, src2.w);}
	}

	void PixelRoutine::MUL(Vector4i &dst, Vector4i &src0, Vector4i &src1)
	{
		// FIXME: Long fixed-point multiply fixup
		{dst.x = MulHigh(src0.x, src1.x); dst.x = AddSat(dst.x, dst.x); dst.x = AddSat(dst.x, dst.x); dst.x = AddSat(dst.x, dst.x); dst.x = AddSat(dst.x, dst.x);}
		{dst.y = MulHigh(src0.y, src1.y); dst.y = AddSat(dst.y, dst.y); dst.y = AddSat(dst.y, dst.y); dst.y = AddSat(dst.y, dst.y); dst.y = AddSat(dst.y, dst.y);}
		{dst.z = MulHigh(src0.z, src1.z); dst.z = AddSat(dst.z, dst.z); dst.z = AddSat(dst.z, dst.z); dst.z = AddSat(dst.z, dst.z); dst.z = AddSat(dst.z, dst.z);}
		{dst.w = MulHigh(src0.w, src1.w); dst.w = AddSat(dst.w, dst.w); dst.w = AddSat(dst.w, dst.w); dst.w = AddSat(dst.w, dst.w); dst.w = AddSat(dst.w, dst.w);}
	}

	void PixelRoutine::DP3(Vector4i &dst, Vector4i &src0, Vector4i &src1)
	{
		Short4 t0;
		Short4 t1;

		// FIXME: Long fixed-point multiply fixup
		t0 = MulHigh(src0.x, src1.x); t0 = AddSat(t0, t0); t0 = AddSat(t0, t0); t0 = AddSat(t0, t0); t0 = AddSat(t0, t0); 
		t1 = MulHigh(src0.y, src1.y); t1 = AddSat(t1, t1); t1 = AddSat(t1, t1); t1 = AddSat(t1, t1); t1 = AddSat(t1, t1); 
		t0 = AddSat(t0, t1);
		t1 = MulHigh(src0.z, src1.z); t1 = AddSat(t1, t1); t1 = AddSat(t1, t1); t1 = AddSat(t1, t1); t1 = AddSat(t1, t1); 
		t0 = AddSat(t0, t1);

		dst.x = t0;
		dst.y = t0;
		dst.z = t0;
		dst.w = t0;
	}

	void PixelRoutine::DP4(Vector4i &dst, Vector4i &src0, Vector4i &src1)
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

		dst.x = t0;
		dst.y = t0;
		dst.z = t0;
		dst.w = t0;
	}

	void PixelRoutine::LRP(Vector4i &dst, Vector4i &src0, Vector4i &src1, Vector4i &src2)
	{
		// FIXME: Long fixed-point multiply fixup
		{dst.x = SubSat(src1.x, src2.x); dst.x = MulHigh(dst.x, src0.x); dst.x = AddSat(dst.x, dst.x); dst.x = AddSat(dst.x, dst.x); dst.x = AddSat(dst.x, dst.x); dst.x = AddSat(dst.x, dst.x); dst.x = AddSat(dst.x, src2.x);}
		{dst.y = SubSat(src1.y, src2.y); dst.y = MulHigh(dst.y, src0.y); dst.y = AddSat(dst.y, dst.y); dst.y = AddSat(dst.y, dst.y); dst.y = AddSat(dst.y, dst.y); dst.y = AddSat(dst.y, dst.y); dst.y = AddSat(dst.y, src2.y);}
		{dst.z = SubSat(src1.z, src2.z); dst.z = MulHigh(dst.z, src0.z); dst.z = AddSat(dst.z, dst.z); dst.z = AddSat(dst.z, dst.z); dst.z = AddSat(dst.z, dst.z); dst.z = AddSat(dst.z, dst.z); dst.z = AddSat(dst.z, src2.z);}
		{dst.w = SubSat(src1.w, src2.w); dst.w = MulHigh(dst.w, src0.w); dst.w = AddSat(dst.w, dst.w); dst.w = AddSat(dst.w, dst.w); dst.w = AddSat(dst.w, dst.w); dst.w = AddSat(dst.w, dst.w); dst.w = AddSat(dst.w, src2.w);}
	}

	void PixelRoutine::TEXCOORD(Vector4i &dst, Float4 &u, Float4 &v, Float4 &s, int coordinate)
	{
		Float4 uw;
		Float4 vw;
		Float4 sw;

		if(state.interpolant[2 + coordinate].component & 0x01)
		{
			uw = Max(u, Float4(0.0f));
			uw = Min(uw, Float4(1.0f));
			dst.x = convertFixed12(uw);
		}
		else
		{
			dst.x = Short4(0x0000, 0x0000, 0x0000, 0x0000);
		}

		if(state.interpolant[2 + coordinate].component & 0x02)
		{
			vw = Max(v, Float4(0.0f));
			vw = Min(vw, Float4(1.0f));
			dst.y = convertFixed12(vw);
		}
		else
		{
			dst.y = Short4(0x0000, 0x0000, 0x0000, 0x0000);
		}

		if(state.interpolant[2 + coordinate].component & 0x04)
		{
			sw = Max(s, Float4(0.0f));
			sw = Min(sw, Float4(1.0f));
			dst.z = convertFixed12(sw);
		}
		else
		{
			dst.z = Short4(0x0000, 0x0000, 0x0000, 0x0000);
		}

		dst.w = Short4(0x1000);
	}

	void PixelRoutine::TEXCRD(Vector4i &dst, Float4 &u, Float4 &v, Float4 &s, int coordinate, bool project)
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
			uw *= Float4(0x1000);
			uw = Max(uw, Float4(-0x8000));
			uw = Min(uw, Float4(0x7FFF));
			dst.x = RoundShort4(uw);
		}
		else
		{
			dst.x = Short4(0x0000);
		}

		if(state.interpolant[2 + coordinate].component & 0x02)
		{
			vw *= Float4(0x1000);
			vw = Max(vw, Float4(-0x8000));
			vw = Min(vw, Float4(0x7FFF));
			dst.y = RoundShort4(vw);
		}
		else
		{
			dst.y = Short4(0x0000, 0x0000, 0x0000, 0x0000);
		}
		
		if(state.interpolant[2 + coordinate].component & 0x04)
		{
			sw *= Float4(0x1000);
			sw = Max(sw, Float4(-0x8000));
			sw = Min(sw, Float4(0x7FFF));
			dst.z = RoundShort4(sw);
		}
		else
		{
			dst.z = Short4(0x0000, 0x0000, 0x0000, 0x0000);
		}
	}

	void PixelRoutine::TEXDP3(Registers &r, Vector4i &dst, Float4 &u, Float4 &v, Float4 &s, Vector4i &src)
	{
		TEXM3X3PAD(r, u, v, s, src, 0, false);

		Short4 t0 = RoundShort4(r.u_ * Float4(0x1000));

		dst.x = t0;
		dst.y = t0;
		dst.z = t0;
		dst.w = t0;
	}

	void PixelRoutine::TEXDP3TEX(Registers &r, Vector4i &dst, Float4 &u, Float4 &v, Float4 &s, int stage, Vector4i &src0)
	{
		TEXM3X3PAD(r, u, v, s, src0, 0, false);

		r.v_ = Float4(0.0f);
		r.w_ = Float4(0.0f);

		sampleTexture(r, dst, stage, r.u_, r.v_, r.w_, r.w_);
	}

	void PixelRoutine::TEXKILL(Int cMask[4], Float4 &u, Float4 &v, Float4 &s)
	{
		Int kill = SignMask(CmpNLT(u, Float4(0.0f))) &
		           SignMask(CmpNLT(v, Float4(0.0f))) &
		           SignMask(CmpNLT(s, Float4(0.0f)));

		for(unsigned int q = 0; q < state.multiSample; q++)
		{
			cMask[q] &= kill;
		}
	}

	void PixelRoutine::TEXKILL(Int cMask[4], Vector4i &src)
	{
		Short4 test = src.x | src.y | src.z;
		Int kill = SignMask(Pack(test, test)) ^ 0x0000000F;

		for(unsigned int q = 0; q < state.multiSample; q++)
		{
			cMask[q] &= kill;
		}
	}

	void PixelRoutine::TEX(Registers &r, Vector4i &dst, Float4 &u, Float4 &v, Float4 &s, int sampler, bool project)
	{
		sampleTexture(r, dst, sampler, u, v, s, s, project);
	}

	void PixelRoutine::TEXLD(Registers &r, Vector4i &dst, Vector4i &src, int sampler, bool project)
	{
		Float4 u = Float4(src.x) * Float4(1.0f / 0x0FFE);
		Float4 v = Float4(src.y) * Float4(1.0f / 0x0FFE);
		Float4 s = Float4(src.z) * Float4(1.0f / 0x0FFE);

		sampleTexture(r, dst, sampler, u, v, s, s, project);
	}

	void PixelRoutine::TEXBEM(Registers &r, Vector4i &dst, Vector4i &src, Float4 &u, Float4 &v, Float4 &s, int stage)
	{
		Float4 du = Float4(src.x) * Float4(1.0f / 0x0FFE);
		Float4 dv = Float4(src.y) * Float4(1.0f / 0x0FFE);

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

	void PixelRoutine::TEXBEML(Registers &r, Vector4i &dst, Vector4i &src, Float4 &u, Float4 &v, Float4 &s, int stage)
	{
		Float4 du = Float4(src.x) * Float4(1.0f / 0x0FFE);
		Float4 dv = Float4(src.y) * Float4(1.0f / 0x0FFE);

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

		L = src.z;
		L = MulHigh(L, *Pointer<Short4>(r.data + OFFSET(DrawData,textureStage[stage].luminanceScale4)));
		L = L << 4;
		L = AddSat(L, *Pointer<Short4>(r.data + OFFSET(DrawData,textureStage[stage].luminanceOffset4)));
		L = Max(L, Short4(0x0000, 0x0000, 0x0000, 0x0000));
		L = Min(L, Short4(0x1000));

		dst.x = MulHigh(dst.x, L); dst.x = dst.x << 4;
		dst.y = MulHigh(dst.y, L); dst.y = dst.y << 4;
		dst.z = MulHigh(dst.z, L); dst.z = dst.z << 4;
	}

	void PixelRoutine::TEXREG2AR(Registers &r, Vector4i &dst, Vector4i &src0, int stage)
	{
		Float4 u = Float4(src0.w) * Float4(1.0f / 0x0FFE);
		Float4 v = Float4(src0.x) * Float4(1.0f / 0x0FFE);
		Float4 s = Float4(src0.z) * Float4(1.0f / 0x0FFE);

		sampleTexture(r, dst, stage, u, v, s, s);
	}

	void PixelRoutine::TEXREG2GB(Registers &r, Vector4i &dst, Vector4i &src0, int stage)
	{
		Float4 u = Float4(src0.y) * Float4(1.0f / 0x0FFE);
		Float4 v = Float4(src0.z) * Float4(1.0f / 0x0FFE);
		Float4 s = v;

		sampleTexture(r, dst, stage, u, v, s, s);
	}

	void PixelRoutine::TEXREG2RGB(Registers &r, Vector4i &dst, Vector4i &src0, int stage)
	{
		Float4 u = Float4(src0.x) * Float4(1.0f / 0x0FFE);
		Float4 v = Float4(src0.y) * Float4(1.0f / 0x0FFE);
		Float4 s = Float4(src0.z) * Float4(1.0f / 0x0FFE);

		sampleTexture(r, dst, stage, u, v, s, s);
	}

	void PixelRoutine::TEXM3X2DEPTH(Registers &r, Vector4i &dst, Float4 &u, Float4 &v, Float4 &s, Vector4i &src, bool signedScaling)
	{
		TEXM3X2PAD(r, u, v, s, src, 1, signedScaling);

		// z / w
		r.u_ *= Rcp_pp(r.v_);   // FIXME: Set result to 1.0 when division by zero

		r.oDepth = r.u_;
	}

	void PixelRoutine::TEXM3X2PAD(Registers &r, Float4 &u, Float4 &v, Float4 &s, Vector4i &src0, int component, bool signedScaling)
	{
		TEXM3X3PAD(r, u, v, s, src0, component, signedScaling);
	}

	void PixelRoutine::TEXM3X2TEX(Registers &r, Vector4i &dst, Float4 &u, Float4 &v, Float4 &s, int stage, Vector4i &src0, bool signedScaling)
	{
		TEXM3X2PAD(r, u, v, s, src0, 1, signedScaling);

		r.w_ = Float4(0.0f);

		sampleTexture(r, dst, stage, r.u_, r.v_, r.w_, r.w_);
	}

	void PixelRoutine::TEXM3X3(Registers &r, Vector4i &dst, Float4 &u, Float4 &v, Float4 &s, Vector4i &src0, bool signedScaling)
	{
		TEXM3X3PAD(r, u, v, s, src0, 2, signedScaling);

		dst.x = RoundShort4(r.u_ * Float4(0x1000));
		dst.y = RoundShort4(r.v_ * Float4(0x1000));
		dst.z = RoundShort4(r.w_ * Float4(0x1000));
		dst.w = Short4(0x1000);
	}

	void PixelRoutine::TEXM3X3PAD(Registers &r, Float4 &u, Float4 &v, Float4 &s, Vector4i &src0, int component, bool signedScaling)
	{
		if(component == 0 || previousScaling != signedScaling)   // FIXME: Other source modifiers?
		{
			r.U = Float4(src0.x);
			r.V = Float4(src0.y);
			r.W = Float4(src0.z);

			previousScaling = signedScaling;
		}

		Float4 x = r.U * u + r.V * v + r.W * s;

		x *= Float4(1.0f / 0x1000);

		switch(component)
		{
		case 0:	r.u_ = x; break;
		case 1:	r.v_ = x; break;
		case 2: r.w_ = x; break;
		default: ASSERT(false);
		}
	}

	void PixelRoutine::TEXM3X3SPEC(Registers &r, Vector4i &dst, Float4 &u, Float4 &v, Float4 &s, int stage, Vector4i &src0, Vector4i &src1)
	{
		TEXM3X3PAD(r, u, v, s, src0, 2, false);

		Float4 E[3];   // Eye vector

		E[0] = Float4(src1.x) * Float4(1.0f / 0x0FFE);
		E[1] = Float4(src1.y) * Float4(1.0f / 0x0FFE);
		E[2] = Float4(src1.z) * Float4(1.0f / 0x0FFE);

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

	void PixelRoutine::TEXM3X3TEX(Registers &r, Vector4i &dst, Float4 &u, Float4 &v, Float4 &s, int stage, Vector4i &src0, bool signedScaling)
	{
		TEXM3X3PAD(r, u, v, s, src0, 2, signedScaling);

		sampleTexture(r, dst, stage, r.u_, r.v_, r.w_, r.w_);
	}

	void PixelRoutine::TEXM3X3VSPEC(Registers &r, Vector4i &dst, Float4 &u, Float4 &v, Float4 &s, int stage, Vector4i &src0)
	{
		TEXM3X3PAD(r, u, v, s, src0, 2, false);

		Float4 E[3];   // Eye vector

		E[0] = r.vf[2 + stage - 2].w;
		E[1] = r.vf[2 + stage - 1].w;
		E[2] = r.vf[2 + stage - 0].w;

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
		r.u_ = Float4(r.ri[5].x);
		r.v_ = Float4(r.ri[5].y);

		// z / w
		r.u_ *= Rcp_pp(r.v_);   // FIXME: Set result to 1.0 when division by zero

		r.oDepth = r.u_;
	}

	void PixelRoutine::CND(Vector4i &dst, Vector4i &src0, Vector4i &src1, Vector4i &src2)
	{
		{Short4 t0; t0 = src0.x; t0 = CmpGT(t0, Short4(0x0800, 0x0800, 0x0800, 0x0800)); Short4 t1; t1 = src1.x; t1 = t1 & t0; t0 = ~t0 & src2.x; t0 = t0 | t1; dst.x = t0;};
		{Short4 t0; t0 = src0.y; t0 = CmpGT(t0, Short4(0x0800, 0x0800, 0x0800, 0x0800)); Short4 t1; t1 = src1.y; t1 = t1 & t0; t0 = ~t0 & src2.y; t0 = t0 | t1; dst.y = t0;};
		{Short4 t0; t0 = src0.z; t0 = CmpGT(t0, Short4(0x0800, 0x0800, 0x0800, 0x0800)); Short4 t1; t1 = src1.z; t1 = t1 & t0; t0 = ~t0 & src2.z; t0 = t0 | t1; dst.z = t0;};
		{Short4 t0; t0 = src0.w; t0 = CmpGT(t0, Short4(0x0800, 0x0800, 0x0800, 0x0800)); Short4 t1; t1 = src1.w; t1 = t1 & t0; t0 = ~t0 & src2.w; t0 = t0 | t1; dst.w = t0;};
	}

	void PixelRoutine::CMP(Vector4i &dst, Vector4i &src0, Vector4i &src1, Vector4i &src2)
	{
		{Short4 t0 = CmpGT(Short4(0x0000, 0x0000, 0x0000, 0x0000), src0.x); Short4 t1; t1 = src2.x; t1 &= t0; t0 = ~t0 & src1.x; t0 |= t1; dst.x = t0;};
		{Short4 t0 = CmpGT(Short4(0x0000, 0x0000, 0x0000, 0x0000), src0.y); Short4 t1; t1 = src2.y; t1 &= t0; t0 = ~t0 & src1.y; t0 |= t1; dst.y = t0;};
		{Short4 t0 = CmpGT(Short4(0x0000, 0x0000, 0x0000, 0x0000), src0.z); Short4 t1; t1 = src2.z; t1 &= t0; t0 = ~t0 & src1.z; t0 |= t1; dst.z = t0;};
		{Short4 t0 = CmpGT(Short4(0x0000, 0x0000, 0x0000, 0x0000), src0.w); Short4 t1; t1 = src2.w; t1 &= t0; t0 = ~t0 & src1.w; t0 |= t1; dst.w = t0;};
	}

	void PixelRoutine::BEM(Registers &r, Vector4i &dst, Vector4i &src0, Vector4i &src1, int stage)
	{
		Short4 t0;
		Short4 t1;

		// dst.x = src0.x + BUMPENVMAT00(stage) * src1.x + BUMPENVMAT10(stage) * src1.y
		t0 = MulHigh(src1.x, *Pointer<Short4>(r.data + OFFSET(DrawData,textureStage[stage].bumpmapMatrix4W[0][0]))); t0 = t0 << 4;   // FIXME: Matrix components range? Overflow hazard.
		t1 = MulHigh(src1.y, *Pointer<Short4>(r.data + OFFSET(DrawData,textureStage[stage].bumpmapMatrix4W[1][0]))); t1 = t1 << 4;   // FIXME: Matrix components range? Overflow hazard.
		t0 = AddSat(t0, t1);
		t0 = AddSat(t0, src0.x);
		dst.x = t0;

		// dst.y = src0.y + BUMPENVMAT01(stage) * src1.x + BUMPENVMAT11(stage) * src1.y
		t0 = MulHigh(src1.x, *Pointer<Short4>(r.data + OFFSET(DrawData,textureStage[stage].bumpmapMatrix4W[0][1]))); t0 = t0 << 4;   // FIXME: Matrix components range? Overflow hazard.
		t1 = MulHigh(src1.y, *Pointer<Short4>(r.data + OFFSET(DrawData,textureStage[stage].bumpmapMatrix4W[1][1]))); t1 = t1 << 4;   // FIXME: Matrix components range? Overflow hazard.
		t0 = AddSat(t0, t1);
		t0 = AddSat(t0, src0.y);
		dst.y = t0;
	}

	void PixelRoutine::M3X2(Registers &r, Vector4f &dst, Vector4f &src0, const Src &src1)
	{
		Vector4f row0 = reg(r, src1, 0);
		Vector4f row1 = reg(r, src1, 1);

		dst.x = dot3(src0, row0);
		dst.y = dot3(src0, row1);
	}

	void PixelRoutine::M3X3(Registers &r, Vector4f &dst, Vector4f &src0, const Src &src1)
	{
		Vector4f row0 = reg(r, src1, 0);
		Vector4f row1 = reg(r, src1, 1);
		Vector4f row2 = reg(r, src1, 2);

		dst.x = dot3(src0, row0);
		dst.y = dot3(src0, row1);
		dst.z = dot3(src0, row2);
	}

	void PixelRoutine::M3X4(Registers &r, Vector4f &dst, Vector4f &src0, const Src &src1)
	{
		Vector4f row0 = reg(r, src1, 0);
		Vector4f row1 = reg(r, src1, 1);
		Vector4f row2 = reg(r, src1, 2);
		Vector4f row3 = reg(r, src1, 3);

		dst.x = dot3(src0, row0);
		dst.y = dot3(src0, row1);
		dst.z = dot3(src0, row2);
		dst.w = dot3(src0, row3);
	}

	void PixelRoutine::M4X3(Registers &r, Vector4f &dst, Vector4f &src0, const Src &src1)
	{
		Vector4f row0 = reg(r, src1, 0);
		Vector4f row1 = reg(r, src1, 1);
		Vector4f row2 = reg(r, src1, 2);

		dst.x = dot4(src0, row0);
		dst.y = dot4(src0, row1);
		dst.z = dot4(src0, row2);
	}

	void PixelRoutine::M4X4(Registers &r, Vector4f &dst, Vector4f &src0, const Src &src1)
	{
		Vector4f row0 = reg(r, src1, 0);
		Vector4f row1 = reg(r, src1, 1);
		Vector4f row2 = reg(r, src1, 2);
		Vector4f row3 = reg(r, src1, 3);

		dst.x = dot4(src0, row0);
		dst.y = dot4(src0, row1);
		dst.z = dot4(src0, row2);
		dst.w = dot4(src0, row3);
	}

	void PixelRoutine::TEXLD(Registers &r, Vector4f &dst, Vector4f &src0, const Src &src1, bool project, bool bias)
	{
		Vector4f tmp;
		sampleTexture(r, tmp, src1, src0.x, src0.y, src0.z, src0.w, src0, src0, project, bias);	

		dst.x = tmp[(src1.swizzle >> 0) & 0x3];
		dst.y = tmp[(src1.swizzle >> 2) & 0x3];
		dst.z = tmp[(src1.swizzle >> 4) & 0x3];
		dst.w = tmp[(src1.swizzle >> 6) & 0x3];
	}
	
	void PixelRoutine::TEXLDD(Registers &r, Vector4f &dst, Vector4f &src0, const Src &src1, Vector4f &src2,  Vector4f &src3, bool project, bool bias)
	{
		Vector4f tmp;
		sampleTexture(r, tmp, src1, src0.x, src0.y, src0.z, src0.w, src2, src3, project, bias, true);

		dst.x = tmp[(src1.swizzle >> 0) & 0x3];
		dst.y = tmp[(src1.swizzle >> 2) & 0x3];
		dst.z = tmp[(src1.swizzle >> 4) & 0x3];
		dst.w = tmp[(src1.swizzle >> 6) & 0x3];
	}
	
	void PixelRoutine::TEXLDL(Registers &r, Vector4f &dst, Vector4f &src0, const Src &src1, bool project, bool bias)
	{
		Vector4f tmp;
		sampleTexture(r, tmp, src1, src0.x, src0.y, src0.z, src0.w, src0, src0, project, bias, false, true);

		dst.x = tmp[(src1.swizzle >> 0) & 0x3];
		dst.y = tmp[(src1.swizzle >> 2) & 0x3];
		dst.z = tmp[(src1.swizzle >> 4) & 0x3];
		dst.w = tmp[(src1.swizzle >> 6) & 0x3];
	}

	void PixelRoutine::TEXKILL(Int cMask[4], Vector4f &src, unsigned char mask)
	{
		Int kill = -1;
		
		if(mask & 0x1) kill &= SignMask(CmpNLT(src.x, Float4(0.0f)));
		if(mask & 0x2) kill &= SignMask(CmpNLT(src.y, Float4(0.0f)));
		if(mask & 0x4) kill &= SignMask(CmpNLT(src.z, Float4(0.0f)));
		if(mask & 0x8) kill &= SignMask(CmpNLT(src.w, Float4(0.0f)));

		// FIXME: Dynamic branching affects TEXKILL?
	//	if(shader->containsDynamicBranching())
	//	{
	//		kill = ~SignMask(enableMask(r));
	//	}

		for(unsigned int q = 0; q < state.multiSample; q++)
		{
			cMask[q] &= kill;
		}

		// FIXME: Branch to end of shader if all killed?
	}

	void PixelRoutine::DISCARD(Registers &r, Int cMask[4], const Shader::Instruction *instruction)
	{
		Int kill = 0;
		
		if(shader->containsDynamicBranching())
		{
			kill = ~SignMask(enableMask(r, instruction));
		}
		
		for(unsigned int q = 0; q < state.multiSample; q++)
		{
			cMask[q] &= kill;
		}

		// FIXME: Branch to end of shader if all killed?
	}

	void PixelRoutine::DFDX(Vector4f &dst, Vector4f &src)
	{
		dst.x = src.x.yyww - src.x.xxzz;
		dst.y = src.y.yyww - src.y.xxzz;
		dst.z = src.z.yyww - src.z.xxzz;
		dst.w = src.w.yyww - src.w.xxzz;
	}

	void PixelRoutine::DFDY(Vector4f &dst, Vector4f &src)
	{
		dst.x = src.x.zwzw - src.x.xyxy;
		dst.y = src.y.zwzw - src.y.xyxy;
		dst.z = src.z.zwzw - src.z.xyxy;
		dst.w = src.w.zwzw - src.w.xyxy;
	}

	void PixelRoutine::FWIDTH(Vector4f &dst, Vector4f &src)
	{
		// abs(dFdx(src)) + abs(dFdy(src));
		dst.x = Abs(src.x.yyww - src.x.xxzz) + Abs(src.x.zwzw - src.x.xyxy);
		dst.y = Abs(src.y.yyww - src.x.xxzz) + Abs(src.y.zwzw - src.y.xyxy);
		dst.z = Abs(src.z.yyww - src.x.xxzz) + Abs(src.z.zwzw - src.z.xyxy);
		dst.w = Abs(src.w.yyww - src.x.xxzz) + Abs(src.w.zwzw - src.w.xyxy);
	}

	void PixelRoutine::BREAK(Registers &r)
	{
		llvm::BasicBlock *deadBlock = Nucleus::createBasicBlock();
		llvm::BasicBlock *endBlock = loopRepEndBlock[loopRepDepth - 1];

		if(breakDepth == 0)
		{
			r.enableIndex = r.enableIndex - breakDepth;
			Nucleus::createBr(endBlock);
		}
		else
		{
			r.enableBreak = r.enableBreak & ~r.enableStack[r.enableIndex];
			Bool allBreak = SignMask(r.enableBreak) == 0x0;

			r.enableIndex = r.enableIndex - breakDepth;
			branch(allBreak, endBlock, deadBlock);
		}

		Nucleus::setInsertBlock(deadBlock);
		r.enableIndex = r.enableIndex + breakDepth;
	}

	void PixelRoutine::BREAKC(Registers &r, Vector4f &src0, Vector4f &src1, Control control)
	{
		Int4 condition;

		switch(control)
		{
		case Shader::CONTROL_GT: condition = CmpNLE(src0.x,  src1.x);	break;
		case Shader::CONTROL_EQ: condition = CmpEQ(src0.x, src1.x);		break;
		case Shader::CONTROL_GE: condition = CmpNLT(src0.x, src1.x);	break;
		case Shader::CONTROL_LT: condition = CmpLT(src0.x,  src1.x);	break;
		case Shader::CONTROL_NE: condition = CmpNEQ(src0.x, src1.x);	break;
		case Shader::CONTROL_LE: condition = CmpLE(src0.x, src1.x);		break;
		default:
			ASSERT(false);
		}

		BREAK(r, condition);
	}

	void PixelRoutine::BREAKP(Registers &r, const Src &predicateRegister)   // FIXME: Factor out parts common with BREAKC
	{
		Int4 condition = As<Int4>(r.p0[predicateRegister.swizzle & 0x3]);

		if(predicateRegister.modifier == Shader::MODIFIER_NOT)
		{
			condition = ~condition;
		}

		BREAK(r, condition);
	}

	void PixelRoutine::BREAK(Registers &r, Int4 &condition)
	{
		condition &= r.enableStack[r.enableIndex];

		llvm::BasicBlock *continueBlock = Nucleus::createBasicBlock();
		llvm::BasicBlock *endBlock = loopRepEndBlock[loopRepDepth - 1];

		r.enableBreak = r.enableBreak & ~condition;
		Bool allBreak = SignMask(r.enableBreak) == 0x0;

		r.enableIndex = r.enableIndex - breakDepth;
		branch(allBreak, endBlock, continueBlock);

		Nucleus::setInsertBlock(continueBlock);
		r.enableIndex = r.enableIndex + breakDepth;
	}

	void PixelRoutine::CONTINUE(Registers &r)
	{
		r.enableContinue = r.enableContinue & ~r.enableStack[r.enableIndex];
	}

	void PixelRoutine::TEST()
	{
		whileTest = true;
	}

	void PixelRoutine::CALL(Registers &r, int labelIndex, int callSiteIndex)
	{
		if(!labelBlock[labelIndex])
		{
			labelBlock[labelIndex] = Nucleus::createBasicBlock();
		}

		if(callRetBlock[labelIndex].size() > 1)
		{
			r.callStack[r.stackIndex++] = UInt(callSiteIndex);
		}

		Int4 restoreLeave = r.enableLeave;

		Nucleus::createBr(labelBlock[labelIndex]);
		Nucleus::setInsertBlock(callRetBlock[labelIndex][callSiteIndex]);

		r.enableLeave = restoreLeave;
	}

	void PixelRoutine::CALLNZ(Registers &r, int labelIndex, int callSiteIndex, const Src &src)
	{
		if(src.type == Shader::PARAMETER_CONSTBOOL)
		{
			CALLNZb(r, labelIndex, callSiteIndex, src);
		}
		else if(src.type == Shader::PARAMETER_PREDICATE)
		{
			CALLNZp(r, labelIndex, callSiteIndex, src);
		}
		else ASSERT(false);
	}

	void PixelRoutine::CALLNZb(Registers &r, int labelIndex, int callSiteIndex, const Src &boolRegister)
	{
		Bool condition = (*Pointer<Byte>(r.data + OFFSET(DrawData,ps.b[boolRegister.index])) != Byte(0));   // FIXME
		
		if(boolRegister.modifier == Shader::MODIFIER_NOT)
		{
			condition = !condition;	
		}

		if(!labelBlock[labelIndex])
		{
			labelBlock[labelIndex] = Nucleus::createBasicBlock();
		}

		if(callRetBlock[labelIndex].size() > 1)
		{
			r.callStack[r.stackIndex++] = UInt(callSiteIndex);
		}

		Int4 restoreLeave = r.enableLeave;

		branch(condition, labelBlock[labelIndex], callRetBlock[labelIndex][callSiteIndex]);
		Nucleus::setInsertBlock(callRetBlock[labelIndex][callSiteIndex]);

		r.enableLeave = restoreLeave;
	}

	void PixelRoutine::CALLNZp(Registers &r, int labelIndex, int callSiteIndex, const Src &predicateRegister)
	{
		Int4 condition = As<Int4>(r.p0[predicateRegister.swizzle & 0x3]);

		if(predicateRegister.modifier == Shader::MODIFIER_NOT)
		{
			condition = ~condition;
		}

		condition &= r.enableStack[r.enableIndex];

		if(!labelBlock[labelIndex])
		{
			labelBlock[labelIndex] = Nucleus::createBasicBlock();
		}

		if(callRetBlock[labelIndex].size() > 1)
		{
			r.callStack[r.stackIndex++] = UInt(callSiteIndex);
		}

		r.enableIndex++;
		r.enableStack[r.enableIndex] = condition;
		Int4 restoreLeave = r.enableLeave;

		Bool notAllFalse = SignMask(condition) != 0;
		branch(notAllFalse, labelBlock[labelIndex], callRetBlock[labelIndex][callSiteIndex]);
		Nucleus::setInsertBlock(callRetBlock[labelIndex][callSiteIndex]);

		r.enableIndex--;
		r.enableLeave = restoreLeave;
	}

	void PixelRoutine::ELSE(Registers &r)
	{
		ifDepth--;

		llvm::BasicBlock *falseBlock = ifFalseBlock[ifDepth];
		llvm::BasicBlock *endBlock = Nucleus::createBasicBlock();

		if(isConditionalIf[ifDepth])
		{
			Int4 condition = ~r.enableStack[r.enableIndex] & r.enableStack[r.enableIndex - 1];
			Bool notAllFalse = SignMask(condition) != 0;

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

	void PixelRoutine::ENDWHILE(Registers &r)
	{
		loopRepDepth--;

		llvm::BasicBlock *testBlock = loopRepTestBlock[loopRepDepth];
		llvm::BasicBlock *endBlock = loopRepEndBlock[loopRepDepth];

		Nucleus::createBr(testBlock);
		Nucleus::setInsertBlock(endBlock);

		r.enableIndex--;
		r.enableBreak = Int4(0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF);
		whileTest = false;
	}

	void PixelRoutine::IF(Registers &r, const Src &src)
	{
		if(src.type == Shader::PARAMETER_CONSTBOOL)
		{
			IFb(r, src);
		}
		else if(src.type == Shader::PARAMETER_PREDICATE)
		{
			IFp(r, src);
		}
		else
		{
			Int4 condition = As<Int4>(reg(r, src).x);
			IF(r, condition);
		}
	}

	void PixelRoutine::IFb(Registers &r, const Src &boolRegister)
	{
		ASSERT(ifDepth < 24 + 4);

		Bool condition = (*Pointer<Byte>(r.data + OFFSET(DrawData,ps.b[boolRegister.index])) != Byte(0));   // FIXME

		if(boolRegister.modifier == Shader::MODIFIER_NOT)
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

	void PixelRoutine::IFp(Registers &r, const Src &predicateRegister)
	{
		Int4 condition = As<Int4>(r.p0[predicateRegister.swizzle & 0x3]);

		if(predicateRegister.modifier == Shader::MODIFIER_NOT)
		{
			condition = ~condition;
		}

		IF(r, condition);
	}

	void PixelRoutine::IFC(Registers &r, Vector4f &src0, Vector4f &src1, Control control)
	{
		Int4 condition;

		switch(control)
		{
		case Shader::CONTROL_GT: condition = CmpNLE(src0.x,  src1.x);	break;
		case Shader::CONTROL_EQ: condition = CmpEQ(src0.x, src1.x);		break;
		case Shader::CONTROL_GE: condition = CmpNLT(src0.x, src1.x);	break;
		case Shader::CONTROL_LT: condition = CmpLT(src0.x,  src1.x);	break;
		case Shader::CONTROL_NE: condition = CmpNEQ(src0.x, src1.x);	break;
		case Shader::CONTROL_LE: condition = CmpLE(src0.x, src1.x);		break;
		default:
			ASSERT(false);
		}

		IF(r, condition);
	}

	void PixelRoutine::IF(Registers &r, Int4 &condition)
	{
		condition &= r.enableStack[r.enableIndex];

		r.enableIndex++;
		r.enableStack[r.enableIndex] = condition;

		llvm::BasicBlock *trueBlock = Nucleus::createBasicBlock();
		llvm::BasicBlock *falseBlock = Nucleus::createBasicBlock();

		Bool notAllFalse = SignMask(condition) != 0;

		branch(notAllFalse, trueBlock, falseBlock);

		isConditionalIf[ifDepth] = true;
		ifFalseBlock[ifDepth] = falseBlock;

		ifDepth++;
		breakDepth++;
	}

	void PixelRoutine::LABEL(int labelIndex)
	{
		if(!labelBlock[labelIndex])
		{
			labelBlock[labelIndex] = Nucleus::createBasicBlock();
		}

		Nucleus::setInsertBlock(labelBlock[labelIndex]);
		currentLabel = labelIndex;
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

	void PixelRoutine::WHILE(Registers &r, const Src &temporaryRegister)
	{
		r.enableIndex++;

		llvm::BasicBlock *loopBlock = Nucleus::createBasicBlock();
		llvm::BasicBlock *testBlock = Nucleus::createBasicBlock();
		llvm::BasicBlock *endBlock = Nucleus::createBasicBlock();
		
		loopRepTestBlock[loopRepDepth] = testBlock;
		loopRepEndBlock[loopRepDepth] = endBlock;

		Int4 restoreBreak = r.enableBreak;
		Int4 restoreContinue = r.enableContinue;

		// FIXME: jump(testBlock)
		Nucleus::createBr(testBlock);
		Nucleus::setInsertBlock(testBlock);
		r.enableContinue = restoreContinue;

		const Vector4f &src = reg(r, temporaryRegister);
		Int4 condition = As<Int4>(src.x);
		condition &= r.enableStack[r.enableIndex - 1];
		r.enableStack[r.enableIndex] = condition;

		Bool notAllFalse = SignMask(condition) != 0;
		branch(notAllFalse, loopBlock, endBlock);
		
		Nucleus::setInsertBlock(endBlock);
		r.enableBreak = restoreBreak;
		
		Nucleus::setInsertBlock(loopBlock);

		loopRepDepth++;
		breakDepth = 0;
	}

	void PixelRoutine::RET(Registers &r)
	{
		if(currentLabel == -1)
		{
			returnBlock = Nucleus::createBasicBlock();
			Nucleus::createBr(returnBlock);
		}
		else
		{
			llvm::BasicBlock *unreachableBlock = Nucleus::createBasicBlock();

			if(callRetBlock[currentLabel].size() > 1)   // Pop the return destination from the call stack
			{
				// FIXME: Encapsulate
				UInt index = r.callStack[--r.stackIndex];
 
				llvm::Value *value = index.loadValue();
				llvm::Value *switchInst = Nucleus::createSwitch(value, unreachableBlock, (int)callRetBlock[currentLabel].size());

				for(unsigned int i = 0; i < callRetBlock[currentLabel].size(); i++)
				{
					Nucleus::addSwitchCase(switchInst, i, callRetBlock[currentLabel][i]);
				}
			}
			else if(callRetBlock[currentLabel].size() == 1)   // Jump directly to the unique return destination
			{
				Nucleus::createBr(callRetBlock[currentLabel][0]);
			}
			else   // Function isn't called
			{
				Nucleus::createBr(unreachableBlock);
			}

			Nucleus::setInsertBlock(unreachableBlock);
			Nucleus::createUnreachable();
		}
	}

	void PixelRoutine::LEAVE(Registers &r)
	{
		r.enableLeave = r.enableLeave & ~r.enableStack[r.enableIndex];

		// FIXME: Return from function if all instances left
		// FIXME: Use enableLeave in other control-flow constructs
	}
	
	void PixelRoutine::writeDestination(Registers &r, Vector4i &d, const Dst &dst)
	{
		switch(dst.type)
		{
		case Shader::PARAMETER_TEMP:
			if(dst.mask & 0x1) r.ri[dst.index].x = d.x;
			if(dst.mask & 0x2) r.ri[dst.index].y = d.y;
			if(dst.mask & 0x4) r.ri[dst.index].z = d.z;
			if(dst.mask & 0x8) r.ri[dst.index].w = d.w;
			break;
		case Shader::PARAMETER_INPUT:
			if(dst.mask & 0x1) r.vi[dst.index].x = d.x;
			if(dst.mask & 0x2) r.vi[dst.index].y = d.y;
			if(dst.mask & 0x4) r.vi[dst.index].z = d.z;
			if(dst.mask & 0x8) r.vi[dst.index].w = d.w;
			break;
		case Shader::PARAMETER_CONST:			ASSERT(false);	break;
		case Shader::PARAMETER_TEXTURE:
			if(dst.mask & 0x1) r.ti[dst.index].x = d.x;
			if(dst.mask & 0x2) r.ti[dst.index].y = d.y;
			if(dst.mask & 0x4) r.ti[dst.index].z = d.z;
			if(dst.mask & 0x8) r.ti[dst.index].w = d.w;
			break;
		case Shader::PARAMETER_COLOROUT:
			if(dst.mask & 0x1) r.vi[dst.index].x = d.x;
			if(dst.mask & 0x2) r.vi[dst.index].y = d.y;
			if(dst.mask & 0x4) r.vi[dst.index].z = d.z;
			if(dst.mask & 0x8) r.vi[dst.index].w = d.w;
			break;
		default:
			ASSERT(false);
		}
	}

	Vector4i PixelRoutine::regi(Registers &r, const Src &src)
	{
		Vector4i *reg;
		int i = src.index;

		Vector4i c;

		if(src.type == Shader::PARAMETER_CONST)
		{
			c.x = *Pointer<Short4>(r.data + OFFSET(DrawData,ps.cW[i][0]));
			c.y = *Pointer<Short4>(r.data + OFFSET(DrawData,ps.cW[i][1]));
			c.z = *Pointer<Short4>(r.data + OFFSET(DrawData,ps.cW[i][2]));
			c.w = *Pointer<Short4>(r.data + OFFSET(DrawData,ps.cW[i][3]));
		}

		switch(src.type)
		{
		case Shader::PARAMETER_TEMP:          reg = &r.ri[i]; break;
		case Shader::PARAMETER_INPUT:         reg = &r.vi[i]; break;
		case Shader::PARAMETER_CONST:         reg = &c;       break;
		case Shader::PARAMETER_TEXTURE:       reg = &r.ti[i]; break;
		case Shader::PARAMETER_VOID:          return r.ri[0]; // Dummy
		case Shader::PARAMETER_FLOAT4LITERAL: return r.ri[0]; // Dummy
		default:
			ASSERT(false);
		}

		const Short4 &x = (*reg)[(src.swizzle >> 0) & 0x3];
		const Short4 &y = (*reg)[(src.swizzle >> 2) & 0x3];
		const Short4 &z = (*reg)[(src.swizzle >> 4) & 0x3];
		const Short4 &w = (*reg)[(src.swizzle >> 6) & 0x3];

		Vector4i mod;

		switch(src.modifier)
		{
		case Shader::MODIFIER_NONE:
			mod.x = x;
			mod.y = y;
			mod.z = z;
			mod.w = w;
			break;
		case Shader::MODIFIER_BIAS:
			mod.x = SubSat(x, Short4(0x0800, 0x0800, 0x0800, 0x0800));
			mod.y = SubSat(y, Short4(0x0800, 0x0800, 0x0800, 0x0800));
			mod.z = SubSat(z, Short4(0x0800, 0x0800, 0x0800, 0x0800));
			mod.w = SubSat(w, Short4(0x0800, 0x0800, 0x0800, 0x0800));
			break;
		case Shader::MODIFIER_BIAS_NEGATE:
			mod.x = SubSat(Short4(0x0800, 0x0800, 0x0800, 0x0800), x);
			mod.y = SubSat(Short4(0x0800, 0x0800, 0x0800, 0x0800), y);
			mod.z = SubSat(Short4(0x0800, 0x0800, 0x0800, 0x0800), z);
			mod.w = SubSat(Short4(0x0800, 0x0800, 0x0800, 0x0800), w);
			break;
		case Shader::MODIFIER_COMPLEMENT:
			mod.x = SubSat(Short4(0x1000), x);
			mod.y = SubSat(Short4(0x1000), y);
			mod.z = SubSat(Short4(0x1000), z);
			mod.w = SubSat(Short4(0x1000), w);
			break;
		case Shader::MODIFIER_NEGATE:
			mod.x = -x;
			mod.y = -y;
			mod.z = -z;
			mod.w = -w;
			break;
		case Shader::MODIFIER_X2:
			mod.x = AddSat(x, x);
			mod.y = AddSat(y, y);
			mod.z = AddSat(z, z);
			mod.w = AddSat(w, w);
			break;
		case Shader::MODIFIER_X2_NEGATE:
			mod.x = -AddSat(x, x);
			mod.y = -AddSat(y, y);
			mod.z = -AddSat(z, z);
			mod.w = -AddSat(w, w);
			break;
		case Shader::MODIFIER_SIGN:
			mod.x = SubSat(x, Short4(0x0800, 0x0800, 0x0800, 0x0800));
			mod.y = SubSat(y, Short4(0x0800, 0x0800, 0x0800, 0x0800));
			mod.z = SubSat(z, Short4(0x0800, 0x0800, 0x0800, 0x0800));
			mod.w = SubSat(w, Short4(0x0800, 0x0800, 0x0800, 0x0800));
			mod.x = AddSat(mod.x, mod.x);
			mod.y = AddSat(mod.y, mod.y);
			mod.z = AddSat(mod.z, mod.z);
			mod.w = AddSat(mod.w, mod.w);
			break;
		case Shader::MODIFIER_SIGN_NEGATE:
			mod.x = SubSat(Short4(0x0800, 0x0800, 0x0800, 0x0800), x);
			mod.y = SubSat(Short4(0x0800, 0x0800, 0x0800, 0x0800), y);
			mod.z = SubSat(Short4(0x0800, 0x0800, 0x0800, 0x0800), z);
			mod.w = SubSat(Short4(0x0800, 0x0800, 0x0800, 0x0800), w);
			mod.x = AddSat(mod.x, mod.x);
			mod.y = AddSat(mod.y, mod.y);
			mod.z = AddSat(mod.z, mod.z);
			mod.w = AddSat(mod.w, mod.w);
			break;
		case Shader::MODIFIER_DZ:
			mod.x = x;
			mod.y = y;
			mod.z = z;
			mod.w = w;
			// Projection performed by texture sampler
			break;
		case Shader::MODIFIER_DW:
			mod.x = x;
			mod.y = y;
			mod.z = z;
			mod.w = w;
			// Projection performed by texture sampler
			break;
		default:
			ASSERT(false);
		}

		if(src.type == Shader::PARAMETER_CONST && (src.modifier == Shader::MODIFIER_X2 || src.modifier == Shader::MODIFIER_X2_NEGATE))
		{
			mod.x = Min(mod.x, Short4(0x1000)); mod.x = Max(mod.x, Short4(-0x1000, -0x1000, -0x1000, -0x1000));
			mod.y = Min(mod.y, Short4(0x1000)); mod.y = Max(mod.y, Short4(-0x1000, -0x1000, -0x1000, -0x1000));
			mod.z = Min(mod.z, Short4(0x1000)); mod.z = Max(mod.z, Short4(-0x1000, -0x1000, -0x1000, -0x1000));
			mod.w = Min(mod.w, Short4(0x1000)); mod.w = Max(mod.w, Short4(-0x1000, -0x1000, -0x1000, -0x1000));
		}

		return mod;
	}

	Vector4f PixelRoutine::reg(Registers &r, const Src &src, int offset)
	{
		Vector4f reg;
		int i = src.index + offset;

		switch(src.type)
		{
		case Shader::PARAMETER_TEMP:
			if(src.rel.type == Shader::PARAMETER_VOID)
			{
				reg = r.rf[i];
			}
			else
			{
				Int a = relativeAddress(r, src);

				reg = r.rf[i + a];
			}
			break;
		case Shader::PARAMETER_INPUT:
			{
				if(src.rel.type == Shader::PARAMETER_VOID)   // Not relative
				{
					reg = r.vf[i];
				}
				else if(src.rel.type == Shader::PARAMETER_LOOP)
				{
					Int aL = r.aL[r.loopDepth];

					reg = r.vf[i + aL];
				}
				else
				{
					Int a = relativeAddress(r, src);
					
					reg = r.vf[i + a];
				}
			}
			break;
		case Shader::PARAMETER_CONST:
			reg = readConstant(r, src, offset);
			break;
		case Shader::PARAMETER_TEXTURE:
			reg = r.vf[2 + i];
			break;
		case Shader::PARAMETER_MISCTYPE:
			if(src.index == 0)				reg = r.vPos;
			if(src.index == 1)				reg = r.vFace;
			break;
		case Shader::PARAMETER_SAMPLER:
			if(src.rel.type == Shader::PARAMETER_VOID)
			{
				reg.x = As<Float4>(Int4(i));
			}
			else if(src.rel.type == Shader::PARAMETER_TEMP)
			{
				reg.x = As<Float4>(Int4(i) + RoundInt(r.rf[src.rel.index].x));
			}
			return reg;
		case Shader::PARAMETER_PREDICATE:	return reg;   // Dummy
		case Shader::PARAMETER_VOID:		return reg;   // Dummy
		case Shader::PARAMETER_FLOAT4LITERAL:
			reg.x = Float4(src.value[0]);
			reg.y = Float4(src.value[1]);
			reg.z = Float4(src.value[2]);
			reg.w = Float4(src.value[3]);
			break;
		case Shader::PARAMETER_CONSTINT:	return reg;   // Dummy
		case Shader::PARAMETER_CONSTBOOL:	return reg;   // Dummy
		case Shader::PARAMETER_LOOP:		return reg;   // Dummy
		case Shader::PARAMETER_COLOROUT:
			reg = r.oC[i];
			break;
		case Shader::PARAMETER_DEPTHOUT:
			reg.x = r.oDepth;
			break;
		default:
			ASSERT(false);
		}

		const Float4 &x = reg[(src.swizzle >> 0) & 0x3];
		const Float4 &y = reg[(src.swizzle >> 2) & 0x3];
		const Float4 &z = reg[(src.swizzle >> 4) & 0x3];
		const Float4 &w = reg[(src.swizzle >> 6) & 0x3];

		Vector4f mod;

		switch(src.modifier)
		{
		case Shader::MODIFIER_NONE:
			mod.x = x;
			mod.y = y;
			mod.z = z;
			mod.w = w;
			break;
		case Shader::MODIFIER_NEGATE:
			mod.x = -x;
			mod.y = -y;
			mod.z = -z;
			mod.w = -w;
			break;
		case Shader::MODIFIER_ABS:
			mod.x = Abs(x);
			mod.y = Abs(y);
			mod.z = Abs(z);
			mod.w = Abs(w);
			break;
		case Shader::MODIFIER_ABS_NEGATE:
			mod.x = -Abs(x);
			mod.y = -Abs(y);
			mod.z = -Abs(z);
			mod.w = -Abs(w);
			break;
		case Shader::MODIFIER_NOT:
			mod.x = As<Float4>(As<Int4>(x) ^ Int4(0xFFFFFFFF));
			mod.y = As<Float4>(As<Int4>(y) ^ Int4(0xFFFFFFFF));
			mod.z = As<Float4>(As<Int4>(z) ^ Int4(0xFFFFFFFF));
			mod.w = As<Float4>(As<Int4>(w) ^ Int4(0xFFFFFFFF));
			break;
		default:
			ASSERT(false);
		}

		return mod;
	}

	Vector4f PixelRoutine::readConstant(Registers &r, const Src &src, int offset)
	{
		Vector4f c;

		int i = src.index + offset;

		if(src.rel.type == Shader::PARAMETER_VOID)   // Not relative
		{
			c.x = c.y = c.z = c.w = *Pointer<Float4>(r.data + OFFSET(DrawData,ps.c[i]));

			c.x = c.x.xxxx;
			c.y = c.y.yyyy;
			c.z = c.z.zzzz;
			c.w = c.w.wwww;

			if(localShaderConstants)   // Constant may be known at compile time
			{
				for(int j = 0; j < shader->getLength(); j++)
				{
					const Shader::Instruction &instruction = *shader->getInstruction(j);

					if(instruction.opcode == Shader::OPCODE_DEF)
					{
						if(instruction.dst.index == i)
						{
							c.x = Float4(instruction.src[0].value[0]);
							c.y = Float4(instruction.src[0].value[1]);
							c.z = Float4(instruction.src[0].value[2]);
							c.w = Float4(instruction.src[0].value[3]);

							break;
						}
					}
				}
			}
		}
		else if(src.rel.type == Shader::PARAMETER_LOOP)
		{
			Int loopCounter = r.aL[r.loopDepth];

			c.x = c.y = c.z = c.w = *Pointer<Float4>(r.data + OFFSET(DrawData,ps.c[i]) + loopCounter * 16);

			c.x = c.x.xxxx;
			c.y = c.y.yyyy;
			c.z = c.z.zzzz;
			c.w = c.w.wwww;
		}
		else
		{
			Int a = relativeAddress(r, src);
			
			c.x = c.y = c.z = c.w = *Pointer<Float4>(r.data + OFFSET(DrawData,ps.c[i]) + a * 16);

			c.x = c.x.xxxx;
			c.y = c.y.yyyy;
			c.z = c.z.zzzz;
			c.w = c.w.wwww;
		}

		return c;
	}

	Int PixelRoutine::relativeAddress(Registers &r, const Shader::Parameter &var)
	{
		ASSERT(var.rel.deterministic);

		if(var.rel.type == Shader::PARAMETER_TEMP)
		{
			return RoundInt(Extract(r.rf[var.rel.index].x, 0)) * var.rel.scale;
		}
		else if(var.rel.type == Shader::PARAMETER_INPUT)
		{
			return RoundInt(Extract(r.vf[var.rel.index].x, 0)) * var.rel.scale;
		}
		else if(var.rel.type == Shader::PARAMETER_OUTPUT)
		{
			return RoundInt(Extract(r.oC[var.rel.index].x, 0)) * var.rel.scale;
		}
		else if(var.rel.type == Shader::PARAMETER_CONST)
		{
			RValue<Float4> c = *Pointer<Float4>(r.data + OFFSET(DrawData,ps.c[var.rel.index]));

			return RoundInt(Extract(c, 0)) * var.rel.scale;
		}
		else ASSERT(false);

		return 0;
	}

	Int4 PixelRoutine::enableMask(Registers &r, const Shader::Instruction *instruction)
	{
		Int4 enable = instruction->analysisBranch ? Int4(r.enableStack[r.enableIndex]) : Int4(0xFFFFFFFF);
		
		if(!whileTest)
		{
			if(shader->containsBreakInstruction() && instruction->analysisBreak)
			{
				enable &= r.enableBreak;
			}

			if(shader->containsContinueInstruction() && instruction->analysisContinue)
			{
				enable &= r.enableContinue;
			}

			if(shader->containsLeaveInstruction() && instruction->analysisLeave)
			{
				enable &= r.enableLeave;
			}
		}

		return enable;
	}

	bool PixelRoutine::colorUsed()
	{
		return state.colorWriteMask || state.alphaTestActive() || state.shaderContainsKill;
	}

	unsigned short PixelRoutine::shaderVersion() const
	{
		return shader ? shader->getVersion() : 0x0000;
	}

	bool PixelRoutine::interpolateZ() const
	{
		return state.depthTestActive || state.pixelFogActive() || (shader && shader->vPosDeclared && fullPixelPositionRegister);
	}

	bool PixelRoutine::interpolateW() const
	{
		return state.perspective || (shader && shader->vPosDeclared && fullPixelPositionRegister);
	}
}
