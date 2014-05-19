; RUIN: %llvm2ice -verbose inst %s | FileCheck %s
; RUIN: %llvm2ice --verbose none %s | FileCheck --check-prefix=ERRORS %s
; RUN: %llvm2iceinsts %s | %szdiff %s | FileCheck --check-prefix=DUMP %s
; RUN: %llvm2iceinsts --pnacl %s | %szdiff %s \
; RUN:                           | FileCheck --check-prefix=DUMP %s

@i1 = global [4 x i8] zeroinitializer, align 4
@i2 = global [4 x i8] zeroinitializer, align 4
@u1 = global [4 x i8] zeroinitializer, align 4

define void @conv1() {
entry:
  %__0 = bitcast [4 x i8]* @u1 to i32*
  %v0 = load i32* %__0, align 1
  %sext = shl i32 %v0, 24
  %v1 = ashr i32 %sext, 24
  %__4 = bitcast [4 x i8]* @i1 to i32*
  store i32 %v1, i32* %__4, align 1
  ret void
  ; CHECK: shl eax, 24
  ; CHECK-NEXT: sar eax, 24
}

define void @conv2() {
entry:
  %__0 = bitcast [4 x i8]* @u1 to i32*
  %v0 = load i32* %__0, align 1
  %sext1 = shl i32 %v0, 16
  %v1 = ashr i32 %sext1, 16
  %__4 = bitcast [4 x i8]* @i2 to i32*
  store i32 %v1, i32* %__4, align 1
  ret void
  ; CHECK: shl eax, 16
  ; CHECK-NEXT: sar eax, 16
}

; ERRORS-NOT: ICE translation error
; DUMP-NOT: SZ
