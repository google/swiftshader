; Test that even if a call return type matches its declaration, it must still be
; a legal call return type (unless declaration is intrinsic).

; REQUIRES: no_minimal_build

; RUN: %p2i --expect-fail -i %s --insts | FileCheck %s

declare i1 @f();

define void @Test() {
entry:
  %v = call i1 @f()
; CHECK: Return type of f is invalid: i1
  ret void
}

