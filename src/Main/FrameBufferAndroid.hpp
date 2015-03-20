#ifndef sw_FrameBufferAndroid_hpp
#define sw_FrameBufferAndroid_hpp

#include "Main/FrameBuffer.hpp"
#include "Common/Debug.hpp"

#include <hardware/gralloc.h>
#include <system/window.h>

namespace sw
{
    class FrameBufferAndroid : public FrameBuffer
    {
    public:
        FrameBufferAndroid(ANativeWindow* window, int width, int height);

        ~FrameBufferAndroid();

        virtual void flip(void *source, Format format) {blit(source, 0, 0, format);};
        virtual void blit(void *source, const Rect *sourceRect, const Rect *destRect, Format format);

        virtual void *lock();
        virtual void unlock();

        bool setSwapRectangle(int l, int t, int w, int h);

    private:
        int lock(ANativeWindowBuffer* buf, int usage, void** vaddr);
        int unlock(ANativeWindowBuffer* buf);

        ANativeWindow* nativeWindow;
        ANativeWindowBuffer* buffer;
        gralloc_module_t const* gralloc;
        void* bits;
    };
}

#endif   // sw_FrameBufferAndroid
