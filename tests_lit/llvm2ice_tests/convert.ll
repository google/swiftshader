; Simple test of signed and unsigned integer conversions.

; RUN: %if --need=target_X8632 --command %p2i --filetype=obj --disassemble \
; RUN:   --target x8632 -i %s --args -O2 \
; RUN:   | %if --need=target_X8632 --command FileCheck %s

; RUN: %if --need=target_X8632 --command %p2i --filetype=obj --disassemble \
; RUN:   --target x8632 -i %s --args -Om1 \
; RUN:   | %if --need=target_X8632 --command FileCheck %s

; TODO(jvoung): Stop skipping unimplemented parts (via --skip-unimplemented)
; once enough infrastructure is in. Also, switch to --filetype=obj
; when possible.
; RUN: %if --need=target_ARM32 --need=allow_dump \
; RUN:   --command %p2i --filetype=asm --assemble \
; RUN:   --disassemble --target arm32 -i %s --args -O2 --skip-unimplemented \
; RUN:   | %if --need=target_ARM32 --need=allow_dump \
; RUN:   --command FileCheck --check-prefix ARM32 %s

; RUN: %if --need=target_ARM32 --need=allow_dump \
; RUN:   --command %p2i --filetype=asm --assemble \
; RUN:   --disassemble --target arm32 -i %s --args -Om1 --skip-unimplemented \
; RUN:   | %if --need=target_ARM32 --need=allow_dump \
; RUN:   --command FileCheck --check-prefix ARM32 %s

@i8v = internal global [1 x i8] zeroinitializer, align 1
@i16v = internal global [2 x i8] zeroinitializer, align 2
@i32v = internal global [4 x i8] zeroinitializer, align 4
@i64v = internal global [8 x i8] zeroinitializer, align 8
@u8v = internal global [1 x i8] zeroinitializer, align 1
@u16v = internal global [2 x i8] zeroinitializer, align 2
@u32v = internal global [4 x i8] zeroinitializer, align 4
@u64v = internal global [8 x i8] zeroinitializer, align 8

define internal void @from_int8() {
entry:
  %__0 = bitcast [1 x i8]* @i8v to i8*
  %v0 = load i8, i8* %__0, align 1
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
; CHECK: mov {{.*}},BYTE PTR
; CHECK: movsx e{{.*}},{{[a-d]l|BYTE PTR}}
; CHECK: mov WORD PTR
; CHECK: movsx
; CHECK: mov DWORD PTR
; CHECK: movsx
; CHECK: sar {{.*}},0x1f
; CHECK-DAG: ds:0x0,{{.*}}i64v
; CHECK-DAG: ds:0x4,{{.*}}i64v

; ARM32-LABEL: from_int8
; ARM32: movw {{.*}}i8v
; ARM32: ldrb
; ARM32: sxtb
; ARM32: movw {{.*}}i16v
; ARM32: strh
; ARM32: sxtb
; ARM32: movw {{.*}}i32v
; ARM32: str r
; ARM32: sxtb
; ARM32: asr
; ARM32: movw {{.*}}i64v
; ARM32-DAG: str r{{.*}}, [r{{[0-9]+}}]
; ARM32-DAG: str r{{.*}}, [{{.*}}, #4]

define internal void @from_int16() {
entry:
  %__0 = bitcast [2 x i8]* @i16v to i16*
  %v0 = load i16, i16* %__0, align 1
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
; CHECK: mov {{.*}},WORD PTR
; CHECK: 0x0 {{.*}}i16v
; CHECK: movsx e{{.*}},{{.*x|[ds]i|bp|WORD PTR}}
; CHECK: 0x0,{{.*}}i32v
; CHECK: movsx e{{.*}},{{.*x|[ds]i|bp|WORD PTR}}
; CHECK: sar {{.*}},0x1f
; CHECK: 0x0,{{.*}}i64v

; ARM32-LABEL: from_int16
; ARM32: movw {{.*}}i16v
; ARM32: ldrh
; ARM32: movw {{.*}}i8v
; ARM32: strb
; ARM32: sxth
; ARM32: movw {{.*}}i32v
; ARM32: str r
; ARM32: sxth
; ARM32: asr
; ARM32: movw {{.*}}i64v
; ARM32: str r

define internal void @from_int32() {
entry:
  %__0 = bitcast [4 x i8]* @i32v to i32*
  %v0 = load i32, i32* %__0, align 1
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
; CHECK: 0x0 {{.*}} i32v
; CHECK: 0x0,{{.*}} i8v
; CHECK: 0x0,{{.*}} i16v
; CHECK: sar {{.*}},0x1f
; CHECK: 0x0,{{.*}} i64v

; ARM32-LABEL: from_int32
; ARM32: movw {{.*}}i32v
; ARM32: ldr r
; ARM32: movw {{.*}}i8v
; ARM32: strb
; ARM32: movw {{.*}}i16v
; ARM32: strh
; ARM32: asr
; ARM32: movw {{.*}}i64v
; ARM32: str r

define internal void @from_int64() {
entry:
  %__0 = bitcast [8 x i8]* @i64v to i64*
  %v0 = load i64, i64* %__0, align 1
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
; CHECK: 0x0 {{.*}} i64v
; CHECK: 0x0,{{.*}} i8v
; CHECK: 0x0,{{.*}} i16v
; CHECK: 0x0,{{.*}} i32v

; ARM32-LABEL: from_int64
; ARM32: movw {{.*}}i64v
; ARM32: ldr r
; ARM32: movw {{.*}}i8v
; ARM32: strb
; ARM32: movw {{.*}}i16v
; ARM32: strh
; ARM32: movw {{.*}}i32v
; ARM32: str r

define internal void @from_uint8() {
entry:
  %__0 = bitcast [1 x i8]* @u8v to i8*
  %v0 = load i8, i8* %__0, align 1
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
; CHECK: 0x0 {{.*}} u8v
; CHECK: movzx e{{.*}},{{[a-d]l|BYTE PTR}}
; CHECK: 0x0,{{.*}} i16v
; CHECK: movzx
; CHECK: 0x0,{{.*}} i32v
; CHECK: movzx
; CHECK: mov {{.*}},0x0
; CHECK: 0x0,{{.*}} i64v

; ARM32-LABEL: from_uint8
; ARM32: movw {{.*}}u8v
; ARM32: ldrb
; ARM32: uxtb
; ARM32: movw {{.*}}i16v
; ARM32: strh
; ARM32: uxtb
; ARM32: movw {{.*}}i32v
; ARM32: str r
; ARM32: uxtb
; ARM32: mov {{.*}}, #0
; ARM32: movw {{.*}}i64v
; ARM32: str r

define internal void @from_uint16() {
entry:
  %__0 = bitcast [2 x i8]* @u16v to i16*
  %v0 = load i16, i16* %__0, align 1
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
; CHECK: 0x0 {{.*}} u16v
; CHECK: 0x0,{{.*}} i8v
; CHECK: movzx e{{.*}},{{.*x|[ds]i|bp|WORD PTR}}
; CHECK: 0x0,{{.*}} i32v
; CHECK: movzx e{{.*}},{{.*x|[ds]i|bp|WORD PTR}}
; CHECK: mov {{.*}},0x0
; CHECK: 0x0,{{.*}} i64v

; ARM32-LABEL: from_uint16
; ARM32: movw {{.*}}u16v
; ARM32: ldrh
; ARM32: movw {{.*}}i8v
; ARM32: strb
; ARM32: uxth
; ARM32: movw {{.*}}i32v
; ARM32: str r
; ARM32: uxth
; ARM32: mov {{.*}}, #0
; ARM32: movw {{.*}}i64v
; ARM32: str r

define internal void @from_uint32() {
entry:
  %__0 = bitcast [4 x i8]* @u32v to i32*
  %v0 = load i32, i32* %__0, align 1
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
; CHECK: 0x0 {{.*}} u32v
; CHECK: 0x0,{{.*}} i8v
; CHECK: 0x0,{{.*}} i16v
; CHECK: mov {{.*}},0x0
; CHECK: 0x0,{{.*}} i64v

; ARM32-LABEL: from_uint32
; ARM32: movw {{.*}}u32v
; ARM32: ldr r
; ARM32: movw {{.*}}i8v
; ARM32: strb
; ARM32: movw {{.*}}i16v
; ARM32: strh
; ARM32: mov {{.*}}, #0
; ARM32: movw {{.*}}i64v
; ARM32: str r

define internal void @from_uint64() {
entry:
  %__0 = bitcast [8 x i8]* @u64v to i64*
  %v0 = load i64, i64* %__0, align 1
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
; CHECK: 0x0 {{.*}} u64v
; CHECK: 0x0,{{.*}} i8v
; CHECK: 0x0,{{.*}} i16v
; CHECK: 0x0,{{.*}} i32v

; ARM32-LABEL: from_uint64
; ARM32: movw {{.*}}u64v
; ARM32: ldr r
; ARM32: movw {{.*}}i8v
; ARM32: strb
; ARM32: movw {{.*}}i16v
; ARM32: strh
; ARM32: movw {{.*}}i32v
; ARM32: str r
