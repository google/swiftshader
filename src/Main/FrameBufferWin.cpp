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

#include "FrameBufferWin.hpp"

namespace sw
{
	FrameBufferWin::FrameBufferWin(HWND windowHandle, int width, int height, bool fullscreen, bool topLeftOrigin) : FrameBuffer(width, height, fullscreen, topLeftOrigin), windowHandle(windowHandle)
	{
		if(!windowed)
		{
			// Force fullscreen window style (no borders)
			originalWindowStyle = GetWindowLong(windowHandle, GWL_STYLE);
			SetWindowLong(windowHandle, GWL_STYLE, WS_POPUP);
		}
	}

	FrameBufferWin::~FrameBufferWin()
	{
		if(!windowed && GetWindowLong(windowHandle, GWL_STYLE) == WS_POPUP)
		{
			SetWindowLong(windowHandle, GWL_STYLE, originalWindowStyle);
		}
	}

	void FrameBufferWin::updateBounds(HWND windowOverride)
	{
		HWND window = windowOverride ? windowOverride : windowHandle;

		if(windowed)
		{
			GetClientRect(window, &bounds);
			ClientToScreen(window, (POINT*)&bounds);
			ClientToScreen(window, (POINT*)&bounds + 1);
		}
		else
		{
			SetRect(&bounds, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
		}
	}
}

#include "FrameBufferDD.hpp"
#include "FrameBufferGDI.hpp"
#include "Common/Configurator.hpp"

extern "C"
{
	sw::FrameBufferWin *createFrameBufferWin(HWND windowHandle, int width, int height, bool fullscreen, bool topLeftOrigin)
	{
		sw::Configurator ini("SwiftShader.ini");
		int api = ini.getInteger("Testing", "FrameBufferAPI", 0);

		if(api == 0 && topLeftOrigin)
		{
			return new sw::FrameBufferDD(windowHandle, width, height, fullscreen, topLeftOrigin);
		}
		else
		{
			return new sw::FrameBufferGDI(windowHandle, width, height, fullscreen, topLeftOrigin);
		}

		return 0;
	}

	sw::FrameBuffer *createFrameBuffer(HDC display, HWND window, int width, int height)
	{
		return createFrameBufferWin(window, width, height, false, false);
	}
}
