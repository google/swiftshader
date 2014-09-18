; Tests the branch optimizations under O2 (against a lack of
; optimizations under Om1).

; RUN: %llvm2ice -O2 --verbose none %s \
; RUN:   | llvm-mc -triple=i686-none-nacl -x86-asm-syntax=intel -filetype=obj \
; RUN:   | llvm-objdump -d -symbolize -x86-asm-syntax=intel - \
; RUN:   | FileCheck --check-prefix=O2 %s
; RUN: %llvm2ice -Om1 --verbose none %s \
; RUN:   | llvm-mc -triple=i686-none-nacl -x86-asm-syntax=intel -filetype=obj \
; RUN:   | llvm-objdump -d -symbolize -x86-asm-syntax=intel - \
; RUN:   | FileCheck --check-prefix=OM1 %s
; RUN: %llvm2ice --verbose none %s | FileCheck --check-prefix=ERRORS %s
; RUN: %llvm2iceinsts %s | %szdiff %s | FileCheck --check-prefix=DUMP %s
; RUN: %llvm2iceinsts --pnacl %s | %szdiff %s \
; RUN:                           | FileCheck --check-prefix=DUMP %s

declare void @dummy()

; An unconditional branch to the next block should be removed.
define void @testUncondToNextBlock() {
entry:
  call void @dummy()
  br label %next
next:
  call void @dummy()
  ret void
}
; O2-LABEL: testUncondToNextBlock
; O2: call
; O2-NEXT: call

; OM1-LABEL: testUncondToNextBlock
; OM1: call
; OM1-NEXT: jmp
; OM1-NEXT: call

; For a conditional branch with a fallthrough to the next block, the
; fallthrough branch should be removed.
define void @testCondFallthroughToNextBlock(i32 %arg) {
entry:
  %cmp = icmp sge i32 %arg, 123
  br i1 %cmp, label %target, label %fallthrough
fallthrough:
  call void @dummy()
  ret void
target:
  call void @dummy()
  ret void
}
; O2-LABEL: testCondFallthroughToNextBlock
; O2: cmp {{.*}}, 123
; O2-NEXT: jge
; O2-NEXT: call
; O2: ret
; O2: call
; O2: ret

; OM1-LABEL: testCondFallthroughToNextBlock
; OM1: cmp {{.*}}, 123
; OM1: jge
; OM1: cmp
; OM1: jne
; OM1: jmp
; OM1: call
; OM1: ret
; OM1: call
; OM1: ret

; For a conditional branch with the next block as the target and a
; different block as the fallthrough, the branch condition should be
; inverted, the fallthrough block changed to the target, and the
; branch to the next block removed.
define void @testCondTargetNextBlock(i32 %arg) {
entry:
  %cmp = icmp sge i32 %arg, 123
  br i1 %cmp, label %fallthrough, label %target
fallthrough:
  call void @dummy()
  ret void
target:
  call void @dummy()
  ret void
}
; O2-LABEL: testCondTargetNextBlock
; O2: cmp {{.*}}, 123
; O2-NEXT: jl
; O2-NEXT: call
; O2: ret
; O2: call
; O2: ret

; OM1-LABEL: testCondTargetNextBlock
; OM1: cmp {{.*}}, 123
; OM1: jge
; OM1: cmp
; OM1: jne
; OM1: jmp
; OM1: call
; OM1: ret
; OM1: call
; OM1: ret

; ERRORS-NOT: ICE translation error
; DUMP-NOT: SZ
