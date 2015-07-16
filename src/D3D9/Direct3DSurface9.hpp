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

#ifndef D3D9_Direct3DSurface9_hpp
#define D3D9_Direct3DSurface9_hpp

#include "Direct3DResource9.hpp"

#include "Surface.hpp"

#include <d3d9.h>

namespace D3D9
{
	class Direct3DBaseTexture9;

	class Direct3DSurface9 : public IDirect3DSurface9, public Direct3DResource9, public sw::Surface
	{
	public:
		Direct3DSurface9(Direct3DDevice9 *device, Unknown *container, int width, int height, D3DFORMAT format, D3DPOOL pool, D3DMULTISAMPLE_TYPE multiSample, unsigned int quality, bool lockableOverride, unsigned long usage);

		virtual ~Direct3DSurface9();

		// IUnknown methods
		long __stdcall QueryInterface(const IID &iid, void **object);
		unsigned long __stdcall AddRef();
		unsigned long __stdcall Release();

		// IDirect3DResource9 methods
		long __stdcall FreePrivateData(const GUID &guid);
		long __stdcall GetPrivateData(const GUID &guid, void *data, unsigned long *size);
		void __stdcall PreLoad();
		long __stdcall SetPrivateData(const GUID &guid, const void *data, unsigned long size, unsigned long flags);
		long __stdcall GetDevice(IDirect3DDevice9 **device);
		unsigned long __stdcall SetPriority(unsigned long newPriority);
		unsigned long __stdcall GetPriority();
		D3DRESOURCETYPE __stdcall GetType();

		// IDirect3DSurface9 methods
		long __stdcall GetDC(HDC *deviceContext);
		long __stdcall ReleaseDC(HDC deviceContext);
		long __stdcall LockRect(D3DLOCKED_RECT *lockedRect, const RECT *rect, unsigned long Flags);
		long __stdcall UnlockRect();
		long __stdcall GetContainer(const IID &iid, void **container);
		long __stdcall GetDesc(D3DSURFACE_DESC *desc);

		// Internal methods
		static sw::Format translateFormat(D3DFORMAT format);
		static int bytes(D3DFORMAT format);

	private:
		static unsigned int memoryUsage(int width, int height, D3DFORMAT format);

		// Creation parameters
		Unknown *const container;
		const int width;
		const int height;
		const D3DFORMAT format;
		const D3DMULTISAMPLE_TYPE multiSample;
		const unsigned int quality;
		const D3DPOOL pool;
		const bool lockable;
		const unsigned long usage;

		Direct3DBaseTexture9 *parentTexture;
	};
}

#endif // D3D9_Direct3DSurface9_hpp
