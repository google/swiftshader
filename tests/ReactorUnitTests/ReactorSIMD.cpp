// Copyright 2022 The SwiftShader Authors. All Rights Reserved.
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

#include "Reactor.hpp"
#include "SIMD.hpp"

#include "gtest/gtest.h"

using namespace rr;

static std::string testName()
{
	auto info = ::testing::UnitTest::GetInstance()->current_test_info();
	return std::string{ info->test_suite_name() } + "_" + info->name();
}

TEST(ReactorSIMD, Add)
{
	ASSERT_GE(SIMD::Width, 4);

	constexpr int arrayLength = 1024;

	FunctionT<void(int *, int *, int *)> function;
	{
		Pointer<Int> r = Pointer<Int>(function.Arg<0>());
		Pointer<Int> a = Pointer<Int>(function.Arg<1>());
		Pointer<Int> b = Pointer<Int>(function.Arg<2>());

		For(Int i = 0, i < arrayLength, i += SIMD::Width)
		{
			SIMD::Int x = *Pointer<SIMD::Int>(&a[i]);
			SIMD::Int y = *Pointer<SIMD::Int>(&b[i]);

			SIMD::Int z = x + y;

			*Pointer<SIMD::Int>(&r[i]) = z;
		}
	}

	auto routine = function(testName().c_str());

	int r[arrayLength] = {};
	int a[arrayLength];
	int b[arrayLength];

	for(int i = 0; i < arrayLength; i++)
	{
		a[i] = i;
		b[i] = arrayLength + i;
	}

	routine(r, a, b);

	for(int i = 0; i < arrayLength; i++)
	{
		ASSERT_EQ(r[i], arrayLength + 2 * i);
	}
}

TEST(ReactorSIMD, Broadcast)
{
	FunctionT<void(int *, int)> function;
	{
		Pointer<Int> r = Pointer<Int>(function.Arg<0>());
		Int a = function.Arg<1>();

		SIMD::Int x = a;

		*Pointer<SIMD::Int>(r) = x;
	}

	auto routine = function(testName().c_str());

	std::vector<int> r(SIMD::Width);

	for(int a = -2; a <= 2; a++)
	{
		routine(r.data(), a);

		for(int i = 0; i < SIMD::Width; i++)
		{
			ASSERT_EQ(r[i], a);
		}
	}
}

TEST(ReactorSIMD, Intrinsics_Scatter)
{
	Function<Void(Pointer<Float> base, Pointer<Float4> val, Pointer<Int4> offsets)> function;
	{
		Pointer<Float> base = function.Arg<0>();
		Pointer<Float4> val = function.Arg<1>();
		Pointer<Int4> offsets = function.Arg<2>();

		auto mask = Int4(~0, ~0, ~0, ~0);
		unsigned int alignment = 1;
		Scatter(base, *val, *offsets, mask, alignment);
	}

	float buffer[16] = { 0 };

	constexpr auto elemSize = sizeof(buffer[0]);

	int offsets[] = {
		1 * elemSize,
		6 * elemSize,
		11 * elemSize,
		13 * elemSize
	};

	float val[4] = { 10, 60, 110, 130 };

	auto routine = function(testName().c_str());
	auto entry = (void (*)(float *, float *, int *))routine->getEntry();

	entry(buffer, val, offsets);

	EXPECT_EQ(buffer[offsets[0] / sizeof(buffer[0])], 10);
	EXPECT_EQ(buffer[offsets[1] / sizeof(buffer[0])], 60);
	EXPECT_EQ(buffer[offsets[2] / sizeof(buffer[0])], 110);
	EXPECT_EQ(buffer[offsets[3] / sizeof(buffer[0])], 130);
}

TEST(ReactorUnitTests, Intrinsics_Gather)
{
	Function<Void(Pointer<Float> base, Pointer<Int4> offsets, Pointer<Float4> result)> function;
	{
		Pointer<Float> base = function.Arg<0>();
		Pointer<Int4> offsets = function.Arg<1>();
		Pointer<Float4> result = function.Arg<2>();

		auto mask = Int4(~0, ~0, ~0, ~0);
		unsigned int alignment = 1;
		bool zeroMaskedLanes = true;
		*result = Gather(base, *offsets, mask, alignment, zeroMaskedLanes);
	}

	float buffer[] = {
		0, 10, 20, 30,
		40, 50, 60, 70,
		80, 90, 100, 110,
		120, 130, 140, 150
	};

	constexpr auto elemSize = sizeof(buffer[0]);

	int offsets[] = {
		1 * elemSize,
		6 * elemSize,
		11 * elemSize,
		13 * elemSize
	};

	auto routine = function(testName().c_str());
	auto entry = (void (*)(float *, int *, float *))routine->getEntry();

	float result[4] = {};
	entry(buffer, offsets, result);

	EXPECT_EQ(result[0], 10);
	EXPECT_EQ(result[1], 60);
	EXPECT_EQ(result[2], 110);
	EXPECT_EQ(result[3], 130);
}
