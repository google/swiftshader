// SwiftShader Software Renderer
//
// Copyright(c) 2005-2012 TransGaming Inc.
//
// All rights reserved. No part of this software may be copied, distributed, transmitted,
// transcribed, stored in a retrieval system, translated into any human or computer
// language by any means, or disclosed to third parties without the explicit written
// agreement of TransGaming Inc. Without such an agreement, no rights or licenses, express
// or implied, including but not limited to any patent rights, are granted to you.
//

#ifndef	sw_FrameBufferDD_hpp
#define	sw_FrameBufferDD_hpp

#include "FrameBufferWin.hpp"

#include <ddraw.h>

namespace sw
{
	class FrameBufferDD : public FrameBufferWin
	{
	public:
		FrameBufferDD(HWND windowHandle, int width, int height, bool fullscreen, bool topLeftOrigin);

		virtual ~FrameBufferDD();

		void flip(void *source, Format sourceFormat, size_t sourceStride) override;
		void blit(void *source, const Rect *sourceRect, const Rect *destRect, Format sourceFormat, size_t sourceStride) override;

		void flip(HWND windowOverride, void *source, Format sourceFormat, size_t sourceStride) override;
		void blit(HWND windowOverride, void *source, const Rect *sourceRect, const Rect *destRect, Format sourceFormat, size_t sourceStride) override;

		void *lock() override;
		void unlock() override;

		void setGammaRamp(GammaRamp *gammaRamp, bool calibrate) override;
		void getGammaRamp(GammaRamp *gammaRamp) override;

		void screenshot(void *destBuffer) override;
		bool getScanline(bool &inVerticalBlank, unsigned int &scanline) override;

		void drawText(int x, int y, const char *string, ...);

	private:
		void initFullscreen();
		void initWindowed();
		void createSurfaces();
		bool readySurfaces();
		void updateClipper(HWND windowOverride);
		void restoreSurfaces();
		void releaseAll();

		HMODULE ddraw;
		typedef HRESULT (WINAPI *DIRECTDRAWCREATE)( GUID FAR *lpGUID, LPDIRECTDRAW FAR *lplpDD, IUnknown FAR *pUnkOuter );
		HRESULT (WINAPI *DirectDrawCreate)( GUID FAR *lpGUID, LPDIRECTDRAW FAR *lplpDD, IUnknown FAR *pUnkOuter );
		typedef HRESULT (WINAPI *DIRECTDRAWENUMERATEEXA)( LPDDENUMCALLBACKEXA lpCallback, LPVOID lpContext, DWORD dwFlags);
		HRESULT (WINAPI *DirectDrawEnumerateExA)( LPDDENUMCALLBACKEXA lpCallback, LPVOID lpContext, DWORD dwFlags);

		IDirectDraw *directDraw;
		IDirectDrawSurface *frontBuffer;
		IDirectDrawSurface *backBuffer;
	};
}

#endif	 //	sw_FrameBufferDD_hpp
