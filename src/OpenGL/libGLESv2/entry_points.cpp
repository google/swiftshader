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

// entry_points.cpp: GL entry points exports and definition

#include "main.h"
#include "entry_points.h"
#include "libEGL/main.h"

extern "C"
{
GL_APICALL void GL_APIENTRY glActiveTexture(GLenum texture)
{
	return gl::ActiveTexture(texture);
}

GL_APICALL void GL_APIENTRY glAttachShader(GLuint program, GLuint shader)
{
	return gl::AttachShader(program, shader);
}

GL_APICALL void GL_APIENTRY glBeginQueryEXT(GLenum target, GLuint name)
{
	return gl::BeginQueryEXT(target, name);
}

GL_APICALL void GL_APIENTRY glBindAttribLocation(GLuint program, GLuint index, const GLchar* name)
{
	return gl::BindAttribLocation(program, index, name);
}

GL_APICALL void GL_APIENTRY glBindBuffer(GLenum target, GLuint buffer)
{
	return gl::BindBuffer(target, buffer);
}

GL_APICALL void GL_APIENTRY glBindFramebuffer(GLenum target, GLuint framebuffer)
{
	return gl::BindFramebuffer(target, framebuffer);
}

GL_APICALL void GL_APIENTRY glBindFramebufferOES(GLenum target, GLuint framebuffer)
{
	return gl::BindFramebuffer(target, framebuffer);
}

GL_APICALL void GL_APIENTRY glBindRenderbuffer(GLenum target, GLuint renderbuffer)
{
	return gl::BindRenderbuffer(target, renderbuffer);
}

GL_APICALL void GL_APIENTRY glBindRenderbufferOES(GLenum target, GLuint renderbuffer)
{
	return gl::BindRenderbuffer(target, renderbuffer);
}

GL_APICALL void GL_APIENTRY glBindTexture(GLenum target, GLuint texture)
{
	return gl::BindTexture(target, texture);
}

GL_APICALL void GL_APIENTRY glBlendColor(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha)
{
	return gl::BlendColor(red, green, blue, alpha);
}

GL_APICALL void GL_APIENTRY glBlendEquation(GLenum mode)
{
	return gl::BlendEquation(mode);
}

GL_APICALL void GL_APIENTRY glBlendEquationSeparate(GLenum modeRGB, GLenum modeAlpha)
{
	return gl::BlendEquationSeparate(modeRGB, modeAlpha);
}

GL_APICALL void GL_APIENTRY glBlendFunc(GLenum sfactor, GLenum dfactor)
{
	return gl::BlendFunc(sfactor, dfactor);
}

GL_APICALL void GL_APIENTRY glBlendFuncSeparate(GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha)
{
	return gl::BlendFuncSeparate(srcRGB, dstRGB, srcAlpha, dstAlpha);
}

GL_APICALL void GL_APIENTRY glBufferData(GLenum target, GLsizeiptr size, const GLvoid* data, GLenum usage)
{
	return gl::BufferData(target, size, data, usage);
}

GL_APICALL void GL_APIENTRY glBufferSubData(GLenum target, GLintptr offset, GLsizeiptr size, const GLvoid* data)
{
	return gl::BufferSubData(target, offset, size, data);
}

GL_APICALL GLenum GL_APIENTRY glCheckFramebufferStatus(GLenum target)
{
	return gl::CheckFramebufferStatus(target);
}

GL_APICALL GLenum GL_APIENTRY glCheckFramebufferStatusOES(GLenum target)
{
	return gl::CheckFramebufferStatus(target);
}

GL_APICALL void GL_APIENTRY glClear(GLbitfield mask)
{
	return gl::Clear(mask);
}

GL_APICALL void GL_APIENTRY glClearColor(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha)
{
	return gl::ClearColor(red, green, blue, alpha);
}

GL_APICALL void GL_APIENTRY glClearDepthf(GLclampf depth)
{
	return gl::ClearDepthf(depth);
}

GL_APICALL void GL_APIENTRY glClearStencil(GLint s)
{
	return gl::ClearStencil(s);
}

GL_APICALL void GL_APIENTRY glColorMask(GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha)
{
	return gl::ColorMask(red, green, blue, alpha);
}

GL_APICALL void GL_APIENTRY glCompileShader(GLuint shader)
{
	return gl::CompileShader(shader);
}

GL_APICALL void GL_APIENTRY glCompressedTexImage2D(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height,
                                                   GLint border, GLsizei imageSize, const GLvoid* data)
{
	return gl::CompressedTexImage2D(target, level, internalformat, width, height, border, imageSize, data);
}

GL_APICALL void GL_APIENTRY glCompressedTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height,
                                                      GLenum format, GLsizei imageSize, const GLvoid* data)
{
	return gl::CompressedTexSubImage2D(target, level, xoffset, yoffset, width, height, format, imageSize, data);
}

GL_APICALL void GL_APIENTRY glCopyTexImage2D(GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border)
{
	return gl::CopyTexImage2D(target, level, internalformat, x, y, width, height, border);
}

GL_APICALL void GL_APIENTRY glCopyTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height)
{
	return gl::CopyTexSubImage2D(target, level, xoffset, yoffset, x, y, width, height);
}

GL_APICALL GLuint GL_APIENTRY glCreateProgram(void)
{
	return gl::CreateProgram();
}

GL_APICALL GLuint GL_APIENTRY glCreateShader(GLenum type)
{
	return gl::CreateShader(type);
}

GL_APICALL void GL_APIENTRY glCullFace(GLenum mode)
{
	return gl::CullFace(mode);
}

GL_APICALL void GL_APIENTRY glDeleteBuffers(GLsizei n, const GLuint* buffers)
{
	return gl::DeleteBuffers(n, buffers);
}

GL_APICALL void GL_APIENTRY glDeleteFencesNV(GLsizei n, const GLuint* fences)
{
	return gl::DeleteFencesNV(n, fences);
}

GL_APICALL void GL_APIENTRY glDeleteFramebuffers(GLsizei n, const GLuint* framebuffers)
{
	return gl::DeleteFramebuffers(n, framebuffers);
}

GL_APICALL void GL_APIENTRY glDeleteFramebuffersOES(GLsizei n, const GLuint* framebuffers)
{
	return gl::DeleteFramebuffers(n, framebuffers);
}

GL_APICALL void GL_APIENTRY glDeleteProgram(GLuint program)
{
	return gl::DeleteProgram(program);
}

GL_APICALL void GL_APIENTRY glDeleteQueriesEXT(GLsizei n, const GLuint *ids)
{
	return gl::DeleteQueriesEXT(n, ids);
}

GL_APICALL void GL_APIENTRY glDeleteRenderbuffers(GLsizei n, const GLuint* renderbuffers)
{
	return gl::DeleteRenderbuffers(n, renderbuffers);
}

GL_APICALL void GL_APIENTRY glDeleteRenderbuffersOES(GLsizei n, const GLuint* renderbuffers)
{
	return gl::DeleteRenderbuffers(n, renderbuffers);
}

GL_APICALL void GL_APIENTRY glDeleteShader(GLuint shader)
{
	return gl::DeleteShader(shader);
}

GL_APICALL void GL_APIENTRY glDeleteTextures(GLsizei n, const GLuint* textures)
{
	return gl::DeleteTextures(n, textures);
}

GL_APICALL void GL_APIENTRY glDepthFunc(GLenum func)
{
	return gl::DepthFunc(func);
}

GL_APICALL void GL_APIENTRY glDepthMask(GLboolean flag)
{
	return gl::DepthMask(flag);
}

GL_APICALL void GL_APIENTRY glDepthRangef(GLclampf zNear, GLclampf zFar)
{
	return gl::DepthRangef(zNear, zFar);
}

GL_APICALL void GL_APIENTRY glDetachShader(GLuint program, GLuint shader)
{
	return gl::DetachShader(program, shader);
}

GL_APICALL void GL_APIENTRY glDisable(GLenum cap)
{
	return gl::Disable(cap);
}

GL_APICALL void GL_APIENTRY glDisableVertexAttribArray(GLuint index)
{
	return gl::DisableVertexAttribArray(index);
}

GL_APICALL void GL_APIENTRY glDrawArrays(GLenum mode, GLint first, GLsizei count)
{
	return gl::DrawArrays(mode, first, count);
}

GL_APICALL void GL_APIENTRY glDrawElements(GLenum mode, GLsizei count, GLenum type, const GLvoid* indices)
{
	return gl::DrawElements(mode, count, type, indices);
}

GL_APICALL void GL_APIENTRY glDrawArraysInstancedEXT(GLenum mode, GLint first, GLsizei count, GLsizei instanceCount)
{
	return gl::DrawArraysInstancedEXT(mode, first, count, instanceCount);
}

GL_APICALL void GL_APIENTRY glDrawElementsInstancedEXT(GLenum mode, GLsizei count, GLenum type, const void *indices, GLsizei instanceCount)
{
	return gl::DrawElementsInstancedEXT(mode, count, type, indices, instanceCount);
}

GL_APICALL void GL_APIENTRY glVertexAttribDivisorEXT(GLuint index, GLuint divisor)
{
	return gl::VertexAttribDivisorEXT(index, divisor);
}

GL_APICALL void GL_APIENTRY glDrawArraysInstancedANGLE(GLenum mode, GLint first, GLsizei count, GLsizei instanceCount)
{
	return gl::DrawArraysInstancedANGLE(mode, first, count, instanceCount);
}

GL_APICALL void GL_APIENTRY glDrawElementsInstancedANGLE(GLenum mode, GLsizei count, GLenum type, const void *indices, GLsizei instanceCount)
{
	return gl::DrawElementsInstancedANGLE(mode, count, type, indices, instanceCount);
}

GL_APICALL void GL_APIENTRY glVertexAttribDivisorANGLE(GLuint index, GLuint divisor)
{
	return gl::VertexAttribDivisorANGLE(index, divisor);
}

GL_APICALL void GL_APIENTRY glEnable(GLenum cap)
{
	return gl::Enable(cap);
}

GL_APICALL void GL_APIENTRY glEnableVertexAttribArray(GLuint index)
{
	return gl::EnableVertexAttribArray(index);
}

GL_APICALL void GL_APIENTRY glEndQueryEXT(GLenum target)
{
	return gl::EndQueryEXT(target);
}

GL_APICALL void GL_APIENTRY glFinishFenceNV(GLuint fence)
{
	return gl::FinishFenceNV(fence);
}

GL_APICALL void GL_APIENTRY glFinish(void)
{
	return gl::Finish();
}

GL_APICALL void GL_APIENTRY glFlush(void)
{
	return gl::Flush();
}

GL_APICALL void GL_APIENTRY glFramebufferRenderbuffer(GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer)
{
	return gl::FramebufferRenderbuffer(target, attachment, renderbuffertarget, renderbuffer);
}

GL_APICALL void GL_APIENTRY glFramebufferRenderbufferOES(GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer)
{
	return gl::FramebufferRenderbuffer(target, attachment, renderbuffertarget, renderbuffer);
}

GL_APICALL void GL_APIENTRY glFramebufferTexture2D(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level)
{
	return gl::FramebufferTexture2D(target, attachment, textarget, texture, level);
}

GL_APICALL void GL_APIENTRY glFramebufferTexture2DOES(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level)
{
	return gl::FramebufferTexture2D(target, attachment, textarget, texture, level);
}

GL_APICALL void GL_APIENTRY glFrontFace(GLenum mode)
{
	return gl::FrontFace(mode);
}

GL_APICALL void GL_APIENTRY glGenBuffers(GLsizei n, GLuint* buffers)
{
	return gl::GenBuffers(n, buffers);
}

GL_APICALL void GL_APIENTRY glGenerateMipmap(GLenum target)
{
	return gl::GenerateMipmap(target);
}

GL_APICALL void GL_APIENTRY glGenerateMipmapOES(GLenum target)
{
	return gl::GenerateMipmap(target);
}

GL_APICALL void GL_APIENTRY glGenFencesNV(GLsizei n, GLuint* fences)
{
	return gl::GenFencesNV(n, fences);
}

GL_APICALL void GL_APIENTRY glGenFramebuffers(GLsizei n, GLuint* framebuffers)
{
	return gl::GenFramebuffers(n, framebuffers);
}

GL_APICALL void GL_APIENTRY glGenFramebuffersOES(GLsizei n, GLuint* framebuffers)
{
	return gl::GenFramebuffers(n, framebuffers);
}

GL_APICALL void GL_APIENTRY glGenQueriesEXT(GLsizei n, GLuint* ids)
{
	return gl::GenQueriesEXT(n, ids);
}

GL_APICALL void GL_APIENTRY glGenRenderbuffers(GLsizei n, GLuint* renderbuffers)
{
	return gl::GenRenderbuffers(n, renderbuffers);
}

GL_APICALL void GL_APIENTRY glGenRenderbuffersOES(GLsizei n, GLuint* renderbuffers)
{
	return gl::GenRenderbuffers(n, renderbuffers);
}

GL_APICALL void GL_APIENTRY glGenTextures(GLsizei n, GLuint* textures)
{
	return gl::GenTextures(n, textures);
}

GL_APICALL void GL_APIENTRY glGetActiveAttrib(GLuint program, GLuint index, GLsizei bufsize, GLsizei *length, GLint *size, GLenum *type, GLchar *name)
{
	return gl::GetActiveAttrib(program, index, bufsize, length, size, type, name);
}

GL_APICALL void GL_APIENTRY glGetActiveUniform(GLuint program, GLuint index, GLsizei bufsize, GLsizei* length, GLint* size, GLenum* type, GLchar* name)
{
	return gl::GetActiveUniform(program, index, bufsize, length, size, type, name);
}

GL_APICALL void GL_APIENTRY glGetAttachedShaders(GLuint program, GLsizei maxcount, GLsizei* count, GLuint* shaders)
{
	return gl::GetAttachedShaders(program, maxcount, count, shaders);
}

GL_APICALL int GL_APIENTRY glGetAttribLocation(GLuint program, const GLchar* name)
{
	return gl::GetAttribLocation(program, name);
}

GL_APICALL void GL_APIENTRY glGetBooleanv(GLenum pname, GLboolean* params)
{
	return gl::GetBooleanv(pname, params);
}

GL_APICALL void GL_APIENTRY glGetBufferParameteriv(GLenum target, GLenum pname, GLint* params)
{
	return gl::GetBufferParameteriv(target, pname, params);
}

GL_APICALL GLenum GL_APIENTRY glGetError(void)
{
	return gl::GetError();
}

GL_APICALL void GL_APIENTRY glGetFenceivNV(GLuint fence, GLenum pname, GLint *params)
{
	return gl::GetFenceivNV(fence, pname, params);
}

GL_APICALL void GL_APIENTRY glGetFloatv(GLenum pname, GLfloat* params)
{
	return gl::GetFloatv(pname, params);
}

GL_APICALL void GL_APIENTRY glGetFramebufferAttachmentParameteriv(GLenum target, GLenum attachment, GLenum pname, GLint* params)
{
	return gl::GetFramebufferAttachmentParameteriv(target, attachment, pname, params);
}

GL_APICALL void GL_APIENTRY glGetFramebufferAttachmentParameterivOES(GLenum target, GLenum attachment, GLenum pname, GLint* params)
{
	return gl::GetFramebufferAttachmentParameteriv(target, attachment, pname, params);
}

GL_APICALL GLenum GL_APIENTRY glGetGraphicsResetStatusEXT(void)
{
	return gl::GetGraphicsResetStatusEXT();
}

GL_APICALL void GL_APIENTRY glGetIntegerv(GLenum pname, GLint* params)
{
	return gl::GetIntegerv(pname, params);
}

GL_APICALL void GL_APIENTRY glGetProgramiv(GLuint program, GLenum pname, GLint* params)
{
	return gl::GetProgramiv(program, pname, params);
}

GL_APICALL void GL_APIENTRY glGetProgramInfoLog(GLuint program, GLsizei bufsize, GLsizei* length, GLchar* infolog)
{
	return gl::GetProgramInfoLog(program, bufsize, length, infolog);
}

GL_APICALL void GL_APIENTRY glGetQueryivEXT(GLenum target, GLenum pname, GLint *params)
{
	return gl::GetQueryivEXT(target, pname, params);
}

GL_APICALL void GL_APIENTRY glGetQueryObjectuivEXT(GLuint name, GLenum pname, GLuint *params)
{
	return gl::GetQueryObjectuivEXT(name, pname, params);
}

GL_APICALL void GL_APIENTRY glGetRenderbufferParameteriv(GLenum target, GLenum pname, GLint* params)
{
	return gl::GetRenderbufferParameteriv(target, pname, params);
}

GL_APICALL void GL_APIENTRY glGetRenderbufferParameterivOES(GLenum target, GLenum pname, GLint* params)
{
	return gl::GetRenderbufferParameteriv(target, pname, params);
}

GL_APICALL void GL_APIENTRY glGetShaderiv(GLuint shader, GLenum pname, GLint* params)
{
	return gl::GetShaderiv(shader, pname, params);
}

GL_APICALL void GL_APIENTRY glGetShaderInfoLog(GLuint shader, GLsizei bufsize, GLsizei* length, GLchar* infolog)
{
	return gl::GetShaderInfoLog(shader, bufsize, length, infolog);
}

GL_APICALL void GL_APIENTRY glGetShaderPrecisionFormat(GLenum shadertype, GLenum precisiontype, GLint* range, GLint* precision)
{
	return gl::GetShaderPrecisionFormat(shadertype, precisiontype, range, precision);
}

GL_APICALL void GL_APIENTRY glGetShaderSource(GLuint shader, GLsizei bufsize, GLsizei* length, GLchar* source)
{
	return gl::GetShaderSource(shader, bufsize, length, source);
}

GL_APICALL const GLubyte* GL_APIENTRY glGetString(GLenum name)
{
	return gl::GetString(name);
}

GL_APICALL void GL_APIENTRY glGetTexParameterfv(GLenum target, GLenum pname, GLfloat* params)
{
	return gl::GetTexParameterfv(target, pname, params);
}

GL_APICALL void GL_APIENTRY glGetTexParameteriv(GLenum target, GLenum pname, GLint* params)
{
	return gl::GetTexParameteriv(target, pname, params);
}

GL_APICALL void GL_APIENTRY glGetnUniformfvEXT(GLuint program, GLint location, GLsizei bufSize, GLfloat* params)
{
	return gl::GetnUniformfvEXT(program, location, bufSize, params);
}

GL_APICALL void GL_APIENTRY glGetUniformfv(GLuint program, GLint location, GLfloat* params)
{
	return gl::GetUniformfv(program, location, params);
}

GL_APICALL void GL_APIENTRY glGetnUniformivEXT(GLuint program, GLint location, GLsizei bufSize, GLint* params)
{
	return gl::GetnUniformivEXT(program, location, bufSize, params);
}

GL_APICALL void GL_APIENTRY glGetUniformiv(GLuint program, GLint location, GLint* params)
{
	return gl::GetUniformiv(program, location, params);
}

GL_APICALL int GL_APIENTRY glGetUniformLocation(GLuint program, const GLchar* name)
{
	return gl::GetUniformLocation(program, name);
}

GL_APICALL void GL_APIENTRY glGetVertexAttribfv(GLuint index, GLenum pname, GLfloat* params)
{
	return gl::GetVertexAttribfv(index, pname, params);
}

GL_APICALL void GL_APIENTRY glGetVertexAttribiv(GLuint index, GLenum pname, GLint* params)
{
	return gl::GetVertexAttribiv(index, pname, params);
}

GL_APICALL void GL_APIENTRY glGetVertexAttribPointerv(GLuint index, GLenum pname, GLvoid** pointer)
{
	return gl::GetVertexAttribPointerv(index, pname, pointer);
}

GL_APICALL void GL_APIENTRY glHint(GLenum target, GLenum mode)
{
	return gl::Hint(target, mode);
}

GL_APICALL GLboolean GL_APIENTRY glIsBuffer(GLuint buffer)
{
	return gl::IsBuffer(buffer);
}

GL_APICALL GLboolean GL_APIENTRY glIsEnabled(GLenum cap)
{
	return gl::IsEnabled(cap);
}

GL_APICALL GLboolean GL_APIENTRY glIsFenceNV(GLuint fence)
{
	return gl::IsFenceNV(fence);
}

GL_APICALL GLboolean GL_APIENTRY glIsFramebuffer(GLuint framebuffer)
{
	return gl::IsFramebuffer(framebuffer);
}

GL_APICALL GLboolean GL_APIENTRY glIsFramebufferOES(GLuint framebuffer)
{
	return gl::IsFramebuffer(framebuffer);
}

GL_APICALL GLboolean GL_APIENTRY glIsProgram(GLuint program)
{
	return gl::IsProgram(program);
}

GL_APICALL GLboolean GL_APIENTRY glIsQueryEXT(GLuint name)
{
	return gl::IsQueryEXT(name);
}

GL_APICALL GLboolean GL_APIENTRY glIsRenderbuffer(GLuint renderbuffer)
{
	return gl::IsRenderbuffer(renderbuffer);
}

GL_APICALL GLboolean GL_APIENTRY glIsRenderbufferOES(GLuint renderbuffer)
{
	return gl::IsRenderbuffer(renderbuffer);
}

GL_APICALL GLboolean GL_APIENTRY glIsShader(GLuint shader)
{
	return gl::IsShader(shader);
}

GL_APICALL GLboolean GL_APIENTRY glIsTexture(GLuint texture)
{
	return gl::IsTexture(texture);
}

GL_APICALL void GL_APIENTRY glLineWidth(GLfloat width)
{
	return gl::LineWidth(width);
}

GL_APICALL void GL_APIENTRY glLinkProgram(GLuint program)
{
	return gl::LinkProgram(program);
}

GL_APICALL void GL_APIENTRY glPixelStorei(GLenum pname, GLint param)
{
	return gl::PixelStorei(pname, param);
}

GL_APICALL void GL_APIENTRY glPolygonOffset(GLfloat factor, GLfloat units)
{
	return gl::PolygonOffset(factor, units);
}

GL_APICALL void GL_APIENTRY glReadnPixelsEXT(GLint x, GLint y, GLsizei width, GLsizei height,
                                             GLenum format, GLenum type, GLsizei bufSize, GLvoid *data)
{
	return gl::ReadnPixelsEXT(x, y, width, height, format, type, bufSize, data);
}

GL_APICALL void GL_APIENTRY glReadPixels(GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid* pixels)
{
	return gl::ReadPixels(x, y, width, height, format, type, pixels);
}

GL_APICALL void GL_APIENTRY glReleaseShaderCompiler(void)
{
	return gl::ReleaseShaderCompiler();
}

GL_APICALL void GL_APIENTRY glRenderbufferStorageMultisample(GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height)
{
	return gl::RenderbufferStorageMultisample(target, samples, internalformat, width, height);
}

GL_APICALL void GL_APIENTRY glRenderbufferStorageMultisampleANGLE(GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height)
{
	return gl::RenderbufferStorageMultisampleANGLE(target, samples, internalformat, width, height);
}

GL_APICALL void GL_APIENTRY glRenderbufferStorage(GLenum target, GLenum internalformat, GLsizei width, GLsizei height)
{
	return gl::RenderbufferStorage(target, internalformat, width, height);
}

GL_APICALL void GL_APIENTRY glRenderbufferStorageOES(GLenum target, GLenum internalformat, GLsizei width, GLsizei height)
{
	return gl::RenderbufferStorage(target, internalformat, width, height);
}

GL_APICALL void GL_APIENTRY glSampleCoverage(GLclampf value, GLboolean invert)
{
	return gl::SampleCoverage(value, invert);
}

GL_APICALL void GL_APIENTRY glSetFenceNV(GLuint fence, GLenum condition)
{
	return gl::SetFenceNV(fence, condition);
}

GL_APICALL void GL_APIENTRY glScissor(GLint x, GLint y, GLsizei width, GLsizei height)
{
	return gl::Scissor(x, y, width, height);
}

GL_APICALL void GL_APIENTRY glShaderBinary(GLsizei n, const GLuint* shaders, GLenum binaryformat, const GLvoid* binary, GLsizei length)
{
	return gl::ShaderBinary(n, shaders, binaryformat, binary, length);
}

GL_APICALL void GL_APIENTRY glShaderSource(GLuint shader, GLsizei count, const GLchar *const *string, const GLint *length)
{
	return gl::ShaderSource(shader, count, string, length);
}

GL_APICALL void GL_APIENTRY glStencilFunc(GLenum func, GLint ref, GLuint mask)
{
	return gl::StencilFunc(func, ref, mask);
}

GL_APICALL void GL_APIENTRY glStencilFuncSeparate(GLenum face, GLenum func, GLint ref, GLuint mask)
{
	return gl::StencilFuncSeparate(face, func, ref, mask);
}

GL_APICALL void GL_APIENTRY glStencilMask(GLuint mask)
{
	return gl::StencilMask(mask);
}

GL_APICALL void GL_APIENTRY glStencilMaskSeparate(GLenum face, GLuint mask)
{
	return gl::StencilMaskSeparate(face, mask);
}

GL_APICALL void GL_APIENTRY glStencilOp(GLenum fail, GLenum zfail, GLenum zpass)
{
	return gl::StencilOp(fail, zfail, zpass);
}

GL_APICALL void GL_APIENTRY glStencilOpSeparate(GLenum face, GLenum fail, GLenum zfail, GLenum zpass)
{
	return gl::StencilOpSeparate(face, fail, zfail, zpass);
}

GLboolean GL_APIENTRY glTestFenceNV(GLuint fence)
{
	return gl::TestFenceNV(fence);
}

GL_APICALL void GL_APIENTRY glTexImage2D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height,
                                         GLint border, GLenum format, GLenum type, const GLvoid* pixels)
{
	return gl::TexImage2D(target, level, internalformat, width, height, border, format, type, pixels);
}

GL_APICALL void GL_APIENTRY glTexParameterf(GLenum target, GLenum pname, GLfloat param)
{
	return gl::TexParameterf(target, pname, param);
}

GL_APICALL void GL_APIENTRY glTexParameterfv(GLenum target, GLenum pname, const GLfloat* params)
{
	return gl::TexParameterfv(target, pname, params);
}

GL_APICALL void GL_APIENTRY glTexParameteri(GLenum target, GLenum pname, GLint param)
{
	return gl::TexParameteri(target, pname, param);
}

GL_APICALL void GL_APIENTRY glTexParameteriv(GLenum target, GLenum pname, const GLint* params)
{
	return gl::TexParameteriv(target, pname, params);
}

GL_APICALL void GL_APIENTRY glTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height,
                                            GLenum format, GLenum type, const GLvoid* pixels)
{
	return gl::TexSubImage2D(target, level, xoffset, yoffset, width, height, format, type, pixels);
}

GL_APICALL void GL_APIENTRY glUniform1f(GLint location, GLfloat x)
{
	return gl::Uniform1f(location, x);
}

GL_APICALL void GL_APIENTRY glUniform1fv(GLint location, GLsizei count, const GLfloat* v)
{
	return gl::Uniform1fv(location, count, v);
}

GL_APICALL void GL_APIENTRY glUniform1i(GLint location, GLint x)
{
	return gl::Uniform1i(location, x);
}

GL_APICALL void GL_APIENTRY glUniform1iv(GLint location, GLsizei count, const GLint* v)
{
	return gl::Uniform1iv(location, count, v);
}

GL_APICALL void GL_APIENTRY glUniform2f(GLint location, GLfloat x, GLfloat y)
{
	return gl::Uniform2f(location, x, y);
}

GL_APICALL void GL_APIENTRY glUniform2fv(GLint location, GLsizei count, const GLfloat* v)
{
	return gl::Uniform2fv(location, count, v);
}

GL_APICALL void GL_APIENTRY glUniform2i(GLint location, GLint x, GLint y)
{
	return gl::Uniform2i(location, x, y);
}

GL_APICALL void GL_APIENTRY glUniform2iv(GLint location, GLsizei count, const GLint* v)
{
	return gl::Uniform2iv(location, count, v);
}

GL_APICALL void GL_APIENTRY glUniform3f(GLint location, GLfloat x, GLfloat y, GLfloat z)
{
	return gl::Uniform3f(location, x, y, z);
}

GL_APICALL void GL_APIENTRY glUniform3fv(GLint location, GLsizei count, const GLfloat* v)
{
	return gl::Uniform3fv(location, count, v);
}

GL_APICALL void GL_APIENTRY glUniform3i(GLint location, GLint x, GLint y, GLint z)
{
	return gl::Uniform3i(location, x, y, z);
}

GL_APICALL void GL_APIENTRY glUniform3iv(GLint location, GLsizei count, const GLint* v)
{
	return gl::Uniform3iv(location, count, v);
}

GL_APICALL void GL_APIENTRY glUniform4f(GLint location, GLfloat x, GLfloat y, GLfloat z, GLfloat w)
{
	return gl::Uniform4f(location, x, y, z, w);
}

GL_APICALL void GL_APIENTRY glUniform4fv(GLint location, GLsizei count, const GLfloat* v)
{
	return gl::Uniform4fv(location, count, v);
}

GL_APICALL void GL_APIENTRY glUniform4i(GLint location, GLint x, GLint y, GLint z, GLint w)
{
	return gl::Uniform4i(location, x, y, z, w);
}

GL_APICALL void GL_APIENTRY glUniform4iv(GLint location, GLsizei count, const GLint* v)
{
	return gl::Uniform4iv(location, count, v);
}

GL_APICALL void GL_APIENTRY glUniformMatrix2fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value)
{
	return gl::UniformMatrix2fv(location, count, transpose, value);
}

GL_APICALL void GL_APIENTRY glUniformMatrix3fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value)
{
	return gl::UniformMatrix3fv(location, count, transpose, value);
}

GL_APICALL void GL_APIENTRY glUniformMatrix4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value)
{
	return gl::UniformMatrix4fv(location, count, transpose, value);
}

GL_APICALL void GL_APIENTRY glUseProgram(GLuint program)
{
	return gl::UseProgram(program);
}

GL_APICALL void GL_APIENTRY glValidateProgram(GLuint program)
{
	return gl::ValidateProgram(program);
}

GL_APICALL void GL_APIENTRY glVertexAttrib1f(GLuint index, GLfloat x)
{
	return gl::VertexAttrib1f(index, x);
}

GL_APICALL void GL_APIENTRY glVertexAttrib1fv(GLuint index, const GLfloat* values)
{
	return gl::VertexAttrib1fv(index, values);
}

GL_APICALL void GL_APIENTRY glVertexAttrib2f(GLuint index, GLfloat x, GLfloat y)
{
	return gl::VertexAttrib2f(index, x, y);
}

GL_APICALL void GL_APIENTRY glVertexAttrib2fv(GLuint index, const GLfloat* values)
{
	return gl::VertexAttrib2fv(index, values);
}

GL_APICALL void GL_APIENTRY glVertexAttrib3f(GLuint index, GLfloat x, GLfloat y, GLfloat z)
{
	return gl::VertexAttrib3f(index, x, y, z);
}

GL_APICALL void GL_APIENTRY glVertexAttrib3fv(GLuint index, const GLfloat* values)
{
	return gl::VertexAttrib3fv(index, values);
}

GL_APICALL void GL_APIENTRY glVertexAttrib4f(GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w)
{
	return gl::VertexAttrib4f(index, x, y, z, w);
}

GL_APICALL void GL_APIENTRY glVertexAttrib4fv(GLuint index, const GLfloat* values)
{
	return gl::VertexAttrib4fv(index, values);
}

GL_APICALL void GL_APIENTRY glVertexAttribPointer(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid* ptr)
{
	return gl::VertexAttribPointer(index, size, type, normalized, stride, ptr);
}

GL_APICALL void GL_APIENTRY glViewport(GLint x, GLint y, GLsizei width, GLsizei height)
{
	return gl::Viewport(x, y, width, height);
}

GL_APICALL void GL_APIENTRY glBlitFramebufferNV(GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter)
{
	return gl::BlitFramebufferNV(srcX0, srcY0, srcX1, srcY1, dstX0, dstY0, dstX1, dstY1, mask, filter);
}

GL_APICALL void GL_APIENTRY glBlitFramebufferANGLE(GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1,
                                                   GLbitfield mask, GLenum filter)
{
	return gl::BlitFramebufferANGLE(srcX0, srcY0, srcX1, srcY1, dstX0, dstY0, dstX1, dstY1, mask, filter);
}

GL_APICALL void GL_APIENTRY glTexImage3DOES(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth,
                                            GLint border, GLenum format, GLenum type, const GLvoid* pixels)
{
	return gl::TexImage3DOES(target, level, internalformat, width, height, depth, border, format, type, pixels);
}

GL_APICALL void GL_APIENTRY glTexSubImage3DOES(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const void *pixels)
{
	return gl::TexSubImage3DOES(target, level, xoffset, yoffset, zoffset, width, height, depth, format, type, pixels);
}

GL_APICALL void GL_APIENTRY glCopyTexSubImage3DOES(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLint x, GLint y, GLsizei width, GLsizei height)
{
	return gl::CopyTexSubImage3DOES(target, level, xoffset, yoffset, zoffset, x, y, width, height);
}

GL_APICALL void GL_APIENTRY glCompressedTexImage3DOES(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLsizei imageSize, const void *data)
{
	return gl::CompressedTexImage3DOES(target, level,internalformat, width, height, depth, border, imageSize, data);
}

GL_APICALL void GL_APIENTRY glCompressedTexSubImage3DOES(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, const void *data)
{
	return gl::CompressedTexSubImage3DOES(target, level, xoffset, yoffset, zoffset, width, height, depth, format, imageSize, data);
}

GL_APICALL void GL_APIENTRY glFramebufferTexture3DOES(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level, GLint zoffset)
{
	return gl::FramebufferTexture3DOES(target, attachment, textarget, texture, level, zoffset);
}

GL_APICALL void GL_APIENTRY glEGLImageTargetTexture2DOES(GLenum target, GLeglImageOES image)
{
	return gl::EGLImageTargetTexture2DOES(target, image);
}

GL_APICALL void GL_APIENTRY glEGLImageTargetRenderbufferStorageOES(GLenum target, GLeglImageOES image)
{
	return gl::EGLImageTargetRenderbufferStorageOES(target, image);
}

GL_APICALL void GL_APIENTRY glDrawBuffersEXT(GLsizei n, const GLenum *bufs)
{
	return gl::DrawBuffersEXT(n, bufs);
}

GL_APICALL void GL_APIENTRY glReadBuffer(GLenum src)
{
	return gl::ReadBuffer(src);
}

GL_APICALL void GL_APIENTRY glDrawRangeElements(GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const void *indices)
{
	return gl::DrawRangeElements(mode, start, end, count, type, indices);
}

GL_APICALL void GL_APIENTRY glTexImage3D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, const void *data)
{
	return gl::TexImage3D(target, level, internalformat, width, height, depth, border, format, type, data);
}

GL_APICALL void GL_APIENTRY glTexSubImage3D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const void *data)
{
	return gl::TexSubImage3D(target, level, xoffset, yoffset, zoffset, width, height, depth, format, type, data);
}

GL_APICALL void GL_APIENTRY glCopyTexSubImage3D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLint x, GLint y, GLsizei width, GLsizei height)
{
	return gl::CopyTexSubImage3D(target, level, xoffset, yoffset, zoffset, x, y, width, height);
}

GL_APICALL void GL_APIENTRY glCompressedTexImage3D(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLsizei imageSize, const void *data)
{
	return gl::CompressedTexImage3D(target, level, internalformat, width, height, depth, border, imageSize, data);
}

GL_APICALL void GL_APIENTRY glCompressedTexSubImage3D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, const void *data)
{
	return gl::CompressedTexSubImage3D(target, level, xoffset, yoffset, zoffset, width, height, depth, format, imageSize, data);
}

GL_APICALL void GL_APIENTRY glGenQueries(GLsizei n, GLuint *ids)
{
	return gl::GenQueries(n, ids);
}

GL_APICALL void GL_APIENTRY glDeleteQueries(GLsizei n, const GLuint *ids)
{
	return gl::DeleteQueries(n, ids);
}

GL_APICALL GLboolean GL_APIENTRY glIsQuery(GLuint id)
{
	return gl::IsQuery(id);
}

GL_APICALL void GL_APIENTRY glBeginQuery(GLenum target, GLuint id)
{
	return gl::BeginQuery(target, id);
}

GL_APICALL void GL_APIENTRY glEndQuery(GLenum target)
{
	return gl::EndQuery(target);
}

GL_APICALL void GL_APIENTRY glGetQueryiv(GLenum target, GLenum pname, GLint *params)
{
	return gl::GetQueryiv(target, pname, params);
}

GL_APICALL void GL_APIENTRY glGetQueryObjectuiv(GLuint id, GLenum pname, GLuint *params)
{
	return gl::GetQueryObjectuiv(id, pname, params);
}

GL_APICALL GLboolean GL_APIENTRY glUnmapBuffer(GLenum target)
{
	return gl::UnmapBuffer(target);
}

GL_APICALL void GL_APIENTRY glGetBufferPointerv(GLenum target, GLenum pname, void **params)
{
	return gl::GetBufferPointerv(target, pname, params);
}

GL_APICALL void GL_APIENTRY glDrawBuffers(GLsizei n, const GLenum *bufs)
{
	return gl::DrawBuffers(n, bufs);
}

GL_APICALL void GL_APIENTRY glUniformMatrix2x3fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value)
{
	return gl::UniformMatrix2x3fv(location, count, transpose, value);
}

GL_APICALL void GL_APIENTRY glUniformMatrix3x2fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value)
{
	return gl::UniformMatrix3x2fv(location, count, transpose, value);
}

GL_APICALL void GL_APIENTRY glUniformMatrix2x4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value)
{
	return gl::UniformMatrix2x4fv(location, count, transpose, value);
}

GL_APICALL void GL_APIENTRY glUniformMatrix4x2fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value)
{
	return gl::UniformMatrix4x2fv(location, count, transpose, value);
}

GL_APICALL void GL_APIENTRY glUniformMatrix3x4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value)
{
	return gl::UniformMatrix3x4fv(location, count, transpose, value);
}

GL_APICALL void GL_APIENTRY glUniformMatrix4x3fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value)
{
	return gl::UniformMatrix4x3fv(location, count, transpose, value);
}

GL_APICALL void GL_APIENTRY glBlitFramebuffer(GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter)
{
	return gl::BlitFramebuffer(srcX0, srcY0, srcX1, srcY1, dstX0, dstY0, dstX1, dstY1, mask, filter);
}

GL_APICALL void GL_APIENTRY glFramebufferTextureLayer(GLenum target, GLenum attachment, GLuint texture, GLint level, GLint layer)
{
	return gl::FramebufferTextureLayer(target, attachment, texture, level, layer);
}

GL_APICALL void *GL_APIENTRY glMapBufferRange(GLenum target, GLintptr offset, GLsizeiptr length, GLbitfield access)
{
	return gl::MapBufferRange(target, offset, length, access);
}

GL_APICALL void GL_APIENTRY glFlushMappedBufferRange(GLenum target, GLintptr offset, GLsizeiptr length)
{
	return gl::FlushMappedBufferRange(target, offset, length);
}

GL_APICALL void GL_APIENTRY glBindVertexArray(GLuint array)
{
	return gl::BindVertexArray(array);
}

GL_APICALL void GL_APIENTRY glBindVertexArrayOES(GLuint array)
{
	return gl::BindVertexArrayOES(array);
}

GL_APICALL void GL_APIENTRY glDeleteVertexArrays(GLsizei n, const GLuint *arrays)
{
	return gl::DeleteVertexArrays(n, arrays);
}

GL_APICALL void GL_APIENTRY glDeleteVertexArraysOES(GLsizei n, const GLuint *arrays)
{
	return gl::DeleteFramebuffersOES(n, arrays);
}

GL_APICALL void GL_APIENTRY glGenVertexArrays(GLsizei n, GLuint *arrays)
{
	return gl::GenVertexArrays(n, arrays);
}

GL_APICALL void GL_APIENTRY glGenVertexArraysOES(GLsizei n, GLuint *arrays)
{
	return gl::GenVertexArraysOES(n, arrays);
}

GL_APICALL GLboolean GL_APIENTRY glIsVertexArray(GLuint array)
{
	return gl::IsVertexArray(array);
}

GL_APICALL GLboolean GL_APIENTRY glIsVertexArrayOES(GLuint array)
{
	return gl::IsVertexArrayOES(array);
}

GL_APICALL void GL_APIENTRY glGetIntegeri_v(GLenum target, GLuint index, GLint *data)
{
	return gl::GetIntegeri_v(target, index, data);
}

GL_APICALL void GL_APIENTRY glBeginTransformFeedback(GLenum primitiveMode)
{
	return gl::BeginTransformFeedback(primitiveMode);
}

GL_APICALL void GL_APIENTRY glEndTransformFeedback(void)
{
	return gl::EndTransformFeedback();
}

GL_APICALL void GL_APIENTRY glBindBufferRange(GLenum target, GLuint index, GLuint buffer, GLintptr offset, GLsizeiptr size)
{
	return gl::BindBufferRange(target, index, buffer, offset, size);
}

GL_APICALL void GL_APIENTRY glBindBufferBase(GLenum target, GLuint index, GLuint buffer)
{
	return gl::BindBufferBase(target, index, buffer);
}

GL_APICALL void GL_APIENTRY glTransformFeedbackVaryings(GLuint program, GLsizei count, const GLchar *const *varyings, GLenum bufferMode)
{
	return gl::TransformFeedbackVaryings(program, count, varyings, bufferMode);
}

GL_APICALL void GL_APIENTRY glGetTransformFeedbackVarying(GLuint program, GLuint index, GLsizei bufSize, GLsizei *length, GLsizei *size, GLenum *type, GLchar *name)
{
	return gl::GetTransformFeedbackVarying(program, index, bufSize, length, size, type, name);
}

GL_APICALL void GL_APIENTRY glVertexAttribIPointer(GLuint index, GLint size, GLenum type, GLsizei stride, const void *pointer)
{
	return gl::VertexAttribIPointer(index, size, type, stride, pointer);
}

GL_APICALL void GL_APIENTRY glGetVertexAttribIiv(GLuint index, GLenum pname, GLint *params)
{
	return gl::GetVertexAttribIiv(index, pname, params);
}

GL_APICALL void GL_APIENTRY glGetVertexAttribIuiv(GLuint index, GLenum pname, GLuint *params)
{
	return gl::GetVertexAttribIuiv(index, pname, params);
}

GL_APICALL void GL_APIENTRY glVertexAttribI4i(GLuint index, GLint x, GLint y, GLint z, GLint w)
{
	return gl::VertexAttribI4i(index, x, y, z, w);
}

GL_APICALL void GL_APIENTRY glVertexAttribI4ui(GLuint index, GLuint x, GLuint y, GLuint z, GLuint w)
{
	return gl::VertexAttribI4ui(index, x, y, z, w);
}

GL_APICALL void GL_APIENTRY glVertexAttribI4iv(GLuint index, const GLint *v)
{
	return gl::VertexAttribI4iv(index, v);
}

GL_APICALL void GL_APIENTRY glVertexAttribI4uiv(GLuint index, const GLuint *v)
{
	return gl::VertexAttribI4uiv(index, v);
}

GL_APICALL void GL_APIENTRY glGetUniformuiv(GLuint program, GLint location, GLuint *params)
{
	return gl::GetUniformuiv(program, location, params);
}

GL_APICALL GLint GL_APIENTRY glGetFragDataLocation(GLuint program, const GLchar *name)
{
	return gl::GetFragDataLocation(program, name);
}

GL_APICALL void GL_APIENTRY glUniform1ui(GLint location, GLuint v0)
{
	return gl::Uniform1ui(location, v0);
}

GL_APICALL void GL_APIENTRY glUniform2ui(GLint location, GLuint v0, GLuint v1)
{
	return gl::Uniform2ui(location, v0, v1);
}

GL_APICALL void GL_APIENTRY glUniform3ui(GLint location, GLuint v0, GLuint v1, GLuint v2)
{
	return gl::Uniform3ui(location, v0, v1, v2);
}

GL_APICALL void GL_APIENTRY glUniform4ui(GLint location, GLuint v0, GLuint v1, GLuint v2, GLuint v3)
{
	return gl::Uniform4ui(location, v0, v1, v2, v3);
}

GL_APICALL void GL_APIENTRY glUniform1uiv(GLint location, GLsizei count, const GLuint *value)
{
	return gl::Uniform1uiv(location, count, value);
}

GL_APICALL void GL_APIENTRY glUniform2uiv(GLint location, GLsizei count, const GLuint *value)
{
	return gl::Uniform2uiv(location, count, value);
}

GL_APICALL void GL_APIENTRY glUniform3uiv(GLint location, GLsizei count, const GLuint *value)
{
	return gl::Uniform3uiv(location, count, value);
}

GL_APICALL void GL_APIENTRY glUniform4uiv(GLint location, GLsizei count, const GLuint *value)
{
	return gl::Uniform4uiv(location, count, value);
}

GL_APICALL void GL_APIENTRY glClearBufferiv(GLenum buffer, GLint drawbuffer, const GLint *value)
{
	return gl::ClearBufferiv(buffer, drawbuffer, value);
}

GL_APICALL void GL_APIENTRY glClearBufferuiv(GLenum buffer, GLint drawbuffer, const GLuint *value)
{
	return gl::ClearBufferuiv(buffer, drawbuffer, value);
}

GL_APICALL void GL_APIENTRY glClearBufferfv(GLenum buffer, GLint drawbuffer, const GLfloat *value)
{
	return gl::ClearBufferfv(buffer, drawbuffer, value);
}

GL_APICALL void GL_APIENTRY glClearBufferfi(GLenum buffer, GLint drawbuffer, GLfloat depth, GLint stencil)
{
	return gl::ClearBufferfi(buffer, drawbuffer, depth, stencil);
}

GL_APICALL const GLubyte *GL_APIENTRY glGetStringi(GLenum name, GLuint index)
{
	return gl::GetStringi(name, index);
}

GL_APICALL void GL_APIENTRY glCopyBufferSubData(GLenum readTarget, GLenum writeTarget, GLintptr readOffset, GLintptr writeOffset, GLsizeiptr size)
{
	return gl::CopyBufferSubData(readTarget, writeTarget, readOffset, writeOffset, size);
}

GL_APICALL void GL_APIENTRY glGetUniformIndices(GLuint program, GLsizei uniformCount, const GLchar *const *uniformNames, GLuint *uniformIndices)
{
	return gl::GetUniformIndices(program, uniformCount, uniformNames, uniformIndices);
}

GL_APICALL void GL_APIENTRY glGetActiveUniformsiv(GLuint program, GLsizei uniformCount, const GLuint *uniformIndices, GLenum pname, GLint *params)
{
	return gl::GetActiveUniformsiv(program, uniformCount, uniformIndices, pname, params);
}

GL_APICALL GLuint GL_APIENTRY glGetUniformBlockIndex(GLuint program, const GLchar *uniformBlockName)
{
	return gl::GetUniformBlockIndex(program, uniformBlockName);
}

GL_APICALL void GL_APIENTRY glGetActiveUniformBlockiv(GLuint program, GLuint uniformBlockIndex, GLenum pname, GLint *params)
{
	return gl::GetActiveUniformBlockiv(program, uniformBlockIndex, pname, params);
}

GL_APICALL void GL_APIENTRY glGetActiveUniformBlockName(GLuint program, GLuint uniformBlockIndex, GLsizei bufSize, GLsizei *length, GLchar *uniformBlockName)
{
	return gl::GetActiveUniformBlockName(program, uniformBlockIndex, bufSize, length, uniformBlockName);
}

GL_APICALL void GL_APIENTRY glUniformBlockBinding(GLuint program, GLuint uniformBlockIndex, GLuint uniformBlockBinding)
{
	return gl::UniformBlockBinding(program, uniformBlockIndex, uniformBlockBinding);
}

GL_APICALL void GL_APIENTRY glDrawArraysInstanced(GLenum mode, GLint first, GLsizei count, GLsizei instanceCount)
{
	return gl::DrawArraysInstanced(mode, first, count, instanceCount);
}

GL_APICALL void GL_APIENTRY glDrawElementsInstanced(GLenum mode, GLsizei count, GLenum type, const void *indices, GLsizei instanceCount)
{
	return gl::DrawElementsInstanced(mode, count, type, indices, instanceCount);
}

GL_APICALL GLsync GL_APIENTRY glFenceSync(GLenum condition, GLbitfield flags)
{
	return gl::FenceSync(condition, flags);
}

GL_APICALL GLboolean GL_APIENTRY glIsSync(GLsync sync)
{
	return gl::IsSync(sync);
}

GL_APICALL void GL_APIENTRY glDeleteSync(GLsync sync)
{
	return gl::DeleteSync(sync);
}

GL_APICALL GLenum GL_APIENTRY glClientWaitSync(GLsync sync, GLbitfield flags, GLuint64 timeout)
{
	return gl::ClientWaitSync(sync, flags, timeout);
}

GL_APICALL void GL_APIENTRY glWaitSync(GLsync sync, GLbitfield flags, GLuint64 timeout)
{
	return gl::WaitSync(sync, flags, timeout);
}

GL_APICALL void GL_APIENTRY glGetInteger64v(GLenum pname, GLint64 *data)
{
	return gl::GetInteger64v(pname, data);
}

GL_APICALL void GL_APIENTRY glGetSynciv(GLsync sync, GLenum pname, GLsizei bufSize, GLsizei *length, GLint *values)
{
	return gl::GetSynciv(sync, pname, bufSize, length, values);
}

GL_APICALL void GL_APIENTRY glGetInteger64i_v(GLenum target, GLuint index, GLint64 *data)
{
	return gl::GetInteger64i_v(target, index, data);
}

GL_APICALL void GL_APIENTRY glGetBufferParameteri64v(GLenum target, GLenum pname, GLint64 *params)
{
	return gl::GetBufferParameteri64v(target, pname, params);
}

GL_APICALL void GL_APIENTRY glGenSamplers(GLsizei count, GLuint *samplers)
{
	return gl::GenSamplers(count, samplers);
}

GL_APICALL void GL_APIENTRY glDeleteSamplers(GLsizei count, const GLuint *samplers)
{
	return gl::DeleteSamplers(count, samplers);
}

GL_APICALL GLboolean GL_APIENTRY glIsSampler(GLuint sampler)
{
	return gl::IsSampler(sampler);
}

GL_APICALL void GL_APIENTRY glBindSampler(GLuint unit, GLuint sampler)
{
	return gl::BindSampler(unit, sampler);
}

GL_APICALL void GL_APIENTRY glSamplerParameteri(GLuint sampler, GLenum pname, GLint param)
{
	return gl::SamplerParameteri(sampler, pname, param);
}

GL_APICALL void GL_APIENTRY glSamplerParameteriv(GLuint sampler, GLenum pname, const GLint *param)
{
	return gl::SamplerParameteriv(sampler, pname, param);
}

GL_APICALL void GL_APIENTRY glSamplerParameterf(GLuint sampler, GLenum pname, GLfloat param)
{
	return gl::SamplerParameterf(sampler, pname, param);
}

GL_APICALL void GL_APIENTRY glSamplerParameterfv(GLuint sampler, GLenum pname, const GLfloat *param)
{
	return gl::SamplerParameterfv(sampler, pname, param);
}

GL_APICALL void GL_APIENTRY glGetSamplerParameteriv(GLuint sampler, GLenum pname, GLint *params)
{
	return gl::GetSamplerParameteriv(sampler, pname, params);
}

GL_APICALL void GL_APIENTRY glGetSamplerParameterfv(GLuint sampler, GLenum pname, GLfloat *params)
{
	return gl::GetSamplerParameterfv(sampler, pname, params);
}

GL_APICALL void GL_APIENTRY glVertexAttribDivisor(GLuint index, GLuint divisor)
{
	return gl::VertexAttribDivisor(index, divisor);
}

GL_APICALL void GL_APIENTRY glBindTransformFeedback(GLenum target, GLuint id)
{
	return gl::BindTransformFeedback(target, id);
}

GL_APICALL void GL_APIENTRY glDeleteTransformFeedbacks(GLsizei n, const GLuint *ids)
{
	return gl::DeleteTransformFeedbacks(n, ids);
}

GL_APICALL void GL_APIENTRY glGenTransformFeedbacks(GLsizei n, GLuint *ids)
{
	return gl::GenTransformFeedbacks(n, ids);
}

GL_APICALL GLboolean GL_APIENTRY glIsTransformFeedback(GLuint id)
{
	return gl::IsTransformFeedback(id);
}

GL_APICALL void GL_APIENTRY glPauseTransformFeedback(void)
{
	return gl::PauseTransformFeedback();
}

GL_APICALL void GL_APIENTRY glResumeTransformFeedback(void)
{
	return gl::ResumeTransformFeedback();
}

GL_APICALL void GL_APIENTRY glGetProgramBinary(GLuint program, GLsizei bufSize, GLsizei *length, GLenum *binaryFormat, void *binary)
{
	return gl::GetProgramBinary(program, bufSize, length, binaryFormat, binary);
}

GL_APICALL void GL_APIENTRY glProgramBinary(GLuint program, GLenum binaryFormat, const void *binary, GLsizei length)
{
	return gl::ProgramBinary(program, binaryFormat, binary, length);
}

GL_APICALL void GL_APIENTRY glProgramParameteri(GLuint program, GLenum pname, GLint value)
{
	return gl::ProgramParameteri(program, pname, value);
}

GL_APICALL void GL_APIENTRY glInvalidateFramebuffer(GLenum target, GLsizei numAttachments, const GLenum *attachments)
{
	return gl::InvalidateFramebuffer(target, numAttachments, attachments);
}

GL_APICALL void GL_APIENTRY glInvalidateSubFramebuffer(GLenum target, GLsizei numAttachments, const GLenum *attachments, GLint x, GLint y, GLsizei width, GLsizei height)
{
	return gl::InvalidateSubFramebuffer(target, numAttachments, attachments, x, y, width, height);
}

GL_APICALL void GL_APIENTRY glTexStorage2D(GLenum target, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height)
{
	return gl::TexStorage2D(target, levels, internalformat, width, height);
}

GL_APICALL void GL_APIENTRY glTexStorage3D(GLenum target, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth)
{
	return gl::TexStorage3D(target, levels, internalformat, width, height, depth);
}

GL_APICALL void GL_APIENTRY glGetInternalformativ(GLenum target, GLenum internalformat, GLenum pname, GLsizei bufSize, GLint *params)
{
	return gl::GetInternalformativ(target, internalformat, pname, bufSize, params);
}
}

egl::Context *es2CreateContext(egl::Display *display, const egl::Context *shareContext, const egl::Config *config);
extern "C" __eglMustCastToProperFunctionPointerType es2GetProcAddress(const char *procname);
egl::Image *createBackBuffer(int width, int height, sw::Format format, int multiSampleDepth);
egl::Image *createBackBufferFromClientBuffer(const egl::ClientBuffer& clientBuffer);
egl::Image *createDepthStencil(int width, int height, sw::Format format, int multiSampleDepth);
sw::FrameBuffer *createFrameBuffer(void *nativeDisplay, EGLNativeWindowType window, int width, int height);

LibGLESv2exports::LibGLESv2exports()
{
	this->glActiveTexture = gl::ActiveTexture;
	this->glAttachShader = gl::AttachShader;
	this->glBeginQueryEXT = gl::BeginQueryEXT;
	this->glBindAttribLocation = gl::BindAttribLocation;
	this->glBindBuffer = gl::BindBuffer;
	this->glBindFramebuffer = gl::BindFramebuffer;
	this->glBindRenderbuffer = gl::BindRenderbuffer;
	this->glBindTexture = gl::BindTexture;
	this->glBlendColor = gl::BlendColor;
	this->glBlendEquation = gl::BlendEquation;
	this->glBlendEquationSeparate = gl::BlendEquationSeparate;
	this->glBlendFunc = gl::BlendFunc;
	this->glBlendFuncSeparate = gl::BlendFuncSeparate;
	this->glBufferData = gl::BufferData;
	this->glBufferSubData = gl::BufferSubData;
	this->glCheckFramebufferStatus = gl::CheckFramebufferStatus;
	this->glClear = gl::Clear;
	this->glClearColor = gl::ClearColor;
	this->glClearDepthf = gl::ClearDepthf;
	this->glClearStencil = gl::ClearStencil;
	this->glColorMask = gl::ColorMask;
	this->glCompileShader = gl::CompileShader;
	this->glCompressedTexImage2D = gl::CompressedTexImage2D;
	this->glCompressedTexSubImage2D = gl::CompressedTexSubImage2D;
	this->glCopyTexImage2D = gl::CopyTexImage2D;
	this->glCopyTexSubImage2D = gl::CopyTexSubImage2D;
	this->glCreateProgram = gl::CreateProgram;
	this->glCreateShader = gl::CreateShader;
	this->glCullFace = gl::CullFace;
	this->glDeleteBuffers = gl::DeleteBuffers;
	this->glDeleteFencesNV = gl::DeleteFencesNV;
	this->glDeleteFramebuffers = gl::DeleteFramebuffers;
	this->glDeleteProgram = gl::DeleteProgram;
	this->glDeleteQueriesEXT = gl::DeleteQueriesEXT;
	this->glDeleteRenderbuffers = gl::DeleteRenderbuffers;
	this->glDeleteShader = gl::DeleteShader;
	this->glDeleteTextures = gl::DeleteTextures;
	this->glDepthFunc = gl::DepthFunc;
	this->glDepthMask = gl::DepthMask;
	this->glDepthRangef = gl::DepthRangef;
	this->glDetachShader = gl::DetachShader;
	this->glDisable = gl::Disable;
	this->glDisableVertexAttribArray = gl::DisableVertexAttribArray;
	this->glDrawArrays = gl::DrawArrays;
	this->glDrawElements = gl::DrawElements;
	this->glDrawArraysInstancedEXT = gl::DrawArraysInstancedEXT;
	this->glDrawElementsInstancedEXT = gl::DrawElementsInstancedEXT;
	this->glVertexAttribDivisorEXT = gl::VertexAttribDivisorEXT;
	this->glDrawArraysInstancedANGLE = gl::DrawArraysInstancedANGLE;
	this->glDrawElementsInstancedANGLE = gl::DrawElementsInstancedANGLE;
	this->glVertexAttribDivisorANGLE = gl::VertexAttribDivisorANGLE;
	this->glEnable = gl::Enable;
	this->glEnableVertexAttribArray = gl::EnableVertexAttribArray;
	this->glEndQueryEXT = gl::EndQueryEXT;
	this->glFinishFenceNV = gl::FinishFenceNV;
	this->glFinish = gl::Finish;
	this->glFlush = gl::Flush;
	this->glFramebufferRenderbuffer = gl::FramebufferRenderbuffer;
	this->glFramebufferTexture2D = gl::FramebufferTexture2D;
	this->glFrontFace = gl::FrontFace;
	this->glGenBuffers = gl::GenBuffers;
	this->glGenerateMipmap = gl::GenerateMipmap;
	this->glGenFencesNV = gl::GenFencesNV;
	this->glGenFramebuffers = gl::GenFramebuffers;
	this->glGenQueriesEXT = gl::GenQueriesEXT;
	this->glGenRenderbuffers = gl::GenRenderbuffers;
	this->glGenTextures = gl::GenTextures;
	this->glGetActiveAttrib = gl::GetActiveAttrib;
	this->glGetActiveUniform = gl::GetActiveUniform;
	this->glGetAttachedShaders = gl::GetAttachedShaders;
	this->glGetAttribLocation = gl::GetAttribLocation;
	this->glGetBooleanv = gl::GetBooleanv;
	this->glGetBufferParameteriv = gl::GetBufferParameteriv;
	this->glGetError = gl::GetError;
	this->glGetFenceivNV = gl::GetFenceivNV;
	this->glGetFloatv = gl::GetFloatv;
	this->glGetFramebufferAttachmentParameteriv = gl::GetFramebufferAttachmentParameteriv;
	this->glGetGraphicsResetStatusEXT = gl::GetGraphicsResetStatusEXT;
	this->glGetIntegerv = gl::GetIntegerv;
	this->glGetProgramiv = gl::GetProgramiv;
	this->glGetProgramInfoLog = gl::GetProgramInfoLog;
	this->glGetQueryivEXT = gl::GetQueryivEXT;
	this->glGetQueryObjectuivEXT = gl::GetQueryObjectuivEXT;
	this->glGetRenderbufferParameteriv = gl::GetRenderbufferParameteriv;
	this->glGetShaderiv = gl::GetShaderiv;
	this->glGetShaderInfoLog = gl::GetShaderInfoLog;
	this->glGetShaderPrecisionFormat = gl::GetShaderPrecisionFormat;
	this->glGetShaderSource = gl::GetShaderSource;
	this->glGetString = gl::GetString;
	this->glGetTexParameterfv = gl::GetTexParameterfv;
	this->glGetTexParameteriv = gl::GetTexParameteriv;
	this->glGetnUniformfvEXT = gl::GetnUniformfvEXT;
	this->glGetUniformfv = gl::GetUniformfv;
	this->glGetnUniformivEXT = gl::GetnUniformivEXT;
	this->glGetUniformiv = gl::GetUniformiv;
	this->glGetUniformLocation = gl::GetUniformLocation;
	this->glGetVertexAttribfv = gl::GetVertexAttribfv;
	this->glGetVertexAttribiv = gl::GetVertexAttribiv;
	this->glGetVertexAttribPointerv = gl::GetVertexAttribPointerv;
	this->glHint = gl::Hint;
	this->glIsBuffer = gl::IsBuffer;
	this->glIsEnabled = gl::IsEnabled;
	this->glIsFenceNV = gl::IsFenceNV;
	this->glIsFramebuffer = gl::IsFramebuffer;
	this->glIsProgram = gl::IsProgram;
	this->glIsQueryEXT = gl::IsQueryEXT;
	this->glIsRenderbuffer = gl::IsRenderbuffer;
	this->glIsShader = gl::IsShader;
	this->glIsTexture = gl::IsTexture;
	this->glLineWidth = gl::LineWidth;
	this->glLinkProgram = gl::LinkProgram;
	this->glPixelStorei = gl::PixelStorei;
	this->glPolygonOffset = gl::PolygonOffset;
	this->glReadnPixelsEXT = gl::ReadnPixelsEXT;
	this->glReadPixels = gl::ReadPixels;
	this->glReleaseShaderCompiler = gl::ReleaseShaderCompiler;
	this->glRenderbufferStorageMultisample = gl::RenderbufferStorageMultisample;
	this->glRenderbufferStorageMultisampleANGLE = gl::RenderbufferStorageMultisampleANGLE;
	this->glRenderbufferStorage = gl::RenderbufferStorage;
	this->glSampleCoverage = gl::SampleCoverage;
	this->glSetFenceNV = gl::SetFenceNV;
	this->glScissor = gl::Scissor;
	this->glShaderBinary = gl::ShaderBinary;
	this->glShaderSource = gl::ShaderSource;
	this->glStencilFunc = gl::StencilFunc;
	this->glStencilFuncSeparate = gl::StencilFuncSeparate;
	this->glStencilMask = gl::StencilMask;
	this->glStencilMaskSeparate = gl::StencilMaskSeparate;
	this->glStencilOp = gl::StencilOp;
	this->glStencilOpSeparate = gl::StencilOpSeparate;
	this->glTestFenceNV = gl::TestFenceNV;
	this->glTexImage2D = gl::TexImage2D;
	this->glTexParameterf = gl::TexParameterf;
	this->glTexParameterfv = gl::TexParameterfv;
	this->glTexParameteri = gl::TexParameteri;
	this->glTexParameteriv = gl::TexParameteriv;
	this->glTexSubImage2D = gl::TexSubImage2D;
	this->glUniform1f = gl::Uniform1f;
	this->glUniform1fv = gl::Uniform1fv;
	this->glUniform1i = gl::Uniform1i;
	this->glUniform1iv = gl::Uniform1iv;
	this->glUniform2f = gl::Uniform2f;
	this->glUniform2fv = gl::Uniform2fv;
	this->glUniform2i = gl::Uniform2i;
	this->glUniform2iv = gl::Uniform2iv;
	this->glUniform3f = gl::Uniform3f;
	this->glUniform3fv = gl::Uniform3fv;
	this->glUniform3i = gl::Uniform3i;
	this->glUniform3iv = gl::Uniform3iv;
	this->glUniform4f = gl::Uniform4f;
	this->glUniform4fv = gl::Uniform4fv;
	this->glUniform4i = gl::Uniform4i;
	this->glUniform4iv = gl::Uniform4iv;
	this->glUniformMatrix2fv = gl::UniformMatrix2fv;
	this->glUniformMatrix3fv = gl::UniformMatrix3fv;
	this->glUniformMatrix4fv = gl::UniformMatrix4fv;
	this->glUseProgram = gl::UseProgram;
	this->glValidateProgram = gl::ValidateProgram;
	this->glVertexAttrib1f = gl::VertexAttrib1f;
	this->glVertexAttrib1fv = gl::VertexAttrib1fv;
	this->glVertexAttrib2f = gl::VertexAttrib2f;
	this->glVertexAttrib2fv = gl::VertexAttrib2fv;
	this->glVertexAttrib3f = gl::VertexAttrib3f;
	this->glVertexAttrib3fv = gl::VertexAttrib3fv;
	this->glVertexAttrib4f = gl::VertexAttrib4f;
	this->glVertexAttrib4fv = gl::VertexAttrib4fv;
	this->glVertexAttribPointer = gl::VertexAttribPointer;
	this->glViewport = gl::Viewport;
	this->glBlitFramebufferNV = gl::BlitFramebufferNV;
	this->glBlitFramebufferANGLE = gl::BlitFramebufferANGLE;
	this->glTexImage3DOES = gl::TexImage3DOES;
	this->glTexSubImage3DOES = gl::TexSubImage3DOES;
	this->glCopyTexSubImage3DOES = gl::CopyTexSubImage3DOES;
	this->glCompressedTexImage3DOES = gl::CompressedTexImage3DOES;
	this->glCompressedTexSubImage3DOES = gl::CompressedTexSubImage3DOES;
	this->glFramebufferTexture3DOES = gl::FramebufferTexture3DOES;
	this->glEGLImageTargetTexture2DOES = gl::EGLImageTargetTexture2DOES;
	this->glEGLImageTargetRenderbufferStorageOES = gl::EGLImageTargetRenderbufferStorageOES;
	this->glIsRenderbufferOES = gl::IsRenderbufferOES;
	this->glBindRenderbufferOES = gl::BindRenderbufferOES;
	this->glDeleteRenderbuffersOES = gl::DeleteRenderbuffersOES;
	this->glGenRenderbuffersOES = gl::GenRenderbuffersOES;
	this->glRenderbufferStorageOES = gl::RenderbufferStorageOES;
	this->glGetRenderbufferParameterivOES = gl::GetRenderbufferParameterivOES;
	this->glIsFramebufferOES = gl::IsFramebufferOES;
	this->glBindFramebufferOES = gl::BindFramebufferOES;
	this->glDeleteFramebuffersOES = gl::DeleteFramebuffersOES;
	this->glGenFramebuffersOES = gl::GenFramebuffersOES;
	this->glCheckFramebufferStatusOES = gl::CheckFramebufferStatusOES;
	this->glFramebufferRenderbufferOES = gl::FramebufferRenderbufferOES;
	this->glFramebufferTexture2DOES = gl::FramebufferTexture2DOES;
	this->glGetFramebufferAttachmentParameterivOES = gl::GetFramebufferAttachmentParameterivOES;
	this->glGenerateMipmapOES = gl::GenerateMipmapOES;
	this->glDrawBuffersEXT = gl::DrawBuffersEXT;

	this->es2CreateContext = ::es2CreateContext;
	this->es2GetProcAddress = ::es2GetProcAddress;
	this->createBackBuffer = ::createBackBuffer;
	this->createBackBufferFromClientBuffer = ::createBackBufferFromClientBuffer;
	this->createDepthStencil = ::createDepthStencil;
	this->createFrameBuffer = ::createFrameBuffer;
}

extern "C" GL_APICALL LibGLESv2exports *libGLESv2_swiftshader()
{
	static LibGLESv2exports libGLESv2;
	return &libGLESv2;
}

LibEGL libEGL;
LibGLES_CM libGLES_CM;
