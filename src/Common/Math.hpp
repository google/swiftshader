// SwiftShader Software Renderer
//
// Copyright(c) 2005-2011 TransGaming Inc.
//
// All rights reserved. No part of this software may be copied, distributed, transmitted,
// transcribed, stored in a retrieval system, translated into any human or computer
// language by any means, or disclosed to third parties without the explicit written
// agreement of TransGaming Inc. Without such an agreement, no rights or licenses, express
// or implied, including but not limited to any patent rights, are granted to you.
//

#ifndef sw_Math_hpp
#define sw_Math_hpp

#include "Types.hpp"

#include <math.h>
#include <intrin.h>

namespace sw
{
	extern const float M_PI;
	extern const float M_PI_180;
	extern const float M_180_PI;
	extern const float M_2PI;
	extern const float M_PI_2;

	inline float rad(float deg)
	{
		return deg * M_PI_180;
	}

	inline float deg(float rad)
	{
		return rad * M_180_PI;
	}

	inline float sin(float x)
	{
		return sinf(x);
	}

	inline float asin(float x)
	{
		return asinf(x);
	}

	inline float sinh(float x)
	{
		return sinhf(x);
	}

	inline float cos(float x)
	{
		return cosf(x);
	}

	inline float acos(float x)
	{
		return acosf(x);
	}

	inline float cosh(float x)
	{
		return coshf(x);
	}

	inline float tan(float x)
	{
		return tanf(x);
	}

	inline float cot(float x)
	{
		return 1.0f / tan(x);
	}

	inline float atan(float x)
	{
		return atanf(x);
	}

	inline float tanh(float x)
	{
		return tanhf(x);
	}

	inline float sqrt(float x)
	{
		return sqrtf(x);
	}

	inline float exp(float x)
	{
		return expf(x);
	}

	inline float exp2(float x)
	{
		static const float ln2 = logf(2);

		return exp(ln2 * x);
	}

	inline float pow(float x, float y)
	{
		return powf(x, y);
	}

	inline float pow2(float x)
	{
		return x * x;
	}

	inline float abs(float x)
	{
		return fabsf(x);
	}

	#undef min
	#undef max

	inline float max(float a, float b)
	{
		return a > b ? a : b;
	}

	inline float min(float a, float b) 
	{
		return a < b ? a : b;
	}

	inline float max(float a, float b, float c)
	{
		return max(max(a, b), c);
	}

	inline float min(float a, float b, float c)
	{
		return min(min(a, b), c);
	}

	inline float max(float a, float b, float c, float d)
	{
		return max(max(a, b), max(c, d));
	}

	inline float min(float a, float b, float c, float d)
	{
		return min(min(a, b), min(c, d));
	}

	inline int max(int a, int b)
	{
		return a > b ? a : b;
	}

	inline int min(int a, int b) 
	{
		return a < b ? a : b;
	}

	inline int max(int a, int b, int c)
	{
		return max(max(a, b), c);
	}

	inline int min(int a, int b, int c)
	{
		return min(min(a, b), c);
	}

	inline int max(int a, int b, int c, int d)
	{
		return max(max(a, b), max(c, d));
	}

	inline int min(int a, int b, int c, int d)
	{
		return min(min(a, b), min(c, d));
	}

	template<class T>
	inline void swap(T &a, T &b)
	{
		T t = a;
		a = b;
		b = t;
	}

	inline int iround(float x)
	{
		return (int)floor(x + 0.5f);
	//	return _mm_cvtss_si32(_mm_load_ss(&x));   // FIXME: Demands SSE support
	}

	inline int ifloor(float x)
	{
		return (int)floor(x);
	}
	
	inline int ceilFix4(int x)
	{
		return (x + 0xF) & 0xFFFFFFF0;
	}

	inline int ceilInt4(int x)
	{
		return (x + 0xF) >> 4;
	}

	#define BITS(x)    ( \
	!!(x & 0x80000000) + \
	!!(x & 0xC0000000) + \
	!!(x & 0xE0000000) + \
	!!(x & 0xF0000000) + \
	!!(x & 0xF8000000) + \
	!!(x & 0xFC000000) + \
	!!(x & 0xFE000000) + \
	!!(x & 0xFF000000) + \
	!!(x & 0xFF800000) + \
	!!(x & 0xFFC00000) + \
	!!(x & 0xFFE00000) + \
	!!(x & 0xFFF00000) + \
	!!(x & 0xFFF80000) + \
	!!(x & 0xFFFC0000) + \
	!!(x & 0xFFFE0000) + \
	!!(x & 0xFFFF0000) + \
	!!(x & 0xFFFF8000) + \
	!!(x & 0xFFFFC000) + \
	!!(x & 0xFFFFE000) + \
	!!(x & 0xFFFFF000) + \
	!!(x & 0xFFFFF800) + \
	!!(x & 0xFFFFFC00) + \
	!!(x & 0xFFFFFE00) + \
	!!(x & 0xFFFFFF00) + \
	!!(x & 0xFFFFFF80) + \
	!!(x & 0xFFFFFFC0) + \
	!!(x & 0xFFFFFFE0) + \
	!!(x & 0xFFFFFFF0) + \
	!!(x & 0xFFFFFFF8) + \
	!!(x & 0xFFFFFFFC) + \
	!!(x & 0xFFFFFFFE) + \
	!!(x & 0xFFFFFFFF))

	inline int exp2(int x)
	{
		return 1 << x;
	}

	inline unsigned long log2(int x)
	{
		unsigned long y;

		_BitScanReverse(&y, x);

		return y;
	}

	inline int ilog2(float x)
	{
		unsigned int y = *(unsigned int*)&x;

		return ((y & 0x7F800000) >> 23) - 127;
	}

	inline float log2(float x)
	{
		unsigned int y = (*(unsigned int*)&x);

		return (float)((y & 0x7F800000) >> 23) - 127 + (float)((*(unsigned int*)&x) & 0x007FFFFF) / 16777216.0f;
	}

	inline bool isPow2(int x)
	{
		return (x & -x) == x;
	}

	template<class T>
	inline T clamp(T x, T a, T b)
	{
		if(x < a) x = a;
		if(x > b) x = b;

		return x;
	}

	inline int ceilPow2(int x)
	{
		int i = 1;

		while(i < x)
		{
			i <<= 1;
		}

		return i;
	}

	inline int floorDiv(int a, int b)
	{
		return a / b + ((a % b) >> 31);
	}

	inline int floorMod(int a, int b)
	{
		int r = a % b;
		return r + ((r >> 31) & b);
	}

	inline int ceilDiv(int a, int b)
	{
		return a / b - (-(a % b) >> 31);
	}

	inline int ceilMod(int a, int b)
	{
		int r = a % b;
		return r - ((-r >> 31) & b);
	}

	template<const int n>
	inline unsigned int unorm(float x)
	{
		const unsigned int max = 0xFFFFFFFF >> (32 - n);

		if(x > 1)
		{
			return max;
		}
		else if(x < 0)
		{
			return 0;
		}
		else
		{
			return (unsigned int)(max * x + 0.5f);
		}
	}

	template<const int n>
	inline int snorm(float x)
	{
		const unsigned int min = 0x80000000 >> (32 - n);
		const unsigned int max = 0xFFFFFFFF >> (32 - n + 1);
		const unsigned int range = 0xFFFFFFFF >> (32 - n);

		if(x > 0)
		{
			if(x > 1)
			{
				return max;
			}
			else
			{
				return (int)(max * x + 0.5f);
			}
		}
		else
		{
			if(x < -1)
			{
				return min;
			}
			else
			{
				return (int)(max * x - 0.5f) & range;
			}
		}
	}

	inline float sRGBtoLinear(float c)
	{
		if(c <= 0.04045f)
		{
			return c * 0.07739938f;   // 1.0f / 12.92f;
		}
		else
		{
			return powf((c + 0.055f) * 0.9478673f, 2.4f);   // 1.0f / 1.055f
		}
	}

	inline float linearToSRGB(float c)
	{
		if(c <= 0.0031308f)
		{
			return c * 12.92f;
		}
		else
		{
			return 1.055f * powf(c, 0.4166667f) - 0.055f;   // 1.0f / 2.4f
		}
	}

	int64_t FNV_1(const unsigned char *data, int size);   // Fowler-Noll-Vo hash function
}

#endif   // sw_Math_hpp
