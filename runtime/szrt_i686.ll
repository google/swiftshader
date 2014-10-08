target triple = "i386-unknown-linux-gnu"

define <4 x float> @Sz_uitofp_v4i32(<4 x i32> %a) {
entry:
  %0 = uitofp <4 x i32> %a to <4 x float>
  ret <4 x float> %0
}

define <4 x i32> @Sz_fptoui_v4f32(<4 x float> %a) {
entry:
  %0 = fptoui <4 x float> %a to <4 x i32>
  ret <4 x i32> %0
}
