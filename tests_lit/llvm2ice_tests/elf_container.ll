; Tests that we generate an ELF container with fields that make sense,
; cross-validating against llvm-mc.

; For the integrated ELF writer, we can't pipe the output because we need
; to seek backward and patch up the file headers. So, use a temporary file.
; RUN: %p2i -i %s --args -O2 --verbose none -elf-writer -o %t \
; RUN:   && llvm-readobj -file-headers -sections -section-data \
; RUN:       -relocations -symbols %t | FileCheck %s

; RUN: %p2i -i %s --args -O2 --verbose none \
; RUN:   | llvm-mc -triple=i686-none-nacl -filetype=obj -o - \
; RUN:   | llvm-readobj -file-headers -sections -section-data \
; RUN:       -relocations -symbols - | FileCheck %s

; Use intrinsics to test external calls.
declare void @llvm.memcpy.p0i8.p0i8.i32(i8*, i8*, i32, i32, i1)
declare void @llvm.memset.p0i8.i32(i8*, i8, i32, i32, i1)

; Test some global data relocs (data, rodata, bss).
@bytes = internal global [7 x i8] c"abcdefg", align 1
@const_bytes = internal constant [7 x i8] c"abcdefg", align 1

@ptr = internal global i32 ptrtoint ([7 x i8]* @bytes to i32), align 4
@const_ptr = internal constant i32 ptrtoint ([7 x i8]* @bytes to i32), align 4

@ptr_to_func = internal global i32 ptrtoint (double ()* @returnDoubleConst to i32), align 4
@const_ptr_to_func = internal constant i32 ptrtoint (double ()* @returnDoubleConst to i32), align 4

@short_zero = internal global [2 x i8] zeroinitializer, align 2
@double_zero = internal global [8 x i8] zeroinitializer, align 8
@const_short_zero = internal constant [2 x i8] zeroinitializer, align 2
@const_double_zero = internal constant [8 x i8] zeroinitializer, align 8


@addend_ptr = internal global i32 add (i32 ptrtoint (i32* @ptr to i32), i32 128)
@const_addend_ptr = internal constant i32 add (i32 ptrtoint (i32* @ptr to i32), i32 64)

; Use float/double constants to test constant pools.
define internal float @returnFloatConst() {
entry:
  %f = fadd float -0.0, 0x3FF3AE1400000000
  ret float %f
}

define internal double @returnDoubleConst() {
entry:
  %d = fadd double 0x7FFFFFFFFFFFFFFFF, 0xFFF7FFFFFFFFFFFF
  %d2 = fadd double %d, 0xFFF8000000000003
  ret double %d2
}

define internal void @test_memcpy(i32 %iptr_dst, i32 %len) {
entry:
  %dst = inttoptr i32 %iptr_dst to i8*
  %src = bitcast [7 x i8]* @bytes to i8*
  call void @llvm.memcpy.p0i8.p0i8.i32(i8* %dst, i8* %src,
                                       i32 %len, i32 1, i1 false)
  ret void
}

define internal void @test_memset(i32 %iptr_dst, i32 %wide_val, i32 %len) {
entry:
  %val = trunc i32 %wide_val to i8
  %dst = inttoptr i32 %iptr_dst to i8*
  call void @llvm.memset.p0i8.i32(i8* %dst, i8 %val,
                                  i32 %len, i32 1, i1 false)
  ret void
}

; Test non-internal functions too.
define void @_start(i32) {
  %f = call float @returnFloatConst()
  %d = call double @returnDoubleConst()
  call void @test_memcpy(i32 0, i32 99)
  call void @test_memset(i32 0, i32 0, i32 99)
  ret void
}


; CHECK: ElfHeader {
; CHECK:   Ident {
; CHECK:     Magic: (7F 45 4C 46)
; CHECK:     Class: 32-bit
; CHECK:     DataEncoding: LittleEndian
; CHECK:     OS/ABI: SystemV (0x0)
; CHECK:     ABIVersion: 0
; CHECK:     Unused: (00 00 00 00 00 00 00)
; CHECK:   }
; CHECK:   Type: Relocatable (0x1)
; CHECK:   Machine: EM_386 (0x3)
; CHECK:   Version: 1
; CHECK:   Entry: 0x0
; CHECK:   ProgramHeaderOffset: 0x0
; CHECK:   SectionHeaderOffset: 0x{{[1-9A-F][0-9A-F]*}}
; CHECK:   Flags [ (0x0)
; CHECK:   ]
; CHECK:   HeaderSize: 52
; CHECK:   ProgramHeaderEntrySize: 0
; CHECK:   ProgramHeaderCount: 0
; CHECK:   SectionHeaderEntrySize: 40
; CHECK:   SectionHeaderCount: {{[1-9][0-9]*}}
; CHECK:   StringTableSectionIndex: {{[1-9][0-9]*}}
; CHECK: }


; CHECK: Sections [
; CHECK:   Section {
; CHECK:     Index: 0
; CHECK:     Name: (0)
; CHECK:     Type: SHT_NULL
; CHECK:     Flags [ (0x0)
; CHECK:     ]
; CHECK:     Address: 0x0
; CHECK:     Offset: 0x0
; CHECK:     Size: 0
; CHECK:     Link: 0
; CHECK:     Info: 0
; CHECK:     AddressAlignment: 0
; CHECK:     EntrySize: 0
; CHECK:     SectionData (
; CHECK-NEXT: )
; CHECK:   }
; CHECK:   Section {
; CHECK:     Index: {{[1-9][0-9]*}}
; CHECK:     Name: .text
; CHECK:     Type: SHT_PROGBITS
; CHECK:     Flags [ (0x6)
; CHECK:       SHF_ALLOC
; CHECK:       SHF_EXECINSTR
; CHECK:     ]
; CHECK:     Address: 0x0
; CHECK:     Offset: 0x{{[1-9A-F][0-9A-F]*}}
; CHECK:     Size: {{[1-9][0-9]*}}
; CHECK:     Link: 0
; CHECK:     Info: 0
; CHECK:     AddressAlignment: 32
; CHECK:     EntrySize: 0
; CHECK:     SectionData (
;   There's probably halt padding (0xF4) in there somewhere.
; CHECK:       {{.*}}F4
; CHECK:     )
; CHECK:   }
; CHECK:   Section {
; CHECK:     Index: {{[1-9][0-9]*}}
; CHECK:     Name: .rodata.cst4
; CHECK:     Type: SHT_PROGBITS
; CHECK:     Flags [ (0x12)
; CHECK:       SHF_ALLOC
; CHECK:       SHF_MERGE
; CHECK:     ]
; CHECK:     Address: 0x0
; CHECK:     Offset: 0x{{[1-9A-F][0-9A-F]*}}
; CHECK:     Size: 8
; CHECK:     Link: 0
; CHECK:     Info: 0
; CHECK:     AddressAlignment: 4
; CHECK:     EntrySize: 4
; CHECK:     SectionData (
; CHECK:       0000: A0709D3F 00000080
; CHECK:     )
; CHECK:   }
; CHECK:   Section {
; CHECK:     Index: {{[1-9][0-9]*}}
; CHECK:     Name: .rodata.cst8
; CHECK:     Type: SHT_PROGBITS
; CHECK:     Flags [ (0x12)
; CHECK:       SHF_ALLOC
; CHECK:       SHF_MERGE
; CHECK:     ]
; CHECK:     Address: 0x0
; CHECK:     Offset: 0x{{[1-9A-F][0-9A-F]*}}
; CHECK:     Size: 24
; CHECK:     Link: 0
; CHECK:     Info: 0
; CHECK:     AddressAlignment: 8
; CHECK:     EntrySize: 8
; CHECK:     SectionData (
; CHECK:       0000: 03000000 0000F8FF FFFFFFFF FFFFF7FF
; CHECK:       0010: FFFFFFFF FFFFFFFF
; CHECK:     )
; CHECK:   }
; CHECK:   Section {
; CHECK:     Index: {{[1-9][0-9]*}}
; CHECK:     Name: .shstrtab
; CHECK:     Type: SHT_STRTAB
; CHECK:     Flags [ (0x0)
; CHECK:     ]
; CHECK:     Address: 0x0
; CHECK:     Offset: 0x{{[1-9A-F][0-9A-F]*}}
; CHECK:     Size: {{[1-9][0-9]*}}
; CHECK:     Link: 0
; CHECK:     Info: 0
; CHECK:     AddressAlignment: 1
; CHECK:     EntrySize: 0
; CHECK:     SectionData (
; CHECK:       {{.*}}.text{{.*}}
; CHECK:     )
; CHECK:   }
; CHECK:   Section {
; CHECK:     Index: {{[1-9][0-9]*}}
; CHECK:     Name: .symtab
; CHECK:     Type: SHT_SYMTAB
; CHECK:     Flags [ (0x0)
; CHECK:     ]
; CHECK:     Address: 0x0
; CHECK:     Offset: 0x{{[1-9A-F][0-9A-F]*}}
; CHECK:     Size: {{[1-9][0-9]*}}
; CHECK:     Link: [[STRTAB_INDEX:[1-9][0-9]*]]
; CHECK:     Info: [[GLOBAL_START_INDEX:[1-9][0-9]*]]
; CHECK:     AddressAlignment: 4
; CHECK:     EntrySize: 16
; CHECK:   }
; CHECK:   Section {
; CHECK:     Index: [[STRTAB_INDEX]]
; CHECK:     Name: .strtab
; CHECK:     Type: SHT_STRTAB
; CHECK:     Flags [ (0x0)
; CHECK:     ]
; CHECK:     Address: 0x0
; CHECK:     Offset: 0x{{[1-9A-F][0-9A-F]*}}
; CHECK:     Size: {{[1-9][0-9]*}}
; CHECK:     Link: 0
; CHECK:     Info: 0
; CHECK:     AddressAlignment: 1
; CHECK:     EntrySize: 0
; CHECK:   }


; CHECK: Relocations [
;  TODO: fill it out.
; CHECK: ]


; CHECK: Symbols [
; CHECK-NEXT:   Symbol {
; CHECK-NEXT:     Name: (0)
; CHECK-NEXT:     Value: 0x0
; CHECK-NEXT:     Size: 0
; CHECK-NEXT:     Binding: Local
; CHECK-NEXT:     Type: None
; CHECK-NEXT:     Other: 0
; CHECK-NEXT:     Section: Undefined (0x0)
; CHECK-NEXT:   }
;  TODO: fill in the data symbols.
; CHECK:        Symbol {
; CHECK:          Name: .L$double$0
; CHECK-NEXT:     Value: 0x10
; CHECK-NEXT:     Size: 0
; CHECK-NEXT:     Binding: Local (0x0)
; CHECK-NEXT:     Type: None (0x0)
; CHECK-NEXT:     Other: 0
; CHECK-NEXT:     Section: .rodata.cst8
; CHECK-NEXT:   }
; CHECK:        Symbol {
; CHECK:          Name: .L$double$2
; CHECK-NEXT:     Value: 0x0
; CHECK-NEXT:     Size: 0
; CHECK-NEXT:     Binding: Local (0x0)
; CHECK-NEXT:     Type: None (0x0)
; CHECK-NEXT:     Other: 0
; CHECK-NEXT:     Section: .rodata.cst8
; CHECK-NEXT:   }
; CHECK:        Symbol {
; CHECK:          Name: .L$float$0
; CHECK-NEXT:     Value: 0x4
; CHECK-NEXT:     Size: 0
; CHECK-NEXT:     Binding: Local (0x0)
; CHECK-NEXT:     Type: None (0x0)
; CHECK-NEXT:     Other: 0
; CHECK-NEXT:     Section: .rodata.cst4
; CHECK-NEXT:   }
; CHECK:        Symbol {
; CHECK:          Name: .L$float$1
; CHECK-NEXT:     Value: 0x0
; CHECK-NEXT:     Size: 0
; CHECK-NEXT:     Binding: Local (0x0)
; CHECK-NEXT:     Type: None (0x0)
; CHECK-NEXT:     Other: 0
; CHECK-NEXT:     Section: .rodata.cst4
; CHECK-NEXT:   }
; CHECK:        Symbol {
; CHECK:          Name: returnDoubleConst
; CHECK-NEXT:     Value: 0x{{[1-9A-F][0-9A-F]*}}
; CHECK-NEXT:     Size: 0
; CHECK-NEXT:     Binding: Local
; CHECK-NEXT:     Type: None
; CHECK-NEXT:     Other: 0
; CHECK-NEXT:     Section: .text
; CHECK-NEXT:   }
; CHECK:        Symbol {
; CHECK:          Name: returnFloatConst
;  This happens to be the first function, so its offset is 0 within the text.
; CHECK-NEXT:     Value: 0x0
; CHECK-NEXT:     Size: 0
; CHECK-NEXT:     Binding: Local
; CHECK-NEXT:     Type: None
; CHECK-NEXT:     Other: 0
; CHECK-NEXT:     Section: .text
; CHECK-NEXT:   }
; CHECK:        Symbol {
; CHECK:          Name: test_memcpy
; CHECK-NEXT:     Value: 0x{{[1-9A-F][0-9A-F]*}}
; CHECK-NEXT:     Size: 0
; CHECK-NEXT:     Binding: Local
; CHECK-NEXT:     Type: None
; CHECK-NEXT:     Other: 0
; CHECK-NEXT:     Section: .text
; CHECK-NEXT:   }
; CHECK:        Symbol {
; CHECK:          Name: _start
; CHECK-NEXT:     Value: 0x{{[1-9A-F][0-9A-F]*}}
; CHECK-NEXT:     Size: 0
; CHECK-NEXT:     Binding: Global
; CHECK-NEXT:     Type: Function
; CHECK-NEXT:     Other: 0
; CHECK-NEXT:     Section: .text
; CHECK-NEXT:   }
; CHECK: ]
