; Show that we know how to translate LDR (register).

; NOTE: We use -O2 to get rid of memory stores.

; REQUIRES: allow_dump

; Compile using standalone assembler.
; RUN: %lc2i --filetype=asm -i %s --target=arm32 --args -O2 \
; RUN:   | FileCheck %s --check-prefix=ASM

; Show bytes in assembled standalone code.
; RUN: %lc2i --filetype=asm -i %s --target=arm32 --assemble --disassemble \
; RUN:   --args -O2 | FileCheck %s --check-prefix=DIS

; Compile using integrated assembler.
; RUN: %lc2i --filetype=iasm -i %s --target=arm32 --args -O2 \
; RUN:   | FileCheck %s --check-prefix=IASM

; Show bytes in assembled integrated code.
; RUN: %lc2i --filetype=iasm -i %s --target=arm32 --assemble --disassemble \
; RUN:   --args -O2 | FileCheck %s --check-prefix=DIS

@ArrayInitPartial = internal global [40 x i8] c"<\00\00\00F\00\00\00P\00\00\00Z\00\00\00d\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00", align 4

@NumArraysElements = internal global [4 x i8] c"\01\00\00\00", align 4

@Arrays = internal constant <{ i32, [4 x i8] }> <{ i32 ptrtoint ([40 x i8]* @ArrayInitPartial to i32), [4 x i8] c"\14\00\00\00" }>, align 4

define internal void @_Z8getArrayjRj(i32 %WhichArray, i32 %Len) {
; ASM-LABEL:_Z8getArrayjRj:
; DIS-LABEL:00000000 <_Z8getArrayjRj>:
; IASM-LABEL:_Z8getArrayjRj:

entry:
; ASM-NEXT:.L_Z8getArrayjRj$entry:
; IASM-NEXT:.L_Z8getArrayjRj$entry:

  %gep_array = mul i32 %WhichArray, 8
  %expanded1 = ptrtoint <{ i32, [4 x i8] }>* @Arrays to i32
  %gep = add i32 %expanded1, %gep_array

; ASM-NEXT:     movw    r2, #:lower16:Arrays
; ASM-NEXT:     movt    r2, #:upper16:Arrays
; DIS-NEXT:   0:        e3002000
; DIS-NEXT:   4:        e3402000
; IASM-NEXT:	movw	r2, #:lower16:Arrays	@ .word e3002000
; IASM-NEXT:	movt	r2, #:upper16:Arrays	@ .word e3402000

  %gep3 = add i32 %gep, 4

; ASM-NEXT:    add     r2, r2, #4
; DIS-NEXT:   8:        e2822004
; IASM-NEXT:	.byte 0x4
; IASM-NEXT:	.byte 0x20
; IASM-NEXT:	.byte 0x82
; IASM-NEXT:	.byte 0xe2

; ***** Here is the use of a LDR (register) instruction.
  %gep3.asptr = inttoptr i32 %gep3 to i32*
  %v1 = load i32, i32* %gep3.asptr, align 1

; ASM-NEXT:    ldr     r2, [r2, r0, lsl #3]
; DIS-NEXT:   c:        e7922180
; IASM-NEXT:	.byte 0x80
; IASM-NEXT:	.byte 0x21
; IASM-NEXT:	.byte 0x92
; IASM-NEXT:	.byte 0xe7


  %Len.asptr3 = inttoptr i32 %Len to i32*
  store i32 %v1, i32* %Len.asptr3, align 1

; ASM-NEXT:    str     r2, [r1]
; DIS-NEXT:  10:        e5812000
; IASM-NEXT:	.byte 0x0
; IASM-NEXT:	.byte 0x20
; IASM-NEXT:	.byte 0x81
; IASM-NEXT:	.byte 0xe5

   ret void

; ASM-NEXT:    bx      lr
; DIS-NEXT:  14:        e12fff1e
; IASM-NEXT:	.byte 0x1e
; IASM-NEXT:	.byte 0xff
; IASM-NEXT:	.byte 0x2f
; IASM-NEXT:	.byte 0xe1

}
