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
#include "common/Object.hpp"

#include <EGL/egl.h>

namespace egl
{
class Display;
class Config;
class Texture;
class Image;

class Surface : public gl::Object
{
public:
	virtual bool initialize();
    virtual void swap() = 0;

    virtual egl::Image *getRenderTarget();
    virtual egl::Image *getDepthStencil();

	void setSwapBehavior(EGLenum swapBehavior);
    void setSwapInterval(EGLint interval);

    virtual EGLint getConfigID() const;
	virtual EGLenum getSurfaceType() const;
	virtual sw::Format getInternalFormat() const;

    virtual EGLint getWidth() const;
    virtual EGLint getHeight() const;
    virtual EGLint getPixelAspectRatio() const;
    virtual EGLenum getRenderBuffer() const;
    virtual EGLenum getSwapBehavior() const;
    virtual EGLenum getTextureFormat() const;
    virtual EGLenum getTextureTarget() const;
	virtual EGLBoolean getLargestPBuffer() const;
	virtual EGLNativeWindowType getWindowHandle() const = 0;

    virtual void setBoundTexture(egl::Texture *texture);
    virtual egl::Texture *getBoundTexture() const;

	virtual bool isWindowSurface() const { return false; }
	virtual bool isPBufferSurface() const { return false; }

protected:
	Surface(const Display *display, const Config *config);

	virtual ~Surface();

    virtual void deleteResources();

    const Display *const display;
    Image *depthStencil;
	Image *backBuffer;
	Texture *texture;

	bool reset(int backbufferWidth, int backbufferHeight);

	const Config *const config;    // EGL config surface was created with
	EGLint height;                 // Height of surface
	EGLint width;                  // Width of surface
//  EGLint horizontalResolution;   // Horizontal dot pitch
//  EGLint verticalResolution;     // Vertical dot pitch
	EGLBoolean largestPBuffer;     // If true, create largest pbuffer possible
//  EGLBoolean mipmapTexture;      // True if texture has mipmaps
//  EGLint mipmapLevel;            // Mipmap level to render to
//  EGLenum multisampleResolve;    // Multisample resolve behavior
	EGLint pixelAspectRatio;       // Display aspect ratio
	EGLenum renderBuffer;          // Render buffer
	EGLenum swapBehavior;          // Buffer swap behavior
	EGLenum textureFormat;         // Format of texture: RGB, RGBA, or no texture
	EGLenum textureTarget;         // Type of texture: 2D or no texture
//  EGLenum vgAlphaFormat;         // Alpha format for OpenVG
//  EGLenum vgColorSpace;          // Color space for OpenVG
	EGLint swapInterval;
};

class WindowSurface : public Surface
{
public:
	WindowSurface(Display *display, const egl::Config *config, EGLNativeWindowType window);
	~WindowSurface() override;

	bool initialize() override;

	bool isWindowSurface() const override { return true; }
	void swap() override;

	EGLNativeWindowType getWindowHandle() const override;

private:
	void deleteResources() override;
	bool checkForResize();
	bool reset(int backBufferWidth, int backBufferHeight);

	const EGLNativeWindowType window;
	sw::FrameBuffer *frameBuffer;
};

class PBufferSurface : public Surface
{
public:
	PBufferSurface(Display *display, const egl::Config *config, EGLint width, EGLint height, EGLenum textureFormat, EGLenum textureTarget, EGLBoolean largestPBuffer);
	~PBufferSurface() override;

	bool isPBufferSurface() const override { return true; }
	void swap() override;

	EGLNativeWindowType getWindowHandle() const override;

private:
	void deleteResources() override;
};
}

#endif   // INCLUDE_SURFACE_H_
