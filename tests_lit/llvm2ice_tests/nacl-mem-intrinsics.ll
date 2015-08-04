; This tests the NaCl intrinsics memset, memcpy and memmove.

; RUN: %if --need=target_X8632 --command %p2i --filetype=obj --disassemble \
; RUN:   --target x8632 -i %s --args -O2 -sandbox \
; RUN:   | %if --need=target_X8632 --command FileCheck %s
; RUN: %if --need=target_X8632 --command %p2i --filetype=obj --disassemble \
; RUN:   --target x8632 -i %s --args -Om1 -sandbox \
; RUN:   | %if --need=target_X8632 --command FileCheck %s

; RUN: %if --need=target_ARM32 --need=allow_dump \
; RUN:   --command %p2i --filetype=asm --assemble --disassemble --target arm32 \
; RUN:   -i %s --args -O2 --skip-unimplemented \
; RUN:   | %if --need=target_ARM32 --need=allow_dump \
; RUN:   --command FileCheck --check-prefix ARM32 %s

declare void @llvm.memcpy.p0i8.p0i8.i32(i8*, i8*, i32, i32, i1)
declare void @llvm.memmove.p0i8.p0i8.i32(i8*, i8*, i32, i32, i1)
declare void @llvm.memset.p0i8.i32(i8*, i8, i32, i32, i1)

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
; ARM32-LABEL: test_memcpy
; ARM32: bl {{.*}} memcpy

; TODO(jvoung) -- if we want to be clever, we can do this and the memmove,
; memset without a function call.
define void @test_memcpy_const_len_align(i32 %iptr_dst, i32 %iptr_src) {
entry:
  %dst = inttoptr i32 %iptr_dst to i8*
  %src = inttoptr i32 %iptr_src to i8*
  call void @llvm.memcpy.p0i8.p0i8.i32(i8* %dst, i8* %src,
                                       i32 32, i32 1, i1 false)
  ret void
}
; CHECK-LABEL: test_memcpy_const_len_align
; CHECK: call {{.*}} R_{{.*}} memcpy
; ARM32-LABEL: test_memcpy_const_len_align
; ARM32: bl {{.*}} memcpy

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
; ARM32-LABEL: test_memmove
; ARM32: bl {{.*}} memmove

define void @test_memmove_const_len_align(i32 %iptr_dst, i32 %iptr_src) {
entry:
  %dst = inttoptr i32 %iptr_dst to i8*
  %src = inttoptr i32 %iptr_src to i8*
  call void @llvm.memmove.p0i8.p0i8.i32(i8* %dst, i8* %src,
                                        i32 32, i32 1, i1 false)
  ret void
}
; CHECK-LABEL: test_memmove_const_len_align
; CHECK: call {{.*}} R_{{.*}} memmove
; ARM32-LABEL: test_memmove_const_len_align
; ARM32: bl {{.*}} memmove

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
; ARM32-LABEL: test_memset
; ARM32: uxtb
; ARM32: bl {{.*}} memset

define void @test_memset_const_len_align(i32 %iptr_dst, i32 %wide_val) {
entry:
  %val = trunc i32 %wide_val to i8
  %dst = inttoptr i32 %iptr_dst to i8*
  call void @llvm.memset.p0i8.i32(i8* %dst, i8 %val,
                                  i32 32, i32 1, i1 false)
  ret void
}
; CHECK-LABEL: test_memset_const_len_align
; CHECK: movzx
; CHECK: call {{.*}} R_{{.*}} memset
; ARM32-LABEL: test_memset_const_len_align
; ARM32: uxtb
; ARM32: bl {{.*}} memset

define void @test_memset_const_val(i32 %iptr_dst, i32 %len) {
entry:
  %dst = inttoptr i32 %iptr_dst to i8*
  call void @llvm.memset.p0i8.i32(i8* %dst, i8 0, i32 %len, i32 1, i1 false)
  ret void
}
; CHECK-LABEL: test_memset_const_val
; CHECK-NOT: movzx
; CHECK: call {{.*}} R_{{.*}} memset
; ARM32-LABEL: test_memset_const_val
; ARM32: uxtb
; ARM32: bl {{.*}} memset

define void @test_memset_const_val_len_very_small(i32 %iptr_dst) {
entry:
  %dst = inttoptr i32 %iptr_dst to i8*
  call void @llvm.memset.p0i8.i32(i8* %dst, i8 10, i32 2, i32 1, i1 false)
  ret void
}
; CHECK-LABEL: test_memset_const_val_len_very_small
; CHECK: mov WORD PTR [{{.*}}],0xa0a
; CHECK-NOT: mov
; ARM32-LABEL: test_memset_const_val_len_very_small
; ARM32: uxtb
; ARM32: bl {{.*}} memset

define void @test_memset_const_val_len_3(i32 %iptr_dst) {
entry:
  %dst = inttoptr i32 %iptr_dst to i8*
  call void @llvm.memset.p0i8.i32(i8* %dst, i8 16, i32 3, i32 1, i1 false)
  ret void
}
; CHECK-LABEL: test_memset_const_val_len_3
; CHECK: mov WORD PTR [{{.*}}],0x1010
; CHECK-NEXT: mov BYTE PTR [{{.*}}+0x2],0x10
; CHECK-NOT: mov
; ARM32-LABEL: test_memset_const_val_len_3
; ARM32: uxtb
; ARM32: bl {{.*}} memset

define void @test_memset_const_val_len_mid(i32 %iptr_dst) {
entry:
  %dst = inttoptr i32 %iptr_dst to i8*
  call void @llvm.memset.p0i8.i32(i8* %dst, i8 32, i32 9, i32 1, i1 false)
  ret void
}
; CHECK-LABEL: test_memset_const_val_len_mid
; CHECK: mov DWORD PTR [{{.*}}+0x4],0x20202020
; CHECK: mov DWORD PTR [{{.*}}],0x20202020
; CHECK-NEXT: mov BYTE PTR [{{.*}}+0x8],0x20
; CHECK-NOT: mov
; ARM32-LABEL: test_memset_const_val_len_mid
; ARM32: uxtb
; ARM32: bl {{.*}} memset

define void @test_memset_zero_const_len_small(i32 %iptr_dst) {
entry:
  %dst = inttoptr i32 %iptr_dst to i8*
  call void @llvm.memset.p0i8.i32(i8* %dst, i8 0, i32 12, i32 1, i1 false)
  ret void
}
; CHECK-LABEL: test_memset_zero_const_len_small
; CHECK: pxor [[ZERO:xmm[0-9]+]],[[ZERO]]
; CHECK-NEXT: movq QWORD PTR [{{.*}}],[[ZERO]]
; CHECK-NEXT: mov DWORD PTR [{{.*}}+0x8],0x0
; CHECK-NOT: mov
; ARM32-LABEL: test_memset_zero_const_len_small
; ARM32: uxtb
; ARM32: bl {{.*}} memset

define void @test_memset_zero_const_len_small_overlap(i32 %iptr_dst) {
entry:
  %dst = inttoptr i32 %iptr_dst to i8*
  call void @llvm.memset.p0i8.i32(i8* %dst, i8 0, i32 15, i32 1, i1 false)
  ret void
}
; CHECK-LABEL: test_memset_zero_const_len_small_overlap
; CHECK: pxor [[ZERO:xmm[0-9]+]],[[ZERO]]
; CHECK-NEXT: movq QWORD PTR [{{.*}}],[[ZERO]]
; CHECK-NEXT: movq QWORD PTR [{{.*}}+0x7],[[ZERO]]
; CHECK-NOT: mov
; ARM32-LABEL: test_memset_zero_const_len_small_overlap
; ARM32: uxtb
; ARM32: bl {{.*}} memset

define void @test_memset_zero_const_len_large_overlap(i32 %iptr_dst) {
entry:
  %dst = inttoptr i32 %iptr_dst to i8*
  call void @llvm.memset.p0i8.i32(i8* %dst, i8 0, i32 30, i32 1, i1 false)
  ret void
}
; CHECK-LABEL: test_memset_zero_const_len_large_overlap
; CHECK: pxor [[ZERO:xmm[0-9]+]],[[ZERO]]
; CHECK-NEXT: movups XMMWORD PTR [{{.*}}],[[ZERO]]
; CHECK-NEXT: movups XMMWORD PTR [{{.*}}+0xe],[[ZERO]]
; CHECK-NOT: mov
; ARM32-LABEL: test_memset_zero_const_len_large_overlap
; ARM32: uxtb
; ARM32: bl {{.*}} memset

define void @test_memset_zero_const_len_large(i32 %iptr_dst) {
entry:
  %dst = inttoptr i32 %iptr_dst to i8*
  call void @llvm.memset.p0i8.i32(i8* %dst, i8 0, i32 33, i32 1, i1 false)
  ret void
}
; CHECK-LABEL: test_memset_zero_const_len_large
; CHECK: pxor [[ZERO:xmm[0-9]+]],[[ZERO]]
; CHECK-NEXT: movups XMMWORD PTR [{{.*}}+0x10],[[ZERO]]
; CHECK-NEXT: movups XMMWORD PTR [{{.*}}],[[ZERO]]
; CHECK-NEXT: mov BYTE PTR [{{.*}}+0x20],0x0
; CHECK-NOT: mov
; ARM32-LABEL: test_memset_zero_const_len_large
; ARM32: uxtb
; ARM32: bl {{.*}} memset
