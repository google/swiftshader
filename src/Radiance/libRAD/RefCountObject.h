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

// RefCountObject.h: Defines the RefCountObject base class that provides
// lifecycle support for GL objects using the traditional BindObject scheme, but
// that need to be reference counted for correct cross-context deletion.
// (Concretely, textures, buffers and renderbuffers.)

#ifndef LIBGLESV2_REFCOUNTOBJECT_H_
#define LIBGLESV2_REFCOUNTOBJECT_H_

#include "common/debug.h"

#define GL_APICALL
#include <GLES2/gl2.h>

#include <cstddef>

namespace rad
{

class RefCountObject
{
  public:
    explicit RefCountObject(GLuint id);
    virtual ~RefCountObject();

    virtual void addRef();
	virtual void release();

    GLuint id() const {return mId;}
    
  private:
    GLuint mId;

    volatile int referenceCount;
};

class RefCountObjectBindingPointer
{
  protected:
    RefCountObjectBindingPointer() : mObject(NULL) { }
    ~RefCountObjectBindingPointer() { ASSERT(mObject == NULL); } // Objects have to be released before the resource manager is destroyed, so they must be explicitly cleaned up.

    void set(RefCountObject *newObject);
    RefCountObject *get() const { return mObject; }

  public:
    GLuint id() const { return (mObject != NULL) ? mObject->id() : 0; }
    bool operator ! () const { return (get() == NULL); }

  private:
    RefCountObject *mObject;
};

template<class ObjectType>
class BindingPointer : public RefCountObjectBindingPointer
{
  public:
    void set(ObjectType *newObject) { RefCountObjectBindingPointer::set(newObject); }
    ObjectType *get() const { return static_cast<ObjectType*>(RefCountObjectBindingPointer::get()); }
    ObjectType *operator -> () const { return get(); }
};

}

#endif   // LIBGLESV2_REFCOUNTOBJECT_H_
