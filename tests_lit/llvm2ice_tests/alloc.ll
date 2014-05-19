; RUIN: %llvm2ice --verbose none %s | FileCheck %s
; RUIN: %llvm2ice --verbose none %s | FileCheck --check-prefix=ERRORS %s
; RUN: %llvm2iceinsts %s | %szdiff %s | FileCheck --check-prefix=DUMP %s
; RUN: %llvm2iceinsts --pnacl %s | %szdiff %s \
; RUN:                           | FileCheck --check-prefix=DUMP %s

define void @fixed_400(i32 %n) {
entry:
  %array = alloca i8, i32 400, align 16
  %__2 = ptrtoint i8* %array to i32
  call void @f1(i32 %__2)
  ret void
  ; CHECK:      sub     esp, 400
  ; CHECK-NEXT: mov     eax, esp
  ; CHECK-NEXT: push    eax
  ; CHECK-NEXT: call    f1
}

declare void @f1(i32)

define void @variable_n(i32 %n) {
entry:
  %array = alloca i8, i32 %n, align 16
  %__2 = ptrtoint i8* %array to i32
  call void @f2(i32 %__2)
  ret void
  ; CHECK:      mov     eax, dword ptr [ebp+8]
  ; CHECK-NEXT: sub     esp, eax
  ; CHECK-NEXT: mov     eax, esp
  ; CHECK-NEXT: push    eax
  ; CHECK-NEXT: call    f2
}

declare void @f2(i32)

; ERRORS-NOT: ICE translation error
; DUMP-NOT: SZ
