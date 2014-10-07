//===- subzero/src/IceTypeConverter.cpp - Convert ICE/LLVM Types ----------===//
//
//                        The Subzero Code Generator
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements how to convert LLVM types to ICE types, and ICE types
// to LLVM types.
//
//===----------------------------------------------------------------------===//

#include "llvm/Support/raw_ostream.h"

#include "IceTypeConverter.h"

namespace Ice {

TypeConverter::TypeConverter(llvm::LLVMContext &Context) {
  addLLVMType(IceType_void, llvm::Type::getVoidTy(Context));
  addLLVMType(IceType_i1, llvm::IntegerType::get(Context, 1));
  addLLVMType(IceType_i8, llvm::IntegerType::get(Context, 8));
  addLLVMType(IceType_i16, llvm::IntegerType::get(Context, 16));
  addLLVMType(IceType_i32, llvm::IntegerType::get(Context, 32));
  addLLVMType(IceType_i64, llvm::IntegerType::get(Context, 64));
  addLLVMType(IceType_f32, llvm::Type::getFloatTy(Context));
  addLLVMType(IceType_f64, llvm::Type::getDoubleTy(Context));
  addLLVMType(IceType_v4i1, llvm::VectorType::get(LLVMTypes[IceType_i1], 4));
  addLLVMType(IceType_v8i1, llvm::VectorType::get(LLVMTypes[IceType_i1], 8));
  addLLVMType(IceType_v16i1, llvm::VectorType::get(LLVMTypes[IceType_i1], 16));
  addLLVMType(IceType_v16i8, llvm::VectorType::get(LLVMTypes[IceType_i8], 16));
  addLLVMType(IceType_v8i16, llvm::VectorType::get(LLVMTypes[IceType_i16], 8));
  addLLVMType(IceType_v4i32, llvm::VectorType::get(LLVMTypes[IceType_i32], 4));
  addLLVMType(IceType_v4f32, llvm::VectorType::get(LLVMTypes[IceType_f32], 4));
  assert(LLVMTypes.size() == static_cast<size_t>(IceType_NUM));
}

void TypeConverter::addLLVMType(Type Ty, llvm::Type *LLVMTy) {
  assert(static_cast<size_t>(Ty) == LLVMTypes.size());
  LLVMTypes.push_back(LLVMTy);
  LLVM2IceMap[LLVMTy] = Ty;
}

Type TypeConverter::convertToIceTypeOther(llvm::Type *LLVMTy) const {
  switch (LLVMTy->getTypeID()) {
  case llvm::Type::PointerTyID:
  case llvm::Type::FunctionTyID:
    return getIcePointerType();
  default:
    return Ice::IceType_NUM;
  }
}

llvm::Type *TypeConverter::getLLVMIntegerType(unsigned NumBits) const {
  switch (NumBits) {
  case 1:
    return LLVMTypes[IceType_i1];
  case 8:
    return LLVMTypes[IceType_i8];
  case 16:
    return LLVMTypes[IceType_i16];
  case 32:
    return LLVMTypes[IceType_i32];
  case 64:
    return LLVMTypes[IceType_i64];
  default:
    return NULL;
  }
}

llvm::Type *TypeConverter::getLLVMVectorType(unsigned Size, Type Ty) const {
  switch (Ty) {
  case IceType_i1:
    switch (Size) {
    case 4:
      return convertToLLVMType(IceType_v4i1);
    case 8:
      return convertToLLVMType(IceType_v8i1);
    case 16:
      return convertToLLVMType(IceType_v16i1);
    default:
      break;
    }
    break;
  case IceType_i8:
    if (Size == 16)
      return convertToLLVMType(IceType_v16i8);
    break;
  case IceType_i16:
    if (Size == 8)
      return convertToLLVMType(IceType_v8i16);
    break;
  case IceType_i32:
    if (Size == 4)
      return convertToLLVMType(IceType_v4i32);
    break;
  case IceType_f32:
    if (Size == 4)
      return convertToLLVMType(IceType_v4f32);
    break;
  default:
    break;
  }
  return NULL;
}

} // end of Ice namespace.
