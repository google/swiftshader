; Test of global initializers.

; RUN: %llvm2ice --verbose inst %s | FileCheck %s
; RUN: %llvm2ice --verbose none %s \
; RUN:     | llvm-mc -triple=i686-none-nacl -x86-asm-syntax=intel -filetype=obj
; RUN: %llvm2ice --verbose none %s | FileCheck --check-prefix=ERRORS %s
; RUN: %llvm2iceinsts %s | %szdiff %s | FileCheck --check-prefix=DUMP %s

@PrimitiveInit = internal global [4 x i8] c"\1B\00\00\00", align 4
; CHECK: .data
; CHECK-NEXT: .local
; CHECK-NEXT: .align 4
; CHECK-NEXT: PrimitiveInit:
; CHECK-NEXT: .byte
; CHECK: .size PrimitiveInit, 4

@PrimitiveInitConst = internal constant [4 x i8] c"\0D\00\00\00", align 4
; CHECK: .section .rodata,"a",@progbits
; CHECK-NEXT: .local
; CHECK-NEXT: .align 4
; CHECK-NEXT: PrimitiveInitConst:
; CHECK-NEXT: .byte
; CHECK: .size PrimitiveInitConst, 4

@ArrayInit = internal global [20 x i8] c"\0A\00\00\00\14\00\00\00\1E\00\00\00(\00\00\002\00\00\00", align 4
; CHECK: .data
; CHECK-NEXT: .local
; CHECK-NEXT: .align 4
; CHECK-NEXT: ArrayInit:
; CHECK-NEXT: .byte
; CHECK: .size ArrayInit, 20

@ArrayInitPartial = internal global [40 x i8] c"<\00\00\00F\00\00\00P\00\00\00Z\00\00\00d\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00", align 4
; CHECK: .data
; CHECK-NEXT: .local
; CHECK-NEXT: .align 4
; CHECK-NEXT: ArrayInitPartial:
; CHECK-NEXT: .byte
; CHECK: .size ArrayInitPartial, 40

@PrimitiveInitStatic = internal global [4 x i8] zeroinitializer, align 4
; CHECK: .data
; CHECK-NEXT: .local PrimitiveInitStatic
; CHECK-NEXT: .comm PrimitiveInitStatic, 4, 4

@PrimitiveUninit = internal global [4 x i8] zeroinitializer, align 4
; CHECK: .data
; CHECK-NEXT: .local PrimitiveUninit
; CHECK-NEXT: .comm PrimitiveUninit, 4, 4

@ArrayUninit = internal global [20 x i8] zeroinitializer, align 4
; CHECK: .data
; CHECK-NEXT: .local ArrayUninit
; CHECK-NEXT: .comm ArrayUninit, 20, 4

@ArrayUninitConstDouble = internal constant [200 x i8] zeroinitializer, align 8
; CHECK: .section .rodata,"a",@progbits
; CHECK-NEXT: .local
; CHECK-NEXT: .align 8
; CHECK-NEXT: ArrayUninitConstDouble:
; CHECK-NEXT: .zero 200
; CHECK-NEXT: .size ArrayUninitConstDouble, 200

@ArrayUninitConstInt = internal constant [20 x i8] zeroinitializer, align 4
; CHECK: .section .rodata,"a",@progbits
; CHECK-NEXT: .local
; CHECK-NEXT: .align 4
; CHECK-NEXT: ArrayUninitConstInt:
; CHECK-NEXT: .zero 20
; CHECK-NEXT: .size ArrayUninitConstInt, 20

@__init_array_start = internal constant [0 x i8] zeroinitializer, align 4
@__fini_array_start = internal constant [0 x i8] zeroinitializer, align 4
@__tls_template_start = internal constant [0 x i8] zeroinitializer, align 8
@__tls_template_alignment = internal constant [4 x i8] c"\01\00\00\00", align 4

define internal i32 @main(i32 %argc, i32 %argv) {
entry:
  %expanded1 = ptrtoint [4 x i8]* @PrimitiveInit to i32
  call void @use(i32 %expanded1)
  %expanded3 = ptrtoint [4 x i8]* @PrimitiveInitConst to i32
  call void @use(i32 %expanded3)
  %expanded5 = ptrtoint [4 x i8]* @PrimitiveInitStatic to i32
  call void @use(i32 %expanded5)
  %expanded7 = ptrtoint [4 x i8]* @PrimitiveUninit to i32
  call void @use(i32 %expanded7)
  %expanded9 = ptrtoint [20 x i8]* @ArrayInit to i32
  call void @use(i32 %expanded9)
  %expanded11 = ptrtoint [40 x i8]* @ArrayInitPartial to i32
  call void @use(i32 %expanded11)
  %expanded13 = ptrtoint [20 x i8]* @ArrayUninit to i32
  call void @use(i32 %expanded13)
  ret i32 0
}
; CHECK-LABEL: main
; CHECK: .att_syntax
; CHECK: leal PrimitiveInit,
; CHECK: .intel_syntax
; CHECK: .att_syntax
; CHECK: leal PrimitiveInitConst,
; CHECK: .intel_syntax
; CHECK: .att_syntax
; CHECK: leal PrimitiveInitStatic,
; CHECK: .intel_syntax
; CHECK: .att_syntax
; CHECK: leal PrimitiveUninit,
; CHECK: .intel_syntax
; CHECK: .att_syntax
; CHECK: leal ArrayInit,
; CHECK: .intel_syntax
; CHECK: .att_syntax
; CHECK: leal ArrayInitPartial,
; CHECK: .intel_syntax
; CHECK: .att_syntax
; CHECK: leal ArrayUninit,
; CHECK: .intel_syntax

declare void @use(i32)

define internal i32 @nacl_tp_tdb_offset(i32 %__0) {
entry:
  ret i32 0
}

define internal i32 @nacl_tp_tls_offset(i32 %size) {
entry:
  %result = sub i32 0, %size
  ret i32 %result
}

; ERRORS-NOT: ICE translation error
; DUMP-NOT: SZ
