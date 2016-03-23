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

    void setRenderbuffer(GLuint handle, Renderbuffer *renderbuffer);

    void checkBufferAllocation(unsigned int buffer);
    void checkTextureAllocation(GLuint texture, TextureType type);
    void checkRenderbufferAllocation(GLuint handle);
    void checkSamplerAllocation(GLuint sampler);

    bool isSampler(GLuint sampler);

  private:
    std::size_t mRefCount;

    typedef std::map<GLint, Buffer*> BufferMap;
    BufferMap mBufferMap;
    gl::NameSpace<Buffer> mBufferNameSpace;

    typedef std::map<GLint, Shader*> ShaderMap;
    ShaderMap mShaderMap;

    typedef std::map<GLint, Program*> ProgramMap;
    ProgramMap mProgramMap;
    gl::NameSpace<Program> mProgramShaderNameSpace;

    typedef std::map<GLint, Texture*> TextureMap;
    TextureMap mTextureMap;
    gl::NameSpace<Texture> mTextureNameSpace;

    typedef std::map<GLint, Renderbuffer*> RenderbufferMap;
    RenderbufferMap mRenderbufferMap;
    gl::NameSpace<Renderbuffer> mRenderbufferNameSpace;

	typedef std::map<GLint, Sampler*> SamplerMap;
	SamplerMap mSamplerMap;
	gl::NameSpace<Sampler> mSamplerNameSpace;

	typedef std::map<GLint, FenceSync*> FenceMap;
	FenceMap mFenceSyncMap;
	gl::NameSpace<FenceSync> mFenceSyncNameSpace;
};

}

#endif // LIBGLESV2_RESOURCEMANAGER_H_
