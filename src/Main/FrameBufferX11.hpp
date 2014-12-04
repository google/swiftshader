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

// Surface.cpp: Implements the egl::Surface class, representing a drawing surface
// such as the client area of a window, including any back buffers.
// Implements EGLSurface and related functionality. [EGL 1.4] section 2.2 page 3.

#ifndef sw_FrameBufferX11_hpp
#define sw_FrameBufferX11_hpp

#include "Main/FrameBuffer.hpp"
#include "Common/Debug.hpp"

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/XShm.h>

namespace sw
{
	class FrameBufferX11 : public FrameBuffer
	{
	public:
		FrameBufferX11(Display *display, Window window, int width, int height);

		~FrameBufferX11();

		virtual void flip(void *source, Format format) {blit(source, 0, 0, format);};
		virtual void blit(void *source, const Rect *sourceRect, const Rect *destRect, Format format);

		virtual void *lock();
		virtual void unlock();

	private:
		bool ownX11;
		Display *x_display;
		Window x_window;
		XImage *x_image;
		GC x_gc;
		XVisualInfo x_visual;

		bool mit_shm;
		XShmSegmentInfo shminfo;

		char *buffer;
	};
}

#endif   // sw_FrameBufferX11_hpp
