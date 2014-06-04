; Simple test of the select instruction.  The CHECK lines are only
; checking for basic instruction patterns that should be present
; regardless of the optimization level, so there are no special OPTM1
; match lines.

; RUN: %llvm2ice -O2 --verbose none %s | FileCheck %s
; RUN: %llvm2ice -Om1 --verbose none %s | FileCheck %s
; RUN: %llvm2ice --verbose none %s | FileCheck --check-prefix=ERRORS %s
; RUN: %llvm2iceinsts %s | %szdiff %s | FileCheck --check-prefix=DUMP %s
; RUN: %llvm2iceinsts --pnacl %s | %szdiff %s \
; RUN:                           | FileCheck --check-prefix=DUMP %s

define void @testSelect(i32 %a, i32 %b) {
entry:
  %cmp = icmp slt i32 %a, %b
  %cond = select i1 %cmp, i32 %a, i32 %b
  tail call void @useInt(i32 %cond)
  %cmp1 = icmp sgt i32 %a, %b
  %cond2 = select i1 %cmp1, i32 10, i32 20
  tail call void @useInt(i32 %cond2)
  ret void
}

declare void @useInt(i32)

; CHECK:      .globl testSelect
; CHECK:      cmp
; CHECK:      cmp
; CHECK:      call useInt
; CHECK:      cmp
; CHECK:      cmp
; CHECK:      call useInt
; CHECK:      ret

; ERRORS-NOT: ICE translation error
; DUMP-NOT: SZ
