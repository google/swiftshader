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

#include "Surface.h"
#include "Common/Thread.hpp"
#include "Common/SharedLibrary.hpp"
#include "common/debug.h"
#include "resource.h"

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
		current->context = EGL_NO_CONTEXT;
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
		if(false)
		{
			WaitForDebugger(instance);
		}
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

	EGLContext getCurrentContext()
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

namespace egl
{
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

extern "C"
{
	EGLContext clientGetCurrentContext()
	{
		return egl::getCurrentContext();
	}

	EGLContext clientGetCurrentDisplay()
	{
		return egl::getCurrentDisplay();
	}
}

namespace es2
{
es2::Context *getContext()
{
	egl::Context *context = static_cast<egl::Context*>(egl::getCurrentContext());
	
	if(context && context->getClientVersion() == 2)
	{
		return static_cast<es2::Context*>(context);
	}
	
	return 0;
}

egl::Display *getDisplay()
{
    return static_cast<egl::Display*>(egl::getCurrentDisplay());
}

Device *getDevice()
{
    Context *context = getContext();

    return context ? context->getDevice() : 0;
}
}

namespace egl
{
GLint getClientVersion()
{
	Context *context = static_cast<egl::Context*>(egl::getCurrentContext());

    return context ? context->getClientVersion() : 0;
}
}

namespace rad
{
	// Records an error code
	void error(GLenum errorCode)
	{
		es2::Context *context = es2::getContext();

		if(context)
		{
			switch(errorCode)
			{
			case GL_INVALID_ENUM:
				context->recordInvalidEnum();
				TRACE("\t! Error generated: invalid enum\n");
				break;
			case GL_INVALID_VALUE:
				context->recordInvalidValue();
				TRACE("\t! Error generated: invalid value\n");
				break;
			case GL_INVALID_OPERATION:
				context->recordInvalidOperation();
				TRACE("\t! Error generated: invalid operation\n");
				break;
			case GL_OUT_OF_MEMORY:
				context->recordOutOfMemory();
				TRACE("\t! Error generated: out of memory\n");
				break;
			case GL_INVALID_FRAMEBUFFER_OPERATION:
				context->recordInvalidFramebufferOperation();
				TRACE("\t! Error generated: invalid framebuffer operation\n");
				break;
			default: UNREACHABLE();
			}
		}
	}
}
