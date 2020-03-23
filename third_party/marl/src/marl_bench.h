// Copyright 2020 The Marl Authors.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "marl/scheduler.h"
#include "marl/thread.h"

#include "benchmark/benchmark.h"

class Schedule : public benchmark::Fixture {
 public:
  void SetUp(const ::benchmark::State&) {}

  void TearDown(const ::benchmark::State&) {}

  // run() creates a scheduler, sets the number of worker threads from the
  // benchmark arguments, calls f, then unbinds and destructs the scheduler.
  // F must be a function of the signature: void(int numTasks)
  template <typename F>
  void run(const ::benchmark::State& state, F&& f) {
    marl::Scheduler scheduler;
    scheduler.setWorkerThreadCount(numThreads(state));
    scheduler.bind();
    f(numTasks(state));
    scheduler.unbind();
  }

  // args() sets up the benchmark to run from [1 .. NumTasks] tasks (in 8^n
  // steps) across 0 worker threads to numLogicalCPUs.
  template <int NumTasks = 0x40000>
  static void args(benchmark::internal::Benchmark* b) {
    b->ArgNames({"tasks", "threads"});
    for (unsigned int tasks = 1U; tasks <= NumTasks; tasks *= 8) {
      for (unsigned int threads = 0U; threads <= marl::Thread::numLogicalCPUs();
           ++threads) {
        b->Args({tasks, threads});
      }
    }
  }

  // numThreads() return the number of threads in the benchmark run from the
  // state.
  static int numThreads(const ::benchmark::State& state) {
    return static_cast<int>(state.range(1));
  }

  // numTasks() return the number of tasks in the benchmark run from the state.
  static int numTasks(const ::benchmark::State& state) {
    return static_cast<int>(state.range(0));
  }

  // doSomeWork() performs some made up bit-shitfy algorithm that's difficult
  // for a compiler to optimize and produces consistent results.
  static uint32_t doSomeWork(uint32_t x);
};