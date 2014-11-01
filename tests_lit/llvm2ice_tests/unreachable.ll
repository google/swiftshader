; This tests the basic structure of the Unreachable instruction.

; TODO(jvoung): fix extra "CALLTARGETS" run. The llvm-objdump symbolizer
; doesn't know how to symbolize non-section-local functions.
; The newer LLVM 3.6 one does work, but watch out for other bugs.

; RUN: %p2i -i %s -a -O2 --verbose none \
; RUN:   | FileCheck --check-prefix=CALLTARGETS %s
; RUN: %p2i -i %s -a -O2 --verbose none \
; RUN:   | llvm-mc -triple=i686-none-nacl -filetype=obj \
; RUN:   | llvm-objdump -d --symbolize -x86-asm-syntax=intel - | FileCheck %s
; RUN: %p2i -i %s -a -Om1 --verbose none \
; RUN:   | llvm-mc -triple=i686-none-nacl -filetype=obj \
; RUN:   | llvm-objdump -d --symbolize -x86-asm-syntax=intel - | FileCheck %s
; RUN: %p2i -i %s -a --verbose none | FileCheck --check-prefix=ERRORS %s
; RUN: %p2i -i %s --insts | %szdiff %s | FileCheck --check-prefix=DUMP %s

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
