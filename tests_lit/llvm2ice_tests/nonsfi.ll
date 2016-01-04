; RUN: %p2i -i %s --filetype=obj --assemble --disassemble --args -O2 -nonsfi=1 \
; RUN:   | FileCheck --check-prefix=NONSFI %s
; RUN: %p2i -i %s --filetype=obj --assemble --disassemble --args -O2 -nonsfi=0 \
; RUN:   | FileCheck --check-prefix=DEFAULT %s

@G1 = internal global [4 x i8] zeroinitializer, align 4
@G2 = internal global [4 x i8] zeroinitializer, align 4

define internal void @testCallRegular() {
entry:
  call void @testCallRegular()
  ret void
}
; Expect a simple direct call to testCallRegular.
; NONSFI-LABEL: testCallRegular
; NONSFI: call {{.*}} R_386_PC32 testCallRegular
; DEFAULT-LABEL: testCallRegular

define internal double @testCallBuiltin(double %val) {
entry:
  %result = frem double %val, %val
  ret double %result
}
; Expect a simple direct call to fmod.
; NONSFI-LABEL: testCallBuiltin
; NONSFI: call {{.*}} R_386_PC32 fmod
; DEFAULT-LABEL: testCallBuiltin

define internal i32 @testLoadBasic() {
entry:
  %a = bitcast [4 x i8]* @G1 to i32*
  %b = load i32, i32* %a, align 1
  ret i32 %b
}
; Expect a load with a R_386_GOTOFF relocation.
; NONSFI-LABEL: testLoadBasic
; NONSFI: mov {{.*}} R_386_GOTOFF G1
; DEFAULT-LABEL: testLoadBasic

define internal i32 @testLoadFixedOffset() {
entry:
  %a = ptrtoint [4 x i8]* @G1 to i32
  %a1 = add i32 %a, 4
  %a2 = inttoptr i32 %a1 to i32*
  %b = load i32, i32* %a2, align 1
  ret i32 %b
}
; Expect a load with a R_386_GOTOFF relocation plus an immediate offset.
; NONSFI-LABEL: testLoadFixedOffset
; NONSFI: mov {{.*}}+0x4] {{.*}} R_386_GOTOFF G1
; DEFAULT-LABEL: testLoadFixedOffset

define internal i32 @testLoadIndexed(i32 %idx) {
entry:
  %a = ptrtoint [4 x i8]* @G1 to i32
  %a0 = mul i32 %idx, 4
  %a1 = add i32 %a0, 12
  %a2 = add i32 %a1, %a
  %a3 = inttoptr i32 %a2 to i32*
  %b = load i32, i32* %a3, align 1
  ret i32 %b
}
; Expect a load with a R_386_GOTOFF relocation plus an immediate offset, plus a
; scaled index register.
; NONSFI-LABEL: testLoadIndexed
; NONSFI: mov {{.*}}*4+0xc] {{.*}} R_386_GOTOFF G1
; DEFAULT-LABEL: testLoadIndexed

define internal i32 @testLoadIndexedBase(i32 %base, i32 %idx) {
entry:
  %a = ptrtoint [4 x i8]* @G1 to i32
  %a0 = mul i32 %idx, 4
  %a1 = add i32 %a0, %base
  %a2 = add i32 %a1, %a
  %a3 = add i32 %a2, 12
  %a4 = inttoptr i32 %a3 to i32*
  %b = load i32, i32* %a4, align 1
  ret i32 %b
}
; Expect a load with a R_386_GOTOFF relocation plus an immediate offset, but
; without the scaled index.
; NONSFI-LABEL: testLoadIndexedBase
; NONSFI: mov {{.*}}*1+0xc] {{.*}} R_386_GOTOFF G1
; By contrast, without -nonsfi, expect a load with a *R_386_32* relocation plus
; an immediate offset, and *with* the scaled index.
; DEFAULT-LABEL: testLoadIndexedBase
; DEFAULT: mov {{.*}},DWORD PTR [{{.*}}+{{.*}}*4+0xc] {{.*}} R_386_32 G1

define internal i32 @testLoadOpt() {
entry:
  %a = bitcast [4 x i8]* @G1 to i32*
  %b = load i32, i32* %a, align 1
  %c = bitcast [4 x i8]* @G2 to i32*
  %d = load i32, i32* %c, align 1
  %e = add i32 %b, %d
  ret i32 %e
}
; Expect a load-folding optimization with a R_386_GOTOFF relocation.
; NONSFI-LABEL: testLoadOpt
; NONSFI: mov [[REG:e..]],{{.*}}+0x0] {{.*}} R_386_GOTOFF G1
; NONSFI-NEXT: add [[REG]],{{.*}}+0x0] {{.*}} R_386_GOTOFF G2
; DEFAULT-LABEL: testLoadOpt

define internal void @testRMW() {
entry:
  %a = bitcast [4 x i8]* @G1 to i32*
  %b = load i32, i32* %a, align 1
  %c = add i32 %b, 1234
  store i32 %c, i32* %a, align 1
  ret void
}
; Expect an RMW optimization with a R_386_GOTOFF relocation.
; NONSFI-LABEL: testRMW
; NONSFI: add DWORD PTR {{.*}}+0x0],0x4d2 {{.*}} R_386_GOTOFF G1
; DEFAULT-LABEL: testRMW
