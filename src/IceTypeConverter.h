//===- subzero/src/IceTypeConverter.h - Convert ICE/LLVM Types --*- C++ -*-===//
//
//                        The Subzero Code Generator
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines how to convert LLVM types to ICE types, and ICE types
// to LLVM types.
//
//===----------------------------------------------------------------------===//

#ifndef SUBZERO_SRC_ICETYPECONVERTER_H
#define SUBZERO_SRC_ICETYPECONVERTER_H

#include "llvm/IR/DerivedTypes.h"

#include "IceDefs.h"
#include "IceTypes.h"

namespace llvm {
class LLVMContext;
} // end of llvm namespace.

namespace Ice {

/// Converts LLVM types to ICE types, and ICE types to LLVM types.
class TypeConverter {
  TypeConverter(const TypeConverter &) = delete;
  TypeConverter &operator=(const TypeConverter &) = delete;

public:
  /// Context is the context to use to build llvm types.
  TypeConverter(llvm::LLVMContext &Context);

  /// Returns the LLVM type for the corresponding ICE type Ty.
  llvm::Type *convertToLLVMType(Type Ty) const {
    // Note: We use "at" here in case Ty wasn't registered.
    return LLVMTypes.at(Ty);
  }

  /// Converts LLVM type LLVMTy to an ICE type. Returns
  /// Ice::IceType_NUM if unable to convert.
  Type convertToIceType(llvm::Type *LLVMTy) const {
    auto Pos = LLVM2IceMap.find(LLVMTy);
    if (Pos == LLVM2IceMap.end())
      return convertToIceTypeOther(LLVMTy);
    return Pos->second;
  }

  /// Returns ICE model of pointer type.
  Type getIcePointerType() const { return IceType_i32; }

  /// Returns LLVM integer type with specified number of bits. Returns
  /// NULL if not a valid PNaCl integer type.
  llvm::Type *getLLVMIntegerType(unsigned NumBits) const;

  /// Returns the LLVM vector type for Size and Ty arguments. Returns
  /// NULL if not a valid PNaCl vector type.
  llvm::Type *getLLVMVectorType(unsigned Size, Type Ty) const;

private:
  // The list of allowable LLVM types. Indexed by ICE type.
  std::vector<llvm::Type *> LLVMTypes;
  // The inverse mapping of LLVMTypes.
  std::map<llvm::Type *, Type> LLVM2IceMap;

  // Add LLVM/ICE pair to internal tables.
  void AddLLVMType(Type Ty, llvm::Type *LLVMTy);

  // Converts types not in LLVM2IceMap.
  Type convertToIceTypeOther(llvm::Type *LLVMTy) const;
};

} // end of Ice namespace.

#endif // SUBZERO_SRC_ICETYPECONVERTER_H
