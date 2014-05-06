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

// Framebuffer.h: Defines the Framebuffer class. Implements GL framebuffer
// objects and related functionality. [OpenGL ES 2.0.24] section 4.4 page 105.

#ifndef LIBGLESV2_FRAMEBUFFER_H_
#define LIBGLESV2_FRAMEBUFFER_H_

#include "RefCountObject.h"
#include "common/angleutils.h"
#include "Image.hpp"

#define GL_APICALL
#include <GLES2/gl2.h>

namespace gl
{
class Renderbuffer;
class Colorbuffer;
class Depthbuffer;
class Stencilbuffer;
class DepthStencilbuffer;

class Framebuffer
{
  public:
    Framebuffer();

    virtual ~Framebuffer();

    void setColorbuffer(GLenum type, GLuint colorbuffer);
    void setDepthbuffer(GLenum type, GLuint depthbuffer);
    void setStencilbuffer(GLenum type, GLuint stencilbuffer);

    void detachTexture(GLuint texture);
    void detachRenderbuffer(GLuint renderbuffer);

    Image *getRenderTarget();
    Image *getDepthStencil();

    Colorbuffer *getColorbuffer();
    DepthStencilbuffer *getDepthbuffer();
    DepthStencilbuffer *getStencilbuffer();

    GLenum getColorbufferType();
    GLenum getDepthbufferType();
    GLenum getStencilbufferType();

    GLuint getColorbufferHandle();
    GLuint getDepthbufferHandle();
    GLuint getStencilbufferHandle();

    bool hasStencil();
    int getSamples();

    virtual GLenum completeness();

  protected:
    GLenum mColorbufferType;
    BindingPointer<Renderbuffer> mColorbufferPointer;

    GLenum mDepthbufferType;
    BindingPointer<Renderbuffer> mDepthbufferPointer;

    GLenum mStencilbufferType;
    BindingPointer<Renderbuffer> mStencilbufferPointer;

  private:
    DISALLOW_COPY_AND_ASSIGN(Framebuffer);

    Renderbuffer *lookupRenderbuffer(GLenum type, GLuint handle) const;
};

class DefaultFramebuffer : public Framebuffer
{
  public:
    DefaultFramebuffer(Colorbuffer *color, DepthStencilbuffer *depthStencil);

    virtual GLenum completeness();

  private:
    DISALLOW_COPY_AND_ASSIGN(DefaultFramebuffer);
};

}

#endif   // LIBGLESV2_FRAMEBUFFER_H_
