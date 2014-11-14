; Tests that we don't get fooled by a fake NaCl intrinsic.

; TODO(kschimpf) Find way to run this through p2i. Note: Can't do this
;                currently because run-llvm2ice.py raises exception on error,
;                and output is lost.
; RUN: llvm-as < %s | pnacl-freeze -allow-local-symbol-tables \
; RUN:              | not %llvm2ice -notranslate -verbose=inst \
; RUN:                -allow-pnacl-reader-error-recovery \
; RUN:                -allow-local-symbol-tables \
; RUN:              | FileCheck %s

declare i32 @llvm.fake.i32(i32)

define i32 @testFake(i32 %v) {
  %r = call i32 @llvm.fake.i32(i32 %v)
  ret i32 %r
}

; CHECK: Error: (218:6) Invalid PNaCl intrinsic call to llvm.fake.i32

