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

#define YARN_REG_RBX 0x00
#define YARN_REG_RBP 0x08
#define YARN_REG_R12 0x10
#define YARN_REG_R13 0x18
#define YARN_REG_R14 0x20
#define YARN_REG_R15 0x28
#define YARN_REG_RDI 0x30
#define YARN_REG_RSI 0x38
#define YARN_REG_RSP 0x40
#define YARN_REG_RIP 0x48

#if defined(__APPLE__)
#define YARN_ASM_SYMBOL(x) _##x
#else
#define YARN_ASM_SYMBOL(x) x
#endif

#ifndef BUILD_ASM

#include <stdint.h>

struct yarn_fiber_context
{
    // callee-saved registers
    uintptr_t RBX;
    uintptr_t RBP;
    uintptr_t R12;
    uintptr_t R13;
    uintptr_t R14;
    uintptr_t R15;

    // parameter registers
    uintptr_t RDI;
    uintptr_t RSI;

    // stack and instruction registers
    uintptr_t RSP;
    uintptr_t RIP;
};

#ifdef __cplusplus
#include <cstddef>
static_assert(offsetof(yarn_fiber_context, RBX) == YARN_REG_RBX, "Bad register offset");
static_assert(offsetof(yarn_fiber_context, RBP) == YARN_REG_RBP, "Bad register offset");
static_assert(offsetof(yarn_fiber_context, R12) == YARN_REG_R12, "Bad register offset");
static_assert(offsetof(yarn_fiber_context, R13) == YARN_REG_R13, "Bad register offset");
static_assert(offsetof(yarn_fiber_context, R14) == YARN_REG_R14, "Bad register offset");
static_assert(offsetof(yarn_fiber_context, R15) == YARN_REG_R15, "Bad register offset");
static_assert(offsetof(yarn_fiber_context, RDI) == YARN_REG_RDI, "Bad register offset");
static_assert(offsetof(yarn_fiber_context, RSI) == YARN_REG_RSI, "Bad register offset");
static_assert(offsetof(yarn_fiber_context, RSP) == YARN_REG_RSP, "Bad register offset");
static_assert(offsetof(yarn_fiber_context, RIP) == YARN_REG_RIP, "Bad register offset");
#endif // __cplusplus

#endif // BUILD_ASM
