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

// main.cpp: DLL entry point and management of thread-local data.

#include "main.h"

#include "Common/Thread.hpp"
#include "Common/SharedLibrary.hpp"
#include "common/debug.h"

static sw::Thread::LocalStorageKey currentTLS = TLS_OUT_OF_INDEXES;

#if !defined(_MSC_VER)
#define CONSTRUCTOR __attribute__((constructor))
#define DESTRUCTOR __attribute__((destructor))
#else
#define CONSTRUCTOR
#define DESTRUCTOR
#endif

static void eglAttachThread()
{
    TRACE("()");

    egl::Current *current = new egl::Current;

    if(current)
    {
        sw::Thread::setLocalStorage(currentTLS, current);

        current->error = EGL_SUCCESS;
        current->API = EGL_OPENGL_ES_API;
        current->display = EGL_NO_DISPLAY;
        current->drawSurface = EGL_NO_SURFACE;
        current->readSurface = EGL_NO_SURFACE;
	}
}

static void eglDetachThread()
{
    TRACE("()");

	egl::Current *current = (egl::Current*)sw::Thread::getLocalStorage(currentTLS);

	if(current)
	{
        delete current;
	}
}

CONSTRUCTOR static bool eglAttachProcess()
{
    TRACE("()");

	#if !defined(ANGLE_DISABLE_TRACE)
        FILE *debug = fopen(TRACE_OUTPUT_FILE, "rt");

        if(debug)
        {
            fclose(debug);
            debug = fopen(TRACE_OUTPUT_FILE, "wt");   // Erase
            fclose(debug);
        }
	#endif

    currentTLS = sw::Thread::allocateLocalStorageKey();

    if(currentTLS == TLS_OUT_OF_INDEXES)
    {
        return false;
    }

	eglAttachThread();

	#if defined(_WIN32)
	const char *libGLESv2_lib = "libGLESv2.dll";
	#else
	const char *libGLESv2_lib = "libGLESv2.so.2";
	#endif
	
    libGLESv2 = loadLibrary(libGLESv2_lib);
    gl::createDevice = (gl::Device*(*)())getProcAddress(libGLESv2, "createDevice");
    gl::createContext = (gl::Context *(*)(const egl::Config*, const gl::Context*))getProcAddress(libGLESv2, "glCreateContext");
    gl::makeCurrent = (void (*)(gl::Context*, egl::Display*, egl::Surface*))getProcAddress(libGLESv2, "glMakeCurrent");
    gl::getProcAddress = (__eglMustCastToProperFunctionPointerType (*)(const char*))getProcAddress(libGLESv2, "glGetProcAddress");
    gl::createBackBuffer = (gl::Image *(*)(int, int, const egl::Config*))getProcAddress(libGLESv2, "createBackBuffer");
    gl::createFrameBuffer = (sw::FrameBuffer *(*)(EGLNativeDisplayType, EGLNativeWindowType, int, int))getProcAddress(libGLESv2, "createFrameBuffer");

	return libGLESv2 != 0;
}

DESTRUCTOR static void eglDetachProcess()
{
    TRACE("()");

	eglDetachThread();
	sw::Thread::freeLocalStorageKey(currentTLS);
	freeLibrary(libGLESv2);
}

#if defined(_WIN32)
extern "C" BOOL WINAPI DllMain(HINSTANCE instance, DWORD reason, LPVOID reserved)
{
    switch(reason)
    {
    case DLL_PROCESS_ATTACH:
	//	MessageBoxA(0, "Attach debugger now and press OK", "SwiftShader loaded", MB_OK);
        return eglAttachProcess();
        break;
    case DLL_THREAD_ATTACH:
        eglAttachThread();
        break;
    case DLL_THREAD_DETACH:
        eglDetachThread();
        break;
    case DLL_PROCESS_DETACH:
        eglDetachProcess();
        break;
    default:
        break;
    }

    return TRUE;
}
#endif

namespace egl
{
static Current *eglGetCurrent(void)
{
	Current *current = (Current*)sw::Thread::getLocalStorage(currentTLS);

	if(!current)
	{
		eglAttachThread();
	}

	return (Current*)sw::Thread::getLocalStorage(currentTLS);
}

void setCurrentError(EGLint error)
{
    Current *current = eglGetCurrent();

    current->error = error;
}

EGLint getCurrentError()
{
    Current *current = eglGetCurrent();

    return current->error;
}

void setCurrentAPI(EGLenum API)
{
    Current *current = eglGetCurrent();

    current->API = API;
}

EGLenum getCurrentAPI()
{
    Current *current = eglGetCurrent();

    return current->API;
}

void setCurrentDisplay(EGLDisplay dpy)
{
    Current *current = eglGetCurrent();

    current->display = dpy;
}

EGLDisplay getCurrentDisplay()
{
    Current *current = eglGetCurrent();

    return current->display;
}

void setCurrentContext(EGLContext ctx)
{
    Current *current = eglGetCurrent();

    current->context = ctx;
}

EGLDisplay getCurrentContext()
{
    Current *current = eglGetCurrent();

    return current->context;
}

void setCurrentDrawSurface(EGLSurface surface)
{
    Current *current = eglGetCurrent();

    current->drawSurface = surface;
}

EGLSurface getCurrentDrawSurface()
{
    Current *current = eglGetCurrent();

    return current->drawSurface;
}

void setCurrentReadSurface(EGLSurface surface)
{
    Current *current = eglGetCurrent();

    current->readSurface = surface;
}

EGLSurface getCurrentReadSurface()
{
    Current *current = eglGetCurrent();

    return current->readSurface;
}
}

void error(EGLint errorCode)
{
    egl::setCurrentError(errorCode);

    switch(errorCode)
    {
    case EGL_SUCCESS:                                                                  break;
    case EGL_NOT_INITIALIZED:     TRACE("\t! Error generated: not initialized\n");     break;
    case EGL_BAD_ACCESS:          TRACE("\t! Error generated: bad access\n");          break;
    case EGL_BAD_ALLOC:           TRACE("\t! Error generated: bad alloc\n");           break;
    case EGL_BAD_ATTRIBUTE:       TRACE("\t! Error generated: bad attribute\n");       break;
    case EGL_BAD_CONFIG:          TRACE("\t! Error generated: bad config\n");          break;
    case EGL_BAD_CONTEXT:         TRACE("\t! Error generated: bad context\n");         break;
    case EGL_BAD_CURRENT_SURFACE: TRACE("\t! Error generated: bad current surface\n"); break;
    case EGL_BAD_DISPLAY:         TRACE("\t! Error generated: bad display\n");         break;
    case EGL_BAD_MATCH:           TRACE("\t! Error generated: bad match\n");           break;
    case EGL_BAD_NATIVE_PIXMAP:   TRACE("\t! Error generated: bad native pixmap\n");   break;
    case EGL_BAD_NATIVE_WINDOW:   TRACE("\t! Error generated: bad native window\n");   break;
    case EGL_BAD_PARAMETER:       TRACE("\t! Error generated: bad parameter\n");       break;
    case EGL_BAD_SURFACE:         TRACE("\t! Error generated: bad surface\n");         break;
    case EGL_CONTEXT_LOST:        TRACE("\t! Error generated: context lost\n");        break;
    default:                      TRACE("\t! Error generated: <0x%X>\n", errorCode);   break;
    }
}

namespace gl
{
	Device *(*createDevice)() = 0;
	Context *(*createContext)(const egl::Config *config, const Context *shareContext) = 0;
	void (*bindTexImage)(egl::Surface *surface) = 0;
	void (*makeCurrent)(Context *context, egl::Display *display, egl::Surface *surface) = 0;
	Context *(*getCurrentContext)() = 0;
	__eglMustCastToProperFunctionPointerType (*getProcAddress)(const char *procname) = 0;
	Image *(*createBackBuffer)(int width, int height, const egl::Config *config) = 0;
	sw::FrameBuffer *(*createFrameBuffer)(EGLNativeDisplayType display, EGLNativeWindowType window, int width, int height) = 0;
}

void *libGLESv2 = 0;   // Handle to the libGLESv2 module
