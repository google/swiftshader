; RUIN: %llvm2ice -verbose inst %s | FileCheck %s
; RUIN: %llvm2ice --verbose none %s | FileCheck --check-prefix=ERRORS %s
; RUN: %szdiff --llvm2ice=%llvm2ice %s | FileCheck --check-prefix=DUMP %s

define void @dummy_inttoptr(i32 %addr_arg) {
entry:
  %ptr = inttoptr i32 %addr_arg to i32*
  ret void
; CHECK: %ptr = i32 %addr_arg
}

; ERRORS-NOT: ICE translation error
; DUMP-NOT: SZ
