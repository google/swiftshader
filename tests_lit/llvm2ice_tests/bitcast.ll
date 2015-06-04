; Trivial smoke test of bitcast between integer and FP types.

; RUN: %p2i --filetype=obj --disassemble -i %s --args -O2 | FileCheck %s
; RUN: %p2i --filetype=obj --disassemble -i %s --args -Om1 | FileCheck %s

define internal i32 @cast_f2i(float %f) {
entry:
  %v0 = bitcast float %f to i32
  ret i32 %v0
}
; CHECK-LABEL: cast_f2i
; CHECK: mov eax

define internal float @cast_i2f(i32 %i) {
entry:
  %v0 = bitcast i32 %i to float
  ret float %v0
}
; CHECK-LABEL: cast_i2f
; CHECK: fld DWORD PTR

define internal i64 @cast_d2ll(double %d) {
entry:
  %v0 = bitcast double %d to i64
  ret i64 %v0
}
; CHECK-LABEL: cast_d2ll
; CHECK: mov edx

define internal i64 @cast_d2ll_const() {
entry:
  %v0 = bitcast double 0x12345678901234 to i64
  ret i64 %v0
}
; CHECK-LABEL: cast_d2ll_const
; CHECK: mov e{{..}},DWORD PTR ds:0x0 {{.*}} .L$double$0
; CHECK: mov e{{..}},DWORD PTR ds:0x4 {{.*}} .L$double$0

define internal double @cast_ll2d(i64 %ll) {
entry:
  %v0 = bitcast i64 %ll to double
  ret double %v0
}
; CHECK-LABEL: cast_ll2d
; CHECK: fld QWORD PTR

define internal double @cast_ll2d_const() {
entry:
  %v0 = bitcast i64 12345678901234 to double
  ret double %v0
}
; CHECK-LABEL: cast_ll2d_const
; CHECK: mov {{.*}},0x73ce2ff2
; CHECK: mov {{.*}},0xb3a
; CHECK: fld QWORD PTR
