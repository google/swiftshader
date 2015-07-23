; Tests the branch optimizations under O2 (against a lack of
; optimizations under Om1).

; RUN: %if --need=target_X8632 --command %p2i --filetype=obj --disassemble \
; RUN:   --target x8632 -i %s --args -O2 \
; RUN:   | %if --need=target_X8632 --command FileCheck --check-prefix=O2 %s

; RUN: %if --need=target_X8632 --command %p2i --filetype=obj --disassemble \
; RUN:   --target x8632 -i %s --args -Om1 \
; RUN:   | %if --need=target_X8632 --command FileCheck --check-prefix=OM1 %s

; TODO(jvoung): Stop skipping unimplemented parts (via --skip-unimplemented)
; once enough infrastructure is in. Also, switch to --filetype=obj
; when possible.
; RUN: %if --need=target_ARM32 --need=allow_dump \
; RUN:   --command %p2i --filetype=asm --assemble \
; RUN:   --disassemble --target arm32 -i %s --args -O2 --skip-unimplemented \
; RUN:   | %if --need=target_ARM32 --need=allow_dump \
; RUN:   --command FileCheck --check-prefix ARM32O2 %s

; RUN: %if --need=target_ARM32 --need=allow_dump \
; RUN:   --command %p2i --filetype=asm --assemble \
; RUN:   --disassemble --target arm32 -i %s --args -Om1 --skip-unimplemented \
; RUN:   | %if --need=target_ARM32 --need=allow_dump \
; RUN:   --command FileCheck \
; RUN:   --check-prefix ARM32OM1 %s

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
; There will be nops for bundle align to end (for NaCl), but there should
; not be a branch.
; O2-NOT: j
; O2: call

; OM1-LABEL: testUncondToNextBlock
; OM1: call
; OM1-NEXT: jmp
; OM1: call

; ARM32O2-LABEL: testUncondToNextBlock
; ARM32O2: bl {{.*}} dummy
; ARM32O2-NEXT: bl {{.*}} dummy

; ARM32OM1-LABEL: testUncondToNextBlock
; ARM32OM1: bl {{.*}} dummy
; ARM32OM1-NEXT: b
; ARM32OM1-NEXT: bl {{.*}} dummy

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
; O2: cmp {{.*}},0x7b
; O2-NEXT: jge
; O2-NOT: j
; O2: call
; O2: ret
; O2: call
; O2: ret

; OM1-LABEL: testCondFallthroughToNextBlock
; OM1: cmp {{.*}},0x7b
; OM1: setge
; OM1: cmp
; OM1: jne
; OM1: jmp
; OM1: call
; OM1: ret
; OM1: call
; OM1: ret

; Note that compare and branch folding isn't implemented yet (unlike x86-32).
; ARM32O2-LABEL: testCondFallthroughToNextBlock
; ARM32O2: cmp {{.*}}, #123
; ARM32O2-NEXT: movge {{.*}}, #1
; ARM32O2-NEXT: cmp {{.*}}, #0
; ARM32O2-NEXT: bne
; ARM32O2-NEXT: bl
; ARM32O2: bx lr
; ARM32O2: bl
; ARM32O2: bx lr

; ARM32OM1-LABEL: testCondFallthroughToNextBlock
; ARM32OM1: cmp {{.*}}, #123
; ARM32OM1-NEXT: movge {{.*}}, #1
; ARM32OM1: cmp {{.*}}, #0
; ARM32OM1: bne
; ARM32OM1: b
; ARM32OM1: bl
; ARM32OM1: bx lr
; ARM32OM1: bl
; ARM32OM1: bx lr

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
; O2: cmp {{.*}},0x7b
; O2-NEXT: jl
; O2-NOT: j
; O2: call
; O2: ret
; O2: call
; O2: ret

; OM1-LABEL: testCondTargetNextBlock
; OM1: cmp {{.*}},0x7b
; OM1: setge
; OM1: cmp
; OM1: jne
; OM1: jmp
; OM1: call
; OM1: ret
; OM1: call
; OM1: ret

; Note that compare and branch folding isn't implemented yet
; (compared to x86-32).
; ARM32O2-LABEL: testCondTargetNextBlock
; ARM32O2: cmp {{.*}}, #123
; ARM32O2-NEXT: movge {{.*}}, #1
; ARM32O2-NEXT: cmp {{.*}}, #0
; ARM32O2-NEXT: beq
; ARM32O2-NEXT: bl
; ARM32O2: bx lr
; ARM32O2: bl
; ARM32O2: bx lr

; ARM32OM1-LABEL: testCondTargetNextBlock
; ARM32OM1: cmp {{.*}}, #123
; ARM32OM1: movge {{.*}}, #1
; ARM32OM1: cmp {{.*}}, #0
; ARM32OM1: bne
; ARM32OM1: b
; ARM32OM1: bl
; ARM32OM1: bx lr
; ARM32OM1: bl
; ARM32OM1: bx lr

; Unconditional branches to the block after a contracted block should be
; removed.
define void @testUncondToBlockAfterContract() {
entry:
  call void @dummy()
  br label %target
contract:
  br label %target
target:
  call void @dummy()
  ret void
}

; O2-LABEL: testUncondToBlockAfterContract
; O2: call
; There will be nops for bundle align to end (for NaCl), but there should
; not be a branch.
; O2-NOT: j
; O2: call

; OM1-LABEL: testUncondToBlockAfterContract
; OM1: call
; OM1-NEXT: jmp
; OM1: call

; ARM32O2-LABEL: testUncondToBlockAfterContract
; ARM32O2: bl {{.*}} dummy
; ARM32O2-NEXT: bl {{.*}} dummy

; ARM32OM1-LABEL: testUncondToBlockAfterContract
; ARM32OM1: bl {{.*}} dummy
; ARM32OM1-NEXT: b
; ARM32OM1-NEXT: bl {{.*}} dummy
