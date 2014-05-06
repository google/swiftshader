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

#define EGLAPI
#include <EGL/egl.h>
#include <EGL/eglext.h>

namespace egl
{
struct Current
{
    EGLint error;
    EGLenum API;
    EGLDisplay display;
    EGLSurface drawSurface;
    EGLSurface readSurface;
};

void setCurrentError(EGLint error);
EGLint getCurrentError();

void setCurrentAPI(EGLenum API);
EGLenum getCurrentAPI();

void setCurrentDisplay(EGLDisplay dpy);
EGLDisplay getCurrentDisplay();

void setCurrentDrawSurface(EGLSurface surface);
EGLSurface getCurrentDrawSurface();

void setCurrentReadSurface(EGLSurface surface);
EGLSurface getCurrentReadSurface();
}

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

#endif  // LIBEGL_MAIN_H_
