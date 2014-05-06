//
// Copyright (c) 2002-2010 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//

// IndexDataManager.h: Defines the IndexDataManager, a class that
// runs the Buffer translation process for index buffers.

#ifndef LIBGLESV2_INDEXDATAMANAGER_H_
#define LIBGLESV2_INDEXDATAMANAGER_H_

#include "Context.h"
#include "Device.hpp"

#define GL_APICALL
#include <GLES2/gl2.h>

#include <vector>
#include <cstddef>

namespace gl
{

struct TranslatedIndexData
{
    UINT minIndex;
    UINT maxIndex;
    UINT indexOffset;

    sw::Resource *indexBuffer;
};

class StreamingIndexBuffer
{
  public:
    StreamingIndexBuffer(Device *device, UINT initialSize);
    virtual ~StreamingIndexBuffer();

    void *map(UINT requiredSpace, UINT *offset);
	void unmap();
    void reserveSpace(UINT requiredSpace, GLenum type);

	UINT size() const {return mBufferSize;}
	sw::Resource *getResource() const;

  private:
	Device *const mDevice;

    sw::Resource *mIndexBuffer;
    UINT mBufferSize;
    UINT mWritePosition;
};

class IndexDataManager
{
  public:
    IndexDataManager(Context *context, Device *evice);
    virtual ~IndexDataManager();

    GLenum prepareIndexData(GLenum type, GLsizei count, Buffer *arrayElementBuffer, const void *indices, TranslatedIndexData *translated);

	static std::size_t typeSize(GLenum type);

  private:
    DISALLOW_COPY_AND_ASSIGN(IndexDataManager);

    Device *const mDevice;

    StreamingIndexBuffer *mStreamingBuffer;
};

}

#endif   // LIBGLESV2_INDEXDATAMANAGER_H_
