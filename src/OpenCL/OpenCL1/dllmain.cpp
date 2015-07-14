// dllmain.cpp : Defines the entry point for the DLL application.
#include "dllmain.h"

#include "windows.h"
#include "opencl.h"
#include "Thread.hpp"
#include "debug.h"

#include <windows.h>
#include <intrin.h>
#include <WinUser.h>

static sw::Thread::LocalStorageKey currentTLS = TLS_OUT_OF_INDEXES;

#if defined(_WIN32)
#define IDD_DIALOG1                     101

static void clAttachThread()
{
	TRACE("()");

	cl::Current *current = new cl::Current();
	{
		TlsSetValue(currentTLS, current);
		current->context = 0;
	}
}

static void clDetachThread()
{
	TRACE("()");
	cl::Current *current = (cl::Current*)TlsGetValue(currentTLS);

	if(current)
	{
		delete current;
	}
}

static bool clAttachProcess()
{
	TRACE("()");

	currentTLS = TlsAlloc();

	if(currentTLS == TLS_OUT_OF_INDEXES)
	{
		return false;
	}

	clAttachThread();

	return true;
}

static void clDetachProcess()
{
	TRACE("()");

	clDetachThread();
	TlsFree(currentTLS);
}

extern "C" BOOL APIENTRY DllMain(HINSTANCE instance, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		return clAttachProcess();
		break;
	case DLL_THREAD_ATTACH:
		clAttachThread();
		break;
	case DLL_THREAD_DETACH:
		clDetachThread();
		break;
	case DLL_PROCESS_DETACH:
		clDetachProcess();
		break;
	default:
		break;
	}

	return TRUE;
}
#endif
namespace cl
{
	static Current *getCurrent(void)
	{
		Current *current = (Current*)TlsGetValue(currentTLS);

		if(!current)
		{
			clAttachThread();
		}

		return (Current*)TlsGetValue(currentTLS);
	}

	void makeCurrent(cl_platform_id platformId, Devices::Context *context)
	{
		Current *current = getCurrent();

		current->context = context;
		current->platform = platformId;

		/*if(context)
		{
			context->makeCurrent(surface);
		}*/
	}

	Devices::Context *getContext()
	{
		Current *current = getCurrent();

		return current->context;
	}
}