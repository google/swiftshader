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

#ifndef	sw_FrameBufferGDI_hpp
#define	sw_FrameBufferGDI_hpp

#include "FrameBuffer.hpp"

namespace sw
{
	class FrameBufferGDI : public FrameBuffer
	{
	public:
		FrameBufferGDI(HWND windowHandle, int width, int height, bool fullscreen, bool topLeftOrigin);

		virtual ~FrameBufferGDI();

		void *lock();
		void unlock();

		void flip(HWND windowOverride, void *source, bool HDR);
		void blit(HWND windowOverride, void *source, const Rect *sourceRect, const Rect *destRect, bool HDR);

		void setGammaRamp(GammaRamp *gammaRamp, bool calibrate);
		void getGammaRamp(GammaRamp *gammaRamp);

		void screenshot(void *destBuffer);
		bool getScanline(bool &inVerticalBlank, unsigned int &scanline);

	private:
		void init(HWND bitmapWindow);
		void release();

		HDC windowContext;
		HDC bitmapContext;
		HWND bitmapWindow;
		
		HBITMAP bitmap;
	};
}

#endif	 //	sw_FrameBufferGDI_hpp
