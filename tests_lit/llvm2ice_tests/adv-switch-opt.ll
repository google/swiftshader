; This tests the advanced lowering of switch statements. The advanced lowering
; uses jump tables, range tests and binary search.

; RUN: %p2i -i %s --target=x8632 --filetype=obj --disassemble --args -O2 \
; RUN:   | FileCheck %s --check-prefix=CHECK --check-prefix=X8632
; RUN: %p2i -i %s --target=x8664 --filetype=obj --disassemble --args -O2 \
; RUN:   | FileCheck %s --check-prefix=CHECK --check-prefix=X8664

; Dense but non-continuous ranges should be converted into a jump table.
define internal i32 @testJumpTable(i32 %a) {
entry:
  switch i32 %a, label %sw.default [
    i32 91, label %sw.default
    i32 92, label %sw.bb1
    i32 93, label %sw.default
    i32 99, label %sw.bb1
    i32 98, label %sw.default
    i32 96, label %sw.bb1
    i32 97, label %sw.epilog
  ]

sw.default:
  %add = add i32 %a, 27
  br label %sw.epilog

sw.bb1:
  %tmp = add i32 %a, 16
  br label %sw.epilog

sw.epilog:
  %result.1 = phi i32 [ %add, %sw.default ], [ %tmp, %sw.bb1 ], [ 17, %entry ]
  ret i32 %result.1
}
; CHECK-LABEL: testJumpTable
; CHECK: sub [[IND:[^,]+]],0x5b
; CHECK-NEXT: cmp [[IND]],0x8
; CHECK-NEXT: ja
; X8632-NEXT: mov [[TARGET:.*]],DWORD PTR {{\[}}[[IND]]*4+0x0] {{[0-9a-f]+}}: R_386_32 .{{.*}}testJumpTable$jumptable
; X8632-NEXT: jmp [[TARGET]]
; X8664-NEXT: mov {{.}}[[TARGET:.*]],DWORD PTR {{\[}}[[IND]]*4+0x0] {{[0-9a-f]+}}: R_X86_64_32S .{{.*}}testJumpTable$jumptable
; X8664-NEXT: jmp {{.}}[[TARGET]]
; Note: x86-32 may do "mov eax, [...]; jmp eax", whereas x86-64 may do
; "mov eax, [...]; jmp rax", so we assume the all characters except the first
; one in the register name will match.

; Continuous ranges which map to the same target should be grouped and
; efficiently tested.
define internal i32 @testRangeTest() {
entry:
  switch i32 10, label %sw.default [
    i32 0, label %sw.epilog
    i32 1, label %sw.epilog
    i32 2, label %sw.epilog
    i32 3, label %sw.epilog
    i32 10, label %sw.bb1
    i32 11, label %sw.bb1
    i32 12, label %sw.bb1
    i32 13, label %sw.bb1
  ]

sw.default:
  br label %sw.epilog

sw.bb1:
  br label %sw.epilog

sw.epilog:
  %result.1 = phi i32 [ 23, %sw.default ], [ 42, %sw.bb1 ], [ 17, %entry ], [ 17, %entry ], [ 17, %entry ], [ 17, %entry ]
  ret i32 %result.1
}
; CHECK-LABEL: testRangeTest
; CHECK: cmp {{.*}},0x3
; CHECK-NEXT: jbe
; CHECK: sub [[REG:[^,]*]],0xa
; CHECK-NEXT: cmp [[REG]],0x3
; CHECK-NEXT: jbe
; CHECK-NEXT: jmp

; Sparse cases should be searched with a binary search.
define internal i32 @testBinarySearch() {
entry:
  switch i32 10, label %sw.default [
    i32 0, label %sw.epilog
    i32 10, label %sw.epilog
    i32 20, label %sw.bb1
    i32 30, label %sw.bb1
  ]

sw.default:
  br label %sw.epilog

sw.bb1:
  br label %sw.epilog

sw.epilog:
  %result.1 = phi i32 [ 23, %sw.default ], [ 42, %sw.bb1 ], [ 17, %entry ], [ 17, %entry ]
  ret i32 %result.1
}
; CHECK-LABEL: testBinarySearch
; CHECK: cmp {{.*}},0x14
; CHECK-NEXT: jb
; CHECK-NEXT: je
; CHECK-NEXT: cmp {{.*}},0x1e
; CHECK-NEXT: je
; CHECK-NEXT: jmp
; CHECK-NEXT: cmp {{.*}},0x0
; CHECK-NEXT: je
; CHECK-NEXT: cmp {{.*}},0xa
; CHECK-NEXT: je
; CHECK-NEXT: jmp

; 64-bit switches where the cases are all 32-bit values should be reduced to a
; 32-bit switch after checking the top byte is 0.
define internal i32 @testSwitchSmall64(i64 %a) {
entry:
  switch i64 %a, label %sw.default [
    i64 123, label %return
    i64 234, label %sw.bb1
    i64 345, label %sw.bb2
    i64 456, label %sw.bb3
  ]

sw.bb1:
  br label %return

sw.bb2:
  br label %return

sw.bb3:
  br label %return

sw.default:
  br label %return

return:
  %retval.0 = phi i32 [ 5, %sw.default ], [ 4, %sw.bb3 ], [ 3, %sw.bb2 ], [ 2, %sw.bb1 ], [ 1, %entry ]
  ret i32 %retval.0
}
; CHECK-LABEL: testSwitchSmall64
; X8632: cmp {{.*}},0x0
; X8632-NEXT: jne
; X8632-NEXT: cmp {{.*}},0x159
; X8632-NEXT: jb
; X8632-NEXT: je
; X8632-NEXT: cmp {{.*}},0x1c8
; X8632-NEXT: je
; X8632-NEXT: jmp
; X8632-NEXT: cmp {{.*}},0x7b
; X8632-NEXT: je
; X8632-NEXT: cmp {{.*}},0xea
; X8632-NEXT: je

; Test for correct 64-bit lowering.
; TODO(ascull): this should generate better code like the 32-bit version
define internal i32 @testSwitch64(i64 %a) {
entry:
  switch i64 %a, label %sw.default [
    i64 123, label %return
    i64 234, label %sw.bb1
    i64 345, label %sw.bb2
    i64 78187493520, label %sw.bb3
  ]

sw.bb1:
  br label %return

sw.bb2:
  br label %return

sw.bb3:
  br label %return

sw.default:
  br label %return

return:
  %retval.0 = phi i32 [ 5, %sw.default ], [ 4, %sw.bb3 ], [ 3, %sw.bb2 ], [ 2, %sw.bb1 ], [ 1, %entry ]
  ret i32 %retval.0
}
; CHECK-LABEL: testSwitch64
; X8632: cmp {{.*}},0x7b
; X8632-NEXT: jne
; X8632-NEXT: cmp {{.*}},0x0
; X8632-NEXT: je
; X8632: cmp {{.*}},0xea
; X8632-NEXT: jne
; X8632-NEXT: cmp {{.*}},0x0
; X8632-NEXT: je
; X8632: cmp {{.*}},0x159
; X8632-NEXT: jne
; X8632-NEXT: cmp {{.*}},0x0
; X8632-NEXT: je
; X8632: cmp {{.*}},0x34567890
; X8632-NEXT: jne
; X8632-NEXT: cmp {{.*}},0x12
; X8632-NEXT: je

; Test for correct 64-bit jump table with UINT64_MAX as one of the values.
define internal i32 @testJumpTable64(i64 %a) {
entry:
  switch i64 %a, label %sw.default [
    i64 -6, label %return
    i64 -4, label %sw.bb1
    i64 -3, label %sw.bb2
    i64 -1, label %sw.bb3
  ]

sw.bb1:
  br label %return

sw.bb2:
  br label %return

sw.bb3:
  br label %return

sw.default:
  br label %return

return:
  %retval.0 = phi i32 [ 5, %sw.default ], [ 4, %sw.bb3 ], [ 3, %sw.bb2 ], [ 2, %sw.bb1 ], [ 1, %entry ]
  ret i32 %retval.0
}

; TODO(ascull): this should generate a jump table. For now, just make sure it
; doesn't crash the compiler.
; CHECK-LABEL: testJumpTable64
