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

// Buffer.h: Defines the Buffer class, representing storage of vertex and/or
// index data. Implements GL buffer objects and related functionality.
// [OpenGL ES 2.0.24] section 2.9 page 21.

#ifndef LIBGLESV2_BUFFER_H_
#define LIBGLESV2_BUFFER_H_

#include "RefCountObject.h"
#include "Common/Resource.hpp"

#define GL_APICALL
#include <GLES2/gl2.h>

#include <cstddef>
#include <vector>

namespace rad
{
class Buffer : public RefCountObject
{
  public:
    explicit Buffer(GLuint id);

    virtual ~Buffer();

    void bufferData(const void *data, GLsizeiptr size, GLenum usage);
    void bufferSubData(const void *data, GLsizeiptr size, GLintptr offset);

	const void *data() { return mContents ? mContents->getBuffer() : 0; }
    size_t size() const { return mSize; }
    GLenum usage() const { return mUsage; }

	sw::Resource *getResource();

  private:
    sw::Resource *mContents;
    size_t mSize;
    GLenum mUsage;
};

}

#endif   // LIBGLESV2_BUFFER_H_
