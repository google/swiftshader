; Shows that the ARM integrated assembler can translate a trivial,
; bundle-aligned function.

; REQUIRES: allow_dump

; RUN: %p2i --filetype=asm -i %s --target=arm32 \
; RUN:   | FileCheck %s --check-prefix=ASM
; RUN: %p2i --filetype=iasm -i %s --target=arm32 \
; RUN:   | FileCheck %s --check-prefix=IASM

define internal void @f() {
  ret void
}

; ASM-LABEL:f:
; ASM-NEXT: .Lf$__0:
; ASM-NEXT: 	bx	lr

; IASM-LABEL:f:
; IASM-NEXT: 	.byte 0x1e
; IASM-NEXT: 	.byte 0xff
; IASM-NEXT: 	.byte 0x2f
; IASM-NEXT: 	.byte 0xe1

; IASM-NEXT: 	.byte 0x70
; IASM-NEXT: 	.byte 0x0
; IASM-NEXT: 	.byte 0x20
; IASM-NEXT: 	.byte 0xe1

; IASM-NEXT: 	.byte 0x70
; IASM-NEXT: 	.byte 0x0
; IASM-NEXT: 	.byte 0x20
; IASM-NEXT: 	.byte 0xe1

; IASM-NEXT: 	.byte 0x70
; IASM-NEXT: 	.byte 0x0
; IASM-NEXT: 	.byte 0x20
; IASM-NEXT: 	.byte 0xe1
