; Test that we handle a vector move.

; NOTE: We use -O2 to force a vector move for the return value.

; REQUIRES: allow_dump

; Compile using standalone assembler.
; RUN: %p2i --filetype=asm -i %s --target=arm32 --args -O2 \
; RUN:   | FileCheck %s --check-prefix=ASM

; Show bytes in assembled standalone code.
; RUN: %p2i --filetype=asm -i %s --target=arm32 --assemble --disassemble \
; RUN:   --args -O2 \
; RUN:   | FileCheck %s --check-prefix=DIS

; Compile using integrated assembler.
; RUN: %p2i --filetype=iasm -i %s --target=arm32 --args -O2 \
; RUN:   | FileCheck %s --check-prefix=IASM

; Show bytes in assembled integrated code.
; RUN: %p2i --filetype=iasm -i %s --target=arm32 --assemble --disassemble \
; RUN:   --args -O2 \
; RUN:   | FileCheck %s --check-prefix=DIS


define internal <4 x float> @testMoveVector(<4 x i32> %a) {
; ASM-LABEL: testMoveVector:
; DIS-LABEL:{{.+}} <testMoveVector>:
; IASM-LABEL: testMoveVector:

entry:
  %0 = sitofp <4 x i32> %a to <4 x float>
  ret <4 x float> %0

; ASM:  vmov.f32        q0, q1
; DIS:  3c:     eef03a40
; IASM-NOT: vmov.f32    q0, q1

}
