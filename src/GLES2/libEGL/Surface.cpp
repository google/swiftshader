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

// Surface.cpp: Implements the egl::Surface class, representing a drawing surface
// such as the client area of a window, including any back buffers.
// Implements EGLSurface and related functionality. [EGL 1.4] section 2.2 page 3.

#include "Surface.h"

#include "main.h"
#include "Display.h"
#include "libGLESv2/Texture.h"
#include "libGLESv2/Device.hpp"
#include "common/debug.h"
#include "Main/FrameBuffer.hpp"

#if defined(_WIN32)
#include <tchar.h>
#endif

#include <algorithm>

namespace egl
{

Surface::Surface(Display *display, const Config *config, EGLNativeWindowType window)
    : mDisplay(display), mConfig(config), mWindow(window)
{
    frameBuffer = 0;
	backBuffer = 0;

    mDepthStencil = NULL;
    mTexture = NULL;
    mTextureFormat = EGL_NO_TEXTURE;
    mTextureTarget = EGL_NO_TEXTURE;

    mPixelAspectRatio = (EGLint)(1.0 * EGL_DISPLAY_SCALING);   // FIXME: Determine actual pixel aspect ratio
    mRenderBuffer = EGL_BACK_BUFFER;
    mSwapBehavior = EGL_BUFFER_PRESERVED;
    mSwapInterval = -1;
    setSwapInterval(1);
}

Surface::Surface(Display *display, const Config *config, EGLint width, EGLint height, EGLenum textureFormat, EGLenum textureType)
    : mDisplay(display), mWindow(NULL), mConfig(config), mWidth(width), mHeight(height)
{
	frameBuffer = 0;
	backBuffer = 0;

    mDepthStencil = NULL;
    mWindowSubclassed = false;
    mTexture = NULL;
    mTextureFormat = textureFormat;
    mTextureTarget = textureType;

    mPixelAspectRatio = (EGLint)(1.0 * EGL_DISPLAY_SCALING);   // FIXME: Determine actual pixel aspect ratio
    mRenderBuffer = EGL_BACK_BUFFER;
    mSwapBehavior = EGL_BUFFER_PRESERVED;
    mSwapInterval = -1;
    setSwapInterval(1);
}

Surface::~Surface()
{
    release();
}

bool Surface::initialize()
{
    ASSERT(!frameBuffer && !backBuffer && !mDepthStencil);

    return reset();
}

void Surface::release()
{	
    if(mDepthStencil)
    {
        mDepthStencil->release();
        mDepthStencil = NULL;
    }

    if(mTexture)
    {
        mTexture->releaseTexImage();
        mTexture = NULL;
    }

	if(backBuffer)
	{
		backBuffer->release();
		backBuffer = 0;
	}

	delete frameBuffer;
	frameBuffer = 0;
}

bool Surface::reset()
{
    if(!mWindow)
    {
        return reset(mWidth, mHeight);
    }

	// FIXME: Wrap into an abstract Window class
	#if defined(_WIN32)
		RECT windowRect;
		GetClientRect(mWindow, &windowRect);

		return reset(windowRect.right - windowRect.left, windowRect.bottom - windowRect.top);
	#else
		XWindowAttributes windowAttributes;
		XGetWindowAttributes(mDisplay->getNativeDisplay(), mWindow, &windowAttributes);
		
		return reset(windowAttributes.width, windowAttributes.height);
	#endif
}

bool Surface::reset(int backBufferWidth, int backBufferHeight)
{
    gl::Device *device = mDisplay->getDevice();

    if(device == NULL)
    {
        return false;
    }

    release();

    if(mWindow)
    {
		frameBuffer = gl::createFrameBuffer(mDisplay->getNativeDisplay(), mWindow, backBufferWidth, backBufferHeight);

		if(!frameBuffer)
		{
			ERR("Could not create frame buffer");
			release();
			return error(EGL_BAD_ALLOC, false);
		}
    }

	backBuffer = gl::createBackBuffer(backBufferWidth, backBufferHeight, mConfig);
	
    if(!backBuffer)
    {
        ERR("Could not create back buffer");
        release();
        return error(EGL_BAD_ALLOC, false);
    }

    if(mConfig->mDepthStencilFormat != sw::FORMAT_NULL)
    {
        mDepthStencil = device->createDepthStencilSurface(backBufferWidth, backBufferHeight, mConfig->mDepthStencilFormat, 1, false);

		if(!mDepthStencil)
		{
			ERR("Could not create depth/stencil buffer for surface");
			release();
			return error(EGL_BAD_ALLOC, false);
		}
    }

    mWidth = backBufferWidth;
    mHeight = backBufferHeight;

    return true;
}

EGLNativeWindowType Surface::getWindowHandle()
{
    return mWindow;
}

void Surface::swap()
{
    #if PERF_PROFILE
		profiler.nextFrame();
	#endif

	if(backBuffer)
    {
		void *source = backBuffer->lockInternal(0, 0, 0, sw::LOCK_READONLY, sw::PUBLIC);
		frameBuffer->flip(source, backBuffer->getInternalFormat());
		backBuffer->unlockInternal();

        checkForResize();
	}
}

gl::Image *Surface::getRenderTarget()
{
    if(backBuffer)
    {
        backBuffer->addRef();
    }

    return backBuffer;
}

gl::Image *Surface::getDepthStencil()
{
    if(mDepthStencil)
    {
        mDepthStencil->addRef();
    }

    return mDepthStencil;
}

void Surface::setSwapInterval(EGLint interval)
{
    if(mSwapInterval == interval)
    {
        return;
    }
    
    mSwapInterval = interval;
    mSwapInterval = std::max(mSwapInterval, mDisplay->getMinSwapInterval());
    mSwapInterval = std::min(mSwapInterval, mDisplay->getMaxSwapInterval());
}

EGLint Surface::getConfigID() const
{
    return mConfig->mConfigID;
}

EGLint Surface::getWidth() const
{
    return mWidth;
}

EGLint Surface::getHeight() const
{
    return mHeight;
}

EGLint Surface::getPixelAspectRatio() const
{
    return mPixelAspectRatio;
}

EGLenum Surface::getRenderBuffer() const
{
    return mRenderBuffer;
}

EGLenum Surface::getSwapBehavior() const
{
    return mSwapBehavior;
}

EGLenum Surface::getTextureFormat() const
{
    return mTextureFormat;
}

EGLenum Surface::getTextureTarget() const
{
    return mTextureTarget;
}

void Surface::setBoundTexture(gl::Texture2D *texture)
{
    mTexture = texture;
}

gl::Texture2D *Surface::getBoundTexture() const
{
    return mTexture;
}

sw::Format Surface::getInternalFormat() const
{
    return mConfig->mRenderTargetFormat;
}

bool Surface::checkForResize()
{
    #if defined(_WIN32)
		RECT client;
		if(!GetClientRect(mWindow, &client))
		{
			ASSERT(false);
			return false;
		}

		int clientWidth = client.right - client.left;
		int clientHeight = client.bottom - client.top;
	#else
		XWindowAttributes windowAttributes;
		XGetWindowAttributes(mDisplay->getNativeDisplay(), mWindow, &windowAttributes);

		int clientWidth = windowAttributes.width;
		int clientHeight = windowAttributes.height;
	#endif

	bool sizeDirty = clientWidth != getWidth() || clientHeight != getHeight();

    if(sizeDirty)
    {
        reset(clientWidth, clientHeight);

        if(static_cast<egl::Surface*>(getCurrentDrawSurface()) == this)
        {
            gl::makeCurrent(static_cast<gl::Context*>(getCurrentContext()), static_cast<egl::Display*>(getCurrentDisplay()), this);
        }

        return true;
    }

    return false;
}
}
