; This tests the basic structure of the Unreachable instruction.

; RUN: %if --need=target_X8632 --command %p2i --filetype=obj --disassemble \
; RUN:   --target x8632 -i %s --args -O2 \
; RUN:   | %if --need=target_X8632 --command FileCheck %s
; RUN: %if --need=target_X8632 --command %p2i --filetype=obj --disassemble \
; RUN:   --target x8632 -i %s --args -Om1 \
; RUN:   | %if --need=target_X8632 --command FileCheck %s

; RUN: %if --need=target_ARM32 --need=allow_dump \
; RUN:   --command %p2i --filetype=asm --assemble \
; RUN:   --disassemble --target arm32 -i %s --args -O2 --skip-unimplemented \
; RUN:   | %if --need=target_ARM32 --need=allow_dump \
; RUN:   --command FileCheck --check-prefix ARM32 %s
; RUN: %if --need=target_ARM32 --need=allow_dump \
; RUN:   --command %p2i --filetype=asm --assemble \
; RUN:   --disassemble --target arm32 -i %s --args -Om1 --skip-unimplemented \
; RUN:   | %if --need=target_ARM32 --need=allow_dump \
; RUN:   --command FileCheck --check-prefix ARM32 %s

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

; ARM32-LABEL: divide
; ARM32: tst
; ARM32: .word 0xe7fedef0
; ARM32: bl {{.*}} __divsi3
; ARM32: bx lr
