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

	void setMinFilter(GLenum minFilter) { mMinFilter = minFilter; }
	void setMagFilter(GLenum magFilter) { mMagFilter = magFilter; }
	void setWrapS(GLenum wrapS) { mWrapModeS = wrapS; }
	void setWrapT(GLenum wrapT) { mWrapModeT = wrapT; }
	void setWrapR(GLenum wrapR) { mWrapModeR = wrapR; }
	void setMinLod(GLfloat minLod) { mMinLod = minLod; }
	void setMaxLod(GLfloat maxLod) { mMaxLod = maxLod; }
	void setComparisonMode(GLenum comparisonMode) { mCompareMode = comparisonMode; }
	void setComparisonFunc(GLenum comparisonFunc) { mCompareFunc = comparisonFunc; }

	GLenum getMinFilter() const { return mMinFilter; }
	GLenum getMagFilter() const { return mMagFilter; }
	GLenum getWrapS() const { return mWrapModeS; }
	GLenum getWrapT() const { return mWrapModeT; }
	GLenum getWrapR() const { return mWrapModeR; }
	GLfloat getMinLod() const { return mMinLod; }
	GLfloat getMaxLod() const { return mMaxLod; }
	GLenum getComparisonMode() const { return mCompareMode; }
	GLenum getComparisonFunc() const { return mCompareFunc; }

private:
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
