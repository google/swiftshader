; Tests if we handle global variables with relocation initializers.

; Test that we handle it in the ICE converter.
; RUN: %lc2i -i %s --args -verbose inst | FileCheck %s

; Test that we handle it using Subzero's bitcode reader.
; RUN: %p2i -i %s --args -verbose inst | FileCheck %s

@bytes = internal global [7 x i8] c"abcdefg"
; CHECK: @bytes = internal global [7 x i8] c"abcdefg"
; CHECK:	.type	bytes,@object
; CHECK:	.section	.data,"aw",@progbits
; CHECK:bytes:
; CHECK:	.byte	97
; CHECK:	.byte	98
; CHECK:	.byte	99
; CHECK:	.byte	100
; CHECK:	.byte	101
; CHECK:	.byte	102
; CHECK:	.byte	103
; CHECK:	.size	bytes, 7

@const_bytes = internal constant [7 x i8] c"abcdefg"
; CHECK: @const_bytes = internal constant [7 x i8] c"abcdefg"
; CHECK:	.type	const_bytes,@object
; CHECK:	.section	.rodata,"a",@progbits
; CHECK:const_bytes:
; CHECK:	.byte	97
; CHECK:	.byte	98
; CHECK:	.byte	99
; CHECK:	.byte	100
; CHECK:	.byte	101
; CHECK:	.byte	102
; CHECK:	.byte	103
; CHECK:	.size	const_bytes, 7

@ptr_to_ptr = internal global i32 ptrtoint (i32* @ptr to i32)
; CHECK: @ptr_to_ptr = internal global i32 ptrtoint (i32* @ptr to i32)
; CHECK:	.type	ptr_to_ptr,@object
; CHECK:	.section	.data,"aw",@progbits
; CHECK:ptr_to_ptr:
; CHECK:	.long	ptr
; CHECK:	.size	ptr_to_ptr, 4

@const_ptr_to_ptr = internal constant i32 ptrtoint (i32* @ptr to i32)
; CHECK: @const_ptr_to_ptr = internal constant i32 ptrtoint (i32* @ptr to i32)
; CHECK:	.type	const_ptr_to_ptr,@object
; CHECK:	.section	.rodata,"a",@progbits
; CHECK:const_ptr_to_ptr:
; CHECK:	.long	ptr
; CHECK:	.size	const_ptr_to_ptr, 4

@ptr_to_func = internal global i32 ptrtoint (void ()* @func to i32)
; CHECK: @ptr_to_func = internal global i32 ptrtoint (void ()* @func to i32)
; CHECK:	.type	ptr_to_func,@object
; CHECK:	.section	.data,"aw",@progbits
; CHECK:ptr_to_func:
; CHECK:	.long	func
; CHECK:	.size	ptr_to_func, 4

@const_ptr_to_func = internal constant i32 ptrtoint (void ()* @func to i32)
; CHECK: @const_ptr_to_func = internal constant i32 ptrtoint (void ()* @func to i32)
; CHECK:	.type	const_ptr_to_func,@object
; CHECK:	.section	.rodata,"a",@progbits
; CHECK:const_ptr_to_func:
; CHECK:	.long	func
; CHECK:	.size	const_ptr_to_func, 4

@compound = internal global <{ [3 x i8], i32 }> <{ [3 x i8] c"foo", i32 ptrtoint (void ()* @func to i32) }>
; CHECK: @compound = internal global <{ [3 x i8], i32 }> <{ [3 x i8] c"foo", i32 ptrtoint (void ()* @func to i32) }>
; CHECK:	.type	compound,@object
; CHECK:	.section	.data,"aw",@progbits
; CHECK:compound:
; CHECK:	.byte	102
; CHECK:	.byte	111
; CHECK:	.byte	111
; CHECK:	.long	func
; CHECK:	.size	compound, 7

@const_compound = internal constant <{ [3 x i8], i32 }> <{ [3 x i8] c"foo", i32 ptrtoint (void ()* @func to i32) }>
; CHECK: @const_compound = internal constant <{ [3 x i8], i32 }> <{ [3 x i8] c"foo", i32 ptrtoint (void ()* @func to i32) }>
; CHECK:	.type	const_compound,@object
; CHECK:	.section	.rodata,"a",@progbits
; CHECK:const_compound:
; CHECK:	.byte	102
; CHECK:	.byte	111
; CHECK:	.byte	111
; CHECK:	.long	func
; CHECK:	.size	const_compound, 7

@ptr = internal global i32 ptrtoint ([7 x i8]* @bytes to i32)
; CHECK: @ptr = internal global i32 ptrtoint ([7 x i8]* @bytes to i32)
; CHECK:	.type	ptr,@object
; CHECK:	.section	.data,"aw",@progbits
; CHECK:ptr:
; CHECK:	.long	bytes
; CHECK:	.size	ptr, 4

@const_ptr = internal constant i32 ptrtoint ([7 x i8]* @bytes to i32)
; CHECK: @const_ptr = internal constant i32 ptrtoint ([7 x i8]* @bytes to i32)
; CHECK:	.type	const_ptr,@object
; CHECK:	.section	.rodata,"a",@progbits
; CHECK:const_ptr:
; CHECK:	.long	bytes
; CHECK:	.size	const_ptr, 4

@addend_ptr = internal global i32 add (i32 ptrtoint (i32* @ptr to i32), i32 1)
; CHECK: @addend_ptr = internal global i32 add (i32 ptrtoint (i32* @ptr to i32), i32 1)
; CHECK:	.type	addend_ptr,@object
; CHECK:	.section	.data,"aw",@progbits
; CHECK:addend_ptr:
; CHECK:	.long	ptr + 1
; CHECK:	.size	addend_ptr, 4

@const_addend_ptr = internal constant i32 add (i32 ptrtoint (i32* @ptr to i32), i32 1)
; CHECK: @const_addend_ptr = internal constant i32 add (i32 ptrtoint (i32* @ptr to i32), i32 1)
; CHECK:	.type	const_addend_ptr,@object
; CHECK:	.section	.rodata,"a",@progbits
; CHECK:const_addend_ptr:
; CHECK:	.long	ptr + 1
; CHECK:	.size	const_addend_ptr, 4

@addend_negative = internal global i32 add (i32 ptrtoint (i32* @ptr to i32), i32 -1)
; CHECK: @addend_negative = internal global i32 add (i32 ptrtoint (i32* @ptr to i32), i32 -1)
; CHECK:	.type	addend_negative,@object
; CHECK:	.section	.data,"aw",@progbits
; CHECK:addend_negative:
; CHECK:	.long	ptr - 1
; CHECK:	.size	addend_negative, 4

@const_addend_negative = internal constant i32 add (i32 ptrtoint (i32* @ptr to i32), i32 -1)
; CHECK: @const_addend_negative = internal constant i32 add (i32 ptrtoint (i32* @ptr to i32), i32 -1)
; CHECK:	.type	const_addend_negative,@object
; CHECK:	.section	.rodata,"a",@progbits
; CHECK:const_addend_negative:
; CHECK:	.long	ptr - 1
; CHECK:	.size	const_addend_negative, 4

@addend_array1 = internal global i32 add (i32 ptrtoint ([7 x i8]* @bytes to i32), i32 1)
; CHECK: @addend_array1 = internal global i32 add (i32 ptrtoint ([7 x i8]* @bytes to i32), i32 1)
; CHECK:	.type	addend_array1,@object
; CHECK:	.section	.data,"aw",@progbits
; CHECK:addend_array1:
; CHECK:	.long	bytes + 1
; CHECK:	.size	addend_array1, 4

@const_addend_array1 = internal constant i32 add (i32 ptrtoint ([7 x i8]* @bytes to i32), i32 1)
; CHECK: @const_addend_array1 = internal constant i32 add (i32 ptrtoint ([7 x i8]* @bytes to i32), i32 1)
; CHECK:	.type	const_addend_array1,@object
; CHECK:	.section	.rodata,"a",@progbits
; CHECK:const_addend_array1:
; CHECK:	.long	bytes + 1
; CHECK:	.size	const_addend_array1, 4

@addend_array2 = internal global i32 add (i32 ptrtoint ([7 x i8]* @bytes to i32), i32 7)
; CHECK: @addend_array2 = internal global i32 add (i32 ptrtoint ([7 x i8]* @bytes to i32), i32 7)
; CHECK:	.type	addend_array2,@object
; CHECK:	.section	.data,"aw",@progbits
; CHECK:addend_array2:
; CHECK:	.long	bytes + 7
; CHECK:	.size	addend_array2, 4

@const_addend_array2 = internal constant i32 add (i32 ptrtoint ([7 x i8]* @bytes to i32), i32 7)
; CHECK: @const_addend_array2 = internal constant i32 add (i32 ptrtoint ([7 x i8]* @bytes to i32), i32 7)
; CHECK:	.type	const_addend_array2,@object
; CHECK:	.section	.rodata,"a",@progbits
; CHECK:const_addend_array2:
; CHECK:	.long	bytes + 7
; CHECK:	.size	const_addend_array2, 4

@addend_array3 = internal global i32 add (i32 ptrtoint ([7 x i8]* @bytes to i32), i32 9)
; CHECK: @addend_array3 = internal global i32 add (i32 ptrtoint ([7 x i8]* @bytes to i32), i32 9)
; CHECK:	.type	addend_array3,@object
; CHECK:	.section	.data,"aw",@progbits
; CHECK:addend_array3:
; CHECK:	.long	bytes + 9
; CHECK:	.size	addend_array3, 4

@const_addend_array3 = internal constant i32 add (i32 ptrtoint ([7 x i8]* @bytes to i32), i32 9)
; CHECK: @const_addend_array3 = internal constant i32 add (i32 ptrtoint ([7 x i8]* @bytes to i32), i32 9)
; CHECK:	.type	const_addend_array3,@object
; CHECK:	.section	.rodata,"a",@progbits
; CHECK:const_addend_array3:
; CHECK:	.long	bytes + 9
; CHECK:	.size	const_addend_array3, 4

@addend_struct1 = internal global i32 add (i32 ptrtoint (<{ [3 x i8], i32 }>* @compound to i32), i32 1)
; CHECK: @addend_struct1 = internal global i32 add (i32 ptrtoint (<{ [3 x i8], i32 }>* @compound to i32), i32 1)
; CHECK:	.type	addend_struct1,@object
; CHECK:	.section	.data,"aw",@progbits
; CHECK:addend_struct1:
; CHECK:	.long	compound + 1
; CHECK:	.size	addend_struct1, 4

@const_addend_struct1 = internal constant i32 add (i32 ptrtoint (<{ [3 x i8], i32 }>* @compound to i32), i32 1)
; CHECK: @const_addend_struct1 = internal constant i32 add (i32 ptrtoint (<{ [3 x i8], i32 }>* @compound to i32), i32 1)
; CHECK:	.type	const_addend_struct1,@object
; CHECK:	.section	.rodata,"a",@progbits
; CHECK:const_addend_struct1:
; CHECK:	.long	compound + 1
; CHECK:	.size	const_addend_struct1, 4

@addend_struct2 = internal global i32 add (i32 ptrtoint (<{ [3 x i8], i32 }>* @compound to i32), i32 4)
; CHECK: @addend_struct2 = internal global i32 add (i32 ptrtoint (<{ [3 x i8], i32 }>* @compound to i32), i32 4)
; CHECK:	.type	addend_struct2,@object
; CHECK:	.section	.data,"aw",@progbits
; CHECK:addend_struct2:
; CHECK:	.long	compound + 4
; CHECK:	.size	addend_struct2, 4

@const_addend_struct2 = internal constant i32 add (i32 ptrtoint (<{ [3 x i8], i32 }>* @compound to i32), i32 4)
; CHECK: @const_addend_struct2 = internal constant i32 add (i32 ptrtoint (<{ [3 x i8], i32 }>* @compound to i32), i32 4)
; CHECK:	.type	const_addend_struct2,@object
; CHECK:	.section	.rodata,"a",@progbits
; CHECK:const_addend_struct2:
; CHECK:	.long	compound + 4
; CHECK:	.size	const_addend_struct2, 4

@ptr_to_func_align = internal global i32 ptrtoint (void ()* @func to i32), align 8
; CHECK: @ptr_to_func_align = internal global i32 ptrtoint (void ()* @func to i32), align 8
; CHECK:	.type	ptr_to_func_align,@object
; CHECK:	.section	.data,"aw",@progbits
; CHECK:	.align	8
; CHECK:ptr_to_func_align:
; CHECK:	.long	func
; CHECK:	.size	ptr_to_func_align, 4

@const_ptr_to_func_align = internal constant i32 ptrtoint (void ()* @func to i32), align 8
; CHECK: @const_ptr_to_func_align = internal constant i32 ptrtoint (void ()* @func to i32), align 8
; CHECK:	.type	const_ptr_to_func_align,@object
; CHECK:	.section	.rodata,"a",@progbits
; CHECK:	.align	8
; CHECK:const_ptr_to_func_align:
; CHECK:	.long	func
; CHECK:	.size	const_ptr_to_func_align, 4

@char = internal constant [1 x i8] c"0"
; CHECK: @char = internal constant [1 x i8] c"0"
; CHECK:	.type	char,@object
; CHECK:	.section	.rodata,"a",@progbits
; CHECK:char:
; CHECK:	.byte	48
; CHECK:	.size	char, 1

@short = internal constant [2 x i8] zeroinitializer
; CHECK: @short = internal constant [2 x i8] zeroinitializer
; CHECK:	.type	short,@object
; CHECK:	.section	.rodata,"a",@progbits
; CHECK:short:
; CHECK:	.zero	2
; CHECK:	.size	short, 2

define void @func() {
  ret void
}

; CHECK: define void @func() {

