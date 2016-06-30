; Verify that ASan properly catches and reports bugs

; REQUIRES: no_minimal_build

; check with a one off the end local access
; RUN: llvm-as %s -o - | pnacl-freeze > %t.pexe && %S/../../pydir/szbuild.py \
; RUN:     --fsanitize-address --sz="-allow-externally-defined-symbols" \
; RUN:     %t.pexe -o %t && %t 2>&1 | FileCheck %s

; check with a many off the end local access
; RUN: llvm-as %s -o - | pnacl-freeze > %t.pexe && %S/../../pydir/szbuild.py \
; RUN:     --fsanitize-address --sz="-allow-externally-defined-symbols" \
; RUN:     %t.pexe -o %t && %t 1 2>&1 | FileCheck %s

; check with a one before the front local access
; RUN: llvm-as %s -o - | pnacl-freeze > %t.pexe && %S/../../pydir/szbuild.py \
; RUN:     --fsanitize-address --sz="-allow-externally-defined-symbols" \
; RUN:     %t.pexe -o %t && %t 1 2 2>&1 | FileCheck %s

; check with a one off the end global access
; RUN: llvm-as %s -o - | pnacl-freeze > %t.pexe && %S/../../pydir/szbuild.py \
; RUN:     --fsanitize-address --sz="-allow-externally-defined-symbols" \
; RUN:     %t.pexe -o %t && %t 1 2 3 2>&1 | FileCheck %s

; check with a many off the end global access
; RUN: llvm-as %s -o - | pnacl-freeze > %t.pexe && %S/../../pydir/szbuild.py \
; RUN:     --fsanitize-address --sz="-allow-externally-defined-symbols" \
; RUN:     %t.pexe -o %t && %t 1 2 3 4 2>&1 | FileCheck %s

; check with a one before the front global access
; RUN: llvm-as %s -o - | pnacl-freeze > %t.pexe && %S/../../pydir/szbuild.py \
; RUN:     --fsanitize-address --sz="-allow-externally-defined-symbols" \
; RUN:     %t.pexe -o %t && %t 1 2 3 4 5 2>&1 | FileCheck %s


declare external void @exit(i32)

; A global array
@array = internal constant [12 x i8] zeroinitializer

define i32 @access(i32 %is_local_i, i32 %err) {
  ; get the base pointer to either the local or global array
  %local = alloca i8, i32 12, align 1
  %global = bitcast [12 x i8]* @array to i8*
  %is_local = icmp ne i32 %is_local_i, 0
  %arr = select i1 %is_local, i8* %local, i8* %global

  ; determine the offset to access
  %err_offset = mul i32 %err, 4
  %pos_offset = add i32 %err_offset, 12
  %pos = icmp sge i32 %err_offset, 0
  %offset = select i1 %pos, i32 %pos_offset, i32 %err

  ; calculate the address to access
  %arraddr = ptrtoint i8* %arr to i32
  %badaddr = add i32 %arraddr, %offset
  %badptr = inttoptr i32 %badaddr to i32*

  ; perform the bad access
  %result = load i32, i32* %badptr, align 1
  ret i32 %result
}

; use argc to determine which test routine to run
define void @_start(i32 %arg) {
  %argcaddr = add i32 %arg, 8
  %argcptr = inttoptr i32 %argcaddr to i32*
  %argc = load i32, i32* %argcptr, align 1
  switch i32 %argc, label %error [i32 1, label %one_local
                                  i32 2, label %many_local
                                  i32 3, label %neg_local
                                  i32 4, label %one_global
                                  i32 5, label %many_global
                                  i32 6, label %neg_global]
one_local:
  ; Access one past the end of a local
  call i32 @access(i32 1, i32 0)
  br label %error
many_local:
  ; Access five past the end of a local
  call i32 @access(i32 1, i32 4)
  br label %error
neg_local:
  ; Access one before the beginning of a local
  call i32 @access(i32 1, i32 -1)
  br label %error
one_global:
  ; Access one past the end of a global
  call i32 @access(i32 0, i32 0)
  br label %error
many_global:
  ; Access five past the end of a global
  call i32 @access(i32 0, i32 4)
  br label %error
neg_global:
  ; Access one before the beginning of a global
  call i32 @access(i32 0, i32 -1)
  br label %error
error:
  call void @exit(i32 1)
  unreachable
}

; CHECK: Illegal access of 4 bytes at
