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

define internal <8 x i16> @testDeref8i16(<8 x i16> *%p) {
; ASM-LABEL: testDeref8i16:
; DIS-LABEL: 00000020 <testDeref8i16>:
; IASM-LABEL: testDeref8i16:

entry:
  %ret = load <8 x i16>, <8 x i16>* %p, align 2
; ASM:     vld1.64	q0, [r0]
; DIS:   20:       f4200acf

  ret <8 x i16> %ret
}

define internal <16 x i8> @testDeref16i8(<16 x i8> *%p) {
; ASM-LABEL: testDeref16i8:
; DIS-LABEL: 00000030 <testDeref16i8>:
; IASM-LABEL: testDeref16i8:

entry:
  %ret = load <16 x i8>, <16 x i8>* %p, align 1
; ASM:     vld1.64	q0, [r0]
; DIS:   30:       f4200acf

  ret <16 x i8> %ret
}
