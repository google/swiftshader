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

// ResourceManager.h : Defines the ResourceManager class, which tracks objects
// shared by multiple GL contexts.

#ifndef LIBGLES_CM_RESOURCEMANAGER_H_
#define LIBGLES_CM_RESOURCEMANAGER_H_

#include "common/NameSpace.hpp"

#include <GLES/gl.h>

#include <map>

namespace es1
{
class Buffer;
class Texture;
class Renderbuffer;

enum TextureType
{
    TEXTURE_2D,
    TEXTURE_CUBE,
    TEXTURE_EXTERNAL,

    TEXTURE_TYPE_COUNT,
    TEXTURE_UNKNOWN
};

class ResourceManager
{
public:
    ResourceManager();
    ~ResourceManager();

    void addRef();
    void release();

    GLuint createBuffer();
    GLuint createTexture();
    GLuint createRenderbuffer();

    void deleteBuffer(GLuint buffer);
    void deleteTexture(GLuint texture);
    void deleteRenderbuffer(GLuint renderbuffer);

    Buffer *getBuffer(GLuint handle);
    Texture *getTexture(GLuint handle);
    Renderbuffer *getRenderbuffer(GLuint handle);

    void checkBufferAllocation(unsigned int buffer);
    void checkTextureAllocation(GLuint texture, TextureType type);
	void checkRenderbufferAllocation(GLuint handle);

private:
    std::size_t mRefCount;

    gl::NameSpace<Buffer> mBufferNameSpace;
	gl::NameSpace<Texture> mTextureNameSpace;
    gl::NameSpace<Renderbuffer> mRenderbufferNameSpace;
};

}

#endif // LIBGLES_CM_RESOURCEMANAGER_H_
