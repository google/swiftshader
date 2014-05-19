; RUIN: %llvm2ice %s -verbose inst | FileCheck %s
; RUIN: %llvm2ice --verbose none %s | FileCheck --check-prefix=ERRORS %s
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
; CHECK: br i1 %r1, label %Equal, label %Unequal
; CHECK: Equal:
; CHECK:  ret i32 %foo
; CHECK: Unequal:
; CHECK:  ret i32 %bar
}

; ERRORS-NOT: ICE translation error
; DUMP-NOT: SZ
