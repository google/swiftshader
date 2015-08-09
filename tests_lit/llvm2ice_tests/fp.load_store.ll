; This tries to be a comprehensive test of f32 and f64 compare operations.
; The CHECK lines are only checking for basic instruction patterns
; that should be present regardless of the optimization level, so
; there are no special OPTM1 match lines.

; RUN: %p2i --filetype=obj --disassemble -i %s --args -O2 | FileCheck %s
; RUN: %p2i --filetype=obj --disassemble -i %s --args -Om1 | FileCheck %s

define internal float @loadFloat(i32 %a) {
entry:
  %__1 = inttoptr i32 %a to float*
  %v0 = load float, float* %__1, align 4
  ret float %v0
}
; CHECK-LABEL: loadFloat
; CHECK: movss
; CHECK: fld

define internal double @loadDouble(i32 %a) {
entry:
  %__1 = inttoptr i32 %a to double*
  %v0 = load double, double* %__1, align 8
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
; CHECK-LABEL: storeFloat
; CHECK: movss
; CHECK: movss

define internal void @storeDouble(i32 %a, double %value) {
entry:
  %__2 = inttoptr i32 %a to double*
  store double %value, double* %__2, align 8
  ret void
}
; CHECK-LABEL: storeDouble
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
