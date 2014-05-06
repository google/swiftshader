//
// Copyright (c) 2002-2010 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//

// RefCountObject.cpp: Defines the RefCountObject base class that provides
// lifecycle support for GL objects using the traditional BindObject scheme, but
// that need to be reference counted for correct cross-context deletion.
// (Concretely, textures, buffers and renderbuffers.)

#include "RefCountObject.h"

#include <windows.h>

namespace gl
{

RefCountObject::RefCountObject(GLuint id)
{
    mId = id;
    
	referenceCount = 0;
	bindCount = 0;
}

RefCountObject::~RefCountObject()
{
    ASSERT(referenceCount == 0);
	ASSERT(bindCount == 0);
}

void RefCountObject::addRef()
{
    InterlockedIncrement(&referenceCount);
}

void RefCountObject::release()
{
    ASSERT(referenceCount > 0);

    if(referenceCount > 0)
	{
		InterlockedDecrement(&referenceCount);
	}

	if(referenceCount == 0 && bindCount == 0)
	{
		delete this;
	}
}

void RefCountObject::bind()
{
	InterlockedIncrement(&bindCount);
}

void RefCountObject::unbind()
{
	ASSERT(bindCount > 0);

	InterlockedDecrement(&bindCount);

	if(referenceCount == 0 && bindCount == 0)
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
