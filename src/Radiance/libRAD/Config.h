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

// Config.h: Defines the egl::Config class, describing the format, type
// and size for an egl::Surface. Implements EGLConfig and related functionality.
// [EGL 1.4] section 3.4 page 15.

#ifndef INCLUDE_CONFIG_H_
#define INCLUDE_CONFIG_H_

#include "Renderer/Surface.hpp"

#define EGLAPI
#include <EGL/egl.h>

#include <set>

namespace egl
{
class Display;

struct DisplayMode
{
	unsigned int width;
	unsigned int height;
	sw::Format format;
};

class Config
{
  public:
    Config(const DisplayMode &displayMode, EGLint minSwapInterval, EGLint maxSwapInterval, sw::Format renderTargetFormat, sw::Format depthStencilFormat, EGLint multiSample);

    EGLConfig getHandle() const;
	bool isSlowConfig() const;

    const DisplayMode mDisplayMode;
    const sw::Format mRenderTargetFormat;
    const sw::Format mDepthStencilFormat;
    const EGLint mMultiSample;

    EGLint mBufferSize;              // Depth of the color buffer
    EGLint mRedSize;                 // Bits of Red in the color buffer
    EGLint mGreenSize;               // Bits of Green in the color buffer
    EGLint mBlueSize;                // Bits of Blue in the color buffer
    EGLint mLuminanceSize;           // Bits of Luminance in the color buffer
    EGLint mAlphaSize;               // Bits of Alpha in the color buffer
    EGLint mAlphaMaskSize;           // Bits of Alpha Mask in the mask buffer
    EGLBoolean mBindToTextureRGB;    // True if bindable to RGB textures.
    EGLBoolean mBindToTextureRGBA;   // True if bindable to RGBA textures.
    EGLenum mColorBufferType;        // Color buffer type
    EGLenum mConfigCaveat;           // Any caveats for the configuration
    EGLint mConfigID;                // Unique EGLConfig identifier
    EGLint mConformant;              // Whether contexts created with this config are conformant
    EGLint mDepthSize;               // Bits of Z in the depth buffer
    EGLint mLevel;                   // Frame buffer level
    EGLBoolean mMatchNativePixmap;   // Match the native pixmap format
    EGLint mMaxPBufferWidth;         // Maximum width of pbuffer
    EGLint mMaxPBufferHeight;        // Maximum height of pbuffer
    EGLint mMaxPBufferPixels;        // Maximum size of pbuffer
    EGLint mMaxSwapInterval;         // Maximum swap interval
    EGLint mMinSwapInterval;         // Minimum swap interval
    EGLBoolean mNativeRenderable;    // EGL_TRUE if native rendering APIs can render to surface
    EGLint mNativeVisualID;          // Handle of corresponding native visual
    EGLint mNativeVisualType;        // Native visual type of the associated visual
    EGLint mRenderableType;          // Which client rendering APIs are supported.
    EGLint mSampleBuffers;           // Number of multisample buffers
    EGLint mSamples;                 // Number of samples per pixel
    EGLint mStencilSize;             // Bits of Stencil in the stencil buffer
    EGLint mSurfaceType;             // Which types of EGL surfaces are supported.
    EGLenum mTransparentType;        // Type of transparency supported
    EGLint mTransparentRedValue;     // Transparent red value
    EGLint mTransparentGreenValue;   // Transparent green value
    EGLint mTransparentBlueValue;    // Transparent blue value
};

// Function object used by STL sorting routines for ordering Configs according to [EGL] section 3.4.1 page 24.
class SortConfig
{
  public:
    explicit SortConfig(const EGLint *attribList);

    bool operator()(const Config *x, const Config *y) const;
    bool operator()(const Config &x, const Config &y) const;

  private:
    void scanForWantedComponents(const EGLint *attribList);
    EGLint wantedComponentsSize(const Config &config) const;

    bool mWantRed;
    bool mWantGreen;
    bool mWantBlue;
    bool mWantAlpha;
    bool mWantLuminance;
};

class ConfigSet
{
    friend class Display;

  public:
    ConfigSet();

    void add(const DisplayMode &displayMode, EGLint minSwapInterval, EGLint maxSwapInterval, sw::Format renderTargetFormat, sw::Format depthStencilFormat, EGLint multiSample);
    size_t size() const;
    bool getConfigs(EGLConfig *configs, const EGLint *attribList, EGLint configSize, EGLint *numConfig);
    const egl::Config *get(EGLConfig configHandle);

  private:
    typedef std::set<Config, SortConfig> Set;
    typedef Set::iterator Iterator;
    Set mSet;

    static const EGLint mSortAttribs[];
};
}

#endif   // INCLUDE_CONFIG_H_
