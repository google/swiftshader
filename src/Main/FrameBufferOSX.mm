#include "FrameBufferOSX.hpp"

namespace sw
{
	void FrameBufferOSX::flip(void *source, Format sourceFormat, size_t sourceStride)
	{
		blit(source, 0, 0, sourceFormat, sourceStride);
	}
}

sw::FrameBuffer *createFrameBuffer(void *display, CALayer *layer, int width, int height)
{
	return new sw::FrameBufferOSX(layer, width, height);
}
