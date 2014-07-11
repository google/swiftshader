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
; CHECK: __frem_v4f32
}

; ERRORS-NOT: ICE translation error
; DUMP-NOT: SZ
