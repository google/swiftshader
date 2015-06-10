// dllmain.cpp : Defines the entry point for the DLL application.
#include "windows.h"
#include "opencl.h"
#include "debug.h"
#include <windows.h>
#include <intrin.h>
#include <WinUser.h>
#include "dllmain.h"

#if defined(_WIN32)
typedef DWORD LocalStorageKey;
#else
typedef pthread_key_t LocalStorageKey;
#endif

static LocalStorageKey currentTLS = TLS_OUT_OF_INDEXES;

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

extern "C" BOOL APIENTRY DllMain(HINSTANCE instance, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	//UNIMPLEMENTED();
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		/*#ifdef NDEBUG
			WaitForDebugger(instance);
		#endif*/
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