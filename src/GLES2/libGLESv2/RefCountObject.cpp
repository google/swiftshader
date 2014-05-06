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

// RefCountObject.cpp: Defines the RefCountObject base class that provides
// lifecycle support for GL objects using the traditional BindObject scheme, but
// that need to be reference counted for correct cross-context deletion.
// (Concretely, textures, buffers and renderbuffers.)

#include "RefCountObject.h"

#include "Common/Thread.hpp"

namespace gl
{

RefCountObject::RefCountObject(GLuint id)
{
    mId = id;
    
	referenceCount = 0;
}

RefCountObject::~RefCountObject()
{
    ASSERT(referenceCount == 0);
}

void RefCountObject::addRef()
{
	sw::atomicIncrement(&referenceCount);
}

void RefCountObject::release()
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

void RefCountObjectBindingPointer::set(RefCountObject *newObject)
{
    // addRef first in case newObject == mObject and this is the last reference to it.
    if(newObject != NULL) newObject->addRef();
    if(mObject != NULL) mObject->release();

    mObject = newObject;
}

}
