; This is a basic test of the alloca instruction and a call.

; RUN: %if --need=target_X8632 --command %p2i --filetype=obj --disassemble \
; RUN:   --target x8632 -i %s --args -O2 -allow-externally-defined-symbols \
; RUN:   | %if --need=target_X8632 --command FileCheck %s

declare void @copy(i32 %arg1, i8* %arr1, i8* %arr2, i8* %arr3, i8* %arr4);

; Test that alloca base addresses get passed correctly to functions.
define internal void @caller1(i32 %arg) {
entry:
  %a1 = alloca i8, i32 32, align 4
  %p1 = bitcast i8* %a1 to i32*
  store i32 %arg, i32* %p1, align 1
  call void @copy(i32 %arg, i8* %a1, i8* %a1, i8* %a1, i8* %a1)
  ret void
}

; CHECK-LABEL:  caller1
; CHECK-NEXT:   sub    esp,0xc
; CHECK-NEXT:   mov    eax,DWORD PTR [esp+0x10]
; CHECK-NEXT:   sub    esp,0x20
; CHECK-NEXT:   mov    ecx,esp
; CHECK-NEXT:   mov    DWORD PTR [esp],eax
; CHECK-NEXT:   sub    esp,0x20
; CHECK-NEXT:   mov    DWORD PTR [esp],eax
; CHECK-NEXT:   lea    eax,[esp+0x20]
; CHECK-NEXT:   mov    DWORD PTR [esp+0x4],eax
; CHECK-NEXT:   lea    eax,[esp+0x20]
; CHECK-NEXT:   mov    DWORD PTR [esp+0x8],eax
; CHECK-NEXT:   lea    eax,[esp+0x20]
; CHECK-NEXT:   mov    DWORD PTR [esp+0xc],eax
; CHECK-NEXT:   lea    eax,[esp+0x20]
; CHECK-NEXT:   mov    DWORD PTR [esp+0x10],eax
; CHECK-NEXT:   call
; CHECK-NEXT:   add    esp,0x20
; CHECK-NEXT:   add    esp,0x2c
; CHECK-NEXT:   ret

; Test that alloca base addresses get passed correctly to functions.
define internal void @caller2(i32 %arg) {
entry:
  %a1 = alloca i8, i32 32, align 4
  %a2 = alloca i8, i32 32, align 4
  %p1 = bitcast i8* %a1 to i32*
  %p2 = bitcast i8* %a2 to i32*
  store i32 %arg, i32* %p1, align 1
  store i32 %arg, i32* %p2, align 1
  call void @copy(i32 %arg, i8* %a1, i8* %a2, i8* %a1, i8* %a2)
  ret void
}

; CHECK-LABEL:  caller2
; CHECK-NEXT:   sub    esp,0xc
; CHECK-NEXT:   mov    eax,DWORD PTR [esp+0x10]
; CHECK-NEXT:   sub    esp,0x40
; CHECK-NEXT:   mov    ecx,esp
; CHECK-NEXT:   mov    DWORD PTR [esp],eax
; CHECK-NEXT:   mov    DWORD PTR [esp+0x20],eax
; CHECK-NEXT:   sub    esp,0x20
; CHECK-NEXT:   mov    DWORD PTR [esp],eax
; CHECK-NEXT:   lea    eax,[esp+0x20]
; CHECK-NEXT:   mov    DWORD PTR [esp+0x4],eax
; CHECK-NEXT:   lea    eax,[esp+0x40]
; CHECK-NEXT:   mov    DWORD PTR [esp+0x8],eax
; CHECK-NEXT:   lea    eax,[esp+0x20]
; CHECK-NEXT:   mov    DWORD PTR [esp+0xc],eax
; CHECK-NEXT:   lea    eax,[esp+0x40]
; CHECK-NEXT:   mov    DWORD PTR [esp+0x10],eax
; CHECK-NEXT:   call
; CHECK-NEXT:   add    esp,0x20
; CHECK-NEXT:   add    esp,0x4c
; CHECK-NEXT:   ret
