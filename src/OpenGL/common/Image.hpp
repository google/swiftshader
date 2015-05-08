#ifndef egl_Image_hpp
#define egl_Image_hpp

#include "libEGL/Texture.hpp"
#include "Renderer/Surface.hpp"

#include <GLES/gl.h>
#include <GLES2/gl2.h>

#if defined(__ANDROID__)
#include <hardware/gralloc.h>
#include <system/window.h>
#include "../../Common/GrallocAndroid.hpp"
#include "../common/AndroidCommon.hpp"
#endif

#ifdef __ANDROID__
#include "../../Common/DebugAndroid.hpp"
#define LOGLOCK(fmt, ...) // ALOGI(fmt " tid=%d", ##__VA_ARGS__, gettid())
#else
#include <assert.h>
#define LOGLOCK(...)
#endif

namespace egl
{
// Types common between gl.h and gl2.h
// We can't include either header in EGL
typedef unsigned int GLenum;
typedef int GLint;
typedef int GLsizei;

int ComputePixelSize(GLenum format, GLenum type);
GLsizei ComputePitch(GLsizei width, GLenum format, GLenum type, GLint alignment);
GLsizei ComputeCompressedPitch(GLsizei width, GLenum format);
GLsizei ComputeCompressedSize(GLsizei width, GLsizei height, GLenum format);

static inline sw::Resource *getParentResource(egl::Texture *texture)
{
	if (texture)
	{
		return texture->getResource();
	}
	return 0;
}

class Image : public sw::Surface
{
public:
	Image(Texture *parentTexture, GLsizei width, GLsizei height, GLenum format, GLenum type)
			: sw::Surface(getParentResource(parentTexture), width, height, 1, selectInternalFormat(format, type), true, true),
			  width(width), height(height), format(format), type(type), internalFormat(selectInternalFormat(format, type)), depth(1),
			  parentTexture(parentTexture)
	{
		shared = false;
		referenceCount = 1;
	}

	Image(Texture *parentTexture, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type)
			: sw::Surface(getParentResource(parentTexture), width, height, depth, selectInternalFormat(format, type), true, true),
			  width(width), height(height), format(format), type(type), internalFormat(selectInternalFormat(format, type)), depth(depth),
			  parentTexture(parentTexture)
	{
		shared = false;
		referenceCount = 1;
	}

	Image(Texture *parentTexture, GLsizei width, GLsizei height, sw::Format internalFormat, int multiSampleDepth, bool lockable, bool renderTarget)
			: sw::Surface(getParentResource(parentTexture), width, height, multiSampleDepth, internalFormat, lockable, renderTarget),
			  width(width), height(height), format(0 /*GL_NONE*/), type(0 /*GL_NONE*/), internalFormat(internalFormat), depth(multiSampleDepth), parentTexture(parentTexture)
	{
		shared = false;
		referenceCount = 1;
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

	virtual void *lock(unsigned int left, unsigned int top, sw::Lock lock)
	{
		return lockExternal(left, top, 0, lock, sw::PUBLIC);
	}

	unsigned int getPitch() const
	{
		return getExternalPitchB();
	}

	virtual void unlock()
	{
		unlockExternal();
	}

	void loadImageData(GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, GLint unpackAlignment, const void *input);
	void loadCompressedData(GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLsizei imageSize, const void *pixels);

	static sw::Format selectInternalFormat(GLenum format, GLenum type);

	virtual void addRef();
	virtual void release();
	virtual void unbind(const Texture *parent);   // Break parent ownership and release

	virtual void destroyShared()   // Release a shared image
	{
		assert(shared);
		shared = false;
		release();
	}

protected:
	const GLsizei width;
	const GLsizei height;
	const GLenum format;
	const GLenum type;
	const sw::Format internalFormat;
	const int depth;

	bool shared;   // Used as an EGLImage

	egl::Texture *parentTexture;

	volatile int referenceCount;

	virtual ~Image();

	void loadD24S8ImageData(GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, int inputPitch, const void *input, void *buffer);
};

#ifdef __ANDROID__

class AndroidNativeImage : public egl::Image
{
public:
	explicit AndroidNativeImage(ANativeWindowBuffer *nativeBuffer)
		: egl::Image(0, nativeBuffer->width, nativeBuffer->height, 1,
					getColorFormatFromAndroid(nativeBuffer->format),
					getPixelFormatFromAndroid(nativeBuffer->format)),
		  nativeBuffer(nativeBuffer)
{
    nativeBuffer->common.incRef(&nativeBuffer->common);
    markShared();
}

private:
	ANativeWindowBuffer *nativeBuffer;

	virtual ~AndroidNativeImage()
	{
		// Wait for any draw calls that use this image to finish
		resource->lock(sw::DESTRUCT);
		resource->unlock();

		nativeBuffer->common.decRef(&nativeBuffer->common);
	}

	virtual void *lockInternal(int x, int y, int z, sw::Lock lock, sw::Accessor client)
	{
		LOGLOCK("image=%p op=%s.swsurface lock=%d", this, __FUNCTION__, lock);
		// Always do this for reference counting.
		void *data = sw::Surface::lockInternal(x, y, z, lock, client);
		if(nativeBuffer)
		{
			if (x || y || z)
			{
				ALOGI("badness: %s called with unsupported parms: image=%p x=%d y=%d z=%d", __FUNCTION__, this, x, y, z);
			}
			LOGLOCK("image=%p op=%s.ani lock=%d", this, __FUNCTION__, lock);
			// Lock the ANativeWindowBuffer and use it's address.
			data = lockNativeBuffer(
				GRALLOC_USAGE_SW_READ_OFTEN | GRALLOC_USAGE_SW_WRITE_OFTEN);
			if (lock == sw::LOCK_UNLOCKED)
			{
				// We're never going to get a corresponding unlock, so unlock
				// immediately. This keeps the gralloc reference counts sane.
				unlockNativeBuffer();
			}
		}
		return data;
	}

	virtual void unlockInternal()
	{
		if(nativeBuffer)   // Unlock the buffer from ANativeWindowBuffer
		{
			LOGLOCK("image=%p op=%s.ani", this, __FUNCTION__);
			unlockNativeBuffer();
		}
		LOGLOCK("image=%p op=%s.swsurface", this, __FUNCTION__);
		sw::Surface::unlockInternal();
	}

	virtual void *lock(unsigned int left, unsigned int top, sw::Lock lock)
	{
		LOGLOCK("image=%p op=%s lock=%d", this, __FUNCTION__, lock);
		(void)sw::Surface::lockExternal(left, top, 0, lock, sw::PUBLIC);
		return lockNativeBuffer(GRALLOC_USAGE_SW_READ_OFTEN | GRALLOC_USAGE_SW_WRITE_OFTEN);
	}

	virtual void unlock()
	{
		LOGLOCK("image=%p op=%s.ani", this, __FUNCTION__);
		unlockNativeBuffer();
		LOGLOCK("image=%p op=%s.swsurface", this, __FUNCTION__);
		sw::Surface::unlockExternal();
	}

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
};

#endif  // __ANDROID__

}

#endif   // egl_Image_hpp
