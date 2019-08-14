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

#if defined(__x86_64__)

#include "OSFiber_asm_x64.h"

// void yarn_fiber_swap(yarn_fiber_context* from, const yarn_fiber_context* to)
// rdi: from
// rsi: to
.text
.global YARN_ASM_SYMBOL(yarn_fiber_swap)
.align 4
YARN_ASM_SYMBOL(yarn_fiber_swap):

    // Save context 'from'

    // Store callee-preserved registers
    movq        %rbx, YARN_REG_RBX(%rdi)
    movq        %rbp, YARN_REG_RBP(%rdi)
    movq        %r12, YARN_REG_R12(%rdi)
    movq        %r13, YARN_REG_R13(%rdi)
    movq        %r14, YARN_REG_R14(%rdi)
    movq        %r15, YARN_REG_R15(%rdi)

    movq        (%rsp), %rcx             /* call stores the return address on the stack before jumping */
    movq        %rcx, YARN_REG_RIP(%rdi)
    leaq        8(%rsp), %rcx            /* skip the pushed return address */
    movq        %rcx, YARN_REG_RSP(%rdi)

    // Load context 'to'
    movq        %rsi, %r8

    // Load callee-preserved registers
    movq        YARN_REG_RBX(%r8), %rbx
    movq        YARN_REG_RBP(%r8), %rbp
    movq        YARN_REG_R12(%r8), %r12
    movq        YARN_REG_R13(%r8), %r13
    movq        YARN_REG_R14(%r8), %r14
    movq        YARN_REG_R15(%r8), %r15

    // Load first two call parameters
    movq        YARN_REG_RDI(%r8), %rdi
    movq        YARN_REG_RSI(%r8), %rsi

    // Load stack pointer
    movq        YARN_REG_RSP(%r8), %rsp

    // Load instruction pointer, and jump
    movq        YARN_REG_RIP(%r8), %rcx
    jmp         *%rcx

#endif // defined(__x86_64__)
