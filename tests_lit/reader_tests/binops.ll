; Tests if we can read binary operators.

; RUN: llvm-as < %s | pnacl-freeze \
; RUN:              | %llvm2ice -notranslate -verbose=inst -build-on-read \
; RUN:                -allow-pnacl-reader-error-recovery \
; RUN:              | FileCheck %s

; TODO(kschimpf): add i8/i16. Needs bitcasts.

define i32 @AddI32(i32 %a, i32 %b) {
  %add = add i32 %b, %a
  ret i32 %add
}

; CHECK:      define i32 @AddI32(i32 %__0, i32 %__1) {
; CHECK-NEXT: __0:
; CHECK-NEXT:   %__2 = add i32 %__1, %__0
; CHECK-NEXT:   ret i32 %__2
; CHECK-NEXT: }

define i64 @AddI64(i64 %a, i64 %b) {
  %add = add i64 %b, %a
  ret i64 %add
}

; CHECK-NEXT: define i64 @AddI64(i64 %__0, i64 %__1) {
; CHECK-NEXT: __0:
; CHECK-NEXT:   %__2 = add i64 %__1, %__0
; CHECK-NEXT:   ret i64 %__2
; CHECK-NEXT: }

define <16 x i8> @AddV16I8(<16 x i8> %a, <16 x i8> %b) {
  %add = add <16 x i8> %b, %a
  ret <16 x i8> %add
}

; CHECK-NEXT: define <16 x i8> @AddV16I8(<16 x i8> %__0, <16 x i8> %__1) {
; CHECK-NEXT: __0:
; CHECK-NEXT:   %__2 = add <16 x i8> %__1, %__0
; CHECK-NEXT:   ret <16 x i8> %__2
; CHECK-NEXT: }

define <8 x i16> @AddV8I16(<8 x i16> %a, <8 x i16> %b) {
  %add = add <8 x i16> %b, %a
  ret <8 x i16> %add
}

; CHECK-NEXT: define <8 x i16> @AddV8I16(<8 x i16> %__0, <8 x i16> %__1) {
; CHECK-NEXT: __0:
; CHECK-NEXT:   %__2 = add <8 x i16> %__1, %__0
; CHECK-NEXT:   ret <8 x i16> %__2
; CHECK-NEXT: }

define <4 x i32> @AddV4I32(<4 x i32> %a, <4 x i32> %b) {
  %add = add <4 x i32> %b, %a
  ret <4 x i32> %add
}

; CHECK-NEXT: define <4 x i32> @AddV4I32(<4 x i32> %__0, <4 x i32> %__1) {
; CHECK-NEXT: __0:
; CHECK-NEXT:   %__2 = add <4 x i32> %__1, %__0
; CHECK-NEXT:   ret <4 x i32> %__2
; CHECK-NEXT: }

define float @AddFloat(float %a, float %b) {
  %add = fadd float %b, %a
  ret float %add
}

; CHECK-NEXT: define float @AddFloat(float %__0, float %__1) {
; CHECK-NEXT: __0:
; CHECK-NEXT:   %__2 = fadd float %__1, %__0
; CHECK-NEXT:   ret float %__2
; CHECK-NEXT: }

define double @AddDouble(double %a, double %b) {
  %add = fadd double %b, %a
  ret double %add
}

; CHECK-NEXT: define double @AddDouble(double %__0, double %__1) {
; CHECK-NEXT: __0:
; CHECK-NEXT:   %__2 = fadd double %__1, %__0
; CHECK-NEXT:   ret double %__2
; CHECK-NEXT: }

define <4 x float> @AddV4Float(<4 x float> %a, <4 x float> %b) {
  %add = fadd <4 x float> %b, %a
  ret <4 x float> %add
}

; CHECK-NEXT: define <4 x float> @AddV4Float(<4 x float> %__0, <4 x float> %__1) {
; CHECK-NEXT: __0:
; CHECK-NEXT:   %__2 = fadd <4 x float> %__1, %__0
; CHECK-NEXT:   ret <4 x float> %__2
; CHECK-NEXT: }

; TODO(kschimpf): sub i8/i16. Needs bitcasts.

define i32 @SubI32(i32 %a, i32 %b) {
  %sub = sub i32 %a, %b
  ret i32 %sub
}

; CHECK-NEXT: define i32 @SubI32(i32 %__0, i32 %__1) {
; CHECK-NEXT: __0:
; CHECK-NEXT:   %__2 = sub i32 %__0, %__1
; CHECK-NEXT:   ret i32 %__2
; CHECK-NEXT: }

define i64 @SubI64(i64 %a, i64 %b) {
  %sub = sub i64 %a, %b
  ret i64 %sub
}

; CHECK-NEXT: define i64 @SubI64(i64 %__0, i64 %__1) {
; CHECK-NEXT: __0:
; CHECK-NEXT:   %__2 = sub i64 %__0, %__1
; CHECK-NEXT:   ret i64 %__2
; CHECK-NEXT: }

define <16 x i8> @SubV16I8(<16 x i8> %a, <16 x i8> %b) {
  %sub = sub <16 x i8> %a, %b
  ret <16 x i8> %sub
}

; CHECK-NEXT: define <16 x i8> @SubV16I8(<16 x i8> %__0, <16 x i8> %__1) {
; CHECK-NEXT: __0:
; CHECK-NEXT:   %__2 = sub <16 x i8> %__0, %__1
; CHECK-NEXT:   ret <16 x i8> %__2
; CHECK-NEXT: }

define <8 x i16> @SubV8I16(<8 x i16> %a, <8 x i16> %b) {
  %sub = sub <8 x i16> %a, %b
  ret <8 x i16> %sub
}

; CHECK-NEXT: define <8 x i16> @SubV8I16(<8 x i16> %__0, <8 x i16> %__1) {
; CHECK-NEXT: __0:
; CHECK-NEXT:   %__2 = sub <8 x i16> %__0, %__1
; CHECK-NEXT:   ret <8 x i16> %__2
; CHECK-NEXT: }

define <4 x i32> @SubV4I32(<4 x i32> %a, <4 x i32> %b) {
  %sub = sub <4 x i32> %a, %b
  ret <4 x i32> %sub
}

; CHECK-NEXT: define <4 x i32> @SubV4I32(<4 x i32> %__0, <4 x i32> %__1) {
; CHECK-NEXT: __0:
; CHECK-NEXT:   %__2 = sub <4 x i32> %__0, %__1
; CHECK-NEXT:   ret <4 x i32> %__2
; CHECK-NEXT: }

define float @SubFloat(float %a, float %b) {
  %sub = fsub float %a, %b
  ret float %sub
}

; CHECK-NEXT: define float @SubFloat(float %__0, float %__1) {
; CHECK-NEXT: __0:
; CHECK-NEXT:   %__2 = fsub float %__0, %__1
; CHECK-NEXT:   ret float %__2
; CHECK-NEXT: }

define double @SubDouble(double %a, double %b) {
  %sub = fsub double %a, %b
  ret double %sub
}

; CHECK-NEXT: define double @SubDouble(double %__0, double %__1) {
; CHECK-NEXT: __0:
; CHECK-NEXT:   %__2 = fsub double %__0, %__1
; CHECK-NEXT:   ret double %__2
; CHECK-NEXT: }

define <4 x float> @SubV4Float(<4 x float> %a, <4 x float> %b) {
  %sub = fsub <4 x float> %a, %b
  ret <4 x float> %sub
}

; CHECK-NEXT: define <4 x float> @SubV4Float(<4 x float> %__0, <4 x float> %__1) {
; CHECK-NEXT: __0:
; CHECK-NEXT:   %__2 = fsub <4 x float> %__0, %__1
; CHECK-NEXT:   ret <4 x float> %__2
; CHECK-NEXT: }

; TODO(kschimpf): mul i8/i16. Needs bitcasts.

define i32 @MulI32(i32 %a, i32 %b) {
  %mul = mul i32 %b, %a
  ret i32 %mul
}

; CHECK-NEXT: define i32 @MulI32(i32 %__0, i32 %__1) {
; CHECK-NEXT: __0:
; CHECK-NEXT:   %__2 = mul i32 %__1, %__0
; CHECK-NEXT:   ret i32 %__2
; CHECK-NEXT: }

define i64 @MulI64(i64 %a, i64 %b) {
  %mul = mul i64 %b, %a
  ret i64 %mul
}

; CHECK-NEXT: define i64 @MulI64(i64 %__0, i64 %__1) {
; CHECK-NEXT: __0:
; CHECK-NEXT:   %__2 = mul i64 %__1, %__0
; CHECK-NEXT:   ret i64 %__2
; CHECK-NEXT: }


define <16 x i8> @MulV16I8(<16 x i8> %a, <16 x i8> %b) {
  %mul = mul <16 x i8> %b, %a
  ret <16 x i8> %mul
}

; CHECK-NEXT: define <16 x i8> @MulV16I8(<16 x i8> %__0, <16 x i8> %__1) {
; CHECK-NEXT: __0:
; CHECK-NEXT:   %__2 = mul <16 x i8> %__1, %__0
; CHECK-NEXT:   ret <16 x i8> %__2
; CHECK-NEXT: }

define float @MulFloat(float %a, float %b) {
  %mul = fmul float %b, %a
  ret float %mul
}

; CHECK-NEXT: define float @MulFloat(float %__0, float %__1) {
; CHECK-NEXT: __0:
; CHECK-NEXT:   %__2 = fmul float %__1, %__0
; CHECK-NEXT:   ret float %__2
; CHECK-NEXT: }

define double @MulDouble(double %a, double %b) {
  %mul = fmul double %b, %a
  ret double %mul
}

; CHECK-NEXT: define double @MulDouble(double %__0, double %__1) {
; CHECK-NEXT: __0:
; CHECK-NEXT:   %__2 = fmul double %__1, %__0
; CHECK-NEXT:   ret double %__2
; CHECK-NEXT: }

define <4 x float> @MulV4Float(<4 x float> %a, <4 x float> %b) {
  %mul = fmul <4 x float> %b, %a
  ret <4 x float> %mul
}

; CHECK-NEXT: define <4 x float> @MulV4Float(<4 x float> %__0, <4 x float> %__1) {
; CHECK-NEXT: __0:
; CHECK-NEXT:   %__2 = fmul <4 x float> %__1, %__0
; CHECK-NEXT:   ret <4 x float> %__2
; CHECK-NEXT: }

; TODO(kschimpf): sdiv i8/i16. Needs bitcasts.

define i32 @SdivI32(i32 %a, i32 %b) {
  %div = sdiv i32 %a, %b
  ret i32 %div
}

; CHECK-NEXT: define i32 @SdivI32(i32 %__0, i32 %__1) {
; CHECK-NEXT: __0:
; CHECK-NEXT:   %__2 = sdiv i32 %__0, %__1
; CHECK-NEXT:   ret i32 %__2
; CHECK-NEXT: }

define i64 @SdivI64(i64 %a, i64 %b) {
  %div = sdiv i64 %a, %b
  ret i64 %div
}

; CHECK-NEXT: define i64 @SdivI64(i64 %__0, i64 %__1) {
; CHECK-NEXT: __0:
; CHECK-NEXT:   %__2 = sdiv i64 %__0, %__1
; CHECK-NEXT:   ret i64 %__2
; CHECK-NEXT: }

define <16 x i8> @SdivV16I8(<16 x i8> %a, <16 x i8> %b) {
  %div = sdiv <16 x i8> %a, %b
  ret <16 x i8> %div
}

; CHECK-NEXT: define <16 x i8> @SdivV16I8(<16 x i8> %__0, <16 x i8> %__1) {
; CHECK-NEXT: __0:
; CHECK-NEXT:   %__2 = sdiv <16 x i8> %__0, %__1
; CHECK-NEXT:   ret <16 x i8> %__2
; CHECK-NEXT: }

define <8 x i16> @SdivV8I16(<8 x i16> %a, <8 x i16> %b) {
  %div = sdiv <8 x i16> %a, %b
  ret <8 x i16> %div
}

; CHECK-NEXT: define <8 x i16> @SdivV8I16(<8 x i16> %__0, <8 x i16> %__1) {
; CHECK-NEXT: __0:
; CHECK-NEXT:   %__2 = sdiv <8 x i16> %__0, %__1
; CHECK-NEXT:   ret <8 x i16> %__2
; CHECK-NEXT: }

define <4 x i32> @SdivV4I32(<4 x i32> %a, <4 x i32> %b) {
  %div = sdiv <4 x i32> %a, %b
  ret <4 x i32> %div
}

; CHECK-NEXT: define <4 x i32> @SdivV4I32(<4 x i32> %__0, <4 x i32> %__1) {
; CHECK-NEXT: __0:
; CHECK-NEXT:   %__2 = sdiv <4 x i32> %__0, %__1
; CHECK-NEXT:   ret <4 x i32> %__2
; CHECK-NEXT: }

; TODO(kschimpf): srem i8/i16. Needs bitcasts.

define i32 @SremI32(i32 %a, i32 %b) {
  %rem = srem i32 %a, %b
  ret i32 %rem
}

; CHECK-NEXT: define i32 @SremI32(i32 %__0, i32 %__1) {
; CHECK-NEXT: __0:
; CHECK-NEXT:   %__2 = srem i32 %__0, %__1
; CHECK-NEXT:   ret i32 %__2
; CHECK-NEXT: }

define i64 @SremI64(i64 %a, i64 %b) {
  %rem = srem i64 %a, %b
  ret i64 %rem
}

; CHECK-NEXT: define i64 @SremI64(i64 %__0, i64 %__1) {
; CHECK-NEXT: __0:
; CHECK-NEXT:   %__2 = srem i64 %__0, %__1
; CHECK-NEXT:   ret i64 %__2
; CHECK-NEXT: }

define <16 x i8> @SremV16I8(<16 x i8> %a, <16 x i8> %b) {
  %rem = srem <16 x i8> %a, %b
  ret <16 x i8> %rem
}

; CHECK-NEXT: define <16 x i8> @SremV16I8(<16 x i8> %__0, <16 x i8> %__1) {
; CHECK-NEXT: __0:
; CHECK-NEXT:   %__2 = srem <16 x i8> %__0, %__1
; CHECK-NEXT:   ret <16 x i8> %__2
; CHECK-NEXT: }

define <8 x i16> @SremV8I16(<8 x i16> %a, <8 x i16> %b) {
  %rem = srem <8 x i16> %a, %b
  ret <8 x i16> %rem
}

; CHECK-NEXT: define <8 x i16> @SremV8I16(<8 x i16> %__0, <8 x i16> %__1) {
; CHECK-NEXT: __0:
; CHECK-NEXT:   %__2 = srem <8 x i16> %__0, %__1
; CHECK-NEXT:   ret <8 x i16> %__2
; CHECK-NEXT: }

define <4 x i32> @SremV4I32(<4 x i32> %a, <4 x i32> %b) {
  %rem = srem <4 x i32> %a, %b
  ret <4 x i32> %rem
}

; CHECK-NEXT: define <4 x i32> @SremV4I32(<4 x i32> %__0, <4 x i32> %__1) {
; CHECK-NEXT: __0:
; CHECK-NEXT:   %__2 = srem <4 x i32> %__0, %__1
; CHECK-NEXT:   ret <4 x i32> %__2
; CHECK-NEXT: }

; TODO(kschimpf): udiv i8/i16. Needs bitcasts.

define i32 @UdivI32(i32 %a, i32 %b) {
  %div = udiv i32 %a, %b
  ret i32 %div
}

; CHECK-NEXT: define i32 @UdivI32(i32 %__0, i32 %__1) {
; CHECK-NEXT: __0:
; CHECK-NEXT:   %__2 = udiv i32 %__0, %__1
; CHECK-NEXT:   ret i32 %__2
; CHECK-NEXT: }

define i64 @UdivI64(i64 %a, i64 %b) {
  %div = udiv i64 %a, %b
  ret i64 %div
}

; CHECK-NEXT: define i64 @UdivI64(i64 %__0, i64 %__1) {
; CHECK-NEXT: __0:
; CHECK-NEXT:   %__2 = udiv i64 %__0, %__1
; CHECK-NEXT:   ret i64 %__2
; CHECK-NEXT: }

define <16 x i8> @UdivV16I8(<16 x i8> %a, <16 x i8> %b) {
  %div = udiv <16 x i8> %a, %b
  ret <16 x i8> %div
}

; CHECK-NEXT: define <16 x i8> @UdivV16I8(<16 x i8> %__0, <16 x i8> %__1) {
; CHECK-NEXT: __0:
; CHECK-NEXT:   %__2 = udiv <16 x i8> %__0, %__1
; CHECK-NEXT:   ret <16 x i8> %__2
; CHECK-NEXT: }

define <8 x i16> @UdivV8I16(<8 x i16> %a, <8 x i16> %b) {
  %div = udiv <8 x i16> %a, %b
  ret <8 x i16> %div
}

; CHECK-NEXT: define <8 x i16> @UdivV8I16(<8 x i16> %__0, <8 x i16> %__1) {
; CHECK-NEXT: __0:
; CHECK-NEXT:   %__2 = udiv <8 x i16> %__0, %__1
; CHECK-NEXT:   ret <8 x i16> %__2
; CHECK-NEXT: }

define <4 x i32> @UdivV4I32(<4 x i32> %a, <4 x i32> %b) {
  %div = udiv <4 x i32> %a, %b
  ret <4 x i32> %div
}

; CHECK-NEXT: define <4 x i32> @UdivV4I32(<4 x i32> %__0, <4 x i32> %__1) {
; CHECK-NEXT: __0:
; CHECK-NEXT:   %__2 = udiv <4 x i32> %__0, %__1
; CHECK-NEXT:   ret <4 x i32> %__2
; CHECK-NEXT: }

; TODO(kschimpf): urem i8/i16. Needs bitcasts.

define i32 @UremI32(i32 %a, i32 %b) {
  %rem = urem i32 %a, %b
  ret i32 %rem
}

; CHECK-NEXT: define i32 @UremI32(i32 %__0, i32 %__1) {
; CHECK-NEXT: __0:
; CHECK-NEXT:   %__2 = urem i32 %__0, %__1
; CHECK-NEXT:   ret i32 %__2
; CHECK-NEXT: }

define i64 @UremI64(i64 %a, i64 %b) {
  %rem = urem i64 %a, %b
  ret i64 %rem
}

; CHECK-NEXT: define i64 @UremI64(i64 %__0, i64 %__1) {
; CHECK-NEXT: __0:
; CHECK-NEXT:   %__2 = urem i64 %__0, %__1
; CHECK-NEXT:   ret i64 %__2
; CHECK-NEXT: }

define <16 x i8> @UremV16I8(<16 x i8> %a, <16 x i8> %b) {
  %rem = urem <16 x i8> %a, %b
  ret <16 x i8> %rem
}

; CHECK-NEXT: define <16 x i8> @UremV16I8(<16 x i8> %__0, <16 x i8> %__1) {
; CHECK-NEXT: __0:
; CHECK-NEXT:   %__2 = urem <16 x i8> %__0, %__1
; CHECK-NEXT:   ret <16 x i8> %__2
; CHECK-NEXT: }

define <8 x i16> @UremV8I16(<8 x i16> %a, <8 x i16> %b) {
  %rem = urem <8 x i16> %a, %b
  ret <8 x i16> %rem
}

; CHECK-NEXT: define <8 x i16> @UremV8I16(<8 x i16> %__0, <8 x i16> %__1) {
; CHECK-NEXT: __0:
; CHECK-NEXT:   %__2 = urem <8 x i16> %__0, %__1
; CHECK-NEXT:   ret <8 x i16> %__2
; CHECK-NEXT: }

define <4 x i32> @UremV4I32(<4 x i32> %a, <4 x i32> %b) {
  %rem = urem <4 x i32> %a, %b
  ret <4 x i32> %rem
}

; CHECK-NEXT: define <4 x i32> @UremV4I32(<4 x i32> %__0, <4 x i32> %__1) {
; CHECK-NEXT: __0:
; CHECK-NEXT:   %__2 = urem <4 x i32> %__0, %__1
; CHECK-NEXT:   ret <4 x i32> %__2
; CHECK-NEXT: }

define float @fdivFloat(float %a, float %b) {
  %div = fdiv float %a, %b
  ret float %div
}

; CHECK-NEXT: define float @fdivFloat(float %__0, float %__1) {
; CHECK-NEXT: __0:
; CHECK-NEXT:   %__2 = fdiv float %__0, %__1
; CHECK-NEXT:   ret float %__2
; CHECK-NEXT: }

define double @fdivDouble(double %a, double %b) {
  %div = fdiv double %a, %b
  ret double %div
}

; CHECK-NEXT: define double @fdivDouble(double %__0, double %__1) {
; CHECK-NEXT: __0:
; CHECK-NEXT:   %__2 = fdiv double %__0, %__1
; CHECK-NEXT:   ret double %__2
; CHECK-NEXT: }

define <4 x float> @fdivV4Float(<4 x float> %a, <4 x float> %b) {
  %div = fdiv <4 x float> %a, %b
  ret <4 x float> %div
}

; CHECK-NEXT: define <4 x float> @fdivV4Float(<4 x float> %__0, <4 x float> %__1) {
; CHECK-NEXT: __0:
; CHECK-NEXT:   %__2 = fdiv <4 x float> %__0, %__1
; CHECK-NEXT:   ret <4 x float> %__2
; CHECK-NEXT: }

define float @fremFloat(float %a, float %b) {
  %rem = frem float %a, %b
  ret float %rem
}

; CHECK-NEXT: define float @fremFloat(float %__0, float %__1) {
; CHECK-NEXT: __0:
; CHECK-NEXT:   %__2 = frem float %__0, %__1
; CHECK-NEXT:   ret float %__2
; CHECK-NEXT: }


define double @fremDouble(double %a, double %b) {
  %rem = frem double %a, %b
  ret double %rem
}

; CHECK-NEXT: define double @fremDouble(double %__0, double %__1) {
; CHECK-NEXT: __0:
; CHECK-NEXT:   %__2 = frem double %__0, %__1
; CHECK-NEXT:   ret double %__2
; CHECK-NEXT: }

define <4 x float> @fremV4Float(<4 x float> %a, <4 x float> %b) {
  %rem = frem <4 x float> %a, %b
  ret <4 x float> %rem
}

; CHECK-NEXT: define <4 x float> @fremV4Float(<4 x float> %__0, <4 x float> %__1) {
; CHECK-NEXT: __0:
; CHECK-NEXT:   %__2 = frem <4 x float> %__0, %__1
; CHECK-NEXT:   ret <4 x float> %__2
; CHECK-NEXT: }

; TODO(kschimpf): and i1/i8/i16. Needs bitcasts.

define i32 @AndI32(i32 %a, i32 %b) {
  %and = and i32 %b, %a
  ret i32 %and
}

; CHECK-NEXT: define i32 @AndI32(i32 %__0, i32 %__1) {
; CHECK-NEXT: __0:
; CHECK-NEXT:   %__2 = and i32 %__1, %__0
; CHECK-NEXT:   ret i32 %__2
; CHECK-NEXT: }

define i64 @AndI64(i64 %a, i64 %b) {
  %and = and i64 %b, %a
  ret i64 %and
}

; CHECK-NEXT: define i64 @AndI64(i64 %__0, i64 %__1) {
; CHECK-NEXT: __0:
; CHECK-NEXT:   %__2 = and i64 %__1, %__0
; CHECK-NEXT:   ret i64 %__2
; CHECK-NEXT: }

define <16 x i8> @AndV16I8(<16 x i8> %a, <16 x i8> %b) {
  %and = and <16 x i8> %b, %a
  ret <16 x i8> %and
}

; CHECK-NEXT: define <16 x i8> @AndV16I8(<16 x i8> %__0, <16 x i8> %__1) {
; CHECK-NEXT: __0:
; CHECK-NEXT:   %__2 = and <16 x i8> %__1, %__0
; CHECK-NEXT:   ret <16 x i8> %__2
; CHECK-NEXT: }

define <8 x i16> @AndV8I16(<8 x i16> %a, <8 x i16> %b) {
  %and = and <8 x i16> %b, %a
  ret <8 x i16> %and
}

; CHECK-NEXT: define <8 x i16> @AndV8I16(<8 x i16> %__0, <8 x i16> %__1) {
; CHECK-NEXT: __0:
; CHECK-NEXT:   %__2 = and <8 x i16> %__1, %__0
; CHECK-NEXT:   ret <8 x i16> %__2
; CHECK-NEXT: }

define <4 x i32> @AndV4I32(<4 x i32> %a, <4 x i32> %b) {
  %and = and <4 x i32> %b, %a
  ret <4 x i32> %and
}

; CHECK-NEXT: define <4 x i32> @AndV4I32(<4 x i32> %__0, <4 x i32> %__1) {
; CHECK-NEXT: __0:
; CHECK-NEXT:   %__2 = and <4 x i32> %__1, %__0
; CHECK-NEXT:   ret <4 x i32> %__2
; CHECK-NEXT: }

; TODO(kschimpf): or i1/i8/i16. Needs bitcasts.

define i32 @OrI32(i32 %a, i32 %b) {
  %or = or i32 %b, %a
  ret i32 %or
}

; CHECK-NEXT: define i32 @OrI32(i32 %__0, i32 %__1) {
; CHECK-NEXT: __0:
; CHECK-NEXT:   %__2 = or i32 %__1, %__0
; CHECK-NEXT:   ret i32 %__2
; CHECK-NEXT: }

define i64 @OrI64(i64 %a, i64 %b) {
  %or = or i64 %b, %a
  ret i64 %or
}

; CHECK-NEXT: define i64 @OrI64(i64 %__0, i64 %__1) {
; CHECK-NEXT: __0:
; CHECK-NEXT:   %__2 = or i64 %__1, %__0
; CHECK-NEXT:   ret i64 %__2
; CHECK-NEXT: }

define <16 x i8> @OrV16I8(<16 x i8> %a, <16 x i8> %b) {
  %or = or <16 x i8> %b, %a
  ret <16 x i8> %or
}

; CHECK-NEXT: define <16 x i8> @OrV16I8(<16 x i8> %__0, <16 x i8> %__1) {
; CHECK-NEXT: __0:
; CHECK-NEXT:   %__2 = or <16 x i8> %__1, %__0
; CHECK-NEXT:   ret <16 x i8> %__2
; CHECK-NEXT: }

define <8 x i16> @OrV8I16(<8 x i16> %a, <8 x i16> %b) {
  %or = or <8 x i16> %b, %a
  ret <8 x i16> %or
}

; CHECK-NEXT: define <8 x i16> @OrV8I16(<8 x i16> %__0, <8 x i16> %__1) {
; CHECK-NEXT: __0:
; CHECK-NEXT:   %__2 = or <8 x i16> %__1, %__0
; CHECK-NEXT:   ret <8 x i16> %__2
; CHECK-NEXT: }

define <4 x i32> @OrV4I32(<4 x i32> %a, <4 x i32> %b) {
  %or = or <4 x i32> %b, %a
  ret <4 x i32> %or
}

; CHECK-NEXT: define <4 x i32> @OrV4I32(<4 x i32> %__0, <4 x i32> %__1) {
; CHECK-NEXT: __0:
; CHECK-NEXT:   %__2 = or <4 x i32> %__1, %__0
; CHECK-NEXT:   ret <4 x i32> %__2
; CHECK-NEXT: }

; TODO(kschimpf): xor i1/i8/i16. Needs bitcasts.

define i32 @XorI32(i32 %a, i32 %b) {
  %xor = xor i32 %b, %a
  ret i32 %xor
}

; CHECK-NEXT: define i32 @XorI32(i32 %__0, i32 %__1) {
; CHECK-NEXT: __0:
; CHECK-NEXT:   %__2 = xor i32 %__1, %__0
; CHECK-NEXT:   ret i32 %__2
; CHECK-NEXT: }

define i64 @XorI64(i64 %a, i64 %b) {
  %xor = xor i64 %b, %a
  ret i64 %xor
}

; CHECK-NEXT: define i64 @XorI64(i64 %__0, i64 %__1) {
; CHECK-NEXT: __0:
; CHECK-NEXT:   %__2 = xor i64 %__1, %__0
; CHECK-NEXT:   ret i64 %__2
; CHECK-NEXT: }

define <16 x i8> @XorV16I8(<16 x i8> %a, <16 x i8> %b) {
  %xor = xor <16 x i8> %b, %a
  ret <16 x i8> %xor
}

; CHECK-NEXT: define <16 x i8> @XorV16I8(<16 x i8> %__0, <16 x i8> %__1) {
; CHECK-NEXT: __0:
; CHECK-NEXT:   %__2 = xor <16 x i8> %__1, %__0
; CHECK-NEXT:   ret <16 x i8> %__2
; CHECK-NEXT: }

define <8 x i16> @XorV8I16(<8 x i16> %a, <8 x i16> %b) {
  %xor = xor <8 x i16> %b, %a
  ret <8 x i16> %xor
}

; CHECK-NEXT: define <8 x i16> @XorV8I16(<8 x i16> %__0, <8 x i16> %__1) {
; CHECK-NEXT: __0:
; CHECK-NEXT:   %__2 = xor <8 x i16> %__1, %__0
; CHECK-NEXT:   ret <8 x i16> %__2
; CHECK-NEXT: }

define <4 x i32> @XorV4I32(<4 x i32> %a, <4 x i32> %b) {
  %xor = xor <4 x i32> %b, %a
  ret <4 x i32> %xor
}

; CHECK-NEXT: define <4 x i32> @XorV4I32(<4 x i32> %__0, <4 x i32> %__1) {
; CHECK-NEXT: __0:
; CHECK-NEXT:   %__2 = xor <4 x i32> %__1, %__0
; CHECK-NEXT:   ret <4 x i32> %__2
; CHECK-NEXT: }

; TODO(kschimpf): shl i8/i16. Needs bitcasts.

define i32 @ShlI32(i32 %a, i32 %b) {
  %shl = shl i32 %b, %a
  ret i32 %shl
}

; CHECK-NEXT: define i32 @ShlI32(i32 %__0, i32 %__1) {
; CHECK-NEXT: __0:
; CHECK-NEXT:   %__2 = shl i32 %__1, %__0
; CHECK-NEXT:   ret i32 %__2
; CHECK-NEXT: }

define i64 @ShlI64(i64 %a, i64 %b) {
  %shl = shl i64 %b, %a
  ret i64 %shl
}

; CHECK-NEXT: define i64 @ShlI64(i64 %__0, i64 %__1) {
; CHECK-NEXT: __0:
; CHECK-NEXT:   %__2 = shl i64 %__1, %__0
; CHECK-NEXT:   ret i64 %__2
; CHECK-NEXT: }

define <16 x i8> @ShlV16I8(<16 x i8> %a, <16 x i8> %b) {
  %shl = shl <16 x i8> %b, %a
  ret <16 x i8> %shl
}

; CHECK-NEXT: define <16 x i8> @ShlV16I8(<16 x i8> %__0, <16 x i8> %__1) {
; CHECK-NEXT: __0:
; CHECK-NEXT:   %__2 = shl <16 x i8> %__1, %__0
; CHECK-NEXT:   ret <16 x i8> %__2
; CHECK-NEXT: }

define <8 x i16> @ShlV8I16(<8 x i16> %a, <8 x i16> %b) {
  %shl = shl <8 x i16> %b, %a
  ret <8 x i16> %shl
}

; CHECK-NEXT: define <8 x i16> @ShlV8I16(<8 x i16> %__0, <8 x i16> %__1) {
; CHECK-NEXT: __0:
; CHECK-NEXT:   %__2 = shl <8 x i16> %__1, %__0
; CHECK-NEXT:   ret <8 x i16> %__2
; CHECK-NEXT: }

define <4 x i32> @ShlV4I32(<4 x i32> %a, <4 x i32> %b) {
  %shl = shl <4 x i32> %b, %a
  ret <4 x i32> %shl
}

; CHECK-NEXT: define <4 x i32> @ShlV4I32(<4 x i32> %__0, <4 x i32> %__1) {
; CHECK-NEXT: __0:
; CHECK-NEXT:   %__2 = shl <4 x i32> %__1, %__0
; CHECK-NEXT:   ret <4 x i32> %__2
; CHECK-NEXT: }

; TODO(kschimpf): ashr i8/i16. Needs bitcasts.

define i32 @ashrI32(i32 %a, i32 %b) {
  %ashr = ashr i32 %b, %a
  ret i32 %ashr
}

; CHECK-NEXT: define i32 @ashrI32(i32 %__0, i32 %__1) {
; CHECK-NEXT: __0:
; CHECK-NEXT:   %__2 = ashr i32 %__1, %__0
; CHECK-NEXT:   ret i32 %__2
; CHECK-NEXT: }

define i64 @AshrI64(i64 %a, i64 %b) {
  %ashr = ashr i64 %b, %a
  ret i64 %ashr
}

; CHECK-NEXT: define i64 @AshrI64(i64 %__0, i64 %__1) {
; CHECK-NEXT: __0:
; CHECK-NEXT:   %__2 = ashr i64 %__1, %__0
; CHECK-NEXT:   ret i64 %__2
; CHECK-NEXT: }

define <16 x i8> @AshrV16I8(<16 x i8> %a, <16 x i8> %b) {
  %ashr = ashr <16 x i8> %b, %a
  ret <16 x i8> %ashr
}

; CHECK-NEXT: define <16 x i8> @AshrV16I8(<16 x i8> %__0, <16 x i8> %__1) {
; CHECK-NEXT: __0:
; CHECK-NEXT:   %__2 = ashr <16 x i8> %__1, %__0
; CHECK-NEXT:   ret <16 x i8> %__2
; CHECK-NEXT: }

define <8 x i16> @AshrV8I16(<8 x i16> %a, <8 x i16> %b) {
  %ashr = ashr <8 x i16> %b, %a
  ret <8 x i16> %ashr
}

; CHECK-NEXT: define <8 x i16> @AshrV8I16(<8 x i16> %__0, <8 x i16> %__1) {
; CHECK-NEXT: __0:
; CHECK-NEXT:   %__2 = ashr <8 x i16> %__1, %__0
; CHECK-NEXT:   ret <8 x i16> %__2
; CHECK-NEXT: }

define <4 x i32> @AshrV4I32(<4 x i32> %a, <4 x i32> %b) {
  %ashr = ashr <4 x i32> %b, %a
  ret <4 x i32> %ashr
}

; CHECK-NEXT: define <4 x i32> @AshrV4I32(<4 x i32> %__0, <4 x i32> %__1) {
; CHECK-NEXT: __0:
; CHECK-NEXT:   %__2 = ashr <4 x i32> %__1, %__0
; CHECK-NEXT:   ret <4 x i32> %__2
; CHECK-NEXT: }

; TODO(kschimpf): lshr i8/i16. Needs bitcasts.

define i32 @lshrI32(i32 %a, i32 %b) {
  %lshr = lshr i32 %b, %a
  ret i32 %lshr
}

; CHECK-NEXT: define i32 @lshrI32(i32 %__0, i32 %__1) {
; CHECK-NEXT: __0:
; CHECK-NEXT:   %__2 = lshr i32 %__1, %__0
; CHECK-NEXT:   ret i32 %__2
; CHECK-NEXT: }

define i64 @LshrI64(i64 %a, i64 %b) {
  %lshr = lshr i64 %b, %a
  ret i64 %lshr
}

; CHECK-NEXT: define i64 @LshrI64(i64 %__0, i64 %__1) {
; CHECK-NEXT: __0:
; CHECK-NEXT:   %__2 = lshr i64 %__1, %__0
; CHECK-NEXT:   ret i64 %__2
; CHECK-NEXT: }

define <16 x i8> @LshrV16I8(<16 x i8> %a, <16 x i8> %b) {
  %lshr = lshr <16 x i8> %b, %a
  ret <16 x i8> %lshr
}

; CHECK-NEXT: define <16 x i8> @LshrV16I8(<16 x i8> %__0, <16 x i8> %__1) {
; CHECK-NEXT: __0:
; CHECK-NEXT:   %__2 = lshr <16 x i8> %__1, %__0
; CHECK-NEXT:   ret <16 x i8> %__2
; CHECK-NEXT: }

define <8 x i16> @LshrV8I16(<8 x i16> %a, <8 x i16> %b) {
  %lshr = lshr <8 x i16> %b, %a
  ret <8 x i16> %lshr
}

; CHECK-NEXT: define <8 x i16> @LshrV8I16(<8 x i16> %__0, <8 x i16> %__1) {
; CHECK-NEXT: __0:
; CHECK-NEXT:   %__2 = lshr <8 x i16> %__1, %__0
; CHECK-NEXT:   ret <8 x i16> %__2
; CHECK-NEXT: }

define <4 x i32> @LshrV4I32(<4 x i32> %a, <4 x i32> %b) {
  %lshr = lshr <4 x i32> %b, %a
  ret <4 x i32> %lshr
}

; CHECK-NEXT: define <4 x i32> @LshrV4I32(<4 x i32> %__0, <4 x i32> %__1) {
; CHECK-NEXT: __0:
; CHECK-NEXT:   %__2 = lshr <4 x i32> %__1, %__0
; CHECK-NEXT:   ret <4 x i32> %__2
; CHECK-NEXT: }
