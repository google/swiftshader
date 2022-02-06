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
static void Transcedental1(benchmark::State &state, Func func, Args &&...args)
{
	FunctionT<float(float)> function;
	{
		Float a = function.Arg<0>();
		Float4 v{ a };
		Float4 r = func(v, args...);
		Return(Float(r.x));
	}

	auto routine = function("one");

	for(auto _ : state)
	{
		routine(1.f);
	}
}

template<typename Func, class... Args>
static void Transcedental2(benchmark::State &state, Func func, Args &&...args)
{
	FunctionT<float(float, float)> function;
	{
		Float a = function.Arg<0>();
		Float b = function.Arg<1>();
		Float4 v1{ a };
		Float4 v2{ b };
		Float4 r = func(v1, v2, args...);
		Return(Float(r.x));
	}

	auto routine = function("two");

	for(auto _ : state)
	{
		routine(0.456f, 0.789f);
	}
}

BENCHMARK_CAPTURE(Transcedental1, rr_Sin, rr::Sin);
BENCHMARK_CAPTURE(Transcedental1, sw_Sin, sw::Sin);
BENCHMARK_CAPTURE(Transcedental1, rr_Cos, rr::Cos);
BENCHMARK_CAPTURE(Transcedental1, sw_Cos, sw::Cos);
BENCHMARK_CAPTURE(Transcedental1, rr_Tan, rr::Tan);
BENCHMARK_CAPTURE(Transcedental1, sw_Tan, sw::Tan);

BENCHMARK_CAPTURE(Transcedental1, rr_Asin, rr::Asin);
BENCHMARK_CAPTURE(Transcedental1, rr_Asin_highpp, sw::Asin, false /* relaxedPrecision */);
BENCHMARK_CAPTURE(Transcedental1, rr_Asin_relaxedp, sw::Asin, true /* relaxedPrecision */);
BENCHMARK_CAPTURE(Transcedental1, rr_Acos, rr::Acos);
BENCHMARK_CAPTURE(Transcedental1, rr_Acos_highp, sw::Acos, false /* relaxedPrecision */);
BENCHMARK_CAPTURE(Transcedental1, rr_Acos_relaxedp, sw::Acos, true /* relaxedPrecision */);

BENCHMARK_CAPTURE(Transcedental1, rr_Atan, rr::Atan);
BENCHMARK_CAPTURE(Transcedental1, sw_Atan, sw::Atan);
BENCHMARK_CAPTURE(Transcedental1, rr_Sinh, rr::Sinh);
BENCHMARK_CAPTURE(Transcedental1, sw_Sinh, sw::Sinh);
BENCHMARK_CAPTURE(Transcedental1, rr_Cosh, rr::Cosh);
BENCHMARK_CAPTURE(Transcedental1, sw_Cosh, sw::Cosh);
BENCHMARK_CAPTURE(Transcedental1, rr_Tanh, rr::Tanh);
BENCHMARK_CAPTURE(Transcedental1, sw_Tanh, sw::Tanh);

BENCHMARK_CAPTURE(Transcedental1, rr_Asinh, rr::Asinh);
BENCHMARK_CAPTURE(Transcedental1, sw_Asinh, sw::Asinh);
BENCHMARK_CAPTURE(Transcedental1, rr_Acosh, rr::Acosh);
BENCHMARK_CAPTURE(Transcedental1, sw_Acosh, sw::Acosh);
BENCHMARK_CAPTURE(Transcedental1, rr_Atanh, rr::Atanh);
BENCHMARK_CAPTURE(Transcedental1, sw_Atanh, sw::Atanh);
BENCHMARK_CAPTURE(Transcedental2, rr_Atan2, rr::Atan2);
BENCHMARK_CAPTURE(Transcedental2, sw_Atan2, sw::Atan2);

BENCHMARK_CAPTURE(Transcedental2, rr_Pow, rr::Pow);
BENCHMARK_CAPTURE(Transcedental2, sw_Pow, sw::Pow);
BENCHMARK_CAPTURE(Transcedental1, rr_Exp, rr::Exp);
BENCHMARK_CAPTURE(Transcedental1, sw_Exp, sw::Exp);
BENCHMARK_CAPTURE(Transcedental1, rr_Log, rr::Log);
BENCHMARK_CAPTURE(Transcedental1, sw_Log, sw::Log);
BENCHMARK_CAPTURE(Transcedental1, rr_Exp2, LIFT(rr::Exp2));
BENCHMARK_CAPTURE(Transcedental1, sw_Exp2, LIFT(sw::Exp2));
BENCHMARK_CAPTURE(Transcedental1, rr_Log2, LIFT(rr::Log2));
BENCHMARK_CAPTURE(Transcedental1, sw_Log2, LIFT(sw::Log2));

BENCHMARK_CAPTURE(Transcedental1, rr_Rcp_pp_exactAtPow2_true, LIFT(Rcp_pp), true);
BENCHMARK_CAPTURE(Transcedental1, rr_Rcp_pp_exactAtPow2_false, LIFT(Rcp_pp), false);
BENCHMARK_CAPTURE(Transcedental1, rr_RcpSqrt_pp, LIFT(RcpSqrt_pp));
