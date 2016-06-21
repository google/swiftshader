; Test for insertion of redzones around global variables

; REQUIRES: allow_dump

; RUN: %p2i -i %s --args -verbose=inst -threads=0 -fsanitize-address \
; RUN:     | FileCheck --check-prefix=DUMP %s

; Function with local variables to be instrumented
define internal void @func() {
  %local0 = alloca i8, i32 4, align 4
  %local1 = alloca i8, i32 32, align 4
  %local2 = alloca i8, i32 13, align 4
  %local3 = alloca i8, i32 75, align 4
  %local4 = alloca i8, i32 64, align 4
  %local5 = alloca i8, i32 4, align 1
  %local6 = alloca i8, i32 32, align 1
  %local7 = alloca i8, i32 13, align 1
  %local8 = alloca i8, i32 75, align 1
  %local9 = alloca i8, i32 64, align 1
  ret void
}

; DUMP-LABEL: ================ Instrumented CFG ================
; DUMP-NEXT: define internal void @func() {
; DUMP-NEXT: __0:
; DUMP-NEXT: %__$rz{{[0-9]+}} = alloca i8, i32 32, align 4
; DUMP-NEXT: %local0 = alloca i8, i32 4, align 4
; DUMP-NEXT: %__$rz{{[0-9]+}} = alloca i8, i32 60, align 1
; DUMP-NEXT: %local1 = alloca i8, i32 32, align 4
; DUMP-NEXT: %__$rz{{[0-9]+}} = alloca i8, i32 32, align 1
; DUMP-NEXT: %local2 = alloca i8, i32 13, align 4
; DUMP-NEXT: %__$rz{{[0-9]+}} = alloca i8, i32 51, align 1
; DUMP-NEXT: %local3 = alloca i8, i32 75, align 4
; DUMP-NEXT: %__$rz{{[0-9]+}} = alloca i8, i32 53, align 1
; DUMP-NEXT: %local4 = alloca i8, i32 64, align 4
; DUMP-NEXT: %__$rz{{[0-9]+}} = alloca i8, i32 32, align 1
; DUMP-NEXT: %local5 = alloca i8, i32 4, align 1
; DUMP-NEXT: %__$rz{{[0-9]+}} = alloca i8, i32 60, align 1
; DUMP-NEXT: %local6 = alloca i8, i32 32, align 1
; DUMP-NEXT: %__$rz{{[0-9]+}} = alloca i8, i32 32, align 1
; DUMP-NEXT: %local7 = alloca i8, i32 13, align 1
; DUMP-NEXT: %__$rz{{[0-9]+}} = alloca i8, i32 51, align 1
; DUMP-NEXT: %local8 = alloca i8, i32 75, align 1
; DUMP-NEXT: %__$rz{{[0-9]+}} = alloca i8, i32 53, align 1
; DUMP-NEXT: %local9 = alloca i8, i32 64, align 
; DUMP-NEXT: %__$rz{{[0-9]+}} = alloca i8, i32 32, align 1
; DUMP-NEXT: ret void
; DUMP-NEXT: }
