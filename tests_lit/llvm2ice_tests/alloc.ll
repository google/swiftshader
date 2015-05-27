; This is a basic test of the alloca instruction.

; RUN: %if --need=target_X8632 --command %p2i --filetype=obj --disassemble \
; RUN:   --target x8632 -i %s --args -O2 \
; RUN:   | %if --need=target_X8632 --command FileCheck %s

; RUN: %if --need=target_X8632 --command %p2i --filetype=obj --disassemble \
; RUN:   --target x8632 -i %s --args -Om1 \
; RUN:   | %if --need=target_X8632 --command FileCheck %s

; TODO(jvoung): Stop skipping unimplemented parts (via --skip-unimplemented)
; once enough infrastructure is in. Also, switch to --filetype=obj
; when possible.
; RUN: %if --need=target_ARM32 --command %p2i --filetype=asm --assemble \
; RUN:   --disassemble --target arm32 -i %s --args -O2 --skip-unimplemented \
; RUN:   | %if --need=target_ARM32 --command FileCheck --check-prefix ARM32 %s

; RUN: %if --need=target_ARM32 --command %p2i --filetype=asm --assemble \
; RUN:   --disassemble --target arm32 -i %s --args -Om1 --skip-unimplemented \
; RUN:   | %if --need=target_ARM32 --command FileCheck --check-prefix ARM32 %s

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

; ARM32-LABEL: fixed_416_align_16
; ARM32:      sub sp, sp, #416
; ARM32:      bl {{.*}} R_{{.*}}    f1

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

; ARM32-LABEL: fixed_416_align_32
; ARM32:      bic sp, sp, #31
; ARM32:      sub sp, sp, #416
; ARM32:      bl {{.*}} R_{{.*}}    f1

; Show that the amount to allocate will be rounded up.
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

; ARM32-LABEL: fixed_351_align_16
; ARM32:      sub sp, sp, #352
; ARM32:      bl {{.*}} R_{{.*}}    f1

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

; ARM32-LABEL: fixed_351_align_32
; ARM32:      bic sp, sp, #31
; ARM32:      sub sp, sp, #352
; ARM32:      bl {{.*}} R_{{.*}}    f1

declare void @f1(i32 %ignored)

declare void @f2(i32 %ignored)

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

; ARM32-LABEL: variable_n_align_16
; ARM32:      add r0, r0, #15
; ARM32:      bic r0, r0, #15
; ARM32:      sub sp, sp, r0
; ARM32:      bl {{.*}} R_{{.*}}    f2

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

; ARM32-LABEL: variable_n_align_32
; ARM32:      bic sp, sp, #31
; ARM32:      add r0, r0, #31
; ARM32:      bic r0, r0, #31
; ARM32:      sub sp, sp, r0
; ARM32:      bl {{.*}} R_{{.*}}    f2

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

; ARM32-LABEL: align0
; ARM32: add r0, r0, #15
; ARM32: bic r0, r0, #15
; ARM32: sub sp, sp, r0

; Test a large alignment where a mask might not fit in an immediate
; field of an instruction for some architectures.
define void @align1MB(i32 %n) {
entry:
  %array = alloca i8, i32 %n, align 1048576
  %__2 = ptrtoint i8* %array to i32
  call void @f2(i32 %__2)
  ret void
}
; CHECK-LABEL: align1MB
; CHECK: and esp,0xfff00000
; CHECK: add [[REG:.*]],0xfffff
; CHECK: and [[REG]],0xfff00000
; CHECK: sub esp,[[REG]]

; ARM32-LABEL: align1MB
; ARM32: movw [[REG:.*]], #0
; ARM32: movt [[REG]], #65520 ; 0xfff0
; ARM32: and sp, sp, [[REG]]
; ARM32: movw [[REG2:.*]], #65535 ; 0xffff
; ARM32: movt [[REG2]], #15
; ARM32: add r0, r0, [[REG2]]
; ARM32: movw [[REG3:.*]], #0
; ARM32: movt [[REG3]], #65520 ; 0xfff0
; ARM32: and r0, r0, [[REG3]]
; ARM32: sub sp, sp, r0

; Test a large alignment where a mask might still fit in an immediate
; field of an instruction for some architectures.
define void @align512MB(i32 %n) {
entry:
  %array = alloca i8, i32 %n, align 536870912
  %__2 = ptrtoint i8* %array to i32
  call void @f2(i32 %__2)
  ret void
}
; CHECK-LABEL: align512MB
; CHECK: and esp,0xe0000000
; CHECK: add [[REG:.*]],0x1fffffff
; CHECK: and [[REG]],0xe0000000
; CHECK: sub esp,[[REG]]

; ARM32-LABEL: align512MB
; ARM32: and sp, sp, #-536870912 ; 0xe0000000
; ARM32: mvn [[REG:.*]], #-536870912 ; 0xe0000000
; ARM32: add r0, r0, [[REG]]
; ARM32: and r0, r0, #-536870912 ; 0xe0000000
; ARM32: sub sp, sp, r0
