; This tries to be a comprehensive test of f32 and f64 compare operations.
; The CHECK lines are only checking for basic instruction patterns
; that should be present regardless of the optimization level, so
; there are no special OPTM1 match lines.

; RUN: %p2i --filetype=obj --disassemble -i %s --args -O2 | FileCheck %s
; RUN: %p2i --filetype=obj --disassemble -i %s --args -Om1 | FileCheck %s

define internal void @fcmpEq(float %a, float %b, double %c, double %d) {
entry:
  %cmp = fcmp oeq float %a, %b
  br i1 %cmp, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  call void @func()
  br label %if.end

if.end:                                           ; preds = %if.then, %entry
  %cmp1 = fcmp oeq double %c, %d
  br i1 %cmp1, label %if.then2, label %if.end3

if.then2:                                         ; preds = %if.end
  call void @func()
  br label %if.end3

if.end3:                                          ; preds = %if.then2, %if.end
  ret void
}
; CHECK-LABEL: fcmpEq
; CHECK: ucomiss
; CHECK: jne
; CHECK-NEXT: jp
; CHECK: call {{.*}} R_{{.*}} func
; CHECK: ucomisd
; CHECK: jne
; CHECK-NEXT: jp
; CHECK: call {{.*}} R_{{.*}} func

declare void @func()

define internal void @fcmpNe(float %a, float %b, double %c, double %d) {
entry:
  %cmp = fcmp une float %a, %b
  br i1 %cmp, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  call void @func()
  br label %if.end

if.end:                                           ; preds = %if.then, %entry
  %cmp1 = fcmp une double %c, %d
  br i1 %cmp1, label %if.then2, label %if.end3

if.then2:                                         ; preds = %if.end
  call void @func()
  br label %if.end3

if.end3:                                          ; preds = %if.then2, %if.end
  ret void
}
; CHECK-LABEL: fcmpNe
; CHECK: ucomiss
; CHECK: jne
; CHECK-NEXT: jp
; CHECK: call {{.*}} R_{{.*}} func
; CHECK: ucomisd
; CHECK: jne
; CHECK-NEXT: jp
; CHECK: call {{.*}} R_{{.*}} func

define internal void @fcmpGt(float %a, float %b, double %c, double %d) {
entry:
  %cmp = fcmp ogt float %a, %b
  br i1 %cmp, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  call void @func()
  br label %if.end

if.end:                                           ; preds = %if.then, %entry
  %cmp1 = fcmp ogt double %c, %d
  br i1 %cmp1, label %if.then2, label %if.end3

if.then2:                                         ; preds = %if.end
  call void @func()
  br label %if.end3

if.end3:                                          ; preds = %if.then2, %if.end
  ret void
}
; CHECK-LABEL: fcmpGt
; CHECK: ucomiss
; CHECK: seta
; CHECK: call {{.*}} R_{{.*}} func
; CHECK: ucomisd
; CHECK: seta
; CHECK: call {{.*}} R_{{.*}} func

define internal void @fcmpGe(float %a, float %b, double %c, double %d) {
entry:
  %cmp = fcmp ult float %a, %b
  br i1 %cmp, label %if.end, label %if.then

if.then:                                          ; preds = %entry
  call void @func()
  br label %if.end

if.end:                                           ; preds = %entry, %if.then
  %cmp1 = fcmp ult double %c, %d
  br i1 %cmp1, label %if.end3, label %if.then2

if.then2:                                         ; preds = %if.end
  call void @func()
  br label %if.end3

if.end3:                                          ; preds = %if.end, %if.then2
  ret void
}
; CHECK-LABEL: fcmpGe
; CHECK: ucomiss
; CHECK: setb
; CHECK: call {{.*}} R_{{.*}} func
; CHECK: ucomisd
; CHECK: setb
; CHECK: call {{.*}} R_{{.*}} func

define internal void @fcmpLt(float %a, float %b, double %c, double %d) {
entry:
  %cmp = fcmp olt float %a, %b
  br i1 %cmp, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  call void @func()
  br label %if.end

if.end:                                           ; preds = %if.then, %entry
  %cmp1 = fcmp olt double %c, %d
  br i1 %cmp1, label %if.then2, label %if.end3

if.then2:                                         ; preds = %if.end
  call void @func()
  br label %if.end3

if.end3:                                          ; preds = %if.then2, %if.end
  ret void
}
; CHECK-LABEL: fcmpLt
; CHECK: ucomiss
; CHECK: seta
; CHECK: call {{.*}} R_{{.*}} func
; CHECK: ucomisd
; CHECK: seta
; CHECK: call {{.*}} R_{{.*}} func

define internal void @fcmpLe(float %a, float %b, double %c, double %d) {
entry:
  %cmp = fcmp ugt float %a, %b
  br i1 %cmp, label %if.end, label %if.then

if.then:                                          ; preds = %entry
  call void @func()
  br label %if.end

if.end:                                           ; preds = %entry, %if.then
  %cmp1 = fcmp ugt double %c, %d
  br i1 %cmp1, label %if.end3, label %if.then2

if.then2:                                         ; preds = %if.end
  call void @func()
  br label %if.end3

if.end3:                                          ; preds = %if.end, %if.then2
  ret void
}
; CHECK-LABEL: fcmpLe
; CHECK: ucomiss
; CHECK: setb
; CHECK: call {{.*}} R_{{.*}} func
; CHECK: ucomisd
; CHECK: setb
; CHECK: call {{.*}} R_{{.*}} func

define internal i32 @fcmpFalseFloat(float %a, float %b) {
entry:
  %cmp = fcmp false float %a, %b
  %cmp.ret_ext = zext i1 %cmp to i32
  ret i32 %cmp.ret_ext
}
; CHECK-LABEL: fcmpFalseFloat
; CHECK: mov {{.*}},0x0

define internal i32 @fcmpFalseDouble(double %a, double %b) {
entry:
  %cmp = fcmp false double %a, %b
  %cmp.ret_ext = zext i1 %cmp to i32
  ret i32 %cmp.ret_ext
}
; CHECK-LABEL: fcmpFalseDouble
; CHECK: mov {{.*}},0x0

define internal i32 @fcmpOeqFloat(float %a, float %b) {
entry:
  %cmp = fcmp oeq float %a, %b
  %cmp.ret_ext = zext i1 %cmp to i32
  ret i32 %cmp.ret_ext
}
; CHECK-LABEL: fcmpOeqFloat
; CHECK: ucomiss
; CHECK: jne
; CHECK: jp

define internal i32 @fcmpOeqDouble(double %a, double %b) {
entry:
  %cmp = fcmp oeq double %a, %b
  %cmp.ret_ext = zext i1 %cmp to i32
  ret i32 %cmp.ret_ext
}
; CHECK-LABEL: fcmpOeqDouble
; CHECK: ucomisd
; CHECK: jne
; CHECK: jp

define internal i32 @fcmpOgtFloat(float %a, float %b) {
entry:
  %cmp = fcmp ogt float %a, %b
  %cmp.ret_ext = zext i1 %cmp to i32
  ret i32 %cmp.ret_ext
}
; CHECK-LABEL: fcmpOgtFloat
; CHECK: ucomiss
; CHECK: seta

define internal i32 @fcmpOgtDouble(double %a, double %b) {
entry:
  %cmp = fcmp ogt double %a, %b
  %cmp.ret_ext = zext i1 %cmp to i32
  ret i32 %cmp.ret_ext
}
; CHECK-LABEL: fcmpOgtDouble
; CHECK: ucomisd
; CHECK: seta

define internal i32 @fcmpOgeFloat(float %a, float %b) {
entry:
  %cmp = fcmp oge float %a, %b
  %cmp.ret_ext = zext i1 %cmp to i32
  ret i32 %cmp.ret_ext
}
; CHECK-LABEL: fcmpOgeFloat
; CHECK: ucomiss
; CHECK: setae

define internal i32 @fcmpOgeDouble(double %a, double %b) {
entry:
  %cmp = fcmp oge double %a, %b
  %cmp.ret_ext = zext i1 %cmp to i32
  ret i32 %cmp.ret_ext
}
; CHECK-LABEL: fcmpOgeDouble
; CHECK: ucomisd
; CHECK: setae

define internal i32 @fcmpOltFloat(float %a, float %b) {
entry:
  %cmp = fcmp olt float %a, %b
  %cmp.ret_ext = zext i1 %cmp to i32
  ret i32 %cmp.ret_ext
}
; CHECK-LABEL: fcmpOltFloat
; CHECK: ucomiss
; CHECK: seta

define internal i32 @fcmpOltDouble(double %a, double %b) {
entry:
  %cmp = fcmp olt double %a, %b
  %cmp.ret_ext = zext i1 %cmp to i32
  ret i32 %cmp.ret_ext
}
; CHECK-LABEL: fcmpOltDouble
; CHECK: ucomisd
; CHECK: seta

define internal i32 @fcmpOleFloat(float %a, float %b) {
entry:
  %cmp = fcmp ole float %a, %b
  %cmp.ret_ext = zext i1 %cmp to i32
  ret i32 %cmp.ret_ext
}
; CHECK-LABEL: fcmpOleFloat
; CHECK: ucomiss
; CHECK: setae

define internal i32 @fcmpOleDouble(double %a, double %b) {
entry:
  %cmp = fcmp ole double %a, %b
  %cmp.ret_ext = zext i1 %cmp to i32
  ret i32 %cmp.ret_ext
}
; CHECK-LABEL: fcmpOleDouble
; CHECK: ucomisd
; CHECK: setae

define internal i32 @fcmpOneFloat(float %a, float %b) {
entry:
  %cmp = fcmp one float %a, %b
  %cmp.ret_ext = zext i1 %cmp to i32
  ret i32 %cmp.ret_ext
}
; CHECK-LABEL: fcmpOneFloat
; CHECK: ucomiss
; CHECK: setne

define internal i32 @fcmpOneDouble(double %a, double %b) {
entry:
  %cmp = fcmp one double %a, %b
  %cmp.ret_ext = zext i1 %cmp to i32
  ret i32 %cmp.ret_ext
}
; CHECK-LABEL: fcmpOneDouble
; CHECK: ucomisd
; CHECK: setne

define internal i32 @fcmpOrdFloat(float %a, float %b) {
entry:
  %cmp = fcmp ord float %a, %b
  %cmp.ret_ext = zext i1 %cmp to i32
  ret i32 %cmp.ret_ext
}
; CHECK-LABEL: fcmpOrdFloat
; CHECK: ucomiss
; CHECK: setnp

define internal i32 @fcmpOrdDouble(double %a, double %b) {
entry:
  %cmp = fcmp ord double %a, %b
  %cmp.ret_ext = zext i1 %cmp to i32
  ret i32 %cmp.ret_ext
}
; CHECK-LABEL: fcmpOrdDouble
; CHECK: ucomisd
; CHECK: setnp

define internal i32 @fcmpUeqFloat(float %a, float %b) {
entry:
  %cmp = fcmp ueq float %a, %b
  %cmp.ret_ext = zext i1 %cmp to i32
  ret i32 %cmp.ret_ext
}
; CHECK-LABEL: fcmpUeqFloat
; CHECK: ucomiss
; CHECK: sete

define internal i32 @fcmpUeqDouble(double %a, double %b) {
entry:
  %cmp = fcmp ueq double %a, %b
  %cmp.ret_ext = zext i1 %cmp to i32
  ret i32 %cmp.ret_ext
}
; CHECK-LABEL: fcmpUeqDouble
; CHECK: ucomisd
; CHECK: sete

define internal i32 @fcmpUgtFloat(float %a, float %b) {
entry:
  %cmp = fcmp ugt float %a, %b
  %cmp.ret_ext = zext i1 %cmp to i32
  ret i32 %cmp.ret_ext
}
; CHECK-LABEL: fcmpUgtFloat
; CHECK: ucomiss
; CHECK: setb

define internal i32 @fcmpUgtDouble(double %a, double %b) {
entry:
  %cmp = fcmp ugt double %a, %b
  %cmp.ret_ext = zext i1 %cmp to i32
  ret i32 %cmp.ret_ext
}
; CHECK-LABEL: fcmpUgtDouble
; CHECK: ucomisd
; CHECK: setb

define internal i32 @fcmpUgeFloat(float %a, float %b) {
entry:
  %cmp = fcmp uge float %a, %b
  %cmp.ret_ext = zext i1 %cmp to i32
  ret i32 %cmp.ret_ext
}
; CHECK-LABEL: fcmpUgeFloat
; CHECK: ucomiss
; CHECK: setbe

define internal i32 @fcmpUgeDouble(double %a, double %b) {
entry:
  %cmp = fcmp uge double %a, %b
  %cmp.ret_ext = zext i1 %cmp to i32
  ret i32 %cmp.ret_ext
}
; CHECK-LABEL: fcmpUgeDouble
; CHECK: ucomisd
; CHECK: setbe

define internal i32 @fcmpUltFloat(float %a, float %b) {
entry:
  %cmp = fcmp ult float %a, %b
  %cmp.ret_ext = zext i1 %cmp to i32
  ret i32 %cmp.ret_ext
}
; CHECK-LABEL: fcmpUltFloat
; CHECK: ucomiss
; CHECK: setb

define internal i32 @fcmpUltDouble(double %a, double %b) {
entry:
  %cmp = fcmp ult double %a, %b
  %cmp.ret_ext = zext i1 %cmp to i32
  ret i32 %cmp.ret_ext
}
; CHECK-LABEL: fcmpUltDouble
; CHECK: ucomisd
; CHECK: setb

define internal i32 @fcmpUleFloat(float %a, float %b) {
entry:
  %cmp = fcmp ule float %a, %b
  %cmp.ret_ext = zext i1 %cmp to i32
  ret i32 %cmp.ret_ext
}
; CHECK-LABEL: fcmpUleFloat
; CHECK: ucomiss
; CHECK: setbe

define internal i32 @fcmpUleDouble(double %a, double %b) {
entry:
  %cmp = fcmp ule double %a, %b
  %cmp.ret_ext = zext i1 %cmp to i32
  ret i32 %cmp.ret_ext
}
; CHECK-LABEL: fcmpUleDouble
; CHECK: ucomisd
; CHECK: setbe

define internal i32 @fcmpUneFloat(float %a, float %b) {
entry:
  %cmp = fcmp une float %a, %b
  %cmp.ret_ext = zext i1 %cmp to i32
  ret i32 %cmp.ret_ext
}
; CHECK-LABEL: fcmpUneFloat
; CHECK: ucomiss
; CHECK: jne
; CHECK: jp

define internal i32 @fcmpUneDouble(double %a, double %b) {
entry:
  %cmp = fcmp une double %a, %b
  %cmp.ret_ext = zext i1 %cmp to i32
  ret i32 %cmp.ret_ext
}
; CHECK-LABEL: fcmpUneDouble
; CHECK: ucomisd
; CHECK: jne
; CHECK: jp

define internal i32 @fcmpUnoFloat(float %a, float %b) {
entry:
  %cmp = fcmp uno float %a, %b
  %cmp.ret_ext = zext i1 %cmp to i32
  ret i32 %cmp.ret_ext
}
; CHECK-LABEL: fcmpUnoFloat
; CHECK: ucomiss
; CHECK: setp

define internal i32 @fcmpUnoDouble(double %a, double %b) {
entry:
  %cmp = fcmp uno double %a, %b
  %cmp.ret_ext = zext i1 %cmp to i32
  ret i32 %cmp.ret_ext
}
; CHECK-LABEL: fcmpUnoDouble
; CHECK: ucomisd
; CHECK: setp

define internal i32 @fcmpTrueFloat(float %a, float %b) {
entry:
  %cmp = fcmp true float %a, %b
  %cmp.ret_ext = zext i1 %cmp to i32
  ret i32 %cmp.ret_ext
}
; CHECK-LABEL: fcmpTrueFloat
; CHECK: mov {{.*}},0x1

define internal i32 @fcmpTrueDouble(double %a, double %b) {
entry:
  %cmp = fcmp true double %a, %b
  %cmp.ret_ext = zext i1 %cmp to i32
  ret i32 %cmp.ret_ext
}
; CHECK-LABEL: fcmpTrueDouble
; CHECK: mov {{.*}},0x1

define internal float @selectFloatVarVar(float %a, float %b) {
entry:
  %cmp = fcmp olt float %a, %b
  %cond = select i1 %cmp, float %a, float %b
  ret float %cond
}
; CHECK-LABEL: selectFloatVarVar
; CHECK: ucomiss
; CHECK: seta
; CHECK: fld

define internal double @selectDoubleVarVar(double %a, double %b) {
entry:
  %cmp = fcmp olt double %a, %b
  %cond = select i1 %cmp, double %a, double %b
  ret double %cond
}
; CHECK-LABEL: selectDoubleVarVar
; CHECK: ucomisd
; CHECK: seta
; CHECK: fld
