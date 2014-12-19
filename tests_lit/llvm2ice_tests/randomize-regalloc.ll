; This is a smoke test of randomized register allocation.  The output
; of this test will change with changes to the random number generator
; implementation.

; RUN: %p2i -i %s --args -O2 -sz-seed=1 -randomize-regalloc \
; RUN:   | llvm-mc -triple=i686-none-nacl -filetype=obj \
; RUN:   | llvm-objdump -d --symbolize -x86-asm-syntax=intel - \
; RUN:   | FileCheck %s --check-prefix=CHECK_1
; RUN: %p2i -i %s --args -Om1 -sz-seed=1 -randomize-regalloc \
; RUN:   | llvm-mc -triple=i686-none-nacl -filetype=obj \
; RUN:   | llvm-objdump -d --symbolize -x86-asm-syntax=intel - \
; RUN:   | FileCheck %s --check-prefix=OPTM1_1

; Same tests but with a different seed, just to verify randomness.
; RUN: %p2i -i %s --args -O2 -sz-seed=123 -randomize-regalloc \
; RUN:   | llvm-mc -triple=i686-none-nacl -filetype=obj \
; RUN:   | llvm-objdump -d --symbolize -x86-asm-syntax=intel - \
; RUN:   | FileCheck %s --check-prefix=CHECK_123
; RUN: %p2i -i %s --args -Om1 -sz-seed=123 -randomize-regalloc \
; RUN:   | llvm-mc -triple=i686-none-nacl -filetype=obj \
; RUN:   | llvm-objdump -d --symbolize -x86-asm-syntax=intel - \
; RUN:   | FileCheck %s --check-prefix=OPTM1_123

define <4 x i32> @mul_v4i32(<4 x i32> %a, <4 x i32> %b) {
entry:
  %res = mul <4 x i32> %a, %b
  ret <4 x i32> %res
; OPTM1_1-LABEL: mul_v4i32:
; OPTM1_1: sub     esp, 60
; OPTM1_1-NEXT: movups  xmmword ptr [esp + 32], xmm0
; OPTM1_1-NEXT: movups  xmmword ptr [esp + 16], xmm1
; OPTM1_1-NEXT: movups  xmm0, xmmword ptr [esp + 32]
; OPTM1_1-NEXT: pshufd  xmm7, xmmword ptr [esp + 32], 49
; OPTM1_1-NEXT: pshufd  xmm4, xmmword ptr [esp + 16], 49
; OPTM1_1-NEXT: pmuludq xmm0, xmmword ptr [esp + 16]
; OPTM1_1-NEXT: pmuludq xmm7, xmm4
; OPTM1_1-NEXT: shufps  xmm0, xmm7, -120
; OPTM1_1-NEXT: pshufd  xmm0, xmm0, -40
; OPTM1_1-NEXT: movups  xmmword ptr [esp], xmm0
; OPTM1_1-NEXT: movups  xmm0, xmmword ptr [esp]
; OPTM1_1-NEXT: add     esp, 60
; OPTM1_1-NEXT: ret

; CHECK_1-LABEL: mul_v4i32:
; CHECK_1: movups  xmm6, xmm0
; CHECK_1-NEXT: pshufd  xmm0, xmm0, 49
; CHECK_1-NEXT: pshufd  xmm5, xmm1, 49
; CHECK_1-NEXT: pmuludq xmm6, xmm1
; CHECK_1-NEXT: pmuludq xmm0, xmm5
; CHECK_1-NEXT: shufps  xmm6, xmm0, -120
; CHECK_1-NEXT: pshufd  xmm6, xmm6, -40
; CHECK_1-NEXT: movups  xmm0, xmm6
; CHECK_1-NEXT: ret

; OPTM1_123-LABEL: mul_v4i32:
; OPTM1_123: sub     esp, 60
; OPTM1_123-NEXT: movups  xmmword ptr [esp + 32], xmm0
; OPTM1_123-NEXT: movups  xmmword ptr [esp + 16], xmm1
; OPTM1_123-NEXT: movups  xmm0, xmmword ptr [esp + 32]
; OPTM1_123-NEXT: pshufd  xmm2, xmmword ptr [esp + 32], 49
; OPTM1_123-NEXT: pshufd  xmm6, xmmword ptr [esp + 16], 49
; OPTM1_123-NEXT: pmuludq xmm0, xmmword ptr [esp + 16]
; OPTM1_123-NEXT: pmuludq xmm2, xmm6
; OPTM1_123-NEXT: shufps  xmm0, xmm2, -120
; OPTM1_123-NEXT: pshufd  xmm0, xmm0, -40
; OPTM1_123-NEXT: movups  xmmword ptr [esp], xmm0
; OPTM1_123-NEXT: movups  xmm0, xmmword ptr [esp]
; OPTM1_123-NEXT: add     esp, 60
; OPTM1_123-NEXT: ret

; CHECK_123-LABEL: mul_v4i32:
; CHECK_123: movups  xmm3, xmm0
; CHECK_123-NEXT: pshufd  xmm0, xmm0, 49
; CHECK_123-NEXT: pshufd  xmm7, xmm1, 49
; CHECK_123-NEXT: pmuludq xmm3, xmm1
; CHECK_123-NEXT: pmuludq xmm0, xmm7
; CHECK_123-NEXT: shufps  xmm3, xmm0, -120
; CHECK_123-NEXT: pshufd  xmm3, xmm3, -40
; CHECK_123-NEXT: movups  xmm0, xmm3
; CHECK_123-NEXT: ret
}

; ERRORS-NOT: ICE translation error
; DUMP-NOT: SZ
