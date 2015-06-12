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

#ifndef sw_Types_hpp
#define sw_Types_hpp

#if defined(_MSC_VER)
	typedef signed __int8 int8_t;
	typedef signed __int16 int16_t;
	typedef signed __int32 int32_t;
	typedef signed __int64 int64_t;
	typedef unsigned __int8 uint8_t;
	typedef unsigned __int16 uint16_t;
	typedef unsigned __int32 uint32_t;
	typedef unsigned __int64 uint64_t;
	#define ALIGN(bytes, type) __declspec(align(bytes)) type
#else
	#include <stdint.h>
	#define ALIGN(bytes, type) type __attribute__((aligned(bytes)))
#endif

namespace sw
{
	typedef ALIGN(1, uint8_t) byte;
	typedef ALIGN(2, uint16_t) word;
	typedef ALIGN(4, uint32_t) dword;
	typedef ALIGN(8, uint64_t) qword;
	typedef ALIGN(16, uint64_t) qword2[2];
	typedef ALIGN(4, uint8_t) byte4[4];
	typedef ALIGN(8, uint8_t) byte8[8];
	typedef ALIGN(16, uint8_t) byte16[16];
	typedef ALIGN(8, uint16_t) word4[4];
	typedef ALIGN(8, uint32_t) dword2[2];
	typedef ALIGN(16, uint32_t) dword4[4];
	typedef ALIGN(16, uint64_t) xword[2];

	typedef ALIGN(1, int8_t) sbyte;
	typedef ALIGN(4, int8_t) sbyte4[4];
	typedef ALIGN(8, int8_t) sbyte8[8];
	typedef ALIGN(16, int8_t) sbyte16[16];
	typedef ALIGN(8, short) short4[4];
	typedef ALIGN(8, unsigned short) ushort4[4];
	typedef ALIGN(16, short) short8[8];
	typedef ALIGN(16, unsigned short) ushort8[8];
	typedef ALIGN(8, int) int2[2];
	typedef ALIGN(8, unsigned int) uint2[2];
	typedef ALIGN(16, unsigned int) uint4[4];

	typedef ALIGN(8, float) float2[2];

	ALIGN(16, struct int4
	{
		int x;
		int y;
		int z;
		int w;

		int &operator[](int i)
		{
			return (&x)[i];
		}

		const int &operator[](int i) const
		{
			return (&x)[i];
		}

		bool operator!=(const int4 &rhs)
		{
			return x != rhs.x || y != rhs.y || z != rhs.z || w != rhs.w;
		}

		bool operator==(const int4 &rhs)
		{
			return x == rhs.x && y == rhs.y && z == rhs.z && w == rhs.w;
		}
	});

	ALIGN(16, struct float4
	{
		float x;
		float y;
		float z;
		float w;

		float &operator[](int i)
		{
			return (&x)[i];
		}

		const float &operator[](int i) const
		{
			return (&x)[i];
		}

		bool operator!=(const float4 &rhs)
		{
			return x != rhs.x || y != rhs.y || z != rhs.z || w != rhs.w;
		}

		bool operator==(const float4 &rhs)
		{
			return x == rhs.x && y == rhs.y && z == rhs.z && w == rhs.w;
		}
	});

	inline float4 vector(float x, float y, float z, float w)
	{
		float4 v;

		v.x = x;
		v.y = y;
		v.z = z;
		v.w = w;

		return v;
	}
	
	inline float4 replicate(float f)
	{
		float4 v;

		v.x = f;
		v.y = f;
		v.z = f;
		v.w = f;

		return v;
	}

	#define OFFSET(s,m) (int)(size_t)&reinterpret_cast<const volatile char&>((((s*)0)->m))
}

#endif   // sw_Types_hpp
