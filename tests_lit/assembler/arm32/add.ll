; Show that we know how to translate add.

; NOTE: We use -O2 to get rid of memory stores.

; REQUIRES: allow_dump

; RUN: %p2i --filetype=asm -i %s --target=arm32 --args -O2 \
; RUN:   | FileCheck %s --check-prefix=ASM
; RUN: %p2i --filetype=iasm -i %s --target=arm32 --args -O2 \
; RUN:   | FileCheck %s --check-prefix=IASM

define internal i32 @add1ToR0(i32 %p) {
  %v = add i32 %p, 1
  ret i32 %v
}

; ASM-LABEL: add1ToR0:
; ASM:       add     r0, r0, #1
; ASM-NEXT:  bx      lr

; IASM-LABEL: add1ToR0:
; IASM:      .byte 0x1
; IASM-NEXT: .byte 0x0
; IASM-NEXT: .byte 0x80
; IASM-NEXT: .byte 0xe2

define internal i32 @Add2Regs(i32 %p1, i32 %p2) {
  %v = add i32 %p1, %p2
  ret i32 %v
}

; ASM-LABEL: Add2Regs:
; ASM:       add r0, r0, r1
; ASM-NEXT:  bx lr

; IASM-LABEL: Add2Regs:

; IASM:      .byte 0x1
; IASM-NEXT: .byte 0x0
; IASM-NEXT: .byte 0x80
; IASM-NEXT: .byte 0xe0
