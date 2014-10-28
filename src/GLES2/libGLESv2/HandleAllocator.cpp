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

// HandleAllocator.cpp: Implements the HandleAllocator class, which is used
// to allocate GL handles.

#include "HandleAllocator.h"

#include "main.h"

namespace gl2
{

HandleAllocator::HandleAllocator() : mBaseValue(1), mNextValue(1)
{
}

HandleAllocator::~HandleAllocator()
{
}

void HandleAllocator::setBaseHandle(GLuint value)
{
    ASSERT(mBaseValue == mNextValue);
    mBaseValue = value;
    mNextValue = value;
}

GLuint HandleAllocator::allocate()
{
    if(mFreeValues.size())
    {
        GLuint handle = mFreeValues.back();
        mFreeValues.pop_back();
        return handle;
    }
    return mNextValue++;
}

void HandleAllocator::release(GLuint handle)
{
    if(handle == mNextValue - 1)
    {
        // Don't drop below base value
        if(mNextValue > mBaseValue)
        {
            mNextValue--;
        }
    }
    else
    {
        // Only free handles that we own - don't drop below the base value
        if(handle >= mBaseValue)
        {
            mFreeValues.push_back(handle);
        }
    }
}

}
