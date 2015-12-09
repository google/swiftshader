; Tests basics and corner cases of arm32 sandboxing, using -Om1 in the hope that
; the output will remain stable.  When packing bundles, we try to limit to a few
; instructions with well known sizes and minimal use of registers and stack
; slots in the lowering sequence.

; REQUIRES: allow_dump, target_ARM32
; RUN: %p2i -i %s --sandbox --filetype=asm --target=arm32 --assemble \
; RUN:   --disassemble --args -Om1 -allow-externally-defined-symbols \
; RUN:   -ffunction-sections  | FileCheck %s

declare void @call_target()
declare void @call_target1(i32 %arg0)
declare void @call_target2(i32 %arg0, i32 %arg1)
declare void @call_target3(i32 %arg0, i32 %arg1, i32 %arg2)
@global_short = internal global [2 x i8] zeroinitializer

; A direct call sequence uses the right mask and register-call sequence.
define internal void @test_direct_call() {
entry:
  call void @call_target()
  ret void
}
; CHECK-LABEL: test_direct_call
; CHECK: sub sp,
; CHECK-NEXT: bic sp, sp, {{.*}} ; 0xc0000000
; CHECK: {{[0-9]*}}c: {{.*}} bl {{.*}} call_target
; CHECK-NEXT: {{[0-9]*}}0:

; An indirect call sequence uses the right mask and register-call sequence.
define internal void @test_indirect_call(i32 %target) {
entry:
  %__1 = inttoptr i32 %target to void ()*
  call void %__1()
  ret void
}
; CHECK-LABEL: test_indirect_call
; CHECK: sub sp,
; CHECK: bic sp, sp, {{.*}} ; 0xc0000000
; CHECK-NOT: bic sp, sp, {{.*}} ; 0xc0000000
; CHECK: ldr [[REG:r[0-9]+]], [sp, 
; CHECK-NEXT: nop
; CHECK: {{[0-9]+}}8: {{.*}} bic [[REG:r[0-9]+]], [[REG]], {{.*}} 0xc000000f
; CHECK-NEXT: blx [[REG]]
; CHECk-NEXT: {{[0-9]+}}0:

; A return sequences uses the right pop / mask / jmp sequence.
define internal void @test_ret() {
entry:
  ret void
}
; CHECK-LABEL: test_ret
; CHECK: 0: {{.*}} bic lr, lr, {{.*}} 0xc000000f
; CHECK-NEXT: bx lr

; Bundle lock without padding.
define internal void @bundle_lock_without_padding() {
entry:
  %addr_short = bitcast [2 x i8]* @global_short to i16*
  store i16 0, i16* %addr_short, align 1
  ret void
}
; CHECK-LABEL: bundle_lock_without_padding
; CHECK: 0: {{.*}} movw
; CHECK-NEXT: movt
; CHECK-NEXT: mov
; CHECK-NEXT: nop
; CHECK-NEXT: bic [[REG:r[0-9]+]], {{.*}} 0xc0000000
; CHECK-NEXT: strh {{.*}}, {{[[]}}[[REG]]
; CHECK-NEXT: bic lr, lr, {{.*}} ; 0xc000000f
; CHECK-NEXT: {{.*}} bx lr

; Bundle lock with padding.
define internal void @bundle_lock_with_padding() {
entry:
  call void @call_target()
  ; bundle boundary
  store i16 0, i16* undef, align 1   ; 3 insts
  store i16 0, i16* undef, align 1   ; 3 insts
  store i16 0, i16* undef, align 1   ; 3 insts
                                     ; SP adjustment + pop
  ; nop
  ; bundle boundary
  ret void
}
; CHECK-LABEL: bundle_lock_with_padding
; CHECK: 48: {{.*}} pop
; CHECK-NEXT: nop
; CHECK-NEXT: bic lr, {{.*}} 0xc000000f
; CHECK-NEXT: {{.*}} bx lr

; Bundle lock align_to_end without any padding.
define internal void @bundle_lock_align_to_end_padding_0() {
entry:
  call void @call_target()
  ; bundle boundary
  call void @call_target3(i32 1, i32 2, i32 3)
  ; bundle boundary
  ret void
}
; CHECK-LABEL: bundle_lock_align_to_end_padding_0
; CHECK: c: {{.*}} bl {{.*}} call_target
; CHECK-NEXT: mov
; CHECK-NEXT: mov
; CHECK-NEXT: mov
; CHECK-NEXT: {{[0-9]+}}c: {{.*}} bl {{.*}} call_target3
; CHECK-NEXT: add sp
; CHECK-NEXT: bic sp, {{.*}} 0xc0000000
; CHECK-NEXT: pop
; CHECK: {{[0-9]+}}0: {{.*}} bic lr, lr, {{.*}} 0xc000000f
; CHECK-NEXT: {{.*}} bx lr

; Bundle lock align_to_end with one bunch of padding.
define internal void @bundle_lock_align_to_end_padding_1() {
entry:
  call void @call_target()
  ; bundle boundary
  call void @call_target2(i32 1, i32 2)
  ; bundle boundary
  ret void
}
; CHECK-LABEL: bundle_lock_align_to_end_padding_1
; CHECK: {{[0-9]*}}c: {{.*}} bl {{.*}} call_target
; CHECK-NEXT: mov
; CHECK-NEXT: mov
; CHECK-NEXT: nop
; CHECK-NEXT: bl {{.*}} call_target2
; CHECK: {{[0-9]+}}0: {{.*}} bic lr, lr, {{.*}} 0xc000000f
; CHECK-NEXT: {{.*}} bx lr

; Bundle lock align_to_end with two bunches of padding.
define internal void @bundle_lock_align_to_end_padding_2() {
entry:
  call void @call_target2(i32 1, i32 2)
  ; bundle boundary
  ret void
}
; CHECK-LABEL: bundle_lock_align_to_end_padding_2
; CHECK: mov
; CHECK-NEXT: mov
; CHECK-NEXT: nop
; CHECK-NEXT: nop
; CHECK-NEXT: bl {{.*}} call_target2
