; This tries to be a comprehensive test of f32 and f64 call/return ops.
; The CHECK lines are only checking for basic instruction patterns
; that should be present regardless of the optimization level, so
; there are no special OPTM1 match lines.

; RUN: %if --need=target_X8632 --command %p2i --filetype=obj --disassemble \
; RUN:   --target x8632 -i %s --args -O2 -allow-externally-defined-symbols \
; RUN:   | %if --need=target_X8632 --command FileCheck %s
; RUN: %if --need=target_X8632 --command %p2i --filetype=obj --disassemble \
; RUN:   --target x8632 -i %s --args -Om1 -allow-externally-defined-symbols \
; RUN:   | %if --need=target_X8632 --command FileCheck %s

; Can't test on ARM yet. Need to use several vpush {contiguous FP regs},
; instead of push {any GPR list}.

define internal i32 @doubleArgs(double %a, i32 %b, double %c) {
entry:
  ret i32 %b
}
; CHECK-LABEL: doubleArgs
; CHECK:      mov eax,DWORD PTR [esp+0xc]
; CHECK-NEXT: ret
; ARM32-LABEL: doubleArgs

define internal i32 @floatArgs(float %a, i32 %b, float %c) {
entry:
  ret i32 %b
}
; CHECK-LABEL: floatArgs
; CHECK:      mov eax,DWORD PTR [esp+0x8]
; CHECK-NEXT: ret

define internal i32 @passFpArgs(float %a, double %b, float %c, double %d, float %e, double %f) {
entry:
  %call = call i32 @ignoreFpArgsNoInline(float %a, i32 123, double %b)
  %call1 = call i32 @ignoreFpArgsNoInline(float %c, i32 123, double %d)
  %call2 = call i32 @ignoreFpArgsNoInline(float %e, i32 123, double %f)
  %add = add i32 %call1, %call
  %add3 = add i32 %add, %call2
  ret i32 %add3
}
; CHECK-LABEL: passFpArgs
; CHECK: mov DWORD PTR [esp+0x4],0x7b
; CHECK: call {{.*}} R_{{.*}} ignoreFpArgsNoInline
; CHECK: mov DWORD PTR [esp+0x4],0x7b
; CHECK: call {{.*}} R_{{.*}} ignoreFpArgsNoInline
; CHECK: mov DWORD PTR [esp+0x4],0x7b
; CHECK: call {{.*}} R_{{.*}} ignoreFpArgsNoInline

declare i32 @ignoreFpArgsNoInline(float %x, i32 %y, double %z)

define internal i32 @passFpConstArg(float %a, double %b) {
entry:
  %call = call i32 @ignoreFpArgsNoInline(float %a, i32 123, double 2.340000e+00)
  ret i32 %call
}
; CHECK-LABEL: passFpConstArg
; CHECK: mov DWORD PTR [esp+0x4],0x7b
; CHECK: call {{.*}} R_{{.*}} ignoreFpArgsNoInline

define internal i32 @passFp32ConstArg(float %a) {
entry:
  %call = call i32 @ignoreFp32ArgsNoInline(float %a, i32 123, float 2.0)
  ret i32 %call
}
; CHECK-LABEL: passFp32ConstArg
; CHECK: mov DWORD PTR [esp+0x4],0x7b
; CHECK: movss DWORD PTR [esp+0x8]
; CHECK: call {{.*}} R_{{.*}} ignoreFp32ArgsNoInline

declare i32 @ignoreFp32ArgsNoInline(float %x, i32 %y, float %z)

define internal float @returnFloatArg(float %a) {
entry:
  ret float %a
}
; CHECK-LABEL: returnFloatArg
; CHECK: fld DWORD PTR [esp

define internal double @returnDoubleArg(double %a) {
entry:
  ret double %a
}
; CHECK-LABEL: returnDoubleArg
; CHECK: fld QWORD PTR [esp

define internal float @returnFloatConst() {
entry:
  ret float 0x3FF3AE1480000000
}
; CHECK-LABEL: returnFloatConst
; CHECK: fld

define internal double @returnDoubleConst() {
entry:
  ret double 1.230000e+00
}
; CHECK-LABEL: returnDoubleConst
; CHECK: fld
