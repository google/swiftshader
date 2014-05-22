; Simple test of the store instruction.

; RUN: %llvm2ice --verbose inst %s | FileCheck %s
; RUN: %llvm2ice --verbose none %s | FileCheck --check-prefix=ERRORS %s
; RUN: %llvm2iceinsts %s | %szdiff %s | FileCheck --check-prefix=DUMP %s
; RUN: %llvm2iceinsts --pnacl %s | %szdiff %s \
; RUN:                           | FileCheck --check-prefix=DUMP %s

define void @store_i64(i32 %addr_arg) {
entry:
  %__1 = inttoptr i32 %addr_arg to i64*
  store i64 1, i64* %__1, align 1
  ret void

; CHECK:       Initial CFG
; CHECK:       %__1 = i32 %addr_arg
; CHECK-NEXT:  store i64 1, {{.*}}, align 1
; CHECK-NEXT:  ret void
}

define void @store_i32(i32 %addr_arg) {
entry:
  %__1 = inttoptr i32 %addr_arg to i32*
  store i32 1, i32* %__1, align 1
  ret void

; CHECK:       Initial CFG
; CHECK:       %__1 = i32 %addr_arg
; CHECK-NEXT:  store i32 1, {{.*}}, align 1
; CHECK-NEXT:  ret void
}

define void @store_i16(i32 %addr_arg) {
entry:
  %__1 = inttoptr i32 %addr_arg to i16*
  store i16 1, i16* %__1, align 1
  ret void

; CHECK:       Initial CFG
; CHECK:       %__1 = i32 %addr_arg
; CHECK-NEXT:  store i16 1, {{.*}}, align 1
; CHECK-NEXT:  ret void
}

define void @store_i8(i32 %addr_arg) {
entry:
  %__1 = inttoptr i32 %addr_arg to i8*
  store i8 1, i8* %__1, align 1
  ret void

; CHECK:       Initial CFG
; CHECK:       %__1 = i32 %addr_arg
; CHECK-NEXT:  store i8 1, {{.*}}, align 1
; CHECK-NEXT:  ret void
}

; ERRORS-NOT: ICE translation error
; DUMP-NOT: SZ
