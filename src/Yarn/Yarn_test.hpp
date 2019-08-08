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

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "Yarn/Scheduler.hpp"

// SchedulerParams holds Scheduler construction parameters for testing.
struct SchedulerParams
{
    int numWorkerThreads;

    friend std::ostream& operator<<(std::ostream& os, const SchedulerParams& params) {
        return os << "SchedulerParams{" <<
            "numWorkerThreads: " << params.numWorkerThreads <<
            "}";
    }
};

// WithoutBoundScheduler is a test fixture that does not bind a scheduler.
class WithoutBoundScheduler : public testing::Test {};

// WithBoundScheduler is a parameterized test fixture that performs tests with
// a bound scheduler using a number of different configurations.
class WithBoundScheduler : public testing::TestWithParam<SchedulerParams>
{
public:
    void SetUp() override
    {
        auto &params = GetParam();

        auto scheduler = new yarn::Scheduler();
        scheduler->bind();
        scheduler->setWorkerThreadCount(params.numWorkerThreads);
    }

    void TearDown() override
    {
        auto scheduler = yarn::Scheduler::get();
        scheduler->unbind();
        delete scheduler;
    }
};
