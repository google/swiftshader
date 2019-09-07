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

#include "Yarn/Pool.hpp"
#include "Yarn/WaitGroup.hpp"

TEST_P(WithBoundScheduler, UnboundedPool_ConstructDestruct)
{
    yarn::UnboundedPool<int> pool;
}

TEST_P(WithBoundScheduler, BoundedPool_ConstructDestruct)
{
    yarn::BoundedPool<int, 10> pool;
}

TEST_P(WithBoundScheduler, UnboundedPool_Borrow)
{
    yarn::UnboundedPool<int> pool;
    for (int i = 0; i < 100; i++)
    {
        pool.borrow();
    }
}

TEST_P(WithBoundScheduler, UnboundedPool_ConcurrentBorrow)
{
    yarn::UnboundedPool<int> pool;
    constexpr int iterations = 10000;
    yarn::WaitGroup wg(iterations);
    for (int i = 0; i < iterations; i++) {
        yarn::schedule([=] { pool.borrow(); wg.done(); });
    }
    wg.wait();
}

TEST_P(WithBoundScheduler, BoundedPool_Borrow)
{
    yarn::BoundedPool<int, 100> pool;
    for (int i = 0; i < 100; i++)
    {
        pool.borrow();
    }
}

TEST_P(WithBoundScheduler, BoundedPool_ConcurrentBorrow)
{
    yarn::BoundedPool<int, 10> pool;
    constexpr int iterations = 10000;
    yarn::WaitGroup wg(iterations);
    for (int i = 0; i < iterations; i++) {
        yarn::schedule([=]
        {
            pool.borrow();
            wg.done();
        });
    }
    wg.wait();
}

struct CtorDtorCounter
{
    CtorDtorCounter() { ctor_count++; }
    ~CtorDtorCounter() { dtor_count++; }
    static void reset() { ctor_count = 0; dtor_count = 0; }
    static int ctor_count;
    static int dtor_count;
};

int CtorDtorCounter::ctor_count = -1;
int CtorDtorCounter::dtor_count = -1;

TEST_P(WithBoundScheduler, UnboundedPool_PolicyReconstruct)
{
    CtorDtorCounter::reset();
    yarn::UnboundedPool<CtorDtorCounter, yarn::PoolPolicy::Reconstruct> pool;
    ASSERT_EQ(CtorDtorCounter::ctor_count, 0);
    ASSERT_EQ(CtorDtorCounter::dtor_count, 0);
    {
        auto loan = pool.borrow();
        ASSERT_EQ(CtorDtorCounter::ctor_count, 1);
        ASSERT_EQ(CtorDtorCounter::dtor_count, 0);
    }
    ASSERT_EQ(CtorDtorCounter::ctor_count, 1);
    ASSERT_EQ(CtorDtorCounter::dtor_count, 1);
    {
        auto loan = pool.borrow();
        ASSERT_EQ(CtorDtorCounter::ctor_count, 2);
        ASSERT_EQ(CtorDtorCounter::dtor_count, 1);
    }
    ASSERT_EQ(CtorDtorCounter::ctor_count, 2);
    ASSERT_EQ(CtorDtorCounter::dtor_count, 2);
}

TEST_P(WithBoundScheduler, BoundedPool_PolicyReconstruct)
{
    CtorDtorCounter::reset();
    yarn::BoundedPool<CtorDtorCounter, 10, yarn::PoolPolicy::Reconstruct> pool;
    ASSERT_EQ(CtorDtorCounter::ctor_count, 0);
    ASSERT_EQ(CtorDtorCounter::dtor_count, 0);
    {
        auto loan = pool.borrow();
        ASSERT_EQ(CtorDtorCounter::ctor_count, 1);
        ASSERT_EQ(CtorDtorCounter::dtor_count, 0);
    }
    ASSERT_EQ(CtorDtorCounter::ctor_count, 1);
    ASSERT_EQ(CtorDtorCounter::dtor_count, 1);
    {
        auto loan = pool.borrow();
        ASSERT_EQ(CtorDtorCounter::ctor_count, 2);
        ASSERT_EQ(CtorDtorCounter::dtor_count, 1);
    }
    ASSERT_EQ(CtorDtorCounter::ctor_count, 2);
    ASSERT_EQ(CtorDtorCounter::dtor_count, 2);
}

TEST_P(WithBoundScheduler, UnboundedPool_PolicyPreserve)
{
    CtorDtorCounter::reset();
    {
        yarn::UnboundedPool<CtorDtorCounter, yarn::PoolPolicy::Preserve> pool;
        int ctor_count;
        {
            auto loan = pool.borrow();
            ASSERT_NE(CtorDtorCounter::ctor_count, 0);
            ASSERT_EQ(CtorDtorCounter::dtor_count, 0);
            ctor_count = CtorDtorCounter::ctor_count;
        }
        ASSERT_EQ(CtorDtorCounter::ctor_count, ctor_count);
        ASSERT_EQ(CtorDtorCounter::dtor_count, 0);
        {
            auto loan = pool.borrow();
            ASSERT_EQ(CtorDtorCounter::ctor_count, ctor_count);
            ASSERT_EQ(CtorDtorCounter::dtor_count, 0);
        }
        ASSERT_EQ(CtorDtorCounter::ctor_count, ctor_count);
        ASSERT_EQ(CtorDtorCounter::dtor_count, 0);
    }
    ASSERT_EQ(CtorDtorCounter::ctor_count, CtorDtorCounter::dtor_count);
}

TEST_P(WithBoundScheduler, BoundedPool_PolicyPreserve)
{
    CtorDtorCounter::reset();
    {
        yarn::BoundedPool<CtorDtorCounter, 10, yarn::PoolPolicy::Preserve> pool;
        int ctor_count;
        {
            auto loan = pool.borrow();
            ASSERT_NE(CtorDtorCounter::ctor_count, 0);
            ASSERT_EQ(CtorDtorCounter::dtor_count, 0);
            ctor_count = CtorDtorCounter::ctor_count;
        }
        ASSERT_EQ(CtorDtorCounter::ctor_count, ctor_count);
        ASSERT_EQ(CtorDtorCounter::dtor_count, 0);
        {
            auto loan = pool.borrow();
            ASSERT_EQ(CtorDtorCounter::ctor_count, ctor_count);
            ASSERT_EQ(CtorDtorCounter::dtor_count, 0);
        }
        ASSERT_EQ(CtorDtorCounter::ctor_count, ctor_count);
        ASSERT_EQ(CtorDtorCounter::dtor_count, 0);
    }
    ASSERT_EQ(CtorDtorCounter::ctor_count, CtorDtorCounter::dtor_count);
}
