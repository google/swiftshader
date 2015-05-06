#include "FrameBufferAndroid.hpp"

#include <cutils/log.h>
#include <ui/Fence.h>

namespace sw
{
    FrameBufferAndroid::FrameBufferAndroid(ANativeWindow* window, int width, int height)
			: FrameBuffer(width, height, false, false),
			  nativeWindow(window), buffer(0), gralloc(0)
    {
        hw_module_t const* pModule;
        hw_get_module(GRALLOC_HARDWARE_MODULE_ID, &pModule);
        gralloc = reinterpret_cast<gralloc_module_t const*>(pModule);

        nativeWindow->common.incRef(&nativeWindow->common);
        native_window_set_usage(nativeWindow, GRALLOC_USAGE_SW_READ_OFTEN | GRALLOC_USAGE_SW_WRITE_OFTEN);
    }

    FrameBufferAndroid::~FrameBufferAndroid()
    {
        if (buffer)
        {
            // Probably doesn't have to cancel assuming a success queueing earlier
            nativeWindow->cancelBuffer(nativeWindow, buffer, -1);
            buffer = 0;
        }
        nativeWindow->common.decRef(&nativeWindow->common);
    }

    void FrameBufferAndroid::blit(void *source, const Rect *sourceRect, const Rect *destRect, Format format)
    {
        copy(source, format);
		nativeWindow->queueBuffer(nativeWindow, buffer, -1);
		if (buffer && locked)
		{
			locked = 0;
			unlock();
		}

		buffer->common.decRef(&buffer->common);
    }

    void* FrameBufferAndroid::lock()
    {
        int fenceFd = -1;
        if (nativeWindow->dequeueBuffer(nativeWindow, &buffer, &fenceFd) != android::NO_ERROR)
        {
            return NULL;
        }

        android::sp<android::Fence> fence(new android::Fence(fenceFd));
        if (fence->wait(android::Fence::TIMEOUT_NEVER) != android::NO_ERROR)
        {
            nativeWindow->cancelBuffer(nativeWindow, buffer, fenceFd);
            return NULL;
        }

        buffer->common.incRef(&buffer->common);

        if (gralloc->lock(
				gralloc, buffer->handle,
				GRALLOC_USAGE_SW_READ_OFTEN | GRALLOC_USAGE_SW_WRITE_OFTEN,
				0, 0, buffer->width, buffer->height, &locked)
			!= android::NO_ERROR)
        {
            ALOGE("%s failed to lock buffer %p", __FUNCTION__, buffer);
            return NULL;
        }

		if ((buffer->width < width) || (buffer->height < height))
		{
			ALOGI("lock failed: buffer of %dx%d too small for window of %dx%d",
				  buffer->width, buffer->height, width, height);
			return NULL;
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
        if (!buffer)
		{
			ALOGE("%s: badness unlock with no active buffer", __FUNCTION__);
			return;
		}
		locked = 0;
        if (gralloc->unlock(gralloc, buffer->handle) != android::NO_ERROR)
		{
			ALOGE("%s: badness unlock failed", __FUNCTION__);
		}
    }
}

sw::FrameBuffer *createFrameBuffer(void *display, ANativeWindow* window, int width, int height)
{
    return new sw::FrameBufferAndroid(window, width, height);
}
