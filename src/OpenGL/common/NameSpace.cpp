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

// NameSpace.cpp: Implements the NameSpace class, which is used
// to allocate GL object names.

#include "common/NameSpace.hpp"

#include "debug.h"

namespace gl
{

NameSpace::NameSpace() : baseValue(1), nextValue(1)
{
}

NameSpace::~NameSpace()
{
}

void NameSpace::setBaseHandle(GLuint value)
{
    ASSERT(baseValue == nextValue);
    baseValue = value;
    nextValue = value;
}

GLuint NameSpace::allocate()
{
    if(freeValues.size())
    {
        GLuint handle = freeValues.back();
        freeValues.pop_back();

        return handle;
    }

    return nextValue++;
}

void NameSpace::release(GLuint handle)
{
    if(handle == nextValue - 1)
    {
        // Don't drop below base value
        if(nextValue > baseValue)
        {
            nextValue--;
        }
    }
    else
    {
        // Only free handles that we own - don't drop below the base value
        if(handle >= baseValue)
        {
            freeValues.push_back(handle);
        }
    }
}

}
