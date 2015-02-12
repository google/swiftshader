; Test of global initializers.

; REQUIRES: allow_dump

; Test -filetype=asm to test the lea "hack" until we are fully confident
; in -filetype=iasm .
; RUN: %p2i -i %s --args --verbose none -filetype=asm | FileCheck %s

; Test -filetype=iasm and try to cross reference instructions w/ the
; symbol table.
; RUN: %p2i -i %s --args --verbose none \
; RUN:   | llvm-mc -triple=i686-none-nacl -filetype=obj \
; RUN:   | llvm-objdump -d -r --symbolize -x86-asm-syntax=intel - \
; RUN:   | FileCheck --check-prefix=IAS %s
; RUN: %p2i -i %s --args --verbose none \
; RUN:   | llvm-mc -triple=i686-none-nacl -filetype=obj \
; RUN:   | llvm-objdump -d -t --symbolize -x86-asm-syntax=intel - \
; RUN:   | FileCheck --check-prefix=SYMTAB %s

@PrimitiveInit = internal global [4 x i8] c"\1B\00\00\00", align 4
; CHECK: .type PrimitiveInit,@object
; CHECK-NEXT: .section .data,"aw",@progbits
; CHECK-NEXT: .align 4
; CHECK-NEXT: PrimitiveInit:
; CHECK-NEXT: .byte
; CHECK: .size PrimitiveInit, 4

@PrimitiveInitConst = internal constant [4 x i8] c"\0D\00\00\00", align 4
; CHECK: .type PrimitiveInitConst,@object
; CHECK-NEXT: .section .rodata,"a",@progbits
; CHECK-NEXT: .align 4
; CHECK-NEXT: PrimitiveInitConst:
; CHECK-NEXT: .byte
; CHECK: .size PrimitiveInitConst, 4

@ArrayInit = internal global [20 x i8] c"\0A\00\00\00\14\00\00\00\1E\00\00\00(\00\00\002\00\00\00", align 4
; CHECK: .type ArrayInit,@object
; CHECK-NEXT: .section .data,"aw",@progbits
; CHECK-NEXT: .align 4
; CHECK-NEXT: ArrayInit:
; CHECK-NEXT: .byte
; CHECK: .size ArrayInit, 20

@ArrayInitPartial = internal global [40 x i8] c"<\00\00\00F\00\00\00P\00\00\00Z\00\00\00d\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00", align 4
; CHECK: .type ArrayInitPartial,@object
; CHECK-NEXT: .section .data,"aw",@progbits
; CHECK-NEXT: .align 4
; CHECK-NEXT: ArrayInitPartial:
; CHECK-NEXT: .byte
; CHECK: .size ArrayInitPartial, 40

@PrimitiveInitStatic = internal global [4 x i8] zeroinitializer, align 4
; CHECK: .type PrimitiveInitStatic,@object
; CHECK-NEXT: .section .bss,"aw",@nobits
; CHECK-NEXT: .align 4
; CHECK-NEXT: PrimitiveInitStatic:
; CHECK-NEXT: .zero 4
; CHECK-NEXT: .size PrimitiveInitStatic, 4

@PrimitiveUninit = internal global [4 x i8] zeroinitializer, align 4
; CHECK: .type PrimitiveUninit,@object
; CHECK-NEXT: .section .bss,"aw",@nobits
; CHECK-NEXT: .align 4
; CHECK-NEXT: PrimitiveUninit:
; CHECK-NEXT: .zero 4
; CHECK-NEXT: .size PrimitiveUninit, 4

@ArrayUninit = internal global [20 x i8] zeroinitializer, align 4
; CHECK: .type ArrayUninit,@object
; CHECK-NEXT: .section .bss,"aw",@nobits
; CHECK-NEXT: .align 4
; CHECK-NEXT: ArrayUninit:
; CHECK-NEXT: .zero 20
; CHECK-NEXT: .size ArrayUninit, 20

@ArrayUninitConstDouble = internal constant [200 x i8] zeroinitializer, align 8
; CHECK: .type ArrayUninitConstDouble,@object
; CHECK-NEXT: .section .rodata,"a",@progbits
; CHECK-NEXT: .align 8
; CHECK-NEXT: ArrayUninitConstDouble:
; CHECK-NEXT: .zero 200
; CHECK-NEXT: .size ArrayUninitConstDouble, 200

@ArrayUninitConstInt = internal constant [20 x i8] zeroinitializer, align 4
; CHECK: .type ArrayUninitConstInt,@object
; CHECK: .section .rodata,"a",@progbits
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
; CHECK: movl $PrimitiveInit,
; CHECK: movl $PrimitiveInitConst,
; CHECK: movl $PrimitiveInitStatic,
; CHECK: movl $PrimitiveUninit,
; CHECK: movl $ArrayInit,
; CHECK: movl $ArrayInitPartial,
; CHECK: movl $ArrayUninit,

; llvm-objdump does not indicate what symbol the mov/relocation applies to
; so we grep for "mov {{.*}}, OFFSET", along with "OFFSET {{.*}} symbol" in
; the symbol table as a sanity check. NOTE: The symbol table sorting has no
; relation to the code's references.
; IAS-LABEL: main
; SYMTAB-LABEL: SYMBOL TABLE

; SYMTAB-DAG: 00000000 {{.*}} .data {{.*}} PrimitiveInit
; IAS: mov {{.*}}, .data
; IAS-NEXT: R_386_32
; IAS: call

; SYMTAB-DAG: 00000000 {{.*}} .rodata {{.*}} PrimitiveInitConst
; IAS: mov {{.*}}, .rodata
; IAS-NEXT: R_386_32
; IAS: call

; SYMTAB-DAG: 00000000 {{.*}} .bss {{.*}} PrimitiveInitStatic
; IAS: mov {{.*}}, .bss
; IAS-NEXT: R_386_32
; IAS: call

; SYMTAB-DAG: 00000004 {{.*}} .bss {{.*}} PrimitiveUninit
; IAS: mov {{.*}}, .bss
; IAS-NEXT: R_386_32
; IAS: call

; SYMTAB-DAG: 00000004{{.*}}.data{{.*}}ArrayInit
; IAS: mov {{.*}}, .data
; IAS-NEXT: R_386_32
; IAS: call

; SYMTAB-DAG: 00000018 {{.*}} .data {{.*}} ArrayInitPartial
; IAS: mov {{.*}}, .data
; IAS-NEXT: R_386_32
; IAS: call

; SYMTAB-DAG: 00000008 {{.*}} .bss {{.*}} ArrayUninit
; IAS: mov {{.*}}, .bss
; IAS-NEXT: R_386_32
; IAS: call


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
