; This is a basic test of the alloca instruction.

; RUN: %p2i --filetype=obj --disassemble -i %s --args -O2 | FileCheck %s
; RUN: %p2i --filetype=obj --disassemble -i %s --args -Om1 | FileCheck %s

define void @fixed_416_align_16(i32 %n) {
entry:
  %array = alloca i8, i32 416, align 16
  %__2 = ptrtoint i8* %array to i32
  call void @f1(i32 %__2)
  ret void
}
; CHECK-LABEL: fixed_416_align_16
; CHECK:      sub     esp,0x1a0
; CHECK:      sub     esp,0x10
; CHECK:      mov     DWORD PTR [esp],eax
; CHECK:      call {{.*}} R_{{.*}}    f1

define void @fixed_416_align_32(i32 %n) {
entry:
  %array = alloca i8, i32 400, align 32
  %__2 = ptrtoint i8* %array to i32
  call void @f1(i32 %__2)
  ret void
}
; CHECK-LABEL: fixed_416_align_32
; CHECK:      and     esp,0xffffffe0
; CHECK:      sub     esp,0x1a0
; CHECK:      sub     esp,0x10
; CHECK:      mov     DWORD PTR [esp],eax
; CHECK:      call {{.*}} R_{{.*}}    f1

define void @fixed_351_align_16(i32 %n) {
entry:
  %array = alloca i8, i32 351, align 16
  %__2 = ptrtoint i8* %array to i32
  call void @f1(i32 %__2)
  ret void
}
; CHECK-LABEL: fixed_351_align_16
; CHECK:      sub     esp,0x160
; CHECK:      sub     esp,0x10
; CHECK:      mov     DWORD PTR [esp],eax
; CHECK:      call {{.*}} R_{{.*}}    f1

define void @fixed_351_align_32(i32 %n) {
entry:
  %array = alloca i8, i32 351, align 32
  %__2 = ptrtoint i8* %array to i32
  call void @f1(i32 %__2)
  ret void
}
; CHECK-LABEL: fixed_351_align_32
; CHECK:      and     esp,0xffffffe0
; CHECK:      sub     esp,0x160
; CHECK:      sub     esp,0x10
; CHECK:      mov     DWORD PTR [esp],eax
; CHECK:      call {{.*}} R_{{.*}}    f1

declare void @f1(i32 %ignored)

define void @variable_n_align_16(i32 %n) {
entry:
  %array = alloca i8, i32 %n, align 16
  %__2 = ptrtoint i8* %array to i32
  call void @f2(i32 %__2)
  ret void
}
; CHECK-LABEL: variable_n_align_16
; CHECK:      mov     eax,DWORD PTR [ebp+0x8]
; CHECK:      add     eax,0xf
; CHECK:      and     eax,0xfffffff0
; CHECK:      sub     esp,eax
; CHECK:      sub     esp,0x10
; CHECK:      mov     DWORD PTR [esp],eax
; CHECK:      call {{.*}} R_{{.*}}    f2

define void @variable_n_align_32(i32 %n) {
entry:
  %array = alloca i8, i32 %n, align 32
  %__2 = ptrtoint i8* %array to i32
  call void @f2(i32 %__2)
  ret void
}
; In -O2, the order of the CHECK-DAG lines in the output is switched.
; CHECK-LABEL: variable_n_align_32
; CHECK-DAG:  and     esp,0xffffffe0
; CHECK-DAG:  mov     eax,DWORD PTR [ebp+0x8]
; CHECK:      add     eax,0x1f
; CHECK:      and     eax,0xffffffe0
; CHECK:      sub     esp,eax
; CHECK:      sub     esp,0x10
; CHECK:      mov     DWORD PTR [esp],eax
; CHECK:      call {{.*}} R_{{.*}}    f2

; Test alloca with default (0) alignment.
define void @align0(i32 %n) {
entry:
  %array = alloca i8, i32 %n
  %__2 = ptrtoint i8* %array to i32
  call void @f2(i32 %__2)
  ret void
}
; CHECK-LABEL: align0
; CHECK: add [[REG:.*]],0xf
; CHECK: and [[REG]],0xfffffff0
; CHECK: sub esp,[[REG]]

declare void @f2(i32 %ignored)