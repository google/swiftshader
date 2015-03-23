// SwiftShader Software Renderer
//
// Copyright(c) 2015 Google Inc.
//
// All rights reserved. No part of this software may be copied, distributed, transmitted,
// transcribed, stored in a retrieval system, translated into any human or computer
// language by any means, or disclosed to third parties without the explicit written
// agreement of Google Inc. Without such an agreement, no rights or licenses, express
// or implied, including but not limited to any patent rights, are granted to you.
//
// libGLESv3.cpp: Implements the exported OpenGL ES 3.0 functions.

#include "main.h"
#include "Framebuffer.h"
#include "Program.h"
#include "Texture.h"
#include "common/debug.h"

#define GL_APICALL
#include <GLES3/gl3.h>
#include <GLES2/gl2ext.h>

typedef std::pair<GLenum, GLenum> InternalFormatTypePair;
typedef std::map<InternalFormatTypePair, GLenum> FormatMap;

// A helper function to insert data into the format map with fewer characters.
static void InsertFormatMapping(FormatMap& map, GLenum internalformat, GLenum format, GLenum type)
{
	map[InternalFormatTypePair(internalformat, type)] = format;
}

static bool validImageSize(GLint level, GLsizei width, GLsizei height)
{
	if(level < 0 || level >= es2::IMPLEMENTATION_MAX_TEXTURE_LEVELS || width < 0 || height < 0)
	{
		return false;
	}

	return true;
}

static bool validateSubImageParams(bool compressed, GLsizei width, GLsizei height, GLint xoffset, GLint yoffset, GLenum target, GLint level, GLenum format, es2::Texture *texture)
{
	if(!texture)
	{
		return error(GL_INVALID_OPERATION, false);
	}

	if(compressed != texture->isCompressed(target, level))
	{
		return error(GL_INVALID_OPERATION, false);
	}

	if(format != GL_NONE && format != texture->getFormat(target, level))
	{
		return error(GL_INVALID_OPERATION, false);
	}

	if(compressed)
	{
		if((width % 4 != 0 && width != texture->getWidth(target, 0)) ||
		   (height % 4 != 0 && height != texture->getHeight(target, 0)))
		{
			return error(GL_INVALID_OPERATION, false);
		}
	}

	if(xoffset + width > texture->getWidth(target, level) ||
		yoffset + height > texture->getHeight(target, level))
	{
		return error(GL_INVALID_VALUE, false);
	}

	return true;
}

static bool validateSubImageParams(bool compressed, GLsizei width, GLsizei height, GLsizei depth, GLint xoffset, GLint yoffset, GLint zoffset, GLenum target, GLint level, GLenum format, es2::Texture *texture)
{
	if(!texture)
	{
		return error(GL_INVALID_OPERATION, false);
	}

	if(compressed != texture->isCompressed(target, level))
	{
		return error(GL_INVALID_OPERATION, false);
	}

	if(format != GL_NONE && format != texture->getFormat(target, level))
	{
		return error(GL_INVALID_OPERATION, false);
	}

	if(compressed)
	{
		if((width % 4 != 0 && width != texture->getWidth(target, 0)) ||
			(height % 4 != 0 && height != texture->getHeight(target, 0)) ||
			(depth % 4 != 0 && depth != texture->getDepth(target, 0)))
		{
			return error(GL_INVALID_OPERATION, false);
		}
	}

	if(xoffset + width > texture->getWidth(target, level) ||
		yoffset + height > texture->getHeight(target, level) ||
		zoffset + depth > texture->getDepth(target, level))
	{
		return error(GL_INVALID_VALUE, false);
	}

	return true;
}

static bool validateColorBufferFormat(GLenum textureFormat, GLenum colorbufferFormat)
{
	switch(textureFormat)
	{
	case GL_ALPHA:
		if(colorbufferFormat != GL_ALPHA &&
			colorbufferFormat != GL_RGBA &&
			colorbufferFormat != GL_RGBA4 &&
			colorbufferFormat != GL_RGB5_A1 &&
			colorbufferFormat != GL_RGBA8)
		{
			return error(GL_INVALID_OPERATION, false);
		}
		break;
	case GL_LUMINANCE:
	case GL_RGB:
		if(colorbufferFormat != GL_RGB &&
			colorbufferFormat != GL_RGB565 &&
			colorbufferFormat != GL_RGB8 &&
			colorbufferFormat != GL_RGBA &&
			colorbufferFormat != GL_RGBA4 &&
			colorbufferFormat != GL_RGB5_A1 &&
			colorbufferFormat != GL_RGBA8)
		{
			return error(GL_INVALID_OPERATION, false);
		}
		break;
	case GL_LUMINANCE_ALPHA:
	case GL_RGBA:
		if(colorbufferFormat != GL_RGBA &&
			colorbufferFormat != GL_RGBA4 &&
			colorbufferFormat != GL_RGB5_A1 &&
			colorbufferFormat != GL_RGBA8)
		{
			return error(GL_INVALID_OPERATION, false);
		}
		break;
	case GL_ETC1_RGB8_OES:
		return error(GL_INVALID_OPERATION, false);
	case GL_COMPRESSED_RGB_S3TC_DXT1_EXT:
	case GL_COMPRESSED_RGBA_S3TC_DXT1_EXT:
	case GL_COMPRESSED_RGBA_S3TC_DXT3_ANGLE:
	case GL_COMPRESSED_RGBA_S3TC_DXT5_ANGLE:
		if(S3TC_SUPPORT)
		{
			return error(GL_INVALID_OPERATION, false);
		}
		else
		{
			return error(GL_INVALID_ENUM, false);
		}
	case GL_DEPTH_COMPONENT:
	case GL_DEPTH_STENCIL:
		return error(GL_INVALID_OPERATION, false);
	default:
		return error(GL_INVALID_ENUM, false);
	}
	return true;
}

static FormatMap BuildFormatMap3D()
{
	FormatMap map;

	//                       Internal format | Format | Type
	InsertFormatMapping(map, GL_RGB, GL_RGB, GL_UNSIGNED_BYTE);
	InsertFormatMapping(map, GL_RGB, GL_RGB, GL_UNSIGNED_SHORT_5_6_5);
	InsertFormatMapping(map, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE);
	InsertFormatMapping(map, GL_RGBA, GL_RGBA, GL_UNSIGNED_SHORT_4_4_4_4);
	InsertFormatMapping(map, GL_RGBA, GL_RGBA, GL_UNSIGNED_SHORT_5_5_5_1);
	InsertFormatMapping(map, GL_LUMINANCE_ALPHA, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE);
	InsertFormatMapping(map, GL_LUMINANCE, GL_LUMINANCE, GL_UNSIGNED_BYTE);
	InsertFormatMapping(map, GL_ALPHA, GL_ALPHA, GL_UNSIGNED_BYTE);
	InsertFormatMapping(map, GL_R8, GL_RED, GL_UNSIGNED_BYTE);
	InsertFormatMapping(map, GL_R8_SNORM, GL_RED, GL_UNSIGNED_BYTE);
	InsertFormatMapping(map, GL_R16F, GL_RED, GL_HALF_FLOAT);
	InsertFormatMapping(map, GL_R16F, GL_RED, GL_FLOAT);
	InsertFormatMapping(map, GL_R32F, GL_RED, GL_FLOAT);
	InsertFormatMapping(map, GL_R8UI, GL_RED_INTEGER, GL_UNSIGNED_BYTE);
	InsertFormatMapping(map, GL_R8I, GL_RED_INTEGER, GL_BYTE);
	InsertFormatMapping(map, GL_R16UI, GL_RED_INTEGER, GL_UNSIGNED_SHORT);
	InsertFormatMapping(map, GL_R16I, GL_RED_INTEGER, GL_SHORT);
	InsertFormatMapping(map, GL_R32UI, GL_RED_INTEGER, GL_UNSIGNED_INT);
	InsertFormatMapping(map, GL_R32I, GL_RED_INTEGER, GL_INT);
	InsertFormatMapping(map, GL_RG8, GL_RG, GL_UNSIGNED_BYTE);
	InsertFormatMapping(map, GL_RG8_SNORM, GL_RG, GL_BYTE);
	InsertFormatMapping(map, GL_R16F, GL_RED, GL_HALF_FLOAT);
	InsertFormatMapping(map, GL_R16F, GL_RED, GL_FLOAT);
	InsertFormatMapping(map, GL_RG32F, GL_RG, GL_FLOAT);
	InsertFormatMapping(map, GL_RG8UI, GL_RG_INTEGER, GL_UNSIGNED_BYTE);
	InsertFormatMapping(map, GL_RG8I, GL_RG_INTEGER, GL_BYTE);
	InsertFormatMapping(map, GL_RG16UI, GL_RG_INTEGER, GL_UNSIGNED_SHORT);
	InsertFormatMapping(map, GL_RG16I, GL_RG_INTEGER, GL_SHORT);
	InsertFormatMapping(map, GL_RG32UI, GL_RG_INTEGER, GL_UNSIGNED_INT);
	InsertFormatMapping(map, GL_RG32I, GL_RG_INTEGER, GL_INT);
	InsertFormatMapping(map, GL_RGB8, GL_RGB, GL_UNSIGNED_BYTE);
	InsertFormatMapping(map, GL_SRGB8, GL_RGB, GL_UNSIGNED_BYTE);
	InsertFormatMapping(map, GL_RGB565, GL_RGB, GL_UNSIGNED_BYTE);
	InsertFormatMapping(map, GL_RGB565, GL_RGB, GL_UNSIGNED_SHORT_5_6_5);
	InsertFormatMapping(map, GL_RGB8_SNORM, GL_RGB, GL_BYTE);
	InsertFormatMapping(map, GL_R11F_G11F_B10F, GL_RGB, GL_UNSIGNED_INT_10F_11F_11F_REV);
	InsertFormatMapping(map, GL_R11F_G11F_B10F, GL_RGB, GL_HALF_FLOAT);
	InsertFormatMapping(map, GL_R11F_G11F_B10F, GL_RGB, GL_FLOAT);
	InsertFormatMapping(map, GL_RGB9_E5, GL_RGB, GL_UNSIGNED_INT_5_9_9_9_REV);
	InsertFormatMapping(map, GL_RGB9_E5, GL_RGB, GL_HALF_FLOAT);
	InsertFormatMapping(map, GL_RGB9_E5, GL_RGB, GL_FLOAT);
	InsertFormatMapping(map, GL_RGB16F, GL_RGB, GL_HALF_FLOAT);
	InsertFormatMapping(map, GL_RGB16F, GL_RGB, GL_FLOAT);
	InsertFormatMapping(map, GL_RGB32F, GL_RGB, GL_FLOAT);
	InsertFormatMapping(map, GL_RGB8UI, GL_RGB_INTEGER, GL_UNSIGNED_BYTE);
	InsertFormatMapping(map, GL_RGB8I, GL_RGB_INTEGER, GL_BYTE);
	InsertFormatMapping(map, GL_RGB16UI, GL_RGB_INTEGER, GL_UNSIGNED_SHORT);
	InsertFormatMapping(map, GL_RGB16I, GL_RGB_INTEGER, GL_SHORT);
	InsertFormatMapping(map, GL_RGB32UI, GL_RGB_INTEGER, GL_UNSIGNED_INT);
	InsertFormatMapping(map, GL_RGB32I, GL_RGB_INTEGER, GL_INT);
	InsertFormatMapping(map, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE);
	InsertFormatMapping(map, GL_SRGB8_ALPHA8, GL_RGBA, GL_UNSIGNED_BYTE);
	InsertFormatMapping(map, GL_RGB5_A1, GL_RGBA, GL_UNSIGNED_BYTE);
	InsertFormatMapping(map, GL_RGB5_A1, GL_RGBA, GL_UNSIGNED_SHORT_5_5_5_1);
	InsertFormatMapping(map, GL_RGB5_A1, GL_RGBA, GL_UNSIGNED_INT_2_10_10_10_REV);
	InsertFormatMapping(map, GL_RGBA4, GL_RGBA, GL_UNSIGNED_BYTE);
	InsertFormatMapping(map, GL_RGBA4, GL_RGBA, GL_UNSIGNED_SHORT_4_4_4_4);
	InsertFormatMapping(map, GL_RGB10_A2, GL_RGBA, GL_UNSIGNED_INT_2_10_10_10_REV);
	InsertFormatMapping(map, GL_RGBA16F, GL_RGBA, GL_HALF_FLOAT);
	InsertFormatMapping(map, GL_RGBA16F, GL_RGBA, GL_FLOAT);
	InsertFormatMapping(map, GL_RGBA32F, GL_RGBA, GL_FLOAT);
	InsertFormatMapping(map, GL_RGBA8UI, GL_RGBA_INTEGER, GL_UNSIGNED_BYTE);
	InsertFormatMapping(map, GL_RGBA8I, GL_RGBA_INTEGER, GL_BYTE);
	InsertFormatMapping(map, GL_RGB10_A2UI, GL_RGBA_INTEGER, GL_UNSIGNED_INT_2_10_10_10_REV);
	InsertFormatMapping(map, GL_RGBA16UI, GL_RGBA_INTEGER, GL_UNSIGNED_SHORT);
	InsertFormatMapping(map, GL_RGBA16I, GL_RGBA_INTEGER, GL_SHORT);
	InsertFormatMapping(map, GL_RGBA32UI, GL_RGBA_INTEGER, GL_UNSIGNED_INT);
	InsertFormatMapping(map, GL_RGBA32I, GL_RGBA_INTEGER, GL_INT);

	InsertFormatMapping(map, GL_DEPTH_COMPONENT16, GL_DEPTH_COMPONENT, GL_UNSIGNED_SHORT);
	InsertFormatMapping(map, GL_DEPTH_COMPONENT16, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT);
	InsertFormatMapping(map, GL_DEPTH_COMPONENT24, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT);
	InsertFormatMapping(map, GL_DEPTH_COMPONENT32F, GL_DEPTH_COMPONENT, GL_FLOAT);
	InsertFormatMapping(map, GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8);
	InsertFormatMapping(map, GL_DEPTH32F_STENCIL8, GL_DEPTH_STENCIL, GL_FLOAT_32_UNSIGNED_INT_24_8_REV);

	return map;
}

static bool ValidateType3D(GLenum type)
{
	switch(type)
	{
	case GL_UNSIGNED_BYTE:
	case GL_BYTE:
	case GL_UNSIGNED_SHORT:
	case GL_SHORT:
	case GL_UNSIGNED_INT:
	case GL_INT:
	case GL_HALF_FLOAT:
	case GL_FLOAT:
	case GL_UNSIGNED_SHORT_5_6_5:
	case GL_UNSIGNED_SHORT_4_4_4_4:
	case GL_UNSIGNED_SHORT_5_5_5_1:
	case GL_UNSIGNED_INT_2_10_10_10_REV:
	case GL_UNSIGNED_INT_10F_11F_11F_REV:
	case GL_UNSIGNED_INT_5_9_9_9_REV:
	case GL_UNSIGNED_INT_24_8:
	case GL_FLOAT_32_UNSIGNED_INT_24_8_REV:
		return true;
	default:
		break;
	}
	return false;
}

static bool ValidateFormat3D(GLenum format)
{
	switch(format)
	{
	case GL_RED:
	case GL_RG:
	case GL_RGB:
	case GL_RGBA:
	case GL_DEPTH_COMPONENT:
	case GL_DEPTH_STENCIL:
	case GL_LUMINANCE_ALPHA:
	case GL_LUMINANCE:
	case GL_ALPHA:
	case GL_RED_INTEGER:
	case GL_RG_INTEGER:
	case GL_RGB_INTEGER:
	case GL_RGBA_INTEGER:
		return true;
	default:
		break;
	}
	return false;
}

static bool ValidateInternalFormat3D(GLenum internalformat, GLenum format, GLenum type)
{
	static const FormatMap formatMap = BuildFormatMap3D();
	FormatMap::const_iterator iter = formatMap.find(InternalFormatTypePair(internalformat, type));
	if(iter != formatMap.end())
	{
		return iter->second == format;
	}
	return false;
}

static bool ValidateQueryTarget(GLenum target)
{
	switch(target)
	{
	case GL_ANY_SAMPLES_PASSED:
	case GL_ANY_SAMPLES_PASSED_CONSERVATIVE:
	case GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN:
		break;
	default:
		return false;
	}

	return true;
}

static bool ValidateBufferTarget(GLenum target)
{
	switch(target)
	{
	case GL_ARRAY_BUFFER:
	case GL_COPY_READ_BUFFER:
	case GL_COPY_WRITE_BUFFER:
	case GL_ELEMENT_ARRAY_BUFFER:
	case GL_PIXEL_PACK_BUFFER:
	case GL_PIXEL_UNPACK_BUFFER:
	case GL_TRANSFORM_FEEDBACK_BUFFER:
	case GL_UNIFORM_BUFFER:
		break;
	default:
		return false;
	}

	return true;
}

extern "C"
{

void GL_APIENTRY glReadBuffer(GLenum src)
{
	TRACE("(GLenum src = 0x%X)", src);

	es2::Context *context = es2::getContext();

	switch(src)
	{
	case GL_BACK:
	case GL_NONE:
		break;
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
		if((src - GL_COLOR_ATTACHMENT0) >= es2::IMPLEMENTATION_MAX_COLOR_ATTACHMENTS)
		{
			return error(GL_INVALID_ENUM);
		}
		break;
	default:
		error(GL_INVALID_ENUM);
	}

	UNIMPLEMENTED();
}

void GL_APIENTRY glDrawRangeElements(GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const void *indices)
{
	TRACE("(GLenum mode = 0x%X, GLuint start = %d, GLuint end = %d, "
		  "GLsizei count = %d, GLenum type = 0x%x, const void* indices = 0x%0.8p)",
		  mode, start, end, count, type, indices);

	switch(mode)
	{
	case GL_POINTS:
	case GL_LINES:
	case GL_LINE_LOOP:
	case GL_LINE_STRIP:
	case GL_TRIANGLES:
	case GL_TRIANGLE_FAN:
	case GL_TRIANGLE_STRIP:
		break;
	default:
		return error(GL_INVALID_ENUM);
	}

	switch(type)
	{
	case GL_UNSIGNED_BYTE:
	case GL_UNSIGNED_SHORT:
	case GL_UNSIGNED_INT:
		break;
	default:
		return error(GL_INVALID_ENUM);
	}

	if((count < 0) || (end < start))
	{
		return error(GL_INVALID_VALUE);
	}

	UNIMPLEMENTED();
}

void GL_APIENTRY glTexImage3D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, const void *pixels)
{
	TRACE("(GLenum target = 0x%X, GLint level = %d, GLenum internalformat = 0x%X, "
	      "GLsizei width = %d, GLsizei height = %d, GLsizei depth = %d, GLint border = %d, "
	      "GLenum format = 0x%X, GLenum type = 0x%x, const GLvoid* pixels = 0x%0.8p)",
	      target, level, internalformat, width, height, depth, border, format, type, pixels);

	switch(target)
	{
	case GL_TEXTURE_3D:
	case GL_TEXTURE_2D_ARRAY:
		break;
	default:
		return error(GL_INVALID_ENUM);
	}

	if(!ValidateType3D(type) || !ValidateFormat3D(format))
	{
		return error(GL_INVALID_ENUM);
	}

	if((level < 0) || (level >= es2::IMPLEMENTATION_MAX_TEXTURE_LEVELS))
	{
		return error(GL_INVALID_VALUE);
	}

	const GLsizei maxSize3D = es2::IMPLEMENTATION_MAX_TEXTURE_SIZE >> level;
	if((width < 0) || (height < 0) || (depth < 0) || (width > maxSize3D) || (height > maxSize3D) || (depth > maxSize3D))
	{
		return error(GL_INVALID_VALUE);
	}

	if(border != 0)
	{
		return error(GL_INVALID_VALUE);
	}

	if(!ValidateInternalFormat3D(internalformat, format, type))
	{
		return error(GL_INVALID_OPERATION);
	}

	es2::Context *context = es2::getContext();

	if(context)
	{
		es2::Texture3D *texture = context->getTexture3D();

		if(!texture)
		{
			return error(GL_INVALID_OPERATION);
		}

		texture->setImage(level, width, height, depth, internalformat, type, context->getUnpackAlignment(), pixels);
	}
}

void GL_APIENTRY glTexSubImage3D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const void *pixels)
{
	TRACE("(GLenum target = 0x%X, GLint level = %d, GLint xoffset = %d, GLint yoffset = %d, "
		"GLint zoffset = %d, GLsizei width = %d, GLsizei height = %d, GLsizei depth = %d, "
		"GLenum format = 0x%X, GLenum type = 0x%x, const GLvoid* pixels = 0x%0.8p)",
		target, level, xoffset, yoffset, zoffset, width, height, depth, format, type, pixels);

	switch(target)
	{
	case GL_TEXTURE_3D:
	case GL_TEXTURE_2D_ARRAY:
		break;
	default:
		return error(GL_INVALID_ENUM);
	}

	if(!ValidateType3D(type) || !ValidateFormat3D(format))
	{
		return error(GL_INVALID_ENUM);
	}

	if((level < 0) || (level >= es2::IMPLEMENTATION_MAX_TEXTURE_LEVELS))
	{
		return error(GL_INVALID_VALUE);
	}

	if((width < 0) || (height < 0) || (depth < 0))
	{
		return error(GL_INVALID_VALUE);
	}

	es2::Context *context = es2::getContext();

	if(context)
	{
		es2::Texture3D *texture = context->getTexture3D();

		if(validateSubImageParams(false, width, height, depth, xoffset, yoffset, zoffset, target, level, format, texture))
		{
			texture->subImage(level, xoffset, yoffset, zoffset, width, height, depth, format, type, context->getUnpackAlignment(), pixels);
		}
	}
}

void GL_APIENTRY glCopyTexSubImage3D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLint x, GLint y, GLsizei width, GLsizei height)
{
	TRACE("(GLenum target = 0x%X, GLint level = %d, GLint xoffset = %d, GLint yoffset = %d, "
		"GLint zoffset = %d, GLint x = %d, GLint y = %d, GLsizei width = %d, GLsizei height = %d)",
		target, level, xoffset, yoffset, zoffset, x, y, width, height);

	switch(target)
	{
	case GL_TEXTURE_3D:
	case GL_TEXTURE_2D_ARRAY:
		break;
	default:
		return error(GL_INVALID_ENUM);
	}

	if((level < 0) || (level >= es2::IMPLEMENTATION_MAX_TEXTURE_LEVELS))
	{
		return error(GL_INVALID_VALUE);
	}

	es2::Context *context = es2::getContext();

	if(context)
	{
		es2::Framebuffer *framebuffer = context->getReadFramebuffer();

		if(framebuffer->completeness() != GL_FRAMEBUFFER_COMPLETE)
		{
			return error(GL_INVALID_FRAMEBUFFER_OPERATION);
		}

		if(context->getReadFramebufferName() != 0 && framebuffer->getColorbuffer()->getSamples() > 1)
		{
			return error(GL_INVALID_OPERATION);
		}

		es2::Renderbuffer *source = framebuffer->getColorbuffer();
		GLenum colorbufferFormat = source->getFormat();
		es2::Texture3D *texture = context->getTexture3D();

		if(!validateSubImageParams(false, width, height, 1, xoffset, yoffset, zoffset, target, level, GL_NONE, texture))
		{
			return;
		}

		GLenum textureFormat = texture->getFormat(target, level);

		if(!validateColorBufferFormat(textureFormat, colorbufferFormat))
		{
			return;
		}

		texture->copySubImage(target, level, xoffset, yoffset, zoffset, x, y, width, height, framebuffer);
	}
}

void GL_APIENTRY glCompressedTexImage3D(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLsizei imageSize, const void *data)
{
	TRACE("(GLenum target = 0x%X, GLint level = %d, GLenum internalformat = 0x%X, GLsizei width = %d, "
		"GLsizei height = %d, GLsizei depth = %d, GLint border = %d, GLsizei imageSize = %d, const GLvoid* data = 0x%0.8p)",
		target, level, internalformat, width, height, depth, border, imageSize, data);

	switch(target)
	{
	case GL_TEXTURE_3D:
	case GL_TEXTURE_2D_ARRAY:
		break;
	default:
		return error(GL_INVALID_ENUM);
	}

	if((level < 0) || (level >= es2::IMPLEMENTATION_MAX_TEXTURE_LEVELS))
	{
		return error(GL_INVALID_VALUE);
	}

	const GLsizei maxSize3D = es2::IMPLEMENTATION_MAX_TEXTURE_SIZE >> level;
	if((width < 0) || (height < 0) || (depth < 0) || (width > maxSize3D) || (height > maxSize3D) || (depth > maxSize3D) || (border != 0) || (imageSize < 0))
	{
		return error(GL_INVALID_VALUE);
	}

	switch(internalformat)
	{
	case GL_ETC1_RGB8_OES:
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
		break;
	case GL_COMPRESSED_RGB_S3TC_DXT1_EXT:
	case GL_COMPRESSED_RGBA_S3TC_DXT1_EXT:
	case GL_COMPRESSED_RGBA_S3TC_DXT3_ANGLE:
	case GL_COMPRESSED_RGBA_S3TC_DXT5_ANGLE:
		if(!S3TC_SUPPORT)
		{
			return error(GL_INVALID_ENUM);
		}
		break;
	case GL_DEPTH_COMPONENT:
	case GL_DEPTH_COMPONENT16:
	case GL_DEPTH_COMPONENT32_OES:
	case GL_DEPTH_STENCIL:
	case GL_DEPTH24_STENCIL8:
		return error(GL_INVALID_OPERATION);
	default:
		return error(GL_INVALID_ENUM);
	}

	if(imageSize != es2::ComputeCompressedSize(width, height, internalformat) * depth)
	{
		return error(GL_INVALID_VALUE);
	}

	es2::Context *context = es2::getContext();

	if(context)
	{
		es2::Texture3D *texture = context->getTexture3D();

		if(!texture)
		{
			return error(GL_INVALID_OPERATION);
		}

		texture->setCompressedImage(level, internalformat, width, height, depth, imageSize, data);
	}
}

void GL_APIENTRY glCompressedTexSubImage3D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, const void *data)
{
	TRACE("(GLenum target = 0x%X, GLint level = %d, GLint xoffset = %d, GLint yoffset = %d, "
	      "GLint zoffset = %d, GLsizei width = %d, GLsizei height = %d, GLsizei depth = %d, "
	      "GLenum format = 0x%X, GLsizei imageSize = %d, const void *data = 0x%0.8p)",
	      target, level, xoffset, yoffset, zoffset, width, height, depth, format, imageSize, data);

	switch(target)
	{
	case GL_TEXTURE_3D:
	case GL_TEXTURE_2D_ARRAY:
		break;
	default:
		return error(GL_INVALID_ENUM);
	}

	if(xoffset < 0 || yoffset < 0 || zoffset < 0 || !validImageSize(level, width, height) || depth < 0 || imageSize < 0)
	{
		return error(GL_INVALID_VALUE);
	}

	switch(format)
	{
	case GL_ETC1_RGB8_OES:
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
		break;
	case GL_COMPRESSED_RGB_S3TC_DXT1_EXT:
	case GL_COMPRESSED_RGBA_S3TC_DXT1_EXT:
	case GL_COMPRESSED_RGBA_S3TC_DXT3_ANGLE:
	case GL_COMPRESSED_RGBA_S3TC_DXT5_ANGLE:
		if(!S3TC_SUPPORT)
		{
			return error(GL_INVALID_ENUM);
		}
		break;
	default:
		return error(GL_INVALID_ENUM);
	}

	if(width == 0 || height == 0 || depth == 0 || data == NULL)
	{
		return;
	}

	es2::Context *context = es2::getContext();

	if(context)
	{
		es2::Texture3D *texture = context->getTexture3D();

		if(!texture)
		{
			return error(GL_INVALID_OPERATION);
		}

		texture->subImageCompressed(level, xoffset, yoffset, zoffset, width, height, depth, format, imageSize, data);
	}
}

void GL_APIENTRY glGenQueries(GLsizei n, GLuint *ids)
{
	TRACE("(GLsizei n = %d, GLuint* ids = 0x%0.8p)", n, ids);

	if(n < 0)
	{
		return error(GL_INVALID_VALUE);
	}

	UNIMPLEMENTED();
}

void GL_APIENTRY glDeleteQueries(GLsizei n, const GLuint *ids)
{
	TRACE("(GLsizei n = %d, GLuint* ids = 0x%0.8p)", n, ids);

	if(n < 0)
	{
		return error(GL_INVALID_VALUE);
	}

	UNIMPLEMENTED();
}

GLboolean GL_APIENTRY glIsQuery(GLuint id)
{
	TRACE("(GLuint id = %d)", id);

	UNIMPLEMENTED();
	return GL_FALSE;
}

void GL_APIENTRY glBeginQuery(GLenum target, GLuint id)
{
	TRACE("(GLenum target = 0x%X, GLuint id = %d)", target, id);

	if(!ValidateQueryTarget(target))
	{
		return error(GL_INVALID_ENUM);
	}

	UNIMPLEMENTED();
}

void GL_APIENTRY glEndQuery(GLenum target)
{
	TRACE("(GLenum target = 0x%X)", target);

	if(!ValidateQueryTarget(target))
	{
		return error(GL_INVALID_ENUM);
	}

	UNIMPLEMENTED();
}

void GL_APIENTRY glGetQueryiv(GLenum target, GLenum pname, GLint *params)
{
	TRACE("(GLenum target = 0x%X, GLenum pname = 0x%X, GLint *params = 0x%0.8p)",
		  target, pname, params);

	if(!ValidateQueryTarget(target) || (pname != GL_CURRENT_QUERY))
	{
		return error(GL_INVALID_ENUM);
	}

	UNIMPLEMENTED();
}

void GL_APIENTRY glGetQueryObjectuiv(GLuint id, GLenum pname, GLuint *params)
{
	TRACE("(GLuint id = %d, GLenum pname = 0x%X, GLint *params = 0x%0.8p)",
	      id, pname, params);

	switch(pname)
	{
	case GL_QUERY_RESULT:
	case GL_QUERY_RESULT_AVAILABLE:
		break;
	default:
		return error(GL_INVALID_ENUM);
	}

	UNIMPLEMENTED();
}

GLboolean GL_APIENTRY glUnmapBuffer(GLenum target)
{
	TRACE("(GLenum target = 0x%X)", target);

	UNIMPLEMENTED();
	return GL_FALSE;
}

void GL_APIENTRY glGetBufferPointerv(GLenum target, GLenum pname, void **params)
{
	TRACE("(GLenum target = 0x%X, GLenum pname = 0x%X, GLint *params = 0x%0.8p)",
	      target, pname, params);

	if(!ValidateBufferTarget(target))
	{
		return error(GL_INVALID_ENUM);
	}

	if(pname != GL_BUFFER_MAP_POINTER)
	{
		return error(GL_INVALID_ENUM);
	}

	UNIMPLEMENTED();
}

void GL_APIENTRY glDrawBuffers(GLsizei n, const GLenum *bufs)
{
	TRACE("(GLsizei n = %d, const GLenum *bufs = 0x%0.8p)", n, bufs);

	UNIMPLEMENTED();
}

void GL_APIENTRY glUniformMatrix2x3fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value)
{
	TRACE("(GLint location = %d, GLsizei count = %d, GLboolean transpose = %d, const GLfloat *value = 0x%0.8p)", location, count, transpose, value);
	UNIMPLEMENTED();
}

void GL_APIENTRY glUniformMatrix3x2fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value)
{
	TRACE("(GLint location = %d, GLsizei count = %d, GLboolean transpose = %d, const GLfloat *value = 0x%0.8p)", location, count, transpose, value);
	UNIMPLEMENTED();
}

void GL_APIENTRY glUniformMatrix2x4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value)
{
	TRACE("(GLint location = %d, GLsizei count = %d, GLboolean transpose = %d, const GLfloat *value = 0x%0.8p)", location, count, transpose, value);
	UNIMPLEMENTED();
}

void GL_APIENTRY glUniformMatrix4x2fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value)
{
	TRACE("(GLint location = %d, GLsizei count = %d, GLboolean transpose = %d, const GLfloat *value = 0x%0.8p)", location, count, transpose, value);
	UNIMPLEMENTED();
}

void GL_APIENTRY glUniformMatrix3x4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value)
{
	TRACE("(GLint location = %d, GLsizei count = %d, GLboolean transpose = %d, const GLfloat *value = 0x%0.8p)", location, count, transpose, value);
	UNIMPLEMENTED();
}

void GL_APIENTRY glUniformMatrix4x3fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value)
{
	TRACE("(GLint location = %d, GLsizei count = %d, GLboolean transpose = %d, const GLfloat *value = 0x%0.8p)", location, count, transpose, value);
	UNIMPLEMENTED();
}

void GL_APIENTRY glBlitFramebuffer(GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter)
{
	TRACE("(GLint srcX0 = %d, GLint srcY0 = %d, GLint srcX1 = %d, GLint srcY1 = %d, "
	      "GLint dstX0 = %d, GLint dstY0 = %d, GLint dstX1 = %d, GLint dstY1 = %d, "
	      "GLbitfield mask = 0x%X, GLenum filter = 0x%X)",
	      srcX0, srcY0, srcX1, srcX1, dstX0, dstY0, dstX1, dstY1, mask, filter);

	switch(filter)
	{
	case GL_NEAREST:
	case GL_LINEAR:
		break;
	default:
		return error(GL_INVALID_ENUM);
	}

	if((mask & ~(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)) != 0)
	{
		return error(GL_INVALID_VALUE);
	}

	es2::Context *context = es2::getContext();

	if(context)
	{
		if(context->getReadFramebufferName() == context->getDrawFramebufferName())
		{
			ERR("Blits with the same source and destination framebuffer are not supported by this implementation.");
			return error(GL_INVALID_OPERATION);
		}

		context->blitFramebuffer(srcX0, srcY0, srcX1, srcY1, dstX0, dstY0, dstX1, dstY1, mask);
	}
}

void GL_APIENTRY glRenderbufferStorageMultisample(GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height)
{
	TRACE("(GLenum target = 0x%X, GLsizei samples = %d, GLenum internalformat = 0x%X, GLsizei width = %d, GLsizei height = %d)",
	      target, samples, internalformat, width, height);

	switch(target)
	{
	case GL_RENDERBUFFER:
		break;
	default:
		return error(GL_INVALID_ENUM);
	}

	if(!es2::IsColorRenderable(internalformat) && !es2::IsDepthRenderable(internalformat) && !es2::IsStencilRenderable(internalformat))
	{
		return error(GL_INVALID_ENUM);
	}

	if(width < 0 || height < 0 || samples < 0)
	{
		return error(GL_INVALID_VALUE);
	}

	es2::Context *context = es2::getContext();

	if(context)
	{
		if(width > es2::IMPLEMENTATION_MAX_RENDERBUFFER_SIZE ||
			height > es2::IMPLEMENTATION_MAX_RENDERBUFFER_SIZE ||
			samples > es2::IMPLEMENTATION_MAX_SAMPLES)
		{
			return error(GL_INVALID_VALUE);
		}

		GLuint handle = context->getRenderbufferName();
		if(handle == 0)
		{
			return error(GL_INVALID_OPERATION);
		}

		switch(internalformat)
		{
		case GL_DEPTH_COMPONENT16:
		case GL_DEPTH_COMPONENT24:
		case GL_DEPTH_COMPONENT32F:
			context->setRenderbufferStorage(new es2::Depthbuffer(width, height, samples));
			break;
		case GL_R8UI:
		case GL_R8I:
		case GL_R16UI:
		case GL_R16I:
		case GL_R32UI:
		case GL_R32I:
		case GL_RG8UI:
		case GL_RG8I:
		case GL_RG16UI:
		case GL_RG16I:
		case GL_RG32UI:
		case GL_RG32I:
		case GL_RGB8UI:
		case GL_RGB8I:
		case GL_RGB16UI:
		case GL_RGB16I:
		case GL_RGB32UI:
		case GL_RGB32I:
		case GL_RGBA8UI:
		case GL_RGBA8I:
		case GL_RGB10_A2UI:
		case GL_RGBA16UI:
		case GL_RGBA16I:
		case GL_RGBA32UI:
		case GL_RGBA32I:
			if(samples > 0)
			{
				return error(GL_INVALID_OPERATION);
			}
		case GL_RGBA4:
		case GL_RGB5_A1:
		case GL_RGB565:
		case GL_SRGB8_ALPHA8:
		case GL_RGB10_A2:
		case GL_R8:
		case GL_RG8:
		case GL_RGB8:
		case GL_RGBA8:
			context->setRenderbufferStorage(new es2::Colorbuffer(width, height, internalformat, samples));
			break;
		case GL_STENCIL_INDEX8:
			context->setRenderbufferStorage(new es2::Stencilbuffer(width, height, samples));
			break;
		case GL_DEPTH24_STENCIL8:
		case GL_DEPTH32F_STENCIL8:
			context->setRenderbufferStorage(new es2::DepthStencilbuffer(width, height, samples));
			break;

		default:
			return error(GL_INVALID_ENUM);
		}
	}
}

void GL_APIENTRY glFramebufferTextureLayer(GLenum target, GLenum attachment, GLuint texture, GLint level, GLint layer)
{
	TRACE("(GLenum target = 0x%X, GLenum attachment = 0x%X, GLuint texture = %d, GLint level = %d, GLint layer = %d)",
	      target, attachment, texture, level, layer);
	
	switch(target)
	{
	case GL_DRAW_FRAMEBUFFER:
	case GL_READ_FRAMEBUFFER:
	case GL_FRAMEBUFFER:
		break;
	default:
		return error(GL_INVALID_ENUM);
	}

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
	case GL_DEPTH_ATTACHMENT:
	case GL_STENCIL_ATTACHMENT:
	case GL_DEPTH_STENCIL_ATTACHMENT:
		break;
	default:
		return error(GL_INVALID_ENUM);
	}

	UNIMPLEMENTED();
}

void *GL_APIENTRY glMapBufferRange(GLenum target, GLintptr offset, GLsizeiptr length, GLbitfield access)
{
	TRACE("(GLenum target = 0x%X,  GLintptr offset = %d, GLsizeiptr length = %d, GLbitfield access = %X)",
	      target, offset, length, access);

	GLint bufferSize;
	glGetBufferParameteriv(target, GL_BUFFER_SIZE, &bufferSize);
	if((offset < 0) || (length<0) || ((offset + length) > bufferSize))
	{
		error(GL_INVALID_VALUE);
	}

	if((access & ~(GL_MAP_READ_BIT |
				   GL_MAP_WRITE_BIT |
				   GL_MAP_INVALIDATE_RANGE_BIT |
				   GL_MAP_INVALIDATE_BUFFER_BIT |
				   GL_MAP_FLUSH_EXPLICIT_BIT |
				   GL_MAP_UNSYNCHRONIZED_BIT)) != 0)
	{
		error(GL_INVALID_VALUE);
	}

	UNIMPLEMENTED();
	return nullptr;
}

void GL_APIENTRY glFlushMappedBufferRange(GLenum target, GLintptr offset, GLsizeiptr length)
{
	TRACE("(GLenum target = 0x%X,  GLintptr offset = %d, GLsizeiptr length = %d)",
	      target, offset, length);

	if(!ValidateBufferTarget(target))
	{
		return error(GL_INVALID_ENUM);
	}

	if((offset < 0) || (length<0)) // FIXME: also check if offset + length exceeds the size of the mapping
	{
		error(GL_INVALID_VALUE);
	}

	UNIMPLEMENTED();
}

void GL_APIENTRY glBindVertexArray(GLuint array)
{
	TRACE("(GLuint array = %d)", array);

	UNIMPLEMENTED();
}

void GL_APIENTRY glDeleteVertexArrays(GLsizei n, const GLuint *arrays)
{
	TRACE("(GLsizei n = %d, const GLuint *arrays = 0x%0.8p)", n, arrays);

	if(n < 0)
	{
		return error(GL_INVALID_VALUE);
	}

	UNIMPLEMENTED();
}

void GL_APIENTRY glGenVertexArrays(GLsizei n, GLuint *arrays)
{
	TRACE("(GLsizei n = %d, const GLuint *arrays = 0x%0.8p)", n, arrays);

	if(n < 0)
	{
		return error(GL_INVALID_VALUE);
	}

	UNIMPLEMENTED();
}

GLboolean GL_APIENTRY glIsVertexArray(GLuint array)
{
	TRACE("(GLuint array = %d)", array);

	UNIMPLEMENTED();
	return GL_FALSE;
}

void GL_APIENTRY glGetIntegeri_v(GLenum target, GLuint index, GLint *data)
{
	TRACE("(GLenum target = 0x%X, GLuint index = %d, GLint* data = 0x%0.8p)",
	      target, index, data);

	es2::Context *context = es2::getContext();

	if(context)
	{
		if(!context->getTransformFeedbackiv(index, target, data) &&
		   !context->getIntegerv(target, data))
		{
			GLenum nativeType;
			unsigned int numParams = 0;
			if(!context->getQueryParameterInfo(target, &nativeType, &numParams))
				return error(GL_INVALID_ENUM);

			if(numParams == 0)
				return; // it is known that target is valid, but there are no parameters to return

			if(nativeType == GL_BOOL)
			{
				GLboolean *boolParams = NULL;
				boolParams = new GLboolean[numParams];

				context->getBooleanv(target, boolParams);

				for(unsigned int i = 0; i < numParams; ++i)
				{
					data[i] = (boolParams[i] == GL_FALSE) ? 0 : 1;
				}

				delete[] boolParams;
			}
			else if(nativeType == GL_FLOAT)
			{
				GLfloat *floatParams = NULL;
				floatParams = new GLfloat[numParams];

				context->getFloatv(target, floatParams);

				for(unsigned int i = 0; i < numParams; ++i)
				{
					if(target == GL_DEPTH_RANGE || target == GL_COLOR_CLEAR_VALUE || target == GL_DEPTH_CLEAR_VALUE || target == GL_BLEND_COLOR)
					{
						data[i] = (GLint)(((GLfloat)(0xFFFFFFFF) * floatParams[i] - 1.0f) * 0.5f);
					}
					else
					{
						data[i] = (GLint)(floatParams[i] > 0.0f ? floor(floatParams[i] + 0.5) : ceil(floatParams[i] - 0.5));
					}
				}

				delete[] floatParams;
			}
		}
	}
}

void GL_APIENTRY glBeginTransformFeedback(GLenum primitiveMode)
{
	TRACE("(GLenum primitiveMode = 0x%X)", primitiveMode);

	switch(primitiveMode)
	{
	case GL_POINTS:
	case GL_LINES:
	case GL_TRIANGLES:
		break;
	default:
		return error(GL_INVALID_ENUM);
	}

	UNIMPLEMENTED();
}

void GL_APIENTRY glEndTransformFeedback(void)
{
	UNIMPLEMENTED();
}

void GL_APIENTRY glBindBufferRange(GLenum target, GLuint index, GLuint buffer, GLintptr offset, GLsizeiptr size)
{
	TRACE("(GLenum target = 0x%X, GLuint index = %d, GLuint buffer = %d, GLintptr offset = %d, GLsizeiptr size = %d)",
	      target, index, buffer, offset, size);

	switch(target)
	{
	case GL_TRANSFORM_FEEDBACK_BUFFER:
	case GL_UNIFORM_BUFFER:
		break;
	default:
		return error(GL_INVALID_ENUM);
	}

	UNIMPLEMENTED();
}

void GL_APIENTRY glBindBufferBase(GLenum target, GLuint index, GLuint buffer)
{
	TRACE("(GLenum target = 0x%X, GLuint index = %d, GLuint buffer = %d)",
	      target, index, buffer);

	switch(target)
	{
	case GL_TRANSFORM_FEEDBACK_BUFFER:
	case GL_UNIFORM_BUFFER:
		break;
	default:
		return error(GL_INVALID_ENUM);
	}

	UNIMPLEMENTED();
}

void GL_APIENTRY glTransformFeedbackVaryings(GLuint program, GLsizei count, const GLchar *const*varyings, GLenum bufferMode)
{
	TRACE("(GLuint program = %d, GLsizei count = %d, const GLchar *const*varyings = 0x%0.8p, GLenum bufferMode = 0x%X)",
	      program, count, varyings, bufferMode);

	UNIMPLEMENTED();
}

void GL_APIENTRY glGetTransformFeedbackVarying(GLuint program, GLuint index, GLsizei bufSize, GLsizei *length, GLsizei *size, GLenum *type, GLchar *name)
{
	TRACE("(GLuint program = %d, GLuint index = %d, GLsizei bufSize = %d, GLsizei *length = 0x%0.8p, GLsizei *size = 0x%0.8p, GLenum *type = 0x%0.8p, GLchar *name = 0x%0.8p)",
	      program, index, bufSize, length, size, type, name);

	es2::Context *context = es2::getContext();

	if(context)
	{
		es2::Program *programObject = context->getProgram(program);

		if(!programObject)
		{
			return error(GL_INVALID_VALUE);
		}
	}

	UNIMPLEMENTED();
}

void GL_APIENTRY glVertexAttribIPointer(GLuint index, GLint size, GLenum type, GLsizei stride, const void *pointer)
{
	TRACE("(GLuint program = %d, GLuint index = %d, GLsizei bufSize = %d, GLsizei *length = 0x%0.8p, GLsizei *size = 0x%0.8p, GLenum *type = 0x%0.8p, GLchar *name = 0x%0.8p)",
	      index, size, type, stride, pointer);
	UNIMPLEMENTED();
}

void GL_APIENTRY glGetVertexAttribIiv(GLuint index, GLenum pname, GLint *params)
{
	TRACE("(GLuint index = %d, GLenum pname = 0x%X, GLint *params = 0x%0.8p)",
	      index, pname, params);
	UNIMPLEMENTED();
}

void GL_APIENTRY glGetVertexAttribIuiv(GLuint index, GLenum pname, GLuint *params)
{
	TRACE("(GLuint index = %d, GLenum pname = 0x%X, GLuint *params = 0x%0.8p)",
	      index, pname, params);
	UNIMPLEMENTED();
}

void GL_APIENTRY glVertexAttribI4i(GLuint index, GLint x, GLint y, GLint z, GLint w)
{
	TRACE("(GLuint index = %d, GLint x = %d, GLint y = %d, GLint z = %d, GLint w = %d)",
	      index, x, y, z, w);
	UNIMPLEMENTED();
}

void GL_APIENTRY glVertexAttribI4ui(GLuint index, GLuint x, GLuint y, GLuint z, GLuint w)
{
	TRACE("(GLuint index = %d, GLint x = %d, GLint y = %d, GLint z = %d, GLint w = %d)",
	      index, x, y, z, w);
	UNIMPLEMENTED();
}

void GL_APIENTRY glVertexAttribI4iv(GLuint index, const GLint *v)
{
	TRACE("(GLuint index = %d, GLint *v = 0x%0.8p)", index, v);
	UNIMPLEMENTED();
}

void GL_APIENTRY glVertexAttribI4uiv(GLuint index, const GLuint *v)
{
	TRACE("(GLuint index = %d, GLint *v = 0x%0.8p)", index, v);
	UNIMPLEMENTED();
}

void GL_APIENTRY glGetUniformuiv(GLuint program, GLint location, GLuint *params)
{
	TRACE("(GLuint program = %d, GLint location = %d, GLuint *params = 0x%0.8p)",
	      program, location, params);
	UNIMPLEMENTED();
}

GLint GL_APIENTRY glGetFragDataLocation(GLuint program, const GLchar *name)
{
	TRACE("(GLuint program = %d, const GLchar *name = 0x%0.8p)", program, name);

	es2::Context *context = es2::getContext();

	if(strstr(name, "gl_") == name)
	{
		return -1;
	}

	if(context)
	{
		es2::Program *programObject = context->getProgram(program);

		if(!programObject)
		{
			if(context->getShader(program))
			{
				return error(GL_INVALID_OPERATION, -1);
			}
			else
			{
				return error(GL_INVALID_VALUE, -1);
			}
		}

		if(!programObject->isLinked())
		{
			return error(GL_INVALID_OPERATION, -1);
		}
	}

	UNIMPLEMENTED();
	return -1;
}

void GL_APIENTRY glUniform1ui(GLint location, GLuint v0)
{
	glUniform1uiv(location, 1, &v0);
}

void GL_APIENTRY glUniform2ui(GLint location, GLuint v0, GLuint v1)
{
	GLuint xy[2] = { v0, v1 };

	glUniform2uiv(location, 1, (GLuint*)&xy);
}

void GL_APIENTRY glUniform3ui(GLint location, GLuint v0, GLuint v1, GLuint v2)
{
	GLuint xyz[3] = { v0, v1, v2 };

	glUniform3uiv(location, 1, (GLuint*)&xyz);
}

void GL_APIENTRY glUniform4ui(GLint location, GLuint v0, GLuint v1, GLuint v2, GLuint v3)
{
	GLuint xyzw[4] = { v0, v1, v2, v3 };

	glUniform4uiv(location, 1, (GLuint*)&xyzw);
}

void GL_APIENTRY glUniform1uiv(GLint location, GLsizei count, const GLuint *value)
{
	TRACE("(GLint location = %d, GLsizei count = %d, const GLuint *value = 0x%0.8p)",
	      location, count, value);
	UNIMPLEMENTED();
}

void GL_APIENTRY glUniform2uiv(GLint location, GLsizei count, const GLuint *value)
{
	TRACE("(GLint location = %d, GLsizei count = %d, const GLuint *value = 0x%0.8p)",
	      location, count, value);
	UNIMPLEMENTED();
}

void GL_APIENTRY glUniform3uiv(GLint location, GLsizei count, const GLuint *value)
{
	TRACE("(GLint location = %d, GLsizei count = %d, const GLuint *value = 0x%0.8p)",
	      location, count, value);
	UNIMPLEMENTED();
}

void GL_APIENTRY glUniform4uiv(GLint location, GLsizei count, const GLuint *value)
{
	TRACE("(GLint location = %d, GLsizei count = %d, const GLuint *value = 0x%0.8p)",
	      location, count, value);
	UNIMPLEMENTED();
}

void GL_APIENTRY glClearBufferiv(GLenum buffer, GLint drawbuffer, const GLint *value)
{
	TRACE("(GLenum buffer = 0x%X, GLint drawbuffer = %d, const GLint *value = 0x%0.8p)",
	      buffer, drawbuffer, value);

	switch(buffer)
	{
	case GL_COLOR:
		if(drawbuffer > es2::IMPLEMENTATION_MAX_DRAW_BUFFERS)
		{
			return error(GL_INVALID_VALUE);
		}
		break;
	case GL_STENCIL:
		if(drawbuffer != 0)
		{
			return error(GL_INVALID_VALUE);
		}
		break;
	default:
		return error(GL_INVALID_ENUM);
	}

	UNIMPLEMENTED();
}

void GL_APIENTRY glClearBufferuiv(GLenum buffer, GLint drawbuffer, const GLuint *value)
{
	TRACE("(GLenum buffer = 0x%X, GLint drawbuffer = %d, const GLuint *value = 0x%0.8p)",
	      buffer, drawbuffer, value);

	switch(buffer)
	{
	case GL_COLOR:
		if(drawbuffer > es2::IMPLEMENTATION_MAX_DRAW_BUFFERS)
		{
			return error(GL_INVALID_VALUE);
		}
		break;
	default:
		return error(GL_INVALID_ENUM);
	}

	UNIMPLEMENTED();
}

void GL_APIENTRY glClearBufferfv(GLenum buffer, GLint drawbuffer, const GLfloat *value)
{
	TRACE("(GLenum buffer = 0x%X, GLint drawbuffer = %d, const GLfloat *value = 0x%0.8p)",
	      buffer, drawbuffer, value);

	switch(buffer)
	{
	case GL_COLOR:
		if(drawbuffer > es2::IMPLEMENTATION_MAX_DRAW_BUFFERS)
		{
			return error(GL_INVALID_VALUE);
		}
		break;
	case GL_DEPTH:
		if(drawbuffer != 0)
		{
			return error(GL_INVALID_VALUE);
		}
		break;
	default:
		return error(GL_INVALID_ENUM);
	}

	UNIMPLEMENTED();
}

void GL_APIENTRY glClearBufferfi(GLenum buffer, GLint drawbuffer, GLfloat depth, GLint stencil)
{
	TRACE("(GLenum buffer = 0x%X, GLint drawbuffer = %d, GLfloat depth = %f, GLint stencil = %d)",
	      buffer, drawbuffer, depth, stencil);

	switch(buffer)
	{
	case GL_DEPTH_STENCIL:
		if(drawbuffer != 0)
		{
			return error(GL_INVALID_VALUE);
		}
		break;
	default:
		return error(GL_INVALID_ENUM);
	}

	UNIMPLEMENTED();
}

const GLubyte *GL_APIENTRY glGetStringi(GLenum name, GLuint index)
{
	TRACE("(GLenum name = 0x%X, GLuint index = %d)", name, index);

	UNIMPLEMENTED();
	return nullptr;
}

void GL_APIENTRY glCopyBufferSubData(GLenum readTarget, GLenum writeTarget, GLintptr readOffset, GLintptr writeOffset, GLsizeiptr size)
{
	TRACE("(GLenum readTarget = 0x%X, GLenum writeTarget = 0x%X,  GLintptr readOffset = %d, GLintptr writeOffset = %d, GLsizeiptr size = %d)",
	      readTarget, writeTarget, readOffset, writeOffset, size);

	switch(readTarget)
	{
	case GL_ARRAY_BUFFER:
	case GL_COPY_READ_BUFFER:
	case GL_COPY_WRITE_BUFFER:
	case GL_ELEMENT_ARRAY_BUFFER:
	case GL_PIXEL_PACK_BUFFER:
	case GL_PIXEL_UNPACK_BUFFER:
	case GL_TRANSFORM_FEEDBACK_BUFFER:
	case GL_UNIFORM_BUFFER:
		break;
	default:
		return error(GL_INVALID_ENUM);
	}

	switch(writeTarget)
	{
	case GL_ARRAY_BUFFER:
	case GL_COPY_READ_BUFFER:
	case GL_COPY_WRITE_BUFFER:
	case GL_ELEMENT_ARRAY_BUFFER:
	case GL_PIXEL_PACK_BUFFER:
	case GL_PIXEL_UNPACK_BUFFER:
	case GL_TRANSFORM_FEEDBACK_BUFFER:
	case GL_UNIFORM_BUFFER:
		break;
	default:
		return error(GL_INVALID_ENUM);
	}

	if(readOffset < 0 || writeOffset < 0 || size < 0)
	{
		return error(GL_INVALID_VALUE);
	}

	UNIMPLEMENTED();
}

void GL_APIENTRY glGetUniformIndices(GLuint program, GLsizei uniformCount, const GLchar *const*uniformNames, GLuint *uniformIndices)
{
	TRACE("(GLuint program = %d, GLsizei uniformCount = %d, const GLchar *const*uniformNames = 0x%0.8p, GLuint *uniformIndices = 0x%0.8p)",
	      program, uniformCount, uniformNames, uniformIndices);

	es2::Context *context = es2::getContext();

	if(context)
	{
		es2::Program *program = context->getCurrentProgram();

		if(!program)
		{
			return error(GL_INVALID_OPERATION);
		}
	}

	UNIMPLEMENTED();
}

void GL_APIENTRY glGetActiveUniformsiv(GLuint program, GLsizei uniformCount, const GLuint *uniformIndices, GLenum pname, GLint *params)
{
	TRACE("(GLuint program = %d, GLsizei uniformCount = %d, const GLchar *const*uniformNames = 0x%0.8p, GLenum pname = 0x%X, GLuint *uniformIndices = 0x%0.8p)",
	      program, uniformCount, uniformIndices, pname, uniformIndices);

	switch(pname)
	{
	case GL_UNIFORM_TYPE:
	case GL_UNIFORM_SIZE:
	case GL_UNIFORM_NAME_LENGTH:
	case GL_UNIFORM_BLOCK_INDEX:
	case GL_UNIFORM_OFFSET:
	case GL_UNIFORM_ARRAY_STRIDE:
	case GL_UNIFORM_MATRIX_STRIDE:
	case GL_UNIFORM_IS_ROW_MAJOR:
		break;
	default:
		return error(GL_INVALID_ENUM);
	}

	es2::Context *context = es2::getContext();

	if(context)
	{
		es2::Program *program = context->getCurrentProgram();

		if(!program)
		{
			return error(GL_INVALID_OPERATION);
		}
	}

	UNIMPLEMENTED();
}

GLuint GL_APIENTRY glGetUniformBlockIndex(GLuint program, const GLchar *uniformBlockName)
{
	TRACE("(GLuint program = %d, const GLchar *uniformBlockName = 0x%0.8p)",
	      program, uniformBlockName);

	es2::Context *context = es2::getContext();

	if(context)
	{
		es2::Program *program = context->getCurrentProgram();

		if(!program)
		{
			return error(GL_INVALID_OPERATION, GL_INVALID_INDEX);
		}
	}

	UNIMPLEMENTED();
	return GL_INVALID_INDEX;
}

void GL_APIENTRY glGetActiveUniformBlockiv(GLuint program, GLuint uniformBlockIndex, GLenum pname, GLint *params)
{
	TRACE("(GLuint program = %d, GLuint uniformBlockIndex = %d, GLenum pname = 0x%X, GLint *params = 0x%0.8p)",
	      program, uniformBlockIndex, pname, params);

	switch(pname)
	{
	case GL_UNIFORM_BLOCK_BINDING:
	case GL_UNIFORM_BLOCK_DATA_SIZE:
	case GL_UNIFORM_BLOCK_NAME_LENGTH:
	case GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS:
	case GL_UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES:
	case GL_UNIFORM_BLOCK_REFERENCED_BY_VERTEX_SHADER:
	case GL_UNIFORM_BLOCK_REFERENCED_BY_FRAGMENT_SHADER:
		break;
	default:
		return error(GL_INVALID_ENUM);
	}

	es2::Context *context = es2::getContext();

	if(context)
	{
		es2::Program *program = context->getCurrentProgram();

		if(!program)
		{
			return error(GL_INVALID_OPERATION);
		}
	}

	UNIMPLEMENTED();
}

void GL_APIENTRY glGetActiveUniformBlockName(GLuint program, GLuint uniformBlockIndex, GLsizei bufSize, GLsizei *length, GLchar *uniformBlockName)
{
	TRACE("(GLuint program = %d, GLuint uniformBlockIndex = %d, GLsizei bufSize = %d, GLsizei *length = 0x%0.8p, GLchar *uniformBlockName = 0x%0.8p)",
	      program, uniformBlockIndex, bufSize, length, uniformBlockName);

	es2::Context *context = es2::getContext();

	if(context)
	{
		es2::Program *program = context->getCurrentProgram();

		if(!program)
		{
			return error(GL_INVALID_OPERATION);
		}
	}

	UNIMPLEMENTED();
}

void GL_APIENTRY glUniformBlockBinding(GLuint program, GLuint uniformBlockIndex, GLuint uniformBlockBinding)
{
	TRACE("(GLuint program = %d, GLuint uniformBlockIndex = %d, GLuint uniformBlockBinding = %d)",
	      program, uniformBlockIndex, uniformBlockBinding);

	es2::Context *context = es2::getContext();

	if(context)
	{
		es2::Program *program = context->getCurrentProgram();

		if(!program)
		{
			return error(GL_INVALID_OPERATION);
		}
	}

	UNIMPLEMENTED();
}

void GL_APIENTRY glDrawArraysInstanced(GLenum mode, GLint first, GLsizei count, GLsizei instancecount)
{
	TRACE("(GLenum mode = 0x%X, GLint first = %d, GLsizei count = %d, GLsizei instancecount = %d)",
	      mode, first, count, instancecount);

	switch(mode)
	{
	case GL_POINTS:
	case GL_LINES:
	case GL_LINE_LOOP:
	case GL_LINE_STRIP:
	case GL_TRIANGLES:
	case GL_TRIANGLE_FAN:
	case GL_TRIANGLE_STRIP:
		break;
	default:
		return error(GL_INVALID_ENUM);
	}

	if(count < 0 || instancecount < 0)
	{
		return error(GL_INVALID_VALUE);
	}

	UNIMPLEMENTED();
}

void GL_APIENTRY glDrawElementsInstanced(GLenum mode, GLsizei count, GLenum type, const void *indices, GLsizei instancecount)
{
	TRACE("(GLenum mode = 0x%X, GLsizei count = %d, GLenum type = 0x%X, const void *indices = 0x%0.8p, GLsizei instancecount = %d)",
	      mode, count, type, indices, instancecount);

	switch(mode)
	{
	case GL_POINTS:
	case GL_LINES:
	case GL_LINE_LOOP:
	case GL_LINE_STRIP:
	case GL_TRIANGLES:
	case GL_TRIANGLE_FAN:
	case GL_TRIANGLE_STRIP:
		break;
	default:
		return error(GL_INVALID_ENUM);
	}

	switch(type)
	{
	case GL_UNSIGNED_BYTE:
	case GL_UNSIGNED_SHORT:
	case GL_UNSIGNED_INT:
		break;
	default:
		return error(GL_INVALID_ENUM);
	}

	if(count < 0 || instancecount < 0)
	{
		return error(GL_INVALID_VALUE);
	}

	UNIMPLEMENTED();
}

GLsync GL_APIENTRY glFenceSync(GLenum condition, GLbitfield flags)
{
	TRACE("(GLenum condition = 0x%X, GLbitfield flags = %X)", condition, flags);

	switch(condition)
	{
	case GL_SYNC_GPU_COMMANDS_COMPLETE:
		break;
	default:
		return error(GL_INVALID_ENUM, nullptr);
	}

	if(flags != 0)
	{
		return error(GL_INVALID_VALUE, nullptr);
	}

	UNIMPLEMENTED();
	return nullptr;
}

GLboolean GL_APIENTRY glIsSync(GLsync sync)
{
	TRACE("(GLsync sync = 0x%0.8p)", sync);

	UNIMPLEMENTED();
	return GL_FALSE;
}

void GL_APIENTRY glDeleteSync(GLsync sync)
{
	TRACE("(GLsync sync = 0x%0.8p)", sync);

	UNIMPLEMENTED();
}

GLenum GL_APIENTRY glClientWaitSync(GLsync sync, GLbitfield flags, GLuint64 timeout)
{
	TRACE("(GLsync sync = 0x%0.8p, GLbitfield flags = %X, GLuint64 timeout = %llu)", sync, flags, timeout);

	if((flags & ~(GL_SYNC_FLUSH_COMMANDS_BIT)) != 0)
	{
		error(GL_INVALID_VALUE);
	}

	UNIMPLEMENTED();
	return GL_FALSE;
}

void GL_APIENTRY glWaitSync(GLsync sync, GLbitfield flags, GLuint64 timeout)
{
	TRACE("(GLsync sync = 0x%0.8p, GLbitfield flags = %X, GLuint64 timeout = %llu)", sync, flags, timeout);

	if(flags != 0)
	{
		return error(GL_INVALID_VALUE);
	}

	if(timeout != GL_TIMEOUT_IGNORED)
	{
		return error(GL_INVALID_VALUE);
	}

	UNIMPLEMENTED();
}

void GL_APIENTRY glGetInteger64v(GLenum pname, GLint64 *data)
{
	TRACE("(GLenum pname = 0x%X, GLint64 *data = 0x%0.8p)", pname, data);
	UNIMPLEMENTED();
}

void GL_APIENTRY glGetSynciv(GLsync sync, GLenum pname, GLsizei bufSize, GLsizei *length, GLint *values)
{
	TRACE("(GLsync sync = 0x%0.8p, GLenum pname = 0x%X, GLsizei bufSize = %d, GLsizei *length = 0x%0.8p, GLint *values = 0x%0.8p)",
	      sync, pname, bufSize, length, values);

	UNIMPLEMENTED();
}

void GL_APIENTRY glGetInteger64i_v(GLenum target, GLuint index, GLint64 *data)
{
	TRACE("(GLenum target = 0x%X, GLuint index = %d, GLint64 *data = 0x%0.8p)", target, index, data);
	UNIMPLEMENTED();
}

void GL_APIENTRY glGetBufferParameteri64v(GLenum target, GLenum pname, GLint64 *params)
{
	TRACE("(GLenum target = 0x%X, GLenum pname = 0x%X, GLint64 *params = 0x%0.8p)", target, pname, params);
	UNIMPLEMENTED();
}

void GL_APIENTRY glGenSamplers(GLsizei count, GLuint *samplers)
{
	TRACE("(GLsizei count = %d, GLuint *samplers = 0x%0.8p)", count, samplers);

	if(count < 0)
	{
		return error(GL_INVALID_VALUE);
	}

	UNIMPLEMENTED();
}

void GL_APIENTRY glDeleteSamplers(GLsizei count, const GLuint *samplers)
{
	TRACE("(GLsizei count = %d, GLuint *samplers = 0x%0.8p)", count, samplers);

	if(count < 0)
	{
		return error(GL_INVALID_VALUE);
	}

	UNIMPLEMENTED();
}

GLboolean GL_APIENTRY glIsSampler(GLuint sampler)
{
	TRACE("(GLuint sampler = %d)", sampler);

	UNIMPLEMENTED();
	return GL_FALSE;
}

void GL_APIENTRY glBindSampler(GLuint unit, GLuint sampler)
{
	TRACE("(GLuint unit = %d, GLuint sampler = %d)", unit, sampler);

	UNIMPLEMENTED();
}

void GL_APIENTRY glSamplerParameteri(GLuint sampler, GLenum pname, GLint param)
{
	TRACE("(GLuint sampler = %d, GLenum pname = 0x%X, GLint param = %d)",
	      sampler, pname, param);

	glSamplerParameteriv(sampler, pname, &param);
}

void GL_APIENTRY glSamplerParameteriv(GLuint sampler, GLenum pname, const GLint *param)
{
	TRACE("(GLuint sampler = %d, GLenum pname = 0x%X, const GLint *param = 0x%0.8p)",
	      sampler, pname, param);

	UNIMPLEMENTED();
}

void GL_APIENTRY glSamplerParameterf(GLuint sampler, GLenum pname, GLfloat param)
{
	TRACE("(GLuint sampler = %d, GLenum pname = 0x%X, GLfloat param = %f)",
	      sampler, pname, param);

	glSamplerParameterfv(sampler, pname, &param);
}

void GL_APIENTRY glSamplerParameterfv(GLuint sampler, GLenum pname, const GLfloat *param)
{
	TRACE("(GLuint sampler = %d, GLenum pname = 0x%X, const GLfloat *param = 0x%0.8p)",
	      sampler, pname, param);

	UNIMPLEMENTED();
}

void GL_APIENTRY glGetSamplerParameteriv(GLuint sampler, GLenum pname, GLint *params)
{
	TRACE("(GLuint sampler = %d, GLenum pname = 0x%X, GLint *params = 0x%0.8p)",
	      sampler, pname, params);

	UNIMPLEMENTED();
}

void GL_APIENTRY glGetSamplerParameterfv(GLuint sampler, GLenum pname, GLfloat *params)
{
	TRACE("(GLuint sampler = %d, GLenum pname = 0x%X, GLfloat *params = 0x%0.8p)",
	      sampler, pname, params);

	UNIMPLEMENTED();
}

void GL_APIENTRY glVertexAttribDivisor(GLuint index, GLuint divisor)
{
	TRACE("(GLuint index = %d, GLuint divisor = %d)", index, divisor);
	UNIMPLEMENTED();
}

void GL_APIENTRY glBindTransformFeedback(GLenum target, GLuint id)
{
	TRACE("(GLenum target = 0x%X, GLuint id = %d)", target, id);

	UNIMPLEMENTED();
}

void GL_APIENTRY glDeleteTransformFeedbacks(GLsizei n, const GLuint *ids)
{
	TRACE("(GLsizei n = %d, const GLuint *ids = 0x%0.8p)", n, ids);

	UNIMPLEMENTED();
}

void GL_APIENTRY glGenTransformFeedbacks(GLsizei n, GLuint *ids)
{
	TRACE("(GLsizei n = %d, const GLuint *ids = 0x%0.8p)", n, ids);

	UNIMPLEMENTED();
}

GLboolean GL_APIENTRY glIsTransformFeedback(GLuint id)
{
	TRACE("(GLuint id = %d)", id);

	UNIMPLEMENTED();
	return GL_FALSE;
}

void GL_APIENTRY glPauseTransformFeedback(void)
{
	UNIMPLEMENTED();
}

void GL_APIENTRY glResumeTransformFeedback(void)
{
	UNIMPLEMENTED();
}

void GL_APIENTRY glGetProgramBinary(GLuint program, GLsizei bufSize, GLsizei *length, GLenum *binaryFormat, void *binary)
{
	TRACE("(GLuint program = %d, GLsizei bufSize = %d, GLsizei *length = 0x%0.8p, GLenum *binaryFormat = 0x%0.8p, void *binary = 0x%0.8p)",
	      program, bufSize, length, binaryFormat, binary);

	UNIMPLEMENTED();
}

void GL_APIENTRY glProgramBinary(GLuint program, GLenum binaryFormat, const void *binary, GLsizei length)
{
	TRACE("(GLuint program = %d, GLenum binaryFormat = 0x%X, const void *binary = 0x%0.8p, GLsizei length = %d)",
	      program, binaryFormat, binaryFormat, length);

	UNIMPLEMENTED();
}

void GL_APIENTRY glProgramParameteri(GLuint program, GLenum pname, GLint value)
{
	TRACE("(GLuint program = %d, GLenum pname = 0x%X, GLint value = %d)",
	      program, pname, value);

	UNIMPLEMENTED();
}

void GL_APIENTRY glInvalidateFramebuffer(GLenum target, GLsizei numAttachments, const GLenum *attachments)
{
	TRACE("(GLenum target = 0x%X, GLsizei numAttachments = %d, const GLenum *attachments = 0x%0.8p)",
	      target, numAttachments, attachments);

	UNIMPLEMENTED();
}

void GL_APIENTRY glInvalidateSubFramebuffer(GLenum target, GLsizei numAttachments, const GLenum *attachments, GLint x, GLint y, GLsizei width, GLsizei height)
{
	TRACE("(GLenum target = 0x%X, GLsizei numAttachments = %d, const GLenum *attachments = 0x%0.8p, GLint x = %d, GLint y = %d, GLsizei width = %d, GLsizei height = %d)",
	      target, numAttachments, attachments, x, y, width, height);

	UNIMPLEMENTED();
}

void GL_APIENTRY glTexStorage2D(GLenum target, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height)
{
	TRACE("(GLenum target = 0x%X, GLsizei levels = %d, GLenum internalformat = 0x%X, GLsizei width = %d, GLsizei height = %d)",
	      target, levels, internalformat, width, height);

	UNIMPLEMENTED();
}

void GL_APIENTRY glTexStorage3D(GLenum target, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth)
{
	TRACE("(GLenum target = 0x%X, GLsizei levels = %d, GLenum internalformat = 0x%X, GLsizei width = %d, GLsizei height = %d, GLsizei depth = %d)",
	      target, levels, internalformat, width, height, depth);

	UNIMPLEMENTED();
}

void GL_APIENTRY glGetInternalformativ(GLenum target, GLenum internalformat, GLenum pname, GLsizei bufSize, GLint *params)
{
	TRACE("(GLenum target = 0x%X, GLenum internalformat = 0x%X, GLenum pname = 0x%X, GLsizei bufSize = %d, GLint *params = 0x%0.8p)",
	      target, internalformat, pname, bufSize, params);

	UNIMPLEMENTED();
}

}
