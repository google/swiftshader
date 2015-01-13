//===- subzero/src/IceFixups.cpp - Implementation of Assembler Fixups -----===//
//
//                        The Subzero Code Generator
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements the AssemblerFixup class, a very basic
// target-independent representation of a fixup or relocation.
//
//===----------------------------------------------------------------------===//

#include "IceFixups.h"
#include "IceOperand.h"

namespace Ice {

RelocOffsetT AssemblerFixup::offset() const {
  if (const auto CR = llvm::dyn_cast<ConstantRelocatable>(value_))
    return CR->getOffset();
  return 0;
}

IceString AssemblerFixup::symbol(const GlobalContext *Ctx) const {
  std::string Buffer;
  llvm::raw_string_ostream Str(Buffer);
  const Constant *C = value_;
  if (const auto CR = llvm::dyn_cast<ConstantRelocatable>(C)) {
    if (CR->getSuppressMangling())
      Str << CR->getName();
    else
      Str << Ctx->mangleName(CR->getName());
  } else {
    // NOTE: currently only float/doubles are put into constant pools.
    // In the future we may put integers as well.
    assert(llvm::isa<ConstantFloat>(C) || llvm::isa<ConstantDouble>(C));
    C->emitPoolLabel(Str);
  }
  return Str.str();
}

void AssemblerFixup::emit(GlobalContext *Ctx) const {
  Ostream &Str = Ctx->getStrEmit();
  Str << symbol(Ctx);
  RelocOffsetT Offset = offset();
  if (Offset)
    Str << " + " << Offset;
}

} // end of namespace Ice
