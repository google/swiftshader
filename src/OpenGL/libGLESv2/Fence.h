// SwiftShader Software Renderer
//
// Copyright(c) 2005-2012 TransGaming Inc.
//
// All rights reserved. No part of this software may be copied, distributed, transmitted,
// transcribed, stored in a retrieval system, translated into any human or computer
// language by any means, or disclosed to third parties without the explicit written
// agreement of TransGaming Inc. Without such an agreement, no rights or licenses, express
// or implied, including but not limited to any patent rights, are granted to you.
//

// Fence.h: Defines the Fence class, which supports the GL_NV_fence extension.

#ifndef LIBGLESV2_FENCE_H_
#define LIBGLESV2_FENCE_H_

#include "common/Object.hpp"
#include <GLES2/gl2.h>

namespace es2
{

class Fence
{
  public:
    Fence();
    virtual ~Fence();

    GLboolean isFence();
    void setFence(GLenum condition);
    GLboolean testFence();
    void finishFence();
    void getFenceiv(GLenum pname, GLint *params);

  private:
    bool mQuery;
    GLenum mCondition;
    GLboolean mStatus;
};

class FenceSync : public gl::NamedObject
{
public:
	FenceSync(GLuint name, GLenum condition, GLbitfield flags);
	virtual ~FenceSync();

	GLenum clientWait(GLbitfield flags, GLuint64 timeout);
	void serverWait(GLbitfield flags, GLuint64 timeout);

	GLenum getCondition() const { return mCondition; }
	GLbitfield getFlags() const { return mFlags; }

private:
	GLenum mCondition;
	GLbitfield mFlags;
};

}

#endif   // LIBGLESV2_FENCE_H_
