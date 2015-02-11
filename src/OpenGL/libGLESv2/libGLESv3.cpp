#include "common/debug.h"

#define GL_APICALL
#include <GLES3/gl3.h>
#include <GLES2/gl2ext.h>

extern "C"
{

void GL_APIENTRY glReadBuffer(GLenum src)
{
	UNIMPLEMENTED();
}

void GL_APIENTRY glDrawRangeElements(GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const void *indices)
{
	UNIMPLEMENTED();
}

void GL_APIENTRY glTexImage3D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, const void *pixels)
{
	UNIMPLEMENTED();
}

void GL_APIENTRY glTexSubImage3D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const void *pixels)
{
	UNIMPLEMENTED();
}

void GL_APIENTRY glCopyTexSubImage3D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLint x, GLint y, GLsizei width, GLsizei height)
{
	UNIMPLEMENTED();
}

void GL_APIENTRY glCompressedTexImage3D(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLsizei imageSize, const void *data)
{
	UNIMPLEMENTED();
}

void GL_APIENTRY glCompressedTexSubImage3D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, const void *data)
{
	UNIMPLEMENTED();
}

void GL_APIENTRY glGenQueries(GLsizei n, GLuint *ids)
{
	UNIMPLEMENTED();
}

void GL_APIENTRY glDeleteQueries(GLsizei n, const GLuint *ids)
{
	UNIMPLEMENTED();
}

GLboolean GL_APIENTRY glIsQuery(GLuint id)
{
	UNIMPLEMENTED();
	return GL_FALSE;
}

void GL_APIENTRY glBeginQuery(GLenum target, GLuint id)
{
	UNIMPLEMENTED();
}

void GL_APIENTRY glEndQuery(GLenum target)
{
	UNIMPLEMENTED();
}

void GL_APIENTRY glGetQueryiv(GLenum target, GLenum pname, GLint *params)
{
	UNIMPLEMENTED();
}

void GL_APIENTRY glGetQueryObjectuiv(GLuint id, GLenum pname, GLuint *params)
{
	UNIMPLEMENTED();
}

GLboolean GL_APIENTRY glUnmapBuffer(GLenum target)
{
	UNIMPLEMENTED();
	return GL_FALSE;
}

void GL_APIENTRY glGetBufferPointerv(GLenum target, GLenum pname, void **params)
{
	UNIMPLEMENTED();
}

void GL_APIENTRY glDrawBuffers(GLsizei n, const GLenum *bufs)
{
	UNIMPLEMENTED();
}

void GL_APIENTRY glUniformMatrix2x3fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value)
{
	UNIMPLEMENTED();
}

void GL_APIENTRY glUniformMatrix3x2fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value)
{
	UNIMPLEMENTED();
}

void GL_APIENTRY glUniformMatrix2x4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value)
{
	UNIMPLEMENTED();
}

void GL_APIENTRY glUniformMatrix4x2fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value)
{
	UNIMPLEMENTED();
}

void GL_APIENTRY glUniformMatrix3x4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value)
{
	UNIMPLEMENTED();
}

void GL_APIENTRY glUniformMatrix4x3fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value)
{
	UNIMPLEMENTED();
}

void GL_APIENTRY glBlitFramebuffer(GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter)
{
	UNIMPLEMENTED();
}

void GL_APIENTRY glRenderbufferStorageMultisample(GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height)
{
	UNIMPLEMENTED();
}

void GL_APIENTRY glFramebufferTextureLayer(GLenum target, GLenum attachment, GLuint texture, GLint level, GLint layer)
{
	UNIMPLEMENTED();
}

void *GL_APIENTRY glMapBufferRange(GLenum target, GLintptr offset, GLsizeiptr length, GLbitfield access)
{
	UNIMPLEMENTED();
	return nullptr;
}

void GL_APIENTRY glFlushMappedBufferRange(GLenum target, GLintptr offset, GLsizeiptr length)
{
	UNIMPLEMENTED();
}

void GL_APIENTRY glBindVertexArray(GLuint array)
{
	UNIMPLEMENTED();
}

void GL_APIENTRY glDeleteVertexArrays(GLsizei n, const GLuint *arrays)
{
	UNIMPLEMENTED();
}

void GL_APIENTRY glGenVertexArrays(GLsizei n, GLuint *arrays)
{
	UNIMPLEMENTED();
}

GLboolean GL_APIENTRY glIsVertexArray(GLuint array)
{
	UNIMPLEMENTED();
	return GL_FALSE;
}

void GL_APIENTRY glGetIntegeri_v(GLenum target, GLuint index, GLint *data)
{
	UNIMPLEMENTED();
}

void GL_APIENTRY glBeginTransformFeedback(GLenum primitiveMode)
{
	UNIMPLEMENTED();
}

void GL_APIENTRY glEndTransformFeedback(void)
{
	UNIMPLEMENTED();
}

void GL_APIENTRY glBindBufferRange(GLenum target, GLuint index, GLuint buffer, GLintptr offset, GLsizeiptr size)
{
	UNIMPLEMENTED();
}

void GL_APIENTRY glBindBufferBase(GLenum target, GLuint index, GLuint buffer)
{
	UNIMPLEMENTED();
}

void GL_APIENTRY glTransformFeedbackVaryings(GLuint program, GLsizei count, const GLchar *const*varyings, GLenum bufferMode)
{
	UNIMPLEMENTED();
}

void GL_APIENTRY glGetTransformFeedbackVarying(GLuint program, GLuint index, GLsizei bufSize, GLsizei *length, GLsizei *size, GLenum *type, GLchar *name)
{
	UNIMPLEMENTED();
}

void GL_APIENTRY glVertexAttribIPointer(GLuint index, GLint size, GLenum type, GLsizei stride, const void *pointer)
{
	UNIMPLEMENTED();
}

void GL_APIENTRY glGetVertexAttribIiv(GLuint index, GLenum pname, GLint *params)
{
	UNIMPLEMENTED();
}

void GL_APIENTRY glGetVertexAttribIuiv(GLuint index, GLenum pname, GLuint *params)
{
	UNIMPLEMENTED();
}

void GL_APIENTRY glVertexAttribI4i(GLuint index, GLint x, GLint y, GLint z, GLint w)
{
	UNIMPLEMENTED();
}

void GL_APIENTRY glVertexAttribI4ui(GLuint index, GLuint x, GLuint y, GLuint z, GLuint w)
{
	UNIMPLEMENTED();
}

void GL_APIENTRY glVertexAttribI4iv(GLuint index, const GLint *v)
{
	UNIMPLEMENTED();
}

void GL_APIENTRY glVertexAttribI4uiv(GLuint index, const GLuint *v)
{
	UNIMPLEMENTED();
}

void GL_APIENTRY glGetUniformuiv(GLuint program, GLint location, GLuint *params)
{
	UNIMPLEMENTED();
}

GLint GL_APIENTRY glGetFragDataLocation(GLuint program, const GLchar *name)
{
	UNIMPLEMENTED();
	return 0;
}

void GL_APIENTRY glUniform1ui(GLint location, GLuint v0)
{
	UNIMPLEMENTED();
}

void GL_APIENTRY glUniform2ui(GLint location, GLuint v0, GLuint v1)
{
	UNIMPLEMENTED();
}

void GL_APIENTRY glUniform3ui(GLint location, GLuint v0, GLuint v1, GLuint v2)
{
	UNIMPLEMENTED();
}

void GL_APIENTRY glUniform4ui(GLint location, GLuint v0, GLuint v1, GLuint v2, GLuint v3)
{
	UNIMPLEMENTED();
}

void GL_APIENTRY glUniform1uiv(GLint location, GLsizei count, const GLuint *value)
{
	UNIMPLEMENTED();
}

void GL_APIENTRY glUniform2uiv(GLint location, GLsizei count, const GLuint *value)
{
	UNIMPLEMENTED();
}

void GL_APIENTRY glUniform3uiv(GLint location, GLsizei count, const GLuint *value)
{
	UNIMPLEMENTED();
}

void GL_APIENTRY glUniform4uiv(GLint location, GLsizei count, const GLuint *value)
{
	UNIMPLEMENTED();
}

void GL_APIENTRY glClearBufferiv(GLenum buffer, GLint drawbuffer, const GLint *value)
{
	UNIMPLEMENTED();
}

void GL_APIENTRY glClearBufferuiv(GLenum buffer, GLint drawbuffer, const GLuint *value)
{
	UNIMPLEMENTED();
}

void GL_APIENTRY glClearBufferfv(GLenum buffer, GLint drawbuffer, const GLfloat *value)
{
	UNIMPLEMENTED();
}

void GL_APIENTRY glClearBufferfi(GLenum buffer, GLint drawbuffer, GLfloat depth, GLint stencil)
{
	UNIMPLEMENTED();
}

const GLubyte *GL_APIENTRY glGetStringi(GLenum name, GLuint index)
{
	UNIMPLEMENTED();
	return nullptr;
}

void GL_APIENTRY glCopyBufferSubData(GLenum readTarget, GLenum writeTarget, GLintptr readOffset, GLintptr writeOffset, GLsizeiptr size)
{
	UNIMPLEMENTED();
}

void GL_APIENTRY glGetUniformIndices(GLuint program, GLsizei uniformCount, const GLchar *const*uniformNames, GLuint *uniformIndices)
{
	UNIMPLEMENTED();
}

void GL_APIENTRY glGetActiveUniformsiv(GLuint program, GLsizei uniformCount, const GLuint *uniformIndices, GLenum pname, GLint *params)
{
	UNIMPLEMENTED();
}

GLuint GL_APIENTRY glGetUniformBlockIndex(GLuint program, const GLchar *uniformBlockName)
{
	UNIMPLEMENTED();
	return 0;
}

void GL_APIENTRY glGetActiveUniformBlockiv(GLuint program, GLuint uniformBlockIndex, GLenum pname, GLint *params)
{
	UNIMPLEMENTED();
}

void GL_APIENTRY glGetActiveUniformBlockName(GLuint program, GLuint uniformBlockIndex, GLsizei bufSize, GLsizei *length, GLchar *uniformBlockName)
{
	UNIMPLEMENTED();
}

void GL_APIENTRY glUniformBlockBinding(GLuint program, GLuint uniformBlockIndex, GLuint uniformBlockBinding)
{
	UNIMPLEMENTED();
}

void GL_APIENTRY glDrawArraysInstanced(GLenum mode, GLint first, GLsizei count, GLsizei instancecount)
{
	UNIMPLEMENTED();
}

void GL_APIENTRY glDrawElementsInstanced(GLenum mode, GLsizei count, GLenum type, const void *indices, GLsizei instancecount)
{
	UNIMPLEMENTED();
}

GLsync GL_APIENTRY glFenceSync(GLenum condition, GLbitfield flags)
{
	UNIMPLEMENTED();
	return nullptr;
}

GLboolean GL_APIENTRY glIsSync(GLsync sync)
{
	UNIMPLEMENTED();
	return GL_FALSE;
}

void GL_APIENTRY glDeleteSync(GLsync sync)
{
	UNIMPLEMENTED();
}

GLenum GL_APIENTRY glClientWaitSync(GLsync sync, GLbitfield flags, GLuint64 timeout)
{
	UNIMPLEMENTED();
	return GL_FALSE;
}

void GL_APIENTRY glWaitSync(GLsync sync, GLbitfield flags, GLuint64 timeout)
{
	UNIMPLEMENTED();
}

void GL_APIENTRY glGetInteger64v(GLenum pname, GLint64 *data)
{
	UNIMPLEMENTED();
}

void GL_APIENTRY glGetSynciv(GLsync sync, GLenum pname, GLsizei bufSize, GLsizei *length, GLint *values)
{
	UNIMPLEMENTED();
}

void GL_APIENTRY glGetInteger64i_v(GLenum target, GLuint index, GLint64 *data)
{
	UNIMPLEMENTED();
}

void GL_APIENTRY glGetBufferParameteri64v(GLenum target, GLenum pname, GLint64 *params)
{
	UNIMPLEMENTED();
}

void GL_APIENTRY glGenSamplers(GLsizei count, GLuint *samplers)
{
	UNIMPLEMENTED();
}

void GL_APIENTRY glDeleteSamplers(GLsizei count, const GLuint *samplers)
{
	UNIMPLEMENTED();
}

GLboolean GL_APIENTRY glIsSampler(GLuint sampler)
{
	UNIMPLEMENTED();
	return GL_FALSE;
}

void GL_APIENTRY glBindSampler(GLuint unit, GLuint sampler)
{
	UNIMPLEMENTED();
}

void GL_APIENTRY glSamplerParameteri(GLuint sampler, GLenum pname, GLint param)
{
	UNIMPLEMENTED();
}

void GL_APIENTRY glSamplerParameteriv(GLuint sampler, GLenum pname, const GLint *param)
{
	UNIMPLEMENTED();
}

void GL_APIENTRY glSamplerParameterf(GLuint sampler, GLenum pname, GLfloat param)
{
	UNIMPLEMENTED();
}

void GL_APIENTRY glSamplerParameterfv(GLuint sampler, GLenum pname, const GLfloat *param)
{
	UNIMPLEMENTED();
}

void GL_APIENTRY glGetSamplerParameteriv(GLuint sampler, GLenum pname, GLint *params)
{
	UNIMPLEMENTED();
}

void GL_APIENTRY glGetSamplerParameterfv(GLuint sampler, GLenum pname, GLfloat *params)
{
	UNIMPLEMENTED();
}

void GL_APIENTRY glVertexAttribDivisor(GLuint index, GLuint divisor)
{
	UNIMPLEMENTED();
}

void GL_APIENTRY glBindTransformFeedback(GLenum target, GLuint id)
{
	UNIMPLEMENTED();
}

void GL_APIENTRY glDeleteTransformFeedbacks(GLsizei n, const GLuint *ids)
{
	UNIMPLEMENTED();
}

void GL_APIENTRY glGenTransformFeedbacks(GLsizei n, GLuint *ids)
{
	UNIMPLEMENTED();
}

GLboolean GL_APIENTRY glIsTransformFeedback(GLuint id)
{
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
	UNIMPLEMENTED();
}

void GL_APIENTRY glProgramBinary(GLuint program, GLenum binaryFormat, const void *binary, GLsizei length)
{
	UNIMPLEMENTED();
}

void GL_APIENTRY glProgramParameteri(GLuint program, GLenum pname, GLint value)
{
	UNIMPLEMENTED();
}

void GL_APIENTRY glInvalidateFramebuffer(GLenum target, GLsizei numAttachments, const GLenum *attachments)
{
	UNIMPLEMENTED();
}

void GL_APIENTRY glInvalidateSubFramebuffer(GLenum target, GLsizei numAttachments, const GLenum *attachments, GLint x, GLint y, GLsizei width, GLsizei height)
{
	UNIMPLEMENTED();
}

void GL_APIENTRY glTexStorage2D(GLenum target, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height)
{
	UNIMPLEMENTED();
}

void GL_APIENTRY glTexStorage3D(GLenum target, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth)
{
	UNIMPLEMENTED();
}

void GL_APIENTRY glGetInternalformativ(GLenum target, GLenum internalformat, GLenum pname, GLsizei bufSize, GLint *params)
{
	UNIMPLEMENTED();
}



}
