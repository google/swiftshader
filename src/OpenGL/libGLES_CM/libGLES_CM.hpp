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

#ifndef libGLES_CM_hpp
#define libGLES_CM_hpp

#include <GLES/gl.h>
#include <GLES/glext.h>
#include <EGL/egl.h>

#include "Common/SharedLibrary.hpp"

namespace sw
{
class FrameBuffer;
enum Format : unsigned char;
}

namespace egl
{
class Display;
class Context;
class Image;
class Config;
}

class LibGLES_CMexports
{
public:
	LibGLES_CMexports();

	void (GL_APIENTRY *glActiveTexture)(GLenum texture);
	void (GL_APIENTRY *glAlphaFunc)(GLenum func, GLclampf ref);
	void (GL_APIENTRY *glAlphaFuncx)(GLenum func, GLclampx ref);
	void (GL_APIENTRY *glBindBuffer)(GLenum target, GLuint buffer);
	void (GL_APIENTRY *glBindFramebuffer)(GLenum target, GLuint framebuffer);
	void (GL_APIENTRY *glBindFramebufferOES)(GLenum target, GLuint framebuffer);
	void (GL_APIENTRY *glBindRenderbufferOES)(GLenum target, GLuint renderbuffer);
	void (GL_APIENTRY *glBindTexture)(GLenum target, GLuint texture);
	void (GL_APIENTRY *glBlendEquationOES)(GLenum mode);
	void (GL_APIENTRY *glBlendEquationSeparateOES)(GLenum modeRGB, GLenum modeAlpha);
	void (GL_APIENTRY *glBlendFunc)(GLenum sfactor, GLenum dfactor);
	void (GL_APIENTRY *glBlendFuncSeparateOES)(GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha);
	void (GL_APIENTRY *glBufferData)(GLenum target, GLsizeiptr size, const GLvoid* data, GLenum usage);
	void (GL_APIENTRY *glBufferSubData)(GLenum target, GLintptr offset, GLsizeiptr size, const GLvoid* data);
	GLenum (GL_APIENTRY *glCheckFramebufferStatusOES)(GLenum target);
	void (GL_APIENTRY *glClear)(GLbitfield mask);
	void (GL_APIENTRY *glClearColor)(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha);
	void (GL_APIENTRY *glClearColorx)(GLclampx red, GLclampx green, GLclampx blue, GLclampx alpha);
	void (GL_APIENTRY *glClearDepthf)(GLclampf depth);
	void (GL_APIENTRY *glClearDepthx)(GLclampx depth);
	void (GL_APIENTRY *glClearStencil)(GLint s);
	void (GL_APIENTRY *glClientActiveTexture)(GLenum texture);
	void (GL_APIENTRY *glClipPlanef)(GLenum plane, const GLfloat *equation);
	void (GL_APIENTRY *glClipPlanex)(GLenum plane, const GLfixed *equation);
	void (GL_APIENTRY *glColor4f)(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
	void (GL_APIENTRY *glColor4ub)(GLubyte red, GLubyte green, GLubyte blue, GLubyte alpha);
	void (GL_APIENTRY *glColor4x)(GLfixed red, GLfixed green, GLfixed blue, GLfixed alpha);
	void (GL_APIENTRY *glColorMask)(GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha);
	void (GL_APIENTRY *glColorPointer)(GLint size, GLenum type, GLsizei stride, const GLvoid *pointer);
	void (GL_APIENTRY *glCompressedTexImage2D)(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height,
	                                           GLint border, GLsizei imageSize, const GLvoid* data);
	void (GL_APIENTRY *glCompressedTexSubImage2D)(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height,
	                                              GLenum format, GLsizei imageSize, const GLvoid* data);
	void (GL_APIENTRY *glCopyTexImage2D)(GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border);
	void (GL_APIENTRY *glCopyTexSubImage2D)(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height);
	void (GL_APIENTRY *glCullFace)(GLenum mode);
	void (GL_APIENTRY *glDeleteBuffers)(GLsizei n, const GLuint* buffers);
	void (GL_APIENTRY *glDeleteFramebuffersOES)(GLsizei n, const GLuint* framebuffers);
	void (GL_APIENTRY *glDeleteRenderbuffersOES)(GLsizei n, const GLuint* renderbuffers);
	void (GL_APIENTRY *glDeleteTextures)(GLsizei n, const GLuint* textures);
	void (GL_APIENTRY *glDepthFunc)(GLenum func);
	void (GL_APIENTRY *glDepthMask)(GLboolean flag);
	void (GL_APIENTRY *glDepthRangex)(GLclampx zNear, GLclampx zFar);
	void (GL_APIENTRY *glDepthRangef)(GLclampf zNear, GLclampf zFar);
	void (GL_APIENTRY *glDisable)(GLenum cap);
	void (GL_APIENTRY *glDisableClientState)(GLenum array);
	void (GL_APIENTRY *glDrawArrays)(GLenum mode, GLint first, GLsizei count);
	void (GL_APIENTRY *glDrawElements)(GLenum mode, GLsizei count, GLenum type, const GLvoid* indices);
	void (GL_APIENTRY *glEnable)(GLenum cap);
	void (GL_APIENTRY *glEnableClientState)(GLenum array);
	void (GL_APIENTRY *glFinish)(void);
	void (GL_APIENTRY *glFlush)(void);
	void (GL_APIENTRY *glFramebufferRenderbufferOES)(GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer);
	void (GL_APIENTRY *glFramebufferTexture2DOES)(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);
	void (GL_APIENTRY *glFogf)(GLenum pname, GLfloat param);
	void (GL_APIENTRY *glFogfv)(GLenum pname, const GLfloat *params);
	void (GL_APIENTRY *glFogx)(GLenum pname, GLfixed param);
	void (GL_APIENTRY *glFogxv)(GLenum pname, const GLfixed *params);
	void (GL_APIENTRY *glFrontFace)(GLenum mode);
	void (GL_APIENTRY *glFrustumf)(GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat zNear, GLfloat zFar);
	void (GL_APIENTRY *glFrustumx)(GLfixed left, GLfixed right, GLfixed bottom, GLfixed top, GLfixed zNear, GLfixed zFar);
	void (GL_APIENTRY *glGenerateMipmapOES)(GLenum target);
	void (GL_APIENTRY *glGenBuffers)(GLsizei n, GLuint* buffers);
	void (GL_APIENTRY *glGenFramebuffersOES)(GLsizei n, GLuint* framebuffers);
	void (GL_APIENTRY *glGenRenderbuffersOES)(GLsizei n, GLuint* renderbuffers);
	void (GL_APIENTRY *glGenTextures)(GLsizei n, GLuint* textures);
	void (GL_APIENTRY *glGetRenderbufferParameterivOES)(GLenum target, GLenum pname, GLint* params);
	void (GL_APIENTRY *glGetBooleanv)(GLenum pname, GLboolean* params);
	void (GL_APIENTRY *glGetBufferParameteriv)(GLenum target, GLenum pname, GLint* params);
	void (GL_APIENTRY *glGetClipPlanef)(GLenum pname, GLfloat eqn[4]);
	void (GL_APIENTRY *glGetClipPlanex)(GLenum pname, GLfixed eqn[4]);
	GLenum (GL_APIENTRY *glGetError)(void);
	void (GL_APIENTRY *glGetFixedv)(GLenum pname, GLfixed *params);
	void (GL_APIENTRY *glGetFloatv)(GLenum pname, GLfloat* params);
	void (GL_APIENTRY *glGetFramebufferAttachmentParameterivOES)(GLenum target, GLenum attachment, GLenum pname, GLint* params);
	void (GL_APIENTRY *glGetIntegerv)(GLenum pname, GLint* params);
	void (GL_APIENTRY *glGetLightfv)(GLenum light, GLenum pname, GLfloat *params);
	void (GL_APIENTRY *glGetLightxv)(GLenum light, GLenum pname, GLfixed *params);
	void (GL_APIENTRY *glGetMaterialfv)(GLenum face, GLenum pname, GLfloat *params);
	void (GL_APIENTRY *glGetMaterialxv)(GLenum face, GLenum pname, GLfixed *params);
	void (GL_APIENTRY *glGetPointerv)(GLenum pname, GLvoid **params);
	const GLubyte* (GL_APIENTRY *glGetString)(GLenum name);
	void (GL_APIENTRY *glGetTexParameterfv)(GLenum target, GLenum pname, GLfloat* params);
	void (GL_APIENTRY *glGetTexParameteriv)(GLenum target, GLenum pname, GLint* params);
	void (GL_APIENTRY *glGetTexEnvfv)(GLenum env, GLenum pname, GLfloat *params);
	void (GL_APIENTRY *glGetTexEnviv)(GLenum env, GLenum pname, GLint *params);
	void (GL_APIENTRY *glGetTexEnvxv)(GLenum env, GLenum pname, GLfixed *params);
	void (GL_APIENTRY *glGetTexParameterxv)(GLenum target, GLenum pname, GLfixed *params);
	void (GL_APIENTRY *glHint)(GLenum target, GLenum mode);
	GLboolean (GL_APIENTRY *glIsBuffer)(GLuint buffer);
	GLboolean (GL_APIENTRY *glIsEnabled)(GLenum cap);
	GLboolean (GL_APIENTRY *glIsFramebufferOES)(GLuint framebuffer);
	GLboolean (GL_APIENTRY *glIsTexture)(GLuint texture);
	GLboolean (GL_APIENTRY *glIsRenderbufferOES)(GLuint renderbuffer);
	void (GL_APIENTRY *glLightModelf)(GLenum pname, GLfloat param);
	void (GL_APIENTRY *glLightModelfv)(GLenum pname, const GLfloat *params);
	void (GL_APIENTRY *glLightModelx)(GLenum pname, GLfixed param);
	void (GL_APIENTRY *glLightModelxv)(GLenum pname, const GLfixed *params);
	void (GL_APIENTRY *glLightf)(GLenum light, GLenum pname, GLfloat param);
	void (GL_APIENTRY *glLightfv)(GLenum light, GLenum pname, const GLfloat *params);
	void (GL_APIENTRY *glLightx)(GLenum light, GLenum pname, GLfixed param);
	void (GL_APIENTRY *glLightxv)(GLenum light, GLenum pname, const GLfixed *params);
	void (GL_APIENTRY *glLineWidth)(GLfloat width);
	void (GL_APIENTRY *glLineWidthx)(GLfixed width);
	void (GL_APIENTRY *glLoadIdentity)(void);
	void (GL_APIENTRY *glLoadMatrixf)(const GLfloat *m);
	void (GL_APIENTRY *glLoadMatrixx)(const GLfixed *m);
	void (GL_APIENTRY *glLogicOp)(GLenum opcode);
	void (GL_APIENTRY *glMaterialf)(GLenum face, GLenum pname, GLfloat param);
	void (GL_APIENTRY *glMaterialfv)(GLenum face, GLenum pname, const GLfloat *params);
	void (GL_APIENTRY *glMaterialx)(GLenum face, GLenum pname, GLfixed param);
	void (GL_APIENTRY *glMaterialxv)(GLenum face, GLenum pname, const GLfixed *params);
	void (GL_APIENTRY *glMatrixMode)(GLenum mode);
	void (GL_APIENTRY *glMultMatrixf)(const GLfloat *m);
	void (GL_APIENTRY *glMultMatrixx)(const GLfixed *m);
	void (GL_APIENTRY *glMultiTexCoord4f)(GLenum target, GLfloat s, GLfloat t, GLfloat r, GLfloat q);
	void (GL_APIENTRY *glMultiTexCoord4x)(GLenum target, GLfixed s, GLfixed t, GLfixed r, GLfixed q);
	void (GL_APIENTRY *glNormal3f)(GLfloat nx, GLfloat ny, GLfloat nz);
	void (GL_APIENTRY *glNormal3x)(GLfixed nx, GLfixed ny, GLfixed nz);
	void (GL_APIENTRY *glNormalPointer)(GLenum type, GLsizei stride, const GLvoid *pointer);
	void (GL_APIENTRY *glOrthof)(GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat zNear, GLfloat zFar);
	void (GL_APIENTRY *glOrthox)(GLfixed left, GLfixed right, GLfixed bottom, GLfixed top, GLfixed zNear, GLfixed zFar);
	void (GL_APIENTRY *glPixelStorei)(GLenum pname, GLint param);
	void (GL_APIENTRY *glPointParameterf)(GLenum pname, GLfloat param);
	void (GL_APIENTRY *glPointParameterfv)(GLenum pname, const GLfloat *params);
	void (GL_APIENTRY *glPointParameterx)(GLenum pname, GLfixed param);
	void (GL_APIENTRY *glPointParameterxv)(GLenum pname, const GLfixed *params);
	void (GL_APIENTRY *glPointSize)(GLfloat size);
	void (GL_APIENTRY *glPointSizePointerOES)(GLenum type, GLsizei stride, const GLvoid *pointer);
	void (GL_APIENTRY *glPointSizex)(GLfixed size);
	void (GL_APIENTRY *glPolygonOffset)(GLfloat factor, GLfloat units);
	void (GL_APIENTRY *glPolygonOffsetx)(GLfixed factor, GLfixed units);
	void (GL_APIENTRY *glPopMatrix)(void);
	void (GL_APIENTRY *glPushMatrix)(void);
	void (GL_APIENTRY *glReadPixels)(GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid* pixels);
	void (GL_APIENTRY *glRenderbufferStorageOES)(GLenum target, GLenum internalformat, GLsizei width, GLsizei height);
	void (GL_APIENTRY *glRotatef)(GLfloat angle, GLfloat x, GLfloat y, GLfloat z);
	void (GL_APIENTRY *glRotatex)(GLfixed angle, GLfixed x, GLfixed y, GLfixed z);
	void (GL_APIENTRY *glSampleCoverage)(GLclampf value, GLboolean invert);
	void (GL_APIENTRY *glSampleCoveragex)(GLclampx value, GLboolean invert);
	void (GL_APIENTRY *glScalef)(GLfloat x, GLfloat y, GLfloat z);
	void (GL_APIENTRY *glScalex)(GLfixed x, GLfixed y, GLfixed z);
	void (GL_APIENTRY *glScissor)(GLint x, GLint y, GLsizei width, GLsizei height);
	void (GL_APIENTRY *glShadeModel)(GLenum mode);
	void (GL_APIENTRY *glStencilFunc)(GLenum func, GLint ref, GLuint mask);
	void (GL_APIENTRY *glStencilMask)(GLuint mask);
	void (GL_APIENTRY *glStencilOp)(GLenum fail, GLenum zfail, GLenum zpass);
	void (GL_APIENTRY *glTexCoordPointer)(GLint size, GLenum type, GLsizei stride, const GLvoid *pointer);
	void (GL_APIENTRY *glTexEnvf)(GLenum target, GLenum pname, GLfloat param);
	void (GL_APIENTRY *glTexEnvfv)(GLenum target, GLenum pname, const GLfloat *params);
	void (GL_APIENTRY *glTexEnvi)(GLenum target, GLenum pname, GLint param);
	void (GL_APIENTRY *glTexEnvx)(GLenum target, GLenum pname, GLfixed param);
	void (GL_APIENTRY *glTexEnviv)(GLenum target, GLenum pname, const GLint *params);
	void (GL_APIENTRY *glTexEnvxv)(GLenum target, GLenum pname, const GLfixed *params);
	void (GL_APIENTRY *glTexImage2D)(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height,
	                                 GLint border, GLenum format, GLenum type, const GLvoid* pixels);
	void (GL_APIENTRY *glTexParameterf)(GLenum target, GLenum pname, GLfloat param);
	void (GL_APIENTRY *glTexParameterfv)(GLenum target, GLenum pname, const GLfloat* params);
	void (GL_APIENTRY *glTexParameteri)(GLenum target, GLenum pname, GLint param);
	void (GL_APIENTRY *glTexParameteriv)(GLenum target, GLenum pname, const GLint* params);
	void (GL_APIENTRY *glTexParameterx)(GLenum target, GLenum pname, GLfixed param);
	void (GL_APIENTRY *glTexParameterxv)(GLenum target, GLenum pname, const GLfixed *params);
	void (GL_APIENTRY *glTexSubImage2D)(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height,
	                                    GLenum format, GLenum type, const GLvoid* pixels);
	void (GL_APIENTRY *glTranslatef)(GLfloat x, GLfloat y, GLfloat z);
	void (GL_APIENTRY *glTranslatex)(GLfixed x, GLfixed y, GLfixed z);
	void (GL_APIENTRY *glVertexPointer)(GLint size, GLenum type, GLsizei stride, const GLvoid *pointer);
	void (GL_APIENTRY *glViewport)(GLint x, GLint y, GLsizei width, GLsizei height);
	void (GL_APIENTRY *glEGLImageTargetTexture2DOES)(GLenum target, GLeglImageOES image);
	void (GL_APIENTRY *glEGLImageTargetRenderbufferStorageOES)(GLenum target, GLeglImageOES image);
	void (GL_APIENTRY *glDrawTexsOES)(GLshort x, GLshort y, GLshort z, GLshort width, GLshort height);
	void (GL_APIENTRY *glDrawTexiOES)(GLint x, GLint y, GLint z, GLint width, GLint height);
	void (GL_APIENTRY *glDrawTexxOES)(GLfixed x, GLfixed y, GLfixed z, GLfixed width, GLfixed height);
	void (GL_APIENTRY *glDrawTexsvOES)(const GLshort *coords);
	void (GL_APIENTRY *glDrawTexivOES)(const GLint *coords);
	void (GL_APIENTRY *glDrawTexxvOES)(const GLfixed *coords);
	void (GL_APIENTRY *glDrawTexfOES)(GLfloat x, GLfloat y, GLfloat z, GLfloat width, GLfloat height);
	void (GL_APIENTRY *glDrawTexfvOES)(const GLfloat *coords);

	egl::Context *(*es1CreateContext)(egl::Display *display, const egl::Context *shareContext, const egl::Config *config);
	__eglMustCastToProperFunctionPointerType (*es1GetProcAddress)(const char *procname);
	egl::Image *(*createBackBuffer)(int width, int height, sw::Format format, int multiSampleDepth);
	egl::Image *(*createDepthStencil)(int width, int height, sw::Format format, int multiSampleDepth);
	sw::FrameBuffer *(*createFrameBuffer)(void *nativeDisplay, EGLNativeWindowType window, int width, int height);
};

class LibGLES_CM
{
public:
	LibGLES_CM()
	{
	}

	~LibGLES_CM()
	{
		freeLibrary(libGLES_CM);
	}

	operator bool()
	{
		return loadExports() != nullptr;
	}

	LibGLES_CMexports *operator->()
	{
		return loadExports();
	}

private:
	LibGLES_CMexports *loadExports()
	{
		if(!loadLibraryAttempted && !libGLES_CM)
		{
			#if defined(_WIN32)
				#if defined(__LP64__)
					const char *libGLES_CM_lib[] = {"libGLES_CM.dll", "lib64GLES_CM_translator.dll"};
				#else
					const char *libGLES_CM_lib[] = {"libGLES_CM.dll", "libGLES_CM_translator.dll"};
				#endif
			#elif defined(__ANDROID__)
				const char *libGLES_CM_lib[] = {"libGLESv1_CM_swiftshader.so", "libGLESv1_CM_swiftshader.so"};
			#elif defined(__linux__)
				#if defined(__LP64__)
					const char *libGLES_CM_lib[] = {"lib64GLES_CM_translator.so", "libGLES_CM.so.1", "libGLES_CM.so"};
				#else
					const char *libGLES_CM_lib[] = {"libGLES_CM_translator.so", "libGLES_CM.so.1", "libGLES_CM.so"};
				#endif
			#elif defined(__APPLE__)
				#if defined(__LP64__)
					const char *libGLES_CM_lib[] = {"lib64GLES_CM_translator.dylib", "libGLES_CM.dylib"};
				#else
					const char *libGLES_CM_lib[] = {"libGLES_CM_translator.dylib", "libGLES_CM.dylib"};
				#endif
			#elif defined(__Fuchsia__)
				const char *libGLES_CM_lib[] = {"libGLES_CM.so"};
			#else
				#error "libGLES_CM::loadExports unimplemented for this platform"
			#endif

			std::string directory = getModuleDirectory();
			libGLES_CM = loadLibrary(directory, libGLES_CM_lib, "libGLES_CM_swiftshader");

			if(libGLES_CM)
			{
				auto libGLES_CM_swiftshader = (LibGLES_CMexports *(*)())getProcAddress(libGLES_CM, "libGLES_CM_swiftshader");
				libGLES_CMexports = libGLES_CM_swiftshader();
			}

			loadLibraryAttempted = true;
		}

		return libGLES_CMexports;
	}

	void *libGLES_CM = nullptr;
	LibGLES_CMexports *libGLES_CMexports = nullptr;
	bool loadLibraryAttempted = false;
};

#endif   // libGLES_CM_hpp
