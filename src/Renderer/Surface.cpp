// SwiftShader Software Renderer
//
// Copyright(c) 2005-2013 TransGaming Inc.
//
// All rights reserved. No part of this software may be copied, distributed, transmitted,
// transcribed, stored in a retrieval system, translated into any human or computer
// language by any means, or disclosed to third parties without the explicit written
// agreement of TransGaming Inc. Without such an agreement, no rights or licenses, express
// or implied, including but not limited to any patent rights, are granted to you.
//

#include "Surface.hpp"

#include "Color.hpp"
#include "Context.hpp"
#include "Renderer.hpp"
#include "Common/Half.hpp"
#include "Common/Memory.hpp"
#include "Common/CPUID.hpp"
#include "Common/Resource.hpp"
#include "Common/Debug.hpp"
#include "Reactor/Reactor.hpp"

#include <xmmintrin.h>
#include <emmintrin.h>

#undef min
#undef max

namespace sw
{
	extern bool quadLayoutEnabled;
	extern bool complementaryDepthBuffer;
	extern TranscendentalPrecision logPrecision;

	unsigned int *Surface::palette = 0;
	unsigned int Surface::paletteID = 0;

	void Rect::clip(int minX, int minY, int maxX, int maxY)
	{
		x0 = clamp(x0, minX, maxX);
		y0 = clamp(y0, minY, maxY);
		x1 = clamp(x1, minX, maxX);
		y1 = clamp(y1, minY, maxY);
	}

	void Surface::Buffer::write(int x, int y, int z, const Color<float> &color)
	{
		void *element = (unsigned char*)buffer + x * bytes + y * pitchB + z * sliceB;

		write(element, color);
	}

	void Surface::Buffer::write(int x, int y, const Color<float> &color)
	{
		void *element = (unsigned char*)buffer + x * bytes + y * pitchB;

		write(element, color);
	}

	inline void Surface::Buffer::write(void *element, const Color<float> &color)
	{
		switch(format)
		{
		case FORMAT_A8:
			*(unsigned char*)element = unorm<8>(color.a);
			break;
		case FORMAT_R8:
			*(unsigned char*)element = unorm<8>(color.r);
			break;
		case FORMAT_R3G3B2:
			*(unsigned char*)element = (unorm<3>(color.r) << 5) | (unorm<3>(color.g) << 2) | (unorm<2>(color.b) << 0);
			break;
		case FORMAT_A8R3G3B2:
			*(unsigned short*)element = (unorm<8>(color.a) << 8) | (unorm<3>(color.r) << 5) | (unorm<3>(color.g) << 2) | (unorm<2>(color.b) << 0);
			break;
		case FORMAT_X4R4G4B4:
			*(unsigned short*)element = 0xF000 | (unorm<4>(color.r) << 8) | (unorm<4>(color.g) << 4) | (unorm<4>(color.b) << 0);
			break;
		case FORMAT_A4R4G4B4:
			*(unsigned short*)element = (unorm<4>(color.a) << 12) | (unorm<4>(color.r) << 8) | (unorm<4>(color.g) << 4) | (unorm<4>(color.b) << 0);
			break;
		case FORMAT_R5G6B5:
			*(unsigned short*)element = (unorm<5>(color.r) << 11) | (unorm<6>(color.g) << 5) | (unorm<5>(color.b) << 0);
			break;
		case FORMAT_A1R5G5B5:
			*(unsigned short*)element = (unorm<1>(color.a) << 15) | (unorm<5>(color.r) << 10) | (unorm<5>(color.g) << 5) | (unorm<5>(color.b) << 0);
			break;
		case FORMAT_X1R5G5B5:
			*(unsigned short*)element = 0x8000 | (unorm<5>(color.r) << 10) | (unorm<5>(color.g) << 5) | (unorm<5>(color.b) << 0);
			break;
		case FORMAT_A8R8G8B8:
			*(unsigned int*)element = (unorm<8>(color.a) << 24) | (unorm<8>(color.r) << 16) | (unorm<8>(color.g) << 8) | (unorm<8>(color.b) << 0);
			break;
		case FORMAT_X8R8G8B8:
			*(unsigned int*)element = 0xFF000000 | (unorm<8>(color.r) << 16) | (unorm<8>(color.g) << 8) | (unorm<8>(color.b) << 0);
			break;
		case FORMAT_A8B8G8R8:
			*(unsigned int*)element = (unorm<8>(color.a) << 24) | (unorm<8>(color.b) << 16) | (unorm<8>(color.g) << 8) | (unorm<8>(color.r) << 0);
			break;
		case FORMAT_X8B8G8R8:
			*(unsigned int*)element = 0xFF000000 | (unorm<8>(color.b) << 16) | (unorm<8>(color.g) << 8) | (unorm<8>(color.r) << 0);
			break;
		case FORMAT_A2R10G10B10:
			*(unsigned int*)element = (unorm<2>(color.a) << 30) | (unorm<10>(color.r) << 20) | (unorm<10>(color.g) << 10) | (unorm<10>(color.b) << 0);
			break;
		case FORMAT_A2B10G10R10:
			*(unsigned int*)element = (unorm<2>(color.a) << 30) | (unorm<10>(color.b) << 20) | (unorm<10>(color.g) << 10) | (unorm<10>(color.r) << 0);
			break;
		case FORMAT_G8R8:
			*(unsigned int*)element = (unorm<8>(color.g) << 8) | (unorm<8>(color.r) << 0);
			break;
		case FORMAT_G16R16:
			*(unsigned int*)element = (unorm<16>(color.g) << 16) | (unorm<16>(color.r) << 0);
			break;
		case FORMAT_A16B16G16R16:
			((unsigned short*)element)[0] = unorm<16>(color.r);
			((unsigned short*)element)[1] = unorm<16>(color.g);
			((unsigned short*)element)[2] = unorm<16>(color.b);
			((unsigned short*)element)[3] = unorm<16>(color.a);
			break;
		case FORMAT_V8U8:
			*(unsigned short*)element = (snorm<8>(color.g) << 8) | (snorm<8>(color.r) << 0);
			break;
		case FORMAT_L6V5U5:
			*(unsigned short*)element = (unorm<6>(color.b) << 10) | (snorm<5>(color.g) << 5) | (snorm<5>(color.r) << 0);
			break;
		case FORMAT_Q8W8V8U8:
			*(unsigned int*)element = (snorm<8>(color.a) << 24) | (snorm<8>(color.b) << 16) | (snorm<8>(color.g) << 8) | (snorm<8>(color.r) << 0);
			break;
		case FORMAT_X8L8V8U8:
			*(unsigned int*)element = 0xFF000000 | (unorm<8>(color.b) << 16) | (snorm<8>(color.g) << 8) | (snorm<8>(color.r) << 0);
			break;
		case FORMAT_V16U16:
			*(unsigned int*)element = (snorm<16>(color.g) << 16) | (snorm<16>(color.r) << 0);
			break;
		case FORMAT_A2W10V10U10:
			*(unsigned int*)element = (unorm<2>(color.a) << 30) | (snorm<10>(color.b) << 20) | (snorm<10>(color.g) << 10) | (snorm<10>(color.r) << 0);
			break;
		case FORMAT_A16W16V16U16:
			((unsigned short*)element)[0] = snorm<16>(color.r);
			((unsigned short*)element)[1] = snorm<16>(color.g);
			((unsigned short*)element)[2] = snorm<16>(color.b);
			((unsigned short*)element)[3] = unorm<16>(color.a);
			break;
		case FORMAT_Q16W16V16U16:
			((unsigned short*)element)[0] = snorm<16>(color.r);
			((unsigned short*)element)[1] = snorm<16>(color.g);
			((unsigned short*)element)[2] = snorm<16>(color.b);
			((unsigned short*)element)[3] = snorm<16>(color.a);
			break;
		case FORMAT_R8G8B8:
			((unsigned char*)element)[0] = unorm<8>(color.b);
			((unsigned char*)element)[1] = unorm<8>(color.g);
			((unsigned char*)element)[2] = unorm<8>(color.r);
			break;
		case FORMAT_R16F:
			*(half*)element = (half)color.r;
			break;
		case FORMAT_G16R16F:
			((half*)element)[0] = (half)color.r;
			((half*)element)[1] = (half)color.g;
			break;
		case FORMAT_A16B16G16R16F:
			((half*)element)[0] = (half)color.r;
			((half*)element)[1] = (half)color.g;
			((half*)element)[2] = (half)color.b;
			((half*)element)[3] = (half)color.a;
			break;
		case FORMAT_R32F:
			*(float*)element = color.r;
			break;
		case FORMAT_G32R32F:
			((float*)element)[0] = color.r;
			((float*)element)[1] = color.g;
			break;
		case FORMAT_A32B32G32R32F:
			((float*)element)[0] = color.r;
			((float*)element)[1] = color.g;
			((float*)element)[2] = color.b;
			((float*)element)[3] = color.a;
			break;
		case FORMAT_D32F:
		case FORMAT_D32F_LOCKABLE:
		case FORMAT_D32FS8_TEXTURE:
		case FORMAT_D32FS8_SHADOW:
			*((float*)element) = color.r;
			break;
		case FORMAT_D32F_COMPLEMENTARY:
			*((float*)element) = 1 - color.r;
			break;
		case FORMAT_S8:
			*((unsigned char*)element) = unorm<8>(color.r);
			break;
		case FORMAT_L8:
			*(unsigned char*)element = unorm<8>(color.r);
			break;
		case FORMAT_A4L4:
			*(unsigned char*)element = (unorm<4>(color.a) << 4) | (unorm<4>(color.r) << 0);
			break;
		case FORMAT_L16:
			*(unsigned short*)element = unorm<16>(color.r);
			break;
		case FORMAT_A8L8:
			*(unsigned short*)element = (unorm<8>(color.a) << 8) | (unorm<8>(color.r) << 0);
			break;
		default:
			ASSERT(false);
		}
	}

	Color<float> Surface::Buffer::read(int x, int y, int z) const
	{
		void *element = (unsigned char*)buffer + x * bytes + y * pitchB + z * sliceB;

		return read(element);
	}

	Color<float> Surface::Buffer::read(int x, int y) const
	{
		void *element = (unsigned char*)buffer + x * bytes + y * pitchB;

		return read(element);
	}

	inline Color<float> Surface::Buffer::read(void *element) const
	{
		float r = 1;
		float g = 1;
		float b = 1;
		float a = 1;

		switch(format)
		{
		case FORMAT_P8:
			{
				ASSERT(palette);

				unsigned int abgr = palette[*(unsigned char*)element];
				
				r = (abgr & 0x000000FF) * (1.0f / 0x000000FF);
				g = (abgr & 0x0000FF00) * (1.0f / 0x0000FF00);
				b = (abgr & 0x00FF0000) * (1.0f / 0x00FF0000);
				a = (abgr & 0xFF000000) * (1.0f / 0xFF000000);
			}
			break;
		case FORMAT_A8P8:
			{
				ASSERT(palette);

				unsigned int bgr = palette[((unsigned char*)element)[0]];
				
				r = (bgr & 0x000000FF) * (1.0f / 0x000000FF);
				g = (bgr & 0x0000FF00) * (1.0f / 0x0000FF00);
				b = (bgr & 0x00FF0000) * (1.0f / 0x00FF0000);
				a = ((unsigned char*)element)[1] * (1.0f / 0xFF);
			}
			break;
		case FORMAT_A8:
			r = 0;
			g = 0;
			b = 0;
			a = *(unsigned char*)element * (1.0f / 0xFF);
			break;
		case FORMAT_R8:
			r = *(unsigned char*)element * (1.0f / 0xFF);
			break;
		case FORMAT_R3G3B2:
			{
				unsigned char rgb = *(unsigned char*)element;
				
				r = (rgb & 0xE0) * (1.0f / 0xE0);
				g = (rgb & 0x1C) * (1.0f / 0x1C);
				b = (rgb & 0x03) * (1.0f / 0x03);
			}
			break;
		case FORMAT_A8R3G3B2:
			{
				unsigned short argb = *(unsigned short*)element;
				
				a = (argb & 0xFF00) * (1.0f / 0xFF00);
				r = (argb & 0x00E0) * (1.0f / 0x00E0);
				g = (argb & 0x001C) * (1.0f / 0x001C);
				b = (argb & 0x0003) * (1.0f / 0x0003);
			}
			break;
		case FORMAT_X4R4G4B4:
			{
				unsigned short rgb = *(unsigned short*)element;
				
				r = (rgb & 0x0F00) * (1.0f / 0x0F00);
				g = (rgb & 0x00F0) * (1.0f / 0x00F0);
				b = (rgb & 0x000F) * (1.0f / 0x000F);
			}
			break;
		case FORMAT_A4R4G4B4:
			{
				unsigned short argb = *(unsigned short*)element;
				
				a = (argb & 0xF000) * (1.0f / 0xF000);
				r = (argb & 0x0F00) * (1.0f / 0x0F00);
				g = (argb & 0x00F0) * (1.0f / 0x00F0);
				b = (argb & 0x000F) * (1.0f / 0x000F);
			}
			break;
		case FORMAT_R5G6B5:
			{
				unsigned short rgb = *(unsigned short*)element;
				
				r = (rgb & 0xF800) * (1.0f / 0xF800);
				g = (rgb & 0x07E0) * (1.0f / 0x07E0);
				b = (rgb & 0x001F) * (1.0f / 0x001F);
			}
			break;
		case FORMAT_A1R5G5B5:
			{
				unsigned short argb = *(unsigned short*)element;
				
				a = (argb & 0x8000) * (1.0f / 0x8000);
				r = (argb & 0x7C00) * (1.0f / 0x7C00);
				g = (argb & 0x03E0) * (1.0f / 0x03E0);
				b = (argb & 0x001F) * (1.0f / 0x001F);
			}
			break;
		case FORMAT_X1R5G5B5:
			{
				unsigned short xrgb = *(unsigned short*)element;
				
				r = (xrgb & 0x7C00) * (1.0f / 0x7C00);
				g = (xrgb & 0x03E0) * (1.0f / 0x03E0);
				b = (xrgb & 0x001F) * (1.0f / 0x001F);
			}
			break;
		case FORMAT_A8R8G8B8:
			{
				unsigned int argb = *(unsigned int*)element;
				
				a = (argb & 0xFF000000) * (1.0f / 0xFF000000);
				r = (argb & 0x00FF0000) * (1.0f / 0x00FF0000);
				g = (argb & 0x0000FF00) * (1.0f / 0x0000FF00);
				b = (argb & 0x000000FF) * (1.0f / 0x000000FF);
			}
			break;
		case FORMAT_X8R8G8B8:
			{
				unsigned int xrgb = *(unsigned int*)element;
				
				r = (xrgb & 0x00FF0000) * (1.0f / 0x00FF0000);
				g = (xrgb & 0x0000FF00) * (1.0f / 0x0000FF00);
				b = (xrgb & 0x000000FF) * (1.0f / 0x000000FF);
			}
			break;
		case FORMAT_A8B8G8R8:
			{
				unsigned int abgr = *(unsigned int*)element;
				
				a = (abgr & 0xFF000000) * (1.0f / 0xFF000000);
				b = (abgr & 0x00FF0000) * (1.0f / 0x00FF0000);
				g = (abgr & 0x0000FF00) * (1.0f / 0x0000FF00);
				r = (abgr & 0x000000FF) * (1.0f / 0x000000FF);
			}
			break;
		case FORMAT_X8B8G8R8:
			{
				unsigned int xbgr = *(unsigned int*)element;
				
				b = (xbgr & 0x00FF0000) * (1.0f / 0x00FF0000);
				g = (xbgr & 0x0000FF00) * (1.0f / 0x0000FF00);
				r = (xbgr & 0x000000FF) * (1.0f / 0x000000FF);
			}
			break;
		case FORMAT_G8R8:
			{
				unsigned short gr = *(unsigned short*)element;
				
				g = (gr & 0xFF00) * (1.0f / 0xFF00);
				r = (gr & 0x00FF) * (1.0f / 0x00FF);
			}
			break;
		case FORMAT_G16R16:
			{
				unsigned int gr = *(unsigned int*)element;
				
				g = (gr & 0xFFFF0000) * (1.0f / 0xFFFF0000);
				r = (gr & 0x0000FFFF) * (1.0f / 0x0000FFFF);
			}
			break;
		case FORMAT_A2R10G10B10:
			{
				unsigned int argb = *(unsigned int*)element;
				
				a = (argb & 0xC0000000) * (1.0f / 0xC0000000);
				r = (argb & 0x3FF00000) * (1.0f / 0x3FF00000);
				g = (argb & 0x000FFC00) * (1.0f / 0x000FFC00);
				b = (argb & 0x000003FF) * (1.0f / 0x000003FF);
			}
			break;
		case FORMAT_A2B10G10R10:
			{
				unsigned int abgr = *(unsigned int*)element;
				
				a = (abgr & 0xC0000000) * (1.0f / 0xC0000000);
				b = (abgr & 0x3FF00000) * (1.0f / 0x3FF00000);
				g = (abgr & 0x000FFC00) * (1.0f / 0x000FFC00);
				r = (abgr & 0x000003FF) * (1.0f / 0x000003FF);
			}
			break;
		case FORMAT_A16B16G16R16:
			r = ((unsigned short*)element)[0] * (1.0f / 0xFFFF);
			g = ((unsigned short*)element)[1] * (1.0f / 0xFFFF);
			b = ((unsigned short*)element)[2] * (1.0f / 0xFFFF);
			a = ((unsigned short*)element)[3] * (1.0f / 0xFFFF);
			break;
		case FORMAT_V8U8:
			{
				unsigned short vu = *(unsigned short*)element;

				r = ((int)(vu & 0x00FF) << 24) * (1.0f / 0x7F000000);
				g = ((int)(vu & 0xFF00) << 16) * (1.0f / 0x7F000000);
			}
			break;
		case FORMAT_L6V5U5:
			{
				unsigned short lvu = *(unsigned short*)element;
				
				r = ((int)(lvu & 0x001F) << 27) * (1.0f / 0x78000000);
				g = ((int)(lvu & 0x03E0) << 22) * (1.0f / 0x78000000);
				b = (lvu & 0xFC00) * (1.0f / 0xFC00);
			}
			break;
		case FORMAT_Q8W8V8U8:
			{
				unsigned int qwvu = *(unsigned int*)element;
				
				r = ((int)(qwvu & 0x000000FF) << 24) * (1.0f / 0x7F000000);
				g = ((int)(qwvu & 0x0000FF00) << 16) * (1.0f / 0x7F000000);
				b = ((int)(qwvu & 0x00FF0000) << 8)  * (1.0f / 0x7F000000);
				a = ((int)(qwvu & 0xFF000000) << 0)  * (1.0f / 0x7F000000);
			}
			break;
		case FORMAT_X8L8V8U8:
			{
				unsigned int xlvu = *(unsigned int*)element;
				
				r = ((int)(xlvu & 0x000000FF) << 24) * (1.0f / 0x7F000000);
				g = ((int)(xlvu & 0x0000FF00) << 16) * (1.0f / 0x7F000000);
				b = (xlvu & 0x00FF0000) * (1.0f / 0x00FF0000);
			}
			break;
		case FORMAT_R8G8B8:
			r = ((unsigned char*)element)[2] * (1.0f / 0xFF);
			g = ((unsigned char*)element)[1] * (1.0f / 0xFF);
			b = ((unsigned char*)element)[0] * (1.0f / 0xFF);
			break;
		case FORMAT_V16U16:
			{
				unsigned int vu = *(unsigned int*)element;
				
				r = ((int)(vu & 0x0000FFFF) << 16) * (1.0f / 0x7FFF0000);
				g = ((int)(vu & 0xFFFF0000) << 0)  * (1.0f / 0x7FFF0000);
			}
			break;
		case FORMAT_A2W10V10U10:
			{
				unsigned int awvu = *(unsigned int*)element;
				
				r = ((int)(awvu & 0x000003FF) << 22) * (1.0f / 0x7FC00000);
				g = ((int)(awvu & 0x000FFC00) << 12) * (1.0f / 0x7FC00000);
				b = ((int)(awvu & 0x3FF00000) << 2)  * (1.0f / 0x7FC00000);
				a = (awvu & 0xC0000000) * (1.0f / 0xC0000000);
			}
			break;
		case FORMAT_A16W16V16U16:
			r = ((signed short*)element)[0] * (1.0f / 0x7FFF);
			g = ((signed short*)element)[1] * (1.0f / 0x7FFF);
			b = ((signed short*)element)[2] * (1.0f / 0x7FFF);
			a = ((unsigned short*)element)[3] * (1.0f / 0xFFFF);
			break;
		case FORMAT_Q16W16V16U16:
			r = ((signed short*)element)[0] * (1.0f / 0x7FFF);
			g = ((signed short*)element)[1] * (1.0f / 0x7FFF);
			b = ((signed short*)element)[2] * (1.0f / 0x7FFF);
			a = ((signed short*)element)[3] * (1.0f / 0x7FFF);
			break;
		case FORMAT_L8:
			r =
			g =
			b = *(unsigned char*)element * (1.0f / 0xFF);
			break;
		case FORMAT_A4L4:
			{
				unsigned char al = *(unsigned char*)element;
				
				r =
				g =
				b = (al & 0x0F) * (1.0f / 0x0F);
				a = (al & 0xF0) * (1.0f / 0xF0);
			}
			break;
		case FORMAT_L16:
			r =
			g =
			b = *(unsigned short*)element * (1.0f / 0xFFFF);
			break;
		case FORMAT_A8L8:
			r =
			g =
			b = ((unsigned char*)element)[0] * (1.0f / 0xFF);
			a = ((unsigned char*)element)[1] * (1.0f / 0xFF);
			break;
		case FORMAT_R16F:
			r = *(half*)element;
			break;
		case FORMAT_G16R16F:
			r = ((half*)element)[0];
			g = ((half*)element)[1];
			break;
		case FORMAT_A16B16G16R16F:
			r = ((half*)element)[0];
			g = ((half*)element)[1];
			b = ((half*)element)[2];
			a = ((half*)element)[3];
			break;
		case FORMAT_R32F:
			r = *(float*)element;
			break;
		case FORMAT_G32R32F:
			r = ((float*)element)[0];
			g = ((float*)element)[1];
			break;
		case FORMAT_A32B32G32R32F:
			r = ((float*)element)[0];
			g = ((float*)element)[1];
			b = ((float*)element)[2];
			a = ((float*)element)[3];
			break;
		case FORMAT_D32F:
		case FORMAT_D32F_LOCKABLE:
		case FORMAT_D32FS8_TEXTURE:
		case FORMAT_D32FS8_SHADOW:
			r = *(float*)element;
			g = r;
			b = r;
			a = r;
			break;
		case FORMAT_D32F_COMPLEMENTARY:
			r = 1.0f - *(float*)element;
			g = r;
			b = r;
			a = r;
			break;
		case FORMAT_S8:
			r = *(unsigned char*)element * (1.0f / 0xFF);
			break;
		default:
			ASSERT(false);
		}

	//	if(sRGB)
	//	{
	//		r = sRGBtoLinear(r);
	//		g = sRGBtoLinear(g);
	//		b = sRGBtoLinear(b);
	//	}

		return Color<float>(r, g, b, a);
	}

	Color<float> Surface::Buffer::sample(float x, float y, float z) const
	{
		x -= 0.5f;
		y -= 0.5f;
		z -= 0.5f;

		int x0 = clamp((int)x, 0, width - 1);
		int x1 = (x0 + 1 >= width) ? x0 : x0 + 1;

		int y0 = clamp((int)y, 0, height - 1);
		int y1 = (y0 + 1 >= height) ? y0 : y0 + 1;

		int z0 = clamp((int)z, 0, depth - 1);
		int z1 = (z0 + 1 >= depth) ? z0 : z0 + 1;

		Color<float> c000 = read(x0, y0, z0);
		Color<float> c100 = read(x1, y0, z0);
		Color<float> c010 = read(x0, y1, z0);
		Color<float> c110 = read(x1, y1, z0);
		Color<float> c001 = read(x0, y0, z1);
		Color<float> c101 = read(x1, y0, z1);
		Color<float> c011 = read(x0, y1, z1);
		Color<float> c111 = read(x1, y1, z1);

		float fx = x - x0;
		float fy = y - y0;
		float fz = z - z0;

		c000 *= (1 - fx) * (1 - fy) * (1 - fz);
		c100 *= fx * (1 - fy) * (1 - fz);
		c010 *= (1 - fx) * fy * (1 - fz);
		c110 *= fx * fy * (1 - fz);
		c001 *= (1 - fx) * (1 - fy) * fz;
		c101 *= fx * (1 - fy) * fz;
		c011 *= (1 - fx) * fy * fz;
		c111 *= fx * fy * fz;

		return c000 + c100 + c010 + c110 + c001 + c101 + c011 + c111;
	}

	Color<float> Surface::Buffer::sample(float x, float y) const
	{
		x -= 0.5f;
		y -= 0.5f;

		int x0 = clamp((int)x, 0, width - 1);
		int x1 = (x0 + 1 >= width) ? x0 : x0 + 1;

		int y0 = clamp((int)y, 0, height - 1);
		int y1 = (y0 + 1 >= height) ? y0 : y0 + 1;

		Color<float> c00 = read(x0, y0);
		Color<float> c10 = read(x1, y0);
		Color<float> c01 = read(x0, y1);
		Color<float> c11 = read(x1, y1);

		float fx = x - x0;
		float fy = y - y0;

		c00 *= (1 - fx) * (1 - fy);
		c10 *= fx * (1 - fy);
		c01 *= (1 - fx) * fy;
		c11 *= fx * fy;

		return c00 + c10 + c01 + c11;
	}

	void *Surface::Buffer::lockRect(int x, int y, int z, Lock lock)
	{
		this->lock = lock;

		switch(lock)
		{
		case LOCK_UNLOCKED:
		case LOCK_READONLY:
			break;
		case LOCK_WRITEONLY:
		case LOCK_READWRITE:
		case LOCK_DISCARD:
			dirty = true;
			break;
		default:
			ASSERT(false);
		}

		if(buffer)
		{
			switch(format)
			{
			#if S3TC_SUPPORT
			case FORMAT_DXT1:
			#endif
			case FORMAT_ATI1:
			case FORMAT_ETC1:
				return (unsigned char*)buffer + 8 * (x / 4) + (y / 4) * pitchB + z * sliceB;
			#if S3TC_SUPPORT
			case FORMAT_DXT3:
			case FORMAT_DXT5:
			#endif
			case FORMAT_ATI2:
				return (unsigned char*)buffer + 16 * (x / 4) + (y / 4) * pitchB + z * sliceB;
			default:
				return (unsigned char*)buffer + x * bytes + y * pitchB + z * sliceB;
			}
		}

		return 0;
	}

	void Surface::Buffer::unlockRect()
	{
		lock = LOCK_UNLOCKED;
	}

	Surface::Surface(Resource *texture, int width, int height, int depth, Format format, bool lockable, bool renderTarget) : lockable(lockable), renderTarget(renderTarget)
	{
		resource = texture ? texture : new Resource(0);
		hasParent = texture != 0;
		depth = max(1, depth);

		external.buffer = 0;
		external.width = width;
		external.height = height;
		external.depth = depth;
		external.format = format;
		external.bytes = bytes(external.format);
		external.pitchB = pitchB(external.width, external.format, renderTarget && !texture);
		external.pitchP = pitchP(external.width, external.format, renderTarget && !texture);
		external.sliceB = sliceB(external.width, external.height, external.format, renderTarget && !texture);
		external.sliceP = sliceP(external.width, external.height, external.format, renderTarget && !texture);
		external.lock = LOCK_UNLOCKED;
		external.dirty = false;

		internal.buffer = 0;
		internal.width = width;
		internal.height = height;
		internal.depth = depth;
		internal.format = selectInternalFormat(format);
		internal.bytes = bytes(internal.format);
		internal.pitchB = pitchB(internal.width, internal.format, renderTarget);
		internal.pitchP = pitchP(internal.width, internal.format, renderTarget);
		internal.sliceB = sliceB(internal.width, internal.height, internal.format, renderTarget);
		internal.sliceP = sliceP(internal.width, internal.height, internal.format, renderTarget);
		internal.lock = LOCK_UNLOCKED;
		internal.dirty = false;

		stencil.buffer = 0;
		stencil.width = width;
		stencil.height = height;
		stencil.depth = depth;
		stencil.format = FORMAT_S8;
		stencil.bytes = bytes(stencil.format);
		stencil.pitchB = pitchB(stencil.width, stencil.format, renderTarget);
		stencil.pitchP = pitchP(stencil.width, stencil.format, renderTarget);
		stencil.sliceB = sliceB(stencil.width, stencil.height, stencil.format, renderTarget);
		stencil.sliceP = sliceP(stencil.width, stencil.height, stencil.format, renderTarget);
		stencil.lock = LOCK_UNLOCKED;
		stencil.dirty = false;

		dirtyMipmaps = true;
		paletteUsed = 0;
	}

	Surface::~Surface()
	{
		// Synchronize so we can deallocate the buffers below
		resource->lock(DESTRUCT);
		resource->unlock();

		if(!hasParent)
		{
			resource->destruct();
		}

		deallocate(external.buffer);

		if(internal.buffer != external.buffer)
		{
			deallocate(internal.buffer);
		}

		deallocate(stencil.buffer);

		external.buffer = 0;
		internal.buffer = 0;
		stencil.buffer = 0;
	}

	void *Surface::lockExternal(int x, int y, int z, Lock lock, Accessor client)
	{
		resource->lock(client);

		if(!external.buffer)
		{
			if(internal.buffer && identicalFormats())
			{
				external.buffer = internal.buffer;
			}
			else
			{
				external.buffer = allocateBuffer(external.width, external.height, external.depth, external.format);
			}
		}

		if(internal.dirty)
		{
			if(lock != LOCK_DISCARD)
			{
				update(external, internal);
			}

			internal.dirty = false;
		}

		switch(lock)
		{
		case LOCK_READONLY:
			break;
		case LOCK_WRITEONLY:
		case LOCK_READWRITE:
		case LOCK_DISCARD:
			dirtyMipmaps = true;
			break;
		default:
			ASSERT(false);
		}

		return external.lockRect(x, y, z, lock);
	}

	void Surface::unlockExternal()
	{
		resource->unlock();

		external.unlockRect();
	}

	void *Surface::lockInternal(int x, int y, int z, Lock lock, Accessor client)
	{
		if(lock != LOCK_UNLOCKED)
		{
			resource->lock(client);
		}

		if(!internal.buffer)
		{
			if(external.buffer && identicalFormats())
			{
				internal.buffer = external.buffer;
			}
			else
			{
				internal.buffer = allocateBuffer(internal.width, internal.height, internal.depth, internal.format);
			}
		}

		// FIXME: WHQL requires conversion to lower external precision and back
		if(logPrecision >= WHQL)
		{
			if(internal.dirty && renderTarget && internal.format != external.format)
			{
				if(lock != LOCK_DISCARD)
				{
					switch(external.format)
					{
					case FORMAT_R3G3B2:
					case FORMAT_A8R3G3B2:
					case FORMAT_A1R5G5B5:
					case FORMAT_A2R10G10B10:
					case FORMAT_A2B10G10R10:
						lockExternal(0, 0, 0, LOCK_READWRITE, client);
						unlockExternal();
						break;
					default:
						// Difference passes WHQL
						break;
					}
				}
			}
		}

		if(external.dirty || (isPalette(external.format) && paletteUsed != Surface::paletteID))
		{
			if(lock != LOCK_DISCARD)
			{
				update(internal, external);
			}

			external.dirty = false;
			paletteUsed = Surface::paletteID;
		}

		switch(lock)
		{
		case LOCK_UNLOCKED:
		case LOCK_READONLY:
			break;
		case LOCK_WRITEONLY:
		case LOCK_READWRITE:
		case LOCK_DISCARD:
			dirtyMipmaps = true;
			break;
		default:
			ASSERT(false);
		}

		if(lock == LOCK_READONLY && client == PUBLIC)
		{
			resolve();
		}

		return internal.lockRect(x, y, z, lock);
	}

	void Surface::unlockInternal()
	{
		resource->unlock();

		internal.unlockRect();
	}

	void *Surface::lockStencil(int front, Accessor client)
	{
		resource->lock(client);

		if(!stencil.buffer)
		{
			stencil.buffer = allocateBuffer(stencil.width, stencil.height, stencil.depth, stencil.format);
		}

		return stencil.lockRect(0, 0, front, LOCK_READWRITE);   // FIXME
	}

	void Surface::unlockStencil()
	{
		resource->unlock();

		stencil.unlockRect();
	}

	int Surface::bytes(Format format)
	{
		switch(format)
		{
		case FORMAT_NULL:				return 0;
		case FORMAT_P8:					return 1;
		case FORMAT_A8P8:				return 2;
		case FORMAT_A8:					return 1;
		case FORMAT_R8:					return 1;
		case FORMAT_R3G3B2:				return 1;
		case FORMAT_A8R3G3B2:			return 2;
		case FORMAT_R5G6B5:				return 2;
		case FORMAT_A1R5G5B5:			return 2;
		case FORMAT_X1R5G5B5:			return 2;
		case FORMAT_X4R4G4B4:			return 2;
		case FORMAT_A4R4G4B4:			return 2;
		case FORMAT_R8G8B8:				return 3;
		case FORMAT_X8R8G8B8:			return 4;
	//	case FORMAT_X8G8R8B8Q:			return 4;
		case FORMAT_A8R8G8B8:			return 4;
	//	case FORMAT_A8G8R8B8Q:			return 4;
		case FORMAT_X8B8G8R8:			return 4;
		case FORMAT_A8B8G8R8:			return 4;
		case FORMAT_A2R10G10B10:		return 4;
		case FORMAT_A2B10G10R10:		return 4;
		case FORMAT_G8R8:				return 2;
		case FORMAT_G16R16:				return 4;
		case FORMAT_A16B16G16R16:		return 8;
		// Compressed formats
		#if S3TC_SUPPORT
		case FORMAT_DXT1:				return 2;   // Column of four pixels
		case FORMAT_DXT3:				return 4;   // Column of four pixels
		case FORMAT_DXT5:				return 4;   // Column of four pixels
		#endif
		case FORMAT_ATI1:				return 2;   // Column of four pixels
		case FORMAT_ATI2:				return 4;   // Column of four pixels
		case FORMAT_ETC1:				return 2;   // Column of four pixels
		// Bumpmap formats
		case FORMAT_V8U8:				return 2;
		case FORMAT_L6V5U5:				return 2;
		case FORMAT_Q8W8V8U8:			return 4;
		case FORMAT_X8L8V8U8:			return 4;
		case FORMAT_A2W10V10U10:		return 4;
		case FORMAT_V16U16:				return 4;
		case FORMAT_A16W16V16U16:		return 8;
		case FORMAT_Q16W16V16U16:		return 8;
		// Luminance formats
		case FORMAT_L8:					return 1;
		case FORMAT_A4L4:				return 1;
		case FORMAT_L16:				return 2;
		case FORMAT_A8L8:				return 2;
		// Floating-point formats
		case FORMAT_R16F:				return 2;
		case FORMAT_G16R16F:			return 4;
		case FORMAT_A16B16G16R16F:		return 8;
		case FORMAT_R32F:				return 4;
		case FORMAT_G32R32F:			return 8;
		case FORMAT_A32B32G32R32F:		return 16;
		// Depth/stencil formats
		case FORMAT_D16:				return 2;
		case FORMAT_D32:				return 4;
		case FORMAT_D24X8:				return 4;
		case FORMAT_D24S8:				return 4;
		case FORMAT_D24FS8:				return 4;
		case FORMAT_D32F:				return 4;
		case FORMAT_D32F_COMPLEMENTARY:	return 4;
		case FORMAT_D32F_LOCKABLE:		return 4;
		case FORMAT_D32FS8_TEXTURE:		return 4;
		case FORMAT_D32FS8_SHADOW:		return 4;
		case FORMAT_DF24S8:				return 4;
		case FORMAT_DF16S8:				return 2;
		case FORMAT_INTZ:				return 4;
		case FORMAT_S8:					return 1;
		default:
			ASSERT(false);
		}

		return 0;
	}

	int Surface::pitchB(int width, Format format, bool target)
	{
		if(target || isDepth(format) || isStencil(format))
		{
			width = ((width + 1) & ~1);
		}

		switch(format)
		{
		#if S3TC_SUPPORT
		case FORMAT_DXT1:
		#endif
		case FORMAT_ETC1:
			return 8 * ((width + 3) / 4);    // 64 bit per 4x4 block, computed per 4 rows
		#if S3TC_SUPPORT
		case FORMAT_DXT3:
		case FORMAT_DXT5:
			return 16 * ((width + 3) / 4);   // 128 bit per 4x4 block, computed per 4 rows
		#endif
		case FORMAT_ATI1:
			return 2 * ((width + 3) / 4);    // 64 bit per 4x4 block, computed per row
		case FORMAT_ATI2:
			return 4 * ((width + 3) / 4);    // 128 bit per 4x4 block, computed per row
		default:
			return bytes(format) * width;
		}
	}

	int Surface::pitchP(int width, Format format, bool target)
	{
		int B = bytes(format);

		return B > 0 ? pitchB(width, format, target) / B : 0;
	}

	int Surface::sliceB(int width, int height, Format format, bool target)
	{
		if(target || isDepth(format) || isStencil(format))
		{
			height = ((height + 1) & ~1);
		}

		switch(format)
		{
		#if S3TC_SUPPORT
		case FORMAT_DXT1:
		case FORMAT_DXT3:
		case FORMAT_DXT5:
		#endif
		case FORMAT_ETC1:
			return pitchB(width, format, target) * ((height + 3) / 4);   // Pitch computed per 4 rows
		case FORMAT_ATI1:
		case FORMAT_ATI2:
		default:
			return pitchB(width, format, target) * height;   // Pitch computed per row
		}
	}

	int Surface::sliceP(int width, int height, Format format, bool target)
	{
		int B = bytes(format);

		return B > 0 ? sliceB(width, height, format, target) / B : 0;
	}

	void Surface::update(Buffer &destination, Buffer &source)
	{
	//	ASSERT(source.lock != LOCK_UNLOCKED);
	//	ASSERT(destination.lock != LOCK_UNLOCKED);
		
		if(destination.buffer != source.buffer)
		{
			ASSERT(source.dirty && !destination.dirty);

			switch(source.format)
			{
			case FORMAT_R8G8B8:		decodeR8G8B8(destination, source);		break;   // FIXME: Check destination format
			case FORMAT_X8B8G8R8:	decodeX8B8G8R8(destination, source);	break;   // FIXME: Check destination format
			case FORMAT_A8B8G8R8:	decodeA8B8G8R8(destination, source);	break;   // FIXME: Check destination format
			case FORMAT_R5G6B5:		decodeR5G6B5(destination, source);		break;   // FIXME: Check destination format
			case FORMAT_X1R5G5B5:	decodeX1R5G5B5(destination, source);	break;   // FIXME: Check destination format
			case FORMAT_A1R5G5B5:	decodeA1R5G5B5(destination, source);	break;   // FIXME: Check destination format
			case FORMAT_X4R4G4B4:	decodeX4R4G4B4(destination, source);	break;   // FIXME: Check destination format
			case FORMAT_A4R4G4B4:	decodeA4R4G4B4(destination, source);	break;   // FIXME: Check destination format
			case FORMAT_P8:			decodeP8(destination, source);			break;   // FIXME: Check destination format
			#if S3TC_SUPPORT
			case FORMAT_DXT1:		decodeDXT1(destination, source);		break;   // FIXME: Check destination format
			case FORMAT_DXT3:		decodeDXT3(destination, source);		break;   // FIXME: Check destination format
			case FORMAT_DXT5:		decodeDXT5(destination, source);		break;   // FIXME: Check destination format
			#endif
			case FORMAT_ATI1:		decodeATI1(destination, source);		break;   // FIXME: Check destination format
			case FORMAT_ATI2:		decodeATI2(destination, source);		break;   // FIXME: Check destination format
			case FORMAT_ETC1:		decodeETC1(destination, source);		break;   // FIXME: Check destination format
			default:				genericUpdate(destination, source);		break;
			}
		}
	}

	void Surface::genericUpdate(Buffer &destination, Buffer &source)
	{
		unsigned char *sourceSlice = (unsigned char*)source.buffer;
		unsigned char *destinationSlice = (unsigned char*)destination.buffer;

		int depth = min(destination.depth, source.depth);
		int height = min(destination.height, source.height);
		int width = min(destination.width, source.width);
		int rowBytes = width * source.bytes;

		for(int z = 0; z < depth; z++)
		{
			unsigned char *sourceRow = sourceSlice;
			unsigned char *destinationRow = destinationSlice;

			for(int y = 0; y < height; y++)
			{
				if(source.format == destination.format)
				{
					memcpy(destinationRow, sourceRow, rowBytes);
				}
				else
				{
					unsigned char *sourceElement = sourceRow;
					unsigned char *destinationElement = destinationRow;

					for(int x = 0; x < width; x++)
					{
						Color<float> color = source.read(sourceElement);
						destination.write(destinationElement, color);

						sourceElement += source.bytes;
						destinationElement += destination.bytes;
					}
				}

				sourceRow += source.pitchB;
				destinationRow += destination.pitchB;
			}

			sourceSlice += source.sliceB;
			destinationSlice += destination.sliceB;
		}
	}

	void Surface::decodeR8G8B8(Buffer &destination, const Buffer &source)
	{
		unsigned char *sourceSlice = (unsigned char*)source.buffer;
		unsigned char *destinationSlice = (unsigned char*)destination.buffer;

		for(int z = 0; z < destination.depth && z < source.depth; z++)
		{
			unsigned char *sourceRow = sourceSlice;
			unsigned char *destinationRow = destinationSlice;

			for(int y = 0; y < destination.height && y < source.height; y++)
			{
				unsigned char *sourceElement = sourceRow;
				unsigned char *destinationElement = destinationRow;

				for(int x = 0; x < destination.width && x < source.width; x++)
				{
					unsigned int b = sourceElement[0];
					unsigned int g = sourceElement[1];
					unsigned int r = sourceElement[2];

					*(unsigned int*)destinationElement = 0xFF000000 | (r << 16) | (g << 8) | (b << 0);

					sourceElement += source.bytes;
					destinationElement += destination.bytes;
				}

				sourceRow += source.pitchB;
				destinationRow += destination.pitchB;
			}

			sourceSlice += source.sliceB;
			destinationSlice += destination.sliceB;
		}
	}

	void Surface::decodeX8B8G8R8(Buffer &destination, const Buffer &source)
	{
		unsigned char *sourceSlice = (unsigned char*)source.buffer;
		unsigned char *destinationSlice = (unsigned char*)destination.buffer;

		for(int z = 0; z < destination.depth && z < source.depth; z++)
		{
			unsigned char *sourceRow = sourceSlice;
			unsigned char *destinationRow = destinationSlice;

			for(int y = 0; y < destination.height && y < source.height; y++)
			{
				unsigned char *sourceElement = sourceRow;
				unsigned char *destinationElement = destinationRow;

				for(int x = 0; x < destination.width && x < source.width; x++)
				{
					unsigned int r = sourceElement[0];
					unsigned int g = sourceElement[1];
					unsigned int b = sourceElement[2];

					*(unsigned int*)destinationElement = 0xFF000000 | (r << 16) | (g << 8) | (b << 0);

					sourceElement += source.bytes;
					destinationElement += destination.bytes;
				}

				sourceRow += source.pitchB;
				destinationRow += destination.pitchB;
			}

			sourceSlice += source.sliceB;
			destinationSlice += destination.sliceB;
		}
	}

	void Surface::decodeA8B8G8R8(Buffer &destination, const Buffer &source)
	{
		unsigned char *sourceSlice = (unsigned char*)source.buffer;
		unsigned char *destinationSlice = (unsigned char*)destination.buffer;

		for(int z = 0; z < destination.depth && z < source.depth; z++)
		{
			unsigned char *sourceRow = sourceSlice;
			unsigned char *destinationRow = destinationSlice;

			for(int y = 0; y < destination.height && y < source.height; y++)
			{
				unsigned char *sourceElement = sourceRow;
				unsigned char *destinationElement = destinationRow;

				for(int x = 0; x < destination.width && x < source.width; x++)
				{
					unsigned int r = sourceElement[0];
					unsigned int g = sourceElement[1];
					unsigned int b = sourceElement[2];
					unsigned int a = sourceElement[3];

					*(unsigned int*)destinationElement = (a << 24) | (r << 16) | (g << 8) | (b << 0);

					sourceElement += source.bytes;
					destinationElement += destination.bytes;
				}

				sourceRow += source.pitchB;
				destinationRow += destination.pitchB;
			}

			sourceSlice += source.sliceB;
			destinationSlice += destination.sliceB;
		}
	}

	void Surface::decodeR5G6B5(Buffer &destination, const Buffer &source)
	{
		unsigned char *sourceSlice = (unsigned char*)source.buffer;
		unsigned char *destinationSlice = (unsigned char*)destination.buffer;

		for(int z = 0; z < destination.depth && z < source.depth; z++)
		{
			unsigned char *sourceRow = sourceSlice;
			unsigned char *destinationRow = destinationSlice;

			for(int y = 0; y < destination.height && y < source.height; y++)
			{
				unsigned char *sourceElement = sourceRow;
				unsigned char *destinationElement = destinationRow;

				for(int x = 0; x < destination.width && x < source.width; x++)
				{
					unsigned int rgb = *(unsigned short*)sourceElement;
						
					unsigned int r = (((rgb & 0xF800) * 67385 + 0x800000) >> 8) & 0x00FF0000;
					unsigned int g = (((rgb & 0x07E0) * 8289  + 0x8000) >> 8) & 0x0000FF00;
					unsigned int b = (((rgb & 0x001F) * 2106  + 0x80) >> 8);

					*(unsigned int*)destinationElement = 0xFF000000 | r | g | b;

					sourceElement += source.bytes;
					destinationElement += destination.bytes;
				}

				sourceRow += source.pitchB;
				destinationRow += destination.pitchB;
			}

			sourceSlice += source.sliceB;
			destinationSlice += destination.sliceB;
		}
	}

	void Surface::decodeX1R5G5B5(Buffer &destination, const Buffer &source)
	{
		unsigned char *sourceSlice = (unsigned char*)source.buffer;
		unsigned char *destinationSlice = (unsigned char*)destination.buffer;

		for(int z = 0; z < destination.depth && z < source.depth; z++)
		{
			unsigned char *sourceRow = sourceSlice;
			unsigned char *destinationRow = destinationSlice;

			for(int y = 0; y < destination.height && y < source.height; y++)
			{
				unsigned char *sourceElement = sourceRow;
				unsigned char *destinationElement = destinationRow;

				for(int x = 0; x < destination.width && x < source.width; x++)
				{
					unsigned int xrgb = *(unsigned short*)sourceElement;
						
					unsigned int r = (((xrgb & 0x7C00) * 134771 + 0x800000) >> 8) & 0x00FF0000;
					unsigned int g = (((xrgb & 0x03E0) * 16846 + 0x8000) >> 8) & 0x0000FF00;
					unsigned int b = (((xrgb & 0x001F) * 2106  + 0x80) >> 8);

					*(unsigned int*)destinationElement = 0xFF000000 | r | g | b;

					sourceElement += source.bytes;
					destinationElement += destination.bytes;
				}

				sourceRow += source.pitchB;
				destinationRow += destination.pitchB;
			}

			sourceSlice += source.sliceB;
			destinationSlice += destination.sliceB;
		}
	}

	void Surface::decodeA1R5G5B5(Buffer &destination, const Buffer &source)
	{
		unsigned char *sourceSlice = (unsigned char*)source.buffer;
		unsigned char *destinationSlice = (unsigned char*)destination.buffer;

		for(int z = 0; z < destination.depth && z < source.depth; z++)
		{
			unsigned char *sourceRow = sourceSlice;
			unsigned char *destinationRow = destinationSlice;

			for(int y = 0; y < destination.height && y < source.height; y++)
			{
				unsigned char *sourceElement = sourceRow;
				unsigned char *destinationElement = destinationRow;

				for(int x = 0; x < destination.width && x < source.width; x++)
				{
					unsigned int argb = *(unsigned short*)sourceElement;
					
					unsigned int a =   (argb & 0x8000) * 130560;
					unsigned int r = (((argb & 0x7C00) * 134771 + 0x800000) >> 8) & 0x00FF0000;
					unsigned int g = (((argb & 0x03E0) * 16846  + 0x8000) >> 8) & 0x0000FF00;
					unsigned int b = (((argb & 0x001F) * 2106   + 0x80) >> 8);

					*(unsigned int*)destinationElement = a | r | g | b;

					sourceElement += source.bytes;
					destinationElement += destination.bytes;
				}

				sourceRow += source.pitchB;
				destinationRow += destination.pitchB;
			}

			sourceSlice += source.sliceB;
			destinationSlice += destination.sliceB;
		}
	}

	void Surface::decodeX4R4G4B4(Buffer &destination, const Buffer &source)
	{
		unsigned char *sourceSlice = (unsigned char*)source.buffer;
		unsigned char *destinationSlice = (unsigned char*)destination.buffer;

		for(int z = 0; z < destination.depth && z < source.depth; z++)
		{
			unsigned char *sourceRow = sourceSlice;
			unsigned char *destinationRow = destinationSlice;

			for(int y = 0; y < destination.height && y < source.height; y++)
			{
				unsigned char *sourceElement = sourceRow;
				unsigned char *destinationElement = destinationRow;

				for(int x = 0; x < destination.width && x < source.width; x++)
				{
					unsigned int xrgb = *(unsigned short*)sourceElement;
						
					unsigned int r = ((xrgb & 0x0F00) * 0x00001100) & 0x00FF0000;
					unsigned int g = ((xrgb & 0x00F0) * 0x00000110) & 0x0000FF00;
					unsigned int b =  (xrgb & 0x000F) * 0x00000011;

					*(unsigned int*)destinationElement = 0xFF000000 | r | g | b;

					sourceElement += source.bytes;
					destinationElement += destination.bytes;
				}

				sourceRow += source.pitchB;
				destinationRow += destination.pitchB;
			}

			sourceSlice += source.sliceB;
			destinationSlice += destination.sliceB;
		}
	}

	void Surface::decodeA4R4G4B4(Buffer &destination, const Buffer &source)
	{
		unsigned char *sourceSlice = (unsigned char*)source.buffer;
		unsigned char *destinationSlice = (unsigned char*)destination.buffer;

		for(int z = 0; z < destination.depth && z < source.depth; z++)
		{
			unsigned char *sourceRow = sourceSlice;
			unsigned char *destinationRow = destinationSlice;

			for(int y = 0; y < destination.height && y < source.height; y++)
			{
				unsigned char *sourceElement = sourceRow;
				unsigned char *destinationElement = destinationRow;

				for(int x = 0; x < destination.width && x < source.width; x++)
				{
					unsigned int argb = *(unsigned short*)sourceElement;
					
					unsigned int a = ((argb & 0xF000) * 0x00011000) & 0xFF000000;
					unsigned int r = ((argb & 0x0F00) * 0x00001100) & 0x00FF0000;
					unsigned int g = ((argb & 0x00F0) * 0x00000110) & 0x0000FF00;
					unsigned int b =  (argb & 0x000F) * 0x00000011;

					*(unsigned int*)destinationElement = a | r | g | b;

					sourceElement += source.bytes;
					destinationElement += destination.bytes;
				}

				sourceRow += source.pitchB;
				destinationRow += destination.pitchB;
			}

			sourceSlice += source.sliceB;
			destinationSlice += destination.sliceB;
		}
	}

	void Surface::decodeP8(Buffer &destination, const Buffer &source)
	{
		unsigned char *sourceSlice = (unsigned char*)source.buffer;
		unsigned char *destinationSlice = (unsigned char*)destination.buffer;

		for(int z = 0; z < destination.depth && z < source.depth; z++)
		{
			unsigned char *sourceRow = sourceSlice;
			unsigned char *destinationRow = destinationSlice;

			for(int y = 0; y < destination.height && y < source.height; y++)
			{
				unsigned char *sourceElement = sourceRow;
				unsigned char *destinationElement = destinationRow;

				for(int x = 0; x < destination.width && x < source.width; x++)
				{
					unsigned int abgr = palette[*(unsigned char*)sourceElement];

					unsigned int r = (abgr & 0x000000FF) << 16;
					unsigned int g = (abgr & 0x0000FF00) << 0;
					unsigned int b = (abgr & 0x00FF0000) >> 16;
					unsigned int a = (abgr & 0xFF000000) >> 0;

					*(unsigned int*)destinationElement = a | r | g | b;

					sourceElement += source.bytes;
					destinationElement += destination.bytes;
				}

				sourceRow += source.pitchB;
				destinationRow += destination.pitchB;
			}

			sourceSlice += source.sliceB;
			destinationSlice += destination.sliceB;
		}
	}

#if S3TC_SUPPORT
	void Surface::decodeDXT1(Buffer &internal, const Buffer &external)
	{
		unsigned int *destSlice = (unsigned int*)internal.buffer;
		const DXT1 *source = (const DXT1*)external.buffer;

		for(int z = 0; z < external.depth; z++)
		{
			unsigned int *dest = destSlice;

			for(int y = 0; y < external.height; y += 4)
			{
				for(int x = 0; x < external.width; x += 4)
				{
					Color<byte> c[4];

					c[0] = source->c0;
					c[1] = source->c1;

					if(source->c0 > source->c1)   // No transparency
					{
						// c2 = 2 / 3 * c0 + 1 / 3 * c1
						c[2].r = (byte)((2 * (word)c[0].r + (word)c[1].r + 1) / 3);
						c[2].g = (byte)((2 * (word)c[0].g + (word)c[1].g + 1) / 3);
						c[2].b = (byte)((2 * (word)c[0].b + (word)c[1].b + 1) / 3);
						c[2].a = 0xFF;

						// c3 = 1 / 3 * c0 + 2 / 3 * c1
						c[3].r = (byte)(((word)c[0].r + 2 * (word)c[1].r + 1) / 3);
						c[3].g = (byte)(((word)c[0].g + 2 * (word)c[1].g + 1) / 3);
						c[3].b = (byte)(((word)c[0].b + 2 * (word)c[1].b + 1) / 3);
						c[3].a = 0xFF;
					}
					else   // c3 transparent
					{
						// c2 = 1 / 2 * c0 + 1 / 2 * c1
						c[2].r = (byte)(((word)c[0].r + (word)c[1].r) / 2);
						c[2].g = (byte)(((word)c[0].g + (word)c[1].g) / 2);
						c[2].b = (byte)(((word)c[0].b + (word)c[1].b) / 2);
						c[2].a = 0xFF;

						c[3].r = 0;
						c[3].g = 0;
						c[3].b = 0;
						c[3].a = 0;
					}

					for(int j = 0; j < 4 && (y + j) < internal.height; j++)
					{
						for(int i = 0; i < 4 && (x + i) < internal.width; i++)
						{
							dest[(x + i) + (y + j) * internal.width] = c[(unsigned int)(source->lut >> 2 * (i + j * 4)) % 4];
						}
					}

					source++;
				}
			}

			(byte*&)destSlice += internal.sliceB;
		}
	}

	void Surface::decodeDXT3(Buffer &internal, const Buffer &external)
	{
		unsigned int *destSlice = (unsigned int*)internal.buffer;
		const DXT3 *source = (const DXT3*)external.buffer;

		for(int z = 0; z < external.depth; z++)
		{
			unsigned int *dest = destSlice;

			for(int y = 0; y < external.height; y += 4)
			{
				for(int x = 0; x < external.width; x += 4)
				{
					Color<byte> c[4];

					c[0] = source->c0;
					c[1] = source->c1;

					// c2 = 2 / 3 * c0 + 1 / 3 * c1
					c[2].r = (byte)((2 * (word)c[0].r + (word)c[1].r + 1) / 3);
					c[2].g = (byte)((2 * (word)c[0].g + (word)c[1].g + 1) / 3);
					c[2].b = (byte)((2 * (word)c[0].b + (word)c[1].b + 1) / 3);

					// c3 = 1 / 3 * c0 + 2 / 3 * c1
					c[3].r = (byte)(((word)c[0].r + 2 * (word)c[1].r + 1) / 3);
					c[3].g = (byte)(((word)c[0].g + 2 * (word)c[1].g + 1) / 3);
					c[3].b = (byte)(((word)c[0].b + 2 * (word)c[1].b + 1) / 3);

					for(int j = 0; j < 4 && (y + j) < internal.height; j++)
					{
						for(int i = 0; i < 4 && (x + i) < internal.width; i++)
						{
							unsigned int a = (unsigned int)(source->a >> 4 * (i + j * 4)) & 0x0F;
							unsigned int color = (c[(unsigned int)(source->lut >> 2 * (i + j * 4)) % 4] & 0x00FFFFFF) | ((a << 28) + (a << 24));

							dest[(x + i) + (y + j) * internal.width] = color;
						}
					}

					source++;
				}
			}

			(byte*&)destSlice += internal.sliceB;
		}
	}

	void Surface::decodeDXT5(Buffer &internal, const Buffer &external)
	{
		unsigned int *destSlice = (unsigned int*)internal.buffer;
		const DXT5 *source = (const DXT5*)external.buffer;

		for(int z = 0; z < external.depth; z++)
		{
			unsigned int *dest = destSlice;

			for(int y = 0; y < external.height; y += 4)
			{
				for(int x = 0; x < external.width; x += 4)
				{
					Color<byte> c[4];

					c[0] = source->c0;
					c[1] = source->c1;

					// c2 = 2 / 3 * c0 + 1 / 3 * c1
					c[2].r = (byte)((2 * (word)c[0].r + (word)c[1].r + 1) / 3);
					c[2].g = (byte)((2 * (word)c[0].g + (word)c[1].g + 1) / 3);
					c[2].b = (byte)((2 * (word)c[0].b + (word)c[1].b + 1) / 3);

					// c3 = 1 / 3 * c0 + 2 / 3 * c1
					c[3].r = (byte)(((word)c[0].r + 2 * (word)c[1].r + 1) / 3);
					c[3].g = (byte)(((word)c[0].g + 2 * (word)c[1].g + 1) / 3);
					c[3].b = (byte)(((word)c[0].b + 2 * (word)c[1].b + 1) / 3);

					byte a[8];

					a[0] = source->a0;
					a[1] = source->a1;
					
					if(a[0] > a[1])
					{
						a[2] = (byte)((6 * (word)a[0] + 1 * (word)a[1] + 3) / 7);
						a[3] = (byte)((5 * (word)a[0] + 2 * (word)a[1] + 3) / 7);
						a[4] = (byte)((4 * (word)a[0] + 3 * (word)a[1] + 3) / 7);
						a[5] = (byte)((3 * (word)a[0] + 4 * (word)a[1] + 3) / 7);
						a[6] = (byte)((2 * (word)a[0] + 5 * (word)a[1] + 3) / 7);
						a[7] = (byte)((1 * (word)a[0] + 6 * (word)a[1] + 3) / 7);
					}
					else
					{
						a[2] = (byte)((4 * (word)a[0] + 1 * (word)a[1] + 2) / 5);
						a[3] = (byte)((3 * (word)a[0] + 2 * (word)a[1] + 2) / 5);
						a[4] = (byte)((2 * (word)a[0] + 3 * (word)a[1] + 2) / 5);
						a[5] = (byte)((1 * (word)a[0] + 4 * (word)a[1] + 2) / 5);
						a[6] = 0;
						a[7] = 0xFF;
					}

					for(int j = 0; j < 4 && (y + j) < internal.height; j++)
					{
						for(int i = 0; i < 4 && (x + i) < internal.width; i++)
						{
							unsigned int alpha = (unsigned int)a[(unsigned int)(source->alut >> (16 + 3 * (i + j * 4))) % 8] << 24;
							unsigned int color = (c[(source->clut >> 2 * (i + j * 4)) % 4] & 0x00FFFFFF) | alpha;
							
							dest[(x + i) + (y + j) * internal.width] = color;
						}
					}

					source++;
				}
			}

			(byte*&)destSlice += internal.sliceB;
		}
	}
#endif

	void Surface::decodeATI1(Buffer &internal, const Buffer &external)
	{
		byte *destSlice = (byte*)internal.buffer;
		const ATI1 *source = (const ATI1*)external.buffer;

		for(int z = 0; z < external.depth; z++)
		{
			byte *dest = destSlice;

			for(int y = 0; y < external.height; y += 4)
			{
				for(int x = 0; x < external.width; x += 4)
				{
					byte r[8];

					r[0] = source->r0;
					r[1] = source->r1;
					
					if(r[0] > r[1])
					{
						r[2] = (byte)((6 * (word)r[0] + 1 * (word)r[1] + 3) / 7);
						r[3] = (byte)((5 * (word)r[0] + 2 * (word)r[1] + 3) / 7);
						r[4] = (byte)((4 * (word)r[0] + 3 * (word)r[1] + 3) / 7);
						r[5] = (byte)((3 * (word)r[0] + 4 * (word)r[1] + 3) / 7);
						r[6] = (byte)((2 * (word)r[0] + 5 * (word)r[1] + 3) / 7);
						r[7] = (byte)((1 * (word)r[0] + 6 * (word)r[1] + 3) / 7);
					}
					else
					{
						r[2] = (byte)((4 * (word)r[0] + 1 * (word)r[1] + 2) / 5);
						r[3] = (byte)((3 * (word)r[0] + 2 * (word)r[1] + 2) / 5);
						r[4] = (byte)((2 * (word)r[0] + 3 * (word)r[1] + 2) / 5);
						r[5] = (byte)((1 * (word)r[0] + 4 * (word)r[1] + 2) / 5);
						r[6] = 0;
						r[7] = 0xFF;
					}

					for(int j = 0; j < 4 && (y + j) < internal.height; j++)
					{
						for(int i = 0; i < 4 && (x + i) < internal.width; i++)
						{
							dest[(x + i) + (y + j) * internal.width] = r[(unsigned int)(source->rlut >> (16 + 3 * (i + j * 4))) % 8];
						}
					}

					source++;
				}
			}

			destSlice += internal.sliceB;
		}
	}

	void Surface::decodeATI2(Buffer &internal, const Buffer &external)
	{
		word *destSlice = (word*)internal.buffer;
		const ATI2 *source = (const ATI2*)external.buffer;

		for(int z = 0; z < external.depth; z++)
		{
			word *dest = destSlice;

			for(int y = 0; y < external.height; y += 4)
			{
				for(int x = 0; x < external.width; x += 4)
				{
					byte X[8];

					X[0] = source->x0;
					X[1] = source->x1;
					
					if(X[0] > X[1])
					{
						X[2] = (byte)((6 * (word)X[0] + 1 * (word)X[1] + 3) / 7);
						X[3] = (byte)((5 * (word)X[0] + 2 * (word)X[1] + 3) / 7);
						X[4] = (byte)((4 * (word)X[0] + 3 * (word)X[1] + 3) / 7);
						X[5] = (byte)((3 * (word)X[0] + 4 * (word)X[1] + 3) / 7);
						X[6] = (byte)((2 * (word)X[0] + 5 * (word)X[1] + 3) / 7);
						X[7] = (byte)((1 * (word)X[0] + 6 * (word)X[1] + 3) / 7);
					}
					else
					{
						X[2] = (byte)((4 * (word)X[0] + 1 * (word)X[1] + 2) / 5);
						X[3] = (byte)((3 * (word)X[0] + 2 * (word)X[1] + 2) / 5);
						X[4] = (byte)((2 * (word)X[0] + 3 * (word)X[1] + 2) / 5);
						X[5] = (byte)((1 * (word)X[0] + 4 * (word)X[1] + 2) / 5);
						X[6] = 0;
						X[7] = 0xFF;
					}

					byte Y[8];

					Y[0] = source->y0;
					Y[1] = source->y1;
					
					if(Y[0] > Y[1])
					{
						Y[2] = (byte)((6 * (word)Y[0] + 1 * (word)Y[1] + 3) / 7);
						Y[3] = (byte)((5 * (word)Y[0] + 2 * (word)Y[1] + 3) / 7);
						Y[4] = (byte)((4 * (word)Y[0] + 3 * (word)Y[1] + 3) / 7);
						Y[5] = (byte)((3 * (word)Y[0] + 4 * (word)Y[1] + 3) / 7);
						Y[6] = (byte)((2 * (word)Y[0] + 5 * (word)Y[1] + 3) / 7);
						Y[7] = (byte)((1 * (word)Y[0] + 6 * (word)Y[1] + 3) / 7);
					}
					else
					{
						Y[2] = (byte)((4 * (word)Y[0] + 1 * (word)Y[1] + 2) / 5);
						Y[3] = (byte)((3 * (word)Y[0] + 2 * (word)Y[1] + 2) / 5);
						Y[4] = (byte)((2 * (word)Y[0] + 3 * (word)Y[1] + 2) / 5);
						Y[5] = (byte)((1 * (word)Y[0] + 4 * (word)Y[1] + 2) / 5);
						Y[6] = 0;
						Y[7] = 0xFF;
					}

					for(int j = 0; j < 4 && (y + j) < internal.height; j++)
					{
						for(int i = 0; i < 4 && (x + i) < internal.width; i++)
						{
							word r = X[(unsigned int)(source->xlut >> (16 + 3 * (i + j * 4))) % 8];
							word g = Y[(unsigned int)(source->ylut >> (16 + 3 * (i + j * 4))) % 8];

							dest[(x + i) + (y + j) * internal.width] = (g << 8) + r;
						}
					}

					source++;
				}
			}

			(byte*&)destSlice += internal.sliceB;
		}
	}

	struct bgrx8
	{
		byte b;
		byte g;
		byte r;
		byte x;

		inline bgrx8()
		{
		}

		inline void set(int red, int green, int blue)
		{
			r = static_cast<byte>(clamp(red, 0, 255));
			g = static_cast<byte>(clamp(green, 0, 255));
			b = static_cast<byte>(clamp(blue, 0, 255));
			x = 255;
		}
	};

	struct ETC1
	{
		struct
		{
			union
			{
				struct   // Individual colors
				{
					byte R2 : 4;
					byte R1 : 4;
					byte G2 : 4;
					byte G1 : 4;
					byte B2 : 4;
					byte B1 : 4;
				};

				struct   // Differential colors
				{
					sbyte dR : 3;
					byte R : 5;
					sbyte dG : 3;
					byte G : 5;
					sbyte dB : 3;
					byte B : 5;
				};
			};

			bool flipbit : 1;
			bool diffbit : 1;
			byte cw2 : 3;
			byte cw1 : 3;

			byte pixelIndexMSB[2];
			byte pixelIndexLSB[2];
		};

		inline int getIndex(int x, int y) const
		{
			int bitIndex = x * 4 + y;
			int bitOffset = bitIndex & 7;
			int lsb = (pixelIndexLSB[1 - (bitIndex >> 3)] >> bitOffset) & 1;
			int msb = (pixelIndexMSB[1 - (bitIndex >> 3)] >> bitOffset) & 1;

			return (msb << 1) | lsb;
		}
	};

	inline int extend_4to8bits(int x)
	{
		return (x << 4) | x;
	}

	inline int extend_5to8bits(int x)
	{
		return (x << 3) | (x >> 2);
	}

	void Surface::decodeETC1(Buffer &internal, const Buffer &external)
	{
		unsigned int *destSlice = (unsigned int*)internal.buffer;
		const ETC1 *source = (const ETC1*)external.buffer;

		for(int z = 0; z < external.depth; z++)
		{
			unsigned int *dest = destSlice;

			for(int y = 0; y < external.height; y += 4)
			{
				for(int x = 0; x < external.width; x += 4)
				{
					bgrx8 *color = reinterpret_cast<bgrx8*>(&dest[x + y * internal.width]);

					int r1, g1, b1;
					int r2, g2, b2;

					if(source->diffbit)
					{
						b1 = extend_5to8bits(source->B);
						g1 = extend_5to8bits(source->G);
						r1 = extend_5to8bits(source->R);

						r2 = extend_5to8bits(source->R + source->dR);
						g2 = extend_5to8bits(source->G + source->dG);
						b2 = extend_5to8bits(source->B + source->dB);
					}
					else
					{
						r1 = extend_4to8bits(source->R1);
						g1 = extend_4to8bits(source->G1);
						b1 = extend_4to8bits(source->B1);

						r2 = extend_4to8bits(source->R2);
						g2 = extend_4to8bits(source->G2);
						b2 = extend_4to8bits(source->B2);
					}

					bgrx8 subblockColors0[4];
					bgrx8 subblockColors1[4];

					// Table 3.17.2 sorted according to table 3.17.3
					static const int intensityModifier[8][4] =
					{
						{2, 8, -2, -8},
						{5, 17, -5, -17},
						{9, 29, -9, -29},
						{13, 42, -13, -42},
						{18, 60, -18, -60},
						{24, 80, -24, -80},
						{33, 106, -33, -106},
						{47, 183, -47, -183}
					};

					const int i10 = intensityModifier[source->cw1][0];
					const int i11 = intensityModifier[source->cw1][1];
					const int i12 = intensityModifier[source->cw1][2];
					const int i13 = intensityModifier[source->cw1][3];

					subblockColors0[0].set(r1 + i10, g1 + i10, b1 + i10);
					subblockColors0[1].set(r1 + i11, g1 + i11, b1 + i11);
					subblockColors0[2].set(r1 + i12, g1 + i12, b1 + i12);
					subblockColors0[3].set(r1 + i13, g1 + i13, b1 + i13);

					const int i20 = intensityModifier[source->cw2][0];
					const int i21 = intensityModifier[source->cw2][1];
					const int i22 = intensityModifier[source->cw2][2];
					const int i23 = intensityModifier[source->cw2][3];

					subblockColors1[0].set(r2 + i20, g2 + i20, b2 + i20);
					subblockColors1[1].set(r2 + i21, g2 + i21, b2 + i21);
					subblockColors1[2].set(r2 + i22, g2 + i22, b2 + i22);
					subblockColors1[3].set(r2 + i23, g2 + i23, b2 + i23);

					if(source->flipbit)
					{
						for(int y = 0; y < 2; y++)
						{
							color[0] = subblockColors0[source->getIndex(0, y)];
							color[1] = subblockColors0[source->getIndex(1, y)];
							color[2] = subblockColors0[source->getIndex(2, y)];
							color[3] = subblockColors0[source->getIndex(3, y)];
							color += internal.width;
						}

						for(int y = 2; y < 4; y++)
						{
							color[0] = subblockColors1[source->getIndex(0, y)];
							color[1] = subblockColors1[source->getIndex(1, y)];
							color[2] = subblockColors1[source->getIndex(2, y)];
							color[3] = subblockColors1[source->getIndex(3, y)];
							color += internal.width;
						}
					}
					else
					{
						for(int y = 0; y < 4; y++)
						{
							color[0] = subblockColors0[source->getIndex(0, y)];
							color[1] = subblockColors0[source->getIndex(1, y)];
							color[2] = subblockColors1[source->getIndex(2, y)];
							color[3] = subblockColors1[source->getIndex(3, y)];
							color += internal.width;
						}
					}

					source++;
				}
			}

			(byte*&)destSlice += internal.sliceB;
		}
	}

	unsigned int Surface::size(int width, int height, int depth, Format format)
	{
		// Dimensions rounded up to multiples of 4, used for DXTC formats
		int width4 = (width + 3) & ~3;
		int height4 = (height + 3) & ~3;

		switch(format)
		{
		#if S3TC_SUPPORT
		case FORMAT_DXT1:
		#endif
		case FORMAT_ATI1:
		case FORMAT_ETC1:
			return width4 * height4 * depth / 2;
		#if S3TC_SUPPORT
		case FORMAT_DXT3:
		case FORMAT_DXT5:
		#endif
		case FORMAT_ATI2:
			return width4 * height4 * depth;
		default:
			return bytes(format) * width * height * depth;
		}

		return 0;
	}

	bool Surface::isStencil(Format format)
	{
		switch(format)
		{
		case FORMAT_D32:
		case FORMAT_D16:
		case FORMAT_D24X8:
		case FORMAT_D32F:
		case FORMAT_D32F_COMPLEMENTARY:
		case FORMAT_D32F_LOCKABLE:
			return false;
		case FORMAT_D24S8:
		case FORMAT_D24FS8:
		case FORMAT_S8:
		case FORMAT_DF24S8:
		case FORMAT_DF16S8:
		case FORMAT_D32FS8_TEXTURE:
		case FORMAT_D32FS8_SHADOW:
		case FORMAT_INTZ:
			return true;
		default:
			return false;
		}
	}

	bool Surface::isDepth(Format format)
	{
		switch(format)
		{
		case FORMAT_D32:
		case FORMAT_D16:
		case FORMAT_D24X8:
		case FORMAT_D24S8:
		case FORMAT_D24FS8:
		case FORMAT_D32F:
		case FORMAT_D32F_COMPLEMENTARY:
		case FORMAT_D32F_LOCKABLE:
		case FORMAT_DF24S8:
		case FORMAT_DF16S8:
		case FORMAT_D32FS8_TEXTURE:
		case FORMAT_D32FS8_SHADOW:
		case FORMAT_INTZ:
			return true;
		case FORMAT_S8:
			return false;
		default:
			return false;
		}
	}

	bool Surface::isPalette(Format format)
	{
		switch(format)
		{
		case FORMAT_P8:
		case FORMAT_A8P8:
			return true;
		default:
			return false;
		}
	}

	bool Surface::isFloatFormat(Format format)
	{
		switch(format)
		{
		case FORMAT_X8R8G8B8:
		case FORMAT_A8R8G8B8:
		case FORMAT_G8R8:
		case FORMAT_G16R16:
		case FORMAT_A16B16G16R16:
		case FORMAT_V8U8:
		case FORMAT_Q8W8V8U8:
		case FORMAT_X8L8V8U8:
		case FORMAT_V16U16:
		case FORMAT_A16W16V16U16:
		case FORMAT_Q16W16V16U16:
		case FORMAT_A8:
		case FORMAT_R8:
		case FORMAT_L8:
		case FORMAT_L16:
		case FORMAT_A8L8:
			return false;
		case FORMAT_R32F:
		case FORMAT_G32R32F:
		case FORMAT_A32B32G32R32F:
		case FORMAT_D32F:
		case FORMAT_D32F_COMPLEMENTARY:
		case FORMAT_D32F_LOCKABLE:
		case FORMAT_D32FS8_TEXTURE:
		case FORMAT_D32FS8_SHADOW:
			return true;
		default:
			ASSERT(false);
		}
		
		return false;
	}

	bool Surface::isUnsignedComponent(Format format, int component)
	{
		switch(format)
		{
		case FORMAT_NULL:
		case FORMAT_X8R8G8B8:
		case FORMAT_A8R8G8B8:
		case FORMAT_G8R8:
		case FORMAT_G16R16:
		case FORMAT_A16B16G16R16:
		case FORMAT_D32F:
		case FORMAT_D32F_COMPLEMENTARY:
		case FORMAT_D32F_LOCKABLE:
		case FORMAT_D32FS8_TEXTURE:
		case FORMAT_D32FS8_SHADOW:
		case FORMAT_A8:
		case FORMAT_R8:
		case FORMAT_L8:
		case FORMAT_L16:
		case FORMAT_A8L8:
			return true;
		case FORMAT_V8U8:
		case FORMAT_X8L8V8U8:
		case FORMAT_V16U16:
			if(component < 2)
			{
				return false;
			}
			else
			{
				return true;
			}
		case FORMAT_A16W16V16U16:
			if(component < 3)
			{
				return false;
			}
			else
			{
				return true;
			}
		case FORMAT_Q8W8V8U8:
		case FORMAT_Q16W16V16U16:
			return false;
		case FORMAT_R32F:
			if(component < 1)
			{
				return false;
			}
			else
			{
				return true;
			}
		case FORMAT_G32R32F:
			if(component < 2)
			{
				return false;
			}
			else
			{
				return true;
			}
		case FORMAT_A32B32G32R32F:
			return false;
		default:
			ASSERT(false);
		}
		
		return false;
	}

	bool Surface::isSRGBreadable(Format format)
	{
		// Keep in sync with Capabilities::isSRGBreadable
		switch(format)
		{
		case FORMAT_L8:
		case FORMAT_A8L8:
		case FORMAT_R8G8B8:
		case FORMAT_A8R8G8B8:
		case FORMAT_X8R8G8B8:
		case FORMAT_A8B8G8R8:
		case FORMAT_X8B8G8R8:
		case FORMAT_R5G6B5:
		case FORMAT_X1R5G5B5:
		case FORMAT_A1R5G5B5:
		case FORMAT_A4R4G4B4:
		#if S3TC_SUPPORT
		case FORMAT_DXT1:
		case FORMAT_DXT3:
		case FORMAT_DXT5:
		#endif
		case FORMAT_ATI1:
		case FORMAT_ATI2:
			return true;
		default:
			return false;
		}

		return false;
	}

	bool Surface::isSRGBwritable(Format format)
	{
		// Keep in sync with Capabilities::isSRGBwritable
		switch(format)
		{
		case FORMAT_NULL:
		case FORMAT_A8R8G8B8:
		case FORMAT_X8R8G8B8:
		case FORMAT_A8B8G8R8:
		case FORMAT_X8B8G8R8:
		case FORMAT_R5G6B5:
			return true;
		default:
			return false;
		}
	}

	bool Surface::isCompressed(Format format)
	{
		switch(format)
		{
		#if S3TC_SUPPORT
		case FORMAT_DXT1:
		case FORMAT_DXT3:
		case FORMAT_DXT5:
		#endif
		case FORMAT_ATI1:
		case FORMAT_ATI2:
		case FORMAT_ETC1:
			return true;
		default:
			return false;
		}
	}

	int Surface::componentCount(Format format)
	{
		switch(format)
		{
		case FORMAT_X8R8G8B8:		return 3;
		case FORMAT_A8R8G8B8:		return 4;
		case FORMAT_G8R8:			return 2;
		case FORMAT_G16R16:			return 2;
		case FORMAT_A16B16G16R16:	return 4;
		case FORMAT_V8U8:			return 2;
		case FORMAT_Q8W8V8U8:		return 4;
		case FORMAT_X8L8V8U8:		return 3;
		case FORMAT_V16U16:			return 2;
		case FORMAT_A16W16V16U16:	return 4;
		case FORMAT_Q16W16V16U16:	return 4;
		case FORMAT_R32F:			return 1;
		case FORMAT_G32R32F:		return 2;
		case FORMAT_A32B32G32R32F:	return 4;
		case FORMAT_D32F_LOCKABLE:	return 1;
		case FORMAT_D32FS8_TEXTURE:	return 1;
		case FORMAT_D32FS8_SHADOW:	return 1;
		case FORMAT_A8:				return 1;
		case FORMAT_R8:				return 1;
		case FORMAT_L8:				return 1;
		case FORMAT_L16:			return 1;
		case FORMAT_A8L8:			return 2;
		default:
			ASSERT(false);
		}

		return 1;
	}

	void *Surface::allocateBuffer(int width, int height, int depth, Format format)
	{
		int width4 = (width + 3) & ~3;
		int height4 = (height + 3) & ~3;

		return allocateZero(size(width4, height4, depth, format));
	}

	void Surface::memfill(void *buffer, int pattern, int bytes)
	{
		while((size_t)buffer & 0x1 && bytes >= 1)
		{
			*(char*)buffer = (char)pattern;
			(char*&)buffer += 1;
			bytes -= 1;
		}

		while((size_t)buffer & 0x3 && bytes >= 2)
		{
			*(short*)buffer = (short)pattern;
			(short*&)buffer += 1;
			bytes -= 2;
		}

		if(CPUID::supportsSSE())
		{
			while((size_t)buffer & 0xF && bytes >= 4)
			{
				*(int*)buffer = pattern;
				(int*&)buffer += 1;
				bytes -= 4;
			}

			__m128 quad = _mm_set_ps1((float&)pattern);
			
			float *pointer = (float*)buffer;
			int qxwords = bytes / 64;
			bytes -= qxwords * 64;

			while(qxwords--)
			{
				_mm_stream_ps(pointer + 0, quad);
				_mm_stream_ps(pointer + 4, quad);
				_mm_stream_ps(pointer + 8, quad);
				_mm_stream_ps(pointer + 12, quad);

				pointer += 16;
			}

			buffer = pointer;
		}

		while(bytes >= 4)
		{
			*(int*)buffer = (int)pattern;
			(int*&)buffer += 1;
			bytes -= 4;
		}

		while(bytes >= 2)
		{
			*(short*)buffer = (short)pattern;
			(short*&)buffer += 1;
			bytes -= 2;
		}

		while(bytes >= 1)
		{
			*(char*)buffer = (char)pattern;
			(char*&)buffer += 1;
			bytes -= 1;
		}
	}

	void Surface::clearColorBuffer(unsigned int color, unsigned int rgbaMask, int x0, int y0, int width, int height)
	{
		// FIXME: Also clear buffers in other formats?

		// Not overlapping
		if(x0 > internal.width) return;
		if(y0 > internal.height) return;
		if(x0 + width < 0) return;
		if(y0 + height < 0) return;

		// Clip against dimensions
		if(x0 < 0) {width += x0; x0 = 0;}
		if(x0 + width > internal.width) width = internal.width - x0;
		if(y0 < 0) {height += y0; y0 = 0;}
		if(y0 + height > internal.height) height = internal.height - y0;

		const bool entire = x0 == 0 && y0 == 0 && width == internal.width && height == internal.height;
		const Lock lock = entire ? LOCK_DISCARD : LOCK_WRITEONLY;

		int width2 = (internal.width + 1) & ~1;

		int x1 = x0 + width;
		int y1 = y0 + height;

		int bytes = 4 * (x1 - x0);

	//	if(lockable || !quadLayoutEnabled)
		{
			unsigned char *buffer = (unsigned char*)lockInternal(x0, y0, 0, lock, PUBLIC);

			unsigned char r8 = (color & 0x00FF0000) >> 16;
			unsigned char g8 = (color & 0x0000FF00) >> 8;
			unsigned char b8 = (color & 0x000000FF) >> 0;
			unsigned char a8 = (color & 0xFF000000) >> 24;

			unsigned short r16 = (r8 << 8) + r8;
			unsigned short g16 = (g8 << 8) + g8;
			unsigned short b16 = (b8 << 8) + b8;
			unsigned short a16 = (a8 << 8) + a8;

			float r32f = r8 / 255.0f;
			float g32f = g8 / 255.0f;
			float b32f = b8 / 255.0f;
			float a32f = a8 / 255.0f;
			
			unsigned char g8r8[4] = {r8, g8, r8, g8};
			unsigned short g16r16[2] = {r16, g16};

			for(int z = 0; z < internal.depth; z++)
			{
				unsigned char *target = buffer;

				for(int y = y0; y < y1; y++)
				{
					switch(internal.format)
					{
					case FORMAT_NULL:
						break;
					case FORMAT_X8R8G8B8:
					case FORMAT_A8R8G8B8:
				//	case FORMAT_X8G8R8B8Q:   // FIXME
				//	case FORMAT_A8G8R8B8Q:   // FIXME
						if(rgbaMask == 0xF || (internal.format == FORMAT_X8R8G8B8 && rgbaMask == 0x7))
						{
							memfill(target, color, 4 * (x1 - x0));
						}
						else
						{
							unsigned int bgraMask = (rgbaMask & 0x1 ? 0x00FF0000 : 0) | (rgbaMask & 0x2 ? 0x0000FF00 : 0) | (rgbaMask & 0x4 ? 0x000000FF : 0) | (rgbaMask & 0x8 ? 0xFF000000 : 0);
							unsigned int invMask = ~bgraMask;
							unsigned int maskedColor = color & bgraMask;
							unsigned int *target32 = (unsigned int*)target;

							for(int x = 0; x < width; x++)
							{
								target32[x] = maskedColor | (target32[x] & invMask);
							}
						}
						break;
					case FORMAT_G8R8:
						if((rgbaMask & 0x3) == 0x3)
						{
							memfill(target, (int&)g8r8, 2 * (x1 - x0));
						}
						else
						{
							unsigned short rgMask = (rgbaMask & 0x1 ? 0x000000FF : 0) | (rgbaMask & 0x2 ? 0x0000FF00 : 0);
							unsigned short invMask = ~rgMask;
							unsigned short maskedColor = (unsigned short&)g8r8 & rgMask;
							unsigned short *target16 = (unsigned short*)target;

							for(int x = 0; x < width; x++)
							{
								target16[x] = maskedColor | (target16[x] & invMask);
							}
						}
						break;
					case FORMAT_G16R16:
						if((rgbaMask & 0x3) == 0x3)
						{
							memfill(target, (int&)g16r16, 4 * (x1 - x0));
						}
						else
						{
							unsigned int rgMask = (rgbaMask & 0x1 ? 0x0000FFFF : 0) | (rgbaMask & 0x2 ? 0xFFFF0000 : 0);
							unsigned int invMask = ~rgMask;
							unsigned int maskedColor = (unsigned int&)g16r16 & rgMask;
							unsigned int *target32 = (unsigned int*)target;

							for(int x = 0; x < width; x++)
							{
								target32[x] = maskedColor | (target32[x] & invMask);
							}
						}
						break;
					case FORMAT_A16B16G16R16:
						if(rgbaMask == 0xF)
						{
							for(int x = 0; x < width; x++)
							{
								((unsigned short*)target)[4 * x + 0] = r16;
								((unsigned short*)target)[4 * x + 1] = g16;
								((unsigned short*)target)[4 * x + 2] = b16;
								((unsigned short*)target)[4 * x + 3] = a16;
							}
						}
						else
						{
							if(rgbaMask & 0x1) for(int x = 0; x < width; x++) ((unsigned short*)target)[4 * x + 0] = r16;
							if(rgbaMask & 0x2) for(int x = 0; x < width; x++) ((unsigned short*)target)[4 * x + 1] = g16;
							if(rgbaMask & 0x4) for(int x = 0; x < width; x++) ((unsigned short*)target)[4 * x + 2] = b16;
							if(rgbaMask & 0x8) for(int x = 0; x < width; x++) ((unsigned short*)target)[4 * x + 3] = a16;
						}
						break;
					case FORMAT_R32F:
						if(rgbaMask & 0x1)
						{
							for(int x = 0; x < width; x++)
							{
								((float*)target)[x] = r32f;
							}
						}
						break;
					case FORMAT_G32R32F:
						if((rgbaMask & 0x3) == 0x3)
						{
							for(int x = 0; x < width; x++)
							{
								((float*)target)[2 * x + 0] = r32f;
								((float*)target)[2 * x + 1] = g32f;
							}
						}
						else
						{
							if(rgbaMask & 0x1) for(int x = 0; x < width; x++) ((float*)target)[2 * x + 0] = r32f;
							if(rgbaMask & 0x2) for(int x = 0; x < width; x++) ((float*)target)[2 * x + 1] = g32f;
						}
						break;
					case FORMAT_A32B32G32R32F:
						if(rgbaMask == 0xF)
						{
							for(int x = 0; x < width; x++)
							{
								((float*)target)[4 * x + 0] = r32f;
								((float*)target)[4 * x + 1] = g32f;
								((float*)target)[4 * x + 2] = b32f;
								((float*)target)[4 * x + 3] = a32f;
							}
						}
						else
						{
							if(rgbaMask & 0x1) for(int x = 0; x < width; x++) ((float*)target)[4 * x + 0] = r32f;
							if(rgbaMask & 0x2) for(int x = 0; x < width; x++) ((float*)target)[4 * x + 1] = g32f;
							if(rgbaMask & 0x4) for(int x = 0; x < width; x++) ((float*)target)[4 * x + 2] = b32f;
							if(rgbaMask & 0x8) for(int x = 0; x < width; x++) ((float*)target)[4 * x + 3] = a32f;
						}
						break;
					default:
						ASSERT(false);
					}

					target += internal.pitchB;
				}

				buffer += internal.sliceB;
			}

			unlockInternal();
		}
	/*	else
		{
		//	unsigned char *target = (unsigned char*&)buffer;
		//
		//	for(int y = y0; y < y1; y++)
		//	{
		//		for(int x = x0; x < x1; x++)
		//		{
		//			target[width2 * 4 * (y & ~1) + 2 * (y & 1) + 8 * (x & ~1) + (x & 1) + 0] =  (color & 0x000000FF) >> 0;
		//			target[width2 * 4 * (y & ~1) + 2 * (y & 1) + 8 * (x & ~1) + (x & 1) + 4] =  (color & 0x00FF0000) >> 16;
		//			target[width2 * 4 * (y & ~1) + 2 * (y & 1) + 8 * (x & ~1) + (x & 1) + 8] =  (color & 0x0000FF00) >> 8;
		//			target[width2 * 4 * (y & ~1) + 2 * (y & 1) + 8 * (x & ~1) + (x & 1) + 12] = (color & 0xFF000000) >> 24;
		//		}
		//	}

			unsigned char colorQ[16];

			colorQ[0] =  (color & 0x000000FF) >> 0;
			colorQ[1] =  (color & 0x000000FF) >> 0;
			colorQ[2] =  (color & 0x000000FF) >> 0;
			colorQ[3] =  (color & 0x000000FF) >> 0;
			colorQ[4] =  (color & 0x00FF0000) >> 16;
			colorQ[5] =  (color & 0x00FF0000) >> 16;
			colorQ[6] =  (color & 0x00FF0000) >> 16;
			colorQ[7] =  (color & 0x00FF0000) >> 16;
			colorQ[8] =  (color & 0x0000FF00) >> 8;
			colorQ[9] =  (color & 0x0000FF00) >> 8;
			colorQ[10] = (color & 0x0000FF00) >> 8;
			colorQ[11] = (color & 0x0000FF00) >> 8;
			colorQ[12] = (color & 0xFF000000) >> 24;
			colorQ[13] = (color & 0xFF000000) >> 24;
			colorQ[14] = (color & 0xFF000000) >> 24;
			colorQ[15] = (color & 0xFF000000) >> 24;

			for(int y = y0; y < y1; y++)
			{
				unsigned char *target = (unsigned char*)lockInternal(0, 0, 0, lock) + width2 * 4 * (y & ~1) + 2 * (y & 1);   // FIXME: Unlock

				if((y & 1) == 0 && y + 1 < y1)   // Fill quad line at once
				{
					if((x0 & 1) != 0)
					{
						target[8 * (x0 & ~1) + 1 + 0] =  (color & 0x000000FF) >> 0;
						target[8 * (x0 & ~1) + 1 + 4] =  (color & 0x00FF0000) >> 16;
						target[8 * (x0 & ~1) + 1 + 8] =  (color & 0x0000FF00) >> 8;
						target[8 * (x0 & ~1) + 1 + 12] = (color & 0xFF000000) >> 24;

						target[8 * (x0 & ~1) + 3 + 0] =  (color & 0x000000FF) >> 0;
						target[8 * (x0 & ~1) + 3 + 4] =  (color & 0x00FF0000) >> 16;
						target[8 * (x0 & ~1) + 3 + 8] =  (color & 0x0000FF00) >> 8;
						target[8 * (x0 & ~1) + 3 + 12] = (color & 0xFF000000) >> 24;
					}

					__asm
					{
						movq mm0, colorQ+0
						movq mm1, colorQ+8

						mov eax, x0
						add eax, 1
						and eax, 0xFFFFFFFE
						cmp eax, x1
						jge qEnd

						mov edi, target

					qLoop:
						movntq [edi+8*eax+0], mm0
						movntq [edi+8*eax+8], mm1

						add eax, 2
						cmp eax, x1
						jl qLoop
					qEnd:
						emms
					}

					if((x1 & 1) != 0)
					{
						target[8 * (x1 & ~1) + 0 + 0] =  (color & 0x000000FF) >> 0;
						target[8 * (x1 & ~1) + 0 + 4] =  (color & 0x00FF0000) >> 16;
						target[8 * (x1 & ~1) + 0 + 8] =  (color & 0x0000FF00) >> 8;
						target[8 * (x1 & ~1) + 0 + 12] = (color & 0xFF000000) >> 24;

						target[8 * (x1 & ~1) + 2 + 0] =  (color & 0x000000FF) >> 0;
						target[8 * (x1 & ~1) + 2 + 4] =  (color & 0x00FF0000) >> 16;
						target[8 * (x1 & ~1) + 2 + 8] =  (color & 0x0000FF00) >> 8;
						target[8 * (x1 & ~1) + 2 + 12] = (color & 0xFF000000) >> 24;
					}

					y++;
				}
				else
				{
					for(int x = x0; x < x1; x++)
					{
						target[8 * (x & ~1) + (x & 1) + 0] =  (color & 0x000000FF) >> 0;
						target[8 * (x & ~1) + (x & 1) + 4] =  (color & 0x00FF0000) >> 16;
						target[8 * (x & ~1) + (x & 1) + 8] =  (color & 0x0000FF00) >> 8;
						target[8 * (x & ~1) + (x & 1) + 12] = (color & 0xFF000000) >> 24;
					}
				}
			}
		}*/
	}

	void Surface::clearDepthBuffer(float depth, int x0, int y0, int width, int height)
	{
		// Not overlapping
		if(x0 > internal.width) return;
		if(y0 > internal.height) return;
		if(x0 + width < 0) return;
		if(y0 + height < 0) return;

		// Clip against dimensions
		if(x0 < 0) {width += x0; x0 = 0;}
		if(x0 + width > internal.width) width = internal.width - x0;
		if(y0 < 0) {height += y0; y0 = 0;}
		if(y0 + height > internal.height) height = internal.height - y0;

		const bool entire = x0 == 0 && y0 == 0 && width == internal.width && height == internal.height;
		const Lock lock = entire ? LOCK_DISCARD : LOCK_WRITEONLY;

		int width2 = (internal.width + 1) & ~1;

		int x1 = x0 + width;
		int y1 = y0 + height;

		if(internal.format == FORMAT_D32F_LOCKABLE ||
		   internal.format == FORMAT_D32FS8_TEXTURE ||
		   internal.format == FORMAT_D32FS8_SHADOW)
		{
			float *target = (float*)lockInternal(0, 0, 0, lock, PUBLIC) + x0 + width2 * y0;

			for(int z = 0; z < internal.depth; z++)
			{
				for(int y = y0; y < y1; y++)
				{
					memfill(target, (int&)depth, 4 * width);
					target += width2;
				}
			}

			unlockInternal();
		}
		else   // Quad layout
		{
			if(complementaryDepthBuffer)
			{
				depth = 1 - depth;
			}

			float *buffer = (float*)lockInternal(0, 0, 0, lock, PUBLIC);

			for(int z = 0; z < internal.depth; z++)
			{
				for(int y = y0; y < y1; y++)
				{
					float *target = buffer + (y & ~1) * width2 + (y & 1) * 2;
			
					if((y & 1) == 0 && y + 1 < y1)   // Fill quad line at once
					{
						if((x0 & 1) != 0)
						{
							target[(x0 & ~1) * 2 + 1] = depth;
							target[(x0 & ~1) * 2 + 3] = depth;
						}

					//	for(int x2 = ((x0 + 1) & ~1) * 2; x2 < x1 * 2; x2 += 4)
					//	{
					//		target[x2 + 0] = depth;
					//		target[x2 + 1] = depth;
					//		target[x2 + 2] = depth;
					//		target[x2 + 3] = depth;
					//	}

					//	__asm
					//	{
					//		movss xmm0, depth
					//		shufps xmm0, xmm0, 0x00
					//
					//		mov eax, x0
					//		add eax, 1
					//		and eax, 0xFFFFFFFE
					//		cmp eax, x1
					//		jge qEnd
					//
					//		mov edi, target
					//
					//	qLoop:
					//		movntps [edi+8*eax], xmm0
					//
					//		add eax, 2
					//		cmp eax, x1
					//		jl qLoop
					//	qEnd:
					//	}

						memfill(&target[((x0 + 1) & ~1) * 2], (int&)depth, 8 * ((x1 & ~1) - ((x0 + 1) & ~1)));

						if((x1 & 1) != 0)
						{
							target[(x1 & ~1) * 2 + 0] = depth;
							target[(x1 & ~1) * 2 + 2] = depth;
						}

						y++;
					}
					else
					{
						for(int x = x0; x < x1; x++)
						{
							target[(x & ~1) * 2 + (x & 1)] = depth;
						}
					}
				}

				buffer += internal.sliceP;
			}

			unlockInternal();
		}
	}

	void Surface::clearStencilBuffer(unsigned char s, unsigned char mask, int x0, int y0, int width, int height)
	{
		// Not overlapping
		if(x0 > internal.width) return;
		if(y0 > internal.height) return;
		if(x0 + width < 0) return;
		if(y0 + height < 0) return;

		// Clip against dimensions
		if(x0 < 0) {width += x0; x0 = 0;}
		if(x0 + width > internal.width) width = internal.width - x0;
		if(y0 < 0) {height += y0; y0 = 0;}
		if(y0 + height > internal.height) height = internal.height - y0;

		int width2 = (internal.width + 1) & ~1;

		int x1 = x0 + width;
		int y1 = y0 + height;

		unsigned char maskedS = s & mask;
		unsigned char invMask = ~mask;
		unsigned int fill = maskedS;
		fill = fill | (fill << 8) | (fill << 16) + (fill << 24);

		if(false)
		{
			char *target = (char*)lockStencil(0, PUBLIC) + x0 + width2 * y0;

			for(int z = 0; z < stencil.depth; z++)
			{
				for(int y = y0; y < y0 + height; y++)
				{
					if(mask == 0xFF)
					{
						memfill(target, fill, width);
					}
					else
					{
						for(int x = 0; x < width; x++)
						{
							target[x] = maskedS | (target[x] & invMask);
						}
					}

					target += width2;
				}
			}

			unlockStencil();
		}
		else   // Quad layout
		{
			char *buffer = (char*)lockStencil(0, PUBLIC);

			if(mask == 0xFF)
			{
				for(int z = 0; z < stencil.depth; z++)
				{
					for(int y = y0; y < y1; y++)
					{
						char *target = buffer + (y & ~1) * width2 + (y & 1) * 2;

						if((y & 1) == 0 && y + 1 < y1 && mask == 0xFF)   // Fill quad line at once
						{
							if((x0 & 1) != 0)
							{
								target[(x0 & ~1) * 2 + 1] = fill;
								target[(x0 & ~1) * 2 + 3] = fill;
							}

							memfill(&target[((x0 + 1) & ~1) * 2], fill, ((x1 + 1) & ~1) * 2 - ((x0 + 1) & ~1) * 2);

							if((x1 & 1) != 0)
							{
								target[(x1 & ~1) * 2 + 0] = fill;
								target[(x1 & ~1) * 2 + 2] = fill;
							}

							y++;
						}
						else
						{
							for(int x = x0; x < x1; x++)
							{
								target[(x & ~1) * 2 + (x & 1)] = maskedS | (target[x] & invMask);
							}
						}
					}

					buffer += stencil.sliceP;
				}
			}

			unlockStencil();
		}
	}

	void Surface::fill(const Color<float> &color, int x0, int y0, int width, int height)
	{
		unsigned char *row;
		Buffer *buffer;
		
		if(internal.dirty)
		{
			row = (unsigned char*)lockInternal(x0, y0, 0, LOCK_WRITEONLY, PUBLIC);
			buffer = &internal;
		}
		else
		{
			row = (unsigned char*)lockExternal(x0, y0, 0, LOCK_WRITEONLY, PUBLIC);
			buffer = &external;
		}

		if(buffer->bytes <= 4)
		{
			int c;
			buffer->write(&c, color);

			if(buffer->bytes <= 1) c = (c << 8)  | c;
			if(buffer->bytes <= 2) c = (c << 16) | c;

			for(int y = 0; y < height; y++)
			{
				memfill(row, c, width * buffer->bytes);

				row += buffer->pitchB;
			}
		}
		else   // Generic
		{
			for(int y = 0; y < height; y++)
			{
				unsigned char *element = row;

				for(int x = 0; x < width; x++)
				{
					buffer->write(element, color);

					element += buffer->bytes;
				}

				row += buffer->pitchB;
			}
		}

		if(buffer == &internal)
		{
			unlockInternal();
		}
		else
		{
			unlockExternal();
		}
	}

	Color<float> Surface::readExternal(int x, int y, int z) const
	{
		ASSERT(external.lock != LOCK_UNLOCKED);

		return external.read(x, y, z);
	}

	Color<float> Surface::readExternal(int x, int y) const
	{
		ASSERT(external.lock != LOCK_UNLOCKED);

		return external.read(x, y);
	}

	Color<float> Surface::sampleExternal(float x, float y, float z) const
	{
		ASSERT(external.lock != LOCK_UNLOCKED);

		return external.sample(x, y, z);
	}

	Color<float> Surface::sampleExternal(float x, float y) const
	{
		ASSERT(external.lock != LOCK_UNLOCKED);

		return external.sample(x, y);
	}

	void Surface::writeExternal(int x, int y, int z, const Color<float> &color)
	{
		ASSERT(external.lock != LOCK_UNLOCKED);

		external.write(x, y, z, color);
	}

	void Surface::writeExternal(int x, int y, const Color<float> &color)
	{
		ASSERT(external.lock != LOCK_UNLOCKED);

		external.write(x, y, color);
	}

	Color<float> Surface::readInternal(int x, int y, int z) const
	{
		ASSERT(internal.lock != LOCK_UNLOCKED);

		return internal.read(x, y, z);
	}

	Color<float> Surface::readInternal(int x, int y) const
	{
		ASSERT(internal.lock != LOCK_UNLOCKED);

		return internal.read(x, y);
	}

	Color<float> Surface::sampleInternal(float x, float y, float z) const
	{
		ASSERT(internal.lock != LOCK_UNLOCKED);

		return internal.sample(x, y, z);
	}

	Color<float> Surface::sampleInternal(float x, float y) const
	{
		ASSERT(internal.lock != LOCK_UNLOCKED);

		return internal.sample(x, y);
	}

	void Surface::writeInternal(int x, int y, int z, const Color<float> &color)
	{
		ASSERT(internal.lock != LOCK_UNLOCKED);

		internal.write(x, y, z, color);
	}

	void Surface::writeInternal(int x, int y, const Color<float> &color)
	{
		ASSERT(internal.lock != LOCK_UNLOCKED);

		internal.write(x, y, color);
	}

	bool Surface::hasStencil() const
	{
		return isStencil(external.format);
	}
	
	bool Surface::hasDepth() const
	{
		return isDepth(external.format);
	}

	bool Surface::hasPalette() const
	{
		return isPalette(external.format);
	}

	bool Surface::isRenderTarget() const
	{
		return renderTarget;
	}

	bool Surface::hasDirtyMipmaps() const
	{
		return dirtyMipmaps;
	}

	void Surface::cleanMipmaps()
	{
		dirtyMipmaps = false;
	}

	Resource *Surface::getResource()
	{
		return resource;
	}

	bool Surface::identicalFormats() const
	{
		return external.format == internal.format &&
		       external.width  == internal.width &&
		       external.height == internal.height &&
		       external.depth  == internal.depth &&
		       external.pitchB == internal.pitchB &&
		       external.sliceB == internal.sliceB;
	}

	Format Surface::selectInternalFormat(Format format) const
	{
		switch(format)
		{
		case FORMAT_NULL:
			return FORMAT_NULL;
		case FORMAT_P8:
		case FORMAT_A8P8:
		case FORMAT_A4R4G4B4:
		case FORMAT_A1R5G5B5:
		case FORMAT_A8R3G3B2:
			return FORMAT_A8R8G8B8;
		case FORMAT_A8:
			return FORMAT_A8;
		case FORMAT_R8:
			return FORMAT_R8;
		case FORMAT_A2R10G10B10:
		case FORMAT_A2B10G10R10:
		case FORMAT_A16B16G16R16:
			return FORMAT_A16B16G16R16;
		case FORMAT_G8R8:
			return FORMAT_G8R8;
		case FORMAT_G16R16:
			return FORMAT_G16R16;
		case FORMAT_A8R8G8B8:
		case FORMAT_A8B8G8R8:
			if(lockable || !quadLayoutEnabled)
			{
				return FORMAT_A8R8G8B8;
			}
			else
			{
				return FORMAT_A8G8R8B8Q;
			}
		case FORMAT_R3G3B2:
		case FORMAT_R5G6B5:
		case FORMAT_R8G8B8:
		case FORMAT_X4R4G4B4:
		case FORMAT_X1R5G5B5:
		case FORMAT_X8R8G8B8:
		case FORMAT_X8B8G8R8:
			if(lockable || !quadLayoutEnabled)
			{
				return FORMAT_X8R8G8B8;
			}
			else
			{
				return FORMAT_X8G8R8B8Q;
			}
		// Compressed formats
		#if S3TC_SUPPORT
		case FORMAT_DXT1:
		case FORMAT_DXT3:
		case FORMAT_DXT5:
			return FORMAT_A8R8G8B8;
		#endif
		case FORMAT_ATI1:
			return FORMAT_R8;
		case FORMAT_ATI2:
			return FORMAT_G8R8;
		case FORMAT_ETC1:
			return FORMAT_X8R8G8B8;
		// Bumpmap formats
		case FORMAT_V8U8:			return FORMAT_V8U8;
		case FORMAT_L6V5U5:			return FORMAT_X8L8V8U8;
		case FORMAT_Q8W8V8U8:		return FORMAT_Q8W8V8U8;
		case FORMAT_X8L8V8U8:		return FORMAT_X8L8V8U8;
		case FORMAT_V16U16:			return FORMAT_V16U16;
		case FORMAT_A2W10V10U10:	return FORMAT_A16W16V16U16;
		case FORMAT_Q16W16V16U16:	return FORMAT_Q16W16V16U16;
		// Floating-point formats
		case FORMAT_R16F:			return FORMAT_R32F;
		case FORMAT_G16R16F:		return FORMAT_G32R32F;
		case FORMAT_A16B16G16R16F:	return FORMAT_A32B32G32R32F;
		case FORMAT_R32F:			return FORMAT_R32F;
		case FORMAT_G32R32F:		return FORMAT_G32R32F;
		case FORMAT_A32B32G32R32F:	return FORMAT_A32B32G32R32F;
		// Luminance formats
		case FORMAT_L8:				return FORMAT_L8;
		case FORMAT_A4L4:			return FORMAT_A8L8;
		case FORMAT_L16:			return FORMAT_L16;
		case FORMAT_A8L8:			return FORMAT_A8L8;
		// Depth/stencil formats
		case FORMAT_D16:
		case FORMAT_D32:
		case FORMAT_D24X8:
		case FORMAT_D24S8:
		case FORMAT_D24FS8:
			if(hasParent)   // Texture
			{
				return FORMAT_D32FS8_SHADOW;
			}
			else if(complementaryDepthBuffer)
			{
				return FORMAT_D32F_COMPLEMENTARY;
			}
			else
			{
				return FORMAT_D32F;
			}
		case FORMAT_D32F_LOCKABLE:  return FORMAT_D32F_LOCKABLE;
		case FORMAT_D32FS8_TEXTURE: return FORMAT_D32FS8_TEXTURE;
		case FORMAT_INTZ:           return FORMAT_D32FS8_TEXTURE;
		case FORMAT_DF24S8:         return FORMAT_D32FS8_SHADOW;
		case FORMAT_DF16S8:         return FORMAT_D32FS8_SHADOW;
		default:
			ASSERT(false);
		}

		return FORMAT_NULL;
	}

	void Surface::setTexturePalette(unsigned int *palette)
	{
		Surface::palette = palette;
		Surface::paletteID++;
	}

	void Surface::resolve()
	{
		if(internal.depth <= 1 || !internal.dirty || !renderTarget || internal.format == FORMAT_NULL)
		{
			return;
		}

		void *source = internal.lockRect(0, 0, 0, LOCK_READWRITE);

		int quality = internal.depth;
		int width = internal.width;
		int height = internal.height;
		int pitch = internal.pitchB;
		int slice = internal.sliceB;

		unsigned char *source0 = (unsigned char*)source;
		unsigned char *source1 = source0 + slice;
		unsigned char *source2 = source1 + slice;
		unsigned char *source3 = source2 + slice;
		unsigned char *source4 = source3 + slice;
		unsigned char *source5 = source4 + slice;
		unsigned char *source6 = source5 + slice;
		unsigned char *source7 = source6 + slice;
		unsigned char *source8 = source7 + slice;
		unsigned char *source9 = source8 + slice;
		unsigned char *sourceA = source9 + slice;
		unsigned char *sourceB = sourceA + slice;
		unsigned char *sourceC = sourceB + slice;
		unsigned char *sourceD = sourceC + slice;
		unsigned char *sourceE = sourceD + slice;
		unsigned char *sourceF = sourceE + slice;

		if(internal.format == FORMAT_X8R8G8B8 || internal.format == FORMAT_A8R8G8B8)
		{
			if(CPUID::supportsSSE2() && (width % 4) == 0)
			{
				if(internal.depth == 2)
				{
					for(int y = 0; y < height; y++)
					{
						for(int x = 0; x < width; x += 4)
						{
							__m128i c0 = _mm_load_si128((__m128i*)(source0 + 4 * x));
							__m128i c1 = _mm_load_si128((__m128i*)(source1 + 4 * x));
							
							c0 = _mm_avg_epu8(c0, c1);

							_mm_store_si128((__m128i*)(source0 + 4 * x), c0);
						}

						source0 += pitch;
						source1 += pitch;
					}
				}
				else if(internal.depth == 4)
				{
					for(int y = 0; y < height; y++)
					{
						for(int x = 0; x < width; x += 4)
						{
							__m128i c0 = _mm_load_si128((__m128i*)(source0 + 4 * x));
							__m128i c1 = _mm_load_si128((__m128i*)(source1 + 4 * x));
							__m128i c2 = _mm_load_si128((__m128i*)(source2 + 4 * x));
							__m128i c3 = _mm_load_si128((__m128i*)(source3 + 4 * x));
							
							c0 = _mm_avg_epu8(c0, c1);
							c2 = _mm_avg_epu8(c2, c3);
							c0 = _mm_avg_epu8(c0, c2);

							_mm_store_si128((__m128i*)(source0 + 4 * x), c0);
						}

						source0 += pitch;
						source1 += pitch;
						source2 += pitch;
						source3 += pitch;
					}
				}
				else if(internal.depth == 8)
				{
					for(int y = 0; y < height; y++)
					{
						for(int x = 0; x < width; x += 4)
						{
							__m128i c0 = _mm_load_si128((__m128i*)(source0 + 4 * x));
							__m128i c1 = _mm_load_si128((__m128i*)(source1 + 4 * x));
							__m128i c2 = _mm_load_si128((__m128i*)(source2 + 4 * x));
							__m128i c3 = _mm_load_si128((__m128i*)(source3 + 4 * x));
							__m128i c4 = _mm_load_si128((__m128i*)(source4 + 4 * x));
							__m128i c5 = _mm_load_si128((__m128i*)(source5 + 4 * x));
							__m128i c6 = _mm_load_si128((__m128i*)(source6 + 4 * x));
							__m128i c7 = _mm_load_si128((__m128i*)(source7 + 4 * x));
							
							c0 = _mm_avg_epu8(c0, c1);
							c2 = _mm_avg_epu8(c2, c3);
							c4 = _mm_avg_epu8(c4, c5);
							c6 = _mm_avg_epu8(c6, c7);
							c0 = _mm_avg_epu8(c0, c2);
							c4 = _mm_avg_epu8(c4, c6);
							c0 = _mm_avg_epu8(c0, c4);

							_mm_store_si128((__m128i*)(source0 + 4 * x), c0);
						}

						source0 += pitch;
						source1 += pitch;
						source2 += pitch;
						source3 += pitch;
						source4 += pitch;
						source5 += pitch;
						source6 += pitch;
						source7 += pitch;
					}
				}
				else if(internal.depth == 16)
				{
					for(int y = 0; y < height; y++)
					{
						for(int x = 0; x < width; x += 4)
						{
							__m128i c0 = _mm_load_si128((__m128i*)(source0 + 4 * x));
							__m128i c1 = _mm_load_si128((__m128i*)(source1 + 4 * x));
							__m128i c2 = _mm_load_si128((__m128i*)(source2 + 4 * x));
							__m128i c3 = _mm_load_si128((__m128i*)(source3 + 4 * x));
							__m128i c4 = _mm_load_si128((__m128i*)(source4 + 4 * x));
							__m128i c5 = _mm_load_si128((__m128i*)(source5 + 4 * x));
							__m128i c6 = _mm_load_si128((__m128i*)(source6 + 4 * x));
							__m128i c7 = _mm_load_si128((__m128i*)(source7 + 4 * x));
							__m128i c8 = _mm_load_si128((__m128i*)(source8 + 4 * x));
							__m128i c9 = _mm_load_si128((__m128i*)(source9 + 4 * x));
							__m128i cA = _mm_load_si128((__m128i*)(sourceA + 4 * x));
							__m128i cB = _mm_load_si128((__m128i*)(sourceB + 4 * x));
							__m128i cC = _mm_load_si128((__m128i*)(sourceC + 4 * x));
							__m128i cD = _mm_load_si128((__m128i*)(sourceD + 4 * x));
							__m128i cE = _mm_load_si128((__m128i*)(sourceE + 4 * x));
							__m128i cF = _mm_load_si128((__m128i*)(sourceF + 4 * x));

							c0 = _mm_avg_epu8(c0, c1);
							c2 = _mm_avg_epu8(c2, c3);
							c4 = _mm_avg_epu8(c4, c5);
							c6 = _mm_avg_epu8(c6, c7);
							c8 = _mm_avg_epu8(c8, c9);
							cA = _mm_avg_epu8(cA, cB);
							cC = _mm_avg_epu8(cC, cD);
							cE = _mm_avg_epu8(cE, cF);
							c0 = _mm_avg_epu8(c0, c2);
							c4 = _mm_avg_epu8(c4, c6);
							c8 = _mm_avg_epu8(c8, cA);
							cC = _mm_avg_epu8(cC, cE);
							c0 = _mm_avg_epu8(c0, c4);
							c8 = _mm_avg_epu8(c8, cC);
							c0 = _mm_avg_epu8(c0, c8);

							_mm_store_si128((__m128i*)(source0 + 4 * x), c0);
						}

						source0 += pitch;
						source1 += pitch;
						source2 += pitch;
						source3 += pitch;
						source4 += pitch;
						source5 += pitch;
						source6 += pitch;
						source7 += pitch;
						source8 += pitch;
						source9 += pitch;
						sourceA += pitch;
						sourceB += pitch;
						sourceC += pitch;
						sourceD += pitch;
						sourceE += pitch;
						sourceF += pitch;
					}
				}
				else ASSERT(false);
			}
			else
			{
				#define AVERAGE(x, y) (((x) & (y)) + ((((x) ^ (y)) >> 1) & 0x7F7F7F7F) + (((x) ^ (y)) & 0x01010101))

				if(internal.depth == 2)
				{
					for(int y = 0; y < height; y++)
					{
						for(int x = 0; x < width; x++)
						{
							unsigned int c0 = *(unsigned int*)(source0 + 4 * x);
							unsigned int c1 = *(unsigned int*)(source1 + 4 * x);

							c0 = AVERAGE(c0, c1);

							*(unsigned int*)(source0 + 4 * x) = c0;
						}

						source0 += pitch;
						source1 += pitch;
					}
				}
				else if(internal.depth == 4)
				{
					for(int y = 0; y < height; y++)
					{
						for(int x = 0; x < width; x++)
						{
							unsigned int c0 = *(unsigned int*)(source0 + 4 * x);
							unsigned int c1 = *(unsigned int*)(source1 + 4 * x);
							unsigned int c2 = *(unsigned int*)(source2 + 4 * x);
							unsigned int c3 = *(unsigned int*)(source3 + 4 * x);

							c0 = AVERAGE(c0, c1);
							c2 = AVERAGE(c2, c3);
							c0 = AVERAGE(c0, c2);

							*(unsigned int*)(source0 + 4 * x) = c0;
						}

						source0 += pitch;
						source1 += pitch;
						source2 += pitch;
						source3 += pitch;
					}
				}
				else if(internal.depth == 8)
				{
					for(int y = 0; y < height; y++)
					{
						for(int x = 0; x < width; x++)
						{
							unsigned int c0 = *(unsigned int*)(source0 + 4 * x);
							unsigned int c1 = *(unsigned int*)(source1 + 4 * x);
							unsigned int c2 = *(unsigned int*)(source2 + 4 * x);
							unsigned int c3 = *(unsigned int*)(source3 + 4 * x);
							unsigned int c4 = *(unsigned int*)(source4 + 4 * x);
							unsigned int c5 = *(unsigned int*)(source5 + 4 * x);
							unsigned int c6 = *(unsigned int*)(source6 + 4 * x);
							unsigned int c7 = *(unsigned int*)(source7 + 4 * x);

							c0 = AVERAGE(c0, c1);
							c2 = AVERAGE(c2, c3);
							c4 = AVERAGE(c4, c5);
							c6 = AVERAGE(c6, c7);
							c0 = AVERAGE(c0, c2);
							c4 = AVERAGE(c4, c6);
							c0 = AVERAGE(c0, c4);

							*(unsigned int*)(source0 + 4 * x) = c0;
						}

						source0 += pitch;
						source1 += pitch;
						source2 += pitch;
						source3 += pitch;
						source4 += pitch;
						source5 += pitch;
						source6 += pitch;
						source7 += pitch;
					}
				}
				else if(internal.depth == 16)
				{
					for(int y = 0; y < height; y++)
					{
						for(int x = 0; x < width; x++)
						{
							unsigned int c0 = *(unsigned int*)(source0 + 4 * x);
							unsigned int c1 = *(unsigned int*)(source1 + 4 * x);
							unsigned int c2 = *(unsigned int*)(source2 + 4 * x);
							unsigned int c3 = *(unsigned int*)(source3 + 4 * x);
							unsigned int c4 = *(unsigned int*)(source4 + 4 * x);
							unsigned int c5 = *(unsigned int*)(source5 + 4 * x);
							unsigned int c6 = *(unsigned int*)(source6 + 4 * x);
							unsigned int c7 = *(unsigned int*)(source7 + 4 * x);
							unsigned int c8 = *(unsigned int*)(source8 + 4 * x);
							unsigned int c9 = *(unsigned int*)(source9 + 4 * x);
							unsigned int cA = *(unsigned int*)(sourceA + 4 * x);
							unsigned int cB = *(unsigned int*)(sourceB + 4 * x);
							unsigned int cC = *(unsigned int*)(sourceC + 4 * x);
							unsigned int cD = *(unsigned int*)(sourceD + 4 * x);
							unsigned int cE = *(unsigned int*)(sourceE + 4 * x);
							unsigned int cF = *(unsigned int*)(sourceF + 4 * x);

							c0 = AVERAGE(c0, c1);
							c2 = AVERAGE(c2, c3);
							c4 = AVERAGE(c4, c5);
							c6 = AVERAGE(c6, c7);
							c8 = AVERAGE(c8, c9);
							cA = AVERAGE(cA, cB);
							cC = AVERAGE(cC, cD);
							cE = AVERAGE(cE, cF);
							c0 = AVERAGE(c0, c2);
							c4 = AVERAGE(c4, c6);
							c8 = AVERAGE(c8, cA);
							cC = AVERAGE(cC, cE);
							c0 = AVERAGE(c0, c4);
							c8 = AVERAGE(c8, cC);
							c0 = AVERAGE(c0, c8);

							*(unsigned int*)(source0 + 4 * x) = c0;
						}

						source0 += pitch;
						source1 += pitch;
						source2 += pitch;
						source3 += pitch;
						source4 += pitch;
						source5 += pitch;
						source6 += pitch;
						source7 += pitch;
						source8 += pitch;
						source9 += pitch;
						sourceA += pitch;
						sourceB += pitch;
						sourceC += pitch;
						sourceD += pitch;
						sourceE += pitch;
						sourceF += pitch;
					}
				}
				else ASSERT(false);

				#undef AVERAGE
			}
		}
		else if(internal.format == FORMAT_G16R16)
		{
			if(CPUID::supportsSSE2() && (width % 4) == 0)
			{
				if(internal.depth == 2)
				{
					for(int y = 0; y < height; y++)
					{
						for(int x = 0; x < width; x += 4)
						{
							__m128i c0 = _mm_load_si128((__m128i*)(source0 + 4 * x));
							__m128i c1 = _mm_load_si128((__m128i*)(source1 + 4 * x));
							
							c0 = _mm_avg_epu16(c0, c1);

							_mm_store_si128((__m128i*)(source0 + 4 * x), c0);
						}

						source0 += pitch;
						source1 += pitch;
					}
				}
				else if(internal.depth == 4)
				{
					for(int y = 0; y < height; y++)
					{
						for(int x = 0; x < width; x += 4)
						{
							__m128i c0 = _mm_load_si128((__m128i*)(source0 + 4 * x));
							__m128i c1 = _mm_load_si128((__m128i*)(source1 + 4 * x));
							__m128i c2 = _mm_load_si128((__m128i*)(source2 + 4 * x));
							__m128i c3 = _mm_load_si128((__m128i*)(source3 + 4 * x));
							
							c0 = _mm_avg_epu16(c0, c1);
							c2 = _mm_avg_epu16(c2, c3);
							c0 = _mm_avg_epu16(c0, c2);

							_mm_store_si128((__m128i*)(source0 + 4 * x), c0);
						}

						source0 += pitch;
						source1 += pitch;
						source2 += pitch;
						source3 += pitch;
					}
				}
				else if(internal.depth == 8)
				{
					for(int y = 0; y < height; y++)
					{
						for(int x = 0; x < width; x += 4)
						{
							__m128i c0 = _mm_load_si128((__m128i*)(source0 + 4 * x));
							__m128i c1 = _mm_load_si128((__m128i*)(source1 + 4 * x));
							__m128i c2 = _mm_load_si128((__m128i*)(source2 + 4 * x));
							__m128i c3 = _mm_load_si128((__m128i*)(source3 + 4 * x));
							__m128i c4 = _mm_load_si128((__m128i*)(source4 + 4 * x));
							__m128i c5 = _mm_load_si128((__m128i*)(source5 + 4 * x));
							__m128i c6 = _mm_load_si128((__m128i*)(source6 + 4 * x));
							__m128i c7 = _mm_load_si128((__m128i*)(source7 + 4 * x));
							
							c0 = _mm_avg_epu16(c0, c1);
							c2 = _mm_avg_epu16(c2, c3);
							c4 = _mm_avg_epu16(c4, c5);
							c6 = _mm_avg_epu16(c6, c7);
							c0 = _mm_avg_epu16(c0, c2);
							c4 = _mm_avg_epu16(c4, c6);
							c0 = _mm_avg_epu16(c0, c4);

							_mm_store_si128((__m128i*)(source0 + 4 * x), c0);
						}

						source0 += pitch;
						source1 += pitch;
						source2 += pitch;
						source3 += pitch;
						source4 += pitch;
						source5 += pitch;
						source6 += pitch;
						source7 += pitch;
					}
				}
				else if(internal.depth == 16)
				{
					for(int y = 0; y < height; y++)
					{
						for(int x = 0; x < width; x += 4)
						{
							__m128i c0 = _mm_load_si128((__m128i*)(source0 + 4 * x));
							__m128i c1 = _mm_load_si128((__m128i*)(source1 + 4 * x));
							__m128i c2 = _mm_load_si128((__m128i*)(source2 + 4 * x));
							__m128i c3 = _mm_load_si128((__m128i*)(source3 + 4 * x));
							__m128i c4 = _mm_load_si128((__m128i*)(source4 + 4 * x));
							__m128i c5 = _mm_load_si128((__m128i*)(source5 + 4 * x));
							__m128i c6 = _mm_load_si128((__m128i*)(source6 + 4 * x));
							__m128i c7 = _mm_load_si128((__m128i*)(source7 + 4 * x));
							__m128i c8 = _mm_load_si128((__m128i*)(source8 + 4 * x));
							__m128i c9 = _mm_load_si128((__m128i*)(source9 + 4 * x));
							__m128i cA = _mm_load_si128((__m128i*)(sourceA + 4 * x));
							__m128i cB = _mm_load_si128((__m128i*)(sourceB + 4 * x));
							__m128i cC = _mm_load_si128((__m128i*)(sourceC + 4 * x));
							__m128i cD = _mm_load_si128((__m128i*)(sourceD + 4 * x));
							__m128i cE = _mm_load_si128((__m128i*)(sourceE + 4 * x));
							__m128i cF = _mm_load_si128((__m128i*)(sourceF + 4 * x));

							c0 = _mm_avg_epu16(c0, c1);
							c2 = _mm_avg_epu16(c2, c3);
							c4 = _mm_avg_epu16(c4, c5);
							c6 = _mm_avg_epu16(c6, c7);
							c8 = _mm_avg_epu16(c8, c9);
							cA = _mm_avg_epu16(cA, cB);
							cC = _mm_avg_epu16(cC, cD);
							cE = _mm_avg_epu16(cE, cF);
							c0 = _mm_avg_epu16(c0, c2);
							c4 = _mm_avg_epu16(c4, c6);
							c8 = _mm_avg_epu16(c8, cA);
							cC = _mm_avg_epu16(cC, cE);
							c0 = _mm_avg_epu16(c0, c4);
							c8 = _mm_avg_epu16(c8, cC);
							c0 = _mm_avg_epu16(c0, c8);

							_mm_store_si128((__m128i*)(source0 + 4 * x), c0);
						}

						source0 += pitch;
						source1 += pitch;
						source2 += pitch;
						source3 += pitch;
						source4 += pitch;
						source5 += pitch;
						source6 += pitch;
						source7 += pitch;
						source8 += pitch;
						source9 += pitch;
						sourceA += pitch;
						sourceB += pitch;
						sourceC += pitch;
						sourceD += pitch;
						sourceE += pitch;
						sourceF += pitch;
					}
				}
				else ASSERT(false);
			}
			else
			{
				#define AVERAGE(x, y) (((x) & (y)) + ((((x) ^ (y)) >> 1) & 0x7FFF7FFF) + (((x) ^ (y)) & 0x00010001))

				if(internal.depth == 2)
				{
					for(int y = 0; y < height; y++)
					{
						for(int x = 0; x < width; x++)
						{
							unsigned int c0 = *(unsigned int*)(source0 + 4 * x);
							unsigned int c1 = *(unsigned int*)(source1 + 4 * x);

							c0 = AVERAGE(c0, c1);

							*(unsigned int*)(source0 + 4 * x) = c0;
						}

						source0 += pitch;
						source1 += pitch;
					}
				}
				else if(internal.depth == 4)
				{
					for(int y = 0; y < height; y++)
					{
						for(int x = 0; x < width; x++)
						{
							unsigned int c0 = *(unsigned int*)(source0 + 4 * x);
							unsigned int c1 = *(unsigned int*)(source1 + 4 * x);
							unsigned int c2 = *(unsigned int*)(source2 + 4 * x);
							unsigned int c3 = *(unsigned int*)(source3 + 4 * x);

							c0 = AVERAGE(c0, c1);
							c2 = AVERAGE(c2, c3);
							c0 = AVERAGE(c0, c2);

							*(unsigned int*)(source0 + 4 * x) = c0;
						}

						source0 += pitch;
						source1 += pitch;
						source2 += pitch;
						source3 += pitch;
					}
				}
				else if(internal.depth == 8)
				{
					for(int y = 0; y < height; y++)
					{
						for(int x = 0; x < width; x++)
						{
							unsigned int c0 = *(unsigned int*)(source0 + 4 * x);
							unsigned int c1 = *(unsigned int*)(source1 + 4 * x);
							unsigned int c2 = *(unsigned int*)(source2 + 4 * x);
							unsigned int c3 = *(unsigned int*)(source3 + 4 * x);
							unsigned int c4 = *(unsigned int*)(source4 + 4 * x);
							unsigned int c5 = *(unsigned int*)(source5 + 4 * x);
							unsigned int c6 = *(unsigned int*)(source6 + 4 * x);
							unsigned int c7 = *(unsigned int*)(source7 + 4 * x);

							c0 = AVERAGE(c0, c1);
							c2 = AVERAGE(c2, c3);
							c4 = AVERAGE(c4, c5);
							c6 = AVERAGE(c6, c7);
							c0 = AVERAGE(c0, c2);
							c4 = AVERAGE(c4, c6);
							c0 = AVERAGE(c0, c4);

							*(unsigned int*)(source0 + 4 * x) = c0;
						}

						source0 += pitch;
						source1 += pitch;
						source2 += pitch;
						source3 += pitch;
						source4 += pitch;
						source5 += pitch;
						source6 += pitch;
						source7 += pitch;
					}
				}
				else if(internal.depth == 16)
				{
					for(int y = 0; y < height; y++)
					{
						for(int x = 0; x < width; x++)
						{
							unsigned int c0 = *(unsigned int*)(source0 + 4 * x);
							unsigned int c1 = *(unsigned int*)(source1 + 4 * x);
							unsigned int c2 = *(unsigned int*)(source2 + 4 * x);
							unsigned int c3 = *(unsigned int*)(source3 + 4 * x);
							unsigned int c4 = *(unsigned int*)(source4 + 4 * x);
							unsigned int c5 = *(unsigned int*)(source5 + 4 * x);
							unsigned int c6 = *(unsigned int*)(source6 + 4 * x);
							unsigned int c7 = *(unsigned int*)(source7 + 4 * x);
							unsigned int c8 = *(unsigned int*)(source8 + 4 * x);
							unsigned int c9 = *(unsigned int*)(source9 + 4 * x);
							unsigned int cA = *(unsigned int*)(sourceA + 4 * x);
							unsigned int cB = *(unsigned int*)(sourceB + 4 * x);
							unsigned int cC = *(unsigned int*)(sourceC + 4 * x);
							unsigned int cD = *(unsigned int*)(sourceD + 4 * x);
							unsigned int cE = *(unsigned int*)(sourceE + 4 * x);
							unsigned int cF = *(unsigned int*)(sourceF + 4 * x);

							c0 = AVERAGE(c0, c1);
							c2 = AVERAGE(c2, c3);
							c4 = AVERAGE(c4, c5);
							c6 = AVERAGE(c6, c7);
							c8 = AVERAGE(c8, c9);
							cA = AVERAGE(cA, cB);
							cC = AVERAGE(cC, cD);
							cE = AVERAGE(cE, cF);
							c0 = AVERAGE(c0, c2);
							c4 = AVERAGE(c4, c6);
							c8 = AVERAGE(c8, cA);
							cC = AVERAGE(cC, cE);
							c0 = AVERAGE(c0, c4);
							c8 = AVERAGE(c8, cC);
							c0 = AVERAGE(c0, c8);

							*(unsigned int*)(source0 + 4 * x) = c0;
						}

						source0 += pitch;
						source1 += pitch;
						source2 += pitch;
						source3 += pitch;
						source4 += pitch;
						source5 += pitch;
						source6 += pitch;
						source7 += pitch;
						source8 += pitch;
						source9 += pitch;
						sourceA += pitch;
						sourceB += pitch;
						sourceC += pitch;
						sourceD += pitch;
						sourceE += pitch;
						sourceF += pitch;
					}
				}
				else ASSERT(false);

				#undef AVERAGE
			}
		}
		else if(internal.format == FORMAT_A16B16G16R16)
		{
			if(CPUID::supportsSSE2() && (width % 2) == 0)
			{
				if(internal.depth == 2)
				{
					for(int y = 0; y < height; y++)
					{
						for(int x = 0; x < width; x += 2)
						{
							__m128i c0 = _mm_load_si128((__m128i*)(source0 + 8 * x));
							__m128i c1 = _mm_load_si128((__m128i*)(source1 + 8 * x));
							
							c0 = _mm_avg_epu16(c0, c1);

							_mm_store_si128((__m128i*)(source0 + 8 * x), c0);
						}

						source0 += pitch;
						source1 += pitch;
					}
				}
				else if(internal.depth == 4)
				{
					for(int y = 0; y < height; y++)
					{
						for(int x = 0; x < width; x += 2)
						{
							__m128i c0 = _mm_load_si128((__m128i*)(source0 + 8 * x));
							__m128i c1 = _mm_load_si128((__m128i*)(source1 + 8 * x));
							__m128i c2 = _mm_load_si128((__m128i*)(source2 + 8 * x));
							__m128i c3 = _mm_load_si128((__m128i*)(source3 + 8 * x));
							
							c0 = _mm_avg_epu16(c0, c1);
							c2 = _mm_avg_epu16(c2, c3);
							c0 = _mm_avg_epu16(c0, c2);

							_mm_store_si128((__m128i*)(source0 + 8 * x), c0);
						}

						source0 += pitch;
						source1 += pitch;
						source2 += pitch;
						source3 += pitch;
					}
				}
				else if(internal.depth == 8)
				{
					for(int y = 0; y < height; y++)
					{
						for(int x = 0; x < width; x += 2)
						{
							__m128i c0 = _mm_load_si128((__m128i*)(source0 + 8 * x));
							__m128i c1 = _mm_load_si128((__m128i*)(source1 + 8 * x));
							__m128i c2 = _mm_load_si128((__m128i*)(source2 + 8 * x));
							__m128i c3 = _mm_load_si128((__m128i*)(source3 + 8 * x));
							__m128i c4 = _mm_load_si128((__m128i*)(source4 + 8 * x));
							__m128i c5 = _mm_load_si128((__m128i*)(source5 + 8 * x));
							__m128i c6 = _mm_load_si128((__m128i*)(source6 + 8 * x));
							__m128i c7 = _mm_load_si128((__m128i*)(source7 + 8 * x));
							
							c0 = _mm_avg_epu16(c0, c1);
							c2 = _mm_avg_epu16(c2, c3);
							c4 = _mm_avg_epu16(c4, c5);
							c6 = _mm_avg_epu16(c6, c7);
							c0 = _mm_avg_epu16(c0, c2);
							c4 = _mm_avg_epu16(c4, c6);
							c0 = _mm_avg_epu16(c0, c4);

							_mm_store_si128((__m128i*)(source0 + 8 * x), c0);
						}

						source0 += pitch;
						source1 += pitch;
						source2 += pitch;
						source3 += pitch;
						source4 += pitch;
						source5 += pitch;
						source6 += pitch;
						source7 += pitch;
					}
				}
				else if(internal.depth == 16)
				{
					for(int y = 0; y < height; y++)
					{
						for(int x = 0; x < width; x += 2)
						{
							__m128i c0 = _mm_load_si128((__m128i*)(source0 + 8 * x));
							__m128i c1 = _mm_load_si128((__m128i*)(source1 + 8 * x));
							__m128i c2 = _mm_load_si128((__m128i*)(source2 + 8 * x));
							__m128i c3 = _mm_load_si128((__m128i*)(source3 + 8 * x));
							__m128i c4 = _mm_load_si128((__m128i*)(source4 + 8 * x));
							__m128i c5 = _mm_load_si128((__m128i*)(source5 + 8 * x));
							__m128i c6 = _mm_load_si128((__m128i*)(source6 + 8 * x));
							__m128i c7 = _mm_load_si128((__m128i*)(source7 + 8 * x));
							__m128i c8 = _mm_load_si128((__m128i*)(source8 + 8 * x));
							__m128i c9 = _mm_load_si128((__m128i*)(source9 + 8 * x));
							__m128i cA = _mm_load_si128((__m128i*)(sourceA + 8 * x));
							__m128i cB = _mm_load_si128((__m128i*)(sourceB + 8 * x));
							__m128i cC = _mm_load_si128((__m128i*)(sourceC + 8 * x));
							__m128i cD = _mm_load_si128((__m128i*)(sourceD + 8 * x));
							__m128i cE = _mm_load_si128((__m128i*)(sourceE + 8 * x));
							__m128i cF = _mm_load_si128((__m128i*)(sourceF + 8 * x));

							c0 = _mm_avg_epu16(c0, c1);
							c2 = _mm_avg_epu16(c2, c3);
							c4 = _mm_avg_epu16(c4, c5);
							c6 = _mm_avg_epu16(c6, c7);
							c8 = _mm_avg_epu16(c8, c9);
							cA = _mm_avg_epu16(cA, cB);
							cC = _mm_avg_epu16(cC, cD);
							cE = _mm_avg_epu16(cE, cF);
							c0 = _mm_avg_epu16(c0, c2);
							c4 = _mm_avg_epu16(c4, c6);
							c8 = _mm_avg_epu16(c8, cA);
							cC = _mm_avg_epu16(cC, cE);
							c0 = _mm_avg_epu16(c0, c4);
							c8 = _mm_avg_epu16(c8, cC);
							c0 = _mm_avg_epu16(c0, c8);

							_mm_store_si128((__m128i*)(source0 + 8 * x), c0);
						}

						source0 += pitch;
						source1 += pitch;
						source2 += pitch;
						source3 += pitch;
						source4 += pitch;
						source5 += pitch;
						source6 += pitch;
						source7 += pitch;
						source8 += pitch;
						source9 += pitch;
						sourceA += pitch;
						sourceB += pitch;
						sourceC += pitch;
						sourceD += pitch;
						sourceE += pitch;
						sourceF += pitch;
					}
				}
				else ASSERT(false);
			}
			else
			{
				#define AVERAGE(x, y) (((x) & (y)) + ((((x) ^ (y)) >> 1) & 0x7FFF7FFF) + (((x) ^ (y)) & 0x00010001))

				if(internal.depth == 2)
				{
					for(int y = 0; y < height; y++)
					{
						for(int x = 0; x < 2 * width; x++)
						{
							unsigned int c0 = *(unsigned int*)(source0 + 4 * x);
							unsigned int c1 = *(unsigned int*)(source1 + 4 * x);

							c0 = AVERAGE(c0, c1);

							*(unsigned int*)(source0 + 4 * x) = c0;
						}

						source0 += pitch;
						source1 += pitch;
					}
				}
				else if(internal.depth == 4)
				{
					for(int y = 0; y < height; y++)
					{
						for(int x = 0; x < 2 * width; x++)
						{
							unsigned int c0 = *(unsigned int*)(source0 + 4 * x);
							unsigned int c1 = *(unsigned int*)(source1 + 4 * x);
							unsigned int c2 = *(unsigned int*)(source2 + 4 * x);
							unsigned int c3 = *(unsigned int*)(source3 + 4 * x);

							c0 = AVERAGE(c0, c1);
							c2 = AVERAGE(c2, c3);
							c0 = AVERAGE(c0, c2);

							*(unsigned int*)(source0 + 4 * x) = c0;
						}

						source0 += pitch;
						source1 += pitch;
						source2 += pitch;
						source3 += pitch;
					}
				}
				else if(internal.depth == 8)
				{
					for(int y = 0; y < height; y++)
					{
						for(int x = 0; x < 2 * width; x++)
						{
							unsigned int c0 = *(unsigned int*)(source0 + 4 * x);
							unsigned int c1 = *(unsigned int*)(source1 + 4 * x);
							unsigned int c2 = *(unsigned int*)(source2 + 4 * x);
							unsigned int c3 = *(unsigned int*)(source3 + 4 * x);
							unsigned int c4 = *(unsigned int*)(source4 + 4 * x);
							unsigned int c5 = *(unsigned int*)(source5 + 4 * x);
							unsigned int c6 = *(unsigned int*)(source6 + 4 * x);
							unsigned int c7 = *(unsigned int*)(source7 + 4 * x);

							c0 = AVERAGE(c0, c1);
							c2 = AVERAGE(c2, c3);
							c4 = AVERAGE(c4, c5);
							c6 = AVERAGE(c6, c7);
							c0 = AVERAGE(c0, c2);
							c4 = AVERAGE(c4, c6);
							c0 = AVERAGE(c0, c4);

							*(unsigned int*)(source0 + 4 * x) = c0;
						}

						source0 += pitch;
						source1 += pitch;
						source2 += pitch;
						source3 += pitch;
						source4 += pitch;
						source5 += pitch;
						source6 += pitch;
						source7 += pitch;
					}
				}
				else if(internal.depth == 16)
				{
					for(int y = 0; y < height; y++)
					{
						for(int x = 0; x < 2 * width; x++)
						{
							unsigned int c0 = *(unsigned int*)(source0 + 4 * x);
							unsigned int c1 = *(unsigned int*)(source1 + 4 * x);
							unsigned int c2 = *(unsigned int*)(source2 + 4 * x);
							unsigned int c3 = *(unsigned int*)(source3 + 4 * x);
							unsigned int c4 = *(unsigned int*)(source4 + 4 * x);
							unsigned int c5 = *(unsigned int*)(source5 + 4 * x);
							unsigned int c6 = *(unsigned int*)(source6 + 4 * x);
							unsigned int c7 = *(unsigned int*)(source7 + 4 * x);
							unsigned int c8 = *(unsigned int*)(source8 + 4 * x);
							unsigned int c9 = *(unsigned int*)(source9 + 4 * x);
							unsigned int cA = *(unsigned int*)(sourceA + 4 * x);
							unsigned int cB = *(unsigned int*)(sourceB + 4 * x);
							unsigned int cC = *(unsigned int*)(sourceC + 4 * x);
							unsigned int cD = *(unsigned int*)(sourceD + 4 * x);
							unsigned int cE = *(unsigned int*)(sourceE + 4 * x);
							unsigned int cF = *(unsigned int*)(sourceF + 4 * x);

							c0 = AVERAGE(c0, c1);
							c2 = AVERAGE(c2, c3);
							c4 = AVERAGE(c4, c5);
							c6 = AVERAGE(c6, c7);
							c8 = AVERAGE(c8, c9);
							cA = AVERAGE(cA, cB);
							cC = AVERAGE(cC, cD);
							cE = AVERAGE(cE, cF);
							c0 = AVERAGE(c0, c2);
							c4 = AVERAGE(c4, c6);
							c8 = AVERAGE(c8, cA);
							cC = AVERAGE(cC, cE);
							c0 = AVERAGE(c0, c4);
							c8 = AVERAGE(c8, cC);
							c0 = AVERAGE(c0, c8);

							*(unsigned int*)(source0 + 4 * x) = c0;
						}

						source0 += pitch;
						source1 += pitch;
						source2 += pitch;
						source3 += pitch;
						source4 += pitch;
						source5 += pitch;
						source6 += pitch;
						source7 += pitch;
						source8 += pitch;
						source9 += pitch;
						sourceA += pitch;
						sourceB += pitch;
						sourceC += pitch;
						sourceD += pitch;
						sourceE += pitch;
						sourceF += pitch;
					}
				}
				else ASSERT(false);

				#undef AVERAGE
			}
		}
		else if(internal.format == FORMAT_R32F)
		{
			if(CPUID::supportsSSE() && (width % 4) == 0)
			{
				if(internal.depth == 2)
				{
					for(int y = 0; y < height; y++)
					{
						for(int x = 0; x < width; x += 4)
						{
							__m128 c0 = _mm_load_ps((float*)(source0 + 4 * x));
							__m128 c1 = _mm_load_ps((float*)(source1 + 4 * x));
							
							c0 = _mm_add_ps(c0, c1);
							c0 = _mm_mul_ps(c0, _mm_set1_ps(1.0f / 2.0f)); 

							_mm_store_ps((float*)(source0 + 4 * x), c0);
						}

						source0 += pitch;
						source1 += pitch;
					}
				}
				else if(internal.depth == 4)
				{
					for(int y = 0; y < height; y++)
					{
						for(int x = 0; x < width; x += 4)
						{
							__m128 c0 = _mm_load_ps((float*)(source0 + 4 * x));
							__m128 c1 = _mm_load_ps((float*)(source1 + 4 * x));
							__m128 c2 = _mm_load_ps((float*)(source2 + 4 * x));
							__m128 c3 = _mm_load_ps((float*)(source3 + 4 * x));
							
							c0 = _mm_add_ps(c0, c1);
							c2 = _mm_add_ps(c2, c3);
							c0 = _mm_add_ps(c0, c2);
							c0 = _mm_mul_ps(c0, _mm_set1_ps(1.0f / 4.0f)); 

							_mm_store_ps((float*)(source0 + 4 * x), c0);
						}

						source0 += pitch;
						source1 += pitch;
						source2 += pitch;
						source3 += pitch;
					}
				}
				else if(internal.depth == 8)
				{
					for(int y = 0; y < height; y++)
					{
						for(int x = 0; x < width; x += 4)
						{
							__m128 c0 = _mm_load_ps((float*)(source0 + 4 * x));
							__m128 c1 = _mm_load_ps((float*)(source1 + 4 * x));
							__m128 c2 = _mm_load_ps((float*)(source2 + 4 * x));
							__m128 c3 = _mm_load_ps((float*)(source3 + 4 * x));
							__m128 c4 = _mm_load_ps((float*)(source4 + 4 * x));
							__m128 c5 = _mm_load_ps((float*)(source5 + 4 * x));
							__m128 c6 = _mm_load_ps((float*)(source6 + 4 * x));
							__m128 c7 = _mm_load_ps((float*)(source7 + 4 * x));
							
							c0 = _mm_add_ps(c0, c1);
							c2 = _mm_add_ps(c2, c3);
							c4 = _mm_add_ps(c4, c5);
							c6 = _mm_add_ps(c6, c7);
							c0 = _mm_add_ps(c0, c2);
							c4 = _mm_add_ps(c4, c6);
							c0 = _mm_add_ps(c0, c4);
							c0 = _mm_mul_ps(c0, _mm_set1_ps(1.0f / 8.0f)); 

							_mm_store_ps((float*)(source0 + 4 * x), c0);
						}

						source0 += pitch;
						source1 += pitch;
						source2 += pitch;
						source3 += pitch;
						source4 += pitch;
						source5 += pitch;
						source6 += pitch;
						source7 += pitch;
					}
				}
				else if(internal.depth == 16)
				{
					for(int y = 0; y < height; y++)
					{
						for(int x = 0; x < width; x += 4)
						{
							__m128 c0 = _mm_load_ps((float*)(source0 + 4 * x));
							__m128 c1 = _mm_load_ps((float*)(source1 + 4 * x));
							__m128 c2 = _mm_load_ps((float*)(source2 + 4 * x));
							__m128 c3 = _mm_load_ps((float*)(source3 + 4 * x));
							__m128 c4 = _mm_load_ps((float*)(source4 + 4 * x));
							__m128 c5 = _mm_load_ps((float*)(source5 + 4 * x));
							__m128 c6 = _mm_load_ps((float*)(source6 + 4 * x));
							__m128 c7 = _mm_load_ps((float*)(source7 + 4 * x));
							__m128 c8 = _mm_load_ps((float*)(source8 + 4 * x));
							__m128 c9 = _mm_load_ps((float*)(source9 + 4 * x));
							__m128 cA = _mm_load_ps((float*)(sourceA + 4 * x));
							__m128 cB = _mm_load_ps((float*)(sourceB + 4 * x));
							__m128 cC = _mm_load_ps((float*)(sourceC + 4 * x));
							__m128 cD = _mm_load_ps((float*)(sourceD + 4 * x));
							__m128 cE = _mm_load_ps((float*)(sourceE + 4 * x));
							__m128 cF = _mm_load_ps((float*)(sourceF + 4 * x));

							c0 = _mm_add_ps(c0, c1);
							c2 = _mm_add_ps(c2, c3);
							c4 = _mm_add_ps(c4, c5);
							c6 = _mm_add_ps(c6, c7);
							c8 = _mm_add_ps(c8, c9);
							cA = _mm_add_ps(cA, cB);
							cC = _mm_add_ps(cC, cD);
							cE = _mm_add_ps(cE, cF);
							c0 = _mm_add_ps(c0, c2);
							c4 = _mm_add_ps(c4, c6);
							c8 = _mm_add_ps(c8, cA);
							cC = _mm_add_ps(cC, cE);
							c0 = _mm_add_ps(c0, c4);
							c8 = _mm_add_ps(c8, cC);
							c0 = _mm_add_ps(c0, c8);
							c0 = _mm_mul_ps(c0, _mm_set1_ps(1.0f / 16.0f)); 

							_mm_store_ps((float*)(source0 + 4 * x), c0);
						}

						source0 += pitch;
						source1 += pitch;
						source2 += pitch;
						source3 += pitch;
						source4 += pitch;
						source5 += pitch;
						source6 += pitch;
						source7 += pitch;
						source8 += pitch;
						source9 += pitch;
						sourceA += pitch;
						sourceB += pitch;
						sourceC += pitch;
						sourceD += pitch;
						sourceE += pitch;
						sourceF += pitch;
					}
				}
				else ASSERT(false);
			}
			else
			{
				if(internal.depth == 2)
				{
					for(int y = 0; y < height; y++)
					{
						for(int x = 0; x < width; x++)
						{
							float c0 = *(float*)(source0 + 4 * x);
							float c1 = *(float*)(source1 + 4 * x);

							c0 = c0 + c1;
							c0 *= 1.0f / 2.0f;

							*(float*)(source0 + 4 * x) = c0;
						}

						source0 += pitch;
						source1 += pitch;
					}
				}
				else if(internal.depth == 4)
				{
					for(int y = 0; y < height; y++)
					{
						for(int x = 0; x < width; x++)
						{
							float c0 = *(float*)(source0 + 4 * x);
							float c1 = *(float*)(source1 + 4 * x);
							float c2 = *(float*)(source2 + 4 * x);
							float c3 = *(float*)(source3 + 4 * x);

							c0 = c0 + c1;
							c2 = c2 + c3;
							c0 = c0 + c2;
							c0 *= 1.0f / 4.0f;

							*(float*)(source0 + 4 * x) = c0;
						}

						source0 += pitch;
						source1 += pitch;
						source2 += pitch;
						source3 += pitch;
					}
				}
				else if(internal.depth == 8)
				{
					for(int y = 0; y < height; y++)
					{
						for(int x = 0; x < width; x++)
						{
							float c0 = *(float*)(source0 + 4 * x);
							float c1 = *(float*)(source1 + 4 * x);
							float c2 = *(float*)(source2 + 4 * x);
							float c3 = *(float*)(source3 + 4 * x);
							float c4 = *(float*)(source4 + 4 * x);
							float c5 = *(float*)(source5 + 4 * x);
							float c6 = *(float*)(source6 + 4 * x);
							float c7 = *(float*)(source7 + 4 * x);

							c0 = c0 + c1;
							c2 = c2 + c3;
							c4 = c4 + c5;
							c6 = c6 + c7;
							c0 = c0 + c2;
							c4 = c4 + c6;
							c0 = c0 + c4;
							c0 *= 1.0f / 8.0f;

							*(float*)(source0 + 4 * x) = c0;
						}

						source0 += pitch;
						source1 += pitch;
						source2 += pitch;
						source3 += pitch;
						source4 += pitch;
						source5 += pitch;
						source6 += pitch;
						source7 += pitch;
					}
				}
				else if(internal.depth == 16)
				{
					for(int y = 0; y < height; y++)
					{
						for(int x = 0; x < width; x++)
						{
							float c0 = *(float*)(source0 + 4 * x);
							float c1 = *(float*)(source1 + 4 * x);
							float c2 = *(float*)(source2 + 4 * x);
							float c3 = *(float*)(source3 + 4 * x);
							float c4 = *(float*)(source4 + 4 * x);
							float c5 = *(float*)(source5 + 4 * x);
							float c6 = *(float*)(source6 + 4 * x);
							float c7 = *(float*)(source7 + 4 * x);
							float c8 = *(float*)(source8 + 4 * x);
							float c9 = *(float*)(source9 + 4 * x);
							float cA = *(float*)(sourceA + 4 * x);
							float cB = *(float*)(sourceB + 4 * x);
							float cC = *(float*)(sourceC + 4 * x);
							float cD = *(float*)(sourceD + 4 * x);
							float cE = *(float*)(sourceE + 4 * x);
							float cF = *(float*)(sourceF + 4 * x);

							c0 = c0 + c1;
							c2 = c2 + c3;
							c4 = c4 + c5;
							c6 = c6 + c7;
							c8 = c8 + c9;
							cA = cA + cB;
							cC = cC + cD;
							cE = cE + cF;
							c0 = c0 + c2;
							c4 = c4 + c6;
							c8 = c8 + cA;
							cC = cC + cE;
							c0 = c0 + c4;
							c8 = c8 + cC;
							c0 = c0 + c8;
							c0 *= 1.0f / 16.0f;

							*(float*)(source0 + 4 * x) = c0;
						}

						source0 += pitch;
						source1 += pitch;
						source2 += pitch;
						source3 += pitch;
						source4 += pitch;
						source5 += pitch;
						source6 += pitch;
						source7 += pitch;
						source8 += pitch;
						source9 += pitch;
						sourceA += pitch;
						sourceB += pitch;
						sourceC += pitch;
						sourceD += pitch;
						sourceE += pitch;
						sourceF += pitch;
					}
				}
				else ASSERT(false);
			}
		}
		else if(internal.format == FORMAT_G32R32F)
		{
			if(CPUID::supportsSSE() && (width % 2) == 0)
			{
				if(internal.depth == 2)
				{
					for(int y = 0; y < height; y++)
					{
						for(int x = 0; x < width; x += 2)
						{
							__m128 c0 = _mm_load_ps((float*)(source0 + 8 * x));
							__m128 c1 = _mm_load_ps((float*)(source1 + 8 * x));
							
							c0 = _mm_add_ps(c0, c1);
							c0 = _mm_mul_ps(c0, _mm_set1_ps(1.0f / 2.0f)); 

							_mm_store_ps((float*)(source0 + 8 * x), c0);
						}

						source0 += pitch;
						source1 += pitch;
					}
				}
				else if(internal.depth == 4)
				{
					for(int y = 0; y < height; y++)
					{
						for(int x = 0; x < width; x += 2)
						{
							__m128 c0 = _mm_load_ps((float*)(source0 + 8 * x));
							__m128 c1 = _mm_load_ps((float*)(source1 + 8 * x));
							__m128 c2 = _mm_load_ps((float*)(source2 + 8 * x));
							__m128 c3 = _mm_load_ps((float*)(source3 + 8 * x));
							
							c0 = _mm_add_ps(c0, c1);
							c2 = _mm_add_ps(c2, c3);
							c0 = _mm_add_ps(c0, c2);
							c0 = _mm_mul_ps(c0, _mm_set1_ps(1.0f / 4.0f)); 

							_mm_store_ps((float*)(source0 + 8 * x), c0);
						}

						source0 += pitch;
						source1 += pitch;
						source2 += pitch;
						source3 += pitch;
					}
				}
				else if(internal.depth == 8)
				{
					for(int y = 0; y < height; y++)
					{
						for(int x = 0; x < width; x += 2)
						{
							__m128 c0 = _mm_load_ps((float*)(source0 + 8 * x));
							__m128 c1 = _mm_load_ps((float*)(source1 + 8 * x));
							__m128 c2 = _mm_load_ps((float*)(source2 + 8 * x));
							__m128 c3 = _mm_load_ps((float*)(source3 + 8 * x));
							__m128 c4 = _mm_load_ps((float*)(source4 + 8 * x));
							__m128 c5 = _mm_load_ps((float*)(source5 + 8 * x));
							__m128 c6 = _mm_load_ps((float*)(source6 + 8 * x));
							__m128 c7 = _mm_load_ps((float*)(source7 + 8 * x));
							
							c0 = _mm_add_ps(c0, c1);
							c2 = _mm_add_ps(c2, c3);
							c4 = _mm_add_ps(c4, c5);
							c6 = _mm_add_ps(c6, c7);
							c0 = _mm_add_ps(c0, c2);
							c4 = _mm_add_ps(c4, c6);
							c0 = _mm_add_ps(c0, c4);
							c0 = _mm_mul_ps(c0, _mm_set1_ps(1.0f / 8.0f)); 

							_mm_store_ps((float*)(source0 + 8 * x), c0);
						}

						source0 += pitch;
						source1 += pitch;
						source2 += pitch;
						source3 += pitch;
						source4 += pitch;
						source5 += pitch;
						source6 += pitch;
						source7 += pitch;
					}
				}
				else if(internal.depth == 16)
				{
					for(int y = 0; y < height; y++)
					{
						for(int x = 0; x < width; x += 2)
						{
							__m128 c0 = _mm_load_ps((float*)(source0 + 8 * x));
							__m128 c1 = _mm_load_ps((float*)(source1 + 8 * x));
							__m128 c2 = _mm_load_ps((float*)(source2 + 8 * x));
							__m128 c3 = _mm_load_ps((float*)(source3 + 8 * x));
							__m128 c4 = _mm_load_ps((float*)(source4 + 8 * x));
							__m128 c5 = _mm_load_ps((float*)(source5 + 8 * x));
							__m128 c6 = _mm_load_ps((float*)(source6 + 8 * x));
							__m128 c7 = _mm_load_ps((float*)(source7 + 8 * x));
							__m128 c8 = _mm_load_ps((float*)(source8 + 8 * x));
							__m128 c9 = _mm_load_ps((float*)(source9 + 8 * x));
							__m128 cA = _mm_load_ps((float*)(sourceA + 8 * x));
							__m128 cB = _mm_load_ps((float*)(sourceB + 8 * x));
							__m128 cC = _mm_load_ps((float*)(sourceC + 8 * x));
							__m128 cD = _mm_load_ps((float*)(sourceD + 8 * x));
							__m128 cE = _mm_load_ps((float*)(sourceE + 8 * x));
							__m128 cF = _mm_load_ps((float*)(sourceF + 8 * x));

							c0 = _mm_add_ps(c0, c1);
							c2 = _mm_add_ps(c2, c3);
							c4 = _mm_add_ps(c4, c5);
							c6 = _mm_add_ps(c6, c7);
							c8 = _mm_add_ps(c8, c9);
							cA = _mm_add_ps(cA, cB);
							cC = _mm_add_ps(cC, cD);
							cE = _mm_add_ps(cE, cF);
							c0 = _mm_add_ps(c0, c2);
							c4 = _mm_add_ps(c4, c6);
							c8 = _mm_add_ps(c8, cA);
							cC = _mm_add_ps(cC, cE);
							c0 = _mm_add_ps(c0, c4);
							c8 = _mm_add_ps(c8, cC);
							c0 = _mm_add_ps(c0, c8);
							c0 = _mm_mul_ps(c0, _mm_set1_ps(1.0f / 16.0f)); 

							_mm_store_ps((float*)(source0 + 8 * x), c0);
						}

						source0 += pitch;
						source1 += pitch;
						source2 += pitch;
						source3 += pitch;
						source4 += pitch;
						source5 += pitch;
						source6 += pitch;
						source7 += pitch;
						source8 += pitch;
						source9 += pitch;
						sourceA += pitch;
						sourceB += pitch;
						sourceC += pitch;
						sourceD += pitch;
						sourceE += pitch;
						sourceF += pitch;
					}
				}
				else ASSERT(false);
			}
			else
			{
				if(internal.depth == 2)
				{
					for(int y = 0; y < height; y++)
					{
						for(int x = 0; x < 2 * width; x++)
						{
							float c0 = *(float*)(source0 + 4 * x);
							float c1 = *(float*)(source1 + 4 * x);

							c0 = c0 + c1;
							c0 *= 1.0f / 2.0f;

							*(float*)(source0 + 4 * x) = c0;
						}

						source0 += pitch;
						source1 += pitch;
					}
				}
				else if(internal.depth == 4)
				{
					for(int y = 0; y < height; y++)
					{
						for(int x = 0; x < 2 * width; x++)
						{
							float c0 = *(float*)(source0 + 4 * x);
							float c1 = *(float*)(source1 + 4 * x);
							float c2 = *(float*)(source2 + 4 * x);
							float c3 = *(float*)(source3 + 4 * x);

							c0 = c0 + c1;
							c2 = c2 + c3;
							c0 = c0 + c2;
							c0 *= 1.0f / 4.0f;

							*(float*)(source0 + 4 * x) = c0;
						}

						source0 += pitch;
						source1 += pitch;
						source2 += pitch;
						source3 += pitch;
					}
				}
				else if(internal.depth == 8)
				{
					for(int y = 0; y < height; y++)
					{
						for(int x = 0; x < 2 * width; x++)
						{
							float c0 = *(float*)(source0 + 4 * x);
							float c1 = *(float*)(source1 + 4 * x);
							float c2 = *(float*)(source2 + 4 * x);
							float c3 = *(float*)(source3 + 4 * x);
							float c4 = *(float*)(source4 + 4 * x);
							float c5 = *(float*)(source5 + 4 * x);
							float c6 = *(float*)(source6 + 4 * x);
							float c7 = *(float*)(source7 + 4 * x);

							c0 = c0 + c1;
							c2 = c2 + c3;
							c4 = c4 + c5;
							c6 = c6 + c7;
							c0 = c0 + c2;
							c4 = c4 + c6;
							c0 = c0 + c4;
							c0 *= 1.0f / 8.0f;

							*(float*)(source0 + 4 * x) = c0;
						}

						source0 += pitch;
						source1 += pitch;
						source2 += pitch;
						source3 += pitch;
						source4 += pitch;
						source5 += pitch;
						source6 += pitch;
						source7 += pitch;
					}
				}
				else if(internal.depth == 16)
				{
					for(int y = 0; y < height; y++)
					{
						for(int x = 0; x < 2 * width; x++)
						{
							float c0 = *(float*)(source0 + 4 * x);
							float c1 = *(float*)(source1 + 4 * x);
							float c2 = *(float*)(source2 + 4 * x);
							float c3 = *(float*)(source3 + 4 * x);
							float c4 = *(float*)(source4 + 4 * x);
							float c5 = *(float*)(source5 + 4 * x);
							float c6 = *(float*)(source6 + 4 * x);
							float c7 = *(float*)(source7 + 4 * x);
							float c8 = *(float*)(source8 + 4 * x);
							float c9 = *(float*)(source9 + 4 * x);
							float cA = *(float*)(sourceA + 4 * x);
							float cB = *(float*)(sourceB + 4 * x);
							float cC = *(float*)(sourceC + 4 * x);
							float cD = *(float*)(sourceD + 4 * x);
							float cE = *(float*)(sourceE + 4 * x);
							float cF = *(float*)(sourceF + 4 * x);

							c0 = c0 + c1;
							c2 = c2 + c3;
							c4 = c4 + c5;
							c6 = c6 + c7;
							c8 = c8 + c9;
							cA = cA + cB;
							cC = cC + cD;
							cE = cE + cF;
							c0 = c0 + c2;
							c4 = c4 + c6;
							c8 = c8 + cA;
							cC = cC + cE;
							c0 = c0 + c4;
							c8 = c8 + cC;
							c0 = c0 + c8;
							c0 *= 1.0f / 16.0f;

							*(float*)(source0 + 4 * x) = c0;
						}

						source0 += pitch;
						source1 += pitch;
						source2 += pitch;
						source3 += pitch;
						source4 += pitch;
						source5 += pitch;
						source6 += pitch;
						source7 += pitch;
						source8 += pitch;
						source9 += pitch;
						sourceA += pitch;
						sourceB += pitch;
						sourceC += pitch;
						sourceD += pitch;
						sourceE += pitch;
						sourceF += pitch;
					}
				}
				else ASSERT(false);
			}
		}
		else if(internal.format == FORMAT_A32B32G32R32F)
		{
			if(CPUID::supportsSSE())
			{
				if(internal.depth == 2)
				{
					for(int y = 0; y < height; y++)
					{
						for(int x = 0; x < width; x++)
						{
							__m128 c0 = _mm_load_ps((float*)(source0 + 16 * x));
							__m128 c1 = _mm_load_ps((float*)(source1 + 16 * x));
							
							c0 = _mm_add_ps(c0, c1);
							c0 = _mm_mul_ps(c0, _mm_set1_ps(1.0f / 2.0f)); 

							_mm_store_ps((float*)(source0 + 16 * x), c0);
						}

						source0 += pitch;
						source1 += pitch;
					}
				}
				else if(internal.depth == 4)
				{
					for(int y = 0; y < height; y++)
					{
						for(int x = 0; x < width; x++)
						{
							__m128 c0 = _mm_load_ps((float*)(source0 + 16 * x));
							__m128 c1 = _mm_load_ps((float*)(source1 + 16 * x));
							__m128 c2 = _mm_load_ps((float*)(source2 + 16 * x));
							__m128 c3 = _mm_load_ps((float*)(source3 + 16 * x));
							
							c0 = _mm_add_ps(c0, c1);
							c2 = _mm_add_ps(c2, c3);
							c0 = _mm_add_ps(c0, c2);
							c0 = _mm_mul_ps(c0, _mm_set1_ps(1.0f / 4.0f)); 

							_mm_store_ps((float*)(source0 + 16 * x), c0);
						}

						source0 += pitch;
						source1 += pitch;
						source2 += pitch;
						source3 += pitch;
					}
				}
				else if(internal.depth == 8)
				{
					for(int y = 0; y < height; y++)
					{
						for(int x = 0; x < width; x++)
						{
							__m128 c0 = _mm_load_ps((float*)(source0 + 16 * x));
							__m128 c1 = _mm_load_ps((float*)(source1 + 16 * x));
							__m128 c2 = _mm_load_ps((float*)(source2 + 16 * x));
							__m128 c3 = _mm_load_ps((float*)(source3 + 16 * x));
							__m128 c4 = _mm_load_ps((float*)(source4 + 16 * x));
							__m128 c5 = _mm_load_ps((float*)(source5 + 16 * x));
							__m128 c6 = _mm_load_ps((float*)(source6 + 16 * x));
							__m128 c7 = _mm_load_ps((float*)(source7 + 16 * x));
							
							c0 = _mm_add_ps(c0, c1);
							c2 = _mm_add_ps(c2, c3);
							c4 = _mm_add_ps(c4, c5);
							c6 = _mm_add_ps(c6, c7);
							c0 = _mm_add_ps(c0, c2);
							c4 = _mm_add_ps(c4, c6);
							c0 = _mm_add_ps(c0, c4);
							c0 = _mm_mul_ps(c0, _mm_set1_ps(1.0f / 8.0f)); 

							_mm_store_ps((float*)(source0 + 16 * x), c0);
						}

						source0 += pitch;
						source1 += pitch;
						source2 += pitch;
						source3 += pitch;
						source4 += pitch;
						source5 += pitch;
						source6 += pitch;
						source7 += pitch;
					}
				}
				else if(internal.depth == 16)
				{
					for(int y = 0; y < height; y++)
					{
						for(int x = 0; x < width; x++)
						{
							__m128 c0 = _mm_load_ps((float*)(source0 + 16 * x));
							__m128 c1 = _mm_load_ps((float*)(source1 + 16 * x));
							__m128 c2 = _mm_load_ps((float*)(source2 + 16 * x));
							__m128 c3 = _mm_load_ps((float*)(source3 + 16 * x));
							__m128 c4 = _mm_load_ps((float*)(source4 + 16 * x));
							__m128 c5 = _mm_load_ps((float*)(source5 + 16 * x));
							__m128 c6 = _mm_load_ps((float*)(source6 + 16 * x));
							__m128 c7 = _mm_load_ps((float*)(source7 + 16 * x));
							__m128 c8 = _mm_load_ps((float*)(source8 + 16 * x));
							__m128 c9 = _mm_load_ps((float*)(source9 + 16 * x));
							__m128 cA = _mm_load_ps((float*)(sourceA + 16 * x));
							__m128 cB = _mm_load_ps((float*)(sourceB + 16 * x));
							__m128 cC = _mm_load_ps((float*)(sourceC + 16 * x));
							__m128 cD = _mm_load_ps((float*)(sourceD + 16 * x));
							__m128 cE = _mm_load_ps((float*)(sourceE + 16 * x));
							__m128 cF = _mm_load_ps((float*)(sourceF + 16 * x));

							c0 = _mm_add_ps(c0, c1);
							c2 = _mm_add_ps(c2, c3);
							c4 = _mm_add_ps(c4, c5);
							c6 = _mm_add_ps(c6, c7);
							c8 = _mm_add_ps(c8, c9);
							cA = _mm_add_ps(cA, cB);
							cC = _mm_add_ps(cC, cD);
							cE = _mm_add_ps(cE, cF);
							c0 = _mm_add_ps(c0, c2);
							c4 = _mm_add_ps(c4, c6);
							c8 = _mm_add_ps(c8, cA);
							cC = _mm_add_ps(cC, cE);
							c0 = _mm_add_ps(c0, c4);
							c8 = _mm_add_ps(c8, cC);
							c0 = _mm_add_ps(c0, c8);
							c0 = _mm_mul_ps(c0, _mm_set1_ps(1.0f / 16.0f)); 

							_mm_store_ps((float*)(source0 + 16 * x), c0);
						}

						source0 += pitch;
						source1 += pitch;
						source2 += pitch;
						source3 += pitch;
						source4 += pitch;
						source5 += pitch;
						source6 += pitch;
						source7 += pitch;
						source8 += pitch;
						source9 += pitch;
						sourceA += pitch;
						sourceB += pitch;
						sourceC += pitch;
						sourceD += pitch;
						sourceE += pitch;
						sourceF += pitch;
					}
				}
				else ASSERT(false);
			}
			else
			{
				if(internal.depth == 2)
				{
					for(int y = 0; y < height; y++)
					{
						for(int x = 0; x < 4 * width; x++)
						{
							float c0 = *(float*)(source0 + 4 * x);
							float c1 = *(float*)(source1 + 4 * x);

							c0 = c0 + c1;
							c0 *= 1.0f / 2.0f;

							*(float*)(source0 + 4 * x) = c0;
						}

						source0 += pitch;
						source1 += pitch;
					}
				}
				else if(internal.depth == 4)
				{
					for(int y = 0; y < height; y++)
					{
						for(int x = 0; x < 4 * width; x++)
						{
							float c0 = *(float*)(source0 + 4 * x);
							float c1 = *(float*)(source1 + 4 * x);
							float c2 = *(float*)(source2 + 4 * x);
							float c3 = *(float*)(source3 + 4 * x);

							c0 = c0 + c1;
							c2 = c2 + c3;
							c0 = c0 + c2;
							c0 *= 1.0f / 4.0f;

							*(float*)(source0 + 4 * x) = c0;
						}

						source0 += pitch;
						source1 += pitch;
						source2 += pitch;
						source3 += pitch;
					}
				}
				else if(internal.depth == 8)
				{
					for(int y = 0; y < height; y++)
					{
						for(int x = 0; x < 4 * width; x++)
						{
							float c0 = *(float*)(source0 + 4 * x);
							float c1 = *(float*)(source1 + 4 * x);
							float c2 = *(float*)(source2 + 4 * x);
							float c3 = *(float*)(source3 + 4 * x);
							float c4 = *(float*)(source4 + 4 * x);
							float c5 = *(float*)(source5 + 4 * x);
							float c6 = *(float*)(source6 + 4 * x);
							float c7 = *(float*)(source7 + 4 * x);

							c0 = c0 + c1;
							c2 = c2 + c3;
							c4 = c4 + c5;
							c6 = c6 + c7;
							c0 = c0 + c2;
							c4 = c4 + c6;
							c0 = c0 + c4;
							c0 *= 1.0f / 8.0f;

							*(float*)(source0 + 4 * x) = c0;
						}

						source0 += pitch;
						source1 += pitch;
						source2 += pitch;
						source3 += pitch;
						source4 += pitch;
						source5 += pitch;
						source6 += pitch;
						source7 += pitch;
					}
				}
				else if(internal.depth == 16)
				{
					for(int y = 0; y < height; y++)
					{
						for(int x = 0; x < 4 * width; x++)
						{
							float c0 = *(float*)(source0 + 4 * x);
							float c1 = *(float*)(source1 + 4 * x);
							float c2 = *(float*)(source2 + 4 * x);
							float c3 = *(float*)(source3 + 4 * x);
							float c4 = *(float*)(source4 + 4 * x);
							float c5 = *(float*)(source5 + 4 * x);
							float c6 = *(float*)(source6 + 4 * x);
							float c7 = *(float*)(source7 + 4 * x);
							float c8 = *(float*)(source8 + 4 * x);
							float c9 = *(float*)(source9 + 4 * x);
							float cA = *(float*)(sourceA + 4 * x);
							float cB = *(float*)(sourceB + 4 * x);
							float cC = *(float*)(sourceC + 4 * x);
							float cD = *(float*)(sourceD + 4 * x);
							float cE = *(float*)(sourceE + 4 * x);
							float cF = *(float*)(sourceF + 4 * x);

							c0 = c0 + c1;
							c2 = c2 + c3;
							c4 = c4 + c5;
							c6 = c6 + c7;
							c8 = c8 + c9;
							cA = cA + cB;
							cC = cC + cD;
							cE = cE + cF;
							c0 = c0 + c2;
							c4 = c4 + c6;
							c8 = c8 + cA;
							cC = cC + cE;
							c0 = c0 + c4;
							c8 = c8 + cC;
							c0 = c0 + c8;
							c0 *= 1.0f / 16.0f;

							*(float*)(source0 + 4 * x) = c0;
						}

						source0 += pitch;
						source1 += pitch;
						source2 += pitch;
						source3 += pitch;
						source4 += pitch;
						source5 += pitch;
						source6 += pitch;
						source7 += pitch;
						source8 += pitch;
						source9 += pitch;
						sourceA += pitch;
						sourceB += pitch;
						sourceC += pitch;
						sourceD += pitch;
						sourceE += pitch;
						sourceF += pitch;
					}
				}
				else ASSERT(false);
			}
		}
		else
		{
		//	UNIMPLEMENTED();
		}
	}
}
