; This tests the basic structure of the Unreachable instruction.

; RUN: %p2i -i %s --filetype=obj --disassemble -a -O2 | FileCheck %s
; RUN: %p2i -i %s --filetype=obj --disassemble -a -Om1 | FileCheck %s

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
; CHECK: cmp
; CHECK: ud2
; CHECK: cdq
; CHECK: idiv
; CHECK: ret
