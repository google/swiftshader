; This tests the basic structure of the Unreachable instruction.

; TODO(jvoung): fix extra "CALLTARGETS" run. The llvm-objdump symbolizer
; doesn't know how to symbolize non-section-local functions.
; The newer LLVM 3.6 one does work, but watch out for other bugs.

; RUN: %llvm2ice -O2 --verbose none %s \
; RUN:   | FileCheck --check-prefix=CALLTARGETS %s
; RUN: %llvm2ice -O2 --verbose none %s \
; RUN:   | llvm-mc -triple=i686-none-nacl -x86-asm-syntax=intel -filetype=obj \
; RUN:   | llvm-objdump -d --symbolize -x86-asm-syntax=intel - | FileCheck %s
; RUN: %llvm2ice -Om1 --verbose none %s \
; RUN:   | llvm-mc -triple=i686-none-nacl -x86-asm-syntax=intel -filetype=obj \
; RUN:   | llvm-objdump -d --symbolize -x86-asm-syntax=intel - | FileCheck %s
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

return:                                           ; preds = %entry
  %div = sdiv i32 %num, %den
  ret i32 %div
}

; CHECK-LABEL: divide
; CALLTARGETS-LABEL: divide
; CHECK: cmp
; CHECK: call -4
; CALLTARGETS: call ice_unreachable
; CHECK: cdq
; CHECK: idiv
; CHECK: ret

; ERRORS-NOT: ICE translation error
; DUMP-NOT: SZ
