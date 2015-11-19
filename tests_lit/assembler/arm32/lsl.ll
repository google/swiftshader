; Show that we know how to translate lsl.

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

define internal i32 @_Z8testUdivhh(i32 %a, i32 %b) {

; ASM-LABEL:_Z8testUdivhh:
; DIS-LABEL:00000000 <_Z8testUdivhh>:
; IASM-LABEL:_Z8testUdivhh:

entry:

; ASM-NEXT:.L_Z8testUdivhh$entry:
; ASM-NEXT:     push    {lr}
; DIS-NEXT:   0:        e52de004
; IASM-NEXT:.L_Z8testUdivhh$entry:
; IASM-NEXT:    .byte 0x4
; IASM-NEXT:    .byte 0xe0
; IASM-NEXT:    .byte 0x2d
; IASM-NEXT:    .byte 0xe5

  %b.arg_trunc = trunc i32 %b to i8
  %a.arg_trunc = trunc i32 %a to i8
  %div3 = udiv i8 %a.arg_trunc, %b.arg_trunc

; ASM-NEXT:     sub     sp, sp, #12
; DIS-NEXT:   4:        e24dd00c
; IASM-NEXT:    .byte 0xc
; IASM-NEXT:    .byte 0xd0
; IASM-NEXT:    .byte 0x4d
; IASM-NEXT:    .byte 0xe2

; ASM-NEXT:     lsls    r2, r1, #24
; DIS-NEXT:   8:        e1b02c01
; IASM-NEXT:    .byte 0x1
; IASM-NEXT:    .byte 0x2c
; IASM-NEXT:    .byte 0xb0
; IASM-NEXT:    .byte 0xe1

  %div3.ret_ext = zext i8 %div3 to i32
  ret i32 %div3.ret_ext
}

define internal i32 @_Z7testShljj(i32 %a, i32 %b) {

; ASM-LABEL:_Z7testShljj:
; DIS-LABEL:00000030 <_Z7testShljj>:
; IASM-LABEL:_Z7testShljj:

entry:

; ASM-NEXT:.L_Z7testShljj$entry:
; IASM-NEXT:.L_Z7testShljj$entry:

  %shl = shl i32 %a, %b

; ASM-NEXT:     lsl     r0, r0, r1
; DIS-NEXT:  30:        e1a00110
; IASM-NEXT:    .byte 0x10
; IASM-NEXT:    .byte 0x1
; IASM-NEXT:    .byte 0xa0
; IASM-NEXT:    .byte 0xe1

  ret i32 %shl
}
