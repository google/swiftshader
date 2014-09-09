; Test if we can read alloca instructions.

; RUN: llvm-as < %s | pnacl-freeze \
; RUN:              | %llvm2ice -notranslate -verbose=inst -build-on-read \
; RUN:                -allow-pnacl-reader-error-recovery \
; RUN:              | FileCheck %s

; Show examples where size is defined by a constant.

define i32 @AllocaA0Size1() {
  %array = alloca i8, i32 1
  %addr = ptrtoint i8* %array to i32
  ret i32 %addr

; CHECK:      __0:
; CHECK-NEXT:   %__0 = alloca i8, i32 1
; CHECK-NEXT:   ret i32 %__0
}

define i32 @AllocaA0Size2() {
  %array = alloca i8, i32 2
  %addr = ptrtoint i8* %array to i32
  ret i32 %addr

; CHECK:      __0:
; CHECK-NEXT:   %__0 = alloca i8, i32 2
; CHECK-NEXT:   ret i32 %__0
}

define i32 @AllocaA0Size3() {
  %array = alloca i8, i32 3
  %addr = ptrtoint i8* %array to i32
  ret i32 %addr

; CHECK:      __0:
; CHECK-NEXT:   %__0 = alloca i8, i32 3
; CHECK-NEXT:   ret i32 %__0
}

define i32 @AllocaA0Size4() {
  %array = alloca i8, i32 4
  %addr = ptrtoint i8* %array to i32
  ret i32 %addr

; CHECK:      __0:
; CHECK-NEXT:   %__0 = alloca i8, i32 4
; CHECK-NEXT:   ret i32 %__0
}

define i32 @AllocaA1Size4(i32 %n) {
  %array = alloca i8, i32 4, align 1
  %addr = ptrtoint i8* %array to i32
  ret i32 %addr

; CHECK:      __0:
; CHECK-NEXT:   %__1 = alloca i8, i32 4, align 1
; CHECK-NEXT:   ret i32 %__1
}

define i32 @AllocaA2Size4(i32 %n) {
  %array = alloca i8, i32 4, align 2
  %addr = ptrtoint i8* %array to i32
  ret i32 %addr

; CHECK:      __0:
; CHECK-NEXT:   %__1 = alloca i8, i32 4, align 2
; CHECK-NEXT:   ret i32 %__1
}

define i32 @AllocaA8Size4(i32 %n) {
  %array = alloca i8, i32 4, align 8
  %addr = ptrtoint i8* %array to i32
  ret i32 %addr

; CHECK:      __0:
; CHECK-NEXT:   %__1 = alloca i8, i32 4, align 8
; CHECK-NEXT:   ret i32 %__1
}

define i32 @Alloca16Size4(i32 %n) {
  %array = alloca i8, i32 4, align 16
  %addr = ptrtoint i8* %array to i32
  ret i32 %addr

; CHECK: __0:
; CHECK-NEXT:   %__1 = alloca i8, i32 4, align 16
; CHECK-NEXT:   ret i32 %__1
}

; Show examples where size is not known at compile time.

define i32 @AllocaVarsizeA0(i32 %n) {
  %array = alloca i8, i32 %n
  %addr = ptrtoint i8* %array to i32
  ret i32 %addr

; CHECK: __0:
; CHECK-NEXT:   %__1 = alloca i8, i32 %__0
; CHECK-NEXT:   ret i32 %__1
}

define i32 @AllocaVarsizeA1(i32 %n) {
  %array = alloca i8, i32 %n, align 1
  %addr = ptrtoint i8* %array to i32
  ret i32 %addr

; CHECK:      __0:
; CHECK-NEXT:   %__1 = alloca i8, i32 %__0, align 1
; CHECK-NEXT:   ret i32 %__1
}

define i32 @AllocaVarsizeA2(i32 %n) {
  %array = alloca i8, i32 %n, align 2
  %addr = ptrtoint i8* %array to i32
  ret i32 %addr

; CHECK:      __0:
; CHECK-NEXT:   %__1 = alloca i8, i32 %__0, align 2
; CHECK-NEXT:   ret i32 %__1
}

define i32 @AllocaVarsizeA4(i32 %n) {
  %array = alloca i8, i32 %n, align 4
  %addr = ptrtoint i8* %array to i32
  ret i32 %addr

; CHECK:      __0:
; CHECK-NEXT:   %__1 = alloca i8, i32 %__0, align 4
; CHECK-NEXT:   ret i32 %__1
}

define i32 @AllocaVarsizeA8(i32 %n) {
  %array = alloca i8, i32 %n, align 8
  %addr = ptrtoint i8* %array to i32
  ret i32 %addr

; CHECK:      __0:
; CHECK-NEXT:   %__1 = alloca i8, i32 %__0, align 8
; CHECK-NEXT:   ret i32 %__1
}

define i32 @AllocaVarsizeA16(i32 %n) {
  %array = alloca i8, i32 %n, align 16
  %addr = ptrtoint i8* %array to i32
  ret i32 %addr

; CHECK:      __0:
; CHECK-NEXT:   %__1 = alloca i8, i32 %__0, align 16
; CHECK-NEXT:   ret i32 %__1
}
