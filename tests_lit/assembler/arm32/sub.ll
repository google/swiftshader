; Show that we know how to translate instruction sub.
; TODO(kschimpf) Currently only know how to test subtract 1 from R0.

; NOTE: We use -O2 to get rid of memory stores.

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

