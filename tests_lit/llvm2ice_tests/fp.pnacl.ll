; RUIN: %llvm2ice --verbose none %s | FileCheck %s
; RUIN: %llvm2ice --verbose none %s | FileCheck --check-prefix=ERRORS %s
; RUN: %szdiff --llvm2ice=%llvm2ice %s | FileCheck --check-prefix=DUMP %s

@__init_array_start = internal constant [0 x i8] zeroinitializer, align 4
@__fini_array_start = internal constant [0 x i8] zeroinitializer, align 4
@__tls_template_start = internal constant [0 x i8] zeroinitializer, align 8
@__tls_template_alignment = internal constant [4 x i8] c"\01\00\00\00", align 4

define internal i32 @doubleArgs(double %a, i32 %b, double %c) {
entry:
  ret i32 %b
}
; CHECK: doubleArgs:
; CHECK:      mov     eax, dword ptr [esp+12]
; CHECK-NEXT: ret

define internal i32 @floatArgs(float %a, i32 %b, float %c) {
entry:
  ret i32 %b
}
; CHECK: floatArgs:
; CHECK:      mov     eax, dword ptr [esp+8]
; CHECK-NEXT: ret

define internal i32 @passFpArgs(float %a, double %b, float %c, double %d, float %e, double %f) {
entry:
  %call = call i32 @ignoreFpArgsNoInline(float %a, i32 123, double %b)
  %call1 = call i32 @ignoreFpArgsNoInline(float %c, i32 123, double %d)
  %call2 = call i32 @ignoreFpArgsNoInline(float %e, i32 123, double %f)
  %add = add i32 %call1, %call
  %add3 = add i32 %add, %call2
  ret i32 %add3
}
; CHECK: passFpArgs:
; CHECK: push 123
; CHECK: call ignoreFpArgsNoInline
; CHECK: push 123
; CHECK: call ignoreFpArgsNoInline
; CHECK: push 123
; CHECK: call ignoreFpArgsNoInline

declare i32 @ignoreFpArgsNoInline(float, i32, double)

define internal i32 @passFpConstArg(float %a, double %b) {
entry:
  %call = call i32 @ignoreFpArgsNoInline(float %a, i32 123, double 2.340000e+00)
  ret i32 %call
}
; CHECK: passFpConstArg:
; CHECK: push 123
; CHECK: call ignoreFpArgsNoInline

define internal float @returnFloatArg(float %a) {
entry:
  ret float %a
}
; CHECK: returnFloatArg:
; CHECK: fld dword ptr [esp

define internal double @returnDoubleArg(double %a) {
entry:
  ret double %a
}
; CHECK: returnDoubleArg:
; CHECK: fld qword ptr [esp

define internal float @returnFloatConst() {
entry:
  ret float 0x3FF3AE1480000000
}
; CHECK: returnFloatConst:
; CHECK: fld

define internal double @returnDoubleConst() {
entry:
  ret double 1.230000e+00
}
; CHECK: returnDoubleConst:
; CHECK: fld

define internal float @addFloat(float %a, float %b) {
entry:
  %add = fadd float %a, %b
  ret float %add
}
; CHECK: addFloat:
; CHECK: addss
; CHECK: fld

define internal double @addDouble(double %a, double %b) {
entry:
  %add = fadd double %a, %b
  ret double %add
}
; CHECK: addDouble:
; CHECK: addsd
; CHECK: fld

define internal float @subFloat(float %a, float %b) {
entry:
  %sub = fsub float %a, %b
  ret float %sub
}
; CHECK: subFloat:
; CHECK: subss
; CHECK: fld

define internal double @subDouble(double %a, double %b) {
entry:
  %sub = fsub double %a, %b
  ret double %sub
}
; CHECK: subDouble:
; CHECK: subsd
; CHECK: fld

define internal float @mulFloat(float %a, float %b) {
entry:
  %mul = fmul float %a, %b
  ret float %mul
}
; CHECK: mulFloat:
; CHECK: mulss
; CHECK: fld

define internal double @mulDouble(double %a, double %b) {
entry:
  %mul = fmul double %a, %b
  ret double %mul
}
; CHECK: mulDouble:
; CHECK: mulsd
; CHECK: fld

define internal float @divFloat(float %a, float %b) {
entry:
  %div = fdiv float %a, %b
  ret float %div
}
; CHECK: divFloat:
; CHECK: divss
; CHECK: fld

define internal double @divDouble(double %a, double %b) {
entry:
  %div = fdiv double %a, %b
  ret double %div
}
; CHECK: divDouble:
; CHECK: divsd
; CHECK: fld

define internal float @remFloat(float %a, float %b) {
entry:
  %div = frem float %a, %b
  ret float %div
}
; CHECK: remFloat:
; CHECK: call fmodf

define internal double @remDouble(double %a, double %b) {
entry:
  %div = frem double %a, %b
  ret double %div
}
; CHECK: remDouble:
; CHECK: call fmod

define internal float @fptrunc(double %a) {
entry:
  %conv = fptrunc double %a to float
  ret float %conv
}
; CHECK: fptrunc:
; CHECK: cvtsd2ss
; CHECK: fld

define internal double @fpext(float %a) {
entry:
  %conv = fpext float %a to double
  ret double %conv
}
; CHECK: fpext:
; CHECK: cvtss2sd
; CHECK: fld

define internal i64 @doubleToSigned64(double %a) {
entry:
  %conv = fptosi double %a to i64
  ret i64 %conv
}
; CHECK: doubleToSigned64:
; CHECK: call cvtdtosi64

define internal i64 @floatToSigned64(float %a) {
entry:
  %conv = fptosi float %a to i64
  ret i64 %conv
}
; CHECK: floatToSigned64:
; CHECK: call cvtftosi64

define internal i64 @doubleToUnsigned64(double %a) {
entry:
  %conv = fptoui double %a to i64
  ret i64 %conv
}
; CHECK: doubleToUnsigned64:
; CHECK: call cvtdtoui64

define internal i64 @floatToUnsigned64(float %a) {
entry:
  %conv = fptoui float %a to i64
  ret i64 %conv
}
; CHECK: floatToUnsigned64:
; CHECK: call cvtftoui64

define internal i32 @doubleToSigned32(double %a) {
entry:
  %conv = fptosi double %a to i32
  ret i32 %conv
}
; CHECK: doubleToSigned32:
; CHECK: cvtsd2si

define internal i32 @floatToSigned32(float %a) {
entry:
  %conv = fptosi float %a to i32
  ret i32 %conv
}
; CHECK: floatToSigned32:
; CHECK: cvtss2si

define internal i32 @doubleToUnsigned32(double %a) {
entry:
  %conv = fptoui double %a to i32
  ret i32 %conv
}
; CHECK: doubleToUnsigned32:
; CHECK: call cvtdtoui32

define internal i32 @floatToUnsigned32(float %a) {
entry:
  %conv = fptoui float %a to i32
  ret i32 %conv
}
; CHECK: floatToUnsigned32:
; CHECK: call cvtftoui32

define internal i32 @doubleToSigned16(double %a) {
entry:
  %conv = fptosi double %a to i16
  %conv.ret_ext = sext i16 %conv to i32
  ret i32 %conv.ret_ext
}
; CHECK: doubleToSigned16:
; CHECK: cvtsd2si
; CHECK: movsx

define internal i32 @floatToSigned16(float %a) {
entry:
  %conv = fptosi float %a to i16
  %conv.ret_ext = sext i16 %conv to i32
  ret i32 %conv.ret_ext
}
; CHECK: floatToSigned16:
; CHECK: cvtss2si
; CHECK: movsx

define internal i32 @doubleToUnsigned16(double %a) {
entry:
  %conv = fptoui double %a to i16
  %conv.ret_ext = zext i16 %conv to i32
  ret i32 %conv.ret_ext
}
; CHECK: doubleToUnsigned16:
; CHECK: cvtsd2si
; CHECK: movzx

define internal i32 @floatToUnsigned16(float %a) {
entry:
  %conv = fptoui float %a to i16
  %conv.ret_ext = zext i16 %conv to i32
  ret i32 %conv.ret_ext
}
; CHECK: floatToUnsigned16:
; CHECK: cvtss2si
; CHECK: movzx

define internal i32 @doubleToSigned8(double %a) {
entry:
  %conv = fptosi double %a to i8
  %conv.ret_ext = sext i8 %conv to i32
  ret i32 %conv.ret_ext
}
; CHECK: doubleToSigned8:
; CHECK: cvtsd2si
; CHECK: movsx

define internal i32 @floatToSigned8(float %a) {
entry:
  %conv = fptosi float %a to i8
  %conv.ret_ext = sext i8 %conv to i32
  ret i32 %conv.ret_ext
}
; CHECK: floatToSigned8:
; CHECK: cvtss2si
; CHECK: movsx

define internal i32 @doubleToUnsigned8(double %a) {
entry:
  %conv = fptoui double %a to i8
  %conv.ret_ext = zext i8 %conv to i32
  ret i32 %conv.ret_ext
}
; CHECK: doubleToUnsigned8:
; CHECK: cvtsd2si
; CHECK: movzx

define internal i32 @floatToUnsigned8(float %a) {
entry:
  %conv = fptoui float %a to i8
  %conv.ret_ext = zext i8 %conv to i32
  ret i32 %conv.ret_ext
}
; CHECK: floatToUnsigned8:
; CHECK: cvtss2si
; CHECK: movzx

define internal i32 @doubleToUnsigned1(double %a) {
entry:
  %tobool = fptoui double %a to i1
  %tobool.ret_ext = zext i1 %tobool to i32
  ret i32 %tobool.ret_ext
}
; CHECK: doubleToUnsigned1:
; CHECK: cvtsd2si
; CHECK: and eax, 1

define internal i32 @floatToUnsigned1(float %a) {
entry:
  %tobool = fptoui float %a to i1
  %tobool.ret_ext = zext i1 %tobool to i32
  ret i32 %tobool.ret_ext
}
; CHECK: floatToUnsigned1:
; CHECK: cvtss2si
; CHECK: and eax, 1

define internal double @signed64ToDouble(i64 %a) {
entry:
  %conv = sitofp i64 %a to double
  ret double %conv
}
; CHECK: signed64ToDouble:
; CHECK: call cvtsi64tod
; CHECK: fstp

define internal float @signed64ToFloat(i64 %a) {
entry:
  %conv = sitofp i64 %a to float
  ret float %conv
}
; CHECK: signed64ToFloat:
; CHECK: call cvtsi64tof
; CHECK: fstp

define internal double @unsigned64ToDouble(i64 %a) {
entry:
  %conv = uitofp i64 %a to double
  ret double %conv
}
; CHECK: unsigned64ToDouble:
; CHECK: call cvtui64tod
; CHECK: fstp

define internal float @unsigned64ToFloat(i64 %a) {
entry:
  %conv = uitofp i64 %a to float
  ret float %conv
}
; CHECK: unsigned64ToFloat:
; CHECK: call cvtui64tof
; CHECK: fstp

define internal double @signed32ToDouble(i32 %a) {
entry:
  %conv = sitofp i32 %a to double
  ret double %conv
}
; CHECK: signed32ToDouble:
; CHECK: cvtsi2sd
; CHECK: fld

define internal float @signed32ToFloat(i32 %a) {
entry:
  %conv = sitofp i32 %a to float
  ret float %conv
}
; CHECK: signed32ToFloat:
; CHECK: cvtsi2ss
; CHECK: fld

define internal double @unsigned32ToDouble(i32 %a) {
entry:
  %conv = uitofp i32 %a to double
  ret double %conv
}
; CHECK: unsigned32ToDouble:
; CHECK: call cvtui32tod
; CHECK: fstp

define internal float @unsigned32ToFloat(i32 %a) {
entry:
  %conv = uitofp i32 %a to float
  ret float %conv
}
; CHECK: unsigned32ToFloat:
; CHECK: call cvtui32tof
; CHECK: fstp

define internal double @signed16ToDouble(i32 %a) {
entry:
  %a.arg_trunc = trunc i32 %a to i16
  %conv = sitofp i16 %a.arg_trunc to double
  ret double %conv
}
; CHECK: signed16ToDouble:
; CHECK: cvtsi2sd
; CHECK: fld

define internal float @signed16ToFloat(i32 %a) {
entry:
  %a.arg_trunc = trunc i32 %a to i16
  %conv = sitofp i16 %a.arg_trunc to float
  ret float %conv
}
; CHECK: signed16ToFloat:
; CHECK: cvtsi2ss
; CHECK: fld

define internal double @unsigned16ToDouble(i32 %a) {
entry:
  %a.arg_trunc = trunc i32 %a to i16
  %conv = uitofp i16 %a.arg_trunc to double
  ret double %conv
}
; CHECK: unsigned16ToDouble:
; CHECK: cvtsi2sd
; CHECK: fld

define internal float @unsigned16ToFloat(i32 %a) {
entry:
  %a.arg_trunc = trunc i32 %a to i16
  %conv = uitofp i16 %a.arg_trunc to float
  ret float %conv
}
; CHECK: unsigned16ToFloat:
; CHECK: cvtsi2ss
; CHECK: fld

define internal double @signed8ToDouble(i32 %a) {
entry:
  %a.arg_trunc = trunc i32 %a to i8
  %conv = sitofp i8 %a.arg_trunc to double
  ret double %conv
}
; CHECK: signed8ToDouble:
; CHECK: cvtsi2sd
; CHECK: fld

define internal float @signed8ToFloat(i32 %a) {
entry:
  %a.arg_trunc = trunc i32 %a to i8
  %conv = sitofp i8 %a.arg_trunc to float
  ret float %conv
}
; CHECK: signed8ToFloat:
; CHECK: cvtsi2ss
; CHECK: fld

define internal double @unsigned8ToDouble(i32 %a) {
entry:
  %a.arg_trunc = trunc i32 %a to i8
  %conv = uitofp i8 %a.arg_trunc to double
  ret double %conv
}
; CHECK: unsigned8ToDouble:
; CHECK: cvtsi2sd
; CHECK: fld

define internal float @unsigned8ToFloat(i32 %a) {
entry:
  %a.arg_trunc = trunc i32 %a to i8
  %conv = uitofp i8 %a.arg_trunc to float
  ret float %conv
}
; CHECK: unsigned8ToFloat:
; CHECK: cvtsi2ss
; CHECK: fld

define internal double @unsigned1ToDouble(i32 %a) {
entry:
  %a.arg_trunc = trunc i32 %a to i1
  %conv = uitofp i1 %a.arg_trunc to double
  ret double %conv
}
; CHECK: unsigned1ToDouble:
; CHECK: cvtsi2sd
; CHECK: fld

define internal float @unsigned1ToFloat(i32 %a) {
entry:
  %a.arg_trunc = trunc i32 %a to i1
  %conv = uitofp i1 %a.arg_trunc to float
  ret float %conv
}
; CHECK: unsigned1ToFloat:
; CHECK: cvtsi2ss
; CHECK: fld

define internal void @fcmpEq(float %a, float %b, double %c, double %d) {
entry:
  %cmp = fcmp oeq float %a, %b
  br i1 %cmp, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  call void @func()
  br label %if.end

if.end:                                           ; preds = %if.then, %entry
  %cmp1 = fcmp oeq double %c, %d
  br i1 %cmp1, label %if.then2, label %if.end3

if.then2:                                         ; preds = %if.end
  call void @func()
  br label %if.end3

if.end3:                                          ; preds = %if.then2, %if.end
  ret void
}
; CHECK: fcmpEq:
; CHECK: ucomiss
; CHECK: jne .
; CHECK-NEXT: jp .
; CHECK: call func
; CHECK: ucomisd
; CHECK: jne .
; CHECK-NEXT: jp .
; CHECK: call func

declare void @func()

define internal void @fcmpNe(float %a, float %b, double %c, double %d) {
entry:
  %cmp = fcmp une float %a, %b
  br i1 %cmp, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  call void @func()
  br label %if.end

if.end:                                           ; preds = %if.then, %entry
  %cmp1 = fcmp une double %c, %d
  br i1 %cmp1, label %if.then2, label %if.end3

if.then2:                                         ; preds = %if.end
  call void @func()
  br label %if.end3

if.end3:                                          ; preds = %if.then2, %if.end
  ret void
}
; CHECK: fcmpNe:
; CHECK: ucomiss
; CHECK: jne .
; CHECK-NEXT: jp .
; CHECK: call func
; CHECK: ucomisd
; CHECK: jne .
; CHECK-NEXT: jp .
; CHECK: call func

define internal void @fcmpGt(float %a, float %b, double %c, double %d) {
entry:
  %cmp = fcmp ogt float %a, %b
  br i1 %cmp, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  call void @func()
  br label %if.end

if.end:                                           ; preds = %if.then, %entry
  %cmp1 = fcmp ogt double %c, %d
  br i1 %cmp1, label %if.then2, label %if.end3

if.then2:                                         ; preds = %if.end
  call void @func()
  br label %if.end3

if.end3:                                          ; preds = %if.then2, %if.end
  ret void
}
; CHECK: fcmpGt:
; CHECK: ucomiss
; CHECK: ja .
; CHECK: call func
; CHECK: ucomisd
; CHECK: ja .
; CHECK: call func

define internal void @fcmpGe(float %a, float %b, double %c, double %d) {
entry:
  %cmp = fcmp ult float %a, %b
  br i1 %cmp, label %if.end, label %if.then

if.then:                                          ; preds = %entry
  call void @func()
  br label %if.end

if.end:                                           ; preds = %entry, %if.then
  %cmp1 = fcmp ult double %c, %d
  br i1 %cmp1, label %if.end3, label %if.then2

if.then2:                                         ; preds = %if.end
  call void @func()
  br label %if.end3

if.end3:                                          ; preds = %if.end, %if.then2
  ret void
}
; CHECK: fcmpGe:
; CHECK: ucomiss
; CHECK: jb .
; CHECK: call func
; CHECK: ucomisd
; CHECK: jb .
; CHECK: call func

define internal void @fcmpLt(float %a, float %b, double %c, double %d) {
entry:
  %cmp = fcmp olt float %a, %b
  br i1 %cmp, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  call void @func()
  br label %if.end

if.end:                                           ; preds = %if.then, %entry
  %cmp1 = fcmp olt double %c, %d
  br i1 %cmp1, label %if.then2, label %if.end3

if.then2:                                         ; preds = %if.end
  call void @func()
  br label %if.end3

if.end3:                                          ; preds = %if.then2, %if.end
  ret void
}
; CHECK: fcmpLt:
; CHECK: ucomiss
; CHECK: ja .
; CHECK: call func
; CHECK: ucomisd
; CHECK: ja .
; CHECK: call func

define internal void @fcmpLe(float %a, float %b, double %c, double %d) {
entry:
  %cmp = fcmp ugt float %a, %b
  br i1 %cmp, label %if.end, label %if.then

if.then:                                          ; preds = %entry
  call void @func()
  br label %if.end

if.end:                                           ; preds = %entry, %if.then
  %cmp1 = fcmp ugt double %c, %d
  br i1 %cmp1, label %if.end3, label %if.then2

if.then2:                                         ; preds = %if.end
  call void @func()
  br label %if.end3

if.end3:                                          ; preds = %if.end, %if.then2
  ret void
}
; CHECK: fcmpLe:
; CHECK: ucomiss
; CHECK: jb .
; CHECK: call func
; CHECK: ucomisd
; CHECK: jb .
; CHECK: call func

define internal i32 @fcmpFalseFloat(float %a, float %b) {
entry:
  %cmp = fcmp false float %a, %b
  %cmp.ret_ext = zext i1 %cmp to i32
  ret i32 %cmp.ret_ext
}
; CHECK: fcmpFalseFloat:
; CHECK: mov {{.*}}, 0

define internal i32 @fcmpFalseDouble(double %a, double %b) {
entry:
  %cmp = fcmp false double %a, %b
  %cmp.ret_ext = zext i1 %cmp to i32
  ret i32 %cmp.ret_ext
}
; CHECK: fcmpFalseDouble:
; CHECK: mov {{.*}}, 0

define internal i32 @fcmpOeqFloat(float %a, float %b) {
entry:
  %cmp = fcmp oeq float %a, %b
  %cmp.ret_ext = zext i1 %cmp to i32
  ret i32 %cmp.ret_ext
}
; CHECK: fcmpOeqFloat:
; CHECK: ucomiss
; CHECK: jne .
; CHECK: jp .

define internal i32 @fcmpOeqDouble(double %a, double %b) {
entry:
  %cmp = fcmp oeq double %a, %b
  %cmp.ret_ext = zext i1 %cmp to i32
  ret i32 %cmp.ret_ext
}
; CHECK: fcmpOeqDouble:
; CHECK: ucomisd
; CHECK: jne .
; CHECK: jp .

define internal i32 @fcmpOgtFloat(float %a, float %b) {
entry:
  %cmp = fcmp ogt float %a, %b
  %cmp.ret_ext = zext i1 %cmp to i32
  ret i32 %cmp.ret_ext
}
; CHECK: fcmpOgtFloat:
; CHECK: ucomiss
; CHECK: ja .

define internal i32 @fcmpOgtDouble(double %a, double %b) {
entry:
  %cmp = fcmp ogt double %a, %b
  %cmp.ret_ext = zext i1 %cmp to i32
  ret i32 %cmp.ret_ext
}
; CHECK: fcmpOgtDouble:
; CHECK: ucomisd
; CHECK: ja .

define internal i32 @fcmpOgeFloat(float %a, float %b) {
entry:
  %cmp = fcmp oge float %a, %b
  %cmp.ret_ext = zext i1 %cmp to i32
  ret i32 %cmp.ret_ext
}
; CHECK: fcmpOgeFloat:
; CHECK: ucomiss
; CHECK: jae .

define internal i32 @fcmpOgeDouble(double %a, double %b) {
entry:
  %cmp = fcmp oge double %a, %b
  %cmp.ret_ext = zext i1 %cmp to i32
  ret i32 %cmp.ret_ext
}
; CHECK: fcmpOgeDouble:
; CHECK: ucomisd
; CHECK: jae .

define internal i32 @fcmpOltFloat(float %a, float %b) {
entry:
  %cmp = fcmp olt float %a, %b
  %cmp.ret_ext = zext i1 %cmp to i32
  ret i32 %cmp.ret_ext
}
; CHECK: fcmpOltFloat:
; CHECK: ucomiss
; CHECK: ja .

define internal i32 @fcmpOltDouble(double %a, double %b) {
entry:
  %cmp = fcmp olt double %a, %b
  %cmp.ret_ext = zext i1 %cmp to i32
  ret i32 %cmp.ret_ext
}
; CHECK: fcmpOltDouble:
; CHECK: ucomisd
; CHECK: ja .

define internal i32 @fcmpOleFloat(float %a, float %b) {
entry:
  %cmp = fcmp ole float %a, %b
  %cmp.ret_ext = zext i1 %cmp to i32
  ret i32 %cmp.ret_ext
}
; CHECK: fcmpOleFloat:
; CHECK: ucomiss
; CHECK: jae .

define internal i32 @fcmpOleDouble(double %a, double %b) {
entry:
  %cmp = fcmp ole double %a, %b
  %cmp.ret_ext = zext i1 %cmp to i32
  ret i32 %cmp.ret_ext
}
; CHECK: fcmpOleDouble:
; CHECK: ucomisd
; CHECK: jae .

define internal i32 @fcmpOneFloat(float %a, float %b) {
entry:
  %cmp = fcmp one float %a, %b
  %cmp.ret_ext = zext i1 %cmp to i32
  ret i32 %cmp.ret_ext
}
; CHECK: fcmpOneFloat:
; CHECK: ucomiss
; CHECK: jne .

define internal i32 @fcmpOneDouble(double %a, double %b) {
entry:
  %cmp = fcmp one double %a, %b
  %cmp.ret_ext = zext i1 %cmp to i32
  ret i32 %cmp.ret_ext
}
; CHECK: fcmpOneDouble:
; CHECK: ucomisd
; CHECK: jne .

define internal i32 @fcmpOrdFloat(float %a, float %b) {
entry:
  %cmp = fcmp ord float %a, %b
  %cmp.ret_ext = zext i1 %cmp to i32
  ret i32 %cmp.ret_ext
}
; CHECK: fcmpOrdFloat:
; CHECK: ucomiss
; CHECK: jnp .

define internal i32 @fcmpOrdDouble(double %a, double %b) {
entry:
  %cmp = fcmp ord double %a, %b
  %cmp.ret_ext = zext i1 %cmp to i32
  ret i32 %cmp.ret_ext
}
; CHECK: fcmpOrdDouble:
; CHECK: ucomisd
; CHECK: jnp .

define internal i32 @fcmpUeqFloat(float %a, float %b) {
entry:
  %cmp = fcmp ueq float %a, %b
  %cmp.ret_ext = zext i1 %cmp to i32
  ret i32 %cmp.ret_ext
}
; CHECK: fcmpUeqFloat:
; CHECK: ucomiss
; CHECK: je .

define internal i32 @fcmpUeqDouble(double %a, double %b) {
entry:
  %cmp = fcmp ueq double %a, %b
  %cmp.ret_ext = zext i1 %cmp to i32
  ret i32 %cmp.ret_ext
}
; CHECK: fcmpUeqDouble:
; CHECK: ucomisd
; CHECK: je .

define internal i32 @fcmpUgtFloat(float %a, float %b) {
entry:
  %cmp = fcmp ugt float %a, %b
  %cmp.ret_ext = zext i1 %cmp to i32
  ret i32 %cmp.ret_ext
}
; CHECK: fcmpUgtFloat:
; CHECK: ucomiss
; CHECK: jb .

define internal i32 @fcmpUgtDouble(double %a, double %b) {
entry:
  %cmp = fcmp ugt double %a, %b
  %cmp.ret_ext = zext i1 %cmp to i32
  ret i32 %cmp.ret_ext
}
; CHECK: fcmpUgtDouble:
; CHECK: ucomisd
; CHECK: jb .

define internal i32 @fcmpUgeFloat(float %a, float %b) {
entry:
  %cmp = fcmp uge float %a, %b
  %cmp.ret_ext = zext i1 %cmp to i32
  ret i32 %cmp.ret_ext
}
; CHECK: fcmpUgeFloat:
; CHECK: ucomiss
; CHECK: jbe .

define internal i32 @fcmpUgeDouble(double %a, double %b) {
entry:
  %cmp = fcmp uge double %a, %b
  %cmp.ret_ext = zext i1 %cmp to i32
  ret i32 %cmp.ret_ext
}
; CHECK: fcmpUgeDouble:
; CHECK: ucomisd
; CHECK: jbe .

define internal i32 @fcmpUltFloat(float %a, float %b) {
entry:
  %cmp = fcmp ult float %a, %b
  %cmp.ret_ext = zext i1 %cmp to i32
  ret i32 %cmp.ret_ext
}
; CHECK: fcmpUltFloat:
; CHECK: ucomiss
; CHECK: jb .

define internal i32 @fcmpUltDouble(double %a, double %b) {
entry:
  %cmp = fcmp ult double %a, %b
  %cmp.ret_ext = zext i1 %cmp to i32
  ret i32 %cmp.ret_ext
}
; CHECK: fcmpUltDouble:
; CHECK: ucomisd
; CHECK: jb .

define internal i32 @fcmpUleFloat(float %a, float %b) {
entry:
  %cmp = fcmp ule float %a, %b
  %cmp.ret_ext = zext i1 %cmp to i32
  ret i32 %cmp.ret_ext
}
; CHECK: fcmpUleFloat:
; CHECK: ucomiss
; CHECK: jbe .

define internal i32 @fcmpUleDouble(double %a, double %b) {
entry:
  %cmp = fcmp ule double %a, %b
  %cmp.ret_ext = zext i1 %cmp to i32
  ret i32 %cmp.ret_ext
}
; CHECK: fcmpUleDouble:
; CHECK: ucomisd
; CHECK: jbe .

define internal i32 @fcmpUneFloat(float %a, float %b) {
entry:
  %cmp = fcmp une float %a, %b
  %cmp.ret_ext = zext i1 %cmp to i32
  ret i32 %cmp.ret_ext
}
; CHECK: fcmpUneFloat:
; CHECK: ucomiss
; CHECK: jne .
; CHECK: jp .

define internal i32 @fcmpUneDouble(double %a, double %b) {
entry:
  %cmp = fcmp une double %a, %b
  %cmp.ret_ext = zext i1 %cmp to i32
  ret i32 %cmp.ret_ext
}
; CHECK: fcmpUneDouble:
; CHECK: ucomisd
; CHECK: jne .
; CHECK: jp .

define internal i32 @fcmpUnoFloat(float %a, float %b) {
entry:
  %cmp = fcmp uno float %a, %b
  %cmp.ret_ext = zext i1 %cmp to i32
  ret i32 %cmp.ret_ext
}
; CHECK: fcmpUnoFloat:
; CHECK: ucomiss
; CHECK: jp .

define internal i32 @fcmpUnoDouble(double %a, double %b) {
entry:
  %cmp = fcmp uno double %a, %b
  %cmp.ret_ext = zext i1 %cmp to i32
  ret i32 %cmp.ret_ext
}
; CHECK: fcmpUnoDouble:
; CHECK: ucomisd
; CHECK: jp .

define internal i32 @fcmpTrueFloat(float %a, float %b) {
entry:
  %cmp = fcmp true float %a, %b
  %cmp.ret_ext = zext i1 %cmp to i32
  ret i32 %cmp.ret_ext
}
; CHECK: fcmpTrueFloat:
; CHECK: mov {{.*}}, 1

define internal i32 @fcmpTrueDouble(double %a, double %b) {
entry:
  %cmp = fcmp true double %a, %b
  %cmp.ret_ext = zext i1 %cmp to i32
  ret i32 %cmp.ret_ext
}
; CHECK: fcmpTrueDouble:
; CHECK: mov {{.*}}, 1

define internal float @loadFloat(i32 %a) {
entry:
  %a.asptr = inttoptr i32 %a to float*
  %v0 = load float* %a.asptr, align 4
  ret float %v0
}
; CHECK: loadFloat:
; CHECK: movss
; CHECK: fld

define internal double @loadDouble(i32 %a) {
entry:
  %a.asptr = inttoptr i32 %a to double*
  %v0 = load double* %a.asptr, align 8
  ret double %v0
}
; CHECK: loadDouble:
; CHECK: movsd
; CHECK: fld

define internal void @storeFloat(i32 %a, float %value) {
entry:
  %a.asptr = inttoptr i32 %a to float*
  store float %value, float* %a.asptr, align 4
  ret void
}
; CHECK: storeFloat:
; CHECK: movss

define internal void @storeDouble(i32 %a, double %value) {
entry:
  %a.asptr = inttoptr i32 %a to double*
  store double %value, double* %a.asptr, align 8
  ret void
}
; CHECK: storeDouble:
; CHECK: movsd

define internal void @storeFloatConst(i32 %a) {
entry:
  %a.asptr = inttoptr i32 %a to float*
  store float 0x3FF3AE1480000000, float* %a.asptr, align 4
  ret void
}
; CHECK: storeFloatConst:
; CHECK: mov
; CHECK: mov

define internal void @storeDoubleConst(i32 %a) {
entry:
  %a.asptr = inttoptr i32 %a to double*
  store double 1.230000e+00, double* %a.asptr, align 8
  ret void
}
; CHECK: storeDoubleConst:
; CHECK: mov
; CHECK: mov

define internal float @selectFloatVarVar(float %a, float %b) {
entry:
  %cmp = fcmp olt float %a, %b
  %cond = select i1 %cmp, float %a, float %b
  ret float %cond
}
; CHECK: selectFloatVarVar:
; CHECK: ucomiss
; CHECK: ja .
; CHECK: fld

define internal double @selectDoubleVarVar(double %a, double %b) {
entry:
  %cmp = fcmp olt double %a, %b
  %cond = select i1 %cmp, double %a, double %b
  ret double %cond
}
; CHECK: selectDoubleVarVar:
; CHECK: ucomisd
; CHECK: ja .
; CHECK: fld

; ERRORS-NOT: ICE translation error
; DUMP-NOT: SZ
