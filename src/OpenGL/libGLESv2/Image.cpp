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

#include "Image.hpp"

#include "Texture.h"
#include "utilities.h"
#include "../common/debug.h"
#include "Common/Thread.hpp"

#include <GLES2/gl2ext.h>

namespace
{
	enum DataType
	{
		Alpha,
		AlphaFloat,
		AlphaHalfFloat,
		Luminance,
		LuminanceFloat,
		LuminanceHalfFloat,
		LuminanceAlpha,
		LuminanceAlphaFloat,
		LuminanceAlphaHalfFloat,
		RGBUByte,
		RGB565,
		RGBFloat,
		RGBHalfFloat,
		RGBAUByte,
		RGBA4444,
		RGBA5551,
		RGBAFloat,
		RGBAHalfFloat,
		BGRA,
		D16,
		D24,
		D32,
		S8,
	};

	template<DataType dataType>
	void LoadImageRow(const unsigned char *source, unsigned char *dest, GLint xoffset, GLsizei width)
	{
		UNIMPLEMENTED();
	}

	template<>
	void LoadImageRow<Alpha>(const unsigned char *source, unsigned char *dest, GLint xoffset, GLsizei width)
	{
		memcpy(dest + xoffset, source, width);
	}

	template<>
	void LoadImageRow<AlphaFloat>(const unsigned char *source, unsigned char *dest, GLint xoffset, GLsizei width)
	{
		const float *sourceF = reinterpret_cast<const float*>(source);
		float *destF = reinterpret_cast<float*>(dest + (xoffset * 16));

		for(int x = 0; x < width; x++)
		{
			destF[4 * x + 0] = 0;
			destF[4 * x + 1] = 0;
			destF[4 * x + 2] = 0;
			destF[4 * x + 3] = sourceF[x];
		}
	}

	template<>
	void LoadImageRow<AlphaHalfFloat>(const unsigned char *source, unsigned char *dest, GLint xoffset, GLsizei width)
	{
		const unsigned short *sourceH = reinterpret_cast<const unsigned short*>(source);
		unsigned short *destH = reinterpret_cast<unsigned short*>(dest + xoffset * 8);

		for(int x = 0; x < width; x++)
		{
			destH[4 * x + 0] = 0;
			destH[4 * x + 1] = 0;
			destH[4 * x + 2] = 0;
			destH[4 * x + 3] = sourceH[x];
		}
	}

	template<>
	void LoadImageRow<Luminance>(const unsigned char *source, unsigned char *dest, GLint xoffset, GLsizei width)
	{
		memcpy(dest + xoffset, source, width);
	}

	template<>
	void LoadImageRow<LuminanceFloat>(const unsigned char *source, unsigned char *dest, GLint xoffset, GLsizei width)
	{
		const float *sourceF = reinterpret_cast<const float*>(source);
		float *destF = reinterpret_cast<float*>(dest + xoffset * 16);

		for(int x = 0; x < width; x++)
		{
			destF[4 * x + 0] = sourceF[x];
			destF[4 * x + 1] = sourceF[x];
			destF[4 * x + 2] = sourceF[x];
			destF[4 * x + 3] = 1.0f;
		}
	}

	template<>
	void LoadImageRow<LuminanceHalfFloat>(const unsigned char *source, unsigned char *dest, GLint xoffset, GLsizei width)
	{
		const unsigned short *sourceH = reinterpret_cast<const unsigned short*>(source);
		unsigned short *destH = reinterpret_cast<unsigned short*>(dest + xoffset * 8);

		for(int x = 0; x < width; x++)
		{
			destH[4 * x + 0] = sourceH[x];
			destH[4 * x + 1] = sourceH[x];
			destH[4 * x + 2] = sourceH[x];
			destH[4 * x + 3] = 0x3C00; // SEEEEEMMMMMMMMMM, S = 0, E = 15, M = 0: 16bit flpt representation of 1
		}
	}

	template<>
	void LoadImageRow<LuminanceAlpha>(const unsigned char *source, unsigned char *dest, GLint xoffset, GLsizei width)
	{
		memcpy(dest + xoffset * 2, source, width * 2);
	}

	template<>
	void LoadImageRow<LuminanceAlphaFloat>(const unsigned char *source, unsigned char *dest, GLint xoffset, GLsizei width)
	{
		const float *sourceF = reinterpret_cast<const float*>(source);
		float *destF = reinterpret_cast<float*>(dest + xoffset * 16);

		for(int x = 0; x < width; x++)
		{
			destF[4 * x + 0] = sourceF[2 * x + 0];
			destF[4 * x + 1] = sourceF[2 * x + 0];
			destF[4 * x + 2] = sourceF[2 * x + 0];
			destF[4 * x + 3] = sourceF[2 * x + 1];
		}
	}

	template<>
	void LoadImageRow<LuminanceAlphaHalfFloat>(const unsigned char *source, unsigned char *dest, GLint xoffset, GLsizei width)
	{
		const unsigned short *sourceH = reinterpret_cast<const unsigned short*>(source);
		unsigned short *destH = reinterpret_cast<unsigned short*>(dest + xoffset * 8);

		for(int x = 0; x < width; x++)
		{
			destH[4 * x + 0] = sourceH[2 * x + 0];
			destH[4 * x + 1] = sourceH[2 * x + 0];
			destH[4 * x + 2] = sourceH[2 * x + 0];
			destH[4 * x + 3] = sourceH[2 * x + 1];
		}
	}

	template<>
	void LoadImageRow<RGBUByte>(const unsigned char *source, unsigned char *dest, GLint xoffset, GLsizei width)
	{
		unsigned char *destB = dest + xoffset * 4;

		for(int x = 0; x < width; x++)
		{
			destB[4 * x + 0] = source[x * 3 + 2];
			destB[4 * x + 1] = source[x * 3 + 1];
			destB[4 * x + 2] = source[x * 3 + 0];
			destB[4 * x + 3] = 0xFF;
		}
	}

	template<>
	void LoadImageRow<RGB565>(const unsigned char *source, unsigned char *dest, GLint xoffset, GLsizei width)
	{
		const unsigned short *source565 = reinterpret_cast<const unsigned short*>(source);
		unsigned char *dest565 = dest + xoffset * 4;

		for(int x = 0; x < width; x++)
		{
			unsigned short rgba = source565[x];
			dest565[4 * x + 0] = ((rgba & 0x001F) << 3) | ((rgba & 0x001F) >> 2);
			dest565[4 * x + 1] = ((rgba & 0x07E0) >> 3) | ((rgba & 0x07E0) >> 9);
			dest565[4 * x + 2] = ((rgba & 0xF800) >> 8) | ((rgba & 0xF800) >> 13);
			dest565[4 * x + 3] = 0xFF;
		}
	}

	template<>
	void LoadImageRow<RGBFloat>(const unsigned char *source, unsigned char *dest, GLint xoffset, GLsizei width)
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
	void LoadImageRow<RGBHalfFloat>(const unsigned char *source, unsigned char *dest, GLint xoffset, GLsizei width)
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
	void LoadImageRow<RGBAUByte>(const unsigned char *source, unsigned char *dest, GLint xoffset, GLsizei width)
	{
		const unsigned int *sourceI = reinterpret_cast<const unsigned int*>(source);
		unsigned int *destI = reinterpret_cast<unsigned int*>(dest + xoffset * 4);

		for(int x = 0; x < width; x++)
		{
			unsigned int rgba = sourceI[x];
			destI[x] = (rgba & 0xFF00FF00) | ((rgba << 16) & 0x00FF0000) | ((rgba >> 16) & 0x000000FF);
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
	void LoadImageRow<RGBAFloat>(const unsigned char *source, unsigned char *dest, GLint xoffset, GLsizei width)
	{
		memcpy(dest + xoffset * 16, source, width * 16);
	}

	template<>
	void LoadImageRow<RGBAHalfFloat>(const unsigned char *source, unsigned char *dest, GLint xoffset, GLsizei width)
	{
		memcpy(dest + xoffset * 8, source, width * 8);
	}

	template<>
	void LoadImageRow<BGRA>(const unsigned char *source, unsigned char *dest, GLint xoffset, GLsizei width)
	{
		memcpy(dest + xoffset * 4, source, width * 4);
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

	template<DataType dataType>
	void LoadImageData(GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, int inputPitch, int destPitch, GLsizei destHeight, const void *input, void *buffer)
	{
		for(int z = 0; z < depth; ++z)
		{
			const unsigned char *inputStart = static_cast<const unsigned char*>(input)+(z * inputPitch * height);
			unsigned char *destStart = static_cast<unsigned char*>(buffer)+((zoffset + z) * destPitch * destHeight);
			for(int y = 0; y < height; ++y)
			{
				const unsigned char *source = inputStart + y * inputPitch;
				unsigned char *dest = destStart + (y + yoffset) * destPitch;

				LoadImageRow<dataType>(source, dest, xoffset, width);
			}
		}
	}
}

namespace es2
{
	static sw::Resource *getParentResource(Texture *texture)
	{
		if(texture)
		{
			return texture->getResource();
		}

		return 0;
	}

	Image::Image(Texture *parentTexture, GLsizei width, GLsizei height, GLenum format, GLenum type)
		: parentTexture(parentTexture)
		, egl::Image(getParentResource(parentTexture), width, height, 1, format, type, selectInternalFormat(format, type))
	{
		referenceCount = 1;
	}

	Image::Image(Texture *parentTexture, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type)
		: parentTexture(parentTexture)
		, egl::Image(getParentResource(parentTexture), width, height, depth, format, type, selectInternalFormat(format, type))
	{
		referenceCount = 1;
	}

	Image::Image(Texture *parentTexture, GLsizei width, GLsizei height, sw::Format internalFormat, int multiSampleDepth, bool lockable, bool renderTarget)
		: parentTexture(parentTexture)
		, egl::Image(getParentResource(parentTexture), width, height, multiSampleDepth, internalFormat, lockable, renderTarget)
	{
		referenceCount = 1;
	}

	Image::~Image()
	{
		ASSERT(referenceCount == 0);
	}

	void Image::addRef()
	{
		if(parentTexture)
		{
			return parentTexture->addRef();
		}

		sw::atomicIncrement(&referenceCount);
	}

	void Image::release()
	{
		if(parentTexture)
		{
			return parentTexture->release();
		}

		if(referenceCount > 0)
		{
			sw::atomicDecrement(&referenceCount);
		}

		if(referenceCount == 0)
		{
			ASSERT(!shared);   // Should still hold a reference if eglDestroyImage hasn't been called
			delete this;
		}
	}

	void Image::unbind(const egl::Texture *parent)
	{
		if(parentTexture == parent)
		{
			parentTexture = 0;
		}

		release();
	}

	sw::Format Image::selectInternalFormat(GLenum format, GLenum type)
	{
		if(format == GL_ETC1_RGB8_OES)
		{
			return sw::FORMAT_ETC1;
		}
		else
		#if S3TC_SUPPORT
		if(format == GL_COMPRESSED_RGB_S3TC_DXT1_EXT ||
		   format == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT)
		{
			return sw::FORMAT_DXT1;
		}
		else if(format == GL_COMPRESSED_RGBA_S3TC_DXT3_ANGLE)
		{
			return sw::FORMAT_DXT3;
		}
		else if(format == GL_COMPRESSED_RGBA_S3TC_DXT5_ANGLE)
		{
			return sw::FORMAT_DXT5;
		}
		else
		#endif
		if(type == GL_FLOAT)
		{
			return sw::FORMAT_A32B32G32R32F;
		}
		else if(type == GL_HALF_FLOAT_OES)
		{
			return sw::FORMAT_A16B16G16R16F;
		}
		else if(type == GL_UNSIGNED_BYTE)
		{
			if(format == GL_LUMINANCE)
			{
				return sw::FORMAT_L8;
			}
			else if(format == GL_LUMINANCE_ALPHA)
			{
				return sw::FORMAT_A8L8;
			}
			else if(format == GL_RGBA || format == GL_BGRA_EXT)
			{
				return sw::FORMAT_A8R8G8B8;
			}
			else if(format == GL_RGB)
			{
				return sw::FORMAT_X8R8G8B8;
			}
			else if(format == GL_ALPHA)
			{
				return sw::FORMAT_A8;
			}
			else UNREACHABLE();
		}
		else if(type == GL_UNSIGNED_SHORT || type == GL_UNSIGNED_INT)
		{
			if(format == GL_DEPTH_COMPONENT)
			{
				return sw::FORMAT_D32FS8_TEXTURE;
			}
			else UNREACHABLE();
		}
		else if(type == GL_UNSIGNED_INT_24_8_OES)
		{
			if(format == GL_DEPTH_STENCIL_OES)
			{
				return sw::FORMAT_D32FS8_TEXTURE;
			}
			else UNREACHABLE();
		}
		else if(type == GL_UNSIGNED_SHORT_4_4_4_4)
		{
			return sw::FORMAT_A8R8G8B8;
		}
		else if(type == GL_UNSIGNED_SHORT_5_5_5_1)
		{
			return sw::FORMAT_A8R8G8B8;
		}
		else if(type == GL_UNSIGNED_SHORT_5_6_5)
		{
			return sw::FORMAT_X8R8G8B8;
		}
		else UNREACHABLE();

		return sw::FORMAT_A8R8G8B8;
	}

	void Image::loadImageData(GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, GLint unpackAlignment, const void *input)
	{
		GLsizei inputPitch = ComputePitch(width, format, type, unpackAlignment);
		void *buffer = lock(0, 0, sw::LOCK_WRITEONLY);
		
		if(buffer)
		{
			switch(type)
			{
			case GL_UNSIGNED_BYTE:
				switch(format)
				{
				case GL_ALPHA:
					LoadImageData<Alpha>(xoffset, yoffset, zoffset, width, height, depth, inputPitch, getPitch(), getHeight(), input, buffer);
					break;
				case GL_LUMINANCE:
					LoadImageData<Luminance>(xoffset, yoffset, zoffset, width, height, depth, inputPitch, getPitch(), getHeight(), input, buffer);
					break;
				case GL_LUMINANCE_ALPHA:
					LoadImageData<LuminanceAlpha>(xoffset, yoffset, zoffset, width, height, depth, inputPitch, getPitch(), getHeight(), input, buffer);
					break;
				case GL_RGB:
					LoadImageData<RGBUByte>(xoffset, yoffset, zoffset, width, height, depth, inputPitch, getPitch(), getHeight(), input, buffer);
					break;
				case GL_RGBA:
					LoadImageData<RGBAUByte>(xoffset, yoffset, zoffset, width, height, depth, inputPitch, getPitch(), getHeight(), input, buffer);
					break;
				case GL_BGRA_EXT:
					LoadImageData<BGRA>(xoffset, yoffset, zoffset, width, height, depth, inputPitch, getPitch(), getHeight(), input, buffer);
					break;
				default: UNREACHABLE();
				}
				break;
			case GL_UNSIGNED_SHORT_5_6_5:
				switch(format)
				{
				case GL_RGB:
					LoadImageData<RGB565>(xoffset, yoffset, zoffset, width, height, depth, inputPitch, getPitch(), getHeight(), input, buffer);
					break;
				default: UNREACHABLE();
				}
				break;
			case GL_UNSIGNED_SHORT_4_4_4_4:
				switch(format)
				{
				case GL_RGBA:
					LoadImageData<RGBA4444>(xoffset, yoffset, zoffset, width, height, depth, inputPitch, getPitch(), getHeight(), input, buffer);
					break;
				default: UNREACHABLE();
				}
				break;
			case GL_UNSIGNED_SHORT_5_5_5_1:
				switch(format)
				{
				case GL_RGBA:
					LoadImageData<RGBA5551>(xoffset, yoffset, zoffset, width, height, depth, inputPitch, getPitch(), getHeight(), input, buffer);
					break;
				default: UNREACHABLE();
				}
				break;
			case GL_FLOAT:
				switch(format)
				{
				// float textures are converted to RGBA, not BGRA
				case GL_ALPHA:
					LoadImageData<AlphaFloat>(xoffset, yoffset, zoffset, width, height, depth, inputPitch, getPitch(), getHeight(), input, buffer);
					break;
				case GL_LUMINANCE:
					LoadImageData<LuminanceFloat>(xoffset, yoffset, zoffset, width, height, depth, inputPitch, getPitch(), getHeight(), input, buffer);
					break;
				case GL_LUMINANCE_ALPHA:
					LoadImageData<LuminanceAlphaFloat>(xoffset, yoffset, zoffset, width, height, depth, inputPitch, getPitch(), getHeight(), input, buffer);
					break;
				case GL_RGB:
					LoadImageData<RGBFloat>(xoffset, yoffset, zoffset, width, height, depth, inputPitch, getPitch(), getHeight(), input, buffer);
					break;
				case GL_RGBA:
					LoadImageData<RGBAFloat>(xoffset, yoffset, zoffset, width, height, depth, inputPitch, getPitch(), getHeight(), input, buffer);
					break;
				default: UNREACHABLE();
				}
				break;
			case GL_HALF_FLOAT_OES:
				switch(format)
				{
				// float textures are converted to RGBA, not BGRA
				case GL_ALPHA:
					LoadImageData<AlphaHalfFloat>(xoffset, yoffset, zoffset, width, height, depth, inputPitch, getPitch(), getHeight(), input, buffer);
					break;
				case GL_LUMINANCE:
					LoadImageData<LuminanceHalfFloat>(xoffset, yoffset, zoffset, width, height, depth, inputPitch, getPitch(), getHeight(), input, buffer);
					break;
				case GL_LUMINANCE_ALPHA:
					LoadImageData<LuminanceAlphaHalfFloat>(xoffset, yoffset, zoffset, width, height, depth, inputPitch, getPitch(), getHeight(), input, buffer);
					break;
				case GL_RGB:
					LoadImageData<RGBHalfFloat>(xoffset, yoffset, zoffset, width, height, depth, inputPitch, getPitch(), getHeight(), input, buffer);
					break;
				case GL_RGBA:
					LoadImageData<RGBAHalfFloat>(xoffset, yoffset, zoffset, width, height, depth, inputPitch, getPitch(), getHeight(), input, buffer);
					break;
				default: UNREACHABLE();
				}
				break;
			case GL_UNSIGNED_SHORT:
				LoadImageData<D16>(xoffset, yoffset, zoffset, width, height, depth, inputPitch, getPitch(), getHeight(), input, buffer);
				break;
			case GL_UNSIGNED_INT:
				LoadImageData<D32>(xoffset, yoffset, zoffset, width, height, depth, inputPitch, getPitch(), getHeight(), input, buffer);
				break;
			case GL_UNSIGNED_INT_24_8_OES:
				loadD24S8ImageData(xoffset, yoffset, zoffset, width, height, depth, inputPitch, input, buffer);
				break;
			default: UNREACHABLE();
			}
		}

		unlock();
	}

	void Image::loadD24S8ImageData(GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, int inputPitch, const void *input, void *buffer)
	{
		LoadImageData<D24>(xoffset, yoffset, zoffset, width, height, depth, inputPitch, getPitch(), getHeight(), input, buffer);

		unsigned char *stencil = reinterpret_cast<unsigned char*>(lockStencil(0, sw::PUBLIC));

		if(stencil)
		{
			LoadImageData<S8>(xoffset, yoffset, zoffset, width, height, depth, inputPitch, getStencilPitchB(), getHeight(), input, stencil);

			unlockStencil();
		}
	}

	void Image::loadCompressedData(GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLsizei imageSize, const void *pixels)
	{
		if(zoffset != 0 || depth != 1)
		{
			UNIMPLEMENTED(); // FIXME
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