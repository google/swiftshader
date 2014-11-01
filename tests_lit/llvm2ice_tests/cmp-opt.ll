; Simple test of non-fused compare/branch.

; RUN: %p2i -i %s --args -O2 --verbose none \
; RUN:   | llvm-mc -triple=i686-none-nacl -filetype=obj \
; RUN:   | llvm-objdump -d --symbolize -x86-asm-syntax=intel - | FileCheck %s
; RUN: %p2i -i %s --args -Om1 --verbose none \
; RUN:   | llvm-mc -triple=i686-none-nacl -filetype=obj \
; RUN:   | llvm-objdump -d --symbolize -x86-asm-syntax=intel - \
; RUN:   | FileCheck --check-prefix=OPTM1 %s
; RUN: %p2i -i %s --args --verbose none | FileCheck --check-prefix=ERRORS %s
; RUN: %p2i -i %s --insts | %szdiff %s | FileCheck --check-prefix=DUMP %s

define void @testBool(i32 %a, i32 %b) {
entry:
  %cmp = icmp slt i32 %a, %b
  %cmp1 = icmp sgt i32 %a, %b
  br i1 %cmp, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  %cmp_ext = zext i1 %cmp to i32
  tail call void @use(i32 %cmp_ext)
  br label %if.end

if.end:                                           ; preds = %if.then, %entry
  br i1 %cmp1, label %if.then5, label %if.end7

if.then5:                                         ; preds = %if.end
  %cmp1_ext = zext i1 %cmp1 to i32
  tail call void @use(i32 %cmp1_ext)
  br label %if.end7

if.end7:                                          ; preds = %if.then5, %if.end
  ret void
}

declare void @use(i32)

; CHECK-LABEL: testBool
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
;
; OPTM1-LABEL: testBool
; Two bool computations
; OPTM1:      cmp
; OPTM1:      cmp
; Test first bool
; OPTM1:      cmp
; OPTM1:      call
; Test second bool
; OPTM1:      cmp
; OPTM1:      call
; OPTM1:      ret

; ERRORS-NOT: ICE translation error
; DUMP-NOT: SZ
