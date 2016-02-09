; Tests validating the vfp calling convention for ARM32.
;
; RUN: %if --need=target_ARM32 --need=allow_dump \
; RUN:   --command %p2i --filetype=asm --assemble \
; RUN:   --disassemble --target arm32 -i %s --args -O2 --skip-unimplemented \
; RUN:   -allow-externally-defined-symbols \
; RUN:   | %if --need=target_ARM32 --need=allow_dump \
; RUN:   --command FileCheck %s
; RUN: %if --need=target_ARM32 --need=allow_dump \
; RUN:   --command %p2i --filetype=asm --assemble --disassemble --target arm32 \
; RUN:   -i %s --args -Om1 --skip-unimplemented \
; RUN:   -allow-externally-defined-symbols \
; RUN:   | %if --need=target_ARM32 --need=allow_dump \
; RUN:   --command FileCheck %s

; Boring tests ensuring float arguments are allocated "correctly." Unfortunately
; this test cannot verify whether the right arguments are being allocated to the
; right register.
declare void @float1(float %p0)
declare void @float2(float %p0, float %p1)
declare void @float3(float %p0, float %p1, float %p2)
declare void @float4(float %p0, float %p1, float %p2, float %p3)
declare void @float5(float %p0, float %p1, float %p2, float %p3, float %p4)
declare void @float6(float %p0, float %p1, float %p2, float %p3, float %p4,
                     float %p5)
declare void @float7(float %p0, float %p1, float %p2, float %p3, float %p4,
                     float %p5, float %p6)
declare void @float8(float %p0, float %p1, float %p2, float %p3, float %p4,
                     float %p5, float %p6, float %p7)
declare void @float9(float %p0, float %p1, float %p2, float %p3, float %p4,
                     float %p5, float %p6, float %p7, float %p8)
declare void @float10(float %p0, float %p1, float %p2, float %p3, float %p4,
                      float %p5, float %p6, float %p7, float %p8, float %p9)
declare void @float11(float %p0, float %p1, float %p2, float %p3, float %p4,
                      float %p5, float %p6, float %p7, float %p8, float %p9,
                      float %p10)
declare void @float12(float %p0, float %p1, float %p2, float %p3, float %p4,
                      float %p5, float %p6, float %p7, float %p8, float %p9,
                      float %p10, float %p11)
declare void @float13(float %p0, float %p1, float %p2, float %p3, float %p4,
                      float %p5, float %p6, float %p7, float %p8, float %p9,
                      float %p10, float %p11, float %p12)
declare void @float14(float %p0, float %p1, float %p2, float %p3, float %p4,
                      float %p5, float %p6, float %p7, float %p8, float %p9,
                      float %p10, float %p11, float %p12, float %p13)
declare void @float15(float %p0, float %p1, float %p2, float %p3, float %p4,
                      float %p5, float %p6, float %p7, float %p8, float %p9,
                      float %p10, float %p11, float %p12, float %p13,
                      float %p14)
declare void @float16(float %p0, float %p1, float %p2, float %p3, float %p4,
                      float %p5, float %p6, float %p7, float %p8, float %p9,
                      float %p10, float %p11, float %p12, float %p13,
                      float %p14, float %p15)
declare void @float17(float %p0, float %p1, float %p2, float %p3, float %p4,
                      float %p5, float %p6, float %p7, float %p8, float %p9,
                      float %p10, float %p11, float %p12, float %p13,
                      float %p14, float %p15, float %p16)
declare void @float18(float %p0, float %p1, float %p2, float %p3, float %p4,
                      float %p5, float %p6, float %p7, float %p8, float %p9,
                      float %p10, float %p11, float %p12, float %p13,
                      float %p14, float %p15, float %p16, float %p17)
define internal void @floatHarness() nounwind {
; CHECK-LABEL: floatHarness
  call void @float1(float 1.0)
; CHECK-DAG: vmov.f32 s0
; CHECK-DAG: movw [[CALL:r[0-9]]], {{.+}} float1
; CHECK-DAG: movt [[CALL]]
; CHECK:     blx [[CALL]]
  call void @float2(float 1.0, float 2.0)
; CHECK-DAG: vmov.f32 s0
; CHECK-DAG: vmov.f32 s1
; CHECK-DAG: movw [[CALL:r[0-9]]], {{.+}} float2
; CHECK-DAG: movt [[CALL]]
; CHECK:     blx [[CALL]]
  call void @float3(float 1.0, float 2.0, float 3.0)
; CHECK-DAG: vmov.f32 s0
; CHECK-DAG: vmov.f32 s1
; CHECK-DAG: vmov.f32 s2
; CHECK-DAG: movw [[CALL:r[0-9]]], {{.+}} float3
; CHECK-DAG: movt [[CALL]]
; CHECK:     blx [[CALL]]
  call void @float4(float 1.0, float 2.0, float 3.0, float 4.0)
; CHECK-DAG: vmov.f32 s0
; CHECK-DAG: vmov.f32 s1
; CHECK-DAG: vmov.f32 s2
; CHECK-DAG: vmov.f32 s3
; CHECK-DAG: movw [[CALL:r[0-9]]], {{.+}} float4
; CHECK-DAG: movt [[CALL]]
; CHECK:     blx [[CALL]]
  call void @float5(float 1.0, float 2.0, float 3.0, float 4.0, float 5.0)
; CHECK-DAG: vmov.f32 s0
; CHECK-DAG: vmov.f32 s1
; CHECK-DAG: vmov.f32 s2
; CHECK-DAG: vmov.f32 s3
; CHECK-DAG: vmov.f32 s4
; CHECK-DAG: movw [[CALL:r[0-9]]], {{.+}} float5
; CHECK-DAG: movt [[CALL]]
; CHECK:     blx [[CALL]]
  call void @float6(float 1.0, float 2.0, float 3.0, float 4.0, float 5.0,
                    float 6.0)
; CHECK-DAG: vmov.f32 s0
; CHECK-DAG: vmov.f32 s1
; CHECK-DAG: vmov.f32 s2
; CHECK-DAG: vmov.f32 s3
; CHECK-DAG: vmov.f32 s4
; CHECK-DAG: vmov.f32 s5
; CHECK-DAG: movw [[CALL:r[0-9]]], {{.+}} float6
; CHECK-DAG: movt [[CALL]]
; CHECK:     blx [[CALL]]
  call void @float7(float 1.0, float 2.0, float 3.0, float 4.0, float 5.0,
                    float 6.0, float 7.0)
; CHECK-DAG: vmov.f32 s0
; CHECK-DAG: vmov.f32 s1
; CHECK-DAG: vmov.f32 s2
; CHECK-DAG: vmov.f32 s3
; CHECK-DAG: vmov.f32 s4
; CHECK-DAG: vmov.f32 s5
; CHECK-DAG: vmov.f32 s6
; CHECK-DAG: movw [[CALL:r[0-9]]], {{.+}} float7
; CHECK-DAG: movt [[CALL]]
; CHECK:     blx [[CALL]]
  call void @float8(float 1.0, float 2.0, float 3.0, float 4.0, float 5.0,
                    float 6.0, float 7.0, float 8.0)
; CHECK-DAG: vmov.f32 s0
; CHECK-DAG: vmov.f32 s1
; CHECK-DAG: vmov.f32 s2
; CHECK-DAG: vmov.f32 s3
; CHECK-DAG: vmov.f32 s4
; CHECK-DAG: vmov.f32 s5
; CHECK-DAG: vmov.f32 s6
; CHECK-DAG: vmov.f32 s7
; CHECK-DAG: movw [[CALL:r[0-9]]], {{.+}} float8
; CHECK-DAG: movt [[CALL]]
; CHECK:     blx [[CALL]]
  call void @float9(float 1.0, float 2.0, float 3.0, float 4.0, float 5.0,
                    float 6.0, float 7.0, float 8.0, float 9.0)
; CHECK-DAG: vmov.f32 s0
; CHECK-DAG: vmov.f32 s1
; CHECK-DAG: vmov.f32 s2
; CHECK-DAG: vmov.f32 s3
; CHECK-DAG: vmov.f32 s4
; CHECK-DAG: vmov.f32 s5
; CHECK-DAG: vmov.f32 s6
; CHECK-DAG: vmov.f32 s7
; CHECK-DAG: vmov.f32 s8
; CHECK-DAG: movw [[CALL:r[0-9]]], {{.+}} float9
; CHECK-DAG: movt [[CALL]]
; CHECK:     blx [[CALL]]
  call void @float10(float 1.0, float 2.0, float 3.0, float 4.0, float 5.0,
                    float 6.0, float 7.0, float 8.0, float 9.0, float 10.0)
; CHECK-DAG: vmov.f32 s0
; CHECK-DAG: vmov.f32 s1
; CHECK-DAG: vmov.f32 s2
; CHECK-DAG: vmov.f32 s3
; CHECK-DAG: vmov.f32 s4
; CHECK-DAG: vmov.f32 s5
; CHECK-DAG: vmov.f32 s6
; CHECK-DAG: vmov.f32 s7
; CHECK-DAG: vmov.f32 s8
; CHECK-DAG: vmov.f32 s9
; CHECK-DAG: movw [[CALL:r[0-9]]], {{.+}} float10
; CHECK-DAG: movt [[CALL]]
; CHECK:     blx [[CALL]]
  call void @float11(float 1.0, float 2.0, float 3.0, float 4.0, float 5.0,
                    float 6.0, float 7.0, float 8.0, float 9.0, float 10.0,
                    float 11.0)
; CHECK-DAG: vmov.f32 s0
; CHECK-DAG: vmov.f32 s1
; CHECK-DAG: vmov.f32 s2
; CHECK-DAG: vmov.f32 s3
; CHECK-DAG: vmov.f32 s4
; CHECK-DAG: vmov.f32 s5
; CHECK-DAG: vmov.f32 s6
; CHECK-DAG: vmov.f32 s7
; CHECK-DAG: vmov.f32 s8
; CHECK-DAG: vmov.f32 s9
; CHECK-DAG: vmov.f32 s10
; CHECK-DAG: movw [[CALL:r[0-9]]], {{.+}} float11
; CHECK-DAG: movt [[CALL]]
; CHECK:     blx [[CALL]]
  call void @float12(float 1.0, float 2.0, float 3.0, float 4.0, float 5.0,
                    float 6.0, float 7.0, float 8.0, float 9.0, float 10.0,
                    float 11.0, float 12.0)
; CHECK-DAG: vmov.f32 s0
; CHECK-DAG: vmov.f32 s1
; CHECK-DAG: vmov.f32 s2
; CHECK-DAG: vmov.f32 s3
; CHECK-DAG: vmov.f32 s4
; CHECK-DAG: vmov.f32 s5
; CHECK-DAG: vmov.f32 s6
; CHECK-DAG: vmov.f32 s7
; CHECK-DAG: vmov.f32 s8
; CHECK-DAG: vmov.f32 s9
; CHECK-DAG: vmov.f32 s10
; CHECK-DAG: vmov.f32 s11
; CHECK-DAG: movw [[CALL:r[0-9]]], {{.+}} float12
; CHECK-DAG: movt [[CALL]]
; CHECK:     blx [[CALL]]
  call void @float13(float 1.0, float 2.0, float 3.0, float 4.0, float 5.0,
                    float 6.0, float 7.0, float 8.0, float 9.0, float 10.0,
                    float 11.0, float 12.0, float 13.0)
; CHECK-DAG: vmov.f32 s0
; CHECK-DAG: vmov.f32 s1
; CHECK-DAG: vmov.f32 s2
; CHECK-DAG: vmov.f32 s3
; CHECK-DAG: vmov.f32 s4
; CHECK-DAG: vmov.f32 s5
; CHECK-DAG: vmov.f32 s6
; CHECK-DAG: vmov.f32 s7
; CHECK-DAG: vmov.f32 s8
; CHECK-DAG: vmov.f32 s9
; CHECK-DAG: vmov.f32 s10
; CHECK-DAG: vmov.f32 s11
; CHECK-DAG: vmov.f32 s12
; CHECK-DAG: movw [[CALL:r[0-9]]], {{.+}} float13
; CHECK-DAG: movt [[CALL]]
; CHECK:     blx [[CALL]]
  call void @float14(float 1.0, float 2.0, float 3.0, float 4.0, float 5.0,
                    float 6.0, float 7.0, float 8.0, float 9.0, float 10.0,
                    float 11.0, float 12.0, float 13.0, float 14.0)
; CHECK-DAG: vmov.f32 s0
; CHECK-DAG: vmov.f32 s1
; CHECK-DAG: vmov.f32 s2
; CHECK-DAG: vmov.f32 s3
; CHECK-DAG: vmov.f32 s4
; CHECK-DAG: vmov.f32 s5
; CHECK-DAG: vmov.f32 s6
; CHECK-DAG: vmov.f32 s7
; CHECK-DAG: vmov.f32 s8
; CHECK-DAG: vmov.f32 s9
; CHECK-DAG: vmov.f32 s10
; CHECK-DAG: vmov.f32 s11
; CHECK-DAG: vmov.f32 s12
; CHECK-DAG: vmov.f32 s13
; CHECK-DAG: movw [[CALL:r[0-9]]], {{.+}} float14
; CHECK-DAG: movt [[CALL]]
; CHECK:     blx [[CALL]]
  call void @float15(float 1.0, float 2.0, float 3.0, float 4.0, float 5.0,
                    float 6.0, float 7.0, float 8.0, float 9.0, float 10.0,
                    float 11.0, float 12.0, float 13.0, float 14.0,
                    float 15.0)
; CHECK-DAG: vmov.f32 s0
; CHECK-DAG: vmov.f32 s1
; CHECK-DAG: vmov.f32 s2
; CHECK-DAG: vmov.f32 s3
; CHECK-DAG: vmov.f32 s4
; CHECK-DAG: vmov.f32 s5
; CHECK-DAG: vmov.f32 s6
; CHECK-DAG: vmov.f32 s7
; CHECK-DAG: vmov.f32 s8
; CHECK-DAG: vmov.f32 s9
; CHECK-DAG: vmov.f32 s10
; CHECK-DAG: vmov.f32 s11
; CHECK-DAG: vmov.f32 s12
; CHECK-DAG: vmov.f32 s13
; CHECK-DAG: vmov.f32 s14
; CHECK-DAG: movw [[CALL:r[0-9]]], {{.+}} float15
; CHECK-DAG: movt [[CALL]]
; CHECK:     blx [[CALL]]
  call void @float16(float 1.0, float 2.0, float 3.0, float 4.0, float 5.0,
                    float 6.0, float 7.0, float 8.0, float 9.0, float 10.0,
                    float 11.0, float 12.0, float 13.0, float 14.0,
                    float 15.0, float 16.0)
; CHECK-DAG: vmov.f32 s0
; CHECK-DAG: vmov.f32 s1
; CHECK-DAG: vmov.f32 s2
; CHECK-DAG: vmov.f32 s3
; CHECK-DAG: vmov.f32 s4
; CHECK-DAG: vmov.f32 s5
; CHECK-DAG: vmov.f32 s6
; CHECK-DAG: vmov.f32 s7
; CHECK-DAG: vmov.f32 s8
; CHECK-DAG: vmov.f32 s9
; CHECK-DAG: vmov.f32 s10
; CHECK-DAG: vmov.f32 s11
; CHECK-DAG: vmov.f32 s12
; CHECK-DAG: vmov.f32 s13
; CHECK-DAG: vmov.f32 s14
; CHECK-DAG: vmov.f32 s15
; CHECK-DAG: movw [[CALL:r[0-9]]], {{.+}} float16
; CHECK-DAG: movt [[CALL]]
; CHECK:     blx [[CALL]]
  call void @float17(float 1.0, float 2.0, float 3.0, float 4.0, float 5.0,
                    float 6.0, float 7.0, float 8.0, float 9.0, float 10.0,
                    float 11.0, float 12.0, float 13.0, float 14.0,
                    float 15.0, float 16.0, float 17.0)
; CHECK-DAG: vmov.f32 s0
; CHECK-DAG: vmov.f32 s1
; CHECK-DAG: vmov.f32 s2
; CHECK-DAG: vmov.f32 s3
; CHECK-DAG: vmov.f32 s4
; CHECK-DAG: vmov.f32 s5
; CHECK-DAG: vmov.f32 s6
; CHECK-DAG: vmov.f32 s7
; CHECK-DAG: vmov.f32 s8
; CHECK-DAG: vmov.f32 s9
; CHECK-DAG: vmov.f32 s10
; CHECK-DAG: vmov.f32 s11
; CHECK-DAG: vmov.f32 s12
; CHECK-DAG: vmov.f32 s13
; CHECK-DAG: vmov.f32 s14
; CHECK-DAG: vmov.f32 s15
; CHECK-DAG: vstr s{{.*}}, [sp]
; CHECK-DAG: movw [[CALL:r[0-9]]], {{.+}} float17
; CHECK-DAG: movt [[CALL]]
; CHECK:     blx [[CALL]]
  call void @float18(float 1.0, float 2.0, float 3.0, float 4.0, float 5.0,
                    float 6.0, float 7.0, float 8.0, float 9.0, float 10.0,
                    float 11.0, float 12.0, float 13.0, float 14.0,
                    float 15.0, float 16.0, float 17.0, float 18.0)
; CHECK-DAG: vmov.f32 s0
; CHECK-DAG: vmov.f32 s1
; CHECK-DAG: vmov.f32 s2
; CHECK-DAG: vmov.f32 s3
; CHECK-DAG: vmov.f32 s4
; CHECK-DAG: vmov.f32 s5
; CHECK-DAG: vmov.f32 s6
; CHECK-DAG: vmov.f32 s7
; CHECK-DAG: vmov.f32 s8
; CHECK-DAG: vmov.f32 s9
; CHECK-DAG: vmov.f32 s10
; CHECK-DAG: vmov.f32 s11
; CHECK-DAG: vmov.f32 s12
; CHECK-DAG: vmov.f32 s13
; CHECK-DAG: vmov.f32 s14
; CHECK-DAG: vmov.f32 s15
; CHECK-DAG: vstr s{{.*}}, [sp]
; CHECK-DAG: vstr s{{.*}}, [sp, #4]
; CHECK-DAG: movw [[CALL:r[0-9]]], {{.+}} float18
; CHECK-DAG: movt [[CALL]]
; CHECK:     blx [[CALL]]
  ret void
}

declare void @double1(double %p0)
declare void @double2(double %p0, double %p1)
declare void @double3(double %p0, double %p1, double %p2)
declare void @double4(double %p0, double %p1, double %p2, double %p3)
declare void @double5(double %p0, double %p1, double %p2, double %p3,
                      double %p4)
declare void @double6(double %p0, double %p1, double %p2, double %p3,
                      double %p4, double %p5)
declare void @double7(double %p0, double %p1, double %p2, double %p3,
                      double %p4, double %p5, double %p6)
declare void @double8(double %p0, double %p1, double %p2, double %p3,
                      double %p4, double %p5, double %p6, double %p7)
declare void @double9(double %p0, double %p1, double %p2, double %p3,
                      double %p4, double %p5, double %p6, double %p7,
                      double %p8)
declare void @double10(double %p0, double %p1, double %p2, double %p3,
                      double %p4, double %p5, double %p6, double %p7,
                      double %p8, double %p9)
define internal void @doubleHarness() nounwind {
; CHECK-LABEL: doubleHarness
  call void @double1(double 1.0)
; CHECK-DAG: vmov.f64 d0
; CHECK-DAG: movw [[CALL:r[0-9]]], {{.+}} double1
; CHECK-DAG: movt [[CALL]]
; CHECK:     blx [[CALL]]
  call void @double2(double 1.0, double 2.0)
; CHECK-DAG: vmov.f64 d0
; CHECK-DAG: vmov.f64 d1
; CHECK-DAG: movw [[CALL:r[0-9]]], {{.+}} double2
; CHECK-DAG: movt [[CALL]]
; CHECK:     blx [[CALL]]
  call void @double3(double 1.0, double 2.0, double 3.0)
; CHECK-DAG: vmov.f64 d0
; CHECK-DAG: vmov.f64 d1
; CHECK-DAG: vmov.f64 d2
; CHECK-DAG: movw [[CALL:r[0-9]]], {{.+}} double3
; CHECK-DAG: movt [[CALL]]
; CHECK:     blx [[CALL]]
  call void @double4(double 1.0, double 2.0, double 3.0, double 4.0)
; CHECK-DAG: vmov.f64 d0
; CHECK-DAG: vmov.f64 d1
; CHECK-DAG: vmov.f64 d2
; CHECK-DAG: vmov.f64 d3
; CHECK-DAG: movw [[CALL:r[0-9]]], {{.+}} double4
; CHECK-DAG: movt [[CALL]]
; CHECK:     blx [[CALL]]
  call void @double5(double 1.0, double 2.0, double 3.0, double 4.0,
                     double 5.0)
; CHECK-DAG: vmov.f64 d0
; CHECK-DAG: vmov.f64 d1
; CHECK-DAG: vmov.f64 d2
; CHECK-DAG: vmov.f64 d3
; CHECK-DAG: vmov.f64 d4
; CHECK-DAG: movw [[CALL:r[0-9]]], {{.+}} double5
; CHECK-DAG: movt [[CALL]]
; CHECK:     blx [[CALL]]
  call void @double6(double 1.0, double 2.0, double 3.0, double 4.0,
                     double 5.0, double 6.0)
; CHECK-DAG: vmov.f64 d0
; CHECK-DAG: vmov.f64 d1
; CHECK-DAG: vmov.f64 d2
; CHECK-DAG: vmov.f64 d3
; CHECK-DAG: vmov.f64 d4
; CHECK-DAG: vmov.f64 d5
; CHECK-DAG: movw [[CALL:r[0-9]]], {{.+}} double6
; CHECK-DAG: movt [[CALL]]
; CHECK:     blx [[CALL]]
  call void @double7(double 1.0, double 2.0, double 3.0, double 4.0,
                     double 5.0, double 6.0, double 7.0)
; CHECK-DAG: vmov.f64 d0
; CHECK-DAG: vmov.f64 d1
; CHECK-DAG: vmov.f64 d2
; CHECK-DAG: vmov.f64 d3
; CHECK-DAG: vmov.f64 d4
; CHECK-DAG: vmov.f64 d5
; CHECK-DAG: vmov.f64 d6
; CHECK-DAG: movw [[CALL:r[0-9]]], {{.+}} double7
; CHECK-DAG: movt [[CALL]]
; CHECK:     blx [[CALL]]
  call void @double8(double 1.0, double 2.0, double 3.0, double 4.0,
                     double 5.0, double 6.0, double 7.0, double 8.0)
; CHECK-DAG: vmov.f64 d0
; CHECK-DAG: vmov.f64 d1
; CHECK-DAG: vmov.f64 d2
; CHECK-DAG: vmov.f64 d3
; CHECK-DAG: vmov.f64 d4
; CHECK-DAG: vmov.f64 d5
; CHECK-DAG: vmov.f64 d6
; CHECK-DAG: vmov.f64 d7
; CHECK-DAG: movw [[CALL:r[0-9]]], {{.+}} double8
; CHECK-DAG: movt [[CALL]]
; CHECK:     blx [[CALL]]
  call void @double9(double 1.0, double 2.0, double 3.0, double 4.0,
                     double 5.0, double 6.0, double 7.0, double 8.0,
                     double 9.0)
; CHECK-DAG: vmov.f64 d0
; CHECK-DAG: vmov.f64 d1
; CHECK-DAG: vmov.f64 d2
; CHECK-DAG: vmov.f64 d3
; CHECK-DAG: vmov.f64 d4
; CHECK-DAG: vmov.f64 d5
; CHECK-DAG: vmov.f64 d6
; CHECK-DAG: vmov.f64 d7
; CHECK-DAG: movw [[CALL:r[0-9]]], {{.+}} double9
; CHECK-DAG: movt [[CALL]]
; CHECK-DAG: vstr d{{.*}}, [sp]
; CHECK:     blx [[CALL]]
  call void @double10(double 1.0, double 2.0, double 3.0, double 4.0,
                     double 5.0, double 6.0, double 7.0, double 8.0,
                     double 9.0, double 10.0)
; CHECK-DAG: vmov.f64 d0
; CHECK-DAG: vmov.f64 d1
; CHECK-DAG: vmov.f64 d2
; CHECK-DAG: vmov.f64 d3
; CHECK-DAG: vmov.f64 d4
; CHECK-DAG: vmov.f64 d5
; CHECK-DAG: vmov.f64 d6
; CHECK-DAG: vmov.f64 d7
; CHECK-DAG: vstr d{{.*}}, [sp]
; CHECK-DAG: vstr d{{.*}}, [sp, #8]
; CHECK-DAG: movw [[CALL:r[0-9]]], {{.+}} double10
; CHECK-DAG: movt [[CALL]]
; CHECK:     blx [[CALL]]

  ret void
}

declare void @testFDF(float %p0, double %p1, float %p2)
declare void @testFDDF(float %p0, double %p1, double %p2, float %p3)
declare void @testFDDDF(float %p0, double %p1, double %p2, double %p3,
                        float %p4)
declare void @testFDDDDF(float %p0, double %p1, double %p2, double %p3,
                         double %p4, float %p5)
declare void @testFDDDDDF(float %p0, double %p1, double %p2, double %p3,
                          double %p4, double %p5, float %p6)
declare void @testFDDDDDDF(float %p0, double %p1, double %p2, double %p3,
                           double %p4, double %p5, double %p6, float %p7)
declare void @testFDDDDDDDF(float %p0, double %p1, double %p2, double %p3,
                            double %p4, double %p5, double %p6, double %p7,
                            float %p8)
declare void @testFDDDDDDDFD(float %p0, double %p1, double %p2, double %p3,
                             double %p4, double %p5, double %p6, double %p7,
                             float %p8, double %p9)
declare void @testFDDDDDDDDF(float %p0, double %p1, double %p2, double %p3,
                             double %p4, double %p5, double %p6, double %p7,
                             double %p8, float %p9)
declare void @testFDDDDDDDDDF(float %p0, double %p1, double %p2, double %p3,
                              double %p4, double %p5, double %p6, double %p7,
                              double %p8, double %p9, float %p10)
declare void @testFDDDDDDDDFD(float %p0, double %p1, double %p2, double %p3,
                              double %p4, double %p5, double %p6, double %p7,
                              double %p8, float %p9, double %p10)
declare void @testFDDDDDDDDFDF(float %p0, double %p1, double %p2, double %p3,
                               double %p4, double %p5, double %p6, double %p7,
                               double %p8, float %p9, double %p10, float %p11)
define internal void @packsFloats() nounwind {
; CHECK-LABEL: packsFloats
  call void @testFDF(float 1.0, double 2.0, float 3.0)
; CHECK-DAG: vmov.f32 s0
; CHECK-DAG: vmov.f64 d1
; CHECK-DAG: vmov.f32 s1
; CHECK-DAG: movw [[CALL:r[0-9]]], {{.+}} testFDF
; CHECK-DAG: movt [[CALL]]
; CHECK:     blx [[CALL]]
  call void @testFDDF(float 1.0, double 2.0, double 3.0, float 4.0)
; CHECK-DAG: vmov.f32 s0
; CHECK-DAG: vmov.f64 d1
; CHECK-DAG: vmov.f64 d2
; CHECK-DAG: vmov.f32 s1
; CHECK-DAG: movw [[CALL:r[0-9]]], {{.+}} testFDDF
; CHECK-DAG: movt [[CALL]]
; CHECK:     blx [[CALL]]
  call void @testFDDDF(float 1.0, double 2.0, double 3.0, double 4.0,
                       float 5.0)
; CHECK-DAG: vmov.f32 s0
; CHECK-DAG: vmov.f64 d1
; CHECK-DAG: vmov.f64 d2
; CHECK-DAG: vmov.f64 d3
; CHECK-DAG: vmov.f32 s1
; CHECK-DAG: movw [[CALL:r[0-9]]], {{.+}} testFDDDF
; CHECK-DAG: movt [[CALL]]
; CHECK:     blx [[CALL]]
  call void @testFDDDDF(float 1.0, double 2.0, double 3.0, double 4.0,
                        double 5.0, float 6.0)
; CHECK-DAG: vmov.f32 s0
; CHECK-DAG: vmov.f64 d1
; CHECK-DAG: vmov.f64 d2
; CHECK-DAG: vmov.f64 d3
; CHECK-DAG: vmov.f64 d4
; CHECK-DAG: vmov.f32 s1
; CHECK-DAG: movw [[CALL:r[0-9]]], {{.+}} testFDDDDF
; CHECK-DAG: movt [[CALL]]
; CHECK:     blx [[CALL]]
  call void @testFDDDDDF(float 1.0, double 2.0, double 3.0, double 4.0,
                         double 5.0, double 6.0, float 7.0)
; CHECK-DAG: vmov.f32 s0
; CHECK-DAG: vmov.f64 d1
; CHECK-DAG: vmov.f64 d2
; CHECK-DAG: vmov.f64 d3
; CHECK-DAG: vmov.f64 d4
; CHECK-DAG: vmov.f64 d5
; CHECK-DAG: vmov.f32 s1
; CHECK-DAG: movw [[CALL:r[0-9]]], {{.+}} testFDDDDDF
; CHECK-DAG: movt [[CALL]]
; CHECK:     blx [[CALL]]
  call void @testFDDDDDDF(float 1.0, double 2.0, double 3.0, double 4.0,
                          double 5.0, double 6.0, double 7.0, float 8.0)
; CHECK-DAG: vmov.f32 s0
; CHECK-DAG: vmov.f64 d1
; CHECK-DAG: vmov.f64 d2
; CHECK-DAG: vmov.f64 d3
; CHECK-DAG: vmov.f64 d4
; CHECK-DAG: vmov.f64 d5
; CHECK-DAG: vmov.f64 d6
; CHECK-DAG: vmov.f32 s1
; CHECK-DAG: movw [[CALL:r[0-9]]], {{.+}} testFDDDDDDF
; CHECK-DAG: movt [[CALL]]
; CHECK:     blx [[CALL]]
  call void @testFDDDDDDDF(float 1.0, double 2.0, double 3.0, double 4.0,
                           double 5.0, double 6.0, double 7.0, double 8.0,
                           float 9.0)
; CHECK-DAG: vmov.f32 s0
; CHECK-DAG: vmov.f64 d1
; CHECK-DAG: vmov.f64 d2
; CHECK-DAG: vmov.f64 d3
; CHECK-DAG: vmov.f64 d4
; CHECK-DAG: vmov.f64 d5
; CHECK-DAG: vmov.f64 d6
; CHECK-DAG: vmov.f64 d7
; CHECK-DAG: vmov.f32 s1
; CHECK-DAG: movw [[CALL:r[0-9]]], {{.+}} testFDDDDDDDF
; CHECK-DAG: movt [[CALL]]
; CHECK:     blx [[CALL]]
  call void @testFDDDDDDDFD(float 1.0, double 2.0, double 3.0, double 4.0,
                            double 5.0, double 6.0, double 7.0, double 8.0,
                            float 9.0, double 10.0)
; CHECK-DAG: vmov.f32 s0
; CHECK-DAG: vmov.f64 d1
; CHECK-DAG: vmov.f64 d2
; CHECK-DAG: vmov.f64 d3
; CHECK-DAG: vmov.f64 d4
; CHECK-DAG: vmov.f64 d5
; CHECK-DAG: vmov.f64 d6
; CHECK-DAG: vmov.f64 d7
; CHECK-DAG: vstr d{{.*}}, [sp]
; CHECK-DAG: vmov.f32 s1
; CHECK-DAG: movw [[CALL:r[0-9]]], {{.+}} testFDDDDDDDFD
; CHECK-DAG: movt [[CALL]]
; CHECK:     blx [[CALL]]
  call void @testFDDDDDDDDF(float 1.0, double 2.0, double 3.0, double 4.0,
                            double 5.0, double 6.0, double 7.0, double 8.0,
                            double 9.0, float 10.0)
; CHECK-DAG: vmov.f32 s0
; CHECK-DAG: vmov.f64 d1
; CHECK-DAG: vmov.f64 d2
; CHECK-DAG: vmov.f64 d3
; CHECK-DAG: vmov.f64 d4
; CHECK-DAG: vmov.f64 d5
; CHECK-DAG: vmov.f64 d6
; CHECK-DAG: vmov.f64 d7
; CHECK-DAG: vstr d{{.*}}, [sp]
; CHECK-DAG: vstr s{{.*}}, [sp, #8]
; CHECK-DAG: movw [[CALL:r[0-9]]], {{.+}} testFDDDDDDDDF
; CHECK-DAG: movt [[CALL]]
; CHECK:     blx [[CALL]]
  call void @testFDDDDDDDDDF(float 1.0, double 2.0, double 3.0, double 4.0,
                             double 5.0, double 6.0, double 7.0, double 8.0,
                             double 9.0, double 10.0, float 11.0)
; CHECK-DAG: vmov.f32 s0
; CHECK-DAG: vmov.f64 d1
; CHECK-DAG: vmov.f64 d2
; CHECK-DAG: vmov.f64 d3
; CHECK-DAG: vmov.f64 d4
; CHECK-DAG: vmov.f64 d5
; CHECK-DAG: vmov.f64 d6
; CHECK-DAG: vmov.f64 d7
; CHECK-DAG: vstr d{{.*}}, [sp]
; CHECK-DAG: vstr d{{.*}}, [sp, #8]
; CHECK-DAG: vstr s{{.*}}, [sp, #16]
; CHECK-DAG: movw [[CALL:r[0-9]]], {{.+}} testFDDDDDDDDDF
; CHECK-DAG: movt [[CALL]]
; CHECK:     blx [[CALL]]
  call void @testFDDDDDDDDFD(float 1.0, double 2.0, double 3.0, double 4.0,
                             double 5.0, double 6.0, double 7.0, double 8.0,
                             double 9.0, float 10.0, double 11.0)
; CHECK-DAG: vmov.f32 s0
; CHECK-DAG: vmov.f64 d1
; CHECK-DAG: vmov.f64 d2
; CHECK-DAG: vmov.f64 d3
; CHECK-DAG: vmov.f64 d4
; CHECK-DAG: vmov.f64 d5
; CHECK-DAG: vmov.f64 d6
; CHECK-DAG: vmov.f64 d7
; CHECK-DAG: vstr d{{.*}}, [sp]
; CHECK-DAG: vstr s{{.*}}, [sp, #8]
; CHECK-DAG: vstr d{{.*}}, [sp, #16]
; CHECK-DAG: movw [[CALL:r[0-9]]], {{.+}} testFDDDDDDDDFD
; CHECK-DAG: movt [[CALL]]
; CHECK:     blx [[CALL]]
  call void @testFDDDDDDDDFDF(float 1.0, double 2.0, double 3.0, double 4.0,
                              double 5.0, double 6.0, double 7.0, double 8.0,
                              double 9.0, float 10.0, double 11.0, float 12.0)
; CHECK-DAG: vmov.f32 s0
; CHECK-DAG: vmov.f64 d1
; CHECK-DAG: vmov.f64 d2
; CHECK-DAG: vmov.f64 d3
; CHECK-DAG: vmov.f64 d4
; CHECK-DAG: vmov.f64 d5
; CHECK-DAG: vmov.f64 d6
; CHECK-DAG: vmov.f64 d7
; CHECK-DAG: vstr d{{.*}}, [sp]
; CHECK-DAG: vstr s{{.*}}, [sp, #8]
; CHECK-DAG: vstr d{{.*}}, [sp, #16]
; CHECK-DAG: vstr s{{.*}}, [sp, #24]
; CHECK-DAG: movw [[CALL:r[0-9]]], {{.+}} testFDDDDDDDDFD
; CHECK-DAG: movt [[CALL]]
; CHECK:     blx [[CALL]]

  ret void
}

; TODO(jpp): add tests for stack alignment.
