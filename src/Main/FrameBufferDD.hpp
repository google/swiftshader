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

		virtual void flip(void *source, Format format);
		virtual void blit(void *source, const Rect *sourceRect, const Rect *destRect, Format format);

		virtual void flip(HWND windowOverride, void *source, Format format);
		virtual void blit(HWND windowOverride, void *source, const Rect *sourceRect, const Rect *destRect, Format format);

		virtual void *lock();
		virtual void unlock();

		virtual void setGammaRamp(GammaRamp *gammaRamp, bool calibrate);
		virtual void getGammaRamp(GammaRamp *gammaRamp);

		virtual void screenshot(void *destBuffer);
		virtual bool getScanline(bool &inVerticalBlank, unsigned int &scanline);

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
