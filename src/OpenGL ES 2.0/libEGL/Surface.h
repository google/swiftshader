//
// Copyright (c) 2002-2010 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//

// Surface.h: Defines the egl::Surface class, representing a drawing surface
// such as the client area of a window, including any back buffers.
// Implements EGLSurface and related functionality. [EGL 1.4] section 2.2 page 3.

#ifndef INCLUDE_SURFACE_H_
#define INCLUDE_SURFACE_H_

#include "libGLESv2/Image.hpp"
#include "libGLESv2/Texture.h"
#include "common/angleutils.h"
#include "Main/FrameBuffer.hpp"

#define EGLAPI
#include <EGL/egl.h>

namespace gl
{
class Image;
class Texture2D;
}

namespace egl
{
class Display;
class Config;

class Surface
{
public:
    Surface(Display *display, const egl::Config *config, HWND window);
    Surface(Display *display, const egl::Config *config, EGLint width, EGLint height, EGLenum textureFormat, EGLenum textureTarget);

    ~Surface();

	bool initialize();
    void swap();

	HWND getWindowHandle();
    virtual EGLint getWidth() const;
    virtual EGLint getHeight() const;

    virtual gl::Image *getRenderTarget();
    virtual gl::Image *getDepthStencil();

    void setSwapInterval(EGLint interval);
    bool checkForResize();   // Returns true if surface changed due to resize or interval update

    virtual EGLenum getTextureFormat() const;
    virtual EGLenum getTextureTarget() const;
    virtual sw::Format getInternalFormat() const;

    virtual void setBoundTexture(gl::Texture2D *texture);
    virtual gl::Texture2D *getBoundTexture() const;

private:
    void release();
    bool reset();

    Display *const mDisplay;
    gl::Image *mDepthStencil;
	sw::FrameBuffer *frameBuffer;
	gl::Image *backBuffer;

    void subclassWindow();
    void unsubclassWindow();
    bool reset(int backbufferWidth, int backbufferHeight);

    const HWND mWindow;            // Window that the surface is created for.
    bool mWindowSubclassed;        // Indicates whether we successfully subclassed mWindow for WM_RESIZE hooking
    const egl::Config *mConfig;    // EGL config surface was created with
    EGLint mHeight;                // Height of surface
    EGLint mWidth;                 // Width of surface
//  EGLint horizontalResolution;   // Horizontal dot pitch
//  EGLint verticalResolution;     // Vertical dot pitch
//  EGLBoolean largestPBuffer;     // If true, create largest pbuffer possible
//  EGLBoolean mipmapTexture;      // True if texture has mipmaps
//  EGLint mipmapLevel;            // Mipmap level to render to
//  EGLenum multisampleResolve;    // Multisample resolve behavior
    EGLint mPixelAspectRatio;      // Display aspect ratio
    EGLenum mRenderBuffer;         // Render buffer
    EGLenum mSwapBehavior;         // Buffer swap behavior
    EGLenum mTextureFormat;        // Format of texture: RGB, RGBA, or no texture
    EGLenum mTextureTarget;        // Type of texture: 2D or no texture
//  EGLenum vgAlphaFormat;         // Alpha format for OpenVG
//  EGLenum vgColorSpace;          // Color space for OpenVG
    EGLint mSwapInterval;
    gl::Texture2D *mTexture;
};
}

#endif   // INCLUDE_SURFACE_H_
