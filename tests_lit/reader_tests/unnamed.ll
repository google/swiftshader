; Tests that we name unnamed global addresses.

; RUN: llvm-as < %s | pnacl-freeze \
; RUN:              | %llvm2ice -notranslate -verbose=inst -build-on-read \
; RUN:                -allow-pnacl-reader-error-recovery \
; RUN:              | FileCheck %s

; RUN: llvm-as < %s | pnacl-freeze \
; RUN:              | %llvm2ice -notranslate -verbose=inst -build-on-read \
; RUN:                -default-function-prefix=h -default-global-prefix=g \
; RUN:                -allow-pnacl-reader-error-recovery \
; RUN:              | FileCheck --check-prefix=BAD %s

; TODO(kschimpf) Check global variable declarations, once generated.

@0 = internal global [4 x i8] zeroinitializer, align 4
@1 = internal constant [10 x i8] c"Some stuff", align 1
@g = internal global [4 x i8] zeroinitializer, align 4

; BAD: Warning: Default global prefix 'g' conflicts with name 'g'.

define i32 @2(i32 %v) {
  ret i32 %v
}

; CHECK:      define i32 @Function(i32 %__0) {
; CHECK-NEXT: __0:
; CHECK-NEXT:   ret i32 %__0
; CHECK-NEXT: }

define void @hg() {
  ret void
}

; BAD: Warning: Default function prefix 'h' conflicts with name 'hg'.

; CHECK-NEXT: define void @hg() {
; CHECK-NEXT: __0:
; CHECK-NEXT:   ret void
; CHECK-NEXT: }

define void @3() {
  ret void
}

; CHECK-NEXT: define void @Function1() {
; CHECK-NEXT: __0:
; CHECK-NEXT:   ret void
; CHECK-NEXT: }
