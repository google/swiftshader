; Trivial smoke test of bitcast between integer and FP types.

; RUN: %p2i --assemble --disassemble -i %s --args -O2 --verbose none \
; RUN:   | FileCheck %s
; RUN: %p2i --assemble --disassemble -i %s --args -Om1 --verbose none \
; RUN:   | FileCheck %s

define internal i32 @cast_f2i(float %f) {
entry:
  %v0 = bitcast float %f to i32
  ret i32 %v0
}
; CHECK-LABEL: cast_f2i
; CHECK: mov eax
; CHECK: ret

define internal float @cast_i2f(i32 %i) {
entry:
  %v0 = bitcast i32 %i to float
  ret float %v0
}
; CHECK-LABEL: cast_i2f
; CHECK: fld DWORD PTR
; CHECK: ret

define internal i64 @cast_d2ll(double %d) {
entry:
  %v0 = bitcast double %d to i64
  ret i64 %v0
}
; CHECK-LABEL: cast_d2ll
; CHECK: mov edx
; CHECK: ret

define internal i64 @cast_d2ll_const() {
entry:
  %v0 = bitcast double 0x12345678901234 to i64
  ret i64 %v0
}
; CHECK-LABEL: cast_d2ll_const
; CHECK: movsd xmm{{.*}},QWORD PTR
; CHECK: mov edx
; CHECK: ret

define internal double @cast_ll2d(i64 %ll) {
entry:
  %v0 = bitcast i64 %ll to double
  ret double %v0
}
; CHECK-LABEL: cast_ll2d
; CHECK: fld QWORD PTR
; CHECK: ret

define internal double @cast_ll2d_const() {
entry:
  %v0 = bitcast i64 12345678901234 to double
  ret double %v0
}
; CHECK-LABEL: cast_ll2d_const
; CHECK: mov {{.*}},0x73ce2ff2
; CHECK: mov {{.*}},0xb3a
; CHECK: fld QWORD PTR
; CHECK: ret
