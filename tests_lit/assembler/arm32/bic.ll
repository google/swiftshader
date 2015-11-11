; Show that we know how to translate bic.

; NOTE: We use -O2 to get rid of memory stores.

; REQUIRES: allow_dump

; Compile using standalone assembler.
; RUN: %p2i --filetype=asm -i %s --target=arm32 --args -O2 \
; RUN:   | FileCheck %s --check-prefix=ASM

; Show bytes in assembled standalone code.
; RUN: %p2i --filetype=asm -i %s --target=arm32 --assemble --disassemble \
; RUN:   --args -O2 | FileCheck %s --check-prefix=DIS

; Compile using integrated assembler.
; RUN: %p2i --filetype=iasm -i %s --target=arm32 --args -O2 -unsafe-ias \
; RUN:   | FileCheck %s --check-prefix=IASM

; Show bytes in assembled integrated code.
; RUN: %p2i --filetype=iasm -i %s --target=arm32 --assemble --disassemble \
; RUN:   --args -O2 -unsafe-ias | FileCheck %s --check-prefix=DIS

define internal i32 @AllocBigAlign() {
  %addr = alloca i8, align 32
  %v = ptrtoint i8* %addr to i32
  ret i32 %v
}

; ASM-LABEL:AllocBigAlign:
; ASM-NEXT:.LAllocBigAlign$__0:
; ASM-NEXT:  push    {fp}
; ASM-NEXT:  mov     fp, sp
; ASM-NEXT:  sub     sp, sp, #12
; ASM-NEXT:  bic     sp, sp, #31
; ASM-NEXT:  sub     sp, sp, #32
; ASM-NEXT:  mov     r0, sp
; ASM-NEXT:  mov     sp, fp
; ASM-NEXT:  pop     {fp}
; ASM-NEXT:  # fp = def.pseudo 
; ASM-NEXT:  bx      lr

; DIS-LABEL:00000000 <AllocBigAlign>:
; DIS-NEXT:   0:        e52db004
; DIS-NEXT:   4:        e1a0b00d
; DIS-NEXT:   8:        e24dd00c
; DIS-NEXT:   c:        e3cdd01f
; DIS-NEXT:  10:        e24dd020
; DIS-NEXT:  14:        e1a0000d
; DIS-NEXT:  18:        e1a0d00b
; DIS-NEXT:  1c:        e49db004
; DIS-NEXT:  20:        e12fff1e

; IASM-LABEL:AllocBigAlign:
; IASM-NEXT:.LAllocBigAlign$__0:
; IASM-NEXT:    .byte 0x4
; IASM-NEXT:    .byte 0xb0
; IASM-NEXT:    .byte 0x2d
; IASM-NEXT:    .byte 0xe5

; IASM:         .byte 0xd
; IASM-NEXT:    .byte 0xb0
; IASM-NEXT:    .byte 0xa0
; IASM-NEXT:    .byte 0xe1

; IASM:         .byte 0xc
; IASM-NEXT:    .byte 0xd0
; IASM-NEXT:    .byte 0x4d
; IASM-NEXT:    .byte 0xe2

; IASM:         .byte 0x1f
; IASM-NEXT:    .byte 0xd0
; IASM-NEXT:    .byte 0xcd
; IASM-NEXT:    .byte 0xe3

; IASM:         .byte 0x20
; IASM-NEXT:    .byte 0xd0
; IASM-NEXT:    .byte 0x4d
; IASM-NEXT:    .byte 0xe2

; IASM:         .byte 0xd
; IASM-NEXT:    .byte 0x0
; IASM-NEXT:    .byte 0xa0
; IASM-NEXT:    .byte 0xe1

; IASM:         .byte 0xb
; IASM-NEXT:    .byte 0xd0
; IASM-NEXT:    .byte 0xa0
; IASM-NEXT:    .byte 0xe1

; IASM-NEXT:    .byte 0x4
; IASM-NEXT:    .byte 0xb0
; IASM-NEXT:    .byte 0x9d
; IASM-NEXT:    .byte 0xe4

; IASM:         .byte 0x1e
; IASM-NEXT:    .byte 0xff
; IASM-NEXT:    .byte 0x2f
; IASM-NEXT:    .byte 0xe1
