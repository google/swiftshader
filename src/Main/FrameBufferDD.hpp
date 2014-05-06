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

#ifndef	sw_FrameBufferDD_hpp
#define	sw_FrameBufferDD_hpp

#include "FrameBuffer.hpp"

#include <ddraw.h>

namespace sw
{
	class FrameBufferDD : public FrameBuffer
	{
	public:
		FrameBufferDD(HWND windowHandle, int width, int height, bool fullscreen);

		virtual ~FrameBufferDD();

		void *lock();
		void unlock();

		void flip(HWND windowOverride, void *source, bool HDR);
		void blit(HWND windowOverride, void *source, const Rect *sourceRect, const Rect *destRect, bool HDR);

		void screenshot(void *destBuffer);
		void setGammaRamp(GammaRamp *gammaRamp, bool calibrate);
		void getGammaRamp(GammaRamp *gammaRamp);

		void drawText(int x, int y, const char *string, ...);
		bool getScanline(bool &inVerticalBlank, unsigned int &scanline);

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
