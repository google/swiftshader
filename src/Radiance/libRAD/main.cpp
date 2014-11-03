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
#include "Common/SharedLibrary.hpp"
#include "common/debug.h"

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
}

static void glDetachThread()
{
    TRACE("()");
}

CONSTRUCTOR static bool glAttachProcess()
{
    TRACE("()");

    glAttachThread();

    #if defined(_WIN32)
	const char *libEGL_lib = "libEGL.dll";
	#else
	const char *libEGL_lib = "libEGL.so.1";
	#endif

	libEGL = loadLibrary(libEGL_lib);
	egl::getCurrentContext = (egl::Context *(*)())getProcAddress(libEGL, "clientGetCurrentContext");
	egl::getCurrentDisplay = (egl::Display *(*)())getProcAddress(libEGL, "clientGetCurrentDisplay");

    return libEGL != 0;
}

DESTRUCTOR static void glDetachProcess()
{
    TRACE("()");

	glDetachThread();
	freeLibrary(libEGL);
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

namespace es2
{
es2::Context *getContext()
{
	egl::Context *context = egl::getCurrentContext();
	
	if(context && context->getClientVersion() == 2)
	{
		return static_cast<es2::Context*>(context);
	}
	
	return 0;
}

egl::Display *getDisplay()
{
    return egl::getCurrentDisplay();
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
	Context *context = egl::getCurrentContext();

    return context ? context->getClientVersion() : 0;
}
}

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

namespace egl
{
	egl::Context *(*getCurrentContext)() = 0;
	egl::Display *(*getCurrentDisplay)() = 0;
}

void *libEGL = 0;   // Handle to the libEGL module
