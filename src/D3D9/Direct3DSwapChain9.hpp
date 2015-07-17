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

#ifndef D3D9_Direct3DSwapChain9_hpp
#define D3D9_Direct3DSwapChain9_hpp

#include "Unknown.hpp"

#include "Direct3DSurface9.hpp"

#include "FrameBufferWin.hpp"

#include <d3d9.h>

namespace D3D9
{
	class Direct3DDevice9;

	class Direct3DSwapChain9 : public IDirect3DSwapChain9, public Unknown
	{
	public:
		Direct3DSwapChain9(Direct3DDevice9 *device, D3DPRESENT_PARAMETERS *presentParameters);

		virtual ~Direct3DSwapChain9();

		// IUnknown methods
		long __stdcall QueryInterface(const IID &iid, void **object);
		unsigned long __stdcall AddRef();
		unsigned long __stdcall Release();

		// IDirect3DSwapChain9 methods
		long __stdcall Present(const RECT *sourceRect, const RECT *destRect, HWND destWindowOverride, const RGNDATA *dirtyRegion, unsigned long flags);
		long __stdcall GetFrontBufferData(IDirect3DSurface9 *destSurface);
		long __stdcall GetBackBuffer(unsigned int index, D3DBACKBUFFER_TYPE type, IDirect3DSurface9 **backBuffer);
		long __stdcall GetRasterStatus(D3DRASTER_STATUS *rasterStatus);
		long __stdcall GetDisplayMode(D3DDISPLAYMODE *displayMode);
		long __stdcall GetDevice(IDirect3DDevice9 **device);
		long __stdcall GetPresentParameters(D3DPRESENT_PARAMETERS *presentParameters);

		// Internal methods
		void reset(D3DPRESENT_PARAMETERS *presentParameters);

		void setGammaRamp(sw::GammaRamp *gammaRamp, bool calibrate);
		void getGammaRamp(sw::GammaRamp *gammaRamp);

		void *lockBackBuffer(int index);
		void unlockBackBuffer(int index);

	private:
		void release();

		// Creation parameters
		Direct3DDevice9 *const device;
		D3DPRESENT_PARAMETERS presentParameters;

		bool lockable;

		sw::FrameBufferWin *frameBuffer;

	public:   // FIXME
		Direct3DSurface9 *backBuffer[3];
	};
}

#endif // D3D9_Direct3DSwapChain9_hpp
