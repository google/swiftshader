; Test for a call to __asan_check() preceding stores

; REQUIRES: allow_dump

; RUN: %p2i -i %s --args -verbose=inst -threads=0 -fsanitize-address \
; RUN:     | FileCheck --check-prefix=DUMP %s

; A global to store data to
@destGlobal8 = internal global [1 x i8] zeroinitializer
@destGlobal16 = internal global [2 x i8] zeroinitializer
@destGlobal32 = internal global [4 x i8] zeroinitializer
@destGlobal64 = internal global [8 x i8] zeroinitializer
@destGlobal128 = internal global [16 x i8] zeroinitializer

; A function with a local variable that does the stores
define internal void @doStores(<4 x i32> %vecSrc) {
  %destLocal8 = alloca i8, i32 1, align 4
  %destLocal16 = alloca i8, i32 2, align 4
  %destLocal32 = alloca i8, i32 4, align 4
  %destLocal64 = alloca i8, i32 8, align 4
  %destLocal128 = alloca i8, i32 16, align 4

  %ptrGlobal8 = bitcast [1 x i8]* @destGlobal8 to i8*
  %ptrGlobal16 = bitcast [2 x i8]* @destGlobal16 to i16*
  %ptrGlobal32 = bitcast [4 x i8]* @destGlobal32 to i32*
  %ptrGlobal64 = bitcast [8 x i8]* @destGlobal64 to i64*
  %ptrGlobal128 = bitcast [16 x i8]* @destGlobal128 to <4 x i32>*

  %ptrLocal8 = bitcast i8* %destLocal8 to i8*
  %ptrLocal16 = bitcast i8* %destLocal16 to i16*
  %ptrLocal32 = bitcast i8* %destLocal32 to i32*
  %ptrLocal64 = bitcast i8* %destLocal64 to i64*
  %ptrLocal128 = bitcast i8* %destLocal128 to <4 x i32>*

  store i8 42, i8* %ptrGlobal8, align 1
  store i16 42, i16* %ptrGlobal16, align 1
  store i32 42, i32* %ptrGlobal32, align 1
  store i64 42, i64* %ptrGlobal64, align 1
  store <4 x i32> %vecSrc, <4 x i32>* %ptrGlobal128, align 4

  store i8 42, i8* %ptrLocal8, align 1
  store i16 42, i16* %ptrLocal16, align 1
  store i32 42, i32* %ptrLocal32, align 1
  store i64 42, i64* %ptrLocal64, align 1
  store <4 x i32> %vecSrc, <4 x i32>* %ptrLocal128, align 4

  ret void
}

; DUMP-LABEL: ================ Instrumented CFG ================
; DUMP-NEXT: define internal void @doStores(<4 x i32> %vecSrc) {
; DUMP-NEXT: __0:
; DUMP:      call void @__asan_check(i32 @destGlobal8, i32 1)
; DUMP-NEXT: store i8 42, i8* @destGlobal8, align 1
; DUMP-NEXT: call void @__asan_check(i32 @destGlobal16, i32 2)
; DUMP-NEXT: store i16 42, i16* @destGlobal16, align 1
; DUMP-NEXT: call void @__asan_check(i32 @destGlobal32, i32 4)
; DUMP-NEXT: store i32 42, i32* @destGlobal32, align 1
; DUMP-NEXT: call void @__asan_check(i32 @destGlobal64, i32 8)
; DUMP-NEXT: store i64 42, i64* @destGlobal64, align 1
; DUMP-NEXT: call void @__asan_check(i32 @destGlobal128, i32 16)
; DUMP-NEXT: store <4 x i32> %vecSrc, <4 x i32>* @destGlobal128, align 4
; DUMP-NEXT: call void @__asan_check(i32 %destLocal8, i32 1)
; DUMP-NEXT: store i8 42, i8* %destLocal8, align 1
; DUMP-NEXT: call void @__asan_check(i32 %destLocal16, i32 2)
; DUMP-NEXT: store i16 42, i16* %destLocal16, align 1
; DUMP-NEXT: call void @__asan_check(i32 %destLocal32, i32 4)
; DUMP-NEXT: store i32 42, i32* %destLocal32, align 1
; DUMP-NEXT: call void @__asan_check(i32 %destLocal64, i32 8)
; DUMP-NEXT: store i64 42, i64* %destLocal64, align 1
; DUMP-NEXT: call void @__asan_check(i32 %destLocal128, i32 16)
; DUMP-NEXT: store <4 x i32> %vecSrc, <4 x i32>* %destLocal128, align 4
; DUMP-NEXT: ret void
; DUMP-NEXT: }
