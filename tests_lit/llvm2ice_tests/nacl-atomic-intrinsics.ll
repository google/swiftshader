; This tests each of the supported NaCl atomic instructions for every
; size allowed.

; RUN: %llvm2ice -O2 --verbose none %s | FileCheck %s
; RUN: %llvm2ice -Om1 --verbose none %s | FileCheck %s
; RUN: %llvm2ice --verbose none %s | FileCheck --check-prefix=ERRORS %s
; RUN: %llvm2iceinsts %s | %szdiff %s | FileCheck --check-prefix=DUMP %s
; RUN: %llvm2iceinsts --pnacl %s | %szdiff %s \
; RUN:                           | FileCheck --check-prefix=DUMP %s

declare i8 @llvm.nacl.atomic.load.i8(i8*, i32)
declare i16 @llvm.nacl.atomic.load.i16(i16*, i32)
declare i32 @llvm.nacl.atomic.load.i32(i32*, i32)
declare i64 @llvm.nacl.atomic.load.i64(i64*, i32)
declare void @llvm.nacl.atomic.store.i8(i8, i8*, i32)
declare void @llvm.nacl.atomic.store.i16(i16, i16*, i32)
declare void @llvm.nacl.atomic.store.i32(i32, i32*, i32)
declare void @llvm.nacl.atomic.store.i64(i64, i64*, i32)
declare i8 @llvm.nacl.atomic.rmw.i8(i32, i8*, i8, i32)
declare i16 @llvm.nacl.atomic.rmw.i16(i32, i16*, i16, i32)
declare i32 @llvm.nacl.atomic.rmw.i32(i32, i32*, i32, i32)
declare i64 @llvm.nacl.atomic.rmw.i64(i32, i64*, i64, i32)
declare i8 @llvm.nacl.atomic.cmpxchg.i8(i8*, i8, i8, i32, i32)
declare i16 @llvm.nacl.atomic.cmpxchg.i16(i16*, i16, i16, i32, i32)
declare i32 @llvm.nacl.atomic.cmpxchg.i32(i32*, i32, i32, i32, i32)
declare i64 @llvm.nacl.atomic.cmpxchg.i64(i64*, i64, i64, i32, i32)
declare void @llvm.nacl.atomic.fence(i32)
declare void @llvm.nacl.atomic.fence.all()
declare i1 @llvm.nacl.atomic.is.lock.free(i32, i8*)

;;; Load

; x86 guarantees load/store to be atomic if naturally aligned.
; The PNaCl IR requires all atomic accesses to be naturally aligned.

define i32 @test_atomic_load_8(i32 %iptr) {
entry:
  %ptr = inttoptr i32 %iptr to i8*
  ; parameter value "6" is for the sequential consistency memory order.
  %i = call i8 @llvm.nacl.atomic.load.i8(i8* %ptr, i32 6)
  %r = zext i8 %i to i32
  ret i32 %r
}
; CHECK-LABEL: test_atomic_load_8
; CHECK: mov {{.*}}, dword
; CHECK: mov {{.*}}, byte

define i32 @test_atomic_load_16(i32 %iptr) {
entry:
  %ptr = inttoptr i32 %iptr to i16*
  %i = call i16 @llvm.nacl.atomic.load.i16(i16* %ptr, i32 6)
  %r = zext i16 %i to i32
  ret i32 %r
}
; CHECK-LABEL: test_atomic_load_16
; CHECK: mov {{.*}}, dword
; CHECK: mov {{.*}}, word

define i32 @test_atomic_load_32(i32 %iptr) {
entry:
  %ptr = inttoptr i32 %iptr to i32*
  %r = call i32 @llvm.nacl.atomic.load.i32(i32* %ptr, i32 6)
  ret i32 %r
}
; CHECK-LABEL: test_atomic_load_32
; CHECK: mov {{.*}}, dword
; CHECK: mov {{.*}}, dword

define i64 @test_atomic_load_64(i32 %iptr) {
entry:
  %ptr = inttoptr i32 %iptr to i64*
  %r = call i64 @llvm.nacl.atomic.load.i64(i64* %ptr, i32 6)
  ret i64 %r
}
; CHECK-LABEL: test_atomic_load_64
; CHECK: movq x{{.*}}, qword
; CHECK: movq qword {{.*}}, x{{.*}}

define i32 @test_atomic_load_32_with_arith(i32 %iptr) {
entry:
  %ptr = inttoptr i32 %iptr to i32*
  %r = call i32 @llvm.nacl.atomic.load.i32(i32* %ptr, i32 6)
  %r2 = add i32 %r, 32
  ret i32 %r2
}
; CHECK-LABEL: test_atomic_load_32_with_arith
; CHECK: mov {{.*}}, dword
; The next instruction may be a separate load or folded into an add.

define i32 @test_atomic_load_32_ignored(i32 %iptr) {
entry:
  %ptr = inttoptr i32 %iptr to i32*
  %ignored = call i32 @llvm.nacl.atomic.load.i32(i32* %ptr, i32 6)
  ret i32 0
}
; CHECK-LABEL: test_atomic_load_32_ignored
; CHECK: mov {{.*}}, dword
; CHECK: mov {{.*}}, dword

define i64 @test_atomic_load_64_ignored(i32 %iptr) {
entry:
  %ptr = inttoptr i32 %iptr to i64*
  %ignored = call i64 @llvm.nacl.atomic.load.i64(i64* %ptr, i32 6)
  ret i64 0
}
; CHECK-LABEL: test_atomic_load_64_ignored
; CHECK: movq x{{.*}}, qword
; CHECK: movq qword {{.*}}, x{{.*}}


;;; Store

define void @test_atomic_store_8(i32 %iptr, i32 %v) {
entry:
  %truncv = trunc i32 %v to i8
  %ptr = inttoptr i32 %iptr to i8*
  call void @llvm.nacl.atomic.store.i8(i8 %truncv, i8* %ptr, i32 6)
  ret void
}
; CHECK-LABEL: test_atomic_store_8
; CHECK: mov byte
; CHECK: mfence

define void @test_atomic_store_16(i32 %iptr, i32 %v) {
entry:
  %truncv = trunc i32 %v to i16
  %ptr = inttoptr i32 %iptr to i16*
  call void @llvm.nacl.atomic.store.i16(i16 %truncv, i16* %ptr, i32 6)
  ret void
}
; CHECK-LABEL: test_atomic_store_16
; CHECK: mov word
; CHECK: mfence

define void @test_atomic_store_32(i32 %iptr, i32 %v) {
entry:
  %ptr = inttoptr i32 %iptr to i32*
  call void @llvm.nacl.atomic.store.i32(i32 %v, i32* %ptr, i32 6)
  ret void
}
; CHECK-LABEL: test_atomic_store_32
; CHECK: mov dword
; CHECK: mfence

define void @test_atomic_store_64(i32 %iptr, i64 %v) {
entry:
  %ptr = inttoptr i32 %iptr to i64*
  call void @llvm.nacl.atomic.store.i64(i64 %v, i64* %ptr, i32 6)
  ret void
}
; CHECK-LABEL: test_atomic_store_64
; CHECK: movq x{{.*}}, qword
; CHECK: movq qword {{.*}}, x{{.*}}
; CHECK: mfence

define void @test_atomic_store_64_const(i32 %iptr) {
entry:
  %ptr = inttoptr i32 %iptr to i64*
  call void @llvm.nacl.atomic.store.i64(i64 12345678901234, i64* %ptr, i32 6)
  ret void
}
; CHECK-LABEL: test_atomic_store_64_const
; CHECK: mov {{.*}}, 1942892530
; CHECK: mov {{.*}}, 2874
; CHECK: movq x{{.*}}, qword
; CHECK: movq qword {{.*}}, x{{.*}}
; CHECK: mfence


;;; RMW

define i32 @test_atomic_rmw_add_8(i32 %iptr, i32 %v) {
entry:
  %trunc = trunc i32 %v to i8
  %ptr = inttoptr i32 %iptr to i8*
  ; "1" is an atomic add, and "6" is sequential consistency.
  %a = call i8 @llvm.nacl.atomic.rmw.i8(i32 1, i8* %ptr, i8 %trunc, i32 6)
  %a_ext = zext i8 %a to i32
  ret i32 %a_ext
}
; CHECK-LABEL: test_atomic_rmw_add_8
; CHECK: lock xadd byte {{.*}}, [[REG:.*]]
; CHECK: mov {{.*}}, {{.*}}[[REG]]

define i32 @test_atomic_rmw_add_16(i32 %iptr, i32 %v) {
entry:
  %trunc = trunc i32 %v to i16
  %ptr = inttoptr i32 %iptr to i16*
  %a = call i16 @llvm.nacl.atomic.rmw.i16(i32 1, i16* %ptr, i16 %trunc, i32 6)
  %a_ext = zext i16 %a to i32
  ret i32 %a_ext
}
; CHECK-LABEL: test_atomic_rmw_add_16
; CHECK: lock xadd word {{.*}}, [[REG:.*]]
; CHECK: mov {{.*}}, {{.*}}[[REG]]

define i32 @test_atomic_rmw_add_32(i32 %iptr, i32 %v) {
entry:
  %ptr = inttoptr i32 %iptr to i32*
  %a = call i32 @llvm.nacl.atomic.rmw.i32(i32 1, i32* %ptr, i32 %v, i32 6)
  ret i32 %a
}
; CHECK-LABEL: test_atomic_rmw_add_32
; CHECK: lock xadd dword {{.*}}, [[REG:.*]]
; CHECK: mov {{.*}}, {{.*}}[[REG]]

;define i64 @test_atomic_rmw_add_64(i32 %iptr, i64 %v) {
;entry:
;  %ptr = inttoptr i32 %iptr to i64*
;  %a = call i64 @llvm.nacl.atomic.rmw.i64(i32 1, i64* %ptr, i64 %v, i32 6)
;  ret i64 %a
;}
; CHECKLATER-LABEL: test_atomic_rmw_add_64
; CHECKLATER: uh need a... cmpxchg8b loop.

define i32 @test_atomic_rmw_add_32_ignored(i32 %iptr, i32 %v) {
entry:
  %ptr = inttoptr i32 %iptr to i32*
  %ignored = call i32 @llvm.nacl.atomic.rmw.i32(i32 1, i32* %ptr, i32 %v, i32 6)
  ret i32 %v
}
; CHECK-LABEL: test_atomic_rmw_add_32_ignored
; CHECK: lock xadd dword {{.*}}, [[REG:.*]]

;define i32 @test_atomic_rmw_sub_32(i32 %iptr, i32 %v) {
;entry:
;  %ptr = inttoptr i32 %iptr to i32*
;  %a = call i32 @llvm.nacl.atomic.rmw.i32(i32 2, i32* %ptr, i32 %v, i32 6)
;  ret i32 %a
;}
; CHECKLATER-LABEL: test_atomic_rmw_sub_32
; CHECKLATER: neg
; CHECKLATER: lock
; CHECKLATER: xadd

;define i32 @test_atomic_rmw_or_32(i32 %iptr, i32 %v) {
;entry:
;  %ptr = inttoptr i32 %iptr to i32*
;  %a = call i32 @llvm.nacl.atomic.rmw.i32(i32 3, i32* %ptr, i32 %v, i32 6)
;  ret i32 %a
;}
; CHECKLATER-LABEL: test_atomic_rmw_or_32
; Need a cmpxchg loop.

;define i32 @test_atomic_rmw_and_32(i32 %iptr, i32 %v) {
;entry:
;  %ptr = inttoptr i32 %iptr to i32*
;  %a = call i32 @llvm.nacl.atomic.rmw.i32(i32 4, i32* %ptr, i32 %v, i32 6)
;  ret i32 %a
;}
; CHECKLATER-LABEL: test_atomic_rmw_and_32
; Also a cmpxchg loop.

;define i32 @test_atomic_rmw_xor_32(i32 %iptr, i32 %v) {
;entry:
;  %ptr = inttoptr i32 %iptr to i32*
;  %a = call i32 @llvm.nacl.atomic.rmw.i32(i32 5, i32* %ptr, i32 %v, i32 6)
;  ret i32 %a
;}
; CHECKLATER-LABEL: test_atomic_rmw_xor_32
; Also a cmpxchg loop.

;define i32 @test_atomic_rmw_xchg_32(i32 %iptr, i32 %v) {
;entry:
;  %ptr = inttoptr i32 %iptr to i32*
;  %a = call i32 @llvm.nacl.atomic.rmw.i32(i32 6, i32* %ptr, i32 %v, i32 6)
;  ret i32 %a
;}
; CHECKLATER-LABEL: test_atomic_rmw_xchg_32

;;;; Cmpxchg

;define i32 @test_atomic_cmpxchg_8(i32 %iptr, i32 %expected, i32 %desired) {
;entry:
;  %ptr = inttoptr i32 %iptr to i8*
;  %trunc_exp = trunc i32 %expected to i8
;  %trunc_des = trunc i32 %desired to i8
;  %old = call i8 @llvm.nacl.atomic.cmpxchg.i8(i8* %ptr, i8 %trunc_exp,
;                                              i8 %trunc_des, i32 6, i32 6)
;  %old_ext = zext i8 %old to i32
;  ret i32 %old_ext
;}
; CHECKLATER-LABEL: test_atomic_cmpxchg_8
; CHECKLATER: lock cmpxchg byte

;define i32 @test_atomic_cmpxchg_16(i32 %iptr, i32 %expected, i32 %desired) {
;entry:
;  %ptr = inttoptr i32 %iptr to i16*
;  %trunc_exp = trunc i32 %expected to i16
;  %trunc_des = trunc i32 %desired to i16
;  %old = call i16 @llvm.nacl.atomic.cmpxchg.i16(i16* %ptr, i16 %trunc_exp,
;                                               i16 %trunc_des, i32 6, i32 6)
;  %old_ext = zext i16 %old to i32
;  ret i32 %old_ext
;}
; CHECKLATER-LABEL: test_atomic_cmpxchg_16
; This one is a bit gross for NaCl right now.
; https://code.google.com/p/nativeclient/issues/detail?id=2981
; But we'll assume that NaCl will have it fixed...
; CHECKLATER: lock cmpxchg word

;define i32 @test_atomic_cmpxchg_32(i32 %iptr, i32 %expected, i32 %desired) {
;entry:
;  %ptr = inttoptr i32 %iptr to i32*
;  %old = call i32 @llvm.nacl.atomic.cmpxchg.i32(i32* %ptr, i32 %expected,
;                                               i32 %desired, i32 6, i32 6)
;  ret i32 %old
;}
; CHECKLATER-LABEL: test_atomic_cmpxchg_32
; CHECKLATER: mov eax
; CHECKLATER: mov ecx
; CHECKLATER: lock cmpxchg dword

;define i64 @test_atomic_cmpxchg_64(i32 %iptr, i64 %expected, i64 %desired) {
;entry:
;  %ptr = inttoptr i32 %iptr to i64*
;  %old = call i64 @llvm.nacl.atomic.cmpxchg.i64(i64* %ptr, i64 %expected,
;                                               i64 %desired, i32 6, i32 6)
;  ret i64 %old
;}
; CHECKLATER-LABEL: test_atomic_cmpxchg_64
; CHECKLATER: mov eax
; CHECKLATER: mov edx
; CHECKLATER: mov ebx
; CHECKLATER: mov ecx
; CHECKLATER: lock cmpxchg8b qword

;define i32 @test_atomic_cmpxchg_32_loop(i32 %iptr,
;       i32 %expected, i32 %desired) {
;entry:
;  br label %loop
;
;loop:
;  %cmp = phi i32 [ %expected, %entry], [%old, %loop]
;  %ptr = inttoptr i32 %iptr to i32*
;  %old = call i32 @llvm.nacl.atomic.cmpxchg.i32(i32* %ptr, i32 %cmp,
;                                               i32 %desired, i32 6, i32 6)
;  %success = icmp eq i32 %cmp, %old
;  br i1 %success, label %done, label %loop
;
;done:
;  ret i32 %old
;}
; CHECKLATER-LABEL: test_atomic_cmpxchg_32_loop

;;;; Fence and is-lock-free.

define void @test_atomic_fence() {
entry:
  call void @llvm.nacl.atomic.fence(i32 6)
  ret void
}
; CHECK-LABEL: test_atomic_fence
; CHECK: mfence

define void @test_atomic_fence_all() {
entry:
  call void @llvm.nacl.atomic.fence.all()
  ret void
}
; CHECK-LABEL: test_atomic_fence_all
; CHECK: mfence

define i32 @test_atomic_is_lock_free(i32 %iptr) {
entry:
  %ptr = inttoptr i32 %iptr to i8*
  %i = call i1 @llvm.nacl.atomic.is.lock.free(i32 4, i8* %ptr)
  %r = zext i1 %i to i32
  ret i32 %r
}
; CHECK-LABEL: test_atomic_is_lock_free
; CHECK: mov {{.*}}, 1

define i32 @test_not_lock_free(i32 %iptr) {
entry:
  %ptr = inttoptr i32 %iptr to i8*
  %i = call i1 @llvm.nacl.atomic.is.lock.free(i32 7, i8* %ptr)
  %r = zext i1 %i to i32
  ret i32 %r
}
; CHECK-LABEL: test_not_lock_free
; CHECK: mov {{.*}}, 0

; TODO(jvoung): at some point we can take advantage of the
; fact that nacl.atomic.is.lock.free will resolve to a constant
; (which adds DCE opportunities). Once we optimize, the test expectations
; for this case should change.
define i32 @test_atomic_is_lock_free_can_dce(i32 %iptr, i32 %x, i32 %y) {
entry:
  %ptr = inttoptr i32 %iptr to i8*
  %i = call i1 @llvm.nacl.atomic.is.lock.free(i32 4, i8* %ptr)
  %i_ext = zext i1 %i to i32
  %cmp = icmp eq i32 %i_ext, 1
  br i1 %cmp, label %lock_free, label %not_lock_free
lock_free:
  ret i32 %i_ext

not_lock_free:
  %z = add i32 %x, %y
  ret i32 %z
}
; CHECK-LABEL: test_atomic_is_lock_free_can_dce
; CHECK: mov {{.*}}, 1
; CHECK: ret
; CHECK: add
; CHECK: ret

; ERRORS-NOT: ICE translation error
; DUMP-NOT: SZ
