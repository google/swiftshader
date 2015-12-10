#ifndef	sw_OSXUtils_hpp
#define	sw_OSXUtils_hpp

#include <EGL/egl.h>

namespace sw
{
namespace OSX
{
	bool IsValidWindow(EGLNativeWindowType window);
	void GetNativeWindowSize(EGLNativeWindowType window, int &width, int &height);
}
}

#endif // sw_OSXUtils_hpp
