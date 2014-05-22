; Simple test of the load instruction.

; RUN: %llvm2ice --verbose inst %s | FileCheck %s
; RUN: %llvm2ice --verbose none %s | FileCheck --check-prefix=ERRORS %s
; RUN: %llvm2iceinsts %s | %szdiff %s | FileCheck --check-prefix=DUMP %s
; RUN: %llvm2iceinsts --pnacl %s | %szdiff %s \
; RUN:                           | FileCheck --check-prefix=DUMP %s

define void @load_i64(i32 %addr_arg) {
entry:
  %__1 = inttoptr i32 %addr_arg to i64*
  %iv = load i64* %__1, align 1
  ret void

; CHECK:       Initial CFG
; CHECK:       %__1 = i32 %addr_arg
; CHECK-NEXT:  %iv = load i64* {{.*}}, align 1
; CHECK-NEXT:  ret void
}

define void @load_i32(i32 %addr_arg) {
entry:
  %__1 = inttoptr i32 %addr_arg to i32*
  %iv = load i32* %__1, align 1
  ret void

; CHECK:       Initial CFG
; CHECK:       %__1 = i32 %addr_arg
; CHECK-NEXT:  %iv = load i32* {{.*}}, align 1
; CHECK-NEXT:  ret void
}

define void @load_i16(i32 %addr_arg) {
entry:
  %__1 = inttoptr i32 %addr_arg to i16*
  %iv = load i16* %__1, align 1
  ret void

; CHECK:       Initial CFG
; CHECK:       %__1 = i32 %addr_arg
; CHECK-NEXT:  %iv = load i16* {{.*}}, align 1
; CHECK-NEXT:  ret void
}

define void @load_i8(i32 %addr_arg) {
entry:
  %__1 = inttoptr i32 %addr_arg to i8*
  %iv = load i8* %__1, align 1
  ret void

; CHECK:       Initial CFG
; CHECK:       %__1 = i32 %addr_arg
; CHECK-NEXT:  %iv = load i8* {{.*}}, align 1
; CHECK-NEXT:  ret void
}

; ERRORS-NOT: ICE translation error
; DUMP-NOT: SZ
