#include "FrameBufferOSX.hpp"

#include "Common/Debug.hpp"

#include <EGL/egl.h>
#import <QuartzCore/QuartzCore.h>

namespace sw {

	FrameBufferOSX::FrameBufferOSX(CALayer* layer, int width, int height)
		: FrameBuffer(width, height, false, false), width(width), height(height),
		  layer(layer), buffer(nullptr), provider(nullptr), currentImage(nullptr)
	{
		destFormat = sw::FORMAT_X8B8G8R8;
		int bufferSize = width * height * 4 * sizeof(uint8_t);
		buffer = new uint8_t[bufferSize];
		provider = CGDataProviderCreateWithData(nullptr, buffer, bufferSize, nullptr);
		colorspace = CGColorSpaceCreateDeviceRGB();
	}

	FrameBufferOSX::~FrameBufferOSX()
	{
		//[CATransaction begin];
		//[layer setContents:nullptr];
		//[CATransaction commit];

		CGImageRelease(currentImage);
		CGColorSpaceRelease(colorspace);
		CGDataProviderRelease(provider);
		
		delete[] buffer;
	}

	void FrameBufferOSX::flip(void *source, Format sourceFormat, size_t sourceStride)
	{
		blit(source, nullptr, nullptr, sourceFormat, sourceStride);
	}

	void FrameBufferOSX::blit(void *source, const Rect *sourceRect, const Rect *destRect, Format sourceFormat, size_t sourceStride)
	{
		copy(source, sourceFormat, sourceStride);

		int bytesPerRow = width * 4 * sizeof(uint8_t);
		CGImageRef image = CGImageCreate(width, height, 8, 32, bytesPerRow, colorspace, kCGBitmapByteOrder32Big, provider, nullptr, false, kCGRenderingIntentDefault);

		[CATransaction begin];
		[layer setContents:(id)image];
		[CATransaction commit];
		[CATransaction flush];

		if(currentImage)
		{
			CGImageRelease(currentImage);
		}
		currentImage = image;
	}

	void *FrameBufferOSX::lock()
	{
		stride = width * 4 * sizeof(uint8_t);
		locked = buffer;
		return locked;
	};
	
	void FrameBufferOSX::unlock()
	{
		locked = nullptr;
	};
}

sw::FrameBuffer *createFrameBuffer(void *display, EGLNativeWindowType nativeWindow, int width, int height)
{
	NSObject *window = reinterpret_cast<NSObject*>(nativeWindow);
	CALayer *layer = nullptr;

	if([window isKindOfClass:[NSView class]])
	{
		NSView *view = reinterpret_cast<NSView*>(window);
		[view setWantsLayer:YES];
		layer = [view layer];
	}
	else if([window isKindOfClass:[CALayer class]])
	{
		layer = reinterpret_cast<CALayer*>(window);
	}
	else ASSERT(0);
	
	return new sw::FrameBufferOSX(layer, width, height);
}
