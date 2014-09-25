; Test that functions are aligned to the NaCl bundle alignment.
; We could be smarter and only do this for indirect call targets
; but typically you want to align functions anyway.
; Also, we are currently using hlts for non-executable padding.

; RUN: %p2i -i %s --args -O2 --verbose none \
; RUN:   | llvm-mc -triple=i686-none-nacl -x86-asm-syntax=intel -filetype=obj \
; RUN:   | llvm-objdump -d --symbolize -x86-asm-syntax=intel - | FileCheck %s

define void @foo() {
  ret void
}
; CHECK-LABEL: foo
; CHECK-NEXT: 0: {{.*}} ret
; CHECK-NEXT: 1: {{.*}} hlt

define void @bar() {
  ret void
}
; CHECK-LABEL: bar
; CHECK-NEXT: 20: {{.*}} ret
