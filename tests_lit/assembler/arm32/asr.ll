; Show that we know how to translate asr

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

define internal i32 @AshrAmt(i32 %a) {
; ASM-LABEL:AshrAmt:
; DIS-LABEL:00000000 <AshrAmt>:
; IASM-LABEL:AshrAmt:

entry:
; ASM-NEXT:.LAshrAmt$entry:
; IASM-NEXT:.LAshrAmt$entry:

  %v = ashr i32 %a, 23

; ASM-NEXT:     asr     r0, r0, #23
; DIS-NEXT:   0:	e1a00bc0
; IASM-NEXT:	.byte 0xc0
; IASM-NEXT:	.byte 0xb
; IASM-NEXT:	.byte 0xa0
; IASM-NEXT:	.byte 0xe1

  ret i32 %v
}

define internal i32 @AshrReg(i32 %a, i32 %b) {
; ASM-LABEL:AshrReg:
; DIS-LABEL:00000010 <AshrReg>:
; IASM-LABEL:AshrReg:

entry:
; ASM-NEXT:.LAshrReg$entry:
; IASM-NEXT:.LAshrReg$entry:

  %v = ashr i32 %a, %b

; ASM-NEXT:     asr     r0, r0, r1
; DIS-NEXT:  10:	e1a00150
; IASM-NEXT:	.byte 0x50
; IASM-NEXT:	.byte 0x1
; IASM-NEXT:	.byte 0xa0
; IASM-NEXT:	.byte 0xe1

  ret i32 %v
}
