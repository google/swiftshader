; This is a test of C-level conversion operations that clang lowers
; into pairs of shifts.

; RUN: %p2i -i %s --filetype=obj --disassemble --no-local-syms --args -O2 \
; RUN:   | FileCheck %s
; RUN: %p2i -i %s --filetype=obj --disassemble --no-local-syms --args -Om1 \
; RUN:   | FileCheck %s

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
; CHECK-LABEL: conv1
; CHECK: shl {{.*}},0x18
; CHECK: sar {{.*}},0x18

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
; CHECK-LABEL: conv2
; CHECK: shl {{.*}},0x10
; CHECK: sar {{.*}},0x10
