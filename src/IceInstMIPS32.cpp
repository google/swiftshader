//===- subzero/src/IceInstMips32.cpp - Mips32 instruction implementation --===//
//
//                        The Subzero Code Generator
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
/// \file
/// This file implements the InstMips32 and OperandMips32 classes,
/// primarily the constructors and the dump()/emit() methods.
///
//===----------------------------------------------------------------------===//

#include "IceAssemblerMIPS32.h"
#include "IceCfg.h"
#include "IceCfgNode.h"
#include "IceInst.h"
#include "IceInstMIPS32.h"
#include "IceOperand.h"
#include "IceRegistersMIPS32.h"
#include "IceTargetLoweringMIPS32.h"

namespace Ice {

const char *InstMIPS32::getWidthString(Type Ty) {
  (void)Ty;
  return "TBD";
}

InstMIPS32Ret::InstMIPS32Ret(Cfg *Func, Variable *RA, Variable *Source)
    : InstMIPS32(Func, InstMIPS32::Ret, Source ? 2 : 1, nullptr) {
  addSource(RA);
  if (Source)
    addSource(Source);
}

// ======================== Dump routines ======================== //

void InstMIPS32::dump(const Cfg *Func) const {
  if (!BuildDefs::dump())
    return;
  Ostream &Str = Func->getContext()->getStrDump();
  Str << "[MIPS32] ";
  Inst::dump(Func);
}

void InstMIPS32Ret::emit(const Cfg *Func) const {
  if (!BuildDefs::dump())
    return;
  assert(getSrcSize() > 0);
  Variable *RA = llvm::cast<Variable>(getSrc(0));
  assert(RA->hasReg());
  assert(RA->getRegNum() == RegMIPS32::Reg_RA);
  Ostream &Str = Func->getContext()->getStrEmit();
  Str << "\t"
      << "jr $ra"
      << "\t";
  RA->emit(Func);
}

void InstMIPS32Ret::emitIAS(const Cfg *Func) const {
  (void)Func;
  llvm_unreachable("Not yet implemented");
}

void InstMIPS32Ret::dump(const Cfg *Func) const {
  if (!BuildDefs::dump())
    return;
  Ostream &Str = Func->getContext()->getStrDump();
  Type Ty = (getSrcSize() == 1 ? IceType_void : getSrc(0)->getType());
  Str << "ret." << Ty << " ";
  dumpSources(Func);
}
}
