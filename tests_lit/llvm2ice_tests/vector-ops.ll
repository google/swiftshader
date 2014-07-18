; This checks support for insertelement and extractelement.

; RUN: %llvm2ice --verbose inst %s | FileCheck %s
; RUN: %llvm2ice --verbose none %s | FileCheck --check-prefix=ERRORS %s
; RUN: %llvm2iceinsts %s | %szdiff %s | FileCheck --check-prefix=DUMP %s
; RUN: %llvm2iceinsts --pnacl %s | %szdiff %s \
; RUN:                           | FileCheck --check-prefix=DUMP %s

; insertelement operations

define <4 x float> @insertelement_v4f32(<4 x float> %vec, float %elt) {
entry:
  %res = insertelement <4 x float> %vec, float %elt, i32 1
  ret <4 x float> %res
; CHECK-LABEL: insertelement_v4f32:
; CHECK: shufps
; CHECK: shufps
}

define <4 x i32> @insertelement_v4i32(<4 x i32> %vec, i32 %elt) {
entry:
  %res = insertelement <4 x i32> %vec, i32 %elt, i32 1
  ret <4 x i32> %res
; CHECK-LABEL: insertelement_v4i32:
; CHECK: shufps
; CHECK: shufps
}

define <8 x i16> @insertelement_v8i16(<8 x i16> %vec, i32 %elt.arg) {
entry:
  %elt = trunc i32 %elt.arg to i16
  %res = insertelement <8 x i16> %vec, i16 %elt, i32 1
  ret <8 x i16> %res
; CHECK-LABEL: insertelement_v8i16
; CHECK: pinsrw
}

define <16 x i8> @insertelement_v16i8(<16 x i8> %vec, i32 %elt.arg) {
entry:
  %elt = trunc i32 %elt.arg to i8
  %res = insertelement <16 x i8> %vec, i8 %elt, i32 1
  ret <16 x i8> %res
; CHECK-LABEL: insertelement_v16i8:
; CHECK: movups
; CHECK: lea
; CHECK: mov
}

define <4 x i1> @insertelement_v4i1(<4 x i1> %vec, i32 %elt.arg) {
entry:
  %elt = trunc i32 %elt.arg to i1
  %res = insertelement <4 x i1> %vec, i1 %elt, i32 1
  ret <4 x i1> %res
; CHECK-LABEL: insertelement_v4i1:
; CHECK: shufps
; CHECK: shufps
}

define <8 x i1> @insertelement_v8i1(<8 x i1> %vec, i32 %elt.arg) {
entry:
  %elt = trunc i32 %elt.arg to i1
  %res = insertelement <8 x i1> %vec, i1 %elt, i32 1
  ret <8 x i1> %res
; CHECK-LABEL: insertelement_v8i1:
; CHECK: pinsrw
}

define <16 x i1> @insertelement_v16i1(<16 x i1> %vec, i32 %elt.arg) {
entry:
  %elt = trunc i32 %elt.arg to i1
  %res = insertelement <16 x i1> %vec, i1 %elt, i32 1
  ret <16 x i1> %res
; CHECK-LABEL: insertelement_v16i1:
; CHECK: movups
; CHECK: lea
; CHECK: mov
}

; extractelement operations

define float @extractelement_v4f32(<4 x float> %vec) {
entry:
  %res = extractelement <4 x float> %vec, i32 1
  ret float %res
; CHECK-LABEL: extractelement_v4f32:
; CHECK: pshufd
}

define i32 @extractelement_v4i32(<4 x i32> %vec) {
entry:
  %res = extractelement <4 x i32> %vec, i32 1
  ret i32 %res
; CHECK-LABEL: extractelement_v4i32:
; CHECK: pshufd
}

define i32 @extractelement_v8i16(<8 x i16> %vec) {
entry:
  %res = extractelement <8 x i16> %vec, i32 1
  %res.ext = zext i16 %res to i32
  ret i32 %res.ext
; CHECK-LABEL: extractelement_v8i16:
; CHECK: pextrw
}

define i32 @extractelement_v16i8(<16 x i8> %vec) {
entry:
  %res = extractelement <16 x i8> %vec, i32 1
  %res.ext = zext i8 %res to i32
  ret i32 %res.ext
; CHECK-LABEL: extractelement_v16i8:
; CHECK: movups
; CHECK: lea
; CHECK: mov
}

define i32 @extractelement_v4i1(<4 x i1> %vec) {
entry:
  %res = extractelement <4 x i1> %vec, i32 1
  %res.ext = zext i1 %res to i32
  ret i32 %res.ext
; CHECK-LABEL: extractelement_v4i1:
; CHECK: pshufd
}

define i32 @extractelement_v8i1(<8 x i1> %vec) {
entry:
  %res = extractelement <8 x i1> %vec, i32 1
  %res.ext = zext i1 %res to i32
  ret i32 %res.ext
; CHECK-LABEL: extractelement_v8i1:
; CHECK: pextrw
}

define i32 @extractelement_v16i1(<16 x i1> %vec) {
entry:
  %res = extractelement <16 x i1> %vec, i32 1
  %res.ext = zext i1 %res to i32
  ret i32 %res.ext
; CHECK-LABEL: extractelement_v16i1:
; CHECK: movups
; CHECK: lea
; CHECK: mov
}

; ERRORS-NOT: ICE translation error
; DUMP-NOT: SZ
