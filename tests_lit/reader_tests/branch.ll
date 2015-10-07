; Tests if we handle a branch instructions.

; RUN: %p2i -i %s --insts | FileCheck %s
; RUN: %if --need=allow_disable_ir_gen --command \
; RUN:   %p2i -i %s --args -notranslate -timing -no-ir-gen \
; RUN: | %if --need=allow_disable_ir_gen --command \
; RUN:   FileCheck --check-prefix=NOIR %s

define internal void @SimpleBranch() {
entry:
  br label %b3
b1:
  br label %b2
b2:
  ret void
b3:
  br label %b1
}

; CHECK:      define internal void @SimpleBranch() {
; CHECK-NEXT: entry:
; CHECK-NEXT:   br label %b3
; CHECK-NEXT: b1:
; CHECK-NEXT:   br label %b2
; CHECK-NEXT: b2:
; CHECK-NEXT:   ret void
; CHECK-NEXT: b3:
; CHECK-NEXT:   br label %b1
; CHECK-NEXT: }

define internal void @CondBranch(i32 %p) {
entry:
  %test = trunc i32 %p to i1
  br i1 %test, label %b1, label %b2
b1:
  ret void
b2:
  br i1 %test, label %b2, label %b1
}

; CHECK-NEXT: define internal void @CondBranch(i32 %p) {
; CHECK-NEXT: entry:
; CHECK-NEXT:   %test = trunc i32 %p to i1
; CHECK-NEXT:   br i1 %test, label %b1, label %b2
; CHECK-NEXT: b1:
; CHECK-NEXT:   ret void
; CHECK-NEXT: b2:
; CHECK-NEXT:   br i1 %test, label %b2, label %b1
; CHECK-NEXT: }

; NOIR: Total across all functions
