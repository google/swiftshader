//===-- subzero/src/IceAPFloat.h - Constant float conversions --*- C++ -*--===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief This file implements a class to represent Subzero float and double
/// values.
///
/// Note: This is a simplified version of
/// llvm/include/llvm/ADT/APFloat.h for use with Subzero.
//===----------------------------------------------------------------------===//

#ifndef SUBZERO_SRC_ICEAPFLOAT_H
#define SUBZERO_SRC_ICEAPFLOAT_H

#include "IceAPInt.h"

namespace Ice {

template <typename IntType, typename FpType>
inline FpType convertAPIntToFp(const APInt &Int) {
  static_assert(sizeof(IntType) == sizeof(FpType),
                "IntType and FpType should be the same width");
  assert(Int.getBitWidth() == sizeof(IntType) * CHAR_BIT);
  union {
    IntType IntValue;
    FpType FpValue;
  } Converter;
  Converter.IntValue = static_cast<IntType>(Int.getRawData());
  return Converter.FpValue;
}

} // end of namespace Ice

#endif //  SUBZERO_SRC_ICEAPFLOAT_H
