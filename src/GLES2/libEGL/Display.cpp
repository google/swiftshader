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

// Display.cpp: Implements the egl::Display class, representing the abstract
// display on which graphics are drawn. Implements EGLDisplay.
// [EGL 1.4] section 2.1.2 page 3.

#include "Display.h"

#include "main.h"
#include "libGLESv2/mathutil.h"
#include "libGLESv2/Device.hpp"
#include "common/debug.h"

#include <algorithm>
#include <vector>
#include <map>

namespace egl
{
typedef std::map<EGLNativeDisplayType, Display*> DisplayMap; 
DisplayMap displays;

egl::Display *Display::getDisplay(EGLNativeDisplayType displayId)
{
    if(displays.find(displayId) != displays.end())
    {
        return displays[displayId];
    }

    egl::Display *display = NULL;

    if(displayId == EGL_DEFAULT_DISPLAY)
    {
        display = new egl::Display(displayId);
    }
    else
    {
        // FIXME: Check if displayId is a valid display device context

        display = new egl::Display(displayId);
    }

    displays[displayId] = display;
    return display;
}

Display::Display(EGLNativeDisplayType displayId) : displayId(displayId)
{
    mDevice = NULL;

    mMinSwapInterval = 1;
    mMaxSwapInterval = 1;
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

    const sw::Format renderTargetFormats[] =
    {
        sw::FORMAT_A1R5G5B5,
    //  sw::FORMAT_A2R10G10B10,   // The color_ramp conformance test uses ReadPixels with UNSIGNED_BYTE causing it to think that rendering skipped a colour value.
        sw::FORMAT_A8R8G8B8,
        sw::FORMAT_R5G6B5,
    //  sw::FORMAT_X1R5G5B5,      // Has no compatible OpenGL ES renderbuffer format
        sw::FORMAT_X8R8G8B8
    };

    const sw::Format depthStencilFormats[] =
    {
        sw::FORMAT_NULL,
    //  sw::FORMAT_D16_LOCKABLE,
        sw::FORMAT_D32,
    //  sw::FORMAT_D15S1,
        sw::FORMAT_D24S8,
        sw::FORMAT_D24X8,
    //  sw::FORMAT_D24X4S4,
        sw::FORMAT_D16,
    //  sw::FORMAT_D32F_LOCKABLE,
    //  sw::FORMAT_D24FS8
    };

	DisplayMode currentDisplayMode = getDisplayMode();
    ConfigSet configSet;

    for(int formatIndex = 0; formatIndex < sizeof(renderTargetFormats) / sizeof(sw::Format); formatIndex++)
    {
        sw::Format renderTargetFormat = renderTargetFormats[formatIndex];

        for(int depthStencilIndex = 0; depthStencilIndex < sizeof(depthStencilFormats) / sizeof(sw::Format); depthStencilIndex++)
        {
            sw::Format depthStencilFormat = depthStencilFormats[depthStencilIndex];
             
            // FIXME: enumerate multi-sampling

            configSet.add(currentDisplayMode, mMinSwapInterval, mMaxSwapInterval, renderTargetFormat, depthStencilFormat, 0);
        }
    }

    // Give the sorted configs a unique ID and store them internally
    EGLint index = 1;
    for(ConfigSet::Iterator config = configSet.mSet.begin(); config != configSet.mSet.end(); config++)
    {
        Config configuration = *config;
        configuration.mConfigID = index;
        index++;

        mConfigSet.mSet.insert(configuration);
    }

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

    delete mDevice;
    mDevice = NULL;
}

bool Display::getConfigs(EGLConfig *configs, const EGLint *attribList, EGLint configSize, EGLint *numConfig)
{
    return mConfigSet.getConfigs(configs, attribList, configSize, numConfig);
}

bool Display::getConfigAttrib(EGLConfig config, EGLint attribute, EGLint *value)
{
    const egl::Config *configuration = mConfigSet.get(config);

    switch (attribute)
    {
      case EGL_BUFFER_SIZE:               *value = configuration->mBufferSize;             break;
      case EGL_ALPHA_SIZE:                *value = configuration->mAlphaSize;              break;
      case EGL_BLUE_SIZE:                 *value = configuration->mBlueSize;               break;
      case EGL_GREEN_SIZE:                *value = configuration->mGreenSize;              break;
      case EGL_RED_SIZE:                  *value = configuration->mRedSize;                break;
      case EGL_DEPTH_SIZE:                *value = configuration->mDepthSize;              break;
      case EGL_STENCIL_SIZE:              *value = configuration->mStencilSize;            break;
      case EGL_CONFIG_CAVEAT:             *value = configuration->mConfigCaveat;           break;
      case EGL_CONFIG_ID:                 *value = configuration->mConfigID;               break;
      case EGL_LEVEL:                     *value = configuration->mLevel;                  break;
      case EGL_NATIVE_RENDERABLE:         *value = configuration->mNativeRenderable;       break;
      case EGL_NATIVE_VISUAL_ID:          *value = configuration->mNativeVisualID;         break;
      case EGL_NATIVE_VISUAL_TYPE:        *value = configuration->mNativeVisualType;       break;
      case EGL_SAMPLES:                   *value = configuration->mSamples;                break;
      case EGL_SAMPLE_BUFFERS:            *value = configuration->mSampleBuffers;          break;
      case EGL_SURFACE_TYPE:              *value = configuration->mSurfaceType;            break;
      case EGL_TRANSPARENT_TYPE:          *value = configuration->mTransparentType;        break;
      case EGL_TRANSPARENT_BLUE_VALUE:    *value = configuration->mTransparentBlueValue;   break;
      case EGL_TRANSPARENT_GREEN_VALUE:   *value = configuration->mTransparentGreenValue;  break;
      case EGL_TRANSPARENT_RED_VALUE:     *value = configuration->mTransparentRedValue;    break;
      case EGL_BIND_TO_TEXTURE_RGB:       *value = configuration->mBindToTextureRGB;       break;
      case EGL_BIND_TO_TEXTURE_RGBA:      *value = configuration->mBindToTextureRGBA;      break;
      case EGL_MIN_SWAP_INTERVAL:         *value = configuration->mMinSwapInterval;        break;
      case EGL_MAX_SWAP_INTERVAL:         *value = configuration->mMaxSwapInterval;        break;
      case EGL_LUMINANCE_SIZE:            *value = configuration->mLuminanceSize;          break;
      case EGL_ALPHA_MASK_SIZE:           *value = configuration->mAlphaMaskSize;          break;
      case EGL_COLOR_BUFFER_TYPE:         *value = configuration->mColorBufferType;        break;
      case EGL_RENDERABLE_TYPE:           *value = configuration->mRenderableType;         break;
      case EGL_MATCH_NATIVE_PIXMAP:       *value = false; UNIMPLEMENTED();                 break;
      case EGL_CONFORMANT:                *value = configuration->mConformant;             break;
      case EGL_MAX_PBUFFER_WIDTH:         *value = configuration->mMaxPBufferWidth;        break;
      case EGL_MAX_PBUFFER_HEIGHT:        *value = configuration->mMaxPBufferHeight;       break;
      case EGL_MAX_PBUFFER_PIXELS:        *value = configuration->mMaxPBufferPixels;       break;
      default:
        return false;
    }

    return true;
}

bool Display::createDevice()
{
    mDevice = gl::createDevice();

    if(!mDevice)
    {
        return error(EGL_BAD_ALLOC, false);
    }

    // Permanent non-default states
	mDevice->setPointSpriteEnable(true);

    return true;
}

EGLSurface Display::createWindowSurface(EGLNativeWindowType window, EGLConfig config, const EGLint *attribList)
{
    const Config *configuration = mConfigSet.get(config);

    if(attribList)
    {
        while(*attribList != EGL_NONE)
        {
            switch (attribList[0])
            {
              case EGL_RENDER_BUFFER:
                switch (attribList[1])
                {
                  case EGL_BACK_BUFFER:
                    break;
                  case EGL_SINGLE_BUFFER:
                    return error(EGL_BAD_MATCH, EGL_NO_SURFACE);   // Rendering directly to front buffer not supported
                  default:
                    return error(EGL_BAD_ATTRIBUTE, EGL_NO_SURFACE);
                }
                break;
              case EGL_VG_COLORSPACE:
                return error(EGL_BAD_MATCH, EGL_NO_SURFACE);
              case EGL_VG_ALPHA_FORMAT:
                return error(EGL_BAD_MATCH, EGL_NO_SURFACE);
              default:
                return error(EGL_BAD_ATTRIBUTE, EGL_NO_SURFACE);
            }

            attribList += 2;
        }
    }

    if(hasExistingWindowSurface(window))
    {
        return error(EGL_BAD_ALLOC, EGL_NO_SURFACE);
    }

    Surface *surface = new Surface(this, configuration, window);

    if(!surface->initialize())
    {
        delete surface;
        return EGL_NO_SURFACE;
    }

    mSurfaceSet.insert(surface);

    return success(surface);
}

EGLSurface Display::createOffscreenSurface(EGLConfig config, const EGLint *attribList)
{
    EGLint width = 0, height = 0;
    EGLenum textureFormat = EGL_NO_TEXTURE;
    EGLenum textureTarget = EGL_NO_TEXTURE;
    const Config *configuration = mConfigSet.get(config);

    if(attribList)
    {
        while(*attribList != EGL_NONE)
        {
            switch (attribList[0])
            {
              case EGL_WIDTH:
                width = attribList[1];
                break;
              case EGL_HEIGHT:
                height = attribList[1];
                break;
              case EGL_LARGEST_PBUFFER:
                if(attribList[1] != EGL_FALSE)
                  UNIMPLEMENTED(); // FIXME
                break;
              case EGL_TEXTURE_FORMAT:
                switch (attribList[1])
                {
                  case EGL_NO_TEXTURE:
                  case EGL_TEXTURE_RGB:
                  case EGL_TEXTURE_RGBA:
                    textureFormat = attribList[1];
                    break;
                  default:
                    return error(EGL_BAD_ATTRIBUTE, EGL_NO_SURFACE);
                }
                break;
              case EGL_TEXTURE_TARGET:
                switch (attribList[1])
                {
                  case EGL_NO_TEXTURE:
                  case EGL_TEXTURE_2D:
                    textureTarget = attribList[1];
                    break;
                  default:
                    return error(EGL_BAD_ATTRIBUTE, EGL_NO_SURFACE);
                }
                break;
              case EGL_MIPMAP_TEXTURE:
                if(attribList[1] != EGL_FALSE)
                  return error(EGL_BAD_ATTRIBUTE, EGL_NO_SURFACE);
                break;
              case EGL_VG_COLORSPACE:
                return error(EGL_BAD_MATCH, EGL_NO_SURFACE);
              case EGL_VG_ALPHA_FORMAT:
                return error(EGL_BAD_MATCH, EGL_NO_SURFACE);
              default:
                return error(EGL_BAD_ATTRIBUTE, EGL_NO_SURFACE);
            }

            attribList += 2;
        }
    }

    if(width < 0 || height < 0)
    {
        return error(EGL_BAD_PARAMETER, EGL_NO_SURFACE);
    }

    if(width == 0 || height == 0)
    {
        return error(EGL_BAD_ATTRIBUTE, EGL_NO_SURFACE);
    }

    if((textureFormat != EGL_NO_TEXTURE && textureTarget == EGL_NO_TEXTURE) ||
       (textureFormat == EGL_NO_TEXTURE && textureTarget != EGL_NO_TEXTURE))
    {
        return error(EGL_BAD_MATCH, EGL_NO_SURFACE);
    }

    if(!(configuration->mSurfaceType & EGL_PBUFFER_BIT))
    {
        return error(EGL_BAD_MATCH, EGL_NO_SURFACE);
    }

    if((textureFormat == EGL_TEXTURE_RGB && configuration->mBindToTextureRGB != EGL_TRUE) ||
       (textureFormat == EGL_TEXTURE_RGBA && configuration->mBindToTextureRGBA != EGL_TRUE))
    {
        return error(EGL_BAD_ATTRIBUTE, EGL_NO_SURFACE);
    }

    Surface *surface = new Surface(this, configuration, width, height, textureFormat, textureTarget);

    if(!surface->initialize())
    {
        delete surface;
        return EGL_NO_SURFACE;
    }

    mSurfaceSet.insert(surface);

    return success(surface);
}

EGLContext Display::createContext(EGLConfig configHandle, const gl::Context *shareContext)
{
    if(!mDevice)
    {
        if(!createDevice())
        {
            return NULL;
        }
    }

    const egl::Config *config = mConfigSet.get(configHandle);

    gl::Context *context = gl::createContext(config, shareContext);
    mContextSet.insert(context);

    return context;
}

void Display::destroySurface(egl::Surface *surface)
{
    delete surface;
    mSurfaceSet.erase(surface);
}

void Display::destroyContext(gl::Context *context)
{
    gl::destroyContext(context);
    mContextSet.erase(context);
}

bool Display::isInitialized() const
{
    return mConfigSet.size() > 0;
}

bool Display::isValidConfig(EGLConfig config)
{
    return mConfigSet.get(config) != NULL;
}

bool Display::isValidContext(gl::Context *context)
{
    return mContextSet.find(context) != mContextSet.end();
}

bool Display::isValidSurface(egl::Surface *surface)
{
    return mSurfaceSet.find(surface) != mSurfaceSet.end();
}

bool Display::isValidWindow(EGLNativeWindowType window)
{
	#if defined(_WIN32)
		return IsWindow(window) == TRUE;
	#else
		XWindowAttributes windowAttributes;
		Status status = XGetWindowAttributes(displayId, window, &windowAttributes);
			
		return status == True;
	#endif
}

bool Display::hasExistingWindowSurface(EGLNativeWindowType window)
{
    for(SurfaceSet::iterator surface = mSurfaceSet.begin(); surface != mSurfaceSet.end(); surface++)
    {
        if((*surface)->getWindowHandle() == window)
        {
            return true;
        }
    }

    return false;
}

EGLint Display::getMinSwapInterval()
{
    return mMinSwapInterval;
}

EGLint Display::getMaxSwapInterval()
{
    return mMaxSwapInterval;
}

gl::Device *Display::getDevice()
{
    if(!mDevice)
    {
        if(!createDevice())
        {
            return NULL;
        }
    }

    return mDevice;
}

EGLNativeDisplayType Display::getNativeDisplay() const
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
