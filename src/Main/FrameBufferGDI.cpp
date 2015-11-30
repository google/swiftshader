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

#include "FrameBufferGDI.hpp"

#include "Debug.hpp"

namespace sw
{
	extern bool forceWindowed;

	FrameBufferGDI::FrameBufferGDI(HWND windowHandle, int width, int height, bool fullscreen, bool topLeftOrigin) : FrameBufferWin(windowHandle, width, height, fullscreen, topLeftOrigin)
	{
		if(!windowed)
		{
			SetWindowPos(windowHandle, HWND_TOPMOST, 0, 0, width, height, SWP_SHOWWINDOW);

			DEVMODE deviceMode;
			deviceMode.dmSize = sizeof(DEVMODE);
			deviceMode.dmPelsWidth= width;
			deviceMode.dmPelsHeight = height;
			deviceMode.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT;	

			ChangeDisplaySettings(&deviceMode, CDS_FULLSCREEN);
		}

		init(this->windowHandle);

		destFormat = FORMAT_X8R8G8B8;
	}

	FrameBufferGDI::~FrameBufferGDI()
	{
		release();

		if(!windowed)
		{
			ChangeDisplaySettings(0, 0);

			RECT clientRect;
			RECT windowRect;
			GetClientRect(windowHandle, &clientRect);
			GetWindowRect(windowHandle, &windowRect);
			int windowWidth = width + (windowRect.right - windowRect.left) - (clientRect.right - clientRect.left);
			int windowHeight = height + (windowRect.bottom - windowRect.top) - (clientRect.bottom - clientRect.top);
			int desktopWidth = GetSystemMetrics(SM_CXSCREEN);
			int desktopHeight = GetSystemMetrics(SM_CYSCREEN);
			SetWindowPos(windowHandle, HWND_TOP, desktopWidth / 2 - windowWidth / 2, desktopHeight / 2 - windowHeight / 2, windowWidth, windowHeight, SWP_SHOWWINDOW);
		}
	}

	void *FrameBufferGDI::lock()
	{
		stride = width * 4;

		return locked;
	}

	void FrameBufferGDI::unlock()
	{
	}

	void FrameBufferGDI::flip(void *source, Format sourceFormat, size_t sourceStride)
	{
		blit(source, 0, 0, sourceFormat, sourceStride);
	}
	
	void FrameBufferGDI::blit(void *source, const Rect *sourceRect, const Rect *destRect, Format sourceFormat, size_t sourceStride)
	{
		copy(source, sourceFormat, sourceStride);

		int sourceLeft = sourceRect ? sourceRect->x0 : 0;
		int sourceTop = sourceRect ? sourceRect->y0 : 0;
		int sourceWidth = sourceRect ? sourceRect->x1 - sourceRect->x0 : width;
		int sourceHeight = sourceRect ? sourceRect->y1 - sourceRect->y0 : height;
		int destLeft = destRect ? destRect->x0 : 0;
		int destTop = destRect ? destRect->y0 : 0;
		int destWidth = destRect ? destRect->x1 - destRect->x0 : bounds.right - bounds.left;
		int destHeight = destRect ? destRect->y1 - destRect->y0 : bounds.bottom - bounds.top;

		StretchBlt(windowContext, destLeft, destTop, destWidth, destHeight, bitmapContext, sourceLeft, sourceTop, sourceWidth, sourceHeight, SRCCOPY);
	}

	void FrameBufferGDI::flip(HWND windowOverride, void *source, Format sourceFormat, size_t sourceStride)
	{
		blit(windowOverride, source, 0, 0, sourceFormat, sourceStride);
	}

	void FrameBufferGDI::blit(HWND windowOverride, void *source, const Rect *sourceRect, const Rect *destRect, Format sourceFormat, size_t sourceStride)
	{
		if(windowed && windowOverride != 0 && windowOverride != bitmapWindow)
		{
			release();
			init(windowOverride);
		}

		blit(source, sourceRect, destRect, sourceFormat, sourceStride);
	}

	void FrameBufferGDI::setGammaRamp(GammaRamp *gammaRamp, bool calibrate)
	{
		SetDeviceGammaRamp(windowContext, gammaRamp);
	}

	void FrameBufferGDI::getGammaRamp(GammaRamp *gammaRamp)
	{
		GetDeviceGammaRamp(windowContext, gammaRamp);
	}

	void FrameBufferGDI::screenshot(void *destBuffer)
	{
		UNIMPLEMENTED();
	}

	bool FrameBufferGDI::getScanline(bool &inVerticalBlank, unsigned int &scanline)
	{
		UNIMPLEMENTED();

		return false;
	}

	void FrameBufferGDI::init(HWND window)
	{
		bitmapWindow = window;

		windowContext = GetDC(window);
		bitmapContext = CreateCompatibleDC(windowContext);

		BITMAPINFO bitmapInfo;
		memset(&bitmapInfo, 0, sizeof(BITMAPINFO));
		bitmapInfo.bmiHeader.biSize = sizeof(BITMAPINFO);
		bitmapInfo.bmiHeader.biBitCount = 32;
		bitmapInfo.bmiHeader.biPlanes = 1;
		bitmapInfo.bmiHeader.biHeight = -height;
		bitmapInfo.bmiHeader.biWidth = width;
		bitmapInfo.bmiHeader.biCompression = BI_RGB;
		
		bitmap = CreateDIBSection(bitmapContext, &bitmapInfo, DIB_RGB_COLORS, &locked, 0, 0);
		SelectObject(bitmapContext, bitmap);

		updateBounds(window);
	}

	void FrameBufferGDI::release()
	{
		SelectObject(bitmapContext, 0);
		DeleteObject(bitmap);
		ReleaseDC(bitmapWindow, windowContext);
		DeleteDC(bitmapContext);
	}
}
