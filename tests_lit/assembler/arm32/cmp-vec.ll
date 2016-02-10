; Test that we handle icmp and fcmp on vectors.

; TODO(eholk): This test will need to be updated once comparison is no
; longer scalarized.

; REQUIRES: allow_dump

; Compile using standalone assembler.
; RUN: %p2i --filetype=asm -i %s --target=arm32 --args -Om1 \
; RUN:   | FileCheck %s --check-prefix=ASM

; Show bytes in assembled standalone code.
; RUN: %p2i --filetype=asm -i %s --target=arm32 --assemble --disassemble \
; RUN:   --args -Om1 | FileCheck %s --check-prefix=DIS

; Show bytes in assembled integrated code.
; RUN: %p2i --filetype=iasm -i %s --target=arm32 --assemble --disassemble \
; RUN:   --args -Om1 | FileCheck %s --check-prefix=DIS

define internal <4 x i32> @cmpEq4I32(<4 x i32> %a, <4 x i32> %b) {
; ASM-LABEL:cmpEq4I32:
; DIS-LABEL:00000000 <cmpEq4I32>:

entry:
  %cmp = icmp eq <4 x i32> %a, %b

; ASM:        cmp     r1, r2
; ASM:        cmp     r1, r2
; ASM:        cmp     r1, r2
; ASM:        cmp     r1, r2
; DIS:  40:        e1510002

  %cmp.ret_ext = zext <4 x i1> %cmp to <4 x i32>
  ret <4 x i32> %cmp.ret_ext
}

define internal <4 x i32> @cmpEq4f32(<4 x float> %a, <4 x float> %b) {
; ASM-LABEL:cmpEq4f32:
; DIS-LABEL:00000240 <cmpEq4f32>:

entry:
  %cmp = fcmp oeq <4 x float> %a, %b

; ASM:        vcmp.f32 s0, s1
; ASM:        vcmp.f32 s0, s1
; ASM:        vcmp.f32 s0, s1
; ASM:        vcmp.f32 s0, s1
; DIS:  27c:  eeb40a60

  %cmp.ret_ext = zext <4 x i1> %cmp to <4 x i32>
  ret <4 x i32> %cmp.ret_ext
}
