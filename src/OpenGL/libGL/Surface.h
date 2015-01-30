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

// Surface.h: Defines the Surface class, representing a drawing surface
// such as the client area of a window, including any back buffers.

#ifndef INCLUDE_SURFACE_H_
#define INCLUDE_SURFACE_H_

#include "Main/FrameBuffer.hpp"

#define _GDI32_
#include <windows.h>
#include <GL/GL.h>
#define GL_GLEXT_PROTOTYPES
#include <GL/glext.h>

#if defined(_WIN32)
typedef HDC     NativeDisplayType;
typedef HBITMAP NativePixmapType;
typedef HWND    NativeWindowType;
#else
#error
#endif

namespace gl
{
class Image;
class Display;

class Surface
{
public:
    Surface(Display *display, NativeWindowType window);
    Surface(Display *display, GLint width, GLint height, GLenum textureFormat, GLenum textureTarget);

    virtual ~Surface();

	bool initialize();
    void swap();
    
    virtual Image *getRenderTarget();
    virtual Image *getDepthStencil();

    void setSwapInterval(GLint interval);

    virtual GLint getWidth() const;
    virtual GLint getHeight() const;
    virtual GLenum getTextureFormat() const;
    virtual GLenum getTextureTarget() const;

	bool checkForResize();   // Returns true if surface changed due to resize

private:
    void release();
    bool reset();

    Display *const mDisplay;
    Image *mDepthStencil;
	sw::FrameBuffer *frameBuffer;
	Image *backBuffer;

	bool reset(int backbufferWidth, int backbufferHeight);
    
    const NativeWindowType mWindow;   // Window that the surface is created for.
    bool mWindowSubclassed;           // Indicates whether we successfully subclassed mWindow for WM_RESIZE hooking
    GLint mHeight;                    // Height of surface
    GLint mWidth;                     // Width of surface
    GLenum mTextureFormat;            // Format of texture: RGB, RGBA, or no texture
    GLenum mTextureTarget;            // Type of texture: 2D or no texture
    GLint mSwapInterval;
};
}

#endif   // INCLUDE_SURFACE_H_
