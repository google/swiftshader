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
#include "libEGL/Display.h"

#define GL_APICALL
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

namespace gl
{
	struct Current
	{
		Context *context;
		egl::Display *display;
	};

	void makeCurrent(Context *context, egl::Display *display, egl::Surface *surface);

	Context *getContext();
	egl::Display *getDisplay();

	Device *getDevice();
}

void error(GLenum errorCode);

template<class T>
const T &error(GLenum errorCode, const T &returnValue)
{
    error(errorCode);

    return returnValue;
}

#endif   // LIBGLESV2_MAIN_H_
