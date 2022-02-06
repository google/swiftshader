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

#include "ShaderCore.hpp"
#include "Reactor/Reactor.hpp"

#include "benchmark/benchmark.h"

#include <vector>

using namespace rr;

BENCHMARK_MAIN();

// Macro that creates a lambda wrapper around the input overloaded function,
// creating a non-overload based on the args. This is useful for passing
// overloaded functions as template arguments.
// See https://stackoverflow.com/questions/25871381/c-overloaded-function-as-template-argument
#define LIFT(fname)                                          \
	[](auto &&...args) -> decltype(auto) {                   \
		return fname(std::forward<decltype(args)>(args)...); \
	}

template<typename Func, class... Args>
static void Transcendental1(benchmark::State &state, Func func, Args &&...args)
{
	const int REPS = state.range(0);

	FunctionT<void(float *, float *)> function;
	{
		Pointer<Float4> r = Pointer<Float>(function.Arg<0>());
		Pointer<Float4> a = Pointer<Float>(function.Arg<1>());

		for(int i = 0; i < REPS; i++)
		{
			r[i] = func(a[i], args...);
		}
	}

	auto routine = function("one");

	std::vector<float> r(REPS * 4);
	std::vector<float> a(REPS * 4, 1.0f);

	for(auto _ : state)
	{
		routine(r.data(), a.data());
	}
}

template<typename Func, class... Args>
static void Transcendental2(benchmark::State &state, Func func, Args &&...args)
{
	const int REPS = state.range(0);

	FunctionT<void(float *, float *, float *)> function;
	{
		Pointer<Float4> r = Pointer<Float>(function.Arg<0>());
		Pointer<Float4> a = Pointer<Float>(function.Arg<1>());
		Pointer<Float4> b = Pointer<Float>(function.Arg<2>());

		for(int i = 0; i < REPS; i++)
		{
			r[i] = func(a[i], b[i], args...);
		}
	}

	auto routine = function("two");

	std::vector<float> r(REPS * 4);
	std::vector<float> a(REPS * 4, 0.456f);
	std::vector<float> b(REPS * 4, 0.789f);

	for(auto _ : state)
	{
		routine(r.data(), a.data(), b.data());
	}
}

// No operation; just copy the input to the output, for use as a baseline.
static Float4 Nop(RValue<Float4> x)
{
	return x;
}

static const int REPS = 10;

BENCHMARK_CAPTURE(Transcendental1, Nop, Nop)->Arg(REPS);

BENCHMARK_CAPTURE(Transcendental1, rr_Sin, rr::Sin)->Arg(REPS);
BENCHMARK_CAPTURE(Transcendental1, sw_Sin, sw::Sin)->Arg(REPS);
BENCHMARK_CAPTURE(Transcendental1, rr_Cos, rr::Cos)->Arg(REPS);
BENCHMARK_CAPTURE(Transcendental1, sw_Cos, sw::Cos)->Arg(REPS);
BENCHMARK_CAPTURE(Transcendental1, rr_Tan, rr::Tan)->Arg(REPS);
BENCHMARK_CAPTURE(Transcendental1, sw_Tan, sw::Tan)->Arg(REPS);

BENCHMARK_CAPTURE(Transcendental1, rr_Asin, rr::Asin)->Arg(REPS);
BENCHMARK_CAPTURE(Transcendental1, sw_Asin_highpp, sw::Asin, false /* relaxedPrecision */)->Arg(REPS);
BENCHMARK_CAPTURE(Transcendental1, sw_Asin_relaxedp, sw::Asin, true /* relaxedPrecision */)->Arg(REPS);
BENCHMARK_CAPTURE(Transcendental1, rr_Acos, rr::Acos)->Arg(REPS);
BENCHMARK_CAPTURE(Transcendental1, sw_Acos_highp, sw::Acos, false /* relaxedPrecision */)->Arg(REPS);
BENCHMARK_CAPTURE(Transcendental1, sw_Acos_relaxedp, sw::Acos, true /* relaxedPrecision */)->Arg(REPS);

BENCHMARK_CAPTURE(Transcendental1, rr_Atan, rr::Atan)->Arg(REPS);
BENCHMARK_CAPTURE(Transcendental1, sw_Atan, sw::Atan)->Arg(REPS);
BENCHMARK_CAPTURE(Transcendental1, rr_Sinh, rr::Sinh)->Arg(REPS);
BENCHMARK_CAPTURE(Transcendental1, sw_Sinh, sw::Sinh)->Arg(REPS);
BENCHMARK_CAPTURE(Transcendental1, rr_Cosh, rr::Cosh)->Arg(REPS);
BENCHMARK_CAPTURE(Transcendental1, sw_Cosh, sw::Cosh)->Arg(REPS);
BENCHMARK_CAPTURE(Transcendental1, rr_Tanh, rr::Tanh)->Arg(REPS);
BENCHMARK_CAPTURE(Transcendental1, sw_Tanh, sw::Tanh)->Arg(REPS);

BENCHMARK_CAPTURE(Transcendental1, rr_Asinh, rr::Asinh)->Arg(REPS);
BENCHMARK_CAPTURE(Transcendental1, sw_Asinh, sw::Asinh)->Arg(REPS);
BENCHMARK_CAPTURE(Transcendental1, rr_Acosh, rr::Acosh)->Arg(REPS);
BENCHMARK_CAPTURE(Transcendental1, sw_Acosh, sw::Acosh)->Arg(REPS);
BENCHMARK_CAPTURE(Transcendental1, rr_Atanh, rr::Atanh)->Arg(REPS);
BENCHMARK_CAPTURE(Transcendental1, sw_Atanh, sw::Atanh)->Arg(REPS);
BENCHMARK_CAPTURE(Transcendental2, rr_Atan2, rr::Atan2)->Arg(REPS);
BENCHMARK_CAPTURE(Transcendental2, sw_Atan2, sw::Atan2)->Arg(REPS);

BENCHMARK_CAPTURE(Transcendental2, rr_Pow, rr::Pow)->Arg(REPS);
BENCHMARK_CAPTURE(Transcendental2, sw_Pow, sw::Pow)->Arg(REPS);
BENCHMARK_CAPTURE(Transcendental1, rr_Exp, rr::Exp)->Arg(REPS);
BENCHMARK_CAPTURE(Transcendental1, sw_Exp, sw::Exp)->Arg(REPS);
BENCHMARK_CAPTURE(Transcendental1, rr_Log, rr::Log)->Arg(REPS);
BENCHMARK_CAPTURE(Transcendental1, sw_Log, sw::Log)->Arg(REPS);
BENCHMARK_CAPTURE(Transcendental1, rr_Exp2, LIFT(rr::Exp2))->Arg(REPS);
BENCHMARK_CAPTURE(Transcendental1, sw_Exp2, LIFT(sw::Exp2))->Arg(REPS);
BENCHMARK_CAPTURE(Transcendental1, rr_Log2, LIFT(rr::Log2))->Arg(REPS);
BENCHMARK_CAPTURE(Transcendental1, sw_Log2, LIFT(sw::Log2))->Arg(REPS);

BENCHMARK_CAPTURE(Transcendental1, rr_Rcp_pp_exactAtPow2_true, LIFT(Rcp_pp), true)->Arg(REPS);
BENCHMARK_CAPTURE(Transcendental1, rr_Rcp_pp_exactAtPow2_false, LIFT(Rcp_pp), false)->Arg(REPS);
BENCHMARK_CAPTURE(Transcendental1, rr_RcpSqrt_pp, LIFT(RcpSqrt_pp))->Arg(REPS);
