; Trivial smoke test of icmp without fused branch opportunity.

; RUN: %llvm2ice --verbose inst %s | FileCheck %s
; RUN: %llvm2ice --verbose none %s | FileCheck --check-prefix=ERRORS %s
; RUN: %llvm2iceinsts %s | %szdiff %s | FileCheck --check-prefix=DUMP %s
; RUN: %llvm2iceinsts --pnacl %s | %szdiff %s \
; RUN:                           | FileCheck --check-prefix=DUMP %s

define void @testBool(i32 %a, i32 %b) {
entry:
  %cmp = icmp eq i32 %a, %b
  %cmp_ext = zext i1 %cmp to i32
  tail call void @use(i32 %cmp_ext)
  ret void
}

declare void @use(i32)

; CHECK-NOT: ICE translation error
; ERRORS-NOT: ICE translation error
; DUMP-NOT: SZ
