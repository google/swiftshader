#ifndef egl_Context_hpp
#define egl_Context_hpp

#define EGLAPI
#include <EGL/egl.h>
#define GL_API
#include <GLES/gl.h>

namespace egl
{
class Surface;
class Image;

class Context
{
public:
	virtual void destroy() = 0;
	virtual void makeCurrent(Surface *surface) = 0;
	virtual int getClientVersion() = 0;
};
}

#endif   // egl_Context_hpp
