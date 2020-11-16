// Copyright 2020 The SwiftShader Authors. All Rights Reserved.
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

#include "Coroutine.hpp"
#include "Reactor.hpp"

#include "benchmark/benchmark.h"

BENCHMARK_MAIN();

class Coroutines : public benchmark::Fixture
{
public:
	void SetUp(const ::benchmark::State &state) {}

	void TearDown(const ::benchmark::State &state) {}
};

BENCHMARK_DEFINE_F(Coroutines, Fibonacci)
(benchmark::State &state)
{
	using namespace rr;

	if(!Caps.CoroutinesSupported)
	{
		state.SkipWithError("Coroutines are not supported");
		return;
	}

	Coroutine<int()> function;
	{
		Yield(Int(0));
		Yield(Int(1));
		Int current = 1;
		Int next = 1;
		While(true)
		{
			Yield(next);
			auto tmp = current + next;
			current = next;
			next = tmp;
		}
	}

	auto coroutine = function();

	const auto iterations = state.range(0);

	int out = 0;
	for(auto _ : state)
	{
		for(int64_t i = 0; i < iterations; i++)
		{
			coroutine->await(out);
		}
	}
}

BENCHMARK_REGISTER_F(Coroutines, Fibonacci)->RangeMultiplier(8)->Range(1, 0x1000000)->ArgName("iterations");
