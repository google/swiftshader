//===- subzero/src/IceBitVector.h - Inline bit vector. ----------*- C++ -*-===//
//
//                        The Subzero Code Generator
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief Defines and implements a bit vector with inline storage. It is a drop
/// in replacement for llvm::SmallBitVector in subzero -- i.e., not all of
/// llvm::SmallBitVector interface is implemented.
///
//===----------------------------------------------------------------------===//

#ifndef SUBZERO_SRC_ICEBITVECTOR_H
#define SUBZERO_SRC_ICEBITVECTOR_H

#include "IceDefs.h"
#include "IceOperand.h"

#include "llvm/Support/MathExtras.h"

#include <algorithm>
#include <climits>
#include <memory>
#include <type_traits>

namespace Ice {
class SmallBitVector {
public:
  using ElementType = uint64_t;
  static constexpr SizeT BitIndexSize = 6; // log2(NumBitsPerPos);
  static constexpr SizeT NumBitsPerPos = sizeof(ElementType) * CHAR_BIT;
  static_assert(1 << BitIndexSize == NumBitsPerPos, "Invalid BitIndexSize.");

  SmallBitVector(const SmallBitVector &BV) { *this = BV; }

  SmallBitVector &operator=(const SmallBitVector &BV) {
    if (&BV != this) {
      resize(BV.size());
      memcpy(Bits, BV.Bits, sizeof(Bits));
    }
    return *this;
  }

  SmallBitVector() { reset(); }

  explicit SmallBitVector(SizeT S) : SmallBitVector() {
    assert(S <= MaxBits);
    resize(S);
  }

  class Reference {
    Reference() = delete;

  public:
    Reference(const Reference &) = default;
    Reference &operator=(const Reference &Rhs) { return *this = (bool)Rhs; }
    Reference &operator=(bool t) {
      if (t) {
        *Data |= _1 << Bit;
      } else {
        *Data &= ~(_1 << Bit);
      }
      return *this;
    }
    operator bool() const { return (*Data & (_1 << Bit)) != 0; }

  private:
    friend class SmallBitVector;
    Reference(ElementType *D, SizeT B) : Data(D), Bit(B) {
      assert(B < NumBitsPerPos);
    }

    ElementType *const Data;
    const SizeT Bit;
  };

  Reference operator[](unsigned Idx) {
    assert(Idx < size());
    return Reference(Bits + (Idx >> BitIndexSize),
                     Idx & ((_1 << BitIndexSize) - 1));
  }

  bool operator[](unsigned Idx) const {
    assert(Idx < size());
    return Bits[Idx >> BitIndexSize] &
           (_1 << (Idx & ((_1 << BitIndexSize) - 1)));
  }

  int find_first() const { return find_first<0>(); }

  int find_next(unsigned Prev) const { return find_next<0>(Prev); }

  bool any() const {
    for (SizeT i = 0; i < BitsElements; ++i) {
      if (Bits[i]) {
        return true;
      }
    }
    return false;
  }

  SizeT size() const { return Size; }

  void resize(SizeT Size) {
    assert(Size <= MaxBits);
    this->Size = Size;
  }

  void reserve(SizeT Size) {
    assert(Size <= MaxBits);
    (void)Size;
  }

  void set(unsigned Idx) { (*this)[Idx] = true; }

  void set() {
    for (SizeT ii = 0; ii < size(); ++ii) {
      (*this)[ii] = true;
    }
  }

  SizeT count() const {
    SizeT Count = 0;
    for (SizeT i = 0; i < BitsElements; ++i) {
      Count += llvm::countPopulation(Bits[i]);
    }
    return Count;
  }

  SmallBitVector operator&(const SmallBitVector &Rhs) const {
    assert(size() == Rhs.size());
    SmallBitVector Ret(std::max(size(), Rhs.size()));
    for (SizeT i = 0; i < BitsElements; ++i) {
      Ret.Bits[i] = Bits[i] & Rhs.Bits[i];
    }
    return Ret;
  }

  SmallBitVector operator~() const {
    SmallBitVector Ret = *this;
    Ret.invert<0>();
    return Ret;
  }

  SmallBitVector &operator|=(const SmallBitVector &Rhs) {
    assert(size() == Rhs.size());
    resize(std::max(size(), Rhs.size()));
    for (SizeT i = 0; i < BitsElements; ++i) {
      Bits[i] |= Rhs.Bits[i];
    }
    return *this;
  }

  SmallBitVector operator|(const SmallBitVector &Rhs) const {
    assert(size() == Rhs.size());
    SmallBitVector Ret(std::max(size(), Rhs.size()));
    for (SizeT i = 0; i < BitsElements; ++i) {
      Ret.Bits[i] = Bits[i] | Rhs.Bits[i];
    }
    return Ret;
  }

  void reset() { memset(Bits, 0, sizeof(Bits)); }

  void reset(const SmallBitVector &Mask) {
    for (const auto V : RegNumBVIter(Mask)) {
      (*this)[unsigned(V)] = false;
    }
  }

private:
  // _1 is the constant 1 of type ElementType.
  static constexpr ElementType _1 = ElementType(1);

  static constexpr SizeT BitsElements = 2;
  ElementType Bits[BitsElements];

  // MaxBits is defined here because it needs Bits to be defined.
  static constexpr SizeT MaxBits = sizeof(Bits) * CHAR_BIT;
  static_assert(sizeof(Bits) == 16, "Bits must be 16 bytes wide.");
  SizeT Size = 0;

  template <SizeT Pos>
  typename std::enable_if<Pos == sizeof(Bits) / sizeof(Bits[0]), int>::type
  find_first() const {
    return -1;
  }

  template <SizeT Pos>
      typename std::enable_if <
      Pos<sizeof(Bits) / sizeof(Bits[0]), int>::type find_first() const {
    if (Bits[Pos] != 0) {
      return NumBitsPerPos * Pos + llvm::countTrailingZeros(Bits[Pos]);
    }
    return find_first<Pos + 1>();
  }

  template <SizeT Pos>
  typename std::enable_if<Pos == sizeof(Bits) / sizeof(Bits[0]), int>::type
  find_next(unsigned) const {
    return -1;
  }

  template <SizeT Pos>
      typename std::enable_if < Pos<sizeof(Bits) / sizeof(Bits[0]), int>::type
                                find_next(unsigned Prev) const {
    if (Prev + 1 < (Pos + 1) * NumBitsPerPos) {
      const ElementType Mask =
          (ElementType(1) << ((Prev + 1) - Pos * NumBitsPerPos)) - 1;
      const ElementType B = Bits[Pos] & ~Mask;
      if (B != 0) {
        return NumBitsPerPos * Pos + llvm::countTrailingZeros(B);
      }
      Prev = (1 + Pos) * NumBitsPerPos - 1;
    }
    return find_next<Pos + 1>(Prev);
  }

  template <SizeT Pos>
  typename std::enable_if<Pos == sizeof(Bits) / sizeof(Bits[0]), void>::type
  invert() {}

  template <SizeT Pos>
      typename std::enable_if <
      Pos<sizeof(Bits) / sizeof(Bits[0]), void>::type invert() {
    if (size() < Pos * NumBitsPerPos) {
      Bits[Pos] = 0;
    } else if ((Pos + 1) * NumBitsPerPos < size()) {
      Bits[Pos] ^= ~ElementType(0);
    } else {
      const ElementType Mask =
          (ElementType(1) << (size() - (Pos * NumBitsPerPos))) - 1;
      Bits[Pos] ^= Mask;
    }
    invert<Pos + 1>();
  }
};

} // end of namespace Ice

#endif // SUBZERO_SRC_ICEBITVECTOR_H
