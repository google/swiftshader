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

	void Blitter::blit(Surface *source, const SliceRect &sourceRect, Surface *dest, const SliceRect &destRect, bool filter)
	{
		if(blitReactor(source, sourceRect, dest, destRect, filter))
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
				dest->copyInternal(source, i, j, x, y, filter);

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
		c = Float4(1.0f, 1.0f, 1.0f, 1.0f);

		switch(format)
		{
		case FORMAT_L8:
			c.xyz = Float(Int(*Pointer<Byte>(element)));
			break;
		case FORMAT_A8:
			c.xyz = 0.0f;
			c.w = Float(Int(*Pointer<Byte>(element)));
			break;
		case FORMAT_R8I:
			c.yzw = 0.0f;
			c.x = Float(Int(*Pointer<SByte>(element)));
			break;
		case FORMAT_R8UI:
			c.yzw = 0.0f;
			c.x = Float(Int(*Pointer<Byte>(element)));
			break;
		case FORMAT_R8I_SNORM:
			c.yzw = 0.0f;
			c.x = Float(Int(*Pointer<SByte>(element)));
			break;
		case FORMAT_R8:
			c.yzw = 0.0f;
			c.x = Float(Int(*Pointer<Byte>(element)));
			break;
		case FORMAT_R16I:
			c.yzw = 0.0f;
			c.x = Float(Int(*Pointer<Short>(element)));
			break;
		case FORMAT_R16UI:
			c.yzw = 0.0f;
			c.x = Float(Int(*Pointer<UShort>(element)));
			break;
		case FORMAT_R32I:
			c.yzw = 0.0f;
			c.x = Float(Int(*Pointer<Int>(element)));
			break;
		case FORMAT_R32UI:
			c.yzw = 0.0f;
			c.x = Float(Int(*Pointer<UInt>(element)));
			break;
		case FORMAT_A8R8G8B8:
			c = Float4(*Pointer<Byte4>(element)).zyxw;
			break;
		case FORMAT_A8B8G8R8I:
			c = Float4(*Pointer<SByte4>(element));
			break;
		case FORMAT_A8B8G8R8UI:
			c = Float4(*Pointer<Byte4>(element));
			break;
		case FORMAT_A8B8G8R8I_SNORM:
			c = Float4(*Pointer<SByte4>(element));
			break;
		case FORMAT_A8B8G8R8:
			c = Float4(*Pointer<Byte4>(element));
			break;
		case FORMAT_X8R8G8B8:
			c = Float4(*Pointer<Byte4>(element)).zyxw;
			c.w = 1.0f;
			break;
		case FORMAT_X8B8G8R8I:
			c = Float4(*Pointer<SByte4>(element));
			c.w = 1.0f;
			break;
		case FORMAT_X8B8G8R8UI:
			c = Float4(*Pointer<Byte4>(element));
			c.w = 1.0f;
			break;
		case FORMAT_X8B8G8R8I_SNORM:
			c = Float4(*Pointer<SByte4>(element));
			c.w = 1.0f;
			break;
		case FORMAT_X8B8G8R8:
			c = Float4(*Pointer<Byte4>(element));
			c.w = 1.0f;
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
			c.w = 1.0f;
			break;
		case FORMAT_X16B16G16R16UI:
			c = Float4(*Pointer<UShort4>(element));
			c.w = 1.0f;
			break;
		case FORMAT_A32B32G32R32I:
			c = Float4(*Pointer<Int4>(element));
			break;
		case FORMAT_A32B32G32R32UI:
			c = Float4(*Pointer<UInt4>(element));
			break;
		case FORMAT_X32B32G32R32I:
			c = Float4(*Pointer<Int4>(element));
			c.w = 1.0f;
			break;
		case FORMAT_X32B32G32R32UI:
			c = Float4(*Pointer<UInt4>(element));
			c.w = 1.0f;
			break;
		case FORMAT_G8R8I:
			c.x = Float(Int(*Pointer<SByte>(element + 0)));
			c.y = Float(Int(*Pointer<SByte>(element + 1)));
			break;
		case FORMAT_G8R8UI:
			c.x = Float(Int(*Pointer<Byte>(element + 0)));
			c.y = Float(Int(*Pointer<Byte>(element + 1)));
			break;
		case FORMAT_G8R8I_SNORM:
			c.x = Float(Int(*Pointer<SByte>(element + 0)));
			c.y = Float(Int(*Pointer<SByte>(element + 1)));
			break;
		case FORMAT_G8R8:
			c.x = Float(Int(*Pointer<Byte>(element + 0)));
			c.y = Float(Int(*Pointer<Byte>(element + 1)));
			break;
		case FORMAT_G16R16I:
			c.x = Float(Int(*Pointer<Short>(element + 0)));
			c.y = Float(Int(*Pointer<Short>(element + 2)));
			break;
		case FORMAT_G16R16:
		case FORMAT_G16R16UI:
			c.x = Float(Int(*Pointer<UShort>(element + 0)));
			c.y = Float(Int(*Pointer<UShort>(element + 2)));
			break;
		case FORMAT_G32R32I:
			c.x = Float(Int(*Pointer<Int>(element + 0)));
			c.y = Float(Int(*Pointer<Int>(element + 4)));
			break;
		case FORMAT_G32R32UI:
			c.x = Float(Int(*Pointer<UInt>(element + 0)));
			c.y = Float(Int(*Pointer<UInt>(element + 4)));
			break;
		case FORMAT_A32B32G32R32F:
			c = *Pointer<Float4>(element);
			break;
		case FORMAT_G32R32F:
			c.x = *Pointer<Float>(element + 0);
			c.y = *Pointer<Float>(element + 4);
			break;
		case FORMAT_R32F:
			c.x = *Pointer<Float>(element);
			break;
		default:
			return false;
		}

		return true;
	}

	bool Blitter::write(Float4 &c, Pointer<Byte> element, Format format)
	{
		switch(format)
		{
		case FORMAT_L8:
			*Pointer<Byte>(element) = Byte(RoundInt(Float(c.x)));
			break;
		case FORMAT_A8:
			*Pointer<Byte>(element) = Byte(RoundInt(Float(c.w)));
			break;
		case FORMAT_A8R8G8B8:
			{
				UShort4 c0 = As<UShort4>(RoundShort4(c.zyxw));
				Byte8 c1 = Pack(c0, c0);
				*Pointer<UInt>(element) = UInt(As<Long>(c1));
			}
			break;
		case FORMAT_A8B8G8R8:
			{
				UShort4 c0 = As<UShort4>(RoundShort4(c));
				Byte8 c1 = Pack(c0, c0);
				*Pointer<UInt>(element) = UInt(As<Long>(c1));
			}
			break;
		case FORMAT_X8R8G8B8:
			{
				UShort4 c0 = As<UShort4>(RoundShort4(c.zyxw));
				Byte8 c1 = Pack(c0, c0);
				*Pointer<UInt>(element) = UInt(As<Long>(c1)) | 0xFF000000;
			}
			break;
		case FORMAT_X8B8G8R8:
			{
				UShort4 c0 = As<UShort4>(RoundShort4(c));
				Byte8 c1 = Pack(c0, c0);
				*Pointer<UInt>(element) = UInt(As<Long>(c1)) | 0xFF000000;
			}
			break;
		case FORMAT_A32B32G32R32F:
			*Pointer<Float4>(element) = c;
			break;
		case FORMAT_G32R32F:
			*Pointer<Float2>(element) = Float2(c);
			break;
		case FORMAT_R32F:
			*Pointer<Float>(element) = c.x;
			break;
		case FORMAT_A8B8G8R8I:
		case FORMAT_A8B8G8R8I_SNORM:
			*Pointer<SByte>(element + 3) = As<SByte>(RoundInt(Float(c.w)));
		case FORMAT_X8B8G8R8I:
		case FORMAT_X8B8G8R8I_SNORM:
			if(format == FORMAT_X8B8G8R8I || format == FORMAT_X8B8G8R8I_SNORM)
			{
				*Pointer<SByte>(element + 3) = SByte(0x7F);
			}
			*Pointer<SByte>(element + 2) = As<SByte>(RoundInt(Float(c.z)));
		case FORMAT_G8R8I:
		case FORMAT_G8R8I_SNORM:
			*Pointer<SByte>(element + 1) = As<SByte>(RoundInt(Float(c.y)));
		case FORMAT_R8I:
		case FORMAT_R8I_SNORM:
			*Pointer<SByte>(element) = As<SByte>(RoundInt(Float(c.x)));
			break;
		case FORMAT_A8B8G8R8UI:
			*Pointer<Byte>(element + 3) = As<Byte>(UInt(Float(c.w) + Float(0.5f)));
		case FORMAT_X8B8G8R8UI:
			if(format == FORMAT_X8B8G8R8UI)
			{
				*Pointer<Byte>(element + 3) = Byte(0xFF);
			}
			*Pointer<Byte>(element + 2) = As<Byte>(UInt(Float(c.z) + Float(0.5f)));
		case FORMAT_G8R8UI:
		case FORMAT_G8R8:
			*Pointer<Byte>(element + 1) = As<Byte>(UInt(Float(c.y) + Float(0.5f)));
		case FORMAT_R8UI:
		case FORMAT_R8:
			*Pointer<Byte>(element) = As<Byte>(UInt(Float(c.x) + Float(0.5f)));
			break;
		case FORMAT_A16B16G16R16I:
			*Pointer<Short4>(element) = Short4(RoundInt(c));
			break;
		case FORMAT_X16B16G16R16I:
			*Pointer<Short4>(element) = Short4(RoundInt(c));
			*Pointer<Short>(element + 6) = Short(0x7F);
			break;
		case FORMAT_G16R16I:
			*Pointer<UInt>(element) = UInt(As<Long>(Short4(RoundInt(c))));
			break;
		case FORMAT_R16I:
			*Pointer<Short>(element) = As<Short>(RoundInt(Float(c.x)));
			break;
		case FORMAT_A16B16G16R16UI:
		case FORMAT_A16B16G16R16:
			*Pointer<UShort4>(element) = UShort4(RoundInt(c));
			break;
		case FORMAT_X16B16G16R16UI:
			*Pointer<UShort4>(element) = UShort4(RoundInt(c));
			*Pointer<UShort>(element + 6) = UShort(0xFF);
			break;
		case FORMAT_G16R16UI:
		case FORMAT_G16R16:
			*Pointer<UInt>(element) = UInt(As<Long>(UShort4(RoundInt(c))));
		case FORMAT_R16UI:
			*Pointer<UShort>(element) = As<UShort>(UInt(Float(c.x) + Float(0.5f)));
			break;
		case FORMAT_A32B32G32R32I:
			*Pointer<Int4>(element) = RoundInt(c);
			break;
		case FORMAT_X32B32G32R32I:
			*Pointer<Int4>(element) = RoundInt(c);
			*Pointer<Int>(element + 12) = Int(0x7FFFFFFF);
			break;
		case FORMAT_G32R32I:
			*Pointer<Int>(element + 4) = RoundInt(Float(c.y));
		case FORMAT_R32I:
			*Pointer<Int>(element) = RoundInt(Float(c.x));
			break;
		case FORMAT_A32B32G32R32UI:
			*Pointer<UInt4>(element) = UInt4(c + Float4(0.5f));
			break;
		case FORMAT_X32B32G32R32UI:
			*Pointer<UInt4>(element) = UInt4(c + Float4(0.5f));
			*Pointer<UInt4>(element + 12) = UInt4(0xFFFFFFFF);
			break;
		case FORMAT_G32R32UI:
			*Pointer<UInt>(element + 4) = UInt(Float(c.y) + Float(0.5f));
		case FORMAT_R32UI:
			*Pointer<UInt>(element) = UInt(Float(c.x) + Float(0.5f));
			break;
		default:
			return false;
		}
		return true;
	}

	bool Blitter::read(Int4 &c, Pointer<Byte> element, Format format)
	{
		c = Int4(0);

		switch(format)
		{
		case FORMAT_A8B8G8R8I:
			Insert(c, Int(*Pointer<SByte>(element + 3)), 3);
		case FORMAT_X8B8G8R8I:
			Insert(c, Int(*Pointer<SByte>(element + 2)), 2);
		case FORMAT_G8R8I:
			Insert(c, Int(*Pointer<SByte>(element + 1)), 1);
		case FORMAT_R8I:
			Insert(c, Int(*Pointer<SByte>(element)), 0);
			if(format != FORMAT_A8B8G8R8I)
			{
				Insert(c, Int(0x7F), 3); // Set alpha
			}
			break;
		case FORMAT_A8B8G8R8UI:
			Insert(c, Int(*Pointer<Byte>(element + 3)), 3);
		case FORMAT_X8B8G8R8UI:
			Insert(c, Int(*Pointer<Byte>(element + 2)), 2);
		case FORMAT_G8R8UI:
			Insert(c, Int(*Pointer<Byte>(element + 1)), 1);
		case FORMAT_R8UI:
			Insert(c, Int(*Pointer<Byte>(element)), 0);
			if(format != FORMAT_A8B8G8R8UI)
			{
				Insert(c, Int(0xFF), 3); // Set alpha
			}
			break;
		case FORMAT_A16B16G16R16I:
			Insert(c, Int(*Pointer<Short>(element + 3)), 3);
		case FORMAT_X16B16G16R16I:
			Insert(c, Int(*Pointer<Short>(element + 2)), 2);
		case FORMAT_G16R16I:
			Insert(c, Int(*Pointer<Short>(element + 1)), 1);
		case FORMAT_R16I:
			Insert(c, Int(*Pointer<Short>(element)), 0);
			if(format != FORMAT_A16B16G16R16I)
			{
				Insert(c, Int(0x7FFF), 3); // Set alpha
			}
			break;
		case FORMAT_A16B16G16R16UI:
			Insert(c, Int(*Pointer<UShort>(element + 3)), 3);
		case FORMAT_X16B16G16R16UI:
			Insert(c, Int(*Pointer<UShort>(element + 2)), 2);
		case FORMAT_G16R16UI:
			Insert(c, Int(*Pointer<UShort>(element + 1)), 1);
		case FORMAT_R16UI:
			Insert(c, Int(*Pointer<UShort>(element)), 0);
			if(format != FORMAT_A16B16G16R16UI)
			{
				Insert(c, Int(0xFFFF), 3); // Set alpha
			}
			break;
		case FORMAT_A32B32G32R32I:
			Insert(c, Int(*Pointer<Int>(element + 3)), 3);
		case FORMAT_X32B32G32R32I:
			Insert(c, Int(*Pointer<Int>(element + 2)), 2);
		case FORMAT_G32R32I:
			Insert(c, Int(*Pointer<Int>(element + 1)), 1);
		case FORMAT_R32I:
			Insert(c, Int(*Pointer<Int>(element)), 0);
			if(format != FORMAT_A32B32G32R32I)
			{
				Insert(c, Int(0x7FFFFFFF), 3); // Set alpha
			}
			break;
		case FORMAT_A32B32G32R32UI:
			Insert(c, Int(*Pointer<UInt>(element + 3)), 3);
		case FORMAT_X32B32G32R32UI:
			Insert(c, Int(*Pointer<UInt>(element + 2)), 2);
		case FORMAT_G32R32UI:
			Insert(c, Int(*Pointer<UInt>(element + 1)), 1);
		case FORMAT_R32UI:
			Insert(c, Int(*Pointer<UInt>(element)), 0);
			if(format != FORMAT_A32B32G32R32UI)
			{
				Insert(c, Int(UInt(0xFFFFFFFFU)), 3); // Set alpha
			}
			break;
		default:
			return false;
		}

		return true;
	}

	bool Blitter::write(Int4 &c, Pointer<Byte> element, Format format)
	{
		switch(format)
		{
		case FORMAT_A8B8G8R8I:
			*Pointer<SByte>(element + 3) = As<SByte>(Extract(c, 3));
		case FORMAT_X8B8G8R8I:
			*Pointer<SByte>(element + 2) = As<SByte>(Extract(c, 2));
		case FORMAT_G8R8I:
			*Pointer<SByte>(element + 1) = As<SByte>(Extract(c, 1));
		case FORMAT_R8I:
			*Pointer<SByte>(element) = As<SByte>(Extract(c, 0));
			if(format != FORMAT_A8B8G8R8I)
			{
				*Pointer<SByte>(element + 3) = SByte(0x7F);
			}
			break;
		case FORMAT_A8B8G8R8UI:
			*Pointer<Byte>(element + 3) = As<Byte>(Extract(c, 3));
		case FORMAT_X8B8G8R8UI:
			*Pointer<Byte>(element + 2) = As<Byte>(Extract(c, 2));
		case FORMAT_G8R8UI:
			*Pointer<Byte>(element + 1) = As<Byte>(Extract(c, 1));
		case FORMAT_R8UI:
			*Pointer<Byte>(element) = As<Byte>(Extract(c, 0));
			if(format != FORMAT_A8B8G8R8UI)
			{
				*Pointer<Byte>(element + 3) = Byte(0xFF);
			}
			break;
		case FORMAT_A16B16G16R16I:
			*Pointer<Short>(element + 3) = As<Short>(Extract(c, 3));
		case FORMAT_X16B16G16R16I:
			*Pointer<Short>(element + 2) = As<Short>(Extract(c, 2));
		case FORMAT_G16R16I:
			*Pointer<Short>(element + 1) = As<Short>(Extract(c, 1));
		case FORMAT_R16I:
			*Pointer<Short>(element) = As<Short>(Extract(c, 0));
			if(format != FORMAT_A16B16G16R16I)
			{
				*Pointer<Short>(element + 3) = Short(0x7FFF);
			}
			break;
		case FORMAT_A16B16G16R16UI:
			*Pointer<UShort>(element + 3) = As<UShort>(Extract(c, 3));
		case FORMAT_X16B16G16R16UI:
			*Pointer<UShort>(element + 2) = As<UShort>(Extract(c, 2));
		case FORMAT_G16R16UI:
			*Pointer<UShort>(element + 1) = As<UShort>(Extract(c, 1));
		case FORMAT_R16UI:
			*Pointer<UShort>(element) = As<UShort>(Extract(c, 0));
			if(format != FORMAT_A16B16G16R16UI)
			{
				*Pointer<UShort>(element + 3) = UShort(0xFFFF);
			}
			break;
		case FORMAT_A32B32G32R32I:
			*Pointer<Int4>(element) = c;
			break;
		case FORMAT_X32B32G32R32I:
			*Pointer<Int4>(element) = c;
			*Pointer<Int>(element + 3) = Int(0x7FFFFFFF);
			break;
		case FORMAT_G32R32I:
			*Pointer<Int2>(element) = As<Int2>(c);
			break;
		case FORMAT_R32I:
			*Pointer<Int>(element) = Extract(c, 0);
			break;
		case FORMAT_A32B32G32R32UI:
			*Pointer<UInt4>(element) = As<UInt4>(c);
			break;
		case FORMAT_X32B32G32R32UI:
			*Pointer<UInt4>(element) = As<UInt4>(c);
			*Pointer<UInt>(element + 3) = UInt(0xFFFFFFFF);
			break;
		case FORMAT_G32R32UI:
			*Pointer<UInt2>(element) = As<UInt2>(c);
			break;
		case FORMAT_R32UI:
			*Pointer<UInt>(element) = As<UInt>(Extract(c, 0));
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
		case FORMAT_A16B16G16R16:
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
		case FORMAT_G32R32F:
		case FORMAT_R32F:
			scale = vector(1.0f, 1.0f, 1.0f, 1.0f);
			break;
		default:
			return false;
		}

		return true;
	}

	Routine *Blitter::generate(BlitState &state)
	{
		Function<Void, Pointer<Byte> > function;
		{
			Pointer<Byte> blit(function.arg(0));

			Pointer<Byte> source = *Pointer<Pointer<Byte> >(blit + OFFSET(BlitData,source));
			Pointer<Byte> dest = *Pointer<Pointer<Byte> >(blit + OFFSET(BlitData,dest));
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

			Float y = y0;

			For(Int j = y0d, j < y1d, j++)
			{
				Float x = x0;
				Pointer<Byte> destLine = dest + j * dPitchB;

				For(Int i = x0d, i < x1d, i++)
				{
					bool intSrc = Surface::isNonNormalizedInteger(state.sourceFormat);
					bool intDst = Surface::isNonNormalizedInteger(state.destFormat);

					if(intSrc && intDst) // Integer types do not support filtering
					{
						Int4 color; // When both formats are true integer types, we don't go to float to avoid losing precision
						Pointer<Byte> s = source + Int(y) * sPitchB + Int(x) * Surface::bytes(state.sourceFormat);
						if(!read(color, s, state.sourceFormat))
						{
							return nullptr;
						}

						Pointer<Byte> d = destLine + i * Surface::bytes(state.destFormat);
						if(!write(color, d, state.destFormat))
						{
							return nullptr;
						}
					}
					else
					{
						Float4 color;

						if(!state.filter || intSrc)
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

						float4 scale, unscale;
						if(!GetScale(unscale, state.sourceFormat) || !GetScale(scale, state.destFormat))
						{
							return nullptr;
						}

						if(unscale != scale)
						{
							color *= Float4(scale.x / unscale.x, scale.y / unscale.y, scale.z / unscale.z, scale.w / unscale.w);
						}

						if(Surface::isFloatFormat(state.sourceFormat) && !Surface::isFloatFormat(state.destFormat))
						{
							color = Min(color, Float4(1.0f, 1.0f, 1.0f, 1.0f));

							color = Max(color, Float4(Surface::isUnsignedComponent(state.destFormat, 0) ? 0.0f : -1.0f,
							                          Surface::isUnsignedComponent(state.destFormat, 1) ? 0.0f : -1.0f,
							                          Surface::isUnsignedComponent(state.destFormat, 2) ? 0.0f : -1.0f,
							                          Surface::isUnsignedComponent(state.destFormat, 3) ? 0.0f : -1.0f));
						}

						Pointer<Byte> d = destLine + i * Surface::bytes(state.destFormat);

						if(!write(color, d, state.destFormat))
						{
							return nullptr;
						}
					}

					x += w;
				}

				y += h;
			}
		}

		return function(L"BlitRoutine");
	}

	bool Blitter::blitReactor(Surface *source, const SliceRect &sourceRect, Surface *dest, const SliceRect &destRect, bool filter)
	{
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
		state.filter = filter;

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

		data.source = source->lock(0, 0, sourceRect.slice, sw::LOCK_READONLY, sw::PUBLIC, useSourceInternal);
		data.dest = dest->lock(0, 0, destRect.slice, sw::LOCK_WRITEONLY, sw::PUBLIC, useDestInternal);
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
