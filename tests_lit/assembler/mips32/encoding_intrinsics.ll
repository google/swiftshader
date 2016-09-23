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
declare i32 @llvm.cttz.i32(i32, i1)
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

define internal i32 @encCttz32(i32 %x) {
entry:
  %r = call i32 @llvm.cttz.i32(i32 %x, i1 false)
  ret i32 %r
}

; ASM-LABEL: encCttz32
; ASM-NEXT: .LencCttz32$entry:
; ASM-NEXT: 	addiu	$v0, $a0, -1
; ASM-NEXT: 	nor	$a0, $a0, $zero
; ASM-NEXT: 	and	$a0, $a0, $v0
; ASM-NEXT: 	clz	$a0, $a0
; ASM-NEXT: 	addiu	$v0, $zero, 32
; ASM-NEXT: 	subu	$v0, $v0, $a0
; ASM-NEXT: 	jr	$ra

; DIS-LABEL: 00000010 <encCttz32>:
; DIS-NEXT:   10:	2482ffff 	addiu	v0,a0,-1
; DIS-NEXT:   14:	00802027 	nor	a0,a0,zero
; DIS-NEXT:   18:	00822024 	and	a0,a0,v0
; DIS-NEXT:   1c:	70842020 	clz	a0,a0
; DIS-NEXT:   20:	24020020 	li	v0,32
; DIS-NEXT:   24:	00441023 	subu	v0,v0,a0
; DIS-NEXT:   28:	03e00008 	jr	ra

; IASM-LABEL: encCttz32
; IASM-NEXT: .LencCttz32$entry:
; IASM-NEXT: 	.byte 0xff
; IASM-NEXT: 	.byte 0xff
; IASM-NEXT: 	.byte 0x82
; IASM-NEXT: 	.byte 0x24
; IASM-NEXT: 	.byte 0x27
; IASM-NEXT: 	.byte 0x20
; IASM-NEXT: 	.byte 0x80
; IASM-NEXT: 	.byte 0x0
; IASM-NEXT: 	.byte 0x24
; IASM-NEXT: 	.byte 0x20
; IASM-NEXT: 	.byte 0x82
; IASM-NEXT: 	.byte 0x0
; IASM-NEXT: 	.byte 0x20
; IASM-NEXT: 	.byte 0x20
; IASM-NEXT: 	.byte 0x84
; IASM-NEXT: 	.byte 0x70
; IASM-NEXT: 	.byte 0x20
; IASM-NEXT: 	.byte 0x0
; IASM-NEXT: 	.byte 0x2
; IASM-NEXT: 	.byte 0x24
; IASM-NEXT: 	.byte 0x23
; IASM-NEXT: 	.byte 0x10
; IASM-NEXT: 	.byte 0x44
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

; DIS-LABEL: 00000030 <encTrap>:
; DIS-NEXT:    30:	00000034 	teq	zero,zero

; IASM-LABEL: encTrap:
; IASM-NEXT: .LencTrap$__0:
; IASM-NEXT: 	.byte 0x34
; IASM-NEXT: 	.byte 0x0
; IASM-NEXT: 	.byte 0x0
; IASM-NEXT: 	.byte 0x0
