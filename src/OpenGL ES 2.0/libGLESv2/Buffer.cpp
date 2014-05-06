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

// Buffer.cpp: Implements the Buffer class, representing storage of vertex and/or
// index data. Implements GL buffer objects and related functionality.
// [OpenGL ES 2.0.24] section 2.9 page 21.

#include "Buffer.h"

#include "main.h"
#include "VertexDataManager.h"
#include "IndexDataManager.h"

namespace gl
{

Buffer::Buffer(GLuint id) : RefCountObject(id)
{
    mContents = 0;
    mSize = 0;
    mUsage = GL_DYNAMIC_DRAW;
}

Buffer::~Buffer()
{
    if(mContents)
	{
		mContents->destruct();
	}
}

void Buffer::bufferData(const void *data, GLsizeiptr size, GLenum usage)
{
    if(size == 0)
    {
		if(mContents)
		{
			mContents->destruct();
			mContents = 0;
		}
    }
    else if(size != mSize)
    {
        if(mContents)
		{
			mContents->destruct();
			mContents = 0;
		}

		mContents = new sw::Resource(size + 1024);

		if(mContents)
		{
			memset((void*)mContents->getBuffer(), 0, size);
		}
    }

    if(data != NULL && size > 0)
    {
		memcpy((void*)mContents->getBuffer(), data, size);
    }

    mSize = size;
    mUsage = usage;
}

void Buffer::bufferSubData(const void *data, GLsizeiptr size, GLintptr offset)
{
	if(mContents)
	{
		char *buffer = (char*)mContents->lock(sw::PUBLIC);
		memcpy(buffer + offset, data, size);
		mContents->unlock();
	}
}

sw::Resource *Buffer::getResource()
{
	return mContents;
}

}
