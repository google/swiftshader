; This test checks that when SSE instructions access memory and require full
; alignment, memory operands are limited to properly aligned stack operands.
; This would only happen when we fuse a load instruction with another
; instruction, which currently only happens with non-scalarized Arithmetic
; instructions.

; RUN: %p2i -i %s --filetype=obj --disassemble --args -O2  | FileCheck %s
; RUN: %p2i -i %s --filetype=obj --disassemble --args -Om1 | FileCheck %s

define <4 x i32> @test_add(i32 %addr_i, <4 x i32> %addend) {
entry:
  %addr = inttoptr i32 %addr_i to <4 x i32>*
  %loaded = load <4 x i32>, <4 x i32>* %addr, align 4
  %result = add <4 x i32> %addend, %loaded
  ret <4 x i32> %result
}
; CHECK-LABEL: test_add
; CHECK-NOT: paddd xmm{{.}},XMMWORD PTR [e{{ax|cx|dx|di|si|bx|bp}}
; CHECK: paddd xmm{{.}},

define <4 x i32> @test_and(i32 %addr_i, <4 x i32> %addend) {
entry:
  %addr = inttoptr i32 %addr_i to <4 x i32>*
  %loaded = load <4 x i32>, <4 x i32>* %addr, align 4
  %result = and <4 x i32> %addend, %loaded
  ret <4 x i32> %result
}
; CHECK-LABEL: test_and
; CHECK-NOT: pand xmm{{.}},XMMWORD PTR [e{{ax|cx|dx|di|si|bx|bp}}
; CHECK: pand xmm{{.}},

define <4 x i32> @test_or(i32 %addr_i, <4 x i32> %addend) {
entry:
  %addr = inttoptr i32 %addr_i to <4 x i32>*
  %loaded = load <4 x i32>, <4 x i32>* %addr, align 4
  %result = or <4 x i32> %addend, %loaded
  ret <4 x i32> %result
}
; CHECK-LABEL: test_or
; CHECK-NOT: por xmm{{.}},XMMWORD PTR [e{{ax|cx|dx|di|si|bx|bp}}
; CHECK: por xmm{{.}},

define <4 x i32> @test_xor(i32 %addr_i, <4 x i32> %addend) {
entry:
  %addr = inttoptr i32 %addr_i to <4 x i32>*
  %loaded = load <4 x i32>, <4 x i32>* %addr, align 4
  %result = xor <4 x i32> %addend, %loaded
  ret <4 x i32> %result
}
; CHECK-LABEL: test_xor
; CHECK-NOT: pxor xmm{{.}},XMMWORD PTR [e{{ax|cx|dx|di|si|bx|bp}}
; CHECK: pxor xmm{{.}},

define <4 x i32> @test_sub(i32 %addr_i, <4 x i32> %addend) {
entry:
  %addr = inttoptr i32 %addr_i to <4 x i32>*
  %loaded = load <4 x i32>, <4 x i32>* %addr, align 4
  %result = sub <4 x i32> %addend, %loaded
  ret <4 x i32> %result
}
; CHECK-LABEL: test_sub
; CHECK-NOT: psubd xmm{{.}},XMMWORD PTR [e{{ax|cx|dx|di|si|bx|bp}}
; CHECK: psubd xmm{{.}},

define <4 x float> @test_fadd(i32 %addr_i, <4 x float> %addend) {
entry:
  %addr = inttoptr i32 %addr_i to <4 x float>*
  %loaded = load <4 x float>, <4 x float>* %addr, align 4
  %result = fadd <4 x float> %addend, %loaded
  ret <4 x float> %result
}
; CHECK-LABEL: test_fadd
; CHECK-NOT: addps xmm{{.}},XMMWORD PTR [e{{ax|cx|dx|di|si|bx|bp}}
; CHECK: addps xmm{{.}},

define <4 x float> @test_fsub(i32 %addr_i, <4 x float> %addend) {
entry:
  %addr = inttoptr i32 %addr_i to <4 x float>*
  %loaded = load <4 x float>, <4 x float>* %addr, align 4
  %result = fsub <4 x float> %addend, %loaded
  ret <4 x float> %result
}
; CHECK-LABEL: test_fsub
; CHECK-NOT: subps xmm{{.}},XMMWORD PTR [e{{ax|cx|dx|di|si|bx|bp}}
; CHECK: subps xmm{{.}},
