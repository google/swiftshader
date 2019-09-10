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
#include "TransformFeedback.h"
#include "VertexArray.h"
#include "common/debug.h"
#include "Common/Version.h"

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <GLES3/gl3.h>

#include <algorithm>
#include <limits>

namespace es2
{

static bool validImageSize(GLint level, GLsizei width, GLsizei height)
{
	if(level < 0 || level >= IMPLEMENTATION_MAX_TEXTURE_LEVELS || width < 0 || height < 0)
	{
		return false;
	}

	return true;
}

}

namespace gl
{

using namespace es2;

void GL_APIENTRY ActiveTexture(GLenum texture)
{
	TRACE("(GLenum texture = 0x%X)", texture);

	auto context = es2::getContext();

	if(context)
	{
		if(texture < GL_TEXTURE0 || texture > GL_TEXTURE0 + es2::MAX_COMBINED_TEXTURE_IMAGE_UNITS - 1)
		{
			return error(GL_INVALID_ENUM);
		}

		context->setActiveSampler(texture - GL_TEXTURE0);
	}
}

void GL_APIENTRY AttachShader(GLuint program, GLuint shader)
{
	TRACE("(GLuint program = %d, GLuint shader = %d)", program, shader);

	auto context = es2::getContext();

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

void GL_APIENTRY BeginQueryEXT(GLenum target, GLuint name)
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

	auto context = es2::getContext();

	if(context)
	{
		context->beginQuery(target, name);
	}
}

void GL_APIENTRY BindAttribLocation(GLuint program, GLuint index, const GLchar* name)
{
	TRACE("(GLuint program = %d, GLuint index = %d, const GLchar* name = %s)", program, index, name);

	if(index >= es2::MAX_VERTEX_ATTRIBS)
	{
		return error(GL_INVALID_VALUE);
	}

	auto context = es2::getContext();

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

void GL_APIENTRY BindBuffer(GLenum target, GLuint buffer)
{
	TRACE("(GLenum target = 0x%X, GLuint buffer = %d)", target, buffer);

	auto context = es2::getContext();

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
		case GL_COPY_READ_BUFFER:
			context->bindCopyReadBuffer(buffer);
			return;
		case GL_COPY_WRITE_BUFFER:
			context->bindCopyWriteBuffer(buffer);
			return;
		case GL_PIXEL_PACK_BUFFER:
			context->bindPixelPackBuffer(buffer);
			return;
		case GL_PIXEL_UNPACK_BUFFER:
			context->bindPixelUnpackBuffer(buffer);
			return;
		case GL_TRANSFORM_FEEDBACK_BUFFER:
			context->bindTransformFeedbackBuffer(buffer);
			return;
		case GL_UNIFORM_BUFFER:
			context->bindGenericUniformBuffer(buffer);
			return;
		default:
			return error(GL_INVALID_ENUM);
		}
	}
}

void GL_APIENTRY BindFramebuffer(GLenum target, GLuint framebuffer)
{
	TRACE("(GLenum target = 0x%X, GLuint framebuffer = %d)", target, framebuffer);

	if(target != GL_FRAMEBUFFER && target != GL_DRAW_FRAMEBUFFER && target != GL_READ_FRAMEBUFFER)
	{
		return error(GL_INVALID_ENUM);
	}

	auto context = es2::getContext();

	if(context)
	{
		if(target == GL_READ_FRAMEBUFFER || target == GL_FRAMEBUFFER)
		{
			context->bindReadFramebuffer(framebuffer);
		}

		if(target == GL_DRAW_FRAMEBUFFER || target == GL_FRAMEBUFFER)
		{
			context->bindDrawFramebuffer(framebuffer);
		}
	}
}

void GL_APIENTRY BindRenderbuffer(GLenum target, GLuint renderbuffer)
{
	TRACE("(GLenum target = 0x%X, GLuint renderbuffer = %d)", target, renderbuffer);

	if(target != GL_RENDERBUFFER)
	{
		return error(GL_INVALID_ENUM);
	}

	auto context = es2::getContext();

	if(context)
	{
		// [OpenGL ES 2.0.25] Section 4.4.3 page 110
		// [OpenGL ES 3.0.4] Section 4.4.2 page 204
		// If renderbuffer is not zero, then the resulting renderbuffer object
		// is a new state vector, initialized with a zero-sized memory buffer.
		context->bindRenderbuffer(renderbuffer);
	}
}

void GL_APIENTRY BindTexture(GLenum target, GLuint texture)
{
	TRACE("(GLenum target = 0x%X, GLuint texture = %d)", target, texture);

	auto context = es2::getContext();

	if(context)
	{
		es2::Texture *textureObject = context->getTexture(texture);

		if(textureObject && textureObject->getTarget() != target && texture != 0)
		{
			return error(GL_INVALID_OPERATION);
		}

		switch(target)
		{
		case GL_TEXTURE_2D:
			context->bindTexture(TEXTURE_2D, texture);
			break;
		case GL_TEXTURE_CUBE_MAP:
			context->bindTexture(TEXTURE_CUBE, texture);
			break;
		case GL_TEXTURE_EXTERNAL_OES:
			context->bindTexture(TEXTURE_EXTERNAL, texture);
			break;
		case GL_TEXTURE_2D_ARRAY:
			context->bindTexture(TEXTURE_2D_ARRAY, texture);
			break;
		case GL_TEXTURE_3D:
			context->bindTexture(TEXTURE_3D, texture);
			break;
		case GL_TEXTURE_RECTANGLE_ARB:
			context->bindTexture(TEXTURE_2D_RECT, texture);
			break;
		default:
			return error(GL_INVALID_ENUM);
		}
	}
}

void GL_APIENTRY BlendColor(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha)
{
	TRACE("(GLclampf red = %f, GLclampf green = %f, GLclampf blue = %f, GLclampf alpha = %f)",
		red, green, blue, alpha);

	auto context = es2::getContext();

	if(context)
	{
		context->setBlendColor(es2::clamp01(red), es2::clamp01(green), es2::clamp01(blue), es2::clamp01(alpha));
	}
}

void GL_APIENTRY BlendEquationSeparate(GLenum modeRGB, GLenum modeAlpha)
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

	auto context = es2::getContext();

	if(context)
	{
		context->setBlendEquation(modeRGB, modeAlpha);
	}
}

void GL_APIENTRY BlendEquation(GLenum mode)
{
	BlendEquationSeparate(mode, mode);
}

void GL_APIENTRY BlendFuncSeparate(GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha)
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
	case GL_SRC_ALPHA_SATURATE:
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
		break;
	default:
		return error(GL_INVALID_ENUM);
	}

	auto context = es2::getContext();

	if(context)
	{
		context->setBlendFactors(srcRGB, dstRGB, srcAlpha, dstAlpha);
	}
}

void GL_APIENTRY BlendFunc(GLenum sfactor, GLenum dfactor)
{
	BlendFuncSeparate(sfactor, dfactor, sfactor, dfactor);
}

void GL_APIENTRY BufferData(GLenum target, GLsizeiptr size, const GLvoid* data, GLenum usage)
{
	size = static_cast<GLint>(size);   // Work around issues with some 64-bit applications

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
	case GL_STREAM_READ:
	case GL_STREAM_COPY:
	case GL_STATIC_READ:
	case GL_STATIC_COPY:
	case GL_DYNAMIC_READ:
	case GL_DYNAMIC_COPY:
		break;
	default:
		return error(GL_INVALID_ENUM);
	}

	auto context = es2::getContext();

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

void GL_APIENTRY BufferSubData(GLenum target, GLintptr offset, GLsizeiptr size, const GLvoid* data)
{
	size = static_cast<GLint>(size);   // Work around issues with some 64-bit applications
	offset = static_cast<GLint>(offset);

	TRACE("(GLenum target = 0x%X, GLintptr offset = %d, GLsizeiptr size = %d, const GLvoid* data = %p)",
	      target, offset, size, data);

	if(size < 0 || offset < 0)
	{
		return error(GL_INVALID_VALUE);
	}

	auto context = es2::getContext();

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

		if(buffer->isMapped())
		{
			// It is an invalid operation to update an already mapped buffer
			return error(GL_INVALID_OPERATION);
		}

		if((size_t)size + offset > buffer->size())
		{
			return error(GL_INVALID_VALUE);
		}

		buffer->bufferSubData(data, size, offset);
	}
}

GLenum GL_APIENTRY CheckFramebufferStatus(GLenum target)
{
	TRACE("(GLenum target = 0x%X)", target);

	if(target != GL_FRAMEBUFFER && target != GL_DRAW_FRAMEBUFFER && target != GL_READ_FRAMEBUFFER)
	{
		return error(GL_INVALID_ENUM, 0);
	}

	auto context = es2::getContext();

	if(context)
	{
		es2::Framebuffer *framebuffer = nullptr;
		if(target == GL_READ_FRAMEBUFFER)
		{
			framebuffer = context->getReadFramebuffer();
		}
		else
		{
			framebuffer = context->getDrawFramebuffer();
		}

		if(!framebuffer)
		{
			return GL_FRAMEBUFFER_UNDEFINED_OES;
		}

		return framebuffer->completeness();
	}

	return 0;
}

void GL_APIENTRY Clear(GLbitfield mask)
{
	TRACE("(GLbitfield mask = %X)", mask);

	if((mask & ~(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT)) != 0)
	{
		return error(GL_INVALID_VALUE);
	}

	auto context = es2::getContext();

	if(context)
	{
		context->clear(mask);
	}
}

void GL_APIENTRY ClearColor(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha)
{
	TRACE("(GLclampf red = %f, GLclampf green = %f, GLclampf blue = %f, GLclampf alpha = %f)",
	      red, green, blue, alpha);

	auto context = es2::getContext();

	if(context)
	{
		context->setClearColor(red, green, blue, alpha);
	}
}

void GL_APIENTRY ClearDepthf(GLclampf depth)
{
	TRACE("(GLclampf depth = %f)", depth);

	auto context = es2::getContext();

	if(context)
	{
		context->setClearDepth(depth);
	}
}

void GL_APIENTRY ClearStencil(GLint s)
{
	TRACE("(GLint s = %d)", s);

	auto context = es2::getContext();

	if(context)
	{
		context->setClearStencil(s);
	}
}

void GL_APIENTRY ColorMask(GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha)
{
	TRACE("(GLboolean red = %d, GLboolean green = %d, GLboolean blue = %d, GLboolean alpha = %d)",
	      red, green, blue, alpha);

	auto context = es2::getContext();

	if(context)
	{
		context->setColorMask(red != GL_FALSE, green != GL_FALSE, blue != GL_FALSE, alpha != GL_FALSE);
	}
}

void GL_APIENTRY CompileShader(GLuint shader)
{
	TRACE("(GLuint shader = %d)", shader);

	auto context = es2::getContext();

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

void GL_APIENTRY CompressedTexImage2D(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height,
                                      GLint border, GLsizei imageSize, const GLvoid* data)
{
	TRACE("(GLenum target = 0x%X, GLint level = %d, GLenum internalformat = 0x%X, GLsizei width = %d, "
	      "GLsizei height = %d, GLint border = %d, GLsizei imageSize = %d, const GLvoid* data = %p)",
	      target, level, internalformat, width, height, border, imageSize, data);

	if(!validImageSize(level, width, height) || border != 0 || imageSize < 0)
	{
		return error(GL_INVALID_VALUE);
	}

	if(!IsCompressed(internalformat))
	{
		return error(GL_INVALID_ENUM);
	}

	if(border != 0)
	{
		return error(GL_INVALID_VALUE);
	}

	auto context = es2::getContext();

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
		case GL_TEXTURE_RECTANGLE_ARB: // Rectangle textures cannot be compressed
		default:
			return error(GL_INVALID_ENUM);
		}

		if(imageSize != gl::ComputeCompressedSize(width, height, internalformat))
		{
			return error(GL_INVALID_VALUE);
		}

		GLenum validationError = context->getPixels(&data, GL_UNSIGNED_BYTE, imageSize);
		if(validationError != GL_NO_ERROR)
		{
			return error(validationError);
		}

		if(target == GL_TEXTURE_2D || target == GL_TEXTURE_RECTANGLE_ARB)
		{
			es2::Texture2D *texture = context->getTexture2D(target);

			if(!texture)
			{
				return error(GL_INVALID_OPERATION);
			}

			texture->setCompressedImage(level, internalformat, width, height, imageSize, data);
		}
		else if(es2::IsCubemapTextureTarget(target))
		{
			es2::TextureCubeMap *texture = context->getTextureCubeMap();

			if(!texture)
			{
				return error(GL_INVALID_OPERATION);
			}

			texture->setCompressedImage(target, level, internalformat, width, height, imageSize, data);
		}
		else UNREACHABLE(target);
	}
}

void GL_APIENTRY CompressedTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height,
                                         GLenum format, GLsizei imageSize, const GLvoid* data)
{
	TRACE("(GLenum target = 0x%X, GLint level = %d, GLint xoffset = %d, GLint yoffset = %d, "
	      "GLsizei width = %d, GLsizei height = %d, GLenum format = 0x%X, "
	      "GLsizei imageSize = %d, const GLvoid* data = %p)",
	      target, level, xoffset, yoffset, width, height, format, imageSize, data);

	if(!es2::IsTexImageTarget(target))
	{
		return error(GL_INVALID_ENUM);
	}

	if(level < 0 || level >= es2::IMPLEMENTATION_MAX_TEXTURE_LEVELS)
	{
		return error(GL_INVALID_VALUE);
	}

	if(xoffset < 0 || yoffset < 0 || !validImageSize(level, width, height) || imageSize < 0)
	{
		return error(GL_INVALID_VALUE);
	}

	if(!IsCompressed(format))
	{
		return error(GL_INVALID_ENUM);
	}

	if(imageSize != gl::ComputeCompressedSize(width, height, format))
	{
		return error(GL_INVALID_VALUE);
	}

	auto context = es2::getContext();

	if(context)
	{
		if(xoffset % 4 != 0 || yoffset % 4 != 0)
		{
			// We wait to check the offsets until this point, because the multiple-of-four restriction does not exist unless DXT1 textures are supported
			return error(GL_INVALID_OPERATION);
		}

		GLenum validationError = context->getPixels(&data, GL_UNSIGNED_BYTE, imageSize);
		if(validationError != GL_NO_ERROR)
		{
			return error(validationError);
		}

		if(target == GL_TEXTURE_2D || target == GL_TEXTURE_RECTANGLE_ARB)
		{
			es2::Texture2D *texture = context->getTexture2D(target);

			GLenum validationError = ValidateSubImageParams(true, false, target, level, xoffset, yoffset, width, height, format, GL_NONE, texture);
			if(validationError != GL_NO_ERROR)
			{
				return error(validationError);
			}

			texture->subImageCompressed(level, xoffset, yoffset, width, height, format, imageSize, data);
		}
		else if(es2::IsCubemapTextureTarget(target))
		{
			es2::TextureCubeMap *texture = context->getTextureCubeMap();

			GLenum validationError = ValidateSubImageParams(true, false, target, level, xoffset, yoffset, width, height, format, GL_NONE, texture);
			if(validationError != GL_NO_ERROR)
			{
				return error(validationError);
			}

			texture->subImageCompressed(target, level, xoffset, yoffset, width, height, format, imageSize, data);
		}
		else UNREACHABLE(target);
	}
}

void GL_APIENTRY CopyTexImage2D(GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border)
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

	auto context = es2::getContext();

	if(context)
	{
		switch(target)
		{
		case GL_TEXTURE_RECTANGLE_ARB:
			if(level != 0)
			{
				return error(GL_INVALID_VALUE);
			}
			// Fall through to GL_TEXTURE_2D case.
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

		if(!framebuffer || (framebuffer->completeness() != GL_FRAMEBUFFER_COMPLETE))
		{
			return error(GL_INVALID_FRAMEBUFFER_OPERATION);
		}

		es2::Renderbuffer *source = framebuffer->getReadColorbuffer();

		if(context->getReadFramebufferName() != 0 && (!source || source->getSamples() > 1))
		{
			return error(GL_INVALID_OPERATION);
		}

		GLenum colorbufferFormat = source->getFormat();

		// Determine the sized internal format.
		if(gl::IsUnsizedInternalFormat(internalformat))
		{
			if(colorbufferFormat == GL_RGB10_A2)
			{
				// Not supported with unsized internalformat.
				// https://www.khronos.org/members/login/bugzilla/show_bug.cgi?id=9807#c56
				return error(GL_INVALID_OPERATION);
			}

			if(gl::GetBaseInternalFormat(colorbufferFormat) == internalformat)
			{
				internalformat = colorbufferFormat;
			}
			else if(GetColorComponentType(colorbufferFormat) == GL_UNSIGNED_NORMALIZED && GetRedSize(colorbufferFormat) <= 8)
			{
				// TODO: Convert to the smallest format that fits all components.
				// e.g. Copying RGBA4 to RGB should result in RGB565, not RGB8.

				internalformat = gl::GetSizedInternalFormat(internalformat, GL_UNSIGNED_BYTE);
			}
			else if(GetColorComponentType(colorbufferFormat) == GL_INT)
			{
				internalformat = gl::GetSizedInternalFormat(internalformat, GL_INT);
			}
			else if(GetColorComponentType(colorbufferFormat) == GL_UNSIGNED_INT)
			{
				internalformat = gl::GetSizedInternalFormat(internalformat, GL_UNSIGNED_INT);
			}
			else if(GetColorComponentType(colorbufferFormat) == GL_FLOAT && GetRedSize(colorbufferFormat) == 16)   // GL_EXT_color_buffer_half_float
			{
				internalformat = gl::GetSizedInternalFormat(internalformat, GL_HALF_FLOAT_OES);
			}
			else if(GetColorComponentType(colorbufferFormat) == GL_FLOAT && GetRedSize(colorbufferFormat) == 32)   // GL_EXT_color_buffer_float
			{
				internalformat = gl::GetSizedInternalFormat(internalformat, GL_FLOAT);
			}
			else
			{
				UNIMPLEMENTED("internalformat = %x, colorbufferFormat = %X", internalformat, colorbufferFormat);

				return error(GL_INVALID_OPERATION);
			}
		}

		if(!ValidateCopyFormats(internalformat, colorbufferFormat))
		{
			return;
		}

		if(target == GL_TEXTURE_2D || target == GL_TEXTURE_RECTANGLE_ARB)
		{
			es2::Texture2D *texture = context->getTexture2D(target);

			if(!texture)
			{
				return error(GL_INVALID_OPERATION);
			}

			texture->copyImage(level, internalformat, x, y, width, height, source);
		}
		else if(es2::IsCubemapTextureTarget(target))
		{
			es2::TextureCubeMap *texture = context->getTextureCubeMap();

			if(!texture)
			{
				return error(GL_INVALID_OPERATION);
			}

			texture->copyImage(target, level, internalformat, x, y, width, height, source);
		}
		else UNREACHABLE(target);
	}
}

void GL_APIENTRY CopyTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height)
{
	TRACE("(GLenum target = 0x%X, GLint level = %d, GLint xoffset = %d, GLint yoffset = %d, "
	      "GLint x = %d, GLint y = %d, GLsizei width = %d, GLsizei height = %d)",
	      target, level, xoffset, yoffset, x, y, width, height);

	if(!es2::IsTexImageTarget(target))
	{
		return error(GL_INVALID_ENUM);
	}

	if(level < 0 || level >= es2::IMPLEMENTATION_MAX_TEXTURE_LEVELS)
	{
		return error(GL_INVALID_VALUE);
	}

	if(xoffset < 0 || yoffset < 0 || width < 0 || height < 0)
	{
		return error(GL_INVALID_VALUE);
	}

	if(std::numeric_limits<GLsizei>::max() - xoffset < width || std::numeric_limits<GLsizei>::max() - yoffset < height)
	{
		return error(GL_INVALID_VALUE);
	}

	auto context = es2::getContext();

	if(context)
	{
		es2::Framebuffer *framebuffer = context->getReadFramebuffer();

		if(!framebuffer || (framebuffer->completeness() != GL_FRAMEBUFFER_COMPLETE))
		{
			return error(GL_INVALID_FRAMEBUFFER_OPERATION);
		}

		es2::Renderbuffer *source = framebuffer->getReadColorbuffer();

		if(context->getReadFramebufferName() != 0 && (!source || source->getSamples() > 1))
		{
			return error(GL_INVALID_OPERATION);
		}

		es2::Texture *texture = nullptr;

		if(target == GL_TEXTURE_2D || target == GL_TEXTURE_RECTANGLE_ARB)
		{
			texture = context->getTexture2D(target);
		}
		else if(es2::IsCubemapTextureTarget(target))
		{
			texture = context->getTextureCubeMap();
		}
		else UNREACHABLE(target);

		GLenum validationError = ValidateSubImageParams(false, true, target, level, xoffset, yoffset, width, height, GL_NONE, GL_NONE, texture);
		if(validationError != GL_NO_ERROR)
		{
			return error(validationError);
		}

		texture->copySubImage(target, level, xoffset, yoffset, 0, x, y, width, height, source);
	}
}

GLuint GL_APIENTRY CreateProgram(void)
{
	TRACE("()");

	auto context = es2::getContext();

	if(context)
	{
		return context->createProgram();
	}

	return 0;
}

GLuint GL_APIENTRY CreateShader(GLenum type)
{
	TRACE("(GLenum type = 0x%X)", type);

	auto context = es2::getContext();

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

void GL_APIENTRY CullFace(GLenum mode)
{
	TRACE("(GLenum mode = 0x%X)", mode);

	switch(mode)
	{
	case GL_FRONT:
	case GL_BACK:
	case GL_FRONT_AND_BACK:
		{
			auto context = es2::getContext();

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

void GL_APIENTRY DeleteBuffers(GLsizei n, const GLuint* buffers)
{
	TRACE("(GLsizei n = %d, const GLuint* buffers = %p)", n, buffers);

	if(n < 0)
	{
		return error(GL_INVALID_VALUE);
	}

	auto context = es2::getContext();

	if(context)
	{
		for(int i = 0; i < n; i++)
		{
			context->deleteBuffer(buffers[i]);
		}
	}
}

void GL_APIENTRY DeleteFencesNV(GLsizei n, const GLuint* fences)
{
	TRACE("(GLsizei n = %d, const GLuint* fences = %p)", n, fences);

	if(n < 0)
	{
		return error(GL_INVALID_VALUE);
	}

	auto context = es2::getContext();

	if(context)
	{
		for(int i = 0; i < n; i++)
		{
			context->deleteFence(fences[i]);
		}
	}
}

void GL_APIENTRY DeleteFramebuffers(GLsizei n, const GLuint* framebuffers)
{
	TRACE("(GLsizei n = %d, const GLuint* framebuffers = %p)", n, framebuffers);

	if(n < 0)
	{
		return error(GL_INVALID_VALUE);
	}

	auto context = es2::getContext();

	if(context)
	{
		for(int i = 0; i < n; i++)
		{
			if(framebuffers[i] != 0)   // Attempts to delete default framebuffer silently ignored.
			{
				context->deleteFramebuffer(framebuffers[i]);
			}
		}
	}
}

void GL_APIENTRY DeleteProgram(GLuint program)
{
	TRACE("(GLuint program = %d)", program);

	if(program == 0)
	{
		return;
	}

	auto context = es2::getContext();

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

void GL_APIENTRY DeleteQueriesEXT(GLsizei n, const GLuint *ids)
{
	TRACE("(GLsizei n = %d, const GLuint *ids = %p)", n, ids);

	if(n < 0)
	{
		return error(GL_INVALID_VALUE);
	}

	auto context = es2::getContext();

	if(context)
	{
		for(int i = 0; i < n; i++)
		{
			context->deleteQuery(ids[i]);
		}
	}
}

void GL_APIENTRY DeleteRenderbuffers(GLsizei n, const GLuint* renderbuffers)
{
	TRACE("(GLsizei n = %d, const GLuint* renderbuffers = %p)", n, renderbuffers);

	if(n < 0)
	{
		return error(GL_INVALID_VALUE);
	}

	auto context = es2::getContext();

	if(context)
	{
		for(int i = 0; i < n; i++)
		{
			context->deleteRenderbuffer(renderbuffers[i]);
		}
	}
}

void GL_APIENTRY DeleteShader(GLuint shader)
{
	TRACE("(GLuint shader = %d)", shader);

	if(shader == 0)
	{
		return;
	}

	auto context = es2::getContext();

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

void GL_APIENTRY DeleteTextures(GLsizei n, const GLuint* textures)
{
	TRACE("(GLsizei n = %d, const GLuint* textures = %p)", n, textures);

	if(n < 0)
	{
		return error(GL_INVALID_VALUE);
	}

	auto context = es2::getContext();

	if(context)
	{
		for(int i = 0; i < n; i++)
		{
			if(textures[i] != 0)   // Attempts to delete default texture silently ignored.
			{
				context->deleteTexture(textures[i]);
			}
		}
	}
}

void GL_APIENTRY DepthFunc(GLenum func)
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

	auto context = es2::getContext();

	if(context)
	{
		context->setDepthFunc(func);
	}
}

void GL_APIENTRY DepthMask(GLboolean flag)
{
	TRACE("(GLboolean flag = %d)", flag);

	auto context = es2::getContext();

	if(context)
	{
		context->setDepthMask(flag != GL_FALSE);
	}
}

void GL_APIENTRY DepthRangef(GLclampf zNear, GLclampf zFar)
{
	TRACE("(GLclampf zNear = %f, GLclampf zFar = %f)", zNear, zFar);

	auto context = es2::getContext();

	if(context)
	{
		context->setDepthRange(zNear, zFar);
	}
}

void GL_APIENTRY DetachShader(GLuint program, GLuint shader)
{
	TRACE("(GLuint program = %d, GLuint shader = %d)", program, shader);

	auto context = es2::getContext();

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

void GL_APIENTRY Disable(GLenum cap)
{
	TRACE("(GLenum cap = 0x%X)", cap);

	auto context = es2::getContext();

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

void GL_APIENTRY DisableVertexAttribArray(GLuint index)
{
	TRACE("(GLuint index = %d)", index);

	if(index >= es2::MAX_VERTEX_ATTRIBS)
	{
		return error(GL_INVALID_VALUE);
	}

	auto context = es2::getContext();

	if(context)
	{
		context->setVertexAttribArrayEnabled(index, false);
	}
}

void GL_APIENTRY DrawArrays(GLenum mode, GLint first, GLsizei count)
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

	auto context = es2::getContext();

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

void GL_APIENTRY DrawElements(GLenum mode, GLsizei count, GLenum type, const GLvoid* indices)
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

	auto context = es2::getContext();

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

void GL_APIENTRY DrawArraysInstancedEXT(GLenum mode, GLint first, GLsizei count, GLsizei instanceCount)
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

	auto context = es2::getContext();

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

void GL_APIENTRY DrawElementsInstancedEXT(GLenum mode, GLsizei count, GLenum type, const void *indices, GLsizei instanceCount)
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

	auto context = es2::getContext();

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

void GL_APIENTRY VertexAttribDivisorEXT(GLuint index, GLuint divisor)
{
	TRACE("(GLuint index = %d, GLuint divisor = %d)", index, divisor);

	auto context = es2::getContext();

	if(context)
	{
		if(index >= es2::MAX_VERTEX_ATTRIBS)
		{
			return error(GL_INVALID_VALUE);
		}

		context->setVertexAttribDivisor(index, divisor);
	}
}

void GL_APIENTRY DrawArraysInstancedANGLE(GLenum mode, GLint first, GLsizei count, GLsizei instanceCount)
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

	auto context = es2::getContext();

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

void GL_APIENTRY DrawElementsInstancedANGLE(GLenum mode, GLsizei count, GLenum type, const void *indices, GLsizei instanceCount)
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

	auto context = es2::getContext();

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

void GL_APIENTRY VertexAttribDivisorANGLE(GLuint index, GLuint divisor)
{
	TRACE("(GLuint index = %d, GLuint divisor = %d)", index, divisor);

	auto context = es2::getContext();

	if(context)
	{
		if(index >= MAX_VERTEX_ATTRIBS)
		{
			return error(GL_INVALID_VALUE);
		}

		context->setVertexAttribDivisor(index, divisor);
	}
}

void GL_APIENTRY Enable(GLenum cap)
{
	TRACE("(GLenum cap = 0x%X)", cap);

	auto context = es2::getContext();

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

void GL_APIENTRY EnableVertexAttribArray(GLuint index)
{
	TRACE("(GLuint index = %d)", index);

	if(index >= es2::MAX_VERTEX_ATTRIBS)
	{
		return error(GL_INVALID_VALUE);
	}

	auto context = es2::getContext();

	if(context)
	{
		context->setVertexAttribArrayEnabled(index, true);
	}
}

void GL_APIENTRY EndQueryEXT(GLenum target)
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

	auto context = es2::getContext();

	if(context)
	{
		context->endQuery(target);
	}
}

void GL_APIENTRY FinishFenceNV(GLuint fence)
{
	TRACE("(GLuint fence = %d)", fence);

	auto context = es2::getContext();

	if(context)
	{
		es2::Fence *fenceObject = context->getFence(fence);

		if(!fenceObject)
		{
			return error(GL_INVALID_OPERATION);
		}

		fenceObject->finishFence();
	}
}

void GL_APIENTRY Finish(void)
{
	TRACE("()");

	auto context = es2::getContext();

	if(context)
	{
		context->finish();
	}
}

void GL_APIENTRY Flush(void)
{
	TRACE("()");

	auto context = es2::getContext();

	if(context)
	{
		context->flush();
	}
}

void GL_APIENTRY FramebufferRenderbuffer(GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer)
{
	TRACE("(GLenum target = 0x%X, GLenum attachment = 0x%X, GLenum renderbuffertarget = 0x%X, "
	      "GLuint renderbuffer = %d)", target, attachment, renderbuffertarget, renderbuffer);

	if((target != GL_FRAMEBUFFER && target != GL_DRAW_FRAMEBUFFER && target != GL_READ_FRAMEBUFFER) ||
	   (renderbuffertarget != GL_RENDERBUFFER && renderbuffer != 0))
	{
		return error(GL_INVALID_ENUM);
	}

	auto context = es2::getContext();

	if(context)
	{
		es2::Framebuffer *framebuffer = nullptr;
		GLuint framebufferName = 0;
		if(target == GL_READ_FRAMEBUFFER)
		{
			framebuffer = context->getReadFramebuffer();
			framebufferName = context->getReadFramebufferName();
		}
		else
		{
			framebuffer = context->getDrawFramebuffer();
			framebufferName = context->getDrawFramebufferName();
		}

		if(!framebuffer || framebufferName == 0)
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

		switch(attachment)
		{
		case GL_DEPTH_ATTACHMENT:
			framebuffer->setDepthbuffer(GL_RENDERBUFFER, renderbuffer);
			break;
		case GL_STENCIL_ATTACHMENT:
			framebuffer->setStencilbuffer(GL_RENDERBUFFER, renderbuffer);
			break;
		case GL_DEPTH_STENCIL_ATTACHMENT:
			framebuffer->setDepthbuffer(GL_RENDERBUFFER, renderbuffer);
			framebuffer->setStencilbuffer(GL_RENDERBUFFER, renderbuffer);
			break;
		default:
			if(attachment < GL_COLOR_ATTACHMENT0 || attachment > GL_COLOR_ATTACHMENT31)
			{
				return error(GL_INVALID_ENUM);
			}

			if((attachment - GL_COLOR_ATTACHMENT0) >= MAX_COLOR_ATTACHMENTS)
			{
				return error(GL_INVALID_OPERATION);
			}

			framebuffer->setColorbuffer(GL_RENDERBUFFER, renderbuffer, attachment - GL_COLOR_ATTACHMENT0);
			break;
		}
	}
}

void GL_APIENTRY FramebufferTexture2D(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level)
{
	TRACE("(GLenum target = 0x%X, GLenum attachment = 0x%X, GLenum textarget = 0x%X, "
	      "GLuint texture = %d, GLint level = %d)", target, attachment, textarget, texture, level);

	if(target != GL_FRAMEBUFFER && target != GL_DRAW_FRAMEBUFFER && target != GL_READ_FRAMEBUFFER)
	{
		return error(GL_INVALID_ENUM);
	}

	auto context = es2::getContext();

	if(context)
	{
		if(texture == 0)
		{
			textarget = GL_NONE;
		}
		else
		{
			es2::Texture *tex = context->getTexture(texture);

			if(!tex)
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
			case GL_TEXTURE_RECTANGLE_ARB:
				if(tex->getTarget() != GL_TEXTURE_RECTANGLE_ARB)
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

			if((textarget == GL_TEXTURE_RECTANGLE_ARB) && (level != 0))
			{
				return error(GL_INVALID_VALUE);
			}

			if((level < 0) || (level >= es2::IMPLEMENTATION_MAX_TEXTURE_LEVELS))
			{
				return error(GL_INVALID_VALUE);
			}

			if(tex->isCompressed(textarget, level))
			{
				return error(GL_INVALID_OPERATION);
			}
		}

		es2::Framebuffer *framebuffer = nullptr;
		GLuint framebufferName = 0;
		if(target == GL_READ_FRAMEBUFFER)
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
		case GL_DEPTH_ATTACHMENT:   framebuffer->setDepthbuffer(textarget, texture, level);   break;
		case GL_STENCIL_ATTACHMENT: framebuffer->setStencilbuffer(textarget, texture, level); break;
		case GL_DEPTH_STENCIL_ATTACHMENT:
			framebuffer->setDepthbuffer(textarget, texture, level);
			framebuffer->setStencilbuffer(textarget, texture, level);
			break;
		default:
			if(attachment < GL_COLOR_ATTACHMENT0 || attachment > GL_COLOR_ATTACHMENT31)
			{
				return error(GL_INVALID_ENUM);
			}

			if((attachment - GL_COLOR_ATTACHMENT0) >= MAX_COLOR_ATTACHMENTS)
			{
				return error(GL_INVALID_OPERATION);
			}

			framebuffer->setColorbuffer(textarget, texture, attachment - GL_COLOR_ATTACHMENT0, level);
			break;
		}
	}
}

void GL_APIENTRY FrontFace(GLenum mode)
{
	TRACE("(GLenum mode = 0x%X)", mode);

	switch(mode)
	{
	case GL_CW:
	case GL_CCW:
		{
			auto context = es2::getContext();

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

void GL_APIENTRY GenBuffers(GLsizei n, GLuint* buffers)
{
	TRACE("(GLsizei n = %d, GLuint* buffers = %p)", n, buffers);

	if(n < 0)
	{
		return error(GL_INVALID_VALUE);
	}

	auto context = es2::getContext();

	if(context)
	{
		for(int i = 0; i < n; i++)
		{
			buffers[i] = context->createBuffer();
		}
	}
}

void GL_APIENTRY GenerateMipmap(GLenum target)
{
	TRACE("(GLenum target = 0x%X)", target);

	auto context = es2::getContext();

	if(context)
	{
		es2::Texture *texture = context->getTargetTexture(target);

		if(!texture)
		{
			return;
		}

		if(!IsMipmappable(texture->getFormat(target, texture->getBaseLevel())))
		{
			return error(GL_INVALID_OPERATION);
		}

		if(target == GL_TEXTURE_CUBE_MAP)
		{
			TextureCubeMap *cube = context->getTextureCubeMap();

			if(!cube->isCubeComplete())
			{
				return error(GL_INVALID_OPERATION);
			}
		}

		// [OpenGL ES 3.2]: "Otherwise, if levelbase is not defined, or if any dimension
		// is zero, all mipmap levels are left unchanged. This is not an error."
		if(!texture->isBaseLevelDefined())
		{
			return;
		}

		texture->generateMipmaps();
	}
}

void GL_APIENTRY GenFencesNV(GLsizei n, GLuint* fences)
{
	TRACE("(GLsizei n = %d, GLuint* fences = %p)", n, fences);

	if(n < 0)
	{
		return error(GL_INVALID_VALUE);
	}

	auto context = es2::getContext();

	if(context)
	{
		for(int i = 0; i < n; i++)
		{
			fences[i] = context->createFence();
		}
	}
}

void GL_APIENTRY GenFramebuffers(GLsizei n, GLuint* framebuffers)
{
	TRACE("(GLsizei n = %d, GLuint* framebuffers = %p)", n, framebuffers);

	if(n < 0)
	{
		return error(GL_INVALID_VALUE);
	}

	auto context = es2::getContext();

	if(context)
	{
		for(int i = 0; i < n; i++)
		{
			framebuffers[i] = context->createFramebuffer();
		}
	}
}

void GL_APIENTRY GenQueriesEXT(GLsizei n, GLuint* ids)
{
	TRACE("(GLsizei n = %d, GLuint* ids = %p)", n, ids);

	if(n < 0)
	{
		return error(GL_INVALID_VALUE);
	}

	auto context = es2::getContext();

	if(context)
	{
		for(int i = 0; i < n; i++)
		{
			ids[i] = context->createQuery();
		}
	}
}

void GL_APIENTRY GenRenderbuffers(GLsizei n, GLuint* renderbuffers)
{
	TRACE("(GLsizei n = %d, GLuint* renderbuffers = %p)", n, renderbuffers);

	if(n < 0)
	{
		return error(GL_INVALID_VALUE);
	}

	auto context = es2::getContext();

	if(context)
	{
		for(int i = 0; i < n; i++)
		{
			renderbuffers[i] = context->createRenderbuffer();
		}
	}
}

void GL_APIENTRY GenTextures(GLsizei n, GLuint* textures)
{
	TRACE("(GLsizei n = %d, GLuint* textures = %p)", n, textures);

	if(n < 0)
	{
		return error(GL_INVALID_VALUE);
	}

	auto context = es2::getContext();

	if(context)
	{
		for(int i = 0; i < n; i++)
		{
			textures[i] = context->createTexture();
		}
	}
}

void GL_APIENTRY GetActiveAttrib(GLuint program, GLuint index, GLsizei bufsize, GLsizei *length, GLint *size, GLenum *type, GLchar *name)
{
	TRACE("(GLuint program = %d, GLuint index = %d, GLsizei bufsize = %d, GLsizei *length = %p, "
	      "GLint *size = %p, GLenum *type = %p, GLchar *name = %p)",
	      program, index, bufsize, length, size, type, name);

	if(bufsize < 0)
	{
		return error(GL_INVALID_VALUE);
	}

	auto context = es2::getContext();

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

void GL_APIENTRY GetActiveUniform(GLuint program, GLuint index, GLsizei bufsize, GLsizei* length, GLint* size, GLenum* type, GLchar* name)
{
	TRACE("(GLuint program = %d, GLuint index = %d, GLsizei bufsize = %d, "
	      "GLsizei* length = %p, GLint* size = %p, GLenum* type = %p, GLchar* name = %s)",
	      program, index, bufsize, length, size, type, name);

	if(bufsize < 0)
	{
		return error(GL_INVALID_VALUE);
	}

	auto context = es2::getContext();

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

void GL_APIENTRY GetAttachedShaders(GLuint program, GLsizei maxcount, GLsizei* count, GLuint* shaders)
{
	TRACE("(GLuint program = %d, GLsizei maxcount = %d, GLsizei* count = %p, GLuint* shaders = %p)",
	      program, maxcount, count, shaders);

	if(maxcount < 0)
	{
		return error(GL_INVALID_VALUE);
	}

	auto context = es2::getContext();

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

int GL_APIENTRY GetAttribLocation(GLuint program, const GLchar* name)
{
	TRACE("(GLuint program = %d, const GLchar* name = %s)", program, name);

	auto context = es2::getContext();

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

void GL_APIENTRY GetBooleanv(GLenum pname, GLboolean* params)
{
	TRACE("(GLenum pname = 0x%X, GLboolean* params = %p)",  pname, params);

	auto context = es2::getContext();

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
				GLfloat *floatParams = nullptr;
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
				GLint *intParams = nullptr;
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

void GL_APIENTRY GetBufferParameteriv(GLenum target, GLenum pname, GLint* params)
{
	TRACE("(GLenum target = 0x%X, GLenum pname = 0x%X, GLint* params = %p)", target, pname, params);

	auto context = es2::getContext();

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

		switch(pname)
		{
		case GL_BUFFER_USAGE:
			*params = buffer->usage();
			break;
		case GL_BUFFER_SIZE:
			*params = (GLint)buffer->size();
			break;
		case GL_BUFFER_ACCESS_FLAGS:
			*params = buffer->access();
			break;
		case GL_BUFFER_MAPPED:
			*params = buffer->isMapped();
			break;
		case GL_BUFFER_MAP_LENGTH:
			*params = (GLint)buffer->length();
			break;
		case GL_BUFFER_MAP_OFFSET:
			*params = (GLint)buffer->offset();
			break;
		default:
			return error(GL_INVALID_ENUM);
		}
	}
}

GLenum GL_APIENTRY GetError(void)
{
	TRACE("()");

	auto context = es2::getContext();

	if(context)
	{
		return context->getError();
	}

	return GL_NO_ERROR;
}

void GL_APIENTRY GetFenceivNV(GLuint fence, GLenum pname, GLint *params)
{
	TRACE("(GLuint fence = %d, GLenum pname = 0x%X, GLint *params = %p)", fence, pname, params);

	auto context = es2::getContext();

	if(context)
	{
		es2::Fence *fenceObject = context->getFence(fence);

		if(!fenceObject)
		{
			return error(GL_INVALID_OPERATION);
		}

		fenceObject->getFenceiv(pname, params);
	}
}

void GL_APIENTRY GetFloatv(GLenum pname, GLfloat* params)
{
	TRACE("(GLenum pname = 0x%X, GLfloat* params = %p)", pname, params);

	auto context = es2::getContext();

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
				GLboolean *boolParams = nullptr;
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
				GLint *intParams = nullptr;
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

void GL_APIENTRY GetFramebufferAttachmentParameteriv(GLenum target, GLenum attachment, GLenum pname, GLint* params)
{
	TRACE("(GLenum target = 0x%X, GLenum attachment = 0x%X, GLenum pname = 0x%X, GLint* params = %p)",
	      target, attachment, pname, params);

	auto context = es2::getContext();

	if(context)
	{
		if(target != GL_FRAMEBUFFER && target != GL_DRAW_FRAMEBUFFER && target != GL_READ_FRAMEBUFFER)
		{
			return error(GL_INVALID_ENUM);
		}

		GLuint framebufferName = 0;

		if(target == GL_READ_FRAMEBUFFER)
		{
			framebufferName = context->getReadFramebufferName();
		}
		else
		{
			framebufferName = context->getDrawFramebufferName();
		}

		switch(attachment)
		{
		case GL_BACK:
		case GL_DEPTH:
		case GL_STENCIL:
			if(framebufferName != 0)
			{
				return error(GL_INVALID_OPERATION);
			}
			break;
		case GL_DEPTH_ATTACHMENT:
		case GL_STENCIL_ATTACHMENT:
			if(framebufferName == 0)
			{
				return error(GL_INVALID_OPERATION);
			}
			break;
		case GL_DEPTH_STENCIL_ATTACHMENT:
			if(framebufferName == 0)
			{
				return error(GL_INVALID_OPERATION);
			}
			break;
		default:
			if(framebufferName == 0)
			{
				return error(GL_INVALID_OPERATION);
			}

			if(attachment < GL_COLOR_ATTACHMENT0 || attachment > GL_COLOR_ATTACHMENT31)
			{
				return error(GL_INVALID_ENUM);
			}

			if((attachment - GL_COLOR_ATTACHMENT0) >= MAX_COLOR_ATTACHMENTS)
			{
				return error(GL_INVALID_OPERATION);
			}
			break;
		}

		es2::Framebuffer *framebuffer = context->getFramebuffer(framebufferName);

		if(!framebuffer)
		{
			return error(GL_INVALID_OPERATION);
		}

		GLenum attachmentType;
		GLuint attachmentHandle;
		GLint attachmentLayer;
		Renderbuffer *renderbuffer = nullptr;
		switch(attachment)
		{
		case GL_BACK:
			attachmentType = framebuffer->getColorbufferType(0);
			attachmentHandle = framebuffer->getColorbufferName(0);
			attachmentLayer = framebuffer->getColorbufferLayer(0);
			renderbuffer = framebuffer->getColorbuffer(0);
			break;
		case GL_DEPTH:
		case GL_DEPTH_ATTACHMENT:
			attachmentType = framebuffer->getDepthbufferType();
			attachmentHandle = framebuffer->getDepthbufferName();
			attachmentLayer = framebuffer->getDepthbufferLayer();
			renderbuffer = framebuffer->getDepthbuffer();
			break;
		case GL_STENCIL:
		case GL_STENCIL_ATTACHMENT:
			attachmentType = framebuffer->getStencilbufferType();
			attachmentHandle = framebuffer->getStencilbufferName();
			attachmentLayer = framebuffer->getStencilbufferLayer();
			renderbuffer = framebuffer->getStencilbuffer();
			break;
		case GL_DEPTH_STENCIL_ATTACHMENT:
			attachmentType = framebuffer->getDepthbufferType();
			attachmentHandle = framebuffer->getDepthbufferName();
			attachmentLayer = framebuffer->getDepthbufferLayer();
			renderbuffer = framebuffer->getDepthbuffer();

			if(attachmentHandle != framebuffer->getStencilbufferName())
			{
				// Different attachments to DEPTH and STENCIL, query fails
				return error(GL_INVALID_OPERATION);
			}
			break;
		default:
			ASSERT((unsigned int)(attachment - GL_COLOR_ATTACHMENT0) < MAX_COLOR_ATTACHMENTS);
			attachmentType = framebuffer->getColorbufferType(attachment - GL_COLOR_ATTACHMENT0);
			attachmentHandle = framebuffer->getColorbufferName(attachment - GL_COLOR_ATTACHMENT0);
			attachmentLayer = framebuffer->getColorbufferLayer(attachment - GL_COLOR_ATTACHMENT0);
			renderbuffer = framebuffer->getColorbuffer(attachment - GL_COLOR_ATTACHMENT0);
			break;
		}

		GLenum attachmentObjectType = GL_NONE;   // Type category
		if(framebufferName == 0)
		{
			attachmentObjectType = GL_FRAMEBUFFER_DEFAULT;
		}
		else if(attachmentType == GL_NONE || Framebuffer::IsRenderbuffer(attachmentType))
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
					*params = renderbuffer->getLevel();
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
				*params = attachmentLayer;
				break;
			case GL_FRAMEBUFFER_ATTACHMENT_RED_SIZE:
				*params = renderbuffer->getRedSize();
				break;
			case GL_FRAMEBUFFER_ATTACHMENT_GREEN_SIZE:
				*params = renderbuffer->getGreenSize();
				break;
			case GL_FRAMEBUFFER_ATTACHMENT_BLUE_SIZE:
				*params = renderbuffer->getBlueSize();
				break;
			case GL_FRAMEBUFFER_ATTACHMENT_ALPHA_SIZE:
				*params = renderbuffer->getAlphaSize();
				break;
			case GL_FRAMEBUFFER_ATTACHMENT_DEPTH_SIZE:
				*params = renderbuffer->getDepthSize();
				break;
			case GL_FRAMEBUFFER_ATTACHMENT_STENCIL_SIZE:
				*params = renderbuffer->getStencilSize();
				break;
			case GL_FRAMEBUFFER_ATTACHMENT_COMPONENT_TYPE:
		//	case GL_FRAMEBUFFER_ATTACHMENT_COMPONENT_TYPE_EXT:   // GL_EXT_color_buffer_half_float
				if(attachment == GL_DEPTH_STENCIL_ATTACHMENT)
				{
					return error(GL_INVALID_OPERATION);
				}

				*params = GetComponentType(renderbuffer->getFormat(), attachment);
				break;
			case GL_FRAMEBUFFER_ATTACHMENT_COLOR_ENCODING:
				*params = GetColorEncoding(renderbuffer->getFormat());
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
				*params = 0;
				break;
			default:
				return error(GL_INVALID_OPERATION);
			}
		}
	}
}

GLenum GL_APIENTRY GetGraphicsResetStatusEXT(void)
{
	TRACE("()");

	return GL_NO_ERROR;
}

void GL_APIENTRY GetIntegerv(GLenum pname, GLint* params)
{
	TRACE("(GLenum pname = 0x%X, GLint* params = %p)", pname, params);

	auto context = es2::getContext();

	if(!context)
	{
		// Not strictly an error, but probably unintended or attempting to rely on non-compliant behavior
		ERR("glGetIntegerv() called without current context.");

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
				GLboolean *boolParams = nullptr;
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
				GLfloat *floatParams = nullptr;
				floatParams = new GLfloat[numParams];

				context->getFloatv(pname, floatParams);

				for(unsigned int i = 0; i < numParams; ++i)
				{
					if(pname == GL_DEPTH_RANGE || pname == GL_COLOR_CLEAR_VALUE || pname == GL_DEPTH_CLEAR_VALUE || pname == GL_BLEND_COLOR)
					{
						params[i] = convert_float_fixed(floatParams[i]);
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

void GL_APIENTRY GetProgramiv(GLuint program, GLenum pname, GLint* params)
{
	TRACE("(GLuint program = %d, GLenum pname = 0x%X, GLint* params = %p)", program, pname, params);

	auto context = es2::getContext();

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
			*params = (GLint)programObject->getInfoLogLength();
			return;
		case GL_ATTACHED_SHADERS:
			*params = programObject->getAttachedShadersCount();
			return;
		case GL_ACTIVE_ATTRIBUTES:
			*params = (GLint)programObject->getActiveAttributeCount();
			return;
		case GL_ACTIVE_ATTRIBUTE_MAX_LENGTH:
			*params = programObject->getActiveAttributeMaxLength();
			return;
		case GL_ACTIVE_UNIFORMS:
			*params = (GLint)programObject->getActiveUniformCount();
			return;
		case GL_ACTIVE_UNIFORM_MAX_LENGTH:
			*params = programObject->getActiveUniformMaxLength();
			return;
		case GL_ACTIVE_UNIFORM_BLOCKS:
			*params = (GLint)programObject->getActiveUniformBlockCount();
			return;
		case GL_ACTIVE_UNIFORM_BLOCK_MAX_NAME_LENGTH:
			*params = programObject->getActiveUniformBlockMaxLength();
			return;
		case GL_TRANSFORM_FEEDBACK_BUFFER_MODE:
			*params = programObject->getTransformFeedbackBufferMode();
			return;
		case GL_TRANSFORM_FEEDBACK_VARYINGS:
			*params = programObject->getTransformFeedbackVaryingCount();
			return;
		case GL_TRANSFORM_FEEDBACK_VARYING_MAX_LENGTH:
			*params = programObject->getTransformFeedbackVaryingMaxLength();
			return;
		case GL_PROGRAM_BINARY_RETRIEVABLE_HINT:
			*params = programObject->getBinaryRetrievableHint();
			return;
		case GL_PROGRAM_BINARY_LENGTH:
			*params = programObject->getBinaryLength();
			return;
		default:
			return error(GL_INVALID_ENUM);
		}
	}
}

void GL_APIENTRY GetProgramInfoLog(GLuint program, GLsizei bufsize, GLsizei* length, GLchar* infolog)
{
	TRACE("(GLuint program = %d, GLsizei bufsize = %d, GLsizei* length = %p, GLchar* infolog = %p)",
	      program, bufsize, length, infolog);

	if(bufsize < 0)
	{
		return error(GL_INVALID_VALUE);
	}

	auto context = es2::getContext();

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

		programObject->getInfoLog(bufsize, length, infolog);
	}
}

void GL_APIENTRY GetQueryivEXT(GLenum target, GLenum pname, GLint *params)
{
	TRACE("GLenum target = 0x%X, GLenum pname = 0x%X, GLint *params = %p)", target, pname, params);

	switch(pname)
	{
	case GL_CURRENT_QUERY_EXT:
		break;
	default:
		return error(GL_INVALID_ENUM);
	}

	auto context = es2::getContext();

	if(context)
	{
		params[0] = context->getActiveQuery(target);
	}
}

void GL_APIENTRY GetQueryObjectuivEXT(GLuint name, GLenum pname, GLuint *params)
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

	auto context = es2::getContext();

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

void GL_APIENTRY GetRenderbufferParameteriv(GLenum target, GLenum pname, GLint* params)
{
	TRACE("(GLenum target = 0x%X, GLenum pname = 0x%X, GLint* params = %p)", target, pname, params);

	auto context = es2::getContext();

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
		case GL_RENDERBUFFER_INTERNAL_FORMAT:
			{
				GLint internalformat = renderbuffer->getFormat();
				*params = (internalformat == GL_NONE) ? GL_RGBA4 : internalformat;
			}
			break;
		case GL_RENDERBUFFER_RED_SIZE:        *params = renderbuffer->getRedSize();     break;
		case GL_RENDERBUFFER_GREEN_SIZE:      *params = renderbuffer->getGreenSize();   break;
		case GL_RENDERBUFFER_BLUE_SIZE:       *params = renderbuffer->getBlueSize();    break;
		case GL_RENDERBUFFER_ALPHA_SIZE:      *params = renderbuffer->getAlphaSize();   break;
		case GL_RENDERBUFFER_DEPTH_SIZE:      *params = renderbuffer->getDepthSize();   break;
		case GL_RENDERBUFFER_STENCIL_SIZE:    *params = renderbuffer->getStencilSize(); break;
		case GL_RENDERBUFFER_SAMPLES:         *params = renderbuffer->getSamples();     break;
		default:
			return error(GL_INVALID_ENUM);
		}
	}
}

void GL_APIENTRY GetShaderiv(GLuint shader, GLenum pname, GLint* params)
{
	TRACE("(GLuint shader = %d, GLenum pname = %d, GLint* params = %p)", shader, pname, params);

	auto context = es2::getContext();

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
			*params = (GLint)shaderObject->getInfoLogLength();
			return;
		case GL_SHADER_SOURCE_LENGTH:
			*params = (GLint)shaderObject->getSourceLength();
			return;
		default:
			return error(GL_INVALID_ENUM);
		}
	}
}

void GL_APIENTRY GetShaderInfoLog(GLuint shader, GLsizei bufsize, GLsizei* length, GLchar* infolog)
{
	TRACE("(GLuint shader = %d, GLsizei bufsize = %d, GLsizei* length = %p, GLchar* infolog = %p)",
	      shader, bufsize, length, infolog);

	if(bufsize < 0)
	{
		return error(GL_INVALID_VALUE);
	}

	auto context = es2::getContext();

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

		shaderObject->getInfoLog(bufsize, length, infolog);
	}
}

void GL_APIENTRY GetShaderPrecisionFormat(GLenum shadertype, GLenum precisiontype, GLint* range, GLint* precision)
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

void GL_APIENTRY GetShaderSource(GLuint shader, GLsizei bufsize, GLsizei* length, GLchar* source)
{
	TRACE("(GLuint shader = %d, GLsizei bufsize = %d, GLsizei* length = %p, GLchar* source = %p)",
	      shader, bufsize, length, source);

	if(bufsize < 0)
	{
		return error(GL_INVALID_VALUE);
	}

	auto context = es2::getContext();

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

		shaderObject->getSource(bufsize, length, source);
	}
}

const GLubyte* GL_APIENTRY GetString(GLenum name)
{
	TRACE("(GLenum name = 0x%X)", name);

	switch(name)
	{
	case GL_VENDOR:
		return (GLubyte*)"Google Inc.";
	case GL_RENDERER:
		return (GLubyte*)"Google SwiftShader";
	case GL_VERSION:
		return (GLubyte*)"OpenGL ES 3.0 SwiftShader " VERSION_STRING;
	case GL_SHADING_LANGUAGE_VERSION:
		return (GLubyte*)"OpenGL ES GLSL ES 3.00 SwiftShader " VERSION_STRING;
	case GL_EXTENSIONS:
		{
			auto context = es2::getContext();
			return context ? context->getExtensions(GL_INVALID_INDEX) : (GLubyte*)nullptr;
		}
	default:
		return error(GL_INVALID_ENUM, (GLubyte*)nullptr);
	}
}

void GL_APIENTRY GetTexParameterfv(GLenum target, GLenum pname, GLfloat* params)
{
	TRACE("(GLenum target = 0x%X, GLenum pname = 0x%X, GLfloat* params = %p)", target, pname, params);

	auto context = es2::getContext();

	if(context)
	{
		es2::Texture *texture = context->getTargetTexture(target);

		if(!texture)
		{
			return;
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
			*params = (GLfloat)texture->getBaseLevel();
			break;
		case GL_TEXTURE_COMPARE_FUNC:
			*params = (GLfloat)texture->getCompareFunc();
			break;
		case GL_TEXTURE_COMPARE_MODE:
			*params = (GLfloat)texture->getCompareMode();
			break;
		case GL_TEXTURE_IMMUTABLE_FORMAT:
			*params = (GLfloat)texture->getImmutableFormat();
			break;
		case GL_TEXTURE_IMMUTABLE_LEVELS:
			*params = (GLfloat)texture->getImmutableLevels();
			break;
		case GL_TEXTURE_MAX_LEVEL:
			*params = (GLfloat)texture->getMaxLevel();
			break;
		case GL_TEXTURE_MAX_LOD:
			*params = texture->getMaxLOD();
			break;
		case GL_TEXTURE_MIN_LOD:
			*params = texture->getMinLOD();
			break;
		case GL_TEXTURE_SWIZZLE_R:
			*params = (GLfloat)texture->getSwizzleR();
			break;
		case GL_TEXTURE_SWIZZLE_G:
			*params = (GLfloat)texture->getSwizzleG();
			break;
		case GL_TEXTURE_SWIZZLE_B:
			*params = (GLfloat)texture->getSwizzleB();
			break;
		case GL_TEXTURE_SWIZZLE_A:
			*params = (GLfloat)texture->getSwizzleA();
			break;
		default:
			return error(GL_INVALID_ENUM);
		}
	}
}

void GL_APIENTRY GetTexParameteriv(GLenum target, GLenum pname, GLint* params)
{
	TRACE("(GLenum target = 0x%X, GLenum pname = 0x%X, GLint* params = %p)", target, pname, params);

	auto context = es2::getContext();

	if(context)
	{
		es2::Texture *texture = context->getTargetTexture(target);

		if(!texture)
		{
			return;
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
			*params = texture->getBaseLevel();
			break;
		case GL_TEXTURE_COMPARE_FUNC:
			*params = (GLint)texture->getCompareFunc();
			break;
		case GL_TEXTURE_COMPARE_MODE:
			*params = (GLint)texture->getCompareMode();
			break;
		case GL_TEXTURE_IMMUTABLE_FORMAT:
			*params = (GLint)texture->getImmutableFormat();
			break;
		case GL_TEXTURE_IMMUTABLE_LEVELS:
			*params = (GLint)texture->getImmutableLevels();
			break;
		case GL_TEXTURE_MAX_LEVEL:
			*params = texture->getMaxLevel();
			break;
		case GL_TEXTURE_MAX_LOD:
			*params = (GLint)roundf(texture->getMaxLOD());
			break;
		case GL_TEXTURE_MIN_LOD:
			*params = (GLint)roundf(texture->getMinLOD());
			break;
		case GL_TEXTURE_SWIZZLE_R:
			*params = (GLint)texture->getSwizzleR();
			break;
		case GL_TEXTURE_SWIZZLE_G:
			*params = (GLint)texture->getSwizzleG();
			break;
		case GL_TEXTURE_SWIZZLE_B:
			*params = (GLint)texture->getSwizzleB();
			break;
		case GL_TEXTURE_SWIZZLE_A:
			*params = (GLint)texture->getSwizzleA();
			break;
		default:
			return error(GL_INVALID_ENUM);
		}
	}
}

void GL_APIENTRY GetnUniformfvEXT(GLuint program, GLint location, GLsizei bufSize, GLfloat* params)
{
	TRACE("(GLuint program = %d, GLint location = %d, GLsizei bufSize = %d, GLfloat* params = %p)",
	      program, location, bufSize, params);

	if(bufSize < 0)
	{
		return error(GL_INVALID_VALUE);
	}

	auto context = es2::getContext();

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

		if(!programObject->isLinked())
		{
			return error(GL_INVALID_OPERATION);
		}

		if(!programObject->getUniformfv(location, &bufSize, params))
		{
			return error(GL_INVALID_OPERATION);
		}
	}
}

void GL_APIENTRY GetUniformfv(GLuint program, GLint location, GLfloat* params)
{
	TRACE("(GLuint program = %d, GLint location = %d, GLfloat* params = %p)", program, location, params);

	auto context = es2::getContext();

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

		if(!programObject->isLinked())
		{
			return error(GL_INVALID_OPERATION);
		}

		if(!programObject->getUniformfv(location, nullptr, params))
		{
			return error(GL_INVALID_OPERATION);
		}
	}
}

void GL_APIENTRY GetnUniformivEXT(GLuint program, GLint location, GLsizei bufSize, GLint* params)
{
	TRACE("(GLuint program = %d, GLint location = %d, GLsizei bufSize = %d, GLint* params = %p)",
	      program, location, bufSize, params);

	if(bufSize < 0)
	{
		return error(GL_INVALID_VALUE);
	}

	auto context = es2::getContext();

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

		if(!programObject->isLinked())
		{
			return error(GL_INVALID_OPERATION);
		}

		if(!programObject->getUniformiv(location, &bufSize, params))
		{
			return error(GL_INVALID_OPERATION);
		}
	}
}

void GL_APIENTRY GetUniformiv(GLuint program, GLint location, GLint* params)
{
	TRACE("(GLuint program = %d, GLint location = %d, GLint* params = %p)", program, location, params);

	auto context = es2::getContext();

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

		if(!programObject->isLinked())
		{
			return error(GL_INVALID_OPERATION);
		}

		if(!programObject->getUniformiv(location, nullptr, params))
		{
			return error(GL_INVALID_OPERATION);
		}
	}
}

int GL_APIENTRY GetUniformLocation(GLuint program, const GLchar* name)
{
	TRACE("(GLuint program = %d, const GLchar* name = %s)", program, name);

	auto context = es2::getContext();

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

void GL_APIENTRY GetVertexAttribfv(GLuint index, GLenum pname, GLfloat* params)
{
	TRACE("(GLuint index = %d, GLenum pname = 0x%X, GLfloat* params = %p)", index, pname, params);

	auto context = es2::getContext();

	if(context)
	{
		if(index >= es2::MAX_VERTEX_ATTRIBS)
		{
			return error(GL_INVALID_VALUE);
		}

		const es2::VertexAttribute &attribState = context->getVertexAttribState(index);

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
			*params = (GLfloat)(attribState.mPureInteger ? GL_TRUE : GL_FALSE);
			break;
		default:
			return error(GL_INVALID_ENUM);
		}
	}
}

void GL_APIENTRY GetVertexAttribiv(GLuint index, GLenum pname, GLint* params)
{
	TRACE("(GLuint index = %d, GLenum pname = 0x%X, GLint* params = %p)", index, pname, params);

	auto context = es2::getContext();

	if(context)
	{
		if(index >= es2::MAX_VERTEX_ATTRIBS)
		{
			return error(GL_INVALID_VALUE);
		}

		const es2::VertexAttribute &attribState = context->getVertexAttribState(index);

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
			*params = (attribState.mPureInteger ? GL_TRUE : GL_FALSE);
			break;
		default:
			return error(GL_INVALID_ENUM);
		}
	}
}

void GL_APIENTRY GetVertexAttribPointerv(GLuint index, GLenum pname, GLvoid** pointer)
{
	TRACE("(GLuint index = %d, GLenum pname = 0x%X, GLvoid** pointer = %p)", index, pname, pointer);

	auto context = es2::getContext();

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

void GL_APIENTRY Hint(GLenum target, GLenum mode)
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

	auto context = es2::getContext();

	if(context)
	{
		switch(target)
		{
		case GL_GENERATE_MIPMAP_HINT:
			context->setGenerateMipmapHint(mode);
			break;
		case GL_FRAGMENT_SHADER_DERIVATIVE_HINT_OES:
			context->setFragmentShaderDerivativeHint(mode);
			break;
		case GL_TEXTURE_FILTERING_HINT_CHROMIUM:
			context->setTextureFilteringHint(mode);
			break;
		default:
			return error(GL_INVALID_ENUM);
		}
	}
}

GLboolean GL_APIENTRY IsBuffer(GLuint buffer)
{
	TRACE("(GLuint buffer = %d)", buffer);

	auto context = es2::getContext();

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

GLboolean GL_APIENTRY IsEnabled(GLenum cap)
{
	TRACE("(GLenum cap = 0x%X)", cap);

	auto context = es2::getContext();

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
		case GL_PRIMITIVE_RESTART_FIXED_INDEX: return context->isPrimitiveRestartFixedIndexEnabled();
		case GL_RASTERIZER_DISCARD:       return context->isRasterizerDiscardEnabled();
		default:
			return error(GL_INVALID_ENUM, false);
		}
	}

	return false;
}

GLboolean GL_APIENTRY IsFenceNV(GLuint fence)
{
	TRACE("(GLuint fence = %d)", fence);

	auto context = es2::getContext();

	if(context)
	{
		es2::Fence *fenceObject = context->getFence(fence);

		if(!fenceObject)
		{
			return GL_FALSE;
		}

		return fenceObject->isFence();
	}

	return GL_FALSE;
}

GLboolean GL_APIENTRY IsFramebuffer(GLuint framebuffer)
{
	TRACE("(GLuint framebuffer = %d)", framebuffer);

	auto context = es2::getContext();

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

GLboolean GL_APIENTRY IsProgram(GLuint program)
{
	TRACE("(GLuint program = %d)", program);

	auto context = es2::getContext();

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

GLboolean GL_APIENTRY IsQueryEXT(GLuint name)
{
	TRACE("(GLuint name = %d)", name);

	if(name == 0)
	{
		return GL_FALSE;
	}

	auto context = es2::getContext();

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

GLboolean GL_APIENTRY IsRenderbuffer(GLuint renderbuffer)
{
	TRACE("(GLuint renderbuffer = %d)", renderbuffer);

	auto context = es2::getContext();

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

GLboolean GL_APIENTRY IsShader(GLuint shader)
{
	TRACE("(GLuint shader = %d)", shader);

	auto context = es2::getContext();

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

GLboolean GL_APIENTRY IsTexture(GLuint texture)
{
	TRACE("(GLuint texture = %d)", texture);

	auto context = es2::getContext();

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

void GL_APIENTRY LineWidth(GLfloat width)
{
	TRACE("(GLfloat width = %f)", width);

	if(width <= 0.0f)
	{
		return error(GL_INVALID_VALUE);
	}

	auto context = es2::getContext();

	if(context)
	{
		context->setLineWidth(width);
	}
}

void GL_APIENTRY LinkProgram(GLuint program)
{
	TRACE("(GLuint program = %d)", program);

	auto context = es2::getContext();

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

		if(programObject == context->getCurrentProgram())
		{
			es2::TransformFeedback* transformFeedback = context->getTransformFeedback();
			if(transformFeedback && transformFeedback->isActive())
			{
				return error(GL_INVALID_OPERATION);
			}
		}

		programObject->link();
	}
}

void GL_APIENTRY PixelStorei(GLenum pname, GLint param)
{
	TRACE("(GLenum pname = 0x%X, GLint param = %d)", pname, param);

	auto context = es2::getContext();

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
		case GL_PACK_ROW_LENGTH:
			if(param < 0)
			{
				return error(GL_INVALID_VALUE);
			}
			context->setPackRowLength(param);
			break;
		case GL_PACK_SKIP_PIXELS:
			if(param < 0)
			{
				return error(GL_INVALID_VALUE);
			}
			context->setPackSkipPixels(param);
			break;
		case GL_PACK_SKIP_ROWS:
			if(param < 0)
			{
				return error(GL_INVALID_VALUE);
			}
			context->setPackSkipRows(param);
			break;
		case GL_UNPACK_ROW_LENGTH:
			if(param < 0)
			{
				return error(GL_INVALID_VALUE);
			}
			context->setUnpackRowLength(param);
			break;
		case GL_UNPACK_IMAGE_HEIGHT:
			if(param < 0)
			{
				return error(GL_INVALID_VALUE);
			}
			context->setUnpackImageHeight(param);
			break;
		case GL_UNPACK_SKIP_PIXELS:
			if(param < 0)
			{
				return error(GL_INVALID_VALUE);
			}
			context->setUnpackSkipPixels(param);
			break;
		case GL_UNPACK_SKIP_ROWS:
			if(param < 0)
			{
				return error(GL_INVALID_VALUE);
			}
			context->setUnpackSkipRows(param);
			break;
		case GL_UNPACK_SKIP_IMAGES:
			if(param < 0)
			{
				return error(GL_INVALID_VALUE);
			}
			context->setUnpackSkipImages(param);
			break;
		default:
			return error(GL_INVALID_ENUM);
		}
	}
}

void GL_APIENTRY PolygonOffset(GLfloat factor, GLfloat units)
{
	TRACE("(GLfloat factor = %f, GLfloat units = %f)", factor, units);

	auto context = es2::getContext();

	if(context)
	{
		context->setPolygonOffsetParams(factor, units);
	}
}

void GL_APIENTRY ReadnPixelsEXT(GLint x, GLint y, GLsizei width, GLsizei height,
                                GLenum format, GLenum type, GLsizei bufSize, GLvoid *data)
{
	TRACE("(GLint x = %d, GLint y = %d, GLsizei width = %d, GLsizei height = %d, "
	      "GLenum format = 0x%X, GLenum type = 0x%X, GLsizei bufSize = 0x%d, GLvoid *data = %p)",
	      x, y, width, height, format, type, bufSize, data);

	if(width < 0 || height < 0 || bufSize < 0)
	{
		return error(GL_INVALID_VALUE);
	}

	auto context = es2::getContext();

	if(context)
	{
		context->readPixels(x, y, width, height, format, type, &bufSize, data);
	}
}

void GL_APIENTRY ReadPixels(GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid* pixels)
{
	TRACE("(GLint x = %d, GLint y = %d, GLsizei width = %d, GLsizei height = %d, "
	      "GLenum format = 0x%X, GLenum type = 0x%X, GLvoid* pixels = %p)",
	      x, y, width, height, format, type,  pixels);

	if(width < 0 || height < 0)
	{
		return error(GL_INVALID_VALUE);
	}

	auto context = es2::getContext();

	if(context)
	{
		context->readPixels(x, y, width, height, format, type, nullptr, pixels);
	}
}

void GL_APIENTRY ReleaseShaderCompiler(void)
{
	TRACE("()");

	es2::Shader::releaseCompiler();
}

void GL_APIENTRY RenderbufferStorageMultisample(GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height)
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

	if(width < 0 || height < 0 || samples < 0 ||
	   width > es2::IMPLEMENTATION_MAX_RENDERBUFFER_SIZE ||
	   height > es2::IMPLEMENTATION_MAX_RENDERBUFFER_SIZE)
	{
		return error(GL_INVALID_VALUE);
	}

	if(samples > es2::IMPLEMENTATION_MAX_SAMPLES ||
	   (IsNonNormalizedInteger(internalformat) && samples > 0))
	{
		return error(GL_INVALID_OPERATION);
	}

	auto context = es2::getContext();

	if(context)
	{
		GLuint handle = context->getRenderbufferName();
		if(handle == 0)
		{
			return error(GL_INVALID_OPERATION);
		}

		if(IsColorRenderable(internalformat))
		{
			context->setRenderbufferStorage(new es2::Colorbuffer(width, height, internalformat, samples));
		}
		else if(IsDepthRenderable(internalformat) && IsStencilRenderable(internalformat))
		{
			context->setRenderbufferStorage(new es2::DepthStencilbuffer(width, height, internalformat, samples));
		}
		else if(IsDepthRenderable(internalformat))
		{
			context->setRenderbufferStorage(new es2::Depthbuffer(width, height, internalformat, samples));
		}
		else if(IsStencilRenderable(internalformat))
		{
			context->setRenderbufferStorage(new es2::Stencilbuffer(width, height, samples));
		}
		else error(GL_INVALID_ENUM);
	}
}

void GL_APIENTRY RenderbufferStorageMultisampleANGLE(GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height)
{
	RenderbufferStorageMultisample(target, samples, internalformat, width, height);
}

void GL_APIENTRY RenderbufferStorage(GLenum target, GLenum internalformat, GLsizei width, GLsizei height)
{
	RenderbufferStorageMultisample(target, 0, internalformat, width, height);
}

void GL_APIENTRY SampleCoverage(GLclampf value, GLboolean invert)
{
	TRACE("(GLclampf value = %f, GLboolean invert = %d)", value, invert);

	auto context = es2::getContext();

	if(context)
	{
		context->setSampleCoverageParams(es2::clamp01(value), invert != GL_FALSE);
	}
}

void GL_APIENTRY SetFenceNV(GLuint fence, GLenum condition)
{
	TRACE("(GLuint fence = %d, GLenum condition = 0x%X)", fence, condition);

	if(condition != GL_ALL_COMPLETED_NV)
	{
		return error(GL_INVALID_ENUM);
	}

	auto context = es2::getContext();

	if(context)
	{
		es2::Fence *fenceObject = context->getFence(fence);

		if(!fenceObject)
		{
			return error(GL_INVALID_OPERATION);
		}

		fenceObject->setFence(condition);
	}
}

void GL_APIENTRY Scissor(GLint x, GLint y, GLsizei width, GLsizei height)
{
	TRACE("(GLint x = %d, GLint y = %d, GLsizei width = %d, GLsizei height = %d)", x, y, width, height);

	if(width < 0 || height < 0)
	{
		return error(GL_INVALID_VALUE);
	}

	auto context = es2::getContext();

	if(context)
	{
		context->setScissorParams(x, y, width, height);
	}
}

void GL_APIENTRY ShaderBinary(GLsizei n, const GLuint* shaders, GLenum binaryformat, const GLvoid* binary, GLsizei length)
{
	TRACE("(GLsizei n = %d, const GLuint* shaders = %p, GLenum binaryformat = 0x%X, "
	      "const GLvoid* binary = %p, GLsizei length = %d)",
	      n, shaders, binaryformat, binary, length);

	// No binary shader formats are supported.
	return error(GL_INVALID_ENUM);
}

void GL_APIENTRY ShaderSource(GLuint shader, GLsizei count, const GLchar *const *string, const GLint *length)
{
	TRACE("(GLuint shader = %d, GLsizei count = %d, const GLchar** string = %p, const GLint* length = %p)",
	      shader, count, string, length);

	if(count < 0)
	{
		return error(GL_INVALID_VALUE);
	}

	auto context = es2::getContext();

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

void GL_APIENTRY StencilFuncSeparate(GLenum face, GLenum func, GLint ref, GLuint mask)
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

	auto context = es2::getContext();

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

void GL_APIENTRY StencilFunc(GLenum func, GLint ref, GLuint mask)
{
	StencilFuncSeparate(GL_FRONT_AND_BACK, func, ref, mask);
}

void GL_APIENTRY StencilMaskSeparate(GLenum face, GLuint mask)
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

	auto context = es2::getContext();

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

void GL_APIENTRY StencilMask(GLuint mask)
{
	StencilMaskSeparate(GL_FRONT_AND_BACK, mask);
}

void GL_APIENTRY StencilOpSeparate(GLenum face, GLenum fail, GLenum zfail, GLenum zpass)
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

	auto context = es2::getContext();

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

void GL_APIENTRY StencilOp(GLenum fail, GLenum zfail, GLenum zpass)
{
	StencilOpSeparate(GL_FRONT_AND_BACK, fail, zfail, zpass);
}

GLboolean GL_APIENTRY TestFenceNV(GLuint fence)
{
	TRACE("(GLuint fence = %d)", fence);

	auto context = es2::getContext();

	if(context)
	{
		es2::Fence *fenceObject = context->getFence(fence);

		if(!fenceObject)
		{
			return error(GL_INVALID_OPERATION, GL_TRUE);
		}

		return fenceObject->testFence();
	}

	return GL_TRUE;
}

void GL_APIENTRY TexImage2D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height,
                            GLint border, GLenum format, GLenum type, const GLvoid* data)
{
	TRACE("(GLenum target = 0x%X, GLint level = %d, GLint internalformat = %d, GLsizei width = %d, GLsizei height = %d, "
	      "GLint border = %d, GLenum format = 0x%X, GLenum type = 0x%X, const GLvoid* data =  %p)",
	      target, level, internalformat, width, height, border, format, type, data);

	if(!validImageSize(level, width, height))
	{
		return error(GL_INVALID_VALUE);
	}

	auto context = es2::getContext();

	if(context)
	{
		// GL_APPLE_texture_format_BGRA8888 allows (only) GL_BGRA_EXT / GL_RGBA, while
		// GL_EXT_texture_format_BGRA8888 also allows GL_BGRA_EXT / GL_BGRA_EXT.
		if(format == GL_BGRA_EXT && internalformat == GL_RGBA)
		{
			internalformat = GL_BGRA_EXT;
		}

		GLenum validationError = ValidateTextureFormatType(format, type, internalformat, target);
		if(validationError != GL_NO_ERROR)
		{
			return error(validationError);
		}

		if(border != 0)
		{
			return error(GL_INVALID_VALUE);
		}

		switch(target)
		{
		case GL_TEXTURE_RECTANGLE_ARB:
			if(level != 0)
			{
				return error(GL_INVALID_VALUE); // Defining level other than 0 is not allowed
			}
			// Fall through to GL_TEXTURE_2D case.
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

		validationError = context->getPixels(&data, type, context->getRequiredBufferSize(width, height, 1, format, type));
		if(validationError != GL_NO_ERROR)
		{
			return error(validationError);
		}

		GLint sizedInternalFormat = gl::GetSizedInternalFormat(internalformat, type);

		if(target == GL_TEXTURE_2D || target == GL_TEXTURE_RECTANGLE_ARB)
		{
			es2::Texture2D *texture = context->getTexture2D(target);

			if(!texture)
			{
				return error(GL_INVALID_OPERATION);
			}

			texture->setImage(level, width, height, sizedInternalFormat, format, type, context->getUnpackParameters(), data);
		}
		else
		{
			es2::TextureCubeMap *texture = context->getTextureCubeMap();

			if(!texture)
			{
				return error(GL_INVALID_OPERATION);
			}

			texture->setImage(target, level, width, height, sizedInternalFormat, format, type, context->getUnpackParameters(), data);
		}
	}
}

void GL_APIENTRY TexParameterf(GLenum target, GLenum pname, GLfloat param)
{
	TRACE("(GLenum target = 0x%X, GLenum pname = 0x%X, GLfloat param = %f)", target, pname, param);

	auto context = es2::getContext();

	if(context)
	{
		es2::Texture *texture = context->getTargetTexture(target);

		if(!texture)
		{
			return;
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
			if(!texture->setBaseLevel((GLint)(roundf(param))))
			{
				return error(GL_INVALID_VALUE);
			}
			break;
		case GL_TEXTURE_COMPARE_FUNC:
			if(!texture->setCompareFunc((GLenum)param))
			{
				return error(GL_INVALID_VALUE);
			}
			break;
		case GL_TEXTURE_COMPARE_MODE:
			if(!texture->setCompareMode((GLenum)param))
			{
				return error(GL_INVALID_VALUE);
			}
			break;
		case GL_TEXTURE_MAX_LEVEL:
			if(!texture->setMaxLevel((GLint)(roundf(param))))
			{
				return error(GL_INVALID_VALUE);
			}
			break;
		case GL_TEXTURE_MAX_LOD:
			if(!texture->setMaxLOD(param))
			{
				return error(GL_INVALID_VALUE);
			}
			break;
		case GL_TEXTURE_MIN_LOD:
			if(!texture->setMinLOD(param))
			{
				return error(GL_INVALID_VALUE);
			}
			break;
		case GL_TEXTURE_SWIZZLE_R:
			if(!texture->setSwizzleR((GLenum)param))
			{
				return error(GL_INVALID_VALUE);
			}
			break;
		case GL_TEXTURE_SWIZZLE_G:
			if(!texture->setSwizzleG((GLenum)param))
			{
				return error(GL_INVALID_VALUE);
			}
			break;
		case GL_TEXTURE_SWIZZLE_B:
			if(!texture->setSwizzleB((GLenum)param))
			{
				return error(GL_INVALID_VALUE);
			}
			break;
		case GL_TEXTURE_SWIZZLE_A:
			if(!texture->setSwizzleA((GLenum)param))
			{
				return error(GL_INVALID_VALUE);
			}
			break;
		default:
			return error(GL_INVALID_ENUM);
		}
	}
}

void GL_APIENTRY TexParameterfv(GLenum target, GLenum pname, const GLfloat* params)
{
	TexParameterf(target, pname, *params);
}

void GL_APIENTRY TexParameteri(GLenum target, GLenum pname, GLint param)
{
	TRACE("(GLenum target = 0x%X, GLenum pname = 0x%X, GLint param = %d)", target, pname, param);

	auto context = es2::getContext();

	if(context)
	{
		es2::Texture *texture = context->getTargetTexture(target);

		if(!texture)
		{
			return;
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
			if((texture->getTarget() == GL_TEXTURE_RECTANGLE_ARB) && (param != 0))
			{
				return error(GL_INVALID_OPERATION); // Base level has to be 0
			}
			if(!texture->setBaseLevel(param))
			{
				return error(GL_INVALID_VALUE);
			}
			break;
		case GL_TEXTURE_COMPARE_FUNC:
			if(!texture->setCompareFunc((GLenum)param))
			{
				return error(GL_INVALID_VALUE);
			}
			break;
		case GL_TEXTURE_COMPARE_MODE:
			if(!texture->setCompareMode((GLenum)param))
			{
				return error(GL_INVALID_VALUE);
			}
			break;
		case GL_TEXTURE_MAX_LEVEL:
			if(!texture->setMaxLevel(param))
			{
				return error(GL_INVALID_VALUE);
			}
			break;
		case GL_TEXTURE_MAX_LOD:
			if(!texture->setMaxLOD((GLfloat)param))
			{
				return error(GL_INVALID_VALUE);
			}
			break;
		case GL_TEXTURE_MIN_LOD:
			if(!texture->setMinLOD((GLfloat)param))
			{
				return error(GL_INVALID_VALUE);
			}
			break;
		case GL_TEXTURE_SWIZZLE_R:
			if(!texture->setSwizzleR((GLenum)param))
			{
				return error(GL_INVALID_VALUE);
			}
			break;
		case GL_TEXTURE_SWIZZLE_G:
			if(!texture->setSwizzleG((GLenum)param))
			{
				return error(GL_INVALID_VALUE);
			}
			break;
		case GL_TEXTURE_SWIZZLE_B:
			if(!texture->setSwizzleB((GLenum)param))
			{
				return error(GL_INVALID_VALUE);
			}
			break;
		case GL_TEXTURE_SWIZZLE_A:
			if(!texture->setSwizzleA((GLenum)param))
			{
				return error(GL_INVALID_VALUE);
			}
			break;
		default:
			return error(GL_INVALID_ENUM);
		}
	}
}

void GL_APIENTRY TexParameteriv(GLenum target, GLenum pname, const GLint* params)
{
	TexParameteri(target, pname, *params);
}

void GL_APIENTRY TexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height,
                               GLenum format, GLenum type, const GLvoid* data)
{
	TRACE("(GLenum target = 0x%X, GLint level = %d, GLint xoffset = %d, GLint yoffset = %d, "
	      "GLsizei width = %d, GLsizei height = %d, GLenum format = 0x%X, GLenum type = 0x%X, "
	      "const GLvoid* data = %p)",
	      target, level, xoffset, yoffset, width, height, format, type, data);

	if(!es2::IsTexImageTarget(target))
	{
		return error(GL_INVALID_ENUM);
	}

	if(level < 0 || level >= es2::IMPLEMENTATION_MAX_TEXTURE_LEVELS)
	{
		return error(GL_INVALID_VALUE);
	}

	if(xoffset < 0 || yoffset < 0 || width < 0 || height < 0)
	{
		return error(GL_INVALID_VALUE);
	}

	if(std::numeric_limits<GLsizei>::max() - xoffset < width || std::numeric_limits<GLsizei>::max() - yoffset < height)
	{
		return error(GL_INVALID_VALUE);
	}

	auto context = es2::getContext();

	if(context)
	{
		if(target == GL_TEXTURE_2D || target == GL_TEXTURE_RECTANGLE_ARB)
		{
			es2::Texture2D *texture = context->getTexture2D(target);

			GLenum validationError = ValidateSubImageParams(false, false, target, level, xoffset, yoffset, width, height, format, type, texture);
			if(validationError != GL_NO_ERROR)
			{
				return error(validationError);
			}

			validationError = context->getPixels(&data, type, context->getRequiredBufferSize(width, height, 1, format, type));
			if(validationError != GL_NO_ERROR)
			{
				return error(validationError);
			}

			texture->subImage(level, xoffset, yoffset, width, height, format, type, context->getUnpackParameters(), data);
		}
		else if(es2::IsCubemapTextureTarget(target))
		{
			es2::TextureCubeMap *texture = context->getTextureCubeMap();

			GLenum validationError = ValidateSubImageParams(false, false, target, level, xoffset, yoffset, width, height, format, type, texture);
			if(validationError != GL_NO_ERROR)
			{
				return error(validationError);
			}

			validationError = context->getPixels(&data, type, context->getRequiredBufferSize(width, height, 1, format, type));
			if(validationError != GL_NO_ERROR)
			{
				return error(validationError);
			}

			texture->subImage(target, level, xoffset, yoffset, width, height, format, type, context->getUnpackParameters(), data);
		}
		else UNREACHABLE(target);
	}
}

void GL_APIENTRY Uniform1fv(GLint location, GLsizei count, const GLfloat* v)
{
	TRACE("(GLint location = %d, GLsizei count = %d, const GLfloat* v = %p)", location, count, v);

	if(count < 0)
	{
		return error(GL_INVALID_VALUE);
	}

	auto context = es2::getContext();

	if(context)
	{
		es2::Program *program = context->getCurrentProgram();

		if(!program)
		{
			return error(GL_INVALID_OPERATION);
		}

		if(location == -1)
		{
			return;
		}

		if(!program->setUniform1fv(location, count, v))
		{
			return error(GL_INVALID_OPERATION);
		}
	}
}

void GL_APIENTRY Uniform1f(GLint location, GLfloat x)
{
	Uniform1fv(location, 1, &x);
}

void GL_APIENTRY Uniform1iv(GLint location, GLsizei count, const GLint* v)
{
	TRACE("(GLint location = %d, GLsizei count = %d, const GLint* v = %p)", location, count, v);

	if(count < 0)
	{
		return error(GL_INVALID_VALUE);
	}

	auto context = es2::getContext();

	if(context)
	{
		es2::Program *program = context->getCurrentProgram();

		if(!program)
		{
			return error(GL_INVALID_OPERATION);
		}

		if(location == -1)
		{
			return;
		}

		if(!program->setUniform1iv(location, count, v))
		{
			return error(GL_INVALID_OPERATION);
		}
	}
}

void GL_APIENTRY Uniform1i(GLint location, GLint x)
{
	Uniform1iv(location, 1, &x);
}

void GL_APIENTRY Uniform2fv(GLint location, GLsizei count, const GLfloat* v)
{
	TRACE("(GLint location = %d, GLsizei count = %d, const GLfloat* v = %p)", location, count, v);

	if(count < 0)
	{
		return error(GL_INVALID_VALUE);
	}

	auto context = es2::getContext();

	if(context)
	{
		es2::Program *program = context->getCurrentProgram();

		if(!program)
		{
			return error(GL_INVALID_OPERATION);
		}

		if(location == -1)
		{
			return;
		}

		if(!program->setUniform2fv(location, count, v))
		{
			return error(GL_INVALID_OPERATION);
		}
	}
}

void GL_APIENTRY Uniform2f(GLint location, GLfloat x, GLfloat y)
{
	GLfloat xy[2] = {x, y};

	Uniform2fv(location, 1, (GLfloat*)&xy);
}

void GL_APIENTRY Uniform2iv(GLint location, GLsizei count, const GLint* v)
{
	TRACE("(GLint location = %d, GLsizei count = %d, const GLint* v = %p)", location, count, v);

	if(count < 0)
	{
		return error(GL_INVALID_VALUE);
	}

	auto context = es2::getContext();

	if(context)
	{
		es2::Program *program = context->getCurrentProgram();

		if(!program)
		{
			return error(GL_INVALID_OPERATION);
		}

		if(location == -1)
		{
			return;
		}

		if(!program->setUniform2iv(location, count, v))
		{
			return error(GL_INVALID_OPERATION);
		}
	}
}

void GL_APIENTRY Uniform2i(GLint location, GLint x, GLint y)
{
	GLint xy[4] = {x, y};

	Uniform2iv(location, 1, (GLint*)&xy);
}

void GL_APIENTRY Uniform3fv(GLint location, GLsizei count, const GLfloat* v)
{
	TRACE("(GLint location = %d, GLsizei count = %d, const GLfloat* v = %p)", location, count, v);

	if(count < 0)
	{
		return error(GL_INVALID_VALUE);
	}

	auto context = es2::getContext();

	if(context)
	{
		es2::Program *program = context->getCurrentProgram();

		if(!program)
		{
			return error(GL_INVALID_OPERATION);
		}

		if(location == -1)
		{
			return;
		}

		if(!program->setUniform3fv(location, count, v))
		{
			return error(GL_INVALID_OPERATION);
		}
	}
}

void GL_APIENTRY Uniform3f(GLint location, GLfloat x, GLfloat y, GLfloat z)
{
	GLfloat xyz[3] = {x, y, z};

	Uniform3fv(location, 1, (GLfloat*)&xyz);
}

void GL_APIENTRY Uniform3iv(GLint location, GLsizei count, const GLint* v)
{
	TRACE("(GLint location = %d, GLsizei count = %d, const GLint* v = %p)", location, count, v);

	if(count < 0)
	{
		return error(GL_INVALID_VALUE);
	}

	auto context = es2::getContext();

	if(context)
	{
		es2::Program *program = context->getCurrentProgram();

		if(!program)
		{
			return error(GL_INVALID_OPERATION);
		}

		if(location == -1)
		{
			return;
		}

		if(!program->setUniform3iv(location, count, v))
		{
			return error(GL_INVALID_OPERATION);
		}
	}
}

void GL_APIENTRY Uniform3i(GLint location, GLint x, GLint y, GLint z)
{
	GLint xyz[3] = {x, y, z};

	Uniform3iv(location, 1, (GLint*)&xyz);
}

void GL_APIENTRY Uniform4fv(GLint location, GLsizei count, const GLfloat* v)
{
	TRACE("(GLint location = %d, GLsizei count = %d, const GLfloat* v = %p)", location, count, v);

	if(count < 0)
	{
		return error(GL_INVALID_VALUE);
	}

	auto context = es2::getContext();

	if(context)
	{
		es2::Program *program = context->getCurrentProgram();

		if(!program)
		{
			return error(GL_INVALID_OPERATION);
		}

		if(location == -1)
		{
			return;
		}

		if(!program->setUniform4fv(location, count, v))
		{
			return error(GL_INVALID_OPERATION);
		}
	}
}

void GL_APIENTRY Uniform4f(GLint location, GLfloat x, GLfloat y, GLfloat z, GLfloat w)
{
	GLfloat xyzw[4] = {x, y, z, w};

	Uniform4fv(location, 1, (GLfloat*)&xyzw);
}

void GL_APIENTRY Uniform4iv(GLint location, GLsizei count, const GLint* v)
{
	TRACE("(GLint location = %d, GLsizei count = %d, const GLint* v = %p)", location, count, v);

	if(count < 0)
	{
		return error(GL_INVALID_VALUE);
	}

	auto context = es2::getContext();

	if(context)
	{
		es2::Program *program = context->getCurrentProgram();

		if(!program)
		{
			return error(GL_INVALID_OPERATION);
		}

		if(location == -1)
		{
			return;
		}

		if(!program->setUniform4iv(location, count, v))
		{
			return error(GL_INVALID_OPERATION);
		}
	}
}

void GL_APIENTRY Uniform4i(GLint location, GLint x, GLint y, GLint z, GLint w)
{
	GLint xyzw[4] = {x, y, z, w};

	Uniform4iv(location, 1, (GLint*)&xyzw);
}

void GL_APIENTRY UniformMatrix2fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value)
{
	TRACE("(GLint location = %d, GLsizei count = %d, GLboolean transpose = %d, const GLfloat* value = %p)",
	      location, count, transpose, value);

	if(count < 0)
	{
		return error(GL_INVALID_VALUE);
	}

	auto context = es2::getContext();

	if(context)
	{
		es2::Program *program = context->getCurrentProgram();

		if(!program)
		{
			return error(GL_INVALID_OPERATION);
		}

		if(location == -1)
		{
			return;
		}

		if(!program->setUniformMatrix2fv(location, count, transpose, value))
		{
			return error(GL_INVALID_OPERATION);
		}
	}
}

void GL_APIENTRY UniformMatrix3fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value)
{
	TRACE("(GLint location = %d, GLsizei count = %d, GLboolean transpose = %d, const GLfloat* value = %p)",
	      location, count, transpose, value);

	if(count < 0)
	{
		return error(GL_INVALID_VALUE);
	}

	auto context = es2::getContext();

	if(context)
	{
		es2::Program *program = context->getCurrentProgram();

		if(!program)
		{
			return error(GL_INVALID_OPERATION);
		}

		if(location == -1)
		{
			return;
		}

		if(!program->setUniformMatrix3fv(location, count, transpose, value))
		{
			return error(GL_INVALID_OPERATION);
		}
	}
}

void GL_APIENTRY UniformMatrix4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value)
{
	TRACE("(GLint location = %d, GLsizei count = %d, GLboolean transpose = %d, const GLfloat* value = %p)",
	      location, count, transpose, value);

	if(count < 0)
	{
		return error(GL_INVALID_VALUE);
	}

	auto context = es2::getContext();

	if(context)
	{
		es2::Program *program = context->getCurrentProgram();

		if(!program)
		{
			return error(GL_INVALID_OPERATION);
		}

		if(location == -1)
		{
			return;
		}

		if(!program->setUniformMatrix4fv(location, count, transpose, value))
		{
			return error(GL_INVALID_OPERATION);
		}
	}
}

void GL_APIENTRY UseProgram(GLuint program)
{
	TRACE("(GLuint program = %d)", program);

	auto context = es2::getContext();

	if(context)
	{
		es2::TransformFeedback* transformFeedback = context->getTransformFeedback();
		if(transformFeedback && transformFeedback->isActive() && !transformFeedback->isPaused())
		{
			return error(GL_INVALID_OPERATION);
		}

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

void GL_APIENTRY ValidateProgram(GLuint program)
{
	TRACE("(GLuint program = %d)", program);

	auto context = es2::getContext();

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

		programObject->validate(context->getDevice());
	}
}

void GL_APIENTRY VertexAttrib1f(GLuint index, GLfloat x)
{
	TRACE("(GLuint index = %d, GLfloat x = %f)", index, x);

	if(index >= es2::MAX_VERTEX_ATTRIBS)
	{
		return error(GL_INVALID_VALUE);
	}

	auto context = es2::getContext();

	if(context)
	{
		GLfloat vals[4] = { x, 0, 0, 1 };
		context->setVertexAttrib(index, vals);
	}
}

void GL_APIENTRY VertexAttrib1fv(GLuint index, const GLfloat* values)
{
	TRACE("(GLuint index = %d, const GLfloat* values = %p)", index, values);

	if(index >= es2::MAX_VERTEX_ATTRIBS)
	{
		return error(GL_INVALID_VALUE);
	}

	auto context = es2::getContext();

	if(context)
	{
		GLfloat vals[4] = { values[0], 0, 0, 1 };
		context->setVertexAttrib(index, vals);
	}
}

void GL_APIENTRY VertexAttrib2f(GLuint index, GLfloat x, GLfloat y)
{
	TRACE("(GLuint index = %d, GLfloat x = %f, GLfloat y = %f)", index, x, y);

	if(index >= es2::MAX_VERTEX_ATTRIBS)
	{
		return error(GL_INVALID_VALUE);
	}

	auto context = es2::getContext();

	if(context)
	{
		GLfloat vals[4] = { x, y, 0, 1 };
		context->setVertexAttrib(index, vals);
	}
}

void GL_APIENTRY VertexAttrib2fv(GLuint index, const GLfloat* values)
{
	TRACE("(GLuint index = %d, const GLfloat* values = %p)", index, values);

	if(index >= es2::MAX_VERTEX_ATTRIBS)
	{
		return error(GL_INVALID_VALUE);
	}

	auto context = es2::getContext();

	if(context)
	{
		GLfloat vals[4] = { values[0], values[1], 0, 1 };
		context->setVertexAttrib(index, vals);
	}
}

void GL_APIENTRY VertexAttrib3f(GLuint index, GLfloat x, GLfloat y, GLfloat z)
{
	TRACE("(GLuint index = %d, GLfloat x = %f, GLfloat y = %f, GLfloat z = %f)", index, x, y, z);

	if(index >= es2::MAX_VERTEX_ATTRIBS)
	{
		return error(GL_INVALID_VALUE);
	}

	auto context = es2::getContext();

	if(context)
	{
		GLfloat vals[4] = { x, y, z, 1 };
		context->setVertexAttrib(index, vals);
	}
}

void GL_APIENTRY VertexAttrib3fv(GLuint index, const GLfloat* values)
{
	TRACE("(GLuint index = %d, const GLfloat* values = %p)", index, values);

	if(index >= es2::MAX_VERTEX_ATTRIBS)
	{
		return error(GL_INVALID_VALUE);
	}

	auto context = es2::getContext();

	if(context)
	{
		GLfloat vals[4] = { values[0], values[1], values[2], 1 };
		context->setVertexAttrib(index, vals);
	}
}

void GL_APIENTRY VertexAttrib4f(GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w)
{
	TRACE("(GLuint index = %d, GLfloat x = %f, GLfloat y = %f, GLfloat z = %f, GLfloat w = %f)", index, x, y, z, w);

	if(index >= es2::MAX_VERTEX_ATTRIBS)
	{
		return error(GL_INVALID_VALUE);
	}

	auto context = es2::getContext();

	if(context)
	{
		GLfloat vals[4] = { x, y, z, w };
		context->setVertexAttrib(index, vals);
	}
}

void GL_APIENTRY VertexAttrib4fv(GLuint index, const GLfloat* values)
{
	TRACE("(GLuint index = %d, const GLfloat* values = %p)", index, values);

	if(index >= es2::MAX_VERTEX_ATTRIBS)
	{
		return error(GL_INVALID_VALUE);
	}

	auto context = es2::getContext();

	if(context)
	{
		context->setVertexAttrib(index, values);
	}
}

void GL_APIENTRY VertexAttribPointer(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid* ptr)
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

	switch(type)
	{
	case GL_BYTE:
	case GL_UNSIGNED_BYTE:
	case GL_SHORT:
	case GL_UNSIGNED_SHORT:
	case GL_FIXED:
	case GL_FLOAT:
	case GL_HALF_FLOAT_OES:   // GL_OES_vertex_half_float
	case GL_HALF_FLOAT:
	case GL_INT:
	case GL_UNSIGNED_INT:
		break;
	case GL_INT_2_10_10_10_REV:
	case GL_UNSIGNED_INT_2_10_10_10_REV:
		if(size != 4)
		{
			return error(GL_INVALID_OPERATION);
		}
		break;
	default:
		return error(GL_INVALID_ENUM);
	}

	if(stride < 0)
	{
		return error(GL_INVALID_VALUE);
	}

	auto context = es2::getContext();

	if(context)
	{
		es2::VertexArray* vertexArray = context->getCurrentVertexArray();
		if((context->getArrayBufferName() == 0) && vertexArray && (vertexArray->name != 0) && ptr)
		{
			// GL_INVALID_OPERATION is generated if a non-zero vertex array object is bound, zero is bound
			// to the GL_ARRAY_BUFFER buffer object binding point and the pointer argument is not NULL.
			return error(GL_INVALID_OPERATION);
		}

		context->setVertexAttribState(index, context->getArrayBuffer(), size, type, (normalized != GL_FALSE), false, stride, ptr);
	}
}

void GL_APIENTRY Viewport(GLint x, GLint y, GLsizei width, GLsizei height)
{
	TRACE("(GLint x = %d, GLint y = %d, GLsizei width = %d, GLsizei height = %d)", x, y, width, height);

	if(width < 0 || height < 0)
	{
		return error(GL_INVALID_VALUE);
	}

	auto context = es2::getContext();

	if(context)
	{
		context->setViewportParams(x, y, width, height);
	}
}

static void GL_APIENTRY BlitFramebufferSW(GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter, bool allowPartialDepthStencilBlit)
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

	auto context = es2::getContext();

	if(context)
	{
		if(context->getReadFramebufferName() == context->getDrawFramebufferName())
		{
			ERR("Blits with the same source and destination framebuffer are not supported by this implementation.");
			return error(GL_INVALID_OPERATION);
		}

		context->blitFramebuffer(srcX0, srcY0, srcX1, srcY1, dstX0, dstY0, dstX1, dstY1, mask, false, allowPartialDepthStencilBlit);
	}
}

void GL_APIENTRY BlitFramebufferNV(GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter)
{
	BlitFramebufferSW(srcX0, srcY0, srcX1, srcY1, dstX0, dstY0, dstX1, dstY1, mask, filter, true);
}

void GL_APIENTRY BlitFramebufferANGLE(GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1,
                                      GLbitfield mask, GLenum filter)
{
	if(srcX1 - srcX0 != dstX1 - dstX0 || srcY1 - srcY0 != dstY1 - dstY0)
	{
		ERR("Scaling and flipping in BlitFramebufferANGLE not supported by this implementation");
		return error(GL_INVALID_OPERATION);
	}

	BlitFramebufferSW(srcX0, srcY0, srcX1, srcY1, dstX0, dstY0, dstX1, dstY1, mask, filter, false);
}

void GL_APIENTRY TexImage3DOES(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth,
                               GLint border, GLenum format, GLenum type, const GLvoid* data)
{
	TRACE("(GLenum target = 0x%X, GLint level = %d, GLenum internalformat = 0x%X, "
	      "GLsizei width = %d, GLsizei height = %d, GLsizei depth = %d, GLint border = %d, "
	      "GLenum format = 0x%X, GLenum type = 0x%x, const GLvoid* data = %p)",
	      target, level, internalformat, width, height, depth, border, format, type, data);

	switch(target)
	{
	case GL_TEXTURE_3D:
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

	if(internalformat != format)
	{
		return error(GL_INVALID_OPERATION);
	}

	GLenum validationError = ValidateTextureFormatType(format, type, internalformat, target);
	if(validationError != GL_NO_ERROR)
	{
		return error(validationError);
	}

	if((level < 0) || (level >= es2::IMPLEMENTATION_MAX_TEXTURE_LEVELS))
	{
		return error(GL_INVALID_VALUE);
	}

	const GLsizei maxSize3D = es2::IMPLEMENTATION_MAX_3D_TEXTURE_SIZE >> level;
	if((width < 0) || (height < 0) || (depth < 0) || (width > maxSize3D) || (height > maxSize3D) || (depth > maxSize3D))
	{
		return error(GL_INVALID_VALUE);
	}

	if(border != 0)
	{
		return error(GL_INVALID_VALUE);
	}

	auto context = es2::getContext();

	if(context)
	{
		es2::Texture3D *texture = context->getTexture3D();

		if(!texture)
		{
			return error(GL_INVALID_OPERATION);
		}

		GLenum validationError = context->getPixels(&data, type, context->getRequiredBufferSize(width, height, depth, format, type));
		if(validationError != GL_NO_ERROR)
		{
			return error(validationError);
		}

		GLint sizedInternalFormat = gl::GetSizedInternalFormat(internalformat, type);
		texture->setImage(level, width, height, depth, sizedInternalFormat, format, type, context->getUnpackParameters(), data);
	}
}

void GL_APIENTRY TexSubImage3DOES(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const void *data)
{
	TRACE("(GLenum target = 0x%X, GLint level = %d, GLint xoffset = %d, GLint yoffset = %d, "
	      "GLint zoffset = %d, GLsizei width = %d, GLsizei height = %d, GLsizei depth = %d, "
	      "GLenum format = 0x%X, GLenum type = 0x%x, const GLvoid* data = %p)",
	      target, level, xoffset, yoffset, zoffset, width, height, depth, format, type, data);

	switch(target)
	{
	case GL_TEXTURE_3D:
		break;
	default:
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

	auto context = es2::getContext();

	if(context)
	{
		es2::Texture3D *texture = context->getTexture3D();

		GLenum validationError = ValidateSubImageParams(false, false, target, level, xoffset, yoffset, zoffset, width, height, depth, format, type, texture);
		if(validationError != GL_NO_ERROR)
		{
			return error(validationError);
		}

		validationError = context->getPixels(&data, type, context->getRequiredBufferSize(width, height, depth, format, type));
		if(validationError != GL_NO_ERROR)
		{
			return error(validationError);
		}

		texture->subImage(level, xoffset, yoffset, zoffset, width, height, depth, format, type, context->getUnpackParameters(), data);
	}
}

void GL_APIENTRY CopyTexSubImage3DOES(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLint x, GLint y, GLsizei width, GLsizei height)
{
	TRACE("(GLenum target = 0x%X, GLint level = %d, GLint xoffset = %d, GLint yoffset = %d, "
	      "GLint zoffset = %d, GLint x = %d, GLint y = %d, GLsizei width = %d, GLsizei height = %d)",
	      target, level, xoffset, yoffset, zoffset, x, y, width, height);

	switch(target)
	{
	case GL_TEXTURE_3D:
		break;
	default:
		return error(GL_INVALID_ENUM);
	}

	if((level < 0) || (level >= es2::IMPLEMENTATION_MAX_TEXTURE_LEVELS))
	{
		return error(GL_INVALID_VALUE);
	}

	auto context = es2::getContext();

	if(context)
	{
		es2::Framebuffer *framebuffer = context->getReadFramebuffer();

		if(!framebuffer || (framebuffer->completeness() != GL_FRAMEBUFFER_COMPLETE))
		{
			return error(GL_INVALID_FRAMEBUFFER_OPERATION);
		}

		es2::Renderbuffer *source = framebuffer->getReadColorbuffer();

		if(context->getReadFramebufferName() != 0 && (!source || source->getSamples() > 1))
		{
			return error(GL_INVALID_OPERATION);
		}

		es2::Texture3D *texture = context->getTexture3D();

		GLenum validationError = ValidateSubImageParams(false, true, target, level, xoffset, yoffset, zoffset, width, height, 1, GL_NONE, GL_NONE, texture);
		if(validationError != GL_NO_ERROR)
		{
			return error(validationError);
		}

		texture->copySubImage(target, level, xoffset, yoffset, zoffset, x, y, width, height, source);
	}
}

void GL_APIENTRY CompressedTexImage3DOES(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLsizei imageSize, const void *data)
{
	TRACE("(GLenum target = 0x%X, GLint level = %d, GLenum internalformat = 0x%X, GLsizei width = %d, "
	      "GLsizei height = %d, GLsizei depth = %d, GLint border = %d, GLsizei imageSize = %d, const GLvoid* data = %p)",
	      target, level, internalformat, width, height, depth, border, imageSize, data);

	switch(target)
	{
	case GL_TEXTURE_3D:
		break;
	default:
		return error(GL_INVALID_ENUM);
	}

	if((level < 0) || (level >= es2::IMPLEMENTATION_MAX_TEXTURE_LEVELS))
	{
		return error(GL_INVALID_VALUE);
	}

	const GLsizei maxSize3D = es2::IMPLEMENTATION_MAX_3D_TEXTURE_SIZE >> level;
	if((width < 0) || (height < 0) || (depth < 0) || (width > maxSize3D) || (height > maxSize3D) || (depth > maxSize3D) ||(border != 0) || (imageSize < 0))
	{
		return error(GL_INVALID_VALUE);
	}

	if(!IsCompressed(internalformat))
	{
		return error(GL_INVALID_ENUM);
	}

	if(imageSize != gl::ComputeCompressedSize(width, height, internalformat) * depth)
	{
		return error(GL_INVALID_VALUE);
	}

	auto context = es2::getContext();

	if(context)
	{
		es2::Texture3D *texture = context->getTexture3D();

		if(!texture)
		{
			return error(GL_INVALID_OPERATION);
		}

		GLenum validationError = context->getPixels(&data, GL_UNSIGNED_BYTE, imageSize);

		if(validationError != GL_NO_ERROR)
		{
			return error(validationError);
		}

		texture->setCompressedImage(level, internalformat, width, height, depth, imageSize, data);
	}
}

void GL_APIENTRY CompressedTexSubImage3DOES(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, const void *data)
{
	TRACE("(GLenum target = 0x%X, GLint level = %d, GLint xoffset = %d, GLint yoffset = %d, "
	      "GLint zoffset = %d, GLsizei width = %d, GLsizei height = %d, GLsizei depth = %d, "
	      "GLenum format = 0x%X, GLsizei imageSize = %d, const void *data = %p)",
	      target, level, xoffset, yoffset, zoffset, width, height, depth, format, imageSize, data);

	switch(target)
	{
	case GL_TEXTURE_3D:
		break;
	default:
		return error(GL_INVALID_ENUM);
	}

	if(level < 0 || level >= es2::IMPLEMENTATION_MAX_TEXTURE_LEVELS)
	{
		return error(GL_INVALID_VALUE);
	}

	if(xoffset < 0 || yoffset < 0 || zoffset < 0 || !validImageSize(level, width, height) || depth < 0 || imageSize < 0)
	{
		return error(GL_INVALID_VALUE);
	}

	if(!IsCompressed(format))
	{
		return error(GL_INVALID_ENUM);
	}

	if(imageSize != gl::ComputeCompressedSize(width, height, format) * depth)
	{
		return error(GL_INVALID_VALUE);
	}

	auto context = es2::getContext();

	if(context)
	{
		es2::Texture3D *texture = context->getTexture3D();

		if(!texture)
		{
			return error(GL_INVALID_OPERATION);
		}

		GLenum validationError = context->getPixels(&data, GL_UNSIGNED_BYTE, imageSize);
		if(validationError != GL_NO_ERROR)
		{
			return error(validationError);
		}

		texture->subImageCompressed(level, xoffset, yoffset, zoffset, width, height, depth, format, imageSize, data);
	}
}

void GL_APIENTRY FramebufferTexture3DOES(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level, GLint zoffset)
{
	TRACE("(GLenum target = 0x%X, GLenum attachment = 0x%X, GLenum textarget = 0x%X, "
	      "GLuint texture = %d, GLint level = %d, GLint zoffset = %d)", target, attachment, textarget, texture, level, zoffset);

	if(target != GL_FRAMEBUFFER && target != GL_DRAW_FRAMEBUFFER && target != GL_READ_FRAMEBUFFER)
	{
		return error(GL_INVALID_ENUM);
	}

	auto context = es2::getContext();

	if(context)
	{
		if(texture == 0)
		{
			textarget = GL_NONE;
		}
		else
		{
			es2::Texture *tex = context->getTexture(texture);

			if(!tex)
			{
				return error(GL_INVALID_OPERATION);
			}

			switch(textarget)
			{
			case GL_TEXTURE_3D:
				if(tex->getTarget() != GL_TEXTURE_3D)
				{
					return error(GL_INVALID_OPERATION);
				}
				break;
			default:
				return error(GL_INVALID_ENUM);
			}

			if((level < 0) || (level >= es2::IMPLEMENTATION_MAX_TEXTURE_LEVELS))
			{
				return error(GL_INVALID_VALUE);
			}

			if(tex->isCompressed(textarget, level))
			{
				return error(GL_INVALID_OPERATION);
			}
		}

		es2::Framebuffer *framebuffer = nullptr;
		GLuint framebufferName = 0;
		if(target == GL_READ_FRAMEBUFFER)
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
		case GL_DEPTH_ATTACHMENT:   framebuffer->setDepthbuffer(textarget, texture, level);   break;
		case GL_STENCIL_ATTACHMENT: framebuffer->setStencilbuffer(textarget, texture, level); break;
		default:
			if(attachment < GL_COLOR_ATTACHMENT0 || attachment > GL_COLOR_ATTACHMENT31)
			{
				return error(GL_INVALID_ENUM);
			}

			if((attachment - GL_COLOR_ATTACHMENT0) >= MAX_COLOR_ATTACHMENTS)
			{
				return error(GL_INVALID_OPERATION);
			}

			framebuffer->setColorbuffer(textarget, texture, attachment - GL_COLOR_ATTACHMENT0, level);
			break;
		}
	}
}

void GL_APIENTRY EGLImageTargetTexture2DOES(GLenum target, GLeglImageOES image)
{
	if(egl::getClientVersion() == 1)
	{
		return libGLES_CM->glEGLImageTargetTexture2DOES(target, image);
	}

	TRACE("(GLenum target = 0x%X, GLeglImageOES image = %p)", target, image);

	switch(target)
	{
	case GL_TEXTURE_2D:
	case GL_TEXTURE_RECTANGLE_ARB:
	case GL_TEXTURE_EXTERNAL_OES:
		break;
	default:
		return error(GL_INVALID_ENUM);
	}

	auto context = es2::getContext();

	if(context)
	{
		es2::Texture2D *texture = context->getTexture2D(target);

		if(!texture)
		{
			return error(GL_INVALID_OPERATION);
		}

		egl::Image *eglImage = context->getSharedImage(image);

		if(!eglImage)
		{
			return error(GL_INVALID_OPERATION);
		}

		texture->setSharedImage(eglImage);
	}
}

void GL_APIENTRY EGLImageTargetRenderbufferStorageOES(GLenum target, GLeglImageOES image)
{
	TRACE("(GLenum target = 0x%X, GLeglImageOES image = %p)", target, image);

	UNIMPLEMENTED();
}

GLboolean GL_APIENTRY IsRenderbufferOES(GLuint renderbuffer)
{
	return IsRenderbuffer(renderbuffer);
}

void GL_APIENTRY BindRenderbufferOES(GLenum target, GLuint renderbuffer)
{
	BindRenderbuffer(target, renderbuffer);
}

void GL_APIENTRY DeleteRenderbuffersOES(GLsizei n, const GLuint* renderbuffers)
{
	DeleteRenderbuffers(n, renderbuffers);
}

void GL_APIENTRY GenRenderbuffersOES(GLsizei n, GLuint* renderbuffers)
{
	GenRenderbuffers(n, renderbuffers);
}

void GL_APIENTRY RenderbufferStorageOES(GLenum target, GLenum internalformat, GLsizei width, GLsizei height)
{
	RenderbufferStorage(target, internalformat, width, height);
}

void GL_APIENTRY GetRenderbufferParameterivOES(GLenum target, GLenum pname, GLint* params)
{
	GetRenderbufferParameteriv(target, pname, params);
}

GLboolean GL_APIENTRY IsFramebufferOES(GLuint framebuffer)
{
	return IsFramebuffer(framebuffer);
}

void GL_APIENTRY BindFramebufferOES(GLenum target, GLuint framebuffer)
{
	BindFramebuffer(target, framebuffer);
}

void GL_APIENTRY DeleteFramebuffersOES(GLsizei n, const GLuint* framebuffers)
{
	DeleteFramebuffers(n, framebuffers);
}

void GL_APIENTRY GenFramebuffersOES(GLsizei n, GLuint* framebuffers)
{
	GenFramebuffers(n, framebuffers);
}

GLenum GL_APIENTRY CheckFramebufferStatusOES(GLenum target)
{
	return CheckFramebufferStatus(target);
}

void GL_APIENTRY FramebufferRenderbufferOES(GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer)
{
	FramebufferRenderbuffer(target, attachment, renderbuffertarget, renderbuffer);
}

void GL_APIENTRY FramebufferTexture2DOES(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level)
{
	FramebufferTexture2D(target, attachment, textarget, texture, level);
}

void GL_APIENTRY GetFramebufferAttachmentParameterivOES(GLenum target, GLenum attachment, GLenum pname, GLint* params)
{
	GetFramebufferAttachmentParameteriv(target, attachment, pname, params);
}

void GL_APIENTRY GenerateMipmapOES(GLenum target)
{
	GenerateMipmap(target);
}

void GL_APIENTRY DrawBuffersEXT(GLsizei n, const GLenum *bufs)
{
	TRACE("(GLsizei n = %d, const GLenum *bufs = %p)", n, bufs);

	if(n < 0 || n > MAX_DRAW_BUFFERS)
	{
		return error(GL_INVALID_VALUE);
	}

	auto context = es2::getContext();

	if(context)
	{
		GLuint drawFramebufferName = context->getDrawFramebufferName();

		if((drawFramebufferName == 0) && (n != 1))
		{
			return error(GL_INVALID_OPERATION);
		}

		for(unsigned int i = 0; i < (unsigned)n; i++)
		{
			switch(bufs[i])
			{
			case GL_BACK:
				if(drawFramebufferName != 0)
				{
					return error(GL_INVALID_OPERATION);
				}
				break;
			case GL_NONE:
				break;
			case GL_COLOR_ATTACHMENT0_EXT:
			case GL_COLOR_ATTACHMENT1_EXT:
			case GL_COLOR_ATTACHMENT2_EXT:
			case GL_COLOR_ATTACHMENT3_EXT:
			case GL_COLOR_ATTACHMENT4_EXT:
			case GL_COLOR_ATTACHMENT5_EXT:
			case GL_COLOR_ATTACHMENT6_EXT:
			case GL_COLOR_ATTACHMENT7_EXT:
			case GL_COLOR_ATTACHMENT8_EXT:
			case GL_COLOR_ATTACHMENT9_EXT:
			case GL_COLOR_ATTACHMENT10_EXT:
			case GL_COLOR_ATTACHMENT11_EXT:
			case GL_COLOR_ATTACHMENT12_EXT:
			case GL_COLOR_ATTACHMENT13_EXT:
			case GL_COLOR_ATTACHMENT14_EXT:
			case GL_COLOR_ATTACHMENT15_EXT:
				{
					GLuint index = (bufs[i] - GL_COLOR_ATTACHMENT0_EXT);

					if(index >= MAX_COLOR_ATTACHMENTS)
					{
						return error(GL_INVALID_OPERATION);
					}

					if(index != i)
					{
						return error(GL_INVALID_OPERATION);
					}

					if(drawFramebufferName == 0)
					{
						return error(GL_INVALID_OPERATION);
					}
				}
				break;
			default:
				return error(GL_INVALID_ENUM);
			}
		}

		context->setFramebufferDrawBuffers(n, bufs);
	}
}

}

#include "entry_points.h"

extern "C" NO_SANITIZE_FUNCTION __eglMustCastToProperFunctionPointerType es2GetProcAddress(const char *procname)
{
	struct Function
	{
		const char *name;
		__eglMustCastToProperFunctionPointerType address;
	};

	static const Function glFunctions[] =
	{
		#define FUNCTION(name) {"gl" #name, (__eglMustCastToProperFunctionPointerType)gl::name}

		FUNCTION(ActiveTexture),
		FUNCTION(AttachShader),
		FUNCTION(BeginQuery),
		FUNCTION(BeginQueryEXT),
		FUNCTION(BeginTransformFeedback),
		FUNCTION(BindAttribLocation),
		FUNCTION(BindBuffer),
		FUNCTION(BindBufferBase),
		FUNCTION(BindBufferRange),
		FUNCTION(BindFramebuffer),
		FUNCTION(BindFramebufferOES),
		FUNCTION(BindRenderbuffer),
		FUNCTION(BindRenderbufferOES),
		FUNCTION(BindSampler),
		FUNCTION(BindTexture),
		FUNCTION(BindTransformFeedback),
		FUNCTION(BindVertexArray),
		FUNCTION(BindVertexArrayOES),
		FUNCTION(BlendColor),
		FUNCTION(BlendEquation),
		FUNCTION(BlendEquationSeparate),
		FUNCTION(BlendFunc),
		FUNCTION(BlendFuncSeparate),
		FUNCTION(BlitFramebuffer),
		FUNCTION(BlitFramebufferANGLE),
		FUNCTION(BufferData),
		FUNCTION(BufferSubData),
		FUNCTION(CheckFramebufferStatus),
		FUNCTION(CheckFramebufferStatusOES),
		FUNCTION(Clear),
		FUNCTION(ClearBufferfi),
		FUNCTION(ClearBufferfv),
		FUNCTION(ClearBufferiv),
		FUNCTION(ClearBufferuiv),
		FUNCTION(ClearColor),
		FUNCTION(ClearDepthf),
		FUNCTION(ClearStencil),
		FUNCTION(ClientWaitSync),
		FUNCTION(ColorMask),
		FUNCTION(CompileShader),
		FUNCTION(CompressedTexImage2D),
		FUNCTION(CompressedTexImage3D),
		FUNCTION(CompressedTexSubImage2D),
		FUNCTION(CompressedTexSubImage3D),
		FUNCTION(CopyBufferSubData),
		FUNCTION(CopyTexImage2D),
		FUNCTION(CopyTexSubImage2D),
		FUNCTION(CopyTexSubImage3D),
		FUNCTION(CreateProgram),
		FUNCTION(CreateShader),
		FUNCTION(CullFace),
		FUNCTION(DeleteBuffers),
		FUNCTION(DeleteFencesNV),
		FUNCTION(DeleteFramebuffers),
		FUNCTION(DeleteFramebuffersOES),
		FUNCTION(DeleteProgram),
		FUNCTION(DeleteQueries),
		FUNCTION(DeleteQueriesEXT),
		FUNCTION(DeleteRenderbuffers),
		FUNCTION(DeleteRenderbuffersOES),
		FUNCTION(DeleteSamplers),
		FUNCTION(DeleteShader),
		FUNCTION(DeleteSync),
		FUNCTION(DeleteTextures),
		FUNCTION(DeleteTransformFeedbacks),
		FUNCTION(DeleteVertexArrays),
		FUNCTION(DeleteVertexArraysOES),
		FUNCTION(DepthFunc),
		FUNCTION(DepthMask),
		FUNCTION(DepthRangef),
		FUNCTION(DetachShader),
		FUNCTION(Disable),
		FUNCTION(DisableVertexAttribArray),
		FUNCTION(DrawArrays),
		FUNCTION(DrawArraysInstanced),
		FUNCTION(DrawBuffers),
		FUNCTION(DrawBuffersEXT),
		FUNCTION(DrawElements),
		FUNCTION(DrawElementsInstanced),
		FUNCTION(DrawRangeElements),
		FUNCTION(EGLImageTargetRenderbufferStorageOES),
		FUNCTION(EGLImageTargetTexture2DOES),
		FUNCTION(Enable),
		FUNCTION(EnableVertexAttribArray),
		FUNCTION(EndQuery),
		FUNCTION(EndQueryEXT),
		FUNCTION(EndTransformFeedback),
		FUNCTION(FenceSync),
		FUNCTION(Finish),
		FUNCTION(FinishFenceNV),
		FUNCTION(Flush),
		FUNCTION(FlushMappedBufferRange),
		FUNCTION(FramebufferRenderbuffer),
		FUNCTION(FramebufferRenderbufferOES),
		FUNCTION(FramebufferTexture2D),
		FUNCTION(FramebufferTexture2DOES),
		FUNCTION(FramebufferTextureLayer),
		FUNCTION(FrontFace),
		FUNCTION(GenBuffers),
		FUNCTION(GenFencesNV),
		FUNCTION(GenFramebuffers),
		FUNCTION(GenFramebuffersOES),
		FUNCTION(GenQueries),
		FUNCTION(GenQueriesEXT),
		FUNCTION(GenRenderbuffers),
		FUNCTION(GenRenderbuffersOES),
		FUNCTION(GenSamplers),
		FUNCTION(GenTextures),
		FUNCTION(GenTransformFeedbacks),
		FUNCTION(GenVertexArrays),
		FUNCTION(GenVertexArraysOES),
		FUNCTION(GenerateMipmap),
		FUNCTION(GenerateMipmapOES),
		FUNCTION(GetActiveAttrib),
		FUNCTION(GetActiveUniform),
		FUNCTION(GetActiveUniformBlockName),
		FUNCTION(GetActiveUniformBlockiv),
		FUNCTION(GetActiveUniformsiv),
		FUNCTION(GetAttachedShaders),
		FUNCTION(GetAttribLocation),
		FUNCTION(GetBooleanv),
		FUNCTION(GetBufferParameteri64v),
		FUNCTION(GetBufferParameteriv),
		FUNCTION(GetBufferPointerv),
		FUNCTION(GetError),
		FUNCTION(GetFenceivNV),
		FUNCTION(GetFloatv),
		FUNCTION(GetFragDataLocation),
		FUNCTION(GetFramebufferAttachmentParameteriv),
		FUNCTION(GetFramebufferAttachmentParameterivOES),
		FUNCTION(GetGraphicsResetStatusEXT),
		FUNCTION(GetInteger64i_v),
		FUNCTION(GetInteger64v),
		FUNCTION(GetIntegeri_v),
		FUNCTION(GetIntegerv),
		FUNCTION(GetInternalformativ),
		FUNCTION(GetProgramBinary),
		FUNCTION(GetProgramInfoLog),
		FUNCTION(GetProgramiv),
		FUNCTION(GetQueryObjectuiv),
		FUNCTION(GetQueryObjectuivEXT),
		FUNCTION(GetQueryiv),
		FUNCTION(GetQueryivEXT),
		FUNCTION(GetRenderbufferParameteriv),
		FUNCTION(GetRenderbufferParameterivOES),
		FUNCTION(GetSamplerParameterfv),
		FUNCTION(GetSamplerParameteriv),
		FUNCTION(GetShaderInfoLog),
		FUNCTION(GetShaderPrecisionFormat),
		FUNCTION(GetShaderSource),
		FUNCTION(GetShaderiv),
		FUNCTION(GetString),
		FUNCTION(GetStringi),
		FUNCTION(GetSynciv),
		FUNCTION(GetTexParameterfv),
		FUNCTION(GetTexParameteriv),
		FUNCTION(GetTransformFeedbackVarying),
		FUNCTION(GetUniformBlockIndex),
		FUNCTION(GetUniformIndices),
		FUNCTION(GetUniformLocation),
		FUNCTION(GetUniformfv),
		FUNCTION(GetUniformiv),
		FUNCTION(GetUniformuiv),
		FUNCTION(GetVertexAttribIiv),
		FUNCTION(GetVertexAttribIuiv),
		FUNCTION(GetVertexAttribPointerv),
		FUNCTION(GetVertexAttribfv),
		FUNCTION(GetVertexAttribiv),
		FUNCTION(GetnUniformfvEXT),
		FUNCTION(GetnUniformivEXT),
		FUNCTION(Hint),
		FUNCTION(InvalidateFramebuffer),
		FUNCTION(InvalidateSubFramebuffer),
		FUNCTION(IsBuffer),
		FUNCTION(IsEnabled),
		FUNCTION(IsFenceNV),
		FUNCTION(IsFramebuffer),
		FUNCTION(IsFramebufferOES),
		FUNCTION(IsProgram),
		FUNCTION(IsQuery),
		FUNCTION(IsQueryEXT),
		FUNCTION(IsRenderbuffer),
		FUNCTION(IsRenderbufferOES),
		FUNCTION(IsSampler),
		FUNCTION(IsShader),
		FUNCTION(IsSync),
		FUNCTION(IsTexture),
		FUNCTION(IsTransformFeedback),
		FUNCTION(IsVertexArray),
		FUNCTION(IsVertexArrayOES),
		FUNCTION(LineWidth),
		FUNCTION(LinkProgram),
		FUNCTION(MapBufferRange),
		FUNCTION(PauseTransformFeedback),
		FUNCTION(PixelStorei),
		FUNCTION(PolygonOffset),
		FUNCTION(ProgramBinary),
		FUNCTION(ProgramParameteri),
		FUNCTION(ReadBuffer),
		FUNCTION(ReadPixels),
		FUNCTION(ReadnPixelsEXT),
		FUNCTION(ReleaseShaderCompiler),
		FUNCTION(RenderbufferStorage),
		FUNCTION(RenderbufferStorageMultisample),
		FUNCTION(RenderbufferStorageMultisampleANGLE),
		FUNCTION(RenderbufferStorageOES),
		FUNCTION(ResumeTransformFeedback),
		FUNCTION(SampleCoverage),
		FUNCTION(SamplerParameterf),
		FUNCTION(SamplerParameterfv),
		FUNCTION(SamplerParameteri),
		FUNCTION(SamplerParameteriv),
		FUNCTION(Scissor),
		FUNCTION(SetFenceNV),
		FUNCTION(ShaderBinary),
		FUNCTION(ShaderSource),
		FUNCTION(StencilFunc),
		FUNCTION(StencilFuncSeparate),
		FUNCTION(StencilMask),
		FUNCTION(StencilMaskSeparate),
		FUNCTION(StencilOp),
		FUNCTION(StencilOpSeparate),
		FUNCTION(TestFenceNV),
		FUNCTION(TexImage2D),
		FUNCTION(TexImage3D),
		FUNCTION(TexImage3DOES),
		FUNCTION(TexParameterf),
		FUNCTION(TexParameterfv),
		FUNCTION(TexParameteri),
		FUNCTION(TexParameteriv),
		FUNCTION(TexStorage2D),
		FUNCTION(TexStorage3D),
		FUNCTION(TexSubImage2D),
		FUNCTION(TexSubImage3D),
		FUNCTION(TransformFeedbackVaryings),
		FUNCTION(Uniform1f),
		FUNCTION(Uniform1fv),
		FUNCTION(Uniform1i),
		FUNCTION(Uniform1iv),
		FUNCTION(Uniform1ui),
		FUNCTION(Uniform1uiv),
		FUNCTION(Uniform2f),
		FUNCTION(Uniform2fv),
		FUNCTION(Uniform2i),
		FUNCTION(Uniform2iv),
		FUNCTION(Uniform2ui),
		FUNCTION(Uniform2uiv),
		FUNCTION(Uniform3f),
		FUNCTION(Uniform3fv),
		FUNCTION(Uniform3i),
		FUNCTION(Uniform3iv),
		FUNCTION(Uniform3ui),
		FUNCTION(Uniform3uiv),
		FUNCTION(Uniform4f),
		FUNCTION(Uniform4fv),
		FUNCTION(Uniform4i),
		FUNCTION(Uniform4iv),
		FUNCTION(Uniform4ui),
		FUNCTION(Uniform4uiv),
		FUNCTION(UniformBlockBinding),
		FUNCTION(UniformMatrix2fv),
		FUNCTION(UniformMatrix2x3fv),
		FUNCTION(UniformMatrix2x4fv),
		FUNCTION(UniformMatrix3fv),
		FUNCTION(UniformMatrix3x2fv),
		FUNCTION(UniformMatrix3x4fv),
		FUNCTION(UniformMatrix4fv),
		FUNCTION(UniformMatrix4x2fv),
		FUNCTION(UniformMatrix4x3fv),
		FUNCTION(UnmapBuffer),
		FUNCTION(UseProgram),
		FUNCTION(ValidateProgram),
		FUNCTION(VertexAttrib1f),
		FUNCTION(VertexAttrib1fv),
		FUNCTION(VertexAttrib2f),
		FUNCTION(VertexAttrib2fv),
		FUNCTION(VertexAttrib3f),
		FUNCTION(VertexAttrib3fv),
		FUNCTION(VertexAttrib4f),
		FUNCTION(VertexAttrib4fv),
		FUNCTION(VertexAttribDivisor),
		FUNCTION(VertexAttribDivisorANGLE),
		FUNCTION(VertexAttribDivisorEXT),
		FUNCTION(VertexAttribI4i),
		FUNCTION(VertexAttribI4iv),
		FUNCTION(VertexAttribI4ui),
		FUNCTION(VertexAttribI4uiv),
		FUNCTION(VertexAttribIPointer),
		FUNCTION(VertexAttribPointer),
		FUNCTION(Viewport),
		FUNCTION(WaitSync),

		#undef FUNCTION
	};

	static const size_t numFunctions = sizeof glFunctions / sizeof(Function);
	static const Function *const glFunctionsEnd = glFunctions + numFunctions;

	// The array must be kept sorted with respect to strcmp(), so that binary search works correctly.
	// The Unix command "LC_COLLATE=C sort" will generate the correct order.
	#ifndef NDEBUG
		for(size_t i = 0; i < numFunctions - 1; i++)
		{
			ASSERT(strcmp(glFunctions[i].name, glFunctions[i + 1].name) < 0);
		}
	#endif

	if(procname && strncmp("gl", procname, 2) == 0)
	{
		struct CompareFunctor
		{
			bool operator()(const Function &a, const Function &b) const
			{
				return strcmp(a.name, b.name) < 0;
			}
		};

		Function needle;
		needle.name = procname;
		const Function *result = std::lower_bound(glFunctions, glFunctionsEnd, needle, CompareFunctor());

		if(result != glFunctionsEnd && strcmp(procname, result->name) == 0)
		{
			return (__eglMustCastToProperFunctionPointerType)result->address;
		}
	}

	return nullptr;
}
