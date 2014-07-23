; This tests the basic structure of the Unreachable instruction.

; RUN: %llvm2ice --verbose inst %s | FileCheck %s
; RUN: %llvm2ice --verbose none %s \
; RUN:                 | llvm-mc -arch=x86 -x86-asm-syntax=intel -filetype=obj
; RUN: %llvm2ice --verbose none %s | FileCheck --check-prefix=ERRORS %s
; RUN: %llvm2iceinsts %s | %szdiff %s | FileCheck --check-prefix=DUMP %s
; RUN: %llvm2iceinsts --pnacl %s | %szdiff %s \
; RUN:                           | FileCheck --check-prefix=DUMP %s

define internal i32 @divide(i32 %num, i32 %den) {
entry:
  %cmp = icmp ne i32 %den, 0
  br i1 %cmp, label %return, label %abort

abort:                                            ; preds = %entry
  unreachable
; CHECK: unreachable

return:                                           ; preds = %entry
  %div = sdiv i32 %num, %den
  ret i32 %div
}

; CHECK: cmp
; CHECK: call ice_unreachable
; CHECK: cdq
; CHECK: idiv
; CHECK: ret

; ERRORS-NOT: ICE translation error
; DUMP-NOT: SZ
