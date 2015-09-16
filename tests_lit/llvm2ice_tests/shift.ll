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

define void @conv1() {
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

define void @conv2() {
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

define i32 @shlImmLarge(i32 %val) {
entry:
  %result = shl i32 %val, 257
  ret i32 %result
}
; CHECK-LABEL: shlImmLarge
; CHECK: shl {{.*}},0x1

define i32 @shlImmNeg(i32 %val) {
entry:
  %result = shl i32 %val, -1
  ret i32 %result
}
; CHECK-LABEL: shlImmNeg
; CHECK: shl {{.*}},0xff

define i32 @lshrImmLarge(i32 %val) {
entry:
  %result = lshr i32 %val, 257
  ret i32 %result
}
; CHECK-LABEL: lshrImmLarge
; CHECK: shr {{.*}},0x1

define i32 @lshrImmNeg(i32 %val) {
entry:
  %result = lshr i32 %val, -1
  ret i32 %result
}
; CHECK-LABEL: lshrImmNeg
; CHECK: shr {{.*}},0xff

define i32 @ashrImmLarge(i32 %val) {
entry:
  %result = ashr i32 %val, 257
  ret i32 %result
}
; CHECK-LABEL: ashrImmLarge
; CHECK: sar {{.*}},0x1

define i32 @ashrImmNeg(i32 %val) {
entry:
  %result = ashr i32 %val, -1
  ret i32 %result
}
; CHECK-LABEL: ashrImmNeg
; CHECK: sar {{.*}},0xff
