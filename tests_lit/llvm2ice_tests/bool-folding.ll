; This tests the optimization where producers and consumers of i1 (bool)
; variables are combined to implicitly use flags instead of explicitly using
; stack or register variables.

; RUN: %p2i -i %s --filetype=obj --disassemble --args -O2 | FileCheck %s

declare void @use_value(i32)

; Basic cmp/branch folding.
define i32 @fold_cmp_br(i32 %arg1, i32 %arg2) {
entry:
  %cmp1 = icmp slt i32 %arg1, %arg2
  br i1 %cmp1, label %branch1, label %branch2
branch1:
  ret i32 1
branch2:
  ret i32 2
}

; CHECK-LABEL: fold_cmp_br
; CHECK: cmp
; CHECK: jge


; Cmp/branch folding with intervening instructions.
define i32 @fold_cmp_br_intervening_insts(i32 %arg1, i32 %arg2) {
entry:
  %cmp1 = icmp slt i32 %arg1, %arg2
  call void @use_value(i32 %arg1)
  br i1 %cmp1, label %branch1, label %branch2
branch1:
  ret i32 1
branch2:
  ret i32 2
}

; CHECK-LABEL: fold_cmp_br_intervening_insts
; CHECK-NOT: cmp
; CHECK: call
; CHECK: cmp
; CHECK: jge


; Cmp/branch non-folding because of live-out.
define i32 @no_fold_cmp_br_liveout(i32 %arg1, i32 %arg2) {
entry:
  %cmp1 = icmp slt i32 %arg1, %arg2
  br label %next
next:
  br i1 %cmp1, label %branch1, label %branch2
branch1:
  ret i32 1
branch2:
  ret i32 2
}

; CHECK-LABEL: no_fold_cmp_br_liveout
; CHECK: cmp
; CHECK: set
; CHECK: cmp
; CHECK: je


; Cmp/branch non-folding because of extra non-whitelisted uses.
define i32 @no_fold_cmp_br_non_whitelist(i32 %arg1, i32 %arg2) {
entry:
  %cmp1 = icmp slt i32 %arg1, %arg2
  %result = zext i1 %cmp1 to i32
  br i1 %cmp1, label %branch1, label %branch2
branch1:
  ret i32 %result
branch2:
  ret i32 2
}

; CHECK-LABEL: no_fold_cmp_br_non_whitelist
; CHECK: cmp
; CHECK: set
; CHECK: movzx
; CHECK: cmp
; CHECK: je


; Basic cmp/select folding.
define i32 @fold_cmp_select(i32 %arg1, i32 %arg2) {
entry:
  %cmp1 = icmp slt i32 %arg1, %arg2
  %result = select i1 %cmp1, i32 %arg1, i32 %arg2
  ret i32 %result
}

; CHECK-LABEL: fold_cmp_select
; CHECK: cmp
; CHECK: jl
; CHECK: mov


; 64-bit cmp/select folding.
define i64 @fold_cmp_select_64(i64 %arg1, i64 %arg2) {
entry:
  %arg1_trunc = trunc i64 %arg1 to i32
  %arg2_trunc = trunc i64 %arg2 to i32
  %cmp1 = icmp slt i32 %arg1_trunc, %arg2_trunc
  %result = select i1 %cmp1, i64 %arg1, i64 %arg2
  ret i64 %result
}

; CHECK-LABEL: fold_cmp_select_64
; CHECK: cmp
; CHECK: jl
; CHECK: mov
; CHECK: mov


; Cmp/select folding with intervening instructions.
define i32 @fold_cmp_select_intervening_insts(i32 %arg1, i32 %arg2) {
entry:
  %cmp1 = icmp slt i32 %arg1, %arg2
  call void @use_value(i32 %arg1)
  %result = select i1 %cmp1, i32 %arg1, i32 %arg2
  ret i32 %result
}

; CHECK-LABEL: fold_cmp_select_intervening_insts
; CHECK-NOT: cmp
; CHECK: call
; CHECK: cmp
; CHECK: jl
; CHECK: mov


; Cmp/multi-select folding.
define i32 @fold_cmp_select_multi(i32 %arg1, i32 %arg2) {
entry:
  %cmp1 = icmp slt i32 %arg1, %arg2
  %a = select i1 %cmp1, i32 %arg1, i32 %arg2
  %b = select i1 %cmp1, i32 %arg2, i32 %arg1
  %c = select i1 %cmp1, i32 123, i32 %arg1
  %partial = add i32 %a, %b
  %result = add i32 %partial, %c
  ret i32 %result
}

; CHECK-LABEL: fold_cmp_select_multi
; CHECK: cmp
; CHECK: jl
; CHECK: cmp
; CHECK: jl
; CHECK: cmp
; CHECK: jl
; CHECK: add
; CHECK: add


; Cmp/multi-select non-folding because of live-out.
define i32 @no_fold_cmp_select_multi_liveout(i32 %arg1, i32 %arg2) {
entry:
  %cmp1 = icmp slt i32 %arg1, %arg2
  %a = select i1 %cmp1, i32 %arg1, i32 %arg2
  %b = select i1 %cmp1, i32 %arg2, i32 %arg1
  br label %next
next:
  %c = select i1 %cmp1, i32 123, i32 %arg1
  %partial = add i32 %a, %b
  %result = add i32 %partial, %c
  ret i32 %result
}

; CHECK-LABEL: no_fold_cmp_select_multi_liveout
; CHECK: set
; CHECK: cmp
; CHECK: jne
; CHECK: cmp
; CHECK: jne
; CHECK: cmp
; CHECK: jne
; CHECK: add
; CHECK: add


; Cmp/multi-select non-folding because of extra non-whitelisted uses.
define i32 @no_fold_cmp_select_multi_non_whitelist(i32 %arg1, i32 %arg2) {
entry:
  %cmp1 = icmp slt i32 %arg1, %arg2
  %a = select i1 %cmp1, i32 %arg1, i32 %arg2
  %b = select i1 %cmp1, i32 %arg2, i32 %arg1
  %c = select i1 %cmp1, i32 123, i32 %arg1
  %ext = zext i1 %cmp1 to i32
  %partial1 = add i32 %a, %b
  %partial2 = add i32 %partial1, %c
  %result = add i32 %partial2, %ext
  ret i32 %result
}

; CHECK-LABEL: no_fold_cmp_select_multi_non_whitelist
; CHECK: set
; CHECK: cmp
; CHECK: jne
; CHECK: cmp
; CHECK: jne
; CHECK: cmp
; CHECK: jne
; CHECK: movzx
; CHECK: add
; CHECK: add
; CHECK: add
