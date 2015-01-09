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

// IndexDataManager.h: Defines the IndexDataManager, a class that
// runs the Buffer translation process for index buffers.

#ifndef LIBGL_INDEXDATAMANAGER_H_
#define LIBGL_INDEXDATAMANAGER_H_

#include "Context.h"

#define GL_APICALL
#include <GLES2/gl2.h>

namespace es2
{

struct TranslatedIndexData
{
    unsigned int minIndex;
    unsigned int maxIndex;
    unsigned int indexOffset;

    sw::Resource *indexBuffer;
};

class StreamingIndexBuffer
{
  public:
    StreamingIndexBuffer(unsigned int initialSize);
    virtual ~StreamingIndexBuffer();

    void *map(unsigned int requiredSpace, unsigned int *offset);
	void unmap();
    void reserveSpace(unsigned int requiredSpace, GLenum type);

	sw::Resource *getResource() const;

  private:
    sw::Resource *mIndexBuffer;
    unsigned int mBufferSize;
    unsigned int mWritePosition;
};

class IndexDataManager
{
  public:
    IndexDataManager();
    virtual ~IndexDataManager();

    GLenum prepareIndexData(GLenum type, GLsizei count, Buffer *arrayElementBuffer, const void *indices, TranslatedIndexData *translated);

	static std::size_t typeSize(GLenum type);

  private:
    StreamingIndexBuffer *mStreamingBuffer;
};

}

#endif   // LIBGL_INDEXDATAMANAGER_H_
