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
using namespace sw;

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
BENCHMARK_CAPTURE(Transcendental1, sw_Sin_highp, sw::Sin, false /* relaxedPrecision */)->Arg(REPS);
BENCHMARK_CAPTURE(Transcendental1, sw_Sin_mediump, sw::Sin, true /* relaxedPrecision */)->Arg(REPS);
BENCHMARK_CAPTURE(Transcendental1, rr_Cos, rr::Cos)->Arg(REPS);
BENCHMARK_CAPTURE(Transcendental1, sw_Cos_highp, sw::Cos, false /* relaxedPrecision */)->Arg(REPS);
BENCHMARK_CAPTURE(Transcendental1, sw_Cos_mediump, sw::Cos, true /* relaxedPrecision */)->Arg(REPS);
BENCHMARK_CAPTURE(Transcendental1, rr_Tan, rr::Tan)->Arg(REPS);
BENCHMARK_CAPTURE(Transcendental1, sw_Tan_highp, sw::Tan, false /* relaxedPrecision */)->Arg(REPS);
BENCHMARK_CAPTURE(Transcendental1, sw_Tan_mediump, sw::Tan, true /* relaxedPrecision */)->Arg(REPS);

BENCHMARK_CAPTURE(Transcendental1, rr_Asin, rr::Asin)->Arg(REPS);
BENCHMARK_CAPTURE(Transcendental1, sw_Asin_highp, sw::Asin, false /* relaxedPrecision */)->Arg(REPS);
BENCHMARK_CAPTURE(Transcendental1, sw_Asin_mediump, sw::Asin, true /* relaxedPrecision */)->Arg(REPS);
BENCHMARK_CAPTURE(Transcendental1, rr_Acos, rr::Acos)->Arg(REPS);
BENCHMARK_CAPTURE(Transcendental1, sw_Acos_highp, sw::Acos, false /* relaxedPrecision */)->Arg(REPS);
BENCHMARK_CAPTURE(Transcendental1, sw_Acos_mediump, sw::Acos, true /* relaxedPrecision */)->Arg(REPS);

BENCHMARK_CAPTURE(Transcendental1, rr_Atan, rr::Atan)->Arg(REPS);
BENCHMARK_CAPTURE(Transcendental1, sw_Atan_highp, sw::Atan, false /* relaxedPrecision */)->Arg(REPS);
BENCHMARK_CAPTURE(Transcendental1, sw_Atan_mediump, sw::Atan, true /* relaxedPrecision */)->Arg(REPS);
BENCHMARK_CAPTURE(Transcendental1, rr_Sinh, rr::Sinh)->Arg(REPS);
BENCHMARK_CAPTURE(Transcendental1, sw_Sinh_highp, sw::Sinh, false /* relaxedPrecision */)->Arg(REPS);
BENCHMARK_CAPTURE(Transcendental1, sw_Sinh_mediump, sw::Sinh, true /* relaxedPrecision */)->Arg(REPS);
BENCHMARK_CAPTURE(Transcendental1, rr_Cosh, rr::Cosh)->Arg(REPS);
BENCHMARK_CAPTURE(Transcendental1, sw_Cosh_highp, sw::Cosh, false /* relaxedPrecision */)->Arg(REPS);
BENCHMARK_CAPTURE(Transcendental1, sw_Cosh_mediump, sw::Cosh, true /* relaxedPrecision */)->Arg(REPS);
BENCHMARK_CAPTURE(Transcendental1, rr_Tanh, rr::Tanh)->Arg(REPS);
BENCHMARK_CAPTURE(Transcendental1, sw_Tanh_highp, sw::Tanh, false /* relaxedPrecision */)->Arg(REPS);
BENCHMARK_CAPTURE(Transcendental1, sw_Tanh_mediump, sw::Tanh, true /* relaxedPrecision */)->Arg(REPS);

BENCHMARK_CAPTURE(Transcendental1, rr_Asinh, rr::Asinh)->Arg(REPS);
BENCHMARK_CAPTURE(Transcendental1, sw_Asinh_highp, sw::Asinh, false /* relaxedPrecision */)->Arg(REPS);
BENCHMARK_CAPTURE(Transcendental1, sw_Asinh_mediump, sw::Asinh, true /* relaxedPrecision */)->Arg(REPS);
BENCHMARK_CAPTURE(Transcendental1, rr_Acosh, rr::Acosh)->Arg(REPS);
BENCHMARK_CAPTURE(Transcendental1, sw_Acosh_highp, sw::Acosh, false /* relaxedPrecision */)->Arg(REPS);
BENCHMARK_CAPTURE(Transcendental1, sw_Acosh_mediump, sw::Acosh, true /* relaxedPrecision */)->Arg(REPS);
BENCHMARK_CAPTURE(Transcendental1, rr_Atanh, rr::Atanh)->Arg(REPS);
BENCHMARK_CAPTURE(Transcendental1, sw_Atanh_highp, sw::Atanh, false /* relaxedPrecision */)->Arg(REPS);
BENCHMARK_CAPTURE(Transcendental1, sw_Atanh_mediump, sw::Atanh, true /* relaxedPrecision */)->Arg(REPS);
BENCHMARK_CAPTURE(Transcendental2, rr_Atan2, rr::Atan2)->Arg(REPS);
BENCHMARK_CAPTURE(Transcendental2, sw_Atan2_highp, sw::Atan2, false /* relaxedPrecision */)->Arg(REPS);
BENCHMARK_CAPTURE(Transcendental2, sw_Atan2_mediump, sw::Atan2, true /* relaxedPrecision */)->Arg(REPS);

BENCHMARK_CAPTURE(Transcendental2, rr_Pow, rr::Pow)->Arg(REPS);
BENCHMARK_CAPTURE(Transcendental2, sw_Pow_highp, sw::Pow<Highp>)->Arg(REPS);
BENCHMARK_CAPTURE(Transcendental2, sw_Pow_mediump, sw::Pow<Mediump>)->Arg(REPS);
BENCHMARK_CAPTURE(Transcendental1, rr_Exp, rr::Exp)->Arg(REPS);
BENCHMARK_CAPTURE(Transcendental1, sw_Exp_highp, sw::Exp, false /* relaxedPrecision */)->Arg(REPS);
BENCHMARK_CAPTURE(Transcendental1, sw_Exp_mediump, sw::Exp, true /* relaxedPrecision */)->Arg(REPS);
BENCHMARK_CAPTURE(Transcendental1, rr_Log, rr::Log)->Arg(REPS);
BENCHMARK_CAPTURE(Transcendental1, sw_Log_highp, sw::Log, false /* relaxedPrecision */)->Arg(REPS);
BENCHMARK_CAPTURE(Transcendental1, sw_Log_mediump, sw::Log, true /* relaxedPrecision */)->Arg(REPS);
BENCHMARK_CAPTURE(Transcendental1, rr_Exp2, LIFT(rr::Exp2))->Arg(REPS);
BENCHMARK_CAPTURE(Transcendental1, sw_Exp2_highp, LIFT(sw::Exp2), false /* relaxedPrecision */)->Arg(REPS);
BENCHMARK_CAPTURE(Transcendental1, sw_Exp2_mediump, LIFT(sw::Exp2), true /* relaxedPrecision */)->Arg(REPS);
BENCHMARK_CAPTURE(Transcendental1, rr_Log2, LIFT(rr::Log2))->Arg(REPS);
BENCHMARK_CAPTURE(Transcendental1, sw_Log2_highp, LIFT(sw::Log2), false /* relaxedPrecision */)->Arg(REPS);
BENCHMARK_CAPTURE(Transcendental1, sw_Log2_mediump, LIFT(sw::Log2), true /* relaxedPrecision */)->Arg(REPS);
