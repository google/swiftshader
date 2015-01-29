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

typedef unsigned int GLuint;

namespace gl
{

class Object
{
public:
    explicit Object(GLuint name);
    virtual ~Object();

    virtual void addRef();
	virtual void release();

    const GLuint name;
    
private:
    volatile int referenceCount;
};

template<class ObjectType>
class BindingPointer
{
public:
	BindingPointer() : object(nullptr) { }

	~BindingPointer() { ASSERT(!object); } // Objects have to be released before the resource manager is destroyed, so they must be explicitly cleaned up.

    void set(ObjectType *newObject) 
	{
		if(newObject) newObject->addRef();
		if(object) object->release();

		object = newObject;
	}
    ObjectType *get() const { return object; }
    ObjectType *operator->() const { return object; }

	GLuint name() const { return object ? object->name : 0; }
    bool operator!() const { return !object; }

private:
    ObjectType *object;
};

}

#endif   // gl_Object_hpp
