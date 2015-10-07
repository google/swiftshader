; Test the lowering sequence for commutative operations.  If there is a source
; operand whose lifetime ends in an operation, it should be the first operand,
; eliminating the need for a move to start the new lifetime.

; RUN: %if --need=target_X8632 --command %p2i --filetype=obj --disassemble \
; RUN:   --target x8632 -i %s --args -O2 \
; RUN:   | %if --need=target_X8632 --command FileCheck %s

define internal i32 @integerAddLeft(i32 %a, i32 %b) {
entry:
  %tmp = add i32 %a, %b
  %result = add i32 %a, %tmp
  ret i32 %result
}
; CHECK-LABEL: integerAddLeft
; CHECK-NEXT: mov {{e..}},DWORD PTR
; CHECK-NEXT: mov {{e..}},DWORD PTR
; CHECK-NEXT: add {{e..}},{{e..}}
; CHECK-NEXT: add {{e..}},{{e..}}

define internal i32 @integerAddRight(i32 %a, i32 %b) {
entry:
  %tmp = add i32 %a, %b
  %result = add i32 %b, %tmp
  ret i32 %result
}
; CHECK-LABEL: integerAddRight
; CHECK-NEXT: mov {{e..}},DWORD PTR
; CHECK-NEXT: mov {{e..}},DWORD PTR
; CHECK-NEXT: add {{e..}},{{e..}}
; CHECK-NEXT: add {{e..}},{{e..}}

define internal i32 @integerMultiplyLeft(i32 %a, i32 %b) {
entry:
  %tmp = mul i32 %a, %b
  %result = mul i32 %a, %tmp
  ret i32 %result
}
; CHECK-LABEL: integerMultiplyLeft
; CHECK-NEXT: mov {{e..}},DWORD PTR
; CHECK-NEXT: mov {{e..}},DWORD PTR
; CHECK-NEXT: imul {{e..}},{{e..}}
; CHECK-NEXT: imul {{e..}},{{e..}}

define internal i32 @integerMultiplyRight(i32 %a, i32 %b) {
entry:
  %tmp = mul i32 %a, %b
  %result = mul i32 %b, %tmp
  ret i32 %result
}
; CHECK-LABEL: integerMultiplyRight
; CHECK-NEXT: mov {{e..}},DWORD PTR
; CHECK-NEXT: mov {{e..}},DWORD PTR
; CHECK-NEXT: imul {{e..}},{{e..}}
; CHECK-NEXT: imul {{e..}},{{e..}}

define internal float @floatAddLeft(float %a, float %b) {
entry:
  %tmp = fadd float %a, %b
  %result = fadd float %a, %tmp
  ret float %result
}
; CHECK-LABEL: floatAddLeft
; CHECK-NEXT: movss xmm0,DWORD PTR
; CHECK-NEXT: movss xmm1,DWORD PTR
; CHECK-NEXT: addss xmm1,xmm0
; CHECK-NEXT: addss xmm0,xmm1

define internal float @floatAddRight(float %a, float %b) {
entry:
  %tmp = fadd float %a, %b
  %result = fadd float %b, %tmp
  ret float %result
}
; CHECK-LABEL: floatAddRight
; CHECK-NEXT: movss xmm0,DWORD PTR
; CHECK-NEXT: movss xmm1,DWORD PTR
; CHECK-NEXT: addss xmm0,xmm1
; CHECK-NEXT: addss xmm1,xmm0

define internal float @floatMultiplyLeft(float %a, float %b) {
entry:
  %tmp = fmul float %a, %b
  %result = fmul float %a, %tmp
  ret float %result
}
; CHECK-LABEL: floatMultiplyLeft
; CHECK-NEXT: movss xmm0,DWORD PTR
; CHECK-NEXT: movss xmm1,DWORD PTR
; CHECK-NEXT: mulss xmm1,xmm0
; CHECK-NEXT: mulss xmm0,xmm1

define internal float @floatMultiplyRight(float %a, float %b) {
entry:
  %tmp = fmul float %a, %b
  %result = fmul float %b, %tmp
  ret float %result
}
; CHECK-LABEL: floatMultiplyRight
; CHECK-NEXT: movss xmm0,DWORD PTR
; CHECK-NEXT: movss xmm1,DWORD PTR
; CHECK-NEXT: mulss xmm0,xmm1
; CHECK-NEXT: mulss xmm1,xmm0
