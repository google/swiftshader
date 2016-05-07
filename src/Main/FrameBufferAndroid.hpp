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

		void flip(void *source, Format sourceFormat, size_t sourceStride) override {blit(source, 0, 0, sourceFormat, sourceStride);};
		void blit(void *source, const Rect *sourceRect, const Rect *destRect, Format sourceFormat, size_t sourceStride) override;

		void *lock() override;
		void unlock() override;

		bool setSwapRectangle(int l, int t, int w, int h);

	private:
		ANativeWindow* nativeWindow;
		ANativeWindowBuffer* buffer;
		gralloc_module_t const* gralloc;
	};
}

#endif   // sw_FrameBufferAndroid
