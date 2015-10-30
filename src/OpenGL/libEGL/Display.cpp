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
#include "libEGL/Surface.h"
#include "libEGL/Context.hpp"
#include "common/debug.h"

#if defined(__unix__) && !defined(__ANDROID__)
#include "Main/libX11.hpp"
#endif

#ifdef __ANDROID__
#include <system/window.h>
#include <sys/ioctl.h>
#include <linux/fb.h>
#include <fcntl.h>
#endif

#include <algorithm>
#include <vector>
#include <map>

namespace egl
{
typedef std::map<EGLNativeDisplayType, Display*> DisplayMap;
DisplayMap displays;

egl::Display *Display::getPlatformDisplay(EGLenum platform, EGLNativeDisplayType displayId)
{
    #ifndef __ANDROID__
        if(platform == EGL_UNKNOWN)   // Default
        {
            #if defined(__unix__)
				if(libX11)
				{
					platform = EGL_PLATFORM_X11_EXT;
				}
				else
				{
					platform = EGL_PLATFORM_GBM_KHR;
				}
            #endif
        }

        if(displayId == EGL_DEFAULT_DISPLAY)
        {
            if(platform == EGL_PLATFORM_X11_EXT)
            {
                #if defined(__unix__)
					if(libX11->XOpenDisplay)
					{
						displayId = libX11->XOpenDisplay(NULL);
					}
					else
					{
						return error(EGL_BAD_PARAMETER, (egl::Display*)EGL_NO_DISPLAY);
					}
                #else
                    return error(EGL_BAD_PARAMETER, (egl::Display*)EGL_NO_DISPLAY);
                #endif
            }
        }
        else
        {
            // FIXME: Check if displayId is a valid display device context for <platform>
        }
    #endif

    if(displays.find(displayId) != displays.end())
    {
        return displays[displayId];
    }

    egl::Display *display = new egl::Display(platform, displayId);

    displays[displayId] = display;
    return display;
}

Display::Display(EGLenum platform, EGLNativeDisplayType displayId) : platform(platform), displayId(displayId)
{
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
	int registers[4];
	cpuid(registers, 1);
	return (registers[3] & 0x02000000) != 0;
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

	const int samples[] =
	{
		0,
		2,
		4
	};

    const sw::Format renderTargetFormats[] =
    {
        sw::FORMAT_A1R5G5B5,
    //  sw::FORMAT_A2R10G10B10,   // The color_ramp conformance test uses ReadPixels with UNSIGNED_BYTE causing it to think that rendering skipped a colour value.
        sw::FORMAT_A8R8G8B8,
        sw::FORMAT_A8B8G8R8,
        sw::FORMAT_R5G6B5,
    //  sw::FORMAT_X1R5G5B5,      // Has no compatible OpenGL ES renderbuffer format
        sw::FORMAT_X8R8G8B8,
        sw::FORMAT_X8B8G8R8
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

	sw::Format currentDisplayFormat = getDisplayFormat();
    ConfigSet configSet;

	for(int samplesIndex = 0; samplesIndex < sizeof(samples) / sizeof(int); samplesIndex++)
    {
		for(int formatIndex = 0; formatIndex < sizeof(renderTargetFormats) / sizeof(sw::Format); formatIndex++)
		{
			sw::Format renderTargetFormat = renderTargetFormats[formatIndex];

			for(int depthStencilIndex = 0; depthStencilIndex < sizeof(depthStencilFormats) / sizeof(sw::Format); depthStencilIndex++)
			{
				sw::Format depthStencilFormat = depthStencilFormats[depthStencilIndex];

				configSet.add(currentDisplayFormat, mMinSwapInterval, mMaxSwapInterval, renderTargetFormat, depthStencilFormat, samples[samplesIndex]);
			}
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

	if(this == getCurrentDisplay())
	{
		setCurrentDisplay(nullptr);
	}
}

bool Display::getConfigs(EGLConfig *configs, const EGLint *attribList, EGLint configSize, EGLint *numConfig)
{
    return mConfigSet.getConfigs(configs, attribList, configSize, numConfig);
}

bool Display::getConfigAttrib(EGLConfig config, EGLint attribute, EGLint *value)
{
    const egl::Config *configuration = mConfigSet.get(config);

    switch(attribute)
    {
    case EGL_BUFFER_SIZE:                *value = configuration->mBufferSize;               break;
    case EGL_ALPHA_SIZE:                 *value = configuration->mAlphaSize;                break;
    case EGL_BLUE_SIZE:                  *value = configuration->mBlueSize;                 break;
    case EGL_GREEN_SIZE:                 *value = configuration->mGreenSize;                break;
    case EGL_RED_SIZE:                   *value = configuration->mRedSize;                  break;
    case EGL_DEPTH_SIZE:                 *value = configuration->mDepthSize;                break;
    case EGL_STENCIL_SIZE:               *value = configuration->mStencilSize;              break;
    case EGL_CONFIG_CAVEAT:              *value = configuration->mConfigCaveat;             break;
    case EGL_CONFIG_ID:                  *value = configuration->mConfigID;                 break;
    case EGL_LEVEL:                      *value = configuration->mLevel;                    break;
    case EGL_NATIVE_RENDERABLE:          *value = configuration->mNativeRenderable;         break;
    case EGL_NATIVE_VISUAL_ID:           *value = configuration->mNativeVisualID;           break;
    case EGL_NATIVE_VISUAL_TYPE:         *value = configuration->mNativeVisualType;         break;
    case EGL_SAMPLES:                    *value = configuration->mSamples;                  break;
    case EGL_SAMPLE_BUFFERS:             *value = configuration->mSampleBuffers;            break;
    case EGL_SURFACE_TYPE:               *value = configuration->mSurfaceType;              break;
    case EGL_TRANSPARENT_TYPE:           *value = configuration->mTransparentType;          break;
    case EGL_TRANSPARENT_BLUE_VALUE:     *value = configuration->mTransparentBlueValue;     break;
    case EGL_TRANSPARENT_GREEN_VALUE:    *value = configuration->mTransparentGreenValue;    break;
    case EGL_TRANSPARENT_RED_VALUE:      *value = configuration->mTransparentRedValue;      break;
    case EGL_BIND_TO_TEXTURE_RGB:        *value = configuration->mBindToTextureRGB;         break;
    case EGL_BIND_TO_TEXTURE_RGBA:       *value = configuration->mBindToTextureRGBA;        break;
    case EGL_MIN_SWAP_INTERVAL:          *value = configuration->mMinSwapInterval;          break;
    case EGL_MAX_SWAP_INTERVAL:          *value = configuration->mMaxSwapInterval;          break;
    case EGL_LUMINANCE_SIZE:             *value = configuration->mLuminanceSize;            break;
    case EGL_ALPHA_MASK_SIZE:            *value = configuration->mAlphaMaskSize;            break;
    case EGL_COLOR_BUFFER_TYPE:          *value = configuration->mColorBufferType;          break;
    case EGL_RENDERABLE_TYPE:            *value = configuration->mRenderableType;           break;
    case EGL_MATCH_NATIVE_PIXMAP:        *value = EGL_FALSE; UNIMPLEMENTED();               break;
    case EGL_CONFORMANT:                 *value = configuration->mConformant;               break;
    case EGL_MAX_PBUFFER_WIDTH:          *value = configuration->mMaxPBufferWidth;          break;
    case EGL_MAX_PBUFFER_HEIGHT:         *value = configuration->mMaxPBufferHeight;         break;
    case EGL_MAX_PBUFFER_PIXELS:         *value = configuration->mMaxPBufferPixels;         break;
	case EGL_RECORDABLE_ANDROID:         *value = configuration->mRecordableAndroid;        break;
	case EGL_FRAMEBUFFER_TARGET_ANDROID: *value = configuration->mFramebufferTargetAndroid; break;
    default:
        return false;
    }

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

    Surface *surface = new WindowSurface(this, configuration, window);

    if(!surface->initialize())
    {
        surface->release();
        return EGL_NO_SURFACE;
    }

	surface->addRef();
    mSurfaceSet.insert(surface);

    return success(surface);
}

EGLSurface Display::createPBufferSurface(EGLConfig config, const EGLint *attribList)
{
    EGLint width = 0, height = 0;
    EGLenum textureFormat = EGL_NO_TEXTURE;
    EGLenum textureTarget = EGL_NO_TEXTURE;
	EGLBoolean largestPBuffer = EGL_FALSE;
    const Config *configuration = mConfigSet.get(config);

    if(attribList)
    {
        while(*attribList != EGL_NONE)
        {
            switch(attribList[0])
            {
            case EGL_WIDTH:
                width = attribList[1];
                break;
            case EGL_HEIGHT:
                height = attribList[1];
                break;
            case EGL_LARGEST_PBUFFER:
                largestPBuffer = attribList[1];
                break;
            case EGL_TEXTURE_FORMAT:
                switch(attribList[1])
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
                switch(attribList[1])
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
				{
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

    Surface *surface = new PBufferSurface(this, configuration, width, height, textureFormat, textureTarget, largestPBuffer);

    if(!surface->initialize())
    {
        surface->release();
        return EGL_NO_SURFACE;
    }

	surface->addRef();
    mSurfaceSet.insert(surface);

    return success(surface);
}

EGLContext Display::createContext(EGLConfig configHandle, const egl::Context *shareContext, EGLint clientVersion)
{
    const egl::Config *config = mConfigSet.get(configHandle);
	egl::Context *context = 0;

	if(clientVersion == 1 && config->mRenderableType & EGL_OPENGL_ES_BIT)
	{
		if(libGLES_CM)
		{
			context = libGLES_CM->es1CreateContext(config, shareContext);
		}
	}
	else if((clientVersion == 2 && config->mRenderableType & EGL_OPENGL_ES2_BIT)
#ifndef __ANDROID__ // Do not allow GLES 3.0 on Android
	     || (clientVersion == 3 && config->mRenderableType & EGL_OPENGL_ES3_BIT)
#endif
	        )
	{
		if(libGLESv2)
		{
			context = libGLESv2->es2CreateContext(config, shareContext, clientVersion);
		}
	}
	else
	{
		return error(EGL_BAD_CONFIG, EGL_NO_CONTEXT);
	}

	if(!context)
	{
		return error(EGL_BAD_ALLOC, EGL_NO_CONTEXT);
	}

	context->addRef();
	mContextSet.insert(context);

    return success(context);
}

void Display::destroySurface(egl::Surface *surface)
{
	surface->release();
    mSurfaceSet.erase(surface);

	if(surface == getCurrentDrawSurface())
	{
		setCurrentDrawSurface(nullptr);
	}

	if(surface == getCurrentReadSurface())
	{
		setCurrentReadSurface(nullptr);
	}
}

void Display::destroyContext(egl::Context *context)
{
	context->release();
    mContextSet.erase(context);

	if(context == getCurrentContext())
	{
		setCurrentContext(nullptr);
		setCurrentDrawSurface(nullptr);
		setCurrentReadSurface(nullptr);
	}
}

bool Display::isInitialized() const
{
    return mConfigSet.size() > 0;
}

bool Display::isValidConfig(EGLConfig config)
{
    return mConfigSet.get(config) != NULL;
}

bool Display::isValidContext(egl::Context *context)
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
    #elif defined(__ANDROID__)
		if(!window)
		{
			ALOGE("%s called with window==NULL %s:%d", __FUNCTION__, __FILE__, __LINE__);
			return false;
		}
		if(static_cast<ANativeWindow*>(window)->common.magic != ANDROID_NATIVE_WINDOW_MAGIC)
		{
			ALOGE("%s called with window==%p bad magic %s:%d", __FUNCTION__, window, __FILE__, __LINE__);
			return false;
		}
		return true;
    #else
        if(platform == EGL_PLATFORM_X11_EXT)
        {
            XWindowAttributes windowAttributes;
            Status status = libX11->XGetWindowAttributes(displayId, window, &windowAttributes);

            return status == True;
        }
    #endif

    return false;
}

bool Display::hasExistingWindowSurface(EGLNativeWindowType window)
{
    for(SurfaceSet::iterator surface = mSurfaceSet.begin(); surface != mSurfaceSet.end(); surface++)
    {
		if((*surface)->isWindowSurface())
		{
			if((*surface)->getWindowHandle() == window)
			{
				return true;
			}
		}
    }

    return false;
}

EGLint Display::getMinSwapInterval() const
{
    return mMinSwapInterval;
}

EGLint Display::getMaxSwapInterval() const
{
    return mMaxSwapInterval;
}

EGLNativeDisplayType Display::getNativeDisplay() const
{
	return displayId;
}

sw::Format Display::getDisplayFormat() const
{
	#if defined(_WIN32)
		HDC deviceContext = GetDC(0);
		unsigned int bpp = ::GetDeviceCaps(deviceContext, BITSPIXEL);
		ReleaseDC(0, deviceContext);

		switch(bpp)
		{
		case 32: return sw::FORMAT_X8R8G8B8;
		case 24: return sw::FORMAT_R8G8B8;
		case 16: return sw::FORMAT_R5G6B5;
		default: UNREACHABLE(bpp);   // Unexpected display mode color depth
		}
	#elif defined(__ANDROID__)
		static const char *const framebuffer[] =
		{
			"/dev/graphics/fb0",
			"/dev/fb0",
			0
		};

		for(int i = 0; framebuffer[i]; i++)
		{
			int fd = open(framebuffer[i], O_RDONLY, 0);

			if(fd != -1)
			{
				struct fb_var_screeninfo info;
				if(ioctl(fd, FBIOGET_VSCREENINFO, &info) >= 0)
				{
					switch(info.bits_per_pixel)
					{
					case 16:
						return sw::FORMAT_R5G6B5;
					case 32:
						if(info.red.length    == 8 && info.red.offset    == 16 &&
						   info.green.length  == 8 && info.green.offset  == 8  &&
						   info.blue.length   == 8 && info.blue.offset   == 0  &&
						   info.transp.length == 0)
						{
							return sw::FORMAT_X8R8G8B8;
						}
						if(info.red.length    == 8 && info.red.offset    == 0  &&
						   info.green.length  == 8 && info.green.offset  == 8  &&
						   info.blue.length   == 8 && info.blue.offset   == 16 &&
						   info.transp.length == 0)
						{
							return sw::FORMAT_X8B8G8R8;
						}
						if(info.red.length    == 8 && info.red.offset    == 16 &&
						   info.green.length  == 8 && info.green.offset  == 8  &&
						   info.blue.length   == 8 && info.blue.offset   == 0  &&
						   info.transp.length == 8 && info.transp.offset == 24)
						{
							return sw::FORMAT_A8R8G8B8;
						}
						if(info.red.length    == 8 && info.red.offset    == 0  &&
						   info.green.length  == 8 && info.green.offset  == 8  &&
						   info.blue.length   == 8 && info.blue.offset   == 16 &&
						   info.transp.length == 8 && info.transp.offset == 24)
						{
							return sw::FORMAT_A8B8G8R8;
						}
						else UNIMPLEMENTED();
					default:
						UNIMPLEMENTED();
					}
				}

				close(fd);
			}
		}

		// No framebuffer device found, or we're in user space
		return sw::FORMAT_X8R8G8B8;
    #else
        if(platform == EGL_PLATFORM_X11_EXT)
        {
            Screen *screen = libX11->XDefaultScreenOfDisplay(displayId);
            unsigned int bpp = libX11->XPlanesOfScreen(screen);

            switch(bpp)
            {
            case 32: return sw::FORMAT_X8R8G8B8;
            case 24: return sw::FORMAT_R8G8B8;
            case 16: return sw::FORMAT_R5G6B5;
            default: UNREACHABLE(bpp);   // Unexpected display mode color depth
            }
        }
        else if(platform == EGL_PLATFORM_GBM_MESA)
        {
            return sw::FORMAT_X8R8G8B8;
        }
        else UNREACHABLE(platform);
	#endif

	return sw::FORMAT_X8R8G8B8;
}

}
