; Show that we know how to translate mul.

; NOTE: We use -O2 to get rid of memory stores.

; REQUIRES: allow_dump

; Compile using standalone assembler.
; RUN: %p2i --filetype=asm -i %s --target=arm32 --args -O2 \
; RUN:   | FileCheck %s --check-prefix=ASM

; Show bytes in assembled standalone code.
; RUN: %p2i --filetype=asm -i %s --target=arm32 --assemble --disassemble \
; RUN:   --args -O2 | FileCheck %s --check-prefix=DIS

; Compile using integrated assembler.
; RUN: %p2i --filetype=iasm -i %s --target=arm32 --args -O2 \
; RUN:   | FileCheck %s --check-prefix=IASM

; Show bytes in assembled integrated code.
; RUN: %p2i --filetype=iasm -i %s --target=arm32 --assemble --disassemble \
; RUN:   --args -O2 | FileCheck %s --check-prefix=DIS

define internal i32 @MulTwoRegs(i32 %a, i32 %b) {
  %v = mul i32 %a, %b
  ret i32 %v
}

; ASM-LABEL:MulTwoRegs:
; ASM-NEXT:.LMulTwoRegs$__0:
; ASM-NEXT:     mul     r0, r0, r1

; DIS-LABEL:00000000 <MulTwoRegs>:
; DIS-NEXT:   0:        e0000190

; IASM-LABEL:MulTwoRegs:
; IASM-NEXT:.LMulTwoRegs$__0:
; IASM-NEXT:    .byte 0x90
; IASM-NEXT:    .byte 0x1
; IASM-NEXT:    .byte 0x0
; IASM-NEXT:    .byte 0xe0

define internal i64 @MulTwoI64Regs(i64 %a, i64 %b) {
  %v = mul i64 %a, %b
  ret i64 %v
}

; ASM-LABEL:MulTwoI64Regs:
; ASM-NEXT:.LMulTwoI64Regs$__0:
; ASM-NEXT:     mul     r3, r0, r3
; ASM-NEXT:     mla     r1, r2, r1, r3
; ASM-NEXT:     umull   r0, r2, r0, r2
; ASM:          add     r2, r2, r1
; ASM-NEXT:     mov     r1, r2
; ASM:          bx      lr


; DIS-LABEL:00000010 <MulTwoI64Regs>:
; DIS-NEXT:  10:        e0030390
; DIS-NEXT:  14:        e0213192
; DIS-NEXT:  18:        e0820290
; DIS-NEXT:  1c:        e0822001
; DIS-NEXT:  20:        e1a01002
; DIS-NEXT:  24:        e12fff1e

; IASM-LABEL:MulTwoI64Regs:
; IASM-NEXT:.LMulTwoI64Regs$__0:
; IASM-NEXT:    .byte 0x90
; IASM-NEXT:    .byte 0x3
; IASM-NEXT:    .byte 0x3
; IASM-NEXT:    .byte 0xe0
; IASM-NEXT:    mla     r1, r2, r1, r3
; IASM-NEXT:    umull   r0, r2, r0, r2
; IASM-NEXT:    .byte 0x1
; IASM-NEXT:    .byte 0x20
; IASM-NEXT:    .byte 0x82
; IASM-NEXT:    .byte 0xe0
; IASM-NEXT:    mov     r1, r2
; IASM-NEXT:    .byte 0x1e
; IASM-NEXT:    .byte 0xff
; IASM-NEXT:    .byte 0x2f
; IASM-NEXT:    .byte 0xe1
