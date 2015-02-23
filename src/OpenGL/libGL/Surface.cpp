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

// Surface.cpp: Implements the Surface class, representing a drawing surface
// such as the client area of a window, including any back buffers.

#include "Surface.h"

#include "main.h"
#include "Display.h"
#include "Image.hpp"
#include "Context.h"
#include "common/debug.h"
#include "Main/FrameBuffer.hpp"

#if defined(_WIN32)
#include <tchar.h>
#endif

#include <algorithm>

namespace gl
{

Surface::Surface(Display *display, NativeWindowType window)
    : mDisplay(display), mWindow(window)
{
    frameBuffer = 0;
	backBuffer = 0;

    mDepthStencil = NULL;
    mTextureFormat = GL_NONE;
    mTextureTarget = GL_NONE;

    mSwapInterval = -1;
    setSwapInterval(1);
}

Surface::Surface(Display *display, GLint width, GLint height, GLenum textureFormat, GLenum textureType)
    : mDisplay(display), mWindow(NULL), mWidth(width), mHeight(height)
{
	frameBuffer = 0;
	backBuffer = 0;

    mDepthStencil = NULL;
    mWindowSubclassed = false;
    mTextureFormat = textureFormat;
    mTextureTarget = textureType;

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
    release();

    if(mWindow)
    {
		frameBuffer = ::createFrameBuffer(mDisplay->getNativeDisplay(), mWindow, backBufferWidth, backBufferHeight);

		if(!frameBuffer)
		{
			ERR("Could not create frame buffer");
			release();
			return error(GL_OUT_OF_MEMORY, false);
		}
    }

	backBuffer = new Image(0, backBufferWidth, backBufferHeight, GL_RGB, GL_UNSIGNED_BYTE);

    if(!backBuffer)
    {
        ERR("Could not create back buffer");
        release();
        return error(GL_OUT_OF_MEMORY, false);
    }

    if(true)   // Always provide a depth/stencil buffer
    {
        mDepthStencil = new Image(0, backBufferWidth, backBufferHeight, sw::FORMAT_D24S8, 1, false, true);

		if(!mDepthStencil)
		{
			ERR("Could not create depth/stencil buffer for surface");
			release();
			return error(GL_OUT_OF_MEMORY, false);
		}
    }

    mWidth = backBufferWidth;
    mHeight = backBufferHeight;

    return true;
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

Image *Surface::getRenderTarget()
{
    if(backBuffer)
    {
        backBuffer->addRef();
    }

    return backBuffer;
}

Image *Surface::getDepthStencil()
{
    if(mDepthStencil)
    {
        mDepthStencil->addRef();
    }

    return mDepthStencil;
}

void Surface::setSwapInterval(GLint interval)
{
    if(mSwapInterval == interval)
    {
        return;
    }
    
    mSwapInterval = interval;
    mSwapInterval = std::max(mSwapInterval, mDisplay->getMinSwapInterval());
    mSwapInterval = std::min(mSwapInterval, mDisplay->getMaxSwapInterval());
}

GLint Surface::getWidth() const
{
    return mWidth;
}

GLint Surface::getHeight() const
{
    return mHeight;
}

GLenum Surface::getTextureFormat() const
{
    return mTextureFormat;
}

GLenum Surface::getTextureTarget() const
{
    return mTextureTarget;
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

        if(getCurrentDrawSurface() == this)
        {
			getContext()->makeCurrent(this);
        }

        return true;
    }

    return false;
}
}
