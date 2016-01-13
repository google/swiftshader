; Test the a=b*b lowering sequence which can use a single temporary register
; instead of two registers.

; RUN: %if --need=target_X8632 --command %p2i --filetype=obj --disassemble \
; RUN:   --target x8632 -i %s --args -O2 -mattr=sse4.1 \
; RUN:   | %if --need=target_X8632 --command FileCheck %s

; RUN: %if --need=target_X8632 --command %p2i --filetype=obj --disassemble \
; RUN:   --target x8632 -i %s --args -Om1 -mattr=sse4.1 \
; RUN:   | %if --need=target_X8632 --command FileCheck %s

define internal float @Square_float(float %a) {
entry:
  %result = fmul float %a, %a
  ret float %result
}
; CHECK-LABEL: Square_float
; CHECK: mulss [[REG:xmm.]],[[REG]]

define internal double @Square_double(double %a) {
entry:
  %result = fmul double %a, %a
  ret double %result
}
; CHECK-LABEL: Square_double
; CHECK: mulsd [[REG:xmm.]],[[REG]]

define internal i32 @Square_i32(i32 %a) {
entry:
  %result = mul i32 %a, %a
  ret i32 %result
}
; CHECK-LABEL: Square_i32
; CHECK: imul [[REG:e..]],[[REG]]

define internal i32 @Square_i16(i32 %a) {
entry:
  %a.16 = trunc i32 %a to i16
  %result = mul i16 %a.16, %a.16
  %result.i32 = sext i16 %result to i32
  ret i32 %result.i32
}
; CHECK-LABEL: Square_i16
; CHECK: imul [[REG:..]],[[REG]]

define internal i32 @Square_i8(i32 %a) {
entry:
  %a.8 = trunc i32 %a to i8
  %result = mul i8 %a.8, %a.8
  %result.i32 = sext i8 %result to i32
  ret i32 %result.i32
}
; CHECK-LABEL: Square_i8
; CHECK: imul al

define internal <4 x float> @Square_v4f32(<4 x float> %a) {
entry:
  %result = fmul <4 x float> %a, %a
  ret <4 x float> %result
}
; CHECK-LABEL: Square_v4f32
; CHECK: mulps [[REG:xmm.]],[[REG]]

define internal <4 x i32> @Square_v4i32(<4 x i32> %a) {
entry:
  %result = mul <4 x i32> %a, %a
  ret <4 x i32> %result
}
; CHECK-LABEL: Square_v4i32
; CHECK: pmulld [[REG:xmm.]],[[REG]]

define internal <8 x i16> @Square_v8i16(<8 x i16> %a) {
entry:
  %result = mul <8 x i16> %a, %a
  ret <8 x i16> %result
}
; CHECK-LABEL: Square_v8i16
; CHECK: pmullw [[REG:xmm.]],[[REG]]

define internal <16 x i8> @Square_v16i8(<16 x i8> %a) {
entry:
  %result = mul <16 x i8> %a, %a
  ret <16 x i8> %result
}
; CHECK-LABEL: Square_v16i8
; CHECK-NOT: pmul
