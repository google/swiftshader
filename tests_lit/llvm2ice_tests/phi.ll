; This tests some of the subtleties of Phi lowering.  In particular,
; it tests that it does the right thing when it tries to enable
; compare/branch fusing.

; TODO(kschimpf) Find out why lc2i must be used.
; RUN: %lc2i -i %s --args -O2 --verbose none --phi-edge-split=0 \
; RUN:   | llvm-mc -triple=i686-none-nacl -x86-asm-syntax=intel -filetype=obj \
; RUN:   | llvm-objdump -d -symbolize -x86-asm-syntax=intel - | FileCheck %s
; RUN: %lc2i -i %s --args --verbose none | FileCheck --check-prefix=ERRORS %s
; RUN: %lc2i -i %s --insts | %szdiff %s | FileCheck --check-prefix=DUMP %s

define internal i32 @testPhi1(i32 %arg) {
entry:
  %cmp1 = icmp sgt i32 %arg, 0
  br i1 %cmp1, label %next, label %target
next:
  br label %target
target:
  %merge = phi i1 [ %cmp1, %entry ], [ false, %next ]
  %result = zext i1 %merge to i32
  ret i32 %result
}
; Test that compare/branch fusing does not happen, and Phi lowering is
; put in the right place.
; CHECK-LABEL: testPhi1
; CHECK: cmp {{.*}}, 0
; CHECK: mov {{.*}}, 1
; CHECK: jg
; CHECK: mov {{.*}}, 0
; CHECK: mov [[PHI:.*]],
; CHECK: cmp {{.*}}, 0
; CHECK: je
; CHECK: mov [[PHI]], 0
; CHECK: movzx {{.*}}, [[PHI]]

define internal i32 @testPhi2(i32 %arg) {
entry:
  %cmp1 = icmp sgt i32 %arg, 0
  br i1 %cmp1, label %next, label %target
next:
  br label %target
target:
  %merge = phi i32 [ 12345, %entry ], [ 54321, %next ]
  ret i32 %merge
}
; Test that compare/branch fusing and Phi lowering happens as expected.
; CHECK-LABEL: testPhi2
; CHECK: mov {{.*}}, 12345
; CHECK: cmp {{.*}}, 0
; CHECK-NEXT: jle
; CHECK: mov [[PHI:.*]], 54321
; CHECK: mov {{.*}}, [[PHI]]

; ERRORS-NOT: ICE translation error
; DUMP-NOT: SZ

; Test that address mode inference doesn't extend past
; multi-definition, non-SSA Phi temporaries.
define internal i32 @testPhi3(i32 %arg) {
entry:
  br label %body
body:
  %merge = phi i32 [ %arg, %entry ], [ %elt, %body ]
  %interior = add i32 %merge, 1000
  %__4 = inttoptr i32 %interior to i32*
  %elt = load i32* %__4, align 1
  %cmp = icmp eq i32 %elt, 0
  br i1 %cmp, label %exit, label %body
exit:
  %__6 = inttoptr i32 %interior to i32*
  store i32 %arg, i32* %__6, align 1
  ret i32 %arg
}
; I can't figure out how to reliably test this for correctness, so I
; will just include patterns for the entire current O2 sequence.  This
; may need to be changed when meaningful optimizations are added.
; The key is to avoid the "bad" pattern like this:
;
; testPhi3:
; .LtestPhi3$entry:
;         mov     eax, dword ptr [esp+4]
;         mov     ecx, eax
; .LtestPhi3$body:
;         mov     ecx, dword ptr [ecx+1000]
;         cmp     ecx, 0
;         jne     .LtestPhi3$body
; .LtestPhi3$exit:
;         mov     dword ptr [ecx+1000], eax
;         ret
;
; This is bad because the final store address is supposed to be the
; same as the load address in the loop, but it has clearly been
; over-optimized into a null pointer dereference.

; CHECK-LABEL: testPhi3
; CHECK: push [[EBX:.*]]
; CHECK: mov {{.*}}, dword ptr [esp
; CHECK: mov
; CHECK: mov {{.*}}[[ADDR:.*1000]]
; CHECK: cmp {{.*}}, 0
; CHECK: jne
; CHECK: mov {{.*}}[[ADDR]]
; CHECK: pop [[EBX]]
