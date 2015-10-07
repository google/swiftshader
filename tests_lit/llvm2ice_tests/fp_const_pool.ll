; This tests that different floating point constants (such as 0.0 and -0.0)
; remain distinct even when they sort of look equal, and also that different
; instances of the same floating point constant (such as NaN and NaN) get the
; same constant pool entry even when "a==a" would suggest they are different.

; REQUIRES: allow_dump

define internal void @consume_float(float %f) {
  ret void
}

define internal void @consume_double(double %d) {
  ret void
}

define internal void @test_zeros() {
entry:
  call void @consume_float(float 0.0)
  call void @consume_float(float -0.0)
  call void @consume_double(double 0.0)
  call void @consume_double(double -0.0)
  ret void
}
; Parse the function, dump the bitcode back out, and stop without translating.
; This tests that +0.0 and -0.0 aren't accidentally merged into a single
; zero-valued constant pool entry.
;
; RUN: %p2i -i %s --insts | FileCheck --check-prefix=ZERO %s
; ZERO: test_zeros
; ZERO-NEXT: entry:
; ZERO-NEXT: call void @consume_float(float 0.0
; ZERO-NEXT: call void @consume_float(float -0.0
; ZERO-NEXT: call void @consume_double(double 0.0
; ZERO-NEXT: call void @consume_double(double -0.0


define internal void @test_nans() {
entry:
  call void @consume_float(float 0x7FF8000000000000)
  call void @consume_float(float 0x7FF8000000000000)
  call void @consume_float(float 0xFFF8000000000000)
  call void @consume_float(float 0xFFF8000000000000)
  call void @consume_double(double 0x7FF8000000000000)
  call void @consume_double(double 0x7FF8000000000000)
  call void @consume_double(double 0xFFF8000000000000)
  call void @consume_double(double 0xFFF8000000000000)
  ret void
}
; The following tests check the emitted constant pool entries and make sure
; there is at most one entry for each NaN value.  We have to run a separate test
; for each NaN because the constant pool entries may be emitted in any order.
;
; RUN: %p2i -i %s --filetype=asm --llvm-source \
; RUN:   | FileCheck --check-prefix=NANS1 %s
; NANS1: float nan
; NANS1-NOT: float nan
;
; RUN: %p2i -i %s --filetype=asm --llvm-source \
; RUN:   | FileCheck --check-prefix=NANS2 %s
; NANS2: float -nan
; NANS2-NOT: float -nan
;
; RUN: %p2i -i %s --filetype=asm --llvm-source \
; RUN:   | FileCheck --check-prefix=NANS3 %s
; NANS3: double nan
; NANS3-NOT: double nan
;
; RUN: %p2i -i %s --filetype=asm --llvm-source \
; RUN:   | FileCheck --check-prefix=NANS4 %s
; NANS4: double -nan
; NANS4-NOT: double -nan
