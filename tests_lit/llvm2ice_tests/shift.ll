; This is a test of C-level conversion operations that clang lowers
; into pairs of shifts.

; RUN: %llvm2ice -O2 --verbose none %s | FileCheck %s
; RUN: %llvm2ice -Om1 --verbose none %s | FileCheck %s
; RUN: %llvm2ice -O2 --verbose none %s \
; RUN:     | llvm-mc -triple=i686-none-nacl -x86-asm-syntax=intel -filetype=obj
; RUN: %llvm2ice -Om1 --verbose none %s \
; RUN:     | llvm-mc -triple=i686-none-nacl -x86-asm-syntax=intel -filetype=obj
; RUN: %llvm2ice --verbose none %s | FileCheck --check-prefix=ERRORS %s
; RUN: %llvm2iceinsts %s | %szdiff %s | FileCheck --check-prefix=DUMP %s
; RUN: %llvm2iceinsts --pnacl %s | %szdiff %s \
; RUN:                           | FileCheck --check-prefix=DUMP %s

@i1 = internal global [4 x i8] zeroinitializer, align 4
@i2 = internal global [4 x i8] zeroinitializer, align 4
@u1 = internal global [4 x i8] zeroinitializer, align 4

define void @conv1() {
entry:
  %__0 = bitcast [4 x i8]* @u1 to i32*
  %v0 = load i32* %__0, align 1
  %sext = shl i32 %v0, 24
  %v1 = ashr i32 %sext, 24
  %__4 = bitcast [4 x i8]* @i1 to i32*
  store i32 %v1, i32* %__4, align 1
  ret void
}
; CHECK: conv1:
; CHECK: shl {{.*}}, 24
; CHECK: sar {{.*}}, 24

define void @conv2() {
entry:
  %__0 = bitcast [4 x i8]* @u1 to i32*
  %v0 = load i32* %__0, align 1
  %sext1 = shl i32 %v0, 16
  %v1 = ashr i32 %sext1, 16
  %__4 = bitcast [4 x i8]* @i2 to i32*
  store i32 %v1, i32* %__4, align 1
  ret void
}
; CHECK: conv2:
; CHECK: shl {{.*}}, 16
; CHECK: sar {{.*}}, 16

; ERRORS-NOT: ICE translation error
; DUMP-NOT: SZ
