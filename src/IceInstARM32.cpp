//===- subzero/src/IceInstARM32.cpp - ARM32 instruction implementation ----===//
//
//                        The Subzero Code Generator
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements the InstARM32 and OperandARM32 classes,
// primarily the constructors and the dump()/emit() methods.
//
//===----------------------------------------------------------------------===//

#include "assembler_arm32.h"
#include "IceCfg.h"
#include "IceCfgNode.h"
#include "IceInst.h"
#include "IceInstARM32.h"
#include "IceOperand.h"
#include "IceRegistersARM32.h"
#include "IceTargetLoweringARM32.h"

namespace Ice {

namespace {

const struct TypeARM32Attributes_ {
  const char *WidthString; // b, h, <blank>, or d
  int8_t SExtAddrOffsetBits;
  int8_t ZExtAddrOffsetBits;
} TypeARM32Attributes[] = {
#define X(tag, elementty, width, sbits, ubits)                                 \
  { width, sbits, ubits }                                                      \
  ,
    ICETYPEARM32_TABLE
#undef X
};

} // end of anonymous namespace

const char *InstARM32::getWidthString(Type Ty) {
  return TypeARM32Attributes[Ty].WidthString;
}

bool OperandARM32Mem::canHoldOffset(Type Ty, bool SignExt, int32_t Offset) {
  int32_t Bits = SignExt ? TypeARM32Attributes[Ty].SExtAddrOffsetBits
                         : TypeARM32Attributes[Ty].ZExtAddrOffsetBits;
  if (Bits == 0)
    return Offset == 0;
  // Note that encodings for offsets are sign-magnitude for ARM, so we check
  // with IsAbsoluteUint().
  if (isScalarFloatingType(Ty))
    return Utils::IsAligned(Offset, 4) && Utils::IsAbsoluteUint(Bits, Offset);
  return Utils::IsAbsoluteUint(Bits, Offset);
}

InstARM32Ret::InstARM32Ret(Cfg *Func, Variable *LR, Variable *Source)
    : InstARM32(Func, InstARM32::Ret, Source ? 2 : 1, nullptr) {
  addSource(LR);
  if (Source)
    addSource(Source);
}

// ======================== Dump routines ======================== //

void InstARM32::dump(const Cfg *Func) const {
  if (!ALLOW_DUMP)
    return;
  Ostream &Str = Func->getContext()->getStrDump();
  Str << "[ARM32] ";
  Inst::dump(Func);
}

void InstARM32Ret::emit(const Cfg *Func) const {
  if (!ALLOW_DUMP)
    return;
  assert(getSrcSize() > 0);
  Variable *LR = llvm::cast<Variable>(getSrc(0));
  assert(LR->hasReg());
  assert(LR->getRegNum() == RegARM32::Reg_lr);
  Ostream &Str = Func->getContext()->getStrEmit();
  Str << "\tbx\t";
  LR->emit(Func);
}

void InstARM32Ret::emitIAS(const Cfg *Func) const {
  (void)Func;
  llvm_unreachable("Not yet implemented");
}

void InstARM32Ret::dump(const Cfg *Func) const {
  if (!ALLOW_DUMP)
    return;
  Ostream &Str = Func->getContext()->getStrDump();
  Type Ty = (getSrcSize() == 1 ? IceType_void : getSrc(0)->getType());
  Str << "ret." << Ty << " ";
  dumpSources(Func);
}

} // end of namespace Ice
