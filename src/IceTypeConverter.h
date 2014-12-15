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

  /// Converts LLVM type LLVMTy to an ICE type. Returns
  /// Ice::IceType_NUM if unable to convert.
  Type convertToIceType(llvm::Type *LLVMTy) const {
    auto Pos = LLVM2IceMap.find(LLVMTy);
    if (Pos == LLVM2IceMap.end())
      return convertToIceTypeOther(LLVMTy);
    return Pos->second;
  }

private:
  // The mapping from LLVM types to corresopnding Ice types.
  std::map<llvm::Type *, Type> LLVM2IceMap;

  // Add LLVM/ICE pair to internal tables.
  void addLLVMType(Type Ty, llvm::Type *LLVMTy);

  // Converts types not in LLVM2IceMap.
  Type convertToIceTypeOther(llvm::Type *LLVMTy) const;
};

} // end of Ice namespace.

#endif // SUBZERO_SRC_ICETYPECONVERTER_H
