; This file tests casting / conversion operations that apply to vector types.
; bitcast operations are in vector-bitcast.ll.

; RUN: %p2i -i %s --args -O2 --verbose none \
; RUN:   | llvm-mc -triple=i686-none-nacl -filetype=obj \
; RUN:   | llvm-objdump -d --symbolize -x86-asm-syntax=intel - | FileCheck %s
; RUN: %p2i -i %s --args -Om1 --verbose none \
; RUN:   | llvm-mc -triple=i686-none-nacl -filetype=obj \
; RUN:   | llvm-objdump -d --symbolize -x86-asm-syntax=intel - | FileCheck %s

; sext operations

define <16 x i8> @test_sext_v16i1_to_v16i8(<16 x i1> %arg) {
entry:
  %res = sext <16 x i1> %arg to <16 x i8>
  ret <16 x i8> %res

; CHECK-LABEL: test_sext_v16i1_to_v16i8:
; CHECK: pxor
; CHECK: pcmpeqb
; CHECK: psubb
; CHECK: pand
; CHECK: pxor
; CHECK: pcmpgtb
}

define <8 x i16> @test_sext_v8i1_to_v8i16(<8 x i1> %arg) {
entry:
  %res = sext <8 x i1> %arg to <8 x i16>
  ret <8 x i16> %res

; CHECK-LABEL: test_sext_v8i1_to_v8i16:
; CHECK: psllw {{.*}}, 15
; CHECK: psraw {{.*}}, 15
}

define <4 x i32> @test_sext_v4i1_to_v4i32(<4 x i1> %arg) {
entry:
  %res = sext <4 x i1> %arg to <4 x i32>
  ret <4 x i32> %res

; CHECK-LABEL: test_sext_v4i1_to_v4i32:
; CHECK: pslld {{.*}}, 31
; CHECK: psrad {{.*}}, 31
}

; zext operations

define <16 x i8> @test_zext_v16i1_to_v16i8(<16 x i1> %arg) {
entry:
  %res = zext <16 x i1> %arg to <16 x i8>
  ret <16 x i8> %res

; CHECK-LABEL: test_zext_v16i1_to_v16i8:
; CHECK: pxor
; CHECK: pcmpeqb
; CHECK: psubb
; CHECK: pand
}

define <8 x i16> @test_zext_v8i1_to_v8i16(<8 x i1> %arg) {
entry:
  %res = zext <8 x i1> %arg to <8 x i16>
  ret <8 x i16> %res

; CHECK-LABEL: test_zext_v8i1_to_v8i16:
; CHECK: pxor
; CHECK: pcmpeqw
; CHECK: psubw
; CHECK: pand
}

define <4 x i32> @test_zext_v4i1_to_v4i32(<4 x i1> %arg) {
entry:
  %res = zext <4 x i1> %arg to <4 x i32>
  ret <4 x i32> %res

; CHECK-LABEL: test_zext_v4i1_to_v4i32:
; CHECK: pxor
; CHECK: pcmpeqd
; CHECK: psubd
; CHECK: pand
}

; trunc operations

define <16 x i1> @test_trunc_v16i8_to_v16i1(<16 x i8> %arg) {
entry:
  %res = trunc <16 x i8> %arg to <16 x i1>
  ret <16 x i1> %res

; CHECK-LABEL: test_trunc_v16i8_to_v16i1:
; CHECK: pxor
; CHECK: pcmpeqb
; CHECK: psubb
; CHECK: pand
}

define <8 x i1> @test_trunc_v8i16_to_v8i1(<8 x i16> %arg) {
entry:
  %res = trunc <8 x i16> %arg to <8 x i1>
  ret <8 x i1> %res

; CHECK-LABEL: test_trunc_v8i16_to_v8i1:
; CHECK: pxor
; CHECK: pcmpeqw
; CHECK: psubw
; CHECK: pand
}

define <4 x i1> @test_trunc_v4i32_to_v4i1(<4 x i32> %arg) {
entry:
  %res = trunc <4 x i32> %arg to <4 x i1>
  ret <4 x i1> %res

; CHECK-LABEL: test_trunc_v4i32_to_v4i1:
; CHECK: pxor
; CHECK: pcmpeqd
; CHECK: psubd
; CHECK: pand
}

; fpto[us]i operations

define <4 x i32> @test_fptosi_v4f32_to_v4i32(<4 x float> %arg) {
entry:
  %res = fptosi <4 x float> %arg to <4 x i32>
  ret <4 x i32> %res

; CHECK-LABEL: test_fptosi_v4f32_to_v4i32:
; CHECK: cvttps2dq
}

define <4 x i32> @test_fptoui_v4f32_to_v4i32(<4 x float> %arg) {
entry:
  %res = fptoui <4 x float> %arg to <4 x i32>
  ret <4 x i32> %res

; CHECK-LABEL: test_fptoui_v4f32_to_v4i32:
; CHECK: call Sz_fptoui_v4f32
}

; [su]itofp operations

define <4 x float> @test_sitofp_v4i32_to_v4f32(<4 x i32> %arg) {
entry:
  %res = sitofp <4 x i32> %arg to <4 x float>
  ret <4 x float> %res

; CHECK-LABEL: test_sitofp_v4i32_to_v4f32:
; CHECK: cvtdq2ps
}

define <4 x float> @test_uitofp_v4i32_to_v4f32(<4 x i32> %arg) {
entry:
  %res = uitofp <4 x i32> %arg to <4 x float>
  ret <4 x float> %res

; CHECK-LABEL: test_uitofp_v4i32_to_v4f32:
; CHECK: call Sz_uitofp_v4i32
}
