; Simple test of signed and unsigned integer conversions.

; TODO(jvoung): llvm-objdump doesn't symbolize global symbols well, so we
; have [0] == i8v, [2] == i16v, [4] == i32v, [8] == i64v, etc.

; RUN: %p2i -i %s --args -O2 --verbose none \
; RUN:   | llvm-mc -triple=i686-none-nacl -filetype=obj \
; RUN:   | llvm-objdump -d --symbolize -x86-asm-syntax=intel - | FileCheck %s
; RUN: %p2i -i %s --args -Om1 --verbose none \
; RUN:   | llvm-mc -triple=i686-none-nacl -filetype=obj \
; RUN:   | llvm-objdump -d --symbolize -x86-asm-syntax=intel - | FileCheck %s

@i8v = internal global [1 x i8] zeroinitializer, align 1
@i16v = internal global [2 x i8] zeroinitializer, align 2
@i32v = internal global [4 x i8] zeroinitializer, align 4
@i64v = internal global [8 x i8] zeroinitializer, align 8
@u8v = internal global [1 x i8] zeroinitializer, align 1
@u16v = internal global [2 x i8] zeroinitializer, align 2
@u32v = internal global [4 x i8] zeroinitializer, align 4
@u64v = internal global [8 x i8] zeroinitializer, align 8

define void @from_int8() {
entry:
  %__0 = bitcast [1 x i8]* @i8v to i8*
  %v0 = load i8* %__0, align 1
  %v1 = sext i8 %v0 to i16
  %__3 = bitcast [2 x i8]* @i16v to i16*
  store i16 %v1, i16* %__3, align 1
  %v2 = sext i8 %v0 to i32
  %__5 = bitcast [4 x i8]* @i32v to i32*
  store i32 %v2, i32* %__5, align 1
  %v3 = sext i8 %v0 to i64
  %__7 = bitcast [8 x i8]* @i64v to i64*
  store i64 %v3, i64* %__7, align 1
  ret void
}
; CHECK-LABEL: from_int8
; CHECK: mov {{.*}}, byte ptr [
; CHECK: movsx e{{.*}}, {{[a-d]l|byte ptr}}
; CHECK: mov word ptr [
; CHECK: movsx
; CHECK: mov dword ptr [
; CHECK: movsx
; CHECK: sar {{.*}}, 31
; This appears to be a bug in llvm-mc. It should be i64v and i64+4.
; CHECK-DAG: [.bss]
; CHECK-DAG: [.bss]

define void @from_int16() {
entry:
  %__0 = bitcast [2 x i8]* @i16v to i16*
  %v0 = load i16* %__0, align 1
  %v1 = trunc i16 %v0 to i8
  %__3 = bitcast [1 x i8]* @i8v to i8*
  store i8 %v1, i8* %__3, align 1
  %v2 = sext i16 %v0 to i32
  %__5 = bitcast [4 x i8]* @i32v to i32*
  store i32 %v2, i32* %__5, align 1
  %v3 = sext i16 %v0 to i64
  %__7 = bitcast [8 x i8]* @i64v to i64*
  store i64 %v3, i64* %__7, align 1
  ret void
}
; CHECK-LABEL: from_int16
; CHECK: mov {{.*}}, word ptr [
; CHECK: [.bss]
; CHECK: movsx e{{.*}}, {{.*x|[ds]i|bp|word ptr}}
; CHECK: [.bss]
; CHECK: movsx e{{.*}}, {{.*x|[ds]i|bp|word ptr}}
; CHECK: sar {{.*}}, 31
; CHECK: [.bss]

define void @from_int32() {
entry:
  %__0 = bitcast [4 x i8]* @i32v to i32*
  %v0 = load i32* %__0, align 1
  %v1 = trunc i32 %v0 to i8
  %__3 = bitcast [1 x i8]* @i8v to i8*
  store i8 %v1, i8* %__3, align 1
  %v2 = trunc i32 %v0 to i16
  %__5 = bitcast [2 x i8]* @i16v to i16*
  store i16 %v2, i16* %__5, align 1
  %v3 = sext i32 %v0 to i64
  %__7 = bitcast [8 x i8]* @i64v to i64*
  store i64 %v3, i64* %__7, align 1
  ret void
}
; CHECK-LABEL: from_int32
; CHECK: [.bss]
; CHECK: [.bss]
; CHECK: [.bss]
; CHECK: sar {{.*}}, 31
; CHECK: [.bss]

define void @from_int64() {
entry:
  %__0 = bitcast [8 x i8]* @i64v to i64*
  %v0 = load i64* %__0, align 1
  %v1 = trunc i64 %v0 to i8
  %__3 = bitcast [1 x i8]* @i8v to i8*
  store i8 %v1, i8* %__3, align 1
  %v2 = trunc i64 %v0 to i16
  %__5 = bitcast [2 x i8]* @i16v to i16*
  store i16 %v2, i16* %__5, align 1
  %v3 = trunc i64 %v0 to i32
  %__7 = bitcast [4 x i8]* @i32v to i32*
  store i32 %v3, i32* %__7, align 1
  ret void
}
; CHECK-LABEL: from_int64
; CHECK: [.bss]
; CHECK: [.bss]
; CHECK: [.bss]
; CHECK: [.bss]


define void @from_uint8() {
entry:
  %__0 = bitcast [1 x i8]* @u8v to i8*
  %v0 = load i8* %__0, align 1
  %v1 = zext i8 %v0 to i16
  %__3 = bitcast [2 x i8]* @i16v to i16*
  store i16 %v1, i16* %__3, align 1
  %v2 = zext i8 %v0 to i32
  %__5 = bitcast [4 x i8]* @i32v to i32*
  store i32 %v2, i32* %__5, align 1
  %v3 = zext i8 %v0 to i64
  %__7 = bitcast [8 x i8]* @i64v to i64*
  store i64 %v3, i64* %__7, align 1
  ret void
}
; CHECK-LABEL: from_uint8
; CHECK: [.bss]
; CHECK: movzx e{{.*}}, {{[a-d]l|byte ptr}}
; CHECK: [.bss]
; CHECK: movzx
; CHECK: [.bss]
; CHECK: movzx
; CHECK: mov {{.*}}, 0
; CHECK: [.bss]

define void @from_uint16() {
entry:
  %__0 = bitcast [2 x i8]* @u16v to i16*
  %v0 = load i16* %__0, align 1
  %v1 = trunc i16 %v0 to i8
  %__3 = bitcast [1 x i8]* @i8v to i8*
  store i8 %v1, i8* %__3, align 1
  %v2 = zext i16 %v0 to i32
  %__5 = bitcast [4 x i8]* @i32v to i32*
  store i32 %v2, i32* %__5, align 1
  %v3 = zext i16 %v0 to i64
  %__7 = bitcast [8 x i8]* @i64v to i64*
  store i64 %v3, i64* %__7, align 1
  ret void
}
; CHECK-LABEL: from_uint16
; CHECK: [.bss]
; CHECK: [.bss]
; CHECK: movzx e{{.*}}, {{.*x|[ds]i|bp|word ptr}}
; CHECK: [.bss]
; CHECK: movzx e{{.*}}, {{.*x|[ds]i|bp|word ptr}}
; CHECK: mov {{.*}}, 0
; CHECK: [.bss]

define void @from_uint32() {
entry:
  %__0 = bitcast [4 x i8]* @u32v to i32*
  %v0 = load i32* %__0, align 1
  %v1 = trunc i32 %v0 to i8
  %__3 = bitcast [1 x i8]* @i8v to i8*
  store i8 %v1, i8* %__3, align 1
  %v2 = trunc i32 %v0 to i16
  %__5 = bitcast [2 x i8]* @i16v to i16*
  store i16 %v2, i16* %__5, align 1
  %v3 = zext i32 %v0 to i64
  %__7 = bitcast [8 x i8]* @i64v to i64*
  store i64 %v3, i64* %__7, align 1
  ret void
}
; CHECK-LABEL: from_uint32
; CHECK: [.bss]
; CHECK: [.bss]
; CHECK: [.bss]
; CHECK: mov {{.*}}, 0
; CHECK: [.bss]

define void @from_uint64() {
entry:
  %__0 = bitcast [8 x i8]* @u64v to i64*
  %v0 = load i64* %__0, align 1
  %v1 = trunc i64 %v0 to i8
  %__3 = bitcast [1 x i8]* @i8v to i8*
  store i8 %v1, i8* %__3, align 1
  %v2 = trunc i64 %v0 to i16
  %__5 = bitcast [2 x i8]* @i16v to i16*
  store i16 %v2, i16* %__5, align 1
  %v3 = trunc i64 %v0 to i32
  %__7 = bitcast [4 x i8]* @i32v to i32*
  store i32 %v3, i32* %__7, align 1
  ret void
}
; CHECK-LABEL: from_uint64
; CHECK: [.bss]
; CHECK: [.bss]
; CHECK: [.bss]
; CHECK: [.bss]
