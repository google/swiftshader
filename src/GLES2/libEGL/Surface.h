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

// Surface.h: Defines the egl::Surface class, representing a drawing surface
// such as the client area of a window, including any back buffers.
// Implements EGLSurface and related functionality. [EGL 1.4] section 2.2 page 3.

#ifndef INCLUDE_SURFACE_H_
#define INCLUDE_SURFACE_H_

#include "Main/FrameBuffer.hpp"

#define EGLAPI
#include <EGL/egl.h>

namespace gl
{
class Image;
}

namespace egl
{
class Display;
class Config;
class Texture2D;

class Surface
{
public:
    Surface(Display *display, const egl::Config *config, EGLNativeWindowType window);
    Surface(Display *display, const egl::Config *config, EGLint width, EGLint height, EGLenum textureFormat, EGLenum textureTarget);

    virtual ~Surface();

	bool initialize();
    void swap();

	EGLNativeWindowType getWindowHandle();
    
    virtual gl::Image *getRenderTarget();
    virtual gl::Image *getDepthStencil();

    void setSwapInterval(EGLint interval);

    virtual EGLint getConfigID() const;
    virtual EGLint getWidth() const;
    virtual EGLint getHeight() const;
    virtual EGLint getPixelAspectRatio() const;
    virtual EGLenum getRenderBuffer() const;
    virtual EGLenum getSwapBehavior() const;
    virtual EGLenum getTextureFormat() const;
    virtual EGLenum getTextureTarget() const;
    virtual sw::Format getInternalFormat() const;

    virtual void setBoundTexture(egl::Texture2D *texture);
    virtual egl::Texture2D *getBoundTexture() const;

	bool checkForResize();   // Returns true if surface changed due to resize

private:
    void release();
    bool reset();

    Display *const mDisplay;
    gl::Image *mDepthStencil;
	sw::FrameBuffer *frameBuffer;
	gl::Image *backBuffer;
	egl::Texture2D *mTexture;

	bool reset(int backbufferWidth, int backbufferHeight);
    
    const EGLNativeWindowType mWindow;   // Window that the surface is created for.
    bool mWindowSubclassed;              // Indicates whether we successfully subclassed mWindow for WM_RESIZE hooking
    const egl::Config *mConfig;          // EGL config surface was created with
    EGLint mHeight;                      // Height of surface
    EGLint mWidth;                       // Width of surface
//  EGLint horizontalResolution;         // Horizontal dot pitch
//  EGLint verticalResolution;           // Vertical dot pitch
//  EGLBoolean largestPBuffer;           // If true, create largest pbuffer possible
//  EGLBoolean mipmapTexture;            // True if texture has mipmaps
//  EGLint mipmapLevel;                  // Mipmap level to render to
//  EGLenum multisampleResolve;          // Multisample resolve behavior
    EGLint mPixelAspectRatio;            // Display aspect ratio
    EGLenum mRenderBuffer;               // Render buffer
    EGLenum mSwapBehavior;               // Buffer swap behavior
    EGLenum mTextureFormat;              // Format of texture: RGB, RGBA, or no texture
    EGLenum mTextureTarget;              // Type of texture: 2D or no texture
//  EGLenum vgAlphaFormat;               // Alpha format for OpenVG
//  EGLenum vgColorSpace;                // Color space for OpenVG
    EGLint mSwapInterval;
};
}

#endif   // INCLUDE_SURFACE_H_
