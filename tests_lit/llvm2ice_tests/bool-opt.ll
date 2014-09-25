; Trivial smoke test of icmp without fused branch opportunity.

; RUN: %p2i -i %s --args --verbose inst | FileCheck %s
; RUN: %p2i -i %s --args --verbose none | FileCheck --check-prefix=ERRORS %s
; RUN: %p2i -i %s --insts | %szdiff %s | FileCheck --check-prefix=DUMP %s

define void @testBool(i32 %a, i32 %b) {
entry:
  %cmp = icmp eq i32 %a, %b
  %cmp_ext = zext i1 %cmp to i32
  tail call void @use(i32 %cmp_ext)
  ret void
}

; Check that correct addressing modes are used for comparing two
; immediates.
define void @testIcmpImm() {
entry:
  %cmp = icmp eq i32 1, 2
  %cmp_ext = zext i1 %cmp to i32
  tail call void @use(i32 %cmp_ext)
  ret void
}
; CHECK-LABEL: testIcmpImm
; CHECK-NOT: cmp {{[0-9]+}},

declare void @use(i32)

; CHECK-NOT: ICE translation error
; ERRORS-NOT: ICE translation error
; DUMP-NOT: SZ
