; This tests a simple loop that sums the elements of an input array.
; The O2 check patterns represent the best code currently achieved.

; RUN: %llvm2ice -O2 --verbose none %s \
; RUN:   | llvm-mc -triple=i686-none-nacl -x86-asm-syntax=intel -filetype=obj \
; RUN:   | llvm-objdump -d -symbolize -x86-asm-syntax=intel - | FileCheck %s
; RUN: %llvm2ice -Om1 --verbose none %s \
; RUN:   | llvm-mc -triple=i686-none-nacl -x86-asm-syntax=intel -filetype=obj \
; RUN:   | llvm-objdump -d -symbolize -x86-asm-syntax=intel - \
; RUN:   | FileCheck --check-prefix=OPTM1 %s
; RUN: %llvm2ice --verbose none %s | FileCheck --check-prefix=ERRORS %s
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

; CHECK-LABEL: simple_loop
; CHECK:      mov ecx, dword ptr [esp{{.*}}+{{.*}}{{[0-9]+}}]
; CHECK:      cmp ecx, 0
; CHECK-NEXT: jg {{[0-9]}}
; NaCl bundle padding
; CHECK-NEXT: nop
; CHECK-NEXT: jmp {{[0-9]}}

; TODO: the mov from ebx to esi seems redundant here - so this may need to be
; modified later

; CHECK:      add [[IREG:[a-z]+]], 1
; CHECK-NEXT: mov [[ICMPREG:[a-z]+]], [[IREG]]
; CHECK:      cmp [[ICMPREG]], ecx
; CHECK-NEXT: jl -{{[0-9]}}
;
; There's nothing remarkable under Om1 to test for, since Om1 generates
; such atrocious code (by design).
; OPTM1-LABEL: simple_loop
; OPTM1:      cmp {{.*}}, 0
; OPTM1:      jg
; OPTM1:      ret

; ERRORS-NOT: ICE translation error
; DUMP-NOT: SZ
