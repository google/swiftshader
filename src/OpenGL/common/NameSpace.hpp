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
#include "debug.h"

#include <unordered_map>

namespace gl
{

template<class ObjectType, GLuint baseName = 1>
class NameSpace
{
public:
    NameSpace() : freeName(baseName)
	{
	}

	~NameSpace()
	{
		ASSERT(empty());
	}

	bool empty()
	{
		return map.empty();
	}

	GLuint firstName()
	{
		return map.begin()->first;
	}

    GLuint allocate()
	{
		GLuint name = freeName;

		while(isReserved(name))
		{
			name++;
		}

		map.insert({name, nullptr});
		freeName = name + 1;

		return name;
	}

	bool isReserved(GLuint name)
	{
		return map.find(name) != map.end();
	}

	void insert(GLuint name, ObjectType *object)
	{
		map[name] = object;

		if(name == freeName)
		{
			freeName++;
		}
	}

    ObjectType *remove(GLuint name)
	{
		auto element = map.find(name);

		if(element != map.end())
		{
			ObjectType *object = element->second;
			map.erase(element);

			if(name < freeName)
			{
				freeName = name;
			}

			return object;
		}

		return nullptr;
	}

	ObjectType *find(GLuint name)
	{
		if(name < baseName)
		{
			return nullptr;
		}

		auto element = map.find(name);

		if(element == map.end())
		{
			return nullptr;
		}

		return element->second;
	}

private:
	typedef std::unordered_map<GLuint, ObjectType*> Map;
	Map map;

	GLuint freeName;   // Lowest known potentially free name
};

}

#endif   // gl_NameSpace_hpp
