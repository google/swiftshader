; Test parsing unreachable instruction.

; RUN: %p2i -i %s --insts | FileCheck %s
; RUN: %if --need=allow_disable_ir_gen --command \
; RUN:   %p2i -i %s --args -notranslate -timing -no-ir-gen \
; RUN: | %if --need=allow_disable_ir_gen --command \
; RUN:   FileCheck --check-prefix=NOIR %s

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

; CHECK:      define internal i32 @divide(i32 %num, i32 %den) {
; CHECK-NEXT: entry:
; CHECK-NEXT:   %cmp = icmp ne i32 %den, 0
; CHECK-NEXT:   br i1 %cmp, label %return, label %abort
; CHECK-NEXT: abort:
; CHECK-NEXT:   unreachable
; CHECK-NEXT: return:
; CHECK-NEXT:   %div = sdiv i32 %num, %den
; CHECK-NEXT:   ret i32 %div
; CHECK-NEXT: }

; NOIR: Total across all functions
