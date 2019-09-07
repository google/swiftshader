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

#include "Yarn/Ticket.hpp"

TEST_P(WithBoundScheduler, Ticket)
{
    yarn::Ticket::Queue queue;

    constexpr int count = 1000;
    std::atomic<int> next = { 0 };
    int result[count] = {};

    for (int i = 0; i < count; i++)
    {
        auto ticket = queue.take();
        yarn::schedule([ticket, i, &result, &next] {
            ticket.wait();
            result[next++] = i;
            ticket.done();
        });
    }

    queue.take().wait();

    for (int i = 0; i < count; i++)
    {
        ASSERT_EQ(result[i], i);
    }
}
