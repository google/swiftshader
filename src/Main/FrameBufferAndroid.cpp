#include "FrameBufferAndroid.hpp"

#include <cutils/log.h>

namespace sw
{
	inline int dequeueBuffer(ANativeWindow* window, ANativeWindowBuffer** buffer)
	{
		#if ANDROID_PLATFORM_SDK_VERSION > 16
			return native_window_dequeue_buffer_and_wait(window, buffer);
		#else
			return window->dequeueBuffer(window, buffer);
		#endif
	}

	inline int queueBuffer(ANativeWindow* window, ANativeWindowBuffer* buffer, int fenceFd)
	{
		#if ANDROID_PLATFORM_SDK_VERSION > 16
			return window->queueBuffer(window, buffer, fenceFd);
		#else
			return window->queueBuffer(window, buffer);
		#endif
	}

	inline int cancelBuffer(ANativeWindow* window, ANativeWindowBuffer* buffer, int fenceFd)
	{
		#if ANDROID_PLATFORM_SDK_VERSION > 16
			return window->cancelBuffer(window, buffer, fenceFd);
		#else
			return window->cancelBuffer(window, buffer);
		#endif
	}

	FrameBufferAndroid::FrameBufferAndroid(ANativeWindow* window, int width, int height)
		: FrameBuffer(width, height, false, false),
		  nativeWindow(window), buffer(nullptr), gralloc(nullptr)
	{
		hw_module_t const* pModule;
		hw_get_module(GRALLOC_HARDWARE_MODULE_ID, &pModule);
		gralloc = reinterpret_cast<gralloc_module_t const*>(pModule);

		nativeWindow->common.incRef(&nativeWindow->common);
		native_window_set_usage(nativeWindow, GRALLOC_USAGE_SW_READ_OFTEN | GRALLOC_USAGE_SW_WRITE_OFTEN);
	}

	FrameBufferAndroid::~FrameBufferAndroid()
	{
		if(buffer)
		{
			// Probably doesn't have to cancel assuming a success queueing earlier
			cancelBuffer(nativeWindow, buffer, -1);
			buffer = nullptr;
		}

		nativeWindow->common.decRef(&nativeWindow->common);
	}

	void FrameBufferAndroid::blit(void *source, const Rect *sourceRect, const Rect *destRect, Format sourceFormat, size_t sourceStride)
	{
		copy(source, sourceFormat, sourceStride);

		if(buffer)
		{
			queueBuffer(nativeWindow, buffer, -1);

			if(locked)
			{
				locked = nullptr;
				unlock();
			}

			buffer->common.decRef(&buffer->common);
		}
	}

	void *FrameBufferAndroid::lock()
	{
		if(dequeueBuffer(nativeWindow, &buffer) != 0)
		{
			return nullptr;
		}

		buffer->common.incRef(&buffer->common);

		if(gralloc->lock(gralloc, buffer->handle,
		                 GRALLOC_USAGE_SW_READ_OFTEN | GRALLOC_USAGE_SW_WRITE_OFTEN,
		                 0, 0, buffer->width, buffer->height, &locked) != 0)
		{
			ALOGE("%s failed to lock buffer %p", __FUNCTION__, buffer);
			return nullptr;
		}

		if((buffer->width < width) || (buffer->height < height))
		{
			ALOGI("lock failed: buffer of %dx%d too small for window of %dx%d",
				  buffer->width, buffer->height, width, height);
			return nullptr;
		}

		switch(buffer->format)
		{
		default: ALOGE("Unsupported buffer format %d", buffer->format); ASSERT(false);
		case HAL_PIXEL_FORMAT_RGB_565: destFormat = FORMAT_R5G6B5; break;
		case HAL_PIXEL_FORMAT_RGB_888: destFormat = FORMAT_R8G8B8; break;
		case HAL_PIXEL_FORMAT_RGBA_8888: destFormat = FORMAT_A8B8G8R8; break;
		case HAL_PIXEL_FORMAT_RGBX_8888: destFormat = FORMAT_X8B8G8R8; break;
		case HAL_PIXEL_FORMAT_BGRA_8888: destFormat = FORMAT_A8R8G8B8; break;
		}

		stride = buffer->stride * Surface::bytes(destFormat);
		return locked;
	}

	void FrameBufferAndroid::unlock()
	{
		if(!buffer)
		{
			ALOGE("%s: badness unlock with no active buffer", __FUNCTION__);
			return;
		}

		locked = nullptr;

		if(gralloc->unlock(gralloc, buffer->handle) != 0)
		{
			ALOGE("%s: badness unlock failed", __FUNCTION__);
		}
	}
}

sw::FrameBuffer *createFrameBuffer(void *display, ANativeWindow* window, int width, int height)
{
	return new sw::FrameBufferAndroid(window, width, height);
}
