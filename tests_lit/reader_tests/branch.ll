; Tests if we handle a branch instructions.

; RUN: llvm-as < %s | pnacl-freeze -allow-local-symbol-tables \
; RUN:              | %llvm2ice -notranslate -verbose=inst -build-on-read \
; RUN:                -allow-pnacl-reader-error-recovery \
; RUN:                -allow-local-symbol-tables \
; RUN:              | FileCheck %s

define void @SimpleBranch() {
entry:
  br label %b3
b1:
  br label %b2
b2:
  ret void
b3:
  br label %b1
}

; CHECK:      define void @SimpleBranch() {
; CHECK-NEXT: entry:
; CHECK-NEXT:   br label %b3
; CHECK-NEXT: b1:
; CHECK-NEXT:   br label %b2
; CHECK-NEXT: b2:
; CHECK-NEXT:   ret void
; CHECK-NEXT: b3:
; CHECK-NEXT:   br label %b1
; CHECK-NEXT: }

define void @CondBranch(i32 %p) {
entry:
  %test = trunc i32 %p to i1
  br i1 %test, label %b1, label %b2
b1:
  ret void
b2:
  br i1 %test, label %b2, label %b1
}

; CHECK-NEXT: define void @CondBranch(i32 %p) {
; CHECK-NEXT: entry:
; CHECK-NEXT:   %test = trunc i32 %p to i1
; CHECK-NEXT:   br i1 %test, label %b1, label %b2
; CHECK-NEXT: b1:
; CHECK-NEXT:   ret void
; CHECK-NEXT: b2:
; CHECK-NEXT:   br i1 %test, label %b2, label %b1
; CHECK-NEXT: }
