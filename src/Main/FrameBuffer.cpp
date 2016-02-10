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

#ifdef __ANDROID__
#include <cutils/properties.h>
#endif

#undef DISPLAY_LOGO
#define DISPLAY_LOGO 0

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
		destFormat = FORMAT_X8R8G8B8;
		sourceFormat = FORMAT_X8R8G8B8;
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
		blitState.destFormat = FORMAT_X8R8G8B8;
		blitState.sourceFormat = FORMAT_X8R8G8B8;
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

			cursorWidth = cursorImage->getWidth();
			cursorHeight = cursorImage->getHeight();
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

	void FrameBuffer::copy(void *source, Format format, size_t stride)
	{
		if(!source)
		{
			return;
		}

		if(!lock())
		{
			return;
		}

		sourceFormat = format;

		if(topLeftOrigin)
		{
			target = source;
		}
		else
		{
			target = (byte*)source + (height - 1) * stride;
		}

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

		profiler.nextFrame();   // Assumes every copy() is a full frame
	}

	void FrameBuffer::copyLocked()
	{
		BlitState update = {};
		update.width = width;
		update.height = height;
		update.destFormat = destFormat;
		update.sourceFormat = sourceFormat;
		update.stride = stride;
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
		const int dBytes = Surface::bytes(state.destFormat);
		const int dStride = state.stride;
		const int sBytes = Surface::bytes(state.sourceFormat);
		const int sStride = topLeftOrigin ? (sBytes * width2) : -(sBytes * width2);

		#ifdef __ANDROID__
			char ro_product_model[PROPERTY_VALUE_MAX] = "";
    		property_get("ro.product.model", ro_product_model, nullptr);
			bool validKey = strstr(ro_product_model, "Android") != nullptr;
		#else
			bool validKey = ValidateSerialNumber(validationKey, CHECKSUM_KEY, SERIAL_PREFIX);
		#endif

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

				switch(state.destFormat)
				{
				case FORMAT_X8R8G8B8:
				case FORMAT_A8R8G8B8:
					{
						Int x = x0;

						switch(state.sourceFormat)
						{
						case FORMAT_X8R8G8B8:
						case FORMAT_A8R8G8B8:
							For(, x < width - 3, x += 4)
							{
								*Pointer<Int4>(d, 1) = *Pointer<Int4>(s, sStride % 16 ? 1 : 16);

								s += 4 * sBytes;
								d += 4 * dBytes;
							}
							break;
						case FORMAT_X8B8G8R8:
						case FORMAT_A8B8G8R8:
							For(, x < width - 3, x += 4)
							{
								Int4 bgra = *Pointer<Int4>(s, sStride % 16 ? 1 : 16);

								*Pointer<Int4>(d, 1) = ((bgra & Int4(0x00FF0000)) >> 16) |
								                       ((bgra & Int4(0x000000FF)) << 16) |
								                       (bgra & Int4(0xFF00FF00));

								s += 4 * sBytes;
								d += 4 * dBytes;
							}
							break;
						case FORMAT_A16B16G16R16:
							For(, x < width - 1, x += 2)
							{
								UShort4 c0 = As<UShort4>(Swizzle(*Pointer<Short4>(s + 0), 0xC6)) >> 8;
								UShort4 c1 = As<UShort4>(Swizzle(*Pointer<Short4>(s + 8), 0xC6)) >> 8;

								*Pointer<Int2>(d) = As<Int2>(Pack(c0, c1));

								s += 2 * sBytes;
								d += 2 * dBytes;
							}
							break;
						case FORMAT_R5G6B5:
							For(, x < width, x++)
							{
								Int rgb = Int(*Pointer<Short>(s));

								*Pointer<Int>(d) = 0xFF000000 |
								                   ((rgb & 0xF800) << 8) | ((rgb & 0xE01F) << 3) |
								                   ((rgb & 0x07E0) << 5) | ((rgb & 0x0600) >> 1) |
								                   ((rgb & 0x001C) >> 2);

								s += sBytes;
								d += dBytes;
							}
							break;
						default:
							ASSERT(false);
							break;
						}

						For(, x < width, x++)
						{
							switch(state.sourceFormat)
							{
							case FORMAT_X8R8G8B8:
							case FORMAT_A8R8G8B8:
								*Pointer<Int>(d) = *Pointer<Int>(s);
								break;
							case FORMAT_X8B8G8R8:
							case FORMAT_A8B8G8R8:
								{
									Int rgba = *Pointer<Int>(s);

									*Pointer<Int>(d) = ((rgba & Int(0x00FF0000)) >> 16) |
									                   ((rgba & Int(0x000000FF)) << 16) |
									                   (rgba & Int(0xFF00FF00));
								}
								break;
							case FORMAT_A16B16G16R16:
								{
									UShort4 c = As<UShort4>(Swizzle(*Pointer<Short4>(s), 0xC6)) >> 8;

									*Pointer<Int>(d) = Int(As<Int2>(Pack(c, c)));
								}
								break;
							case FORMAT_R5G6B5:
								{
									Int rgb = Int(*Pointer<Short>(s));

									*Pointer<Int>(d) = 0xFF000000 |
									                   ((rgb & 0xF800) << 8) | ((rgb & 0xE01F) << 3) |
								                       ((rgb & 0x07E0) << 5) | ((rgb & 0x0600) >> 1) |
								                       ((rgb & 0x001C) >> 2);
								}
								break;
							default:
								ASSERT(false);
								break;
							}

							s += sBytes;
							d += dBytes;
						}
					}
					break;
				case FORMAT_X8B8G8R8:
				case FORMAT_A8B8G8R8:
					{
						Int x = x0;

						switch(state.sourceFormat)
						{
						case FORMAT_X8B8G8R8:
						case FORMAT_A8B8G8R8:
							For(, x < width - 3, x += 4)
							{
								*Pointer<Int4>(d, 1) = *Pointer<Int4>(s, sStride % 16 ? 1 : 16);

								s += 4 * sBytes;
								d += 4 * dBytes;
							}
							break;
						case FORMAT_X8R8G8B8:
						case FORMAT_A8R8G8B8:
							For(, x < width - 3, x += 4)
							{
								Int4 bgra = *Pointer<Int4>(s, sStride % 16 ? 1 : 16);

								*Pointer<Int4>(d, 1) = ((bgra & Int4(0x00FF0000)) >> 16) |
								                       ((bgra & Int4(0x000000FF)) << 16) |
								                       (bgra & Int4(0xFF00FF00));

								s += 4 * sBytes;
								d += 4 * dBytes;
							}
							break;
						case FORMAT_A16B16G16R16:
							For(, x < width - 1, x += 2)
							{
								UShort4 c0 = *Pointer<UShort4>(s + 0) >> 8;
								UShort4 c1 = *Pointer<UShort4>(s + 8) >> 8;

								*Pointer<Int2>(d) = As<Int2>(Pack(c0, c1));

								s += 2 * sBytes;
								d += 2 * dBytes;
							}
							break;
						case FORMAT_R5G6B5:
							For(, x < width, x++)
							{
								Int rgb = Int(*Pointer<Short>(s));

								*Pointer<Int>(d) = 0xFF000000 |
								                   ((rgb & 0x001F) << 19) | ((rgb & 0x001C) << 14) |
								                   ((rgb & 0x07E0) << 5) | ((rgb & 0x0600) >> 1) |
								                   ((rgb & 0xF800) >> 8) | ((rgb & 0xE000) >> 13);

								s += sBytes;
								d += dBytes;
							}
							break;
						default:
							ASSERT(false);
							break;
						}

						For(, x < width, x++)
						{
							switch(state.sourceFormat)
							{
							case FORMAT_X8B8G8R8:
							case FORMAT_A8B8G8R8:
								*Pointer<Int>(d) = *Pointer<Int>(s);
								break;
							case FORMAT_X8R8G8B8:
							case FORMAT_A8R8G8B8:
								{
									Int bgra = *Pointer<Int>(s);
									*Pointer<Int>(d) = ((bgra & Int(0x00FF0000)) >> 16) |
									                   ((bgra & Int(0x000000FF)) << 16) |
									                   (bgra & Int(0xFF00FF00));
								}
								break;
							case FORMAT_A16B16G16R16:
								{
									UShort4 c = *Pointer<UShort4>(s) >> 8;

									*Pointer<Int>(d) = Int(As<Int2>(Pack(c, c)));
								}
								break;
							case FORMAT_R5G6B5:
								{
									Int rgb = Int(*Pointer<Short>(s));

									*Pointer<Int>(d) = 0xFF000000 |
									                   ((rgb & 0x001F) << 19) | ((rgb & 0x001C) << 14) |
								                       ((rgb & 0x07E0) << 5) | ((rgb & 0x0600) >> 1) |
								                       ((rgb & 0xF800) >> 8) | ((rgb & 0xE000) >> 13);
								}
								break;
							default:
								ASSERT(false);
								break;
							}

							s += sBytes;
							d += dBytes;
						}
					}
					break;
				case FORMAT_R8G8B8:
					{
						For(Int x = x0, x < width, x++)
						{
							switch(state.sourceFormat)
							{
							case FORMAT_X8R8G8B8:
							case FORMAT_A8R8G8B8:
								*Pointer<Byte>(d + 0) = *Pointer<Byte>(s + 0);
								*Pointer<Byte>(d + 1) = *Pointer<Byte>(s + 1);
								*Pointer<Byte>(d + 2) = *Pointer<Byte>(s + 2);
								break;
							case FORMAT_X8B8G8R8:
							case FORMAT_A8B8G8R8:
								*Pointer<Byte>(d + 0) = *Pointer<Byte>(s + 2);
								*Pointer<Byte>(d + 1) = *Pointer<Byte>(s + 1);
								*Pointer<Byte>(d + 2) = *Pointer<Byte>(s + 0);
								break;
							case FORMAT_A16B16G16R16:
								*Pointer<Byte>(d + 0) = *Pointer<Byte>(s + 5);
								*Pointer<Byte>(d + 1) = *Pointer<Byte>(s + 3);
								*Pointer<Byte>(d + 2) = *Pointer<Byte>(s + 1);
								break;
							case FORMAT_R5G6B5:
								{
									Int rgb = Int(*Pointer<Short>(s));

									*Pointer<Byte>(d + 0) = Byte(((rgb & 0x001F) << 3) | ((rgb & 0x001C) >> 2));
									*Pointer<Byte>(d + 1) = Byte(((rgb & 0x07E0) << 5) | ((rgb & 0x0600) >> 1));
									*Pointer<Byte>(d + 2) = Byte(((rgb & 0xF800) << 8) | ((rgb & 0xE000) << 3));
								}
								break;
							default:
								ASSERT(false);
								break;
							}

							s += sBytes;
							d += dBytes;
						}
					}
					break;
				case FORMAT_R5G6B5:
					{
						For(Int x = x0, x < width, x++)
						{
							switch(state.sourceFormat)
							{
							case FORMAT_X8R8G8B8:
							case FORMAT_A8R8G8B8:
								{
									Int c = *Pointer<Int>(s);

									*Pointer<Short>(d) = Short((c & 0x00F80000) >> 8 |
									                           (c & 0x0000FC00) >> 5 |
									                           (c & 0x000000F8) >> 3);
								}
								break;
							case FORMAT_X8B8G8R8:
							case FORMAT_A8B8G8R8:
								{
									Int c = *Pointer<Int>(s);

									*Pointer<Short>(d) = Short((c & 0x00F80000) >> 19 |
									                           (c & 0x0000FC00) >> 5 |
									                           (c & 0x000000F8) << 8);
								}
								break;
							case FORMAT_A16B16G16R16:
								{
									UShort4 cc = *Pointer<UShort4>(s) >> 8;
									Int c = Int(As<Int2>(Pack(cc, cc)));

									*Pointer<Short>(d) = Short((c & 0x00F80000) >> 19 |
									                           (c & 0x0000FC00) >> 5 |
									                           (c & 0x000000F8) << 8);
								}
								break;
							case FORMAT_R5G6B5:
								*Pointer<Short>(d) = *Pointer<Short>(s);
								break;
							default:
								ASSERT(false);
								break;
							}

							s += sBytes;
							d += dBytes;
						}
					}
					break;
				default:
					ASSERT(false);
					break;
				}
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

		switch(state.sourceFormat)
		{
		case FORMAT_X8R8G8B8:
		case FORMAT_A8R8G8B8:
			c2 = UnpackLow(As<Byte8>(c2), *Pointer<Byte8>(s));
			break;
		case FORMAT_X8B8G8R8:
		case FORMAT_A8B8G8R8:
			c2 = Swizzle(UnpackLow(As<Byte8>(c2), *Pointer<Byte8>(s)), 0xC6);
			break;
		case FORMAT_A16B16G16R16:
			c2 = Swizzle(*Pointer<Short4>(s), 0xC6);
			break;
		case FORMAT_R5G6B5:
			{
				Int rgb(*Pointer<Short>(s));
				rgb = 0xFF000000 |
				      ((rgb & 0xF800) << 8) | ((rgb & 0xE01F) << 3) |
				      ((rgb & 0x07E0) << 5) | ((rgb & 0x0600) >> 1) |
				      ((rgb & 0x001C) >> 2);
				c2 = Unpack(As<Byte4>(rgb));
			}
			break;
		default:
			ASSERT(false);
			break;
		}

		c1 = As<Short4>(As<UShort4>(c1) >> 9);
		c2 = As<Short4>(As<UShort4>(c2) >> 9);

		Short4 alpha = Swizzle(c1, 0xFF) & Short4(0xFFFFu, 0xFFFFu, 0xFFFFu, 0x0000);

		c1 = (c1 - c2) * alpha;
		c1 = c1 >> 7;
		c1 = c1 + c2;
		c1 = c1 + c1;

		switch(state.destFormat)
		{
		case FORMAT_X8R8G8B8:
		case FORMAT_A8R8G8B8:
			*Pointer<UInt>(d) = UInt(As<Long>(Pack(As<UShort4>(c1), As<UShort4>(c1))));
			break;
		case FORMAT_X8B8G8R8:
		case FORMAT_A8B8G8R8:
			{
				c1 = Swizzle(c1, 0xC6);

				*Pointer<UInt>(d) = UInt(As<Long>(Pack(As<UShort4>(c1), As<UShort4>(c1))));
			}
			break;
		case FORMAT_R8G8B8:
			{
				Int c = Int(As<Int2>(Pack(As<UShort4>(c1), As<UShort4>(c1))));

				*Pointer<Byte>(d + 0) = Byte(c >> 0);
				*Pointer<Byte>(d + 1) = Byte(c >> 8);
				*Pointer<Byte>(d + 2) = Byte(c >> 16);
			}
			break;
		case FORMAT_R5G6B5:
			{
				Int c = Int(As<Int2>(Pack(As<UShort4>(c1), As<UShort4>(c1))));

				*Pointer<Short>(d) = Short((c & 0x00F80000) >> 8 |
				                           (c & 0x0000FC00) >> 5 |
				                           (c & 0x000000F8) >> 3);
			}
			break;
		default:
			ASSERT(false);
			break;
		}
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
