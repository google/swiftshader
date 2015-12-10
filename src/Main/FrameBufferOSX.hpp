#ifndef sw_FrameBufferOSX_hpp
#define sw_FrameBufferOSX_hpp

#include "Main/FrameBuffer.hpp"

#import <Cocoa/Cocoa.h>

@class CALayer;

namespace sw
{
	class FrameBufferOSX : public FrameBuffer
	{
	public:
		FrameBufferOSX(CALayer *layer, int width, int height);
		~FrameBufferOSX() override;

		void flip(void *source, Format sourceFormat, size_t sourceStride) override;
		void blit(void *source, const Rect *sourceRect, const Rect *destRect, Format sourceFormat, size_t sourceStride) override;

		void *lock() override;
		void unlock() override;

	private:
		int width;
		int height;
		CALayer *layer;
		uint8_t *buffer;
		CGDataProviderRef provider;
		CGColorSpaceRef colorspace;
		CGImageRef currentImage;
	};
}

#endif   // sw_FrameBufferOSX
