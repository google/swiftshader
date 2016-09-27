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
declare i64 @llvm.ctlz.i64(i64, i1)
declare i32 @llvm.cttz.i32(i32, i1)
declare i64 @llvm.cttz.i64(i64, i1)
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

; DIS-LABEL: {{.*}} <encCtlz32>:
; DIS-NEXT:  {{.*}} 70842020 	clz	a0,a0
; DIS-NEXT:  {{.*}} 00801021 	move	v0,a0
; DIS-NEXT:  {{.*}} 03e00008 	jr	ra

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

define internal i32 @encCtlz32Const() {
entry:
  %r = call i32 @llvm.ctlz.i32(i32 123456, i1 false)
  ret i32 %r
}

; ASM-LABEL: encCtlz32Const
; ASM-NEXT: .LencCtlz32Const$entry:
; ASM-NEXT: 	lui	$v0, 1
; ASM-NEXT: 	ori	$v0, $v0, 57920
; ASM-NEXT: 	clz	$v0, $v0
; ASM-NEXT: 	jr	$ra

; DIS-LABEL: {{.*}} <encCtlz32Const>:
; DIS-NEXT:  {{.*}} 3c020001 	lui	v0,0x1
; DIS-NEXT:  {{.*}} 3442e240 	ori	v0,v0,0xe240
; DIS-NEXT:  {{.*}} 70421020 	clz	v0,v0
; DIS-NEXT:  {{.*}} 03e00008 	jr	ra

; IASM-LABEL: encCtlz32Const
; IASM-NEXT: .LencCtlz32Const$entry:
; IASM-NEXT: 	.byte 0x1
; IASM-NEXT: 	.byte 0x0
; IASM-NEXT: 	.byte 0x2
; IASM-NEXT: 	.byte 0x3c
; IASM-NEXT: 	.byte 0x40
; IASM-NEXT: 	.byte 0xe2
; IASM-NEXT: 	.byte 0x42
; IASM-NEXT: 	.byte 0x34
; IASM-NEXT: 	.byte 0x20
; IASM-NEXT: 	.byte 0x10
; IASM-NEXT: 	.byte 0x42
; IASM-NEXT: 	.byte 0x70
; IASM-NEXT: 	.byte 0x8
; IASM-NEXT: 	.byte 0x0
; IASM-NEXT: 	.byte 0xe0
; IASM-NEXT: 	.byte 0x3

define internal i64 @encCtlz64(i64 %x) {
entry:
  %r = call i64 @llvm.ctlz.i64(i64 %x, i1 false)
  ret i64 %r
}

; ASM-LABEL: encCtlz64
; ASM-NEXT: .LencCtlz64$entry:
; ASM-NEXT: 	clz	$v0, $a1
; ASM-NEXT: 	clz	$a0, $a0
; ASM-NEXT: 	addiu	$a0, $a0, 32
; ASM-NEXT: 	movn	$a0, $v0, $a1
; ASM-NEXT: 	addiu	$v0, $zero, 0
; ASM-NEXT: 	move	$v1, $v0
; ASM-NEXT: 	move	$v0, $a0
; ASM-NEXT: 	jr	$ra

; DIS-LABEL: {{.*}} <encCtlz64>:
; DIS-NEXT:  {{.*}} 70a21020 	clz	v0,a1
; DIS-NEXT:  {{.*}} 70842020 	clz	a0,a0
; DIS-NEXT:  {{.*}} 24840020 	addiu	a0,a0,32
; DIS-NEXT:  {{.*}} {{.*}}0b 	movn	{{.*}}
; DIS-NEXT:  {{.*}} 24020000 	li	v0,0
; DIS-NEXT:  {{.*}} 00401821 	move	v1,v0
; DIS-NEXT:  {{.*}} 00801021 	move	v0,a0
; DIS-NEXT:  {{.*}} 03e00008 	jr	ra

; IASM-LABEL: encCtlz64
; IASM-NEXT: .LencCtlz64$entry:
; IASM-NEXT: 	.byte 0x20
; IASM-NEXT: 	.byte 0x10
; IASM-NEXT: 	.byte 0xa2
; IASM-NEXT: 	.byte 0x70
; IASM-NEXT: 	.byte 0x20
; IASM-NEXT: 	.byte 0x20
; IASM-NEXT: 	.byte 0x84
; IASM-NEXT: 	.byte 0x70
; IASM-NEXT: 	.byte 0x20
; IASM-NEXT: 	.byte 0x0
; IASM-NEXT: 	.byte 0x84
; IASM-NEXT: 	.byte 0x24
; IASM-NEXT: 	.byte 0xb
; IASM-NEXT: 	.byte 0x20
; IASM-NEXT: 	.byte 0x82
; IASM-NEXT: 	.byte 0x0
; IASM-NEXT: 	.byte 0x0
; IASM-NEXT: 	.byte 0x0
; IASM-NEXT: 	.byte 0x2
; IASM-NEXT: 	.byte 0x24
; IASM-NEXT: 	.byte 0x21
; IASM-NEXT: 	.byte 0x18
; IASM-NEXT: 	.byte 0x40
; IASM-NEXT: 	.byte 0x0
; IASM-NEXT: 	.byte 0x21
; IASM-NEXT: 	.byte 0x10
; IASM-NEXT: 	.byte 0x80
; IASM-NEXT: 	.byte 0x0
; IASM-NEXT: 	.byte 0x8
; IASM-NEXT: 	.byte 0x0
; IASM-NEXT: 	.byte 0xe0
; IASM-NEXT: 	.byte 0x3

define internal i32 @encCtlz64Const(i64 %x) {
entry:
  %r = call i64 @llvm.ctlz.i64(i64 123456789012, i1 false)
  %r2 = trunc i64 %r to i32
  ret i32 %r2
}

; ASM-LABEL: encCtlz64Const
; ASM-NEXT: .LencCtlz64Const$entry:
; ASM-NEXT: 	# $zero = def.pseudo
; ASM-NEXT: 	addiu	$v0, $zero, 28
; ASM-NEXT: 	lui	$v1, 48793
; ASM-NEXT: 	ori	$v1, $v1, 6676
; ASM-NEXT: 	clz	$a0, $v0
; ASM-NEXT: 	clz	$v1, $v1
; ASM-NEXT: 	addiu	$v1, $v1, 32
; ASM-NEXT: 	movn	$v1, $a0, $v0
; ASM-NEXT: 	move	$v0, $v1
; ASM-NEXT: 	jr	$ra

; DIS-LABEL: {{.*}} <encCtlz64Const>:
; DIS-NEXT:  {{.*}} 2402001c 	li	v0,28
; DIS-NEXT:  {{.*}} 3c03be99 	lui	v1,0xbe99
; DIS-NEXT:  {{.*}} 34631a14 	ori	v1,v1,0x1a14
; DIS-NEXT:  {{.*}} 70442020 	clz	a0,v0
; DIS-NEXT:  {{.*}} 70631820 	clz	v1,v1
; DIS-NEXT:  {{.*}} 24630020 	addiu	v1,v1,32
; DIS-NEXT:  {{.*}} {{.*}}0b 	movn	{{.*}}
; DIS-NEXT:  {{.*}} 00601021 	move	v0,v1
; DIS-NEXT:  {{.*}} 03e00008 	jr	ra

; IASM-LABEL: encCtlz64Const
; IASM-NEXT: .LencCtlz64Const$entry:
; IASM-NEXT: 	.byte 0x1c
; IASM-NEXT: 	.byte 0x0
; IASM-NEXT: 	.byte 0x2
; IASM-NEXT: 	.byte 0x24
; IASM-NEXT: 	.byte 0x99
; IASM-NEXT: 	.byte 0xbe
; IASM-NEXT: 	.byte 0x3
; IASM-NEXT: 	.byte 0x3c
; IASM-NEXT: 	.byte 0x14
; IASM-NEXT: 	.byte 0x1a
; IASM-NEXT: 	.byte 0x63
; IASM-NEXT: 	.byte 0x34
; IASM-NEXT: 	.byte 0x20
; IASM-NEXT: 	.byte 0x20
; IASM-NEXT: 	.byte 0x44
; IASM-NEXT: 	.byte 0x70
; IASM-NEXT: 	.byte 0x20
; IASM-NEXT: 	.byte 0x18
; IASM-NEXT: 	.byte 0x63
; IASM-NEXT: 	.byte 0x70
; IASM-NEXT: 	.byte 0x20
; IASM-NEXT: 	.byte 0x0
; IASM-NEXT: 	.byte 0x63
; IASM-NEXT: 	.byte 0x24
; IASM-NEXT: 	.byte 0xb
; IASM-NEXT: 	.byte 0x18
; IASM-NEXT: 	.byte 0x64
; IASM-NEXT: 	.byte 0x0
; IASM-NEXT: 	.byte 0x21
; IASM-NEXT: 	.byte 0x10
; IASM-NEXT: 	.byte 0x60
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

; DIS-LABEL: {{.*}} <encCttz32>:
; DIS-NEXT:  {{.*}} 2482ffff 	addiu	v0,a0,-1
; DIS-NEXT:  {{.*}} 00802027 	nor	a0,a0,zero
; DIS-NEXT:  {{.*}} 00822024 	and	a0,a0,v0
; DIS-NEXT:  {{.*}} 70842020 	clz	a0,a0
; DIS-NEXT:  {{.*}} 24020020 	li	v0,32
; DIS-NEXT:  {{.*}} 00441023 	subu	v0,v0,a0
; DIS-NEXT:  {{.*}} 03e00008 	jr	ra

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

define internal i32 @encCttz32Const() {
entry:
  %r = call i32 @llvm.cttz.i32(i32 123456, i1 false)
  ret i32 %r
}

; ASM-LABEL: encCttz32Const
; ASM-NEXT: .LencCttz32Const$entry:
; ASM-NEXT: 	lui     $v0, 1
; ASM-NEXT: 	ori     $v0, $v0, 57920
; ASM-NEXT: 	addiu   $v1, $v0, -1
; ASM-NEXT: 	nor     $v0, $v0, $zero
; ASM-NEXT: 	and     $v0, $v0, $v1
; ASM-NEXT: 	clz     $v0, $v0
; ASM-NEXT: 	addiu   $v1, $zero, 32
; ASM-NEXT: 	subu    $v1, $v1, $v0
; ASM-NEXT: 	move    $v0, $v1
; ASM-NEXT: 	jr      $ra

; DIS-LABEL: {{.*}} <encCttz32Const>:
; DIS-NEXT:  {{.*}} 3c020001 	lui	v0,0x1
; DIS-NEXT:  {{.*}} ori	v0,v0,0xe240
; DIS-NEXT:  {{.*}} addiu	v1,v0,-1
; DIS-NEXT:  {{.*}} nor	v0,v0,zero
; DIS-NEXT:  {{.*}} and	v0,v0,v1
; DIS-NEXT:  {{.*}} clz	v0,v0
; DIS-NEXT:  {{.*}} li	v1,32
; DIS-NEXT:  {{.*}} subu	v1,v1,v0
; DIS-NEXT:  {{.*}} move	v0,v1
; DIS-NEXT:  {{.*}} jr	ra

; IASM-LABEL: encCttz32Const:
; IASM-NEXT: .LencCttz32Const$entry:
; IASM-NEXT: 	.byte 0x1
; IASM-NEXT: 	.byte 0x0
; IASM-NEXT: 	.byte 0x2
; IASM-NEXT: 	.byte 0x3c
; IASM-NEXT: 	.byte 0x40
; IASM-NEXT: 	.byte 0xe2
; IASM-NEXT: 	.byte 0x42
; IASM-NEXT: 	.byte 0x34
; IASM-NEXT: 	.byte 0xff
; IASM-NEXT: 	.byte 0xff
; IASM-NEXT: 	.byte 0x43
; IASM-NEXT: 	.byte 0x24
; IASM-NEXT: 	.byte 0x27
; IASM-NEXT: 	.byte 0x10
; IASM-NEXT: 	.byte 0x40
; IASM-NEXT: 	.byte 0x0
; IASM-NEXT: 	.byte 0x24
; IASM-NEXT: 	.byte 0x10
; IASM-NEXT: 	.byte 0x43
; IASM-NEXT: 	.byte 0x0
; IASM-NEXT: 	.byte 0x20
; IASM-NEXT: 	.byte 0x10
; IASM-NEXT: 	.byte 0x42
; IASM-NEXT: 	.byte 0x70
; IASM-NEXT: 	.byte 0x20
; IASM-NEXT: 	.byte 0x0
; IASM-NEXT: 	.byte 0x3
; IASM-NEXT: 	.byte 0x24
; IASM-NEXT: 	.byte 0x23
; IASM-NEXT: 	.byte 0x18
; IASM-NEXT: 	.byte 0x62
; IASM-NEXT: 	.byte 0x0
; IASM-NEXT: 	.byte 0x21
; IASM-NEXT: 	.byte 0x10
; IASM-NEXT: 	.byte 0x60
; IASM-NEXT: 	.byte 0x0
; IASM-NEXT: 	.byte 0x8
; IASM-NEXT: 	.byte 0x0
; IASM-NEXT: 	.byte 0xe0
; IASM-NEXT: 	.byte 0x3

define internal i64 @encCttz64(i64 %x) {
entry:
  %r = call i64 @llvm.cttz.i64(i64 %x, i1 false)
  ret i64 %r
}

; ASM-LABEL: encCttz64
; ASM-NEXT: .LencCttz64$entry:
; ASM-NEXT: 	addiu   $v0, $a1, -1
; ASM-NEXT: 	nor     $a1, $a1, $zero
; ASM-NEXT: 	and     $a1, $a1, $v0
; ASM-NEXT: 	clz     $a1, $a1
; ASM-NEXT: 	addiu   $v0, $zero, 64
; ASM-NEXT: 	subu    $v0, $v0, $a1
; ASM-NEXT: 	addiu   $v1, $a0, -1
; ASM-NEXT: 	nor     $a1, $a0, $zero
; ASM-NEXT: 	and     $a1, $a1, $v1
; ASM-NEXT: 	clz     $a1, $a1
; ASM-NEXT: 	addiu   $v1, $zero, 32
; ASM-NEXT: 	subu    $v1, $v1, $a1
; ASM-NEXT: 	movn    $v0, $v1, $a0
; ASM-NEXT: 	addiu   $v1, $zero, 0
; ASM-NEXT: 	jr      $ra

; DIS-LABEL: {{.*}} <encCttz64>:
; DIS-NEXT:  {{.*}} 24a2ffff 	addiu	v0,a1,-1
; DIS-NEXT:  {{.*}} 00a02827 	nor	a1,a1,zero
; DIS-NEXT:  {{.*}} 00a22824 	and	a1,a1,v0
; DIS-NEXT:  {{.*}} 70a52820 	clz	a1,a1
; DIS-NEXT:  {{.*}} 24020040 	li	v0,64
; DIS-NEXT:  {{.*}} 00451023 	subu	v0,v0,a1
; DIS-NEXT:  {{.*}} 2483ffff 	addiu	v1,a0,-1
; DIS-NEXT:  {{.*}} 00802827 	nor	a1,a0,zero
; DIS-NEXT:  {{.*}} 00a32824 	and	a1,a1,v1
; DIS-NEXT:  {{.*}} 70a52820 	clz	a1,a1
; DIS-NEXT:  {{.*}} 24030020 	li	v1,32
; DIS-NEXT:  {{.*}} 00651823 	subu	v1,v1,a1
; DIS-NEXT:  {{.*}} {{.*}}0b 	movn	{{.*}}
; DIS-NEXT:  {{.*}} 24030000 	li	v1,0
; DIS-NEXT:  {{.*}} 03e00008 	jr	ra

; IASM-LABEL: encCttz64:
; IASM-NEXT: .LencCttz64$entry:
; IASM-NEXT: 	.byte 0xff
; IASM-NEXT: 	.byte 0xff
; IASM-NEXT: 	.byte 0xa2
; IASM-NEXT: 	.byte 0x24
; IASM-NEXT: 	.byte 0x27
; IASM-NEXT: 	.byte 0x28
; IASM-NEXT: 	.byte 0xa0
; IASM-NEXT: 	.byte 0x0
; IASM-NEXT: 	.byte 0x24
; IASM-NEXT: 	.byte 0x28
; IASM-NEXT: 	.byte 0xa2
; IASM-NEXT: 	.byte 0x0
; IASM-NEXT: 	.byte 0x20
; IASM-NEXT: 	.byte 0x28
; IASM-NEXT: 	.byte 0xa5
; IASM-NEXT: 	.byte 0x70
; IASM-NEXT: 	.byte 0x40
; IASM-NEXT: 	.byte 0x0
; IASM-NEXT: 	.byte 0x2
; IASM-NEXT: 	.byte 0x24
; IASM-NEXT: 	.byte 0x23
; IASM-NEXT: 	.byte 0x10
; IASM-NEXT: 	.byte 0x45
; IASM-NEXT: 	.byte 0x0
; IASM-NEXT: 	.byte 0xff
; IASM-NEXT: 	.byte 0xff
; IASM-NEXT: 	.byte 0x83
; IASM-NEXT: 	.byte 0x24
; IASM-NEXT: 	.byte 0x27
; IASM-NEXT: 	.byte 0x28
; IASM-NEXT: 	.byte 0x80
; IASM-NEXT: 	.byte 0x0
; IASM-NEXT: 	.byte 0x24
; IASM-NEXT: 	.byte 0x28
; IASM-NEXT: 	.byte 0xa3
; IASM-NEXT: 	.byte 0x0
; IASM-NEXT: 	.byte 0x20
; IASM-NEXT: 	.byte 0x28
; IASM-NEXT: 	.byte 0xa5
; IASM-NEXT: 	.byte 0x70
; IASM-NEXT: 	.byte 0x20
; IASM-NEXT: 	.byte 0x0
; IASM-NEXT: 	.byte 0x3
; IASM-NEXT: 	.byte 0x24
; IASM-NEXT: 	.byte 0x23
; IASM-NEXT: 	.byte 0x18
; IASM-NEXT: 	.byte 0x65
; IASM-NEXT: 	.byte 0x0
; IASM-NEXT: 	.byte 0xb
; IASM-NEXT: 	.byte 0x10
; IASM-NEXT: 	.byte 0x43
; IASM-NEXT: 	.byte 0x0
; IASM-NEXT: 	.byte 0x0
; IASM-NEXT: 	.byte 0x0
; IASM-NEXT: 	.byte 0x3
; IASM-NEXT: 	.byte 0x24
; IASM-NEXT: 	.byte 0x8
; IASM-NEXT: 	.byte 0x0
; IASM-NEXT: 	.byte 0xe0
; IASM-NEXT: 	.byte 0x3

define internal i64 @encCttz64Const(i64 %x) {
entry:
  %r = call i64 @llvm.cttz.i64(i64 123456789012, i1 false)
  ret i64 %r
}

; ASM-LABEL: encCttz64Const
; ASM-NEXT: .LencCttz64Const$entry:
; ASM-NEXT: 	# $zero = def.pseudo
; ASM-NEXT: 	addiu   $v0, $zero, 28
; ASM-NEXT: 	lui     $v1, 48793
; ASM-NEXT: 	ori     $v1, $v1, 6676
; ASM-NEXT: 	addiu   $a0, $v0, -1
; ASM-NEXT: 	nor     $v0, $v0, $zero
; ASM-NEXT: 	and     $v0, $v0, $a0
; ASM-NEXT: 	clz     $v0, $v0
; ASM-NEXT: 	addiu   $a0, $zero, 64
; ASM-NEXT: 	subu    $a0, $a0, $v0
; ASM-NEXT: 	addiu   $v0, $v1, -1
; ASM-NEXT: 	nor     $a1, $v1, $zero
; ASM-NEXT: 	and     $a1, $a1, $v0
; ASM-NEXT: 	clz     $a1, $a1
; ASM-NEXT: 	addiu   $v0, $zero, 32
; ASM-NEXT: 	subu    $v0, $v0, $a1
; ASM-NEXT: 	movn    $a0, $v0, $v1
; ASM-NEXT: 	addiu   $v0, $zero, 0
; ASM-NEXT: 	move    $v1, $v0
; ASM-NEXT: 	move    $v0, $a0
; ASM-NEXT: 	jr      $ra

; DIS-LABEL: {{.*}} <encCttz64Const>:
; DIS-NEXT:  {{.*}} 2402001c 	li	v0,28
; DIS-NEXT:  {{.*}} 3c03be99 	lui	v1,0xbe99
; DIS-NEXT:  {{.*}} 34631a14 	ori	v1,v1,0x1a14
; DIS-NEXT:  {{.*}} 2444ffff 	addiu	a0,v0,-1
; DIS-NEXT:  {{.*}} 00401027 	nor	v0,v0,zero
; DIS-NEXT:  {{.*}} 00441024 	and	v0,v0,a0
; DIS-NEXT:  {{.*}} 70421020 	clz	v0,v0
; DIS-NEXT:  {{.*}} 24040040 	li	a0,64
; DIS-NEXT:  {{.*}} 00822023 	subu	a0,a0,v0
; DIS-NEXT:  {{.*}} 2462ffff 	addiu	v0,v1,-1
; DIS-NEXT:  {{.*}} 00602827 	nor	a1,v1,zero
; DIS-NEXT:  {{.*}} 00a22824 	and	a1,a1,v0
; DIS-NEXT:  {{.*}} 70a52820 	clz	a1,a1
; DIS-NEXT:  {{.*}} 24020020 	li	v0,32
; DIS-NEXT:  {{.*}} 00451023 	subu	v0,v0,a1
; DIS-NEXT:  {{.*}} {{.*}}0b 	movn	{{.*}}
; DIS-NEXT:  {{.*}} 24{{.*}} 	li	{{.*}},0
; DIS-NEXT:  {{.*}} {{.*}}21 	move	v1,{{.*}}
; DIS-NEXT:  {{.*}} {{.*}}21 	move	v0,{{.*}}
; DIS-NEXT:  {{.*}} 03e00008 	jr	ra

; IASM-LABEL: encCttz64Const:
; IASM-NEXT: .LencCttz64Const$entry:
; IASM-NEXT: 	.byte 0x1c
; IASM-NEXT: 	.byte 0x0
; IASM-NEXT: 	.byte 0x2
; IASM-NEXT: 	.byte 0x24
; IASM-NEXT: 	.byte 0x99
; IASM-NEXT: 	.byte 0xbe
; IASM-NEXT: 	.byte 0x3
; IASM-NEXT: 	.byte 0x3c
; IASM-NEXT: 	.byte 0x14
; IASM-NEXT: 	.byte 0x1a
; IASM-NEXT: 	.byte 0x63
; IASM-NEXT: 	.byte 0x34
; IASM-NEXT: 	.byte 0xff
; IASM-NEXT: 	.byte 0xff
; IASM-NEXT: 	.byte 0x44
; IASM-NEXT: 	.byte 0x24
; IASM-NEXT: 	.byte 0x27
; IASM-NEXT: 	.byte 0x10
; IASM-NEXT: 	.byte 0x40
; IASM-NEXT: 	.byte 0x0
; IASM-NEXT: 	.byte 0x24
; IASM-NEXT: 	.byte 0x10
; IASM-NEXT: 	.byte 0x44
; IASM-NEXT: 	.byte 0x0
; IASM-NEXT: 	.byte 0x20
; IASM-NEXT: 	.byte 0x10
; IASM-NEXT: 	.byte 0x42
; IASM-NEXT: 	.byte 0x70
; IASM-NEXT: 	.byte 0x40
; IASM-NEXT: 	.byte 0x0
; IASM-NEXT: 	.byte 0x4
; IASM-NEXT: 	.byte 0x24
; IASM-NEXT: 	.byte 0x23
; IASM-NEXT: 	.byte 0x20
; IASM-NEXT: 	.byte 0x82
; IASM-NEXT: 	.byte 0x0
; IASM-NEXT: 	.byte 0xff
; IASM-NEXT: 	.byte 0xff
; IASM-NEXT: 	.byte 0x62
; IASM-NEXT: 	.byte 0x24
; IASM-NEXT: 	.byte 0x27
; IASM-NEXT: 	.byte 0x28
; IASM-NEXT: 	.byte 0x60
; IASM-NEXT: 	.byte 0x0
; IASM-NEXT: 	.byte 0x24
; IASM-NEXT: 	.byte 0x28
; IASM-NEXT: 	.byte 0xa2
; IASM-NEXT: 	.byte 0x0
; IASM-NEXT: 	.byte 0x20
; IASM-NEXT: 	.byte 0x28
; IASM-NEXT: 	.byte 0xa5
; IASM-NEXT: 	.byte 0x70
; IASM-NEXT: 	.byte 0x20
; IASM-NEXT: 	.byte 0x0
; IASM-NEXT: 	.byte 0x2
; IASM-NEXT: 	.byte 0x24
; IASM-NEXT: 	.byte 0x23
; IASM-NEXT: 	.byte 0x10
; IASM-NEXT: 	.byte 0x45
; IASM-NEXT: 	.byte 0x0
; IASM-NEXT: 	.byte 0xb
; IASM-NEXT: 	.byte 0x20
; IASM-NEXT: 	.byte 0x82
; IASM-NEXT: 	.byte 0x0
; IASM-NEXT: 	.byte 0x0
; IASM-NEXT: 	.byte 0x0
; IASM-NEXT: 	.byte 0x2
; IASM-NEXT: 	.byte 0x24
; IASM-NEXT: 	.byte 0x21
; IASM-NEXT: 	.byte 0x18
; IASM-NEXT: 	.byte 0x40
; IASM-NEXT: 	.byte 0x0
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

; DIS-LABEL: {{.*}} <encTrap>:
; DIS-NEXT:  {{.*}} 00000034 	teq	zero,zero

; IASM-LABEL: encTrap:
; IASM-NEXT: .LencTrap$__0:
; IASM-NEXT: 	.byte 0x34
; IASM-NEXT: 	.byte 0x0
; IASM-NEXT: 	.byte 0x0
; IASM-NEXT: 	.byte 0x0
