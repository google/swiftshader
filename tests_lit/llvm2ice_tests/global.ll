; Trivial test of the use of internal versus external global
; variables.

; TODO(kschimpf) find out why lc2i is needed.
; RUN: %lc2i -i %s --args --verbose inst | FileCheck %s
; RUN: %lc2i -i %s --args --verbose none | FileCheck --check-prefix=ERRORS %s
; RUN: %lc2i -i %s --insts | %szdiff %s | FileCheck --check-prefix=DUMP %s

@intern_global = internal global [4 x i8] c"\00\00\00\0C", align 4
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
