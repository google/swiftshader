; This tries to be a comprehensive test of f32 and f64 convert operations.
; The CHECK lines are only checking for basic instruction patterns
; that should be present regardless of the optimization level, so
; there are no special OPTM1 match lines.

; RUN: %p2i --filetype=obj --disassemble -i %s --args -O2 | FileCheck %s
; RUN: %p2i --filetype=obj --disassemble -i %s --args -Om1 | FileCheck %s

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
; CHECK: call {{.*}} R_{{.*}} __Sz_fptosi_f64_i64

define internal i64 @floatToSigned64(float %a) {
entry:
  %conv = fptosi float %a to i64
  ret i64 %conv
}
; CHECK-LABEL: floatToSigned64
; CHECK: call {{.*}} R_{{.*}} __Sz_fptosi_f32_i64

define internal i64 @doubleToUnsigned64(double %a) {
entry:
  %conv = fptoui double %a to i64
  ret i64 %conv
}
; CHECK-LABEL: doubleToUnsigned64
; CHECK: call {{.*}} R_{{.*}} __Sz_fptoui_f64_i64

define internal i64 @floatToUnsigned64(float %a) {
entry:
  %conv = fptoui float %a to i64
  ret i64 %conv
}
; CHECK-LABEL: floatToUnsigned64
; CHECK: call {{.*}} R_{{.*}} __Sz_fptoui_f32_i64

define internal i32 @doubleToSigned32(double %a) {
entry:
  %conv = fptosi double %a to i32
  ret i32 %conv
}
; CHECK-LABEL: doubleToSigned32
; CHECK: cvttsd2si

define internal i32 @doubleToSigned32Const() {
entry:
  %conv = fptosi double 867.5309 to i32
  ret i32 %conv
}
; CHECK-LABEL: doubleToSigned32Const
; CHECK: cvttsd2si

define internal i32 @floatToSigned32(float %a) {
entry:
  %conv = fptosi float %a to i32
  ret i32 %conv
}
; CHECK-LABEL: floatToSigned32
; CHECK: cvttss2si

define internal i32 @doubleToUnsigned32(double %a) {
entry:
  %conv = fptoui double %a to i32
  ret i32 %conv
}
; CHECK-LABEL: doubleToUnsigned32
; CHECK: call {{.*}} R_{{.*}} __Sz_fptoui_f64_i32

define internal i32 @floatToUnsigned32(float %a) {
entry:
  %conv = fptoui float %a to i32
  ret i32 %conv
}
; CHECK-LABEL: floatToUnsigned32
; CHECK: call {{.*}} R_{{.*}} __Sz_fptoui_f32_i32

define internal i32 @doubleToSigned16(double %a) {
entry:
  %conv = fptosi double %a to i16
  %conv.ret_ext = sext i16 %conv to i32
  ret i32 %conv.ret_ext
}
; CHECK-LABEL: doubleToSigned16
; CHECK: cvttsd2si
; CHECK: movsx

define internal i32 @floatToSigned16(float %a) {
entry:
  %conv = fptosi float %a to i16
  %conv.ret_ext = sext i16 %conv to i32
  ret i32 %conv.ret_ext
}
; CHECK-LABEL: floatToSigned16
; CHECK: cvttss2si
; CHECK: movsx

define internal i32 @doubleToUnsigned16(double %a) {
entry:
  %conv = fptoui double %a to i16
  %conv.ret_ext = zext i16 %conv to i32
  ret i32 %conv.ret_ext
}
; CHECK-LABEL: doubleToUnsigned16
; CHECK: cvttsd2si
; CHECK: movzx

define internal i32 @floatToUnsigned16(float %a) {
entry:
  %conv = fptoui float %a to i16
  %conv.ret_ext = zext i16 %conv to i32
  ret i32 %conv.ret_ext
}
; CHECK-LABEL: floatToUnsigned16
; CHECK: cvttss2si
; CHECK: movzx

define internal i32 @doubleToSigned8(double %a) {
entry:
  %conv = fptosi double %a to i8
  %conv.ret_ext = sext i8 %conv to i32
  ret i32 %conv.ret_ext
}
; CHECK-LABEL: doubleToSigned8
; CHECK: cvttsd2si
; CHECK: movsx

define internal i32 @floatToSigned8(float %a) {
entry:
  %conv = fptosi float %a to i8
  %conv.ret_ext = sext i8 %conv to i32
  ret i32 %conv.ret_ext
}
; CHECK-LABEL: floatToSigned8
; CHECK: cvttss2si
; CHECK: movsx

define internal i32 @doubleToUnsigned8(double %a) {
entry:
  %conv = fptoui double %a to i8
  %conv.ret_ext = zext i8 %conv to i32
  ret i32 %conv.ret_ext
}
; CHECK-LABEL: doubleToUnsigned8
; CHECK: cvttsd2si
; CHECK: movzx

define internal i32 @floatToUnsigned8(float %a) {
entry:
  %conv = fptoui float %a to i8
  %conv.ret_ext = zext i8 %conv to i32
  ret i32 %conv.ret_ext
}
; CHECK-LABEL: floatToUnsigned8
; CHECK: cvttss2si
; CHECK: movzx

define internal i32 @doubleToUnsigned1(double %a) {
entry:
  %tobool = fptoui double %a to i1
  %tobool.ret_ext = zext i1 %tobool to i32
  ret i32 %tobool.ret_ext
}
; CHECK-LABEL: doubleToUnsigned1
; CHECK: cvttsd2si
; CHECK: and eax,0x1

define internal i32 @floatToUnsigned1(float %a) {
entry:
  %tobool = fptoui float %a to i1
  %tobool.ret_ext = zext i1 %tobool to i32
  ret i32 %tobool.ret_ext
}
; CHECK-LABEL: floatToUnsigned1
; CHECK: cvttss2si
; CHECK: and eax,0x1

define internal double @signed64ToDouble(i64 %a) {
entry:
  %conv = sitofp i64 %a to double
  ret double %conv
}
; CHECK-LABEL: signed64ToDouble
; CHECK: call {{.*}} R_{{.*}} __Sz_sitofp_i64_f64
; CHECK: fstp QWORD

define internal float @signed64ToFloat(i64 %a) {
entry:
  %conv = sitofp i64 %a to float
  ret float %conv
}
; CHECK-LABEL: signed64ToFloat
; CHECK: call {{.*}} R_{{.*}} __Sz_sitofp_i64_f32
; CHECK: fstp DWORD

define internal double @unsigned64ToDouble(i64 %a) {
entry:
  %conv = uitofp i64 %a to double
  ret double %conv
}
; CHECK-LABEL: unsigned64ToDouble
; CHECK: call {{.*}} R_{{.*}} __Sz_uitofp_i64_f64
; CHECK: fstp

define internal float @unsigned64ToFloat(i64 %a) {
entry:
  %conv = uitofp i64 %a to float
  ret float %conv
}
; CHECK-LABEL: unsigned64ToFloat
; CHECK: call {{.*}} R_{{.*}} __Sz_uitofp_i64_f32
; CHECK: fstp

define internal double @unsigned64ToDoubleConst() {
entry:
  %conv = uitofp i64 12345678901234 to double
  ret double %conv
}
; CHECK-LABEL: unsigned64ToDouble
; CHECK: mov DWORD PTR [esp+0x4],0xb3a
; CHECK: mov DWORD PTR [esp],0x73ce2ff2
; CHECK: call {{.*}} R_{{.*}} __Sz_uitofp_i64_f64
; CHECK: fstp

define internal double @signed32ToDouble(i32 %a) {
entry:
  %conv = sitofp i32 %a to double
  ret double %conv
}
; CHECK-LABEL: signed32ToDouble
; CHECK: cvtsi2sd
; CHECK: fld

define internal double @signed32ToDoubleConst() {
entry:
  %conv = sitofp i32 123 to double
  ret double %conv
}
; CHECK-LABEL: signed32ToDoubleConst
; CHECK: cvtsi2sd {{.*[^1]}}
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
; CHECK: call {{.*}} R_{{.*}} __Sz_uitofp_i32_f64
; CHECK: fstp QWORD

define internal float @unsigned32ToFloat(i32 %a) {
entry:
  %conv = uitofp i32 %a to float
  ret float %conv
}
; CHECK-LABEL: unsigned32ToFloat
; CHECK: call {{.*}} R_{{.*}} __Sz_uitofp_i32_f32
; CHECK: fstp DWORD

define internal double @signed16ToDouble(i32 %a) {
entry:
  %a.arg_trunc = trunc i32 %a to i16
  %conv = sitofp i16 %a.arg_trunc to double
  ret double %conv
}
; CHECK-LABEL: signed16ToDouble
; CHECK: cvtsi2sd
; CHECK: fld QWORD

define internal float @signed16ToFloat(i32 %a) {
entry:
  %a.arg_trunc = trunc i32 %a to i16
  %conv = sitofp i16 %a.arg_trunc to float
  ret float %conv
}
; CHECK-LABEL: signed16ToFloat
; CHECK: cvtsi2ss
; CHECK: fld DWORD

define internal double @unsigned16ToDouble(i32 %a) {
entry:
  %a.arg_trunc = trunc i32 %a to i16
  %conv = uitofp i16 %a.arg_trunc to double
  ret double %conv
}
; CHECK-LABEL: unsigned16ToDouble
; CHECK: cvtsi2sd
; CHECK: fld

define internal double @unsigned16ToDoubleConst() {
entry:
  %conv = uitofp i16 12345 to double
  ret double %conv
}
; CHECK-LABEL: unsigned16ToDoubleConst
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

define internal float @int32BitcastToFloat(i32 %a) {
entry:
  %conv = bitcast i32 %a to float
  ret float %conv
}
; CHECK-LABEL: int32BitcastToFloat
; CHECK: mov

define internal float @int32BitcastToFloatConst() {
entry:
  %conv = bitcast i32 8675309 to float
  ret float %conv
}
; CHECK-LABEL: int32BitcastToFloatConst
; CHECK: mov

define internal double @int64BitcastToDouble(i64 %a) {
entry:
  %conv = bitcast i64 %a to double
  ret double %conv
}
; CHECK-LABEL: int64BitcastToDouble
; CHECK: mov

define internal double @int64BitcastToDoubleConst() {
entry:
  %conv = bitcast i64 9035768 to double
  ret double %conv
}
; CHECK-LABEL: int64BitcastToDoubleConst
; CHECK: mov
