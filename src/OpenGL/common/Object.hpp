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

// Object.hpp: Defines the Object base class that provides
// lifecycle support for GL objects using the traditional BindObject scheme, but
// that need to be reference counted for correct cross-context deletion.

#ifndef gl_Object_hpp
#define gl_Object_hpp

#include "common/debug.h"

#include <set>

typedef unsigned int GLuint;

namespace gl
{

class Object
{
public:
    Object();
    virtual ~Object();

    virtual void addRef();
	virtual void release();

private:
    volatile int referenceCount;

#ifndef NDEBUG
public:
	static std::set<Object*> instances;   // For leak checking
#endif
};

class NamedObject : public Object
{
public:
    explicit NamedObject(GLuint name);
    virtual ~NamedObject();

    const GLuint name;
};

template<class ObjectType>
class BindingPointer
{
public:
	BindingPointer() : object(nullptr) { }

	BindingPointer(const BindingPointer<ObjectType> &other) : object(nullptr)
    {
        operator=(other.object);
    }

	~BindingPointer()
	{
		ASSERT(!object);   // Objects have to be released before the resource manager is destroyed, so they must be explicitly cleaned up. Assign null to all binding pointers to make the reference count go to zero.
	}

    ObjectType *operator=(ObjectType *newObject)
	{
		if(newObject) newObject->addRef();
		if(object) object->release();

		object = newObject;

		return object;
	}

	ObjectType *operator=(const BindingPointer<ObjectType> &other)
	{
		return operator=(other.object);
	}

    operator ObjectType*() const { return object; }
    ObjectType *operator->() const { return object; }
	GLuint name() const { return object ? object->name : 0; }
    bool operator!() const { return !object; }

private:
    ObjectType *object;
};

}

#endif   // gl_Object_hpp
