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
	Image(sw::Resource *resource, GLsizei width, GLsizei height, GLenum format, GLenum type, sw::Format internalFormat)
		: width(width), height(height), format(format), type(type), internalFormat(internalFormat), multiSampleDepth(1)
		, sw::Surface(resource, width, height, 1, internalFormat, true, true)
	{
		shared = false;
	}

	Image(sw::Resource *resource, int width, int height, int depth, sw::Format internalFormat, bool lockable, bool renderTarget)
		: width(width), height(height), format(0 /*GL_NONE*/), type(0 /*GL_NONE*/), internalFormat(internalFormat), multiSampleDepth(depth)
		, sw::Surface(resource, width, height, depth, internalFormat, lockable, renderTarget)
	{
		shared = false;
	}

	GLsizei getWidth()
	{
		return width;
	}

	GLsizei getHeight()
	{
		return height;
	}

	GLenum getFormat()
	{
		return format;
	}
	
	GLenum getType()
	{
		return type;
	}

	sw::Format getInternalFormat()
	{
		return internalFormat;
	}

	int getMultiSampleDepth()
	{
		return multiSampleDepth;
	}

	bool isShared() const
    {
        return shared;
    }

    void markShared()
    {
        shared = true;
    }

	void *lock(unsigned int left, unsigned int top, sw::Lock lock)
	{
		return lockExternal(left, top, 0, lock, sw::PUBLIC);
	}

	unsigned int getPitch() const
	{
		return getExternalPitchB();
	}

	void unlock()
	{
		unlockExternal();
	}

	virtual void addRef() = 0;
	virtual void release() = 0;
	virtual void unbind() = 0;   // Break parent ownership and release

	virtual void loadImageData(GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, GLint unpackAlignment, const void *input) = 0;
	virtual void loadCompressedData(GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLsizei imageSize, const void *pixels) = 0;

protected:
	const GLsizei width;
	const GLsizei height;
	const GLenum format;
	const GLenum type;
	const sw::Format internalFormat;
	const int multiSampleDepth;

private:
	bool shared;   // Used as an EGLImage
};
}

#endif   // egl_Image_hpp
