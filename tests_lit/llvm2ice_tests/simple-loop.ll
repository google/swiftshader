; RUIN: %llvm2ice -verbose inst %s | FileCheck %s
; RUIN: %llvm2ice --verbose none %s | FileCheck --check-prefix=ERRORS %s
; RUN: %llvm2iceinsts %s | %szdiff %s | FileCheck --check-prefix=DUMP %s
; RUN: %llvm2iceinsts --pnacl %s | %szdiff %s \
; RUN:                           | FileCheck --check-prefix=DUMP %s

define i32 @simple_loop(i32 %a, i32 %n) {
entry:
  %cmp4 = icmp sgt i32 %n, 0
  br i1 %cmp4, label %for.body, label %for.end

for.body:
  %i.06 = phi i32 [ %inc, %for.body ], [ 0, %entry ]
  %sum.05 = phi i32 [ %add, %for.body ], [ 0, %entry ]
  %gep_array = mul i32 %i.06, 4
  %gep = add i32 %a, %gep_array
  %__9 = inttoptr i32 %gep to i32*
  %v0 = load i32* %__9, align 1
  %add = add i32 %v0, %sum.05
  %inc = add i32 %i.06, 1
  %cmp = icmp slt i32 %inc, %n
  br i1 %cmp, label %for.body, label %for.end

for.end:
  %sum.0.lcssa = phi i32 [ 0, %entry ], [ %add, %for.body ]
  ret i32 %sum.0.lcssa
}

; Checks for verbose instruction output

; CHECK:  br i1 %cmp4, label %for.body, label %for.end
; CHECK-NEXT: for.body
; CHECK:  %i.06 = phi i32 [ %inc, %for.body ], [ 0, %entry ]
; CHECK-NEXT:  %sum.05 = phi i32 [ %add, %for.body ], [ 0, %entry ]

; Checks for emitted assembly

; CHECK:      .globl simple_loop

; CHECK:      mov ecx, dword ptr [esp+{{[0-9]+}}]
; CHECK:      cmp ecx, 0
; CHECK-NEXT: jg {{.*}}for.body
; CHECK-NEXT: jmp {{.*}}for.end

; TODO: the mov from ebx to esi seems redundant here - so this may need to be
; modified later

; CHECK:      add [[IREG:[a-z]+]], 1
; CHECK-NEXT: mov [[ICMPREG:[a-z]+]], [[IREG]]
; CHECK:      cmp [[ICMPREG]], ecx
; CHECK-NEXT: jl {{.*}}for.body

; ERRORS-NOT: ICE translation error
; DUMP-NOT: SZ
