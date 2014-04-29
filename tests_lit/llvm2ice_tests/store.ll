; RUIN: %llvm2ice %s -verbose inst | FileCheck %s
; RUIN: %llvm2ice --verbose none %s | FileCheck --check-prefix=ERRORS %s
; RUN: %szdiff --llvm2ice=%llvm2ice %s | FileCheck --check-prefix=DUMP %s

define void @store_i64(i32 %addr_arg) {
entry:
  %ptr64 = inttoptr i32 %addr_arg to i64*
  store i64 1, i64* %ptr64, align 1
  ret void

; CHECK:       %ptr64 = i32 %addr_arg
; CHECK-NEXT:  store i64 1, {{.*}}, align 1
; CHECK-NEXT:  ret void
}

define void @store_i32(i32 %addr_arg) {
entry:
  %ptr32 = inttoptr i32 %addr_arg to i32*
  store i32 1, i32* %ptr32, align 1
  ret void

; CHECK:       %ptr32 = i32 %addr_arg
; CHECK-NEXT:  store i32 1, {{.*}}, align 1
; CHECK-NEXT:  ret void
}

define void @store_i16(i32 %addr_arg) {
entry:
  %ptr16 = inttoptr i32 %addr_arg to i16*
  store i16 1, i16* %ptr16, align 1
  ret void

; CHECK:       %ptr16 = i32 %addr_arg
; CHECK-NEXT:  store i16 1, {{.*}}, align 1
; CHECK-NEXT:  ret void
}

define void @store_i8(i32 %addr_arg) {
entry:
  %ptr8 = inttoptr i32 %addr_arg to i8*
  store i8 1, i8* %ptr8, align 1
  ret void

; CHECK:       %ptr8 = i32 %addr_arg
; CHECK-NEXT:  store i8 1, {{.*}}, align 1
; CHECK-NEXT:  ret void
}

; ERRORS-NOT: ICE translation error
; DUMP-NOT: SZ
