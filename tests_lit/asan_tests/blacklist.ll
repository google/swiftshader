; Test to ensure that blacklisted functions are not instrumented and others are.

; REQUIRES: allow_dump

; RUN: %p2i -i %s --args -verbose=inst -threads=0 -fsanitize-address \
; RUN:     -allow-externally-defined-symbols | FileCheck --check-prefix=DUMP %s

declare external i32 @malloc(i32)
declare external void @free(i32)

; A black listed function
define internal void @_Balloc() {
  %local = alloca i8, i32 4, align 4
  %heapvar = call i32 @malloc(i32 42)
  call void @free(i32 %heapvar)
  ret void
}

; DUMP-LABEL: ================ Instrumented CFG ================
; DUMP-NEXT: define internal void @_Balloc() {
; DUMP-NEXT: __0:
; DUMP-NEXT:   %local = alloca i8, i32 4, align 4
; DUMP-NEXT:   %heapvar = call i32 @malloc(i32 42)
; DUMP-NEXT:   call void @free(i32 %heapvar)
; DUMP-NEXT:   ret void
; DUMP-NEXT: }

; A non black listed function
define internal void @func() {
  %local = alloca i8, i32 4, align 4
  %heapvar = call i32 @malloc(i32 42)
  call void @free(i32 %heapvar)
  ret void
}

; DUMP-LABEL: ================ Instrumented CFG ================
; DUMP-NEXT: define internal void @func() {
; DUMP-NEXT: __0:
; DUMP-NEXT:   %local = alloca i8, i32 64, align 8
; DUMP-NEXT:   %__$rz1 = alloca i8, i32 32, align 8
; DUMP-NEXT:   call void @__asan_poison(i32 %__$rz1, i32 32, i32 -1)
; DUMP-NEXT:   %__$rz0 = add i32 %local, 4
; DUMP-NEXT:   call void @__asan_poison(i32 %__$rz0, i32 60, i32 -1)
; DUMP-NEXT:   %heapvar = call i32 @__asan_malloc(i32 42)
; DUMP-NEXT:   call void @__asan_free(i32 %heapvar)
; DUMP-NEXT:   call void @__asan_unpoison(i32 %__$rz0, i32 60)
; DUMP-NEXT:   call void @__asan_unpoison(i32 %__$rz1, i32 32)
; DUMP-NEXT:   ret void
; DUMP-NEXT: }
