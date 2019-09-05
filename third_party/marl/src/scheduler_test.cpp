// Copyright 2019 The Marl Authors.
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

#include "marl_test.h"

#include "marl/waitgroup.h"

TEST(WithoutBoundScheduler, SchedulerConstructAndDestruct) {
  auto scheduler = new marl::Scheduler();
  delete scheduler;
}

TEST(WithoutBoundScheduler, SchedulerBindGetUnbind) {
  auto scheduler = new marl::Scheduler();
  scheduler->bind();
  auto got = marl::Scheduler::get();
  ASSERT_EQ(scheduler, got);
  scheduler->unbind();
  got = marl::Scheduler::get();
  ASSERT_EQ(got, nullptr);
  delete scheduler;
}

TEST_P(WithBoundScheduler, SetAndGetWorkerThreadCount) {
  ASSERT_EQ(marl::Scheduler::get()->getWorkerThreadCount(),
            GetParam().numWorkerThreads);
}

TEST_P(WithBoundScheduler, DestructWithPendingTasks) {
  for (int i = 0; i < 10000; i++) {
    marl::schedule([] {});
  }
}

TEST_P(WithBoundScheduler, DestructWithPendingFibers) {
  marl::WaitGroup wg(1);
  for (int i = 0; i < 10000; i++) {
    marl::schedule([=] { wg.wait(); });
  }
  wg.done();

  auto scheduler = marl::Scheduler::get();
  scheduler->unbind();
  delete scheduler;

  // Rebind a new scheduler so WithBoundScheduler::TearDown() is happy.
  (new marl::Scheduler())->bind();
}

TEST_P(WithBoundScheduler, FibersResumeOnSameThread) {
  marl::WaitGroup fence(1);
  marl::WaitGroup wg(1000);
  for (int i = 0; i < 1000; i++) {
    marl::schedule([=] {
      auto threadID = std::this_thread::get_id();
      fence.wait();
      ASSERT_EQ(threadID, std::this_thread::get_id());
      wg.done();
    });
  }
  // just to try and get some tasks to yield.
  std::this_thread::sleep_for(std::chrono::milliseconds(10));
  fence.done();
  wg.wait();
}

TEST_P(WithBoundScheduler, FibersResumeOnSameStdThread) {
  auto scheduler = marl::Scheduler::get();

  marl::WaitGroup fence(1);
  marl::WaitGroup wg(1000);

  std::vector<std::thread> threads;
  for (int i = 0; i < 1000; i++) {
    threads.push_back(std::thread([=] {
      scheduler->bind();

      auto threadID = std::this_thread::get_id();
      fence.wait();
      ASSERT_EQ(threadID, std::this_thread::get_id());
      wg.done();

      scheduler->unbind();
    }));
  }
  // just to try and get some tasks to yield.
  std::this_thread::sleep_for(std::chrono::milliseconds(10));
  fence.done();
  wg.wait();

  for (auto& thread : threads) {
    thread.join();
  }
}