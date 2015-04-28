// SwiftShader Software Renderer
//
// Copyright(c) 2015 Google Inc.
//
// All rights reserved. No part of this software may be copied, distributed, transmitted,
// transcribed, stored in a retrieval system, translated into any human or computer
// language by any means, or disclosed to third parties without the explicit written
// agreement of Google Inc. Without such an agreement, no rights or licenses, express
// or implied, including but not limited to any patent rights, are granted to you.
//

#include "VertexArray.h"

namespace es2
{

VertexArray::VertexArray(GLuint name) : gl::NamedObject(name)
{
}

VertexArray::~VertexArray()
{
	for(size_t i = 0; i < MAX_VERTEX_ATTRIBS; i++)
    {
		mVertexAttributes[i].mBoundBuffer = NULL;
    }
    mElementArrayBuffer = NULL;
}

void VertexArray::detachBuffer(GLuint bufferName)
{
	for(size_t attribute = 0; attribute < MAX_VERTEX_ATTRIBS; attribute++)
    {
		if(mVertexAttributes[attribute].mBoundBuffer.name() == bufferName)
        {
			mVertexAttributes[attribute].mBoundBuffer = NULL;
        }
    }

    if (mElementArrayBuffer.name() == bufferName)
    {
        mElementArrayBuffer = NULL;
    }
}

const VertexAttribute& VertexArray::getVertexAttribute(size_t attributeIndex) const
{
	ASSERT(attributeIndex < MAX_VERTEX_ATTRIBS);
    return mVertexAttributes[attributeIndex];
}

void VertexArray::setVertexAttribDivisor(GLuint index, GLuint divisor)
{
	ASSERT(index < MAX_VERTEX_ATTRIBS);
	mVertexAttributes[index].mDivisor = divisor;
}

void VertexArray::enableAttribute(unsigned int attributeIndex, bool enabledState)
{
	ASSERT(attributeIndex < MAX_VERTEX_ATTRIBS);
	mVertexAttributes[attributeIndex].mArrayEnabled = enabledState;
}

void VertexArray::setAttributeState(unsigned int attributeIndex, Buffer *boundBuffer, GLint size, GLenum type,
                                    bool normalized, GLsizei stride, const void *pointer)
{
	ASSERT(attributeIndex < MAX_VERTEX_ATTRIBS);
    mVertexAttributes[attributeIndex].mBoundBuffer = boundBuffer;
    mVertexAttributes[attributeIndex].mSize = size;
    mVertexAttributes[attributeIndex].mType = type;
    mVertexAttributes[attributeIndex].mNormalized = normalized;
	mVertexAttributes[attributeIndex].mStride = stride;
	mVertexAttributes[attributeIndex].mPointer = pointer;
}

void VertexArray::setElementArrayBuffer(Buffer *buffer)
{
    mElementArrayBuffer = buffer;
}

}
