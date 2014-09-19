; Tests that we don't get fooled by a fake NaCl intrinsic.


; RUN: llvm-as < %s | pnacl-freeze -allow-local-symbol-tables \
; RUN:              | not %llvm2ice -notranslate -verbose=inst -build-on-read \
; RUN:                -allow-pnacl-reader-error-recovery \
; RUN:                -allow-local-symbol-tables \
; RUN:              | FileCheck %s

declare i32 @llvm.fake.i32(i32)

define i32 @testFake(i32 %v) {
  %r = call i32 @llvm.fake.i32(i32 %v)
  ret i32 %r
}

; CHECK: Error: (218:6) Invalid PNaCl intrinsic call to llvm.fake.i32

