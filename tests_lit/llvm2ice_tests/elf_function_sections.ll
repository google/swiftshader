; Tests filetype=obj with -ffunction-sections.

; RUN: %p2i -i %s --filetype=obj --args -O2 --verbose none -o %t \
; RUN:     -ffunction-sections && \
; RUN:   llvm-readobj -file-headers -sections -section-data \
; RUN:     -relocations -symbols %t | FileCheck %s

; RUN: %p2i -i %s --args -O2 --verbose none -ffunction-sections \
; RUN:   | llvm-mc -triple=i686-none-nacl -filetype=obj -o - \
; RUN:   | llvm-readobj -file-headers -sections -section-data \
; RUN:       -relocations -symbols - | FileCheck %s

declare void @llvm.memcpy.p0i8.p0i8.i32(i8*, i8*, i32, i32, i1)

define internal i32 @foo(i32 %x, i32 %len) {
  %y = add i32 %x, %x
  %dst = inttoptr i32 %y to i8*
  %src = inttoptr i32 %x to i8*
  call void @llvm.memcpy.p0i8.p0i8.i32(i8* %dst, i8* %src,
                                       i32 %len, i32 1, i1 false)

  ret i32 %y
}

define internal i32 @bar(i32 %x) {
  ret i32 %x
}

define void @_start(i32 %x) {
  %y = call i32 @bar(i32 4)
  %ignored = call i32 @foo(i32 %x, i32 %y)
  ret void
}

; CHECK:   Section {
; CHECK:     Name: .text.foo
; CHECK:     Type: SHT_PROGBITS
; CHECK:     Flags [ (0x6)
; CHECK:       SHF_ALLOC
; CHECK:       SHF_EXECINSTR
; CHECK:     ]
; CHECK:   }
; CHECK:   Section {
; CHECK:     Name: .rel.text.foo
; CHECK:     Type: SHT_REL
; CHECK:     Flags [ (0x0)
; CHECK:     ]
; CHECK:   }

; CHECK:   Section {
; CHECK:     Name: .text.bar
; CHECK:     Type: SHT_PROGBITS
; CHECK:     Flags [ (0x6)
; CHECK:       SHF_ALLOC
; CHECK:       SHF_EXECINSTR
; CHECK:     ]
; CHECK:   }

; CHECK:   Section {
; CHECK:     Name: .text._start
; CHECK:     Type: SHT_PROGBITS
; CHECK:     Flags [ (0x6)
; CHECK:       SHF_ALLOC
; CHECK:       SHF_EXECINSTR
; CHECK:     ]
; CHECK:   }
; CHECK:   Section {
; CHECK:     Name: .rel.text._start
; CHECK:     Type: SHT_REL
; CHECK:     Flags [ (0x0)
; CHECK:     ]
; CHECK:     )
; CHECK:   }

; CHECK: Relocations [
; CHECK:   Section ({{[0-9]+}}) .rel.text.foo {
; CHECK:     0x21 R_386_PC32 memcpy 0x0
; CHECK:   }
;   Relocation can be against the start of the section or
;   the function's symbol itself.
; CHECK:   Section ({{[0-9]+}}) .rel.text._start {
; CHECK:     0x13 R_386_PC32 {{.*}}bar 0x0
; CHECK:     0x25 R_386_PC32 {{.*}}foo 0x0
; CHECK:   }
; CHECK: ]

; CHECK: Symbols [
; CHECK:          Name: bar
; CHECK:          Name: foo
; CHECK:          Name: _start
; CHECK:          Name: memcpy
; CHECK: ]
