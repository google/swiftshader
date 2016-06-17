; Test that calls to malloc() and free() are replaced

; REQUIRES: allow_dump

; RUN: %p2i -i %s --args -verbose=inst -threads=0 -fsanitize-address \
; RUN:     --allow-externally-defined-symbols | FileCheck --check-prefix=DUMP %s

declare external i32 @malloc(i32)
declare external void @free(i32)

define internal void @func() {
  %ptr = call i32 @malloc(i32 42)
  call void @free(i32 %ptr)
  ret void
}

; DUMP-LABEL: ================ Instrumented CFG ================
; DUMP-NEXT: define internal void @func() {
; DUMP-NEXT: __0:
; DUMP-NEXT: %ptr = call i32 @__asan_malloc(i32 42)
; DUMP-NEXT: call void @__asan_free(i32 %ptr)
; DUMP-NEXT: ret void
; DUMP-NEXT: }