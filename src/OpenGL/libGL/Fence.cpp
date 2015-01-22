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

// Fence.cpp: Implements the Fence class, which supports the GL_NV_fence extension.

#include "Fence.h"

#include "main.h"
#include "Common/Thread.hpp"

namespace gl
{

Fence::Fence()
{ 
    mQuery = false;
    mCondition = GL_NONE;
    mStatus = GL_FALSE;
}

Fence::~Fence()
{
    mQuery = false;
}

GLboolean Fence::isFence()
{
    // GL_NV_fence spec:
    // A name returned by GenFencesNV, but not yet set via SetFenceNV, is not the name of an existing fence.
    return mQuery;
}

void Fence::setFence(GLenum condition)
{
    if(condition != GL_ALL_COMPLETED_NV)
    {
        return error(GL_INVALID_VALUE);
    }

    mQuery = true;
    mCondition = condition;
    mStatus = GL_FALSE;
}

GLboolean Fence::testFence()
{
    if(!mQuery)
    {
        return error(GL_INVALID_OPERATION, GL_TRUE);
    }

    // The current assumtion is that no matter where the fence is placed, it is
    // done by the time it is tested, which is similar to Context::flush(), since
    // we don't queue anything without processing it as fast as possible.
    mStatus = GL_TRUE;

    return mStatus;
}

void Fence::finishFence()
{
    if(!mQuery)
    {
        return error(GL_INVALID_OPERATION);
    }

    while(!testFence())
    {
        sw::Thread::yield();
    }
}

void Fence::getFenceiv(GLenum pname, GLint *params)
{
    if(!mQuery)
    {
        return error(GL_INVALID_OPERATION);
    }

    switch (pname)
    {
    case GL_FENCE_STATUS_NV:
		{
			// GL_NV_fence spec:
			// Once the status of a fence has been finished (via FinishFenceNV) or tested and the returned status is TRUE (via either TestFenceNV
			// or GetFenceivNV querying the FENCE_STATUS_NV), the status remains TRUE until the next SetFenceNV of the fence.
			if(mStatus)
			{
				params[0] = GL_TRUE;
				return;
			}
            
			mStatus = testFence();

			params[0] = mStatus;            
			break;
		}
    case GL_FENCE_CONDITION_NV:
        params[0] = mCondition;
        break;
    default:
        return error(GL_INVALID_ENUM);
        break;
    }
}

}
