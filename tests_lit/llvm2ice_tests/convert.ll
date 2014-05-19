; RUIN: %llvm2ice %s | FileCheck %s
; RUIN: %llvm2ice --verbose none %s | FileCheck --check-prefix=ERRORS %s
; RUN: %llvm2iceinsts %s | %szdiff %s | FileCheck --check-prefix=DUMP %s
; RUN: %llvm2iceinsts --pnacl %s | %szdiff %s \
; RUN:                           | FileCheck --check-prefix=DUMP %s

@i8v = global [1 x i8] zeroinitializer, align 1
@i16v = global [2 x i8] zeroinitializer, align 2
@i32v = global [4 x i8] zeroinitializer, align 4
@i64v = global [8 x i8] zeroinitializer, align 8
@u8v = global [1 x i8] zeroinitializer, align 1
@u16v = global [2 x i8] zeroinitializer, align 2
@u32v = global [4 x i8] zeroinitializer, align 4
@u64v = global [8 x i8] zeroinitializer, align 8

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
  ; CHECK: mov al, byte ptr [
  ; CHECK-NEXT: movsx cx, al
  ; CHECK-NEXT: mov word ptr [
  ; CHECK-NEXT: movsx ecx, al
  ; CHECK-NEXT: mov dword ptr [
  ; CHECK-NEXT: movsx ecx, al
  ; CHECK-NEXT: sar eax, 31
  ; CHECK-NEXT: mov dword ptr [i64v+4],
  ; CHECK-NEXT: mov dword ptr [i64v],
}

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
  ; CHECK: mov ax, word ptr [
  ; CHECK-NEXT: mov cx, ax
  ; CHECK-NEXT: mov byte ptr [
  ; CHECK-NEXT: movsx ecx, ax
  ; CHECK-NEXT: mov dword ptr [
  ; CHECK-NEXT: movsx ecx, ax
  ; CHECK-NEXT: sar eax, 31
  ; CHECK-NEXT: mov dword ptr [i64v+4],
  ; CHECK-NEXT: mov dword ptr [i64v],
}

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
  ; CHECK: mov eax, dword ptr [
  ; CHECK-NEXT: mov ecx, eax
  ; CHECK-NEXT: mov byte ptr [
  ; CHECK-NEXT: mov ecx, eax
  ; CHECK-NEXT: mov word ptr [
  ; CHECK-NEXT: mov ecx, eax
  ; CHECK-NEXT: sar eax, 31
  ; CHECK-NEXT: mov dword ptr [i64v+4],
  ; CHECK-NEXT: mov dword ptr [i64v],
}

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
  ; CHECK: mov eax, dword ptr [
  ; CHECK-NEXT: mov ecx, eax
  ; CHECK-NEXT: mov byte ptr [
  ; CHECK-NEXT: mov ecx, eax
  ; CHECK-NEXT: mov word ptr [
  ; CHECK-NEXT: mov dword ptr [
}

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
  ; CHECK: mov al, byte ptr [
  ; CHECK-NEXT: movzx cx, al
  ; CHECK-NEXT: mov word ptr [
  ; CHECK-NEXT: movzx ecx, al
  ; CHECK-NEXT: mov dword ptr [
  ; CHECK-NEXT: movzx eax, al
  ; CHECK-NEXT: mov ecx, 0
  ; CHECK-NEXT: mov dword ptr [i64v+4],
  ; CHECK-NEXT: mov dword ptr [i64v],
}

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
  ; CHECK: mov ax, word ptr [
  ; CHECK-NEXT: mov cx, ax
  ; CHECK-NEXT: mov byte ptr [
  ; CHECK-NEXT: movzx ecx, ax
  ; CHECK-NEXT: mov dword ptr [
  ; CHECK-NEXT: movzx eax, ax
  ; CHECK-NEXT: mov ecx, 0
  ; CHECK-NEXT: mov dword ptr [i64v+4],
  ; CHECK-NEXT: mov dword ptr [i64v],
}

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
  ; CHECK: mov eax, dword ptr [
  ; CHECK-NEXT: mov ecx, eax
  ; CHECK-NEXT: mov byte ptr [
  ; CHECK-NEXT: mov ecx, eax
  ; CHECK-NEXT: mov word ptr [
  ; CHECK-NEXT: mov ecx, 0
  ; CHECK-NEXT: mov dword ptr [i64v+4],
  ; CHECK-NEXT: mov dword ptr [i64v],
}

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
  ; CHECK: mov eax, dword ptr [
  ; CHECK-NEXT: mov ecx, eax
  ; CHECK-NEXT: mov byte ptr [
  ; CHECK-NEXT: mov ecx, eax
  ; CHECK-NEXT: mov word ptr [
  ; CHECK-NEXT: mov dword ptr [
}

; ERRORS-NOT: ICE translation error
; DUMP-NOT: SZ
