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

#include "Framebuffer.h"
#include "libEGL/Surface.h"
#include "Common/Thread.hpp"
#include "common/debug.h"

static sw::Thread::LocalStorageKey currentTLS = TLS_OUT_OF_INDEXES;

#if !defined(_MSC_VER)
#define CONSTRUCTOR __attribute__((constructor))
#define DESTRUCTOR __attribute__((destructor))
#else
#define CONSTRUCTOR
#define DESTRUCTOR
#endif

static void glAttachThread()
{
    TRACE("()");

	gl::Current *current = new gl::Current;

    if(current)
    {
		sw::Thread::setLocalStorage(currentTLS, current);

        current->context = NULL;
        current->display = NULL;
    }
}

static void glDetachThread()
{
    TRACE("()");

	gl::Current *current = (gl::Current*)sw::Thread::getLocalStorage(currentTLS);

    if(current)
    {
        delete current;
    }
}

CONSTRUCTOR static bool glAttachProcess()
{
    TRACE("()");

	currentTLS = sw::Thread::allocateLocalStorageKey();

    if(currentTLS == TLS_OUT_OF_INDEXES)
    {
        return false;
    }

    glAttachThread();

    return true;
}

DESTRUCTOR static void glDetachProcess()
{
    TRACE("()");

	glDetachThread();

    sw::Thread::freeLocalStorageKey(currentTLS);
}

#if defined(_WIN32)
extern "C" BOOL WINAPI DllMain(HINSTANCE instance, DWORD reason, LPVOID reserved)
{
    switch(reason)
    {
    case DLL_PROCESS_ATTACH:
        return glAttachProcess();
        break;
    case DLL_THREAD_ATTACH:
        glAttachThread();
        break;
    case DLL_THREAD_DETACH:
        glDetachThread();
        break;
    case DLL_PROCESS_DETACH:
        glDetachProcess();
        break;
    default:
        break;
    }

    return TRUE;
}
#endif

namespace gl
{
static gl::Current *getCurrent(void)
{
	Current *current = (Current*)sw::Thread::getLocalStorage(currentTLS);

	if(!current)
	{
		glAttachThread();
	}

	return (Current*)sw::Thread::getLocalStorage(currentTLS);
}

void makeCurrent(Context *context, egl::Display *display, egl::Surface *surface)
{
    Current *current = getCurrent();

    current->context = context;
    current->display = display;

    if(context && display && surface)
    {
        context->makeCurrent(display, surface);
    }
}

Context *getContext()
{
    Current *current = getCurrent();

    return current->context;
}

egl::Display *getDisplay()
{
    Current *current = getCurrent();

    return current->display;
}

Device *getDevice()
{
    egl::Display *display = getDisplay();

    return display->getDevice();
}
}

// Records an error code
void error(GLenum errorCode)
{
    gl::Context *context = gl::getContext();

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
