; This test checks support for vector arithmetic.

; RUN: %llvm2ice -O2 --verbose none %s | FileCheck %s
; RUN: %llvm2ice -Om1 --verbose none %s | FileCheck %s
; RUN: %llvm2ice --verbose none %s | FileCheck --check-prefix=ERRORS %s
; RUN: %llvm2iceinsts %s | %szdiff %s | FileCheck --check-prefix=DUMP %s
; RUN: %llvm2iceinsts --pnacl %s | %szdiff %s \
; RUN:                           | FileCheck --check-prefix=DUMP %s

define <4 x float> @test_fadd(<4 x float> %arg0, <4 x float> %arg1) {
entry:
  %res = fadd <4 x float> %arg0, %arg1
  ret <4 x float> %res
; CHECK-LABEL: test_fadd:
; CHECK: addps
}

define <4 x float> @test_fsub(<4 x float> %arg0, <4 x float> %arg1) {
entry:
  %res = fsub <4 x float> %arg0, %arg1
  ret <4 x float> %res
; CHECK-LABEL: test_fsub:
; CHECK: subps
}

define <4 x float> @test_fmul(<4 x float> %arg0, <4 x float> %arg1) {
entry:
  %res = fmul <4 x float> %arg0, %arg1
  ret <4 x float> %res
; CHECK-LABEL: test_fmul:
; CHECK: mulps
}

define <4 x float> @test_fdiv(<4 x float> %arg0, <4 x float> %arg1) {
entry:
  %res = fdiv <4 x float> %arg0, %arg1
  ret <4 x float> %res
; CHECK-LABEL: test_fdiv:
; CHECK: divps
}

define <4 x float> @test_frem(<4 x float> %arg0, <4 x float> %arg1) {
entry:
  %res = frem <4 x float> %arg0, %arg1
  ret <4 x float> %res
; CHECK-LABEL: test_frem:
; CHECK: Sz_frem_v4f32
}

define <16 x i8> @test_add_v16i8(<16 x i8> %arg0, <16 x i8> %arg1) {
entry:
  %res = add <16 x i8> %arg0, %arg1
  ret <16 x i8> %res
; CHECK-LABEL: test_add_v16i8:
; CHECK: paddb
}

define <16 x i8> @test_and_v16i8(<16 x i8> %arg0, <16 x i8> %arg1) {
entry:
  %res = and <16 x i8> %arg0, %arg1
  ret <16 x i8> %res
; CHECK-LABEL: test_and_v16i8:
; CHECK: pand
}

define <16 x i8> @test_or_v16i8(<16 x i8> %arg0, <16 x i8> %arg1) {
entry:
  %res = or <16 x i8> %arg0, %arg1
  ret <16 x i8> %res
; CHECK-LABEL: test_or_v16i8:
; CHECK: por
}

define <16 x i8> @test_xor_v16i8(<16 x i8> %arg0, <16 x i8> %arg1) {
entry:
  %res = xor <16 x i8> %arg0, %arg1
  ret <16 x i8> %res
; CHECK-LABEL: test_xor_v16i8:
; CHECK: pxor
}

define <16 x i8> @test_sub_v16i8(<16 x i8> %arg0, <16 x i8> %arg1) {
entry:
  %res = sub <16 x i8> %arg0, %arg1
  ret <16 x i8> %res
; CHECK-LABEL: test_sub_v16i8:
; CHECK: psubb
}

define <16 x i8> @test_mul_v16i8(<16 x i8> %arg0, <16 x i8> %arg1) {
entry:
  %res = mul <16 x i8> %arg0, %arg1
  ret <16 x i8> %res
; CHECK-LABEL: test_mul_v16i8:
; CHECK: Sz_mul_v16i8
}

define <16 x i8> @test_shl_v16i8(<16 x i8> %arg0, <16 x i8> %arg1) {
entry:
  %res = shl <16 x i8> %arg0, %arg1
  ret <16 x i8> %res
; CHECK-LABEL: test_shl_v16i8:
; CHECK: Sz_shl_v16i8
}

define <16 x i8> @test_lshr_v16i8(<16 x i8> %arg0, <16 x i8> %arg1) {
entry:
  %res = lshr <16 x i8> %arg0, %arg1
  ret <16 x i8> %res
; CHECK-LABEL: test_lshr_v16i8:
; CHECK: Sz_lshr_v16i8
}

define <16 x i8> @test_ashr_v16i8(<16 x i8> %arg0, <16 x i8> %arg1) {
entry:
  %res = ashr <16 x i8> %arg0, %arg1
  ret <16 x i8> %res
; CHECK-LABEL: test_ashr_v16i8:
; CHECK: Sz_ashr_v16i8
}

define <16 x i8> @test_udiv_v16i8(<16 x i8> %arg0, <16 x i8> %arg1) {
entry:
  %res = udiv <16 x i8> %arg0, %arg1
  ret <16 x i8> %res
; CHECK-LABEL: test_udiv_v16i8:
; CHECK: Sz_udiv_v16i8
}

define <16 x i8> @test_sdiv_v16i8(<16 x i8> %arg0, <16 x i8> %arg1) {
entry:
  %res = sdiv <16 x i8> %arg0, %arg1
  ret <16 x i8> %res
; CHECK-LABEL: test_sdiv_v16i8:
; CHECK: Sz_sdiv_v16i8
}

define <16 x i8> @test_urem_v16i8(<16 x i8> %arg0, <16 x i8> %arg1) {
entry:
  %res = urem <16 x i8> %arg0, %arg1
  ret <16 x i8> %res
; CHECK-LABEL: test_urem_v16i8:
; CHECK: Sz_urem_v16i8
}

define <16 x i8> @test_srem_v16i8(<16 x i8> %arg0, <16 x i8> %arg1) {
entry:
  %res = srem <16 x i8> %arg0, %arg1
  ret <16 x i8> %res
; CHECK-LABEL: test_srem_v16i8:
; CHECK: Sz_srem_v16i8
}

define <8 x i16> @test_add_v8i16(<8 x i16> %arg0, <8 x i16> %arg1) {
entry:
  %res = add <8 x i16> %arg0, %arg1
  ret <8 x i16> %res
; CHECK-LABEL: test_add_v8i16:
; CHECK: paddw
}

define <8 x i16> @test_and_v8i16(<8 x i16> %arg0, <8 x i16> %arg1) {
entry:
  %res = and <8 x i16> %arg0, %arg1
  ret <8 x i16> %res
; CHECK-LABEL: test_and_v8i16:
; CHECK: pand
}

define <8 x i16> @test_or_v8i16(<8 x i16> %arg0, <8 x i16> %arg1) {
entry:
  %res = or <8 x i16> %arg0, %arg1
  ret <8 x i16> %res
; CHECK-LABEL: test_or_v8i16:
; CHECK: por
}

define <8 x i16> @test_xor_v8i16(<8 x i16> %arg0, <8 x i16> %arg1) {
entry:
  %res = xor <8 x i16> %arg0, %arg1
  ret <8 x i16> %res
; CHECK-LABEL: test_xor_v8i16:
; CHECK: pxor
}

define <8 x i16> @test_sub_v8i16(<8 x i16> %arg0, <8 x i16> %arg1) {
entry:
  %res = sub <8 x i16> %arg0, %arg1
  ret <8 x i16> %res
; CHECK-LABEL: test_sub_v8i16:
; CHECK: psubw
}

define <8 x i16> @test_mul_v8i16(<8 x i16> %arg0, <8 x i16> %arg1) {
entry:
  %res = mul <8 x i16> %arg0, %arg1
  ret <8 x i16> %res
; CHECK-LABEL: test_mul_v8i16:
; CHECK: pmullw
}

define <8 x i16> @test_shl_v8i16(<8 x i16> %arg0, <8 x i16> %arg1) {
entry:
  %res = shl <8 x i16> %arg0, %arg1
  ret <8 x i16> %res
; CHECK-LABEL: test_shl_v8i16:
; CHECK: Sz_shl_v8i16
}

define <8 x i16> @test_lshr_v8i16(<8 x i16> %arg0, <8 x i16> %arg1) {
entry:
  %res = lshr <8 x i16> %arg0, %arg1
  ret <8 x i16> %res
; CHECK-LABEL: test_lshr_v8i16:
; CHECK: Sz_lshr_v8i16
}

define <8 x i16> @test_ashr_v8i16(<8 x i16> %arg0, <8 x i16> %arg1) {
entry:
  %res = ashr <8 x i16> %arg0, %arg1
  ret <8 x i16> %res
; CHECK-LABEL: test_ashr_v8i16:
; CHECK: Sz_ashr_v8i16
}

define <8 x i16> @test_udiv_v8i16(<8 x i16> %arg0, <8 x i16> %arg1) {
entry:
  %res = udiv <8 x i16> %arg0, %arg1
  ret <8 x i16> %res
; CHECK-LABEL: test_udiv_v8i16:
; CHECK: Sz_udiv_v8i16
}

define <8 x i16> @test_sdiv_v8i16(<8 x i16> %arg0, <8 x i16> %arg1) {
entry:
  %res = sdiv <8 x i16> %arg0, %arg1
  ret <8 x i16> %res
; CHECK-LABEL: test_sdiv_v8i16:
; CHECK: Sz_sdiv_v8i16
}

define <8 x i16> @test_urem_v8i16(<8 x i16> %arg0, <8 x i16> %arg1) {
entry:
  %res = urem <8 x i16> %arg0, %arg1
  ret <8 x i16> %res
; CHECK-LABEL: test_urem_v8i16:
; CHECK: Sz_urem_v8i16
}

define <8 x i16> @test_srem_v8i16(<8 x i16> %arg0, <8 x i16> %arg1) {
entry:
  %res = srem <8 x i16> %arg0, %arg1
  ret <8 x i16> %res
; CHECK-LABEL: test_srem_v8i16:
; CHECK: Sz_srem_v8i16
}

define <4 x i32> @test_add_v4i32(<4 x i32> %arg0, <4 x i32> %arg1) {
entry:
  %res = add <4 x i32> %arg0, %arg1
  ret <4 x i32> %res
; CHECK-LABEL: test_add_v4i32:
; CHECK: paddd
}

define <4 x i32> @test_and_v4i32(<4 x i32> %arg0, <4 x i32> %arg1) {
entry:
  %res = and <4 x i32> %arg0, %arg1
  ret <4 x i32> %res
; CHECK-LABEL: test_and_v4i32:
; CHECK: pand
}

define <4 x i32> @test_or_v4i32(<4 x i32> %arg0, <4 x i32> %arg1) {
entry:
  %res = or <4 x i32> %arg0, %arg1
  ret <4 x i32> %res
; CHECK-LABEL: test_or_v4i32:
; CHECK: por
}

define <4 x i32> @test_xor_v4i32(<4 x i32> %arg0, <4 x i32> %arg1) {
entry:
  %res = xor <4 x i32> %arg0, %arg1
  ret <4 x i32> %res
; CHECK-LABEL: test_xor_v4i32:
; CHECK: pxor
}

define <4 x i32> @test_sub_v4i32(<4 x i32> %arg0, <4 x i32> %arg1) {
entry:
  %res = sub <4 x i32> %arg0, %arg1
  ret <4 x i32> %res
; CHECK-LABEL: test_sub_v4i32:
; CHECK: psubd
}

define <4 x i32> @test_mul_v4i32(<4 x i32> %arg0, <4 x i32> %arg1) {
entry:
  %res = mul <4 x i32> %arg0, %arg1
  ret <4 x i32> %res
; CHECK-LABEL: test_mul_v4i32:
; CHECK: pmuludq
; CHECK: pmuludq
}

define <4 x i32> @test_shl_v4i32(<4 x i32> %arg0, <4 x i32> %arg1) {
entry:
  %res = shl <4 x i32> %arg0, %arg1
  ret <4 x i32> %res
; CHECK-LABEL: test_shl_v4i32:
; CHECK: Sz_shl_v4i32
}

define <4 x i32> @test_lshr_v4i32(<4 x i32> %arg0, <4 x i32> %arg1) {
entry:
  %res = lshr <4 x i32> %arg0, %arg1
  ret <4 x i32> %res
; CHECK-LABEL: test_lshr_v4i32:
; CHECK: Sz_lshr_v4i32
}

define <4 x i32> @test_ashr_v4i32(<4 x i32> %arg0, <4 x i32> %arg1) {
entry:
  %res = ashr <4 x i32> %arg0, %arg1
  ret <4 x i32> %res
; CHECK-LABEL: test_ashr_v4i32:
; CHECK: Sz_ashr_v4i32
}

define <4 x i32> @test_udiv_v4i32(<4 x i32> %arg0, <4 x i32> %arg1) {
entry:
  %res = udiv <4 x i32> %arg0, %arg1
  ret <4 x i32> %res
; CHECK-LABEL: test_udiv_v4i32:
; CHECK: Sz_udiv_v4i32
}

define <4 x i32> @test_sdiv_v4i32(<4 x i32> %arg0, <4 x i32> %arg1) {
entry:
  %res = sdiv <4 x i32> %arg0, %arg1
  ret <4 x i32> %res
; CHECK-LABEL: test_sdiv_v4i32:
; CHECK: Sz_sdiv_v4i32
}

define <4 x i32> @test_urem_v4i32(<4 x i32> %arg0, <4 x i32> %arg1) {
entry:
  %res = urem <4 x i32> %arg0, %arg1
  ret <4 x i32> %res
; CHECK-LABEL: test_urem_v4i32:
; CHECK: Sz_urem_v4i32
}

define <4 x i32> @test_srem_v4i32(<4 x i32> %arg0, <4 x i32> %arg1) {
entry:
  %res = srem <4 x i32> %arg0, %arg1
  ret <4 x i32> %res
; CHECK-LABEL: test_srem_v4i32:
; CHECK: Sz_srem_v4i32
}

; ERRORS-NOT: ICE translation error
; DUMP-NOT: SZ
