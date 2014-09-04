; Tests if we can read select instructions.

; RUN: llvm-as < %s | pnacl-freeze \
; RUN:              | %llvm2ice -notranslate -verbose=inst -build-on-read \
; RUN:                -allow-pnacl-reader-error-recovery \
; RUN:              | FileCheck %s

define void @Seli1(i32 %p) {
  %vc = trunc i32 %p to i1
  %vt = trunc i32 %p to i1
  %ve = trunc i32 %p to i1
  %r = select i1 %vc, i1 %vt, i1 %ve
  ret void
}

; CHECK:      define void @Seli1(i32 %__0) {
; CHECK-NEXT: __0:
; CHECK-NEXT:   %__1 = trunc i32 %__0 to i1
; CHECK-NEXT:   %__2 = trunc i32 %__0 to i1
; CHECK-NEXT:   %__3 = trunc i32 %__0 to i1
; CHECK-NEXT:   %__4 = select i1 %__1, i1 %__2, i1 %__3
; CHECK-NEXT:   ret void
; CHECK-NEXT: }

define void @Seli8(i32 %p) {
  %vc = trunc i32 %p to i1
  %vt = trunc i32 %p to i8
  %ve = trunc i32 %p to i8
  %r = select i1 %vc, i8 %vt, i8 %ve
  ret void
}

; CHECK-NEXT: define void @Seli8(i32 %__0) {
; CHECK-NEXT: __0:
; CHECK-NEXT:   %__1 = trunc i32 %__0 to i1
; CHECK-NEXT:   %__2 = trunc i32 %__0 to i8
; CHECK-NEXT:   %__3 = trunc i32 %__0 to i8
; CHECK-NEXT:   %__4 = select i1 %__1, i8 %__2, i8 %__3
; CHECK-NEXT:   ret void
; CHECK-NEXT: }

define void @Seli16(i32 %p) {
  %vc = trunc i32 %p to i1
  %vt = trunc i32 %p to i16
  %ve = trunc i32 %p to i16
  %r = select i1 %vc, i16 %vt, i16 %ve
  ret void
}

; CHECK-NEXT: define void @Seli16(i32 %__0) {
; CHECK-NEXT: __0:
; CHECK-NEXT:   %__1 = trunc i32 %__0 to i1
; CHECK-NEXT:   %__2 = trunc i32 %__0 to i16
; CHECK-NEXT:   %__3 = trunc i32 %__0 to i16
; CHECK-NEXT:   %__4 = select i1 %__1, i16 %__2, i16 %__3
; CHECK-NEXT:   ret void
; CHECK-NEXT: }

define i32 @Seli32(i32 %pc, i32 %pt, i32 %pe) {
  %vc = trunc i32 %pc to i1
  %r = select i1 %vc, i32 %pt, i32 %pe
  ret i32 %r
}

; CHECK-NEXT: define i32 @Seli32(i32 %__0, i32 %__1, i32 %__2) {
; CHECK-NEXT: __0:
; CHECK-NEXT:   %__3 = trunc i32 %__0 to i1
; CHECK-NEXT:   %__4 = select i1 %__3, i32 %__1, i32 %__2
; CHECK-NEXT:   ret i32 %__4
; CHECK-NEXT: }

define i64 @Seli64(i64 %pc, i64 %pt, i64 %pe) {
  %vc = trunc i64 %pc to i1
  %r = select i1 %vc, i64 %pt, i64 %pe
  ret i64 %r
}

; CHECK-NEXT: define i64 @Seli64(i64 %__0, i64 %__1, i64 %__2) {
; CHECK-NEXT: __0:
; CHECK-NEXT:   %__3 = trunc i64 %__0 to i1
; CHECK-NEXT:   %__4 = select i1 %__3, i64 %__1, i64 %__2
; CHECK-NEXT:   ret i64 %__4
; CHECK-NEXT: }

define float @SelFloat(i32 %pc, float %pt, float %pe) {
  %vc = trunc i32 %pc to i1
  %r = select i1 %vc, float %pt, float %pe
  ret float %r
}

; CHECK-NEXT: define float @SelFloat(i32 %__0, float %__1, float %__2) {
; CHECK-NEXT: __0:
; CHECK-NEXT:   %__3 = trunc i32 %__0 to i1
; CHECK-NEXT:   %__4 = select i1 %__3, float %__1, float %__2
; CHECK-NEXT:   ret float %__4
; CHECK-NEXT: }

define double @SelDouble(i32 %pc, double %pt, double %pe) {
  %vc = trunc i32 %pc to i1
  %r = select i1 %vc, double %pt, double %pe
  ret double %r
}

; CHECK-NEXT: define double @SelDouble(i32 %__0, double %__1, double %__2) {
; CHECK-NEXT: __0:
; CHECK-NEXT:   %__3 = trunc i32 %__0 to i1
; CHECK-NEXT:   %__4 = select i1 %__3, double %__1, double %__2
; CHECK-NEXT:   ret double %__4
; CHECK-NEXT: }

define <16 x i1> @SelV16x1(i32 %pc, <16 x i1> %pt, <16 x i1> %pe) {
  %vc = trunc i32 %pc to i1
  %r = select i1 %vc, <16 x i1> %pt, <16 x i1> %pe
  ret <16 x i1> %r
}

; CHECK-NEXT: define <16 x i1> @SelV16x1(i32 %__0, <16 x i1> %__1, <16 x i1> %__2) {
; CHECK-NEXT: __0:
; CHECK-NEXT:   %__3 = trunc i32 %__0 to i1
; CHECK-NEXT:   %__4 = select i1 %__3, <16 x i1> %__1, <16 x i1> %__2
; CHECK-NEXT:   ret <16 x i1> %__4
; CHECK-NEXT: }

define <8 x i1> @SelV8x1(i32 %pc, <8 x i1> %pt, <8 x i1> %pe) {
  %vc = trunc i32 %pc to i1
  %r = select i1 %vc, <8 x i1> %pt, <8 x i1> %pe
  ret <8 x i1> %r
}

; CHECK-NEXT: define <8 x i1> @SelV8x1(i32 %__0, <8 x i1> %__1, <8 x i1> %__2) {
; CHECK-NEXT: __0:
; CHECK-NEXT:   %__3 = trunc i32 %__0 to i1
; CHECK-NEXT:   %__4 = select i1 %__3, <8 x i1> %__1, <8 x i1> %__2
; CHECK-NEXT:   ret <8 x i1> %__4
; CHECK-NEXT: }

define <4 x i1> @SelV4x1(i32 %pc, <4 x i1> %pt, <4 x i1> %pe) {
  %vc = trunc i32 %pc to i1
  %r = select i1 %vc, <4 x i1> %pt, <4 x i1> %pe
  ret <4 x i1> %r
}

; CHECK-NEXT: define <4 x i1> @SelV4x1(i32 %__0, <4 x i1> %__1, <4 x i1> %__2) {
; CHECK-NEXT: __0:
; CHECK-NEXT:   %__3 = trunc i32 %__0 to i1
; CHECK-NEXT:   %__4 = select i1 %__3, <4 x i1> %__1, <4 x i1> %__2
; CHECK-NEXT:   ret <4 x i1> %__4
; CHECK-NEXT: }

define <16 x i8> @SelV16x8(i32 %pc, <16 x i8> %pt, <16 x i8> %pe) {
  %vc = trunc i32 %pc to i1
  %r = select i1 %vc, <16 x i8> %pt, <16 x i8> %pe
  ret <16 x i8> %r
}

; CHECK-NEXT: define <16 x i8> @SelV16x8(i32 %__0, <16 x i8> %__1, <16 x i8> %__2) {
; CHECK-NEXT: __0:
; CHECK-NEXT:   %__3 = trunc i32 %__0 to i1
; CHECK-NEXT:   %__4 = select i1 %__3, <16 x i8> %__1, <16 x i8> %__2
; CHECK-NEXT:   ret <16 x i8> %__4
; CHECK-NEXT: }

define <8 x i16> @SelV8x16(i32 %pc, <8 x i16> %pt, <8 x i16> %pe) {
  %vc = trunc i32 %pc to i1
  %r = select i1 %vc, <8 x i16> %pt, <8 x i16> %pe
  ret <8 x i16> %r
}

; CHECK-NEXT: define <8 x i16> @SelV8x16(i32 %__0, <8 x i16> %__1, <8 x i16> %__2) {
; CHECK-NEXT: __0:
; CHECK-NEXT:   %__3 = trunc i32 %__0 to i1
; CHECK-NEXT:   %__4 = select i1 %__3, <8 x i16> %__1, <8 x i16> %__2
; CHECK-NEXT:   ret <8 x i16> %__4
; CHECK-NEXT: }

define <4 x i32> @SelV4x32(i32 %pc, <4 x i32> %pt, <4 x i32> %pe) {
  %vc = trunc i32 %pc to i1
  %r = select i1 %vc, <4 x i32> %pt, <4 x i32> %pe
  ret <4 x i32> %r
}

; CHECK-NEXT: define <4 x i32> @SelV4x32(i32 %__0, <4 x i32> %__1, <4 x i32> %__2) {
; CHECK-NEXT: __0:
; CHECK-NEXT:   %__3 = trunc i32 %__0 to i1
; CHECK-NEXT:   %__4 = select i1 %__3, <4 x i32> %__1, <4 x i32> %__2
; CHECK-NEXT:   ret <4 x i32> %__4
; CHECK-NEXT: }

define <4 x float> @SelV4xfloat(i32 %pc, <4 x float> %pt, <4 x float> %pe) {
  %vc = trunc i32 %pc to i1
  %r = select i1 %vc, <4 x float> %pt, <4 x float> %pe
  ret <4 x float> %r
}

; CHECK-NEXT: define <4 x float> @SelV4xfloat(i32 %__0, <4 x float> %__1, <4 x float> %__2) {
; CHECK-NEXT: __0:
; CHECK-NEXT:   %__3 = trunc i32 %__0 to i1
; CHECK-NEXT:   %__4 = select i1 %__3, <4 x float> %__1, <4 x float> %__2
; CHECK-NEXT:   ret <4 x float> %__4
; CHECK-NEXT: }

define <16 x i1> @SelV16x1Vcond(<16 x i1> %pc, <16 x i1> %pt, <16 x i1> %pe) {
  %r = select <16 x i1> %pc, <16 x i1> %pt, <16 x i1> %pe
  ret <16 x i1> %r
}

; CHECK-NEXT: define <16 x i1> @SelV16x1Vcond(<16 x i1> %__0, <16 x i1> %__1, <16 x i1> %__2) {
; CHECK-NEXT: __0:
; CHECK-NEXT:   %__3 = select <16 x i1> %__0, <16 x i1> %__1, <16 x i1> %__2
; CHECK-NEXT:   ret <16 x i1> %__3
; CHECK-NEXT: }

define <8 x i1> @SelV8x1Vcond(<8 x i1> %pc, <8 x i1> %pt, <8 x i1> %pe) {
  %r = select <8 x i1> %pc, <8 x i1> %pt, <8 x i1> %pe
  ret <8 x i1> %r
}

; CHECK-NEXT: define <8 x i1> @SelV8x1Vcond(<8 x i1> %__0, <8 x i1> %__1, <8 x i1> %__2) {
; CHECK-NEXT: __0:
; CHECK-NEXT:   %__3 = select <8 x i1> %__0, <8 x i1> %__1, <8 x i1> %__2
; CHECK-NEXT:   ret <8 x i1> %__3
; CHECK-NEXT: }

define <4 x i1> @SelV4x1Vcond(<4 x i1> %pc, <4 x i1> %pt, <4 x i1> %pe) {
  %r = select <4 x i1> %pc, <4 x i1> %pt, <4 x i1> %pe
  ret <4 x i1> %r
}

; CHECK-NEXT: define <4 x i1> @SelV4x1Vcond(<4 x i1> %__0, <4 x i1> %__1, <4 x i1> %__2) {
; CHECK-NEXT: __0:
; CHECK-NEXT:   %__3 = select <4 x i1> %__0, <4 x i1> %__1, <4 x i1> %__2
; CHECK-NEXT:   ret <4 x i1> %__3
; CHECK-NEXT: }

define <16 x i8> @SelV16x8Vcond(<16 x i1> %pc, <16 x i8> %pt, <16 x i8> %pe) {
  %r = select <16 x i1> %pc, <16 x i8> %pt, <16 x i8> %pe
  ret <16 x i8> %r
}

; CHECK-NEXT: define <16 x i8> @SelV16x8Vcond(<16 x i1> %__0, <16 x i8> %__1, <16 x i8> %__2) {
; CHECK-NEXT: __0:
; CHECK-NEXT:   %__3 = select <16 x i1> %__0, <16 x i8> %__1, <16 x i8> %__2
; CHECK-NEXT:   ret <16 x i8> %__3
; CHECK-NEXT: }

define <8 x i16> @SelV8x16Vcond(<8 x i1> %pc, <8 x i16> %pt, <8 x i16> %pe) {
  %r = select <8 x i1> %pc, <8 x i16> %pt, <8 x i16> %pe
  ret <8 x i16> %r
}

; CHECK-NEXT: define <8 x i16> @SelV8x16Vcond(<8 x i1> %__0, <8 x i16> %__1, <8 x i16> %__2) {
; CHECK-NEXT: __0:
; CHECK-NEXT:   %__3 = select <8 x i1> %__0, <8 x i16> %__1, <8 x i16> %__2
; CHECK-NEXT:   ret <8 x i16> %__3
; CHECK-NEXT: }

define <4 x i32> @SelV4x32Vcond(<4 x i1> %pc, <4 x i32> %pt, <4 x i32> %pe) {
  %r = select <4 x i1> %pc, <4 x i32> %pt, <4 x i32> %pe
  ret <4 x i32> %r
}

; CHECK-NEXT: define <4 x i32> @SelV4x32Vcond(<4 x i1> %__0, <4 x i32> %__1, <4 x i32> %__2) {
; CHECK-NEXT: __0:
; CHECK-NEXT:   %__3 = select <4 x i1> %__0, <4 x i32> %__1, <4 x i32> %__2
; CHECK-NEXT:   ret <4 x i32> %__3
; CHECK-NEXT: }

define <4 x float> @SelV4xfloatVcond(<4 x i1> %pc, <4 x float> %pt, <4 x float> %pe) {
  %r = select <4 x i1> %pc, <4 x float> %pt, <4 x float> %pe
  ret <4 x float> %r
}

; CHECK-NEXT: define <4 x float> @SelV4xfloatVcond(<4 x i1> %__0, <4 x float> %__1, <4 x float> %__2) {
; CHECK-NEXT: __0:
; CHECK-NEXT:   %__3 = select <4 x i1> %__0, <4 x float> %__1, <4 x float> %__2
; CHECK-NEXT:   ret <4 x float> %__3
; CHECK-NEXT: }
