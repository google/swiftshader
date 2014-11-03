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

// Renderbuffer.h: Defines the wrapper class Renderbuffer, as well as the
// class hierarchy used to store its contents: RenderbufferStorage, Colorbuffer,
// DepthStencilbuffer, Depthbuffer and Stencilbuffer. Implements GL renderbuffer
// objects and related functionality. [OpenGL ES 2.0.24] section 4.4.3 page 108.

#ifndef LIBGLESV2_RENDERBUFFER_H_
#define LIBGLESV2_RENDERBUFFER_H_

#include "RefCountObject.h"
#include "Image.hpp"

#define GL_APICALL
#include <GLES2/gl2.h>

namespace rad
{
class Texture2D;
class TextureCubeMap;
class Renderbuffer;
class Colorbuffer;
class DepthStencilbuffer;

class RenderbufferInterface
{
public:
	RenderbufferInterface();

	virtual ~RenderbufferInterface() {};

	virtual void addProxyRef(const Renderbuffer *proxy);
    virtual void releaseProxy(const Renderbuffer *proxy);

	virtual egl::Image *getRenderTarget() = 0;
    virtual egl::Image *createSharedImage() = 0;
    virtual bool isShared() const = 0;

	virtual GLsizei getWidth() const = 0;
	virtual GLsizei getHeight() const = 0;
	virtual GLenum getFormat() const = 0;
	virtual sw::Format getInternalFormat() const = 0;
	virtual GLsizei getSamples() const = 0;

	GLuint getRedSize() const;
	GLuint getGreenSize() const;
	GLuint getBlueSize() const;
	GLuint getAlphaSize() const;
	GLuint getDepthSize() const;
	GLuint getStencilSize() const;
};

class RenderbufferTexture2D : public RenderbufferInterface
{
public:
	RenderbufferTexture2D(Texture2D *texture);

	virtual ~RenderbufferTexture2D();

	virtual void addProxyRef(const Renderbuffer *proxy);
    virtual void releaseProxy(const Renderbuffer *proxy);

	virtual egl::Image *getRenderTarget();
    virtual egl::Image *createSharedImage();
    virtual bool isShared() const;

	virtual GLsizei getWidth() const;
	virtual GLsizei getHeight() const;
	virtual GLenum getFormat() const;
	virtual sw::Format getInternalFormat() const;
	virtual GLsizei getSamples() const;

private:
	BindingPointer<Texture2D> mTexture2D;
};

class RenderbufferTextureCubeMap : public RenderbufferInterface
{
public:
	RenderbufferTextureCubeMap(TextureCubeMap *texture, GLenum target);

	virtual ~RenderbufferTextureCubeMap();

	virtual void addProxyRef(const Renderbuffer *proxy);
    virtual void releaseProxy(const Renderbuffer *proxy);

	virtual Image *getRenderTarget();
    virtual egl::Image *createSharedImage();
    virtual bool isShared() const;

	virtual GLsizei getWidth() const;
	virtual GLsizei getHeight() const;
	virtual GLenum getFormat() const;
	virtual sw::Format getInternalFormat() const;
	virtual GLsizei getSamples() const;

private:
	BindingPointer<TextureCubeMap> mTextureCubeMap;
	GLenum mTarget;
};

// A class derived from RenderbufferStorage is created whenever glRenderbufferStorage
// is called. The specific concrete type depends on whether the internal format is
// colour depth, stencil or packed depth/stencil.
class RenderbufferStorage : public RenderbufferInterface
{
public:
	RenderbufferStorage();

	virtual ~RenderbufferStorage() = 0;

	virtual egl::Image *getRenderTarget() = 0;
    virtual egl::Image *createSharedImage() = 0;
    virtual bool isShared() const = 0;

	virtual GLsizei getWidth() const;
	virtual GLsizei getHeight() const;
	virtual GLenum getFormat() const;
	virtual sw::Format getInternalFormat() const;
	virtual GLsizei getSamples() const;

protected:
	GLsizei mWidth;
	GLsizei mHeight;
	GLenum format;
	sw::Format internalFormat;
	GLsizei mSamples;
};

// Renderbuffer implements the GL renderbuffer object.
// It's only a proxy for a RenderbufferInterface instance; the internal object
// can change whenever glRenderbufferStorage is called.
class Renderbuffer : public RefCountObject
{
public:
	Renderbuffer(GLuint id, RenderbufferInterface *storage);

	virtual ~Renderbuffer();

	// These functions from RefCountObject are overloaded here because
    // Textures need to maintain their own count of references to them via
    // Renderbuffers/RenderbufferTextures. These functions invoke those
    // reference counting functions on the RenderbufferInterface.
    virtual void addRef();
    virtual void release();

	egl::Image *getRenderTarget();
    virtual egl::Image *createSharedImage();
    virtual bool isShared() const;

	GLsizei getWidth() const;
	GLsizei getHeight() const;
	GLenum getFormat() const;
	sw::Format getInternalFormat() const;
	GLuint getRedSize() const;
	GLuint getGreenSize() const;
	GLuint getBlueSize() const;
	GLuint getAlphaSize() const;
	GLuint getDepthSize() const;
	GLuint getStencilSize() const;
	GLsizei getSamples() const;

	void setStorage(RenderbufferStorage *newStorage);

private:
	RenderbufferInterface *mInstance;
};

class Colorbuffer : public RenderbufferStorage
{
public:
	explicit Colorbuffer(egl::Image *renderTarget);
	Colorbuffer(GLsizei width, GLsizei height, GLenum format, GLsizei samples);

	virtual ~Colorbuffer();

	virtual egl::Image *getRenderTarget();
    virtual egl::Image *createSharedImage();
    virtual bool isShared() const;

private:
	egl::Image *mRenderTarget;
};

class DepthStencilbuffer : public RenderbufferStorage
{
public:
	explicit DepthStencilbuffer(egl::Image *depthStencil);
	DepthStencilbuffer(GLsizei width, GLsizei height, GLsizei samples);

	~DepthStencilbuffer();

	virtual egl::Image *getRenderTarget();
    virtual egl::Image *createSharedImage();
    virtual bool isShared() const;

protected:
	egl::Image *mDepthStencil;
};

class Depthbuffer : public DepthStencilbuffer
{
public:
	explicit Depthbuffer(egl::Image *depthStencil);
	Depthbuffer(GLsizei width, GLsizei height, GLsizei samples);

	virtual ~Depthbuffer();
};

class Stencilbuffer : public DepthStencilbuffer
{
public:
	explicit Stencilbuffer(egl::Image *depthStencil);
	Stencilbuffer(GLsizei width, GLsizei height, GLsizei samples);

	virtual ~Stencilbuffer();
};
}

#endif   // LIBGLESV2_RENDERBUFFER_H_
