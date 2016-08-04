; Translate with -fsanitize-address and -O2 to test alignment and ordering of
; redzones when allocas are coalesced.

; REQUIRES: no_minimal_build

; RUN: %p2i --filetype=obj --disassemble --target x8632 -i %s --args -O2 \
; RUN:     -allow-externally-defined-symbols -fsanitize-address | FileCheck %s

define internal i32 @func(i32 %arg1, i32 %arg2) {
  %l1 = alloca i8, i32 4, align 4
  %l2 = alloca i8, i32 5, align 1
  ret i32 42
}

; CHECK: func
; CHECK-NEXT: sub    esp,0xbc
; CHECK-NEXT: lea    eax,[esp+0x10]
; CHECK-NEXT: mov    DWORD PTR [esp],eax
; CHECK-NEXT: mov    DWORD PTR [esp+0x4],0x20
; CHECK-NEXT: mov    DWORD PTR [esp+0x8],0xffffffff
; CHECK-NEXT: __asan_poison
; CHECK-NEXT: lea    eax,[esp+0x74]
; CHECK-NEXT: mov    DWORD PTR [esp],eax
; CHECK-NEXT: mov    DWORD PTR [esp+0x4],0x3c
; CHECK-NEXT: mov    DWORD PTR [esp+0x8],0xffffffff
; CHECK-NEXT: __asan_poison
; CHECK-NEXT: lea    eax,[esp+0x35]
; CHECK-NEXT: mov    DWORD PTR [esp],eax
; CHECK-NEXT: mov    DWORD PTR [esp+0x4],0x3b
; CHECK-NEXT: mov    DWORD PTR [esp+0x8],0xffffffff
; CHECK-NEXT: __asan_poison
; CHECK-NEXT: lea    eax,[esp+0x74]
; CHECK-NEXT: mov    DWORD PTR [esp],eax
; CHECK-NEXT: mov    DWORD PTR [esp+0x4],0x3c
; CHECK-NEXT: __asan_unpoison
; CHECK-NEXT: lea    eax,[esp+0x35]
; CHECK-NEXT: mov    DWORD PTR [esp],eax
; CHECK-NEXT: mov    DWORD PTR [esp+0x4],0x3b
; CHECK-NEXT: __asan_unpoison
; CHECK-NEXT: lea    eax,[esp+0x10]
; CHECK-NEXT: mov    DWORD PTR [esp],eax
; CHECK-NEXT: mov    DWORD PTR [esp+0x4],0x20
; CHECK-NEXT: __asan_unpoison
; CHECK-NEXT: mov    eax,0x2a
; CHECK-NEXT: add    esp,0xbc
; CHECK-NEXT: ret
