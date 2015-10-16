; Show that we can handle variable (i.e. stack) spills.

; RUN: %p2i --filetype=asm -i %s --target=arm32 --args -Om1 \
; RUN:   | FileCheck %s --check-prefix=ASM
; RUN: %p2i --filetype=iasm -i %s --target=arm32 --args -Om1 \
; RUN:   | FileCheck %s --check-prefix=IASM

define internal i32 @add1ToR0(i32 %p) {
  %v = add i32 %p, 1
  ret i32 %v
}

; ASM-LABEL: add1ToR0:
; IASM-LABEL: add1ToR0:

; ASM:          sub     sp, sp, #8
; IASM:         .byte 0x8
; IASM-NEXT:    .byte 0xd0
; IASM-NEXT:    .byte 0x4d
; IASM-NEXT:    .byte 0xe2

; ASM-NEXT:     str     r0, [sp, #4]
; IASM-NEXT:    .byte 0x4
; IASM-NEXT:    .byte 0x0
; IASM-NEXT:    .byte 0x8d
; IASM-NEXT:    .byte 0xe5

; ASM-NEXT:     ldr     r0, [sp, #4]
; IASM-NEXT:    .byte 0x4
; IASM-NEXT:    .byte 0x0
; IASM-NEXT:    .byte 0x9d
; IASM-NEXT:    .byte 0xe5

; ASM-NEXT:     add     r0, r0, #1
; IASM-NEXT:    .byte 0x1
; IASM-NEXT:    .byte 0x0
; IASM-NEXT:    .byte 0x80
; IASM-NEXT:    .byte 0xe2

; ASM-NEXT:     str     r0, [sp]
; IASM-NEXT:    .byte 0x0
; IASM-NEXT:    .byte 0x0
; IASM-NEXT:    .byte 0x8d
; IASM-NEXT:    .byte 0xe5

; ASM-NEXT:     ldr     r0, [sp]
; IASM-NEXT:    .byte 0x0
; IASM-NEXT:    .byte 0x0
; IASM-NEXT:    .byte 0x9d
; IASM-NEXT:    .byte 0xe5

; ASM-NEXT:     add     sp, sp, #8
; IASM-NEXT:    .byte 0x8
; IASM-NEXT:    .byte 0xd0
; IASM-NEXT:    .byte 0x8d
; IASM-NEXT:    .byte 0xe2

; ASM-NEXT:     bx      lr
; IASM-NEXT:	.byte 0x1e
; IASM-NEXT:	.byte 0xff
; IASM-NEXT:	.byte 0x2f
; IASM-NEXT:	.byte 0xe1

