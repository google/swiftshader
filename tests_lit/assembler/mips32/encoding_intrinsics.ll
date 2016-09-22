; Test encoding of MIPS32 instructions used in intrinsic calls

; REQUIRES: allow_dump

; Compile using standalone assembler.
; RUN: %p2i --filetype=asm -i %s --target=mips32 --args -O2 \
; RUN:   --allow-externally-defined-symbols --skip-unimplemented \
; RUN:   | FileCheck %s --check-prefix=ASM

; Show bytes in assembled standalone code.
; RUN: %p2i --filetype=asm -i %s --target=mips32 --assemble --disassemble \
; RUN:   --args -O2 --allow-externally-defined-symbols --skip-unimplemented \
; RUN:   | FileCheck %s --check-prefix=DIS

; Compile using integrated assembler.
; RUN: %p2i --filetype=iasm -i %s --target=mips32 --args -O2 \
; RUN:   --allow-externally-defined-symbols --skip-unimplemented \
; RUN:   | FileCheck %s --check-prefix=IASM

; Show bytes in assembled integrated code.
; RUN: %p2i --filetype=iasm -i %s --target=mips32 --assemble --disassemble \
; RUN:   --args -O2 --allow-externally-defined-symbols --skip-unimplemented \
; RUN:   | FileCheck %s --check-prefix=DIS

declare i32 @llvm.ctlz.i32(i32, i1)
declare void @llvm.trap()

define internal i32 @encCtlz32(i32 %x) {
entry:
  %r = call i32 @llvm.ctlz.i32(i32 %x, i1 false)
  ret i32 %r
}

; ASM-LABEL: encCtlz32
; ASM-NEXT: .LencCtlz32$entry:
; ASM-NEXT: 	clz	$a0, $a0
; ASM-NEXT: 	move	$v0, $a0
; ASM-NEXT: 	jr	$ra

; DIS-LABEL: 00000000 <encCtlz32>:
; DIS-NEXT:    0:	70842020 	clz	a0,a0
; DIS-NEXT:    4:	00801021 	move	v0,a0
; DIS-NEXT:    8:	03e00008 	jr	ra

; IASM-LABEL: encCtlz32
; IASM-NEXT: .LencCtlz32$entry:
; IASM-NEXT: 	.byte 0x20
; IASM-NEXT: 	.byte 0x20
; IASM-NEXT: 	.byte 0x84
; IASM-NEXT: 	.byte 0x70
; IASM-NEXT: 	.byte 0x21
; IASM-NEXT: 	.byte 0x10
; IASM-NEXT: 	.byte 0x80
; IASM-NEXT: 	.byte 0x0
; IASM-NEXT: 	.byte 0x8
; IASM-NEXT: 	.byte 0x0
; IASM-NEXT: 	.byte 0xe0
; IASM-NEXT: 	.byte 0x3

define internal void @encTrap() {
  unreachable
}

; ASM-LABEL: encTrap
; ASM-NEXT: .LencTrap$__0:
; ASM-NEXT: 	teq	$zero, $zero, 0

; DIS-LABEL: 00000010 <encTrap>:
; DIS-NEXT:    10:	00000034 	teq	zero,zero

; IASM-LABEL: encTrap:
; IASM-NEXT: .LencTrap$__0:
; IASM-NEXT: 	.byte 0x34
; IASM-NEXT: 	.byte 0x0
; IASM-NEXT: 	.byte 0x0
; IASM-NEXT: 	.byte 0x0
