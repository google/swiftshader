; This tests the NaCl intrinsics not related to atomic operations.

; RUN: %llvm2ice -O2 --verbose none %s | FileCheck %s
; RUN: %llvm2ice -O2 --verbose none %s | FileCheck %s --check-prefix=CHECKO2REM
; RUN: %llvm2ice -Om1 --verbose none %s | FileCheck %s
; RUN: %llvm2ice --verbose none %s | FileCheck --check-prefix=ERRORS %s

; RUN: %llvm2iceinsts %s | %szdiff %s | FileCheck --check-prefix=DUMP %s
; RUN: %llvm2iceinsts --pnacl %s | %szdiff %s \
; RUN:                           | FileCheck --check-prefix=DUMP %s

declare i8* @llvm.nacl.read.tp()
declare void @llvm.memcpy.p0i8.p0i8.i32(i8*, i8*, i32, i32, i1)
declare void @llvm.memmove.p0i8.p0i8.i32(i8*, i8*, i32, i32, i1)
declare void @llvm.memset.p0i8.i32(i8*, i8, i32, i32, i1)
declare void @llvm.nacl.longjmp(i8*, i32)
declare i32 @llvm.nacl.setjmp(i8*)
declare float @llvm.sqrt.f32(float)
declare double @llvm.sqrt.f64(double)
declare void @llvm.trap()

define i32 @test_nacl_read_tp() {
entry:
  %ptr = call i8* @llvm.nacl.read.tp()
  %__1 = ptrtoint i8* %ptr to i32
  ret i32 %__1
}
; CHECK-LABEL: test_nacl_read_tp
; CHECK: mov e{{.*}}, dword ptr gs:[0]
; CHECKO2REM-LABEL: test_nacl_read_tp
; CHECKO2REM: mov e{{.*}}, dword ptr gs:[0]

define i32 @test_nacl_read_tp_more_addressing() {
entry:
  %ptr = call i8* @llvm.nacl.read.tp()
  %__1 = ptrtoint i8* %ptr to i32
  %x = add i32 %__1, %__1
  %__3 = inttoptr i32 %x to i32*
  %v = load i32* %__3, align 1
  %ptr2 = call i8* @llvm.nacl.read.tp()
  %__6 = ptrtoint i8* %ptr2 to i32
  %y = add i32 %__6, 4
  %__8 = inttoptr i32 %y to i32*
  store i32 %v, i32* %__8, align 1
  ret i32 %v
}
; CHECK-LABEL: test_nacl_read_tp_more_addressing
; CHECK: mov e{{.*}}, dword ptr gs:[0]
; CHECK: mov e{{.*}}, dword ptr gs:[0]
; CHECKO2REM-LABEL: test_nacl_read_tp_more_addressing
; CHECKO2REM: mov e{{.*}}, dword ptr gs:[0]
; CHECKO2REM: mov e{{.*}}, dword ptr gs:[0]

define i32 @test_nacl_read_tp_dead(i32 %a) {
entry:
  %ptr = call i8* @llvm.nacl.read.tp()
  ; Not actually using the result of nacl read tp call.
  ; In O2 mode this should be DCE'ed.
  ret i32 %a
}
; Consider nacl.read.tp side-effect free, so it can be eliminated.
; CHECKO2REM-LABEL: test_nacl_read_tp_dead
; CHECKO2REM-NOT: mov e{{.*}}, dword ptr gs:[0]

define void @test_memcpy(i32 %iptr_dst, i32 %iptr_src, i32 %len) {
entry:
  %dst = inttoptr i32 %iptr_dst to i8*
  %src = inttoptr i32 %iptr_src to i8*
  call void @llvm.memcpy.p0i8.p0i8.i32(i8* %dst, i8* %src,
                                       i32 %len, i32 1, i1 0)
  ret void
}
; CHECK-LABEL: test_memcpy
; CHECK: call memcpy

; TODO(jvoung) -- if we want to be clever, we can do this and the memmove,
; memset without a function call.
define void @test_memcpy_const_len_align(i32 %iptr_dst, i32 %iptr_src) {
entry:
  %dst = inttoptr i32 %iptr_dst to i8*
  %src = inttoptr i32 %iptr_src to i8*
  call void @llvm.memcpy.p0i8.p0i8.i32(i8* %dst, i8* %src,
                                       i32 8, i32 1, i1 0)
  ret void
}
; CHECK-LABEL: test_memcpy_const_len_align
; CHECK: call memcpy

define void @test_memmove(i32 %iptr_dst, i32 %iptr_src, i32 %len) {
entry:
  %dst = inttoptr i32 %iptr_dst to i8*
  %src = inttoptr i32 %iptr_src to i8*
  call void @llvm.memmove.p0i8.p0i8.i32(i8* %dst, i8* %src,
                                        i32 %len, i32 1, i1 0)
  ret void
}
; CHECK-LABEL: test_memmove
; CHECK: call memmove

define void @test_memmove_const_len_align(i32 %iptr_dst, i32 %iptr_src) {
entry:
  %dst = inttoptr i32 %iptr_dst to i8*
  %src = inttoptr i32 %iptr_src to i8*
  call void @llvm.memmove.p0i8.p0i8.i32(i8* %dst, i8* %src,
                                        i32 8, i32 1, i1 0)
  ret void
}
; CHECK-LABEL: test_memmove_const_len_align
; CHECK: call memmove

define void @test_memset(i32 %iptr_dst, i32 %wide_val, i32 %len) {
entry:
  %val = trunc i32 %wide_val to i8
  %dst = inttoptr i32 %iptr_dst to i8*
  call void @llvm.memset.p0i8.i32(i8* %dst, i8 %val,
                                  i32 %len, i32 1, i1 0)
  ret void
}
; CHECK-LABEL: test_memset
; CHECK: call memset

define void @test_memset_const_len_align(i32 %iptr_dst, i32 %wide_val) {
entry:
  %val = trunc i32 %wide_val to i8
  %dst = inttoptr i32 %iptr_dst to i8*
  call void @llvm.memset.p0i8.i32(i8* %dst, i8 %val,
                                  i32 8, i32 1, i1 0)
  ret void
}
; CHECK-LABEL: test_memset_const_len_align
; CHECK: call memset

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
; CHECK: call setjmp
; CHECK: call longjmp
; CHECKO2REM-LABEL: test_setjmplongjmp
; CHECKO2REM: call setjmp
; CHECKO2REM: call longjmp

define i32 @test_setjmp_unused(i32 %iptr_env, i32 %i_other) {
entry:
  %env = inttoptr i32 %iptr_env to i8*
  %i = call i32 @llvm.nacl.setjmp(i8* %env)
  ret i32 %i_other
}
; Don't consider setjmp side-effect free, so it's not eliminated if
; result unused.
; CHECKO2REM-LABEL: test_setjmp_unused
; CHECKO2REM: call setjmp

define float @test_sqrt_float(float %x, i32 %iptr) {
entry:
  %r = call float @llvm.sqrt.f32(float %x)
  %r2 = call float @llvm.sqrt.f32(float %r)
  %r3 = call float @llvm.sqrt.f32(float -0.0)
  %r4 = fadd float %r2, %r3
  br label %next

next:
  %__6 = inttoptr i32 %iptr to float*
  %y = load float* %__6, align 4
  %r5 = call float @llvm.sqrt.f32(float %y)
  %r6 = fadd float %r4, %r5
  ret float %r6
}
; CHECK-LABEL: test_sqrt_float
; CHECK: sqrtss xmm{{.*}}
; CHECK: sqrtss xmm{{.*}}
; CHECK: sqrtss xmm{{.*}}, dword ptr
; CHECK-LABEL: .L{{.*}}next
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
  br label %next

next:
  %__6 = inttoptr i32 %iptr to double*
  %y = load double* %__6, align 8
  %r5 = call double @llvm.sqrt.f64(double %y)
  %r6 = fadd double %r4, %r5
  ret double %r6
}
; CHECK-LABEL: test_sqrt_double
; CHECK: sqrtsd xmm{{.*}}
; CHECK: sqrtsd xmm{{.*}}
; CHECK: sqrtsd xmm{{.*}}, qword ptr
; CHECK-LABEL: .L{{.*}}next
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

; ERRORS-NOT: ICE translation error
; DUMP-NOT: SZ
