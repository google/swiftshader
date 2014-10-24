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

// HandleAllocator.h: Defines the HandleAllocator class, which is used to
// allocate GL handles.

#ifndef LIBGLES_CM_HANDLEALLOCATOR_H_
#define LIBGLES_CM_HANDLEALLOCATOR_H_

#define GL_API
#include <GLES/gl.h>

#include <vector>

namespace gl
{

class HandleAllocator
{
  public:
    HandleAllocator();
    virtual ~HandleAllocator();

    void setBaseHandle(GLuint value);

    GLuint allocate();
    void release(GLuint handle);

  private:
    GLuint mBaseValue;
    GLuint mNextValue;
    typedef std::vector<GLuint> HandleList;
    HandleList mFreeValues;
};

}

#endif   // LIBGLES_CM_HANDLEALLOCATOR_H_
