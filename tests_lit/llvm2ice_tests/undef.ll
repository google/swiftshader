; This test checks that undef values are represented as zero.

; RUN: %llvm2ice --verbose none %s | FileCheck  %s
; RUN: %llvm2ice -O2 --verbose none %s | FileCheck  %s
; RUN: %llvm2ice --verbose none %s | FileCheck --check-prefix=ERRORS %s
; RUN: %llvm2iceinsts %s | %szdiff %s | FileCheck --check-prefix=DUMP %s
; RUN: %llvm2iceinsts --pnacl %s | %szdiff %s \
; RUN:                           | FileCheck --check-prefix=DUMP %s

define i32 @undefi32() {
entry:
; CHECK-LABEL: undefi32:
  ret i32 undef
; CHECK: mov eax, 0
; CHECK: ret
}

define i64 @undefi64() {
entry:
; CHECK-LABEL: undefi64:
  ret i64 undef
; CHECK-DAG: mov eax, 0
; CHECK-DAG: mov edx, 0
; CHECK: ret
}

define float @undeffloat() {
entry:
; CHECK-LABEL: undeffloat:
  ret float undef
; CHECK-NOT: sub esp
; CHECK: fld
; CHECK: ret
}

; ERRORS-NOT: ICE translation error
; DUMP-NOT: SZ
