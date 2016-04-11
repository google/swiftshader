; Test that we handle select on vectors.

; TODO(eholk): This test will need to be updated once comparison is no longer
; scalarized.

; REQUIRES: allow_dump

; Compile using standalone assembler.
; RUN: %p2i --filetype=asm -i %s --target=arm32 --args -O2 \
; RUN:   | FileCheck %s --check-prefix=ASM

define internal <4 x float> @select4float(<4 x i1> %s, <4 x float> %a,
                                          <4 x float> %b) {
; ASM-LABEL:select4float:
; DIS-LABEL:00000000 <select4float>:

entry:
  %res = select <4 x i1> %s, <4 x float> %a, <4 x float> %b

; ASM:	# q3 = def.pseudo
; ASM-NEXT:	vmov.s8	r0, d0[0]
; ASM-NEXT:	vmov.f32	s16, s4
; ASM-NEXT:	vmov.f32	s17, s8
; ASM-NEXT:	tst	r0, #1
; ASM-NEXT:	vmovne.f32	s17, s16
; ASM-NEXT:	vmov.f32	s12, s17
; ASM-NEXT:	vmov.s8	r0, d0[4]
; ASM-NEXT:	vmov.f32	s16, s5
; ASM-NEXT:	vmov.f32	s17, s9
; ASM-NEXT:	tst	r0, #1
; ASM-NEXT:	vmovne.f32	s17, s16
; ASM-NEXT:	vmov.f32	s13, s17
; ASM-NEXT:	vmov.s8	r0, d1[0]
; ASM-NEXT:	vmov.f32	s16, s6
; ASM-NEXT:	vmov.f32	s17, s10
; ASM-NEXT:	tst	r0, #1
; ASM-NEXT:	vmovne.f32	s17, s16
; ASM-NEXT:	vmov.f32	s14, s17
; ASM-NEXT:	vmov.s8	r0, d1[4]
; ASM-NEXT:	vmov.f32	s4, s7
; ASM-NEXT:	vmov.f32	s8, s11
; ASM-NEXT:	tst	r0, #1
; ASM-NEXT:	vmovne.f32	s8, s4
; ASM-NEXT:	vmov.f32	s15, s8
; ASM-NEXT:	vmov.f32	q0, q3
; ASM-NEXT:	vpop	{s16, s17}
; ASM-NEXT:	# s16 = def.pseudo
; ASM-NEXT:	# s17 = def.pseudo
; ASM-NEXT:	bx	lr

  ret <4 x float> %res
}

define internal <4 x i32> @select4i32(<4 x i1> %s, <4 x i32> %a, <4 x i32> %b) {
; ASM-LABEL:select4i32:
; DIS-LABEL:00000000 <select4i32>:

entry:
  %res = select <4 x i1> %s, <4 x i32> %a, <4 x i32> %b

; ASM:	# q3 = def.pseudo
; ASM-NEXT:	vmov.s8	r0, d0[0]
; ASM-NEXT:	vmov.32	r1, d2[0]
; ASM-NEXT:	vmov.32	r2, d4[0]
; ASM-NEXT:	tst	r0, #1
; ASM-NEXT:	movne	r2, r1
; ASM-NEXT:	vmov.32	d6[0], r2
; ASM-NEXT:	vmov.s8	r0, d0[4]
; ASM-NEXT:	vmov.32	r1, d2[1]
; ASM-NEXT:	vmov.32	r2, d4[1]
; ASM-NEXT:	tst	r0, #1
; ASM-NEXT:	movne	r2, r1
; ASM-NEXT:	vmov.32	d6[1], r2
; ASM-NEXT:	vmov.s8	r0, d1[0]
; ASM-NEXT:	vmov.32	r1, d3[0]
; ASM-NEXT:	vmov.32	r2, d5[0]
; ASM-NEXT:	tst	r0, #1
; ASM-NEXT:	movne	r2, r1
; ASM-NEXT:	vmov.32	d7[0], r2
; ASM-NEXT:	vmov.s8	r0, d1[4]
; ASM-NEXT:	vmov.32	r1, d3[1]
; ASM-NEXT:	vmov.32	r2, d5[1]
; ASM-NEXT:	tst	r0, #1
; ASM-NEXT:	movne	r2, r1
; ASM-NEXT:	vmov.32	d7[1], r2
; ASM-NEXT:	vmov.i32	q0, q3
; ASM-NEXT:	bx	lr

  ret <4 x i32> %res
}

define internal <8 x i16> @select8i16(<8 x i1> %s, <8 x i16> %a, <8 x i16> %b) {
; ASM-LABEL:select8i16:
; DIS-LABEL:00000000 <select8i16>:

entry:
  %res = select <8 x i1> %s, <8 x i16> %a, <8 x i16> %b

; ASM:	# q3 = def.pseudo
; ASM-NEXT:	vmov.s8	r0, d0[0]
; ASM-NEXT:	vmov.s16	r1, d2[0]
; ASM-NEXT:	vmov.s16	r2, d4[0]
; ASM-NEXT:	tst	r0, #1
; ASM-NEXT:	movne	r2, r1
; ASM-NEXT:	vmov.16	d6[0], r2
; ASM-NEXT:	vmov.s8	r0, d0[2]
; ASM-NEXT:	vmov.s16	r1, d2[1]
; ASM-NEXT:	vmov.s16	r2, d4[1]
; ASM-NEXT:	tst	r0, #1
; ASM-NEXT:	movne	r2, r1
; ASM-NEXT:	vmov.16	d6[1], r2
; ASM-NEXT:	vmov.s8	r0, d0[4]
; ASM-NEXT:	vmov.s16	r1, d2[2]
; ASM-NEXT:	vmov.s16	r2, d4[2]
; ASM-NEXT:	tst	r0, #1
; ASM-NEXT:	movne	r2, r1
; ASM-NEXT:	vmov.16	d6[2], r2
; ASM-NEXT:	vmov.s8	r0, d0[6]
; ASM-NEXT:	vmov.s16	r1, d2[3]
; ASM-NEXT:	vmov.s16	r2, d4[3]
; ASM-NEXT:	tst	r0, #1
; ASM-NEXT:	movne	r2, r1
; ASM-NEXT:	vmov.16	d6[3], r2
; ASM-NEXT:	vmov.s8	r0, d1[0]
; ASM-NEXT:	vmov.s16	r1, d3[0]
; ASM-NEXT:	vmov.s16	r2, d5[0]
; ASM-NEXT:	tst	r0, #1
; ASM-NEXT:	movne	r2, r1
; ASM-NEXT:	vmov.16	d7[0], r2
; ASM-NEXT:	vmov.s8	r0, d1[2]
; ASM-NEXT:	vmov.s16	r1, d3[1]
; ASM-NEXT:	vmov.s16	r2, d5[1]
; ASM-NEXT:	tst	r0, #1
; ASM-NEXT:	movne	r2, r1
; ASM-NEXT:	vmov.16	d7[1], r2
; ASM-NEXT:	vmov.s8	r0, d1[4]
; ASM-NEXT:	vmov.s16	r1, d3[2]
; ASM-NEXT:	vmov.s16	r2, d5[2]
; ASM-NEXT:	tst	r0, #1
; ASM-NEXT:	movne	r2, r1
; ASM-NEXT:	vmov.16	d7[2], r2
; ASM-NEXT:	vmov.s8	r0, d1[6]
; ASM-NEXT:	vmov.s16	r1, d3[3]
; ASM-NEXT:	vmov.s16	r2, d5[3]
; ASM-NEXT:	tst	r0, #1
; ASM-NEXT:	movne	r2, r1
; ASM-NEXT:	vmov.16	d7[3], r2
; ASM-NEXT:	vmov.i16	q0, q3
; ASM-NEXT:	bx	lr

  ret <8 x i16> %res
}

define internal <16 x i8> @select16i8(<16 x i1> %s, <16 x i8> %a,
                                      <16 x i8> %b) {
; ASM-LABEL:select16i8:
; DIS-LABEL:00000000 <select16i8>:

entry:
  %res = select <16 x i1> %s, <16 x i8> %a, <16 x i8> %b

; ASM:	# q3 = def.pseudo
; ASM-NEXT:	vmov.s8	r0, d0[0]
; ASM-NEXT:	vmov.s8	r1, d2[0]
; ASM-NEXT:	vmov.s8	r2, d4[0]
; ASM-NEXT:	tst	r0, #1
; ASM-NEXT:	movne	r2, r1
; ASM-NEXT:	vmov.8	d6[0], r2
; ASM-NEXT:	vmov.s8	r0, d0[1]
; ASM-NEXT:	vmov.s8	r1, d2[1]
; ASM-NEXT:	vmov.s8	r2, d4[1]
; ASM-NEXT:	tst	r0, #1
; ASM-NEXT:	movne	r2, r1
; ASM-NEXT:	vmov.8	d6[1], r2
; ASM-NEXT:	vmov.s8	r0, d0[2]
; ASM-NEXT:	vmov.s8	r1, d2[2]
; ASM-NEXT:	vmov.s8	r2, d4[2]
; ASM-NEXT:	tst	r0, #1
; ASM-NEXT:	movne	r2, r1
; ASM-NEXT:	vmov.8	d6[2], r2
; ASM-NEXT:	vmov.s8	r0, d0[3]
; ASM-NEXT:	vmov.s8	r1, d2[3]
; ASM-NEXT:	vmov.s8	r2, d4[3]
; ASM-NEXT:	tst	r0, #1
; ASM-NEXT:	movne	r2, r1
; ASM-NEXT:	vmov.8	d6[3], r2
; ASM-NEXT:	vmov.s8	r0, d0[4]
; ASM-NEXT:	vmov.s8	r1, d2[4]
; ASM-NEXT:	vmov.s8	r2, d4[4]
; ASM-NEXT:	tst	r0, #1
; ASM-NEXT:	movne	r2, r1
; ASM-NEXT:	vmov.8	d6[4], r2
; ASM-NEXT:	vmov.s8	r0, d0[5]
; ASM-NEXT:	vmov.s8	r1, d2[5]
; ASM-NEXT:	vmov.s8	r2, d4[5]
; ASM-NEXT:	tst	r0, #1
; ASM-NEXT:	movne	r2, r1
; ASM-NEXT:	vmov.8	d6[5], r2
; ASM-NEXT:	vmov.s8	r0, d0[6]
; ASM-NEXT:	vmov.s8	r1, d2[6]
; ASM-NEXT:	vmov.s8	r2, d4[6]
; ASM-NEXT:	tst	r0, #1
; ASM-NEXT:	movne	r2, r1
; ASM-NEXT:	vmov.8	d6[6], r2
; ASM-NEXT:	vmov.s8	r0, d0[7]
; ASM-NEXT:	vmov.s8	r1, d2[7]
; ASM-NEXT:	vmov.s8	r2, d4[7]
; ASM-NEXT:	tst	r0, #1
; ASM-NEXT:	movne	r2, r1
; ASM-NEXT:	vmov.8	d6[7], r2
; ASM-NEXT:	vmov.s8	r0, d1[0]
; ASM-NEXT:	vmov.s8	r1, d3[0]
; ASM-NEXT:	vmov.s8	r2, d5[0]
; ASM-NEXT:	tst	r0, #1
; ASM-NEXT:	movne	r2, r1
; ASM-NEXT:	vmov.8	d7[0], r2
; ASM-NEXT:	vmov.s8	r0, d1[1]
; ASM-NEXT:	vmov.s8	r1, d3[1]
; ASM-NEXT:	vmov.s8	r2, d5[1]
; ASM-NEXT:	tst	r0, #1
; ASM-NEXT:	movne	r2, r1
; ASM-NEXT:	vmov.8	d7[1], r2
; ASM-NEXT:	vmov.s8	r0, d1[2]
; ASM-NEXT:	vmov.s8	r1, d3[2]
; ASM-NEXT:	vmov.s8	r2, d5[2]
; ASM-NEXT:	tst	r0, #1
; ASM-NEXT:	movne	r2, r1
; ASM-NEXT:	vmov.8	d7[2], r2
; ASM-NEXT:	vmov.s8	r0, d1[3]
; ASM-NEXT:	vmov.s8	r1, d3[3]
; ASM-NEXT:	vmov.s8	r2, d5[3]
; ASM-NEXT:	tst	r0, #1
; ASM-NEXT:	movne	r2, r1
; ASM-NEXT:	vmov.8	d7[3], r2
; ASM-NEXT:	vmov.s8	r0, d1[4]
; ASM-NEXT:	vmov.s8	r1, d3[4]
; ASM-NEXT:	vmov.s8	r2, d5[4]
; ASM-NEXT:	tst	r0, #1
; ASM-NEXT:	movne	r2, r1
; ASM-NEXT:	vmov.8	d7[4], r2
; ASM-NEXT:	vmov.s8	r0, d1[5]
; ASM-NEXT:	vmov.s8	r1, d3[5]
; ASM-NEXT:	vmov.s8	r2, d5[5]
; ASM-NEXT:	tst	r0, #1
; ASM-NEXT:	movne	r2, r1
; ASM-NEXT:	vmov.8	d7[5], r2
; ASM-NEXT:	vmov.s8	r0, d1[6]
; ASM-NEXT:	vmov.s8	r1, d3[6]
; ASM-NEXT:	vmov.s8	r2, d5[6]
; ASM-NEXT:	tst	r0, #1
; ASM-NEXT:	movne	r2, r1
; ASM-NEXT:	vmov.8	d7[6], r2
; ASM-NEXT:	vmov.s8	r0, d1[7]
; ASM-NEXT:	vmov.s8	r1, d3[7]
; ASM-NEXT:	vmov.s8	r2, d5[7]
; ASM-NEXT:	tst	r0, #1
; ASM-NEXT:	movne	r2, r1
; ASM-NEXT:	vmov.8	d7[7], r2
; ASM-NEXT:	vmov.i8	q0, q3
; ASM-NEXT:	bx	lr

  ret <16 x i8> %res
}
