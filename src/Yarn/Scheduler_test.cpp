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

#include "Yarn_test.hpp"

#include "Yarn/WaitGroup.hpp"

TEST(WithoutBoundScheduler, SchedulerConstructAndDestruct)
{
    auto scheduler = new yarn::Scheduler();
    delete scheduler;
}

TEST(WithoutBoundScheduler, SchedulerBindGetUnbind)
{
    auto scheduler = new yarn::Scheduler();
    scheduler->bind();
    auto got = yarn::Scheduler::get();
    ASSERT_EQ(scheduler, got);
    scheduler->unbind();
    got = yarn::Scheduler::get();
    ASSERT_EQ(got, nullptr);
    delete scheduler;
}

TEST_P(WithBoundScheduler, SetAndGetWorkerThreadCount)
{
    ASSERT_EQ(yarn::Scheduler::get()->getWorkerThreadCount(), GetParam().numWorkerThreads);
}

TEST_P(WithBoundScheduler, DestructWithPendingTasks)
{
    for (int i = 0; i < 10000; i++)
    {
        yarn::schedule([] {});
    }
}

TEST_P(WithBoundScheduler, DestructWithPendingFibers)
{
    yarn::WaitGroup wg(1);
    for (int i = 0; i < 10000; i++)
    {
        yarn::schedule([=] { wg.wait(); });
    }
    wg.done();

    auto scheduler = yarn::Scheduler::get();
    scheduler->unbind();
    delete scheduler;

    // Rebind a new scheduler so WithBoundScheduler::TearDown() is happy.
    (new yarn::Scheduler())->bind();
}

TEST_P(WithBoundScheduler, FibersResumeOnSameYarnThread)
{
    yarn::WaitGroup fence(1);
    yarn::WaitGroup wg(1000);
    for (int i = 0; i < 1000; i++)
    {
        yarn::schedule([=] {
            auto threadID = std::this_thread::get_id();
            fence.wait();
            ASSERT_EQ(threadID, std::this_thread::get_id());
            wg.done();
        });
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(10)); // just to try and get some tasks to yield.
    fence.done();
    wg.wait();
}

TEST_P(WithBoundScheduler, FibersResumeOnSameStdThread)
{
    auto scheduler = yarn::Scheduler::get();

    yarn::WaitGroup fence(1);
    yarn::WaitGroup wg(1000);

    std::vector<std::thread> threads;
    for (int i = 0; i < 1000; i++)
    {
        threads.push_back(std::thread([=] {
            scheduler->bind();

            auto threadID = std::this_thread::get_id();
            fence.wait();
            ASSERT_EQ(threadID, std::this_thread::get_id());
            wg.done();

            scheduler->unbind();
        }));
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(10)); // just to try and get some tasks to yield.
    fence.done();
    wg.wait();

    for (auto& thread : threads)
    {
        thread.join();
    }
}