; This is a regression test that idiv and div operands are legalized
; (they cannot be constants and can only be reg/mem for x86).

; RUN: %p2i -i %s --args -O2 --verbose none \
; RUN:   | llvm-mc -triple=i686-none-nacl -filetype=obj \
; RUN:   | llvm-objdump -d --symbolize -x86-asm-syntax=intel - | FileCheck %s
; RUN: %p2i -i %s --args -Om1 --verbose none \
; RUN:   | llvm-mc -triple=i686-none-nacl -filetype=obj \
; RUN:   | llvm-objdump -d --symbolize -x86-asm-syntax=intel - | FileCheck %s

define i32 @Sdiv_const8_b(i8 %a) {
; CHECK-LABEL: Sdiv_const8_b
entry:
  %div = sdiv i8 %a, 12
; CHECK: mov {{.*}}, 12
; CHECK-NOT: idiv 12
  %div_ext = sext i8 %div to i32
  ret i32 %div_ext
}

define i32 @Sdiv_const16_b(i16 %a) {
; CHECK-LABEL: Sdiv_const16_b
entry:
  %div = sdiv i16 %a, 1234
; CHECK: mov {{.*}}, 1234
; CHECK-NOT: idiv 1234
  %div_ext = sext i16 %div to i32
  ret i32 %div_ext
}

define i32 @Sdiv_const32_b(i32 %a) {
; CHECK-LABEL: Sdiv_const32_b
entry:
  %div = sdiv i32 %a, 1234
; CHECK: mov {{.*}}, 1234
; CHECK-NOT: idiv 1234
  ret i32 %div
}

define i32 @Srem_const_b(i32 %a) {
; CHECK-LABEL: Srem_const_b
entry:
  %rem = srem i32 %a, 2345
; CHECK: mov {{.*}}, 2345
; CHECK-NOT: idiv 2345
  ret i32 %rem
}

define i32 @Udiv_const_b(i32 %a) {
; CHECK-LABEL: Udiv_const_b
entry:
  %div = udiv i32 %a, 3456
; CHECK: mov {{.*}}, 3456
; CHECK-NOT: div 3456
  ret i32 %div
}

define i32 @Urem_const_b(i32 %a) {
; CHECK-LABEL: Urem_const_b
entry:
  %rem = urem i32 %a, 4567
; CHECK: mov {{.*}}, 4567
; CHECK-NOT: div 4567
  ret i32 %rem
}
