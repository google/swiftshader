; This tries to be a comprehensive test of i8 operations.

; RUN: %p2i -i %s --args -O2 --verbose none \
; RUN:   | llvm-mc -triple=i686-none-nacl -x86-asm-syntax=intel -filetype=obj \
; RUN:   | llvm-objdump -d --symbolize -x86-asm-syntax=intel - | FileCheck %s
; RUN: %p2i -i %s --args -Om1 --verbose none \
; RUN:   | llvm-mc -triple=i686-none-nacl -x86-asm-syntax=intel -filetype=obj \
; RUN:   | llvm-objdump -d --symbolize -x86-asm-syntax=intel - | FileCheck %s
; RUN: %p2i -i %s --args --verbose none | FileCheck --check-prefix=ERRORS %s
; RUN: %p2i -i %s --insts | %szdiff %s | FileCheck --check-prefix=DUMP %s

define internal i32 @add8Bit(i32 %a, i32 %b) {
entry:
  %a_8 = trunc i32 %a to i8
  %b_8 = trunc i32 %b to i8
  %add = add i8 %b_8, %a_8
  %ret = zext i8 %add to i32
  ret i32 %ret
}
; CHECK-LABEL: add8Bit
; CHECK: add {{[abcd]l}}

define internal i32 @add8BitConst(i32 %a, i32 %b) {
entry:
  %a_8 = trunc i32 %a to i8
  %add = add i8 %a_8, 123
  %ret = zext i8 %add to i32
  ret i32 %ret
}
; CHECK-LABEL: add8BitConst
; CHECK: add {{[abcd]l}}

define internal i32 @sub8Bit(i32 %a, i32 %b) {
entry:
  %a_8 = trunc i32 %a to i8
  %b_8 = trunc i32 %b to i8
  %sub = sub i8 %b_8, %a_8
  %ret = zext i8 %sub to i32
  ret i32 %ret
}
; CHECK-LABEL: sub8Bit
; XCHECK: sub {{[abcd]l}}

define internal i32 @sub8BitConst(i32 %a, i32 %b) {
entry:
  %a_8 = trunc i32 %a to i8
  %sub = sub i8 %a_8, 123
  %ret = zext i8 %sub to i32
  ret i32 %ret
}
; CHECK-LABEL: sub8BitConst
; XCHECK: sub {{[abcd]l}}

define internal i32 @mul8Bit(i32 %a, i32 %b) {
entry:
  %a_8 = trunc i32 %a to i8
  %b_8 = trunc i32 %b to i8
  %mul = mul i8 %b_8, %a_8
  %ret = zext i8 %mul to i32
  ret i32 %ret
}
; CHECK-LABEL: mul8Bit
; CHECK: mul {{[abcd]l|byte ptr}}

define internal i32 @mul8BitConst(i32 %a, i32 %b) {
entry:
  %a_8 = trunc i32 %a to i8
  %mul = mul i8 %a_8, 56
  %ret = zext i8 %mul to i32
  ret i32 %ret
}
; CHECK-LABEL: mul8BitConst
; 8-bit imul only accepts r/m, not imm
; CHECK: mov {{.*}}, 56
; CHECK: mul {{[abcd]l|byte ptr}}

define internal i32 @udiv8Bit(i32 %a, i32 %b) {
entry:
  %a_8 = trunc i32 %a to i8
  %b_8 = trunc i32 %b to i8
  %udiv = udiv i8 %b_8, %a_8
  %ret = zext i8 %udiv to i32
  ret i32 %ret
}
; CHECK-LABEL: udiv8Bit
; CHECK: div {{[abcd]l|byte ptr}}

define internal i32 @udiv8BitConst(i32 %a, i32 %b) {
entry:
  %a_8 = trunc i32 %a to i8
  %udiv = udiv i8 %a_8, 123
  %ret = zext i8 %udiv to i32
  ret i32 %ret
}
; CHECK-LABEL: udiv8BitConst
; CHECK: div {{[abcd]l|byte ptr}}

define internal i32 @urem8Bit(i32 %a, i32 %b) {
entry:
  %a_8 = trunc i32 %a to i8
  %b_8 = trunc i32 %b to i8
  %urem = urem i8 %b_8, %a_8
  %ret = zext i8 %urem to i32
  ret i32 %ret
}
; CHECK-LABEL: urem8Bit
; CHECK: div {{[abcd]l|byte ptr}}

define internal i32 @urem8BitConst(i32 %a, i32 %b) {
entry:
  %a_8 = trunc i32 %a to i8
  %urem = urem i8 %a_8, 123
  %ret = zext i8 %urem to i32
  ret i32 %ret
}
; CHECK-LABEL: urem8BitConst
; CHECK: div {{[abcd]l|byte ptr}}


define internal i32 @sdiv8Bit(i32 %a, i32 %b) {
entry:
  %a_8 = trunc i32 %a to i8
  %b_8 = trunc i32 %b to i8
  %sdiv = sdiv i8 %b_8, %a_8
  %ret = zext i8 %sdiv to i32
  ret i32 %ret
}
; CHECK-LABEL: sdiv8Bit
; CHECK: idiv {{[abcd]l|byte ptr}}

define internal i32 @sdiv8BitConst(i32 %a, i32 %b) {
entry:
  %a_8 = trunc i32 %a to i8
  %sdiv = sdiv i8 %a_8, 123
  %ret = zext i8 %sdiv to i32
  ret i32 %ret
}
; CHECK-LABEL: sdiv8BitConst
; CHECK: idiv {{[abcd]l|byte ptr}}

define internal i32 @srem8Bit(i32 %a, i32 %b) {
entry:
  %a_8 = trunc i32 %a to i8
  %b_8 = trunc i32 %b to i8
  %srem = srem i8 %b_8, %a_8
  %ret = zext i8 %srem to i32
  ret i32 %ret
}
; CHECK-LABEL: srem8Bit
; CHECK: idiv {{[abcd]l|byte ptr}}

define internal i32 @srem8BitConst(i32 %a, i32 %b) {
entry:
  %a_8 = trunc i32 %a to i8
  %srem = srem i8 %a_8, 123
  %ret = zext i8 %srem to i32
  ret i32 %ret
}
; CHECK-LABEL: srem8BitConst
; CHECK: idiv {{[abcd]l|byte ptr}}

define internal i32 @shl8Bit(i32 %a, i32 %b) {
entry:
  %a_8 = trunc i32 %a to i8
  %b_8 = trunc i32 %b to i8
  %shl = shl i8 %b_8, %a_8
  %ret = zext i8 %shl to i32
  ret i32 %ret
}
; CHECK-LABEL: shl8Bit
; CHECK: shl {{[abd]l|byte ptr}}, cl

define internal i32 @shl8BitConst(i32 %a, i32 %b) {
entry:
  %a_8 = trunc i32 %a to i8
  %shl = shl i8 %a_8, 6
  %ret = zext i8 %shl to i32
  ret i32 %ret
}
; CHECK-LABEL: shl8BitConst
; CHECK: shl {{[abcd]l|byte ptr}}, 6

define internal i32 @lshr8Bit(i32 %a, i32 %b) {
entry:
  %a_8 = trunc i32 %a to i8
  %b_8 = trunc i32 %b to i8
  %lshr = lshr i8 %b_8, %a_8
  %ret = zext i8 %lshr to i32
  ret i32 %ret
}
; CHECK-LABEL: lshr8Bit
; CHECK: shr {{[abd]l|byte ptr}}, cl

define internal i32 @lshr8BitConst(i32 %a, i32 %b) {
entry:
  %a_8 = trunc i32 %a to i8
  %lshr = lshr i8 %a_8, 6
  %ret = zext i8 %lshr to i32
  ret i32 %ret
}
; CHECK-LABEL: lshr8BitConst
; CHECK: shr {{[abcd]l|byte ptr}}, 6

define internal i32 @ashr8Bit(i32 %a, i32 %b) {
entry:
  %a_8 = trunc i32 %a to i8
  %b_8 = trunc i32 %b to i8
  %ashr = ashr i8 %b_8, %a_8
  %ret = zext i8 %ashr to i32
  ret i32 %ret
}
; CHECK-LABEL: ashr8Bit
; CHECK: sar {{[abd]l|byte ptr}}, cl

define internal i32 @ashr8BitConst(i32 %a, i32 %b) {
entry:
  %a_8 = trunc i32 %a to i8
  %ashr = ashr i8 %a_8, 6
  %ret = zext i8 %ashr to i32
  ret i32 %ret
}
; CHECK-LABEL: ashr8BitConst
; CHECK: sar {{[abcd]l|byte ptr}}, 6


; ERRORS-NOT: ICE translation error
; DUMP-NOT: SZ
