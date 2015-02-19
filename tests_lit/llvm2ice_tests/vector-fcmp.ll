; This file checks support for comparing vector values with the fcmp
; instruction.

; RUN: %p2i -i %s --assemble --disassemble -a -O2 --verbose none \
; RUN:   | FileCheck %s
; RUN: %p2i -i %s --assemble --disassemble -a -Om1 --verbose none \
; RUN:   | FileCheck %s

; Check that sext elimination occurs when the result of the comparison
; instruction is alrady sign extended.  Sign extension to 4 x i32 uses
; the pslld instruction.
define <4 x i32> @sextElimination(<4 x float> %a, <4 x float> %b) {
entry:
  %res.trunc = fcmp oeq <4 x float> %a, %b
  %res = sext <4 x i1> %res.trunc to <4 x i32>
  ret <4 x i32> %res
; CHECK-LABEL: sextElimination
; CHECK: cmpeqps
; CHECK-NOT: pslld
}

define <4 x i32> @fcmpFalseVector(<4 x float> %a, <4 x float> %b) {
entry:
  %res.trunc = fcmp false <4 x float> %a, %b
  %res = sext <4 x i1> %res.trunc to <4 x i32>
  ret <4 x i32> %res
; CHECK-LABEL: fcmpFalseVector
; CHECK: pxor
}

define <4 x i32> @fcmpOeqVector(<4 x float> %a, <4 x float> %b) {
entry:
  %res.trunc = fcmp oeq <4 x float> %a, %b
  %res = sext <4 x i1> %res.trunc to <4 x i32>
  ret <4 x i32> %res
; CHECK-LABEL: fcmpOeqVector
; CHECK: cmpeqps
}

define <4 x i32> @fcmpOgeVector(<4 x float> %a, <4 x float> %b) {
entry:
  %res.trunc = fcmp oge <4 x float> %a, %b
  %res = sext <4 x i1> %res.trunc to <4 x i32>
  ret <4 x i32> %res
; CHECK-LABEL: fcmpOgeVector
; CHECK: cmpleps
}

define <4 x i32> @fcmpOgtVector(<4 x float> %a, <4 x float> %b) {
entry:
  %res.trunc = fcmp ogt <4 x float> %a, %b
  %res = sext <4 x i1> %res.trunc to <4 x i32>
  ret <4 x i32> %res
; CHECK-LABEL: fcmpOgtVector
; CHECK: cmpltps
}

define <4 x i32> @fcmpOleVector(<4 x float> %a, <4 x float> %b) {
entry:
  %res.trunc = fcmp ole <4 x float> %a, %b
  %res = sext <4 x i1> %res.trunc to <4 x i32>
  ret <4 x i32> %res
; CHECK-LABEL: fcmpOleVector
; CHECK: cmpleps
}

define <4 x i32> @fcmpOltVector(<4 x float> %a, <4 x float> %b) {
entry:
  %res.trunc = fcmp olt <4 x float> %a, %b
  %res = sext <4 x i1> %res.trunc to <4 x i32>
  ret <4 x i32> %res
; CHECK-LABEL: fcmpOltVector
; CHECK: cmpltps
}

define <4 x i32> @fcmpOneVector(<4 x float> %a, <4 x float> %b) {
entry:
  %res.trunc = fcmp one <4 x float> %a, %b
  %res = sext <4 x i1> %res.trunc to <4 x i32>
  ret <4 x i32> %res
; CHECK-LABEL: fcmpOneVector
; CHECK: cmpneqps
; CHECK: cmpordps
; CHECK: pand
}

define <4 x i32> @fcmpOrdVector(<4 x float> %a, <4 x float> %b) {
entry:
  %res.trunc = fcmp ord <4 x float> %a, %b
  %res = sext <4 x i1> %res.trunc to <4 x i32>
  ret <4 x i32> %res
; CHECK-LABEL: fcmpOrdVector
; CHECK: cmpordps
}

define <4 x i32> @fcmpTrueVector(<4 x float> %a, <4 x float> %b) {
entry:
  %res.trunc = fcmp true <4 x float> %a, %b
  %res = sext <4 x i1> %res.trunc to <4 x i32>
  ret <4 x i32> %res
; CHECK-LABEL: fcmpTrueVector
; CHECK: pcmpeqd
}

define <4 x i32> @fcmpUeqVector(<4 x float> %a, <4 x float> %b) {
entry:
  %res.trunc = fcmp ueq <4 x float> %a, %b
  %res = sext <4 x i1> %res.trunc to <4 x i32>
  ret <4 x i32> %res
; CHECK-LABEL: fcmpUeqVector
; CHECK: cmpeqps
; CHECK: cmpunordps
; CHECK: por
}

define <4 x i32> @fcmpUgeVector(<4 x float> %a, <4 x float> %b) {
entry:
  %res.trunc = fcmp uge <4 x float> %a, %b
  %res = sext <4 x i1> %res.trunc to <4 x i32>
  ret <4 x i32> %res
; CHECK-LABEL: fcmpUgeVector
; CHECK: cmpnltps
}

define <4 x i32> @fcmpUgtVector(<4 x float> %a, <4 x float> %b) {
entry:
  %res.trunc = fcmp ugt <4 x float> %a, %b
  %res = sext <4 x i1> %res.trunc to <4 x i32>
  ret <4 x i32> %res
; CHECK-LABEL: fcmpUgtVector
; CHECK: cmpnleps
}

define <4 x i32> @fcmpUleVector(<4 x float> %a, <4 x float> %b) {
entry:
  %res.trunc = fcmp ule <4 x float> %a, %b
  %res = sext <4 x i1> %res.trunc to <4 x i32>
  ret <4 x i32> %res
; CHECK-LABEL: fcmpUleVector
; CHECK: cmpnltps
}

define <4 x i32> @fcmpUltVector(<4 x float> %a, <4 x float> %b) {
entry:
  %res.trunc = fcmp ult <4 x float> %a, %b
  %res = sext <4 x i1> %res.trunc to <4 x i32>
  ret <4 x i32> %res
; CHECK-LABEL: fcmpUltVector
; CHECK: cmpnleps
}

define <4 x i32> @fcmpUneVector(<4 x float> %a, <4 x float> %b) {
entry:
  %res.trunc = fcmp une <4 x float> %a, %b
  %res = sext <4 x i1> %res.trunc to <4 x i32>
  ret <4 x i32> %res
; CHECK-LABEL: fcmpUneVector
; CHECK: cmpneqps
}

define <4 x i32> @fcmpUnoVector(<4 x float> %a, <4 x float> %b) {
entry:
  %res.trunc = fcmp uno <4 x float> %a, %b
  %res = sext <4 x i1> %res.trunc to <4 x i32>
  ret <4 x i32> %res
; CHECK-LABEL: fcmpUnoVector
; CHECK: cmpunordps
}
