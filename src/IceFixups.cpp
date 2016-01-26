//===- subzero/src/IceFixups.cpp - Implementation of Assembler Fixups -----===//
//
//                        The Subzero Code Generator
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief Implements the AssemblerFixup class, a very basic target-independent
/// representation of a fixup or relocation.
///
//===----------------------------------------------------------------------===//

#include "IceFixups.h"

#include "IceOperand.h"

namespace Ice {

const Constant *AssemblerFixup::NullSymbol = nullptr;

RelocOffsetT AssemblerFixup::offset() const {
  if (isNullSymbol())
    return addend_;
  if (const auto *CR = llvm::dyn_cast<ConstantRelocatable>(value_))
    return CR->getOffset() + addend_;
  return addend_;
}

IceString AssemblerFixup::symbol(const GlobalContext *Ctx,
                                 const Assembler *Asm) const {
  std::string Buffer;
  llvm::raw_string_ostream Str(Buffer);
  const Constant *C = value_;
  assert(!isNullSymbol());
  if (const auto *CR = llvm::dyn_cast<ConstantRelocatable>(C)) {
    if (CR->getSuppressMangling())
      Str << CR->getName();
    else
      Str << Ctx->mangleName(CR->getName());
    if (Asm && !Asm->fixupIsPCRel(kind()) && Ctx->getFlags().getUseNonsfi()) {
      Str << "@GOTOFF";
    }
  } else {
    // NOTE: currently only float/doubles are put into constant pools. In the
    // future we may put integers as well.
    assert(llvm::isa<ConstantFloat>(C) || llvm::isa<ConstantDouble>(C));
    C->emitPoolLabel(Str, Ctx);
  }
  return Str.str();
}

size_t AssemblerFixup::emit(GlobalContext *Ctx, const Assembler &Asm) const {
  static constexpr const size_t FixupSize = 4;
  if (!BuildDefs::dump())
    return FixupSize;
  Ostream &Str = Ctx->getStrEmit();
  Str << "\t.long ";
  if (isNullSymbol())
    Str << "__Sz_AbsoluteZero";
  else
    Str << symbol(Ctx, &Asm);
  RelocOffsetT Offset = Asm.load<RelocOffsetT>(position());
  if (Offset)
    Str << " + " << Offset;
  // For PCRel fixups, we write the pc-offset from a symbol into the Buffer
  // (e.g., -4), but we don't represent that in the fixup's offset. Otherwise
  // the fixup holds the true offset, and so does the Buffer. Just load the
  // offset from the buffer.
  if (Asm.fixupIsPCRel(kind()))
    Str << " - .";
  Str << "\n";
  return FixupSize;
}

size_t AssemblerTextFixup::emit(GlobalContext *Ctx, const Assembler &) const {
  Ctx->getStrEmit() << Message << "\n";
  return NumBytes;
}

} // end of namespace Ice
