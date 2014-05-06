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

#include "SamplerCore.hpp"

#include "Constants.hpp"
#include "Debug.hpp"

namespace sw
{
	SamplerCore::SamplerCore(Pointer<Byte> &constants, const Sampler::State &state) : constants(constants), state(state)
	{
	}

	void SamplerCore::sampleTexture(Pointer<Byte> &texture, Color4i &c, Float4 &u, Float4 &v, Float4 &w, Float4 &q, Color4f &dsx, Color4f &dsy, bool bias, bool fixed12, bool gradients, bool lodProvided)
	{
		#if PERF_PROFILE
			AddAtomic(Pointer<Long>(&profiler.texOperations), Long(4));

			if(state.compressedFormat)
			{
				AddAtomic(Pointer<Long>(&profiler.compressedTex), Long(4));
			}
		#endif

		bool cubeTexture = state.textureType == TEXTURE_CUBE;
		bool volumeTexture = state.textureType == TEXTURE_3D;

		Float4 uuuu = u;
		Float4 vvvv = v;
		Float4 wwww = w;

		if(state.textureType == TEXTURE_NULL)
		{
			c.r = Short4(0x0000, 0x0000, 0x0000, 0x0000);
			c.g = Short4(0x0000, 0x0000, 0x0000, 0x0000);
			c.b = Short4(0x0000, 0x0000, 0x0000, 0x0000);

			if(fixed12)   // FIXME: Convert to fixed12 at higher level, when required
			{
				c.a = Short4(0x1000, 0x1000, 0x1000, 0x1000);
			}
			else
			{
				c.a = Short4((short)0xFFFF, (short)0xFFFF, (short)0xFFFF, (short)0xFFFF);   // FIXME
			}
		}
		else
		{
			Int face[4];
			Float4 lodU;
			Float4 lodV;

			if(cubeTexture)
			{
				cubeFace(face, uuuu, vvvv, lodU, lodV, u, v, w);
			}

			Float lod;
			Float anisotropy;
			Float4 uDelta;
			Float4 vDelta;

			if(!volumeTexture)
			{
				if(!cubeTexture)
				{
					computeLod(texture, lod, anisotropy, uDelta, vDelta, uuuu, vvvv, Float(q.x), dsx, dsy, bias, gradients, lodProvided);
				}
				else
				{
					computeLod(texture, lod, anisotropy, uDelta, vDelta, lodU, lodV, Float(q.x), dsx, dsy, bias, gradients, lodProvided);
				}
			}
			else
			{
				computeLod3D(texture, lod, uuuu, vvvv, wwww, Float(q.x), dsx, dsy, bias, gradients, lodProvided);
			}

			if(cubeTexture)
			{
				uuuu += Float4(0.5f, 0.5f, 0.5f, 0.5f);
				vvvv += Float4(0.5f, 0.5f, 0.5f, 0.5f);
			}

			if(!hasFloatTexture())
			{
				sampleFilter(texture, c, uuuu, vvvv, wwww, lod, anisotropy, uDelta, vDelta, face, lodProvided);
			}
			else
			{
				Color4f cf;

				sampleFloatFilter(texture, cf, uuuu, vvvv, wwww, lod, anisotropy, uDelta, vDelta, face, lodProvided);

				convertFixed12(c, cf);
			}

			if(fixed12 && !hasFloatTexture())
			{
				for(int component = 0; component < textureComponentCount(); component++)
				{
					if(state.sRGB && isRGBComponent(component))
					{
						sRGBtoLinear16_12(c[component]);   // FIXME: Perform linearization at surface level for read-only textures
					}
					else
					{
						if(hasUnsignedTextureComponent(component))
						{
							c[component] = As<UShort4>(c[component]) >> 4;
						}
						else
						{
							c[component] = c[component] >> 3;
						}
					}
				}
			}

			if(fixed12 && state.textureFilter != FILTER_GATHER)
			{
				int componentCount = textureComponentCount();

				switch(state.textureFormat)
				{
				case FORMAT_R8:
				case FORMAT_X8R8G8B8:
				case FORMAT_A8R8G8B8:
				case FORMAT_V8U8:
				case FORMAT_Q8W8V8U8:
				case FORMAT_X8L8V8U8:
				case FORMAT_V16U16:
				case FORMAT_A16W16V16U16:
				case FORMAT_Q16W16V16U16:
				case FORMAT_G8R8:
				case FORMAT_G16R16:
				case FORMAT_A16B16G16R16:
					if(componentCount < 2) c.g = Short4(0x1000, 0x1000, 0x1000, 0x1000);
					if(componentCount < 3) c.b = Short4(0x1000, 0x1000, 0x1000, 0x1000);
					if(componentCount < 4) c.a = Short4(0x1000, 0x1000, 0x1000, 0x1000);
					break;
				case FORMAT_A8:
					c.a = c.r;
					c.r = Short4(0x0000, 0x0000, 0x0000, 0x0000);
					c.g = Short4(0x0000, 0x0000, 0x0000, 0x0000);
					c.b = Short4(0x0000, 0x0000, 0x0000, 0x0000);
					break;
				case FORMAT_L8:
				case FORMAT_L16:
					c.g = c.r;
					c.b = c.r;
					c.a = Short4(0x1000, 0x1000, 0x1000, 0x1000);
					break;
				case FORMAT_A8L8:
					c.a = c.g;
					c.g = c.r;
					c.b = c.r;
					break;
				case FORMAT_R32F:
					c.g = Short4(0x1000, 0x1000, 0x1000, 0x1000);
				case FORMAT_G32R32F:
					c.b = Short4(0x1000, 0x1000, 0x1000, 0x1000);
					c.a = Short4(0x1000, 0x1000, 0x1000, 0x1000);
				case FORMAT_A32B32G32R32F:
					break;
				case FORMAT_D32F_LOCKABLE:
				case FORMAT_D32F_TEXTURE:
				case FORMAT_D32F_SHADOW:
					c.g = c.r;
					c.b = c.r;
					c.a = c.r;
					break;
				default:
					ASSERT(false);
				}
			}
		}
	}

	void SamplerCore::sampleTexture(Pointer<Byte> &texture, Color4f &c, Float4 &u, Float4 &v, Float4 &w, Float4 &q, Color4f &dsx, Color4f &dsy, bool bias, bool gradients, bool lodProvided)
	{
		#if PERF_PROFILE
			AddAtomic(Pointer<Long>(&profiler.texOperations), Long(4));

			if(state.compressedFormat)
			{
				AddAtomic(Pointer<Long>(&profiler.compressedTex), Long(4));
			}
		#endif

		bool cubeTexture = state.textureType == TEXTURE_CUBE;
		bool volumeTexture = state.textureType == TEXTURE_3D;

		if(state.textureType == TEXTURE_NULL)
		{
			c.r = Float4(0.0f, 0.0f, 0.0f, 0.0f);
			c.g = Float4(0.0f, 0.0f, 0.0f, 0.0f);
			c.b = Float4(0.0f, 0.0f, 0.0f, 0.0f);
			c.a = Float4(1.0f, 1.0f, 1.0f, 1.0f);
		}
		else
		{
			if(hasFloatTexture())   // FIXME: Mostly identical to integer sampling
			{
				Float4 uuuu = u;
				Float4 vvvv = v;
				Float4 wwww = w;

				Int face[4];
				Float4 lodU;
				Float4 lodV;

				if(cubeTexture)
				{
					cubeFace(face, uuuu, vvvv, lodU, lodV, u, v, w);
				}

				Float lod;
				Float anisotropy;
				Float4 uDelta;
				Float4 vDelta;

				if(!volumeTexture)
				{
					if(!cubeTexture)
					{
						computeLod(texture, lod, anisotropy, uDelta, vDelta, uuuu, vvvv, Float(q.x), dsx, dsy, bias, gradients, lodProvided);
					}
					else
					{
						computeLod(texture, lod, anisotropy, uDelta, vDelta, lodU, lodV, Float(q.x), dsx, dsy, bias, gradients, lodProvided);
					}
				}
				else
				{
					computeLod3D(texture, lod, uuuu, vvvv, wwww, Float(q.x), dsx, dsy, bias, gradients, lodProvided);
				}

				if(cubeTexture)
				{
					uuuu += Float4(0.5f, 0.5f, 0.5f, 0.5f);
					vvvv += Float4(0.5f, 0.5f, 0.5f, 0.5f);
				}

				sampleFloatFilter(texture, c, uuuu, vvvv, wwww, lod, anisotropy, uDelta, vDelta, face, lodProvided);
			}
			else
			{
				Color4i ci;

				sampleTexture(texture, ci, u, v, w, q, dsx, dsy, bias, false, gradients, lodProvided);

				for(int component = 0; component < textureComponentCount(); component++)
				{
					if(state.sRGB && isRGBComponent(component))
					{
						sRGBtoLinear16_12(ci[component]);   // FIXME: Perform linearization at surface level for read-only textures
						convertSigned12(c[component], ci[component]);
					}
					else
					{
						if(hasUnsignedTextureComponent(component))
						{
							convertUnsigned16(c[component], ci[component]);
						}
						else
						{
							convertSigned15(c[component], ci[component]);
						}
					}
				}
			}

			int componentCount = textureComponentCount();

			if(state.textureFilter != FILTER_GATHER)
			{
				switch(state.textureFormat)
				{
				case FORMAT_R8:
				case FORMAT_X8R8G8B8:
				case FORMAT_A8R8G8B8:
				case FORMAT_V8U8:
				case FORMAT_Q8W8V8U8:
				case FORMAT_X8L8V8U8:
				case FORMAT_V16U16:
				case FORMAT_A16W16V16U16:
				case FORMAT_Q16W16V16U16:
				case FORMAT_G8R8:
				case FORMAT_G16R16:
				case FORMAT_A16B16G16R16:
					if(componentCount < 2) c.g = Float4(1.0f, 1.0f, 1.0f, 1.0f);
					if(componentCount < 3) c.b = Float4(1.0f, 1.0f, 1.0f, 1.0f);
					if(componentCount < 4) c.a = Float4(1.0f, 1.0f, 1.0f, 1.0f);
					break;
				case FORMAT_A8:
					c.a = c.r;
					c.r = Float4(0.0f, 0.0f, 0.0f, 0.0f);
					c.g = Float4(0.0f, 0.0f, 0.0f, 0.0f);
					c.b = Float4(0.0f, 0.0f, 0.0f, 0.0f);
					break;
				case FORMAT_L8:
				case FORMAT_L16:
					c.g = c.r;
					c.b = c.r;
					c.a = Float4(1.0f, 1.0f, 1.0f, 1.0f);
					break;
				case FORMAT_A8L8:
					c.a = c.g;
					c.g = c.r;
					c.b = c.r;
					break;
				case FORMAT_R32F:
					c.g = Float4(1.0f, 1.0f, 1.0f, 1.0f);
				case FORMAT_G32R32F:
					c.b = Float4(1.0f, 1.0f, 1.0f, 1.0f);
					c.a = Float4(1.0f, 1.0f, 1.0f, 1.0f);
				case FORMAT_A32B32G32R32F:
					break;
				case FORMAT_D32F_LOCKABLE:
				case FORMAT_D32F_TEXTURE:
				case FORMAT_D32F_SHADOW:
					c.g = c.r;
					c.b = c.r;
					c.a = c.r;
					break;
				default:
					ASSERT(false);
				}
			}
		}
	}

	void SamplerCore::border(Short4 &mask, Float4 &coordinates)
	{
		Int4 border = As<Int4>(CmpLT(Abs(coordinates - Float4(0.5f)), Float4(0.5f)));
		mask = As<Short4>(Int2(As<Int4>(Pack(border, border))));
	}

	void SamplerCore::border(Int4 &mask, Float4 &coordinates)
	{
		mask = As<Int4>(CmpLT(Abs(coordinates - Float4(0.5f)), Float4(0.5f)));
	}

	Short4 SamplerCore::offsetSample(Short4 &uvw, Pointer<Byte> &mipmap, int halfOffset, bool wrap, int count)
	{
		if(wrap)
		{
			switch(count)
			{
			case -1: return uvw - *Pointer<Short4>(mipmap + halfOffset);
			case  0: return uvw;
			case +1: return uvw + *Pointer<Short4>(mipmap + halfOffset);
			case  2: return uvw + *Pointer<Short4>(mipmap + halfOffset) + *Pointer<Short4>(mipmap + halfOffset);
			}
		}
		else   // Clamp or mirror
		{
			switch(count)
			{
			case -1: return SubSat(As<UShort4>(uvw), *Pointer<UShort4>(mipmap + halfOffset));
			case  0: return uvw;
			case +1: return AddSat(As<UShort4>(uvw), *Pointer<UShort4>(mipmap + halfOffset));
			case  2: return AddSat(AddSat(As<UShort4>(uvw), *Pointer<UShort4>(mipmap + halfOffset)), *Pointer<UShort4>(mipmap + halfOffset));
			}
		}

		return uvw;
	}

	void SamplerCore::sampleFilter(Pointer<Byte> &texture, Color4i &c, Float4 &u, Float4 &v, Float4 &w, Float &lod, Float &anisotropy, Float4 &uDelta, Float4 &vDelta, Int face[4], bool lodProvided)
	{
		bool volumeTexture = state.textureType == TEXTURE_3D;

		sampleAniso(texture, c, u, v, w, lod, anisotropy, uDelta, vDelta, face, false, lodProvided);

		if(state.mipmapFilter > MIPMAP_POINT)
		{
			Color4i cc;

			sampleAniso(texture, cc, u, v, w, lod, anisotropy, uDelta, vDelta, face, true, lodProvided);

			lod *= Float(1 << 16);

			UShort4 utri = UShort4(Float4(lod));   // FIXME: Optimize
			Short4 stri = utri >> 1;   // FIXME: Optimize

			if(hasUnsignedTextureComponent(0)) cc.r = MulHigh(As<UShort4>(cc.r), utri); else cc.r = MulHigh(cc.r, stri);
			if(hasUnsignedTextureComponent(1)) cc.g = MulHigh(As<UShort4>(cc.g), utri); else cc.g = MulHigh(cc.g, stri);
			if(hasUnsignedTextureComponent(2)) cc.b = MulHigh(As<UShort4>(cc.b), utri); else cc.b = MulHigh(cc.b, stri);
			if(hasUnsignedTextureComponent(3)) cc.a = MulHigh(As<UShort4>(cc.a), utri); else cc.a = MulHigh(cc.a, stri);

			utri = ~utri;
			stri = Short4(0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF) - stri;

			if(hasUnsignedTextureComponent(0)) c.r = MulHigh(As<UShort4>(c.r), utri); else c.r = MulHigh(c.r, stri);
			if(hasUnsignedTextureComponent(1)) c.g = MulHigh(As<UShort4>(c.g), utri); else c.g = MulHigh(c.g, stri);
			if(hasUnsignedTextureComponent(2)) c.b = MulHigh(As<UShort4>(c.b), utri); else c.b = MulHigh(c.b, stri);
			if(hasUnsignedTextureComponent(3)) c.a = MulHigh(As<UShort4>(c.a), utri); else c.a = MulHigh(c.a, stri);
			
			c.r += cc.r;
			c.g += cc.g;
			c.b += cc.b;
			c.a += cc.a;
			
			if(!hasUnsignedTextureComponent(0)) c.r += c.r;
			if(!hasUnsignedTextureComponent(1)) c.g += c.g;
			if(!hasUnsignedTextureComponent(2)) c.b += c.b;
			if(!hasUnsignedTextureComponent(3)) c.a += c.a;
		}

		Short4 borderMask;

		if((AddressingMode)state.addressingModeU == ADDRESSING_BORDER)
		{
			Short4 u0;
			
			border(u0, u);

			borderMask = u0;
		}

		if((AddressingMode)state.addressingModeV == ADDRESSING_BORDER)
		{
			Short4 v0;
			
			border(v0, v);

			if((AddressingMode)state.addressingModeU == ADDRESSING_BORDER)
			{
				borderMask &= v0;
			}
			else
			{
				borderMask = v0;
			}
		}

		if((AddressingMode)state.addressingModeW == ADDRESSING_BORDER && volumeTexture)
		{
			Short4 s0;

			border(s0, w);

			if((AddressingMode)state.addressingModeU == ADDRESSING_BORDER ||
			   (AddressingMode)state.addressingModeV == ADDRESSING_BORDER)
			{
				borderMask &= s0;
			}
			else
			{
				borderMask = s0;
			}
		}

		if((AddressingMode)state.addressingModeU == ADDRESSING_BORDER ||
		   (AddressingMode)state.addressingModeV == ADDRESSING_BORDER ||
		   ((AddressingMode)state.addressingModeW == ADDRESSING_BORDER && volumeTexture))
		{
			Short4 b;

			c.r = borderMask & c.r | ~borderMask & (*Pointer<Short4>(texture + OFFSET(Texture,borderColor4[0])) >> (hasUnsignedTextureComponent(0) ? 0 : 1));
			c.g = borderMask & c.g | ~borderMask & (*Pointer<Short4>(texture + OFFSET(Texture,borderColor4[1])) >> (hasUnsignedTextureComponent(1) ? 0 : 1));
			c.b = borderMask & c.b | ~borderMask & (*Pointer<Short4>(texture + OFFSET(Texture,borderColor4[2])) >> (hasUnsignedTextureComponent(2) ? 0 : 1));
			c.a = borderMask & c.a | ~borderMask & (*Pointer<Short4>(texture + OFFSET(Texture,borderColor4[3])) >> (hasUnsignedTextureComponent(3) ? 0 : 1));
		}
	}

	void SamplerCore::sampleAniso(Pointer<Byte> &texture, Color4i &c, Float4 &u, Float4 &v, Float4 &w, Float &lod, Float &anisotropy, Float4 &uDelta, Float4 &vDelta, Int face[4], bool secondLOD, bool lodProvided)
	{
		if(state.textureFilter != FILTER_ANISOTROPIC || lodProvided)
		{
			sampleQuad(texture, c, u, v, w, lod, face, secondLOD);
		}
		else
		{
			Int a = RoundInt(anisotropy);

			Color4i cSum;

			cSum.r = Short4(0, 0, 0, 0);
			cSum.g = Short4(0, 0, 0, 0);
			cSum.b = Short4(0, 0, 0, 0);
			cSum.a = Short4(0, 0, 0, 0);

			Float4 A = *Pointer<Float4>(constants + OFFSET(Constants,uvWeight) + 16 * a);
			Float4 B = *Pointer<Float4>(constants + OFFSET(Constants,uvStart) + 16 * a);
			UShort4 cw = *Pointer<UShort4>(constants + OFFSET(Constants,cWeight) + 8 * a);
			Short4 sw = Short4(cw >> 1);

			Float4 du = uDelta;
			Float4 dv = vDelta;

			Float4 u0 = u + B * du;
			Float4 v0 = v + B * dv;

			du *= A;
			dv *= A;

			Int i = 0;

			Do
			{
				sampleQuad(texture, c, u0, v0, w, lod, face, secondLOD);

				u0 += du;
				v0 += dv;

				if(hasUnsignedTextureComponent(0)) cSum.r += As<Short4>(MulHigh(As<UShort4>(c.r), cw)); else cSum.r += MulHigh(c.r, sw);
				if(hasUnsignedTextureComponent(1)) cSum.g += As<Short4>(MulHigh(As<UShort4>(c.g), cw)); else cSum.g += MulHigh(c.g, sw);
				if(hasUnsignedTextureComponent(2)) cSum.b += As<Short4>(MulHigh(As<UShort4>(c.b), cw)); else cSum.b += MulHigh(c.b, sw);
				if(hasUnsignedTextureComponent(3)) cSum.a += As<Short4>(MulHigh(As<UShort4>(c.a), cw)); else cSum.a += MulHigh(c.a, sw);

				i++;
			}
			Until(i >= a)

			if(hasUnsignedTextureComponent(0)) c.r = cSum.r; else c.r = AddSat(cSum.r, cSum.r);
			if(hasUnsignedTextureComponent(1)) c.g = cSum.g; else c.g = AddSat(cSum.g, cSum.g);
			if(hasUnsignedTextureComponent(2)) c.b = cSum.b; else c.b = AddSat(cSum.b, cSum.b);
			if(hasUnsignedTextureComponent(3)) c.a = cSum.a; else c.a = AddSat(cSum.a, cSum.a);
		}
	}

	void SamplerCore::sampleQuad(Pointer<Byte> &texture, Color4i &c, Float4 &u, Float4 &v, Float4 &w, Float &lod, Int face[4], bool secondLOD)
	{
		if(state.textureType != TEXTURE_3D)
		{
			sampleQuad2D(texture, c, u, v, lod, face, secondLOD);
		}
		else
		{
			sample3D(texture, c, u, v, w, lod, secondLOD);
		}
	}

	void SamplerCore::sampleQuad2D(Pointer<Byte> &texture, Color4i &c, Float4 &u, Float4 &v, Float &lod, Int face[4], bool secondLOD)
	{
		int componentCount = textureComponentCount();
		bool gather = state.textureFilter == FILTER_GATHER;

		Pointer<Byte> mipmap;
		Pointer<Byte> buffer[4];

		selectMipmap(texture, buffer, mipmap, lod, face, secondLOD);

		Short4 uuuu;
		Short4 vvvv;

		address(uuuu, u, (AddressingMode)state.addressingModeU);
		address(vvvv, v, (AddressingMode)state.addressingModeV);

		if(state.textureFilter == FILTER_POINT)
		{
			sampleTexel(c, uuuu, vvvv, vvvv, mipmap, buffer);
		}
		else
		{
			Color4i c0;
			Color4i c1;
			Color4i c2;
			Color4i c3;

			Short4 uuuu0 = offsetSample(uuuu, mipmap, OFFSET(Mipmap,uHalf), (AddressingMode)state.addressingModeU == ADDRESSING_WRAP, gather ? 0 : -1);
			Short4 vvvv0 = offsetSample(vvvv, mipmap, OFFSET(Mipmap,vHalf), (AddressingMode)state.addressingModeV == ADDRESSING_WRAP, gather ? 0 : -1);
			Short4 uuuu1 = offsetSample(uuuu, mipmap, OFFSET(Mipmap,uHalf), (AddressingMode)state.addressingModeU == ADDRESSING_WRAP, gather ? 2 : +1);
			Short4 vvvv1 = offsetSample(vvvv, mipmap, OFFSET(Mipmap,vHalf), (AddressingMode)state.addressingModeV == ADDRESSING_WRAP, gather ? 2 : +1);

			sampleTexel(c0, uuuu0, vvvv0, vvvv0, mipmap, buffer);
			sampleTexel(c1, uuuu1, vvvv0, vvvv0, mipmap, buffer);
			sampleTexel(c2, uuuu0, vvvv1, vvvv1, mipmap, buffer);
			sampleTexel(c3, uuuu1, vvvv1, vvvv1, mipmap, buffer);
	
			if(!gather)   // Blend
			{
				// Fractions
				UShort4 f0u = uuuu0;
				UShort4 f0v = vvvv0;
			
				if(!state.hasNPOTTexture)
				{
					f0u = f0u << *Pointer<Long1>(mipmap + OFFSET(Mipmap,uInt));   // .u
					f0v = f0v << *Pointer<Long1>(mipmap + OFFSET(Mipmap,vInt));   // .v
				}
				else
				{
					f0u = f0u * *Pointer<UShort4>(mipmap + OFFSET(Mipmap,width));
					f0v = f0v * *Pointer<UShort4>(mipmap + OFFSET(Mipmap,height));
				}

				Short4 f1u = ~f0u;
				Short4 f1v = ~f0v;
			
				Short4 f0u0v = MulHigh(As<UShort4>(f0u), As<UShort4>(f0v));
				Short4 f1u0v = MulHigh(As<UShort4>(f1u), As<UShort4>(f0v));
				Short4 f0u1v = MulHigh(As<UShort4>(f0u), As<UShort4>(f1v));
				Short4 f1u1v = MulHigh(As<UShort4>(f1u), As<UShort4>(f1v));

				// Signed fractions
				Short4 f1u1vs;
				Short4 f0u1vs;
				Short4 f1u0vs;
				Short4 f0u0vs;

				if(!hasUnsignedTextureComponent(0) || !hasUnsignedTextureComponent(1) || !hasUnsignedTextureComponent(2) || !hasUnsignedTextureComponent(3))
				{
					f1u1vs = As<UShort4>(f1u1v) >> 1;
					f0u1vs = As<UShort4>(f0u1v) >> 1;
					f1u0vs = As<UShort4>(f1u0v) >> 1;
					f0u0vs = As<UShort4>(f0u0v) >> 1;
				}

				// Bilinear interpolation
				if(componentCount >= 1)
				{
					if(has16bitTexture() && hasUnsignedTextureComponent(0))
					{
						c0.r = As<UShort4>(c0.r) - MulHigh(As<UShort4>(c0.r), f0u) + MulHigh(As<UShort4>(c1.r), f0u);
						c2.r = As<UShort4>(c2.r) - MulHigh(As<UShort4>(c2.r), f0u) + MulHigh(As<UShort4>(c3.r), f0u);
						c.r  = As<UShort4>(c0.r) - MulHigh(As<UShort4>(c0.r), f0v) + MulHigh(As<UShort4>(c2.r), f0v);
					}
					else
					{
						if(hasUnsignedTextureComponent(0))
						{
							c0.r = MulHigh(As<UShort4>(c0.r), As<UShort4>(f1u1v));
							c1.r = MulHigh(As<UShort4>(c1.r), As<UShort4>(f0u1v));
							c2.r = MulHigh(As<UShort4>(c2.r), As<UShort4>(f1u0v));
							c3.r = MulHigh(As<UShort4>(c3.r), As<UShort4>(f0u0v));
						}
						else
						{
							c0.r = MulHigh(c0.r, f1u1vs);
							c1.r = MulHigh(c1.r, f0u1vs);
							c2.r = MulHigh(c2.r, f1u0vs);
							c3.r = MulHigh(c3.r, f0u0vs);
						}

						c.r = (c0.r + c1.r) + (c2.r + c3.r);
						if(!hasUnsignedTextureComponent(0)) c.r = AddSat(c.r, c.r);   // Correct for signed fractions
					}
				}

				if(componentCount >= 2)
				{
					if(has16bitTexture() && hasUnsignedTextureComponent(1))
					{
						c0.g = As<UShort4>(c0.g) - MulHigh(As<UShort4>(c0.g), f0u) + MulHigh(As<UShort4>(c1.g), f0u);
						c2.g = As<UShort4>(c2.g) - MulHigh(As<UShort4>(c2.g), f0u) + MulHigh(As<UShort4>(c3.g), f0u);
						c.g  = As<UShort4>(c0.g) - MulHigh(As<UShort4>(c0.g), f0v) + MulHigh(As<UShort4>(c2.g), f0v);
					}
					else
					{
						if(hasUnsignedTextureComponent(1))
						{
							c0.g = MulHigh(As<UShort4>(c0.g), As<UShort4>(f1u1v));
							c1.g = MulHigh(As<UShort4>(c1.g), As<UShort4>(f0u1v));
							c2.g = MulHigh(As<UShort4>(c2.g), As<UShort4>(f1u0v));
							c3.g = MulHigh(As<UShort4>(c3.g), As<UShort4>(f0u0v));
						}
						else
						{
							c0.g = MulHigh(c0.g, f1u1vs);
							c1.g = MulHigh(c1.g, f0u1vs);
							c2.g = MulHigh(c2.g, f1u0vs);
							c3.g = MulHigh(c3.g, f0u0vs);
						}

						c.g = (c0.g + c1.g) + (c2.g + c3.g);
						if(!hasUnsignedTextureComponent(1)) c.g = AddSat(c.g, c.g);   // Correct for signed fractions
					}
				}

				if(componentCount >= 3)
				{
					if(has16bitTexture() && hasUnsignedTextureComponent(2))
					{
						c0.b = As<UShort4>(c0.b) - MulHigh(As<UShort4>(c0.b), f0u) + MulHigh(As<UShort4>(c1.b), f0u);
						c2.b = As<UShort4>(c2.b) - MulHigh(As<UShort4>(c2.b), f0u) + MulHigh(As<UShort4>(c3.b), f0u);
						c.b  = As<UShort4>(c0.b) - MulHigh(As<UShort4>(c0.b), f0v) + MulHigh(As<UShort4>(c2.b), f0v);
					}
					else
					{
						if(hasUnsignedTextureComponent(2))
						{
							c0.b = MulHigh(As<UShort4>(c0.b), As<UShort4>(f1u1v));
							c1.b = MulHigh(As<UShort4>(c1.b), As<UShort4>(f0u1v));
							c2.b = MulHigh(As<UShort4>(c2.b), As<UShort4>(f1u0v));
							c3.b = MulHigh(As<UShort4>(c3.b), As<UShort4>(f0u0v));
						}
						else
						{
							c0.b = MulHigh(c0.b, f1u1vs);
							c1.b = MulHigh(c1.b, f0u1vs);
							c2.b = MulHigh(c2.b, f1u0vs);
							c3.b = MulHigh(c3.b, f0u0vs);
						}

						c.b = (c0.b + c1.b) + (c2.b + c3.b);
						if(!hasUnsignedTextureComponent(2)) c.b = AddSat(c.b, c.b);   // Correct for signed fractions
					}
				}

				if(componentCount >= 4)
				{
					if(has16bitTexture() && hasUnsignedTextureComponent(3))
					{
						c0.a = As<UShort4>(c0.a) - MulHigh(As<UShort4>(c0.a), f0u) + MulHigh(As<UShort4>(c1.a), f0u);
						c2.a = As<UShort4>(c2.a) - MulHigh(As<UShort4>(c2.a), f0u) + MulHigh(As<UShort4>(c3.a), f0u);
						c.a  = As<UShort4>(c0.a) - MulHigh(As<UShort4>(c0.a), f0v) + MulHigh(As<UShort4>(c2.a), f0v);
					}
					else
					{
						if(hasUnsignedTextureComponent(3))
						{
							c0.a = MulHigh(As<UShort4>(c0.a), As<UShort4>(f1u1v));
							c1.a = MulHigh(As<UShort4>(c1.a), As<UShort4>(f0u1v));
							c2.a = MulHigh(As<UShort4>(c2.a), As<UShort4>(f1u0v));
							c3.a = MulHigh(As<UShort4>(c3.a), As<UShort4>(f0u0v));
						}
						else
						{
							c0.a = MulHigh(c0.a, f1u1vs);
							c1.a = MulHigh(c1.a, f0u1vs);
							c2.a = MulHigh(c2.a, f1u0vs);
							c3.a = MulHigh(c3.a, f0u0vs);
						}

						c.a = (c0.a + c1.a) + (c2.a + c3.a);
						if(!hasUnsignedTextureComponent(3)) c.a = AddSat(c.a, c.a);   // Correct for signed fractions
					}
				}
			}
			else
			{
				c.r = c1.r;
				c.g = c2.r;
				c.b = c3.r;
				c.a = c0.r;
			}
		}
	}

	void SamplerCore::sample3D(Pointer<Byte> &texture, Color4i &c_, Float4 &u_, Float4 &v_, Float4 &w_, Float &lod, bool secondLOD)
	{
		int componentCount = textureComponentCount();

		Pointer<Byte> mipmap;
		Pointer<Byte> buffer[4];
		Int face[4];
		
		selectMipmap(texture, buffer, mipmap, lod, face, secondLOD);

		Short4 uuuu;
		Short4 vvvv;
		Short4 wwww;

		address(uuuu, u_, (AddressingMode)state.addressingModeU);
		address(vvvv, v_, (AddressingMode)state.addressingModeV);
		address(wwww, w_, (AddressingMode)state.addressingModeW);

		if(state.textureFilter <= FILTER_POINT)
		{
			sampleTexel(c_, uuuu, vvvv, wwww, mipmap, buffer);
		}
		else
		{
			Color4i c[2][2][2];

			Short4 u[2][2][2];
			Short4 v[2][2][2];
			Short4 s[2][2][2];

			for(int i = 0; i < 2; i++)
			{
				for(int j = 0; j < 2; j++)
				{
					for(int k = 0; k < 2; k++)
					{
						u[i][j][k] = offsetSample(uuuu, mipmap, OFFSET(Mipmap,uHalf), (AddressingMode)state.addressingModeU == ADDRESSING_WRAP, i * 2 - 1);
						v[i][j][k] = offsetSample(vvvv, mipmap, OFFSET(Mipmap,vHalf), (AddressingMode)state.addressingModeV == ADDRESSING_WRAP, j * 2 - 1);
						s[i][j][k] = offsetSample(wwww, mipmap, OFFSET(Mipmap,wHalf), (AddressingMode)state.addressingModeW == ADDRESSING_WRAP, k * 2 - 1);
					}
				}
			}

			Short4 f[2][2][2];
			Short4 fs[2][2][2];
			Short4 f0u;
			Short4 f0v;
			Short4 f0s;
			Short4 f1u;
			Short4 f1v;
			Short4 f1s;

			// Fractions
			f0u = u[0][0][0];
			f0v = v[0][0][0];
			f0s = s[0][0][0];

			if(!state.hasNPOTTexture)
			{
				f0u = f0u << *Pointer<Long1>(mipmap + OFFSET(Mipmap,uInt));
				f0v = f0v << *Pointer<Long1>(mipmap + OFFSET(Mipmap,vInt));
				f0s = f0s << *Pointer<Long1>(mipmap + OFFSET(Mipmap,wInt));
			}
			else
			{
				f0u *= *Pointer<Short4>(mipmap + OFFSET(Mipmap,width));
				f0v *= *Pointer<Short4>(mipmap + OFFSET(Mipmap,height));
				f0s *= *Pointer<Short4>(mipmap + OFFSET(Mipmap,depth));
			}

			f1u = ~f0u;
			f1v = ~f0v;
			f1s = ~f0s;

			f[1][1][1] = MulHigh(As<UShort4>(f1u), As<UShort4>(f1v));
			f[0][1][1] = MulHigh(As<UShort4>(f0u), As<UShort4>(f1v));
			f[1][0][1] = MulHigh(As<UShort4>(f1u), As<UShort4>(f0v));
			f[0][0][1] = MulHigh(As<UShort4>(f0u), As<UShort4>(f0v));
			f[1][1][0] = MulHigh(As<UShort4>(f1u), As<UShort4>(f1v));
			f[0][1][0] = MulHigh(As<UShort4>(f0u), As<UShort4>(f1v));
			f[1][0][0] = MulHigh(As<UShort4>(f1u), As<UShort4>(f0v));
			f[0][0][0] = MulHigh(As<UShort4>(f0u), As<UShort4>(f0v));

			f[1][1][1] = MulHigh(As<UShort4>(f[1][1][1]), As<UShort4>(f1s));
			f[0][1][1] = MulHigh(As<UShort4>(f[0][1][1]), As<UShort4>(f1s));
			f[1][0][1] = MulHigh(As<UShort4>(f[1][0][1]), As<UShort4>(f1s));
			f[0][0][1] = MulHigh(As<UShort4>(f[0][0][1]), As<UShort4>(f1s));
			f[1][1][0] = MulHigh(As<UShort4>(f[1][1][0]), As<UShort4>(f0s));
			f[0][1][0] = MulHigh(As<UShort4>(f[0][1][0]), As<UShort4>(f0s));
			f[1][0][0] = MulHigh(As<UShort4>(f[1][0][0]), As<UShort4>(f0s));
			f[0][0][0] = MulHigh(As<UShort4>(f[0][0][0]), As<UShort4>(f0s));

			// Signed fractions
			if(!hasUnsignedTextureComponent(0) || !hasUnsignedTextureComponent(1) || !hasUnsignedTextureComponent(2) || !hasUnsignedTextureComponent(3))
			{
				fs[0][0][0] = As<UShort4>(f[0][0][0]) >> 1;
				fs[0][0][1] = As<UShort4>(f[0][0][1]) >> 1;
				fs[0][1][0] = As<UShort4>(f[0][1][0]) >> 1;
				fs[0][1][1] = As<UShort4>(f[0][1][1]) >> 1;
				fs[1][0][0] = As<UShort4>(f[1][0][0]) >> 1;
				fs[1][0][1] = As<UShort4>(f[1][0][1]) >> 1;
				fs[1][1][0] = As<UShort4>(f[1][1][0]) >> 1;
				fs[1][1][1] = As<UShort4>(f[1][1][1]) >> 1;
			}

			for(int i = 0; i < 2; i++)
			{
				for(int j = 0; j < 2; j++)
				{
					for(int k = 0; k < 2; k++)
					{
						sampleTexel(c[i][j][k], u[i][j][k], v[i][j][k], s[i][j][k], mipmap, buffer);

						if(componentCount >= 1) if(hasUnsignedTextureComponent(0)) c[i][j][k].r = MulHigh(As<UShort4>(c[i][j][k].r), As<UShort4>(f[1 - i][1 - j][1 - k])); else c[i][j][k].r = MulHigh(c[i][j][k].r, fs[1 - i][1 - j][1 - k]);
						if(componentCount >= 2) if(hasUnsignedTextureComponent(1)) c[i][j][k].g = MulHigh(As<UShort4>(c[i][j][k].g), As<UShort4>(f[1 - i][1 - j][1 - k])); else c[i][j][k].g = MulHigh(c[i][j][k].g, fs[1 - i][1 - j][1 - k]);
						if(componentCount >= 3) if(hasUnsignedTextureComponent(2)) c[i][j][k].b = MulHigh(As<UShort4>(c[i][j][k].b), As<UShort4>(f[1 - i][1 - j][1 - k])); else c[i][j][k].b = MulHigh(c[i][j][k].b, fs[1 - i][1 - j][1 - k]);
						if(componentCount >= 4) if(hasUnsignedTextureComponent(3)) c[i][j][k].a = MulHigh(As<UShort4>(c[i][j][k].a), As<UShort4>(f[1 - i][1 - j][1 - k])); else c[i][j][k].a = MulHigh(c[i][j][k].a, fs[1 - i][1 - j][1 - k]);

						if(i != 0 || j != 0 || k != 0)
						{
							if(componentCount >= 1) c[0][0][0].r += c[i][j][k].r;
							if(componentCount >= 2) c[0][0][0].g += c[i][j][k].g;
							if(componentCount >= 3) c[0][0][0].b += c[i][j][k].b;
							if(componentCount >= 4) c[0][0][0].a += c[i][j][k].a;
						}
					}
				}
			}

			if(componentCount >= 1) c_.r = c[0][0][0].r;
			if(componentCount >= 2) c_.g = c[0][0][0].g;
			if(componentCount >= 3) c_.b = c[0][0][0].b;
			if(componentCount >= 4) c_.a = c[0][0][0].a;

			// Correct for signed fractions
			if(componentCount >= 1) if(!hasUnsignedTextureComponent(0)) c_.r = AddSat(c_.r, c_.r);
			if(componentCount >= 2) if(!hasUnsignedTextureComponent(1)) c_.g = AddSat(c_.g, c_.g);
			if(componentCount >= 3) if(!hasUnsignedTextureComponent(2)) c_.b = AddSat(c_.b, c_.b);
			if(componentCount >= 4) if(!hasUnsignedTextureComponent(3)) c_.a = AddSat(c_.a, c_.a);
		}
	}

	void SamplerCore::sampleFloatFilter(Pointer<Byte> &texture, Color4f &c, Float4 &u, Float4 &v, Float4 &w, Float &lod, Float &anisotropy, Float4 &uDelta, Float4 &vDelta, Int face[4], bool lodProvided)
	{
		bool volumeTexture = state.textureType == TEXTURE_3D;

		sampleFloatAniso(texture, c, u, v, w, lod, anisotropy, uDelta, vDelta, face, false, lodProvided);

		if(state.mipmapFilter > MIPMAP_POINT)
		{
			Color4f cc;

			sampleFloatAniso(texture, cc, u, v, w, lod, anisotropy, uDelta, vDelta, face, true, lodProvided);

			Float4 lod4 = Float4(Fraction(lod));

			c.r = (cc.r - c.r) * lod4 + c.r;
			c.g = (cc.g - c.g) * lod4 + c.g;
			c.b = (cc.b - c.b) * lod4 + c.b;
			c.a = (cc.a - c.a) * lod4 + c.a;
		}

		Int4 borderMask;

		if((AddressingMode)state.addressingModeU == ADDRESSING_BORDER)
		{
			Int4 u0;
			
			border(u0, u);

			borderMask = u0;
		}

		if((AddressingMode)state.addressingModeV == ADDRESSING_BORDER)
		{
			Int4 v0;
			
			border(v0, v);

			if((AddressingMode)state.addressingModeU == ADDRESSING_BORDER)
			{
				borderMask &= v0;
			}
			else
			{
				borderMask = v0;
			}
		}

		if((AddressingMode)state.addressingModeW == ADDRESSING_BORDER && volumeTexture)
		{
			Int4 s0;

			border(s0, w);

			if((AddressingMode)state.addressingModeU == ADDRESSING_BORDER ||
			   (AddressingMode)state.addressingModeV == ADDRESSING_BORDER)
			{
				borderMask &= s0;
			}
			else
			{
				borderMask = s0;
			}
		}

		if((AddressingMode)state.addressingModeU == ADDRESSING_BORDER ||
		   (AddressingMode)state.addressingModeV == ADDRESSING_BORDER ||
		   ((AddressingMode)state.addressingModeW == ADDRESSING_BORDER && volumeTexture))
		{
			Int4 b;

			c.r = As<Float4>(borderMask & As<Int4>(c.r) | ~borderMask & *Pointer<Int4>(texture + OFFSET(Texture,borderColorF[0])));
			c.g = As<Float4>(borderMask & As<Int4>(c.g) | ~borderMask & *Pointer<Int4>(texture + OFFSET(Texture,borderColorF[1])));
			c.b = As<Float4>(borderMask & As<Int4>(c.b) | ~borderMask & *Pointer<Int4>(texture + OFFSET(Texture,borderColorF[2])));
			c.a = As<Float4>(borderMask & As<Int4>(c.a) | ~borderMask & *Pointer<Int4>(texture + OFFSET(Texture,borderColorF[3])));
		}
	}

	void SamplerCore::sampleFloatAniso(Pointer<Byte> &texture, Color4f &c, Float4 &u, Float4 &v, Float4 &w, Float &lod, Float &anisotropy, Float4 &uDelta, Float4 &vDelta, Int face[4], bool secondLOD, bool lodProvided)
	{
		if(state.textureFilter != FILTER_ANISOTROPIC || lodProvided)
		{
			sampleFloat(texture, c, u, v, w, lod, face, secondLOD);
		}
		else
		{
			Int a = RoundInt(anisotropy);

			Color4f cSum;

			cSum.r = Float4(0, 0, 0, 0);
			cSum.g = Float4(0, 0, 0, 0);
			cSum.b = Float4(0, 0, 0, 0);
			cSum.a = Float4(0, 0, 0, 0);

			Float4 A = *Pointer<Float4>(constants + OFFSET(Constants,uvWeight) + 16 * a);
			Float4 B = *Pointer<Float4>(constants + OFFSET(Constants,uvStart) + 16 * a);

			Float4 du = uDelta;
			Float4 dv = vDelta;

			Float4 u0 = u + B * du;
			Float4 v0 = v + B * dv;

			du *= A;
			dv *= A;

			Int i = 0;

			Do
			{
				sampleFloat(texture, c, u0, v0, w, lod, face, secondLOD);

				u0 += du;
				v0 += dv;

				cSum.r += c.r * A;
				cSum.g += c.g * A;
				cSum.b += c.b * A;
				cSum.a += c.a * A;

				i++;
			}
			Until(i >= a)

			c.r = cSum.r;
			c.g = cSum.g;
			c.b = cSum.b;
			c.a = cSum.a;
		}
	}

	void SamplerCore::sampleFloat(Pointer<Byte> &texture, Color4f &c, Float4 &u, Float4 &v, Float4 &w, Float &lod, Int face[4], bool secondLOD)
	{
		if(state.textureType != TEXTURE_3D)
		{
			sampleFloat2D(texture, c, u, v, w, lod, face, secondLOD);
		}
		else
		{
			sampleFloat3D(texture, c, u, v, w, lod, secondLOD);
		}
	}
	
	void SamplerCore::sampleFloat2D(Pointer<Byte> &texture, Color4f &c, Float4 &u, Float4 &v, Float4 &z, Float &lod, Int face[4], bool secondLOD)
	{
		int componentCount = textureComponentCount();
		bool gather = state.textureFilter == FILTER_GATHER;

		Pointer<Byte> mipmap;
		Pointer<Byte> buffer[4];
		
		selectMipmap(texture, buffer, mipmap, lod, face, secondLOD);

		Short4 uuuu;
		Short4 vvvv;

		address(uuuu, u, (AddressingMode)state.addressingModeU);
		address(vvvv, v, (AddressingMode)state.addressingModeV);

		if(state.textureFilter == FILTER_POINT)
		{
			sampleTexel(c, uuuu, vvvv, vvvv, z, mipmap, buffer);
		}
		else
		{
			Color4f c0;
			Color4f c1;
			Color4f c2;
			Color4f c3;

			Short4 uuuu0 = offsetSample(uuuu, mipmap, OFFSET(Mipmap,uHalf), (AddressingMode)state.addressingModeU == ADDRESSING_WRAP, gather ? 0 : -1);
			Short4 vvvv0 = offsetSample(vvvv, mipmap, OFFSET(Mipmap,vHalf), (AddressingMode)state.addressingModeV == ADDRESSING_WRAP, gather ? 0 : -1);
			Short4 uuuu1 = offsetSample(uuuu, mipmap, OFFSET(Mipmap,uHalf), (AddressingMode)state.addressingModeU == ADDRESSING_WRAP, gather ? 2 : +1);
			Short4 vvvv1 = offsetSample(vvvv, mipmap, OFFSET(Mipmap,vHalf), (AddressingMode)state.addressingModeV == ADDRESSING_WRAP, gather ? 2 : +1);

			sampleTexel(c0, uuuu0, vvvv0, vvvv0, z, mipmap, buffer);		
			sampleTexel(c1, uuuu1, vvvv0, vvvv0, z, mipmap, buffer);
			sampleTexel(c2, uuuu0, vvvv1, vvvv1, z, mipmap, buffer);
			sampleTexel(c3, uuuu1, vvvv1, vvvv1, z, mipmap, buffer);

			if(!gather)   // Blend
			{
				// Fractions
				Float4 fu = Fraction(Float4(As<UShort4>(uuuu0)) * *Pointer<Float4>(mipmap + OFFSET(Mipmap,fWidth)));
				Float4 fv = Fraction(Float4(As<UShort4>(vvvv0)) * *Pointer<Float4>(mipmap + OFFSET(Mipmap,fHeight)));

				if(componentCount >= 1) c0.r = c0.r + fu * (c1.r - c0.r);
				if(componentCount >= 2) c0.g = c0.g + fu * (c1.g - c0.g);
				if(componentCount >= 3) c0.b = c0.b + fu * (c1.b - c0.b);
				if(componentCount >= 4) c0.a = c0.a + fu * (c1.a - c0.a);

				if(componentCount >= 1) c2.r = c2.r + fu * (c3.r - c2.r);
				if(componentCount >= 2) c2.g = c2.g + fu * (c3.g - c2.g);
				if(componentCount >= 3) c2.b = c2.b + fu * (c3.b - c2.b);
				if(componentCount >= 4) c2.a = c2.a + fu * (c3.a - c2.a);

				if(componentCount >= 1) c.r = c0.r + fv * (c2.r - c0.r);
				if(componentCount >= 2) c.g = c0.g + fv * (c2.g - c0.g);
				if(componentCount >= 3) c.b = c0.b + fv * (c2.b - c0.b);
				if(componentCount >= 4) c.a = c0.a + fv * (c2.a - c0.a);
			}
			else
			{
				c.r = c1.r;
				c.g = c2.r;
				c.b = c3.r;
				c.a = c0.r;
			}
		}
	}

	void SamplerCore::sampleFloat3D(Pointer<Byte> &texture, Color4f &c, Float4 &u, Float4 &v, Float4 &w, Float &lod, bool secondLOD)
	{
		int componentCount = textureComponentCount();

		Pointer<Byte> mipmap;
		Pointer<Byte> buffer[4];
		Int face[4];

		selectMipmap(texture, buffer, mipmap, lod, face, secondLOD);

		Short4 uuuu;
		Short4 vvvv;
		Short4 wwww;

		address(uuuu, u, (AddressingMode)state.addressingModeU);
		address(vvvv, v, (AddressingMode)state.addressingModeV);
		address(wwww, w, (AddressingMode)state.addressingModeW);

		if(state.textureFilter <= FILTER_POINT)
		{
			sampleTexel(c, uuuu, vvvv, wwww, w, mipmap, buffer);
		}
		else
		{
			Color4f &c0 = c;
			Color4f c1;
			Color4f c2;
			Color4f c3;
			Color4f c4;
			Color4f c5;
			Color4f c6;
			Color4f c7;

			Short4 uuuu0 = offsetSample(uuuu, mipmap, OFFSET(Mipmap,uHalf), (AddressingMode)state.addressingModeU == ADDRESSING_WRAP, -1);
			Short4 vvvv0 = offsetSample(vvvv, mipmap, OFFSET(Mipmap,vHalf), (AddressingMode)state.addressingModeV == ADDRESSING_WRAP, -1);
			Short4 wwww0 = offsetSample(wwww, mipmap, OFFSET(Mipmap,wHalf), (AddressingMode)state.addressingModeW == ADDRESSING_WRAP, -1);
			Short4 uuuu1 = offsetSample(uuuu, mipmap, OFFSET(Mipmap,uHalf), (AddressingMode)state.addressingModeU == ADDRESSING_WRAP, +1);
			Short4 vvvv1 = offsetSample(vvvv, mipmap, OFFSET(Mipmap,vHalf), (AddressingMode)state.addressingModeV == ADDRESSING_WRAP, +1);
			Short4 wwww1 = offsetSample(wwww, mipmap, OFFSET(Mipmap,wHalf), (AddressingMode)state.addressingModeW == ADDRESSING_WRAP, +1);

			sampleTexel(c0, uuuu0, vvvv0, wwww0, w, mipmap, buffer);
			sampleTexel(c1, uuuu1, vvvv0, wwww0, w, mipmap, buffer);
			sampleTexel(c2, uuuu0, vvvv1, wwww0, w, mipmap, buffer);
			sampleTexel(c3, uuuu1, vvvv1, wwww0, w, mipmap, buffer);
			sampleTexel(c4, uuuu0, vvvv0, wwww1, w, mipmap, buffer);
			sampleTexel(c5, uuuu1, vvvv0, wwww1, w, mipmap, buffer);
			sampleTexel(c6, uuuu0, vvvv1, wwww1, w, mipmap, buffer);
			sampleTexel(c7, uuuu1, vvvv1, wwww1, w, mipmap, buffer);

			// Fractions
			Float4 fu = Fraction(Float4(As<UShort4>(uuuu0)) * *Pointer<Float4>(mipmap + OFFSET(Mipmap,fWidth)));
			Float4 fv = Fraction(Float4(As<UShort4>(vvvv0)) * *Pointer<Float4>(mipmap + OFFSET(Mipmap,fHeight)));
			Float4 fw = Fraction(Float4(As<UShort4>(wwww0)) * *Pointer<Float4>(mipmap + OFFSET(Mipmap,fDepth)));

			// Blend first slice
			if(componentCount >= 1) c0.r = c0.r + fu * (c1.r - c0.r);
			if(componentCount >= 2) c0.g = c0.g + fu * (c1.g - c0.g);
			if(componentCount >= 3) c0.b = c0.b + fu * (c1.b - c0.b);
			if(componentCount >= 4) c0.a = c0.a + fu * (c1.a - c0.a);

			if(componentCount >= 1) c2.r = c2.r + fu * (c3.r - c2.r);
			if(componentCount >= 2) c2.g = c2.g + fu * (c3.g - c2.g);
			if(componentCount >= 3) c2.b = c2.b + fu * (c3.b - c2.b);
			if(componentCount >= 4) c2.a = c2.a + fu * (c3.a - c2.a);

			if(componentCount >= 1) c0.r = c0.r + fv * (c2.r - c0.r);
			if(componentCount >= 2) c0.g = c0.g + fv * (c2.g - c0.g);
			if(componentCount >= 3) c0.b = c0.b + fv * (c2.b - c0.b);
			if(componentCount >= 4) c0.a = c0.a + fv * (c2.a - c0.a);

			// Blend second slice
			if(componentCount >= 1) c4.r = c4.r + fu * (c5.r - c4.r);
			if(componentCount >= 2) c4.g = c4.g + fu * (c5.g - c4.g);
			if(componentCount >= 3) c4.b = c4.b + fu * (c5.b - c4.b);
			if(componentCount >= 4) c4.a = c4.a + fu * (c5.a - c4.a);

			if(componentCount >= 1) c6.r = c6.r + fu * (c7.r - c6.r);
			if(componentCount >= 2) c6.g = c6.g + fu * (c7.g - c6.g);
			if(componentCount >= 3) c6.b = c6.b + fu * (c7.b - c6.b);
			if(componentCount >= 4) c6.a = c6.a + fu * (c7.a - c6.a);

			if(componentCount >= 1) c4.r = c4.r + fv * (c6.r - c4.r);
			if(componentCount >= 2) c4.g = c4.g + fv * (c6.g - c4.g);
			if(componentCount >= 3) c4.b = c4.b + fv * (c6.b - c4.b);
			if(componentCount >= 4) c4.a = c4.a + fv * (c6.a - c4.a);

			// Blend slices
			if(componentCount >= 1) c0.r = c0.r + fw * (c4.r - c0.r);
			if(componentCount >= 2) c0.g = c0.g + fw * (c4.g - c0.g);
			if(componentCount >= 3) c0.b = c0.b + fw * (c4.b - c0.b);
			if(componentCount >= 4) c0.a = c0.a + fw * (c4.a - c0.a);
		}
	}

	void SamplerCore::computeLod(Pointer<Byte> &texture, Float &lod, Float &anisotropy, Float4 &uDelta, Float4 &vDelta, Float4 &uuuu, Float4 &vvvv, Float &lodBias, Color4f &dsx, Color4f &dsy, bool bias, bool gradients, bool lodProvided)
	{
		if(!lodProvided)
		{
			Float4 duvdxy;

			if(!gradients)
			{
				duvdxy = Float4(uuuu.yz, vvvv.yz) - Float4(uuuu.xx, vvvv.xx);
			}
			else
			{
				Float4 dudxy = Float4(dsx.x.xx, dsy.x.xx);
				Float4 dvdxy = Float4(dsx.y.xx, dsy.y.xx);
				
				duvdxy = Float4(dudxy.xz, dvdxy.xz);
			}

			// Scale by texture dimensions and LOD
			Float4 dUVdxy = duvdxy * *Pointer<Float4>(texture + OFFSET(Texture,widthHeightLOD));

			Float4 dUV2dxy = dUVdxy * dUVdxy;
			Float4 dUV2 = dUV2dxy.xy + dUV2dxy.zw;

			lod = Max(Float(dUV2.x), Float(dUV2.y));   // Square length of major axis

			if(state.textureFilter == FILTER_ANISOTROPIC)
			{
				Float det = Abs(Float(dUVdxy.x) * Float(dUVdxy.w) - Float(dUVdxy.y) * Float(dUVdxy.z));

				Float4 dudx = duvdxy.xxxx;
				Float4 dudy = duvdxy.yyyy;
				Float4 dvdx = duvdxy.zzzz;
				Float4 dvdy = duvdxy.wwww;

				Int4 mask = As<Int4>(CmpNLT(dUV2.x, dUV2.y));
				uDelta = As<Float4>(As<Int4>(dudx) & mask | As<Int4>(dudy) & ~mask);
				vDelta = As<Float4>(As<Int4>(dvdx) & mask | As<Int4>(dvdy) & ~mask);

				anisotropy = lod * Rcp_pp(det);
				anisotropy = Min(anisotropy, *Pointer<Float>(texture + OFFSET(Texture,maxAnisotropy)));

				lod *= Rcp_pp(anisotropy * anisotropy);
			}

			// log2(sqrt(lod))
			lod = Float(As<Int>(lod));
			lod -= Float(0x3F800000);
			lod *= As<Float>(Int(0x33800000));

			if(bias)
			{
				lod += lodBias;
			}

			// FIXME: Hack to satisfy WHQL
			if(state.textureType == TEXTURE_CUBE)
			{
				lod += Float(-0.15f);
			}
		}
		else
		{
			lod = lodBias + *Pointer<Float>(texture + OFFSET(Texture,LOD));
		}

		lod = Max(lod, Float(0));
		lod = Min(lod, Float(MIPMAP_LEVELS - 2));   // Trilinear accesses lod+1
	}

	void SamplerCore::computeLod3D(Pointer<Byte> &texture, Float &lod, Float4 &uuuu, Float4 &vvvv, Float4 &wwww, Float &lodBias, Color4f &dsx, Color4f &dsy, bool bias, bool gradients, bool lodProvided)
	{
		if(state.mipmapFilter == MIPMAP_NONE)
		{
		}
		else   // Point and linear filter
		{
			if(!lodProvided)
			{
				Float4 dudxy;
				Float4 dvdxy;
				Float4 dsdxy;

				if(!gradients)
				{
					dudxy = uuuu.ywyw - uuuu;
					dvdxy = vvvv.ywyw - vvvv;
					dsdxy = wwww.ywyw - wwww;
				}
				else
				{
					dudxy = dsx.x;
					dvdxy = dsx.y;
					dsdxy = dsx.z;

					dudxy = Float4(dudxy.xx, dsy.x.xx);
					dvdxy = Float4(dvdxy.xx, dsy.y.xx);
					dsdxy = Float4(dsdxy.xx, dsy.z.xx);

					dudxy = Float4(dudxy.xz, dudxy.xz);
					dvdxy = Float4(dvdxy.xz, dvdxy.xz);
					dsdxy = Float4(dsdxy.xz, dsdxy.xz);
				}

				// Scale by texture dimensions and LOD
				dudxy *= *Pointer<Float4>(texture + OFFSET(Texture,widthLOD));
				dvdxy *= *Pointer<Float4>(texture + OFFSET(Texture,heightLOD));
				dsdxy *= *Pointer<Float4>(texture + OFFSET(Texture,depthLOD));

				dudxy *= dudxy;
				dvdxy *= dvdxy;
				dsdxy *= dsdxy;

				dudxy += dvdxy;
				dudxy += dsdxy;

				lod = Max(Float(dudxy.x), Float(dudxy.y));   // FIXME: Max(dudxy.x, dudxy.y);

				// log2(sqrt(lod))
				lod = Float(As<Int>(lod));
				lod -= Float(0x3F800000);
				lod *= As<Float>(Int(0x33800000));

				if(bias)
				{
					lod += lodBias;
				}
			}
			else
			{
				lod = lodBias + *Pointer<Float>(texture + OFFSET(Texture,LOD));
			}

			lod = Max(lod, Float(0.0f));    // FIXME
			lod = Min(lod, Float(MIPMAP_LEVELS - 2));   // Trilinear accesses lod+1
		}
	}

	void SamplerCore::cubeFace(Int face[4], Float4 &U, Float4 &V, Float4 &lodU, Float4 &lodV, Float4 &x, Float4 &y, Float4 &z)
	{
		Int4 xp = CmpNLE(x, Float4(0.0f, 0.0f, 0.0f, 0.0f));   // x > 0
		Int4 yp = CmpNLE(y, Float4(0.0f, 0.0f, 0.0f, 0.0f));   // y > 0
		Int4 zp = CmpNLE(z, Float4(0.0f, 0.0f, 0.0f, 0.0f));   // z > 0

		Float4 absX = Abs(x);
		Float4 absY = Abs(y);
		Float4 absZ = Abs(z);
		Int4 xy = CmpNLE(absX, absY);   // abs(x) > abs(y)
		Int4 yz = CmpNLE(absY, absZ);   // abs(y) > abs(z)
		Int4 zx = CmpNLE(absZ, absX);   // abs(z) > abs(x)

		Int4 xyz = ~zx & xy;   // abs(x) > abs(y) && abs(x) > abs(z)
		Int4 yzx = ~xy & yz;   // abs(y) > abs(z) && abs(y) > abs(x)
		Int4 zxy = ~yz & zx;   // abs(z) > abs(x) && abs(z) > abs(y)

		// FACE_POSITIVE_X = 000b
		// FACE_NEGATIVE_X = 001b
		// FACE_POSITIVE_Y = 010b
		// FACE_NEGATIVE_Y = 011b
		// FACE_POSITIVE_Z = 100b
		// FACE_NEGATIVE_Z = 101b

		Int yAxis = SignMask(yzx);
		Int zAxis = SignMask(zxy);

		Int4 n = (~xp & xyz) | (~yp & yzx) | (~zp & zxy);
		Int negative = SignMask(n);

		face[0] = *Pointer<Int>(constants + OFFSET(Constants,transposeBit0) + negative * 4);
		face[0] |= *Pointer<Int>(constants + OFFSET(Constants,transposeBit1) + yAxis * 4);
		face[0] |= *Pointer<Int>(constants + OFFSET(Constants,transposeBit2) + zAxis * 4);
		face[1] = (face[0] >> 4)  & 0x7;
		face[2] = (face[0] >> 8)  & 0x7;
		face[3] = (face[0] >> 12) & 0x7;
		face[0] &= 0x7;

		// U = xyz * -z + ~xyz * (yzx * ~yp * -x + (yzx * ~yp) * x)
		U = As<Float4>((xyz & As<Int4>(-z)) | (~xyz & (((yzx & ~yp) & Int4(0x80000000, 0x80000000, 0x80000000, 0x80000000)) ^ As<Int4>(x))));

		// V = yzx * z + ~yzx * (~neg * -y + neg * y)
		V = As<Float4>((~yzx & ((~n & Int4(0x80000000, 0x80000000, 0x80000000, 0x80000000)) ^ As<Int4>(y))) | (yzx & As<Int4>(z)));

		// M = xyz * x + yzx * y + zxy * z
		Float4 M = As<Float4>((xyz & As<Int4>(x)) | (yzx & As<Int4>(y)) | (zxy & As<Int4>(z)));
		
		M = reciprocal(M);
		U *= M * Float4(0.5f, 0.5f, 0.5f, 0.5f);
		V *= M * Float4(0.5f, 0.5f, 0.5f, 0.5f);

		// Project coordinates onto one face for consistent LOD calculation
		{
			yp = Swizzle(yp, 0);
			n = Swizzle(n, 0);
			xyz = Swizzle(xyz, 0);
			yzx = Swizzle(yzx, 0);
			zxy = Swizzle(zxy, 0);

			// U = xyz * -z + ~xyz * (yzx * ~yp * -x + (yzx * ~yp) * x)
			lodU = As<Float4>((xyz & As<Int4>(-z)) | (~xyz & (((yzx & ~yp) & Int4(0x80000000, 0x80000000, 0x80000000, 0x80000000)) ^ As<Int4>(x))));

			// V = yzx * z + ~yzx * (~neg * -y + neg * y)
			lodV = As<Float4>((~yzx & ((~n & Int4(0x80000000, 0x80000000, 0x80000000, 0x80000000)) ^ As<Int4>(y))) | (yzx & As<Int4>(z)));

			// M = xyz * x + yzx * y + zxy * z
			Float4 M = As<Float4>((xyz & As<Int4>(x)) | (yzx & As<Int4>(y)) | (zxy & As<Int4>(z)));
			
			M = Rcp_pp(M);
			lodU *= M * Float4(0.5f, 0.5f, 0.5f, 0.5f);
			lodV *= M * Float4(0.5f, 0.5f, 0.5f, 0.5f);
		}
	}

	void SamplerCore::computeIndices(Int index[4], Short4 uuuu, Short4 vvvv, Short4 wwww, const Pointer<Byte> &mipmap)
	{
		Short4 uuu2;

		if(!state.hasNPOTTexture && !hasFloatTexture())
		{
			vvvv = As<UShort4>(vvvv) >> *Pointer<Long1>(mipmap + OFFSET(Mipmap,vFrac));
			uuu2 = uuuu;
			uuuu = As<Short4>(UnpackLow(uuuu, vvvv));
			uuu2 = As<Short4>(UnpackHigh(uuu2, vvvv));
			uuuu = As<Short4>(As<UInt2>(uuuu) >> *Pointer<Long1>(mipmap + OFFSET(Mipmap,uFrac)));
			uuu2 = As<Short4>(As<UInt2>(uuu2) >> *Pointer<Long1>(mipmap + OFFSET(Mipmap,uFrac)));

			if(state.textureType == TEXTURE_3D)
			{
				wwww = As<UShort4>(wwww) >> *Pointer<Long1>(mipmap + OFFSET(Mipmap,wFrac));
				Short4 www2 = wwww;
				wwww = As<Short4>(UnpackLow(wwww, wwww));
				www2 = As<Short4>(UnpackHigh(www2, www2));
				wwww = As<Short4>(As<UInt2>(wwww) >> 16);
				www2 = As<Short4>(As<UInt2>(www2) >> 16);
				wwww = As<Short4>(As<Int2>(wwww) << *Pointer<Long1>(mipmap + OFFSET(Mipmap,uInt)));
				www2 = As<Short4>(As<Int2>(www2) << *Pointer<Long1>(mipmap + OFFSET(Mipmap,uInt)));
				wwww = As<Short4>(As<Int2>(wwww) << *Pointer<Long1>(mipmap + OFFSET(Mipmap,vInt)));   // FIXME: Combine uInt and vInt shift
				www2 = As<Short4>(As<Int2>(www2) << *Pointer<Long1>(mipmap + OFFSET(Mipmap,vInt)));
				uuuu = As<Short4>(As<Int2>(uuuu) + As<Int2>(wwww));
				uuu2 = As<Short4>(As<Int2>(uuu2) + As<Int2>(www2));
			}
		}
		else
		{
			uuuu = MulHigh(As<UShort4>(uuuu), *Pointer<UShort4>(mipmap + OFFSET(Mipmap,width)));
			vvvv = MulHigh(As<UShort4>(vvvv), *Pointer<UShort4>(mipmap + OFFSET(Mipmap,height)));
			uuu2 = uuuu;
			uuuu = As<Short4>(UnpackLow(uuuu, vvvv));
			uuu2 = As<Short4>(UnpackHigh(uuu2, vvvv));
			uuuu = As<Short4>(MulAdd(uuuu, *Pointer<Short4>(mipmap + OFFSET(Mipmap,onePitchP))));
			uuu2 = As<Short4>(MulAdd(uuu2, *Pointer<Short4>(mipmap + OFFSET(Mipmap,onePitchP))));

			if(state.textureType == TEXTURE_3D)
			{
				wwww = MulHigh(As<UShort4>(wwww), *Pointer<UShort4>(mipmap + OFFSET(Mipmap,depth)));
				Short4 www2 = wwww;
				wwww = As<Short4>(UnpackLow(wwww, Short4(0x0000, 0x0000, 0x0000, 0x0000)));
				www2 = As<Short4>(UnpackHigh(www2, Short4(0x0000, 0x0000, 0x0000, 0x0000)));
				wwww = As<Short4>(MulAdd(wwww, *Pointer<Short4>(mipmap + OFFSET(Mipmap,sliceP))));
				www2 = As<Short4>(MulAdd(www2, *Pointer<Short4>(mipmap + OFFSET(Mipmap,sliceP))));
				uuuu = As<Short4>(As<Int2>(uuuu) + As<Int2>(wwww));
				uuu2 = As<Short4>(As<Int2>(uuu2) + As<Int2>(www2));
			}
		}

		index[0] = Extract(As<Int2>(uuuu), 0);
		index[1] = Extract(As<Int2>(uuuu), 1);
		index[2] = Extract(As<Int2>(uuu2), 0);
		index[3] = Extract(As<Int2>(uuu2), 1);
	}

	void SamplerCore::sampleTexel(Color4i &c, Short4 &uuuu, Short4 &vvvv, Short4 &wwww, Pointer<Byte> &mipmap, Pointer<Byte> buffer[4])
	{
		Int index[4];

		computeIndices(index, uuuu, vvvv, wwww, mipmap);

		int f0 = state.textureType == TEXTURE_CUBE ? 0 : 0;
		int f1 = state.textureType == TEXTURE_CUBE ? 1 : 0;
		int f2 = state.textureType == TEXTURE_CUBE ? 2 : 0;
		int f3 = state.textureType == TEXTURE_CUBE ? 3 : 0;

		if(!has16bitTexture())
		{
			Int c0;
			Int c1;
			Int c2;
			Int c3;

			switch(textureComponentCount())
			{
			case 4:
				{
					Byte8 c0 = *Pointer<Byte8>(buffer[f0] + 4 * index[0]);
					Byte8 c1 = *Pointer<Byte8>(buffer[f1] + 4 * index[1]);
					Byte8 c2 = *Pointer<Byte8>(buffer[f2] + 4 * index[2]);
					Byte8 c3 = *Pointer<Byte8>(buffer[f3] + 4 * index[3]);
					c.r = UnpackLow(c0, c1);
					c.g = UnpackLow(c2, c3);
					
					switch(state.textureFormat)
					{
					case FORMAT_A8R8G8B8:
						c.b = c.r;
						c.b = As<Short4>(UnpackLow(c.b, c.g));
						c.r = As<Short4>(UnpackHigh(c.r, c.g));
						c.g = c.b;
						c.a = c.r;
						c.b = UnpackLow(As<Byte8>(c.b), As<Byte8>(c.b));
						c.g = UnpackHigh(As<Byte8>(c.g), As<Byte8>(c.g));
						c.r = UnpackLow(As<Byte8>(c.r), As<Byte8>(c.r));
						c.a = UnpackHigh(As<Byte8>(c.a), As<Byte8>(c.a));
						break;
					case FORMAT_Q8W8V8U8:
						c.b = c.r;
						c.r = As<Short4>(UnpackLow(c.r, c.g));
						c.b = As<Short4>(UnpackHigh(c.b, c.g));
						c.g = c.r;
						c.a = c.b;
						c.r = UnpackLow(As<Byte8>(c.r), As<Byte8>(c.r));
						c.g = UnpackHigh(As<Byte8>(c.g), As<Byte8>(c.g));
						c.b = UnpackLow(As<Byte8>(c.b), As<Byte8>(c.b));
						c.a = UnpackHigh(As<Byte8>(c.a), As<Byte8>(c.a));
						break;
					default:
						ASSERT(false);
					}
				}
				break;
			case 3:
				{
					Byte8 c0 = *Pointer<Byte8>(buffer[f0] + 4 * index[0]);
					Byte8 c1 = *Pointer<Byte8>(buffer[f1] + 4 * index[1]);
					Byte8 c2 = *Pointer<Byte8>(buffer[f2] + 4 * index[2]);
					Byte8 c3 = *Pointer<Byte8>(buffer[f3] + 4 * index[3]);
					c.r = UnpackLow(c0, c1);
					c.g = UnpackLow(c2, c3);

					switch(state.textureFormat)
					{
					case FORMAT_X8R8G8B8:
						c.b = c.r;
						c.b = As<Short4>(UnpackLow(c.b, c.g));
						c.r = As<Short4>(UnpackHigh(c.r, c.g));
						c.g = c.b;
						c.b = UnpackLow(As<Byte8>(c.b), As<Byte8>(c.b));
						c.g = UnpackHigh(As<Byte8>(c.g), As<Byte8>(c.g));
						c.r = UnpackLow(As<Byte8>(c.r), As<Byte8>(c.r));
						break;
					case FORMAT_X8L8V8U8:
						c.b = c.r;
						c.r = As<Short4>(UnpackLow(c.r, c.g));
						c.b = As<Short4>(UnpackHigh(c.b, c.g));
						c.g = c.r;
						c.r = UnpackLow(As<Byte8>(c.r), As<Byte8>(Short4(0x0000, 0x0000, 0x0000, 0x0000)));
						c.r = c.r << 8;
						c.g = UnpackHigh(As<Byte8>(c.g), As<Byte8>(Short4(0x0000, 0x0000, 0x0000, 0x0000)));
						c.g = c.g << 8;
						c.b = UnpackLow(As<Byte8>(c.b), As<Byte8>(c.b));
						break;
					default:
						ASSERT(false);
					}
				}
				break;
			case 2:
				c.r = Insert(c.r, *Pointer<Short>(buffer[f0] + 2 * index[0]), 0);
				c.r = Insert(c.r, *Pointer<Short>(buffer[f1] + 2 * index[1]), 1);
				c.r = Insert(c.r, *Pointer<Short>(buffer[f2] + 2 * index[2]), 2);
				c.r = Insert(c.r, *Pointer<Short>(buffer[f3] + 2 * index[3]), 3);

				switch(state.textureFormat)
				{
				case FORMAT_G8R8:
				case FORMAT_V8U8:
				case FORMAT_A8L8:
					// FIXME: Unpack properly to 0.16 format
					c.g = c.r;
					c.r = c.r << 8;
					break;
				default:
					ASSERT(false);
				}
				break;
			case 1:
				c0 = Int(*Pointer<Byte>(buffer[f0] + index[0]));
				c1 = Int(*Pointer<Byte>(buffer[f1] + index[1]));
				c2 = Int(*Pointer<Byte>(buffer[f2] + index[2]));
				c3 = Int(*Pointer<Byte>(buffer[f3] + index[3]));
				c0 = c0 | (c1 << 8) | (c2 << 16) | (c3 << 24);
				c.r = As<Short4>(Int2(c0));
				c.r = UnpackLow(As<Byte8>(c.r), As<Byte8>(c.r));
				break;
			default:
				ASSERT(false);
			}
		}
		else
		{
			switch(textureComponentCount())
			{
			case 4:
				c.r = *Pointer<Short4>(buffer[f0] + 8 * index[0]);
				c.g = *Pointer<Short4>(buffer[f1] + 8 * index[1]);
				c.b = *Pointer<Short4>(buffer[f2] + 8 * index[2]);
				c.a = *Pointer<Short4>(buffer[f3] + 8 * index[3]);
				transpose4x4(c.r, c.g, c.b, c.a);
				break;
			case 2:
				c.r = *Pointer<Short4>(buffer[f0] + 4 * index[0]);
				c.r = As<Short4>(UnpackLow(c.r, *Pointer<Short4>(buffer[f1] + 4 * index[1])));
				c.b = *Pointer<Short4>(buffer[f2] + 4 * index[2]);
				c.b = As<Short4>(UnpackLow(c.b, *Pointer<Short4>(buffer[f3] + 4 * index[3])));
				c.g = c.r;
				c.r = As<Short4>(UnpackLow(As<Int2>(c.r), As<Int2>(c.b)));
				c.g = As<Short4>(UnpackHigh(As<Int2>(c.g), As<Int2>(c.b)));
				break;
			case 1:
				c.r = Insert(c.r, *Pointer<Short>(buffer[f0] + 2 * index[0]), 0);
				c.r = Insert(c.r, *Pointer<Short>(buffer[f1] + 2 * index[1]), 1);
				c.r = Insert(c.r, *Pointer<Short>(buffer[f2] + 2 * index[2]), 2);
				c.r = Insert(c.r, *Pointer<Short>(buffer[f3] + 2 * index[3]), 3);
				break;
			default:
				ASSERT(false);
			}
		}
	}

	void SamplerCore::sampleTexel(Color4f &c, Short4 &uuuu, Short4 &vvvv, Short4 &wwww, Float4 &z, Pointer<Byte> &mipmap, Pointer<Byte> buffer[4])
	{
		Int index[4];

		computeIndices(index, uuuu, vvvv, wwww, mipmap);

		int f0 = state.textureType == TEXTURE_CUBE ? 0 : 0;
		int f1 = state.textureType == TEXTURE_CUBE ? 1 : 0;
		int f2 = state.textureType == TEXTURE_CUBE ? 2 : 0;
		int f3 = state.textureType == TEXTURE_CUBE ? 3 : 0;

		// Read texels
		switch(textureComponentCount())
		{
		case 4:
			c.r = *Pointer<Float4>(buffer[f0] + index[0] * 16, 16);
			c.g = *Pointer<Float4>(buffer[f1] + index[1] * 16, 16);
			c.b = *Pointer<Float4>(buffer[f2] + index[2] * 16, 16);
			c.a = *Pointer<Float4>(buffer[f3] + index[3] * 16, 16);
			transpose4x4(c.r, c.g, c.b, c.a);
			break;
		case 2:
			// FIXME: Optimal shuffling?
			c.r.xy = *Pointer<Float4>(buffer[f0] + index[0] * 8);
			c.r.zw = *Pointer<Float4>(buffer[f1] + index[1] * 8 - 8);
			c.b.xy = *Pointer<Float4>(buffer[f2] + index[2] * 8);
			c.b.zw = *Pointer<Float4>(buffer[f3] + index[3] * 8 - 8);
			c.g = c.r;
			c.r = Float4(c.r.xz, c.b.xz);
			c.g = Float4(c.g.yw, c.b.yw);
			break;
		case 1:
			// FIXME: Optimal shuffling?
			c.r.x = *Pointer<Float>(buffer[f0] + index[0] * 4);
			c.r.y = *Pointer<Float>(buffer[f1] + index[1] * 4);
			c.r.z = *Pointer<Float>(buffer[f2] + index[2] * 4);
			c.r.w = *Pointer<Float>(buffer[f3] + index[3] * 4);

			if(state.textureFormat == FORMAT_D32F_SHADOW && state.textureFilter != FILTER_GATHER)
			{
				Float4 d = Min(Max(z, Float4(0.0f)), Float4(1.0f));

				c.r = As<Float4>(As<Int4>(CmpNLT(c.r, d)) & As<Int4>(Float4(1.0f, 1.0f, 1.0f, 1.0f)));   // FIXME: Only less-equal?
			}
			break;
		default:
			ASSERT(false);
		}
	}

	void SamplerCore::selectMipmap(Pointer<Byte> &texture, Pointer<Byte> buffer[4], Pointer<Byte> &mipmap, Float &lod, Int face[4], bool secondLOD)
	{
		if(state.mipmapFilter < MIPMAP_POINT)
		{
			mipmap = texture + OFFSET(Texture,mipmap[0]);
		}
		else
		{
			Int ilod;

			if(state.mipmapFilter == MIPMAP_POINT)
			{
				ilod = RoundInt(lod);
			}
			else   // Linear
			{
				ilod = Int(lod);
			}

			mipmap = texture + OFFSET(Texture,mipmap) + ilod * sizeof(Mipmap) + secondLOD * sizeof(Mipmap);
		}

		if(state.textureType != TEXTURE_CUBE)
		{
			buffer[0] = *Pointer<Pointer<Byte>>(mipmap + OFFSET(Mipmap,buffer[0]));
		}
		else
		{
			for(int i = 0; i < 4; i++)
			{
				buffer[i] = *Pointer<Pointer<Byte>>(mipmap + OFFSET(Mipmap,buffer) + face[i] * sizeof(void*));
			}
		}
	}

	void SamplerCore::address(Short4 &uuuu, Float4 &uw, AddressingMode addressingMode)
	{
		if(addressingMode == ADDRESSING_CLAMP)
		{
			Float4 clamp = Min(Max(uw, Float4(0.0f)), Float4(65535.0f / 65536.0f));

			uuuu = Short4(Int4(clamp * Float4(1 << 16)));
		}
		else if(addressingMode == ADDRESSING_MIRROR)
		{
			Int4 convert = Int4(uw * Float4(1 << 16));
			Int4 mirror = (convert << 15) >> 31;

			convert ^= mirror;

			uuuu = Short4(convert);
		}
		else if(addressingMode == ADDRESSING_MIRRORONCE)
		{
			// Absolute value
			Int4 convert = Int4(Abs(uw * Float4(1 << 16)));

			// Clamp
			convert -= Int4(0x00008000, 0x00008000, 0x00008000, 0x00008000);
			convert = As<Int4>(Pack(convert, convert));

			uuuu = As<Short4>(Int2(convert)) + Short4((short)0x8000, (short)0x8000, (short)0x8000, (short)0x8000);
		}
		else   // Wrap (or border)
		{
			uuuu = Short4(Int4(uw * Float4(1 << 16)));
		}
	}

	void SamplerCore::convertFixed12(Short4 &ci, Float4 &cf)
	{
		ci = RoundShort4(cf * Float4(0x1000, 0x1000, 0x1000, 0x1000));
	}

	void SamplerCore::convertFixed12(Color4i &ci, Color4f &cf)
	{
		convertFixed12(ci.r, cf.r);
		convertFixed12(ci.g, cf.g);
		convertFixed12(ci.b, cf.b);
		convertFixed12(ci.a, cf.a);
	}

	void SamplerCore::convertSigned12(Float4 &cf, Short4 &ci)
	{
		cf = Float4(ci) * Float4(1.0f / 0x0FFE);
	}

//	void SamplerCore::convertSigned12(Color4f &cf, Color4i &ci)
//	{
//		convertSigned12(cf.r, ci.r);
//		convertSigned12(cf.g, ci.g);
//		convertSigned12(cf.b, ci.b);
//		convertSigned12(cf.a, ci.a);
//	}

	void SamplerCore::convertSigned15(Float4 &cf, Short4 &ci)
	{
		cf = Float4(ci) * Float4(1.0f / 0x7FFF, 1.0f / 0x7FFF, 1.0f / 0x7FFF, 1.0f / 0x7FFF);
	}

	void SamplerCore::convertUnsigned16(Float4 &cf, Short4 &ci)
	{
		cf = Float4(As<UShort4>(ci)) * Float4(1.0f / 0xFFFF, 1.0f / 0xFFFF, 1.0f / 0xFFFF, 1.0f / 0xFFFF);
	}

	void SamplerCore::sRGBtoLinear16_12(Short4 &c)
	{
		c = As<UShort4>(c) >> 8;

		Pointer<Byte> LUT = Pointer<Byte>(constants + OFFSET(Constants,sRGBtoLinear8));

		c = Insert(c, *Pointer<Short>(LUT + 2 * Int(Extract(c, 0))), 0);
		c = Insert(c, *Pointer<Short>(LUT + 2 * Int(Extract(c, 1))), 1);
		c = Insert(c, *Pointer<Short>(LUT + 2 * Int(Extract(c, 2))), 2);
		c = Insert(c, *Pointer<Short>(LUT + 2 * Int(Extract(c, 3))), 3);
	}

	bool SamplerCore::hasFloatTexture() const
	{
		return Surface::isFloatFormat((Format)state.textureFormat);
	}

	bool SamplerCore::hasUnsignedTextureComponent(int component) const
	{
		return Surface::isUnsignedComponent((Format)state.textureFormat, component);
	}

	int SamplerCore::textureComponentCount() const
	{
		return Surface::componentCount((Format)state.textureFormat);
	}

	bool SamplerCore::has16bitTexture() const
	{
		switch(state.textureFormat)
		{
		case FORMAT_G8R8:
		case FORMAT_X8R8G8B8:
		case FORMAT_A8R8G8B8:
		case FORMAT_V8U8:
		case FORMAT_Q8W8V8U8:
		case FORMAT_X8L8V8U8:
		case FORMAT_R32F:
		case FORMAT_G32R32F:
		case FORMAT_A32B32G32R32F:
		case FORMAT_A8:
		case FORMAT_R8:
		case FORMAT_L8:
		case FORMAT_A8L8:
		case FORMAT_D32F_LOCKABLE:
		case FORMAT_D32F_TEXTURE:
		case FORMAT_D32F_SHADOW:
			return false;
		case FORMAT_L16:
		case FORMAT_G16R16:
		case FORMAT_A16B16G16R16:
		case FORMAT_V16U16:
		case FORMAT_A16W16V16U16:
		case FORMAT_Q16W16V16U16:
			return true;
		default:
			ASSERT(false);
		}
		
		return false;
	}

	bool SamplerCore::isRGBComponent(int component) const
	{
		switch(state.textureFormat)
		{
		case FORMAT_G8R8:          return component < 2;
		case FORMAT_X8R8G8B8:      return component < 3;
		case FORMAT_A8R8G8B8:      return component < 3;
		case FORMAT_V8U8:          return false;
		case FORMAT_Q8W8V8U8:      return false;
		case FORMAT_X8L8V8U8:      return false;
		case FORMAT_R32F:          return component < 1;
		case FORMAT_G32R32F:       return component < 2;
		case FORMAT_A32B32G32R32F: return component < 3;
		case FORMAT_A8:            return false;
		case FORMAT_R8:            return component < 1;
		case FORMAT_L8:            return component < 1;
		case FORMAT_A8L8:          return component < 1;
		case FORMAT_D32F_LOCKABLE: return false;
		case FORMAT_D32F_TEXTURE:  return false;
		case FORMAT_D32F_SHADOW:   return false;
		case FORMAT_L16:           return component < 1;
		case FORMAT_G16R16:        return component < 2;
		case FORMAT_A16B16G16R16:  return component < 3;
		case FORMAT_V16U16:        return false;
		case FORMAT_A16W16V16U16:  return false;
		case FORMAT_Q16W16V16U16:  return false;
		default:
			ASSERT(false);
		}
		
		return false;
	}
}
