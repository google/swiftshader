; Show that we know how to translate move (immediate) ARM instruction.

; RUN: %p2i --filetype=asm -i %s --target=arm32 \
; RUN:   | FileCheck %s --check-prefix=ASM
; RUN: %p2i --filetype=iasm -i %s --target=arm32 \
; RUN:   | FileCheck %s --check-prefix=IASM

define internal i32 @Imm1() {
  ret i32 1
}

; ASM-LABEL: Imm1:
; ASM: mov	r0, #1
; IASM-LABEL: Imm1:
; IASM:	.byte 0x1
; IASM:	.byte 0x0
; IASM:	.byte 0xa0
; IASM:	.byte 0xe3


define internal i32 @rotateFImmAA() {
  ; immediate = 0x000002a8 = b 0000 0000 0000 0000 0000 0010 1010 1000
  ret i32 680 
}

; ASM-LABEL: rotateFImmAA:
; ASM: mov	r0, #680

; IASM-LABEL: rotateFImmAA:
; IASM:	.byte 0xaa
; IASM:	.byte 0xf
; IASM:	.byte 0xa0
; IASM:	.byte 0xe3

define internal i32 @rotateEImmAA() {
 ; immediate = 0x00000aa0 = b 0000 0000 0000 0000 0000 1010 1010 0000
  ret i32 2720
}

; ASM-LABEL: rotateEImmAA:
; ASM: mov	r0, #2720

; IASM-LABEL: rotateEImmAA:
; IASM:	.byte 0xaa
; IASM:	.byte 0xe
; IASM:	.byte 0xa0
; IASM:	.byte 0xe3

define internal i32 @rotateDImmAA() {
  ; immediate = 0x00002a80 = b 0000 0000 0000 0000 0010 1010 1000 0000
  ret i32 10880 
}

; ASM-LABEL: rotateDImmAA:
; ASM: mov	r0, #10880

; IASM-LABEL: rotateDImmAA:
; IASM:	.byte 0xaa
; IASM:	.byte 0xd
; IASM:	.byte 0xa0
; IASM:	.byte 0xe3

define internal i32 @rotateCImmAA() {
  ; immediate = 0x0000aa00 = b 0000 0000 0000 0000 1010 1010 0000 0000
  ret i32 43520 
}

; ASM-LABEL: rotateCImmAA:
; ASM: mov	r0, #43520

; IASM-LABEL: rotateCImmAA:
; IASM:	.byte 0xaa
; IASM:	.byte 0xc
; IASM:	.byte 0xa0
; IASM:	.byte 0xe3

define internal i32 @rotateBImmAA() {
  ; immediate = 0x0002a800 = b 0000 0000 0000 0010 1010 1000 0000 0000
  ret i32 174080 
}

; ASM-LABEL: rotateBImmAA:
; ASM: mov	r0, #174080

; IASM-LABEL: rotateBImmAA:
; IASM:	.byte 0xaa
; IASM:	.byte 0xb
; IASM:	.byte 0xa0
; IASM:	.byte 0xe3

define internal i32 @rotateAImmAA() {
  ; immediate = 0x000aa000 = b 0000 0000 0000 1010 1010 0000 0000 0000
  ret i32 696320 
}

; ASM-LABEL: rotateAImmAA:
; ASM: mov	r0, #696320

; IASM-LABEL: rotateAImmAA:
; IASM:	.byte 0xaa
; IASM:	.byte 0xa
; IASM:	.byte 0xa0
; IASM:	.byte 0xe3

define internal i32 @rotate9ImmAA() {
  ; immediate = 0x002a8000 = b 0000 0000 0010 1010 1000 0000 0000 0000
  ret i32 2785280
}

; ASM-LABEL: rotate9ImmAA:
; ASM: mov	r0, #2785280

; IASM-LABEL: rotate9ImmAA:
; IASM:	.byte 0xaa
; IASM:	.byte 0x9
; IASM:	.byte 0xa0
; IASM:	.byte 0xe3

define internal i32 @rotate8ImmAA() {
  ; immediate = 0x00aa0000 = b 0000 0000 1010 1010 0000 0000 0000 0000
  ret i32 11141120
}

; ASM-LABEL: rotate8ImmAA:
; ASM: mov	r0, #11141120

; IASM-LABEL: rotate8ImmAA:
; IASM:	.byte 0xaa
; IASM:	.byte 0x8
; IASM:	.byte 0xa0
; IASM:	.byte 0xe3

define internal i32 @rotate7ImmAA() {
  ; immediate = 0x02a80000 = b 0000 0010 1010 1000 0000 0000 0000 0000
  ret i32 44564480
}

; ASM-LABEL: rotate7ImmAA:
; ASM: 	mov	r0, #44564480

; IASM-LABEL: rotate7ImmAA:
; IASM:	.byte 0xaa
; IASM:	.byte 0x7
; IASM:	.byte 0xa0
; IASM:	.byte 0xe3

define internal i32 @rotate6ImmAA() {
  ; immediate = 0x0aa00000 = b 0000 1010 1010 0000 0000 0000 0000 0000
  ret i32 178257920
}

; ASM-LABEL: rotate6ImmAA:
; ASM: 	mov	r0, #178257920

; IASM-LABEL: rotate6ImmAA:
; IASM:	.byte 0xaa
; IASM:	.byte 0x6
; IASM:	.byte 0xa0
; IASM:	.byte 0xe3

define internal i32 @rotate5ImmAA() {
  ; immediate = 0x2a800000 = b 0010 1010 1000 0000 0000 0000 0000 0000
  ret i32 713031680
}

; ASM-LABEL: rotate5ImmAA:
; ASM: 	mov	r0, #713031680

; IASM-LABEL: rotate5ImmAA:
; IASM:	.byte 0xaa
; IASM:	.byte 0x5
; IASM:	.byte 0xa0
; IASM:	.byte 0xe3

define internal i32 @rotate4ImmAA() {
  ; immediate = 0xaa000000 = b 1010 1010 0000 0000 0000 0000 0000 0000
  ret i32 2852126720
}

; ASM-LABEL: rotate4ImmAA:
; ASM: mov	r0, #2852126720

; IASM-LABEL: rotate4ImmAA:
; IASM:	.byte 0xaa
; IASM:	.byte 0x4
; IASM:	.byte 0xa0
; IASM:	.byte 0xe3

define internal i32 @rotate3ImmAA() {
  ; immediate = 0xa8000002 = b 1010 1000 0000 0000 0000 0000 0000 0010
  ret i32 2818572290
}

; ASM-LABEL: rotate3ImmAA:
; ASM: mov	r0, #2818572290

; IASM-LABEL: rotate3ImmAA:
; IASM:	.byte 0xaa
; IASM:	.byte 0x3
; IASM:	.byte 0xa0
; IASM:	.byte 0xe3

define internal i32 @rotate2ImmAA() {
  ; immediate = 0xa000000a = b 1010 0000 0000 0000 0000 0000 0000 1010
  ret i32 2684354570
}

; ASM-LABEL: rotate2ImmAA:
; ASM: 	mov	r0, #2684354570

; IASM-LABEL: rotate2ImmAA:
; IASM:	.byte 0xaa
; IASM:	.byte 0x2
; IASM:	.byte 0xa0
; IASM:	.byte 0xe3

define internal i32 @rotate1ImmAA() {
  ; immediate = 0x8000002a = b 1000 1000 0000 0000 0000 0000 0010 1010
  ret i32 2147483690
}

; ASM-LABEL: rotate1ImmAA:
; ASM: mov	r0, #2147483690

; IASM-LABEL: rotate1ImmAA:
; IASM:	.byte 0xaa
; IASM:	.byte 0x1
; IASM:	.byte 0xa0
; IASM:	.byte 0xe3

define internal i32 @rotate0ImmAA() {
  ; immediate = 0x000000aa = b 0000 0000 0000 0000 0000 0000 1010 1010
  ret i32 170
}

; ASM-LABEL: rotate0ImmAA:
; ASM: mov	r0, #170

; IASM-LABEL: rotate0ImmAA:
; IASM:	.byte 0xaa
; IASM:	.byte 0x0
; IASM:	.byte 0xa0
; IASM:	.byte 0xe3
