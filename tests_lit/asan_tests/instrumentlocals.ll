; Test for insertion of redzones around local variables

; REQUIRES: allow_dump

; RUN: %p2i -i %s --args -verbose=inst -threads=0 -fsanitize-address \
; RUN:     | FileCheck --check-prefix=DUMP %s

; Function with local variables to be instrumented
define internal void @func() {
  %local1 = alloca i8, i32 4, align 4
  %local2 = alloca i8, i32 32, align 1
  %local3 = alloca i8, i32 13, align 2
  %local4 = alloca i8, i32 75, align 4
  %local5 = alloca i8, i32 64, align 8
  ret void
}

; DUMP-LABEL: ================ Instrumented CFG ================
; DUMP-NEXT: define internal void @func() {
; DUMP-NEXT: __0:
; DUMP-NEXT: %local1 = alloca i8, i32 64, align 8
; DUMP-NEXT: %local2 = alloca i8, i32 64, align 8
; DUMP-NEXT: %local3 = alloca i8, i32 64, align 8
; DUMP-NEXT: %local4 = alloca i8, i32 128, align 8
; DUMP-NEXT: %local5 = alloca i8, i32 96, align 8
; DUMP-NEXT: %__$rz[[RZ0:[0-9]+]] = alloca i8, i32 32, align 8
; DUMP-NEXT: call void @__asan_poison(i32 %__$rz[[RZ0]], i32 32)
; DUMP-NEXT: %__$rz[[RZ1:[0-9]+]] = add i32 %local1, 4
; DUMP-NEXT: call void @__asan_poison(i32 %__$rz[[RZ1]], i32 60)
; DUMP-NEXT: %__$rz[[RZ2:[0-9]+]] = add i32 %local2, 32
; DUMP-NEXT: call void @__asan_poison(i32 %__$rz[[RZ2]], i32 32)
; DUMP-NEXT: %__$rz[[RZ3:[0-9]+]] = add i32 %local3, 13
; DUMP-NEXT: call void @__asan_poison(i32 %__$rz[[RZ3]], i32 51)
; DUMP-NEXT: %__$rz[[RZ4:[0-9]+]] = add i32 %local4, 75
; DUMP-NEXT: call void @__asan_poison(i32 %__$rz[[RZ4]], i32 53)
; DUMP-NEXT: %__$rz[[RZ5:[0-9]+]] = add i32 %local5, 64
; DUMP-NEXT: call void @__asan_poison(i32 %__$rz[[RZ5]], i32 32)
; DUMP-NEXT: call void @__asan_unpoison(i32 %__$rz[[RZ1]], i32 60)
; DUMP-NEXT: call void @__asan_unpoison(i32 %__$rz[[RZ2]], i32 32)
; DUMP-NEXT: call void @__asan_unpoison(i32 %__$rz[[RZ3]], i32 51)
; DUMP-NEXT: call void @__asan_unpoison(i32 %__$rz[[RZ4]], i32 53)
; DUMP-NEXT: call void @__asan_unpoison(i32 %__$rz[[RZ5]], i32 32)
; DUMP-NEXT: call void @__asan_unpoison(i32 %__$rz[[RZ0]], i32 32)
; DUMP-NEXT: ret void
; DUMP-NEXT: }
