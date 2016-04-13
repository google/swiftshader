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

// utilities.cpp: Conversion functions and other utility routines.

#include "utilities.h"

#include "main.h"
#include "mathutil.h"
#include "Context.h"
#include "common/debug.h"

#include <limits>
#include <stdio.h>
#include <stdlib.h>

namespace es2
{
	// ES2 requires that format is equal to internal format at all glTex*Image2D entry points and the implementation
	// can decide the true, sized, internal format. The ES2FormatMap determines the internal format for all valid
	// format and type combinations.

	typedef std::pair<GLenum, GLenum> FormatTypePair;
	typedef std::pair<FormatTypePair, GLenum> FormatPair;
	typedef std::map<FormatTypePair, GLenum> FormatMap;

	// A helper function to insert data into the format map with fewer characters.
	static inline void InsertFormatMapping(FormatMap *map, GLenum format, GLenum type, GLenum internalFormat)
	{
		map->insert(FormatPair(FormatTypePair(format, type), internalFormat));
	}

	FormatMap BuildFormatMap()
	{
		static const GLenum GL_BGRA4_ANGLEX = 0x6ABC;
		static const GLenum GL_BGR5_A1_ANGLEX = 0x6ABD;

		FormatMap map;

		//                       | Format | Type | Internal format |
		InsertFormatMapping(&map, GL_RGBA, GL_UNSIGNED_BYTE, GL_RGBA8);
		InsertFormatMapping(&map, GL_RGBA, GL_BYTE, GL_RGBA8_SNORM);
		InsertFormatMapping(&map, GL_RGBA, GL_UNSIGNED_SHORT_4_4_4_4, GL_RGBA4);
		InsertFormatMapping(&map, GL_RGBA, GL_UNSIGNED_SHORT_5_5_5_1, GL_RGB5_A1);
		InsertFormatMapping(&map, GL_RGBA, GL_UNSIGNED_INT_2_10_10_10_REV, GL_RGB10_A2);
		InsertFormatMapping(&map, GL_RGBA, GL_FLOAT, GL_RGBA32F);
		InsertFormatMapping(&map, GL_RGBA, GL_HALF_FLOAT, GL_RGBA16F);
		InsertFormatMapping(&map, GL_RGBA, GL_HALF_FLOAT_OES, GL_RGBA16F);

		InsertFormatMapping(&map, GL_RGBA_INTEGER, GL_UNSIGNED_BYTE, GL_RGBA8UI);
		InsertFormatMapping(&map, GL_RGBA_INTEGER, GL_BYTE, GL_RGBA8I);
		InsertFormatMapping(&map, GL_RGBA_INTEGER, GL_UNSIGNED_SHORT, GL_RGBA16UI);
		InsertFormatMapping(&map, GL_RGBA_INTEGER, GL_SHORT, GL_RGBA16I);
		InsertFormatMapping(&map, GL_RGBA_INTEGER, GL_UNSIGNED_INT, GL_RGBA32UI);
		InsertFormatMapping(&map, GL_RGBA_INTEGER, GL_INT, GL_RGBA32I);
		InsertFormatMapping(&map, GL_RGBA_INTEGER, GL_UNSIGNED_INT_2_10_10_10_REV, GL_RGB10_A2UI);

		InsertFormatMapping(&map, GL_RGB, GL_UNSIGNED_BYTE, GL_RGB8);
		InsertFormatMapping(&map, GL_RGB, GL_BYTE, GL_RGB8_SNORM);
		InsertFormatMapping(&map, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, GL_RGB565);
		InsertFormatMapping(&map, GL_RGB, GL_UNSIGNED_INT_10F_11F_11F_REV, GL_R11F_G11F_B10F);
		InsertFormatMapping(&map, GL_RGB, GL_UNSIGNED_INT_5_9_9_9_REV, GL_RGB9_E5);
		InsertFormatMapping(&map, GL_RGB, GL_FLOAT, GL_RGB32F);
		InsertFormatMapping(&map, GL_RGB, GL_HALF_FLOAT, GL_RGB16F);
		InsertFormatMapping(&map, GL_RGB, GL_HALF_FLOAT_OES, GL_RGB16F);

		InsertFormatMapping(&map, GL_RGB_INTEGER, GL_UNSIGNED_BYTE, GL_RGB8UI);
		InsertFormatMapping(&map, GL_RGB_INTEGER, GL_BYTE, GL_RGB8I);
		InsertFormatMapping(&map, GL_RGB_INTEGER, GL_UNSIGNED_SHORT, GL_RGB16UI);
		InsertFormatMapping(&map, GL_RGB_INTEGER, GL_SHORT, GL_RGB16I);
		InsertFormatMapping(&map, GL_RGB_INTEGER, GL_UNSIGNED_INT, GL_RGB32UI);
		InsertFormatMapping(&map, GL_RGB_INTEGER, GL_INT, GL_RGB32I);

		InsertFormatMapping(&map, GL_RG, GL_UNSIGNED_BYTE, GL_RG8);
		InsertFormatMapping(&map, GL_RG, GL_BYTE, GL_RG8_SNORM);
		InsertFormatMapping(&map, GL_RG, GL_FLOAT, GL_RG32F);
		InsertFormatMapping(&map, GL_RG, GL_HALF_FLOAT, GL_RG16F);
		InsertFormatMapping(&map, GL_RG, GL_HALF_FLOAT_OES, GL_RG16F);

		InsertFormatMapping(&map, GL_RG_INTEGER, GL_UNSIGNED_BYTE, GL_RG8UI);
		InsertFormatMapping(&map, GL_RG_INTEGER, GL_BYTE, GL_RG8I);
		InsertFormatMapping(&map, GL_RG_INTEGER, GL_UNSIGNED_SHORT, GL_RG16UI);
		InsertFormatMapping(&map, GL_RG_INTEGER, GL_SHORT, GL_RG16I);
		InsertFormatMapping(&map, GL_RG_INTEGER, GL_UNSIGNED_INT, GL_RG32UI);
		InsertFormatMapping(&map, GL_RG_INTEGER, GL_INT, GL_RG32I);

		InsertFormatMapping(&map, GL_RED, GL_UNSIGNED_BYTE, GL_R8);
		InsertFormatMapping(&map, GL_RED, GL_BYTE, GL_R8_SNORM);
		InsertFormatMapping(&map, GL_RED, GL_FLOAT, GL_R32F);
		InsertFormatMapping(&map, GL_RED, GL_HALF_FLOAT, GL_R16F);
		InsertFormatMapping(&map, GL_RED, GL_HALF_FLOAT_OES, GL_R16F);

		InsertFormatMapping(&map, GL_RED_INTEGER, GL_UNSIGNED_BYTE, GL_R8UI);
		InsertFormatMapping(&map, GL_RED_INTEGER, GL_BYTE, GL_R8I);
		InsertFormatMapping(&map, GL_RED_INTEGER, GL_UNSIGNED_SHORT, GL_R16UI);
		InsertFormatMapping(&map, GL_RED_INTEGER, GL_SHORT, GL_R16I);
		InsertFormatMapping(&map, GL_RED_INTEGER, GL_UNSIGNED_INT, GL_R32UI);
		InsertFormatMapping(&map, GL_RED_INTEGER, GL_INT, GL_R32I);

		InsertFormatMapping(&map, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, GL_LUMINANCE8_ALPHA8_EXT);
		InsertFormatMapping(&map, GL_LUMINANCE, GL_UNSIGNED_BYTE, GL_LUMINANCE8_EXT);
		InsertFormatMapping(&map, GL_ALPHA, GL_UNSIGNED_BYTE, GL_ALPHA8_EXT);
		InsertFormatMapping(&map, GL_LUMINANCE_ALPHA, GL_FLOAT, GL_LUMINANCE_ALPHA32F_EXT);
		InsertFormatMapping(&map, GL_LUMINANCE, GL_FLOAT, GL_LUMINANCE32F_EXT);
		InsertFormatMapping(&map, GL_ALPHA, GL_FLOAT, GL_ALPHA32F_EXT);
		InsertFormatMapping(&map, GL_LUMINANCE_ALPHA, GL_HALF_FLOAT, GL_LUMINANCE_ALPHA16F_EXT);
		InsertFormatMapping(&map, GL_LUMINANCE_ALPHA, GL_HALF_FLOAT_OES, GL_LUMINANCE_ALPHA16F_EXT);
		InsertFormatMapping(&map, GL_LUMINANCE, GL_HALF_FLOAT, GL_LUMINANCE16F_EXT);
		InsertFormatMapping(&map, GL_LUMINANCE, GL_HALF_FLOAT_OES, GL_LUMINANCE16F_EXT);
		InsertFormatMapping(&map, GL_ALPHA, GL_HALF_FLOAT, GL_ALPHA16F_EXT);
		InsertFormatMapping(&map, GL_ALPHA, GL_HALF_FLOAT_OES, GL_ALPHA16F_EXT);

		InsertFormatMapping(&map, GL_BGRA_EXT, GL_UNSIGNED_BYTE, GL_BGRA8_EXT);
		InsertFormatMapping(&map, GL_BGRA_EXT, GL_UNSIGNED_SHORT_4_4_4_4_REV_EXT, GL_BGRA4_ANGLEX);
		InsertFormatMapping(&map, GL_BGRA_EXT, GL_UNSIGNED_SHORT_1_5_5_5_REV_EXT, GL_BGR5_A1_ANGLEX);

		InsertFormatMapping(&map, GL_SRGB_EXT, GL_UNSIGNED_BYTE, GL_SRGB8);
		InsertFormatMapping(&map, GL_SRGB_ALPHA_EXT, GL_UNSIGNED_BYTE, GL_SRGB8_ALPHA8);

		InsertFormatMapping(&map, GL_COMPRESSED_RGB_S3TC_DXT1_EXT, GL_UNSIGNED_BYTE, GL_COMPRESSED_RGB_S3TC_DXT1_EXT);
		InsertFormatMapping(&map, GL_COMPRESSED_RGBA_S3TC_DXT1_EXT, GL_UNSIGNED_BYTE, GL_COMPRESSED_RGBA_S3TC_DXT1_EXT);
		InsertFormatMapping(&map, GL_COMPRESSED_RGBA_S3TC_DXT3_ANGLE, GL_UNSIGNED_BYTE, GL_COMPRESSED_RGBA_S3TC_DXT3_ANGLE);
		InsertFormatMapping(&map, GL_COMPRESSED_RGBA_S3TC_DXT5_ANGLE, GL_UNSIGNED_BYTE, GL_COMPRESSED_RGBA_S3TC_DXT5_ANGLE);

		InsertFormatMapping(&map, GL_DEPTH_COMPONENT, GL_UNSIGNED_SHORT, GL_DEPTH_COMPONENT16);
		InsertFormatMapping(&map, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, GL_DEPTH_COMPONENT32_OES);
		InsertFormatMapping(&map, GL_DEPTH_COMPONENT, GL_FLOAT, GL_DEPTH_COMPONENT32F);

		InsertFormatMapping(&map, GL_STENCIL, GL_UNSIGNED_BYTE, GL_STENCIL_INDEX8);

		InsertFormatMapping(&map, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, GL_DEPTH24_STENCIL8);
		InsertFormatMapping(&map, GL_DEPTH_STENCIL, GL_FLOAT_32_UNSIGNED_INT_24_8_REV, GL_DEPTH32F_STENCIL8);

		return map;
	}

	GLenum GetSizedInternalFormat(GLenum internalFormat, GLenum type)
	{
		switch(internalFormat)
		{
		case GL_ALPHA:
		case GL_LUMINANCE:
		case GL_LUMINANCE_ALPHA:
		case GL_RED:
		case GL_RG:
		case GL_RGB:
		case GL_RGBA:
		case GL_RED_INTEGER:
		case GL_RG_INTEGER:
		case GL_RGB_INTEGER:
		case GL_RGBA_INTEGER:
		case GL_BGRA_EXT:
		case GL_DEPTH_COMPONENT:
		case GL_DEPTH_STENCIL:
		case GL_SRGB_EXT:
		case GL_SRGB_ALPHA_EXT:
			{
				static const FormatMap formatMap = BuildFormatMap();
				FormatMap::const_iterator iter = formatMap.find(FormatTypePair(internalFormat, type));
				return (iter != formatMap.end()) ? iter->second : GL_NONE;
			}
		default:
			return internalFormat;
		}
	}

	unsigned int UniformComponentCount(GLenum type)
	{
		switch(type)
		{
		case GL_BOOL:
		case GL_FLOAT:
		case GL_INT:
		case GL_UNSIGNED_INT:
		case GL_SAMPLER_2D:
		case GL_SAMPLER_CUBE:
        case GL_SAMPLER_EXTERNAL_OES:
		case GL_SAMPLER_3D_OES:
		case GL_SAMPLER_2D_ARRAY:
		case GL_SAMPLER_2D_SHADOW:
		case GL_SAMPLER_CUBE_SHADOW:
		case GL_SAMPLER_2D_ARRAY_SHADOW:
		case GL_INT_SAMPLER_2D:
		case GL_UNSIGNED_INT_SAMPLER_2D:
		case GL_INT_SAMPLER_CUBE:
		case GL_UNSIGNED_INT_SAMPLER_CUBE:
		case GL_INT_SAMPLER_3D:
		case GL_UNSIGNED_INT_SAMPLER_3D:
		case GL_INT_SAMPLER_2D_ARRAY:
		case GL_UNSIGNED_INT_SAMPLER_2D_ARRAY:
			return 1;
		case GL_BOOL_VEC2:
		case GL_FLOAT_VEC2:
		case GL_INT_VEC2:
		case GL_UNSIGNED_INT_VEC2:
			return 2;
		case GL_INT_VEC3:
		case GL_UNSIGNED_INT_VEC3:
		case GL_FLOAT_VEC3:
		case GL_BOOL_VEC3:
			return 3;
		case GL_BOOL_VEC4:
		case GL_FLOAT_VEC4:
		case GL_INT_VEC4:
		case GL_UNSIGNED_INT_VEC4:
		case GL_FLOAT_MAT2:
			return 4;
		case GL_FLOAT_MAT2x3:
		case GL_FLOAT_MAT3x2:
			return 6;
		case GL_FLOAT_MAT2x4:
		case GL_FLOAT_MAT4x2:
			return 8;
		case GL_FLOAT_MAT3:
			return 9;
		case GL_FLOAT_MAT3x4:
		case GL_FLOAT_MAT4x3:
			return 12;
		case GL_FLOAT_MAT4:
			return 16;
		default:
			UNREACHABLE(type);
		}

		return 0;
	}

	GLenum UniformComponentType(GLenum type)
	{
		switch(type)
		{
		case GL_BOOL:
		case GL_BOOL_VEC2:
		case GL_BOOL_VEC3:
		case GL_BOOL_VEC4:
			return GL_BOOL;
		case GL_FLOAT:
		case GL_FLOAT_VEC2:
		case GL_FLOAT_VEC3:
		case GL_FLOAT_VEC4:
		case GL_FLOAT_MAT2:
		case GL_FLOAT_MAT2x3:
		case GL_FLOAT_MAT2x4:
		case GL_FLOAT_MAT3:
		case GL_FLOAT_MAT3x2:
		case GL_FLOAT_MAT3x4:
		case GL_FLOAT_MAT4:
		case GL_FLOAT_MAT4x2:
		case GL_FLOAT_MAT4x3:
			return GL_FLOAT;
		case GL_INT:
		case GL_SAMPLER_2D:
		case GL_SAMPLER_CUBE:
		case GL_SAMPLER_EXTERNAL_OES:
		case GL_SAMPLER_3D_OES:
		case GL_SAMPLER_2D_ARRAY:
		case GL_SAMPLER_2D_SHADOW:
		case GL_SAMPLER_CUBE_SHADOW:
		case GL_SAMPLER_2D_ARRAY_SHADOW:
		case GL_INT_SAMPLER_2D:
		case GL_UNSIGNED_INT_SAMPLER_2D:
		case GL_INT_SAMPLER_CUBE:
		case GL_UNSIGNED_INT_SAMPLER_CUBE:
		case GL_INT_SAMPLER_3D:
		case GL_UNSIGNED_INT_SAMPLER_3D:
		case GL_INT_SAMPLER_2D_ARRAY:
		case GL_UNSIGNED_INT_SAMPLER_2D_ARRAY:
		case GL_INT_VEC2:
		case GL_INT_VEC3:
		case GL_INT_VEC4:
			return GL_INT;
		case GL_UNSIGNED_INT:
		case GL_UNSIGNED_INT_VEC2:
		case GL_UNSIGNED_INT_VEC3:
		case GL_UNSIGNED_INT_VEC4:
			return GL_UNSIGNED_INT;
		default:
			UNREACHABLE(type);
		}

		return GL_NONE;
	}

	size_t UniformTypeSize(GLenum type)
	{
		switch(type)
		{
		case GL_BOOL:  return sizeof(GLboolean);
		case GL_FLOAT: return sizeof(GLfloat);
		case GL_INT:   return sizeof(GLint);
		case GL_UNSIGNED_INT: return sizeof(GLuint);
		}

		return UniformTypeSize(UniformComponentType(type)) * UniformComponentCount(type);
	}

	bool IsSamplerUniform(GLenum type)
	{
		switch(type)
		{
		case GL_SAMPLER_2D:
		case GL_SAMPLER_CUBE:
		case GL_SAMPLER_EXTERNAL_OES:
		case GL_SAMPLER_3D_OES:
		case GL_SAMPLER_2D_ARRAY:
		case GL_SAMPLER_2D_SHADOW:
		case GL_SAMPLER_CUBE_SHADOW:
		case GL_SAMPLER_2D_ARRAY_SHADOW:
		case GL_INT_SAMPLER_2D:
		case GL_UNSIGNED_INT_SAMPLER_2D:
		case GL_INT_SAMPLER_CUBE:
		case GL_UNSIGNED_INT_SAMPLER_CUBE:
		case GL_INT_SAMPLER_3D:
		case GL_UNSIGNED_INT_SAMPLER_3D:
		case GL_INT_SAMPLER_2D_ARRAY:
		case GL_UNSIGNED_INT_SAMPLER_2D_ARRAY:
			return true;
		default:
			return false;
		}
	}

	int VariableRowCount(GLenum type)
	{
		switch(type)
		{
		case GL_NONE:
			return 0;
		case GL_BOOL:
		case GL_FLOAT:
		case GL_INT:
		case GL_UNSIGNED_INT:
		case GL_BOOL_VEC2:
		case GL_FLOAT_VEC2:
		case GL_INT_VEC2:
		case GL_UNSIGNED_INT_VEC2:
		case GL_INT_VEC3:
		case GL_UNSIGNED_INT_VEC3:
		case GL_FLOAT_VEC3:
		case GL_BOOL_VEC3:
		case GL_BOOL_VEC4:
		case GL_FLOAT_VEC4:
		case GL_INT_VEC4:
		case GL_UNSIGNED_INT_VEC4:
		case GL_SAMPLER_2D:
		case GL_SAMPLER_CUBE:
        case GL_SAMPLER_EXTERNAL_OES:
		case GL_SAMPLER_3D_OES:
		case GL_SAMPLER_2D_ARRAY:
		case GL_SAMPLER_2D_SHADOW:
		case GL_SAMPLER_CUBE_SHADOW:
		case GL_SAMPLER_2D_ARRAY_SHADOW:
		case GL_INT_SAMPLER_2D:
		case GL_UNSIGNED_INT_SAMPLER_2D:
		case GL_INT_SAMPLER_CUBE:
		case GL_UNSIGNED_INT_SAMPLER_CUBE:
		case GL_INT_SAMPLER_3D:
		case GL_UNSIGNED_INT_SAMPLER_3D:
		case GL_INT_SAMPLER_2D_ARRAY:
		case GL_UNSIGNED_INT_SAMPLER_2D_ARRAY:
			return 1;
		case GL_FLOAT_MAT2:
		case GL_FLOAT_MAT3x2:
		case GL_FLOAT_MAT4x2:
			return 2;
		case GL_FLOAT_MAT3:
		case GL_FLOAT_MAT2x3:
		case GL_FLOAT_MAT4x3:
			return 3;
		case GL_FLOAT_MAT4:
		case GL_FLOAT_MAT2x4:
		case GL_FLOAT_MAT3x4:
			return 4;
		default:
			UNREACHABLE(type);
		}

		return 0;
	}

	int VariableColumnCount(GLenum type)
	{
		switch(type)
		{
		case GL_NONE:
			return 0;
		case GL_BOOL:
		case GL_FLOAT:
		case GL_INT:
		case GL_UNSIGNED_INT:
			return 1;
		case GL_BOOL_VEC2:
		case GL_FLOAT_VEC2:
		case GL_INT_VEC2:
		case GL_UNSIGNED_INT_VEC2:
		case GL_FLOAT_MAT2:
		case GL_FLOAT_MAT2x3:
		case GL_FLOAT_MAT2x4:
			return 2;
		case GL_INT_VEC3:
		case GL_UNSIGNED_INT_VEC3:
		case GL_FLOAT_VEC3:
		case GL_BOOL_VEC3:
		case GL_FLOAT_MAT3:
		case GL_FLOAT_MAT3x2:
		case GL_FLOAT_MAT3x4:
			return 3;
		case GL_BOOL_VEC4:
		case GL_FLOAT_VEC4:
		case GL_INT_VEC4:
		case GL_UNSIGNED_INT_VEC4:
		case GL_FLOAT_MAT4:
		case GL_FLOAT_MAT4x2:
		case GL_FLOAT_MAT4x3:
			return 4;
		default:
			UNREACHABLE(type);
		}

		return 0;
	}

	int VariableRegisterCount(GLenum type)
	{
		// Number of registers used is the number of columns for matrices or 1 for scalars and vectors
		return (VariableRowCount(type) > 1) ? VariableColumnCount(type) : 1;
	}

	int VariableRegisterSize(GLenum type)
	{
		// Number of components per register is the number of rows for matrices or columns for scalars and vectors
		int nbRows = VariableRowCount(type);
		return (nbRows > 1) ? nbRows : VariableColumnCount(type);
	}

	int AllocateFirstFreeBits(unsigned int *bits, unsigned int allocationSize, unsigned int bitsSize)
	{
		ASSERT(allocationSize <= bitsSize);

		unsigned int mask = std::numeric_limits<unsigned int>::max() >> (std::numeric_limits<unsigned int>::digits - allocationSize);

		for(unsigned int i = 0; i < bitsSize - allocationSize + 1; i++)
		{
			if((*bits & mask) == 0)
			{
				*bits |= mask;
				return i;
			}

			mask <<= 1;
		}

		return -1;
	}

	GLint floatToInt(GLfloat value)
	{
		return static_cast<GLint>((static_cast<GLfloat>(0xFFFFFFFF) * value - 1.0f) * 0.5f);
	}

	bool IsCompressed(GLenum format, GLint clientVersion)
	{
		return ValidateCompressedFormat(format, clientVersion, true) == GL_NONE;
	}

	GLenum ValidateCompressedFormat(GLenum format, GLint clientVersion, bool expectCompressedFormats)
	{
		switch(format)
		{
		case GL_COMPRESSED_RGB_S3TC_DXT1_EXT:
		case GL_COMPRESSED_RGBA_S3TC_DXT1_EXT:
		case GL_COMPRESSED_RGBA_S3TC_DXT3_ANGLE:
		case GL_COMPRESSED_RGBA_S3TC_DXT5_ANGLE:
			return S3TC_SUPPORT ? (expectCompressedFormats ? GL_NONE : GL_INVALID_OPERATION) : GL_INVALID_ENUM;
		case GL_ETC1_RGB8_OES:
			return expectCompressedFormats ? GL_NONE : GL_INVALID_OPERATION;
		case GL_COMPRESSED_R11_EAC:
		case GL_COMPRESSED_SIGNED_R11_EAC:
		case GL_COMPRESSED_RG11_EAC:
		case GL_COMPRESSED_SIGNED_RG11_EAC:
		case GL_COMPRESSED_RGB8_ETC2:
		case GL_COMPRESSED_SRGB8_ETC2:
		case GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2:
		case GL_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2:
		case GL_COMPRESSED_RGBA8_ETC2_EAC:
		case GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC:
		case GL_COMPRESSED_RGBA_ASTC_4x4_KHR:
		case GL_COMPRESSED_RGBA_ASTC_5x4_KHR:
		case GL_COMPRESSED_RGBA_ASTC_5x5_KHR:
		case GL_COMPRESSED_RGBA_ASTC_6x5_KHR:
		case GL_COMPRESSED_RGBA_ASTC_6x6_KHR:
		case GL_COMPRESSED_RGBA_ASTC_8x5_KHR:
		case GL_COMPRESSED_RGBA_ASTC_8x6_KHR:
		case GL_COMPRESSED_RGBA_ASTC_8x8_KHR:
		case GL_COMPRESSED_RGBA_ASTC_10x5_KHR:
		case GL_COMPRESSED_RGBA_ASTC_10x6_KHR:
		case GL_COMPRESSED_RGBA_ASTC_10x8_KHR:
		case GL_COMPRESSED_RGBA_ASTC_10x10_KHR:
		case GL_COMPRESSED_RGBA_ASTC_12x10_KHR:
		case GL_COMPRESSED_RGBA_ASTC_12x12_KHR:
		case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR:
		case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x4_KHR:
		case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x5_KHR:
		case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x5_KHR:
		case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x6_KHR:
		case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x5_KHR:
		case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x6_KHR:
		case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x8_KHR:
		case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x5_KHR:
		case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x6_KHR:
		case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x8_KHR:
		case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x10_KHR:
		case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x10_KHR:
		case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x12_KHR:
			return (clientVersion >= 3) ? (expectCompressedFormats ? GL_NONE : GL_INVALID_OPERATION) : GL_INVALID_ENUM;
		default:
			return expectCompressedFormats ? GL_INVALID_ENUM : GL_NONE; // Not compressed format
		}
	}

	GLenum ValidateSubImageParams(bool compressed, GLsizei width, GLsizei height, GLint xoffset, GLint yoffset, GLenum target, GLint level, GLenum sizedInternalFormat, Texture *texture)
	{
		if(!texture)
		{
			return GL_INVALID_OPERATION;
		}

		if(compressed != texture->isCompressed(target, level))
		{
			return GL_INVALID_OPERATION;
		}

		if(sizedInternalFormat != GL_NONE && sizedInternalFormat != texture->getFormat(target, level))
		{
			return GL_INVALID_OPERATION;
		}

		if(compressed)
		{
			if((width % 4 != 0 && width != texture->getWidth(target, 0)) ||
			   (height % 4 != 0 && height != texture->getHeight(target, 0)))
			{
				return GL_INVALID_OPERATION;
			}
		}

		if(xoffset + width > texture->getWidth(target, level) ||
		   yoffset + height > texture->getHeight(target, level))
		{
			return GL_INVALID_VALUE;
		}

		return GL_NONE;
	}

	GLenum ValidateSubImageParams(bool compressed, GLsizei width, GLsizei height, GLsizei depth, GLint xoffset, GLint yoffset, GLint zoffset, GLenum target, GLint level, GLenum sizedInternalFormat, Texture *texture)
	{
		if(!texture)
		{
			return GL_INVALID_OPERATION;
		}

		if(compressed != texture->isCompressed(target, level))
		{
			return GL_INVALID_OPERATION;
		}

		if(sizedInternalFormat != GL_NONE && sizedInternalFormat != GetSizedInternalFormat(texture->getFormat(target, level), texture->getType(target, level)))
		{
			return GL_INVALID_OPERATION;
		}

		if(compressed)
		{
			if((width % 4 != 0 && width != texture->getWidth(target, 0)) ||
			   (height % 4 != 0 && height != texture->getHeight(target, 0)) ||
			   (depth % 4 != 0 && depth != texture->getDepth(target, 0)))
			{
				return GL_INVALID_OPERATION;
			}
		}

		if(xoffset + width > texture->getWidth(target, level) ||
		   yoffset + height > texture->getHeight(target, level) ||
		   zoffset + depth > texture->getDepth(target, level))
		{
			return GL_INVALID_VALUE;
		}

		return GL_NONE;
	}

	bool ValidReadPixelsFormatType(GLenum internalFormat, GLenum internalType, GLenum format, GLenum type, GLint clientVersion)
	{
		switch(format)
		{
		case GL_RGBA:
			switch(type)
			{
			case GL_UNSIGNED_BYTE:
				break;
			case GL_UNSIGNED_INT_2_10_10_10_REV:
				return (clientVersion >= 3) && (internalFormat == GL_RGB10_A2);
			case GL_FLOAT:
				return (clientVersion >= 3) && (internalType == GL_FLOAT);
			default:
				return false;
			}
			break;
		case GL_RGBA_INTEGER:
			if(clientVersion < 3)
			{
				return false;
			}
			switch(type)
			{
			case GL_INT:
				if(internalType != GL_INT)
				{
					return false;
				}
				break;
			case GL_UNSIGNED_INT:
				if(internalType != GL_UNSIGNED_INT)
				{
					return false;
				}
				break;
			default:
				return false;
			}
			break;
		case GL_BGRA_EXT:
			switch(type)
			{
			case GL_UNSIGNED_BYTE:
			case GL_UNSIGNED_SHORT_4_4_4_4_REV_EXT:
			case GL_UNSIGNED_SHORT_1_5_5_5_REV_EXT:
				break;
			default:
				return false;
			}
			break;
		case GL_RG_EXT:
		case GL_RED_EXT:
			return (clientVersion >= 3) && (type == GL_UNSIGNED_BYTE);
		default:
			return false;
		}
		return true;
	}

	bool IsDepthTexture(GLenum format)
	{
		return format == GL_DEPTH_COMPONENT ||
		       format == GL_DEPTH_STENCIL_OES ||
		       format == GL_DEPTH_COMPONENT16 ||
		       format == GL_DEPTH_COMPONENT24 ||
		       format == GL_DEPTH_COMPONENT32_OES ||
		       format == GL_DEPTH_COMPONENT32F ||
		       format == GL_DEPTH24_STENCIL8 ||
		       format == GL_DEPTH32F_STENCIL8;
	}

	bool IsStencilTexture(GLenum format)
	{
		return format == GL_STENCIL_INDEX_OES ||
		       format == GL_DEPTH_STENCIL_OES ||
		       format == GL_DEPTH24_STENCIL8 ||
		       format == GL_DEPTH32F_STENCIL8;
	}

	bool IsCubemapTextureTarget(GLenum target)
	{
		return (target >= GL_TEXTURE_CUBE_MAP_POSITIVE_X && target <= GL_TEXTURE_CUBE_MAP_NEGATIVE_Z);
	}

	int CubeFaceIndex(GLenum cubeFace)
	{
		switch(cubeFace)
		{
		case GL_TEXTURE_CUBE_MAP:
		case GL_TEXTURE_CUBE_MAP_POSITIVE_X: return 0;
		case GL_TEXTURE_CUBE_MAP_NEGATIVE_X: return 1;
		case GL_TEXTURE_CUBE_MAP_POSITIVE_Y: return 2;
		case GL_TEXTURE_CUBE_MAP_NEGATIVE_Y: return 3;
		case GL_TEXTURE_CUBE_MAP_POSITIVE_Z: return 4;
		case GL_TEXTURE_CUBE_MAP_NEGATIVE_Z: return 5;
		default: UNREACHABLE(cubeFace); return 0;
		}
	}

	bool IsTextureTarget(GLenum target)
	{
		return target == GL_TEXTURE_2D || IsCubemapTextureTarget(target) || target == GL_TEXTURE_3D || target == GL_TEXTURE_2D_ARRAY;
	}

	bool ValidateTextureFormatType(GLenum format, GLenum type, GLint internalformat, GLint clientVersion)
	{
		switch(type)
		{
		case GL_UNSIGNED_BYTE:
		case GL_UNSIGNED_SHORT_4_4_4_4:
		case GL_UNSIGNED_SHORT_5_5_5_1:
		case GL_UNSIGNED_SHORT_5_6_5:
		case GL_FLOAT:               // GL_OES_texture_float
		case GL_HALF_FLOAT_OES:      // GL_OES_texture_half_float
		case GL_UNSIGNED_INT_24_8:   // GL_OES_packed_depth_stencil (GL_UNSIGNED_INT_24_8_EXT)
		case GL_UNSIGNED_SHORT:      // GL_OES_depth_texture
		case GL_UNSIGNED_INT:        // GL_OES_depth_texture
			break;
		case GL_BYTE:
		case GL_SHORT:
		case GL_INT:
		case GL_HALF_FLOAT:
		case GL_UNSIGNED_INT_2_10_10_10_REV:
		case GL_UNSIGNED_INT_10F_11F_11F_REV:
		case GL_UNSIGNED_INT_5_9_9_9_REV:
		case GL_FLOAT_32_UNSIGNED_INT_24_8_REV:
			if(clientVersion < 3)
			{
				return error(GL_INVALID_ENUM, false);
			}
			break;
		default:
			return error(GL_INVALID_ENUM, false);
		}

		switch(format)
		{
		case GL_ALPHA:
		case GL_RGB:
		case GL_RGBA:
		case GL_LUMINANCE:
		case GL_LUMINANCE_ALPHA:
		case GL_BGRA_EXT:          // GL_EXT_texture_format_BGRA8888
		case GL_DEPTH_STENCIL:     // GL_OES_packed_depth_stencil (GL_DEPTH_STENCIL_OES)
		case GL_DEPTH_COMPONENT:   // GL_OES_depth_texture
			break;
		case GL_RED:
		case GL_RED_INTEGER:
		case GL_RG:
		case GL_RG_INTEGER:
		case GL_RGB_INTEGER:
		case GL_RGBA_INTEGER:
			if(clientVersion < 3)
			{
				return error(GL_INVALID_ENUM, false);
			}
			break;
		default:
			return error(GL_INVALID_ENUM, false);
		}

		if(internalformat != format)
		{
			if(clientVersion < 3)
			{
				return error(GL_INVALID_OPERATION, false);
			}

			switch(internalformat)
			{
			case GL_R8:
			case GL_R8UI:
			case GL_R8I:
			case GL_R16UI:
			case GL_R16I:
			case GL_R32UI:
			case GL_R32I:
			case GL_RG8:
			case GL_RG8UI:
			case GL_RG8I:
			case GL_RG16UI:
			case GL_RG16I:
			case GL_RG32UI:
			case GL_RG32I:
			case GL_SRGB8_ALPHA8:
			case GL_RGB8UI:
			case GL_RGB8I:
			case GL_RGB16UI:
			case GL_RGB16I:
			case GL_RGB32UI:
			case GL_RGB32I:
			case GL_RG8_SNORM:
			case GL_R8_SNORM:
			case GL_RGB10_A2:
			case GL_RGBA8UI:
			case GL_RGBA8I:
			case GL_RGB10_A2UI:
			case GL_RGBA16UI:
			case GL_RGBA16I:
			case GL_RGBA32I:
			case GL_RGBA32UI:
			case GL_RGBA4:
			case GL_RGB5_A1:
			case GL_RGB565:
			case GL_RGB8_OES:
			case GL_RGBA8_OES:
			case GL_R16F:
			case GL_RG16F:
			case GL_R11F_G11F_B10F:
			case GL_RGB16F:
			case GL_RGBA16F:
			case GL_R32F:
			case GL_RG32F:
			case GL_RGB32F:
			case GL_RGBA32F:
			case GL_DEPTH_COMPONENT24:
			case GL_DEPTH_COMPONENT32_OES:
			case GL_DEPTH_COMPONENT32F:
			case GL_DEPTH32F_STENCIL8:
			case GL_DEPTH_COMPONENT16:
			case GL_STENCIL_INDEX8:
			case GL_DEPTH24_STENCIL8_OES:
			case GL_RGBA8_SNORM:
			case GL_SRGB8:
			case GL_RGB8_SNORM:
			case GL_RGB9_E5:
				break;
			default:
				return error(GL_INVALID_ENUM, false);
			}
		}

		// Validate format, type, and sized internalformat combinations [OpenGL ES 3.0 Table 3.2]
		bool validSizedInternalformat = false;
		#define VALIDATE_INTERNALFORMAT(...) { GLenum validInternalformats[] = {__VA_ARGS__}; for(GLenum v : validInternalformats) {if(internalformat == v) validSizedInternalformat = true;} } break;

		switch(format)
		{
		case GL_RGBA:
			switch(type)
			{
			case GL_UNSIGNED_BYTE:               VALIDATE_INTERNALFORMAT(GL_RGBA8, GL_RGB5_A1, GL_RGBA4, GL_SRGB8_ALPHA8)
			case GL_BYTE:                        VALIDATE_INTERNALFORMAT(GL_RGBA8_SNORM)
			case GL_HALF_FLOAT_OES:              break;
			case GL_UNSIGNED_SHORT_4_4_4_4:      VALIDATE_INTERNALFORMAT(GL_RGBA4)
			case GL_UNSIGNED_SHORT_5_5_5_1:      VALIDATE_INTERNALFORMAT(GL_RGB5_A1)
			case GL_UNSIGNED_INT_2_10_10_10_REV: VALIDATE_INTERNALFORMAT(GL_RGB10_A2, GL_RGB5_A1)
			case GL_HALF_FLOAT:                  VALIDATE_INTERNALFORMAT(GL_RGBA16F)
			case GL_FLOAT:                       VALIDATE_INTERNALFORMAT(GL_RGBA32F, GL_RGBA16F)
			default:                             return error(GL_INVALID_OPERATION, false);
			}
			break;
		case GL_RGBA_INTEGER:
			switch(type)
			{
			case GL_UNSIGNED_BYTE:               VALIDATE_INTERNALFORMAT(GL_RGBA8UI)
			case GL_BYTE:                        VALIDATE_INTERNALFORMAT(GL_RGBA8I)
			case GL_UNSIGNED_SHORT:              VALIDATE_INTERNALFORMAT(GL_RGBA16UI)
			case GL_SHORT:                       VALIDATE_INTERNALFORMAT(GL_RGBA16I)
			case GL_UNSIGNED_INT:                VALIDATE_INTERNALFORMAT(GL_RGBA32UI)
			case GL_INT:                         VALIDATE_INTERNALFORMAT(GL_RGBA32I)
			case GL_UNSIGNED_INT_2_10_10_10_REV: VALIDATE_INTERNALFORMAT(GL_RGB10_A2UI)
			default:                             return error(GL_INVALID_OPERATION, false);
			}
			break;
		case GL_RGB:
			switch(type)
			{
			case GL_UNSIGNED_BYTE:                VALIDATE_INTERNALFORMAT(GL_RGB8, GL_RGB565, GL_SRGB8)
			case GL_BYTE:                         VALIDATE_INTERNALFORMAT(GL_RGB8_SNORM)
			case GL_HALF_FLOAT_OES:               break;
			case GL_UNSIGNED_SHORT_5_6_5:         VALIDATE_INTERNALFORMAT(GL_RGB565)
			case GL_UNSIGNED_INT_10F_11F_11F_REV: VALIDATE_INTERNALFORMAT(GL_R11F_G11F_B10F)
			case GL_UNSIGNED_INT_5_9_9_9_REV:     VALIDATE_INTERNALFORMAT(GL_RGB9_E5)
			case GL_HALF_FLOAT:                   VALIDATE_INTERNALFORMAT(GL_RGB16F, GL_R11F_G11F_B10F, GL_RGB9_E5)
			case GL_FLOAT:                        VALIDATE_INTERNALFORMAT(GL_RGB32F, GL_RGB16F, GL_R11F_G11F_B10F, GL_RGB9_E5)
			default:                              return error(GL_INVALID_OPERATION, false);
			}
			break;
		case GL_RGB_INTEGER:
			switch(type)
			{
			case GL_UNSIGNED_BYTE:               VALIDATE_INTERNALFORMAT(GL_RGB8UI)
			case GL_BYTE:                        VALIDATE_INTERNALFORMAT(GL_RGB8I)
			case GL_UNSIGNED_SHORT:              VALIDATE_INTERNALFORMAT(GL_RGB16UI)
			case GL_SHORT:                       VALIDATE_INTERNALFORMAT(GL_RGB16I)
			case GL_UNSIGNED_INT:                VALIDATE_INTERNALFORMAT(GL_RGB32UI)
			case GL_INT:                         VALIDATE_INTERNALFORMAT(GL_RGB32I)
			default:                             return error(GL_INVALID_OPERATION, false);
			}
			break;
		case GL_RG:
			switch(type)
			{
			case GL_UNSIGNED_BYTE: VALIDATE_INTERNALFORMAT(GL_RG8)
			case GL_BYTE:          VALIDATE_INTERNALFORMAT(GL_RG8_SNORM)
			case GL_HALF_FLOAT:    VALIDATE_INTERNALFORMAT(GL_RG16F)
			case GL_FLOAT:         VALIDATE_INTERNALFORMAT(GL_RG32F, GL_RG16F)
			default:               return error(GL_INVALID_OPERATION, false);
			}
			break;
		case GL_RG_INTEGER:
			switch(type)
			{
			case GL_UNSIGNED_BYTE:  VALIDATE_INTERNALFORMAT(GL_RG8UI)
			case GL_BYTE:           VALIDATE_INTERNALFORMAT(GL_RG8I)
			case GL_UNSIGNED_SHORT: VALIDATE_INTERNALFORMAT(GL_RG16UI)
			case GL_SHORT:          VALIDATE_INTERNALFORMAT(GL_RG16I)
			case GL_UNSIGNED_INT:   VALIDATE_INTERNALFORMAT(GL_RG32UI)
			case GL_INT:            VALIDATE_INTERNALFORMAT(GL_RG32I)
			default:                return error(GL_INVALID_OPERATION, false);
			}
			break;
		case GL_RED:
			switch(type)
			{
			case GL_UNSIGNED_BYTE: VALIDATE_INTERNALFORMAT(GL_R8)
			case GL_BYTE:          VALIDATE_INTERNALFORMAT(GL_R8_SNORM)
			case GL_HALF_FLOAT:    VALIDATE_INTERNALFORMAT(GL_R16F)
			case GL_FLOAT:         VALIDATE_INTERNALFORMAT(GL_R32F, GL_R16F)
			default:               return error(GL_INVALID_OPERATION, false);
			}
			break;
		case GL_RED_INTEGER:
			switch(type)
			{
			case GL_UNSIGNED_BYTE:  VALIDATE_INTERNALFORMAT(GL_R8UI)
			case GL_BYTE:           VALIDATE_INTERNALFORMAT(GL_R8I)
			case GL_UNSIGNED_SHORT: VALIDATE_INTERNALFORMAT(GL_R16UI)
			case GL_SHORT:          VALIDATE_INTERNALFORMAT(GL_R16I)
			case GL_UNSIGNED_INT:   VALIDATE_INTERNALFORMAT(GL_R32UI)
			case GL_INT:            VALIDATE_INTERNALFORMAT(GL_R32I)
			default:                return error(GL_INVALID_OPERATION, false);
			}
			break;
		case GL_DEPTH_COMPONENT:
			switch(type)
			{
			case GL_UNSIGNED_SHORT: VALIDATE_INTERNALFORMAT(GL_DEPTH_COMPONENT16)
			case GL_UNSIGNED_INT:   VALIDATE_INTERNALFORMAT(GL_DEPTH_COMPONENT24, GL_DEPTH_COMPONENT16)
			case GL_FLOAT:          VALIDATE_INTERNALFORMAT(GL_DEPTH_COMPONENT32F)
			default:                return error(GL_INVALID_OPERATION, false);
			}
			break;
		case GL_DEPTH_STENCIL:
			switch(type)
			{
			case GL_UNSIGNED_INT_24_8:              VALIDATE_INTERNALFORMAT(GL_DEPTH24_STENCIL8)
			case GL_FLOAT_32_UNSIGNED_INT_24_8_REV: VALIDATE_INTERNALFORMAT(GL_DEPTH32F_STENCIL8)
			default:                                return error(GL_INVALID_OPERATION, false);
			}
			break;
		case GL_LUMINANCE_ALPHA:
		case GL_LUMINANCE:
		case GL_ALPHA:
			switch(type)
			{
			case GL_UNSIGNED_BYTE:
			case GL_HALF_FLOAT_OES:
			case GL_FLOAT:
				break;
			default:
				return error(GL_INVALID_OPERATION, false);
			}
			break;
		case GL_BGRA_EXT:
			if(type != GL_UNSIGNED_BYTE)
			{
				return error(GL_INVALID_OPERATION, false);
			}
			break;
		default:
			UNREACHABLE(format);
			return error(GL_INVALID_ENUM, false);
		}

		#undef VALIDATE_INTERNALFORMAT

		if(internalformat != format && !validSizedInternalformat)
		{
			return error(GL_INVALID_OPERATION, false);
		}

		return true;
	}

	bool IsColorRenderable(GLenum internalformat, GLint clientVersion)
	{
		switch(internalformat)
		{
		case GL_R8:
		case GL_R8UI:
		case GL_R8I:
		case GL_R16UI:
		case GL_R16I:
		case GL_R32UI:
		case GL_R32I:
		case GL_RG8:
		case GL_RG8UI:
		case GL_RG8I:
		case GL_RG16UI:
		case GL_RG16I:
		case GL_RG32UI:
		case GL_RG32I:
		case GL_SRGB8_ALPHA8:
		case GL_RGB10_A2:
		case GL_RGBA8UI:
		case GL_RGBA8I:
		case GL_RGB10_A2UI:
		case GL_RGBA16UI:
		case GL_RGBA16I:
		case GL_RGBA32I:
		case GL_RGBA32UI:
		case GL_RGBA4:
		case GL_RGB5_A1:
		case GL_RGB565:
		case GL_RGB8_OES:
		case GL_RGBA8_OES:
		case GL_RGB:
		case GL_RGBA:
			return true;
		case GL_R16F:
		case GL_RG16F:
		case GL_R11F_G11F_B10F:
		case GL_RGB16F:
		case GL_RGBA16F:
		case GL_R32F:
		case GL_RG32F:
		case GL_RGB32F:
		case GL_RGBA32F:
			return clientVersion >= 3;
		case GL_DEPTH_COMPONENT24:
		case GL_DEPTH_COMPONENT32_OES:
		case GL_DEPTH_COMPONENT32F:
		case GL_DEPTH32F_STENCIL8:
		case GL_DEPTH_COMPONENT16:
		case GL_STENCIL_INDEX8:
		case GL_DEPTH24_STENCIL8_OES:
			return false;
		default:
			UNIMPLEMENTED();
		}

		return false;
	}

	bool IsDepthRenderable(GLenum internalformat)
	{
		switch(internalformat)
		{
		case GL_DEPTH_COMPONENT24:
		case GL_DEPTH_COMPONENT32_OES:
		case GL_DEPTH_COMPONENT32F:
		case GL_DEPTH32F_STENCIL8:
		case GL_DEPTH_COMPONENT16:
		case GL_DEPTH24_STENCIL8_OES:
			return true;
		case GL_STENCIL_INDEX8:
		case GL_R8:
		case GL_R8UI:
		case GL_R8I:
		case GL_R16UI:
		case GL_R16I:
		case GL_R32UI:
		case GL_R32I:
		case GL_RG8:
		case GL_RG8UI:
		case GL_RG8I:
		case GL_RG16UI:
		case GL_RG16I:
		case GL_RG32UI:
		case GL_RG32I:
		case GL_SRGB8_ALPHA8:
		case GL_RGB10_A2:
		case GL_RGBA8UI:
		case GL_RGBA8I:
		case GL_RGB10_A2UI:
		case GL_RGBA16UI:
		case GL_RGBA16I:
		case GL_RGBA32I:
		case GL_RGBA32UI:
		case GL_RGBA4:
		case GL_RGB5_A1:
		case GL_RGB565:
		case GL_RGB8_OES:
		case GL_RGBA8_OES:
		case GL_RGB:
		case GL_RGBA:
		case GL_R16F:
		case GL_RG16F:
		case GL_R11F_G11F_B10F:
		case GL_RGB16F:
		case GL_RGBA16F:
		case GL_R32F:
		case GL_RG32F:
		case GL_RGB32F:
		case GL_RGBA32F:
			return false;
		default:
			UNIMPLEMENTED();
		}

		return false;
	}

	bool IsStencilRenderable(GLenum internalformat)
	{
		switch(internalformat)
		{
		case GL_DEPTH32F_STENCIL8:
		case GL_STENCIL_INDEX8:
		case GL_DEPTH24_STENCIL8_OES:
			return true;
		case GL_R8:
		case GL_R8UI:
		case GL_R8I:
		case GL_R16UI:
		case GL_R16I:
		case GL_R32UI:
		case GL_R32I:
		case GL_RG8:
		case GL_RG8UI:
		case GL_RG8I:
		case GL_RG16UI:
		case GL_RG16I:
		case GL_RG32UI:
		case GL_RG32I:
		case GL_SRGB8_ALPHA8:
		case GL_RGB10_A2:
		case GL_RGBA8UI:
		case GL_RGBA8I:
		case GL_RGB10_A2UI:
		case GL_RGBA16UI:
		case GL_RGBA16I:
		case GL_RGBA32I:
		case GL_RGBA32UI:
		case GL_RGBA4:
		case GL_RGB5_A1:
		case GL_RGB565:
		case GL_RGB8_OES:
		case GL_RGBA8_OES:
		case GL_RGB:
		case GL_RGBA:
		case GL_R16F:
		case GL_RG16F:
		case GL_R11F_G11F_B10F:
		case GL_RGB16F:
		case GL_RGBA16F:
		case GL_R32F:
		case GL_RG32F:
		case GL_RGB32F:
		case GL_RGBA32F:
		case GL_DEPTH_COMPONENT16:
		case GL_DEPTH_COMPONENT24:
		case GL_DEPTH_COMPONENT32_OES:
		case GL_DEPTH_COMPONENT32F:
			return false;
		default:
			UNIMPLEMENTED();
		}

		return false;
	}

	std::string ParseUniformName(const std::string &name, size_t *outSubscript)
	{
		// Strip any trailing array operator and retrieve the subscript
		size_t open = name.find_last_of('[');
		size_t close = name.find_last_of(']');
		bool hasIndex = (open != std::string::npos) && (close == name.length() - 1);
		if(!hasIndex)
		{
			if(outSubscript)
			{
				*outSubscript = GL_INVALID_INDEX;
			}
			return name;
		}

		if(outSubscript)
		{
			int index = atoi(name.substr(open + 1).c_str());
			if(index >= 0)
			{
				*outSubscript = index;
			}
			else
			{
				*outSubscript = GL_INVALID_INDEX;
			}
		}

		return name.substr(0, open);
	}
}

namespace es2sw
{
	sw::DepthCompareMode ConvertDepthComparison(GLenum comparison)
	{
		switch(comparison)
		{
		case GL_NEVER:    return sw::DEPTH_NEVER;
		case GL_ALWAYS:   return sw::DEPTH_ALWAYS;
		case GL_LESS:     return sw::DEPTH_LESS;
		case GL_LEQUAL:   return sw::DEPTH_LESSEQUAL;
		case GL_EQUAL:    return sw::DEPTH_EQUAL;
		case GL_GREATER:  return sw::DEPTH_GREATER;
		case GL_GEQUAL:   return sw::DEPTH_GREATEREQUAL;
		case GL_NOTEQUAL: return sw::DEPTH_NOTEQUAL;
		default: UNREACHABLE(comparison);
		}

		return sw::DEPTH_ALWAYS;
	}

	sw::StencilCompareMode ConvertStencilComparison(GLenum comparison)
	{
		switch(comparison)
		{
		case GL_NEVER:    return sw::STENCIL_NEVER;
		case GL_ALWAYS:   return sw::STENCIL_ALWAYS;
		case GL_LESS:     return sw::STENCIL_LESS;
		case GL_LEQUAL:   return sw::STENCIL_LESSEQUAL;
		case GL_EQUAL:    return sw::STENCIL_EQUAL;
		case GL_GREATER:  return sw::STENCIL_GREATER;
		case GL_GEQUAL:   return sw::STENCIL_GREATEREQUAL;
		case GL_NOTEQUAL: return sw::STENCIL_NOTEQUAL;
		default: UNREACHABLE(comparison);
		}

		return sw::STENCIL_ALWAYS;
	}

	sw::Color<float> ConvertColor(es2::Color color)
	{
		return sw::Color<float>(color.red, color.green, color.blue, color.alpha);
	}

	sw::BlendFactor ConvertBlendFunc(GLenum blend)
	{
		switch(blend)
		{
		case GL_ZERO:                     return sw::BLEND_ZERO;
		case GL_ONE:                      return sw::BLEND_ONE;
		case GL_SRC_COLOR:                return sw::BLEND_SOURCE;
		case GL_ONE_MINUS_SRC_COLOR:      return sw::BLEND_INVSOURCE;
		case GL_DST_COLOR:                return sw::BLEND_DEST;
		case GL_ONE_MINUS_DST_COLOR:      return sw::BLEND_INVDEST;
		case GL_SRC_ALPHA:                return sw::BLEND_SOURCEALPHA;
		case GL_ONE_MINUS_SRC_ALPHA:      return sw::BLEND_INVSOURCEALPHA;
		case GL_DST_ALPHA:                return sw::BLEND_DESTALPHA;
		case GL_ONE_MINUS_DST_ALPHA:      return sw::BLEND_INVDESTALPHA;
		case GL_CONSTANT_COLOR:           return sw::BLEND_CONSTANT;
		case GL_ONE_MINUS_CONSTANT_COLOR: return sw::BLEND_INVCONSTANT;
		case GL_CONSTANT_ALPHA:           return sw::BLEND_CONSTANTALPHA;
		case GL_ONE_MINUS_CONSTANT_ALPHA: return sw::BLEND_INVCONSTANTALPHA;
		case GL_SRC_ALPHA_SATURATE:       return sw::BLEND_SRCALPHASAT;
		default: UNREACHABLE(blend);
		}

		return sw::BLEND_ZERO;
	}

	sw::BlendOperation ConvertBlendOp(GLenum blendOp)
	{
		switch(blendOp)
		{
		case GL_FUNC_ADD:              return sw::BLENDOP_ADD;
		case GL_FUNC_SUBTRACT:         return sw::BLENDOP_SUB;
		case GL_FUNC_REVERSE_SUBTRACT: return sw::BLENDOP_INVSUB;
		case GL_MIN_EXT:               return sw::BLENDOP_MIN;
		case GL_MAX_EXT:               return sw::BLENDOP_MAX;
		default: UNREACHABLE(blendOp);
		}

		return sw::BLENDOP_ADD;
	}

	sw::StencilOperation ConvertStencilOp(GLenum stencilOp)
	{
		switch(stencilOp)
		{
		case GL_ZERO:      return sw::OPERATION_ZERO;
		case GL_KEEP:      return sw::OPERATION_KEEP;
		case GL_REPLACE:   return sw::OPERATION_REPLACE;
		case GL_INCR:      return sw::OPERATION_INCRSAT;
		case GL_DECR:      return sw::OPERATION_DECRSAT;
		case GL_INVERT:    return sw::OPERATION_INVERT;
		case GL_INCR_WRAP: return sw::OPERATION_INCR;
		case GL_DECR_WRAP: return sw::OPERATION_DECR;
		default: UNREACHABLE(stencilOp);
		}

		return sw::OPERATION_KEEP;
	}

	sw::AddressingMode ConvertTextureWrap(GLenum wrap)
	{
		switch(wrap)
		{
		case GL_REPEAT:            return sw::ADDRESSING_WRAP;
		case GL_CLAMP_TO_EDGE:     return sw::ADDRESSING_CLAMP;
		case GL_MIRRORED_REPEAT:   return sw::ADDRESSING_MIRROR;
		default: UNREACHABLE(wrap);
		}

		return sw::ADDRESSING_WRAP;
	}

	sw::SwizzleType ConvertSwizzleType(GLenum swizzleType)
	{
		switch(swizzleType)
		{
		case GL_RED:   return sw::SWIZZLE_RED;
		case GL_GREEN: return sw::SWIZZLE_GREEN;
		case GL_BLUE:  return sw::SWIZZLE_BLUE;
		case GL_ALPHA: return sw::SWIZZLE_ALPHA;
		case GL_ZERO:  return sw::SWIZZLE_ZERO;
		case GL_ONE:   return sw::SWIZZLE_ONE;
		default: UNREACHABLE(swizzleType);
		}

		return sw::SWIZZLE_RED;
	};

	sw::CullMode ConvertCullMode(GLenum cullFace, GLenum frontFace)
	{
		switch(cullFace)
		{
		case GL_FRONT:
			return (frontFace == GL_CCW ? sw::CULL_CLOCKWISE : sw::CULL_COUNTERCLOCKWISE);
		case GL_BACK:
			return (frontFace == GL_CCW ? sw::CULL_COUNTERCLOCKWISE : sw::CULL_CLOCKWISE);
		case GL_FRONT_AND_BACK:
			return sw::CULL_NONE;   // culling will be handled during draw
		default: UNREACHABLE(cullFace);
		}

		return sw::CULL_COUNTERCLOCKWISE;
	}

	unsigned int ConvertColorMask(bool red, bool green, bool blue, bool alpha)
	{
		return (red   ? 0x00000001 : 0) |
			   (green ? 0x00000002 : 0) |
			   (blue  ? 0x00000004 : 0) |
			   (alpha ? 0x00000008 : 0);
	}

	sw::MipmapType ConvertMipMapFilter(GLenum minFilter)
	{
		switch(minFilter)
		{
		case GL_NEAREST:
		case GL_LINEAR:
			return sw::MIPMAP_NONE;
			break;
		case GL_NEAREST_MIPMAP_NEAREST:
		case GL_LINEAR_MIPMAP_NEAREST:
			return sw::MIPMAP_POINT;
			break;
		case GL_NEAREST_MIPMAP_LINEAR:
		case GL_LINEAR_MIPMAP_LINEAR:
			return sw::MIPMAP_LINEAR;
			break;
		default:
			UNREACHABLE(minFilter);
			return sw::MIPMAP_NONE;
		}
	}

	sw::FilterType ConvertTextureFilter(GLenum minFilter, GLenum magFilter, float maxAnisotropy)
	{
		if(maxAnisotropy > 1.0f)
		{
			return sw::FILTER_ANISOTROPIC;
		}

		sw::FilterType magFilterType = sw::FILTER_POINT;
		switch(magFilter)
		{
		case GL_NEAREST: magFilterType = sw::FILTER_POINT;  break;
		case GL_LINEAR:  magFilterType = sw::FILTER_LINEAR; break;
		default: UNREACHABLE(magFilter);
		}

		switch(minFilter)
		{
		case GL_NEAREST:
		case GL_NEAREST_MIPMAP_NEAREST:
		case GL_NEAREST_MIPMAP_LINEAR:
			return (magFilterType == sw::FILTER_POINT) ? sw::FILTER_POINT : sw::FILTER_MIN_POINT_MAG_LINEAR;
		case GL_LINEAR:
		case GL_LINEAR_MIPMAP_NEAREST:
		case GL_LINEAR_MIPMAP_LINEAR:
			return (magFilterType == sw::FILTER_POINT) ? sw::FILTER_MIN_LINEAR_MAG_POINT : sw::FILTER_LINEAR;
		default:
			UNREACHABLE(minFilter);
			return (magFilterType == sw::FILTER_POINT) ? sw::FILTER_POINT : sw::FILTER_MIN_POINT_MAG_LINEAR;
		}
	}

	bool ConvertPrimitiveType(GLenum primitiveType, GLsizei elementCount, GLenum elementType,  sw::DrawType &drawType, int &primitiveCount)
	{
		switch(primitiveType)
		{
		case GL_POINTS:
			drawType = sw::DRAW_POINTLIST;
			primitiveCount = elementCount;
			break;
		case GL_LINES:
			drawType = sw::DRAW_LINELIST;
			primitiveCount = elementCount / 2;
			break;
		case GL_LINE_LOOP:
			drawType = sw::DRAW_LINELOOP;
			primitiveCount = elementCount;
			break;
		case GL_LINE_STRIP:
			drawType = sw::DRAW_LINESTRIP;
			primitiveCount = elementCount - 1;
			break;
		case GL_TRIANGLES:
			drawType = sw::DRAW_TRIANGLELIST;
			primitiveCount = elementCount / 3;
			break;
		case GL_TRIANGLE_STRIP:
			drawType = sw::DRAW_TRIANGLESTRIP;
			primitiveCount = elementCount - 2;
			break;
		case GL_TRIANGLE_FAN:
			drawType = sw::DRAW_TRIANGLEFAN;
			primitiveCount = elementCount - 2;
			break;
		default:
			return false;
		}

		sw::DrawType elementSize;
		switch(elementType)
		{
		case GL_NONE:           elementSize = sw::DRAW_NONINDEXED; break;
		case GL_UNSIGNED_BYTE:  elementSize = sw::DRAW_INDEXED8;   break;
		case GL_UNSIGNED_SHORT: elementSize = sw::DRAW_INDEXED16;  break;
		case GL_UNSIGNED_INT:   elementSize = sw::DRAW_INDEXED32;  break;
		default: return false;
		}

		drawType = sw::DrawType(drawType | elementSize);

		return true;
	}

	sw::Format ConvertRenderbufferFormat(GLenum format)
	{
		switch(format)
		{
		case GL_RGBA4:
		case GL_RGB5_A1:
		case GL_RGBA8_OES:            return sw::FORMAT_A8B8G8R8;
		case GL_RGB565:               return sw::FORMAT_R5G6B5;
		case GL_RGB8_OES:             return sw::FORMAT_X8B8G8R8;
		case GL_DEPTH_COMPONENT16:
		case GL_STENCIL_INDEX8:
		case GL_DEPTH24_STENCIL8_OES: return sw::FORMAT_D24S8;
		case GL_DEPTH_COMPONENT32_OES:return sw::FORMAT_D32;
		case GL_R8:                   return sw::FORMAT_R8;
		case GL_RG8:                  return sw::FORMAT_G8R8;
		case GL_R8I:                  return sw::FORMAT_R8I;
		case GL_RG8I:                 return sw::FORMAT_G8R8I;
		case GL_RGB8I:                return sw::FORMAT_X8B8G8R8I;
		case GL_RGBA8I:               return sw::FORMAT_A8B8G8R8I;
		case GL_R8UI:                 return sw::FORMAT_R8UI;
		case GL_RG8UI:                return sw::FORMAT_G8R8UI;
		case GL_RGB8UI:               return sw::FORMAT_X8B8G8R8UI;
		case GL_RGBA8UI:              return sw::FORMAT_A8B8G8R8UI;
		case GL_R16I:                 return sw::FORMAT_R16I;
		case GL_RG16I:                return sw::FORMAT_G16R16I;
		case GL_RGB16I:               return sw::FORMAT_X16B16G16R16I;
		case GL_RGBA16I:              return sw::FORMAT_A16B16G16R16I;
		case GL_R16UI:                return sw::FORMAT_R16UI;
		case GL_RG16UI:               return sw::FORMAT_G16R16UI;
		case GL_RGB16UI:              return sw::FORMAT_X16B16G16R16UI;
		case GL_RGB10_A2UI:
		case GL_RGBA16UI:             return sw::FORMAT_A16B16G16R16UI;
		case GL_R32I:                 return sw::FORMAT_R32I;
		case GL_RG32I:                return sw::FORMAT_G32R32I;
		case GL_RGB32I:               return sw::FORMAT_X32B32G32R32I;
		case GL_RGBA32I:              return sw::FORMAT_A32B32G32R32I;
		case GL_R32UI:                return sw::FORMAT_R32UI;
		case GL_RG32UI:               return sw::FORMAT_G32R32UI;
		case GL_RGB32UI:              return sw::FORMAT_X32B32G32R32UI;
		case GL_RGBA32UI:             return sw::FORMAT_A32B32G32R32UI;
		case GL_R16F:                 return sw::FORMAT_R16F;
		case GL_RG16F:                return sw::FORMAT_G16R16F;
		case GL_R11F_G11F_B10F:
		case GL_RGB16F:               return sw::FORMAT_B16G16R16F;
		case GL_RGBA16F:              return sw::FORMAT_A16B16G16R16F;
		case GL_R32F:                 return sw::FORMAT_R32F;
		case GL_RG32F:                return sw::FORMAT_G32R32F;
		case GL_RGB32F:               return sw::FORMAT_B32G32R32F;
		case GL_RGBA32F:              return sw::FORMAT_A32B32G32R32F;
		case GL_RGB10_A2:             return sw::FORMAT_A2B10G10R10;
		case GL_SRGB8_ALPHA8:         // FIXME: Implement SRGB
		default: UNREACHABLE(format); return sw::FORMAT_NULL;
		}
	}
}

namespace sw2es
{
	unsigned int GetStencilSize(sw::Format stencilFormat)
	{
		switch(stencilFormat)
		{
		case sw::FORMAT_D24FS8:
		case sw::FORMAT_D24S8:
		case sw::FORMAT_D32FS8_TEXTURE:
		case sw::FORMAT_D32FS8_SHADOW:
		case sw::FORMAT_S8:
			return 8;
	//	case sw::FORMAT_D24X4S4:
	//		return 4;
	//	case sw::FORMAT_D15S1:
	//		return 1;
	//	case sw::FORMAT_D16_LOCKABLE:
		case sw::FORMAT_D32:
		case sw::FORMAT_D24X8:
		case sw::FORMAT_D32F_LOCKABLE:
		case sw::FORMAT_D16:
			return 0;
	//	case sw::FORMAT_D32_LOCKABLE:  return 0;
	//	case sw::FORMAT_S8_LOCKABLE:   return 8;
		default:
			return 0;
		}
	}

	unsigned int GetAlphaSize(sw::Format colorFormat)
	{
		switch(colorFormat)
		{
		case sw::FORMAT_A16B16G16R16F:
		case sw::FORMAT_A16B16G16R16I:
		case sw::FORMAT_A16B16G16R16UI:
			return 16;
		case sw::FORMAT_A32B32G32R32F:
		case sw::FORMAT_A32B32G32R32I:
		case sw::FORMAT_A32B32G32R32UI:
			return 32;
		case sw::FORMAT_A2R10G10B10:
			return 2;
		case sw::FORMAT_A8R8G8B8:
		case sw::FORMAT_A8B8G8R8:
		case sw::FORMAT_A8B8G8R8I:
		case sw::FORMAT_A8B8G8R8UI:
		case sw::FORMAT_A8B8G8R8I_SNORM:
			return 8;
		case sw::FORMAT_A2B10G10R10:
			return 2;
		case sw::FORMAT_A1R5G5B5:
			return 1;
		case sw::FORMAT_X8R8G8B8:
		case sw::FORMAT_X8B8G8R8:
		case sw::FORMAT_R5G6B5:
			return 0;
		default:
			return 0;
		}
	}

	unsigned int GetRedSize(sw::Format colorFormat)
	{
		switch(colorFormat)
		{
		case sw::FORMAT_R16F:
		case sw::FORMAT_G16R16F:
		case sw::FORMAT_B16G16R16F:
		case sw::FORMAT_A16B16G16R16F:
		case sw::FORMAT_R16I:
		case sw::FORMAT_G16R16I:
		case sw::FORMAT_X16B16G16R16I:
		case sw::FORMAT_A16B16G16R16I:
		case sw::FORMAT_R16UI:
		case sw::FORMAT_G16R16UI:
		case sw::FORMAT_X16B16G16R16UI:
		case sw::FORMAT_A16B16G16R16UI:
			return 16;
		case sw::FORMAT_R32F:
		case sw::FORMAT_G32R32F:
		case sw::FORMAT_B32G32R32F:
		case sw::FORMAT_A32B32G32R32F:
		case sw::FORMAT_R32I:
		case sw::FORMAT_G32R32I:
		case sw::FORMAT_X32B32G32R32I:
		case sw::FORMAT_A32B32G32R32I:
		case sw::FORMAT_R32UI:
		case sw::FORMAT_G32R32UI:
		case sw::FORMAT_X32B32G32R32UI:
		case sw::FORMAT_A32B32G32R32UI:
			return 32;
		case sw::FORMAT_A2B10G10R10:
		case sw::FORMAT_A2R10G10B10:
			return 10;
		case sw::FORMAT_A8R8G8B8:
		case sw::FORMAT_A8B8G8R8:
		case sw::FORMAT_X8R8G8B8:
		case sw::FORMAT_X8B8G8R8:
		case sw::FORMAT_R8:
		case sw::FORMAT_G8R8:
		case sw::FORMAT_R8I:
		case sw::FORMAT_G8R8I:
		case sw::FORMAT_X8B8G8R8I:
		case sw::FORMAT_A8B8G8R8I:
		case sw::FORMAT_R8UI:
		case sw::FORMAT_G8R8UI:
		case sw::FORMAT_X8B8G8R8UI:
		case sw::FORMAT_A8B8G8R8UI:
		case sw::FORMAT_R8I_SNORM:
		case sw::FORMAT_G8R8I_SNORM:
		case sw::FORMAT_X8B8G8R8I_SNORM:
		case sw::FORMAT_A8B8G8R8I_SNORM:
			return 8;
		case sw::FORMAT_A1R5G5B5:
		case sw::FORMAT_R5G6B5:
			return 5;
		default:
			return 0;
		}
	}

	unsigned int GetGreenSize(sw::Format colorFormat)
	{
		switch(colorFormat)
		{
		case sw::FORMAT_G16R16F:
		case sw::FORMAT_B16G16R16F:
		case sw::FORMAT_A16B16G16R16F:
		case sw::FORMAT_G16R16I:
		case sw::FORMAT_X16B16G16R16I:
		case sw::FORMAT_A16B16G16R16I:
		case sw::FORMAT_G16R16UI:
		case sw::FORMAT_X16B16G16R16UI:
		case sw::FORMAT_A16B16G16R16UI:
			return 16;
		case sw::FORMAT_G32R32F:
		case sw::FORMAT_B32G32R32F:
		case sw::FORMAT_A32B32G32R32F:
		case sw::FORMAT_G32R32I:
		case sw::FORMAT_X32B32G32R32I:
		case sw::FORMAT_A32B32G32R32I:
		case sw::FORMAT_G32R32UI:
		case sw::FORMAT_X32B32G32R32UI:
		case sw::FORMAT_A32B32G32R32UI:
			return 32;
		case sw::FORMAT_A2B10G10R10:
		case sw::FORMAT_A2R10G10B10:
			return 10;
		case sw::FORMAT_A8R8G8B8:
		case sw::FORMAT_A8B8G8R8:
		case sw::FORMAT_X8R8G8B8:
		case sw::FORMAT_X8B8G8R8:
		case sw::FORMAT_G8R8:
		case sw::FORMAT_G8R8I:
		case sw::FORMAT_X8B8G8R8I:
		case sw::FORMAT_A8B8G8R8I:
		case sw::FORMAT_G8R8UI:
		case sw::FORMAT_X8B8G8R8UI:
		case sw::FORMAT_A8B8G8R8UI:
		case sw::FORMAT_G8R8I_SNORM:
		case sw::FORMAT_X8B8G8R8I_SNORM:
		case sw::FORMAT_A8B8G8R8I_SNORM:
			return 8;
		case sw::FORMAT_A1R5G5B5:
			return 5;
		case sw::FORMAT_R5G6B5:
			return 6;
		default:
			return 0;
		}
	}

	unsigned int GetBlueSize(sw::Format colorFormat)
	{
		switch(colorFormat)
		{
		case sw::FORMAT_B16G16R16F:
		case sw::FORMAT_A16B16G16R16F:
		case sw::FORMAT_X16B16G16R16I:
		case sw::FORMAT_A16B16G16R16I:
		case sw::FORMAT_X16B16G16R16UI:
		case sw::FORMAT_A16B16G16R16UI:
			return 16;
		case sw::FORMAT_B32G32R32F:
		case sw::FORMAT_A32B32G32R32F:
		case sw::FORMAT_X32B32G32R32I:
		case sw::FORMAT_A32B32G32R32I:
		case sw::FORMAT_X32B32G32R32UI:
		case sw::FORMAT_A32B32G32R32UI:
			return 32;
		case sw::FORMAT_A2B10G10R10:
		case sw::FORMAT_A2R10G10B10:
			return 10;
		case sw::FORMAT_A8R8G8B8:
		case sw::FORMAT_A8B8G8R8:
		case sw::FORMAT_X8R8G8B8:
		case sw::FORMAT_X8B8G8R8:
		case sw::FORMAT_X8B8G8R8I:
		case sw::FORMAT_A8B8G8R8I:
		case sw::FORMAT_X8B8G8R8UI:
		case sw::FORMAT_A8B8G8R8UI:
		case sw::FORMAT_X8B8G8R8I_SNORM:
		case sw::FORMAT_A8B8G8R8I_SNORM:
			return 8;
		case sw::FORMAT_A1R5G5B5:
		case sw::FORMAT_R5G6B5:
			return 5;
		default:
			return 0;
		}
	}

	unsigned int GetDepthSize(sw::Format depthFormat)
	{
		switch(depthFormat)
		{
	//	case sw::FORMAT_D16_LOCKABLE:   return 16;
		case sw::FORMAT_D32:            return 32;
	//	case sw::FORMAT_D15S1:          return 15;
		case sw::FORMAT_D24S8:          return 24;
		case sw::FORMAT_D24X8:          return 24;
	//	case sw::FORMAT_D24X4S4:        return 24;
		case sw::FORMAT_DF16S8:
		case sw::FORMAT_D16:            return 16;
		case sw::FORMAT_D32F:
		case sw::FORMAT_D32F_COMPLEMENTARY:
		case sw::FORMAT_D32F_LOCKABLE:  return 32;
		case sw::FORMAT_DF24S8:
		case sw::FORMAT_D24FS8:         return 24;
	//	case sw::FORMAT_D32_LOCKABLE:   return 32;
	//	case sw::FORMAT_S8_LOCKABLE:    return 0;
		case sw::FORMAT_D32FS8_SHADOW:
		case sw::FORMAT_D32FS8_TEXTURE: return 32;
		default:                        return 0;
		}
	}

	GLenum GetComponentType(sw::Format format, GLenum attachment)
	{
		// Can be one of GL_FLOAT, GL_INT, GL_UNSIGNED_INT, GL_SIGNED_NORMALIZED, or GL_UNSIGNED_NORMALIZED
		switch(attachment)
		{
		case GL_COLOR_ATTACHMENT0:
		case GL_COLOR_ATTACHMENT1:
		case GL_COLOR_ATTACHMENT2:
		case GL_COLOR_ATTACHMENT3:
		case GL_COLOR_ATTACHMENT4:
		case GL_COLOR_ATTACHMENT5:
		case GL_COLOR_ATTACHMENT6:
		case GL_COLOR_ATTACHMENT7:
		case GL_COLOR_ATTACHMENT8:
		case GL_COLOR_ATTACHMENT9:
		case GL_COLOR_ATTACHMENT10:
		case GL_COLOR_ATTACHMENT11:
		case GL_COLOR_ATTACHMENT12:
		case GL_COLOR_ATTACHMENT13:
		case GL_COLOR_ATTACHMENT14:
		case GL_COLOR_ATTACHMENT15:
		case GL_COLOR_ATTACHMENT16:
		case GL_COLOR_ATTACHMENT17:
		case GL_COLOR_ATTACHMENT18:
		case GL_COLOR_ATTACHMENT19:
		case GL_COLOR_ATTACHMENT20:
		case GL_COLOR_ATTACHMENT21:
		case GL_COLOR_ATTACHMENT22:
		case GL_COLOR_ATTACHMENT23:
		case GL_COLOR_ATTACHMENT24:
		case GL_COLOR_ATTACHMENT25:
		case GL_COLOR_ATTACHMENT26:
		case GL_COLOR_ATTACHMENT27:
		case GL_COLOR_ATTACHMENT28:
		case GL_COLOR_ATTACHMENT29:
		case GL_COLOR_ATTACHMENT30:
		case GL_COLOR_ATTACHMENT31:
			switch(format)
			{
			case sw::FORMAT_R8I:
			case sw::FORMAT_G8R8I:
			case sw::FORMAT_X8B8G8R8I:
			case sw::FORMAT_A8B8G8R8I:
			case sw::FORMAT_R16I:
			case sw::FORMAT_G16R16I:
			case sw::FORMAT_X16B16G16R16I:
			case sw::FORMAT_A16B16G16R16I:
			case sw::FORMAT_R32I:
			case sw::FORMAT_G32R32I:
			case sw::FORMAT_X32B32G32R32I:
			case sw::FORMAT_A32B32G32R32I:
				return GL_INT;
			case sw::FORMAT_R8UI:
			case sw::FORMAT_G8R8UI:
			case sw::FORMAT_X8B8G8R8UI:
			case sw::FORMAT_A8B8G8R8UI:
			case sw::FORMAT_R16UI:
			case sw::FORMAT_G16R16UI:
			case sw::FORMAT_X16B16G16R16UI:
			case sw::FORMAT_A16B16G16R16UI:
			case sw::FORMAT_R32UI:
			case sw::FORMAT_G32R32UI:
			case sw::FORMAT_X32B32G32R32UI:
			case sw::FORMAT_A32B32G32R32UI:
				return GL_UNSIGNED_INT;
			case sw::FORMAT_R16F:
			case sw::FORMAT_G16R16F:
			case sw::FORMAT_B16G16R16F:
			case sw::FORMAT_A16B16G16R16F:
			case sw::FORMAT_R32F:
			case sw::FORMAT_G32R32F:
			case sw::FORMAT_B32G32R32F:
			case sw::FORMAT_A32B32G32R32F:
				return GL_FLOAT;
			case sw::FORMAT_R8:
			case sw::FORMAT_G8R8:
			case sw::FORMAT_A2B10G10R10:
			case sw::FORMAT_A2R10G10B10:
			case sw::FORMAT_A8R8G8B8:
			case sw::FORMAT_A8B8G8R8:
			case sw::FORMAT_X8R8G8B8:
			case sw::FORMAT_X8B8G8R8:
			case sw::FORMAT_A1R5G5B5:
			case sw::FORMAT_R5G6B5:
				return GL_UNSIGNED_NORMALIZED;
			case sw::FORMAT_R8I_SNORM:
			case sw::FORMAT_X8B8G8R8I_SNORM:
			case sw::FORMAT_A8B8G8R8I_SNORM:
			case sw::FORMAT_G8R8I_SNORM:
				return GL_SIGNED_NORMALIZED;
			default:
				UNREACHABLE(format);
				return 0;
			}
		case GL_DEPTH_ATTACHMENT:
		case GL_STENCIL_ATTACHMENT:
			// Only color buffers may have integer components.
			return GL_FLOAT;
		default:
			UNREACHABLE(attachment);
			return 0;
		}
	}

	GLenum ConvertBackBufferFormat(sw::Format format)
	{
		switch(format)
		{
		case sw::FORMAT_A4R4G4B4: return GL_RGBA4;
		case sw::FORMAT_A8R8G8B8: return GL_RGBA8_OES;
		case sw::FORMAT_A8B8G8R8: return GL_RGBA8_OES;
		case sw::FORMAT_A1R5G5B5: return GL_RGB5_A1;
		case sw::FORMAT_R5G6B5:   return GL_RGB565;
		case sw::FORMAT_X8R8G8B8: return GL_RGB8_OES;
		case sw::FORMAT_X8B8G8R8: return GL_RGB8_OES;
		default:
			UNREACHABLE(format);
		}

		return GL_RGBA4;
	}

	GLenum ConvertDepthStencilFormat(sw::Format format)
	{
		switch(format)
		{
		case sw::FORMAT_D16:
		case sw::FORMAT_D24X8:
		case sw::FORMAT_D32:
			return GL_DEPTH_COMPONENT16;
		case sw::FORMAT_D24S8:
			return GL_DEPTH24_STENCIL8_OES;
		case sw::FORMAT_D32F:
		case sw::FORMAT_D32F_COMPLEMENTARY:
		case sw::FORMAT_D32F_LOCKABLE:
			return GL_DEPTH_COMPONENT32F;
		case sw::FORMAT_D32FS8_TEXTURE:
		case sw::FORMAT_D32FS8_SHADOW:
			return GL_DEPTH32F_STENCIL8;
		case sw::FORMAT_S8:
			return GL_STENCIL_INDEX8;
		default:
			UNREACHABLE(format);
		}

		return GL_DEPTH24_STENCIL8_OES;
	}
}
