; Test if we can read compare instructions.

; RUN: llvm-as < %s | pnacl-freeze \
; RUN:              | %llvm2ice -notranslate -verbose=inst -build-on-read \
; RUN:                -allow-pnacl-reader-error-recovery \
; RUN:              | FileCheck %s

define i1 @IcmpI1(i32 %p1, i32 %p2) {
  %a1 = trunc i32 %p1 to i1
  %a2 = trunc i32 %p2 to i1
  %veq = icmp eq i1 %a1, %a2
  %vne = icmp ne i1 %a1, %a2
  %vugt = icmp ugt i1 %a1, %a2
  %vuge = icmp uge i1 %a1, %a2
  %vult = icmp ult i1 %a1, %a2
  %vule = icmp ule i1 %a1, %a2
  %vsgt = icmp sgt i1 %a1, %a2
  %vsge = icmp sge i1 %a1, %a2
  %vslt = icmp slt i1 %a1, %a2
  %vsle = icmp sle i1 %a1, %a2
  ret i1 %veq
}

; CHECK: define i1 @IcmpI1(i32 %__0, i32 %__1) {
; CHECK-NEXT: __0:
; CHECK-NEXT:   %__2 = trunc i32 %__0 to i1
; CHECK-NEXT:   %__3 = trunc i32 %__1 to i1
; CHECK-NEXT:   %__4 = icmp eq i1 %__2, %__3
; CHECK-NEXT:   %__5 = icmp ne i1 %__2, %__3
; CHECK-NEXT:   %__6 = icmp ugt i1 %__2, %__3
; CHECK-NEXT:   %__7 = icmp uge i1 %__2, %__3
; CHECK-NEXT:   %__8 = icmp ult i1 %__2, %__3
; CHECK-NEXT:   %__9 = icmp ule i1 %__2, %__3
; CHECK-NEXT:   %__10 = icmp sgt i1 %__2, %__3
; CHECK-NEXT:   %__11 = icmp sge i1 %__2, %__3
; CHECK-NEXT:   %__12 = icmp slt i1 %__2, %__3
; CHECK-NEXT:   %__13 = icmp sle i1 %__2, %__3
; CHECK-NEXT:   ret i1 %__4
; CHECK-NEXT: }

define i1 @IcmpI8(i32 %p1, i32 %p2) {
  %a1 = trunc i32 %p1 to i8
  %a2 = trunc i32 %p2 to i8
  %veq = icmp eq i8 %a1, %a2
  %vne = icmp ne i8 %a1, %a2
  %vugt = icmp ugt i8 %a1, %a2
  %vuge = icmp uge i8 %a1, %a2
  %vult = icmp ult i8 %a1, %a2
  %vule = icmp ule i8 %a1, %a2
  %vsgt = icmp sgt i8 %a1, %a2
  %vsge = icmp sge i8 %a1, %a2
  %vslt = icmp slt i8 %a1, %a2
  %vsle = icmp sle i8 %a1, %a2
  ret i1 %veq
}

; CHECK-NEXT: define i1 @IcmpI8(i32 %__0, i32 %__1) {
; CHECK-NEXT: __0:
; CHECK-NEXT:   %__2 = trunc i32 %__0 to i8
; CHECK-NEXT:   %__3 = trunc i32 %__1 to i8
; CHECK-NEXT:   %__4 = icmp eq i8 %__2, %__3
; CHECK-NEXT:   %__5 = icmp ne i8 %__2, %__3
; CHECK-NEXT:   %__6 = icmp ugt i8 %__2, %__3
; CHECK-NEXT:   %__7 = icmp uge i8 %__2, %__3
; CHECK-NEXT:   %__8 = icmp ult i8 %__2, %__3
; CHECK-NEXT:   %__9 = icmp ule i8 %__2, %__3
; CHECK-NEXT:   %__10 = icmp sgt i8 %__2, %__3
; CHECK-NEXT:   %__11 = icmp sge i8 %__2, %__3
; CHECK-NEXT:   %__12 = icmp slt i8 %__2, %__3
; CHECK-NEXT:   %__13 = icmp sle i8 %__2, %__3
; CHECK-NEXT:   ret i1 %__4
; CHECK-NEXT: }

define i1 @IcmpI16(i32 %p1, i32 %p2) {
  %a1 = trunc i32 %p1 to i16
  %a2 = trunc i32 %p2 to i16
  %veq = icmp eq i16 %a1, %a2
  %vne = icmp ne i16 %a1, %a2
  %vugt = icmp ugt i16 %a1, %a2
  %vuge = icmp uge i16 %a1, %a2
  %vult = icmp ult i16 %a1, %a2
  %vule = icmp ule i16 %a1, %a2
  %vsgt = icmp sgt i16 %a1, %a2
  %vsge = icmp sge i16 %a1, %a2
  %vslt = icmp slt i16 %a1, %a2
  %vsle = icmp sle i16 %a1, %a2
  ret i1 %veq
}

; CHECK-NEXT: define i1 @IcmpI16(i32 %__0, i32 %__1) {
; CHECK-NEXT: __0:
; CHECK-NEXT:   %__2 = trunc i32 %__0 to i16
; CHECK-NEXT:   %__3 = trunc i32 %__1 to i16
; CHECK-NEXT:   %__4 = icmp eq i16 %__2, %__3
; CHECK-NEXT:   %__5 = icmp ne i16 %__2, %__3
; CHECK-NEXT:   %__6 = icmp ugt i16 %__2, %__3
; CHECK-NEXT:   %__7 = icmp uge i16 %__2, %__3
; CHECK-NEXT:   %__8 = icmp ult i16 %__2, %__3
; CHECK-NEXT:   %__9 = icmp ule i16 %__2, %__3
; CHECK-NEXT:   %__10 = icmp sgt i16 %__2, %__3
; CHECK-NEXT:   %__11 = icmp sge i16 %__2, %__3
; CHECK-NEXT:   %__12 = icmp slt i16 %__2, %__3
; CHECK-NEXT:   %__13 = icmp sle i16 %__2, %__3
; CHECK-NEXT:   ret i1 %__4
; CHECK-NEXT: }

define i1 @IcmpI32(i32 %a1, i32 %a2) {
  %veq = icmp eq i32 %a1, %a2
  %vne = icmp ne i32 %a1, %a2
  %vugt = icmp ugt i32 %a1, %a2
  %vuge = icmp uge i32 %a1, %a2
  %vult = icmp ult i32 %a1, %a2
  %vule = icmp ule i32 %a1, %a2
  %vsgt = icmp sgt i32 %a1, %a2
  %vsge = icmp sge i32 %a1, %a2
  %vslt = icmp slt i32 %a1, %a2
  %vsle = icmp sle i32 %a1, %a2
  ret i1 %veq
}

; CHECK-NEXT: define i1 @IcmpI32(i32 %__0, i32 %__1) {
; CHECK-NEXT: __0:
; CHECK-NEXT:   %__2 = icmp eq i32 %__0, %__1
; CHECK-NEXT:   %__3 = icmp ne i32 %__0, %__1
; CHECK-NEXT:   %__4 = icmp ugt i32 %__0, %__1
; CHECK-NEXT:   %__5 = icmp uge i32 %__0, %__1
; CHECK-NEXT:   %__6 = icmp ult i32 %__0, %__1
; CHECK-NEXT:   %__7 = icmp ule i32 %__0, %__1
; CHECK-NEXT:   %__8 = icmp sgt i32 %__0, %__1
; CHECK-NEXT:   %__9 = icmp sge i32 %__0, %__1
; CHECK-NEXT:   %__10 = icmp slt i32 %__0, %__1
; CHECK-NEXT:   %__11 = icmp sle i32 %__0, %__1
; CHECK-NEXT:   ret i1 %__2
; CHECK-NEXT: }

define i1 @IcmpI64(i64 %a1, i64 %a2) {
  %veq = icmp eq i64 %a1, %a2
  %vne = icmp ne i64 %a1, %a2
  %vugt = icmp ugt i64 %a1, %a2
  %vuge = icmp uge i64 %a1, %a2
  %vult = icmp ult i64 %a1, %a2
  %vule = icmp ule i64 %a1, %a2
  %vsgt = icmp sgt i64 %a1, %a2
  %vsge = icmp sge i64 %a1, %a2
  %vslt = icmp slt i64 %a1, %a2
  %vsle = icmp sle i64 %a1, %a2
  ret i1 %veq
}

; CHECK-NEXT: define i1 @IcmpI64(i64 %__0, i64 %__1) {
; CHECK-NEXT: __0:
; CHECK-NEXT:   %__2 = icmp eq i64 %__0, %__1
; CHECK-NEXT:   %__3 = icmp ne i64 %__0, %__1
; CHECK-NEXT:   %__4 = icmp ugt i64 %__0, %__1
; CHECK-NEXT:   %__5 = icmp uge i64 %__0, %__1
; CHECK-NEXT:   %__6 = icmp ult i64 %__0, %__1
; CHECK-NEXT:   %__7 = icmp ule i64 %__0, %__1
; CHECK-NEXT:   %__8 = icmp sgt i64 %__0, %__1
; CHECK-NEXT:   %__9 = icmp sge i64 %__0, %__1
; CHECK-NEXT:   %__10 = icmp slt i64 %__0, %__1
; CHECK-NEXT:   %__11 = icmp sle i64 %__0, %__1
; CHECK-NEXT:   ret i1 %__2
; CHECK-NEXT: }

define <4 x i1> @IcmpV4xI1(<4 x i1> %a1, <4 x i1> %a2) {
  %veq = icmp eq <4 x i1> %a1, %a2
  %vne = icmp ne <4 x i1> %a1, %a2
  %vugt = icmp ugt <4 x i1> %a1, %a2
  %vuge = icmp uge <4 x i1> %a1, %a2
  %vult = icmp ult <4 x i1> %a1, %a2
  %vule = icmp ule <4 x i1> %a1, %a2
  %vsgt = icmp sgt <4 x i1> %a1, %a2
  %vsge = icmp sge <4 x i1> %a1, %a2
  %vslt = icmp slt <4 x i1> %a1, %a2
  %vsle = icmp sle <4 x i1> %a1, %a2
  ret <4 x i1> %veq
}

; CHECK-NEXT: define <4 x i1> @IcmpV4xI1(<4 x i1> %__0, <4 x i1> %__1) {
; CHECK-NEXT: __0:
; CHECK-NEXT:   %__2 = icmp eq <4 x i1> %__0, %__1
; CHECK-NEXT:   %__3 = icmp ne <4 x i1> %__0, %__1
; CHECK-NEXT:   %__4 = icmp ugt <4 x i1> %__0, %__1
; CHECK-NEXT:   %__5 = icmp uge <4 x i1> %__0, %__1
; CHECK-NEXT:   %__6 = icmp ult <4 x i1> %__0, %__1
; CHECK-NEXT:   %__7 = icmp ule <4 x i1> %__0, %__1
; CHECK-NEXT:   %__8 = icmp sgt <4 x i1> %__0, %__1
; CHECK-NEXT:   %__9 = icmp sge <4 x i1> %__0, %__1
; CHECK-NEXT:   %__10 = icmp slt <4 x i1> %__0, %__1
; CHECK-NEXT:   %__11 = icmp sle <4 x i1> %__0, %__1
; CHECK-NEXT:   ret <4 x i1> %__2
; CHECK-NEXT: }

define <8 x i1> @IcmpV8xI1(<8 x i1> %a1, <8 x i1> %a2) {
  %veq = icmp eq <8 x i1> %a1, %a2
  %vne = icmp ne <8 x i1> %a1, %a2
  %vugt = icmp ugt <8 x i1> %a1, %a2
  %vuge = icmp uge <8 x i1> %a1, %a2
  %vult = icmp ult <8 x i1> %a1, %a2
  %vule = icmp ule <8 x i1> %a1, %a2
  %vsgt = icmp sgt <8 x i1> %a1, %a2
  %vsge = icmp sge <8 x i1> %a1, %a2
  %vslt = icmp slt <8 x i1> %a1, %a2
  %vsle = icmp sle <8 x i1> %a1, %a2
  ret <8 x i1> %veq
}

; CHECK-NEXT: define <8 x i1> @IcmpV8xI1(<8 x i1> %__0, <8 x i1> %__1) {
; CHECK-NEXT: __0:
; CHECK-NEXT:   %__2 = icmp eq <8 x i1> %__0, %__1
; CHECK-NEXT:   %__3 = icmp ne <8 x i1> %__0, %__1
; CHECK-NEXT:   %__4 = icmp ugt <8 x i1> %__0, %__1
; CHECK-NEXT:   %__5 = icmp uge <8 x i1> %__0, %__1
; CHECK-NEXT:   %__6 = icmp ult <8 x i1> %__0, %__1
; CHECK-NEXT:   %__7 = icmp ule <8 x i1> %__0, %__1
; CHECK-NEXT:   %__8 = icmp sgt <8 x i1> %__0, %__1
; CHECK-NEXT:   %__9 = icmp sge <8 x i1> %__0, %__1
; CHECK-NEXT:   %__10 = icmp slt <8 x i1> %__0, %__1
; CHECK-NEXT:   %__11 = icmp sle <8 x i1> %__0, %__1
; CHECK-NEXT:   ret <8 x i1> %__2
; CHECK-NEXT: }

define <16 x i1> @IcmpV16xI1(<16 x i1> %a1, <16 x i1> %a2) {
  %veq = icmp eq <16 x i1> %a1, %a2
  %vne = icmp ne <16 x i1> %a1, %a2
  %vugt = icmp ugt <16 x i1> %a1, %a2
  %vuge = icmp uge <16 x i1> %a1, %a2
  %vult = icmp ult <16 x i1> %a1, %a2
  %vule = icmp ule <16 x i1> %a1, %a2
  %vsgt = icmp sgt <16 x i1> %a1, %a2
  %vsge = icmp sge <16 x i1> %a1, %a2
  %vslt = icmp slt <16 x i1> %a1, %a2
  %vsle = icmp sle <16 x i1> %a1, %a2
  ret <16 x i1> %veq
}

; CHECK-NEXT: define <16 x i1> @IcmpV16xI1(<16 x i1> %__0, <16 x i1> %__1) {
; CHECK-NEXT: __0:
; CHECK-NEXT:   %__2 = icmp eq <16 x i1> %__0, %__1
; CHECK-NEXT:   %__3 = icmp ne <16 x i1> %__0, %__1
; CHECK-NEXT:   %__4 = icmp ugt <16 x i1> %__0, %__1
; CHECK-NEXT:   %__5 = icmp uge <16 x i1> %__0, %__1
; CHECK-NEXT:   %__6 = icmp ult <16 x i1> %__0, %__1
; CHECK-NEXT:   %__7 = icmp ule <16 x i1> %__0, %__1
; CHECK-NEXT:   %__8 = icmp sgt <16 x i1> %__0, %__1
; CHECK-NEXT:   %__9 = icmp sge <16 x i1> %__0, %__1
; CHECK-NEXT:   %__10 = icmp slt <16 x i1> %__0, %__1
; CHECK-NEXT:   %__11 = icmp sle <16 x i1> %__0, %__1
; CHECK-NEXT:   ret <16 x i1> %__2
; CHECK-NEXT: }

define <16 x i1> @IcmpV16xI8(<16 x i8> %a1, <16 x i8> %a2) {
  %veq = icmp eq <16 x i8> %a1, %a2
  %vne = icmp ne <16 x i8> %a1, %a2
  %vugt = icmp ugt <16 x i8> %a1, %a2
  %vuge = icmp uge <16 x i8> %a1, %a2
  %vult = icmp ult <16 x i8> %a1, %a2
  %vule = icmp ule <16 x i8> %a1, %a2
  %vsgt = icmp sgt <16 x i8> %a1, %a2
  %vsge = icmp sge <16 x i8> %a1, %a2
  %vslt = icmp slt <16 x i8> %a1, %a2
  %vsle = icmp sle <16 x i8> %a1, %a2
  ret <16 x i1> %veq
}

; CHECK-NEXT: define <16 x i1> @IcmpV16xI8(<16 x i8> %__0, <16 x i8> %__1) {
; CHECK-NEXT: __0:
; CHECK-NEXT:   %__2 = icmp eq <16 x i8> %__0, %__1
; CHECK-NEXT:   %__3 = icmp ne <16 x i8> %__0, %__1
; CHECK-NEXT:   %__4 = icmp ugt <16 x i8> %__0, %__1
; CHECK-NEXT:   %__5 = icmp uge <16 x i8> %__0, %__1
; CHECK-NEXT:   %__6 = icmp ult <16 x i8> %__0, %__1
; CHECK-NEXT:   %__7 = icmp ule <16 x i8> %__0, %__1
; CHECK-NEXT:   %__8 = icmp sgt <16 x i8> %__0, %__1
; CHECK-NEXT:   %__9 = icmp sge <16 x i8> %__0, %__1
; CHECK-NEXT:   %__10 = icmp slt <16 x i8> %__0, %__1
; CHECK-NEXT:   %__11 = icmp sle <16 x i8> %__0, %__1
; CHECK-NEXT:   ret <16 x i1> %__2
; CHECK-NEXT: }

define <8 x i1> @IcmpV8xI16(<8 x i16> %a1, <8 x i16> %a2) {
  %veq = icmp eq <8 x i16> %a1, %a2
  %vne = icmp ne <8 x i16> %a1, %a2
  %vugt = icmp ugt <8 x i16> %a1, %a2
  %vuge = icmp uge <8 x i16> %a1, %a2
  %vult = icmp ult <8 x i16> %a1, %a2
  %vule = icmp ule <8 x i16> %a1, %a2
  %vsgt = icmp sgt <8 x i16> %a1, %a2
  %vsge = icmp sge <8 x i16> %a1, %a2
  %vslt = icmp slt <8 x i16> %a1, %a2
  %vsle = icmp sle <8 x i16> %a1, %a2
  ret <8 x i1> %veq
}

; CHECK-NEXT: define <8 x i1> @IcmpV8xI16(<8 x i16> %__0, <8 x i16> %__1) {
; CHECK-NEXT: __0:
; CHECK-NEXT:   %__2 = icmp eq <8 x i16> %__0, %__1
; CHECK-NEXT:   %__3 = icmp ne <8 x i16> %__0, %__1
; CHECK-NEXT:   %__4 = icmp ugt <8 x i16> %__0, %__1
; CHECK-NEXT:   %__5 = icmp uge <8 x i16> %__0, %__1
; CHECK-NEXT:   %__6 = icmp ult <8 x i16> %__0, %__1
; CHECK-NEXT:   %__7 = icmp ule <8 x i16> %__0, %__1
; CHECK-NEXT:   %__8 = icmp sgt <8 x i16> %__0, %__1
; CHECK-NEXT:   %__9 = icmp sge <8 x i16> %__0, %__1
; CHECK-NEXT:   %__10 = icmp slt <8 x i16> %__0, %__1
; CHECK-NEXT:   %__11 = icmp sle <8 x i16> %__0, %__1
; CHECK-NEXT:   ret <8 x i1> %__2
; CHECK-NEXT: }

define <4 x i1> @IcmpV4xI32(<4 x i32> %a1, <4 x i32> %a2) {
  %veq = icmp eq <4 x i32> %a1, %a2
  %vne = icmp ne <4 x i32> %a1, %a2
  %vugt = icmp ugt <4 x i32> %a1, %a2
  %vuge = icmp uge <4 x i32> %a1, %a2
  %vult = icmp ult <4 x i32> %a1, %a2
  %vule = icmp ule <4 x i32> %a1, %a2
  %vsgt = icmp sgt <4 x i32> %a1, %a2
  %vsge = icmp sge <4 x i32> %a1, %a2
  %vslt = icmp slt <4 x i32> %a1, %a2
  %vsle = icmp sle <4 x i32> %a1, %a2
  ret <4 x i1> %veq
}

; CHECK-NEXT: define <4 x i1> @IcmpV4xI32(<4 x i32> %__0, <4 x i32> %__1) {
; CHECK-NEXT: __0:
; CHECK-NEXT:   %__2 = icmp eq <4 x i32> %__0, %__1
; CHECK-NEXT:   %__3 = icmp ne <4 x i32> %__0, %__1
; CHECK-NEXT:   %__4 = icmp ugt <4 x i32> %__0, %__1
; CHECK-NEXT:   %__5 = icmp uge <4 x i32> %__0, %__1
; CHECK-NEXT:   %__6 = icmp ult <4 x i32> %__0, %__1
; CHECK-NEXT:   %__7 = icmp ule <4 x i32> %__0, %__1
; CHECK-NEXT:   %__8 = icmp sgt <4 x i32> %__0, %__1
; CHECK-NEXT:   %__9 = icmp sge <4 x i32> %__0, %__1
; CHECK-NEXT:   %__10 = icmp slt <4 x i32> %__0, %__1
; CHECK-NEXT:   %__11 = icmp sle <4 x i32> %__0, %__1
; CHECK-NEXT:   ret <4 x i1> %__2
; CHECK-NEXT: }

define i1 @FcmpFloat(float %a1, float %a2) {
  %vfalse = fcmp false float %a1, %a2
  %voeq = fcmp oeq float %a1, %a2
  %vogt = fcmp ogt float %a1, %a2
  %voge = fcmp oge float %a1, %a2
  %volt = fcmp olt float %a1, %a2
  %vole = fcmp ole float %a1, %a2
  %vone = fcmp one float %a1, %a2
  %ord = fcmp ord float %a1, %a2
  %vueq = fcmp ueq float %a1, %a2
  %vugt = fcmp ugt float %a1, %a2
  %vuge = fcmp uge float %a1, %a2
  %vult = fcmp ult float %a1, %a2
  %vule = fcmp ule float %a1, %a2
  %vune = fcmp une float %a1, %a2
  %vuno = fcmp uno float %a1, %a2
  %vtrue = fcmp true float %a1, %a2
  ret i1 %voeq
}

; CHECK-NEXT: define i1 @FcmpFloat(float %__0, float %__1) {
; CHECK-NEXT: __0:
; CHECK-NEXT:   %__2 = fcmp false float %__0, %__1
; CHECK-NEXT:   %__3 = fcmp oeq float %__0, %__1
; CHECK-NEXT:   %__4 = fcmp ogt float %__0, %__1
; CHECK-NEXT:   %__5 = fcmp oge float %__0, %__1
; CHECK-NEXT:   %__6 = fcmp olt float %__0, %__1
; CHECK-NEXT:   %__7 = fcmp ole float %__0, %__1
; CHECK-NEXT:   %__8 = fcmp one float %__0, %__1
; CHECK-NEXT:   %__9 = fcmp ord float %__0, %__1
; CHECK-NEXT:   %__10 = fcmp ueq float %__0, %__1
; CHECK-NEXT:   %__11 = fcmp ugt float %__0, %__1
; CHECK-NEXT:   %__12 = fcmp uge float %__0, %__1
; CHECK-NEXT:   %__13 = fcmp ult float %__0, %__1
; CHECK-NEXT:   %__14 = fcmp ule float %__0, %__1
; CHECK-NEXT:   %__15 = fcmp une float %__0, %__1
; CHECK-NEXT:   %__16 = fcmp uno float %__0, %__1
; CHECK-NEXT:   %__17 = fcmp true float %__0, %__1
; CHECK-NEXT:   ret i1 %__3
; CHECK-NEXT: }

define i1 @FcmpDouble(double %a1, double %a2) {
  %vfalse = fcmp false double %a1, %a2
  %voeq = fcmp oeq double %a1, %a2
  %vogt = fcmp ogt double %a1, %a2
  %voge = fcmp oge double %a1, %a2
  %volt = fcmp olt double %a1, %a2
  %vole = fcmp ole double %a1, %a2
  %vone = fcmp one double %a1, %a2
  %ord = fcmp ord double %a1, %a2
  %vueq = fcmp ueq double %a1, %a2
  %vugt = fcmp ugt double %a1, %a2
  %vuge = fcmp uge double %a1, %a2
  %vult = fcmp ult double %a1, %a2
  %vule = fcmp ule double %a1, %a2
  %vune = fcmp une double %a1, %a2
  %vuno = fcmp uno double %a1, %a2
  %vtrue = fcmp true double %a1, %a2
  ret i1 %voeq
}

; CHECK-NEXT: define i1 @FcmpDouble(double %__0, double %__1) {
; CHECK-NEXT: __0:
; CHECK-NEXT:   %__2 = fcmp false double %__0, %__1
; CHECK-NEXT:   %__3 = fcmp oeq double %__0, %__1
; CHECK-NEXT:   %__4 = fcmp ogt double %__0, %__1
; CHECK-NEXT:   %__5 = fcmp oge double %__0, %__1
; CHECK-NEXT:   %__6 = fcmp olt double %__0, %__1
; CHECK-NEXT:   %__7 = fcmp ole double %__0, %__1
; CHECK-NEXT:   %__8 = fcmp one double %__0, %__1
; CHECK-NEXT:   %__9 = fcmp ord double %__0, %__1
; CHECK-NEXT:   %__10 = fcmp ueq double %__0, %__1
; CHECK-NEXT:   %__11 = fcmp ugt double %__0, %__1
; CHECK-NEXT:   %__12 = fcmp uge double %__0, %__1
; CHECK-NEXT:   %__13 = fcmp ult double %__0, %__1
; CHECK-NEXT:   %__14 = fcmp ule double %__0, %__1
; CHECK-NEXT:   %__15 = fcmp une double %__0, %__1
; CHECK-NEXT:   %__16 = fcmp uno double %__0, %__1
; CHECK-NEXT:   %__17 = fcmp true double %__0, %__1
; CHECK-NEXT:   ret i1 %__3
; CHECK-NEXT: }

define <4 x i1> @FcmpV4xFloat(<4 x float> %a1, <4 x float> %a2) {
  %vfalse = fcmp false <4 x float> %a1, %a2
  %voeq = fcmp oeq <4 x float> %a1, %a2
  %vogt = fcmp ogt <4 x float> %a1, %a2
  %voge = fcmp oge <4 x float> %a1, %a2
  %volt = fcmp olt <4 x float> %a1, %a2
  %vole = fcmp ole <4 x float> %a1, %a2
  %vone = fcmp one <4 x float> %a1, %a2
  %ord = fcmp ord <4 x float> %a1, %a2
  %vueq = fcmp ueq <4 x float> %a1, %a2
  %vugt = fcmp ugt <4 x float> %a1, %a2
  %vuge = fcmp uge <4 x float> %a1, %a2
  %vult = fcmp ult <4 x float> %a1, %a2
  %vule = fcmp ule <4 x float> %a1, %a2
  %vune = fcmp une <4 x float> %a1, %a2
  %vuno = fcmp uno <4 x float> %a1, %a2
  %vtrue = fcmp true <4 x float> %a1, %a2
  ret <4 x i1> %voeq
}

; CHECK-NEXT: define <4 x i1> @FcmpV4xFloat(<4 x float> %__0, <4 x float> %__1) {
; CHECK-NEXT: __0:
; CHECK-NEXT:   %__2 = fcmp false <4 x float> %__0, %__1
; CHECK-NEXT:   %__3 = fcmp oeq <4 x float> %__0, %__1
; CHECK-NEXT:   %__4 = fcmp ogt <4 x float> %__0, %__1
; CHECK-NEXT:   %__5 = fcmp oge <4 x float> %__0, %__1
; CHECK-NEXT:   %__6 = fcmp olt <4 x float> %__0, %__1
; CHECK-NEXT:   %__7 = fcmp ole <4 x float> %__0, %__1
; CHECK-NEXT:   %__8 = fcmp one <4 x float> %__0, %__1
; CHECK-NEXT:   %__9 = fcmp ord <4 x float> %__0, %__1
; CHECK-NEXT:   %__10 = fcmp ueq <4 x float> %__0, %__1
; CHECK-NEXT:   %__11 = fcmp ugt <4 x float> %__0, %__1
; CHECK-NEXT:   %__12 = fcmp uge <4 x float> %__0, %__1
; CHECK-NEXT:   %__13 = fcmp ult <4 x float> %__0, %__1
; CHECK-NEXT:   %__14 = fcmp ule <4 x float> %__0, %__1
; CHECK-NEXT:   %__15 = fcmp une <4 x float> %__0, %__1
; CHECK-NEXT:   %__16 = fcmp uno <4 x float> %__0, %__1
; CHECK-NEXT:   %__17 = fcmp true <4 x float> %__0, %__1
; CHECK-NEXT:   ret <4 x i1> %__3
; CHECK-NEXT: }
