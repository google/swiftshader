; This tests the basic structure of the Unreachable instruction.

; RUN: %p2i -i %s --assemble --disassemble -a -O2 --verbose none \
; RUN:   | FileCheck %s
; RUN: %p2i -i %s --assemble --disassemble -a -Om1 --verbose none \
; RUN:   | FileCheck %s

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
; CHECK: call {{.*}} R_{{.*}} ice_unreachable
; CHECK: cdq
; CHECK: idiv
; CHECK: ret
