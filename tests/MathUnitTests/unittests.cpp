// Copyright 2019 The SwiftShader Authors. All Rights Reserved.
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

#include "System/CPUID.hpp"
#include "System/Half.hpp"
#include "System/Math.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <cstdlib>

using namespace sw;

// Returns the whole-number ULP error of `a` relative to `x`.
// Use the doouble-precision version below. This just illustrates the principle.
[[deprecated]] float ULP_32(float x, float a)
{
	// Flip the last mantissa bit to compute the 'unit in the last place' error.
	float x1 = bit_cast<float>(bit_cast<uint32_t>(x) ^ 0x00000001);
	float ulp = abs(x1 - x);

	return abs(a - x) / ulp;
}

double ULP_32(double x, double a)
{
	// binary64 has 52 mantissa bits, while binary32 has 23, so the ULP for the latter is 29 bits shifted.
	double x1 = bit_cast<double>(bit_cast<uint64_t>(x) ^ 0x0000000020000000ull);
	double ulp = abs(x1 - x);

	return abs(a - x) / ulp;
}

float ULP_16(float x, float a)
{
	// binary32 has 23 mantissa bits, while binary16 has 10, so the ULP for the latter is 13 bits shifted.
	double x1 = bit_cast<float>(bit_cast<uint32_t>(x) ^ 0x00002000);
	float ulp = abs(x1 - x);

	return abs(a - x) / ulp;
}

// ULP-32: 3.36676240, Vulkan margin: 1.0737335
float Exp2_legacy(float x)
{
	// This implementation is based on 2^(i + f) = 2^i * 2^f,
	// where i is the integer part of x and f is the fraction.

	// For 2^i we can put the integer part directly in the exponent of
	// the IEEE-754 floating-point number. Clamp to prevent overflow
	// past the representation of infinity.
	float x0 = x;
	// x0 = Min(x0, bit_cast<float>(int(0x43010000)));  // 129.00000e+0f
	// x0 = Max(x0, bit_cast<float>(int(0xC2FDFFFF)));  // -126.99999e+0f

	int i = (int)round(x0 - 0.5f);
	float ii = bit_cast<float>((i + int(127)) << 23);  // Add single-precision bias, and shift into exponent.

	// For the fractional part use a polynomial
	// which approximates 2^f in the 0 to 1 range.
	float f = x0 - float(i);
	float ff = bit_cast<float>(int(0x3AF61905));     // 1.8775767e-3f
	ff = ff * f + bit_cast<float>(int(0x3C134806));  // 8.9893397e-3f
	ff = ff * f + bit_cast<float>(int(0x3D64AA23));  // 5.5826318e-2f
	ff = ff * f + bit_cast<float>(int(0x3E75EAD4));  // 2.4015361e-1f
	ff = ff * f + bit_cast<float>(int(0x3F31727B));  // 6.9315308e-1f
	ff = ff * f + float(1.0f);

	return ii * ff;
}

TEST(MathTest, Exp2Exhaustive)
{
	CPUID::setDenormalsAreZero(true);
	CPUID::setFlushToZero(true);

	float worst_margin = 0;
	float worst_ulp = 0;
	float worst_x = 0;
	float worst_val = 0;
	float worst_ref = 0;

	for(float x = -10; x <= 10; x = inc(x))
	{
		float val = Exp2_legacy(x);

		double ref = exp2((double)x);
		float ulp = (float)ULP_32(ref, (double)val);

		float tolerance = (3 + 2 * abs(x));
		float margin = ulp / tolerance;

		if(margin > worst_margin)
		{
			worst_margin = margin;
			worst_ulp = ulp;
			worst_x = x;
			worst_val = val;
			worst_ref = ref;
		}
	}

	ASSERT_TRUE(worst_margin <= 1.0f);

	CPUID::setDenormalsAreZero(false);
	CPUID::setFlushToZero(false);
}

// Polynomial approximation of order 5 for sin(x * 2 * pi) in the range [-1/4, 1/4]
static float sin5(float x)
{
	// A * x^5 + B * x^3 + C * x
	// Exact at x = 0, 1/12, 1/6, 1/4, and their negatives, which correspond to x * 2 * pi = 0, pi/6, pi/3, pi/2
	const float A = (36288 - 20736 * sqrt(3)) / 5;
	const float B = 288 * sqrt(3) - 540;
	const float C = (47 - 9 * sqrt(3)) / 5;

	float x2 = x * x;

	return ((A * x2 + B) * x2 + C) * x;
}

TEST(MathTest, SinExhaustive)
{
	const float tolerance = powf(2.0f, -12.0f);  // Vulkan requires absolute error <= 2^−11 inside the range [−pi, pi]
	const float pi = 3.1415926535f;

	for(float x = -pi; x <= pi; x = inc(x))
	{
		// Range reduction and mirroring
		float x_2 = 0.25f - x * (0.5f / pi);
		float z = 0.25f - fabs(x_2 - round(x_2));

		float val = sin5(z);

		ASSERT_NEAR(val, sinf(x), tolerance);
	}
}

TEST(MathTest, CosExhaustive)
{
	const float tolerance = powf(2.0f, -12.0f);  // Vulkan requires absolute error <= 2^−11 inside the range [−pi, pi]
	const float pi = 3.1415926535f;

	for(float x = -pi; x <= pi; x = inc(x))
	{
		// Phase shift, range reduction, and mirroring
		float x_2 = x * (0.5f / pi);
		float z = 0.25f - fabs(x_2 - round(x_2));

		float val = sin5(z);

		ASSERT_NEAR(val, cosf(x), tolerance);
	}
}

TEST(MathTest, UnsignedFloat11_10)
{
	// Test the largest value which causes underflow to 0, and the smallest value
	// which produces a denormalized result.

	EXPECT_EQ(R11G11B10F::float32ToFloat11(bit_cast<float>(0x3500007F)), 0x0000);
	EXPECT_EQ(R11G11B10F::float32ToFloat11(bit_cast<float>(0x35000080)), 0x0001);

	EXPECT_EQ(R11G11B10F::float32ToFloat10(bit_cast<float>(0x3580003F)), 0x0000);
	EXPECT_EQ(R11G11B10F::float32ToFloat10(bit_cast<float>(0x35800040)), 0x0001);
}

// Clamps to the [0, hi] range. NaN input produces 0, hi must be non-NaN.
float clamp0hi(float x, float hi)
{
	// If x=NaN, x > 0 will compare false and we return 0.
	if(!(x > 0))
	{
		return 0;
	}

	// x is non-NaN at this point, so std::min() is safe for non-NaN hi.
	return std::min(x, hi);
}

unsigned int RGB9E5_reference(float r, float g, float b)
{
	// Vulkan 1.1.117 section 15.2.1 RGB to Shared Exponent Conversion

	// B is the exponent bias (15)
	constexpr int g_sharedexp_bias = 15;

	// N is the number of mantissa bits per component (9)
	constexpr int g_sharedexp_mantissabits = 9;

	// Emax is the maximum allowed biased exponent value (31)
	constexpr int g_sharedexp_maxexponent = 31;

	constexpr float g_sharedexp_max =
	    ((static_cast<float>(1 << g_sharedexp_mantissabits) - 1) /
	     static_cast<float>(1 << g_sharedexp_mantissabits)) *
	    static_cast<float>(1 << (g_sharedexp_maxexponent - g_sharedexp_bias));

	const float red_c = clamp0hi(r, g_sharedexp_max);
	const float green_c = clamp0hi(g, g_sharedexp_max);
	const float blue_c = clamp0hi(b, g_sharedexp_max);

	const float max_c = fmax(fmax(red_c, green_c), blue_c);
	const float exp_p = fmax(-g_sharedexp_bias - 1, floor(log2(max_c))) + 1 + g_sharedexp_bias;
	const int max_s = static_cast<int>(floor((max_c / exp2(exp_p - g_sharedexp_bias - g_sharedexp_mantissabits)) + 0.5f));
	const int exp_s = static_cast<int>((max_s < exp2(g_sharedexp_mantissabits)) ? exp_p : exp_p + 1);

	unsigned int R = static_cast<unsigned int>(floor((red_c / exp2(exp_s - g_sharedexp_bias - g_sharedexp_mantissabits)) + 0.5f));
	unsigned int G = static_cast<unsigned int>(floor((green_c / exp2(exp_s - g_sharedexp_bias - g_sharedexp_mantissabits)) + 0.5f));
	unsigned int B = static_cast<unsigned int>(floor((blue_c / exp2(exp_s - g_sharedexp_bias - g_sharedexp_mantissabits)) + 0.5f));
	unsigned int E = exp_s;

	return (E << 27) | (B << 18) | (G << 9) | R;
}

TEST(MathTest, SharedExponentSparse)
{
	for(uint64_t i = 0; i < 0x0000000100000000; i += 0x400)
	{
		float f = bit_cast<float>(i);

		unsigned int ref = RGB9E5_reference(f, 0.0f, 0.0f);
		unsigned int val = RGB9E5(f, 0.0f, 0.0f);

		EXPECT_EQ(ref, val);
	}
}

TEST(MathTest, SharedExponentRandom)
{
	srand(0);

	unsigned int x = 0;
	unsigned int y = 0;
	unsigned int z = 0;

	for(int i = 0; i < 10000000; i++)
	{
		float r = bit_cast<float>(x);
		float g = bit_cast<float>(y);
		float b = bit_cast<float>(z);

		unsigned int ref = RGB9E5_reference(r, g, b);
		unsigned int val = RGB9E5(r, g, b);

		EXPECT_EQ(ref, val);

		x += rand();
		y += rand();
		z += rand();
	}
}

TEST(MathTest, SharedExponentExhaustive)
{
	for(uint64_t i = 0; i < 0x0000000100000000; i += 1)
	{
		float f = bit_cast<float>(i);

		unsigned int ref = RGB9E5_reference(f, 0.0f, 0.0f);
		unsigned int val = RGB9E5(f, 0.0f, 0.0f);

		EXPECT_EQ(ref, val);
	}
}
