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

// Display.cpp: Implements the Display class, representing the abstract
// display on which graphics are drawn.

#include "Display.h"

#include "main.h"
#include "mathutil.h"
#include "Device.hpp"
#include "common/debug.h"

#include <algorithm>
#include <vector>
#include <map>

namespace gl
{
typedef std::map<NativeDisplayType, Display*> DisplayMap; 
DisplayMap displays;

Display *Display::getDisplay(NativeDisplayType displayId)
{
    if(displays.find(displayId) != displays.end())
    {
        return displays[displayId];
    }

    // FIXME: Check if displayId is a valid display device context
    Display *display = new Display(displayId);

    displays[displayId] = display;
    return display;
}

Display::Display(NativeDisplayType displayId) : displayId(displayId)
{
    mMinSwapInterval = 1;
    mMaxSwapInterval = 0;
}

Display::~Display()
{
    terminate();

	displays.erase(displayId);
}

static void cpuid(int registers[4], int info)
{
	#if defined(_WIN32)
        __cpuid(registers, info);
    #else
        __asm volatile("cpuid": "=a" (registers[0]), "=b" (registers[1]), "=c" (registers[2]), "=d" (registers[3]): "a" (info));
    #endif
}

static bool detectSSE()
{
	#if defined(__APPLE__)
		int SSE = false;
		size_t length = sizeof(SSE);
		sysctlbyname("hw.optional.sse", &SSE, &length, 0, 0);
		return SSE;
	#else
		int registers[4];
		cpuid(registers, 1);
		return (registers[3] & 0x02000000) != 0;
	#endif
}

bool Display::initialize()
{
    if(isInitialized())
    {
        return true;
    }

	if(!detectSSE())
	{
		 return false;
	}
		
    mMinSwapInterval = 0;
    mMaxSwapInterval = 4;

    if(!isInitialized())
    {
        terminate();

        return false;
    }

    return true;
}

void Display::terminate()
{
    while(!mSurfaceSet.empty())
    {
        destroySurface(*mSurfaceSet.begin());
    }

    while(!mContextSet.empty())
    {
        destroyContext(*mContextSet.begin());
    }
}

gl::Context *Display::createContext(const gl::Context *shareContext)
{
    gl::Context *context = new gl::Context(shareContext);
    mContextSet.insert(context);

    return context;
}

void Display::destroySurface(Surface *surface)
{
    delete surface;
    mSurfaceSet.erase(surface);
}

void Display::destroyContext(gl::Context *context)
{
    delete context;

	if(context == gl::getContext())
	{
		gl::makeCurrent(NULL, NULL, NULL);
	}

    mContextSet.erase(context);
}

bool Display::isInitialized() const
{
    return mMinSwapInterval <= mMaxSwapInterval;
}

bool Display::isValidContext(gl::Context *context)
{
    return mContextSet.find(context) != mContextSet.end();
}

bool Display::isValidSurface(Surface *surface)
{
    return mSurfaceSet.find(surface) != mSurfaceSet.end();
}

bool Display::isValidWindow(NativeWindowType window)
{
	#if defined(_WIN32)
		return IsWindow(window) == TRUE;
	#else
		XWindowAttributes windowAttributes;
		Status status = XGetWindowAttributes(displayId, window, &windowAttributes);
			
		return status == True;
	#endif
}

GLint Display::getMinSwapInterval()
{
    return mMinSwapInterval;
}

GLint Display::getMaxSwapInterval()
{
    return mMaxSwapInterval;
}

Surface *Display::getPrimarySurface()
{
    if(mSurfaceSet.size() == 0)
	{
		Surface *surface = new Surface(this, WindowFromDC(displayId));

		if(!surface->initialize())
		{
			delete surface;
			return 0;
		}

		mSurfaceSet.insert(surface);

		gl::setCurrentDrawSurface(surface);
	}

    return *mSurfaceSet.begin();
}

NativeDisplayType Display::getNativeDisplay() const
{
	return displayId;
}

DisplayMode Display::getDisplayMode() const
{
	DisplayMode displayMode = {0};

	#if defined(_WIN32)
		HDC deviceContext = GetDC(0);
	
		displayMode.width = ::GetDeviceCaps(deviceContext, HORZRES);
		displayMode.height = ::GetDeviceCaps(deviceContext, VERTRES);
		unsigned int bpp = ::GetDeviceCaps(deviceContext, BITSPIXEL);
	
		switch(bpp)
		{
		case 32: displayMode.format = sw::FORMAT_X8R8G8B8; break;
		case 24: displayMode.format = sw::FORMAT_R8G8B8;   break;
		case 16: displayMode.format = sw::FORMAT_R5G6B5;   break;
		default:
			ASSERT(false);   // Unexpected display mode color depth
		}
	
		ReleaseDC(0, deviceContext);
	#else
		Screen *screen = XDefaultScreenOfDisplay(displayId);
		displayMode.width = XWidthOfScreen(screen);
		displayMode.height = XHeightOfScreen(screen);
		unsigned int bpp = XPlanesOfScreen(screen);

		switch(bpp)
		{
		case 32: displayMode.format = sw::FORMAT_X8R8G8B8; break;
		case 24: displayMode.format = sw::FORMAT_R8G8B8;   break;
		case 16: displayMode.format = sw::FORMAT_R5G6B5;   break;
		default:
			ASSERT(false);   // Unexpected display mode color depth
		}
	#endif

	return displayMode;
}

}
