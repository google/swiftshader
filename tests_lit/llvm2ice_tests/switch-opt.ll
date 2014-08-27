; This tests a switch statement, including multiple branches to the
; same label which also results in phi instructions with multiple
; entries for the same incoming edge.

; RUN: %llvm2ice --verbose inst %s | FileCheck %s
; RUN: %llvm2ice --verbose none %s | FileCheck --check-prefix=ERRORS %s
; RUN: %llvm2iceinsts %s | %szdiff %s | FileCheck --check-prefix=DUMP %s
; RUN: %llvm2iceinsts --pnacl %s | %szdiff %s \
; RUN:                           | FileCheck --check-prefix=DUMP %s

define i32 @testSwitch(i32 %a) {
entry:
  switch i32 %a, label %sw.default [
    i32 1, label %sw.epilog
    i32 2, label %sw.epilog
    i32 3, label %sw.epilog
    i32 7, label %sw.bb1
    i32 8, label %sw.bb1
    i32 15, label %sw.bb2
    i32 14, label %sw.bb2
  ]

sw.default:                                       ; preds = %entry
  %add = add i32 %a, 27
  br label %sw.epilog

sw.bb1:                                           ; preds = %entry, %entry
  %phitmp = sub i32 21, %a
  br label %sw.bb2

sw.bb2:                                           ; preds = %sw.bb1, %entry, %entry
  %result.0 = phi i32 [ 1, %entry ], [ 1, %entry ], [ %phitmp, %sw.bb1 ]
  br label %sw.epilog

sw.epilog:                                        ; preds = %sw.bb2, %sw.default, %entry, %entry, %entry
  %result.1 = phi i32 [ %add, %sw.default ], [ %result.0, %sw.bb2 ], [ 17, %entry ], [ 17, %entry ], [ 17, %entry ]
  ret i32 %result.1
}

; Check for a valid addressing mode when the switch operand is an
; immediate.  It's important that there is exactly one case, because
; for two or more cases the source operand is legalized into a
; register.
define i32 @testSwitchImm() {
entry:
  switch i32 10, label %sw.default [
    i32 1, label %sw.default
  ]

sw.default:
  ret i32 20
}
; CHECK-LABEL: testSwitchImm
; CHECK-NOT: cmp {{[0-9]*}},

; CHECK-NOT: ICE translation error
; ERRORS-NOT: ICE translation error
; DUMP-NOT: SZ
