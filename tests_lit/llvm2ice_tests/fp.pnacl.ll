; This tries to be a comprehensive test of f32 and f64 operations.
; The CHECK lines are only checking for basic instruction patterns
; that should be present regardless of the optimization level, so
; there are no special OPTM1 match lines.

; RUN: %llvm2ice -O2 --verbose none %s | FileCheck %s
; RUN: %llvm2ice -Om1 --verbose none %s | FileCheck %s
; RUN: %llvm2ice --verbose none %s | FileCheck --check-prefix=ERRORS %s
; RUN: %llvm2iceinsts %s | %szdiff %s | FileCheck --check-prefix=DUMP %s
; RUN: %llvm2iceinsts --pnacl %s | %szdiff %s \
; RUN:                           | FileCheck --check-prefix=DUMP %s

@__init_array_start = internal constant [0 x i8] zeroinitializer, align 4
@__fini_array_start = internal constant [0 x i8] zeroinitializer, align 4
@__tls_template_start = internal constant [0 x i8] zeroinitializer, align 8
@__tls_template_alignment = internal constant [4 x i8] c"\01\00\00\00", align 4

define internal i32 @doubleArgs(double %a, i32 %b, double %c) {
entry:
  ret i32 %b
}
; CHECK-LABEL: doubleArgs
; CHECK:      mov     eax, dword ptr [esp+12]
; CHECK-NEXT: ret

define internal i32 @floatArgs(float %a, i32 %b, float %c) {
entry:
  ret i32 %b
}
; CHECK-LABEL: floatArgs
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
; CHECK-LABEL: passFpArgs
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
; CHECK-LABEL: passFpConstArg
; CHECK: push 123
; CHECK: call ignoreFpArgsNoInline

define internal i32 @passFp32ConstArg(float %a) {
entry:
  %call = call i32 @ignoreFp32ArgsNoInline(float %a, i32 123, float 2.0)
  ret i32 %call
}
; CHECK-LABEL: passFp32ConstArg
; CHECK: push dword
; CHECK: push 123
; CHECK: call ignoreFp32ArgsNoInline

declare i32 @ignoreFp32ArgsNoInline(float, i32, float)

define internal float @returnFloatArg(float %a) {
entry:
  ret float %a
}
; CHECK-LABEL: returnFloatArg
; CHECK: fld dword ptr [esp

define internal double @returnDoubleArg(double %a) {
entry:
  ret double %a
}
; CHECK-LABEL: returnDoubleArg
; CHECK: fld qword ptr [esp

define internal float @returnFloatConst() {
entry:
  ret float 0x3FF3AE1480000000
}
; CHECK-LABEL: returnFloatConst
; CHECK: fld

define internal double @returnDoubleConst() {
entry:
  ret double 1.230000e+00
}
; CHECK-LABEL: returnDoubleConst
; CHECK: fld

define internal float @addFloat(float %a, float %b) {
entry:
  %add = fadd float %a, %b
  ret float %add
}
; CHECK-LABEL: addFloat
; CHECK: addss
; CHECK: fld

define internal double @addDouble(double %a, double %b) {
entry:
  %add = fadd double %a, %b
  ret double %add
}
; CHECK-LABEL: addDouble
; CHECK: addsd
; CHECK: fld

define internal float @subFloat(float %a, float %b) {
entry:
  %sub = fsub float %a, %b
  ret float %sub
}
; CHECK-LABEL: subFloat
; CHECK: subss
; CHECK: fld

define internal double @subDouble(double %a, double %b) {
entry:
  %sub = fsub double %a, %b
  ret double %sub
}
; CHECK-LABEL: subDouble
; CHECK: subsd
; CHECK: fld

define internal float @mulFloat(float %a, float %b) {
entry:
  %mul = fmul float %a, %b
  ret float %mul
}
; CHECK-LABEL: mulFloat
; CHECK: mulss
; CHECK: fld

define internal double @mulDouble(double %a, double %b) {
entry:
  %mul = fmul double %a, %b
  ret double %mul
}
; CHECK-LABEL: mulDouble
; CHECK: mulsd
; CHECK: fld

define internal float @divFloat(float %a, float %b) {
entry:
  %div = fdiv float %a, %b
  ret float %div
}
; CHECK-LABEL: divFloat
; CHECK: divss
; CHECK: fld

define internal double @divDouble(double %a, double %b) {
entry:
  %div = fdiv double %a, %b
  ret double %div
}
; CHECK-LABEL: divDouble
; CHECK: divsd
; CHECK: fld

define internal float @remFloat(float %a, float %b) {
entry:
  %div = frem float %a, %b
  ret float %div
}
; CHECK-LABEL: remFloat
; CHECK: call fmodf

define internal double @remDouble(double %a, double %b) {
entry:
  %div = frem double %a, %b
  ret double %div
}
; CHECK-LABEL: remDouble
; CHECK: call fmod

define internal float @fptrunc(double %a) {
entry:
  %conv = fptrunc double %a to float
  ret float %conv
}
; CHECK-LABEL: fptrunc
; CHECK: cvtsd2ss
; CHECK: fld

define internal double @fpext(float %a) {
entry:
  %conv = fpext float %a to double
  ret double %conv
}
; CHECK-LABEL: fpext
; CHECK: cvtss2sd
; CHECK: fld

define internal i64 @doubleToSigned64(double %a) {
entry:
  %conv = fptosi double %a to i64
  ret i64 %conv
}
; CHECK-LABEL: doubleToSigned64
; CHECK: call cvtdtosi64

define internal i64 @floatToSigned64(float %a) {
entry:
  %conv = fptosi float %a to i64
  ret i64 %conv
}
; CHECK-LABEL: floatToSigned64
; CHECK: call cvtftosi64

define internal i64 @doubleToUnsigned64(double %a) {
entry:
  %conv = fptoui double %a to i64
  ret i64 %conv
}
; CHECK-LABEL: doubleToUnsigned64
; CHECK: call cvtdtoui64

define internal i64 @floatToUnsigned64(float %a) {
entry:
  %conv = fptoui float %a to i64
  ret i64 %conv
}
; CHECK-LABEL: floatToUnsigned64
; CHECK: call cvtftoui64

define internal i32 @doubleToSigned32(double %a) {
entry:
  %conv = fptosi double %a to i32
  ret i32 %conv
}
; CHECK-LABEL: doubleToSigned32
; CHECK: cvtsd2si

define internal i32 @floatToSigned32(float %a) {
entry:
  %conv = fptosi float %a to i32
  ret i32 %conv
}
; CHECK-LABEL: floatToSigned32
; CHECK: cvtss2si

define internal i32 @doubleToUnsigned32(double %a) {
entry:
  %conv = fptoui double %a to i32
  ret i32 %conv
}
; CHECK-LABEL: doubleToUnsigned32
; CHECK: call cvtdtoui32

define internal i32 @floatToUnsigned32(float %a) {
entry:
  %conv = fptoui float %a to i32
  ret i32 %conv
}
; CHECK-LABEL: floatToUnsigned32
; CHECK: call cvtftoui32

define internal i32 @doubleToSigned16(double %a) {
entry:
  %conv = fptosi double %a to i16
  %conv.ret_ext = sext i16 %conv to i32
  ret i32 %conv.ret_ext
}
; CHECK-LABEL: doubleToSigned16
; CHECK: cvtsd2si
; CHECK: movsx

define internal i32 @floatToSigned16(float %a) {
entry:
  %conv = fptosi float %a to i16
  %conv.ret_ext = sext i16 %conv to i32
  ret i32 %conv.ret_ext
}
; CHECK-LABEL: floatToSigned16
; CHECK: cvtss2si
; CHECK: movsx

define internal i32 @doubleToUnsigned16(double %a) {
entry:
  %conv = fptoui double %a to i16
  %conv.ret_ext = zext i16 %conv to i32
  ret i32 %conv.ret_ext
}
; CHECK-LABEL: doubleToUnsigned16
; CHECK: cvtsd2si
; CHECK: movzx

define internal i32 @floatToUnsigned16(float %a) {
entry:
  %conv = fptoui float %a to i16
  %conv.ret_ext = zext i16 %conv to i32
  ret i32 %conv.ret_ext
}
; CHECK-LABEL: floatToUnsigned16
; CHECK: cvtss2si
; CHECK: movzx

define internal i32 @doubleToSigned8(double %a) {
entry:
  %conv = fptosi double %a to i8
  %conv.ret_ext = sext i8 %conv to i32
  ret i32 %conv.ret_ext
}
; CHECK-LABEL: doubleToSigned8
; CHECK: cvtsd2si
; CHECK: movsx

define internal i32 @floatToSigned8(float %a) {
entry:
  %conv = fptosi float %a to i8
  %conv.ret_ext = sext i8 %conv to i32
  ret i32 %conv.ret_ext
}
; CHECK-LABEL: floatToSigned8
; CHECK: cvtss2si
; CHECK: movsx

define internal i32 @doubleToUnsigned8(double %a) {
entry:
  %conv = fptoui double %a to i8
  %conv.ret_ext = zext i8 %conv to i32
  ret i32 %conv.ret_ext
}
; CHECK-LABEL: doubleToUnsigned8
; CHECK: cvtsd2si
; CHECK: movzx

define internal i32 @floatToUnsigned8(float %a) {
entry:
  %conv = fptoui float %a to i8
  %conv.ret_ext = zext i8 %conv to i32
  ret i32 %conv.ret_ext
}
; CHECK-LABEL: floatToUnsigned8
; CHECK: cvtss2si
; CHECK: movzx

define internal i32 @doubleToUnsigned1(double %a) {
entry:
  %tobool = fptoui double %a to i1
  %tobool.ret_ext = zext i1 %tobool to i32
  ret i32 %tobool.ret_ext
}
; CHECK-LABEL: doubleToUnsigned1
; CHECK: cvtsd2si
; CHECK: and eax, 1

define internal i32 @floatToUnsigned1(float %a) {
entry:
  %tobool = fptoui float %a to i1
  %tobool.ret_ext = zext i1 %tobool to i32
  ret i32 %tobool.ret_ext
}
; CHECK-LABEL: floatToUnsigned1
; CHECK: cvtss2si
; CHECK: and eax, 1

define internal double @signed64ToDouble(i64 %a) {
entry:
  %conv = sitofp i64 %a to double
  ret double %conv
}
; CHECK-LABEL: signed64ToDouble
; CHECK: call cvtsi64tod
; CHECK: fstp

define internal float @signed64ToFloat(i64 %a) {
entry:
  %conv = sitofp i64 %a to float
  ret float %conv
}
; CHECK-LABEL: signed64ToFloat
; CHECK: call cvtsi64tof
; CHECK: fstp

define internal double @unsigned64ToDouble(i64 %a) {
entry:
  %conv = uitofp i64 %a to double
  ret double %conv
}
; CHECK-LABEL: unsigned64ToDouble
; CHECK: call cvtui64tod
; CHECK: fstp

define internal float @unsigned64ToFloat(i64 %a) {
entry:
  %conv = uitofp i64 %a to float
  ret float %conv
}
; CHECK-LABEL: unsigned64ToFloat
; CHECK: call cvtui64tof
; CHECK: fstp

define internal double @unsigned64ToDoubleConst() {
entry:
  %conv = uitofp i64 12345678901234 to double
  ret double %conv
}
; CHECK-LABEL: unsigned64ToDouble
; CHECK: push 2874
; CHECK: push 1942892530
; CHECK: call cvtui64tod
; CHECK: fstp

define internal double @signed32ToDouble(i32 %a) {
entry:
  %conv = sitofp i32 %a to double
  ret double %conv
}
; CHECK-LABEL: signed32ToDouble
; CHECK: cvtsi2sd
; CHECK: fld

define internal float @signed32ToFloat(i32 %a) {
entry:
  %conv = sitofp i32 %a to float
  ret float %conv
}
; CHECK-LABEL: signed32ToFloat
; CHECK: cvtsi2ss
; CHECK: fld

define internal double @unsigned32ToDouble(i32 %a) {
entry:
  %conv = uitofp i32 %a to double
  ret double %conv
}
; CHECK-LABEL: unsigned32ToDouble
; CHECK: call cvtui32tod
; CHECK: fstp

define internal float @unsigned32ToFloat(i32 %a) {
entry:
  %conv = uitofp i32 %a to float
  ret float %conv
}
; CHECK-LABEL: unsigned32ToFloat
; CHECK: call cvtui32tof
; CHECK: fstp

define internal double @signed16ToDouble(i32 %a) {
entry:
  %a.arg_trunc = trunc i32 %a to i16
  %conv = sitofp i16 %a.arg_trunc to double
  ret double %conv
}
; CHECK-LABEL: signed16ToDouble
; CHECK: cvtsi2sd
; CHECK: fld

define internal float @signed16ToFloat(i32 %a) {
entry:
  %a.arg_trunc = trunc i32 %a to i16
  %conv = sitofp i16 %a.arg_trunc to float
  ret float %conv
}
; CHECK-LABEL: signed16ToFloat
; CHECK: cvtsi2ss
; CHECK: fld

define internal double @unsigned16ToDouble(i32 %a) {
entry:
  %a.arg_trunc = trunc i32 %a to i16
  %conv = uitofp i16 %a.arg_trunc to double
  ret double %conv
}
; CHECK-LABEL: unsigned16ToDouble
; CHECK: cvtsi2sd
; CHECK: fld

define internal float @unsigned16ToFloat(i32 %a) {
entry:
  %a.arg_trunc = trunc i32 %a to i16
  %conv = uitofp i16 %a.arg_trunc to float
  ret float %conv
}
; CHECK-LABEL: unsigned16ToFloat
; CHECK: cvtsi2ss
; CHECK: fld

define internal double @signed8ToDouble(i32 %a) {
entry:
  %a.arg_trunc = trunc i32 %a to i8
  %conv = sitofp i8 %a.arg_trunc to double
  ret double %conv
}
; CHECK-LABEL: signed8ToDouble
; CHECK: cvtsi2sd
; CHECK: fld

define internal float @signed8ToFloat(i32 %a) {
entry:
  %a.arg_trunc = trunc i32 %a to i8
  %conv = sitofp i8 %a.arg_trunc to float
  ret float %conv
}
; CHECK-LABEL: signed8ToFloat
; CHECK: cvtsi2ss
; CHECK: fld

define internal double @unsigned8ToDouble(i32 %a) {
entry:
  %a.arg_trunc = trunc i32 %a to i8
  %conv = uitofp i8 %a.arg_trunc to double
  ret double %conv
}
; CHECK-LABEL: unsigned8ToDouble
; CHECK: cvtsi2sd
; CHECK: fld

define internal float @unsigned8ToFloat(i32 %a) {
entry:
  %a.arg_trunc = trunc i32 %a to i8
  %conv = uitofp i8 %a.arg_trunc to float
  ret float %conv
}
; CHECK-LABEL: unsigned8ToFloat
; CHECK: cvtsi2ss
; CHECK: fld

define internal double @unsigned1ToDouble(i32 %a) {
entry:
  %a.arg_trunc = trunc i32 %a to i1
  %conv = uitofp i1 %a.arg_trunc to double
  ret double %conv
}
; CHECK-LABEL: unsigned1ToDouble
; CHECK: cvtsi2sd
; CHECK: fld

define internal float @unsigned1ToFloat(i32 %a) {
entry:
  %a.arg_trunc = trunc i32 %a to i1
  %conv = uitofp i1 %a.arg_trunc to float
  ret float %conv
}
; CHECK-LABEL: unsigned1ToFloat
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
; CHECK-LABEL: fcmpEq
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
; CHECK-LABEL: fcmpNe
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
; CHECK-LABEL: fcmpGt
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
; CHECK-LABEL: fcmpGe
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
; CHECK-LABEL: fcmpLt
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
; CHECK-LABEL: fcmpLe
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
; CHECK-LABEL: fcmpFalseFloat
; CHECK: mov {{.*}}, 0

define internal i32 @fcmpFalseDouble(double %a, double %b) {
entry:
  %cmp = fcmp false double %a, %b
  %cmp.ret_ext = zext i1 %cmp to i32
  ret i32 %cmp.ret_ext
}
; CHECK-LABEL: fcmpFalseDouble
; CHECK: mov {{.*}}, 0

define internal i32 @fcmpOeqFloat(float %a, float %b) {
entry:
  %cmp = fcmp oeq float %a, %b
  %cmp.ret_ext = zext i1 %cmp to i32
  ret i32 %cmp.ret_ext
}
; CHECK-LABEL: fcmpOeqFloat
; CHECK: ucomiss
; CHECK: jne .
; CHECK: jp .

define internal i32 @fcmpOeqDouble(double %a, double %b) {
entry:
  %cmp = fcmp oeq double %a, %b
  %cmp.ret_ext = zext i1 %cmp to i32
  ret i32 %cmp.ret_ext
}
; CHECK-LABEL: fcmpOeqDouble
; CHECK: ucomisd
; CHECK: jne .
; CHECK: jp .

define internal i32 @fcmpOgtFloat(float %a, float %b) {
entry:
  %cmp = fcmp ogt float %a, %b
  %cmp.ret_ext = zext i1 %cmp to i32
  ret i32 %cmp.ret_ext
}
; CHECK-LABEL: fcmpOgtFloat
; CHECK: ucomiss
; CHECK: ja .

define internal i32 @fcmpOgtDouble(double %a, double %b) {
entry:
  %cmp = fcmp ogt double %a, %b
  %cmp.ret_ext = zext i1 %cmp to i32
  ret i32 %cmp.ret_ext
}
; CHECK-LABEL: fcmpOgtDouble
; CHECK: ucomisd
; CHECK: ja .

define internal i32 @fcmpOgeFloat(float %a, float %b) {
entry:
  %cmp = fcmp oge float %a, %b
  %cmp.ret_ext = zext i1 %cmp to i32
  ret i32 %cmp.ret_ext
}
; CHECK-LABEL: fcmpOgeFloat
; CHECK: ucomiss
; CHECK: jae .

define internal i32 @fcmpOgeDouble(double %a, double %b) {
entry:
  %cmp = fcmp oge double %a, %b
  %cmp.ret_ext = zext i1 %cmp to i32
  ret i32 %cmp.ret_ext
}
; CHECK-LABEL: fcmpOgeDouble
; CHECK: ucomisd
; CHECK: jae .

define internal i32 @fcmpOltFloat(float %a, float %b) {
entry:
  %cmp = fcmp olt float %a, %b
  %cmp.ret_ext = zext i1 %cmp to i32
  ret i32 %cmp.ret_ext
}
; CHECK-LABEL: fcmpOltFloat
; CHECK: ucomiss
; CHECK: ja .

define internal i32 @fcmpOltDouble(double %a, double %b) {
entry:
  %cmp = fcmp olt double %a, %b
  %cmp.ret_ext = zext i1 %cmp to i32
  ret i32 %cmp.ret_ext
}
; CHECK-LABEL: fcmpOltDouble
; CHECK: ucomisd
; CHECK: ja .

define internal i32 @fcmpOleFloat(float %a, float %b) {
entry:
  %cmp = fcmp ole float %a, %b
  %cmp.ret_ext = zext i1 %cmp to i32
  ret i32 %cmp.ret_ext
}
; CHECK-LABEL: fcmpOleFloat
; CHECK: ucomiss
; CHECK: jae .

define internal i32 @fcmpOleDouble(double %a, double %b) {
entry:
  %cmp = fcmp ole double %a, %b
  %cmp.ret_ext = zext i1 %cmp to i32
  ret i32 %cmp.ret_ext
}
; CHECK-LABEL: fcmpOleDouble
; CHECK: ucomisd
; CHECK: jae .

define internal i32 @fcmpOneFloat(float %a, float %b) {
entry:
  %cmp = fcmp one float %a, %b
  %cmp.ret_ext = zext i1 %cmp to i32
  ret i32 %cmp.ret_ext
}
; CHECK-LABEL: fcmpOneFloat
; CHECK: ucomiss
; CHECK: jne .

define internal i32 @fcmpOneDouble(double %a, double %b) {
entry:
  %cmp = fcmp one double %a, %b
  %cmp.ret_ext = zext i1 %cmp to i32
  ret i32 %cmp.ret_ext
}
; CHECK-LABEL: fcmpOneDouble
; CHECK: ucomisd
; CHECK: jne .

define internal i32 @fcmpOrdFloat(float %a, float %b) {
entry:
  %cmp = fcmp ord float %a, %b
  %cmp.ret_ext = zext i1 %cmp to i32
  ret i32 %cmp.ret_ext
}
; CHECK-LABEL: fcmpOrdFloat
; CHECK: ucomiss
; CHECK: jnp .

define internal i32 @fcmpOrdDouble(double %a, double %b) {
entry:
  %cmp = fcmp ord double %a, %b
  %cmp.ret_ext = zext i1 %cmp to i32
  ret i32 %cmp.ret_ext
}
; CHECK-LABEL: fcmpOrdDouble
; CHECK: ucomisd
; CHECK: jnp .

define internal i32 @fcmpUeqFloat(float %a, float %b) {
entry:
  %cmp = fcmp ueq float %a, %b
  %cmp.ret_ext = zext i1 %cmp to i32
  ret i32 %cmp.ret_ext
}
; CHECK-LABEL: fcmpUeqFloat
; CHECK: ucomiss
; CHECK: je .

define internal i32 @fcmpUeqDouble(double %a, double %b) {
entry:
  %cmp = fcmp ueq double %a, %b
  %cmp.ret_ext = zext i1 %cmp to i32
  ret i32 %cmp.ret_ext
}
; CHECK-LABEL: fcmpUeqDouble
; CHECK: ucomisd
; CHECK: je .

define internal i32 @fcmpUgtFloat(float %a, float %b) {
entry:
  %cmp = fcmp ugt float %a, %b
  %cmp.ret_ext = zext i1 %cmp to i32
  ret i32 %cmp.ret_ext
}
; CHECK-LABEL: fcmpUgtFloat
; CHECK: ucomiss
; CHECK: jb .

define internal i32 @fcmpUgtDouble(double %a, double %b) {
entry:
  %cmp = fcmp ugt double %a, %b
  %cmp.ret_ext = zext i1 %cmp to i32
  ret i32 %cmp.ret_ext
}
; CHECK-LABEL: fcmpUgtDouble
; CHECK: ucomisd
; CHECK: jb .

define internal i32 @fcmpUgeFloat(float %a, float %b) {
entry:
  %cmp = fcmp uge float %a, %b
  %cmp.ret_ext = zext i1 %cmp to i32
  ret i32 %cmp.ret_ext
}
; CHECK-LABEL: fcmpUgeFloat
; CHECK: ucomiss
; CHECK: jbe .

define internal i32 @fcmpUgeDouble(double %a, double %b) {
entry:
  %cmp = fcmp uge double %a, %b
  %cmp.ret_ext = zext i1 %cmp to i32
  ret i32 %cmp.ret_ext
}
; CHECK-LABEL: fcmpUgeDouble
; CHECK: ucomisd
; CHECK: jbe .

define internal i32 @fcmpUltFloat(float %a, float %b) {
entry:
  %cmp = fcmp ult float %a, %b
  %cmp.ret_ext = zext i1 %cmp to i32
  ret i32 %cmp.ret_ext
}
; CHECK-LABEL: fcmpUltFloat
; CHECK: ucomiss
; CHECK: jb .

define internal i32 @fcmpUltDouble(double %a, double %b) {
entry:
  %cmp = fcmp ult double %a, %b
  %cmp.ret_ext = zext i1 %cmp to i32
  ret i32 %cmp.ret_ext
}
; CHECK-LABEL: fcmpUltDouble
; CHECK: ucomisd
; CHECK: jb .

define internal i32 @fcmpUleFloat(float %a, float %b) {
entry:
  %cmp = fcmp ule float %a, %b
  %cmp.ret_ext = zext i1 %cmp to i32
  ret i32 %cmp.ret_ext
}
; CHECK-LABEL: fcmpUleFloat
; CHECK: ucomiss
; CHECK: jbe .

define internal i32 @fcmpUleDouble(double %a, double %b) {
entry:
  %cmp = fcmp ule double %a, %b
  %cmp.ret_ext = zext i1 %cmp to i32
  ret i32 %cmp.ret_ext
}
; CHECK-LABEL: fcmpUleDouble
; CHECK: ucomisd
; CHECK: jbe .

define internal i32 @fcmpUneFloat(float %a, float %b) {
entry:
  %cmp = fcmp une float %a, %b
  %cmp.ret_ext = zext i1 %cmp to i32
  ret i32 %cmp.ret_ext
}
; CHECK-LABEL: fcmpUneFloat
; CHECK: ucomiss
; CHECK: jne .
; CHECK: jp .

define internal i32 @fcmpUneDouble(double %a, double %b) {
entry:
  %cmp = fcmp une double %a, %b
  %cmp.ret_ext = zext i1 %cmp to i32
  ret i32 %cmp.ret_ext
}
; CHECK-LABEL: fcmpUneDouble
; CHECK: ucomisd
; CHECK: jne .
; CHECK: jp .

define internal i32 @fcmpUnoFloat(float %a, float %b) {
entry:
  %cmp = fcmp uno float %a, %b
  %cmp.ret_ext = zext i1 %cmp to i32
  ret i32 %cmp.ret_ext
}
; CHECK-LABEL: fcmpUnoFloat
; CHECK: ucomiss
; CHECK: jp .

define internal i32 @fcmpUnoDouble(double %a, double %b) {
entry:
  %cmp = fcmp uno double %a, %b
  %cmp.ret_ext = zext i1 %cmp to i32
  ret i32 %cmp.ret_ext
}
; CHECK-LABEL: fcmpUnoDouble
; CHECK: ucomisd
; CHECK: jp .

define internal i32 @fcmpTrueFloat(float %a, float %b) {
entry:
  %cmp = fcmp true float %a, %b
  %cmp.ret_ext = zext i1 %cmp to i32
  ret i32 %cmp.ret_ext
}
; CHECK-LABEL: fcmpTrueFloat
; CHECK: mov {{.*}}, 1

define internal i32 @fcmpTrueDouble(double %a, double %b) {
entry:
  %cmp = fcmp true double %a, %b
  %cmp.ret_ext = zext i1 %cmp to i32
  ret i32 %cmp.ret_ext
}
; CHECK-LABEL: fcmpTrueDouble
; CHECK: mov {{.*}}, 1

define internal float @loadFloat(i32 %a) {
entry:
  %__1 = inttoptr i32 %a to float*
  %v0 = load float* %__1, align 4
  ret float %v0
}
; CHECK-LABEL: loadFloat
; CHECK: movss
; CHECK: fld

define internal double @loadDouble(i32 %a) {
entry:
  %__1 = inttoptr i32 %a to double*
  %v0 = load double* %__1, align 8
  ret double %v0
}
; CHECK-LABEL: loadDouble
; CHECK: movsd
; CHECK: fld

define internal void @storeFloat(i32 %a, float %value) {
entry:
  %__2 = inttoptr i32 %a to float*
  store float %value, float* %__2, align 4
  ret void
}
; CHECK-LABEL: storeFloat:
; CHECK: movss
; CHECK: movss

define internal void @storeDouble(i32 %a, double %value) {
entry:
  %__2 = inttoptr i32 %a to double*
  store double %value, double* %__2, align 8
  ret void
}
; CHECK-LABEL: storeDouble:
; CHECK: movsd
; CHECK: movsd

define internal void @storeFloatConst(i32 %a) {
entry:
  %a.asptr = inttoptr i32 %a to float*
  store float 0x3FF3AE1480000000, float* %a.asptr, align 4
  ret void
}
; CHECK-LABEL: storeFloatConst
; CHECK: movss
; CHECK: movss

define internal void @storeDoubleConst(i32 %a) {
entry:
  %a.asptr = inttoptr i32 %a to double*
  store double 1.230000e+00, double* %a.asptr, align 8
  ret void
}
; CHECK-LABEL: storeDoubleConst
; CHECK: movsd
; CHECK: movsd

define internal float @selectFloatVarVar(float %a, float %b) {
entry:
  %cmp = fcmp olt float %a, %b
  %cond = select i1 %cmp, float %a, float %b
  ret float %cond
}
; CHECK-LABEL: selectFloatVarVar
; CHECK: ucomiss
; CHECK: ja .
; CHECK: fld

define internal double @selectDoubleVarVar(double %a, double %b) {
entry:
  %cmp = fcmp olt double %a, %b
  %cond = select i1 %cmp, double %a, double %b
  ret double %cond
}
; CHECK-LABEL: selectDoubleVarVar
; CHECK: ucomisd
; CHECK: ja .
; CHECK: fld

; ERRORS-NOT: ICE translation error
; DUMP-NOT: SZ
