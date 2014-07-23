; Trivial smoke test of bitcast between integer and FP types.

; RUN: %llvm2ice -O2 --verbose none %s | FileCheck %s
; RUN: %llvm2ice -O2 --verbose none %s \
; RUN:               | llvm-mc -arch=x86 -x86-asm-syntax=intel -filetype=obj
; RUN: %llvm2ice -Om1 --verbose none %s \
; RUN:               | llvm-mc -arch=x86 -x86-asm-syntax=intel -filetype=obj
; RUN: %llvm2ice --verbose none %s | FileCheck --check-prefix=ERRORS %s
; RUN: %llvm2iceinsts %s | %szdiff %s | FileCheck --check-prefix=DUMP %s
; RUN: %llvm2iceinsts --pnacl %s | %szdiff %s \
; RUN:                           | FileCheck --check-prefix=DUMP %s

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
; CHECK: fld dword ptr
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
; CHECK: movsd xmm{{.*}}, {{.*}}L$double
; CHECK: mov edx
; CHECK: ret

define internal double @cast_ll2d(i64 %ll) {
entry:
  %v0 = bitcast i64 %ll to double
  ret double %v0
}
; CHECK-LABEL: cast_ll2d
; CHECK: fld qword ptr
; CHECK: ret

define internal double @cast_ll2d_const() {
entry:
  %v0 = bitcast i64 12345678901234 to double
  ret double %v0
}
; CHECK-LABEL: cast_ll2d_const
; CHECK: mov {{.*}}, 1942892530
; CHECK: mov {{.*}}, 2874
; CHECK: fld qword ptr
; CHECK: ret

; ERRORS-NOT: ICE translation error
; DUMP-NOT: SZ
