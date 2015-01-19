#ifndef egl_Image_hpp
#define egl_Image_hpp

#include "Renderer/Surface.hpp"

#include <assert.h>

namespace egl
{
// Types common between gl.h and gl2.h
// We can't include either header in EGL
typedef unsigned int GLenum;
typedef int GLint;
typedef int GLsizei;

class Texture;

class Image : public sw::Surface
{
public:
	Image(sw::Resource *resource, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, sw::Format internalFormat)
		: width(width), height(height), format(format), type(type), internalFormat(internalFormat), depth(depth)
		, sw::Surface(resource, width, height, depth, internalFormat, true, true)
	{
		shared = false;
	}

	Image(sw::Resource *resource, int width, int height, int depth, sw::Format internalFormat, bool lockable, bool renderTarget)
		: width(width), height(height), format(0 /*GL_NONE*/), type(0 /*GL_NONE*/), internalFormat(internalFormat), depth(depth)
		, sw::Surface(resource, width, height, depth, internalFormat, lockable, renderTarget)
	{
		shared = false;
	}

	GLsizei getWidth() const
	{
		return width;
	}

	GLsizei getHeight() const
	{
		return height;
	}

	int getDepth() const
	{
		// FIXME: add member if the depth dimension (for 3D textures or 2D testure arrays)
		// and multi sample depth are ever simultaneously required.
		return depth;
	}

	GLenum getFormat() const
	{
		return format;
	}

	GLenum getType() const
	{
		return type;
	}

	sw::Format getInternalFormat() const
	{
		return internalFormat;
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
	virtual void unbind(const Texture *parent) = 0;   // Break parent ownership and release

	void destroyShared()   // Release a shared image
    {
		assert(shared);
        shared = false;
		release();
    }

	virtual void loadImageData(GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, GLint unpackAlignment, const void *input) = 0;
	virtual void loadCompressedData(GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLsizei imageSize, const void *pixels) = 0;

protected:
	virtual ~Image()
	{
	}

	const GLsizei width;
	const GLsizei height;
	const GLenum format;
	const GLenum type;
	const sw::Format internalFormat;
	const int depth;

	bool shared;   // Used as an EGLImage
};
}

#endif   // egl_Image_hpp
