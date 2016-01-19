; Show that we know how to translate vpush and vpop.

; NOTE: We use -O2 because vpush/vpop only occur if optimized. Uses
; simple call with double parameters to cause the insertion of
; vpush/vpop.

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

define internal double @testVpushVpop(double %v1, double %v2) {
; ASM-LABEL: testVpushVpop:
; DIS-LABEL: 00000000 <testVpushVpop>:
; IASM-LABEL: testVpushVpop:

entry:
; ASM-NEXT: .LtestVpushVpop$entry:
; IASM-NEXT: .LtestVpushVpop$entry:

; ASM-NEXT:     vpush   {s16, s17, s18, s19}
; DIS-NEXT:    0:       ed2d8a04
; IASM-NEXT:    .byte 0x4
; IASM-NEXT:    .byte 0x8a
; IASM-NEXT:    .byte 0x2d
; IASM-NEXT:    .byte 0xed

; ASM-NEXT:     push    {lr}
; DIS-NEXT:    4:       e52de004
; IASM-NEXT:    .byte 0x4
; IASM-NEXT:    .byte 0xe0
; IASM-NEXT:    .byte 0x2d
; IASM-NEXT:    .byte 0xe5

; ASM-NEXT:     sub     sp, sp, #12
; DIS-NEXT:    8:       e24dd00c
; IASM-NEXT:    .byte 0xc
; IASM-NEXT:    .byte 0xd0
; IASM-NEXT:    .byte 0x4d
; IASM-NEXT:    .byte 0xe2

; ASM-NEXT:     vmov.f64        d8, d0
; DIS-NEXT:    c:       eeb08b40
; IASM-NEXT:    vmov.f64        d8, d0

; ASM-NEXT:     vmov.f64        d9, d1
; DIS-NEXT:   10:       eeb09b41
; IASM-NEXT:    vmov.f64        d9, d1

  call void @foo()

; ASM-NEXT:     bl      foo
; DIS-NEXT:   14:       ebfffffe
; IASM-NEXT:    bl      foo     @ .word ebfffffe

  %res = fadd double %v1, %v2

; ASM-NEXT:     vadd.f64        d8, d8, d9
; DIS-NEXT:   18:       ee388b09
; IASM-NEXT:    .byte 0x9
; IASM-NEXT:    .byte 0x8b
; IASM-NEXT:    .byte 0x38
; IASM-NEXT:    .byte 0xee

; ASM-NEXT:     vmov.f64        d0, d8
; DIS-NEXT:   1c:       eeb00b48
; IASM-NEXT:    vmov.f64        d0, d8

  ret double %res

; ASM-NEXT:     add     sp, sp, #12
; DIS-NEXT:   20:       e28dd00c
; IASM-NEXT:    .byte 0xc
; IASM-NEXT:    .byte 0xd0
; IASM-NEXT:    .byte 0x8d
; IASM-NEXT:    .byte 0xe2

; ASM-NEXT:     pop     {lr}
; ASM-NEXT:     # lr = def.pseudo 
; DIS-NEXT:   24:       e49de004
; IASM-NEXT:    .byte 0x4
; IASM-NEXT:    .byte 0xe0
; IASM-NEXT:    .byte 0x9d
; IASM-NEXT:    .byte 0xe4

; ASM-NEXT:     vpop    {s16, s17, s18, s19}
; ASM-NEXT:     # s16 = def.pseudo 
; ASM-NEXT:     # s17 = def.pseudo 
; ASM-NEXT:     # s18 = def.pseudo 
; ASM-NEXT:     # s19 = def.pseudo 
; DIS-NEXT:   28:       ecbd8a04
; IASM-NEXT:    .byte 0x4
; IASM-NEXT:    .byte 0x8a
; IASM-NEXT:    .byte 0xbd
; IASM-NEXT:    .byte 0xec

; ASM-NEXT:     bx      lr
; DIS-NEXT:   2c:       e12fff1e
; IASM-NEXT:    .byte 0x1e
; IASM-NEXT:    .byte 0xff
; IASM-NEXT:    .byte 0x2f
; IASM-NEXT:    .byte 0xe1

}

define internal void @foo() {
  ret void
}
