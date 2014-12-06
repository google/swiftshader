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

#ifndef LIBGLESV2_MAIN_H_
#define LIBGLESV2_MAIN_H_

#include "Context.h"
#include "Device.hpp"
#include "common/debug.h"
#include "Display.h"

#define GL_APICALL
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

namespace es2
{
	Context *getContext();
	egl::Display *getDisplay();
	Device *getDevice();
}

namespace egl
{
	GLint getClientVersion();
}

namespace rad
{
void error(GLenum errorCode);

template<class T>
const T &error(GLenum errorCode, const T &returnValue)
{
    error(errorCode);

    return returnValue;
}
}

#define EGLAPI
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <RAD/rad.h>

namespace egl
{
	struct Current
	{
		EGLint error;
		EGLenum API;
		EGLDisplay display;
		EGLContext context;
		EGLSurface drawSurface;
		EGLSurface readSurface;
	};

	void setCurrentError(EGLint error);
	EGLint getCurrentError();

	void setCurrentAPI(EGLenum API);
	EGLenum getCurrentAPI();

	void setCurrentDisplay(EGLDisplay dpy);
	EGLDisplay getCurrentDisplay();

	void setCurrentContext(EGLContext ctx);
	EGLContext getCurrentContext();

	void setCurrentDrawSurface(EGLSurface surface);
	EGLSurface getCurrentDrawSurface();

	void setCurrentReadSurface(EGLSurface surface);
	EGLSurface getCurrentReadSurface();

	void error(EGLint errorCode);

	template<class T>
	const T &error(EGLint errorCode, const T &returnValue)
	{
		error(errorCode);

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

#endif   // LIBGLESV2_MAIN_H_
