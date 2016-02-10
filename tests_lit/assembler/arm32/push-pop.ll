; Show that we know how to translate push and pop.
; TODO(kschimpf) Translate pop instructions.

; NOTE: We use -O2 to get rid of memory stores.

; REQUIRES: allow_dump

; Compile using standalone assembler.
; RUN: %p2i --filetype=asm -i %s --target=arm32 --args -O2 -allow-extern \
; RUN:   -reg-use r0,r1,r2,r3,r4,r5 | FileCheck %s --check-prefix=ASM

; Show bytes in assembled standalone code.
; RUN: %p2i --filetype=asm -i %s --target=arm32 --assemble --disassemble \
; RUN:   --args -O2 -allow-extern -reg-use r0,r1,r2,r3,r4,r5 \
; RUN:   | FileCheck %s --check-prefix=DIS

; Compile using integrated assembler.
; RUN: %p2i --filetype=iasm -i %s --target=arm32 --args -O2 \
; RUN:   -allow-extern -reg-use r0,r1,r2,r3,r4,r5 \
; RUN:   | FileCheck %s --check-prefix=IASM

; Show bytes in assembled integrated code.
; RUN: %p2i --filetype=iasm -i %s --target=arm32 --assemble --disassemble \
; RUN:   --args -O2 -allow-extern -reg-use r0,r1,r2,r3,r4,r5 \
; RUN:   | FileCheck %s --check-prefix=DIS

declare external void @DoSomething()

define internal void @SinglePushPop() {
; ASM-LABEL:SinglePushPop:
; DIS-LABEL:{{.+}} <SinglePushPop>:
; IASM-LABEL:SinglePushPop:

; ASM:    push    {lr}
; DIS:    {{.+}}  e52de004
; IASM-NOT: push

  call void @DoSomething();
  ret void

; ASM:    pop     {lr}
; DIS:    {{.+}}  e49de004
; IASM-NOT: pop

}

; This test is based on taking advantage of the over-eager -O2
; register allocator that puts V1 and V2 into callee-save registers,
; since the call instruction kills the scratch registers. This
; requires the callee-save registers to be pushed/popped in the
; prolog/epilog.
define internal i32 @MultPushPop(i32 %v1, i32 %v2) {
; ASM-LABEL:MultPushPop:
; DIS_LABEL: {{.+}} <MultPushPop>:
; IASM-LABEL:MultPushPop:
; ASM:    push    {r4, r5, lr}
; DIS:    {{.+}}: e92d4030

; IASM-NOT: push


  call void @DoSomething();
  %v3 = add i32 %v1, %v2
  ret i32 %v3

; ASM:    pop     {r4, r5, lr}
; DIS:    {{.+}}  e8bd4030
; IASM-NOT: pop

}
