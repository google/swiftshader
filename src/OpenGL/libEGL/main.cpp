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

#include "libEGL.hpp"
#include "Context.hpp"
#include "Surface.h"

#include "resource.h"
#include "Common/Thread.hpp"
#include "Common/SharedLibrary.hpp"
#include "common/debug.h"

#define EGL_EGLEXT_PROTOTYPES
#include <EGL/eglext.h>

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
		current->display = nullptr;
		current->context = nullptr;
		current->drawSurface = nullptr;
        current->readSurface = nullptr;
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

CONSTRUCTOR static void eglAttachProcess()
{
    TRACE("()");

	#if !defined(ANGLE_DISABLE_TRACE) && defined(TRACE_OUTPUT_FILE)
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
        return;
    }

	eglAttachThread();
}

DESTRUCTOR static void eglDetachProcess()
{
    TRACE("()");

	eglDetachThread();
	sw::Thread::freeLocalStorageKey(currentTLS);
}

#if defined(_WIN32)
static INT_PTR CALLBACK DebuggerWaitDialogProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	RECT rect;

    switch(uMsg)
    {
    case WM_INITDIALOG:
		GetWindowRect(GetDesktopWindow(), &rect);
		SetWindowPos(hwnd, HWND_TOP, rect.right / 2, rect.bottom / 2, 0, 0, SWP_NOSIZE);
		SetTimer(hwnd, 1, 100, NULL);
		return TRUE;
    case WM_COMMAND:
        if(LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hwnd, 0);
		}
        break;
    case WM_TIMER:
		if(IsDebuggerPresent())
		{
			EndDialog(hwnd, 0);
		}
    }

    return FALSE;
}

static void WaitForDebugger(HINSTANCE instance)
{
    if(!IsDebuggerPresent())
    {
        HRSRC dialog = FindResource(instance, MAKEINTRESOURCE(IDD_DIALOG1), RT_DIALOG);
		DLGTEMPLATE *dialogTemplate = (DLGTEMPLATE*)LoadResource(instance, dialog);
		DialogBoxIndirect(instance, dialogTemplate, NULL, DebuggerWaitDialogProc);
    }
}

extern "C" BOOL WINAPI DllMain(HINSTANCE instance, DWORD reason, LPVOID reserved)
{
    switch(reason)
    {
    case DLL_PROCESS_ATTACH:
		#ifndef NDEBUG
			WaitForDebugger(instance);
		#endif
        eglAttachProcess();
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

void setCurrentDisplay(egl::Display *dpy)
{
    Current *current = eglGetCurrent();

    current->display = dpy;
}

egl::Display *getCurrentDisplay()
{
    Current *current = eglGetCurrent();

    return current->display;
}

void setCurrentContext(egl::Context *ctx)
{
    Current *current = eglGetCurrent();

	if(ctx)
	{
		ctx->addRef();
	}

	if(current->context)
	{
		current->context->release();
	}

    current->context = ctx;
}

egl::Context *getCurrentContext()
{
    Current *current = eglGetCurrent();

    return current->context;
}

void setCurrentDrawSurface(egl::Surface *surface)
{
    Current *current = eglGetCurrent();

	if(surface)
	{
		surface->addRef();
	}

	if(current->drawSurface)
	{
		current->drawSurface->release();
	}

    current->drawSurface = surface;
}

egl::Surface *getCurrentDrawSurface()
{
    Current *current = eglGetCurrent();

    return current->drawSurface;
}

void setCurrentReadSurface(egl::Surface *surface)
{
    Current *current = eglGetCurrent();

	if(surface)
	{
		surface->addRef();
	}

	if(current->readSurface)
	{
		current->readSurface->release();
	}

    current->readSurface = surface;
}

egl::Surface *getCurrentReadSurface()
{
    Current *current = eglGetCurrent();

    return current->readSurface;
}

void error(EGLint errorCode)
{
    egl::setCurrentError(errorCode);

	if(errorCode != EGL_SUCCESS)
	{
		switch(errorCode)
		{
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
}
}

LibEGLexports::LibEGLexports()
{
	this->eglGetError = ::eglGetError;
	this->eglGetDisplay = ::eglGetDisplay;
	this->eglInitialize = ::eglInitialize;
	this->eglTerminate = ::eglTerminate;
	this->eglQueryString = ::eglQueryString;
	this->eglGetConfigs = ::eglGetConfigs;
	this->eglChooseConfig = ::eglChooseConfig;
	this->eglGetConfigAttrib = ::eglGetConfigAttrib;
	this->eglCreateWindowSurface = ::eglCreateWindowSurface;
	this->eglCreatePbufferSurface = ::eglCreatePbufferSurface;
	this->eglCreatePixmapSurface = ::eglCreatePixmapSurface;
	this->eglDestroySurface = ::eglDestroySurface;
	this->eglQuerySurface = ::eglQuerySurface;
	this->eglBindAPI = ::eglBindAPI;
	this->eglQueryAPI = ::eglQueryAPI;
	this->eglWaitClient = ::eglWaitClient;
	this->eglReleaseThread = ::eglReleaseThread;
	this->eglCreatePbufferFromClientBuffer = ::eglCreatePbufferFromClientBuffer;
	this->eglSurfaceAttrib = ::eglSurfaceAttrib;
	this->eglBindTexImage = ::eglBindTexImage;
	this->eglReleaseTexImage = ::eglReleaseTexImage;
	this->eglSwapInterval = ::eglSwapInterval;
	this->eglCreateContext = ::eglCreateContext;
	this->eglDestroyContext = ::eglDestroyContext;
	this->eglMakeCurrent = ::eglMakeCurrent;
	this->eglGetCurrentContext = ::eglGetCurrentContext;
	this->eglGetCurrentSurface = ::eglGetCurrentSurface;
	this->eglGetCurrentDisplay = ::eglGetCurrentDisplay;
	this->eglQueryContext = ::eglQueryContext;
	this->eglWaitGL = ::eglWaitGL;
	this->eglWaitNative = ::eglWaitNative;
	this->eglSwapBuffers = ::eglSwapBuffers;
	this->eglCopyBuffers = ::eglCopyBuffers;
	this->eglCreateImageKHR = ::eglCreateImageKHR;
	this->eglDestroyImageKHR = ::eglDestroyImageKHR;
	this->eglGetProcAddress = ::eglGetProcAddress;

	this->clientGetCurrentContext = egl::getCurrentContext;
	this->clientGetCurrentDisplay = egl::getCurrentDisplay;
}

extern "C" LibEGLexports *libEGL_swiftshader()
{
	static LibEGLexports libEGL;
	return &libEGL;
}

LibGLES_CM libGLES_CM;
LibGLESv2 libGLESv2;
