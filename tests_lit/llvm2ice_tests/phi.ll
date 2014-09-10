; This tests some of the subtleties of Phi lowering.  In particular,
; it tests that it does the right thing when it tries to enable
; compare/branch fusing.

; RUN: %llvm2ice -O2 --verbose none --no-phi-edge-split %s \
; RUN:   | llvm-mc -triple=i686-none-nacl -x86-asm-syntax=intel -filetype=obj \
; RUN:   | llvm-objdump -d -symbolize -x86-asm-syntax=intel - | FileCheck %s
; RUN: %llvm2ice --verbose none %s | FileCheck --check-prefix=ERRORS %s
; RUN: %llvm2iceinsts %s | %szdiff %s | FileCheck --check-prefix=DUMP %s
; RUN: %llvm2iceinsts --pnacl %s | %szdiff %s \
; RUN:                           | FileCheck --check-prefix=DUMP %s

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
; CHECK: jne
; CHECK: :
; CHECK: mov [[PHI]], 0
; CHECK: :
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
; CHECK-NEXT: jg
; CHECK: :
; CHECK: mov [[PHI:.*]], 54321
; CHECK: :
; CHECK: mov {{.*}}, [[PHI]]

; ERRORS-NOT: ICE translation error
; DUMP-NOT: SZ
