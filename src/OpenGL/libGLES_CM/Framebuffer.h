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

// Framebuffer.h: Defines the Framebuffer class. Implements GL framebuffer
// objects and related functionality. [OpenGL ES 2.0.24] section 4.4 page 105.

#ifndef LIBGLES_CM_FRAMEBUFFER_H_
#define LIBGLES_CM_FRAMEBUFFER_H_

#include "common/Object.hpp"
#include "common/Image.hpp"

#include <GLES/gl.h>

namespace es1
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

    egl::Image *getRenderTarget();
    egl::Image *getDepthBuffer();
	egl::Image *getStencilBuffer();

    Renderbuffer *getColorbuffer();
    Renderbuffer *getDepthbuffer();
    Renderbuffer *getStencilbuffer();

    GLenum getColorbufferType();
    GLenum getDepthbufferType();
    GLenum getStencilbufferType();

    GLuint getColorbufferName();
    GLuint getDepthbufferName();
    GLuint getStencilbufferName();

    bool hasStencil();

	virtual GLenum completeness();
	GLenum completeness(int &width, int &height, int &samples);

	GLenum getImplementationColorReadFormat();
	GLenum getImplementationColorReadType();

protected:
    GLenum mColorbufferType;
    gl::BindingPointer<Renderbuffer> mColorbufferPointer;

    GLenum mDepthbufferType;
    gl::BindingPointer<Renderbuffer> mDepthbufferPointer;

    GLenum mStencilbufferType;
    gl::BindingPointer<Renderbuffer> mStencilbufferPointer;

private:
    Renderbuffer *lookupRenderbuffer(GLenum type, GLuint handle) const;
};

class DefaultFramebuffer : public Framebuffer
{
public:
    DefaultFramebuffer(Colorbuffer *colorbuffer, DepthStencilbuffer *depthStencil);
};

}

#endif   // LIBGLES_CM_FRAMEBUFFER_H_
