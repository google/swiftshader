#ifndef egl_Image_hpp
#define egl_Image_hpp

#include "libEGL/Texture.hpp"
#include "Renderer/Surface.hpp"

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#if defined(__ANDROID__)
#include <hardware/gralloc.h>
#include <system/window.h>
#include "../../Common/GrallocAndroid.hpp"
#include "../../Common/DebugAndroid.hpp"
#define LOGLOCK(fmt, ...) // ALOGI(fmt " tid=%d", ##__VA_ARGS__, gettid())
#else
#include <assert.h>
#define LOGLOCK(...)
#endif

// Implementation-defined formats
#define SW_YV12_BT601 0x32315659   // YCrCb 4:2:0 Planar, 16-byte aligned, BT.601 color space, studio swing
#define SW_YV12_BT709 0x48315659   // YCrCb 4:2:0 Planar, 16-byte aligned, BT.709 color space, studio swing
#define SW_YV12_JFIF  0x4A315659   // YCrCb 4:2:0 Planar, 16-byte aligned, BT.601 color space, full swing

namespace egl
{

sw::Format ConvertFormatType(GLenum format, GLenum type);
sw::Format SelectInternalFormat(GLenum format, GLenum type);
GLsizei ComputePitch(GLsizei width, GLenum format, GLenum type, GLint alignment);
GLsizei ComputeCompressedSize(GLsizei width, GLsizei height, GLenum format);

static inline sw::Resource *getParentResource(egl::Texture *texture)
{
	return texture ? texture->getResource() : nullptr;
}

class Image : public sw::Surface
{
public:
	Image(Texture *parentTexture, GLsizei width, GLsizei height, GLenum format, GLenum type)
		: sw::Surface(getParentResource(parentTexture), width, height, 1, SelectInternalFormat(format, type), true, true),
		  width(width), height(height), format(format), type(type), internalFormat(SelectInternalFormat(format, type)), depth(1),
		  parentTexture(parentTexture)
	{
		shared = false;
		referenceCount = 1;
	}

	Image(Texture *parentTexture, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, int pitchP = 0)
		: sw::Surface(getParentResource(parentTexture), width, height, depth, SelectInternalFormat(format, type), true, true, pitchP),
		  width(width), height(height), format(format), type(type), internalFormat(SelectInternalFormat(format, type)), depth(depth),
		  parentTexture(parentTexture)
	{
		shared = false;
		referenceCount = 1;
	}

	Image(GLsizei width, GLsizei height, sw::Format internalFormat, int multiSampleDepth, bool lockable, bool renderTarget)
		: sw::Surface(nullptr, width, height, multiSampleDepth, internalFormat, lockable, renderTarget),
		  width(width), height(height), format(0 /*GL_NONE*/), type(0 /*GL_NONE*/), internalFormat(internalFormat), depth(multiSampleDepth), parentTexture(nullptr)
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

	struct UnpackInfo
	{
		UnpackInfo() : alignment(4), rowLength(0), imageHeight(0), skipPixels(0), skipRows(0), skipImages(0) {}

		GLint alignment;
		GLint rowLength;
		GLint imageHeight;
		GLint skipPixels;
		GLint skipRows;
		GLint skipImages;
	};

	void loadImageData(GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const UnpackInfo& unpackInfo, const void *input);
	void loadCompressedData(GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLsizei imageSize, const void *pixels);

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

	void loadD24S8ImageData(GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, int inputPitch, int inputHeight, const void *input, void *buffer);
	void loadD32FS8ImageData(GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, int inputPitch, int inputHeight, const void *input, void *buffer);
};

#ifdef __ANDROID__

inline GLenum GLPixelFormatFromAndroid(int halFormat)
{
	switch(halFormat)
	{
	case HAL_PIXEL_FORMAT_RGBA_8888: return GL_RGBA;
	case HAL_PIXEL_FORMAT_RGBX_8888: return GL_RGB;
	case HAL_PIXEL_FORMAT_RGB_888:   return GL_NONE;   // Unsupported
	case HAL_PIXEL_FORMAT_BGRA_8888: return GL_BGRA_EXT;
	case HAL_PIXEL_FORMAT_RGB_565:   return GL_RGB565;
	case HAL_PIXEL_FORMAT_YV12:      return SW_YV12_BT601;
	default:                         return GL_NONE;
	}
}

inline GLenum GLPixelTypeFromAndroid(int halFormat)
{
	switch(halFormat)
	{
	case HAL_PIXEL_FORMAT_RGBA_8888: return GL_UNSIGNED_BYTE;
	case HAL_PIXEL_FORMAT_RGBX_8888: return GL_UNSIGNED_BYTE;
	case HAL_PIXEL_FORMAT_RGB_888:   return GL_NONE;   // Unsupported
	case HAL_PIXEL_FORMAT_BGRA_8888: return GL_UNSIGNED_BYTE;
	case HAL_PIXEL_FORMAT_RGB_565:   return GL_UNSIGNED_SHORT_5_6_5;
	case HAL_PIXEL_FORMAT_YV12:      return GL_UNSIGNED_BYTE;
	default:                         return GL_NONE;
	}
}

class AndroidNativeImage : public egl::Image
{
public:
	explicit AndroidNativeImage(ANativeWindowBuffer *nativeBuffer)
		: egl::Image(0, nativeBuffer->width, nativeBuffer->height, 1,
		             GLPixelFormatFromAndroid(nativeBuffer->format),
		             GLPixelTypeFromAndroid(nativeBuffer->format),
		             nativeBuffer->stride),
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
			if(x != 0 || y != 0 || z != 0)
			{
				ALOGI("badness: %s called with unsupported parms: image=%p x=%d y=%d z=%d", __FUNCTION__, this, x, y, z);
			}

			LOGLOCK("image=%p op=%s.ani lock=%d", this, __FUNCTION__, lock);

			// Lock the ANativeWindowBuffer and use its address.
			data = lockNativeBuffer(GRALLOC_USAGE_SW_READ_OFTEN | GRALLOC_USAGE_SW_WRITE_OFTEN);

			if(lock == sw::LOCK_UNLOCKED)
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
		void *buffer = nullptr;
		GrallocModule::getInstance()->lock(nativeBuffer->handle, usage, 0, 0, nativeBuffer->width, nativeBuffer->height, &buffer);

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
