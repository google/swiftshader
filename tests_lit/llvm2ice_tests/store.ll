; Simple test of the store instruction.

; REQUIRES: allow_dump

; RUN: %p2i -i %s --args --verbose inst -threads=0 | FileCheck %s

; RUN: %if --need=target_MIPS32 --need=allow_dump \
; RUN:   --command %p2i --filetype=asm --assemble \
; RUN:   --disassemble --target mips32 -i %s --args -O2 --skip-unimplemented \
; RUN:   -allow-externally-defined-symbols \
; RUN:   | %if --need=target_MIPS32 --need=allow_dump \
; RUN:   --command FileCheck --check-prefix MIPS32 %s

define internal void @store_i64(i32 %addr_arg) {
entry:
  %__1 = inttoptr i32 %addr_arg to i64*
  store i64 1, i64* %__1, align 1
  ret void

; CHECK:       Initial CFG
; CHECK:     entry:
; CHECK-NEXT:  store i64 1, i64* %addr_arg, align 1
; CHECK-NEXT:  ret void
}
; MIPS32-LABEL: store_i64
; MIPS32: li
; MIPS32: li
; MIPS32: sw
; MIPS32: sw

define internal void @store_i32(i32 %addr_arg) {
entry:
  %__1 = inttoptr i32 %addr_arg to i32*
  store i32 1, i32* %__1, align 1
  ret void

; CHECK:       Initial CFG
; CHECK:     entry:
; CHECK-NEXT:  store i32 1, i32* %addr_arg, align 1
; CHECK-NEXT:  ret void
}
; MIPS32-LABEL: store_i32
; MIPS32: li
; MIPS32: sw

define internal void @store_i16(i32 %addr_arg) {
entry:
  %__1 = inttoptr i32 %addr_arg to i16*
  store i16 1, i16* %__1, align 1
  ret void

; CHECK:       Initial CFG
; CHECK:     entry:
; CHECK-NEXT:  store i16 1, i16* %addr_arg, align 1
; CHECK-NEXT:  ret void
}
; MIPS32-LABEL: store_i16
; MIPS32: li
; MIPS32: sw

define internal void @store_i8(i32 %addr_arg) {
entry:
  %__1 = inttoptr i32 %addr_arg to i8*
  store i8 1, i8* %__1, align 1
  ret void

; CHECK:       Initial CFG
; CHECK:     entry:
; CHECK-NEXT:  store i8 1, i8* %addr_arg, align 1
; CHECK-NEXT:  ret void
}
; MIPS32-LABEL: store_i8
; MIPS32: li
; MIPS32: sw