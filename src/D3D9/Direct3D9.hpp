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

#ifndef D3D9_Direct3D9_hpp
#define D3D9_Direct3D9_hpp

#include "Unknown.hpp"

#include <stdio.h>
#include <d3d9.h>

namespace D3D9
{
	class Direct3DDevice9;

	class Direct3D9 : public IDirect3D9, public Unknown
	{
	public:
		Direct3D9(int version, const HINSTANCE instance);

		virtual ~Direct3D9();

		// IUnknown methods
		long __stdcall QueryInterface(const IID &iid, void **object);
		unsigned long __stdcall AddRef();
		unsigned long __stdcall Release();

		// IDirect3D9 methods
		long __stdcall RegisterSoftwareDevice(void *initializeFunction);
		unsigned int __stdcall GetAdapterCount();
		long __stdcall GetAdapterIdentifier(unsigned int adapter, unsigned long flags, D3DADAPTER_IDENTIFIER9 *identifier);
		unsigned int __stdcall GetAdapterModeCount(unsigned int adapter, D3DFORMAT format);
		long __stdcall EnumAdapterModes(unsigned int adapter, D3DFORMAT format, unsigned int index, D3DDISPLAYMODE *mode);
		long __stdcall GetAdapterDisplayMode(unsigned int adapter, D3DDISPLAYMODE *mode);
		long __stdcall CheckDeviceType(unsigned int adapter, D3DDEVTYPE checkType, D3DFORMAT displayFormat, D3DFORMAT backBufferFormat, int windowed);
		long __stdcall CheckDeviceFormat(unsigned int adapter, D3DDEVTYPE deviceType, D3DFORMAT adapterFormat, unsigned long usage, D3DRESOURCETYPE type, D3DFORMAT checkFormat);
		long __stdcall CheckDeviceMultiSampleType(unsigned int adapter, D3DDEVTYPE deviceType, D3DFORMAT surfaceFormat, int windowed, D3DMULTISAMPLE_TYPE multiSampleType, unsigned long *qualityLevels);
		long __stdcall CheckDepthStencilMatch(unsigned int adapter, D3DDEVTYPE deviceType, D3DFORMAT adapterFormat, D3DFORMAT renderTargetFormat, D3DFORMAT depthStencilFormat);
		long __stdcall CheckDeviceFormatConversion(unsigned int adapter, D3DDEVTYPE deviceType, D3DFORMAT sourceFormat, D3DFORMAT targetFormat);
		long __stdcall GetDeviceCaps(unsigned int adapter, D3DDEVTYPE deviceType, D3DCAPS9 *caps);
		HMONITOR __stdcall GetAdapterMonitor(unsigned int adapter);
		long __stdcall CreateDevice(unsigned int adapter, D3DDEVTYPE deviceType, HWND focusWindow, unsigned long behaviorFlags, D3DPRESENT_PARAMETERS *presentParameters, IDirect3DDevice9 **returnedDeviceInterface);

	protected:
		// Creation parameters
		const int version;
		const HINSTANCE instance;

		// Real D3D9 library
		HMODULE d3d9Lib;

	private:
		void loadSystemD3D9();

		DEVMODE *displayMode;
		int numDisplayModes;
		bool disableAlphaMode;   // Disable A8R8G8B8 display mode support
		bool disable10BitMode;   // Disable A2R10G10B10 display mode support

		//Real IDirect3D9 object
		IDirect3D9 *d3d9;
	};
}

#endif   // D3D9_Direct3D9_hpp
