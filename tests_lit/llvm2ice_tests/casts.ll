; RUIN: %llvm2ice -verbose inst %s | FileCheck %s
; RUIN: %llvm2ice --verbose none %s | FileCheck --check-prefix=ERRORS %s
; RUN: %szdiff --llvm2ice=%llvm2ice %s | FileCheck --check-prefix=DUMP %s

define i64 @simple_zext(i32 %arg) {
entry:
  %c = zext i32 %arg to i64
  ret i64 %c

; CHECK:        entry:
; CHECK-NEXT:       %c = zext i32 %arg to i64
; CHECK-NEXT:       ret i64 %c
}

; ERRORS-NOT: ICE translation error
; DUMP-NOT: SZ
