; RUIN: %llvm2ice -verbose inst %s | FileCheck %s
; RUIN: %llvm2ice --verbose none %s | FileCheck --check-prefix=ERRORS %s
; RUN: %szdiff --llvm2ice=%llvm2ice %s | FileCheck --check-prefix=DUMP %s

define i32 @Add(i32 %a, i32 %b) {
; CHECK: define i32 @Add
entry:
  %add = add i32 %b, %a
; CHECK: add
  tail call void @Use(i32 %add)
; CHECK: call Use
  ret i32 %add
}

declare void @Use(i32)

define i32 @And(i32 %a, i32 %b) {
; CHECK: define i32 @And
entry:
  %and = and i32 %b, %a
; CHECK: and
  tail call void @Use(i32 %and)
; CHECK: call Use
  ret i32 %and
}

define i32 @Or(i32 %a, i32 %b) {
; CHECK: define i32 @Or
entry:
  %or = or i32 %b, %a
; CHECK: or
  tail call void @Use(i32 %or)
; CHECK: call Use
  ret i32 %or
}

define i32 @Xor(i32 %a, i32 %b) {
; CHECK: define i32 @Xor
entry:
  %xor = xor i32 %b, %a
; CHECK: xor
  tail call void @Use(i32 %xor)
; CHECK: call Use
  ret i32 %xor
}

define i32 @Sub(i32 %a, i32 %b) {
; CHECK: define i32 @Sub
entry:
  %sub = sub i32 %a, %b
; CHECK: sub
  tail call void @Use(i32 %sub)
; CHECK: call Use
  ret i32 %sub
}

define i32 @Mul(i32 %a, i32 %b) {
; CHECK: define i32 @Mul
entry:
  %mul = mul i32 %b, %a
; CHECK: imul
  tail call void @Use(i32 %mul)
; CHECK: call Use
  ret i32 %mul
}

define i32 @Sdiv(i32 %a, i32 %b) {
; CHECK: define i32 @Sdiv
entry:
  %div = sdiv i32 %a, %b
; CHECK: cdq
; CHECK: idiv
  tail call void @Use(i32 %div)
; CHECK: call Use
  ret i32 %div
}

define i32 @Srem(i32 %a, i32 %b) {
; CHECK: define i32 @Srem
entry:
  %rem = srem i32 %a, %b
; CHECK: cdq
; CHECK: idiv
  tail call void @Use(i32 %rem)
; CHECK: call Use
  ret i32 %rem
}

define i32 @Udiv(i32 %a, i32 %b) {
; CHECK: define i32 @Udiv
entry:
  %div = udiv i32 %a, %b
; CHECK: div
  tail call void @Use(i32 %div)
; CHECK: call Use
  ret i32 %div
}

define i32 @Urem(i32 %a, i32 %b) {
; CHECK: define i32 @Urem
entry:
  %rem = urem i32 %a, %b
; CHECK: div
  tail call void @Use(i32 %rem)
; CHECK: call Use
  ret i32 %rem
}

; ERRORS-NOT: ICE translation error
; DUMP-NOT: SZ
