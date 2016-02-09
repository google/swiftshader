; Show that we convert direct calls, into indirect calls (to handle far
;  branches).

; NOTE: We use -O2 to get rid of memory stores.

; REQUIRES: allow_dump

; Compile using standalone assembler.
; RUN: %p2i --filetype=asm -i %s --target=arm32 --args -O2 -allow-extern \
; RUN:   -reg-use r5 | FileCheck %s --check-prefix=ASM

; Show bytes in assembled standalone code.
; RUN: %p2i --filetype=asm -i %s --target=arm32 --assemble --disassemble \
; RUN:   --args -O2 -allow-extern -reg-use r5 | FileCheck %s --check-prefix=DIS

; Compile using integrated assembler.
; RUN: %p2i --filetype=iasm -i %s --target=arm32 --args -O2 \
; RUN:   -allow-extern -reg-use r5 | FileCheck %s --check-prefix=IASM

; Show bytes in assembled integrated code.
; RUN: %p2i --filetype=iasm -i %s --target=arm32 --assemble --disassemble \
; RUN:   --args -O2 -allow-extern -reg-use r5 | FileCheck %s --check-prefix=DIS

declare external void @doSomething()

define internal void @callSomething() {
; ASM-LABEL:callSomething:
; DIS-LABEL:{{.+}} <callSomething>:
; IASM-LABEL:callSomething:

  call void @doSomething();

; ASM:      movw        r5, #:lower16:doSomething
; DIS:      {{.+}}:     e3005000
; ASM-NOT:  movw

; ASM-NEXT: movt        r5, #:upper16:doSomething
; DIS-NEXT: {{.+}}:     e3405000
; ASM-NOT:  movt

; ASM-NEXT: blx r5
; DIS-NEXT: {{.+}}:     e12fff35
; ASM-NOT:  blx
  ret void
}
