; This file tests support for the select instruction with vector valued inputs.

; RUN: %p2i -i %s --filetype=obj --disassemble --args -O2 \
; RUN:   | FileCheck %s
; RUN: %p2i -i %s --filetype=obj --disassemble --args -Om1 \
; RUN:   | FileCheck %s
; RUN: %p2i -i %s --filetype=obj --disassemble --args -O2 -mattr=sse4.1 \
; RUN:   | FileCheck --check-prefix=SSE41 %s
; RUN: %p2i -i %s --filetype=obj --disassemble --args -Om1 -mattr=sse4.1 \
; RUN:   | FileCheck --check-prefix=SSE41 %s

; RUN: %if --need=target_MIPS32 --need=allow_dump \
; RUN:   --command %p2i --filetype=asm --assemble --disassemble --target mips32\
; RUN:   -i %s --args -O2 --skip-unimplemented \
; RUN:   | %if --need=target_MIPS32 --need=allow_dump \
; RUN:   --command FileCheck --check-prefix MIPS32 %s

define internal <16 x i8> @test_select_v16i8(<16 x i1> %cond, <16 x i8> %arg1,
                                             <16 x i8> %arg2) {
entry:
  %res = select <16 x i1> %cond, <16 x i8> %arg1, <16 x i8> %arg2
  ret <16 x i8> %res
; CHECK-LABEL: test_select_v16i8
; CHECK: pand
; CHECK: pandn
; CHECK: por

; SSE41-LABEL: test_select_v16i8
; SSE41: pblendvb xmm{{[0-7]}},{{xmm[0-7]|XMMWORD}}

; MIPS32-LABEL: test_select_v16i8
; MIPS32: lw [[T0:.*]],36(sp)
; MIPS32: lw [[T1:.*]],40(sp)
; MIPS32: lw [[T2:.*]],44(sp)
; MIPS32: lw [[T3:.*]],48(sp)
; MIPS32: lw [[T4:.*]],52(sp)
; MIPS32: lw [[T5:.*]],56(sp)
; MIPS32: lw [[T6:.*]],60(sp)
; MIPS32: lw [[T7:.*]],64(sp)
; MIPS32: move [[T8:.*]],zero
; MIPS32: move [[T9:.*]],zero
; MIPS32: move [[T10:.*]],zero
; MIPS32: move [[T11:.*]],zero
; MIPS32: andi [[T12:.*]],a0,0xff
; MIPS32: andi [[T12]],[[T12]],0x1
; MIPS32: andi [[T13:.*]],[[T0]],0xff
; MIPS32: andi [[T14:.*]],[[T4]],0xff
; MIPS32: movn [[T14]],[[T13]],[[T12]]
; MIPS32: andi [[T14]],[[T14]],0xff
; MIPS32: srl [[T8]],[[T8]],0x8
; MIPS32: sll [[T8]],[[T8]],0x8
; MIPS32: or [[T14]],[[T14]],[[T8]]
; MIPS32: srl [[T8]],a0,0x8
; MIPS32: andi [[T8]],[[T8]],0xff
; MIPS32: andi [[T8]],[[T8]],0x1
; MIPS32: srl [[T12]],[[T0]],0x8
; MIPS32: andi [[T12]],[[T12]],0xff
; MIPS32: srl [[T13]],[[T4]],0x8
; MIPS32: andi [[T13]],[[T13]],0xff
; MIPS32: movn [[T13]],[[T12]],[[T8]]
; MIPS32: andi [[T13]],[[T13]],0xff
; MIPS32: sll [[T13]],[[T13]],0x8
; MIPS32: lui [[T8]],0xffff
; MIPS32: ori [[T8]],[[T8]],0xff
; MIPS32: and [[T14]],[[T14]],[[T8]]
; MIPS32: or [[T13]],[[T13]],[[T14]]
; MIPS32: srl [[T8]],a0,0x10
; MIPS32: andi [[T8]],[[T8]],0xff
; MIPS32: andi [[T8]],[[T8]],0x1
; MIPS32: srl [[T12]],[[T0]],0x10
; MIPS32: andi [[T12]],[[T12]],0xff
; MIPS32: srl [[T14]],[[T4]],0x10
; MIPS32: andi [[T14]],[[T14]],0xff
; MIPS32: movn [[T14]],[[T12]],[[T8]]
; MIPS32: andi [[T14]],[[T14]],0xff
; MIPS32: sll [[T14]],[[T14]],0x10
; MIPS32: lui [[T8]],0xff00
; MIPS32: ori [[T8]],[[T8]],0xffff
; MIPS32: and [[T13]],[[T13]],[[T8]]
; MIPS32: or [[T14]],[[T14]],[[T13]]
; MIPS32: srl [[T15:.*]],a0,0x18
; MIPS32: andi [[T15]],[[T15]],0x1
; MIPS32: srl [[T0]],[[T0]],0x18
; MIPS32: srl [[T4]],[[T4]],0x18
; MIPS32: movn [[T4]],[[T0]],[[T15]]
; MIPS32: srl [[T4]],[[T4]],0x18
; MIPS32: sll [[T14]],[[T14]],0x8
; MIPS32: srl [[T14]],[[T14]],0x8
; MIPS32: or [[RV_E0:.*]],[[T4]],[[T14]]
; MIPS32: andi [[T0]],a1,0xff
; MIPS32: andi [[T0]],[[T0]],0x1
; MIPS32: andi [[T15]],[[T1]],0xff
; MIPS32: andi [[T8]],[[T5]],0xff
; MIPS32: movn [[T8]],[[T15]],[[T0]]
; MIPS32: andi [[T8]],[[T8]],0xff
; MIPS32: srl [[T9]],[[T9]],0x8
; MIPS32: sll [[T9]],[[T9]],0x8
; MIPS32: or [[T8]],[[T8]],[[T9]]
; MIPS32: srl [[T0]],a1,0x8
; MIPS32: andi [[T0]],[[T0]],0xff
; MIPS32: andi [[T0]],[[T0]],0x1
; MIPS32: srl [[T15]],[[T1]],0x8
; MIPS32: andi [[T15]],[[T15]],0xff
; MIPS32: srl [[T9]],[[T5]],0x8
; MIPS32: andi [[T9]],[[T9]],0xff
; MIPS32: movn [[T9]],[[T15]],[[T0]]
; MIPS32: andi [[T9]],[[T9]],0xff
; MIPS32: sll [[T9]],[[T9]],0x8
; MIPS32: lui [[T0]],0xffff
; MIPS32: ori [[T0]],[[T0]],0xff
; MIPS32: and [[T8]],[[T8]],[[T0]]
; MIPS32: or [[T9]],[[T9]],[[T8]]
; MIPS32: srl [[T0]],a1,0x10
; MIPS32: andi [[T0]],[[T0]],0xff
; MIPS32: andi [[T0]],[[T0]],0x1
; MIPS32: srl [[T15]],[[T1]],0x10
; MIPS32: andi [[T15]],[[T15]],0xff
; MIPS32: srl [[T8]],[[T5]],0x10
; MIPS32: andi [[T8]],[[T8]],0xff
; MIPS32: movn [[T8]],[[T15]],[[T0]]
; MIPS32: andi [[T8]],[[T8]],0xff
; MIPS32: sll [[T8]],[[T8]],0x10
; MIPS32: lui [[T0]],0xff00
; MIPS32: ori [[T0]],[[T0]],0xffff
; MIPS32: and [[T9]],[[T9]],[[T0]]
; MIPS32: or [[T8]],[[T8]],[[T9]]
; MIPS32: srl [[T16:.*]],a1,0x18
; MIPS32: andi [[T16]],[[T16]],0x1
; MIPS32: srl [[T1]],[[T1]],0x18
; MIPS32: srl [[T5]],[[T5]],0x18
; MIPS32: movn [[T5]],[[T1]],[[T16]]
; MIPS32: srl [[T5]],[[T5]],0x18
; MIPS32: sll [[T8]],[[T8]],0x8
; MIPS32: srl [[T8]],[[T8]],0x8
; MIPS32: or [[RV_E1:.*]],[[T5]],[[T8]]
; MIPS32: andi [[T0]],a2,0xff
; MIPS32: andi [[T0]],[[T0]],0x1
; MIPS32: andi [[T1]],[[T2]],0xff
; MIPS32: andi [[T15]],[[T6]],0xff
; MIPS32: movn [[T15]],[[T1]],[[T0]]
; MIPS32: andi [[T15]],[[T15]],0xff
; MIPS32: srl [[T10]],[[T10]],0x8
; MIPS32: sll [[T10]],[[T10]],0x8
; MIPS32: or [[T15]],[[T15]],[[T10]]
; MIPS32: srl [[T0]],a2,0x8
; MIPS32: andi [[T0]],[[T0]],0xff
; MIPS32: andi [[T0]],[[T0]],0x1
; MIPS32: srl [[T1]],[[T2]],0x8
; MIPS32: andi [[T1]],[[T1]],0xff
; MIPS32: srl [[T16]],[[T6]],0x8
; MIPS32: andi [[T16]],[[T16]],0xff
; MIPS32: movn [[T16]],[[T1]],[[T0]]
; MIPS32: andi [[T16]],[[T16]],0xff
; MIPS32: sll [[T16]],[[T16]],0x8
; MIPS32: lui [[T0]],0xffff
; MIPS32: ori [[T0]],[[T0]],0xff
; MIPS32: and [[T15]],[[T15]],[[T0]]
; MIPS32: or [[T16]],[[T16]],[[T15]]
; MIPS32: srl [[T0]],a2,0x10
; MIPS32: andi [[T0]],[[T0]],0xff
; MIPS32: andi [[T0]],[[T0]],0x1
; MIPS32: srl [[T1]],[[T2]],0x10
; MIPS32: andi [[T1]],[[T1]],0xff
; MIPS32: srl [[T15]],[[T6]],0x10
; MIPS32: andi [[T15]],[[T15]],0xff
; MIPS32: movn [[T15]],[[T1]],[[T0]]
; MIPS32: andi [[T15]],[[T15]],0xff
; MIPS32: sll [[T15]],[[T15]],0x10
; MIPS32: lui [[T0]],0xff00
; MIPS32: ori [[T0]],[[T0]],0xffff
; MIPS32: and [[T16]],[[T16]],[[T0]]
; MIPS32: or [[T15]],[[T15]],[[T16]]
; MIPS32: srl [[T17:.*]],a2,0x18
; MIPS32: andi [[T17]],[[T17]],0x1
; MIPS32: srl [[T2]],[[T2]],0x18
; MIPS32: srl [[T6]],[[T6]],0x18
; MIPS32: movn [[T6]],[[T2]],[[T17]]
; MIPS32: srl [[T6]],[[T6]],0x18
; MIPS32: sll [[T15]],[[T15]],0x8
; MIPS32: srl [[T15]],[[T15]],0x8
; MIPS32: or [[RV_E2:.*]],[[T6]],[[T15]]
; MIPS32: andi [[T0]],a3,0xff
; MIPS32: andi [[T0]],[[T0]],0x1
; MIPS32: andi [[T1]],[[T3]],0xff
; MIPS32: andi [[T15]],[[T7]],0xff
; MIPS32: movn [[T15]],[[T1]],[[T0]]
; MIPS32: andi [[T15]],[[T15]],0xff
; MIPS32: srl [[T11]],[[T11]],0x8
; MIPS32: sll [[T11]],[[T11]],0x8
; MIPS32: or [[T15]],[[T15]],[[T11]]
; MIPS32: srl [[T0]],a3,0x8
; MIPS32: andi [[T0]],[[T0]],0xff
; MIPS32: andi [[T0]],[[T0]],0x1
; MIPS32: srl [[T1]],[[T3]],0x8
; MIPS32: andi [[T1]],[[T1]],0xff
; MIPS32: srl [[T16]],[[T7]],0x8
; MIPS32: andi [[T16]],[[T16]],0xff
; MIPS32: movn [[T16]],[[T1]],[[T0]]
; MIPS32: andi [[T16]],[[T16]],0xff
; MIPS32: sll [[T16]],[[T16]],0x8
; MIPS32: lui [[T0]],0xffff
; MIPS32: ori [[T0]],[[T0]],0xff
; MIPS32: and [[T15]],[[T15]],[[T0]]
; MIPS32: or [[T16]],[[T16]],[[T15]]
; MIPS32: srl [[T0]],a3,0x10
; MIPS32: andi [[T0]],[[T0]],0xff
; MIPS32: andi [[T0]],[[T0]],0x1
; MIPS32: srl [[T1]],[[T3]],0x10
; MIPS32: andi [[T1]],[[T1]],0xff
; MIPS32: srl [[T15]],[[T7]],0x10
; MIPS32: andi [[T15]],[[T15]],0xff
; MIPS32: movn [[T15]],[[T1]],[[T0]]
; MIPS32: andi [[T15]],[[T15]],0xff
; MIPS32: sll [[T15]],[[T15]],0x10
; MIPS32: lui [[T0]],0xff00
; MIPS32: ori [[T0]],[[T0]],0xffff
; MIPS32: and [[T16]],[[T16]],[[T0]]
; MIPS32: or [[T15]],[[T15]],[[T16]]
; MIPS32: srl [[T18:.*]],a3,0x18
; MIPS32: andi [[T18]],[[T18]],0x1
; MIPS32: srl [[T3]],[[T3]],0x18
; MIPS32: srl [[T7]],[[T7]],0x18
; MIPS32: movn [[T7]],[[T3]],[[T18]]
; MIPS32: srl [[T7]],[[T7]],0x18
; MIPS32: sll [[T15]],[[T15]],0x8
; MIPS32: srl [[T15]],[[T15]],0x8
; MIPS32: or [[RV_E3:.*]],[[T7]],[[T15]]
}

define internal <16 x i1> @test_select_v16i1(<16 x i1> %cond, <16 x i1> %arg1,
                                             <16 x i1> %arg2) {
entry:
  %res = select <16 x i1> %cond, <16 x i1> %arg1, <16 x i1> %arg2
  ret <16 x i1> %res
; CHECK-LABEL: test_select_v16i1
; CHECK: pand
; CHECK: pandn
; CHECK: por

; SSE41-LABEL: test_select_v16i1
; SSE41: pblendvb xmm{{[0-7]}},{{xmm[0-7]|XMMWORD}}

; MIPS32-LABEL: test_select_v16i1
; MIPS32: lw [[T0:.*]],36(sp)
; MIPS32: lw [[T1:.*]],40(sp)
; MIPS32: lw [[T2:.*]],44(sp)
; MIPS32: lw [[T3:.*]],48(sp)
; MIPS32: lw [[T4:.*]],52(sp)
; MIPS32: lw [[T5:.*]],56(sp)
; MIPS32: lw [[T6:.*]],60(sp)
; MIPS32: lw [[T7:.*]],64(sp)
; MIPS32: move [[T8:.*]],zero
; MIPS32: move [[T9:.*]],zero
; MIPS32: move [[T10:.*]],zero
; MIPS32: move [[T11:.*]],zero
; MIPS32: andi [[T12:.*]],a0,0xff
; MIPS32: andi [[T12]],[[T12]],0x1
; MIPS32: andi [[T13:.*]],[[T0]],0xff
; MIPS32: andi [[T13]],[[T13]],0x1
; MIPS32: andi [[T14:.*]],[[T4]],0xff
; MIPS32: andi [[T14]],[[T14]],0x1
; MIPS32: movn [[T14]],[[T13]],[[T12]]
; MIPS32: andi [[T14]],[[T14]],0xff
; MIPS32: srl [[T8]],[[T8]],0x8
; MIPS32: sll [[T8]],[[T8]],0x8
; MIPS32: or [[T14]],[[T14]],[[T8]]
; MIPS32: srl [[T8]],a0,0x8
; MIPS32: andi [[T8]],[[T8]],0xff
; MIPS32: andi [[T8]],[[T8]],0x1
; MIPS32: srl [[T12]],[[T0]],0x8
; MIPS32: andi [[T12]],[[T12]],0xff
; MIPS32: andi [[T12]],[[T12]],0x1
; MIPS32: srl [[T13]],[[T4]],0x8
; MIPS32: andi [[T13]],[[T13]],0xff
; MIPS32: andi [[T13]],[[T13]],0x1
; MIPS32: movn [[T13]],[[T12]],[[T8]]
; MIPS32: andi [[T13]],[[T13]],0xff
; MIPS32: sll [[T13]],[[T13]],0x8
; MIPS32: lui [[T8]],0xffff
; MIPS32: ori [[T8]],[[T8]],0xff
; MIPS32: and [[T14]],[[T14]],[[T8]]
; MIPS32: or [[T13]],[[T13]],[[T14]]
; MIPS32: srl [[T8]],a0,0x10
; MIPS32: andi [[T8]],[[T8]],0xff
; MIPS32: andi [[T8]],[[T8]],0x1
; MIPS32: srl [[T12]],[[T0]],0x10
; MIPS32: andi [[T12]],[[T12]],0xff
; MIPS32: andi [[T12]],[[T12]],0x1
; MIPS32: srl [[T14]],[[T4]],0x10
; MIPS32: andi [[T14]],[[T14]],0xff
; MIPS32: andi [[T14]],[[T14]],0x1
; MIPS32: movn [[T14]],[[T12]],[[T8]]
; MIPS32: andi [[T14]],[[T14]],0xff
; MIPS32: sll [[T14]],[[T14]],0x10
; MIPS32: lui [[T8]],0xff00
; MIPS32: ori [[T8]],[[T8]],0xffff
; MIPS32: and [[T13]],[[T13]],[[T8]]
; MIPS32: or [[T14]],[[T14]],[[T13]]
; MIPS32: srl [[T15:.*]],a0,0x18
; MIPS32: andi [[T15]],[[T15]],0x1
; MIPS32: srl [[T0]],[[T0]],0x18
; MIPS32: andi [[T0]],[[T0]],0x1
; MIPS32: srl [[T4]],[[T4]],0x18
; MIPS32: andi [[T4]],[[T4]],0x1
; MIPS32: movn [[T4]],[[T0]],[[T15]]
; MIPS32: srl [[T4]],[[T4]],0x18
; MIPS32: sll [[T14]],[[T14]],0x8
; MIPS32: srl [[T14]],[[T14]],0x8
; MIPS32: or [[RV_E0:.*]],[[T4]],[[T14]]
; MIPS32: andi [[T0]],a1,0xff
; MIPS32: andi [[T0]],[[T0]],0x1
; MIPS32: andi [[T15]],[[T1]],0xff
; MIPS32: andi [[T15]],[[T15]],0x1
; MIPS32: andi [[T8]],[[T5]],0xff
; MIPS32: andi [[T8]],[[T8]],0x1
; MIPS32: movn [[T8]],[[T15]],[[T0]]
; MIPS32: andi [[T8]],[[T8]],0xff
; MIPS32: srl [[T9]],[[T9]],0x8
; MIPS32: sll [[T9]],[[T9]],0x8
; MIPS32: or [[T8]],[[T8]],[[T9]]
; MIPS32: srl [[T0]],a1,0x8
; MIPS32: andi [[T0]],[[T0]],0xff
; MIPS32: andi [[T0]],[[T0]],0x1
; MIPS32: srl [[T15]],[[T1]],0x8
; MIPS32: andi [[T15]],[[T15]],0xff
; MIPS32: andi [[T15]],[[T15]],0x1
; MIPS32: srl [[T9]],[[T5]],0x8
; MIPS32: andi [[T9]],[[T9]],0xff
; MIPS32: andi [[T9]],[[T9]],0x1
; MIPS32: movn [[T9]],[[T15]],[[T0]]
; MIPS32: andi [[T9]],[[T9]],0xff
; MIPS32: sll [[T9]],[[T9]],0x8
; MIPS32: lui [[T0]],0xffff
; MIPS32: ori [[T0]],[[T0]],0xff
; MIPS32: and [[T8]],[[T8]],[[T0]]
; MIPS32: or [[T9]],[[T9]],[[T8]]
; MIPS32: srl [[T0]],a1,0x10
; MIPS32: andi [[T0]],[[T0]],0xff
; MIPS32: andi [[T0]],[[T0]],0x1
; MIPS32: srl [[T15]],[[T1]],0x10
; MIPS32: andi [[T15]],[[T15]],0xff
; MIPS32: andi [[T15]],[[T15]],0x1
; MIPS32: srl [[T8]],[[T5]],0x10
; MIPS32: andi [[T8]],[[T8]],0xff
; MIPS32: andi [[T8]],[[T8]],0x1
; MIPS32: movn [[T8]],[[T15]],[[T0]]
; MIPS32: andi [[T8]],[[T8]],0xff
; MIPS32: sll [[T8]],[[T8]],0x10
; MIPS32: lui [[T0]],0xff00
; MIPS32: ori [[T0]],[[T0]],0xffff
; MIPS32: and [[T9]],[[T9]],[[T0]]
; MIPS32: or [[T8]],[[T8]],[[T9]]
; MIPS32: srl [[T16:.*]],a1,0x18
; MIPS32: andi [[T16]],[[T16]],0x1
; MIPS32: srl [[T1]],[[T1]],0x18
; MIPS32: andi [[T1]],[[T1]],0x1
; MIPS32: srl [[T5]],[[T5]],0x18
; MIPS32: andi [[T5]],[[T5]],0x1
; MIPS32: movn [[T5]],[[T1]],[[T16]]
; MIPS32: srl [[T5]],[[T5]],0x18
; MIPS32: sll [[T8]],[[T8]],0x8
; MIPS32: srl [[T8]],[[T8]],0x8
; MIPS32: or [[RV_E1:.*]],[[T5]],[[T8]]
; MIPS32: andi [[T0]],a2,0xff
; MIPS32: andi [[T0]],[[T0]],0x1
; MIPS32: andi [[T1]],[[T2]],0xff
; MIPS32: andi [[T1]],[[T1]],0x1
; MIPS32: andi [[T15]],[[T6]],0xff
; MIPS32: andi [[T15]],[[T15]],0x1
; MIPS32: movn [[T15]],[[T1]],[[T0]]
; MIPS32: andi [[T15]],[[T15]],0xff
; MIPS32: srl [[T10]],[[T10]],0x8
; MIPS32: sll [[T10]],[[T10]],0x8
; MIPS32: or [[T15]],[[T15]],[[T10]]
; MIPS32: srl [[T0]],a2,0x8
; MIPS32: andi [[T0]],[[T0]],0xff
; MIPS32: andi [[T0]],[[T0]],0x1
; MIPS32: srl [[T1]],[[T2]],0x8
; MIPS32: andi [[T1]],[[T1]],0xff
; MIPS32: andi [[T1]],[[T1]],0x1
; MIPS32: srl [[T16]],[[T6]],0x8
; MIPS32: andi [[T16]],[[T16]],0xff
; MIPS32: andi [[T16]],[[T16]],0x1
; MIPS32: movn [[T16]],[[T1]],[[T0]]
; MIPS32: andi [[T16]],[[T16]],0xff
; MIPS32: sll [[T16]],[[T16]],0x8
; MIPS32: lui [[T0]],0xffff
; MIPS32: ori [[T0]],[[T0]],0xff
; MIPS32: and [[T15]],[[T15]],[[T0]]
; MIPS32: or [[T16]],[[T16]],[[T15]]
; MIPS32: srl [[T0]],a2,0x10
; MIPS32: andi [[T0]],[[T0]],0xff
; MIPS32: andi [[T0]],[[T0]],0x1
; MIPS32: srl [[T1]],[[T2]],0x10
; MIPS32: andi [[T1]],[[T1]],0xff
; MIPS32: andi [[T1]],[[T1]],0x1
; MIPS32: srl [[T15]],[[T6]],0x10
; MIPS32: andi [[T15]],[[T15]],0xff
; MIPS32: andi [[T15]],[[T15]],0x1
; MIPS32: movn [[T15]],[[T1]],[[T0]]
; MIPS32: andi [[T15]],[[T15]],0xff
; MIPS32: sll [[T15]],[[T15]],0x10
; MIPS32: lui [[T0]],0xff00
; MIPS32: ori [[T0]],[[T0]],0xffff
; MIPS32: and [[T16]],[[T16]],[[T0]]
; MIPS32: or [[T15]],[[T15]],[[T16]]
; MIPS32: srl [[T17:.*]],a2,0x18
; MIPS32: andi [[T17]],[[T17]],0x1
; MIPS32: srl [[T2]],[[T2]],0x18
; MIPS32: andi [[T2]],[[T2]],0x1
; MIPS32: srl [[T6]],[[T6]],0x18
; MIPS32: andi [[T6]],[[T6]],0x1
; MIPS32: movn [[T6]],[[T2]],[[T17]]
; MIPS32: srl [[T6]],[[T6]],0x18
; MIPS32: sll [[T15]],[[T15]],0x8
; MIPS32: srl [[T15]],[[T15]],0x8
; MIPS32: or [[RV_E2:.*]],[[T6]],[[T15]]
; MIPS32: andi [[T0]],a3,0xff
; MIPS32: andi [[T0]],[[T0]],0x1
; MIPS32: andi [[T1]],[[T3]],0xff
; MIPS32: andi [[T1]],[[T1]],0x1
; MIPS32: andi [[T15]],[[T7]],0xff
; MIPS32: andi [[T15]],[[T15]],0x1
; MIPS32: movn [[T15]],[[T1]],[[T0]]
; MIPS32: andi [[T15]],[[T15]],0xff
; MIPS32: srl [[T11]],[[T11]],0x8
; MIPS32: sll [[T11]],[[T11]],0x8
; MIPS32: or [[T15]],[[T15]],[[T11]]
; MIPS32: srl [[T0]],a3,0x8
; MIPS32: andi [[T0]],[[T0]],0xff
; MIPS32: andi [[T0]],[[T0]],0x1
; MIPS32: srl [[T1]],[[T3]],0x8
; MIPS32: andi [[T1]],[[T1]],0xff
; MIPS32: andi [[T1]],[[T1]],0x1
; MIPS32: srl [[T16]],[[T7]],0x8
; MIPS32: andi [[T16]],[[T16]],0xff
; MIPS32: andi [[T16]],[[T16]],0x1
; MIPS32: movn [[T16]],[[T1]],[[T0]]
; MIPS32: andi [[T16]],[[T16]],0xff
; MIPS32: sll [[T16]],[[T16]],0x8
; MIPS32: lui [[T0]],0xffff
; MIPS32: ori [[T0]],[[T0]],0xff
; MIPS32: and [[T15]],[[T15]],[[T0]]
; MIPS32: or [[T16]],[[T16]],[[T15]]
; MIPS32: srl [[T0]],a3,0x10
; MIPS32: andi [[T0]],[[T0]],0xff
; MIPS32: andi [[T0]],[[T0]],0x1
; MIPS32: srl [[T1]],[[T3]],0x10
; MIPS32: andi [[T1]],[[T1]],0xff
; MIPS32: andi [[T1]],[[T1]],0x1
; MIPS32: srl [[T15]],[[T7]],0x10
; MIPS32: andi [[T15]],[[T15]],0xff
; MIPS32: andi [[T15]],[[T15]],0x1
; MIPS32: movn [[T15]],[[T1]],[[T0]]
; MIPS32: andi [[T15]],[[T15]],0xff
; MIPS32: sll [[T15]],[[T15]],0x10
; MIPS32: lui [[T0]],0xff00
; MIPS32: ori [[T0]],[[T0]],0xffff
; MIPS32: and [[T16]],[[T16]],[[T0]]
; MIPS32: or [[T15]],[[T15]],[[T16]]
; MIPS32: srl [[T18:.*]],a3,0x18
; MIPS32: andi [[T18]],[[T18]],0x1
; MIPS32: srl [[T3]],[[T3]],0x18
; MIPS32: andi [[T3]],[[T3]],0x1
; MIPS32: srl [[T7]],[[T7]],0x18
; MIPS32: andi [[T7]],[[T7]],0x1
; MIPS32: movn [[T7]],[[T3]],[[T18]]
; MIPS32: srl [[T7]],[[T7]],0x18
; MIPS32: sll [[T15]],[[T15]],0x8
; MIPS32: srl [[T15]],[[T15]],0x8
; MIPS32: or [[RV_E3:.*]],[[T7]],[[T15]]
}

define internal <8 x i16> @test_select_v8i16(<8 x i1> %cond, <8 x i16> %arg1,
                                             <8 x i16> %arg2) {
entry:
  %res = select <8 x i1> %cond, <8 x i16> %arg1, <8 x i16> %arg2
  ret <8 x i16> %res
; CHECK-LABEL: test_select_v8i16
; CHECK: pand
; CHECK: pandn
; CHECK: por

; SSE41-LABEL: test_select_v8i16
; SSE41: pblendvb xmm{{[0-7]}},{{xmm[0-7]|XMMWORD}}

; MIPS32-LABEL: test_select_v8i16
; MIPS32: lw [[T0:.*]],36(sp)
; MIPS32: lw [[T1:.*]],40(sp)
; MIPS32: lw [[T2:.*]],44(sp)
; MIPS32: lw [[T3:.*]],48(sp)
; MIPS32: lw [[T4:.*]],52(sp)
; MIPS32: lw [[T5:.*]],56(sp)
; MIPS32: lw [[T6:.*]],60(sp)
; MIPS32: lw [[T7:.*]],64(sp)
; MIPS32: move [[T8:.*]],zero
; MIPS32: move [[T9:.*]],zero
; MIPS32: move [[T10:.*]],zero
; MIPS32: move [[T11:.*]],zero
; MIPS32: andi [[T12:.*]],a0,0xffff
; MIPS32: andi [[T12]],[[T12]],0x1
; MIPS32: andi [[T13:.*]],[[T0]],0xffff
; MIPS32: andi [[T14:.*]],[[T4]],0xffff
; MIPS32: movn [[T14]],[[T13]],[[T12]]
; MIPS32: andi [[T14]],[[T14]],0xffff
; MIPS32: srl [[T8]],[[T8]],0x10
; MIPS32: sll [[T8]],[[T8]],0x10
; MIPS32: or [[T14]],[[T14]],[[T8]]
; MIPS32: srl [[T15:.*]],a0,0x10
; MIPS32: andi [[T15]],[[T15]],0x1
; MIPS32: srl [[T0]],[[T0]],0x10
; MIPS32: srl [[T4]],[[T4]],0x10
; MIPS32: movn [[T4]],[[T0]],[[T15]]
; MIPS32: sll [[T4]],[[T4]],0x10
; MIPS32: sll [[T14]],[[T14]],0x10
; MIPS32: srl [[T14]],[[T14]],0x10
; MIPS32: or [[RV_E0:.*]],[[T4]],[[T14]]
; MIPS32: andi [[T0]],a1,0xffff
; MIPS32: andi [[T0]],[[T0]],0x1
; MIPS32: andi [[T15]],[[T1]],0xffff
; MIPS32: andi [[T8]],[[T5]],0xffff
; MIPS32: movn [[T8]],[[T15]],[[T0]]
; MIPS32: andi [[T8]],[[T8]],0xffff
; MIPS32: srl [[T9]],[[T9]],0x10
; MIPS32: sll [[T9]],[[T9]],0x10
; MIPS32: or [[T8]],[[T8]],[[T9]]
; MIPS32: srl [[T16:.*]],a1,0x10
; MIPS32: andi [[T16]],[[T16]],0x1
; MIPS32: srl [[T1]],[[T1]],0x10
; MIPS32: srl [[T5]],[[T5]],0x10
; MIPS32: movn [[T5]],[[T1]],[[T16]]
; MIPS32: sll [[T5]],[[T5]],0x10
; MIPS32: sll [[T8]],[[T8]],0x10
; MIPS32: srl [[T8]],[[T8]],0x10
; MIPS32: or [[RV_E1:.*]],[[T5]],[[T8]]
; MIPS32: andi [[T0]],a2,0xffff
; MIPS32: andi [[T0]],[[T0]],0x1
; MIPS32: andi [[T1]],[[T2]],0xffff
; MIPS32: andi [[T15]],[[T6]],0xffff
; MIPS32: movn [[T15]],[[T1]],[[T0]]
; MIPS32: andi [[T15]],[[T15]],0xffff
; MIPS32: srl [[T10]],[[T10]],0x10
; MIPS32: sll [[T10]],[[T10]],0x10
; MIPS32: or [[T15]],[[T15]],[[T10]]
; MIPS32: srl [[T17:.*]],a2,0x10
; MIPS32: andi [[T17]],[[T17]],0x1
; MIPS32: srl [[T2]],[[T2]],0x10
; MIPS32: srl [[T6]],[[T6]],0x10
; MIPS32: movn [[T6]],[[T2]],[[T17]]
; MIPS32: sll [[T6]],[[T6]],0x10
; MIPS32: sll [[T15]],[[T15]],0x10
; MIPS32: srl [[T15]],[[T15]],0x10
; MIPS32: or [[RV_E2:.*]],[[T6]],[[T15]]
; MIPS32: andi [[T0]],a3,0xffff
; MIPS32: andi [[T0]],[[T0]],0x1
; MIPS32: andi [[T1]],[[T3]],0xffff
; MIPS32: andi [[T15]],[[T7]],0xffff
; MIPS32: movn [[T15]],[[T1]],[[T0]]
; MIPS32: andi [[T15]],[[T15]],0xffff
; MIPS32: srl [[T11]],[[T11]],0x10
; MIPS32: sll [[T11]],[[T11]],0x10
; MIPS32: or [[T15]],[[T15]],[[T11]]
; MIPS32: srl [[T18:.*]],a3,0x10
; MIPS32: andi [[T18]],[[T18]],0x1
; MIPS32: srl [[T3]],[[T3]],0x10
; MIPS32: srl [[T7]],[[T7]],0x10
; MIPS32: movn [[T7]],[[T3]],[[T18]]
; MIPS32: sll [[T7]],[[T7]],0x10
; MIPS32: sll [[T15]],[[T15]],0x10
; MIPS32: srl [[T15]],[[T15]],0x10
; MIPS32: or [[RV_E3:.*]],[[T7]],[[T15]]
}

define internal <8 x i1> @test_select_v8i1(<8 x i1> %cond, <8 x i1> %arg1,
                                           <8 x i1> %arg2) {
entry:
  %res = select <8 x i1> %cond, <8 x i1> %arg1, <8 x i1> %arg2
  ret <8 x i1> %res
; CHECK-LABEL: test_select_v8i1
; CHECK: pand
; CHECK: pandn
; CHECK: por

; SSE41-LABEL: test_select_v8i1
; SSE41: pblendvb xmm{{[0-7]}},{{xmm[0-7]|XMMWORD}}

; MIPS32-LABEL: test_select_v8i1
; MIPS32: lw [[T0:.*]],36(sp)
; MIPS32: lw [[T1:.*]],40(sp)
; MIPS32: lw [[T2:.*]],44(sp)
; MIPS32: lw [[T3:.*]],48(sp)
; MIPS32: lw [[T4:.*]],52(sp)
; MIPS32: lw [[T5:.*]],56(sp)
; MIPS32: lw [[T6:.*]],60(sp)
; MIPS32: lw [[T7:.*]],64(sp)
; MIPS32: move [[T8:.*]],zero
; MIPS32: move [[T9:.*]],zero
; MIPS32: move [[T10:.*]],zero
; MIPS32: move [[T11:.*]],zero
; MIPS32: andi [[T12:.*]],a0,0xffff
; MIPS32: andi [[T12]],[[T12]],0x1
; MIPS32: andi [[T13:.*]],[[T0]],0xffff
; MIPS32: andi [[T13]],[[T13]],0x1
; MIPS32: andi [[T14:.*]],[[T4]],0xffff
; MIPS32: andi [[T14]],[[T14]],0x1
; MIPS32: movn [[T14]],[[T13]],[[T12]]
; MIPS32: andi [[T14]],[[T14]],0xffff
; MIPS32: srl [[T8]],[[T8]],0x10
; MIPS32: sll [[T8]],[[T8]],0x10
; MIPS32: or [[T14]],[[T14]],[[T8]]
; MIPS32: srl [[T15:.*]],a0,0x10
; MIPS32: andi [[T15]],[[T15]],0x1
; MIPS32: srl [[T0]],[[T0]],0x10
; MIPS32: andi [[T0]],[[T0]],0x1
; MIPS32: srl [[T4]],[[T4]],0x10
; MIPS32: andi [[T4]],[[T4]],0x1
; MIPS32: movn [[T4]],[[T0]],[[T15]]
; MIPS32: sll [[T4]],[[T4]],0x10
; MIPS32: sll [[T14]],[[T14]],0x10
; MIPS32: srl [[T14]],[[T14]],0x10
; MIPS32: or [[RV_E0:.*]],[[T4]],[[T14]]
; MIPS32: andi [[T0]],a1,0xffff
; MIPS32: andi [[T0]],[[T0]],0x1
; MIPS32: andi [[T15]],[[T1]],0xffff
; MIPS32: andi [[T15]],[[T15]],0x1
; MIPS32: andi [[T8]],[[T5]],0xffff
; MIPS32: andi [[T8]],[[T8]],0x1
; MIPS32: movn [[T8]],[[T15]],[[T0]]
; MIPS32: andi [[T8]],[[T8]],0xffff
; MIPS32: srl [[T9]],[[T9]],0x10
; MIPS32: sll [[T9]],[[T9]],0x10
; MIPS32: or [[T8]],[[T8]],[[T9]]
; MIPS32: srl [[T16:.*]],a1,0x10
; MIPS32: andi [[T16]],[[T16]],0x1
; MIPS32: srl [[T1]],[[T1]],0x10
; MIPS32: andi [[T1]],[[T1]],0x1
; MIPS32: srl [[T5]],[[T5]],0x10
; MIPS32: andi [[T5]],[[T5]],0x1
; MIPS32: movn [[T5]],[[T1]],[[T16]]
; MIPS32: sll [[T5]],[[T5]],0x10
; MIPS32: sll [[T8]],[[T8]],0x10
; MIPS32: srl [[T8]],[[T8]],0x10
; MIPS32: or [[RV_E1:.*]],[[T5]],[[T8]]
; MIPS32: andi [[T0]],a2,0xffff
; MIPS32: andi [[T0]],[[T0]],0x1
; MIPS32: andi [[T1]],[[T2]],0xffff
; MIPS32: andi [[T1]],[[T1]],0x1
; MIPS32: andi [[T15]],[[T6]],0xffff
; MIPS32: andi [[T15]],[[T15]],0x1
; MIPS32: movn [[T15]],[[T1]],[[T0]]
; MIPS32: andi [[T15]],[[T15]],0xffff
; MIPS32: srl [[T10]],[[T10]],0x10
; MIPS32: sll [[T10]],[[T10]],0x10
; MIPS32: or [[T15]],[[T15]],[[T10]]
; MIPS32: srl [[T17:.*]],a2,0x10
; MIPS32: andi [[T17]],[[T17]],0x1
; MIPS32: srl [[T2]],[[T2]],0x10
; MIPS32: andi [[T2]],[[T2]],0x1
; MIPS32: srl [[T6]],[[T6]],0x10
; MIPS32: andi [[T6]],[[T6]],0x1
; MIPS32: movn [[T6]],[[T2]],[[T17]]
; MIPS32: sll [[T6]],[[T6]],0x10
; MIPS32: sll [[T15]],[[T15]],0x10
; MIPS32: srl [[T15]],[[T15]],0x10
; MIPS32: or [[RV_E2:.*]],[[T6]],[[T15]]
; MIPS32: andi [[T0]],a3,0xffff
; MIPS32: andi [[T0]],[[T0]],0x1
; MIPS32: andi [[T1]],[[T3]],0xffff
; MIPS32: andi [[T1]],[[T1]],0x1
; MIPS32: andi [[T15]],[[T7]],0xffff
; MIPS32: andi [[T15]],[[T15]],0x1
; MIPS32: movn [[T15]],[[T1]],[[T0]]
; MIPS32: andi [[T15]],[[T15]],0xffff
; MIPS32: srl [[T11]],[[T11]],0x10
; MIPS32: sll [[T11]],[[T11]],0x10
; MIPS32: or [[T15]],[[T15]],[[T11]]
; MIPS32: srl [[T18:.*]],a3,0x10
; MIPS32: andi [[T18]],[[T18]],0x1
; MIPS32: srl [[T3]],[[T3]],0x10
; MIPS32: andi [[T3]],[[T3]],0x1
; MIPS32: srl [[T7]],[[T7]],0x10
; MIPS32: andi [[T7]],[[T7]],0x1
; MIPS32: movn [[T7]],[[T3]],[[T18]]
; MIPS32: sll [[T7]],[[T7]],0x10
; MIPS32: sll [[T15]],[[T15]],0x10
; MIPS32: srl [[T15]],[[T15]],0x10
; MIPS32: or [[RV_E3:.*]],[[T7]],[[T15]]
}

define internal <4 x i32> @test_select_v4i32(<4 x i1> %cond, <4 x i32> %arg1,
                                             <4 x i32> %arg2) {
entry:
  %res = select <4 x i1> %cond, <4 x i32> %arg1, <4 x i32> %arg2
  ret <4 x i32> %res
; CHECK-LABEL: test_select_v4i32
; CHECK: pand
; CHECK: pandn
; CHECK: por

; SSE41-LABEL: test_select_v4i32
; SSE41: pslld xmm0,0x1f
; SSE41: blendvps xmm{{[0-7]}},{{xmm[0-7]|XMMWORD}}

; MIPS32-LABEL: test_select_v4i32
; MIPS32: lw [[T0:.*]],16(sp)
; MIPS32: lw [[T1:.*]],20(sp)
; MIPS32: lw [[T2:.*]],24(sp)
; MIPS32: lw [[T3:.*]],28(sp)
; MIPS32: lw [[T4:.*]],32(sp)
; MIPS32: lw [[T5:.*]],36(sp)
; MIPS32: lw [[T6:.*]],40(sp)
; MIPS32: lw [[T7:.*]],44(sp)
; MIPS32: andi [[T8:.*]],a0,0x1
; MIPS32: movn [[T4]],[[T0]],[[T8]]
; MIPS32: andi [[T9:.*]],a1,0x1
; MIPS32: movn [[T5]],[[T1]],[[T9]]
; MIPS32: andi [[T10:.*]],a2,0x1
; MIPS32: movn [[T6]],[[T2]],[[T10]]
; MIPS32: andi [[T11:.*]],a3,0x1
; MIPS32: movn [[T7]],[[T3]],[[T11]]
; MIPS32: move v0,[[T4]]
; MIPS32: move v1,[[T5]]
; MIPS32: move a0,[[T6]]
; MIPS32: move a1,[[T7]]
}

define internal <4 x float> @test_select_v4f32(
    <4 x i1> %cond, <4 x float> %arg1, <4 x float> %arg2) {
entry:
  %res = select <4 x i1> %cond, <4 x float> %arg1, <4 x float> %arg2
  ret <4 x float> %res
; CHECK-LABEL: test_select_v4f32
; CHECK: pand
; CHECK: pandn
; CHECK: por

; SSE41-LABEL: test_select_v4f32
; SSE41: pslld xmm0,0x1f
; SSE41: blendvps xmm{{[0-7]}},{{xmm[0-7]|XMMWORD}}

; MIPS32-LABEL: test_select_v4f32
; MIPS32: lw [[T0:.*]],16(sp)
; MIPS32: lw [[T1:.*]],20(sp)
; MIPS32: lw [[T2:.*]],24(sp)
; MIPS32: lw [[T3:.*]],28(sp)
; MIPS32: lw [[T4:.*]],32(sp)
; MIPS32: lw [[T5:.*]],36(sp)
; MIPS32: lw [[T6:.*]],40(sp)
; MIPS32: lw [[T7:.*]],44(sp)
; MIPS32: lw [[T8:.*]],48(sp)
; MIPS32: lw [[T9:.*]],52(sp)
; MIPS32: andi [[T10:.*]],a2,0x1
; MIPS32: mtc1 [[T2]],[[F0:.*]]
; MIPS32: mtc1 [[T6]],[[F1:.*]]
; MIPS32: movn.s [[T11:.*]],[[F0]],[[T10]]
; MIPS32: mfc1 v0,[[T11]]
; MIPS32: andi [[T12:.*]],a3,0x1
; MIPS32: mtc1 [[T3]],[[F0]]
; MIPS32: mtc1 [[T7]],[[T11]]
; MIPS32: movn.s [[T11]],[[F0]],[[T12]]
; MIPS32: mfc1 v1,[[T11]]
; MIPS32: andi [[T0]],[[T0]],0x1
; MIPS32: mtc1 [[T4]],[[F0]]
; MIPS32: mtc1 [[T8]],[[T11]]
; MIPS32: movn.s [[T11]],[[F0]],[[T0]]
; MIPS32: mfc1 a1,[[T11]]
; MIPS32: andi [[T1]],[[T1]],0x1
; MIPS32: mtc1 [[T5]],[[F0]]
; MIPS32: mtc1 [[T9]],[[T11]]
; MIPS32: movn.s [[T11]],[[F0]],[[T1]]
; MIPS32: mfc1 a2,[[T11]]
; MIPS32: move [[RET:.*]],a0
; MIPS32: sw v0,0([[RET]])
; MIPS32: sw v1,4([[RET]])
; MIPS32: sw a1,8([[RET]])
; MIPS32: sw a2,12([[RET]])
; MIPS32: move v0,a0
}

define internal <4 x i1> @test_select_v4i1(<4 x i1> %cond, <4 x i1> %arg1,
                                           <4 x i1> %arg2) {
entry:
  %res = select <4 x i1> %cond, <4 x i1> %arg1, <4 x i1> %arg2
  ret <4 x i1> %res
; CHECK-LABEL: test_select_v4i1
; CHECK: pand
; CHECK: pandn
; CHECK: por

; SSE41-LABEL: test_select_v4i1
; SSE41: pslld xmm0,0x1f
; SSE41: blendvps xmm{{[0-7]}},{{xmm[0-7]|XMMWORD}}

; MIPS32-LABEL: test_select_v4i1
; MIPS32: lw [[T0:.*]],16(sp)
; MIPS32: lw [[T1:.*]],20(sp)
; MIPS32: lw [[T2:.*]],24(sp)
; MIPS32: lw [[T3:.*]],28(sp)
; MIPS32: lw [[T4:.*]],32(sp)
; MIPS32: lw [[T5:.*]],36(sp)
; MIPS32: lw [[T6:.*]],40(sp)
; MIPS32: lw [[T7:.*]],44(sp)
; MIPS32: andi [[T8:.*]],a0,0x1
; MIPS32: andi [[T0]],[[T0]],0x1
; MIPS32: andi [[T4]],[[T4]],0x1
; MIPS32: movn [[T4]],[[T0]],[[T8]]
; MIPS32: andi [[T9:.*]],a1,0x1
; MIPS32: andi [[T1]],[[T1]],0x1
; MIPS32: andi [[T5]],[[T5]],0x1
; MIPS32: movn [[T5]],[[T1]],[[T9]]
; MIPS32: andi [[T10:.*]],a2,0x1
; MIPS32: andi [[T2]],[[T2]],0x1
; MIPS32: andi [[T6]],[[T6]],0x1
; MIPS32: movn [[T6]],[[T2]],[[T10]]
; MIPS32: andi [[T11:.*]],a3,0x1
; MIPS32: andi [[T3]],[[T3]],0x1
; MIPS32: andi [[T7]],[[T7]],0x1
; MIPS32: movn [[T7]],[[T3]],[[T11]]
; MIPS32: move v0,[[T4]]
; MIPS32: move v1,[[T5]]
; MIPS32: move a0,[[T6]]
; MIPS32: move a1,[[T7]]
}
