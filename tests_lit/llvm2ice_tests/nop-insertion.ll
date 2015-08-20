; This is a smoke test of nop insertion.

; REQUIRES: allow_dump

; Use filetype=asm because this currently depends on the # variant
; assembler comment.

; RUN: %p2i -i %s --filetype=asm -a -sz-seed=1 -nop-insertion \
; RUN:    -nop-insertion-percentage=50 -max-nops-per-instruction=1 \
; RUN:    | FileCheck %s --check-prefix=PROB50
; RUN: %p2i -i %s --filetype=asm -a -sz-seed=1 -nop-insertion \
; RUN:    -nop-insertion-percentage=90 -max-nops-per-instruction=1 \
; RUN:    | FileCheck %s --check-prefix=PROB90
; RUN: %p2i -i %s --filetype=asm -a -sz-seed=1 -nop-insertion \
; RUN:    -nop-insertion-percentage=50 -max-nops-per-instruction=2 \
; RUN:    | FileCheck %s --check-prefix=MAXNOPS2
; RUN: %p2i -i %s --filetype=asm -a -sz-seed=1 -nop-insertion -sandbox\
; RUN:    -nop-insertion-percentage=50 -max-nops-per-instruction=1 \
; RUN:    | FileCheck %s --check-prefix=SANDBOX50


define <4 x i32> @mul_v4i32(<4 x i32> %a, <4 x i32> %b) {
entry:
  %res = mul <4 x i32> %a, %b
  ret <4 x i32> %res

; PROB50-LABEL: mul_v4i32
; PROB50: nop # variant = 1
; PROB50: subl $60, %esp
; PROB50: nop # variant = 3
; PROB50: movups %xmm0, 32(%esp)
; PROB50: movups %xmm1, 16(%esp)
; PROB50: movups 32(%esp), %xmm0
; PROB50: nop # variant = 1
; PROB50: pshufd $49, 32(%esp), %xmm1
; PROB50: nop # variant = 4
; PROB50: pshufd $49, 16(%esp), %xmm2
; PROB50: nop # variant = 1
; PROB50: pmuludq 16(%esp), %xmm0
; PROB50: pmuludq %xmm2, %xmm1
; PROB50: nop # variant = 0
; PROB50: shufps $136, %xmm1, %xmm0
; PROB50: nop # variant = 3
; PROB50: pshufd $216, %xmm0, %xmm0
; PROB50: nop # variant = 1
; PROB50: movups %xmm0, (%esp)
; PROB50: movups (%esp), %xmm0
; PROB50: addl $60, %esp
; PROB50: ret

; PROB90-LABEL: mul_v4i32
; PROB90: nop # variant = 1
; PROB90: subl $60, %esp
; PROB90: nop # variant = 3
; PROB90: movups %xmm0, 32(%esp)
; PROB90: nop # variant = 4
; PROB90: movups %xmm1, 16(%esp)
; PROB90: nop # variant = 1
; PROB90: movups 32(%esp), %xmm0
; PROB90: nop # variant = 4
; PROB90: pshufd $49, 32(%esp), %xmm1
; PROB90: nop # variant = 1
; PROB90: pshufd $49, 16(%esp), %xmm2
; PROB90: nop # variant = 4
; PROB90: pmuludq 16(%esp), %xmm0
; PROB90: nop # variant = 2
; PROB90: pmuludq %xmm2, %xmm1
; PROB90: shufps $136, %xmm1, %xmm0
; PROB90: nop # variant = 1
; PROB90: pshufd $216, %xmm0, %xmm0
; PROB90: movups %xmm0, (%esp)
; PROB90: nop # variant = 1
; PROB90: movups (%esp), %xmm0
; PROB90: nop # variant = 0
; PROB90: addl $60, %esp
; PROB90: nop # variant = 0
; PROB90: ret
; PROB90: nop # variant = 4

; MAXNOPS2-LABEL: mul_v4i32
; MAXNOPS2: nop # variant = 1
; MAXNOPS2: nop # variant = 3
; MAXNOPS2: subl $60, %esp
; MAXNOPS2: movups %xmm0, 32(%esp)
; MAXNOPS2: nop # variant = 1
; MAXNOPS2: nop # variant = 4
; MAXNOPS2: movups %xmm1, 16(%esp)
; MAXNOPS2: nop # variant = 1
; MAXNOPS2: movups 32(%esp), %xmm0
; MAXNOPS2: nop # variant = 0
; MAXNOPS2: nop # variant = 3
; MAXNOPS2: pshufd $49, 32(%esp), %xmm1
; MAXNOPS2: nop # variant = 1
; MAXNOPS2: pshufd $49, 16(%esp), %xmm2
; MAXNOPS2: pmuludq 16(%esp), %xmm0
; MAXNOPS2: pmuludq %xmm2, %xmm1
; MAXNOPS2: nop # variant = 0
; MAXNOPS2: shufps $136, %xmm1, %xmm0
; MAXNOPS2: nop # variant = 0
; MAXNOPS2: nop # variant = 0
; MAXNOPS2: pshufd $216, %xmm0, %xmm0
; MAXNOPS2: nop # variant = 1
; MAXNOPS2: nop # variant = 3
; MAXNOPS2: movups %xmm0, (%esp)
; MAXNOPS2: nop # variant = 3
; MAXNOPS2: movups (%esp), %xmm0
; MAXNOPS2: addl $60, %esp
; MAXNOPS2: nop # variant = 3
; MAXNOPS2: ret


; SANDBOX50-LABEL: mul_v4i32
; SANDBOX50: nop # variant = 1
; SANDBOX50: subl $60, %esp
; SANDBOX50: nop # variant = 3
; SANDBOX50: movups %xmm0, 32(%esp)
; SANDBOX50: movups %xmm1, 16(%esp)
; SANDBOX50: movups 32(%esp), %xmm0
; SANDBOX50: nop # variant = 1
; SANDBOX50: pshufd $49, 32(%esp), %xmm1
; SANDBOX50: nop # variant = 4
; SANDBOX50: pshufd $49, 16(%esp), %xmm2
; SANDBOX50: nop # variant = 1
; SANDBOX50: pmuludq 16(%esp), %xmm0
; SANDBOX50: pmuludq %xmm2, %xmm1
; SANDBOX50: nop # variant = 0
; SANDBOX50: shufps $136, %xmm1, %xmm0
; SANDBOX50: nop # variant = 3
; SANDBOX50: pshufd $216, %xmm0, %xmm0
; SANDBOX50: nop # variant = 1
; SANDBOX50: movups %xmm0, (%esp)
; SANDBOX50: movups (%esp), %xmm0
; SANDBOX50: addl $60, %esp
; SANDBOX50: pop %ecx
; SANDBOX50: .bundle_lock
; SANDBOX50: andl $-32, %ecx
; SANDBOX50: jmp *%ecx
; SANDBOX50: .bundle_unlock

}
