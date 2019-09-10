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

#ifndef libGLESv2_hpp
#define libGLESv2_hpp

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
class ClientBuffer;
}

class LibGLESv2exports
{
public:
	LibGLESv2exports();

	void (GL_APIENTRY *glActiveTexture)(GLenum texture);
	void (GL_APIENTRY *glAttachShader)(GLuint program, GLuint shader);
	void (GL_APIENTRY *glBeginQueryEXT)(GLenum target, GLuint name);
	void (GL_APIENTRY *glBindAttribLocation)(GLuint program, GLuint index, const GLchar* name);
	void (GL_APIENTRY *glBindBuffer)(GLenum target, GLuint buffer);
	void (GL_APIENTRY *glBindFramebuffer)(GLenum target, GLuint framebuffer);
	void (GL_APIENTRY *glBindRenderbuffer)(GLenum target, GLuint renderbuffer);
	void (GL_APIENTRY *glBindTexture)(GLenum target, GLuint texture);
	void (GL_APIENTRY *glBlendColor)(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha);
	void (GL_APIENTRY *glBlendEquation)(GLenum mode);
	void (GL_APIENTRY *glBlendEquationSeparate)(GLenum modeRGB, GLenum modeAlpha);
	void (GL_APIENTRY *glBlendFunc)(GLenum sfactor, GLenum dfactor);
	void (GL_APIENTRY *glBlendFuncSeparate)(GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha);
	void (GL_APIENTRY *glBufferData)(GLenum target, GLsizeiptr size, const GLvoid* data, GLenum usage);
	void (GL_APIENTRY *glBufferSubData)(GLenum target, GLintptr offset, GLsizeiptr size, const GLvoid* data);
	GLenum (GL_APIENTRY *glCheckFramebufferStatus)(GLenum target);
	void (GL_APIENTRY *glClear)(GLbitfield mask);
	void (GL_APIENTRY *glClearColor)(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha);
	void (GL_APIENTRY *glClearDepthf)(GLclampf depth);
	void (GL_APIENTRY *glClearStencil)(GLint s);
	void (GL_APIENTRY *glColorMask)(GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha);
	void (GL_APIENTRY *glCompileShader)(GLuint shader);
	void (GL_APIENTRY *glCompressedTexImage2D)(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height,
	                                           GLint border, GLsizei imageSize, const GLvoid* data);
	void (GL_APIENTRY *glCompressedTexSubImage2D)(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height,
	                                              GLenum format, GLsizei imageSize, const GLvoid* data);
	void (GL_APIENTRY *glCopyTexImage2D)(GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border);
	void (GL_APIENTRY *glCopyTexSubImage2D)(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height);
	GLuint (GL_APIENTRY *glCreateProgram)(void);
	GLuint (GL_APIENTRY *glCreateShader)(GLenum type);
	void (GL_APIENTRY *glCullFace)(GLenum mode);
	void (GL_APIENTRY *glDeleteBuffers)(GLsizei n, const GLuint* buffers);
	void (GL_APIENTRY *glDeleteFencesNV)(GLsizei n, const GLuint* fences);
	void (GL_APIENTRY *glDeleteFramebuffers)(GLsizei n, const GLuint* framebuffers);
	void (GL_APIENTRY *glDeleteProgram)(GLuint program);
	void (GL_APIENTRY *glDeleteQueriesEXT)(GLsizei n, const GLuint *ids);
	void (GL_APIENTRY *glDeleteRenderbuffers)(GLsizei n, const GLuint* renderbuffers);
	void (GL_APIENTRY *glDeleteShader)(GLuint shader);
	void (GL_APIENTRY *glDeleteTextures)(GLsizei n, const GLuint* textures);
	void (GL_APIENTRY *glDepthFunc)(GLenum func);
	void (GL_APIENTRY *glDepthMask)(GLboolean flag);
	void (GL_APIENTRY *glDepthRangef)(GLclampf zNear, GLclampf zFar);
	void (GL_APIENTRY *glDetachShader)(GLuint program, GLuint shader);
	void (GL_APIENTRY *glDisable)(GLenum cap);
	void (GL_APIENTRY *glDisableVertexAttribArray)(GLuint index);
	void (GL_APIENTRY *glDrawArrays)(GLenum mode, GLint first, GLsizei count);
	void (GL_APIENTRY *glDrawElements)(GLenum mode, GLsizei count, GLenum type, const GLvoid* indices);
	void (GL_APIENTRY *glDrawArraysInstancedEXT)(GLenum mode, GLint first, GLsizei count, GLsizei instanceCount);
	void (GL_APIENTRY *glDrawElementsInstancedEXT)(GLenum mode, GLsizei count, GLenum type, const void *indices, GLsizei instanceCount);
	void (GL_APIENTRY *glVertexAttribDivisorEXT)(GLuint index, GLuint divisor);
	void (GL_APIENTRY *glDrawArraysInstancedANGLE)(GLenum mode, GLint first, GLsizei count, GLsizei instanceCount);
	void (GL_APIENTRY *glDrawElementsInstancedANGLE)(GLenum mode, GLsizei count, GLenum type, const void *indices, GLsizei instanceCount);
	void (GL_APIENTRY *glVertexAttribDivisorANGLE)(GLuint index, GLuint divisor);
	void (GL_APIENTRY *glEnable)(GLenum cap);
	void (GL_APIENTRY *glEnableVertexAttribArray)(GLuint index);
	void (GL_APIENTRY *glEndQueryEXT)(GLenum target);
	void (GL_APIENTRY *glFinishFenceNV)(GLuint fence);
	void (GL_APIENTRY *glFinish)(void);
	void (GL_APIENTRY *glFlush)(void);
	void (GL_APIENTRY *glFramebufferRenderbuffer)(GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer);
	void (GL_APIENTRY *glFramebufferTexture2D)(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);
	void (GL_APIENTRY *glFrontFace)(GLenum mode);
	void (GL_APIENTRY *glGenBuffers)(GLsizei n, GLuint* buffers);
	void (GL_APIENTRY *glGenerateMipmap)(GLenum target);
	void (GL_APIENTRY *glGenFencesNV)(GLsizei n, GLuint* fences);
	void (GL_APIENTRY *glGenFramebuffers)(GLsizei n, GLuint* framebuffers);
	void (GL_APIENTRY *glGenQueriesEXT)(GLsizei n, GLuint* ids);
	void (GL_APIENTRY *glGenRenderbuffers)(GLsizei n, GLuint* renderbuffers);
	void (GL_APIENTRY *glGenTextures)(GLsizei n, GLuint* textures);
	void (GL_APIENTRY *glGetActiveAttrib)(GLuint program, GLuint index, GLsizei bufsize, GLsizei *length, GLint *size, GLenum *type, GLchar *name);
	void (GL_APIENTRY *glGetActiveUniform)(GLuint program, GLuint index, GLsizei bufsize, GLsizei* length, GLint* size, GLenum* type, GLchar* name);
	void (GL_APIENTRY *glGetAttachedShaders)(GLuint program, GLsizei maxcount, GLsizei* count, GLuint* shaders);
	int (GL_APIENTRY *glGetAttribLocation)(GLuint program, const GLchar* name);
	void (GL_APIENTRY *glGetBooleanv)(GLenum pname, GLboolean* params);
	void (GL_APIENTRY *glGetBufferParameteriv)(GLenum target, GLenum pname, GLint* params);
	GLenum (GL_APIENTRY *glGetError)(void);
	void (GL_APIENTRY *glGetFenceivNV)(GLuint fence, GLenum pname, GLint *params);
	void (GL_APIENTRY *glGetFloatv)(GLenum pname, GLfloat* params);
	void (GL_APIENTRY *glGetFramebufferAttachmentParameteriv)(GLenum target, GLenum attachment, GLenum pname, GLint* params);
	GLenum (GL_APIENTRY *glGetGraphicsResetStatusEXT)(void);
	void (GL_APIENTRY *glGetIntegerv)(GLenum pname, GLint* params);
	void (GL_APIENTRY *glGetProgramiv)(GLuint program, GLenum pname, GLint* params);
	void (GL_APIENTRY *glGetProgramInfoLog)(GLuint program, GLsizei bufsize, GLsizei* length, GLchar* infolog);
	void (GL_APIENTRY *glGetQueryivEXT)(GLenum target, GLenum pname, GLint *params);
	void (GL_APIENTRY *glGetQueryObjectuivEXT)(GLuint name, GLenum pname, GLuint *params);
	void (GL_APIENTRY *glGetRenderbufferParameteriv)(GLenum target, GLenum pname, GLint* params);
	void (GL_APIENTRY *glGetShaderiv)(GLuint shader, GLenum pname, GLint* params);
	void (GL_APIENTRY *glGetShaderInfoLog)(GLuint shader, GLsizei bufsize, GLsizei* length, GLchar* infolog);
	void (GL_APIENTRY *glGetShaderPrecisionFormat)(GLenum shadertype, GLenum precisiontype, GLint* range, GLint* precision);
	void (GL_APIENTRY *glGetShaderSource)(GLuint shader, GLsizei bufsize, GLsizei* length, GLchar* source);
	const GLubyte* (GL_APIENTRY *glGetString)(GLenum name);
	void (GL_APIENTRY *glGetTexParameterfv)(GLenum target, GLenum pname, GLfloat* params);
	void (GL_APIENTRY *glGetTexParameteriv)(GLenum target, GLenum pname, GLint* params);
	void (GL_APIENTRY *glGetnUniformfvEXT)(GLuint program, GLint location, GLsizei bufSize, GLfloat* params);
	void (GL_APIENTRY *glGetUniformfv)(GLuint program, GLint location, GLfloat* params);
	void (GL_APIENTRY *glGetnUniformivEXT)(GLuint program, GLint location, GLsizei bufSize, GLint* params);
	void (GL_APIENTRY *glGetUniformiv)(GLuint program, GLint location, GLint* params);
	int (GL_APIENTRY *glGetUniformLocation)(GLuint program, const GLchar* name);
	void (GL_APIENTRY *glGetVertexAttribfv)(GLuint index, GLenum pname, GLfloat* params);
	void (GL_APIENTRY *glGetVertexAttribiv)(GLuint index, GLenum pname, GLint* params);
	void (GL_APIENTRY *glGetVertexAttribPointerv)(GLuint index, GLenum pname, GLvoid** pointer);
	void (GL_APIENTRY *glHint)(GLenum target, GLenum mode);
	GLboolean (GL_APIENTRY *glIsBuffer)(GLuint buffer);
	GLboolean (GL_APIENTRY *glIsEnabled)(GLenum cap);
	GLboolean (GL_APIENTRY *glIsFenceNV)(GLuint fence);
	GLboolean (GL_APIENTRY *glIsFramebuffer)(GLuint framebuffer);
	GLboolean (GL_APIENTRY *glIsProgram)(GLuint program);
	GLboolean (GL_APIENTRY *glIsQueryEXT)(GLuint name);
	GLboolean (GL_APIENTRY *glIsRenderbuffer)(GLuint renderbuffer);
	GLboolean (GL_APIENTRY *glIsShader)(GLuint shader);
	GLboolean (GL_APIENTRY *glIsTexture)(GLuint texture);
	void (GL_APIENTRY *glLineWidth)(GLfloat width);
	void (GL_APIENTRY *glLinkProgram)(GLuint program);
	void (GL_APIENTRY *glPixelStorei)(GLenum pname, GLint param);
	void (GL_APIENTRY *glPolygonOffset)(GLfloat factor, GLfloat units);
	void (GL_APIENTRY *glReadnPixelsEXT)(GLint x, GLint y, GLsizei width, GLsizei height,
	                                     GLenum format, GLenum type, GLsizei bufSize, GLvoid *data);
	void (GL_APIENTRY *glReadPixels)(GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid* pixels);
	void (GL_APIENTRY *glReleaseShaderCompiler)(void);
	void (GL_APIENTRY *glRenderbufferStorageMultisample)(GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height);
	void (GL_APIENTRY *glRenderbufferStorageMultisampleANGLE)(GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height);
	void (GL_APIENTRY *glRenderbufferStorage)(GLenum target, GLenum internalformat, GLsizei width, GLsizei height);
	void (GL_APIENTRY *glSampleCoverage)(GLclampf value, GLboolean invert);
	void (GL_APIENTRY *glSetFenceNV)(GLuint fence, GLenum condition);
	void (GL_APIENTRY *glScissor)(GLint x, GLint y, GLsizei width, GLsizei height);
	void (GL_APIENTRY *glShaderBinary)(GLsizei n, const GLuint* shaders, GLenum binaryformat, const GLvoid* binary, GLsizei length);
	void (GL_APIENTRY *glShaderSource)(GLuint shader, GLsizei count, const GLchar *const *string, const GLint *length);
	void (GL_APIENTRY *glStencilFunc)(GLenum func, GLint ref, GLuint mask);
	void (GL_APIENTRY *glStencilFuncSeparate)(GLenum face, GLenum func, GLint ref, GLuint mask);
	void (GL_APIENTRY *glStencilMask)(GLuint mask);
	void (GL_APIENTRY *glStencilMaskSeparate)(GLenum face, GLuint mask);
	void (GL_APIENTRY *glStencilOp)(GLenum fail, GLenum zfail, GLenum zpass);
	void (GL_APIENTRY *glStencilOpSeparate)(GLenum face, GLenum fail, GLenum zfail, GLenum zpass);
	GLboolean (GL_APIENTRY *glTestFenceNV)(GLuint fence);
	void (GL_APIENTRY *glTexImage2D)(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height,
	                                 GLint border, GLenum format, GLenum type, const GLvoid* pixels);
	void (GL_APIENTRY *glTexParameterf)(GLenum target, GLenum pname, GLfloat param);
	void (GL_APIENTRY *glTexParameterfv)(GLenum target, GLenum pname, const GLfloat* params);
	void (GL_APIENTRY *glTexParameteri)(GLenum target, GLenum pname, GLint param);
	void (GL_APIENTRY *glTexParameteriv)(GLenum target, GLenum pname, const GLint* params);
	void (GL_APIENTRY *glTexSubImage2D)(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height,
	                                    GLenum format, GLenum type, const GLvoid* pixels);
	void (GL_APIENTRY *glUniform1f)(GLint location, GLfloat x);
	void (GL_APIENTRY *glUniform1fv)(GLint location, GLsizei count, const GLfloat* v);
	void (GL_APIENTRY *glUniform1i)(GLint location, GLint x);
	void (GL_APIENTRY *glUniform1iv)(GLint location, GLsizei count, const GLint* v);
	void (GL_APIENTRY *glUniform2f)(GLint location, GLfloat x, GLfloat y);
	void (GL_APIENTRY *glUniform2fv)(GLint location, GLsizei count, const GLfloat* v);
	void (GL_APIENTRY *glUniform2i)(GLint location, GLint x, GLint y);
	void (GL_APIENTRY *glUniform2iv)(GLint location, GLsizei count, const GLint* v);
	void (GL_APIENTRY *glUniform3f)(GLint location, GLfloat x, GLfloat y, GLfloat z);
	void (GL_APIENTRY *glUniform3fv)(GLint location, GLsizei count, const GLfloat* v);
	void (GL_APIENTRY *glUniform3i)(GLint location, GLint x, GLint y, GLint z);
	void (GL_APIENTRY *glUniform3iv)(GLint location, GLsizei count, const GLint* v);
	void (GL_APIENTRY *glUniform4f)(GLint location, GLfloat x, GLfloat y, GLfloat z, GLfloat w);
	void (GL_APIENTRY *glUniform4fv)(GLint location, GLsizei count, const GLfloat* v);
	void (GL_APIENTRY *glUniform4i)(GLint location, GLint x, GLint y, GLint z, GLint w);
	void (GL_APIENTRY *glUniform4iv)(GLint location, GLsizei count, const GLint* v);
	void (GL_APIENTRY *glUniformMatrix2fv)(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
	void (GL_APIENTRY *glUniformMatrix3fv)(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
	void (GL_APIENTRY *glUniformMatrix4fv)(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
	void (GL_APIENTRY *glUseProgram)(GLuint program);
	void (GL_APIENTRY *glValidateProgram)(GLuint program);
	void (GL_APIENTRY *glVertexAttrib1f)(GLuint index, GLfloat x);
	void (GL_APIENTRY *glVertexAttrib1fv)(GLuint index, const GLfloat* values);
	void (GL_APIENTRY *glVertexAttrib2f)(GLuint index, GLfloat x, GLfloat y);
	void (GL_APIENTRY *glVertexAttrib2fv)(GLuint index, const GLfloat* values);
	void (GL_APIENTRY *glVertexAttrib3f)(GLuint index, GLfloat x, GLfloat y, GLfloat z);
	void (GL_APIENTRY *glVertexAttrib3fv)(GLuint index, const GLfloat* values);
	void (GL_APIENTRY *glVertexAttrib4f)(GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w);
	void (GL_APIENTRY *glVertexAttrib4fv)(GLuint index, const GLfloat* values);
	void (GL_APIENTRY *glVertexAttribPointer)(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid* ptr);
	void (GL_APIENTRY *glViewport)(GLint x, GLint y, GLsizei width, GLsizei height);
	void (GL_APIENTRY *glBlitFramebufferNV)(GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter);
	void (GL_APIENTRY *glBlitFramebufferANGLE)(GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1,
	                                           GLbitfield mask, GLenum filter);
	void (GL_APIENTRY *glTexImage3DOES)(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth,
	                                    GLint border, GLenum format, GLenum type, const GLvoid* pixels);
	void (GL_APIENTRY *glTexSubImage3DOES)(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const void *pixels);
	void (GL_APIENTRY *glCopyTexSubImage3DOES)(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLint x, GLint y, GLsizei width, GLsizei height);
	void (GL_APIENTRY *glCompressedTexImage3DOES)(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLsizei imageSize, const void *data);
	void (GL_APIENTRY *glCompressedTexSubImage3DOES)(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, const void *data);
	void (GL_APIENTRY *glFramebufferTexture3DOES)(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level, GLint zoffset);
	void (GL_APIENTRY *glEGLImageTargetTexture2DOES)(GLenum target, GLeglImageOES image);
	void (GL_APIENTRY *glEGLImageTargetRenderbufferStorageOES)(GLenum target, GLeglImageOES image);
	GLboolean (GL_APIENTRY *glIsRenderbufferOES)(GLuint renderbuffer);
	void (GL_APIENTRY *glBindRenderbufferOES)(GLenum target, GLuint renderbuffer);
	void (GL_APIENTRY *glDeleteRenderbuffersOES)(GLsizei n, const GLuint* renderbuffers);
	void (GL_APIENTRY *glGenRenderbuffersOES)(GLsizei n, GLuint* renderbuffers);
	void (GL_APIENTRY *glRenderbufferStorageOES)(GLenum target, GLenum internalformat, GLsizei width, GLsizei height);
	void (GL_APIENTRY *glGetRenderbufferParameterivOES)(GLenum target, GLenum pname, GLint* params);
	GLboolean (GL_APIENTRY *glIsFramebufferOES)(GLuint framebuffer);
	void (GL_APIENTRY *glBindFramebufferOES)(GLenum target, GLuint framebuffer);
	void (GL_APIENTRY *glDeleteFramebuffersOES)(GLsizei n, const GLuint* framebuffers);
	void (GL_APIENTRY *glGenFramebuffersOES)(GLsizei n, GLuint* framebuffers);
	GLenum (GL_APIENTRY *glCheckFramebufferStatusOES)(GLenum target);
	void (GL_APIENTRY *glFramebufferRenderbufferOES)(GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer);
	void (GL_APIENTRY *glFramebufferTexture2DOES)(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);
	void (GL_APIENTRY *glGetFramebufferAttachmentParameterivOES)(GLenum target, GLenum attachment, GLenum pname, GLint* params);
	void (GL_APIENTRY *glGenerateMipmapOES)(GLenum target);
	void (GL_APIENTRY *glDrawBuffersEXT)(GLsizei n, const GLenum *bufs);

	egl::Context *(*es2CreateContext)(egl::Display *display, const egl::Context *shareContext, const egl::Config *config);
	__eglMustCastToProperFunctionPointerType (*es2GetProcAddress)(const char *procname);
	egl::Image *(*createBackBuffer)(int width, int height, sw::Format format, int multiSampleDepth);
	egl::Image *(*createBackBufferFromClientBuffer)(const egl::ClientBuffer& clientBuffer);
	egl::Image *(*createDepthStencil)(int width, int height, sw::Format format, int multiSampleDepth);
	sw::FrameBuffer *(*createFrameBuffer)(void *nativeDisplay, EGLNativeWindowType window, int width, int height);
};

class LibGLESv2
{
public:
	LibGLESv2()
	{
	}

	~LibGLESv2()
	{
		freeLibrary(libGLESv2);
	}

	operator bool()
	{
		return loadExports() != nullptr;
	}

	LibGLESv2exports *operator->()
	{
		return loadExports();
	}

private:
	LibGLESv2exports *loadExports()
	{
		if(!loadLibraryAttempted && !libGLESv2)
		{
			#if defined(_WIN32)
				#if defined(__LP64__)
					const char *libGLESv2_lib[] = {"libswiftshader_libGLESv2.dll", "libGLESv2.dll", "lib64GLES_V2_translator.dll"};
				#else
					const char *libGLESv2_lib[] = {"libswiftshader_libGLESv2.dll", "libGLESv2.dll", "libGLES_V2_translator.dll"};
				#endif
			#elif defined(__ANDROID__)
				const char *libGLESv2_lib[] = {"libGLESv2_swiftshader.so", "libGLESv2_swiftshader.so"};
			#elif defined(__linux__)
				#if defined(__LP64__)
					const char *libGLESv2_lib[] = {"lib64GLES_V2_translator.so", "libGLESv2.so.2", "libGLESv2.so"};
				#else
					const char *libGLESv2_lib[] = {"libGLES_V2_translator.so", "libGLESv2.so.2", "libGLESv2.so"};
				#endif
			#elif defined(__APPLE__)
				#if defined(__LP64__)
					const char *libGLESv2_lib[] = {"libswiftshader_libGLESv2.dylib", "lib64GLES_V2_translator.dylib", "libGLESv2.dylib"};
				#else
					const char *libGLESv2_lib[] = {"libswiftshader_libGLESv2.dylib", "libGLES_V2_translator.dylib", "libGLESv2.dylib"};
				#endif
			#elif defined(__Fuchsia__)
				const char *libGLESv2_lib[] = {"libswiftshader_libGLESv2.so", "libGLESv2.so"};
			#else
				#error "libGLESv2::loadExports unimplemented for this platform"
			#endif

			std::string directory = getModuleDirectory();
			libGLESv2 = loadLibrary(directory, libGLESv2_lib, "libGLESv2_swiftshader");

			if(libGLESv2)
			{
				auto libGLESv2_swiftshader = (LibGLESv2exports *(*)())getProcAddress(libGLESv2, "libGLESv2_swiftshader");
				libGLESv2exports = libGLESv2_swiftshader();
			}

			loadLibraryAttempted = true;
		}

		return libGLESv2exports;
	}

	void *libGLESv2 = nullptr;
	LibGLESv2exports *libGLESv2exports = nullptr;
	bool loadLibraryAttempted = false;
};

#endif   // libGLESv2_hpp
