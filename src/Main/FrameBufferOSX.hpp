#ifndef sw_FrameBufferOSX_hpp
#define sw_FrameBufferOSX_hpp

#include "Main/FrameBuffer.hpp"

@class CALayer;

namespace sw
{
	class FrameBufferOSX : public FrameBuffer
	{
	public:
		FrameBufferOSX(CALayer *window, int width, int height) : FrameBuffer(width, height, false, false) {};

		~FrameBufferOSX() override {};

		void flip(void *source, Format sourceFormat, size_t sourceStride) override;
		void blit(void *source, const Rect *sourceRect, const Rect *destRect, Format sourceFormat, size_t sourceStride) override {};

		void *lock() override {return nullptr;};
		void unlock() override {};
	};
}

#endif   // sw_FrameBufferOSX
