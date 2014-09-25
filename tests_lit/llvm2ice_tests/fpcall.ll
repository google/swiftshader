; Test that for calls returning a floating-point value, the calling
; ABI with respect to the x87 floating point stack is honored.  In
; particular, the top-of-stack must be popped regardless of whether
; its value is used.

; RUN: %p2i -i %s --args -O2 --verbose none \
; RUN:   | llvm-mc -triple=i686-none-nacl -x86-asm-syntax=intel -filetype=obj \
; RUN:   | llvm-objdump -d --symbolize -x86-asm-syntax=intel - | FileCheck %s
; RUN: %p2i -i %s --args -Om1 --verbose none \
; RUN:   | llvm-mc -triple=i686-none-nacl -x86-asm-syntax=intel -filetype=obj \
; RUN:   | llvm-objdump -d --symbolize -x86-asm-syntax=intel - | FileCheck %s
; RUN: %p2i -i %s --args --verbose none | FileCheck --check-prefix=ERRORS %s
; RUN: %p2i -i %s --insts | %szdiff %s | FileCheck --check-prefix=DUMP %s

define float @dummy() {
entry:
  ret float 0.000000e+00
}
; CHECK-LABEL: dummy

; The call is ignored, but the top of the FP stack still needs to be
; popped.
define i32 @ignored_fp_call() {
entry:
  %ignored = call float @dummy()
  ret i32 0
}
; CHECK-LABEL: ignored_fp_call
; CHECK: call dummy
; CHECK: fstp

; The top of the FP stack is popped and subsequently used.
define i32 @converted_fp_call() {
entry:
  %fp = call float @dummy()
  %ret = fptosi float %fp to i32
  ret i32 %ret
}
; CHECK-LABEL: converted_fp_call
; CHECK: call dummy
; CHECK: fstp
; CHECK: cvttss2si

; The top of the FP stack is ultimately passed through as the return
; value.  Note: the translator could optimized by not popping and
; re-pushing, in which case the test would need to be changed.
define float @returned_fp_call() {
entry:
  %fp = call float @dummy()
  ret float %fp
}
; CHECK-LABEL: returned_fp_call
; CHECK: call dummy
; CHECK: fstp
; CHECK: fld

; ERRORS-NOT: ICE translation error
; DUMP-NOT: SZ
