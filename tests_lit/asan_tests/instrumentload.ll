; Test for a call to __asan_check() preceding loads

; REQUIRES: allow_dump

; RUN: %p2i -i %s --args -verbose=inst -threads=0 -fsanitize-address \
; RUN:     | FileCheck --check-prefix=DUMP %s

; Constants to load data from
@srcConst8 = internal constant [1 x i8] c"D"
@srcConst16 = internal constant [2 x i8] c"DA"
@srcConst32 = internal constant [4 x i8] c"DATA"
@srcConst64 = internal constant [8 x i8] c"DATADATA"
@srcConst128 = internal constant [16 x i8] c"DATADATADATADATA"

; A global to load data from
@srcGlobal8 = internal global [1 x i8] c"D"
@srcGlobal16 = internal global [2 x i8] c"DA"
@srcGlobal32 = internal global [4 x i8] c"DATA"
@srcGlobal64 = internal global [8 x i8] c"DATADATA"
@srcGlobal128 = internal global [16 x i8] c"DATADATADATADATA"

; A function with a local variable that does the loads
define internal void @doLoads() {
  %srcLocal8 = alloca i8, i32 1, align 4
  %srcLocal16 = alloca i8, i32 2, align 4
  %srcLocal32 = alloca i8, i32 4, align 4
  %srcLocal64 = alloca i8, i32 8, align 4
  %srcLocal128 = alloca i8, i32 16, align 4

  %ptrConst8 = bitcast [1 x i8]* @srcConst8 to i8*
  %ptrConst16 = bitcast [2 x i8]* @srcConst16 to i16*
  %ptrConst32 = bitcast [4 x i8]* @srcConst32 to i32*
  %ptrConst64 = bitcast [8 x i8]* @srcConst64 to i64*
  %ptrConst128 = bitcast [16 x i8]* @srcConst128 to <4 x i32>*

  %ptrGlobal8 = bitcast [1 x i8]* @srcGlobal8 to i8*
  %ptrGlobal16 = bitcast [2 x i8]* @srcGlobal16 to i16*
  %ptrGlobal32 = bitcast [4 x i8]* @srcGlobal32 to i32*
  %ptrGlobal64 = bitcast [8 x i8]* @srcGlobal64 to i64*
  %ptrGlobal128 = bitcast [16 x i8]* @srcGlobal128 to <4 x i32>*

  %ptrLocal8 = bitcast i8* %srcLocal8 to i8*
  %ptrLocal16 = bitcast i8* %srcLocal16 to i16*
  %ptrLocal32 = bitcast i8* %srcLocal32 to i32*
  %ptrLocal64 = bitcast i8* %srcLocal64 to i64*
  %ptrLocal128 = bitcast i8* %srcLocal128 to <4 x i32>*

  %dest1 = load i8, i8* %ptrConst8, align 1
  %dest2 = load i16, i16* %ptrConst16, align 1
  %dest3 = load i32, i32* %ptrConst32, align 1
  %dest4 = load i64, i64* %ptrConst64, align 1
  %dest5 = load <4 x i32>, <4 x i32>* %ptrConst128, align 4

  %dest6 = load i8, i8* %ptrGlobal8, align 1
  %dest7 = load i16, i16* %ptrGlobal16, align 1
  %dest8 = load i32, i32* %ptrGlobal32, align 1
  %dest9 = load i64, i64* %ptrGlobal64, align 1
  %dest10 = load <4 x i32>, <4 x i32>* %ptrGlobal128, align 4

  %dest11 = load i8, i8* %ptrLocal8, align 1
  %dest12 = load i16, i16* %ptrLocal16, align 1
  %dest13 = load i32, i32* %ptrLocal32, align 1
  %dest14 = load i64, i64* %ptrLocal64, align 1
  %dest15 = load <4 x i32>, <4 x i32>* %ptrLocal128, align 4

  ret void
}

; DUMP-LABEL: ================ Instrumented CFG ================
; DUMP-NEXT: define internal void @doLoads() {
; DUMP-NEXT: __0:
; DUMP: call void @__asan_check(i32 @srcConst8, i32 1)
; DUMP-NEXT: %dest1 = load i8, i8* @srcConst8, align 1
; DUMP-NEXT: call void @__asan_check(i32 @srcConst16, i32 2)
; DUMP-NEXT: %dest2 = load i16, i16* @srcConst16, align 1
; DUMP-NEXT: call void @__asan_check(i32 @srcConst32, i32 4)
; DUMP-NEXT: %dest3 = load i32, i32* @srcConst32, align 1
; DUMP-NEXT: call void @__asan_check(i32 @srcConst64, i32 8)
; DUMP-NEXT: %dest4 = load i64, i64* @srcConst64, align 1
; DUMP-NEXT: call void @__asan_check(i32 @srcConst128, i32 16)
; DUMP-NEXT: %dest5 = load <4 x i32>, <4 x i32>* @srcConst128, align 4
; DUMP-NEXT: call void @__asan_check(i32 @srcGlobal8, i32 1)
; DUMP-NEXT: %dest6 = load i8, i8* @srcGlobal8, align 1
; DUMP-NEXT: call void @__asan_check(i32 @srcGlobal16, i32 2)
; DUMP-NEXT: %dest7 = load i16, i16* @srcGlobal16, align 1
; DUMP-NEXT: call void @__asan_check(i32 @srcGlobal32, i32 4)
; DUMP-NEXT: %dest8 = load i32, i32* @srcGlobal32, align 1
; DUMP-NEXT: call void @__asan_check(i32 @srcGlobal64, i32 8)
; DUMP-NEXT: %dest9 = load i64, i64* @srcGlobal64, align 1
; DUMP-NEXT: call void @__asan_check(i32 @srcGlobal128, i32 16)
; DUMP-NEXT: %dest10 = load <4 x i32>, <4 x i32>* @srcGlobal128, align 4
; DUMP-NEXT: call void @__asan_check(i32 %srcLocal8, i32 1)
; DUMP-NEXT: %dest11 = load i8, i8* %srcLocal8, align 1
; DUMP-NEXT: call void @__asan_check(i32 %srcLocal16, i32 2)
; DUMP-NEXT: %dest12 = load i16, i16* %srcLocal16, align 1
; DUMP-NEXT: call void @__asan_check(i32 %srcLocal32, i32 4)
; DUMP-NEXT: %dest13 = load i32, i32* %srcLocal32, align 1
; DUMP-NEXT: call void @__asan_check(i32 %srcLocal64, i32 8)
; DUMP-NEXT: %dest14 = load i64, i64* %srcLocal64, align 1
; DUMP-NEXT: call void @__asan_check(i32 %srcLocal128, i32 16)
; DUMP-NEXT: %dest15 = load <4 x i32>, <4 x i32>* %srcLocal128, align 4
; DUMP-NEXT: ret void
; DUMP-NEXT }
