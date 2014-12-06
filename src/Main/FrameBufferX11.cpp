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

#include "FrameBufferX11.hpp"

#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>
#include <assert.h>

namespace sw
{
	static int (*PreviousXErrorHandler)(Display *display, XErrorEvent *event) = 0;
	static bool shmBadAccess = false;

	// Catches BadAcces errors so we can fall back to not using MIT-SHM
	static int XShmErrorHandler(Display *display, XErrorEvent *event)
	{
		if(event->error_code == BadAccess)
		{
			shmBadAccess = true;
			return 0;
		}
		else
		{
			return PreviousXErrorHandler(display, event);
		}
	}

	FrameBufferX11::FrameBufferX11(Display *display, Window window, int width, int height) : FrameBuffer(width, height, false, false), x_window(window), x_display(display), ownX11(!display)
	{
		if(!x_display)
		{
			x_display = XOpenDisplay(0);
		}

		int screen = DefaultScreen(x_display);
		x_gc = XDefaultGC(x_display, screen);
		int depth = XDefaultDepth(x_display, screen);

		Status status = XMatchVisualInfo(x_display, screen, 32, TrueColor, &x_visual);
		bool match = (status != 0 && x_visual.blue_mask == 0xFF);   // Prefer X8R8G8B8
		Visual *visual = match ? x_visual.visual : XDefaultVisual(x_display, screen);

		mit_shm = (XShmQueryExtension(x_display) == True);

		if(mit_shm)
		{
			x_image = XShmCreateImage(x_display, visual, depth, ZPixmap, 0, &shminfo, width, height);

			shminfo.shmid = shmget(IPC_PRIVATE, x_image->bytes_per_line * x_image->height, IPC_CREAT | SHM_R | SHM_W);
			shminfo.shmaddr = x_image->data = buffer = (char*)shmat(shminfo.shmid, 0, 0);
			shminfo.readOnly = False;

			PreviousXErrorHandler = XSetErrorHandler(XShmErrorHandler);
			XShmAttach(x_display, &shminfo);   // May produce a BadAccess error
			XSync(x_display, False);
			XSetErrorHandler(PreviousXErrorHandler);

			if(shmBadAccess)
			{
				mit_shm = false;

				XDestroyImage(x_image);
				shmdt(shminfo.shmaddr);
				shmctl(shminfo.shmid, IPC_RMID, 0);

				shmBadAccess = false;
			}
		}

		if(!mit_shm)
		{
			buffer = new char[width * height * 4];
			x_image = XCreateImage(x_display, visual, depth, ZPixmap, 0, buffer, width, height, 32, width * 4);
		}
	}

	FrameBufferX11::~FrameBufferX11()
	{
		if(!mit_shm)
		{
			x_image->data = 0;
			XDestroyImage(x_image);

			delete[] buffer;
			buffer = 0;
		}
		else
		{
			XShmDetach(x_display, &shminfo);
			XDestroyImage(x_image);
			shmdt(shminfo.shmaddr);
			shmctl(shminfo.shmid, IPC_RMID, 0);
		}

		if(ownX11)
		{
			XCloseDisplay(x_display);
		}
	}

	void *FrameBufferX11::lock()
	{
		stride = x_image->bytes_per_line;
		locked = buffer;

		return locked;
	}

	void FrameBufferX11::unlock()
	{
		locked = 0;
	}

	void FrameBufferX11::blit(void *source, const Rect *sourceRect, const Rect *destRect, Format format)
	{
		copy(source, format);

		if(!mit_shm)
		{
			XPutImage(x_display, x_window, x_gc, x_image, 0, 0, 0, 0, width, height);
		}
		else
		{
			XShmPutImage(x_display, x_window, x_gc, x_image, 0, 0, 0, 0, width, height, False);
		}

		XSync(x_display, False);
	}
}

extern "C"
{
	sw::FrameBuffer *createFrameBuffer(void *display, Window window, int width, int height)
	{
		return new sw::FrameBufferX11((Display*)display, window, width, height);
	}
}
