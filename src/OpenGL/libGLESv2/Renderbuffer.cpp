// SwiftShader Software Renderer
//
// Copyright(c) 2005-2013 TransGaming Inc.
//
// All rights reserved. No part of this software may be copied, distributed, transmitted,
// transcribed, stored in a retrieval system, translated into any human or computer
// language by any means, or disclosed to third parties without the explicit written
// agreement of TransGaming Inc. Without such an agreement, no rights or licenses, express
// or implied, including but not limited to any patent rights, are granted to you.
//

// Renderbuffer.cpp: the Renderbuffer class and its derived classes
// Colorbuffer, Depthbuffer and Stencilbuffer. Implements GL renderbuffer
// objects and related functionality. [OpenGL ES 2.0.24] section 4.4.3 page 108.

#include "Renderbuffer.h"

#include "main.h"
#include "Texture.h"
#include "utilities.h"

namespace es2
{
RenderbufferInterface::RenderbufferInterface()
{
}

// The default case for classes inherited from RenderbufferInterface is not to
// need to do anything upon the reference count to the parent Renderbuffer incrementing
// or decrementing. 
void RenderbufferInterface::addProxyRef(const Renderbuffer *proxy)
{
}

void RenderbufferInterface::releaseProxy(const Renderbuffer *proxy)
{
}

GLuint RenderbufferInterface::getRedSize() const
{
	return sw2es::GetRedSize(getInternalFormat());
}

GLuint RenderbufferInterface::getGreenSize() const
{
	return sw2es::GetGreenSize(getInternalFormat());
}

GLuint RenderbufferInterface::getBlueSize() const
{
	return sw2es::GetBlueSize(getInternalFormat());
}

GLuint RenderbufferInterface::getAlphaSize() const
{
	return sw2es::GetAlphaSize(getInternalFormat());
}

GLuint RenderbufferInterface::getDepthSize() const
{
	return sw2es::GetDepthSize(getInternalFormat());
}

GLuint RenderbufferInterface::getStencilSize() const
{
	return sw2es::GetStencilSize(getInternalFormat());
}

///// RenderbufferTexture2D Implementation ////////

RenderbufferTexture2D::RenderbufferTexture2D(Texture2D *texture, GLint level) : mLevel(level)
{
	mTexture2D = texture;
}

RenderbufferTexture2D::~RenderbufferTexture2D()
{
	mTexture2D = NULL;
}

// Textures need to maintain their own reference count for references via
// Renderbuffers acting as proxies. Here, we notify the texture of a reference.
void RenderbufferTexture2D::addProxyRef(const Renderbuffer *proxy)
{
	mTexture2D->addProxyRef(proxy);
}

void RenderbufferTexture2D::releaseProxy(const Renderbuffer *proxy)
{
	mTexture2D->releaseProxy(proxy);
}

// Increments refcount on image.
// caller must release() the returned image
egl::Image *RenderbufferTexture2D::getRenderTarget()
{
	return mTexture2D->getRenderTarget(GL_TEXTURE_2D, mLevel);
}

// Increments refcount on image.
// caller must release() the returned image
egl::Image *RenderbufferTexture2D::createSharedImage()
{
	return mTexture2D->createSharedImage(GL_TEXTURE_2D, mLevel);
}

bool RenderbufferTexture2D::isShared() const
{
	return mTexture2D->isShared(GL_TEXTURE_2D, mLevel);
}

GLsizei RenderbufferTexture2D::getWidth() const
{
	return mTexture2D->getWidth(GL_TEXTURE_2D, mLevel);
}

GLsizei RenderbufferTexture2D::getHeight() const
{
	return mTexture2D->getHeight(GL_TEXTURE_2D, mLevel);
}

GLenum RenderbufferTexture2D::getFormat() const
{
	return mTexture2D->getFormat(GL_TEXTURE_2D, mLevel);
}

sw::Format RenderbufferTexture2D::getInternalFormat() const
{
	return mTexture2D->getInternalFormat(GL_TEXTURE_2D, mLevel);
}

GLsizei RenderbufferTexture2D::getSamples() const
{
	return 0;
}

///// RenderbufferTexture3D Implementation ////////

RenderbufferTexture3D::RenderbufferTexture3D(Texture3D *texture, GLint level, GLint layer) : mLevel(level), mLayer(layer)
{
	mTexture3D = texture;
	if(mLayer != 0)
	{
		UNIMPLEMENTED();
	}
}

RenderbufferTexture3D::~RenderbufferTexture3D()
{
	mTexture3D = NULL;
}

// Textures need to maintain their own reference count for references via
// Renderbuffers acting as proxies. Here, we notify the texture of a reference.
void RenderbufferTexture3D::addProxyRef(const Renderbuffer *proxy)
{
	mTexture3D->addProxyRef(proxy);
}

void RenderbufferTexture3D::releaseProxy(const Renderbuffer *proxy)
{
	mTexture3D->releaseProxy(proxy);
}

// Increments refcount on image.
// caller must release() the returned image
egl::Image *RenderbufferTexture3D::getRenderTarget()
{
	return mTexture3D->getRenderTarget(mTexture3D->getTarget(), mLevel);
}

// Increments refcount on image.
// caller must release() the returned image
egl::Image *RenderbufferTexture3D::createSharedImage()
{
	return mTexture3D->createSharedImage(mTexture3D->getTarget(), mLevel);
}

bool RenderbufferTexture3D::isShared() const
{
	return mTexture3D->isShared(mTexture3D->getTarget(), mLevel);
}

GLsizei RenderbufferTexture3D::getWidth() const
{
	return mTexture3D->getWidth(mTexture3D->getTarget(), mLevel);
}

GLsizei RenderbufferTexture3D::getHeight() const
{
	return mTexture3D->getHeight(mTexture3D->getTarget(), mLevel);
}

GLsizei RenderbufferTexture3D::getDepth() const
{
	return mTexture3D->getDepth(mTexture3D->getTarget(), mLevel);
}

GLenum RenderbufferTexture3D::getFormat() const
{
	return mTexture3D->getFormat(mTexture3D->getTarget(), mLevel);
}

sw::Format RenderbufferTexture3D::getInternalFormat() const
{
	return mTexture3D->getInternalFormat(mTexture3D->getTarget(), mLevel);
}

GLsizei RenderbufferTexture3D::getSamples() const
{
	return 0;
}

///// RenderbufferTextureCubeMap Implementation ////////

RenderbufferTextureCubeMap::RenderbufferTextureCubeMap(TextureCubeMap *texture, GLenum target, GLint level) : mTarget(target), mLevel(level)
{
	mTextureCubeMap = texture;
}

RenderbufferTextureCubeMap::~RenderbufferTextureCubeMap()
{
	mTextureCubeMap = NULL;
}

// Textures need to maintain their own reference count for references via
// Renderbuffers acting as proxies. Here, we notify the texture of a reference.
void RenderbufferTextureCubeMap::addProxyRef(const Renderbuffer *proxy)
{
    mTextureCubeMap->addProxyRef(proxy);
}

void RenderbufferTextureCubeMap::releaseProxy(const Renderbuffer *proxy)
{
    mTextureCubeMap->releaseProxy(proxy);
}

// Increments refcount on image.
// caller must release() the returned image
egl::Image *RenderbufferTextureCubeMap::getRenderTarget()
{
	return mTextureCubeMap->getRenderTarget(mTarget, mLevel);
}

// Increments refcount on image.
// caller must release() the returned image
egl::Image *RenderbufferTextureCubeMap::createSharedImage()
{
	return mTextureCubeMap->createSharedImage(mTarget, mLevel);
}

bool RenderbufferTextureCubeMap::isShared() const
{
	return mTextureCubeMap->isShared(mTarget, mLevel);
}

GLsizei RenderbufferTextureCubeMap::getWidth() const
{
	return mTextureCubeMap->getWidth(mTarget, mLevel);
}

GLsizei RenderbufferTextureCubeMap::getHeight() const
{
	return mTextureCubeMap->getHeight(mTarget, mLevel);
}

GLenum RenderbufferTextureCubeMap::getFormat() const
{
	return mTextureCubeMap->getFormat(mTarget, mLevel);
}

sw::Format RenderbufferTextureCubeMap::getInternalFormat() const
{
	return mTextureCubeMap->getInternalFormat(mTarget, mLevel);
}

GLsizei RenderbufferTextureCubeMap::getSamples() const
{
	return 0;
}

////// Renderbuffer Implementation //////

Renderbuffer::Renderbuffer(GLuint name, RenderbufferInterface *instance) : NamedObject(name)
{
	ASSERT(instance != NULL);
	mInstance = instance;
}

Renderbuffer::~Renderbuffer()
{
	delete mInstance;
}

// The RenderbufferInterface contained in this Renderbuffer may need to maintain
// its own reference count, so we pass it on here.
void Renderbuffer::addRef()
{
    mInstance->addProxyRef(this);

    Object::addRef();
}

void Renderbuffer::release()
{
    mInstance->releaseProxy(this);

    Object::release();
}

// Increments refcount on image.
// caller must Release() the returned image
egl::Image *Renderbuffer::getRenderTarget()
{
	return mInstance->getRenderTarget();
}

// Increments refcount on image.
// caller must Release() the returned image
egl::Image *Renderbuffer::createSharedImage()
{
    return mInstance->createSharedImage();
}

bool Renderbuffer::isShared() const
{
    return mInstance->isShared();
}

GLsizei Renderbuffer::getWidth() const
{
	return mInstance->getWidth();
}

GLsizei Renderbuffer::getHeight() const
{
	return mInstance->getHeight();
}

GLsizei Renderbuffer::getDepth() const
{
	return mInstance->getDepth();
}

GLint Renderbuffer::getLayer() const
{
	return mInstance->getLayer();
}

GLint Renderbuffer::getLevel() const
{
	return mInstance->getLevel();
}

GLenum Renderbuffer::getFormat() const
{
	return mInstance->getFormat();
}

sw::Format Renderbuffer::getInternalFormat() const
{
	return mInstance->getInternalFormat();
}

GLuint Renderbuffer::getRedSize() const
{
	return mInstance->getRedSize();
}

GLuint Renderbuffer::getGreenSize() const
{
	return mInstance->getGreenSize();
}

GLuint Renderbuffer::getBlueSize() const
{
	return mInstance->getBlueSize();
}

GLuint Renderbuffer::getAlphaSize() const
{
	return mInstance->getAlphaSize();
}

GLuint Renderbuffer::getDepthSize() const
{
	return mInstance->getDepthSize();
}

GLuint Renderbuffer::getStencilSize() const
{
	return mInstance->getStencilSize();
}

GLsizei Renderbuffer::getSamples() const
{
	return mInstance->getSamples();
}

void Renderbuffer::setLayer(GLint layer)
{
	return mInstance->setLayer(layer);
}

void Renderbuffer::setLevel(GLint level)
{
	return mInstance->setLevel(level);
}

void Renderbuffer::setStorage(RenderbufferStorage *newStorage)
{
	ASSERT(newStorage != NULL);

	delete mInstance;
	mInstance = newStorage;
}

RenderbufferStorage::RenderbufferStorage()
{
	mWidth = 0;
	mHeight = 0;
	format = GL_RGBA4;
	internalFormat = sw::FORMAT_A8B8G8R8;
	mSamples = 0;
}

RenderbufferStorage::~RenderbufferStorage()
{
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

sw::Format RenderbufferStorage::getInternalFormat() const
{
	return internalFormat;
}

GLsizei RenderbufferStorage::getSamples() const
{
	return mSamples;
}

Colorbuffer::Colorbuffer(egl::Image *renderTarget) : mRenderTarget(renderTarget)
{
	if(renderTarget)
	{
		renderTarget->addRef();
		
		mWidth = renderTarget->getWidth();
		mHeight = renderTarget->getHeight();
		internalFormat = renderTarget->getInternalFormat();
		format = sw2es::ConvertBackBufferFormat(internalFormat);
		mSamples = renderTarget->getDepth() & ~1;
	}
}

Colorbuffer::Colorbuffer(int width, int height, GLenum format, GLsizei samples) : mRenderTarget(nullptr)
{
	Device *device = getDevice();

	sw::Format requestedFormat = es2sw::ConvertRenderbufferFormat(format);
	int supportedSamples = Context::getSupportedMultisampleCount(samples);

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

// Increments refcount on image.
// caller must release() the returned image
egl::Image *Colorbuffer::getRenderTarget()
{
	if(mRenderTarget)
	{
		mRenderTarget->addRef();
	}

	return mRenderTarget;
}

// Increments refcount on image.
// caller must release() the returned image
egl::Image *Colorbuffer::createSharedImage()
{
    if(mRenderTarget)
    {
        mRenderTarget->addRef();
        mRenderTarget->markShared();
    }

    return mRenderTarget;
}

bool Colorbuffer::isShared() const
{
    return mRenderTarget->isShared();
}

DepthStencilbuffer::DepthStencilbuffer(egl::Image *depthStencil) : mDepthStencil(depthStencil)
{
	if(depthStencil)
	{
		depthStencil->addRef();

		mWidth = depthStencil->getWidth();
		mHeight = depthStencil->getHeight();
		internalFormat = depthStencil->getInternalFormat();
		format = sw2es::ConvertDepthStencilFormat(internalFormat);
		mSamples = depthStencil->getDepth() & ~1;
	}
}

DepthStencilbuffer::DepthStencilbuffer(int width, int height, GLenum requestedFormat, GLsizei samples) : mDepthStencil(nullptr)
{
	format = requestedFormat;
	switch(requestedFormat)
	{
	case GL_STENCIL_INDEX8:
	case GL_DEPTH_COMPONENT24:
	case GL_DEPTH24_STENCIL8_OES:
		internalFormat = sw::FORMAT_D24S8;
		break;
	case GL_DEPTH32F_STENCIL8:
		internalFormat = sw::FORMAT_D32FS8_TEXTURE;
		break;
	case GL_DEPTH_COMPONENT16:
		internalFormat = sw::FORMAT_D16;
		break;
	case GL_DEPTH_COMPONENT32_OES:
		internalFormat = sw::FORMAT_D32;
		break;
	case GL_DEPTH_COMPONENT32F:
		internalFormat = sw::FORMAT_D32F;
		break;
	default:
		UNREACHABLE(requestedFormat);
		format = GL_DEPTH24_STENCIL8_OES;
		internalFormat = sw::FORMAT_D24S8;
	}

	Device *device = getDevice();

	int supportedSamples = Context::getSupportedMultisampleCount(samples);

	if(width > 0 && height > 0)
	{
		mDepthStencil = device->createDepthStencilSurface(width, height, internalFormat, supportedSamples, false);

		if(!mDepthStencil)
		{
			error(GL_OUT_OF_MEMORY);
			return;
		}
	}

	mWidth = width;
	mHeight = height;
	mSamples = supportedSamples;
}

DepthStencilbuffer::~DepthStencilbuffer()
{
	if(mDepthStencil)
	{
		mDepthStencil->release();
	}
}

// Increments refcount on image.
// caller must release() the returned image
egl::Image *DepthStencilbuffer::getRenderTarget()
{
	if(mDepthStencil)
	{
		mDepthStencil->addRef();
	}

	return mDepthStencil;
}

// Increments refcount on image.
// caller must release() the returned image
egl::Image *DepthStencilbuffer::createSharedImage()
{
    if(mDepthStencil)
    {
        mDepthStencil->addRef();
        mDepthStencil->markShared();
    }

    return mDepthStencil;
}

bool DepthStencilbuffer::isShared() const
{
    return mDepthStencil->isShared();
}

Depthbuffer::Depthbuffer(egl::Image *depthStencil) : DepthStencilbuffer(depthStencil)
{
}

Depthbuffer::Depthbuffer(int width, int height, GLenum format, GLsizei samples) : DepthStencilbuffer(width, height, format, samples)
{
}

Depthbuffer::~Depthbuffer()
{
}

Stencilbuffer::Stencilbuffer(egl::Image *depthStencil) : DepthStencilbuffer(depthStencil)
{
}

Stencilbuffer::Stencilbuffer(int width, int height, GLsizei samples) : DepthStencilbuffer(width, height, GL_STENCIL_INDEX8, samples)
{
}

Stencilbuffer::~Stencilbuffer()
{
}

}
