; Trivial smoke test of bitcast between integer and FP types.

; RUN: %llvm2ice -O2 --verbose none %s | FileCheck %s
; RUN: %llvm2ice --verbose none %s | FileCheck --check-prefix=ERRORS %s
; RUN: %llvm2iceinsts %s | %szdiff %s | FileCheck --check-prefix=DUMP %s
; RUN: %llvm2iceinsts --pnacl %s | %szdiff %s \
; RUN:                           | FileCheck --check-prefix=DUMP %s

define internal i32 @cast_f2i(float %f) {
entry:
  %v0 = bitcast float %f to i32
  ret i32 %v0
}

; CHECK: mov eax
; CHECK: ret

define internal float @cast_i2f(i32 %i) {
entry:
  %v0 = bitcast i32 %i to float
  ret float %v0
}

; CHECK: fld dword ptr
; CHECK: ret

define internal i64 @cast_d2ll(double %d) {
entry:
  %v0 = bitcast double %d to i64
  ret i64 %v0
}

; CHECK: mov edx
; CHECK: ret

define internal double @cast_ll2d(i64 %ll) {
entry:
  %v0 = bitcast i64 %ll to double
  ret double %v0
}

; CHECK: fld qword ptr
; CHECK: ret

; ERRORS-NOT: ICE translation error
; DUMP-NOT: SZ
