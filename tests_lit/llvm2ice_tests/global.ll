; Trivial test of the use of internal versus external global
; functions.

; RUN: %llvm2ice --verbose inst %s | FileCheck %s
; RUN: %llvm2ice --verbose none %s | FileCheck --check-prefix=ERRORS %s
; RUN: %llvm2iceinsts %s | %szdiff %s | FileCheck --check-prefix=DUMP %s

; Note: We don't run this test using a PNaCl bitcode file, because
; external globals are not in the PNaCl ABI.

@intern_global = global [4 x i8] [i8 0, i8 0, i8 0, i8 12], align 4
@extern_global = external global [4 x i8]

define i32 @test_intern_global() {
; CHECK: define i32 @test_intern_global
entry:
  %__1 = bitcast [4 x i8]* @intern_global to i32*
  %v0 = load i32* %__1, align 1
  ret i32 %v0
}

define i32 @test_extern_global() {
; CHECK: define i32 @test_extern_global
entry:
  %__1 = bitcast [4 x i8]* @extern_global to i32*
  %v0 = load i32* %__1, align 1
  ret i32 %v0
}

; ERRORS-NOT: ICE translation error
; DUMP-NOT: SZ
