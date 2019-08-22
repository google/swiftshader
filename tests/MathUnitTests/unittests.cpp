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

#include "System/Half.hpp"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <cstdlib>

using namespace sw;

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
