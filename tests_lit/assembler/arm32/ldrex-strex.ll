; Tests assembly of ldrex and strex instructions

; REQUIRES: allow_dump

; Compile using standalone assembler.
; RUN: %p2i --filetype=asm -i %s --target=arm32 --args -Om1 \
; RUN:   | FileCheck %s --check-prefix=ASM

; Show bytes in assembled standalone code.
; RUN: %p2i --filetype=asm -i %s --target=arm32 --assemble --disassemble \
; RUN:   --args -Om1 | FileCheck %s --check-prefix=DIS

; Compile using integrated assembler.
; RUN: %p2i --filetype=iasm -i %s --target=arm32 --args -Om1 \
; RUN:   | FileCheck %s --check-prefix=IASM

; Show bytes in assembled integrated code.
; RUN: %p2i --filetype=iasm -i %s --target=arm32 --assemble --disassemble \
; RUN:   --args -Om1 | FileCheck %s --check-prefix=DIS

declare i8 @llvm.nacl.atomic.rmw.i8(i32, i8*, i8, i32)

declare i16 @llvm.nacl.atomic.rmw.i16(i32, i16*, i16, i32)

declare i32 @llvm.nacl.atomic.rmw.i32(i32, i32*, i32, i32) #0

declare i64 @llvm.nacl.atomic.rmw.i64(i32, i64*, i64, i32) #0

define internal i32 @testI8Form(i32 %ptr, i32 %a) {
; ASM-LABEL:testI8Form:
; DIS-LABEL:00000000 <testI8Form>:
; IASM-LABEL:testI8Form:

entry:
; ASM-NEXT:.LtestI8Form$entry:
; IASM-NEXT:.LtestI8Form$entry:

; ASM-NEXT:     sub     sp, sp, #28
; DIS-NEXT:   0:        e24dd01c
; IASM-NEXT:    .byte 0x1c
; IASM-NEXT:    .byte 0xd0
; IASM-NEXT:    .byte 0x4d
; IASM-NEXT:    .byte 0xe2

; ASM-NEXT:     str     r0, [sp, #24]
; ASM-NEXT:     # [sp, #24] = def.pseudo
; DIS-NEXT:   4:        e58d0018
; IASM-NEXT:    .byte 0x18
; IASM-NEXT:    .byte 0x0
; IASM-NEXT:    .byte 0x8d
; IASM-NEXT:    .byte 0xe5

; ASM-NEXT:     str     r1, [sp, #20]
; ASM-NEXT:     # [sp, #20] = def.pseudo
; DIS-NEXT:   8:        e58d1014
; IASM-NEXT:    .byte 0x14
; IASM-NEXT:    .byte 0x10
; IASM-NEXT:    .byte 0x8d
; IASM-NEXT:    .byte 0xe5

  %ptr.asptr = inttoptr i32 %ptr to i8*
  %a.arg_trunc = trunc i32 %a to i8

; ASM-NEXT:     ldr     r0, [sp, #20]
; DIS-NEXT:   c:        e59d0014
; IASM-NEXT:    .byte 0x14
; IASM-NEXT:    .byte 0x0
; IASM-NEXT:    .byte 0x9d
; IASM-NEXT:    .byte 0xe5

; ASM-NEXT:     strb    r0, [sp, #16]
; DIS-NEXT:  10:        e5cd0010
; ASM-NEXT:     # [sp, #16] = def.pseudo
; IASM-NEXT:    .byte 0x10
; IASM-NEXT:    .byte 0x0
; IASM-NEXT:    .byte 0xcd
; IASM-NEXT:    .byte 0xe5

  %v = call i8 @llvm.nacl.atomic.rmw.i8(i32 1, i8* %ptr.asptr,
                                        i8 %a.arg_trunc, i32 6)

; ASM-NEXT:     ldrb    r0, [sp, #16]
; DIS-NEXT:  14:        e5dd0010
; IASM-NEXT:    .byte 0x10
; IASM-NEXT:    .byte 0x0
; IASM-NEXT:    .byte 0xdd
; IASM-NEXT:    .byte 0xe5

; ASM-NEXT:     strb    r0, [sp, #4]
; ASM-NEXT:     # [sp, #4] = def.pseudo
; DIS-NEXT:  18:        e5cd0004
; IASM-NEXT:    .byte 0x4
; IASM-NEXT:    .byte 0x0
; IASM-NEXT:    .byte 0xcd
; IASM-NEXT:    .byte 0xe5

; ASM-NEXT:     ldr     r0, [sp, #24]
; DIS-NEXT:  1c:        e59d0018
; IASM-NEXT:    .byte 0x18
; IASM-NEXT:    .byte 0x0
; IASM-NEXT:    .byte 0x9d
; IASM-NEXT:    .byte 0xe5

; ASM-NEXT:     str     r0, [sp]
; ASM-NEXT:     # [sp] = def.pseudo
; DIS-NEXT:  20:        e58d0000
; IASM-NEXT:    .byte 0x0
; IASM-NEXT:    .byte 0x0
; IASM-NEXT:    .byte 0x8d
; IASM-NEXT:    .byte 0xe5

; ASM-NEXT:     dmb     sy
; DIS-NEXT:  24:        f57ff05f
; IASM-NEXT:    .byte 0x5f
; IASM-NEXT:    .byte 0xf0
; IASM-NEXT:    .byte 0x7f
; IASM-NEXT:    .byte 0xf5

; ASM-NEXT:.LtestI8Form$local$__0:
; IASM-NEXT:.LtestI8Form$local$__0:

; ASM-NEXT:     ldr     r0, [sp]
; DIS-NEXT:  28:        e59d0000
; IASM-NEXT:    .byte 0x0
; IASM-NEXT:    .byte 0x0
; IASM-NEXT:    .byte 0x9d
; IASM-NEXT:    .byte 0xe5

; ASM-NEXT:     ldrb    r1, [sp, #4]
; DIS-NEXT:  2c:        e5dd1004
; IASM-NEXT:    .byte 0x4
; IASM-NEXT:    .byte 0x10
; IASM-NEXT:    .byte 0xdd
; IASM-NEXT:    .byte 0xe5

; ASM-NEXT:     uxtb    r1, r1
; DIS-NEXT:  30:        e6ef1071
; IASM-NEXT:    .byte 0x71
; IASM-NEXT:    .byte 0x10
; IASM-NEXT:    .byte 0xef
; IASM-NEXT:    .byte 0xe6

; ***** Example of ldrexb *****
; ASM-NEXT:     ldrexb  r2, [r0]
; DIS-NEXT:  34:        e1d02f9f
; IASM-NEXT:    .byte 0x9f
; IASM-NEXT:    .byte 0x2f
; IASM-NEXT:    .byte 0xd0
; IASM-NEXT:    .byte 0xe1

; ASM-NEXT:     add     r1, r2, r1
; ASM-NEXT:     # r3 = def.pseudo
; DIS-NEXT:  38:        e0821001
; IASM-NEXT:    .byte 0x1
; IASM-NEXT:    .byte 0x10
; IASM-NEXT:    .byte 0x82
; IASM-NEXT:    .byte 0xe0

; ***** Example of strexb *****
; ASM-NEXT:     strexb  r3, r1, [r0]
; DIS-NEXT:  3c:        e1c03f91
; IASM-NEXT:    .byte 0x91
; IASM-NEXT:    .byte 0x3f
; IASM-NEXT:    .byte 0xc0
; IASM-NEXT:    .byte 0xe1

  %retval = zext i8 %v to i32
  ret i32 %retval
}

define internal i32 @testI16Form(i32 %ptr, i32 %a) {
; ASM-LABEL:testI16Form:
; DIS-LABEL:00000070 <testI16Form>:
; IASM-LABEL:testI16Form:

entry:
; ASM-NEXT:.LtestI16Form$entry:
; IASM-NEXT:.LtestI16Form$entry:

; ASM-NEXT:     sub     sp, sp, #28
; DIS-NEXT:  70:        e24dd01c
; IASM-NEXT:    .byte 0x1c
; IASM-NEXT:    .byte 0xd0
; IASM-NEXT:    .byte 0x4d
; IASM-NEXT:    .byte 0xe2

; ASM-NEXT:     str     r0, [sp, #24]
; ASM-NEXT:     # [sp, #24] = def.pseudo
; DIS-NEXT:  74:        e58d0018
; IASM-NEXT:    .byte 0x18
; IASM-NEXT:    .byte 0x0
; IASM-NEXT:    .byte 0x8d
; IASM-NEXT:    .byte 0xe5

; ASM-NEXT:     str     r1, [sp, #20]
; ASM-NEXT:     # [sp, #20] = def.pseudo
; DIS-NEXT:  78:        e58d1014
; IASM-NEXT:    .byte 0x14
; IASM-NEXT:    .byte 0x10
; IASM-NEXT:    .byte 0x8d
; IASM-NEXT:    .byte 0xe5

  %ptr.asptr = inttoptr i32 %ptr to i16*
  %a.arg_trunc = trunc i32 %a to i16

; ASM-NEXT:     ldr     r0, [sp, #20]
; DIS-NEXT:  7c:        e59d0014
; IASM-NEXT:    .byte 0x14
; IASM-NEXT:    .byte 0x0
; IASM-NEXT:    .byte 0x9d
; IASM-NEXT:    .byte 0xe5

; ASM-NEXT:     strh    r0, [sp, #16]
; ASM-NEXT:     # [sp, #16] = def.pseudo
; DIS-NEXT:  80:        e1cd01b0
; IASM-NEXT:    .byte 0xb0
; IASM-NEXT:    .byte 0x1
; IASM-NEXT:    .byte 0xcd
; IASM-NEXT:    .byte 0xe1

  %v = call i16 @llvm.nacl.atomic.rmw.i16(i32 1, i16* %ptr.asptr,
                                          i16 %a.arg_trunc, i32 6)

; ASM-NEXT:     ldrh    r0, [sp, #16]
; DIS-NEXT:  84:        e1dd01b0
; IASM-NEXT:    .byte 0xb0
; IASM-NEXT:    .byte 0x1
; IASM-NEXT:    .byte 0xdd
; IASM-NEXT:    .byte 0xe1

; ASM-NEXT:     strh    r0, [sp, #4]
; ASM-NEXT:     # [sp, #4] = def.pseudo
; DIS-NEXT:  88:        e1cd00b4
; IASM-NEXT:    .byte 0xb4
; IASM-NEXT:    .byte 0x0
; IASM-NEXT:    .byte 0xcd
; IASM-NEXT:    .byte 0xe1

; ASM-NEXT:     ldr     r0, [sp, #24]
; DIS-NEXT:  8c:        e59d0018
; IASM-NEXT:    .byte 0x18
; IASM-NEXT:    .byte 0x0
; IASM-NEXT:    .byte 0x9d
; IASM-NEXT:    .byte 0xe5

; ASM-NEXT:     str     r0, [sp]
; ASM-NEXT:     # [sp] = def.pseudo
; DIS-NEXT:  90:        e58d0000
; IASM-NEXT:    .byte 0x0
; IASM-NEXT:    .byte 0x0
; IASM-NEXT:    .byte 0x8d
; IASM-NEXT:    .byte 0xe5

; ASM-NEXT:     dmb     sy
; DIS-NEXT:  94:        f57ff05f
; IASM-NEXT:    .byte 0x5f
; IASM-NEXT:    .byte 0xf0
; IASM-NEXT:    .byte 0x7f
; IASM-NEXT:    .byte 0xf5

; ASM-NEXT:.LtestI16Form$local$__0:
; IASM-NEXT:.LtestI16Form$local$__0:

; ASM-NEXT:     ldr     r0, [sp]
; DIS-NEXT:  98:        e59d0000
; IASM-NEXT:    .byte 0x0
; IASM-NEXT:    .byte 0x0
; IASM-NEXT:    .byte 0x9d
; IASM-NEXT:    .byte 0xe5

; ASM-NEXT:     ldrh    r1, [sp, #4]
; DIS-NEXT:  9c:        e1dd10b4
; IASM-NEXT:    .byte 0xb4
; IASM-NEXT:    .byte 0x10
; IASM-NEXT:    .byte 0xdd
; IASM-NEXT:    .byte 0xe1

; ASM-NEXT:     uxth    r1, r1
; DIS-NEXT:  a0:        e6ff1071
; IASM-NEXT:    .byte 0x71
; IASM-NEXT:    .byte 0x10
; IASM-NEXT:    .byte 0xff
; IASM-NEXT:    .byte 0xe6

; ***** Example of ldrexh *****
; ASM-NEXT:     ldrexh  r2, [r0]
; DIS-NEXT:  a4:        e1f02f9f
; IASM-NEXT:    .byte 0x9f
; IASM-NEXT:    .byte 0x2f
; IASM-NEXT:    .byte 0xf0
; IASM-NEXT:    .byte 0xe1

; ASM-NEXT:     add     r1, r2, r1
; ASM-NEXT:     # r3 = def.pseudo
; DIS-NEXT:  a8:        e0821001
; IASM-NEXT:        .byte 0x1
; IASM-NEXT:        .byte 0x10
; IASM-NEXT:        .byte 0x82
; IASM-NEXT:        .byte 0xe0

; ***** Example of strexh *****
; ASM-NEXT:     strexh  r3, r1, [r0]
; DIS-NEXT:  ac:        e1e03f91
; IASM-NEXT:    .byte 0x91
; IASM-NEXT:    .byte 0x3f
; IASM-NEXT:    .byte 0xe0
; IASM-NEXT:    .byte 0xe1

  %retval = zext i16 %v to i32
  ret i32 %retval
}

define internal i32 @testI32Form(i32 %ptr, i32 %a) {
; ASM-LABEL:testI32Form:
; DIS-LABEL:000000e0 <testI32Form>:
; IASM-LABEL:testI32Form:

entry:
; ASM-NEXT:.LtestI32Form$entry:
; IASM-NEXT:.LtestI32Form$entry:

; ASM-NEXT:     sub     sp, sp, #20
; DIS-NEXT:  e0:        e24dd014
; IASM-NEXT:    .byte 0x14
; IASM-NEXT:    .byte 0xd0
; IASM-NEXT:    .byte 0x4d
; IASM-NEXT:    .byte 0xe2

; ASM-NEXT:     str     r0, [sp, #16]
; ASM-NEXT:     # [sp, #16] = def.pseudo
; DIS-NEXT:  e4:        e58d0010
; IASM-NEXT:    .byte 0x10
; IASM-NEXT:    .byte 0x0
; IASM-NEXT:    .byte 0x8d
; IASM-NEXT:    .byte 0xe5

; ASM-NEXT:     str     r1, [sp, #12]
; ASM-NEXT:     # [sp, #12] = def.pseudo
; DIS-NEXT:  e8:        e58d100c
; IASM-NEXT:    .byte 0xc
; IASM-NEXT:    .byte 0x10
; IASM-NEXT:    .byte 0x8d
; IASM-NEXT:    .byte 0xe5

  %ptr.asptr = inttoptr i32 %ptr to i32*
  %v = call i32 @llvm.nacl.atomic.rmw.i32(i32 1, i32* %ptr.asptr,
                                          i32 %a, i32 6)

; ASM-NEXT:     ldr     r0, [sp, #12]
; DIS-NEXT:  ec:        e59d000c
; IASM-NEXT:    .byte 0xc
; IASM-NEXT:    .byte 0x0
; IASM-NEXT:    .byte 0x9d
; IASM-NEXT:    .byte 0xe5

; ASM-NEXT:     str     r0, [sp, #4]
; ASM-NEXT:     # [sp, #4] = def.pseudo
; DIS-NEXT:  f0:        e58d0004
; IASM-NEXT:    .byte 0x4
; IASM-NEXT:    .byte 0x0
; IASM-NEXT:    .byte 0x8d
; IASM-NEXT:    .byte 0xe5

; ASM-NEXT:     ldr     r0, [sp, #16]
; DIS-NEXT:  f4:        e59d0010
; IASM-NEXT:    .byte 0x10
; IASM-NEXT:    .byte 0x0
; IASM-NEXT:    .byte 0x9d
; IASM-NEXT:    .byte 0xe5

; ASM-NEXT:     str     r0, [sp]
; ASM-NEXT:     # [sp] = def.pseudo
; DIS-NEXT:  f8:        e58d0000
; IASM-NEXT:    .byte 0x0
; IASM-NEXT:    .byte 0x0
; IASM-NEXT:    .byte 0x8d
; IASM-NEXT:    .byte 0xe5

; ASM-NEXT:     dmb     sy
; DIS-NEXT:  fc:        f57ff05f
; IASM-NEXT:    .byte 0x5f
; IASM-NEXT:    .byte 0xf0
; IASM-NEXT:    .byte 0x7f
; IASM-NEXT:    .byte 0xf5

; ASM-NEXT:.LtestI32Form$local$__0:
; IASM-NEXT:.LtestI32Form$local$__0:

; ASM-NEXT:     ldr     r0, [sp]
; DIS-NEXT: 100:        e59d0000
; IASM-NEXT:    .byte 0x0
; IASM-NEXT:    .byte 0x0
; IASM-NEXT:    .byte 0x9d
; IASM-NEXT:    .byte 0xe5

; ASM-NEXT:     ldr     r1, [sp, #4]
; DIS-NEXT: 104:        e59d1004
; IASM-NEXT:    .byte 0x4
; IASM-NEXT:    .byte 0x10
; IASM-NEXT:    .byte 0x9d
; IASM-NEXT:    .byte 0xe5

; ***** Example of ldrex *****
; ASM-NEXT:     ldrex   r2, [r0]
; DIS-NEXT: 108:        e1902f9f
; IASM-NEXT:    .byte 0x9f
; IASM-NEXT:    .byte 0x2f
; IASM-NEXT:    .byte 0x90
; IASM-NEXT:    .byte 0xe1

; ASM-NEXT:     add     r1, r2, r1
; ASM-NEXT:     # r3 = def.pseudo
; DIS-NEXT: 10c:        e0821001
; IASM-NEXT:    .byte 0x1
; IASM-NEXT:    .byte 0x10
; IASM-NEXT:    .byte 0x82
; IASM-NEXT:    .byte 0xe0

; ***** Example of strex *****
; ASM-NEXT:     strex   r3, r1, [r0]
; DIS-NEXT: 110:        e1803f91
; IASM-NEXT:    .byte 0x91
; IASM-NEXT:    .byte 0x3f
; IASM-NEXT:    .byte 0x80
; IASM-NEXT:    .byte 0xe1

  ret i32 %v
}

define internal i64 @testI64Form(i32 %ptr, i64 %a) {
; ASM-LABEL:testI64Form:
; DIS-LABEL:00000130 <testI64Form>:
; IASM-LABEL:testI64Form:

entry:
; ASM-NEXT:.LtestI64Form$entry:
; IASM-NEXT:.LtestI64Form$entry:

; ASM-NEXT:     push    {r4, r5}
; DIS-NEXT: 130:        e92d0030
; IASM-NEXT:    .byte 0x30
; IASM-NEXT:    .byte 0x0
; IASM-NEXT:    .byte 0x2d
; IASM-NEXT:    .byte 0xe9

; ASM-NEXT:     sub     sp, sp, #32
; DIS-NEXT: 134:        e24dd020
; IASM-NEXT:    .byte 0x20
; IASM-NEXT:    .byte 0xd0
; IASM-NEXT:    .byte 0x4d
; IASM-NEXT:    .byte 0xe2

; ASM-NEXT:     str     r0, [sp, #28]
; ASM-NEXT:     # [sp, #28] = def.pseudo
; DIS-NEXT: 138:        e58d001c
; IASM-NEXT:    .byte 0x1c
; IASM-NEXT:    .byte 0x0
; IASM-NEXT:    .byte 0x8d
; IASM-NEXT:    .byte 0xe5

; ASM-NEXT:     mov     r0, r2
; DIS-NEXT: 13c:        e1a00002
; IASM-NEXT:    .byte 0x2
; IASM-NEXT:    .byte 0x0
; IASM-NEXT:    .byte 0xa0
; IASM-NEXT:    .byte 0xe1

; ASM-NEXT:     str     r0, [sp, #24]
; ASM-NEXT:     # [sp, #24] = def.pseudo
; DIS-NEXT: 140:        e58d0018
; IASM-NEXT:    .byte 0x18
; IASM-NEXT:    .byte 0x0
; IASM-NEXT:    .byte 0x8d
; IASM-NEXT:    .byte 0xe5

; ASM-NEXT:     mov     r0, r3
; DIS-NEXT: 144:        e1a00003
; IASM-NEXT:    .byte 0x3
; IASM-NEXT:    .byte 0x0
; IASM-NEXT:    .byte 0xa0
; IASM-NEXT:    .byte 0xe1

; ASM-NEXT:     str     r0, [sp, #20]
; ASM-NEXT:     # [sp, #20] = def.pseudo
; ASM-NEXT:     # [sp] = def.pseudo
; DIS-NEXT: 148:        e58d0014
; IASM-NEXT:    .byte 0x14
; IASM-NEXT:    .byte 0x0
; IASM-NEXT:    .byte 0x8d
; IASM-NEXT:    .byte 0xe5

  %ptr.asptr = inttoptr i32 %ptr to i64*
  %v = call i64 @llvm.nacl.atomic.rmw.i64(i32 1, i64* %ptr.asptr,
                                          i64 %a, i32 6)

; ASM-NEXT:     ldr     r0, [sp, #24]
; DIS-NEXT: 14c:        e59d0018
; IASM-NEXT:    .byte 0x18
; IASM-NEXT:    .byte 0x0
; IASM-NEXT:    .byte 0x9d
; IASM-NEXT:    .byte 0xe5

; ASM-NEXT:     str     r0, [sp, #8]
; ASM-NEXT:     # [sp, #8] = def.pseudo
; DIS-NEXT: 150:        e58d0008
; IASM-NEXT:    .byte 0x8
; IASM-NEXT:    .byte 0x0
; IASM-NEXT:    .byte 0x8d
; IASM-NEXT:    .byte 0xe5

; ASM-NEXT:     ldr     r0, [sp, #20]
; DIS-NEXT: 154:        e59d0014
; IASM-NEXT:    .byte 0x14
; IASM-NEXT:    .byte 0x0
; IASM-NEXT:    .byte 0x9d
; IASM-NEXT:    .byte 0xe5

; ASM-NEXT:     str     r0, [sp, #4]
; ASM-NEXT:     # [sp, #4] = def.pseudo
; DIS-NEXT: 158:        e58d0004
; IASM-NEXT:    .byte 0x4
; IASM-NEXT:    .byte 0x0
; IASM-NEXT:    .byte 0x8d
; IASM-NEXT:    .byte 0xe5

; ASM-NEXT:     ldr     r0, [sp, #28]
; DIS-NEXT: 15c:        e59d001c
; IASM-NEXT:    .byte 0x1c
; IASM-NEXT:    .byte 0x0
; IASM-NEXT:    .byte 0x9d
; IASM-NEXT:    .byte 0xe5

; ASM-NEXT:     str     r0, [sp]
; ASM-NEXT:     # [sp] = def.pseudo
; DIS-NEXT: 160:        e58d0000
; IASM-NEXT:    .byte 0x0
; IASM-NEXT:    .byte 0x0
; IASM-NEXT:    .byte 0x8d
; IASM-NEXT:    .byte 0xe5

; ASM-NEXT:     dmb     sy
; DIS-NEXT: 164:        f57ff05f
; IASM-NEXT:    .byte 0x5f
; IASM-NEXT:    .byte 0xf0
; IASM-NEXT:    .byte 0x7f
; IASM-NEXT:    .byte 0xf5

; ASM-NEXT:.LtestI64Form$local$__0:
; IASM-NEXT:.LtestI64Form$local$__0:

; ASM-NEXT:     ldr     r0, [sp]
; ASM-NEXT:     # r2, r3 = def.pseudo [sp]
; DIS-NEXT: 168:        e59d0000
; IASM-NEXT:    .byte 0x0
; IASM-NEXT:    .byte 0x0
; IASM-NEXT:    .byte 0x9d
; IASM-NEXT:    .byte 0xe5

; ASM-NEXT:     ldr     r1, [sp, #8]
; DIS-NEXT: 16c:        e59d1008
; IASM-NEXT:    .byte 0x8
; IASM-NEXT:    .byte 0x10
; IASM-NEXT:    .byte 0x9d
; IASM-NEXT:    .byte 0xe5

; ASM-NEXT:     mov     r2, r1
; DIS-NEXT: 170:        e1a02001
; IASM-NEXT:    .byte 0x1
; IASM-NEXT:    .byte 0x20
; IASM-NEXT:    .byte 0xa0
; IASM-NEXT:    .byte 0xe1

; ASM-NEXT:     ldr     r1, [sp, #4]
; DIS-NEXT: 174:        e59d1004
; IASM-NEXT:    .byte 0x4
; IASM-NEXT:    .byte 0x10
; IASM-NEXT:    .byte 0x9d
; IASM-NEXT:    .byte 0xe5

; ASM-NEXT:     mov     r3, r1
; DIS-NEXT: 178:        e1a03001
; IASM-NEXT:    .byte 0x1
; IASM-NEXT:    .byte 0x30
; IASM-NEXT:    .byte 0xa0
; IASM-NEXT:    .byte 0xe1

; ***** Example of ldrexd *****
; ASM-NEXT:     ldrexd  r4, r5, [r0]
; ASM-NEXT:     # r4 = def.pseudo r4, r5
; ASM-NEXT:     # r5 = def.pseudo r4, r5
; ASM-NEXT:     # r2, r3 = def.pseudo r2, r3
; DIS-NEXT: 17c:        e1b04f9f
; IASM-NEXT:    .byte 0x9f
; IASM-NEXT:    .byte 0x4f
; IASM-NEXT:    .byte 0xb0
; IASM-NEXT:    .byte 0xe1

; ASM-NEXT:     adds    r2, r4, r2
; DIS-NEXT: 180:        e0942002
; IASM-NEXT:    .byte 0x2
; IASM-NEXT:    .byte 0x20
; IASM-NEXT:    .byte 0x94
; IASM-NEXT:    .byte 0xe0

; ASM-NEXT:     adc     r3, r5, r3
; ASM-NEXT:     # r1 = def.pseudo
; DIS-NEXT: 184:        e0a53003
; IASM-NEXT:    .byte 0x3
; IASM-NEXT:    .byte 0x30
; IASM-NEXT:    .byte 0xa5
; IASM-NEXT:    .byte 0xe0

; ***** Example of strexd *****
; ASM-NEXT:     strexd  r1, r2, r3, [r0]
; DIS-NEXT: 188:        e1a01f92
; IASM-NEXT:    .byte 0x92
; IASM-NEXT:    .byte 0x1f
; IASM-NEXT:    .byte 0xa0
; IASM-NEXT:    .byte 0xe1

  ret i64 %v
}
