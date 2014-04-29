; RUIN: %llvm2ice -verbose inst %s | FileCheck %s
; RUIN: %llvm2ice --verbose none %s | FileCheck --check-prefix=ERRORS %s
; RUN: %szdiff --llvm2ice=%llvm2ice %s | FileCheck --check-prefix=DUMP %s

define internal i32 @cast_f2i(float %f) {
entry:
  %v0 = bitcast float %f to i32
  ret i32 %v0
}

define internal float @cast_i2f(i32 %i) {
entry:
  %v0 = bitcast i32 %i to float
  ret float %v0
}

define internal i64 @cast_d2ll(double %d) {
entry:
  %v0 = bitcast double %d to i64
  ret i64 %v0
}

define internal double @cast_ll2d(i64 %ll) {
entry:
  %v0 = bitcast i64 %ll to double
  ret double %v0
}

; ERRORS-NOT: ICE translation error
; DUMP-NOT: SZ
