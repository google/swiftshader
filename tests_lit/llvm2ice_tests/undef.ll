; This test checks that undef values are represented as zero.

; RUN: %llvm2ice --verbose none %s | FileCheck  %s
; RUN: %llvm2ice -O2 --verbose none %s | FileCheck  %s
; RUN: %llvm2ice --verbose none %s | FileCheck --check-prefix=ERRORS %s
; RUN: %llvm2iceinsts %s | %szdiff %s | FileCheck --check-prefix=DUMP %s
; RUN: %llvm2iceinsts --pnacl %s | %szdiff %s \
; RUN:                           | FileCheck --check-prefix=DUMP %s

define i32 @undef_i32() {
entry:
  ret i32 undef
; CHECK-LABEL: undef_i32:
; CHECK: mov eax, 0
; CHECK: ret
}

define i64 @undef_i64() {
entry:
  ret i64 undef
; CHECK-LABEL: undef_i64:
; CHECK-DAG: mov eax, 0
; CHECK-DAG: mov edx, 0
; CHECK: ret
}

define float @undef_float() {
entry:
  ret float undef
; CHECK-LABEL: undef_float:
; CHECK-NOT: sub esp
; CHECK: fld
; CHECK: ret
}

define <4 x i1> @undef_v4i1() {
entry:
  ret <4 x i1> undef
; CHECK-LABEL: undef_v4i1:
; CHECK: pxor
; CHECK: ret
}

define <8 x i1> @undef_v8i1() {
entry:
  ret <8 x i1> undef
; CHECK-LABEL: undef_v8i1:
; CHECK: pxor
; CHECK: ret
}

define <16 x i1> @undef_v16i1() {
entry:
  ret <16 x i1> undef
; CHECK-LABEL: undef_v16i1:
; CHECK: pxor
; CHECK: ret
}

define <16 x i8> @undef_v16i8() {
entry:
  ret <16 x i8> undef
; CHECK-LABEL: undef_v16i8:
; CHECK: pxor
; CHECK: ret
}

define <8 x i16> @undef_v8i16() {
entry:
  ret <8 x i16> undef
; CHECK-LABEL: undef_v8i16:
; CHECK: pxor
; CHECK: ret
}

define <4 x i32> @undef_v4i32() {
entry:
  ret <4 x i32> undef
; CHECK-LABEL: undef_v4i32:
; CHECK: pxor
; CHECK: ret
}

define <4 x float> @undef_v4f32() {
entry:
  ret <4 x float> undef
; CHECK-LABEL: undef_v4f32:
; CHECK: pxor
; CHECK: ret
}

; ERRORS-NOT: ICE translation error
; DUMP-NOT: SZ
