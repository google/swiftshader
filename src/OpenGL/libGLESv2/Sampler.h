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

// Sampler.h: Defines the es2::Sampler class

#ifndef LIBGLESV2_SAMPLER_H_
#define LIBGLESV2_SAMPLER_H_

#include "common/Object.hpp"
#include "Renderer/Renderer.hpp"

#include <GLES2/gl2.h>

namespace es2
{

class Sampler : public gl::NamedObject
{
public:
	Sampler(GLuint name) : NamedObject(name)
	{
		mMinFilter = GL_NEAREST_MIPMAP_LINEAR;
		mMagFilter = GL_LINEAR;

		mWrapModeS = GL_REPEAT;
		mWrapModeT = GL_REPEAT;
		mWrapModeR = GL_REPEAT;

		mMinLod = -1000.0f;
		mMaxLod = 1000.0f;
		mCompareMode = GL_NONE;
		mCompareFunc = GL_LEQUAL;
	}

	GLenum mMinFilter;
	GLenum mMagFilter;

	GLenum mWrapModeS;
	GLenum mWrapModeT;
	GLenum mWrapModeR;

	GLfloat mMinLod;
	GLfloat mMaxLod;
	GLenum mCompareMode;
	GLenum mCompareFunc;
};

}

#endif // LIBGLESV2_SAMPLER_H_
