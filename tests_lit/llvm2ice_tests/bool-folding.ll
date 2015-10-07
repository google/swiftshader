; This tests the optimization where producers and consumers of i1 (bool)
; variables are combined to implicitly use flags instead of explicitly using
; stack or register variables.

; RUN: %p2i -i %s --filetype=obj --disassemble --args -O2 \
; RUN:   -allow-externally-defined-symbols | FileCheck %s

; RUN: %if --need=allow_dump --need=target_ARM32 --command %p2i --filetype=asm \
; RUN:   --target arm32 -i %s --args -O2 --skip-unimplemented \
; RUN:   -allow-externally-defined-symbols \
; RUN:   | %if --need=allow_dump --need=target_ARM32 --command FileCheck %s \
; RUN:   --check-prefix=ARM32

declare void @use_value(i32)

; Basic cmp/branch folding.
define internal i32 @fold_cmp_br(i32 %arg1, i32 %arg2) {
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
; ARM32-LABEL: fold_cmp_br
; ARM32: cmp
; ARM32: beq


; Cmp/branch folding with intervening instructions.
define internal i32 @fold_cmp_br_intervening_insts(i32 %arg1, i32 %arg2) {
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
; ARM32-LABEL: fold_cmp_br_intervening_insts
; ARM32: push {{[{].*[}]}}
; ARM32: movlt [[TMP:r[0-9]+]], #1
; ARM32: mov [[P:r[4-7]]], [[TMP]]
; ARM32: bl
; ARM32: cmp [[P]], #0
; ARM32: beq


; Cmp/branch non-folding because of live-out.
define internal i32 @no_fold_cmp_br_liveout(i32 %arg1, i32 %arg2) {
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
; ARM32-LABEL: no_fold_cmp_br_liveout
; ARM32: cmp
; ARM32: movlt [[REG:r[0-9]+]]
; ARM32: cmp [[REG]], #0
; ARM32: beq


; Cmp/branch non-folding because of extra non-whitelisted uses.
define internal i32 @no_fold_cmp_br_non_whitelist(i32 %arg1, i32 %arg2) {
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
; ARM32-LABEL: no_fold_cmp_br_non_whitelist
; ARM32: mov [[R:r[0-9]+]], #0
; ARM32: cmp r0, r1
; ARM32: movlt [[R]], #1
; ARM32: mov [[R2:r[0-9]+]], [[R]]
; ARM32: and [[R3:r[0-9]+]], [[R2]], #1
; ARM32: cmp [[R]]
; ARM32: beq


; Basic cmp/select folding.
define internal i32 @fold_cmp_select(i32 %arg1, i32 %arg2) {
entry:
  %cmp1 = icmp slt i32 %arg1, %arg2
  %result = select i1 %cmp1, i32 %arg1, i32 %arg2
  ret i32 %result
}

; CHECK-LABEL: fold_cmp_select
; CHECK: cmp
; CHECK: cmovl
; ARM32-LABEL: fold_cmp_select
; ARM32: mov [[R:r[0-9]+]], #0
; ARM32: cmp r0, r1
; ARM32: movlt [[R]], #1
; ARM32: cmp [[R]], #0


; 64-bit cmp/select folding.
define internal i64 @fold_cmp_select_64(i64 %arg1, i64 %arg2) {
entry:
  %arg1_trunc = trunc i64 %arg1 to i32
  %arg2_trunc = trunc i64 %arg2 to i32
  %cmp1 = icmp slt i32 %arg1_trunc, %arg2_trunc
  %result = select i1 %cmp1, i64 %arg1, i64 %arg2
  ret i64 %result
}

; CHECK-LABEL: fold_cmp_select_64
; CHECK: cmp
; CHECK: cmovl
; CHECK: cmovl
; ARM32-LABEL: fold_cmp_select_64
; ARM32: mov [[R:r[0-9]+]], #0
; ARM32: cmp r0, r2
; ARM32: movlt [[R]], #1
; ARM32: cmp [[R]], #0
; ARM32: movne
; ARM32: movne
; ARM32-DAG: mov r0
; ARM32-DAG: mov r1
; ARM32: bx lr


define internal i64 @fold_cmp_select_64_undef(i64 %arg1) {
entry:
  %arg1_trunc = trunc i64 %arg1 to i32
  %cmp1 = icmp slt i32 undef, %arg1_trunc
  %result = select i1 %cmp1, i64 %arg1, i64 undef
  ret i64 %result
}
; CHECK-LABEL: fold_cmp_select_64_undef
; CHECK: cmp
; CHECK: cmovl
; CHECK: cmovl
; ARM32-LABEL: fold_cmp_select_64_undef
; ARM32: cmp {{r[0-9]+}}, r0
; ARM32: movlt [[R:r[0-9]+]], #1
; ARM32: cmp [[R]]
; ARM32: movne
; ARM32: movne
; ARM32-DAG: mov r0
; ARM32-DAG: mov r1
; ARM32: bx lr


; Cmp/select folding with intervening instructions.
define internal i32 @fold_cmp_select_intervening_insts(i32 %arg1, i32 %arg2) {
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
; CHECK: cmovl
; ARM32-LABEL: fold_cmp_select_intervening_insts
; ARM32: mov [[RES0:r[4-7]+]], r0
; ARM32: mov [[RES1:r[4-7]+]], r1
; ARM32: mov [[R:r[0-9]+]], #0
; ARM32: cmp r{{[0-9]+}}, r{{[0-9]+}}
; ARM32: movlt [[R]], #1
; ARM32: mov [[R2:r[4-7]]], [[R]]
; ARM32: bl use_value
; ARM32: cmp [[R2]], #0
; ARM32: movne [[RES1]], [[RES0]]
; ARM32: mov r0, [[RES1]]


; Cmp/multi-select folding.
define internal i32 @fold_cmp_select_multi(i32 %arg1, i32 %arg2) {
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
; CHECK: cmovl
; CHECK: cmp
; CHECK: cmovl
; CHECK: cmp
; CHECK: cmovge
; CHECK: add
; CHECK: add
; ARM32-LABEL: fold_cmp_select_multi
; ARM32-DAG: mov [[T0:r[0-9]+]], #0
; ARM32-DAG: cmp r0, r1
; ARM32: movlt [[T0]], #1
; ARM32-DAG: mov [[T1:r[0-9]+]], r1
; ARM32-DAG: cmp [[T0]], #0
; ARM32: [[T1]], r0
; ARM32-DAG: mov [[T2:r[0-9]+]], r0
; ARM32-DAG: cmp [[T0]], #0
; ARM32: [[T2]], r1
; ARM32: cmp [[T0]], #0
; ARM32: movne
; ARM32: add
; ARM32: add
; ARM32: bx lr


; Cmp/multi-select non-folding because of live-out.
define internal i32 @no_fold_cmp_select_multi_liveout(i32 %arg1, i32 %arg2) {
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
; CHECK: cmovne
; CHECK: cmp
; CHECK: cmovne
; CHECK: cmp
; CHECK: cmove
; CHECK: add
; CHECK: add
; ARM32-LABEL: no_fold_cmp_select_multi_liveout
; ARM32-LABEL: fold_cmp_select_multi
; ARM32-DAG: mov [[T0:r[0-9]+]], #0
; ARM32-DAG: cmp r0, r1
; ARM32: movlt [[T0]], #1
; ARM32-DAG: mov [[T1:r[0-9]+]], r1
; ARM32-DAG: cmp [[T0]], #0
; ARM32: [[T1]], r0
; ARM32-DAG: mov [[T2:r[0-9]+]], r0
; ARM32-DAG: cmp [[T0]], #0
; ARM32: [[T2]], r1
; ARM32: cmp [[T0]], #0
; ARM32: movne
; ARM32: add
; ARM32: add
; ARM32: bx lr

; Cmp/multi-select non-folding because of extra non-whitelisted uses.
define internal i32 @no_fold_cmp_select_multi_non_whitelist(i32 %arg1,
                                                            i32 %arg2) {
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
; CHECK: cmovne
; CHECK: cmp
; CHECK: cmovne
; CHECK: cmp
; CHECK: cmove
; CHECK: movzx
; CHECK: add
; CHECK: add
; CHECK: add
; ARM32-LABEL: no_fold_cmp_select_multi_non_whitelist
; ARM32-DAG: mov [[T0:r[0-9]+]], #0
; ARM32-DAG: cmp r0, r1
; ARM32: movlt [[T0]], #1
; ARM32-DAG: mov [[T1:r[0-9]+]], r1
; ARM32-DAG: cmp [[T0]], #0
; ARM32: [[T1]], r0
; ARM32-DAG: mov [[T2:r[0-9]+]], r0
; ARM32-DAG: cmp [[T0]], #0
; ARM32: [[T2]], r1
; ARM32: cmp [[T0]], #0
; ARM32: movne
; ARM32: and {{.*}}, [[T0]], #1
; ARM32: add
; ARM32: add
; ARM32: add
; ARM32: bx lr
