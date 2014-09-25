; Trivial structural test of 64-bit icmp instructions.

; RUN: %p2i -i %s --args --verbose inst | FileCheck %s
; RUN: %p2i -i %s --args --verbose none | FileCheck --check-prefix=ERRORS %s
; RUN: %p2i -i %s --insts | %szdiff %s | FileCheck --check-prefix=DUMP %s

define void @dummy_icmp(i64 %foo, i64 %bar) {
; CHECK: define void @dummy_icmp
entry:
  %r1 = icmp eq i64 %foo, %bar
  %r2 = icmp slt i64 %foo, %bar
  ret void
; CHECK:       entry:
; CHECK-NEXT:   %r1 = icmp eq i64 %foo, %bar
; CHECK-NEXT:   %r2 = icmp slt i64 %foo, %bar
; CHECK-NEXT:   ret void 
}

; ERRORS-NOT: ICE translation error
; DUMP-NOT: SZ
