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

// NameSpace.h: Defines the NameSpace class, which is used to
// allocate GL object names.

#ifndef gl_NameSpace_hpp
#define gl_NameSpace_hpp

#include <vector>

typedef unsigned int GLuint;

namespace gl
{

template<class ObjectType, GLuint baseName = 1>
class NameSpace
{
public:
    NameSpace() : baseValue(baseName), nextValue(baseName)
	{
	}

    GLuint allocate()
	{
		if(freeValues.size())
		{
			GLuint handle = freeValues.back();
			freeValues.pop_back();

			return handle;
		}

		return nextValue++;
	}

    void release(GLuint handle)
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

private:
    GLuint baseValue;
    GLuint nextValue;
    typedef std::vector<GLuint> HandleList;
    HandleList freeValues;
};

}

#endif   // gl_NameSpace_hpp
