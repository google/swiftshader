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
