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

#include <math.h>
#if defined(_MSC_VER)
	#include <intrin.h>
#endif

namespace sw
{
	inline float abs(float x)
	{
		return fabsf(x);
	}

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

	unsigned char sRGB8toLinear8(unsigned char value);

	uint64_t FNV_1a(const unsigned char *data, int size);   // Fowler-Noll-Vo hash function

	// Round up to the next multiple of alignment
	inline unsigned int align(unsigned int value, unsigned int alignment)
	{
		return ((value + alignment - 1) / alignment) * alignment;
	}

	class RGB9E5Data
	{
		union
		{
			struct
			{
				unsigned int R : 9;
				unsigned int G : 9;
				unsigned int B : 9;
				unsigned int E : 5;
			};
			unsigned int uint;
		};

		// Exponent Bias
		static const int Bias = 15;

		// Number of mantissa bits per component
		static const int MantissaBits = 9;

	public:
		RGB9E5Data(float red, float green, float blue)
		{
			// Maximum allowed biased exponent value
			static const int MaxExponent = 31;

			static const float MaxValue = ((pow(2.0f, MantissaBits) - 1) / pow(2.0f, MantissaBits)) * pow(2.0f, MaxExponent - Bias);

			const float red_c = sw::max(0.0f, sw::min(MaxValue, red));
			const float green_c = sw::max(0.0f, sw::min(MaxValue, green));
			const float blue_c = sw::max(0.0f, sw::min(MaxValue, blue));

			const float max_c = sw::max(sw::max(red_c, green_c), blue_c);
			const float exp_p = sw::max(-Bias - 1.0f, floor(log(max_c))) + 1.0f + Bias;
			const int max_s = static_cast<int>(floor((max_c / (pow(2.0f, exp_p - Bias - MantissaBits))) + 0.5f));
			const int exp_s = static_cast<int>((max_s < pow(2.0f, MantissaBits)) ? exp_p : exp_p + 1);

			R = static_cast<unsigned int>(floor((red_c / (pow(2.0f, exp_s - Bias - MantissaBits))) + 0.5f));
			G = static_cast<unsigned int>(floor((green_c / (pow(2.0f, exp_s - Bias - MantissaBits))) + 0.5f));
			B = static_cast<unsigned int>(floor((blue_c / (pow(2.0f, exp_s - Bias - MantissaBits))) + 0.5f));
			E = exp_s;
		}

		void toRGBFloats(float *red, float *green, float *blue) const
		{
			*red = R * pow(2.0f, (int)E - Bias - MantissaBits);
			*green = G * pow(2.0f, (int)E - Bias - MantissaBits);
			*blue = B * pow(2.0f, (int)E - Bias - MantissaBits);
		}

		unsigned int toUInt() const
		{
			return uint;
		}
	};

	class R11G11B10FData
	{
		union
		{
			struct
			{
				unsigned int R : 11;
				unsigned int G : 11;
				unsigned int B : 10;
			};

			unsigned int uint;
		};

		static inline unsigned short float32ToFloat11(float fp32)
		{
			const unsigned int float32MantissaMask = 0x7FFFFF;
			const unsigned int float32ExponentMask = 0x7F800000;
			const unsigned int float32SignMask = 0x80000000;
			const unsigned int float32ValueMask = ~float32SignMask;
			const unsigned int float32ExponentFirstBit = 23;
			const unsigned int float32ExponentBias = 127;

			const unsigned short float11Max = 0x7BF;
			const unsigned short float11MantissaMask = 0x3F;
			const unsigned short float11ExponentMask = 0x7C0;
			const unsigned short float11BitMask = 0x7FF;
			const unsigned int float11ExponentBias = 14;

			const unsigned int float32Maxfloat11 = 0x477E0000;
			const unsigned int float32Minfloat11 = 0x38800000;

			const unsigned int float32Bits = *(unsigned int*)(&fp32);
			const bool float32Sign = (float32Bits & float32SignMask) == float32SignMask;

			unsigned int float32Val = float32Bits & float32ValueMask;

			if((float32Val & float32ExponentMask) == float32ExponentMask)
			{
				// INF or NAN
				if((float32Val & float32MantissaMask) != 0)
				{
					return float11ExponentMask | (((float32Val >> 17) | (float32Val >> 11) | (float32Val >> 6) | (float32Val)) & float11MantissaMask);
				}
				else if(float32Sign)
				{
					// -INF is clamped to 0 since float11 is positive only
					return 0;
				}
				else
				{
					return float11ExponentMask;
				}
			}
			else if(float32Sign)
			{
				// float11 is positive only, so clamp to zero
				return 0;
			}
			else if(float32Val > float32Maxfloat11)
			{
				// The number is too large to be represented as a float11, set to max
				return float11Max;
			}
			else
			{
				if(float32Val < float32Minfloat11)
				{
					// The number is too small to be represented as a normalized float11
					// Convert it to a denormalized value.
					const unsigned int shift = (float32ExponentBias - float11ExponentBias) - (float32Val >> float32ExponentFirstBit);
					float32Val = ((1 << float32ExponentFirstBit) | (float32Val & float32MantissaMask)) >> shift;
				}
				else
				{
					// Rebias the exponent to represent the value as a normalized float11
					float32Val += 0xC8000000;
				}

				return ((float32Val + 0xFFFF + ((float32Val >> 17) & 1)) >> 17) & float11BitMask;
			}
		}

		static inline unsigned short float32ToFloat10(float fp32)
		{
			const unsigned int float32MantissaMask = 0x7FFFFF;
			const unsigned int float32ExponentMask = 0x7F800000;
			const unsigned int float32SignMask = 0x80000000;
			const unsigned int float32ValueMask = ~float32SignMask;
			const unsigned int float32ExponentFirstBit = 23;
			const unsigned int float32ExponentBias = 127;

			const unsigned short float10Max = 0x3DF;
			const unsigned short float10MantissaMask = 0x1F;
			const unsigned short float10ExponentMask = 0x3E0;
			const unsigned short float10BitMask = 0x3FF;
			const unsigned int float10ExponentBias = 14;

			const unsigned int float32Maxfloat10 = 0x477C0000;
			const unsigned int float32Minfloat10 = 0x38800000;

			const unsigned int float32Bits = *(unsigned int*)(&fp32);
			const bool float32Sign = (float32Bits & float32SignMask) == float32SignMask;

			unsigned int float32Val = float32Bits & float32ValueMask;

			if((float32Val & float32ExponentMask) == float32ExponentMask)
			{
				// INF or NAN
				if((float32Val & float32MantissaMask) != 0)
				{
					return float10ExponentMask | (((float32Val >> 18) | (float32Val >> 13) | (float32Val >> 3) | (float32Val)) & float10MantissaMask);
				}
				else if(float32Sign)
				{
					// -INF is clamped to 0 since float11 is positive only
					return 0;
				}
				else
				{
					return float10ExponentMask;
				}
			}
			else if(float32Sign)
			{
				// float10 is positive only, so clamp to zero
				return 0;
			}
			else if(float32Val > float32Maxfloat10)
			{
				// The number is too large to be represented as a float11, set to max
				return float10Max;
			}
			else
			{
				if(float32Val < float32Minfloat10)
				{
					// The number is too small to be represented as a normalized float11
					// Convert it to a denormalized value.
					const unsigned int shift = (float32ExponentBias - float10ExponentBias) - (float32Val >> float32ExponentFirstBit);
					float32Val = ((1 << float32ExponentFirstBit) | (float32Val & float32MantissaMask)) >> shift;
				}
				else
				{
					// Rebias the exponent to represent the value as a normalized float11
					float32Val += 0xC8000000;
				}

				return ((float32Val + 0x1FFFF + ((float32Val >> 18) & 1)) >> 18) & float10BitMask;
			}
		}

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
		R11G11B10FData(float r, float g, float b)
		{
			R = float32ToFloat11(r);
			G = float32ToFloat11(g);
			B = float32ToFloat10(b);
		}

		void toRGBFloats(float *red, float *green, float *blue) const
		{
			*red = float11ToFloat32(R);
			*green = float11ToFloat32(G);
			*blue = float10ToFloat32(B);
		}

		unsigned int toUInt() const
		{
			return uint;
		}
	};
}

#endif   // sw_Math_hpp
