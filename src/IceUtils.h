//===- subzero/src/IceUtils.h - Utility functions ---------------*- C++ -*-===//
//
//                        The Subzero Code Generator
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares some utility functions
//
//===----------------------------------------------------------------------===//

#ifndef SUBZERO_SRC_ICEUTILS_H
#define SUBZERO_SRC_ICEUTILS_H

#include <climits>

namespace Ice {

// Similar to bit_cast, but allows copying from types of unrelated
// sizes. This method was introduced to enable the strict aliasing
// optimizations of GCC 4.4. Basically, GCC mindlessly relies on
// obscure details in the C++ standard that make reinterpret_cast
// virtually useless.
template <class D, class S> inline D bit_copy(const S &source) {
  D destination;
  // This use of memcpy is safe: source and destination cannot overlap.
  memcpy(&destination, reinterpret_cast<const void *>(&source),
         sizeof(destination));
  return destination;
}

class Utils {
public:
  // Check whether an N-bit two's-complement representation can hold value.
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

  template <typename T> static inline bool WouldOverflowAdd(T X, T Y) {
    return ((X > 0 && Y > 0 && (X > std::numeric_limits<T>::max() - Y)) ||
            (X < 0 && Y < 0 && (X < std::numeric_limits<T>::min() - Y)));
  }
};

} // end of namespace Ice

#endif // SUBZERO_SRC_ICEUTILS_H
