; Test vldrd and vstrd when address is offset with an immediate.

; REQUIRES: allow_dump

; Compile using standalone assembler.
; RUN: %p2i --filetype=asm -i %s --target=arm32 --args -Om1 \
; RUN:   -reg-use d20 \
; RUN:   | FileCheck %s --check-prefix=ASM

; Show bytes in assembled standalone code.
; RUN: %p2i --filetype=asm -i %s --target=arm32 --assemble --disassemble \
; RUN:   --args -Om1 \
; RUN:   -reg-use d20 \
; RUN:   | FileCheck %s --check-prefix=DIS

; Compile using integrated assembler.
; RUN: %p2i --filetype=iasm -i %s --target=arm32 --args -Om1 \
; RUN:   -reg-use d20 \
; RUN:   | FileCheck %s --check-prefix=IASM

; Show bytes in assembled integrated code.
; RUN: %p2i --filetype=iasm -i %s --target=arm32 --assemble --disassemble \
; RUN:   --args -Om1 \
; RUN:   -reg-use d20 \
; RUN:   | FileCheck %s --check-prefix=DIS

define internal i64 @testVldrStrImm(double %d) {
; ASM-LABEL: testVldrStrImm:
; DIS-LABEL: 00000000 <testVldrStrImm>:
; IASM-LABEL: testVldrStrImm:

entry:
; ASM-NEXT: .LtestVldrStrImm$entry:
; IASM-NEXT: .LtestVldrStrImm$entry:

; ASM:  vstr    d0, [sp, #8]
; DIS:    4:   ed8d0b02
; IASM-NOT: vstr

  %v = bitcast double %d to i64

; ASM:  vldr    d20, [sp, #8]
; DIS:    8:   eddd4b02
; IASM-NOT: vldr

  ret i64 %v
}
