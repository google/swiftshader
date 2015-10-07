; This is a test of C-level conversion operations that clang lowers
; into pairs of shifts.

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


@i1 = internal global [4 x i8] zeroinitializer, align 4
@i2 = internal global [4 x i8] zeroinitializer, align 4
@u1 = internal global [4 x i8] zeroinitializer, align 4

define internal void @conv1() {
entry:
  %__0 = bitcast [4 x i8]* @u1 to i32*
  %v0 = load i32, i32* %__0, align 1
  %sext = shl i32 %v0, 24
  %v1 = ashr i32 %sext, 24
  %__4 = bitcast [4 x i8]* @i1 to i32*
  store i32 %v1, i32* %__4, align 1
  ret void
}
; CHECK-LABEL: conv1
; CHECK: shl {{.*}},0x18
; CHECK: sar {{.*}},0x18

; ARM32-LABEL: conv1
; ARM32: lsl {{.*}}, #24
; ARM32: asr {{.*}}, #24

define internal void @conv2() {
entry:
  %__0 = bitcast [4 x i8]* @u1 to i32*
  %v0 = load i32, i32* %__0, align 1
  %sext1 = shl i32 %v0, 16
  %v1 = lshr i32 %sext1, 16
  %__4 = bitcast [4 x i8]* @i2 to i32*
  store i32 %v1, i32* %__4, align 1
  ret void
}
; CHECK-LABEL: conv2
; CHECK: shl {{.*}},0x10
; CHECK: shr {{.*}},0x10

; ARM32-LABEL: conv2
; ARM32: lsl {{.*}}, #16
; ARM32: lsr {{.*}}, #16

define internal i32 @shlImmLarge(i32 %val) {
entry:
  %result = shl i32 %val, 257
  ret i32 %result
}
; CHECK-LABEL: shlImmLarge
; CHECK: shl {{.*}},0x1

define internal i32 @shlImmNeg(i32 %val) {
entry:
  %result = shl i32 %val, -1
  ret i32 %result
}
; CHECK-LABEL: shlImmNeg
; CHECK: shl {{.*}},0xff

define internal i32 @lshrImmLarge(i32 %val) {
entry:
  %result = lshr i32 %val, 257
  ret i32 %result
}
; CHECK-LABEL: lshrImmLarge
; CHECK: shr {{.*}},0x1

define internal i32 @lshrImmNeg(i32 %val) {
entry:
  %result = lshr i32 %val, -1
  ret i32 %result
}
; CHECK-LABEL: lshrImmNeg
; CHECK: shr {{.*}},0xff

define internal i32 @ashrImmLarge(i32 %val) {
entry:
  %result = ashr i32 %val, 257
  ret i32 %result
}
; CHECK-LABEL: ashrImmLarge
; CHECK: sar {{.*}},0x1

define internal i32 @ashrImmNeg(i32 %val) {
entry:
  %result = ashr i32 %val, -1
  ret i32 %result
}
; CHECK-LABEL: ashrImmNeg
; CHECK: sar {{.*}},0xff

define internal i64 @shlImm64One(i64 %val) {
entry:
  %result = shl i64 %val, 1
  ret i64 %result
}
; CHECK-LABEL: shlImm64One
; CHECK: shl {{.*}},1

define internal i64 @shlImm64LessThan32(i64 %val) {
entry:
  %result = shl i64 %val, 4
  ret i64 %result
}
; CHECK-LABEL: shlImm64LessThan32
; CHECK: shl {{.*}},0x4

define internal i64 @shlImm64Equal32(i64 %val) {
entry:
  %result = shl i64 %val, 32
  ret i64 %result
}
; CHECK-LABEL: shlImm64Equal32
; CHECK-NOT: shl

define internal i64 @shlImm64GreaterThan32(i64 %val) {
entry:
  %result = shl i64 %val, 40
  ret i64 %result
}
; CHECK-LABEL: shlImm64GreaterThan32
; CHECK: shl {{.*}},0x8

define internal i64 @lshrImm64One(i64 %val) {
entry:
  %result = lshr i64 %val, 1
  ret i64 %result
}
; CHECK-LABEL: lshrImm64One
; CHECK: shr {{.*}},1

define internal i64 @lshrImm64LessThan32(i64 %val) {
entry:
  %result = lshr i64 %val, 4
  ret i64 %result
}
; CHECK-LABEL: lshrImm64LessThan32
; CHECK: shrd {{.*}},0x4
; CHECK: shr {{.*}},0x4

define internal i64 @lshrImm64Equal32(i64 %val) {
entry:
  %result = lshr i64 %val, 32
  ret i64 %result
}
; CHECK-LABEL: lshrImm64Equal32
; CHECK-NOT: shr

define internal i64 @lshrImm64GreaterThan32(i64 %val) {
entry:
  %result = lshr i64 %val, 40
  ret i64 %result
}
; CHECK-LABEL: lshrImm64GreaterThan32
; CHECK-NOT: shrd
; CHECK: shr {{.*}},0x8

define internal i64 @ashrImm64One(i64 %val) {
entry:
  %result = ashr i64 %val, 1
  ret i64 %result
}
; CHECK-LABEL: ashrImm64One
; CHECK: shrd {{.*}},0x1
; CHECK: sar {{.*}},1

define internal i64 @ashrImm64LessThan32(i64 %val) {
entry:
  %result = ashr i64 %val, 4
  ret i64 %result
}
; CHECK-LABEL: ashrImm64LessThan32
; CHECK: shrd {{.*}},0x4
; CHECK: sar {{.*}},0x4

define internal i64 @ashrImm64Equal32(i64 %val) {
entry:
  %result = ashr i64 %val, 32
  ret i64 %result
}
; CHECK-LABEL: ashrImm64Equal32
; CHECK: sar {{.*}},0x1f
; CHECK-NOT: shrd

define internal i64 @ashrImm64GreaterThan32(i64 %val) {
entry:
  %result = ashr i64 %val, 40
  ret i64 %result
}
; CHECK-LABEL: ashrImm64GreaterThan32
; CHECK: sar {{.*}},0x1f
; CHECK: shrd {{.*}},0x8
