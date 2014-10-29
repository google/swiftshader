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

#include "mathutil.h"
#include "Context.h"
#include "common/debug.h"

#include <limits>
#include <stdio.h>

namespace es1
{
	GLsizei ComputePitch(GLsizei width, GLenum format, GLenum type, GLint alignment)
	{
		ASSERT(alignment > 0 && isPow2(alignment));

		GLsizei rawPitch = ComputePixelSize(format, type) * width;
		return (rawPitch + alignment - 1) & ~(alignment - 1);
	}

	GLsizei ComputeCompressedPitch(GLsizei width, GLenum format)
	{
		return ComputeCompressedSize(width, 1, format);
	}

	GLsizei ComputeCompressedSize(GLsizei width, GLsizei height, GLenum format)
	{
		switch(format)
		{
		case GL_COMPRESSED_RGB_S3TC_DXT1_EXT:
		case GL_COMPRESSED_RGBA_S3TC_DXT1_EXT:
			return 8 * (GLsizei)ceil((float)width / 4.0f) * (GLsizei)ceil((float)height / 4.0f);
		default:
			return 0;
		}
	}

	bool IsCompressed(GLenum format)
	{
		if(format == GL_COMPRESSED_RGB_S3TC_DXT1_EXT ||
		   format == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	bool IsDepthTexture(GLenum format)
	{
		if(format == GL_DEPTH_STENCIL_OES)
		{
			return true;
		}

		return false;
	}

	bool IsStencilTexture(GLenum format)
	{
		if(format == GL_DEPTH_STENCIL_OES)
		{
			return true;
		}

		return false;
	}

	// Returns the size, in bytes, of a single texel in an Image
	int ComputePixelSize(GLenum format, GLenum type)
	{
		switch(type)
		{
		case GL_UNSIGNED_BYTE:
			switch(format)
			{
			case GL_ALPHA:           return sizeof(unsigned char);
			case GL_LUMINANCE:       return sizeof(unsigned char);
			case GL_LUMINANCE_ALPHA: return sizeof(unsigned char) * 2;
			case GL_RGB:             return sizeof(unsigned char) * 3;
			case GL_RGBA:            return sizeof(unsigned char) * 4;
			case GL_BGRA_EXT:        return sizeof(unsigned char) * 4;
			default: UNREACHABLE();
			}
			break;
		case GL_UNSIGNED_SHORT_4_4_4_4:
		case GL_UNSIGNED_SHORT_5_5_5_1:
		case GL_UNSIGNED_SHORT_5_6_5:
		case GL_UNSIGNED_SHORT:
			return sizeof(unsigned short);
		case GL_UNSIGNED_INT:
		case GL_UNSIGNED_INT_24_8_OES:
			return sizeof(unsigned int);
		case GL_FLOAT:
			switch(format)
			{
			case GL_ALPHA:           return sizeof(float);
			case GL_LUMINANCE:       return sizeof(float);
			case GL_LUMINANCE_ALPHA: return sizeof(float) * 2;
			case GL_RGB:             return sizeof(float) * 3;
			case GL_RGBA:            return sizeof(float) * 4;
			default: UNREACHABLE();
			}
			break;
		default: UNREACHABLE();
		}

		return 0;
	}

	bool IsCubemapTextureTarget(GLenum target)
	{
		return (target >= GL_TEXTURE_CUBE_MAP_POSITIVE_X_OES && target <= GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_OES);
	}

	int CubeFaceIndex(GLenum cubeFace)
	{
		switch(cubeFace)
		{
		case GL_TEXTURE_CUBE_MAP_OES:
		case GL_TEXTURE_CUBE_MAP_POSITIVE_X_OES: return 0;
		case GL_TEXTURE_CUBE_MAP_NEGATIVE_X_OES: return 1;
		case GL_TEXTURE_CUBE_MAP_POSITIVE_Y_OES: return 2;
		case GL_TEXTURE_CUBE_MAP_NEGATIVE_Y_OES: return 3;
		case GL_TEXTURE_CUBE_MAP_POSITIVE_Z_OES: return 4;
		case GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_OES: return 5;
		default: UNREACHABLE(); return 0;
		}
	}

	bool IsTextureTarget(GLenum target)
	{
		return target == GL_TEXTURE_2D;
	}

	// Verify that format/type are one of the combinations from table 3.4.
	bool CheckTextureFormatType(GLenum format, GLenum type)
	{
		switch(type)
		{
		case GL_UNSIGNED_BYTE:
			switch(format)
			{
			case GL_RGBA:
			case GL_BGRA_EXT:
			case GL_RGB:
			case GL_ALPHA:
			case GL_LUMINANCE:
			case GL_LUMINANCE_ALPHA:
				return true;
			default:
				return false;
			}
		case GL_FLOAT:
		case GL_UNSIGNED_SHORT_4_4_4_4:
		case GL_UNSIGNED_SHORT_5_5_5_1:
			return (format == GL_RGBA);
		case GL_UNSIGNED_SHORT_5_6_5:
			return (format == GL_RGB);
		case GL_UNSIGNED_INT_24_8_OES:
			return (format == GL_DEPTH_STENCIL_OES);
		default:
			return false;
		}
	}

	bool IsColorRenderable(GLenum internalformat)
	{
		switch(internalformat)
		{
		case GL_RGBA4_OES:
		case GL_RGB5_A1_OES:
		case GL_RGB565_OES:
		case GL_RGB8_OES:
		case GL_RGBA8_OES:
			return true;
		case GL_DEPTH_COMPONENT16_OES:
		case GL_STENCIL_INDEX8_OES:
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
		case GL_DEPTH_COMPONENT16_OES:
		case GL_DEPTH24_STENCIL8_OES:
			return true;
		case GL_STENCIL_INDEX8_OES:
		case GL_RGBA4_OES:
		case GL_RGB5_A1_OES:
		case GL_RGB565_OES:
		case GL_RGB8_OES:
		case GL_RGBA8_OES:
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
		case GL_STENCIL_INDEX8_OES:
		case GL_DEPTH24_STENCIL8_OES:
			return true;
		case GL_RGBA4_OES:
		case GL_RGB5_A1_OES:
		case GL_RGB565_OES:
		case GL_RGB8_OES:
		case GL_RGBA8_OES:
		case GL_DEPTH_COMPONENT16_OES:
			return false;
		default:
			UNIMPLEMENTED();
		}

		return false;
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
		default: UNREACHABLE();
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
		default: UNREACHABLE();
		}

		return sw::STENCIL_ALWAYS;
	}

	sw::Color<float> ConvertColor(es1::Color color)
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
		case GL_SRC_ALPHA_SATURATE:       return sw::BLEND_SRCALPHASAT;
		default: UNREACHABLE();
		}

		return sw::BLEND_ZERO;
	}

	sw::BlendOperation ConvertBlendOp(GLenum blendOp)
	{
		switch(blendOp)
		{
		case GL_FUNC_ADD_OES:              return sw::BLENDOP_ADD;
		case GL_FUNC_SUBTRACT_OES:         return sw::BLENDOP_SUB;
		case GL_FUNC_REVERSE_SUBTRACT_OES: return sw::BLENDOP_INVSUB;
		case GL_MIN_EXT:                   return sw::BLENDOP_MIN;
		case GL_MAX_EXT:                   return sw::BLENDOP_MAX;
		default: UNREACHABLE();
		}

		return sw::BLENDOP_ADD;
	}

	sw::StencilOperation ConvertStencilOp(GLenum stencilOp)
	{
		switch(stencilOp)
		{
		case GL_ZERO:          return sw::OPERATION_ZERO;
		case GL_KEEP:          return sw::OPERATION_KEEP;
		case GL_REPLACE:       return sw::OPERATION_REPLACE;
		case GL_INCR:          return sw::OPERATION_INCRSAT;
		case GL_DECR:          return sw::OPERATION_DECRSAT;
		case GL_INVERT:        return sw::OPERATION_INVERT;
		case GL_INCR_WRAP_OES: return sw::OPERATION_INCR;
		case GL_DECR_WRAP_OES: return sw::OPERATION_DECR;
		default: UNREACHABLE();
		}

		return sw::OPERATION_KEEP;
	}

	sw::AddressingMode ConvertTextureWrap(GLenum wrap)
	{
		switch(wrap)
		{
		case GL_REPEAT:              return sw::ADDRESSING_WRAP;
		case GL_CLAMP_TO_EDGE:       return sw::ADDRESSING_CLAMP;
		case GL_MIRRORED_REPEAT_OES: return sw::ADDRESSING_MIRROR;
		default: UNREACHABLE();
		}

		return sw::ADDRESSING_WRAP;
	}

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
		default: UNREACHABLE();
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

	sw::FilterType ConvertMagFilter(GLenum magFilter)
	{
		switch(magFilter)
		{
		case GL_NEAREST: return sw::FILTER_POINT;
		case GL_LINEAR:  return sw::FILTER_LINEAR;
		default: UNREACHABLE();
		}

		return sw::FILTER_POINT;
	}

	void ConvertMinFilter(GLenum texFilter, sw::FilterType *minFilter, sw::MipmapType *mipFilter, float maxAnisotropy)
	{
		switch(texFilter)
		{
		case GL_NEAREST:
			*minFilter = sw::FILTER_POINT;
			*mipFilter = sw::MIPMAP_NONE;
			break;
		case GL_LINEAR:
			*minFilter = sw::FILTER_LINEAR;
			*mipFilter = sw::MIPMAP_NONE;
			break;
		case GL_NEAREST_MIPMAP_NEAREST:
			*minFilter = sw::FILTER_POINT;
			*mipFilter = sw::MIPMAP_POINT;
			break;
		case GL_LINEAR_MIPMAP_NEAREST:
			*minFilter = sw::FILTER_LINEAR;
			*mipFilter = sw::MIPMAP_POINT;
			break;
		case GL_NEAREST_MIPMAP_LINEAR:
			*minFilter = sw::FILTER_POINT;
			*mipFilter = sw::MIPMAP_LINEAR;
			break;
		case GL_LINEAR_MIPMAP_LINEAR:
			*minFilter = sw::FILTER_LINEAR;
			*mipFilter = sw::MIPMAP_LINEAR;
			break;
		default:
			*minFilter = sw::FILTER_POINT;
			*mipFilter = sw::MIPMAP_NONE;
			UNREACHABLE();
		}

		if(maxAnisotropy > 1.0f)
		{
			*minFilter = sw::FILTER_ANISOTROPIC;
		}
	}

	bool ConvertPrimitiveType(GLenum primitiveType, GLsizei elementCount,  es1::PrimitiveType &swPrimitiveType, int &primitiveCount)
	{
		switch(primitiveType)
		{
		case GL_POINTS:
			swPrimitiveType = es1::DRAW_POINTLIST;
			primitiveCount = elementCount;
			break;
		case GL_LINES:
			swPrimitiveType = es1::DRAW_LINELIST;
			primitiveCount = elementCount / 2;
			break;
		case GL_LINE_LOOP:
			swPrimitiveType = es1::DRAW_LINELOOP;
			primitiveCount = elementCount;
			break;
		case GL_LINE_STRIP:
			swPrimitiveType = es1::DRAW_LINESTRIP;
			primitiveCount = elementCount - 1;
			break;
		case GL_TRIANGLES:
			swPrimitiveType = es1::DRAW_TRIANGLELIST;
			primitiveCount = elementCount / 3;
			break;
		case GL_TRIANGLE_STRIP:
			swPrimitiveType = es1::DRAW_TRIANGLESTRIP;
			primitiveCount = elementCount - 2;
			break;
		case GL_TRIANGLE_FAN:
			swPrimitiveType = es1::DRAW_TRIANGLEFAN;
			primitiveCount = elementCount - 2;
			break;
		default:
			return false;
		}

		return true;
	}

	sw::Format ConvertRenderbufferFormat(GLenum format)
	{
		switch(format)
		{
		case GL_RGBA4_OES:
		case GL_RGB5_A1_OES:
		case GL_RGBA8_OES:            return sw::FORMAT_A8R8G8B8;
		case GL_RGB565_OES:           return sw::FORMAT_R5G6B5;
		case GL_RGB8_OES:             return sw::FORMAT_X8R8G8B8;
		case GL_DEPTH_COMPONENT16_OES:
		case GL_STENCIL_INDEX8_OES:       
		case GL_DEPTH24_STENCIL8_OES: return sw::FORMAT_D24S8;
		default: UNREACHABLE();       return sw::FORMAT_A8R8G8B8;
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
			return 16;
		case sw::FORMAT_A32B32G32R32F:
			return 32;
		case sw::FORMAT_A2R10G10B10:
			return 2;
		case sw::FORMAT_A8R8G8B8:
			return 8;
		case sw::FORMAT_A1R5G5B5:
			return 1;
		case sw::FORMAT_X8R8G8B8:
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
		case sw::FORMAT_A16B16G16R16F:
			return 16;
		case sw::FORMAT_A32B32G32R32F:
			return 32;
		case sw::FORMAT_A2R10G10B10:
			return 10;
		case sw::FORMAT_A8R8G8B8:
		case sw::FORMAT_X8R8G8B8:
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
		case sw::FORMAT_A16B16G16R16F:
			return 16;
		case sw::FORMAT_A32B32G32R32F:
			return 32;
		case sw::FORMAT_A2R10G10B10:
			return 10;
		case sw::FORMAT_A8R8G8B8:
		case sw::FORMAT_X8R8G8B8:
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
		case sw::FORMAT_A16B16G16R16F:
			return 16;
		case sw::FORMAT_A32B32G32R32F:
			return 32;
		case sw::FORMAT_A2R10G10B10:
			return 10;
		case sw::FORMAT_A8R8G8B8:
		case sw::FORMAT_X8R8G8B8:
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
		case sw::FORMAT_D16:            return 16;
		case sw::FORMAT_D32F_LOCKABLE:  return 32;
		case sw::FORMAT_D24FS8:         return 24;
	//	case sw::FORMAT_D32_LOCKABLE:   return 32;
	//	case sw::FORMAT_S8_LOCKABLE:    return 0;
		case sw::FORMAT_D32FS8_TEXTURE: return 32;
		default:                        return 0;
		}
	}

	GLenum ConvertBackBufferFormat(sw::Format format)
	{
		switch(format)
		{
		case sw::FORMAT_A4R4G4B4: return GL_RGBA4_OES;
		case sw::FORMAT_A8R8G8B8: return GL_RGBA8_OES;
		case sw::FORMAT_A1R5G5B5: return GL_RGB5_A1_OES;
		case sw::FORMAT_R5G6B5:   return GL_RGB565_OES;
		case sw::FORMAT_X8R8G8B8: return GL_RGB8_OES;
		default:
			UNREACHABLE();
		}

		return GL_RGBA4_OES;
	}

	GLenum ConvertDepthStencilFormat(sw::Format format)
	{
		switch(format)
		{
		case sw::FORMAT_D16:
		case sw::FORMAT_D24X8:
		case sw::FORMAT_D32:
			return GL_DEPTH_COMPONENT16_OES;
		case sw::FORMAT_D24S8:
			return GL_DEPTH24_STENCIL8_OES;
		default:
			UNREACHABLE();
		}

		return GL_DEPTH24_STENCIL8_OES;
	}
}
