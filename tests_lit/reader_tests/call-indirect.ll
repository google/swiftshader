; Test parsing indirect calls in Subzero.

; RUN: llvm-as < %s | pnacl-freeze -allow-local-symbol-tables \
; RUN:              | %llvm2ice -notranslate -verbose=inst -build-on-read \
; RUN:                -allow-pnacl-reader-error-recovery \
; RUN:                -allow-local-symbol-tables \
; RUN:              | FileCheck %s

define internal void @CallIndirectVoid(i32 %f_addr) {
entry:
  %f = inttoptr i32 %f_addr to void ()*
  call void %f()
  ret void
}

; CHECK:      define internal void @CallIndirectVoid(i32 %f_addr) {
; CHECK-NEXT: entry:
; CHECK-NEXT:   call void %f_addr()
; CHECK-NEXT:   ret void
; CHECK-NEXT: }

define internal i32 @CallIndirectI32(i32 %f_addr) {
entry:
  %f = inttoptr i32 %f_addr to i32(i8, i1)*
  %r = call i32 %f(i8 1, i1 false)
  ret i32 %r
}

; CHECK-NEXT: define internal i32 @CallIndirectI32(i32 %f_addr) {
; CHECK-NEXT: entry:
; CHECK-NEXT:   %r = call i32 %f_addr(i8 1, i1 false)
; CHECK-NEXT:   ret i32 %r
; CHECK-NEXT: }
