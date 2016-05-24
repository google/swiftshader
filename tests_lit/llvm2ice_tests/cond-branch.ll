; Tests for conditional branch instructions

; RUN: %if --need=allow_dump --need=target_MIPS32 --command %p2i \
; RUN:   --filetype=asm --target mips32 -i %s --args -O2 --skip-unimplemented \
; RUN:   -allow-externally-defined-symbols \
; RUN:   | %if --need=allow_dump --need=target_MIPS32 --command FileCheck %s \
; RUN:   --check-prefix=COMMON --check-prefix=MIPS32

; RUN: %if --need=allow_dump --need=target_MIPS32 --command %p2i \
; RUN:   --filetype=asm --target mips32 -i %s --args -Om1 --skip-unimplemented \
; RUN:   -allow-externally-defined-symbols \
; RUN:   | %if --need=allow_dump --need=target_MIPS32 --command FileCheck %s \
; RUN:   --check-prefix=COMMON --check-prefix=MIPS32-OM1

define internal i32 @cond_br_eq(i32 %arg1, i32 %arg2) {
entry:
  %cmp1 = icmp eq i32 %arg1, %arg2
  br i1 %cmp1, label %branch1, label %branch2
branch1:
  ret i32 1
branch2:
  ret i32 2
}
; COMMON-LABEL: cond_br_eq
; MIPS32: bne
; MIPS32-OM1: xor
; MIPS32-OM1: sltiu {{.*}}, {{.*}}, 1
; MIPS32-OM1: beqz

define internal i32 @cond_br_ne(i32 %arg1, i32 %arg2) {
entry:
  %cmp1 = icmp ne i32 %arg1, %arg2
  br i1 %cmp1, label %branch1, label %branch2
branch1:
  ret i32 1
branch2:
  ret i32 2
}
; COMMON-LABEL: cond_br_ne
; MIPS32: beq
; MIPS32-OM1: xor
; MIPS32-OM1: sltu {{.*}}, $zero, {{.*}}
; MIPS32-OM1: beqz

define internal i32 @cond_br_slt(i32 %arg1, i32 %arg2) {
entry:
  %cmp1 = icmp slt i32 %arg1, %arg2
  br i1 %cmp1, label %branch1, label %branch2
branch1:
  ret i32 1
branch2:
  ret i32 2
}
; COMMON-LABEL: cond_br_slt
; MIPS32: slt
; MIPS32: beqz
; MIPS32-OM1: slt
; MIPS32-OM1: beqz

define internal i32 @cond_br_sle(i32 %arg1, i32 %arg2) {
entry:
  %cmp1 = icmp sle i32 %arg1, %arg2
  br i1 %cmp1, label %branch1, label %branch2
branch1:
  ret i32 1
branch2:
  ret i32 2
}
; COMMON-LABEL: cond_br_sle
; MIPS32: slt
; MIPS32: bnez
; MIPS32-OM1: slt
; MIPS32-OM1: xori {{.*}}, {{.*}}, 1
; MIPS32-OM1: beqz

define internal i32 @cond_br_sgt(i32 %arg1, i32 %arg2) {
entry:
  %cmp1 = icmp sgt i32 %arg1, %arg2
  br i1 %cmp1, label %branch1, label %branch2
branch1:
  ret i32 1
branch2:
  ret i32 2
}
; COMMON-LABEL: cond_br_sgt
; MIPS32: slt
; MIPS32: beqz
; MIPS32-OM1: slt
; MIPS32-OM1: beqz

define internal i32 @cond_br_sge(i32 %arg1, i32 %arg2) {
entry:
  %cmp1 = icmp sge i32 %arg1, %arg2
  br i1 %cmp1, label %branch1, label %branch2
branch1:
  ret i32 1
branch2:
  ret i32 2
}
; COMMON-LABEL: cond_br_sge
; MIPS32: slt
; MIPS32: bnez
; MIPS32-OM1: slt
; MIPS32-OM1: xori {{.*}}, {{.*}}, 1
; MIPS32-OM1: beqz

define internal i32 @cond_br_ugt(i32 %arg1, i32 %arg2) {
entry:
  %cmp1 = icmp ugt i32 %arg1, %arg2
  br i1 %cmp1, label %branch1, label %branch2
branch1:
  ret i32 1
branch2:
  ret i32 2
}
; COMMON-LABEL: cond_br_ugt
; MIPS32: sltu
; MIPS32: beqz
; MIPS32-OM1: sltu
; MIPS32-OM1: beqz

define internal i32 @cond_br_uge(i32 %arg1, i32 %arg2) {
entry:
  %cmp1 = icmp uge i32 %arg1, %arg2
  br i1 %cmp1, label %branch1, label %branch2
branch1:
  ret i32 1
branch2:
  ret i32 2
}
; COMMON-LABEL: cond_br_uge
; MIPS32: sltu
; MIPS32: bnez
; MIPS32-OM1: sltu
; MIPS32-OM1: xori {{.*}}, {{.*}}, 1
; MIPS32-OM1: beqz

define internal i32 @cond_br_ult(i32 %arg1, i32 %arg2) {
entry:
  %cmp1 = icmp ult i32 %arg1, %arg2
  br i1 %cmp1, label %branch1, label %branch2
branch1:
  ret i32 1
branch2:
  ret i32 2
}
; COMMON-LABEL: cond_br_ult
; MIPS32: sltu
; MIPS32: beqz
; MIPS32-OM1: sltu
; MIPS32-OM1: beqz

define internal i32 @cond_br_ule(i32 %arg1, i32 %arg2) {
entry:
  %cmp1 = icmp ule i32 %arg1, %arg2
  br i1 %cmp1, label %branch1, label %branch2
branch1:
  ret i32 1
branch2:
  ret i32 2
}
; COMMON-LABEL: cond_br_ule
; MIPS32: sltu
; MIPS32: bnez
; MIPS32-OM1: sltu
; MIPS32-OM1: xori {{.*}}, {{.*}}, 1
; MIPS32-OM1: beqz
