; Tests various aspects of i1 related lowering.

; RUN: %p2i -i %s --filetype=obj --disassemble --args -O2 | FileCheck %s
; RUN: %p2i -i %s --filetype=obj --disassemble --args -Om1 | FileCheck %s

; Test that and with true uses immediate 1, not -1.
define internal i32 @testAndTrue(i32 %arg) {
entry:
  %arg_i1 = trunc i32 %arg to i1
  %result_i1 = and i1 %arg_i1, true
  %result = zext i1 %result_i1 to i32
  ret i32 %result
}
; CHECK-LABEL: testAndTrue
; CHECK: and {{.*}},0x1

; Test that or with true uses immediate 1, not -1.
define internal i32 @testOrTrue(i32 %arg) {
entry:
  %arg_i1 = trunc i32 %arg to i1
  %result_i1 = or i1 %arg_i1, true
  %result = zext i1 %result_i1 to i32
  ret i32 %result
}
; CHECK-LABEL: testOrTrue
; CHECK: or {{.*}},0x1

; Test that xor with true uses immediate 1, not -1.
define internal i32 @testXorTrue(i32 %arg) {
entry:
  %arg_i1 = trunc i32 %arg to i1
  %result_i1 = xor i1 %arg_i1, true
  %result = zext i1 %result_i1 to i32
  ret i32 %result
}
; CHECK-LABEL: testXorTrue
; CHECK: xor {{.*}},0x1

; Test that trunc to i1 masks correctly.
define internal i32 @testTrunc(i32 %arg) {
entry:
  %arg_i1 = trunc i32 %arg to i1
  %result = zext i1 %arg_i1 to i32
  ret i32 %result
}
; CHECK-LABEL: testTrunc
; CHECK: and {{.*}},0x1

; Test zext to i8.
define internal i32 @testZextI8(i32 %arg) {
entry:
  %arg_i1 = trunc i32 %arg to i1
  %result_i8 = zext i1 %arg_i1 to i8
  %result = zext i8 %result_i8 to i32
  ret i32 %result
}
; CHECK-LABEL: testZextI8
; match the trunc instruction
; CHECK: and {{.*}},0x1
; match the zext i1 instruction (NOTE: no mov need between i1 and i8).
; CHECK: and {{.*}},0x1

; Test zext to i16.
define internal i32 @testZextI16(i32 %arg) {
entry:
  %arg_i1 = trunc i32 %arg to i1
  %result_i16 = zext i1 %arg_i1 to i16
  %result = zext i16 %result_i16 to i32
  ret i32 %result
}
; CHECK-LABEL: testZextI16
; match the trunc instruction
; CHECK: and {{.*}},0x1
; match the zext i1 instruction (note 32-bit reg is used because it's shorter).
; CHECK: movzx [[REG:e.*]],{{[a-d]l|BYTE PTR}}
; CHECK: and [[REG]],0x1

; Test zext to i32.
define internal i32 @testZextI32(i32 %arg) {
entry:
  %arg_i1 = trunc i32 %arg to i1
  %result_i32 = zext i1 %arg_i1 to i32
  ret i32 %result_i32
}
; CHECK-LABEL: testZextI32
; match the trunc instruction
; CHECK: and {{.*}},0x1
; match the zext i1 instruction
; CHECK: movzx
; CHECK: and {{.*}},0x1

; Test zext to i64.
define internal i64 @testZextI64(i32 %arg) {
entry:
  %arg_i1 = trunc i32 %arg to i1
  %result_i64 = zext i1 %arg_i1 to i64
  ret i64 %result_i64
}
; CHECK-LABEL: testZextI64
; match the trunc instruction
; CHECK: and {{.*}},0x1
; match the zext i1 instruction
; CHECK: movzx
; CHECK: and {{.*}},0x1
; CHECK: mov {{.*}},0x0

; Test sext to i8.
define internal i32 @testSextI8(i32 %arg) {
entry:
  %arg_i1 = trunc i32 %arg to i1
  %result_i8 = sext i1 %arg_i1 to i8
  %result = sext i8 %result_i8 to i32
  ret i32 %result
}
; CHECK-LABEL: testSextI8
; match the trunc instruction
; CHECK: and {{.*}},0x1
; match the sext i1 instruction
; CHECK: shl [[REG:.*]],0x7
; CHECK-NEXT: sar [[REG]],0x7

; Test sext to i16.
define internal i32 @testSextI16(i32 %arg) {
entry:
  %arg_i1 = trunc i32 %arg to i1
  %result_i16 = sext i1 %arg_i1 to i16
  %result = sext i16 %result_i16 to i32
  ret i32 %result
}
; CHECK-LABEL: testSextI16
; match the trunc instruction
; CHECK: and {{.*}},0x1
; match the sext i1 instruction
; CHECK: movzx e[[REG:.*]],{{[a-d]l|BYTE PTR}}
; CHECK-NEXT: shl [[REG]],0xf
; CHECK-NEXT: sar [[REG]],0xf

; Test sext to i32.
define internal i32 @testSextI32(i32 %arg) {
entry:
  %arg_i1 = trunc i32 %arg to i1
  %result_i32 = sext i1 %arg_i1 to i32
  ret i32 %result_i32
}
; CHECK-LABEL: testSextI32
; match the trunc instruction
; CHECK: and {{.*}},0x1
; match the sext i1 instruction
; CHECK: movzx [[REG:.*]],
; CHECK-NEXT: shl [[REG]],0x1f
; CHECK-NEXT: sar [[REG]],0x1f

; Test sext to i64.
define internal i64 @testSextI64(i32 %arg) {
entry:
  %arg_i1 = trunc i32 %arg to i1
  %result_i64 = sext i1 %arg_i1 to i64
  ret i64 %result_i64
}
; CHECK-LABEL: testSextI64
; match the trunc instruction
; CHECK: and {{.*}},0x1
; match the sext i1 instruction
; CHECK: movzx [[REG:.*]],
; CHECK-NEXT: shl [[REG]],0x1f
; CHECK-NEXT: sar [[REG]],0x1f

; Test fptosi float to i1.
define internal i32 @testFptosiFloat(float %arg) {
entry:
  %arg_i1 = fptosi float %arg to i1
  %result = sext i1 %arg_i1 to i32
  ret i32 %result
}
; CHECK-LABEL: testFptosiFloat
; CHECK: cvttss2si
; CHECK: and {{.*}},0x1
; CHECK: movzx [[REG:.*]],
; CHECK-NEXT: shl [[REG]],0x1f
; CHECK-NEXT: sar [[REG]],0x1f

; Test fptosi double to i1.
define internal i32 @testFptosiDouble(double %arg) {
entry:
  %arg_i1 = fptosi double %arg to i1
  %result = sext i1 %arg_i1 to i32
  ret i32 %result
}
; CHECK-LABEL: testFptosiDouble
; CHECK: cvttsd2si
; CHECK: and {{.*}},0x1
; CHECK: movzx [[REG:.*]],
; CHECK-NEXT: shl [[REG]],0x1f
; CHECK-NEXT: sar [[REG]],0x1f
