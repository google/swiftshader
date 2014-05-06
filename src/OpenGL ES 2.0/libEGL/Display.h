//
// Copyright (c) 2002-2011 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//

// Display.h: Defines the egl::Display class, representing the abstract
// display on which graphics are drawn. Implements EGLDisplay.
// [EGL 1.4] section 2.1.2 page 3.

#ifndef INCLUDE_DISPLAY_H_
#define INCLUDE_DISPLAY_H_

#include "Config.h"
#include "Surface.h"
#include "libGLESv2/Context.h"
#include "libGLESv2/Device.hpp"

#ifndef WIN32_LEAN_AND_MEAN
	#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <set>

namespace sw
{
	class Context;
}

namespace egl
{
	class Display
	{
	public:
		~Display();

		static egl::Display *getDisplay(EGLNativeDisplayType displayId);

		bool initialize();
		void terminate();

		virtual void startScene() {};   // FIXME: Remove when Chrome no longer calls getDevice() directly
		virtual void endScene() {};     // FIXME: Remove when Chrome no longer calls getDevice() directly

		bool getConfigs(EGLConfig *configs, const EGLint *attribList, EGLint configSize, EGLint *numConfig);
		bool getConfigAttrib(EGLConfig config, EGLint attribute, EGLint *value);

		EGLSurface createWindowSurface(HWND window, EGLConfig config, const EGLint *attribList);
		EGLSurface createOffscreenSurface(EGLConfig config, HANDLE shareHandle, const EGLint *attribList);
		EGLContext createContext(EGLConfig configHandle, const gl::Context *shareContext);

		void destroySurface(egl::Surface *surface);
		void destroyContext(gl::Context *context);

		bool isInitialized() const;
		bool isValidConfig(EGLConfig config);
		bool isValidContext(gl::Context *context);
		bool isValidSurface(egl::Surface *surface);
		bool hasExistingWindowSurface(HWND window);

		EGLint getMinSwapInterval();
		EGLint getMaxSwapInterval();

		virtual gl::Device *getDeviceChrome() {return 0;};   // FIXME: Remove when Chrome no longer calls getDevice() directly
		virtual gl::Device *getDevice();

		const char *getExtensionString() const;

	private:
		Display(EGLNativeDisplayType displayId);

		const EGLNativeDisplayType displayId;
		gl::Device *mDevice;

		EGLint mMaxSwapInterval;
		EGLint mMinSwapInterval;
    
		typedef std::set<Surface*> SurfaceSet;
		SurfaceSet mSurfaceSet;

		ConfigSet mConfigSet;

		typedef std::set<gl::Context*> ContextSet;
		ContextSet mContextSet;

		bool createDevice();

		void initExtensionString();
		std::string mExtensionString;
	};
}

#endif   // INCLUDE_DISPLAY_H_
