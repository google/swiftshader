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

; ASM-NEXT:     vpush   {s28, s29, s30, s31}
; DIS-NEXT:    0:       ed2dea04
; IASM-NEXT:    .byte 0x4
; IASM-NEXT:    .byte 0xea
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

; ASM-NEXT:     vmov.f64        d15, d0
; DIS-NEXT:    c:       eeb0fb40
; IASM-NEXT:    vmov.f64        d15, d0

; ASM-NEXT:     vmov.f64        d14, d1
; DIS-NEXT:   10:       eeb0eb41
; IASM-NEXT:    vmov.f64        d14, d1

  call void @foo()

; ASM-NEXT:     bl      foo
; DIS-NEXT:   14:       ebfffffe
; IASM-NEXT:    bl      foo     @ .word ebfffffe

  %res = fadd double %v1, %v2

; ASM-NEXT:     vadd.f64        d15, d15, d14
; DIS-NEXT:   18:       ee3ffb0e
; IASM-NEXT:    .byte 0xe
; IASM-NEXT:    .byte 0xfb
; IASM-NEXT:    .byte 0x3f
; IASM-NEXT:    .byte 0xee

; ASM-NEXT:     vmov.f64        d0, d15
; DIS-NEXT:   1c:       eeb00b4f
; IASM-NEXT:    vmov.f64        d0, d15

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

; ASM-NEXT:     vpop    {s28, s29, s30, s31}
; ASM-NEXT:     # s28 = def.pseudo 
; ASM-NEXT:     # s29 = def.pseudo 
; ASM-NEXT:     # s30 = def.pseudo 
; ASM-NEXT:     # s31 = def.pseudo 
; DIS-NEXT:   28:       ecbdea04
; IASM-NEXT:    .byte 0x4
; IASM-NEXT:    .byte 0xea
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
