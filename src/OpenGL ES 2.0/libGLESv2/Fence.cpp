//
// Copyright (c) 2002-2010 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//

// Fence.cpp: Implements the Fence class, which supports the GL_NV_fence extension.

#include "Fence.h"

#include "main.h"

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

	UNIMPLEMENTED();
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
        Sleep(0);
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
