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

class NameSpace
{
  public:
    NameSpace();
    virtual ~NameSpace();

    void setBaseHandle(GLuint value);

    GLuint allocate();
    void release(GLuint handle);

private:
    GLuint baseValue;
    GLuint nextValue;
    typedef std::vector<GLuint> HandleList;
    HandleList freeValues;
};

}

#endif   // gl_NameSpace_hpp
