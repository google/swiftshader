#ifndef egl_Context_hpp
#define egl_Context_hpp

#include "common/Object.hpp"

#include <EGL/egl.h>
#include <GLES/gl.h>

namespace egl
{
class Surface;
class Image;

class Context : public gl::Object
{
public:
	virtual void makeCurrent(Surface *surface) = 0;
	virtual void bindTexImage(Surface *surface) = 0;
	virtual EGLenum validateSharedImage(EGLenum target, GLuint name, GLuint textureLevel) = 0;
	virtual Image *createSharedImage(EGLenum target, GLuint name, GLuint textureLevel) = 0;
	virtual int getClientVersion() const = 0;
	virtual void finish() = 0;

protected:
	virtual ~Context() {};
};
}

#endif   // egl_Context_hpp
