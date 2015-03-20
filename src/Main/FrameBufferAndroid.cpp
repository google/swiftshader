#include "FrameBufferAndroid.hpp"

#include <cutils/log.h>
#include <ui/Fence.h>

namespace sw
{
    FrameBufferAndroid::FrameBufferAndroid(ANativeWindow* window, int width, int height)
        : FrameBuffer(width, height, false, false),
        nativeWindow(window), buffer(0), gralloc(0), bits(NULL)
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

        if (buffer && bits)
        {
            bits = 0;
            unlock(buffer);
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

        if (lock(buffer, GRALLOC_USAGE_SW_READ_OFTEN | GRALLOC_USAGE_SW_WRITE_OFTEN, &bits) != android::NO_ERROR)
        {
            ALOGE("connect() failed to lock buffer %p", buffer);
            return NULL;
        }

        locked = bits;
        return locked;
    }

    void FrameBufferAndroid::unlock()
    {
        locked = 0;
    }

    int FrameBufferAndroid::lock(ANativeWindowBuffer* buf, int usage, void** vaddr)
    {
        return gralloc->lock(gralloc, buf->handle, usage, 0, 0, buf->width, buf->height, vaddr);
    }

    int FrameBufferAndroid::unlock(ANativeWindowBuffer* buf)
    {
        if (!buf) return -1;
        return gralloc->unlock(gralloc, buf->handle);
    }
}

extern "C"
{
    sw::FrameBuffer *createFrameBuffer(void *display, void* window, int width, int height)
    {
        return new sw::FrameBufferAndroid((ANativeWindow*)window, width, height);
    }
}
