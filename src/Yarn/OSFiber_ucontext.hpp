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

#if !defined(_XOPEN_SOURCE)
// This must come before other #includes, otherwise we'll end up with ucontext_t
// definition mismatches, leading to memory corruption hilarity.
#define _XOPEN_SOURCE
#endif //  !defined(_XOPEN_SOURCE)

#include "Debug.hpp"

#include <functional>
#include <memory>

#include <ucontext.h>

#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#endif // defined(__clang__)

namespace yarn {

class OSFiber
{
public:
    // createFiberFromCurrentThread() returns a fiber created from the current
    // thread.
    static inline OSFiber* createFiberFromCurrentThread();

    // createFiber() returns a new fiber with the given stack size that will
    // call func when switched to. func() must end by switching back to another
    // fiber, and must not return.
    static inline OSFiber* createFiber(size_t stackSize, const std::function<void()>& func);

    // switchTo() immediately switches execution to the given fiber.
    // switchTo() must be called on the currently executing fiber.
    inline void switchTo(OSFiber*);

private:
	std::unique_ptr<uint8_t[]> stack;
	ucontext_t context;
	std::function<void()> target;
};

OSFiber* OSFiber::createFiberFromCurrentThread()
{
	auto out = new OSFiber();
	out->context = {};
	getcontext(&out->context);
	return out;
}

OSFiber* OSFiber::createFiber(size_t stackSize, const std::function<void()>& func)
{
	union Args
	{
		OSFiber* self;
		struct { int a; int b; };
	};

	struct Target
	{
		static void Main(int a, int b)
		{
			Args u;
			u.a = a; u.b = b;
			std::function<void()> func;
			std::swap(func, u.self->target);
			func();
		}
	};

	auto out = new OSFiber();
	out->context = {};
	out->stack = std::unique_ptr<uint8_t[]>(new uint8_t[stackSize]);
	out->target = func;

	auto alignmentOffset = 15 - (reinterpret_cast<uintptr_t>(out->stack.get() + 15) & 15);
	auto res = getcontext(&out->context);
	YARN_ASSERT(res == 0, "getcontext() returned %d", int(res));
	out->context.uc_stack.ss_sp = out->stack.get() + alignmentOffset;
	out->context.uc_stack.ss_size = stackSize - alignmentOffset;
	out->context.uc_link = nullptr;

	Args args;
	args.self = out;
	makecontext(&out->context, reinterpret_cast<void(*)()>(&Target::Main), 2, args.a, args.b);

	return out;
}

void OSFiber::switchTo(OSFiber* fiber)
{
	auto res = swapcontext(&context, &fiber->context);
	YARN_ASSERT(res == 0, "swapcontext() returned %d", int(res));
}

}  // namespace yarn

#if defined(__clang__)
#pragma clang diagnostic pop
#endif // defined(__clang__)
