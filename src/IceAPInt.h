//===-- subzero/src/IceAPInt.h - Constant integer conversions --*- C++ -*--===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief This file implements a class to represent 64 bit integer constant
/// values, and thier conversion to variable bit sized integers.
///
/// Note: This is a simplified version of llvm/include/llvm/ADT/APInt.h for use
/// with Subzero.
//===----------------------------------------------------------------------===//

#ifndef SUBZERO_SRC_ICEAPINT_H
#define SUBZERO_SRC_ICEAPINT_H

#include "IceDefs.h"

namespace Ice {

class APInt {
public:
  /// Bits in an (internal) value.
  static const SizeT APINT_BITS_PER_WORD = sizeof(uint64_t) * CHAR_BIT;

  APInt(SizeT Bits, uint64_t Val) : BitWidth(Bits), Val(Val) {
    assert(Bits && "bitwidth too small");
    assert(Bits <= APINT_BITS_PER_WORD && "bitwidth too big");
    clearUnusedBits();
  }

  uint32_t getBitWidth() const { return BitWidth; }

  int64_t getSExtValue() const {
    return static_cast<int64_t>(Val << (APINT_BITS_PER_WORD - BitWidth)) >>
           (APINT_BITS_PER_WORD - BitWidth);
  }

  uint64_t getRawData() const { return Val; }

private:
  uint32_t BitWidth; // The number of bits in this APInt.
  uint64_t Val;      // The (64-bit) equivalent integer value.

  /// Clear unused high order bits.
  void clearUnusedBits() {
    // If all bits are used, we want to leave the value alone.
    if (BitWidth == APINT_BITS_PER_WORD)
      return;

    // Mask out the high bits.
    Val &= ~static_cast<uint64_t>(0) >> (APINT_BITS_PER_WORD - BitWidth);
  }
};

} // end of namespace Ice

#endif // SUBZERO_SRC_ICEAPINT_H
