#include "OSXUtils.hpp"

#include "common/debug.h"

#import <Cocoa/Cocoa.h>

namespace sw
{
namespace OSX
{
	bool IsValidWindow(EGLNativeWindowType window)
	{
		NSObject *object = reinterpret_cast<NSObject*>(window);
		return window && ([object isKindOfClass:[NSView class]] || [object isKindOfClass:[CALayer class]]);
	}

	void GetNativeWindowSize(EGLNativeWindowType window, int &width, int &height)
	{
		NSObject *object = reinterpret_cast<NSObject*>(window);

		if([object isKindOfClass:[NSView class]])
		{
			NSView *view = reinterpret_cast<NSView*>(object);
			width = [view convertRectToBacking:[view bounds]].size.width;
			height = [view convertRectToBacking:[view bounds]].size.height;
		}
		else if([object isKindOfClass:[CALayer class]])
		{
			CALayer *layer = reinterpret_cast<CALayer*>(object);
			width = [layer bounds].size.width * layer.contentsScale;
			height = [layer bounds].size.height * layer.contentsScale;
		}
		else UNREACHABLE(0);
	}
}
}
