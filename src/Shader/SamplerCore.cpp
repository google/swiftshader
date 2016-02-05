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

#include "SamplerCore.hpp"

#include "Constants.hpp"
#include "Debug.hpp"

namespace
{
	void applySwizzle(sw::SwizzleType swizzle, sw::Short4& s, const sw::Vector4s& c)
	{
		switch(swizzle)
		{
		case sw::SWIZZLE_RED:	s = c.x; break;
		case sw::SWIZZLE_GREEN: s = c.y; break;
		case sw::SWIZZLE_BLUE:  s = c.z; break;
		case sw::SWIZZLE_ALPHA: s = c.w; break;
		case sw::SWIZZLE_ZERO:  s = sw::Short4(0x0000, 0x0000, 0x0000, 0x0000); break;
		case sw::SWIZZLE_ONE:   s = sw::Short4(0x1000, 0x1000, 0x1000, 0x1000); break;
		default: ASSERT(false);
		}
	}

	void applySwizzle(sw::SwizzleType swizzle, sw::Float4& f, const sw::Vector4f& c)
	{
		switch(swizzle)
		{
		case sw::SWIZZLE_RED:	f = c.x; break;
		case sw::SWIZZLE_GREEN: f = c.y; break;
		case sw::SWIZZLE_BLUE:  f = c.z; break;
		case sw::SWIZZLE_ALPHA: f = c.w; break;
		case sw::SWIZZLE_ZERO:  f = sw::Float4(0.0f, 0.0f, 0.0f, 0.0f); break;
		case sw::SWIZZLE_ONE:   f = sw::Float4(1.0f, 1.0f, 1.0f, 1.0f); break;
		default: ASSERT(false);
		}
	}
}

namespace sw
{
	SamplerCore::SamplerCore(Pointer<Byte> &constants, const Sampler::State &state) : constants(constants), state(state)
	{
	}

	void SamplerCore::sampleTexture(Pointer<Byte> &texture, Vector4s &c, Float4 &u, Float4 &v, Float4 &w, Float4 &q, Vector4f &dsx, Vector4f &dsy, bool bias, bool gradients, bool lodProvided, bool fixed12)
	{
		#if PERF_PROFILE
			AddAtomic(Pointer<Long>(&profiler.texOperations), 4);

			if(state.compressedFormat)
			{
				AddAtomic(Pointer<Long>(&profiler.compressedTex), 4);
			}
		#endif

		bool cubeTexture = state.textureType == TEXTURE_CUBE;
		bool volumeTexture = state.textureType == TEXTURE_3D || state.textureType == TEXTURE_2D_ARRAY;

		Float4 uuuu = u;
		Float4 vvvv = v;
		Float4 wwww = w;

		if(state.textureType == TEXTURE_NULL)
		{
			c.x = Short4(0x0000, 0x0000, 0x0000, 0x0000);
			c.y = Short4(0x0000, 0x0000, 0x0000, 0x0000);
			c.z = Short4(0x0000, 0x0000, 0x0000, 0x0000);

			if(fixed12)   // FIXME: Convert to fixed12 at higher level, when required
			{
				c.w = Short4(0x1000, 0x1000, 0x1000, 0x1000);
			}
			else
			{
				c.w = Short4((short)0xFFFF, (short)0xFFFF, (short)0xFFFF, (short)0xFFFF);   // FIXME
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
					computeLod(texture, lod, anisotropy, uDelta, vDelta, uuuu, vvvv, q.x, dsx, dsy, bias, gradients, lodProvided);
				}
				else
				{
					computeLod(texture, lod, anisotropy, uDelta, vDelta, lodU, lodV, q.x, dsx, dsy, bias, gradients, lodProvided);
				}
			}
			else
			{
				computeLod3D(texture, lod, uuuu, vvvv, wwww, q.x, dsx, dsy, bias, gradients, lodProvided);
			}

			if(cubeTexture)
			{
				uuuu += Float4(0.5f);
				vvvv += Float4(0.5f);
			}

			if(!hasFloatTexture())
			{
				sampleFilter(texture, c, uuuu, vvvv, wwww, lod, anisotropy, uDelta, vDelta, face, lodProvided);
			}
			else
			{
				Vector4f cf;

				sampleFloatFilter(texture, cf, uuuu, vvvv, wwww, lod, anisotropy, uDelta, vDelta, face, lodProvided);

				convertFixed12(c, cf);
			}

			if(fixed12 && !hasFloatTexture())
			{
				if(has16bitTextureFormat())
				{
					switch(state.textureFormat)
					{
					case FORMAT_R5G6B5:
						if(state.sRGB)
						{
							sRGBtoLinear16_5_12(c.x);
							sRGBtoLinear16_6_12(c.y);
							sRGBtoLinear16_5_12(c.z);
						}
						else
						{
							c.x = MulHigh(As<UShort4>(c.x), UShort4(0x10000000 / 0xF800));
							c.y = MulHigh(As<UShort4>(c.y), UShort4(0x10000000 / 0xFC00));
							c.z = MulHigh(As<UShort4>(c.z), UShort4(0x10000000 / 0xF800));
						}
						break;
					default:
						ASSERT(false);
					}
				}
				else
				{
					for(int component = 0; component < textureComponentCount(); component++)
					{
						if(state.sRGB && isRGBComponent(component))
						{
							sRGBtoLinear16_8_12(c[component]);   // FIXME: Perform linearization at surface level for read-only textures
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
			}

			if(fixed12 && state.textureFilter != FILTER_GATHER)
			{
				int componentCount = textureComponentCount();

				switch(state.textureFormat)
				{
				case FORMAT_R8I_SNORM:
				case FORMAT_G8R8I_SNORM:
				case FORMAT_X8B8G8R8I_SNORM:
				case FORMAT_A8B8G8R8I_SNORM:
				case FORMAT_R8:
				case FORMAT_R5G6B5:
				case FORMAT_G8R8:
				case FORMAT_R8I:
				case FORMAT_R8UI:
				case FORMAT_G8R8I:
				case FORMAT_G8R8UI:
				case FORMAT_X8B8G8R8I:
				case FORMAT_X8B8G8R8UI:
				case FORMAT_A8B8G8R8I:
				case FORMAT_A8B8G8R8UI:
				case FORMAT_R16I:
				case FORMAT_R16UI:
				case FORMAT_G16R16:
				case FORMAT_G16R16I:
				case FORMAT_G16R16UI:
				case FORMAT_X16B16G16R16I:
				case FORMAT_X16B16G16R16UI:
				case FORMAT_A16B16G16R16:
				case FORMAT_A16B16G16R16I:
				case FORMAT_A16B16G16R16UI:
				case FORMAT_R32I:
				case FORMAT_R32UI:
				case FORMAT_G32R32I:
				case FORMAT_G32R32UI:
				case FORMAT_X32B32G32R32I:
				case FORMAT_X32B32G32R32UI:
				case FORMAT_A32B32G32R32I:
				case FORMAT_A32B32G32R32UI:
				case FORMAT_X8R8G8B8:
				case FORMAT_X8B8G8R8:
				case FORMAT_A8R8G8B8:
				case FORMAT_A8B8G8R8:
				case FORMAT_V8U8:
				case FORMAT_Q8W8V8U8:
				case FORMAT_X8L8V8U8:
				case FORMAT_V16U16:
				case FORMAT_A16W16V16U16:
				case FORMAT_Q16W16V16U16:
				case FORMAT_YV12_BT601:
				case FORMAT_YV12_BT709:
				case FORMAT_YV12_JFIF:
					if(componentCount < 2) c.y = Short4(0x1000, 0x1000, 0x1000, 0x1000);
					if(componentCount < 3) c.z = Short4(0x1000, 0x1000, 0x1000, 0x1000);
					if(componentCount < 4) c.w = Short4(0x1000, 0x1000, 0x1000, 0x1000);
					break;
				case FORMAT_A8:
					c.w = c.x;
					c.x = Short4(0x0000, 0x0000, 0x0000, 0x0000);
					c.y = Short4(0x0000, 0x0000, 0x0000, 0x0000);
					c.z = Short4(0x0000, 0x0000, 0x0000, 0x0000);
					break;
				case FORMAT_L8:
				case FORMAT_L16:
					c.y = c.x;
					c.z = c.x;
					c.w = Short4(0x1000, 0x1000, 0x1000, 0x1000);
					break;
				case FORMAT_A8L8:
					c.w = c.y;
					c.y = c.x;
					c.z = c.x;
					break;
				case FORMAT_R32F:
					c.y = Short4(0x1000, 0x1000, 0x1000, 0x1000);
				case FORMAT_G32R32F:
					c.z = Short4(0x1000, 0x1000, 0x1000, 0x1000);
					c.w = Short4(0x1000, 0x1000, 0x1000, 0x1000);
				case FORMAT_A32B32G32R32F:
					break;
				case FORMAT_D32F:
				case FORMAT_D32F_LOCKABLE:
				case FORMAT_D32FS8_TEXTURE:
				case FORMAT_D32FS8_SHADOW:
					c.y = c.x;
					c.z = c.x;
					c.w = c.x;
					break;
				default:
					ASSERT(false);
				}
			}
		}

		if(fixed12 &&
		   ((state.swizzleR != SWIZZLE_RED) ||
		    (state.swizzleG != SWIZZLE_GREEN) ||
		    (state.swizzleB != SWIZZLE_BLUE) ||
		    (state.swizzleA != SWIZZLE_ALPHA)))
		{
			const Vector4s col(c);
			applySwizzle(state.swizzleR, c.x, col);
			applySwizzle(state.swizzleG, c.y, col);
			applySwizzle(state.swizzleB, c.z, col);
			applySwizzle(state.swizzleA, c.w, col);
		}
	}

	void SamplerCore::sampleTexture(Pointer<Byte> &texture, Vector4f &c, Float4 &u, Float4 &v, Float4 &w, Float4 &q, Vector4f &dsx, Vector4f &dsy, bool bias, bool gradients, bool lodProvided)
	{
		#if PERF_PROFILE
			AddAtomic(Pointer<Long>(&profiler.texOperations), 4);

			if(state.compressedFormat)
			{
				AddAtomic(Pointer<Long>(&profiler.compressedTex), 4);
			}
		#endif

		bool cubeTexture = state.textureType == TEXTURE_CUBE;
		bool volumeTexture = state.textureType == TEXTURE_3D || state.textureType == TEXTURE_2D_ARRAY;

		if(state.textureType == TEXTURE_NULL)
		{
			c.x = Float4(0.0f);
			c.y = Float4(0.0f);
			c.z = Float4(0.0f);
			c.w = Float4(1.0f);
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
						computeLod(texture, lod, anisotropy, uDelta, vDelta, uuuu, vvvv, q.x, dsx, dsy, bias, gradients, lodProvided);
					}
					else
					{
						computeLod(texture, lod, anisotropy, uDelta, vDelta, lodU, lodV, q.x, dsx, dsy, bias, gradients, lodProvided);
					}
				}
				else
				{
					computeLod3D(texture, lod, uuuu, vvvv, wwww, q.x, dsx, dsy, bias, gradients, lodProvided);
				}

				if(cubeTexture)
				{
					uuuu += Float4(0.5f);
					vvvv += Float4(0.5f);
				}

				sampleFloatFilter(texture, c, uuuu, vvvv, wwww, lod, anisotropy, uDelta, vDelta, face, lodProvided);
			}
			else
			{
				Vector4s cs;

				sampleTexture(texture, cs, u, v, w, q, dsx, dsy, bias, gradients, lodProvided, false);

				for(int component = 0; component < textureComponentCount(); component++)
				{
					if(has16bitTextureFormat())
					{
						switch(state.textureFormat)
						{
						case FORMAT_R5G6B5:
							if(state.sRGB)
							{
								sRGBtoLinear16_5_12(cs.x);
								sRGBtoLinear16_6_12(cs.y);
								sRGBtoLinear16_5_12(cs.z);

								convertSigned12(c.x, cs.x);
								convertSigned12(c.y, cs.y);
								convertSigned12(c.z, cs.z);
							}
							else
							{
								c.x = Float4(As<UShort4>(cs.x)) * Float4(1.0f / 0xF800);
								c.y = Float4(As<UShort4>(cs.y)) * Float4(1.0f / 0xFC00);
								c.z = Float4(As<UShort4>(cs.z)) * Float4(1.0f / 0xF800);
							}
							break;
						default:
							ASSERT(false);
						}
					}
					else
					{
						switch(state.textureFormat)
						{
						case FORMAT_R8I:
						case FORMAT_G8R8I:
						case FORMAT_X8B8G8R8I:
						case FORMAT_A8B8G8R8I:
							c[component] = As<Float4>(Int4(cs[component]) >> 8);
							break;
						case FORMAT_R8UI:
						case FORMAT_G8R8UI:
						case FORMAT_X8B8G8R8UI:
						case FORMAT_A8B8G8R8UI:
							c[component] = As<Float4>(Int4(As<UShort4>(cs[component]) >> 8));
							break;
						case FORMAT_R16I:
						case FORMAT_G16R16I:
						case FORMAT_X16B16G16R16I:
						case FORMAT_A16B16G16R16I:
							c[component] = As<Float4>(Int4(cs[component]));
							break;
						case FORMAT_R16UI:
						case FORMAT_G16R16UI:
						case FORMAT_X16B16G16R16UI:
						case FORMAT_A16B16G16R16UI:
							c[component] = As<Float4>(Int4(As<UShort4>(cs[component])));
							break;
						default:
							// Normalized integer formats
							if(state.sRGB && isRGBComponent(component))
							{
								sRGBtoLinear16_8_12(cs[component]);   // FIXME: Perform linearization at surface level for read-only textures
								convertSigned12(c[component], cs[component]);
							}
							else
							{
								if(hasUnsignedTextureComponent(component))
								{
									convertUnsigned16(c[component], cs[component]);
								}
								else
								{
									convertSigned15(c[component], cs[component]);
								}
							}
							break;
						}
					}
				}
			}

			int componentCount = textureComponentCount();

			if(state.textureFilter != FILTER_GATHER)
			{
				switch(state.textureFormat)
				{
				case FORMAT_R8I:
				case FORMAT_R8UI:
				case FORMAT_R16I:
				case FORMAT_R16UI:
				case FORMAT_R32I:
				case FORMAT_R32UI:
					c.y = As<Float4>(UInt4(0));
				case FORMAT_G8R8I:
				case FORMAT_G8R8UI:
				case FORMAT_G16R16I:
				case FORMAT_G16R16UI:
				case FORMAT_G32R32I:
				case FORMAT_G32R32UI:
					c.z = As<Float4>(UInt4(0));
				case FORMAT_X8B8G8R8I:
				case FORMAT_X8B8G8R8UI:
				case FORMAT_X16B16G16R16I:
				case FORMAT_X16B16G16R16UI:
				case FORMAT_X32B32G32R32I:
				case FORMAT_X32B32G32R32UI:
					c.w = As<Float4>(UInt4(1));
				case FORMAT_A8B8G8R8I:
				case FORMAT_A8B8G8R8UI:
				case FORMAT_A16B16G16R16I:
				case FORMAT_A16B16G16R16UI:
				case FORMAT_A32B32G32R32I:
				case FORMAT_A32B32G32R32UI:
					break;
				case FORMAT_R8I_SNORM:
				case FORMAT_G8R8I_SNORM:
				case FORMAT_X8B8G8R8I_SNORM:
				case FORMAT_A8B8G8R8I_SNORM:
				case FORMAT_R8:
				case FORMAT_R5G6B5:
				case FORMAT_G8R8:
				case FORMAT_G16R16:
				case FORMAT_A16B16G16R16:
				case FORMAT_X8R8G8B8:
				case FORMAT_X8B8G8R8:
				case FORMAT_A8R8G8B8:
				case FORMAT_A8B8G8R8:
				case FORMAT_V8U8:
				case FORMAT_Q8W8V8U8:
				case FORMAT_X8L8V8U8:
				case FORMAT_V16U16:
				case FORMAT_A16W16V16U16:
				case FORMAT_Q16W16V16U16:
					if(componentCount < 2) c.y = Float4(1.0f);
					if(componentCount < 3) c.z = Float4(1.0f);
					if(componentCount < 4) c.w = Float4(1.0f);
					break;
				case FORMAT_A8:
					c.w = c.x;
					c.x = Float4(0.0f);
					c.y = Float4(0.0f);
					c.z = Float4(0.0f);
					break;
				case FORMAT_L8:
				case FORMAT_L16:
					c.y = c.x;
					c.z = c.x;
					c.w = Float4(1.0f);
					break;
				case FORMAT_A8L8:
					c.w = c.y;
					c.y = c.x;
					c.z = c.x;
					break;
				case FORMAT_R32F:
					c.y = Float4(1.0f);
				case FORMAT_G32R32F:
					c.z = Float4(1.0f);
					c.w = Float4(1.0f);
				case FORMAT_A32B32G32R32F:
					break;
				case FORMAT_D32F:
				case FORMAT_D32F_LOCKABLE:
				case FORMAT_D32FS8_TEXTURE:
				case FORMAT_D32FS8_SHADOW:
					c.y = c.x;
					c.z = c.x;
					c.w = c.x;
					break;
				default:
					ASSERT(false);
				}
			}
		}

		if((state.swizzleR != SWIZZLE_RED) ||
		   (state.swizzleG != SWIZZLE_GREEN) ||
		   (state.swizzleB != SWIZZLE_BLUE) ||
		   (state.swizzleA != SWIZZLE_ALPHA))
		{
			const Vector4f col(c);
			applySwizzle(state.swizzleR, c.x, col);
			applySwizzle(state.swizzleG, c.y, col);
			applySwizzle(state.swizzleB, c.z, col);
			applySwizzle(state.swizzleA, c.w, col);
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

	void SamplerCore::sampleFilter(Pointer<Byte> &texture, Vector4s &c, Float4 &u, Float4 &v, Float4 &w, Float &lod, Float &anisotropy, Float4 &uDelta, Float4 &vDelta, Int face[4], bool lodProvided)
	{
		bool volumeTexture = state.textureType == TEXTURE_3D || state.textureType == TEXTURE_2D_ARRAY;

		sampleAniso(texture, c, u, v, w, lod, anisotropy, uDelta, vDelta, face, false, lodProvided);

		if(state.mipmapFilter > MIPMAP_POINT)
		{
			Vector4s cc;

			sampleAniso(texture, cc, u, v, w, lod, anisotropy, uDelta, vDelta, face, true, lodProvided);

			lod *= Float(1 << 16);

			UShort4 utri = UShort4(Float4(lod));   // FIXME: Optimize
			Short4 stri = utri >> 1;   // FIXME: Optimize

			if(hasUnsignedTextureComponent(0)) cc.x = MulHigh(As<UShort4>(cc.x), utri); else cc.x = MulHigh(cc.x, stri);
			if(hasUnsignedTextureComponent(1)) cc.y = MulHigh(As<UShort4>(cc.y), utri); else cc.y = MulHigh(cc.y, stri);
			if(hasUnsignedTextureComponent(2)) cc.z = MulHigh(As<UShort4>(cc.z), utri); else cc.z = MulHigh(cc.z, stri);
			if(hasUnsignedTextureComponent(3)) cc.w = MulHigh(As<UShort4>(cc.w), utri); else cc.w = MulHigh(cc.w, stri);

			utri = ~utri;
			stri = Short4(0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF) - stri;

			if(hasUnsignedTextureComponent(0)) c.x = MulHigh(As<UShort4>(c.x), utri); else c.x = MulHigh(c.x, stri);
			if(hasUnsignedTextureComponent(1)) c.y = MulHigh(As<UShort4>(c.y), utri); else c.y = MulHigh(c.y, stri);
			if(hasUnsignedTextureComponent(2)) c.z = MulHigh(As<UShort4>(c.z), utri); else c.z = MulHigh(c.z, stri);
			if(hasUnsignedTextureComponent(3)) c.w = MulHigh(As<UShort4>(c.w), utri); else c.w = MulHigh(c.w, stri);
			
			c.x += cc.x;
			c.y += cc.y;
			c.z += cc.z;
			c.w += cc.w;
			
			if(!hasUnsignedTextureComponent(0)) c.x += c.x;
			if(!hasUnsignedTextureComponent(1)) c.y += c.y;
			if(!hasUnsignedTextureComponent(2)) c.z += c.z;
			if(!hasUnsignedTextureComponent(3)) c.w += c.w;
		}

		Short4 borderMask;

		if(state.addressingModeU == ADDRESSING_BORDER)
		{
			Short4 u0;
			
			border(u0, u);

			borderMask = u0;
		}

		if(state.addressingModeV == ADDRESSING_BORDER)
		{
			Short4 v0;
			
			border(v0, v);

			if(state.addressingModeU == ADDRESSING_BORDER)
			{
				borderMask &= v0;
			}
			else
			{
				borderMask = v0;
			}
		}

		if(state.addressingModeW == ADDRESSING_BORDER && volumeTexture)
		{
			Short4 s0;

			border(s0, w);

			if(state.addressingModeU == ADDRESSING_BORDER ||
			   state.addressingModeV == ADDRESSING_BORDER)
			{
				borderMask &= s0;
			}
			else
			{
				borderMask = s0;
			}
		}

		if(state.addressingModeU == ADDRESSING_BORDER ||
		   state.addressingModeV == ADDRESSING_BORDER ||
		   (state.addressingModeW == ADDRESSING_BORDER && volumeTexture))
		{
			Short4 b;

			c.x = borderMask & c.x | ~borderMask & (*Pointer<Short4>(texture + OFFSET(Texture,borderColor4[0])) >> (hasUnsignedTextureComponent(0) ? 0 : 1));
			c.y = borderMask & c.y | ~borderMask & (*Pointer<Short4>(texture + OFFSET(Texture,borderColor4[1])) >> (hasUnsignedTextureComponent(1) ? 0 : 1));
			c.z = borderMask & c.z | ~borderMask & (*Pointer<Short4>(texture + OFFSET(Texture,borderColor4[2])) >> (hasUnsignedTextureComponent(2) ? 0 : 1));
			c.w = borderMask & c.w | ~borderMask & (*Pointer<Short4>(texture + OFFSET(Texture,borderColor4[3])) >> (hasUnsignedTextureComponent(3) ? 0 : 1));
		}
	}

	void SamplerCore::sampleAniso(Pointer<Byte> &texture, Vector4s &c, Float4 &u, Float4 &v, Float4 &w, Float &lod, Float &anisotropy, Float4 &uDelta, Float4 &vDelta, Int face[4], bool secondLOD, bool lodProvided)
	{
		if(state.textureFilter != FILTER_ANISOTROPIC || lodProvided)
		{
			sampleQuad(texture, c, u, v, w, lod, face, secondLOD);
		}
		else
		{
			Int a = RoundInt(anisotropy);

			Vector4s cSum;

			cSum.x = Short4(0, 0, 0, 0);
			cSum.y = Short4(0, 0, 0, 0);
			cSum.z = Short4(0, 0, 0, 0);
			cSum.w = Short4(0, 0, 0, 0);

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

				if(hasUnsignedTextureComponent(0)) cSum.x += As<Short4>(MulHigh(As<UShort4>(c.x), cw)); else cSum.x += MulHigh(c.x, sw);
				if(hasUnsignedTextureComponent(1)) cSum.y += As<Short4>(MulHigh(As<UShort4>(c.y), cw)); else cSum.y += MulHigh(c.y, sw);
				if(hasUnsignedTextureComponent(2)) cSum.z += As<Short4>(MulHigh(As<UShort4>(c.z), cw)); else cSum.z += MulHigh(c.z, sw);
				if(hasUnsignedTextureComponent(3)) cSum.w += As<Short4>(MulHigh(As<UShort4>(c.w), cw)); else cSum.w += MulHigh(c.w, sw);

				i++;
			}
			Until(i >= a)

			if(hasUnsignedTextureComponent(0)) c.x = cSum.x; else c.x = AddSat(cSum.x, cSum.x);
			if(hasUnsignedTextureComponent(1)) c.y = cSum.y; else c.y = AddSat(cSum.y, cSum.y);
			if(hasUnsignedTextureComponent(2)) c.z = cSum.z; else c.z = AddSat(cSum.z, cSum.z);
			if(hasUnsignedTextureComponent(3)) c.w = cSum.w; else c.w = AddSat(cSum.w, cSum.w);
		}
	}

	void SamplerCore::sampleQuad(Pointer<Byte> &texture, Vector4s &c, Float4 &u, Float4 &v, Float4 &w, Float &lod, Int face[4], bool secondLOD)
	{
		if(state.textureType != TEXTURE_3D)
		{
			sampleQuad2D(texture, c, u, v, w, lod, face, secondLOD);
		}
		else
		{
			sample3D(texture, c, u, v, w, lod, secondLOD);
		}
	}

	void SamplerCore::sampleQuad2D(Pointer<Byte> &texture, Vector4s &c, Float4 &u, Float4 &v, Float4 &w, Float &lod, Int face[4], bool secondLOD)
	{
		int componentCount = textureComponentCount();
		bool gather = state.textureFilter == FILTER_GATHER;

		Pointer<Byte> mipmap;
		Pointer<Byte> buffer[4];

		selectMipmap(texture, buffer, mipmap, lod, face, secondLOD);

		Short4 uuuu;
		Short4 vvvv;
		Short4 wwww;

		address(uuuu, u, state.addressingModeU);
		address(vvvv, v, state.addressingModeV);
		if(state.textureType == TEXTURE_2D_ARRAY)
		{
			addressW(wwww, w, mipmap);
		}
		else
		{
			wwww = vvvv;
		}

		if(state.textureFilter == FILTER_POINT || state.textureFilter == FILTER_MIN_POINT_MAG_LINEAR)
		{
			sampleTexel(c, uuuu, vvvv, wwww, mipmap, buffer);
		}
		else
		{
			Vector4s c0;
			Vector4s c1;
			Vector4s c2;
			Vector4s c3;

			Short4 uuuu0 = offsetSample(uuuu, mipmap, OFFSET(Mipmap,uHalf), state.addressingModeU == ADDRESSING_WRAP, gather ? 0 : -1);
			Short4 vvvv0 = offsetSample(vvvv, mipmap, OFFSET(Mipmap,vHalf), state.addressingModeV == ADDRESSING_WRAP, gather ? 0 : -1);
			Short4 uuuu1 = offsetSample(uuuu, mipmap, OFFSET(Mipmap,uHalf), state.addressingModeU == ADDRESSING_WRAP, gather ? 2 : +1);
			Short4 vvvv1 = offsetSample(vvvv, mipmap, OFFSET(Mipmap,vHalf), state.addressingModeV == ADDRESSING_WRAP, gather ? 2 : +1);

			sampleTexel(c0, uuuu0, vvvv0, wwww, mipmap, buffer);
			sampleTexel(c1, uuuu1, vvvv0, wwww, mipmap, buffer);
			sampleTexel(c2, uuuu0, vvvv1, wwww, mipmap, buffer);
			sampleTexel(c3, uuuu1, vvvv1, wwww, mipmap, buffer);
	
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

				UShort4 f1u = ~f0u;
				UShort4 f1v = ~f0v;
			
				UShort4 f0u0v = MulHigh(f0u, f0v);
				UShort4 f1u0v = MulHigh(f1u, f0v);
				UShort4 f0u1v = MulHigh(f0u, f1v);
				UShort4 f1u1v = MulHigh(f1u, f1v);

				// Signed fractions
				Short4 f1u1vs;
				Short4 f0u1vs;
				Short4 f1u0vs;
				Short4 f0u0vs;

				if(!hasUnsignedTextureComponent(0) || !hasUnsignedTextureComponent(1) || !hasUnsignedTextureComponent(2) || !hasUnsignedTextureComponent(3))
				{
					f1u1vs = f1u1v >> 1;
					f0u1vs = f0u1v >> 1;
					f1u0vs = f1u0v >> 1;
					f0u0vs = f0u0v >> 1;
				}

				// Bilinear interpolation
				if(componentCount >= 1)
				{
					if(has16bitTextureComponents() && hasUnsignedTextureComponent(0))
					{
						c0.x = As<UShort4>(c0.x) - MulHigh(As<UShort4>(c0.x), f0u) + MulHigh(As<UShort4>(c1.x), f0u);
						c2.x = As<UShort4>(c2.x) - MulHigh(As<UShort4>(c2.x), f0u) + MulHigh(As<UShort4>(c3.x), f0u);
						c.x  = As<UShort4>(c0.x) - MulHigh(As<UShort4>(c0.x), f0v) + MulHigh(As<UShort4>(c2.x), f0v);
					}
					else
					{
						if(hasUnsignedTextureComponent(0))
						{
							c0.x = MulHigh(As<UShort4>(c0.x), f1u1v);
							c1.x = MulHigh(As<UShort4>(c1.x), f0u1v);
							c2.x = MulHigh(As<UShort4>(c2.x), f1u0v);
							c3.x = MulHigh(As<UShort4>(c3.x), f0u0v);
						}
						else
						{
							c0.x = MulHigh(c0.x, f1u1vs);
							c1.x = MulHigh(c1.x, f0u1vs);
							c2.x = MulHigh(c2.x, f1u0vs);
							c3.x = MulHigh(c3.x, f0u0vs);
						}

						c.x = (c0.x + c1.x) + (c2.x + c3.x);
						if(!hasUnsignedTextureComponent(0)) c.x = AddSat(c.x, c.x);   // Correct for signed fractions
					}
				}

				if(componentCount >= 2)
				{
					if(has16bitTextureComponents() && hasUnsignedTextureComponent(1))
					{
						c0.y = As<UShort4>(c0.y) - MulHigh(As<UShort4>(c0.y), f0u) + MulHigh(As<UShort4>(c1.y), f0u);
						c2.y = As<UShort4>(c2.y) - MulHigh(As<UShort4>(c2.y), f0u) + MulHigh(As<UShort4>(c3.y), f0u);
						c.y  = As<UShort4>(c0.y) - MulHigh(As<UShort4>(c0.y), f0v) + MulHigh(As<UShort4>(c2.y), f0v);
					}
					else
					{
						if(hasUnsignedTextureComponent(1))
						{
							c0.y = MulHigh(As<UShort4>(c0.y), f1u1v);
							c1.y = MulHigh(As<UShort4>(c1.y), f0u1v);
							c2.y = MulHigh(As<UShort4>(c2.y), f1u0v);
							c3.y = MulHigh(As<UShort4>(c3.y), f0u0v);
						}
						else
						{
							c0.y = MulHigh(c0.y, f1u1vs);
							c1.y = MulHigh(c1.y, f0u1vs);
							c2.y = MulHigh(c2.y, f1u0vs);
							c3.y = MulHigh(c3.y, f0u0vs);
						}

						c.y = (c0.y + c1.y) + (c2.y + c3.y);
						if(!hasUnsignedTextureComponent(1)) c.y = AddSat(c.y, c.y);   // Correct for signed fractions
					}
				}

				if(componentCount >= 3)
				{
					if(has16bitTextureComponents() && hasUnsignedTextureComponent(2))
					{
						c0.z = As<UShort4>(c0.z) - MulHigh(As<UShort4>(c0.z), f0u) + MulHigh(As<UShort4>(c1.z), f0u);
						c2.z = As<UShort4>(c2.z) - MulHigh(As<UShort4>(c2.z), f0u) + MulHigh(As<UShort4>(c3.z), f0u);
						c.z  = As<UShort4>(c0.z) - MulHigh(As<UShort4>(c0.z), f0v) + MulHigh(As<UShort4>(c2.z), f0v);
					}
					else
					{
						if(hasUnsignedTextureComponent(2))
						{
							c0.z = MulHigh(As<UShort4>(c0.z), f1u1v);
							c1.z = MulHigh(As<UShort4>(c1.z), f0u1v);
							c2.z = MulHigh(As<UShort4>(c2.z), f1u0v);
							c3.z = MulHigh(As<UShort4>(c3.z), f0u0v);
						}
						else
						{
							c0.z = MulHigh(c0.z, f1u1vs);
							c1.z = MulHigh(c1.z, f0u1vs);
							c2.z = MulHigh(c2.z, f1u0vs);
							c3.z = MulHigh(c3.z, f0u0vs);
						}

						c.z = (c0.z + c1.z) + (c2.z + c3.z);
						if(!hasUnsignedTextureComponent(2)) c.z = AddSat(c.z, c.z);   // Correct for signed fractions
					}
				}

				if(componentCount >= 4)
				{
					if(has16bitTextureComponents() && hasUnsignedTextureComponent(3))
					{
						c0.w = As<UShort4>(c0.w) - MulHigh(As<UShort4>(c0.w), f0u) + MulHigh(As<UShort4>(c1.w), f0u);
						c2.w = As<UShort4>(c2.w) - MulHigh(As<UShort4>(c2.w), f0u) + MulHigh(As<UShort4>(c3.w), f0u);
						c.w  = As<UShort4>(c0.w) - MulHigh(As<UShort4>(c0.w), f0v) + MulHigh(As<UShort4>(c2.w), f0v);
					}
					else
					{
						if(hasUnsignedTextureComponent(3))
						{
							c0.w = MulHigh(As<UShort4>(c0.w), f1u1v);
							c1.w = MulHigh(As<UShort4>(c1.w), f0u1v);
							c2.w = MulHigh(As<UShort4>(c2.w), f1u0v);
							c3.w = MulHigh(As<UShort4>(c3.w), f0u0v);
						}
						else
						{
							c0.w = MulHigh(c0.w, f1u1vs);
							c1.w = MulHigh(c1.w, f0u1vs);
							c2.w = MulHigh(c2.w, f1u0vs);
							c3.w = MulHigh(c3.w, f0u0vs);
						}

						c.w = (c0.w + c1.w) + (c2.w + c3.w);
						if(!hasUnsignedTextureComponent(3)) c.w = AddSat(c.w, c.w);   // Correct for signed fractions
					}
				}
			}
			else
			{
				c.x = c1.x;
				c.y = c2.x;
				c.z = c3.x;
				c.w = c0.x;
			}
		}
	}

	void SamplerCore::sample3D(Pointer<Byte> &texture, Vector4s &c_, Float4 &u_, Float4 &v_, Float4 &w_, Float &lod, bool secondLOD)
	{
		int componentCount = textureComponentCount();

		Pointer<Byte> mipmap;
		Pointer<Byte> buffer[4];
		Int face[4];
		
		selectMipmap(texture, buffer, mipmap, lod, face, secondLOD);

		Short4 uuuu;
		Short4 vvvv;
		Short4 wwww;

		address(uuuu, u_, state.addressingModeU);
		address(vvvv, v_, state.addressingModeV);
		addressW(wwww, w_, mipmap);

		if(state.textureFilter <= FILTER_POINT || state.textureFilter == FILTER_MIN_POINT_MAG_LINEAR)
		{
			sampleTexel(c_, uuuu, vvvv, wwww, mipmap, buffer);
		}
		else
		{
			Vector4s c[2][2][2];

			Short4 u[2][2][2];
			Short4 v[2][2][2];
			Short4 s[2][2][2];

			for(int i = 0; i < 2; i++)
			{
				for(int j = 0; j < 2; j++)
				{
					for(int k = 0; k < 2; k++)
					{
						u[i][j][k] = offsetSample(uuuu, mipmap, OFFSET(Mipmap,uHalf), state.addressingModeU == ADDRESSING_WRAP, i * 2 - 1);
						v[i][j][k] = offsetSample(vvvv, mipmap, OFFSET(Mipmap,vHalf), state.addressingModeV == ADDRESSING_WRAP, j * 2 - 1);
						s[i][j][k] = offsetSample(wwww, mipmap, OFFSET(Mipmap,wHalf), state.addressingModeW == ADDRESSING_WRAP, k * 2 - 1);
					}
				}
			}

			// Fractions
			UShort4 f[2][2][2];
			Short4 fs[2][2][2];
			UShort4 f0u;
			UShort4 f0v;
			UShort4 f0s;

			if(!state.hasNPOTTexture)
			{
				f0u = As<UShort4>(u[0][0][0]) << *Pointer<Long1>(mipmap + OFFSET(Mipmap,uInt));
				f0v = As<UShort4>(v[0][0][0]) << *Pointer<Long1>(mipmap + OFFSET(Mipmap,vInt));
				f0s = As<UShort4>(s[0][0][0]) << *Pointer<Long1>(mipmap + OFFSET(Mipmap,wInt));
			}
			else
			{
				f0u = As<UShort4>(u[0][0][0]) * *Pointer<UShort4>(mipmap + OFFSET(Mipmap,width));
				f0v = As<UShort4>(v[0][0][0]) * *Pointer<UShort4>(mipmap + OFFSET(Mipmap,height));
				f0s = As<UShort4>(s[0][0][0]) * *Pointer<UShort4>(mipmap + OFFSET(Mipmap,depth));
			}

			UShort4 f1u = ~f0u;
			UShort4 f1v = ~f0v;
			UShort4 f1s = ~f0s;

			f[1][1][1] = MulHigh(f1u, f1v);
			f[0][1][1] = MulHigh(f0u, f1v);
			f[1][0][1] = MulHigh(f1u, f0v);
			f[0][0][1] = MulHigh(f0u, f0v);
			f[1][1][0] = MulHigh(f1u, f1v);
			f[0][1][0] = MulHigh(f0u, f1v);
			f[1][0][0] = MulHigh(f1u, f0v);
			f[0][0][0] = MulHigh(f0u, f0v);

			f[1][1][1] = MulHigh(f[1][1][1], f1s);
			f[0][1][1] = MulHigh(f[0][1][1], f1s);
			f[1][0][1] = MulHigh(f[1][0][1], f1s);
			f[0][0][1] = MulHigh(f[0][0][1], f1s);
			f[1][1][0] = MulHigh(f[1][1][0], f0s);
			f[0][1][0] = MulHigh(f[0][1][0], f0s);
			f[1][0][0] = MulHigh(f[1][0][0], f0s);
			f[0][0][0] = MulHigh(f[0][0][0], f0s);

			// Signed fractions
			if(!hasUnsignedTextureComponent(0) || !hasUnsignedTextureComponent(1) || !hasUnsignedTextureComponent(2) || !hasUnsignedTextureComponent(3))
			{
				fs[0][0][0] = f[0][0][0] >> 1;
				fs[0][0][1] = f[0][0][1] >> 1;
				fs[0][1][0] = f[0][1][0] >> 1;
				fs[0][1][1] = f[0][1][1] >> 1;
				fs[1][0][0] = f[1][0][0] >> 1;
				fs[1][0][1] = f[1][0][1] >> 1;
				fs[1][1][0] = f[1][1][0] >> 1;
				fs[1][1][1] = f[1][1][1] >> 1;
			}

			for(int i = 0; i < 2; i++)
			{
				for(int j = 0; j < 2; j++)
				{
					for(int k = 0; k < 2; k++)
					{
						sampleTexel(c[i][j][k], u[i][j][k], v[i][j][k], s[i][j][k], mipmap, buffer);

						if(componentCount >= 1) { if(hasUnsignedTextureComponent(0)) c[i][j][k].x = MulHigh(As<UShort4>(c[i][j][k].x), f[1 - i][1 - j][1 - k]); else c[i][j][k].x = MulHigh(c[i][j][k].x, fs[1 - i][1 - j][1 - k]); }
						if(componentCount >= 2) { if(hasUnsignedTextureComponent(1)) c[i][j][k].y = MulHigh(As<UShort4>(c[i][j][k].y), f[1 - i][1 - j][1 - k]); else c[i][j][k].y = MulHigh(c[i][j][k].y, fs[1 - i][1 - j][1 - k]); }
						if(componentCount >= 3) { if(hasUnsignedTextureComponent(2)) c[i][j][k].z = MulHigh(As<UShort4>(c[i][j][k].z), f[1 - i][1 - j][1 - k]); else c[i][j][k].z = MulHigh(c[i][j][k].z, fs[1 - i][1 - j][1 - k]); }
						if(componentCount >= 4) { if(hasUnsignedTextureComponent(3)) c[i][j][k].w = MulHigh(As<UShort4>(c[i][j][k].w), f[1 - i][1 - j][1 - k]); else c[i][j][k].w = MulHigh(c[i][j][k].w, fs[1 - i][1 - j][1 - k]); }

						if(i != 0 || j != 0 || k != 0)
						{
							if(componentCount >= 1) c[0][0][0].x += c[i][j][k].x;
							if(componentCount >= 2) c[0][0][0].y += c[i][j][k].y;
							if(componentCount >= 3) c[0][0][0].z += c[i][j][k].z;
							if(componentCount >= 4) c[0][0][0].w += c[i][j][k].w;
						}
					}
				}
			}

			if(componentCount >= 1) c_.x = c[0][0][0].x;
			if(componentCount >= 2) c_.y = c[0][0][0].y;
			if(componentCount >= 3) c_.z = c[0][0][0].z;
			if(componentCount >= 4) c_.w = c[0][0][0].w;

			// Correct for signed fractions
			if(componentCount >= 1) if(!hasUnsignedTextureComponent(0)) c_.x = AddSat(c_.x, c_.x);
			if(componentCount >= 2) if(!hasUnsignedTextureComponent(1)) c_.y = AddSat(c_.y, c_.y);
			if(componentCount >= 3) if(!hasUnsignedTextureComponent(2)) c_.z = AddSat(c_.z, c_.z);
			if(componentCount >= 4) if(!hasUnsignedTextureComponent(3)) c_.w = AddSat(c_.w, c_.w);
		}
	}

	void SamplerCore::sampleFloatFilter(Pointer<Byte> &texture, Vector4f &c, Float4 &u, Float4 &v, Float4 &w, Float &lod, Float &anisotropy, Float4 &uDelta, Float4 &vDelta, Int face[4], bool lodProvided)
	{
		bool volumeTexture = state.textureType == TEXTURE_3D || state.textureType == TEXTURE_2D_ARRAY;

		sampleFloatAniso(texture, c, u, v, w, lod, anisotropy, uDelta, vDelta, face, false, lodProvided);

		if(state.mipmapFilter > MIPMAP_POINT)
		{
			Vector4f cc;

			sampleFloatAniso(texture, cc, u, v, w, lod, anisotropy, uDelta, vDelta, face, true, lodProvided);

			Float4 lod4 = Float4(Frac(lod));

			c.x = (cc.x - c.x) * lod4 + c.x;
			c.y = (cc.y - c.y) * lod4 + c.y;
			c.z = (cc.z - c.z) * lod4 + c.z;
			c.w = (cc.w - c.w) * lod4 + c.w;
		}

		Int4 borderMask;

		if(state.addressingModeU == ADDRESSING_BORDER)
		{
			Int4 u0;
			
			border(u0, u);

			borderMask = u0;
		}

		if(state.addressingModeV == ADDRESSING_BORDER)
		{
			Int4 v0;
			
			border(v0, v);

			if(state.addressingModeU == ADDRESSING_BORDER)
			{
				borderMask &= v0;
			}
			else
			{
				borderMask = v0;
			}
		}

		if(state.addressingModeW == ADDRESSING_BORDER && volumeTexture)
		{
			Int4 s0;

			border(s0, w);

			if(state.addressingModeU == ADDRESSING_BORDER ||
			   state.addressingModeV == ADDRESSING_BORDER)
			{
				borderMask &= s0;
			}
			else
			{
				borderMask = s0;
			}
		}

		if(state.addressingModeU == ADDRESSING_BORDER ||
		   state.addressingModeV == ADDRESSING_BORDER ||
		   (state.addressingModeW == ADDRESSING_BORDER && volumeTexture))
		{
			Int4 b;

			c.x = As<Float4>(borderMask & As<Int4>(c.x) | ~borderMask & *Pointer<Int4>(texture + OFFSET(Texture,borderColorF[0])));
			c.y = As<Float4>(borderMask & As<Int4>(c.y) | ~borderMask & *Pointer<Int4>(texture + OFFSET(Texture,borderColorF[1])));
			c.z = As<Float4>(borderMask & As<Int4>(c.z) | ~borderMask & *Pointer<Int4>(texture + OFFSET(Texture,borderColorF[2])));
			c.w = As<Float4>(borderMask & As<Int4>(c.w) | ~borderMask & *Pointer<Int4>(texture + OFFSET(Texture,borderColorF[3])));
		}
	}

	void SamplerCore::sampleFloatAniso(Pointer<Byte> &texture, Vector4f &c, Float4 &u, Float4 &v, Float4 &w, Float &lod, Float &anisotropy, Float4 &uDelta, Float4 &vDelta, Int face[4], bool secondLOD, bool lodProvided)
	{
		if(state.textureFilter != FILTER_ANISOTROPIC || lodProvided)
		{
			sampleFloat(texture, c, u, v, w, lod, face, secondLOD);
		}
		else
		{
			Int a = RoundInt(anisotropy);

			Vector4f cSum;

			cSum.x = Float4(0.0f);
			cSum.y = Float4(0.0f);
			cSum.z = Float4(0.0f);
			cSum.w = Float4(0.0f);

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

				cSum.x += c.x * A;
				cSum.y += c.y * A;
				cSum.z += c.z * A;
				cSum.w += c.w * A;

				i++;
			}
			Until(i >= a)

			c.x = cSum.x;
			c.y = cSum.y;
			c.z = cSum.z;
			c.w = cSum.w;
		}
	}

	void SamplerCore::sampleFloat(Pointer<Byte> &texture, Vector4f &c, Float4 &u, Float4 &v, Float4 &w, Float &lod, Int face[4], bool secondLOD)
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
	
	void SamplerCore::sampleFloat2D(Pointer<Byte> &texture, Vector4f &c, Float4 &u, Float4 &v, Float4 &w, Float &lod, Int face[4], bool secondLOD)
	{
		int componentCount = textureComponentCount();
		bool gather = state.textureFilter == FILTER_GATHER;

		Pointer<Byte> mipmap;
		Pointer<Byte> buffer[4];
		
		selectMipmap(texture, buffer, mipmap, lod, face, secondLOD);

		Short4 uuuu;
		Short4 vvvv;
		Short4 wwww;

		address(uuuu, u, state.addressingModeU);
		address(vvvv, v, state.addressingModeV);
		if(state.textureType == TEXTURE_2D_ARRAY)
		{
			addressW(wwww, w, mipmap);
		}
		else
		{
			wwww = vvvv;
		}

		if(state.textureFilter == FILTER_POINT || state.textureFilter == FILTER_MIN_POINT_MAG_LINEAR)
		{
			sampleTexel(c, uuuu, vvvv, wwww, w, mipmap, buffer);
		}
		else
		{
			Vector4f c0;
			Vector4f c1;
			Vector4f c2;
			Vector4f c3;

			Short4 uuuu0 = offsetSample(uuuu, mipmap, OFFSET(Mipmap,uHalf), state.addressingModeU == ADDRESSING_WRAP, gather ? 0 : -1);
			Short4 vvvv0 = offsetSample(vvvv, mipmap, OFFSET(Mipmap,vHalf), state.addressingModeV == ADDRESSING_WRAP, gather ? 0 : -1);
			Short4 uuuu1 = offsetSample(uuuu, mipmap, OFFSET(Mipmap,uHalf), state.addressingModeU == ADDRESSING_WRAP, gather ? 2 : +1);
			Short4 vvvv1 = offsetSample(vvvv, mipmap, OFFSET(Mipmap,vHalf), state.addressingModeV == ADDRESSING_WRAP, gather ? 2 : +1);

			sampleTexel(c0, uuuu0, vvvv0, wwww, w, mipmap, buffer);		
			sampleTexel(c1, uuuu1, vvvv0, wwww, w, mipmap, buffer);
			sampleTexel(c2, uuuu0, vvvv1, wwww, w, mipmap, buffer);
			sampleTexel(c3, uuuu1, vvvv1, wwww, w, mipmap, buffer);

			if(!gather)   // Blend
			{
				// Fractions
				Float4 fu = Frac(Float4(As<UShort4>(uuuu0)) * *Pointer<Float4>(mipmap + OFFSET(Mipmap,fWidth)));
				Float4 fv = Frac(Float4(As<UShort4>(vvvv0)) * *Pointer<Float4>(mipmap + OFFSET(Mipmap,fHeight)));

				if(componentCount >= 1) c0.x = c0.x + fu * (c1.x - c0.x);
				if(componentCount >= 2) c0.y = c0.y + fu * (c1.y - c0.y);
				if(componentCount >= 3) c0.z = c0.z + fu * (c1.z - c0.z);
				if(componentCount >= 4) c0.w = c0.w + fu * (c1.w - c0.w);

				if(componentCount >= 1) c2.x = c2.x + fu * (c3.x - c2.x);
				if(componentCount >= 2) c2.y = c2.y + fu * (c3.y - c2.y);
				if(componentCount >= 3) c2.z = c2.z + fu * (c3.z - c2.z);
				if(componentCount >= 4) c2.w = c2.w + fu * (c3.w - c2.w);

				if(componentCount >= 1) c.x = c0.x + fv * (c2.x - c0.x);
				if(componentCount >= 2) c.y = c0.y + fv * (c2.y - c0.y);
				if(componentCount >= 3) c.z = c0.z + fv * (c2.z - c0.z);
				if(componentCount >= 4) c.w = c0.w + fv * (c2.w - c0.w);
			}
			else
			{
				c.x = c1.x;
				c.y = c2.x;
				c.z = c3.x;
				c.w = c0.x;
			}
		}
	}

	void SamplerCore::sampleFloat3D(Pointer<Byte> &texture, Vector4f &c, Float4 &u, Float4 &v, Float4 &w, Float &lod, bool secondLOD)
	{
		int componentCount = textureComponentCount();

		Pointer<Byte> mipmap;
		Pointer<Byte> buffer[4];
		Int face[4];

		selectMipmap(texture, buffer, mipmap, lod, face, secondLOD);

		Short4 uuuu;
		Short4 vvvv;
		Short4 wwww;

		address(uuuu, u, state.addressingModeU);
		address(vvvv, v, state.addressingModeV);
		addressW(wwww, w, mipmap);

		if(state.textureFilter <= FILTER_POINT || state.textureFilter == FILTER_MIN_POINT_MAG_LINEAR)
		{
			sampleTexel(c, uuuu, vvvv, wwww, w, mipmap, buffer);
		}
		else
		{
			Vector4f &c0 = c;
			Vector4f c1;
			Vector4f c2;
			Vector4f c3;
			Vector4f c4;
			Vector4f c5;
			Vector4f c6;
			Vector4f c7;

			Short4 uuuu0 = offsetSample(uuuu, mipmap, OFFSET(Mipmap,uHalf), state.addressingModeU == ADDRESSING_WRAP, -1);
			Short4 vvvv0 = offsetSample(vvvv, mipmap, OFFSET(Mipmap,vHalf), state.addressingModeV == ADDRESSING_WRAP, -1);
			Short4 wwww0 = offsetSample(wwww, mipmap, OFFSET(Mipmap,wHalf), state.addressingModeW == ADDRESSING_WRAP, -1);
			Short4 uuuu1 = offsetSample(uuuu, mipmap, OFFSET(Mipmap,uHalf), state.addressingModeU == ADDRESSING_WRAP, +1);
			Short4 vvvv1 = offsetSample(vvvv, mipmap, OFFSET(Mipmap,vHalf), state.addressingModeV == ADDRESSING_WRAP, +1);
			Short4 wwww1 = offsetSample(wwww, mipmap, OFFSET(Mipmap,wHalf), state.addressingModeW == ADDRESSING_WRAP, +1);

			sampleTexel(c0, uuuu0, vvvv0, wwww0, w, mipmap, buffer);
			sampleTexel(c1, uuuu1, vvvv0, wwww0, w, mipmap, buffer);
			sampleTexel(c2, uuuu0, vvvv1, wwww0, w, mipmap, buffer);
			sampleTexel(c3, uuuu1, vvvv1, wwww0, w, mipmap, buffer);
			sampleTexel(c4, uuuu0, vvvv0, wwww1, w, mipmap, buffer);
			sampleTexel(c5, uuuu1, vvvv0, wwww1, w, mipmap, buffer);
			sampleTexel(c6, uuuu0, vvvv1, wwww1, w, mipmap, buffer);
			sampleTexel(c7, uuuu1, vvvv1, wwww1, w, mipmap, buffer);

			// Fractions
			Float4 fu = Frac(Float4(As<UShort4>(uuuu0)) * *Pointer<Float4>(mipmap + OFFSET(Mipmap,fWidth)));
			Float4 fv = Frac(Float4(As<UShort4>(vvvv0)) * *Pointer<Float4>(mipmap + OFFSET(Mipmap,fHeight)));
			Float4 fw = Frac(Float4(As<UShort4>(wwww0)) * *Pointer<Float4>(mipmap + OFFSET(Mipmap,fDepth)));

			// Blend first slice
			if(componentCount >= 1) c0.x = c0.x + fu * (c1.x - c0.x);
			if(componentCount >= 2) c0.y = c0.y + fu * (c1.y - c0.y);
			if(componentCount >= 3) c0.z = c0.z + fu * (c1.z - c0.z);
			if(componentCount >= 4) c0.w = c0.w + fu * (c1.w - c0.w);

			if(componentCount >= 1) c2.x = c2.x + fu * (c3.x - c2.x);
			if(componentCount >= 2) c2.y = c2.y + fu * (c3.y - c2.y);
			if(componentCount >= 3) c2.z = c2.z + fu * (c3.z - c2.z);
			if(componentCount >= 4) c2.w = c2.w + fu * (c3.w - c2.w);

			if(componentCount >= 1) c0.x = c0.x + fv * (c2.x - c0.x);
			if(componentCount >= 2) c0.y = c0.y + fv * (c2.y - c0.y);
			if(componentCount >= 3) c0.z = c0.z + fv * (c2.z - c0.z);
			if(componentCount >= 4) c0.w = c0.w + fv * (c2.w - c0.w);

			// Blend second slice
			if(componentCount >= 1) c4.x = c4.x + fu * (c5.x - c4.x);
			if(componentCount >= 2) c4.y = c4.y + fu * (c5.y - c4.y);
			if(componentCount >= 3) c4.z = c4.z + fu * (c5.z - c4.z);
			if(componentCount >= 4) c4.w = c4.w + fu * (c5.w - c4.w);

			if(componentCount >= 1) c6.x = c6.x + fu * (c7.x - c6.x);
			if(componentCount >= 2) c6.y = c6.y + fu * (c7.y - c6.y);
			if(componentCount >= 3) c6.z = c6.z + fu * (c7.z - c6.z);
			if(componentCount >= 4) c6.w = c6.w + fu * (c7.w - c6.w);

			if(componentCount >= 1) c4.x = c4.x + fv * (c6.x - c4.x);
			if(componentCount >= 2) c4.y = c4.y + fv * (c6.y - c4.y);
			if(componentCount >= 3) c4.z = c4.z + fv * (c6.z - c4.z);
			if(componentCount >= 4) c4.w = c4.w + fv * (c6.w - c4.w);

			// Blend slices
			if(componentCount >= 1) c0.x = c0.x + fw * (c4.x - c0.x);
			if(componentCount >= 2) c0.y = c0.y + fw * (c4.y - c0.y);
			if(componentCount >= 3) c0.z = c0.z + fw * (c4.z - c0.z);
			if(componentCount >= 4) c0.w = c0.w + fw * (c4.w - c0.w);
		}
	}

	void SamplerCore::computeLod(Pointer<Byte> &texture, Float &lod, Float &anisotropy, Float4 &uDelta, Float4 &vDelta, Float4 &uuuu, Float4 &vvvv, const Float &lodBias, Vector4f &dsx, Vector4f &dsy, bool bias, bool gradients, bool lodProvided)
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

		lod = Max(lod, 0.0f);
		lod = Min(lod, Float(MIPMAP_LEVELS - 2));   // Trilinear accesses lod+1
	}

	void SamplerCore::computeLod3D(Pointer<Byte> &texture, Float &lod, Float4 &uuuu, Float4 &vvvv, Float4 &wwww, const Float &lodBias, Vector4f &dsx, Vector4f &dsy, bool bias, bool gradients, bool lodProvided)
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
		Int4 xp = CmpNLE(x, Float4(0.0f));   // x > 0
		Int4 yp = CmpNLE(y, Float4(0.0f));   // y > 0
		Int4 zp = CmpNLE(z, Float4(0.0f));   // z > 0

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
		U *= M * Float4(0.5f);
		V *= M * Float4(0.5f);

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
			lodU *= M * Float4(0.5f);
			lodV *= M * Float4(0.5f);
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
		}

		if((state.textureType == TEXTURE_3D) || (state.textureType == TEXTURE_2D_ARRAY))
		{
			if(state.textureType != TEXTURE_2D_ARRAY)
			{
				wwww = MulHigh(As<UShort4>(wwww), *Pointer<UShort4>(mipmap + OFFSET(Mipmap, depth)));
			}
			Short4 www2 = wwww;
			wwww = As<Short4>(UnpackLow(wwww, Short4(0x0000, 0x0000, 0x0000, 0x0000)));
			www2 = As<Short4>(UnpackHigh(www2, Short4(0x0000, 0x0000, 0x0000, 0x0000)));
			wwww = As<Short4>(MulAdd(wwww, *Pointer<Short4>(mipmap + OFFSET(Mipmap,sliceP))));
			www2 = As<Short4>(MulAdd(www2, *Pointer<Short4>(mipmap + OFFSET(Mipmap,sliceP))));
			uuuu = As<Short4>(As<Int2>(uuuu) + As<Int2>(wwww));
			uuu2 = As<Short4>(As<Int2>(uuu2) + As<Int2>(www2));
		}

		index[0] = Extract(As<Int2>(uuuu), 0);
		index[1] = Extract(As<Int2>(uuuu), 1);
		index[2] = Extract(As<Int2>(uuu2), 0);
		index[3] = Extract(As<Int2>(uuu2), 1);
	}

	void SamplerCore::sampleTexel(Vector4s &c, Short4 &uuuu, Short4 &vvvv, Short4 &wwww, Pointer<Byte> &mipmap, Pointer<Byte> buffer[4])
	{
		Int index[4];

		computeIndices(index, uuuu, vvvv, wwww, mipmap);

		int f0 = state.textureType == TEXTURE_CUBE ? 0 : 0;
		int f1 = state.textureType == TEXTURE_CUBE ? 1 : 0;
		int f2 = state.textureType == TEXTURE_CUBE ? 2 : 0;
		int f3 = state.textureType == TEXTURE_CUBE ? 3 : 0;

		if(has16bitTextureFormat())
		{
			c.x = Insert(c.x, *Pointer<Short>(buffer[f0] + 2 * index[0]), 0);
			c.x = Insert(c.x, *Pointer<Short>(buffer[f1] + 2 * index[1]), 1);
			c.x = Insert(c.x, *Pointer<Short>(buffer[f2] + 2 * index[2]), 2);
			c.x = Insert(c.x, *Pointer<Short>(buffer[f3] + 2 * index[3]), 3);

			switch(state.textureFormat)
			{
			case FORMAT_R5G6B5:
				c.z = (c.x & Short4(0x001Fu)) << 11;
				c.y = (c.x & Short4(0x07E0u)) << 5;
				c.x = (c.x & Short4(0xF800u));
				break;
			default:
				ASSERT(false);
			}
		}
		else if(has8bitTextureComponents())
		{
			switch(textureComponentCount())
			{
			case 4:
				{
					Byte8 c0 = *Pointer<Byte8>(buffer[f0] + 4 * index[0]);
					Byte8 c1 = *Pointer<Byte8>(buffer[f1] + 4 * index[1]);
					Byte8 c2 = *Pointer<Byte8>(buffer[f2] + 4 * index[2]);
					Byte8 c3 = *Pointer<Byte8>(buffer[f3] + 4 * index[3]);
					c.x = UnpackLow(c0, c1);
					c.y = UnpackLow(c2, c3);
					
					switch(state.textureFormat)
					{
					case FORMAT_A8R8G8B8:
						c.z = c.x;
						c.z = As<Short4>(UnpackLow(c.z, c.y));
						c.x = As<Short4>(UnpackHigh(c.x, c.y));
						c.y = c.z;
						c.w = c.x;
						c.z = UnpackLow(As<Byte8>(c.z), As<Byte8>(c.z));
						c.y = UnpackHigh(As<Byte8>(c.y), As<Byte8>(c.y));
						c.x = UnpackLow(As<Byte8>(c.x), As<Byte8>(c.x));
						c.w = UnpackHigh(As<Byte8>(c.w), As<Byte8>(c.w));
						break;
					case FORMAT_A8B8G8R8:
					case FORMAT_A8B8G8R8I:
					case FORMAT_A8B8G8R8UI:
					case FORMAT_A8B8G8R8I_SNORM:
					case FORMAT_Q8W8V8U8:
						c.z = c.x;
						c.x = As<Short4>(UnpackLow(c.x, c.y));
						c.z = As<Short4>(UnpackHigh(c.z, c.y));
						c.y = c.x;
						c.w = c.z;
						c.x = UnpackLow(As<Byte8>(c.x), As<Byte8>(c.x));
						c.y = UnpackHigh(As<Byte8>(c.y), As<Byte8>(c.y));
						c.z = UnpackLow(As<Byte8>(c.z), As<Byte8>(c.z));
						c.w = UnpackHigh(As<Byte8>(c.w), As<Byte8>(c.w));
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
					c.x = UnpackLow(c0, c1);
					c.y = UnpackLow(c2, c3);

					switch(state.textureFormat)
					{
					case FORMAT_X8R8G8B8:
						c.z = c.x;
						c.z = As<Short4>(UnpackLow(c.z, c.y));
						c.x = As<Short4>(UnpackHigh(c.x, c.y));
						c.y = c.z;
						c.z = UnpackLow(As<Byte8>(c.z), As<Byte8>(c.z));
						c.y = UnpackHigh(As<Byte8>(c.y), As<Byte8>(c.y));
						c.x = UnpackLow(As<Byte8>(c.x), As<Byte8>(c.x));
						break;
					case FORMAT_X8B8G8R8I_SNORM:
					case FORMAT_X8B8G8R8UI:
					case FORMAT_X8B8G8R8I:
					case FORMAT_X8B8G8R8:
					case FORMAT_X8L8V8U8:
						c.z = c.x;
						c.x = As<Short4>(UnpackLow(c.x, c.y));
						c.z = As<Short4>(UnpackHigh(c.z, c.y));
						c.y = c.x;
						c.x = UnpackLow(As<Byte8>(c.x), As<Byte8>(c.x));
						c.y = UnpackHigh(As<Byte8>(c.y), As<Byte8>(c.y));
						c.z = UnpackLow(As<Byte8>(c.z), As<Byte8>(c.z));
						break;
					default:
						ASSERT(false);
					}
				}
				break;
			case 2:
				c.x = Insert(c.x, *Pointer<Short>(buffer[f0] + 2 * index[0]), 0);
				c.x = Insert(c.x, *Pointer<Short>(buffer[f1] + 2 * index[1]), 1);
				c.x = Insert(c.x, *Pointer<Short>(buffer[f2] + 2 * index[2]), 2);
				c.x = Insert(c.x, *Pointer<Short>(buffer[f3] + 2 * index[3]), 3);

				switch(state.textureFormat)
				{
				case FORMAT_G8R8:
				case FORMAT_G8R8I:
				case FORMAT_G8R8UI:
				case FORMAT_G8R8I_SNORM:
				case FORMAT_V8U8:
				case FORMAT_A8L8:
					c.y = (c.x & Short4(0xFF00u, 0xFF00u, 0xFF00u, 0xFF00u)) | As<Short4>(As<UShort4>(c.x) >> 8);
					c.x = (c.x & Short4(0x00FFu, 0x00FFu, 0x00FFu, 0x00FFu)) | (c.x << 8);
					break;
				default:
					ASSERT(false);
				}
				break;
			case 1:
				{
					Int c0 = Int(*Pointer<Byte>(buffer[f0] + index[0]));
					Int c1 = Int(*Pointer<Byte>(buffer[f1] + index[1]));
					Int c2 = Int(*Pointer<Byte>(buffer[f2] + index[2]));
					Int c3 = Int(*Pointer<Byte>(buffer[f3] + index[3]));
					c0 = c0 | (c1 << 8) | (c2 << 16) | (c3 << 24);
					c.x = Unpack(As<Byte4>(c0));
				}
				break;
			default:
				ASSERT(false);
			}
		}
		else if(has16bitTextureComponents())
		{
			switch(textureComponentCount())
			{
			case 4:
				c.x = *Pointer<Short4>(buffer[f0] + 8 * index[0]);
				c.y = *Pointer<Short4>(buffer[f1] + 8 * index[1]);
				c.z = *Pointer<Short4>(buffer[f2] + 8 * index[2]);
				c.w = *Pointer<Short4>(buffer[f3] + 8 * index[3]);
				transpose4x4(c.x, c.y, c.z, c.w);
				break;
			case 2:
				c.x = *Pointer<Short4>(buffer[f0] + 4 * index[0]);
				c.x = As<Short4>(UnpackLow(c.x, *Pointer<Short4>(buffer[f1] + 4 * index[1])));
				c.z = *Pointer<Short4>(buffer[f2] + 4 * index[2]);
				c.z = As<Short4>(UnpackLow(c.z, *Pointer<Short4>(buffer[f3] + 4 * index[3])));
				c.y = c.x;
				c.x = As<Short4>(UnpackLow(As<Int2>(c.x), As<Int2>(c.z)));
				c.y = As<Short4>(UnpackHigh(As<Int2>(c.y), As<Int2>(c.z)));
				break;
			case 1:
				c.x = Insert(c.x, *Pointer<Short>(buffer[f0] + 2 * index[0]), 0);
				c.x = Insert(c.x, *Pointer<Short>(buffer[f1] + 2 * index[1]), 1);
				c.x = Insert(c.x, *Pointer<Short>(buffer[f2] + 2 * index[2]), 2);
				c.x = Insert(c.x, *Pointer<Short>(buffer[f3] + 2 * index[3]), 3);
				break;
			default:
				ASSERT(false);
			}
		}
		else if(hasYuvFormat())
		{
			// Generic YPbPr to RGB transformation
			// R = Y                               +           2 * (1 - Kr) * Pr
			// G = Y - 2 * Kb * (1 - Kb) / Kg * Pb - 2 * Kr * (1 - Kr) / Kg * Pr
			// B = Y +           2 * (1 - Kb) * Pb

			float Kb = 0.114f;
			float Kr = 0.299f;
			int studioSwing = 1;

			switch(state.textureFormat)
			{
			case FORMAT_YV12_BT601:
				Kb = 0.114f;
				Kr = 0.299f;
				studioSwing = 1;
				break;
			case FORMAT_YV12_BT709:
				Kb = 0.0722f;
				Kr = 0.2126f;
				studioSwing = 1;
				break;
			case FORMAT_YV12_JFIF:
				Kb = 0.114f;
				Kr = 0.299f;
				studioSwing = 0;
				break;
			default:
				ASSERT(false);
			}

			const float Kg = 1.0f - Kr - Kb;

			const float Rr = 2 * (1 - Kr);
			const float Gb = -2 * Kb * (1 - Kb) / Kg;
			const float Gr = -2 * Kr * (1 - Kr) / Kg;
			const float Bb = 2 * (1 - Kb);

			// Scaling and bias for studio-swing range: Y = [16 .. 235], U/V = [16 .. 240]
			const float Yy = studioSwing ? 255.0f / (235 - 16) : 1.0f;
			const float Uu = studioSwing ? 255.0f / (240 - 16) : 1.0f;
			const float Vv = studioSwing ? 255.0f / (240 - 16) : 1.0f;

			const float Rv = Vv *  Rr;
			const float Gu = Uu *  Gb;
			const float Gv = Vv *  Gr;
			const float Bu = Uu *  Bb;

			const float R0 = (studioSwing * -16 * Yy - 128 * Rv) / 255;
			const float G0 = (studioSwing * -16 * Yy - 128 * Gu - 128 * Gv) / 255;
			const float B0 = (studioSwing * -16 * Yy - 128 * Bu) / 255;

			Int c0 = Int(*Pointer<Byte>(buffer[0] + index[0]));
			Int c1 = Int(*Pointer<Byte>(buffer[0] + index[1]));
			Int c2 = Int(*Pointer<Byte>(buffer[0] + index[2]));
			Int c3 = Int(*Pointer<Byte>(buffer[0] + index[3]));
			c0 = c0 | (c1 << 8) | (c2 << 16) | (c3 << 24);
			UShort4 Y = As<UShort4>(Unpack(As<Byte4>(c0)));

			computeIndices(index, uuuu, vvvv, wwww, mipmap + sizeof(Mipmap));
			c0 = Int(*Pointer<Byte>(buffer[1] + index[0]));
			c1 = Int(*Pointer<Byte>(buffer[1] + index[1]));
			c2 = Int(*Pointer<Byte>(buffer[1] + index[2]));
			c3 = Int(*Pointer<Byte>(buffer[1] + index[3]));
			c0 = c0 | (c1 << 8) | (c2 << 16) | (c3 << 24);
			UShort4 V = As<UShort4>(Unpack(As<Byte4>(c0)));

			c0 = Int(*Pointer<Byte>(buffer[2] + index[0]));
			c1 = Int(*Pointer<Byte>(buffer[2] + index[1]));
			c2 = Int(*Pointer<Byte>(buffer[2] + index[2]));
			c3 = Int(*Pointer<Byte>(buffer[2] + index[3]));
			c0 = c0 | (c1 << 8) | (c2 << 16) | (c3 << 24);
			UShort4 U = As<UShort4>(Unpack(As<Byte4>(c0)));

			const UShort4 yY = UShort4(iround(Yy * 0x4000));
			const UShort4 rV = UShort4(iround(Rv * 0x4000));
			const UShort4 gU = UShort4(iround(-Gu * 0x4000));
			const UShort4 gV = UShort4(iround(-Gv * 0x4000));
			const UShort4 bU = UShort4(iround(Bu * 0x4000));

			const UShort4 r0 = UShort4(iround(-R0 * 0x4000));
			const UShort4 g0 = UShort4(iround(G0 * 0x4000));
			const UShort4 b0 = UShort4(iround(-B0 * 0x4000));

			UShort4 y = MulHigh(Y, yY);
			UShort4 r = SubSat(y + MulHigh(V, rV), r0);
			UShort4 g = SubSat(y + g0, MulHigh(U, gU) + MulHigh(V, gV));
			UShort4 b = SubSat(y + MulHigh(U, bU), b0);

			c.x = Min(r, UShort4(0x3FFF)) << 2;
			c.y = Min(g, UShort4(0x3FFF)) << 2;
			c.z = Min(b, UShort4(0x3FFF)) << 2;
		}
		else ASSERT(false);
	}

	void SamplerCore::sampleTexel(Vector4f &c, Short4 &uuuu, Short4 &vvvv, Short4 &wwww, Float4 &z, Pointer<Byte> &mipmap, Pointer<Byte> buffer[4])
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
			c.x = *Pointer<Float4>(buffer[f0] + index[0] * 16, 16);
			c.y = *Pointer<Float4>(buffer[f1] + index[1] * 16, 16);
			c.z = *Pointer<Float4>(buffer[f2] + index[2] * 16, 16);
			c.w = *Pointer<Float4>(buffer[f3] + index[3] * 16, 16);
			transpose4x4(c.x, c.y, c.z, c.w);
			break;
		case 2:
			// FIXME: Optimal shuffling?
			c.x.xy = *Pointer<Float4>(buffer[f0] + index[0] * 8);
			c.x.zw = *Pointer<Float4>(buffer[f1] + index[1] * 8 - 8);
			c.z.xy = *Pointer<Float4>(buffer[f2] + index[2] * 8);
			c.z.zw = *Pointer<Float4>(buffer[f3] + index[3] * 8 - 8);
			c.y = c.x;
			c.x = Float4(c.x.xz, c.z.xz);
			c.y = Float4(c.y.yw, c.z.yw);
			break;
		case 1:
			// FIXME: Optimal shuffling?
			c.x.x = *Pointer<Float>(buffer[f0] + index[0] * 4);
			c.x.y = *Pointer<Float>(buffer[f1] + index[1] * 4);
			c.x.z = *Pointer<Float>(buffer[f2] + index[2] * 4);
			c.x.w = *Pointer<Float>(buffer[f3] + index[3] * 4);

			if(state.textureFormat == FORMAT_D32FS8_SHADOW && state.textureFilter != FILTER_GATHER)
			{
				Float4 d = Min(Max(z, Float4(0.0f)), Float4(1.0f));

				c.x = As<Float4>(As<Int4>(CmpNLT(c.x, d)) & As<Int4>(Float4(1.0f)));   // FIXME: Only less-equal?
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
			buffer[0] = *Pointer<Pointer<Byte> >(mipmap + OFFSET(Mipmap,buffer[0]));

			if(hasYuvFormat())
			{
				buffer[1] = *Pointer<Pointer<Byte> >(mipmap + OFFSET(Mipmap,buffer[1]));
				buffer[2] = *Pointer<Pointer<Byte> >(mipmap + OFFSET(Mipmap,buffer[2]));
			}
		}
		else
		{
			for(int i = 0; i < 4; i++)
			{
				buffer[i] = *Pointer<Pointer<Byte> >(mipmap + OFFSET(Mipmap,buffer) + face[i] * sizeof(void*));
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

	void SamplerCore::addressW(Short4 &wwww, Float4 &w, Pointer<Byte>& mipmap)
	{
		if(state.textureType == TEXTURE_2D_ARRAY)
		{
			wwww = Min(Max(Short4(RoundInt(w)), Short4(0)), *Pointer<Short4>(mipmap + OFFSET(Mipmap, depth)) - Short4(1));
		}
		else
		{
			address(wwww, w, state.addressingModeW);
		}
	}

	void SamplerCore::convertFixed12(Short4 &cs, Float4 &cf)
	{
		cs = RoundShort4(cf * Float4(0x1000));
	}

	void SamplerCore::convertFixed12(Vector4s &cs, Vector4f &cf)
	{
		convertFixed12(cs.x, cf.x);
		convertFixed12(cs.y, cf.y);
		convertFixed12(cs.z, cf.z);
		convertFixed12(cs.w, cf.w);
	}

	void SamplerCore::convertSigned12(Float4 &cf, Short4 &cs)
	{
		cf = Float4(cs) * Float4(1.0f / 0x0FFE);
	}

//	void SamplerCore::convertSigned12(Vector4f &cf, Vector4s &cs)
//	{
//		convertSigned12(cf.x, cs.x);
//		convertSigned12(cf.y, cs.y);
//		convertSigned12(cf.z, cs.z);
//		convertSigned12(cf.w, cs.w);
//	}

	void SamplerCore::convertSigned15(Float4 &cf, Short4 &cs)
	{
		cf = Float4(cs) * Float4(1.0f / 0x7FFF);
	}

	void SamplerCore::convertUnsigned16(Float4 &cf, Short4 &cs)
	{
		cf = Float4(As<UShort4>(cs)) * Float4(1.0f / 0xFFFF);
	}

	void SamplerCore::sRGBtoLinear16_8_12(Short4 &c)
	{
		c = As<UShort4>(c) >> 8;

		Pointer<Byte> LUT = Pointer<Byte>(constants + OFFSET(Constants,sRGBtoLinear8_12));

		c = Insert(c, *Pointer<Short>(LUT + 2 * Int(Extract(c, 0))), 0);
		c = Insert(c, *Pointer<Short>(LUT + 2 * Int(Extract(c, 1))), 1);
		c = Insert(c, *Pointer<Short>(LUT + 2 * Int(Extract(c, 2))), 2);
		c = Insert(c, *Pointer<Short>(LUT + 2 * Int(Extract(c, 3))), 3);
	}

	void SamplerCore::sRGBtoLinear16_6_12(Short4 &c)
	{
		c = As<UShort4>(c) >> 10;

		Pointer<Byte> LUT = Pointer<Byte>(constants + OFFSET(Constants,sRGBtoLinear6_12));

		c = Insert(c, *Pointer<Short>(LUT + 2 * Int(Extract(c, 0))), 0);
		c = Insert(c, *Pointer<Short>(LUT + 2 * Int(Extract(c, 1))), 1);
		c = Insert(c, *Pointer<Short>(LUT + 2 * Int(Extract(c, 2))), 2);
		c = Insert(c, *Pointer<Short>(LUT + 2 * Int(Extract(c, 3))), 3);
	}

	void SamplerCore::sRGBtoLinear16_5_12(Short4 &c)
	{
		c = As<UShort4>(c) >> 11;

		Pointer<Byte> LUT = Pointer<Byte>(constants + OFFSET(Constants,sRGBtoLinear5_12));

		c = Insert(c, *Pointer<Short>(LUT + 2 * Int(Extract(c, 0))), 0);
		c = Insert(c, *Pointer<Short>(LUT + 2 * Int(Extract(c, 1))), 1);
		c = Insert(c, *Pointer<Short>(LUT + 2 * Int(Extract(c, 2))), 2);
		c = Insert(c, *Pointer<Short>(LUT + 2 * Int(Extract(c, 3))), 3);
	}

	bool SamplerCore::hasFloatTexture() const
	{
		return Surface::isFloatFormat(state.textureFormat);
	}

	bool SamplerCore::hasUnsignedTextureComponent(int component) const
	{
		return Surface::isUnsignedComponent(state.textureFormat, component);
	}

	int SamplerCore::textureComponentCount() const
	{
		return Surface::componentCount(state.textureFormat);
	}

	bool SamplerCore::has16bitTextureFormat() const
	{
		switch(state.textureFormat)
		{
		case FORMAT_R5G6B5:
			return true;
		case FORMAT_R8I_SNORM:
		case FORMAT_G8R8I_SNORM:
		case FORMAT_X8B8G8R8I_SNORM:
		case FORMAT_A8B8G8R8I_SNORM:
		case FORMAT_R8I:
		case FORMAT_R8UI:
		case FORMAT_G8R8I:
		case FORMAT_G8R8UI:
		case FORMAT_X8B8G8R8I:
		case FORMAT_X8B8G8R8UI:
		case FORMAT_A8B8G8R8I:
		case FORMAT_A8B8G8R8UI:
		case FORMAT_R32I:
		case FORMAT_R32UI:
		case FORMAT_G32R32I:
		case FORMAT_G32R32UI:
		case FORMAT_X32B32G32R32I:
		case FORMAT_X32B32G32R32UI:
		case FORMAT_A32B32G32R32I:
		case FORMAT_A32B32G32R32UI:
		case FORMAT_G8R8:
		case FORMAT_X8R8G8B8:
		case FORMAT_X8B8G8R8:
		case FORMAT_A8R8G8B8:
		case FORMAT_A8B8G8R8:
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
		case FORMAT_D32F:
		case FORMAT_D32F_LOCKABLE:
		case FORMAT_D32FS8_TEXTURE:
		case FORMAT_D32FS8_SHADOW:
		case FORMAT_L16:
		case FORMAT_G16R16:
		case FORMAT_A16B16G16R16:
		case FORMAT_V16U16:
		case FORMAT_A16W16V16U16:
		case FORMAT_Q16W16V16U16:
		case FORMAT_R16I:
		case FORMAT_R16UI:
		case FORMAT_G16R16I:
		case FORMAT_G16R16UI:
		case FORMAT_X16B16G16R16I:
		case FORMAT_X16B16G16R16UI:
		case FORMAT_A16B16G16R16I:
		case FORMAT_A16B16G16R16UI:
		case FORMAT_YV12_BT601:
		case FORMAT_YV12_BT709:
		case FORMAT_YV12_JFIF:
			return false;
		default:
			ASSERT(false);
		}
		
		return false;
	}

	bool SamplerCore::has8bitTextureComponents() const
	{
		switch(state.textureFormat)
		{
		case FORMAT_G8R8:
		case FORMAT_X8R8G8B8:
		case FORMAT_X8B8G8R8:
		case FORMAT_A8R8G8B8:
		case FORMAT_A8B8G8R8:
		case FORMAT_V8U8:
		case FORMAT_Q8W8V8U8:
		case FORMAT_X8L8V8U8:
		case FORMAT_A8:
		case FORMAT_R8:
		case FORMAT_L8:
		case FORMAT_A8L8:
		case FORMAT_R8I_SNORM:
		case FORMAT_G8R8I_SNORM:
		case FORMAT_X8B8G8R8I_SNORM:
		case FORMAT_A8B8G8R8I_SNORM:
		case FORMAT_R8I:
		case FORMAT_R8UI:
		case FORMAT_G8R8I:
		case FORMAT_G8R8UI:
		case FORMAT_X8B8G8R8I:
		case FORMAT_X8B8G8R8UI:
		case FORMAT_A8B8G8R8I:
		case FORMAT_A8B8G8R8UI:
			return true;
		case FORMAT_R5G6B5:
		case FORMAT_R32F:
		case FORMAT_G32R32F:
		case FORMAT_A32B32G32R32F:
		case FORMAT_D32F:
		case FORMAT_D32F_LOCKABLE:
		case FORMAT_D32FS8_TEXTURE:
		case FORMAT_D32FS8_SHADOW:
		case FORMAT_L16:
		case FORMAT_G16R16:
		case FORMAT_A16B16G16R16:
		case FORMAT_V16U16:
		case FORMAT_A16W16V16U16:
		case FORMAT_Q16W16V16U16:
		case FORMAT_R32I:
		case FORMAT_R32UI:
		case FORMAT_G32R32I:
		case FORMAT_G32R32UI:
		case FORMAT_X32B32G32R32I:
		case FORMAT_X32B32G32R32UI:
		case FORMAT_A32B32G32R32I:
		case FORMAT_A32B32G32R32UI:
		case FORMAT_R16I:
		case FORMAT_R16UI:
		case FORMAT_G16R16I:
		case FORMAT_G16R16UI:
		case FORMAT_X16B16G16R16I:
		case FORMAT_X16B16G16R16UI:
		case FORMAT_A16B16G16R16I:
		case FORMAT_A16B16G16R16UI:
		case FORMAT_YV12_BT601:
		case FORMAT_YV12_BT709:
		case FORMAT_YV12_JFIF:
			return false;
		default:
			ASSERT(false);
		}
		
		return false;
	}

	bool SamplerCore::has16bitTextureComponents() const
	{
		switch(state.textureFormat)
		{
		case FORMAT_R5G6B5:
		case FORMAT_R8I_SNORM:
		case FORMAT_G8R8I_SNORM:
		case FORMAT_X8B8G8R8I_SNORM:
		case FORMAT_A8B8G8R8I_SNORM:
		case FORMAT_R8I:
		case FORMAT_R8UI:
		case FORMAT_G8R8I:
		case FORMAT_G8R8UI:
		case FORMAT_X8B8G8R8I:
		case FORMAT_X8B8G8R8UI:
		case FORMAT_A8B8G8R8I:
		case FORMAT_A8B8G8R8UI:
		case FORMAT_R32I:
		case FORMAT_R32UI:
		case FORMAT_G32R32I:
		case FORMAT_G32R32UI:
		case FORMAT_X32B32G32R32I:
		case FORMAT_X32B32G32R32UI:
		case FORMAT_A32B32G32R32I:
		case FORMAT_A32B32G32R32UI:
		case FORMAT_G8R8:
		case FORMAT_X8R8G8B8:
		case FORMAT_X8B8G8R8:
		case FORMAT_A8R8G8B8:
		case FORMAT_A8B8G8R8:
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
		case FORMAT_D32F:
		case FORMAT_D32F_LOCKABLE:
		case FORMAT_D32FS8_TEXTURE:
		case FORMAT_D32FS8_SHADOW:
		case FORMAT_YV12_BT601:
		case FORMAT_YV12_BT709:
		case FORMAT_YV12_JFIF:
			return false;
		case FORMAT_L16:
		case FORMAT_G16R16:
		case FORMAT_A16B16G16R16:
		case FORMAT_R16I:
		case FORMAT_R16UI:
		case FORMAT_G16R16I:
		case FORMAT_G16R16UI:
		case FORMAT_X16B16G16R16I:
		case FORMAT_X16B16G16R16UI:
		case FORMAT_A16B16G16R16I:
		case FORMAT_A16B16G16R16UI:
		case FORMAT_V16U16:
		case FORMAT_A16W16V16U16:
		case FORMAT_Q16W16V16U16:
			return true;
		default:
			ASSERT(false);
		}
		
		return false;
	}

	bool SamplerCore::hasYuvFormat() const
	{
		switch(state.textureFormat)
		{
		case FORMAT_YV12_BT601:
		case FORMAT_YV12_BT709:
		case FORMAT_YV12_JFIF:
			return true;
		case FORMAT_R5G6B5:
		case FORMAT_R8I_SNORM:
		case FORMAT_G8R8I_SNORM:
		case FORMAT_X8B8G8R8I_SNORM:
		case FORMAT_A8B8G8R8I_SNORM:
		case FORMAT_R8I:
		case FORMAT_R8UI:
		case FORMAT_G8R8I:
		case FORMAT_G8R8UI:
		case FORMAT_X8B8G8R8I:
		case FORMAT_X8B8G8R8UI:
		case FORMAT_A8B8G8R8I:
		case FORMAT_A8B8G8R8UI:
		case FORMAT_R32I:
		case FORMAT_R32UI:
		case FORMAT_G32R32I:
		case FORMAT_G32R32UI:
		case FORMAT_X32B32G32R32I:
		case FORMAT_X32B32G32R32UI:
		case FORMAT_A32B32G32R32I:
		case FORMAT_A32B32G32R32UI:
		case FORMAT_G8R8:
		case FORMAT_X8R8G8B8:
		case FORMAT_X8B8G8R8:
		case FORMAT_A8R8G8B8:
		case FORMAT_A8B8G8R8:
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
		case FORMAT_D32F:
		case FORMAT_D32F_LOCKABLE:
		case FORMAT_D32FS8_TEXTURE:
		case FORMAT_D32FS8_SHADOW:
		case FORMAT_L16:
		case FORMAT_G16R16:
		case FORMAT_A16B16G16R16:
		case FORMAT_R16I:
		case FORMAT_R16UI:
		case FORMAT_G16R16I:
		case FORMAT_G16R16UI:
		case FORMAT_X16B16G16R16I:
		case FORMAT_X16B16G16R16UI:
		case FORMAT_A16B16G16R16I:
		case FORMAT_A16B16G16R16UI:
		case FORMAT_V16U16:
		case FORMAT_A16W16V16U16:
		case FORMAT_Q16W16V16U16:
			return false;
		default:
			ASSERT(false);
		}
		
		return false;
	}

	bool SamplerCore::isRGBComponent(int component) const
	{
		switch(state.textureFormat)
		{
		case FORMAT_R5G6B5:         return component < 3;
		case FORMAT_R8I_SNORM:      return component < 1;
		case FORMAT_G8R8I_SNORM:    return component < 2;
		case FORMAT_X8B8G8R8I_SNORM: return component < 3;
		case FORMAT_A8B8G8R8I_SNORM: return component < 3;
		case FORMAT_R8I:            return component < 1;
		case FORMAT_R8UI:           return component < 1;
		case FORMAT_G8R8I:          return component < 2;
		case FORMAT_G8R8UI:         return component < 2;
		case FORMAT_X8B8G8R8I:      return component < 3;
		case FORMAT_X8B8G8R8UI:     return component < 3;
		case FORMAT_A8B8G8R8I:      return component < 3;
		case FORMAT_A8B8G8R8UI:     return component < 3;
		case FORMAT_R32I:           return component < 1;
		case FORMAT_R32UI:          return component < 1;
		case FORMAT_G32R32I:        return component < 2;
		case FORMAT_G32R32UI:       return component < 2;
		case FORMAT_X32B32G32R32I:  return component < 3;
		case FORMAT_X32B32G32R32UI: return component < 3;
		case FORMAT_A32B32G32R32I:  return component < 3;
		case FORMAT_A32B32G32R32UI: return component < 3;
		case FORMAT_G8R8:           return component < 2;
		case FORMAT_X8R8G8B8:       return component < 3;
		case FORMAT_X8B8G8R8:       return component < 3;
		case FORMAT_A8R8G8B8:       return component < 3;
		case FORMAT_A8B8G8R8:       return component < 3;
		case FORMAT_V8U8:           return false;
		case FORMAT_Q8W8V8U8:       return false;
		case FORMAT_X8L8V8U8:       return false;
		case FORMAT_R32F:           return component < 1;
		case FORMAT_G32R32F:        return component < 2;
		case FORMAT_A32B32G32R32F:  return component < 3;
		case FORMAT_A8:             return false;
		case FORMAT_R8:             return component < 1;
		case FORMAT_L8:             return component < 1;
		case FORMAT_A8L8:           return component < 1;
		case FORMAT_D32F:           return false;
		case FORMAT_D32F_LOCKABLE:  return false;
		case FORMAT_D32FS8_TEXTURE: return false;
		case FORMAT_D32FS8_SHADOW:  return false;
		case FORMAT_L16:            return component < 1;
		case FORMAT_G16R16:         return component < 2;
		case FORMAT_A16B16G16R16:   return component < 3;
		case FORMAT_R16I:           return component < 1;
		case FORMAT_R16UI:          return component < 1;
		case FORMAT_G16R16I:        return component < 2;
		case FORMAT_G16R16UI:       return component < 2;
		case FORMAT_X16B16G16R16I:  return component < 3;
		case FORMAT_X16B16G16R16UI: return component < 3;
		case FORMAT_A16B16G16R16I:  return component < 3;
		case FORMAT_A16B16G16R16UI: return component < 3;
		case FORMAT_V16U16:         return false;
		case FORMAT_A16W16V16U16:   return false;
		case FORMAT_Q16W16V16U16:   return false;
		case FORMAT_YV12_BT601:     return component < 3;
		case FORMAT_YV12_BT709:     return component < 3;
		case FORMAT_YV12_JFIF:      return component < 3;
		default:
			ASSERT(false);
		}
		
		return false;
	}
}
