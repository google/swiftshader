//
// Copyright (c) 2002-2010 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//

// VertexDataManager.h: Defines the VertexDataManager, a class that
// runs the Buffer translation process.

#ifndef LIBGLESV2_VERTEXDATAMANAGER_H_
#define LIBGLESV2_VERTEXDATAMANAGER_H_

#include "Context.h"
#include "Device.hpp"

#define GL_APICALL
#include <GLES2/gl2.h>

#include <vector>
#include <cstddef>

namespace gl
{

struct TranslatedAttribute
{
    bool active;

    sw::StreamType type;
	int count;
	bool normalized;

    UINT offset;
    UINT stride;   // 0 means not to advance the read pointer at all

    sw::Resource *vertexBuffer;
};

class VertexBuffer
{
  public:
    VertexBuffer(Device *device, UINT size);
    virtual ~VertexBuffer();

    void unmap();

    sw::Resource *getResource() const;

  protected:
    Device *const mDevice;
    sw::Resource *mVertexBuffer;

  private:
    DISALLOW_COPY_AND_ASSIGN(VertexBuffer);
};

class ConstantVertexBuffer : public VertexBuffer
{
  public:
    ConstantVertexBuffer(Device *device, float x, float y, float z, float w);
    ~ConstantVertexBuffer();
};

class ArrayVertexBuffer : public VertexBuffer
{
  public:
    ArrayVertexBuffer(Device *device, UINT size);
    ~ArrayVertexBuffer();

    UINT size() const { return mBufferSize; }
    virtual void *map(const VertexAttribute &attribute, UINT requiredSpace, UINT *streamOffset) = 0;
    virtual void reserveRequiredSpace() = 0;
    void addRequiredSpace(UINT requiredSpace);

  protected:
    UINT mBufferSize;
    UINT mWritePosition;
    UINT mRequiredSpace;
};

class StreamingVertexBuffer : public ArrayVertexBuffer
{
  public:
    StreamingVertexBuffer(Device *device, UINT initialSize);
    ~StreamingVertexBuffer();

    void *map(const VertexAttribute &attribute, UINT requiredSpace, UINT *streamOffset);
    void reserveRequiredSpace();
};

class VertexDataManager
{
  public:
    VertexDataManager(Context *context, Device *backend);
    virtual ~VertexDataManager();

    void dirtyCurrentValue(int index) { mDirtyCurrentValue[index] = true; }

    GLenum prepareVertexData(GLint start, GLsizei count, TranslatedAttribute *outAttribs);

  private:
    DISALLOW_COPY_AND_ASSIGN(VertexDataManager);

    UINT spaceRequired(const VertexAttribute &attrib, std::size_t count) const;
    UINT writeAttributeData(ArrayVertexBuffer *vertexBuffer, GLint start, GLsizei count, const VertexAttribute &attribute);

    Context *const mContext;
    Device *const mDevice;

    StreamingVertexBuffer *mStreamingBuffer;

    bool mDirtyCurrentValue[MAX_VERTEX_ATTRIBS];
    ConstantVertexBuffer *mCurrentValueBuffer[MAX_VERTEX_ATTRIBS];
};

}

#endif   // LIBGLESV2_VERTEXDATAMANAGER_H_
