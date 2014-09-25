; Simple test of functions returning one of its arguments.

; RUN: %p2i -i %s --args --verbose inst | FileCheck %s
; RUN: %p2i -i %s --args --verbose none | FileCheck --check-prefix=ERRORS %s
; RUN: %p2i -i %s --insts | %szdiff %s | FileCheck --check-prefix=DUMP %s

define i32 @func_single_arg(i32 %a) {
; CHECK: define i32 @func_single_arg
entry:
  ret i32 %a
; CHECK: ret i32 %a
}

define i32 @func_multiple_args(i32 %a, i32 %b, i32 %c) {
; CHECK: func_multiple_args
entry:
  ret i32 %c
; CHECK: ret i32 %c
}

; ERRORS-NOT: ICE translation error
; DUMP-NOT: SZ
