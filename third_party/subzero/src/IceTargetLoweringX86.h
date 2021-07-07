//===---- subzero/src/IceTargetLoweringX86.h - x86 lowering -*- C++ -*---===//
//
//                        The Subzero Code Generator
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief Declares common functionlity for lowering to the X86 architecture
/// (32-bit and 64-bit).
///
//===----------------------------------------------------------------------===//

#include <inttypes.h>

namespace Ice {
namespace X86 {

template <typename T> struct PoolTypeConverter {};

template <> struct PoolTypeConverter<float> {
  using PrimitiveIntType = uint32_t;
  using IceType = ConstantFloat;
  static constexpr Type Ty = IceType_f32;
  static constexpr const char *TypeName = "float";
  static constexpr const char *AsmTag = ".long";
  static constexpr const char *PrintfString = "0x%x";
};

template <> struct PoolTypeConverter<double> {
  using PrimitiveIntType = uint64_t;
  using IceType = ConstantDouble;
  static constexpr Type Ty = IceType_f64;
  static constexpr const char *TypeName = "double";
  static constexpr const char *AsmTag = ".quad";
  static constexpr const char *PrintfString = "%" PRIu64;
};

// Add converter for int type constant pooling
template <> struct PoolTypeConverter<uint32_t> {
  using PrimitiveIntType = uint32_t;
  using IceType = ConstantInteger32;
  static constexpr Type Ty = IceType_i32;
  static constexpr const char *TypeName = "i32";
  static constexpr const char *AsmTag = ".long";
  static constexpr const char *PrintfString = "0x%x";
};

// Add converter for int type constant pooling
template <> struct PoolTypeConverter<uint16_t> {
  using PrimitiveIntType = uint32_t;
  using IceType = ConstantInteger32;
  static constexpr Type Ty = IceType_i16;
  static constexpr const char *TypeName = "i16";
  static constexpr const char *AsmTag = ".short";
  static constexpr const char *PrintfString = "0x%x";
};

// Add converter for int type constant pooling
template <> struct PoolTypeConverter<uint8_t> {
  using PrimitiveIntType = uint32_t;
  using IceType = ConstantInteger32;
  static constexpr Type Ty = IceType_i8;
  static constexpr const char *TypeName = "i8";
  static constexpr const char *AsmTag = ".byte";
  static constexpr const char *PrintfString = "0x%x";
};

} // end of namespace X86
} // end of namespace Ice
