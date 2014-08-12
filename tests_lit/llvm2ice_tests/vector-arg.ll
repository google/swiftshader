; This file checks that Subzero generates code in accordance with the
; calling convention for vectors.

; RUN: %llvm2ice -O2 --verbose none %s | FileCheck %s
; RUN: %llvm2ice -Om1 --verbose none %s | FileCheck --check-prefix=OPTM1 %s
; RUN: %llvm2ice -O2 --verbose none %s \
; RUN:               | llvm-mc -arch=x86 -x86-asm-syntax=intel -filetype=obj
; RUN: %llvm2ice -Om1 --verbose none %s \
; RUN:               | llvm-mc -arch=x86 -x86-asm-syntax=intel -filetype=obj
; RUN: %llvm2ice --verbose none %s | FileCheck --check-prefix=ERRORS %s
; RUN: %llvm2iceinsts %s | %szdiff %s | FileCheck --check-prefix=DUMP %s
; RUN: %llvm2iceinsts --pnacl %s | %szdiff %s \
; RUN:                           | FileCheck --check-prefix=DUMP %s

; The first five functions test that vectors are moved from their
; correct argument location to xmm0.

define <4 x float> @test_returning_arg0(<4 x float> %arg0, <4 x float> %arg1, <4 x float> %arg2, <4 x float> %arg3, <4 x float> %arg4, <4 x float> %arg5) {
entry:
  ret <4 x float> %arg0
; CHECK-LABEL: test_returning_arg0:
; CHECK-NOT: mov
; CHECK: ret

; OPTM1-LABEL: test_returning_arg0:
; OPTM1: movups xmmword ptr [[LOC:.*]], xmm0
; OPTM1: movups xmm0, xmmword ptr [[LOC]]
; OPTM1: ret
}

define <4 x float> @test_returning_arg1(<4 x float> %arg0, <4 x float> %arg1, <4 x float> %arg2, <4 x float> %arg3, <4 x float> %arg4, <4 x float> %arg5) {
entry:
  ret <4 x float> %arg1
; CHECK-LABEL: test_returning_arg1:
; CHECK: movups xmm0, xmm1
; CHECK: ret

; OPTM1-LABEL: test_returning_arg1:
; OPTM1: movups xmmword ptr [[LOC:.*]], xmm1
; OPTM1: movups xmm0, xmmword ptr [[LOC]]
; OPTM1: ret
}

define <4 x float> @test_returning_arg2(<4 x float> %arg0, <4 x float> %arg1, <4 x float> %arg2, <4 x float> %arg3, <4 x float> %arg4, <4 x float> %arg5) {
entry:
  ret <4 x float> %arg2
; CHECK-LABEL: test_returning_arg2:
; CHECK: movups xmm0, xmm2
; CHECK: ret

; OPTM1-LABEL: test_returning_arg2:
; OPTM1: movups xmmword ptr [[LOC:.*]], xmm2
; OPTM1: movups xmm0, xmmword ptr [[LOC]]
; OPTM1: ret
}

define <4 x float> @test_returning_arg3(<4 x float> %arg0, <4 x float> %arg1, <4 x float> %arg2, <4 x float> %arg3, <4 x float> %arg4, <4 x float> %arg5) {
entry:
  ret <4 x float> %arg3
; CHECK-LABEL: test_returning_arg3:
; CHECK: movups xmm0, xmm3
; CHECK: ret

; OPTM1-LABEL: test_returning_arg3:
; OPTM1: movups xmmword ptr [[LOC:.*]], xmm3
; OPTM1: movups xmm0, xmmword ptr [[LOC]]
; OPTM1: ret
}

define <4 x float> @test_returning_arg4(<4 x float> %arg0, <4 x float> %arg1, <4 x float> %arg2, <4 x float> %arg3, <4 x float> %arg4, <4 x float> %arg5) {
entry:
  ret <4 x float> %arg4
; CHECK-LABEL: test_returning_arg4:
; CHECK: movups xmm0, xmmword ptr [esp+4]
; CHECK: ret

; OPTM1-LABEL: test_returning_arg4:
; OPTM1: movups xmm0, xmmword ptr {{.*}}
; OPTM1: ret
}

; The next five functions check that xmm arguments are handled
; correctly when interspersed with stack arguments in the argument
; list.

define <4 x float> @test_returning_interspersed_arg0(i32 %i32arg0, double %doublearg0, <4 x float> %arg0, <4 x float> %arg1, i32 %i32arg1, <4 x float> %arg2, double %doublearg1, <4 x float> %arg3, i32 %i32arg2, double %doublearg2, float %floatarg0, <4 x float> %arg4, <4 x float> %arg5, float %floatarg1) {
entry:
  ret <4 x float> %arg0
; CHECK-LABEL: test_returning_interspersed_arg0:
; CHECK-NOT: mov
; CHECK: ret

; OPTM1-LABEL: test_returning_interspersed_arg0:
; OPTM1: movups xmmword ptr [[LOC:.*]], xmm0
; OPTM1: movups xmm0, xmmword ptr [[LOC]]
; OPTM1: ret
}

define <4 x float> @test_returning_interspersed_arg1(i32 %i32arg0, double %doublearg0, <4 x float> %arg0, <4 x float> %arg1, i32 %i32arg1, <4 x float> %arg2, double %doublearg1, <4 x float> %arg3, i32 %i32arg2, double %doublearg2, float %floatarg0, <4 x float> %arg4, <4 x float> %arg5, float %floatarg1) {
entry:
  ret <4 x float> %arg1
; CHECK-LABEL: test_returning_interspersed_arg1:
; CHECK: movups xmm0, xmm1
; CHECK: ret

; OPTM1-LABEL: test_returning_interspersed_arg1:
; OPTM1: movups xmmword ptr [[LOC:.*]], xmm1
; OPTM1: movups xmm0, xmmword ptr [[LOC]]
; OPTM1: ret
}

define <4 x float> @test_returning_interspersed_arg2(i32 %i32arg0, double %doublearg0, <4 x float> %arg0, <4 x float> %arg1, i32 %i32arg1, <4 x float> %arg2, double %doublearg1, <4 x float> %arg3, i32 %i32arg2, double %doublearg2, float %floatarg0, <4 x float> %arg4, <4 x float> %arg5, float %floatarg1) {
entry:
  ret <4 x float> %arg2
; CHECK-LABEL: test_returning_interspersed_arg2:
; CHECK: movups xmm0, xmm2
; CHECK: ret

; OPTM1-LABEL: test_returning_interspersed_arg2:
; OPTM1: movups xmmword ptr [[LOC:.*]], xmm2
; OPTM1: movups xmm0, xmmword ptr [[LOC]]
; OPTM1: ret
}

define <4 x float> @test_returning_interspersed_arg3(i32 %i32arg0, double %doublearg0, <4 x float> %arg0, <4 x float> %arg1, i32 %i32arg1, <4 x float> %arg2, double %doublearg1, <4 x float> %arg3, i32 %i32arg2, double %doublearg2, float %floatarg0, <4 x float> %arg4, <4 x float> %arg5, float %floatarg1) {
entry:
  ret <4 x float> %arg3
; CHECK-LABEL: test_returning_interspersed_arg3:
; CHECK: movups xmm0, xmm3
; CHECK: ret

; OPTM1-LABEL: test_returning_interspersed_arg3:
; OPTM1: movups xmmword ptr [[LOC:.*]], xmm3
; OPTM1: movups xmm0, xmmword ptr [[LOC]]
; OPTM1: ret
}

define <4 x float> @test_returning_interspersed_arg4(i32 %i32arg0, double %doublearg0, <4 x float> %arg0, <4 x float> %arg1, i32 %i32arg1, <4 x float> %arg2, double %doublearg1, <4 x float> %arg3, i32 %i32arg2, double %doublearg2, float %floatarg0, <4 x float> %arg4, <4 x float> %arg5, float %floatarg1) {
entry:
  ret <4 x float> %arg4
; CHECK-LABEL: test_returning_interspersed_arg4:
; CHECK: movups xmm0, xmmword ptr [esp+52]
; CHECK: ret

; OPTM1-LABEL: test_returning_interspersed_arg4:
; OPTM1: movups xmm0, xmmword ptr {{.*}}
; OPTM1: ret
}

; Test that vectors are passed correctly as arguments to a function.

declare void @VectorArgs(<4 x float>, <4 x float>, <4 x float>, <4 x float>, <4 x float>, <4 x float>)

declare void @killXmmRegisters()

define void @test_passing_vectors(<4 x float> %arg0, <4 x float> %arg1, <4 x float> %arg2, <4 x float> %arg3, <4 x float> %arg4, <4 x float> %arg5, <4 x float> %arg6, <4 x float> %arg7, <4 x float> %arg8, <4 x float> %arg9) {
entry:
  ; Kills XMM registers so that no in-arg lowering code interferes
  ; with the test.
  call void @killXmmRegisters()
  call void @VectorArgs(<4 x float> %arg9, <4 x float> %arg8, <4 x float> %arg7, <4 x float> %arg6, <4 x float> %arg5, <4 x float> %arg4)
  ret void
; CHECK-LABEL: test_passing_vectors:
; CHECK: sub esp, 32
; CHECK: movups  [[ARG5:.*]], xmmword ptr [esp+64]
; CHECK: movups  xmmword ptr [esp], [[ARG5]]
; CHECK: movups  [[ARG6:.*]], xmmword ptr [esp+48]
; CHECK: movups  xmmword ptr [esp+16], [[ARG6]]
; CHECK: movups  xmm0, xmmword ptr [esp+128]
; CHECK: movups  xmm1, xmmword ptr [esp+112]
; CHECK: movups  xmm2, xmmword ptr [esp+96]
; CHECK: movups  xmm3, xmmword ptr [esp+80]
; CHECK: call VectorArgs
; CHECK-NEXT: add esp, 32
; CHECK: ret

; OPTM1-LABEL: test_passing_vectors:
; OPTM1: sub esp, 32
; OPTM1: movups  [[ARG5:.*]], xmmword ptr {{.*}}
; OPTM1: movups  xmmword ptr [esp], [[ARG5]]
; OPTM1: movups  [[ARG6:.*]], xmmword ptr {{.*}}
; OPTM1: movups  xmmword ptr [esp+16], [[ARG6]]
; OPTM1: movups  xmm0, xmmword ptr {{.*}}
; OPTM1: movups  xmm1, xmmword ptr {{.*}}
; OPTM1: movups  xmm2, xmmword ptr {{.*}}
; OPTM1: movups  xmm3, xmmword ptr {{.*}}
; OPTM1: call VectorArgs
; OPTM1-NEXT: add esp, 32
; OPTM1: ret
}

declare void @InterspersedVectorArgs(<4 x float>, i64, <4 x float>, i64, <4 x float>, float, <4 x float>, double, <4 x float>, i32, <4 x float>)

define void @test_passing_vectors_interspersed(<4 x float> %arg0, <4 x float> %arg1, <4 x float> %arg2, <4 x float> %arg3, <4 x float> %arg4, <4 x float> %arg5, <4 x float> %arg6, <4 x float> %arg7, <4 x float> %arg8, <4 x float> %arg9) {
entry:
  ; Kills XMM registers so that no in-arg lowering code interferes
  ; with the test.
  call void @killXmmRegisters()
  call void @InterspersedVectorArgs(<4 x float> %arg9, i64 0, <4 x float> %arg8, i64 1, <4 x float> %arg7, float 2.000000e+00, <4 x float> %arg6, double 3.000000e+00, <4 x float> %arg5, i32 4, <4 x float> %arg4)
  ret void
; CHECK-LABEL: test_passing_vectors_interspersed:
; CHECK: sub esp, 80
; CHECK: movups  [[ARG9:.*]], xmmword ptr [esp+112]
; CHECK: movups  xmmword ptr [esp+32], [[ARG9]]
; CHECK: movups  [[ARG11:.*]], xmmword ptr [esp+96]
; CHECK: movups  xmmword ptr [esp+64], [[ARG11]]
; CHECK: movups  xmm0, xmmword ptr [esp+176]
; CHECK: movups  xmm1, xmmword ptr [esp+160]
; CHECK: movups  xmm2, xmmword ptr [esp+144]
; CHECK: movups  xmm3, xmmword ptr [esp+128]
; CHECK: call InterspersedVectorArgs
; CHECK-NEXT: add esp, 80
; CHECK: ret

; OPTM1-LABEL: test_passing_vectors_interspersed:
; OPTM1: sub esp, 80
; OPTM1: movups  [[ARG9:.*]], xmmword ptr {{.*}}
; OPTM1: movups  xmmword ptr [esp+32], [[ARG9]]
; OPTM1: movups  [[ARG11:.*]], xmmword ptr {{.*}}
; OPTM1: movups  xmmword ptr [esp+64], [[ARG11]]
; OPTM1: movups  xmm0, xmmword ptr {{.*}}
; OPTM1: movups  xmm1, xmmword ptr {{.*}}
; OPTM1: movups  xmm2, xmmword ptr {{.*}}
; OPTM1: movups  xmm3, xmmword ptr {{.*}}
; OPTM1: call InterspersedVectorArgs
; OPTM1-NEXT: add esp, 80
; OPTM1: ret
}

; Test that a vector returned from a function is recognized to be in
; xmm0.

declare <4 x float> @VectorReturn(<4 x float> %arg0)

define void @test_receiving_vectors(<4 x float> %arg0) {
entry:
  %result = call <4 x float> @VectorReturn(<4 x float> %arg0)
  %result2 = call <4 x float> @VectorReturn(<4 x float> %result)
  ret void
; CHECK-LABEL: test_receiving_vectors:
; CHECK: call VectorReturn
; CHECK-NOT: movups xmm0
; CHECK: call VectorReturn
; CHECK: ret

; OPTM1-LABEL: test_receiving_vectors:
; OPTM1: call VectorReturn
; OPTM1: movups {{.*}}, xmm0
; OPTM1: movups xmm0, {{.*}}
; OPTM1: call VectorReturn
; OPTM1: ret
}

; ERRORS-NOT: ICE translation error
; DUMP-NOT: SZ
