//===- subzero/src/IceUtils.h - Utility functions ---------------*- C++ -*-===//
//
//                        The Subzero Code Generator
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file declares some utility functions.
///
//===----------------------------------------------------------------------===//

#ifndef SUBZERO_SRC_ICEUTILS_H
#define SUBZERO_SRC_ICEUTILS_H

#include <climits>

namespace Ice {

/// Similar to bit_cast, but allows copying from types of unrelated sizes. This
/// method was introduced to enable the strict aliasing optimizations of GCC
/// 4.4. Basically, GCC mindlessly relies on obscure details in the C++ standard
/// that make reinterpret_cast virtually useless.
template <class D, class S> inline D bit_copy(const S &source) {
  D destination;
  // This use of memcpy is safe: source and destination cannot overlap.
  memcpy(&destination, reinterpret_cast<const void *>(&source),
         sizeof(destination));
  return destination;
}

class Utils {
  Utils() = delete;
  Utils(const Utils &) = delete;
  Utils &operator=(const Utils &) = delete;

public:
  /// Check whether an N-bit two's-complement representation can hold value.
  template <typename T> static inline bool IsInt(int N, T value) {
    assert((0 < N) &&
           (static_cast<unsigned int>(N) < (CHAR_BIT * sizeof(value))));
    T limit = static_cast<T>(1) << (N - 1);
    return (-limit <= value) && (value < limit);
  }

  template <typename T> static inline bool IsUint(int N, T value) {
    assert((0 < N) &&
           (static_cast<unsigned int>(N) < (CHAR_BIT * sizeof(value))));
    T limit = static_cast<T>(1) << N;
    return (0 <= value) && (value < limit);
  }

  /// Check whether the magnitude of value fits in N bits, i.e., whether an
  /// (N+1)-bit sign-magnitude representation can hold value.
  template <typename T> static inline bool IsAbsoluteUint(int N, T Value) {
    assert((0 < N) &&
           (static_cast<unsigned int>(N) < (CHAR_BIT * sizeof(Value))));
    if (Value < 0)
      Value = -Value;
    return IsUint(N, Value);
  }

  /// Return true if the addition X + Y will cause integer overflow for integers
  /// of type T.
  template <typename T> static inline bool WouldOverflowAdd(T X, T Y) {
    return ((X > 0 && Y > 0 && (X > std::numeric_limits<T>::max() - Y)) ||
            (X < 0 && Y < 0 && (X < std::numeric_limits<T>::min() - Y)));
  }

  /// Adds x to y and stores the result in sum. Returns true if the addition
  /// overflowed.
  static inline bool add_overflow(uint32_t x, uint32_t y, uint32_t *sum) {
    static_assert(std::is_same<uint32_t, unsigned>::value, "Must match type");
#if __has_builtin(__builtin_uadd_overflow)
    return __builtin_uadd_overflow(x, y, sum);
#else
    *sum = x + y;
    return WouldOverflowAdd(x, y);
#endif
  }

  /// Return true if X is already aligned by N, where N is a power of 2.
  template <typename T> static inline bool IsAligned(T X, intptr_t N) {
    assert(llvm::isPowerOf2_64(N));
    return (X & (N - 1)) == 0;
  }

  /// Return Value adjusted to the next highest multiple of Alignment.
  static inline uint32_t applyAlignment(uint32_t Value, uint32_t Alignment) {
    assert(llvm::isPowerOf2_32(Alignment));
    return (Value + Alignment - 1) & -Alignment;
  }

  /// Return amount which must be added to adjust Pos to the next highest
  /// multiple of Align.
  static inline uint64_t OffsetToAlignment(uint64_t Pos, uint64_t Align) {
    assert(llvm::isPowerOf2_64(Align));
    uint64_t Mod = Pos & (Align - 1);
    if (Mod == 0)
      return 0;
    return Align - Mod;
  }

  /// Rotate the value bit pattern to the left by shift bits.
  /// Precondition: 0 <= shift < 32
  static inline uint32_t rotateLeft32(uint32_t value, uint32_t shift) {
    if (shift == 0)
      return value;
    return (value << shift) | (value >> (32 - shift));
  }

  /// Rotate the value bit pattern to the right by shift bits.
  static inline uint32_t rotateRight32(uint32_t value, uint32_t shift) {
    if (shift == 0)
      return value;
    return (value >> shift) | (value << (32 - shift));
  }
};

} // end of namespace Ice

#endif // SUBZERO_SRC_ICEUTILS_H
