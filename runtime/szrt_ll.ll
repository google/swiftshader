;;===- subzero/runtime/szrt_ll.ll - Subzero runtime source ----------------===;;
;;
;;                        The Subzero Code Generator
;;
;; This file is distributed under the University of Illinois Open Source
;; License. See LICENSE.TXT for details.
;;
;;===----------------------------------------------------------------------===;;
;;
;; This file implements wrappers for particular bitcode instructions that are
;; too uncommon and complex for a particular target to bother implementing
;; directly in Subzero target lowering.  This needs to be compiled by some
;; non-Subzero compiler.
;;
;;===----------------------------------------------------------------------===;;

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
