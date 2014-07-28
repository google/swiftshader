//===- subzero/src/IceTypes.cpp - Primitive type properties ---------------===//
//
//                        The Subzero Code Generator
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines a few attributes of Subzero primitive types.
//
//===----------------------------------------------------------------------===//

#include "IceDefs.h"
#include "IceTypes.h"

namespace Ice {

namespace {

const struct {
  size_t TypeWidthInBytes;
  size_t TypeAlignInBytes;
  size_t TypeNumElements;
  Type TypeElementType;
  const char *DisplayString;
} TypeAttributes[] = {
#define X(tag, size, align, elts, elty, str)                                   \
  { size, align, elts, elty, str }                                             \
  ,
    ICETYPE_TABLE
#undef X
  };

const size_t TypeAttributesSize =
    sizeof(TypeAttributes) / sizeof(*TypeAttributes);

} // end anonymous namespace

size_t typeWidthInBytes(Type Ty) {
  size_t Width = 0;
  size_t Index = static_cast<size_t>(Ty);
  if (Index < TypeAttributesSize) {
    Width = TypeAttributes[Index].TypeWidthInBytes;
  } else {
    llvm_unreachable("Invalid type for typeWidthInBytes()");
  }
  return Width;
}

size_t typeAlignInBytes(Type Ty) {
  size_t Align = 0;
  size_t Index = static_cast<size_t>(Ty);
  if (Index < TypeAttributesSize) {
    Align = TypeAttributes[Index].TypeAlignInBytes;
  } else {
    llvm_unreachable("Invalid type for typeAlignInBytes()");
  }
  return Align;
}

size_t typeNumElements(Type Ty) {
  size_t NumElements = 0;
  size_t Index = static_cast<size_t>(Ty);
  if (Index < TypeAttributesSize) {
    NumElements = TypeAttributes[Index].TypeNumElements;
  } else {
    llvm_unreachable("Invalid type for typeNumElements()");
  }
  return NumElements;
}

Type typeElementType(Type Ty) {
  Type ElementType = IceType_void;
  size_t Index = static_cast<size_t>(Ty);
  if (Index < TypeAttributesSize) {
    ElementType = TypeAttributes[Index].TypeElementType;
  } else {
    llvm_unreachable("Invalid type for typeElementType()");
  }
  return ElementType;
}

const char *typeString(Type Ty) {
  size_t Index = static_cast<size_t>(Ty);
  if (Index < TypeAttributesSize) {
    return TypeAttributes[Index].DisplayString;
  }
  llvm_unreachable("Invalid type for typeString");
  return "???";
}

} // end of namespace Ice
