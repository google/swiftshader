#ifndef egl_Image_hpp
#define egl_Image_hpp

#include "Renderer/Surface.hpp"

#if defined(__ANDROID__)
#include <hardware/gralloc.h>
#include <system/window.h>
#include "../../Common/GrallocAndroid.hpp"
#endif

#ifdef __ANDROID__
#include "../../Common/DebugAndroid.hpp"
#else
#include <assert.h>
#endif

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
		: sw::Surface(resource, width, height, depth, internalFormat, true, true),
		  width(width), height(height), format(format), type(type), internalFormat(internalFormat), depth(depth)
	{
		shared = false;

		#if defined(__ANDROID__)
			nativeBuffer = 0;
		#endif
	}

	Image(sw::Resource *resource, int width, int height, int depth, sw::Format internalFormat, bool lockable, bool renderTarget)
		: sw::Surface(resource, width, height, depth, internalFormat, lockable, renderTarget),
		  width(width), height(height), format(0 /*GL_NONE*/), type(0 /*GL_NONE*/), internalFormat(internalFormat), depth(depth)
	{
		shared = false;

		#if defined(__ANDROID__)
			nativeBuffer = 0;
		#endif
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
		#if defined(__ANDROID__)
			if(nativeBuffer)   // Lock the buffer from ANativeWindowBuffer
			{
				return lockNativeBuffer(GRALLOC_USAGE_SW_READ_OFTEN | GRALLOC_USAGE_SW_WRITE_OFTEN);
			}
		#endif

		return lockExternal(left, top, 0, lock, sw::PUBLIC);
	}

	unsigned int getPitch() const
	{
		return getExternalPitchB();
	}

	void unlock()
	{
		#if defined(__ANDROID__)
			if(nativeBuffer)   // Unlock the buffer from ANativeWindowBuffer
			{
				return unlockNativeBuffer();
			}
		#endif

		unlockExternal();
	}

	virtual void addRef() = 0;
	virtual void release() = 0;
	virtual void unbind(const Texture *parent) = 0;   // Break parent ownership and release

	void destroyShared()   // Release a shared image
	{
		#if defined(__ANDROID__)
			if(nativeBuffer)
			{
				nativeBuffer->common.decRef(&nativeBuffer->common);
			}
		#endif

		assert(shared);
		shared = false;
		release();
	}

	virtual void loadImageData(GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, GLint unpackAlignment, const void *input) = 0;
	virtual void loadCompressedData(GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLsizei imageSize, const void *pixels) = 0;

	#if defined(__ANDROID__)
	void setNativeBuffer(ANativeWindowBuffer* buffer)
	{
		nativeBuffer = buffer;
		nativeBuffer->common.incRef(&nativeBuffer->common);
	}

	virtual void *lockInternal(int x, int y, int z, sw::Lock lock, sw::Accessor client)
	{
		if(nativeBuffer)   // Lock the buffer from ANativeWindowBuffer
		{
			return lockNativeBuffer(GRALLOC_USAGE_SW_READ_OFTEN | GRALLOC_USAGE_SW_WRITE_OFTEN);
		}
		return sw::Surface::lockInternal(x, y, z, lock, client);
	}

	virtual void unlockInternal()
	{
		if(nativeBuffer)   // Unlock the buffer from ANativeWindowBuffer
		{
			return unlockNativeBuffer();
		}
		return sw::Surface::unlockInternal();
	}
	#endif

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

	#if defined(__ANDROID__)
	ANativeWindowBuffer *nativeBuffer;

	void* lockNativeBuffer(int usage)
	{
		void *buffer = 0;
		GrallocModule::getInstance()->lock(
			nativeBuffer->handle, usage, 0, 0,
			nativeBuffer->width, nativeBuffer->height, &buffer);
		return buffer;
	}

	void unlockNativeBuffer()
	{
		GrallocModule::getInstance()->unlock(nativeBuffer->handle);
	}
	#endif
};
}

#endif   // egl_Image_hpp
