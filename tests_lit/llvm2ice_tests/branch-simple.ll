; 1. Trivial smoke test of compare and branch, with multiple basic
; blocks.
; 2. For a conditional branch on a constant boolean value, make sure
; we don't lower to a cmp instructions with an immediate as the first
; source operand.

; RUN: %llvm2ice -O2 --verbose inst %s | FileCheck %s
; RUN: %llvm2ice -Om1 --verbose inst %s | FileCheck %s
; RUN: %llvm2ice --verbose none %s | FileCheck --check-prefix=ERRORS %s
; RUN: %llvm2iceinsts %s | %szdiff %s | FileCheck --check-prefix=DUMP %s
; RUN: %llvm2iceinsts --pnacl %s | %szdiff %s \
; RUN:                           | FileCheck --check-prefix=DUMP %s

define i32 @simple_cond_branch(i32 %foo, i32 %bar) {
entry:
  %r1 = icmp eq i32 %foo, %bar
  br i1 %r1, label %Equal, label %Unequal
Equal:
  ret i32 %foo
Unequal:
  ret i32 %bar
; CHECK-LABEL: simple_cond_branch
; CHECK: br i1 %r1, label %Equal, label %Unequal
; CHECK: Equal:
; CHECK:  ret i32 %foo
; CHECK: Unequal:
; CHECK:  ret i32 %bar
}

define internal i32 @test_br_const() {
__0:
  br i1 true, label %__1, label %__2
__1:
  ret i32 21
__2:
  ret i32 43
}
; CHECK-LABEL: test_br_const
; CHECK-NOT: cmp {{[0-9]*}},

; ERRORS-NOT: ICE translation error
; DUMP-NOT: SZ
