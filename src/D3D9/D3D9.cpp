// SwiftShader Software Renderer
//
// Copyright(c) 2005-2011 TransGaming Inc.
//
// All rights reserved. No part of this software may be copied, distributed, transmitted,
// transcribed, stored in a retrieval system, translated into any human or computer
// language by any means, or disclosed to third parties without the explicit written
// agreement of TransGaming Inc. Without such an agreement, no rights or licenses, express
// or implied, including but not limited to any patent rights, are granted to you.
//

#include "Direct3D9.hpp"
#include "Direct3D9Ex.hpp"

#include "Debug.hpp"

#include "resource.h"

#include <stdio.h>
#include <assert.h>

namespace D3D9
{
	class Direct3DShaderValidator9
	{
	public:
		Direct3DShaderValidator9();

		virtual ~Direct3DShaderValidator9();

		virtual int __stdcall ValidateShader(long *shader, long *shader1, long *shader2, long *shader3);
		virtual int __stdcall ValidateShader2(long *shader);
		virtual int __stdcall ValidateShader3(long *shader, long *shader1, long *shader2, long *shader3);
		virtual int __stdcall ValidateShader4(long *shader, long *shader1, long *shader2, long *shader3);
		virtual int __stdcall ValidateShader5(long *shader, long *shader1, long *shader2);
		virtual int __stdcall ValidateShader6(long *shader, long *shader1, long *shader2, long *shader3);
	};

	Direct3DShaderValidator9::Direct3DShaderValidator9()
	{
	}

	Direct3DShaderValidator9::~Direct3DShaderValidator9()
	{
	}

	int __stdcall Direct3DShaderValidator9::ValidateShader(long *shader, long *shader1, long *shader2, long *shader3)   // FIXME
	{
		TRACE("");

		UNIMPLEMENTED();

		return true;   // FIXME
	}

	int __stdcall Direct3DShaderValidator9::ValidateShader2(long *shader)   // FIXME
	{
		TRACE("");

		UNIMPLEMENTED();

		return true;   // FIXME
	}

	int __stdcall Direct3DShaderValidator9::ValidateShader3(long *shader, long *shader1, long *shader2, long *shader3)
	{
		TRACE("");

		UNIMPLEMENTED();

		return true;   // FIXME
	}

	int __stdcall Direct3DShaderValidator9::ValidateShader4(long *shader, long *shader1, long *shader2, long *shader3)
	{
		TRACE("");

		UNIMPLEMENTED();

		return true;   // FIXME
	}

	int __stdcall Direct3DShaderValidator9::ValidateShader5(long *shader, long *shader1, long *shader2)   // FIXME
	{
		TRACE("");

		UNIMPLEMENTED();

		return true;   // FIXME
	}

	int __stdcall Direct3DShaderValidator9::ValidateShader6(long *shader, long *shader1, long *shader2, long *shader3)   // FIXME
	{
		TRACE("");

		UNIMPLEMENTED();

		return true;   // FIXME
	}
}

using namespace D3D9;

extern "C"
{
	HINSTANCE dllInstance = 0;

	int	__stdcall DllMain(HINSTANCE instance, unsigned long reason, void *reserved)
	{
		#ifndef NDEBUG
			if(dllInstance == 0)
			{
				FILE *file = fopen("debug.txt", "w");   // Clear debug log
				fclose(file);
			}
		#endif

		GTRACE("HINSTANCE instance = 0x%0.8p, unsigned long reason = %d, void *reserved = 0x%0.8p", instance, reason, reserved);

		dllInstance	= instance;

		switch(reason)
		{
		case DLL_PROCESS_DETACH:
			break;
		case DLL_PROCESS_ATTACH:
			DisableThreadLibraryCalls(instance);
			break;
		case DLL_THREAD_ATTACH:
			break;
		case DLL_THREAD_DETACH:
			break;
		default:
			SetLastError(ERROR_INVALID_PARAMETER);
			return FALSE;
		}

		return TRUE;
	}

	IDirect3D9 *__stdcall Direct3DCreate9(unsigned int version)
	{
		GTRACE("");

		// D3D_SDK_VERSION check
		if(version != (31 | 0x80000000) && // 9.0a/b DEBUG_INFO
		   version != 31 &&                // 9.0a/b
		   version != (32 | 0x80000000) && // 9.0c DEBUG_INFO
		   version != 32)                  // 9.0c
		{
			return 0;
		}

		IDirect3D9 *device = new D3D9::Direct3D9(version, dllInstance);

		if(device)
		{
			device->AddRef();
		}

		return device;
	}

	HRESULT __stdcall Direct3DCreate9Ex(unsigned int version, IDirect3D9Ex **device)
	{
		// D3D_SDK_VERSION check
		if(version != (31 | 0x80000000) && // 9.0a/b DEBUG_INFO
		   version != 31 &&                // 9.0a/b
		   version != (32 | 0x80000000) && // 9.0c DEBUG_INFO
		   version != 32)                  // 9.0c
		{
			return NOTAVAILABLE();
		}

		*device = new D3D9::Direct3D9Ex(version, dllInstance);

		if(device)
		{
			(*device)->AddRef();
		}
		else
		{
			return OUTOFMEMORY();
		}

		return D3D_OK;
	}

	int __stdcall CheckFullscreen()
	{
		GTRACE("");

		UNIMPLEMENTED();

		return FALSE;
	}

	int __stdcall D3DPERF_BeginEvent(D3DCOLOR color, const wchar_t *name)
	{
		GTRACE("");

	//	UNIMPLEMENTED();   // PIX unsupported

		return -1;
	}

	int __stdcall D3DPERF_EndEvent()
	{
		GTRACE("");

	//	UNIMPLEMENTED();   // PIX unsupported

		return -1;
	}

	unsigned long __stdcall D3DPERF_GetStatus()
	{
		GTRACE("");

	//	UNIMPLEMENTED();   // PIX unsupported

		return 0;
	}

	int __stdcall D3DPERF_QueryRepeatFrame()
	{
		GTRACE("");

	//	UNIMPLEMENTED();   // PIX unsupported

		return FALSE;
	}

	void __stdcall D3DPERF_SetMarker(D3DCOLOR color, const wchar_t *name)
	{
		GTRACE("");

	//	UNIMPLEMENTED();   // PIX unsupported
	}

	void __stdcall D3DPERF_SetOptions(unsigned long options)
	{
		GTRACE("");

	//	UNIMPLEMENTED();   // PIX unsupported
	}

	void __stdcall D3DPERF_SetRegion(D3DCOLOR color, const wchar_t *name)
	{
		GTRACE("");
	
	//	UNIMPLEMENTED();   // PIX unsupported
	}

	void __cdecl DebugSetLevel(long level)
	{
		GTRACE("long level = %d", level);

	//	UNIMPLEMENTED();   // Debug output unsupported
	}

	void __cdecl DebugSetMute(long mute)
	{
		GTRACE("long mute = %d", mute);

	//	UNIMPLEMENTED();   // Debug output unsupported
	}
	
	void *__stdcall Direct3DShaderValidatorCreate9()
	{
		GTRACE("");

	//	UNIMPLEMENTED();

		return 0;

	//	return new D3D9::Direct3DShaderValidator9();
	}

	void __stdcall PSGPError()
	{
		GTRACE("");

		UNIMPLEMENTED();
	}

	void __stdcall PSGPSampleTexture()
	{
		GTRACE("");

		UNIMPLEMENTED();
	}
}
