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

// Config.cpp: Implements the egl::Config class, describing the format, type
// and size for an egl::Surface. Implements EGLConfig and related functionality.
// [EGL 1.4] section 3.4 page 15.

#include "Config.h"

#include "common/debug.h"

#define EGLAPI
#include <EGL/eglext.h>

#include <algorithm>
#include <vector>

using namespace std;

namespace egl
{
Config::Config(const DisplayMode &displayMode, EGLint minInterval, EGLint maxInterval, sw::Format renderTargetFormat, sw::Format depthStencilFormat, EGLint multiSample)
    : mDisplayMode(displayMode), mRenderTargetFormat(renderTargetFormat), mDepthStencilFormat(depthStencilFormat), mMultiSample(multiSample)
{
    mBindToTextureRGB = EGL_FALSE;
    mBindToTextureRGBA = EGL_FALSE;

    switch (renderTargetFormat)
    {
      case sw::FORMAT_A1R5G5B5:
        mBufferSize = 16;
        mRedSize = 5;
        mGreenSize = 5;
        mBlueSize = 5;
        mAlphaSize = 1;
        break;
      case sw::FORMAT_A2R10G10B10:
        mBufferSize = 32;
        mRedSize = 10;
        mGreenSize = 10;
        mBlueSize = 10;
        mAlphaSize = 2;
        break;
      case sw::FORMAT_A8R8G8B8:
        mBufferSize = 32;
        mRedSize = 8;
        mGreenSize = 8;
        mBlueSize = 8;
        mAlphaSize = 8;
        mBindToTextureRGBA = true;
        break;
      case sw::FORMAT_R5G6B5:
        mBufferSize = 16;
        mRedSize = 5;
        mGreenSize = 6;
        mBlueSize = 5;
        mAlphaSize = 0;
        break;
      case sw::FORMAT_X8R8G8B8:
        mBufferSize = 32;
        mRedSize = 8;
        mGreenSize = 8;
        mBlueSize = 8;
        mAlphaSize = 0;
        mBindToTextureRGB = true;
        break;
      default:
        UNREACHABLE();   // Other formats should not be valid
    }

    mLuminanceSize = 0;
    mAlphaMaskSize = 0;
    mColorBufferType = EGL_RGB_BUFFER;
    mConfigCaveat = isSlowConfig() ? EGL_SLOW_CONFIG : EGL_NONE;
    mConfigID = 0;
    mConformant = EGL_OPENGL_ES_BIT | EGL_OPENGL_ES2_BIT | EGL_OPENGL_ES3_BIT;

	switch (depthStencilFormat)
	{
	case sw::FORMAT_NULL:
		mDepthSize = 0;
		mStencilSize = 0;
		break;
//  case sw::FORMAT_D16_LOCKABLE:
//      mDepthSize = 16;
//      mStencilSize = 0;
//      break;
	case sw::FORMAT_D32:
		mDepthSize = 32;
		mStencilSize = 0;
		break;
//	case sw::FORMAT_D15S1:
//		mDepthSize = 15;
//		mStencilSize = 1;
//		break;
	case sw::FORMAT_D24S8:
		mDepthSize = 24;
		mStencilSize = 8;
		break;
	case sw::FORMAT_D24X8:
		mDepthSize = 24;
		mStencilSize = 0;
		break;
//	case sw::FORMAT_D24X4S4:
//		mDepthSize = 24;
//		mStencilSize = 4;
//		break;
	case sw::FORMAT_D16:
		mDepthSize = 16;
		mStencilSize = 0;
		break;
//  case sw::FORMAT_D32F_LOCKABLE:
//      mDepthSize = 32;
//      mStencilSize = 0;
//      break;
//  case sw::FORMAT_D24FS8:
//      mDepthSize = 24;
//      mStencilSize = 8;
//      break;
	  default:
		UNREACHABLE();
	}

    mLevel = 0;
    mMatchNativePixmap = EGL_NONE;
    mMaxPBufferWidth = 4096;
    mMaxPBufferHeight = 4096;
    mMaxPBufferPixels = mMaxPBufferWidth * mMaxPBufferHeight;
    mMaxSwapInterval = maxInterval;
    mMinSwapInterval = minInterval;
    mNativeRenderable = EGL_FALSE;
    mNativeVisualID = 0;
    mNativeVisualType = 0;
    mRenderableType = EGL_OPENGL_ES_BIT | EGL_OPENGL_ES2_BIT | EGL_OPENGL_ES3_BIT;
    mSampleBuffers = multiSample ? 1 : 0;
    mSamples = multiSample;
    mSurfaceType = EGL_PBUFFER_BIT | EGL_WINDOW_BIT | EGL_SWAP_BEHAVIOR_PRESERVED_BIT;
    mTransparentType = EGL_NONE;
    mTransparentRedValue = 0;
    mTransparentGreenValue = 0;
    mTransparentBlueValue = 0;
}

EGLConfig Config::getHandle() const
{
    return (EGLConfig)(size_t)mConfigID;
}

bool Config::isSlowConfig() const
{
	return mRenderTargetFormat != sw::FORMAT_X8R8G8B8 && mRenderTargetFormat != sw::FORMAT_A8R8G8B8;
}

SortConfig::SortConfig(const EGLint *attribList)
    : mWantRed(false), mWantGreen(false), mWantBlue(false), mWantAlpha(false), mWantLuminance(false)
{
    scanForWantedComponents(attribList);
}

void SortConfig::scanForWantedComponents(const EGLint *attribList)
{
    // [EGL] section 3.4.1 page 24
    // Sorting rule #3: by larger total number of color bits, not considering
    // components that are 0 or don't-care.
    for(const EGLint *attr = attribList; attr[0] != EGL_NONE; attr += 2)
    {
        if(attr[1] != 0 && attr[1] != EGL_DONT_CARE)
        {
            switch (attr[0])
            {
              case EGL_RED_SIZE:       mWantRed = true; break;
              case EGL_GREEN_SIZE:     mWantGreen = true; break;
              case EGL_BLUE_SIZE:      mWantBlue = true; break;
              case EGL_ALPHA_SIZE:     mWantAlpha = true; break;
              case EGL_LUMINANCE_SIZE: mWantLuminance = true; break;
            }
        }
    }
}

EGLint SortConfig::wantedComponentsSize(const Config &config) const
{
    EGLint total = 0;

    if(mWantRed)       total += config.mRedSize;
    if(mWantGreen)     total += config.mGreenSize;
    if(mWantBlue)      total += config.mBlueSize;
    if(mWantAlpha)     total += config.mAlphaSize;
    if(mWantLuminance) total += config.mLuminanceSize;

    return total;
}

bool SortConfig::operator()(const Config *x, const Config *y) const
{
    return (*this)(*x, *y);
}

bool SortConfig::operator()(const Config &x, const Config &y) const
{
    #define SORT(attribute)                        \
        if(x.attribute != y.attribute)             \
        {                                          \
            return x.attribute < y.attribute;      \
        }

    META_ASSERT(EGL_NONE < EGL_SLOW_CONFIG && EGL_SLOW_CONFIG < EGL_NON_CONFORMANT_CONFIG);
    SORT(mConfigCaveat);

    META_ASSERT(EGL_RGB_BUFFER < EGL_LUMINANCE_BUFFER);
    SORT(mColorBufferType);

    // By larger total number of color bits, only considering those that are requested to be > 0.
    EGLint xComponentsSize = wantedComponentsSize(x);
    EGLint yComponentsSize = wantedComponentsSize(y);
    if(xComponentsSize != yComponentsSize)
    {
        return xComponentsSize > yComponentsSize;
    }

    SORT(mBufferSize);
    SORT(mSampleBuffers);
    SORT(mSamples);
    SORT(mDepthSize);
    SORT(mStencilSize);
    SORT(mAlphaMaskSize);
    SORT(mNativeVisualType);
    SORT(mConfigID);

    #undef SORT

    return false;
}

// We'd like to use SortConfig to also eliminate duplicate configs.
// This works as long as we never have two configs with different per-RGB-component layouts,
// but the same total.
// 5551 and 565 are different because R+G+B is different.
// 5551 and 555 are different because bufferSize is different.
const EGLint ConfigSet::mSortAttribs[] =
{
    EGL_RED_SIZE, 1,
    EGL_GREEN_SIZE, 1,
    EGL_BLUE_SIZE, 1,
    EGL_LUMINANCE_SIZE, 1,
    // BUT NOT ALPHA
    EGL_NONE
};

ConfigSet::ConfigSet()
    : mSet(SortConfig(mSortAttribs))
{
}

void ConfigSet::add(const DisplayMode &displayMode, EGLint minSwapInterval, EGLint maxSwapInterval, sw::Format renderTargetFormat, sw::Format depthStencilFormat, EGLint multiSample)
{
    Config config(displayMode, minSwapInterval, maxSwapInterval, renderTargetFormat, depthStencilFormat, multiSample);

    mSet.insert(config);
}

size_t ConfigSet::size() const
{
    return mSet.size();
}

bool ConfigSet::getConfigs(EGLConfig *configs, const EGLint *attribList, EGLint configSize, EGLint *numConfig)
{
    vector<const Config*> passed;
    passed.reserve(mSet.size());

    for(Iterator config = mSet.begin(); config != mSet.end(); config++)
    {
        bool match = true;
        const EGLint *attribute = attribList;

        while(attribute[0] != EGL_NONE)
        {
            switch(attribute[0])
            {
            case EGL_BUFFER_SIZE:                match = config->mBufferSize >= attribute[1];                      break;
            case EGL_ALPHA_SIZE:                 match = config->mAlphaSize >= attribute[1];                       break;
            case EGL_BLUE_SIZE:                  match = config->mBlueSize >= attribute[1];                        break;
            case EGL_GREEN_SIZE:                 match = config->mGreenSize >= attribute[1];                       break;
            case EGL_RED_SIZE:                   match = config->mRedSize >= attribute[1];                         break;
            case EGL_DEPTH_SIZE:                 match = config->mDepthSize >= attribute[1];                       break;
            case EGL_STENCIL_SIZE:               match = config->mStencilSize >= attribute[1];                     break;
            case EGL_CONFIG_CAVEAT:              match = config->mConfigCaveat == attribute[1];                    break;
            case EGL_CONFIG_ID:                  match = config->mConfigID == attribute[1];                        break;
            case EGL_LEVEL:                      match = config->mLevel >= attribute[1];                           break;
            case EGL_NATIVE_RENDERABLE:          match = config->mNativeRenderable == attribute[1];                break;
            case EGL_NATIVE_VISUAL_ID:           match = config->mNativeVisualID == attribute[1];                  break;
            case EGL_NATIVE_VISUAL_TYPE:         match = config->mNativeVisualType == attribute[1];                break;
            case EGL_SAMPLES:                    match = config->mSamples >= attribute[1];                         break;
            case EGL_SAMPLE_BUFFERS:             match = config->mSampleBuffers >= attribute[1];                   break;
            case EGL_SURFACE_TYPE:               match = (config->mSurfaceType & attribute[1]) == attribute[1];    break;
            case EGL_TRANSPARENT_TYPE:           match = config->mTransparentType == attribute[1];                 break;
            case EGL_TRANSPARENT_BLUE_VALUE:     match = config->mTransparentBlueValue == attribute[1];            break;
            case EGL_TRANSPARENT_GREEN_VALUE:    match = config->mTransparentGreenValue == attribute[1];           break;
            case EGL_TRANSPARENT_RED_VALUE:      match = config->mTransparentRedValue == attribute[1];             break;
            case EGL_BIND_TO_TEXTURE_RGB:        match = config->mBindToTextureRGB == attribute[1];                break;
            case EGL_BIND_TO_TEXTURE_RGBA:       match = config->mBindToTextureRGBA == attribute[1];               break;
            case EGL_MIN_SWAP_INTERVAL:          match = config->mMinSwapInterval == attribute[1];                 break;
            case EGL_MAX_SWAP_INTERVAL:          match = config->mMaxSwapInterval == attribute[1];                 break;
            case EGL_LUMINANCE_SIZE:             match = config->mLuminanceSize >= attribute[1];                   break;
            case EGL_ALPHA_MASK_SIZE:            match = config->mAlphaMaskSize >= attribute[1];                   break;
            case EGL_COLOR_BUFFER_TYPE:          match = config->mColorBufferType == attribute[1];                 break;
            case EGL_RENDERABLE_TYPE:            match = (config->mRenderableType & attribute[1]) == attribute[1]; break;
            case EGL_MATCH_NATIVE_PIXMAP:        match = false; UNIMPLEMENTED();                                   break;
            case EGL_CONFORMANT:                 match = (config->mConformant & attribute[1]) == attribute[1];     break;
            case EGL_MAX_PBUFFER_WIDTH:          match = config->mMaxPBufferWidth >= attribute[1];                 break;
            case EGL_MAX_PBUFFER_HEIGHT:         match = config->mMaxPBufferHeight >= attribute[1];                break;
            case EGL_MAX_PBUFFER_PIXELS:         match = config->mMaxPBufferPixels >= attribute[1];                break;
            case EGL_RECORDABLE_ANDROID:         match = true; /* UNIMPLEMENTED(); EGL_ANDROID_recordable */       break;
            case EGL_FRAMEBUFFER_TARGET_ANDROID: match = true; /* UNIMPLEMENTED(); EGL_ANDROID_framebuffer_target */ break;
			default:
				UNIMPLEMENTED();
				match = false;
            }

            if(!match)
            {
                break;
            }

            attribute += 2;
        }

        if(match)
        {
            passed.push_back(&*config);
        }
    }

    if(configs)
    {
        sort(passed.begin(), passed.end(), SortConfig(attribList));

        EGLint index;
        for(index = 0; index < configSize && index < static_cast<EGLint>(passed.size()); index++)
        {
            configs[index] = passed[index]->getHandle();
        }

        *numConfig = index;
    }
    else
    {
        *numConfig = static_cast<EGLint>(passed.size());
    }

    return true;
}

const egl::Config *ConfigSet::get(EGLConfig configHandle)
{
    for(Iterator config = mSet.begin(); config != mSet.end(); config++)
    {
        if(config->getHandle() == configHandle)
        {
            return &(*config);
        }
    }

    return NULL;
}
}
