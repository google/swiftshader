; Check that functions with multiple returns are correctly instrumented

; REQUIRES: allow_dump

; RUN: %p2i -i %s --args -verbose=inst -threads=0 -fsanitize-address \
; RUN:     | FileCheck --check-prefix=DUMP %s

define internal void @ret_twice(i32 %condarg) {
  %local1 = alloca i8, i32 4, align 4
  %local2 = alloca i8, i32 4, align 4
  %cond = icmp ne i32 %condarg, 0
  br i1 %cond, label %yes, label %no
yes:
  ret void
no:
  ret void
}

; DUMP-LABEL: ================ Instrumented CFG ================
; DUMP-NEXT: define internal void @ret_twice(i32 %condarg) {
; DUMP-NEXT: __0:
; DUMP-NEXT:   %local1 = alloca i8, i32 64, align 8
; DUMP-NEXT:   %local2 = alloca i8, i32 64, align 8
; DUMP-NEXT:   %__$rz2 = alloca i8, i32 32, align 8
; DUMP-NEXT:   call void @__asan_poison(i32 %__$rz2, i32 32)
; DUMP-NEXT:   %__$rz0 = add i32 %local1, 4
; DUMP-NEXT:   call void @__asan_poison(i32 %__$rz0, i32 60)
; DUMP-NEXT:   %__$rz1 = add i32 %local2, 4
; DUMP-NEXT:   call void @__asan_poison(i32 %__$rz1, i32 60)
; DUMP-NEXT:   %cond = icmp ne i32 %condarg, 0
; DUMP-NEXT:   br i1 %cond, label %yes, label %no
; DUMP-NEXT: yes:
; DUMP-NEXT:   call void @__asan_unpoison(i32 %__$rz0, i32 60)
; DUMP-NEXT:   call void @__asan_unpoison(i32 %__$rz1, i32 60)
; DUMP-NEXT:   call void @__asan_unpoison(i32 %__$rz2, i32 32)
; DUMP-NEXT:   ret void
; DUMP-NEXT: no:
; DUMP-NEXT:   call void @__asan_unpoison(i32 %__$rz0, i32 60)
; DUMP-NEXT:   call void @__asan_unpoison(i32 %__$rz1, i32 60)
; DUMP-NEXT:   call void @__asan_unpoison(i32 %__$rz2, i32 32)
; DUMP-NEXT:   ret void
; DUMP-NEXT: }