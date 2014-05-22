; Simple smoke test of the call instruction.  The assembly checks
; currently only verify the function labels.

; RUN: %llvm2ice --verbose inst %s | FileCheck %s
; RUN: %llvm2ice --verbose none %s | FileCheck --check-prefix=ERRORS %s
; RUN: %llvm2iceinsts %s | %szdiff %s | FileCheck --check-prefix=DUMP %s
; RUN: %llvm2iceinsts --pnacl %s | %szdiff %s \
; RUN:                           | FileCheck --check-prefix=DUMP %s

define i32 @fib(i32 %n) {
; CHECK: define i32 @fib
entry:
  %cmp = icmp slt i32 %n, 2
  br i1 %cmp, label %return, label %if.end

if.end:                                           ; preds = %entry
  %sub = add i32 %n, -1
  %call = tail call i32 @fib(i32 %sub)
  %sub1 = add i32 %n, -2
  %call2 = tail call i32 @fib(i32 %sub1)
  %add = add i32 %call2, %call
  ret i32 %add

return:                                           ; preds = %entry
  ret i32 %n
}

define i32 @fact(i32 %n) {
; CHECK: define i32 @fact
entry:
  %cmp = icmp slt i32 %n, 2
  br i1 %cmp, label %return, label %if.end

if.end:                                           ; preds = %entry
  %sub = add i32 %n, -1
  %call = tail call i32 @fact(i32 %sub)
  %mul = mul i32 %call, %n
  ret i32 %mul

return:                                           ; preds = %entry
  ret i32 %n
}

define i32 @redirect(i32 %n) {
; CHECK: define i32 @redirect
entry:
  %call = tail call i32 @redirect_target(i32 %n)
  ret i32 %call
}

declare i32 @redirect_target(i32)

define void @call_void(i32 %n) {
; CHECK: define void @call_void

entry:
  %cmp2 = icmp sgt i32 %n, 0
  br i1 %cmp2, label %if.then, label %if.end

if.then:                                          ; preds = %entry, %if.then
  %n.tr3 = phi i32 [ %call.i, %if.then ], [ %n, %entry ]
  %sub = add i32 %n.tr3, -1
  %call.i = tail call i32 @redirect_target(i32 %sub)
  %cmp = icmp sgt i32 %call.i, 0
  br i1 %cmp, label %if.then, label %if.end

if.end:                                           ; preds = %if.then, %entry
  ret void
}

; ERRORS-NOT: ICE translation error
; DUMP-NOT: SZ
