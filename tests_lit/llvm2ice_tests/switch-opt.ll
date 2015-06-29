; This tests a switch statement, including multiple branches to the
; same label which also results in phi instructions with multiple
; entries for the same incoming edge.

; RUN: %p2i -i %s --filetype=obj --disassemble --args -O2 | FileCheck %s

; TODO(jvoung): Update to -02 once the phi assignments is done for ARM
; RUN: %if --need=target_ARM32 --command %p2i --filetype=asm --assemble \
; RUN:   --disassemble --target arm32 -i %s --args -Om1 --skip-unimplemented \
; RUN:   | %if --need=target_ARM32 --command FileCheck --check-prefix ARM32 %s

define i32 @testSwitch(i32 %a) {
entry:
  switch i32 %a, label %sw.default [
    i32 1, label %sw.epilog
    i32 2, label %sw.epilog
    i32 3, label %sw.epilog
    i32 7, label %sw.bb1
    i32 8, label %sw.bb1
    i32 15, label %sw.bb2
    i32 14, label %sw.bb2
  ]

sw.default:                                       ; preds = %entry
  %add = add i32 %a, 27
  br label %sw.epilog

sw.bb1:                                           ; preds = %entry, %entry
  %phitmp = sub i32 21, %a
  br label %sw.bb2

sw.bb2:                                           ; preds = %sw.bb1, %entry, %entry
  %result.0 = phi i32 [ 1, %entry ], [ 1, %entry ], [ %phitmp, %sw.bb1 ]
  br label %sw.epilog

sw.epilog:                                        ; preds = %sw.bb2, %sw.default, %entry, %entry, %entry
  %result.1 = phi i32 [ %add, %sw.default ], [ %result.0, %sw.bb2 ], [ 17, %entry ], [ 17, %entry ], [ 17, %entry ]
  ret i32 %result.1
}

; Check for a valid addressing mode when the switch operand is an
; immediate.  It's important that there is exactly one case, because
; for two or more cases the source operand is legalized into a
; register.
define i32 @testSwitchImm() {
entry:
  switch i32 10, label %sw.default [
    i32 1, label %sw.default
  ]

sw.default:
  ret i32 20
}
; CHECK-LABEL: testSwitchImm
; CHECK-NOT: cmp 0x{{[0-9a-f]*}},

; ARM32-LABEL: testSwitchImm
; ARM32: cmp {{r[0-9]+}}, #1
; ARM32-NEXT: beq
; ARM32-NEXT: b

; Test for correct 64-bit lowering.
define internal i32 @testSwitch64(i64 %a) {
entry:
  switch i64 %a, label %sw.default [
    i64 123, label %return
    i64 234, label %sw.bb1
    i64 345, label %sw.bb2
    i64 78187493520, label %sw.bb3
  ]

sw.bb1:                                           ; preds = %entry
  br label %return

sw.bb2:                                           ; preds = %entry
  br label %return

sw.bb3:                                           ; preds = %entry
  br label %return

sw.default:                                       ; preds = %entry
  br label %return

return:                                           ; preds = %sw.default, %sw.bb3, %sw.bb2, %sw.bb1, %entry
  %retval.0 = phi i32 [ 5, %sw.default ], [ 4, %sw.bb3 ], [ 3, %sw.bb2 ], [ 2, %sw.bb1 ], [ 1, %entry ]
  ret i32 %retval.0
}
; CHECK-LABEL: testSwitch64
; CHECK: cmp {{.*}},0x7b
; CHECK-NEXT: jne
; CHECK-NEXT: cmp {{.*}},0x0
; CHECK-NEXT: je
; CHECK: cmp {{.*}},0xea
; CHECK-NEXT: jne
; CHECK-NEXT: cmp {{.*}},0x0
; CHECK-NEXT: je
; CHECK: cmp {{.*}},0x159
; CHECK-NEXT: jne
; CHECK-NEXT: cmp {{.*}},0x0
; CHECK-NEXT: je
; CHECK: cmp {{.*}},0x34567890
; CHECK-NEXT: jne
; CHECK-NEXT: cmp {{.*}},0x12
; CHECK-NEXT: je

; ARM32-LABEL: testSwitch64
; ARM32: cmp {{r[0-9]+}}, #123
; ARM32-NEXT: cmpeq {{r[0-9]+}}, #0
; ARM32-NEXT: beq
; ARM32: cmp {{r[0-9]+}}, #234
; ARM32-NEXT: cmpeq {{r[0-9]+}}, #0
; ARM32-NEXT: beq
; ARM32: movw [[REG:r[0-9]+]], #345
; ARM32-NEXT: cmp {{r[0-9]+}}, [[REG]]
; ARM32-NEXT: cmpeq {{r[0-9]+}}, #0
; ARM32-NEXT: beq
; ARM32: movw [[REG:r[0-9]+]], #30864
; ARM32-NEXT: movt [[REG]], #13398
; ARM32-NEXT: cmp {{r[0-9]+}}, [[REG]]
; ARM32-NEXT: cmpeq {{r[0-9]+}}, #18
; ARM32-NEXT: beq
; ARM32-NEXT: b

; Similar to testSwitchImm, make sure proper addressing modes are
; used.  In reality, this is tested by running the output through the
; assembler.
define i32 @testSwitchImm64() {
entry:
  switch i64 10, label %sw.default [
    i64 1, label %sw.default
  ]

sw.default:
  ret i32 20
}
; CHECK-LABEL: testSwitchImm64
; CHECK: cmp {{.*}},0x1
; CHECK-NEXT: jne
; CHECK-NEXT: cmp {{.*}},0x0
; CHECK-NEXT: je

; ARM32-LABEL: testSwitchImm64
; ARM32: cmp {{r[0-9]+}}, #1
; ARM32-NEXT: cmpeq {{r[0-9]+}}, #0
; ARM32-NEXT: beq [[ADDR:[0-9a-f]+]]
; ARM32-NEXT: b [[ADDR]]

