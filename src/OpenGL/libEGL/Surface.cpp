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
#include "Texture.hpp"
#include "Image.hpp"
#include "Context.hpp"
#include "common/debug.h"
#include "Main/FrameBuffer.hpp"

#if defined(__unix__) && !defined(__ANDROID__)
#include "Main/libX11.hpp"
#endif

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
    deleteResources();
}

bool Surface::initialize()
{
    ASSERT(!frameBuffer && !backBuffer && !mDepthStencil);

    return reset();
}

void Surface::deleteResources()
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
	#elif defined(__ANDROID__)
		return reset(ANativeWindow_getWidth(mWindow), ANativeWindow_getHeight(mWindow));
	#else
		XWindowAttributes windowAttributes;
		libX11->XGetWindowAttributes(mDisplay->getNativeDisplay(), mWindow, &windowAttributes);

		return reset(windowAttributes.width, windowAttributes.height);
	#endif
}

bool Surface::reset(int backBufferWidth, int backBufferHeight)
{
    deleteResources();

    if(mWindow)
    {
		if(libGLES_CM)
		{
			frameBuffer = libGLES_CM->createFrameBuffer(mDisplay->getNativeDisplay(), mWindow, backBufferWidth, backBufferHeight);
		}
		else if(libGLESv2)
		{
			frameBuffer = libGLESv2->createFrameBuffer(mDisplay->getNativeDisplay(), mWindow, backBufferWidth, backBufferHeight);
		}

		if(!frameBuffer)
		{
			ERR("Could not create frame buffer");
			deleteResources();
			return error(EGL_BAD_ALLOC, false);
		}
    }

	if(libGLES_CM)
	{
		backBuffer = libGLES_CM->createBackBuffer(backBufferWidth, backBufferHeight, mConfig);
	}
	else if(libGLESv2)
	{
		backBuffer = libGLESv2->createBackBuffer(backBufferWidth, backBufferHeight, mConfig);
	}

    if(!backBuffer)
    {
        ERR("Could not create back buffer");
        deleteResources();
        return error(EGL_BAD_ALLOC, false);
    }

    if(mConfig->mDepthStencilFormat != sw::FORMAT_NULL)
    {

		if(libGLES_CM)
		{
			mDepthStencil = libGLES_CM->createDepthStencil(backBufferWidth, backBufferHeight, mConfig->mDepthStencilFormat, 1, false);
		}
		else if(libGLESv2)
		{
			mDepthStencil = libGLESv2->createDepthStencil(backBufferWidth, backBufferHeight, mConfig->mDepthStencilFormat, 1, false);
		}

		if(!mDepthStencil)
		{
			ERR("Could not create depth/stencil buffer for surface");
			deleteResources();
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
	if(backBuffer)
    {
		void *source = backBuffer->lockInternal(0, 0, 0, sw::LOCK_READONLY, sw::PUBLIC);
		frameBuffer->flip(source, backBuffer->getInternalFormat());
		backBuffer->unlockInternal();

        checkForResize();
	}
}

egl::Image *Surface::getRenderTarget()
{
    if(backBuffer)
    {
        backBuffer->addRef();
    }

    return backBuffer;
}

egl::Image *Surface::getDepthStencil()
{
    if(mDepthStencil)
    {
        mDepthStencil->addRef();
    }

    return mDepthStencil;
}

void Surface::setSwapBehavior(EGLenum swapBehavior)
{
	mSwapBehavior = swapBehavior;
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

EGLenum Surface::getSurfaceType() const
{
    return mConfig->mSurfaceType;
}

sw::Format Surface::getInternalFormat() const
{
    return mConfig->mRenderTargetFormat;
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

void Surface::setBoundTexture(egl::Texture *texture)
{
    mTexture = texture;
}

egl::Texture *Surface::getBoundTexture() const
{
    return mTexture;
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
	#elif defined(__ANDROID__)
		int clientWidth = ANativeWindow_getWidth(mWindow);
		int clientHeight = ANativeWindow_getHeight(mWindow);
	#else
		XWindowAttributes windowAttributes;
		libX11->XGetWindowAttributes(mDisplay->getNativeDisplay(), mWindow, &windowAttributes);

		int clientWidth = windowAttributes.width;
		int clientHeight = windowAttributes.height;
	#endif

	bool sizeDirty = clientWidth != getWidth() || clientHeight != getHeight();

    if(sizeDirty)
    {
        reset(clientWidth, clientHeight);

        if(static_cast<egl::Surface*>(getCurrentDrawSurface()) == this)
        {
			static_cast<egl::Context*>(getCurrentContext())->makeCurrent(this);
        }

        return true;
    }

    return false;
}
}
