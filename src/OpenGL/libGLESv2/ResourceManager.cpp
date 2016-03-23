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

// ResourceManager.cpp: Implements the ResourceManager class, which tracks and
// retrieves objects which may be shared by multiple Contexts.

#include "ResourceManager.h"

#include "Buffer.h"
#include "Fence.h"
#include "Program.h"
#include "Renderbuffer.h"
#include "Sampler.h"
#include "Shader.h"
#include "Texture.h"

namespace es2
{
ResourceManager::ResourceManager()
{
    mRefCount = 1;
}

ResourceManager::~ResourceManager()
{
    while(!mBufferNameSpace.empty())
    {
        deleteBuffer(mBufferNameSpace.firstName());
    }

    while(!mProgramMap.empty())
    {
        deleteProgram(mProgramMap.begin()->first);
    }

    while(!mShaderMap.empty())
    {
        deleteShader(mShaderMap.begin()->first);
    }

    while(!mRenderbufferNameSpace.empty())
    {
        deleteRenderbuffer(mRenderbufferNameSpace.firstName());
    }

    while(!mTextureNameSpace.empty())
    {
        deleteTexture(mTextureNameSpace.firstName());
    }

	while(!mSamplerNameSpace.empty())
	{
		deleteSampler(mSamplerNameSpace.firstName());
	}

	while(!mFenceSyncNameSpace.empty())
	{
		deleteFenceSync(mFenceSyncNameSpace.firstName());
	}
}

void ResourceManager::addRef()
{
    mRefCount++;
}

void ResourceManager::release()
{
    if(--mRefCount == 0)
    {
        delete this;
    }
}

// Returns an unused buffer name
GLuint ResourceManager::createBuffer()
{
    return mBufferNameSpace.allocate();
}

// Returns an unused shader/program name
GLuint ResourceManager::createShader(GLenum type)
{
    GLuint handle = mProgramShaderNameSpace.allocate();

    if(type == GL_VERTEX_SHADER)
    {
        mShaderMap[handle] = new VertexShader(this, handle);
    }
    else if(type == GL_FRAGMENT_SHADER)
    {
        mShaderMap[handle] = new FragmentShader(this, handle);
    }
    else UNREACHABLE(type);

    return handle;
}

// Returns an unused program/shader name
GLuint ResourceManager::createProgram()
{
    GLuint handle = mProgramShaderNameSpace.allocate();

    mProgramMap[handle] = new Program(this, handle);

    return handle;
}

// Returns an unused texture name
GLuint ResourceManager::createTexture()
{
	return mTextureNameSpace.allocate();
}

// Returns an unused renderbuffer name
GLuint ResourceManager::createRenderbuffer()
{
	return mRenderbufferNameSpace.allocate();
}

// Returns an unused sampler name
GLuint ResourceManager::createSampler()
{
	return mSamplerNameSpace.allocate();
}

// Returns the next unused fence name, and allocates the fence
GLuint ResourceManager::createFenceSync(GLenum condition, GLbitfield flags)
{
	GLuint name = mFenceSyncNameSpace.allocate();

	FenceSync *fenceSync = new FenceSync(name, condition, flags);
	fenceSync->addRef();

	mFenceSyncNameSpace.insert(name, fenceSync);

	return name;
}

void ResourceManager::deleteBuffer(GLuint buffer)
{
	Buffer *bufferObject = mBufferNameSpace.remove(buffer);

    if(bufferObject)
    {
		bufferObject->release();
    }
}

void ResourceManager::deleteShader(GLuint shader)
{
    ShaderMap::iterator shaderObject = mShaderMap.find(shader);

    if(shaderObject != mShaderMap.end())
    {
        if(shaderObject->second->getRefCount() == 0)
        {
			delete shaderObject->second;
			mProgramShaderNameSpace.remove(shaderObject->first);
            mShaderMap.erase(shaderObject);
        }
        else
        {
            shaderObject->second->flagForDeletion();
        }
    }
}

void ResourceManager::deleteProgram(GLuint program)
{
    ProgramMap::iterator programObject = mProgramMap.find(program);

    if(programObject != mProgramMap.end())
    {
        if(programObject->second->getRefCount() == 0)
        {
			delete programObject->second;
			mProgramShaderNameSpace.remove(programObject->first);
            mProgramMap.erase(programObject);
        }
        else
        {
            programObject->second->flagForDeletion();
        }
    }
}

void ResourceManager::deleteTexture(GLuint texture)
{
    Texture *textureObject = mTextureNameSpace.remove(texture);

    if(textureObject)
    {
		textureObject->release();
    }
}

void ResourceManager::deleteRenderbuffer(GLuint renderbuffer)
{
    Renderbuffer *renderbufferObject = mRenderbufferNameSpace.remove(renderbuffer);

    if(renderbufferObject)
    {
		renderbufferObject->release();
    }
}

void ResourceManager::deleteSampler(GLuint sampler)
{
	Sampler *samplerObject = mSamplerNameSpace.remove(sampler);

	if(samplerObject)
	{
		samplerObject->release();
	}
}

void ResourceManager::deleteFenceSync(GLuint fenceSync)
{
	FenceSync *fenceObject = mFenceSyncNameSpace.remove(fenceSync);

	if(fenceObject)
	{
		fenceObject->release();
	}
}

Buffer *ResourceManager::getBuffer(unsigned int handle)
{
    return mBufferNameSpace.find(handle);
}

Shader *ResourceManager::getShader(unsigned int handle)
{
    ShaderMap::iterator shader = mShaderMap.find(handle);

    if(shader == mShaderMap.end())
    {
        return nullptr;
    }
    else
    {
        return shader->second;
    }
}

Texture *ResourceManager::getTexture(unsigned int handle)
{
    return mTextureNameSpace.find(handle);
}

Program *ResourceManager::getProgram(unsigned int handle)
{
    ProgramMap::iterator program = mProgramMap.find(handle);

    if(program == mProgramMap.end())
    {
        return nullptr;
    }
    else
    {
        return program->second;
    }
}

Renderbuffer *ResourceManager::getRenderbuffer(unsigned int handle)
{
    return mRenderbufferNameSpace.find(handle);
}

Sampler *ResourceManager::getSampler(unsigned int handle)
{
	return mSamplerNameSpace.find(handle);
}

FenceSync *ResourceManager::getFenceSync(unsigned int handle)
{
	return mFenceSyncNameSpace.find(handle);
}

void ResourceManager::checkBufferAllocation(unsigned int buffer)
{
    if(buffer != 0 && !getBuffer(buffer))
    {
        Buffer *bufferObject = new Buffer(buffer);
		bufferObject->addRef();

		mBufferNameSpace.insert(buffer, bufferObject);
    }
}

void ResourceManager::checkTextureAllocation(GLuint texture, TextureType type)
{
    if(!getTexture(texture) && texture != 0)
    {
        Texture *textureObject;

        if(type == TEXTURE_2D)
        {
            textureObject = new Texture2D(texture);
        }
        else if(type == TEXTURE_CUBE)
        {
            textureObject = new TextureCubeMap(texture);
        }
		else if(type == TEXTURE_EXTERNAL)
		{
			textureObject = new TextureExternal(texture);
		}
		else if(type == TEXTURE_3D)
		{
			textureObject = new Texture3D(texture);
		}
		else if(type == TEXTURE_2D_ARRAY)
		{
			textureObject = new Texture2DArray(texture);
		}
		else
        {
            UNREACHABLE(type);
            return;
        }

		textureObject->addRef();

		mTextureNameSpace.insert(texture, textureObject);
    }
}

void ResourceManager::checkRenderbufferAllocation(GLuint handle)
{
	if(handle != 0 && !getRenderbuffer(handle))
	{
		Renderbuffer *renderbufferObject = new Renderbuffer(handle, new Colorbuffer(0, 0, GL_RGBA4_OES, 0));
		renderbufferObject->addRef();

		mRenderbufferNameSpace.insert(handle, renderbufferObject);
	}
}

void ResourceManager::checkSamplerAllocation(GLuint sampler)
{
	if(sampler != 0 && !getSampler(sampler))
	{
		Sampler *samplerObject = new Sampler(sampler);
		samplerObject->addRef();

		mSamplerNameSpace.insert(sampler, samplerObject);
	}
}

bool ResourceManager::isSampler(GLuint sampler)
{
	return mSamplerNameSpace.find(sampler) != nullptr;
}

}
