; Tests various aspects of x86 opcode encodings. E.g., some opcodes like
; those for pmull vary more wildly depending on operand size (rather than
; follow a usual pattern).

; RUN: %p2i -i %s --args -O2 -mattr=sse4.1 --verbose none \
; RUN:   | llvm-mc -triple=i686-none-nacl -x86-asm-syntax=intel -filetype=obj \
; RUN:   | llvm-objdump -d --symbolize -x86-asm-syntax=intel - | FileCheck %s
; RUN: %p2i -i %s --args --verbose none | FileCheck --check-prefix=ERRORS %s
; RUN: %p2i -i %s --insts | %szdiff %s | FileCheck --check-prefix=DUMP %s

define <8 x i16> @test_mul_v8i16(<8 x i16> %arg0, <8 x i16> %arg1) {
entry:
  %res = mul <8 x i16> %arg0, %arg1
  ret <8 x i16> %res
; CHECK-LABEL: test_mul_v8i16
; CHECK: 66 0f d5 c1 pmullw xmm0, xmm1
}

; Test register and address mode encoding.
define <8 x i16> @test_mul_v8i16_more_regs(<8 x i1> %cond, <8 x i16> %arg0, <8 x i16> %arg1, <8 x i16> %arg2, <8 x i16> %arg3, <8 x i16> %arg4, <8 x i16> %arg5, <8 x i16> %arg6, <8 x i16> %arg7, <8 x i16> %arg8) {
entry:
  %res1 = mul <8 x i16> %arg0, %arg1
  %res2 = mul <8 x i16> %arg0, %arg2
  %res3 = mul <8 x i16> %arg0, %arg3
  %res4 = mul <8 x i16> %arg0, %arg4
  %res5 = mul <8 x i16> %arg0, %arg5
  %res6 = mul <8 x i16> %arg0, %arg6
  %res7 = mul <8 x i16> %arg0, %arg7
  %res8 = mul <8 x i16> %arg0, %arg8
  %res_acc1 = select <8 x i1> %cond, <8 x i16> %res1, <8 x i16> %res2
  %res_acc2 = select <8 x i1> %cond, <8 x i16> %res3, <8 x i16> %res4
  %res_acc3 = select <8 x i1> %cond, <8 x i16> %res5, <8 x i16> %res6
  %res_acc4 = select <8 x i1> %cond, <8 x i16> %res7, <8 x i16> %res8
  %res_acc1_3 = select <8 x i1> %cond, <8 x i16> %res_acc1, <8 x i16> %res_acc3
  %res_acc2_4 = select <8 x i1> %cond, <8 x i16> %res_acc2, <8 x i16> %res_acc4
  %res = select <8 x i1> %cond, <8 x i16> %res_acc1_3, <8 x i16> %res_acc2_4
  ret <8 x i16> %res
; CHECK-LABEL: test_mul_v8i16_more_regs
; CHECK-DAG: 66 0f d5 c2 pmullw xmm0, xmm2
; CHECK-DAG: 66 0f d5 c3 pmullw xmm0, xmm3
; CHECK-DAG: 66 0f d5 c4 pmullw xmm0, xmm4
; CHECK-DAG: 66 0f d5 c5 pmullw xmm0, xmm5
; CHECK-DAG: 66 0f d5 c6 pmullw xmm0, xmm6
; CHECK-DAG: 66 0f d5 c7 pmullw xmm0, xmm7
; CHECK-DAG: 66 0f d5 44 24 70 pmullw xmm0, xmmword ptr [esp + 112]
; CHECK-DAG: 66 0f d5 8c 24 80 00 00 00 pmullw xmm1, xmmword ptr [esp + 128]
}

define <4 x i32> @test_mul_v4i32(<4 x i32> %arg0, <4 x i32> %arg1) {
entry:
  %res = mul <4 x i32> %arg0, %arg1
  ret <4 x i32> %res
; CHECK-LABEL: test_mul_v4i32
; CHECK: 66 0f 38 40 c1  pmulld  xmm0, xmm1
}

define <4 x i32> @test_mul_v4i32_more_regs(<4 x i1> %cond, <4 x i32> %arg0, <4 x i32> %arg1, <4 x i32> %arg2, <4 x i32> %arg3, <4 x i32> %arg4, <4 x i32> %arg5, <4 x i32> %arg6, <4 x i32> %arg7, <4 x i32> %arg8) {
entry:
  %res1 = mul <4 x i32> %arg0, %arg1
  %res2 = mul <4 x i32> %arg0, %arg2
  %res3 = mul <4 x i32> %arg0, %arg3
  %res4 = mul <4 x i32> %arg0, %arg4
  %res5 = mul <4 x i32> %arg0, %arg5
  %res6 = mul <4 x i32> %arg0, %arg6
  %res7 = mul <4 x i32> %arg0, %arg7
  %res8 = mul <4 x i32> %arg0, %arg8
  %res_acc1 = select <4 x i1> %cond, <4 x i32> %res1, <4 x i32> %res2
  %res_acc2 = select <4 x i1> %cond, <4 x i32> %res3, <4 x i32> %res4
  %res_acc3 = select <4 x i1> %cond, <4 x i32> %res5, <4 x i32> %res6
  %res_acc4 = select <4 x i1> %cond, <4 x i32> %res7, <4 x i32> %res8
  %res_acc1_3 = select <4 x i1> %cond, <4 x i32> %res_acc1, <4 x i32> %res_acc3
  %res_acc2_4 = select <4 x i1> %cond, <4 x i32> %res_acc2, <4 x i32> %res_acc4
  %res = select <4 x i1> %cond, <4 x i32> %res_acc1_3, <4 x i32> %res_acc2_4
  ret <4 x i32> %res
; CHECK-LABEL: test_mul_v4i32_more_regs
; CHECK-DAG: 66 0f 38 40 c2 pmulld xmm0, xmm2
; CHECK-DAG: 66 0f 38 40 c3 pmulld xmm0, xmm3
; CHECK-DAG: 66 0f 38 40 c4 pmulld xmm0, xmm4
; CHECK-DAG: 66 0f 38 40 c5 pmulld xmm0, xmm5
; CHECK-DAG: 66 0f 38 40 c6 pmulld xmm0, xmm6
; CHECK-DAG: 66 0f 38 40 c7 pmulld xmm0, xmm7
; CHECK-DAG: 66 0f 38 40 44 24 70 pmulld xmm0, xmmword ptr [esp + 112]
; CHECK-DAG: 66 0f 38 40 8c 24 80 00 00 00 pmulld xmm1, xmmword ptr [esp + 128]
}

; ERRORS-NOT: ICE translation error
; DUMP-NOT: SZ
