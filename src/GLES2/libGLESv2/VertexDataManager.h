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

#ifndef LIBGLESV2_VERTEXDATAMANAGER_H_
#define LIBGLESV2_VERTEXDATAMANAGER_H_

#include "Context.h"
#include "Device.hpp"

#define GL_APICALL
#include <GLES2/gl2.h>

namespace gl2
{

struct TranslatedAttribute
{
    sw::StreamType type;
	int count;
	bool normalized;

    unsigned int offset;
    unsigned int stride;   // 0 means not to advance the read pointer at all

    sw::Resource *vertexBuffer;
};

class VertexBuffer
{
  public:
    VertexBuffer(unsigned int size);
    virtual ~VertexBuffer();

    void unmap();

    sw::Resource *getResource() const;

  protected:
    sw::Resource *mVertexBuffer;
};

class ConstantVertexBuffer : public VertexBuffer
{
  public:
    ConstantVertexBuffer(float x, float y, float z, float w);
    ~ConstantVertexBuffer();
};

class StreamingVertexBuffer : public VertexBuffer
{
  public:
    StreamingVertexBuffer(unsigned int size);
    ~StreamingVertexBuffer();

    void *map(const VertexAttribute &attribute, unsigned int requiredSpace, unsigned int *streamOffset);
    void reserveRequiredSpace();
    void addRequiredSpace(unsigned int requiredSpace);

  protected:
    unsigned int mBufferSize;
    unsigned int mWritePosition;
    unsigned int mRequiredSpace;
};

class VertexDataManager
{
  public:
    VertexDataManager(Context *context);
    virtual ~VertexDataManager();

    void dirtyCurrentValue(int index) { mDirtyCurrentValue[index] = true; }

    GLenum prepareVertexData(GLint start, GLsizei count, TranslatedAttribute *outAttribs);

  private:
    unsigned int writeAttributeData(StreamingVertexBuffer *vertexBuffer, GLint start, GLsizei count, const VertexAttribute &attribute);

    Context *const mContext;

    StreamingVertexBuffer *mStreamingBuffer;

    bool mDirtyCurrentValue[MAX_VERTEX_ATTRIBS];
    ConstantVertexBuffer *mCurrentValueBuffer[MAX_VERTEX_ATTRIBS];
};

}

#endif   // LIBGLESV2_VERTEXDATAMANAGER_H_
