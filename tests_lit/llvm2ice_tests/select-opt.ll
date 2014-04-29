; RUIN: %llvm2ice %s | FileCheck %s
; RUIN: %llvm2ice --verbose none %s | FileCheck --check-prefix=ERRORS %s
; RUN: %szdiff --llvm2ice=%llvm2ice %s | FileCheck --check-prefix=DUMP %s

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
