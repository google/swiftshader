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

// IndexDataManager.cpp: Defines the IndexDataManager, a class that
// runs the Buffer translation process for index buffers.

#include "IndexDataManager.h"

#include "Buffer.h"
#include "common/debug.h"

#include <string.h>
#include <algorithm>

namespace
{
    enum { INITIAL_INDEX_BUFFER_SIZE = 4096 * sizeof(GLuint) };
}

namespace gl
{

IndexDataManager::IndexDataManager()
{
    mStreamingBuffer = new StreamingIndexBuffer(INITIAL_INDEX_BUFFER_SIZE);

    if(!mStreamingBuffer)
    {
        ERR("Failed to allocate the streaming index buffer.");
    }
}

IndexDataManager::~IndexDataManager()
{
    delete mStreamingBuffer;
}

void copyIndices(GLenum type, const void *input, GLsizei count, void *output)
{
    if(type == GL_UNSIGNED_BYTE)
    {
        memcpy(output, input, count * sizeof(GLubyte));
    }
    else if(type == GL_UNSIGNED_INT)
    {
        memcpy(output, input, count * sizeof(GLuint));
    }
    else if(type == GL_UNSIGNED_SHORT)
    {
        memcpy(output, input, count * sizeof(GLushort));
    }
    else UNREACHABLE(type);
}

template<class IndexType>
void computeRange(const IndexType *indices, GLsizei count, GLuint *minIndex, GLuint *maxIndex)
{
    *minIndex = indices[0];
    *maxIndex = indices[0];

    for(GLsizei i = 0; i < count; i++)
    {
        if(*minIndex > indices[i]) *minIndex = indices[i];
        if(*maxIndex < indices[i]) *maxIndex = indices[i];
    }
}

void computeRange(GLenum type, const void *indices, GLsizei count, GLuint *minIndex, GLuint *maxIndex)
{
    if(type == GL_UNSIGNED_BYTE)
    {
        computeRange(static_cast<const GLubyte*>(indices), count, minIndex, maxIndex);
    }
    else if(type == GL_UNSIGNED_INT)
    {
        computeRange(static_cast<const GLuint*>(indices), count, minIndex, maxIndex);
    }
    else if(type == GL_UNSIGNED_SHORT)
    {
        computeRange(static_cast<const GLushort*>(indices), count, minIndex, maxIndex);
    }
    else UNREACHABLE(type);
}

GLenum IndexDataManager::prepareIndexData(GLenum type, GLsizei count, Buffer *buffer, const void *indices, TranslatedIndexData *translated)
{
    if(!mStreamingBuffer)
    {
        return GL_OUT_OF_MEMORY;
    }

    intptr_t offset = reinterpret_cast<intptr_t>(indices);
    bool alignedOffset = false;

    if(buffer != NULL)
    {
        switch(type)
        {
          case GL_UNSIGNED_BYTE:  alignedOffset = (offset % sizeof(GLubyte) == 0);  break;
          case GL_UNSIGNED_SHORT: alignedOffset = (offset % sizeof(GLushort) == 0); break;
          case GL_UNSIGNED_INT:   alignedOffset = (offset % sizeof(GLuint) == 0);   break;
          default: UNREACHABLE(type); alignedOffset = false;
        }

        if(typeSize(type) * count + offset > static_cast<std::size_t>(buffer->size()))
        {
            return GL_INVALID_OPERATION;
        }

        indices = static_cast<const GLubyte*>(buffer->data()) + offset;
    }

    StreamingIndexBuffer *streamingBuffer = mStreamingBuffer;

	sw::Resource *staticBuffer = buffer ? buffer->getResource() : NULL;

    if(staticBuffer)
    {
        computeRange(type, indices, count, &translated->minIndex, &translated->maxIndex);

		translated->indexBuffer = staticBuffer;
		translated->indexOffset = offset;
    }
    else
    {
		unsigned int streamOffset = 0;
        int convertCount = count;

        streamingBuffer->reserveSpace(convertCount * typeSize(type), type);
        void *output = streamingBuffer->map(typeSize(type) * convertCount, &streamOffset);
        
        if(output == NULL)
        {
            ERR("Failed to map index buffer.");
            return GL_OUT_OF_MEMORY;
        }

        copyIndices(type, staticBuffer ? buffer->data() : indices, convertCount, output);
        streamingBuffer->unmap();

        computeRange(type, indices, count, &translated->minIndex, &translated->maxIndex);

		translated->indexBuffer = streamingBuffer->getResource();
		translated->indexOffset = streamOffset;
    }

    return GL_NO_ERROR;
}

std::size_t IndexDataManager::typeSize(GLenum type)
{
    switch(type)
    {
    case GL_UNSIGNED_INT:   return sizeof(GLuint);
    case GL_UNSIGNED_SHORT: return sizeof(GLushort);
    case GL_UNSIGNED_BYTE:  return sizeof(GLubyte);
    default: UNREACHABLE(type); return sizeof(GLushort);
    }
}

StreamingIndexBuffer::StreamingIndexBuffer(unsigned int initialSize) : mBufferSize(initialSize), mIndexBuffer(NULL)
{
	if(initialSize > 0)
    {
		mIndexBuffer = new sw::Resource(initialSize + 16);

        if(!mIndexBuffer)
        {
            ERR("Out of memory allocating an index buffer of size %u.", initialSize);
        }
    }

    mWritePosition = 0;
}

StreamingIndexBuffer::~StreamingIndexBuffer()
{
	if(mIndexBuffer)
    {
        mIndexBuffer->destruct();
    }
}

void *StreamingIndexBuffer::map(unsigned int requiredSpace, unsigned int *offset)
{
    void *mapPtr = NULL;

    if(mIndexBuffer)
    {
        mapPtr = (char*)mIndexBuffer->lock(sw::PUBLIC) + mWritePosition;
     
        if(!mapPtr)
        {
            ERR(" Lock failed");
            return NULL;
        }

        *offset = mWritePosition;
        mWritePosition += requiredSpace;
    }

    return mapPtr;
}

void StreamingIndexBuffer::unmap()
{
    if(mIndexBuffer)
    {
        mIndexBuffer->unlock();
    }
}

void StreamingIndexBuffer::reserveSpace(unsigned int requiredSpace, GLenum type)
{
    if(requiredSpace > mBufferSize)
    {
        if(mIndexBuffer)
        {
            mIndexBuffer->destruct();
            mIndexBuffer = 0;
        }

        mBufferSize = std::max(requiredSpace, 2 * mBufferSize);

		mIndexBuffer = new sw::Resource(mBufferSize + 16);

        if(!mIndexBuffer)
        {
            ERR("Out of memory allocating an index buffer of size %u.", mBufferSize);
        }

        mWritePosition = 0;
    }
    else if(mWritePosition + requiredSpace > mBufferSize)   // Recycle
    {
		if(mIndexBuffer)
		{
			mIndexBuffer->destruct();
			mIndexBuffer = new sw::Resource(mBufferSize + 16);
		}

        mWritePosition = 0;
    }
}

sw::Resource *StreamingIndexBuffer::getResource() const
{
    return mIndexBuffer;
}

}
