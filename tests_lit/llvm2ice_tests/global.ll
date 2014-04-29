; RUIN: %llvm2ice -verbose inst %s | FileCheck %s
; RUIN: %llvm2ice --verbose none %s | FileCheck --check-prefix=ERRORS %s
; RUN: %szdiff --llvm2ice=%llvm2ice %s | FileCheck --check-prefix=DUMP %s

@intern_global = global i32 12, align 4
@extern_global = external global i32

define i32 @test_intern_global() {
; CHECK: define i32 @test_intern_global
entry:
  %v0 = load i32* @intern_global, align 1
  ret i32 %v0
}

define i32 @test_extern_global() {
; CHECK: define i32 @test_extern_global
entry:
  %v0 = load i32* @extern_global, align 1
  ret i32 %v0
}

; ERRORS-NOT: ICE translation error
; DUMP-NOT: SZ
