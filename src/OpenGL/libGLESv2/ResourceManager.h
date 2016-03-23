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

#ifndef LIBGLESV2_RESOURCEMANAGER_H_
#define LIBGLESV2_RESOURCEMANAGER_H_

#include "common/NameSpace.hpp"

#include <GLES2/gl2.h>

#include <map>

namespace es2
{
class Buffer;
class Shader;
class Program;
class Texture;
class Renderbuffer;
class Sampler;
class FenceSync;

enum TextureType
{
    TEXTURE_2D,
	TEXTURE_3D,
	TEXTURE_2D_ARRAY,
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
    GLuint createShader(GLenum type);
    GLuint createProgram();
    GLuint createTexture();
    GLuint createRenderbuffer();
    GLuint createSampler();
    GLuint createFenceSync(GLenum condition, GLbitfield flags);

    void deleteBuffer(GLuint buffer);
    void deleteShader(GLuint shader);
    void deleteProgram(GLuint program);
    void deleteTexture(GLuint texture);
    void deleteRenderbuffer(GLuint renderbuffer);
    void deleteSampler(GLuint sampler);
    void deleteFenceSync(GLuint fenceSync);

    Buffer *getBuffer(GLuint handle);
    Shader *getShader(GLuint handle);
    Program *getProgram(GLuint handle);
    Texture *getTexture(GLuint handle);
    Renderbuffer *getRenderbuffer(GLuint handle);
    Sampler *getSampler(GLuint handle);
    FenceSync *getFenceSync(GLuint handle);

    void checkBufferAllocation(unsigned int buffer);
    void checkTextureAllocation(GLuint texture, TextureType type);
    void checkRenderbufferAllocation(GLuint handle);
    void checkSamplerAllocation(GLuint sampler);

    bool isSampler(GLuint sampler);

private:
    std::size_t mRefCount;

    gl::NameSpace<Buffer> mBufferNameSpace;
    gl::NameSpace<Program> mProgramNameSpace;
	gl::NameSpace<Shader> mShaderNameSpace;
	gl::NameSpace<void> mProgramShaderNameSpace;   // Shaders and programs share a namespace
    gl::NameSpace<Texture> mTextureNameSpace;
    gl::NameSpace<Renderbuffer> mRenderbufferNameSpace;
	gl::NameSpace<Sampler> mSamplerNameSpace;
	gl::NameSpace<FenceSync> mFenceSyncNameSpace;
};

}

#endif // LIBGLESV2_RESOURCEMANAGER_H_
