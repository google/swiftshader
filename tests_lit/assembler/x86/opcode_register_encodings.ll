; Tests various aspects of x86 opcode encodings. E.g., some opcodes like
; those for pmull vary more wildly depending on operand size (rather than
; follow a usual pattern).

; RUN: %p2i -i %s --args -O2 -mattr=sse4.1 -sandbox --verbose none \
; RUN:   | llvm-mc -triple=i686-none-nacl -x86-asm-syntax=intel -filetype=obj \
; RUN:   | llvm-objdump -d --symbolize -x86-asm-syntax=intel - | FileCheck %s
; RUN: %p2i -i %s --args --verbose none | FileCheck --check-prefix=ERRORS %s

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

; Test movq, which is used by atomic stores.
declare void @llvm.nacl.atomic.store.i64(i64, i64*, i32)

define void @test_atomic_store_64(i32 %iptr, i32 %iptr2, i32 %iptr3, i64 %v) {
entry:
  %ptr = inttoptr i32 %iptr to i64*
  %ptr2 = inttoptr i32 %iptr2 to i64*
  %ptr3 = inttoptr i32 %iptr3 to i64*
  call void @llvm.nacl.atomic.store.i64(i64 %v, i64* %ptr2, i32 6)
  call void @llvm.nacl.atomic.store.i64(i64 1234567891024, i64* %ptr, i32 6)
  call void @llvm.nacl.atomic.store.i64(i64 %v, i64* %ptr3, i32 6)
  ret void
}
; CHECK-LABEL: test_atomic_store_64
; CHECK-DAG: f3 0f 7e 04 24    movq xmm0, qword ptr [esp]
; CHECK-DAG: f3 0f 7e 44 24 08 movq xmm0, qword ptr [esp + 8]
; CHECK-DAG: 66 0f d6 0{{.*}}  movq qword ptr [e{{.*}}], xmm0

; Test "movups" via vector stores and loads.
define void @store_v16xI8(i32 %addr, i32 %addr2, i32 %addr3, <16 x i8> %v) {
  %addr_v16xI8 = inttoptr i32 %addr to <16 x i8>*
  %addr2_v16xI8 = inttoptr i32 %addr2 to <16 x i8>*
  %addr3_v16xI8 = inttoptr i32 %addr3 to <16 x i8>*
  store <16 x i8> %v, <16 x i8>* %addr2_v16xI8, align 1
  store <16 x i8> %v, <16 x i8>* %addr_v16xI8, align 1
  store <16 x i8> %v, <16 x i8>* %addr3_v16xI8, align 1
  ret void
}
; CHECK-LABEL: store_v16xI8
; CHECK: 0f 11 0{{.*}} movups xmmword ptr [e{{.*}}], xmm0

define <16 x i8> @load_v16xI8(i32 %addr, i32 %addr2, i32 %addr3) {
  %addr_v16xI8 = inttoptr i32 %addr to <16 x i8>*
  %addr2_v16xI8 = inttoptr i32 %addr2 to <16 x i8>*
  %addr3_v16xI8 = inttoptr i32 %addr3 to <16 x i8>*
  %res1 = load <16 x i8>* %addr2_v16xI8, align 1
  %res2 = load <16 x i8>* %addr_v16xI8, align 1
  %res3 = load <16 x i8>* %addr3_v16xI8, align 1
  %res12 = add <16 x i8> %res1, %res2
  %res123 = add <16 x i8> %res12, %res3
  ret <16 x i8> %res123
}
; CHECK-LABEL: load_v16xI8
; CHECK: 0f 10 0{{.*}} movups xmm0, xmmword ptr [e{{.*}}]

; Test segment override prefix. This happens w/ nacl.read.tp.
declare i8* @llvm.nacl.read.tp()

; Also test more address complex operands via address-mode-optimization.
define i32 @test_nacl_read_tp_more_addressing() {
entry:
  %ptr = call i8* @llvm.nacl.read.tp()
  %__1 = ptrtoint i8* %ptr to i32
  %x = add i32 %__1, %__1
  %__3 = inttoptr i32 %x to i32*
  %v = load i32* %__3, align 1
  %v_add = add i32 %v, 1

  %ptr2 = call i8* @llvm.nacl.read.tp()
  %__6 = ptrtoint i8* %ptr2 to i32
  %y = add i32 %__6, -128
  %__8 = inttoptr i32 %y to i32*
  %v_add2 = add i32 %v, 4
  store i32 %v_add2, i32* %__8, align 1

  %z = add i32 %__6, 256
  %__9 = inttoptr i32 %z to i32*
  %v_add3 = add i32 %v, 91
  store i32 %v_add2, i32* %__9, align 1

  ret i32 %v
}
; CHECK-LABEL: test_nacl_read_tp_more_addressing
; CHECK: 65 8b 05 00 00 00 00  mov eax, dword ptr gs:[0]
; CHECK: 8b 04 00              mov eax, dword ptr [eax + eax]
; CHECK: 65 8b 0d 00 00 00 00  mov ecx, dword ptr gs:[0]
; CHECK: 89 51 80              mov dword ptr [ecx - 128], edx
; CHECK: 89 91 00 01 00 00     mov dword ptr [ecx + 256], edx

; ERRORS-NOT: ICE translation error
