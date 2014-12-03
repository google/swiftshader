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

namespace es2
{

IndexDataManager::IndexDataManager()
{
}

IndexDataManager::~IndexDataManager()
{
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
    else UNREACHABLE();
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
    else UNREACHABLE();
}

GLenum IndexDataManager::prepareIndexData(GLenum type, GLsizei count, sw::Resource *buffer, const void *indices, TranslatedIndexData *translated)
{
    intptr_t offset = reinterpret_cast<intptr_t>(indices);
    bool alignedOffset = false;

    switch(type)
    {
    case GL_UNSIGNED_BYTE:  alignedOffset = (offset % sizeof(GLubyte) == 0);  break;
    case GL_UNSIGNED_SHORT: alignedOffset = (offset % sizeof(GLushort) == 0); break;
    case GL_UNSIGNED_INT:   alignedOffset = (offset % sizeof(GLuint) == 0);   break;
    default: UNREACHABLE(); alignedOffset = false;
    }

    if(typeSize(type) * count + offset > static_cast<std::size_t>(buffer->size))
    {
        return GL_INVALID_OPERATION;
    }

    indices = static_cast<const GLubyte*>(buffer->data()) + offset;
	
	computeRange(type, indices, count, &translated->minIndex, &translated->maxIndex);

	translated->indexBuffer = buffer;
	translated->indexOffset = offset;
    
    return GL_NO_ERROR;
}

std::size_t IndexDataManager::typeSize(GLenum type)
{
    switch(type)
    {
    case GL_UNSIGNED_INT:   return sizeof(GLuint);
    case GL_UNSIGNED_SHORT: return sizeof(GLushort);
    case GL_UNSIGNED_BYTE:  return sizeof(GLubyte);
    default: UNREACHABLE(); return sizeof(GLushort);
    }
}

}
