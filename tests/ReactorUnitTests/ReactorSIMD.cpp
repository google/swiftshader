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
