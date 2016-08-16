; Test that loads of local pointers to allocation functions are instrumented

; REQUIRES: allow_dump

; RUN: %p2i -i %s --args -verbose=inst -threads=0 -fsanitize-address \
; RUN:     -allow-externally-defined-symbols | FileCheck --check-prefix=DUMP %s

declare external i32 @malloc(i32)
declare external i32 @realloc(i32, i32)
declare external i32 @calloc(i32, i32)
declare external void @free(i32)

define internal void @func() {
  %malloc_addr = bitcast i32 (i32)* @malloc to i32*
  %realloc_addr = bitcast i32 (i32, i32)* @realloc to i32*
  %calloc_addr = bitcast i32 (i32, i32)* @calloc to i32*
  %free_addr = bitcast void (i32)* @free to i32*

  %local_malloc = load i32, i32* %malloc_addr, align 1
  %local_realloc = load i32, i32* %realloc_addr, align 1
  %local_calloc = load i32, i32* %calloc_addr, align 1
  %local_free = load i32, i32* %free_addr, align 1

  %local_mallocfunc = inttoptr i32 %local_malloc to i32 (i32)*
  %local_reallocfunc = inttoptr i32 %local_realloc to i32 (i32, i32)*
  %local_callocfunc = inttoptr i32 %local_calloc to i32 (i32, i32)*
  %local_freefunc = inttoptr i32 %local_free to void (i32)*

  %buf = call i32 %local_mallocfunc(i32 42)
  call void %local_freefunc(i32 %buf)
  ret void
}

; DUMP-LABEL: ================ Instrumented CFG ================
; DUMP-NEXT: @func() {
; DUMP-NEXT: __0:
; DUMP-NEXT:   call void @__asan_check_load(i32 @__asan_malloc, i32 4)
; DUMP-NEXT:   %local_malloc = load i32, i32* @__asan_malloc, align 1
; DUMP-NEXT:   call void @__asan_check_load(i32 @__asan_realloc, i32 4)
; DUMP-NEXT:   %local_realloc = load i32, i32* @__asan_realloc, align 1
; DUMP-NEXT:   call void @__asan_check_load(i32 @__asan_calloc, i32 4)
; DUMP-NEXT:   %local_calloc = load i32, i32* @__asan_calloc, align 1
; DUMP-NEXT:   call void @__asan_check_load(i32 @__asan_free, i32 4)
; DUMP-NEXT:   %local_free = load i32, i32* @__asan_free, align 1
; DUMP-NEXT:   %buf = call i32 %local_malloc(i32 42)
; DUMP-NEXT:   call void %local_free(i32 %buf)
; DUMP-NEXT:   ret void
; DUMP-NEXT: }
