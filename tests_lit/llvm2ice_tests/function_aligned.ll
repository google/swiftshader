; Test that functions are aligned to the NaCl bundle alignment.
; We could be smarter and only do this for indirect call targets
; but typically you want to align functions anyway.
; Also, we are currently using hlts for non-executable padding.

; RUN: %p2i --filetype=obj --disassemble -i %s --args -O2 | FileCheck %s

; TODO(jvoung): Stop skipping unimplemented parts (via --skip-unimplemented)
; once enough infrastructure is in. Also, switch to --filetype=obj
; when possible.
; RUN: %if --need=target_ARM32 --need=allow_dump \
; RUN:   --command %p2i --filetype=asm --assemble \
; RUN:   --disassemble --target arm32 -i %s --args -O2 --skip-unimplemented \
; RUN:   | %if --need=target_ARM32 --need=allow_dump \
; RUN:   --command FileCheck --check-prefix ARM32 %s

define void @foo() {
  ret void
}
; CHECK-LABEL: foo
; CHECK-NEXT: 0: {{.*}} ret
; CHECK-NEXT: 1: {{.*}} hlt
; ARM32-LABEL: foo
; ARM32-NEXT: 0: {{.*}} bx lr
; ARM32-NEXT: 4: e7fedef0 udf
; ARM32-NEXT: 8: e7fedef0 udf
; ARM32-NEXT: c: e7fedef0 udf

define void @bar() {
  ret void
}
; CHECK-LABEL: bar
; CHECK-NEXT: 20: {{.*}} ret
; ARM32-LABEL: bar
; ARM32-NEXT: 10: {{.*}} bx lr
