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

// Minimal assembly implementations of fiber context switching for Unix-based
// platforms.
//
// Note: Unlike makecontext, swapcontext or the Windows fiber APIs, these
// assembly implementations *do not* save or restore signal masks,
// floating-point control or status registers, FS and GS segment registers,
// thread-local storage state nor any SIMD registers. This should not be a
// problem as the yarn scheduler requires fibers to be executed on a single
// thread.

#if defined(__x86_64__)
#include "OSFiber_asm_x64.h"
#elif defined(__i386__)
#include "OSFiber_asm_x86.h"
#elif defined(__aarch64__)
#include "OSFiber_asm_aarch64.h"
#elif defined(__arm__)
#include "OSFiber_asm_arm.h"
#else
#error "Unsupported target"
#endif

#include <functional>
#include <memory>

extern "C"
{

extern void yarn_fiber_set_target(yarn_fiber_context*, void* stack, uint32_t stack_size, void(*target)(void*), void* arg);
extern void yarn_fiber_swap(yarn_fiber_context* from, const yarn_fiber_context* to);

} // extern "C"

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
    static inline void run(OSFiber* self);

    yarn_fiber_context context;
    std::function<void()> target;
    std::unique_ptr<uint8_t[]> stack;
};

OSFiber* OSFiber::createFiberFromCurrentThread()
{
    auto out = new OSFiber();
    out->context = {};
    return out;
}

OSFiber* OSFiber::createFiber(size_t stackSize, const std::function<void()>& func)
{
    auto out = new OSFiber();
    out->context = {};
    out->target = func;
    out->stack = std::unique_ptr<uint8_t[]>(new uint8_t[stackSize]);
    yarn_fiber_set_target(&out->context, out->stack.get(), stackSize, reinterpret_cast<void (*)(void*)>(&OSFiber::run), out);
    return out;
}

void OSFiber::run(OSFiber* self)
{
    std::function<void()> func;
    std::swap(func, self->target);
    func();
}

void OSFiber::switchTo(OSFiber* fiber)
{
    yarn_fiber_swap(&context, &fiber->context);
}

}  // namespace yarn
