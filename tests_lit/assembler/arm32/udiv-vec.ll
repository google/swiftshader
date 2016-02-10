; Show that we know how to translate vector division instructions.

; REQUIRES: allow_dump

; Compile using standalone assembler.
; RUN: %p2i --filetype=asm -i %s --target=arm32 --args -O2 -mattr=hwdiv-arm \
; RUN:   | FileCheck %s --check-prefix=ASM

; Show bytes in assembled standalone code.
; RUN: %p2i --filetype=asm -i %s --target=arm32 --assemble --disassemble \
; RUN:   --args -O2 -mattr=hwdiv-arm \
; RUN:   | FileCheck %s --check-prefix=DIS

; Compile using integrated assembler.
; RUN: %p2i --filetype=iasm -i %s --target=arm32 --args -O2 -mattr=hwdiv-arm \
; RUN:   | FileCheck %s --check-prefix=IASM

; Show bytes in assembled integrated code.
; RUN: %p2i --filetype=iasm -i %s --target=arm32 --assemble --disassemble \
; RUN:   --args -O2 -mattr=hwdiv-arm \
; RUN:   | FileCheck %s --check-prefix=DIS

define internal <4 x float> @testVdivFloat4(<4 x float> %v1, <4 x float> %v2) {
; ASM-LABEL: testVdivFloat4:
; DIS-LABEL: 00000000 <testVdivFloat4>:
; IASM-LABEL: testVdivFloat4:

entry:
  %res = fdiv <4 x float> %v1, %v2

; TODO(eholk): this code could be a lot better. Fix the code generator
; and update the test. Same for the rest of the tests.

; ASM:     vdiv.f32        s8, s8, s9
; ASM:     vdiv.f32        s8, s8, s9
; ASM:     vdiv.f32        s8, s8, s9
; ASM:     vdiv.f32        s0, s0, s4

; DIS:   8:	ee844a24
; DIS:  1c:	ee844a24
; DIS:  2c:	ee844a24
; DIS:  3c:	ee800a02

; IASM-NOT:     vdiv

  ret <4 x float> %res
}

define internal <4 x i32> @testVdiv4i32(<4 x i32> %v1, <4 x i32> %v2) {
; ASM-LABEL: testVdiv4i32:
; DIS-LABEL: 00000050 <testVdiv4i32>:
; IASM-LABEL: testVdiv4i32:

entry:
  %res = udiv <4 x i32> %v1, %v2

; ASM:     udiv r0, r0, r1
; ASM:     udiv r0, r0, r1
; ASM:     udiv r0, r0, r1
; ASM:     udiv r0, r0, r1

; DIS:  64:	e730f110
; DIS:  84:	e730f110
; DIS:  a0:	e730f110
; DIS:  bc:	e730f110

; IASM-NOT:     udiv

  ret <4 x i32> %res
}

define internal <8 x i16> @testVdiv8i16(<8 x i16> %v1, <8 x i16> %v2) {
; ASM-LABEL: testVdiv8i16:
; DIS-LABEL: 000000d0 <testVdiv8i16>:
; IASM-LABEL: testVdiv8i16:

entry:
  %res = udiv <8 x i16> %v1, %v2

; ASM:     uxth            r0, r0
; ASM:     uxth            r1, r1
; ASM:     udiv r0, r0, r1
; ASM:     uxth            r0, r0
; ASM:     uxth            r1, r1
; ASM:     udiv r0, r0, r1
; ASM:     uxth            r0, r0
; ASM:     uxth            r1, r1
; ASM:     udiv r0, r0, r1
; ASM:     uxth            r0, r0
; ASM:     uxth            r1, r1
; ASM:     udiv r0, r0, r1
; ASM:     uxth            r0, r0
; ASM:     uxth            r1, r1
; ASM:     udiv r0, r0, r1
; ASM:     uxth            r0, r0
; ASM:     uxth            r1, r1
; ASM:     udiv r0, r0, r1
; ASM:     uxth            r0, r0
; ASM:     uxth            r1, r1
; ASM:     udiv r0, r0, r1
; ASM:     uxth            r0, r0
; ASM:     uxth            r1, r1
; ASM:     udiv r0, r0, r1

; DIS:  e4:	e6ff0070
; DIS:  e8:	e6ff1071
; DIS:  ec:	e730f110
; DIS: 10c:	e6ff0070
; DIS: 110:	e6ff1071
; DIS: 114:	e730f110
; DIS: 130:	e6ff0070
; DIS: 134:	e6ff1071
; DIS: 138:	e730f110
; DIS: 154:	e6ff0070
; DIS: 158:	e6ff1071
; DIS: 15c:	e730f110
; DIS: 178:	e6ff0070
; DIS: 17c:	e6ff1071
; DIS: 180:	e730f110
; DIS: 19c:	e6ff0070
; DIS: 1a0:	e6ff1071
; DIS: 1a4:	e730f110
; DIS: 1c0:	e6ff0070
; DIS: 1c4:	e6ff1071
; DIS: 1c8:	e730f110
; DIS: 1e4:	e6ff0070
; DIS: 1e8:	e6ff1071
; DIS: 1ec:	e730f110

; IASM-NOT:     uxth
; IASM-NOT:     udiv

  ret <8 x i16> %res
}

define internal <16 x i8> @testVdiv16i8(<16 x i8> %v1, <16 x i8> %v2) {
; ASM-LABEL: testVdiv16i8:
; DIS-LABEL: 00000200 <testVdiv16i8>:
; IASM-LABEL: testVdiv16i8:

entry:
  %res = udiv <16 x i8> %v1, %v2

; ASM:     uxtb            r0, r0
; ASM:     uxtb            r1, r1
; ASM:     udiv r0, r0, r1
; ASM:     uxtb            r0, r0
; ASM:     uxtb            r1, r1
; ASM:     udiv r0, r0, r1
; ASM:     uxtb            r0, r0
; ASM:     uxtb            r1, r1
; ASM:     udiv r0, r0, r1
; ASM:     uxtb            r0, r0
; ASM:     uxtb            r1, r1
; ASM:     udiv r0, r0, r1
; ASM:     uxtb            r0, r0
; ASM:     uxtb            r1, r1
; ASM:     udiv r0, r0, r1
; ASM:     uxtb            r0, r0
; ASM:     uxtb            r1, r1
; ASM:     udiv r0, r0, r1
; ASM:     uxtb            r0, r0
; ASM:     uxtb            r1, r1
; ASM:     udiv r0, r0, r1
; ASM:     uxtb            r0, r0
; ASM:     uxtb            r1, r1
; ASM:     udiv r0, r0, r1
; ASM:     uxtb            r0, r0
; ASM:     uxtb            r1, r1
; ASM:     udiv r0, r0, r1
; ASM:     uxtb            r0, r0
; ASM:     uxtb            r1, r1
; ASM:     udiv r0, r0, r1
; ASM:     uxtb            r0, r0
; ASM:     uxtb            r1, r1
; ASM:     udiv r0, r0, r1
; ASM:     uxtb            r0, r0
; ASM:     uxtb            r1, r1
; ASM:     udiv r0, r0, r1
; ASM:     uxtb            r0, r0
; ASM:     uxtb            r1, r1
; ASM:     udiv r0, r0, r1
; ASM:     uxtb            r0, r0
; ASM:     uxtb            r1, r1
; ASM:     udiv r0, r0, r1
; ASM:     uxtb            r0, r0
; ASM:     uxtb            r1, r1
; ASM:     udiv r0, r0, r1
; ASM:     uxtb            r0, r0
; ASM:     uxtb            r1, r1
; ASM:     udiv r0, r0, r1

; DIS: 214:	e6ef0070
; DIS: 218:	e6ef1071
; DIS: 21c:	e730f110
; DIS: 23c:	e6ef0070
; DIS: 240:	e6ef1071
; DIS: 244:	e730f110
; DIS: 260:	e6ef0070
; DIS: 264:	e6ef1071
; DIS: 268:	e730f110
; DIS: 284:	e6ef0070
; DIS: 288:	e6ef1071
; DIS: 28c:	e730f110
; DIS: 2a8:	e6ef0070
; DIS: 2ac:	e6ef1071
; DIS: 2b0:	e730f110
; DIS: 2cc:	e6ef0070
; DIS: 2d0:	e6ef1071
; DIS: 2d4:	e730f110
; DIS: 2f0:	e6ef0070
; DIS: 2f4:	e6ef1071
; DIS: 2f8:	e730f110
; DIS: 314:	e6ef0070
; DIS: 318:	e6ef1071
; DIS: 31c:	e730f110
; DIS: 338:	e6ef0070
; DIS: 33c:	e6ef1071
; DIS: 340:	e730f110
; DIS: 35c:	e6ef0070
; DIS: 360:	e6ef1071
; DIS: 364:	e730f110
; DIS: 380:	e6ef0070
; DIS: 384:	e6ef1071
; DIS: 388:	e730f110
; DIS: 3a4:	e6ef0070
; DIS: 3a8:	e6ef1071
; DIS: 3ac:	e730f110
; DIS: 3c8:	e6ef0070
; DIS: 3cc:	e6ef1071
; DIS: 3d0:	e730f110
; DIS: 3ec:	e6ef0070
; DIS: 3f0:	e6ef1071
; DIS: 3f4:	e730f110
; DIS: 410:	e6ef0070
; DIS: 414:	e6ef1071
; DIS: 418:	e730f110
; DIS: 434:	e6ef0070
; DIS: 438:	e6ef1071
; DIS: 43c:	e730f110

; IASM-NOT:     uxtb
; IASM-NOT:     udiv

  ret <16 x i8> %res
}
