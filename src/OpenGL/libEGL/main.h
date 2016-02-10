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

// main.h: Management of thread-local data.

#ifndef LIBEGL_MAIN_H_
#define LIBEGL_MAIN_H_

#include "libGLES_CM/libGLES_CM.hpp"
#include "libGLESv2/libGLESv2.hpp"

#include <EGL/egl.h>
#include <EGL/eglext.h>

namespace egl
{
	class Display;
	class Context;
	class Surface;

	struct Current
	{
		EGLint error;
		EGLenum API;
		EGLDisplay display;
		Context *context;
		Surface *drawSurface;
		Surface *readSurface;
	};

	void setCurrentError(EGLint error);
	EGLint getCurrentError();

	void setCurrentAPI(EGLenum API);
	EGLenum getCurrentAPI();

	void setCurrentDisplay(EGLDisplay dpy);
	EGLDisplay getCurrentDisplay();

	void setCurrentContext(Context *ctx);
	Context *getCurrentContext();

	void setCurrentDrawSurface(Surface *surface);
	Surface *getCurrentDrawSurface();

	void setCurrentReadSurface(Surface *surface);
	Surface *getCurrentReadSurface();

	void error(EGLint errorCode);

	template<class T>
	const T &error(EGLint errorCode, const T &returnValue)
	{
		egl::error(errorCode);

		return returnValue;
	}

	template<class T>
	const T &success(const T &returnValue)
	{
		egl::setCurrentError(EGL_SUCCESS);

		return returnValue;
	}

	class Config;
	class Surface;
	class Display;
	class Context;
	class Image;
}

extern LibGLES_CM libGLES_CM;
extern LibGLESv2 libGLESv2;

#endif  // LIBEGL_MAIN_H_
