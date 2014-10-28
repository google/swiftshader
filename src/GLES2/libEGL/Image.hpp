#ifndef egl_Image_hpp
#define egl_Image_hpp

#include "Renderer/Surface.hpp"

namespace egl
{
// Types common between gl.h and gl2.h
// We can't include either header in EGL
typedef unsigned int GLenum;
typedef int GLint;
typedef int GLsizei;

class Image : public sw::Surface
{
public:
	Image(sw::Resource *texture, int width, int height, int depth, sw::Format format, bool lockable, bool renderTarget)
		: sw::Surface(texture, width, height, depth, format, lockable, renderTarget)
	{
	}

	virtual void loadImageData(GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, GLint unpackAlignment, const void *input) = 0;
	virtual void loadCompressedData(GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLsizei imageSize, const void *pixels) = 0;

	virtual void *lock(unsigned int left, unsigned int top, sw::Lock lock) = 0;
	virtual unsigned int getPitch() const = 0;
	virtual void unlock() = 0;

	virtual int getWidth() = 0;
	virtual int getHeight() = 0;
	virtual GLenum getFormat() = 0;
	virtual GLenum getType() = 0;
	virtual sw::Format getInternalFormat() = 0;
	virtual int getMultiSampleDepth() = 0;

	virtual void addRef() = 0;
	virtual void release() = 0;
	virtual void unbind() = 0;   // Break parent ownership and release

	virtual bool isShared() const = 0;
	virtual void markShared() = 0;
};
}

#endif   // egl_Image_hpp
