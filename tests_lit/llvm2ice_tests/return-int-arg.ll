; RUIN: %llvm2ice -verbose inst %s | FileCheck %s
; RUIN: %llvm2ice --verbose none %s | FileCheck --check-prefix=ERRORS %s
; RUN: %llvm2iceinsts %s | %szdiff %s | FileCheck --check-prefix=DUMP %s
; RUN: %llvm2iceinsts --pnacl %s | %szdiff %s \
; RUN:                           | FileCheck --check-prefix=DUMP %s

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
