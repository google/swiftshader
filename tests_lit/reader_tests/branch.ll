; Tests if we handle a branch instructions.

; RUN: llvm-as < %s | pnacl-freeze \
; RUN:              | %llvm2ice -notranslate -verbose=inst -build-on-read \
; RUN:                -allow-pnacl-reader-error-recovery \
; RUN:              | FileCheck %s

define void @SimpleBranch() {
  br label %b3
b1:
  br label %b2
b2:
  ret void
b3:
  br label %b1
}

; CHECK:      define void @SimpleBranch() {
; CHECK-NEXT: __0:
; CHECK-NEXT:   br label %__3
; CHECK-NEXT: __1:
; CHECK-NEXT:   br label %__2
; CHECK-NEXT: __2:
; CHECK-NEXT:   ret void
; CHECK-NEXT: __3:
; CHECK-NEXT:   br label %__1
; CHECK-NEXT: }

define void @CondBranch(i32 %p) {
  %test = trunc i32 %p to i1
  br i1 %test, label %b1, label %b2
b1:
  ret void
b2:
  br i1 %test, label %b2, label %b1
}

; CHECK-NEXT: define void @CondBranch(i32 %__0) {
; CHECK-NEXT: __0:
; CHECK-NEXT:   %__1 = trunc i32 %__0 to i1
; CHECK-NEXT:   br i1 %__1, label %__1, label %__2
; CHECK-NEXT: __1:
; CHECK-NEXT:   ret void
; CHECK-NEXT: __2:
; CHECK-NEXT:   br i1 %__1, label %__2, label %__1
; CHECK-NEXT: }
