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

// Object.cpp: Defines the Object base class that provides
// lifecycle support for GL objects using the traditional BindObject scheme, but
// that need to be reference counted for correct cross-context deletion.

#include "Object.hpp"

#include "Common/Thread.hpp"

namespace gl
{
#ifndef NDEBUG
std::set<Object*> Object::instances;
#endif

Object::Object()
{
	referenceCount = 0;

	#ifndef NDEBUG
		instances.insert(this);
	#endif
}

Object::~Object()
{
    ASSERT(referenceCount == 0);

	#ifndef NDEBUG
		ASSERT(instances.find(this) != instances.end());   // Check for double deletion
		instances.erase(this);
	#endif
}

void Object::addRef()
{
	sw::atomicIncrement(&referenceCount);
}

void Object::release()
{
    ASSERT(referenceCount > 0);

    if(referenceCount > 0)
	{
		sw::atomicDecrement(&referenceCount);
	}

	if(referenceCount == 0)
	{
		delete this;
	}
}

NamedObject::NamedObject(GLuint name) : name(name)
{
}

NamedObject::~NamedObject()
{
}

#ifndef NDEBUG
struct ObjectLeakCheck
{
	~ObjectLeakCheck()
	{
		ASSERT(Object::instances.empty());   // Check for GL object leak at termination
	}
};

static ObjectLeakCheck objectLeakCheck;
#endif

}
