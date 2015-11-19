; Show that we handle constants in a movw, when it isn't represented as
; ConstantRelocatable (see mov-imm.ll for the ConstantRelocatable case).

; REQUIRES: allow_dump

; Compile using standalone assembler.
; RUN: %p2i --filetype=asm -i %s --target=arm32 --args -Om1 \
; RUN:   --test-stack-extra 4084 | FileCheck %s --check-prefix=ASM

; Show bytes in assembled standalone code.
; RUN: %p2i --filetype=asm -i %s --target=arm32 --assemble --disassemble \
; RUN:   --args -Om1 --test-stack-extra 4084 | FileCheck %s --check-prefix=DIS

; Compile using integrated assembler.
; RUN: %p2i --filetype=iasm -i %s --target=arm32 --args -Om1 \
; RUN:   --test-stack-extra 4084 | FileCheck %s --check-prefix=IASM

; Show bytes in assembled integrated code.
; RUN: %p2i --filetype=iasm -i %s --target=arm32 --assemble --disassemble \
; RUN:   --args -Om1 --test-stack-extra 4084 \
; RUN:   | FileCheck %s --check-prefix=DIS

define internal i32 @foo(i32 %x) {
entry:

; ASM-LABEL: foo:
; ASM-NEXT: .Lfoo$entry:
; ******* Movw case to check *******
; ASM-NEXT:     movw    ip, #4092
; ASM-NEXT:     sub     sp, sp, ip
; ASM-NEXT:     str     r0, [sp, #4088]
; ASM-NEXT:     # [sp, #4088] = def.pseudo
; DIS-LABEL: 00000000 <foo>:
; DIS-NEXT:    0:       e300cffc
; DIS-NEXT:    4:       e04dd00c
; DIS-NEXT:    8:       e58d0ff8

; IASM-LABEL: foo:
; IASM-NEXT: .Lfoo$entry:

; IASM-NEXT:    .byte 0xfc
; IASM-NEXT:    .byte 0xcf
; IASM-NEXT:    .byte 0x0
; IASM-NEXT:    .byte 0xe3

; IASM-NEXT:    .byte 0xc
; IASM-NEXT:    .byte 0xd0
; IASM-NEXT:    .byte 0x4d
; IASM-NEXT:    .byte 0xe0

; IASM-NEXT:    .byte 0xf8
; IASM-NEXT:    .byte 0xf
; IASM-NEXT:    .byte 0x8d
; IASM-NEXT:    .byte 0xe5

  %mul = mul i32 %x, %x

; ASM-NEXT:     ldr     r0, [sp, #4088]
; ASM-NEXT:     ldr     r1, [sp, #4088]
; ASM-NEXT:     mul     r0, r0, r1
; ASM-NEXT:     str     r0, [sp, #4084]
; ASM-NEXT:     # [sp, #4084] = def.pseudo

; DIS-NEXT:    c:       e59d0ff8
; DIS-NEXT:   10:       e59d1ff8
; DIS-NEXT:   14:       e0000190
; DIS-NEXT:   18:       e58d0ff4

; IASM-NEXT:    .byte 0xf8
; IASM-NEXT:    .byte 0xf
; IASM-NEXT:    .byte 0x9d
; IASM-NEXT:    .byte 0xe5

; IASM-NEXT:    .byte 0xf8
; IASM-NEXT:    .byte 0x1f
; IASM-NEXT:    .byte 0x9d
; IASM-NEXT:    .byte 0xe5

; IASM-NEXT:    .byte 0x90
; IASM-NEXT:    .byte 0x1
; IASM-NEXT:    .byte 0x0
; IASM-NEXT:    .byte 0xe0

; IASM-NEXT:    .byte 0xf4
; IASM-NEXT:    .byte 0xf
; IASM-NEXT:    .byte 0x8d
; IASM-NEXT:    .byte 0xe5

  ret i32 %mul

; ASM-NEXT:     ldr     r0, [sp, #4084]
; ******* Movw case to check *******
; ASM-NEXT:     movw    ip, #4092
; ASM-NEXT:     add     sp, sp, ip
; ASM-NEXT:     bx      lr

; DIS-NEXT:   1c:       e59d0ff4
; DIS-NEXT:   20:       e300cffc
; DIS-NEXT:   24:       e08dd00c
; DIS-NEXT:   28:       e12fff1e

; IASM-NEXT:    .byte 0xf4
; IASM-NEXT:    .byte 0xf
; IASM-NEXT:    .byte 0x9d
; IASM-NEXT:    .byte 0xe5

; IASM-NEXT:    .byte 0xfc
; IASM-NEXT:    .byte 0xcf
; IASM-NEXT:    .byte 0x0
; IASM-NEXT:    .byte 0xe3

; IASM-NEXT:    .byte 0xc
; IASM-NEXT:    .byte 0xd0
; IASM-NEXT:    .byte 0x8d
; IASM-NEXT:    .byte 0xe0

; IASM-NEXT:    .byte 0x1e
; IASM-NEXT:    .byte 0xff
; IASM-NEXT:    .byte 0x2f
; IASM-NEXT:    .byte 0xe1

}
