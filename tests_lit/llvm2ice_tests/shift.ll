; RUIN: %llvm2ice -verbose inst %s | FileCheck %s
; RUIN: %llvm2ice --verbose none %s | FileCheck --check-prefix=ERRORS %s
; RUN: %szdiff --llvm2ice=%llvm2ice %s | FileCheck --check-prefix=DUMP %s

@i1 = common global i32 0, align 4
@i2 = common global i32 0, align 4
@u1 = common global i32 0, align 4
@u2 = common global i32 0, align 4

define void @conv1() {
entry:
  %v0 = load i32* @u1, align 1
  %sext = shl i32 %v0, 24
  %v1 = ashr i32 %sext, 24
  store i32 %v1, i32* @i1, align 1
  ret void
  ; CHECK: shl eax, 24
  ; CHECK-NEXT: sar eax, 24
}

define void @conv2() {
entry:
  %v0 = load i32* @u1, align 1
  %sext1 = shl i32 %v0, 16
  %v1 = ashr i32 %sext1, 16
  store i32 %v1, i32* @i2, align 1
  ret void
  ; CHECK: shl eax, 16
  ; CHECK-NEXT: sar eax, 16
}

; ERRORS-NOT: ICE translation error
; DUMP-NOT: SZ
