; RUIN: %llvm2ice -verbose inst %s | FileCheck %s
; RUIN: %llvm2ice --verbose none %s | FileCheck --check-prefix=ERRORS %s
; RUN: %szdiff --llvm2ice=%llvm2ice %s | FileCheck --check-prefix=DUMP %s

define void @foo() {
; CHECK: define void @foo()
entry:
  ret void
; CHECK: entry
; CHECK-NEXT: ret void
}

; ERRORS-NOT: ICE translation error
; DUMP-NOT: SZ
