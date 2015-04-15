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

#ifndef	sw_FrameBufferWin_hpp
#define	sw_FrameBufferWin_hpp

#include "FrameBuffer.hpp"

namespace sw
{
	struct GammaRamp
	{
		short red[256];
		short green[256];
		short blue[256];
	};

	class FrameBufferWin : public FrameBuffer
	{
	public:
		FrameBufferWin(HWND windowHandle, int width, int height, bool fullscreen, bool topLeftOrigin);

		virtual ~FrameBufferWin();

		virtual void flip(void *source, Format format) = 0;
		virtual void blit(void *source, const Rect *sourceRect, const Rect *destRect, Format format) = 0;

		virtual void flip(HWND windowOverride, void *source, Format format) = 0;
		virtual void blit(HWND windowOverride, void *source, const Rect *sourceRect, const Rect *destRect, Format format) = 0;

		virtual void *lock() = 0;
		virtual void unlock() = 0;

		virtual void setGammaRamp(GammaRamp *gammaRamp, bool calibrate) = 0;
		virtual void getGammaRamp(GammaRamp *gammaRamp) = 0;

		virtual void screenshot(void *destBuffer) = 0;
		virtual bool getScanline(bool &inVerticalBlank, unsigned int &scanline) = 0;

	protected:
		void updateBounds(HWND windowOverride);

		HWND windowHandle;
		DWORD originalWindowStyle;
		RECT bounds;
	};
}

#endif	 //	sw_FrameBufferWin_hpp
