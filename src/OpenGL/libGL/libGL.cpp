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
// libGL.cpp: Implements the exported OpenGL functions.

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
#include "Main/Register.hpp"

#define _GDI32_
#include <windows.h>
#include <GL/GL.h>
#include <GL/glext.h>

#include <limits>

static bool validImageSize(GLint level, GLsizei width, GLsizei height)
{
	if(level < 0 || level >= gl::IMPLEMENTATION_MAX_TEXTURE_LEVELS || width < 0 || height < 0)
	{
		return false;
	}

	return true;
}

static bool validateSubImageParams(bool compressed, GLsizei width, GLsizei height, GLint xoffset, GLint yoffset, GLenum target, GLint level, GLenum format, gl::Texture *texture)
{
	if(!texture)
	{
		return error(GL_INVALID_OPERATION, false);
	}

	if(compressed != texture->isCompressed(target, level))
	{
		return error(GL_INVALID_OPERATION, false);
	}

	if(format != GL_NONE && format != texture->getFormat(target, level) && target != GL_TEXTURE_1D)
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

// Check for combinations of format and type that are valid for ReadPixels
static bool validReadFormatType(GLenum format, GLenum type)
{
	switch(format)
	{
	case GL_RGBA:
		switch(type)
		{
		case GL_UNSIGNED_BYTE:
			break;
		default:
			return false;
		}
		break;
	case GL_BGRA_EXT:
		switch(type)
		{
		case GL_UNSIGNED_BYTE:
		case GL_UNSIGNED_SHORT_4_4_4_4_REV:
		case GL_UNSIGNED_SHORT_1_5_5_5_REV:
			break;
		default:
			return false;
		}
		break;
	case gl::IMPLEMENTATION_COLOR_READ_FORMAT:
		switch(type)
		{
		case gl::IMPLEMENTATION_COLOR_READ_TYPE:
			break;
		default:
			return false;
		}
		break;
	default:
		return false;
	}

	return true;
}

extern "C"
{

void APIENTRY glActiveTexture(GLenum texture)
{
	TRACE("(GLenum texture = 0x%X)", texture);

	gl::Context *context = gl::getContext();

	if(context)
	{
		if(context->getListIndex() != 0)
		{
			UNIMPLEMENTED();
		}

		if(texture < GL_TEXTURE0 || texture > GL_TEXTURE0 + gl::MAX_COMBINED_TEXTURE_IMAGE_UNITS - 1)
		{
			return error(GL_INVALID_ENUM);
		}

		context->setActiveSampler(texture - GL_TEXTURE0);
	}
}

void APIENTRY glAttachShader(GLuint program, GLuint shader)
{
	TRACE("(GLuint program = %d, GLuint shader = %d)", program, shader);

	gl::Context *context = gl::getContext();

	if(context)
	{
		gl::Program *programObject = context->getProgram(program);
		gl::Shader *shaderObject = context->getShader(shader);

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

void APIENTRY glBeginQueryEXT(GLenum target, GLuint name)
{
	TRACE("(GLenum target = 0x%X, GLuint name = %d)", target, name);

	switch(target)
	{
	case GL_ANY_SAMPLES_PASSED:
	case GL_ANY_SAMPLES_PASSED_CONSERVATIVE:
		break;
	default:
		return error(GL_INVALID_ENUM);
	}

	if(name == 0)
	{
		return error(GL_INVALID_OPERATION);
	}

	gl::Context *context = gl::getContext();

	if(context)
	{
		if(context->getListIndex() != 0)
		{
			UNIMPLEMENTED();
		}

		context->beginQuery(target, name);
	}
}

void APIENTRY glBindAttribLocation(GLuint program, GLuint index, const GLchar* name)
{
	TRACE("(GLuint program = %d, GLuint index = %d, const GLchar* name = %s)", program, index, name);

	if(index >= gl::MAX_VERTEX_ATTRIBS)
	{
		return error(GL_INVALID_VALUE);
	}

	gl::Context *context = gl::getContext();

	if(context)
	{
		gl::Program *programObject = context->getProgram(program);

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

void APIENTRY glBindBuffer(GLenum target, GLuint buffer)
{
	TRACE("(GLenum target = 0x%X, GLuint buffer = %d)", target, buffer);

	gl::Context *context = gl::getContext();

	if(context)
	{
		switch(target)
		{
		case GL_ARRAY_BUFFER:
			context->bindArrayBuffer(buffer);
			return;
		case GL_ELEMENT_ARRAY_BUFFER:
			context->bindElementArrayBuffer(buffer);
			return;
		default:
			return error(GL_INVALID_ENUM);
		}
	}
}

void APIENTRY glBindFramebuffer(GLenum target, GLuint framebuffer)
{
	TRACE("(GLenum target = 0x%X, GLuint framebuffer = %d)", target, framebuffer);

	if(target != GL_FRAMEBUFFER && target != GL_DRAW_FRAMEBUFFER_EXT && target != GL_READ_FRAMEBUFFER_EXT)
	{
		return error(GL_INVALID_ENUM);
	}

	gl::Context *context = gl::getContext();

	if(context)
	{
		if(context->getListIndex() != 0)
		{
			UNIMPLEMENTED();
		}

		if(target == GL_READ_FRAMEBUFFER_EXT || target == GL_FRAMEBUFFER)
		{
			context->bindReadFramebuffer(framebuffer);
		}
			
		if(target == GL_DRAW_FRAMEBUFFER_EXT || target == GL_FRAMEBUFFER)
		{
			context->bindDrawFramebuffer(framebuffer);
		}
	}
}

void APIENTRY glBindRenderbuffer(GLenum target, GLuint renderbuffer)
{
	TRACE("(GLenum target = 0x%X, GLuint renderbuffer = %d)", target, renderbuffer);

	if(target != GL_RENDERBUFFER)
	{
		return error(GL_INVALID_ENUM);
	}

	gl::Context *context = gl::getContext();

	if(context)
	{
		if(context->getListIndex() != 0)
		{
			UNIMPLEMENTED();
		}

		context->bindRenderbuffer(renderbuffer);
	}
}

void APIENTRY glBindTexture(GLenum target, GLuint texture)
{
	TRACE("(GLenum target = 0x%X, GLuint texture = %d)", target, texture);

	gl::Context *context = gl::getContext();

	if(context)
	{
		if(context->getListIndex() != 0)
		{
			UNIMPLEMENTED();
		}

		gl::Texture *textureObject = context->getTexture(texture);

		if(textureObject && textureObject->getTarget() != target && texture != 0)
		{
			return error(GL_INVALID_OPERATION);
		}

		switch(target)
		{
		case GL_TEXTURE_1D:
			context->bindTexture1D(texture);
			return;
		case GL_TEXTURE_2D:
			context->bindTexture2D(texture);
			return;
		case GL_TEXTURE_CUBE_MAP:
			context->bindTextureCubeMap(texture);
			return;
		default:
			return error(GL_INVALID_ENUM);
		}
	}
}

void APIENTRY glBlendColor(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha)
{
	TRACE("(GLclampf red = %f, GLclampf green = %f, GLclampf blue = %f, GLclampf alpha = %f)",
		  red, green, blue, alpha);

	gl::Context* context = gl::getContext();

	if(context)
	{
		if(context->getListIndex() != 0)
		{
			UNIMPLEMENTED();
		}

		context->setBlendColor(gl::clamp01(red), gl::clamp01(green), gl::clamp01(blue), gl::clamp01(alpha));
	}
}

void APIENTRY glBlendEquation(GLenum mode)
{
	glBlendEquationSeparate(mode, mode);
}

void APIENTRY glBlendEquationSeparate(GLenum modeRGB, GLenum modeAlpha)
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

	gl::Context *context = gl::getContext();

	if(context)
	{
		if(context->getListIndex() != 0)
		{
			UNIMPLEMENTED();
		}

		context->setBlendEquation(modeRGB, modeAlpha);
	}
}

void APIENTRY glBlendFunc(GLenum sfactor, GLenum dfactor)
{
	glBlendFuncSeparate(sfactor, dfactor, sfactor, dfactor);
}

void APIENTRY glBlendFuncSeparate(GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha)
{
	TRACE("(GLenum srcRGB = 0x%X, GLenum dstRGB = 0x%X, GLenum srcAlpha = 0x%X, GLenum dstAlpha = 0x%X)",
	      srcRGB, dstRGB, srcAlpha, dstAlpha);

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
	default:
		return error(GL_INVALID_ENUM);
	}

	gl::Context *context = gl::getContext();

	if(context)
	{
		if(context->getListIndex() != 0)
		{
			UNIMPLEMENTED();
		}

		context->setBlendFactors(srcRGB, dstRGB, srcAlpha, dstAlpha);
	}
}

void APIENTRY glBufferData(GLenum target, GLsizeiptr size, const GLvoid* data, GLenum usage)
{
	TRACE("(GLenum target = 0x%X, GLsizeiptr size = %d, const GLvoid* data = %p, GLenum usage = %d)",
	      target, size, data, usage);

	if(size < 0)
	{
		return error(GL_INVALID_VALUE);
	}

	switch(usage)
	{
	case GL_STREAM_DRAW:
	case GL_STATIC_DRAW:
	case GL_DYNAMIC_DRAW:
		break;
	default:
		return error(GL_INVALID_ENUM);
	}

	gl::Context *context = gl::getContext();

	if(context)
	{
		gl::Buffer *buffer;

		switch(target)
		{
		case GL_ARRAY_BUFFER:
			buffer = context->getArrayBuffer();
			break;
		case GL_ELEMENT_ARRAY_BUFFER:
			buffer = context->getElementArrayBuffer();
			break;
		default:
			return error(GL_INVALID_ENUM);
		}

		if(!buffer)
		{
			return error(GL_INVALID_OPERATION);
		}

		buffer->bufferData(data, size, usage);
	}
}

void APIENTRY glBufferSubData(GLenum target, GLintptr offset, GLsizeiptr size, const GLvoid* data)
{
	TRACE("(GLenum target = 0x%X, GLintptr offset = %d, GLsizeiptr size = %d, const GLvoid* data = %p)",
	      target, offset, size, data);

	if(size < 0 || offset < 0)
	{
		return error(GL_INVALID_VALUE);
	}

	if(data == NULL)
	{
		return;
	}

	gl::Context *context = gl::getContext();

	if(context)
	{
		gl::Buffer *buffer;

		switch(target)
		{
		case GL_ARRAY_BUFFER:
			buffer = context->getArrayBuffer();
			break;
		case GL_ELEMENT_ARRAY_BUFFER:
			buffer = context->getElementArrayBuffer();
			break;
		default:
			return error(GL_INVALID_ENUM);
		}

		if(!buffer)
		{
			return error(GL_INVALID_OPERATION);
		}

		if((size_t)size + offset > buffer->size())
		{
			return error(GL_INVALID_VALUE);
		}

		buffer->bufferSubData(data, size, offset);
	}
}

GLenum APIENTRY glCheckFramebufferStatus(GLenum target)
{
	TRACE("(GLenum target = 0x%X)", target);

	if(target != GL_FRAMEBUFFER && target != GL_DRAW_FRAMEBUFFER_EXT && target != GL_READ_FRAMEBUFFER_EXT)
	{
		return error(GL_INVALID_ENUM, 0);
	}

	gl::Context *context = gl::getContext();

	if(context)
	{
		if(context->getListIndex() != 0)
		{
			UNIMPLEMENTED();
		}

		gl::Framebuffer *framebuffer = NULL;
		if(target == GL_READ_FRAMEBUFFER_EXT)
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

void APIENTRY glClear(GLbitfield mask)
{
	TRACE("(GLbitfield mask = %X)", mask);

	if((mask & ~(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT)) != 0)
	{
		return error(GL_INVALID_VALUE);
	}

	gl::Context *context = gl::getContext();

	if(context)
	{
		if(context->getListIndex() != 0)
		{
			return context->listCommand(gl::newCommand(glClear, mask));
		}

		context->clear(mask);
	}
}

void APIENTRY glClearColor(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha)
{
	TRACE("(GLclampf red = %f, GLclampf green = %f, GLclampf blue = %f, GLclampf alpha = %f)",
	      red, green, blue, alpha);

	gl::Context *context = gl::getContext();

	if(context)
	{
		if(context->getListIndex() != 0)
		{
			UNIMPLEMENTED();
		}

		context->setClearColor(red, green, blue, alpha);
	}
}

void APIENTRY glClearDepthf(GLclampf depth)
{
	TRACE("(GLclampf depth = %f)", depth);

	gl::Context *context = gl::getContext();

	if(context)
	{
		if(context->getListIndex() != 0)
		{
			UNIMPLEMENTED();
		}

		context->setClearDepth(depth);
	}
}

void APIENTRY glVertexAttrib1dv(GLuint index, const GLdouble *v)
{
	TRACE("*");
	UNIMPLEMENTED();
}
void APIENTRY glVertexAttrib1sv(GLuint index, const GLshort *v)
{
	TRACE("*");
	UNIMPLEMENTED();
}
void APIENTRY glVertexAttrib2dv(GLuint index, const GLdouble *v)
{
	TRACE("*");
	UNIMPLEMENTED();
}

void APIENTRY glVertexAttrib2sv(GLuint index, const GLshort *v)
{
	TRACE("*");
	UNIMPLEMENTED();
}
void APIENTRY glVertexAttrib3dv(GLuint index, const GLdouble *v)
{
	TRACE("*");
	UNIMPLEMENTED();
}
void APIENTRY glVertexAttrib3sv(GLuint index, const GLshort *v)
{
	TRACE("*");
	UNIMPLEMENTED();
}
void APIENTRY glVertexAttrib4Nbv(GLuint index, const GLbyte *v)
{
	TRACE("*");
	UNIMPLEMENTED();
}
void APIENTRY glVertexAttrib4Niv(GLuint index, const GLint *v)
{
	TRACE("*");
	UNIMPLEMENTED();
}
void APIENTRY glVertexAttrib4Nsv(GLuint index, const GLshort *v)
{
	TRACE("*");
	UNIMPLEMENTED();
}
void APIENTRY glVertexAttrib4Nubv(GLuint index, const GLubyte *v)
{
	TRACE("*");
	UNIMPLEMENTED();
}
void APIENTRY glVertexAttrib4Nuiv(GLuint index, const GLuint *v)
{
	TRACE("*");
	UNIMPLEMENTED();
}
void APIENTRY glVertexAttrib4Nusv(GLuint index, const GLushort *v)
{
	TRACE("*");
	UNIMPLEMENTED();
}
void APIENTRY glFogCoordfv(const GLfloat *coord)
{
	TRACE("*");
	UNIMPLEMENTED();
}
void APIENTRY glFogCoorddv(const GLdouble *coord)
{
	TRACE("*");
	UNIMPLEMENTED();
}
void APIENTRY glMultiDrawElements(GLenum mode, const GLsizei *count, GLenum type, const void *const*indices, GLsizei drawcount)
{
	TRACE("*");
	UNIMPLEMENTED();
}
void APIENTRY glSecondaryColor3bv(const GLbyte *v)
{
	TRACE("*");
	UNIMPLEMENTED();
}
void APIENTRY glSecondaryColor3dv(const GLdouble *v)
{
	TRACE("*");
	UNIMPLEMENTED();
}
void APIENTRY glSecondaryColor3fv(const GLfloat *v)
{
	TRACE("*");
	UNIMPLEMENTED();
}
void APIENTRY glSecondaryColor3i(GLint red, GLint green, GLint blue)
{
	TRACE("*");
	UNIMPLEMENTED();
}
void APIENTRY glSecondaryColor3iv(const GLint *v)
{
	TRACE("*");
	UNIMPLEMENTED();
}
void APIENTRY glSecondaryColor3s(GLshort red, GLshort green, GLshort blue)
{
	TRACE("*");
	UNIMPLEMENTED();
}
void APIENTRY glSecondaryColor3sv(const GLshort *v)
{
	TRACE("*");
	UNIMPLEMENTED();
}
void APIENTRY glSecondaryColor3ubv(const GLubyte *v)
{
	TRACE("*");
	UNIMPLEMENTED();
}
void APIENTRY glSecondaryColor3ui(GLuint red, GLuint green, GLuint blue)
{
	TRACE("*");
	UNIMPLEMENTED();
}
void APIENTRY glSecondaryColor3uiv(const GLuint *v)
{
	TRACE("*");
	UNIMPLEMENTED();
}
void APIENTRY glSecondaryColor3us(GLushort red, GLushort green, GLushort blue)
{
	TRACE("*");
	UNIMPLEMENTED();
}
void APIENTRY glSecondaryColor3usv(const GLushort *v)
{
	TRACE("*");
	UNIMPLEMENTED();
}
void APIENTRY glWindowPos2dv(const GLdouble *v)
{
	TRACE("*");
	UNIMPLEMENTED();
}
void APIENTRY glWindowPos2fv(const GLfloat *v)
{
	TRACE("*");
	UNIMPLEMENTED();
}
void APIENTRY glWindowPos2iv(const GLint *v)
{
	TRACE("*");
	UNIMPLEMENTED();
}
void APIENTRY glWindowPos2s(GLshort x, GLshort y)
{
	TRACE("*");
	UNIMPLEMENTED();
}
void APIENTRY glWindowPos2sv(const GLshort *v)
{
	TRACE("*");
	UNIMPLEMENTED();
}
void APIENTRY glWindowPos3dv(const GLdouble *v)
{
	TRACE("*");
	UNIMPLEMENTED();
}
void APIENTRY glWindowPos3fv(const GLfloat *v)
{
	TRACE("*");
	UNIMPLEMENTED();
}
void APIENTRY glWindowPos3iv(const GLint *v)
{
	TRACE("*");
	UNIMPLEMENTED();
}
void APIENTRY glWindowPos3s(GLshort x, GLshort y, GLshort z)
{
	TRACE("*");
	UNIMPLEMENTED();
}
void APIENTRY glWindowPos3sv(const GLshort *v)
{
	TRACE("*");
	UNIMPLEMENTED();
}
void APIENTRY glMultiTexCoord1dv(GLenum target, const GLdouble *v)
{
	TRACE("*");
	UNIMPLEMENTED();
}
void APIENTRY glMultiTexCoord1fv(GLenum target, const GLfloat *v)
{
	TRACE("*");
	UNIMPLEMENTED();
}
void APIENTRY glMultiTexCoord1i(GLenum target, GLint s)
{
	TRACE("*");
	UNIMPLEMENTED();
}
void APIENTRY glMultiTexCoord1iv(GLenum target, const GLint *v)
{
	TRACE("*");
	UNIMPLEMENTED();
}
void APIENTRY glMultiTexCoord1s(GLenum target, GLshort s)
{
	TRACE("*");
	UNIMPLEMENTED();
}
void APIENTRY glMultiTexCoord1sv(GLenum target, const GLshort *v)
{
	TRACE("*");
	UNIMPLEMENTED();
}
void APIENTRY glMultiTexCoord2dv(GLenum target, const GLdouble *v)
{
	TRACE("*");
	UNIMPLEMENTED();
}
void APIENTRY glMultiTexCoord2fv(GLenum target, const GLfloat *v)
{
	TRACE("*");
	UNIMPLEMENTED();
}
void APIENTRY glMultiTexCoord2i(GLenum target, GLint s, GLint t)
{
	TRACE("*");
	UNIMPLEMENTED();
}
void APIENTRY glMultiTexCoord2iv(GLenum target, const GLint *v)
{
	TRACE("*");
	UNIMPLEMENTED();
}
void APIENTRY glMultiTexCoord2s(GLenum target, GLshort s, GLshort t)
{
	TRACE("*");
	UNIMPLEMENTED();
}
void APIENTRY glMultiTexCoord2sv(GLenum target, const GLshort *v)
{
	TRACE("*");
	UNIMPLEMENTED();
}
void APIENTRY glMultiTexCoord3dv(GLenum target, const GLdouble *v)
{
	TRACE("*");
	UNIMPLEMENTED();
}
void APIENTRY glMultiTexCoord3fv(GLenum target, const GLfloat *v)
{
	TRACE("*");
	UNIMPLEMENTED();
}
void APIENTRY glMultiTexCoord3i(GLenum target, GLint s, GLint t, GLint r)
{
	TRACE("*");
	UNIMPLEMENTED();
}
void APIENTRY glMultiTexCoord3iv(GLenum target, const GLint *v)
{
	TRACE("*");
	UNIMPLEMENTED();
}
void APIENTRY glMultiTexCoord3s(GLenum target, GLshort s, GLshort t, GLshort r)
{
	TRACE("*");
	UNIMPLEMENTED();
}
void APIENTRY glMultiTexCoord3sv(GLenum target, const GLshort *v)
{
	TRACE("*");
	UNIMPLEMENTED();
}
void APIENTRY glMultiTexCoord4dv(GLenum target, const GLdouble *v)
{
	TRACE("*");
	UNIMPLEMENTED();
}
void APIENTRY glMultiTexCoord4fv(GLenum target, const GLfloat *v)
{
	TRACE("*");
	UNIMPLEMENTED();
}
void APIENTRY glMultiTexCoord4i(GLenum target, GLint s, GLint t, GLint r, GLint q)
{
	TRACE("*");
	UNIMPLEMENTED();
}
void APIENTRY glMultiTexCoord4iv(GLenum target, const GLint *v)
{
	TRACE("*");
	UNIMPLEMENTED();
}
void APIENTRY glMultiTexCoord4s(GLenum target, GLshort s, GLshort t, GLshort r, GLshort q)
{
	TRACE("*");
	UNIMPLEMENTED();
}
void APIENTRY glMultiTexCoord4sv(GLenum target, const GLshort *v)
{
	TRACE("*");
	UNIMPLEMENTED();
}
void APIENTRY glBlendEquationEXT(GLenum mode)
{
	TRACE("*");
	UNIMPLEMENTED();
}
void APIENTRY glBlendFuncSeparateEXT(GLenum sfactorRGB, GLenum dfactorRGB, GLenum sfactorAlpha, GLenum dfactorAlpha)
{
	TRACE("*");
	UNIMPLEMENTED();
}
void APIENTRY glSecondaryColor3bEXT(GLbyte red, GLbyte green, GLbyte blue)
{
	TRACE("*");
	UNIMPLEMENTED();
}
void APIENTRY glSecondaryColor3bvEXT(const GLbyte *v)
{
	TRACE("*");
	UNIMPLEMENTED();
}
void APIENTRY glSecondaryColor3dEXT(GLdouble red, GLdouble green, GLdouble blue)
{
	TRACE("*");
	UNIMPLEMENTED();
}
void APIENTRY glSecondaryColor3dvEXT(const GLdouble *v)
{
	TRACE("*");
	UNIMPLEMENTED();
}
void APIENTRY glSecondaryColor3fEXT(GLfloat red, GLfloat green, GLfloat blue)
{
	TRACE("*");
	UNIMPLEMENTED();
}
void APIENTRY glSecondaryColor3fvEXT(const GLfloat *v)
{
	TRACE("*");
	UNIMPLEMENTED();
}
void APIENTRY glSecondaryColor3iEXT(GLint red, GLint green, GLint blue)
{
	TRACE("*");
	UNIMPLEMENTED();
}
void APIENTRY glSecondaryColor3ivEXT(const GLint *v)
{
	TRACE("*");
	UNIMPLEMENTED();
}
void APIENTRY glSecondaryColor3sEXT(GLshort red, GLshort green, GLshort blue)
{
	TRACE("*");
	UNIMPLEMENTED();
}
void APIENTRY glSecondaryColor3svEXT(const GLshort *v)
{
	TRACE("*");
	UNIMPLEMENTED();
}
void APIENTRY glSecondaryColor3ubEXT(GLubyte red, GLubyte green, GLubyte blue)
{
	TRACE("*");
	UNIMPLEMENTED();
}
void APIENTRY glSecondaryColor3ubvEXT(const GLubyte *v)
{
	TRACE("*");
	UNIMPLEMENTED();
}
void APIENTRY glSecondaryColor3uiEXT(GLuint red, GLuint green, GLuint blue)
{
	TRACE("*");
	UNIMPLEMENTED();
}
void APIENTRY glSecondaryColor3uivEXT(const GLuint *v)
{
	TRACE("*");
	UNIMPLEMENTED();
}
void APIENTRY glSecondaryColor3usEXT(GLushort red, GLushort green, GLushort blue)
{
	TRACE("*");
	UNIMPLEMENTED();
}
void APIENTRY glSecondaryColor3usvEXT(const GLushort *v)
{
	TRACE("*");
	UNIMPLEMENTED();
}
void APIENTRY glSecondaryColorPointerEXT(GLint size, GLenum type, GLsizei stride, const void *pointer)
{
	TRACE("*");
	UNIMPLEMENTED();
}
void APIENTRY glBlitFramebufferEXT(GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter)
{
	TRACE("*");
	UNIMPLEMENTED();
}
void APIENTRY glRenderbufferStorageMultisampleEXT(GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height)
{
	TRACE("*");
	UNIMPLEMENTED();
}


void APIENTRY glClearStencil(GLint s)
{
	TRACE("(GLint s = %d)", s);

	gl::Context *context = gl::getContext();

	if(context)
	{
		if(context->getListIndex() != 0)
		{
			UNIMPLEMENTED();
		}

		context->setClearStencil(s);
	}
}

void APIENTRY glColorMask(GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha)
{
	TRACE("(GLboolean red = %d, GLboolean green = %d, GLboolean blue = %d, GLboolean alpha = %d)",
	      red, green, blue, alpha);

	gl::Context *context = gl::getContext();

	if(context)
	{
		if(context->getListIndex() != 0)
		{
			UNIMPLEMENTED();
		}

		context->setColorMask(red == GL_TRUE, green == GL_TRUE, blue == GL_TRUE, alpha == GL_TRUE);
	}
}

void APIENTRY glCompileShader(GLuint shader)
{
	TRACE("(GLuint shader = %d)", shader);

	gl::Context *context = gl::getContext();

	if(context)
	{
		gl::Shader *shaderObject = context->getShader(shader);

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

void APIENTRY glCompressedTexImage2D(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height,
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
	case GL_COMPRESSED_RGB_S3TC_DXT1_EXT:
	case GL_COMPRESSED_RGBA_S3TC_DXT1_EXT:
	case GL_COMPRESSED_RGBA_S3TC_DXT3_EXT:
	case GL_COMPRESSED_RGBA_S3TC_DXT5_EXT:
		if(!S3TC_SUPPORT)
		{
			return error(GL_INVALID_ENUM);
		}
		break;
	case GL_DEPTH_COMPONENT:
	case GL_DEPTH_COMPONENT16:
	case GL_DEPTH_COMPONENT32:
	case GL_DEPTH_STENCIL_EXT:
	case GL_DEPTH24_STENCIL8_EXT:
		return error(GL_INVALID_OPERATION);
	default:
		return error(GL_INVALID_ENUM);
	}

	if(border != 0)
	{
		return error(GL_INVALID_VALUE);
	}

	gl::Context *context = gl::getContext();

	if(context)
	{
		if(context->getListIndex() != 0)
		{
			UNIMPLEMENTED();
		}

		if(level > gl::IMPLEMENTATION_MAX_TEXTURE_LEVELS)
		{
			return error(GL_INVALID_VALUE);
		}

		switch(target)
		{
		case GL_TEXTURE_2D:
			if(width > (gl::IMPLEMENTATION_MAX_TEXTURE_SIZE >> level) ||
				height > (gl::IMPLEMENTATION_MAX_TEXTURE_SIZE >> level))
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

			if(width > (gl::IMPLEMENTATION_MAX_CUBE_MAP_TEXTURE_SIZE >> level) ||
				height > (gl::IMPLEMENTATION_MAX_CUBE_MAP_TEXTURE_SIZE >> level))
			{
				return error(GL_INVALID_VALUE);
			}
			break;
		default:
			return error(GL_INVALID_ENUM);
		}

		if(imageSize != gl::ComputeCompressedSize(width, height, internalformat))
		{
			return error(GL_INVALID_VALUE);
		}

		if(target == GL_TEXTURE_2D)
		{
			gl::Texture2D *texture = context->getTexture2D(target);

			if(!texture)
			{
				return error(GL_INVALID_OPERATION);
			}

			texture->setCompressedImage(level, internalformat, width, height, imageSize, data);
		}
		else
		{
			gl::TextureCubeMap *texture = context->getTextureCubeMap();

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
			default: UNREACHABLE();
			}
		}
	}
}

void APIENTRY glCompressedTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height,
	GLenum format, GLsizei imageSize, const GLvoid* data)
{
	TRACE("(GLenum target = 0x%X, GLint level = %d, GLint xoffset = %d, GLint yoffset = %d, "
		"GLsizei width = %d, GLsizei height = %d, GLenum format = 0x%X, "
		"GLsizei imageSize = %d, const GLvoid* data = %p)",
		target, level, xoffset, yoffset, width, height, format, imageSize, data);

	if(!gl::IsTextureTarget(target))
	{
		return error(GL_INVALID_ENUM);
	}

	if(xoffset < 0 || yoffset < 0 || !validImageSize(level, width, height) || imageSize < 0)
	{
		return error(GL_INVALID_VALUE);
	}

	switch(format)
	{
	case GL_COMPRESSED_RGB_S3TC_DXT1_EXT:
	case GL_COMPRESSED_RGBA_S3TC_DXT1_EXT:
	case GL_COMPRESSED_RGBA_S3TC_DXT3_EXT:
	case GL_COMPRESSED_RGBA_S3TC_DXT5_EXT:
		if(!S3TC_SUPPORT)
		{
			return error(GL_INVALID_ENUM);
		}
		break;
	default:
		return error(GL_INVALID_ENUM);
	}

	if(width == 0 || height == 0 || data == NULL)
	{
		return;
	}

	gl::Context *context = gl::getContext();

	if(context)
	{
		if(context->getListIndex() != 0)
		{
			UNIMPLEMENTED();
		}

		if(level > gl::IMPLEMENTATION_MAX_TEXTURE_LEVELS)
		{
			return error(GL_INVALID_VALUE);
		}

		if(imageSize != gl::ComputeCompressedSize(width, height, format))
		{
			return error(GL_INVALID_VALUE);
		}

		if(xoffset % 4 != 0 || yoffset % 4 != 0)
		{
			// We wait to check the offsets until this point, because the multiple-of-four restriction does not exist unless DXT1 textures are supported
			return error(GL_INVALID_OPERATION);
		}

		if(target == GL_TEXTURE_2D)
		{
			gl::Texture2D *texture = context->getTexture2D(target);

			if(validateSubImageParams(true, width, height, xoffset, yoffset, target, level, format, texture))
			{
				texture->subImageCompressed(level, xoffset, yoffset, width, height, format, imageSize, data);
			}
		}
		else if(gl::IsCubemapTextureTarget(target))
		{
			gl::TextureCubeMap *texture = context->getTextureCubeMap();

			if(validateSubImageParams(true, width, height, xoffset, yoffset, target, level, format, texture))
			{
				texture->subImageCompressed(target, level, xoffset, yoffset, width, height, format, imageSize, data);
			}
		}
		else
		{
			UNREACHABLE();
		}
	}
}

void APIENTRY glCopyTexImage2D(GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border)
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

	gl::Context *context = gl::getContext();

	if(context)
	{
		if(context->getListIndex() != 0)
		{
			UNIMPLEMENTED();
		}

		switch(target)
		{
		case GL_TEXTURE_2D:
			if(width > (gl::IMPLEMENTATION_MAX_TEXTURE_SIZE >> level) ||
				height > (gl::IMPLEMENTATION_MAX_TEXTURE_SIZE >> level))
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

			if(width > (gl::IMPLEMENTATION_MAX_CUBE_MAP_TEXTURE_SIZE >> level) ||
				height > (gl::IMPLEMENTATION_MAX_CUBE_MAP_TEXTURE_SIZE >> level))
			{
				return error(GL_INVALID_VALUE);
			}
			break;
		default:
			return error(GL_INVALID_ENUM);
		}

		gl::Framebuffer *framebuffer = context->getReadFramebuffer();

		if(framebuffer->completeness() != GL_FRAMEBUFFER_COMPLETE)
		{
			return error(GL_INVALID_FRAMEBUFFER_OPERATION);
		}

		if(context->getReadFramebufferName() != 0 && framebuffer->getColorbuffer()->getSamples() > 1)
		{
			return error(GL_INVALID_OPERATION);
		}

		gl::Renderbuffer *source = framebuffer->getColorbuffer();
		GLenum colorbufferFormat = source->getFormat();

		switch(internalformat)
		{
		case GL_ALPHA:
			if(colorbufferFormat != GL_ALPHA &&
				colorbufferFormat != GL_RGBA &&
				colorbufferFormat != GL_RGBA4 &&
				colorbufferFormat != GL_RGB5_A1 &&
				colorbufferFormat != GL_RGBA8_EXT)
			{
				return error(GL_INVALID_OPERATION);
			}
			break;
		case GL_LUMINANCE:
		case GL_RGB:
			if(colorbufferFormat != GL_RGB &&
				colorbufferFormat != GL_RGB565 &&
				colorbufferFormat != GL_RGB8_EXT &&
				colorbufferFormat != GL_RGBA &&
				colorbufferFormat != GL_RGBA4 &&
				colorbufferFormat != GL_RGB5_A1 &&
				colorbufferFormat != GL_RGBA8_EXT)
			{
				return error(GL_INVALID_OPERATION);
			}
			break;
		case GL_LUMINANCE_ALPHA:
		case GL_RGBA:
			if(colorbufferFormat != GL_RGBA &&
				colorbufferFormat != GL_RGBA4 &&
				colorbufferFormat != GL_RGB5_A1 &&
				colorbufferFormat != GL_RGBA8_EXT)
			{
				return error(GL_INVALID_OPERATION);
			}
			break;
		case GL_COMPRESSED_RGB_S3TC_DXT1_EXT:
		case GL_COMPRESSED_RGBA_S3TC_DXT1_EXT:
		case GL_COMPRESSED_RGBA_S3TC_DXT3_EXT:
		case GL_COMPRESSED_RGBA_S3TC_DXT5_EXT:
			if(S3TC_SUPPORT)
			{
				return error(GL_INVALID_OPERATION);
			}
			else
			{
				return error(GL_INVALID_ENUM);
			}
		default:
			return error(GL_INVALID_ENUM);
		}

		if(target == GL_TEXTURE_2D)
		{
			gl::Texture2D *texture = context->getTexture2D(target);

			if(!texture)
			{
				return error(GL_INVALID_OPERATION);
			}

			texture->copyImage(level, internalformat, x, y, width, height, framebuffer);
		}
		else if(gl::IsCubemapTextureTarget(target))
		{
			gl::TextureCubeMap *texture = context->getTextureCubeMap();

			if(!texture)
			{
				return error(GL_INVALID_OPERATION);
			}

			texture->copyImage(target, level, internalformat, x, y, width, height, framebuffer);
		}
		else UNREACHABLE();
	}
}

void APIENTRY glCopyTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height)
{
	TRACE("(GLenum target = 0x%X, GLint level = %d, GLint xoffset = %d, GLint yoffset = %d, "
		"GLint x = %d, GLint y = %d, GLsizei width = %d, GLsizei height = %d)",
		target, level, xoffset, yoffset, x, y, width, height);

	if(!gl::IsTextureTarget(target))
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

	gl::Context *context = gl::getContext();

	if(context)
	{
		if(context->getListIndex() != 0)
		{
			UNIMPLEMENTED();
		}

		if(level > gl::IMPLEMENTATION_MAX_TEXTURE_LEVELS)
		{
			return error(GL_INVALID_VALUE);
		}

		gl::Framebuffer *framebuffer = context->getReadFramebuffer();

		if(framebuffer->completeness() != GL_FRAMEBUFFER_COMPLETE)
		{
			return error(GL_INVALID_FRAMEBUFFER_OPERATION);
		}

		if(context->getReadFramebufferName() != 0 && framebuffer->getColorbuffer()->getSamples() > 1)
		{
			return error(GL_INVALID_OPERATION);
		}

		gl::Renderbuffer *source = framebuffer->getColorbuffer();
		GLenum colorbufferFormat = source->getFormat();
		gl::Texture *texture = NULL;

		if(target == GL_TEXTURE_2D)
		{
			texture = context->getTexture2D(target);
		}
		else if(gl::IsCubemapTextureTarget(target))
		{
			texture = context->getTextureCubeMap();
		}
		else UNREACHABLE();

		if(!validateSubImageParams(false, width, height, xoffset, yoffset, target, level, GL_NONE, texture))
		{
			return;
		}

		GLenum textureFormat = texture->getFormat(target, level);

		switch(textureFormat)
		{
		case GL_ALPHA:
			if(colorbufferFormat != GL_ALPHA &&
				colorbufferFormat != GL_RGBA &&
				colorbufferFormat != GL_RGBA4 &&
				colorbufferFormat != GL_RGB5_A1 &&
				colorbufferFormat != GL_RGBA8_EXT)
			{
				return error(GL_INVALID_OPERATION);
			}
			break;
		case GL_LUMINANCE:
		case GL_RGB:
			if(colorbufferFormat != GL_RGB &&
				colorbufferFormat != GL_RGB565 &&
				colorbufferFormat != GL_RGB8_EXT &&
				colorbufferFormat != GL_RGBA &&
				colorbufferFormat != GL_RGBA4 &&
				colorbufferFormat != GL_RGB5_A1 &&
				colorbufferFormat != GL_RGBA8_EXT)
			{
				return error(GL_INVALID_OPERATION);
			}
			break;
		case GL_LUMINANCE_ALPHA:
		case GL_RGBA:
			if(colorbufferFormat != GL_RGBA &&
				colorbufferFormat != GL_RGBA4 &&
				colorbufferFormat != GL_RGB5_A1 &&
				colorbufferFormat != GL_RGBA8_EXT)
			{
				return error(GL_INVALID_OPERATION);
			}
			break;
		case GL_COMPRESSED_RGB_S3TC_DXT1_EXT:
		case GL_COMPRESSED_RGBA_S3TC_DXT1_EXT:
		case GL_COMPRESSED_RGBA_S3TC_DXT3_EXT:
		case GL_COMPRESSED_RGBA_S3TC_DXT5_EXT:
			return error(GL_INVALID_OPERATION);
		case GL_DEPTH_COMPONENT:
		case GL_DEPTH_STENCIL_EXT:
			return error(GL_INVALID_OPERATION);
		case GL_BGRA_EXT:
			if(colorbufferFormat != GL_RGB8)
			{
				UNIMPLEMENTED();
				return error(GL_INVALID_OPERATION);
			}
			break;
		default:
			return error(GL_INVALID_ENUM);
		}

		texture->copySubImage(target, level, xoffset, yoffset, x, y, width, height, framebuffer);
	}
}

GLuint APIENTRY glCreateProgram(void)
{
	TRACE("()");

	gl::Context *context = gl::getContext();

	if(context)
	{
		return context->createProgram();
	}

	return 0;
}

GLuint APIENTRY glCreateShader(GLenum type)
{
	TRACE("(GLenum type = 0x%X)", type);

	gl::Context *context = gl::getContext();

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

void APIENTRY glCullFace(GLenum mode)
{
	TRACE("(GLenum mode = 0x%X)", mode);

	switch(mode)
	{
	case GL_FRONT:
	case GL_BACK:
	case GL_FRONT_AND_BACK:
	{
		gl::Context *context = gl::getContext();

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

void APIENTRY glDeleteBuffers(GLsizei n, const GLuint* buffers)
{
	TRACE("(GLsizei n = %d, const GLuint* buffers = %p)", n, buffers);

	if(n < 0)
	{
		return error(GL_INVALID_VALUE);
	}

	gl::Context *context = gl::getContext();

	if(context)
	{
		for(int i = 0; i < n; i++)
		{
			context->deleteBuffer(buffers[i]);
		}
	}
}

void APIENTRY glDeleteFencesNV(GLsizei n, const GLuint* fences)
{
	TRACE("(GLsizei n = %d, const GLuint* fences = %p)", n, fences);

	if(n < 0)
	{
		return error(GL_INVALID_VALUE);
	}

	gl::Context *context = gl::getContext();

	if(context)
	{
		if(context->getListIndex() != 0)
		{
			UNIMPLEMENTED();
		}

		for(int i = 0; i < n; i++)
		{
			context->deleteFence(fences[i]);
		}
	}
}

void APIENTRY glDeleteFramebuffers(GLsizei n, const GLuint* framebuffers)
{
	TRACE("(GLsizei n = %d, const GLuint* framebuffers = %p)", n, framebuffers);

	if(n < 0)
	{
		return error(GL_INVALID_VALUE);
	}

	gl::Context *context = gl::getContext();

	if(context)
	{
		if(context->getListIndex() != 0)
		{
			UNIMPLEMENTED();
		}

		for(int i = 0; i < n; i++)
		{
			if(framebuffers[i] != 0)
			{
				context->deleteFramebuffer(framebuffers[i]);
			}
		}
	}
}

void APIENTRY glDeleteProgram(GLuint program)
{
	TRACE("(GLuint program = %d)", program);

	if(program == 0)
	{
		return;
	}

	gl::Context *context = gl::getContext();

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

void APIENTRY glDeleteQueriesEXT(GLsizei n, const GLuint *ids)
{
	TRACE("(GLsizei n = %d, const GLuint *ids = %p)", n, ids);

	if(n < 0)
	{
		return error(GL_INVALID_VALUE);
	}

	gl::Context *context = gl::getContext();

	if(context)
	{
		for(int i = 0; i < n; i++)
		{
			context->deleteQuery(ids[i]);
		}
	}
}

void APIENTRY glDeleteRenderbuffers(GLsizei n, const GLuint* renderbuffers)
{
	TRACE("(GLsizei n = %d, const GLuint* renderbuffers = %p)", n, renderbuffers);

	if(n < 0)
	{
		return error(GL_INVALID_VALUE);
	}

	gl::Context *context = gl::getContext();

	if(context)
	{
		if(context->getListIndex() != 0)
		{
			UNIMPLEMENTED();
		}

		for(int i = 0; i < n; i++)
		{
			context->deleteRenderbuffer(renderbuffers[i]);
		}
	}
}

void APIENTRY glDeleteShader(GLuint shader)
{
	TRACE("(GLuint shader = %d)", shader);

	if(shader == 0)
	{
		return;
	}

	gl::Context *context = gl::getContext();

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

void APIENTRY glDeleteTextures(GLsizei n, const GLuint* textures)
{
	TRACE("(GLsizei n = %d, const GLuint* textures = %p)", n, textures);

	if(n < 0)
	{
		return error(GL_INVALID_VALUE);
	}

	gl::Context *context = gl::getContext();

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

void APIENTRY glDepthFunc(GLenum func)
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

	gl::Context *context = gl::getContext();

	if(context)
	{
		if(context->getListIndex() != 0)
		{
			UNIMPLEMENTED();
		}

		context->setDepthFunc(func);
	}
}

void APIENTRY glDepthMask(GLboolean flag)
{
	TRACE("(GLboolean flag = %d)", flag);

	gl::Context *context = gl::getContext();

	if(context)
	{
		if(context->getListIndex() != 0)
		{
			UNIMPLEMENTED();
		}

		context->setDepthMask(flag != GL_FALSE);
	}
}

void APIENTRY glDepthRangef(GLclampf zNear, GLclampf zFar)
{
	TRACE("(GLclampf zNear = %f, GLclampf zFar = %f)", zNear, zFar);

	gl::Context *context = gl::getContext();

	if(context)
	{
		if(context->getListIndex() != 0)
		{
			UNIMPLEMENTED();
		}

		context->setDepthRange(zNear, zFar);
	}
}

void APIENTRY glDetachShader(GLuint program, GLuint shader)
{
	TRACE("(GLuint program = %d, GLuint shader = %d)", program, shader);

	gl::Context *context = gl::getContext();

	if(context)
	{
		gl::Program *programObject = context->getProgram(program);
		gl::Shader *shaderObject = context->getShader(shader);

		if(!programObject)
		{
			gl::Shader *shaderByProgramHandle;
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
			gl::Program *programByShaderHandle = context->getProgram(shader);
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

void APIENTRY glDisable(GLenum cap)
{
	TRACE("(GLenum cap = 0x%X)", cap);

	gl::Context *context = gl::getContext();

	if(context)
	{
		if(context->getListIndex() != 0)
		{
			UNIMPLEMENTED();
		}

		switch(cap)
		{
		case GL_CULL_FACE:                context->setCullFace(false);					break;
		case GL_POLYGON_OFFSET_FILL:      context->setPolygonOffsetFill(false);			break;
		case GL_SAMPLE_ALPHA_TO_COVERAGE: context->setSampleAlphaToCoverage(false);		break;
		case GL_SAMPLE_COVERAGE:          context->setSampleCoverage(false);			break;
		case GL_SCISSOR_TEST:             context->setScissorTest(false);				break;
		case GL_STENCIL_TEST:             context->setStencilTest(false);				break;
		case GL_DEPTH_TEST:               context->setDepthTest(false);					break;
		case GL_BLEND:                    context->setBlend(false);						break;
		case GL_DITHER:                   context->setDither(false);					break;
		case GL_LIGHTING:                 context->setLighting(false);					break;
		case GL_FOG:                      context->setFog(false);						break;
		case GL_ALPHA_TEST:               context->setAlphaTest(false);					break;
		case GL_TEXTURE_2D:               context->setTexture2D(false);					break;
		case GL_LIGHT0:                   context->setLight(0, false);					break;
		case GL_LIGHT1:                   context->setLight(1, false);					break;
		case GL_LIGHT2:                   context->setLight(2, false);					break;
		case GL_LIGHT3:                   context->setLight(3, false);					break;
		case GL_LIGHT4:                   context->setLight(4, false);					break;
		case GL_LIGHT5:                   context->setLight(5, false);					break;
		case GL_LIGHT6:                   context->setLight(6, false);					break;
		case GL_LIGHT7:                   context->setLight(7, false);					break;
		case GL_COLOR_MATERIAL:           context->setColorMaterial(false);				break;
		case GL_RESCALE_NORMAL:           context->setNormalizeNormals(false);			break;
		case GL_TEXTURE_1D:				  context->set1DTextureEnable(false);			break;
		default:
			return error(GL_INVALID_ENUM);
		}
	}
}

void APIENTRY glDisableVertexAttribArray(GLuint index)
{
	TRACE("(GLuint index = %d)", index);

	if(index >= gl::MAX_VERTEX_ATTRIBS)
	{
		return error(GL_INVALID_VALUE);
	}

	gl::Context *context = gl::getContext();

	if(context)
	{
		context->setEnableVertexAttribArray(index, false);
	}
}

void APIENTRY glCaptureAttribs()
{
	TRACE("()");

	gl::Context *context = gl::getContext();

	if(context)
	{
		context->captureAttribs();
	}
}

void APIENTRY glRestoreAttribs()
{
	TRACE("()");

	gl::Context *context = gl::getContext();

	if(context)
	{
		context->restoreAttribs();
	}
}

void APIENTRY glDrawArrays(GLenum mode, GLint first, GLsizei count)
{
	TRACE("(GLenum mode = 0x%X, GLint first = %d, GLsizei count = %d)", mode, first, count);

	if(count < 0 || first < 0)
	{
		return error(GL_INVALID_VALUE);
	}

	gl::Context *context = gl::getContext();

	if(context)
	{
		if(context->getListIndex() != 0)
		{
			ASSERT(context->getListMode() != GL_COMPILE_AND_EXECUTE);   // UNIMPLEMENTED!

			context->listCommand(gl::newCommand(glCaptureAttribs));
			context->captureDrawArrays(mode, first, count);
			context->listCommand(gl::newCommand(glDrawArrays, mode, first, count));
			context->listCommand(gl::newCommand(glRestoreAttribs));

			return;
		}

		context->drawArrays(mode, first, count);
	}
}

void APIENTRY glDrawElements(GLenum mode, GLsizei count, GLenum type, const GLvoid* indices)
{
	TRACE("(GLenum mode = 0x%X, GLsizei count = %d, GLenum type = 0x%X, const GLvoid* indices = %p)",
		mode, count, type, indices);

	if(count < 0)
	{
		return error(GL_INVALID_VALUE);
	}

	gl::Context *context = gl::getContext();

	if(context)
	{
		if(context->getListIndex() != 0)
		{
			UNIMPLEMENTED();
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

		context->drawElements(mode, count, type, indices);
	}
}

void APIENTRY glEnable(GLenum cap)
{
	TRACE("(GLenum cap = 0x%X)", cap);

	gl::Context *context = gl::getContext();

	if(context)
	{
		if(context->getListIndex() != 0)
		{
			UNIMPLEMENTED();
		}

		switch(cap)
		{
		case GL_CULL_FACE:                context->setCullFace(true);              break;
		case GL_POLYGON_OFFSET_FILL:      context->setPolygonOffsetFill(true);     break;
		case GL_SAMPLE_ALPHA_TO_COVERAGE: context->setSampleAlphaToCoverage(true); break;
		case GL_SAMPLE_COVERAGE:          context->setSampleCoverage(true);        break;
		case GL_SCISSOR_TEST:             context->setScissorTest(true);           break;
		case GL_STENCIL_TEST:             context->setStencilTest(true);           break;
		case GL_DEPTH_TEST:               context->setDepthTest(true);             break;
		case GL_BLEND:                    context->setBlend(true);                 break;
		case GL_DITHER:                   context->setDither(true);                break;
		case GL_TEXTURE_2D:               context->setTexture2D(true);             break;
		case GL_ALPHA_TEST:               context->setAlphaTest(true);             break;
		case GL_COLOR_MATERIAL:           context->setColorMaterial(true);         break;
		case GL_FOG:                      context->setFog(true);                   break;
		case GL_LIGHTING:                 context->setLighting(true);              break;
		case GL_LIGHT0:                   context->setLight(0, true);              break;
		case GL_LIGHT1:                   context->setLight(1, true);              break;
		case GL_LIGHT2:                   context->setLight(2, true);              break;
		case GL_LIGHT3:                   context->setLight(3, true);              break;
		case GL_LIGHT4:                   context->setLight(4, true);              break;
		case GL_LIGHT5:                   context->setLight(5, true);              break;
		case GL_LIGHT6:                   context->setLight(6, true);              break;
		case GL_LIGHT7:                   context->setLight(7, true);              break;
		case GL_RESCALE_NORMAL:           context->setNormalizeNormals(true);      break;
		case GL_TEXTURE_1D:				  context->set1DTextureEnable(true);	   break;
		default:
			return error(GL_INVALID_ENUM);
		}
	}
}

void APIENTRY glEnableVertexAttribArray(GLuint index)
{
	TRACE("(GLuint index = %d)", index);

	if(index >= gl::MAX_VERTEX_ATTRIBS)
	{
		return error(GL_INVALID_VALUE);
	}

	gl::Context *context = gl::getContext();

	if(context)
	{
		context->setEnableVertexAttribArray(index, true);
	}
}

void APIENTRY glEndQueryEXT(GLenum target)
{
	TRACE("GLenum target = 0x%X)", target);

	switch(target)
	{
	case GL_ANY_SAMPLES_PASSED:
	case GL_ANY_SAMPLES_PASSED_CONSERVATIVE:
		break;
	default:
		return error(GL_INVALID_ENUM);
	}

	gl::Context *context = gl::getContext();

	if(context)
	{
		if(context->getListIndex() != 0)
		{
			UNIMPLEMENTED();
		}

		context->endQuery(target);
	}
}

void APIENTRY glFinishFenceNV(GLuint fence)
{
	TRACE("(GLuint fence = %d)", fence);

	gl::Context *context = gl::getContext();

	if(context)
	{
		if(context->getListIndex() != 0)
		{
			UNIMPLEMENTED();
		}

		gl::Fence* fenceObject = context->getFence(fence);

		if(fenceObject == NULL)
		{
			return error(GL_INVALID_OPERATION);
		}

		fenceObject->finishFence();
	}
}

void APIENTRY glFinish(void)
{
	TRACE("()");

	gl::Context *context = gl::getContext();

	if(context)
	{
		context->finish();
	}
}

void APIENTRY glFlush(void)
{
	TRACE("()");

	gl::Context *context = gl::getContext();

	if(context)
	{
		context->flush();
	}
}

void APIENTRY glFramebufferRenderbuffer(GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer)
{
	TRACE("(GLenum target = 0x%X, GLenum attachment = 0x%X, GLenum renderbuffertarget = 0x%X, "
		"GLuint renderbuffer = %d)", target, attachment, renderbuffertarget, renderbuffer);

	if((target != GL_FRAMEBUFFER && target != GL_DRAW_FRAMEBUFFER_EXT && target != GL_READ_FRAMEBUFFER_EXT) ||
		(renderbuffertarget != GL_RENDERBUFFER && renderbuffer != 0))
	{
		return error(GL_INVALID_ENUM);
	}

	gl::Context *context = gl::getContext();

	if(context)
	{
		if(context->getListIndex() != 0)
		{
			UNIMPLEMENTED();
		}

		gl::Framebuffer *framebuffer = NULL;
		GLuint framebufferName = 0;
		if(target == GL_READ_FRAMEBUFFER_EXT)
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

		switch(attachment)
		{
		case GL_COLOR_ATTACHMENT0:
			framebuffer->setColorbuffer(GL_RENDERBUFFER, renderbuffer);
			break;
		case GL_DEPTH_ATTACHMENT:
			framebuffer->setDepthbuffer(GL_RENDERBUFFER, renderbuffer);
			break;
		case GL_STENCIL_ATTACHMENT:
			framebuffer->setStencilbuffer(GL_RENDERBUFFER, renderbuffer);
			break;
		default:
			return error(GL_INVALID_ENUM);
		}
	}
}

void APIENTRY glFramebufferTexture1D(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level)
{
	UNIMPLEMENTED();
}

void APIENTRY glFramebufferTexture2D(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level)
{
	TRACE("(GLenum target = 0x%X, GLenum attachment = 0x%X, GLenum textarget = 0x%X, "
		"GLuint texture = %d, GLint level = %d)", target, attachment, textarget, texture, level);

	if(target != GL_FRAMEBUFFER && target != GL_DRAW_FRAMEBUFFER_EXT && target != GL_READ_FRAMEBUFFER_EXT)
	{
		return error(GL_INVALID_ENUM);
	}

	switch(attachment)
	{
	case GL_COLOR_ATTACHMENT0:
	case GL_DEPTH_ATTACHMENT:
	case GL_STENCIL_ATTACHMENT:
		break;
	default:
		return error(GL_INVALID_ENUM);
	}

	gl::Context *context = gl::getContext();

	if(context)
	{
		if(context->getListIndex() != 0)
		{
			UNIMPLEMENTED();
		}

		if(texture == 0)
		{
			textarget = GL_NONE;
		}
		else
		{
			gl::Texture *tex = context->getTexture(texture);

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

			if(level != 0)
			{
				return error(GL_INVALID_VALUE);
			}
		}

		gl::Framebuffer *framebuffer = NULL;
		GLuint framebufferName = 0;
		if(target == GL_READ_FRAMEBUFFER_EXT)
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

		switch(attachment)
		{
		case GL_COLOR_ATTACHMENT0:  framebuffer->setColorbuffer(textarget, texture);   break;
		case GL_DEPTH_ATTACHMENT:   framebuffer->setDepthbuffer(textarget, texture);   break;
		case GL_STENCIL_ATTACHMENT: framebuffer->setStencilbuffer(textarget, texture); break;
		}
	}
}

void APIENTRY glFramebufferTexture3D(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level, GLint zoffset)
{
	UNIMPLEMENTED();
}

void APIENTRY glFrontFace(GLenum mode)
{
	TRACE("(GLenum mode = 0x%X)", mode);

	switch(mode)
	{
	case GL_CW:
	case GL_CCW:
	{
		gl::Context *context = gl::getContext();

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

void APIENTRY glGenBuffers(GLsizei n, GLuint* buffers)
{
	TRACE("(GLsizei n = %d, GLuint* buffers = %p)", n, buffers);

	if(n < 0)
	{
		return error(GL_INVALID_VALUE);
	}

	gl::Context *context = gl::getContext();

	if(context)
	{
		for(int i = 0; i < n; i++)
		{
			buffers[i] = context->createBuffer();
		}
	}
}

void APIENTRY glGenerateMipmap(GLenum target)
{
	TRACE("(GLenum target = 0x%X)", target);

	gl::Context *context = gl::getContext();

	if(context)
	{
		if(context->getListIndex() != 0)
		{
			UNIMPLEMENTED();
		}

		gl::Texture *texture;

		switch(target)
		{
		case GL_TEXTURE_2D:
			texture = context->getTexture2D(target);
			break;
		case GL_TEXTURE_CUBE_MAP:
			texture = context->getTextureCubeMap();
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

void APIENTRY glGenFencesNV(GLsizei n, GLuint* fences)
{
	TRACE("(GLsizei n = %d, GLuint* fences = %p)", n, fences);

	if(n < 0)
	{
		return error(GL_INVALID_VALUE);
	}

	gl::Context *context = gl::getContext();

	if(context)
	{
		if(context->getListIndex() != 0)
		{
			UNIMPLEMENTED();
		}

		for(int i = 0; i < n; i++)
		{
			fences[i] = context->createFence();
		}
	}
}

void APIENTRY glGenFramebuffers(GLsizei n, GLuint* framebuffers)
{
	TRACE("(GLsizei n = %d, GLuint* framebuffers = %p)", n, framebuffers);

	if(n < 0)
	{
		return error(GL_INVALID_VALUE);
	}

	gl::Context *context = gl::getContext();

	if(context)
	{
		if(context->getListIndex() != 0)
		{
			UNIMPLEMENTED();
		}

		for(int i = 0; i < n; i++)
		{
			framebuffers[i] = context->createFramebuffer();
		}
	}
}

void APIENTRY glGenQueriesEXT(GLsizei n, GLuint* ids)
{
	TRACE("(GLsizei n = %d, GLuint* ids = %p)", n, ids);

	if(n < 0)
	{
		return error(GL_INVALID_VALUE);
	}

	gl::Context *context = gl::getContext();

	if(context)
	{
		for(int i = 0; i < n; i++)
		{
			ids[i] = context->createQuery();
		}
	}
}

void APIENTRY glGenRenderbuffers(GLsizei n, GLuint* renderbuffers)
{
	TRACE("(GLsizei n = %d, GLuint* renderbuffers = %p)", n, renderbuffers);

	if(n < 0)
	{
		return error(GL_INVALID_VALUE);
	}

	gl::Context *context = gl::getContext();

	if(context)
	{
		if(context->getListIndex() != 0)
		{
			UNIMPLEMENTED();
		}

		for(int i = 0; i < n; i++)
		{
			renderbuffers[i] = context->createRenderbuffer();
		}
	}
}

void APIENTRY glGenTextures(GLsizei n, GLuint* textures)
{
	TRACE("(GLsizei n = %d, GLuint* textures = %p)", n, textures);

	if(n < 0)
	{
		return error(GL_INVALID_VALUE);
	}

	gl::Context *context = gl::getContext();

	if(context)
	{
		for(int i = 0; i < n; i++)
		{
			textures[i] = context->createTexture();
		}
	}
}

void APIENTRY glGetActiveAttrib(GLuint program, GLuint index, GLsizei bufsize, GLsizei *length, GLint *size, GLenum *type, GLchar *name)
{
	TRACE("(GLuint program = %d, GLuint index = %d, GLsizei bufsize = %d, GLsizei *length = %p, "
		"GLint *size = %p, GLenum *type = %p, GLchar *name = %p)",
		program, index, bufsize, length, size, type, name);

	if(bufsize < 0)
	{
		return error(GL_INVALID_VALUE);
	}

	gl::Context *context = gl::getContext();

	if(context)
	{
		gl::Program *programObject = context->getProgram(program);

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

		if(index >= (GLuint)programObject->getActiveAttributeCount())
		{
			return error(GL_INVALID_VALUE);
		}

		programObject->getActiveAttribute(index, bufsize, length, size, type, name);
	}
}

void APIENTRY glGetActiveUniform(GLuint program, GLuint index, GLsizei bufsize, GLsizei* length, GLint* size, GLenum* type, GLchar* name)
{
	TRACE("(GLuint program = %d, GLuint index = %d, GLsizei bufsize = %d, "
		"GLsizei* length = %p, GLint* size = %p, GLenum* type = %p, GLchar* name = %s)",
		program, index, bufsize, length, size, type, name);

	if(bufsize < 0)
	{
		return error(GL_INVALID_VALUE);
	}

	gl::Context *context = gl::getContext();

	if(context)
	{
		gl::Program *programObject = context->getProgram(program);

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

		if(index >= (GLuint)programObject->getActiveUniformCount())
		{
			return error(GL_INVALID_VALUE);
		}

		programObject->getActiveUniform(index, bufsize, length, size, type, name);
	}
}

void APIENTRY glGetAttachedShaders(GLuint program, GLsizei maxcount, GLsizei* count, GLuint* shaders)
{
	TRACE("(GLuint program = %d, GLsizei maxcount = %d, GLsizei* count = %p, GLuint* shaders = %p)",
		program, maxcount, count, shaders);

	if(maxcount < 0)
	{
		return error(GL_INVALID_VALUE);
	}

	gl::Context *context = gl::getContext();

	if(context)
	{
		gl::Program *programObject = context->getProgram(program);

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

int APIENTRY glGetAttribLocation(GLuint program, const GLchar* name)
{
	TRACE("(GLuint program = %d, const GLchar* name = %s)", program, name);

	gl::Context *context = gl::getContext();

	if(context)
	{
		gl::Program *programObject = context->getProgram(program);

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

void APIENTRY glGetBooleanv(GLenum pname, GLboolean* params)
{
	TRACE("(GLenum pname = 0x%X, GLboolean* params = %p)", pname, params);

	gl::Context *context = gl::getContext();

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

				delete[] floatParams;
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

				delete[] intParams;
			}
		}
	}
}

void APIENTRY glGetBufferParameteriv(GLenum target, GLenum pname, GLint* params)
{
	TRACE("(GLenum target = 0x%X, GLenum pname = 0x%X, GLint* params = %p)", target, pname, params);

	gl::Context *context = gl::getContext();

	if(context)
	{
		gl::Buffer *buffer;

		switch(target)
		{
		case GL_ARRAY_BUFFER:
			buffer = context->getArrayBuffer();
			break;
		case GL_ELEMENT_ARRAY_BUFFER:
			buffer = context->getElementArrayBuffer();
			break;
		default:
			return error(GL_INVALID_ENUM);
		}

		if(!buffer)
		{
			// A null buffer means that "0" is bound to the requested buffer target
			return error(GL_INVALID_OPERATION);
		}

		switch(pname)
		{
		case GL_BUFFER_USAGE:
			*params = buffer->usage();
			break;
		case GL_BUFFER_SIZE:
			*params = buffer->size();
			break;
		default:
			return error(GL_INVALID_ENUM);
		}
	}
}

GLenum APIENTRY glGetError(void)
{
	TRACE("()");

	gl::Context *context = gl::getContext();

	if(context)
	{
		return context->getError();
	}

	return GL_NO_ERROR;
}

void APIENTRY glGetFenceivNV(GLuint fence, GLenum pname, GLint *params)
{
	TRACE("(GLuint fence = %d, GLenum pname = 0x%X, GLint *params = %p)", fence, pname, params);

	gl::Context *context = gl::getContext();

	if(context)
	{
		gl::Fence *fenceObject = context->getFence(fence);

		if(fenceObject == NULL)
		{
			return error(GL_INVALID_OPERATION);
		}

		fenceObject->getFenceiv(pname, params);
	}
}

void APIENTRY glGetFloatv(GLenum pname, GLfloat* params)
{
	TRACE("(GLenum pname = 0x%X, GLfloat* params = %p)", pname, params);

	gl::Context *context = gl::getContext();

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

				delete[] boolParams;
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

				delete[] intParams;
			}
		}
	}
}

void APIENTRY glGetFramebufferAttachmentParameteriv(GLenum target, GLenum attachment, GLenum pname, GLint* params)
{
	TRACE("(GLenum target = 0x%X, GLenum attachment = 0x%X, GLenum pname = 0x%X, GLint* params = %p)",
		target, attachment, pname, params);

	gl::Context *context = gl::getContext();

	if(context)
	{
		if(target != GL_FRAMEBUFFER && target != GL_DRAW_FRAMEBUFFER_EXT && target != GL_READ_FRAMEBUFFER_EXT)
		{
			return error(GL_INVALID_ENUM);
		}

		gl::Framebuffer *framebuffer = NULL;
		if(target == GL_READ_FRAMEBUFFER_EXT)
		{
			if(context->getReadFramebufferName() == 0)
			{
				return error(GL_INVALID_OPERATION);
			}

			framebuffer = context->getReadFramebuffer();
		}
		else
		{
			if(context->getDrawFramebufferName() == 0)
			{
				return error(GL_INVALID_OPERATION);
			}

			framebuffer = context->getDrawFramebuffer();
		}

		GLenum attachmentType;
		GLuint attachmentHandle;
		switch(attachment)
		{
		case GL_COLOR_ATTACHMENT0:
			attachmentType = framebuffer->getColorbufferType();
			attachmentHandle = framebuffer->getColorbufferName();
			break;
		case GL_DEPTH_ATTACHMENT:
			attachmentType = framebuffer->getDepthbufferType();
			attachmentHandle = framebuffer->getDepthbufferName();
			break;
		case GL_STENCIL_ATTACHMENT:
			attachmentType = framebuffer->getStencilbufferType();
			attachmentHandle = framebuffer->getStencilbufferName();
			break;
		default:
			return error(GL_INVALID_ENUM);
		}

		GLenum attachmentObjectType;   // Type category
		if(attachmentType == GL_NONE || attachmentType == GL_RENDERBUFFER)
		{
			attachmentObjectType = attachmentType;
		}
		else if(gl::IsTextureTarget(attachmentType))
		{
			attachmentObjectType = GL_TEXTURE;
		}
		else UNREACHABLE();

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
				if(gl::IsCubemapTextureTarget(attachmentType))
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
		default:
			return error(GL_INVALID_ENUM);
		}
	}
}

GLenum APIENTRY glGetGraphicsResetStatusEXT(void)
{
	TRACE("()");

	return GL_NO_ERROR;
}

void APIENTRY glGetIntegerv(GLenum pname, GLint* params)
{
	TRACE("(GLenum pname = 0x%X, GLint* params = %p)", pname, params);

	gl::Context *context = gl::getContext();

	if(context)
	{
		if(!(context->getIntegerv(pname, params)))
		{
			GLenum nativeType;
			unsigned int numParams = 0;
			if(!context->getQueryParameterInfo(pname, &nativeType, &numParams))
			{
				return error(GL_INVALID_ENUM);
			}

			if(numParams == 0)
				return; // it is known that pname is valid, but there are no parameters to return

			if(nativeType == GL_BOOL)
			{
				GLboolean *boolParams = NULL;
				boolParams = new GLboolean[numParams];

				context->getBooleanv(pname, boolParams);

				for(unsigned int i = 0; i < numParams; ++i)
				{
					if(boolParams[i] == GL_FALSE)
						params[i] = 0;
					else
						params[i] = 1;
				}

				delete[] boolParams;
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
						params[i] = (GLint)(((GLfloat)(0xFFFFFFFF) * floatParams[i] - 1.0f) / 2.0f);
					}
					else
					{
						params[i] = (GLint)(floatParams[i] > 0.0f ? floor(floatParams[i] + 0.5) : ceil(floatParams[i] - 0.5));
					}
				}

				delete[] floatParams;
			}
		}
	}
}

void APIENTRY glGetProgramiv(GLuint program, GLenum pname, GLint* params)
{
	TRACE("(GLuint program = %d, GLenum pname = 0x%X, GLint* params = %p)", program, pname, params);

	gl::Context *context = gl::getContext();

	if(context)
	{
		gl::Program *programObject = context->getProgram(program);

		if(!programObject)
		{
			return error(GL_INVALID_VALUE);
		}

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
		default:
			return error(GL_INVALID_ENUM);
		}
	}
}

void APIENTRY glGetProgramInfoLog(GLuint program, GLsizei bufsize, GLsizei* length, GLchar* infolog)
{
	TRACE("(GLuint program = %d, GLsizei bufsize = %d, GLsizei* length = %p, GLchar* infolog = %p)",
		program, bufsize, length, infolog);

	if(bufsize < 0)
	{
		return error(GL_INVALID_VALUE);
	}

	gl::Context *context = gl::getContext();

	if(context)
	{
		gl::Program *programObject = context->getProgram(program);

		if(!programObject)
		{
			return error(GL_INVALID_VALUE);
		}

		programObject->getInfoLog(bufsize, length, infolog);
	}
}

void APIENTRY glGetQueryivEXT(GLenum target, GLenum pname, GLint *params)
{
	TRACE("GLenum target = 0x%X, GLenum pname = 0x%X, GLint *params = %p)", target, pname, params);

	switch(pname)
	{
	case GL_CURRENT_QUERY:
		break;
	default:
		return error(GL_INVALID_ENUM);
	}

	gl::Context *context = gl::getContext();

	if(context)
	{
		params[0] = context->getActiveQuery(target);
	}
}

void APIENTRY glGetQueryObjectuivEXT(GLuint name, GLenum pname, GLuint *params)
{
	TRACE("(GLuint name = %d, GLenum pname = 0x%X, GLuint *params = %p)", name, pname, params);

	switch(pname)
	{
	case GL_QUERY_RESULT:
	case GL_QUERY_RESULT_AVAILABLE:
		break;
	default:
		return error(GL_INVALID_ENUM);
	}

	gl::Context *context = gl::getContext();

	if(context)
	{
		gl::Query *queryObject = context->getQuery(name, false, GL_NONE);

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
		case GL_QUERY_RESULT:
			params[0] = queryObject->getResult();
			break;
		case GL_QUERY_RESULT_AVAILABLE:
			params[0] = queryObject->isResultAvailable();
			break;
		default:
			ASSERT(false);
		}
	}
}

void APIENTRY glGetRenderbufferParameteriv(GLenum target, GLenum pname, GLint* params)
{
	TRACE("(GLenum target = 0x%X, GLenum pname = 0x%X, GLint* params = %p)", target, pname, params);

	gl::Context *context = gl::getContext();

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

		gl::Renderbuffer *renderbuffer = context->getRenderbuffer(context->getRenderbufferName());

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
		case GL_RENDERBUFFER_SAMPLES_EXT:     *params = renderbuffer->getSamples();     break;
		default:
			return error(GL_INVALID_ENUM);
		}
	}
}

void APIENTRY glGetShaderiv(GLuint shader, GLenum pname, GLint* params)
{
	TRACE("(GLuint shader = %d, GLenum pname = %d, GLint* params = %p)", shader, pname, params);

	gl::Context *context = gl::getContext();

	if(context)
	{
		gl::Shader *shaderObject = context->getShader(shader);

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

void APIENTRY glGetShaderInfoLog(GLuint shader, GLsizei bufsize, GLsizei* length, GLchar* infolog)
{
	TRACE("(GLuint shader = %d, GLsizei bufsize = %d, GLsizei* length = %p, GLchar* infolog = %p)",
		shader, bufsize, length, infolog);

	if(bufsize < 0)
	{
		return error(GL_INVALID_VALUE);
	}

	gl::Context *context = gl::getContext();

	if(context)
	{
		gl::Shader *shaderObject = context->getShader(shader);

		if(!shaderObject)
		{
			return error(GL_INVALID_VALUE);
		}

		shaderObject->getInfoLog(bufsize, length, infolog);
	}
}

void APIENTRY glGetShaderPrecisionFormat(GLenum shadertype, GLenum precisiontype, GLint* range, GLint* precision)
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
		// Single-precision floating-point numbers can accurately represent integers up to +/-16777216
		range[0] = 24;
		range[1] = 24;
		*precision = 0;
		break;
	default:
		return error(GL_INVALID_ENUM);
	}
}

void APIENTRY glGetShaderSource(GLuint shader, GLsizei bufsize, GLsizei* length, GLchar* source)
{
	TRACE("(GLuint shader = %d, GLsizei bufsize = %d, GLsizei* length = %p, GLchar* source = %p)",
		shader, bufsize, length, source);

	if(bufsize < 0)
	{
		return error(GL_INVALID_VALUE);
	}

	gl::Context *context = gl::getContext();

	if(context)
	{
		gl::Shader *shaderObject = context->getShader(shader);

		if(!shaderObject)
		{
			return error(GL_INVALID_OPERATION);
		}

		shaderObject->getSource(bufsize, length, source);
	}
}

const GLubyte* APIENTRY glGetString(GLenum name)
{
	TRACE("(GLenum name = 0x%X)", name);

	switch(name)
	{
	case GL_VENDOR:
		return (GLubyte*)"NVIDIA Corporation";// "TransGaming Inc.";
	case GL_RENDERER:
		return (GLubyte*)"Quadro K600/PCIe/SSE2";// "SwiftShader";
	case GL_VERSION:
		return (GLubyte*)"2.1.2 NVIDIA 347.62";//"2.1.2 SwiftShader "VERSION_STRING;
	case GL_SHADING_LANGUAGE_VERSION:
		return (GLubyte*)/*"3.0.0 NVIDIA "VERSION_STRING;*/"4.50 NVIDIA";
	case GL_EXTENSIONS:
		// Keep list sorted in following order:
		// OES extensions
		// EXT extensions
		// Vendor extensions
		return (GLubyte*)
			////1.1
			//"GL_EXT_blend_logic_op "
			//"GL_EXT_copy_texture "
			//"GL_EXT_polygon_offset "
			//"GL_EXT_subtexture "
			//"GL_EXT_texture "
			//"GL_EXT_texture_object "
			////"GL_EXT_vertex_array "
			////1.2
			//"GL_EXT_bgra "
			//"GL_EXT_draw_range_elements "
			//"GL_EXT_packed_pixels "
			//"GL_EXT_rescale_normal "
			//"GL_EXT_separate_specular_color "
			//"GL_EXT_texture3D "
			//"GL_EXT_texture_edge_clamp "
			//"GL_SGIS_texture_edge_clamp "
			//"GL_SGIS_texture_lod "
			////1.3
			//"GL_ARB_multisample "
			//"GL_ARB_multitexture "
			//"GL_ARB_texture_border_clamp "
			//"GL_ARB_texture_compression "
			//"GL_ARB_texture_cube_map "
			//"GL_ARB_texture_env_add "
			//"GL_ARB_texture_env_combine "
			//"GL_ARB_texture_env_dot3 "
			//"GL_ARB_transpose_matrix "
			////1.4
			//"GL_ARB_depth_texture "
			//"GL_ARB_point_parameters "
			//"GL_ARB_shadow "
			//"GL_ARB_texture_env_crossbar "
			//"GL_ARB_texture_mirrored_repeat "
			//"GL_ARB_windows_pos "
			//"GL_EXT_blend_color "
			//"GL_EXT_blend_func_separate "
			//"GL_EXT_fog_coord "
			//"GL_EXT_multi_draw_arrays "
			//"GL_EXT_secondary_color "
			//"GL_EXT_stencil_wrap "
			//"GL_EXT_texture_lod_bias "
			//"GL_NV_blend_square "
			//"GL_SGIS_generate_mipmap "
			////1.5
			//"GL_ARB_occlusion_query "
			//"GL_ARB_vertex_buffer_object "
			//"GL_EXT_shadow_funcs "
			////2.0
			//"GL_ARB_draw_buffers "
			//"GL_ARB_fragment_shader "
			//"GL_ARB_point_sprite "
			//"GL_ARB_shader objects "
			//"GL_ARB_shading_language_100 "
			//"GL_ARB_texture_non_power_of_two "
			//"GL_ARB_vertex_shader "
			//"GL_EXT_blend_equation_separate "
			//"GL_EXT_stencil_two_side "

			////2.1
			//"GL_ARB_pixel_buffer_object "
			//"GL_EXT_texture_sRGB "

			//"GL_EXT_framebuffer_object "
			//"GL_ARB_framebuffer_object "
			//"GL_ARB_shader_atomic_counters "
			//"GL_ARB_shader_bit_encoding "
			//"GL_ARB_shader_draw_parameters "
			//"GL_ARB_shader_group_vote "
			//"GL_ARB_shader_image_load_store "
			//"GL_ARB_shader_image_size "
			//"GL_ARB_shader_objects "
			//"GL_ARB_shader_precision "
			//"GL_ARB_query_buffer_object "
			//"GL_ARB_shader_storage_buffer_object "
			//"GL_ARB_shader_subroutine "
			//"GL_ARB_shader_texture_lod "
			//"GL_ARB_shadow "
			//"GL_ARB_shading_language_420pack "
			//"GL_ARB_shading_language_include "
			//"GL_ARB_shading_language_packing ";


			//NVIDIA CARD EXTENSIONS
			"GL_AMD_multi_draw_indirect "
			"GL_AMD_seamless_cubemap_per_texture "
			"GL_ARB_arrays_of_arrays "
			"GL_ARB_base_instance "
			"GL_ARB_bindless_texture "
			"GL_ARB_blend_func_extended "
			"GL_ARB_buffer_storage "
			"GL_ARB_clear_buffer_object "
			"GL_ARB_clear_texture "
			"GL_ARB_clip_control "
			"GL_ARB_color_buffer_float "
			"GL_ARB_compressed_texture_pixel_storage "
			"GL_ARB_compute_shader "
			"GL_ARB_compute_variable_group_size "
			"GL_ARB_conditional_render_inverted "
			"GL_ARB_conservative_depth "
			"GL_ARB_copy_buffer "
			"GL_ARB_copy_image "
			"GL_ARB_cull_distance "
			"GL_ARB_debug_output "
			"GL_ARB_depth_buffer_float "
			"GL_ARB_depth_clamp "
			"GL_ARB_depth_texture "
			"GL_ARB_derivative_control "
			"GL_ARB_direct_state_access "
			"GL_ARB_draw_buffers "
			"GL_ARB_draw_buffers_blend "
			"GL_ARB_draw_elements_base_vertex "
			"GL_ARB_draw_indirect "
			"GL_ARB_draw_instanced "
			"GL_ARB_enhanced_layouts "
			"GL_ARB_ES2_compatibility "
			"GL_ARB_ES3_1_compatibility "
			"GL_ARB_ES3_compatibility "
			"GL_ARB_explicit_attrib_location "
			"GL_ARB_explicit_uniform_location "
			"GL_ARB_fragment_coord_conventions "
			"GL_ARB_fragment_layer_viewport "
			"GL_ARB_fragment_program "
			"GL_ARB_fragment_program_shadow "
			"GL_ARB_fragment_shader "
			"GL_ARB_framebuffer_no_attachments "
			"GL_ARB_framebuffer_object "
			"GL_ARB_framebuffer_sRGB "
			"GL_ARB_geometry_shader4 "
			"GL_ARB_get_program_binary "
			"GL_ARB_get_texture_sub_image "
			"GL_ARB_gpu_shader5 "
			"GL_ARB_gpu_shader_fp64 "
			"GL_ARB_half_float_pixel "
			"GL_ARB_half_float_vertex "
			"GL_ARB_imaging "
			"GL_ARB_indirect_parameters "
			"GL_ARB_instanced_arrays "
			"GL_ARB_internalformat_query "
			"GL_ARB_internalformat_query2 "
			"GL_ARB_invalidate_subdata "
			"GL_ARB_map_buffer_alignment "
			"GL_ARB_map_buffer_range "
			"GL_ARB_multi_bind "
			"GL_ARB_multi_draw_indirect "
			"GL_ARB_multisample "
			"GL_ARB_multitexture "
			"GL_ARB_occlusion_query "
			"GL_ARB_occlusion_query2 "
			"GL_ARB_pipeline_statistics_query "
			"GL_ARB_pixel_buffer_object "
			"GL_ARB_point_parameters "
			"GL_ARB_point_sprite "
			"GL_ARB_program_interface_query "
			"GL_ARB_provoking_vertex "
			"GL_ARB_query_buffer_object "
			"GL_ARB_robust_buffer_access_behavior "
			"GL_ARB_robustness "
			"GL_ARB_sample_shading "
			"GL_ARB_sampler_objects "
			"GL_ARB_seamless_cube_map "
			"GL_ARB_seamless_cubemap_per_texture "
			"GL_ARB_separate_shader_objects "
			"GL_ARB_shader_atomic_counters "
			"GL_ARB_shader_bit_encoding "
			"GL_ARB_shader_draw_parameters "
			"GL_ARB_shader_group_vote "
			"GL_ARB_shader_image_load_store "
			"GL_ARB_shader_image_size "
			"GL_ARB_shader_objects "
			"GL_ARB_shader_precision "
			"GL_ARB_shader_storage_buffer_object "
			"GL_ARB_shader_subroutine "
			"GL_ARB_shader_texture_image_samples "
			"GL_ARB_shader_texture_lod "
			"GL_ARB_shading_language_100 "
			"GL_ARB_shading_language_420pack "
			"GL_ARB_shading_language_include "
			"GL_ARB_shading_language_packing "
			"GL_ARB_shadow "
			"GL_ARB_sparse_buffer "
			"GL_ARB_sparse_texture "
			"GL_ARB_stencil_texturing "
			"GL_ARB_sync "
			"GL_ARB_tessellation_shader "
			"GL_ARB_texture_barrier "
			"GL_ARB_texture_border_clamp "
			"GL_ARB_texture_buffer_object "
			"GL_ARB_texture_buffer_object_rgb32 "
			"GL_ARB_texture_buffer_range "
			"GL_ARB_texture_compression "
			"GL_ARB_texture_compression_bptc "
			"GL_ARB_texture_compression_rgtc "
			"GL_ARB_texture_cube_map "
			"GL_ARB_texture_cube_map_array "
			"GL_ARB_texture_env_add "
			"GL_ARB_texture_env_combine "
			"GL_ARB_texture_env_crossbar "
			"GL_ARB_texture_env_dot3 "
			"GL_ARB_texture_float "
			"GL_ARB_texture_gather "
			"GL_ARB_texture_mirror_clamp_to_edge "
			"GL_ARB_texture_mirrored_repeat "
			"GL_ARB_texture_multisample "
			"GL_ARB_texture_non_power_of_two "
			"GL_ARB_texture_query_levels "
			"GL_ARB_texture_query_lod "
			"GL_ARB_texture_rectangle "
			"GL_ARB_texture_rg "
			"GL_ARB_texture_rgb10_a2ui "
			"GL_ARB_texture_stencil8 "
			"GL_ARB_texture_storage "
			"GL_ARB_texture_storage_multisample "
			"GL_ARB_texture_swizzle "
			"GL_ARB_texture_view "
			"GL_ARB_timer_query "
			"GL_ARB_transform_feedback2 "
			"GL_ARB_transform_feedback3 "
			"GL_ARB_transform_feedback_instanced "
			"GL_ARB_transform_feedback_overflow_query "
			"GL_ARB_transpose_matrix "
			"GL_ARB_uniform_buffer_object "
			"GL_ARB_vertex_array_bgra "
			"GL_ARB_vertex_array_object "
			"GL_ARB_vertex_attrib_64bit "
			"GL_ARB_vertex_attrib_binding "
			"GL_ARB_vertex_buffer_object "
			"GL_ARB_vertex_program "
			"GL_ARB_vertex_shader "
			"GL_ARB_vertex_type_10f_11f_11f_rev "
			"GL_ARB_vertex_type_2_10_10_10_rev "
			"GL_ARB_viewport_array "
			"GL_ARB_window_pos "
			"GL_ATI_draw_buffers "
			"GL_ATI_texture_float "
			"GL_ATI_texture_mirror_once "
			"GL_EXT_abgr "
			"GL_EXT_bgra "
			"GL_EXT_bindable_uniform "
			"GL_EXT_blend_color "
			"GL_EXT_blend_equation_separate "
			"GL_EXT_blend_func_separate "
			"GL_EXT_blend_minmax "
			"GL_EXT_blend_subtract "
			"GL_EXT_Cg_shader "
			"GL_EXT_compiled_vertex_array "
			"GL_EXT_depth_bounds_test "
			"GL_EXT_direct_state_access "
			"GL_EXT_draw_buffers2 "
			"GL_EXT_draw_instanced "
			"GL_EXT_draw_range_elements "
			"GL_EXT_fog_coord "
			"GL_EXT_framebuffer_blit "
			"GL_EXT_framebuffer_multisample "
			"GL_EXT_framebuffer_multisample_blit_scaled "
			"GL_EXT_framebuffer_object "
			"GL_EXT_framebuffer_sRGB "
			"GL_EXT_geometry_shader4 "
			"GL_EXT_gpu_program_parameters "
			"GL_EXT_gpu_shader4 "
			"GL_EXT_import_sync_object "
			"GL_EXT_multi_draw_arrays "
			"GL_EXT_packed_depth_stencil "
			"GL_EXT_packed_float "
			//"GL_EXT_packed_pixels "
			"GL_EXT_pixel_buffer_object "
			"GL_EXT_point_parameters "
			"GL_EXT_polygon_offset_clamp "
			"GL_EXT_provoking_vertex "
			"GL_EXT_rescale_normal "
			"GL_EXT_secondary_color "
			"GL_EXT_separate_shader_objects "
			"GL_EXT_separate_specular_color "
			"GL_EXT_shader_image_load_store "
			"GL_EXT_shader_integer_mix "
			"GL_EXT_shadow_funcs "
			"GL_EXT_stencil_two_side "
			"GL_EXT_stencil_wrap "
			"GL_EXT_texture3D "
			"GL_EXT_texture_array "
			"GL_EXT_texture_buffer_object "
			"GL_EXT_texture_compression_dxt1 "
			"GL_EXT_texture_compression_latc "
			"GL_EXT_texture_compression_rgtc "
			"GL_EXT_texture_compression_s3tc "
			"GL_EXT_texture_cube_map "
			"GL_EXT_texture_edge_clamp "
			"GL_EXT_texture_env_add "
			"GL_EXT_texture_env_combine "
			"GL_EXT_texture_env_dot3 "
			"GL_EXT_texture_filter_anisotropic "
			"GL_EXT_texture_integer "
			"GL_EXT_texture_lod "
			"GL_EXT_texture_lod_bias "
			"GL_EXT_texture_mirror_clamp "
			"GL_EXT_texture_object "
			"GL_EXT_texture_shared_exponent "
			"GL_EXT_texture_sRGB "
			"GL_EXT_texture_sRGB_decode "
			"GL_EXT_texture_storage "
			"GL_EXT_texture_swizzle "
			"GL_EXT_timer_query "
			"GL_EXT_transform_feedback2 "
			"GL_EXT_vertex_array "
			"GL_EXT_vertex_array_bgra "
			"GL_EXT_vertex_attrib_64bit "
			"GL_EXTX_framebuffer_mixed_formats "
			"GL_IBM_rasterpos_clip "
			"GL_IBM_texture_mirrored_repeat "
			"GL_KHR_blend_equation_advanced "
			"GL_KHR_context_flush_control "
			"GL_KHR_debug "
			"GL_KHR_robust_buffer_access_behavior "
			"GL_KHR_robustness "
			"GL_KTX_buffer_region "
			"GL_NV_bindless_multi_draw_indirect "
			"GL_NV_bindless_multi_draw_indirect_count "
			"GL_NV_bindless_texture "
			"GL_NV_blend_equation_advanced "
			"GL_NV_blend_square "
			"GL_NV_command_list "
			"GL_NV_compute_program5 "
			"GL_NV_conditional_render "
			"GL_NV_copy_depth_to_color "
			"GL_NV_copy_image "
			"GL_NV_deep_texture3D "
			"GL_NV_depth_buffer_float "
			"GL_NV_depth_clamp "
			"GL_NV_draw_texture "
			"GL_NV_ES1_1_compatibility "
			"GL_NV_ES3_1_compatibility "
			"GL_NV_explicit_multisample "
			"GL_NV_fence "
			"GL_NV_float_buffer "
			"GL_NV_fog_distance "
			"GL_NV_fragment_program "
			"GL_NV_fragment_program2 "
			"GL_NV_fragment_program_option "
			"GL_NV_framebuffer_multisample_coverage "
			"GL_NV_geometry_shader4 "
			"GL_NV_gpu_program4 "
			"GL_NV_gpu_program4_1 "
			"GL_NV_gpu_program5 "
			"GL_NV_gpu_program5_mem_extended "
			"GL_NV_gpu_program_fp64 "
			"GL_NV_gpu_shader5 "
			"GL_NV_half_float "
			"GL_NV_internalformat_sample_query "
			"GL_NV_light_max_exponent "
			"GL_NV_multisample_coverage "
			"GL_NV_multisample_filter_hint "
			"GL_NV_occlusion_query "
			"GL_NV_packed_depth_stencil "
			"GL_NV_parameter_buffer_object "
			"GL_NV_parameter_buffer_object2 "
			"GL_NV_path_rendering "
			"GL_NV_pixel_data_range "
			"GL_NV_point_sprite "
			"GL_NV_primitive_restart "
			"GL_NV_register_combiners "
			"GL_NV_register_combiners2 "
			"GL_NV_shader_atomic_counters "
			"GL_NV_shader_atomic_float "
			"GL_NV_shader_buffer_load "
			"GL_NV_shader_storage_buffer_object "
			"GL_NV_shader_thread_group "
			"GL_NV_shader_thread_shuffle "
			"GL_NV_texgen_reflection "
			"GL_NV_texture_barrier "
			"GL_NV_texture_compression_vtc "
			"GL_NV_texture_env_combine4 "
			"GL_NV_texture_multisample "
			"GL_NV_texture_rectangle "
			"GL_NV_texture_shader "
			"GL_NV_texture_shader2 "
			"GL_NV_texture_shader3 "
			"GL_NV_transform_feedback "
			"GL_NV_transform_feedback2 "
			"GL_NV_uniform_buffer_unified_memory "
			"GL_NV_vertex_array_range "
			"GL_NV_vertex_array_range2 "
			"GL_NV_vertex_attrib_integer_64bit "
			"GL_NV_vertex_buffer_unified_memory "
			"GL_NV_vertex_program "
			"GL_NV_vertex_program1_1 "
			"GL_NV_vertex_program2 "
			"GL_NV_vertex_program2_option "
			"GL_NV_vertex_program3 "
			"GL_NV_video_capture "
			"GL_NVX_conditional_render "
			"GL_NVX_gpu_memory_info "
			"GL_NVX_nvenc_interop "
			"GL_S3_s3tc "
			"GL_SGIS_generate_mipmap "
			"GL_SGIS_texture_lod "
			"GL_SGIX_depth_texture "
			"GL_SGIX_shadow "
			"GL_SUN_slice_accum "
			//"GL_WIN_swap_hint "
			"WGL_ARB_buffer_region "
			"WGL_ARB_context_flush_control "
			"WGL_ARB_create_context "
			"WGL_ARB_create_context_profile "
			"WGL_ARB_create_context_robustness "
			"WGL_ARB_extensions_string "
			"WGL_ARB_make_current_read "
			"WGL_ARB_multisample "
			"WGL_ARB_pbuffer "
			"WGL_ARB_pixel_format "
			"WGL_ARB_pixel_format_float "
			"WGL_ARB_render_texture "
			"WGL_ATI_pixel_format_float "
			"WGL_EXT_create_context_es2_profile "
			"WGL_EXT_create_context_es_profile "
			"WGL_EXT_extensions_string "
			"WGL_EXT_framebuffer_sRGB "
			"WGL_EXT_pixel_format_packed_float "
			"WGL_EXT_swap_control "
			"WGL_EXT_swap_control_tear "
			"WGL_NV_copy_image "
			"WGL_NV_delay_before_swap "
			"WGL_NV_DX_interop "
			"WGL_NV_DX_interop2 "
			"WGL_NV_float_buffer "
			"WGL_NV_gpu_affinity "
			"WGL_NV_multisample_coverage "
			"WGL_NV_render_depth_texture "
			"WGL_NV_render_texture_rectangle "
			"WGL_NV_swap_group "
			"WGL_NV_video_capture "
			"WGL_NVX_DX_interop";


		/*"WGL_ARB_extensions_string "
		"GL_ARB_extensions_string "
		"WGL_ARB_create_context "
		"GL_ARB_create_context "
		"GL_ARB_texture_non_power_of_two "
		"GL_ARB_framebuffer_object "
		"GL_ARB_fragment_shader "
		"GL_ARB_vertex_shader "
		"GL_ARB_shader_objects "
		"GL_ARB_shading_language_100 "
		"GL_EXT_framebuffer_object "
		"GL_EXT_blend_minmax "
		"GL_EXT_depth_texture "
		"GL_EXT_depth_texture_cube_map "
		"GL_EXT_element_index_uint "
		"GL_EXT_packed_depth_stencil "
		"GL_EXT_rgb8_rgba8 "
		"GL_EXT_standard_derivatives "
		"GL_EXT_texture_float "
		"GL_EXT_texture_float_linear "
		"GL_EXT_texture_half_float "
		"GL_EXT_texture_half_float_linear "
		"GL_EXT_texture_npot "
		"GL_EXT_occlusion_query_boolean "
		"GL_EXT_read_format_bgra "
		#if (S3TC_SUPPORT)
		"GL_EXT_texture_compression_dxt1 "
		#endif
		"GL_EXT_blend_func_separate "
		"GL_EXT_secondary_color "
		"GL_EXT_texture_filter_anisotropic "
		"GL_EXT_texture_format_BGRA8888 "
		"GL_EXT_framebuffer_blit "
		"GL_EXT_framebuffer_multisample "
		#if (S3TC_SUPPORT)
		"GL_EXT_texture_compression_dxt3 "
		"GL_EXT_texture_compression_dxt5 "
		#endif
		"GL_NV_fence";*/



		/*"GL_AMD_multi_draw_indirect "
		"GL_AMD_seamless_cubemap_per_texture "
		"GL_ARB_arrays_of_arrays "
		"GL_ARB_base_instance "
		"GL_ARB_bindless_texture "
		"GL_ARB_blend_func_extended "
		"GL_ARB_buffer_storage "
		"GL_ARB_clear_buffer_object "
		"GL_ARB_clear_texture "
		"GL_ARB_color_buffer_float "
		"GL_ARB_compatibility "
		"GL_ARB_compressed_texture_pixel_storage "
		"GL_ARB_conservative_depth "
		"GL_ARB_compute_shader "
		"GL_ARB_compute_variable_group_size "
		"GL_ARB_copy_buffer "
		"GL_ARB_copy_image "
		"GL_ARB_debug_output "
		"GL_ARB_depth_buffer_float "
		"GL_ARB_depth_clamp "
		"GL_ARB_depth_texture "
		"GL_ARB_draw_buffers "
		"GL_ARB_draw_buffers_blend "
		"GL_ARB_draw_indirect "
		"GL_ARB_draw_elements_base_vertex "
		"GL_ARB_draw_instanced "
		"GL_ARB_enhanced_layouts "
		"GL_ARB_ES2_compatibility "
		"GL_ARB_ES3_compatibility "
		"GL_ARB_explicit_attrib_location "
		"GL_ARB_explicit_uniform_location "
		"GL_ARB_fragment_coord_conventions "
		"GL_ARB_fragment_layer_viewport "
		"GL_ARB_fragment_program "
		"GL_ARB_fragment_program_shadow "
		"GL_ARB_fragment_shader "
		"GL_ARB_framebuffer_no_attachments "
		"GL_ARB_framebuffer_object "
		"GL_ARB_framebuffer_sRGB "
		"GL_ARB_geometry_shader4 "
		"GL_ARB_get_program_binary "
		"GL_ARB_gpu_shader5 "
		"GL_ARB_gpu_shader_fp64 "
		"GL_ARB_half_float_pixel "
		"GL_ARB_half_float_vertex "
		"GL_ARB_indirect_parameters "
		"GL_ARB_instanced_arrays "
		"GL_ARB_internalformat_query "
		"GL_ARB_internalformat_query2 "
		"GL_ARB_invalidate_subdata "
		"GL_ARB_map_buffer_alignment "
		"GL_ARB_map_buffer_range "
		"GL_ARB_multi_bind "
		"GL_ARB_multi_draw_indirect "
		"GL_ARB_multisample "
		"GL_ARB_multitexture "
		"GL_ARB_occlusion_query "
		"GL_ARB_occlusion_query2 "
		"GL_ARB_pixel_buffer_object "
		"GL_ARB_point_parameters "
		"GL_ARB_point_sprite "
		"GL_ARB_program_interface_query "
		"GL_ARB_provoking_vertex "
		"GL_ARB_robust_buffer_access_behavior "
		"GL_ARB_robustness "
		"GL_ARB_sample_shading "
		"GL_ARB_sampler_objects "
		"GL_ARB_seamless_cube_map "
		"GL_ARB_seamless_cubemap_per_texture "
		"GL_ARB_separate_shader_objects "
		"GL_ARB_shader_atomic_counters "
		"GL_ARB_shader_bit_encoding "
		"GL_ARB_shader_draw_parameters "
		"GL_ARB_shader_group_vote "
		"GL_ARB_shader_image_load_store "
		"GL_ARB_shader_image_size "
		"GL_ARB_shader_objects "
		"GL_ARB_shader_precision "
		"GL_ARB_query_buffer_object "
		"GL_ARB_shader_storage_buffer_object "
		"GL_ARB_shader_subroutine "
		"GL_ARB_shader_texture_lod "
		"GL_ARB_shading_language_100 "
		"GL_ARB_shading_language_420pack "
		"GL_ARB_shading_language_include "
		"GL_ARB_shading_language_packing "
		"GL_ARB_shadow "
		"GL_ARB_sparse_texture "
		"GL_ARB_stencil_texturing "
		"GL_ARB_sync "
		"GL_ARB_tessellation_shader "
		"GL_ARB_texture_border_clamp "
		"GL_ARB_texture_buffer_object "
		"GL_ARB_texture_buffer_object_rgb32 "
		"GL_ARB_texture_buffer_range "
		"GL_ARB_texture_compression "
		"GL_ARB_texture_compression_bptc "
		"GL_ARB_texture_compression_rgtc "
		"GL_ARB_texture_cube_map "
		"GL_ARB_texture_cube_map_array "
		"GL_ARB_texture_env_add "
		"GL_ARB_texture_env_combine "
		"GL_ARB_texture_env_crossbar "
		"GL_ARB_texture_env_dot3 "
		"GL_ARB_texture_float "
		"GL_ARB_texture_gather "
		"GL_ARB_texture_mirror_clamp_to_edge "
		"GL_ARB_texture_mirrored_repeat "
		"GL_ARB_texture_multisample "
		"GL_ARB_texture_non_power_of_two "
		"GL_ARB_texture_query_levels "
		"GL_ARB_texture_query_lod "
		"GL_ARB_texture_rectangle "
		"GL_ARB_texture_rg "
		"GL_ARB_texture_rgb10_a2ui "
		"GL_ARB_texture_stencil8 "
		"GL_ARB_texture_storage "
		"GL_ARB_texture_storage_multisample "
		"GL_ARB_texture_swizzle "
		"GL_ARB_texture_view "
		"GL_ARB_timer_query "
		"GL_ARB_transform_feedback2 "
		"GL_ARB_transform_feedback3 "
		"GL_ARB_transform_feedback_instanced "
		"GL_ARB_transpose_matrix "
		"GL_ARB_uniform_buffer_object "
		"GL_ARB_vertex_array_bgra "
		"GL_ARB_vertex_array_object "
		"GL_ARB_vertex_attrib_64bit "
		"GL_ARB_vertex_attrib_binding "
		"GL_ARB_vertex_buffer_object "
		"GL_ARB_vertex_program "
		"GL_ARB_vertex_shader "
		"GL_ARB_vertex_type_10f_11f_11f_rev "
		"GL_ARB_vertex_type_2_10_10_10_rev "
		"GL_ARB_viewport_array "
		"GL_ARB_window_pos "
		"GL_ATI_draw_buffers "
		"GL_ATI_texture_float "
		"GL_ATI_texture_mirror_once "
		"GL_S3_s3tc "
		"GL_EXT_abgr "
		"GL_EXT_bgra "
		"GL_EXT_bindable_uniform "
		"GL_EXT_blend_color "
		"GL_EXT_blend_equation_separate "
		"GL_EXT_blend_func_separate "
		"GL_EXT_blend_minmax "
		"GL_EXT_blend_subtract "
		"GL_EXT_compiled_vertex_array "
		"GL_EXT_Cg_shader "
		"GL_EXT_depth_bounds_test "
		"GL_EXT_direct_state_access "
		"GL_EXT_draw_buffers2 "
		"GL_EXT_draw_instanced "
		"GL_EXT_draw_range_elements "
		"GL_EXT_fog_coord "
		"GL_EXT_framebuffer_blit "
		"GL_EXT_framebuffer_multisample "
		"GL_EXT_framebuffer_multisample_blit_scaled "
		"GL_EXT_framebuffer_object "
		"GL_EXT_framebuffer_sRGB "
		"GL_EXT_geometry_shader4 "
		"GL_EXT_gpu_program_parameters "
		"GL_EXT_gpu_shader4 "
		"GL_EXT_multi_draw_arrays "
		"GL_EXT_packed_depth_stencil "
		"GL_EXT_packed_float "
		"GL_EXT_packed_pixels "
		"GL_EXT_pixel_buffer_object "
		"GL_EXT_point_parameters "
		"GL_EXT_post_depth_coverage "
		"GL_EXT_provoking_vertex "
		"GL_EXT_rescale_normal "
		"GL_EXT_raster_multisample "
		"GL_EXT_secondary_color "
		"GL_EXT_separate_shader_objects "
		"GL_EXT_separate_specular_color "
		"GL_EXT_shader_image_load_formatted "
		"GL_EXT_shader_image_load_store "
		"GL_EXT_shader_integer_mix "
		"GL_EXT_shadow_funcs "
		"GL_EXT_sparse_texture2 "
		"GL_EXT_stencil_two_side "
		"GL_EXT_stencil_wrap "
		"GL_EXT_texture3D "
		"GL_EXT_texture_array "
		"GL_EXT_texture_buffer_object "
		"GL_EXT_texture_compression_dxt1 "
		"GL_EXT_texture_compression_latc "
		"GL_EXT_texture_compression_rgtc "
		"GL_EXT_texture_compression_s3tc "
		"GL_EXT_texture_cube_map "
		"GL_EXT_texture_edge_clamp "
		"GL_EXT_texture_env_add "
		"GL_EXT_texture_env_combine "
		"GL_EXT_texture_env_dot3 "
		"GL_EXT_texture_filter_anisotropic "
		"GL_EXT_texture_filter_minmax "
		"GL_EXT_texture_integer "
		"GL_EXT_texture_lod_bias "
		"GL_EXT_texture_mirror_clamp "
		"GL_EXT_texture_object "
		"GL_EXT_texture_shared_exponent "
		"GL_EXT_texture_sRGB "
		"GL_EXT_texture_sRGB_decode "
		"GL_EXT_texture_swizzle "
		"GL_EXT_timer_query "
		"GL_EXT_transform_feedback2 "
		"GL_EXT_vertex_array "
		"GL_EXT_vertex_array_bgra "
		"GL_EXT_vertex_attrib_64bit "
		"GL_EXT_import_sync_object "
		"GL_KHR_debug "
		"GL_NV_bindless_multi_draw_indirect "
		"GL_NV_bindless_texture "
		"GL_NV_blend_equation_advanced "
		"GL_NV_blend_square "
		"GL_NV_command_list "
		"GL_NV_compute_program5 "
		"GL_NV_conditional_render "
		"GL_NV_conservative_raster "
		"GL_NV_copy_depth_to_color "
		"GL_NV_copy_image "
		"GL_NV_deep_texture3D "
		"GL_NV_depth_buffer_float "
		"GL_NV_depth_clamp "
		"GL_NV_draw_texture "
		"GL_NV_explicit_multisample "
		"GL_NV_fence "
		"GL_NV_fill_rectangle "
		"GL_NV_float_buffer "
		"GL_NV_fog_distance "
		"GL_NV_fragment_coverage_to_color "
		"GL_NV_fragment_program "
		"GL_NV_fragment_program_option "
		"GL_NV_fragment_program2 "
		"GL_NV_fragment_shader_interlock "
		"GL_NV_framebuffer_mixed_samples "
		"GL_NV_framebuffer_multisample_coverage "
		"GL_NV_geometry_shader_passthrough "
		"GL_NV_geometry_shader4 "
		"GL_NV_gpu_program4 "
		"GL_NV_gpu_program5 "
		"GL_NV_gpu_program5_mem_extended "
		"GL_NV_gpu_shader5 "
		"GL_NV_half_float "
		"GL_NV_light_max_exponent "
		"GL_NV_multisample_coverage "
		"GL_NV_multisample_filter_hint "
		"GL_NV_occlusion_query "
		"GL_NV_packed_depth_stencil "
		"GL_NV_parameter_buffer_object "
		"GL_NV_parameter_buffer_object2 "
		"GL_NV_path_rendering "
		"GL_NV_path_rendering_shared_edge "
		"GL_NV_pixel_data_range "
		"GL_NV_point_sprite "
		"GL_NV_primitive_restart "
		"GL_NV_register_combiners "
		"GL_NV_register_combiners2 "
		"GL_NV_sample_locations "
		"GL_NV_sample_mask_override_coverage "
		"GL_NV_shader_atomic_counters "
		"GL_NV_shader_atomic_fp16_vector "
		"GL_NV_shader_atomic_float "
		"GL_NV_shader_buffer_load "
		"GL_NV_shader_storage_buffer_object "
		"GL_NV_shader_thread_group "
		"GL_NV_shader_thread_shuffle "
		"GL_NV_texgen_reflection "
		"GL_NV_texture_barrier "
		"GL_NV_texture_compression_vtc "
		"GL_NV_texture_env_combine4 "
		"GL_NV_texture_expand_normal "
		"GL_NV_texture_multisample "
		"GL_NV_texture_rectangle "
		"GL_NV_texture_shader "
		"GL_NV_texture_shader2 "
		"GL_NV_texture_shader3 "
		"GL_NV_transform_feedback "
		"GL_NV_transform_feedback2 "
		"GL_NV_vertex_array_range "
		"GL_NV_vertex_array_range2 "
		"GL_NV_vertex_attrib_integer_64bit "
		"GL_NV_vertex_buffer_unified_memory "
		"GL_NV_vertex_program "
		"GL_NV_vertex_program1_1 "
		"GL_NV_vertex_program2 "
		"GL_NV_vertex_program2_option "
		"GL_NV_vertex_program3 "
		"GL_NV_video_capture "
		"GL_NVX_conditional_render "
		"GL_NVX_gpu_memory_info "
		"GL_SGIS_generate_mipmap "
		"GL_SGIS_texture_lod "
		"GL_SGIX_depth_texture "
		"GL_SGIX_shadow "
		"GL_SUN_slice_accum "
		"WGL_ARB_buffer_region "
		"WGL_ARB_create_context "
		"WGL_ARB_extensions_string "
		"WGL_ARB_pbuffer "
		"WGL_ARB_pixel_format "
		"WGL ARB render texture "
		"WGL ATI pixel_format_float "
		"WGL_EXT_extensions_string "
		"WGL_EXT_pbuffer "
		"WGL_EXT_pixel_format "
		"WGL_EXT_swap_control "
		"WGL_EXT_swap_control_tear "
		"WGL_NV_dx_interop "
		"WGL_NV_gpu_affinity "
		"WGL NV render depth texture "
		"WGL NV render texture rectangle "
		"WGL_NV_swap_group "
		"WGL_NV_video_out";*/



	default:
		return error(GL_INVALID_ENUM, (GLubyte*)NULL);
	}

	return NULL;
}

void APIENTRY glGetTexParameterfv(GLenum target, GLenum pname, GLfloat* params)
{
	TRACE("(GLenum target = 0x%X, GLenum pname = 0x%X, GLfloat* params = %p)", target, pname, params);

	gl::Context *context = gl::getContext();

	if(context)
	{
		gl::Texture *texture;

		switch(target)
		{
		case GL_TEXTURE_2D:
			texture = context->getTexture2D(target);
			break;
		case GL_TEXTURE_CUBE_MAP:
			texture = context->getTextureCubeMap();
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
		case GL_TEXTURE_MAX_ANISOTROPY_EXT:
			*params = texture->getMaxAnisotropy();
			break;
		default:
			return error(GL_INVALID_ENUM);
		}
	}
}

void APIENTRY glGetTexParameteriv(GLenum target, GLenum pname, GLint* params)
{
	TRACE("(GLenum target = 0x%X, GLenum pname = 0x%X, GLint* params = %p)", target, pname, params);

	gl::Context *context = gl::getContext();

	if(context)
	{
		gl::Texture *texture;

		switch(target)
		{
		case GL_TEXTURE_2D:
			texture = context->getTexture2D(target);
			break;
		case GL_TEXTURE_CUBE_MAP:
			texture = context->getTextureCubeMap();
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
		case GL_TEXTURE_MAX_ANISOTROPY_EXT:
			*params = (GLint)texture->getMaxAnisotropy();
			break;
		default:
			return error(GL_INVALID_ENUM);
		}
	}
}

void APIENTRY glGetnUniformfvEXT(GLuint program, GLint location, GLsizei bufSize, GLfloat* params)
{
	TRACE("(GLuint program = %d, GLint location = %d, GLsizei bufSize = %d, GLfloat* params = %p)",
		program, location, bufSize, params);

	if(bufSize < 0)
	{
		return error(GL_INVALID_VALUE);
	}

	gl::Context *context = gl::getContext();

	if(context)
	{
		if(program == 0)
		{
			return error(GL_INVALID_VALUE);
		}

		gl::Program *programObject = context->getProgram(program);

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

void APIENTRY glGetUniformfv(GLuint program, GLint location, GLfloat* params)
{
	TRACE("(GLuint program = %d, GLint location = %d, GLfloat* params = %p)", program, location, params);

	gl::Context *context = gl::getContext();

	if(context)
	{
		if(program == 0)
		{
			return error(GL_INVALID_VALUE);
		}

		gl::Program *programObject = context->getProgram(program);

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

void APIENTRY glGetnUniformivEXT(GLuint program, GLint location, GLsizei bufSize, GLint* params)
{
	TRACE("(GLuint program = %d, GLint location = %d, GLsizei bufSize = %d, GLint* params = %p)",
		program, location, bufSize, params);

	if(bufSize < 0)
	{
		return error(GL_INVALID_VALUE);
	}

	gl::Context *context = gl::getContext();

	if(context)
	{
		if(program == 0)
		{
			return error(GL_INVALID_VALUE);
		}

		gl::Program *programObject = context->getProgram(program);

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

void APIENTRY glGetUniformiv(GLuint program, GLint location, GLint* params)
{
	TRACE("(GLuint program = %d, GLint location = %d, GLint* params = %p)", program, location, params);

	gl::Context *context = gl::getContext();

	if(context)
	{
		if(program == 0)
		{
			return error(GL_INVALID_VALUE);
		}

		gl::Program *programObject = context->getProgram(program);

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

int APIENTRY glGetUniformLocation(GLuint program, const GLchar* name)
{
	TRACE("(GLuint program = %d, const GLchar* name = %s)", program, name);

	gl::Context *context = gl::getContext();

	if(strstr(name, "gl_") == name)
	{
		return -1;
	}

	if(context)
	{
		gl::Program *programObject = context->getProgram(program);

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

void APIENTRY glGetVertexAttribfv(GLuint index, GLenum pname, GLfloat* params)
{
	TRACE("(GLuint index = %d, GLenum pname = 0x%X, GLfloat* params = %p)", index, pname, params);

	gl::Context *context = gl::getContext();

	if(context)
	{
		if(index >= gl::MAX_VERTEX_ATTRIBS)
		{
			return error(GL_INVALID_VALUE);
		}

		const gl::VertexAttribute &attribState = context->getVertexAttribState(index);

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
			for(int i = 0; i < 4; ++i)
			{
				params[i] = attribState.mCurrentValue[i];
			}
			break;
		default: return error(GL_INVALID_ENUM);
		}
	}
}

void APIENTRY glGetVertexAttribiv(GLuint index, GLenum pname, GLint* params)
{
	TRACE("(GLuint index = %d, GLenum pname = 0x%X, GLint* params = %p)", index, pname, params);

	gl::Context *context = gl::getContext();

	if(context)
	{
		if(index >= gl::MAX_VERTEX_ATTRIBS)
		{
			return error(GL_INVALID_VALUE);
		}

		const gl::VertexAttribute &attribState = context->getVertexAttribState(index);

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
			for(int i = 0; i < 4; ++i)
			{
				float currentValue = attribState.mCurrentValue[i];
				params[i] = (GLint)(currentValue > 0.0f ? floor(currentValue + 0.5f) : ceil(currentValue - 0.5f));
			}
			break;
		default: return error(GL_INVALID_ENUM);
		}
	}
}

void APIENTRY glGetVertexAttribPointerv(GLuint index, GLenum pname, GLvoid** pointer)
{
	TRACE("(GLuint index = %d, GLenum pname = 0x%X, GLvoid** pointer = %p)", index, pname, pointer);

	gl::Context *context = gl::getContext();

	if(context)
	{
		if(index >= gl::MAX_VERTEX_ATTRIBS)
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

void APIENTRY glHint(GLenum target, GLenum mode)
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

	gl::Context *context = gl::getContext();
	switch(target)
	{
	case GL_GENERATE_MIPMAP_HINT:
		if(context) context->setGenerateMipmapHint(mode);
		break;
	case GL_FRAGMENT_SHADER_DERIVATIVE_HINT:
		if(context) context->setFragmentShaderDerivativeHint(mode);
		break;
	default:
		return error(GL_INVALID_ENUM);
	}
}

GLboolean APIENTRY glIsBuffer(GLuint buffer)
{
	TRACE("(GLuint buffer = %d)", buffer);

	gl::Context *context = gl::getContext();

	if(context && buffer)
	{
		gl::Buffer *bufferObject = context->getBuffer(buffer);

		if(bufferObject)
		{
			return GL_TRUE;
		}
	}

	return GL_FALSE;
}

GLboolean APIENTRY glIsEnabled(GLenum cap)
{
	TRACE("(GLenum cap = 0x%X)", cap);

	gl::Context *context = gl::getContext();

	if(context)
	{
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
		default:
			return error(GL_INVALID_ENUM, false);
		}
	}

	return false;
}

GLboolean APIENTRY glIsFenceNV(GLuint fence)
{
	TRACE("(GLuint fence = %d)", fence);

	gl::Context *context = gl::getContext();

	if(context)
	{
		gl::Fence *fenceObject = context->getFence(fence);

		if(fenceObject == NULL)
		{
			return GL_FALSE;
		}

		return fenceObject->isFence();
	}

	return GL_FALSE;
}

GLboolean APIENTRY glIsFramebuffer(GLuint framebuffer)
{
	TRACE("(GLuint framebuffer = %d)", framebuffer);

	gl::Context *context = gl::getContext();

	if(context && framebuffer)
	{
		gl::Framebuffer *framebufferObject = context->getFramebuffer(framebuffer);

		if(framebufferObject)
		{
			return GL_TRUE;
		}
	}

	return GL_FALSE;
}

GLboolean APIENTRY glIsProgram(GLuint program)
{
	TRACE("(GLuint program = %d)", program);

	gl::Context *context = gl::getContext();

	if(context && program)
	{
		gl::Program *programObject = context->getProgram(program);

		if(programObject)
		{
			return GL_TRUE;
		}
	}

	return GL_FALSE;
}

GLboolean APIENTRY glIsQueryEXT(GLuint name)
{
	TRACE("(GLuint name = %d)", name);

	if(name == 0)
	{
		return GL_FALSE;
	}

	gl::Context *context = gl::getContext();

	if(context)
	{
		gl::Query *queryObject = context->getQuery(name, false, GL_NONE);

		if(queryObject)
		{
			return GL_TRUE;
		}
	}

	return GL_FALSE;
}

GLboolean APIENTRY glIsRenderbuffer(GLuint renderbuffer)
{
	TRACE("(GLuint renderbuffer = %d)", renderbuffer);

	gl::Context *context = gl::getContext();

	if(context && renderbuffer)
	{
		gl::Renderbuffer *renderbufferObject = context->getRenderbuffer(renderbuffer);

		if(renderbufferObject)
		{
			return GL_TRUE;
		}
	}

	return GL_FALSE;
}

GLboolean APIENTRY glIsShader(GLuint shader)
{
	TRACE("(GLuint shader = %d)", shader);

	gl::Context *context = gl::getContext();

	if(context && shader)
	{
		gl::Shader *shaderObject = context->getShader(shader);

		if(shaderObject)
		{
			return GL_TRUE;
		}
	}

	return GL_FALSE;
}

GLboolean APIENTRY glIsTexture(GLuint texture)
{
	TRACE("(GLuint texture = %d)", texture);

	gl::Context *context = gl::getContext();

	if(context && texture)
	{
		gl::Texture *textureObject = context->getTexture(texture);

		if(textureObject)
		{
			return GL_TRUE;
		}
	}

	return GL_FALSE;
}

void APIENTRY glLineWidth(GLfloat width)
{
	TRACE("(GLfloat width = %f)", width);

	if(width <= 0.0f)
	{
		return error(GL_INVALID_VALUE);
	}

	gl::Context *context = gl::getContext();

	if(context)
	{
		if(context->getListIndex() != 0)
		{
			UNIMPLEMENTED();
		}

		context->setLineWidth(width);
	}
}

void APIENTRY glLinkProgram(GLuint program)
{
	TRACE("(GLuint program = %d)", program);

	gl::Context *context = gl::getContext();

	if(context)
	{
		gl::Program *programObject = context->getProgram(program);

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

void APIENTRY glPixelStorei(GLenum pname, GLint param)
{
	TRACE("(GLenum pname = 0x%X, GLint param = %d)", pname, param);

	gl::Context *context = gl::getContext();

	if(context)
	{
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
		default:
			return error(GL_INVALID_ENUM);
		}
	}
}

void APIENTRY glPolygonOffset(GLfloat factor, GLfloat units)
{
	TRACE("(GLfloat factor = %f, GLfloat units = %f)", factor, units);

	gl::Context *context = gl::getContext();

	if(context)
	{
		if(context->getListIndex() != 0)
		{
			UNIMPLEMENTED();
		}

		context->setPolygonOffsetParams(factor, units);
	}
}

void APIENTRY glReadnPixelsEXT(GLint x, GLint y, GLsizei width, GLsizei height,
	GLenum format, GLenum type, GLsizei bufSize, GLvoid *data)
{
	TRACE("(GLint x = %d, GLint y = %d, GLsizei width = %d, GLsizei height = %d, "
		"GLenum format = 0x%X, GLenum type = 0x%X, GLsizei bufSize = 0x%d, GLvoid *data = %p)",
		x, y, width, height, format, type, bufSize, data);

	if(width < 0 || height < 0 || bufSize < 0)
	{
		return error(GL_INVALID_VALUE);
	}

	if(!validReadFormatType(format, type))
	{
		return error(GL_INVALID_OPERATION);
	}

	gl::Context *context = gl::getContext();

	if(context)
	{
		if(context->getListIndex() != 0)
		{
			UNIMPLEMENTED();
		}

		context->readPixels(x, y, width, height, format, type, &bufSize, data);
	}
}

void APIENTRY glReadPixels(GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid* pixels)
{
	TRACE("(GLint x = %d, GLint y = %d, GLsizei width = %d, GLsizei height = %d, "
		"GLenum format = 0x%X, GLenum type = 0x%X, GLvoid* pixels = %p)",
		x, y, width, height, format, type, pixels);

	if(width < 0 || height < 0)
	{
		return error(GL_INVALID_VALUE);
	}

	if(!validReadFormatType(format, type))
	{
		return error(GL_INVALID_OPERATION);
	}

	gl::Context *context = gl::getContext();

	if(context)
	{
		context->readPixels(x, y, width, height, format, type, NULL, pixels);
	}
}

void APIENTRY glReleaseShaderCompiler(void)
{
	TRACE("()");

	gl::Shader::releaseCompiler();
}

void APIENTRY glRenderbufferStorageMultisampleANGLE(GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height)
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

	if(!gl::IsColorRenderable(internalformat) && !gl::IsDepthRenderable(internalformat) && !gl::IsStencilRenderable(internalformat))
	{
		return error(GL_INVALID_ENUM);
	}

	if(width < 0 || height < 0 || samples < 0)
	{
		return error(GL_INVALID_VALUE);
	}

	gl::Context *context = gl::getContext();

	if(context)
	{
		if(context->getListIndex() != 0)
		{
			UNIMPLEMENTED();
		}

		if(width > gl::IMPLEMENTATION_MAX_RENDERBUFFER_SIZE ||
			height > gl::IMPLEMENTATION_MAX_RENDERBUFFER_SIZE ||
			samples > gl::IMPLEMENTATION_MAX_SAMPLES)
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
			context->setRenderbufferStorage(new gl::Depthbuffer(width, height, samples));
			break;
		case GL_RGBA4:
		case GL_RGB5_A1:
		case GL_RGB565:
		case GL_RGB8_EXT:
		case GL_RGBA8_EXT:
			context->setRenderbufferStorage(new gl::Colorbuffer(width, height, internalformat, samples));
			break;
		case GL_STENCIL_INDEX8:
			context->setRenderbufferStorage(new gl::Stencilbuffer(width, height, samples));
			break;
		case GL_DEPTH24_STENCIL8_EXT:
			context->setRenderbufferStorage(new gl::DepthStencilbuffer(width, height, samples));
			break;
		default:
			return error(GL_INVALID_ENUM);
		}
	}
}

void APIENTRY glRenderbufferStorage(GLenum target, GLenum internalformat, GLsizei width, GLsizei height)
{
	glRenderbufferStorageMultisampleANGLE(target, 0, internalformat, width, height);
}

void APIENTRY glSampleCoverage(GLclampf value, GLboolean invert)
{
	TRACE("(GLclampf value = %f, GLboolean invert = %d)", value, invert);

	gl::Context* context = gl::getContext();

	if(context)
	{
		if(context->getListIndex() != 0)
		{
			UNIMPLEMENTED();
		}

		context->setSampleCoverageParams(gl::clamp01(value), invert == GL_TRUE);
	}
}

void APIENTRY glSetFenceNV(GLuint fence, GLenum condition)
{
	TRACE("(GLuint fence = %d, GLenum condition = 0x%X)", fence, condition);

	if(condition != GL_ALL_COMPLETED_NV)
	{
		return error(GL_INVALID_ENUM);
	}

	gl::Context *context = gl::getContext();

	if(context)
	{
		if(context->getListIndex() != 0)
		{
			UNIMPLEMENTED();
		}

		gl::Fence *fenceObject = context->getFence(fence);

		if(fenceObject == NULL)
		{
			return error(GL_INVALID_OPERATION);
		}

		fenceObject->setFence(condition);
	}
}

void APIENTRY glScissor(GLint x, GLint y, GLsizei width, GLsizei height)
{
	TRACE("(GLint x = %d, GLint y = %d, GLsizei width = %d, GLsizei height = %d)", x, y, width, height);

	if(width < 0 || height < 0)
	{
		return error(GL_INVALID_VALUE);
	}

	gl::Context* context = gl::getContext();

	if(context)
	{
		if(context->getListIndex() != 0)
		{
			UNIMPLEMENTED();
		}

		context->setScissorParams(x, y, width, height);
	}
}

void APIENTRY glShaderBinary(GLsizei n, const GLuint* shaders, GLenum binaryformat, const GLvoid* binary, GLsizei length)
{
	TRACE("(GLsizei n = %d, const GLuint* shaders = %p, GLenum binaryformat = 0x%X, "
		"const GLvoid* binary = %p, GLsizei length = %d)",
		n, shaders, binaryformat, binary, length);

	// No binary shader formats are supported.
	return error(GL_INVALID_ENUM);
}

void APIENTRY glShaderSource(GLuint shader, GLsizei count, const GLchar *const *string, const GLint *length)
{
	TRACE("(GLuint shader = %d, GLsizei count = %d, const GLchar** string = %p, const GLint* length = %p)",
		shader, count, string, length);

	if(count < 0)
	{
		return error(GL_INVALID_VALUE);
	}

	gl::Context *context = gl::getContext();

	if(context)
	{
		if(context->getListIndex() != 0)
		{
			UNIMPLEMENTED();
		}

		gl::Shader *shaderObject = context->getShader(shader);

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

void APIENTRY glStencilFunc(GLenum func, GLint ref, GLuint mask)
{
	glStencilFuncSeparate(GL_FRONT_AND_BACK, func, ref, mask);
}

void APIENTRY glStencilFuncSeparate(GLenum face, GLenum func, GLint ref, GLuint mask)
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

	gl::Context *context = gl::getContext();

	if(context)
	{
		if(context->getListIndex() != 0)
		{
			UNIMPLEMENTED();
		}

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

void APIENTRY glStencilMask(GLuint mask)
{
	glStencilMaskSeparate(GL_FRONT_AND_BACK, mask);
}

void APIENTRY glStencilMaskSeparate(GLenum face, GLuint mask)
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

	gl::Context *context = gl::getContext();

	if(context)
	{
		if(context->getListIndex() != 0)
		{
			UNIMPLEMENTED();
		}

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

void APIENTRY glStencilOp(GLenum fail, GLenum zfail, GLenum zpass)
{
	glStencilOpSeparate(GL_FRONT_AND_BACK, fail, zfail, zpass);
}

void APIENTRY glStencilOpSeparate(GLenum face, GLenum fail, GLenum zfail, GLenum zpass)
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

	gl::Context *context = gl::getContext();

	if(context)
	{
		if(context->getListIndex() != 0)
		{
			UNIMPLEMENTED();
		}

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

GLboolean APIENTRY glTestFenceNV(GLuint fence)
{
	TRACE("(GLuint fence = %d)", fence);

	gl::Context *context = gl::getContext();

	if(context)
	{
		if(context->getListIndex() != 0)
		{
			UNIMPLEMENTED();
		}

		gl::Fence *fenceObject = context->getFence(fence);

		if(fenceObject == NULL)
		{
			return error(GL_INVALID_OPERATION, GL_TRUE);
		}

		return fenceObject->testFence();
	}

	return GL_TRUE;
}

void APIENTRY glTexImage2D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height,
	GLint border, GLenum format, GLenum type, const GLvoid* pixels)
{
	TRACE("(GLenum target = 0x%X, GLint level = %d, GLint internalformat = %d, GLsizei width = %d, GLsizei height = %d, "
		"GLint border = %d, GLenum format = 0x%X, GLenum type = 0x%X, const GLvoid* pixels =  %p)",
		target, level, internalformat, width, height, border, format, type, pixels);

	if(!validImageSize(level, width, height))
	{
		return error(GL_INVALID_VALUE);
	}

	if(internalformat != format)
	{
		//TRACE("UNIMPLEMENTED!!");
		//return error(GL_INVALID_OPERATION);
	}

	switch(format)
	{
	case GL_ALPHA:
	case GL_LUMINANCE:
	case GL_LUMINANCE_ALPHA:
		switch(type)
		{
		case GL_UNSIGNED_BYTE:
		case GL_FLOAT:
		case GL_HALF_FLOAT:
			break;
		default:
			return error(GL_INVALID_ENUM);
		}
		break;
	case GL_RGB:
		switch(type)
		{
		case GL_UNSIGNED_BYTE:
		case GL_UNSIGNED_SHORT_5_6_5:
		case GL_FLOAT:
		case GL_HALF_FLOAT:
			break;
		default:
			return error(GL_INVALID_ENUM);
		}
		break;
	case GL_RGBA:
		switch(type)
		{
		case GL_UNSIGNED_BYTE:
		case GL_UNSIGNED_SHORT_4_4_4_4:
		case GL_UNSIGNED_SHORT_5_5_5_1:
		case GL_FLOAT:
		case GL_HALF_FLOAT:
			break;
		default:
			return error(GL_INVALID_ENUM);
		}
		break;
	case GL_BGRA_EXT:
		switch(type)
		{
		case GL_UNSIGNED_BYTE:
		case GL_UNSIGNED_SHORT_5_6_5:
		case GL_UNSIGNED_INT_8_8_8_8_REV:
			break;
		default:
			return error(GL_INVALID_ENUM);
		}
		break;
	case GL_COMPRESSED_RGB_S3TC_DXT1_EXT:  // error cases for compressed textures are handled below
	case GL_COMPRESSED_RGBA_S3TC_DXT1_EXT:
	case GL_COMPRESSED_RGBA_S3TC_DXT3_EXT:
	case GL_COMPRESSED_RGBA_S3TC_DXT5_EXT:
		break;
	case GL_DEPTH_COMPONENT:
		switch(type)
		{
		case GL_UNSIGNED_SHORT:
		case GL_UNSIGNED_INT:
			break;
		default:
			return error(GL_INVALID_ENUM);
		}
		break;
	case GL_DEPTH_STENCIL_EXT:
		switch(type)
		{
		case GL_UNSIGNED_INT_24_8_EXT:
			break;
		default:
			return error(GL_INVALID_ENUM);
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
	case GL_TEXTURE_1D:
	case GL_TEXTURE_2D:
	case GL_TEXTURE_RECTANGLE:
		if(width > (gl::IMPLEMENTATION_MAX_TEXTURE_SIZE >> level) ||
			height > (gl::IMPLEMENTATION_MAX_TEXTURE_SIZE >> level))
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

		if(width > (gl::IMPLEMENTATION_MAX_CUBE_MAP_TEXTURE_SIZE >> level) ||
			height > (gl::IMPLEMENTATION_MAX_CUBE_MAP_TEXTURE_SIZE >> level))
		{
			return error(GL_INVALID_VALUE);
		}
		break;
	case GL_PROXY_TEXTURE_2D:
		pixels = 0;

		if(width > (gl::IMPLEMENTATION_MAX_TEXTURE_SIZE >> level) ||
			height > (gl::IMPLEMENTATION_MAX_TEXTURE_SIZE >> level))
		{
			//UNIMPLEMENTED();
			width = 0;
			height = 0;
			internalformat = GL_NONE;
			format = GL_NONE;
			type = GL_NONE;

			//return;// error(GL_INVALID_VALUE);
		}
		break;
	default:
		return error(GL_INVALID_ENUM);
	}

	if(format == GL_COMPRESSED_RGB_S3TC_DXT1_EXT ||
		format == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT ||
		format == GL_COMPRESSED_RGBA_S3TC_DXT3_EXT ||
		format == GL_COMPRESSED_RGBA_S3TC_DXT5_EXT)
	{
		if(S3TC_SUPPORT)
		{
			return error(GL_INVALID_OPERATION);
		}
		else
		{
			return error(GL_INVALID_ENUM);
		}
	}

	gl::Context *context = gl::getContext();

	if(context)
	{
		if(context->getListIndex() != 0)
		{
			UNIMPLEMENTED();
		}

		if(target == GL_TEXTURE_1D)
		{
			gl::Texture1D *texture = context->getTexture1D();

			if(!texture)
			{
				return error(GL_INVALID_OPERATION);
			}

			texture->setImage(level, width, height, format, type, context->getUnpackAlignment(), pixels);
		}
		else if(target == GL_TEXTURE_2D || target == GL_PROXY_TEXTURE_2D || target == GL_TEXTURE_RECTANGLE)
		{
			gl::Texture2D *texture = context->getTexture2D(target);

			if(!texture)
			{
				return error(GL_INVALID_OPERATION);
			}

			texture->setImage(level, width, height, format, type, context->getUnpackAlignment(), pixels);
		}
		else
		{
			gl::TextureCubeMap *texture = context->getTextureCubeMap();

			if(!texture)
			{
				return error(GL_INVALID_OPERATION);
			}

			texture->setImage(target, level, width, height, format, type, context->getUnpackAlignment(), pixels);
		}
	}
}

void APIENTRY glTexParameterf(GLenum target, GLenum pname, GLfloat param)
{
	TRACE("(GLenum target = 0x%X, GLenum pname = 0x%X, GLfloat param = %f)", target, pname, param);

	gl::Context *context = gl::getContext();

	if(context)
	{
		if(context->getListIndex() != 0)
		{
			UNIMPLEMENTED();
		}

		gl::Texture *texture;

		switch(target)
		{
		case GL_TEXTURE_2D:
			texture = context->getTexture2D(target);
			break;
		case GL_TEXTURE_CUBE_MAP:
			texture = context->getTextureCubeMap();
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
		case GL_TEXTURE_MIN_LOD:
			//TRACE("() UNIMPLEMENTED!!");   // FIXME
			//UNIMPLEMENTED();
			break;
		case GL_TEXTURE_MAX_LOD:
			//TRACE("() UNIMPLEMENTED!!");   // FIXME
			//UNIMPLEMENTED();
			break;
		case GL_TEXTURE_LOD_BIAS:
			if(param != 0.0f)
			{
				UNIMPLEMENTED();
			}
			break;
		default:
			return error(GL_INVALID_ENUM);
		}
	}
}

void APIENTRY glTexParameterfv(GLenum target, GLenum pname, const GLfloat* params)
{
	glTexParameterf(target, pname, *params);
}

void APIENTRY glTexParameteri(GLenum target, GLenum pname, GLint param)
{
	TRACE("(GLenum target = 0x%X, GLenum pname = 0x%X, GLint param = %d)", target, pname, param);

	gl::Context *context = gl::getContext();

	if(context)
	{
		if(context->getListIndex() != 0)
		{
			UNIMPLEMENTED();
		}

		gl::Texture *texture;

		switch(target)
		{
		case GL_TEXTURE_1D:
			texture = context->getTexture1D();
			break;
		case GL_TEXTURE_2D:
			texture = context->getTexture2D(target);
			break;
		case GL_TEXTURE_CUBE_MAP:
			texture = context->getTextureCubeMap();
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
		case GL_TEXTURE_MAX_LEVEL:
			if(!texture->setMaxLevel(param))
			{
				return error(GL_INVALID_ENUM);
			}
			break;
		default:
			return error(GL_INVALID_ENUM);
		}
	}
}

void APIENTRY glTexParameteriv(GLenum target, GLenum pname, const GLint* params)
{
	glTexParameteri(target, pname, *params);
}

void APIENTRY glTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height,
	GLenum format, GLenum type, const GLvoid* pixels)
{
	TRACE("(GLenum target = 0x%X, GLint level = %d, GLint xoffset = %d, GLint yoffset = %d, "
		"GLsizei width = %d, GLsizei height = %d, GLenum format = 0x%X, GLenum type = 0x%X, "
		"const GLvoid* pixels = %p)",
		target, level, xoffset, yoffset, width, height, format, type, pixels);

	if(!gl::IsTextureTarget(target))
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

	if(!gl::CheckTextureFormatType(format, type))
	{
		return error(GL_INVALID_ENUM);
	}

	if(width == 0 || height == 0 || pixels == NULL)
	{
		return;
	}

	gl::Context *context = gl::getContext();

	if(context)
	{
		if(context->getListIndex() != 0)
		{
			UNIMPLEMENTED();
		}

		if(level > gl::IMPLEMENTATION_MAX_TEXTURE_LEVELS)
		{
			return error(GL_INVALID_VALUE);
		}
		if(target == GL_TEXTURE_1D)
		{
			gl::Texture1D *texture = context->getTexture1D();

			if(validateSubImageParams(false, width, height, xoffset, yoffset, target, level, format, texture))
			{
				texture->subImage(level, xoffset, yoffset, width, height, format, type, context->getUnpackAlignment(), pixels);
			}
		}
		else if(target == GL_TEXTURE_2D || target == GL_TEXTURE_RECTANGLE)
		{
			gl::Texture2D *texture = context->getTexture2D(target);

			if(validateSubImageParams(false, width, height, xoffset, yoffset, target, level, format, texture))
			{
				texture->subImage(level, xoffset, yoffset, width, height, format, type, context->getUnpackAlignment(), pixels);
			}
		}
		else if(gl::IsCubemapTextureTarget(target))
		{
			gl::TextureCubeMap *texture = context->getTextureCubeMap();

			if(validateSubImageParams(false, width, height, xoffset, yoffset, target, level, format, texture))
			{
				texture->subImage(target, level, xoffset, yoffset, width, height, format, type, context->getUnpackAlignment(), pixels);
			}
		}
		else
		{
			UNREACHABLE();
		}
	}
}

void APIENTRY glUniform1f(GLint location, GLfloat x)
{
	glUniform1fv(location, 1, &x);
}

void APIENTRY glUniform1fv(GLint location, GLsizei count, const GLfloat* v)
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

	gl::Context *context = gl::getContext();

	if(context)
	{
		if(context->getListIndex() != 0)
		{
			UNIMPLEMENTED();
		}

		gl::Program *program = context->getCurrentProgram();

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

void APIENTRY glUniform1i(GLint location, GLint x)
{
	glUniform1iv(location, 1, &x);
}

void APIENTRY glUniform1iv(GLint location, GLsizei count, const GLint* v)
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

	gl::Context *context = gl::getContext();

	if(context)
	{
		if(context->getListIndex() != 0)
		{
			UNIMPLEMENTED();
		}

		gl::Program *program = context->getCurrentProgram();

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

void APIENTRY glUniform2f(GLint location, GLfloat x, GLfloat y)
{
	GLfloat xy[2] = { x, y };

	glUniform2fv(location, 1, (GLfloat*)&xy);
}

void APIENTRY glUniform2fv(GLint location, GLsizei count, const GLfloat* v)
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

	gl::Context *context = gl::getContext();

	if(context)
	{
		if(context->getListIndex() != 0)
		{
			UNIMPLEMENTED();
		}

		gl::Program *program = context->getCurrentProgram();

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

void APIENTRY glUniform2i(GLint location, GLint x, GLint y)
{
	GLint xy[4] = { x, y };

	glUniform2iv(location, 1, (GLint*)&xy);
}

void APIENTRY glUniform2iv(GLint location, GLsizei count, const GLint* v)
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

	gl::Context *context = gl::getContext();

	if(context)
	{
		if(context->getListIndex() != 0)
		{
			UNIMPLEMENTED();
		}

		gl::Program *program = context->getCurrentProgram();

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

void APIENTRY glUniform3f(GLint location, GLfloat x, GLfloat y, GLfloat z)
{
	GLfloat xyz[3] = { x, y, z };

	glUniform3fv(location, 1, (GLfloat*)&xyz);
}

void APIENTRY glUniform3fv(GLint location, GLsizei count, const GLfloat* v)
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

	gl::Context *context = gl::getContext();

	if(context)
	{
		if(context->getListIndex() != 0)
		{
			UNIMPLEMENTED();
		}

		gl::Program *program = context->getCurrentProgram();

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

void APIENTRY glUniform3i(GLint location, GLint x, GLint y, GLint z)
{
	GLint xyz[3] = { x, y, z };

	glUniform3iv(location, 1, (GLint*)&xyz);
}

void APIENTRY glUniform3iv(GLint location, GLsizei count, const GLint* v)
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

	gl::Context *context = gl::getContext();

	if(context)
	{
		if(context->getListIndex() != 0)
		{
			UNIMPLEMENTED();
		}

		gl::Program *program = context->getCurrentProgram();

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

void APIENTRY glUniform4f(GLint location, GLfloat x, GLfloat y, GLfloat z, GLfloat w)
{
	GLfloat xyzw[4] = { x, y, z, w };

	glUniform4fv(location, 1, (GLfloat*)&xyzw);
}

void APIENTRY glUniform4fv(GLint location, GLsizei count, const GLfloat* v)
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

	gl::Context *context = gl::getContext();

	if(context)
	{
		if(context->getListIndex() != 0)
		{
			UNIMPLEMENTED();
		}

		gl::Program *program = context->getCurrentProgram();

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

void APIENTRY glUniform4i(GLint location, GLint x, GLint y, GLint z, GLint w)
{
	GLint xyzw[4] = { x, y, z, w };

	glUniform4iv(location, 1, (GLint*)&xyzw);
}

void APIENTRY glUniform4iv(GLint location, GLsizei count, const GLint* v)
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

	gl::Context *context = gl::getContext();

	if(context)
	{
		if(context->getListIndex() != 0)
		{
			UNIMPLEMENTED();
		}

		gl::Program *program = context->getCurrentProgram();

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

void APIENTRY glUniformMatrix2fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value)
{
	TRACE("(GLint location = %d, GLsizei count = %d, GLboolean transpose = %d, const GLfloat* value = %p)",
		location, count, transpose, value);

	if(count < 0 || transpose != GL_FALSE)
	{
		return error(GL_INVALID_VALUE);
	}

	if(location == -1)
	{
		return;
	}

	gl::Context *context = gl::getContext();

	if(context)
	{
		if(context->getListIndex() != 0)
		{
			UNIMPLEMENTED();
		}

		gl::Program *program = context->getCurrentProgram();

		if(!program)
		{
			return error(GL_INVALID_OPERATION);
		}

		if(!program->setUniformMatrix2fv(location, count, value))
		{
			return error(GL_INVALID_OPERATION);
		}
	}
}

void APIENTRY glUniformMatrix3fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value)
{
	TRACE("(GLint location = %d, GLsizei count = %d, GLboolean transpose = %d, const GLfloat* value = %p)",
		location, count, transpose, value);

	if(count < 0 || transpose != GL_FALSE)
	{
		return error(GL_INVALID_VALUE);
	}

	if(location == -1)
	{
		return;
	}

	gl::Context *context = gl::getContext();

	if(context)
	{
		if(context->getListIndex() != 0)
		{
			UNIMPLEMENTED();
		}

		gl::Program *program = context->getCurrentProgram();

		if(!program)
		{
			return error(GL_INVALID_OPERATION);
		}

		if(!program->setUniformMatrix3fv(location, count, value))
		{
			return error(GL_INVALID_OPERATION);
		}
	}
}

void APIENTRY glUniformMatrix4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value)
{
	TRACE("(GLint location = %d, GLsizei count = %d, GLboolean transpose = %d, const GLfloat* value = %p)",
		location, count, transpose, value);

	if(count < 0 || transpose != GL_FALSE)
	{
		return error(GL_INVALID_VALUE);
	}

	if(location == -1)
	{
		return;
	}

	gl::Context *context = gl::getContext();

	if(context)
	{
		if(context->getListIndex() != 0)
		{
			UNIMPLEMENTED();
		}

		gl::Program *program = context->getCurrentProgram();

		if(!program)
		{
			return error(GL_INVALID_OPERATION);
		}

		if(!program->setUniformMatrix4fv(location, count, value))
		{
			return error(GL_INVALID_OPERATION);
		}
	}
}

void APIENTRY glUseProgram(GLuint program)
{
	TRACE("(GLuint program = %d)", program);

	gl::Context *context = gl::getContext();

	if(context)
	{
		if(context->getListIndex() != 0)
		{
			UNIMPLEMENTED();
		}

		gl::Program *programObject = context->getProgram(program);

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

void APIENTRY glValidateProgram(GLuint program)
{
	TRACE("(GLuint program = %d)", program);

	gl::Context *context = gl::getContext();

	if(context)
	{
		gl::Program *programObject = context->getProgram(program);

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

void APIENTRY glVertexAttrib1f(GLuint index, GLfloat x)
{
	TRACE("(GLuint index = %d, GLfloat x = %f)", index, x);

	if(index >= gl::MAX_VERTEX_ATTRIBS)
	{
		return error(GL_INVALID_VALUE);
	}

	gl::Context *context = gl::getContext();

	if(context)
	{
		if(context->getListIndex() != 0)
		{
			UNIMPLEMENTED();
		}

		//GLfloat vals[4] = { x, 0, 0, 1 };
		context->setVertexAttrib(index, x, 0, 0, 1);
	}
}

void APIENTRY glVertexAttrib1fv(GLuint index, const GLfloat* values)
{
	TRACE("(GLuint index = %d, const GLfloat* values = %p)", index, values);

	if(index >= gl::MAX_VERTEX_ATTRIBS)
	{
		return error(GL_INVALID_VALUE);
	}

	gl::Context *context = gl::getContext();

	if(context)
	{
		if(context->getListIndex() != 0)
		{
			UNIMPLEMENTED();
		}

		//GLfloat vals[4] = { values[0], 0, 0, 1 };
		context->setVertexAttrib(index, values[0], 0, 0, 1);
	}
}

void APIENTRY glVertexAttrib2f(GLuint index, GLfloat x, GLfloat y)
{
	TRACE("(GLuint index = %d, GLfloat x = %f, GLfloat y = %f)", index, x, y);

	if(index >= gl::MAX_VERTEX_ATTRIBS)
	{
		return error(GL_INVALID_VALUE);
	}

	gl::Context *context = gl::getContext();

	if(context)
	{
		if(context->getListIndex() != 0)
		{
			UNIMPLEMENTED();
		}

		//GLfloat vals[4] = { x, y, 0, 1 };
		context->setVertexAttrib(index, x, y, 0, 1);
	}
}

void APIENTRY glVertexAttrib2fv(GLuint index, const GLfloat* values)
{
	TRACE("(GLuint index = %d, const GLfloat* values = %p)", index, values);

	if(index >= gl::MAX_VERTEX_ATTRIBS)
	{
		return error(GL_INVALID_VALUE);
	}

	gl::Context *context = gl::getContext();

	if(context)
	{
		if(context->getListIndex() != 0)
		{
			UNIMPLEMENTED();
		}

		//GLfloat vals[4] = {  };
		context->setVertexAttrib(index, values[0], values[1], 0, 1);
	}
}

void APIENTRY glVertexAttrib3f(GLuint index, GLfloat x, GLfloat y, GLfloat z)
{
	TRACE("(GLuint index = %d, GLfloat x = %f, GLfloat y = %f, GLfloat z = %f)", index, x, y, z);

	if(index >= gl::MAX_VERTEX_ATTRIBS)
	{
		return error(GL_INVALID_VALUE);
	}

	gl::Context *context = gl::getContext();

	if(context)
	{
		if(context->getListIndex() != 0)
		{
			UNIMPLEMENTED();
		}

		//GLfloat vals[4] = { x, y, z, 1 };
		context->setVertexAttrib(index, x, y, z, 1);
	}
}

void APIENTRY glVertexAttrib3fv(GLuint index, const GLfloat* values)
{
	TRACE("(GLuint index = %d, const GLfloat* values = %p)", index, values);

	if(index >= gl::MAX_VERTEX_ATTRIBS)
	{
		return error(GL_INVALID_VALUE);
	}

	gl::Context *context = gl::getContext();

	if(context)
	{
		if(context->getListIndex() != 0)
		{
			UNIMPLEMENTED();
		}

		//GLfloat vals[4] = { values[0], values[1], values[2], 1 };
		context->setVertexAttrib(index, values[0], values[1], values[2], 1);
	}
}

void APIENTRY glVertexAttrib4f(GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w)
{
	TRACE("(GLuint index = %d, GLfloat x = %f, GLfloat y = %f, GLfloat z = %f, GLfloat w = %f)", index, x, y, z, w);

	if(index >= gl::MAX_VERTEX_ATTRIBS)
	{
		return error(GL_INVALID_VALUE);
	}

	gl::Context *context = gl::getContext();

	if(context)
	{
		if(context->getListIndex() != 0)
		{
			UNIMPLEMENTED();
		}

		//GLfloat vals[4] = { x, y, z, w };
		context->setVertexAttrib(index, x, y, z, w);
	}
}

void APIENTRY glVertexAttrib4fv(GLuint index, const GLfloat* values)
{
	TRACE("(GLuint index = %d, const GLfloat* values = %p)", index, values);

	if(index >= gl::MAX_VERTEX_ATTRIBS)
	{
		return error(GL_INVALID_VALUE);
	}

	gl::Context *context = gl::getContext();

	if(context)
	{
		if(context->getListIndex() != 0)
		{
			UNIMPLEMENTED();
		}

		context->setVertexAttrib(index, values[0], values[1], values[2], values[3]);
	}
}

void APIENTRY glVertexAttribPointer(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid* ptr)
{
	TRACE("(GLuint index = %d, GLint size = %d, GLenum type = 0x%X, "
		"GLboolean normalized = %d, GLsizei stride = %d, const GLvoid* ptr = %p)",
		index, size, type, normalized, stride, ptr);

	if(index >= gl::MAX_VERTEX_ATTRIBS)
	{
		return error(GL_INVALID_VALUE);
	}

	if(size < 1 || size > 4)
	{
		return error(GL_INVALID_VALUE);
	}

	switch(type)
	{
	case GL_BYTE:
	case GL_UNSIGNED_BYTE:
	case GL_SHORT:
	case GL_UNSIGNED_SHORT:
	case GL_FIXED:
	case GL_FLOAT:
		break;
	default:
		return error(GL_INVALID_ENUM);
	}

	if(stride < 0)
	{
		return error(GL_INVALID_VALUE);
	}

	gl::Context *context = gl::getContext();

	if(context)
	{
		context->setVertexAttribState(index, context->getArrayBuffer(), size, type, (normalized == GL_TRUE), stride, ptr);
	}
}

void APIENTRY glViewport(GLint x, GLint y, GLsizei width, GLsizei height)
{
	TRACE("(GLint x = %d, GLint y = %d, GLsizei width = %d, GLsizei height = %d)", x, y, width, height);

	if(width < 0 || height < 0)
	{
		return error(GL_INVALID_VALUE);
	}

	gl::Context *context = gl::getContext();

	if(context)
	{
		if(context->getListIndex() != 0)
		{
			UNIMPLEMENTED();
		}

		context->setViewportParams(x, y, width, height);
	}
}

void APIENTRY glBlitFramebufferANGLE(GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1,
	GLbitfield mask, GLenum filter)
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

	if(srcX1 - srcX0 != dstX1 - dstX0 || srcY1 - srcY0 != dstY1 - dstY0)
	{
		ERR("Scaling and flipping in BlitFramebufferANGLE not supported by this implementation");
		return error(GL_INVALID_OPERATION);
	}

	gl::Context *context = gl::getContext();

	if(context)
	{
		if(context->getListIndex() != 0)
		{
			UNIMPLEMENTED();
		}

		if(context->getReadFramebufferName() == context->getDrawFramebufferName())
		{
			ERR("Blits with the same source and destination framebuffer are not supported by this implementation.");
			return error(GL_INVALID_OPERATION);
		}

		context->blitFramebuffer(srcX0, srcY0, srcX1, srcY1, dstX0, dstY0, dstX1, dstY1, mask);
	}
}

void APIENTRY glTexImage3DOES(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth,
	GLint border, GLenum format, GLenum type, const GLvoid* pixels)
{
	TRACE("(GLenum target = 0x%X, GLint level = %d, GLenum internalformat = 0x%X, "
		"GLsizei width = %d, GLsizei height = %d, GLsizei depth = %d, GLint border = %d, "
		"GLenum format = 0x%X, GLenum type = 0x%x, const GLvoid* pixels = %p)",
		target, level, internalformat, width, height, depth, border, format, type, pixels);

	UNIMPLEMENTED();   // FIXME
}

void WINAPI GlmfBeginGlsBlock()
{
	UNIMPLEMENTED();
}

void WINAPI GlmfCloseMetaFile()
{
	UNIMPLEMENTED();
}

void WINAPI GlmfEndGlsBlock()
{
	UNIMPLEMENTED();
}

void WINAPI GlmfEndPlayback()
{
	UNIMPLEMENTED();
}

void WINAPI GlmfInitPlayback()
{
	UNIMPLEMENTED();
}

void WINAPI GlmfPlayGlsRecord()
{
	UNIMPLEMENTED();
}

void APIENTRY glAccum(GLenum op, GLfloat value)
{
	UNIMPLEMENTED();
}

void APIENTRY glAlphaFunc(GLenum func, GLclampf ref)
{
	TRACE("(GLenum func = 0x%X, GLclampf ref = %f)", func, ref);

	gl::Context *context = gl::getContext();

	if(context)
	{
		if(context->getListIndex() != 0)
		{
			UNIMPLEMENTED();
		}

		context->alphaFunc(func, ref);
	}
}

GLboolean APIENTRY glAreTexturesResident(GLsizei n, const GLuint *textures, GLboolean *residences)
{
	UNIMPLEMENTED();
	return GL_FALSE;
}

void APIENTRY glArrayElement(GLint i)
{
	UNIMPLEMENTED();
}

void APIENTRY glBegin(GLenum mode)
{
	TRACE("(GLenum mode = 0x%X)", mode);

	switch(mode)
	{
	case GL_POINTS:
	case GL_LINES:
	case GL_LINE_STRIP:
	case GL_LINE_LOOP:
	case GL_TRIANGLES:
	case GL_TRIANGLE_STRIP:
	case GL_TRIANGLE_FAN:
	case GL_QUADS:
	case GL_QUAD_STRIP:
	case GL_POLYGON:
		break;
	default:
		return error(GL_INVALID_ENUM);
	}

	gl::Context *context = gl::getContext();

	if(context)
	{
		if(context->getListIndex() != 0)
		{
			UNIMPLEMENTED();
		}

		context->begin(mode);
	}
}

void APIENTRY glBitmap(GLsizei width, GLsizei height, GLfloat xorig, GLfloat yorig, GLfloat xmove, GLfloat ymove, const GLubyte *bitmap)
{
	UNIMPLEMENTED();
}

void APIENTRY glCallList(GLuint list)
{
	TRACE("(GLuint list = %d)", list);

	if(list == 0)
	{
		return error(GL_INVALID_VALUE);
	}

	gl::Context *context = gl::getContext();

	if(context)
	{
		if(context->getListIndex() != 0)
		{
			UNIMPLEMENTED();
		}

		context->callList(list);
	}
}

void APIENTRY glCallLists(GLsizei n, GLenum type, const GLvoid *lists)
{
	TRACE("(GLsizei n = %d, GLenum type = 0x%X, const GLvoid *lists)", n, type);

	gl::Context *context = gl::getContext();

	if(context)
	{
		if(context->getListIndex() != 0)
		{
			UNIMPLEMENTED();
		}

		for(int i = 0; i < n; i++)
		{
			switch(type)
			{
			case GL_UNSIGNED_INT: context->callList(((unsigned int*)lists)[i]); break;
			default:
				UNIMPLEMENTED();
				UNREACHABLE();
			}
		}
	}
}

void APIENTRY glClearAccum(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha)
{
	UNIMPLEMENTED();
}

void APIENTRY glClearDepth(GLclampd depth)
{
	TRACE("(GLclampd depth = %d)", depth);

	glClearDepthf((float)depth);   // FIXME
}

void APIENTRY glClearIndex(GLfloat c)
{
	UNIMPLEMENTED();
}

void APIENTRY glClipPlane(GLenum plane, const GLdouble *equation)
{
	UNIMPLEMENTED();
}

void APIENTRY glColor3b(GLbyte red, GLbyte green, GLbyte blue)
{
	UNIMPLEMENTED();
}

void APIENTRY glColor3bv(const GLbyte *v)
{
	UNIMPLEMENTED();
}

void APIENTRY glColor3d(GLdouble red, GLdouble green, GLdouble blue)
{
	UNIMPLEMENTED();
}

void APIENTRY glColor3dv(const GLdouble *v)
{
	UNIMPLEMENTED();
}

void APIENTRY glColor3f(GLfloat red, GLfloat green, GLfloat blue)
{
	TRACE("(GLfloat red = %f, GLfloat green = %f, GLfloat blue = %f)", red, green, blue);

	gl::Context *context = gl::getContext();

	if(context)
	{
		if(context->getListIndex() != 0)
		{
			UNIMPLEMENTED();
		}

		//context->color(red, green, blue, 1.0f);
		//GLfloat vals[4] = {};
		context->setVertexAttrib(sw::Color0, red, green, blue, 1);
	}
}

void APIENTRY glColor3fv(const GLfloat *v)
{
	UNIMPLEMENTED();
}

void APIENTRY glColor3i(GLint red, GLint green, GLint blue)
{
	UNIMPLEMENTED();
}

void APIENTRY glColor3iv(const GLint *v)
{
	UNIMPLEMENTED();
}

void APIENTRY glColor3s(GLshort red, GLshort green, GLshort blue)
{
	UNIMPLEMENTED();
}

void APIENTRY glColor3sv(const GLshort *v)
{
	UNIMPLEMENTED();
}

void APIENTRY glColor3ub(GLubyte red, GLubyte green, GLubyte blue)
{
	UNIMPLEMENTED();
}

void APIENTRY glColor3ubv(const GLubyte *v)
{
	UNIMPLEMENTED();
}

void APIENTRY glColor3ui(GLuint red, GLuint green, GLuint blue)
{
	UNIMPLEMENTED();
}

void APIENTRY glColor3uiv(const GLuint *v)
{
	UNIMPLEMENTED();
}

void APIENTRY glColor3us(GLushort red, GLushort green, GLushort blue)
{
	UNIMPLEMENTED();
}

void APIENTRY glColor3usv(const GLushort *v)
{
	UNIMPLEMENTED();
}

void APIENTRY glColor4b(GLbyte red, GLbyte green, GLbyte blue, GLbyte alpha)
{
	UNIMPLEMENTED();
}

void APIENTRY glColor4bv(const GLbyte *v)
{
	UNIMPLEMENTED();
}

void APIENTRY glColor4d(GLdouble red, GLdouble green, GLdouble blue, GLdouble alpha)
{
	UNIMPLEMENTED();
}

void APIENTRY glColor4dv(const GLdouble *v)
{
	UNIMPLEMENTED();
}

void APIENTRY glColor4f(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha)
{
	TRACE("(GLfloat red = %f, GLfloat green = %f, GLfloat blue = %f, GLfloat alpha = %f)", red, green, blue, alpha);

	gl::Context *context = gl::getContext();

	if(context)
	{
		if(context->getListIndex() != 0)
		{
			UNIMPLEMENTED();
		}

		//context->color(red, green, blue, alpha);
		//GLfloat vals[4] = {red, green, blue, alpha};
		context->setVertexAttrib(sw::Color0, red, green, blue, alpha);
	}
}

void APIENTRY glColor4fv(const GLfloat *v)
{
	UNIMPLEMENTED();
}

void APIENTRY glColor4i(GLint red, GLint green, GLint blue, GLint alpha)
{
	UNIMPLEMENTED();
}

void APIENTRY glColor4iv(const GLint *v)
{
	UNIMPLEMENTED();
}

void APIENTRY glColor4s(GLshort red, GLshort green, GLshort blue, GLshort alpha)
{
	UNIMPLEMENTED();
}

void APIENTRY glColor4sv(const GLshort *v)
{
	UNIMPLEMENTED();
}

void APIENTRY glColor4ub(GLubyte red, GLubyte green, GLubyte blue, GLubyte alpha)
{
	UNIMPLEMENTED();
}

void APIENTRY glColor4ubv(const GLubyte *v)
{
	UNIMPLEMENTED();
}

void APIENTRY glColor4ui(GLuint red, GLuint green, GLuint blue, GLuint alpha)
{
	UNIMPLEMENTED();
}

void APIENTRY glColor4uiv(const GLuint *v)
{
	UNIMPLEMENTED();
}

void APIENTRY glColor4us(GLushort red, GLushort green, GLushort blue, GLushort alpha)
{
	UNIMPLEMENTED();
}

void APIENTRY glColor4usv(const GLushort *v)
{
	UNIMPLEMENTED();
}

void APIENTRY glColorMaterial(GLenum face, GLenum mode)
{
	TRACE("(GLenum face = 0x%X, GLenum mode = 0x%X)", face, mode);

	// FIXME: Validate enums

	gl::Context *context = gl::getContext();

	if(context)
	{
		if(context->getListIndex() != 0)
		{
			UNIMPLEMENTED();
		}

		switch(face)
		{
		case GL_FRONT:
			context->setColorMaterialMode(mode);   // FIXME: Front only
			break;
		case GL_FRONT_AND_BACK:
			context->setColorMaterialMode(mode);
			break;
		default:
			UNIMPLEMENTED();
			return error(GL_INVALID_ENUM);
		}
	}
}

void APIENTRY glColorPointer(GLint size, GLenum type, GLsizei stride, const GLvoid *pointer)
{
	TRACE("(*)");

	glVertexAttribPointer(sw::Color0, size, type, true, stride, pointer);
}

void APIENTRY glCopyPixels(GLint x, GLint y, GLsizei width, GLsizei height, GLenum type)
{
	UNIMPLEMENTED();
}

void APIENTRY glCopyTexImage1D(GLenum target, GLint level, GLenum internalFormat, GLint x, GLint y, GLsizei width, GLint border)
{
	UNIMPLEMENTED();
}

void APIENTRY glCopyTexSubImage1D(GLenum target, GLint level, GLint xoffset, GLint x, GLint y, GLsizei width)
{
	UNIMPLEMENTED();
}

void APIENTRY glDebugEntry()
{
	UNIMPLEMENTED();
}

void APIENTRY glDeleteLists(GLuint list, GLsizei range)
{
	TRACE("(GLuint list = %d, GLsizei range = %d)", list, range);

	if(range < 0)
	{
		return error(GL_INVALID_VALUE);
	}

	gl::Context *context = gl::getContext();

	if(context)
	{
		for(GLuint i = list; i < list + range; i++)
		{
			context->deleteList(i);
		}
	}
}

void APIENTRY glDepthRange(GLclampd zNear, GLclampd zFar)
{
	UNIMPLEMENTED();
}

void APIENTRY glDisableClientState(GLenum array)
{
	TRACE("(GLenum array = 0x%X)", array);

	gl::Context *context = gl::getContext();

	if(context)
	{
		GLenum texture = context->getClientActiveTexture();

		switch(array)
		{
		case GL_VERTEX_ARRAY:        context->setEnableVertexAttribArray(sw::Position, false);                            break;
		case GL_COLOR_ARRAY:         context->setEnableVertexAttribArray(sw::Color0, false);                              break;
		case GL_TEXTURE_COORD_ARRAY: context->setEnableVertexAttribArray(sw::TexCoord0 + (texture - GL_TEXTURE0), false); break;
		case GL_NORMAL_ARRAY:        context->setEnableVertexAttribArray(sw::Normal, false);                              break;
		default:                     UNIMPLEMENTED();
		}
	}
}

void APIENTRY glDrawBuffer(GLenum mode)
{
	UNIMPLEMENTED();
}

void APIENTRY glDrawPixels(GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *pixels)
{
	UNIMPLEMENTED();
}

void APIENTRY glEdgeFlag(GLboolean flag)
{
	UNIMPLEMENTED();
}

void APIENTRY glEdgeFlagPointer(GLsizei stride, const GLvoid *pointer)
{
	UNIMPLEMENTED();
}

void APIENTRY glEdgeFlagv(const GLboolean *flag)
{
	UNIMPLEMENTED();
}

void APIENTRY glEnableClientState(GLenum array)
{
	TRACE("(GLenum array = 0x%X)", array);

	gl::Context *context = gl::getContext();

	if(context)
	{
		GLenum texture = context->getClientActiveTexture();

		switch(array)
		{
		case GL_VERTEX_ARRAY:        context->setEnableVertexAttribArray(sw::Position, true);                            break;
		case GL_COLOR_ARRAY:         context->setEnableVertexAttribArray(sw::Color0, true);                              break;
		case GL_TEXTURE_COORD_ARRAY: context->setEnableVertexAttribArray(sw::TexCoord0 + (texture - GL_TEXTURE0), true); break;
		case GL_NORMAL_ARRAY:        context->setEnableVertexAttribArray(sw::Normal, true);                              break;
		default:                     UNIMPLEMENTED();
		}
	}
}

void APIENTRY glEnd()
{
	TRACE("()");

	gl::Context *context = gl::getContext();

	if(context)
	{
		if(context->getListIndex() != 0)
		{
			UNIMPLEMENTED();
		}

		context->end();
	}
}

void APIENTRY glEndList()
{
	TRACE("()");

	gl::Context *context = gl::getContext();

	if(context)
	{
		context->endList();
	}
}

void APIENTRY glEvalCoord1d(GLdouble u)
{
	UNIMPLEMENTED();
}

void APIENTRY glEvalCoord1dv(const GLdouble *u)
{
	UNIMPLEMENTED();
}

void APIENTRY glEvalCoord1f(GLfloat u)
{
	UNIMPLEMENTED();
}

void APIENTRY glEvalCoord1fv(const GLfloat *u)
{
	UNIMPLEMENTED();
}

void APIENTRY glEvalCoord2d(GLdouble u, GLdouble v)
{
	UNIMPLEMENTED();
}

void APIENTRY glEvalCoord2dv(const GLdouble *u)
{
	UNIMPLEMENTED();
}

void APIENTRY glEvalCoord2f(GLfloat u, GLfloat v)
{
	UNIMPLEMENTED();
}

void APIENTRY glEvalCoord2fv(const GLfloat *u)
{
	UNIMPLEMENTED();
}

void APIENTRY glEvalMesh1(GLenum mode, GLint i1, GLint i2)
{
	UNIMPLEMENTED();
}

void APIENTRY glEvalMesh2(GLenum mode, GLint i1, GLint i2, GLint j1, GLint j2)
{
	UNIMPLEMENTED();
}

void APIENTRY glEvalPoint1(GLint i)
{
	UNIMPLEMENTED();
}

void APIENTRY glEvalPoint2(GLint i, GLint j)
{
	UNIMPLEMENTED();
}

void APIENTRY glFeedbackBuffer(GLsizei size, GLenum type, GLfloat *buffer)
{
	UNIMPLEMENTED();
}

void APIENTRY glFogf(GLenum pname, GLfloat param)
{
	TRACE("(GLenum pname = 0x%X, GLfloat param = %f)", pname, param);

	gl::Context *context = gl::getContext();

	if(context)
	{
		if(context->getListIndex() != 0)
		{
			UNIMPLEMENTED();
		}

		gl::Device *device = gl::getDevice();   // FIXME

		switch(pname)
		{
		case GL_FOG_START: device->setFogStart(param); break;
		case GL_FOG_END:   device->setFogEnd(param);   break;
		default:
			UNIMPLEMENTED();
			return error(GL_INVALID_ENUM);
		}
	}
}

void APIENTRY glFogfv(GLenum pname, const GLfloat *params)
{
	TRACE("(GLenum pname = 0x%X, const GLfloat *params)", pname);

	gl::Context *context = gl::getContext();

	if(context)
	{
		if(context->getListIndex() != 0)
		{
			UNIMPLEMENTED();
		}

		switch(pname)
		{
		case GL_FOG_COLOR:
		{
			gl::Device *device = gl::getDevice();   // FIXME
			device->setFogColor(sw::Color<float>(params[0], params[1], params[2], params[3]));
		}
		break;
		default:
			UNIMPLEMENTED();
			return error(GL_INVALID_ENUM);
		}
	}
}

void APIENTRY glFogi(GLenum pname, GLint param)
{
	TRACE("(GLenum pname = 0x%X, GLint param = %d)", pname, param);

	gl::Context *context = gl::getContext();

	if(context)
	{
		if(context->getListIndex() != 0)
		{
			UNIMPLEMENTED();
		}

		switch(pname)
		{
		case GL_FOG_MODE:
		{
			gl::Device *device = gl::getDevice();   // FIXME
			switch(param)
			{
			case GL_LINEAR: device->setVertexFogMode(sw::FOG_LINEAR); break;
			default:
				UNIMPLEMENTED();
				return error(GL_INVALID_ENUM);
			}
		}
		break;
		default:
			UNIMPLEMENTED();
			return error(GL_INVALID_ENUM);
		}
	}
}

void APIENTRY glFogiv(GLenum pname, const GLint *params)
{
	UNIMPLEMENTED();
}

void APIENTRY glFrustum(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble zNear, GLdouble zFar)
{
	TRACE("(GLdouble left = %f, GLdouble right = %f, GLdouble bottom = %f, GLdouble top = %f, GLdouble zNear = %f, GLdouble zFar = %f)", left, right, bottom, top, zNear, zFar);

	gl::Context *context = gl::getContext();

	if(context)
	{
		if(context->getListIndex() != 0)
		{
			UNIMPLEMENTED();
		}

		context->frustum(left, right, bottom, top, zNear, zFar);
	}
}

GLuint APIENTRY glGenLists(GLsizei range)
{
	TRACE("(GLsizei range = %d)", range);

	if(range < 0)
	{
		return error(GL_INVALID_VALUE, 0);
	}

	gl::Context *context = gl::getContext();

	if(context)
	{
		return context->genLists(range);
	}

	return 0;
}

void APIENTRY glGetClipPlane(GLenum plane, GLdouble *equation)
{
	UNIMPLEMENTED();
}

void APIENTRY glGetDoublev(GLenum pname, GLdouble *params)
{
	UNIMPLEMENTED();
}

void APIENTRY glGetLightfv(GLenum light, GLenum pname, GLfloat *params)
{
	UNIMPLEMENTED();
}

void APIENTRY glGetLightiv(GLenum light, GLenum pname, GLint *params)
{
	UNIMPLEMENTED();
}

void APIENTRY glGetMapdv(GLenum target, GLenum query, GLdouble *v)
{
	UNIMPLEMENTED();
}

void APIENTRY glGetMapfv(GLenum target, GLenum query, GLfloat *v)
{
	UNIMPLEMENTED();
}

void APIENTRY glGetMapiv(GLenum target, GLenum query, GLint *v)
{
	UNIMPLEMENTED();
}

void APIENTRY glGetMaterialfv(GLenum face, GLenum pname, GLfloat *params)
{
	UNIMPLEMENTED();
}

void APIENTRY glGetMaterialiv(GLenum face, GLenum pname, GLint *params)
{
	UNIMPLEMENTED();
}

void APIENTRY glGetPixelMapfv(GLenum map, GLfloat *values)
{
	UNIMPLEMENTED();
}

void APIENTRY glGetPixelMapuiv(GLenum map, GLuint *values)
{
	UNIMPLEMENTED();
}

void APIENTRY glGetPixelMapusv(GLenum map, GLushort *values)
{
	UNIMPLEMENTED();
}

void APIENTRY glGetPointerv(GLenum pname, GLvoid* *params)
{
	UNIMPLEMENTED();
}

void APIENTRY glGetPolygonStipple(GLubyte *mask)
{
	UNIMPLEMENTED();
}

void APIENTRY glGetTexEnvfv(GLenum target, GLenum pname, GLfloat *params)
{
	UNIMPLEMENTED();
}

void APIENTRY glGetTexEnviv(GLenum target, GLenum pname, GLint *params)
{
	UNIMPLEMENTED();
}

void APIENTRY glGetTexGendv(GLenum coord, GLenum pname, GLdouble *params)
{
	UNIMPLEMENTED();
}

void APIENTRY glGetTexGenfv(GLenum coord, GLenum pname, GLfloat *params)
{
	UNIMPLEMENTED();
}

void APIENTRY glGetTexGeniv(GLenum coord, GLenum pname, GLint *params)
{
	UNIMPLEMENTED();
}

void APIENTRY glGetTexImage(GLenum target, GLint level, GLenum format, GLenum type, GLvoid *pixels)
{
	TRACE("(GLenum target = 0x%X, GLint level = %d, GLenum format = 0x%X, GLenum type = 0x%X, GLint *pixels%p)", target, level, format, type, pixels);

	gl::Context *context = gl::getContext();

	if(context)
	{
		gl::Texture *texture;

		switch(target)
		{
		case GL_TEXTURE_2D:
			texture = context->getTexture2D(target);
			break;
		case GL_TEXTURE_CUBE_MAP:
			texture = context->getTextureCubeMap();
			break;
		default:
			UNIMPLEMENTED();
			return error(GL_INVALID_ENUM);
		}

		if(format == texture->getFormat(target, level) && type == texture->getType(target, level))
		{
			gl::Image *image = texture->getRenderTarget(target, level);
			void *source = image->lock(0, 0, sw::LOCK_READONLY);
			memcpy(pixels, source, image->getPitch() * image->getHeight());
			image->unlock();
		}
		else
		{
			UNIMPLEMENTED();
		}
	}
}

void APIENTRY glGetTexLevelParameterfv(GLenum target, GLint level, GLenum pname, GLfloat *params)
{
	UNIMPLEMENTED();
}

void APIENTRY glGetTexLevelParameteriv(GLenum target, GLint level, GLenum pname, GLint *params)
{
	TRACE("(GLenum target = 0x%X, GLint level = %d, GLenum pname = 0x%X, GLint *params = %p)", target, level, pname, params);

	gl::Context *context = gl::getContext();

	if(context)
	{
		gl::Texture *texture;

		switch(target)
		{
		case GL_TEXTURE_2D:
		case GL_PROXY_TEXTURE_2D:
			texture = context->getTexture2D(target);
			break;
		case GL_TEXTURE_CUBE_MAP:
			texture = context->getTextureCubeMap();
			break;
		default:
			UNIMPLEMENTED();
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
		case GL_TEXTURE_WIDTH:
			*params = texture->getWidth(target, level);
			break;
		case GL_TEXTURE_HEIGHT:
			*params = texture->getHeight(target, level);
			break;
		case GL_TEXTURE_INTERNAL_FORMAT:
			*params = texture->getInternalFormat(target, level);
			break;
		case GL_TEXTURE_BORDER_COLOR:
			UNIMPLEMENTED();
			break;
		case GL_TEXTURE_BORDER:
			UNIMPLEMENTED();
			break;
		case GL_TEXTURE_MAX_ANISOTROPY_EXT:
			*params = (GLint)texture->getMaxAnisotropy();
			break;
		default:
			return error(GL_INVALID_ENUM);
		}
	}
}

void APIENTRY glIndexMask(GLuint mask)
{
	UNIMPLEMENTED();
}

void APIENTRY glIndexPointer(GLenum type, GLsizei stride, const GLvoid *pointer)
{
	UNIMPLEMENTED();
}

void APIENTRY glIndexd(GLdouble c)
{
	UNIMPLEMENTED();
}

void APIENTRY glIndexdv(const GLdouble *c)
{
	UNIMPLEMENTED();
}

void APIENTRY glIndexf(GLfloat c)
{
	UNIMPLEMENTED();
}

void APIENTRY glIndexfv(const GLfloat *c)
{
	UNIMPLEMENTED();
}

void APIENTRY glIndexi(GLint c)
{
	UNIMPLEMENTED();
}

void APIENTRY glIndexiv(const GLint *c)
{
	UNIMPLEMENTED();
}

void APIENTRY glIndexs(GLshort c)
{
	UNIMPLEMENTED();
}

void APIENTRY glIndexsv(const GLshort *c)
{
	UNIMPLEMENTED();
}

void APIENTRY glIndexub(GLubyte c)
{
	UNIMPLEMENTED();
}

void APIENTRY glIndexubv(const GLubyte *c)
{
	UNIMPLEMENTED();
}

void APIENTRY glInitNames(void)
{
	UNIMPLEMENTED();
}

void APIENTRY glInterleavedArrays(GLenum format, GLsizei stride, const GLvoid *pointer)
{
	UNIMPLEMENTED();
}

GLboolean APIENTRY glIsList(GLuint list)
{
	UNIMPLEMENTED();
	return GL_FALSE;
}

void APIENTRY glLightModelf(GLenum pname, GLfloat param)
{
	UNIMPLEMENTED();
}

void APIENTRY glLightModelfv(GLenum pname, const GLfloat *params)
{
	TRACE("(GLenum pname = 0x%X, const GLint *params)", pname);

	gl::Context *context = gl::getContext();

	if(context)
	{
		if(context->getListIndex() != 0)
		{
			UNIMPLEMENTED();
		}

		gl::Device *device = gl::getDevice();   // FIXME

		switch(pname)
		{
		case GL_LIGHT_MODEL_AMBIENT:
			device->setGlobalAmbient(sw::Color<float>(params[0], params[1], params[2], params[3]));
			break;
		default:
			UNIMPLEMENTED();
			return error(GL_INVALID_ENUM);
		}
	}
}

void APIENTRY glLightModeli(GLenum pname, GLint param)
{
	UNIMPLEMENTED();
}

void APIENTRY glLightModeliv(GLenum pname, const GLint *params)
{
	TRACE("(GLenum pname = 0x%X, const GLint *params)", pname);
	UNIMPLEMENTED();
}

void APIENTRY glLightf(GLenum light, GLenum pname, GLfloat param)
{
	UNIMPLEMENTED();
}

void APIENTRY glLightfv(GLenum light, GLenum pname, const GLfloat *params)
{
	TRACE("(GLenum light = 0x%X, GLenum pname = 0x%X, const GLint *params)", light, pname);

	gl::Context *context = gl::getContext();

	if(context)
	{
		if(context->getListIndex() != 0)
		{
			UNIMPLEMENTED();
		}

		gl::Device *device = gl::getDevice();   // FIXME

		switch(pname)
		{
		case GL_AMBIENT:  device->setLightAmbient(light - GL_LIGHT0, sw::Color<float>(params[0], params[1], params[2], params[3]));  break;
		case GL_DIFFUSE:  device->setLightDiffuse(light - GL_LIGHT0, sw::Color<float>(params[0], params[1], params[2], params[3]));  break;
		case GL_SPECULAR: device->setLightSpecular(light - GL_LIGHT0, sw::Color<float>(params[0], params[1], params[2], params[3])); break;
		case GL_POSITION:
			if(params[3] == 0.0f)   // Directional light
			{
				// Create a very far out point light
				float max = std::max(std::max(abs(params[0]), abs(params[1])), abs(params[2]));
				device->setLightPosition(light - GL_LIGHT0, sw::Point(params[0] / max * 1e10f, params[1] / max * 1e10f, params[2] / max * 1e10f));
			}
			else
			{
				device->setLightPosition(light - GL_LIGHT0, sw::Point(params[0] / params[3], params[1] / params[3], params[2] / params[3]));
			}
			break;
		default:
			UNIMPLEMENTED();
			return error(GL_INVALID_ENUM);
		}
	}
}

void APIENTRY glLighti(GLenum light, GLenum pname, GLint param)
{
	UNIMPLEMENTED();
}

void APIENTRY glLightiv(GLenum light, GLenum pname, const GLint *params)
{
	UNIMPLEMENTED();
}

void APIENTRY glLineStipple(GLint factor, GLushort pattern)
{
	UNIMPLEMENTED();
}

void APIENTRY glListBase(GLuint base)
{
	UNIMPLEMENTED();
}

void APIENTRY glLoadIdentity()
{
	TRACE("()");

	gl::Context *context = gl::getContext();

	if(context)
	{
		if(context->getListIndex() != 0)
		{
			UNIMPLEMENTED();
		}

		context->loadIdentity();
	}
}

void APIENTRY glLoadMatrixd(const GLdouble *m)
{
	UNIMPLEMENTED();
}

void APIENTRY glLoadMatrixf(const GLfloat *m)
{
	UNIMPLEMENTED();
}

void APIENTRY glLoadName(GLuint name)
{
	UNIMPLEMENTED();
}

void APIENTRY glLogicOp(GLenum opcode)
{
	UNIMPLEMENTED();
}

void APIENTRY glMap1d(GLenum target, GLdouble u1, GLdouble u2, GLint stride, GLint order, const GLdouble *points)
{
	UNIMPLEMENTED();
}

void APIENTRY glMap1f(GLenum target, GLfloat u1, GLfloat u2, GLint stride, GLint order, const GLfloat *points)
{
	UNIMPLEMENTED();
}

void APIENTRY glMap2d(GLenum target, GLdouble u1, GLdouble u2, GLint ustride, GLint uorder, GLdouble v1, GLdouble v2, GLint vstride, GLint vorder, const GLdouble *points)
{
	UNIMPLEMENTED();
}

void APIENTRY glMap2f(GLenum target, GLfloat u1, GLfloat u2, GLint ustride, GLint uorder, GLfloat v1, GLfloat v2, GLint vstride, GLint vorder, const GLfloat *points)
{
	UNIMPLEMENTED();
}

void APIENTRY glMapGrid1d(GLint un, GLdouble u1, GLdouble u2)
{
	UNIMPLEMENTED();
}

void APIENTRY glMapGrid1f(GLint un, GLfloat u1, GLfloat u2)
{
	UNIMPLEMENTED();
}

void APIENTRY glMapGrid2d(GLint un, GLdouble u1, GLdouble u2, GLint vn, GLdouble v1, GLdouble v2)
{
	UNIMPLEMENTED();
}

void APIENTRY glMapGrid2f(GLint un, GLfloat u1, GLfloat u2, GLint vn, GLfloat v1, GLfloat v2)
{
	UNIMPLEMENTED();
}

void APIENTRY glMaterialf(GLenum face, GLenum pname, GLfloat param)
{
	UNIMPLEMENTED();
}

void APIENTRY glMaterialfv(GLenum face, GLenum pname, const GLfloat *params)
{
	UNIMPLEMENTED();
}

void APIENTRY glMateriali(GLenum face, GLenum pname, GLint param)
{
	UNIMPLEMENTED();
}

void APIENTRY glMaterialiv(GLenum face, GLenum pname, const GLint *params)
{
	UNIMPLEMENTED();
}

void APIENTRY glMatrixMode(GLenum mode)
{
	TRACE("(*)");

	gl::Context *context = gl::getContext();

	if(context)
	{
		if(context->getListIndex() != 0)
		{
			UNIMPLEMENTED();
		}

		context->setMatrixMode(mode);
	}
}

void APIENTRY glMultMatrixd(const GLdouble *m)
{
	TRACE("(*)");

	gl::Context *context = gl::getContext();

	if(context)
	{
		if(context->getListIndex() != 0)
		{
			UNIMPLEMENTED();
		}

		context->multiply(m);
	}
}

void APIENTRY glMultMatrixm(sw::Matrix m)
{
	gl::Context *context = gl::getContext();

	if(context)
	{
		context->multiply((GLfloat*)m.m);
	}
}

void APIENTRY glMultMatrixf(const GLfloat *m)
{
	TRACE("(*)");

	gl::Context *context = gl::getContext();

	if(context)
	{
		if(context->getListIndex() != 0)
		{
			return context->listCommand(gl::newCommand(glMultMatrixm, sw::Matrix(m)));
		}

		context->multiply(m);
	}
}

void APIENTRY glNewList(GLuint list, GLenum mode)
{
	TRACE("(GLuint list = %d, GLenum mode = 0x%X)", list, mode);

	if(list == 0)
	{
		return error(GL_INVALID_VALUE);
	}

	switch(mode)
	{
	case GL_COMPILE:
	case GL_COMPILE_AND_EXECUTE:
		break;
	default:
		return error(GL_INVALID_ENUM);
	}

	gl::Context *context = gl::getContext();

	if(context)
	{
		if(context->getListIndex() != 0)
		{
			UNIMPLEMENTED();
		}

		context->newList(list, mode);
	}
}

void APIENTRY glNormal3b(GLbyte nx, GLbyte ny, GLbyte nz)
{
	UNIMPLEMENTED();
}

void APIENTRY glNormal3bv(const GLbyte *v)
{
	UNIMPLEMENTED();
}

void APIENTRY glNormal3d(GLdouble nx, GLdouble ny, GLdouble nz)
{
	UNIMPLEMENTED();
}

void APIENTRY glNormal3dv(const GLdouble *v)
{
	UNIMPLEMENTED();
}

void APIENTRY glNormal3f(GLfloat nx, GLfloat ny, GLfloat nz)
{
	TRACE("(GLfloat nx = %f, GLfloat ny = %f, GLfloat nz = %f)", nx, ny, nz);

	gl::Context *context = gl::getContext();

	if(context)
	{
		if(context->getListIndex() != 0)
		{
			UNIMPLEMENTED();
		}

		//context->normal(nx, ny, nz);
		context->setVertexAttrib(sw::Normal, nx, ny, nz, 0);
	}
}

void APIENTRY glNormal3fv(const GLfloat *v)
{
	UNIMPLEMENTED();
}

void APIENTRY glNormal3i(GLint nx, GLint ny, GLint nz)
{
	UNIMPLEMENTED();
}

void APIENTRY glNormal3iv(const GLint *v)
{
	UNIMPLEMENTED();
}

void APIENTRY glNormal3s(GLshort nx, GLshort ny, GLshort nz)
{
	UNIMPLEMENTED();
}

void APIENTRY glNormal3sv(const GLshort *v)
{
	UNIMPLEMENTED();
}

void APIENTRY glNormalPointer(GLenum type, GLsizei stride, const GLvoid *pointer)
{
	TRACE("(*)");

	glVertexAttribPointer(sw::Normal, 3, type, true, stride, pointer);
}

void APIENTRY glOrtho(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble zNear, GLdouble zFar)
{
	TRACE("(*)");

	gl::Context *context = gl::getContext();

	if(context)
	{
		if(context->getListIndex() != 0)
		{
			UNIMPLEMENTED();
		}

		context->ortho(left, right, bottom, top, zNear, zFar);
	}
}

void APIENTRY glPassThrough(GLfloat token)
{
	UNIMPLEMENTED();
}

void APIENTRY glPixelMapfv(GLenum map, GLsizei mapsize, const GLfloat *values)
{
	UNIMPLEMENTED();
}

void APIENTRY glPixelMapuiv(GLenum map, GLsizei mapsize, const GLuint *values)
{
	UNIMPLEMENTED();
}

void APIENTRY glPixelMapusv(GLenum map, GLsizei mapsize, const GLushort *values)
{
	UNIMPLEMENTED();
}

void APIENTRY glPixelStoref(GLenum pname, GLfloat param)
{
	UNIMPLEMENTED();
}

void APIENTRY glPixelTransferf(GLenum pname, GLfloat param)
{
	UNIMPLEMENTED();
}

void APIENTRY glPixelTransferi(GLenum pname, GLint param)
{
	UNIMPLEMENTED();
}

void APIENTRY glPixelZoom(GLfloat xfactor, GLfloat yfactor)
{
	UNIMPLEMENTED();
}

void APIENTRY glPointSize(GLfloat size)
{
	UNIMPLEMENTED();
}

void APIENTRY glPolygonMode(GLenum face, GLenum mode)
{
	UNIMPLEMENTED();
}

void APIENTRY glPolygonStipple(const GLubyte *mask)
{
	UNIMPLEMENTED();
}

void APIENTRY glPopAttrib(void)
{
	TRACE("()");
	UNIMPLEMENTED();
}

void APIENTRY glPopClientAttrib(void)
{
	TRACE("()");
}

void APIENTRY glPopMatrix(void)
{
	TRACE("()");

	gl::Context *context = gl::getContext();

	if(context)
	{
		if(context->getListIndex() != 0)
		{
			return context->listCommand(gl::newCommand(glPopMatrix));
		}

		context->popMatrix();
	}
}

void APIENTRY glPopName(void)
{
	UNIMPLEMENTED();
}

void APIENTRY glPrioritizeTextures(GLsizei n, const GLuint *textures, const GLclampf *priorities)
{
	UNIMPLEMENTED();
}

void APIENTRY glPushAttrib(GLbitfield mask)
{
	TRACE("(GLbitfield mask = %u)", mask);
	//UNIMPLEMENTED();
}

void APIENTRY glPushClientAttrib(GLbitfield mask)
{
	TRACE("(GLbitfield mask = %u)", mask);
}

void APIENTRY glPushMatrix(void)
{
	TRACE("()");

	gl::Context *context = gl::getContext();

	if(context)
	{
		if(context->getListIndex() != 0)
		{
			return context->listCommand(gl::newCommand(glPushMatrix));
		}

		context->pushMatrix();
	}
}

void APIENTRY glPushName(GLuint name)
{
	UNIMPLEMENTED();
}

void APIENTRY glRasterPos2d(GLdouble x, GLdouble y)
{
	UNIMPLEMENTED();
}

void APIENTRY glRasterPos2dv(const GLdouble *v)
{
	UNIMPLEMENTED();
}

void APIENTRY glRasterPos2f(GLfloat x, GLfloat y)
{
	UNIMPLEMENTED();
}

void APIENTRY glRasterPos2fv(const GLfloat *v)
{
	UNIMPLEMENTED();
}

void APIENTRY glRasterPos2i(GLint x, GLint y)
{
	UNIMPLEMENTED();
}

void APIENTRY glRasterPos2iv(const GLint *v)
{
	UNIMPLEMENTED();
}

void APIENTRY glRasterPos2s(GLshort x, GLshort y)
{
	UNIMPLEMENTED();
}

void APIENTRY glRasterPos2sv(const GLshort *v)
{
	UNIMPLEMENTED();
}

void APIENTRY glRasterPos3d(GLdouble x, GLdouble y, GLdouble z)
{
	UNIMPLEMENTED();
}

void APIENTRY glRasterPos3dv(const GLdouble *v)
{
	UNIMPLEMENTED();
}

void APIENTRY glRasterPos3f(GLfloat x, GLfloat y, GLfloat z)
{
	UNIMPLEMENTED();
}

void APIENTRY glRasterPos3fv(const GLfloat *v)
{
	UNIMPLEMENTED();
}

void APIENTRY glRasterPos3i(GLint x, GLint y, GLint z)
{
	UNIMPLEMENTED();
}

void APIENTRY glRasterPos3iv(const GLint *v)
{
	UNIMPLEMENTED();
}

void APIENTRY glRasterPos3s(GLshort x, GLshort y, GLshort z)
{
	UNIMPLEMENTED();
}

void APIENTRY glRasterPos3sv(const GLshort *v)
{
	UNIMPLEMENTED();
}

void APIENTRY glRasterPos4d(GLdouble x, GLdouble y, GLdouble z, GLdouble w)
{
	UNIMPLEMENTED();
}

void APIENTRY glRasterPos4dv(const GLdouble *v)
{
	UNIMPLEMENTED();
}

void APIENTRY glRasterPos4f(GLfloat x, GLfloat y, GLfloat z, GLfloat w)
{
	UNIMPLEMENTED();
}

void APIENTRY glRasterPos4fv(const GLfloat *v)
{
	UNIMPLEMENTED();
}

void APIENTRY glRasterPos4i(GLint x, GLint y, GLint z, GLint w)
{
	UNIMPLEMENTED();
}

void APIENTRY glRasterPos4iv(const GLint *v)
{
	UNIMPLEMENTED();
}

void APIENTRY glRasterPos4s(GLshort x, GLshort y, GLshort z, GLshort w)
{
	UNIMPLEMENTED();
}

void APIENTRY glRasterPos4sv(const GLshort *v)
{
	UNIMPLEMENTED();
}

void APIENTRY glReadBuffer(GLenum mode)
{
	UNIMPLEMENTED();
}

void APIENTRY glRectd(GLdouble x1, GLdouble y1, GLdouble x2, GLdouble y2)
{
	UNIMPLEMENTED();
}

void APIENTRY glRectdv(const GLdouble *v1, const GLdouble *v2)
{
	UNIMPLEMENTED();
}

void APIENTRY glRectf(GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2)
{
	UNIMPLEMENTED();
}

void APIENTRY glRectfv(const GLfloat *v1, const GLfloat *v2)
{
	UNIMPLEMENTED();
}

void APIENTRY glRecti(GLint x1, GLint y1, GLint x2, GLint y2)
{
	UNIMPLEMENTED();
}

void APIENTRY glRectiv(const GLint *v1, const GLint *v2)
{
	UNIMPLEMENTED();
}

void APIENTRY glRects(GLshort x1, GLshort y1, GLshort x2, GLshort y2)
{
	UNIMPLEMENTED();
}

void APIENTRY glRectsv(const GLshort *v1, const GLshort *v2)
{
	UNIMPLEMENTED();
}

GLint APIENTRY glRenderMode(GLenum mode)
{
	UNIMPLEMENTED();
	return 0;
}

void APIENTRY glRotated(GLdouble angle, GLdouble x, GLdouble y, GLdouble z)
{
	UNIMPLEMENTED();
}

void APIENTRY glRotatef(GLfloat angle, GLfloat x, GLfloat y, GLfloat z)
{
	TRACE("(*)");

	gl::Context *context = gl::getContext();

	if(context)
	{
		if(context->getListIndex() != 0)
		{
			UNIMPLEMENTED();
		}

		context->rotate(angle, x, y, z);
	}
}

void APIENTRY glScaled(GLdouble x, GLdouble y, GLdouble z)
{
	UNIMPLEMENTED();
}

void APIENTRY glScalef(GLfloat x, GLfloat y, GLfloat z)
{
	TRACE("(GLfloat x = %f, GLfloat y = %f, GLfloat z = %f)", x, y, z);

	gl::Context *context = gl::getContext();

	if(context)
	{
		if(context->getListIndex() != 0)
		{
			return context->listCommand(gl::newCommand(glScalef, x, y, z));
		}

		context->scale(x, y, z);
	}
}

void APIENTRY glSelectBuffer(GLsizei size, GLuint *buffer)
{
	UNIMPLEMENTED();
}

void APIENTRY glShadeModel(GLenum mode)
{
	TRACE("(*)");

	gl::Context *context = gl::getContext();

	if(context)
	{
		if(context->getListIndex() != 0)
		{
			UNIMPLEMENTED();
		}

		context->setShadeModel(mode);
	}
}

void APIENTRY glTexCoord1d(GLdouble s)
{
	UNIMPLEMENTED();
}

void APIENTRY glTexCoord1dv(const GLdouble *v)
{
	UNIMPLEMENTED();
}

void APIENTRY glTexCoord1f(GLfloat s)
{
	UNIMPLEMENTED();
}

void APIENTRY glTexCoord1fv(const GLfloat *v)
{
	UNIMPLEMENTED();
}

void APIENTRY glTexCoord1i(GLint s)
{
	UNIMPLEMENTED();
}

void APIENTRY glTexCoord1iv(const GLint *v)
{
	UNIMPLEMENTED();
}

void APIENTRY glTexCoord1s(GLshort s)
{
	UNIMPLEMENTED();
}

void APIENTRY glTexCoord1sv(const GLshort *v)
{
	UNIMPLEMENTED();
}

void APIENTRY glTexCoord2d(GLdouble s, GLdouble t)
{
	UNIMPLEMENTED();
}

void APIENTRY glTexCoord2dv(const GLdouble *v)
{
	UNIMPLEMENTED();
}

void APIENTRY glTexCoord2f(GLfloat s, GLfloat t)
{
	TRACE("(GLfloat s = %f, GLfloat t = %f)", s, t);

	gl::Context *context = gl::getContext();

	if(context)
	{
		if(context->getListIndex() != 0)
		{
			UNIMPLEMENTED();
		}

		//context->texCoord(s, t, 0.0f, 1.0f);
		unsigned int texture = context->getActiveTexture();
		context->setVertexAttrib(sw::TexCoord0/* + texture*/, s, t, 0.0f, 1.0f);
	}
}

void APIENTRY glTexCoord2fv(const GLfloat *v)
{
	UNIMPLEMENTED();
}

void APIENTRY glTexCoord2i(GLint s, GLint t)
{
	UNIMPLEMENTED();
}

void APIENTRY glTexCoord2iv(const GLint *v)
{
	UNIMPLEMENTED();
}

void APIENTRY glTexCoord2s(GLshort s, GLshort t)
{
	UNIMPLEMENTED();
}

void APIENTRY glTexCoord2sv(const GLshort *v)
{
	UNIMPLEMENTED();
}

void APIENTRY glTexCoord3d(GLdouble s, GLdouble t, GLdouble r)
{
	UNIMPLEMENTED();
}

void APIENTRY glTexCoord3dv(const GLdouble *v)
{
	UNIMPLEMENTED();
}

void APIENTRY glTexCoord3f(GLfloat s, GLfloat t, GLfloat r)
{
	UNIMPLEMENTED();
}

void APIENTRY glTexCoord3fv(const GLfloat *v)
{
	UNIMPLEMENTED();
}

void APIENTRY glTexCoord3i(GLint s, GLint t, GLint r)
{
	UNIMPLEMENTED();
}

void APIENTRY glTexCoord3iv(const GLint *v)
{
	UNIMPLEMENTED();
}

void APIENTRY glTexCoord3s(GLshort s, GLshort t, GLshort r)
{
	UNIMPLEMENTED();
}

void APIENTRY glTexCoord3sv(const GLshort *v)
{
	UNIMPLEMENTED();
}

void APIENTRY glTexCoord4d(GLdouble s, GLdouble t, GLdouble r, GLdouble q)
{
	UNIMPLEMENTED();
}

void APIENTRY glTexCoord4dv(const GLdouble *v)
{
	UNIMPLEMENTED();
}

void APIENTRY glTexCoord4f(GLfloat s, GLfloat t, GLfloat r, GLfloat q)
{
	UNIMPLEMENTED();
}

void APIENTRY glTexCoord4fv(const GLfloat *v)
{
	UNIMPLEMENTED();
}

void APIENTRY glTexCoord4i(GLint s, GLint t, GLint r, GLint q)
{
	UNIMPLEMENTED();
}

void APIENTRY glTexCoord4iv(const GLint *v)
{
	UNIMPLEMENTED();
}

void APIENTRY glTexCoord4s(GLshort s, GLshort t, GLshort r, GLshort q)
{
	UNIMPLEMENTED();
}

void APIENTRY glTexCoord4sv(const GLshort *v)
{
	UNIMPLEMENTED();
}

void APIENTRY glTexCoordPointer(GLint size, GLenum type, GLsizei stride, const GLvoid *pointer)
{
	TRACE("(*)");

	gl::Context *context = gl::getContext();

	if(context)
	{
		GLenum texture = context->getClientActiveTexture();

		glVertexAttribPointer(sw::TexCoord0 + (texture - GL_TEXTURE0), size, type, false, stride, pointer);
	}
}

void APIENTRY glTexEnvf(GLenum target, GLenum pname, GLfloat param)
{
	UNIMPLEMENTED();
}

void APIENTRY glTexEnvfv(GLenum target, GLenum pname, const GLfloat *params)
{
	UNIMPLEMENTED();
}

void APIENTRY glTexEnvi(GLenum target, GLenum pname, GLint param)
{
	TRACE("(GLenum target = 0x%X, GLenum pname = 0x%X, GLint param = %d)", target, pname, param);

	if(target != GL_TEXTURE_ENV || pname != GL_TEXTURE_ENV_MODE)
	{
		return error(GL_INVALID_ENUM);
	}

	switch((GLenum)param)
	{
	case GL_REPLACE:
	case GL_MODULATE:
	case GL_DECAL:
	case GL_BLEND:
		break;
	default:
		error(GL_INVALID_ENUM);
	}

	gl::Context *context = gl::getContext();
}

void APIENTRY glTexEnviv(GLenum target, GLenum pname, const GLint *params)
{
	UNIMPLEMENTED();
}

void APIENTRY glTexGend(GLenum coord, GLenum pname, GLdouble param)
{
	UNIMPLEMENTED();
}

void APIENTRY glTexGendv(GLenum coord, GLenum pname, const GLdouble *params)
{
	UNIMPLEMENTED();
}

void APIENTRY glTexGenf(GLenum coord, GLenum pname, GLfloat param)
{
	UNIMPLEMENTED();
}

void APIENTRY glTexGenfv(GLenum coord, GLenum pname, const GLfloat *params)
{
	UNIMPLEMENTED();
}

void APIENTRY glTexGeni(GLenum coord, GLenum pname, GLint param)
{
	UNIMPLEMENTED();
}

void APIENTRY glTexGeniv(GLenum coord, GLenum pname, const GLint *params)
{
	UNIMPLEMENTED();
}

void APIENTRY glTexImage1D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLint border, GLenum format, GLenum type, const GLvoid *pixels)
{
	TRACE("(GLenum target = 0x%X, GLint level = %d, GLint internalformat = %d, GLsizei width = %d, "
		"GLint border = %d, GLenum format = 0x%X, GLenum type = 0x%X, const GLvoid* pixels =  %p)",
		target, level, internalformat, width, border, format, type, pixels);

	glTexImage2D(target, level, internalformat, width, 1, border, format, type, pixels);
}

void APIENTRY glTexSubImage1D(GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const GLvoid *pixels)
{
	TRACE("(GLenum target = 0x%X, GLint level = %d, GLint xoffset = %d, GLsizei width = %d, "
		"GLenum format =  0x%X, GLenum type = 0x%X, const GLvoid* pixels =  %p)",
		target, level, xoffset, width, format, type, pixels);

	glTexSubImage2D(target, level, xoffset, 0, width, 1, format, type, pixels);
}

void APIENTRY glTranslated(GLdouble x, GLdouble y, GLdouble z)
{
	TRACE("(*)");

	gl::Context *context = gl::getContext();

	if(context)
	{
		if(context->getListIndex() != 0)
		{
			return context->listCommand(gl::newCommand(glTranslated, x, y, z));
		}

		context->translate(x, y, z);   // FIXME
	}
}

void APIENTRY glTranslatef(GLfloat x, GLfloat y, GLfloat z)
{
	TRACE("(GLfloat x = %f, GLfloat y = %f, GLfloat z = %f)", x, y, z);

	gl::Context *context = gl::getContext();

	if(context)
	{
		if(context->getListIndex() != 0)
		{
			return context->listCommand(gl::newCommand(glTranslatef, x, y, z));
		}

		context->translate(x, y, z);
	}
}

void APIENTRY glVertex2d(GLdouble x, GLdouble y)
{
	UNIMPLEMENTED();
}

void APIENTRY glVertex2dv(const GLdouble *v)
{
	UNIMPLEMENTED();
}

void APIENTRY glVertex2f(GLfloat x, GLfloat y)
{
	UNIMPLEMENTED();
}

void APIENTRY glVertex2fv(const GLfloat *v)
{
	UNIMPLEMENTED();
}

void APIENTRY glVertex2i(GLint x, GLint y)
{
	UNIMPLEMENTED();
}

void APIENTRY glVertex2iv(const GLint *v)
{
	UNIMPLEMENTED();
}

void APIENTRY glVertex2s(GLshort x, GLshort y)
{
	UNIMPLEMENTED();
}

void APIENTRY glVertex2sv(const GLshort *v)
{
	UNIMPLEMENTED();
}

void APIENTRY glVertex3d(GLdouble x, GLdouble y, GLdouble z)
{
	UNIMPLEMENTED();
}

void APIENTRY glVertex3dv(const GLdouble *v)
{
	UNIMPLEMENTED();
}

void APIENTRY glVertex3f(GLfloat x, GLfloat y, GLfloat z)
{
	TRACE("(GLfloat x = %f, GLfloat y = %f, GLfloat z = %f)", x, y, z);

	gl::Context *context = gl::getContext();

	if(context)
	{
		if(context->getListIndex() != 0)
		{
			UNIMPLEMENTED();
		}

		context->position(x, y, z, 1.0f);
	}
}

void APIENTRY glVertex3fv(const GLfloat *v)
{
	UNIMPLEMENTED();
}

GLAPI void APIENTRY glVertexAttrib4dv(GLuint index, const GLdouble *v)
{
	UNIMPLEMENTED();
}

void APIENTRY glVertex3i(GLint x, GLint y, GLint z)
{
	UNIMPLEMENTED();
}

void APIENTRY glVertex3iv(const GLint *v)
{
	UNIMPLEMENTED();
}

void APIENTRY glVertex3s(GLshort x, GLshort y, GLshort z)
{
	UNIMPLEMENTED();
}

void APIENTRY glVertex3sv(const GLshort *v)
{
	UNIMPLEMENTED();
}

void APIENTRY glVertex4d(GLdouble x, GLdouble y, GLdouble z, GLdouble w)
{
	UNIMPLEMENTED();
}

void APIENTRY glVertex4dv(const GLdouble *v)
{
	UNIMPLEMENTED();
}

void APIENTRY glVertex4f(GLfloat x, GLfloat y, GLfloat z, GLfloat w)
{
	UNIMPLEMENTED();
}

void APIENTRY glVertex4fv(const GLfloat *v)
{
	UNIMPLEMENTED();
}

void APIENTRY glVertex4i(GLint x, GLint y, GLint z, GLint w)
{
	UNIMPLEMENTED();
}

void APIENTRY glVertex4iv(const GLint *v)
{
	UNIMPLEMENTED();
}

void APIENTRY glVertex4s(GLshort x, GLshort y, GLshort z, GLshort w)
{
	UNIMPLEMENTED();
}

void APIENTRY glVertex4sv(const GLshort *v)
{
	UNIMPLEMENTED();
}

void APIENTRY glVertexPointer(GLint size, GLenum type, GLsizei stride, const GLvoid *pointer)
{
	TRACE("(GLint size = %d, GLenum type = 0x%X, GLsizei stride = %d, const GLvoid *pointer = %p)", size, type, stride, pointer);

	glVertexAttribPointer(sw::Position, size, type, false, stride, pointer);
}

void APIENTRY glDrawRangeElements(GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const void *indices) { UNIMPLEMENTED(); }
void APIENTRY glTexImage3D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, const void *pixels) { UNIMPLEMENTED(); }
void APIENTRY glTexSubImage3D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const void *pixels) { UNIMPLEMENTED(); }
void APIENTRY glCopyTexSubImage3D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLint x, GLint y, GLsizei width, GLsizei height) { UNIMPLEMENTED(); }

void APIENTRY glClientActiveTexture(GLenum texture)
{
	TRACE("(GLenum texture = 0x%X)", texture);

	switch(texture)
	{
	case GL_TEXTURE0:
	case GL_TEXTURE1:
		break;
	default:
		UNIMPLEMENTED();
		UNREACHABLE();
	}

	gl::Context *context = gl::getContext();

	if(context)
	{
		context->clientActiveTexture(texture);
	}
}

void APIENTRY glCompressedTexImage1D(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLint border, GLsizei imageSize, const void *data) { UNIMPLEMENTED(); }
void APIENTRY glCompressedTexImage3D(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLsizei imageSize, const void *data) { UNIMPLEMENTED(); }
void APIENTRY glCompressedTexSubImage1D(GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLsizei imageSize, const void *data) { UNIMPLEMENTED(); }
void APIENTRY glCompressedTexSubImage3D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, const void *data) { UNIMPLEMENTED(); }
void APIENTRY glGetCompressedTexImage(GLenum target, GLint level, void *img) { UNIMPLEMENTED(); }
void APIENTRY glMultiTexCoord1f(GLenum target, GLfloat s) { UNIMPLEMENTED(); }
void APIENTRY glMultiTexCoord1d(GLenum target, GLdouble s) { UNIMPLEMENTED(); }

void APIENTRY glMultiTexCoord2f(GLenum texture, GLfloat s, GLfloat t)
{
	TRACE("(GLenum texture = 0x%X, GLfloat s = %f, GLfloat t = %f)", texture, s, t);

	gl::Context *context = gl::getContext();

	if(context)
	{
		if(context->getListIndex() != 0)
		{
			UNIMPLEMENTED();
		}

		//context->texCoord(s, t, 0.0f, 1.0f);
		context->setVertexAttrib(sw::TexCoord0 + (texture - GL_TEXTURE0), s, t, 0.0f, 1.0f);
	}
}

void APIENTRY glMultiTexCoord2d(GLenum target, GLdouble s, GLdouble t) { UNIMPLEMENTED(); }
void APIENTRY glMultiTexCoord3f(GLenum target, GLfloat s, GLfloat t, GLfloat r) { UNIMPLEMENTED(); }
void APIENTRY glMultiTexCoord3d(GLenum target, GLdouble s, GLdouble t, GLdouble r) { UNIMPLEMENTED(); }
void APIENTRY glMultiTexCoord4f(GLenum target, GLfloat s, GLfloat t, GLfloat r, GLfloat q) { UNIMPLEMENTED(); }
void APIENTRY glMultiTexCoord4d(GLenum target, GLdouble s, GLdouble t, GLdouble r, GLdouble q) { UNIMPLEMENTED(); }
void APIENTRY glLoadTransposeMatrixf(const GLfloat *m) { UNIMPLEMENTED(); }
void APIENTRY glLoadTransposeMatrixd(const GLdouble *m) { UNIMPLEMENTED(); }
void APIENTRY glMultTransposeMatrixf(const GLfloat *m) { UNIMPLEMENTED(); }
void APIENTRY glMultTransposeMatrixd(const GLdouble *m) { UNIMPLEMENTED(); }
void APIENTRY glFogCoordf(GLfloat coord) { UNIMPLEMENTED(); }
void APIENTRY glFogCoordd(GLdouble coord) { UNIMPLEMENTED(); }
void APIENTRY glFogCoordPointer(GLenum type, GLsizei stride, const void *pointer) { UNIMPLEMENTED(); }
void APIENTRY glMultiDrawArrays(GLenum mode, const GLint *first, const GLsizei *count, GLsizei drawcount) { UNIMPLEMENTED(); }
void APIENTRY glPointParameteri(GLenum pname, GLint param) { UNIMPLEMENTED(); }
void APIENTRY glPointParameterf(GLenum pname, GLfloat param) { UNIMPLEMENTED(); }
void APIENTRY glPointParameteriv(GLenum pname, const GLint *params) { UNIMPLEMENTED(); }
void APIENTRY glPointParameterfv(GLenum pname, const GLfloat *params) { UNIMPLEMENTED(); }
void APIENTRY glSecondaryColor3b(GLbyte red, GLbyte green, GLbyte blue) { UNIMPLEMENTED(); }
void APIENTRY glSecondaryColor3f(GLfloat red, GLfloat green, GLfloat blue) { UNIMPLEMENTED(); }
void APIENTRY glSecondaryColor3d(GLdouble red, GLdouble green, GLdouble blue) { UNIMPLEMENTED(); }
void APIENTRY glSecondaryColor3ub(GLubyte red, GLubyte green, GLubyte blue) { UNIMPLEMENTED(); }
void APIENTRY glSecondaryColorPointer(GLint size, GLenum type, GLsizei stride, const void *pointer) { UNIMPLEMENTED(); }
void APIENTRY glWindowPos2f(GLfloat x, GLfloat y) { UNIMPLEMENTED(); }
void APIENTRY glWindowPos2d(GLdouble x, GLdouble y) { UNIMPLEMENTED(); }
void APIENTRY glWindowPos2i(GLint x, GLint y) { UNIMPLEMENTED(); }
void APIENTRY glWindowPos3f(GLfloat x, GLfloat y, GLfloat z) { UNIMPLEMENTED(); }
void APIENTRY glWindowPos3d(GLdouble x, GLdouble y, GLdouble z) { UNIMPLEMENTED(); }
void APIENTRY glWindowPos3i(GLint x, GLint y, GLint z) { UNIMPLEMENTED(); }
void APIENTRY glGetBufferSubData(GLenum target, GLintptr offset, GLsizeiptr size, void *data) { UNIMPLEMENTED(); }
void *APIENTRY glMapBuffer(GLenum target, GLenum access) { UNIMPLEMENTED(); return 0; }
GLboolean APIENTRY glUnmapBuffer(GLenum target) { UNIMPLEMENTED(); return GL_FALSE; }
void APIENTRY glGetBufferPointerv(GLenum target, GLenum pname, void **params) { UNIMPLEMENTED(); }
void APIENTRY glGenQueries(GLsizei n, GLuint *ids) { UNIMPLEMENTED(); }
void APIENTRY glDeleteQueries(GLsizei n, const GLuint *ids) { UNIMPLEMENTED(); }
GLboolean APIENTRY glIsQuery(GLuint id) { UNIMPLEMENTED(); return 0; }
void APIENTRY glBeginQuery(GLenum target, GLuint id) { UNIMPLEMENTED(); }
void APIENTRY glEndQuery(GLenum target) { UNIMPLEMENTED(); }
void APIENTRY glGetQueryiv(GLenum target, GLenum pname, GLint *params) { UNIMPLEMENTED(); }
void APIENTRY glGetQueryObjectiv(GLuint id, GLenum pname, GLint *params) { UNIMPLEMENTED(); }
void APIENTRY glGetQueryObjectuiv(GLuint id, GLenum pname, GLuint *params) { UNIMPLEMENTED(); }
void APIENTRY glVertexAttrib1s(GLuint index, GLshort x) { UNIMPLEMENTED(); }
void APIENTRY glVertexAttrib1d(GLuint index, GLdouble x) { UNIMPLEMENTED(); }
void APIENTRY glVertexAttrib2s(GLuint index, GLshort x, GLshort y) { UNIMPLEMENTED(); }
void APIENTRY glVertexAttrib2d(GLuint index, GLdouble x, GLdouble y) { UNIMPLEMENTED(); }
void APIENTRY glVertexAttrib3s(GLuint index, GLshort x, GLshort y, GLshort z) { UNIMPLEMENTED(); }
void APIENTRY glVertexAttrib3d(GLuint index, GLdouble x, GLdouble y, GLdouble z) { UNIMPLEMENTED(); }
void APIENTRY glVertexAttrib4s(GLuint index, GLshort x, GLshort y, GLshort z, GLshort w) { UNIMPLEMENTED(); }
void APIENTRY glVertexAttrib4d(GLuint index, GLdouble x, GLdouble y, GLdouble z, GLdouble w) { UNIMPLEMENTED(); }
void APIENTRY glVertexAttrib4Nub(GLuint index, GLubyte x, GLubyte y, GLubyte z, GLubyte w) { UNIMPLEMENTED(); }
void APIENTRY glGetVertexAttribdv(GLuint index, GLenum pname, GLdouble *params) { UNIMPLEMENTED(); }
void APIENTRY glDrawBuffers(GLsizei n, const GLenum *bufs) { UNIMPLEMENTED(); }
void APIENTRY glUniformMatrix2x3fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value) { UNIMPLEMENTED(); }
void APIENTRY glUniformMatrix3x2fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value) { UNIMPLEMENTED(); }
void APIENTRY glUniformMatrix2x4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value) { UNIMPLEMENTED(); }
void APIENTRY glUniformMatrix4x2fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value) { UNIMPLEMENTED(); }
void APIENTRY glUniformMatrix3x4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value) { UNIMPLEMENTED(); }
void APIENTRY glUniformMatrix4x3fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value) { UNIMPLEMENTED(); }

void APIENTRY glFramebufferTextureLayer(GLenum target, GLenum attachment, GLuint texture, GLint level, GLint layer) { UNIMPLEMENTED(); }
void APIENTRY glRenderbufferStorageMultisample(GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height) { UNIMPLEMENTED(); }
void APIENTRY glBlitFramebuffer(GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter) { UNIMPLEMENTED(); }

void WINAPI glBindAttribLocationARB(GLhandleARB programObj, GLuint index, const GLcharARB *name) { UNIMPLEMENTED(); }
void WINAPI glGetActiveAttribARB(GLhandleARB programObj, GLuint index, GLsizei maxLength, GLsizei *length, GLint *size, GLenum *type, GLcharARB *name) { UNIMPLEMENTED(); }
GLint WINAPI glGetAttribLocationARB(GLhandleARB programObj, const GLcharARB *name) { UNIMPLEMENTED(); return 0; }
GLAPI void APIENTRY glDeleteObjectARB(GLhandleARB obj){ UNIMPLEMENTED(); }
GLAPI GLhandleARB APIENTRY glGetHandleARB(GLenum pname){ UNIMPLEMENTED(); return 0; }
GLAPI void APIENTRY glDetachObjectARB(GLhandleARB containerObj, GLhandleARB attachedObj){ UNIMPLEMENTED(); }
GLAPI GLhandleARB APIENTRY glCreateShaderObjectARB(GLenum shaderType){ UNIMPLEMENTED(); return 0; }
GLAPI void APIENTRY glShaderSourceARB(GLhandleARB shaderObj, GLsizei count, const GLcharARB **string, const GLint *length){ UNIMPLEMENTED(); }
GLAPI void APIENTRY glCompileShaderARB(GLhandleARB shaderObj){ UNIMPLEMENTED(); }
GLAPI GLhandleARB APIENTRY glCreateProgramObjectARB(void){ UNIMPLEMENTED(); return 0; }
GLAPI void APIENTRY glAttachObjectARB(GLhandleARB containerObj, GLhandleARB obj){ UNIMPLEMENTED(); }
GLAPI void APIENTRY glLinkProgramARB(GLhandleARB programObj){ UNIMPLEMENTED(); }
GLAPI void APIENTRY glUseProgramObjectARB(GLhandleARB programObj){ UNIMPLEMENTED(); }
GLAPI void APIENTRY glValidateProgramARB(GLhandleARB programObj){ UNIMPLEMENTED(); }
GLAPI void APIENTRY glUniform1fARB(GLint location, GLfloat v0){ UNIMPLEMENTED(); }
GLAPI void APIENTRY glUniform2fARB(GLint location, GLfloat v0, GLfloat v1){ UNIMPLEMENTED(); }
GLAPI void APIENTRY glUniform3fARB(GLint location, GLfloat v0, GLfloat v1, GLfloat v2){ UNIMPLEMENTED(); }
GLAPI void APIENTRY glUniform4fARB(GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3){ UNIMPLEMENTED(); }
GLAPI void APIENTRY glUniform1iARB(GLint location, GLint v0){ UNIMPLEMENTED(); }
GLAPI void APIENTRY glUniform2iARB(GLint location, GLint v0, GLint v1){ UNIMPLEMENTED(); }
GLAPI void APIENTRY glUniform3iARB(GLint location, GLint v0, GLint v1, GLint v2){ UNIMPLEMENTED(); }
GLAPI void APIENTRY glUniform4iARB(GLint location, GLint v0, GLint v1, GLint v2, GLint v3){ UNIMPLEMENTED(); }
GLAPI void APIENTRY glUniform1fvARB(GLint location, GLsizei count, const GLfloat *value){ UNIMPLEMENTED(); }
GLAPI void APIENTRY glUniform2fvARB(GLint location, GLsizei count, const GLfloat *value){ UNIMPLEMENTED(); }
GLAPI void APIENTRY glUniform3fvARB(GLint location, GLsizei count, const GLfloat *value){ UNIMPLEMENTED(); }
GLAPI void APIENTRY glUniform4fvARB(GLint location, GLsizei count, const GLfloat *value){ UNIMPLEMENTED(); }
GLAPI void APIENTRY glUniform1ivARB(GLint location, GLsizei count, const GLint *value){ UNIMPLEMENTED(); }
GLAPI void APIENTRY glUniform2ivARB(GLint location, GLsizei count, const GLint *value){ UNIMPLEMENTED(); }
GLAPI void APIENTRY glUniform3ivARB(GLint location, GLsizei count, const GLint *value){ UNIMPLEMENTED(); }
GLAPI void APIENTRY glUniform4ivARB(GLint location, GLsizei count, const GLint *value){ UNIMPLEMENTED(); }
GLAPI void APIENTRY glUniformMatrix2fvARB(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value){ UNIMPLEMENTED(); }
GLAPI void APIENTRY glUniformMatrix3fvARB(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value){ UNIMPLEMENTED(); }
GLAPI void APIENTRY glUniformMatrix4fvARB(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value){ UNIMPLEMENTED(); }
GLAPI void APIENTRY glGetObjectParameterfvARB(GLhandleARB obj, GLenum pname, GLfloat *params){ UNIMPLEMENTED(); }
GLAPI void APIENTRY glGetObjectParameterivARB(GLhandleARB obj, GLenum pname, GLint *params){ UNIMPLEMENTED(); }
GLAPI void APIENTRY glGetInfoLogARB(GLhandleARB obj, GLsizei maxLength, GLsizei *length, GLcharARB *infoLog){ UNIMPLEMENTED(); }
GLAPI void APIENTRY glGetAttachedObjectsARB(GLhandleARB containerObj, GLsizei maxCount, GLsizei *count, GLhandleARB *obj){ UNIMPLEMENTED(); }
GLAPI GLint APIENTRY glGetUniformLocationARB(GLhandleARB programObj, const GLcharARB *name){ UNIMPLEMENTED(); return 0; }
GLAPI void APIENTRY glGetActiveUniformARB(GLhandleARB programObj, GLuint index, GLsizei maxLength, GLsizei *length, GLint *size, GLenum *type, GLcharARB *name){ UNIMPLEMENTED(); }
GLAPI void APIENTRY glGetUniformfvARB(GLhandleARB programObj, GLint location, GLfloat *params){ UNIMPLEMENTED(); }
GLAPI void APIENTRY glGetUniformivARB(GLhandleARB programObj, GLint location, GLint *params){ UNIMPLEMENTED(); }
GLAPI void APIENTRY glGetShaderSourceARB(GLhandleARB obj, GLsizei maxLength, GLsizei *length, GLcharARB *source){ UNIMPLEMENTED(); }

GLAPI GLboolean APIENTRY glIsRenderbufferEXT(GLuint renderbuffer){ UNIMPLEMENTED(); return 0; }
GLAPI void APIENTRY glBindRenderbufferEXT(GLenum target, GLuint renderbuffer){ UNIMPLEMENTED(); }
GLAPI void APIENTRY glDeleteRenderbuffersEXT(GLsizei n, const GLuint *renderbuffers){ UNIMPLEMENTED(); }
GLAPI void APIENTRY glGenRenderbuffersEXT(GLsizei n, GLuint *renderbuffers){ UNIMPLEMENTED(); }
GLAPI void APIENTRY glRenderbufferStorageEXT(GLenum target, GLenum internalformat, GLsizei width, GLsizei height){ UNIMPLEMENTED(); }
GLAPI void APIENTRY glGetRenderbufferParameterivEXT(GLenum target, GLenum pname, GLint *params){ UNIMPLEMENTED(); }
GLAPI GLboolean APIENTRY glIsFramebufferEXT(GLuint framebuffer){ UNIMPLEMENTED(); return 0; }
GLAPI void APIENTRY glBindFramebufferEXT(GLenum target, GLuint framebuffer){ UNIMPLEMENTED(); }
GLAPI void APIENTRY glDeleteFramebuffersEXT(GLsizei n, const GLuint *framebuffers)
{
	TRACE("(GLsizei n = %d, const GLuint* framebuffers = %p)", n, framebuffers);

	glDeleteFramebuffers(n, framebuffers);
}
GLAPI void APIENTRY glGenFramebuffersEXT(GLsizei n, GLuint *framebuffers)
{
	TRACE("(GLsizei n = %d, GLuint* framebuffers = %p)", n, framebuffers);

	glGenFramebuffers(n, framebuffers);
}
GLAPI GLenum APIENTRY glCheckFramebufferStatusEXT(GLenum target){ UNIMPLEMENTED(); return 0; }
GLAPI void APIENTRY glFramebufferTexture1DEXT(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level){ UNIMPLEMENTED(); }
GLAPI void APIENTRY glFramebufferTexture2DEXT(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level){ UNIMPLEMENTED(); }
GLAPI void APIENTRY glFramebufferTexture3DEXT(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level, GLint zoffset){ UNIMPLEMENTED(); }
GLAPI void APIENTRY glFramebufferRenderbufferEXT(GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer){ UNIMPLEMENTED(); }
GLAPI void APIENTRY glGetFramebufferAttachmentParameterivEXT(GLenum target, GLenum attachment, GLenum pname, GLint *params){ UNIMPLEMENTED(); }
GLAPI void APIENTRY glGenerateMipmapEXT(GLenum target){ UNIMPLEMENTED(); }

HGLRC WINAPI wglCreateContextAttribsARB(HDC hDC, HGLRC hShareContext, const int *attribList)
{
	return wglCreateContext(hDC);
}

HANDLE WINAPI wglCreateBufferRegionARB(HDC hDC, int iLayerPlane, UINT uType) { UNIMPLEMENTED(); return 0; }
VOID WINAPI wglDeleteBufferRegionARB(HANDLE hRegion) { UNIMPLEMENTED(); }
BOOL WINAPI wglSaveBufferRegionARB(HANDLE hRegion, int x, int y, int width, int height) { UNIMPLEMENTED(); return FALSE; }
BOOL WINAPI wglRestoreBufferRegionARB(HANDLE hRegion, int x, int y, int width, int height, int xSrc, int ySrc) { UNIMPLEMENTED(); return FALSE; }

void APIENTRY glColorMaski(GLuint index, GLboolean r, GLboolean g, GLboolean b, GLboolean a){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glGetBooleani_v(GLenum target, GLuint index, GLboolean *data){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glGetIntegeri_v(GLenum target, GLuint index, GLint *data){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glEnablei(GLenum target, GLuint index){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glDisablei(GLenum target, GLuint index){ TRACE("*"); UNIMPLEMENTED(); }
GLboolean APIENTRY glIsEnabledi(GLenum target, GLuint index){ TRACE("*"); UNIMPLEMENTED(); return 0; }
void APIENTRY glBeginTransformFeedback(GLenum primitiveMode){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glEndTransformFeedback(void){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glBindBufferRange(GLenum target, GLuint index, GLuint buffer, GLintptr offset, GLsizeiptr size){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glBindBufferBase(GLenum target, GLuint index, GLuint buffer){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glTransformFeedbackVaryings(GLuint program, GLsizei count, const GLchar *const*varyings, GLenum bufferMode){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glGetTransformFeedbackVarying(GLuint program, GLuint index, GLsizei bufSize, GLsizei *length, GLsizei *size, GLenum *type, GLchar *name){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glClampColor(GLenum target, GLenum clamp){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glBeginConditionalRender(GLuint id, GLenum mode){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glEndConditionalRender(void){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glVertexAttribIPointer(GLuint index, GLint size, GLenum type, GLsizei stride, const void *pointer){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glGetVertexAttribIiv(GLuint index, GLenum pname, GLint *params){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glGetVertexAttribIuiv(GLuint index, GLenum pname, GLuint *params){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glVertexAttribI1i(GLuint index, GLint x){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glVertexAttribI2i(GLuint index, GLint x, GLint y){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glVertexAttribI3i(GLuint index, GLint x, GLint y, GLint z){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glVertexAttribI4i(GLuint index, GLint x, GLint y, GLint z, GLint w){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glVertexAttribI1ui(GLuint index, GLuint x){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glVertexAttribI2ui(GLuint index, GLuint x, GLuint y){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glVertexAttribI3ui(GLuint index, GLuint x, GLuint y, GLuint z){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glVertexAttribI4ui(GLuint index, GLuint x, GLuint y, GLuint z, GLuint w){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glVertexAttribI1iv(GLuint index, const GLint *v){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glVertexAttribI2iv(GLuint index, const GLint *v){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glVertexAttribI3iv(GLuint index, const GLint *v){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glVertexAttribI4iv(GLuint index, const GLint *v){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glVertexAttribI1uiv(GLuint index, const GLuint *v){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glVertexAttribI2uiv(GLuint index, const GLuint *v){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glVertexAttribI3uiv(GLuint index, const GLuint *v){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glVertexAttribI4uiv(GLuint index, const GLuint *v){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glVertexAttribI4bv(GLuint index, const GLbyte *v){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glVertexAttribI4sv(GLuint index, const GLshort *v){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glVertexAttribI4ubv(GLuint index, const GLubyte *v){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glVertexAttribI4usv(GLuint index, const GLushort *v){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glGetUniformuiv(GLuint program, GLint location, GLuint *params){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glBindFragDataLocation(GLuint program, GLuint color, const GLchar *name){ TRACE("*"); UNIMPLEMENTED(); }
GLint APIENTRY glGetFragDataLocation(GLuint program, const GLchar *name){ TRACE("*"); UNIMPLEMENTED(); return 0; }
void APIENTRY glUniform1ui(GLint location, GLuint v0){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glUniform2ui(GLint location, GLuint v0, GLuint v1){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glUniform3ui(GLint location, GLuint v0, GLuint v1, GLuint v2){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glUniform4ui(GLint location, GLuint v0, GLuint v1, GLuint v2, GLuint v3){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glUniform1uiv(GLint location, GLsizei count, const GLuint *value){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glUniform2uiv(GLint location, GLsizei count, const GLuint *value){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glUniform3uiv(GLint location, GLsizei count, const GLuint *value){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glUniform4uiv(GLint location, GLsizei count, const GLuint *value){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glTexParameterIiv(GLenum target, GLenum pname, const GLint *params){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glTexParameterIuiv(GLenum target, GLenum pname, const GLuint *params){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glGetTexParameterIiv(GLenum target, GLenum pname, GLint *params){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glGetTexParameterIuiv(GLenum target, GLenum pname, GLuint *params){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glClearBufferiv(GLenum buffer, GLint drawbuffer, const GLint *value){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glClearBufferuiv(GLenum buffer, GLint drawbuffer, const GLuint *value){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glClearBufferfv(GLenum buffer, GLint drawbuffer, const GLfloat *value){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glClearBufferfi(GLenum buffer, GLint drawbuffer, GLfloat depth, GLint stencil){ TRACE("*"); UNIMPLEMENTED(); }
const GLubyte *APIENTRY glGetStringi(GLenum name, GLuint index){ TRACE("*"); UNIMPLEMENTED(); return (GLubyte*)""; return 0; }
void *APIENTRY glMapNamedBufferEXT(GLuint buffer, GLenum access){ TRACE("*"); UNIMPLEMENTED(); return 0; }
GLboolean APIENTRY glUnmapNamedBufferEXT(GLuint buffer){ TRACE("*"); UNIMPLEMENTED(); return 0; }
void APIENTRY glGetNamedBufferParameterivEXT(GLuint buffer, GLenum pname, GLint *params){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glGetTextureImageEXT(GLuint texture, GLenum target, GLint level, GLenum format, GLenum type, void *pixels){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glTextureSubImage2DEXT(GLuint texture, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void *pixels){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glTextureSubImage3DEXT(GLuint texture, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const void *pixels){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glcuR0d4nX(GLint* i1)
{
	i1[1] = 0;
}
//
//000007FEE763807A  lea         rax, [rsp + 20h]
//000007FEE763807F  lea         rcx, [rsp + 38h]
//000007FEE7638084  mov         qword ptr[rsp + 20h], rbp
//000007FEE7638089  mov         qword ptr[rsp + 28h], rsi
//000007FEE763808E  mov         dword ptr[rsp + 38h], 0Dh
//000007FEE7638096  mov         dword ptr[rsp + 3Ch], 8
//000007FEE763809E  mov         qword ptr[rsp + 50h], rax
//000007FEE76380A3  mov         dword ptr[rsp + 48h], 7
//000007FEE76380AB  mov         qword ptr[rsp + 40h], 0
//000007FEE76380B4  call        qword ptr[7FEE81F6890h]

void APIENTRY glCopyTexImage1DEXT(GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLint border) { TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glCopyTexImage2DEXT(GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border) { TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glCopyTexSubImage1DEXT(GLenum target, GLint level, GLint xoffset, GLint x, GLint y, GLsizei width) { TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glCopyTexSubImage2DEXT(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height) { TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glCopyTexSubImage3DEXT(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLint x, GLint y, GLsizei width, GLsizei height) { TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glPolygonOffsetEXT(GLfloat factor, GLfloat bias) { TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glTexSubImage1DEXT(GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const void *pixels) { TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glTexSubImage2DEXT(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void *pixels) { TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glTexImage3DEXT(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, const void *pixels) { TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glTexSubImage3DEXT(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const void *pixels) { TRACE("*"); UNIMPLEMENTED(); }
GLboolean APIENTRY glAreTexturesResidentEXT(GLsizei n, const GLuint *textures, GLboolean *residences) { TRACE("*"); UNIMPLEMENTED(); return 0; }
void APIENTRY glBindTextureEXT(GLenum target, GLuint texture) { TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glDeleteTexturesEXT(GLsizei n, const GLuint *textures) { TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glGenTexturesEXT(GLsizei n, GLuint *textures) { TRACE("*"); UNIMPLEMENTED(); }
GLboolean APIENTRY glIsTextureEXT(GLuint texture) { TRACE("*"); UNIMPLEMENTED(); return 0; }
void APIENTRY glPrioritizeTexturesEXT(GLsizei n, const GLuint *textures, const GLclampf *priorities) { TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glDrawRangeElementsEXT(GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const void *indices) { TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glSampleCoverageARB(GLfloat value, GLboolean invert) { TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glActiveTextureARB(GLenum texture)
{
	TRACE("(GLenum texture = 0x%X)", texture);
	glActiveTexture(texture);
}
void APIENTRY glClientActiveTextureARB(GLenum texture) { TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glMultiTexCoord1dARB(GLenum target, GLdouble s) { TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glMultiTexCoord1dvARB(GLenum target, const GLdouble *v) { TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glMultiTexCoord1fARB(GLenum target, GLfloat s) { TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glMultiTexCoord1fvARB(GLenum target, const GLfloat *v) { TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glMultiTexCoord1iARB(GLenum target, GLint s) { TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glMultiTexCoord1ivARB(GLenum target, const GLint *v) { TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glMultiTexCoord1sARB(GLenum target, GLshort s) { TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glMultiTexCoord1svARB(GLenum target, const GLshort *v) { TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glMultiTexCoord2dARB(GLenum target, GLdouble s, GLdouble t) { TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glMultiTexCoord2dvARB(GLenum target, const GLdouble *v) { TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glMultiTexCoord2fARB(GLenum target, GLfloat s, GLfloat t) { TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glMultiTexCoord2fvARB(GLenum target, const GLfloat *v) { TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glMultiTexCoord2iARB(GLenum target, GLint s, GLint t) { TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glMultiTexCoord2ivARB(GLenum target, const GLint *v) { TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glMultiTexCoord2sARB(GLenum target, GLshort s, GLshort t) { TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glMultiTexCoord2svARB(GLenum target, const GLshort *v) { TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glMultiTexCoord3dARB(GLenum target, GLdouble s, GLdouble t, GLdouble r) { TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glMultiTexCoord3dvARB(GLenum target, const GLdouble *v) { TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glMultiTexCoord3fARB(GLenum target, GLfloat s, GLfloat t, GLfloat r) { TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glMultiTexCoord3fvARB(GLenum target, const GLfloat *v) { TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glMultiTexCoord3iARB(GLenum target, GLint s, GLint t, GLint r) { TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glMultiTexCoord3ivARB(GLenum target, const GLint *v) { TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glMultiTexCoord3sARB(GLenum target, GLshort s, GLshort t, GLshort r) { TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glMultiTexCoord3svARB(GLenum target, const GLshort *v) { TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glMultiTexCoord4dARB(GLenum target, GLdouble s, GLdouble t, GLdouble r, GLdouble q) { TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glMultiTexCoord4dvARB(GLenum target, const GLdouble *v) { TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glMultiTexCoord4fARB(GLenum target, GLfloat s, GLfloat t, GLfloat r, GLfloat q) { TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glMultiTexCoord4fvARB(GLenum target, const GLfloat *v) { TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glMultiTexCoord4iARB(GLenum target, GLint s, GLint t, GLint r, GLint q) { TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glMultiTexCoord4ivARB(GLenum target, const GLint *v) { TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glMultiTexCoord4sARB(GLenum target, GLshort s, GLshort t, GLshort r, GLshort q) { TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glMultiTexCoord4svARB(GLenum target, const GLshort *v) { TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glCompressedTexImage3DARB(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLsizei imageSize, const void *data) { TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glCompressedTexImage2DARB(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const void *data) { TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glCompressedTexImage1DARB(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLint border, GLsizei imageSize, const void *data) { TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glCompressedTexSubImage3DARB(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, const void *data) { TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glCompressedTexSubImage2DARB(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const void *data) { TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glCompressedTexSubImage1DARB(GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLsizei imageSize, const void *data) { TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glGetCompressedTexImageARB(GLenum target, GLint level, void *img) { TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glPointParameterfARB(GLenum pname, GLfloat param) { TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glPointParameterfvARB(GLenum pname, const GLfloat *params) { TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glBlendColorEXT(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha) { TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glFogCoordfEXT(GLfloat coord) { TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glFogCoordfvEXT(const GLfloat *coord) { TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glFogCoorddEXT(GLdouble coord) { TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glFogCoorddvEXT(const GLdouble *coord) { TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glFogCoordPointerEXT(GLenum type, GLsizei stride, const void *pointer) { TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glMultiDrawArraysEXT(GLenum mode, const GLint *first, const GLsizei *count, GLsizei primcount) { TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glMultiDrawElementsEXT(GLenum mode, const GLsizei *count, GLenum type, const void *const*indices, GLsizei primcount) { TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glGenQueriesARB(GLsizei n, GLuint *ids) { TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glDeleteQueriesARB(GLsizei n, const GLuint *ids) { TRACE("*"); UNIMPLEMENTED(); }
GLboolean APIENTRY glIsQueryARB(GLuint id) { TRACE("*"); UNIMPLEMENTED(); return 0; }
void APIENTRY glBeginQueryARB(GLenum target, GLuint id) { TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glEndQueryARB(GLenum target) { TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glGetQueryivARB(GLenum target, GLenum pname, GLint *params) { TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glGetQueryObjectivARB(GLuint id, GLenum pname, GLint *params) { TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glGetQueryObjectuivARB(GLuint id, GLenum pname, GLuint *params) { TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glBindBufferARB(GLenum target, GLuint buffer) { TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glDeleteBuffersARB(GLsizei n, const GLuint *buffers) { TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glGenBuffersARB(GLsizei n, GLuint *buffers) { TRACE("*"); UNIMPLEMENTED(); }
GLboolean APIENTRY glIsBufferARB(GLuint buffer) { TRACE("*"); UNIMPLEMENTED(); return 0; }
void APIENTRY glBufferDataARB(GLenum target, GLsizeiptrARB size, const void *data, GLenum usage) { TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glBufferSubDataARB(GLenum target, GLintptrARB offset, GLsizeiptrARB size, const void *data) { TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glGetBufferSubDataARB(GLenum target, GLintptrARB offset, GLsizeiptrARB size, void *data) { TRACE("*"); UNIMPLEMENTED(); }
void *APIENTRY glMapBufferARB(GLenum target, GLenum access) { TRACE("*"); UNIMPLEMENTED(); return 0; }
GLboolean APIENTRY glUnmapBufferARB(GLenum target) { TRACE("*"); UNIMPLEMENTED(); return 0; }
void APIENTRY glGetBufferParameterivARB(GLenum target, GLenum pname, GLint *params) { TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glGetBufferPointervARB(GLenum target, GLenum pname, void **params) { TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glDrawBuffersARB(GLsizei n, const GLenum *bufs) { TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glBlendEquationSeparateEXT(GLenum modeRGB, GLenum modeAlpha) { TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glActiveStencilFaceEXT(GLenum face) { TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glClampColorARB(GLenum target, GLenum clamp){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glDrawArraysInstancedARB(GLenum mode, GLint first, GLsizei count, GLsizei primcount){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glDrawElementsInstancedARB(GLenum mode, GLsizei count, GLenum type, const void *indices, GLsizei primcount){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glProgramParameteriARB(GLuint program, GLenum pname, GLint value){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glFramebufferTextureARB(GLenum target, GLenum attachment, GLuint texture, GLint level){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glFramebufferTextureLayerARB(GLenum target, GLenum attachment, GLuint texture, GLint level, GLint layer){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glFramebufferTextureFaceARB(GLenum target, GLenum attachment, GLuint texture, GLint level, GLenum face){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glColorTable(GLenum target, GLenum internalformat, GLsizei width, GLenum format, GLenum type, const void *table){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glColorTableParameterfv(GLenum target, GLenum pname, const GLfloat *params){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glColorTableParameteriv(GLenum target, GLenum pname, const GLint *params){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glCopyColorTable(GLenum target, GLenum internalformat, GLint x, GLint y, GLsizei width){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glGetColorTable(GLenum target, GLenum format, GLenum type, void *table){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glGetColorTableParameterfv(GLenum target, GLenum pname, GLfloat *params){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glGetColorTableParameteriv(GLenum target, GLenum pname, GLint *params){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glColorSubTable(GLenum target, GLsizei start, GLsizei count, GLenum format, GLenum type, const void *data){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glCopyColorSubTable(GLenum target, GLsizei start, GLint x, GLint y, GLsizei width){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glConvolutionFilter1D(GLenum target, GLenum internalformat, GLsizei width, GLenum format, GLenum type, const void *image){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glConvolutionFilter2D(GLenum target, GLenum internalformat, GLsizei width, GLsizei height, GLenum format, GLenum type, const void *image){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glConvolutionParameterf(GLenum target, GLenum pname, GLfloat params){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glConvolutionParameterfv(GLenum target, GLenum pname, const GLfloat *params){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glConvolutionParameteri(GLenum target, GLenum pname, GLint params){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glConvolutionParameteriv(GLenum target, GLenum pname, const GLint *params){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glCopyConvolutionFilter1D(GLenum target, GLenum internalformat, GLint x, GLint y, GLsizei width){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glCopyConvolutionFilter2D(GLenum target, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glGetConvolutionFilter(GLenum target, GLenum format, GLenum type, void *image){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glGetConvolutionParameterfv(GLenum target, GLenum pname, GLfloat *params){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glGetConvolutionParameteriv(GLenum target, GLenum pname, GLint *params){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glGetSeparableFilter(GLenum target, GLenum format, GLenum type, void *row, void *column, void *span){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glSeparableFilter2D(GLenum target, GLenum internalformat, GLsizei width, GLsizei height, GLenum format, GLenum type, const void *row, const void *column){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glGetHistogram(GLenum target, GLboolean reset, GLenum format, GLenum type, void *values){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glGetHistogramParameterfv(GLenum target, GLenum pname, GLfloat *params){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glGetHistogramParameteriv(GLenum target, GLenum pname, GLint *params){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glGetMinmax(GLenum target, GLboolean reset, GLenum format, GLenum type, void *values){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glGetMinmaxParameterfv(GLenum target, GLenum pname, GLfloat *params){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glGetMinmaxParameteriv(GLenum target, GLenum pname, GLint *params){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glHistogram(GLenum target, GLsizei width, GLenum internalformat, GLboolean sink){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glMinmax(GLenum target, GLenum internalformat, GLboolean sink){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glResetHistogram(GLenum target){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glResetMinmax(GLenum target){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glVertexAttribDivisorARB(GLuint index, GLuint divisor){ TRACE("*"); UNIMPLEMENTED(); }
void *APIENTRY glMapBufferRange(GLenum target, GLintptr offset, GLsizeiptr length, GLbitfield access){ TRACE("*"); UNIMPLEMENTED(); return 0; }
void APIENTRY glFlushMappedBufferRange(GLenum target, GLintptr offset, GLsizeiptr length){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glTexBufferARB(GLenum target, GLenum internalformat, GLuint buffer){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glBindVertexArray(GLuint array){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glDeleteVertexArrays(GLsizei n, const GLuint *arrays){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glGenVertexArrays(GLsizei n, GLuint *arrays){ TRACE("*"); UNIMPLEMENTED(); }
GLboolean APIENTRY glIsVertexArray(GLuint array){ TRACE("*"); UNIMPLEMENTED(); return 0; }
void APIENTRY glProgramStringARB(GLenum target, GLenum format, GLsizei len, const void *string){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glBindProgramARB(GLenum target, GLuint program){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glDeleteProgramsARB(GLsizei n, const GLuint *programs){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glGenProgramsARB(GLsizei n, GLuint *programs){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glProgramEnvParameter4dARB(GLenum target, GLuint index, GLdouble x, GLdouble y, GLdouble z, GLdouble w){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glProgramEnvParameter4dvARB(GLenum target, GLuint index, const GLdouble *params){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glProgramEnvParameter4fARB(GLenum target, GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glProgramEnvParameter4fvARB(GLenum target, GLuint index, const GLfloat *params){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glProgramLocalParameter4dARB(GLenum target, GLuint index, GLdouble x, GLdouble y, GLdouble z, GLdouble w){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glProgramLocalParameter4dvARB(GLenum target, GLuint index, const GLdouble *params){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glProgramLocalParameter4fARB(GLenum target, GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glProgramLocalParameter4fvARB(GLenum target, GLuint index, const GLfloat *params){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glGetProgramEnvParameterdvARB(GLenum target, GLuint index, GLdouble *params){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glGetProgramEnvParameterfvARB(GLenum target, GLuint index, GLfloat *params){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glGetProgramLocalParameterdvARB(GLenum target, GLuint index, GLdouble *params){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glGetProgramLocalParameterfvARB(GLenum target, GLuint index, GLfloat *params){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glGetProgramivARB(GLenum target, GLenum pname, GLint *params){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glGetProgramStringARB(GLenum target, GLenum pname, void *string){ TRACE("*"); UNIMPLEMENTED(); }
GLboolean APIENTRY glIsProgramARB(GLuint program){ TRACE("*"); UNIMPLEMENTED(); return 0; }
void APIENTRY glEnableVertexAttribArrayARB(GLuint index){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glDisableVertexAttribArrayARB(GLuint index){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glGetVertexAttribdvARB(GLuint index, GLenum pname, GLdouble *params){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glGetVertexAttribfvARB(GLuint index, GLenum pname, GLfloat *params){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glGetVertexAttribivARB(GLuint index, GLenum pname, GLint *params){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glGetVertexAttribPointervARB(GLuint index, GLenum pname, void **pointer){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glVertexAttrib1dARB(GLuint index, GLdouble x){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glVertexAttrib1dvARB(GLuint index, const GLdouble *v){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glVertexAttrib1fARB(GLuint index, GLfloat x){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glVertexAttrib1fvARB(GLuint index, const GLfloat *v){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glVertexAttrib1sARB(GLuint index, GLshort x){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glVertexAttrib1svARB(GLuint index, const GLshort *v){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glVertexAttrib2dARB(GLuint index, GLdouble x, GLdouble y){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glVertexAttrib2dvARB(GLuint index, const GLdouble *v){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glVertexAttrib2fARB(GLuint index, GLfloat x, GLfloat y){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glVertexAttrib2fvARB(GLuint index, const GLfloat *v){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glVertexAttrib2sARB(GLuint index, GLshort x, GLshort y){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glVertexAttrib2svARB(GLuint index, const GLshort *v){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glVertexAttrib3dARB(GLuint index, GLdouble x, GLdouble y, GLdouble z){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glVertexAttrib3dvARB(GLuint index, const GLdouble *v){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glVertexAttrib3fARB(GLuint index, GLfloat x, GLfloat y, GLfloat z){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glVertexAttrib3fvARB(GLuint index, const GLfloat *v){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glVertexAttrib3sARB(GLuint index, GLshort x, GLshort y, GLshort z){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glVertexAttrib3svARB(GLuint index, const GLshort *v){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glVertexAttrib4NbvARB(GLuint index, const GLbyte *v){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glVertexAttrib4NivARB(GLuint index, const GLint *v){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glVertexAttrib4NsvARB(GLuint index, const GLshort *v){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glVertexAttrib4NubARB(GLuint index, GLubyte x, GLubyte y, GLubyte z, GLubyte w){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glVertexAttrib4NubvARB(GLuint index, const GLubyte *v){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glVertexAttrib4NuivARB(GLuint index, const GLuint *v){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glVertexAttrib4NusvARB(GLuint index, const GLushort *v){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glVertexAttrib4bvARB(GLuint index, const GLbyte *v){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glVertexAttrib4dARB(GLuint index, GLdouble x, GLdouble y, GLdouble z, GLdouble w){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glVertexAttrib4dvARB(GLuint index, const GLdouble *v){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glVertexAttrib4fARB(GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glVertexAttrib4fvARB(GLuint index, const GLfloat *v){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glVertexAttrib4ivARB(GLuint index, const GLint *v){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glVertexAttrib4sARB(GLuint index, GLshort x, GLshort y, GLshort z, GLshort w){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glVertexAttrib4svARB(GLuint index, const GLshort *v){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glVertexAttrib4ubvARB(GLuint index, const GLubyte *v){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glVertexAttrib4uivARB(GLuint index, const GLuint *v){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glVertexAttrib4usvARB(GLuint index, const GLushort *v){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glVertexAttribPointerARB(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void *pointer){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glWindowPos2dARB(GLdouble x, GLdouble y){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glWindowPos2dvARB(const GLdouble *v){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glWindowPos2fARB(GLfloat x, GLfloat y){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glWindowPos2fvARB(const GLfloat *v){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glWindowPos2iARB(GLint x, GLint y){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glWindowPos2ivARB(const GLint *v){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glWindowPos2sARB(GLshort x, GLshort y){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glWindowPos2svARB(const GLshort *v){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glWindowPos3dARB(GLdouble x, GLdouble y, GLdouble z){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glWindowPos3dvARB(const GLdouble *v){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glWindowPos3fARB(GLfloat x, GLfloat y, GLfloat z){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glWindowPos3fvARB(const GLfloat *v){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glWindowPos3iARB(GLint x, GLint y, GLint z){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glWindowPos3ivARB(const GLint *v){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glWindowPos3sARB(GLshort x, GLshort y, GLshort z){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glWindowPos3svARB(const GLshort *v){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glDrawBuffersATI(GLsizei n, const GLenum *bufs){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glUniformBufferEXT(GLuint program, GLint location, GLuint buffer){ TRACE("*"); UNIMPLEMENTED(); }
GLint APIENTRY glGetUniformBufferSizeEXT(GLuint program, GLint location){ TRACE("*"); UNIMPLEMENTED();  return 0; }
GLintptr APIENTRY glGetUniformOffsetEXT(GLuint program, GLint location){ TRACE("*"); UNIMPLEMENTED();  return 0; }
void APIENTRY glLockArraysEXT(GLint first, GLsizei count){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glUnlockArraysEXT(void){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glDepthBoundsEXT(GLclampd zmin, GLclampd zmax){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glMatrixLoadfEXT(GLenum mode, const GLfloat *m){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glMatrixLoaddEXT(GLenum mode, const GLdouble *m){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glMatrixMultfEXT(GLenum mode, const GLfloat *m){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glMatrixMultdEXT(GLenum mode, const GLdouble *m){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glMatrixLoadIdentityEXT(GLenum mode){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glMatrixRotatefEXT(GLenum mode, GLfloat angle, GLfloat x, GLfloat y, GLfloat z){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glMatrixRotatedEXT(GLenum mode, GLdouble angle, GLdouble x, GLdouble y, GLdouble z){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glMatrixScalefEXT(GLenum mode, GLfloat x, GLfloat y, GLfloat z){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glMatrixScaledEXT(GLenum mode, GLdouble x, GLdouble y, GLdouble z){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glMatrixTranslatefEXT(GLenum mode, GLfloat x, GLfloat y, GLfloat z){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glMatrixTranslatedEXT(GLenum mode, GLdouble x, GLdouble y, GLdouble z){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glMatrixFrustumEXT(GLenum mode, GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble zNear, GLdouble zFar){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glMatrixOrthoEXT(GLenum mode, GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble zNear, GLdouble zFar){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glMatrixPopEXT(GLenum mode){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glMatrixPushEXT(GLenum mode){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glClientAttribDefaultEXT(GLbitfield mask){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glPushClientAttribDefaultEXT(GLbitfield mask){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glTextureParameterfEXT(GLuint texture, GLenum target, GLenum pname, GLfloat param){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glTextureParameterfvEXT(GLuint texture, GLenum target, GLenum pname, const GLfloat *params){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glTextureParameteriEXT(GLuint texture, GLenum target, GLenum pname, GLint param){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glTextureParameterivEXT(GLuint texture, GLenum target, GLenum pname, const GLint *params){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glTextureImage1DEXT(GLuint texture, GLenum target, GLint level, GLint internalformat, GLsizei width, GLint border, GLenum format, GLenum type, const void *pixels){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glTextureImage2DEXT(GLuint texture, GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void *pixels){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glTextureSubImage1DEXT(GLuint texture, GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const void *pixels){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glCopyTextureImage1DEXT(GLuint texture, GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLint border){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glCopyTextureImage2DEXT(GLuint texture, GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glCopyTextureSubImage1DEXT(GLuint texture, GLenum target, GLint level, GLint xoffset, GLint x, GLint y, GLsizei width){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glCopyTextureSubImage2DEXT(GLuint texture, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glGetTextureParameterfvEXT(GLuint texture, GLenum target, GLenum pname, GLfloat *params){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glGetTextureParameterivEXT(GLuint texture, GLenum target, GLenum pname, GLint *params){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glGetTextureLevelParameterfvEXT(GLuint texture, GLenum target, GLint level, GLenum pname, GLfloat *params){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glGetTextureLevelParameterivEXT(GLuint texture, GLenum target, GLint level, GLenum pname, GLint *params){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glTextureImage3DEXT(GLuint texture, GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, const void *pixels){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glCopyTextureSubImage3DEXT(GLuint texture, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLint x, GLint y, GLsizei width, GLsizei height){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glBindMultiTextureEXT(GLenum texunit, GLenum target, GLuint texture){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glMultiTexCoordPointerEXT(GLenum texunit, GLint size, GLenum type, GLsizei stride, const void *pointer){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glMultiTexEnvfEXT(GLenum texunit, GLenum target, GLenum pname, GLfloat param){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glMultiTexEnvfvEXT(GLenum texunit, GLenum target, GLenum pname, const GLfloat *params){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glMultiTexEnviEXT(GLenum texunit, GLenum target, GLenum pname, GLint param){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glMultiTexEnvivEXT(GLenum texunit, GLenum target, GLenum pname, const GLint *params){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glMultiTexGendEXT(GLenum texunit, GLenum coord, GLenum pname, GLdouble param){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glMultiTexGendvEXT(GLenum texunit, GLenum coord, GLenum pname, const GLdouble *params){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glMultiTexGenfEXT(GLenum texunit, GLenum coord, GLenum pname, GLfloat param){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glMultiTexGenfvEXT(GLenum texunit, GLenum coord, GLenum pname, const GLfloat *params){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glMultiTexGeniEXT(GLenum texunit, GLenum coord, GLenum pname, GLint param){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glMultiTexGenivEXT(GLenum texunit, GLenum coord, GLenum pname, const GLint *params){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glGetMultiTexEnvfvEXT(GLenum texunit, GLenum target, GLenum pname, GLfloat *params){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glGetMultiTexEnvivEXT(GLenum texunit, GLenum target, GLenum pname, GLint *params){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glGetMultiTexGendvEXT(GLenum texunit, GLenum coord, GLenum pname, GLdouble *params){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glGetMultiTexGenfvEXT(GLenum texunit, GLenum coord, GLenum pname, GLfloat *params){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glGetMultiTexGenivEXT(GLenum texunit, GLenum coord, GLenum pname, GLint *params){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glMultiTexParameteriEXT(GLenum texunit, GLenum target, GLenum pname, GLint param){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glMultiTexParameterivEXT(GLenum texunit, GLenum target, GLenum pname, const GLint *params){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glMultiTexParameterfEXT(GLenum texunit, GLenum target, GLenum pname, GLfloat param){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glMultiTexParameterfvEXT(GLenum texunit, GLenum target, GLenum pname, const GLfloat *params){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glMultiTexImage1DEXT(GLenum texunit, GLenum target, GLint level, GLint internalformat, GLsizei width, GLint border, GLenum format, GLenum type, const void *pixels){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glMultiTexImage2DEXT(GLenum texunit, GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void *pixels){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glMultiTexSubImage1DEXT(GLenum texunit, GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const void *pixels){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glMultiTexSubImage2DEXT(GLenum texunit, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void *pixels){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glCopyMultiTexImage1DEXT(GLenum texunit, GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLint border){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glCopyMultiTexImage2DEXT(GLenum texunit, GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glCopyMultiTexSubImage1DEXT(GLenum texunit, GLenum target, GLint level, GLint xoffset, GLint x, GLint y, GLsizei width){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glCopyMultiTexSubImage2DEXT(GLenum texunit, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glGetMultiTexImageEXT(GLenum texunit, GLenum target, GLint level, GLenum format, GLenum type, void *pixels){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glGetMultiTexParameterfvEXT(GLenum texunit, GLenum target, GLenum pname, GLfloat *params){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glGetMultiTexParameterivEXT(GLenum texunit, GLenum target, GLenum pname, GLint *params){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glGetMultiTexLevelParameterfvEXT(GLenum texunit, GLenum target, GLint level, GLenum pname, GLfloat *params){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glGetMultiTexLevelParameterivEXT(GLenum texunit, GLenum target, GLint level, GLenum pname, GLint *params){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glMultiTexImage3DEXT(GLenum texunit, GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, const void *pixels){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glMultiTexSubImage3DEXT(GLenum texunit, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const void *pixels){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glCopyMultiTexSubImage3DEXT(GLenum texunit, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLint x, GLint y, GLsizei width, GLsizei height){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glEnableClientStateIndexedEXT(GLenum array, GLuint index){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glDisableClientStateIndexedEXT(GLenum array, GLuint index){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glGetFloatIndexedvEXT(GLenum target, GLuint index, GLfloat *data){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glGetDoubleIndexedvEXT(GLenum target, GLuint index, GLdouble *data){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glGetPointerIndexedvEXT(GLenum target, GLuint index, void **data){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glEnableIndexedEXT(GLenum target, GLuint index){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glDisableIndexedEXT(GLenum target, GLuint index){ TRACE("*"); UNIMPLEMENTED(); }
GLboolean APIENTRY glIsEnabledIndexedEXT(GLenum target, GLuint index){ TRACE("*"); UNIMPLEMENTED(); return 0; }
void APIENTRY glGetIntegerIndexedvEXT(GLenum target, GLuint index, GLint *data){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glGetBooleanIndexedvEXT(GLenum target, GLuint index, GLboolean *data){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glCompressedTextureImage3DEXT(GLuint texture, GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLsizei imageSize, const void *bits){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glCompressedTextureImage2DEXT(GLuint texture, GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const void *bits){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glCompressedTextureImage1DEXT(GLuint texture, GLenum target, GLint level, GLenum internalformat, GLsizei width, GLint border, GLsizei imageSize, const void *bits){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glCompressedTextureSubImage3DEXT(GLuint texture, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, const void *bits){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glCompressedTextureSubImage2DEXT(GLuint texture, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const void *bits){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glCompressedTextureSubImage1DEXT(GLuint texture, GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLsizei imageSize, const void *bits){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glGetCompressedTextureImageEXT(GLuint texture, GLenum target, GLint lod, void *img){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glCompressedMultiTexImage3DEXT(GLenum texunit, GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLsizei imageSize, const void *bits){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glCompressedMultiTexImage2DEXT(GLenum texunit, GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const void *bits){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glCompressedMultiTexImage1DEXT(GLenum texunit, GLenum target, GLint level, GLenum internalformat, GLsizei width, GLint border, GLsizei imageSize, const void *bits){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glCompressedMultiTexSubImage3DEXT(GLenum texunit, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, const void *bits){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glCompressedMultiTexSubImage2DEXT(GLenum texunit, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const void *bits){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glCompressedMultiTexSubImage1DEXT(GLenum texunit, GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLsizei imageSize, const void *bits){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glGetCompressedMultiTexImageEXT(GLenum texunit, GLenum target, GLint lod, void *img){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glMatrixLoadTransposefEXT(GLenum mode, const GLfloat *m){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glMatrixLoadTransposedEXT(GLenum mode, const GLdouble *m){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glMatrixMultTransposefEXT(GLenum mode, const GLfloat *m){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glMatrixMultTransposedEXT(GLenum mode, const GLdouble *m){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glNamedBufferDataEXT(GLuint buffer, GLsizeiptr size, const void *data, GLenum usage){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glNamedBufferSubDataEXT(GLuint buffer, GLintptr offset, GLsizeiptr size, const void *data){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glGetNamedBufferPointervEXT(GLuint buffer, GLenum pname, void **params){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glGetNamedBufferSubDataEXT(GLuint buffer, GLintptr offset, GLsizeiptr size, void *data){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glProgramUniform1fEXT(GLuint program, GLint location, GLfloat v0){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glProgramUniform2fEXT(GLuint program, GLint location, GLfloat v0, GLfloat v1){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glProgramUniform3fEXT(GLuint program, GLint location, GLfloat v0, GLfloat v1, GLfloat v2){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glProgramUniform4fEXT(GLuint program, GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glProgramUniform1iEXT(GLuint program, GLint location, GLint v0){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glProgramUniform2iEXT(GLuint program, GLint location, GLint v0, GLint v1){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glProgramUniform3iEXT(GLuint program, GLint location, GLint v0, GLint v1, GLint v2){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glProgramUniform4iEXT(GLuint program, GLint location, GLint v0, GLint v1, GLint v2, GLint v3){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glProgramUniform1fvEXT(GLuint program, GLint location, GLsizei count, const GLfloat *value){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glProgramUniform2fvEXT(GLuint program, GLint location, GLsizei count, const GLfloat *value){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glProgramUniform3fvEXT(GLuint program, GLint location, GLsizei count, const GLfloat *value){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glProgramUniform4fvEXT(GLuint program, GLint location, GLsizei count, const GLfloat *value){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glProgramUniform1ivEXT(GLuint program, GLint location, GLsizei count, const GLint *value){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glProgramUniform2ivEXT(GLuint program, GLint location, GLsizei count, const GLint *value){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glProgramUniform3ivEXT(GLuint program, GLint location, GLsizei count, const GLint *value){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glProgramUniform4ivEXT(GLuint program, GLint location, GLsizei count, const GLint *value){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glProgramUniformMatrix2fvEXT(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glProgramUniformMatrix3fvEXT(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glProgramUniformMatrix4fvEXT(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glProgramUniformMatrix2x3fvEXT(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glProgramUniformMatrix3x2fvEXT(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glProgramUniformMatrix2x4fvEXT(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glProgramUniformMatrix4x2fvEXT(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glProgramUniformMatrix3x4fvEXT(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glProgramUniformMatrix4x3fvEXT(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glTextureBufferEXT(GLuint texture, GLenum target, GLenum internalformat, GLuint buffer){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glMultiTexBufferEXT(GLenum texunit, GLenum target, GLenum internalformat, GLuint buffer){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glTextureParameterIivEXT(GLuint texture, GLenum target, GLenum pname, const GLint *params){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glTextureParameterIuivEXT(GLuint texture, GLenum target, GLenum pname, const GLuint *params){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glGetTextureParameterIivEXT(GLuint texture, GLenum target, GLenum pname, GLint *params){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glGetTextureParameterIuivEXT(GLuint texture, GLenum target, GLenum pname, GLuint *params){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glMultiTexParameterIivEXT(GLenum texunit, GLenum target, GLenum pname, const GLint *params){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glMultiTexParameterIuivEXT(GLenum texunit, GLenum target, GLenum pname, const GLuint *params){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glGetMultiTexParameterIivEXT(GLenum texunit, GLenum target, GLenum pname, GLint *params){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glGetMultiTexParameterIuivEXT(GLenum texunit, GLenum target, GLenum pname, GLuint *params){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glProgramUniform1uiEXT(GLuint program, GLint location, GLuint v0){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glProgramUniform2uiEXT(GLuint program, GLint location, GLuint v0, GLuint v1){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glProgramUniform3uiEXT(GLuint program, GLint location, GLuint v0, GLuint v1, GLuint v2){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glProgramUniform4uiEXT(GLuint program, GLint location, GLuint v0, GLuint v1, GLuint v2, GLuint v3){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glProgramUniform1uivEXT(GLuint program, GLint location, GLsizei count, const GLuint *value){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glProgramUniform2uivEXT(GLuint program, GLint location, GLsizei count, const GLuint *value){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glProgramUniform3uivEXT(GLuint program, GLint location, GLsizei count, const GLuint *value){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glProgramUniform4uivEXT(GLuint program, GLint location, GLsizei count, const GLuint *value){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glNamedProgramLocalParameters4fvEXT(GLuint program, GLenum target, GLuint index, GLsizei count, const GLfloat *params){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glNamedProgramLocalParameterI4iEXT(GLuint program, GLenum target, GLuint index, GLint x, GLint y, GLint z, GLint w){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glNamedProgramLocalParameterI4ivEXT(GLuint program, GLenum target, GLuint index, const GLint *params){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glNamedProgramLocalParametersI4ivEXT(GLuint program, GLenum target, GLuint index, GLsizei count, const GLint *params){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glNamedProgramLocalParameterI4uiEXT(GLuint program, GLenum target, GLuint index, GLuint x, GLuint y, GLuint z, GLuint w){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glNamedProgramLocalParameterI4uivEXT(GLuint program, GLenum target, GLuint index, const GLuint *params){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glNamedProgramLocalParametersI4uivEXT(GLuint program, GLenum target, GLuint index, GLsizei count, const GLuint *params){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glGetNamedProgramLocalParameterIivEXT(GLuint program, GLenum target, GLuint index, GLint *params){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glGetNamedProgramLocalParameterIuivEXT(GLuint program, GLenum target, GLuint index, GLuint *params){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glEnableClientStateiEXT(GLenum array, GLuint index){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glDisableClientStateiEXT(GLenum array, GLuint index){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glGetFloati_vEXT(GLenum pname, GLuint index, GLfloat *params){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glGetDoublei_vEXT(GLenum pname, GLuint index, GLdouble *params){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glGetPointeri_vEXT(GLenum pname, GLuint index, void **params){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glNamedProgramStringEXT(GLuint program, GLenum target, GLenum format, GLsizei len, const void *string){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glNamedProgramLocalParameter4dEXT(GLuint program, GLenum target, GLuint index, GLdouble x, GLdouble y, GLdouble z, GLdouble w){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glNamedProgramLocalParameter4dvEXT(GLuint program, GLenum target, GLuint index, const GLdouble *params){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glNamedProgramLocalParameter4fEXT(GLuint program, GLenum target, GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glNamedProgramLocalParameter4fvEXT(GLuint program, GLenum target, GLuint index, const GLfloat *params){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glGetNamedProgramLocalParameterdvEXT(GLuint program, GLenum target, GLuint index, GLdouble *params){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glGetNamedProgramLocalParameterfvEXT(GLuint program, GLenum target, GLuint index, GLfloat *params){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glGetNamedProgramivEXT(GLuint program, GLenum target, GLenum pname, GLint *params){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glGetNamedProgramStringEXT(GLuint program, GLenum target, GLenum pname, void *string){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glNamedRenderbufferStorageEXT(GLuint renderbuffer, GLenum internalformat, GLsizei width, GLsizei height){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glGetNamedRenderbufferParameterivEXT(GLuint renderbuffer, GLenum pname, GLint *params){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glNamedRenderbufferStorageMultisampleEXT(GLuint renderbuffer, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glNamedRenderbufferStorageMultisampleCoverageEXT(GLuint renderbuffer, GLsizei coverageSamples, GLsizei colorSamples, GLenum internalformat, GLsizei width, GLsizei height){ TRACE("*"); UNIMPLEMENTED(); }
GLenum APIENTRY glCheckNamedFramebufferStatusEXT(GLuint framebuffer, GLenum target){ TRACE("*"); UNIMPLEMENTED(); return 0; }
void APIENTRY glNamedFramebufferTexture1DEXT(GLuint framebuffer, GLenum attachment, GLenum textarget, GLuint texture, GLint level){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glNamedFramebufferTexture2DEXT(GLuint framebuffer, GLenum attachment, GLenum textarget, GLuint texture, GLint level){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glNamedFramebufferTexture3DEXT(GLuint framebuffer, GLenum attachment, GLenum textarget, GLuint texture, GLint level, GLint zoffset){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glNamedFramebufferRenderbufferEXT(GLuint framebuffer, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glGetNamedFramebufferAttachmentParameterivEXT(GLuint framebuffer, GLenum attachment, GLenum pname, GLint *params){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glGenerateTextureMipmapEXT(GLuint texture, GLenum target){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glGenerateMultiTexMipmapEXT(GLenum texunit, GLenum target){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glFramebufferDrawBufferEXT(GLuint framebuffer, GLenum mode){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glFramebufferDrawBuffersEXT(GLuint framebuffer, GLsizei n, const GLenum *bufs){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glFramebufferReadBufferEXT(GLuint framebuffer, GLenum mode){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glGetFramebufferParameterivEXT(GLuint framebuffer, GLenum pname, GLint *params){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glNamedCopyBufferSubDataEXT(GLuint readBuffer, GLuint writeBuffer, GLintptr readOffset, GLintptr writeOffset, GLsizeiptr size){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glNamedFramebufferTextureEXT(GLuint framebuffer, GLenum attachment, GLuint texture, GLint level){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glNamedFramebufferTextureLayerEXT(GLuint framebuffer, GLenum attachment, GLuint texture, GLint level, GLint layer){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glNamedFramebufferTextureFaceEXT(GLuint framebuffer, GLenum attachment, GLuint texture, GLint level, GLenum face){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glTextureRenderbufferEXT(GLuint texture, GLenum target, GLuint renderbuffer){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glMultiTexRenderbufferEXT(GLenum texunit, GLenum target, GLuint renderbuffer){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glVertexArrayVertexOffsetEXT(GLuint vaobj, GLuint buffer, GLint size, GLenum type, GLsizei stride, GLintptr offset){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glVertexArrayColorOffsetEXT(GLuint vaobj, GLuint buffer, GLint size, GLenum type, GLsizei stride, GLintptr offset){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glVertexArrayEdgeFlagOffsetEXT(GLuint vaobj, GLuint buffer, GLsizei stride, GLintptr offset){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glVertexArrayIndexOffsetEXT(GLuint vaobj, GLuint buffer, GLenum type, GLsizei stride, GLintptr offset){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glVertexArrayNormalOffsetEXT(GLuint vaobj, GLuint buffer, GLenum type, GLsizei stride, GLintptr offset){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glVertexArrayTexCoordOffsetEXT(GLuint vaobj, GLuint buffer, GLint size, GLenum type, GLsizei stride, GLintptr offset){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glVertexArrayMultiTexCoordOffsetEXT(GLuint vaobj, GLuint buffer, GLenum texunit, GLint size, GLenum type, GLsizei stride, GLintptr offset){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glVertexArrayFogCoordOffsetEXT(GLuint vaobj, GLuint buffer, GLenum type, GLsizei stride, GLintptr offset){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glVertexArraySecondaryColorOffsetEXT(GLuint vaobj, GLuint buffer, GLint size, GLenum type, GLsizei stride, GLintptr offset){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glVertexArrayVertexAttribOffsetEXT(GLuint vaobj, GLuint buffer, GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, GLintptr offset){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glVertexArrayVertexAttribIOffsetEXT(GLuint vaobj, GLuint buffer, GLuint index, GLint size, GLenum type, GLsizei stride, GLintptr offset){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glEnableVertexArrayEXT(GLuint vaobj, GLenum array){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glDisableVertexArrayEXT(GLuint vaobj, GLenum array){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glEnableVertexArrayAttribEXT(GLuint vaobj, GLuint index){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glDisableVertexArrayAttribEXT(GLuint vaobj, GLuint index){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glGetVertexArrayIntegervEXT(GLuint vaobj, GLenum pname, GLint *param){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glGetVertexArrayPointervEXT(GLuint vaobj, GLenum pname, void **param){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glGetVertexArrayIntegeri_vEXT(GLuint vaobj, GLuint index, GLenum pname, GLint *param){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glGetVertexArrayPointeri_vEXT(GLuint vaobj, GLuint index, GLenum pname, void **param){ TRACE("*"); UNIMPLEMENTED(); }
void *APIENTRY glMapNamedBufferRangeEXT(GLuint buffer, GLintptr offset, GLsizeiptr length, GLbitfield access){ TRACE("*"); UNIMPLEMENTED(); return 0; }
void APIENTRY glFlushMappedNamedBufferRangeEXT(GLuint buffer, GLintptr offset, GLsizeiptr length){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glNamedBufferStorageEXT(GLuint buffer, GLsizeiptr size, const void *data, GLbitfield flags){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glClearNamedBufferDataEXT(GLuint buffer, GLenum internalformat, GLenum format, GLenum type, const void *data){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glClearNamedBufferSubDataEXT(GLuint buffer, GLenum internalformat, GLsizeiptr offset, GLsizeiptr size, GLenum format, GLenum type, const void *data){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glNamedFramebufferParameteriEXT(GLuint framebuffer, GLenum pname, GLint param){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glGetNamedFramebufferParameterivEXT(GLuint framebuffer, GLenum pname, GLint *params){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glProgramUniform1dEXT(GLuint program, GLint location, GLdouble x){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glProgramUniform2dEXT(GLuint program, GLint location, GLdouble x, GLdouble y){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glProgramUniform3dEXT(GLuint program, GLint location, GLdouble x, GLdouble y, GLdouble z){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glProgramUniform4dEXT(GLuint program, GLint location, GLdouble x, GLdouble y, GLdouble z, GLdouble w){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glProgramUniform1dvEXT(GLuint program, GLint location, GLsizei count, const GLdouble *value){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glProgramUniform2dvEXT(GLuint program, GLint location, GLsizei count, const GLdouble *value){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glProgramUniform3dvEXT(GLuint program, GLint location, GLsizei count, const GLdouble *value){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glProgramUniform4dvEXT(GLuint program, GLint location, GLsizei count, const GLdouble *value){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glProgramUniformMatrix2dvEXT(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLdouble *value){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glProgramUniformMatrix3dvEXT(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLdouble *value){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glProgramUniformMatrix4dvEXT(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLdouble *value){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glProgramUniformMatrix2x3dvEXT(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLdouble *value){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glProgramUniformMatrix2x4dvEXT(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLdouble *value){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glProgramUniformMatrix3x2dvEXT(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLdouble *value){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glProgramUniformMatrix3x4dvEXT(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLdouble *value){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glProgramUniformMatrix4x2dvEXT(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLdouble *value){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glProgramUniformMatrix4x3dvEXT(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLdouble *value){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glTextureBufferRangeEXT(GLuint texture, GLenum target, GLenum internalformat, GLuint buffer, GLintptr offset, GLsizeiptr size){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glTextureStorage1DEXT(GLuint texture, GLenum target, GLsizei levels, GLenum internalformat, GLsizei width){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glTextureStorage2DEXT(GLuint texture, GLenum target, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glTextureStorage3DEXT(GLuint texture, GLenum target, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glTextureStorage2DMultisampleEXT(GLuint texture, GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height, GLboolean fixedsamplelocations){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glTextureStorage3DMultisampleEXT(GLuint texture, GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLboolean fixedsamplelocations){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glVertexArrayBindVertexBufferEXT(GLuint vaobj, GLuint bindingindex, GLuint buffer, GLintptr offset, GLsizei stride){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glVertexArrayVertexAttribFormatEXT(GLuint vaobj, GLuint attribindex, GLint size, GLenum type, GLboolean normalized, GLuint relativeoffset){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glVertexArrayVertexAttribIFormatEXT(GLuint vaobj, GLuint attribindex, GLint size, GLenum type, GLuint relativeoffset){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glVertexArrayVertexAttribLFormatEXT(GLuint vaobj, GLuint attribindex, GLint size, GLenum type, GLuint relativeoffset){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glVertexArrayVertexAttribBindingEXT(GLuint vaobj, GLuint attribindex, GLuint bindingindex){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glVertexArrayVertexBindingDivisorEXT(GLuint vaobj, GLuint bindingindex, GLuint divisor){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glVertexArrayVertexAttribLOffsetEXT(GLuint vaobj, GLuint buffer, GLuint index, GLint size, GLenum type, GLsizei stride, GLintptr offset){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glTexturePageCommitmentEXT(GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLboolean resident){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glVertexArrayVertexAttribDivisorEXT(GLuint vaobj, GLuint index, GLuint divisor){ TRACE("*"); UNIMPLEMENTED(); }


void APIENTRY glColorMaskIndexedEXT(GLuint index, GLboolean r, GLboolean g, GLboolean b, GLboolean a){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glDrawArraysInstancedEXT(GLenum mode, GLint start, GLsizei count, GLsizei primcount){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glDrawElementsInstancedEXT(GLenum mode, GLsizei count, GLenum type, const void *indices, GLsizei primcount){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glProgramParameteriEXT(GLuint program, GLenum pname, GLint value){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glFramebufferTextureEXT(GLenum target, GLenum attachment, GLuint texture, GLint level){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glFramebufferTextureLayerEXT(GLenum target, GLenum attachment, GLuint texture, GLint level, GLint layer){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glFramebufferTextureFaceEXT(GLenum target, GLenum attachment, GLuint texture, GLint level, GLenum face){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glProgramEnvParameters4fvEXT(GLenum target, GLuint index, GLsizei count, const GLfloat *params){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glProgramLocalParameters4fvEXT(GLenum target, GLuint index, GLsizei count, const GLfloat *params){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glGetUniformuivEXT(GLuint program, GLint location, GLuint *params){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glBindFragDataLocationEXT(GLuint program, GLuint color, const GLchar *name){ TRACE("*"); UNIMPLEMENTED(); }
GLint APIENTRY glGetFragDataLocationEXT(GLuint program, const GLchar *name){ TRACE("*"); UNIMPLEMENTED(); return 0; }
void APIENTRY glUniform1uiEXT(GLint location, GLuint v0){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glUniform2uiEXT(GLint location, GLuint v0, GLuint v1){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glUniform3uiEXT(GLint location, GLuint v0, GLuint v1, GLuint v2){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glUniform4uiEXT(GLint location, GLuint v0, GLuint v1, GLuint v2, GLuint v3){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glUniform1uivEXT(GLint location, GLsizei count, const GLuint *value){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glUniform2uivEXT(GLint location, GLsizei count, const GLuint *value){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glUniform3uivEXT(GLint location, GLsizei count, const GLuint *value){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glUniform4uivEXT(GLint location, GLsizei count, const GLuint *value){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glVertexAttribI1iEXT(GLuint index, GLint x){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glVertexAttribI2iEXT(GLuint index, GLint x, GLint y){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glVertexAttribI3iEXT(GLuint index, GLint x, GLint y, GLint z){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glVertexAttribI4iEXT(GLuint index, GLint x, GLint y, GLint z, GLint w){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glVertexAttribI1uiEXT(GLuint index, GLuint x){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glVertexAttribI2uiEXT(GLuint index, GLuint x, GLuint y){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glVertexAttribI3uiEXT(GLuint index, GLuint x, GLuint y, GLuint z){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glVertexAttribI4uiEXT(GLuint index, GLuint x, GLuint y, GLuint z, GLuint w){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glVertexAttribI1ivEXT(GLuint index, const GLint *v){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glVertexAttribI2ivEXT(GLuint index, const GLint *v){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glVertexAttribI3ivEXT(GLuint index, const GLint *v){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glVertexAttribI4ivEXT(GLuint index, const GLint *v){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glVertexAttribI1uivEXT(GLuint index, const GLuint *v){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glVertexAttribI2uivEXT(GLuint index, const GLuint *v){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glVertexAttribI3uivEXT(GLuint index, const GLuint *v){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glVertexAttribI4uivEXT(GLuint index, const GLuint *v){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glVertexAttribI4bvEXT(GLuint index, const GLbyte *v){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glVertexAttribI4svEXT(GLuint index, const GLshort *v){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glVertexAttribI4ubvEXT(GLuint index, const GLubyte *v){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glVertexAttribI4usvEXT(GLuint index, const GLushort *v){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glVertexAttribIPointerEXT(GLuint index, GLint size, GLenum type, GLsizei stride, const void *pointer){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glGetVertexAttribIivEXT(GLuint index, GLenum pname, GLint *params){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glGetVertexAttribIuivEXT(GLuint index, GLenum pname, GLuint *params){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glPointParameterfEXT(GLenum pname, GLfloat param){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glPointParameterfvEXT(GLenum pname, const GLfloat *params){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glTexBufferEXT(GLenum target, GLenum internalformat, GLuint buffer){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glTexParameterIivEXT(GLenum target, GLenum pname, const GLint *params){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glTexParameterIuivEXT(GLenum target, GLenum pname, const GLuint *params){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glGetTexParameterIivEXT(GLenum target, GLenum pname, GLint *params){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glGetTexParameterIuivEXT(GLenum target, GLenum pname, GLuint *params){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glClearColorIiEXT(GLint red, GLint green, GLint blue, GLint alpha){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glClearColorIuiEXT(GLuint red, GLuint green, GLuint blue, GLuint alpha){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glGetQueryObjecti64vEXT(GLuint id, GLenum pname, GLint64 *params){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glGetQueryObjectui64vEXT(GLuint id, GLenum pname, GLuint64 *params){ TRACE("*"); UNIMPLEMENTED(); }
#define GL_GLEXT_PROTOTYPES
void APIENTRY glArrayElementEXT(GLint i){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glColorPointerEXT(GLint size, GLenum type, GLsizei stride, GLsizei count, const void *pointer){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glDrawArraysEXT(GLenum mode, GLint first, GLsizei count){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glEdgeFlagPointerEXT(GLsizei stride, GLsizei count, const GLboolean *pointer){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glGetPointervEXT(GLenum pname, void **params){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glIndexPointerEXT(GLenum type, GLsizei stride, GLsizei count, const void *pointer){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glNormalPointerEXT(GLenum type, GLsizei stride, GLsizei count, const void *pointer){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glTexCoordPointerEXT(GLint size, GLenum type, GLsizei stride, GLsizei count, const void *pointer){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glVertexPointerEXT(GLint size, GLenum type, GLsizei stride, GLsizei count, const void *pointer){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glBeginConditionalRenderNV(GLuint id, GLenum mode){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glEndConditionalRenderNV(void){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glDepthRangedNV(GLdouble zNear, GLdouble zFar){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glClearDepthdNV(GLdouble depth){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glDepthBoundsdNV(GLdouble zmin, GLdouble zmax){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glGetMultisamplefvNV(GLenum pname, GLuint index, GLfloat *val){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glSampleMaskIndexedNV(GLuint index, GLbitfield mask){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glTexRenderbufferNV(GLenum target, GLuint renderbuffer){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glProgramNamedParameter4fNV(GLuint id, GLsizei len, const GLubyte *name, GLfloat x, GLfloat y, GLfloat z, GLfloat w){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glProgramNamedParameter4fvNV(GLuint id, GLsizei len, const GLubyte *name, const GLfloat *v){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glProgramNamedParameter4dNV(GLuint id, GLsizei len, const GLubyte *name, GLdouble x, GLdouble y, GLdouble z, GLdouble w){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glProgramNamedParameter4dvNV(GLuint id, GLsizei len, const GLubyte *name, const GLdouble *v){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glGetProgramNamedParameterfvNV(GLuint id, GLsizei len, const GLubyte *name, GLfloat *params){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glGetProgramNamedParameterdvNV(GLuint id, GLsizei len, const GLubyte *name, GLdouble *params){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glRenderbufferStorageMultisampleCoverageNV(GLenum target, GLsizei coverageSamples, GLsizei colorSamples, GLenum internalformat, GLsizei width, GLsizei height){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glProgramLocalParameterI4iNV(GLenum target, GLuint index, GLint x, GLint y, GLint z, GLint w){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glProgramLocalParameterI4ivNV(GLenum target, GLuint index, const GLint *params){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glProgramLocalParametersI4ivNV(GLenum target, GLuint index, GLsizei count, const GLint *params){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glProgramLocalParameterI4uiNV(GLenum target, GLuint index, GLuint x, GLuint y, GLuint z, GLuint w){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glProgramLocalParameterI4uivNV(GLenum target, GLuint index, const GLuint *params){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glProgramLocalParametersI4uivNV(GLenum target, GLuint index, GLsizei count, const GLuint *params){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glProgramEnvParameterI4iNV(GLenum target, GLuint index, GLint x, GLint y, GLint z, GLint w){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glProgramEnvParameterI4ivNV(GLenum target, GLuint index, const GLint *params){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glProgramEnvParametersI4ivNV(GLenum target, GLuint index, GLsizei count, const GLint *params){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glProgramEnvParameterI4uiNV(GLenum target, GLuint index, GLuint x, GLuint y, GLuint z, GLuint w){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glProgramEnvParameterI4uivNV(GLenum target, GLuint index, const GLuint *params){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glProgramEnvParametersI4uivNV(GLenum target, GLuint index, GLsizei count, const GLuint *params){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glGetProgramLocalParameterIivNV(GLenum target, GLuint index, GLint *params){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glGetProgramLocalParameterIuivNV(GLenum target, GLuint index, GLuint *params){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glGetProgramEnvParameterIivNV(GLenum target, GLuint index, GLint *params){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glGetProgramEnvParameterIuivNV(GLenum target, GLuint index, GLuint *params){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glVertex2hNV(GLhalfNV x, GLhalfNV y){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glVertex2hvNV(const GLhalfNV *v){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glVertex3hNV(GLhalfNV x, GLhalfNV y, GLhalfNV z){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glVertex3hvNV(const GLhalfNV *v){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glVertex4hNV(GLhalfNV x, GLhalfNV y, GLhalfNV z, GLhalfNV w){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glVertex4hvNV(const GLhalfNV *v){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glNormal3hNV(GLhalfNV nx, GLhalfNV ny, GLhalfNV nz){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glNormal3hvNV(const GLhalfNV *v){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glColor3hNV(GLhalfNV red, GLhalfNV green, GLhalfNV blue){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glColor3hvNV(const GLhalfNV *v){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glColor4hNV(GLhalfNV red, GLhalfNV green, GLhalfNV blue, GLhalfNV alpha){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glColor4hvNV(const GLhalfNV *v){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glTexCoord1hNV(GLhalfNV s){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glTexCoord1hvNV(const GLhalfNV *v){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glTexCoord2hNV(GLhalfNV s, GLhalfNV t){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glTexCoord2hvNV(const GLhalfNV *v){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glTexCoord3hNV(GLhalfNV s, GLhalfNV t, GLhalfNV r){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glTexCoord3hvNV(const GLhalfNV *v){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glTexCoord4hNV(GLhalfNV s, GLhalfNV t, GLhalfNV r, GLhalfNV q){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glTexCoord4hvNV(const GLhalfNV *v){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glMultiTexCoord1hNV(GLenum target, GLhalfNV s){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glMultiTexCoord1hvNV(GLenum target, const GLhalfNV *v){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glMultiTexCoord2hNV(GLenum target, GLhalfNV s, GLhalfNV t){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glMultiTexCoord2hvNV(GLenum target, const GLhalfNV *v){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glMultiTexCoord3hNV(GLenum target, GLhalfNV s, GLhalfNV t, GLhalfNV r){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glMultiTexCoord3hvNV(GLenum target, const GLhalfNV *v){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glMultiTexCoord4hNV(GLenum target, GLhalfNV s, GLhalfNV t, GLhalfNV r, GLhalfNV q){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glMultiTexCoord4hvNV(GLenum target, const GLhalfNV *v){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glFogCoordhNV(GLhalfNV fog){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glFogCoordhvNV(const GLhalfNV *fog){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glSecondaryColor3hNV(GLhalfNV red, GLhalfNV green, GLhalfNV blue){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glSecondaryColor3hvNV(const GLhalfNV *v){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glVertexWeighthNV(GLhalfNV weight){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glVertexWeighthvNV(const GLhalfNV *weight){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glVertexAttrib1hNV(GLuint index, GLhalfNV x){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glVertexAttrib1hvNV(GLuint index, const GLhalfNV *v){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glVertexAttrib2hNV(GLuint index, GLhalfNV x, GLhalfNV y){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glVertexAttrib2hvNV(GLuint index, const GLhalfNV *v){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glVertexAttrib3hNV(GLuint index, GLhalfNV x, GLhalfNV y, GLhalfNV z){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glVertexAttrib3hvNV(GLuint index, const GLhalfNV *v){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glVertexAttrib4hNV(GLuint index, GLhalfNV x, GLhalfNV y, GLhalfNV z, GLhalfNV w){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glVertexAttrib4hvNV(GLuint index, const GLhalfNV *v){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glVertexAttribs1hvNV(GLuint index, GLsizei n, const GLhalfNV *v){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glVertexAttribs2hvNV(GLuint index, GLsizei n, const GLhalfNV *v){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glVertexAttribs3hvNV(GLuint index, GLsizei n, const GLhalfNV *v){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glVertexAttribs4hvNV(GLuint index, GLsizei n, const GLhalfNV *v){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glBeginOcclusionQueryNV(GLuint id){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glEndOcclusionQueryNV(void){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glGetOcclusionQueryivNV(GLuint id, GLenum pname, GLint *params){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glGetOcclusionQueryuivNV(GLuint id, GLenum pname, GLuint *params){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glGenOcclusionQueriesNV(GLsizei n, GLuint *ids){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glDeleteOcclusionQueriesNV(GLsizei n, const GLuint *ids){ TRACE("*"); UNIMPLEMENTED(); }
GLboolean APIENTRY glIsOcclusionQueryNV(GLuint id){ TRACE("*"); UNIMPLEMENTED(); return 0; }
void APIENTRY glProgramBufferParametersfvNV(GLenum target, GLuint bindingIndex, GLuint wordIndex, GLsizei count, const GLfloat *params){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glProgramBufferParametersIivNV(GLenum target, GLuint bindingIndex, GLuint wordIndex, GLsizei count, const GLint *params){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glProgramBufferParametersIuivNV(GLenum target, GLuint bindingIndex, GLuint wordIndex, GLsizei count, const GLuint *params){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glPixelDataRangeNV(GLenum target, GLsizei length, const void *pointer){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glFlushPixelDataRangeNV(GLenum target){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glPointParameteriNV(GLenum pname, GLint param){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glPointParameterivNV(GLenum pname, const GLint *params){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glPrimitiveRestartNV(void){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glPrimitiveRestartIndexNV(GLuint index){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glCombinerParameterfvNV(GLenum pname, const GLfloat *params){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glCombinerParameterfNV(GLenum pname, GLfloat param){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glCombinerParameterivNV(GLenum pname, const GLint *params){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glCombinerParameteriNV(GLenum pname, GLint param){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glCombinerInputNV(GLenum stage, GLenum portion, GLenum variable, GLenum input, GLenum mapping, GLenum componentUsage){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glCombinerOutputNV(GLenum stage, GLenum portion, GLenum abOutput, GLenum cdOutput, GLenum sumOutput, GLenum scale, GLenum bias, GLboolean abDotProduct, GLboolean cdDotProduct, GLboolean muxSum){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glFinalCombinerInputNV(GLenum variable, GLenum input, GLenum mapping, GLenum componentUsage){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glGetCombinerInputParameterfvNV(GLenum stage, GLenum portion, GLenum variable, GLenum pname, GLfloat *params){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glGetCombinerInputParameterivNV(GLenum stage, GLenum portion, GLenum variable, GLenum pname, GLint *params){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glGetCombinerOutputParameterfvNV(GLenum stage, GLenum portion, GLenum pname, GLfloat *params){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glGetCombinerOutputParameterivNV(GLenum stage, GLenum portion, GLenum pname, GLint *params){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glGetFinalCombinerInputParameterfvNV(GLenum variable, GLenum pname, GLfloat *params){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glGetFinalCombinerInputParameterivNV(GLenum variable, GLenum pname, GLint *params){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glCombinerStageParameterfvNV(GLenum stage, GLenum pname, const GLfloat *params){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glGetCombinerStageParameterfvNV(GLenum stage, GLenum pname, GLfloat *params){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glMakeBufferResidentNV(GLenum target, GLenum access){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glMakeBufferNonResidentNV(GLenum target){ TRACE("*"); UNIMPLEMENTED(); }
GLboolean APIENTRY glIsBufferResidentNV(GLenum target){ TRACE("*"); UNIMPLEMENTED(); return 0; }
void APIENTRY glMakeNamedBufferResidentNV(GLuint buffer, GLenum access){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glMakeNamedBufferNonResidentNV(GLuint buffer){ TRACE("*"); UNIMPLEMENTED(); }
GLboolean APIENTRY glIsNamedBufferResidentNV(GLuint buffer){ TRACE("*"); UNIMPLEMENTED(); return 0; }
void APIENTRY glGetBufferParameterui64vNV(GLenum target, GLenum pname, GLuint64EXT *params){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glGetNamedBufferParameterui64vNV(GLuint buffer, GLenum pname, GLuint64EXT *params){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glGetIntegerui64vNV(GLenum value, GLuint64EXT *result){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glUniformui64NV(GLint location, GLuint64EXT value){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glUniformui64vNV(GLint location, GLsizei count, const GLuint64EXT *value){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glGetUniformui64vNV(GLuint program, GLint location, GLuint64EXT *params){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glProgramUniformui64NV(GLuint program, GLint location, GLuint64EXT value){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glProgramUniformui64vNV(GLuint program, GLint location, GLsizei count, const GLuint64EXT *value){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glBeginTransformFeedbackNV(GLenum primitiveMode){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glEndTransformFeedbackNV(void){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glTransformFeedbackAttribsNV(GLuint count, const GLint *attribs, GLenum bufferMode){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glBindBufferRangeNV(GLenum target, GLuint index, GLuint buffer, GLintptr offset, GLsizeiptr size){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glBindBufferOffsetNV(GLenum target, GLuint index, GLuint buffer, GLintptr offset){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glBindBufferBaseNV(GLenum target, GLuint index, GLuint buffer){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glTransformFeedbackVaryingsNV(GLuint program, GLsizei count, const GLint *locations, GLenum bufferMode){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glActiveVaryingNV(GLuint program, const GLchar *name){ TRACE("*"); UNIMPLEMENTED(); }
GLint APIENTRY glGetVaryingLocationNV(GLuint program, const GLchar *name){ TRACE("*"); UNIMPLEMENTED(); return 0; }
void APIENTRY glGetActiveVaryingNV(GLuint program, GLuint index, GLsizei bufSize, GLsizei *length, GLsizei *size, GLenum *type, GLchar *name){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glGetTransformFeedbackVaryingNV(GLuint program, GLuint index, GLint *location){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glTransformFeedbackStreamAttribsNV(GLsizei count, const GLint *attribs, GLsizei nbuffers, const GLint *bufstreams, GLenum bufferMode){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glFlushVertexArrayRangeNV(void){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glVertexArrayRangeNV(GLsizei length, const void *pointer){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glBufferAddressRangeNV(GLenum pname, GLuint index, GLuint64EXT address, GLsizeiptr length){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glVertexFormatNV(GLint size, GLenum type, GLsizei stride){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glNormalFormatNV(GLenum type, GLsizei stride){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glColorFormatNV(GLint size, GLenum type, GLsizei stride){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glIndexFormatNV(GLenum type, GLsizei stride){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glTexCoordFormatNV(GLint size, GLenum type, GLsizei stride){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glEdgeFlagFormatNV(GLsizei stride){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glSecondaryColorFormatNV(GLint size, GLenum type, GLsizei stride){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glFogCoordFormatNV(GLenum type, GLsizei stride){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glVertexAttribFormatNV(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glVertexAttribIFormatNV(GLuint index, GLint size, GLenum type, GLsizei stride){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glGetIntegerui64i_vNV(GLenum value, GLuint index, GLuint64EXT *result){ TRACE("*"); UNIMPLEMENTED(); }
GLboolean APIENTRY glAreProgramsResidentNV(GLsizei n, const GLuint *programs, GLboolean *residences){ TRACE("*"); UNIMPLEMENTED(); return 0; }
void APIENTRY glBindProgramNV(GLenum target, GLuint id){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glDeleteProgramsNV(GLsizei n, const GLuint *programs){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glExecuteProgramNV(GLenum target, GLuint id, const GLfloat *params){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glGenProgramsNV(GLsizei n, GLuint *programs){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glGetProgramParameterdvNV(GLenum target, GLuint index, GLenum pname, GLdouble *params){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glGetProgramParameterfvNV(GLenum target, GLuint index, GLenum pname, GLfloat *params){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glGetProgramivNV(GLuint id, GLenum pname, GLint *params){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glGetProgramStringNV(GLuint id, GLenum pname, GLubyte *program){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glGetTrackMatrixivNV(GLenum target, GLuint address, GLenum pname, GLint *params){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glGetVertexAttribdvNV(GLuint index, GLenum pname, GLdouble *params){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glGetVertexAttribfvNV(GLuint index, GLenum pname, GLfloat *params){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glGetVertexAttribivNV(GLuint index, GLenum pname, GLint *params){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glGetVertexAttribPointervNV(GLuint index, GLenum pname, void **pointer){ TRACE("*"); UNIMPLEMENTED(); }
GLboolean APIENTRY glIsProgramNV(GLuint id){ TRACE("*"); UNIMPLEMENTED(); return 0; }
void APIENTRY glLoadProgramNV(GLenum target, GLuint id, GLsizei len, const GLubyte *program){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glProgramParameter4dNV(GLenum target, GLuint index, GLdouble x, GLdouble y, GLdouble z, GLdouble w){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glProgramParameter4dvNV(GLenum target, GLuint index, const GLdouble *v){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glProgramParameter4fNV(GLenum target, GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glProgramParameter4fvNV(GLenum target, GLuint index, const GLfloat *v){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glProgramParameters4dvNV(GLenum target, GLuint index, GLsizei count, const GLdouble *v){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glProgramParameters4fvNV(GLenum target, GLuint index, GLsizei count, const GLfloat *v){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glRequestResidentProgramsNV(GLsizei n, const GLuint *programs){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glTrackMatrixNV(GLenum target, GLuint address, GLenum matrix, GLenum transform){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glVertexAttribPointerNV(GLuint index, GLint fsize, GLenum type, GLsizei stride, const void *pointer){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glVertexAttrib1dNV(GLuint index, GLdouble x){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glVertexAttrib1dvNV(GLuint index, const GLdouble *v){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glVertexAttrib1fNV(GLuint index, GLfloat x){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glVertexAttrib1fvNV(GLuint index, const GLfloat *v){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glVertexAttrib1sNV(GLuint index, GLshort x){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glVertexAttrib1svNV(GLuint index, const GLshort *v){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glVertexAttrib2dNV(GLuint index, GLdouble x, GLdouble y){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glVertexAttrib2dvNV(GLuint index, const GLdouble *v){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glVertexAttrib2fNV(GLuint index, GLfloat x, GLfloat y){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glVertexAttrib2fvNV(GLuint index, const GLfloat *v){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glVertexAttrib2sNV(GLuint index, GLshort x, GLshort y){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glVertexAttrib2svNV(GLuint index, const GLshort *v){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glVertexAttrib3dNV(GLuint index, GLdouble x, GLdouble y, GLdouble z){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glVertexAttrib3dvNV(GLuint index, const GLdouble *v){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glVertexAttrib3fNV(GLuint index, GLfloat x, GLfloat y, GLfloat z){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glVertexAttrib3fvNV(GLuint index, const GLfloat *v){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glVertexAttrib3sNV(GLuint index, GLshort x, GLshort y, GLshort z){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glVertexAttrib3svNV(GLuint index, const GLshort *v){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glVertexAttrib4dNV(GLuint index, GLdouble x, GLdouble y, GLdouble z, GLdouble w){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glVertexAttrib4dvNV(GLuint index, const GLdouble *v){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glVertexAttrib4fNV(GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glVertexAttrib4fvNV(GLuint index, const GLfloat *v){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glVertexAttrib4sNV(GLuint index, GLshort x, GLshort y, GLshort z, GLshort w){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glVertexAttrib4svNV(GLuint index, const GLshort *v){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glVertexAttrib4ubNV(GLuint index, GLubyte x, GLubyte y, GLubyte z, GLubyte w){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glVertexAttrib4ubvNV(GLuint index, const GLubyte *v){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glVertexAttribs1dvNV(GLuint index, GLsizei count, const GLdouble *v){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glVertexAttribs1fvNV(GLuint index, GLsizei count, const GLfloat *v){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glVertexAttribs1svNV(GLuint index, GLsizei count, const GLshort *v){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glVertexAttribs2dvNV(GLuint index, GLsizei count, const GLdouble *v){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glVertexAttribs2fvNV(GLuint index, GLsizei count, const GLfloat *v){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glVertexAttribs2svNV(GLuint index, GLsizei count, const GLshort *v){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glVertexAttribs3dvNV(GLuint index, GLsizei count, const GLdouble *v){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glVertexAttribs3fvNV(GLuint index, GLsizei count, const GLfloat *v){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glVertexAttribs3svNV(GLuint index, GLsizei count, const GLshort *v){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glVertexAttribs4dvNV(GLuint index, GLsizei count, const GLdouble *v){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glVertexAttribs4fvNV(GLuint index, GLsizei count, const GLfloat *v){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glVertexAttribs4svNV(GLuint index, GLsizei count, const GLshort *v){ TRACE("*"); UNIMPLEMENTED(); }
void APIENTRY glVertexAttribs4ubvNV(GLuint index, GLsizei count, const GLubyte *v){ TRACE("*"); UNIMPLEMENTED(); }

GLboolean APIENTRY wgl1fx34c0da()
{
	return -1;// UNIMPLEMENTED();
}

BOOL WINAPI wglSwapIntervalEXT(int interval)
{
	gl::Surface *drawSurface = static_cast<gl::Surface*>(gl::getCurrentDrawSurface());

	if(drawSurface)
	{
		drawSurface->setSwapInterval(interval);
		return TRUE;
	}

	SetLastError(ERROR_DC_NOT_FOUND);
	return FALSE;
}

int WINAPI wglGetSwapIntervalEXT(void)
{
	TRACE("(*)");
	UNIMPLEMENTED();
	return 1;
}

int WINAPI wglChoosePixelFormat(HDC hdc, const PIXELFORMATDESCRIPTOR *ppfd)
{
	TRACE("(*)");

	return 1;
}

BOOL WINAPI wglCopyContext(HGLRC, HGLRC, UINT)
{
	UNIMPLEMENTED();
	return FALSE;
}

HGLRC WINAPI wglCreateContext(HDC hdc)
{
	TRACE("(*)");

	gl::Display *display = gl::Display::getDisplay(hdc);
	display->initialize();

	gl::Context *context = display->createContext(nullptr);

	return (HGLRC)context;
}

HGLRC WINAPI wglCreateLayerContext(HDC, int)
{
	UNIMPLEMENTED();
	return 0;
}

BOOL WINAPI wglDeleteContext(HGLRC context)
{
	gl::Display *display = gl::getDisplay();

	if(display && context)
	{
		display->destroyContext(reinterpret_cast<gl::Context*>(context));

		return TRUE;
	}

	return TRUE;// FALSE;
}

BOOL WINAPI wglDescribeLayerPlane(HDC, int, int, UINT, LPLAYERPLANEDESCRIPTOR)
{
	UNIMPLEMENTED();
	return FALSE;
}



void APIENTRY glVertexAttrib4bv(GLuint index, const GLbyte *v)
{
	TRACE("*");
	UNIMPLEMENTED();
}
void APIENTRY glVertexAttrib4iv(GLuint index, const GLint *v)
{
	TRACE("*");
	UNIMPLEMENTED();
}
void APIENTRY glVertexAttrib4sv(GLuint index, const GLshort *v)
{
	TRACE("*");
	UNIMPLEMENTED();
}
void APIENTRY glVertexAttrib4ubv(GLuint index, const GLubyte *v)
{
	TRACE("*");
	UNIMPLEMENTED();
}
void APIENTRY glVertexAttrib4uiv(GLuint index, const GLuint *v)
{
	TRACE("*");
	UNIMPLEMENTED();
}
void APIENTRY glVertexAttrib4usv(GLuint index, const GLushort *v)
{
	TRACE("*");
	UNIMPLEMENTED();
}


int WINAPI wglDescribePixelFormat(HDC hdc, int iPixelFormat, UINT nBytes, LPPIXELFORMATDESCRIPTOR ppfd)
{
	TRACE("(*)");

	ASSERT(nBytes == sizeof(PIXELFORMATDESCRIPTOR));   // FIXME

	ppfd->nSize = sizeof(PIXELFORMATDESCRIPTOR);
	ppfd->nVersion = 1;
	ppfd->dwFlags = PFD_DRAW_TO_WINDOW | PFD_DRAW_TO_BITMAP | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	ppfd->iPixelType = PFD_TYPE_RGBA;
	ppfd->cColorBits = 32;
	ppfd->cRedBits = 8;
	ppfd->cRedShift = 16;
	ppfd->cGreenBits = 8;
	ppfd->cGreenShift = 8;
	ppfd->cBlueBits = 8;
	ppfd->cBlueShift = 0;
	ppfd->cAlphaBits = 0;
	ppfd->cAlphaShift = 24;
	ppfd->cAccumBits = 0;
	ppfd->cAccumRedBits = 0;
	ppfd->cAccumGreenBits = 0;
	ppfd->cAccumBlueBits = 0;
	ppfd->cAccumAlphaBits = 0;
	ppfd->cDepthBits = 24;
	ppfd->cStencilBits = 0;
	ppfd->cAuxBuffers = 0;
	ppfd->iLayerType = 0;
	ppfd->bReserved = 0;
	ppfd->dwLayerMask = 0;
	ppfd->dwVisibleMask = 0;
	ppfd->dwDamageMask = 0;

	return 1;
}

HGLRC WINAPI wglGetCurrentContext(VOID)
{
	TRACE("(*)");
	return (HGLRC)gl::getContext();
}

HDC WINAPI wglGetCurrentDC(VOID)
{
	TRACE("(*)");
	gl::Display *display = gl::getDisplay();
	return display ? display->getNativeDisplay() : 0;
}

void WINAPI wglGetDefaultProcAddress()
{
	UNIMPLEMENTED();
}

int WINAPI wglGetLayerPaletteEntries(HDC, int, int, int, COLORREF*)
{
	UNIMPLEMENTED();
	return 0;
}

void WINAPI wglGetPixelFormat()
{
	UNIMPLEMENTED();
}

const char *WINAPI wglGetExtensionsStringARB(HDC hdc)
{
	TRACE("(*)");

	return "GL_ARB_texture_non_power_of_two "
		"GL_ARB_framebuffer_object "
		"GL_ARB_fragment_shader "
		"GL_ARB_vertex_shader "
		"GL_ARB_shader_objects "
		"GL_ARB_shading_language_100 "
		"GL_EXT_framebuffer_object";
	//"wglCreateContextAttribsARB";
	//NVIDIA CARD
	"WGL_ARB_buffer_region WGL_ARB_create_context WGL_ARB_create_context_profile WGL_ARB_create_context_robustness WGL_ARB_context_flush_control WGL_ARB_extensions_string WGL_ARB_make_current_read WGL_ARB_multisample WGL_ARB_pbuffer WGL_ARB_pixel_format WGL_ARB_pixel_format_float WGL_ARB_render_texture WGL_ATI_pixel_format_float WGL_EXT_create_context_es_profile WGL_EXT_create_context_es2_profile WGL_EXT_extensions_string WGL_EXT_framebuffer_sRGB WGL_EXT_pixel_format_packed_float WGL_EXT_swap_control WGL_EXT_swap_control_tear WGL_NVX_DX_interop WGL_NV_DX_interop WGL_NV_DX_interop2 WGL_NV_copy_image WGL_NV_delay_before_swap WGL_NV_float_buffer WGL_NV_gpu_affinity WGL_NV_multisample_coverage WGL_NV_render_depth_texture WGL_NV_render_texture_rectangle WGL_NV_swap_group WGL_NV_video_capture";
}

const char *WINAPI wglGetExtensionsStringEXT()
{
	TRACE("(*)");
	return wglGetExtensionsStringARB(0);
}

PROC WINAPI wglGetProcAddress(LPCSTR lpszProc)
{
	TRACE("(LPCSTR lpszProc = \"%s\")", lpszProc);

	struct Extension
	{
		const char *name;
		PROC address;
	};

	static const Extension glExtensions[] =
	{
#define EXT(function) {#function, (PROC)function}

		// Core 2.1
		EXT(glDrawRangeElements),
		EXT(glTexImage3D),
		EXT(glTexSubImage3D),
		EXT(glCopyTexSubImage3D),
		EXT(glActiveTexture),
		EXT(glClientActiveTexture),
		EXT(glCompressedTexImage1D),
		EXT(glCompressedTexImage2D),
		EXT(glCompressedTexImage3D),
		EXT(glCompressedTexSubImage1D),
		EXT(glCompressedTexSubImage2D),
		EXT(glCompressedTexSubImage3D),
		EXT(glGetCompressedTexImage),
		EXT(glMultiTexCoord1f),
		EXT(glMultiTexCoord1d),
		EXT(glMultiTexCoord2f),
		EXT(glMultiTexCoord2d),
		EXT(glMultiTexCoord3f),
		EXT(glMultiTexCoord3d),
		EXT(glMultiTexCoord4f),
		EXT(glMultiTexCoord4d),
		EXT(glLoadTransposeMatrixf),
		EXT(glLoadTransposeMatrixd),
		EXT(glMultTransposeMatrixf),
		EXT(glMultTransposeMatrixd),
		EXT(glSampleCoverage),
		EXT(glBlendEquation),
		EXT(glBlendColor),
		EXT(glFogCoordf),
		EXT(glFogCoordd),
		EXT(glFogCoordPointer),
		EXT(glMultiDrawArrays),
		EXT(glPointParameteri),
		EXT(glPointParameterf),
		EXT(glPointParameteriv),
		EXT(glPointParameterfv),
		EXT(glSecondaryColor3b),
		EXT(glSecondaryColor3f),
		EXT(glSecondaryColor3d),
		EXT(glSecondaryColor3ub),
		EXT(glSecondaryColorPointer),
		EXT(glBlendFuncSeparate),
		EXT(glWindowPos2f),
		EXT(glWindowPos2d),
		EXT(glWindowPos2i),
		EXT(glWindowPos3f),
		EXT(glWindowPos3d),
		EXT(glWindowPos3i),
		EXT(glBindBuffer),
		EXT(glDeleteBuffers),
		EXT(glGenBuffers),
		EXT(glIsBuffer),
		EXT(glBufferData),
		EXT(glBufferSubData),
		EXT(glGetBufferSubData),
		EXT(glMapBuffer),
		EXT(glUnmapBuffer),
		EXT(glGetBufferParameteriv),
		EXT(glGetBufferPointerv),
		EXT(glGenQueries),
		EXT(glDeleteQueries),
		EXT(glIsQuery),
		EXT(glBeginQuery),
		EXT(glEndQuery),
		EXT(glGetQueryiv),
		EXT(glGetQueryObjectiv),
		EXT(glGetQueryObjectuiv),
		EXT(glShaderSource),
		EXT(glCreateShader),
		EXT(glIsShader),
		EXT(glCompileShader),
		EXT(glDeleteShader),
		EXT(glCreateProgram),
		EXT(glIsProgram),
		EXT(glAttachShader),
		EXT(glDetachShader),
		EXT(glLinkProgram),
		EXT(glUseProgram),
		EXT(glValidateProgram),
		EXT(glDeleteProgram),
		EXT(glUniform1f),
		EXT(glUniform2f),
		EXT(glUniform3f),
		EXT(glUniform4f),
		EXT(glUniform1i),
		EXT(glUniform2i),
		EXT(glUniform3i),
		EXT(glUniform4i),
		EXT(glUniform1fv),
		EXT(glUniform2fv),
		EXT(glUniform3fv),
		EXT(glUniform4fv),
		EXT(glUniform1iv),
		EXT(glUniform2iv),
		EXT(glUniform3iv),
		EXT(glUniform4iv),
		EXT(glUniformMatrix2fv),
		EXT(glUniformMatrix3fv),
		EXT(glUniformMatrix4fv),
		EXT(glGetShaderiv),
		EXT(glGetProgramiv),
		EXT(glGetShaderInfoLog),
		EXT(glGetProgramInfoLog),
		EXT(glGetAttachedShaders),
		EXT(glGetUniformLocation),
		EXT(glGetActiveUniform),
		EXT(glGetUniformfv),
		EXT(glGetUniformiv),
		EXT(glGetShaderSource),
		EXT(glVertexAttrib1s),
		EXT(glVertexAttrib1f),
		EXT(glVertexAttrib1d),
		EXT(glVertexAttrib2s),
		EXT(glVertexAttrib2f),
		EXT(glVertexAttrib2d),
		EXT(glVertexAttrib3s),
		EXT(glVertexAttrib3f),
		EXT(glVertexAttrib3d),
		EXT(glVertexAttrib4s),
		EXT(glVertexAttrib4f),
		EXT(glVertexAttrib4d),
		EXT(glVertexAttrib4Nub),
		EXT(glVertexAttribPointer),
		EXT(glEnableVertexAttribArray),
		EXT(glDisableVertexAttribArray),
		EXT(glGetVertexAttribfv),
		EXT(glGetVertexAttribdv),
		EXT(glGetVertexAttribiv),
		EXT(glGetVertexAttribPointerv),
		EXT(glBindAttribLocation),
		EXT(glGetActiveAttrib),
		EXT(glGetAttribLocation),
		EXT(glDrawBuffers),
		EXT(glStencilOpSeparate),
		EXT(glStencilFuncSeparate),
		EXT(glStencilMaskSeparate),
		EXT(glBlendEquationSeparate),
		EXT(glUniformMatrix2x3fv),
		EXT(glUniformMatrix3x2fv),
		EXT(glUniformMatrix2x4fv),
		EXT(glUniformMatrix4x2fv),
		EXT(glUniformMatrix3x4fv),
		EXT(glUniformMatrix4x3fv),
		EXT(glGenFencesNV),
		EXT(glDeleteFencesNV),
		EXT(glSetFenceNV),
		EXT(glTestFenceNV),
		EXT(glFinishFenceNV),
		EXT(glIsFenceNV),
		EXT(glGetFenceivNV),

		EXT(glIsRenderbuffer),
		EXT(glBindRenderbuffer),
		EXT(glDeleteRenderbuffers),
		EXT(glGenRenderbuffers),
		EXT(glRenderbufferStorage),
		EXT(glGetRenderbufferParameteriv),
		EXT(glIsFramebuffer),
		EXT(glBindFramebuffer),
		EXT(glDeleteFramebuffers),
		EXT(glGenFramebuffers),
		EXT(glCheckFramebufferStatus),
		EXT(glFramebufferTexture1D),
		EXT(glFramebufferTexture2D),
		EXT(glFramebufferTexture3D),
		EXT(glFramebufferRenderbuffer),
		EXT(glGetFramebufferAttachmentParameteriv),
		EXT(glGenerateMipmap),
		EXT(glReleaseShaderCompiler),
		EXT(glShaderBinary),
		EXT(glGetShaderPrecisionFormat),
		EXT(glDepthRangef),
		EXT(glClearDepthf),

		// UNIMPLEMENTED
		EXT(glVertexAttrib1dv),
		EXT(glVertexAttrib1fv),
		EXT(glVertexAttrib1sv),
		EXT(glVertexAttrib2dv),
		EXT(glVertexAttrib2fv),
		EXT(glVertexAttrib2sv),
		EXT(glVertexAttrib3dv),
		EXT(glVertexAttrib3fv),
		EXT(glVertexAttrib3sv),
		EXT(glVertexAttrib4Nbv),
		EXT(glVertexAttrib4Niv),
		EXT(glVertexAttrib4Nsv),
		EXT(glVertexAttrib4Nubv),
		EXT(glVertexAttrib4Nuiv),
		EXT(glVertexAttrib4Nusv),
		EXT(glVertexAttrib4bv),
		EXT(glVertexAttrib4dv),
		EXT(glVertexAttrib4iv),
		EXT(glVertexAttrib4sv),
		EXT(glVertexAttrib4ubv),
		EXT(glVertexAttrib4uiv),
		EXT(glVertexAttrib4usv),
		EXT(glVertexAttrib4fv),
		EXT(glFogCoordfv),
		EXT(glFogCoorddv),
		EXT(glMultiDrawElements),
		EXT(glSecondaryColor3bv),
		EXT(glSecondaryColor3dv),
		EXT(glSecondaryColor3fv),
		EXT(glSecondaryColor3i),
		EXT(glSecondaryColor3iv),
		EXT(glSecondaryColor3s),
		EXT(glSecondaryColor3sv),
		EXT(glSecondaryColor3ubv),
		EXT(glSecondaryColor3ui),
		EXT(glSecondaryColor3uiv),
		EXT(glSecondaryColor3us),
		EXT(glSecondaryColor3usv),
		EXT(glWindowPos2s),
		EXT(glWindowPos2dv),
		EXT(glWindowPos2fv),
		EXT(glWindowPos2iv),
		EXT(glWindowPos2sv),
		EXT(glWindowPos3s),
		EXT(glWindowPos3dv),
		EXT(glWindowPos3fv),
		EXT(glWindowPos3iv),
		EXT(glWindowPos3sv),
		EXT(glMultiTexCoord1dv),
		EXT(glMultiTexCoord1fv),
		EXT(glMultiTexCoord1i),
		EXT(glMultiTexCoord1iv),
		EXT(glMultiTexCoord1s),
		EXT(glMultiTexCoord1sv),
		EXT(glMultiTexCoord2dv),
		EXT(glMultiTexCoord2fv),
		EXT(glMultiTexCoord2i),
		EXT(glMultiTexCoord2iv),
		EXT(glMultiTexCoord2s),
		EXT(glMultiTexCoord2sv),
		EXT(glMultiTexCoord3dv),
		EXT(glMultiTexCoord3fv),
		EXT(glMultiTexCoord3i),
		EXT(glMultiTexCoord3iv),
		EXT(glMultiTexCoord3s),
		EXT(glMultiTexCoord3sv),
		EXT(glMultiTexCoord4dv),
		EXT(glMultiTexCoord4fv),
		EXT(glMultiTexCoord4i),
		EXT(glMultiTexCoord4iv),
		EXT(glMultiTexCoord4s),
		EXT(glMultiTexCoord4sv),
		EXT(glBlendEquationEXT),
		EXT(glBlendFuncSeparateEXT),
		EXT(glSecondaryColor3bEXT),
		EXT(glSecondaryColor3bvEXT),
		EXT(glSecondaryColor3dEXT),
		EXT(glSecondaryColor3dvEXT),
		EXT(glSecondaryColor3fEXT),
		EXT(glSecondaryColor3fvEXT),
		EXT(glSecondaryColor3iEXT),
		EXT(glSecondaryColor3ivEXT),
		EXT(glSecondaryColor3sEXT),
		EXT(glSecondaryColor3svEXT),
		EXT(glSecondaryColor3ubEXT),
		EXT(glSecondaryColor3ubvEXT),
		EXT(glSecondaryColor3uiEXT),
		EXT(glSecondaryColor3uivEXT),
		EXT(glSecondaryColor3usEXT),
		EXT(glSecondaryColor3usvEXT),
		EXT(glSecondaryColorPointerEXT),
		EXT(glBlitFramebufferEXT),
		EXT(glRenderbufferStorageMultisampleEXT),
		EXT(glSecondaryColor3i),
		EXT(glSecondaryColor3s),
		EXT(glSecondaryColor3ui),
		EXT(glSecondaryColor3us),
		EXT(glSecondaryColor3bv),
		EXT(glSecondaryColor3fv),
		EXT(glSecondaryColor3dv),
		EXT(glSecondaryColor3ubv),
		EXT(glSecondaryColor3iv),
		EXT(glSecondaryColor3sv),
		EXT(glSecondaryColor3uiv),
		EXT(glSecondaryColor3usv),
		EXT(glBindAttribLocationARB),
		EXT(glGetActiveAttribARB),
		EXT(glGetAttribLocationARB),
		EXT(glAttachObjectARB),
		EXT(glCompileShaderARB),
		EXT(glCreateProgramObjectARB),
		EXT(glCreateShaderObjectARB),
		EXT(glDeleteObjectARB),
		EXT(glDetachObjectARB),
		EXT(glGetActiveUniformARB),
		EXT(glGetAttachedObjectsARB),
		EXT(glGetHandleARB),
		EXT(glGetInfoLogARB),
		EXT(glGetObjectParameterfvARB),
		EXT(glGetObjectParameterivARB),
		EXT(glGetShaderSourceARB),
		EXT(glGetUniformLocationARB),
		EXT(glGetUniformfvARB),
		EXT(glGetUniformivARB),
		EXT(glLinkProgramARB),
		EXT(glShaderSourceARB),
		EXT(glUniform1fARB),
		EXT(glUniform1fvARB),
		EXT(glUniform1iARB),
		EXT(glUniform1ivARB),
		EXT(glUniform2fARB),
		EXT(glUniform2fvARB),
		EXT(glUniform2iARB),
		EXT(glUniform2ivARB),
		EXT(glUniform3fARB),
		EXT(glUniform3fvARB),
		EXT(glUniform3iARB),
		EXT(glUniform3ivARB),
		EXT(glUniform4fARB),
		EXT(glUniform4fvARB),
		EXT(glUniform4iARB),
		EXT(glUniform4ivARB),
		EXT(glUniformMatrix2fvARB),
		EXT(glUniformMatrix3fvARB),
		EXT(glUniformMatrix4fvARB),
		EXT(glUseProgramObjectARB),
		EXT(glValidateProgramARB),
		EXT(glIsRenderbufferEXT),
		EXT(glBindRenderbufferEXT),
		EXT(glDeleteRenderbuffersEXT),
		EXT(glGenRenderbuffersEXT),
		EXT(glRenderbufferStorageEXT),
		EXT(glGetRenderbufferParameterivEXT),
		EXT(glIsFramebufferEXT),
		EXT(glBindFramebufferEXT),
		EXT(glDeleteFramebuffersEXT),
		EXT(glGenFramebuffersEXT),
		EXT(glCheckFramebufferStatusEXT),
		EXT(glFramebufferTexture1DEXT),
		EXT(glFramebufferTexture2DEXT),
		EXT(glFramebufferTexture3DEXT),
		EXT(glFramebufferRenderbufferEXT),
		EXT(glGetFramebufferAttachmentParameterivEXT),
		EXT(glGenerateMipmapEXT),
		EXT(glColorMaski),
		EXT(glGetBooleani_v),
		EXT(glGetIntegeri_v),
		EXT(glEnablei),
		EXT(glIsEnabledi),
		EXT(glDisablei),
		EXT(glBeginTransformFeedback),
		EXT(glEndTransformFeedback),
		EXT(glBindBufferRange),
		EXT(glBindBufferBase),
		EXT(glTransformFeedbackVaryings),
		EXT(glGetTransformFeedbackVarying),
		EXT(glClampColor),
		EXT(glBeginConditionalRender),
		EXT(glEndConditionalRender),
		EXT(glVertexAttribIPointer),
		EXT(glGetVertexAttribIiv),
		EXT(glGetVertexAttribIuiv),
		EXT(glVertexAttribI2i),
		EXT(glVertexAttribI1i),
		EXT(glVertexAttribI3i),
		EXT(glVertexAttribI4i),
		EXT(glVertexAttribI1ui),
		EXT(glVertexAttribI2ui),
		EXT(glVertexAttribI3ui),
		EXT(glVertexAttribI4ui),
		EXT(glVertexAttribI1iv),
		EXT(glVertexAttribI2iv),
		EXT(glVertexAttribI3iv),
		EXT(glVertexAttribI4iv),
		EXT(glVertexAttribI1uiv),
		EXT(glVertexAttribI2uiv),
		EXT(glVertexAttribI3uiv),
		EXT(glVertexAttribI4uiv),
		EXT(glVertexAttribI4bv),
		EXT(glVertexAttribI4sv),
		EXT(glVertexAttribI4ubv),
		EXT(glVertexAttribI4usv),
		EXT(glGetUniformuiv),
		EXT(glBindFragDataLocation),
		EXT(glGetFragDataLocation),
		EXT(glUniform1ui),
		EXT(glUniform2ui),
		EXT(glUniform3ui),
		EXT(glUniform4ui),
		EXT(glUniform1uiv),
		EXT(glUniform2uiv),
		EXT(glUniform3uiv),
		EXT(glUniform4uiv),
		EXT(glTexParameterIiv),
		EXT(glTexParameterIuiv),
		EXT(glGetTexParameterIiv),
		EXT(glGetTexParameterIuiv),
		EXT(glClearBufferiv),
		EXT(glClearBufferuiv),
		EXT(glClearBufferfv),
		EXT(glClearBufferfi),
		EXT(glGetStringi),
		//EXT(wglCreateContextAttribsARB),
		EXT(glMapNamedBufferEXT),
		EXT(glUnmapNamedBufferEXT),
		EXT(glGetNamedBufferParameterivEXT),
		EXT(glGetTextureImageEXT),
		EXT(glTextureSubImage2DEXT),
		EXT(glTextureSubImage3DEXT),


		EXT(glcuR0d4nX),
		EXT(wgl1fx34c0da),


		//MORE unimplemented()
		EXT(glCopyTexImage1DEXT),
		EXT(glCopyTexImage2DEXT),
		EXT(glCopyTexSubImage1DEXT),
		EXT(glCopyTexSubImage2DEXT),
		EXT(glCopyTexSubImage3DEXT),
		EXT(glPolygonOffsetEXT),
		EXT(glTexSubImage1DEXT),
		EXT(glTexSubImage2DEXT),
		EXT(glTexImage3DEXT),
		EXT(glTexSubImage3DEXT),
		EXT(glAreTexturesResidentEXT),
		EXT(glBindTextureEXT),
		EXT(glDeleteTexturesEXT),
		EXT(glGenTexturesEXT),
		EXT(glIsTextureEXT),
		EXT(glPrioritizeTexturesEXT),
		EXT(glDrawRangeElementsEXT),
		EXT(glSampleCoverageARB),
		EXT(glActiveTextureARB),
		EXT(glClientActiveTextureARB),
		EXT(glMultiTexCoord1dARB),
		EXT(glMultiTexCoord1dvARB),
		EXT(glMultiTexCoord1fARB),
		EXT(glMultiTexCoord1fvARB),
		EXT(glMultiTexCoord1iARB),
		EXT(glMultiTexCoord1ivARB),
		EXT(glMultiTexCoord1sARB),
		EXT(glMultiTexCoord1svARB),
		EXT(glMultiTexCoord2dARB),
		EXT(glMultiTexCoord2dvARB),
		EXT(glMultiTexCoord2fARB),
		EXT(glMultiTexCoord2fvARB),
		EXT(glMultiTexCoord2iARB),
		EXT(glMultiTexCoord2ivARB),
		EXT(glMultiTexCoord2sARB),
		EXT(glMultiTexCoord2svARB),
		EXT(glMultiTexCoord3dARB),
		EXT(glMultiTexCoord3dvARB),
		EXT(glMultiTexCoord3fARB),
		EXT(glMultiTexCoord3fvARB),
		EXT(glMultiTexCoord3iARB),
		EXT(glMultiTexCoord3ivARB),
		EXT(glMultiTexCoord3sARB),
		EXT(glMultiTexCoord3svARB),
		EXT(glMultiTexCoord4dARB),
		EXT(glMultiTexCoord4dvARB),
		EXT(glMultiTexCoord4fARB),
		EXT(glMultiTexCoord4fvARB),
		EXT(glMultiTexCoord4iARB),
		EXT(glMultiTexCoord4ivARB),
		EXT(glMultiTexCoord4sARB),
		EXT(glMultiTexCoord4svARB),
		EXT(glCompressedTexImage3DARB),
		EXT(glCompressedTexImage2DARB),
		EXT(glCompressedTexImage1DARB),
		EXT(glCompressedTexSubImage3DARB),
		EXT(glCompressedTexSubImage2DARB),
		EXT(glCompressedTexSubImage1DARB),
		EXT(glGetCompressedTexImageARB),
		EXT(glPointParameterfARB),
		EXT(glPointParameterfvARB),
		EXT(glBlendColorEXT),
		EXT(glFogCoordfEXT),
		EXT(glFogCoordfvEXT),
		EXT(glFogCoorddEXT),
		EXT(glFogCoorddvEXT),
		EXT(glFogCoordPointerEXT),
		EXT(glMultiDrawArraysEXT),
		EXT(glMultiDrawElementsEXT),
		EXT(glGenQueriesARB),
		EXT(glDeleteQueriesARB),
		EXT(glIsQueryARB),
		EXT(glBeginQueryARB),
		EXT(glEndQueryARB),
		EXT(glGetQueryivARB),
		EXT(glGetQueryObjectivARB),
		EXT(glGetQueryObjectuivARB),
		EXT(glBindBufferARB),
		EXT(glDeleteBuffersARB),
		EXT(glGenBuffersARB),
		EXT(glIsBufferARB),
		EXT(glBufferDataARB),
		EXT(glBufferSubDataARB),
		EXT(glGetBufferSubDataARB),
		EXT(glMapBufferARB),
		EXT(glUnmapBufferARB),
		EXT(glGetBufferParameterivARB),
		EXT(glGetBufferPointervARB),
		EXT(glDrawBuffersARB),
		EXT(glBlendEquationSeparateEXT),
		EXT(glActiveStencilFaceEXT),
		EXT(glClampColorARB),
		EXT(glDrawArraysInstancedARB),
		EXT(glDrawElementsInstancedARB),
		EXT(glProgramParameteriARB),
		EXT(glFramebufferTextureARB),
		EXT(glFramebufferTextureLayerARB),
		EXT(glFramebufferTextureFaceARB),
		EXT(glColorTable),
		EXT(glColorTableParameterfv),
		EXT(glColorTableParameteriv),
		EXT(glCopyColorTable),
		EXT(glGetColorTable),
		EXT(glGetColorTableParameterfv),
		EXT(glGetColorTableParameteriv),
		EXT(glColorSubTable),
		EXT(glCopyColorSubTable),
		EXT(glConvolutionFilter1D),
		EXT(glConvolutionFilter2D),
		EXT(glConvolutionParameterf),
		EXT(glConvolutionParameterfv),
		EXT(glConvolutionParameteri),
		EXT(glConvolutionParameteriv),
		EXT(glCopyConvolutionFilter1D),
		EXT(glCopyConvolutionFilter2D),
		EXT(glGetConvolutionFilter),
		EXT(glGetConvolutionParameterfv),
		EXT(glGetConvolutionParameteriv),
		EXT(glGetSeparableFilter),
		EXT(glSeparableFilter2D),
		EXT(glGetHistogram),
		EXT(glGetHistogramParameterfv),
		EXT(glGetHistogramParameteriv),
		EXT(glGetMinmax),
		EXT(glGetMinmaxParameterfv),
		EXT(glGetMinmaxParameteriv),
		EXT(glHistogram),
		EXT(glMinmax),
		EXT(glResetHistogram),
		EXT(glResetMinmax),
		EXT(glVertexAttribDivisorARB),
		EXT(glMapBufferRange),
		EXT(glFlushMappedBufferRange),
		EXT(glTexBufferARB),
		EXT(glBindVertexArray),
		EXT(glDeleteVertexArrays),
		EXT(glGenVertexArrays),
		EXT(glIsVertexArray),
		EXT(glProgramStringARB),
		EXT(glBindProgramARB),
		EXT(glDeleteProgramsARB),
		EXT(glGenProgramsARB),
		EXT(glProgramEnvParameter4dARB),
		EXT(glProgramEnvParameter4dvARB),
		EXT(glProgramEnvParameter4fARB),
		EXT(glProgramEnvParameter4fvARB),
		EXT(glProgramLocalParameter4dARB),
		EXT(glProgramLocalParameter4dvARB),
		EXT(glProgramLocalParameter4fARB),
		EXT(glProgramLocalParameter4fvARB),
		EXT(glGetProgramEnvParameterdvARB),
		EXT(glGetProgramEnvParameterfvARB),
		EXT(glGetProgramLocalParameterdvARB),
		EXT(glGetProgramLocalParameterfvARB),
		EXT(glGetProgramivARB),
		EXT(glGetProgramStringARB),
		EXT(glIsProgramARB),
		EXT(glEnableVertexAttribArrayARB),
		EXT(glDisableVertexAttribArrayARB),
		EXT(glGetVertexAttribdvARB),
		EXT(glGetVertexAttribfvARB),
		EXT(glGetVertexAttribivARB),
		EXT(glGetVertexAttribPointervARB),
		EXT(glVertexAttrib1dARB),
		EXT(glVertexAttrib1dvARB),
		EXT(glVertexAttrib1fARB),
		EXT(glVertexAttrib1fvARB),
		EXT(glVertexAttrib1sARB),
		EXT(glVertexAttrib1svARB),
		EXT(glVertexAttrib2dARB),
		EXT(glVertexAttrib2dvARB),
		EXT(glVertexAttrib2fARB),
		EXT(glVertexAttrib2fvARB),
		EXT(glVertexAttrib2sARB),
		EXT(glVertexAttrib2svARB),
		EXT(glVertexAttrib3dARB),
		EXT(glVertexAttrib3dvARB),
		EXT(glVertexAttrib3fARB),
		EXT(glVertexAttrib3fvARB),
		EXT(glVertexAttrib3sARB),
		EXT(glVertexAttrib3svARB),
		EXT(glVertexAttrib4NbvARB),
		EXT(glVertexAttrib4NivARB),
		EXT(glVertexAttrib4NsvARB),
		EXT(glVertexAttrib4NubARB),
		EXT(glVertexAttrib4NubvARB),
		EXT(glVertexAttrib4NuivARB),
		EXT(glVertexAttrib4NusvARB),
		EXT(glVertexAttrib4bvARB),
		EXT(glVertexAttrib4dARB),
		EXT(glVertexAttrib4dvARB),
		EXT(glVertexAttrib4fARB),
		EXT(glVertexAttrib4fvARB),
		EXT(glVertexAttrib4ivARB),
		EXT(glVertexAttrib4sARB),
		EXT(glVertexAttrib4svARB),
		EXT(glVertexAttrib4ubvARB),
		EXT(glVertexAttrib4uivARB),
		EXT(glVertexAttrib4usvARB),
		EXT(glVertexAttribPointerARB),
		EXT(glEnableVertexAttribArrayARB),
		EXT(glDisableVertexAttribArrayARB),
		EXT(glWindowPos2dARB),
		EXT(glWindowPos2dvARB),
		EXT(glWindowPos2fARB),
		EXT(glWindowPos2fvARB),
		EXT(glWindowPos2iARB),
		EXT(glWindowPos2ivARB),
		EXT(glWindowPos2sARB),
		EXT(glWindowPos2svARB),
		EXT(glWindowPos3dARB),
		EXT(glWindowPos3dvARB),
		EXT(glWindowPos3fARB),
		EXT(glWindowPos3fvARB),
		EXT(glWindowPos3iARB),
		EXT(glWindowPos3ivARB),
		EXT(glWindowPos3sARB),
		EXT(glWindowPos3svARB),
		EXT(glDrawBuffersATI),
		EXT(glUniformBufferEXT),
		EXT(glGetUniformBufferSizeEXT),
		EXT(glGetUniformOffsetEXT),
		EXT(glLockArraysEXT),
		EXT(glUnlockArraysEXT),
		EXT(glDepthBoundsEXT),
		EXT(glMatrixLoadfEXT),
		EXT(glMatrixLoaddEXT),
		EXT(glMatrixMultfEXT),
		EXT(glMatrixMultdEXT),
		EXT(glMatrixLoadIdentityEXT),
		EXT(glMatrixRotatefEXT),
		EXT(glMatrixRotatedEXT),
		EXT(glMatrixScalefEXT),
		EXT(glMatrixScaledEXT),
		EXT(glMatrixTranslatefEXT),
		EXT(glMatrixTranslatedEXT),
		EXT(glMatrixFrustumEXT),
		EXT(glMatrixOrthoEXT),
		EXT(glMatrixPopEXT),
		EXT(glMatrixPushEXT),
		EXT(glClientAttribDefaultEXT),
		EXT(glPushClientAttribDefaultEXT),
		EXT(glTextureParameterfEXT),
		EXT(glTextureParameterfvEXT),
		EXT(glTextureParameteriEXT),
		EXT(glTextureParameterivEXT),
		EXT(glTextureImage1DEXT),
		EXT(glTextureImage2DEXT),
		EXT(glTextureSubImage1DEXT),
		EXT(glTextureSubImage2DEXT),
		EXT(glCopyTextureImage1DEXT),
		EXT(glCopyTextureImage2DEXT),
		EXT(glCopyTextureSubImage1DEXT),
		EXT(glCopyTextureSubImage2DEXT),
		EXT(glGetTextureImageEXT),
		EXT(glGetTextureParameterfvEXT),
		EXT(glGetTextureParameterivEXT),
		EXT(glGetTextureLevelParameterfvEXT),
		EXT(glGetTextureLevelParameterivEXT),
		EXT(glTextureImage3DEXT),
		EXT(glTextureSubImage3DEXT),
		EXT(glCopyTextureSubImage3DEXT),
		EXT(glBindMultiTextureEXT),
		EXT(glMultiTexCoordPointerEXT),
		EXT(glMultiTexEnvfEXT),
		EXT(glMultiTexEnvfvEXT),
		EXT(glMultiTexEnviEXT),
		EXT(glMultiTexEnvivEXT),
		EXT(glMultiTexGendEXT),
		EXT(glMultiTexGendvEXT),
		EXT(glMultiTexGenfEXT),
		EXT(glMultiTexGenfvEXT),
		EXT(glMultiTexGeniEXT),
		EXT(glMultiTexGenivEXT),
		EXT(glGetMultiTexEnvfvEXT),
		EXT(glGetMultiTexEnvivEXT),
		EXT(glGetMultiTexGendvEXT),
		EXT(glGetMultiTexGenfvEXT),
		EXT(glGetMultiTexGenivEXT),
		EXT(glMultiTexParameteriEXT),
		EXT(glMultiTexParameterivEXT),
		EXT(glMultiTexParameterfEXT),
		EXT(glMultiTexParameterfvEXT),
		EXT(glMultiTexImage1DEXT),
		EXT(glMultiTexImage2DEXT),
		EXT(glMultiTexSubImage1DEXT),
		EXT(glMultiTexSubImage2DEXT),
		EXT(glCopyMultiTexImage1DEXT),
		EXT(glCopyMultiTexImage2DEXT),
		EXT(glCopyMultiTexSubImage1DEXT),
		EXT(glCopyMultiTexSubImage2DEXT),
		EXT(glGetMultiTexImageEXT),
		EXT(glGetMultiTexParameterfvEXT),
		EXT(glGetMultiTexParameterivEXT),
		EXT(glGetMultiTexLevelParameterfvEXT),
		EXT(glGetMultiTexLevelParameterivEXT),
		EXT(glMultiTexImage3DEXT),
		EXT(glMultiTexSubImage3DEXT),
		EXT(glCopyMultiTexSubImage3DEXT),
		EXT(glEnableClientStateIndexedEXT),
		EXT(glDisableClientStateIndexedEXT),
		EXT(glGetFloatIndexedvEXT),
		EXT(glGetDoubleIndexedvEXT),
		EXT(glGetPointerIndexedvEXT),
		EXT(glEnableIndexedEXT),
		EXT(glDisableIndexedEXT),
		EXT(glIsEnabledIndexedEXT),
		EXT(glGetIntegerIndexedvEXT),
		EXT(glGetBooleanIndexedvEXT),
		EXT(glCompressedTextureImage3DEXT),
		EXT(glCompressedTextureImage2DEXT),
		EXT(glCompressedTextureImage1DEXT),
		EXT(glCompressedTextureSubImage3DEXT),
		EXT(glCompressedTextureSubImage2DEXT),
		EXT(glCompressedTextureSubImage1DEXT),
		EXT(glGetCompressedTextureImageEXT),
		EXT(glCompressedMultiTexImage3DEXT),
		EXT(glCompressedMultiTexImage2DEXT),
		EXT(glCompressedMultiTexImage1DEXT),
		EXT(glCompressedMultiTexSubImage3DEXT),
		EXT(glCompressedMultiTexSubImage2DEXT),
		EXT(glCompressedMultiTexSubImage1DEXT),
		EXT(glGetCompressedMultiTexImageEXT),
		EXT(glMatrixLoadTransposefEXT),
		EXT(glMatrixLoadTransposedEXT),
		EXT(glMatrixMultTransposefEXT),
		EXT(glMatrixMultTransposedEXT),
		EXT(glNamedBufferDataEXT),
		EXT(glNamedBufferSubDataEXT),
		EXT(glMapNamedBufferEXT),
		EXT(glUnmapNamedBufferEXT),
		EXT(glGetNamedBufferParameterivEXT),
		EXT(glGetNamedBufferPointervEXT),
		EXT(glGetNamedBufferSubDataEXT),
		EXT(glProgramUniform1fEXT),
		EXT(glProgramUniform2fEXT),
		EXT(glProgramUniform3fEXT),
		EXT(glProgramUniform4fEXT),
		EXT(glProgramUniform1iEXT),
		EXT(glProgramUniform2iEXT),
		EXT(glProgramUniform3iEXT),
		EXT(glProgramUniform4iEXT),
		EXT(glProgramUniform1fvEXT),
		EXT(glProgramUniform2fvEXT),
		EXT(glProgramUniform3fvEXT),
		EXT(glProgramUniform4fvEXT),
		EXT(glProgramUniform1ivEXT),
		EXT(glProgramUniform2ivEXT),
		EXT(glProgramUniform3ivEXT),
		EXT(glProgramUniform4ivEXT),
		EXT(glProgramUniformMatrix2fvEXT),
		EXT(glProgramUniformMatrix3fvEXT),
		EXT(glProgramUniformMatrix4fvEXT),
		EXT(glProgramUniformMatrix2x3fvEXT),
		EXT(glProgramUniformMatrix3x2fvEXT),
		EXT(glProgramUniformMatrix2x4fvEXT),
		EXT(glProgramUniformMatrix4x2fvEXT),
		EXT(glProgramUniformMatrix3x4fvEXT),
		EXT(glProgramUniformMatrix4x3fvEXT),
		EXT(glTextureBufferEXT),
		EXT(glMultiTexBufferEXT),
		EXT(glTextureParameterIivEXT),
		EXT(glTextureParameterIuivEXT),
		EXT(glGetTextureParameterIivEXT),
		EXT(glGetTextureParameterIuivEXT),
		EXT(glMultiTexParameterIivEXT),
		EXT(glMultiTexParameterIuivEXT),
		EXT(glGetMultiTexParameterIivEXT),
		EXT(glGetMultiTexParameterIuivEXT),
		EXT(glProgramUniform1uiEXT),
		EXT(glProgramUniform2uiEXT),
		EXT(glProgramUniform3uiEXT),
		EXT(glProgramUniform4uiEXT),
		EXT(glProgramUniform1uivEXT),
		EXT(glProgramUniform2uivEXT),
		EXT(glProgramUniform3uivEXT),
		EXT(glProgramUniform4uivEXT),
		EXT(glNamedProgramLocalParameters4fvEXT),
		EXT(glNamedProgramLocalParameterI4iEXT),
		EXT(glNamedProgramLocalParameterI4ivEXT),
		EXT(glNamedProgramLocalParametersI4ivEXT),
		EXT(glNamedProgramLocalParameterI4uiEXT),
		EXT(glNamedProgramLocalParameterI4uivEXT),
		EXT(glNamedProgramLocalParametersI4uivEXT),
		EXT(glGetNamedProgramLocalParameterIivEXT),
		EXT(glGetNamedProgramLocalParameterIuivEXT),
		EXT(glEnableClientStateiEXT),
		EXT(glDisableClientStateiEXT),
		EXT(glGetFloati_vEXT),
		EXT(glGetDoublei_vEXT),
		EXT(glGetPointeri_vEXT),
		EXT(glNamedProgramStringEXT),
		EXT(glNamedProgramLocalParameter4dEXT),
		EXT(glNamedProgramLocalParameter4dvEXT),
		EXT(glNamedProgramLocalParameter4fEXT),
		EXT(glNamedProgramLocalParameter4fvEXT),
		EXT(glGetNamedProgramLocalParameterdvEXT),
		EXT(glGetNamedProgramLocalParameterfvEXT),
		EXT(glGetNamedProgramivEXT),
		EXT(glGetNamedProgramStringEXT),
		EXT(glNamedRenderbufferStorageEXT),
		EXT(glGetNamedRenderbufferParameterivEXT),
		EXT(glNamedRenderbufferStorageMultisampleEXT),
		EXT(glNamedRenderbufferStorageMultisampleCoverageEXT),
		EXT(glCheckNamedFramebufferStatusEXT),
		EXT(glNamedFramebufferTexture1DEXT),
		EXT(glNamedFramebufferTexture2DEXT),
		EXT(glNamedFramebufferTexture3DEXT),
		EXT(glNamedFramebufferRenderbufferEXT),
		EXT(glGetNamedFramebufferAttachmentParameterivEXT),
		EXT(glGenerateTextureMipmapEXT),
		EXT(glGenerateMultiTexMipmapEXT),
		EXT(glFramebufferDrawBufferEXT),
		EXT(glFramebufferDrawBuffersEXT),
		EXT(glFramebufferReadBufferEXT),
		EXT(glGetFramebufferParameterivEXT),
		EXT(glNamedCopyBufferSubDataEXT),
		EXT(glNamedFramebufferTextureEXT),
		EXT(glNamedFramebufferTextureLayerEXT),
		EXT(glNamedFramebufferTextureFaceEXT),
		EXT(glTextureRenderbufferEXT),
		EXT(glMultiTexRenderbufferEXT),
		EXT(glVertexArrayVertexOffsetEXT),
		EXT(glVertexArrayColorOffsetEXT),
		EXT(glVertexArrayEdgeFlagOffsetEXT),
		EXT(glVertexArrayIndexOffsetEXT),
		EXT(glVertexArrayNormalOffsetEXT),
		EXT(glVertexArrayTexCoordOffsetEXT),
		EXT(glVertexArrayMultiTexCoordOffsetEXT),
		EXT(glVertexArrayFogCoordOffsetEXT),
		EXT(glVertexArraySecondaryColorOffsetEXT),
		EXT(glVertexArrayVertexAttribOffsetEXT),
		EXT(glVertexArrayVertexAttribIOffsetEXT),
		EXT(glEnableVertexArrayEXT),
		EXT(glDisableVertexArrayEXT),
		EXT(glEnableVertexArrayAttribEXT),
		EXT(glDisableVertexArrayAttribEXT),
		EXT(glGetVertexArrayIntegervEXT),
		EXT(glGetVertexArrayPointervEXT),
		EXT(glGetVertexArrayIntegeri_vEXT),
		EXT(glGetVertexArrayPointeri_vEXT),
		EXT(glMapNamedBufferRangeEXT),
		EXT(glFlushMappedNamedBufferRangeEXT),
		EXT(glNamedBufferStorageEXT),
		EXT(glClearNamedBufferDataEXT),
		EXT(glClearNamedBufferSubDataEXT),
		EXT(glNamedFramebufferParameteriEXT),
		EXT(glGetNamedFramebufferParameterivEXT),
		EXT(glProgramUniform1dEXT),
		EXT(glProgramUniform2dEXT),
		EXT(glProgramUniform3dEXT),
		EXT(glProgramUniform4dEXT),
		EXT(glProgramUniform1dvEXT),
		EXT(glProgramUniform2dvEXT),
		EXT(glProgramUniform3dvEXT),
		EXT(glProgramUniform4dvEXT),
		EXT(glProgramUniformMatrix2dvEXT),
		EXT(glProgramUniformMatrix3dvEXT),
		EXT(glProgramUniformMatrix4dvEXT),
		EXT(glProgramUniformMatrix2x3dvEXT),
		EXT(glProgramUniformMatrix2x4dvEXT),
		EXT(glProgramUniformMatrix3x2dvEXT),
		EXT(glProgramUniformMatrix3x4dvEXT),
		EXT(glProgramUniformMatrix4x2dvEXT),
		EXT(glProgramUniformMatrix4x3dvEXT),
		EXT(glTextureBufferRangeEXT),
		EXT(glTextureStorage1DEXT),
		EXT(glTextureStorage2DEXT),
		EXT(glTextureStorage3DEXT),
		EXT(glTextureStorage2DMultisampleEXT),
		EXT(glTextureStorage3DMultisampleEXT),
		EXT(glVertexArrayBindVertexBufferEXT),
		EXT(glVertexArrayVertexAttribFormatEXT),
		EXT(glVertexArrayVertexAttribIFormatEXT),
		EXT(glVertexArrayVertexAttribLFormatEXT),
		EXT(glVertexArrayVertexAttribBindingEXT),
		EXT(glVertexArrayVertexBindingDivisorEXT),
		EXT(glVertexArrayVertexAttribLOffsetEXT),
		EXT(glTexturePageCommitmentEXT),
		EXT(glVertexArrayVertexAttribDivisorEXT),
		EXT(glColorMaskIndexedEXT),
		EXT(glDrawArraysInstancedEXT),
		EXT(glDrawElementsInstancedEXT),
		EXT(glProgramParameteriEXT),
		EXT(glFramebufferTextureEXT),
		EXT(glFramebufferTextureLayerEXT),
		EXT(glFramebufferTextureFaceEXT),
		EXT(glProgramEnvParameters4fvEXT),
		EXT(glProgramLocalParameters4fvEXT),
		EXT(glGetUniformuivEXT),
		EXT(glBindFragDataLocationEXT),
		EXT(glGetFragDataLocationEXT),
		EXT(glUniform1uiEXT),
		EXT(glUniform2uiEXT),
		EXT(glUniform3uiEXT),
		EXT(glUniform4uiEXT),
		EXT(glUniform1uivEXT),
		EXT(glUniform2uivEXT),
		EXT(glUniform3uivEXT),
		EXT(glUniform4uivEXT),
		EXT(glVertexAttribI1iEXT),
		EXT(glVertexAttribI2iEXT),
		EXT(glVertexAttribI3iEXT),
		EXT(glVertexAttribI4iEXT),
		EXT(glVertexAttribI1uiEXT),
		EXT(glVertexAttribI2uiEXT),
		EXT(glVertexAttribI3uiEXT),
		EXT(glVertexAttribI4uiEXT),
		EXT(glVertexAttribI1ivEXT),
		EXT(glVertexAttribI2ivEXT),
		EXT(glVertexAttribI3ivEXT),
		EXT(glVertexAttribI4ivEXT),
		EXT(glVertexAttribI1uivEXT),
		EXT(glVertexAttribI2uivEXT),
		EXT(glVertexAttribI3uivEXT),
		EXT(glVertexAttribI4uivEXT),
		EXT(glVertexAttribI4bvEXT),
		EXT(glVertexAttribI4svEXT),
		EXT(glVertexAttribI4ubvEXT),
		EXT(glVertexAttribI4usvEXT),
		EXT(glVertexAttribIPointerEXT),
		EXT(glGetVertexAttribIivEXT),
		EXT(glGetVertexAttribIuivEXT),
		EXT(glPointParameterfEXT),
		EXT(glPointParameterfvEXT),
		EXT(glTexBufferEXT),
		EXT(glTexParameterIivEXT),
		EXT(glTexParameterIuivEXT),
		EXT(glGetTexParameterIivEXT),
		EXT(glGetTexParameterIuivEXT),
		EXT(glClearColorIiEXT),
		EXT(glClearColorIuiEXT),
		EXT(glGetQueryObjecti64vEXT),
		EXT(glGetQueryObjectui64vEXT),
		EXT(glArrayElementEXT),
		EXT(glColorPointerEXT),
		EXT(glDrawArraysEXT),
		EXT(glEdgeFlagPointerEXT),
		EXT(glGetPointervEXT),
		EXT(glIndexPointerEXT),
		EXT(glNormalPointerEXT),
		EXT(glTexCoordPointerEXT),
		EXT(glVertexPointerEXT),
		EXT(glBeginConditionalRenderNV),
		EXT(glEndConditionalRenderNV),
		EXT(glDepthRangedNV),
		EXT(glClearDepthdNV),
		EXT(glDepthBoundsdNV),
		EXT(glGetMultisamplefvNV),
		EXT(glSampleMaskIndexedNV),
		EXT(glTexRenderbufferNV),
		EXT(glProgramNamedParameter4fNV),
		EXT(glProgramNamedParameter4fvNV),
		EXT(glProgramNamedParameter4dNV),
		EXT(glProgramNamedParameter4dvNV),
		EXT(glGetProgramNamedParameterfvNV),
		EXT(glGetProgramNamedParameterdvNV),
		EXT(glRenderbufferStorageMultisampleCoverageNV),
		EXT(glProgramLocalParameterI4iNV),
		EXT(glProgramLocalParameterI4ivNV),
		EXT(glProgramLocalParametersI4ivNV),
		EXT(glProgramLocalParameterI4uiNV),
		EXT(glProgramLocalParameterI4uivNV),
		EXT(glProgramLocalParametersI4uivNV),
		EXT(glProgramEnvParameterI4iNV),
		EXT(glProgramEnvParameterI4ivNV),
		EXT(glProgramEnvParametersI4ivNV),
		EXT(glProgramEnvParameterI4uiNV),
		EXT(glProgramEnvParameterI4uivNV),
		EXT(glProgramEnvParametersI4uivNV),
		EXT(glGetProgramLocalParameterIivNV),
		EXT(glGetProgramLocalParameterIuivNV),
		EXT(glGetProgramEnvParameterIivNV),
		EXT(glGetProgramEnvParameterIuivNV),
		EXT(glVertex2hNV),
		EXT(glVertex2hvNV),
		EXT(glVertex3hNV),
		EXT(glVertex3hvNV),
		EXT(glVertex4hNV),
		EXT(glVertex4hvNV),
		EXT(glNormal3hNV),
		EXT(glNormal3hvNV),
		EXT(glColor3hNV),
		EXT(glColor3hvNV),
		EXT(glColor4hNV),
		EXT(glColor4hvNV),
		EXT(glTexCoord1hNV),
		EXT(glTexCoord1hvNV),
		EXT(glTexCoord2hNV),
		EXT(glTexCoord2hvNV),
		EXT(glTexCoord3hNV),
		EXT(glTexCoord3hvNV),
		EXT(glTexCoord4hNV),
		EXT(glTexCoord4hvNV),
		EXT(glMultiTexCoord1hNV),
		EXT(glMultiTexCoord1hvNV),
		EXT(glMultiTexCoord2hNV),
		EXT(glMultiTexCoord2hvNV),
		EXT(glMultiTexCoord3hNV),
		EXT(glMultiTexCoord3hvNV),
		EXT(glMultiTexCoord4hNV),
		EXT(glMultiTexCoord4hvNV),
		EXT(glFogCoordhNV),
		EXT(glFogCoordhvNV),
		EXT(glSecondaryColor3hNV),
		EXT(glSecondaryColor3hvNV),
		EXT(glVertexWeighthNV),
		EXT(glVertexWeighthvNV),
		EXT(glVertexAttrib1hNV),
		EXT(glVertexAttrib1hvNV),
		EXT(glVertexAttrib2hNV),
		EXT(glVertexAttrib2hvNV),
		EXT(glVertexAttrib3hNV),
		EXT(glVertexAttrib3hvNV),
		EXT(glVertexAttrib4hNV),
		EXT(glVertexAttrib4hvNV),
		EXT(glVertexAttribs1hvNV),
		EXT(glVertexAttribs2hvNV),
		EXT(glVertexAttribs3hvNV),
		EXT(glVertexAttribs4hvNV),
		EXT(glBeginOcclusionQueryNV),
		EXT(glEndOcclusionQueryNV),
		EXT(glGetOcclusionQueryivNV),
		EXT(glGetOcclusionQueryuivNV),
		EXT(glGenOcclusionQueriesNV),
		EXT(glDeleteOcclusionQueriesNV),
		EXT(glIsOcclusionQueryNV),
		EXT(glProgramBufferParametersfvNV),
		EXT(glProgramBufferParametersIivNV),
		EXT(glProgramBufferParametersIuivNV),
		EXT(glPixelDataRangeNV),
		EXT(glFlushPixelDataRangeNV),
		EXT(glPointParameteriNV),
		EXT(glPointParameterivNV),
		EXT(glPrimitiveRestartNV),
		EXT(glPrimitiveRestartIndexNV),
		EXT(glCombinerParameterfvNV),
		EXT(glCombinerParameterfNV),
		EXT(glCombinerParameterivNV),
		EXT(glCombinerParameteriNV),
		EXT(glCombinerInputNV),
		EXT(glCombinerOutputNV),
		EXT(glFinalCombinerInputNV),
		EXT(glGetCombinerInputParameterfvNV),
		EXT(glGetCombinerInputParameterivNV),
		EXT(glGetCombinerOutputParameterfvNV),
		EXT(glGetCombinerOutputParameterivNV),
		EXT(glGetFinalCombinerInputParameterfvNV),
		EXT(glGetFinalCombinerInputParameterivNV),
		EXT(glCombinerStageParameterfvNV),
		EXT(glGetCombinerStageParameterfvNV),
		EXT(glMakeBufferResidentNV),
		EXT(glMakeBufferNonResidentNV),
		EXT(glIsBufferResidentNV),
		EXT(glMakeNamedBufferResidentNV),
		EXT(glMakeNamedBufferNonResidentNV),
		EXT(glIsNamedBufferResidentNV),
		EXT(glGetBufferParameterui64vNV),
		EXT(glGetNamedBufferParameterui64vNV),
		EXT(glGetIntegerui64vNV),
		EXT(glUniformui64NV),
		EXT(glUniformui64vNV),
		EXT(glGetUniformui64vNV),
		EXT(glProgramUniformui64NV),
		EXT(glProgramUniformui64vNV),
		EXT(glBeginTransformFeedbackNV),
		EXT(glEndTransformFeedbackNV),
		EXT(glTransformFeedbackAttribsNV),
		EXT(glBindBufferRangeNV),
		EXT(glBindBufferOffsetNV),
		EXT(glBindBufferBaseNV),
		EXT(glTransformFeedbackVaryingsNV),
		EXT(glActiveVaryingNV),
		EXT(glGetVaryingLocationNV),
		EXT(glGetActiveVaryingNV),
		EXT(glGetTransformFeedbackVaryingNV),
		EXT(glTransformFeedbackStreamAttribsNV),
		EXT(glFlushVertexArrayRangeNV),
		EXT(glVertexArrayRangeNV),
		EXT(glBufferAddressRangeNV),
		EXT(glVertexFormatNV),
		EXT(glNormalFormatNV),
		EXT(glColorFormatNV),
		EXT(glIndexFormatNV),
		EXT(glTexCoordFormatNV),
		EXT(glEdgeFlagFormatNV),
		EXT(glSecondaryColorFormatNV),
		EXT(glFogCoordFormatNV),
		EXT(glVertexAttribFormatNV),
		EXT(glVertexAttribIFormatNV),
		EXT(glGetIntegerui64i_vNV),
		EXT(glAreProgramsResidentNV),
		EXT(glBindProgramNV),
		EXT(glDeleteProgramsNV),
		EXT(glExecuteProgramNV),
		EXT(glGenProgramsNV),
		EXT(glGetProgramParameterdvNV),
		EXT(glGetProgramParameterfvNV),
		EXT(glGetProgramivNV),
		EXT(glGetProgramStringNV),
		EXT(glGetTrackMatrixivNV),
		EXT(glGetVertexAttribdvNV),
		EXT(glGetVertexAttribfvNV),
		EXT(glGetVertexAttribivNV),
		EXT(glGetVertexAttribPointervNV),
		EXT(glIsProgramNV),
		EXT(glLoadProgramNV),
		EXT(glProgramParameter4dNV),
		EXT(glProgramParameter4dvNV),
		EXT(glProgramParameter4fNV),
		EXT(glProgramParameter4fvNV),
		EXT(glProgramParameters4dvNV),
		EXT(glProgramParameters4fvNV),
		EXT(glRequestResidentProgramsNV),
		EXT(glTrackMatrixNV),
		EXT(glVertexAttribPointerNV),
		EXT(glVertexAttrib1dNV),
		EXT(glVertexAttrib1dvNV),
		EXT(glVertexAttrib1fNV),
		EXT(glVertexAttrib1fvNV),
		EXT(glVertexAttrib1sNV),
		EXT(glVertexAttrib1svNV),
		EXT(glVertexAttrib2dNV),
		EXT(glVertexAttrib2dvNV),
		EXT(glVertexAttrib2fNV),
		EXT(glVertexAttrib2fvNV),
		EXT(glVertexAttrib2sNV),
		EXT(glVertexAttrib2svNV),
		EXT(glVertexAttrib3dNV),
		EXT(glVertexAttrib3dvNV),
		EXT(glVertexAttrib3fNV),
		EXT(glVertexAttrib3fvNV),
		EXT(glVertexAttrib3sNV),
		EXT(glVertexAttrib3svNV),
		EXT(glVertexAttrib4dNV),
		EXT(glVertexAttrib4dvNV),
		EXT(glVertexAttrib4fNV),
		EXT(glVertexAttrib4fvNV),
		EXT(glVertexAttrib4sNV),
		EXT(glVertexAttrib4svNV),
		EXT(glVertexAttrib4ubNV),
		EXT(glVertexAttrib4ubvNV),
		EXT(glVertexAttribs1dvNV),
		EXT(glVertexAttribs1fvNV),
		EXT(glVertexAttribs1svNV),
		EXT(glVertexAttribs2dvNV),
		EXT(glVertexAttribs2fvNV),
		EXT(glVertexAttribs2svNV),
		EXT(glVertexAttribs3dvNV),
		EXT(glVertexAttribs3fvNV),
		EXT(glVertexAttribs3svNV),
		EXT(glVertexAttribs4dvNV),
		EXT(glVertexAttribs4fvNV),
		EXT(glVertexAttribs4svNV),
		EXT(glVertexAttribs4ubvNV),

		// ARB
		EXT(wglGetExtensionsStringARB),
		EXT(glIsRenderbuffer),
		EXT(glBindRenderbuffer),
		EXT(glDeleteRenderbuffers),
		EXT(glGenRenderbuffers),
		EXT(glRenderbufferStorage),
		EXT(glRenderbufferStorageMultisample),
		EXT(glGetRenderbufferParameteriv),
		EXT(glIsFramebuffer),
		EXT(glBindFramebuffer),
		EXT(glDeleteFramebuffers),
		EXT(glGenFramebuffers),
		EXT(glCheckFramebufferStatus),
		EXT(glFramebufferTexture1D),
		EXT(glFramebufferTexture2D),
		EXT(glFramebufferTexture3D),
		EXT(glFramebufferTextureLayer),
		EXT(glFramebufferRenderbuffer),
		EXT(glGetFramebufferAttachmentParameteriv),
		EXT(glBlitFramebuffer),
		EXT(glGenerateMipmap),

		// EXT
		EXT(wglSwapIntervalEXT),
		EXT(wglGetExtensionsStringEXT),

		//UNIMPLEMENTEDg
		EXT(wglGetSwapIntervalEXT),
#undef EXT
	};

	std::set<const char *> setset;
	for(int ext = 0; ext < sizeof(glExtensions) / sizeof(Extension); ext++)
	{
		int size = setset.size();
		setset.insert(glExtensions[ext].name);
		if(size == setset.size())
		{
			size = size;
		}
	}

	for(int ext = 0; ext < sizeof(glExtensions) / sizeof(Extension); ext++)
	{
		if(strcmp(lpszProc, glExtensions[ext].name) == 0)
		{
			return (PROC)glExtensions[ext].address;
		}
	}

	FARPROC proc = GetProcAddress(GetModuleHandle("opengl32.dll"), lpszProc);  // FIXME?

	if(proc)
	{
		return proc;
	}

	TRACE("(LPCSTR lpszProc = \"%s\") NOT FOUND!!!", lpszProc);

	return (PROC)glIsQueryEXT;
}

BOOL WINAPI wglMakeCurrent(HDC hdc, HGLRC hglrc)
{
	TRACE("(*)");

	if(hdc && hglrc)
	{
		gl::Display *display = (gl::Display*)gl::Display::getDisplay(hdc);
		gl::makeCurrent((gl::Context*)hglrc, display, display->getPrimarySurface());
		gl::setCurrentDrawSurface(display->getPrimarySurface());
		gl::setCurrentDisplay(display);
	}
	else
	{
		gl::makeCurrent(0, 0, 0);
	}

	return TRUE;
}

BOOL WINAPI wglRealizeLayerPalette(HDC, int, BOOL)
{
	UNIMPLEMENTED();
	return FALSE;
}

int WINAPI wglSetLayerPaletteEntries(HDC, int, int, int, CONST COLORREF*)
{
	UNIMPLEMENTED();
	return 0;
}

BOOL WINAPI wglSetPixelFormat(HDC hdc, int iPixelFormat, const PIXELFORMATDESCRIPTOR *ppfd)
{
	TRACE("(*)");
	//UNIMPLEMENTED();

	return TRUE;
}

BOOL WINAPI wglShareLists(HGLRC hglrc1, HGLRC hglrc2)
{
	TRACE("*");
	return TRUE;
}

BOOL WINAPI wglSwapBuffers(HDC hdc)
{
	TRACE("(*)");

	gl::Display *display = gl::getDisplay();

	if(display)
	{
		display->getPrimarySurface()->swap();
		return TRUE;
	}

	return FALSE;
}

BOOL WINAPI wglSwapLayerBuffers(HDC, UINT)
{
	UNIMPLEMENTED();
	return FALSE;
}

DWORD WINAPI wglSwapMultipleBuffers(UINT, CONST WGLSWAP*)
{
	UNIMPLEMENTED();
	return 0;
}

BOOL WINAPI wglUseFontBitmapsA(HDC, DWORD, DWORD, DWORD)
{
	UNIMPLEMENTED();
	return FALSE;
}

BOOL WINAPI wglUseFontBitmapsW(HDC, DWORD, DWORD, DWORD)
{
	UNIMPLEMENTED();
	return FALSE;
}

BOOL WINAPI wglUseFontOutlinesA(HDC, DWORD, DWORD, DWORD, FLOAT, FLOAT, int, LPGLYPHMETRICSFLOAT)
{
	UNIMPLEMENTED();
	return FALSE;
}

BOOL WINAPI wglUseFontOutlinesW(HDC, DWORD, DWORD, DWORD, FLOAT, FLOAT, int, LPGLYPHMETRICSFLOAT)
{
	UNIMPLEMENTED();
	return FALSE;
}

void APIENTRY Register(const char *licenseKey)
{
	RegisterLicenseKey(licenseKey);
}

}