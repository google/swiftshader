; Tests various aspects of i1 related lowering.

; RUN: %llvm2ice -O2 --verbose none %s \
; RUN:   | llvm-mc -triple=i686-none-nacl -x86-asm-syntax=intel -filetype=obj \
; RUN:   | llvm-objdump -d --symbolize -x86-asm-syntax=intel - | FileCheck %s
; RUN: %llvm2ice -Om1 --verbose none %s \
; RUN:   | llvm-mc -triple=i686-none-nacl -x86-asm-syntax=intel -filetype=obj \
; RUN:   | llvm-objdump -d --symbolize -x86-asm-syntax=intel - | FileCheck %s
; RUN: %llvm2ice --verbose none %s | FileCheck --check-prefix=ERRORS %s
; RUN: %llvm2iceinsts %s | %szdiff %s | FileCheck --check-prefix=DUMP %s
; RUN: %llvm2iceinsts --pnacl %s | %szdiff %s \
; RUN:                           | FileCheck --check-prefix=DUMP %s

; Test that xor with true uses immediate 1, not -1.
define internal i32 @testXorTrue(i32 %arg) {
entry:
  %arg_i1 = trunc i32 %arg to i1
  %result_i1 = xor i1 %arg_i1, true
  %result = zext i1 %result_i1 to i32
  ret i32 %result
}
; CHECK-LABEL: testXorTrue
; CHECK: xor {{.*}}, 1

; Test that trunc to i1 masks correctly.
define internal i32 @testTrunc(i32 %arg) {
entry:
  %arg_i1 = trunc i32 %arg to i1
  %result = zext i1 %arg_i1 to i32
  ret i32 %result
}
; CHECK-LABEL: testTrunc
; CHECK: and {{.*}}, 1

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
; CHECK: and {{.*}}, 1
; match the zext i1 instruction
; CHECK: movzx
; CHECK: and {{.*}}, 1

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
; CHECK: and {{.*}}, 1
; match the zext i1 instruction
; CHECK: movzx
; CHECK: and {{.*}}, 1

; Test zext to i32.
define internal i32 @testZextI32(i32 %arg) {
entry:
  %arg_i1 = trunc i32 %arg to i1
  %result_i32 = zext i1 %arg_i1 to i32
  ret i32 %result_i32
}
; CHECK-LABEL: testZextI32
; match the trunc instruction
; CHECK: and {{.*}}, 1
; match the zext i1 instruction
; CHECK: movzx
; CHECK: and {{.*}}, 1

; Test zext to i64.
define internal i64 @testZextI64(i32 %arg) {
entry:
  %arg_i1 = trunc i32 %arg to i1
  %result_i64 = zext i1 %arg_i1 to i64
  ret i64 %result_i64
}
; CHECK-LABEL: testZextI64
; match the trunc instruction
; CHECK: and {{.*}}, 1
; match the zext i1 instruction
; CHECK: movzx
; CHECK: and {{.*}}, 1
; CHECK: mov {{.*}}, 0

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
; CHECK: and {{.*}}, 1
; match the sext i1 instruction
; CHECK: shl [[REG:.*]], 7
; CHECK-NEXT: sar [[REG]], 7

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
; CHECK: and {{.*}}, 1
; match the sext i1 instruction
; CHECK: movzx [[REG:.*]],
; CHECK-NEXT: shl [[REG]], 15
; CHECK-NEXT: sar [[REG]], 15

; Test sext to i32.
define internal i32 @testSextI32(i32 %arg) {
entry:
  %arg_i1 = trunc i32 %arg to i1
  %result_i32 = sext i1 %arg_i1 to i32
  ret i32 %result_i32
}
; CHECK-LABEL: testSextI32
; match the trunc instruction
; CHECK: and {{.*}}, 1
; match the sext i1 instruction
; CHECK: movzx [[REG:.*]],
; CHECK-NEXT: shl [[REG]], 31
; CHECK-NEXT: sar [[REG]], 31

; Test sext to i64.
define internal i64 @testSextI64(i32 %arg) {
entry:
  %arg_i1 = trunc i32 %arg to i1
  %result_i64 = sext i1 %arg_i1 to i64
  ret i64 %result_i64
}
; CHECK-LABEL: testSextI64
; match the trunc instruction
; CHECK: and {{.*}}, 1
; match the sext i1 instruction
; CHECK: movzx [[REG:.*]],
; CHECK-NEXT: shl [[REG]], 31
; CHECK-NEXT: sar [[REG]], 31

; Test fptosi float to i1.
define internal i32 @testFptosiFloat(float %arg) {
entry:
  %arg_i1 = fptosi float %arg to i1
  %result = sext i1 %arg_i1 to i32
  ret i32 %result
}
; CHECK-LABEL: testFptosiFloat
; CHECK: cvttss2si
; CHECK: and {{.*}}, 1
; CHECK: movzx [[REG:.*]],
; CHECK-NEXT: shl [[REG]], 31
; CHECK-NEXT: sar [[REG]], 31

; Test fptosi double to i1.
define internal i32 @testFptosiDouble(double %arg) {
entry:
  %arg_i1 = fptosi double %arg to i1
  %result = sext i1 %arg_i1 to i32
  ret i32 %result
}
; CHECK-LABEL: testFptosiDouble
; CHECK: cvttsd2si
; CHECK: and {{.*}}, 1
; CHECK: movzx [[REG:.*]],
; CHECK-NEXT: shl [[REG]], 31
; CHECK-NEXT: sar [[REG]], 31

; ERRORS-NOT: ICE translation error
; DUMP-NOT: SZ
