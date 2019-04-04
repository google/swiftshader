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

#include "FrameBufferAndroid.hpp"

#ifndef ANDROID_NDK_BUILD
#include "Common/GrallocAndroid.hpp"
#include <system/window.h>
#else
#include <android/native_window.h>
#endif

namespace sw
{
#if !defined(ANDROID_NDK_BUILD)
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
#endif // !defined(ANDROID_NDK_BUILD)

	FrameBufferAndroid::FrameBufferAndroid(ANativeWindow* window, int width, int height)
		: FrameBuffer(width, height, false, false),
		  nativeWindow(window), buffer(nullptr)
	{
#ifndef ANDROID_NDK_BUILD
		nativeWindow->common.incRef(&nativeWindow->common);
		native_window_set_usage(nativeWindow, GRALLOC_USAGE_SW_READ_OFTEN | GRALLOC_USAGE_SW_WRITE_OFTEN);
#endif
	}

	FrameBufferAndroid::~FrameBufferAndroid()
	{
#ifndef ANDROID_NDK_BUILD
		nativeWindow->common.decRef(&nativeWindow->common);
#endif
	}

	void FrameBufferAndroid::blit(sw::Surface *source, const Rect *sourceRect, const Rect *destRect)
	{
		copy(source);

		if(buffer)
		{
			if(framebuffer)
			{
				framebuffer = nullptr;
				unlock();
			}

#ifndef ANDROID_NDK_BUILD
			queueBuffer(nativeWindow, buffer, -1);
#endif
		}
	}

	void *FrameBufferAndroid::lock()
	{

#if defined(ANDROID_NDK_BUILD)
		ANativeWindow_Buffer surfaceBuffer;
		if (ANativeWindow_lock(nativeWindow, &surfaceBuffer, nullptr) != 0) {
			TRACE("%s failed to lock buffer %p", __FUNCTION__, buffer);
			return nullptr;
		}
		framebuffer = surfaceBuffer.bits;

		if((surfaceBuffer.width < width) || (surfaceBuffer.height < height))
		{
			TRACE("lock failed: buffer of %dx%d too small for window of %dx%d",
			      surfaceBuffer.width, surfaceBuffer.height, width, height);
			return nullptr;
		}

		switch(surfaceBuffer.format)
		{
		case AHARDWAREBUFFER_FORMAT_R8G8B8A8_UNORM:     format = FORMAT_A8B8G8R8; break;
		case AHARDWAREBUFFER_FORMAT_R8G8B8X8_UNORM:     format = FORMAT_X8B8G8R8; break;
		case AHARDWAREBUFFER_FORMAT_R8G8B8_UNORM:
			// Frame buffers are expected to have 16-bit or 32-bit colors, not 24-bit.
			TRACE("Unsupported frame buffer format R8G8B8"); ASSERT(false);
			format = FORMAT_R8G8B8;   // Wrong component order.
			break;
		case AHARDWAREBUFFER_FORMAT_R5G6B5_UNORM:       format = FORMAT_R5G6B5; break;
		default:
			TRACE("Unsupported frame buffer format %d", surfaceBuffer.format); ASSERT(false);
			format = FORMAT_NULL;
			break;
		}
		stride = surfaceBuffer.stride * Surface::bytes(format);
#else // !defined(ANDROID_NDK_BUILD)
		if(dequeueBuffer(nativeWindow, &buffer) != 0)
		{
			return nullptr;
		}
		if(GrallocModule::getInstance()->lock(buffer->handle,
		                 GRALLOC_USAGE_SW_READ_OFTEN | GRALLOC_USAGE_SW_WRITE_OFTEN,
		                 0, 0, buffer->width, buffer->height, &framebuffer) != 0)
		{
			TRACE("%s failed to lock buffer %p", __FUNCTION__, buffer);
			return nullptr;
		}

		if((buffer->width < width) || (buffer->height < height))
		{
			TRACE("lock failed: buffer of %dx%d too small for window of %dx%d",
			      buffer->width, buffer->height, width, height);
			return nullptr;
		}

		switch(buffer->format)
		{
		case HAL_PIXEL_FORMAT_RGB_565:   format = FORMAT_R5G6B5; break;
		case HAL_PIXEL_FORMAT_RGBA_8888: format = FORMAT_A8B8G8R8; break;
#if ANDROID_PLATFORM_SDK_VERSION > 16
		case HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED: format = FORMAT_X8B8G8R8; break;
#endif
		case HAL_PIXEL_FORMAT_RGBX_8888: format = FORMAT_X8B8G8R8; break;
		case HAL_PIXEL_FORMAT_BGRA_8888: format = FORMAT_A8R8G8B8; break;
		case HAL_PIXEL_FORMAT_RGB_888:
			// Frame buffers are expected to have 16-bit or 32-bit colors, not 24-bit.
			TRACE("Unsupported frame buffer format RGB_888"); ASSERT(false);
			format = FORMAT_R8G8B8;   // Wrong component order.
			break;
		default:
			TRACE("Unsupported frame buffer format %d", buffer->format); ASSERT(false);
			format = FORMAT_NULL;
			break;
		}
		stride = buffer->stride * Surface::bytes(format);
#endif // !defined(ANDROID_NDK_BUILD)

		return framebuffer;
	}

	void FrameBufferAndroid::unlock()
	{
		if(!buffer)
		{
			TRACE("%s: badness unlock with no active buffer", __FUNCTION__);
			return;
		}

		framebuffer = nullptr;

#ifdef ANDROID_NDK_BUILD
		ANativeWindow_unlockAndPost(nativeWindow);
#else
		if(GrallocModule::getInstance()->unlock(buffer->handle) != 0)
		{
			TRACE("%s: badness unlock failed", __FUNCTION__);
		}
#endif
	}
}

sw::FrameBuffer *createFrameBuffer(void *display, ANativeWindow* window, int width, int height)
{
	return new sw::FrameBufferAndroid(window, width, height);
}
