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

#ifndef	sw_FrameBuffer_hpp
#define	sw_FrameBuffer_hpp

#include "Reactor/Nucleus.hpp"
#include "Renderer/Surface.hpp"
#include "Common/Thread.hpp"

namespace sw
{
	class Surface;

	struct BlitState
	{
		int width;
		int height;
		Format destFormat;
		Format sourceFormat;
		int stride;
		int cursorWidth;
		int cursorHeight;
	};

	class FrameBuffer
	{
	public:
		FrameBuffer(int width, int height, bool fullscreen, bool topLeftOrigin);

		virtual ~FrameBuffer();

		int getWidth() const;
		int getHeight() const;
		int getStride() const;

		virtual void flip(void *source, Format sourceFormat, size_t sourceStride) = 0;
		virtual void blit(void *source, const Rect *sourceRect, const Rect *destRect, Format sourceFormat, size_t sourceStride) = 0;

		virtual void *lock() = 0;
		virtual void unlock() = 0;

		static void setCursorImage(sw::Surface *cursor);
		static void setCursorOrigin(int x0, int y0);
		static void setCursorPosition(int x, int y);

		static Routine *copyRoutine(const BlitState &state);

	protected:
		void copy(void *source, Format format, size_t stride);
		int width;
		int height;
		Format sourceFormat;
		Format destFormat;
		int stride;
		bool windowed;

		void *locked;   // Video memory back buffer

	private:
		void copyLocked();

		static void threadFunction(void *parameters);

		void *target;   // Render target buffer

		void (*blitFunction)(void *dst, void *src);
		Routine *blitRoutine;
		BlitState blitState;

		static void blend(const BlitState &state, const Pointer<Byte> &d, const Pointer<Byte> &s, const Pointer<Byte> &c);

		static void *cursor;
		static int cursorWidth;
		static int cursorHeight;
		static int cursorHotspotX;
		static int cursorHotspotY;
		static int cursorPositionX;
		static int cursorPositionY;
		static int cursorX;
		static int cursorY;

		Thread *blitThread;
		Event syncEvent;
		Event blitEvent;
		volatile bool terminate;

		static bool topLeftOrigin;
	};
}

#endif	 //	sw_FrameBuffer_hpp
