; Tests the Subzero "name mangling" when using the "llvm2ice --prefix"
; option.

; RUN: %llvm2ice --verbose none %s | FileCheck %s
; RUN: %llvm2ice --verbose none --prefix Subzero %s | FileCheck --check-prefix=MANGLE %s
; RUN: %llvm2ice --verbose none %s | FileCheck --check-prefix=ERRORS %s
; RUN: %llvm2iceinsts %s | %szdiff %s | FileCheck --check-prefix=DUMP %s
; RUN: %llvm2iceinsts --pnacl %s | %szdiff %s \
; RUN:                           | FileCheck --check-prefix=DUMP %s

define internal void @FuncC(i32 %i) {
entry:
  ret void
}
; FuncC is a C symbol that isn't recognized as a C++ mangled symbol.
; CHECK: FuncC:
; MANGLE: SubzeroFuncC

define internal void @_ZN13TestNamespace4FuncEi(i32 %i) {
entry:
  ret void
}
; This is Func(int) nested inside namespace TestNamespace.
; CHECK: _ZN13TestNamespace4FuncEi:
; MANGLE: _ZN7Subzero13TestNamespace4FuncEi:

define internal void @_ZN13TestNamespace15NestedNamespace4FuncEi(i32 %i) {
entry:
  ret void
}
; This is Func(int) nested inside two namespaces.
; CHECK: _ZN13TestNamespace15NestedNamespace4FuncEi:
; MANGLE: _ZN7Subzero13TestNamespace15NestedNamespace4FuncEi:

define internal void @_Z13FuncCPlusPlusi(i32 %i) {
entry:
  ret void
}
; This is a non-nested, mangled C++ symbol.
; CHECK: _Z13FuncCPlusPlusi:
; MANGLE: _ZN7Subzero13FuncCPlusPlusEi:

define internal void @_ZN12_GLOBAL__N_18FuncAnonEi(i32 %i) {
entry:
  ret void
}
; This is FuncAnon(int) nested inside an anonymous namespace.
; CHECK: _ZN12_GLOBAL__N_18FuncAnonEi:
; MANGLE: _ZN7Subzero12_GLOBAL__N_18FuncAnonEi:

; Now for the illegitimate examples.

; Test for _ZN with no suffix.  Don't crash, prepend Subzero.
define internal void @_ZN(i32 %i) {
entry:
  ret void
}
; MANGLE: Subzero_ZN:

; Test for _Z<len><str> where <len> is smaller than it should be.
define internal void @_Z12FuncCPlusPlusi(i32 %i) {
entry:
  ret void
}
; MANGLE: _ZN7Subzero12FuncCPlusPluEsi:

; Test for _Z<len><str> where <len> is slightly larger than it should be.
define internal void @_Z14FuncCPlusPlusi(i32 %i) {
entry:
  ret void
}
; MANGLE: _ZN7Subzero14FuncCPlusPlusiE:

; Test for _Z<len><str> where <len> is much larger than it should be.
define internal void @_Z114FuncCPlusPlusi(i32 %i) {
entry:
  ret void
}
; MANGLE: Subzero_Z114FuncCPlusPlusi:

; Test for _Z<len><str> where we try to overflow the uint32_t holding <len>.
define internal void @_Z4294967296FuncCPlusPlusi(i32 %i) {
entry:
  ret void
}
; MANGLE: Subzero_Z4294967296FuncCPlusPlusi:

; Test for _Z<len><str> where <len> is 0.
define internal void @_Z0FuncCPlusPlusi(i32 %i) {
entry:
  ret void
}
; MANGLE: _ZN7Subzero0EFuncCPlusPlusi:

; Test for _Z<len><str> where <len> is -1.  LLVM explicitly allows the
; '-' character in identifiers.

define internal void @_Z-1FuncCPlusPlusi(i32 %i) {
entry:
  ret void
}
; MANGLE: Subzero_Z-1FuncCPlusPlusi:

; ERRORS-NOT: ICE translation error
; DUMP-NOT: SZ
