; This tests the NaCl intrinsics not related to atomic operations.

; RUN: %p2i -i %s --filetype=obj --disassemble --args -O2 -sandbox \
; RUN:   | FileCheck %s
; RUN: %p2i -i %s --filetype=obj --disassemble --args -Om1 -sandbox \
; RUN:   | FileCheck %s

; Do another run w/ O2 and a different check-prefix (otherwise O2 and Om1
; share the same "CHECK" prefix). This separate run helps check that
; some code is optimized out.
; RUN: %p2i -i %s --filetype=obj --disassemble --args -O2 -sandbox \
; RUN:   | FileCheck --check-prefix=CHECKO2REM %s

; Do O2 runs without -sandbox to make sure llvm.nacl.read.tp gets
; lowered to __nacl_read_tp instead of gs:0x0.
; We also know that because it's O2, it'll have the O2REM optimizations.
; RUN: %p2i -i %s --filetype=obj --disassemble --args -O2 \
; RUN:   | FileCheck --check-prefix=CHECKO2UNSANDBOXEDREM %s

declare i8* @llvm.nacl.read.tp()
declare void @llvm.memcpy.p0i8.p0i8.i32(i8*, i8*, i32, i32, i1)
declare void @llvm.memmove.p0i8.p0i8.i32(i8*, i8*, i32, i32, i1)
declare void @llvm.memset.p0i8.i32(i8*, i8, i32, i32, i1)
declare void @llvm.nacl.longjmp(i8*, i32)
declare i32 @llvm.nacl.setjmp(i8*)
declare float @llvm.sqrt.f32(float)
declare double @llvm.sqrt.f64(double)
declare float @llvm.fabs.f32(float)
declare double @llvm.fabs.f64(double)
declare <4 x float> @llvm.fabs.v4f32(<4 x float>)
declare void @llvm.trap()
declare i16 @llvm.bswap.i16(i16)
declare i32 @llvm.bswap.i32(i32)
declare i64 @llvm.bswap.i64(i64)
declare i32 @llvm.ctlz.i32(i32, i1)
declare i64 @llvm.ctlz.i64(i64, i1)
declare i32 @llvm.cttz.i32(i32, i1)
declare i64 @llvm.cttz.i64(i64, i1)
declare i32 @llvm.ctpop.i32(i32)
declare i64 @llvm.ctpop.i64(i64)
declare i8* @llvm.stacksave()
declare void @llvm.stackrestore(i8*)

define i32 @test_nacl_read_tp() {
entry:
  %ptr = call i8* @llvm.nacl.read.tp()
  %__1 = ptrtoint i8* %ptr to i32
  ret i32 %__1
}
; CHECK-LABEL: test_nacl_read_tp
; CHECK: mov e{{.*}},DWORD PTR gs:0x0
; CHECKO2REM-LABEL: test_nacl_read_tp
; CHECKO2REM: mov e{{.*}},DWORD PTR gs:0x0
; CHECKO2UNSANDBOXEDREM-LABEL: test_nacl_read_tp
; CHECKO2UNSANDBOXEDREM: call {{.*}} R_{{.*}} __nacl_read_tp

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
  %y = add i32 %__6, 4
  %__8 = inttoptr i32 %y to i32*
  %v_add2 = add i32 %v, 4
  store i32 %v_add2, i32* %__8, align 1
  ret i32 %v
}
; CHECK-LABEL: test_nacl_read_tp_more_addressing
; CHECK: mov e{{.*}},DWORD PTR gs:0x0
; CHECK: mov e{{.*}},DWORD PTR gs:0x0
; CHECKO2REM-LABEL: test_nacl_read_tp_more_addressing
; CHECKO2REM: mov e{{.*}},DWORD PTR gs:0x0
; CHECKO2REM: mov e{{.*}},DWORD PTR gs:0x0
; CHECKO2UNSANDBOXEDREM-LABEL: test_nacl_read_tp_more_addressing
; CHECKO2UNSANDBOXEDREM: call {{.*}} R_{{.*}} __nacl_read_tp
; CHECKO2UNSANDBOXEDREM: call {{.*}} R_{{.*}} __nacl_read_tp

define i32 @test_nacl_read_tp_dead(i32 %a) {
entry:
  %ptr = call i8* @llvm.nacl.read.tp()
  ; Not actually using the result of nacl read tp call.
  ; In O2 mode this should be DCE'ed.
  ret i32 %a
}
; Consider nacl.read.tp side-effect free, so it can be eliminated.
; CHECKO2REM-LABEL: test_nacl_read_tp_dead
; CHECKO2REM-NOT: mov e{{.*}}, DWORD PTR gs:0x0
; CHECKO2UNSANDBOXEDREM-LABEL: test_nacl_read_tp_dead
; CHECKO2UNSANDBOXEDREM-NOT: call {{.*}} R_{{.*}} __nacl_read_tp

define void @test_memcpy(i32 %iptr_dst, i32 %iptr_src, i32 %len) {
entry:
  %dst = inttoptr i32 %iptr_dst to i8*
  %src = inttoptr i32 %iptr_src to i8*
  call void @llvm.memcpy.p0i8.p0i8.i32(i8* %dst, i8* %src,
                                       i32 %len, i32 1, i1 false)
  ret void
}
; CHECK-LABEL: test_memcpy
; CHECK: call {{.*}} R_{{.*}} memcpy
; CHECKO2REM-LABEL: test_memcpy
; CHECKO2UNSANDBOXEDREM-LABEL: test_memcpy

; TODO(jvoung) -- if we want to be clever, we can do this and the memmove,
; memset without a function call.
define void @test_memcpy_const_len_align(i32 %iptr_dst, i32 %iptr_src) {
entry:
  %dst = inttoptr i32 %iptr_dst to i8*
  %src = inttoptr i32 %iptr_src to i8*
  call void @llvm.memcpy.p0i8.p0i8.i32(i8* %dst, i8* %src,
                                       i32 8, i32 1, i1 false)
  ret void
}
; CHECK-LABEL: test_memcpy_const_len_align
; CHECK: call {{.*}} R_{{.*}} memcpy

define void @test_memmove(i32 %iptr_dst, i32 %iptr_src, i32 %len) {
entry:
  %dst = inttoptr i32 %iptr_dst to i8*
  %src = inttoptr i32 %iptr_src to i8*
  call void @llvm.memmove.p0i8.p0i8.i32(i8* %dst, i8* %src,
                                        i32 %len, i32 1, i1 false)
  ret void
}
; CHECK-LABEL: test_memmove
; CHECK: call {{.*}} R_{{.*}} memmove

define void @test_memmove_const_len_align(i32 %iptr_dst, i32 %iptr_src) {
entry:
  %dst = inttoptr i32 %iptr_dst to i8*
  %src = inttoptr i32 %iptr_src to i8*
  call void @llvm.memmove.p0i8.p0i8.i32(i8* %dst, i8* %src,
                                        i32 8, i32 1, i1 false)
  ret void
}
; CHECK-LABEL: test_memmove_const_len_align
; CHECK: call {{.*}} R_{{.*}} memmove

define void @test_memset(i32 %iptr_dst, i32 %wide_val, i32 %len) {
entry:
  %val = trunc i32 %wide_val to i8
  %dst = inttoptr i32 %iptr_dst to i8*
  call void @llvm.memset.p0i8.i32(i8* %dst, i8 %val,
                                  i32 %len, i32 1, i1 false)
  ret void
}
; CHECK-LABEL: test_memset
; CHECK: movzx
; CHECK: call {{.*}} R_{{.*}} memset

define void @test_memset_const_len_align(i32 %iptr_dst, i32 %wide_val) {
entry:
  %val = trunc i32 %wide_val to i8
  %dst = inttoptr i32 %iptr_dst to i8*
  call void @llvm.memset.p0i8.i32(i8* %dst, i8 %val,
                                  i32 8, i32 1, i1 false)
  ret void
}
; CHECK-LABEL: test_memset_const_len_align
; CHECK: movzx
; CHECK: call {{.*}} R_{{.*}} memset

define void @test_memset_const_val(i32 %iptr_dst, i32 %len) {
entry:
  %dst = inttoptr i32 %iptr_dst to i8*
  call void @llvm.memset.p0i8.i32(i8* %dst, i8 0, i32 %len, i32 1, i1 false)
  ret void
}
; CHECK-LABEL: test_memset_const_val
; Make sure the argument is legalized (can't movzx reg, 0).
; CHECK: movzx {{.*}},{{[^0]}}
; CHECK: call {{.*}} R_{{.*}} memset


define i32 @test_setjmplongjmp(i32 %iptr_env) {
entry:
  %env = inttoptr i32 %iptr_env to i8*
  %i = call i32 @llvm.nacl.setjmp(i8* %env)
  %r1 = icmp eq i32 %i, 0
  br i1 %r1, label %Zero, label %NonZero
Zero:
  ; Redundant inttoptr, to make --pnacl cast-eliding/re-insertion happy.
  %env2 = inttoptr i32 %iptr_env to i8*
  call void @llvm.nacl.longjmp(i8* %env2, i32 1)
  ret i32 0
NonZero:
  ret i32 1
}
; CHECK-LABEL: test_setjmplongjmp
; CHECK: call {{.*}} R_{{.*}} setjmp
; CHECK: call {{.*}} R_{{.*}} longjmp
; CHECKO2REM-LABEL: test_setjmplongjmp
; CHECKO2REM: call {{.*}} R_{{.*}} setjmp
; CHECKO2REM: call {{.*}} R_{{.*}} longjmp

define i32 @test_setjmp_unused(i32 %iptr_env, i32 %i_other) {
entry:
  %env = inttoptr i32 %iptr_env to i8*
  %i = call i32 @llvm.nacl.setjmp(i8* %env)
  ret i32 %i_other
}
; Don't consider setjmp side-effect free, so it's not eliminated if
; result unused.
; CHECKO2REM-LABEL: test_setjmp_unused
; CHECKO2REM: call {{.*}} R_{{.*}} setjmp

define float @test_sqrt_float(float %x, i32 %iptr) {
entry:
  %r = call float @llvm.sqrt.f32(float %x)
  %r2 = call float @llvm.sqrt.f32(float %r)
  %r3 = call float @llvm.sqrt.f32(float -0.0)
  %r4 = fadd float %r2, %r3
  ret float %r4
}
; CHECK-LABEL: test_sqrt_float
; CHECK: sqrtss xmm{{.*}}
; CHECK: sqrtss xmm{{.*}}
; CHECK: sqrtss xmm{{.*}},DWORD PTR

define float @test_sqrt_float_mergeable_load(float %x, i32 %iptr) {
entry:
  %__2 = inttoptr i32 %iptr to float*
  %y = load float* %__2, align 4
  %r5 = call float @llvm.sqrt.f32(float %y)
  %r6 = fadd float %x, %r5
  ret float %r6
}
; CHECK-LABEL: test_sqrt_float_mergeable_load
; We could fold the load and the sqrt into one operation, but the
; current folding only handles load + arithmetic op. The sqrt inst
; is considered an intrinsic call and not an arithmetic op.
; CHECK: sqrtss xmm{{.*}}

define double @test_sqrt_double(double %x, i32 %iptr) {
entry:
  %r = call double @llvm.sqrt.f64(double %x)
  %r2 = call double @llvm.sqrt.f64(double %r)
  %r3 = call double @llvm.sqrt.f64(double -0.0)
  %r4 = fadd double %r2, %r3
  ret double %r4
}
; CHECK-LABEL: test_sqrt_double
; CHECK: sqrtsd xmm{{.*}}
; CHECK: sqrtsd xmm{{.*}}
; CHECK: sqrtsd xmm{{.*}},QWORD PTR

define double @test_sqrt_double_mergeable_load(double %x, i32 %iptr) {
entry:
  %__2 = inttoptr i32 %iptr to double*
  %y = load double* %__2, align 8
  %r5 = call double @llvm.sqrt.f64(double %y)
  %r6 = fadd double %x, %r5
  ret double %r6
}
; CHECK-LABEL: test_sqrt_double_mergeable_load
; CHECK: sqrtsd xmm{{.*}}

define float @test_sqrt_ignored(float %x, double %y) {
entry:
  %ignored1 = call float @llvm.sqrt.f32(float %x)
  %ignored2 = call double @llvm.sqrt.f64(double %y)
  ret float 0.0
}
; CHECKO2REM-LABEL: test_sqrt_ignored
; CHECKO2REM-NOT: sqrtss
; CHECKO2REM-NOT: sqrtsd

define float @test_fabs_float(float %x) {
entry:
  %r = call float @llvm.fabs.f32(float %x)
  %r2 = call float @llvm.fabs.f32(float %r)
  %r3 = call float @llvm.fabs.f32(float -0.0)
  %r4 = fadd float %r2, %r3
  ret float %r4
}
; CHECK-LABEL: test_fabs_float
; CHECK: pcmpeqd
; CHECK: psrld
; CHECK: pand
; CHECK: pcmpeqd
; CHECK: psrld
; CHECK: pand
; CHECK: pcmpeqd
; CHECK: psrld
; CHECK: pand

define double @test_fabs_double(double %x) {
entry:
  %r = call double @llvm.fabs.f64(double %x)
  %r2 = call double @llvm.fabs.f64(double %r)
  %r3 = call double @llvm.fabs.f64(double -0.0)
  %r4 = fadd double %r2, %r3
  ret double %r4
}
; CHECK-LABEL: test_fabs_double
; CHECK: pcmpeqd
; CHECK: psrlq
; CHECK: pand
; CHECK: pcmpeqd
; CHECK: psrlq
; CHECK: pand
; CHECK: pcmpeqd
; CHECK: psrlq
; CHECK: pand

define <4 x float> @test_fabs_v4f32(<4 x float> %x) {
entry:
  %r = call <4 x float> @llvm.fabs.v4f32(<4 x float> %x)
  %r2 = call <4 x float> @llvm.fabs.v4f32(<4 x float> %r)
  %r3 = call <4 x float> @llvm.fabs.v4f32(<4 x float> undef)
  %r4 = fadd <4 x float> %r2, %r3
  ret <4 x float> %r4
}
; CHECK-LABEL: test_fabs_v4f32
; CHECK: pcmpeqd
; CHECK: psrld
; CHECK: pand
; CHECK: pcmpeqd
; CHECK: psrld
; CHECK: pand
; CHECK: pcmpeqd
; CHECK: psrld
; CHECK: pand

define i32 @test_trap(i32 %br) {
entry:
  %r1 = icmp eq i32 %br, 0
  br i1 %r1, label %Zero, label %NonZero
Zero:
  call void @llvm.trap()
  unreachable
NonZero:
  ret i32 1
}
; CHECK-LABEL: test_trap
; CHECK: ud2

define i32 @test_bswap_16(i32 %x) {
entry:
  %x_trunc = trunc i32 %x to i16
  %r = call i16 @llvm.bswap.i16(i16 %x_trunc)
  %r_zext = zext i16 %r to i32
  ret i32 %r_zext
}
; CHECK-LABEL: test_bswap_16
; Make sure this is the right operand size so that the most significant bit
; to least significant bit rotation happens at the right boundary.
; CHECK: rol {{[abcd]x|si|di|bp|word ptr}},0x8

define i32 @test_bswap_32(i32 %x) {
entry:
  %r = call i32 @llvm.bswap.i32(i32 %x)
  ret i32 %r
}
; CHECK-LABEL: test_bswap_32
; CHECK: bswap e{{.*}}

define i64 @test_bswap_64(i64 %x) {
entry:
  %r = call i64 @llvm.bswap.i64(i64 %x)
  ret i64 %r
}
; CHECK-LABEL: test_bswap_64
; CHECK: bswap e{{.*}}
; CHECK: bswap e{{.*}}

define i32 @test_ctlz_32(i32 %x) {
entry:
  %r = call i32 @llvm.ctlz.i32(i32 %x, i1 false)
  ret i32 %r
}
; CHECK-LABEL: test_ctlz_32
; TODO(jvoung): If we detect that LZCNT is supported, then use that
; and avoid the need to do the cmovne and xor stuff to guarantee that
; the result is well-defined w/ input == 0.
; CHECK: bsr [[REG_TMP:e.*]],{{.*}}
; CHECK: mov [[REG_RES:e.*]],0x3f
; CHECK: cmovne [[REG_RES]],[[REG_TMP]]
; CHECK: xor [[REG_RES]],0x1f

define i32 @test_ctlz_32_const() {
entry:
  %r = call i32 @llvm.ctlz.i32(i32 123456, i1 false)
  ret i32 %r
}
; Could potentially constant fold this, but the front-end should have done that.
; The dest operand must be a register and the source operand must be a register
; or memory.
; CHECK-LABEL: test_ctlz_32_const
; CHECK: bsr e{{.*}},{{.*}}e{{.*}}

define i32 @test_ctlz_32_ignored(i32 %x) {
entry:
  %ignored = call i32 @llvm.ctlz.i32(i32 %x, i1 false)
  ret i32 1
}
; CHECKO2REM-LABEL: test_ctlz_32_ignored
; CHECKO2REM-NOT: bsr

define i64 @test_ctlz_64(i64 %x) {
entry:
  %r = call i64 @llvm.ctlz.i64(i64 %x, i1 false)
  ret i64 %r
}
; CHECKO2REM-LABEL: test_ctlz_64
; CHECK-LABEL: test_ctlz_64
; CHECK: bsr [[REG_TMP1:e.*]],{{.*}}
; CHECK: mov [[REG_RES1:e.*]],0x3f
; CHECK: cmovne [[REG_RES1]],[[REG_TMP1]]
; CHECK: xor [[REG_RES1]],0x1f
; CHECK: add [[REG_RES1]],0x20
; CHECK: bsr [[REG_RES2:e.*]],{{.*}}
; CHECK: xor [[REG_RES2]],0x1f
; CHECK: test [[REG_UPPER:.*]],[[REG_UPPER]]
; CHECK: cmove [[REG_RES2]],[[REG_RES1]]
; CHECK: mov {{.*}},0x0

define i32 @test_ctlz_64_const(i64 %x) {
entry:
  %r = call i64 @llvm.ctlz.i64(i64 123456789012, i1 false)
  %r2 = trunc i64 %r to i32
  ret i32 %r2
}
; CHECK-LABEL: test_ctlz_64_const
; CHECK: bsr e{{.*}},{{.*}}e{{.*}}
; CHECK: bsr e{{.*}},{{.*}}e{{.*}}


define i32 @test_ctlz_64_ignored(i64 %x) {
entry:
  %ignored = call i64 @llvm.ctlz.i64(i64 1234567890, i1 false)
  ret i32 2
}
; CHECKO2REM-LABEL: test_ctlz_64_ignored
; CHECKO2REM-NOT: bsr

define i32 @test_cttz_32(i32 %x) {
entry:
  %r = call i32 @llvm.cttz.i32(i32 %x, i1 false)
  ret i32 %r
}
; CHECK-LABEL: test_cttz_32
; CHECK: bsf [[REG_IF_NOTZERO:e.*]],{{.*}}
; CHECK: mov [[REG_IF_ZERO:e.*]],0x20
; CHECK: cmovne [[REG_IF_ZERO]],[[REG_IF_NOTZERO]]

define i64 @test_cttz_64(i64 %x) {
entry:
  %r = call i64 @llvm.cttz.i64(i64 %x, i1 false)
  ret i64 %r
}
; CHECK-LABEL: test_cttz_64
; CHECK: bsf [[REG_IF_NOTZERO:e.*]],{{.*}}
; CHECK: mov [[REG_RES1:e.*]],0x20
; CHECK: cmovne [[REG_RES1]],[[REG_IF_NOTZERO]]
; CHECK: add [[REG_RES1]],0x20
; CHECK: bsf [[REG_RES2:e.*]],[[REG_LOWER:.*]]
; CHECK: test [[REG_LOWER]],[[REG_LOWER]]
; CHECK: cmove [[REG_RES2]],[[REG_RES1]]
; CHECK: mov {{.*}},0x0

define i32 @test_popcount_32(i32 %x) {
entry:
  %r = call i32 @llvm.ctpop.i32(i32 %x)
  ret i32 %r
}
; CHECK-LABEL: test_popcount_32
; CHECK: call {{.*}} R_{{.*}} __popcountsi2

define i64 @test_popcount_64(i64 %x) {
entry:
  %r = call i64 @llvm.ctpop.i64(i64 %x)
  ret i64 %r
}
; CHECK-LABEL: test_popcount_64
; CHECK: call {{.*}} R_{{.*}} __popcountdi2
; __popcountdi2 only returns a 32-bit result, so clear the upper bits of
; the return value just in case.
; CHECK: mov {{.*}},0x0


define i32 @test_popcount_64_ret_i32(i64 %x) {
entry:
  %r_i64 = call i64 @llvm.ctpop.i64(i64 %x)
  %r = trunc i64 %r_i64 to i32
  ret i32 %r
}
; If there is a trunc, then the mov {{.*}}, 0 is dead and gets optimized out.
; CHECKO2REM-LABEL: test_popcount_64_ret_i32
; CHECKO2REM: call {{.*}} R_{{.*}} __popcountdi2
; CHECKO2REM-NOT: mov {{.*}}, 0

define void @test_stacksave_noalloca() {
entry:
  %sp = call i8* @llvm.stacksave()
  call void @llvm.stackrestore(i8* %sp)
  ret void
}
; CHECK-LABEL: test_stacksave_noalloca
; CHECK: mov {{.*}},esp
; CHECK: mov esp,{{.*}}

declare i32 @foo(i32 %x)

define void @test_stacksave_multiple(i32 %x) {
entry:
  %x_4 = mul i32 %x, 4
  %sp1 = call i8* @llvm.stacksave()
  %tmp1 = alloca i8, i32 %x_4, align 4

  %sp2 = call i8* @llvm.stacksave()
  %tmp2 = alloca i8, i32 %x_4, align 4

  %y = call i32 @foo(i32 %x)

  %sp3 = call i8* @llvm.stacksave()
  %tmp3 = alloca i8, i32 %x_4, align 4

  %__9 = bitcast i8* %tmp1 to i32*
  store i32 %y, i32* %__9, align 1

  %__10 = bitcast i8* %tmp2 to i32*
  store i32 %x, i32* %__10, align 1

  %__11 = bitcast i8* %tmp3 to i32*
  store i32 %x, i32* %__11, align 1

  call void @llvm.stackrestore(i8* %sp1)
  ret void
}
; CHECK-LABEL: test_stacksave_multiple
; At least 3 copies of esp, but probably more from having to do the allocas.
; CHECK: mov {{.*}},esp
; CHECK: mov {{.*}},esp
; CHECK: mov {{.*}},esp
; CHECK: mov esp,{{.*}}
