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

// Renderbuffer.h: Defines the wrapper class Renderbuffer, as well as the
// class hierarchy used to store its contents: RenderbufferStorage, Colorbuffer,
// DepthStencilbuffer, Depthbuffer and Stencilbuffer. Implements GL renderbuffer
// objects and related functionality. [OpenGL ES 2.0.24] section 4.4.3 page 108.

#ifndef LIBGLESV2_RENDERBUFFER_H_
#define LIBGLESV2_RENDERBUFFER_H_

#include "RefCountObject.h"
#include "Image.hpp"
#include "common/angleutils.h"

#define GL_APICALL
#include <GLES2/gl2.h>

namespace gl
{
class Texture;
class Colorbuffer;
class DepthStencilbuffer;

// A class derived from RenderbufferStorage is created whenever glRenderbufferStorage
// is called. The specific concrete type depends on whether the internal format is
// colour depth, stencil or packed depth/stencil.
class RenderbufferStorage
{
  public:
    RenderbufferStorage();

    virtual ~RenderbufferStorage() = 0;

    virtual Colorbuffer *getColorbuffer();
    virtual DepthStencilbuffer *getDepthbuffer();
    virtual DepthStencilbuffer *getStencilbuffer();

    virtual Image *getRenderTarget();
    virtual Image *getDepthStencil();

    virtual GLsizei getWidth() const;
    virtual GLsizei getHeight() const;
    virtual GLenum getFormat() const;
    GLuint getRedSize() const;
    GLuint getGreenSize() const;
    GLuint getBlueSize() const;
    GLuint getAlphaSize() const;
    GLuint getDepthSize() const;
    GLuint getStencilSize() const;
    virtual GLsizei getSamples() const;

    virtual sw::Format getInternalFormat() const;

    unsigned int getSerial() const;

  protected:
    GLsizei mWidth;
    GLsizei mHeight;
    GLenum format;
    sw::Format internalFormat;
    GLsizei mSamples;

  private:
    DISALLOW_COPY_AND_ASSIGN(RenderbufferStorage);

    static unsigned int issueSerial();

    const unsigned int mSerial;

    static unsigned int mCurrentSerial;
};

// Renderbuffer implements the GL renderbuffer object.
// It's only a proxy for a RenderbufferStorage instance; the internal object
// can change whenever glRenderbufferStorage is called.
class Renderbuffer : public RefCountObject
{
  public:
    Renderbuffer(GLuint id, RenderbufferStorage *storage);

    ~Renderbuffer();

    Colorbuffer *getColorbuffer();
    DepthStencilbuffer *getDepthbuffer();
    DepthStencilbuffer *getStencilbuffer();

    Image *getRenderTarget();
    Image *getDepthStencil();

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

    unsigned int getSerial() const;

    void setStorage(RenderbufferStorage *newStorage);
    RenderbufferStorage *getStorage() { return mStorage; }

  private:
    DISALLOW_COPY_AND_ASSIGN(Renderbuffer);

    RenderbufferStorage *mStorage;
};

class Colorbuffer : public RenderbufferStorage
{
  public:
    explicit Colorbuffer(Image *renderTarget);
    Colorbuffer(Texture *texture, GLenum target);
    Colorbuffer(GLsizei width, GLsizei height, GLenum format, GLsizei samples);

    virtual ~Colorbuffer();

    virtual Colorbuffer *getColorbuffer();

    virtual Image *getRenderTarget();

    virtual GLsizei getWidth() const;
    virtual GLsizei getHeight() const;
    virtual GLenum getFormat() const;
    virtual GLenum getType() const;

    virtual sw::Format getInternalFormat() const;

  private:
    DISALLOW_COPY_AND_ASSIGN(Colorbuffer);

    Image *mRenderTarget;
    Texture *mTexture;
    GLenum mTarget;
};

class DepthStencilbuffer : public RenderbufferStorage
{
  public:
    explicit DepthStencilbuffer(Image *depthStencil);
    DepthStencilbuffer(GLsizei width, GLsizei height, GLsizei samples);

    ~DepthStencilbuffer();

    virtual DepthStencilbuffer *getDepthbuffer();
    virtual DepthStencilbuffer *getStencilbuffer();

    virtual Image *getDepthStencil();

  private:
    DISALLOW_COPY_AND_ASSIGN(DepthStencilbuffer);
    Image *mDepthStencil;
};

class Depthbuffer : public DepthStencilbuffer
{
  public:
    explicit Depthbuffer(Image *depthStencil);
    Depthbuffer(GLsizei width, GLsizei height, GLsizei samples);

    virtual ~Depthbuffer();

    virtual DepthStencilbuffer *getDepthbuffer();
    virtual DepthStencilbuffer *getStencilbuffer();

  private:
    DISALLOW_COPY_AND_ASSIGN(Depthbuffer);
};

class Stencilbuffer : public DepthStencilbuffer
{
  public:
    explicit Stencilbuffer(Image *depthStencil);
    Stencilbuffer(GLsizei width, GLsizei height, GLsizei samples);

    virtual ~Stencilbuffer();

    virtual DepthStencilbuffer *getDepthbuffer();
    virtual DepthStencilbuffer *getStencilbuffer();

  private:
    DISALLOW_COPY_AND_ASSIGN(Stencilbuffer);
};
}

#endif   // LIBGLESV2_RENDERBUFFER_H_
