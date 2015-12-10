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
			width = [view bounds].size.width;
			height = [view bounds].size.height;
		}
		else if([object isKindOfClass:[CALayer class]])
		{
			CALayer *layer = reinterpret_cast<CALayer*>(object);
			width = CGRectGetWidth([layer frame]);
			height = CGRectGetHeight([layer frame]);
		}
		else UNREACHABLE(0);
	}
}
}
