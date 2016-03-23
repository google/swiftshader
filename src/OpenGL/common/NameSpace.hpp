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

#include "Object.hpp"

#include <unordered_set>
#include <algorithm>

typedef unsigned int GLuint;

namespace gl
{

template<class ObjectType, GLuint baseName = 1>
class NameSpace : std::unordered_set<GLuint>
{
public:
    NameSpace() : freeName(baseName)
	{
	}

    GLuint allocate()
	{
		GLuint name = freeName;

		while(find(name) != end())
		{
			name++;
		}

		insert(name);
		freeName = name + 1;

		return name;
	}

    void release(GLuint name)
	{
		erase(name);
		freeName = std::min(name, freeName);
	}

private:
	GLuint freeName;
};

}

#endif   // gl_NameSpace_hpp
