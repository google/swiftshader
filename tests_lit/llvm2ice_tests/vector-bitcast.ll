; This file tests bitcasts of vector type. For most operations, these
; should be lowered to a no-op on -O2.

; TODO(jvoung): fix extra "CALLTARGETS" run. The llvm-objdump symbolizer
; doesn't know how to symbolize non-section-local functions.
; The newer LLVM 3.6 one does work, but watch out for other bugs.

; RUN: %p2i -i %s --args -O2 --verbose none \
; RUN:   | FileCheck --check-prefix=CALLTARGETS %s
; RUN: %p2i -i %s --args -O2 --verbose none \
; RUN:   | llvm-mc -triple=i686-none-nacl -filetype=obj \
; RUN:   | llvm-objdump -d -symbolize -x86-asm-syntax=intel - | FileCheck %s
; RUN: %p2i -i %s --args -Om1 --verbose none \
; RUN:   | llvm-mc -triple=i686-none-nacl -filetype=obj \
; RUN:   | llvm-objdump -d -symbolize -x86-asm-syntax=intel - \
; RUN:   | FileCheck --check-prefix=OPTM1 %s

define <16 x i8> @test_bitcast_v16i8_to_v16i8(<16 x i8> %arg) {
entry:
  %res = bitcast <16 x i8> %arg to <16 x i8>
  ret <16 x i8> %res

; CHECK-LABEL: test_bitcast_v16i8_to_v16i8
; CHECK-NEXT: ret
}

define <8 x i16> @test_bitcast_v16i8_to_v8i16(<16 x i8> %arg) {
entry:
  %res = bitcast <16 x i8> %arg to <8 x i16>
  ret <8 x i16> %res

; CHECK-LABEL: test_bitcast_v16i8_to_v8i16
; CHECK-NEXT: ret
}

define <4 x i32> @test_bitcast_v16i8_to_v4i32(<16 x i8> %arg) {
entry:
  %res = bitcast <16 x i8> %arg to <4 x i32>
  ret <4 x i32> %res

; CHECK-LABEL: test_bitcast_v16i8_to_v4i32:
; CHECK-NEXT: ret
}

define <4 x float> @test_bitcast_v16i8_to_v4f32(<16 x i8> %arg) {
entry:
  %res = bitcast <16 x i8> %arg to <4 x float>
  ret <4 x float> %res

; CHECK-LABEL: test_bitcast_v16i8_to_v4f32:
; CHECK-NEXT: ret
}

define <16 x i8> @test_bitcast_v8i16_to_v16i8(<8 x i16> %arg) {
entry:
  %res = bitcast <8 x i16> %arg to <16 x i8>
  ret <16 x i8> %res

; CHECK-LABEL: test_bitcast_v8i16_to_v16i8:
; CHECK-NEXT: ret
}

define <8 x i16> @test_bitcast_v8i16_to_v8i16(<8 x i16> %arg) {
entry:
  %res = bitcast <8 x i16> %arg to <8 x i16>
  ret <8 x i16> %res

; CHECK-LABEL: test_bitcast_v8i16_to_v8i16:
; CHECK-NEXT: ret
}

define <4 x i32> @test_bitcast_v8i16_to_v4i32(<8 x i16> %arg) {
entry:
  %res = bitcast <8 x i16> %arg to <4 x i32>
  ret <4 x i32> %res

; CHECK-LABEL: test_bitcast_v8i16_to_v4i32:
; CHECK-NEXT: ret
}

define <4 x float> @test_bitcast_v8i16_to_v4f32(<8 x i16> %arg) {
entry:
  %res = bitcast <8 x i16> %arg to <4 x float>
  ret <4 x float> %res

; CHECK-LABEL: test_bitcast_v8i16_to_v4f32:
; CHECK-NEXT: ret
}

define <16 x i8> @test_bitcast_v4i32_to_v16i8(<4 x i32> %arg) {
entry:
  %res = bitcast <4 x i32> %arg to <16 x i8>
  ret <16 x i8> %res

; CHECK-LABEL: test_bitcast_v4i32_to_v16i8:
; CHECK-NEXT: ret
}

define <8 x i16> @test_bitcast_v4i32_to_v8i16(<4 x i32> %arg) {
entry:
  %res = bitcast <4 x i32> %arg to <8 x i16>
  ret <8 x i16> %res

; CHECK-LABEL: test_bitcast_v4i32_to_v8i16:
; CHECK-NEXT: ret
}

define <4 x i32> @test_bitcast_v4i32_to_v4i32(<4 x i32> %arg) {
entry:
  %res = bitcast <4 x i32> %arg to <4 x i32>
  ret <4 x i32> %res

; CHECK-LABEL: test_bitcast_v4i32_to_v4i32:
; CHECK-NEXT: ret
}

define <4 x float> @test_bitcast_v4i32_to_v4f32(<4 x i32> %arg) {
entry:
  %res = bitcast <4 x i32> %arg to <4 x float>
  ret <4 x float> %res

; CHECK-LABEL: test_bitcast_v4i32_to_v4f32:
; CHECK-NEXT: ret
}

define <16 x i8> @test_bitcast_v4f32_to_v16i8(<4 x float> %arg) {
entry:
  %res = bitcast <4 x float> %arg to <16 x i8>
  ret <16 x i8> %res

; CHECK-LABEL: test_bitcast_v4f32_to_v16i8:
; CHECK-NEXT: ret
}

define <8 x i16> @test_bitcast_v4f32_to_v8i16(<4 x float> %arg) {
entry:
  %res = bitcast <4 x float> %arg to <8 x i16>
  ret <8 x i16> %res

; CHECK-LABEL: test_bitcast_v4f32_to_v8i16:
; CHECK-NEXT: ret
}

define <4 x i32> @test_bitcast_v4f32_to_v4i32(<4 x float> %arg) {
entry:
  %res = bitcast <4 x float> %arg to <4 x i32>
  ret <4 x i32> %res

; CHECK-LABEL: test_bitcast_v4f32_to_v4i32:
; CHECK-NEXT: ret
}

define <4 x float> @test_bitcast_v4f32_to_v4f32(<4 x float> %arg) {
entry:
  %res = bitcast <4 x float> %arg to <4 x float>
  ret <4 x float> %res

; CHECK-LABEL: test_bitcast_v4f32_to_v4f32:
; CHECK-NEXT: ret
}

define i8 @test_bitcast_v8i1_to_i8(<8 x i1> %arg) {
entry:
  %res = bitcast <8 x i1> %arg to i8
  ret i8 %res

; CHECK-LABEL: test_bitcast_v8i1_to_i8:
; CALLTARGETS-LABEL: test_bitcast_v8i1_to_i8:
; CHECK: call -4
; CALLTARGETS: call Sz_bitcast_v8i1_to_i8

; OPTM1-LABEL: test_bitcast_v8i1_to_i8:
; OPMT1: call -4
}

define i16 @test_bitcast_v16i1_to_i16(<16 x i1> %arg) {
entry:
  %res = bitcast <16 x i1> %arg to i16
  ret i16 %res

; CHECK-LABEL: test_bitcast_v16i1_to_i16:
; CALLTARGETS-LABEL: test_bitcast_v16i1_to_i16:
; CHECK: call -4
; CALLTARGETS: call Sz_bitcast_v16i1_to_i16

; OPTM1-LABEL: test_bitcast_v16i1_to_i16:
; OPMT1: call -4
}

define <8 x i1> @test_bitcast_i8_to_v8i1(i32 %arg) {
entry:
  %arg.trunc = trunc i32 %arg to i8
  %res = bitcast i8 %arg.trunc to <8 x i1>
  ret <8 x i1> %res

; CHECK-LABEL: test_bitcast_i8_to_v8i1:
; CALLTARGETS-LABEL: test_bitcast_i8_to_v8i1
; CHECK: call -4
; CALLTARGETS: call Sz_bitcast_i8_to_v8i1

; OPTM1-LABEL: test_bitcast_i8_to_v8i1:
; OPTM1: call -4
}

define <16 x i1> @test_bitcast_i16_to_v16i1(i32 %arg) {
entry:
  %arg.trunc = trunc i32 %arg to i16
  %res = bitcast i16 %arg.trunc to <16 x i1>
  ret <16 x i1> %res

; CHECK-LABEL: test_bitcast_i16_to_v16i1:
; CALLTARGETS-LABEL: test_bitcast_i16_to_v16i1
; CHECK: call -4
; CALLTARGETS: call Sz_bitcast_i16_to_v16i1

; OPTM1-LABEL: test_bitcast_i16_to_v16i1:
; OPTM1: call -4
}
