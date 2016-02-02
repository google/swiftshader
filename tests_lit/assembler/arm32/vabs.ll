; Show that we translate intrinsics for fabs on float and double.

; REQUIRES: allow_dump

; Compile using standalone assembler.
; RUN: %p2i --filetype=asm -i %s --target=arm32 --args -Om1 \
; RUN:   -reg-use s20,d22 \
; RUN:   | FileCheck %s --check-prefix=ASM

; Show bytes in assembled standalone code.
; RUN: %p2i --filetype=asm -i %s --target=arm32 --assemble --disassemble \
; RUN:   --args -Om1 \
; RUN:   -reg-use s20,d22 \
; RUN:   | FileCheck %s --check-prefix=DIS

; Compile using integrated assembler.
; RUN: %p2i --filetype=iasm -i %s --target=arm32 --args -Om1 \
; RUN:   -reg-use s20,d22 \
; RUN:   | FileCheck %s --check-prefix=IASM

; Show bytes in assembled integrated code.
; RUN: %p2i --filetype=iasm -i %s --target=arm32 --assemble --disassemble \
; RUN:   --args -Om1 \
; RUN:   -reg-use s20,d22 \
; RUN:   | FileCheck %s --check-prefix=DIS

declare float @llvm.fabs.f32(float)
declare double @llvm.fabs.f64(double)

define internal float @test_fabs_float(float %x) {
; ASM-LABEL: test_fabs_float:
; DIS-LABEL: 00000000 <test_fabs_float>:
; IASM-LABEL: test_fabs_float:

entry:
  %r = call float @llvm.fabs.f32(float %x)

; ASM:  vabs.f32        s20, s20
; DIS:   10:    eeb0aaca
; IASM-NOT: vabs.f32

  ret float %r
}

define internal double @test_fabs_double(double %x) {
; ASM-LABEL: test_fabs_double:
; DIS-LABEL: 00000030 <test_fabs_double>:
; IASM-LABEL: test_fabs_double:

entry:
  %r = call double @llvm.fabs.f64(double %x)

; ASM:  vabs.f64        d22, d22
; DIS:   3c:    eef06be6
; IASM-NOT: vabs.64

  ret double %r
}
