; This checks the correctness of the lowering code for the small
; integer variants of sdiv and srem.

; RUN: %p2i -i %s --args -O2 --verbose none \
; RUN:   | llvm-mc -triple=i686-none-nacl -filetype=obj \
; RUN:   | llvm-objdump -d --symbolize -x86-asm-syntax=intel - | FileCheck %s
; RUN: %p2i -i %s --args -Om1 --verbose none \
; RUN:   | llvm-mc -triple=i686-none-nacl -filetype=obj \
; RUN:   | llvm-objdump -d --symbolize -x86-asm-syntax=intel - | FileCheck %s

define i32 @sdiv_i8(i32 %a.i32, i32 %b.i32) {
entry:
  %a = trunc i32 %a.i32 to i8
  %b = trunc i32 %b.i32 to i8
  %res = sdiv i8 %a, %b
  %res.i32 = zext i8 %res to i32
  ret i32 %res.i32
; CHECK-LABEL: sdiv_i8:
; CHECK: cbw
; CHECK: idiv
}

define i32 @sdiv_i16(i32 %a.i32, i32 %b.i32) {
entry:
  %a = trunc i32 %a.i32 to i16
  %b = trunc i32 %b.i32 to i16
  %res = sdiv i16 %a, %b
  %res.i32 = zext i16 %res to i32
  ret i32 %res.i32
; CHECK-LABEL: sdiv_i16:
; CHECK: cwd
; CHECK: idiv
}

define i32 @sdiv_i32(i32 %a, i32 %b) {
entry:
  %res = sdiv i32 %a, %b
  ret i32 %res
; CHECK-LABEL: sdiv_i32:
; CHECK: cdq
; CHECK: idiv
}

define i32 @srem_i8(i32 %a.i32, i32 %b.i32) {
entry:
  %a = trunc i32 %a.i32 to i8
  %b = trunc i32 %b.i32 to i8
  %res = srem i8 %a, %b
  %res.i32 = zext i8 %res to i32
  ret i32 %res.i32
; CHECK-LABEL: srem_i8:
; CHECK: cbw
; CHECK: idiv
}

define i32 @srem_i16(i32 %a.i32, i32 %b.i32) {
entry:
  %a = trunc i32 %a.i32 to i16
  %b = trunc i32 %b.i32 to i16
  %res = srem i16 %a, %b
  %res.i32 = zext i16 %res to i32
  ret i32 %res.i32
; CHECK-LABEL: srem_i16:
; CHECK: cwd
; CHECK: idiv
}

define i32 @srem_i32(i32 %a, i32 %b) {
entry:
  %res = srem i32 %a, %b
  ret i32 %res
; CHECK-LABEL: srem_i32:
; CHECK: cdq
; CHECK: idiv
}
