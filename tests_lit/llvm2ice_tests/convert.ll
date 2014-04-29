; RUIN: %llvm2ice %s | FileCheck %s
; RUIN: %llvm2ice --verbose none %s | FileCheck --check-prefix=ERRORS %s
; RUN: %szdiff --llvm2ice=%llvm2ice %s | FileCheck --check-prefix=DUMP %s

@i8v = common global i8 0, align 1
@i16v = common global i16 0, align 2
@i32v = common global i32 0, align 4
@i64v = common global i64 0, align 8
@u8v = common global i8 0, align 1
@u16v = common global i16 0, align 2
@u32v = common global i32 0, align 4
@u64v = common global i64 0, align 8
@i1 = common global i32 0, align 4
@i2 = common global i32 0, align 4
@u1 = common global i32 0, align 4
@u2 = common global i32 0, align 4

define void @from_int8() {
entry:
  %v0 = load i8* @i8v, align 1
  %v1 = sext i8 %v0 to i16
  store i16 %v1, i16* @i16v, align 1
  %v2 = sext i8 %v0 to i32
  store i32 %v2, i32* @i32v, align 1
  %v3 = sext i8 %v0 to i64
  store i64 %v3, i64* @i64v, align 1
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
  %v0 = load i16* @i16v, align 1
  %v1 = trunc i16 %v0 to i8
  store i8 %v1, i8* @i8v, align 1
  %v2 = sext i16 %v0 to i32
  store i32 %v2, i32* @i32v, align 1
  %v3 = sext i16 %v0 to i64
  store i64 %v3, i64* @i64v, align 1
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
  %v0 = load i32* @i32v, align 1
  %v1 = trunc i32 %v0 to i8
  store i8 %v1, i8* @i8v, align 1
  %v2 = trunc i32 %v0 to i16
  store i16 %v2, i16* @i16v, align 1
  %v3 = sext i32 %v0 to i64
  store i64 %v3, i64* @i64v, align 1
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
  %v0 = load i64* @i64v, align 1
  %v1 = trunc i64 %v0 to i8
  store i8 %v1, i8* @i8v, align 1
  %v2 = trunc i64 %v0 to i16
  store i16 %v2, i16* @i16v, align 1
  %v3 = trunc i64 %v0 to i32
  store i32 %v3, i32* @i32v, align 1
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
  %v0 = load i8* @u8v, align 1
  %v1 = zext i8 %v0 to i16
  store i16 %v1, i16* @i16v, align 1
  %v2 = zext i8 %v0 to i32
  store i32 %v2, i32* @i32v, align 1
  %v3 = zext i8 %v0 to i64
  store i64 %v3, i64* @i64v, align 1
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
  %v0 = load i16* @u16v, align 1
  %v1 = trunc i16 %v0 to i8
  store i8 %v1, i8* @i8v, align 1
  %v2 = zext i16 %v0 to i32
  store i32 %v2, i32* @i32v, align 1
  %v3 = zext i16 %v0 to i64
  store i64 %v3, i64* @i64v, align 1
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
  %v0 = load i32* @u32v, align 1
  %v1 = trunc i32 %v0 to i8
  store i8 %v1, i8* @i8v, align 1
  %v2 = trunc i32 %v0 to i16
  store i16 %v2, i16* @i16v, align 1
  %v3 = zext i32 %v0 to i64
  store i64 %v3, i64* @i64v, align 1
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
  %v0 = load i64* @u64v, align 1
  %v1 = trunc i64 %v0 to i8
  store i8 %v1, i8* @i8v, align 1
  %v2 = trunc i64 %v0 to i16
  store i16 %v2, i16* @i16v, align 1
  %v3 = trunc i64 %v0 to i32
  store i32 %v3, i32* @i32v, align 1
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
