; Assembly test for simple arithmetic operations.

; RUN: %if --need=target_X8632 --command %p2i --filetype=obj --disassemble \
; RUN:   --target x8632 -i %s --args -O2 \
; RUN:   | %if --need=target_X8632 --command FileCheck %s

; TODO(jvoung): Stop skipping unimplemented parts (via --skip-unimplemented)
; once enough infrastructure is in. Also, switch to --filetype=obj
; when possible.
; RUN: %if --need=target_ARM32 --need=allow_dump \
; RUN:   --command %p2i --filetype=asm --assemble --disassemble --target arm32 \
; RUN:   -i %s --args -O2 --skip-unimplemented \
; RUN:   | %if --need=target_ARM32 --need=allow_dump \
; RUN:   --command FileCheck --check-prefix ARM32 %s
; RUN: %if --need=target_ARM32 --need=allow_dump \
; RUN:   --command %p2i --filetype=asm --assemble --disassemble --target arm32 \
; RUN:   -i %s --args -O2 --mattr=hwdiv-arm --skip-unimplemented \
; RUN:   | %if --need=target_ARM32 --need=allow_dump \
; RUN:   --command FileCheck --check-prefix ARM32HWDIV %s
; RUN: %if --need=target_ARM32 --need=allow_dump \
; RUN:   --command %p2i --filetype=asm --assemble --disassemble --target arm32 \
; RUN:   -i %s --args -Om1 --skip-unimplemented \
; RUN:   | %if --need=target_ARM32 --need=allow_dump \
; RUN:   --command FileCheck --check-prefix ARM32 %s

define i32 @Add(i32 %a, i32 %b) {
entry:
  %add = add i32 %b, %a
  ret i32 %add
}
; CHECK-LABEL: Add
; CHECK: add e
; ARM32-LABEL: Add
; ARM32: add r

define i32 @And(i32 %a, i32 %b) {
entry:
  %and = and i32 %b, %a
  ret i32 %and
}
; CHECK-LABEL: And
; CHECK: and e
; ARM32-LABEL: And
; ARM32: and r

define i32 @Or(i32 %a, i32 %b) {
entry:
  %or = or i32 %b, %a
  ret i32 %or
}
; CHECK-LABEL: Or
; CHECK: or e
; ARM32-LABEL: Or
; ARM32: orr r

define i32 @Xor(i32 %a, i32 %b) {
entry:
  %xor = xor i32 %b, %a
  ret i32 %xor
}
; CHECK-LABEL: Xor
; CHECK: xor e
; ARM32-LABEL: Xor
; ARM32: eor r

define i32 @Sub(i32 %a, i32 %b) {
entry:
  %sub = sub i32 %a, %b
  ret i32 %sub
}
; CHECK-LABEL: Sub
; CHECK: sub e
; ARM32-LABEL: Sub
; ARM32: sub r

define i32 @Mul(i32 %a, i32 %b) {
entry:
  %mul = mul i32 %b, %a
  ret i32 %mul
}
; CHECK-LABEL: Mul
; CHECK: imul e
; ARM32-LABEL: Mul
; ARM32: mul r

; Check for a valid ARM mul instruction where operands have to be registers.
; On the other hand x86-32 does allow an immediate.
define i32 @MulImm(i32 %a, i32 %b) {
entry:
  %mul = mul i32 %a, 99
  ret i32 %mul
}
; CHECK-LABEL: MulImm
; CHECK: imul e{{.*}},e{{.*}},0x63
; ARM32-LABEL: MulImm
; ARM32: mov {{.*}}, #99
; ARM32: mul r{{.*}}, r{{.*}}, r{{.*}}

; Check for a valid addressing mode in the x86-32 mul instruction when
; the second source operand is an immediate.
define i64 @MulImm64(i64 %a) {
entry:
  %mul = mul i64 %a, 99
  ret i64 %mul
}
; NOTE: the lowering is currently a bit inefficient for small 64-bit constants.
; The top bits of the immediate are 0, but the instructions modeling that
; multiply by 0 are not eliminated (see expanded 64-bit ARM lowering).
; CHECK-LABEL: MulImm64
; CHECK: mov {{.*}},0x63
; CHECK: mov {{.*}},0x0
; CHECK-NOT: mul {{[0-9]+}}
;
; ARM32-LABEL: MulImm64
; ARM32: movw {{.*}}, #99
; ARM32: movw {{.*}}, #0
; ARM32: mul r
; ARM32: mla r
; ARM32: umull r
; ARM32: add r

define i32 @Sdiv(i32 %a, i32 %b) {
entry:
  %div = sdiv i32 %a, %b
  ret i32 %div
}
; CHECK-LABEL: Sdiv
; CHECK: cdq
; CHECK: idiv e
;
; ARM32-LABEL: Sdiv
; ARM32: tst [[DENOM:r.*]], [[DENOM]]
; ARM32: bne
; ARM32: .word 0xe7fedef0
; ARM32: {{.*}} bl {{.*}} __divsi3
; ARM32HWDIV-LABEL: Sdiv
; ARM32HWDIV: tst
; ARM32HWDIV: bne
; ARM32HWDIV: sdiv

define i32 @SdivConst(i32 %a) {
entry:
  %div = sdiv i32 %a, 219
  ret i32 %div
}
; CHECK-LABEL: SdivConst
; CHECK: cdq
; CHECK: idiv e
;
; ARM32-LABEL: SdivConst
; ARM32-NOT: tst
; ARM32: bl {{.*}} __divsi3
; ARM32HWDIV-LABEL: SdivConst
; ARM32HWDIV-NOT: tst
; ARM32HWDIV: sdiv

define i32 @Srem(i32 %a, i32 %b) {
entry:
  %rem = srem i32 %a, %b
  ret i32 %rem
}
; CHECK-LABEL: Srem
; CHECK: cdq
; CHECK: idiv e
;
; ARM32-LABEL: Srem
; ARM32: tst [[DENOM:r.*]], [[DENOM]]
; ARM32: bne
; ARM32: bl {{.*}} __modsi3
; ARM32HWDIV-LABEL: Srem
; ARM32HWDIV: tst
; ARM32HWDIV: bne
; ARM32HWDIV: sdiv
; ARM32HWDIV: mls

define i32 @Udiv(i32 %a, i32 %b) {
entry:
  %div = udiv i32 %a, %b
  ret i32 %div
}
; CHECK-LABEL: Udiv
; CHECK: div e
;
; ARM32-LABEL: Udiv
; ARM32: tst [[DENOM:r.*]], [[DENOM]]
; ARM32: bne
; ARM32: bl {{.*}} __udivsi3
; ARM32HWDIV-LABEL: Udiv
; ARM32HWDIV: tst
; ARM32HWDIV: bne
; ARM32HWDIV: udiv

define i32 @Urem(i32 %a, i32 %b) {
entry:
  %rem = urem i32 %a, %b
  ret i32 %rem
}
; CHECK-LABEL: Urem
; CHECK: div e
;
; ARM32-LABEL: Urem
; ARM32: tst [[DENOM:r.*]], [[DENOM]]
; ARM32: bne
; ARM32: bl {{.*}} __umodsi3
; ARM32HWDIV-LABEL: Urem
; ARM32HWDIV: tst
; ARM32HWDIV: bne
; ARM32HWDIV: udiv
; ARM32HWDIV: mls
