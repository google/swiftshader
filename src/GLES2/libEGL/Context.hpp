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
	virtual void bindTexImage(Surface *surface) = 0;
	virtual EGLenum validateSharedImage(EGLenum target, GLuint name, GLuint textureLevel) = 0;
	virtual Image *createSharedImage(EGLenum target, GLuint name, GLuint textureLevel) = 0;
};
}

#endif   // egl_Context_hpp
