; This file checks support for comparing vector values with the icmp
; instruction.

; RUN: %llvm2ice -O2 --verbose none %s \
; RUN:   | llvm-mc -triple=i686-none-nacl -x86-asm-syntax=intel -filetype=obj \
; RUN:   | llvm-objdump -d --symbolize -x86-asm-syntax=intel - | FileCheck %s
; RUN: %llvm2ice -Om1 --verbose none %s \
; RUN:   | llvm-mc -triple=i686-none-nacl -x86-asm-syntax=intel -filetype=obj \
; RUN:   | llvm-objdump -d --symbolize -x86-asm-syntax=intel - | FileCheck %s
; RUN: %llvm2ice --verbose none %s | FileCheck --check-prefix=ERRORS %s
; RUN: %llvm2iceinsts %s | %szdiff %s | FileCheck --check-prefix=DUMP %s
; RUN: %llvm2iceinsts --pnacl %s | %szdiff %s \
; RUN:                           | FileCheck --check-prefix=DUMP %s

; Check that sext elimination occurs when the result of the comparison
; instruction is alrady sign extended.  Sign extension to 4 x i32 uses
; the pslld instruction.
define <4 x i32> @test_sext_elimination(<4 x i32> %a, <4 x i32> %b) {
entry:
  %res.trunc = icmp eq <4 x i32> %a, %b
  %res = sext <4 x i1> %res.trunc to <4 x i32>
  ret <4 x i32> %res
; CHECK-LABEL: test_sext_elimination:
; CHECK: pcmpeqd
; CHECK-NOT: pslld
}

define <4 x i1> @test_icmp_v4i32_eq(<4 x i32> %a, <4 x i32> %b) {
entry:
  %res = icmp eq <4 x i32> %a, %b
  ret <4 x i1> %res
; CHECK-LABEL: test_icmp_v4i32_eq:
; CHECK: pcmpeqd
}

define <4 x i1> @test_icmp_v4i32_ne(<4 x i32> %a, <4 x i32> %b) {
entry:
  %res = icmp ne <4 x i32> %a, %b
  ret <4 x i1> %res
; CHECK-LABEL: test_icmp_v4i32_ne:
; CHECK: pcmpeqd
; CHECK: pxor
}

define <4 x i1> @test_icmp_v4i32_sgt(<4 x i32> %a, <4 x i32> %b) {
entry:
  %res = icmp sgt <4 x i32> %a, %b
  ret <4 x i1> %res
; CHECK: pcmpgtd
}

define <4 x i1> @test_icmp_v4i32_sle(<4 x i32> %a, <4 x i32> %b) {
entry:
  %res = icmp sle <4 x i32> %a, %b
  ret <4 x i1> %res
; CHECK-LABEL: test_icmp_v4i32_sle:
; CHECK: pcmpgtd
; CHECK: pxor
}

define <4 x i1> @test_icmp_v4i32_slt(<4 x i32> %a, <4 x i32> %b) {
entry:
  %res = icmp slt <4 x i32> %a, %b
  ret <4 x i1> %res
; CHECK-LABEL: test_icmp_v4i32_slt:
; CHECK: pcmpgtd
}

define <4 x i1> @test_icmp_v4i32_uge(<4 x i32> %a, <4 x i32> %b) {
entry:
  %res = icmp uge <4 x i32> %a, %b
  ret <4 x i1> %res
; CHECK-LABEL: test_icmp_v4i32_uge:
; CHECK: pxor
; CHECK: pcmpgtd
; CHECK: pxor
}

define <4 x i1> @test_icmp_v4i32_ugt(<4 x i32> %a, <4 x i32> %b) {
entry:
  %res = icmp ugt <4 x i32> %a, %b
  ret <4 x i1> %res
; CHECK-LABEL: test_icmp_v4i32_ugt:
; CHECK: pxor
; CHECK: pcmpgtd
}

define <4 x i1> @test_icmp_v4i32_ule(<4 x i32> %a, <4 x i32> %b) {
entry:
  %res = icmp ule <4 x i32> %a, %b
  ret <4 x i1> %res
; CHECK-LABEL: test_icmp_v4i32_ule:
; CHECK: pxor
; CHECK: pcmpgtd
; CHECK: pxor
}

define <4 x i1> @test_icmp_v4i32_ult(<4 x i32> %a, <4 x i32> %b) {
entry:
  %res = icmp ult <4 x i32> %a, %b
  ret <4 x i1> %res
; CHECK-LABEL: test_icmp_v4i32_ult:
; CHECK: pxor
; CHECK: pcmpgtd
}

define <4 x i1> @test_icmp_v4i1_eq(<4 x i1> %a, <4 x i1> %b) {
entry:
  %res = icmp eq <4 x i1> %a, %b
  ret <4 x i1> %res
; CHECK-LABEL: test_icmp_v4i1_eq:
; CHECK: pcmpeqd
}

define <4 x i1> @test_icmp_v4i1_ne(<4 x i1> %a, <4 x i1> %b) {
entry:
  %res = icmp ne <4 x i1> %a, %b
  ret <4 x i1> %res
; CHECK-LABEL: test_icmp_v4i1_ne:
; CHECK: pcmpeqd
; CHECK: pxor
}

define <4 x i1> @test_icmp_v4i1_sgt(<4 x i1> %a, <4 x i1> %b) {
entry:
  %res = icmp sgt <4 x i1> %a, %b
  ret <4 x i1> %res
; CHECK-LABEL: test_icmp_v4i1_sgt:
; CHECK: pcmpgtd
}

define <4 x i1> @test_icmp_v4i1_sle(<4 x i1> %a, <4 x i1> %b) {
entry:
  %res = icmp sle <4 x i1> %a, %b
  ret <4 x i1> %res
; CHECK-LABEL: test_icmp_v4i1_sle:
; CHECK: pcmpgtd
; CHECK: pxor
}

define <4 x i1> @test_icmp_v4i1_slt(<4 x i1> %a, <4 x i1> %b) {
entry:
  %res = icmp slt <4 x i1> %a, %b
  ret <4 x i1> %res
; CHECK-LABEL: test_icmp_v4i1_slt:
; CHECK: pcmpgtd
}

define <4 x i1> @test_icmp_v4i1_uge(<4 x i1> %a, <4 x i1> %b) {
entry:
  %res = icmp uge <4 x i1> %a, %b
  ret <4 x i1> %res
; CHECK-LABEL: test_icmp_v4i1_uge:
; CHECK: pxor
; CHECK: pcmpgtd
; CHECK: pxor
}

define <4 x i1> @test_icmp_v4i1_ugt(<4 x i1> %a, <4 x i1> %b) {
entry:
  %res = icmp ugt <4 x i1> %a, %b
  ret <4 x i1> %res
; CHECK-LABEL: test_icmp_v4i1_ugt:
; CHECK: pxor
; CHECK: pcmpgtd
}

define <4 x i1> @test_icmp_v4i1_ule(<4 x i1> %a, <4 x i1> %b) {
entry:
  %res = icmp ule <4 x i1> %a, %b
  ret <4 x i1> %res
; CHECK-LABEL: test_icmp_v4i1_ule:
; CHECK: pxor
; CHECK: pcmpgtd
; CHECK: pxor
}

define <4 x i1> @test_icmp_v4i1_ult(<4 x i1> %a, <4 x i1> %b) {
entry:
  %res = icmp ult <4 x i1> %a, %b
  ret <4 x i1> %res
; CHECK-LABEL: test_icmp_v4i1_ult:
; CHECK: pxor
; CHECK: pcmpgtd
}

define <8 x i1> @test_icmp_v8i16_eq(<8 x i16> %a, <8 x i16> %b) {
entry:
  %res = icmp eq <8 x i16> %a, %b
  ret <8 x i1> %res
; CHECK-LABEL: test_icmp_v8i16_eq:
; CHECK: pcmpeqw
}

define <8 x i1> @test_icmp_v8i16_ne(<8 x i16> %a, <8 x i16> %b) {
entry:
  %res = icmp ne <8 x i16> %a, %b
  ret <8 x i1> %res
; CHECK-LABEL: test_icmp_v8i16_ne:
; CHECK: pcmpeqw
; CHECK: pxor
}

define <8 x i1> @test_icmp_v8i16_sgt(<8 x i16> %a, <8 x i16> %b) {
entry:
  %res = icmp sgt <8 x i16> %a, %b
  ret <8 x i1> %res
; CHECK-LABEL: test_icmp_v8i16_sgt:
; CHECK: pcmpgtw
}

define <8 x i1> @test_icmp_v8i16_sle(<8 x i16> %a, <8 x i16> %b) {
entry:
  %res = icmp sle <8 x i16> %a, %b
  ret <8 x i1> %res
; CHECK-LABEL: test_icmp_v8i16_sle:
; CHECK: pcmpgtw
; CHECK: pxor
}

define <8 x i1> @test_icmp_v8i16_slt(<8 x i16> %a, <8 x i16> %b) {
entry:
  %res = icmp slt <8 x i16> %a, %b
  ret <8 x i1> %res
; CHECK-LABEL: test_icmp_v8i16_slt:
; CHECK: pcmpgtw
}

define <8 x i1> @test_icmp_v8i16_uge(<8 x i16> %a, <8 x i16> %b) {
entry:
  %res = icmp uge <8 x i16> %a, %b
  ret <8 x i1> %res
; CHECK-LABEL: test_icmp_v8i16_uge:
; CHECK: pxor
; CHECK: pcmpgtw
; CHECK: pxor
}

define <8 x i1> @test_icmp_v8i16_ugt(<8 x i16> %a, <8 x i16> %b) {
entry:
  %res = icmp ugt <8 x i16> %a, %b
  ret <8 x i1> %res
; CHECK-LABEL: test_icmp_v8i16_ugt:
; CHECK: pxor
; CHECK: pcmpgtw
}

define <8 x i1> @test_icmp_v8i16_ule(<8 x i16> %a, <8 x i16> %b) {
entry:
  %res = icmp ule <8 x i16> %a, %b
  ret <8 x i1> %res
; CHECK-LABEL: test_icmp_v8i16_ule:
; CHECK: pxor
; CHECK: pcmpgtw
; CHECK: pxor
}

define <8 x i1> @test_icmp_v8i16_ult(<8 x i16> %a, <8 x i16> %b) {
entry:
  %res = icmp ult <8 x i16> %a, %b
  ret <8 x i1> %res
; CHECK-LABEL: test_icmp_v8i16_ult:
; CHECK: pxor
; CHECK: pcmpgtw
}

define <8 x i1> @test_icmp_v8i1_eq(<8 x i1> %a, <8 x i1> %b) {
entry:
  %res = icmp eq <8 x i1> %a, %b
  ret <8 x i1> %res
; CHECK-LABEL: test_icmp_v8i1_eq:
; CHECK: pcmpeqw
}

define <8 x i1> @test_icmp_v8i1_ne(<8 x i1> %a, <8 x i1> %b) {
entry:
  %res = icmp ne <8 x i1> %a, %b
  ret <8 x i1> %res
; CHECK-LABEL: test_icmp_v8i1_ne:
; CHECK: pcmpeqw
; CHECK: pxor
}

define <8 x i1> @test_icmp_v8i1_sgt(<8 x i1> %a, <8 x i1> %b) {
entry:
  %res = icmp sgt <8 x i1> %a, %b
  ret <8 x i1> %res
; CHECK-LABEL: test_icmp_v8i1_sgt:
; CHECK: pcmpgtw
}

define <8 x i1> @test_icmp_v8i1_sle(<8 x i1> %a, <8 x i1> %b) {
entry:
  %res = icmp sle <8 x i1> %a, %b
  ret <8 x i1> %res
; CHECK-LABEL: test_icmp_v8i1_sle:
; CHECK: pcmpgtw
; CHECK: pxor
}

define <8 x i1> @test_icmp_v8i1_slt(<8 x i1> %a, <8 x i1> %b) {
entry:
  %res = icmp slt <8 x i1> %a, %b
  ret <8 x i1> %res
; CHECK-LABEL: test_icmp_v8i1_slt:
; CHECK: pcmpgtw
}

define <8 x i1> @test_icmp_v8i1_uge(<8 x i1> %a, <8 x i1> %b) {
entry:
  %res = icmp uge <8 x i1> %a, %b
  ret <8 x i1> %res
; CHECK-LABEL: test_icmp_v8i1_uge:
; CHECK: pxor
; CHECK: pcmpgtw
; CHECK: pxor
}

define <8 x i1> @test_icmp_v8i1_ugt(<8 x i1> %a, <8 x i1> %b) {
entry:
  %res = icmp ugt <8 x i1> %a, %b
  ret <8 x i1> %res
; CHECK-LABEL: test_icmp_v8i1_ugt:
; CHECK: pxor
; CHECK: pcmpgtw
}

define <8 x i1> @test_icmp_v8i1_ule(<8 x i1> %a, <8 x i1> %b) {
entry:
  %res = icmp ule <8 x i1> %a, %b
  ret <8 x i1> %res
; CHECK-LABEL: test_icmp_v8i1_ule:
; CHECK: pxor
; CHECK: pcmpgtw
; CHECK: pxor
}

define <8 x i1> @test_icmp_v8i1_ult(<8 x i1> %a, <8 x i1> %b) {
entry:
  %res = icmp ult <8 x i1> %a, %b
  ret <8 x i1> %res
; CHECK-LABEL: test_icmp_v8i1_ult:
; CHECK: pxor
; CHECK: pcmpgtw
}

define <16 x i1> @test_icmp_v16i8_eq(<16 x i8> %a, <16 x i8> %b) {
entry:
  %res = icmp eq <16 x i8> %a, %b
  ret <16 x i1> %res
; CHECK-LABEL: test_icmp_v16i8_eq:
; CHECK: pcmpeqb
}

define <16 x i1> @test_icmp_v16i8_ne(<16 x i8> %a, <16 x i8> %b) {
entry:
  %res = icmp ne <16 x i8> %a, %b
  ret <16 x i1> %res
; CHECK-LABEL: test_icmp_v16i8_ne:
; CHECK: pcmpeqb
; CHECK: pxor
}

define <16 x i1> @test_icmp_v16i8_sgt(<16 x i8> %a, <16 x i8> %b) {
entry:
  %res = icmp sgt <16 x i8> %a, %b
  ret <16 x i1> %res
; CHECK-LABEL: test_icmp_v16i8_sgt:
; CHECK: pcmpgtb
}

define <16 x i1> @test_icmp_v16i8_sle(<16 x i8> %a, <16 x i8> %b) {
entry:
  %res = icmp sle <16 x i8> %a, %b
  ret <16 x i1> %res
; CHECK-LABEL: test_icmp_v16i8_sle:
; CHECK: pcmpgtb
; CHECK: pxor
}

define <16 x i1> @test_icmp_v16i8_slt(<16 x i8> %a, <16 x i8> %b) {
entry:
  %res = icmp slt <16 x i8> %a, %b
  ret <16 x i1> %res
; CHECK-LABEL: test_icmp_v16i8_slt:
; CHECK: pcmpgtb
}

define <16 x i1> @test_icmp_v16i8_uge(<16 x i8> %a, <16 x i8> %b) {
entry:
  %res = icmp uge <16 x i8> %a, %b
  ret <16 x i1> %res
; CHECK-LABEL: test_icmp_v16i8_uge:
; CHECK: pxor
; CHECK: pcmpgtb
; CHECK: pxor
}

define <16 x i1> @test_icmp_v16i8_ugt(<16 x i8> %a, <16 x i8> %b) {
entry:
  %res = icmp ugt <16 x i8> %a, %b
  ret <16 x i1> %res
; CHECK-LABEL: test_icmp_v16i8_ugt:
; CHECK: pxor
; CHECK: pcmpgtb
}

define <16 x i1> @test_icmp_v16i8_ule(<16 x i8> %a, <16 x i8> %b) {
entry:
  %res = icmp ule <16 x i8> %a, %b
  ret <16 x i1> %res
; CHECK-LABEL: test_icmp_v16i8_ule:
; CHECK: pxor
; CHECK: pcmpgtb
; CHECK: pxor
}

define <16 x i1> @test_icmp_v16i8_ult(<16 x i8> %a, <16 x i8> %b) {
entry:
  %res = icmp ult <16 x i8> %a, %b
  ret <16 x i1> %res
; CHECK-LABEL: test_icmp_v16i8_ult:
; CHECK: pxor
; CHECK: pcmpgtb
}

define <16 x i1> @test_icmp_v16i1_eq(<16 x i1> %a, <16 x i1> %b) {
entry:
  %res = icmp eq <16 x i1> %a, %b
  ret <16 x i1> %res
; CHECK-LABEL: test_icmp_v16i1_eq:
; CHECK: pcmpeqb
}

define <16 x i1> @test_icmp_v16i1_ne(<16 x i1> %a, <16 x i1> %b) {
entry:
  %res = icmp ne <16 x i1> %a, %b
  ret <16 x i1> %res
; CHECK-LABEL: test_icmp_v16i1_ne:
; CHECK: pcmpeqb
; CHECK: pxor
}

define <16 x i1> @test_icmp_v16i1_sgt(<16 x i1> %a, <16 x i1> %b) {
entry:
  %res = icmp sgt <16 x i1> %a, %b
  ret <16 x i1> %res
; CHECK-LABEL: test_icmp_v16i1_sgt:
; CHECK: pcmpgtb
}

define <16 x i1> @test_icmp_v16i1_sle(<16 x i1> %a, <16 x i1> %b) {
entry:
  %res = icmp sle <16 x i1> %a, %b
  ret <16 x i1> %res
; CHECK-LABEL: test_icmp_v16i1_sle:
; CHECK: pcmpgtb
; CHECK: pxor
}

define <16 x i1> @test_icmp_v16i1_slt(<16 x i1> %a, <16 x i1> %b) {
entry:
  %res = icmp slt <16 x i1> %a, %b
  ret <16 x i1> %res
; CHECK-LABEL: test_icmp_v16i1_slt:
; CHECK: pcmpgtb
}

define <16 x i1> @test_icmp_v16i1_uge(<16 x i1> %a, <16 x i1> %b) {
entry:
  %res = icmp uge <16 x i1> %a, %b
  ret <16 x i1> %res
; CHECK-LABEL: test_icmp_v16i1_uge:
; CHECK: pxor
; CHECK: pcmpgtb
; CHECK: pxor
}

define <16 x i1> @test_icmp_v16i1_ugt(<16 x i1> %a, <16 x i1> %b) {
entry:
  %res = icmp ugt <16 x i1> %a, %b
  ret <16 x i1> %res
; CHECK-LABEL: test_icmp_v16i1_ugt:
; CHECK: pxor
; CHECK: pcmpgtb
}

define <16 x i1> @test_icmp_v16i1_ule(<16 x i1> %a, <16 x i1> %b) {
entry:
  %res = icmp ule <16 x i1> %a, %b
  ret <16 x i1> %res
; CHECK-LABEL: test_icmp_v16i1_ule:
; CHECK: pxor
; CHECK: pcmpgtb
; CHECK: pxor
}

define <16 x i1> @test_icmp_v16i1_ult(<16 x i1> %a, <16 x i1> %b) {
entry:
  %res = icmp ult <16 x i1> %a, %b
  ret <16 x i1> %res
; CHECK-LABEL: test_icmp_v16i1_ult:
; CHECK: pxor
; CHECK: pcmpgtb
}

; ERRORS-NOT: ICE translation error
; DUMP-NOT: SZ
