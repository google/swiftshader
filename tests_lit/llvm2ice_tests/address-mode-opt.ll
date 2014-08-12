; This file checks support for address mode optimization.

; RUN: %llvm2ice -O2 --verbose none %s | FileCheck %s
; RUN: %llvm2ice -O2 --verbose none %s \
; RUN:               | llvm-mc -arch=x86 -x86-asm-syntax=intel -filetype=obj
; RUN: %llvm2ice --verbose none %s | FileCheck --check-prefix=ERRORS %s

define float @load_arg_plus_200000(float* %arg) {
entry:
  %arg.int = ptrtoint float* %arg to i32
  %addr.int = add i32 %arg.int, 200000
  %addr.ptr = inttoptr i32 %addr.int to float*
  %addr.load = load float* %addr.ptr, align 4
  ret float %addr.load
; CHECK-LABEL: load_arg_plus_200000:
; CHECK: movss xmm0, dword ptr [eax+200000]
}

define float @load_200000_plus_arg(float* %arg) {
entry:
  %arg.int = ptrtoint float* %arg to i32
  %addr.int = add i32 200000, %arg.int
  %addr.ptr = inttoptr i32 %addr.int to float*
  %addr.load = load float* %addr.ptr, align 4
  ret float %addr.load
; CHECK-LABEL: load_200000_plus_arg:
; CHECK: movss xmm0, dword ptr [eax+200000]
}

define float @load_arg_minus_200000(float* %arg) {
entry:
  %arg.int = ptrtoint float* %arg to i32
  %addr.int = sub i32 %arg.int, 200000
  %addr.ptr = inttoptr i32 %addr.int to float*
  %addr.load = load float* %addr.ptr, align 4
  ret float %addr.load
; CHECK-LABEL: load_arg_minus_200000:
; CHECK: movss xmm0, dword ptr [eax-200000]
}

define float @load_200000_minus_arg(float* %arg) {
entry:
  %arg.int = ptrtoint float* %arg to i32
  %addr.int = sub i32 200000, %arg.int
  %addr.ptr = inttoptr i32 %addr.int to float*
  %addr.load = load float* %addr.ptr, align 4
  ret float %addr.load
; CHECK-LABEL: load_200000_minus_arg:
; CHECK: movss xmm0, dword ptr [eax]
}

define float @address_mode_opt_chaining(float* %arg) {
entry:
  %arg.int = ptrtoint float* %arg to i32
  %addr1.int = add i32 12, %arg.int
  %addr2.int = sub i32 %addr1.int, 4
  %addr2.ptr = inttoptr i32 %addr2.int to float*
  %addr2.load = load float* %addr2.ptr, align 4
  ret float %addr2.load
; CHECK-LABEL: address_mode_opt_chaining:
; CHECK: movss xmm0, dword ptr [eax+8]
}

; ERRORS-NOT: ICE translation error
