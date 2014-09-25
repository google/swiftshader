; RUN: %p2i -i %s --args --verbose inst | FileCheck %s
; RUN: %p2i -i %s --args --verbose none | FileCheck --check-prefix=ERRORS %s
; RUN: %p2i -i %s --insts | %szdiff %s | FileCheck --check-prefix=DUMP %s

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
