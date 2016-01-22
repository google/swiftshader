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

#include "common/Object.hpp"
#include "Common/Resource.hpp"

#include <GLES2/gl2.h>

#include <cstddef>
#include <vector>

namespace es2
{
class Buffer : public gl::NamedObject
{
  public:
    explicit Buffer(GLuint name);

    virtual ~Buffer();

    void bufferData(const void *data, GLsizeiptr size, GLenum usage);
    void bufferSubData(const void *data, GLsizeiptr size, GLintptr offset);

	const void *data() const { return mContents ? mContents->data() : 0; }
    size_t size() const { return mSize; }
    GLenum usage() const { return mUsage; }
	bool isMapped() const { return mIsMapped; }
	GLintptr offset() const { return mOffset; }
	GLsizeiptr length() const { return mLength; }
	GLbitfield access() const { return mAccess; }

	void* mapRange(GLintptr offset, GLsizeiptr length, GLbitfield access);
	bool unmap();
	void flushMappedRange(GLintptr offset, GLsizeiptr length) {}

	sw::Resource *getResource();

  private:
    sw::Resource *mContents;
    size_t mSize;
    GLenum mUsage;
	bool mIsMapped;
	GLintptr mOffset;
	GLsizeiptr mLength;
	GLbitfield mAccess;
};

class UniformBufferBinding
{
public:
	UniformBufferBinding() : offset(0), size(0) { }

	void set(Buffer *newUniformBuffer, int newOffset = 0, int newSize = 0)
	{
		uniformBuffer = newUniformBuffer;
		offset = newOffset;
		size = newSize;
	}

	int getOffset() const { return offset; }
	int getSize() const { return size; }
	const gl::BindingPointer<Buffer>& get() const { return uniformBuffer; }

private:
	gl::BindingPointer<Buffer> uniformBuffer;
	int offset;
	int size;
};

}

#endif   // LIBGLESV2_BUFFER_H_
