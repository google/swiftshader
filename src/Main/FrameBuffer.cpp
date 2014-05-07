// SwiftShader Software Renderer
//
// Copyright(c) 2005-2012 TransGaming Inc.
//
// All rights reserved. No part of this software may be copied, distributed, transmitted,
// transcribed, stored in a retrieval system, translated into any human or computer
// language by any means, or disclosed to third parties without the explicit written
// agreement of TransGaming Inc. Without such an agreement, no rights or licenses, express
// or implied, including but not limited to any patent rights, are granted to you.
//

#include "FrameBuffer.hpp"

#include "Timer.hpp"
#include "CPUID.hpp"
#include "serialvalid.h"
#include "Register.hpp"
#include "Renderer/Surface.hpp"
#include "Reactor/Reactor.hpp"
#include "Common/Debug.hpp"

#include <stdio.h>
#include <string.h>
#include <time.h>

#ifndef DISPLAY_LOGO
#define DISPLAY_LOGO (NDEBUG & 1)
#endif

#define ASYNCHRONOUS_BLIT 0   // FIXME: Currently leads to rare race conditions

extern const int logoWidth;
extern const int logoHeight;
extern const unsigned int logoData[];

namespace sw
{
	extern bool forceWindowed;

	Surface *FrameBuffer::logo;
	unsigned int *FrameBuffer::logoImage;
	void *FrameBuffer::cursor;
	int FrameBuffer::cursorWidth = 0;
	int FrameBuffer::cursorHeight = 0;
	int FrameBuffer::cursorHotspotX;
	int FrameBuffer::cursorHotspotY;
	int FrameBuffer::cursorPositionX;
	int FrameBuffer::cursorPositionY;
	int FrameBuffer::cursorX;
	int FrameBuffer::cursorY;
	bool FrameBuffer::topLeftOrigin = false;

	FrameBuffer::FrameBuffer(int width, int height, bool fullscreen, bool topLeftOrigin)
	{
		this->topLeftOrigin = topLeftOrigin;

		locked = 0;

		this->width = width;
		this->height = height;
		bitDepth = 32;
		HDRdisplay = false;
		stride = 0;

		if(forceWindowed)
		{
			fullscreen = false;
		}

		windowed = !fullscreen;

		blitFunction = 0;
		blitRoutine = 0;

		blitState.width = 0;
		blitState.height = 0;
		blitState.depth = 0;
		blitState.cursorWidth = 0;
		blitState.cursorHeight = 0;

		logo = 0;

		if(ASYNCHRONOUS_BLIT)
		{
			terminate = false;
			FrameBuffer *parameters = this;
			blitThread = new Thread(threadFunction, &parameters);
		}
	}

	FrameBuffer::~FrameBuffer()
	{
		if(ASYNCHRONOUS_BLIT)
		{
			terminate = true;
			blitEvent.signal();
			blitThread->join();
			delete blitThread;
		}

		delete blitRoutine;
	}

	int FrameBuffer::getWidth() const
	{
		return width;
	}

	int FrameBuffer::getHeight() const
	{
		return height;
	}

	int FrameBuffer::getStride() const
	{
		return stride;
	}

	void FrameBuffer::setCursorImage(sw::Surface *cursorImage)
	{
		if(cursorImage)
		{
			cursor = cursorImage->lockExternal(0, 0, 0, sw::LOCK_READONLY, sw::PUBLIC);
			cursorImage->unlockExternal();

			cursorWidth = cursorImage->getExternalWidth();
			cursorHeight = cursorImage->getExternalHeight();
		}
		else
		{
			cursorWidth = 0;
			cursorHeight = 0;
		}
	}

	void FrameBuffer::setCursorOrigin(int x0, int y0)
	{
		cursorHotspotX = x0;
		cursorHotspotY = y0;
	}

	void FrameBuffer::setCursorPosition(int x, int y)
	{
		cursorPositionX = x;
		cursorPositionY = y;
	}

	void FrameBuffer::copy(void *source, bool HDR)
	{
		if(!source)
		{
			return;
		}

		if(!lock())
		{
			return;
		}

		if(topLeftOrigin)
		{
			target = source;
		}
		else
		{
			const int width2 = (width + 1) & ~1;
			const int sBytes = HDR ? 8 : 4;
			const int sStride = sBytes * width2;

			target = (byte*)source + (height - 1) * sStride;
		}

		HDRdisplay = HDR;

		cursorX = cursorPositionX - cursorHotspotX;
		cursorY = cursorPositionY - cursorHotspotY;

		if(ASYNCHRONOUS_BLIT)
		{
			blitEvent.signal();
			syncEvent.wait();
		}
		else
		{
			copyLocked();
		}
	
		unlock();
	}

	void FrameBuffer::copyLocked()
	{
		BlitState update = {0};

		update.width = width;
		update.height = height;
		update.depth = bitDepth;
		update.stride = stride;
		update.HDR = HDRdisplay;
		update.cursorWidth = cursorWidth;
		update.cursorHeight = cursorHeight;

		if(memcmp(&blitState, &update, sizeof(BlitState)) != 0)
		{
			blitState = update;
			delete blitRoutine;

			blitRoutine = copyRoutine(blitState);
			blitFunction = (void(*)(void*, void*))blitRoutine->getEntry();
		}

		blitFunction(locked, target);
	}

	Routine *FrameBuffer::copyRoutine(const BlitState &state)
	{
		initializeLogo();

		const int width = state.width;
		const int height = state.height;
		const int width2 = (state.width + 1) & ~1;
		const int dBytes = state.depth / 8;
		const int dStride = state.stride;
		const int sBytes = state.HDR ? 8 : 4;
		const int sStride = topLeftOrigin ? (sBytes * width2) : -(sBytes * width2);

	//	char compareApp[32] = SCRAMBLE31(validationApp, APPNAME_SCRAMBLE);
	//	bool validApp = strcmp(compareApp, registeredApp) == 0;
		bool validKey = ValidateSerialNumber(validationKey, CHECKSUM_KEY, SERIAL_PREFIX);

		// Date of the end of the logo-free license
		const int endYear = 2099;
		const int endMonth = 12;
		const int endDay = 31;

		const int endDate = (endYear << 16) + (endMonth << 8) + endDay;

		time_t rawtime = time(0);
		tm *timeinfo = localtime(&rawtime);
		int year = timeinfo->tm_year + 1900;
		int month = timeinfo->tm_mon + 1;
		int day = timeinfo->tm_mday;

		int date = (year << 16) + (month << 8) + day;

		if(date > endDate)
		{
			validKey = false;
		}

		Function<Void, Pointer<Byte>, Pointer<Byte> > function;
		{
			Pointer<Byte> dst(function.arg(0));
			Pointer<Byte> src(function.arg(1));

			For(Int y = 0, y < height, y++)
			{
				Pointer<Byte> d = dst + y * dStride;
				Pointer<Byte> s = src + y * sStride;

				Int x0 = 0;

				#if DISPLAY_LOGO
					If(!Bool(validKey)/* || !Bool(validApp)*/)
					{
						If(y > height - logoHeight)
						{
							x0 = logoWidth;
							s += logoWidth * sBytes;
							d += logoWidth * dBytes;
						}
					}
				#endif

				if(state.depth == 32)
				{
					if(width2 % 4 == 0 && !state.HDR)
					{
						For(Int x = x0, x < width2, x += 4)
						{
							*Pointer<Int4>(d, 1) = *Pointer<Int4>(s, 16);

							s += 4 * sBytes;
							d += 4 * dBytes;
						}
					}
					else
					{
						For(Int x = x0, x < width2, x += 2)
						{
							Int2 c01;

							if(!state.HDR)
							{
								c01 = *Pointer<Int2>(s);
							}
							else
							{
								UShort4 c0 = As<UShort4>(Swizzle(*Pointer<Short4>(s + 0), 0xC6)) >> 8;
								UShort4 c1 = As<UShort4>(Swizzle(*Pointer<Short4>(s + 8), 0xC6)) >> 8;
									
								c01 = As<Int2>(Pack(c0, c1));
							}

							*Pointer<Int2>(d) = c01;

							s += 2 * sBytes;
							d += 2 * dBytes;
						}
					}
				}
				else if(state.depth == 24)
				{
					For(Int x = x0, x < width, x++)
					{
						if(!state.HDR)
						{
							*Pointer<Byte>(d + 0) = *Pointer<Byte>(s + 0);
							*Pointer<Byte>(d + 1) = *Pointer<Byte>(s + 1);
							*Pointer<Byte>(d + 2) = *Pointer<Byte>(s + 2);
						}
						else
						{
							*Pointer<Byte>(d + 0) = *Pointer<Byte>(s + 5);
							*Pointer<Byte>(d + 1) = *Pointer<Byte>(s + 3);
							*Pointer<Byte>(d + 2) = *Pointer<Byte>(s + 1);
						}

						s += sBytes;
						d += dBytes;
					}
				}
				else if(state.depth == 16)
				{
					For(Int x = x0, x < width, x++)
					{
						Int c;
							
						if(!state.HDR)
						{
							c = *Pointer<Int>(s);
						}
						else
						{
							UShort4 cc = As<UShort4>(Swizzle(*Pointer<Short4>(s + 0), 0xC6)) >> 8;

							c = Int(As<Int2>(Pack(cc, cc)));
						}

						*Pointer<Short>(d) = Short((c & 0x00F80000) >> 8 |
						                           (c & 0x0000FC00) >> 5 |
						                           (c & 0x000000F8) >> 3);

						s += sBytes;
						d += dBytes;
					}
				}
				else ASSERT(false);
			}

			#if DISPLAY_LOGO
				If(!Bool(validKey)/* || !Bool(validApp)*/)
				{
					UInt hash = UInt(0x0B020C04) + UInt(0xC0F090E0);   // Initial value
					UInt imageHash = S3TC_SUPPORT ? UInt(0x0F0D0700) + UInt(0xA0C0A090) : UInt(0x0207040B) + UInt(0xD0406010);

					While(hash != imageHash)
					{
						For(y = (height - 1), height - 1 - y < logoHeight, y--)
						{
							Pointer<Byte> logo = *Pointer<Pointer<Byte> >(&logoImage) + 4 * (logoHeight - height + y) * logoWidth;
							Pointer<Byte> s = src + y * sStride;
							Pointer<Byte> d = dst + y * dStride;

							For(Int x = 0, x < logoWidth, x++)
							{
								hash *= 16777619;
								hash ^= *Pointer<UInt>(logo);

								If(y >= 0 && x < width)
								{
									blend(state, d, s, logo);
								}

								logo += 4;
								s += sBytes;
								d += dBytes;
							}
						}
					}
				}
			#endif

			Int x0 = *Pointer<Int>(&cursorX);
			Int y0 = *Pointer<Int>(&cursorY);

			For(Int y1 = 0, y1 < cursorHeight, y1++)
			{
				Int y = y0 + y1;

				If(y >= 0 && y < height)
				{
					Pointer<Byte> d = dst + y * dStride + x0 * dBytes;
					Pointer<Byte> s = src + y * sStride + x0 * sBytes;
					Pointer<Byte> c = *Pointer<Pointer<Byte> >(&cursor) + y1 * cursorWidth * 4;

					For(Int x1 = 0, x1 < cursorWidth, x1++)
					{
						Int x = x0 + x1;

						If(x >= 0 && x < width)
						{
							blend(state, d, s, c);
						}
						
						c += 4;
						s += sBytes;
						d += dBytes;
					}
				}
			}
		}

		return function(L"FrameBuffer");
	}

	void FrameBuffer::blend(const BlitState &state, const Pointer<Byte> &d, const Pointer<Byte> &s, const Pointer<Byte> &c)
	{
		Short4 c1;
		Short4 c2;

		c1 = UnpackLow(As<Byte8>(c1), *Pointer<Byte8>(c));
		
		if(!state.HDR)
		{
			c2 = UnpackLow(As<Byte8>(c2), *Pointer<Byte8>(s));
		}
		else
		{
			c2 = Swizzle(*Pointer<Short4>(s + 0), 0xC6);
		}

		c1 = As<Short4>(As<UShort4>(c1) >> 9);
		c2 = As<Short4>(As<UShort4>(c2) >> 9);

		Short4 alpha = Swizzle(c1, 0xFF) & Short4(0xFFFFu, 0xFFFFu, 0xFFFFu, 0x0000);

		c1 = (c1 - c2) * alpha;
		c1 = c1 >> 7;
		c1 = c1 + c2;
		c1 = c1 + c1;

		c1 = As<Short4>(Pack(As<UShort4>(c1), As<UShort4>(c1)));

		if(state.depth == 32)
		{
			*Pointer<UInt>(d) = UInt(As<Long>(c1));
		}
		else if(state.depth == 24)
		{
			Int c = Int(As<Int2>(c1));

			*Pointer<Byte>(d + 0) = Byte(c >> 0);
			*Pointer<Byte>(d + 1) = Byte(c >> 8);
			*Pointer<Byte>(d + 2) = Byte(c >> 16);
		}
		else if(state.depth == 16)
		{
			Int c = Int(As<Int2>(c1));

			*Pointer<Short>(d) = Short((c & 0x00F80000) >> 8 |
			                           (c & 0x0000FC00) >> 5 |
			                           (c & 0x000000F8) >> 3);
		}
		else ASSERT(false);
	}

	void FrameBuffer::threadFunction(void *parameters)
	{
		FrameBuffer *frameBuffer = *static_cast<FrameBuffer**>(parameters);

		while(!frameBuffer->terminate)
		{
			frameBuffer->blitEvent.wait();

			if(!frameBuffer->terminate)
			{
				frameBuffer->copyLocked();

				frameBuffer->syncEvent.signal();
			}
		}
	}

	void FrameBuffer::initializeLogo()
	{
		#if DISPLAY_LOGO
			if(!logo)
			{
				#if S3TC_SUPPORT
					logo = new Surface(0, logoWidth, logoHeight, 1, FORMAT_DXT5, true, false);
					void *data = logo->lockExternal(0, 0, 0, LOCK_WRITEONLY, sw::PUBLIC);
					memcpy(data, logoData, logoWidth * logoHeight);
					logo->unlockExternal();
				#else
					logo = new Surface(0, logoWidth, logoHeight, 1, FORMAT_A8R8G8B8, true, false);
					void *data = logo->lockExternal(0, 0, 0, LOCK_WRITEONLY, sw::PUBLIC);
					memcpy(data, logoData, logoWidth * logoHeight * 4);
					logo->unlockExternal();
				#endif

				logoImage = (unsigned int*)logo->lockInternal(0, 0, 0, LOCK_READONLY, sw::PUBLIC);
				logo->unlockInternal();
			}
		#endif
	}
}