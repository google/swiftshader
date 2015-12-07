; Show that we know how to translate lsr.

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

define internal i32 @LshrAmt(i32 %a) {
; ASM-LABEL:LshrAmt:
; DIS-LABEL:00000000 <LshrAmt>:
; IASM-LABEL:LshrAmt:

entry:
; ASM-NEXT:.LLshrAmt$entry:
; IASM-NEXT:.LLshrAmt$entry:

  %v = lshr i32 %a, 23

; ASM-NEXT:     lsr     r0, r0, #23
; DIS-NEXT:   0:        e1a00ba0
; IASM-NEXT:    .byte 0xa0
; IASM-NEXT:    .byte 0xb
; IASM-NEXT:    .byte 0xa0
; IASM-NEXT:    .byte 0xe1

  ret i32 %v
}

define internal i32 @LshrReg(i32 %a, i32 %b) {
; ASM-LABEL:LshrReg:
; DIS-LABEL:00000010 <LshrReg>:
; IASM-LABEL:LshrReg:

entry:
; ASM-NEXT:.LLshrReg$entry:
; IASM-NEXT:.LLshrReg$entry:

  %v = lshr i32 %a, %b

; ASM-NEXT:     lsr     r0, r0, r1
; DIS-NEXT:  10:        e1a00130
; IASM-NEXT:    .byte 0x30
; IASM-NEXT:    .byte 0x1
; IASM-NEXT:    .byte 0xa0
; IASM-NEXT:    .byte 0xe1

  ret i32 %v
}
