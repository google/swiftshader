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

// utilities.h: Conversion functions and other utility routines.

#ifndef LIBGLESV2_UTILITIES_H
#define LIBGLESV2_UTILITIES_H

#include "Device.hpp"
#include "common/Image.hpp"
#include "Texture.h"

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#include <string>

namespace es2
{
	struct Color;

	unsigned int UniformComponentCount(GLenum type);
	GLenum UniformComponentType(GLenum type);
	size_t UniformTypeSize(GLenum type);
	bool IsSamplerUniform(GLenum type);
	int VariableRowCount(GLenum type);
	int VariableColumnCount(GLenum type);
	int VariableRegisterCount(GLenum type);
	int VariableRegisterSize(GLenum type);

	int AllocateFirstFreeBits(unsigned int *bits, unsigned int allocationSize, unsigned int bitsSize);

	GLint floatToInt(GLfloat value);

	bool IsCompressed(GLenum format, GLint clientVersion);
	GLenum GetSizedInternalFormat(GLenum internalFormat, GLenum type);
	GLenum ValidateCompressedFormat(GLenum format, GLint clientVersion, bool expectCompressedFormats);
	GLenum ValidateSubImageParams(bool compressed, GLsizei width, GLsizei height, GLint xoffset, GLint yoffset, GLenum target, GLint level, GLenum sizedInternalFormat, Texture *texture);
	GLenum ValidateSubImageParams(bool compressed, GLsizei width, GLsizei height, GLsizei depth, GLint xoffset, GLint yoffset, GLint zoffset, GLenum target, GLint level, GLenum sizedInternalFormat, Texture *texture);
	bool ValidReadPixelsFormatType(GLenum internalFormat, GLenum internalType, GLenum format, GLenum type, GLint clientVersion);
	bool IsDepthTexture(GLenum format);
	bool IsStencilTexture(GLenum format);
	bool IsCubemapTextureTarget(GLenum target);
	int CubeFaceIndex(GLenum cubeTarget);
	bool IsTextureTarget(GLenum target);
	bool ValidateTextureFormatType(GLenum format, GLenum type, GLint internalformat, GLint clientVersion);

	bool IsColorRenderable(GLenum internalformat, GLint clientVersion);
	bool IsDepthRenderable(GLenum internalformat);
	bool IsStencilRenderable(GLenum internalformat);

	// Parse the base uniform name and array index.  Returns the base name of the uniform. outSubscript is
	// set to GL_INVALID_INDEX if the provided name is not an array or the array index is invalid.
	std::string ParseUniformName(const std::string &name, size_t *outSubscript);
}

namespace es2sw
{
	sw::DepthCompareMode ConvertDepthComparison(GLenum comparison);
	sw::StencilCompareMode ConvertStencilComparison(GLenum comparison);
	sw::Color<float> ConvertColor(es2::Color color);
	sw::BlendFactor ConvertBlendFunc(GLenum blend);
	sw::BlendOperation ConvertBlendOp(GLenum blendOp);
	sw::LogicalOperation ConvertLogicalOperation(GLenum logicalOperation);
	sw::StencilOperation ConvertStencilOp(GLenum stencilOp);
	sw::AddressingMode ConvertTextureWrap(GLenum wrap);
	sw::SwizzleType ConvertSwizzleType(GLenum swizzleType);
	sw::CullMode ConvertCullMode(GLenum cullFace, GLenum frontFace);
	unsigned int ConvertColorMask(bool red, bool green, bool blue, bool alpha);
	sw::MipmapType ConvertMipMapFilter(GLenum minFilter);
	sw::FilterType ConvertTextureFilter(GLenum minFilter, GLenum magFilter, float maxAnisotropy);
	bool ConvertPrimitiveType(GLenum primitiveType, GLsizei elementCount,  GLenum elementType, sw::DrawType &swPrimitiveType, int &primitiveCount, int &verticesPerPrimitive);
	sw::Format ConvertRenderbufferFormat(GLenum format);
}

namespace sw2es
{
	GLuint GetAlphaSize(sw::Format colorFormat);
	GLuint GetRedSize(sw::Format colorFormat);
	GLuint GetGreenSize(sw::Format colorFormat);
	GLuint GetBlueSize(sw::Format colorFormat);
	GLuint GetDepthSize(sw::Format depthFormat);
	GLuint GetStencilSize(sw::Format stencilFormat);
	GLenum GetComponentType(sw::Format format, GLenum attachment);

	GLenum ConvertBackBufferFormat(sw::Format format);
	GLenum ConvertDepthStencilFormat(sw::Format format);
}

#endif  // LIBGLESV2_UTILITIES_H
