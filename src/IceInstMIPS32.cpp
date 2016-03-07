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
/// \brief Implements the InstMips32 and OperandMips32 classes, primarily the
/// constructors and the dump()/emit() methods.
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
#include <limits>

namespace Ice {
namespace MIPS32 {

bool OperandMIPS32Mem::canHoldOffset(Type Ty, bool SignExt, int32_t Offset) {
  (void)SignExt;
  (void)Ty;
  if ((std::numeric_limits<int16_t>::min() <= Offset) &&
      (Offset <= std::numeric_limits<int16_t>::max()))
    return true;
  return false;
}

OperandMIPS32Mem::OperandMIPS32Mem(Cfg *Func, Type Ty, Variable *Base,
                                   ConstantInteger32 *ImmOffset, AddrMode Mode)
    : OperandMIPS32(kMem, Ty), Base(Base), ImmOffset(ImmOffset), Mode(Mode) {
  // The Neg modes are only needed for Reg +/- Reg.
  (void)Func;
  // assert(!isNegAddrMode());
  NumVars = 1;
  Vars = &this->Base;
}

const char *InstMIPS32::getWidthString(Type Ty) {
  (void)Ty;
  return "TBD";
}

template <> const char *InstMIPS32Addiu::Opcode = "addiu";
template <> const char *InstMIPS32Lui::Opcode = "lui";
template <> const char *InstMIPS32La::Opcode = "la";
// Three-addr ops
template <> const char *InstMIPS32Add::Opcode = "add";
template <> const char *InstMIPS32Addu::Opcode = "addu";
template <> const char *InstMIPS32And::Opcode = "and";
template <> const char *InstMIPS32Mfhi::Opcode = "mfhi";
template <> const char *InstMIPS32Mflo::Opcode = "mflo";
template <> const char *InstMIPS32Mthi::Opcode = "mthi";
template <> const char *InstMIPS32Mtlo::Opcode = "mtlo";
template <> const char *InstMIPS32Mul::Opcode = "mul";
template <> const char *InstMIPS32Mult::Opcode = "mult";
template <> const char *InstMIPS32Multu::Opcode = "multu";
template <> const char *InstMIPS32Or::Opcode = "or";
template <> const char *InstMIPS32Ori::Opcode = "ori";
template <> const char *InstMIPS32Sltu::Opcode = "sltu";
template <> const char *InstMIPS32Sub::Opcode = "sub";
template <> const char *InstMIPS32Subu::Opcode = "subu";
template <> const char *InstMIPS32Xor::Opcode = "xor";

template <> void InstMIPS32Mflo::emit(const Cfg *Func) const {
  if (!BuildDefs::dump())
    return;
  emitUnaryopGPRFLoHi(Opcode, this, Func);
}

template <> void InstMIPS32Mfhi::emit(const Cfg *Func) const {
  if (!BuildDefs::dump())
    return;
  emitUnaryopGPRFLoHi(Opcode, this, Func);
}

template <> void InstMIPS32Mtlo::emit(const Cfg *Func) const {
  if (!BuildDefs::dump())
    return;
  emitUnaryopGPRTLoHi(Opcode, this, Func);
}

template <> void InstMIPS32Mthi::emit(const Cfg *Func) const {
  if (!BuildDefs::dump())
    return;
  emitUnaryopGPRTLoHi(Opcode, this, Func);
}

template <> void InstMIPS32Mult::emit(const Cfg *Func) const {
  if (!BuildDefs::dump())
    return;
  emitThreeAddrLoHi(Opcode, this, Func);
}

template <> void InstMIPS32Multu::emit(const Cfg *Func) const {
  if (!BuildDefs::dump())
    return;
  emitThreeAddrLoHi(Opcode, this, Func);
}

InstMIPS32Br::InstMIPS32Br(Cfg *Func, const CfgNode *TargetTrue,
                           const CfgNode *TargetFalse,
                           const InstMIPS32Label *Label)
    : InstMIPS32(Func, InstMIPS32::Br, 0, nullptr), TargetTrue(TargetTrue),
      TargetFalse(TargetFalse), Label(Label) {}

InstMIPS32Label::InstMIPS32Label(Cfg *Func, TargetMIPS32 *Target)
    : InstMIPS32(Func, InstMIPS32::Label, 0, nullptr),
      Number(Target->makeNextLabelNumber()) {}

IceString InstMIPS32Label::getName(const Cfg *Func) const {
  if (!BuildDefs::dump())
    return "";
  return ".L" + Func->getFunctionName() + "$local$__" + std::to_string(Number);
}

void InstMIPS32Label::dump(const Cfg *Func) const {
  if (!BuildDefs::dump())
    return;
  Ostream &Str = Func->getContext()->getStrDump();
  Str << getName(Func) << ":";
}

void InstMIPS32Label::emit(const Cfg *Func) const {
  if (!BuildDefs::dump())
    return;
  Ostream &Str = Func->getContext()->getStrEmit();
  Str << getName(Func) << ":";
}

void InstMIPS32Label::emitIAS(const Cfg *Func) const {
  (void)Func;
  llvm_unreachable("Not yet implemented");
}

InstMIPS32Call::InstMIPS32Call(Cfg *Func, Variable *Dest, Operand *CallTarget)
    : InstMIPS32(Func, InstMIPS32::Call, 1, Dest) {
  HasSideEffects = true;
  addSource(CallTarget);
}

InstMIPS32Mov::InstMIPS32Mov(Cfg *Func, Variable *Dest, Operand *Src)
    : InstMIPS32(Func, InstMIPS32::Mov, 2, Dest) {
  auto *Dest64 = llvm::dyn_cast<Variable64On32>(Dest);
  auto *Src64 = llvm::dyn_cast<Variable64On32>(Src);

  assert(Dest64 == nullptr || Src64 == nullptr);

  if (Dest64 != nullptr) {
    // this-> is needed below because there is a parameter named Dest.
    this->Dest = Dest64->getLo();
    DestHi = Dest64->getHi();
  }

  if (Src64 == nullptr) {
    addSource(Src);
  } else {
    addSource(Src64->getLo());
    addSource(Src64->getHi());
  }
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

void OperandMIPS32Mem::emit(const Cfg *Func) const {
  if (!BuildDefs::dump())
    return;
  llvm_unreachable("Not yet implemented");
  (void)Func;
}

void InstMIPS32::emitUnaryopGPR(const char *Opcode, const InstMIPS32 *Inst,
                                const Cfg *Func) {
  if (!BuildDefs::dump())
    return;
  Ostream &Str = Func->getContext()->getStrEmit();
  Str << "\t" << Opcode << "\t";
  Inst->getDest()->emit(Func);
  Str << ", ";
  Inst->getSrc(0)->emit(Func);
}
void InstMIPS32::emitUnaryopGPRFLoHi(const char *Opcode, const InstMIPS32 *Inst,
                                     const Cfg *Func) {
  if (!BuildDefs::dump())
    return;
  Ostream &Str = Func->getContext()->getStrEmit();
  Str << "\t" << Opcode << "\t";
  Inst->getDest()->emit(Func);
}

void InstMIPS32::emitUnaryopGPRTLoHi(const char *Opcode, const InstMIPS32 *Inst,
                                     const Cfg *Func) {
  if (!BuildDefs::dump())
    return;
  Ostream &Str = Func->getContext()->getStrEmit();
  Str << "\t" << Opcode << "\t";
  Inst->getSrc(0)->emit(Func);
}

void InstMIPS32::emitThreeAddr(const char *Opcode, const InstMIPS32 *Inst,
                               const Cfg *Func) {
  if (!BuildDefs::dump())
    return;
  Ostream &Str = Func->getContext()->getStrEmit();
  assert(Inst->getSrcSize() == 2);
  Str << "\t" << Opcode << "\t";
  Inst->getDest()->emit(Func);
  Str << ", ";
  Inst->getSrc(0)->emit(Func);
  Str << ", ";
  Inst->getSrc(1)->emit(Func);
}

void InstMIPS32::emitThreeAddrLoHi(const char *Opcode, const InstMIPS32 *Inst,
                                   const Cfg *Func) {
  if (!BuildDefs::dump())
    return;
  Ostream &Str = Func->getContext()->getStrEmit();
  assert(Inst->getSrcSize() == 2);
  Str << "\t" << Opcode << "\t";
  Inst->getSrc(0)->emit(Func);
  Str << ", ";
  Inst->getSrc(1)->emit(Func);
}

void InstMIPS32Ret::emit(const Cfg *Func) const {
  if (!BuildDefs::dump())
    return;
  assert(getSrcSize() > 0);
  auto *RA = llvm::cast<Variable>(getSrc(0));
  assert(RA->hasReg());
  assert(RA->getRegNum() == RegMIPS32::Reg_RA);
  Ostream &Str = Func->getContext()->getStrEmit();
  Str << "\t"
         "jr"
         "\t";
  RA->emit(Func);
}

void InstMIPS32Br::emit(const Cfg *Func) const {
  if (!BuildDefs::dump())
    return;
  Ostream &Str = Func->getContext()->getStrEmit();
  Str << "\t"
         "b"
      << "\t";
  if (Label) {
    Str << Label->getName(Func);
  } else {
    if (isUnconditionalBranch()) {
      Str << getTargetFalse()->getAsmName();
    } else {
      // TODO(reed kotler): Finish implementing conditional branch.
    }
  }
}

void InstMIPS32Call::emit(const Cfg *Func) const {
  if (!BuildDefs::dump())
    return;
  Ostream &Str = Func->getContext()->getStrEmit();
  assert(getSrcSize() == 1);
  if (llvm::isa<ConstantInteger32>(getCallTarget())) {
    // This shouldn't happen (typically have to copy the full 32-bits to a
    // register and do an indirect jump).
    llvm::report_fatal_error("MIPS2Call to ConstantInteger32");
  } else if (const auto *CallTarget =
                 llvm::dyn_cast<ConstantRelocatable>(getCallTarget())) {
    // Calls only have 24-bits, but the linker should insert veneers to extend
    // the range if needed.
    Str << "\t"
           "jal"
           "\t";
    CallTarget->emitWithoutPrefix(Func->getTarget());
  } else {
    Str << "\t"
           "jal"
           "\t";
    getCallTarget()->emit(Func);
  }
}

void InstMIPS32Call::emitIAS(const Cfg *Func) const {
  (void)Func;
  llvm_unreachable("Not yet implemented");
}

void InstMIPS32Call::dump(const Cfg *Func) const {
  if (!BuildDefs::dump())
    return;
  Ostream &Str = Func->getContext()->getStrDump();
  if (getDest()) {
    dumpDest(Func);
    Str << " = ";
  }
  Str << "call ";
  getCallTarget()->dump(Func);
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

void InstMIPS32Mov::emit(const Cfg *Func) const {
  if (!BuildDefs::dump())
    return;
  assert(!(isMultiDest() && isMultiSource()) && "Invalid vmov type.");
  if (isMultiDest()) {
    emitMultiDestSingleSource(Func);
    return;
  }

  if (isMultiSource()) {
    emitSingleDestMultiSource(Func);
    return;
  }

  emitSingleDestSingleSource(Func);
}

void InstMIPS32Mov::emitIAS(const Cfg *Func) const {
  assert(getSrcSize() == 1);
  (void)Func;
  llvm_unreachable("Not yet implemented");
}

void InstMIPS32Mov::dump(const Cfg *Func) const {
  if (!BuildDefs::dump())
    return;
  assert(getSrcSize() == 1 || getSrcSize() == 2);
  Ostream &Str = Func->getContext()->getStrDump();
  Variable *Dest = getDest();
  Variable *DestHi = getDestHi();
  Dest->dump(Func);
  if (DestHi) {
    Str << ", ";
    DestHi->dump(Func);
  }

  dumpOpcode(Str, " = mov", getDest()->getType());
  Str << " ";

  dumpSources(Func);
}

void InstMIPS32Mov::emitMultiDestSingleSource(const Cfg *Func) const {
  if (!BuildDefs::dump())
    return;
  Ostream &Str = Func->getContext()->getStrEmit();
  Variable *DestLo = getDest();
  Variable *DestHi = getDestHi();
  auto *Src = llvm::cast<Variable>(getSrc(0));

  assert(DestHi->hasReg());
  assert(DestLo->hasReg());
  assert(llvm::isa<Variable>(Src) && Src->hasReg());

  // Str << "\t"
  //    << "vmov" << getPredicate() << "\t";
  DestLo->emit(Func);
  Str << ", ";
  DestHi->emit(Func);
  Str << ", ";
  Src->emit(Func);
}

void InstMIPS32Mov::emitSingleDestMultiSource(const Cfg *Func) const {
  if (!BuildDefs::dump())
    return;
  Ostream &Str = Func->getContext()->getStrEmit();
  Variable *Dest = getDest();
  auto *SrcLo = llvm::cast<Variable>(getSrc(0));
  auto *SrcHi = llvm::cast<Variable>(getSrc(1));

  assert(SrcHi->hasReg());
  assert(SrcLo->hasReg());
  assert(Dest->hasReg());
  assert(getSrcSize() == 2);

  // Str << "\t"
  //    << "vmov" << getPredicate() << "\t";
  Dest->emit(Func);
  Str << ", ";
  SrcLo->emit(Func);
  Str << ", ";
  SrcHi->emit(Func);
}

void InstMIPS32Mov::emitSingleDestSingleSource(const Cfg *Func) const {
  if (!BuildDefs::dump())
    return;
  Ostream &Str = Func->getContext()->getStrEmit();
  Variable *Dest = getDest();
  Operand *Src = getSrc(0);
  auto *S = llvm::dyn_cast<Variable>(Src);
  Str << "\t";
  if (Dest->hasReg()) {
    if (S && S->hasReg())
      Str << "move";
    else
      Str << "lw";
  } else {
    if (S && S->hasReg()) {
      Str << "sw"
             "\t";
      getSrc(0)->emit(Func);
      Str << ", ";
      getDest()->emit(Func);
      return;
    } else
      Str << "move";
  }

  Str << "\t";
  getDest()->emit(Func);
  Str << ", ";
  getSrc(0)->emit(Func);
}

} // end of namespace MIPS32
} // end of namespace Ice
