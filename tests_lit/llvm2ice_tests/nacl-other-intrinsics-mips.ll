; RUN: %if --need=target_MIPS32 --need=allow_dump \
; RUN:   --command %p2i --filetype=asm --assemble --disassemble --target mips32\
; RUN:   -i %s --args -Om1 --skip-unimplemented \
; RUN:   -allow-externally-defined-symbols \
; RUN:   | %if --need=target_MIPS32 --need=allow_dump \
; RUN:   --command FileCheck --check-prefix MIPS32 %s

; RUN: %if --need=target_MIPS32 --need=allow_dump \
; RUN:   --command %p2i --filetype=asm --assemble --disassemble --target mips32\
; RUN:   -i %s --args -O2 --skip-unimplemented \
; RUN:   -allow-externally-defined-symbols \
; RUN:   | %if --need=target_MIPS32 --need=allow_dump \
; RUN:   --command FileCheck --check-prefix MIPS32-O2 %s

declare float @llvm.sqrt.f32(float)
declare double @llvm.sqrt.f64(double)
declare float @llvm.fabs.f32(float)
declare double @llvm.fabs.f64(double)

define internal float @test_sqrt_float(float %x, i32 %iptr) {
entry:
  %r = call float @llvm.sqrt.f32(float %x)
  %r2 = call float @llvm.sqrt.f32(float %r)
  %r3 = call float @llvm.sqrt.f32(float -0.0)
  %r4 = fadd float %r2, %r3
  ret float %r4
}
; MIPS32-LABEL: test_sqrt_float
; MIPS32: sqrt.s
; MIPS32: sqrt.s
; MIPS32: sqrt.s
; MIPS32: add.s
; MIPS32-O2-LABEL: test_sqrt_float
; MIPS32-O2: sqrt.s
; MIPS32-O2: sqrt.s
; MIPS32-O2: sqrt.s
; MIPS32-O2: add.s

define internal double @test_sqrt_double(double %x, i32 %iptr) {
entry:
  %r = call double @llvm.sqrt.f64(double %x)
  %r2 = call double @llvm.sqrt.f64(double %r)
  %r3 = call double @llvm.sqrt.f64(double -0.0)
  %r4 = fadd double %r2, %r3
  ret double %r4
}
; MIPS32-LABEL: test_sqrt_double
; MIPS32: sqrt.d
; MIPS32: sqrt.d
; MIPS32: sqrt.d
; MIPS32: add.d
; MIPS32-O2-LABEL: test_sqrt_double
; MIPS32-O2: sqrt.d
; MIPS32-O2: sqrt.d
; MIPS32-O2: sqrt.d
; MIPS32-O2: add.d

define internal float @test_sqrt_ignored(float %x, double %y) {
entry:
  %ignored1 = call float @llvm.sqrt.f32(float %x)
  %ignored2 = call double @llvm.sqrt.f64(double %y)
  ret float 0.0
}
; MIPS32-LABEL: test_sqrt_ignored
; MIPS32: sqrt.s
; MIPS32: sqrt.d
; MIPS32-O2-LABEL: test_sqrt_ignored

define internal float @test_fabs_float(float %x) {
entry:
  %r = call float @llvm.fabs.f32(float %x)
  %r2 = call float @llvm.fabs.f32(float %r)
  %r3 = call float @llvm.fabs.f32(float -0.0)
  %r4 = fadd float %r2, %r3
  ret float %r4
}
; MIPS32-LABEL: test_fabs_float
; MIPS32: abs.s
; MIPS32: abs.s
; MIPS32: abs.s
; MIPS32: add.s
; MIPS32-O2-LABEL: test_fabs_float
; MIPS32-O2: abs.s
; MIPS32-O2: abs.s
; MIPS32-O2: abs.s
; MIPS32-O2: add.s

define internal double @test_fabs_double(double %x) {
entry:
  %r = call double @llvm.fabs.f64(double %x)
  %r2 = call double @llvm.fabs.f64(double %r)
  %r3 = call double @llvm.fabs.f64(double -0.0)
  %r4 = fadd double %r2, %r3
  ret double %r4
}
; MIPS32-LABEL: test_fabs_double
; MIPS32: abs.d
; MIPS32: abs.d
; MIPS32: abs.d
; MIPS32: add.d
; MIPS32-O2-LABEL: test_fabs_double
; MIPS32-O2: abs.d
; MIPS32-O2: abs.d
; MIPS32-O2: abs.d
; MIPS32-O2: add.d
