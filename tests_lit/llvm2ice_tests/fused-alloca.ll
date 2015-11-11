; This is a basic test of the alloca instruction.

; RUN: %if --need=target_X8632 --command %p2i --filetype=obj --disassemble \
; RUN:   --target x8632 -i %s --args -O2 -allow-externally-defined-symbols \
; RUN:   | %if --need=target_X8632 --command FileCheck %s

; Test that a sequence of allocas with less than stack alignment get fused.
define internal void @fused_small_align(i32 %arg) {
entry:
  %a1 = alloca i8, i32 8, align 4
  %a2 = alloca i8, i32 12, align 4
  %a3 = alloca i8, i32 16, align 8
  %p1 = bitcast i8* %a1 to i32*
  %p2 = bitcast i8* %a2 to i32*
  %p3 = bitcast i8* %a3 to i32*
  store i32 %arg, i32* %p1, align 1
  store i32 %arg, i32* %p2, align 1
  store i32 %arg, i32* %p3, align 1
  ret void
}
; CHECK-LABEL: fused_small_align
; CHECK-NEXT: sub    esp,0xc
; CHECK-NEXT: mov    eax,DWORD PTR [esp+0x10]
; CHECK-NEXT: sub    esp,0x30
; CHECK-NEXT: mov    {{.*}},esp
; CHECK-NEXT: mov    DWORD PTR [esp+0x10],eax
; CHECK-NEXT: mov    DWORD PTR [esp+0x18],eax
; CHECK-NEXT: mov    DWORD PTR [esp],eax
; CHECK-NEXT: add    esp,0x3c

; Test that a sequence of allocas with greater than stack alignment get fused.
define internal void @fused_large_align(i32 %arg) {
entry:
  %a1 = alloca i8, i32 8, align 32
  %a2 = alloca i8, i32 12, align 64
  %a3 = alloca i8, i32 16, align 32
  %p1 = bitcast i8* %a1 to i32*
  %p2 = bitcast i8* %a2 to i32*
  %p3 = bitcast i8* %a3 to i32*
  store i32 %arg, i32* %p1, align 1
  store i32 %arg, i32* %p2, align 1
  store i32 %arg, i32* %p3, align 1
  ret void
}
; CHECK-LABEL: fused_large_align
; CHECK-NEXT: push   ebp
; CHECK-NEXT: mov    ebp,esp
; CHECK-NEXT: sub    esp,0x8
; CHECK-NEXT: mov    eax,DWORD PTR [ebp+0x8]
; CHECK-NEXT: and    esp,0xffffffc0
; CHECK-NEXT: sub    esp,0x80
; CHECK-NEXT: mov    ecx,esp
; CHECK-NEXT: mov    DWORD PTR [esp+0x40],eax
; CHECK-NEXT: mov    DWORD PTR [esp],eax
; CHECK-NEXT: mov    DWORD PTR [esp+0x60],eax
; CHECK-NEXT: mov    esp,ebp
; CHECK-NEXT: pop    ebp
