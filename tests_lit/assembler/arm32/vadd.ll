; Show that we know how to translate vadd.

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

define internal float @testVadd(float %v1, float %v2) {
; ASM-LABEL: testVadd:
; DIS-LABEL: 00000000 <testVadd>:
; IASM-LABEL: testVadd:

entry:
; ASM-NEXT: .LtestVadd$entry:
; IASM-NEXT: .LtestVadd$entry:

  %res = fadd float %v1, %v2

; ASM-NEXT:     vadd.f32        s0, s0, s1
; DIS-NEXT:    0:       ee300a20
; IASM-NEXT:    .byte 0x20
; IASM-NEXT:    .byte 0xa
; IASM-NEXT:    .byte 0x30
; IASM-NEXT:    .byte 0xee

  ret float %res
}
