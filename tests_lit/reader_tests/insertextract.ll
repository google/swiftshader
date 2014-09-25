; Tests insertelement and extractelement vector instructions.

; RUN: %p2i -i %s --insts | FileCheck %s

; TODO(kschimpf): Change index arguments to valid constant indices once
; we can handle constants.

define void @ExtractV4xi1(<4 x i1> %v, i32 %i) {
entry:
  %e = extractelement <4 x i1> %v, i32 %i
  ret void
}

; CHECK:      define void @ExtractV4xi1(<4 x i1> %v, i32 %i) {
; CHECK-NEXT: entry:
; CHECK-NEXT:   %e = extractelement <4 x i1> %v, i32 %i
; CHECK-NEXT:   ret void
; CHECK-NEXT: }

define void @ExtractV8xi1(<8 x i1> %v, i32 %i) {
entry:
  %e = extractelement <8 x i1> %v, i32 %i
  ret void
}

; CHECK-NEXT: define void @ExtractV8xi1(<8 x i1> %v, i32 %i) {
; CHECK-NEXT: entry:
; CHECK-NEXT:   %e = extractelement <8 x i1> %v, i32 %i
; CHECK-NEXT:   ret void
; CHECK-NEXT: }

define void @ExtractV16xi1(<16 x i1> %v, i32 %i) {
entry:
  %e = extractelement <16 x i1> %v, i32 %i
  ret void
}

; CHECK-NEXT: define void @ExtractV16xi1(<16 x i1> %v, i32 %i) {
; CHECK-NEXT: entry:
; CHECK-NEXT:   %e = extractelement <16 x i1> %v, i32 %i
; CHECK-NEXT:   ret void
; CHECK-NEXT: }

define void @ExtractV16xi8(<16 x i8> %v, i32 %i) {
entry:
  %e = extractelement <16 x i8> %v, i32 %i
  ret void
}

; CHECK-NEXT: define void @ExtractV16xi8(<16 x i8> %v, i32 %i) {
; CHECK-NEXT: entry:
; CHECK-NEXT:   %e = extractelement <16 x i8> %v, i32 %i
; CHECK-NEXT:   ret void
; CHECK-NEXT: }

define void @ExtractV8xi16(<8 x i16> %v, i32 %i) {
entry:
  %e = extractelement <8 x i16> %v, i32 %i
  ret void
}

; CHECK-NEXT: define void @ExtractV8xi16(<8 x i16> %v, i32 %i) {
; CHECK-NEXT: entry:
; CHECK-NEXT:   %e = extractelement <8 x i16> %v, i32 %i
; CHECK-NEXT:   ret void
; CHECK-NEXT: }

define i32 @ExtractV4xi32(<4 x i32> %v, i32 %i) {
entry:
  %e = extractelement <4 x i32> %v, i32 %i
  ret i32 %e
}

; CHECK-NEXT: define i32 @ExtractV4xi32(<4 x i32> %v, i32 %i) {
; CHECK-NEXT: entry:
; CHECK-NEXT:   %e = extractelement <4 x i32> %v, i32 %i
; CHECK-NEXT:   ret i32 %e
; CHECK-NEXT: }

define float @ExtractV4xfloat(<4 x float> %v, i32 %i) {
entry:
  %e = extractelement <4 x float> %v, i32 %i
  ret float %e
}

; CHECK-NEXT: define float @ExtractV4xfloat(<4 x float> %v, i32 %i) {
; CHECK-NEXT: entry:
; CHECK-NEXT:   %e = extractelement <4 x float> %v, i32 %i
; CHECK-NEXT:   ret float %e
; CHECK-NEXT: }

define <4 x i1> @InsertV4xi1(<4 x i1> %v, i32 %pe, i32 %i) {
entry:
  %e = trunc i32 %pe to i1
  %r = insertelement <4 x i1> %v, i1 %e, i32 %i
  ret <4 x i1> %r
}

; CHECK-NEXT: define <4 x i1> @InsertV4xi1(<4 x i1> %v, i32 %pe, i32 %i) {
; CHECK-NEXT: entry:
; CHECK-NEXT:   %e = trunc i32 %pe to i1
; CHECK-NEXT:   %r = insertelement <4 x i1> %v, i1 %e, i32 %i
; CHECK-NEXT:   ret <4 x i1> %r
; CHECK-NEXT: }

define <8 x i1> @InsertV8xi1(<8 x i1> %v, i32 %pe, i32 %i) {
entry:
  %e = trunc i32 %pe to i1
  %r = insertelement <8 x i1> %v, i1 %e, i32 %i
  ret <8 x i1> %r
}

; CHECK-NEXT: define <8 x i1> @InsertV8xi1(<8 x i1> %v, i32 %pe, i32 %i) {
; CHECK-NEXT: entry:
; CHECK-NEXT:   %e = trunc i32 %pe to i1
; CHECK-NEXT:   %r = insertelement <8 x i1> %v, i1 %e, i32 %i
; CHECK-NEXT:   ret <8 x i1> %r
; CHECK-NEXT: }

define <16 x i1> @InsertV16xi1(<16 x i1> %v, i32 %pe, i32 %i) {
entry:
  %e = trunc i32 %pe to i1
  %r = insertelement <16 x i1> %v, i1 %e, i32 %i
  ret <16 x i1> %r
}

; CHECK-NEXT: define <16 x i1> @InsertV16xi1(<16 x i1> %v, i32 %pe, i32 %i) {
; CHECK-NEXT: entry:
; CHECK-NEXT:   %e = trunc i32 %pe to i1
; CHECK-NEXT:   %r = insertelement <16 x i1> %v, i1 %e, i32 %i
; CHECK-NEXT:   ret <16 x i1> %r
; CHECK-NEXT: }

define <16 x i8> @InsertV16xi8(<16 x i8> %v, i32 %pe, i32 %i) {
entry:
  %e = trunc i32 %pe to i8
  %r = insertelement <16 x i8> %v, i8 %e, i32 %i
  ret <16 x i8> %r
}

; CHECK-NEXT: define <16 x i8> @InsertV16xi8(<16 x i8> %v, i32 %pe, i32 %i) {
; CHECK-NEXT: entry:
; CHECK-NEXT:   %e = trunc i32 %pe to i8
; CHECK-NEXT:   %r = insertelement <16 x i8> %v, i8 %e, i32 %i
; CHECK-NEXT:   ret <16 x i8> %r
; CHECK-NEXT: }

define <8 x i16> @InsertV8xi16(<8 x i16> %v, i32 %pe, i32 %i) {
entry:
  %e = trunc i32 %pe to i16
  %r = insertelement <8 x i16> %v, i16 %e, i32 %i
  ret <8 x i16> %r
}

; CHECK-NEXT: define <8 x i16> @InsertV8xi16(<8 x i16> %v, i32 %pe, i32 %i) {
; CHECK-NEXT: entry:
; CHECK-NEXT:   %e = trunc i32 %pe to i16
; CHECK-NEXT:   %r = insertelement <8 x i16> %v, i16 %e, i32 %i
; CHECK-NEXT:   ret <8 x i16> %r
; CHECK-NEXT: }

define <4 x i32> @InsertV4xi32(<4 x i32> %v, i32 %e, i32 %i) {
entry:
  %r = insertelement <4 x i32> %v, i32 %e, i32 %i
  ret <4 x i32> %r
}

; CHECK-NEXT: define <4 x i32> @InsertV4xi32(<4 x i32> %v, i32 %e, i32 %i) {
; CHECK-NEXT: entry:
; CHECK-NEXT:   %r = insertelement <4 x i32> %v, i32 %e, i32 %i
; CHECK-NEXT:   ret <4 x i32> %r
; CHECK-NEXT: }

define <4 x float> @InsertV4xfloat(<4 x float> %v, float %e, i32 %i) {
entry:
  %r = insertelement <4 x float> %v, float %e, i32 %i
  ret <4 x float> %r
}

; CHECK-NEXT: define <4 x float> @InsertV4xfloat(<4 x float> %v, float %e, i32 %i) {
; CHECK-NEXT: entry:
; CHECK-NEXT:   %r = insertelement <4 x float> %v, float %e, i32 %i
; CHECK-NEXT:   ret <4 x float> %r
; CHECK-NEXT: }
