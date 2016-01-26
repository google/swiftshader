; Show that we know how to translate vector load instructions.

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

define internal <4 x float> @testDerefFloat4(<4 x float> *%p) {
; ASM-LABEL: testDerefFloat4:
; DIS-LABEL: 00000000 <testDerefFloat4>:
; IASM-LABEL: testDerefFloat4:

entry:
  %ret = load <4 x float>, <4 x float>* %p, align 4
; ASM:     vld1.64	q0, [r0]
; DIS:   0:       f4200acf

  ret <4 x float> %ret
}

define internal <4 x i32> @testDeref4i32(<4 x i32> *%p) {
; ASM-LABEL: testDeref4i32:
; DIS-LABEL: 00000010 <testDeref4i32>:
; IASM-LABEL: testDeref4i32:

entry:
  %ret = load <4 x i32>, <4 x i32>* %p, align 4
; ASM:     vld1.64	q0, [r0]
; DIS:   10:       f4200acf

  ret <4 x i32> %ret
}
