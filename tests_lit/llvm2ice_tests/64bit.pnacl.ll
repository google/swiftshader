; This tries to be a comprehensive test of i64 operations, in
; particular the patterns for lowering i64 operations into constituent
; i32 operations on x86-32.

; TODO(jvoung): fix extra "CALLTARGETS" run. The llvm-objdump symbolizer
; doesn't know how to symbolize non-section-local functions.
; The newer LLVM 3.6 one does work, but watch out for other bugs.

; RUN: %p2i -i %s --args -O2 --verbose none \
; RUN:   | FileCheck --check-prefix=CALLTARGETS %s
; RUN: %p2i -i %s --args -O2 --verbose none \
; RUN:   | llvm-mc -triple=i686-none-nacl -x86-asm-syntax=intel -filetype=obj \
; RUN:   | llvm-objdump -d --symbolize -x86-asm-syntax=intel - | FileCheck %s
; RUN: %p2i -i %s --args -Om1 --verbose none \
; RUN:   | llvm-mc -triple=i686-none-nacl -x86-asm-syntax=intel -filetype=obj \
; RUN:   | llvm-objdump -d --symbolize -x86-asm-syntax=intel - \
; RUN:   | FileCheck --check-prefix=OPTM1 %s
; RUN: %p2i -i %s --args --verbose none | FileCheck --check-prefix=ERRORS %s

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
; CHECK-LABEL: pass64BitArg
; CALLTARGETS-LABEL: pass64BitArg
; CHECK:      sub     esp
; CHECK:      mov     dword ptr [esp + 4]
; CHECK:      mov     dword ptr [esp]
; CHECK:      mov     dword ptr [esp + 8], 123
; CHECK:      mov     dword ptr [esp + 16]
; CHECK:      mov     dword ptr [esp + 12]
; CHECK:      call    -4
; CALLTARGETS: call ignore64BitArgNoInline
; CHECK:      sub     esp
; CHECK:      mov     dword ptr [esp + 4]
; CHECK:      mov     dword ptr [esp]
; CHECK:      mov     dword ptr [esp + 8], 123
; CHECK:      mov     dword ptr [esp + 16]
; CHECK:      mov     dword ptr [esp + 12]
; CHECK:      call    -4
; CALLTARGETS: call ignore64BitArgNoInline
; CHECK:      sub     esp
; CHECK:      mov     dword ptr [esp + 4]
; CHECK:      mov     dword ptr [esp]
; CHECK:      mov     dword ptr [esp + 8], 123
; CHECK:      mov     dword ptr [esp + 16]
; CHECK:      mov     dword ptr [esp + 12]
; CHECK:      call    -4
; CALLTARGETS: call ignore64BitArgNoInline
;
; OPTM1-LABEL: pass64BitArg
; OPTM1:      sub     esp
; OPTM1:      mov     dword ptr [esp + 4]
; OPTM1:      mov     dword ptr [esp]
; OPTM1:      mov     dword ptr [esp + 8], 123
; OPTM1:      mov     dword ptr [esp + 16]
; OPTM1:      mov     dword ptr [esp + 12]
; OPTM1:      call    -4
; OPTM1:      sub     esp
; OPTM1:      mov     dword ptr [esp + 4]
; OPTM1:      mov     dword ptr [esp]
; OPTM1:      mov     dword ptr [esp + 8], 123
; OPTM1:      mov     dword ptr [esp + 16]
; OPTM1:      mov     dword ptr [esp + 12]
; OPTM1:      call    -4
; OPTM1:      sub     esp
; OPTM1:      mov     dword ptr [esp + 4]
; OPTM1:      mov     dword ptr [esp]
; OPTM1:      mov     dword ptr [esp + 8], 123
; OPTM1:      mov     dword ptr [esp + 16]
; OPTM1:      mov     dword ptr [esp + 12]
; OPTM1:      call    -4

declare i32 @ignore64BitArgNoInline(i64, i32, i64)

define internal i32 @pass64BitConstArg(i64 %a, i64 %b) {
entry:
  %call = call i32 @ignore64BitArgNoInline(i64 %a, i32 123, i64 -2401053092306725256)
  ret i32 %call
}
; CHECK-LABEL: pass64BitConstArg
; CALLTARGETS-LABEL: pass64BitConstArg
; CHECK:      sub     esp
; CHECK:      mov     dword ptr [esp + 4]
; CHECK-NEXT: mov     dword ptr [esp]
; CHECK-NEXT: mov     dword ptr [esp + 8], 123
; Bundle padding might be added (so not using -NEXT).
; CHECK:      mov     dword ptr [esp + 16], 3735928559
; CHECK-NEXT: mov     dword ptr [esp + 12], 305419896
; Bundle padding will push the call down.
; CHECK-NOT:  mov
; CHECK:      call    -4
; CALLTARGETS: call ignore64BitArgNoInline
;
; OPTM1-LABEL: pass64BitConstArg
; OPTM1:      sub     esp
; OPTM1:      mov     dword ptr [esp + 4]
; OPTM1-NEXT: mov     dword ptr [esp]
; OPTM1-NEXT: mov     dword ptr [esp + 8], 123
; Bundle padding might be added (so not using -NEXT).
; OPTM1:      mov     dword ptr [esp + 16], 3735928559
; OPTM1-NEXT: mov     dword ptr [esp + 12], 305419896
; OPTM1-NOT:  mov
; OPTM1:      call    -4

define internal i64 @return64BitArg(i64 %a) {
entry:
  ret i64 %a
}
; CHECK-LABEL: return64BitArg
; CHECK: mov     {{.*}}, dword ptr [esp + 4]
; CHECK: mov     {{.*}}, dword ptr [esp + 8]
;
; OPTM1-LABEL: return64BitArg
; OPTM1: mov     {{.*}}, dword ptr [esp + 4]
; OPTM1: mov     {{.*}}, dword ptr [esp + 8]

define internal i64 @return64BitConst() {
entry:
  ret i64 -2401053092306725256
}
; CHECK-LABEL: return64BitConst
; CHECK: mov     eax, 305419896
; CHECK: mov     edx, 3735928559
;
; OPTM1-LABEL: return64BitConst
; OPTM1: mov     eax, 305419896
; OPTM1: mov     edx, 3735928559

define internal i64 @add64BitSigned(i64 %a, i64 %b) {
entry:
  %add = add i64 %b, %a
  ret i64 %add
}
; CHECK-LABEL: add64BitSigned
; CHECK: add
; CHECK: adc
;
; OPTM1-LABEL: add64BitSigned
; OPTM1: add
; OPTM1: adc

define internal i64 @add64BitUnsigned(i64 %a, i64 %b) {
entry:
  %add = add i64 %b, %a
  ret i64 %add
}
; CHECK-LABEL: add64BitUnsigned
; CHECK: add
; CHECK: adc
;
; OPTM1-LABEL: add64BitUnsigned
; OPTM1: add
; OPTM1: adc

define internal i64 @sub64BitSigned(i64 %a, i64 %b) {
entry:
  %sub = sub i64 %a, %b
  ret i64 %sub
}
; CHECK-LABEL: sub64BitSigned
; CHECK: sub
; CHECK: sbb
;
; OPTM1-LABEL: sub64BitSigned
; OPTM1: sub
; OPTM1: sbb

define internal i64 @sub64BitUnsigned(i64 %a, i64 %b) {
entry:
  %sub = sub i64 %a, %b
  ret i64 %sub
}
; CHECK-LABEL: sub64BitUnsigned
; CHECK: sub
; CHECK: sbb
;
; OPTM1-LABEL: sub64BitUnsigned
; OPTM1: sub
; OPTM1: sbb

define internal i64 @mul64BitSigned(i64 %a, i64 %b) {
entry:
  %mul = mul i64 %b, %a
  ret i64 %mul
}
; CHECK-LABEL: mul64BitSigned
; CHECK: imul
; CHECK: imul
; CHECK: mul
; CHECK: add
; CHECK: add
;
; OPTM1-LABEL: mul64BitSigned
; OPTM1: imul
; OPTM1: imul
; OPTM1: mul
; OPTM1: add
; OPTM1: add

define internal i64 @mul64BitUnsigned(i64 %a, i64 %b) {
entry:
  %mul = mul i64 %b, %a
  ret i64 %mul
}
; CHECK-LABEL: mul64BitUnsigned
; CHECK: imul
; CHECK: imul
; CHECK: mul
; CHECK: add
; CHECK: add
;
; OPTM1-LABEL: mul64BitUnsigned
; OPTM1: imul
; OPTM1: imul
; OPTM1: mul
; OPTM1: add
; OPTM1: add

define internal i64 @div64BitSigned(i64 %a, i64 %b) {
entry:
  %div = sdiv i64 %a, %b
  ret i64 %div
}
; CHECK-LABEL: div64BitSigned
; CALLTARGETS-LABEL: div64BitSigned
; CHECK: call    -4
; CALLTARGETS: call __divdi3

; OPTM1-LABEL: div64BitSigned
; OPTM1: call    -4

define internal i64 @div64BitSignedConst(i64 %a) {
entry:
  %div = sdiv i64 %a, 12345678901234
  ret i64 %div
}
; CHECK-LABEL: div64BitSignedConst
; CALLTARGETS-LABEL: div64BitSignedConst
; CHECK: mov     dword ptr [esp + 12], 2874
; CHECK: mov     dword ptr [esp + 8],  1942892530
; CHECK: call    -4
; CALLTARGETS: call __divdi3
;
; OPTM1-LABEL: div64BitSignedConst
; OPTM1: mov     dword ptr [esp + 12], 2874
; OPTM1: mov     dword ptr [esp + 8],  1942892530
; OPTM1: call    -4

define internal i64 @div64BitUnsigned(i64 %a, i64 %b) {
entry:
  %div = udiv i64 %a, %b
  ret i64 %div
}
; CHECK-LABEL: div64BitUnsigned
; CALLTARGETS-LABEL: div64BitUnsigned
; CHECK: call    -4
; CALLTARGETS: call __udivdi3
;
; OPTM1-LABEL: div64BitUnsigned
; OPTM1: call    -4

define internal i64 @rem64BitSigned(i64 %a, i64 %b) {
entry:
  %rem = srem i64 %a, %b
  ret i64 %rem
}
; CHECK-LABEL: rem64BitSigned
; CALLTARGETS-LABEL: rem64BitSigned
; CHECK: call    -4
; CALLTARGETS: call __moddi3
;
; OPTM1-LABEL: rem64BitSigned
; OPTM1: call    -4

define internal i64 @rem64BitUnsigned(i64 %a, i64 %b) {
entry:
  %rem = urem i64 %a, %b
  ret i64 %rem
}
; CHECK-LABEL: rem64BitUnsigned
; CALLTARGETS-LABEL: rem64BitUnsigned
; CHECK: call    -4
; CALLTARGETS: call __umoddi3
;
; OPTM1-LABEL: rem64BitUnsigned
; OPTM1: call    -4

define internal i64 @shl64BitSigned(i64 %a, i64 %b) {
entry:
  %shl = shl i64 %a, %b
  ret i64 %shl
}
; CHECK-LABEL: shl64BitSigned
; CHECK: shld
; CHECK: shl e
; CHECK: test {{.*}}, 32
; CHECK: je
;
; OPTM1-LABEL: shl64BitSigned
; OPTM1: shld
; OPTM1: shl e
; OPTM1: test {{.*}}, 32
; OPTM1: je

define internal i32 @shl64BitSignedTrunc(i64 %a, i64 %b) {
entry:
  %shl = shl i64 %a, %b
  %result = trunc i64 %shl to i32
  ret i32 %result
}
; CHECK-LABEL: shl64BitSignedTrunc
; CHECK: mov
; CHECK: shl e
; CHECK: test {{.*}}, 32
; CHECK: je
;
; OPTM1-LABEL: shl64BitSignedTrunc
; OPTM1: shld
; OPTM1: shl e
; OPTM1: test {{.*}}, 32
; OPTM1: je

define internal i64 @shl64BitUnsigned(i64 %a, i64 %b) {
entry:
  %shl = shl i64 %a, %b
  ret i64 %shl
}
; CHECK-LABEL: shl64BitUnsigned
; CHECK: shld
; CHECK: shl e
; CHECK: test {{.*}}, 32
; CHECK: je
;
; OPTM1-LABEL: shl64BitUnsigned
; OPTM1: shld
; OPTM1: shl e
; OPTM1: test {{.*}}, 32
; OPTM1: je

define internal i64 @shr64BitSigned(i64 %a, i64 %b) {
entry:
  %shr = ashr i64 %a, %b
  ret i64 %shr
}
; CHECK-LABEL: shr64BitSigned
; CHECK: shrd
; CHECK: sar
; CHECK: test {{.*}}, 32
; CHECK: je
; CHECK: sar {{.*}}, 31
;
; OPTM1-LABEL: shr64BitSigned
; OPTM1: shrd
; OPTM1: sar
; OPTM1: test {{.*}}, 32
; OPTM1: je
; OPTM1: sar {{.*}}, 31

define internal i32 @shr64BitSignedTrunc(i64 %a, i64 %b) {
entry:
  %shr = ashr i64 %a, %b
  %result = trunc i64 %shr to i32
  ret i32 %result
}
; CHECK-LABEL: shr64BitSignedTrunc
; CHECK: shrd
; CHECK: sar
; CHECK: test {{.*}}, 32
; CHECK: je
;
; OPTM1-LABEL: shr64BitSignedTrunc
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
; CHECK-LABEL: shr64BitUnsigned
; CHECK: shrd
; CHECK: shr
; CHECK: test {{.*}}, 32
; CHECK: je
;
; OPTM1-LABEL: shr64BitUnsigned
; OPTM1: shrd
; OPTM1: shr
; OPTM1: test {{.*}}, 32
; OPTM1: je

define internal i32 @shr64BitUnsignedTrunc(i64 %a, i64 %b) {
entry:
  %shr = lshr i64 %a, %b
  %result = trunc i64 %shr to i32
  ret i32 %result
}
; CHECK-LABEL: shr64BitUnsignedTrunc
; CHECK: shrd
; CHECK: shr
; CHECK: test {{.*}}, 32
; CHECK: je
;
; OPTM1-LABEL: shr64BitUnsignedTrunc
; OPTM1: shrd
; OPTM1: shr
; OPTM1: test {{.*}}, 32
; OPTM1: je

define internal i64 @and64BitSigned(i64 %a, i64 %b) {
entry:
  %and = and i64 %b, %a
  ret i64 %and
}
; CHECK-LABEL: and64BitSigned
; CHECK: and
; CHECK: and
;
; OPTM1-LABEL: and64BitSigned
; OPTM1: and
; OPTM1: and

define internal i64 @and64BitUnsigned(i64 %a, i64 %b) {
entry:
  %and = and i64 %b, %a
  ret i64 %and
}
; CHECK-LABEL: and64BitUnsigned
; CHECK: and
; CHECK: and
;
; OPTM1-LABEL: and64BitUnsigned
; OPTM1: and
; OPTM1: and

define internal i64 @or64BitSigned(i64 %a, i64 %b) {
entry:
  %or = or i64 %b, %a
  ret i64 %or
}
; CHECK-LABEL: or64BitSigned
; CHECK: or
; CHECK: or
;
; OPTM1-LABEL: or64BitSigned
; OPTM1: or
; OPTM1: or

define internal i64 @or64BitUnsigned(i64 %a, i64 %b) {
entry:
  %or = or i64 %b, %a
  ret i64 %or
}
; CHECK-LABEL: or64BitUnsigned
; CHECK: or
; CHECK: or
;
; OPTM1-LABEL: or64BitUnsigned
; OPTM1: or
; OPTM1: or

define internal i64 @xor64BitSigned(i64 %a, i64 %b) {
entry:
  %xor = xor i64 %b, %a
  ret i64 %xor
}
; CHECK-LABEL: xor64BitSigned
; CHECK: xor
; CHECK: xor
;
; OPTM1-LABEL: xor64BitSigned
; OPTM1: xor
; OPTM1: xor

define internal i64 @xor64BitUnsigned(i64 %a, i64 %b) {
entry:
  %xor = xor i64 %b, %a
  ret i64 %xor
}
; CHECK-LABEL: xor64BitUnsigned
; CHECK: xor
; CHECK: xor
;
; OPTM1-LABEL: xor64BitUnsigned
; OPTM1: xor
; OPTM1: xor

define internal i32 @trunc64To32Signed(i64 %a) {
entry:
  %conv = trunc i64 %a to i32
  ret i32 %conv
}
; CHECK-LABEL: trunc64To32Signed
; CHECK: mov     eax, dword ptr [esp + 4]
;
; OPTM1-LABEL: trunc64To32Signed
; OPTM1: mov     eax, dword ptr [esp +

define internal i32 @trunc64To16Signed(i64 %a) {
entry:
  %conv = trunc i64 %a to i16
  %conv.ret_ext = sext i16 %conv to i32
  ret i32 %conv.ret_ext
}
; CHECK-LABEL: trunc64To16Signed
; CHECK:      mov     eax, dword ptr [esp + 4]
; CHECK-NEXT: movsx  eax, ax
;
; OPTM1-LABEL: trunc64To16Signed
; OPTM1:      mov     eax, dword ptr [esp +
; OPTM1: movsx  eax,

define internal i32 @trunc64To8Signed(i64 %a) {
entry:
  %conv = trunc i64 %a to i8
  %conv.ret_ext = sext i8 %conv to i32
  ret i32 %conv.ret_ext
}
; CHECK-LABEL: trunc64To8Signed
; CHECK:      mov     eax, dword ptr [esp + 4]
; CHECK-NEXT: movsx  eax, al
;
; OPTM1-LABEL: trunc64To8Signed
; OPTM1:      mov     eax, dword ptr [esp +
; OPTM1: movsx  eax,

define internal i32 @trunc64To32SignedConst() {
entry:
  %conv = trunc i64 12345678901234 to i32
  ret i32 %conv
}
; CHECK-LABEL: trunc64To32SignedConst
; CHECK: mov eax, 1942892530
;
; OPTM1-LABEL: trunc64To32SignedConst
; OPTM1: mov eax, 1942892530

define internal i32 @trunc64To16SignedConst() {
entry:
  %conv = trunc i64 12345678901234 to i16
  %conv.ret_ext = sext i16 %conv to i32
  ret i32 %conv.ret_ext
}
; CHECK-LABEL: trunc64To16SignedConst
; CHECK: mov eax, 1942892530
; CHECK: movsx eax, ax
;
; OPTM1-LABEL: trunc64To16SignedConst
; OPTM1: mov eax, 1942892530
; OPTM1: movsx eax,

define internal i32 @trunc64To32Unsigned(i64 %a) {
entry:
  %conv = trunc i64 %a to i32
  ret i32 %conv
}
; CHECK-LABEL: trunc64To32Unsigned
; CHECK: mov     eax, dword ptr [esp + 4]
;
; OPTM1-LABEL: trunc64To32Unsigned
; OPTM1: mov     eax, dword ptr [esp +

define internal i32 @trunc64To16Unsigned(i64 %a) {
entry:
  %conv = trunc i64 %a to i16
  %conv.ret_ext = zext i16 %conv to i32
  ret i32 %conv.ret_ext
}
; CHECK-LABEL: trunc64To16Unsigned
; CHECK:      mov     eax, dword ptr [esp + 4]
; CHECK-NEXT: movzx  eax, ax
;
; OPTM1-LABEL: trunc64To16Unsigned
; OPTM1:      mov     eax, dword ptr [esp +
; OPTM1: movzx  eax,

define internal i32 @trunc64To8Unsigned(i64 %a) {
entry:
  %conv = trunc i64 %a to i8
  %conv.ret_ext = zext i8 %conv to i32
  ret i32 %conv.ret_ext
}
; CHECK-LABEL: trunc64To8Unsigned
; CHECK:      mov     eax, dword ptr [esp + 4]
; CHECK-NEXT: movzx  eax, al
;
; OPTM1-LABEL: trunc64To8Unsigned
; OPTM1: mov    eax, dword ptr [esp +
; OPTM1: movzx  eax,

define internal i32 @trunc64To1(i64 %a) {
entry:
;  %tobool = icmp ne i64 %a, 0
  %tobool = trunc i64 %a to i1
  %tobool.ret_ext = zext i1 %tobool to i32
  ret i32 %tobool.ret_ext
}
; CHECK-LABEL: trunc64To1
; CHECK:      mov     eax, dword ptr [esp + 4]
; CHECK:      and     eax, 1
; CHECK:      and     eax, 1
;
; OPTM1-LABEL: trunc64To1
; OPTM1:      mov     eax, dword ptr [esp +
; OPTM1:      and     eax, 1
; OPTM1:      and     eax, 1

define internal i64 @sext32To64(i32 %a) {
entry:
  %conv = sext i32 %a to i64
  ret i64 %conv
}
; CHECK-LABEL: sext32To64
; CHECK: mov
; CHECK: sar {{.*}}, 31
;
; OPTM1-LABEL: sext32To64
; OPTM1: mov
; OPTM1: sar {{.*}}, 31

define internal i64 @sext16To64(i32 %a) {
entry:
  %a.arg_trunc = trunc i32 %a to i16
  %conv = sext i16 %a.arg_trunc to i64
  ret i64 %conv
}
; CHECK-LABEL: sext16To64
; CHECK: movsx
; CHECK: sar {{.*}}, 31
;
; OPTM1-LABEL: sext16To64
; OPTM1: movsx
; OPTM1: sar {{.*}}, 31

define internal i64 @sext8To64(i32 %a) {
entry:
  %a.arg_trunc = trunc i32 %a to i8
  %conv = sext i8 %a.arg_trunc to i64
  ret i64 %conv
}
; CHECK-LABEL: sext8To64
; CHECK: movsx
; CHECK: sar {{.*}}, 31
;
; OPTM1-LABEL: sext8To64
; OPTM1: movsx
; OPTM1: sar {{.*}}, 31

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
;
; OPTM1-LABEL: sext1To64
; OPTM1: mov
; OPTM1: shl {{.*}}, 31
; OPTM1: sar {{.*}}, 31

define internal i64 @zext32To64(i32 %a) {
entry:
  %conv = zext i32 %a to i64
  ret i64 %conv
}
; CHECK-LABEL: zext32To64
; CHECK: mov
; CHECK: mov {{.*}}, 0
;
; OPTM1-LABEL: zext32To64
; OPTM1: mov
; OPTM1: mov {{.*}}, 0

define internal i64 @zext16To64(i32 %a) {
entry:
  %a.arg_trunc = trunc i32 %a to i16
  %conv = zext i16 %a.arg_trunc to i64
  ret i64 %conv
}
; CHECK-LABEL: zext16To64
; CHECK: movzx
; CHECK: mov {{.*}}, 0
;
; OPTM1-LABEL: zext16To64
; OPTM1: movzx
; OPTM1: mov {{.*}}, 0

define internal i64 @zext8To64(i32 %a) {
entry:
  %a.arg_trunc = trunc i32 %a to i8
  %conv = zext i8 %a.arg_trunc to i64
  ret i64 %conv
}
; CHECK-LABEL: zext8To64
; CHECK: movzx
; CHECK: mov {{.*}}, 0
;
; OPTM1-LABEL: zext8To64
; OPTM1: movzx
; OPTM1: mov {{.*}}, 0

define internal i64 @zext1To64(i32 %a) {
entry:
  %a.arg_trunc = trunc i32 %a to i1
  %conv = zext i1 %a.arg_trunc to i64
  ret i64 %conv
}
; CHECK-LABEL: zext1To64
; CHECK: and {{.*}}, 1
; CHECK: mov {{.*}}, 0
;
; OPTM1-LABEL: zext1To64
; OPTM1: and {{.*}}, 1
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
; CHECK-LABEL: icmpEq64
; CHECK: jne
; CHECK: jne
; CHECK: call
; CHECK: jne
; CHECK: jne
; CHECK: call
;
; OPTM1-LABEL: icmpEq64
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
; CHECK-LABEL: icmpNe64
; CHECK: jne
; CHECK: jne
; CHECK: call
; CHECK: jne
; CHECK: jne
; CHECK: call
;
; OPTM1-LABEL: icmpNe64
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
; CHECK-LABEL: icmpGt64
; CHECK: ja
; CHECK: jb
; CHECK: ja
; CHECK: call
; CHECK: jg
; CHECK: jl
; CHECK: ja
; CHECK: call
;
; OPTM1-LABEL: icmpGt64
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
; CHECK-LABEL: icmpGe64
; CHECK: ja
; CHECK: jb
; CHECK: jae
; CHECK: call
; CHECK: jg
; CHECK: jl
; CHECK: jae
; CHECK: call
;
; OPTM1-LABEL: icmpGe64
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
; CHECK-LABEL: icmpLt64
; CHECK: jb
; CHECK: ja
; CHECK: jb
; CHECK: call
; CHECK: jl
; CHECK: jg
; CHECK: jb
; CHECK: call
;
; OPTM1-LABEL: icmpLt64
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
; CHECK-LABEL: icmpLe64
; CHECK: jb
; CHECK: ja
; CHECK: jbe
; CHECK: call
; CHECK: jl
; CHECK: jg
; CHECK: jbe
; CHECK: call
;
; OPTM1-LABEL: icmpLe64
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
; CHECK-LABEL: icmpEq64Bool
; CHECK: jne
; CHECK: jne
;
; OPTM1-LABEL: icmpEq64Bool
; OPTM1: jne
; OPTM1: jne

define internal i32 @icmpNe64Bool(i64 %a, i64 %b) {
entry:
  %cmp = icmp ne i64 %a, %b
  %cmp.ret_ext = zext i1 %cmp to i32
  ret i32 %cmp.ret_ext
}
; CHECK-LABEL: icmpNe64Bool
; CHECK: jne
; CHECK: jne
;
; OPTM1-LABEL: icmpNe64Bool
; OPTM1: jne
; OPTM1: jne

define internal i32 @icmpSgt64Bool(i64 %a, i64 %b) {
entry:
  %cmp = icmp sgt i64 %a, %b
  %cmp.ret_ext = zext i1 %cmp to i32
  ret i32 %cmp.ret_ext
}
; CHECK-LABEL: icmpSgt64Bool
; CHECK: cmp
; CHECK: jg
; CHECK: jl
; CHECK: cmp
; CHECK: ja
;
; OPTM1-LABEL: icmpSgt64Bool
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
; CHECK-LABEL: icmpUgt64Bool
; CHECK: cmp
; CHECK: ja
; CHECK: jb
; CHECK: cmp
; CHECK: ja
;
; OPTM1-LABEL: icmpUgt64Bool
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
; CHECK-LABEL: icmpSge64Bool
; CHECK: cmp
; CHECK: jg
; CHECK: jl
; CHECK: cmp
; CHECK: jae
;
; OPTM1-LABEL: icmpSge64Bool
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
; CHECK-LABEL: icmpUge64Bool
; CHECK: cmp
; CHECK: ja
; CHECK: jb
; CHECK: cmp
; CHECK: jae
;
; OPTM1-LABEL: icmpUge64Bool
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
; CHECK-LABEL: icmpSlt64Bool
; CHECK: cmp
; CHECK: jl
; CHECK: jg
; CHECK: cmp
; CHECK: jb
;
; OPTM1-LABEL: icmpSlt64Bool
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
; CHECK-LABEL: icmpUlt64Bool
; CHECK: cmp
; CHECK: jb
; CHECK: ja
; CHECK: cmp
; CHECK: jb
;
; OPTM1-LABEL: icmpUlt64Bool
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
; CHECK-LABEL: icmpSle64Bool
; CHECK: cmp
; CHECK: jl
; CHECK: jg
; CHECK: cmp
; CHECK: jbe
;
; OPTM1-LABEL: icmpSle64Bool
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
; CHECK-LABEL: icmpUle64Bool
; CHECK: cmp
; CHECK: jb
; CHECK: ja
; CHECK: cmp
; CHECK: jbe
;
; OPTM1-LABEL: icmpUle64Bool
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
; CHECK-LABEL: load64
; CHECK: mov e[[REGISTER:[a-z]+]], dword ptr [esp + 4]
; CHECK-NEXT: mov {{.*}}, dword ptr [e[[REGISTER]]]
; CHECK-NEXT: mov {{.*}}, dword ptr [e[[REGISTER]] + 4]
;
; OPTM1-LABEL: load64
; OPTM1: mov e{{..}}, dword ptr [e{{..}}]
; OPTM1: mov e{{..}}, dword ptr [e{{..}} + 4]

define internal void @store64(i32 %a, i64 %value) {
entry:
  %__2 = inttoptr i32 %a to i64*
  store i64 %value, i64* %__2, align 1
  ret void
}
; CHECK-LABEL: store64
; CHECK: mov e[[REGISTER:[a-z]+]], dword ptr [esp + 4]
; CHECK: mov dword ptr [e[[REGISTER]] + 4],
; CHECK: mov dword ptr [e[[REGISTER]]],
;
; OPTM1-LABEL: store64
; OPTM1: mov dword ptr [e[[REGISTER:[a-z]+]] + 4],
; OPTM1: mov dword ptr [e[[REGISTER]]],

define internal void @store64Const(i32 %a) {
entry:
  %__1 = inttoptr i32 %a to i64*
  store i64 -2401053092306725256, i64* %__1, align 1
  ret void
}
; CHECK-LABEL: store64Const
; CHECK: mov e[[REGISTER:[a-z]+]], dword ptr [esp + 4]
; CHECK: mov dword ptr [e[[REGISTER]] + 4], 3735928559
; CHECK: mov dword ptr [e[[REGISTER]]], 305419896
;
; OPTM1-LABEL: store64Const
; OPTM1: mov dword ptr [e[[REGISTER:[a-z]+]] + 4], 3735928559
; OPTM1: mov dword ptr [e[[REGISTER]]], 305419896

define internal i64 @select64VarVar(i64 %a, i64 %b) {
entry:
  %cmp = icmp ult i64 %a, %b
  %cond = select i1 %cmp, i64 %a, i64 %b
  ret i64 %cond
}
; CHECK-LABEL: select64VarVar
; CHECK: cmp
; CHECK: jb
; CHECK: ja
; CHECK: cmp
; CHECK: jb
; CHECK: cmp
; CHECK: jne
;
; OPTM1-LABEL: select64VarVar
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
; CHECK-LABEL: select64VarConst
; CHECK: cmp
; CHECK: jb
; CHECK: ja
; CHECK: cmp
; CHECK: jb
; CHECK: cmp
; CHECK: jne
;
; OPTM1-LABEL: select64VarConst
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
; CHECK-LABEL: select64ConstVar
; CHECK: cmp
; CHECK: jb
; CHECK: ja
; CHECK: cmp
; CHECK: jb
; CHECK: cmp
; CHECK: jne
;
; OPTM1-LABEL: select64ConstVar
; OPTM1: cmp
; OPTM1: jb
; OPTM1: ja
; OPTM1: cmp
; OPTM1: jb
; OPTM1: cmp
; OPTM1: jne

define internal void @icmpEq64Imm() {
entry:
  %cmp = icmp eq i64 123, 234
  br i1 %cmp, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  call void @func()
  br label %if.end

if.end:                                           ; preds = %if.then, %entry
  %cmp1 = icmp eq i64 345, 456
  br i1 %cmp1, label %if.then2, label %if.end3

if.then2:                                         ; preds = %if.end
  call void @func()
  br label %if.end3

if.end3:                                          ; preds = %if.then2, %if.end
  ret void
}
; The following checks are not strictly necessary since one of the RUN
; lines actually runs the output through the assembler.
; CHECK-LABEL: icmpEq64Imm
; CHECK-NOT: cmp {{[0-9]+}},
; OPTM1-LABEL: icmpEq64Imm
; OPTM1-LABEL-NOT: cmp {{[0-9]+}},

define internal void @icmpLt64Imm() {
entry:
  %cmp = icmp ult i64 123, 234
  br i1 %cmp, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  call void @func()
  br label %if.end

if.end:                                           ; preds = %if.then, %entry
  %cmp1 = icmp slt i64 345, 456
  br i1 %cmp1, label %if.then2, label %if.end3

if.then2:                                         ; preds = %if.end
  call void @func()
  br label %if.end3

if.end3:                                          ; preds = %if.then2, %if.end
  ret void
}
; The following checks are not strictly necessary since one of the RUN
; lines actually runs the output through the assembler.
; CHECK-LABEL: icmpLt64Imm
; CHECK-NOT: cmp {{[0-9]+}},
; OPTM1-LABEL: icmpLt64Imm
; OPTM1-NOT: cmp {{[0-9]+}},

; ERRORS-NOT: ICE translation error
