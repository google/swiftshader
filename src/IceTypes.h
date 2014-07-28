//===- subzero/src/IceTypes.h - Primitive ICE types -------------*- C++ -*-===//
//
//                        The Subzero Code Generator
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares a few properties of the primitive types allowed
// in Subzero.  Every Subzero source file is expected to include
// IceTypes.h.
//
//===----------------------------------------------------------------------===//

#ifndef SUBZERO_SRC_ICETYPES_H
#define SUBZERO_SRC_ICETYPES_H

#include "IceTypes.def"

namespace Ice {

enum Type {
#define X(tag, size, align, elts, elty, str) tag,
  ICETYPE_TABLE
#undef X
      IceType_NUM
};

enum TargetArch {
  Target_X8632,
  Target_X8664,
  Target_ARM32,
  Target_ARM64
};

enum OptLevel {
  Opt_m1,
  Opt_0,
  Opt_1,
  Opt_2
};

size_t typeWidthInBytes(Type Ty);
size_t typeAlignInBytes(Type Ty);
size_t typeNumElements(Type Ty);
Type typeElementType(Type Ty);
const char *typeString(Type Ty);

inline bool isVectorType(Type Ty) { return typeNumElements(Ty) > 1; }

template <typename StreamType>
inline StreamType &operator<<(StreamType &Str, const Type &Ty) {
  Str << typeString(Ty);
  return Str;
}

} // end of namespace Ice

#endif // SUBZERO_SRC_ICETYPES_H
