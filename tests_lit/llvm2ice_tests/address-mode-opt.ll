; This file checks support for address mode optimization.

; RUN: %llvm2ice -O2 --verbose none %s \
; RUN:   | llvm-mc -triple=i686-none-nacl -x86-asm-syntax=intel -filetype=obj \
; RUN:   | llvm-objdump -d --symbolize -x86-asm-syntax=intel - | FileCheck %s
; RUN: %llvm2ice --verbose none %s | FileCheck --check-prefix=ERRORS %s

define float @load_arg_plus_200000(float* %arg) {
entry:
  %arg.int = ptrtoint float* %arg to i32
  %addr.int = add i32 %arg.int, 200000
  %addr.ptr = inttoptr i32 %addr.int to float*
  %addr.load = load float* %addr.ptr, align 4
  ret float %addr.load
; CHECK-LABEL: load_arg_plus_200000:
; CHECK: movss xmm0, dword ptr [eax + 200000]
}

define float @load_200000_plus_arg(float* %arg) {
entry:
  %arg.int = ptrtoint float* %arg to i32
  %addr.int = add i32 200000, %arg.int
  %addr.ptr = inttoptr i32 %addr.int to float*
  %addr.load = load float* %addr.ptr, align 4
  ret float %addr.load
; CHECK-LABEL: load_200000_plus_arg:
; CHECK: movss xmm0, dword ptr [eax + 200000]
}

define float @load_arg_minus_200000(float* %arg) {
entry:
  %arg.int = ptrtoint float* %arg to i32
  %addr.int = sub i32 %arg.int, 200000
  %addr.ptr = inttoptr i32 %addr.int to float*
  %addr.load = load float* %addr.ptr, align 4
  ret float %addr.load
; CHECK-LABEL: load_arg_minus_200000:
; CHECK: movss xmm0, dword ptr [eax - 200000]
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
; CHECK: movss xmm0, dword ptr [eax + 8]
}

define float @address_mode_opt_chaining_overflow(float* %arg) {
entry:
  %arg.int = ptrtoint float* %arg to i32
  %addr1.int = add i32 2147483640, %arg.int
  %addr2.int = add i32 %addr1.int, 2147483643
  %addr2.ptr = inttoptr i32 %addr2.int to float*
  %addr2.load = load float* %addr2.ptr, align 4
  ret float %addr2.load
; CHECK-LABEL: address_mode_opt_chaining_overflow:
; CHECK: 2147483640
; CHECK: movss xmm0, dword ptr [{{.*}} + 2147483643]
}

define float @address_mode_opt_chaining_overflow_sub(float* %arg) {
entry:
  %arg.int = ptrtoint float* %arg to i32
  %addr1.int = sub i32 %arg.int, 2147483640
  %addr2.int = sub i32 %addr1.int, 2147483643
  %addr2.ptr = inttoptr i32 %addr2.int to float*
  %addr2.load = load float* %addr2.ptr, align 4
  ret float %addr2.load
; CHECK-LABEL: address_mode_opt_chaining_overflow_sub:
; CHECK: 2147483640
; CHECK: movss xmm0, dword ptr [{{.*}} - 2147483643]
}

define float @address_mode_opt_chaining_no_overflow(float* %arg) {
entry:
  %arg.int = ptrtoint float* %arg to i32
  %addr1.int = sub i32 %arg.int, 2147483640
  %addr2.int = add i32 %addr1.int, 2147483643
  %addr2.ptr = inttoptr i32 %addr2.int to float*
  %addr2.load = load float* %addr2.ptr, align 4
  ret float %addr2.load
; CHECK-LABEL: address_mode_opt_chaining_no_overflow:
; CHECK: movss xmm0, dword ptr [{{.*}} + 3]
}

define float @address_mode_opt_add_pos_min_int(float* %arg) {
entry:
  %arg.int = ptrtoint float* %arg to i32
  %addr1.int = add i32 %arg.int, 2147483648
  %addr1.ptr = inttoptr i32 %addr1.int to float*
  %addr1.load = load float* %addr1.ptr, align 4
  ret float %addr1.load
; CHECK-LABEL: address_mode_opt_add_pos_min_int:
; CHECK: movss xmm0, dword ptr [{{.*}} - 2147483648]
}

define float @address_mode_opt_sub_min_int(float* %arg) {
entry:
  %arg.int = ptrtoint float* %arg to i32
  %addr1.int = sub i32 %arg.int, 2147483648
  %addr1.ptr = inttoptr i32 %addr1.int to float*
  %addr1.load = load float* %addr1.ptr, align 4
  ret float %addr1.load
; CHECK-LABEL: address_mode_opt_sub_min_int:
; CHECK: movss xmm0, dword ptr [{{.*}} - 2147483648]
}



; ERRORS-NOT: ICE translation error
