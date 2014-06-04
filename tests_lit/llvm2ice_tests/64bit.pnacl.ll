; This tries to be a comprehensive test of i64 operations, in
; particular the patterns for lowering i64 operations into constituent
; i32 operations on x86-32.

; RUN: %llvm2ice -O2 --verbose none %s | FileCheck %s
; RUN: %llvm2ice -Om1 --verbose none %s | FileCheck --check-prefix=OPTM1 %s
; RUN: %llvm2ice --verbose none %s | FileCheck --check-prefix=ERRORS %s
; RUN: %llvm2iceinsts %s | %szdiff %s | FileCheck --check-prefix=DUMP %s
; RUN: %llvm2iceinsts --pnacl %s | %szdiff %s \
; RUN:                           | FileCheck --check-prefix=DUMP %s

@__init_array_start = internal constant [0 x i8] zeroinitializer, align 4
@__fini_array_start = internal constant [0 x i8] zeroinitializer, align 4
@__tls_template_start = internal constant [0 x i8] zeroinitializer, align 8
@__tls_template_alignment = internal constant [4 x i8] c"\01\00\00\00", align 4

define internal i32 @ignore64BitArg(i64 %a, i32 %b, i64 %c) {
entry:
  ret i32 %b
}

define internal i32 @pass64BitArg(i64 %a, i64 %b, i64 %c, i64 %d, i64 %e, i64 %f) {
entry:
  %call = call i32 @ignore64BitArgNoInline(i64 %a, i32 123, i64 %b)
  %call1 = call i32 @ignore64BitArgNoInline(i64 %c, i32 123, i64 %d)
  %call2 = call i32 @ignore64BitArgNoInline(i64 %e, i32 123, i64 %f)
  %add = add i32 %call1, %call
  %add3 = add i32 %add, %call2
  ret i32 %add3
}
; CHECK: pass64BitArg:
; CHECK:      push    123
; CHECK-NEXT: push
; CHECK-NEXT: push
; CHECK-NEXT: call    ignore64BitArgNoInline
; CHECK:      push
; CHECK-NEXT: push
; CHECK-NEXT: push    123
; CHECK-NEXT: push
; CHECK-NEXT: push
; CHECK-NEXT: call    ignore64BitArgNoInline
; CHECK:      push
; CHECK-NEXT: push
; CHECK-NEXT: push    123
; CHECK-NEXT: push
; CHECK-NEXT: push
; CHECK-NEXT: call    ignore64BitArgNoInline
;
; OPTM1: pass64BitArg:
; OPTM1:      push    123
; OPTM1-NEXT: push
; OPTM1-NEXT: push
; OPTM1-NEXT: call    ignore64BitArgNoInline
; OPTM1:      push
; OPTM1-NEXT: push
; OPTM1-NEXT: push    123
; OPTM1-NEXT: push
; OPTM1-NEXT: push
; OPTM1-NEXT: call    ignore64BitArgNoInline
; OPTM1:      push
; OPTM1-NEXT: push
; OPTM1-NEXT: push    123
; OPTM1-NEXT: push
; OPTM1-NEXT: push
; OPTM1-NEXT: call    ignore64BitArgNoInline

declare i32 @ignore64BitArgNoInline(i64, i32, i64)

define internal i32 @pass64BitConstArg(i64 %a, i64 %b) {
entry:
  %call = call i32 @ignore64BitArgNoInline(i64 %a, i32 123, i64 -2401053092306725256)
  ret i32 %call
}
; CHECK: pass64BitConstArg:
; CHECK:      push    3735928559
; CHECK-NEXT: push    305419896
; CHECK-NEXT: push    123
; CHECK-NEXT: push    ecx
; CHECK-NEXT: push    eax
; CHECK-NEXT: call    ignore64BitArgNoInline
;
; OPTM1: pass64BitConstArg:
; OPTM1:      push    3735928559
; OPTM1-NEXT: push    305419896
; OPTM1-NEXT: push    123
; OPTM1-NEXT: push    dword ptr [
; OPTM1-NEXT: push    dword ptr [
; OPTM1-NEXT: call    ignore64BitArgNoInline

define internal i64 @return64BitArg(i64 %a) {
entry:
  ret i64 %a
}
; CHECK: return64BitArg:
; CHECK: mov     {{.*}}, dword ptr [esp+4]
; CHECK: mov     {{.*}}, dword ptr [esp+8]
; CHECK: ret
;
; OPTM1: return64BitArg:
; OPTM1: mov     {{.*}}, dword ptr [esp+4]
; OPTM1: mov     {{.*}}, dword ptr [esp+8]
; OPTM1: ret

define internal i64 @return64BitConst() {
entry:
  ret i64 -2401053092306725256
}
; CHECK: return64BitConst:
; CHECK: mov     eax, 305419896
; CHECK: mov     edx, 3735928559
; CHECK: ret
;
; OPTM1: return64BitConst:
; OPTM1: mov     eax, 305419896
; OPTM1: mov     edx, 3735928559
; OPTM1: ret

define internal i64 @add64BitSigned(i64 %a, i64 %b) {
entry:
  %add = add i64 %b, %a
  ret i64 %add
}
; CHECK: add64BitSigned:
; CHECK: add
; CHECK: adc
; CHECK: ret
;
; OPTM1: add64BitSigned:
; OPTM1: add
; OPTM1: adc
; OPTM1: ret

define internal i64 @add64BitUnsigned(i64 %a, i64 %b) {
entry:
  %add = add i64 %b, %a
  ret i64 %add
}
; CHECK: add64BitUnsigned:
; CHECK: add
; CHECK: adc
; CHECK: ret
;
; OPTM1: add64BitUnsigned:
; OPTM1: add
; OPTM1: adc
; OPTM1: ret

define internal i64 @sub64BitSigned(i64 %a, i64 %b) {
entry:
  %sub = sub i64 %a, %b
  ret i64 %sub
}
; CHECK: sub64BitSigned:
; CHECK: sub
; CHECK: sbb
; CHECK: ret
;
; OPTM1: sub64BitSigned:
; OPTM1: sub
; OPTM1: sbb
; OPTM1: ret

define internal i64 @sub64BitUnsigned(i64 %a, i64 %b) {
entry:
  %sub = sub i64 %a, %b
  ret i64 %sub
}
; CHECK: sub64BitUnsigned:
; CHECK: sub
; CHECK: sbb
; CHECK: ret
;
; OPTM1: sub64BitUnsigned:
; OPTM1: sub
; OPTM1: sbb
; OPTM1: ret

define internal i64 @mul64BitSigned(i64 %a, i64 %b) {
entry:
  %mul = mul i64 %b, %a
  ret i64 %mul
}
; CHECK: mul64BitSigned:
; CHECK: imul
; CHECK: imul
; CHECK: mul
; CHECK: add
; CHECK: add
; CHECK: ret
;
; OPTM1: mul64BitSigned:
; OPTM1: imul
; OPTM1: imul
; OPTM1: mul
; OPTM1: add
; OPTM1: add
; OPTM1: ret

define internal i64 @mul64BitUnsigned(i64 %a, i64 %b) {
entry:
  %mul = mul i64 %b, %a
  ret i64 %mul
}
; CHECK: mul64BitUnsigned:
; CHECK: imul
; CHECK: imul
; CHECK: mul
; CHECK: add
; CHECK: add
; CHECK: ret
;
; OPTM1: mul64BitUnsigned:
; OPTM1: imul
; OPTM1: imul
; OPTM1: mul
; OPTM1: add
; OPTM1: add
; OPTM1: ret

define internal i64 @div64BitSigned(i64 %a, i64 %b) {
entry:
  %div = sdiv i64 %a, %b
  ret i64 %div
}
; CHECK: div64BitSigned:
; CHECK: call    __divdi3
; CHECK: ret
;
; OPTM1: div64BitSigned:
; OPTM1: call    __divdi3
; OPTM1: ret

define internal i64 @div64BitUnsigned(i64 %a, i64 %b) {
entry:
  %div = udiv i64 %a, %b
  ret i64 %div
}
; CHECK: div64BitUnsigned:
; CHECK: call    __udivdi3
; CHECK: ret
;
; OPTM1: div64BitUnsigned:
; OPTM1: call    __udivdi3
; OPTM1: ret

define internal i64 @rem64BitSigned(i64 %a, i64 %b) {
entry:
  %rem = srem i64 %a, %b
  ret i64 %rem
}
; CHECK: rem64BitSigned:
; CHECK: call    __moddi3
; CHECK: ret
;
; OPTM1: rem64BitSigned:
; OPTM1: call    __moddi3
; OPTM1: ret

define internal i64 @rem64BitUnsigned(i64 %a, i64 %b) {
entry:
  %rem = urem i64 %a, %b
  ret i64 %rem
}
; CHECK: rem64BitUnsigned:
; CHECK: call    __umoddi3
; CHECK: ret
;
; OPTM1: rem64BitUnsigned:
; OPTM1: call    __umoddi3
; OPTM1: ret

define internal i64 @shl64BitSigned(i64 %a, i64 %b) {
entry:
  %shl = shl i64 %a, %b
  ret i64 %shl
}
; CHECK: shl64BitSigned:
; CHECK: shld
; CHECK: shl e
; CHECK: test {{.*}}, 32
; CHECK: je
;
; OPTM1: shl64BitSigned:
; OPTM1: shld
; OPTM1: shl e
; OPTM1: test {{.*}}, 32
; OPTM1: je

define internal i64 @shl64BitUnsigned(i64 %a, i64 %b) {
entry:
  %shl = shl i64 %a, %b
  ret i64 %shl
}
; CHECK: shl64BitUnsigned:
; CHECK: shld
; CHECK: shl e
; CHECK: test {{.*}}, 32
; CHECK: je
;
; OPTM1: shl64BitUnsigned:
; OPTM1: shld
; OPTM1: shl e
; OPTM1: test {{.*}}, 32
; OPTM1: je

define internal i64 @shr64BitSigned(i64 %a, i64 %b) {
entry:
  %shr = ashr i64 %a, %b
  ret i64 %shr
}
; CHECK: shr64BitSigned:
; CHECK: shrd
; CHECK: sar
; CHECK: test {{.*}}, 32
; CHECK: je
; CHECK: sar {{.*}}, 31
;
; OPTM1: shr64BitSigned:
; OPTM1: shrd
; OPTM1: sar
; OPTM1: test {{.*}}, 32
; OPTM1: je
; OPTM1: sar {{.*}}, 31

define internal i64 @shr64BitUnsigned(i64 %a, i64 %b) {
entry:
  %shr = lshr i64 %a, %b
  ret i64 %shr
}
; CHECK: shr64BitUnsigned:
; CHECK: shrd
; CHECK: shr
; CHECK: test {{.*}}, 32
; CHECK: je
;
; OPTM1: shr64BitUnsigned:
; OPTM1: shrd
; OPTM1: shr
; OPTM1: test {{.*}}, 32
; OPTM1: je

define internal i64 @and64BitSigned(i64 %a, i64 %b) {
entry:
  %and = and i64 %b, %a
  ret i64 %and
}
; CHECK: and64BitSigned:
; CHECK: and
; CHECK: and
;
; OPTM1: and64BitSigned:
; OPTM1: and
; OPTM1: and

define internal i64 @and64BitUnsigned(i64 %a, i64 %b) {
entry:
  %and = and i64 %b, %a
  ret i64 %and
}
; CHECK: and64BitUnsigned:
; CHECK: and
; CHECK: and
;
; OPTM1: and64BitUnsigned:
; OPTM1: and
; OPTM1: and

define internal i64 @or64BitSigned(i64 %a, i64 %b) {
entry:
  %or = or i64 %b, %a
  ret i64 %or
}
; CHECK: or64BitSigned:
; CHECK: or
; CHECK: or
;
; OPTM1: or64BitSigned:
; OPTM1: or
; OPTM1: or

define internal i64 @or64BitUnsigned(i64 %a, i64 %b) {
entry:
  %or = or i64 %b, %a
  ret i64 %or
}
; CHECK: or64BitUnsigned:
; CHECK: or
; CHECK: or
;
; OPTM1: or64BitUnsigned:
; OPTM1: or
; OPTM1: or

define internal i64 @xor64BitSigned(i64 %a, i64 %b) {
entry:
  %xor = xor i64 %b, %a
  ret i64 %xor
}
; CHECK: xor64BitSigned:
; CHECK: xor
; CHECK: xor
;
; OPTM1: xor64BitSigned:
; OPTM1: xor
; OPTM1: xor

define internal i64 @xor64BitUnsigned(i64 %a, i64 %b) {
entry:
  %xor = xor i64 %b, %a
  ret i64 %xor
}
; CHECK: xor64BitUnsigned:
; CHECK: xor
; CHECK: xor
;
; OPTM1: xor64BitUnsigned:
; OPTM1: xor
; OPTM1: xor

define internal i32 @trunc64To32Signed(i64 %a) {
entry:
  %conv = trunc i64 %a to i32
  ret i32 %conv
}
; CHECK: trunc64To32Signed:
; CHECK: mov     eax, dword ptr [esp+4]
; CHECK-NEXT: ret
;
; OPTM1: trunc64To32Signed:
; OPTM1: mov     eax, dword ptr [esp+
; OPTM1: ret

define internal i32 @trunc64To16Signed(i64 %a) {
entry:
  %conv = trunc i64 %a to i16
  %conv.ret_ext = sext i16 %conv to i32
  ret i32 %conv.ret_ext
}
; CHECK: trunc64To16Signed:
; CHECK:      mov     eax, dword ptr [esp+4]
; CHECK-NEXT: movsx  eax, ax
; CHECK-NEXT: ret
;
; OPTM1: trunc64To16Signed:
; OPTM1:      mov     eax, dword ptr [esp+
; OPTM1: movsx  eax,
; OPTM1: ret

define internal i32 @trunc64To8Signed(i64 %a) {
entry:
  %conv = trunc i64 %a to i8
  %conv.ret_ext = sext i8 %conv to i32
  ret i32 %conv.ret_ext
}
; CHECK: trunc64To8Signed:
; CHECK:      mov     eax, dword ptr [esp+4]
; CHECK-NEXT: movsx  eax, al
; CHECK-NEXT: ret
;
; OPTM1: trunc64To8Signed:
; OPTM1:      mov     eax, dword ptr [esp+
; OPTM1: movsx  eax,
; OPTM1: ret

define internal i32 @trunc64To32Unsigned(i64 %a) {
entry:
  %conv = trunc i64 %a to i32
  ret i32 %conv
}
; CHECK: trunc64To32Unsigned:
; CHECK: mov     eax, dword ptr [esp+4]
; CHECK-NEXT: ret
;
; OPTM1: trunc64To32Unsigned:
; OPTM1: mov     eax, dword ptr [esp+
; OPTM1: ret

define internal i32 @trunc64To16Unsigned(i64 %a) {
entry:
  %conv = trunc i64 %a to i16
  %conv.ret_ext = zext i16 %conv to i32
  ret i32 %conv.ret_ext
}
; CHECK: trunc64To16Unsigned:
; CHECK:      mov     eax, dword ptr [esp+4]
; CHECK-NEXT: movzx  eax, ax
; CHECK-NEXT: ret
;
; OPTM1: trunc64To16Unsigned:
; OPTM1:      mov     eax, dword ptr [esp+
; OPTM1: movzx  eax,
; OPTM1: ret

define internal i32 @trunc64To8Unsigned(i64 %a) {
entry:
  %conv = trunc i64 %a to i8
  %conv.ret_ext = zext i8 %conv to i32
  ret i32 %conv.ret_ext
}
; CHECK: trunc64To8Unsigned:
; CHECK:      mov     eax, dword ptr [esp+4]
; CHECK-NEXT: movzx  eax, al
; CHECK-NEXT: ret
;
; OPTM1: trunc64To8Unsigned:
; OPTM1:      mov     eax, dword ptr [esp+
; OPTM1: movzx  eax,
; OPTM1: ret

define internal i32 @trunc64To1(i64 %a) {
entry:
;  %tobool = icmp ne i64 %a, 0
  %tobool = trunc i64 %a to i1
  %tobool.ret_ext = zext i1 %tobool to i32
  ret i32 %tobool.ret_ext
}
; CHECK: trunc64To1:
; CHECK:      mov     eax, dword ptr [esp+4]
; CHECK:      and     eax, 1
; CHECK-NEXT: ret
;
; OPTM1: trunc64To1:
; OPTM1:      mov     eax, dword ptr [esp+
; OPTM1:      and     eax, 1
; OPTM1: ret

define internal i64 @sext32To64(i32 %a) {
entry:
  %conv = sext i32 %a to i64
  ret i64 %conv
}
; CHECK: sext32To64:
; CHECK: mov
; CHECK: sar {{.*}}, 31
;
; OPTM1: sext32To64:
; OPTM1: mov
; OPTM1: sar {{.*}}, 31

define internal i64 @sext16To64(i32 %a) {
entry:
  %a.arg_trunc = trunc i32 %a to i16
  %conv = sext i16 %a.arg_trunc to i64
  ret i64 %conv
}
; CHECK: sext16To64:
; CHECK: movsx
; CHECK: sar {{.*}}, 31
;
; OPTM1: sext16To64:
; OPTM1: movsx
; OPTM1: sar {{.*}}, 31

define internal i64 @sext8To64(i32 %a) {
entry:
  %a.arg_trunc = trunc i32 %a to i8
  %conv = sext i8 %a.arg_trunc to i64
  ret i64 %conv
}
; CHECK: sext8To64:
; CHECK: movsx
; CHECK: sar {{.*}}, 31
;
; OPTM1: sext8To64:
; OPTM1: movsx
; OPTM1: sar {{.*}}, 31

define internal i64 @zext32To64(i32 %a) {
entry:
  %conv = zext i32 %a to i64
  ret i64 %conv
}
; CHECK: zext32To64:
; CHECK: mov
; CHECK: mov {{.*}}, 0
;
; OPTM1: zext32To64:
; OPTM1: mov
; OPTM1: mov {{.*}}, 0

define internal i64 @zext16To64(i32 %a) {
entry:
  %a.arg_trunc = trunc i32 %a to i16
  %conv = zext i16 %a.arg_trunc to i64
  ret i64 %conv
}
; CHECK: zext16To64:
; CHECK: movzx
; CHECK: mov {{.*}}, 0
;
; OPTM1: zext16To64:
; OPTM1: movzx
; OPTM1: mov {{.*}}, 0

define internal i64 @zext8To64(i32 %a) {
entry:
  %a.arg_trunc = trunc i32 %a to i8
  %conv = zext i8 %a.arg_trunc to i64
  ret i64 %conv
}
; CHECK: zext8To64:
; CHECK: movzx
; CHECK: mov {{.*}}, 0
;
; OPTM1: zext8To64:
; OPTM1: movzx
; OPTM1: mov {{.*}}, 0

define internal i64 @zext1To64(i32 %a) {
entry:
  %a.arg_trunc = trunc i32 %a to i1
  %conv = zext i1 %a.arg_trunc to i64
  ret i64 %conv
}
; CHECK: zext1To64:
; CHECK: movzx
; CHECK: mov {{.*}}, 0
;
; OPTM1: zext1To64:
; OPTM1: movzx
; OPTM1: mov {{.*}}, 0

define internal void @icmpEq64(i64 %a, i64 %b, i64 %c, i64 %d) {
entry:
  %cmp = icmp eq i64 %a, %b
  br i1 %cmp, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  call void @func()
  br label %if.end

if.end:                                           ; preds = %if.then, %entry
  %cmp1 = icmp eq i64 %c, %d
  br i1 %cmp1, label %if.then2, label %if.end3

if.then2:                                         ; preds = %if.end
  call void @func()
  br label %if.end3

if.end3:                                          ; preds = %if.then2, %if.end
  ret void
}
; CHECK: icmpEq64:
; CHECK: jne
; CHECK: jne
; CHECK: call
; CHECK: jne
; CHECK: jne
; CHECK: call
;
; OPTM1: icmpEq64:
; OPTM1: jne
; OPTM1: jne
; OPTM1: call
; OPTM1: jne
; OPTM1: jne
; OPTM1: call

declare void @func()

define internal void @icmpNe64(i64 %a, i64 %b, i64 %c, i64 %d) {
entry:
  %cmp = icmp ne i64 %a, %b
  br i1 %cmp, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  call void @func()
  br label %if.end

if.end:                                           ; preds = %if.then, %entry
  %cmp1 = icmp ne i64 %c, %d
  br i1 %cmp1, label %if.then2, label %if.end3

if.then2:                                         ; preds = %if.end
  call void @func()
  br label %if.end3

if.end3:                                          ; preds = %if.end, %if.then2
  ret void
}
; CHECK: icmpNe64:
; CHECK: jne
; CHECK: jne
; CHECK: call
; CHECK: jne
; CHECK: jne
; CHECK: call
;
; OPTM1: icmpNe64:
; OPTM1: jne
; OPTM1: jne
; OPTM1: call
; OPTM1: jne
; OPTM1: jne
; OPTM1: call

define internal void @icmpGt64(i64 %a, i64 %b, i64 %c, i64 %d) {
entry:
  %cmp = icmp ugt i64 %a, %b
  br i1 %cmp, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  call void @func()
  br label %if.end

if.end:                                           ; preds = %if.then, %entry
  %cmp1 = icmp sgt i64 %c, %d
  br i1 %cmp1, label %if.then2, label %if.end3

if.then2:                                         ; preds = %if.end
  call void @func()
  br label %if.end3

if.end3:                                          ; preds = %if.then2, %if.end
  ret void
}
; CHECK: icmpGt64:
; CHECK: ja
; CHECK: jb
; CHECK: ja
; CHECK: call
; CHECK: jg
; CHECK: jl
; CHECK: ja
; CHECK: call
;
; OPTM1: icmpGt64:
; OPTM1: ja
; OPTM1: jb
; OPTM1: ja
; OPTM1: call
; OPTM1: jg
; OPTM1: jl
; OPTM1: ja
; OPTM1: call

define internal void @icmpGe64(i64 %a, i64 %b, i64 %c, i64 %d) {
entry:
  %cmp = icmp uge i64 %a, %b
  br i1 %cmp, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  call void @func()
  br label %if.end

if.end:                                           ; preds = %if.then, %entry
  %cmp1 = icmp sge i64 %c, %d
  br i1 %cmp1, label %if.then2, label %if.end3

if.then2:                                         ; preds = %if.end
  call void @func()
  br label %if.end3

if.end3:                                          ; preds = %if.end, %if.then2
  ret void
}
; CHECK: icmpGe64:
; CHECK: ja
; CHECK: jb
; CHECK: jae
; CHECK: call
; CHECK: jg
; CHECK: jl
; CHECK: jae
; CHECK: call
;
; OPTM1: icmpGe64:
; OPTM1: ja
; OPTM1: jb
; OPTM1: jae
; OPTM1: call
; OPTM1: jg
; OPTM1: jl
; OPTM1: jae
; OPTM1: call

define internal void @icmpLt64(i64 %a, i64 %b, i64 %c, i64 %d) {
entry:
  %cmp = icmp ult i64 %a, %b
  br i1 %cmp, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  call void @func()
  br label %if.end

if.end:                                           ; preds = %if.then, %entry
  %cmp1 = icmp slt i64 %c, %d
  br i1 %cmp1, label %if.then2, label %if.end3

if.then2:                                         ; preds = %if.end
  call void @func()
  br label %if.end3

if.end3:                                          ; preds = %if.then2, %if.end
  ret void
}
; CHECK: icmpLt64:
; CHECK: jb
; CHECK: ja
; CHECK: jb
; CHECK: call
; CHECK: jl
; CHECK: jg
; CHECK: jb
; CHECK: call
;
; OPTM1: icmpLt64:
; OPTM1: jb
; OPTM1: ja
; OPTM1: jb
; OPTM1: call
; OPTM1: jl
; OPTM1: jg
; OPTM1: jb
; OPTM1: call

define internal void @icmpLe64(i64 %a, i64 %b, i64 %c, i64 %d) {
entry:
  %cmp = icmp ule i64 %a, %b
  br i1 %cmp, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  call void @func()
  br label %if.end

if.end:                                           ; preds = %if.then, %entry
  %cmp1 = icmp sle i64 %c, %d
  br i1 %cmp1, label %if.then2, label %if.end3

if.then2:                                         ; preds = %if.end
  call void @func()
  br label %if.end3

if.end3:                                          ; preds = %if.end, %if.then2
  ret void
}
; CHECK: icmpLe64:
; CHECK: jb
; CHECK: ja
; CHECK: jbe
; CHECK: call
; CHECK: jl
; CHECK: jg
; CHECK: jbe
; CHECK: call
;
; OPTM1: icmpLe64:
; OPTM1: jb
; OPTM1: ja
; OPTM1: jbe
; OPTM1: call
; OPTM1: jl
; OPTM1: jg
; OPTM1: jbe
; OPTM1: call

define internal i32 @icmpEq64Bool(i64 %a, i64 %b) {
entry:
  %cmp = icmp eq i64 %a, %b
  %cmp.ret_ext = zext i1 %cmp to i32
  ret i32 %cmp.ret_ext
}
; CHECK: icmpEq64Bool:
; CHECK: jne
; CHECK: jne
;
; OPTM1: icmpEq64Bool:
; OPTM1: jne
; OPTM1: jne

define internal i32 @icmpNe64Bool(i64 %a, i64 %b) {
entry:
  %cmp = icmp ne i64 %a, %b
  %cmp.ret_ext = zext i1 %cmp to i32
  ret i32 %cmp.ret_ext
}
; CHECK: icmpNe64Bool:
; CHECK: jne
; CHECK: jne
;
; OPTM1: icmpNe64Bool:
; OPTM1: jne
; OPTM1: jne

define internal i32 @icmpSgt64Bool(i64 %a, i64 %b) {
entry:
  %cmp = icmp sgt i64 %a, %b
  %cmp.ret_ext = zext i1 %cmp to i32
  ret i32 %cmp.ret_ext
}
; CHECK: icmpSgt64Bool:
; CHECK: cmp
; CHECK: jg
; CHECK: jl
; CHECK: cmp
; CHECK: ja
;
; OPTM1: icmpSgt64Bool:
; OPTM1: cmp
; OPTM1: jg
; OPTM1: jl
; OPTM1: cmp
; OPTM1: ja

define internal i32 @icmpUgt64Bool(i64 %a, i64 %b) {
entry:
  %cmp = icmp ugt i64 %a, %b
  %cmp.ret_ext = zext i1 %cmp to i32
  ret i32 %cmp.ret_ext
}
; CHECK: icmpUgt64Bool:
; CHECK: cmp
; CHECK: ja
; CHECK: jb
; CHECK: cmp
; CHECK: ja
;
; OPTM1: icmpUgt64Bool:
; OPTM1: cmp
; OPTM1: ja
; OPTM1: jb
; OPTM1: cmp
; OPTM1: ja

define internal i32 @icmpSge64Bool(i64 %a, i64 %b) {
entry:
  %cmp = icmp sge i64 %a, %b
  %cmp.ret_ext = zext i1 %cmp to i32
  ret i32 %cmp.ret_ext
}
; CHECK: icmpSge64Bool:
; CHECK: cmp
; CHECK: jg
; CHECK: jl
; CHECK: cmp
; CHECK: jae
;
; OPTM1: icmpSge64Bool:
; OPTM1: cmp
; OPTM1: jg
; OPTM1: jl
; OPTM1: cmp
; OPTM1: jae

define internal i32 @icmpUge64Bool(i64 %a, i64 %b) {
entry:
  %cmp = icmp uge i64 %a, %b
  %cmp.ret_ext = zext i1 %cmp to i32
  ret i32 %cmp.ret_ext
}
; CHECK: icmpUge64Bool:
; CHECK: cmp
; CHECK: ja
; CHECK: jb
; CHECK: cmp
; CHECK: jae
;
; OPTM1: icmpUge64Bool:
; OPTM1: cmp
; OPTM1: ja
; OPTM1: jb
; OPTM1: cmp
; OPTM1: jae

define internal i32 @icmpSlt64Bool(i64 %a, i64 %b) {
entry:
  %cmp = icmp slt i64 %a, %b
  %cmp.ret_ext = zext i1 %cmp to i32
  ret i32 %cmp.ret_ext
}
; CHECK: icmpSlt64Bool:
; CHECK: cmp
; CHECK: jl
; CHECK: jg
; CHECK: cmp
; CHECK: jb
;
; OPTM1: icmpSlt64Bool:
; OPTM1: cmp
; OPTM1: jl
; OPTM1: jg
; OPTM1: cmp
; OPTM1: jb

define internal i32 @icmpUlt64Bool(i64 %a, i64 %b) {
entry:
  %cmp = icmp ult i64 %a, %b
  %cmp.ret_ext = zext i1 %cmp to i32
  ret i32 %cmp.ret_ext
}
; CHECK: icmpUlt64Bool:
; CHECK: cmp
; CHECK: jb
; CHECK: ja
; CHECK: cmp
; CHECK: jb
;
; OPTM1: icmpUlt64Bool:
; OPTM1: cmp
; OPTM1: jb
; OPTM1: ja
; OPTM1: cmp
; OPTM1: jb

define internal i32 @icmpSle64Bool(i64 %a, i64 %b) {
entry:
  %cmp = icmp sle i64 %a, %b
  %cmp.ret_ext = zext i1 %cmp to i32
  ret i32 %cmp.ret_ext
}
; CHECK: icmpSle64Bool:
; CHECK: cmp
; CHECK: jl
; CHECK: jg
; CHECK: cmp
; CHECK: jbe
;
; OPTM1: icmpSle64Bool:
; OPTM1: cmp
; OPTM1: jl
; OPTM1: jg
; OPTM1: cmp
; OPTM1: jbe

define internal i32 @icmpUle64Bool(i64 %a, i64 %b) {
entry:
  %cmp = icmp ule i64 %a, %b
  %cmp.ret_ext = zext i1 %cmp to i32
  ret i32 %cmp.ret_ext
}
; CHECK: icmpUle64Bool:
; CHECK: cmp
; CHECK: jb
; CHECK: ja
; CHECK: cmp
; CHECK: jbe
;
; OPTM1: icmpUle64Bool:
; OPTM1: cmp
; OPTM1: jb
; OPTM1: ja
; OPTM1: cmp
; OPTM1: jbe

define internal i64 @load64(i32 %a) {
entry:
  %__1 = inttoptr i32 %a to i64*
  %v0 = load i64* %__1, align 1
  ret i64 %v0
}
; CHECK: load64:
; CHECK: mov e[[REGISTER:[a-z]+]], dword ptr [esp+4]
; CHECK-NEXT: mov {{.*}}, dword ptr [e[[REGISTER]]]
; CHECK-NEXT: mov {{.*}}, dword ptr [e[[REGISTER]]+4]
;
; OPTM1: load64:
; OPTM1: mov e{{..}}, dword ptr [e{{..}}]
; OPTM1: mov e{{..}}, dword ptr [e{{..}}+4]

define internal void @store64(i32 %a, i64 %value) {
entry:
  %__2 = inttoptr i32 %a to i64*
  store i64 %value, i64* %__2, align 1
  ret void
}
; CHECK: store64:
; CHECK: mov e[[REGISTER:[a-z]+]], dword ptr [esp+4]
; CHECK: mov dword ptr [e[[REGISTER]]+4],
; CHECK: mov dword ptr [e[[REGISTER]]],
;
; OPTM1: store64:
; OPTM1: mov dword ptr [e[[REGISTER:[a-z]+]]+4],
; OPTM1: mov dword ptr [e[[REGISTER]]],

define internal void @store64Const(i32 %a) {
entry:
  %a.asptr = inttoptr i32 %a to i64*
  store i64 -2401053092306725256, i64* %a.asptr, align 1
  ret void
}
; CHECK: store64Const:
; CHECK: mov e[[REGISTER:[a-z]+]], dword ptr [esp+4]
; CHECK: mov dword ptr [e[[REGISTER]]+4], 3735928559
; CHECK: mov dword ptr [e[[REGISTER]]], 305419896
;
; OPTM1: store64Const:
; OPTM1: mov dword ptr [e[[REGISTER:[a-z]+]]+4], 3735928559
; OPTM1: mov dword ptr [e[[REGISTER]]], 305419896

define internal i64 @select64VarVar(i64 %a, i64 %b) {
entry:
  %cmp = icmp ult i64 %a, %b
  %cond = select i1 %cmp, i64 %a, i64 %b
  ret i64 %cond
}
; CHECK: select64VarVar:
; CHECK: cmp
; CHECK: jb
; CHECK: ja
; CHECK: cmp
; CHECK: jb
; CHECK: cmp
; CHECK: jne
;
; OPTM1: select64VarVar:
; OPTM1: cmp
; OPTM1: jb
; OPTM1: ja
; OPTM1: cmp
; OPTM1: jb
; OPTM1: cmp
; OPTM1: jne

define internal i64 @select64VarConst(i64 %a, i64 %b) {
entry:
  %cmp = icmp ult i64 %a, %b
  %cond = select i1 %cmp, i64 %a, i64 -2401053092306725256
  ret i64 %cond
}
; CHECK: select64VarConst:
; CHECK: cmp
; CHECK: jb
; CHECK: ja
; CHECK: cmp
; CHECK: jb
; CHECK: cmp
; CHECK: jne
;
; OPTM1: select64VarConst:
; OPTM1: cmp
; OPTM1: jb
; OPTM1: ja
; OPTM1: cmp
; OPTM1: jb
; OPTM1: cmp
; OPTM1: jne

define internal i64 @select64ConstVar(i64 %a, i64 %b) {
entry:
  %cmp = icmp ult i64 %a, %b
  %cond = select i1 %cmp, i64 -2401053092306725256, i64 %b
  ret i64 %cond
}
; CHECK: select64ConstVar:
; CHECK: cmp
; CHECK: jb
; CHECK: ja
; CHECK: cmp
; CHECK: jb
; CHECK: cmp
; CHECK: jne
;
; OPTM1: select64ConstVar:
; OPTM1: cmp
; OPTM1: jb
; OPTM1: ja
; OPTM1: cmp
; OPTM1: jb
; OPTM1: cmp
; OPTM1: jne

; ERRORS-NOT: ICE translation error
; DUMP-NOT: SZ
