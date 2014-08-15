; This is a smoke test of nop insertion.

; RUN: %llvm2ice -rng-seed=1 -nop-insertion -nop-insertion-percentage=50 \
; RUN:    -max-nops-per-instruction=1 %s | FileCheck %s --check-prefix=PROB50
; RUN: %llvm2ice -rng-seed=1 -nop-insertion -nop-insertion-percentage=90 \
; RUN:    -max-nops-per-instruction=1 %s | FileCheck %s --check-prefix=PROB90
; RUN: %llvm2ice -rng-seed=1 -nop-insertion -nop-insertion-percentage=50 \
; RUN:    -max-nops-per-instruction=2 %s | FileCheck %s --check-prefix=MAXNOPS2

define <4 x i32> @mul_v4i32(<4 x i32> %a, <4 x i32> %b) {
entry:
  %res = mul <4 x i32> %a, %b
  ret <4 x i32> %res
; PROB50-LABEL: mul_v4i32:
; PROB50: nop # variant = 3
; PROB50: sub esp, 60
; PROB50: nop # variant = 4
; PROB50: movups xmmword ptr [esp+32], xmm0
; PROB50: movups xmmword ptr [esp+16], xmm1
; PROB50: nop # variant = 0
; PROB50: movups xmm0, xmmword ptr [esp+32]
; PROB50: nop # variant = 4
; PROB50: pshufd xmm1, xmmword ptr [esp+32], 49
; PROB50: pshufd xmm2, xmmword ptr [esp+16], 49
; PROB50: pmuludq xmm0, xmmword ptr [esp+16]
; PROB50: pmuludq xmm1, xmm2
; PROB50: nop # variant = 0
; PROB50: shufps xmm0, xmm1, 136
; PROB50: pshufd xmm3, xmm0, 216
; PROB50: nop # variant = 2
; PROB50: movups xmmword ptr [esp], xmm3
; PROB50: movups xmm0, xmmword ptr [esp]
; PROB50: add esp, 60
; PROB50: nop # variant = 0
; PROB50: ret

; PROB90-LABEL: mul_v4i32:
; PROB90: nop # variant = 3
; PROB90: sub esp, 60
; PROB90: nop # variant = 4
; PROB90: movups xmmword ptr [esp+32], xmm0
; PROB90: nop # variant = 3
; PROB90: movups xmmword ptr [esp+16], xmm1
; PROB90: nop # variant = 2
; PROB90: movups xmm0, xmmword ptr [esp+32]
; PROB90: nop # variant = 3
; PROB90: pshufd xmm1, xmmword ptr [esp+32], 49
; PROB90: nop # variant = 4
; PROB90: pshufd xmm2, xmmword ptr [esp+16], 49
; PROB90: nop # variant = 0
; PROB90: pmuludq xmm0, xmmword ptr [esp+16]
; PROB90: nop # variant = 2
; PROB90: pmuludq xmm1, xmm2
; PROB90: nop # variant = 3
; PROB90: shufps xmm0, xmm1, 136
; PROB90: nop # variant = 4
; PROB90: pshufd xmm3, xmm0, 216
; PROB90: nop # variant = 2
; PROB90: movups xmmword ptr [esp], xmm3
; PROB90: nop # variant = 4
; PROB90: movups xmm0, xmmword ptr [esp]
; PROB90: nop # variant = 2
; PROB90: add esp, 60
; PROB90: nop # variant = 3
; PROB90: ret

; MAXNOPS2-LABEL: mul_v4i32:
; MAXNOPS2: sub esp, 60
; MAXNOPS2: nop # variant = 4
; MAXNOPS2: movups xmmword ptr [esp+32], xmm0
; MAXNOPS2: nop # variant = 0
; MAXNOPS2: nop # variant = 4
; MAXNOPS2: movups xmmword ptr [esp+16], xmm1
; MAXNOPS2: movups xmm0, xmmword ptr [esp+32]
; MAXNOPS2: nop # variant = 0
; MAXNOPS2: pshufd xmm1, xmmword ptr [esp+32], 49
; MAXNOPS2: nop # variant = 2
; MAXNOPS2: pshufd xmm2, xmmword ptr [esp+16], 49
; MAXNOPS2: pmuludq xmm0, xmmword ptr [esp+16]
; MAXNOPS2: nop # variant = 0
; MAXNOPS2: nop # variant = 3
; MAXNOPS2: pmuludq xmm1, xmm2
; MAXNOPS2: shufps xmm0, xmm1, 136
; MAXNOPS2: pshufd xmm3, xmm0, 216
; MAXNOPS2: nop # variant = 3
; MAXNOPS2: movups xmmword ptr [esp], xmm3
; MAXNOPS2: nop # variant = 0
; MAXNOPS2: movups xmm0, xmmword ptr [esp]
; MAXNOPS2: nop # variant = 2
; MAXNOPS2: add esp, 60
; MAXNOPS2: nop # variant = 4
; MAXNOPS2: ret
}
