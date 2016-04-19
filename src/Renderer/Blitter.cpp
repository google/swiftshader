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

#include "Blitter.hpp"

#include "Common/Debug.hpp"
#include "Reactor/Reactor.hpp"

namespace sw
{
	Blitter blitter;

	Blitter::Blitter()
	{
		blitCache = new RoutineCache<BlitState>(1024);
	}

	Blitter::~Blitter()
	{
		delete blitCache;
	}

	void Blitter::clear(void* pixel, sw::Format format, Surface *dest, const SliceRect &dRect, unsigned int rgbaMask)
	{
		sw::Surface color(1, 1, 1, format, pixel, sw::Surface::bytes(format), sw::Surface::bytes(format));
		Blitter::Options clearOptions = static_cast<sw::Blitter::Options>((rgbaMask & 0xF) | CLEAR_OPERATION);
		SliceRect sRect(dRect);
		sRect.slice = 0;
		blit(&color, sRect, dest, dRect, clearOptions);
	}

	void Blitter::blit(Surface *source, const SliceRect &sRect, Surface *dest, const SliceRect &dRect, bool filter)
	{
		Blitter::Options options = filter ? static_cast<Blitter::Options>(WRITE_RGBA | FILTER_LINEAR) : WRITE_RGBA;
		blit(source, sRect, dest, dRect, options);
	}

	void Blitter::blit(Surface *source, const SliceRect &sourceRect, Surface *dest, const SliceRect &destRect, const Blitter::Options& options)
	{
		if(dest->getInternalFormat() == FORMAT_NULL)
		{
			return;
		}

		if(blitReactor(source, sourceRect, dest, destRect, options))
		{
			return;
		}

		SliceRect sRect = sourceRect;
		SliceRect dRect = destRect;

		bool flipX = destRect.x0 > destRect.x1;
		bool flipY = destRect.y0 > destRect.y1;

		if(flipX)
		{
			swap(dRect.x0, dRect.x1);
			swap(sRect.x0, sRect.x1);
		}
		if(flipY)
		{
			swap(dRect.y0, dRect.y1);
			swap(sRect.y0, sRect.y1);
		}

		source->lockInternal(sRect.x0, sRect.y0, sRect.slice, sw::LOCK_READONLY, sw::PUBLIC);
		dest->lockInternal(dRect.x0, dRect.y0, dRect.slice, sw::LOCK_WRITEONLY, sw::PUBLIC);

		float w = static_cast<float>(sRect.x1 - sRect.x0) / static_cast<float>(dRect.x1 - dRect.x0);
		float h = static_cast<float>(sRect.y1 - sRect.y0) / static_cast<float>(dRect.y1 - dRect.y0);

		const float xStart = (float)sRect.x0 + 0.5f * w;
		float y = (float)sRect.y0 + 0.5f * h;

		for(int j = dRect.y0; j < dRect.y1; j++)
		{
			float x = xStart;

			for(int i = dRect.x0; i < dRect.x1; i++)
			{
				// FIXME: Support RGBA mask
				dest->copyInternal(source, i, j, x, y, (options & FILTER_LINEAR) == FILTER_LINEAR);

				x += w;
			}

			y += h;
		}

		source->unlockInternal();
		dest->unlockInternal();
	}

	void Blitter::blit3D(Surface *source, Surface *dest)
	{
		source->lockInternal(0, 0, 0, sw::LOCK_READONLY, sw::PUBLIC);
		dest->lockInternal(0, 0, 0, sw::LOCK_WRITEONLY, sw::PUBLIC);

		float w = static_cast<float>(source->getWidth())  / static_cast<float>(dest->getWidth());
		float h = static_cast<float>(source->getHeight()) / static_cast<float>(dest->getHeight());
		float d = static_cast<float>(source->getDepth())  / static_cast<float>(dest->getDepth());

		float z = 0.5f * d;
		for(int k = 0; k < dest->getDepth(); ++k)
		{
			float y = 0.5f * h;
			for(int j = 0; j < dest->getHeight(); ++j)
			{
				float x = 0.5f * w;
				for(int i = 0; i < dest->getWidth(); ++i)
				{
					dest->copyInternal(source, i, j, k, x, y, z, true);
					x += w;
				}
				y += h;
			}
			z += d;
		}

		source->unlockInternal();
		dest->unlockInternal();
	}

	bool Blitter::read(Float4 &c, Pointer<Byte> element, Format format)
	{
		c = Float4(0.0f, 0.0f, 0.0f, 1.0f);

		switch(format)
		{
		case FORMAT_L8:
			c.xyz = Float(Int(*Pointer<Byte>(element)));
			c.w = float(0xFF);
			break;
		case FORMAT_A8:
			c.w = Float(Int(*Pointer<Byte>(element)));
			break;
		case FORMAT_R8I:
		case FORMAT_R8I_SNORM:
			c.x = Float(Int(*Pointer<SByte>(element)));
			c.w = float(0x7F);
			break;
		case FORMAT_R8:
		case FORMAT_R8UI:
			c.x = Float(Int(*Pointer<Byte>(element)));
			c.w = float(0xFF);
			break;
		case FORMAT_R16I:
			c.x = Float(Int(*Pointer<Short>(element)));
			c.w = float(0x7FFF);
			break;
		case FORMAT_R16UI:
			c.x = Float(Int(*Pointer<UShort>(element)));
			c.w = float(0xFFFF);
			break;
		case FORMAT_R32I:
			c.x = Float(Int(*Pointer<Int>(element)));
			c.w = float(0x7FFFFFFF);
			break;
		case FORMAT_R32UI:
			c.x = Float(Int(*Pointer<UInt>(element)));
			c.w = float(0xFFFFFFFF);
			break;
		case FORMAT_A8R8G8B8:
			c = Float4(*Pointer<Byte4>(element)).zyxw;
			break;
		case FORMAT_A8B8G8R8I:
		case FORMAT_A8B8G8R8I_SNORM:
			c = Float4(*Pointer<SByte4>(element));
			break;
		case FORMAT_A8B8G8R8:
		case FORMAT_A8B8G8R8UI:
			c = Float4(*Pointer<Byte4>(element));
			break;
		case FORMAT_X8R8G8B8:
			c = Float4(*Pointer<Byte4>(element)).zyxw;
			c.w = float(0xFF);
			break;
		case FORMAT_R8G8B8:
			c.z = Float(Int(*Pointer<Byte>(element + 0)));
			c.y = Float(Int(*Pointer<Byte>(element + 1)));
			c.x = Float(Int(*Pointer<Byte>(element + 2)));
			c.w = float(0xFF);
			break;
		case FORMAT_B8G8R8:
			c.x = Float(Int(*Pointer<Byte>(element + 0)));
			c.y = Float(Int(*Pointer<Byte>(element + 1)));
			c.z = Float(Int(*Pointer<Byte>(element + 2)));
			c.w = float(0xFF);
			break;
		case FORMAT_X8B8G8R8I:
		case FORMAT_X8B8G8R8I_SNORM:
			c = Float4(*Pointer<SByte4>(element));
			c.w = float(0x7F);
			break;
		case FORMAT_X8B8G8R8:
		case FORMAT_X8B8G8R8UI:
			c = Float4(*Pointer<Byte4>(element));
			c.w = float(0xFF);
			break;
		case FORMAT_A16B16G16R16I:
			c = Float4(*Pointer<Short4>(element));
			break;
		case FORMAT_A16B16G16R16:
		case FORMAT_A16B16G16R16UI:
			c = Float4(*Pointer<UShort4>(element));
			break;
		case FORMAT_X16B16G16R16I:
			c = Float4(*Pointer<Short4>(element));
			c.w = float(0x7FFF);
			break;
		case FORMAT_X16B16G16R16UI:
			c = Float4(*Pointer<UShort4>(element));
			c.w = float(0xFFFF);
			break;
		case FORMAT_A32B32G32R32I:
			c = Float4(*Pointer<Int4>(element));
			break;
		case FORMAT_A32B32G32R32UI:
			c = Float4(*Pointer<UInt4>(element));
			break;
		case FORMAT_X32B32G32R32I:
			c = Float4(*Pointer<Int4>(element));
			c.w = float(0x7FFFFFFF);
			break;
		case FORMAT_X32B32G32R32UI:
			c = Float4(*Pointer<UInt4>(element));
			c.w = float(0xFFFFFFFF);
			break;
		case FORMAT_G8R8I:
		case FORMAT_G8R8I_SNORM:
			c.x = Float(Int(*Pointer<SByte>(element + 0)));
			c.y = Float(Int(*Pointer<SByte>(element + 1)));
			c.w = float(0x7F);
			break;
		case FORMAT_G8R8:
		case FORMAT_G8R8UI:
			c.x = Float(Int(*Pointer<Byte>(element + 0)));
			c.y = Float(Int(*Pointer<Byte>(element + 1)));
			c.w = float(0xFF);
			break;
		case FORMAT_G16R16I:
			c.x = Float(Int(*Pointer<Short>(element + 0)));
			c.y = Float(Int(*Pointer<Short>(element + 2)));
			c.w = float(0x7FFF);
			break;
		case FORMAT_G16R16:
		case FORMAT_G16R16UI:
			c.x = Float(Int(*Pointer<UShort>(element + 0)));
			c.y = Float(Int(*Pointer<UShort>(element + 2)));
			c.w = float(0xFFFF);
			break;
		case FORMAT_G32R32I:
			c.x = Float(Int(*Pointer<Int>(element + 0)));
			c.y = Float(Int(*Pointer<Int>(element + 4)));
			c.w = float(0x7FFFFFFF);
			break;
		case FORMAT_G32R32UI:
			c.x = Float(Int(*Pointer<UInt>(element + 0)));
			c.y = Float(Int(*Pointer<UInt>(element + 4)));
			c.w = float(0xFFFFFFFF);
			break;
		case FORMAT_A32B32G32R32F:
			c = *Pointer<Float4>(element);
			break;
		case FORMAT_X32B32G32R32F:
		case FORMAT_B32G32R32F:
			c.z = *Pointer<Float>(element + 8);
		case FORMAT_G32R32F:
			c.x = *Pointer<Float>(element + 0);
			c.y = *Pointer<Float>(element + 4);
			break;
		case FORMAT_R32F:
			c.x = *Pointer<Float>(element);
			break;
		case FORMAT_R5G6B5:
			c.x = Float(Int((*Pointer<UShort>(element) & UShort(0xF800)) >> UShort(11)));
			c.y = Float(Int((*Pointer<UShort>(element) & UShort(0x07E0)) >> UShort(5)));
			c.z = Float(Int(*Pointer<UShort>(element) & UShort(0x001F)));
			break;
		case FORMAT_A2B10G10R10:
			c.x = Float(Int((*Pointer<UInt>(element) & UInt(0x000003FF))));
			c.y = Float(Int((*Pointer<UInt>(element) & UInt(0x000FFC00)) >> 10));
			c.z = Float(Int((*Pointer<UInt>(element) & UInt(0x3FF00000)) >> 20));
			c.w = Float(Int((*Pointer<UInt>(element) & UInt(0xC0000000)) >> 30));
			break;
		case FORMAT_D16:
			c.x = Float(Int((*Pointer<UShort>(element))));
			break;
		case FORMAT_D24S8:
			c.x = Float(Int((*Pointer<UInt>(element))));
			break;
		case FORMAT_D32:
			c.x = Float(Int((*Pointer<UInt>(element))));
			break;
		case FORMAT_D32F:
			c.x = *Pointer<Float>(element);
			break;
		case FORMAT_D32F_COMPLEMENTARY:
			c.x = 1.0f - *Pointer<Float>(element);
			break;
		case FORMAT_D32F_LOCKABLE:
			c.x = *Pointer<Float>(element);
			break;
		case FORMAT_D32FS8_TEXTURE:
			c.x = *Pointer<Float>(element);
			break;
		case FORMAT_D32FS8_SHADOW:
			c.x = *Pointer<Float>(element);
			break;
		default:
			return false;
		}

		return true;
	}

	bool Blitter::write(Float4 &c, Pointer<Byte> element, Format format, const Blitter::Options& options)
	{
		bool writeR = (options & WRITE_RED) == WRITE_RED;
		bool writeG = (options & WRITE_GREEN) == WRITE_GREEN;
		bool writeB = (options & WRITE_BLUE) == WRITE_BLUE;
		bool writeA = (options & WRITE_ALPHA) == WRITE_ALPHA;
		bool writeRGBA = writeR && writeG && writeB && writeA;

		switch(format)
		{
		case FORMAT_L8:
			*Pointer<Byte>(element) = Byte(RoundInt(Float(c.x)));
			break;
		case FORMAT_A8:
			if(writeA) { *Pointer<Byte>(element) = Byte(RoundInt(Float(c.w))); }
			break;
		case FORMAT_A8R8G8B8:
			if(writeRGBA)
			{
				UShort4 c0 = As<UShort4>(RoundShort4(c.zyxw));
				Byte8 c1 = Pack(c0, c0);
				*Pointer<UInt>(element) = UInt(As<Long>(c1));
			}
			else
			{
				if(writeB) { *Pointer<Byte>(element + 0) = Byte(RoundInt(Float(c.z))); }
				if(writeG) { *Pointer<Byte>(element + 1) = Byte(RoundInt(Float(c.y))); }
				if(writeR) { *Pointer<Byte>(element + 2) = Byte(RoundInt(Float(c.x))); }
				if(writeA) { *Pointer<Byte>(element + 3) = Byte(RoundInt(Float(c.w))); }
			}
			break;
		case FORMAT_A8B8G8R8:
			if(writeRGBA)
			{
				UShort4 c0 = As<UShort4>(RoundShort4(c));
				Byte8 c1 = Pack(c0, c0);
				*Pointer<UInt>(element) = UInt(As<Long>(c1));
			}
			else
			{
				if(writeR) { *Pointer<Byte>(element + 0) = Byte(RoundInt(Float(c.x))); }
				if(writeG) { *Pointer<Byte>(element + 1) = Byte(RoundInt(Float(c.y))); }
				if(writeB) { *Pointer<Byte>(element + 2) = Byte(RoundInt(Float(c.z))); }
				if(writeA) { *Pointer<Byte>(element + 3) = Byte(RoundInt(Float(c.w))); }
			}
			break;
		case FORMAT_X8R8G8B8:
			if(writeRGBA)
			{
				UShort4 c0 = As<UShort4>(RoundShort4(c.zyxw));
				Byte8 c1 = Pack(c0, c0);
				*Pointer<UInt>(element) = UInt(As<Long>(c1)) | 0xFF000000;
			}
			else
			{
				if(writeB) { *Pointer<Byte>(element + 0) = Byte(RoundInt(Float(c.z))); }
				if(writeG) { *Pointer<Byte>(element + 1) = Byte(RoundInt(Float(c.y))); }
				if(writeR) { *Pointer<Byte>(element + 2) = Byte(RoundInt(Float(c.x))); }
				if(writeA) { *Pointer<Byte>(element + 3) = Byte(0xFF); }
			}
			break;
		case FORMAT_X8B8G8R8:
			if(writeRGBA)
			{
				UShort4 c0 = As<UShort4>(RoundShort4(c));
				Byte8 c1 = Pack(c0, c0);
				*Pointer<UInt>(element) = UInt(As<Long>(c1)) | 0xFF000000;
			}
			else
			{
				if(writeR) { *Pointer<Byte>(element + 0) = Byte(RoundInt(Float(c.x))); }
				if(writeG) { *Pointer<Byte>(element + 1) = Byte(RoundInt(Float(c.y))); }
				if(writeB) { *Pointer<Byte>(element + 2) = Byte(RoundInt(Float(c.z))); }
				if(writeA) { *Pointer<Byte>(element + 3) = Byte(0xFF); }
			}
			break;
		case FORMAT_R8G8B8:
			if(writeR) { *Pointer<Byte>(element + 2) = Byte(RoundInt(Float(c.x))); }
			if(writeG) { *Pointer<Byte>(element + 1) = Byte(RoundInt(Float(c.y))); }
			if(writeB) { *Pointer<Byte>(element + 0) = Byte(RoundInt(Float(c.z))); }
			break;
		case FORMAT_B8G8R8:
			if(writeR) { *Pointer<Byte>(element + 0) = Byte(RoundInt(Float(c.x))); }
			if(writeG) { *Pointer<Byte>(element + 1) = Byte(RoundInt(Float(c.y))); }
			if(writeB) { *Pointer<Byte>(element + 2) = Byte(RoundInt(Float(c.z))); }
			break;
		case FORMAT_A32B32G32R32F:
			if(writeRGBA)
			{
				*Pointer<Float4>(element) = c;
			}
			else
			{
				if(writeR) { *Pointer<Float>(element) = c.x; }
				if(writeG) { *Pointer<Float>(element + 4) = c.y; }
				if(writeB) { *Pointer<Float>(element + 8) = c.z; }
				if(writeA) { *Pointer<Float>(element + 12) = c.w; }
			}
			break;
		case FORMAT_X32B32G32R32F:
			if(writeA) { *Pointer<Float>(element + 12) = 1.0f; }
		case FORMAT_B32G32R32F:
			if(writeR) { *Pointer<Float>(element) = c.x; }
			if(writeG) { *Pointer<Float>(element + 4) = c.y; }
			if(writeB) { *Pointer<Float>(element + 8) = c.z; }
			break;
		case FORMAT_G32R32F:
			if(writeR && writeG)
			{
				*Pointer<Float2>(element) = Float2(c);
			}
			else
			{
				if(writeR) { *Pointer<Float>(element) = c.x; }
				if(writeG) { *Pointer<Float>(element + 4) = c.y; }
			}
			break;
		case FORMAT_R32F:
			if(writeR) { *Pointer<Float>(element) = c.x; }
			break;
		case FORMAT_A8B8G8R8I:
		case FORMAT_A8B8G8R8I_SNORM:
			if(writeA) { *Pointer<SByte>(element + 3) = SByte(RoundInt(Float(c.w))); }
		case FORMAT_X8B8G8R8I:
		case FORMAT_X8B8G8R8I_SNORM:
			if(writeA && (format == FORMAT_X8B8G8R8I || format == FORMAT_X8B8G8R8I_SNORM))
			{
				*Pointer<SByte>(element + 3) = SByte(0x7F);
			}
			if(writeB) { *Pointer<SByte>(element + 2) = SByte(RoundInt(Float(c.z))); }
		case FORMAT_G8R8I:
		case FORMAT_G8R8I_SNORM:
			if(writeG) { *Pointer<SByte>(element + 1) = SByte(RoundInt(Float(c.y))); }
		case FORMAT_R8I:
		case FORMAT_R8I_SNORM:
			if(writeR) { *Pointer<SByte>(element) = SByte(RoundInt(Float(c.x))); }
			break;
		case FORMAT_A8B8G8R8UI:
			if(writeA) { *Pointer<Byte>(element + 3) = Byte(RoundInt(Float(c.w))); }
		case FORMAT_X8B8G8R8UI:
			if(writeA && (format == FORMAT_X8B8G8R8UI))
			{
				*Pointer<Byte>(element + 3) = Byte(0xFF);
			}
			if(writeB) { *Pointer<Byte>(element + 2) = Byte(RoundInt(Float(c.z))); }
		case FORMAT_G8R8UI:
		case FORMAT_G8R8:
			if(writeG) { *Pointer<Byte>(element + 1) = Byte(RoundInt(Float(c.y))); }
		case FORMAT_R8UI:
		case FORMAT_R8:
			if(writeR) { *Pointer<Byte>(element) = Byte(RoundInt(Float(c.x))); }
			break;
		case FORMAT_A16B16G16R16I:
			if(writeRGBA)
			{
				*Pointer<Short4>(element) = Short4(RoundInt(c));
			}
			else
			{
				if(writeR) { *Pointer<Short>(element) = Short(RoundInt(Float(c.x))); }
				if(writeG) { *Pointer<Short>(element + 2) = Short(RoundInt(Float(c.y))); }
				if(writeB) { *Pointer<Short>(element + 4) = Short(RoundInt(Float(c.z))); }
				if(writeA) { *Pointer<Short>(element + 6) = Short(RoundInt(Float(c.w))); }
			}
			break;
		case FORMAT_X16B16G16R16I:
			if(writeRGBA)
			{
				*Pointer<Short4>(element) = Short4(RoundInt(c));
			}
			else
			{
				if(writeR) { *Pointer<Short>(element) = Short(RoundInt(Float(c.x))); }
				if(writeG) { *Pointer<Short>(element + 2) = Short(RoundInt(Float(c.y))); }
				if(writeB) { *Pointer<Short>(element + 4) = Short(RoundInt(Float(c.z))); }
			}
			if(writeA) { *Pointer<Short>(element + 6) = Short(0x7F); }
			break;
		case FORMAT_G16R16I:
			if(writeR && writeG)
			{
				*Pointer<UInt>(element) = UInt(As<Long>(Short4(RoundInt(c))));
			}
			else
			{
				if(writeR) { *Pointer<Short>(element) = Short(RoundInt(Float(c.x))); }
				if(writeG) { *Pointer<Short>(element + 2) = Short(RoundInt(Float(c.y))); }
			}
			break;
		case FORMAT_R16I:
			if(writeR) { *Pointer<Short>(element) = Short(RoundInt(Float(c.x))); }
			break;
		case FORMAT_A16B16G16R16UI:
		case FORMAT_A16B16G16R16:
			if(writeRGBA)
			{
				*Pointer<UShort4>(element) = UShort4(RoundInt(c));
			}
			else
			{
				if(writeR) { *Pointer<UShort>(element) = UShort(RoundInt(Float(c.x))); }
				if(writeG) { *Pointer<UShort>(element + 2) = UShort(RoundInt(Float(c.y))); }
				if(writeB) { *Pointer<UShort>(element + 4) = UShort(RoundInt(Float(c.z))); }
				if(writeA) { *Pointer<UShort>(element + 6) = UShort(RoundInt(Float(c.w))); }
			}
			break;
		case FORMAT_X16B16G16R16UI:
			if(writeRGBA)
			{
				*Pointer<UShort4>(element) = UShort4(RoundInt(c));
			}
			else
			{
				if(writeR) { *Pointer<UShort>(element) = UShort(RoundInt(Float(c.x))); }
				if(writeG) { *Pointer<UShort>(element + 2) = UShort(RoundInt(Float(c.y))); }
				if(writeB) { *Pointer<UShort>(element + 4) = UShort(RoundInt(Float(c.z))); }
			}
			if(writeA) { *Pointer<UShort>(element + 6) = UShort(0xFF); }
			break;
		case FORMAT_G16R16UI:
		case FORMAT_G16R16:
			if(writeR && writeG)
			{
				*Pointer<UInt>(element) = UInt(As<Long>(UShort4(RoundInt(c))));
			}
			else
			{
				if(writeR) { *Pointer<UShort>(element) = UShort(RoundInt(Float(c.x))); }
				if(writeG) { *Pointer<UShort>(element + 2) = UShort(RoundInt(Float(c.y))); }
			}
			break;
		case FORMAT_R16UI:
			if(writeR) { *Pointer<UShort>(element) = UShort(RoundInt(Float(c.x))); }
			break;
		case FORMAT_A32B32G32R32I:
			if(writeRGBA)
			{
				*Pointer<Int4>(element) = RoundInt(c);
			}
			else
			{
				if(writeR) { *Pointer<Int>(element) = RoundInt(Float(c.x)); }
				if(writeG) { *Pointer<Int>(element + 4) = RoundInt(Float(c.y)); }
				if(writeB) { *Pointer<Int>(element + 8) = RoundInt(Float(c.z)); }
				if(writeA) { *Pointer<Int>(element + 12) = RoundInt(Float(c.w)); }
			}
			break;
		case FORMAT_X32B32G32R32I:
			if(writeRGBA)
			{
				*Pointer<Int4>(element) = RoundInt(c);
			}
			else
			{
				if(writeR) { *Pointer<Int>(element) = RoundInt(Float(c.x)); }
				if(writeG) { *Pointer<Int>(element + 4) = RoundInt(Float(c.y)); }
				if(writeB) { *Pointer<Int>(element + 8) = RoundInt(Float(c.z)); }
			}
			if(writeA) { *Pointer<Int>(element + 12) = Int(0x7FFFFFFF); }
			break;
		case FORMAT_G32R32I:
			if(writeG) { *Pointer<Int>(element + 4) = RoundInt(Float(c.y)); }
		case FORMAT_R32I:
			if(writeR) { *Pointer<Int>(element) = RoundInt(Float(c.x)); }
			break;
		case FORMAT_A32B32G32R32UI:
			if(writeRGBA)
			{
				*Pointer<UInt4>(element) = UInt4(RoundInt(c));
			}
			else
			{
				if(writeR) { *Pointer<UInt>(element) = As<UInt>(RoundInt(Float(c.x))); }
				if(writeG) { *Pointer<UInt>(element + 4) = As<UInt>(RoundInt(Float(c.y))); }
				if(writeB) { *Pointer<UInt>(element + 8) = As<UInt>(RoundInt(Float(c.z))); }
				if(writeA) { *Pointer<UInt>(element + 12) = As<UInt>(RoundInt(Float(c.w))); }
			}
			break;
		case FORMAT_X32B32G32R32UI:
			if(writeRGBA)
			{
				*Pointer<UInt4>(element) = UInt4(RoundInt(c));
			}
			else
			{
				if(writeR) { *Pointer<UInt>(element) = As<UInt>(RoundInt(Float(c.x))); }
				if(writeG) { *Pointer<UInt>(element + 4) = As<UInt>(RoundInt(Float(c.y))); }
				if(writeB) { *Pointer<UInt>(element + 8) = As<UInt>(RoundInt(Float(c.z))); }
			}
			if(writeA) { *Pointer<UInt4>(element + 12) = UInt4(0xFFFFFFFF); }
			break;
		case FORMAT_G32R32UI:
			if(writeG) { *Pointer<UInt>(element + 4) = As<UInt>(RoundInt(Float(c.y))); }
		case FORMAT_R32UI:
			if(writeR) { *Pointer<UInt>(element) = As<UInt>(RoundInt(Float(c.x))); }
			break;
		case FORMAT_R5G6B5:
			if(writeR && writeG && writeB)
			{
				*Pointer<UShort>(element) = UShort(RoundInt(Float(c.z)) |
				                                  (RoundInt(Float(c.y)) << Int(5)) |
				                                  (RoundInt(Float(c.x)) << Int(11)));
			}
			else
			{
				unsigned short mask = (writeB ? 0x001F : 0x0000) | (writeG ? 0x07E0 : 0x0000) | (writeR ? 0xF800 : 0x0000);
				unsigned short unmask = ~mask;
				*Pointer<UShort>(element) = (*Pointer<UShort>(element) & UShort(unmask)) |
				                            (UShort(RoundInt(Float(c.z)) |
				                                   (RoundInt(Float(c.y)) << Int(5)) |
				                                   (RoundInt(Float(c.x)) << Int(11))) & UShort(mask));
			}
			break;
		case FORMAT_A2B10G10R10:
			if(writeRGBA)
			{
				*Pointer<UInt>(element) = UInt(RoundInt(Float(c.x)) |
				                              (RoundInt(Float(c.y)) << 10) |
				                              (RoundInt(Float(c.z)) << 20) |
				                              (RoundInt(Float(c.w)) << 30));
			}
			else
			{
				unsigned int mask = (writeA ? 0xC0000000 : 0x0000) |
				                    (writeB ? 0x3FF00000 : 0x0000) |
				                    (writeG ? 0x000FFC00 : 0x0000) |
				                    (writeR ? 0x000003FF : 0x0000);
				unsigned int unmask = ~mask;
				*Pointer<UInt>(element) = (*Pointer<UInt>(element) & UInt(unmask)) |
				                            (UInt(RoundInt(Float(c.x)) |
				                                  (RoundInt(Float(c.y)) << 10) |
				                                  (RoundInt(Float(c.z)) << 20) |
				                                  (RoundInt(Float(c.w)) << 30)) & UInt(mask));
			}
			break;
		case FORMAT_D16:
			*Pointer<UShort>(element) = UShort(RoundInt(Float(c.x)));
			break;
		case FORMAT_D24S8:
			*Pointer<UInt>(element) = UInt(RoundInt(Float(c.x)));
			break;
		case FORMAT_D32:
			*Pointer<UInt>(element) = UInt(RoundInt(Float(c.x)));
			break;
		case FORMAT_D32F:
			*Pointer<Float>(element) = c.x;
			break;
		case FORMAT_D32F_COMPLEMENTARY:
			*Pointer<Float>(element) = 1.0f - c.x;
			break;
		case FORMAT_D32F_LOCKABLE:
			*Pointer<Float>(element) = c.x;
			break;
		case FORMAT_D32FS8_TEXTURE:
			*Pointer<Float>(element) = c.x;
			break;
		case FORMAT_D32FS8_SHADOW:
			*Pointer<Float>(element) = c.x;
			break;
		default:
			return false;
		}
		return true;
	}

	bool Blitter::read(Int4 &c, Pointer<Byte> element, Format format)
	{
		c = Int4(0, 0, 0, 0xFFFFFFFF);

		switch(format)
		{
		case FORMAT_A8B8G8R8I:
			c = Insert(c, Int(*Pointer<SByte>(element + 3)), 3);
		case FORMAT_X8B8G8R8I:
			c = Insert(c, Int(*Pointer<SByte>(element + 2)), 2);
		case FORMAT_G8R8I:
			c = Insert(c, Int(*Pointer<SByte>(element + 1)), 1);
		case FORMAT_R8I:
			c = Insert(c, Int(*Pointer<SByte>(element)), 0);
			if(format != FORMAT_A8B8G8R8I)
			{
				c = Insert(c, Int(0x7F), 3); // Set alpha
			}
			break;
		case FORMAT_A8B8G8R8UI:
			c = Insert(c, Int(*Pointer<Byte>(element + 3)), 3);
		case FORMAT_X8B8G8R8UI:
			c = Insert(c, Int(*Pointer<Byte>(element + 2)), 2);
		case FORMAT_G8R8UI:
			c = Insert(c, Int(*Pointer<Byte>(element + 1)), 1);
		case FORMAT_R8UI:
			c = Insert(c, Int(*Pointer<Byte>(element)), 0);
			if(format != FORMAT_A8B8G8R8UI)
			{
				c = Insert(c, Int(0xFF), 3); // Set alpha
			}
			break;
		case FORMAT_A16B16G16R16I:
			c = Insert(c, Int(*Pointer<Short>(element + 6)), 3);
		case FORMAT_X16B16G16R16I:
			c = Insert(c, Int(*Pointer<Short>(element + 4)), 2);
		case FORMAT_G16R16I:
			c = Insert(c, Int(*Pointer<Short>(element + 2)), 1);
		case FORMAT_R16I:
			c = Insert(c, Int(*Pointer<Short>(element)), 0);
			if(format != FORMAT_A16B16G16R16I)
			{
				c = Insert(c, Int(0x7FFF), 3); // Set alpha
			}
			break;
		case FORMAT_A16B16G16R16UI:
			c = Insert(c, Int(*Pointer<UShort>(element + 6)), 3);
		case FORMAT_X16B16G16R16UI:
			c = Insert(c, Int(*Pointer<UShort>(element + 4)), 2);
		case FORMAT_G16R16UI:
			c = Insert(c, Int(*Pointer<UShort>(element + 2)), 1);
		case FORMAT_R16UI:
			c = Insert(c, Int(*Pointer<UShort>(element)), 0);
			if(format != FORMAT_A16B16G16R16UI)
			{
				c = Insert(c, Int(0xFFFF), 3); // Set alpha
			}
			break;
		case FORMAT_A32B32G32R32I:
			c = *Pointer<Int4>(element);
			break;
		case FORMAT_X32B32G32R32I:
			c = Insert(c, *Pointer<Int>(element + 8), 2);
		case FORMAT_G32R32I:
			c = Insert(c, *Pointer<Int>(element + 4), 1);
		case FORMAT_R32I:
			c = Insert(c, *Pointer<Int>(element), 0);
			c = Insert(c, Int(0x7FFFFFFF), 3); // Set alpha
			break;
		case FORMAT_A32B32G32R32UI:
			c = *Pointer<UInt4>(element);
			break;
		case FORMAT_X32B32G32R32UI:
			c = Insert(c, Int(*Pointer<UInt>(element + 8)), 2);
		case FORMAT_G32R32UI:
			c = Insert(c, Int(*Pointer<UInt>(element + 4)), 1);
		case FORMAT_R32UI:
			c = Insert(c, Int(*Pointer<UInt>(element)), 0);
			c = Insert(c, Int(UInt(0xFFFFFFFFU)), 3); // Set alpha
			break;
		default:
			return false;
		}

		return true;
	}

	bool Blitter::write(Int4 &c, Pointer<Byte> element, Format format, const Blitter::Options& options)
	{
		bool writeR = (options & WRITE_RED) == WRITE_RED;
		bool writeG = (options & WRITE_GREEN) == WRITE_GREEN;
		bool writeB = (options & WRITE_BLUE) == WRITE_BLUE;
		bool writeA = (options & WRITE_ALPHA) == WRITE_ALPHA;
		bool writeRGBA = writeR && writeG && writeB && writeA;

		switch(format)
		{
		case FORMAT_A8B8G8R8I:
			if(writeA) { *Pointer<SByte>(element + 3) = SByte(Extract(c, 3)); }
		case FORMAT_X8B8G8R8I:
			if(writeA && (format != FORMAT_A8B8G8R8I))
			{
				*Pointer<SByte>(element + 3) = SByte(0x7F);
			}
			if(writeB) { *Pointer<SByte>(element + 2) = SByte(Extract(c, 2)); }
		case FORMAT_G8R8I:
			if(writeG) { *Pointer<SByte>(element + 1) = SByte(Extract(c, 1)); }
		case FORMAT_R8I:
			if(writeR) { *Pointer<SByte>(element) = SByte(Extract(c, 0)); }
			break;
		case FORMAT_A8B8G8R8UI:
			if(writeA) { *Pointer<Byte>(element + 3) = Byte(Extract(c, 3)); }
		case FORMAT_X8B8G8R8UI:
			if(writeA && (format != FORMAT_A8B8G8R8UI))
			{
				*Pointer<Byte>(element + 3) = Byte(0xFF);
			}
			if(writeB) { *Pointer<Byte>(element + 2) = Byte(Extract(c, 2)); }
		case FORMAT_G8R8UI:
			if(writeG) { *Pointer<Byte>(element + 1) = Byte(Extract(c, 1)); }
		case FORMAT_R8UI:
			if(writeR) { *Pointer<Byte>(element) = Byte(Extract(c, 0)); }
			break;
		case FORMAT_A16B16G16R16I:
			if(writeA) { *Pointer<Short>(element + 6) = Short(Extract(c, 3)); }
		case FORMAT_X16B16G16R16I:
			if(writeA && (format != FORMAT_A16B16G16R16I))
			{
				*Pointer<Short>(element + 6) = Short(0x7FFF);
			}
			if(writeB) { *Pointer<Short>(element + 4) = Short(Extract(c, 2)); }
		case FORMAT_G16R16I:
			if(writeG) { *Pointer<Short>(element + 2) = Short(Extract(c, 1)); }
		case FORMAT_R16I:
			if(writeR) { *Pointer<Short>(element) = Short(Extract(c, 0)); }
			break;
		case FORMAT_A16B16G16R16UI:
			if(writeA) { *Pointer<UShort>(element + 6) = UShort(Extract(c, 3)); }
		case FORMAT_X16B16G16R16UI:
			if(writeA && (format != FORMAT_A16B16G16R16UI))
			{
				*Pointer<UShort>(element + 6) = UShort(0xFFFF);
			}
			if(writeB) { *Pointer<UShort>(element + 4) = UShort(Extract(c, 2)); }
		case FORMAT_G16R16UI:
			if(writeG) { *Pointer<UShort>(element + 2) = UShort(Extract(c, 1)); }
		case FORMAT_R16UI:
			if(writeR) { *Pointer<UShort>(element) = UShort(Extract(c, 0)); }
			break;
		case FORMAT_A32B32G32R32I:
			if(writeRGBA)
			{
				*Pointer<Int4>(element) = c;
			}
			else
			{
				if(writeR) { *Pointer<Int>(element) = Extract(c, 0); }
				if(writeG) { *Pointer<Int>(element + 4) = Extract(c, 1); }
				if(writeB) { *Pointer<Int>(element + 8) = Extract(c, 2); }
				if(writeA) { *Pointer<Int>(element + 12) = Extract(c, 3); }
			}
			break;
		case FORMAT_X32B32G32R32I:
			if(writeRGBA)
			{
				*Pointer<Int4>(element) = c;
			}
			else
			{
				if(writeR) { *Pointer<Int>(element) = Extract(c, 0); }
				if(writeG) { *Pointer<Int>(element + 4) = Extract(c, 1); }
				if(writeB) { *Pointer<Int>(element + 8) = Extract(c, 2); }
			}
			if(writeA) { *Pointer<Int>(element + 12) = Int(0x7FFFFFFF); }
			break;
		case FORMAT_G32R32I:
			if(writeR) { *Pointer<Int>(element) = Extract(c, 0); }
			if(writeG) { *Pointer<Int>(element + 4) = Extract(c, 1); }
			break;
		case FORMAT_R32I:
			if(writeR) { *Pointer<Int>(element) = Extract(c, 0); }
			break;
		case FORMAT_A32B32G32R32UI:
			if(writeRGBA)
			{
				*Pointer<UInt4>(element) = As<UInt4>(c);
			}
			else
			{
				if(writeR) { *Pointer<UInt>(element) = As<UInt>(Extract(c, 0)); }
				if(writeG) { *Pointer<UInt>(element + 4) = As<UInt>(Extract(c, 1)); }
				if(writeB) { *Pointer<UInt>(element + 8) = As<UInt>(Extract(c, 2)); }
				if(writeA) { *Pointer<UInt>(element + 12) = As<UInt>(Extract(c, 3)); }
			}
			break;
		case FORMAT_X32B32G32R32UI:
			if(writeRGBA)
			{
				*Pointer<UInt4>(element) = As<UInt4>(c);
			}
			else
			{
				if(writeR) { *Pointer<UInt>(element) = As<UInt>(Extract(c, 0)); }
				if(writeG) { *Pointer<UInt>(element + 4) = As<UInt>(Extract(c, 1)); }
				if(writeB) { *Pointer<UInt>(element + 8) = As<UInt>(Extract(c, 2)); }
			}
			if(writeA) { *Pointer<UInt>(element + 3) = UInt(0xFFFFFFFF); }
			break;
		case FORMAT_G32R32UI:
			if(writeR) { *Pointer<UInt>(element) = As<UInt>(Extract(c, 0)); }
			if(writeG) { *Pointer<UInt>(element + 4) = As<UInt>(Extract(c, 1)); }
			break;
		case FORMAT_R32UI:
			if(writeR) { *Pointer<UInt>(element) = As<UInt>(Extract(c, 0)); }
			break;
		default:
			return false;
		}

		return true;
	}

	bool Blitter::GetScale(float4& scale, Format format)
	{
		switch(format)
		{
		case FORMAT_L8:
		case FORMAT_A8:
		case FORMAT_A8R8G8B8:
		case FORMAT_X8R8G8B8:
		case FORMAT_R8:
		case FORMAT_G8R8:
		case FORMAT_R8G8B8:
		case FORMAT_B8G8R8:
		case FORMAT_X8B8G8R8:
		case FORMAT_A8B8G8R8:
			scale = vector(0xFF, 0xFF, 0xFF, 0xFF);
			break;
		case FORMAT_R8I_SNORM:
		case FORMAT_G8R8I_SNORM:
		case FORMAT_X8B8G8R8I_SNORM:
		case FORMAT_A8B8G8R8I_SNORM:
			scale = vector(0x7F, 0x7F, 0x7F, 0x7F);
			break;
		case FORMAT_A16B16G16R16:
			scale = vector(0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF);
			break;
		case FORMAT_R8I:
		case FORMAT_R8UI:
		case FORMAT_G8R8I:
		case FORMAT_G8R8UI:
		case FORMAT_X8B8G8R8I:
		case FORMAT_X8B8G8R8UI:
		case FORMAT_A8B8G8R8I:
		case FORMAT_A8B8G8R8UI:
		case FORMAT_R16I:
		case FORMAT_R16UI:
		case FORMAT_G16R16:
		case FORMAT_G16R16I:
		case FORMAT_G16R16UI:
		case FORMAT_X16B16G16R16I:
		case FORMAT_X16B16G16R16UI:
		case FORMAT_A16B16G16R16I:
		case FORMAT_A16B16G16R16UI:
		case FORMAT_R32I:
		case FORMAT_R32UI:
		case FORMAT_G32R32I:
		case FORMAT_G32R32UI:
		case FORMAT_X32B32G32R32I:
		case FORMAT_X32B32G32R32UI:
		case FORMAT_A32B32G32R32I:
		case FORMAT_A32B32G32R32UI:
		case FORMAT_A32B32G32R32F:
		case FORMAT_X32B32G32R32F:
		case FORMAT_B32G32R32F:
		case FORMAT_G32R32F:
		case FORMAT_R32F:
			scale = vector(1.0f, 1.0f, 1.0f, 1.0f);
			break;
		case FORMAT_R5G6B5:
			scale = vector(0x1F, 0x3F, 0x1F, 1.0f);
			break;
		case FORMAT_A2B10G10R10:
			scale = vector(0x3FF, 0x3FF, 0x3FF, 0x03);
			break;
		case FORMAT_D16:
			scale = vector(0xFFFF, 0.0f, 0.0f, 0.0f);
			break;
		case FORMAT_D24S8:
			scale = vector(0xFFFFFF, 0.0f, 0.0f, 0.0f);
			break;
		case FORMAT_D32:
			scale = vector(0xFFFFFFFF, 0.0f, 0.0f, 0.0f);
			break;
		case FORMAT_D32F:
		case FORMAT_D32F_COMPLEMENTARY:
		case FORMAT_D32F_LOCKABLE:
		case FORMAT_D32FS8_TEXTURE:
		case FORMAT_D32FS8_SHADOW:
			scale = vector(1.0f, 0.0f, 0.0f, 0.0f);
			break;
		default:
			return false;
		}

		return true;
	}

	bool Blitter::ApplyScaleAndClamp(Float4& value, const BlitState& state)
	{
		float4 scale, unscale;
		if(Surface::isNonNormalizedInteger(state.sourceFormat) &&
		   !Surface::isNonNormalizedInteger(state.destFormat) &&
		   (state.options & CLEAR_OPERATION))
		{
			// If we're clearing a buffer from an int or uint color into a normalized color,
			// then the whole range of the int or uint color must be scaled between 0 and 1.
			switch(state.sourceFormat)
			{
			case FORMAT_A32B32G32R32I:
				unscale = vector(0x7FFFFFFF, 0x7FFFFFFF, 0x7FFFFFFF, 0x7FFFFFFF);
				break;
			case FORMAT_A32B32G32R32UI:
				unscale = vector(0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF);
				break;
			default:
				return false;
			}
		}
		else if(!GetScale(unscale, state.sourceFormat))
		{
			return false;
		}

		if(!GetScale(scale, state.destFormat))
		{
			return false;
		}

		if(unscale != scale)
		{
			value *= Float4(scale.x / unscale.x, scale.y / unscale.y, scale.z / unscale.z, scale.w / unscale.w);
		}

		if(Surface::isFloatFormat(state.sourceFormat) && !Surface::isFloatFormat(state.destFormat))
		{
			value = Min(value, Float4(scale.x, scale.y, scale.z, scale.w));

			value = Max(value, Float4(Surface::isUnsignedComponent(state.destFormat, 0) ? 0.0f : -scale.x,
			                          Surface::isUnsignedComponent(state.destFormat, 1) ? 0.0f : -scale.y,
			                          Surface::isUnsignedComponent(state.destFormat, 2) ? 0.0f : -scale.z,
			                          Surface::isUnsignedComponent(state.destFormat, 3) ? 0.0f : -scale.w));
		}

		return true;
	}

	Routine *Blitter::generate(BlitState &state)
	{
		Function<Void(Pointer<Byte>)> function;
		{
			Pointer<Byte> blit(function.Arg<0>());

			Pointer<Byte> source = *Pointer<Pointer<Byte>>(blit + OFFSET(BlitData,source));
			Pointer<Byte> dest = *Pointer<Pointer<Byte>>(blit + OFFSET(BlitData,dest));
			Int sPitchB = *Pointer<Int>(blit + OFFSET(BlitData,sPitchB));
			Int dPitchB = *Pointer<Int>(blit + OFFSET(BlitData,dPitchB));

			Float x0 = *Pointer<Float>(blit + OFFSET(BlitData,x0));
			Float y0 = *Pointer<Float>(blit + OFFSET(BlitData,y0));
			Float w = *Pointer<Float>(blit + OFFSET(BlitData,w));
			Float h = *Pointer<Float>(blit + OFFSET(BlitData,h));

			Int x0d = *Pointer<Int>(blit + OFFSET(BlitData,x0d));
			Int x1d = *Pointer<Int>(blit + OFFSET(BlitData,x1d));
			Int y0d = *Pointer<Int>(blit + OFFSET(BlitData,y0d));
			Int y1d = *Pointer<Int>(blit + OFFSET(BlitData,y1d));

			Int sWidth = *Pointer<Int>(blit + OFFSET(BlitData,sWidth));
			Int sHeight = *Pointer<Int>(blit + OFFSET(BlitData,sHeight));

			bool intSrc = Surface::isNonNormalizedInteger(state.sourceFormat);
			bool intDst = Surface::isNonNormalizedInteger(state.destFormat);
			bool intBoth = intSrc && intDst;

			bool hasConstantColorI = false;
			Int4 constantColorI;
			bool hasConstantColorF = false;
			Float4 constantColorF;
			if(state.options & CLEAR_OPERATION)
			{
				if(intBoth) // Integer types
				{
					if(!read(constantColorI, source, state.sourceFormat))
					{
						return nullptr;
					}
					hasConstantColorI = true;
				}
				else
				{
					if(!read(constantColorF, source, state.sourceFormat))
					{
						return nullptr;
					}
					hasConstantColorF = true;

					if(!ApplyScaleAndClamp(constantColorF, state))
					{
						return nullptr;
					}
				}
			}

			Float y = y0;

			For(Int j = y0d, j < y1d, j++)
			{
				Float x = x0;
				Pointer<Byte> destLine = dest + j * dPitchB;

				For(Int i = x0d, i < x1d, i++)
				{
					Pointer<Byte> d = destLine + i * Surface::bytes(state.destFormat);
					if(hasConstantColorI)
					{
						if(!write(constantColorI, d, state.destFormat, state.options))
						{
							return nullptr;
						}
					}
					else if(hasConstantColorF)
					{
						if(!write(constantColorF, d, state.destFormat, state.options))
						{
							return nullptr;
						}
					}
					else if(intBoth) // Integer types do not support filtering
					{
						Int4 color; // When both formats are true integer types, we don't go to float to avoid losing precision
						Pointer<Byte> s = source + Int(y) * sPitchB + Int(x) * Surface::bytes(state.sourceFormat);
						if(!read(color, s, state.sourceFormat))
						{
							return nullptr;
						}

						if(!write(color, d, state.destFormat, state.options))
						{
							return nullptr;
						}
					}
					else
					{
						Float4 color;

						if(!(state.options & FILTER_LINEAR) || intSrc)
						{
							Int X = Int(x);
							Int Y = Int(y);

							Pointer<Byte> s = source + Y * sPitchB + X * Surface::bytes(state.sourceFormat);

							if(!read(color, s, state.sourceFormat))
							{
								return nullptr;
							}
						}
						else   // Bilinear filtering
						{
							Float x0 = x - 0.5f;
							Float y0 = y - 0.5f;

							Int X0 = Max(Int(x0), 0);
							Int Y0 = Max(Int(y0), 0);

							Int X1 = IfThenElse(X0 + 1 >= sWidth, X0, X0 + 1);
							Int Y1 = IfThenElse(Y0 + 1 >= sHeight, Y0, Y0 + 1);

							Pointer<Byte> s00 = source + Y0 * sPitchB + X0 * Surface::bytes(state.sourceFormat);
							Pointer<Byte> s01 = source + Y0 * sPitchB + X1 * Surface::bytes(state.sourceFormat);
							Pointer<Byte> s10 = source + Y1 * sPitchB + X0 * Surface::bytes(state.sourceFormat);
							Pointer<Byte> s11 = source + Y1 * sPitchB + X1 * Surface::bytes(state.sourceFormat);

							Float4 c00; if(!read(c00, s00, state.sourceFormat)) return nullptr;
							Float4 c01; if(!read(c01, s01, state.sourceFormat)) return nullptr;
							Float4 c10; if(!read(c10, s10, state.sourceFormat)) return nullptr;
							Float4 c11; if(!read(c11, s11, state.sourceFormat)) return nullptr;

							Float4 fx = Float4(x0 - Float(X0));
							Float4 fy = Float4(y0 - Float(Y0));

							color = c00 * (Float4(1.0f) - fx) * (Float4(1.0f) - fy) +
							        c01 * fx * (Float4(1.0f) - fy) +
							        c10 * (Float4(1.0f) - fx) * fy +
							        c11 * fx * fy;
						}

						if(!ApplyScaleAndClamp(color, state) || !write(color, d, state.destFormat, state.options))
						{
							return nullptr;
						}
					}

					if(!hasConstantColorI && !hasConstantColorF) { x += w; }
				}

				if(!hasConstantColorI && !hasConstantColorF) { y += h; }
			}
		}

		return function(L"BlitRoutine");
	}

	bool Blitter::blitReactor(Surface *source, const SliceRect &sourceRect, Surface *dest, const SliceRect &destRect, const Blitter::Options& options)
	{
		ASSERT(!(options & CLEAR_OPERATION) || ((source->getWidth() == 1) && (source->getHeight() == 1) && (source->getDepth() == 1)));

		Rect dRect = destRect;
		Rect sRect = sourceRect;
		if(destRect.x0 > destRect.x1)
		{
			swap(dRect.x0, dRect.x1);
			swap(sRect.x0, sRect.x1);
		}
		if(destRect.y0 > destRect.y1)
		{
			swap(dRect.y0, dRect.y1);
			swap(sRect.y0, sRect.y1);
		}

		BlitState state;

		bool useSourceInternal = !source->isExternalDirty();
		bool useDestInternal = !dest->isExternalDirty();

		state.sourceFormat = source->getFormat(useSourceInternal);
		state.destFormat = dest->getFormat(useDestInternal);
		state.options = options;

		criticalSection.lock();
		Routine *blitRoutine = blitCache->query(state);

		if(!blitRoutine)
		{
			blitRoutine = generate(state);

			if(!blitRoutine)
			{
				criticalSection.unlock();
				return false;
			}

			blitCache->add(state, blitRoutine);
		}

		criticalSection.unlock();

		void (*blitFunction)(const BlitData *data) = (void(*)(const BlitData*))blitRoutine->getEntry();

		BlitData data;

		bool isRGBA = ((options & WRITE_RGBA) == WRITE_RGBA);
		bool isEntireDest = dest->isEntire(destRect);

		data.source = source->lock(0, 0, sourceRect.slice, sw::LOCK_READONLY, sw::PUBLIC, useSourceInternal);
		data.dest = dest->lock(0, 0, destRect.slice, isRGBA ? (isEntireDest ? sw::LOCK_DISCARD : sw::LOCK_WRITEONLY) : sw::LOCK_READWRITE, sw::PUBLIC, useDestInternal);
		data.sPitchB = source->getPitchB(useSourceInternal);
		data.dPitchB = dest->getPitchB(useDestInternal);

		data.w = 1.0f / (dRect.x1 - dRect.x0) * (sRect.x1 - sRect.x0);
		data.h = 1.0f / (dRect.y1 - dRect.y0) * (sRect.y1 - sRect.y0);
		data.x0 = (float)sRect.x0 + 0.5f * data.w;
		data.y0 = (float)sRect.y0 + 0.5f * data.h;

		data.x0d = dRect.x0;
		data.x1d = dRect.x1;
		data.y0d = dRect.y0;
		data.y1d = dRect.y1;

		data.sWidth = source->getWidth();
		data.sHeight = source->getHeight();

		blitFunction(&data);

		source->unlock(useSourceInternal);
		dest->unlock(useDestInternal);

		return true;
	}
}
