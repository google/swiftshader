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

#ifndef LIBGL_MAIN_H_
#define LIBGL_MAIN_H_

#include "Context.h"
#include "Device.hpp"
#include "common/debug.h"
#include "Display.h"

#define _GDI32_
#include <windows.h>
#include <GL/GL.h>
#define GL_GLEXT_PROTOTYPES
#include <GL/glext.h>

namespace gl
{
	struct Current
	{
		Context *context;
		Display *display;
		Surface *drawSurface;
		Surface *readSurface;
	};

	void makeCurrent(Context *context, Display *display, Surface *surface);

	Context *getContext();
	Display *getDisplay();
	Device *getDevice();
	Surface *getCurrentDrawSurface();
	Surface *getCurrentReadSurface();
	
	void setCurrentDisplay(Display *dpy);
	void setCurrentContext(gl::Context *ctx);
	void setCurrentDrawSurface(Surface *surface);
	void setCurrentReadSurface(Surface *surface);
}

void error(GLenum errorCode);

template<class T>
T &error(GLenum errorCode, T &returnValue)
{
    error(errorCode);

    return returnValue;
}

template<class T>
const T &error(GLenum errorCode, const T &returnValue)
{
    error(errorCode);

    return returnValue;
}

extern "C" sw::FrameBuffer *createFrameBuffer(NativeDisplayType display, NativeWindowType window, int width, int height);

#endif   // LIBGL_MAIN_H_
