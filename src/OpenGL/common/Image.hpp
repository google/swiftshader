// Copyright 2016 The SwiftShader Authors. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef egl_Image_hpp
#define egl_Image_hpp

#include "libEGL/Texture.hpp"
#include "Renderer/Surface.hpp"

#include <GLES3/gl3.h>
#include <GLES2/gl2ext.h>

#if defined(__ANDROID__)
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

class Context;

sw::Format ConvertFormatType(GLenum format, GLenum type);
sw::Format SelectInternalFormat(GLenum format, GLenum type);
GLsizei ComputePitch(GLsizei width, GLenum format, GLenum type, GLint alignment);
GLsizei ComputeCompressedSize(GLsizei width, GLsizei height, GLenum format);
size_t ComputePackingOffset(GLenum format, GLenum type, GLsizei width, GLsizei height, GLint alignment, GLint skipImages, GLint skipRows, GLint skipPixels);

class [[clang::lto_visibility_public]] Image : public sw::Surface, public gl::Object
{
protected:
	// 2D texture image
	Image(Texture *parentTexture, GLsizei width, GLsizei height, GLenum format, GLenum type)
		: sw::Surface(parentTexture->getResource(), width, height, 1, 0, 1, SelectInternalFormat(format, type), true, true),
		  width(width), height(height), depth(1), format(format), type(type), internalFormat(SelectInternalFormat(format, type)),
		  parentTexture(parentTexture)
	{
		shared = false;
		Object::addRef();
		parentTexture->addRef();
	}

	// 3D/Cube texture image
	Image(Texture *parentTexture, GLsizei width, GLsizei height, GLsizei depth, int border, GLenum format, GLenum type)
		: sw::Surface(parentTexture->getResource(), width, height, depth, border, 1, SelectInternalFormat(format, type), true, true),
		  width(width), height(height), depth(depth), format(format), type(type), internalFormat(SelectInternalFormat(format, type)),
		  parentTexture(parentTexture)
	{
		shared = false;
		Object::addRef();
		parentTexture->addRef();
	}

	// Native EGL image
	Image(GLsizei width, GLsizei height, GLenum format, GLenum type, int pitchP)
		: sw::Surface(nullptr, width, height, 1, 0, 1, SelectInternalFormat(format, type), true, true, pitchP),
		  width(width), height(height), depth(1), format(format), type(type), internalFormat(SelectInternalFormat(format, type)),
		  parentTexture(nullptr)
	{
		shared = true;
		Object::addRef();
	}

	// Render target
	Image(GLsizei width, GLsizei height, sw::Format internalFormat, int multiSampleDepth, bool lockable)
		: sw::Surface(nullptr, width, height, 1, 0, multiSampleDepth, internalFormat, lockable, true),
		  width(width), height(height), depth(1), format(0 /*GL_NONE*/), type(0 /*GL_NONE*/), internalFormat(internalFormat),
		  parentTexture(nullptr)
	{
		shared = false;
		Object::addRef();
	}

public:
	// 2D texture image
	static Image *create(Texture *parentTexture, GLsizei width, GLsizei height, GLenum format, GLenum type);

	// 3D/Cube texture image
	static Image *create(Texture *parentTexture, GLsizei width, GLsizei height, GLsizei depth, int border, GLenum format, GLenum type);

	// Native EGL image
	static Image *create(GLsizei width, GLsizei height, GLenum format, GLenum type, int pitchP);

	// Render target
	static Image *create(GLsizei width, GLsizei height, sw::Format internalFormat, int multiSampleDepth, bool lockable);

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

	unsigned int getSlice() const
	{
		return getExternalSliceB();
	}

	virtual void unlock()
	{
		unlockExternal();
	}

	void *lockInternal(int x, int y, int z, sw::Lock lock, sw::Accessor client) override = 0;
	void unlockInternal() override = 0;

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

	void loadImageData(Context *context, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const UnpackInfo& unpackInfo, const void *input);
	void loadCompressedData(GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLsizei imageSize, const void *pixels);

	void release() override = 0;
	void unbind(const Texture *parent);   // Break parent ownership and release
	bool isChildOf(const Texture *parent) const;

	virtual void destroyShared()   // Release a shared image
	{
		assert(shared);
		shared = false;
		release();
	}

protected:
	const GLsizei width;
	const GLsizei height;
	const int depth;
	const GLenum format;
	const GLenum type;
	const sw::Format internalFormat;

	bool shared;   // Used as an EGLImage

	egl::Texture *parentTexture;

	~Image() override = 0;

	void loadD24S8ImageData(GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, int inputPitch, int inputHeight, const void *input, void *buffer);
	void loadD32FS8ImageData(GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, int inputPitch, int inputHeight, const void *input, void *buffer);
};

#ifdef __ANDROID__

inline GLenum GLPixelFormatFromAndroid(int halFormat)
{
	switch(halFormat)
	{
	case HAL_PIXEL_FORMAT_RGBA_8888: return GL_RGBA8;
#if ANDROID_PLATFORM_SDK_VERSION > 16
	case HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED: return GL_RGB8;
#endif
	case HAL_PIXEL_FORMAT_RGBX_8888: return GL_RGB8;
	case HAL_PIXEL_FORMAT_BGRA_8888: return GL_BGRA8_EXT;
	case HAL_PIXEL_FORMAT_RGB_565:   return GL_RGB565;
	case HAL_PIXEL_FORMAT_YV12:      return SW_YV12_BT601;
#ifdef GRALLOC_MODULE_API_VERSION_0_2
	case HAL_PIXEL_FORMAT_YCbCr_420_888: return SW_YV12_BT601;
#endif
	case HAL_PIXEL_FORMAT_RGB_888:   // Unsupported.
	default:
		ALOGE("Unsupported EGL image format %d", halFormat); ASSERT(false);
		return GL_NONE;
	}
}

inline GLenum GLPixelTypeFromAndroid(int halFormat)
{
	switch(halFormat)
	{
	case HAL_PIXEL_FORMAT_RGBA_8888: return GL_UNSIGNED_BYTE;
#if ANDROID_PLATFORM_SDK_VERSION > 16
	case HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED: return GL_UNSIGNED_BYTE;
#endif
	case HAL_PIXEL_FORMAT_RGBX_8888: return GL_UNSIGNED_BYTE;
	case HAL_PIXEL_FORMAT_BGRA_8888: return GL_UNSIGNED_BYTE;
	case HAL_PIXEL_FORMAT_RGB_565:   return GL_UNSIGNED_SHORT_5_6_5;
	case HAL_PIXEL_FORMAT_YV12:      return GL_UNSIGNED_BYTE;
#ifdef GRALLOC_MODULE_API_VERSION_0_2
	case HAL_PIXEL_FORMAT_YCbCr_420_888: return GL_UNSIGNED_BYTE;
#endif
	case HAL_PIXEL_FORMAT_RGB_888:   // Unsupported.
	default:
		ALOGE("Unsupported EGL image format %d", halFormat); ASSERT(false);
		return GL_NONE;
	}
}

class AndroidNativeImage : public egl::Image
{
public:
	explicit AndroidNativeImage(ANativeWindowBuffer *nativeBuffer)
		: egl::Image(nativeBuffer->width, nativeBuffer->height,
		             GLPixelFormatFromAndroid(nativeBuffer->format),
		             GLPixelTypeFromAndroid(nativeBuffer->format),
		             nativeBuffer->stride),
		  nativeBuffer(nativeBuffer)
	{
		nativeBuffer->common.incRef(&nativeBuffer->common);
	}

private:
	ANativeWindowBuffer *nativeBuffer;

	~AndroidNativeImage() override
	{
		sync();   // Wait for any threads that use this image to finish.

		nativeBuffer->common.decRef(&nativeBuffer->common);
	}

	void *lockInternal(int x, int y, int z, sw::Lock lock, sw::Accessor client) override
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

	void unlockInternal() override
	{
		if(nativeBuffer)   // Unlock the buffer from ANativeWindowBuffer
		{
			LOGLOCK("image=%p op=%s.ani", this, __FUNCTION__);
			unlockNativeBuffer();
		}

		LOGLOCK("image=%p op=%s.swsurface", this, __FUNCTION__);
		sw::Surface::unlockInternal();
	}

	void *lock(unsigned int left, unsigned int top, sw::Lock lock) override
	{
		LOGLOCK("image=%p op=%s lock=%d", this, __FUNCTION__, lock);
		(void)sw::Surface::lockExternal(left, top, 0, lock, sw::PUBLIC);

		return lockNativeBuffer(GRALLOC_USAGE_SW_READ_OFTEN | GRALLOC_USAGE_SW_WRITE_OFTEN);
	}

	void unlock() override
	{
		LOGLOCK("image=%p op=%s.ani", this, __FUNCTION__);
		unlockNativeBuffer();

		LOGLOCK("image=%p op=%s.swsurface", this, __FUNCTION__);
		sw::Surface::unlockExternal();
	}

	void *lockNativeBuffer(int usage)
	{
		void *buffer = nullptr;
		GrallocModule::getInstance()->lock(nativeBuffer->handle, usage, 0, 0, nativeBuffer->width, nativeBuffer->height, &buffer);

		return buffer;
	}

	void unlockNativeBuffer()
	{
		GrallocModule::getInstance()->unlock(nativeBuffer->handle);
	}

	void release() override
	{
		Image::release();
	}
};

#endif  // __ANDROID__

}

#endif   // egl_Image_hpp
