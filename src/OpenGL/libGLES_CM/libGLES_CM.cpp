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
// libGLES_CM.cpp: Implements the exported OpenGL ES 1.1 functions.

#include "main.h"
#include "mathutil.h"
#include "utilities.h"
#include "Buffer.h"
#include "Context.h"
#include "Framebuffer.h"
#include "Renderbuffer.h"
#include "Texture.h"
#include "common/debug.h"
#include "Common/SharedLibrary.hpp"
#include "Common/Version.h"
#include "Main/Register.hpp"

#define EGLAPI
#include <EGL/egl.h>
#include <EGL/eglext.h>

#define GL_API
#include <GLES/gl.h>
#define GL_GLEXT_PROTOTYPES
#include <GLES/glext.h>

#include <exception>
#include <limits>

static bool validImageSize(GLint level, GLsizei width, GLsizei height)
{
	if(level < 0 || level >= es1::IMPLEMENTATION_MAX_TEXTURE_LEVELS || width < 0 || height < 0)
	{
		return false;
	}

	return true;
}

static bool validateSubImageParams(bool compressed, GLsizei width, GLsizei height, GLint xoffset, GLint yoffset, GLenum target, GLint level, GLenum format, es1::Texture *texture)
{
	if(!texture)
	{
		return error(GL_INVALID_OPERATION, false);
	}

	if(compressed != texture->isCompressed(target, level))
	{
		return error(GL_INVALID_OPERATION, false);
	}

	if(format != GL_NONE_OES && format != texture->getFormat(target, level))
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

extern "C"
{

EGLint EGLAPIENTRY eglGetError(void)
{
	static auto eglGetError = (EGLint (EGLAPIENTRY*)(void))getProcAddress(libEGL, "eglGetError");
	return eglGetError();
}

EGLDisplay EGLAPIENTRY eglGetDisplay(EGLNativeDisplayType display_id)
{
	static auto eglGetDisplay = (EGLDisplay (EGLAPIENTRY*)(EGLNativeDisplayType display_id))getProcAddress(libEGL, "eglGetDisplay");
	return eglGetDisplay(display_id);
}

EGLBoolean EGLAPIENTRY eglInitialize(EGLDisplay dpy, EGLint *major, EGLint *minor)
{
	static auto eglInitialize = (EGLBoolean (EGLAPIENTRY*)(EGLDisplay dpy, EGLint *major, EGLint *minor))getProcAddress(libEGL, "eglInitialize");
	return eglInitialize(dpy, major, minor);
}

EGLBoolean EGLAPIENTRY eglTerminate(EGLDisplay dpy)
{
	static auto eglTerminate = (EGLBoolean (EGLAPIENTRY*)(EGLDisplay dpy))getProcAddress(libEGL, "eglTerminate");
	return eglTerminate(dpy);
}

const char *EGLAPIENTRY eglQueryString(EGLDisplay dpy, EGLint name)
{
	static auto eglQueryString = (const char *(EGLAPIENTRY*)(EGLDisplay dpy, EGLint name))getProcAddress(libEGL, "eglQueryString");
	return eglQueryString(dpy, name);
}

EGLBoolean EGLAPIENTRY eglGetConfigs(EGLDisplay dpy, EGLConfig *configs, EGLint config_size, EGLint *num_config)
{
	static auto eglGetConfigs = (EGLBoolean (EGLAPIENTRY*)(EGLDisplay dpy, EGLConfig *configs, EGLint config_size, EGLint *num_config))getProcAddress(libEGL, "eglGetConfigs");
	return eglGetConfigs(dpy, configs, config_size, num_config);
}

EGLBoolean EGLAPIENTRY eglChooseConfig(EGLDisplay dpy, const EGLint *attrib_list, EGLConfig *configs, EGLint config_size, EGLint *num_config)
{
	static auto eglChooseConfig = (EGLBoolean (EGLAPIENTRY*)(EGLDisplay dpy, const EGLint *attrib_list, EGLConfig *configs, EGLint config_size, EGLint *num_config))getProcAddress(libEGL, "eglChooseConfig");
	return eglChooseConfig(dpy, attrib_list, configs, config_size, num_config);
}

EGLBoolean EGLAPIENTRY eglGetConfigAttrib(EGLDisplay dpy, EGLConfig config, EGLint attribute, EGLint *value)
{
	static auto eglGetConfigAttrib = (EGLBoolean (EGLAPIENTRY*)(EGLDisplay dpy, EGLConfig config, EGLint attribute, EGLint *value))getProcAddress(libEGL, "eglGetConfigAttrib");
	return eglGetConfigAttrib(dpy, config, attribute, value);
}

EGLSurface EGLAPIENTRY eglCreateWindowSurface(EGLDisplay dpy, EGLConfig config, EGLNativeWindowType window, const EGLint *attrib_list)
{
	static auto eglCreateWindowSurface = (EGLSurface (EGLAPIENTRY*)(EGLDisplay dpy, EGLConfig config, EGLNativeWindowType window, const EGLint *attrib_list))getProcAddress(libEGL, "eglCreateWindowSurface");
	return eglCreateWindowSurface(dpy, config, window, attrib_list);
}

EGLSurface EGLAPIENTRY eglCreatePbufferSurface(EGLDisplay dpy, EGLConfig config, const EGLint *attrib_list)
{
	static auto eglCreatePbufferSurface = (EGLSurface (EGLAPIENTRY*)(EGLDisplay dpy, EGLConfig config, const EGLint *attrib_list))getProcAddress(libEGL, "eglCreatePbufferSurface");
	return eglCreatePbufferSurface(dpy, config, attrib_list);
}

EGLSurface EGLAPIENTRY eglCreatePixmapSurface(EGLDisplay dpy, EGLConfig config, EGLNativePixmapType pixmap, const EGLint *attrib_list)
{
	static auto eglCreatePixmapSurface = (EGLSurface (EGLAPIENTRY*)(EGLDisplay dpy, EGLConfig config, EGLNativePixmapType pixmap, const EGLint *attrib_list))getProcAddress(libEGL, "eglCreatePixmapSurface");
	return eglCreatePixmapSurface(dpy, config, pixmap, attrib_list);
}

EGLBoolean EGLAPIENTRY eglDestroySurface(EGLDisplay dpy, EGLSurface surface)
{
	static auto eglDestroySurface = (EGLBoolean (EGLAPIENTRY*)(EGLDisplay dpy, EGLSurface surface))getProcAddress(libEGL, "eglDestroySurface");
	return eglDestroySurface(dpy, surface);
}

EGLBoolean EGLAPIENTRY eglQuerySurface(EGLDisplay dpy, EGLSurface surface, EGLint attribute, EGLint *value)
{
	static auto eglQuerySurface = (EGLBoolean (EGLAPIENTRY*)(EGLDisplay dpy, EGLSurface surface, EGLint attribute, EGLint *value))getProcAddress(libEGL, "eglQuerySurface");
	return eglQuerySurface(dpy, surface, attribute, value);
}

EGLBoolean EGLAPIENTRY eglBindAPI(EGLenum api)
{
	static auto eglBindAPI = (EGLBoolean (EGLAPIENTRY*)(EGLenum api))getProcAddress(libEGL, "eglBindAPI");
	return eglBindAPI(api);
}

EGLenum EGLAPIENTRY eglQueryAPI(void)
{
	static auto eglQueryAPI = (EGLenum (EGLAPIENTRY*)(void))getProcAddress(libEGL, "eglQueryAPI");
	return eglQueryAPI();
}

EGLBoolean EGLAPIENTRY eglWaitClient(void)
{
	static auto eglWaitClient = (EGLBoolean (EGLAPIENTRY*)(void))getProcAddress(libEGL, "eglWaitClient");
	return eglWaitClient();
}

EGLBoolean EGLAPIENTRY eglReleaseThread(void)
{
	static auto eglReleaseThread = (EGLBoolean (EGLAPIENTRY*)(void))getProcAddress(libEGL, "eglReleaseThread");
	return eglReleaseThread();
}

EGLSurface EGLAPIENTRY eglCreatePbufferFromClientBuffer(EGLDisplay dpy, EGLenum buftype, EGLClientBuffer buffer, EGLConfig config, const EGLint *attrib_list)
{
	static auto eglCreatePbufferFromClientBuffer = (EGLSurface (EGLAPIENTRY*)(EGLDisplay dpy, EGLenum buftype, EGLClientBuffer buffer, EGLConfig config, const EGLint *attrib_list))getProcAddress(libEGL, "eglCreatePbufferFromClientBuffer");
	return eglCreatePbufferFromClientBuffer(dpy, buftype, buffer, config, attrib_list);
}

EGLBoolean EGLAPIENTRY eglSurfaceAttrib(EGLDisplay dpy, EGLSurface surface, EGLint attribute, EGLint value)
{
	static auto eglSurfaceAttrib = (EGLBoolean (EGLAPIENTRY*)(EGLDisplay dpy, EGLSurface surface, EGLint attribute, EGLint value))getProcAddress(libEGL, "eglSurfaceAttrib");
	return eglSurfaceAttrib(dpy, surface, attribute, value);
}

EGLBoolean EGLAPIENTRY eglBindTexImage(EGLDisplay dpy, EGLSurface surface, EGLint buffer)
{
	static auto eglBindTexImage = (EGLBoolean (EGLAPIENTRY*)(EGLDisplay dpy, EGLSurface surface, EGLint buffer))getProcAddress(libEGL, "eglBindTexImage");
	return eglBindTexImage(dpy, surface, buffer);
}

EGLBoolean EGLAPIENTRY eglReleaseTexImage(EGLDisplay dpy, EGLSurface surface, EGLint buffer)
{
	static auto eglReleaseTexImage = (EGLBoolean (EGLAPIENTRY*)(EGLDisplay dpy, EGLSurface surface, EGLint buffer))getProcAddress(libEGL, "eglReleaseTexImage");
	return eglReleaseTexImage(dpy, surface, buffer);
}

EGLBoolean EGLAPIENTRY eglSwapInterval(EGLDisplay dpy, EGLint interval)
{
	static auto eglSwapInterval = (EGLBoolean (EGLAPIENTRY*)(EGLDisplay dpy, EGLint interval))getProcAddress(libEGL, "eglSwapInterval");
	return eglSwapInterval(dpy, interval);
}

EGLContext EGLAPIENTRY eglCreateContext(EGLDisplay dpy, EGLConfig config, EGLContext share_context, const EGLint *attrib_list)
{
	static auto eglCreateContext = (EGLContext (EGLAPIENTRY*)(EGLDisplay dpy, EGLConfig config, EGLContext share_context, const EGLint *attrib_list))getProcAddress(libEGL, "eglCreateContext");
	return eglCreateContext(dpy, config, share_context, attrib_list);
}

EGLBoolean EGLAPIENTRY eglDestroyContext(EGLDisplay dpy, EGLContext ctx)
{
	static auto eglDestroyContext = (EGLBoolean (EGLAPIENTRY*)(EGLDisplay dpy, EGLContext ctx))getProcAddress(libEGL, "eglDestroyContext");
	return eglDestroyContext(dpy, ctx);
}

EGLBoolean EGLAPIENTRY eglMakeCurrent(EGLDisplay dpy, EGLSurface draw, EGLSurface read, EGLContext ctx)
{
	static auto eglMakeCurrent = (EGLBoolean (EGLAPIENTRY*)(EGLDisplay dpy, EGLSurface draw, EGLSurface read, EGLContext ctx))getProcAddress(libEGL, "eglMakeCurrent");
	return eglMakeCurrent(dpy, draw, read, ctx);
}

EGLContext EGLAPIENTRY eglGetCurrentContext(void)
{
	static auto eglGetCurrentContext = (EGLContext (EGLAPIENTRY*)(void))getProcAddress(libEGL, "eglGetCurrentContext");
	return eglGetCurrentContext();
}

EGLSurface EGLAPIENTRY eglGetCurrentSurface(EGLint readdraw)
{
	static auto eglGetCurrentSurface = (EGLSurface (EGLAPIENTRY*)(EGLint readdraw))getProcAddress(libEGL, "eglGetCurrentSurface");
	return eglGetCurrentSurface(readdraw);
}

EGLDisplay EGLAPIENTRY eglGetCurrentDisplay(void)
{
	static auto eglGetCurrentDisplay = (EGLDisplay (EGLAPIENTRY*)(void))getProcAddress(libEGL, "eglGetCurrentDisplay");
	return eglGetCurrentDisplay();
}

EGLBoolean EGLAPIENTRY eglQueryContext(EGLDisplay dpy, EGLContext ctx, EGLint attribute, EGLint *value)
{
	static auto eglQueryContext = (EGLBoolean (EGLAPIENTRY*)(EGLDisplay dpy, EGLContext ctx, EGLint attribute, EGLint *value))getProcAddress(libEGL, "eglQueryContext");
	return eglQueryContext(dpy, ctx, attribute, value);
}

EGLBoolean EGLAPIENTRY eglWaitGL(void)
{
	static auto eglWaitGL = (EGLBoolean (EGLAPIENTRY*)(void))getProcAddress(libEGL, "eglWaitGL");
	return eglWaitGL();
}

EGLBoolean EGLAPIENTRY eglWaitNative(EGLint engine)
{
	static auto eglWaitNative = (EGLBoolean (EGLAPIENTRY*)(EGLint engine))getProcAddress(libEGL, "eglWaitNative");
	return eglWaitNative(engine);
}

EGLBoolean EGLAPIENTRY eglSwapBuffers(EGLDisplay dpy, EGLSurface surface)
{
	static auto eglSwapBuffers = (EGLBoolean (EGLAPIENTRY*)(EGLDisplay dpy, EGLSurface surface))getProcAddress(libEGL, "eglSwapBuffers");
	return eglSwapBuffers(dpy, surface);
}

EGLBoolean EGLAPIENTRY eglCopyBuffers(EGLDisplay dpy, EGLSurface surface, EGLNativePixmapType target)
{
	static auto eglCopyBuffers = (EGLBoolean (EGLAPIENTRY*)(EGLDisplay dpy, EGLSurface surface, EGLNativePixmapType target))getProcAddress(libEGL, "eglCopyBuffers");
	return eglCopyBuffers(dpy, surface, target);
}

EGLImageKHR EGLAPIENTRY eglCreateImageKHR(EGLDisplay dpy, EGLContext ctx, EGLenum target, EGLClientBuffer buffer, const EGLint *attrib_list)
{
	static auto eglCreateImageKHR = (EGLImageKHR (EGLAPIENTRY*)(EGLDisplay dpy, EGLContext ctx, EGLenum target, EGLClientBuffer buffer, const EGLint *attrib_list))getProcAddress(libEGL, "eglCreateImageKHR");
	return eglCreateImageKHR(dpy, ctx, target, buffer, attrib_list);
}

EGLBoolean EGLAPIENTRY eglDestroyImageKHR(EGLDisplay dpy, EGLImageKHR image)
{
	static auto eglDestroyImageKHR = (EGLBoolean (EGLAPIENTRY*)(EGLDisplay dpy, EGLImageKHR image))getProcAddress(libEGL, "eglDestroyImageKHR");
	return eglDestroyImageKHR(dpy, image);
}

__eglMustCastToProperFunctionPointerType EGLAPIENTRY eglGetProcAddress(const char *procname)
{
	static auto eglGetProcAddress = (__eglMustCastToProperFunctionPointerType (EGLAPIENTRY*)(const char*))getProcAddress(libEGL, "eglGetProcAddress");
	return eglGetProcAddress(procname);
}

void GL_APIENTRY glActiveTexture(GLenum texture)
{
	TRACE("(GLenum texture = 0x%X)", texture);

	es1::Context *context = es1::getContext();

	if(context)
	{
		if(texture < GL_TEXTURE0 || texture > GL_TEXTURE0 + es1::MAX_TEXTURE_UNITS - 1)
		{
			return error(GL_INVALID_ENUM);
		}

		context->setActiveSampler(texture - GL_TEXTURE0);
	}
}

void GL_APIENTRY glAlphaFunc(GLenum func, GLclampf ref)
{
	UNIMPLEMENTED();
}

void GL_APIENTRY glAlphaFuncx(GLenum func, GLclampx ref)
{
	UNIMPLEMENTED();
}

void GL_APIENTRY glBindBuffer(GLenum target, GLuint buffer)
{
	TRACE("(GLenum target = 0x%X, GLuint buffer = %d)", target, buffer);

	es1::Context *context = es1::getContext();

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

void GL_APIENTRY glBindFramebuffer(GLenum target, GLuint framebuffer)
{
	TRACE("(GLenum target = 0x%X, GLuint framebuffer = %d)", target, framebuffer);

	if(target != GL_FRAMEBUFFER_OES)
	{
		return error(GL_INVALID_ENUM);
	}

	es1::Context *context = es1::getContext();

	if(context)
	{
		context->bindFramebuffer(framebuffer);
	}
}

void GL_APIENTRY glBindFramebufferOES(GLenum target, GLuint framebuffer)
{
	TRACE("(GLenum target = 0x%X, GLuint framebuffer = %d)", target, framebuffer);

	if(target != GL_FRAMEBUFFER_OES)
	{
		return error(GL_INVALID_ENUM);
	}

	es1::Context *context = es1::getContext();

	if(context)
	{
		context->bindFramebuffer(framebuffer);
	}
}

void GL_APIENTRY glBindRenderbufferOES(GLenum target, GLuint renderbuffer)
{
	TRACE("(GLenum target = 0x%X, GLuint renderbuffer = %d)", target, renderbuffer);

	if(target != GL_RENDERBUFFER_OES)
	{
		return error(GL_INVALID_ENUM);
	}

	es1::Context *context = es1::getContext();

	if(context)
	{
		if(renderbuffer != 0 && !context->getRenderbuffer(renderbuffer))
		{
			// [OpenGL ES 2.0.25] Section 4.4.3 page 112
			// [OpenGL ES 3.0.2] Section 4.4.2 page 201
			// 'renderbuffer' must be either zero or the name of an existing renderbuffer object of
			// type 'renderbuffertarget', otherwise an INVALID_OPERATION error is generated.
			return error(GL_INVALID_OPERATION);
		}

		context->bindRenderbuffer(renderbuffer);
	}
}

void GL_APIENTRY glBindTexture(GLenum target, GLuint texture)
{
	TRACE("(GLenum target = 0x%X, GLuint texture = %d)", target, texture);

	es1::Context *context = es1::getContext();

	if(context)
	{
		es1::Texture *textureObject = context->getTexture(texture);

		if(textureObject && textureObject->getTarget() != target && texture != 0)
		{
			return error(GL_INVALID_OPERATION);
		}

		switch(target)
		{
		case GL_TEXTURE_2D:
			context->bindTexture2D(texture);
			return;
		case GL_TEXTURE_EXTERNAL_OES:
			context->bindTextureExternal(texture);
			return;
		default:
			return error(GL_INVALID_ENUM);
		}
	}
}

void GL_APIENTRY glBlendEquationSeparateOES(GLenum modeRGB, GLenum modeAlpha);

void GL_APIENTRY glBlendEquationOES(GLenum mode)
{
	glBlendEquationSeparateOES(mode, mode);
}

void GL_APIENTRY glBlendEquationSeparateOES(GLenum modeRGB, GLenum modeAlpha)
{
	TRACE("(GLenum modeRGB = 0x%X, GLenum modeAlpha = 0x%X)", modeRGB, modeAlpha);

	switch(modeRGB)
	{
	case GL_FUNC_ADD_OES:
	case GL_FUNC_SUBTRACT_OES:
	case GL_FUNC_REVERSE_SUBTRACT_OES:
	case GL_MIN_EXT:
	case GL_MAX_EXT:
		break;
	default:
		return error(GL_INVALID_ENUM);
	}

	switch(modeAlpha)
	{
	case GL_FUNC_ADD_OES:
	case GL_FUNC_SUBTRACT_OES:
	case GL_FUNC_REVERSE_SUBTRACT_OES:
	case GL_MIN_EXT:
	case GL_MAX_EXT:
		break;
	default:
		return error(GL_INVALID_ENUM);
	}

	es1::Context *context = es1::getContext();

	if(context)
	{
		context->setBlendEquation(modeRGB, modeAlpha);
	}
}

void GL_APIENTRY glBlendFuncSeparateOES(GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha);

void GL_APIENTRY glBlendFunc(GLenum sfactor, GLenum dfactor)
{
	glBlendFuncSeparateOES(sfactor, dfactor, sfactor, dfactor);
}

void GL_APIENTRY glBlendFuncSeparateOES(GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha)
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
		break;
	default:
		return error(GL_INVALID_ENUM);
	}

	es1::Context *context = es1::getContext();

	if(context)
	{
		context->setBlendFactors(srcRGB, dstRGB, srcAlpha, dstAlpha);
	}
}

void GL_APIENTRY glBufferData(GLenum target, GLsizeiptr size, const GLvoid* data, GLenum usage)
{
	size = static_cast<GLint>(size);   // Work around issues with some 64-bit applications

	TRACE("(GLenum target = 0x%X, GLsizeiptr size = %d, const GLvoid* data = 0x%0.8p, GLenum usage = %d)",
	      target, size, data, usage);

	if(size < 0)
	{
		return error(GL_INVALID_VALUE);
	}

	switch(usage)
	{
	case GL_STATIC_DRAW:
	case GL_DYNAMIC_DRAW:
		break;
	default:
		return error(GL_INVALID_ENUM);
	}

	es1::Context *context = es1::getContext();

	if(context)
	{
		es1::Buffer *buffer;

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

void GL_APIENTRY glBufferSubData(GLenum target, GLintptr offset, GLsizeiptr size, const GLvoid* data)
{
	size = static_cast<GLint>(size);   // Work around issues with some 64-bit applications
	offset = static_cast<GLint>(offset);

	TRACE("(GLenum target = 0x%X, GLintptr offset = %d, GLsizeiptr size = %d, const GLvoid* data = 0x%0.8p)",
	      target, offset, size, data);

	if(size < 0 || offset < 0)
	{
		return error(GL_INVALID_VALUE);
	}

	if(data == NULL)
	{
		return;
	}

	es1::Context *context = es1::getContext();

	if(context)
	{
		es1::Buffer *buffer;

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

GLenum GL_APIENTRY glCheckFramebufferStatusOES(GLenum target)
{
	TRACE("(GLenum target = 0x%X)", target);

	if(target != GL_FRAMEBUFFER_OES)
	{
		return error(GL_INVALID_ENUM, 0);
	}

	es1::Context *context = es1::getContext();

	if(context)
	{
		es1::Framebuffer *framebuffer = context->getFramebuffer();

		return framebuffer->completeness();
	}

	return 0;
}

void GL_APIENTRY glClear(GLbitfield mask)
{
	TRACE("(GLbitfield mask = %X)", mask);

	if((mask & ~(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT)) != 0)
	{
		return error(GL_INVALID_VALUE);
	}

	es1::Context *context = es1::getContext();

	if(context)
	{
		context->clear(mask);
	}
}

void GL_APIENTRY glClearColor(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha)
{
	TRACE("(GLclampf red = %f, GLclampf green = %f, GLclampf blue = %f, GLclampf alpha = %f)",
	      red, green, blue, alpha);

	es1::Context *context = es1::getContext();

	if(context)
	{
		context->setClearColor(red, green, blue, alpha);
	}
}

void GL_APIENTRY glClearColorx(GLclampx red, GLclampx green, GLclampx blue, GLclampx alpha)
{
	UNIMPLEMENTED();
}

void GL_APIENTRY glClearDepthf(GLclampf depth)
{
	TRACE("(GLclampf depth = %f)", depth);

	es1::Context *context = es1::getContext();

	if(context)
	{
		context->setClearDepth(depth);
	}
}

void GL_APIENTRY glClearDepthx(GLclampx depth)
{
	UNIMPLEMENTED();
}

void GL_APIENTRY glClearStencil(GLint s)
{
	TRACE("(GLint s = %d)", s);

	es1::Context *context = es1::getContext();

	if(context)
	{
		context->setClearStencil(s);
	}
}

void GL_APIENTRY glClientActiveTexture(GLenum texture)
{
	TRACE("(GLenum texture = 0x%X)", texture);

	switch(texture)
	{
	case GL_TEXTURE0:
	case GL_TEXTURE1:
		break;
	default:
		return error(GL_INVALID_ENUM);
	}

	es1::Context *context = es1::getContext();

	if(context)
	{
		context->clientActiveTexture(texture);
	}
}

void GL_APIENTRY glClipPlanef(GLenum plane, const GLfloat *equation)
{
	UNIMPLEMENTED();
}

void GL_APIENTRY glClipPlanex(GLenum plane, const GLfixed *equation)
{
	UNIMPLEMENTED();
}

void GL_APIENTRY glColor4f(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha)
{
	UNIMPLEMENTED();
}

void GL_APIENTRY glColor4ub(GLubyte red, GLubyte green, GLubyte blue, GLubyte alpha)
{
	UNIMPLEMENTED();
}

void GL_APIENTRY glColor4x(GLfixed red, GLfixed green, GLfixed blue, GLfixed alpha)
{
	UNIMPLEMENTED();
}

void GL_APIENTRY glColorMask(GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha)
{
	TRACE("(GLboolean red = %d, GLboolean green = %d, GLboolean blue = %d, GLboolean alpha = %d)",
	      red, green, blue, alpha);

	es1::Context *context = es1::getContext();

	if(context)
	{
		context->setColorMask(red == GL_TRUE, green == GL_TRUE, blue == GL_TRUE, alpha == GL_TRUE);
	}
}

void GL_APIENTRY glVertexAttribPointer(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid* ptr)
{
	TRACE("(GLuint index = %d, GLint size = %d, GLenum type = 0x%X, "
	      "GLboolean normalized = %d, GLsizei stride = %d, const GLvoid* ptr = 0x%0.8p)",
	      index, size, type, normalized, stride, ptr);

	if(index >= es1::MAX_VERTEX_ATTRIBS)
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

	es1::Context *context = es1::getContext();

	if(context)
	{
		context->setVertexAttribState(index, context->getArrayBuffer(), size, type, (normalized == GL_TRUE), stride, ptr);
	}
}

void GL_APIENTRY glColorPointer(GLint size, GLenum type, GLsizei stride, const GLvoid *pointer)
{
	UNIMPLEMENTED();
}

void GL_APIENTRY glCompressedTexImage2D(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height,
                                        GLint border, GLsizei imageSize, const GLvoid* data)
{
	TRACE("(GLenum target = 0x%X, GLint level = %d, GLenum internalformat = 0x%X, GLsizei width = %d, "
	      "GLsizei height = %d, GLint border = %d, GLsizei imageSize = %d, const GLvoid* data = 0x%0.8p)",
	      target, level, internalformat, width, height, border, imageSize, data);

	if(!validImageSize(level, width, height) || border != 0 || imageSize < 0)
	{
		return error(GL_INVALID_VALUE);
	}

	switch(internalformat)
	{
	case GL_ETC1_RGB8_OES:
		break;
	case GL_COMPRESSED_RGB_S3TC_DXT1_EXT:
	case GL_COMPRESSED_RGBA_S3TC_DXT1_EXT:
		if(!S3TC_SUPPORT)
		{
			return error(GL_INVALID_ENUM);
		}
		break;
	case GL_DEPTH_COMPONENT16_OES:
	case GL_DEPTH_COMPONENT32_OES:
	case GL_DEPTH_STENCIL_OES:
	case GL_DEPTH24_STENCIL8_OES:
		return error(GL_INVALID_OPERATION);
	default:
		return error(GL_INVALID_ENUM);
	}

	if(border != 0)
	{
		return error(GL_INVALID_VALUE);
	}

	es1::Context *context = es1::getContext();

	if(context)
	{
		if(level > es1::IMPLEMENTATION_MAX_TEXTURE_LEVELS)
		{
			return error(GL_INVALID_VALUE);
		}

		switch(target)
		{
		case GL_TEXTURE_2D:
			if(width > (es1::IMPLEMENTATION_MAX_TEXTURE_SIZE >> level) ||
			   height > (es1::IMPLEMENTATION_MAX_TEXTURE_SIZE >> level))
			{
				return error(GL_INVALID_VALUE);
			}
			break;
		default:
			return error(GL_INVALID_ENUM);
		}

		if(imageSize != es1::ComputeCompressedSize(width, height, internalformat))
		{
			return error(GL_INVALID_VALUE);
		}

		if(target == GL_TEXTURE_2D)
		{
			es1::Texture2D *texture = context->getTexture2D();

			if(!texture)
			{
				return error(GL_INVALID_OPERATION);
			}

			texture->setCompressedImage(level, internalformat, width, height, imageSize, data);
		}
		else UNREACHABLE();
	}
}

void GL_APIENTRY glCompressedTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height,
                                           GLenum format, GLsizei imageSize, const GLvoid* data)
{
	TRACE("(GLenum target = 0x%X, GLint level = %d, GLint xoffset = %d, GLint yoffset = %d, "
	      "GLsizei width = %d, GLsizei height = %d, GLenum format = 0x%X, "
	      "GLsizei imageSize = %d, const GLvoid* data = 0x%0.8p)",
	      target, level, xoffset, yoffset, width, height, format, imageSize, data);

	if(!es1::IsTextureTarget(target))
	{
		return error(GL_INVALID_ENUM);
	}

	if(xoffset < 0 || yoffset < 0 || !validImageSize(level, width, height) || imageSize < 0)
	{
		return error(GL_INVALID_VALUE);
	}

	switch(format)
	{
	case GL_ETC1_RGB8_OES:
		break;
	case GL_COMPRESSED_RGB_S3TC_DXT1_EXT:
	case GL_COMPRESSED_RGBA_S3TC_DXT1_EXT:
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

	es1::Context *context = es1::getContext();

	if(context)
	{
		if(level > es1::IMPLEMENTATION_MAX_TEXTURE_LEVELS)
		{
			return error(GL_INVALID_VALUE);
		}

		if(imageSize != es1::ComputeCompressedSize(width, height, format))
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
			es1::Texture2D *texture = context->getTexture2D();

			if(validateSubImageParams(true, width, height, xoffset, yoffset, target, level, format, texture))
			{
				texture->subImageCompressed(level, xoffset, yoffset, width, height, format, imageSize, data);
			}
		}
		else UNREACHABLE();
	}
}

void GL_APIENTRY glCopyTexImage2D(GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border)
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

	es1::Context *context = es1::getContext();

	if(context)
	{
		switch(target)
		{
		case GL_TEXTURE_2D:
			if(width > (es1::IMPLEMENTATION_MAX_TEXTURE_SIZE >> level) ||
				height > (es1::IMPLEMENTATION_MAX_TEXTURE_SIZE >> level))
			{
				return error(GL_INVALID_VALUE);
			}
			break;
		default:
			return error(GL_INVALID_ENUM);
		}

		es1::Framebuffer *framebuffer = context->getFramebuffer();

		if(framebuffer->completeness() != GL_FRAMEBUFFER_COMPLETE_OES)
		{
			return error(GL_INVALID_FRAMEBUFFER_OPERATION_OES);
		}

		if(context->getFramebufferHandle() != 0 && framebuffer->getColorbuffer()->getSamples() > 1)
		{
			return error(GL_INVALID_OPERATION);
		}

		es1::Renderbuffer *source = framebuffer->getColorbuffer();
		GLenum colorbufferFormat = source->getFormat();

		// [OpenGL ES 2.0.24] table 3.9
		switch(internalformat)
		{
		case GL_ALPHA:
			if(colorbufferFormat != GL_ALPHA &&
			   colorbufferFormat != GL_RGBA &&
			   colorbufferFormat != GL_RGBA4_OES &&
			   colorbufferFormat != GL_RGB5_A1_OES &&
			   colorbufferFormat != GL_RGBA8_OES)
			{
				return error(GL_INVALID_OPERATION);
			}
			break;
		case GL_LUMINANCE:
		case GL_RGB:
			if(colorbufferFormat != GL_RGB &&
			   colorbufferFormat != GL_RGB565_OES &&
			   colorbufferFormat != GL_RGB8_OES &&
			   colorbufferFormat != GL_RGBA &&
			   colorbufferFormat != GL_RGBA4_OES &&
			   colorbufferFormat != GL_RGB5_A1_OES &&
			   colorbufferFormat != GL_RGBA8_OES)
			{
				return error(GL_INVALID_OPERATION);
			}
			break;
		case GL_LUMINANCE_ALPHA:
		case GL_RGBA:
			if(colorbufferFormat != GL_RGBA &&
			   colorbufferFormat != GL_RGBA4_OES &&
			   colorbufferFormat != GL_RGB5_A1_OES &&
			   colorbufferFormat != GL_RGBA8_OES)
			{
				return error(GL_INVALID_OPERATION);
			}
			break;
		case GL_ETC1_RGB8_OES:
			return error(GL_INVALID_OPERATION);
		case GL_COMPRESSED_RGB_S3TC_DXT1_EXT:
		case GL_COMPRESSED_RGBA_S3TC_DXT1_EXT:
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
			es1::Texture2D *texture = context->getTexture2D();

			if(!texture)
			{
				return error(GL_INVALID_OPERATION);
			}

			texture->copyImage(level, internalformat, x, y, width, height, framebuffer);
		}
		else UNREACHABLE();
	}
}

void GL_APIENTRY glCopyTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height)
{
	TRACE("(GLenum target = 0x%X, GLint level = %d, GLint xoffset = %d, GLint yoffset = %d, "
	      "GLint x = %d, GLint y = %d, GLsizei width = %d, GLsizei height = %d)",
	      target, level, xoffset, yoffset, x, y, width, height);

	if(!es1::IsTextureTarget(target))
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

	es1::Context *context = es1::getContext();

	if(context)
	{
		if(level > es1::IMPLEMENTATION_MAX_TEXTURE_LEVELS)
		{
			return error(GL_INVALID_VALUE);
		}

		es1::Framebuffer *framebuffer = context->getFramebuffer();

		if(framebuffer->completeness() != GL_FRAMEBUFFER_COMPLETE_OES)
		{
			return error(GL_INVALID_FRAMEBUFFER_OPERATION_OES);
		}

		if(context->getFramebufferHandle() != 0 && framebuffer->getColorbuffer()->getSamples() > 1)
		{
			return error(GL_INVALID_OPERATION);
		}

		es1::Renderbuffer *source = framebuffer->getColorbuffer();
		GLenum colorbufferFormat = source->getFormat();
		es1::Texture *texture = NULL;

		if(target == GL_TEXTURE_2D)
		{
			texture = context->getTexture2D();
		}
		else UNREACHABLE();

		if(!validateSubImageParams(false, width, height, xoffset, yoffset, target, level, GL_NONE_OES, texture))
		{
			return;
		}

		GLenum textureFormat = texture->getFormat(target, level);

		// [OpenGL ES 2.0.24] table 3.9
		switch(textureFormat)
		{
		case GL_ALPHA:
			if(colorbufferFormat != GL_ALPHA &&
			   colorbufferFormat != GL_RGBA &&
			   colorbufferFormat != GL_RGBA4_OES &&
			   colorbufferFormat != GL_RGB5_A1_OES &&
			   colorbufferFormat != GL_RGBA8_OES)
			{
				return error(GL_INVALID_OPERATION);
			}
			break;
		case GL_LUMINANCE:
		case GL_RGB:
			if(colorbufferFormat != GL_RGB &&
			   colorbufferFormat != GL_RGB565_OES &&
			   colorbufferFormat != GL_RGB8_OES &&
			   colorbufferFormat != GL_RGBA &&
			   colorbufferFormat != GL_RGBA4_OES &&
			   colorbufferFormat != GL_RGB5_A1_OES &&
			   colorbufferFormat != GL_RGBA8_OES)
			{
				return error(GL_INVALID_OPERATION);
			}
			break;
		case GL_LUMINANCE_ALPHA:
		case GL_RGBA:
			if(colorbufferFormat != GL_RGBA &&
			   colorbufferFormat != GL_RGBA4_OES &&
			   colorbufferFormat != GL_RGB5_A1_OES &&
			   colorbufferFormat != GL_RGBA8_OES)
			{
				return error(GL_INVALID_OPERATION);
			}
			break;
		case GL_ETC1_RGB8_OES:
			return error(GL_INVALID_OPERATION);
		case GL_COMPRESSED_RGB_S3TC_DXT1_EXT:
		case GL_COMPRESSED_RGBA_S3TC_DXT1_EXT:
			if(S3TC_SUPPORT)
			{
				return error(GL_INVALID_OPERATION);
			}
			else
			{
				return error(GL_INVALID_ENUM);
			}
		case GL_DEPTH_STENCIL_OES:
			return error(GL_INVALID_OPERATION);
		default:
			return error(GL_INVALID_ENUM);
		}

		texture->copySubImage(target, level, xoffset, yoffset, x, y, width, height, framebuffer);
	}
}

void GL_APIENTRY glCullFace(GLenum mode)
{
	TRACE("(GLenum mode = 0x%X)", mode);

	switch(mode)
	{
	case GL_FRONT:
	case GL_BACK:
	case GL_FRONT_AND_BACK:
		{
			es1::Context *context = es1::getContext();

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

void GL_APIENTRY glDeleteBuffers(GLsizei n, const GLuint* buffers)
{
	TRACE("(GLsizei n = %d, const GLuint* buffers = 0x%0.8p)", n, buffers);

	if(n < 0)
	{
		return error(GL_INVALID_VALUE);
	}

	es1::Context *context = es1::getContext();

	if(context)
	{
		for(int i = 0; i < n; i++)
		{
			context->deleteBuffer(buffers[i]);
		}
	}
}

void GL_APIENTRY glDeleteFramebuffersOES(GLsizei n, const GLuint* framebuffers)
{
	TRACE("(GLsizei n = %d, const GLuint* framebuffers = 0x%0.8p)", n, framebuffers);

	if(n < 0)
	{
		return error(GL_INVALID_VALUE);
	}

	es1::Context *context = es1::getContext();

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

void GL_APIENTRY glDeleteRenderbuffersOES(GLsizei n, const GLuint* renderbuffers)
{
	TRACE("(GLsizei n = %d, const GLuint* renderbuffers = 0x%0.8p)", n, renderbuffers);

	if(n < 0)
	{
		return error(GL_INVALID_VALUE);
	}

	es1::Context *context = es1::getContext();

	if(context)
	{
		for(int i = 0; i < n; i++)
		{
			context->deleteRenderbuffer(renderbuffers[i]);
		}
	}
}

void GL_APIENTRY glDeleteTextures(GLsizei n, const GLuint* textures)
{
	TRACE("(GLsizei n = %d, const GLuint* textures = 0x%0.8p)", n, textures);

	if(n < 0)
	{
		return error(GL_INVALID_VALUE);
	}

	es1::Context *context = es1::getContext();

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

void GL_APIENTRY glDepthFunc(GLenum func)
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

	es1::Context *context = es1::getContext();

	if(context)
	{
		context->setDepthFunc(func);
	}
}

void GL_APIENTRY glDepthMask(GLboolean flag)
{
	TRACE("(GLboolean flag = %d)", flag);

	es1::Context *context = es1::getContext();

	if(context)
	{
		context->setDepthMask(flag != GL_FALSE);
	}
}

void GL_APIENTRY glDepthRangex(GLclampx zNear, GLclampx zFar)
{
	UNIMPLEMENTED();
}

void GL_APIENTRY glDepthRangef(GLclampf zNear, GLclampf zFar)
{
	TRACE("(GLclampf zNear = %f, GLclampf zFar = %f)", zNear, zFar);

	es1::Context *context = es1::getContext();

	if(context)
	{
		context->setDepthRange(zNear, zFar);
	}
}

void GL_APIENTRY glDisable(GLenum cap)
{
	TRACE("(GLenum cap = 0x%X)", cap);

	es1::Context *context = es1::getContext();

	if(context)
	{
		switch(cap)
		{
		case GL_CULL_FACE:                context->setCullFace(false);              break;
		case GL_POLYGON_OFFSET_FILL:      context->setPolygonOffsetFill(false);     break;
		case GL_SAMPLE_ALPHA_TO_COVERAGE: context->setSampleAlphaToCoverage(false); break;
		case GL_SAMPLE_COVERAGE:          context->setSampleCoverage(false);        break;
		case GL_SCISSOR_TEST:             context->setScissorTest(false);           break;
		case GL_STENCIL_TEST:             context->setStencilTest(false);           break;
		case GL_DEPTH_TEST:               context->setDepthTest(false);             break;
		case GL_BLEND:                    context->setBlend(false);                 break;
		case GL_DITHER:                   context->setDither(false);                break;
		case GL_LIGHTING:                 context->setLighting(false);              break;
		case GL_LIGHT0:                   context->setLight(0, false);              break;
		case GL_LIGHT1:                   context->setLight(1, false);              break;
		case GL_LIGHT2:                   context->setLight(2, false);              break;
		case GL_LIGHT3:                   context->setLight(3, false);              break;
		case GL_LIGHT4:                   context->setLight(4, false);              break;
		case GL_LIGHT5:                   context->setLight(5, false);              break;
		case GL_LIGHT6:                   context->setLight(6, false);              break;
		case GL_LIGHT7:                   context->setLight(7, false);              break;
		case GL_FOG:                      context->setFog(false);					break;
		case GL_TEXTURE_2D:               context->setTexture2D(false);             break;
		case GL_ALPHA_TEST:               UNIMPLEMENTED(); break;
		case GL_COLOR_LOGIC_OP:           UNIMPLEMENTED(); break;
		case GL_POINT_SMOOTH:             UNIMPLEMENTED(); break;
		case GL_LINE_SMOOTH:              UNIMPLEMENTED(); break;
		case GL_COLOR_MATERIAL:           UNIMPLEMENTED(); break;
		case GL_NORMALIZE:                UNIMPLEMENTED(); break;
		case GL_RESCALE_NORMAL:           UNIMPLEMENTED(); break;
		case GL_VERTEX_ARRAY:             UNIMPLEMENTED(); break;
		case GL_NORMAL_ARRAY:             UNIMPLEMENTED(); break;
		case GL_COLOR_ARRAY:              UNIMPLEMENTED(); break;
		case GL_TEXTURE_COORD_ARRAY:      UNIMPLEMENTED(); break;
		case GL_MULTISAMPLE:              UNIMPLEMENTED(); break;
		case GL_SAMPLE_ALPHA_TO_ONE:      UNIMPLEMENTED(); break;
		default:
			return error(GL_INVALID_ENUM);
		}
	}
}

void GL_APIENTRY glDisableClientState(GLenum array)
{
	TRACE("(GLenum array = 0x%X)", array);

	es1::Context *context = es1::getContext();

	if (context)
	{
		GLenum texture = context->getClientActiveTexture();

		switch (array)
		{
		case GL_VERTEX_ARRAY:        context->setEnableVertexAttribArray(sw::Position, false);                            break;
		case GL_COLOR_ARRAY:         context->setEnableVertexAttribArray(sw::Color0, false);                              break;
		case GL_TEXTURE_COORD_ARRAY: context->setEnableVertexAttribArray(sw::TexCoord0 + (texture - GL_TEXTURE0), false); break;
		case GL_NORMAL_ARRAY:        context->setEnableVertexAttribArray(sw::Normal, false);                              break;
		default:                     UNIMPLEMENTED();
		}
	}
}

void GL_APIENTRY glDrawArrays(GLenum mode, GLint first, GLsizei count)
{
	TRACE("(GLenum mode = 0x%X, GLint first = %d, GLsizei count = %d)", mode, first, count);

	if(count < 0 || first < 0)
	{
		return error(GL_INVALID_VALUE);
	}

	es1::Context *context = es1::getContext();

	if(context)
	{
		context->drawArrays(mode, first, count);
	}
}

void GL_APIENTRY glDrawElements(GLenum mode, GLsizei count, GLenum type, const GLvoid* indices)
{
	TRACE("(GLenum mode = 0x%X, GLsizei count = %d, GLenum type = 0x%X, const GLvoid* indices = 0x%0.8p)",
	      mode, count, type, indices);

	if(count < 0)
	{
		return error(GL_INVALID_VALUE);
	}

	es1::Context *context = es1::getContext();

	if(context)
	{
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

void GL_APIENTRY glEnable(GLenum cap)
{
	TRACE("(GLenum cap = 0x%X)", cap);

	es1::Context *context = es1::getContext();

	if(context)
	{
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
		case GL_LIGHTING:                 context->setLighting(true);              break;
		case GL_LIGHT0:                   context->setLight(0, true);              break;
		case GL_LIGHT1:                   context->setLight(1, true);              break;
		case GL_LIGHT2:                   context->setLight(2, true);              break;
		case GL_LIGHT3:                   context->setLight(3, true);              break;
		case GL_LIGHT4:                   context->setLight(4, true);              break;
		case GL_LIGHT5:                   context->setLight(5, true);              break;
		case GL_LIGHT6:                   context->setLight(6, true);              break;
		case GL_LIGHT7:                   context->setLight(7, true);              break;
		case GL_FOG:                      context->setFog(true);				   break;
		case GL_TEXTURE_2D:               context->setTexture2D(true);             break;
		case GL_ALPHA_TEST:               UNIMPLEMENTED(); break;
		case GL_COLOR_LOGIC_OP:           UNIMPLEMENTED(); break;
		case GL_POINT_SMOOTH:             UNIMPLEMENTED(); break;
		case GL_LINE_SMOOTH:              UNIMPLEMENTED(); break;
		case GL_COLOR_MATERIAL:           UNIMPLEMENTED(); break;
		case GL_NORMALIZE:                UNIMPLEMENTED(); break;
		case GL_RESCALE_NORMAL:           UNIMPLEMENTED(); break;
		case GL_VERTEX_ARRAY:             UNIMPLEMENTED(); break;
		case GL_NORMAL_ARRAY:             UNIMPLEMENTED(); break;
		case GL_COLOR_ARRAY:              UNIMPLEMENTED(); break;
		case GL_TEXTURE_COORD_ARRAY:      UNIMPLEMENTED(); break;
		case GL_MULTISAMPLE:              UNIMPLEMENTED(); break;
		case GL_SAMPLE_ALPHA_TO_ONE:      UNIMPLEMENTED(); break;
		default:
			return error(GL_INVALID_ENUM);
		}
	}
}

void GL_APIENTRY glEnableClientState(GLenum array)
{
	TRACE("(GLenum array = 0x%X)", array);

	es1::Context *context = es1::getContext();

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

void GL_APIENTRY glFinish(void)
{
	TRACE("()");

	es1::Context *context = es1::getContext();

	if(context)
	{
		context->finish();
	}
}

void GL_APIENTRY glFlush(void)
{
	TRACE("()");

	es1::Context *context = es1::getContext();

	if(context)
	{
		context->flush();
	}
}

void GL_APIENTRY glFramebufferRenderbufferOES(GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer)
{
	TRACE("(GLenum target = 0x%X, GLenum attachment = 0x%X, GLenum renderbuffertarget = 0x%X, "
	      "GLuint renderbuffer = %d)", target, attachment, renderbuffertarget, renderbuffer);

	if(target != GL_FRAMEBUFFER_OES || (renderbuffertarget != GL_RENDERBUFFER_OES && renderbuffer != 0))
	{
		return error(GL_INVALID_ENUM);
	}

	es1::Context *context = es1::getContext();

	if(context)
	{
		es1::Framebuffer *framebuffer = context->getFramebuffer();
		GLuint framebufferHandle = context->getFramebufferHandle();

		if(!framebuffer || (framebufferHandle == 0 && renderbuffer != 0))
		{
			return error(GL_INVALID_OPERATION);
		}

		switch(attachment)
		{
		case GL_COLOR_ATTACHMENT0_OES:
			framebuffer->setColorbuffer(GL_RENDERBUFFER_OES, renderbuffer);
			break;
		case GL_DEPTH_ATTACHMENT_OES:
			framebuffer->setDepthbuffer(GL_RENDERBUFFER_OES, renderbuffer);
			break;
		case GL_STENCIL_ATTACHMENT_OES:
			framebuffer->setStencilbuffer(GL_RENDERBUFFER_OES, renderbuffer);
			break;
		default:
			return error(GL_INVALID_ENUM);
		}
	}
}

void GL_APIENTRY glFramebufferTexture2DOES(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level)
{
	TRACE("(GLenum target = 0x%X, GLenum attachment = 0x%X, GLenum textarget = 0x%X, "
	      "GLuint texture = %d, GLint level = %d)", target, attachment, textarget, texture, level);

	if(target != GL_FRAMEBUFFER_OES)
	{
		return error(GL_INVALID_ENUM);
	}

	switch(attachment)
	{
	case GL_COLOR_ATTACHMENT0_OES:
	case GL_DEPTH_ATTACHMENT_OES:
	case GL_STENCIL_ATTACHMENT_OES:
		break;
	default:
		return error(GL_INVALID_ENUM);
	}

	es1::Context *context = es1::getContext();

	if(context)
	{
		if(texture == 0)
		{
			textarget = GL_NONE_OES;
		}
		else
		{
			es1::Texture *tex = context->getTexture(texture);

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
			default:
				return error(GL_INVALID_ENUM);
			}

			if(level != 0)
			{
				return error(GL_INVALID_VALUE);
			}
		}

		es1::Framebuffer *framebuffer = context->getFramebuffer();
		GLuint framebufferHandle = context->getFramebufferHandle();

		if(framebufferHandle == 0 || !framebuffer)
		{
			return error(GL_INVALID_OPERATION);
		}

		switch(attachment)
		{
		case GL_COLOR_ATTACHMENT0_OES:  framebuffer->setColorbuffer(textarget, texture);   break;
		case GL_DEPTH_ATTACHMENT_OES:   framebuffer->setDepthbuffer(textarget, texture);   break;
		case GL_STENCIL_ATTACHMENT_OES: framebuffer->setStencilbuffer(textarget, texture); break;
		}
	}
}

void GL_APIENTRY glFogf(GLenum pname, GLfloat param)
{
	TRACE("(GLenum pname = 0x%X, GLfloat param = %f)", pname, param);

	es1::Context *context = es1::getContext();
	if(context)
	{
		switch(pname)
		{
		case GL_FOG_MODE:
			switch((GLenum)param)
			{
			case GL_LINEAR:
			case GL_EXP:
			case GL_EXP2:
				context->setFogMode((GLenum)param);
				break;
			default:
				return error(GL_INVALID_ENUM);
			}
			break;

		case GL_FOG_DENSITY:
			if(param < 0)
			{
				return error(GL_INVALID_VALUE);
			}

			context->setFogDensity(param);
			break;

		case GL_FOG_START:
			context->setFogStart(param);
			break;

		case GL_FOG_END:
			context->setFogEnd(param);
			break;

		case GL_FOG_COLOR:
		default:
			return error(GL_INVALID_ENUM);
		}
	}	
}

void GL_APIENTRY glFogfv(GLenum pname, const GLfloat *params)
{
	TRACE("(GLenum pname = 0x%X, const GLfloat *params)", pname);

	es1::Context *context = es1::getContext();

	if(context)
	{
		switch(pname)
		{
		case GL_FOG_MODE:
			switch((GLenum)params[0])
			{
			case GL_LINEAR:
			case GL_EXP:
			case GL_EXP2:
				context->setFogMode((GLenum)params[0]);
				break;
			default:
				return error(GL_INVALID_ENUM);
			}
			break;

		case GL_FOG_DENSITY:
			if(params[0] < 0)
			{
				return error(GL_INVALID_VALUE);
			}

			context->setFogDensity(params[0]);
			break;

		case GL_FOG_START:
			context->setFogStart(params[0]);
			break;

		case GL_FOG_END:
			context->setFogEnd(params[0]);
			break;

		case GL_FOG_COLOR:
			context->setFogColor(params[0], params[1], params[2], params[3]);
			break;

		default:
			return error(GL_INVALID_ENUM);
		}
	}	
}

void GL_APIENTRY glFogx(GLenum pname, GLfixed param)
{
	UNIMPLEMENTED();
}

void GL_APIENTRY glFogxv(GLenum pname, const GLfixed *params)
{
	UNIMPLEMENTED();
}

void GL_APIENTRY glFrontFace(GLenum mode)
{
	TRACE("(GLenum mode = 0x%X)", mode);

	switch(mode)
	{
	case GL_CW:
	case GL_CCW:
		{
			es1::Context *context = es1::getContext();

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

void GL_APIENTRY glFrustumf(GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat zNear, GLfloat zFar)
{
	UNIMPLEMENTED();
}

void GL_APIENTRY glFrustumx(GLfixed left, GLfixed right, GLfixed bottom, GLfixed top, GLfixed zNear, GLfixed zFar)
{
	UNIMPLEMENTED();
}

void GL_APIENTRY glGenerateMipmapOES(GLenum target)
{
	TRACE("(GLenum target = 0x%X)", target);

	es1::Context *context = es1::getContext();

	if(context)
	{
		es1::Texture *texture;

		switch(target)
		{
		case GL_TEXTURE_2D:
			texture = context->getTexture2D();
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

void GL_APIENTRY glGenBuffers(GLsizei n, GLuint* buffers)
{
	TRACE("(GLsizei n = %d, GLuint* buffers = 0x%0.8p)", n, buffers);

	if(n < 0)
	{
		return error(GL_INVALID_VALUE);
	}

	es1::Context *context = es1::getContext();

	if(context)
	{
		for(int i = 0; i < n; i++)
		{
			buffers[i] = context->createBuffer();
		}
	}
}

void GL_APIENTRY glGenFramebuffersOES(GLsizei n, GLuint* framebuffers)
{
	TRACE("(GLsizei n = %d, GLuint* framebuffers = 0x%0.8p)", n, framebuffers);

	if(n < 0)
	{
		return error(GL_INVALID_VALUE);
	}

	es1::Context *context = es1::getContext();

	if(context)
	{
		for(int i = 0; i < n; i++)
		{
			framebuffers[i] = context->createFramebuffer();
		}
	}
}

void GL_APIENTRY glGenRenderbuffersOES(GLsizei n, GLuint* renderbuffers)
{
	TRACE("(GLsizei n = %d, GLuint* renderbuffers = 0x%0.8p)", n, renderbuffers);

	if(n < 0)
	{
		return error(GL_INVALID_VALUE);
	}

	es1::Context *context = es1::getContext();

	if(context)
	{
		for(int i = 0; i < n; i++)
		{
			renderbuffers[i] = context->createRenderbuffer();
		}
	}
}

void GL_APIENTRY glGenTextures(GLsizei n, GLuint* textures)
{
	TRACE("(GLsizei n = %d, GLuint* textures =  0x%0.8p)", n, textures);

	if(n < 0)
	{
		return error(GL_INVALID_VALUE);
	}

	es1::Context *context = es1::getContext();

	if(context)
	{
		for(int i = 0; i < n; i++)
		{
			textures[i] = context->createTexture();
		}
	}
}

void GL_APIENTRY glGetRenderbufferParameterivOES(GLenum target, GLenum pname, GLint* params)
{
	TRACE("(GLenum target = 0x%X, GLenum pname = 0x%X, GLint* params = 0x%0.8p)", target, pname, params);

	es1::Context *context = es1::getContext();

	if(context)
	{
		if(target != GL_RENDERBUFFER_OES)
		{
			return error(GL_INVALID_ENUM);
		}

		if(context->getRenderbufferHandle() == 0)
		{
			return error(GL_INVALID_OPERATION);
		}

		es1::Renderbuffer *renderbuffer = context->getRenderbuffer(context->getRenderbufferHandle());

		switch(pname)
		{
		case GL_RENDERBUFFER_WIDTH_OES:           *params = renderbuffer->getWidth();       break;
		case GL_RENDERBUFFER_HEIGHT_OES:          *params = renderbuffer->getHeight();      break;
		case GL_RENDERBUFFER_INTERNAL_FORMAT_OES: *params = renderbuffer->getFormat();      break;
		case GL_RENDERBUFFER_RED_SIZE_OES:        *params = renderbuffer->getRedSize();     break;
		case GL_RENDERBUFFER_GREEN_SIZE_OES:      *params = renderbuffer->getGreenSize();   break;
		case GL_RENDERBUFFER_BLUE_SIZE_OES:       *params = renderbuffer->getBlueSize();    break;
		case GL_RENDERBUFFER_ALPHA_SIZE_OES:      *params = renderbuffer->getAlphaSize();   break;
		case GL_RENDERBUFFER_DEPTH_SIZE_OES:      *params = renderbuffer->getDepthSize();   break;
		case GL_RENDERBUFFER_STENCIL_SIZE_OES:    *params = renderbuffer->getStencilSize(); break;
		default:
			return error(GL_INVALID_ENUM);
		}
	}
}

void GL_APIENTRY glGetBooleanv(GLenum pname, GLboolean* params)
{
	TRACE("(GLenum pname = 0x%X, GLboolean* params = 0x%0.8p)",  pname, params);

	es1::Context *context = es1::getContext();

	if(context)
	{
		if(!(context->getBooleanv(pname, params)))
		{
			unsigned int numParams = context->getQueryParameterNum(pname);

			if(numParams < 0)
			{
				return error(GL_INVALID_ENUM);
			}

			if(numParams == 0)
			{
				return;
			}

			if(context->isQueryParameterFloat(pname))
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
			else if(context->isQueryParameterInt(pname))
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

void GL_APIENTRY glGetBufferParameteriv(GLenum target, GLenum pname, GLint* params)
{
	TRACE("(GLenum target = 0x%X, GLenum pname = 0x%X, GLint* params = 0x%0.8p)", target, pname, params);

	es1::Context *context = es1::getContext();

	if(context)
	{
		es1::Buffer *buffer;

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

void GL_APIENTRY glGetClipPlanef(GLenum pname, GLfloat eqn[4])
{
	UNIMPLEMENTED();
}

void GL_APIENTRY glGetClipPlanex(GLenum pname, GLfixed eqn[4])
{
	UNIMPLEMENTED();
}

GLenum GL_APIENTRY glGetError(void)
{
	TRACE("()");

es1::Context *context = es1::getContext();

if(context)
{
	return context->getError();
}

return GL_NO_ERROR;
}

void GL_APIENTRY glGetFixedv(GLenum pname, GLfixed *params)
{
	UNIMPLEMENTED();
}

void GL_APIENTRY glGetFloatv(GLenum pname, GLfloat* params)
{
	TRACE("(GLenum pname = 0x%X, GLfloat* params = 0x%0.8p)", pname, params);

	es1::Context *context = es1::getContext();

	if(context)
	{
		if(!(context->getFloatv(pname, params)))
		{
			unsigned int numParams = context->getQueryParameterNum(pname);

			if(numParams < 0)
			{
				return error(GL_INVALID_ENUM);
			}

			if(numParams == 0)
			{
				return;
			}

			if(context->isQueryParameterBool(pname))
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
			else if(context->isQueryParameterInt(pname))
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

void GL_APIENTRY glGetFramebufferAttachmentParameterivOES(GLenum target, GLenum attachment, GLenum pname, GLint* params)
{
	TRACE("(GLenum target = 0x%X, GLenum attachment = 0x%X, GLenum pname = 0x%X, GLint* params = 0x%0.8p)",
	      target, attachment, pname, params);

	es1::Context *context = es1::getContext();

	if(context)
	{
		if(target != GL_FRAMEBUFFER_OES)
		{
			return error(GL_INVALID_ENUM);
		}

		if(context->getFramebufferHandle() == 0)
		{
			return error(GL_INVALID_OPERATION);
		}

		es1::Framebuffer *framebuffer = context->getFramebuffer();

		GLenum attachmentType;
		GLuint attachmentHandle;
		switch(attachment)
		{
		case GL_COLOR_ATTACHMENT0_OES:
			attachmentType = framebuffer->getColorbufferType();
			attachmentHandle = framebuffer->getColorbufferHandle();
			break;
		case GL_DEPTH_ATTACHMENT_OES:
			attachmentType = framebuffer->getDepthbufferType();
			attachmentHandle = framebuffer->getDepthbufferHandle();
			break;
		case GL_STENCIL_ATTACHMENT_OES:
			attachmentType = framebuffer->getStencilbufferType();
			attachmentHandle = framebuffer->getStencilbufferHandle();
			break;
		default:
			return error(GL_INVALID_ENUM);
		}

		GLenum attachmentObjectType;   // Type category
		if(attachmentType == GL_NONE_OES || attachmentType == GL_RENDERBUFFER_OES)
		{
			attachmentObjectType = attachmentType;
		}
		else if(es1::IsTextureTarget(attachmentType))
		{
			attachmentObjectType = GL_TEXTURE;
		}
		else UNREACHABLE();

		switch(pname)
		{
		case GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE_OES:
			*params = attachmentObjectType;
			break;
		case GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME_OES:
			if(attachmentObjectType == GL_RENDERBUFFER_OES || attachmentObjectType == GL_TEXTURE)
			{
				*params = attachmentHandle;
			}
			else
			{
				return error(GL_INVALID_ENUM);
			}
			break;
		case GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LEVEL_OES:
			if(attachmentObjectType == GL_TEXTURE)
			{
				*params = 0; // FramebufferTexture2D will not allow level to be set to anything else in GL ES 2.0
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

void GL_APIENTRY glGetIntegerv(GLenum pname, GLint* params)
{
	TRACE("(GLenum pname = 0x%X, GLint* params = 0x%0.8p)", pname, params);

	es1::Context *context = es1::getContext();

	if(context)
	{
		if(!(context->getIntegerv(pname, params)))
		{
			unsigned int numParams = context->getQueryParameterNum(pname);

			if(numParams < 0)
			{
				return error(GL_INVALID_ENUM);
			}

			if(numParams == 0)
			{
				return;
			}

			if(context->isQueryParameterBool(pname))
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

				delete [] boolParams;
			}
			else if(context->isQueryParameterFloat(pname))
			{
				GLfloat *floatParams = NULL;
				floatParams = new GLfloat[numParams];

				context->getFloatv(pname, floatParams);

				for(unsigned int i = 0; i < numParams; ++i)
				{
					if(pname == GL_DEPTH_RANGE || pname == GL_COLOR_CLEAR_VALUE || pname == GL_DEPTH_CLEAR_VALUE)
					{
						params[i] = (GLint)(((GLfloat)(0xFFFFFFFF) * floatParams[i] - 1.0f) / 2.0f);
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

void GL_APIENTRY glGetLightfv(GLenum light, GLenum pname, GLfloat *params)
{
	UNIMPLEMENTED();
}

void GL_APIENTRY glGetLightxv(GLenum light, GLenum pname, GLfixed *params)
{
	UNIMPLEMENTED();
}

void GL_APIENTRY glGetMaterialfv(GLenum face, GLenum pname, GLfloat *params)
{
	UNIMPLEMENTED();
}

void GL_APIENTRY glGetMaterialxv(GLenum face, GLenum pname, GLfixed *params)
{
	UNIMPLEMENTED();
}

void GL_APIENTRY glGetPointerv(GLenum pname, GLvoid **params)
{
	UNIMPLEMENTED();
}

const GLubyte* GL_APIENTRY glGetString(GLenum name)
{
	TRACE("(GLenum name = 0x%X)", name);

	switch(name)
	{
	case GL_VENDOR:
		return (GLubyte*)"TransGaming Inc.";
	case GL_RENDERER:
		return (GLubyte*)"SwiftShader";
	case GL_VERSION:
		return (GLubyte*)"OpenGL ES 1.1 SwiftShader " VERSION_STRING;
	case GL_EXTENSIONS:
		// Keep list sorted in following order:
		// OES extensions
		// EXT extensions
		// Vendor extensions
		return (GLubyte*)
			"GL_OES_blend_equation_separate "
			"GL_OES_blend_func_separate "
			"GL_OES_blend_subtract "
			"GL_OES_compressed_ETC1_RGB8_texture "
			"GL_OES_depth_texture "
			"GL_OES_EGL_image "
			"GL_OES_EGL_image_external "
			"GL_OES_element_index_uint "
			"GL_OES_framebuffer_object "
			"GL_OES_packed_depth_stencil "
			"GL_OES_rgb8_rgba8 "
			"GL_OES_stencil8 "
			"GL_OES_stencil_wrap "
			"GL_OES_texture_mirrored_repeat "
			"GL_OES_texture_npot "
			"GL_EXT_blend_minmax "
			"GL_EXT_read_format_bgra "
			#if (S3TC_SUPPORT)
			"GL_EXT_texture_compression_dxt1 "
			"GL_ANGLE_texture_compression_dxt3 "
			"GL_ANGLE_texture_compression_dxt5 "
			#endif
			"GL_EXT_texture_filter_anisotropic "
			"GL_EXT_texture_format_BGRA8888";
	default:
		return error(GL_INVALID_ENUM, (GLubyte*)NULL);
	}
}

void GL_APIENTRY glGetTexParameterfv(GLenum target, GLenum pname, GLfloat* params)
{
	TRACE("(GLenum target = 0x%X, GLenum pname = 0x%X, GLfloat* params = 0x%0.8p)", target, pname, params);

	es1::Context *context = es1::getContext();

	if(context)
	{
		es1::Texture *texture;

		switch(target)
		{
		case GL_TEXTURE_2D:
			texture = context->getTexture2D();
			break;
		case GL_TEXTURE_EXTERNAL_OES:
			texture = context->getTextureExternal();
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
		case GL_REQUIRED_TEXTURE_IMAGE_UNITS_OES:
			*params = (GLfloat)1;
			break;
		default:
			return error(GL_INVALID_ENUM);
		}
	}
}

void GL_APIENTRY glGetTexParameteriv(GLenum target, GLenum pname, GLint* params)
{
	TRACE("(GLenum target = 0x%X, GLenum pname = 0x%X, GLint* params = 0x%0.8p)", target, pname, params);

	es1::Context *context = es1::getContext();

	if(context)
	{
		es1::Texture *texture;

		switch(target)
		{
		case GL_TEXTURE_2D:
			texture = context->getTexture2D();
			break;
		case GL_TEXTURE_EXTERNAL_OES:
			texture = context->getTextureExternal();
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
		case GL_REQUIRED_TEXTURE_IMAGE_UNITS_OES:
			*params = 1;
			break;
		default:
			return error(GL_INVALID_ENUM);
		}
	}
}

void GL_APIENTRY glGetTexEnvfv(GLenum env, GLenum pname, GLfloat *params)
{
	UNIMPLEMENTED();
}

void GL_APIENTRY glGetTexEnviv(GLenum env, GLenum pname, GLint *params)
{
	UNIMPLEMENTED();
}

void GL_APIENTRY glGetTexEnvxv(GLenum env, GLenum pname, GLfixed *params)
{
	UNIMPLEMENTED();
}

void GL_APIENTRY glGetTexParameterxv(GLenum target, GLenum pname, GLfixed *params)
{
	UNIMPLEMENTED();
}

void GL_APIENTRY glHint(GLenum target, GLenum mode)
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

	es1::Context *context = es1::getContext();

	if(context)
	{
		switch(target)
		{
		case GL_GENERATE_MIPMAP_HINT:
			context->setGenerateMipmapHint(mode);
			break;
		default:
			return error(GL_INVALID_ENUM);
		}
	}
}

GLboolean GL_APIENTRY glIsBuffer(GLuint buffer)
{
	TRACE("(GLuint buffer = %d)", buffer);

	es1::Context *context = es1::getContext();

	if(context && buffer)
	{
		es1::Buffer *bufferObject = context->getBuffer(buffer);

		if(bufferObject)
		{
			return GL_TRUE;
		}
	}

	return GL_FALSE;
}

GLboolean GL_APIENTRY glIsEnabled(GLenum cap)
{
	TRACE("(GLenum cap = 0x%X)", cap);

	es1::Context *context = es1::getContext();

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
			return error(GL_INVALID_ENUM, GL_FALSE);
		}
	}

	return GL_FALSE;
}

GLboolean GL_APIENTRY glIsFramebufferOES(GLuint framebuffer)
{
	TRACE("(GLuint framebuffer = %d)", framebuffer);

	es1::Context *context = es1::getContext();

	if(context && framebuffer)
	{
		es1::Framebuffer *framebufferObject = context->getFramebuffer(framebuffer);

		if(framebufferObject)
		{
			return GL_TRUE;
		}
	}

	return GL_FALSE;
}

GLboolean GL_APIENTRY glIsTexture(GLuint texture)
{
	TRACE("(GLuint texture = %d)", texture);

	es1::Context *context = es1::getContext();

	if(context && texture)
	{
		es1::Texture *textureObject = context->getTexture(texture);

		if(textureObject)
		{
			return GL_TRUE;
		}
	}

	return GL_FALSE;
}

GLboolean GL_APIENTRY glIsRenderbufferOES(GLuint renderbuffer)
{
	TRACE("(GLuint renderbuffer = %d)", renderbuffer);

	es1::Context *context = es1::getContext();

	if(context && renderbuffer)
	{
		es1::Renderbuffer *renderbufferObject = context->getRenderbuffer(renderbuffer);

		if(renderbufferObject)
		{
			return GL_TRUE;
		}
	}

	return GL_FALSE;
}

void GL_APIENTRY glLightModelf(GLenum pname, GLfloat param)
{
	UNIMPLEMENTED();
}

void GL_APIENTRY glLightModelfv(GLenum pname, const GLfloat *params)
{
	UNIMPLEMENTED();
}

void GL_APIENTRY glLightModelx(GLenum pname, GLfixed param)
{
	UNIMPLEMENTED();
}

void GL_APIENTRY glLightModelxv(GLenum pname, const GLfixed *params)
{
	UNIMPLEMENTED();
}

void GL_APIENTRY glLightf(GLenum light, GLenum pname, GLfloat param)
{
	UNIMPLEMENTED();
}

void GL_APIENTRY glLightfv(GLenum light, GLenum pname, const GLfloat *params)
{
	TRACE("(GLenum light = 0x%X, GLenum pname = 0x%X, const GLint *params)", light, pname);

	es1::Context *context = es1::getContext();

	if(context)
	{
		int index = light - GL_LIGHT0;

		if(index < 0 || index > es1::MAX_LIGHTS)
		{
			return error(GL_INVALID_ENUM);
		}

		switch(pname)
		{
		case GL_AMBIENT:               context->setLightAmbient(index, params[0], params[1], params[2], params[3]);  break;
		case GL_DIFFUSE:               context->setLightDiffuse(index, params[0], params[1], params[2], params[3]);  break;
		case GL_SPECULAR:              context->setLightSpecular(index, params[0], params[1], params[2], params[3]); break;
		case GL_POSITION:              context->setLightPosition(index, params[0], params[1], params[2], params[3]); break;
		case GL_SPOT_DIRECTION:        context->setLightDirection(index, params[0], params[1], params[2]);           break;
		case GL_SPOT_EXPONENT:         UNIMPLEMENTED(); break;
		case GL_SPOT_CUTOFF:           UNIMPLEMENTED(); break;
		case GL_CONSTANT_ATTENUATION:  context->setLightAttenuationConstant(index, params[0]);                       break;
		case GL_LINEAR_ATTENUATION:    context->setLightAttenuationLinear(index, params[0]);                         break;
		case GL_QUADRATIC_ATTENUATION: context->setLightAttenuationQuadratic(index, params[0]);                      break;
		default:
			return error(GL_INVALID_ENUM);
		}
	}
}

void GL_APIENTRY glLightx(GLenum light, GLenum pname, GLfixed param)
{
	UNIMPLEMENTED();
}

void GL_APIENTRY glLightxv(GLenum light, GLenum pname, const GLfixed *params)
{
	UNIMPLEMENTED();
}

void GL_APIENTRY glLineWidth(GLfloat width)
{
	TRACE("(GLfloat width = %f)", width);

	if(width <= 0.0f)
	{
		return error(GL_INVALID_VALUE);
	}

	es1::Context *context = es1::getContext();

	if(context)
	{
		context->setLineWidth(width);
	}
}

void GL_APIENTRY glLineWidthx(GLfixed width)
{
	UNIMPLEMENTED();
}

void GL_APIENTRY glLoadIdentity(void)
{
	TRACE("()");

	es1::Context *context = es1::getContext();

	if(context)
	{
		context->loadIdentity();
	}
}

void GL_APIENTRY glLoadMatrixf(const GLfloat *m)
{
	TRACE("(const GLfloat *m)");

	es1::Context *context = es1::getContext();

	if(context)
	{
		context->load(m);
	}
}

void GL_APIENTRY glLoadMatrixx(const GLfixed *m)
{
	UNIMPLEMENTED();
}

void GL_APIENTRY glLogicOp(GLenum opcode)
{
	UNIMPLEMENTED();
}

void GL_APIENTRY glMaterialf(GLenum face, GLenum pname, GLfloat param)
{
	UNIMPLEMENTED();
}

void GL_APIENTRY glMaterialfv(GLenum face, GLenum pname, const GLfloat *params)
{
	UNIMPLEMENTED();
}

void GL_APIENTRY glMaterialx(GLenum face, GLenum pname, GLfixed param)
{
	UNIMPLEMENTED();
}

void GL_APIENTRY glMaterialxv(GLenum face, GLenum pname, const GLfixed *params)
{
	UNIMPLEMENTED();
}

void GL_APIENTRY glMatrixMode(GLenum mode)
{
	TRACE("(GLenum mode = 0x%X)", mode);

	es1::Context *context = es1::getContext();

	if(context)
	{
		context->setMatrixMode(mode);
	}
}

void GL_APIENTRY glMultMatrixf(const GLfloat *m)
{
	TRACE("(const GLfloat *m)");

	es1::Context *context = es1::getContext();

	if(context)
	{
		context->multiply(m);
	}
}

void GL_APIENTRY glMultMatrixx(const GLfixed *m)
{
	UNIMPLEMENTED();
}

void GL_APIENTRY glMultiTexCoord4f(GLenum target, GLfloat s, GLfloat t, GLfloat r, GLfloat q)
{
	UNIMPLEMENTED();
}

void GL_APIENTRY glMultiTexCoord4x(GLenum target, GLfixed s, GLfixed t, GLfixed r, GLfixed q)
{
	UNIMPLEMENTED();
}

void GL_APIENTRY glNormal3f(GLfloat nx, GLfloat ny, GLfloat nz)
{
	UNIMPLEMENTED();
}

void GL_APIENTRY glNormal3x(GLfixed nx, GLfixed ny, GLfixed nz)
{
	UNIMPLEMENTED();
}

void GL_APIENTRY glNormalPointer(GLenum type, GLsizei stride, const GLvoid *pointer)
{
	TRACE("(GLenum type = 0x%X, GLsizei stride = %d, const GLvoid *pointer = 0x%0.8p)", type, stride, pointer);

glVertexAttribPointer(sw::Normal, 3, type, false, stride, pointer);
}

void GL_APIENTRY glOrthof(GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat zNear, GLfloat zFar)
{
	TRACE("(GLfloat left = %f, GLfloat right = %f, GLfloat bottom = %f, GLfloat top = %f, GLfloat zNear = %f, GLfloat zFar = %f)", left, right, bottom, top, zNear, zFar);

	es1::Context *context = es1::getContext();

	if(context)
	{
		context->ortho(left, right, bottom, top, zNear, zFar);
	}
}

void GL_APIENTRY glOrthox(GLfixed left, GLfixed right, GLfixed bottom, GLfixed top, GLfixed zNear, GLfixed zFar)
{
	UNIMPLEMENTED();
}

void GL_APIENTRY glPixelStorei(GLenum pname, GLint param)
{
	TRACE("(GLenum pname = 0x%X, GLint param = %d)", pname, param);

	es1::Context *context = es1::getContext();

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

void GL_APIENTRY glPointParameterf(GLenum pname, GLfloat param)
{
	UNIMPLEMENTED();
}

void GL_APIENTRY glPointParameterfv(GLenum pname, const GLfloat *params)
{
	UNIMPLEMENTED();
}

void GL_APIENTRY glPointParameterx(GLenum pname, GLfixed param)
{
	UNIMPLEMENTED();
}

void GL_APIENTRY glPointParameterxv(GLenum pname, const GLfixed *params)
{
	UNIMPLEMENTED();
}

void GL_APIENTRY glPointSize(GLfloat size)
{
	UNIMPLEMENTED();
}

void GL_APIENTRY glPointSizePointerOES(GLenum type, GLsizei stride, const GLvoid *pointer)
{
	UNIMPLEMENTED();
}

void GL_APIENTRY glPointSizex(GLfixed size)
{
	UNIMPLEMENTED();
}

void GL_APIENTRY glPolygonOffset(GLfloat factor, GLfloat units)
{
	TRACE("(GLfloat factor = %f, GLfloat units = %f)", factor, units);

	es1::Context *context = es1::getContext();

	if(context)
	{
		context->setPolygonOffsetParams(factor, units);
	}
}

void GL_APIENTRY glPolygonOffsetx(GLfixed factor, GLfixed units)
{
	UNIMPLEMENTED();
}

void GL_APIENTRY glPopMatrix(void)
{
	TRACE("()");

	es1::Context *context = es1::getContext();

	if(context)
	{
		context->popMatrix();
	}
}

void GL_APIENTRY glPushMatrix(void)
{
	TRACE("()");

	es1::Context *context = es1::getContext();

	if(context)
	{
		context->pushMatrix();
	}
}

void GL_APIENTRY glReadPixels(GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid* pixels)
{
	TRACE("(GLint x = %d, GLint y = %d, GLsizei width = %d, GLsizei height = %d, "
	      "GLenum format = 0x%X, GLenum type = 0x%X, GLvoid* pixels = 0x%0.8p)",
	      x, y, width, height, format, type,  pixels);

	if(width < 0 || height < 0)
	{
		return error(GL_INVALID_VALUE);
	}
	
	es1::Context *context = es1::getContext();

	if(context)
	{
		context->readPixels(x, y, width, height, format, type, NULL, pixels);
	}
}

void GL_APIENTRY glRenderbufferStorageOES(GLenum target, GLenum internalformat, GLsizei width, GLsizei height)
{
	TRACE("(GLenum target = 0x%X, GLenum internalformat = 0x%X, GLsizei width = %d, GLsizei height = %d)",
	      target, internalformat, width, height);

	switch(target)
	{
	case GL_RENDERBUFFER_OES:
		break;
	default:
		return error(GL_INVALID_ENUM);
	}

	if(!es1::IsColorRenderable(internalformat) && !es1::IsDepthRenderable(internalformat) && !es1::IsStencilRenderable(internalformat))
	{
		return error(GL_INVALID_ENUM);
	}

	if(width < 0 || height < 0)
	{
		return error(GL_INVALID_VALUE);
	}

	es1::Context *context = es1::getContext();

	if(context)
	{
		if(width > es1::IMPLEMENTATION_MAX_RENDERBUFFER_SIZE ||
		   height > es1::IMPLEMENTATION_MAX_RENDERBUFFER_SIZE)
		{
			return error(GL_INVALID_VALUE);
		}

		GLuint handle = context->getRenderbufferHandle();
		if(handle == 0)
		{
			return error(GL_INVALID_OPERATION);
		}

		switch(internalformat)
		{
		case GL_DEPTH_COMPONENT16_OES:
			context->setRenderbufferStorage(new es1::Depthbuffer(width, height, 0));
			break;
		case GL_RGBA4_OES:
		case GL_RGB5_A1_OES:
		case GL_RGB565_OES:
		case GL_RGB8_OES:
		case GL_RGBA8_OES:
			context->setRenderbufferStorage(new es1::Colorbuffer(width, height, internalformat, 0));
			break;
		case GL_STENCIL_INDEX8_OES:
			context->setRenderbufferStorage(new es1::Stencilbuffer(width, height, 0));
			break;
		case GL_DEPTH24_STENCIL8_OES:
			context->setRenderbufferStorage(new es1::DepthStencilbuffer(width, height, 0));
			break;
		default:
			return error(GL_INVALID_ENUM);
		}
	}
}

void GL_APIENTRY glRotatef(GLfloat angle, GLfloat x, GLfloat y, GLfloat z)
{
	TRACE("(GLfloat angle = %f, GLfloat x = %f, GLfloat y = %f, GLfloat z = %f)", angle, x, y, z);

	es1::Context *context = es1::getContext();

	if(context)
	{
		context->rotate(angle, x, y, z);
	}
}

void GL_APIENTRY glRotatex(GLfixed angle, GLfixed x, GLfixed y, GLfixed z)
{
	UNIMPLEMENTED();
}

void GL_APIENTRY glSampleCoverage(GLclampf value, GLboolean invert)
{
	TRACE("(GLclampf value = %f, GLboolean invert = %d)", value, invert);

	es1::Context* context = es1::getContext();

	if(context)
	{
		context->setSampleCoverageParams(es1::clamp01(value), invert == GL_TRUE);
	}
}

void GL_APIENTRY glSampleCoveragex(GLclampx value, GLboolean invert)
{
	UNIMPLEMENTED();
}

void GL_APIENTRY glScalef(GLfloat x, GLfloat y, GLfloat z)
{
	TRACE("(GLfloat x = %f, GLfloat y = %f, GLfloat z = %f)", x, y, z);

	es1::Context *context = es1::getContext();

	if(context)
	{
		context->scale(x, y, z);
	}
}

void GL_APIENTRY glScalex(GLfixed x, GLfixed y, GLfixed z)
{
	UNIMPLEMENTED();
}

void GL_APIENTRY glScissor(GLint x, GLint y, GLsizei width, GLsizei height)
{
	TRACE("(GLint x = %d, GLint y = %d, GLsizei width = %d, GLsizei height = %d)", x, y, width, height);

	if(width < 0 || height < 0)
	{
		return error(GL_INVALID_VALUE);
	}

	es1::Context* context = es1::getContext();

	if(context)
	{
		context->setScissorParams(x, y, width, height);
	}
}

void GL_APIENTRY glShadeModel(GLenum mode)
{
	UNIMPLEMENTED();
}

void GL_APIENTRY glStencilFunc(GLenum func, GLint ref, GLuint mask)
{
	TRACE("(GLenum func = 0x%X, GLint ref = %d, GLuint mask = %d)",  func, ref, mask);

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

	es1::Context *context = es1::getContext();

	if(context)
	{
		context->setStencilParams(func, ref, mask);
	}
}

void GL_APIENTRY glStencilMask(GLuint mask)
{
	TRACE("(GLuint mask = %d)", mask);

	es1::Context *context = es1::getContext();

	if(context)
	{
		context->setStencilWritemask(mask);
	}
}

void GL_APIENTRY glStencilOp(GLenum fail, GLenum zfail, GLenum zpass)
{
	TRACE("(GLenum fail = 0x%X, GLenum zfail = 0x%X, GLenum zpas = 0x%Xs)", fail, zfail, zpass);

	switch(fail)
	{
	case GL_ZERO:
	case GL_KEEP:
	case GL_REPLACE:
	case GL_INCR:
	case GL_DECR:
	case GL_INVERT:
	case GL_INCR_WRAP_OES:
	case GL_DECR_WRAP_OES:
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
	case GL_INCR_WRAP_OES:
	case GL_DECR_WRAP_OES:
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
	case GL_INCR_WRAP_OES:
	case GL_DECR_WRAP_OES:
		break;
	default:
		return error(GL_INVALID_ENUM);
	}

	es1::Context *context = es1::getContext();

	if(context)
	{
		context->setStencilOperations(fail, zfail, zpass);
	}
}

void GL_APIENTRY glTexCoordPointer(GLint size, GLenum type, GLsizei stride, const GLvoid *pointer)
{
	TRACE("(GLint size = %d, GLenum type = 0x%X, GLsizei stride = %d, const GLvoid *pointer = 0x%0.8p)", size, type, stride, pointer);

	es1::Context *context = es1::getContext();

	if(context)
	{
		GLenum texture = context->getClientActiveTexture();

		glVertexAttribPointer(sw::TexCoord0 + (texture - GL_TEXTURE0), size, type, false, stride, pointer);
	}
}

void GL_APIENTRY glTexEnvf(GLenum target, GLenum pname, GLfloat param)
{
	UNIMPLEMENTED();
}

void GL_APIENTRY glTexEnvfv(GLenum target, GLenum pname, const GLfloat *params)
{
	UNIMPLEMENTED();
}

void GL_APIENTRY glTexEnvi(GLenum target, GLenum pname, GLint param)
{
	UNIMPLEMENTED();
}

void GL_APIENTRY glTexEnvx(GLenum target, GLenum pname, GLfixed param)
{
	UNIMPLEMENTED();
}

void GL_APIENTRY glTexEnviv(GLenum target, GLenum pname, const GLint *params)
{
	UNIMPLEMENTED();
}

void GL_APIENTRY glTexEnvxv(GLenum target, GLenum pname, const GLfixed *params)
{
	UNIMPLEMENTED();
}

void GL_APIENTRY glTexImage2D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height,
                              GLint border, GLenum format, GLenum type, const GLvoid* pixels)
{
	TRACE("(GLenum target = 0x%X, GLint level = %d, GLint internalformat = %d, GLsizei width = %d, GLsizei height = %d, "
	      "GLint border = %d, GLenum format = 0x%X, GLenum type = 0x%X, const GLvoid* pixels =  0x%0.8p)",
	      target, level, internalformat, width, height, border, format, type, pixels);

	if(!validImageSize(level, width, height))
	{
		return error(GL_INVALID_VALUE);
	}

	if(internalformat != format)
	{
		return error(GL_INVALID_OPERATION);
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
			break;
		default:
			return error(GL_INVALID_ENUM);
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
	case GL_ETC1_RGB8_OES:
		return error(GL_INVALID_OPERATION);
	case GL_COMPRESSED_RGB_S3TC_DXT1_EXT:
	case GL_COMPRESSED_RGBA_S3TC_DXT1_EXT:
		if(S3TC_SUPPORT)
		{
			return error(GL_INVALID_OPERATION);
		}
		else
		{
			return error(GL_INVALID_ENUM);
		}
	case GL_DEPTH_STENCIL_OES:
		switch(type)
		{
		case GL_UNSIGNED_INT_24_8_OES:
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

	es1::Context *context = es1::getContext();

	if(context)
	{
		switch(target)
		{
		case GL_TEXTURE_2D:
			if(width > (es1::IMPLEMENTATION_MAX_TEXTURE_SIZE >> level) ||
			   height > (es1::IMPLEMENTATION_MAX_TEXTURE_SIZE >> level))
			{
				return error(GL_INVALID_VALUE);
			}
			break;
		default:
			return error(GL_INVALID_ENUM);
		}

		if(target == GL_TEXTURE_2D)
		{
			es1::Texture2D *texture = context->getTexture2D();

			if(!texture)
			{
				return error(GL_INVALID_OPERATION);
			}

			texture->setImage(level, width, height, format, type, context->getUnpackAlignment(), pixels);
		}
		else UNREACHABLE();
	}
}

void GL_APIENTRY glTexParameterf(GLenum target, GLenum pname, GLfloat param)
{
	TRACE("(GLenum target = 0x%X, GLenum pname = 0x%X, GLfloat param = %f)", target, pname, param);

	es1::Context *context = es1::getContext();

	if(context)
	{
		es1::Texture *texture;

		switch(target)
		{
		case GL_TEXTURE_2D:
			texture = context->getTexture2D();
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
		default:
			return error(GL_INVALID_ENUM);
		}
	}
}

void GL_APIENTRY glTexParameterfv(GLenum target, GLenum pname, const GLfloat* params)
{
	glTexParameterf(target, pname, *params);
}

void GL_APIENTRY glTexParameteri(GLenum target, GLenum pname, GLint param)
{
	TRACE("(GLenum target = 0x%X, GLenum pname = 0x%X, GLint param = %d)", target, pname, param);

	es1::Context *context = es1::getContext();

	if(context)
	{
		es1::Texture *texture;

		switch(target)
		{
		case GL_TEXTURE_2D:
			texture = context->getTexture2D();
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
		case GL_TEXTURE_CROP_RECT_OES:
			UNIMPLEMENTED();
			break;
		default:
			return error(GL_INVALID_ENUM);
		}
	}
}

void GL_APIENTRY glTexParameteriv(GLenum target, GLenum pname, const GLint* params)
{
	glTexParameteri(target, pname, *params);
}

void GL_APIENTRY glTexParameterx(GLenum target, GLenum pname, GLfixed param)
{
	UNIMPLEMENTED();
}

void GL_APIENTRY glTexParameterxv(GLenum target, GLenum pname, const GLfixed *params)
{
	UNIMPLEMENTED();
}

void GL_APIENTRY glTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height,
                                 GLenum format, GLenum type, const GLvoid* pixels)
{
	TRACE("(GLenum target = 0x%X, GLint level = %d, GLint xoffset = %d, GLint yoffset = %d, "
	      "GLsizei width = %d, GLsizei height = %d, GLenum format = 0x%X, GLenum type = 0x%X, "
	      "const GLvoid* pixels = 0x%0.8p)",
	      target, level, xoffset, yoffset, width, height, format, type, pixels);

	if(!es1::IsTextureTarget(target))
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

	if(!es1::CheckTextureFormatType(format, type))
	{
		return error(GL_INVALID_ENUM);
	}

	if(width == 0 || height == 0 || pixels == NULL)
	{
		return;
	}

	es1::Context *context = es1::getContext();

	if(context)
	{
		if(level > es1::IMPLEMENTATION_MAX_TEXTURE_LEVELS)
		{
			return error(GL_INVALID_VALUE);
		}

		if(target == GL_TEXTURE_2D)
		{
			es1::Texture2D *texture = context->getTexture2D();

			if(validateSubImageParams(false, width, height, xoffset, yoffset, target, level, format, texture))
			{
				texture->subImage(level, xoffset, yoffset, width, height, format, type, context->getUnpackAlignment(), pixels);
			}
		}
		else UNREACHABLE();
	}
}

void GL_APIENTRY glTranslatef(GLfloat x, GLfloat y, GLfloat z)
{
	TRACE("(GLfloat x = %f, GLfloat y = %f, GLfloat z = %f)", x, y, z);

	es1::Context *context = es1::getContext();

	if(context)
	{
		context->translate(x, y, z);
	}
}

void GL_APIENTRY glTranslatex(GLfixed x, GLfixed y, GLfixed z)
{
	UNIMPLEMENTED();
}

void GL_APIENTRY glVertexPointer(GLint size, GLenum type, GLsizei stride, const GLvoid *pointer)
{
	TRACE("(GLint size = %d, GLenum type = 0x%X, GLsizei stride = %d, const GLvoid *pointer = 0x%0.8p)", size, type, stride, pointer);

	glVertexAttribPointer(sw::Position, size, type, false, stride, pointer);
}

void GL_APIENTRY glViewport(GLint x, GLint y, GLsizei width, GLsizei height)
{
	TRACE("(GLint x = %d, GLint y = %d, GLsizei width = %d, GLsizei height = %d)", x, y, width, height);

	if(width < 0 || height < 0)
	{
		return error(GL_INVALID_VALUE);
	}

	es1::Context *context = es1::getContext();

	if(context)
	{
		context->setViewportParams(x, y, width, height);
	}
}

void GL_APIENTRY glEGLImageTargetTexture2DOES(GLenum target, GLeglImageOES image)
{
	TRACE("(GLenum target = 0x%X, GLeglImageOES image = 0x%0.8p)", target, image);

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

	es1::Context *context = es1::getContext();

	if(context)
	{
		es1::Texture2D *texture = 0;

		switch(target)
		{
		case GL_TEXTURE_2D:           texture = context->getTexture2D();       break;
		case GL_TEXTURE_EXTERNAL_OES: texture = context->getTextureExternal(); break;
		default:                      UNREACHABLE();
		}

		if(!texture)
		{
			return error(GL_INVALID_OPERATION);
		}

		egl::Image *glImage = static_cast<egl::Image*>(image);

		texture->setImage(glImage);
	}
}

void GL_APIENTRY glEGLImageTargetRenderbufferStorageOES(GLenum target, GLeglImageOES image)
{
	TRACE("(GLenum target = 0x%X, GLeglImageOES image = 0x%0.8p)", target, image);

	UNIMPLEMENTED();
}

void GL_APIENTRY glDrawTexsOES(GLshort x, GLshort y, GLshort z, GLshort width, GLshort height)
{
	UNIMPLEMENTED();
}

void GL_APIENTRY glDrawTexiOES(GLint x, GLint y, GLint z, GLint width, GLint height)
{
	UNIMPLEMENTED();
}

void GL_APIENTRY glDrawTexxOES(GLfixed x, GLfixed y, GLfixed z, GLfixed width, GLfixed height)
{
	UNIMPLEMENTED();
}

void GL_APIENTRY glDrawTexsvOES(const GLshort *coords)
{
	UNIMPLEMENTED();
}

void GL_APIENTRY glDrawTexivOES(const GLint *coords)
{
	UNIMPLEMENTED();
}

void GL_APIENTRY glDrawTexxvOES(const GLfixed *coords)
{
	UNIMPLEMENTED();
}

void GL_APIENTRY glDrawTexfOES(GLfloat x, GLfloat y, GLfloat z, GLfloat width, GLfloat height)
{
	UNIMPLEMENTED();
}

void GL_APIENTRY glDrawTexfvOES(const GLfloat *coords)
{
	UNIMPLEMENTED();
}

__eglMustCastToProperFunctionPointerType glGetProcAddress(const char *procname)
{
	struct Extension
	{
		const char *name;
		__eglMustCastToProperFunctionPointerType address;
	};

	static const Extension glExtensions[] =
	{
		#define EXTENSION(name) {#name, (__eglMustCastToProperFunctionPointerType)name}

		EXTENSION(glEGLImageTargetTexture2DOES),
		EXTENSION(glEGLImageTargetRenderbufferStorageOES),
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
		EXTENSION(glBlendEquationOES),
		EXTENSION(glBlendEquationSeparateOES),
		EXTENSION(glBlendFuncSeparateOES),
		EXTENSION(glPointSizePointerOES),

		#undef EXTENSION
	};

	for(int ext = 0; ext < sizeof(glExtensions) / sizeof(Extension); ext++)
	{
		if(strcmp(procname, glExtensions[ext].name) == 0)
		{
			return (__eglMustCastToProperFunctionPointerType)glExtensions[ext].address;
		}
	}

	return NULL;
}

void GL_APIENTRY Register(const char *licenseKey)
{
	RegisterLicenseKey(licenseKey);
}

}
