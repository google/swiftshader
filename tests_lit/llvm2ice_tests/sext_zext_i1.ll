; Test sext and zext instructions with i1 source operands.

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

define internal i8 @sext1To8(i32 %a) {
entry:
  %a.arg_trunc = trunc i32 %a to i1
  %conv = sext i1 %a.arg_trunc to i8
  ret i8 %conv
}
; CHECK-LABEL: sext1To8
; CHECK: mov
; CHECK: shl {{.*}}, 7
; CHECK: sar {{.*}}, 7

define internal i16 @sext1To16(i32 %a) {
entry:
  %a.arg_trunc = trunc i32 %a to i1
  %conv = sext i1 %a.arg_trunc to i16
  ret i16 %conv
}
; CHECK-LABEL: sext1To16
; CHECK: mov
; CHECK: shl {{.*}}, 15
; CHECK: sar {{.*}}, 15

define internal i32 @sext1To32(i32 %a) {
entry:
  %a.arg_trunc = trunc i32 %a to i1
  %conv = sext i1 %a.arg_trunc to i32
  ret i32 %conv
}
; CHECK-LABEL: sext1To32
; CHECK: mov
; CHECK: shl {{.*}}, 31
; CHECK: sar {{.*}}, 31

define internal i64 @sext1To64(i32 %a) {
entry:
  %a.arg_trunc = trunc i32 %a to i1
  %conv = sext i1 %a.arg_trunc to i64
  ret i64 %conv
}
; CHECK-LABEL: sext1To64
; CHECK: mov
; CHECK: shl {{.*}}, 31
; CHECK: sar {{.*}}, 31
; CHECK: sar {{.*}}, 31

define internal i8 @zext1To8(i32 %a) {
entry:
  %a.arg_trunc = trunc i32 %a to i1
  %conv = zext i1 %a.arg_trunc to i8
  ret i8 %conv
}
; CHECK-LABEL: zext1To8
; CHECK: and {{.*}}, 1

define internal i16 @zext1To16(i32 %a) {
entry:
  %a.arg_trunc = trunc i32 %a to i1
  %conv = zext i1 %a.arg_trunc to i16
  ret i16 %conv
}
; CHECK-LABEL: zext1To16
; CHECK: and {{.*}}, 1

define internal i32 @zext1To32(i32 %a) {
entry:
  %a.arg_trunc = trunc i32 %a to i1
  %conv = zext i1 %a.arg_trunc to i32
  ret i32 %conv
}
; CHECK-LABEL: zext1To32
; CHECK: and {{.*}}, 1

define internal i64 @zext1To64(i32 %a) {
entry:
  %a.arg_trunc = trunc i32 %a to i1
  %conv = zext i1 %a.arg_trunc to i64
  ret i64 %conv
}
; CHECK-LABEL: zext1To64
; CHECK: and {{.*}}, 1

; ERRORS-NOT: ICE translation error
; DUMP-NOT: SZ
