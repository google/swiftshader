; Trivial smoke test of icmp without fused branch opportunity.

; RUN: %p2i -i %s --args --verbose none | FileCheck %s

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
