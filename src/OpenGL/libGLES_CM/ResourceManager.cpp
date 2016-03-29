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
    while(!mBufferNameSpace.empty())
    {
        deleteBuffer(mBufferNameSpace.firstName());
    }

    while(!mRenderbufferNameSpace.empty())
    {
        deleteRenderbuffer(mRenderbufferNameSpace.firstName());
    }

    while(!mTextureNameSpace.empty())
    {
        deleteTexture(mTextureNameSpace.firstName());
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

void ResourceManager::deleteBuffer(GLuint buffer)
{
    Buffer *bufferObject = mBufferNameSpace.remove(buffer);

    if(bufferObject)
    {
		bufferObject->release();
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

Buffer *ResourceManager::getBuffer(unsigned int handle)
{
    return mBufferNameSpace.find(handle);
}

Texture *ResourceManager::getTexture(unsigned int handle)
{
    return mTextureNameSpace.find(handle);
}

Renderbuffer *ResourceManager::getRenderbuffer(unsigned int handle)
{
    return mRenderbufferNameSpace.find(handle);
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

}
