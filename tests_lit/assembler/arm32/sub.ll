; Show that we know how to translate instruction sub.

; NOTE: We use -O2 to get rid of memory stores.

; REQUIRES: allow_dump

; RUN: %p2i --filetype=asm -i %s --target=arm32 --args -O2 \
; RUN:   | FileCheck %s --check-prefix=ASM
; RUN: %p2i --filetype=iasm -i %s --target=arm32 --args -O2 \
; RUN:   | FileCheck %s --check-prefix=IASM

define internal i32 @sub1FromR0(i32 %p) {
  %v = sub i32 %p, 1
  ret i32 %v
}

; ASM-LABEL: sub1FromR0:
; ASM:	sub	r0, r0, #1
; ASM:	bx	lr

; IASM-LABEL: sub1FromR0:
; IASM:	     .byte 0x1
; IASM-NEXT: .byte 0x0
; IASM-NEXT: .byte 0x40
; IASM-NEXT: .byte 0xe2


define internal i32 @Sub2Regs(i32 %p1, i32 %p2) {
  %v = sub i32 %p1, %p2
  ret i32 %v
}

; ASM-LABEL: Sub2Regs:
; ASM:       sub r0, r0, r1
; ASM-NEXT:  bx lr

; IASM-LABEL: Sub2Regs:

; IASM:      .byte 0x1
; IASM-NEXT: .byte 0x0
; IASM-NEXT: .byte 0x40
; IASM-NEXT: .byte 0xe0

