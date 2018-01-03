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

#include "Image.hpp"

#include "../libEGL/Context.hpp"
#include "../libEGL/Texture.hpp"
#include "../common/debug.h"
#include "Common/Math.hpp"
#include "Common/Thread.hpp"

#include <GLES3/gl3.h>

#include <string.h>

namespace
{
	int getNumBlocks(int w, int h, int blockSizeX, int blockSizeY)
	{
		return ((w + blockSizeX - 1) / blockSizeX) * ((h + blockSizeY - 1) / blockSizeY);
	}

	enum DataType
	{
		Bytes_1,
		Bytes_2,
		Bytes_4,
		Bytes_8,
		Bytes_16,
		ByteRGB,
		UByteRGB,
		ShortRGB,
		UShortRGB,
		IntRGB,
		UIntRGB,
		RGB565,
		FloatRGB,
		HalfFloatRGB,
		RGBA4444,
		RGBA5551,
		R11G11B10F,
		RGB9E5,
		D16,
		D24,
		D32,
		D32F,
		D32FS8,
		S8,
		S24_8,
	};

	template<DataType dataType>
	void LoadImageRow(const unsigned char *source, unsigned char *dest, GLint xoffset, GLsizei width)
	{
		UNIMPLEMENTED();
	}

	template<>
	void LoadImageRow<Bytes_1>(const unsigned char *source, unsigned char *dest, GLint xoffset, GLsizei width)
	{
		memcpy(dest + xoffset, source, width);
	}

	template<>
	void LoadImageRow<Bytes_2>(const unsigned char *source, unsigned char *dest, GLint xoffset, GLsizei width)
	{
		memcpy(dest + xoffset * 2, source, width * 2);
	}

	template<>
	void LoadImageRow<Bytes_4>(const unsigned char *source, unsigned char *dest, GLint xoffset, GLsizei width)
	{
		memcpy(dest + xoffset * 4, source, width * 4);
	}

	template<>
	void LoadImageRow<Bytes_8>(const unsigned char *source, unsigned char *dest, GLint xoffset, GLsizei width)
	{
		memcpy(dest + xoffset * 8, source, width * 8);
	}

	template<>
	void LoadImageRow<Bytes_16>(const unsigned char *source, unsigned char *dest, GLint xoffset, GLsizei width)
	{
		memcpy(dest + xoffset * 16, source, width * 16);
	}

	template<>
	void LoadImageRow<ByteRGB>(const unsigned char *source, unsigned char *dest, GLint xoffset, GLsizei width)
	{
		unsigned char *destB = dest + xoffset * 4;

		for(int x = 0; x < width; x++)
		{
			destB[4 * x + 0] = source[x * 3 + 0];
			destB[4 * x + 1] = source[x * 3 + 1];
			destB[4 * x + 2] = source[x * 3 + 2];
			destB[4 * x + 3] = 0x7F;
		}
	}

	template<>
	void LoadImageRow<UByteRGB>(const unsigned char *source, unsigned char *dest, GLint xoffset, GLsizei width)
	{
		unsigned char *destB = dest + xoffset * 4;

		for(int x = 0; x < width; x++)
		{
			destB[4 * x + 0] = source[x * 3 + 0];
			destB[4 * x + 1] = source[x * 3 + 1];
			destB[4 * x + 2] = source[x * 3 + 2];
			destB[4 * x + 3] = 0xFF;
		}
	}

	template<>
	void LoadImageRow<ShortRGB>(const unsigned char *source, unsigned char *dest, GLint xoffset, GLsizei width)
	{
		const unsigned short *sourceS = reinterpret_cast<const unsigned short*>(source);
		unsigned short *destS = reinterpret_cast<unsigned short*>(dest + xoffset * 8);

		for(int x = 0; x < width; x++)
		{
			destS[4 * x + 0] = sourceS[x * 3 + 0];
			destS[4 * x + 1] = sourceS[x * 3 + 1];
			destS[4 * x + 2] = sourceS[x * 3 + 2];
			destS[4 * x + 3] = 0x7FFF;
		}
	}

	template<>
	void LoadImageRow<UShortRGB>(const unsigned char *source, unsigned char *dest, GLint xoffset, GLsizei width)
	{
		const unsigned short *sourceS = reinterpret_cast<const unsigned short*>(source);
		unsigned short *destS = reinterpret_cast<unsigned short*>(dest + xoffset * 8);

		for(int x = 0; x < width; x++)
		{
			destS[4 * x + 0] = sourceS[x * 3 + 0];
			destS[4 * x + 1] = sourceS[x * 3 + 1];
			destS[4 * x + 2] = sourceS[x * 3 + 2];
			destS[4 * x + 3] = 0xFFFF;
		}
	}

	template<>
	void LoadImageRow<IntRGB>(const unsigned char *source, unsigned char *dest, GLint xoffset, GLsizei width)
	{
		const unsigned int *sourceI = reinterpret_cast<const unsigned int*>(source);
		unsigned int *destI = reinterpret_cast<unsigned int*>(dest + xoffset * 16);

		for(int x = 0; x < width; x++)
		{
			destI[4 * x + 0] = sourceI[x * 3 + 0];
			destI[4 * x + 1] = sourceI[x * 3 + 1];
			destI[4 * x + 2] = sourceI[x * 3 + 2];
			destI[4 * x + 3] = 0x7FFFFFFF;
		}
	}

	template<>
	void LoadImageRow<UIntRGB>(const unsigned char *source, unsigned char *dest, GLint xoffset, GLsizei width)
	{
		const unsigned int *sourceI = reinterpret_cast<const unsigned int*>(source);
		unsigned int *destI = reinterpret_cast<unsigned int*>(dest + xoffset * 16);

		for(int x = 0; x < width; x++)
		{
			destI[4 * x + 0] = sourceI[x * 3 + 0];
			destI[4 * x + 1] = sourceI[x * 3 + 1];
			destI[4 * x + 2] = sourceI[x * 3 + 2];
			destI[4 * x + 3] = 0xFFFFFFFF;
		}
	}

	template<>
	void LoadImageRow<RGB565>(const unsigned char *source, unsigned char *dest, GLint xoffset, GLsizei width)
	{
		memcpy(dest + xoffset * 2, source, width * 2);
	}

	template<>
	void LoadImageRow<FloatRGB>(const unsigned char *source, unsigned char *dest, GLint xoffset, GLsizei width)
	{
		const float *sourceF = reinterpret_cast<const float*>(source);
		float *destF = reinterpret_cast<float*>(dest + xoffset * 16);

		for(int x = 0; x < width; x++)
		{
			destF[4 * x + 0] = sourceF[x * 3 + 0];
			destF[4 * x + 1] = sourceF[x * 3 + 1];
			destF[4 * x + 2] = sourceF[x * 3 + 2];
			destF[4 * x + 3] = 1.0f;
		}
	}

	template<>
	void LoadImageRow<HalfFloatRGB>(const unsigned char *source, unsigned char *dest, GLint xoffset, GLsizei width)
	{
		const unsigned short *sourceH = reinterpret_cast<const unsigned short*>(source);
		unsigned short *destH = reinterpret_cast<unsigned short*>(dest + xoffset * 8);

		for(int x = 0; x < width; x++)
		{
			destH[4 * x + 0] = sourceH[x * 3 + 0];
			destH[4 * x + 1] = sourceH[x * 3 + 1];
			destH[4 * x + 2] = sourceH[x * 3 + 2];
			destH[4 * x + 3] = 0x3C00; // SEEEEEMMMMMMMMMM, S = 0, E = 15, M = 0: 16bit flpt representation of 1
		}
	}

	template<>
	void LoadImageRow<RGBA4444>(const unsigned char *source, unsigned char *dest, GLint xoffset, GLsizei width)
	{
		const unsigned short *source4444 = reinterpret_cast<const unsigned short*>(source);
		unsigned char *dest4444 = dest + xoffset * 4;

		for(int x = 0; x < width; x++)
		{
			unsigned short rgba = source4444[x];
			dest4444[4 * x + 0] = ((rgba & 0x00F0) << 0) | ((rgba & 0x00F0) >> 4);
			dest4444[4 * x + 1] = ((rgba & 0x0F00) >> 4) | ((rgba & 0x0F00) >> 8);
			dest4444[4 * x + 2] = ((rgba & 0xF000) >> 8) | ((rgba & 0xF000) >> 12);
			dest4444[4 * x + 3] = ((rgba & 0x000F) << 4) | ((rgba & 0x000F) >> 0);
		}
	}

	template<>
	void LoadImageRow<RGBA5551>(const unsigned char *source, unsigned char *dest, GLint xoffset, GLsizei width)
	{
		const unsigned short *source5551 = reinterpret_cast<const unsigned short*>(source);
		unsigned char *dest5551 = dest + xoffset * 4;

		for(int x = 0; x < width; x++)
		{
			unsigned short rgba = source5551[x];
			dest5551[4 * x + 0] = ((rgba & 0x003E) << 2) | ((rgba & 0x003E) >> 3);
			dest5551[4 * x + 1] = ((rgba & 0x07C0) >> 3) | ((rgba & 0x07C0) >> 8);
			dest5551[4 * x + 2] = ((rgba & 0xF800) >> 8) | ((rgba & 0xF800) >> 13);
			dest5551[4 * x + 3] = (rgba & 0x0001) ? 0xFF : 0;
		}
	}

	template<>
	void LoadImageRow<R11G11B10F>(const unsigned char *source, unsigned char *dest, GLint xoffset, GLsizei width)
	{
		const sw::R11G11B10F *sourceRGB = reinterpret_cast<const sw::R11G11B10F*>(source);
		sw::half *destF = reinterpret_cast<sw::half*>(dest + xoffset * 8);

		for(int x = 0; x < width; x++, sourceRGB++, destF+=4)
		{
			sourceRGB->toRGB16F(destF);
			destF[3] = 1.0f;
		}
	}

	template<>
	void LoadImageRow<RGB9E5>(const unsigned char *source, unsigned char *dest, GLint xoffset, GLsizei width)
	{
		const sw::RGB9E5 *sourceRGB = reinterpret_cast<const sw::RGB9E5*>(source);
		sw::half *destF = reinterpret_cast<sw::half*>(dest + xoffset * 8);

		for(int x = 0; x < width; x++, sourceRGB++, destF += 4)
		{
			sourceRGB->toRGB16F(destF);
			destF[3] = 1.0f;
		}
	}

	template<>
	void LoadImageRow<D16>(const unsigned char *source, unsigned char *dest, GLint xoffset, GLsizei width)
	{
		const unsigned short *sourceD16 = reinterpret_cast<const unsigned short*>(source);
		float *destF = reinterpret_cast<float*>(dest + xoffset * 4);

		for(int x = 0; x < width; x++)
		{
			destF[x] = (float)sourceD16[x] / 0xFFFF;
		}
	}

	template<>
	void LoadImageRow<D24>(const unsigned char *source, unsigned char *dest, GLint xoffset, GLsizei width)
	{
		const unsigned int *sourceD24 = reinterpret_cast<const unsigned int*>(source);
		float *destF = reinterpret_cast<float*>(dest + xoffset * 4);

		for(int x = 0; x < width; x++)
		{
			destF[x] = (float)(sourceD24[x] & 0xFFFFFF00) / 0xFFFFFF00;
		}
	}

	template<>
	void LoadImageRow<D32>(const unsigned char *source, unsigned char *dest, GLint xoffset, GLsizei width)
	{
		const unsigned int *sourceD32 = reinterpret_cast<const unsigned int*>(source);
		float *destF = reinterpret_cast<float*>(dest + xoffset * 4);

		for(int x = 0; x < width; x++)
		{
			destF[x] = (float)sourceD32[x] / 0xFFFFFFFF;
		}
	}

	template<>
	void LoadImageRow<S8>(const unsigned char *source, unsigned char *dest, GLint xoffset, GLsizei width)
	{
		const unsigned int *sourceI = reinterpret_cast<const unsigned int*>(source);
		unsigned char *destI = dest + xoffset;

		for(int x = 0; x < width; x++)
		{
			destI[x] = static_cast<unsigned char>(sourceI[x] & 0x000000FF);   // FIXME: Quad layout
		}
	}

	template<>
	void LoadImageRow<D32F>(const unsigned char *source, unsigned char *dest, GLint xoffset, GLsizei width)
	{
		const float *sourceF = reinterpret_cast<const float*>(source);
		float *destF = reinterpret_cast<float*>(dest + xoffset * 4);

		for(int x = 0; x < width; x++)
		{
			destF[x] = sw::clamp(sourceF[x], 0.0f, 1.0f);
		}
	}

	template<>
	void LoadImageRow<D32FS8>(const unsigned char *source, unsigned char *dest, GLint xoffset, GLsizei width)
	{
		struct D32FS8 { float depth32f; unsigned int stencil24_8; };
		const D32FS8 *sourceD32FS8 = reinterpret_cast<const D32FS8*>(source);
		float *destF = reinterpret_cast<float*>(dest + xoffset * 4);

		for(int x = 0; x < width; x++)
		{
			destF[x] = sw::clamp(sourceD32FS8[x].depth32f, 0.0f, 1.0f);
		}
	}

	template<>
	void LoadImageRow<S24_8>(const unsigned char *source, unsigned char *dest, GLint xoffset, GLsizei width)
	{
		struct D32FS8 { float depth32f; unsigned int stencil24_8; };
		const D32FS8 *sourceD32FS8 = reinterpret_cast<const D32FS8*>(source);
		unsigned char *destI = dest + xoffset;

		for(int x = 0; x < width; x++)
		{
			destI[x] = static_cast<unsigned char>(sourceD32FS8[x].stencil24_8 & 0x000000FF);   // FIXME: Quad layout
		}
	}

	template<DataType dataType>
	void LoadImageData(GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, int inputPitch, int inputHeight, int destPitch, GLsizei destSlice, const void *input, void *buffer)
	{
		for(int z = 0; z < depth; ++z)
		{
			const unsigned char *inputStart = static_cast<const unsigned char*>(input) + (z * inputPitch * inputHeight);
			unsigned char *destStart = static_cast<unsigned char*>(buffer) + ((zoffset + z) * destSlice);
			for(int y = 0; y < height; ++y)
			{
				const unsigned char *source = inputStart + y * inputPitch;
				unsigned char *dest = destStart + (y + yoffset) * destPitch;

				LoadImageRow<dataType>(source, dest, xoffset, width);
			}
		}
	}
}

namespace egl
{
	sw::Format ConvertFormatType(GLenum format, GLenum type)
	{
		switch(format)
		{
		case GL_LUMINANCE:
			switch(type)
			{
			case GL_UNSIGNED_BYTE:  return sw::FORMAT_L8;
			case GL_HALF_FLOAT:     return sw::FORMAT_L16F;
			case GL_HALF_FLOAT_OES: return sw::FORMAT_L16F;
			case GL_FLOAT:          return sw::FORMAT_L32F;
			default: UNREACHABLE(type);
			}
			break;
		case GL_LUMINANCE8_EXT:
			return sw::FORMAT_L8;
		case GL_LUMINANCE16F_EXT:
			return sw::FORMAT_L16F;
		case GL_LUMINANCE32F_EXT:
			return sw::FORMAT_L32F;
		case GL_LUMINANCE_ALPHA:
			switch(type)
			{
			case GL_UNSIGNED_BYTE:  return sw::FORMAT_A8L8;
			case GL_HALF_FLOAT:     return sw::FORMAT_A16L16F;
			case GL_HALF_FLOAT_OES: return sw::FORMAT_A16L16F;
			case GL_FLOAT:          return sw::FORMAT_A32L32F;
			default: UNREACHABLE(type);
			}
			break;
		case GL_LUMINANCE8_ALPHA8_EXT:
			return sw::FORMAT_A8L8;
		case GL_LUMINANCE_ALPHA16F_EXT:
			return sw::FORMAT_A16L16F;
		case GL_LUMINANCE_ALPHA32F_EXT:
			return sw::FORMAT_A32L32F;
		case GL_RGBA:
			switch(type)
			{
			case GL_UNSIGNED_BYTE:          return sw::FORMAT_A8B8G8R8;
			case GL_UNSIGNED_SHORT_4_4_4_4: return sw::FORMAT_R4G4B4A4;
			case GL_UNSIGNED_SHORT_5_5_5_1: return sw::FORMAT_R5G5B5A1;
			case GL_HALF_FLOAT:             return sw::FORMAT_A16B16G16R16F;
			case GL_HALF_FLOAT_OES:         return sw::FORMAT_A16B16G16R16F;
			case GL_FLOAT:                  return sw::FORMAT_A32B32G32R32F;
			default: UNREACHABLE(type);
			}
			break;
		case GL_BGRA_EXT:
		case GL_BGRA8_EXT:
			switch(type)
			{
			case GL_UNSIGNED_BYTE:                  return sw::FORMAT_A8R8G8B8;
			case GL_UNSIGNED_SHORT_4_4_4_4_REV_EXT: return sw::FORMAT_A4R4G4B4;
			case GL_UNSIGNED_SHORT_1_5_5_5_REV_EXT: return sw::FORMAT_A1R5G5B5;
			default: UNREACHABLE(type);
			}
			break;
		case GL_RGB:
			switch(type)
			{
			case GL_UNSIGNED_BYTE:          return sw::FORMAT_B8G8R8;
			case GL_UNSIGNED_SHORT_5_6_5:   return sw::FORMAT_R5G6B5;
			case GL_HALF_FLOAT:             return sw::FORMAT_B16G16R16F;
			case GL_HALF_FLOAT_OES:         return sw::FORMAT_B16G16R16F;
			case GL_FLOAT:                  return sw::FORMAT_B32G32R32F;
			default: UNREACHABLE(type);
			}
			break;
		case GL_RG:
			switch(type)
			{
			case GL_UNSIGNED_BYTE:          return sw::FORMAT_G8R8;
			case GL_HALF_FLOAT:             return sw::FORMAT_G16R16F;
			case GL_HALF_FLOAT_OES:         return sw::FORMAT_G16R16F;
			case GL_FLOAT:                  return sw::FORMAT_G32R32F;
			default: UNREACHABLE(type);
			}
			break;
		case GL_RED:
			switch(type)
			{
			case GL_UNSIGNED_BYTE:          return sw::FORMAT_R8;
			case GL_HALF_FLOAT:             return sw::FORMAT_R16F;
			case GL_HALF_FLOAT_OES:         return sw::FORMAT_R16F;
			case GL_FLOAT:                  return sw::FORMAT_R32F;
			default: UNREACHABLE(type);
			}
			break;
		case GL_ALPHA:
			switch(type)
			{
			case GL_UNSIGNED_BYTE:          return sw::FORMAT_A8;
			case GL_HALF_FLOAT:             return sw::FORMAT_A16F;
			case GL_HALF_FLOAT_OES:         return sw::FORMAT_A16F;
			case GL_FLOAT:                  return sw::FORMAT_A32F;
			default: UNREACHABLE(type);
			}
			break;
		case GL_ALPHA8_EXT:
			return sw::FORMAT_A8;
		case GL_ALPHA16F_EXT:
			return sw::FORMAT_A16F;
		case GL_ALPHA32F_EXT:
			return sw::FORMAT_A32F;
		case GL_RED_INTEGER:
			switch(type)
			{
			case GL_INT:          return sw::FORMAT_R32I;
			case GL_UNSIGNED_INT: return sw::FORMAT_R32UI;
			default: UNREACHABLE(type);
			}
			break;
		case GL_RG_INTEGER:
			switch(type)
			{
			case GL_INT:          return sw::FORMAT_G32R32I;
			case GL_UNSIGNED_INT: return sw::FORMAT_G32R32UI;
			default: UNREACHABLE(type);
			}
			break;
		case GL_RGBA_INTEGER:
			switch(type)
			{
			case GL_INT:          return sw::FORMAT_A32B32G32R32I;
			case GL_UNSIGNED_INT: return sw::FORMAT_A32B32G32R32UI;
			case GL_UNSIGNED_INT_2_10_10_10_REV: return sw::FORMAT_A2B10G10R10UI;
			default: UNREACHABLE(type);
			}
			break;
		case GL_DEPTH_COMPONENT:
			switch(type)
			{
			case GL_UNSIGNED_SHORT:        return sw::FORMAT_D16;
			case GL_UNSIGNED_INT_24_8_OES: return sw::FORMAT_D24S8;
			case GL_UNSIGNED_INT:          return sw::FORMAT_D32;
			case GL_FLOAT:                 return sw::FORMAT_D32F_LOCKABLE;
			default: UNREACHABLE(type);
			}
			break;
		default:
			UNREACHABLE(format);
		}

		return sw::FORMAT_NULL;
	}

	sw::Format SelectInternalFormat(GLenum format, GLenum type)
	{
		switch(format)
		{
		case GL_ETC1_RGB8_OES:
			return sw::FORMAT_ETC1;
		case GL_COMPRESSED_R11_EAC:
			return sw::FORMAT_R11_EAC;
		case GL_COMPRESSED_SIGNED_R11_EAC:
			return sw::FORMAT_SIGNED_R11_EAC;
		case GL_COMPRESSED_RG11_EAC:
			return sw::FORMAT_RG11_EAC;
		case GL_COMPRESSED_SIGNED_RG11_EAC:
			return sw::FORMAT_SIGNED_RG11_EAC;
		case GL_COMPRESSED_RGB8_ETC2:
			return sw::FORMAT_RGB8_ETC2;
		case GL_COMPRESSED_SRGB8_ETC2:
			return sw::FORMAT_SRGB8_ETC2;
		case GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2:
			return sw::FORMAT_RGB8_PUNCHTHROUGH_ALPHA1_ETC2;
		case GL_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2:
			return sw::FORMAT_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2;
		case GL_COMPRESSED_RGBA8_ETC2_EAC:
			return sw::FORMAT_RGBA8_ETC2_EAC;
		case GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC:
			return sw::FORMAT_SRGB8_ALPHA8_ETC2_EAC;
		case GL_COMPRESSED_RGBA_ASTC_4x4_KHR:
			return sw::FORMAT_RGBA_ASTC_4x4_KHR;
		case GL_COMPRESSED_RGBA_ASTC_5x4_KHR:
			return sw::FORMAT_RGBA_ASTC_5x4_KHR;
		case GL_COMPRESSED_RGBA_ASTC_5x5_KHR:
			return sw::FORMAT_RGBA_ASTC_5x5_KHR;
		case GL_COMPRESSED_RGBA_ASTC_6x5_KHR:
			return sw::FORMAT_RGBA_ASTC_6x5_KHR;
		case GL_COMPRESSED_RGBA_ASTC_6x6_KHR:
			return sw::FORMAT_RGBA_ASTC_6x6_KHR;
		case GL_COMPRESSED_RGBA_ASTC_8x5_KHR:
			return sw::FORMAT_RGBA_ASTC_8x5_KHR;
		case GL_COMPRESSED_RGBA_ASTC_8x6_KHR:
			return sw::FORMAT_RGBA_ASTC_8x6_KHR;
		case GL_COMPRESSED_RGBA_ASTC_8x8_KHR:
			return sw::FORMAT_RGBA_ASTC_8x8_KHR;
		case GL_COMPRESSED_RGBA_ASTC_10x5_KHR:
			return sw::FORMAT_RGBA_ASTC_10x5_KHR;
		case GL_COMPRESSED_RGBA_ASTC_10x6_KHR:
			return sw::FORMAT_RGBA_ASTC_10x6_KHR;
		case GL_COMPRESSED_RGBA_ASTC_10x8_KHR:
			return sw::FORMAT_RGBA_ASTC_10x8_KHR;
		case GL_COMPRESSED_RGBA_ASTC_10x10_KHR:
			return sw::FORMAT_RGBA_ASTC_10x10_KHR;
		case GL_COMPRESSED_RGBA_ASTC_12x10_KHR:
			return sw::FORMAT_RGBA_ASTC_12x10_KHR;
		case GL_COMPRESSED_RGBA_ASTC_12x12_KHR:
			return sw::FORMAT_RGBA_ASTC_12x12_KHR;
		case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR:
			return sw::FORMAT_SRGB8_ALPHA8_ASTC_4x4_KHR;
		case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x4_KHR:
			return sw::FORMAT_SRGB8_ALPHA8_ASTC_5x4_KHR;
		case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x5_KHR:
			return sw::FORMAT_SRGB8_ALPHA8_ASTC_5x5_KHR;
		case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x5_KHR:
			return sw::FORMAT_SRGB8_ALPHA8_ASTC_6x5_KHR;
		case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x6_KHR:
			return sw::FORMAT_SRGB8_ALPHA8_ASTC_6x6_KHR;
		case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x5_KHR:
			return sw::FORMAT_SRGB8_ALPHA8_ASTC_8x5_KHR;
		case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x6_KHR:
			return sw::FORMAT_SRGB8_ALPHA8_ASTC_8x6_KHR;
		case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x8_KHR:
			return sw::FORMAT_SRGB8_ALPHA8_ASTC_8x8_KHR;
		case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x5_KHR:
			return sw::FORMAT_SRGB8_ALPHA8_ASTC_10x5_KHR;
		case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x6_KHR:
			return sw::FORMAT_SRGB8_ALPHA8_ASTC_10x6_KHR;
		case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x8_KHR:
			return sw::FORMAT_SRGB8_ALPHA8_ASTC_10x8_KHR;
		case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x10_KHR:
			return sw::FORMAT_SRGB8_ALPHA8_ASTC_10x10_KHR;
		case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x10_KHR:
			return sw::FORMAT_SRGB8_ALPHA8_ASTC_12x10_KHR;
		case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x12_KHR:
			return sw::FORMAT_SRGB8_ALPHA8_ASTC_12x12_KHR;
		#if S3TC_SUPPORT
		case GL_COMPRESSED_RGB_S3TC_DXT1_EXT:
		case GL_COMPRESSED_RGBA_S3TC_DXT1_EXT:
			return sw::FORMAT_DXT1;
		case GL_COMPRESSED_RGBA_S3TC_DXT3_ANGLE:
			return sw::FORMAT_DXT3;
		case GL_COMPRESSED_RGBA_S3TC_DXT5_ANGLE:
			return sw::FORMAT_DXT5;
		#endif
		default:
			break;
		}

		switch(type)
		{
		case GL_FLOAT:
			switch(format)
			{
			case GL_ALPHA:
			case GL_ALPHA32F_EXT:
				return sw::FORMAT_A32F;
			case GL_LUMINANCE:
			case GL_LUMINANCE32F_EXT:
				return sw::FORMAT_L32F;
			case GL_LUMINANCE_ALPHA:
			case GL_LUMINANCE_ALPHA32F_EXT:
				return sw::FORMAT_A32L32F;
			case GL_RED:
			case GL_R32F:
				return sw::FORMAT_R32F;
			case GL_RG:
			case GL_RG32F:
				return sw::FORMAT_G32R32F;
			case GL_RGB:
			case GL_RGB32F:
				return sw::FORMAT_X32B32G32R32F;
			case GL_R11F_G11F_B10F:
			case GL_RGB9_E5:
				return sw::FORMAT_X16B16G16R16F_UNSIGNED;
			case GL_RGBA:
			case GL_RGBA32F:
				return sw::FORMAT_A32B32G32R32F;
			case GL_R16F:
				return sw::FORMAT_R16F;
			case GL_RG16F:
				return sw::FORMAT_G16R16F;
			case GL_RGB16F:
				return sw::FORMAT_X16B16G16R16F;
			case GL_RGBA16F:
				return sw::FORMAT_A16B16G16R16F;
			case GL_DEPTH_COMPONENT:
			case GL_DEPTH_COMPONENT32F:
				return sw::FORMAT_D32F;
			default:
				UNREACHABLE(format);
			}
		case GL_HALF_FLOAT:
		case GL_HALF_FLOAT_OES:
			switch(format)
			{
			case GL_ALPHA:
			case GL_ALPHA16F_EXT:
				return sw::FORMAT_A16F;
			case GL_LUMINANCE:
			case GL_LUMINANCE16F_EXT:
				return sw::FORMAT_L16F;
			case GL_LUMINANCE_ALPHA:
			case GL_LUMINANCE_ALPHA16F_EXT:
				return sw::FORMAT_A16L16F;
			case GL_RED:
			case GL_R16F:
				return sw::FORMAT_R16F;
			case GL_RG:
			case GL_RG16F:
				return sw::FORMAT_G16R16F;
			case GL_RGBA:
			case GL_RGBA16F:
				return sw::FORMAT_A16B16G16R16F;
			case GL_RGB:
			case GL_RGB16F:
				return sw::FORMAT_X16B16G16R16F;
			case GL_R11F_G11F_B10F:
			case GL_RGB9_E5:
				return sw::FORMAT_X16B16G16R16F_UNSIGNED;
			default:
				UNREACHABLE(format);
			}
		case GL_BYTE:
			switch(format)
			{
			case GL_R8_SNORM:
			case GL_R8:
			case GL_RED:
				return sw::FORMAT_R8_SNORM;
			case GL_R8I:
			case GL_RED_INTEGER:
				return sw::FORMAT_R8I;
			case GL_RG8_SNORM:
			case GL_RG8:
			case GL_RG:
				return sw::FORMAT_G8R8_SNORM;
			case GL_RG8I:
			case GL_RG_INTEGER:
				return sw::FORMAT_G8R8I;
			case GL_RGB8_SNORM:
			case GL_RGB8:
			case GL_RGB:
				return sw::FORMAT_X8B8G8R8_SNORM;
			case GL_RGB8I:
			case GL_RGB_INTEGER:
				return sw::FORMAT_X8B8G8R8I;
			case GL_RGBA8_SNORM:
			case GL_RGBA8:
			case GL_RGBA:
				return sw::FORMAT_A8B8G8R8_SNORM;
			case GL_RGBA8I:
			case GL_RGBA_INTEGER:
				return sw::FORMAT_A8B8G8R8I;
			default:
				UNREACHABLE(format);
			}
		case GL_UNSIGNED_BYTE:
			switch(format)
			{
			case GL_LUMINANCE:
			case GL_LUMINANCE8_EXT:
				return sw::FORMAT_L8;
			case GL_LUMINANCE_ALPHA:
			case GL_LUMINANCE8_ALPHA8_EXT:
				return sw::FORMAT_A8L8;
			case GL_R8_SNORM:
				return sw::FORMAT_R8_SNORM;
			case GL_R8:
			case GL_RED:
				return sw::FORMAT_R8;
			case GL_R8UI:
			case GL_RED_INTEGER:
				return sw::FORMAT_R8UI;
			case GL_RG8_SNORM:
			case GL_RG8:
			case GL_RG:
				return sw::FORMAT_G8R8;
			case GL_RG8UI:
			case GL_RG_INTEGER:
				return sw::FORMAT_G8R8UI;
			case GL_RGB8_SNORM:
			case GL_RGB8:
			case GL_RGB:
				return sw::FORMAT_X8B8G8R8;
			case GL_SRGB8:
				return sw::FORMAT_SRGB8_X8;
			case GL_RGB8UI:
			case GL_RGB_INTEGER:
				return sw::FORMAT_X8B8G8R8UI;
			case GL_RGBA8_SNORM:
			case GL_RGBA8:
			case GL_RGBA:
				return sw::FORMAT_A8B8G8R8;
			case GL_SRGB8_ALPHA8:
				return sw::FORMAT_SRGB8_A8;
			case GL_RGBA8UI:
			case GL_RGBA_INTEGER:
				return sw::FORMAT_A8B8G8R8UI;
			case GL_BGRA_EXT:
			case GL_BGRA8_EXT:
				return sw::FORMAT_A8R8G8B8;
			case GL_ALPHA:
			case GL_ALPHA8_EXT:
				return sw::FORMAT_A8;
			case SW_YV12_BT601:
				return sw::FORMAT_YV12_BT601;
			case SW_YV12_BT709:
				return sw::FORMAT_YV12_BT709;
			case SW_YV12_JFIF:
				return sw::FORMAT_YV12_JFIF;
			default:
				UNREACHABLE(format);
			}
		case GL_SHORT:
			switch(format)
			{
			case GL_R16I:
			case GL_RED_INTEGER:
				return sw::FORMAT_R16I;
			case GL_RG16I:
			case GL_RG_INTEGER:
				return sw::FORMAT_G16R16I;
			case GL_RGB16I:
			case GL_RGB_INTEGER:
				return sw::FORMAT_X16B16G16R16I;
			case GL_RGBA16I:
			case GL_RGBA_INTEGER:
				return sw::FORMAT_A16B16G16R16I;
			default:
				UNREACHABLE(format);
			}
		case GL_UNSIGNED_SHORT:
			switch(format)
			{
			case GL_R16UI:
			case GL_RED_INTEGER:
				return sw::FORMAT_R16UI;
			case GL_RG16UI:
			case GL_RG_INTEGER:
				return sw::FORMAT_G16R16UI;
			case GL_RGB16UI:
			case GL_RGB_INTEGER:
				return sw::FORMAT_X16B16G16R16UI;
			case GL_RGBA16UI:
			case GL_RGBA_INTEGER:
				return sw::FORMAT_A16B16G16R16UI;
			case GL_DEPTH_COMPONENT:
			case GL_DEPTH_COMPONENT16:
				return sw::FORMAT_D32F_LOCKABLE;
			default:
				UNREACHABLE(format);
			}
		case GL_INT:
			switch(format)
			{
			case GL_RED_INTEGER:
			case GL_R32I:
				return sw::FORMAT_R32I;
			case GL_RG_INTEGER:
			case GL_RG32I:
				return sw::FORMAT_G32R32I;
			case GL_RGB_INTEGER:
			case GL_RGB32I:
				return sw::FORMAT_X32B32G32R32I;
			case GL_RGBA_INTEGER:
			case GL_RGBA32I:
				return sw::FORMAT_A32B32G32R32I;
			default:
				UNREACHABLE(format);
			}
		case GL_UNSIGNED_INT:
			switch(format)
			{
			case GL_RED_INTEGER:
			case GL_R32UI:
				return sw::FORMAT_R32UI;
			case GL_RG_INTEGER:
			case GL_RG32UI:
				return sw::FORMAT_G32R32UI;
			case GL_RGB_INTEGER:
			case GL_RGB32UI:
				return sw::FORMAT_X32B32G32R32UI;
			case GL_RGBA_INTEGER:
			case GL_RGBA32UI:
				return sw::FORMAT_A32B32G32R32UI;
			case GL_DEPTH_COMPONENT:
			case GL_DEPTH_COMPONENT16:
			case GL_DEPTH_COMPONENT24:
			case GL_DEPTH_COMPONENT32_OES:
				return sw::FORMAT_D32F_LOCKABLE;
			default:
				UNREACHABLE(format);
			}
		case GL_UNSIGNED_INT_24_8_OES:
			if(format == GL_DEPTH_STENCIL || format == GL_DEPTH24_STENCIL8)
			{
				return sw::FORMAT_D32FS8_TEXTURE;
			}
			else UNREACHABLE(format);
		case GL_FLOAT_32_UNSIGNED_INT_24_8_REV:
			if(format == GL_DEPTH_STENCIL || format == GL_DEPTH32F_STENCIL8)
			{
				return sw::FORMAT_D32FS8_TEXTURE;
			}
			else UNREACHABLE(format);
		case GL_UNSIGNED_SHORT_4_4_4_4:
			return sw::FORMAT_A8R8G8B8;
		case GL_UNSIGNED_SHORT_5_5_5_1:
			return sw::FORMAT_A8R8G8B8;
		case GL_UNSIGNED_SHORT_5_6_5:
			return sw::FORMAT_R5G6B5;
		case GL_UNSIGNED_INT_2_10_10_10_REV:
			if(format == GL_RGB10_A2UI)
			{
				return sw::FORMAT_A2B10G10R10UI;
			}
			else
			{
				return sw::FORMAT_A2B10G10R10;
			}
		case GL_UNSIGNED_INT_10F_11F_11F_REV:
		case GL_UNSIGNED_INT_5_9_9_9_REV:   // 5 is the exponent field, not alpha.
			return sw::FORMAT_X16B16G16R16F_UNSIGNED;
		default:
			UNREACHABLE(type);
		}

		return sw::FORMAT_NULL;
	}

	// Returns the size, in bytes, of a single texel in an Image
	static int ComputePixelSize(GLenum format, GLenum type)
	{
		switch(type)
		{
		case GL_BYTE:
			switch(format)
			{
			case GL_R8:
			case GL_R8I:
			case GL_R8_SNORM:
			case GL_RED:             return sizeof(char);
			case GL_RED_INTEGER:     return sizeof(char);
			case GL_RG8:
			case GL_RG8I:
			case GL_RG8_SNORM:
			case GL_RG:              return sizeof(char) * 2;
			case GL_RG_INTEGER:      return sizeof(char) * 2;
			case GL_RGB8:
			case GL_RGB8I:
			case GL_RGB8_SNORM:
			case GL_RGB:             return sizeof(char) * 3;
			case GL_RGB_INTEGER:     return sizeof(char) * 3;
			case GL_RGBA8:
			case GL_RGBA8I:
			case GL_RGBA8_SNORM:
			case GL_RGBA:            return sizeof(char) * 4;
			case GL_RGBA_INTEGER:    return sizeof(char) * 4;
			default: UNREACHABLE(format);
			}
			break;
		case GL_UNSIGNED_BYTE:
			switch(format)
			{
			case GL_R8:
			case GL_R8UI:
			case GL_RED:             return sizeof(unsigned char);
			case GL_RED_INTEGER:     return sizeof(unsigned char);
			case GL_ALPHA8_EXT:
			case GL_ALPHA:           return sizeof(unsigned char);
			case GL_LUMINANCE8_EXT:
			case GL_LUMINANCE:       return sizeof(unsigned char);
			case GL_LUMINANCE8_ALPHA8_EXT:
			case GL_LUMINANCE_ALPHA: return sizeof(unsigned char) * 2;
			case GL_RG8:
			case GL_RG8UI:
			case GL_RG:              return sizeof(unsigned char) * 2;
			case GL_RG_INTEGER:      return sizeof(unsigned char) * 2;
			case GL_RGB8:
			case GL_RGB8UI:
			case GL_SRGB8:
			case GL_RGB:             return sizeof(unsigned char) * 3;
			case GL_RGB_INTEGER:     return sizeof(unsigned char) * 3;
			case GL_RGBA8:
			case GL_RGBA8UI:
			case GL_SRGB8_ALPHA8:
			case GL_RGBA:            return sizeof(unsigned char) * 4;
			case GL_RGBA_INTEGER:    return sizeof(unsigned char) * 4;
			case GL_BGRA_EXT:
			case GL_BGRA8_EXT:       return sizeof(unsigned char)* 4;
			default: UNREACHABLE(format);
			}
			break;
		case GL_SHORT:
			switch(format)
			{
			case GL_R16I:
			case GL_RED_INTEGER:     return sizeof(short);
			case GL_RG16I:
			case GL_RG_INTEGER:      return sizeof(short) * 2;
			case GL_RGB16I:
			case GL_RGB_INTEGER:     return sizeof(short) * 3;
			case GL_RGBA16I:
			case GL_RGBA_INTEGER:    return sizeof(short) * 4;
			default: UNREACHABLE(format);
			}
			break;
		case GL_UNSIGNED_SHORT:
			switch(format)
			{
			case GL_DEPTH_COMPONENT16:
			case GL_DEPTH_COMPONENT: return sizeof(unsigned short);
			case GL_R16UI:
			case GL_RED_INTEGER:     return sizeof(unsigned short);
			case GL_RG16UI:
			case GL_RG_INTEGER:      return sizeof(unsigned short) * 2;
			case GL_RGB16UI:
			case GL_RGB_INTEGER:     return sizeof(unsigned short) * 3;
			case GL_RGBA16UI:
			case GL_RGBA_INTEGER:    return sizeof(unsigned short) * 4;
			default: UNREACHABLE(format);
			}
			break;
		case GL_INT:
			switch(format)
			{
			case GL_R32I:
			case GL_RED_INTEGER:     return sizeof(int);
			case GL_RG32I:
			case GL_RG_INTEGER:      return sizeof(int) * 2;
			case GL_RGB32I:
			case GL_RGB_INTEGER:     return sizeof(int) * 3;
			case GL_RGBA32I:
			case GL_RGBA_INTEGER:    return sizeof(int) * 4;
			default: UNREACHABLE(format);
			}
			break;
		case GL_UNSIGNED_INT:
			switch(format)
			{
			case GL_DEPTH_COMPONENT16:
			case GL_DEPTH_COMPONENT24:
			case GL_DEPTH_COMPONENT32_OES:
			case GL_DEPTH_COMPONENT: return sizeof(unsigned int);
			case GL_R32UI:
			case GL_RED_INTEGER:     return sizeof(unsigned int);
			case GL_RG32UI:
			case GL_RG_INTEGER:      return sizeof(unsigned int) * 2;
			case GL_RGB32UI:
			case GL_RGB_INTEGER:     return sizeof(unsigned int) * 3;
			case GL_RGBA32UI:
			case GL_RGBA_INTEGER:    return sizeof(unsigned int) * 4;
			default: UNREACHABLE(format);
			}
			break;
		case GL_UNSIGNED_SHORT_4_4_4_4:
		case GL_UNSIGNED_SHORT_5_5_5_1:
		case GL_UNSIGNED_SHORT_5_6_5:
		case GL_UNSIGNED_SHORT_4_4_4_4_REV_EXT:
		case GL_UNSIGNED_SHORT_1_5_5_5_REV_EXT:
			return sizeof(unsigned short);
		case GL_UNSIGNED_INT_10F_11F_11F_REV:
		case GL_UNSIGNED_INT_5_9_9_9_REV:
		case GL_UNSIGNED_INT_2_10_10_10_REV:
		case GL_UNSIGNED_INT_24_8_OES:
			return sizeof(unsigned int);
		case GL_FLOAT_32_UNSIGNED_INT_24_8_REV:
			return sizeof(float) + sizeof(unsigned int);
		case GL_FLOAT:
			switch(format)
			{
			case GL_DEPTH_COMPONENT32F:
			case GL_DEPTH_COMPONENT: return sizeof(float);
			case GL_ALPHA32F_EXT:
			case GL_ALPHA:           return sizeof(float);
			case GL_LUMINANCE32F_EXT:
			case GL_LUMINANCE:       return sizeof(float);
			case GL_LUMINANCE_ALPHA32F_EXT:
			case GL_LUMINANCE_ALPHA: return sizeof(float) * 2;
			case GL_RED:             return sizeof(float);
			case GL_R32F:            return sizeof(float);
			case GL_RG:              return sizeof(float) * 2;
			case GL_RG32F:           return sizeof(float) * 2;
			case GL_RGB:             return sizeof(float) * 3;
			case GL_RGB32F:          return sizeof(float) * 3;
			case GL_RGBA:            return sizeof(float) * 4;
			case GL_RGBA32F:         return sizeof(float) * 4;
			case GL_R11F_G11F_B10F:  return sizeof(int);
			case GL_RGB9_E5:         return sizeof(int);
			default: UNREACHABLE(format);
			}
			break;
		case GL_HALF_FLOAT:
		case GL_HALF_FLOAT_OES:
			switch(format)
			{
			case GL_ALPHA16F_EXT:
			case GL_ALPHA:           return sizeof(unsigned short);
			case GL_LUMINANCE16F_EXT:
			case GL_LUMINANCE:       return sizeof(unsigned short);
			case GL_LUMINANCE_ALPHA16F_EXT:
			case GL_LUMINANCE_ALPHA: return sizeof(unsigned short) * 2;
			case GL_RED:             return sizeof(unsigned short);
			case GL_R16F:            return sizeof(unsigned short);
			case GL_RG:              return sizeof(unsigned short) * 2;
			case GL_RG16F:           return sizeof(unsigned short) * 2;
			case GL_RGB:             return sizeof(unsigned short) * 3;
			case GL_RGB16F:          return sizeof(unsigned short) * 3;
			case GL_RGBA:            return sizeof(unsigned short) * 4;
			case GL_RGBA16F:         return sizeof(unsigned short) * 4;
			case GL_R11F_G11F_B10F:  return sizeof(int);
			case GL_RGB9_E5:         return sizeof(int);
			default: UNREACHABLE(format);
			}
			break;
		default: UNREACHABLE(type);
		}

		return 0;
	}

	GLsizei ComputePitch(GLsizei width, GLenum format, GLenum type, GLint alignment)
	{
		ASSERT(alignment > 0 && sw::isPow2(alignment));

		GLsizei rawPitch = ComputePixelSize(format, type) * width;
		return (rawPitch + alignment - 1) & ~(alignment - 1);
	}

	size_t ComputePackingOffset(GLenum format, GLenum type, GLsizei width, GLsizei height, GLint alignment, GLint skipImages, GLint skipRows, GLint skipPixels)
	{
		GLsizei pitchB = ComputePitch(width, format, type, alignment);
		return (skipImages * height + skipRows) * pitchB + skipPixels * ComputePixelSize(format, type);
	}

	inline GLsizei ComputeCompressedPitch(GLsizei width, GLenum format)
	{
		return ComputeCompressedSize(width, 1, format);
	}

	GLsizei ComputeCompressedSize(GLsizei width, GLsizei height, GLenum format)
	{
		switch(format)
		{
		case GL_COMPRESSED_RGB_S3TC_DXT1_EXT:
		case GL_COMPRESSED_RGBA_S3TC_DXT1_EXT:
		case GL_ETC1_RGB8_OES:
		case GL_COMPRESSED_R11_EAC:
		case GL_COMPRESSED_SIGNED_R11_EAC:
		case GL_COMPRESSED_RGB8_ETC2:
		case GL_COMPRESSED_SRGB8_ETC2:
		case GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2:
		case GL_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2:
			return 8 * getNumBlocks(width, height, 4, 4);
		case GL_COMPRESSED_RGBA_S3TC_DXT3_ANGLE:
		case GL_COMPRESSED_RGBA_S3TC_DXT5_ANGLE:
		case GL_COMPRESSED_RG11_EAC:
		case GL_COMPRESSED_SIGNED_RG11_EAC:
		case GL_COMPRESSED_RGBA8_ETC2_EAC:
		case GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC:
		case GL_COMPRESSED_RGBA_ASTC_4x4_KHR:
		case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR:
			return 16 * getNumBlocks(width, height, 4, 4);
		case GL_COMPRESSED_RGBA_ASTC_5x4_KHR:
		case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x4_KHR:
			return 16 * getNumBlocks(width, height, 5, 4);
		case GL_COMPRESSED_RGBA_ASTC_5x5_KHR:
		case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x5_KHR:
			return 16 * getNumBlocks(width, height, 5, 5);
		case GL_COMPRESSED_RGBA_ASTC_6x5_KHR:
		case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x5_KHR:
			return 16 * getNumBlocks(width, height, 6, 5);
		case GL_COMPRESSED_RGBA_ASTC_6x6_KHR:
		case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x6_KHR:
			return 16 * getNumBlocks(width, height, 6, 6);
		case GL_COMPRESSED_RGBA_ASTC_8x5_KHR:
		case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x5_KHR:
			return 16 * getNumBlocks(width, height, 8, 5);
		case GL_COMPRESSED_RGBA_ASTC_8x6_KHR:
		case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x6_KHR:
			return 16 * getNumBlocks(width, height, 8, 6);
		case GL_COMPRESSED_RGBA_ASTC_8x8_KHR:
		case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x8_KHR:
			return 16 * getNumBlocks(width, height, 8, 8);
		case GL_COMPRESSED_RGBA_ASTC_10x5_KHR:
		case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x5_KHR:
			return 16 * getNumBlocks(width, height, 10, 5);
		case GL_COMPRESSED_RGBA_ASTC_10x6_KHR:
		case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x6_KHR:
			return 16 * getNumBlocks(width, height, 10, 6);
		case GL_COMPRESSED_RGBA_ASTC_10x8_KHR:
		case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x8_KHR:
			return 16 * getNumBlocks(width, height, 10, 8);
		case GL_COMPRESSED_RGBA_ASTC_10x10_KHR:
		case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x10_KHR:
			return 16 * getNumBlocks(width, height, 10, 10);
		case GL_COMPRESSED_RGBA_ASTC_12x10_KHR:
		case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x10_KHR:
			return 16 * getNumBlocks(width, height, 12, 10);
		case GL_COMPRESSED_RGBA_ASTC_12x12_KHR:
		case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x12_KHR:
			return 16 * getNumBlocks(width, height, 12, 12);
		default:
			return 0;
		}
	}

	class ImageImplementation : public Image
	{
	public:
		ImageImplementation(Texture *parentTexture, GLsizei width, GLsizei height, GLenum format, GLenum type)
			: Image(parentTexture, width, height, format, type) {}
		ImageImplementation(Texture *parentTexture, GLsizei width, GLsizei height, GLsizei depth, int border, GLenum format, GLenum type)
			: Image(parentTexture, width, height, depth, border, format, type) {}
		ImageImplementation(GLsizei width, GLsizei height, GLenum format, GLenum type, int pitchP)
			: Image(width, height, format, type, pitchP) {}
		ImageImplementation(GLsizei width, GLsizei height, sw::Format internalFormat, int multiSampleDepth, bool lockable)
			: Image(width, height, internalFormat, multiSampleDepth, lockable) {}

		~ImageImplementation() override
		{
			sync();   // Wait for any threads that use this image to finish.
		}

		void *lockInternal(int x, int y, int z, sw::Lock lock, sw::Accessor client) override
		{
			return Image::lockInternal(x, y, z, lock, client);
		}

		void unlockInternal() override
		{
			return Image::unlockInternal();
		}

		void release() override
		{
			return Image::release();
		}
	};

	Image *Image::create(Texture *parentTexture, GLsizei width, GLsizei height, GLenum format, GLenum type)
	{
		return new ImageImplementation(parentTexture, width, height, format, type);
	}

	Image *Image::create(Texture *parentTexture, GLsizei width, GLsizei height, GLsizei depth, int border, GLenum format, GLenum type)
	{
		return new ImageImplementation(parentTexture, width, height, depth, border, format, type);
	}

	Image *Image::create(GLsizei width, GLsizei height, GLenum format, GLenum type, int pitchP)
	{
		return new ImageImplementation(width, height, format, type, pitchP);
	}

	Image *Image::create(GLsizei width, GLsizei height, sw::Format internalFormat, int multiSampleDepth, bool lockable)
	{
		return new ImageImplementation(width, height, internalFormat, multiSampleDepth, lockable);
	}

	Image::~Image()
	{
		// sync() must be called in the destructor of the most derived class to ensure their vtable isn't destroyed
		// before all threads are done using this image. Image itself is abstract so it can't be the most derived.
		ASSERT(isUnlocked());

		if(parentTexture)
		{
			parentTexture->release();
		}

		ASSERT(!shared);
	}

	void *Image::lockInternal(int x, int y, int z, sw::Lock lock, sw::Accessor client)
	{
		return Surface::lockInternal(x, y, z, lock, client);
	}

	void Image::unlockInternal()
	{
		Surface::unlockInternal();
	}

	void Image::release()
	{
		int refs = dereference();

		if(refs > 0)
		{
			if(parentTexture)
			{
				parentTexture->sweep();
			}
		}
		else
		{
			delete this;
		}
	}

	void Image::unbind(const egl::Texture *parent)
	{
		if(parentTexture == parent)
		{
			parentTexture = nullptr;
		}

		release();
	}

	bool Image::isChildOf(const egl::Texture *parent) const
	{
		return parentTexture == parent;
	}

	void Image::loadImageData(Context *context, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const UnpackInfo& unpackInfo, const void *input)
	{
		sw::Format uploadFormat = SelectInternalFormat(format, type);
		if(uploadFormat == sw::FORMAT_NULL)
		{
			return;
		}

		GLsizei inputWidth = (unpackInfo.rowLength == 0) ? width : unpackInfo.rowLength;
		GLsizei inputPitch = ComputePitch(inputWidth, format, type, unpackInfo.alignment);
		GLsizei inputHeight = (unpackInfo.imageHeight == 0) ? height : unpackInfo.imageHeight;
		input = ((char*)input) + ComputePackingOffset(format, type, inputWidth, inputHeight, unpackInfo.alignment, unpackInfo.skipImages, unpackInfo.skipRows, unpackInfo.skipPixels);

		if(uploadFormat == internalFormat ||
		   (uploadFormat == sw::FORMAT_A8B8G8R8 && internalFormat == sw::FORMAT_SRGB8_A8) ||
		   (uploadFormat == sw::FORMAT_X8B8G8R8 && internalFormat == sw::FORMAT_SRGB8_X8) ||
		   (uploadFormat == sw::FORMAT_A2B10G10R10 && internalFormat == sw::FORMAT_A2B10G10R10UI))
		{
			void *buffer = lock(0, 0, sw::LOCK_WRITEONLY);

			if(buffer)
			{
				switch(type)
				{
				case GL_BYTE:
					switch(format)
					{
					case GL_R8:
					case GL_R8I:
					case GL_R8_SNORM:
					case GL_RED:
					case GL_RED_INTEGER:
					case GL_ALPHA:
					case GL_ALPHA8_EXT:
					case GL_LUMINANCE:
					case GL_LUMINANCE8_EXT:
						LoadImageData<Bytes_1>(xoffset, yoffset, zoffset, width, height, depth, inputPitch, inputHeight, getPitch(), getSlice(), input, buffer);
						break;
					case GL_RG8:
					case GL_RG8I:
					case GL_RG8_SNORM:
					case GL_RG:
					case GL_RG_INTEGER:
					case GL_LUMINANCE_ALPHA:
					case GL_LUMINANCE8_ALPHA8_EXT:
						LoadImageData<Bytes_2>(xoffset, yoffset, zoffset, width, height, depth, inputPitch, inputHeight, getPitch(), getSlice(), input, buffer);
						break;
					case GL_RGB8:
					case GL_RGB8I:
					case GL_RGB8_SNORM:
					case GL_RGB:
					case GL_RGB_INTEGER:
						LoadImageData<ByteRGB>(xoffset, yoffset, zoffset, width, height, depth, inputPitch, inputHeight, getPitch(), getSlice(), input, buffer);
						break;
					case GL_RGBA8:
					case GL_RGBA8I:
					case GL_RGBA8_SNORM:
					case GL_RGBA:
					case GL_RGBA_INTEGER:
					case GL_BGRA_EXT:
					case GL_BGRA8_EXT:
						LoadImageData<Bytes_4>(xoffset, yoffset, zoffset, width, height, depth, inputPitch, inputHeight, getPitch(), getSlice(), input, buffer);
						break;
					default: UNREACHABLE(format);
					}
					break;
				case GL_UNSIGNED_BYTE:
					switch(format)
					{
					case GL_R8:
					case GL_R8UI:
					case GL_R8_SNORM:
					case GL_RED:
					case GL_RED_INTEGER:
					case GL_ALPHA:
					case GL_ALPHA8_EXT:
					case GL_LUMINANCE:
					case GL_LUMINANCE8_EXT:
						LoadImageData<Bytes_1>(xoffset, yoffset, zoffset, width, height, depth, inputPitch, inputHeight, getPitch(), getSlice(), input, buffer);
						break;
					case GL_RG8:
					case GL_RG8UI:
					case GL_RG8_SNORM:
					case GL_RG:
					case GL_RG_INTEGER:
					case GL_LUMINANCE_ALPHA:
					case GL_LUMINANCE8_ALPHA8_EXT:
						LoadImageData<Bytes_2>(xoffset, yoffset, zoffset, width, height, depth, inputPitch, inputHeight, getPitch(), getSlice(), input, buffer);
						break;
					case GL_RGB8:
					case GL_RGB8UI:
					case GL_RGB8_SNORM:
					case GL_RGB:
					case GL_RGB_INTEGER:
					case GL_SRGB8:
						LoadImageData<UByteRGB>(xoffset, yoffset, zoffset, width, height, depth, inputPitch, inputHeight, getPitch(), getSlice(), input, buffer);
						break;
					case GL_RGBA8:
					case GL_RGBA8UI:
					case GL_RGBA8_SNORM:
					case GL_RGBA:
					case GL_RGBA_INTEGER:
					case GL_BGRA_EXT:
					case GL_BGRA8_EXT:
					case GL_SRGB8_ALPHA8:
						LoadImageData<Bytes_4>(xoffset, yoffset, zoffset, width, height, depth, inputPitch, inputHeight, getPitch(), getSlice(), input, buffer);
						break;
					default: UNREACHABLE(format);
					}
					break;
				case GL_UNSIGNED_SHORT_5_6_5:
					switch(format)
					{
					case GL_RGB565:
					case GL_RGB:
						LoadImageData<RGB565>(xoffset, yoffset, zoffset, width, height, depth, inputPitch, inputHeight, getPitch(), getSlice(), input, buffer);
						break;
					default: UNREACHABLE(format);
					}
					break;
				case GL_UNSIGNED_SHORT_4_4_4_4:
					switch(format)
					{
					case GL_RGBA4:
					case GL_RGBA:
						LoadImageData<RGBA4444>(xoffset, yoffset, zoffset, width, height, depth, inputPitch, inputHeight, getPitch(), getSlice(), input, buffer);
						break;
					default: UNREACHABLE(format);
					}
					break;
				case GL_UNSIGNED_SHORT_5_5_5_1:
					switch(format)
					{
					case GL_RGB5_A1:
					case GL_RGBA:
						LoadImageData<RGBA5551>(xoffset, yoffset, zoffset, width, height, depth, inputPitch, inputHeight, getPitch(), getSlice(), input, buffer);
						break;
					default: UNREACHABLE(format);
					}
					break;
				case GL_UNSIGNED_INT_10F_11F_11F_REV:
					switch(format)
					{
					case GL_R11F_G11F_B10F:
					case GL_RGB:
						LoadImageData<R11G11B10F>(xoffset, yoffset, zoffset, width, height, depth, inputPitch, inputHeight, getPitch(), getSlice(), input, buffer);
						break;
					default: UNREACHABLE(format);
					}
					break;
				case GL_UNSIGNED_INT_5_9_9_9_REV:
					switch(format)
					{
					case GL_RGB9_E5:
					case GL_RGB:
						LoadImageData<RGB9E5>(xoffset, yoffset, zoffset, width, height, depth, inputPitch, inputHeight, getPitch(), getSlice(), input, buffer);
						break;
					default: UNREACHABLE(format);
					}
					break;
				case GL_UNSIGNED_INT_2_10_10_10_REV:
					switch(format)
					{
					case GL_RGB10_A2UI:
					case GL_RGB10_A2:
					case GL_RGBA:
					case GL_RGBA_INTEGER:
						LoadImageData<Bytes_4>(xoffset, yoffset, zoffset, width, height, depth, inputPitch, inputHeight, getPitch(), getSlice(), input, buffer);
						break;
					default: UNREACHABLE(format);
					}
					break;
				case GL_FLOAT:
					switch(format)
					{
					// float textures are converted to RGBA, not BGRA
					case GL_ALPHA:
					case GL_ALPHA32F_EXT:
						LoadImageData<Bytes_4>(xoffset, yoffset, zoffset, width, height, depth, inputPitch, inputHeight, getPitch(), getSlice(), input, buffer);
						break;
					case GL_LUMINANCE:
					case GL_LUMINANCE32F_EXT:
						LoadImageData<Bytes_4>(xoffset, yoffset, zoffset, width, height, depth, inputPitch, inputHeight, getPitch(), getSlice(), input, buffer);
						break;
					case GL_LUMINANCE_ALPHA:
					case GL_LUMINANCE_ALPHA32F_EXT:
						LoadImageData<Bytes_8>(xoffset, yoffset, zoffset, width, height, depth, inputPitch, inputHeight, getPitch(), getSlice(), input, buffer);
						break;
					case GL_RED:
					case GL_R32F:
						LoadImageData<Bytes_4>(xoffset, yoffset, zoffset, width, height, depth, inputPitch, inputHeight, getPitch(), getSlice(), input, buffer);
						break;
					case GL_RG:
					case GL_RG32F:
						LoadImageData<Bytes_8>(xoffset, yoffset, zoffset, width, height, depth, inputPitch, inputHeight, getPitch(), getSlice(), input, buffer);
						break;
					case GL_RGB:
					case GL_RGB32F:
						LoadImageData<FloatRGB>(xoffset, yoffset, zoffset, width, height, depth, inputPitch, inputHeight, getPitch(), getSlice(), input, buffer);
						break;
					case GL_RGBA:
					case GL_RGBA32F:
						LoadImageData<Bytes_16>(xoffset, yoffset, zoffset, width, height, depth, inputPitch, inputHeight, getPitch(), getSlice(), input, buffer);
						break;
					case GL_R11F_G11F_B10F:
						LoadImageData<R11G11B10F>(xoffset, yoffset, zoffset, width, height, depth, inputPitch, inputHeight, getPitch(), getSlice(), input, buffer);
						break;
					case GL_RGB9_E5:
						LoadImageData<RGB9E5>(xoffset, yoffset, zoffset, width, height, depth, inputPitch, inputHeight, getPitch(), getSlice(), input, buffer);
						break;
					case GL_DEPTH_COMPONENT:
					case GL_DEPTH_COMPONENT32F:
						LoadImageData<D32F>(xoffset, yoffset, zoffset, width, height, depth, inputPitch, inputHeight, getPitch(), getSlice(), input, buffer);
						break;
					default: UNREACHABLE(format);
					}
					break;
				case GL_HALF_FLOAT:
				case GL_HALF_FLOAT_OES:
					switch(format)
					{
					case GL_ALPHA:
					case GL_ALPHA16F_EXT:
						LoadImageData<Bytes_2>(xoffset, yoffset, zoffset, width, height, depth, inputPitch, inputHeight, getPitch(), getSlice(), input, buffer);
						break;
					case GL_LUMINANCE:
					case GL_LUMINANCE16F_EXT:
						LoadImageData<Bytes_2>(xoffset, yoffset, zoffset, width, height, depth, inputPitch, inputHeight, getPitch(), getSlice(), input, buffer);
						break;
					case GL_LUMINANCE_ALPHA:
					case GL_LUMINANCE_ALPHA16F_EXT:
						LoadImageData<Bytes_4>(xoffset, yoffset, zoffset, width, height, depth, inputPitch, inputHeight, getPitch(), getSlice(), input, buffer);
						break;
					case GL_RED:
					case GL_R16F:
						LoadImageData<Bytes_2>(xoffset, yoffset, zoffset, width, height, depth, inputPitch, inputHeight, getPitch(), getSlice(), input, buffer);
						break;
					case GL_RG:
					case GL_RG16F:
						LoadImageData<Bytes_4>(xoffset, yoffset, zoffset, width, height, depth, inputPitch, inputHeight, getPitch(), getSlice(), input, buffer);
						break;
					case GL_RGB:
					case GL_RGB16F:
						LoadImageData<HalfFloatRGB>(xoffset, yoffset, zoffset, width, height, depth, inputPitch, inputHeight, getPitch(), getSlice(), input, buffer);
						break;
					case GL_RGBA:
					case GL_RGBA16F:
						LoadImageData<Bytes_8>(xoffset, yoffset, zoffset, width, height, depth, inputPitch, inputHeight, getPitch(), getSlice(), input, buffer);
						break;
					case GL_R11F_G11F_B10F:
						LoadImageData<R11G11B10F>(xoffset, yoffset, zoffset, width, height, depth, inputPitch, inputHeight, getPitch(), getSlice(), input, buffer);
						break;
					case GL_RGB9_E5:
						LoadImageData<RGB9E5>(xoffset, yoffset, zoffset, width, height, depth, inputPitch, inputHeight, getPitch(), getSlice(), input, buffer);
						break;
					default: UNREACHABLE(format);
					}
					break;
				case GL_SHORT:
					switch(format)
					{
					case GL_R16I:
					case GL_RED:
					case GL_RED_INTEGER:
					case GL_ALPHA:
					case GL_LUMINANCE:
						LoadImageData<Bytes_2>(xoffset, yoffset, zoffset, width, height, depth, inputPitch, inputHeight, getPitch(), getSlice(), input, buffer);
						break;
					case GL_RG16I:
					case GL_RG:
					case GL_RG_INTEGER:
					case GL_LUMINANCE_ALPHA:
						LoadImageData<Bytes_4>(xoffset, yoffset, zoffset, width, height, depth, inputPitch, inputHeight, getPitch(), getSlice(), input, buffer);
						break;
					case GL_RGB16I:
					case GL_RGB:
					case GL_RGB_INTEGER:
						LoadImageData<ShortRGB>(xoffset, yoffset, zoffset, width, height, depth, inputPitch, inputHeight, getPitch(), getSlice(), input, buffer);
						break;
					case GL_RGBA16I:
					case GL_RGBA:
					case GL_RGBA_INTEGER:
					case GL_BGRA_EXT:
					case GL_BGRA8_EXT:
						LoadImageData<Bytes_8>(xoffset, yoffset, zoffset, width, height, depth, inputPitch, inputHeight, getPitch(), getSlice(), input, buffer);
						break;
					default: UNREACHABLE(format);
					}
					break;
				case GL_UNSIGNED_SHORT:
					switch(format)
					{
					case GL_R16UI:
					case GL_RED:
					case GL_RED_INTEGER:
					case GL_ALPHA:
					case GL_LUMINANCE:
						LoadImageData<Bytes_2>(xoffset, yoffset, zoffset, width, height, depth, inputPitch, inputHeight, getPitch(), getSlice(), input, buffer);
						break;
					case GL_RG16UI:
					case GL_RG:
					case GL_RG_INTEGER:
					case GL_LUMINANCE_ALPHA:
						LoadImageData<Bytes_4>(xoffset, yoffset, zoffset, width, height, depth, inputPitch, inputHeight, getPitch(), getSlice(), input, buffer);
						break;
					case GL_RGB16UI:
					case GL_RGB:
					case GL_RGB_INTEGER:
						LoadImageData<UShortRGB>(xoffset, yoffset, zoffset, width, height, depth, inputPitch, inputHeight, getPitch(), getSlice(), input, buffer);
						break;
					case GL_RGBA16UI:
					case GL_RGBA:
					case GL_RGBA_INTEGER:
					case GL_BGRA_EXT:
					case GL_BGRA8_EXT:
						LoadImageData<Bytes_8>(xoffset, yoffset, zoffset, width, height, depth, inputPitch, inputHeight, getPitch(), getSlice(), input, buffer);
						break;
					case GL_DEPTH_COMPONENT:
					case GL_DEPTH_COMPONENT16:
						LoadImageData<D16>(xoffset, yoffset, zoffset, width, height, depth, inputPitch, inputHeight, getPitch(), getSlice(), input, buffer);
						break;
					default: UNREACHABLE(format);
					}
					break;
				case GL_INT:
					switch(format)
					{
					case GL_R32I:
					case GL_RED:
					case GL_RED_INTEGER:
					case GL_ALPHA:
					case GL_LUMINANCE:
						LoadImageData<Bytes_4>(xoffset, yoffset, zoffset, width, height, depth, inputPitch, inputHeight, getPitch(), getSlice(), input, buffer);
						break;
					case GL_RG32I:
					case GL_RG:
					case GL_RG_INTEGER:
					case GL_LUMINANCE_ALPHA:
						LoadImageData<Bytes_8>(xoffset, yoffset, zoffset, width, height, depth, inputPitch, inputHeight, getPitch(), getSlice(), input, buffer);
						break;
					case GL_RGB32I:
					case GL_RGB:
					case GL_RGB_INTEGER:
						LoadImageData<IntRGB>(xoffset, yoffset, zoffset, width, height, depth, inputPitch, inputHeight, getPitch(), getSlice(), input, buffer);
						break;
					case GL_RGBA32I:
					case GL_RGBA:
					case GL_RGBA_INTEGER:
					case GL_BGRA_EXT:
					case GL_BGRA8_EXT:
						LoadImageData<Bytes_16>(xoffset, yoffset, zoffset, width, height, depth, inputPitch, inputHeight, getPitch(), getSlice(), input, buffer);
						break;
					default: UNREACHABLE(format);
					}
					break;
				case GL_UNSIGNED_INT:
					switch(format)
					{
					case GL_R32UI:
					case GL_RED:
					case GL_RED_INTEGER:
					case GL_ALPHA:
					case GL_LUMINANCE:
						LoadImageData<Bytes_4>(xoffset, yoffset, zoffset, width, height, depth, inputPitch, inputHeight, getPitch(), getSlice(), input, buffer);
						break;
					case GL_RG32UI:
					case GL_RG:
					case GL_RG_INTEGER:
					case GL_LUMINANCE_ALPHA:
						LoadImageData<Bytes_8>(xoffset, yoffset, zoffset, width, height, depth, inputPitch, inputHeight, getPitch(), getSlice(), input, buffer);
						break;
					case GL_RGB32UI:
					case GL_RGB:
					case GL_RGB_INTEGER:
						LoadImageData<UIntRGB>(xoffset, yoffset, zoffset, width, height, depth, inputPitch, inputHeight, getPitch(), getSlice(), input, buffer);
						break;
					case GL_RGBA32UI:
					case GL_RGBA:
					case GL_RGBA_INTEGER:
					case GL_BGRA_EXT:
					case GL_BGRA8_EXT:
						LoadImageData<Bytes_16>(xoffset, yoffset, zoffset, width, height, depth, inputPitch, inputHeight, getPitch(), getSlice(), input, buffer);
						break;
					case GL_DEPTH_COMPONENT16:
					case GL_DEPTH_COMPONENT24:
					case GL_DEPTH_COMPONENT32_OES:
					case GL_DEPTH_COMPONENT:
						LoadImageData<D32>(xoffset, yoffset, zoffset, width, height, depth, inputPitch, inputHeight, getPitch(), getSlice(), input, buffer);
						break;
					default: UNREACHABLE(format);
					}
					break;
				case GL_UNSIGNED_INT_24_8_OES:
					loadD24S8ImageData(xoffset, yoffset, zoffset, width, height, depth, inputPitch, inputHeight, input, buffer);
					break;
				case GL_FLOAT_32_UNSIGNED_INT_24_8_REV:
					loadD32FS8ImageData(xoffset, yoffset, zoffset, width, height, depth, inputPitch, inputHeight, input, buffer);
					break;
				default: UNREACHABLE(type);
				}
			}

			unlock();
		}
		else
		{
			sw::Surface *source = sw::Surface::create(width, height, depth, ConvertFormatType(format, type), const_cast<void*>(input), inputPitch, inputPitch * inputHeight);
			sw::Rect sourceRect(0, 0, width, height);
			sw::Rect destRect(xoffset, yoffset, xoffset + width, yoffset + height);
			context->blit(source, sourceRect, this, destRect);
			delete source;
		}
	}

	void Image::loadD24S8ImageData(GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, int inputPitch, int inputHeight, const void *input, void *buffer)
	{
		LoadImageData<D24>(xoffset, yoffset, zoffset, width, height, depth, inputPitch, inputHeight, getPitch(), getSlice(), input, buffer);

		unsigned char *stencil = reinterpret_cast<unsigned char*>(lockStencil(0, 0, 0, sw::PUBLIC));

		if(stencil)
		{
			LoadImageData<S8>(xoffset, yoffset, zoffset, width, height, depth, inputPitch, inputHeight, getStencilPitchB(), getHeight(), input, stencil);

			unlockStencil();
		}
	}

	void Image::loadD32FS8ImageData(GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, int inputPitch, int inputHeight, const void *input, void *buffer)
	{
		LoadImageData<D32FS8>(xoffset, yoffset, zoffset, width, height, depth, inputPitch, inputHeight, getPitch(), getSlice(), input, buffer);

		unsigned char *stencil = reinterpret_cast<unsigned char*>(lockStencil(0, 0, 0, sw::PUBLIC));

		if(stencil)
		{
			LoadImageData<S24_8>(xoffset, yoffset, zoffset, width, height, depth, inputPitch, inputHeight, getStencilPitchB(), getHeight(), input, stencil);

			unlockStencil();
		}
	}

	void Image::loadCompressedData(GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLsizei imageSize, const void *pixels)
	{
		if(zoffset != 0 || depth != 1)
		{
			UNIMPLEMENTED();   // FIXME
		}

		int inputPitch = ComputeCompressedPitch(width, format);
		int rows = imageSize / inputPitch;
		void *buffer = lock(xoffset, yoffset, sw::LOCK_WRITEONLY);

		if(buffer)
		{
			for(int i = 0; i < rows; i++)
			{
				memcpy((void*)((GLbyte*)buffer + i * getPitch()), (void*)((GLbyte*)pixels + i * inputPitch), inputPitch);
			}
		}

		unlock();
	}
}
