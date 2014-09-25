; Trivial test of a trivial function.

; RUN: %p2i -i %s --args --verbose inst | FileCheck %s
; RUN: %p2i -i %s --args --verbose none | FileCheck --check-prefix=ERRORS %s
; RUN: %p2i -i %s --insts | %szdiff %s | FileCheck --check-prefix=DUMP %s

define void @foo() {
; CHECK: define void @foo()
entry:
  ret void
; CHECK: entry
; CHECK-NEXT: ret void
}

; ERRORS-NOT: ICE translation error
; DUMP-NOT: SZ
