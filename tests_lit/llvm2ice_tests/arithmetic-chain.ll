; This is a very early test that just checks the representation of
; arithmetic instructions, i64, variables, and constants.  No assembly
; tests are done.

; RUN: %p2i -i %s --args --verbose inst | FileCheck %s
; RUN: %p2i -i %s --args --verbose none | FileCheck --check-prefix=ERRORS %s
; RUN: %p2i -i %s --insts | %szdiff %s | FileCheck --check-prefix=DUMP %s

define i64 @arithmetic_chain(i64 %foo, i64 %bar) {
entry:
  %r1 = add i64 %foo, %bar
  %r2 = add i64 %foo, %r1
  %r3 = mul i64 %bar, %r1
  %r4 = shl i64 %r3, %r2
  %r5 = add i64 %r4, 8
  ret i64 %r5

; CHECK:      entry:
; CHECK-NEXT:  %r1 = add i64 %foo, %bar
; CHECK-NEXT:  %r2 = add i64 %foo, %r1
; CHECK-NEXT:  %r3 = mul i64 %bar, %r1
; CHECK-NEXT:  %r4 = shl i64 %r3, %r2
; CHECK-NEXT:  %r5 = add i64 %r4, 8
; CHECK-NEXT:  ret i64 %r5
}

; ERRORS-NOT: ICE translation error
; DUMP-NOT: SZ
