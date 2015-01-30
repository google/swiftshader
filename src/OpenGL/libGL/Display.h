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

// Display.h: Defines the Display class, representing the abstract
// display on which graphics are drawn.

#ifndef INCLUDE_DISPLAY_H_
#define INCLUDE_DISPLAY_H_

#include "Surface.h"
#include "Context.h"
#include "Device.hpp"

#include <set>

namespace gl
{
	struct DisplayMode
	{
		unsigned int width;
		unsigned int height;
		sw::Format format;
	};

	class Display
	{
	public:
		~Display();

		static Display *getDisplay(NativeDisplayType displayId);

		bool initialize();
		void terminate();

		Context *createContext(const Context *shareContext);

		void destroySurface(Surface *surface);
		void destroyContext(Context *context);

		bool isInitialized() const;
		bool isValidContext(Context *context);
		bool isValidSurface(Surface *surface);
		bool isValidWindow(NativeWindowType window);

		GLint getMinSwapInterval();
		GLint getMaxSwapInterval();

        virtual Surface *getPrimarySurface();

		NativeDisplayType getNativeDisplay() const;

	private:
		Display(NativeDisplayType displayId);
		
		DisplayMode getDisplayMode() const;

		const NativeDisplayType displayId;

		GLint mMaxSwapInterval;
		GLint mMinSwapInterval;
    
		typedef std::set<Surface*> SurfaceSet;
		SurfaceSet mSurfaceSet;

		typedef std::set<Context*> ContextSet;
		ContextSet mContextSet;
	};
}

#endif   // INCLUDE_DISPLAY_H_
