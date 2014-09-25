; This checks support for insertelement and extractelement.

; RUN: %p2i -i %s --args -O2 --verbose none \
; RUN:   | llvm-mc -triple=i686-none-nacl -x86-asm-syntax=intel -filetype=obj \
; RUN:   | llvm-objdump -d --symbolize -x86-asm-syntax=intel - | FileCheck %s
; RUN: %p2i -i %s --args -Om1 --verbose none \
; RUN:   | llvm-mc -triple=i686-none-nacl -x86-asm-syntax=intel -filetype=obj \
; RUN:   | llvm-objdump -d --symbolize -x86-asm-syntax=intel - | FileCheck %s
; RUN: %p2i -i %s --args -O2 -mattr=sse4.1 --verbose none \
; RUN:   | llvm-mc -triple=i686-none-nacl -x86-asm-syntax=intel -filetype=obj \
; RUN:   | llvm-objdump -d --symbolize -x86-asm-syntax=intel - \
; RUN:   | FileCheck --check-prefix=SSE41 %s
; RUN: %p2i -i %s --args -Om1 -mattr=sse4.1 --verbose none \
; RUN:   | llvm-mc -triple=i686-none-nacl -x86-asm-syntax=intel -filetype=obj \
; RUN:   | llvm-objdump -d --symbolize -x86-asm-syntax=intel - \
; RUN:   | FileCheck --check-prefix=SSE41 %s
; RUN: %p2i -i %s --args --verbose none | FileCheck --check-prefix=ERRORS %s
; RUN: %p2i -i %s --insts | %szdiff %s | FileCheck --check-prefix=DUMP %s

; insertelement operations

define <4 x float> @insertelement_v4f32_0(<4 x float> %vec, float %elt) {
entry:
  %res = insertelement <4 x float> %vec, float %elt, i32 0
  ret <4 x float> %res
; CHECK-LABEL: insertelement_v4f32_0:
; CHECK: movss

; SSE41-LABEL: insertelement_v4f32_0:
; SSE41: insertps {{.*}}, {{.*}}, 0
}

define <4 x i32> @insertelement_v4i32_0(<4 x i32> %vec, i32 %elt) {
entry:
  %res = insertelement <4 x i32> %vec, i32 %elt, i32 0
  ret <4 x i32> %res
; CHECK-LABEL: insertelement_v4i32_0:
; CHECK: movd xmm{{.*}},
; CHECK: movss

; SSE41-LABEL: insertelement_v4i32_0:
; SSE41: pinsrd {{.*}}, {{.*}}, 0
}


define <4 x float> @insertelement_v4f32_1(<4 x float> %vec, float %elt) {
entry:
  %res = insertelement <4 x float> %vec, float %elt, i32 1
  ret <4 x float> %res
; CHECK-LABEL: insertelement_v4f32_1:
; CHECK: shufps
; CHECK: shufps

; SSE41-LABEL: insertelement_v4f32_1:
; SSE41: insertps {{.*}}, {{.*}}, 16
}

define <4 x i32> @insertelement_v4i32_1(<4 x i32> %vec, i32 %elt) {
entry:
  %res = insertelement <4 x i32> %vec, i32 %elt, i32 1
  ret <4 x i32> %res
; CHECK-LABEL: insertelement_v4i32_1:
; CHECK: shufps
; CHECK: shufps

; SSE41-LABEL: insertelement_v4i32_1:
; SSE41: pinsrd {{.*}}, {{.*}}, 1
}

define <8 x i16> @insertelement_v8i16(<8 x i16> %vec, i32 %elt.arg) {
entry:
  %elt = trunc i32 %elt.arg to i16
  %res = insertelement <8 x i16> %vec, i16 %elt, i32 1
  ret <8 x i16> %res
; CHECK-LABEL: insertelement_v8i16:
; CHECK: pinsrw

; SSE41-LABEL: insertelement_v8i16:
; SSE41: pinsrw
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

; SSE41-LABEL: insertelement_v16i8:
; SSE41: pinsrb
}

define <4 x i1> @insertelement_v4i1_0(<4 x i1> %vec, i32 %elt.arg) {
entry:
  %elt = trunc i32 %elt.arg to i1
  %res = insertelement <4 x i1> %vec, i1 %elt, i32 0
  ret <4 x i1> %res
; CHECK-LABEL: insertelement_v4i1_0:
; CHECK: movss

; SSE41-LABEL: insertelement_v4i1_0:
; SSE41: pinsrd {{.*}}, {{.*}}, 0
}

define <4 x i1> @insertelement_v4i1_1(<4 x i1> %vec, i32 %elt.arg) {
entry:
  %elt = trunc i32 %elt.arg to i1
  %res = insertelement <4 x i1> %vec, i1 %elt, i32 1
  ret <4 x i1> %res
; CHECK-LABEL: insertelement_v4i1_1:
; CHECK: shufps
; CHECK: shufps

; SSE41-LABEL: insertelement_v4i1_1:
; SSE41: pinsrd {{.*}}, {{.*}}, 1
}

define <8 x i1> @insertelement_v8i1(<8 x i1> %vec, i32 %elt.arg) {
entry:
  %elt = trunc i32 %elt.arg to i1
  %res = insertelement <8 x i1> %vec, i1 %elt, i32 1
  ret <8 x i1> %res
; CHECK-LABEL: insertelement_v8i1:
; CHECK: pinsrw

; SSE41-LABEL: insertelement_v8i1:
; SSE41: pinsrw
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

; SSE41-LABEL: insertelement_v16i1:
; SSE41: pinsrb
}

; extractelement operations

define float @extractelement_v4f32(<4 x float> %vec) {
entry:
  %res = extractelement <4 x float> %vec, i32 1
  ret float %res
; CHECK-LABEL: extractelement_v4f32:
; CHECK: pshufd

; SSE41-LABEL: extractelement_v4f32:
; SSE41: pshufd
}

define i32 @extractelement_v4i32(<4 x i32> %vec) {
entry:
  %res = extractelement <4 x i32> %vec, i32 1
  ret i32 %res
; CHECK-LABEL: extractelement_v4i32:
; CHECK: pshufd
; CHECK: movd {{.*}}, xmm

; SSE41-LABEL: extractelement_v4i32:
; SSE41: pextrd
}

define i32 @extractelement_v8i16(<8 x i16> %vec) {
entry:
  %res = extractelement <8 x i16> %vec, i32 1
  %res.ext = zext i16 %res to i32
  ret i32 %res.ext
; CHECK-LABEL: extractelement_v8i16:
; CHECK: pextrw

; SSE41-LABEL: extractelement_v8i16:
; SSE41: pextrw
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

; SSE41-LABEL: extractelement_v16i8:
; SSE41: pextrb
}

define i32 @extractelement_v4i1(<4 x i1> %vec) {
entry:
  %res = extractelement <4 x i1> %vec, i32 1
  %res.ext = zext i1 %res to i32
  ret i32 %res.ext
; CHECK-LABEL: extractelement_v4i1:
; CHECK: pshufd

; SSE41-LABEL: extractelement_v4i1:
; SSE41: pextrd
}

define i32 @extractelement_v8i1(<8 x i1> %vec) {
entry:
  %res = extractelement <8 x i1> %vec, i32 1
  %res.ext = zext i1 %res to i32
  ret i32 %res.ext
; CHECK-LABEL: extractelement_v8i1:
; CHECK: pextrw

; SSE41-LABEL: extractelement_v8i1:
; SSE41: pextrw
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

; SSE41-LABEL: extractelement_v16i1:
; SSE41: pextrb
}

; ERRORS-NOT: ICE translation error
; DUMP-NOT: SZ
