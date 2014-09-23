; This checks to ensure that Subzero aligns spill slots.

; RUN: %llvm2ice --verbose none %s \
; RUN:   | llvm-mc -triple=i686-none-nacl -x86-asm-syntax=intel -filetype=obj \
; RUN:   | llvm-objdump -d --symbolize -x86-asm-syntax=intel - | FileCheck %s
; RUN: %llvm2ice -O2 --verbose none %s \
; RUN:   | llvm-mc -triple=i686-none-nacl -x86-asm-syntax=intel -filetype=obj \
; RUN:   | llvm-objdump -d --symbolize -x86-asm-syntax=intel - | FileCheck %s
; RUN: %llvm2ice --verbose none %s | FileCheck --check-prefix=ERRORS %s

; The location of the stack slot for a variable is inferred from the
; return sequence.

; In this file, "global" refers to a variable with a live range across
; multiple basic blocks (not an LLVM global variable) and "local"
; refers to a variable that is live in only a single basic block.

define <4 x i32> @align_global_vector(i32 %arg) {
entry:
  %vec.global = insertelement <4 x i32> undef, i32 %arg, i32 0
  br label %block
block:
  call void @ForceXmmSpills()
  ret <4 x i32> %vec.global
; CHECK-LABEL: align_global_vector:
; CHECK: movups xmm0, xmmword ptr [esp]
; CHECK-NEXT: add esp, 28
; CHECK-NEXT: ret
}

define <4 x i32> @align_local_vector(i32 %arg) {
entry:
  br label %block
block:
  %vec.local = insertelement <4 x i32> undef, i32 %arg, i32 0
  call void @ForceXmmSpills()
  ret <4 x i32> %vec.local
; CHECK-LABEL: align_local_vector:
; CHECK: movups xmm0, xmmword ptr [esp]
; CHECK-NEXT: add esp, 28
; CHECK-NEXT: ret
}

declare void @ForceXmmSpills()

define <4 x i32> @align_global_vector_ebp_based(i32 %arg) {
entry:
  %alloc = alloca i8, i32 1, align 1
  %vec.global = insertelement <4 x i32> undef, i32 %arg, i32 0
  br label %block
block:
  call void @ForceXmmSpillsAndUseAlloca(i8* %alloc)
  ret <4 x i32> %vec.global
; CHECK-LABEL: align_global_vector_ebp_based:
; CHECK: movups xmm0, xmmword ptr [ebp - 24]
; CHECK-NEXT: mov esp, ebp
; CHECK-NEXT: pop ebp
; CHECK: ret
}

define <4 x i32> @align_local_vector_ebp_based(i32 %arg) {
entry:
  %alloc = alloca i8, i32 1, align 1
  %vec.local = insertelement <4 x i32> undef, i32 %arg, i32 0
  call void @ForceXmmSpillsAndUseAlloca(i8* %alloc)
  ret <4 x i32> %vec.local
; CHECK-LABEL: align_local_vector_ebp_based:
; CHECK: movups xmm0, xmmword ptr [ebp - 24]
; CHECK-NEXT: mov esp, ebp
; CHECK-NEXT: pop ebp
; CHECK: ret
}

define <4 x i32> @align_local_vector_and_global_float(i32 %arg) {
entry:
  %float.global = sitofp i32 %arg to float
  call void @ForceXmmSpillsAndUseFloat(float %float.global)
  br label %block
block:
  %vec.local = insertelement <4 x i32> undef, i32 undef, i32 0
  call void @ForceXmmSpillsAndUseFloat(float %float.global)
  ret <4 x i32> %vec.local
; CHECK-LABEL: align_local_vector_and_global_float:
; CHECK: cvtsi2ss xmm0, eax
; CHECK-NEXT: movss dword ptr [esp + {{12|28}}], xmm0
; CHECK: movups xmm0, xmmword ptr [{{esp|esp \+ 16}}]
; CHECK-NEXT: add esp, 44
; CHECK-NEXT: ret
}

declare void @ForceXmmSpillsAndUseAlloca(i8*)
declare void @ForceXmmSpillsAndUseFloat(float)

; ERRORS-NOT: ICE translation error
