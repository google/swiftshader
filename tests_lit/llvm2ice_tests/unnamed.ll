; Tests that we name unnamed global addresses.

; RUN: %p2i -i %s --insts | FileCheck %s

; RUN: %p2i -i %s --insts --args -default-function-prefix=h \
; RUN:     -default-global-prefix=g | FileCheck --check-prefix=BAD %s

@0 = internal global [4 x i8] zeroinitializer, align 4

; CHECK:      @Global = internal global [4 x i8] zeroinitializer, align 4

@1 = internal constant [10 x i8] c"Some stuff", align 1

; CHECK-NEXT: @Global1 = internal constant [10 x i8] c"Some stuff", align 1

@g = internal global [4 x i8] zeroinitializer, align 4

; BAD: Warning: Default global prefix 'g' conflicts with name 'g'.

; CHECK-NEXT: @g = internal global [4 x i8] zeroinitializer, align 4

define i32 @2(i32 %v) {
  ret i32 %v
}

; CHECK-NEXT: define i32 @Function(i32 %v) {
; CHECK-NEXT: __0:
; CHECK-NEXT:   ret i32 %v
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

