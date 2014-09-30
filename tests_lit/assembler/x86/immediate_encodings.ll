; Tests various aspects of x86 immediate encoding. Some encodings are shorter.
; For example, the encoding is shorter for 8-bit immediates or when using EAX.
; This assumes that EAX is chosen as the first free register in O2 mode.

; RUN: %p2i -i %s --args -O2 --verbose none \
; RUN:   | llvm-mc -triple=i686-none-nacl -x86-asm-syntax=intel -filetype=obj \
; RUN:   | llvm-objdump -d --symbolize -x86-asm-syntax=intel - | FileCheck %s
; RUN: %p2i -i %s --args --verbose none | FileCheck --check-prefix=ERRORS %s
; RUN: %p2i -i %s --insts | %szdiff %s | FileCheck --check-prefix=DUMP %s

define internal i32 @testXor8Imm8(i32 %arg) {
entry:
  %arg_i8 = trunc i32 %arg to i8
  %result_i8 = xor i8 %arg_i8, 127
  %result = zext i8 %result_i8 to i32
  ret i32 %result
}
; CHECK-LABEL: testXor8Imm8
; CHECK: 34 7f   xor al, 127

define internal i32 @testXor8Imm8Neg(i32 %arg) {
entry:
  %arg_i8 = trunc i32 %arg to i8
  %result_i8 = xor i8 %arg_i8, -128
  %result = zext i8 %result_i8 to i32
  ret i32 %result
}
; CHECK-LABEL: testXor8Imm8Neg
; CHECK: 34 80   xor al, -128

define internal i32 @testXor8Imm8NotEAX(i32 %arg, i32 %arg2, i32 %arg3) {
entry:
  %arg_i8 = trunc i32 %arg to i8
  %arg2_i8 = trunc i32 %arg2 to i8
  %arg3_i8 = trunc i32 %arg3 to i8
  %x1 = xor i8 %arg_i8, 127
  %x2 = xor i8 %arg2_i8, 127
  %x3 = xor i8 %arg3_i8, 127
  %x4 = add i8 %x1, %x2
  %x5 = add i8 %x4, %x3
  %result = zext i8 %x5 to i32
  ret i32 %result
}
; CHECK-LABEL: testXor8Imm8NotEAX
; CHECK: 80 f{{[1-3]}} 7f xor {{[^a]}}l, 127

define internal i32 @testXor16Imm8(i32 %arg) {
entry:
  %arg_i16 = trunc i32 %arg to i16
  %result_i16 = xor i16 %arg_i16, 127
  %result = zext i16 %result_i16 to i32
  ret i32 %result
}
; CHECK-LABEL: testXor16Imm8
; CHECK: 66 83 f0 7f  xor ax, 127

define internal i32 @testXor16Imm8Neg(i32 %arg) {
entry:
  %arg_i16 = trunc i32 %arg to i16
  %result_i16 = xor i16 %arg_i16, -128
  %result = zext i16 %result_i16 to i32
  ret i32 %result
}
; CHECK-LABEL: testXor16Imm8Neg
; CHECK: 66 83 f0 80  xor ax, 128

define internal i32 @testXor16Imm16Eax(i32 %arg) {
entry:
  %arg_i16 = trunc i32 %arg to i16
  %tmp = xor i16 %arg_i16, 1024
  %result_i16 = add i16 %tmp, 1
  %result = zext i16 %result_i16 to i32
  ret i32 %result
}
; CHECK-LABEL: testXor16Imm16Eax
; CHECK: 66 35 00 04  xor ax, 1024
; CHECK-NEXT: add ax, 1

define internal i32 @testXor16Imm16NegEax(i32 %arg) {
entry:
  %arg_i16 = trunc i32 %arg to i16
  %tmp = xor i16 %arg_i16, -256
  %result_i16 = add i16 %tmp, 1
  %result = zext i16 %result_i16 to i32
  ret i32 %result
}
; CHECK-LABEL: testXor16Imm16NegEax
; CHECK: 66 35 00 ff  xor ax, 65280
; CHECK-NEXT: add ax, 1

define internal i32 @testXor16Imm16NotEAX(i32 %arg_i32, i32 %arg2_i32, i32 %arg3_i32) {
entry:
  %arg = trunc i32 %arg_i32 to i16
  %arg2 = trunc i32 %arg2_i32 to i16
  %arg3 = trunc i32 %arg3_i32 to i16
  %x = xor i16 %arg, 32767
  %x2 = xor i16 %arg2, 32767
  %x3 = xor i16 %arg3, 32767
  %add1 = add i16 %x, %x2
  %add2 = add i16 %add1, %x3
  %result = zext i16 %add2 to i32
  ret i32 %result
}
; CHECK-LABEL: testXor16Imm16NotEAX
; CHECK: 66 81 f{{[1-3]}} ff 7f  xor {{[^a]}}x, 32767
; CHECK-NEXT: 66 81 f{{[1-3]}} ff 7f  xor {{[^a]}}x, 32767

define internal i32 @testXor32Imm8(i32 %arg) {
entry:
  %result = xor i32 %arg, 127
  ret i32 %result
}
; CHECK-LABEL: testXor32Imm8
; CHECK: 83 f0 7f   xor eax, 127

define internal i32 @testXor32Imm8Neg(i32 %arg) {
entry:
  %result = xor i32 %arg, -128
  ret i32 %result
}
; CHECK-LABEL: testXor32Imm8Neg
; CHECK: 83 f0 80   xor eax, -128

define internal i32 @testXor32Imm32Eax(i32 %arg) {
entry:
  %result = xor i32 %arg, 16777216
  ret i32 %result
}
; CHECK-LABEL: testXor32Imm32Eax
; CHECK: 35 00 00 00 01   xor eax, 16777216

define internal i32 @testXor32Imm32NegEax(i32 %arg) {
entry:
  %result = xor i32 %arg, -256
  ret i32 %result
}
; CHECK-LABEL: testXor32Imm32NegEax
; CHECK: 35 00 ff ff ff   xor eax, 4294967040

define internal i32 @testXor32Imm32NotEAX(i32 %arg, i32 %arg2, i32 %arg3) {
entry:
  %x = xor i32 %arg, 32767
  %x2 = xor i32 %arg2, 32767
  %x3 = xor i32 %arg3, 32767
  %add1 = add i32 %x, %x2
  %add2 = add i32 %add1, %x3
  ret i32 %add2
}
; CHECK-LABEL: testXor32Imm32NotEAX
; CHECK: 81 f{{[1-3]}} ff 7f 00 00   xor e{{[^a]}}x, 32767

; Should be similar for add, sub, etc., so sample a few.

define internal i32 @testAdd8Imm8(i32 %arg) {
entry:
  %arg_i8 = trunc i32 %arg to i8
  %result_i8 = add i8 %arg_i8, 126
  %result = zext i8 %result_i8 to i32
  ret i32 %result
}
; CHECK-LABEL: testAdd8Imm8
; CHECK: 04 7e   add al, 126

define internal i32 @testSub8Imm8(i32 %arg) {
entry:
  %arg_i8 = trunc i32 %arg to i8
  %result_i8 = sub i8 %arg_i8, 125
  %result = zext i8 %result_i8 to i32
  ret i32 %result
}
; CHECK-LABEL: testSub8Imm8
; CHECK: 2c 7d  sub al, 125

; imul has some shorter 8-bit immediate encodings.
; It also has a shorter encoding for eax, but we don't do that yet.

define internal i32 @testMul16Imm8(i32 %arg) {
entry:
  %arg_i16 = trunc i32 %arg to i16
  %tmp = mul i16 %arg_i16, 99
  %result_i16 = add i16 %tmp, 1
  %result = zext i16 %result_i16 to i32
  ret i32 %result
}
; CHECK-LABEL: testMul16Imm8
; CHECK: 66 6b c0 63  imul ax, ax, 99
; CHECK-NEXT: add ax, 1

define internal i32 @testMul16Imm8Neg(i32 %arg) {
entry:
  %arg_i16 = trunc i32 %arg to i16
  %tmp = mul i16 %arg_i16, -111
  %result_i16 = add i16 %tmp, 1
  %result = zext i16 %result_i16 to i32
  ret i32 %result
}
; CHECK-LABEL: testMul16Imm8Neg
; CHECK: 66 6b c0 91  imul ax, ax, 145
; CHECK-NEXT: add ax, 1

define internal i32 @testMul16Imm16(i32 %arg) {
entry:
  %arg_i16 = trunc i32 %arg to i16
  %tmp = mul i16 %arg_i16, 1024
  %result_i16 = add i16 %tmp, 1
  %result = zext i16 %result_i16 to i32
  ret i32 %result
}
; CHECK-LABEL: testMul16Imm16
; CHECK: 66 69 c0 00 04  imul ax, ax, 1024
; CHECK-NEXT: add ax, 1

define internal i32 @testMul16Imm16Neg(i32 %arg) {
entry:
  %arg_i16 = trunc i32 %arg to i16
  %tmp = mul i16 %arg_i16, -256
  %result_i16 = add i16 %tmp, 1
  %result = zext i16 %result_i16 to i32
  ret i32 %result
}
; CHECK-LABEL: testMul16Imm16Neg
; CHECK: 66 69 c0 00 ff  imul ax, ax, 65280
; CHECK-NEXT: add ax, 1

define internal i32 @testMul32Imm8(i32 %arg) {
entry:
  %result = mul i32 %arg, 99
  ret i32 %result
}
; CHECK-LABEL: testMul32Imm8
; CHECK: 6b c0 63  imul eax, eax, 99

define internal i32 @testMul32Imm8Neg(i32 %arg) {
entry:
  %result = mul i32 %arg, -111
  ret i32 %result
}
; CHECK-LABEL: testMul32Imm8Neg
; CHECK: 6b c0 91  imul eax, eax, -111

define internal i32 @testMul32Imm16(i32 %arg) {
entry:
  %result = mul i32 %arg, 1024
  ret i32 %result
}
; CHECK-LABEL: testMul32Imm16
; CHECK: 69 c0 00 04 00 00  imul eax, eax, 1024

define internal i32 @testMul32Imm16Neg(i32 %arg) {
entry:
  %result = mul i32 %arg, -256
  ret i32 %result
}
; CHECK-LABEL: testMul32Imm16Neg
; CHECK: 69 c0 00 ff ff ff  imul eax, eax, 4294967040

; ERRORS-NOT: ICE translation error
; DUMP-NOT: SZ
