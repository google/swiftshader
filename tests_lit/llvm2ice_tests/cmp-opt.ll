; RUIN: %llvm2ice %s | FileCheck %s
; RUIN: %llvm2ice --verbose none %s | FileCheck --check-prefix=ERRORS %s
; RUN: %szdiff --llvm2ice=%llvm2ice %s | FileCheck --check-prefix=DUMP %s

define void @testBool(i32 %a, i32 %b) {
entry:
  %cmp = icmp slt i32 %a, %b
  %cmp1 = icmp sgt i32 %a, %b
  br i1 %cmp, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  tail call void @use(i1 %cmp)
  br label %if.end

if.end:                                           ; preds = %if.then, %entry
  br i1 %cmp1, label %if.then5, label %if.end7

if.then5:                                         ; preds = %if.end
  tail call void @use(i1 %cmp1)
  br label %if.end7

if.end7:                                          ; preds = %if.then5, %if.end
  ret void
}

declare void @use(i1 zeroext)

; ERRORS-NOT: ICE translation error

; CHECK:      .globl testBool
; Two bool computations
; CHECK:      cmp
; CHECK:      cmp
; Test first bool
; CHECK:      cmp
; CHECK:      call
; Test second bool
; CHECK:      cmp
; CHECK:      call
; CHECK:      ret
; DUMP-NOT: SZ
