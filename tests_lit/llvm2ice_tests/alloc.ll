; This is a basic test of the alloca instruction.

; RUN: %p2i -i %s --args -O2 --verbose none \
; RUN:   | llvm-mc -triple=i686-none-nacl -x86-asm-syntax=intel -filetype=obj \
; RUN:   | llvm-objdump -d --symbolize -x86-asm-syntax=intel - | FileCheck %s
; RUN: %p2i -i %s --args -Om1 --verbose none \
; RUN:   | llvm-mc -triple=i686-none-nacl -x86-asm-syntax=intel -filetype=obj \
; RUN:   | llvm-objdump -d --symbolize -x86-asm-syntax=intel - | FileCheck %s
; RUN: %p2i -i %s --args --verbose none | FileCheck --check-prefix=ERRORS %s
; TODO(kschimpf) Find out why lc2i is needed.
; RUN: %lc2i -i %s --insts | %szdiff %s | FileCheck --check-prefix=DUMP %s

define void @fixed_416_align_16(i32 %n) {
entry:
  %array = alloca i8, i32 416, align 16
  %__2 = ptrtoint i8* %array to i32
  call void @f1(i32 %__2)
  ret void
}
; CHECK-LABEL: fixed_416_align_16:
; CHECK:      sub     esp, 416
; CHECK:      sub     esp, 16
; CHECK:      mov     dword ptr [esp], eax
; CHECK:      call    f1

define void @fixed_416_align_32(i32 %n) {
entry:
  %array = alloca i8, i32 400, align 32
  %__2 = ptrtoint i8* %array to i32
  call void @f1(i32 %__2)
  ret void
}
; CHECK-LABEL: fixed_416_align_32:
; CHECK:      and     esp, -32
; CHECK:      sub     esp, 416
; CHECK:      sub     esp, 16
; CHECK:      mov     dword ptr [esp], eax
; CHECK:      call    f1

define void @fixed_351_align_16(i32 %n) {
entry:
  %array = alloca i8, i32 351, align 16
  %__2 = ptrtoint i8* %array to i32
  call void @f1(i32 %__2)
  ret void
}
; CHECK-LABEL: fixed_351_align_16:
; CHECK:      sub     esp, 352
; CHECK:      sub     esp, 16
; CHECK:      mov     dword ptr [esp], eax
; CHECK:      call    f1

define void @fixed_351_align_32(i32 %n) {
entry:
  %array = alloca i8, i32 351, align 32
  %__2 = ptrtoint i8* %array to i32
  call void @f1(i32 %__2)
  ret void
}
; CHECK-LABEL: fixed_351_align_32:
; CHECK:      and     esp, -32
; CHECK:      sub     esp, 352
; CHECK:      sub     esp, 16
; CHECK:      mov     dword ptr [esp], eax
; CHECK:      call    f1

define void @f1(i32 %ignored) {
entry:
  ret void
}

define void @variable_n_align_16(i32 %n) {
entry:
  %array = alloca i8, i32 %n, align 16
  %__2 = ptrtoint i8* %array to i32
  call void @f2(i32 %__2)
  ret void
}
; CHECK-LABEL: variable_n_align_16:
; CHECK:      mov     eax, dword ptr [ebp + 8]
; CHECK:      add     eax, 15
; CHECK:      and     eax, -16
; CHECK:      sub     esp, eax
; CHECK:      sub     esp, 16
; CHECK:      mov     dword ptr [esp], eax
; CHECK:      call    f2

define void @variable_n_align_32(i32 %n) {
entry:
  %array = alloca i8, i32 %n, align 32
  %__2 = ptrtoint i8* %array to i32
  call void @f2(i32 %__2)
  ret void
}
; In -O2, the order of the CHECK-DAG lines in the output is switched.
; CHECK-LABEL: variable_n_align_32:
; CHECK-DAG:  and     esp, -32
; CHECK-DAG:  mov     eax, dword ptr [ebp + 8]
; CHECK:      add     eax, 31
; CHECK:      and     eax, -32
; CHECK:      sub     esp, eax
; CHECK:      sub     esp, 16
; CHECK:      mov     dword ptr [esp], eax
; CHECK:      call    f2

; Test alloca with default (0) alignment.
define void @align0(i32 %n) {
entry:
  %array = alloca i8, i32 %n
  %__2 = ptrtoint i8* %array to i32
  call void @f2(i32 %__2)
  ret void
}
; CHECK-LABEL: align0
; CHECK: add [[REG:.*]], 15
; CHECK: and [[REG]], -16
; CHECK: sub esp, [[REG]]

define void @f2(i32 %ignored) {
entry:
  ret void
}

; ERRORS-NOT: ICE translation error
; DUMP-NOT: SZ
