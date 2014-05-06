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

// main.cpp: DLL entry point and management of thread-local data.

#include "main.h"

#include "common/debug.h"

static DWORD currentTLS = TLS_OUT_OF_INDEXES;

BOOL WINAPI DllMain(HINSTANCE instance, DWORD reason, LPVOID reserved)
{
    switch (reason)
    {
      case DLL_PROCESS_ATTACH:
        {
#if !defined(ANGLE_DISABLE_TRACE)
            FILE *debug = fopen(TRACE_OUTPUT_FILE, "rt");

            if(debug)
            {
                fclose(debug);
                debug = fopen(TRACE_OUTPUT_FILE, "wt");   // Erase
                fclose(debug);
            }
#endif

            currentTLS = TlsAlloc();

            if(currentTLS == TLS_OUT_OF_INDEXES)
            {
                return FALSE;
            }
        }
        // Fall throught to initialize index
      case DLL_THREAD_ATTACH:
        {
            egl::Current *current = (egl::Current*)LocalAlloc(LPTR, sizeof(egl::Current));

            if(current)
            {
                TlsSetValue(currentTLS, current);

                current->error = EGL_SUCCESS;
                current->API = EGL_OPENGL_ES_API;
                current->display = EGL_NO_DISPLAY;
                current->drawSurface = EGL_NO_SURFACE;
                current->readSurface = EGL_NO_SURFACE;
            }
        }
        break;
      case DLL_THREAD_DETACH:
        {
            void *current = TlsGetValue(currentTLS);

            if(current)
            {
                LocalFree((HLOCAL)current);
            }
        }
        break;
      case DLL_PROCESS_DETACH:
        {
            void *current = TlsGetValue(currentTLS);

            if(current)
            {
                LocalFree((HLOCAL)current);
            }

            TlsFree(currentTLS);
        }
        break;
      default:
        break;
    }

    return TRUE;
}

namespace egl
{
void setCurrentError(EGLint error)
{
    Current *current = (Current*)TlsGetValue(currentTLS);

    current->error = error;
}

EGLint getCurrentError()
{
    Current *current = (Current*)TlsGetValue(currentTLS);

    return current->error;
}

void setCurrentAPI(EGLenum API)
{
    Current *current = (Current*)TlsGetValue(currentTLS);

    current->API = API;
}

EGLenum getCurrentAPI()
{
    Current *current = (Current*)TlsGetValue(currentTLS);

    return current->API;
}

void setCurrentDisplay(EGLDisplay dpy)
{
    Current *current = (Current*)TlsGetValue(currentTLS);

    current->display = dpy;
}

EGLDisplay getCurrentDisplay()
{
    Current *current = (Current*)TlsGetValue(currentTLS);

    return current->display;
}

void setCurrentDrawSurface(EGLSurface surface)
{
    Current *current = (Current*)TlsGetValue(currentTLS);

    current->drawSurface = surface;
}

EGLSurface getCurrentDrawSurface()
{
    Current *current = (Current*)TlsGetValue(currentTLS);

    return current->drawSurface;
}

void setCurrentReadSurface(EGLSurface surface)
{
    Current *current = (Current*)TlsGetValue(currentTLS);

    current->readSurface = surface;
}

EGLSurface getCurrentReadSurface()
{
    Current *current = (Current*)TlsGetValue(currentTLS);

    return current->readSurface;
}
}

void error(EGLint errorCode)
{
    egl::setCurrentError(errorCode);
}
