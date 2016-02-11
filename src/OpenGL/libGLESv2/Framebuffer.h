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

#ifndef LIBGLESV2_FRAMEBUFFER_H_
#define LIBGLESV2_FRAMEBUFFER_H_

#include "common/Object.hpp"
#include "common/Image.hpp"

#include <GLES2/gl2.h>

namespace es2
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

    void setColorbuffer(GLenum type, GLuint colorbuffer, GLuint index, GLint level = 0, GLint layer = 0);
    void setDepthbuffer(GLenum type, GLuint depthbuffer, GLint level = 0, GLint layer = 0);
    void setStencilbuffer(GLenum type, GLuint stencilbuffer, GLint level = 0, GLint layer = 0);

	void setReadBuffer(GLenum buf);
	void setDrawBuffer(GLuint index, GLenum buf);
	GLenum getReadBuffer() const;
	GLenum getDrawBuffer(GLuint index) const;

    void detachTexture(GLuint texture);
    void detachRenderbuffer(GLuint renderbuffer);

    egl::Image *getRenderTarget(GLuint index);
    egl::Image *getReadRenderTarget();
    egl::Image *getDepthStencil();

    Renderbuffer *getColorbuffer(GLuint index);
    Renderbuffer *getReadColorbuffer();
    Renderbuffer *getDepthbuffer();
    Renderbuffer *getStencilbuffer();

    GLenum getColorbufferType(GLuint index);
    GLenum getDepthbufferType();
    GLenum getStencilbufferType();

    GLuint getColorbufferName(GLuint index);
    GLuint getDepthbufferName();
    GLuint getStencilbufferName();

	GLint getColorbufferLayer(GLuint index);
	GLint getDepthbufferLayer();
	GLint getStencilbufferLayer();

    bool hasStencil();

	GLenum completeness();
	GLenum completeness(int &width, int &height, int &samples);

	GLenum getImplementationColorReadFormat();
	GLenum getImplementationColorReadType();

	virtual bool isDefaultFramebuffer() const { return false; }

	static bool IsRenderbuffer(GLenum type);

protected:
    GLenum mColorbufferType[MAX_COLOR_ATTACHMENTS];
    gl::BindingPointer<Renderbuffer> mColorbufferPointer[MAX_COLOR_ATTACHMENTS];
	GLenum readBuffer;
	GLenum drawBuffer[MAX_COLOR_ATTACHMENTS];

    GLenum mDepthbufferType;
    gl::BindingPointer<Renderbuffer> mDepthbufferPointer;

    GLenum mStencilbufferType;
    gl::BindingPointer<Renderbuffer> mStencilbufferPointer;

private:
    Renderbuffer *lookupRenderbuffer(GLenum type, GLuint handle, GLint level, GLint layer) const;
};

class DefaultFramebuffer : public Framebuffer
{
public:
    DefaultFramebuffer(Colorbuffer *colorbuffer, DepthStencilbuffer *depthStencil);

	bool isDefaultFramebuffer() const override { return true; }
};

}

#endif   // LIBGLESV2_FRAMEBUFFER_H_
