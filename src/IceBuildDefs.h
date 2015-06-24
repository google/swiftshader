//===- subzero/src/IceBuildDefs.h - Translator build defines ----*- C++ -*-===//
//
//                        The Subzero Code Generator
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines constexpr functions to query various #define values.
//
//===----------------------------------------------------------------------===//

#ifndef SUBZERO_SRC_ICEBUILDDEFS_H
#define SUBZERO_SRC_ICEBUILDDEFS_H

namespace Ice {
namespace BuildDefs {

// The ALLOW_* etc. symbols must be #defined to zero or non-zero.
constexpr bool disableIrGen() { return ALLOW_DISABLE_IR_GEN; }
constexpr bool dump() { return ALLOW_DUMP; }
constexpr bool llvmCl() { return ALLOW_LLVM_CL; }
constexpr bool llvmIr() { return ALLOW_LLVM_IR; }
constexpr bool llvmIrAsInput() { return ALLOW_LLVM_IR_AS_INPUT; }
constexpr bool minimal() { return ALLOW_MINIMAL_BUILD; }
constexpr bool textualBitcode() { return INPUT_IS_TEXTUAL_BITCODE; }

// NDEBUG can be undefined, or defined to something arbitrary.
constexpr bool asserts() {
#ifdef NDEBUG
  return false;
#else  // !NDEBUG
  return true;
#endif // !NDEBUG
}

// ALLOW_EXTRA_VALIDATION can be undefined, or defined to something non-zero.
constexpr bool extraValidation() {
#if ALLOW_EXTRA_VALIDATION
  return true;
#else  // !ALLOW_EXTRA_VALIDATION
  return false;
#endif // !ALLOW_EXTRA_VALIDATION
}

} // end of namespace BuildDefs
} // end of namespace Ice

#endif // SUBZERO_SRC_ICEBUILDDEFS_H
