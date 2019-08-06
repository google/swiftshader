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

#define YARN_REG_r0  0x00
#define YARN_REG_r1  0x08
#define YARN_REG_r16 0x10
#define YARN_REG_r17 0x18
#define YARN_REG_r18 0x20
#define YARN_REG_r19 0x28
#define YARN_REG_r20 0x30
#define YARN_REG_r21 0x38
#define YARN_REG_r22 0x40
#define YARN_REG_r23 0x48
#define YARN_REG_r24 0x50
#define YARN_REG_r25 0x58
#define YARN_REG_r26 0x60
#define YARN_REG_r27 0x68
#define YARN_REG_r28 0x70
#define YARN_REG_v8  0x78
#define YARN_REG_v9  0x80
#define YARN_REG_v10 0x88
#define YARN_REG_v11 0x90
#define YARN_REG_v12 0x98
#define YARN_REG_v13 0xa0
#define YARN_REG_v14 0xa8
#define YARN_REG_v15 0xb0
#define YARN_REG_SP  0xb8
#define YARN_REG_LR  0xc0

#if defined(__APPLE__)
#define YARN_ASM_SYMBOL(x) _##x
#else
#define YARN_ASM_SYMBOL(x) x
#endif

#ifndef BUILD_ASM

#include <stdint.h>

// Procedure Call Standard for the ARM 64-bit Architecture
// http://infocenter.arm.com/help/topic/com.arm.doc.ihi0055b/IHI0055B_aapcs64.pdf
struct yarn_fiber_context
{
    // parameter registers
    uintptr_t r0;
    uintptr_t r1;

    // special purpose registers
    uintptr_t r16;
    uintptr_t r17;
    uintptr_t r18; // platform specific (maybe inter-procedural state)

    // callee-saved registers
    uintptr_t r19;
    uintptr_t r20;
    uintptr_t r21;
    uintptr_t r22;
    uintptr_t r23;
    uintptr_t r24;
    uintptr_t r25;
    uintptr_t r26;
    uintptr_t r27;
    uintptr_t r28;

    uintptr_t v8;
    uintptr_t v9;
    uintptr_t v10;
    uintptr_t v11;
    uintptr_t v12;
    uintptr_t v13;
    uintptr_t v14;
    uintptr_t v15;

    uintptr_t SP; // stack pointer
    uintptr_t LR; // link register (R30)
};

#ifdef __cplusplus
#include <cstddef>
static_assert(offsetof(yarn_fiber_context, r0)  == YARN_REG_r0,  "Bad register offset");
static_assert(offsetof(yarn_fiber_context, r1)  == YARN_REG_r1,  "Bad register offset");
static_assert(offsetof(yarn_fiber_context, r16) == YARN_REG_r16, "Bad register offset");
static_assert(offsetof(yarn_fiber_context, r17) == YARN_REG_r17, "Bad register offset");
static_assert(offsetof(yarn_fiber_context, r18) == YARN_REG_r18, "Bad register offset");
static_assert(offsetof(yarn_fiber_context, r19) == YARN_REG_r19, "Bad register offset");
static_assert(offsetof(yarn_fiber_context, r20) == YARN_REG_r20, "Bad register offset");
static_assert(offsetof(yarn_fiber_context, r21) == YARN_REG_r21, "Bad register offset");
static_assert(offsetof(yarn_fiber_context, r22) == YARN_REG_r22, "Bad register offset");
static_assert(offsetof(yarn_fiber_context, r23) == YARN_REG_r23, "Bad register offset");
static_assert(offsetof(yarn_fiber_context, r24) == YARN_REG_r24, "Bad register offset");
static_assert(offsetof(yarn_fiber_context, r25) == YARN_REG_r25, "Bad register offset");
static_assert(offsetof(yarn_fiber_context, r26) == YARN_REG_r26, "Bad register offset");
static_assert(offsetof(yarn_fiber_context, r27) == YARN_REG_r27, "Bad register offset");
static_assert(offsetof(yarn_fiber_context, r28) == YARN_REG_r28, "Bad register offset");
static_assert(offsetof(yarn_fiber_context, v8)  == YARN_REG_v8,  "Bad register offset");
static_assert(offsetof(yarn_fiber_context, v9)  == YARN_REG_v9,  "Bad register offset");
static_assert(offsetof(yarn_fiber_context, v10) == YARN_REG_v10, "Bad register offset");
static_assert(offsetof(yarn_fiber_context, v11) == YARN_REG_v11, "Bad register offset");
static_assert(offsetof(yarn_fiber_context, v12) == YARN_REG_v12, "Bad register offset");
static_assert(offsetof(yarn_fiber_context, v13) == YARN_REG_v13, "Bad register offset");
static_assert(offsetof(yarn_fiber_context, v14) == YARN_REG_v14, "Bad register offset");
static_assert(offsetof(yarn_fiber_context, v15) == YARN_REG_v15, "Bad register offset");
static_assert(offsetof(yarn_fiber_context, SP)  == YARN_REG_SP,  "Bad register offset");
static_assert(offsetof(yarn_fiber_context, LR)  == YARN_REG_LR,  "Bad register offset");
#endif // __cplusplus

#endif // BUILD_ASM
