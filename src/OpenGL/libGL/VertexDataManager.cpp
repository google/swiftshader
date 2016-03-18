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

// VertexDataManager.h: Defines the VertexDataManager, a class that
// runs the Buffer translation process.

#include "VertexDataManager.h"

#include "Buffer.h"
#include "Program.h"
#include "IndexDataManager.h"
#include "common/debug.h"

namespace
{
    enum {INITIAL_STREAM_BUFFER_SIZE = 1024 * 1024};
}

namespace gl
{

VertexDataManager::VertexDataManager(Context *context) : mContext(context)
{
    for(int i = 0; i < MAX_VERTEX_ATTRIBS; i++)
    {
        mDirtyCurrentValue[i] = true;
        mCurrentValueBuffer[i] = NULL;
    }

    mStreamingBuffer = new StreamingVertexBuffer(INITIAL_STREAM_BUFFER_SIZE);

    if(!mStreamingBuffer)
    {
        ERR("Failed to allocate the streaming vertex buffer.");
    }
}

VertexDataManager::~VertexDataManager()
{
    delete mStreamingBuffer;

    for(int i = 0; i < MAX_VERTEX_ATTRIBS; i++)
    {
        delete mCurrentValueBuffer[i];
    }
}

unsigned int VertexDataManager::writeAttributeData(StreamingVertexBuffer *vertexBuffer, GLint start, GLsizei count, const VertexAttribute &attribute)
{
    Buffer *buffer = attribute.mBoundBuffer;

    int inputStride = attribute.stride();
    int elementSize = attribute.typeSize();
    unsigned int streamOffset = 0;

    char *output = nullptr;

    if(vertexBuffer)
    {
        output = (char*)vertexBuffer->map(attribute, attribute.typeSize() * count, &streamOffset);
    }

    if(!output)
    {
        ERR("Failed to map vertex buffer.");
        return ~0u;
    }

    const char *input = nullptr;

    if(buffer)
    {
        int offset = attribute.mOffset;

        input = static_cast<const char*>(buffer->data()) + offset;
    }
    else
    {
        input = static_cast<const char*>(attribute.mPointer);
    }

    input += inputStride * start;

    if(inputStride == elementSize)
    {
        memcpy(output, input, count * inputStride);
    }
    else
    {
		for(int i = 0; i < count; i++)
		{
			memcpy(output, input, elementSize);
			output += elementSize;
			input += inputStride;
		}
    }

    vertexBuffer->unmap();

    return streamOffset;
}

GLenum VertexDataManager::prepareVertexData(GLint start, GLsizei count, TranslatedAttribute *translated)
{
    if(!mStreamingBuffer)
    {
        return GL_OUT_OF_MEMORY;
    }

    const VertexAttributeArray &attribs = mContext->getVertexAttributes();
    Program *program = mContext->getCurrentProgram();

    // Determine the required storage size per used buffer
    for(int i = 0; i < MAX_VERTEX_ATTRIBS; i++)
    {
        if(!program || program->getAttributeStream(i) != -1)
        {
            if(attribs[i].mArrayEnabled)
            {
                if(!attribs[i].mBoundBuffer)
                {
                    mStreamingBuffer->addRequiredSpace(attribs[i].typeSize() * count);
                }
            }
        }
    }

    mStreamingBuffer->reserveRequiredSpace();

    // Perform the vertex data translations
    for(int i = 0; i < MAX_VERTEX_ATTRIBS; i++)
    {
        if(!program || program->getAttributeStream(i) != -1)
        {
            if(attribs[i].mArrayEnabled)
            {
                Buffer *buffer = attribs[i].mBoundBuffer;

                if(!buffer && attribs[i].mPointer == NULL)
                {
                    // This is an application error that would normally result in a crash, but we catch it and return an error
                    ERR("An enabled vertex array has no buffer and no pointer.");
                    return GL_INVALID_OPERATION;
                }

                sw::Resource *staticBuffer = buffer ? buffer->getResource() : NULL;

                if(staticBuffer)
                {
					translated[i].vertexBuffer = staticBuffer;
					translated[i].offset = start * attribs[i].stride() + attribs[i].mOffset;
					translated[i].stride = attribs[i].stride();
                }
                else
                {
                    unsigned int streamOffset = writeAttributeData(mStreamingBuffer, start, count, attribs[i]);

					if(streamOffset == ~0u)
					{
						return GL_OUT_OF_MEMORY;
					}

					translated[i].vertexBuffer = mStreamingBuffer->getResource();
					translated[i].offset = streamOffset;
					translated[i].stride = attribs[i].typeSize();
                }

				switch(attribs[i].mType)
				{
				case GL_BYTE:           translated[i].type = sw::STREAMTYPE_SBYTE;  break;
				case GL_UNSIGNED_BYTE:  translated[i].type = sw::STREAMTYPE_BYTE;   break;
				case GL_SHORT:          translated[i].type = sw::STREAMTYPE_SHORT;  break;
				case GL_UNSIGNED_SHORT: translated[i].type = sw::STREAMTYPE_USHORT; break;
				case GL_INT:            translated[i].type = sw::STREAMTYPE_INT;    break;
				case GL_UNSIGNED_INT:   translated[i].type = sw::STREAMTYPE_UINT;   break;
				case GL_FIXED:          translated[i].type = sw::STREAMTYPE_FIXED;  break;
				case GL_FLOAT:          translated[i].type = sw::STREAMTYPE_FLOAT;  break;
				default: UNREACHABLE(attribs[i].mType); translated[i].type = sw::STREAMTYPE_FLOAT;  break;
				}

				translated[i].count = attribs[i].mSize;
				translated[i].normalized = attribs[i].mNormalized;
            }
            else
            {
                if(mDirtyCurrentValue[i])
                {
                    delete mCurrentValueBuffer[i];
                    mCurrentValueBuffer[i] = new ConstantVertexBuffer(attribs[i].mCurrentValue[0], attribs[i].mCurrentValue[1], attribs[i].mCurrentValue[2], attribs[i].mCurrentValue[3]);
                    mDirtyCurrentValue[i] = false;
                }

                translated[i].vertexBuffer = mCurrentValueBuffer[i]->getResource();

                translated[i].type = sw::STREAMTYPE_FLOAT;
				translated[i].count = 4;
                translated[i].stride = 0;
                translated[i].offset = 0;
            }
        }
    }

    return GL_NO_ERROR;
}

VertexBuffer::VertexBuffer(unsigned int size) : mVertexBuffer(NULL)
{
    if(size > 0)
    {
        mVertexBuffer = new sw::Resource(size + 1024);

        if(!mVertexBuffer)
        {
            ERR("Out of memory allocating a vertex buffer of size %u.", size);
        }
    }
}

VertexBuffer::~VertexBuffer()
{
    if(mVertexBuffer)
    {
        mVertexBuffer->destruct();
    }
}

void VertexBuffer::unmap()
{
    if(mVertexBuffer)
    {
		mVertexBuffer->unlock();
    }
}

sw::Resource *VertexBuffer::getResource() const
{
    return mVertexBuffer;
}

ConstantVertexBuffer::ConstantVertexBuffer(float x, float y, float z, float w) : VertexBuffer(4 * sizeof(float))
{
    if(mVertexBuffer)
    {
		float *vector = (float*)mVertexBuffer->lock(sw::PUBLIC);

        vector[0] = x;
        vector[1] = y;
        vector[2] = z;
        vector[3] = w;

        mVertexBuffer->unlock();
    }
}

ConstantVertexBuffer::~ConstantVertexBuffer()
{
}

StreamingVertexBuffer::StreamingVertexBuffer(unsigned int size) : VertexBuffer(size)
{
    mBufferSize = size;
    mWritePosition = 0;
    mRequiredSpace = 0;
}

StreamingVertexBuffer::~StreamingVertexBuffer()
{
}

void StreamingVertexBuffer::addRequiredSpace(unsigned int requiredSpace)
{
    mRequiredSpace += requiredSpace;
}

void *StreamingVertexBuffer::map(const VertexAttribute &attribute, unsigned int requiredSpace, unsigned int *offset)
{
    void *mapPtr = NULL;

    if(mVertexBuffer)
    {
		// We can use a private lock because we never overwrite the content
		mapPtr = (char*)mVertexBuffer->lock(sw::PRIVATE) + mWritePosition;

        *offset = mWritePosition;
        mWritePosition += requiredSpace;
    }

    return mapPtr;
}

void StreamingVertexBuffer::reserveRequiredSpace()
{
    if(mRequiredSpace > mBufferSize)
    {
        if(mVertexBuffer)
        {
            mVertexBuffer->destruct();
            mVertexBuffer = 0;
        }

        mBufferSize = std::max(mRequiredSpace, 3 * mBufferSize / 2);   // 1.5 x mBufferSize is arbitrary and should be checked to see we don't have too many reallocations.

		mVertexBuffer = new sw::Resource(mBufferSize);

        if(!mVertexBuffer)
        {
            ERR("Out of memory allocating a vertex buffer of size %u.", mBufferSize);
        }

        mWritePosition = 0;
    }
    else if(mWritePosition + mRequiredSpace > mBufferSize)   // Recycle
    {
        if(mVertexBuffer)
        {
            mVertexBuffer->destruct();
			mVertexBuffer = new sw::Resource(mBufferSize);
        }

        mWritePosition = 0;
    }

    mRequiredSpace = 0;
}

}
