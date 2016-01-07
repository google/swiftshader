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

define internal float @testVaddFloat(float %v1, float %v2) {
; ASM-LABEL: testVaddFloat:
; DIS-LABEL: 00000000 <testVaddFloat>:
; IASM-LABEL: testVaddFloat:

entry:
; ASM-NEXT: .LtestVaddFloat$entry:
; IASM-NEXT: .LtestVaddFloat$entry:

  %res = fadd float %v1, %v2

; ASM-NEXT:     vadd.f32        s0, s0, s1
; DIS-NEXT:    0:       ee300a20
; IASM-NEXT:    .byte 0x20
; IASM-NEXT:    .byte 0xa
; IASM-NEXT:    .byte 0x30
; IASM-NEXT:    .byte 0xee

  ret float %res
}

define internal double @testVaddDouble(double %v1, double %v2) {
; ASM-LABEL: testVaddDouble:
; DIS-LABEL: 00000010 <testVaddDouble>:
; IASM-LABEL: testVaddDouble:

entry:
; ASM-NEXT: .LtestVaddDouble$entry:
; IASM-NEXT: .LtestVaddDouble$entry:

  %res = fadd double %v1, %v2

; ASM-NEXT:     vadd.f64        d0, d0, d1
; DIS-NEXT:   10:       ee300b01
; IASM-NEXT:    .byte 0x1
; IASM-NEXT:    .byte 0xb
; IASM-NEXT:    .byte 0x30
; IASM-NEXT:    .byte 0xee

  ret double %res
}
