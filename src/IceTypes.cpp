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

// Show tags match between ICETYPE_TABLE and ICETYPE_PROPS_TABLE.

// Define a temporary set of enum values based on ICETYPE_TABLE
enum {
#define X(tag, size, align, elts, elty, str) _table_tag_##tag,
  ICETYPE_TABLE
#undef X
      _enum_table_tag_Names
};
// Define a temporary set of enum values based on ICETYPE_PROPS_TABLE
enum {
#define X(tag, IsVec, IsInt, IsFloat, IsIntArith, IsLoadStore, CompareResult)  \
  _props_table_tag_##tag,
  ICETYPE_PROPS_TABLE
#undef X
      _enum_props_table_tag_Names
};
// Assert that tags in ICETYPE_TABLE are also in ICETYPE_PROPS_TABLE.
#define X(tag, size, align, elts, elty, str)                                   \
  static_assert(                                                               \
      (unsigned)_table_tag_##tag == (unsigned)_props_table_tag_##tag,          \
      "Inconsistency between ICETYPE_PROPS_TABLE and ICETYPE_TABLE");
ICETYPE_TABLE;
#undef X
// Assert that tags in ICETYPE_PROPS_TABLE is in ICETYPE_TABLE.
#define X(tag, IsVec, IsInt, IsFloat, IsIntArith, IsLoadStore, CompareResult)  \
  static_assert(                                                               \
      (unsigned)_table_tag_##tag == (unsigned)_props_table_tag_##tag,          \
      "Inconsistency between ICETYPE_PROPS_TABLE and ICETYPE_TABLE");
ICETYPE_PROPS_TABLE
#undef X

// Show vector definitions match in ICETYPE_TABLE and
// ICETYPE_PROPS_TABLE.

// Define constants for each element size in ICETYPE_TABLE.
enum {
#define X(tag, size, align, elts, elty, str) _table_elts_##tag = elts,
  ICETYPE_TABLE
#undef X
      _enum_table_elts_Elements = 0
};
// Define constants for boolean flag if vector in ICETYPE_PROPS_TABLE.
enum {
#define X(tag, IsVec, IsInt, IsFloat, IsIntArith, IsLoadStore, CompareResult)  \
  _props_table_IsVec_##tag = IsVec,
  ICETYPE_PROPS_TABLE
#undef X
};
// Verify that the number of vector elements is consistent with IsVec.
#define X(tag, IsVec, IsInt, IsFloat, IsIntArith, IsLoadStore, CompareResult)  \
  static_assert((_table_elts_##tag > 1) == _props_table_IsVec_##tag,           \
                "Inconsistent vector specification in ICETYPE_PROPS_TABLE");
ICETYPE_PROPS_TABLE;
#undef X

struct TypeAttributeFields {
  size_t TypeWidthInBytes;
  size_t TypeAlignInBytes;
  size_t TypeNumElements;
  Type TypeElementType;
  const char *DisplayString;
};

const struct TypeAttributeFields TypeAttributes[] = {
#define X(tag, size, align, elts, elty, str)                                   \
  { size, align, elts, elty, str }                                             \
  ,
  ICETYPE_TABLE
#undef X
};

struct TypePropertyFields {
  bool TypeIsVectorType;
  bool TypeIsIntegerType;
  bool TypeIsScalarIntegerType;
  bool TypeIsVectorIntegerType;
  bool TypeIsIntegerArithmeticType;
  bool TypeIsFloatingType;
  bool TypeIsScalarFloatingType;
  bool TypeIsVectorFloatingType;
  bool TypeIsLoadStoreType;
  Type CompareResultType;
};

const TypePropertyFields TypePropertiesTable[] = {
#define X(tag, IsVec, IsInt, IsFloat, IsIntArith, IsLoadStore, CompareResult)  \
  {                                                                            \
    IsVec, IsInt, IsInt && !IsVec, IsInt && IsVec, IsIntArith, IsFloat,        \
        IsFloat && !IsVec, IsFloat && IsVec, IsLoadStore, CompareResult        \
  }                                                                            \
  ,
  ICETYPE_PROPS_TABLE
#undef X
};

} // end anonymous namespace

size_t typeWidthInBytes(Type Ty) {
  size_t Index = static_cast<size_t>(Ty);
  if (Index < IceType_NUM)
    return TypeAttributes[Index].TypeWidthInBytes;
  llvm_unreachable("Invalid type for typeWidthInBytes()");
  return 0;
}

size_t typeAlignInBytes(Type Ty) {
  size_t Index = static_cast<size_t>(Ty);
  if (Index < IceType_NUM)
    return TypeAttributes[Index].TypeAlignInBytes;
  llvm_unreachable("Invalid type for typeAlignInBytes()");
  return 1;
}

size_t typeNumElements(Type Ty) {
  size_t Index = static_cast<size_t>(Ty);
  if (Index < IceType_NUM)
    return TypeAttributes[Index].TypeNumElements;
  llvm_unreachable("Invalid type for typeNumElements()");
  return 1;
}

Type typeElementType(Type Ty) {
  size_t Index = static_cast<size_t>(Ty);
  if (Index < IceType_NUM)
    return TypeAttributes[Index].TypeElementType;
  llvm_unreachable("Invalid type for typeElementType()");
  return IceType_void;
}

bool isVectorType(Type Ty) {
  size_t Index = static_cast<size_t>(Ty);
  if (Index < IceType_NUM)
    return TypePropertiesTable[Index].TypeIsVectorType;
  llvm_unreachable("Invalid type for isVectorType()");
  return false;
}

bool isIntegerType(Type Ty) {
  size_t Index = static_cast<size_t>(Ty);
  if (Index < IceType_NUM)
    return TypePropertiesTable[Index].TypeIsIntegerType;
  llvm_unreachable("Invalid type for isIntegerType()");
  return false;
}

bool isScalarIntegerType(Type Ty) {
  size_t Index = static_cast<size_t>(Ty);
  if (Index < IceType_NUM)
    return TypePropertiesTable[Index].TypeIsScalarIntegerType;
  llvm_unreachable("Invalid type for isScalIntegerType()");
  return false;
}

bool isVectorIntegerType(Type Ty) {
  size_t Index = static_cast<size_t>(Ty);
  if (Index < IceType_NUM)
    return TypePropertiesTable[Index].TypeIsVectorIntegerType;
  llvm_unreachable("Invalid type for isVectorIntegerType()");
  return false;
}

bool isIntegerArithmeticType(Type Ty) {
  size_t Index = static_cast<size_t>(Ty);
  if (Index < IceType_NUM)
    return TypePropertiesTable[Index].TypeIsIntegerArithmeticType;
  llvm_unreachable("Invalid type for isIntegerArithmeticType()");
  return false;
}

bool isFloatingType(Type Ty) {
  size_t Index = static_cast<size_t>(Ty);
  if (Index < IceType_NUM)
    return TypePropertiesTable[Index].TypeIsFloatingType;
  llvm_unreachable("Invalid type for isFloatingType()");
  return false;
}

bool isScalarFloatingType(Type Ty) {
  size_t Index = static_cast<size_t>(Ty);
  if (Index < IceType_NUM)
    return TypePropertiesTable[Index].TypeIsScalarFloatingType;
  llvm_unreachable("Invalid type for isScalarFloatingType()");
  return false;
}

bool isVectorFloatingType(Type Ty) {
  size_t Index = static_cast<size_t>(Ty);
  if (Index < IceType_NUM)
    return TypePropertiesTable[Index].TypeIsVectorFloatingType;
  llvm_unreachable("Invalid type for isVectorFloatingType()");
  return false;
}

bool isLoadStoreType(Type Ty) {
  size_t Index = static_cast<size_t>(Ty);
  if (Index < IceType_NUM)
    return TypePropertiesTable[Index].TypeIsLoadStoreType;
  llvm_unreachable("Invalid type for isLoadStoreType()");
  return false;
}

Type getCompareResultType(Type Ty) {
  size_t Index = static_cast<size_t>(Ty);
  if (Index < IceType_NUM)
    return TypePropertiesTable[Index].CompareResultType;
  llvm_unreachable("Invalid type for getCompareResultType");
  return IceType_void;
}

SizeT getScalarIntBitWidth(Type Ty) {
  assert(isScalarIntegerType(Ty));
  if (Ty == IceType_i1)
    return 1;
  return typeWidthInBytes(Ty) * CHAR_BIT;
}

// ======================== Dump routines ======================== //

const char *typeString(Type Ty) {
  size_t Index = static_cast<size_t>(Ty);
  if (Index < IceType_NUM)
    return TypeAttributes[Index].DisplayString;
  llvm_unreachable("Invalid type for typeString");
  return "???";
}

void FuncSigType::dump(Ostream &Stream) const {
  Stream << ReturnType << " (";
  bool IsFirst = true;
  for (const Type ArgTy : ArgList) {
    if (IsFirst) {
      IsFirst = false;
    } else {
      Stream << ", ";
    }
    Stream << ArgTy;
  }
  Stream << ")";
}

} // end of namespace Ice
