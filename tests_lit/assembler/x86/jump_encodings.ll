; Tests various aspects of x86 branch encodings (near vs far,
; forward vs backward, using CFG labels, or local labels).

; Use -ffunction-sections so that the offsets reset for each function.
; RUN: %p2i -i %s --args -O2 --verbose none -ffunction-sections \
; RUN:   | llvm-mc -triple=i686-none-nacl -x86-asm-syntax=intel -filetype=obj \
; RUN:   | llvm-objdump -d --symbolize -x86-asm-syntax=intel - | FileCheck %s
; RUN: %p2i -i %s --args --verbose none | FileCheck --check-prefix=ERRORS %s

; Use atomic ops as filler, which shouldn't get optimized out.
declare void @llvm.nacl.atomic.store.i32(i32, i32*, i32)
declare i32 @llvm.nacl.atomic.load.i32(i32*, i32)
declare i32 @llvm.nacl.atomic.rmw.i32(i32, i32*, i32, i32)

define void @test_near_backward(i32 %iptr, i32 %val) {
entry:
  br label %next
next:
  %ptr = inttoptr i32 %iptr to i32*
  call void @llvm.nacl.atomic.store.i32(i32 %val, i32* %ptr, i32 6)
  br label %next2
next2:
  call void @llvm.nacl.atomic.store.i32(i32 %val, i32* %ptr, i32 6)
  %cmp = icmp ult i32 %val, 0
  br i1 %cmp, label %next2, label %next
}

; CHECK-LABEL: test_near_backward
; CHECK:      8: {{.*}}  mov dword ptr
; CHECK-NEXT: a: {{.*}}  mfence
; CHECK-NEXT: d: {{.*}}  mov dword ptr
; CHECK-NEXT: f: {{.*}}  mfence
; CHECK-NEXT: 12: {{.*}} cmp
; (0x15 + 2) - 10 == 0xd
; CHECK-NEXT: 15: 72 f6 jb -10
; (0x17 + 2) - 17 == 0x8
; CHECK-NEXT: 17: eb ef jmp -17

; Test one of the backward branches being too large for 8 bits
; and one being just okay.
define void @test_far_backward1(i32 %iptr, i32 %val) {
entry:
  br label %next
next:
  %ptr = inttoptr i32 %iptr to i32*
  %tmp = call i32 @llvm.nacl.atomic.load.i32(i32* %ptr, i32 6)
  br label %next2
next2:
  call void @llvm.nacl.atomic.store.i32(i32 %tmp, i32* %ptr, i32 6)
  call void @llvm.nacl.atomic.store.i32(i32 %val, i32* %ptr, i32 6)
  call void @llvm.nacl.atomic.store.i32(i32 %val, i32* %ptr, i32 6)
  call void @llvm.nacl.atomic.store.i32(i32 %val, i32* %ptr, i32 6)
  call void @llvm.nacl.atomic.store.i32(i32 %val, i32* %ptr, i32 6)
  call void @llvm.nacl.atomic.store.i32(i32 %val, i32* %ptr, i32 6)
  call void @llvm.nacl.atomic.store.i32(i32 %val, i32* %ptr, i32 6)
  call void @llvm.nacl.atomic.store.i32(i32 %val, i32* %ptr, i32 6)
  call void @llvm.nacl.atomic.store.i32(i32 %val, i32* %ptr, i32 6)
  call void @llvm.nacl.atomic.store.i32(i32 %val, i32* %ptr, i32 6)
  call void @llvm.nacl.atomic.store.i32(i32 %val, i32* %ptr, i32 6)
  call void @llvm.nacl.atomic.store.i32(i32 %val, i32* %ptr, i32 6)
  call void @llvm.nacl.atomic.store.i32(i32 %val, i32* %ptr, i32 6)
  call void @llvm.nacl.atomic.store.i32(i32 %val, i32* %ptr, i32 6)
  call void @llvm.nacl.atomic.store.i32(i32 %val, i32* %ptr, i32 6)
  call void @llvm.nacl.atomic.store.i32(i32 %val, i32* %ptr, i32 6)
  call void @llvm.nacl.atomic.store.i32(i32 %val, i32* %ptr, i32 6)
  call void @llvm.nacl.atomic.store.i32(i32 %val, i32* %ptr, i32 6)
  call void @llvm.nacl.atomic.store.i32(i32 %val, i32* %ptr, i32 6)
  call void @llvm.nacl.atomic.store.i32(i32 %val, i32* %ptr, i32 6)
  call void @llvm.nacl.atomic.store.i32(i32 %val, i32* %ptr, i32 6)
  call void @llvm.nacl.atomic.store.i32(i32 %val, i32* %ptr, i32 6)
  call void @llvm.nacl.atomic.store.i32(i32 %val, i32* %ptr, i32 6)
  call void @llvm.nacl.atomic.store.i32(i32 %val, i32* %ptr, i32 6)
  %cmp = icmp ugt i32 %val, 0
  br i1 %cmp, label %next2, label %next
}

; CHECK-LABEL: test_far_backward1
; CHECK:      8: {{.*}}  mov {{.*}}, dword ptr [e{{[^s]}}
; CHECK-NEXT: a: {{.*}}  mov dword ptr
; CHECK-NEXT: c: {{.*}}  mfence
; (0x85 + 2) - 125 == 0xa
; CHECK: 85: 77 83 ja -125
; (0x87 + 5) - 132 == 0x8
; CHECK-NEXT: 87: e9 7c ff ff ff jmp -132

; Same as test_far_backward1, but with the conditional branch being
; the one that is too far.
define void @test_far_backward2(i32 %iptr, i32 %val) {
entry:
  br label %next
next:
  %ptr = inttoptr i32 %iptr to i32*
  %tmp = call i32 @llvm.nacl.atomic.load.i32(i32* %ptr, i32 6)
  %tmp2 = call i32 @llvm.nacl.atomic.load.i32(i32* %ptr, i32 6)
  %tmp3 = call i32 @llvm.nacl.atomic.load.i32(i32* %ptr, i32 6)
  %tmp4 = call i32 @llvm.nacl.atomic.load.i32(i32* %ptr, i32 6)
  %tmp5 = call i32 @llvm.nacl.atomic.load.i32(i32* %ptr, i32 6)
  br label %next2
next2:
  call void @llvm.nacl.atomic.store.i32(i32 %tmp, i32* %ptr, i32 6)
  call void @llvm.nacl.atomic.store.i32(i32 %tmp2, i32* %ptr, i32 6)
  call void @llvm.nacl.atomic.store.i32(i32 %tmp3, i32* %ptr, i32 6)
  call void @llvm.nacl.atomic.store.i32(i32 %tmp4, i32* %ptr, i32 6)
  call void @llvm.nacl.atomic.store.i32(i32 %tmp5, i32* %ptr, i32 6)
  call void @llvm.nacl.atomic.store.i32(i32 %val, i32* %ptr, i32 6)
  call void @llvm.nacl.atomic.store.i32(i32 %val, i32* %ptr, i32 6)
  call void @llvm.nacl.atomic.store.i32(i32 %val, i32* %ptr, i32 6)
  call void @llvm.nacl.atomic.store.i32(i32 %val, i32* %ptr, i32 6)
  call void @llvm.nacl.atomic.store.i32(i32 %val, i32* %ptr, i32 6)
  call void @llvm.nacl.atomic.store.i32(i32 %val, i32* %ptr, i32 6)
  call void @llvm.nacl.atomic.store.i32(i32 %val, i32* %ptr, i32 6)
  call void @llvm.nacl.atomic.store.i32(i32 %val, i32* %ptr, i32 6)
  call void @llvm.nacl.atomic.store.i32(i32 %val, i32* %ptr, i32 6)
  call void @llvm.nacl.atomic.store.i32(i32 %val, i32* %ptr, i32 6)
  call void @llvm.nacl.atomic.store.i32(i32 %val, i32* %ptr, i32 6)
  call void @llvm.nacl.atomic.store.i32(i32 %val, i32* %ptr, i32 6)
  call void @llvm.nacl.atomic.store.i32(i32 %val, i32* %ptr, i32 6)
  call void @llvm.nacl.atomic.store.i32(i32 %val, i32* %ptr, i32 6)
  call void @llvm.nacl.atomic.store.i32(i32 %val, i32* %ptr, i32 6)
  call void @llvm.nacl.atomic.store.i32(i32 %val, i32* %ptr, i32 6)
  call void @llvm.nacl.atomic.store.i32(i32 %val, i32* %ptr, i32 6)
  call void @llvm.nacl.atomic.store.i32(i32 %val, i32* %ptr, i32 6)
  %cmp = icmp sle i32 %val, 0
  br i1 %cmp, label %next, label %next2
}

; CHECK-LABEL: test_far_backward2
; CHECK:      c:  {{.*}}  mov {{.*}}, dword ptr [e{{[^s]}}
; CHECK:      14: {{.*}}  mov {{.*}}, dword ptr
; CHECK-NEXT: 16: {{.*}}  mov dword ptr
; CHECK-NEXT: 18: {{.*}}  mfence
; (0x8c + 6) - 134 == 0xc
; CHECK: 8c: 0f 8e 7a ff ff ff jle -134
; (0x92 + 2) - 126 == 0x16
; CHECK-NEXT: 92: eb 82 jmp -126

define void @test_near_forward(i32 %iptr, i32 %val) {
entry:
  br label %next1
next1:
  %ptr = inttoptr i32 %iptr to i32*
  %cmp = icmp ult i32 %val, 0
  br i1 %cmp, label %next3, label %next2
next2:
  call void @llvm.nacl.atomic.store.i32(i32 %val, i32* %ptr, i32 6)
  br label %next3
next3:
  call void @llvm.nacl.atomic.store.i32(i32 %val, i32* %ptr, i32 6)
  br label %next1
}
; Forward branches for non-local labels currently use the fully relaxed
; form to avoid needing a relaxation pass.
; CHECK-LABEL: test_near_forward
; CHECK:       8: {{.*}}            cmp
; CHECK-NEXT:  b: 0f 82 05 00 00 00 jb 5
; CHECK-NEXT: 11: {{.*}}            mov dword ptr
; CHECK-NEXT: 13: {{.*}}            mfence
; Forward branch is 5 bytes ahead to here.
; CHECK-NEXT: 16: {{.*}}            mov dword ptr
; Jumps back to (0x1b + 2) - 21 == 0x8 (to before the forward branch,
; therefore knowing that the forward branch was indeed 6 bytes).
; CHECK:      1b: eb eb             jmp -21


; Unlike forward branches to cfg nodes, "local" forward branches
; always use a 1 byte displacement.
; Check local forward branches, followed by a near backward branch
; to make sure that the instruction size accounting for the forward
; branches are correct, by the time the backward branch is hit.
; A 64-bit compare happens to use local forward branches.
define void @test_local_forward_then_back(i64 %val64, i32 %iptr, i32 %val) {
entry:
  br label %next
next:
  %ptr = inttoptr i32 %iptr to i32*
  call void @llvm.nacl.atomic.store.i32(i32 %val, i32* %ptr, i32 6)
  br label %next2
next2:
  %cmp = icmp ult i64 %val64, 0
  br i1 %cmp, label %next, label %next2
}
; CHECK-LABEL: test_local_forward_then_back
; CHECK:      14: {{.*}} mov dword ptr
; CHECK-NEXT: 16: {{.*}} mfence
; CHECK-NEXT: 19: {{.*}} mov dword ptr {{.*}}, 1
; CHECK-NEXT: 20: {{.*}} cmp
; CHECK-NEXT: 23: {{.*}} jb 14
; (0x37 + 2) - 37 == 0x14
; CHECK:      37: {{.*}} jne -37
; (0x39 + 2) - 34 == 0x19
; CHECK:      39: {{.*}} jmp -34


; Test that backward local branches also work and are small.
; Some of the atomic instructions use a cmpxchg loop.
define void @test_local_backward(i64 %val64, i32 %iptr, i32 %val) {
entry:
  br label %next
next:
  %ptr = inttoptr i32 %iptr to i32*
  %a = call i32 @llvm.nacl.atomic.rmw.i32(i32 5, i32* %ptr, i32 %val, i32 6)
  br label %next2
next2:
  %success = icmp eq i32 1, %a
  br i1 %success, label %next, label %next2
}
; CHECK-LABEL: test_local_backward
; CHECK:       9: {{.*}} mov {{.*}}, dword
; CHECK:       b: {{.*}} mov
; CHECK-NEXT:  d: {{.*}} xor
; CHECK-NEXT:  f: {{.*}} lock
; CHECK-NEXT: 10: {{.*}} cmpxchg
; (0x13 + 2) - 10 == 0xb
; CHECK-NEXT: 13: 75 f6 jne -10
; (0x1c + 2) - 21 == 0x9
; CHECK:      1c: 74 eb je -21

; ERRORS-NOT: ICE translation error
