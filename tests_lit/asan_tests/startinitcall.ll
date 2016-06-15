; Test for a call to __asan_init in _start

; REQUIRES: allow_dump

; RUN: %p2i -i %s --args -verbose=inst -threads=0 -fsanitize-address \
; RUN:     | FileCheck --check-prefix=DUMP %s

; notStart() should not be instrumented
define internal void @notStart() {
  ret void
}

; DUMP-LABEL: define internal void @notStart() { # notStart(i=1:b=1)
; DUMP-NEXT: __0:
; DUMP-NOT: __asan_init()
; DUMP: ret void
; DUMP-NEXT: }

; _start() should be instrumented
define void @_start() {
  ret void
}

; DUMP-LABEL: define void @_start() { # _start(i=2:b=1)
; DUMP-NEXT: __0:
; DUMP-NEXT: call void @__asan_init()
; DUMP-NEXT: ret void
; DUMP-NEXT: }
