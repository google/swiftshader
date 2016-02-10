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
// libGLESv2.cpp: Implements the exported OpenGL ES 2.0 functions.

#include "main.h"
#include "mathutil.h"
#include "utilities.h"
#include "Buffer.h"
#include "Context.h"
#include "Fence.h"
#include "Framebuffer.h"
#include "Program.h"
#include "Renderbuffer.h"
#include "Shader.h"
#include "Texture.h"
#include "Query.h"
#include "common/debug.h"
#include "Common/Version.h"

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <GLES3/gl3.h>

#include <limits>

#ifdef ANDROID
#include <cutils/log.h>
#endif

namespace es2
{

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

static bool validateColorBufferFormat(GLenum textureFormat, GLenum colorbufferFormat)
{
	GLenum validationError = ValidateCompressedFormat(textureFormat, egl::getClientVersion(), false);
	if(validationError != GL_NONE)
	{
		return error(validationError, false);
	}

	// [OpenGL ES 2.0.24] table 3.9
	switch(textureFormat)
	{
	case GL_ALPHA:
		if(colorbufferFormat != GL_ALPHA &&
		   colorbufferFormat != GL_RGBA &&
		   colorbufferFormat != GL_RGBA4 &&
		   colorbufferFormat != GL_RGB5_A1 &&
		   colorbufferFormat != GL_RGBA8_OES)
		{
			return error(GL_INVALID_OPERATION, false);
		}
		break;
	case GL_LUMINANCE:
	case GL_RGB:
		if(colorbufferFormat != GL_RGB &&
		   colorbufferFormat != GL_RGB565 &&
		   colorbufferFormat != GL_RGB8_OES &&
		   colorbufferFormat != GL_RGBA &&
		   colorbufferFormat != GL_RGBA4 &&
		   colorbufferFormat != GL_RGB5_A1 &&
		   colorbufferFormat != GL_RGBA8_OES)
		{
			return error(GL_INVALID_OPERATION, false);
		}
		break;
	case GL_LUMINANCE_ALPHA:
	case GL_RGBA:
		if(colorbufferFormat != GL_RGBA &&
		   colorbufferFormat != GL_RGBA4 &&
		   colorbufferFormat != GL_RGB5_A1 &&
		   colorbufferFormat != GL_RGBA8_OES)
		{
			return error(GL_INVALID_OPERATION, false);
		}
		break;
	case GL_DEPTH_COMPONENT:
	case GL_DEPTH_STENCIL_OES:
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
	InsertFormatMapping(map, GL_R8_EXT, GL_RED_EXT, GL_UNSIGNED_BYTE);
	InsertFormatMapping(map, GL_R16F_EXT, GL_RED_EXT, GL_HALF_FLOAT);
	InsertFormatMapping(map, GL_R16F_EXT, GL_RED_EXT, GL_HALF_FLOAT_OES);
	InsertFormatMapping(map, GL_R16F_EXT, GL_RED_EXT, GL_FLOAT);
	InsertFormatMapping(map, GL_R32F_EXT, GL_RED_EXT, GL_FLOAT);
	InsertFormatMapping(map, GL_RG8_EXT, GL_RG_EXT, GL_UNSIGNED_BYTE);
	InsertFormatMapping(map, GL_R16F_EXT, GL_RED_EXT, GL_HALF_FLOAT);
	InsertFormatMapping(map, GL_R16F_EXT, GL_RED_EXT, GL_HALF_FLOAT_OES);
	InsertFormatMapping(map, GL_R16F_EXT, GL_RED_EXT, GL_FLOAT);
	InsertFormatMapping(map, GL_RG32F_EXT, GL_RG_EXT, GL_FLOAT);
	InsertFormatMapping(map, GL_RGB8_OES, GL_RGB, GL_UNSIGNED_BYTE);
	InsertFormatMapping(map, GL_SRGB8_NV, GL_RGB, GL_UNSIGNED_BYTE);
	InsertFormatMapping(map, GL_RGB565, GL_RGB, GL_UNSIGNED_BYTE);
	InsertFormatMapping(map, GL_RGB565, GL_RGB, GL_UNSIGNED_SHORT_5_6_5);
	InsertFormatMapping(map, GL_RGB16F_EXT, GL_RGB, GL_HALF_FLOAT);
	InsertFormatMapping(map, GL_RGB16F_EXT, GL_RGB, GL_HALF_FLOAT_OES);
	InsertFormatMapping(map, GL_RGB16F_EXT, GL_RGB, GL_FLOAT);
	InsertFormatMapping(map, GL_RGB32F_EXT, GL_RGB, GL_FLOAT);
	InsertFormatMapping(map, GL_RGBA8_OES, GL_RGBA, GL_UNSIGNED_BYTE);
	InsertFormatMapping(map, GL_SRGB8_ALPHA8_EXT, GL_RGBA, GL_UNSIGNED_BYTE);
	InsertFormatMapping(map, GL_RGB5_A1, GL_RGBA, GL_UNSIGNED_BYTE);
	InsertFormatMapping(map, GL_RGB5_A1, GL_RGBA, GL_UNSIGNED_SHORT_5_5_5_1);
	InsertFormatMapping(map, GL_RGB5_A1, GL_RGBA, GL_UNSIGNED_INT_2_10_10_10_REV_EXT);
	InsertFormatMapping(map, GL_RGBA4, GL_RGBA, GL_UNSIGNED_BYTE);
	InsertFormatMapping(map, GL_RGBA4, GL_RGBA, GL_UNSIGNED_SHORT_4_4_4_4);
	InsertFormatMapping(map, GL_RGB10_A2_EXT, GL_RGBA, GL_UNSIGNED_INT_2_10_10_10_REV_EXT);
	InsertFormatMapping(map, GL_RGBA16F_EXT, GL_RGBA, GL_HALF_FLOAT);
	InsertFormatMapping(map, GL_RGBA16F_EXT, GL_RGBA, GL_HALF_FLOAT_OES);
	InsertFormatMapping(map, GL_RGBA16F_EXT, GL_RGBA, GL_FLOAT);
	InsertFormatMapping(map, GL_RGBA32F_EXT, GL_RGBA, GL_FLOAT);

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
	case GL_HALF_FLOAT_OES:
	case GL_FLOAT:
	case GL_UNSIGNED_SHORT_5_6_5:
	case GL_UNSIGNED_SHORT_4_4_4_4:
	case GL_UNSIGNED_SHORT_5_5_5_1:
	case GL_UNSIGNED_INT_2_10_10_10_REV_EXT:
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
	case GL_RED_EXT:
	case GL_RG_EXT:
	case GL_RGB:
	case GL_RGBA:
	case GL_DEPTH_COMPONENT:
	case GL_DEPTH_STENCIL_OES:
	case GL_LUMINANCE_ALPHA:
	case GL_LUMINANCE:
	case GL_ALPHA:
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

void ActiveTexture(GLenum texture)
{
	TRACE("(GLenum texture = 0x%X)", texture);

	es2::Context *context = es2::getContext();

	if(context)
	{
		if(texture < GL_TEXTURE0 || texture > GL_TEXTURE0 + es2::MAX_COMBINED_TEXTURE_IMAGE_UNITS - 1)
		{
			return error(GL_INVALID_ENUM);
		}

		context->setActiveSampler(texture - GL_TEXTURE0);
	}
}

void AttachShader(GLuint program, GLuint shader)
{
	TRACE("(GLuint program = %d, GLuint shader = %d)", program, shader);

	es2::Context *context = es2::getContext();

	if(context)
	{
		es2::Program *programObject = context->getProgram(program);
		es2::Shader *shaderObject = context->getShader(shader);

		if(!programObject)
		{
			if(context->getShader(program))
			{
				return error(GL_INVALID_OPERATION);
			}
			else
			{
				return error(GL_INVALID_VALUE);
			}
		}

		if(!shaderObject)
		{
			if(context->getProgram(shader))
			{
				return error(GL_INVALID_OPERATION);
			}
			else
			{
				return error(GL_INVALID_VALUE);
			}
		}

		if(!programObject->attachShader(shaderObject))
		{
			return error(GL_INVALID_OPERATION);
		}
	}
}

void BeginQueryEXT(GLenum target, GLuint name)
{
	TRACE("(GLenum target = 0x%X, GLuint name = %d)", target, name);

	switch(target)
	{
	case GL_ANY_SAMPLES_PASSED_EXT:
	case GL_ANY_SAMPLES_PASSED_CONSERVATIVE_EXT:
		break;
	default:
		return error(GL_INVALID_ENUM);
	}

	if(name == 0)
	{
		return error(GL_INVALID_OPERATION);
	}

	es2::Context *context = es2::getContext();

	if(context)
	{
		context->beginQuery(target, name);
	}
}

void BindAttribLocation(GLuint program, GLuint index, const GLchar* name)
{
	TRACE("(GLuint program = %d, GLuint index = %d, const GLchar* name = %s)", program, index, name);

	if(index >= es2::MAX_VERTEX_ATTRIBS)
	{
		return error(GL_INVALID_VALUE);
	}

	es2::Context *context = es2::getContext();

	if(context)
	{
		es2::Program *programObject = context->getProgram(program);

		if(!programObject)
		{
			if(context->getShader(program))
			{
				return error(GL_INVALID_OPERATION);
			}
			else
			{
				return error(GL_INVALID_VALUE);
			}
		}

		if(strncmp(name, "gl_", 3) == 0)
		{
			return error(GL_INVALID_OPERATION);
		}

		programObject->bindAttributeLocation(index, name);
	}
}

void BindBuffer(GLenum target, GLuint buffer)
{
	TRACE("(GLenum target = 0x%X, GLuint buffer = %d)", target, buffer);

	es2::Context *context = es2::getContext();

	if(context)
	{
		GLint clientVersion = egl::getClientVersion();

		switch(target)
		{
		case GL_ARRAY_BUFFER:
			context->bindArrayBuffer(buffer);
			return;
		case GL_ELEMENT_ARRAY_BUFFER:
			context->bindElementArrayBuffer(buffer);
			return;
		case GL_COPY_READ_BUFFER:
			if(clientVersion >= 3)
			{
				context->bindCopyReadBuffer(buffer);
				return;
			}
			else return error(GL_INVALID_ENUM);
		case GL_COPY_WRITE_BUFFER:
			if(clientVersion >= 3)
			{
				context->bindCopyWriteBuffer(buffer);
				return;
			}
			else return error(GL_INVALID_ENUM);
		case GL_PIXEL_PACK_BUFFER:
			if(clientVersion >= 3)
			{
				context->bindPixelPackBuffer(buffer);
				return;
			}
			else return error(GL_INVALID_ENUM);
		case GL_PIXEL_UNPACK_BUFFER:
			if(clientVersion >= 3)
			{
				context->bindPixelUnpackBuffer(buffer);
				return;
			}
			else return error(GL_INVALID_ENUM);
		case GL_TRANSFORM_FEEDBACK_BUFFER:
			if(clientVersion >= 3)
			{
				context->bindTransformFeedbackBuffer(buffer);
				return;
			}
			else return error(GL_INVALID_ENUM);
		case GL_UNIFORM_BUFFER:
			if(clientVersion >= 3)
			{
				context->bindGenericUniformBuffer(buffer);
				return;
			}
			else return error(GL_INVALID_ENUM);
		default:
			return error(GL_INVALID_ENUM);
		}
	}
}

void BindFramebuffer(GLenum target, GLuint framebuffer)
{
	TRACE("(GLenum target = 0x%X, GLuint framebuffer = %d)", target, framebuffer);

	if(target != GL_FRAMEBUFFER && target != GL_DRAW_FRAMEBUFFER_ANGLE && target != GL_READ_FRAMEBUFFER_ANGLE)
	{
		return error(GL_INVALID_ENUM);
	}

	es2::Context *context = es2::getContext();

	if(context)
	{
		if(target == GL_READ_FRAMEBUFFER_ANGLE || target == GL_FRAMEBUFFER)
		{
			context->bindReadFramebuffer(framebuffer);
		}

		if(target == GL_DRAW_FRAMEBUFFER_ANGLE || target == GL_FRAMEBUFFER)
		{
			context->bindDrawFramebuffer(framebuffer);
		}
	}
}

void BindRenderbuffer(GLenum target, GLuint renderbuffer)
{
	TRACE("(GLenum target = 0x%X, GLuint renderbuffer = %d)", target, renderbuffer);

	if(target != GL_RENDERBUFFER)
	{
		return error(GL_INVALID_ENUM);
	}

	es2::Context *context = es2::getContext();

	if(context)
	{
		// [OpenGL ES 2.0.25] Section 4.4.3 page 110
		// [OpenGL ES 3.0.4] Section 4.4.2 page 204
		// If renderbuffer is not zero, then the resulting renderbuffer object
		// is a new state vector, initialized with a zero-sized memory buffer.
		context->bindRenderbuffer(renderbuffer);
	}
}

void BindTexture(GLenum target, GLuint texture)
{
	TRACE("(GLenum target = 0x%X, GLuint texture = %d)", target, texture);

	es2::Context *context = es2::getContext();

	if(context)
	{
		es2::Texture *textureObject = context->getTexture(texture);

		if(textureObject && textureObject->getTarget() != target && texture != 0)
		{
			return error(GL_INVALID_OPERATION);
		}

		GLint clientVersion = context->getClientVersion();

		switch(target)
		{
		case GL_TEXTURE_2D:
			context->bindTexture2D(texture);
			break;
		case GL_TEXTURE_CUBE_MAP:
			context->bindTextureCubeMap(texture);
			break;
		case GL_TEXTURE_EXTERNAL_OES:
			context->bindTextureExternal(texture);
			break;
		case GL_TEXTURE_2D_ARRAY:
			if(clientVersion < 3)
			{
				return error(GL_INVALID_ENUM);
			}
			context->bindTexture2DArray(texture);
			break;
		case GL_TEXTURE_3D_OES:
			context->bindTexture3D(texture);
			break;
		default:
			return error(GL_INVALID_ENUM);
		}
	}
}

void BlendColor(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha)
{
	TRACE("(GLclampf red = %f, GLclampf green = %f, GLclampf blue = %f, GLclampf alpha = %f)",
		red, green, blue, alpha);

	es2::Context* context = es2::getContext();

	if(context)
	{
		context->setBlendColor(es2::clamp01(red), es2::clamp01(green), es2::clamp01(blue), es2::clamp01(alpha));
	}
}

void BlendEquation(GLenum mode)
{
	glBlendEquationSeparate(mode, mode);
}

void BlendEquationSeparate(GLenum modeRGB, GLenum modeAlpha)
{
	TRACE("(GLenum modeRGB = 0x%X, GLenum modeAlpha = 0x%X)", modeRGB, modeAlpha);

	switch(modeRGB)
	{
	case GL_FUNC_ADD:
	case GL_FUNC_SUBTRACT:
	case GL_FUNC_REVERSE_SUBTRACT:
	case GL_MIN_EXT:
	case GL_MAX_EXT:
		break;
	default:
		return error(GL_INVALID_ENUM);
	}

	switch(modeAlpha)
	{
	case GL_FUNC_ADD:
	case GL_FUNC_SUBTRACT:
	case GL_FUNC_REVERSE_SUBTRACT:
	case GL_MIN_EXT:
	case GL_MAX_EXT:
		break;
	default:
		return error(GL_INVALID_ENUM);
	}

	es2::Context *context = es2::getContext();

	if(context)
	{
		context->setBlendEquation(modeRGB, modeAlpha);
	}
}

void BlendFunc(GLenum sfactor, GLenum dfactor)
{
	glBlendFuncSeparate(sfactor, dfactor, sfactor, dfactor);
}

void BlendFuncSeparate(GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha)
{
	TRACE("(GLenum srcRGB = 0x%X, GLenum dstRGB = 0x%X, GLenum srcAlpha = 0x%X, GLenum dstAlpha = 0x%X)",
	      srcRGB, dstRGB, srcAlpha, dstAlpha);

	GLint clientVersion = egl::getClientVersion();

	switch(srcRGB)
	{
	case GL_ZERO:
	case GL_ONE:
	case GL_SRC_COLOR:
	case GL_ONE_MINUS_SRC_COLOR:
	case GL_DST_COLOR:
	case GL_ONE_MINUS_DST_COLOR:
	case GL_SRC_ALPHA:
	case GL_ONE_MINUS_SRC_ALPHA:
	case GL_DST_ALPHA:
	case GL_ONE_MINUS_DST_ALPHA:
	case GL_CONSTANT_COLOR:
	case GL_ONE_MINUS_CONSTANT_COLOR:
	case GL_CONSTANT_ALPHA:
	case GL_ONE_MINUS_CONSTANT_ALPHA:
	case GL_SRC_ALPHA_SATURATE:
		break;
	default:
		return error(GL_INVALID_ENUM);
	}

	switch(dstRGB)
	{
	case GL_ZERO:
	case GL_ONE:
	case GL_SRC_COLOR:
	case GL_ONE_MINUS_SRC_COLOR:
	case GL_DST_COLOR:
	case GL_ONE_MINUS_DST_COLOR:
	case GL_SRC_ALPHA:
	case GL_ONE_MINUS_SRC_ALPHA:
	case GL_DST_ALPHA:
	case GL_ONE_MINUS_DST_ALPHA:
	case GL_CONSTANT_COLOR:
	case GL_ONE_MINUS_CONSTANT_COLOR:
	case GL_CONSTANT_ALPHA:
	case GL_ONE_MINUS_CONSTANT_ALPHA:
		break;
	case GL_SRC_ALPHA_SATURATE:
		if(clientVersion < 3)
		{
			return error(GL_INVALID_ENUM);
		}
		break;
	default:
		return error(GL_INVALID_ENUM);
	}

	switch(srcAlpha)
	{
	case GL_ZERO:
	case GL_ONE:
	case GL_SRC_COLOR:
	case GL_ONE_MINUS_SRC_COLOR:
	case GL_DST_COLOR:
	case GL_ONE_MINUS_DST_COLOR:
	case GL_SRC_ALPHA:
	case GL_ONE_MINUS_SRC_ALPHA:
	case GL_DST_ALPHA:
	case GL_ONE_MINUS_DST_ALPHA:
	case GL_CONSTANT_COLOR:
	case GL_ONE_MINUS_CONSTANT_COLOR:
	case GL_CONSTANT_ALPHA:
	case GL_ONE_MINUS_CONSTANT_ALPHA:
	case GL_SRC_ALPHA_SATURATE:
		break;
	default:
		return error(GL_INVALID_ENUM);
	}

	switch(dstAlpha)
	{
	case GL_ZERO:
	case GL_ONE:
	case GL_SRC_COLOR:
	case GL_ONE_MINUS_SRC_COLOR:
	case GL_DST_COLOR:
	case GL_ONE_MINUS_DST_COLOR:
	case GL_SRC_ALPHA:
	case GL_ONE_MINUS_SRC_ALPHA:
	case GL_DST_ALPHA:
	case GL_ONE_MINUS_DST_ALPHA:
	case GL_CONSTANT_COLOR:
	case GL_ONE_MINUS_CONSTANT_COLOR:
	case GL_CONSTANT_ALPHA:
	case GL_ONE_MINUS_CONSTANT_ALPHA:
		break;
	case GL_SRC_ALPHA_SATURATE:
		if(clientVersion < 3)
		{
			return error(GL_INVALID_ENUM);
		}
		break;
	default:
		return error(GL_INVALID_ENUM);
	}

	es2::Context *context = es2::getContext();

	if(context)
	{
		context->setBlendFactors(srcRGB, dstRGB, srcAlpha, dstAlpha);
	}
}

void BufferData(GLenum target, GLsizeiptr size, const GLvoid* data, GLenum usage)
{
	size = static_cast<GLint>(size);   // Work around issues with some 64-bit applications

	TRACE("(GLenum target = 0x%X, GLsizeiptr size = %d, const GLvoid* data = %p, GLenum usage = %d)",
	      target, size, data, usage);

	if(size < 0)
	{
		return error(GL_INVALID_VALUE);
	}

	GLint clientVersion = egl::getClientVersion();

	switch(usage)
	{
	case GL_STREAM_DRAW:
	case GL_STATIC_DRAW:
	case GL_DYNAMIC_DRAW:
		break;
	case GL_STREAM_READ:
	case GL_STREAM_COPY:
	case GL_STATIC_READ:
	case GL_STATIC_COPY:
	case GL_DYNAMIC_READ:
	case GL_DYNAMIC_COPY:
		if(clientVersion < 3)
		{
			return error(GL_INVALID_ENUM);
		}
		break;
	default:
		return error(GL_INVALID_ENUM);
	}

	es2::Context *context = es2::getContext();

	if(context)
	{
		es2::Buffer *buffer = nullptr;
		if(!context->getBuffer(target, &buffer))
		{
			return error(GL_INVALID_ENUM);
		}

		if(!buffer)
		{
			// A null buffer means that "0" is bound to the requested buffer target
			return error(GL_INVALID_OPERATION);
		}

		buffer->bufferData(data, size, usage);
	}
}

void BufferSubData(GLenum target, GLintptr offset, GLsizeiptr size, const GLvoid* data)
{
	size = static_cast<GLint>(size);   // Work around issues with some 64-bit applications
	offset = static_cast<GLint>(offset);

	TRACE("(GLenum target = 0x%X, GLintptr offset = %d, GLsizeiptr size = %d, const GLvoid* data = %p)",
	      target, offset, size, data);

	if(size < 0 || offset < 0)
	{
		return error(GL_INVALID_VALUE);
	}

	es2::Context *context = es2::getContext();

	if(context)
	{
		es2::Buffer *buffer = nullptr;
		if(!context->getBuffer(target, &buffer))
		{
			return error(GL_INVALID_ENUM);
		}

		if(!buffer)
		{
			// A null buffer means that "0" is bound to the requested buffer target
			return error(GL_INVALID_OPERATION);
		}

		if((size_t)size + offset > buffer->size())
		{
			return error(GL_INVALID_VALUE);
		}

		buffer->bufferSubData(data, size, offset);
	}
}

GLenum CheckFramebufferStatus(GLenum target)
{
	TRACE("(GLenum target = 0x%X)", target);

	if(target != GL_FRAMEBUFFER && target != GL_DRAW_FRAMEBUFFER_ANGLE && target != GL_READ_FRAMEBUFFER_ANGLE)
	{
		return error(GL_INVALID_ENUM, 0);
	}

	es2::Context *context = es2::getContext();

	if(context)
	{
		es2::Framebuffer *framebuffer = NULL;
		if(target == GL_READ_FRAMEBUFFER_ANGLE)
		{
			framebuffer = context->getReadFramebuffer();
		}
		else
		{
			framebuffer = context->getDrawFramebuffer();
		}

		return framebuffer->completeness();
	}

	return 0;
}

void Clear(GLbitfield mask)
{
	TRACE("(GLbitfield mask = %X)", mask);

	if((mask & ~(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT)) != 0)
	{
		return error(GL_INVALID_VALUE);
	}

	es2::Context *context = es2::getContext();

	if(context)
	{
		context->clear(mask);
	}
}

void ClearColor(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha)
{
	TRACE("(GLclampf red = %f, GLclampf green = %f, GLclampf blue = %f, GLclampf alpha = %f)",
	      red, green, blue, alpha);

	es2::Context *context = es2::getContext();

	if(context)
	{
		context->setClearColor(red, green, blue, alpha);
	}
}

void ClearDepthf(GLclampf depth)
{
	TRACE("(GLclampf depth = %f)", depth);

	es2::Context *context = es2::getContext();

	if(context)
	{
		context->setClearDepth(depth);
	}
}

void ClearStencil(GLint s)
{
	TRACE("(GLint s = %d)", s);

	es2::Context *context = es2::getContext();

	if(context)
	{
		context->setClearStencil(s);
	}
}

void ColorMask(GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha)
{
	TRACE("(GLboolean red = %d, GLboolean green = %d, GLboolean blue = %d, GLboolean alpha = %d)",
	      red, green, blue, alpha);

	es2::Context *context = es2::getContext();

	if(context)
	{
		context->setColorMask(red == GL_TRUE, green == GL_TRUE, blue == GL_TRUE, alpha == GL_TRUE);
	}
}

void CompileShader(GLuint shader)
{
	TRACE("(GLuint shader = %d)", shader);

	es2::Context *context = es2::getContext();

	if(context)
	{
		es2::Shader *shaderObject = context->getShader(shader);

		if(!shaderObject)
		{
			if(context->getProgram(shader))
			{
				return error(GL_INVALID_OPERATION);
			}
			else
			{
				return error(GL_INVALID_VALUE);
			}
		}

		shaderObject->compile();
	}
}

void CompressedTexImage2D(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height,
                          GLint border, GLsizei imageSize, const GLvoid* data)
{
	TRACE("(GLenum target = 0x%X, GLint level = %d, GLenum internalformat = 0x%X, GLsizei width = %d, "
	      "GLsizei height = %d, GLint border = %d, GLsizei imageSize = %d, const GLvoid* data = %p)",
	      target, level, internalformat, width, height, border, imageSize, data);

	if(!validImageSize(level, width, height) || border != 0 || imageSize < 0)
	{
		return error(GL_INVALID_VALUE);
	}

	switch(internalformat)
	{
	case GL_DEPTH_COMPONENT:
	case GL_DEPTH_COMPONENT16:
	case GL_DEPTH_COMPONENT32_OES:
	case GL_DEPTH_STENCIL_OES:
	case GL_DEPTH24_STENCIL8_OES:
		return error(GL_INVALID_OPERATION);
	default:
		{
			GLenum validationError = ValidateCompressedFormat(internalformat, egl::getClientVersion(), true);
			if(validationError != GL_NONE)
			{
				return error(validationError);
			}
		}
		break;
	}

	if(border != 0)
	{
		return error(GL_INVALID_VALUE);
	}

	es2::Context *context = es2::getContext();

	if(context)
	{
		if(level >= es2::IMPLEMENTATION_MAX_TEXTURE_LEVELS)
		{
			return error(GL_INVALID_VALUE);
		}

		switch(target)
		{
		case GL_TEXTURE_2D:
			if(width > (es2::IMPLEMENTATION_MAX_TEXTURE_SIZE >> level) ||
			   height > (es2::IMPLEMENTATION_MAX_TEXTURE_SIZE >> level))
			{
				return error(GL_INVALID_VALUE);
			}
			break;
		case GL_TEXTURE_CUBE_MAP_POSITIVE_X:
		case GL_TEXTURE_CUBE_MAP_NEGATIVE_X:
		case GL_TEXTURE_CUBE_MAP_POSITIVE_Y:
		case GL_TEXTURE_CUBE_MAP_NEGATIVE_Y:
		case GL_TEXTURE_CUBE_MAP_POSITIVE_Z:
		case GL_TEXTURE_CUBE_MAP_NEGATIVE_Z:
			if(width != height)
			{
				return error(GL_INVALID_VALUE);
			}

			if(width > (es2::IMPLEMENTATION_MAX_CUBE_MAP_TEXTURE_SIZE >> level) ||
			   height > (es2::IMPLEMENTATION_MAX_CUBE_MAP_TEXTURE_SIZE >> level))
			{
				return error(GL_INVALID_VALUE);
			}
			break;
		default:
			return error(GL_INVALID_ENUM);
		}

		if(imageSize != egl::ComputeCompressedSize(width, height, internalformat))
		{
			return error(GL_INVALID_VALUE);
		}

		if(target == GL_TEXTURE_2D)
		{
			es2::Texture2D *texture = context->getTexture2D();

			if(!texture)
			{
				return error(GL_INVALID_OPERATION);
			}

			texture->setCompressedImage(level, internalformat, width, height, imageSize, data);
		}
		else
		{
			es2::TextureCubeMap *texture = context->getTextureCubeMap();

			if(!texture)
			{
				return error(GL_INVALID_OPERATION);
			}

			switch(target)
			{
			case GL_TEXTURE_CUBE_MAP_POSITIVE_X:
			case GL_TEXTURE_CUBE_MAP_NEGATIVE_X:
			case GL_TEXTURE_CUBE_MAP_POSITIVE_Y:
			case GL_TEXTURE_CUBE_MAP_NEGATIVE_Y:
			case GL_TEXTURE_CUBE_MAP_POSITIVE_Z:
			case GL_TEXTURE_CUBE_MAP_NEGATIVE_Z:
				texture->setCompressedImage(target, level, internalformat, width, height, imageSize, data);
				break;
			default: UNREACHABLE(target);
			}
		}
	}
}

void CompressedTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height,
                             GLenum format, GLsizei imageSize, const GLvoid* data)
{
	TRACE("(GLenum target = 0x%X, GLint level = %d, GLint xoffset = %d, GLint yoffset = %d, "
	      "GLsizei width = %d, GLsizei height = %d, GLenum format = 0x%X, "
	      "GLsizei imageSize = %d, const GLvoid* data = %p)",
	      target, level, xoffset, yoffset, width, height, format, imageSize, data);

	if(!es2::IsTextureTarget(target))
	{
		return error(GL_INVALID_ENUM);
	}

	if(xoffset < 0 || yoffset < 0 || !validImageSize(level, width, height) || imageSize < 0)
	{
		return error(GL_INVALID_VALUE);
	}

	GLenum validationError = ValidateCompressedFormat(format, egl::getClientVersion(), true);
	if(validationError != GL_NONE)
	{
		return error(validationError);
	}

	if(width == 0 || height == 0 || data == NULL)
	{
		return;
	}

	es2::Context *context = es2::getContext();

	if(context)
	{
		if(level >= es2::IMPLEMENTATION_MAX_TEXTURE_LEVELS)
		{
			return error(GL_INVALID_VALUE);
		}

		if(imageSize != egl::ComputeCompressedSize(width, height, format))
		{
			return error(GL_INVALID_VALUE);
		}

		if(xoffset % 4 != 0 || yoffset % 4 != 0)
		{
			// We wait to check the offsets until this point, because the multiple-of-four restriction does not exist unless DXT1 textures are supported
			return error(GL_INVALID_OPERATION);
		}

		GLenum sizedInternalFormat = GetSizedInternalFormat(format, GL_NONE);

		if(target == GL_TEXTURE_2D)
		{
			es2::Texture2D *texture = context->getTexture2D();

			GLenum validationError = ValidateSubImageParams(true, width, height, xoffset, yoffset, target, level, sizedInternalFormat, texture);

			if(validationError == GL_NONE)
			{
				texture->subImageCompressed(level, xoffset, yoffset, width, height, sizedInternalFormat, imageSize, data);
			}
			else
			{
				return error(validationError);
			}
		}
		else if(es2::IsCubemapTextureTarget(target))
		{
			es2::TextureCubeMap *texture = context->getTextureCubeMap();

			GLenum validationError = ValidateSubImageParams(true, width, height, xoffset, yoffset, target, level, sizedInternalFormat, texture);

			if(validationError == GL_NONE)
			{
				texture->subImageCompressed(target, level, xoffset, yoffset, width, height, sizedInternalFormat, imageSize, data);
			}
			else
			{
				return error(validationError);
			}
		}
		else UNREACHABLE(target);
	}
}

void CopyTexImage2D(GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border)
{
	TRACE("(GLenum target = 0x%X, GLint level = %d, GLenum internalformat = 0x%X, "
	      "GLint x = %d, GLint y = %d, GLsizei width = %d, GLsizei height = %d, GLint border = %d)",
	      target, level, internalformat, x, y, width, height, border);

	if(!validImageSize(level, width, height))
	{
		return error(GL_INVALID_VALUE);
	}

	if(border != 0)
	{
		return error(GL_INVALID_VALUE);
	}

	es2::Context *context = es2::getContext();

	if(context)
	{
		switch(target)
		{
		case GL_TEXTURE_2D:
			if(width > (es2::IMPLEMENTATION_MAX_TEXTURE_SIZE >> level) ||
			   height > (es2::IMPLEMENTATION_MAX_TEXTURE_SIZE >> level))
			{
				return error(GL_INVALID_VALUE);
			}
			break;
		case GL_TEXTURE_CUBE_MAP_POSITIVE_X:
		case GL_TEXTURE_CUBE_MAP_NEGATIVE_X:
		case GL_TEXTURE_CUBE_MAP_POSITIVE_Y:
		case GL_TEXTURE_CUBE_MAP_NEGATIVE_Y:
		case GL_TEXTURE_CUBE_MAP_POSITIVE_Z:
		case GL_TEXTURE_CUBE_MAP_NEGATIVE_Z:
			if(width != height)
			{
				return error(GL_INVALID_VALUE);
			}

			if(width > (es2::IMPLEMENTATION_MAX_CUBE_MAP_TEXTURE_SIZE >> level) ||
			   height > (es2::IMPLEMENTATION_MAX_CUBE_MAP_TEXTURE_SIZE >> level))
			{
				return error(GL_INVALID_VALUE);
			}
			break;
		default:
			return error(GL_INVALID_ENUM);
		}

		es2::Framebuffer *framebuffer = context->getReadFramebuffer();

		if(framebuffer->completeness() != GL_FRAMEBUFFER_COMPLETE)
		{
			return error(GL_INVALID_FRAMEBUFFER_OPERATION);
		}

		es2::Renderbuffer *source = framebuffer->getReadColorbuffer();

		if(context->getReadFramebufferName() != 0 && (!source || source->getSamples() > 1))
		{
			return error(GL_INVALID_OPERATION);
		}

		GLenum colorbufferFormat = source->getFormat();

		if(!validateColorBufferFormat(internalformat, colorbufferFormat))
		{
			return;
		}

		if(target == GL_TEXTURE_2D)
		{
			es2::Texture2D *texture = context->getTexture2D();

			if(!texture)
			{
				return error(GL_INVALID_OPERATION);
			}

			texture->copyImage(level, internalformat, x, y, width, height, framebuffer);
		}
		else if(es2::IsCubemapTextureTarget(target))
		{
			es2::TextureCubeMap *texture = context->getTextureCubeMap();

			if(!texture)
			{
				return error(GL_INVALID_OPERATION);
			}

			texture->copyImage(target, level, internalformat, x, y, width, height, framebuffer);
		}
		else UNREACHABLE(target);
	}
}

void CopyTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height)
{
	TRACE("(GLenum target = 0x%X, GLint level = %d, GLint xoffset = %d, GLint yoffset = %d, "
	      "GLint x = %d, GLint y = %d, GLsizei width = %d, GLsizei height = %d)",
	      target, level, xoffset, yoffset, x, y, width, height);

	if(!es2::IsTextureTarget(target))
	{
		return error(GL_INVALID_ENUM);
	}

	if(level < 0 || xoffset < 0 || yoffset < 0 || width < 0 || height < 0)
	{
		return error(GL_INVALID_VALUE);
	}

	if(std::numeric_limits<GLsizei>::max() - xoffset < width || std::numeric_limits<GLsizei>::max() - yoffset < height)
	{
		return error(GL_INVALID_VALUE);
	}

	if(width == 0 || height == 0)
	{
		return;
	}

	es2::Context *context = es2::getContext();

	if(context)
	{
		if(level >= es2::IMPLEMENTATION_MAX_TEXTURE_LEVELS)
		{
			return error(GL_INVALID_VALUE);
		}

		es2::Framebuffer *framebuffer = context->getReadFramebuffer();

		if(framebuffer->completeness() != GL_FRAMEBUFFER_COMPLETE)
		{
			return error(GL_INVALID_FRAMEBUFFER_OPERATION);
		}

		es2::Renderbuffer *source = framebuffer->getReadColorbuffer();

		if(context->getReadFramebufferName() != 0 && (!source || source->getSamples() > 1))
		{
			return error(GL_INVALID_OPERATION);
		}

		es2::Texture *texture = NULL;

		if(target == GL_TEXTURE_2D)
		{
			texture = context->getTexture2D();
		}
		else if(es2::IsCubemapTextureTarget(target))
		{
			texture = context->getTextureCubeMap();
		}
		else UNREACHABLE(target);

		GLenum validationError = ValidateSubImageParams(false, width, height, xoffset, yoffset, target, level, GL_NONE, texture);
		if(validationError != GL_NONE)
		{
			return error(validationError);
		}

		texture->copySubImage(target, level, xoffset, yoffset, 0, x, y, width, height, framebuffer);
	}
}

GLuint CreateProgram(void)
{
	TRACE("()");

	es2::Context *context = es2::getContext();

	if(context)
	{
		return context->createProgram();
	}

	return 0;
}

GLuint CreateShader(GLenum type)
{
	TRACE("(GLenum type = 0x%X)", type);

	es2::Context *context = es2::getContext();

	if(context)
	{
		switch(type)
		{
		case GL_FRAGMENT_SHADER:
		case GL_VERTEX_SHADER:
			return context->createShader(type);
		default:
			return error(GL_INVALID_ENUM, 0);
		}
	}

	return 0;
}

void CullFace(GLenum mode)
{
	TRACE("(GLenum mode = 0x%X)", mode);

	switch(mode)
	{
	case GL_FRONT:
	case GL_BACK:
	case GL_FRONT_AND_BACK:
		{
			es2::Context *context = es2::getContext();

			if(context)
			{
				context->setCullMode(mode);
			}
		}
		break;
	default:
		return error(GL_INVALID_ENUM);
	}
}

void DeleteBuffers(GLsizei n, const GLuint* buffers)
{
	TRACE("(GLsizei n = %d, const GLuint* buffers = %p)", n, buffers);

	if(n < 0)
	{
		return error(GL_INVALID_VALUE);
	}

	es2::Context *context = es2::getContext();

	if(context)
	{
		for(int i = 0; i < n; i++)
		{
			context->deleteBuffer(buffers[i]);
		}
	}
}

void DeleteFencesNV(GLsizei n, const GLuint* fences)
{
	TRACE("(GLsizei n = %d, const GLuint* fences = %p)", n, fences);

	if(n < 0)
	{
		return error(GL_INVALID_VALUE);
	}

	es2::Context *context = es2::getContext();

	if(context)
	{
		for(int i = 0; i < n; i++)
		{
			context->deleteFence(fences[i]);
		}
	}
}

void DeleteFramebuffers(GLsizei n, const GLuint* framebuffers)
{
	TRACE("(GLsizei n = %d, const GLuint* framebuffers = %p)", n, framebuffers);

	if(n < 0)
	{
		return error(GL_INVALID_VALUE);
	}

	es2::Context *context = es2::getContext();

	if(context)
	{
		for(int i = 0; i < n; i++)
		{
			if(framebuffers[i] != 0)
			{
				context->deleteFramebuffer(framebuffers[i]);
			}
		}
	}
}

void DeleteProgram(GLuint program)
{
	TRACE("(GLuint program = %d)", program);

	if(program == 0)
	{
		return;
	}

	es2::Context *context = es2::getContext();

	if(context)
	{
		if(!context->getProgram(program))
		{
			if(context->getShader(program))
			{
				return error(GL_INVALID_OPERATION);
			}
			else
			{
				return error(GL_INVALID_VALUE);
			}
		}

		context->deleteProgram(program);
	}
}

void DeleteQueriesEXT(GLsizei n, const GLuint *ids)
{
	TRACE("(GLsizei n = %d, const GLuint *ids = %p)", n, ids);

	if(n < 0)
	{
		return error(GL_INVALID_VALUE);
	}

	es2::Context *context = es2::getContext();

	if(context)
	{
		for(int i = 0; i < n; i++)
		{
			context->deleteQuery(ids[i]);
		}
	}
}

void DeleteRenderbuffers(GLsizei n, const GLuint* renderbuffers)
{
	TRACE("(GLsizei n = %d, const GLuint* renderbuffers = %p)", n, renderbuffers);

	if(n < 0)
	{
		return error(GL_INVALID_VALUE);
	}

	es2::Context *context = es2::getContext();

	if(context)
	{
		for(int i = 0; i < n; i++)
		{
			context->deleteRenderbuffer(renderbuffers[i]);
		}
	}
}

void DeleteShader(GLuint shader)
{
	TRACE("(GLuint shader = %d)", shader);

	if(shader == 0)
	{
		return;
	}

	es2::Context *context = es2::getContext();

	if(context)
	{
		if(!context->getShader(shader))
		{
			if(context->getProgram(shader))
			{
				return error(GL_INVALID_OPERATION);
			}
			else
			{
				return error(GL_INVALID_VALUE);
			}
		}

		context->deleteShader(shader);
	}
}

void DeleteTextures(GLsizei n, const GLuint* textures)
{
	TRACE("(GLsizei n = %d, const GLuint* textures = %p)", n, textures);

	if(n < 0)
	{
		return error(GL_INVALID_VALUE);
	}

	es2::Context *context = es2::getContext();

	if(context)
	{
		for(int i = 0; i < n; i++)
		{
			if(textures[i] != 0)
			{
				context->deleteTexture(textures[i]);
			}
		}
	}
}

void DepthFunc(GLenum func)
{
	TRACE("(GLenum func = 0x%X)", func);

	switch(func)
	{
	case GL_NEVER:
	case GL_ALWAYS:
	case GL_LESS:
	case GL_LEQUAL:
	case GL_EQUAL:
	case GL_GREATER:
	case GL_GEQUAL:
	case GL_NOTEQUAL:
		break;
	default:
		return error(GL_INVALID_ENUM);
	}

	es2::Context *context = es2::getContext();

	if(context)
	{
		context->setDepthFunc(func);
	}
}

void DepthMask(GLboolean flag)
{
	TRACE("(GLboolean flag = %d)", flag);

	es2::Context *context = es2::getContext();

	if(context)
	{
		context->setDepthMask(flag != GL_FALSE);
	}
}

void DepthRangef(GLclampf zNear, GLclampf zFar)
{
	TRACE("(GLclampf zNear = %f, GLclampf zFar = %f)", zNear, zFar);

	es2::Context *context = es2::getContext();

	if(context)
	{
		context->setDepthRange(zNear, zFar);
	}
}

void DetachShader(GLuint program, GLuint shader)
{
	TRACE("(GLuint program = %d, GLuint shader = %d)", program, shader);

	es2::Context *context = es2::getContext();

	if(context)
	{

		es2::Program *programObject = context->getProgram(program);
		es2::Shader *shaderObject = context->getShader(shader);

		if(!programObject)
		{
			es2::Shader *shaderByProgramHandle;
			shaderByProgramHandle = context->getShader(program);
			if(!shaderByProgramHandle)
			{
				return error(GL_INVALID_VALUE);
			}
			else
			{
				return error(GL_INVALID_OPERATION);
			}
		}

		if(!shaderObject)
		{
			es2::Program *programByShaderHandle = context->getProgram(shader);
			if(!programByShaderHandle)
			{
				return error(GL_INVALID_VALUE);
			}
			else
			{
				return error(GL_INVALID_OPERATION);
			}
		}

		if(!programObject->detachShader(shaderObject))
		{
			return error(GL_INVALID_OPERATION);
		}
	}
}

void Disable(GLenum cap)
{
	TRACE("(GLenum cap = 0x%X)", cap);

	es2::Context *context = es2::getContext();

	if(context)
	{
		switch(cap)
		{
		case GL_CULL_FACE:                     context->setCullFaceEnabled(false);                   break;
		case GL_POLYGON_OFFSET_FILL:           context->setPolygonOffsetFillEnabled(false);          break;
		case GL_SAMPLE_ALPHA_TO_COVERAGE:      context->setSampleAlphaToCoverageEnabled(false);      break;
		case GL_SAMPLE_COVERAGE:               context->setSampleCoverageEnabled(false);             break;
		case GL_SCISSOR_TEST:                  context->setScissorTestEnabled(false);                break;
		case GL_STENCIL_TEST:                  context->setStencilTestEnabled(false);                break;
		case GL_DEPTH_TEST:                    context->setDepthTestEnabled(false);                  break;
		case GL_BLEND:                         context->setBlendEnabled(false);                      break;
		case GL_DITHER:                        context->setDitherEnabled(false);                     break;
		case GL_PRIMITIVE_RESTART_FIXED_INDEX: context->setPrimitiveRestartFixedIndexEnabled(false); break;
		case GL_RASTERIZER_DISCARD:            context->setRasterizerDiscardEnabled(false);          break;
		default:
			return error(GL_INVALID_ENUM);
		}
	}
}

void DisableVertexAttribArray(GLuint index)
{
	TRACE("(GLuint index = %d)", index);

	if(index >= es2::MAX_VERTEX_ATTRIBS)
	{
		return error(GL_INVALID_VALUE);
	}

	es2::Context *context = es2::getContext();

	if(context)
	{
		context->setVertexAttribArrayEnabled(index, false);
	}
}

void DrawArrays(GLenum mode, GLint first, GLsizei count)
{
	TRACE("(GLenum mode = 0x%X, GLint first = %d, GLsizei count = %d)", mode, first, count);

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

	if(count < 0 || first < 0)
	{
		return error(GL_INVALID_VALUE);
	}

	es2::Context *context = es2::getContext();

	if(context)
	{
		es2::TransformFeedback* transformFeedback = context->getTransformFeedback();
		if(transformFeedback && transformFeedback->isActive() && (mode != transformFeedback->primitiveMode()))
		{
			return error(GL_INVALID_OPERATION);
		}

		context->drawArrays(mode, first, count);
	}
}

void DrawElements(GLenum mode, GLsizei count, GLenum type, const GLvoid* indices)
{
	TRACE("(GLenum mode = 0x%X, GLsizei count = %d, GLenum type = 0x%X, const GLvoid* indices = %p)",
	      mode, count, type, indices);

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

	if(count < 0)
	{
		return error(GL_INVALID_VALUE);
	}

	es2::Context *context = es2::getContext();

	if(context)
	{
		es2::TransformFeedback* transformFeedback = context->getTransformFeedback();
		if(transformFeedback && transformFeedback->isActive() && !transformFeedback->isPaused())
		{
			return error(GL_INVALID_OPERATION);
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

		context->drawElements(mode, 0, MAX_ELEMENT_INDEX, count, type, indices);
	}
}

void DrawArraysInstancedEXT(GLenum mode, GLint first, GLsizei count, GLsizei instanceCount)
{
	TRACE("(GLenum mode = 0x%X, GLint first = %d, GLsizei count = %d, GLsizei instanceCount = %d)",
		mode, first, count, instanceCount);

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

	if(count < 0 || instanceCount < 0)
	{
		return error(GL_INVALID_VALUE);
	}

	es2::Context *context = es2::getContext();

	if(context)
	{
		es2::TransformFeedback* transformFeedback = context->getTransformFeedback();
		if(transformFeedback && transformFeedback->isActive() && (mode != transformFeedback->primitiveMode()))
		{
			return error(GL_INVALID_OPERATION);
		}

		context->drawArrays(mode, first, count, instanceCount);
	}
}

void DrawElementsInstancedEXT(GLenum mode, GLsizei count, GLenum type, const void *indices, GLsizei instanceCount)
{
	TRACE("(GLenum mode = 0x%X, GLsizei count = %d, GLenum type = 0x%X, const void *indices = %p, GLsizei instanceCount = %d)",
		mode, count, type, indices, instanceCount);

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

	if(count < 0 || instanceCount < 0)
	{
		return error(GL_INVALID_VALUE);
	}

	es2::Context *context = es2::getContext();

	if(context)
	{
		es2::TransformFeedback* transformFeedback = context->getTransformFeedback();
		if(transformFeedback && transformFeedback->isActive() && !transformFeedback->isPaused())
		{
			return error(GL_INVALID_OPERATION);
		}

		context->drawElements(mode, 0, MAX_ELEMENT_INDEX, count, type, indices, instanceCount);
	}
}

void VertexAttribDivisorEXT(GLuint index, GLuint divisor)
{
	TRACE("(GLuint index = %d, GLuint divisor = %d)", index, divisor);

	es2::Context *context = es2::getContext();

	if(context)
	{
		if(index >= es2::MAX_VERTEX_ATTRIBS)
		{
			return error(GL_INVALID_VALUE);
		}

		context->setVertexAttribDivisor(index, divisor);
	}
}

void DrawArraysInstancedANGLE(GLenum mode, GLint first, GLsizei count, GLsizei instanceCount)
{
	TRACE("(GLenum mode = 0x%X, GLint first = %d, GLsizei count = %d, GLsizei instanceCount = %d)",
		mode, first, count, instanceCount);

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

	if(count < 0 || instanceCount < 0)
	{
		return error(GL_INVALID_VALUE);
	}

	es2::Context *context = es2::getContext();

	if(context)
	{
		if(!context->hasZeroDivisor())
		{
			return error(GL_INVALID_OPERATION);
		}

		es2::TransformFeedback* transformFeedback = context->getTransformFeedback();
		if(transformFeedback && transformFeedback->isActive() && (mode != transformFeedback->primitiveMode()))
		{
			return error(GL_INVALID_OPERATION);
		}

		context->drawArrays(mode, first, count, instanceCount);
	}
}

void DrawElementsInstancedANGLE(GLenum mode, GLsizei count, GLenum type, const void *indices, GLsizei instanceCount)
{
	TRACE("(GLenum mode = 0x%X, GLsizei count = %d, GLenum type = 0x%X, const void *indices = %p, GLsizei instanceCount = %d)",
		mode, count, type, indices, instanceCount);

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

	if(count < 0 || instanceCount < 0)
	{
		return error(GL_INVALID_VALUE);
	}

	es2::Context *context = es2::getContext();

	if(context)
	{
		if(!context->hasZeroDivisor())
		{
			return error(GL_INVALID_OPERATION);
		}

		es2::TransformFeedback* transformFeedback = context->getTransformFeedback();
		if(transformFeedback && transformFeedback->isActive() && !transformFeedback->isPaused())
		{
			return error(GL_INVALID_OPERATION);
		}

		context->drawElements(mode, 0, MAX_ELEMENT_INDEX, count, type, indices, instanceCount);
	}
}

void VertexAttribDivisorANGLE(GLuint index, GLuint divisor)
{
	TRACE("(GLuint index = %d, GLuint divisor = %d)", index, divisor);

	es2::Context *context = es2::getContext();

	if(context)
	{
		if(index >= MAX_VERTEX_ATTRIBS)
		{
			return error(GL_INVALID_VALUE);
		}

		context->setVertexAttribDivisor(index, divisor);
	}
}

void Enable(GLenum cap)
{
	TRACE("(GLenum cap = 0x%X)", cap);

	es2::Context *context = es2::getContext();

	if(context)
	{
		switch(cap)
		{
		case GL_CULL_FACE:                     context->setCullFaceEnabled(true);                   break;
		case GL_POLYGON_OFFSET_FILL:           context->setPolygonOffsetFillEnabled(true);          break;
		case GL_SAMPLE_ALPHA_TO_COVERAGE:      context->setSampleAlphaToCoverageEnabled(true);      break;
		case GL_SAMPLE_COVERAGE:               context->setSampleCoverageEnabled(true);             break;
		case GL_SCISSOR_TEST:                  context->setScissorTestEnabled(true);                break;
		case GL_STENCIL_TEST:                  context->setStencilTestEnabled(true);                break;
		case GL_DEPTH_TEST:                    context->setDepthTestEnabled(true);                  break;
		case GL_BLEND:                         context->setBlendEnabled(true);                      break;
		case GL_DITHER:                        context->setDitherEnabled(true);                     break;
		case GL_PRIMITIVE_RESTART_FIXED_INDEX: context->setPrimitiveRestartFixedIndexEnabled(true); break;
		case GL_RASTERIZER_DISCARD:            context->setRasterizerDiscardEnabled(true);          break;
		default:
			return error(GL_INVALID_ENUM);
		}
	}
}

void EnableVertexAttribArray(GLuint index)
{
	TRACE("(GLuint index = %d)", index);

	if(index >= es2::MAX_VERTEX_ATTRIBS)
	{
		return error(GL_INVALID_VALUE);
	}

	es2::Context *context = es2::getContext();

	if(context)
	{
		context->setVertexAttribArrayEnabled(index, true);
	}
}

void EndQueryEXT(GLenum target)
{
	TRACE("GLenum target = 0x%X)", target);

	switch(target)
	{
	case GL_ANY_SAMPLES_PASSED_EXT:
	case GL_ANY_SAMPLES_PASSED_CONSERVATIVE_EXT:
		break;
	default:
		return error(GL_INVALID_ENUM);
	}

	es2::Context *context = es2::getContext();

	if(context)
	{
		context->endQuery(target);
	}
}

void FinishFenceNV(GLuint fence)
{
	TRACE("(GLuint fence = %d)", fence);

	es2::Context *context = es2::getContext();

	if(context)
	{
		es2::Fence* fenceObject = context->getFence(fence);

		if(fenceObject == NULL)
		{
			return error(GL_INVALID_OPERATION);
		}

		fenceObject->finishFence();
	}
}

void Finish(void)
{
	TRACE("()");

	es2::Context *context = es2::getContext();

	if(context)
	{
		context->finish();
	}
}

void Flush(void)
{
	TRACE("()");

	es2::Context *context = es2::getContext();

	if(context)
	{
		context->flush();
	}
}

void FramebufferRenderbuffer(GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer)
{
	TRACE("(GLenum target = 0x%X, GLenum attachment = 0x%X, GLenum renderbuffertarget = 0x%X, "
	      "GLuint renderbuffer = %d)", target, attachment, renderbuffertarget, renderbuffer);

	if((target != GL_FRAMEBUFFER && target != GL_DRAW_FRAMEBUFFER_ANGLE && target != GL_READ_FRAMEBUFFER_ANGLE) ||
	   (renderbuffertarget != GL_RENDERBUFFER && renderbuffer != 0))
	{
		return error(GL_INVALID_ENUM);
	}

	es2::Context *context = es2::getContext();

	if(context)
	{
		es2::Framebuffer *framebuffer = NULL;
		GLuint framebufferName = 0;
		if(target == GL_READ_FRAMEBUFFER_ANGLE)
		{
			framebuffer = context->getReadFramebuffer();
			framebufferName = context->getReadFramebufferName();
		}
		else
		{
			framebuffer = context->getDrawFramebuffer();
			framebufferName = context->getDrawFramebufferName();
		}

		if(!framebuffer || (framebufferName == 0 && renderbuffer != 0))
		{
			return error(GL_INVALID_OPERATION);
		}

		// [OpenGL ES 2.0.25] Section 4.4.3 page 112
		// [OpenGL ES 3.0.2] Section 4.4.2 page 201
		// 'renderbuffer' must be either zero or the name of an existing renderbuffer object of
		// type 'renderbuffertarget', otherwise an INVALID_OPERATION error is generated.
		if(renderbuffer != 0)
		{
			if(!context->getRenderbuffer(renderbuffer))
			{
				return error(GL_INVALID_OPERATION);
			}
		}

		GLint clientVersion = context->getClientVersion();

		switch(attachment)
		{
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
			if(clientVersion < 3)
			{
				return error(GL_INVALID_ENUM);
			}
			// fall through
		case GL_COLOR_ATTACHMENT0:
			if((attachment - GL_COLOR_ATTACHMENT0) >= es2::IMPLEMENTATION_MAX_COLOR_ATTACHMENTS)
			{
				return error(GL_INVALID_ENUM);
			}
			framebuffer->setColorbuffer(GL_RENDERBUFFER, renderbuffer, attachment - GL_COLOR_ATTACHMENT0);
			break;
		case GL_DEPTH_ATTACHMENT:
			framebuffer->setDepthbuffer(GL_RENDERBUFFER, renderbuffer);
			break;
		case GL_STENCIL_ATTACHMENT:
			framebuffer->setStencilbuffer(GL_RENDERBUFFER, renderbuffer);
			break;
		case GL_DEPTH_STENCIL_ATTACHMENT:
			if(clientVersion >= 3)
			{
				framebuffer->setDepthbuffer(GL_RENDERBUFFER, renderbuffer);
				framebuffer->setStencilbuffer(GL_RENDERBUFFER, renderbuffer);
				break;
			}
			else return error(GL_INVALID_ENUM);
		default:
			return error(GL_INVALID_ENUM);
		}
	}
}

void FramebufferTexture2D(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level)
{
	TRACE("(GLenum target = 0x%X, GLenum attachment = 0x%X, GLenum textarget = 0x%X, "
	      "GLuint texture = %d, GLint level = %d)", target, attachment, textarget, texture, level);

	if(target != GL_FRAMEBUFFER && target != GL_DRAW_FRAMEBUFFER_ANGLE && target != GL_READ_FRAMEBUFFER_ANGLE)
	{
		return error(GL_INVALID_ENUM);
	}

	es2::Context *context = es2::getContext();

	if(context)
	{
		if(texture == 0)
		{
			textarget = GL_NONE;
		}
		else
		{
			es2::Texture *tex = context->getTexture(texture);

			if(tex == NULL)
			{
				return error(GL_INVALID_OPERATION);
			}

			switch(textarget)
			{
			case GL_TEXTURE_2D:
				if(tex->getTarget() != GL_TEXTURE_2D)
				{
					return error(GL_INVALID_OPERATION);
				}
				break;
			case GL_TEXTURE_CUBE_MAP_POSITIVE_X:
			case GL_TEXTURE_CUBE_MAP_NEGATIVE_X:
			case GL_TEXTURE_CUBE_MAP_POSITIVE_Y:
			case GL_TEXTURE_CUBE_MAP_NEGATIVE_Y:
			case GL_TEXTURE_CUBE_MAP_POSITIVE_Z:
			case GL_TEXTURE_CUBE_MAP_NEGATIVE_Z:
				if(tex->getTarget() != GL_TEXTURE_CUBE_MAP)
				{
					return error(GL_INVALID_OPERATION);
				}
				break;
			default:
				return error(GL_INVALID_ENUM);
			}

			if(tex->isCompressed(textarget, level))
			{
				return error(GL_INVALID_OPERATION);
			}

			if((level != 0) && (context->getClientVersion() < 3))
			{
				return error(GL_INVALID_VALUE);
			}

			if((level < 0) || (level >= es2::IMPLEMENTATION_MAX_TEXTURE_LEVELS))
			{
				return error(GL_INVALID_VALUE);
			}
		}

		es2::Framebuffer *framebuffer = NULL;
		GLuint framebufferName = 0;
		if(target == GL_READ_FRAMEBUFFER_ANGLE)
		{
			framebuffer = context->getReadFramebuffer();
			framebufferName = context->getReadFramebufferName();
		}
		else
		{
			framebuffer = context->getDrawFramebuffer();
			framebufferName = context->getDrawFramebufferName();
		}

		if(framebufferName == 0 || !framebuffer)
		{
			return error(GL_INVALID_OPERATION);
		}

		GLint clientVersion = context->getClientVersion();

		switch(attachment)
		{
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
			if(clientVersion < 3)
			{
				return error(GL_INVALID_ENUM);
			}
			// fall through
		case GL_COLOR_ATTACHMENT0:
			if((attachment - GL_COLOR_ATTACHMENT0) >= es2::IMPLEMENTATION_MAX_COLOR_ATTACHMENTS)
			{
				return error(GL_INVALID_ENUM);
			}
			framebuffer->setColorbuffer(textarget, texture, attachment - GL_COLOR_ATTACHMENT0, level);
			break;
		case GL_DEPTH_ATTACHMENT:   framebuffer->setDepthbuffer(textarget, texture, level);   break;
		case GL_STENCIL_ATTACHMENT: framebuffer->setStencilbuffer(textarget, texture, level); break;
		default:
			return error(GL_INVALID_ENUM);
		}
	}
}

void FrontFace(GLenum mode)
{
	TRACE("(GLenum mode = 0x%X)", mode);

	switch(mode)
	{
	case GL_CW:
	case GL_CCW:
		{
			es2::Context *context = es2::getContext();

			if(context)
			{
				context->setFrontFace(mode);
			}
		}
		break;
	default:
		return error(GL_INVALID_ENUM);
	}
}

void GenBuffers(GLsizei n, GLuint* buffers)
{
	TRACE("(GLsizei n = %d, GLuint* buffers = %p)", n, buffers);

	if(n < 0)
	{
		return error(GL_INVALID_VALUE);
	}

	es2::Context *context = es2::getContext();

	if(context)
	{
		for(int i = 0; i < n; i++)
		{
			buffers[i] = context->createBuffer();
		}
	}
}

void GenerateMipmap(GLenum target)
{
	TRACE("(GLenum target = 0x%X)", target);

	es2::Context *context = es2::getContext();

	if(context)
	{
		es2::Texture *texture = nullptr;

		GLint clientVersion = context->getClientVersion();

		switch(target)
		{
		case GL_TEXTURE_2D:
			texture = context->getTexture2D();
			break;
		case GL_TEXTURE_CUBE_MAP:
			texture = context->getTextureCubeMap();
			break;
		case GL_TEXTURE_2D_ARRAY:
			if(clientVersion < 3)
			{
				return error(GL_INVALID_ENUM);
			}
			else
			{
				texture = context->getTexture2DArray();
			}
			break;
		case GL_TEXTURE_3D_OES:
			texture = context->getTexture3D();
			break;
		default:
			return error(GL_INVALID_ENUM);
		}

		if(texture->isCompressed(target, 0) || texture->isDepth(target, 0))
		{
			return error(GL_INVALID_OPERATION);
		}

		texture->generateMipmaps();
	}
}

void GenFencesNV(GLsizei n, GLuint* fences)
{
	TRACE("(GLsizei n = %d, GLuint* fences = %p)", n, fences);

	if(n < 0)
	{
		return error(GL_INVALID_VALUE);
	}

	es2::Context *context = es2::getContext();

	if(context)
	{
		for(int i = 0; i < n; i++)
		{
			fences[i] = context->createFence();
		}
	}
}

void GenFramebuffers(GLsizei n, GLuint* framebuffers)
{
	TRACE("(GLsizei n = %d, GLuint* framebuffers = %p)", n, framebuffers);

	if(n < 0)
	{
		return error(GL_INVALID_VALUE);
	}

	es2::Context *context = es2::getContext();

	if(context)
	{
		for(int i = 0; i < n; i++)
		{
			framebuffers[i] = context->createFramebuffer();
		}
	}
}

void GenQueriesEXT(GLsizei n, GLuint* ids)
{
	TRACE("(GLsizei n = %d, GLuint* ids = %p)", n, ids);

	if(n < 0)
	{
		return error(GL_INVALID_VALUE);
	}

	es2::Context *context = es2::getContext();

	if(context)
	{
		for(int i = 0; i < n; i++)
		{
			ids[i] = context->createQuery();
		}
	}
}

void GenRenderbuffers(GLsizei n, GLuint* renderbuffers)
{
	TRACE("(GLsizei n = %d, GLuint* renderbuffers = %p)", n, renderbuffers);

	if(n < 0)
	{
		return error(GL_INVALID_VALUE);
	}

	es2::Context *context = es2::getContext();

	if(context)
	{
		for(int i = 0; i < n; i++)
		{
			renderbuffers[i] = context->createRenderbuffer();
		}
	}
}

void GenTextures(GLsizei n, GLuint* textures)
{
	TRACE("(GLsizei n = %d, GLuint* textures = %p)", n, textures);

	if(n < 0)
	{
		return error(GL_INVALID_VALUE);
	}

	es2::Context *context = es2::getContext();

	if(context)
	{
		for(int i = 0; i < n; i++)
		{
			textures[i] = context->createTexture();
		}
	}
}

void GetActiveAttrib(GLuint program, GLuint index, GLsizei bufsize, GLsizei *length, GLint *size, GLenum *type, GLchar *name)
{
	TRACE("(GLuint program = %d, GLuint index = %d, GLsizei bufsize = %d, GLsizei *length = %p, "
	      "GLint *size = %p, GLenum *type = %p, GLchar *name = %p)",
	      program, index, bufsize, length, size, type, name);

	if(bufsize < 0)
	{
		return error(GL_INVALID_VALUE);
	}

	es2::Context *context = es2::getContext();

	if(context)
	{
		es2::Program *programObject = context->getProgram(program);

		if(!programObject)
		{
			if(context->getShader(program))
			{
				return error(GL_INVALID_OPERATION);
			}
			else
			{
				return error(GL_INVALID_VALUE);
			}
		}

		if(index >= programObject->getActiveAttributeCount())
		{
			return error(GL_INVALID_VALUE);
		}

		programObject->getActiveAttribute(index, bufsize, length, size, type, name);
	}
}

void GetActiveUniform(GLuint program, GLuint index, GLsizei bufsize, GLsizei* length, GLint* size, GLenum* type, GLchar* name)
{
	TRACE("(GLuint program = %d, GLuint index = %d, GLsizei bufsize = %d, "
	      "GLsizei* length = %p, GLint* size = %p, GLenum* type = %p, GLchar* name = %s)",
	      program, index, bufsize, length, size, type, name);

	if(bufsize < 0)
	{
		return error(GL_INVALID_VALUE);
	}

	es2::Context *context = es2::getContext();

	if(context)
	{
		es2::Program *programObject = context->getProgram(program);

		if(!programObject)
		{
			if(context->getShader(program))
			{
				return error(GL_INVALID_OPERATION);
			}
			else
			{
				return error(GL_INVALID_VALUE);
			}
		}

		if(index >= programObject->getActiveUniformCount())
		{
			return error(GL_INVALID_VALUE);
		}

		programObject->getActiveUniform(index, bufsize, length, size, type, name);
	}
}

void GetAttachedShaders(GLuint program, GLsizei maxcount, GLsizei* count, GLuint* shaders)
{
	TRACE("(GLuint program = %d, GLsizei maxcount = %d, GLsizei* count = %p, GLuint* shaders = %p)",
	      program, maxcount, count, shaders);

	if(maxcount < 0)
	{
		return error(GL_INVALID_VALUE);
	}

	es2::Context *context = es2::getContext();

	if(context)
	{
		es2::Program *programObject = context->getProgram(program);

		if(!programObject)
		{
			if(context->getShader(program))
			{
				return error(GL_INVALID_OPERATION);
			}
			else
			{
				return error(GL_INVALID_VALUE);
			}
		}

		return programObject->getAttachedShaders(maxcount, count, shaders);
	}
}

int GetAttribLocation(GLuint program, const GLchar* name)
{
	TRACE("(GLuint program = %d, const GLchar* name = %s)", program, name);

	es2::Context *context = es2::getContext();

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

		return programObject->getAttributeLocation(name);
	}

	return -1;
}

void GetBooleanv(GLenum pname, GLboolean* params)
{
	TRACE("(GLenum pname = 0x%X, GLboolean* params = %p)",  pname, params);

	es2::Context *context = es2::getContext();

	if(context)
	{
		if(!(context->getBooleanv(pname, params)))
		{
			GLenum nativeType;
			unsigned int numParams = 0;
			if(!context->getQueryParameterInfo(pname, &nativeType, &numParams))
				return error(GL_INVALID_ENUM);

			if(numParams == 0)
				return; // it is known that the pname is valid, but there are no parameters to return

			if(nativeType == GL_FLOAT)
			{
				GLfloat *floatParams = NULL;
				floatParams = new GLfloat[numParams];

				context->getFloatv(pname, floatParams);

				for(unsigned int i = 0; i < numParams; ++i)
				{
					if(floatParams[i] == 0.0f)
						params[i] = GL_FALSE;
					else
						params[i] = GL_TRUE;
				}

				delete [] floatParams;
			}
			else if(nativeType == GL_INT)
			{
				GLint *intParams = NULL;
				intParams = new GLint[numParams];

				context->getIntegerv(pname, intParams);

				for(unsigned int i = 0; i < numParams; ++i)
				{
					if(intParams[i] == 0)
						params[i] = GL_FALSE;
					else
						params[i] = GL_TRUE;
				}

				delete [] intParams;
			}
		}
	}
}

void GetBufferParameteriv(GLenum target, GLenum pname, GLint* params)
{
	TRACE("(GLenum target = 0x%X, GLenum pname = 0x%X, GLint* params = %p)", target, pname, params);

	es2::Context *context = es2::getContext();

	if(context)
	{
		es2::Buffer *buffer;
		if(!context->getBuffer(target, &buffer))
		{
			return error(GL_INVALID_ENUM);
		}

		if(!buffer)
		{
			// A null buffer means that "0" is bound to the requested buffer target
			return error(GL_INVALID_OPERATION);
		}

		GLint clientVersion = context->getClientVersion();

		switch(pname)
		{
		case GL_BUFFER_USAGE:
			*params = buffer->usage();
			break;
		case GL_BUFFER_SIZE:
			*params = buffer->size();
			break;
		case GL_BUFFER_ACCESS_FLAGS:
			if(clientVersion >= 3)
			{
				*params = buffer->access();
				break;
			}
			else return error(GL_INVALID_ENUM);
		case GL_BUFFER_MAPPED:
			if(clientVersion >= 3)
			{
				*params = buffer->isMapped();
				break;
			}
			else return error(GL_INVALID_ENUM);
		case GL_BUFFER_MAP_LENGTH:
			if(clientVersion >= 3)
			{
				*params = buffer->length();
				break;
			}
			else return error(GL_INVALID_ENUM);
		case GL_BUFFER_MAP_OFFSET:
			if(clientVersion >= 3)
			{
				*params = buffer->offset();
				break;
			}
			else return error(GL_INVALID_ENUM);
		default:
			return error(GL_INVALID_ENUM);
		}
	}
}

GLenum GetError(void)
{
	TRACE("()");

	es2::Context *context = es2::getContext();

	if(context)
	{
		return context->getError();
	}

	return GL_NO_ERROR;
}

void GetFenceivNV(GLuint fence, GLenum pname, GLint *params)
{
	TRACE("(GLuint fence = %d, GLenum pname = 0x%X, GLint *params = %p)", fence, pname, params);

	es2::Context *context = es2::getContext();

	if(context)
	{
		es2::Fence *fenceObject = context->getFence(fence);

		if(fenceObject == NULL)
		{
			return error(GL_INVALID_OPERATION);
		}

		fenceObject->getFenceiv(pname, params);
	}
}

void GetFloatv(GLenum pname, GLfloat* params)
{
	TRACE("(GLenum pname = 0x%X, GLfloat* params = %p)", pname, params);

	es2::Context *context = es2::getContext();

	if(context)
	{
		if(!(context->getFloatv(pname, params)))
		{
			GLenum nativeType;
			unsigned int numParams = 0;
			if(!context->getQueryParameterInfo(pname, &nativeType, &numParams))
				return error(GL_INVALID_ENUM);

			if(numParams == 0)
				return; // it is known that the pname is valid, but that there are no parameters to return.

			if(nativeType == GL_BOOL)
			{
				GLboolean *boolParams = NULL;
				boolParams = new GLboolean[numParams];

				context->getBooleanv(pname, boolParams);

				for(unsigned int i = 0; i < numParams; ++i)
				{
					if(boolParams[i] == GL_FALSE)
						params[i] = 0.0f;
					else
						params[i] = 1.0f;
				}

				delete [] boolParams;
			}
			else if(nativeType == GL_INT)
			{
				GLint *intParams = NULL;
				intParams = new GLint[numParams];

				context->getIntegerv(pname, intParams);

				for(unsigned int i = 0; i < numParams; ++i)
				{
					params[i] = (GLfloat)intParams[i];
				}

				delete [] intParams;
			}
		}
	}
}

void GetFramebufferAttachmentParameteriv(GLenum target, GLenum attachment, GLenum pname, GLint* params)
{
	TRACE("(GLenum target = 0x%X, GLenum attachment = 0x%X, GLenum pname = 0x%X, GLint* params = %p)",
	      target, attachment, pname, params);

	es2::Context *context = es2::getContext();

	if(context)
	{
		if(target != GL_FRAMEBUFFER && target != GL_DRAW_FRAMEBUFFER_ANGLE && target != GL_READ_FRAMEBUFFER_ANGLE)
		{
			return error(GL_INVALID_ENUM);
		}

		GLint clientVersion = context->getClientVersion();

		es2::Framebuffer *framebuffer = NULL;
		if(target == GL_READ_FRAMEBUFFER_ANGLE)
		{
			if(context->getReadFramebufferName() == 0)
			{
				switch(attachment)
				{
				case GL_BACK:
				case GL_DEPTH:
				case GL_STENCIL:
					if(clientVersion >= 3)
					{
						break;
					}
					// fall through
				default:
					return error(GL_INVALID_ENUM);
				}
			}

			framebuffer = context->getReadFramebuffer();
		}
		else
		{
			if(context->getDrawFramebufferName() == 0)
			{
				switch(attachment)
				{
				case GL_BACK:
				case GL_DEPTH:
				case GL_STENCIL:
					if(clientVersion >= 3)
					{
						break;
					}
					// fall through
				default:
					return error(GL_INVALID_ENUM);
				}
			}

			framebuffer = context->getDrawFramebuffer();
		}

		GLenum attachmentType;
		GLuint attachmentHandle;
		GLint attachmentLayer;
		Renderbuffer* renderbuffer = nullptr;
		switch(attachment)
		{
		case GL_BACK:
			if(clientVersion >= 3)
			{
				attachmentType = framebuffer->getColorbufferType(0);
				attachmentHandle = framebuffer->getColorbufferName(0);
				attachmentLayer = framebuffer->getColorbufferLayer(0);
				renderbuffer = framebuffer->getColorbuffer(0);
			}
			else return error(GL_INVALID_ENUM);
			break;
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
			if(clientVersion < 3)
			{
				return error(GL_INVALID_ENUM);
			}
			// fall through
		case GL_COLOR_ATTACHMENT0:
			if((attachment - GL_COLOR_ATTACHMENT0) >= es2::IMPLEMENTATION_MAX_COLOR_ATTACHMENTS)
			{
				return error(GL_INVALID_ENUM);
			}
			attachmentType = framebuffer->getColorbufferType(attachment - GL_COLOR_ATTACHMENT0);
			attachmentHandle = framebuffer->getColorbufferName(attachment - GL_COLOR_ATTACHMENT0);
			attachmentLayer = framebuffer->getColorbufferLayer(attachment - GL_COLOR_ATTACHMENT0);
			renderbuffer = framebuffer->getColorbuffer(attachment - GL_COLOR_ATTACHMENT0);
			break;
		case GL_DEPTH:
			if(clientVersion < 3)
			{
				return error(GL_INVALID_ENUM);
			}
			// fall through
		case GL_DEPTH_ATTACHMENT:
			attachmentType = framebuffer->getDepthbufferType();
			attachmentHandle = framebuffer->getDepthbufferName();
			attachmentLayer = framebuffer->getDepthbufferLayer();
			renderbuffer = framebuffer->getDepthbuffer();
			break;
		case GL_STENCIL:
			if(clientVersion < 3)
			{
				return error(GL_INVALID_ENUM);
			}
			// fall through
		case GL_STENCIL_ATTACHMENT:
			attachmentType = framebuffer->getStencilbufferType();
			attachmentHandle = framebuffer->getStencilbufferName();
			attachmentLayer = framebuffer->getStencilbufferLayer();
			renderbuffer = framebuffer->getStencilbuffer();
			break;
		case GL_DEPTH_STENCIL_ATTACHMENT:
			if(clientVersion >= 3)
			{
				attachmentType = framebuffer->getDepthbufferType();
				attachmentHandle = framebuffer->getDepthbufferName();
				attachmentLayer = framebuffer->getDepthbufferLayer();
				if(attachmentHandle != framebuffer->getStencilbufferName())
				{
					// Different attachments to DEPTH and STENCIL, query fails
					return error(GL_INVALID_OPERATION);
				}
				renderbuffer = framebuffer->getDepthbuffer();
			}
			else return error(GL_INVALID_ENUM);
			break;
		default:
			return error(GL_INVALID_ENUM);
		}

		GLenum attachmentObjectType = GL_NONE;   // Type category
		if(attachmentType == GL_NONE || attachmentType == GL_RENDERBUFFER)
		{
			attachmentObjectType = attachmentType;
		}
		else if(es2::IsTextureTarget(attachmentType))
		{
			attachmentObjectType = GL_TEXTURE;
		}
		else UNREACHABLE(attachmentType);

		if(attachmentObjectType != GL_NONE)
		{
			switch(pname)
			{
			case GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE:
				*params = attachmentObjectType;
				break;
			case GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME:
				if(attachmentObjectType == GL_RENDERBUFFER || attachmentObjectType == GL_TEXTURE)
				{
					*params = attachmentHandle;
				}
				else
				{
					return error(GL_INVALID_ENUM);
				}
				break;
			case GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LEVEL:
				if(attachmentObjectType == GL_TEXTURE)
				{
					*params = 0; // FramebufferTexture2D will not allow level to be set to anything else in GL ES 2.0
				}
				else
				{
					return error(GL_INVALID_ENUM);
				}
				break;
			case GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_CUBE_MAP_FACE:
				if(attachmentObjectType == GL_TEXTURE)
				{
					if(es2::IsCubemapTextureTarget(attachmentType))
					{
						*params = attachmentType;
					}
					else
					{
						*params = 0;
					}
				}
				else
				{
					return error(GL_INVALID_ENUM);
				}
				break;
			case GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LAYER:
				if(clientVersion >= 3)
				{
					*params = attachmentLayer;
				}
				else return error(GL_INVALID_ENUM);
				break;
			case GL_FRAMEBUFFER_ATTACHMENT_RED_SIZE:
				if(clientVersion >= 3)
				{
					*params = renderbuffer->getRedSize();
				}
				else return error(GL_INVALID_ENUM);
				break;
			case GL_FRAMEBUFFER_ATTACHMENT_GREEN_SIZE:
				if(clientVersion >= 3)
				{
					*params = renderbuffer->getGreenSize();
				}
				else return error(GL_INVALID_ENUM);
				break;
			case GL_FRAMEBUFFER_ATTACHMENT_BLUE_SIZE:
				if(clientVersion >= 3)
				{
					*params = renderbuffer->getBlueSize();
				}
				else return error(GL_INVALID_ENUM);
				break;
			case GL_FRAMEBUFFER_ATTACHMENT_ALPHA_SIZE:
				if(clientVersion >= 3)
				{
					*params = renderbuffer->getAlphaSize();
				}
				else return error(GL_INVALID_ENUM);
				break;
			case GL_FRAMEBUFFER_ATTACHMENT_DEPTH_SIZE:
				if(clientVersion >= 3)
				{
					*params = renderbuffer->getDepthSize();
				}
				else return error(GL_INVALID_ENUM);
				break;
			case GL_FRAMEBUFFER_ATTACHMENT_STENCIL_SIZE:
				if(clientVersion >= 3)
				{
					*params = renderbuffer->getStencilSize();
				}
				else return error(GL_INVALID_ENUM);
				break;
			case GL_FRAMEBUFFER_ATTACHMENT_COMPONENT_TYPE:
				if(clientVersion >= 3)
				{
					if(attachment == GL_DEPTH_STENCIL_ATTACHMENT)
					{
						return error(GL_INVALID_OPERATION);
					}

					*params = sw2es::GetComponentType(renderbuffer->getInternalFormat(), attachment);
				}
				else return error(GL_INVALID_ENUM);
				break;
			case GL_FRAMEBUFFER_ATTACHMENT_COLOR_ENCODING:
				if(clientVersion >= 3)
				{
					*params = GL_LINEAR; // FIXME: GL_SRGB will also be possible, when sRGB is added
				}
				else return error(GL_INVALID_ENUM);
				break;
			default:
				return error(GL_INVALID_ENUM);
			}
		}
		else
		{
			// ES 2.0.25 spec pg 127 states that if the value of FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE
			// is NONE, then querying any other pname will generate INVALID_ENUM.

			// ES 3.0.2 spec pg 235 states that if the attachment type is none,
			// GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME will return zero and be an
			// INVALID_OPERATION for all other pnames

			switch(pname)
			{
			case GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE:
				*params = GL_NONE;
				break;

			case GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME:
				if(clientVersion < 3)
				{
					return error(GL_INVALID_ENUM);
				}
				*params = 0;
				break;

			default:
				if(clientVersion < 3)
				{
					return error(GL_INVALID_ENUM);
				}
				else
				{
					return error(GL_INVALID_OPERATION);
				}
			}
		}
	}
}

GLenum GetGraphicsResetStatusEXT(void)
{
	TRACE("()");

	return GL_NO_ERROR;
}

void GetIntegerv(GLenum pname, GLint* params)
{
	TRACE("(GLenum pname = 0x%X, GLint* params = %p)", pname, params);

	es2::Context *context = es2::getContext();

	if(!context)
	{
		// Not strictly an error, but probably unintended or attempting to rely on non-compliant behavior
		#ifdef __ANDROID__
			ALOGI("expected_badness glGetIntegerv() called without current context.");
		#else
			ERR("glGetIntegerv() called without current context.");
		#endif

		// This is not spec compliant! When there is no current GL context, functions should
		// have no side effects. Google Maps queries these values before creating a context,
		// so we need this as a bug-compatible workaround.
		switch(pname)
		{
		case GL_MAX_TEXTURE_SIZE:                 *params = es2::IMPLEMENTATION_MAX_TEXTURE_SIZE;  return;
		case GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS:   *params = es2::MAX_VERTEX_TEXTURE_IMAGE_UNITS;   return;
		case GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS: *params = es2::MAX_COMBINED_TEXTURE_IMAGE_UNITS; return;
		case GL_STENCIL_BITS:                     *params = 8;                                     return;
		case GL_ALIASED_LINE_WIDTH_RANGE:
			params[0] = (GLint)es2::ALIASED_LINE_WIDTH_RANGE_MIN;
			params[1] = (GLint)es2::ALIASED_LINE_WIDTH_RANGE_MAX;
			return;
		}
	}

	if(context)
	{
		if(!(context->getIntegerv(pname, params)))
		{
			GLenum nativeType;
			unsigned int numParams = 0;
			if(!context->getQueryParameterInfo(pname, &nativeType, &numParams))
				return error(GL_INVALID_ENUM);

			if(numParams == 0)
				return; // it is known that pname is valid, but there are no parameters to return

			if(nativeType == GL_BOOL)
			{
				GLboolean *boolParams = NULL;
				boolParams = new GLboolean[numParams];

				context->getBooleanv(pname, boolParams);

				for(unsigned int i = 0; i < numParams; ++i)
				{
					params[i] = (boolParams[i] == GL_FALSE) ? 0 : 1;
				}

				delete [] boolParams;
			}
			else if(nativeType == GL_FLOAT)
			{
				GLfloat *floatParams = NULL;
				floatParams = new GLfloat[numParams];

				context->getFloatv(pname, floatParams);

				for(unsigned int i = 0; i < numParams; ++i)
				{
					if(pname == GL_DEPTH_RANGE || pname == GL_COLOR_CLEAR_VALUE || pname == GL_DEPTH_CLEAR_VALUE || pname == GL_BLEND_COLOR)
					{
						params[i] = es2::floatToInt(floatParams[i]);
					}
					else
					{
						params[i] = (GLint)(floatParams[i] > 0.0f ? floor(floatParams[i] + 0.5) : ceil(floatParams[i] - 0.5));
					}
				}

				delete [] floatParams;
			}
		}
	}
}

void GetProgramiv(GLuint program, GLenum pname, GLint* params)
{
	TRACE("(GLuint program = %d, GLenum pname = 0x%X, GLint* params = %p)", program, pname, params);

	es2::Context *context = es2::getContext();

	if(context)
	{
		es2::Program *programObject = context->getProgram(program);

		if(!programObject)
		{
			return error(GL_INVALID_VALUE);
		}

		GLint clientVersion = egl::getClientVersion();

		switch(pname)
		{
		case GL_DELETE_STATUS:
			*params = programObject->isFlaggedForDeletion();
			return;
		case GL_LINK_STATUS:
			*params = programObject->isLinked();
			return;
		case GL_VALIDATE_STATUS:
			*params = programObject->isValidated();
			return;
		case GL_INFO_LOG_LENGTH:
			*params = programObject->getInfoLogLength();
			return;
		case GL_ATTACHED_SHADERS:
			*params = programObject->getAttachedShadersCount();
			return;
		case GL_ACTIVE_ATTRIBUTES:
			*params = programObject->getActiveAttributeCount();
			return;
		case GL_ACTIVE_ATTRIBUTE_MAX_LENGTH:
			*params = programObject->getActiveAttributeMaxLength();
			return;
		case GL_ACTIVE_UNIFORMS:
			*params = programObject->getActiveUniformCount();
			return;
		case GL_ACTIVE_UNIFORM_MAX_LENGTH:
			*params = programObject->getActiveUniformMaxLength();
			return;
		case GL_ACTIVE_UNIFORM_BLOCKS:
			if(clientVersion >= 3)
			{
				*params = programObject->getActiveUniformBlockCount();
				return;
			}
			else return error(GL_INVALID_ENUM);
		case GL_ACTIVE_UNIFORM_BLOCK_MAX_NAME_LENGTH:
			if(clientVersion >= 3)
			{
				*params = programObject->getActiveUniformBlockMaxLength();
				return;
			}
			else return error(GL_INVALID_ENUM);
		case GL_TRANSFORM_FEEDBACK_BUFFER_MODE:
			if(clientVersion >= 3)
			{
				*params = programObject->getTransformFeedbackBufferMode();
				return;
			}
			else return error(GL_INVALID_ENUM);
		case GL_TRANSFORM_FEEDBACK_VARYINGS:
			if(clientVersion >= 3)
			{
				*params = programObject->getTransformFeedbackVaryingCount();
				return;
			}
			else return error(GL_INVALID_ENUM);
		case GL_TRANSFORM_FEEDBACK_VARYING_MAX_LENGTH:
			if(clientVersion >= 3)
			{
				*params = programObject->getTransformFeedbackVaryingMaxLength();
				return;
			}
			else return error(GL_INVALID_ENUM);
		case GL_PROGRAM_BINARY_RETRIEVABLE_HINT:
			if(clientVersion >= 3)
			{
				*params = programObject->getBinaryRetrievableHint();
				return;
			}
			else return error(GL_INVALID_ENUM);
		case GL_PROGRAM_BINARY_LENGTH:
			if(clientVersion >= 3)
			{
				*params = programObject->getBinaryLength();
				return;
			}
			else return error(GL_INVALID_ENUM);
		default:
			return error(GL_INVALID_ENUM);
		}
	}
}

void GetProgramInfoLog(GLuint program, GLsizei bufsize, GLsizei* length, GLchar* infolog)
{
	TRACE("(GLuint program = %d, GLsizei bufsize = %d, GLsizei* length = %p, GLchar* infolog = %p)",
	      program, bufsize, length, infolog);

	if(bufsize < 0)
	{
		return error(GL_INVALID_VALUE);
	}

	es2::Context *context = es2::getContext();

	if(context)
	{
		es2::Program *programObject = context->getProgram(program);

		if(!programObject)
		{
			return error(GL_INVALID_VALUE);
		}

		programObject->getInfoLog(bufsize, length, infolog);
	}
}

void GetQueryivEXT(GLenum target, GLenum pname, GLint *params)
{
	TRACE("GLenum target = 0x%X, GLenum pname = 0x%X, GLint *params = %p)", target, pname, params);

	switch(pname)
	{
	case GL_CURRENT_QUERY_EXT:
		break;
	default:
		return error(GL_INVALID_ENUM);
	}

	es2::Context *context = es2::getContext();

	if(context)
	{
		params[0] = context->getActiveQuery(target);
	}
}

void GetQueryObjectuivEXT(GLuint name, GLenum pname, GLuint *params)
{
	TRACE("(GLuint name = %d, GLenum pname = 0x%X, GLuint *params = %p)", name, pname, params);

	switch(pname)
	{
	case GL_QUERY_RESULT_EXT:
	case GL_QUERY_RESULT_AVAILABLE_EXT:
		break;
	default:
		return error(GL_INVALID_ENUM);
	}

	es2::Context *context = es2::getContext();

	if(context)
	{
		es2::Query *queryObject = context->getQuery(name);

		if(!queryObject)
		{
			return error(GL_INVALID_OPERATION);
		}

		if(context->getActiveQuery(queryObject->getType()) == name)
		{
			return error(GL_INVALID_OPERATION);
		}

		switch(pname)
		{
		case GL_QUERY_RESULT_EXT:
			params[0] = queryObject->getResult();
			break;
		case GL_QUERY_RESULT_AVAILABLE_EXT:
			params[0] = queryObject->isResultAvailable();
			break;
		default:
			ASSERT(false);
		}
	}
}

void GetRenderbufferParameteriv(GLenum target, GLenum pname, GLint* params)
{
	TRACE("(GLenum target = 0x%X, GLenum pname = 0x%X, GLint* params = %p)", target, pname, params);

	es2::Context *context = es2::getContext();

	if(context)
	{
		if(target != GL_RENDERBUFFER)
		{
			return error(GL_INVALID_ENUM);
		}

		if(context->getRenderbufferName() == 0)
		{
			return error(GL_INVALID_OPERATION);
		}

		es2::Renderbuffer *renderbuffer = context->getRenderbuffer(context->getRenderbufferName());

		switch(pname)
		{
		case GL_RENDERBUFFER_WIDTH:           *params = renderbuffer->getWidth();       break;
		case GL_RENDERBUFFER_HEIGHT:          *params = renderbuffer->getHeight();      break;
		case GL_RENDERBUFFER_INTERNAL_FORMAT: *params = renderbuffer->getFormat();      break;
		case GL_RENDERBUFFER_RED_SIZE:        *params = renderbuffer->getRedSize();     break;
		case GL_RENDERBUFFER_GREEN_SIZE:      *params = renderbuffer->getGreenSize();   break;
		case GL_RENDERBUFFER_BLUE_SIZE:       *params = renderbuffer->getBlueSize();    break;
		case GL_RENDERBUFFER_ALPHA_SIZE:      *params = renderbuffer->getAlphaSize();   break;
		case GL_RENDERBUFFER_DEPTH_SIZE:      *params = renderbuffer->getDepthSize();   break;
		case GL_RENDERBUFFER_STENCIL_SIZE:    *params = renderbuffer->getStencilSize(); break;
		case GL_RENDERBUFFER_SAMPLES_ANGLE:   *params = renderbuffer->getSamples();     break;
		default:
			return error(GL_INVALID_ENUM);
		}
	}
}

void GetShaderiv(GLuint shader, GLenum pname, GLint* params)
{
	TRACE("(GLuint shader = %d, GLenum pname = %d, GLint* params = %p)", shader, pname, params);

	es2::Context *context = es2::getContext();

	if(context)
	{
		es2::Shader *shaderObject = context->getShader(shader);

		if(!shaderObject)
		{
			return error(GL_INVALID_VALUE);
		}

		switch(pname)
		{
		case GL_SHADER_TYPE:
			*params = shaderObject->getType();
			return;
		case GL_DELETE_STATUS:
			*params = shaderObject->isFlaggedForDeletion();
			return;
		case GL_COMPILE_STATUS:
			*params = shaderObject->isCompiled() ? GL_TRUE : GL_FALSE;
			return;
		case GL_INFO_LOG_LENGTH:
			*params = shaderObject->getInfoLogLength();
			return;
		case GL_SHADER_SOURCE_LENGTH:
			*params = shaderObject->getSourceLength();
			return;
		default:
			return error(GL_INVALID_ENUM);
		}
	}
}

void GetShaderInfoLog(GLuint shader, GLsizei bufsize, GLsizei* length, GLchar* infolog)
{
	TRACE("(GLuint shader = %d, GLsizei bufsize = %d, GLsizei* length = %p, GLchar* infolog = %p)",
	      shader, bufsize, length, infolog);

	if(bufsize < 0)
	{
		return error(GL_INVALID_VALUE);
	}

	es2::Context *context = es2::getContext();

	if(context)
	{
		es2::Shader *shaderObject = context->getShader(shader);

		if(!shaderObject)
		{
			return error(GL_INVALID_VALUE);
		}

		shaderObject->getInfoLog(bufsize, length, infolog);
	}
}

void GetShaderPrecisionFormat(GLenum shadertype, GLenum precisiontype, GLint* range, GLint* precision)
{
	TRACE("(GLenum shadertype = 0x%X, GLenum precisiontype = 0x%X, GLint* range = %p, GLint* precision = %p)",
	      shadertype, precisiontype, range, precision);

	switch(shadertype)
	{
	case GL_VERTEX_SHADER:
	case GL_FRAGMENT_SHADER:
		break;
	default:
		return error(GL_INVALID_ENUM);
	}

	switch(precisiontype)
	{
	case GL_LOW_FLOAT:
	case GL_MEDIUM_FLOAT:
	case GL_HIGH_FLOAT:
		// IEEE 754 single-precision
		range[0] = 127;
		range[1] = 127;
		*precision = 23;
		break;
	case GL_LOW_INT:
	case GL_MEDIUM_INT:
	case GL_HIGH_INT:
		// Full integer precision is supported
		range[0] = 31;
		range[1] = 30;
		*precision = 0;
		break;
	default:
		return error(GL_INVALID_ENUM);
	}
}

void GetShaderSource(GLuint shader, GLsizei bufsize, GLsizei* length, GLchar* source)
{
	TRACE("(GLuint shader = %d, GLsizei bufsize = %d, GLsizei* length = %p, GLchar* source = %p)",
	      shader, bufsize, length, source);

	if(bufsize < 0)
	{
		return error(GL_INVALID_VALUE);
	}

	es2::Context *context = es2::getContext();

	if(context)
	{
		es2::Shader *shaderObject = context->getShader(shader);

		if(!shaderObject)
		{
			return error(GL_INVALID_OPERATION);
		}

		shaderObject->getSource(bufsize, length, source);
	}
}

const GLubyte* GetString(GLenum name)
{
	TRACE("(GLenum name = 0x%X)", name);

	switch(name)
	{
	case GL_VENDOR:
		return (GLubyte*)"Google Inc.";
	case GL_RENDERER:
		return (GLubyte*)"Google SwiftShader";
	case GL_VERSION:
	{
		es2::Context *context = es2::getContext();
		return (context && (context->getClientVersion() >= 3)) ?
		       (GLubyte*)"OpenGL ES 3.0 SwiftShader " VERSION_STRING :
		       (GLubyte*)"OpenGL ES 2.0 SwiftShader " VERSION_STRING;
	}
	case GL_SHADING_LANGUAGE_VERSION:
	{
		es2::Context *context = es2::getContext();
		return (context && (context->getClientVersion() >= 3)) ?
		       (GLubyte*)"OpenGL ES GLSL ES 3.00 SwiftShader " VERSION_STRING :
		       (GLubyte*)"OpenGL ES GLSL ES 1.00 SwiftShader " VERSION_STRING;
	}
	case GL_EXTENSIONS:
	{
		es2::Context *context = es2::getContext();
		return context ? context->getExtensions(GL_INVALID_INDEX) : (GLubyte*)NULL;
	}
	default:
		return error(GL_INVALID_ENUM, (GLubyte*)NULL);
	}
}

void GetTexParameterfv(GLenum target, GLenum pname, GLfloat* params)
{
	TRACE("(GLenum target = 0x%X, GLenum pname = 0x%X, GLfloat* params = %p)", target, pname, params);

	es2::Context *context = es2::getContext();

	if(context)
	{
		es2::Texture *texture;

		GLint clientVersion = context->getClientVersion();

		switch(target)
		{
		case GL_TEXTURE_2D:
			texture = context->getTexture2D();
			break;
		case GL_TEXTURE_CUBE_MAP:
			texture = context->getTextureCubeMap();
			break;
		case GL_TEXTURE_EXTERNAL_OES:
			texture = context->getTextureExternal();
			break;
		case GL_TEXTURE_2D_ARRAY:
			if(clientVersion < 3)
			{
				return error(GL_INVALID_ENUM);
			}
			else
			{
				texture = context->getTexture2DArray();
			}
			break;
		case GL_TEXTURE_3D_OES:
			texture = context->getTexture3D();
			break;
		default:
			return error(GL_INVALID_ENUM);
		}

		switch(pname)
		{
		case GL_TEXTURE_MAG_FILTER:
			*params = (GLfloat)texture->getMagFilter();
			break;
		case GL_TEXTURE_MIN_FILTER:
			*params = (GLfloat)texture->getMinFilter();
			break;
		case GL_TEXTURE_WRAP_S:
			*params = (GLfloat)texture->getWrapS();
			break;
		case GL_TEXTURE_WRAP_T:
			*params = (GLfloat)texture->getWrapT();
			break;
		case GL_TEXTURE_WRAP_R_OES:
			*params = (GLfloat)texture->getWrapR();
			break;
		case GL_TEXTURE_MAX_ANISOTROPY_EXT:
			*params = texture->getMaxAnisotropy();
			break;
		case GL_REQUIRED_TEXTURE_IMAGE_UNITS_OES:
			*params = (GLfloat)1;
			break;
		case GL_TEXTURE_BASE_LEVEL:
			if(clientVersion >= 3)
			{
				*params = (GLfloat)texture->getBaseLevel();
				break;
			}
			else return error(GL_INVALID_ENUM);
		case GL_TEXTURE_COMPARE_FUNC:
			if(clientVersion >= 3)
			{
				*params = (GLfloat)texture->getCompareFunc();
				break;
			}
			else return error(GL_INVALID_ENUM);
		case GL_TEXTURE_COMPARE_MODE:
			if(clientVersion >= 3)
			{
				*params = (GLfloat)texture->getCompareMode();
				break;
			}
			else return error(GL_INVALID_ENUM);
		case GL_TEXTURE_IMMUTABLE_FORMAT:
			if(clientVersion >= 3)
			{
				*params = (GLfloat)texture->getImmutableFormat();
				break;
			}
			else return error(GL_INVALID_ENUM);
		case GL_TEXTURE_MAX_LEVEL:
			if(clientVersion >= 3)
			{
				*params = (GLfloat)texture->getMaxLevel();
				break;
			}
			else return error(GL_INVALID_ENUM);
		case GL_TEXTURE_MAX_LOD:
			if(clientVersion >= 3)
			{
				*params = texture->getMaxLOD();
				break;
			}
			else return error(GL_INVALID_ENUM);
		case GL_TEXTURE_MIN_LOD:
			if(clientVersion >= 3)
			{
				*params = texture->getMinLOD();
				break;
			}
			else return error(GL_INVALID_ENUM);
		case GL_TEXTURE_SWIZZLE_R:
			if(clientVersion >= 3)
			{
				*params = (GLfloat)texture->getSwizzleR();
				break;
			}
			else return error(GL_INVALID_ENUM);
		case GL_TEXTURE_SWIZZLE_G:
			if(clientVersion >= 3)
			{
				*params = (GLfloat)texture->getSwizzleG();
				break;
			}
			else return error(GL_INVALID_ENUM);
		case GL_TEXTURE_SWIZZLE_B:
			if(clientVersion >= 3)
			{
				*params = (GLfloat)texture->getSwizzleB();
				break;
			}
			else return error(GL_INVALID_ENUM);
		case GL_TEXTURE_SWIZZLE_A:
			if(clientVersion >= 3)
			{
				*params = (GLfloat)texture->getSwizzleA();
				break;
			}
			else return error(GL_INVALID_ENUM);
		default:
			return error(GL_INVALID_ENUM);
		}
	}
}

void GetTexParameteriv(GLenum target, GLenum pname, GLint* params)
{
	TRACE("(GLenum target = 0x%X, GLenum pname = 0x%X, GLint* params = %p)", target, pname, params);

	es2::Context *context = es2::getContext();

	if(context)
	{
		es2::Texture *texture;

		GLint clientVersion = context->getClientVersion();

		switch(target)
		{
		case GL_TEXTURE_2D:
			texture = context->getTexture2D();
			break;
		case GL_TEXTURE_CUBE_MAP:
			texture = context->getTextureCubeMap();
			break;
		case GL_TEXTURE_EXTERNAL_OES:
			texture = context->getTextureExternal();
			break;
		case GL_TEXTURE_2D_ARRAY:
			if(clientVersion < 3)
			{
				return error(GL_INVALID_ENUM);
			}
			else
			{
				texture = context->getTexture2DArray();
			}
			break;
		case GL_TEXTURE_3D_OES:
			texture = context->getTexture3D();
			break;
		default:
			return error(GL_INVALID_ENUM);
		}

		switch(pname)
		{
		case GL_TEXTURE_MAG_FILTER:
			*params = texture->getMagFilter();
			break;
		case GL_TEXTURE_MIN_FILTER:
			*params = texture->getMinFilter();
			break;
		case GL_TEXTURE_WRAP_S:
			*params = texture->getWrapS();
			break;
		case GL_TEXTURE_WRAP_T:
			*params = texture->getWrapT();
			break;
		case GL_TEXTURE_WRAP_R_OES:
			*params = texture->getWrapR();
			break;
		case GL_TEXTURE_MAX_ANISOTROPY_EXT:
			*params = (GLint)texture->getMaxAnisotropy();
			break;
		case GL_REQUIRED_TEXTURE_IMAGE_UNITS_OES:
			*params = 1;
			break;
		case GL_TEXTURE_BASE_LEVEL:
			if(clientVersion >= 3)
			{
				*params = texture->getBaseLevel();
				break;
			}
			else return error(GL_INVALID_ENUM);
		case GL_TEXTURE_COMPARE_FUNC:
			if(clientVersion >= 3)
			{
				*params = (GLint)texture->getCompareFunc();
				break;
			}
			else return error(GL_INVALID_ENUM);
		case GL_TEXTURE_COMPARE_MODE:
			if(clientVersion >= 3)
			{
				*params = (GLint)texture->getCompareMode();
				break;
			}
			else return error(GL_INVALID_ENUM);
		case GL_TEXTURE_IMMUTABLE_FORMAT:
			if(clientVersion >= 3)
			{
				*params = (GLint)texture->getImmutableFormat();
				break;
			}
			else return error(GL_INVALID_ENUM);
		case GL_TEXTURE_MAX_LEVEL:
			if(clientVersion >= 3)
			{
				*params = texture->getMaxLevel();
				break;
			}
			else return error(GL_INVALID_ENUM);
		case GL_TEXTURE_MAX_LOD:
			if(clientVersion >= 3)
			{
				*params = (GLint)roundf(texture->getMaxLOD());
				break;
			}
			else return error(GL_INVALID_ENUM);
		case GL_TEXTURE_MIN_LOD:
			if(clientVersion >= 3)
			{
				*params = (GLint)roundf(texture->getMinLOD());
				break;
			}
			else return error(GL_INVALID_ENUM);
		case GL_TEXTURE_SWIZZLE_R:
			if(clientVersion >= 3)
			{
				*params = (GLint)texture->getSwizzleR();
				break;
			}
			else return error(GL_INVALID_ENUM);
		case GL_TEXTURE_SWIZZLE_G:
			if(clientVersion >= 3)
			{
				*params = (GLint)texture->getSwizzleG();
				break;
			}
			else return error(GL_INVALID_ENUM);
		case GL_TEXTURE_SWIZZLE_B:
			if(clientVersion >= 3)
			{
				*params = (GLint)texture->getSwizzleB();
				break;
			}
			else return error(GL_INVALID_ENUM);
		case GL_TEXTURE_SWIZZLE_A:
			if(clientVersion >= 3)
			{
				*params = (GLint)texture->getSwizzleA();
				break;
			}
			else return error(GL_INVALID_ENUM);
		default:
			return error(GL_INVALID_ENUM);
		}
	}
}

void GetnUniformfvEXT(GLuint program, GLint location, GLsizei bufSize, GLfloat* params)
{
	TRACE("(GLuint program = %d, GLint location = %d, GLsizei bufSize = %d, GLfloat* params = %p)",
	      program, location, bufSize, params);

	if(bufSize < 0)
	{
		return error(GL_INVALID_VALUE);
	}

	es2::Context *context = es2::getContext();

	if(context)
	{
		if(program == 0)
		{
			return error(GL_INVALID_VALUE);
		}

		es2::Program *programObject = context->getProgram(program);

		if(!programObject || !programObject->isLinked())
		{
			return error(GL_INVALID_OPERATION);
		}

		if(!programObject->getUniformfv(location, &bufSize, params))
		{
			return error(GL_INVALID_OPERATION);
		}
	}
}

void GetUniformfv(GLuint program, GLint location, GLfloat* params)
{
	TRACE("(GLuint program = %d, GLint location = %d, GLfloat* params = %p)", program, location, params);

	es2::Context *context = es2::getContext();

	if(context)
	{
		if(program == 0)
		{
			return error(GL_INVALID_VALUE);
		}

		es2::Program *programObject = context->getProgram(program);

		if(!programObject || !programObject->isLinked())
		{
			return error(GL_INVALID_OPERATION);
		}

		if(!programObject->getUniformfv(location, NULL, params))
		{
			return error(GL_INVALID_OPERATION);
		}
	}
}

void GetnUniformivEXT(GLuint program, GLint location, GLsizei bufSize, GLint* params)
{
	TRACE("(GLuint program = %d, GLint location = %d, GLsizei bufSize = %d, GLint* params = %p)",
	      program, location, bufSize, params);

	if(bufSize < 0)
	{
		return error(GL_INVALID_VALUE);
	}

	es2::Context *context = es2::getContext();

	if(context)
	{
		if(program == 0)
		{
			return error(GL_INVALID_VALUE);
		}

		es2::Program *programObject = context->getProgram(program);

		if(!programObject || !programObject->isLinked())
		{
			return error(GL_INVALID_OPERATION);
		}

		if(!programObject)
		{
			return error(GL_INVALID_OPERATION);
		}

		if(!programObject->getUniformiv(location, &bufSize, params))
		{
			return error(GL_INVALID_OPERATION);
		}
	}
}

void GetUniformiv(GLuint program, GLint location, GLint* params)
{
	TRACE("(GLuint program = %d, GLint location = %d, GLint* params = %p)", program, location, params);

	es2::Context *context = es2::getContext();

	if(context)
	{
		if(program == 0)
		{
			return error(GL_INVALID_VALUE);
		}

		es2::Program *programObject = context->getProgram(program);

		if(!programObject || !programObject->isLinked())
		{
			return error(GL_INVALID_OPERATION);
		}

		if(!programObject)
		{
			return error(GL_INVALID_OPERATION);
		}

		if(!programObject->getUniformiv(location, NULL, params))
		{
			return error(GL_INVALID_OPERATION);
		}
	}
}

int GetUniformLocation(GLuint program, const GLchar* name)
{
	TRACE("(GLuint program = %d, const GLchar* name = %s)", program, name);

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

		return programObject->getUniformLocation(name);
	}

	return -1;
}

void GetVertexAttribfv(GLuint index, GLenum pname, GLfloat* params)
{
	TRACE("(GLuint index = %d, GLenum pname = 0x%X, GLfloat* params = %p)", index, pname, params);

	es2::Context *context = es2::getContext();

	if(context)
	{
		if(index >= es2::MAX_VERTEX_ATTRIBS)
		{
			return error(GL_INVALID_VALUE);
		}

		const es2::VertexAttribute &attribState = context->getVertexAttribState(index);

		GLint clientVersion = context->getClientVersion();

		switch(pname)
		{
		case GL_VERTEX_ATTRIB_ARRAY_ENABLED:
			*params = (GLfloat)(attribState.mArrayEnabled ? GL_TRUE : GL_FALSE);
			break;
		case GL_VERTEX_ATTRIB_ARRAY_SIZE:
			*params = (GLfloat)attribState.mSize;
			break;
		case GL_VERTEX_ATTRIB_ARRAY_STRIDE:
			*params = (GLfloat)attribState.mStride;
			break;
		case GL_VERTEX_ATTRIB_ARRAY_TYPE:
			*params = (GLfloat)attribState.mType;
			break;
		case GL_VERTEX_ATTRIB_ARRAY_NORMALIZED:
			*params = (GLfloat)(attribState.mNormalized ? GL_TRUE : GL_FALSE);
			break;
		case GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING:
			*params = (GLfloat)attribState.mBoundBuffer.name();
			break;
		case GL_CURRENT_VERTEX_ATTRIB:
			{
				const VertexAttribute& attrib = context->getCurrentVertexAttributes()[index];
				for(int i = 0; i < 4; ++i)
				{
					params[i] = attrib.getCurrentValueF(i);
				}
			}
			break;
		case GL_VERTEX_ATTRIB_ARRAY_INTEGER:
			if(clientVersion >= 3)
			{
				switch(attribState.mType)
				{
				case GL_BYTE:
				case GL_UNSIGNED_BYTE:
				case GL_SHORT:
				case GL_UNSIGNED_SHORT:
				case GL_INT:
				case GL_INT_2_10_10_10_REV:
				case GL_UNSIGNED_INT:
				case GL_FIXED:
					*params = (GLfloat)GL_TRUE;
					break;
				default:
					*params = (GLfloat)GL_FALSE;
					break;
				}
				break;
			}
			else return error(GL_INVALID_ENUM);
		default: return error(GL_INVALID_ENUM);
		}
	}
}

void GetVertexAttribiv(GLuint index, GLenum pname, GLint* params)
{
	TRACE("(GLuint index = %d, GLenum pname = 0x%X, GLint* params = %p)", index, pname, params);

	es2::Context *context = es2::getContext();

	if(context)
	{
		if(index >= es2::MAX_VERTEX_ATTRIBS)
		{
			return error(GL_INVALID_VALUE);
		}

		const es2::VertexAttribute &attribState = context->getVertexAttribState(index);

		GLint clientVersion = context->getClientVersion();

		switch(pname)
		{
		case GL_VERTEX_ATTRIB_ARRAY_ENABLED:
			*params = (attribState.mArrayEnabled ? GL_TRUE : GL_FALSE);
			break;
		case GL_VERTEX_ATTRIB_ARRAY_SIZE:
			*params = attribState.mSize;
			break;
		case GL_VERTEX_ATTRIB_ARRAY_STRIDE:
			*params = attribState.mStride;
			break;
		case GL_VERTEX_ATTRIB_ARRAY_TYPE:
			*params = attribState.mType;
			break;
		case GL_VERTEX_ATTRIB_ARRAY_NORMALIZED:
			*params = (attribState.mNormalized ? GL_TRUE : GL_FALSE);
			break;
		case GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING:
			*params = attribState.mBoundBuffer.name();
			break;
		case GL_CURRENT_VERTEX_ATTRIB:
			{
				const VertexAttribute& attrib = context->getCurrentVertexAttributes()[index];
				for(int i = 0; i < 4; ++i)
				{
					float currentValue = attrib.getCurrentValueF(i);
					params[i] = (GLint)(currentValue > 0.0f ? floor(currentValue + 0.5f) : ceil(currentValue - 0.5f));
				}
			}
			break;
		case GL_VERTEX_ATTRIB_ARRAY_INTEGER:
			if(clientVersion >= 3)
			{
				switch(attribState.mType)
				{
				case GL_BYTE:
				case GL_UNSIGNED_BYTE:
				case GL_SHORT:
				case GL_UNSIGNED_SHORT:
				case GL_INT:
				case GL_INT_2_10_10_10_REV:
				case GL_UNSIGNED_INT:
				case GL_FIXED:
					*params = GL_TRUE;
					break;
				default:
					*params = GL_FALSE;
					break;
				}
				break;
			}
			else return error(GL_INVALID_ENUM);
		default: return error(GL_INVALID_ENUM);
		}
	}
}

void GetVertexAttribPointerv(GLuint index, GLenum pname, GLvoid** pointer)
{
	TRACE("(GLuint index = %d, GLenum pname = 0x%X, GLvoid** pointer = %p)", index, pname, pointer);

	es2::Context *context = es2::getContext();

	if(context)
	{
		if(index >= es2::MAX_VERTEX_ATTRIBS)
		{
			return error(GL_INVALID_VALUE);
		}

		if(pname != GL_VERTEX_ATTRIB_ARRAY_POINTER)
		{
			return error(GL_INVALID_ENUM);
		}

		*pointer = const_cast<GLvoid*>(context->getVertexAttribPointer(index));
	}
}

void Hint(GLenum target, GLenum mode)
{
	TRACE("(GLenum target = 0x%X, GLenum mode = 0x%X)", target, mode);

	switch(mode)
	{
	case GL_FASTEST:
	case GL_NICEST:
	case GL_DONT_CARE:
		break;
	default:
		return error(GL_INVALID_ENUM);
	}

	es2::Context *context = es2::getContext();
	switch(target)
	{
	case GL_GENERATE_MIPMAP_HINT:
		if(context) context->setGenerateMipmapHint(mode);
		break;
	case GL_FRAGMENT_SHADER_DERIVATIVE_HINT_OES:
		if(context) context->setFragmentShaderDerivativeHint(mode);
		break;
	default:
		return error(GL_INVALID_ENUM);
	}
}

GLboolean IsBuffer(GLuint buffer)
{
	TRACE("(GLuint buffer = %d)", buffer);

	es2::Context *context = es2::getContext();

	if(context && buffer)
	{
		es2::Buffer *bufferObject = context->getBuffer(buffer);

		if(bufferObject)
		{
			return GL_TRUE;
		}
	}

	return GL_FALSE;
}

GLboolean IsEnabled(GLenum cap)
{
	TRACE("(GLenum cap = 0x%X)", cap);

	es2::Context *context = es2::getContext();

	if(context)
	{
		GLint clientVersion = context->getClientVersion();

		switch(cap)
		{
		case GL_CULL_FACE:                return context->isCullFaceEnabled();
		case GL_POLYGON_OFFSET_FILL:      return context->isPolygonOffsetFillEnabled();
		case GL_SAMPLE_ALPHA_TO_COVERAGE: return context->isSampleAlphaToCoverageEnabled();
		case GL_SAMPLE_COVERAGE:          return context->isSampleCoverageEnabled();
		case GL_SCISSOR_TEST:             return context->isScissorTestEnabled();
		case GL_STENCIL_TEST:             return context->isStencilTestEnabled();
		case GL_DEPTH_TEST:               return context->isDepthTestEnabled();
		case GL_BLEND:                    return context->isBlendEnabled();
		case GL_DITHER:                   return context->isDitherEnabled();
		case GL_PRIMITIVE_RESTART_FIXED_INDEX:
			if(clientVersion >= 3)
			{
				return context->isPrimitiveRestartFixedIndexEnabled();
			}
			else return error(GL_INVALID_ENUM, false);
		case GL_RASTERIZER_DISCARD:
			if(clientVersion >= 3)
			{
				return context->isRasterizerDiscardEnabled();
			}
			else return error(GL_INVALID_ENUM, false);
		default:
			return error(GL_INVALID_ENUM, false);
		}
	}

	return false;
}

GLboolean IsFenceNV(GLuint fence)
{
	TRACE("(GLuint fence = %d)", fence);

	es2::Context *context = es2::getContext();

	if(context)
	{
		es2::Fence *fenceObject = context->getFence(fence);

		if(fenceObject == NULL)
		{
			return GL_FALSE;
		}

		return fenceObject->isFence();
	}

	return GL_FALSE;
}

GLboolean IsFramebuffer(GLuint framebuffer)
{
	TRACE("(GLuint framebuffer = %d)", framebuffer);

	es2::Context *context = es2::getContext();

	if(context && framebuffer)
	{
		es2::Framebuffer *framebufferObject = context->getFramebuffer(framebuffer);

		if(framebufferObject)
		{
			return GL_TRUE;
		}
	}

	return GL_FALSE;
}

GLboolean IsProgram(GLuint program)
{
	TRACE("(GLuint program = %d)", program);

	es2::Context *context = es2::getContext();

	if(context && program)
	{
		es2::Program *programObject = context->getProgram(program);

		if(programObject)
		{
			return GL_TRUE;
		}
	}

	return GL_FALSE;
}

GLboolean IsQueryEXT(GLuint name)
{
	TRACE("(GLuint name = %d)", name);

	if(name == 0)
	{
		return GL_FALSE;
	}

	es2::Context *context = es2::getContext();

	if(context)
	{
		es2::Query *queryObject = context->getQuery(name);

		if(queryObject)
		{
			return GL_TRUE;
		}
	}

	return GL_FALSE;
}

GLboolean IsRenderbuffer(GLuint renderbuffer)
{
	TRACE("(GLuint renderbuffer = %d)", renderbuffer);

	es2::Context *context = es2::getContext();

	if(context && renderbuffer)
	{
		es2::Renderbuffer *renderbufferObject = context->getRenderbuffer(renderbuffer);

		if(renderbufferObject)
		{
			return GL_TRUE;
		}
	}

	return GL_FALSE;
}

GLboolean IsShader(GLuint shader)
{
	TRACE("(GLuint shader = %d)", shader);

	es2::Context *context = es2::getContext();

	if(context && shader)
	{
		es2::Shader *shaderObject = context->getShader(shader);

		if(shaderObject)
		{
			return GL_TRUE;
		}
	}

	return GL_FALSE;
}

GLboolean IsTexture(GLuint texture)
{
	TRACE("(GLuint texture = %d)", texture);

	es2::Context *context = es2::getContext();

	if(context && texture)
	{
		es2::Texture *textureObject = context->getTexture(texture);

		if(textureObject)
		{
			return GL_TRUE;
		}
	}

	return GL_FALSE;
}

void LineWidth(GLfloat width)
{
	TRACE("(GLfloat width = %f)", width);

	if(width <= 0.0f)
	{
		return error(GL_INVALID_VALUE);
	}

	es2::Context *context = es2::getContext();

	if(context)
	{
		context->setLineWidth(width);
	}
}

void LinkProgram(GLuint program)
{
	TRACE("(GLuint program = %d)", program);

	es2::Context *context = es2::getContext();

	if(context)
	{
		es2::Program *programObject = context->getProgram(program);

		if(!programObject)
		{
			if(context->getShader(program))
			{
				return error(GL_INVALID_OPERATION);
			}
			else
			{
				return error(GL_INVALID_VALUE);
			}
		}

		programObject->link();
	}
}

void PixelStorei(GLenum pname, GLint param)
{
	TRACE("(GLenum pname = 0x%X, GLint param = %d)", pname, param);

	es2::Context *context = es2::getContext();

	if(context)
	{
		GLint clientVersion = context->getClientVersion();

		switch(pname)
		{
		case GL_UNPACK_ALIGNMENT:
			if(param != 1 && param != 2 && param != 4 && param != 8)
			{
				return error(GL_INVALID_VALUE);
			}
			context->setUnpackAlignment(param);
			break;
		case GL_PACK_ALIGNMENT:
			if(param != 1 && param != 2 && param != 4 && param != 8)
			{
				return error(GL_INVALID_VALUE);
			}
			context->setPackAlignment(param);
			break;
		case GL_PACK_ROW_LENGTH:
			if(clientVersion >= 3)
			{
				if(param < 0)
				{
					return error(GL_INVALID_VALUE);
				}
				context->setPackRowLength(param);
				break;
			}
			else return error(GL_INVALID_ENUM);
		case GL_PACK_SKIP_PIXELS:
			if(clientVersion >= 3)
			{
				if(param < 0)
				{
					return error(GL_INVALID_VALUE);
				}
				context->setPackSkipPixels(param);
				break;
			}
			else return error(GL_INVALID_ENUM);
		case GL_PACK_SKIP_ROWS:
			if(clientVersion >= 3)
			{
				if(param < 0)
				{
					return error(GL_INVALID_VALUE);
				}
				context->setPackSkipRows(param);
				break;
			}
			else return error(GL_INVALID_ENUM);
		case GL_UNPACK_ROW_LENGTH:
			if(clientVersion >= 3)
			{
				if(param < 0)
				{
					return error(GL_INVALID_VALUE);
				}
				context->setUnpackRowLength(param);
				break;
			}
			else return error(GL_INVALID_ENUM);
		case GL_UNPACK_IMAGE_HEIGHT:
			if(clientVersion >= 3)
			{
				if(param < 0)
				{
					return error(GL_INVALID_VALUE);
				}
				context->setUnpackImageHeight(param);
				break;
			}
			else return error(GL_INVALID_ENUM);
		case GL_UNPACK_SKIP_PIXELS:
			if(clientVersion >= 3)
			{
				if(param < 0)
				{
					return error(GL_INVALID_VALUE);
				}
				context->setUnpackSkipPixels(param);
				break;
			}
			else return error(GL_INVALID_ENUM);
		case GL_UNPACK_SKIP_ROWS:
			if(clientVersion >= 3)
			{
				if(param < 0)
				{
					return error(GL_INVALID_VALUE);
				}
				context->setUnpackSkipRows(param);
				break;
			}
			else return error(GL_INVALID_ENUM);
		case GL_UNPACK_SKIP_IMAGES:
			if(clientVersion >= 3) {
				if(param < 0)
				{
					return error(GL_INVALID_VALUE);
				}
				context->setUnpackSkipImages(param);
				break;
			}
			else return error(GL_INVALID_ENUM);
		default:
			return error(GL_INVALID_ENUM);
		}
	}
}

void PolygonOffset(GLfloat factor, GLfloat units)
{
	TRACE("(GLfloat factor = %f, GLfloat units = %f)", factor, units);

	es2::Context *context = es2::getContext();

	if(context)
	{
		context->setPolygonOffsetParams(factor, units);
	}
}

void ReadnPixelsEXT(GLint x, GLint y, GLsizei width, GLsizei height,
                    GLenum format, GLenum type, GLsizei bufSize, GLvoid *data)
{
	TRACE("(GLint x = %d, GLint y = %d, GLsizei width = %d, GLsizei height = %d, "
	      "GLenum format = 0x%X, GLenum type = 0x%X, GLsizei bufSize = 0x%d, GLvoid *data = %p)",
	      x, y, width, height, format, type, bufSize, data);

	if(width < 0 || height < 0 || bufSize < 0)
	{
		return error(GL_INVALID_VALUE);
	}

	es2::Context *context = es2::getContext();

	if(context)
	{
		context->readPixels(x, y, width, height, format, type, &bufSize, data);
	}
}

void ReadPixels(GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid* pixels)
{
	TRACE("(GLint x = %d, GLint y = %d, GLsizei width = %d, GLsizei height = %d, "
	      "GLenum format = 0x%X, GLenum type = 0x%X, GLvoid* pixels = %p)",
	      x, y, width, height, format, type,  pixels);

	if(width < 0 || height < 0)
	{
		return error(GL_INVALID_VALUE);
	}

	es2::Context *context = es2::getContext();

	if(context)
	{
		context->readPixels(x, y, width, height, format, type, NULL, pixels);
	}
}

void ReleaseShaderCompiler(void)
{
	TRACE("()");

	es2::Shader::releaseCompiler();
}

void RenderbufferStorageMultisampleANGLE(GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height)
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

		GLint clientVersion = context->getClientVersion();
		switch(internalformat)
		{
		case GL_DEPTH_COMPONENT32F:
			if(clientVersion < 3)
			{
				return error(GL_INVALID_ENUM);
			}
			// fall through
		case GL_DEPTH_COMPONENT16:
		case GL_DEPTH_COMPONENT24:
		case GL_DEPTH_COMPONENT32_OES:
			context->setRenderbufferStorage(new es2::Depthbuffer(width, height, samples));
			break;
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
		case GL_R16F:
		case GL_RG16F:
		case GL_R11F_G11F_B10F:
		case GL_RGBA16F:
		case GL_R32F:
		case GL_RG32F:
		case GL_RGB32F:
		case GL_RGBA32F:
			if(clientVersion < 3)
			{
				return error(GL_INVALID_ENUM);
			}
			// fall through
		case GL_RGBA4:
		case GL_RGB5_A1:
		case GL_RGB565:
		case GL_RGB8_OES:
		case GL_RGBA8_OES:
			context->setRenderbufferStorage(new es2::Colorbuffer(width, height, internalformat, samples));
			break;
		case GL_STENCIL_INDEX8:
			context->setRenderbufferStorage(new es2::Stencilbuffer(width, height, samples));
			break;
		case GL_DEPTH32F_STENCIL8:
			if(clientVersion < 3)
			{
				return error(GL_INVALID_ENUM);
			}
			// fall through
		case GL_DEPTH24_STENCIL8_OES:
			context->setRenderbufferStorage(new es2::DepthStencilbuffer(width, height, samples));
			break;
		default:
			return error(GL_INVALID_ENUM);
		}
	}
}

void RenderbufferStorage(GLenum target, GLenum internalformat, GLsizei width, GLsizei height)
{
	glRenderbufferStorageMultisampleANGLE(target, 0, internalformat, width, height);
}

void SampleCoverage(GLclampf value, GLboolean invert)
{
	TRACE("(GLclampf value = %f, GLboolean invert = %d)", value, invert);

	es2::Context* context = es2::getContext();

	if(context)
	{
		context->setSampleCoverageParams(es2::clamp01(value), invert == GL_TRUE);
	}
}

void SetFenceNV(GLuint fence, GLenum condition)
{
	TRACE("(GLuint fence = %d, GLenum condition = 0x%X)", fence, condition);

	if(condition != GL_ALL_COMPLETED_NV)
	{
		return error(GL_INVALID_ENUM);
	}

	es2::Context *context = es2::getContext();

	if(context)
	{
		es2::Fence *fenceObject = context->getFence(fence);

		if(fenceObject == NULL)
		{
			return error(GL_INVALID_OPERATION);
		}

		fenceObject->setFence(condition);
	}
}

void Scissor(GLint x, GLint y, GLsizei width, GLsizei height)
{
	TRACE("(GLint x = %d, GLint y = %d, GLsizei width = %d, GLsizei height = %d)", x, y, width, height);

	if(width < 0 || height < 0)
	{
		return error(GL_INVALID_VALUE);
	}

	es2::Context* context = es2::getContext();

	if(context)
	{
		context->setScissorParams(x, y, width, height);
	}
}

void ShaderBinary(GLsizei n, const GLuint* shaders, GLenum binaryformat, const GLvoid* binary, GLsizei length)
{
	TRACE("(GLsizei n = %d, const GLuint* shaders = %p, GLenum binaryformat = 0x%X, "
	      "const GLvoid* binary = %p, GLsizei length = %d)",
	      n, shaders, binaryformat, binary, length);

	// No binary shader formats are supported.
	return error(GL_INVALID_ENUM);
}

void ShaderSource(GLuint shader, GLsizei count, const GLchar *const *string, const GLint *length)
{
	TRACE("(GLuint shader = %d, GLsizei count = %d, const GLchar** string = %p, const GLint* length = %p)",
	      shader, count, string, length);

	if(count < 0)
	{
		return error(GL_INVALID_VALUE);
	}

	es2::Context *context = es2::getContext();

	if(context)
	{
		es2::Shader *shaderObject = context->getShader(shader);

		if(!shaderObject)
		{
			if(context->getProgram(shader))
			{
				return error(GL_INVALID_OPERATION);
			}
			else
			{
				return error(GL_INVALID_VALUE);
			}
		}

		shaderObject->setSource(count, string, length);
	}
}

void StencilFunc(GLenum func, GLint ref, GLuint mask)
{
	glStencilFuncSeparate(GL_FRONT_AND_BACK, func, ref, mask);
}

void StencilFuncSeparate(GLenum face, GLenum func, GLint ref, GLuint mask)
{
	TRACE("(GLenum face = 0x%X, GLenum func = 0x%X, GLint ref = %d, GLuint mask = %d)", face, func, ref, mask);

	switch(face)
	{
	case GL_FRONT:
	case GL_BACK:
	case GL_FRONT_AND_BACK:
		break;
	default:
		return error(GL_INVALID_ENUM);
	}

	switch(func)
	{
	case GL_NEVER:
	case GL_ALWAYS:
	case GL_LESS:
	case GL_LEQUAL:
	case GL_EQUAL:
	case GL_GEQUAL:
	case GL_GREATER:
	case GL_NOTEQUAL:
		break;
	default:
		return error(GL_INVALID_ENUM);
	}

	es2::Context *context = es2::getContext();

	if(context)
	{
		if(face == GL_FRONT || face == GL_FRONT_AND_BACK)
		{
			context->setStencilParams(func, ref, mask);
		}

		if(face == GL_BACK || face == GL_FRONT_AND_BACK)
		{
			context->setStencilBackParams(func, ref, mask);
		}
	}
}

void StencilMask(GLuint mask)
{
	glStencilMaskSeparate(GL_FRONT_AND_BACK, mask);
}

void StencilMaskSeparate(GLenum face, GLuint mask)
{
	TRACE("(GLenum face = 0x%X, GLuint mask = %d)", face, mask);

	switch(face)
	{
	case GL_FRONT:
	case GL_BACK:
	case GL_FRONT_AND_BACK:
		break;
	default:
		return error(GL_INVALID_ENUM);
	}

	es2::Context *context = es2::getContext();

	if(context)
	{
		if(face == GL_FRONT || face == GL_FRONT_AND_BACK)
		{
			context->setStencilWritemask(mask);
		}

		if(face == GL_BACK || face == GL_FRONT_AND_BACK)
		{
			context->setStencilBackWritemask(mask);
		}
	}
}

void StencilOp(GLenum fail, GLenum zfail, GLenum zpass)
{
	glStencilOpSeparate(GL_FRONT_AND_BACK, fail, zfail, zpass);
}

void StencilOpSeparate(GLenum face, GLenum fail, GLenum zfail, GLenum zpass)
{
	TRACE("(GLenum face = 0x%X, GLenum fail = 0x%X, GLenum zfail = 0x%X, GLenum zpas = 0x%Xs)",
	      face, fail, zfail, zpass);

	switch(face)
	{
	case GL_FRONT:
	case GL_BACK:
	case GL_FRONT_AND_BACK:
		break;
	default:
		return error(GL_INVALID_ENUM);
	}

	switch(fail)
	{
	case GL_ZERO:
	case GL_KEEP:
	case GL_REPLACE:
	case GL_INCR:
	case GL_DECR:
	case GL_INVERT:
	case GL_INCR_WRAP:
	case GL_DECR_WRAP:
		break;
	default:
		return error(GL_INVALID_ENUM);
	}

	switch(zfail)
	{
	case GL_ZERO:
	case GL_KEEP:
	case GL_REPLACE:
	case GL_INCR:
	case GL_DECR:
	case GL_INVERT:
	case GL_INCR_WRAP:
	case GL_DECR_WRAP:
		break;
	default:
		return error(GL_INVALID_ENUM);
	}

	switch(zpass)
	{
	case GL_ZERO:
	case GL_KEEP:
	case GL_REPLACE:
	case GL_INCR:
	case GL_DECR:
	case GL_INVERT:
	case GL_INCR_WRAP:
	case GL_DECR_WRAP:
		break;
	default:
		return error(GL_INVALID_ENUM);
	}

	es2::Context *context = es2::getContext();

	if(context)
	{
		if(face == GL_FRONT || face == GL_FRONT_AND_BACK)
		{
			context->setStencilOperations(fail, zfail, zpass);
		}

		if(face == GL_BACK || face == GL_FRONT_AND_BACK)
		{
			context->setStencilBackOperations(fail, zfail, zpass);
		}
	}
}

GLboolean TestFenceNV(GLuint fence)
{
	TRACE("(GLuint fence = %d)", fence);

	es2::Context *context = es2::getContext();

	if(context)
	{
		es2::Fence *fenceObject = context->getFence(fence);

		if(fenceObject == NULL)
		{
			return error(GL_INVALID_OPERATION, GL_TRUE);
		}

		return fenceObject->testFence();
	}

	return GL_TRUE;
}

void TexImage2D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height,
                GLint border, GLenum format, GLenum type, const GLvoid* pixels)
{
	TRACE("(GLenum target = 0x%X, GLint level = %d, GLint internalformat = %d, GLsizei width = %d, GLsizei height = %d, "
	      "GLint border = %d, GLenum format = 0x%X, GLenum type = 0x%X, const GLvoid* pixels =  %p)",
	      target, level, internalformat, width, height, border, format, type, pixels);

	if(!validImageSize(level, width, height))
	{
		return error(GL_INVALID_VALUE);
	}

	es2::Context *context = es2::getContext();

	if(context)
	{
		GLint clientVersion = context->getClientVersion();
		if(clientVersion < 3)
		{
			if(internalformat != (GLint)format)
			{
				return error(GL_INVALID_OPERATION);
			}
		}

		GLenum validationError = ValidateCompressedFormat(format, clientVersion, false);
		if(validationError != GL_NONE)
		{
			return error(validationError);
		}

		switch(format)
		{
		case GL_ALPHA:
		case GL_LUMINANCE:
		case GL_LUMINANCE_ALPHA:
			switch(type)
			{
			case GL_HALF_FLOAT:
				if(clientVersion < 3)
				{
					return error(GL_INVALID_ENUM);
				}
				break;
			case GL_UNSIGNED_BYTE:
			case GL_FLOAT:
			case GL_HALF_FLOAT_OES:
				break;
			default:
				return error(GL_INVALID_ENUM);
			}
			break;
		case GL_RED:
			switch(internalformat)
			{
			case GL_R8:
				switch(type)
				{
				case GL_UNSIGNED_BYTE:
					break;
				default:
					return error(GL_INVALID_ENUM);
				}
				break;
			case GL_R8_SNORM:
				switch(type)
				{
				case GL_BYTE:
					break;
				default:
					return error(GL_INVALID_ENUM);
				}
				break;
			case GL_R16F:
				switch(type)
				{
				case GL_HALF_FLOAT:
					if(clientVersion < 3)
					{
						return error(GL_INVALID_ENUM);
					}
					break;
				case GL_FLOAT:
				case GL_HALF_FLOAT_OES:
					break;
				default:
					return error(GL_INVALID_ENUM);
				}
				break;
			case GL_R32F:
				switch(type)
				{
				case GL_FLOAT:
					break;
				default:
					return error(GL_INVALID_ENUM);
				}
				break;
			default:
				return error(GL_INVALID_VALUE);
			}
			break;
		case GL_RED_INTEGER:
			switch(internalformat)
			{
			case GL_R8UI:
				switch(type)
				{
				case GL_UNSIGNED_BYTE:
					break;
				default:
					return error(GL_INVALID_ENUM);
				}
				break;
			case GL_R8I:
				switch(type)
				{
				case GL_BYTE:
					break;
				default:
					return error(GL_INVALID_ENUM);
				}
				break;
			case GL_R16UI:
				switch(type)
				{
				case GL_UNSIGNED_SHORT:
					break;
				default:
					return error(GL_INVALID_ENUM);
				}
				break;
			case GL_R16I:
				switch(type)
				{
				case GL_SHORT:
					break;
				default:
					return error(GL_INVALID_ENUM);
				}
				break;
			case GL_R32UI:
				switch(type)
				{
				case GL_UNSIGNED_INT:
					break;
				default:
					return error(GL_INVALID_ENUM);
				}
				break;
			case GL_R32I:
				switch(type)
				{
				case GL_INT:
					break;
				default:
					return error(GL_INVALID_ENUM);
				}
				break;
			default:
				return error(GL_INVALID_VALUE);
			}
			break;
		case GL_RG_INTEGER:
			switch(internalformat)
			{
			case GL_RG8UI:
				switch(type)
				{
				case GL_UNSIGNED_BYTE:
					break;
				default:
					return error(GL_INVALID_ENUM);
				}
				break;
			case GL_RG8I:
				switch(type)
				{
				case GL_BYTE:
					break;
				default:
					return error(GL_INVALID_ENUM);
				}
				break;
			case GL_RG16UI:
				switch(type)
				{
				case GL_UNSIGNED_SHORT:
					break;
				default:
					return error(GL_INVALID_ENUM);
				}
				break;
			case GL_RG16I:
				switch(type)
				{
				case GL_SHORT:
					break;
				default:
					return error(GL_INVALID_ENUM);
				}
				break;
			case GL_RG32UI:
				switch(type)
				{
				case GL_UNSIGNED_INT:
					break;
				default:
					return error(GL_INVALID_ENUM);
				}
				break;
			case GL_RG32I:
				switch(type)
				{
				case GL_INT:
					break;
				default:
					return error(GL_INVALID_ENUM);
				}
				break;
			default:
				return error(GL_INVALID_VALUE);
			}
			break;
		case GL_RGB_INTEGER:
			switch(internalformat)
			{
			case GL_RGB8UI:
				switch(type)
				{
				case GL_UNSIGNED_BYTE:
					break;
				default:
					return error(GL_INVALID_ENUM);
				}
				break;
			case GL_RGB8I:
				switch(type)
				{
				case GL_BYTE:
					break;
				default:
					return error(GL_INVALID_ENUM);
				}
				break;
			case GL_RGB16UI:
				switch(type)
				{
				case GL_UNSIGNED_SHORT:
					break;
				default:
					return error(GL_INVALID_ENUM);
				}
				break;
			case GL_RGB16I:
				switch(type)
				{
				case GL_SHORT:
					break;
				default:
					return error(GL_INVALID_ENUM);
				}
				break;
			case GL_RGB32UI:
				switch(type)
				{
				case GL_UNSIGNED_INT:
					break;
				default:
					return error(GL_INVALID_ENUM);
				}
				break;
			case GL_RGB32I:
				switch(type)
				{
				case GL_INT:
					break;
				default:
					return error(GL_INVALID_ENUM);
				}
				break;
			default:
				return error(GL_INVALID_VALUE);
			}
			break;
		case GL_RGBA_INTEGER:
			switch(internalformat)
			{
			case GL_RGBA8UI:
				switch(type)
				{
				case GL_UNSIGNED_BYTE:
					break;
				default:
					return error(GL_INVALID_ENUM);
				}
				break;
			case GL_RGBA8I:
				switch(type)
				{
				case GL_BYTE:
					break;
				default:
					return error(GL_INVALID_ENUM);
				}
				break;
			case GL_RGB10_A2UI:
				switch(type)
				{
				case GL_UNSIGNED_INT_2_10_10_10_REV:
					break;
				default:
					return error(GL_INVALID_ENUM);
				}
				break;
			case GL_RGBA16UI:
				switch(type)
				{
				case GL_UNSIGNED_SHORT:
					break;
				default:
					return error(GL_INVALID_ENUM);
				}
				break;
			case GL_RGBA16I:
				switch(type)
				{
				case GL_SHORT:
					break;
				default:
					return error(GL_INVALID_ENUM);
				}
				break;
			case GL_RGBA32UI:
				switch(type)
				{
				case GL_UNSIGNED_INT:
					break;
				default:
					return error(GL_INVALID_ENUM);
				}
				break;
			case GL_RGBA32I:
				switch(type)
				{
				case GL_INT:
					break;
				default:
					return error(GL_INVALID_ENUM);
				}
				break;
			default:
				return error(GL_INVALID_VALUE);
			}
			break;
		case GL_RG:
			switch(internalformat)
			{
			case GL_RG8:
				switch(type)
				{
				case GL_UNSIGNED_BYTE:
					break;
				default:
					return error(GL_INVALID_ENUM);
				}
				break;
			case GL_RG8_SNORM:
				switch(type)
				{
				case GL_BYTE:
					break;
				default:
					return error(GL_INVALID_ENUM);
				}
				break;
			case GL_RG16F:
				switch(type)
				{
				case GL_HALF_FLOAT:
					if(clientVersion < 3)
					{
						return error(GL_INVALID_ENUM);
					}
					break;
				case GL_FLOAT:
				case GL_HALF_FLOAT_OES:
					break;
				default:
					return error(GL_INVALID_ENUM);
				}
				break;
			case GL_RG32F:
				switch(type)
				{
				case GL_FLOAT:
					break;
				default:
					return error(GL_INVALID_ENUM);
				}
				break;
			default:
				return error(GL_INVALID_VALUE);
			}
			break;
		case GL_RGB:
			switch(internalformat)
			{
			case GL_RGB:
				switch(type)
				{
				case GL_HALF_FLOAT:
					if(clientVersion < 3)
					{
						return error(GL_INVALID_ENUM);
					}
					break;
				case GL_UNSIGNED_BYTE:
				case GL_UNSIGNED_SHORT_5_6_5:
				case GL_FLOAT:
				case GL_HALF_FLOAT_OES:
					break;
				default:
					return error(GL_INVALID_ENUM);
				}
				break;
			case GL_RGB8:
				switch(type)
				{
				case GL_UNSIGNED_BYTE:
					break;
				default:
					return error(GL_INVALID_ENUM);
				}
				break;
			case GL_SRGB8:
				switch(type)
				{
				case GL_UNSIGNED_BYTE:
					break;
				default:
					return error(GL_INVALID_ENUM);
				}
				break;
			case GL_RGB565:
				switch(type)
				{
				case GL_UNSIGNED_BYTE:
				case GL_UNSIGNED_SHORT_5_6_5:
					break;
				default:
					return error(GL_INVALID_ENUM);
				}
				break;
			case GL_RGB8_SNORM:
				switch(type)
				{
				case GL_BYTE:
					break;
				default:
					return error(GL_INVALID_ENUM);
				}
				break;
			case GL_R11F_G11F_B10F:
				switch(type)
				{
				case GL_HALF_FLOAT:
					if(clientVersion < 3)
					{
						return error(GL_INVALID_ENUM);
					}
					break;
				case GL_UNSIGNED_INT_10F_11F_11F_REV:
				case GL_FLOAT:
				case GL_HALF_FLOAT_OES:
					break;
				default:
					return error(GL_INVALID_ENUM);
				}
				break;
			case GL_RGB9_E5:
				switch(type)
				{
				case GL_HALF_FLOAT:
					if(clientVersion < 3)
					{
						return error(GL_INVALID_ENUM);
					}
					break;
				case GL_UNSIGNED_INT_5_9_9_9_REV:
				case GL_FLOAT:
				case GL_HALF_FLOAT_OES:
					break;
				default:
					return error(GL_INVALID_ENUM);
				}
				break;
			case GL_RGB16F:
				switch(type)
				{
				case GL_HALF_FLOAT:
					if(clientVersion < 3)
					{
						return error(GL_INVALID_ENUM);
					}
					break;
				case GL_FLOAT:
				case GL_HALF_FLOAT_OES:
					break;
				default:
					return error(GL_INVALID_ENUM);
				}
				break;
			case GL_RGB32F:
				switch(type)
				{
				case GL_FLOAT:
					break;
				default:
					return error(GL_INVALID_ENUM);
				}
				break;
			default:
				return error(GL_INVALID_VALUE);
			}
			break;
		case GL_RGBA:
			switch(internalformat)
			{
			case GL_RGBA:
				switch(type)
				{
				case GL_HALF_FLOAT:
					if(clientVersion < 3)
					{
						return error(GL_INVALID_ENUM);
					}
					break;
				case GL_UNSIGNED_BYTE:
				case GL_UNSIGNED_SHORT_4_4_4_4:
				case GL_UNSIGNED_SHORT_5_5_5_1:
				case GL_FLOAT:
				case GL_HALF_FLOAT_OES:
					break;
				default:
					return error(GL_INVALID_ENUM);
				}
				break;
			case GL_RGBA8:
				switch(type)
				{
				case GL_UNSIGNED_BYTE:
					break;
				default:
					return error(GL_INVALID_ENUM);
				}
				break;
			case GL_SRGB8_ALPHA8:
				switch(type)
				{
				case GL_UNSIGNED_BYTE:
					break;
				default:
					return error(GL_INVALID_ENUM);
				}
				break;
			case GL_RGB5_A1:
				switch(type)
				{
				case GL_UNSIGNED_BYTE:
				case GL_UNSIGNED_SHORT_5_5_5_1:
				case GL_UNSIGNED_INT_2_10_10_10_REV:
					break;
				default:
					return error(GL_INVALID_ENUM);
				}
				break;
			case GL_RGBA8_SNORM:
				switch(type)
				{
				case GL_BYTE:
					break;
				default:
					return error(GL_INVALID_ENUM);
				}
				break;
			case GL_RGBA4:
				switch(type)
				{
				case GL_UNSIGNED_BYTE:
				case GL_UNSIGNED_SHORT_4_4_4_4:
					break;
				default:
					return error(GL_INVALID_ENUM);
				}
				break;
			case GL_RGB10_A2:
				switch(type)
				{
				case GL_UNSIGNED_INT_2_10_10_10_REV:
					break;
				default:
					return error(GL_INVALID_ENUM);
				}
				break;
			case GL_RGBA16F:
				switch(type)
				{
				case GL_HALF_FLOAT:
					if(clientVersion < 3)
					{
						return error(GL_INVALID_ENUM);
					}
					break;
				case GL_FLOAT:
				case GL_HALF_FLOAT_OES:
					break;
				default:
					return error(GL_INVALID_ENUM);
				}
				break;
			case GL_RGBA32F:
				switch(type)
				{
				case GL_FLOAT:
					break;
				default:
					return error(GL_INVALID_ENUM);
				}
				break;
			default:
				return error(GL_INVALID_VALUE);
			}
			break;
		case GL_BGRA_EXT:
			switch(type)
			{
			case GL_UNSIGNED_BYTE:
				break;
			default:
				return error(GL_INVALID_ENUM);
			}
			break;
		case GL_DEPTH_COMPONENT:
			switch(internalformat)
			{
			case GL_DEPTH_COMPONENT:
			case GL_DEPTH_COMPONENT16:
				switch(type)
				{
				case GL_UNSIGNED_SHORT:
				case GL_UNSIGNED_INT:
					break;
				default:
					return error(GL_INVALID_ENUM);
				}
				break;
			case GL_DEPTH_COMPONENT24:
			case GL_DEPTH_COMPONENT32_OES:
				switch(type)
				{
				case GL_UNSIGNED_INT:
					break;
				default:
					return error(GL_INVALID_ENUM);
				}
				break;
			case GL_DEPTH_COMPONENT32F:
				switch(type)
				{
				case GL_FLOAT:
					break;
				default:
					return error(GL_INVALID_ENUM);
				}
				break;
			default:
				return error(GL_INVALID_VALUE);
			}
			break;
		case GL_DEPTH_STENCIL_OES:
			switch(internalformat)
			{
			case GL_DEPTH_STENCIL_OES:
			case GL_DEPTH24_STENCIL8:
				switch(type)
				{
				case GL_UNSIGNED_INT_24_8_OES:
					break;
				default:
					return error(GL_INVALID_ENUM);
				}
				break;
			case GL_DEPTH32F_STENCIL8:
				switch(type)
				{
				case GL_FLOAT_32_UNSIGNED_INT_24_8_REV:
					break;
				default:
					return error(GL_INVALID_ENUM);
				}
				break;
			default:
				return error(GL_INVALID_VALUE);
			}
			break;
		default:
			return error(GL_INVALID_VALUE);
		}

		if(border != 0)
		{
			return error(GL_INVALID_VALUE);
		}

		switch(target)
		{
		case GL_TEXTURE_2D:
			if(width > (es2::IMPLEMENTATION_MAX_TEXTURE_SIZE >> level) ||
			   height > (es2::IMPLEMENTATION_MAX_TEXTURE_SIZE >> level))
			{
				return error(GL_INVALID_VALUE);
			}
			break;
		case GL_TEXTURE_CUBE_MAP_POSITIVE_X:
		case GL_TEXTURE_CUBE_MAP_NEGATIVE_X:
		case GL_TEXTURE_CUBE_MAP_POSITIVE_Y:
		case GL_TEXTURE_CUBE_MAP_NEGATIVE_Y:
		case GL_TEXTURE_CUBE_MAP_POSITIVE_Z:
		case GL_TEXTURE_CUBE_MAP_NEGATIVE_Z:
			if(width != height)
			{
				return error(GL_INVALID_VALUE);
			}

			if(width > (es2::IMPLEMENTATION_MAX_CUBE_MAP_TEXTURE_SIZE >> level) ||
			   height > (es2::IMPLEMENTATION_MAX_CUBE_MAP_TEXTURE_SIZE >> level))
			{
				return error(GL_INVALID_VALUE);
			}
			break;
		default:
			return error(GL_INVALID_ENUM);
		}

		GLenum sizedInternalFormat = GetSizedInternalFormat(format, type);

		if(target == GL_TEXTURE_2D)
		{
			es2::Texture2D *texture = context->getTexture2D();

			if(!texture)
			{
				return error(GL_INVALID_OPERATION);
			}

			texture->setImage(level, width, height, sizedInternalFormat, type, context->getUnpackInfo(), pixels);
		}
		else
		{
			es2::TextureCubeMap *texture = context->getTextureCubeMap();

			if(!texture)
			{
				return error(GL_INVALID_OPERATION);
			}

			texture->setImage(target, level, width, height, sizedInternalFormat, type, context->getUnpackInfo(), pixels);
		}
	}
}

void TexParameterf(GLenum target, GLenum pname, GLfloat param)
{
	TRACE("(GLenum target = 0x%X, GLenum pname = 0x%X, GLfloat param = %f)", target, pname, param);

	es2::Context *context = es2::getContext();

	if(context)
	{
		es2::Texture *texture;

		GLint clientVersion = context->getClientVersion();

		switch(target)
		{
		case GL_TEXTURE_2D:
			texture = context->getTexture2D();
			break;
		case GL_TEXTURE_2D_ARRAY:
			if(clientVersion < 3)
			{
				return error(GL_INVALID_ENUM);
			}
			else
			{
				texture = context->getTexture2DArray();
			}
			break;
		case GL_TEXTURE_3D_OES:
			texture = context->getTexture3D();
			break;
		case GL_TEXTURE_CUBE_MAP:
			texture = context->getTextureCubeMap();
			break;
		case GL_TEXTURE_EXTERNAL_OES:
			texture = context->getTextureExternal();
			break;
		default:
			return error(GL_INVALID_ENUM);
		}

		switch(pname)
		{
		case GL_TEXTURE_WRAP_S:
			if(!texture->setWrapS((GLenum)param))
			{
				return error(GL_INVALID_ENUM);
			}
			break;
		case GL_TEXTURE_WRAP_T:
			if(!texture->setWrapT((GLenum)param))
			{
				return error(GL_INVALID_ENUM);
			}
			break;
		case GL_TEXTURE_WRAP_R_OES:
			if(!texture->setWrapR((GLenum)param))
			{
				return error(GL_INVALID_ENUM);
			}
			break;
		case GL_TEXTURE_MIN_FILTER:
			if(!texture->setMinFilter((GLenum)param))
			{
				return error(GL_INVALID_ENUM);
			}
			break;
		case GL_TEXTURE_MAG_FILTER:
			if(!texture->setMagFilter((GLenum)param))
			{
				return error(GL_INVALID_ENUM);
			}
			break;
		case GL_TEXTURE_MAX_ANISOTROPY_EXT:
			if(!texture->setMaxAnisotropy(param))
			{
				return error(GL_INVALID_VALUE);
			}
			break;
		case GL_TEXTURE_BASE_LEVEL:
			if(clientVersion < 3 || !texture->setBaseLevel((GLint)(roundf(param))))
			{
				return error(GL_INVALID_VALUE);
			}
			break;
		case GL_TEXTURE_COMPARE_FUNC:
			if(clientVersion < 3 || !texture->setCompareFunc((GLenum)param))
			{
				return error(GL_INVALID_VALUE);
			}
			break;
		case GL_TEXTURE_COMPARE_MODE:
			if(clientVersion < 3 || !texture->setCompareMode((GLenum)param))
			{
				return error(GL_INVALID_VALUE);
			}
			break;
		case GL_TEXTURE_IMMUTABLE_FORMAT:
			if(clientVersion < 3 || !texture->setImmutableFormat((GLboolean)param))
			{
				return error(GL_INVALID_VALUE);
			}
			break;
		case GL_TEXTURE_MAX_LEVEL:
			if(clientVersion < 3 || !texture->setMaxLevel((GLint)(roundf(param))))
			{
				return error(GL_INVALID_VALUE);
			}
			break;
		case GL_TEXTURE_MAX_LOD:
			if(clientVersion < 3 || !texture->setMaxLOD(param))
			{
				return error(GL_INVALID_VALUE);
			}
			break;
		case GL_TEXTURE_MIN_LOD:
			if(clientVersion < 3 || !texture->setMinLOD(param))
			{
				return error(GL_INVALID_VALUE);
			}
			break;
		case GL_TEXTURE_SWIZZLE_R:
			if(clientVersion < 3 || !texture->setSwizzleR((GLenum)param))
			{
				return error(GL_INVALID_VALUE);
			}
			break;
		case GL_TEXTURE_SWIZZLE_G:
			if(clientVersion < 3 || !texture->setSwizzleG((GLenum)param))
			{
				return error(GL_INVALID_VALUE);
			}
			break;
		case GL_TEXTURE_SWIZZLE_B:
			if(clientVersion < 3 || !texture->setSwizzleB((GLenum)param))
			{
				return error(GL_INVALID_VALUE);
			}
			break;
		case GL_TEXTURE_SWIZZLE_A:
			if(clientVersion < 3 || !texture->setSwizzleA((GLenum)param))
			{
				return error(GL_INVALID_VALUE);
			}
			break;
		default:
			return error(GL_INVALID_ENUM);
		}
	}
}

void TexParameterfv(GLenum target, GLenum pname, const GLfloat* params)
{
	glTexParameterf(target, pname, *params);
}

void TexParameteri(GLenum target, GLenum pname, GLint param)
{
	TRACE("(GLenum target = 0x%X, GLenum pname = 0x%X, GLint param = %d)", target, pname, param);

	es2::Context *context = es2::getContext();

	if(context)
	{
		es2::Texture *texture;

		GLint clientVersion = context->getClientVersion();

		switch(target)
		{
		case GL_TEXTURE_2D:
			texture = context->getTexture2D();
			break;
		case GL_TEXTURE_2D_ARRAY:
			if(clientVersion < 3)
			{
				return error(GL_INVALID_ENUM);
			}
			else
			{
				texture = context->getTexture2DArray();
			}
			break;
		case GL_TEXTURE_3D_OES:
			texture = context->getTexture3D();
			break;
		case GL_TEXTURE_CUBE_MAP:
			texture = context->getTextureCubeMap();
			break;
		case GL_TEXTURE_EXTERNAL_OES:
			texture = context->getTextureExternal();
			break;
		default:
			return error(GL_INVALID_ENUM);
		}

		switch(pname)
		{
		case GL_TEXTURE_WRAP_S:
			if(!texture->setWrapS((GLenum)param))
			{
				return error(GL_INVALID_ENUM);
			}
			break;
		case GL_TEXTURE_WRAP_T:
			if(!texture->setWrapT((GLenum)param))
			{
				return error(GL_INVALID_ENUM);
			}
			break;
		case GL_TEXTURE_WRAP_R_OES:
			if(!texture->setWrapR((GLenum)param))
			{
				return error(GL_INVALID_ENUM);
			}
			break;
		case GL_TEXTURE_MIN_FILTER:
			if(!texture->setMinFilter((GLenum)param))
			{
				return error(GL_INVALID_ENUM);
			}
			break;
		case GL_TEXTURE_MAG_FILTER:
			if(!texture->setMagFilter((GLenum)param))
			{
				return error(GL_INVALID_ENUM);
			}
			break;
		case GL_TEXTURE_MAX_ANISOTROPY_EXT:
			if(!texture->setMaxAnisotropy((GLfloat)param))
			{
				return error(GL_INVALID_VALUE);
			}
			break;
		case GL_TEXTURE_BASE_LEVEL:
			if(clientVersion < 3 || !texture->setBaseLevel(param))
			{
				return error(GL_INVALID_VALUE);
			}
			break;
		case GL_TEXTURE_COMPARE_FUNC:
			if(clientVersion < 3 || !texture->setCompareFunc((GLenum)param))
			{
				return error(GL_INVALID_VALUE);
			}
			break;
		case GL_TEXTURE_COMPARE_MODE:
			if(clientVersion < 3 || !texture->setCompareMode((GLenum)param))
			{
				return error(GL_INVALID_VALUE);
			}
			break;
		case GL_TEXTURE_IMMUTABLE_FORMAT:
			if(clientVersion < 3 || !texture->setImmutableFormat((GLboolean)param))
			{
				return error(GL_INVALID_VALUE);
			}
			break;
		case GL_TEXTURE_MAX_LEVEL:
			if(clientVersion < 3 || !texture->setMaxLevel(param))
			{
				return error(GL_INVALID_VALUE);
			}
			break;
		case GL_TEXTURE_MAX_LOD:
			if(clientVersion < 3 || !texture->setMaxLOD((GLfloat)param))
			{
				return error(GL_INVALID_VALUE);
			}
			break;
		case GL_TEXTURE_MIN_LOD:
			if(clientVersion < 3 || !texture->setMinLOD((GLfloat)param))
			{
				return error(GL_INVALID_VALUE);
			}
			break;
		case GL_TEXTURE_SWIZZLE_R:
			if(clientVersion < 3 || !texture->setSwizzleR((GLenum)param))
			{
				return error(GL_INVALID_VALUE);
			}
			break;
		case GL_TEXTURE_SWIZZLE_G:
			if(clientVersion < 3 || !texture->setSwizzleG((GLenum)param))
			{
				return error(GL_INVALID_VALUE);
			}
			break;
		case GL_TEXTURE_SWIZZLE_B:
			if(clientVersion < 3 || !texture->setSwizzleB((GLenum)param))
			{
				return error(GL_INVALID_VALUE);
			}
			break;
		case GL_TEXTURE_SWIZZLE_A:
			if(clientVersion < 3 || !texture->setSwizzleA((GLenum)param))
			{
				return error(GL_INVALID_VALUE);
			}
			break;
		default:
			return error(GL_INVALID_ENUM);
		}
	}
}

void TexParameteriv(GLenum target, GLenum pname, const GLint* params)
{
	glTexParameteri(target, pname, *params);
}

void TexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height,
                   GLenum format, GLenum type, const GLvoid* pixels)
{
	TRACE("(GLenum target = 0x%X, GLint level = %d, GLint xoffset = %d, GLint yoffset = %d, "
	      "GLsizei width = %d, GLsizei height = %d, GLenum format = 0x%X, GLenum type = 0x%X, "
	      "const GLvoid* pixels = %p)",
	      target, level, xoffset, yoffset, width, height, format, type, pixels);

	if(!es2::IsTextureTarget(target))
	{
		return error(GL_INVALID_ENUM);
	}

	if(level < 0 || xoffset < 0 || yoffset < 0 || width < 0 || height < 0)
	{
		return error(GL_INVALID_VALUE);
	}

	if(std::numeric_limits<GLsizei>::max() - xoffset < width || std::numeric_limits<GLsizei>::max() - yoffset < height)
	{
		return error(GL_INVALID_VALUE);
	}

	if(!es2::CheckTextureFormatType(format, type, egl::getClientVersion()))
	{
		return error(GL_INVALID_ENUM);
	}

	if(width == 0 || height == 0 || pixels == NULL)
	{
		return;
	}

	es2::Context *context = es2::getContext();

	if(context)
	{
		if(level >= es2::IMPLEMENTATION_MAX_TEXTURE_LEVELS)
		{
			return error(GL_INVALID_VALUE);
		}

		GLenum sizedInternalFormat = GetSizedInternalFormat(format, type);

		if(target == GL_TEXTURE_2D)
		{
			es2::Texture2D *texture = context->getTexture2D();

			GLenum validationError = ValidateSubImageParams(false, width, height, xoffset, yoffset, target, level, sizedInternalFormat, texture);

			if(validationError == GL_NONE)
			{
				texture->subImage(level, xoffset, yoffset, width, height, sizedInternalFormat, type, context->getUnpackInfo(), pixels);
			}
			else
			{
				return error(validationError);
			}
		}
		else if(es2::IsCubemapTextureTarget(target))
		{
			es2::TextureCubeMap *texture = context->getTextureCubeMap();

			GLenum validationError = ValidateSubImageParams(false, width, height, xoffset, yoffset, target, level, sizedInternalFormat, texture);

			if(validationError == GL_NONE)
			{
				texture->subImage(target, level, xoffset, yoffset, width, height, sizedInternalFormat, type, context->getUnpackInfo(), pixels);
			}
			else
			{
				return error(validationError);
			}
		}
		else UNREACHABLE(target);
	}
}

void Uniform1f(GLint location, GLfloat x)
{
	glUniform1fv(location, 1, &x);
}

void Uniform1fv(GLint location, GLsizei count, const GLfloat* v)
{
	TRACE("(GLint location = %d, GLsizei count = %d, const GLfloat* v = %p)", location, count, v);

	if(count < 0)
	{
		return error(GL_INVALID_VALUE);
	}

	if(location == -1)
	{
		return;
	}

	es2::Context *context = es2::getContext();

	if(context)
	{
		es2::Program *program = context->getCurrentProgram();

		if(!program)
		{
			return error(GL_INVALID_OPERATION);
		}

		if(!program->setUniform1fv(location, count, v))
		{
			return error(GL_INVALID_OPERATION);
		}
	}
}

void Uniform1i(GLint location, GLint x)
{
	glUniform1iv(location, 1, &x);
}

void Uniform1iv(GLint location, GLsizei count, const GLint* v)
{
	TRACE("(GLint location = %d, GLsizei count = %d, const GLint* v = %p)", location, count, v);

	if(count < 0)
	{
		return error(GL_INVALID_VALUE);
	}

	if(location == -1)
	{
		return;
	}

	es2::Context *context = es2::getContext();

	if(context)
	{
		es2::Program *program = context->getCurrentProgram();

		if(!program)
		{
			return error(GL_INVALID_OPERATION);
		}

		if(!program->setUniform1iv(location, count, v))
		{
			return error(GL_INVALID_OPERATION);
		}
	}
}

void Uniform2f(GLint location, GLfloat x, GLfloat y)
{
	GLfloat xy[2] = {x, y};

	glUniform2fv(location, 1, (GLfloat*)&xy);
}

void Uniform2fv(GLint location, GLsizei count, const GLfloat* v)
{
	TRACE("(GLint location = %d, GLsizei count = %d, const GLfloat* v = %p)", location, count, v);

	if(count < 0)
	{
		return error(GL_INVALID_VALUE);
	}

	if(location == -1)
	{
		return;
	}

	es2::Context *context = es2::getContext();

	if(context)
	{
		es2::Program *program = context->getCurrentProgram();

		if(!program)
		{
			return error(GL_INVALID_OPERATION);
		}

		if(!program->setUniform2fv(location, count, v))
		{
			return error(GL_INVALID_OPERATION);
		}
	}
}

void Uniform2i(GLint location, GLint x, GLint y)
{
	GLint xy[4] = {x, y};

	glUniform2iv(location, 1, (GLint*)&xy);
}

void Uniform2iv(GLint location, GLsizei count, const GLint* v)
{
	TRACE("(GLint location = %d, GLsizei count = %d, const GLint* v = %p)", location, count, v);

	if(count < 0)
	{
		return error(GL_INVALID_VALUE);
	}

	if(location == -1)
	{
		return;
	}

	es2::Context *context = es2::getContext();

	if(context)
	{
		es2::Program *program = context->getCurrentProgram();

		if(!program)
		{
			return error(GL_INVALID_OPERATION);
		}

		if(!program->setUniform2iv(location, count, v))
		{
			return error(GL_INVALID_OPERATION);
		}
	}
}

void Uniform3f(GLint location, GLfloat x, GLfloat y, GLfloat z)
{
	GLfloat xyz[3] = {x, y, z};

	glUniform3fv(location, 1, (GLfloat*)&xyz);
}

void Uniform3fv(GLint location, GLsizei count, const GLfloat* v)
{
	TRACE("(GLint location = %d, GLsizei count = %d, const GLfloat* v = %p)", location, count, v);

	if(count < 0)
	{
		return error(GL_INVALID_VALUE);
	}

	if(location == -1)
	{
		return;
	}

	es2::Context *context = es2::getContext();

	if(context)
	{
		es2::Program *program = context->getCurrentProgram();

		if(!program)
		{
			return error(GL_INVALID_OPERATION);
		}

		if(!program->setUniform3fv(location, count, v))
		{
			return error(GL_INVALID_OPERATION);
		}
	}
}

void Uniform3i(GLint location, GLint x, GLint y, GLint z)
{
	GLint xyz[3] = {x, y, z};

	glUniform3iv(location, 1, (GLint*)&xyz);
}

void Uniform3iv(GLint location, GLsizei count, const GLint* v)
{
	TRACE("(GLint location = %d, GLsizei count = %d, const GLint* v = %p)", location, count, v);

	if(count < 0)
	{
		return error(GL_INVALID_VALUE);
	}

	if(location == -1)
	{
		return;
	}

	es2::Context *context = es2::getContext();

	if(context)
	{
		es2::Program *program = context->getCurrentProgram();

		if(!program)
		{
			return error(GL_INVALID_OPERATION);
		}

		if(!program->setUniform3iv(location, count, v))
		{
			return error(GL_INVALID_OPERATION);
		}
	}
}

void Uniform4f(GLint location, GLfloat x, GLfloat y, GLfloat z, GLfloat w)
{
	GLfloat xyzw[4] = {x, y, z, w};

	glUniform4fv(location, 1, (GLfloat*)&xyzw);
}

void Uniform4fv(GLint location, GLsizei count, const GLfloat* v)
{
	TRACE("(GLint location = %d, GLsizei count = %d, const GLfloat* v = %p)", location, count, v);

	if(count < 0)
	{
		return error(GL_INVALID_VALUE);
	}

	if(location == -1)
	{
		return;
	}

	es2::Context *context = es2::getContext();

	if(context)
	{
		es2::Program *program = context->getCurrentProgram();

		if(!program)
		{
			return error(GL_INVALID_OPERATION);
		}

		if(!program->setUniform4fv(location, count, v))
		{
			return error(GL_INVALID_OPERATION);
		}
	}
}

void Uniform4i(GLint location, GLint x, GLint y, GLint z, GLint w)
{
	GLint xyzw[4] = {x, y, z, w};

	glUniform4iv(location, 1, (GLint*)&xyzw);
}

void Uniform4iv(GLint location, GLsizei count, const GLint* v)
{
	TRACE("(GLint location = %d, GLsizei count = %d, const GLint* v = %p)", location, count, v);

	if(count < 0)
	{
		return error(GL_INVALID_VALUE);
	}

	if(location == -1)
	{
		return;
	}

	es2::Context *context = es2::getContext();

	if(context)
	{
		es2::Program *program = context->getCurrentProgram();

		if(!program)
		{
			return error(GL_INVALID_OPERATION);
		}

		if(!program->setUniform4iv(location, count, v))
		{
			return error(GL_INVALID_OPERATION);
		}
	}
}

void UniformMatrix2fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value)
{
	TRACE("(GLint location = %d, GLsizei count = %d, GLboolean transpose = %d, const GLfloat* value = %p)",
	      location, count, transpose, value);

	if(count < 0)
	{
		return error(GL_INVALID_VALUE);
	}

	if(location == -1)
	{
		return;
	}

	es2::Context *context = es2::getContext();

	if(context)
	{
		if(context->getClientVersion() < 3 && transpose != GL_FALSE)
		{
			return error(GL_INVALID_VALUE);
		}

		es2::Program *program = context->getCurrentProgram();

		if(!program)
		{
			return error(GL_INVALID_OPERATION);
		}

		if(!program->setUniformMatrix2fv(location, count, transpose, value))
		{
			return error(GL_INVALID_OPERATION);
		}
	}
}

void UniformMatrix3fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value)
{
	TRACE("(GLint location = %d, GLsizei count = %d, GLboolean transpose = %d, const GLfloat* value = %p)",
	      location, count, transpose, value);

	if(count < 0)
	{
		return error(GL_INVALID_VALUE);
	}

	if(location == -1)
	{
		return;
	}

	es2::Context *context = es2::getContext();

	if(context)
	{
		if(context->getClientVersion() < 3 && transpose != GL_FALSE)
		{
			return error(GL_INVALID_VALUE);
		}

		es2::Program *program = context->getCurrentProgram();

		if(!program)
		{
			return error(GL_INVALID_OPERATION);
		}

		if(!program->setUniformMatrix3fv(location, count, transpose, value))
		{
			return error(GL_INVALID_OPERATION);
		}
	}
}

void UniformMatrix4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value)
{
	TRACE("(GLint location = %d, GLsizei count = %d, GLboolean transpose = %d, const GLfloat* value = %p)",
	      location, count, transpose, value);

	if(count < 0)
	{
		return error(GL_INVALID_VALUE);
	}

	if(location == -1)
	{
		return;
	}

	es2::Context *context = es2::getContext();

	if(context)
	{
		if(context->getClientVersion() < 3 && transpose != GL_FALSE)
		{
			return error(GL_INVALID_VALUE);
		}

		es2::Program *program = context->getCurrentProgram();

		if(!program)
		{
			return error(GL_INVALID_OPERATION);
		}

		if(!program->setUniformMatrix4fv(location, count, transpose, value))
		{
			return error(GL_INVALID_OPERATION);
		}
	}
}

void UseProgram(GLuint program)
{
	TRACE("(GLuint program = %d)", program);

	es2::Context *context = es2::getContext();

	if(context)
	{
		es2::Program *programObject = context->getProgram(program);

		if(!programObject && program != 0)
		{
			if(context->getShader(program))
			{
				return error(GL_INVALID_OPERATION);
			}
			else
			{
				return error(GL_INVALID_VALUE);
			}
		}

		if(program != 0 && !programObject->isLinked())
		{
			return error(GL_INVALID_OPERATION);
		}

		context->useProgram(program);
	}
}

void ValidateProgram(GLuint program)
{
	TRACE("(GLuint program = %d)", program);

	es2::Context *context = es2::getContext();

	if(context)
	{
		es2::Program *programObject = context->getProgram(program);

		if(!programObject)
		{
			if(context->getShader(program))
			{
				return error(GL_INVALID_OPERATION);
			}
			else
			{
				return error(GL_INVALID_VALUE);
			}
		}

		programObject->validate();
	}
}

void VertexAttrib1f(GLuint index, GLfloat x)
{
	TRACE("(GLuint index = %d, GLfloat x = %f)", index, x);

	if(index >= es2::MAX_VERTEX_ATTRIBS)
	{
		return error(GL_INVALID_VALUE);
	}

	es2::Context *context = es2::getContext();

	if(context)
	{
		GLfloat vals[4] = { x, 0, 0, 1 };
		context->setVertexAttrib(index, vals);
	}
}

void VertexAttrib1fv(GLuint index, const GLfloat* values)
{
	TRACE("(GLuint index = %d, const GLfloat* values = %p)", index, values);

	if(index >= es2::MAX_VERTEX_ATTRIBS)
	{
		return error(GL_INVALID_VALUE);
	}

	es2::Context *context = es2::getContext();

	if(context)
	{
		GLfloat vals[4] = { values[0], 0, 0, 1 };
		context->setVertexAttrib(index, vals);
	}
}

void VertexAttrib2f(GLuint index, GLfloat x, GLfloat y)
{
	TRACE("(GLuint index = %d, GLfloat x = %f, GLfloat y = %f)", index, x, y);

	if(index >= es2::MAX_VERTEX_ATTRIBS)
	{
		return error(GL_INVALID_VALUE);
	}

	es2::Context *context = es2::getContext();

	if(context)
	{
		GLfloat vals[4] = { x, y, 0, 1 };
		context->setVertexAttrib(index, vals);
	}
}

void VertexAttrib2fv(GLuint index, const GLfloat* values)
{
	TRACE("(GLuint index = %d, const GLfloat* values = %p)", index, values);

	if(index >= es2::MAX_VERTEX_ATTRIBS)
	{
		return error(GL_INVALID_VALUE);
	}

	es2::Context *context = es2::getContext();

	if(context)
	{
		GLfloat vals[4] = { values[0], values[1], 0, 1 };
		context->setVertexAttrib(index, vals);
	}
}

void VertexAttrib3f(GLuint index, GLfloat x, GLfloat y, GLfloat z)
{
	TRACE("(GLuint index = %d, GLfloat x = %f, GLfloat y = %f, GLfloat z = %f)", index, x, y, z);

	if(index >= es2::MAX_VERTEX_ATTRIBS)
	{
		return error(GL_INVALID_VALUE);
	}

	es2::Context *context = es2::getContext();

	if(context)
	{
		GLfloat vals[4] = { x, y, z, 1 };
		context->setVertexAttrib(index, vals);
	}
}

void VertexAttrib3fv(GLuint index, const GLfloat* values)
{
	TRACE("(GLuint index = %d, const GLfloat* values = %p)", index, values);

	if(index >= es2::MAX_VERTEX_ATTRIBS)
	{
		return error(GL_INVALID_VALUE);
	}

	es2::Context *context = es2::getContext();

	if(context)
	{
		GLfloat vals[4] = { values[0], values[1], values[2], 1 };
		context->setVertexAttrib(index, vals);
	}
}

void VertexAttrib4f(GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w)
{
	TRACE("(GLuint index = %d, GLfloat x = %f, GLfloat y = %f, GLfloat z = %f, GLfloat w = %f)", index, x, y, z, w);

	if(index >= es2::MAX_VERTEX_ATTRIBS)
	{
		return error(GL_INVALID_VALUE);
	}

	es2::Context *context = es2::getContext();

	if(context)
	{
		GLfloat vals[4] = { x, y, z, w };
		context->setVertexAttrib(index, vals);
	}
}

void VertexAttrib4fv(GLuint index, const GLfloat* values)
{
	TRACE("(GLuint index = %d, const GLfloat* values = %p)", index, values);

	if(index >= es2::MAX_VERTEX_ATTRIBS)
	{
		return error(GL_INVALID_VALUE);
	}

	es2::Context *context = es2::getContext();

	if(context)
	{
		context->setVertexAttrib(index, values);
	}
}

void VertexAttribPointer(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid* ptr)
{
	TRACE("(GLuint index = %d, GLint size = %d, GLenum type = 0x%X, "
	      "GLboolean normalized = %d, GLsizei stride = %d, const GLvoid* ptr = %p)",
	      index, size, type, normalized, stride, ptr);

	if(index >= es2::MAX_VERTEX_ATTRIBS)
	{
		return error(GL_INVALID_VALUE);
	}

	if(size < 1 || size > 4)
	{
		return error(GL_INVALID_VALUE);
	}

	GLint clientVersion = egl::getClientVersion();

	switch(type)
	{
	case GL_BYTE:
	case GL_UNSIGNED_BYTE:
	case GL_SHORT:
	case GL_UNSIGNED_SHORT:
	case GL_FIXED:
	case GL_FLOAT:
		break;
	case GL_INT_2_10_10_10_REV:
	case GL_UNSIGNED_INT_2_10_10_10_REV:
		if(clientVersion >= 3)
		{
			if(size != 4)
			{
				return error(GL_INVALID_OPERATION);
			}
			break;
		}
		else return error(GL_INVALID_ENUM);
	case GL_INT:
	case GL_UNSIGNED_INT:
	case GL_HALF_FLOAT:
		if(clientVersion >= 3)
		{
			break;
		}
		else return error(GL_INVALID_ENUM);
	default:
		return error(GL_INVALID_ENUM);
	}

	if(stride < 0)
	{
		return error(GL_INVALID_VALUE);
	}

	es2::Context *context = es2::getContext();

	if(context)
	{
		context->setVertexAttribState(index, context->getArrayBuffer(), size, type, (normalized == GL_TRUE), stride, ptr);
	}
}

void Viewport(GLint x, GLint y, GLsizei width, GLsizei height)
{
	TRACE("(GLint x = %d, GLint y = %d, GLsizei width = %d, GLsizei height = %d)", x, y, width, height);

	if(width < 0 || height < 0)
	{
		return error(GL_INVALID_VALUE);
	}

	es2::Context *context = es2::getContext();

	if(context)
	{
		context->setViewportParams(x, y, width, height);
	}
}

void BlitFramebufferNV(GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter)
{
	TRACE("(GLint srcX0 = %d, GLint srcY0 = %d, GLint srcX1 = %d, GLint srcY1 = %d, "
	      "GLint dstX0 = %d, GLint dstY0 = %d, GLint dstX1 = %d, GLint dstY1 = %d, "
	      "GLbitfield mask = 0x%X, GLenum filter = 0x%X)",
	      srcX0, srcY0, srcX1, srcX1, dstX0, dstY0, dstX1, dstY1, mask, filter);

	switch(filter)
	{
	case GL_NEAREST:
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

void BlitFramebufferANGLE(GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1,
                          GLbitfield mask, GLenum filter)
{
	if(srcX1 - srcX0 != dstX1 - dstX0 || srcY1 - srcY0 != dstY1 - dstY0)
	{
		ERR("Scaling and flipping in BlitFramebufferANGLE not supported by this implementation");
		return error(GL_INVALID_OPERATION);
	}

	glBlitFramebufferNV(srcX0, srcY0, srcX1, srcY1, dstX0, dstY0, dstX1, dstY1, mask, filter);
}

void TexImage3DOES(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth,
                   GLint border, GLenum format, GLenum type, const GLvoid* pixels)
{
	TRACE("(GLenum target = 0x%X, GLint level = %d, GLenum internalformat = 0x%X, "
	      "GLsizei width = %d, GLsizei height = %d, GLsizei depth = %d, GLint border = %d, "
	      "GLenum format = 0x%X, GLenum type = 0x%x, const GLvoid* pixels = %p)",
	      target, level, internalformat, width, height, depth, border, format, type, pixels);

	switch(target)
	{
	case GL_TEXTURE_3D_OES:
		switch(format)
		{
		case GL_DEPTH_COMPONENT:
		case GL_DEPTH_STENCIL_OES:
			return error(GL_INVALID_OPERATION);
		default:
			break;
		}
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

		texture->setImage(level, width, height, depth, GetSizedInternalFormat(internalformat, type), type, context->getUnpackInfo(), pixels);
	}
}

void TexSubImage3DOES(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const void *pixels)
{
	TRACE("(GLenum target = 0x%X, GLint level = %d, GLint xoffset = %d, GLint yoffset = %d, "
	      "GLint zoffset = %d, GLsizei width = %d, GLsizei height = %d, GLsizei depth = %d, "
	      "GLenum format = 0x%X, GLenum type = 0x%x, const GLvoid* pixels = %p)",
	      target, level, xoffset, yoffset, zoffset, width, height, depth, format, type, pixels);

	switch(target)
	{
	case GL_TEXTURE_3D_OES:
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

		GLenum sizedInternalFormat = GetSizedInternalFormat(format, type);

		GLenum validationError = ValidateSubImageParams(false, width, height, depth, xoffset, yoffset, zoffset, target, level, sizedInternalFormat, texture);
		if(validationError == GL_NONE)
		{
			texture->subImage(level, xoffset, yoffset, zoffset, width, height, depth, sizedInternalFormat, type, context->getUnpackInfo(), pixels);
		}
		else
		{
			return error(validationError);
		}
	}
}

void CopyTexSubImage3DOES(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLint x, GLint y, GLsizei width, GLsizei height)
{
	TRACE("(GLenum target = 0x%X, GLint level = %d, GLint xoffset = %d, GLint yoffset = %d, "
	      "GLint zoffset = %d, GLint x = %d, GLint y = %d, GLsizei width = %d, GLsizei height = %d)",
	      target, level, xoffset, yoffset, zoffset, x, y, width, height);

	switch(target)
	{
	case GL_TEXTURE_3D_OES:
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

		es2::Renderbuffer *source = framebuffer->getReadColorbuffer();

		if(context->getReadFramebufferName() != 0 && (!source || source->getSamples() > 1))
		{
			return error(GL_INVALID_OPERATION);
		}

		es2::Texture3D *texture = context->getTexture3D();

		GLenum validationError = ValidateSubImageParams(false, width, height, 1, xoffset, yoffset, zoffset, target, level, GL_NONE, texture);

		if(validationError != GL_NONE)
		{
			return error(validationError);
		}

		texture->copySubImage(target, level, xoffset, yoffset, zoffset, x, y, width, height, framebuffer);
	}
}

void CompressedTexImage3DOES(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLsizei imageSize, const void *data)
{
	TRACE("(GLenum target = 0x%X, GLint level = %d, GLenum internalformat = 0x%X, GLsizei width = %d, "
	      "GLsizei height = %d, GLsizei depth = %d, GLint border = %d, GLsizei imageSize = %d, const GLvoid* data = %p)",
	      target, level, internalformat, width, height, depth, border, imageSize, data);

	switch(target)
	{
	case GL_TEXTURE_3D_OES:
		break;
	default:
		return error(GL_INVALID_ENUM);
	}

	if((level < 0) || (level >= es2::IMPLEMENTATION_MAX_TEXTURE_LEVELS))
	{
		return error(GL_INVALID_VALUE);
	}

	const GLsizei maxSize3D = es2::IMPLEMENTATION_MAX_TEXTURE_SIZE >> level;
	if((width < 0) || (height < 0) || (depth < 0) || (width > maxSize3D) || (height > maxSize3D) || (depth > maxSize3D) ||(border != 0) || (imageSize < 0))
	{
		return error(GL_INVALID_VALUE);
	}

	switch(internalformat)
	{
	case GL_DEPTH_COMPONENT:
	case GL_DEPTH_COMPONENT16:
	case GL_DEPTH_COMPONENT32_OES:
	case GL_DEPTH_STENCIL_OES:
	case GL_DEPTH24_STENCIL8_OES:
		return error(GL_INVALID_OPERATION);
	default:
		{
			GLenum validationError = ValidateCompressedFormat(internalformat, egl::getClientVersion(), true);
			if(validationError != GL_NONE)
			{
				return error(validationError);
			}
		}
	}

	if(imageSize != egl::ComputeCompressedSize(width, height, internalformat) * depth)
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

void CompressedTexSubImage3DOES(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, const void *data)
{
	TRACE("(GLenum target = 0x%X, GLint level = %d, GLint xoffset = %d, GLint yoffset = %d, "
	      "GLint zoffset = %d, GLsizei width = %d, GLsizei height = %d, GLsizei depth = %d, "
	      "GLenum format = 0x%X, GLsizei imageSize = %d, const void *data = %p)",
	      target, level, xoffset, yoffset, zoffset, width, height, depth, format, imageSize, data);

	switch(target)
	{
	case GL_TEXTURE_3D_OES:
		break;
	default:
		return error(GL_INVALID_ENUM);
	}

	if(xoffset < 0 || yoffset < 0 || zoffset < 0 || !validImageSize(level, width, height) || depth < 0 || imageSize < 0)
	{
		return error(GL_INVALID_VALUE);
	}

	GLenum validationError = ValidateCompressedFormat(format, egl::getClientVersion(), true);
	if(validationError != GL_NONE)
	{
		return error(validationError);
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

void FramebufferTexture3DOES(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level, GLint zoffset)
{
	TRACE("(GLenum target = 0x%X, GLenum attachment = 0x%X, GLenum textarget = 0x%X, "
	      "GLuint texture = %d, GLint level = %d, GLint zoffset = %d)", target, attachment, textarget, texture, level, zoffset);

	if(target != GL_FRAMEBUFFER && target != GL_DRAW_FRAMEBUFFER_ANGLE && target != GL_READ_FRAMEBUFFER_ANGLE)
	{
		return error(GL_INVALID_ENUM);
	}

	es2::Context *context = es2::getContext();

	if(context)
	{
		if(texture == 0)
		{
			textarget = GL_NONE;
		}
		else
		{
			es2::Texture *tex = context->getTexture(texture);

			if(tex == NULL)
			{
				return error(GL_INVALID_OPERATION);
			}

			if(tex->isCompressed(textarget, level))
			{
				return error(GL_INVALID_OPERATION);
			}

			switch(textarget)
			{
			case GL_TEXTURE_3D_OES:
				if(tex->getTarget() != GL_TEXTURE_3D_OES)
				{
					return error(GL_INVALID_OPERATION);
				}
				break;
			default:
				return error(GL_INVALID_ENUM);
			}

			if(level != 0)
			{
				return error(GL_INVALID_VALUE);
			}
		}

		es2::Framebuffer *framebuffer = NULL;
		GLuint framebufferName = 0;
		if(target == GL_READ_FRAMEBUFFER_ANGLE)
		{
			framebuffer = context->getReadFramebuffer();
			framebufferName = context->getReadFramebufferName();
		}
		else
		{
			framebuffer = context->getDrawFramebuffer();
			framebufferName = context->getDrawFramebufferName();
		}

		if(framebufferName == 0 || !framebuffer)
		{
			return error(GL_INVALID_OPERATION);
		}

		GLint clientVersion = context->getClientVersion();

		switch(attachment)
		{
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
			if(clientVersion < 3)
			{
				return error(GL_INVALID_ENUM);
			}
			// fall through
		case GL_COLOR_ATTACHMENT0:
			if((attachment - GL_COLOR_ATTACHMENT0) >= es2::IMPLEMENTATION_MAX_COLOR_ATTACHMENTS)
			{
				return error(GL_INVALID_ENUM);
			}
			framebuffer->setColorbuffer(textarget, texture, attachment - GL_COLOR_ATTACHMENT0);
			break;
		case GL_DEPTH_ATTACHMENT:   framebuffer->setDepthbuffer(textarget, texture);   break;
		case GL_STENCIL_ATTACHMENT: framebuffer->setStencilbuffer(textarget, texture); break;
		default:
			return error(GL_INVALID_ENUM);
		}
	}
}

void EGLImageTargetTexture2DOES(GLenum target, GLeglImageOES image)
{
	if(egl::getClientVersion() == 1)
	{
		return libGLES_CM->glEGLImageTargetTexture2DOES(target, image);
	}

	TRACE("(GLenum target = 0x%X, GLeglImageOES image = %p)", target, image);

	switch(target)
	{
	case GL_TEXTURE_2D:
	case GL_TEXTURE_EXTERNAL_OES:
		break;
	default:
		return error(GL_INVALID_ENUM);
	}

	if(!image)
	{
		return error(GL_INVALID_OPERATION);
	}

	es2::Context *context = es2::getContext();

	if(context)
	{
		es2::Texture2D *texture = 0;

		switch(target)
		{
		case GL_TEXTURE_2D:           texture = context->getTexture2D();       break;
		case GL_TEXTURE_EXTERNAL_OES: texture = context->getTextureExternal(); break;
		default:                      UNREACHABLE(target);
		}

		if(!texture)
		{
			return error(GL_INVALID_OPERATION);
		}

		egl::Image *glImage = static_cast<egl::Image*>(image);

		texture->setImage(glImage);
	}
}

void EGLImageTargetRenderbufferStorageOES(GLenum target, GLeglImageOES image)
{
	TRACE("(GLenum target = 0x%X, GLeglImageOES image = %p)", target, image);

	UNIMPLEMENTED();
}

GLboolean IsRenderbufferOES(GLuint renderbuffer)
{
	return IsRenderbuffer(renderbuffer);
}

void BindRenderbufferOES(GLenum target, GLuint renderbuffer)
{
	BindRenderbuffer(target, renderbuffer);
}

void DeleteRenderbuffersOES(GLsizei n, const GLuint* renderbuffers)
{
	DeleteRenderbuffers(n, renderbuffers);
}

void GenRenderbuffersOES(GLsizei n, GLuint* renderbuffers)
{
	GenRenderbuffers(n, renderbuffers);
}

void RenderbufferStorageOES(GLenum target, GLenum internalformat, GLsizei width, GLsizei height)
{
	RenderbufferStorage(target, internalformat, width, height);
}

void GetRenderbufferParameterivOES(GLenum target, GLenum pname, GLint* params)
{
	GetRenderbufferParameteriv(target, pname, params);
}

GLboolean IsFramebufferOES(GLuint framebuffer)
{
	return IsFramebuffer(framebuffer);
}

void BindFramebufferOES(GLenum target, GLuint framebuffer)
{
	BindFramebuffer(target, framebuffer);
}

void DeleteFramebuffersOES(GLsizei n, const GLuint* framebuffers)
{
	DeleteFramebuffers(n, framebuffers);
}

void GenFramebuffersOES(GLsizei n, GLuint* framebuffers)
{
	GenFramebuffers(n, framebuffers);
}

GLenum CheckFramebufferStatusOES(GLenum target)
{
	return CheckFramebufferStatus(target);
}

void FramebufferRenderbufferOES(GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer)
{
	FramebufferRenderbuffer(target, attachment, renderbuffertarget, renderbuffer);
}

void FramebufferTexture2DOES(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level)
{
	FramebufferTexture2D(target, attachment, textarget, texture, level);
}

void GetFramebufferAttachmentParameterivOES(GLenum target, GLenum attachment, GLenum pname, GLint* params)
{
	GetFramebufferAttachmentParameteriv(target, attachment, pname, params);
}

void GenerateMipmapOES(GLenum target)
{
	GenerateMipmap(target);
}

}

extern "C" __eglMustCastToProperFunctionPointerType es2GetProcAddress(const char *procname)
{
	struct Extension
	{
		const char *name;
		__eglMustCastToProperFunctionPointerType address;
	};

	static const Extension glExtensions[] =
	{
		#define EXTENSION(name) {#name, (__eglMustCastToProperFunctionPointerType)name}

		EXTENSION(glTexImage3DOES),
		EXTENSION(glBlitFramebufferANGLE),
		EXTENSION(glBlitFramebufferNV),
		EXTENSION(glRenderbufferStorageMultisampleANGLE),
		EXTENSION(glDeleteFencesNV),
		EXTENSION(glGenFencesNV),
		EXTENSION(glIsFenceNV),
		EXTENSION(glTestFenceNV),
		EXTENSION(glGetFenceivNV),
		EXTENSION(glFinishFenceNV),
		EXTENSION(glSetFenceNV),
		EXTENSION(glGetGraphicsResetStatusEXT),
		EXTENSION(glReadnPixelsEXT),
		EXTENSION(glGetnUniformfvEXT),
		EXTENSION(glGetnUniformivEXT),
		EXTENSION(glGenQueriesEXT),
		EXTENSION(glDeleteQueriesEXT),
		EXTENSION(glIsQueryEXT),
		EXTENSION(glBeginQueryEXT),
		EXTENSION(glEndQueryEXT),
		EXTENSION(glGetQueryivEXT),
		EXTENSION(glGetQueryObjectuivEXT),
		EXTENSION(glEGLImageTargetTexture2DOES),
		EXTENSION(glEGLImageTargetRenderbufferStorageOES),
		EXTENSION(glDrawElementsInstancedEXT),
		EXTENSION(glDrawArraysInstancedEXT),
		EXTENSION(glVertexAttribDivisorEXT),
		EXTENSION(glDrawArraysInstancedANGLE),
		EXTENSION(glDrawElementsInstancedANGLE),
		EXTENSION(glVertexAttribDivisorANGLE),
		EXTENSION(glIsRenderbufferOES),
		EXTENSION(glBindRenderbufferOES),
		EXTENSION(glDeleteRenderbuffersOES),
		EXTENSION(glGenRenderbuffersOES),
		EXTENSION(glRenderbufferStorageOES),
		EXTENSION(glGetRenderbufferParameterivOES),
		EXTENSION(glIsFramebufferOES),
		EXTENSION(glBindFramebufferOES),
		EXTENSION(glDeleteFramebuffersOES),
		EXTENSION(glGenFramebuffersOES),
		EXTENSION(glCheckFramebufferStatusOES),
		EXTENSION(glFramebufferRenderbufferOES),
		EXTENSION(glFramebufferTexture2DOES),
		EXTENSION(glGetFramebufferAttachmentParameterivOES),
		EXTENSION(glGenerateMipmapOES),

		#undef EXTENSION
	};

	for(unsigned int ext = 0; ext < sizeof(glExtensions) / sizeof(Extension); ext++)
	{
		if(strcmp(procname, glExtensions[ext].name) == 0)
		{
			return (__eglMustCastToProperFunctionPointerType)glExtensions[ext].address;
		}
	}

	return NULL;
}
