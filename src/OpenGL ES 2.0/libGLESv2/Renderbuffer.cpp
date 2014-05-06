//
// Copyright (c) 2002-2010 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//

// Renderbuffer.cpp: the Renderbuffer class and its derived classes
// Colorbuffer, Depthbuffer and Stencilbuffer. Implements GL renderbuffer
// objects and related functionality. [OpenGL ES 2.0.24] section 4.4.3 page 108.

#include "Renderbuffer.h"

#include "main.h"
#include "Texture.h"
#include "utilities.h"

namespace gl
{
unsigned int RenderbufferStorage::mCurrentSerial = 1;

Renderbuffer::Renderbuffer(GLuint id, RenderbufferStorage *storage) : RefCountObject(id)
{
    ASSERT(storage != NULL);
    mStorage = storage;
}

Renderbuffer::~Renderbuffer()
{
    delete mStorage;
}

bool Renderbuffer::isColorbuffer() const
{
    return mStorage->isColorbuffer();
}

bool Renderbuffer::isDepthbuffer() const
{
    return mStorage->isDepthbuffer();
}

bool Renderbuffer::isStencilbuffer() const
{
    return mStorage->isStencilbuffer();
}

Image *Renderbuffer::getRenderTarget()
{
    return mStorage->getRenderTarget();
}

Image *Renderbuffer::getDepthStencil()
{
    return mStorage->getDepthStencil();
}

GLsizei Renderbuffer::getWidth() const
{
    return mStorage->getWidth();
}

GLsizei Renderbuffer::getHeight() const
{
    return mStorage->getHeight();
}

GLenum Renderbuffer::getFormat() const
{
    return mStorage->getFormat();
}

sw::Format Renderbuffer::getInternalFormat() const
{
    return mStorage->getInternalFormat();
}

GLuint Renderbuffer::getRedSize() const
{
    return mStorage->getRedSize();
}

GLuint Renderbuffer::getGreenSize() const
{
    return mStorage->getGreenSize();
}

GLuint Renderbuffer::getBlueSize() const
{
    return mStorage->getBlueSize();
}

GLuint Renderbuffer::getAlphaSize() const
{
    return mStorage->getAlphaSize();
}

GLuint Renderbuffer::getDepthSize() const
{
    return mStorage->getDepthSize();
}

GLuint Renderbuffer::getStencilSize() const
{
    return mStorage->getStencilSize();
}

GLsizei Renderbuffer::getSamples() const
{
    return mStorage->getSamples();
}

unsigned int Renderbuffer::getSerial() const
{
    return mStorage->getSerial();
}

void Renderbuffer::setStorage(RenderbufferStorage *newStorage)
{
    ASSERT(newStorage != NULL);

    delete mStorage;
    mStorage = newStorage;
}

RenderbufferStorage::RenderbufferStorage() : mSerial(issueSerial())
{
    mWidth = 0;
    mHeight = 0;
    format = GL_RGBA4;
    internalFormat = sw::FORMAT_A8R8G8B8;
    mSamples = 0;
}

RenderbufferStorage::~RenderbufferStorage()
{
}

bool RenderbufferStorage::isColorbuffer() const
{
    return false;
}

bool RenderbufferStorage::isDepthbuffer() const
{
    return false;
}

bool RenderbufferStorage::isStencilbuffer() const
{
    return false;
}

Image *RenderbufferStorage::getRenderTarget()
{
    return NULL;
}

Image *RenderbufferStorage::getDepthStencil()
{
    return NULL;
}

GLsizei RenderbufferStorage::getWidth() const
{
    return mWidth;
}

GLsizei RenderbufferStorage::getHeight() const
{
    return mHeight;
}

GLenum RenderbufferStorage::getFormat() const
{
    return format;
}

GLuint RenderbufferStorage::getRedSize() const
{
    return sw2es::GetRedSize(getInternalFormat());
}

GLuint RenderbufferStorage::getGreenSize() const
{
    return sw2es::GetGreenSize(getInternalFormat());
}

GLuint RenderbufferStorage::getBlueSize() const
{
    return sw2es::GetBlueSize(getInternalFormat());
}

GLuint RenderbufferStorage::getAlphaSize() const
{
    return sw2es::GetAlphaSize(getInternalFormat());
}

GLuint RenderbufferStorage::getDepthSize() const
{
    return sw2es::GetDepthSize(getInternalFormat());
}

GLuint RenderbufferStorage::getStencilSize() const
{
    return sw2es::GetStencilSize(getInternalFormat());
}

GLsizei RenderbufferStorage::getSamples() const
{
    return mSamples;
}

sw::Format RenderbufferStorage::getInternalFormat() const
{
    return internalFormat;
}

unsigned int RenderbufferStorage::getSerial() const
{
    return mSerial;
}

unsigned int RenderbufferStorage::issueSerial()
{
    return mCurrentSerial++;
}

Colorbuffer::Colorbuffer(Image *renderTarget) : mRenderTarget(renderTarget), mTexture(NULL)
{
    if(renderTarget)
    {
        renderTarget->addRef();
		
        mWidth = renderTarget->getWidth();
        mHeight = renderTarget->getHeight();
		internalFormat = renderTarget->getInternalFormat();
        format = sw2es::ConvertBackBufferFormat(internalFormat);
        mSamples = renderTarget->getMultiSampleDepth();
    }
}

Colorbuffer::Colorbuffer(Texture *texture, GLenum target) : mRenderTarget(NULL), mTexture(texture), mTarget(target)
{
    if(texture)
    {
        mWidth = texture->getWidth();
        mHeight = texture->getHeight();
        internalFormat = texture->getInternalFormat();
        format = texture->getFormat();
        mSamples = 0;

        mRenderTarget = texture->getRenderTarget(target);
    }
}

Colorbuffer::Colorbuffer(int width, int height, GLenum format, GLsizei samples) : mRenderTarget(NULL), mTexture(NULL)
{
    Device *device = getDevice();

    sw::Format requestedFormat = es2sw::ConvertRenderbufferFormat(format);
    int supportedSamples = getContext()->getNearestSupportedSamples(requestedFormat, samples);

    if(supportedSamples == -1)
    {
        error(GL_OUT_OF_MEMORY);

        return;
    }

    if(width > 0 && height > 0)
    {
        mRenderTarget = device->createRenderTarget(width, height, requestedFormat, supportedSamples, false);

        if(!mRenderTarget)
        {
            error(GL_OUT_OF_MEMORY);
			return;
        }
    }

    mWidth = width;
    mHeight = height;
    this->format = format;
    internalFormat = requestedFormat;
    mSamples = supportedSamples;
}

Colorbuffer::~Colorbuffer()
{
    if(mRenderTarget)
    {
        mRenderTarget->release();
    }
}

GLsizei Colorbuffer::getWidth() const
{
    if(mTexture)
    {
        return mTexture->getWidth();
    }

    return mWidth;
}

GLsizei Colorbuffer::getHeight() const
{
    if(mTexture)
    {
        return mTexture->getHeight();
    }

    return mHeight;
}

sw::Format Colorbuffer::getInternalFormat() const
{
    if(mTexture)
    {
        return mTexture->getInternalFormat();
    }

    return internalFormat;
}

GLenum Colorbuffer::getType() const
{
    if(mTexture)
    {
        return mTexture->getType();
    }

    return GL_UNSIGNED_BYTE;
}

GLenum Colorbuffer::getFormat() const
{
    if(mTexture)
    {
        return mTexture->getFormat();
    }

    return format;
}

bool Colorbuffer::isColorbuffer() const
{
    return true;
}

Image *Colorbuffer::getRenderTarget()
{
    if(mTexture)
    {
        if(mRenderTarget)
        {
            mRenderTarget->release();
        }

        mRenderTarget = mTexture->getRenderTarget(mTarget);
    }

    return mRenderTarget;
}

DepthStencilbuffer::DepthStencilbuffer(Image *depthStencil) : mDepthStencil(depthStencil)
{
    if(depthStencil)
    {
        depthStencil->addRef();

		mWidth = depthStencil->getWidth();
        mHeight = depthStencil->getHeight();
		internalFormat = depthStencil->getInternalFormat();
		format = sw2es::ConvertDepthStencilFormat(internalFormat);
		mSamples = depthStencil->getMultiSampleDepth();
    }
}

DepthStencilbuffer::DepthStencilbuffer(int width, int height, GLsizei samples)
{
    Device *device = getDevice();

    mDepthStencil = NULL;
    
    int supportedSamples = getContext()->getNearestSupportedSamples(sw::FORMAT_D24S8, samples);

    if(supportedSamples == -1)
    {
        error(GL_OUT_OF_MEMORY);

        return;
    }

    if(width > 0 && height > 0)
    {
        mDepthStencil = device->createDepthStencilSurface(width, height, sw::FORMAT_D24S8, supportedSamples, false);

        if(!mDepthStencil)
        {
            error(GL_OUT_OF_MEMORY);
            return;
        }
    }

    mWidth = width;
    mHeight = height;
    format = GL_DEPTH24_STENCIL8_OES;
    internalFormat = sw::FORMAT_D24S8;
    mSamples = supportedSamples;
}

DepthStencilbuffer::~DepthStencilbuffer()
{
    if(mDepthStencil)
    {
        mDepthStencil->release();
    }
}

bool DepthStencilbuffer::isDepthbuffer() const
{
    return true;
}

bool DepthStencilbuffer::isStencilbuffer() const
{
    return true;
}

Image *DepthStencilbuffer::getDepthStencil()
{
    return mDepthStencil;
}

Depthbuffer::Depthbuffer(Image *depthStencil) : DepthStencilbuffer(depthStencil)
{
    if(depthStencil)
    {
        format = GL_DEPTH_COMPONENT16;   // If the renderbuffer parameters are queried, the calling function
                                         // will expect one of the valid renderbuffer formats for use in 
                                         // glRenderbufferStorage
    }
}

Depthbuffer::Depthbuffer(int width, int height, GLsizei samples) : DepthStencilbuffer(width, height, samples)
{
    if(getDepthStencil())
    {
        format = GL_DEPTH_COMPONENT16;   // If the renderbuffer parameters are queried, the calling function
                                         // will expect one of the valid renderbuffer formats for use in 
                                         // glRenderbufferStorage
    }
}

Depthbuffer::~Depthbuffer()
{
}

bool Depthbuffer::isDepthbuffer() const
{
    return true;
}

bool Depthbuffer::isStencilbuffer() const
{
    return false;
}

Stencilbuffer::Stencilbuffer(Image *depthStencil) : DepthStencilbuffer(depthStencil)
{
    if(depthStencil)
    {
        format = GL_STENCIL_INDEX8;   // If the renderbuffer parameters are queried, the calling function
                                      // will expect one of the valid renderbuffer formats for use in 
                                      // glRenderbufferStorage
    }
}

Stencilbuffer::Stencilbuffer(int width, int height, GLsizei samples) : DepthStencilbuffer(width, height, samples)
{
    if(getDepthStencil())
    {
        format = GL_STENCIL_INDEX8;   // If the renderbuffer parameters are queried, the calling function
                                      // will expect one of the valid renderbuffer formats for use in 
                                      // glRenderbufferStorage
    }
}

Stencilbuffer::~Stencilbuffer()
{
}

bool Stencilbuffer::isDepthbuffer() const
{
    return false;
}

bool Stencilbuffer::isStencilbuffer() const
{
    return true;
}
}
