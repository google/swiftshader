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

#include "marl/defer.h"
#include "marl/waitgroup.h"

#include <atomic>
#include <unordered_set>

TEST_F(WithoutBoundScheduler, SchedulerConstructAndDestruct) {
  auto scheduler = new marl::Scheduler();
  delete scheduler;
}

TEST_F(WithoutBoundScheduler, SchedulerBindGetUnbind) {
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
  std::atomic<int> counter = {0};
  for (int i = 0; i < 1000; i++) {
    marl::schedule([&] { counter++; });
  }

  auto scheduler = marl::Scheduler::get();
  scheduler->unbind();
  delete scheduler;

  // All scheduled tasks should be completed before the scheduler is destructed.
  ASSERT_EQ(counter.load(), 1000);

  // Rebind a new scheduler so WithBoundScheduler::TearDown() is happy.
  (new marl::Scheduler())->bind();
}

TEST_P(WithBoundScheduler, DestructWithPendingFibers) {
  std::atomic<int> counter = {0};

  marl::WaitGroup wg(1);
  for (int i = 0; i < 1000; i++) {
    marl::schedule([&] {
      wg.wait();
      counter++;
    });
  }

  // Schedule a task to unblock all the tasks scheduled above.
  // We assume that some of these tasks will not finish before the scheduler
  // destruction logic kicks in.
  marl::schedule([=] {
    wg.done();  // Ready, steady, go...
  });

  auto scheduler = marl::Scheduler::get();
  scheduler->unbind();
  delete scheduler;

  // All scheduled tasks should be completed before the scheduler is destructed.
  ASSERT_EQ(counter.load(), 1000);

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

  // on 32-bit OSs, excessive numbers of threads can run out of address space.
  constexpr auto num_threads = sizeof(void*) > 4 ? 1000 : 100;

  marl::WaitGroup fence(1);
  marl::WaitGroup wg(num_threads);

  std::vector<std::thread> threads;
  for (int i = 0; i < num_threads; i++) {
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

TEST_F(WithoutBoundScheduler, TasksOnlyScheduledOnWorkerThreads) {
  auto scheduler = std::unique_ptr<marl::Scheduler>(new marl::Scheduler());
  scheduler->bind();
  scheduler->setWorkerThreadCount(8);
  std::mutex mutex;
  std::unordered_set<std::thread::id> threads;
  marl::WaitGroup wg;
  for (int i = 0; i < 10000; i++) {
    wg.add(1);
    marl::schedule([&mutex, &threads, wg] {
      defer(wg.done());
      std::unique_lock<std::mutex> lock(mutex);
      threads.emplace(std::this_thread::get_id());
    });
  }
  wg.wait();

  ASSERT_LE(threads.size(), 8U);
  ASSERT_EQ(threads.count(std::this_thread::get_id()), 0U);

  scheduler->unbind();
}