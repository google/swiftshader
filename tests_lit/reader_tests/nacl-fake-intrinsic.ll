; Tests that we don't get fooled by a fake NaCl intrinsic.

; TODO(kschimpf) Find way to run this through p2i. Note: Can't do this
;                currently because run-llvm2ice.py raises exception on error,
;                and output is lost.
; RUN: %if --need=allow_dump --command \
; RUN:   llvm-as < %s \
; RUN: | %if --need=allow_dump --command \
; RUN:   pnacl-freeze -allow-local-symbol-tables \
; RUN: | %if --need=allow_dump --command \
; RUN:   not %llvm2ice -notranslate -verbose=inst -build-on-read \
; RUN:       -allow-pnacl-reader-error-recovery \
; RUN:       -allow-local-symbol-tables \
; RUN: | %if --need=allow_dump --command \
; RUN:   FileCheck %s

; RUN: %if --need=no_dump --command \
; RUN:   llvm-as < %s \
; RUN: | %if --need=no_dump --command \
; RUN:   pnacl-freeze -allow-local-symbol-tables \
; RUN: | %if --need=no_dump --command \
; RUN:   not %llvm2ice -notranslate -verbose=inst -build-on-read \
; RUN:        -allow-pnacl-reader-error-recovery -allow-local-symbol-tables \
; RUN: | %if --need=no_dump --command \
; RUN:   FileCheck --check-prefix=MIN %s

declare i32 @llvm.fake.i32(i32)

define i32 @testFake(i32 %v) {
  %r = call i32 @llvm.fake.i32(i32 %v)
  ret i32 %r
}

; CHECK: Error: ({{.*}}) Invalid PNaCl intrinsic call to llvm.fake.i32
; MIN: Error: ({{.*}}) Invalid input record

