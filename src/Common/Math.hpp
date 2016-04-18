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

#ifndef sw_Math_hpp
#define sw_Math_hpp

#include "Types.hpp"

#include <cmath>
#if defined(_MSC_VER)
	#include <intrin.h>
#endif

namespace sw
{
	using std::abs;

	#undef min
	#undef max

	template<class T>
	inline T max(T a, T b)
	{
		return a > b ? a : b;
	}

	template<class T>
	inline T min(T a, T b)
	{
		return a < b ? a : b;
	}

	template<class T>
	inline T max(T a, T b, T c)
	{
		return max(max(a, b), c);
	}

	template<class T>
	inline T min(T a, T b, T c)
	{
		return min(min(a, b), c);
	}

	template<class T>
	inline T max(T a, T b, T c, T d)
	{
		return max(max(a, b), max(c, d));
	}

	template<class T>
	inline T min(T a, T b, T c, T d)
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
	!!((x) & 0x80000000) + \
	!!((x) & 0xC0000000) + \
	!!((x) & 0xE0000000) + \
	!!((x) & 0xF0000000) + \
	!!((x) & 0xF8000000) + \
	!!((x) & 0xFC000000) + \
	!!((x) & 0xFE000000) + \
	!!((x) & 0xFF000000) + \
	!!((x) & 0xFF800000) + \
	!!((x) & 0xFFC00000) + \
	!!((x) & 0xFFE00000) + \
	!!((x) & 0xFFF00000) + \
	!!((x) & 0xFFF80000) + \
	!!((x) & 0xFFFC0000) + \
	!!((x) & 0xFFFE0000) + \
	!!((x) & 0xFFFF0000) + \
	!!((x) & 0xFFFF8000) + \
	!!((x) & 0xFFFFC000) + \
	!!((x) & 0xFFFFE000) + \
	!!((x) & 0xFFFFF000) + \
	!!((x) & 0xFFFFF800) + \
	!!((x) & 0xFFFFFC00) + \
	!!((x) & 0xFFFFFE00) + \
	!!((x) & 0xFFFFFF00) + \
	!!((x) & 0xFFFFFF80) + \
	!!((x) & 0xFFFFFFC0) + \
	!!((x) & 0xFFFFFFE0) + \
	!!((x) & 0xFFFFFFF0) + \
	!!((x) & 0xFFFFFFF8) + \
	!!((x) & 0xFFFFFFFC) + \
	!!((x) & 0xFFFFFFFE) + \
	!!((x) & 0xFFFFFFFF))

	#define MAX(x, y) ((x) > (y) ? (x) : (y))
	#define MIN(x, y) ((x) < (y) ? (x) : (y))

	inline float exp2(float x)
	{
		return exp2f(x);
	}

	inline int exp2(int x)
	{
		return 1 << x;
	}

	inline unsigned long log2(int x)
	{
		#if defined(_MSC_VER)
			unsigned long y;
			_BitScanReverse(&y, x);
			return y;
		#else
			return 31 - __builtin_clz(x);
		#endif
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

	inline float clamp01(float x)
	{
		return clamp(x, 0.0f, 1.0f);
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
		static const unsigned int max = 0xFFFFFFFF >> (32 - n);
		static const float maxf = static_cast<float>(max);

		if(x >= 1.0f)
		{
			return max;
		}
		else if(x <= 0.0f)
		{
			return 0;
		}
		else
		{
			return static_cast<unsigned int>(maxf * x + 0.5f);
		}
	}

	template<const int n>
	inline int snorm(float x)
	{
		static const unsigned int min = 0x80000000 >> (32 - n);
		static const unsigned int max = 0xFFFFFFFF >> (32 - n + 1);
		static const float maxf = static_cast<float>(max);
		static const unsigned int range = 0xFFFFFFFF >> (32 - n);

		if(x >= 0.0f)
		{
			if(x >= 1.0f)
			{
				return max;
			}
			else
			{
				return static_cast<int>(maxf * x + 0.5f);
			}
		}
		else
		{
			if(x <= -1.0f)
			{
				return min;
			}
			else
			{
				return static_cast<int>(maxf * x - 0.5f) & range;
			}
		}
	}

	template<const int n>
	inline unsigned int ucast(float x)
	{
		static const unsigned int max = 0xFFFFFFFF >> (32 - n);
		static const float maxf = static_cast<float>(max);

		if(x >= maxf)
		{
			return max;
		}
		else if(x <= 0.0f)
		{
			return 0;
		}
		else
		{
			return static_cast<unsigned int>(x + 0.5f);
		}
	}

	template<const int n>
	inline int scast(float x)
	{
		static const unsigned int min = 0x80000000 >> (32 - n);
		static const unsigned int max = 0xFFFFFFFF >> (32 - n + 1);
		static const float maxf = static_cast<float>(max);
		static const unsigned int range = 0xFFFFFFFF >> (32 - n);

		if(x > 0.0f)
		{
			if(x >= maxf)
			{
				return max;
			}
			else
			{
				return static_cast<int>(maxf * x + 0.5f);
			}
		}
		else
		{
			if(x <= -1.0f)
			{
				return min;
			}
			else
			{
				return static_cast<int>(maxf * x - 0.5f) & range;
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

	unsigned char sRGB8toLinear8(unsigned char value);

	uint64_t FNV_1a(const unsigned char *data, int size);   // Fowler-Noll-Vo hash function

	// Round up to the next multiple of alignment
	inline unsigned int align(unsigned int value, unsigned int alignment)
	{
		return ((value + alignment - 1) / alignment) * alignment;
	}

	inline int clampToSignedInt(unsigned int x)
	{
		return static_cast<int>(min(x, 0x7FFFFFFFu));
	}

	class RGB9E5Data
	{
		unsigned int R : 9;
		unsigned int G : 9;
		unsigned int B : 9;
		unsigned int E : 5;

	public:
		void toRGBFloats(float* rgb) const
		{
			static const float Offset = -24.0f; // Exponent Bias (15) + Number of mantissa bits per component (9) = 24

			const float factor = powf(2.0f, static_cast<float>(E) + Offset);
			rgb[0] = static_cast<float>(R) * factor;
			rgb[1] = static_cast<float>(G) * factor;
			rgb[2] = static_cast<float>(B) * factor;
		}
	};

	class R11G11B10FData
	{
		unsigned int R : 11;
		unsigned int G : 11;
		unsigned int B : 10;

		static inline float float11ToFloat32(unsigned short fp11)
		{
			unsigned short exponent = (fp11 >> 6) & 0x1F;
			unsigned short mantissa = fp11 & 0x3F;

			unsigned int output;
			if(exponent == 0x1F)
			{
				// INF or NAN
				output = 0x7f800000 | (mantissa << 17);
			}
			else
			{
				if(exponent != 0)
				{
					// normalized
				}
				else if(mantissa != 0)
				{
					// The value is denormalized
					exponent = 1;

					do
					{
						exponent--;
						mantissa <<= 1;
					} while((mantissa & 0x40) == 0);

					mantissa = mantissa & 0x3F;
				}
				else // The value is zero
				{
					exponent = static_cast<unsigned short>(-112);
				}

				output = ((exponent + 112) << 23) | (mantissa << 17);
			}

			return *(float*)(&output);
		}

		static inline float float10ToFloat32(unsigned short fp10)
		{
			unsigned short exponent = (fp10 >> 5) & 0x1F;
			unsigned short mantissa = fp10 & 0x1F;

			unsigned int output;
			if(exponent == 0x1F)
			{
				// INF or NAN
				output = 0x7f800000 | (mantissa << 17);
			}
			else
			{
				if(exponent != 0)
				{
					// normalized
				}
				else if(mantissa != 0)
				{
					// The value is denormalized
					exponent = 1;

					do
					{
						exponent--;
						mantissa <<= 1;
					} while((mantissa & 0x20) == 0);

					mantissa = mantissa & 0x1F;
				}
				else // The value is zero
				{
					exponent = static_cast<unsigned short>(-112);
				}

				output = ((exponent + 112) << 23) | (mantissa << 18);
			}

			return *(float*)(&output);
		}

	public:
		void toRGBFloats(float* rgb) const
		{
			rgb[0] = float11ToFloat32(R);
			rgb[1] = float11ToFloat32(G);
			rgb[2] = float10ToFloat32(B);
		}
	};
}

#endif   // sw_Math_hpp
