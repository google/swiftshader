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
#include "Renderbuffer.h"
#include "Texture.h"

namespace es1
{
ResourceManager::ResourceManager()
{
    mRefCount = 1;
}

ResourceManager::~ResourceManager()
{
    while(!mBufferMap.empty())
    {
        deleteBuffer(mBufferMap.begin()->first);
    }

    while(!mRenderbufferMap.empty())
    {
        deleteRenderbuffer(mRenderbufferMap.begin()->first);
    }

    while(!mTextureMap.empty())
    {
        deleteTexture(mTextureMap.begin()->first);
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
    GLuint handle = mBufferNameSpace.allocate();

    mBufferMap[handle] = nullptr;

    return handle;
}

// Returns an unused texture name
GLuint ResourceManager::createTexture()
{
    GLuint handle = mTextureNameSpace.allocate();

    mTextureMap[handle] = nullptr;

    return handle;
}

// Returns an unused renderbuffer name
GLuint ResourceManager::createRenderbuffer()
{
    GLuint handle = mRenderbufferNameSpace.allocate();

    mRenderbufferMap[handle] = nullptr;

    return handle;
}

void ResourceManager::deleteBuffer(GLuint buffer)
{
    BufferMap::iterator bufferObject = mBufferMap.find(buffer);

    if(bufferObject != mBufferMap.end())
    {
        mBufferNameSpace.release(bufferObject->first);
        if(bufferObject->second) bufferObject->second->release();
        mBufferMap.erase(bufferObject);
    }
}

void ResourceManager::deleteTexture(GLuint texture)
{
    TextureMap::iterator textureObject = mTextureMap.find(texture);

    if(textureObject != mTextureMap.end())
    {
        mTextureNameSpace.release(textureObject->first);
        if(textureObject->second) textureObject->second->release();
        mTextureMap.erase(textureObject);
    }
}

void ResourceManager::deleteRenderbuffer(GLuint renderbuffer)
{
    RenderbufferMap::iterator renderbufferObject = mRenderbufferMap.find(renderbuffer);

    if(renderbufferObject != mRenderbufferMap.end())
    {
        mRenderbufferNameSpace.release(renderbufferObject->first);
        if(renderbufferObject->second) renderbufferObject->second->release();
        mRenderbufferMap.erase(renderbufferObject);
    }
}

Buffer *ResourceManager::getBuffer(unsigned int handle)
{
    BufferMap::iterator buffer = mBufferMap.find(handle);

    if(buffer == mBufferMap.end())
    {
        return nullptr;
    }
    else
    {
        return buffer->second;
    }
}

Texture *ResourceManager::getTexture(unsigned int handle)
{
    if(handle == 0) return nullptr;

    TextureMap::iterator texture = mTextureMap.find(handle);

    if(texture == mTextureMap.end())
    {
        return nullptr;
    }
    else
    {
        return texture->second;
    }
}

Renderbuffer *ResourceManager::getRenderbuffer(unsigned int handle)
{
    RenderbufferMap::iterator renderbuffer = mRenderbufferMap.find(handle);

    if(renderbuffer == mRenderbufferMap.end())
    {
        return nullptr;
    }
    else
    {
        return renderbuffer->second;
    }
}

void ResourceManager::setRenderbuffer(GLuint handle, Renderbuffer *buffer)
{
    mRenderbufferMap[handle] = buffer;
}

void ResourceManager::checkBufferAllocation(unsigned int buffer)
{
    if(buffer != 0 && !getBuffer(buffer))
    {
        Buffer *bufferObject = new Buffer(buffer);
		bufferObject->addRef();

		mBufferNameSpace.insert(buffer);
        mBufferMap[buffer] = bufferObject;
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
        else if(type == TEXTURE_EXTERNAL)
        {
            textureObject = new TextureExternal(texture);
        }
        else
        {
            UNREACHABLE(type);
            return;
        }

		textureObject->addRef();

		mTextureNameSpace.insert(texture);
        mTextureMap[texture] = textureObject;
    }
}

void ResourceManager::checkRenderbufferAllocation(GLuint handle)
{
	if(handle != 0 && !getRenderbuffer(handle))
	{
		Renderbuffer *renderbufferObject = new Renderbuffer(handle, new Colorbuffer(0, 0, GL_RGBA4_OES, 0));
		renderbufferObject->addRef();

		mRenderbufferNameSpace.insert(handle);
		mRenderbufferMap[handle] = renderbufferObject;
	}
}

}
