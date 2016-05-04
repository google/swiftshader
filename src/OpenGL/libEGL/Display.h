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

// Display.h: Defines the egl::Display class, representing the abstract
// display on which graphics are drawn. Implements EGLDisplay.
// [EGL 1.4] section 2.1.2 page 3.

#ifndef INCLUDE_DISPLAY_H_
#define INCLUDE_DISPLAY_H_

#include "Config.h"
#include "Sync.hpp"

#include <set>

namespace egl
{
	class Surface;
	class Context;

	const EGLDisplay PRIMARY_DISPLAY = (EGLDisplay)1;
	const EGLDisplay HEADLESS_DISPLAY = (EGLDisplay)0xFACE1E55;

	class Display
	{
	public:
		static Display *get(EGLDisplay dpy);

		bool initialize();
		void terminate();

		bool getConfigs(EGLConfig *configs, const EGLint *attribList, EGLint configSize, EGLint *numConfig);
		bool getConfigAttrib(EGLConfig config, EGLint attribute, EGLint *value);

		EGLSurface createWindowSurface(EGLNativeWindowType window, EGLConfig config, const EGLint *attribList);
		EGLSurface createPBufferSurface(EGLConfig config, const EGLint *attribList);
		EGLContext createContext(EGLConfig configHandle, const Context *shareContext, EGLint clientVersion);
		EGLSyncKHR createSync(Context *context);

		void destroySurface(Surface *surface);
		void destroyContext(Context *context);
		void destroySync(FenceSync *sync);

		bool isInitialized() const;
		bool isValidConfig(EGLConfig config);
		bool isValidContext(Context *context);
		bool isValidSurface(Surface *surface);
		bool isValidWindow(EGLNativeWindowType window);
		bool hasExistingWindowSurface(EGLNativeWindowType window);
		bool isValidSync(FenceSync *sync);

		EGLint getMinSwapInterval() const;
		EGLint getMaxSwapInterval() const;

		void *getNativeDisplay() const;
		const char *getExtensionString() const;

	private:
		explicit Display(void *nativeDisplay);
		~Display();

		sw::Format getDisplayFormat() const;

		void *const nativeDisplay;

		EGLint mMaxSwapInterval;
		EGLint mMinSwapInterval;

		typedef std::set<Surface*> SurfaceSet;
		SurfaceSet mSurfaceSet;

		ConfigSet mConfigSet;

		typedef std::set<Context*> ContextSet;
		ContextSet mContextSet;

		typedef std::set<FenceSync*> SyncSet;
		SyncSet mSyncSet;
	};
}

#endif   // INCLUDE_DISPLAY_H_
