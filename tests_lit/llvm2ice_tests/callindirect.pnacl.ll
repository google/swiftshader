; Test of multiple indirect calls to the same target.  Each call
; should be to the same operand, whether it's in a register or on the
; stack.

; RUN: %p2i -i %s --args -O2 --verbose none \
; RUN:   | llvm-mc -triple=i686-none-nacl -filetype=obj \
; RUN:   | llvm-objdump -d --symbolize -x86-asm-syntax=intel - | FileCheck %s
; RUN: %p2i -i %s --args -Om1 --verbose none \
; RUN:   | llvm-mc -triple=i686-none-nacl -filetype=obj \
; RUN:   | llvm-objdump -d --symbolize -x86-asm-syntax=intel - \
; RUN:   | FileCheck --check-prefix=OPTM1 %s
; RUN: %p2i -i %s --args --verbose none | FileCheck --check-prefix=ERRORS %s

@__init_array_start = internal constant [0 x i8] zeroinitializer, align 4
@__fini_array_start = internal constant [0 x i8] zeroinitializer, align 4
@__tls_template_start = internal constant [0 x i8] zeroinitializer, align 8
@__tls_template_alignment = internal constant [4 x i8] c"\01\00\00\00", align 4

define internal void @CallIndirect(i32 %f) {
entry:
  %__1 = inttoptr i32 %f to void ()*
  call void %__1()
  call void %__1()
  call void %__1()
  call void %__1()
  call void %__1()
  ret void
}
; CHECK-LABEL: CallIndirect
; CHECK: call [[REGISTER:[a-z]+]]
; CHECK: call [[REGISTER]]
; CHECK: call [[REGISTER]]
; CHECK: call [[REGISTER]]
; CHECK: call [[REGISTER]]
;
; OPTM1-LABEL: CallIndirect
; OPTM1: call [[TARGET:.+]]
; OPTM1: call [[TARGET]]
; OPTM1: call [[TARGET]]
; OPTM1: call [[TARGET]]
; OPTM1: call [[TARGET]]

@fp_v = internal global [4 x i8] zeroinitializer, align 4

define internal void @CallIndirectGlobal() {
entry:
  %fp_ptr_i32 = bitcast [4 x i8]* @fp_v to i32*
  %fp_ptr = load i32* %fp_ptr_i32, align 1
  %fp = inttoptr i32 %fp_ptr to void ()*
  call void %fp()
  call void %fp()
  call void %fp()
  call void %fp()
  ret void
}
; CHECK-LABEL: CallIndirectGlobal
; CHECK: call [[REGISTER:[a-z]+]]
; CHECK: call [[REGISTER]]
; CHECK: call [[REGISTER]]
; CHECK: call [[REGISTER]]
;
; OPTM1-LABEL: CallIndirectGlobal
; OPTM1: call [[TARGET:.+]]
; OPTM1: call [[TARGET]]
; OPTM1: call [[TARGET]]
; OPTM1: call [[TARGET]]

; Calling an absolute address is used for non-IRT PNaCl pexes to directly
; access syscall trampolines. Do we need to support this?
; define internal void @CallIndirectConst() {
; entry:
;   %__1 = inttoptr i32 66496 to void ()*
;   call void %__1()
;   call void %__1()
;   call void %__1()
;   call void %__1()
;   call void %__1()
;   ret void
; }

; ERRORS-NOT: ICE translation error
