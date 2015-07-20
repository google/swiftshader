//===- subzero/src/IceInstX86BaseImpl.h - Generic X86 instructions -*- C++ -*=//
//
//                        The Subzero Code Generator
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file implements the InstX86Base class and its descendants.
///
//===----------------------------------------------------------------------===//

#ifndef SUBZERO_SRC_ICEINSTX86BASEIMPL_H
#define SUBZERO_SRC_ICEINSTX86BASEIMPL_H

#include "IceInstX86Base.h"

#include "IceAssemblerX86Base.h"
#include "IceCfg.h"
#include "IceCfgNode.h"
#include "IceDefs.h"
#include "IceInst.h"
#include "IceOperand.h"
#include "IceTargetLowering.h"

namespace Ice {

namespace X86Internal {

template <class Machine>
const char *InstX86Base<Machine>::getWidthString(Type Ty) {
  return Traits::TypeAttributes[Ty].WidthString;
}

template <class Machine>
const char *InstX86Base<Machine>::getFldString(Type Ty) {
  return Traits::TypeAttributes[Ty].FldString;
}

template <class Machine>
typename InstX86Base<Machine>::Traits::Cond::BrCond
InstX86Base<Machine>::getOppositeCondition(typename Traits::Cond::BrCond Cond) {
  return Traits::InstBrAttributes[Cond].Opposite;
}

template <class Machine>
InstX86FakeRMW<Machine>::InstX86FakeRMW(Cfg *Func, Operand *Data, Operand *Addr,
                                        InstArithmetic::OpKind Op,
                                        Variable *Beacon)
    : InstX86Base<Machine>(Func, InstX86Base<Machine>::FakeRMW, 3, nullptr),
      Op(Op) {
  this->addSource(Data);
  this->addSource(Addr);
  this->addSource(Beacon);
}

template <class Machine>
InstX86AdjustStack<Machine>::InstX86AdjustStack(Cfg *Func, SizeT Amount,
                                                Variable *Esp)
    : InstX86Base<Machine>(Func, InstX86Base<Machine>::Adjuststack, 1, Esp),
      Amount(Amount) {
  this->addSource(Esp);
}

template <class Machine>
InstX86Mul<Machine>::InstX86Mul(Cfg *Func, Variable *Dest, Variable *Source1,
                                Operand *Source2)
    : InstX86Base<Machine>(Func, InstX86Base<Machine>::Mul, 2, Dest) {
  this->addSource(Source1);
  this->addSource(Source2);
}

template <class Machine>
InstX86Shld<Machine>::InstX86Shld(Cfg *Func, Variable *Dest, Variable *Source1,
                                  Variable *Source2)
    : InstX86Base<Machine>(Func, InstX86Base<Machine>::Shld, 3, Dest) {
  this->addSource(Dest);
  this->addSource(Source1);
  this->addSource(Source2);
}

template <class Machine>
InstX86Shrd<Machine>::InstX86Shrd(Cfg *Func, Variable *Dest, Variable *Source1,
                                  Variable *Source2)
    : InstX86Base<Machine>(Func, InstX86Base<Machine>::Shrd, 3, Dest) {
  this->addSource(Dest);
  this->addSource(Source1);
  this->addSource(Source2);
}

template <class Machine>
InstX86Label<Machine>::InstX86Label(
    Cfg *Func, typename InstX86Base<Machine>::Traits::TargetLowering *Target)
    : InstX86Base<Machine>(Func, InstX86Base<Machine>::Label, 0, nullptr),
      Number(Target->makeNextLabelNumber()) {}

template <class Machine>
IceString InstX86Label<Machine>::getName(const Cfg *Func) const {
  return ".L" + Func->getFunctionName() + "$local$__" + std::to_string(Number);
}

template <class Machine>
InstX86Br<Machine>::InstX86Br(
    Cfg *Func, const CfgNode *TargetTrue, const CfgNode *TargetFalse,
    const InstX86Label<Machine> *Label,
    typename InstX86Base<Machine>::Traits::Cond::BrCond Condition)
    : InstX86Base<Machine>(Func, InstX86Base<Machine>::Br, 0, nullptr),
      Condition(Condition), TargetTrue(TargetTrue), TargetFalse(TargetFalse),
      Label(Label) {}

template <class Machine>
bool InstX86Br<Machine>::optimizeBranch(const CfgNode *NextNode) {
  // If there is no next block, then there can be no fallthrough to
  // optimize.
  if (NextNode == nullptr)
    return false;
  // Intra-block conditional branches can't be optimized.
  if (Label)
    return false;
  // If there is no fallthrough node, such as a non-default case label
  // for a switch instruction, then there is no opportunity to
  // optimize.
  if (getTargetFalse() == nullptr)
    return false;

  // Unconditional branch to the next node can be removed.
  if (Condition == InstX86Base<Machine>::Traits::Cond::Br_None &&
      getTargetFalse() == NextNode) {
    assert(getTargetTrue() == nullptr);
    this->setDeleted();
    return true;
  }
  // If the fallthrough is to the next node, set fallthrough to nullptr
  // to indicate.
  if (getTargetFalse() == NextNode) {
    TargetFalse = nullptr;
    return true;
  }
  // If TargetTrue is the next node, and TargetFalse is not nullptr
  // (which was already tested above), then invert the branch
  // condition, swap the targets, and set new fallthrough to nullptr.
  if (getTargetTrue() == NextNode) {
    assert(Condition != InstX86Base<Machine>::Traits::Cond::Br_None);
    Condition = this->getOppositeCondition(Condition);
    TargetTrue = getTargetFalse();
    TargetFalse = nullptr;
    return true;
  }
  return false;
}

template <class Machine>
bool InstX86Br<Machine>::repointEdges(CfgNode *OldNode, CfgNode *NewNode) {
  bool Found = false;
  if (TargetFalse == OldNode) {
    TargetFalse = NewNode;
    Found = true;
  }
  if (TargetTrue == OldNode) {
    TargetTrue = NewNode;
    Found = true;
  }
  return Found;
}

template <class Machine>
InstX86Jmp<Machine>::InstX86Jmp(Cfg *Func, Operand *Target)
    : InstX86Base<Machine>(Func, InstX86Base<Machine>::Jmp, 1, nullptr) {
  this->addSource(Target);
}

template <class Machine>
InstX86Call<Machine>::InstX86Call(Cfg *Func, Variable *Dest,
                                  Operand *CallTarget)
    : InstX86Base<Machine>(Func, InstX86Base<Machine>::Call, 1, Dest) {
  this->HasSideEffects = true;
  this->addSource(CallTarget);
}

template <class Machine>
InstX86Cmov<Machine>::InstX86Cmov(
    Cfg *Func, Variable *Dest, Operand *Source,
    typename InstX86Base<Machine>::Traits::Cond::BrCond Condition)
    : InstX86Base<Machine>(Func, InstX86Base<Machine>::Cmov, 2, Dest),
      Condition(Condition) {
  // The final result is either the original Dest, or Source, so mark
  // both as sources.
  this->addSource(Dest);
  this->addSource(Source);
}

template <class Machine>
InstX86Cmpps<Machine>::InstX86Cmpps(
    Cfg *Func, Variable *Dest, Operand *Source,
    typename InstX86Base<Machine>::Traits::Cond::CmppsCond Condition)
    : InstX86Base<Machine>(Func, InstX86Base<Machine>::Cmpps, 2, Dest),
      Condition(Condition) {
  this->addSource(Dest);
  this->addSource(Source);
}

template <class Machine>
InstX86Cmpxchg<Machine>::InstX86Cmpxchg(Cfg *Func, Operand *DestOrAddr,
                                        Variable *Eax, Variable *Desired,
                                        bool Locked)
    : InstX86BaseLockable<Machine>(Func, InstX86Base<Machine>::Cmpxchg, 3,
                                   llvm::dyn_cast<Variable>(DestOrAddr),
                                   Locked) {
  assert(Eax->getRegNum() ==
         InstX86Base<Machine>::Traits::RegisterSet::Reg_eax);
  this->addSource(DestOrAddr);
  this->addSource(Eax);
  this->addSource(Desired);
}

template <class Machine>
InstX86Cmpxchg8b<Machine>::InstX86Cmpxchg8b(
    Cfg *Func, typename InstX86Base<Machine>::Traits::X86OperandMem *Addr,
    Variable *Edx, Variable *Eax, Variable *Ecx, Variable *Ebx, bool Locked)
    : InstX86BaseLockable<Machine>(Func, InstX86Base<Machine>::Cmpxchg, 5,
                                   nullptr, Locked) {
  assert(Edx->getRegNum() ==
         InstX86Base<Machine>::Traits::RegisterSet::Reg_edx);
  assert(Eax->getRegNum() ==
         InstX86Base<Machine>::Traits::RegisterSet::Reg_eax);
  assert(Ecx->getRegNum() ==
         InstX86Base<Machine>::Traits::RegisterSet::Reg_ecx);
  assert(Ebx->getRegNum() ==
         InstX86Base<Machine>::Traits::RegisterSet::Reg_ebx);
  this->addSource(Addr);
  this->addSource(Edx);
  this->addSource(Eax);
  this->addSource(Ecx);
  this->addSource(Ebx);
}

template <class Machine>
InstX86Cvt<Machine>::InstX86Cvt(Cfg *Func, Variable *Dest, Operand *Source,
                                CvtVariant Variant)
    : InstX86Base<Machine>(Func, InstX86Base<Machine>::Cvt, 1, Dest),
      Variant(Variant) {
  this->addSource(Source);
}

template <class Machine>
InstX86Icmp<Machine>::InstX86Icmp(Cfg *Func, Operand *Src0, Operand *Src1)
    : InstX86Base<Machine>(Func, InstX86Base<Machine>::Icmp, 2, nullptr) {
  this->addSource(Src0);
  this->addSource(Src1);
}

template <class Machine>
InstX86Ucomiss<Machine>::InstX86Ucomiss(Cfg *Func, Operand *Src0, Operand *Src1)
    : InstX86Base<Machine>(Func, InstX86Base<Machine>::Ucomiss, 2, nullptr) {
  this->addSource(Src0);
  this->addSource(Src1);
}

template <class Machine>
InstX86UD2<Machine>::InstX86UD2(Cfg *Func)
    : InstX86Base<Machine>(Func, InstX86Base<Machine>::UD2, 0, nullptr) {}

template <class Machine>
InstX86Test<Machine>::InstX86Test(Cfg *Func, Operand *Src1, Operand *Src2)
    : InstX86Base<Machine>(Func, InstX86Base<Machine>::Test, 2, nullptr) {
  this->addSource(Src1);
  this->addSource(Src2);
}

template <class Machine>
InstX86Mfence<Machine>::InstX86Mfence(Cfg *Func)
    : InstX86Base<Machine>(Func, InstX86Base<Machine>::Mfence, 0, nullptr) {
  this->HasSideEffects = true;
}

template <class Machine>
InstX86Store<Machine>::InstX86Store(
    Cfg *Func, Operand *Value,
    typename InstX86Base<Machine>::Traits::X86Operand *Mem)
    : InstX86Base<Machine>(Func, InstX86Base<Machine>::Store, 2, nullptr) {
  this->addSource(Value);
  this->addSource(Mem);
}

template <class Machine>
InstX86StoreP<Machine>::InstX86StoreP(
    Cfg *Func, Variable *Value,
    typename InstX86Base<Machine>::Traits::X86OperandMem *Mem)
    : InstX86Base<Machine>(Func, InstX86Base<Machine>::StoreP, 2, nullptr) {
  this->addSource(Value);
  this->addSource(Mem);
}

template <class Machine>
InstX86StoreQ<Machine>::InstX86StoreQ(
    Cfg *Func, Variable *Value,
    typename InstX86Base<Machine>::Traits::X86OperandMem *Mem)
    : InstX86Base<Machine>(Func, InstX86Base<Machine>::StoreQ, 2, nullptr) {
  this->addSource(Value);
  this->addSource(Mem);
}

template <class Machine>
InstX86Nop<Machine>::InstX86Nop(Cfg *Func, InstX86Nop::NopVariant Variant)
    : InstX86Base<Machine>(Func, InstX86Base<Machine>::Nop, 0, nullptr),
      Variant(Variant) {}

template <class Machine>
InstX86Fld<Machine>::InstX86Fld(Cfg *Func, Operand *Src)
    : InstX86Base<Machine>(Func, InstX86Base<Machine>::Fld, 1, nullptr) {
  this->addSource(Src);
}

template <class Machine>
InstX86Fstp<Machine>::InstX86Fstp(Cfg *Func, Variable *Dest)
    : InstX86Base<Machine>(Func, InstX86Base<Machine>::Fstp, 0, Dest) {}

template <class Machine>
InstX86Pop<Machine>::InstX86Pop(Cfg *Func, Variable *Dest)
    : InstX86Base<Machine>(Func, InstX86Base<Machine>::Pop, 0, Dest) {
  // A pop instruction affects the stack pointer and so it should not
  // be allowed to be automatically dead-code eliminated.  (The
  // corresponding push instruction doesn't need this treatment
  // because it has no dest variable and therefore won't be dead-code
  // eliminated.)  This is needed for late-stage liveness analysis
  // (e.g. asm-verbose mode).
  this->HasSideEffects = true;
}

template <class Machine>
InstX86Push<Machine>::InstX86Push(Cfg *Func, Variable *Source)
    : InstX86Base<Machine>(Func, InstX86Base<Machine>::Push, 1, nullptr) {
  this->addSource(Source);
}

template <class Machine>
InstX86Ret<Machine>::InstX86Ret(Cfg *Func, Variable *Source)
    : InstX86Base<Machine>(Func, InstX86Base<Machine>::Ret, Source ? 1 : 0,
                           nullptr) {
  if (Source)
    this->addSource(Source);
}

template <class Machine>
InstX86Setcc<Machine>::InstX86Setcc(
    Cfg *Func, Variable *Dest,
    typename InstX86Base<Machine>::Traits::Cond::BrCond Cond)
    : InstX86Base<Machine>(Func, InstX86Base<Machine>::Setcc, 0, Dest),
      Condition(Cond) {}

template <class Machine>
InstX86Xadd<Machine>::InstX86Xadd(Cfg *Func, Operand *Dest, Variable *Source,
                                  bool Locked)
    : InstX86BaseLockable<Machine>(Func, InstX86Base<Machine>::Xadd, 2,
                                   llvm::dyn_cast<Variable>(Dest), Locked) {
  this->addSource(Dest);
  this->addSource(Source);
}

template <class Machine>
InstX86Xchg<Machine>::InstX86Xchg(Cfg *Func, Operand *Dest, Variable *Source)
    : InstX86Base<Machine>(Func, InstX86Base<Machine>::Xchg, 2,
                           llvm::dyn_cast<Variable>(Dest)) {
  this->addSource(Dest);
  this->addSource(Source);
}

// ======================== Dump routines ======================== //

template <class Machine>
void InstX86Base<Machine>::dump(const Cfg *Func) const {
  if (!BuildDefs::dump())
    return;
  Ostream &Str = Func->getContext()->getStrDump();
  Str << "[" << Traits::TargetName << "] ";
  Inst::dump(Func);
}

template <class Machine>
void InstX86FakeRMW<Machine>::dump(const Cfg *Func) const {
  if (!BuildDefs::dump())
    return;
  Ostream &Str = Func->getContext()->getStrDump();
  Type Ty = getData()->getType();
  Str << "rmw " << InstArithmetic::getOpName(getOp()) << " " << Ty << " *";
  getAddr()->dump(Func);
  Str << ", ";
  getData()->dump(Func);
  Str << ", beacon=";
  getBeacon()->dump(Func);
}

template <class Machine>
void InstX86Label<Machine>::emit(const Cfg *Func) const {
  if (!BuildDefs::dump())
    return;
  Ostream &Str = Func->getContext()->getStrEmit();
  Str << getName(Func) << ":";
}

template <class Machine>
void InstX86Label<Machine>::emitIAS(const Cfg *Func) const {
  typename InstX86Base<Machine>::Traits::Assembler *Asm =
      Func->getAssembler<typename InstX86Base<Machine>::Traits::Assembler>();
  Asm->BindLocalLabel(Number);
}

template <class Machine>
void InstX86Label<Machine>::dump(const Cfg *Func) const {
  if (!BuildDefs::dump())
    return;
  Ostream &Str = Func->getContext()->getStrDump();
  Str << getName(Func) << ":";
}

template <class Machine> void InstX86Br<Machine>::emit(const Cfg *Func) const {
  if (!BuildDefs::dump())
    return;
  Ostream &Str = Func->getContext()->getStrEmit();
  Str << "\t";

  if (Condition == InstX86Base<Machine>::Traits::Cond::Br_None) {
    Str << "jmp";
  } else {
    Str << InstX86Base<Machine>::Traits::InstBrAttributes[Condition].EmitString;
  }

  if (Label) {
    Str << "\t" << Label->getName(Func);
  } else {
    if (Condition == InstX86Base<Machine>::Traits::Cond::Br_None) {
      Str << "\t" << getTargetFalse()->getAsmName();
    } else {
      Str << "\t" << getTargetTrue()->getAsmName();
      if (getTargetFalse()) {
        Str << "\n\tjmp\t" << getTargetFalse()->getAsmName();
      }
    }
  }
}

template <class Machine>
void InstX86Br<Machine>::emitIAS(const Cfg *Func) const {
  typename InstX86Base<Machine>::Traits::Assembler *Asm =
      Func->getAssembler<typename InstX86Base<Machine>::Traits::Assembler>();
  if (Label) {
    class Label *L = Asm->GetOrCreateLocalLabel(Label->getNumber());
    // In all these cases, local Labels should only be used for Near.
    const bool Near = true;
    if (Condition == InstX86Base<Machine>::Traits::Cond::Br_None) {
      Asm->jmp(L, Near);
    } else {
      Asm->j(Condition, L, Near);
    }
  } else {
    // Pessimistically assume it's far. This only affects Labels that
    // are not Bound.
    const bool Near = false;
    if (Condition == InstX86Base<Machine>::Traits::Cond::Br_None) {
      class Label *L =
          Asm->GetOrCreateCfgNodeLabel(getTargetFalse()->getIndex());
      assert(!getTargetTrue());
      Asm->jmp(L, Near);
    } else {
      class Label *L =
          Asm->GetOrCreateCfgNodeLabel(getTargetTrue()->getIndex());
      Asm->j(Condition, L, Near);
      if (getTargetFalse()) {
        class Label *L2 =
            Asm->GetOrCreateCfgNodeLabel(getTargetFalse()->getIndex());
        Asm->jmp(L2, Near);
      }
    }
  }
}

template <class Machine> void InstX86Br<Machine>::dump(const Cfg *Func) const {
  if (!BuildDefs::dump())
    return;
  Ostream &Str = Func->getContext()->getStrDump();
  Str << "br ";

  if (Condition == InstX86Base<Machine>::Traits::Cond::Br_None) {
    Str << "label %"
        << (Label ? Label->getName(Func) : getTargetFalse()->getName());
    return;
  }

  Str << InstX86Base<Machine>::Traits::InstBrAttributes[Condition]
             .DisplayString;
  if (Label) {
    Str << ", label %" << Label->getName(Func);
  } else {
    Str << ", label %" << getTargetTrue()->getName();
    if (getTargetFalse()) {
      Str << ", label %" << getTargetFalse()->getName();
    }
  }
}

template <class Machine> void InstX86Jmp<Machine>::emit(const Cfg *Func) const {
  if (!BuildDefs::dump())
    return;
  Ostream &Str = Func->getContext()->getStrEmit();
  assert(this->getSrcSize() == 1);
  Str << "\tjmp\t*";
  getJmpTarget()->emit(Func);
}

template <class Machine>
void InstX86Jmp<Machine>::emitIAS(const Cfg *Func) const {
  // Note: Adapted (mostly copied) from InstX86Call<Machine>::emitIAS().
  typename InstX86Base<Machine>::Traits::Assembler *Asm =
      Func->getAssembler<typename InstX86Base<Machine>::Traits::Assembler>();
  Operand *Target = getJmpTarget();
  if (const auto Var = llvm::dyn_cast<Variable>(Target)) {
    if (Var->hasReg()) {
      Asm->jmp(InstX86Base<Machine>::Traits::RegisterSet::getEncodedGPR(
          Var->getRegNum()));
    } else {
      // The jmp instruction with a memory operand should be possible
      // to encode, but it isn't a valid sandboxed instruction, and
      // there shouldn't be a register allocation issue to jump
      // through a scratch register, so we don't really need to bother
      // implementing it.
      llvm::report_fatal_error("Assembler can't jmp to memory operand");
    }
  } else if (const auto Mem = llvm::dyn_cast<
                 typename InstX86Base<Machine>::Traits::X86OperandMem>(
                 Target)) {
    (void)Mem;
    assert(Mem->getSegmentRegister() ==
           InstX86Base<Machine>::Traits::X86OperandMem::DefaultSegment);
    llvm::report_fatal_error("Assembler can't jmp to memory operand");
  } else if (const auto CR = llvm::dyn_cast<ConstantRelocatable>(Target)) {
    assert(CR->getOffset() == 0 && "We only support jumping to a function");
    Asm->jmp(CR);
  } else if (const auto Imm = llvm::dyn_cast<ConstantInteger32>(Target)) {
    // NaCl trampoline calls refer to an address within the sandbox directly.
    // This is usually only needed for non-IRT builds and otherwise not
    // very portable or stable. Usually this is only done for "calls"
    // and not jumps.
    // TODO(jvoung): Support this when there is a lowering that
    // actually triggers this case.
    (void)Imm;
    llvm::report_fatal_error("Unexpected jmp to absolute address");
  } else {
    llvm::report_fatal_error("Unexpected operand type");
  }
}

template <class Machine> void InstX86Jmp<Machine>::dump(const Cfg *Func) const {
  if (!BuildDefs::dump())
    return;
  Ostream &Str = Func->getContext()->getStrDump();
  Str << "jmp ";
  getJmpTarget()->dump(Func);
}

template <class Machine>
void InstX86Call<Machine>::emit(const Cfg *Func) const {
  if (!BuildDefs::dump())
    return;
  Ostream &Str = Func->getContext()->getStrEmit();
  assert(this->getSrcSize() == 1);
  Str << "\tcall\t";
  if (const auto CI = llvm::dyn_cast<ConstantInteger32>(getCallTarget())) {
    // Emit without a leading '$'.
    Str << CI->getValue();
  } else if (const auto CallTarget =
                 llvm::dyn_cast<ConstantRelocatable>(getCallTarget())) {
    CallTarget->emitWithoutPrefix(Func->getTarget());
  } else {
    Str << "*";
    getCallTarget()->emit(Func);
  }
  Func->getTarget()->resetStackAdjustment();
}

template <class Machine>
void InstX86Call<Machine>::emitIAS(const Cfg *Func) const {
  typename InstX86Base<Machine>::Traits::Assembler *Asm =
      Func->getAssembler<typename InstX86Base<Machine>::Traits::Assembler>();
  Operand *Target = getCallTarget();
  if (const auto Var = llvm::dyn_cast<Variable>(Target)) {
    if (Var->hasReg()) {
      Asm->call(InstX86Base<Machine>::Traits::RegisterSet::getEncodedGPR(
          Var->getRegNum()));
    } else {
      Asm->call(
          static_cast<typename InstX86Base<Machine>::Traits::TargetLowering *>(
              Func->getTarget())
              ->stackVarToAsmOperand(Var));
    }
  } else if (const auto Mem = llvm::dyn_cast<
                 typename InstX86Base<Machine>::Traits::X86OperandMem>(
                 Target)) {
    assert(Mem->getSegmentRegister() ==
           InstX86Base<Machine>::Traits::X86OperandMem::DefaultSegment);
    Asm->call(Mem->toAsmAddress(Asm));
  } else if (const auto CR = llvm::dyn_cast<ConstantRelocatable>(Target)) {
    assert(CR->getOffset() == 0 && "We only support calling a function");
    Asm->call(CR);
  } else if (const auto Imm = llvm::dyn_cast<ConstantInteger32>(Target)) {
    Asm->call(Immediate(Imm->getValue()));
  } else {
    llvm_unreachable("Unexpected operand type");
  }
  Func->getTarget()->resetStackAdjustment();
}

template <class Machine>
void InstX86Call<Machine>::dump(const Cfg *Func) const {
  if (!BuildDefs::dump())
    return;
  Ostream &Str = Func->getContext()->getStrDump();
  if (this->getDest()) {
    this->dumpDest(Func);
    Str << " = ";
  }
  Str << "call ";
  getCallTarget()->dump(Func);
}

// The ShiftHack parameter is used to emit "cl" instead of "ecx" for
// shift instructions, in order to be syntactically valid.  The
// this->Opcode parameter needs to be char* and not IceString because of
// template issues.
template <class Machine>
void InstX86Base<Machine>::emitTwoAddress(const char *Opcode, const Inst *Inst,
                                          const Cfg *Func, bool ShiftHack) {
  if (!BuildDefs::dump())
    return;
  Ostream &Str = Func->getContext()->getStrEmit();
  assert(Inst->getSrcSize() == 2);
  Operand *Dest = Inst->getDest();
  if (Dest == nullptr)
    Dest = Inst->getSrc(0);
  assert(Dest == Inst->getSrc(0));
  Operand *Src1 = Inst->getSrc(1);
  Str << "\t" << Opcode << InstX86Base<Machine>::getWidthString(Dest->getType())
      << "\t";
  const auto ShiftReg = llvm::dyn_cast<Variable>(Src1);
  if (ShiftHack && ShiftReg &&
      ShiftReg->getRegNum() ==
          InstX86Base<Machine>::Traits::RegisterSet::Reg_ecx)
    Str << "%cl";
  else
    Src1->emit(Func);
  Str << ", ";
  Dest->emit(Func);
}

template <class Machine>
void emitIASOpTyGPR(const Cfg *Func, Type Ty, const Operand *Op,
                    const typename InstX86Base<
                        Machine>::Traits::Assembler::GPREmitterOneOp &Emitter) {
  typename InstX86Base<Machine>::Traits::Assembler *Asm =
      Func->getAssembler<typename InstX86Base<Machine>::Traits::Assembler>();
  if (const auto Var = llvm::dyn_cast<Variable>(Op)) {
    if (Var->hasReg()) {
      // We cheat a little and use GPRRegister even for byte operations.
      typename InstX86Base<Machine>::Traits::RegisterSet::GPRRegister VarReg =
          InstX86Base<Machine>::Traits::RegisterSet::getEncodedByteRegOrGPR(
              Ty, Var->getRegNum());
      (Asm->*(Emitter.Reg))(Ty, VarReg);
    } else {
      typename InstX86Base<Machine>::Traits::Address StackAddr(
          static_cast<typename InstX86Base<Machine>::Traits::TargetLowering *>(
              Func->getTarget())
              ->stackVarToAsmOperand(Var));
      (Asm->*(Emitter.Addr))(Ty, StackAddr);
    }
  } else if (const auto Mem = llvm::dyn_cast<
                 typename InstX86Base<Machine>::Traits::X86OperandMem>(Op)) {
    Mem->emitSegmentOverride(Asm);
    (Asm->*(Emitter.Addr))(Ty, Mem->toAsmAddress(Asm));
  } else {
    llvm_unreachable("Unexpected operand type");
  }
}

template <class Machine, bool VarCanBeByte, bool SrcCanBeByte>
void emitIASRegOpTyGPR(
    const Cfg *Func, Type Ty, const Variable *Var, const Operand *Src,
    const typename InstX86Base<Machine>::Traits::Assembler::GPREmitterRegOp
        &Emitter) {
  typename InstX86Base<Machine>::Traits::Assembler *Asm =
      Func->getAssembler<typename InstX86Base<Machine>::Traits::Assembler>();
  assert(Var->hasReg());
  // We cheat a little and use GPRRegister even for byte operations.
  typename InstX86Base<Machine>::Traits::RegisterSet::GPRRegister VarReg =
      VarCanBeByte
          ? InstX86Base<Machine>::Traits::RegisterSet::getEncodedByteRegOrGPR(
                Ty, Var->getRegNum())
          : InstX86Base<Machine>::Traits::RegisterSet::getEncodedGPR(
                Var->getRegNum());
  if (const auto SrcVar = llvm::dyn_cast<Variable>(Src)) {
    if (SrcVar->hasReg()) {
      typename InstX86Base<Machine>::Traits::RegisterSet::GPRRegister SrcReg =
          SrcCanBeByte
              ? InstX86Base<Machine>::Traits::RegisterSet::
                    getEncodedByteRegOrGPR(Ty, SrcVar->getRegNum())
              : InstX86Base<Machine>::Traits::RegisterSet::getEncodedGPR(
                    SrcVar->getRegNum());
      (Asm->*(Emitter.GPRGPR))(Ty, VarReg, SrcReg);
    } else {
      typename InstX86Base<Machine>::Traits::Address SrcStackAddr =
          static_cast<typename InstX86Base<Machine>::Traits::TargetLowering *>(
              Func->getTarget())
              ->stackVarToAsmOperand(SrcVar);
      (Asm->*(Emitter.GPRAddr))(Ty, VarReg, SrcStackAddr);
    }
  } else if (const auto Mem = llvm::dyn_cast<
                 typename InstX86Base<Machine>::Traits::X86OperandMem>(Src)) {
    Mem->emitSegmentOverride(Asm);
    (Asm->*(Emitter.GPRAddr))(Ty, VarReg, Mem->toAsmAddress(Asm));
  } else if (const auto Imm = llvm::dyn_cast<ConstantInteger32>(Src)) {
    (Asm->*(Emitter.GPRImm))(Ty, VarReg, Immediate(Imm->getValue()));
  } else if (const auto Reloc = llvm::dyn_cast<ConstantRelocatable>(Src)) {
    AssemblerFixup *Fixup = Asm->createFixup(llvm::ELF::R_386_32, Reloc);
    (Asm->*(Emitter.GPRImm))(Ty, VarReg, Immediate(Reloc->getOffset(), Fixup));
  } else if (const auto Split = llvm::dyn_cast<
                 typename InstX86Base<Machine>::Traits::VariableSplit>(Src)) {
    (Asm->*(Emitter.GPRAddr))(Ty, VarReg, Split->toAsmAddress(Func));
  } else {
    llvm_unreachable("Unexpected operand type");
  }
}

template <class Machine>
void emitIASAddrOpTyGPR(
    const Cfg *Func, Type Ty,
    const typename InstX86Base<Machine>::Traits::Address &Addr,
    const Operand *Src,
    const typename InstX86Base<Machine>::Traits::Assembler::GPREmitterAddrOp
        &Emitter) {
  typename InstX86Base<Machine>::Traits::Assembler *Asm =
      Func->getAssembler<typename InstX86Base<Machine>::Traits::Assembler>();
  // Src can only be Reg or Immediate.
  if (const auto SrcVar = llvm::dyn_cast<Variable>(Src)) {
    assert(SrcVar->hasReg());
    typename InstX86Base<Machine>::Traits::RegisterSet::GPRRegister SrcReg =
        InstX86Base<Machine>::Traits::RegisterSet::getEncodedByteRegOrGPR(
            Ty, SrcVar->getRegNum());
    (Asm->*(Emitter.AddrGPR))(Ty, Addr, SrcReg);
  } else if (const auto Imm = llvm::dyn_cast<ConstantInteger32>(Src)) {
    (Asm->*(Emitter.AddrImm))(Ty, Addr, Immediate(Imm->getValue()));
  } else if (const auto Reloc = llvm::dyn_cast<ConstantRelocatable>(Src)) {
    AssemblerFixup *Fixup = Asm->createFixup(llvm::ELF::R_386_32, Reloc);
    (Asm->*(Emitter.AddrImm))(Ty, Addr, Immediate(Reloc->getOffset(), Fixup));
  } else {
    llvm_unreachable("Unexpected operand type");
  }
}

template <class Machine>
void emitIASAsAddrOpTyGPR(
    const Cfg *Func, Type Ty, const Operand *Op0, const Operand *Op1,
    const typename InstX86Base<Machine>::Traits::Assembler::GPREmitterAddrOp
        &Emitter) {
  if (const auto Op0Var = llvm::dyn_cast<Variable>(Op0)) {
    assert(!Op0Var->hasReg());
    typename InstX86Base<Machine>::Traits::Address StackAddr(
        static_cast<typename InstX86Base<Machine>::Traits::TargetLowering *>(
            Func->getTarget())
            ->stackVarToAsmOperand(Op0Var));
    emitIASAddrOpTyGPR<Machine>(Func, Ty, StackAddr, Op1, Emitter);
  } else if (const auto Op0Mem = llvm::dyn_cast<
                 typename InstX86Base<Machine>::Traits::X86OperandMem>(Op0)) {
    typename InstX86Base<Machine>::Traits::Assembler *Asm =
        Func->getAssembler<typename InstX86Base<Machine>::Traits::Assembler>();
    Op0Mem->emitSegmentOverride(Asm);
    emitIASAddrOpTyGPR<Machine>(Func, Ty, Op0Mem->toAsmAddress(Asm), Op1,
                                Emitter);
  } else if (const auto Split = llvm::dyn_cast<
                 typename InstX86Base<Machine>::Traits::VariableSplit>(Op0)) {
    emitIASAddrOpTyGPR<Machine>(Func, Ty, Split->toAsmAddress(Func), Op1,
                                Emitter);
  } else {
    llvm_unreachable("Unexpected operand type");
  }
}

template <class Machine>
void InstX86Base<Machine>::emitIASGPRShift(
    const Cfg *Func, Type Ty, const Variable *Var, const Operand *Src,
    const typename InstX86Base<Machine>::Traits::Assembler::GPREmitterShiftOp
        &Emitter) {
  typename InstX86Base<Machine>::Traits::Assembler *Asm =
      Func->getAssembler<typename InstX86Base<Machine>::Traits::Assembler>();
  // Technically, the Dest Var can be mem as well, but we only use Reg.
  // We can extend this to check Dest if we decide to use that form.
  assert(Var->hasReg());
  // We cheat a little and use GPRRegister even for byte operations.
  typename InstX86Base<Machine>::Traits::RegisterSet::GPRRegister VarReg =
      InstX86Base<Machine>::Traits::RegisterSet::getEncodedByteRegOrGPR(
          Ty, Var->getRegNum());
  // Src must be reg == ECX or an Imm8.
  // This is asserted by the assembler.
  if (const auto SrcVar = llvm::dyn_cast<Variable>(Src)) {
    assert(SrcVar->hasReg());
    typename InstX86Base<Machine>::Traits::RegisterSet::GPRRegister SrcReg =
        InstX86Base<Machine>::Traits::RegisterSet::getEncodedByteRegOrGPR(
            Ty, SrcVar->getRegNum());
    (Asm->*(Emitter.GPRGPR))(Ty, VarReg, SrcReg);
  } else if (const auto Imm = llvm::dyn_cast<ConstantInteger32>(Src)) {
    (Asm->*(Emitter.GPRImm))(Ty, VarReg, Immediate(Imm->getValue()));
  } else {
    llvm_unreachable("Unexpected operand type");
  }
}

template <class Machine>
void emitIASGPRShiftDouble(
    const Cfg *Func, const Variable *Dest, const Operand *Src1Op,
    const Operand *Src2Op,
    const typename InstX86Base<Machine>::Traits::Assembler::GPREmitterShiftD
        &Emitter) {
  typename InstX86Base<Machine>::Traits::Assembler *Asm =
      Func->getAssembler<typename InstX86Base<Machine>::Traits::Assembler>();
  // Dest can be reg or mem, but we only use the reg variant.
  assert(Dest->hasReg());
  typename InstX86Base<Machine>::Traits::RegisterSet::GPRRegister DestReg =
      InstX86Base<Machine>::Traits::RegisterSet::getEncodedGPR(
          Dest->getRegNum());
  // SrcVar1 must be reg.
  const auto SrcVar1 = llvm::cast<Variable>(Src1Op);
  assert(SrcVar1->hasReg());
  typename InstX86Base<Machine>::Traits::RegisterSet::GPRRegister SrcReg =
      InstX86Base<Machine>::Traits::RegisterSet::getEncodedGPR(
          SrcVar1->getRegNum());
  Type Ty = SrcVar1->getType();
  // Src2 can be the implicit CL register or an immediate.
  if (const auto Imm = llvm::dyn_cast<ConstantInteger32>(Src2Op)) {
    (Asm->*(Emitter.GPRGPRImm))(Ty, DestReg, SrcReg,
                                Immediate(Imm->getValue()));
  } else {
    assert(llvm::cast<Variable>(Src2Op)->getRegNum() ==
           InstX86Base<Machine>::Traits::RegisterSet::Reg_ecx);
    (Asm->*(Emitter.GPRGPR))(Ty, DestReg, SrcReg);
  }
}

template <class Machine>
void emitIASXmmShift(
    const Cfg *Func, Type Ty, const Variable *Var, const Operand *Src,
    const typename InstX86Base<Machine>::Traits::Assembler::XmmEmitterShiftOp
        &Emitter) {
  typename InstX86Base<Machine>::Traits::Assembler *Asm =
      Func->getAssembler<typename InstX86Base<Machine>::Traits::Assembler>();
  assert(Var->hasReg());
  typename InstX86Base<Machine>::Traits::RegisterSet::XmmRegister VarReg =
      InstX86Base<Machine>::Traits::RegisterSet::getEncodedXmm(
          Var->getRegNum());
  if (const auto SrcVar = llvm::dyn_cast<Variable>(Src)) {
    if (SrcVar->hasReg()) {
      typename InstX86Base<Machine>::Traits::RegisterSet::XmmRegister SrcReg =
          InstX86Base<Machine>::Traits::RegisterSet::getEncodedXmm(
              SrcVar->getRegNum());
      (Asm->*(Emitter.XmmXmm))(Ty, VarReg, SrcReg);
    } else {
      typename InstX86Base<Machine>::Traits::Address SrcStackAddr =
          static_cast<typename InstX86Base<Machine>::Traits::TargetLowering *>(
              Func->getTarget())
              ->stackVarToAsmOperand(SrcVar);
      (Asm->*(Emitter.XmmAddr))(Ty, VarReg, SrcStackAddr);
    }
  } else if (const auto Mem = llvm::dyn_cast<
                 typename InstX86Base<Machine>::Traits::X86OperandMem>(Src)) {
    assert(Mem->getSegmentRegister() ==
           InstX86Base<Machine>::Traits::X86OperandMem::DefaultSegment);
    (Asm->*(Emitter.XmmAddr))(Ty, VarReg, Mem->toAsmAddress(Asm));
  } else if (const auto Imm = llvm::dyn_cast<ConstantInteger32>(Src)) {
    (Asm->*(Emitter.XmmImm))(Ty, VarReg, Immediate(Imm->getValue()));
  } else {
    llvm_unreachable("Unexpected operand type");
  }
}

template <class Machine>
void emitIASRegOpTyXMM(
    const Cfg *Func, Type Ty, const Variable *Var, const Operand *Src,
    const typename InstX86Base<Machine>::Traits::Assembler::XmmEmitterRegOp
        &Emitter) {
  typename InstX86Base<Machine>::Traits::Assembler *Asm =
      Func->getAssembler<typename InstX86Base<Machine>::Traits::Assembler>();
  assert(Var->hasReg());
  typename InstX86Base<Machine>::Traits::RegisterSet::XmmRegister VarReg =
      InstX86Base<Machine>::Traits::RegisterSet::getEncodedXmm(
          Var->getRegNum());
  if (const auto SrcVar = llvm::dyn_cast<Variable>(Src)) {
    if (SrcVar->hasReg()) {
      typename InstX86Base<Machine>::Traits::RegisterSet::XmmRegister SrcReg =
          InstX86Base<Machine>::Traits::RegisterSet::getEncodedXmm(
              SrcVar->getRegNum());
      (Asm->*(Emitter.XmmXmm))(Ty, VarReg, SrcReg);
    } else {
      typename InstX86Base<Machine>::Traits::Address SrcStackAddr =
          static_cast<typename InstX86Base<Machine>::Traits::TargetLowering *>(
              Func->getTarget())
              ->stackVarToAsmOperand(SrcVar);
      (Asm->*(Emitter.XmmAddr))(Ty, VarReg, SrcStackAddr);
    }
  } else if (const auto Mem = llvm::dyn_cast<
                 typename InstX86Base<Machine>::Traits::X86OperandMem>(Src)) {
    assert(Mem->getSegmentRegister() ==
           InstX86Base<Machine>::Traits::X86OperandMem::DefaultSegment);
    (Asm->*(Emitter.XmmAddr))(Ty, VarReg, Mem->toAsmAddress(Asm));
  } else if (const auto Imm = llvm::dyn_cast<Constant>(Src)) {
    (Asm->*(Emitter.XmmAddr))(
        Ty, VarReg,
        InstX86Base<Machine>::Traits::Address::ofConstPool(Asm, Imm));
  } else {
    llvm_unreachable("Unexpected operand type");
  }
}

template <class Machine, typename DReg_t, typename SReg_t,
          DReg_t (*destEnc)(int32_t), SReg_t (*srcEnc)(int32_t)>
void emitIASCastRegOp(const Cfg *Func, Type DispatchTy, const Variable *Dest,
                      const Operand *Src,
                      const typename InstX86Base<Machine>::Traits::Assembler::
                          template CastEmitterRegOp<DReg_t, SReg_t> &Emitter) {
  typename InstX86Base<Machine>::Traits::Assembler *Asm =
      Func->getAssembler<typename InstX86Base<Machine>::Traits::Assembler>();
  assert(Dest->hasReg());
  DReg_t DestReg = destEnc(Dest->getRegNum());
  if (const auto SrcVar = llvm::dyn_cast<Variable>(Src)) {
    if (SrcVar->hasReg()) {
      SReg_t SrcReg = srcEnc(SrcVar->getRegNum());
      (Asm->*(Emitter.RegReg))(DispatchTy, DestReg, SrcReg);
    } else {
      typename InstX86Base<Machine>::Traits::Address SrcStackAddr =
          static_cast<typename InstX86Base<Machine>::Traits::TargetLowering *>(
              Func->getTarget())
              ->stackVarToAsmOperand(SrcVar);
      (Asm->*(Emitter.RegAddr))(DispatchTy, DestReg, SrcStackAddr);
    }
  } else if (const auto Mem = llvm::dyn_cast<
                 typename InstX86Base<Machine>::Traits::X86OperandMem>(Src)) {
    Mem->emitSegmentOverride(Asm);
    (Asm->*(Emitter.RegAddr))(DispatchTy, DestReg, Mem->toAsmAddress(Asm));
  } else {
    llvm_unreachable("Unexpected operand type");
  }
}

template <class Machine, typename DReg_t, typename SReg_t,
          DReg_t (*destEnc)(int32_t), SReg_t (*srcEnc)(int32_t)>
void emitIASThreeOpImmOps(
    const Cfg *Func, Type DispatchTy, const Variable *Dest, const Operand *Src0,
    const Operand *Src1,
    const typename InstX86Base<Machine>::Traits::Assembler::
        template ThreeOpImmEmitter<DReg_t, SReg_t> Emitter) {
  typename InstX86Base<Machine>::Traits::Assembler *Asm =
      Func->getAssembler<typename InstX86Base<Machine>::Traits::Assembler>();
  // This only handles Dest being a register, and Src1 being an immediate.
  assert(Dest->hasReg());
  DReg_t DestReg = destEnc(Dest->getRegNum());
  Immediate Imm(llvm::cast<ConstantInteger32>(Src1)->getValue());
  if (const auto SrcVar = llvm::dyn_cast<Variable>(Src0)) {
    if (SrcVar->hasReg()) {
      SReg_t SrcReg = srcEnc(SrcVar->getRegNum());
      (Asm->*(Emitter.RegRegImm))(DispatchTy, DestReg, SrcReg, Imm);
    } else {
      typename InstX86Base<Machine>::Traits::Address SrcStackAddr =
          static_cast<typename InstX86Base<Machine>::Traits::TargetLowering *>(
              Func->getTarget())
              ->stackVarToAsmOperand(SrcVar);
      (Asm->*(Emitter.RegAddrImm))(DispatchTy, DestReg, SrcStackAddr, Imm);
    }
  } else if (const auto Mem = llvm::dyn_cast<
                 typename InstX86Base<Machine>::Traits::X86OperandMem>(Src0)) {
    Mem->emitSegmentOverride(Asm);
    (Asm->*(Emitter.RegAddrImm))(DispatchTy, DestReg, Mem->toAsmAddress(Asm),
                                 Imm);
  } else {
    llvm_unreachable("Unexpected operand type");
  }
}

template <class Machine>
void emitIASMovlikeXMM(
    const Cfg *Func, const Variable *Dest, const Operand *Src,
    const typename InstX86Base<Machine>::Traits::Assembler::XmmEmitterMovOps
        Emitter) {
  typename InstX86Base<Machine>::Traits::Assembler *Asm =
      Func->getAssembler<typename InstX86Base<Machine>::Traits::Assembler>();
  if (Dest->hasReg()) {
    typename InstX86Base<Machine>::Traits::RegisterSet::XmmRegister DestReg =
        InstX86Base<Machine>::Traits::RegisterSet::getEncodedXmm(
            Dest->getRegNum());
    if (const auto SrcVar = llvm::dyn_cast<Variable>(Src)) {
      if (SrcVar->hasReg()) {
        (Asm->*(Emitter.XmmXmm))(
            DestReg, InstX86Base<Machine>::Traits::RegisterSet::getEncodedXmm(
                         SrcVar->getRegNum()));
      } else {
        typename InstX86Base<Machine>::Traits::Address StackAddr(
            static_cast<typename InstX86Base<Machine>::Traits::TargetLowering
                            *>(Func->getTarget())
                ->stackVarToAsmOperand(SrcVar));
        (Asm->*(Emitter.XmmAddr))(DestReg, StackAddr);
      }
    } else if (const auto SrcMem = llvm::dyn_cast<
                   typename InstX86Base<Machine>::Traits::X86OperandMem>(Src)) {
      assert(SrcMem->getSegmentRegister() ==
             InstX86Base<Machine>::Traits::X86OperandMem::DefaultSegment);
      (Asm->*(Emitter.XmmAddr))(DestReg, SrcMem->toAsmAddress(Asm));
    } else {
      llvm_unreachable("Unexpected operand type");
    }
  } else {
    typename InstX86Base<Machine>::Traits::Address StackAddr(
        static_cast<typename InstX86Base<Machine>::Traits::TargetLowering *>(
            Func->getTarget())
            ->stackVarToAsmOperand(Dest));
    // Src must be a register in this case.
    const auto SrcVar = llvm::cast<Variable>(Src);
    assert(SrcVar->hasReg());
    (Asm->*(Emitter.AddrXmm))(
        StackAddr, InstX86Base<Machine>::Traits::RegisterSet::getEncodedXmm(
                       SrcVar->getRegNum()));
  }
}

template <class Machine>
void InstX86Sqrtss<Machine>::emit(const Cfg *Func) const {
  if (!BuildDefs::dump())
    return;
  Ostream &Str = Func->getContext()->getStrEmit();
  assert(this->getSrcSize() == 1);
  Type Ty = this->getSrc(0)->getType();
  assert(isScalarFloatingType(Ty));
  Str << "\tsqrt" << InstX86Base<Machine>::Traits::TypeAttributes[Ty].SdSsString
      << "\t";
  this->getSrc(0)->emit(Func);
  Str << ", ";
  this->getDest()->emit(Func);
}

template <class Machine>
void InstX86Addss<Machine>::emit(const Cfg *Func) const {
  if (!BuildDefs::dump())
    return;
  char buf[30];
  snprintf(
      buf, llvm::array_lengthof(buf), "add%s",
      InstX86Base<Machine>::Traits::TypeAttributes[this->getDest()->getType()]
          .SdSsString);
  this->emitTwoAddress(buf, this, Func);
}

template <class Machine>
void InstX86Padd<Machine>::emit(const Cfg *Func) const {
  if (!BuildDefs::dump())
    return;
  char buf[30];
  snprintf(
      buf, llvm::array_lengthof(buf), "padd%s",
      InstX86Base<Machine>::Traits::TypeAttributes[this->getDest()->getType()]
          .PackString);
  this->emitTwoAddress(buf, this, Func);
}

template <class Machine>
void InstX86Pmull<Machine>::emit(const Cfg *Func) const {
  if (!BuildDefs::dump())
    return;
  char buf[30];
  bool TypesAreValid = this->getDest()->getType() == IceType_v4i32 ||
                       this->getDest()->getType() == IceType_v8i16;
  bool InstructionSetIsValid =
      this->getDest()->getType() == IceType_v8i16 ||
      static_cast<typename InstX86Base<Machine>::Traits::TargetLowering *>(
          Func->getTarget())
              ->getInstructionSet() >= InstX86Base<Machine>::Traits::SSE4_1;
  (void)TypesAreValid;
  (void)InstructionSetIsValid;
  assert(TypesAreValid);
  assert(InstructionSetIsValid);
  snprintf(
      buf, llvm::array_lengthof(buf), "pmull%s",
      InstX86Base<Machine>::Traits::TypeAttributes[this->getDest()->getType()]
          .PackString);
  this->emitTwoAddress(buf, this, Func);
}

template <class Machine>
void InstX86Pmull<Machine>::emitIAS(const Cfg *Func) const {
  Type Ty = this->getDest()->getType();
  bool TypesAreValid = Ty == IceType_v4i32 || Ty == IceType_v8i16;
  bool InstructionSetIsValid =
      Ty == IceType_v8i16 ||
      static_cast<typename InstX86Base<Machine>::Traits::TargetLowering *>(
          Func->getTarget())
              ->getInstructionSet() >= InstX86Base<Machine>::Traits::SSE4_1;
  (void)TypesAreValid;
  (void)InstructionSetIsValid;
  assert(TypesAreValid);
  assert(InstructionSetIsValid);
  assert(this->getSrcSize() == 2);
  Type ElementTy = typeElementType(Ty);
  emitIASRegOpTyXMM<Machine>(Func, ElementTy, this->getDest(), this->getSrc(1),
                             this->Emitter);
}

template <class Machine>
void InstX86Subss<Machine>::emit(const Cfg *Func) const {
  if (!BuildDefs::dump())
    return;
  char buf[30];
  snprintf(
      buf, llvm::array_lengthof(buf), "sub%s",
      InstX86Base<Machine>::Traits::TypeAttributes[this->getDest()->getType()]
          .SdSsString);
  this->emitTwoAddress(buf, this, Func);
}

template <class Machine>
void InstX86Psub<Machine>::emit(const Cfg *Func) const {
  if (!BuildDefs::dump())
    return;
  char buf[30];
  snprintf(
      buf, llvm::array_lengthof(buf), "psub%s",
      InstX86Base<Machine>::Traits::TypeAttributes[this->getDest()->getType()]
          .PackString);
  this->emitTwoAddress(buf, this, Func);
}

template <class Machine>
void InstX86Mulss<Machine>::emit(const Cfg *Func) const {
  if (!BuildDefs::dump())
    return;
  char buf[30];
  snprintf(
      buf, llvm::array_lengthof(buf), "mul%s",
      InstX86Base<Machine>::Traits::TypeAttributes[this->getDest()->getType()]
          .SdSsString);
  this->emitTwoAddress(buf, this, Func);
}

template <class Machine>
void InstX86Pmuludq<Machine>::emit(const Cfg *Func) const {
  if (!BuildDefs::dump())
    return;
  assert(this->getSrc(0)->getType() == IceType_v4i32 &&
         this->getSrc(1)->getType() == IceType_v4i32);
  this->emitTwoAddress(this->Opcode, this, Func);
}

template <class Machine>
void InstX86Divss<Machine>::emit(const Cfg *Func) const {
  if (!BuildDefs::dump())
    return;
  char buf[30];
  snprintf(
      buf, llvm::array_lengthof(buf), "div%s",
      InstX86Base<Machine>::Traits::TypeAttributes[this->getDest()->getType()]
          .SdSsString);
  this->emitTwoAddress(buf, this, Func);
}

template <class Machine> void InstX86Div<Machine>::emit(const Cfg *Func) const {
  if (!BuildDefs::dump())
    return;
  Ostream &Str = Func->getContext()->getStrEmit();
  assert(this->getSrcSize() == 3);
  Operand *Src1 = this->getSrc(1);
  Str << "\t" << this->Opcode << this->getWidthString(Src1->getType()) << "\t";
  Src1->emit(Func);
}

template <class Machine>
void InstX86Div<Machine>::emitIAS(const Cfg *Func) const {
  assert(this->getSrcSize() == 3);
  const Operand *Src = this->getSrc(1);
  Type Ty = Src->getType();
  static const typename InstX86Base<Machine>::Traits::Assembler::GPREmitterOneOp
      Emitter = {&InstX86Base<Machine>::Traits::Assembler::div,
                 &InstX86Base<Machine>::Traits::Assembler::div};
  emitIASOpTyGPR<Machine>(Func, Ty, Src, Emitter);
}

template <class Machine>
void InstX86Idiv<Machine>::emit(const Cfg *Func) const {
  if (!BuildDefs::dump())
    return;
  Ostream &Str = Func->getContext()->getStrEmit();
  assert(this->getSrcSize() == 3);
  Operand *Src1 = this->getSrc(1);
  Str << "\t" << this->Opcode << this->getWidthString(Src1->getType()) << "\t";
  Src1->emit(Func);
}

template <class Machine>
void InstX86Idiv<Machine>::emitIAS(const Cfg *Func) const {
  assert(this->getSrcSize() == 3);
  const Operand *Src = this->getSrc(1);
  Type Ty = Src->getType();
  static const typename InstX86Base<Machine>::Traits::Assembler::GPREmitterOneOp
      Emitter = {&InstX86Base<Machine>::Traits::Assembler::idiv,
                 &InstX86Base<Machine>::Traits::Assembler::idiv};
  emitIASOpTyGPR<Machine>(Func, Ty, Src, Emitter);
}

// pblendvb and blendvps take xmm0 as a final implicit argument.
template <class Machine>
void emitVariableBlendInst(const char *Opcode, const Inst *Inst,
                           const Cfg *Func) {
  if (!BuildDefs::dump())
    return;
  Ostream &Str = Func->getContext()->getStrEmit();
  assert(Inst->getSrcSize() == 3);
  assert(llvm::cast<Variable>(Inst->getSrc(2))->getRegNum() ==
         InstX86Base<Machine>::Traits::RegisterSet::Reg_xmm0);
  Str << "\t" << Opcode << "\t";
  Inst->getSrc(1)->emit(Func);
  Str << ", ";
  Inst->getDest()->emit(Func);
}

template <class Machine>
void emitIASVariableBlendInst(
    const Inst *Inst, const Cfg *Func,
    const typename InstX86Base<Machine>::Traits::Assembler::XmmEmitterRegOp
        &Emitter) {
  assert(Inst->getSrcSize() == 3);
  assert(llvm::cast<Variable>(Inst->getSrc(2))->getRegNum() ==
         InstX86Base<Machine>::Traits::RegisterSet::Reg_xmm0);
  const Variable *Dest = Inst->getDest();
  const Operand *Src = Inst->getSrc(1);
  emitIASRegOpTyXMM<Machine>(Func, Dest->getType(), Dest, Src, Emitter);
}

template <class Machine>
void InstX86Blendvps<Machine>::emit(const Cfg *Func) const {
  if (!BuildDefs::dump())
    return;
  assert(static_cast<typename InstX86Base<Machine>::Traits::TargetLowering *>(
             Func->getTarget())
             ->getInstructionSet() >= InstX86Base<Machine>::Traits::SSE4_1);
  emitVariableBlendInst<Machine>(this->Opcode, this, Func);
}

template <class Machine>
void InstX86Blendvps<Machine>::emitIAS(const Cfg *Func) const {
  assert(static_cast<typename InstX86Base<Machine>::Traits::TargetLowering *>(
             Func->getTarget())
             ->getInstructionSet() >= InstX86Base<Machine>::Traits::SSE4_1);
  static const typename InstX86Base<Machine>::Traits::Assembler::XmmEmitterRegOp
      Emitter = {&InstX86Base<Machine>::Traits::Assembler::blendvps,
                 &InstX86Base<Machine>::Traits::Assembler::blendvps};
  emitIASVariableBlendInst<Machine>(this, Func, Emitter);
}

template <class Machine>
void InstX86Pblendvb<Machine>::emit(const Cfg *Func) const {
  if (!BuildDefs::dump())
    return;
  assert(static_cast<typename InstX86Base<Machine>::Traits::TargetLowering *>(
             Func->getTarget())
             ->getInstructionSet() >= InstX86Base<Machine>::Traits::SSE4_1);
  emitVariableBlendInst<Machine>(this->Opcode, this, Func);
}

template <class Machine>
void InstX86Pblendvb<Machine>::emitIAS(const Cfg *Func) const {
  assert(static_cast<typename InstX86Base<Machine>::Traits::TargetLowering *>(
             Func->getTarget())
             ->getInstructionSet() >= InstX86Base<Machine>::Traits::SSE4_1);
  static const typename InstX86Base<Machine>::Traits::Assembler::XmmEmitterRegOp
      Emitter = {&InstX86Base<Machine>::Traits::Assembler::pblendvb,
                 &InstX86Base<Machine>::Traits::Assembler::pblendvb};
  emitIASVariableBlendInst<Machine>(this, Func, Emitter);
}

template <class Machine>
void InstX86Imul<Machine>::emit(const Cfg *Func) const {
  if (!BuildDefs::dump())
    return;
  Ostream &Str = Func->getContext()->getStrEmit();
  assert(this->getSrcSize() == 2);
  Variable *Dest = this->getDest();
  if (isByteSizedArithType(Dest->getType())) {
    // The 8-bit version of imul only allows the form "imul r/m8".
    const auto Src0Var = llvm::dyn_cast<Variable>(this->getSrc(0));
    (void)Src0Var;
    assert(Src0Var &&
           Src0Var->getRegNum() ==
               InstX86Base<Machine>::Traits::RegisterSet::Reg_eax);
    Str << "\timulb\t";
    this->getSrc(1)->emit(Func);
  } else if (llvm::isa<Constant>(this->getSrc(1))) {
    Str << "\timul" << this->getWidthString(Dest->getType()) << "\t";
    this->getSrc(1)->emit(Func);
    Str << ", ";
    this->getSrc(0)->emit(Func);
    Str << ", ";
    Dest->emit(Func);
  } else {
    this->emitTwoAddress("imul", this, Func);
  }
}

template <class Machine>
void InstX86Imul<Machine>::emitIAS(const Cfg *Func) const {
  assert(this->getSrcSize() == 2);
  const Variable *Var = this->getDest();
  Type Ty = Var->getType();
  const Operand *Src = this->getSrc(1);
  if (isByteSizedArithType(Ty)) {
    // The 8-bit version of imul only allows the form "imul r/m8".
    const auto Src0Var = llvm::dyn_cast<Variable>(this->getSrc(0));
    (void)Src0Var;
    assert(Src0Var &&
           Src0Var->getRegNum() ==
               InstX86Base<Machine>::Traits::RegisterSet::Reg_eax);
    static const typename InstX86Base<
        Machine>::Traits::Assembler::GPREmitterOneOp Emitter = {
        &InstX86Base<Machine>::Traits::Assembler::imul,
        &InstX86Base<Machine>::Traits::Assembler::imul};
    emitIASOpTyGPR<Machine>(Func, Ty, this->getSrc(1), Emitter);
  } else {
    // We only use imul as a two-address instruction even though
    // there is a 3 operand version when one of the operands is a constant.
    assert(Var == this->getSrc(0));
    static const typename InstX86Base<
        Machine>::Traits::Assembler::GPREmitterRegOp Emitter = {
        &InstX86Base<Machine>::Traits::Assembler::imul,
        &InstX86Base<Machine>::Traits::Assembler::imul,
        &InstX86Base<Machine>::Traits::Assembler::imul};
    emitIASRegOpTyGPR<Machine>(Func, Ty, Var, Src, Emitter);
  }
}

template <class Machine>
void InstX86Insertps<Machine>::emitIAS(const Cfg *Func) const {
  assert(this->getSrcSize() == 3);
  assert(static_cast<typename InstX86Base<Machine>::Traits::TargetLowering *>(
             Func->getTarget())
             ->getInstructionSet() >= InstX86Base<Machine>::Traits::SSE4_1);
  const Variable *Dest = this->getDest();
  assert(Dest == this->getSrc(0));
  Type Ty = Dest->getType();
  static const typename InstX86Base<Machine>::Traits::Assembler::
      template ThreeOpImmEmitter<
          typename InstX86Base<Machine>::Traits::RegisterSet::XmmRegister,
          typename InstX86Base<Machine>::Traits::RegisterSet::XmmRegister>
          Emitter = {&InstX86Base<Machine>::Traits::Assembler::insertps,
                     &InstX86Base<Machine>::Traits::Assembler::insertps};
  emitIASThreeOpImmOps<
      Machine, typename InstX86Base<Machine>::Traits::RegisterSet::XmmRegister,
      typename InstX86Base<Machine>::Traits::RegisterSet::XmmRegister,
      InstX86Base<Machine>::Traits::RegisterSet::getEncodedXmm,
      InstX86Base<Machine>::Traits::RegisterSet::getEncodedXmm>(
      Func, Ty, Dest, this->getSrc(1), this->getSrc(2), Emitter);
}

template <class Machine>
void InstX86Cbwdq<Machine>::emit(const Cfg *Func) const {
  if (!BuildDefs::dump())
    return;
  Ostream &Str = Func->getContext()->getStrEmit();
  assert(this->getSrcSize() == 1);
  Operand *Src0 = this->getSrc(0);
  assert(llvm::isa<Variable>(Src0));
  assert(llvm::cast<Variable>(Src0)->getRegNum() ==
         InstX86Base<Machine>::Traits::RegisterSet::Reg_eax);
  switch (Src0->getType()) {
  default:
    llvm_unreachable("unexpected source type!");
    break;
  case IceType_i8:
    assert(this->getDest()->getRegNum() ==
           InstX86Base<Machine>::Traits::RegisterSet::Reg_eax);
    Str << "\tcbtw";
    break;
  case IceType_i16:
    assert(this->getDest()->getRegNum() ==
           InstX86Base<Machine>::Traits::RegisterSet::Reg_edx);
    Str << "\tcwtd";
    break;
  case IceType_i32:
    assert(this->getDest()->getRegNum() ==
           InstX86Base<Machine>::Traits::RegisterSet::Reg_edx);
    Str << "\tcltd";
    break;
  }
}

template <class Machine>
void InstX86Cbwdq<Machine>::emitIAS(const Cfg *Func) const {
  typename InstX86Base<Machine>::Traits::Assembler *Asm =
      Func->getAssembler<typename InstX86Base<Machine>::Traits::Assembler>();
  assert(this->getSrcSize() == 1);
  Operand *Src0 = this->getSrc(0);
  assert(llvm::isa<Variable>(Src0));
  assert(llvm::cast<Variable>(Src0)->getRegNum() ==
         InstX86Base<Machine>::Traits::RegisterSet::Reg_eax);
  switch (Src0->getType()) {
  default:
    llvm_unreachable("unexpected source type!");
    break;
  case IceType_i8:
    assert(this->getDest()->getRegNum() ==
           InstX86Base<Machine>::Traits::RegisterSet::Reg_eax);
    Asm->cbw();
    break;
  case IceType_i16:
    assert(this->getDest()->getRegNum() ==
           InstX86Base<Machine>::Traits::RegisterSet::Reg_edx);
    Asm->cwd();
    break;
  case IceType_i32:
    assert(this->getDest()->getRegNum() ==
           InstX86Base<Machine>::Traits::RegisterSet::Reg_edx);
    Asm->cdq();
    break;
  }
}

template <class Machine> void InstX86Mul<Machine>::emit(const Cfg *Func) const {
  if (!BuildDefs::dump())
    return;
  Ostream &Str = Func->getContext()->getStrEmit();
  assert(this->getSrcSize() == 2);
  assert(llvm::isa<Variable>(this->getSrc(0)));
  assert(llvm::cast<Variable>(this->getSrc(0))->getRegNum() ==
         InstX86Base<Machine>::Traits::RegisterSet::Reg_eax);
  assert(
      this->getDest()->getRegNum() ==
      InstX86Base<Machine>::Traits::RegisterSet::Reg_eax); // TODO: allow edx?
  Str << "\tmul" << this->getWidthString(this->getDest()->getType()) << "\t";
  this->getSrc(1)->emit(Func);
}

template <class Machine>
void InstX86Mul<Machine>::emitIAS(const Cfg *Func) const {
  assert(this->getSrcSize() == 2);
  assert(llvm::isa<Variable>(this->getSrc(0)));
  assert(llvm::cast<Variable>(this->getSrc(0))->getRegNum() ==
         InstX86Base<Machine>::Traits::RegisterSet::Reg_eax);
  assert(
      this->getDest()->getRegNum() ==
      InstX86Base<Machine>::Traits::RegisterSet::Reg_eax); // TODO: allow edx?
  const Operand *Src = this->getSrc(1);
  Type Ty = Src->getType();
  static const typename InstX86Base<Machine>::Traits::Assembler::GPREmitterOneOp
      Emitter = {&InstX86Base<Machine>::Traits::Assembler::mul,
                 &InstX86Base<Machine>::Traits::Assembler::mul};
  emitIASOpTyGPR<Machine>(Func, Ty, Src, Emitter);
}

template <class Machine> void InstX86Mul<Machine>::dump(const Cfg *Func) const {
  if (!BuildDefs::dump())
    return;
  Ostream &Str = Func->getContext()->getStrDump();
  this->dumpDest(Func);
  Str << " = mul." << this->getDest()->getType() << " ";
  this->dumpSources(Func);
}

template <class Machine>
void InstX86Shld<Machine>::emit(const Cfg *Func) const {
  if (!BuildDefs::dump())
    return;
  Ostream &Str = Func->getContext()->getStrEmit();
  Variable *Dest = this->getDest();
  assert(this->getSrcSize() == 3);
  assert(Dest == this->getSrc(0));
  Str << "\tshld" << this->getWidthString(Dest->getType()) << "\t";
  if (const auto ShiftReg = llvm::dyn_cast<Variable>(this->getSrc(2))) {
    (void)ShiftReg;
    assert(ShiftReg->getRegNum() ==
           InstX86Base<Machine>::Traits::RegisterSet::Reg_ecx);
    Str << "%cl";
  } else {
    this->getSrc(2)->emit(Func);
  }
  Str << ", ";
  this->getSrc(1)->emit(Func);
  Str << ", ";
  Dest->emit(Func);
}

template <class Machine>
void InstX86Shld<Machine>::emitIAS(const Cfg *Func) const {
  assert(this->getSrcSize() == 3);
  assert(this->getDest() == this->getSrc(0));
  const Variable *Dest = this->getDest();
  const Operand *Src1 = this->getSrc(1);
  const Operand *Src2 = this->getSrc(2);
  static const typename InstX86Base<
      Machine>::Traits::Assembler::GPREmitterShiftD Emitter = {
      &InstX86Base<Machine>::Traits::Assembler::shld,
      &InstX86Base<Machine>::Traits::Assembler::shld};
  emitIASGPRShiftDouble<Machine>(Func, Dest, Src1, Src2, Emitter);
}

template <class Machine>
void InstX86Shld<Machine>::dump(const Cfg *Func) const {
  if (!BuildDefs::dump())
    return;
  Ostream &Str = Func->getContext()->getStrDump();
  this->dumpDest(Func);
  Str << " = shld." << this->getDest()->getType() << " ";
  this->dumpSources(Func);
}

template <class Machine>
void InstX86Shrd<Machine>::emit(const Cfg *Func) const {
  if (!BuildDefs::dump())
    return;
  Ostream &Str = Func->getContext()->getStrEmit();
  Variable *Dest = this->getDest();
  assert(this->getSrcSize() == 3);
  assert(Dest == this->getSrc(0));
  Str << "\tshrd" << this->getWidthString(Dest->getType()) << "\t";
  if (const auto ShiftReg = llvm::dyn_cast<Variable>(this->getSrc(2))) {
    (void)ShiftReg;
    assert(ShiftReg->getRegNum() ==
           InstX86Base<Machine>::Traits::RegisterSet::Reg_ecx);
    Str << "%cl";
  } else {
    this->getSrc(2)->emit(Func);
  }
  Str << ", ";
  this->getSrc(1)->emit(Func);
  Str << ", ";
  Dest->emit(Func);
}

template <class Machine>
void InstX86Shrd<Machine>::emitIAS(const Cfg *Func) const {
  assert(this->getSrcSize() == 3);
  assert(this->getDest() == this->getSrc(0));
  const Variable *Dest = this->getDest();
  const Operand *Src1 = this->getSrc(1);
  const Operand *Src2 = this->getSrc(2);
  static const typename InstX86Base<
      Machine>::Traits::Assembler::GPREmitterShiftD Emitter = {
      &InstX86Base<Machine>::Traits::Assembler::shrd,
      &InstX86Base<Machine>::Traits::Assembler::shrd};
  emitIASGPRShiftDouble<Machine>(Func, Dest, Src1, Src2, Emitter);
}

template <class Machine>
void InstX86Shrd<Machine>::dump(const Cfg *Func) const {
  if (!BuildDefs::dump())
    return;
  Ostream &Str = Func->getContext()->getStrDump();
  this->dumpDest(Func);
  Str << " = shrd." << this->getDest()->getType() << " ";
  this->dumpSources(Func);
}

template <class Machine>
void InstX86Cmov<Machine>::emit(const Cfg *Func) const {
  if (!BuildDefs::dump())
    return;
  Ostream &Str = Func->getContext()->getStrEmit();
  Variable *Dest = this->getDest();
  Str << "\t";
  assert(Condition != InstX86Base<Machine>::Traits::Cond::Br_None);
  assert(this->getDest()->hasReg());
  Str << "cmov"
      << InstX86Base<Machine>::Traits::InstBrAttributes[Condition].DisplayString
      << this->getWidthString(Dest->getType()) << "\t";
  this->getSrc(1)->emit(Func);
  Str << ", ";
  Dest->emit(Func);
}

template <class Machine>
void InstX86Cmov<Machine>::emitIAS(const Cfg *Func) const {
  assert(Condition != InstX86Base<Machine>::Traits::Cond::Br_None);
  assert(this->getDest()->hasReg());
  assert(this->getSrcSize() == 2);
  Operand *Src = this->getSrc(1);
  Type SrcTy = Src->getType();
  assert(SrcTy == IceType_i16 || SrcTy == IceType_i32);
  typename InstX86Base<Machine>::Traits::Assembler *Asm =
      Func->getAssembler<typename InstX86Base<Machine>::Traits::Assembler>();
  if (const auto *SrcVar = llvm::dyn_cast<Variable>(Src)) {
    if (SrcVar->hasReg()) {
      Asm->cmov(SrcTy, Condition,
                InstX86Base<Machine>::Traits::RegisterSet::getEncodedGPR(
                    this->getDest()->getRegNum()),
                InstX86Base<Machine>::Traits::RegisterSet::getEncodedGPR(
                    SrcVar->getRegNum()));
    } else {
      Asm->cmov(
          SrcTy, Condition,
          InstX86Base<Machine>::Traits::RegisterSet::getEncodedGPR(
              this->getDest()->getRegNum()),
          static_cast<typename InstX86Base<Machine>::Traits::TargetLowering *>(
              Func->getTarget())
              ->stackVarToAsmOperand(SrcVar));
    }
  } else if (const auto Mem = llvm::dyn_cast<
                 typename InstX86Base<Machine>::Traits::X86OperandMem>(Src)) {
    assert(Mem->getSegmentRegister() ==
           InstX86Base<Machine>::Traits::X86OperandMem::DefaultSegment);
    Asm->cmov(SrcTy, Condition,
              InstX86Base<Machine>::Traits::RegisterSet::getEncodedGPR(
                  this->getDest()->getRegNum()),
              Mem->toAsmAddress(Asm));
  } else {
    llvm_unreachable("Unexpected operand type");
  }
}

template <class Machine>
void InstX86Cmov<Machine>::dump(const Cfg *Func) const {
  if (!BuildDefs::dump())
    return;
  Ostream &Str = Func->getContext()->getStrDump();
  Str << "cmov"
      << InstX86Base<Machine>::Traits::InstBrAttributes[Condition].DisplayString
      << ".";
  Str << this->getDest()->getType() << " ";
  this->dumpDest(Func);
  Str << ", ";
  this->dumpSources(Func);
}

template <class Machine>
void InstX86Cmpps<Machine>::emit(const Cfg *Func) const {
  if (!BuildDefs::dump())
    return;
  Ostream &Str = Func->getContext()->getStrEmit();
  assert(this->getSrcSize() == 2);
  assert(Condition < InstX86Base<Machine>::Traits::Cond::Cmpps_Invalid);
  Str << "\t";
  Str << "cmp"
      << InstX86Base<Machine>::Traits::InstCmppsAttributes[Condition].EmitString
      << "ps"
      << "\t";
  this->getSrc(1)->emit(Func);
  Str << ", ";
  this->getDest()->emit(Func);
}

template <class Machine>
void InstX86Cmpps<Machine>::emitIAS(const Cfg *Func) const {
  typename InstX86Base<Machine>::Traits::Assembler *Asm =
      Func->getAssembler<typename InstX86Base<Machine>::Traits::Assembler>();
  assert(this->getSrcSize() == 2);
  assert(Condition < InstX86Base<Machine>::Traits::Cond::Cmpps_Invalid);
  // Assuming there isn't any load folding for cmpps, and vector constants
  // are not allowed in PNaCl.
  assert(llvm::isa<Variable>(this->getSrc(1)));
  const auto SrcVar = llvm::cast<Variable>(this->getSrc(1));
  if (SrcVar->hasReg()) {
    Asm->cmpps(InstX86Base<Machine>::Traits::RegisterSet::getEncodedXmm(
                   this->getDest()->getRegNum()),
               InstX86Base<Machine>::Traits::RegisterSet::getEncodedXmm(
                   SrcVar->getRegNum()),
               Condition);
  } else {
    typename InstX86Base<Machine>::Traits::Address SrcStackAddr =
        static_cast<typename InstX86Base<Machine>::Traits::TargetLowering *>(
            Func->getTarget())
            ->stackVarToAsmOperand(SrcVar);
    Asm->cmpps(InstX86Base<Machine>::Traits::RegisterSet::getEncodedXmm(
                   this->getDest()->getRegNum()),
               SrcStackAddr, Condition);
  }
}

template <class Machine>
void InstX86Cmpps<Machine>::dump(const Cfg *Func) const {
  if (!BuildDefs::dump())
    return;
  Ostream &Str = Func->getContext()->getStrDump();
  assert(Condition < InstX86Base<Machine>::Traits::Cond::Cmpps_Invalid);
  this->dumpDest(Func);
  Str << " = cmp"
      << InstX86Base<Machine>::Traits::InstCmppsAttributes[Condition].EmitString
      << "ps"
      << "\t";
  this->dumpSources(Func);
}

template <class Machine>
void InstX86Cmpxchg<Machine>::emit(const Cfg *Func) const {
  if (!BuildDefs::dump())
    return;
  Ostream &Str = Func->getContext()->getStrEmit();
  assert(this->getSrcSize() == 3);
  if (this->Locked) {
    Str << "\tlock";
  }
  Str << "\tcmpxchg" << this->getWidthString(this->getSrc(0)->getType())
      << "\t";
  this->getSrc(2)->emit(Func);
  Str << ", ";
  this->getSrc(0)->emit(Func);
}

template <class Machine>
void InstX86Cmpxchg<Machine>::emitIAS(const Cfg *Func) const {
  assert(this->getSrcSize() == 3);
  typename InstX86Base<Machine>::Traits::Assembler *Asm =
      Func->getAssembler<typename InstX86Base<Machine>::Traits::Assembler>();
  Type Ty = this->getSrc(0)->getType();
  const auto Mem =
      llvm::cast<typename InstX86Base<Machine>::Traits::X86OperandMem>(
          this->getSrc(0));
  assert(Mem->getSegmentRegister() ==
         InstX86Base<Machine>::Traits::X86OperandMem::DefaultSegment);
  const typename InstX86Base<Machine>::Traits::Address Addr =
      Mem->toAsmAddress(Asm);
  const auto VarReg = llvm::cast<Variable>(this->getSrc(2));
  assert(VarReg->hasReg());
  const typename InstX86Base<Machine>::Traits::RegisterSet::GPRRegister Reg =
      InstX86Base<Machine>::Traits::RegisterSet::getEncodedGPR(
          VarReg->getRegNum());
  Asm->cmpxchg(Ty, Addr, Reg, this->Locked);
}

template <class Machine>
void InstX86Cmpxchg<Machine>::dump(const Cfg *Func) const {
  if (!BuildDefs::dump())
    return;
  Ostream &Str = Func->getContext()->getStrDump();
  if (this->Locked) {
    Str << "lock ";
  }
  Str << "cmpxchg." << this->getSrc(0)->getType() << " ";
  this->dumpSources(Func);
}

template <class Machine>
void InstX86Cmpxchg8b<Machine>::emit(const Cfg *Func) const {
  if (!BuildDefs::dump())
    return;
  Ostream &Str = Func->getContext()->getStrEmit();
  assert(this->getSrcSize() == 5);
  if (this->Locked) {
    Str << "\tlock";
  }
  Str << "\tcmpxchg8b\t";
  this->getSrc(0)->emit(Func);
}

template <class Machine>
void InstX86Cmpxchg8b<Machine>::emitIAS(const Cfg *Func) const {
  assert(this->getSrcSize() == 5);
  typename InstX86Base<Machine>::Traits::Assembler *Asm =
      Func->getAssembler<typename InstX86Base<Machine>::Traits::Assembler>();
  const auto Mem =
      llvm::cast<typename InstX86Base<Machine>::Traits::X86OperandMem>(
          this->getSrc(0));
  assert(Mem->getSegmentRegister() ==
         InstX86Base<Machine>::Traits::X86OperandMem::DefaultSegment);
  const typename InstX86Base<Machine>::Traits::Address Addr =
      Mem->toAsmAddress(Asm);
  Asm->cmpxchg8b(Addr, this->Locked);
}

template <class Machine>
void InstX86Cmpxchg8b<Machine>::dump(const Cfg *Func) const {
  if (!BuildDefs::dump())
    return;
  Ostream &Str = Func->getContext()->getStrDump();
  if (this->Locked) {
    Str << "lock ";
  }
  Str << "cmpxchg8b ";
  this->dumpSources(Func);
}

template <class Machine> void InstX86Cvt<Machine>::emit(const Cfg *Func) const {
  if (!BuildDefs::dump())
    return;
  Ostream &Str = Func->getContext()->getStrEmit();
  assert(this->getSrcSize() == 1);
  Str << "\tcvt";
  if (isTruncating())
    Str << "t";
  Str << InstX86Base<Machine>::Traits::TypeAttributes[this->getSrc(0)
                                                          ->getType()]
             .CvtString << "2"
      << InstX86Base<
             Machine>::Traits::TypeAttributes[this->getDest()->getType()]
             .CvtString << "\t";
  this->getSrc(0)->emit(Func);
  Str << ", ";
  this->getDest()->emit(Func);
}

template <class Machine>
void InstX86Cvt<Machine>::emitIAS(const Cfg *Func) const {
  assert(this->getSrcSize() == 1);
  const Variable *Dest = this->getDest();
  const Operand *Src = this->getSrc(0);
  Type DestTy = Dest->getType();
  Type SrcTy = Src->getType();
  switch (Variant) {
  case Si2ss: {
    assert(isScalarIntegerType(SrcTy));
    assert(typeWidthInBytes(SrcTy) <= 4);
    assert(isScalarFloatingType(DestTy));
    static const typename InstX86Base<Machine>::Traits::Assembler::
        template CastEmitterRegOp<
            typename InstX86Base<Machine>::Traits::RegisterSet::XmmRegister,
            typename InstX86Base<Machine>::Traits::RegisterSet::GPRRegister>
            Emitter = {&InstX86Base<Machine>::Traits::Assembler::cvtsi2ss,
                       &InstX86Base<Machine>::Traits::Assembler::cvtsi2ss};
    emitIASCastRegOp<
        Machine,
        typename InstX86Base<Machine>::Traits::RegisterSet::XmmRegister,
        typename InstX86Base<Machine>::Traits::RegisterSet::GPRRegister,
        InstX86Base<Machine>::Traits::RegisterSet::getEncodedXmm,
        InstX86Base<Machine>::Traits::RegisterSet::getEncodedGPR>(
        Func, DestTy, Dest, Src, Emitter);
    return;
  }
  case Tss2si: {
    assert(isScalarFloatingType(SrcTy));
    assert(isScalarIntegerType(DestTy));
    assert(typeWidthInBytes(DestTy) <= 4);
    static const typename InstX86Base<Machine>::Traits::Assembler::
        template CastEmitterRegOp<
            typename InstX86Base<Machine>::Traits::RegisterSet::GPRRegister,
            typename InstX86Base<Machine>::Traits::RegisterSet::XmmRegister>
            Emitter = {&InstX86Base<Machine>::Traits::Assembler::cvttss2si,
                       &InstX86Base<Machine>::Traits::Assembler::cvttss2si};
    emitIASCastRegOp<
        Machine,
        typename InstX86Base<Machine>::Traits::RegisterSet::GPRRegister,
        typename InstX86Base<Machine>::Traits::RegisterSet::XmmRegister,
        InstX86Base<Machine>::Traits::RegisterSet::getEncodedGPR,
        InstX86Base<Machine>::Traits::RegisterSet::getEncodedXmm>(
        Func, SrcTy, Dest, Src, Emitter);
    return;
  }
  case Float2float: {
    assert(isScalarFloatingType(SrcTy));
    assert(isScalarFloatingType(DestTy));
    assert(DestTy != SrcTy);
    static const typename InstX86Base<
        Machine>::Traits::Assembler::XmmEmitterRegOp Emitter = {
        &InstX86Base<Machine>::Traits::Assembler::cvtfloat2float,
        &InstX86Base<Machine>::Traits::Assembler::cvtfloat2float};
    emitIASRegOpTyXMM<Machine>(Func, SrcTy, Dest, Src, Emitter);
    return;
  }
  case Dq2ps: {
    assert(isVectorIntegerType(SrcTy));
    assert(isVectorFloatingType(DestTy));
    static const typename InstX86Base<
        Machine>::Traits::Assembler::XmmEmitterRegOp Emitter = {
        &InstX86Base<Machine>::Traits::Assembler::cvtdq2ps,
        &InstX86Base<Machine>::Traits::Assembler::cvtdq2ps};
    emitIASRegOpTyXMM<Machine>(Func, DestTy, Dest, Src, Emitter);
    return;
  }
  case Tps2dq: {
    assert(isVectorFloatingType(SrcTy));
    assert(isVectorIntegerType(DestTy));
    static const typename InstX86Base<
        Machine>::Traits::Assembler::XmmEmitterRegOp Emitter = {
        &InstX86Base<Machine>::Traits::Assembler::cvttps2dq,
        &InstX86Base<Machine>::Traits::Assembler::cvttps2dq};
    emitIASRegOpTyXMM<Machine>(Func, DestTy, Dest, Src, Emitter);
    return;
  }
  }
}

template <class Machine> void InstX86Cvt<Machine>::dump(const Cfg *Func) const {
  if (!BuildDefs::dump())
    return;
  Ostream &Str = Func->getContext()->getStrDump();
  this->dumpDest(Func);
  Str << " = cvt";
  if (isTruncating())
    Str << "t";
  Str << InstX86Base<Machine>::Traits::TypeAttributes[this->getSrc(0)
                                                          ->getType()]
             .CvtString << "2"
      << InstX86Base<
             Machine>::Traits::TypeAttributes[this->getDest()->getType()]
             .CvtString << " ";
  this->dumpSources(Func);
}

template <class Machine>
void InstX86Icmp<Machine>::emit(const Cfg *Func) const {
  if (!BuildDefs::dump())
    return;
  Ostream &Str = Func->getContext()->getStrEmit();
  assert(this->getSrcSize() == 2);
  Str << "\tcmp" << this->getWidthString(this->getSrc(0)->getType()) << "\t";
  this->getSrc(1)->emit(Func);
  Str << ", ";
  this->getSrc(0)->emit(Func);
}

template <class Machine>
void InstX86Icmp<Machine>::emitIAS(const Cfg *Func) const {
  assert(this->getSrcSize() == 2);
  const Operand *Src0 = this->getSrc(0);
  const Operand *Src1 = this->getSrc(1);
  Type Ty = Src0->getType();
  static const typename InstX86Base<Machine>::Traits::Assembler::GPREmitterRegOp
      RegEmitter = {&InstX86Base<Machine>::Traits::Assembler::cmp,
                    &InstX86Base<Machine>::Traits::Assembler::cmp,
                    &InstX86Base<Machine>::Traits::Assembler::cmp};
  static const typename InstX86Base<
      Machine>::Traits::Assembler::GPREmitterAddrOp AddrEmitter = {
      &InstX86Base<Machine>::Traits::Assembler::cmp,
      &InstX86Base<Machine>::Traits::Assembler::cmp};
  if (const auto SrcVar0 = llvm::dyn_cast<Variable>(Src0)) {
    if (SrcVar0->hasReg()) {
      emitIASRegOpTyGPR<Machine>(Func, Ty, SrcVar0, Src1, RegEmitter);
      return;
    }
  }
  emitIASAsAddrOpTyGPR<Machine>(Func, Ty, Src0, Src1, AddrEmitter);
}

template <class Machine>
void InstX86Icmp<Machine>::dump(const Cfg *Func) const {
  if (!BuildDefs::dump())
    return;
  Ostream &Str = Func->getContext()->getStrDump();
  Str << "cmp." << this->getSrc(0)->getType() << " ";
  this->dumpSources(Func);
}

template <class Machine>
void InstX86Ucomiss<Machine>::emit(const Cfg *Func) const {
  if (!BuildDefs::dump())
    return;
  Ostream &Str = Func->getContext()->getStrEmit();
  assert(this->getSrcSize() == 2);
  Str << "\tucomi"
      << InstX86Base<Machine>::Traits::TypeAttributes[this->getSrc(0)
                                                          ->getType()]
             .SdSsString << "\t";
  this->getSrc(1)->emit(Func);
  Str << ", ";
  this->getSrc(0)->emit(Func);
}

template <class Machine>
void InstX86Ucomiss<Machine>::emitIAS(const Cfg *Func) const {
  assert(this->getSrcSize() == 2);
  // Currently src0 is always a variable by convention, to avoid having
  // two memory operands.
  assert(llvm::isa<Variable>(this->getSrc(0)));
  const auto Src0Var = llvm::cast<Variable>(this->getSrc(0));
  Type Ty = Src0Var->getType();
  static const typename InstX86Base<Machine>::Traits::Assembler::XmmEmitterRegOp
      Emitter = {&InstX86Base<Machine>::Traits::Assembler::ucomiss,
                 &InstX86Base<Machine>::Traits::Assembler::ucomiss};
  emitIASRegOpTyXMM<Machine>(Func, Ty, Src0Var, this->getSrc(1), Emitter);
}

template <class Machine>
void InstX86Ucomiss<Machine>::dump(const Cfg *Func) const {
  if (!BuildDefs::dump())
    return;
  Ostream &Str = Func->getContext()->getStrDump();
  Str << "ucomiss." << this->getSrc(0)->getType() << " ";
  this->dumpSources(Func);
}

template <class Machine> void InstX86UD2<Machine>::emit(const Cfg *Func) const {
  if (!BuildDefs::dump())
    return;
  Ostream &Str = Func->getContext()->getStrEmit();
  assert(this->getSrcSize() == 0);
  Str << "\tud2";
}

template <class Machine>
void InstX86UD2<Machine>::emitIAS(const Cfg *Func) const {
  typename InstX86Base<Machine>::Traits::Assembler *Asm =
      Func->getAssembler<typename InstX86Base<Machine>::Traits::Assembler>();
  Asm->ud2();
}

template <class Machine> void InstX86UD2<Machine>::dump(const Cfg *Func) const {
  if (!BuildDefs::dump())
    return;
  Ostream &Str = Func->getContext()->getStrDump();
  Str << "ud2";
}

template <class Machine>
void InstX86Test<Machine>::emit(const Cfg *Func) const {
  if (!BuildDefs::dump())
    return;
  Ostream &Str = Func->getContext()->getStrEmit();
  assert(this->getSrcSize() == 2);
  Str << "\ttest" << this->getWidthString(this->getSrc(0)->getType()) << "\t";
  this->getSrc(1)->emit(Func);
  Str << ", ";
  this->getSrc(0)->emit(Func);
}

template <class Machine>
void InstX86Test<Machine>::emitIAS(const Cfg *Func) const {
  assert(this->getSrcSize() == 2);
  const Operand *Src0 = this->getSrc(0);
  const Operand *Src1 = this->getSrc(1);
  Type Ty = Src0->getType();
  // The Reg/Addr form of test is not encodeable.
  static const typename InstX86Base<Machine>::Traits::Assembler::GPREmitterRegOp
      RegEmitter = {&InstX86Base<Machine>::Traits::Assembler::test, nullptr,
                    &InstX86Base<Machine>::Traits::Assembler::test};
  static const typename InstX86Base<
      Machine>::Traits::Assembler::GPREmitterAddrOp AddrEmitter = {
      &InstX86Base<Machine>::Traits::Assembler::test,
      &InstX86Base<Machine>::Traits::Assembler::test};
  if (const auto SrcVar0 = llvm::dyn_cast<Variable>(Src0)) {
    if (SrcVar0->hasReg()) {
      emitIASRegOpTyGPR<Machine>(Func, Ty, SrcVar0, Src1, RegEmitter);
      return;
    }
  }
  llvm_unreachable("Nothing actually generates this so it's untested");
  emitIASAsAddrOpTyGPR<Machine>(Func, Ty, Src0, Src1, AddrEmitter);
}

template <class Machine>
void InstX86Test<Machine>::dump(const Cfg *Func) const {
  if (!BuildDefs::dump())
    return;
  Ostream &Str = Func->getContext()->getStrDump();
  Str << "test." << this->getSrc(0)->getType() << " ";
  this->dumpSources(Func);
}

template <class Machine>
void InstX86Mfence<Machine>::emit(const Cfg *Func) const {
  if (!BuildDefs::dump())
    return;
  Ostream &Str = Func->getContext()->getStrEmit();
  assert(this->getSrcSize() == 0);
  Str << "\tmfence";
}

template <class Machine>
void InstX86Mfence<Machine>::emitIAS(const Cfg *Func) const {
  typename InstX86Base<Machine>::Traits::Assembler *Asm =
      Func->getAssembler<typename InstX86Base<Machine>::Traits::Assembler>();
  Asm->mfence();
}

template <class Machine>
void InstX86Mfence<Machine>::dump(const Cfg *Func) const {
  if (!BuildDefs::dump())
    return;
  Ostream &Str = Func->getContext()->getStrDump();
  Str << "mfence";
}

template <class Machine>
void InstX86Store<Machine>::emit(const Cfg *Func) const {
  if (!BuildDefs::dump())
    return;
  Ostream &Str = Func->getContext()->getStrEmit();
  assert(this->getSrcSize() == 2);
  Type Ty = this->getSrc(0)->getType();
  Str << "\tmov" << this->getWidthString(Ty)
      << InstX86Base<Machine>::Traits::TypeAttributes[Ty].SdSsString << "\t";
  this->getSrc(0)->emit(Func);
  Str << ", ";
  this->getSrc(1)->emit(Func);
}

template <class Machine>
void InstX86Store<Machine>::emitIAS(const Cfg *Func) const {
  assert(this->getSrcSize() == 2);
  const Operand *Dest = this->getSrc(1);
  const Operand *Src = this->getSrc(0);
  Type DestTy = Dest->getType();
  if (isScalarFloatingType(DestTy)) {
    // Src must be a register, since Dest is a Mem operand of some kind.
    const auto SrcVar = llvm::cast<Variable>(Src);
    assert(SrcVar->hasReg());
    typename InstX86Base<Machine>::Traits::RegisterSet::XmmRegister SrcReg =
        InstX86Base<Machine>::Traits::RegisterSet::getEncodedXmm(
            SrcVar->getRegNum());
    typename InstX86Base<Machine>::Traits::Assembler *Asm =
        Func->getAssembler<typename InstX86Base<Machine>::Traits::Assembler>();
    if (const auto DestVar = llvm::dyn_cast<Variable>(Dest)) {
      assert(!DestVar->hasReg());
      typename InstX86Base<Machine>::Traits::Address StackAddr(
          static_cast<typename InstX86Base<Machine>::Traits::TargetLowering *>(
              Func->getTarget())
              ->stackVarToAsmOperand(DestVar));
      Asm->movss(DestTy, StackAddr, SrcReg);
    } else {
      const auto DestMem =
          llvm::cast<typename InstX86Base<Machine>::Traits::X86OperandMem>(
              Dest);
      assert(DestMem->getSegmentRegister() ==
             InstX86Base<Machine>::Traits::X86OperandMem::DefaultSegment);
      Asm->movss(DestTy, DestMem->toAsmAddress(Asm), SrcReg);
    }
    return;
  } else {
    assert(isScalarIntegerType(DestTy));
    static const typename InstX86Base<
        Machine>::Traits::Assembler::GPREmitterAddrOp GPRAddrEmitter = {
        &InstX86Base<Machine>::Traits::Assembler::mov,
        &InstX86Base<Machine>::Traits::Assembler::mov};
    emitIASAsAddrOpTyGPR<Machine>(Func, DestTy, Dest, Src, GPRAddrEmitter);
  }
}

template <class Machine>
void InstX86Store<Machine>::dump(const Cfg *Func) const {
  if (!BuildDefs::dump())
    return;
  Ostream &Str = Func->getContext()->getStrDump();
  Str << "mov." << this->getSrc(0)->getType() << " ";
  this->getSrc(1)->dump(Func);
  Str << ", ";
  this->getSrc(0)->dump(Func);
}

template <class Machine>
void InstX86StoreP<Machine>::emit(const Cfg *Func) const {
  if (!BuildDefs::dump())
    return;
  Ostream &Str = Func->getContext()->getStrEmit();
  assert(this->getSrcSize() == 2);
  Str << "\tmovups\t";
  this->getSrc(0)->emit(Func);
  Str << ", ";
  this->getSrc(1)->emit(Func);
}

template <class Machine>
void InstX86StoreP<Machine>::emitIAS(const Cfg *Func) const {
  typename InstX86Base<Machine>::Traits::Assembler *Asm =
      Func->getAssembler<typename InstX86Base<Machine>::Traits::Assembler>();
  assert(this->getSrcSize() == 2);
  const auto SrcVar = llvm::cast<Variable>(this->getSrc(0));
  const auto DestMem =
      llvm::cast<typename InstX86Base<Machine>::Traits::X86OperandMem>(
          this->getSrc(1));
  assert(DestMem->getSegmentRegister() ==
         InstX86Base<Machine>::Traits::X86OperandMem::DefaultSegment);
  assert(SrcVar->hasReg());
  Asm->movups(DestMem->toAsmAddress(Asm),
              InstX86Base<Machine>::Traits::RegisterSet::getEncodedXmm(
                  SrcVar->getRegNum()));
}

template <class Machine>
void InstX86StoreP<Machine>::dump(const Cfg *Func) const {
  if (!BuildDefs::dump())
    return;
  Ostream &Str = Func->getContext()->getStrDump();
  Str << "storep." << this->getSrc(0)->getType() << " ";
  this->getSrc(1)->dump(Func);
  Str << ", ";
  this->getSrc(0)->dump(Func);
}

template <class Machine>
void InstX86StoreQ<Machine>::emit(const Cfg *Func) const {
  if (!BuildDefs::dump())
    return;
  Ostream &Str = Func->getContext()->getStrEmit();
  assert(this->getSrcSize() == 2);
  assert(this->getSrc(1)->getType() == IceType_i64 ||
         this->getSrc(1)->getType() == IceType_f64);
  Str << "\tmovq\t";
  this->getSrc(0)->emit(Func);
  Str << ", ";
  this->getSrc(1)->emit(Func);
}

template <class Machine>
void InstX86StoreQ<Machine>::emitIAS(const Cfg *Func) const {
  typename InstX86Base<Machine>::Traits::Assembler *Asm =
      Func->getAssembler<typename InstX86Base<Machine>::Traits::Assembler>();
  assert(this->getSrcSize() == 2);
  const auto SrcVar = llvm::cast<Variable>(this->getSrc(0));
  const auto DestMem =
      llvm::cast<typename InstX86Base<Machine>::Traits::X86OperandMem>(
          this->getSrc(1));
  assert(DestMem->getSegmentRegister() ==
         InstX86Base<Machine>::Traits::X86OperandMem::DefaultSegment);
  assert(SrcVar->hasReg());
  Asm->movq(DestMem->toAsmAddress(Asm),
            InstX86Base<Machine>::Traits::RegisterSet::getEncodedXmm(
                SrcVar->getRegNum()));
}

template <class Machine>
void InstX86StoreQ<Machine>::dump(const Cfg *Func) const {
  if (!BuildDefs::dump())
    return;
  Ostream &Str = Func->getContext()->getStrDump();
  Str << "storeq." << this->getSrc(0)->getType() << " ";
  this->getSrc(1)->dump(Func);
  Str << ", ";
  this->getSrc(0)->dump(Func);
}

template <class Machine> void InstX86Lea<Machine>::emit(const Cfg *Func) const {
  if (!BuildDefs::dump())
    return;
  Ostream &Str = Func->getContext()->getStrEmit();
  assert(this->getSrcSize() == 1);
  assert(this->getDest()->hasReg());
  Str << "\tleal\t";
  Operand *Src0 = this->getSrc(0);
  if (const auto Src0Var = llvm::dyn_cast<Variable>(Src0)) {
    Type Ty = Src0Var->getType();
    // lea on x86-32 doesn't accept mem128 operands, so cast VSrc0 to an
    // acceptable type.
    Src0Var->asType(isVectorType(Ty) ? IceType_i32 : Ty)->emit(Func);
  } else {
    Src0->emit(Func);
  }
  Str << ", ";
  this->getDest()->emit(Func);
}

template <class Machine> void InstX86Mov<Machine>::emit(const Cfg *Func) const {
  if (!BuildDefs::dump())
    return;
  Ostream &Str = Func->getContext()->getStrEmit();
  assert(this->getSrcSize() == 1);
  Operand *Src = this->getSrc(0);
  Type SrcTy = Src->getType();
  Type DestTy = this->getDest()->getType();
  Str << "\tmov"
      << (!isScalarFloatingType(DestTy)
              ? this->getWidthString(SrcTy)
              : InstX86Base<Machine>::Traits::TypeAttributes[DestTy].SdSsString)
      << "\t";
  // For an integer truncation operation, src is wider than dest.
  // Ideally, we use a mov instruction whose data width matches the
  // narrower dest.  This is a problem if e.g. src is a register like
  // esi or si where there is no 8-bit version of the register.  To be
  // safe, we instead widen the dest to match src.  This works even
  // for stack-allocated dest variables because typeWidthOnStack()
  // pads to a 4-byte boundary even if only a lower portion is used.
  // TODO: This assert disallows usages such as copying a floating point
  // value between a vector and a scalar (which movss is used for).
  // Clean this up.
  assert(Func->getTarget()->typeWidthInBytesOnStack(DestTy) ==
         Func->getTarget()->typeWidthInBytesOnStack(SrcTy));
  Src->emit(Func);
  Str << ", ";
  this->getDest()->asType(SrcTy)->emit(Func);
}

template <class Machine>
void InstX86Mov<Machine>::emitIAS(const Cfg *Func) const {
  assert(this->getSrcSize() == 1);
  const Variable *Dest = this->getDest();
  const Operand *Src = this->getSrc(0);
  Type DestTy = Dest->getType();
  Type SrcTy = Src->getType();
  // Mov can be used for GPRs or XMM registers. Also, the type does not
  // necessarily match (Mov can be used for bitcasts). However, when
  // the type does not match, one of the operands must be a register.
  // Thus, the strategy is to find out if Src or Dest are a register,
  // then use that register's type to decide on which emitter set to use.
  // The emitter set will include reg-reg movs, but that case should
  // be unused when the types don't match.
  static const typename InstX86Base<Machine>::Traits::Assembler::XmmEmitterRegOp
      XmmRegEmitter = {&InstX86Base<Machine>::Traits::Assembler::movss,
                       &InstX86Base<Machine>::Traits::Assembler::movss};
  static const typename InstX86Base<Machine>::Traits::Assembler::GPREmitterRegOp
      GPRRegEmitter = {&InstX86Base<Machine>::Traits::Assembler::mov,
                       &InstX86Base<Machine>::Traits::Assembler::mov,
                       &InstX86Base<Machine>::Traits::Assembler::mov};
  static const typename InstX86Base<
      Machine>::Traits::Assembler::GPREmitterAddrOp GPRAddrEmitter = {
      &InstX86Base<Machine>::Traits::Assembler::mov,
      &InstX86Base<Machine>::Traits::Assembler::mov};
  // For an integer truncation operation, src is wider than dest.
  // Ideally, we use a mov instruction whose data width matches the
  // narrower dest.  This is a problem if e.g. src is a register like
  // esi or si where there is no 8-bit version of the register.  To be
  // safe, we instead widen the dest to match src.  This works even
  // for stack-allocated dest variables because typeWidthOnStack()
  // pads to a 4-byte boundary even if only a lower portion is used.
  // TODO: This assert disallows usages such as copying a floating point
  // value between a vector and a scalar (which movss is used for).
  // Clean this up.
  assert(
      Func->getTarget()->typeWidthInBytesOnStack(this->getDest()->getType()) ==
      Func->getTarget()->typeWidthInBytesOnStack(Src->getType()));
  if (Dest->hasReg()) {
    if (isScalarFloatingType(DestTy)) {
      emitIASRegOpTyXMM<Machine>(Func, DestTy, Dest, Src, XmmRegEmitter);
      return;
    } else {
      assert(isScalarIntegerType(DestTy));
      // Widen DestTy for truncation (see above note). We should only do this
      // when both Src and Dest are integer types.
      if (isScalarIntegerType(SrcTy)) {
        DestTy = SrcTy;
      }
      emitIASRegOpTyGPR<Machine>(Func, DestTy, Dest, Src, GPRRegEmitter);
      return;
    }
  } else {
    // Dest must be Stack and Src *could* be a register. Use Src's type
    // to decide on the emitters.
    typename InstX86Base<Machine>::Traits::Address StackAddr(
        static_cast<typename InstX86Base<Machine>::Traits::TargetLowering *>(
            Func->getTarget())
            ->stackVarToAsmOperand(Dest));
    if (isScalarFloatingType(SrcTy)) {
      // Src must be a register.
      const auto SrcVar = llvm::cast<Variable>(Src);
      assert(SrcVar->hasReg());
      typename InstX86Base<Machine>::Traits::Assembler *Asm =
          Func->getAssembler<
              typename InstX86Base<Machine>::Traits::Assembler>();
      Asm->movss(SrcTy, StackAddr,
                 InstX86Base<Machine>::Traits::RegisterSet::getEncodedXmm(
                     SrcVar->getRegNum()));
      return;
    } else {
      // Src can be a register or immediate.
      assert(isScalarIntegerType(SrcTy));
      emitIASAddrOpTyGPR<Machine>(Func, SrcTy, StackAddr, Src, GPRAddrEmitter);
      return;
    }
    return;
  }
}

template <class Machine>
void InstX86Movd<Machine>::emitIAS(const Cfg *Func) const {
  typename InstX86Base<Machine>::Traits::Assembler *Asm =
      Func->getAssembler<typename InstX86Base<Machine>::Traits::Assembler>();
  assert(this->getSrcSize() == 1);
  const Variable *Dest = this->getDest();
  const auto SrcVar = llvm::cast<Variable>(this->getSrc(0));
  // For insert/extract element (one of Src/Dest is an Xmm vector and
  // the other is an int type).
  if (SrcVar->getType() == IceType_i32) {
    assert(isVectorType(Dest->getType()));
    assert(Dest->hasReg());
    typename InstX86Base<Machine>::Traits::RegisterSet::XmmRegister DestReg =
        InstX86Base<Machine>::Traits::RegisterSet::getEncodedXmm(
            Dest->getRegNum());
    if (SrcVar->hasReg()) {
      Asm->movd(DestReg,
                InstX86Base<Machine>::Traits::RegisterSet::getEncodedGPR(
                    SrcVar->getRegNum()));
    } else {
      typename InstX86Base<Machine>::Traits::Address StackAddr(
          static_cast<typename InstX86Base<Machine>::Traits::TargetLowering *>(
              Func->getTarget())
              ->stackVarToAsmOperand(SrcVar));
      Asm->movd(DestReg, StackAddr);
    }
  } else {
    assert(isVectorType(SrcVar->getType()));
    assert(SrcVar->hasReg());
    assert(Dest->getType() == IceType_i32);
    typename InstX86Base<Machine>::Traits::RegisterSet::XmmRegister SrcReg =
        InstX86Base<Machine>::Traits::RegisterSet::getEncodedXmm(
            SrcVar->getRegNum());
    if (Dest->hasReg()) {
      Asm->movd(InstX86Base<Machine>::Traits::RegisterSet::getEncodedGPR(
                    Dest->getRegNum()),
                SrcReg);
    } else {
      typename InstX86Base<Machine>::Traits::Address StackAddr(
          static_cast<typename InstX86Base<Machine>::Traits::TargetLowering *>(
              Func->getTarget())
              ->stackVarToAsmOperand(Dest));
      Asm->movd(StackAddr, SrcReg);
    }
  }
}

template <class Machine>
void InstX86Movp<Machine>::emit(const Cfg *Func) const {
  if (!BuildDefs::dump())
    return;
  // TODO(wala,stichnot): movups works with all vector operands, but
  // there exist other instructions (movaps, movdqa, movdqu) that may
  // perform better, depending on the data type and alignment of the
  // operands.
  Ostream &Str = Func->getContext()->getStrEmit();
  assert(this->getSrcSize() == 1);
  Str << "\tmovups\t";
  this->getSrc(0)->emit(Func);
  Str << ", ";
  this->getDest()->emit(Func);
}

template <class Machine>
void InstX86Movp<Machine>::emitIAS(const Cfg *Func) const {
  assert(this->getSrcSize() == 1);
  assert(isVectorType(this->getDest()->getType()));
  const Variable *Dest = this->getDest();
  const Operand *Src = this->getSrc(0);
  static const typename InstX86Base<
      Machine>::Traits::Assembler::XmmEmitterMovOps Emitter = {
      &InstX86Base<Machine>::Traits::Assembler::movups,
      &InstX86Base<Machine>::Traits::Assembler::movups,
      &InstX86Base<Machine>::Traits::Assembler::movups};
  emitIASMovlikeXMM<Machine>(Func, Dest, Src, Emitter);
}

template <class Machine>
void InstX86Movq<Machine>::emit(const Cfg *Func) const {
  if (!BuildDefs::dump())
    return;
  Ostream &Str = Func->getContext()->getStrEmit();
  assert(this->getSrcSize() == 1);
  assert(this->getDest()->getType() == IceType_i64 ||
         this->getDest()->getType() == IceType_f64);
  Str << "\tmovq\t";
  this->getSrc(0)->emit(Func);
  Str << ", ";
  this->getDest()->emit(Func);
}

template <class Machine>
void InstX86Movq<Machine>::emitIAS(const Cfg *Func) const {
  assert(this->getSrcSize() == 1);
  assert(this->getDest()->getType() == IceType_i64 ||
         this->getDest()->getType() == IceType_f64);
  const Variable *Dest = this->getDest();
  const Operand *Src = this->getSrc(0);
  static const typename InstX86Base<
      Machine>::Traits::Assembler::XmmEmitterMovOps Emitter = {
      &InstX86Base<Machine>::Traits::Assembler::movq,
      &InstX86Base<Machine>::Traits::Assembler::movq,
      &InstX86Base<Machine>::Traits::Assembler::movq};
  emitIASMovlikeXMM<Machine>(Func, Dest, Src, Emitter);
}

template <class Machine>
void InstX86MovssRegs<Machine>::emitIAS(const Cfg *Func) const {
  // This is Binop variant is only intended to be used for reg-reg moves
  // where part of the Dest register is untouched.
  assert(this->getSrcSize() == 2);
  const Variable *Dest = this->getDest();
  assert(Dest == this->getSrc(0));
  const auto SrcVar = llvm::cast<Variable>(this->getSrc(1));
  assert(Dest->hasReg() && SrcVar->hasReg());
  typename InstX86Base<Machine>::Traits::Assembler *Asm =
      Func->getAssembler<typename InstX86Base<Machine>::Traits::Assembler>();
  Asm->movss(IceType_f32,
             InstX86Base<Machine>::Traits::RegisterSet::getEncodedXmm(
                 Dest->getRegNum()),
             InstX86Base<Machine>::Traits::RegisterSet::getEncodedXmm(
                 SrcVar->getRegNum()));
}

template <class Machine>
void InstX86Movsx<Machine>::emitIAS(const Cfg *Func) const {
  assert(this->getSrcSize() == 1);
  const Variable *Dest = this->getDest();
  const Operand *Src = this->getSrc(0);
  // Dest must be a > 8-bit register, but Src can be 8-bit. In practice
  // we just use the full register for Dest to avoid having an
  // OperandSizeOverride prefix. It also allows us to only dispatch on SrcTy.
  Type SrcTy = Src->getType();
  assert(typeWidthInBytes(Dest->getType()) > 1);
  assert(typeWidthInBytes(Dest->getType()) > typeWidthInBytes(SrcTy));
  emitIASRegOpTyGPR<Machine, false, true>(Func, SrcTy, Dest, Src,
                                          this->Emitter);
}

template <class Machine>
void InstX86Movzx<Machine>::emitIAS(const Cfg *Func) const {
  assert(this->getSrcSize() == 1);
  const Variable *Dest = this->getDest();
  const Operand *Src = this->getSrc(0);
  Type SrcTy = Src->getType();
  assert(typeWidthInBytes(Dest->getType()) > 1);
  assert(typeWidthInBytes(Dest->getType()) > typeWidthInBytes(SrcTy));
  emitIASRegOpTyGPR<Machine, false, true>(Func, SrcTy, Dest, Src,
                                          this->Emitter);
}

template <class Machine> void InstX86Nop<Machine>::emit(const Cfg *Func) const {
  if (!BuildDefs::dump())
    return;
  Ostream &Str = Func->getContext()->getStrEmit();
  // TODO: Emit the right code for each variant.
  Str << "\tnop\t# variant = " << Variant;
}

template <class Machine>
void InstX86Nop<Machine>::emitIAS(const Cfg *Func) const {
  typename InstX86Base<Machine>::Traits::Assembler *Asm =
      Func->getAssembler<typename InstX86Base<Machine>::Traits::Assembler>();
  // TODO: Emit the right code for the variant.
  Asm->nop();
}

template <class Machine> void InstX86Nop<Machine>::dump(const Cfg *Func) const {
  if (!BuildDefs::dump())
    return;
  Ostream &Str = Func->getContext()->getStrDump();
  Str << "nop (variant = " << Variant << ")";
}

template <class Machine> void InstX86Fld<Machine>::emit(const Cfg *Func) const {
  if (!BuildDefs::dump())
    return;
  Ostream &Str = Func->getContext()->getStrEmit();
  assert(this->getSrcSize() == 1);
  Type Ty = this->getSrc(0)->getType();
  SizeT Width = typeWidthInBytes(Ty);
  const auto Var = llvm::dyn_cast<Variable>(this->getSrc(0));
  if (Var && Var->hasReg()) {
    // This is a physical xmm register, so we need to spill it to a
    // temporary stack slot.
    Str << "\tsubl\t$" << Width << ", %esp"
        << "\n";
    Str << "\tmov"
        << InstX86Base<Machine>::Traits::TypeAttributes[Ty].SdSsString << "\t";
    Var->emit(Func);
    Str << ", (%esp)\n";
    Str << "\tfld" << this->getFldString(Ty) << "\t"
        << "(%esp)\n";
    Str << "\taddl\t$" << Width << ", %esp";
    return;
  }
  Str << "\tfld" << this->getFldString(Ty) << "\t";
  this->getSrc(0)->emit(Func);
}

template <class Machine>
void InstX86Fld<Machine>::emitIAS(const Cfg *Func) const {
  typename InstX86Base<Machine>::Traits::Assembler *Asm =
      Func->getAssembler<typename InstX86Base<Machine>::Traits::Assembler>();
  assert(this->getSrcSize() == 1);
  const Operand *Src = this->getSrc(0);
  Type Ty = Src->getType();
  if (const auto Var = llvm::dyn_cast<Variable>(Src)) {
    if (Var->hasReg()) {
      // This is a physical xmm register, so we need to spill it to a
      // temporary stack slot.
      Immediate Width(typeWidthInBytes(Ty));
      Asm->sub(IceType_i32,
               InstX86Base<Machine>::Traits::RegisterSet::Encoded_Reg_esp,
               Width);
      typename InstX86Base<Machine>::Traits::Address StackSlot =
          typename InstX86Base<Machine>::Traits::Address(
              InstX86Base<Machine>::Traits::RegisterSet::Encoded_Reg_esp, 0);
      Asm->movss(Ty, StackSlot,
                 InstX86Base<Machine>::Traits::RegisterSet::getEncodedXmm(
                     Var->getRegNum()));
      Asm->fld(Ty, StackSlot);
      Asm->add(IceType_i32,
               InstX86Base<Machine>::Traits::RegisterSet::Encoded_Reg_esp,
               Width);
    } else {
      typename InstX86Base<Machine>::Traits::Address StackAddr(
          static_cast<typename InstX86Base<Machine>::Traits::TargetLowering *>(
              Func->getTarget())
              ->stackVarToAsmOperand(Var));
      Asm->fld(Ty, StackAddr);
    }
  } else if (const auto Mem = llvm::dyn_cast<
                 typename InstX86Base<Machine>::Traits::X86OperandMem>(Src)) {
    assert(Mem->getSegmentRegister() ==
           InstX86Base<Machine>::Traits::X86OperandMem::DefaultSegment);
    Asm->fld(Ty, Mem->toAsmAddress(Asm));
  } else if (const auto Imm = llvm::dyn_cast<Constant>(Src)) {
    Asm->fld(Ty, InstX86Base<Machine>::Traits::Address::ofConstPool(Asm, Imm));
  } else {
    llvm_unreachable("Unexpected operand type");
  }
}

template <class Machine> void InstX86Fld<Machine>::dump(const Cfg *Func) const {
  if (!BuildDefs::dump())
    return;
  Ostream &Str = Func->getContext()->getStrDump();
  Str << "fld." << this->getSrc(0)->getType() << " ";
  this->dumpSources(Func);
}

template <class Machine>
void InstX86Fstp<Machine>::emit(const Cfg *Func) const {
  if (!BuildDefs::dump())
    return;
  Ostream &Str = Func->getContext()->getStrEmit();
  assert(this->getSrcSize() == 0);
  // TODO(jvoung,stichnot): Utilize this by setting Dest to nullptr to
  // "partially" delete the fstp if the Dest is unused.
  // Even if Dest is unused, the fstp should be kept for the SideEffects
  // of popping the stack.
  if (!this->getDest()) {
    Str << "\tfstp\tst(0)";
    return;
  }
  Type Ty = this->getDest()->getType();
  size_t Width = typeWidthInBytes(Ty);
  if (!this->getDest()->hasReg()) {
    Str << "\tfstp" << this->getFldString(Ty) << "\t";
    this->getDest()->emit(Func);
    return;
  }
  // Dest is a physical (xmm) register, so st(0) needs to go through
  // memory.  Hack this by creating a temporary stack slot, spilling
  // st(0) there, loading it into the xmm register, and deallocating
  // the stack slot.
  Str << "\tsubl\t$" << Width << ", %esp\n";
  Str << "\tfstp" << this->getFldString(Ty) << "\t"
      << "(%esp)\n";
  Str << "\tmov" << InstX86Base<Machine>::Traits::TypeAttributes[Ty].SdSsString
      << "\t"
      << "(%esp), ";
  this->getDest()->emit(Func);
  Str << "\n";
  Str << "\taddl\t$" << Width << ", %esp";
}

template <class Machine>
void InstX86Fstp<Machine>::emitIAS(const Cfg *Func) const {
  typename InstX86Base<Machine>::Traits::Assembler *Asm =
      Func->getAssembler<typename InstX86Base<Machine>::Traits::Assembler>();
  assert(this->getSrcSize() == 0);
  const Variable *Dest = this->getDest();
  // TODO(jvoung,stichnot): Utilize this by setting Dest to nullptr to
  // "partially" delete the fstp if the Dest is unused.
  // Even if Dest is unused, the fstp should be kept for the SideEffects
  // of popping the stack.
  if (!Dest) {
    Asm->fstp(InstX86Base<Machine>::Traits::RegisterSet::getEncodedSTReg(0));
    return;
  }
  Type Ty = Dest->getType();
  if (!Dest->hasReg()) {
    typename InstX86Base<Machine>::Traits::Address StackAddr(
        static_cast<typename InstX86Base<Machine>::Traits::TargetLowering *>(
            Func->getTarget())
            ->stackVarToAsmOperand(Dest));
    Asm->fstp(Ty, StackAddr);
  } else {
    // Dest is a physical (xmm) register, so st(0) needs to go through
    // memory.  Hack this by creating a temporary stack slot, spilling
    // st(0) there, loading it into the xmm register, and deallocating
    // the stack slot.
    Immediate Width(typeWidthInBytes(Ty));
    Asm->sub(IceType_i32,
             InstX86Base<Machine>::Traits::RegisterSet::Encoded_Reg_esp, Width);
    typename InstX86Base<Machine>::Traits::Address StackSlot =
        typename InstX86Base<Machine>::Traits::Address(
            InstX86Base<Machine>::Traits::RegisterSet::Encoded_Reg_esp, 0);
    Asm->fstp(Ty, StackSlot);
    Asm->movss(Ty, InstX86Base<Machine>::Traits::RegisterSet::getEncodedXmm(
                       Dest->getRegNum()),
               StackSlot);
    Asm->add(IceType_i32,
             InstX86Base<Machine>::Traits::RegisterSet::Encoded_Reg_esp, Width);
  }
}

template <class Machine>
void InstX86Fstp<Machine>::dump(const Cfg *Func) const {
  if (!BuildDefs::dump())
    return;
  Ostream &Str = Func->getContext()->getStrDump();
  this->dumpDest(Func);
  Str << " = fstp." << this->getDest()->getType() << ", st(0)";
}

template <class Machine>
void InstX86Pcmpeq<Machine>::emit(const Cfg *Func) const {
  if (!BuildDefs::dump())
    return;
  char buf[30];
  snprintf(
      buf, llvm::array_lengthof(buf), "pcmpeq%s",
      InstX86Base<Machine>::Traits::TypeAttributes[this->getDest()->getType()]
          .PackString);
  this->emitTwoAddress(buf, this, Func);
}

template <class Machine>
void InstX86Pcmpgt<Machine>::emit(const Cfg *Func) const {
  if (!BuildDefs::dump())
    return;
  char buf[30];
  snprintf(
      buf, llvm::array_lengthof(buf), "pcmpgt%s",
      InstX86Base<Machine>::Traits::TypeAttributes[this->getDest()->getType()]
          .PackString);
  this->emitTwoAddress(buf, this, Func);
}

template <class Machine>
void InstX86Pextr<Machine>::emit(const Cfg *Func) const {
  if (!BuildDefs::dump())
    return;
  Ostream &Str = Func->getContext()->getStrEmit();
  assert(this->getSrcSize() == 2);
  // pextrb and pextrd are SSE4.1 instructions.
  assert(this->getSrc(0)->getType() == IceType_v8i16 ||
         this->getSrc(0)->getType() == IceType_v8i1 ||
         static_cast<typename InstX86Base<Machine>::Traits::TargetLowering *>(
             Func->getTarget())
                 ->getInstructionSet() >= InstX86Base<Machine>::Traits::SSE4_1);
  Str << "\t" << this->Opcode
      << InstX86Base<Machine>::Traits::TypeAttributes[this->getSrc(0)
                                                          ->getType()]
             .PackString << "\t";
  this->getSrc(1)->emit(Func);
  Str << ", ";
  this->getSrc(0)->emit(Func);
  Str << ", ";
  Variable *Dest = this->getDest();
  // pextrw must take a register dest. There is an SSE4.1 version that takes
  // a memory dest, but we aren't using it. For uniformity, just restrict
  // them all to have a register dest for now.
  assert(Dest->hasReg());
  Dest->asType(IceType_i32)->emit(Func);
}

template <class Machine>
void InstX86Pextr<Machine>::emitIAS(const Cfg *Func) const {
  assert(this->getSrcSize() == 2);
  // pextrb and pextrd are SSE4.1 instructions.
  const Variable *Dest = this->getDest();
  Type DispatchTy = Dest->getType();
  assert(DispatchTy == IceType_i16 ||
         static_cast<typename InstX86Base<Machine>::Traits::TargetLowering *>(
             Func->getTarget())
                 ->getInstructionSet() >= InstX86Base<Machine>::Traits::SSE4_1);
  // pextrw must take a register dest. There is an SSE4.1 version that takes
  // a memory dest, but we aren't using it. For uniformity, just restrict
  // them all to have a register dest for now.
  assert(Dest->hasReg());
  // pextrw's Src(0) must be a register (both SSE4.1 and SSE2).
  assert(llvm::cast<Variable>(this->getSrc(0))->hasReg());
  static const typename InstX86Base<Machine>::Traits::Assembler::
      template ThreeOpImmEmitter<
          typename InstX86Base<Machine>::Traits::RegisterSet::GPRRegister,
          typename InstX86Base<Machine>::Traits::RegisterSet::XmmRegister>
          Emitter = {&InstX86Base<Machine>::Traits::Assembler::pextr, nullptr};
  emitIASThreeOpImmOps<
      Machine, typename InstX86Base<Machine>::Traits::RegisterSet::GPRRegister,
      typename InstX86Base<Machine>::Traits::RegisterSet::XmmRegister,
      InstX86Base<Machine>::Traits::RegisterSet::getEncodedGPR,
      InstX86Base<Machine>::Traits::RegisterSet::getEncodedXmm>(
      Func, DispatchTy, Dest, this->getSrc(0), this->getSrc(1), Emitter);
}

template <class Machine>
void InstX86Pinsr<Machine>::emit(const Cfg *Func) const {
  if (!BuildDefs::dump())
    return;
  Ostream &Str = Func->getContext()->getStrEmit();
  assert(this->getSrcSize() == 3);
  // pinsrb and pinsrd are SSE4.1 instructions.
  assert(this->getDest()->getType() == IceType_v8i16 ||
         this->getDest()->getType() == IceType_v8i1 ||
         static_cast<typename InstX86Base<Machine>::Traits::TargetLowering *>(
             Func->getTarget())
                 ->getInstructionSet() >= InstX86Base<Machine>::Traits::SSE4_1);
  Str << "\t" << this->Opcode
      << InstX86Base<
             Machine>::Traits::TypeAttributes[this->getDest()->getType()]
             .PackString << "\t";
  this->getSrc(2)->emit(Func);
  Str << ", ";
  Operand *Src1 = this->getSrc(1);
  if (const auto Src1Var = llvm::dyn_cast<Variable>(Src1)) {
    // If src1 is a register, it should always be r32.
    if (Src1Var->hasReg()) {
      Src1Var->asType(IceType_i32)->emit(Func);
    } else {
      Src1Var->emit(Func);
    }
  } else {
    Src1->emit(Func);
  }
  Str << ", ";
  this->getDest()->emit(Func);
}

template <class Machine>
void InstX86Pinsr<Machine>::emitIAS(const Cfg *Func) const {
  assert(this->getSrcSize() == 3);
  assert(this->getDest() == this->getSrc(0));
  // pinsrb and pinsrd are SSE4.1 instructions.
  const Operand *Src0 = this->getSrc(1);
  Type DispatchTy = Src0->getType();
  assert(DispatchTy == IceType_i16 ||
         static_cast<typename InstX86Base<Machine>::Traits::TargetLowering *>(
             Func->getTarget())
                 ->getInstructionSet() >= InstX86Base<Machine>::Traits::SSE4_1);
  // If src1 is a register, it should always be r32 (this should fall out
  // from the encodings for ByteRegs overlapping the encodings for r32),
  // but we have to trust the regalloc to not choose "ah", where it
  // doesn't overlap.
  static const typename InstX86Base<Machine>::Traits::Assembler::
      template ThreeOpImmEmitter<
          typename InstX86Base<Machine>::Traits::RegisterSet::XmmRegister,
          typename InstX86Base<Machine>::Traits::RegisterSet::GPRRegister>
          Emitter = {&InstX86Base<Machine>::Traits::Assembler::pinsr,
                     &InstX86Base<Machine>::Traits::Assembler::pinsr};
  emitIASThreeOpImmOps<
      Machine, typename InstX86Base<Machine>::Traits::RegisterSet::XmmRegister,
      typename InstX86Base<Machine>::Traits::RegisterSet::GPRRegister,
      InstX86Base<Machine>::Traits::RegisterSet::getEncodedXmm,
      InstX86Base<Machine>::Traits::RegisterSet::getEncodedGPR>(
      Func, DispatchTy, this->getDest(), Src0, this->getSrc(2), Emitter);
}

template <class Machine>
void InstX86Pshufd<Machine>::emitIAS(const Cfg *Func) const {
  assert(this->getSrcSize() == 2);
  const Variable *Dest = this->getDest();
  Type Ty = Dest->getType();
  static const typename InstX86Base<Machine>::Traits::Assembler::
      template ThreeOpImmEmitter<
          typename InstX86Base<Machine>::Traits::RegisterSet::XmmRegister,
          typename InstX86Base<Machine>::Traits::RegisterSet::XmmRegister>
          Emitter = {&InstX86Base<Machine>::Traits::Assembler::pshufd,
                     &InstX86Base<Machine>::Traits::Assembler::pshufd};
  emitIASThreeOpImmOps<
      Machine, typename InstX86Base<Machine>::Traits::RegisterSet::XmmRegister,
      typename InstX86Base<Machine>::Traits::RegisterSet::XmmRegister,
      InstX86Base<Machine>::Traits::RegisterSet::getEncodedXmm,
      InstX86Base<Machine>::Traits::RegisterSet::getEncodedXmm>(
      Func, Ty, Dest, this->getSrc(0), this->getSrc(1), Emitter);
}

template <class Machine>
void InstX86Shufps<Machine>::emitIAS(const Cfg *Func) const {
  assert(this->getSrcSize() == 3);
  const Variable *Dest = this->getDest();
  assert(Dest == this->getSrc(0));
  Type Ty = Dest->getType();
  static const typename InstX86Base<Machine>::Traits::Assembler::
      template ThreeOpImmEmitter<
          typename InstX86Base<Machine>::Traits::RegisterSet::XmmRegister,
          typename InstX86Base<Machine>::Traits::RegisterSet::XmmRegister>
          Emitter = {&InstX86Base<Machine>::Traits::Assembler::shufps,
                     &InstX86Base<Machine>::Traits::Assembler::shufps};
  emitIASThreeOpImmOps<
      Machine, typename InstX86Base<Machine>::Traits::RegisterSet::XmmRegister,
      typename InstX86Base<Machine>::Traits::RegisterSet::XmmRegister,
      InstX86Base<Machine>::Traits::RegisterSet::getEncodedXmm,
      InstX86Base<Machine>::Traits::RegisterSet::getEncodedXmm>(
      Func, Ty, Dest, this->getSrc(1), this->getSrc(2), Emitter);
}

template <class Machine> void InstX86Pop<Machine>::emit(const Cfg *Func) const {
  if (!BuildDefs::dump())
    return;
  Ostream &Str = Func->getContext()->getStrEmit();
  assert(this->getSrcSize() == 0);
  Str << "\tpop\t";
  this->getDest()->emit(Func);
}

template <class Machine>
void InstX86Pop<Machine>::emitIAS(const Cfg *Func) const {
  assert(this->getSrcSize() == 0);
  typename InstX86Base<Machine>::Traits::Assembler *Asm =
      Func->getAssembler<typename InstX86Base<Machine>::Traits::Assembler>();
  if (this->getDest()->hasReg()) {
    Asm->popl(InstX86Base<Machine>::Traits::RegisterSet::getEncodedGPR(
        this->getDest()->getRegNum()));
  } else {
    Asm->popl(
        static_cast<typename InstX86Base<Machine>::Traits::TargetLowering *>(
            Func->getTarget())
            ->stackVarToAsmOperand(this->getDest()));
  }
}

template <class Machine> void InstX86Pop<Machine>::dump(const Cfg *Func) const {
  if (!BuildDefs::dump())
    return;
  Ostream &Str = Func->getContext()->getStrDump();
  this->dumpDest(Func);
  Str << " = pop." << this->getDest()->getType() << " ";
}

template <class Machine>
void InstX86AdjustStack<Machine>::emit(const Cfg *Func) const {
  if (!BuildDefs::dump())
    return;
  Ostream &Str = Func->getContext()->getStrEmit();
  Str << "\tsubl\t$" << Amount << ", %esp";
  Func->getTarget()->updateStackAdjustment(Amount);
}

template <class Machine>
void InstX86AdjustStack<Machine>::emitIAS(const Cfg *Func) const {
  typename InstX86Base<Machine>::Traits::Assembler *Asm =
      Func->getAssembler<typename InstX86Base<Machine>::Traits::Assembler>();
  Asm->sub(IceType_i32,
           InstX86Base<Machine>::Traits::RegisterSet::Encoded_Reg_esp,
           Immediate(Amount));
  Func->getTarget()->updateStackAdjustment(Amount);
}

template <class Machine>
void InstX86AdjustStack<Machine>::dump(const Cfg *Func) const {
  if (!BuildDefs::dump())
    return;
  Ostream &Str = Func->getContext()->getStrDump();
  Str << "esp = sub.i32 esp, " << Amount;
}

template <class Machine>
void InstX86Push<Machine>::emit(const Cfg *Func) const {
  if (!BuildDefs::dump())
    return;
  Ostream &Str = Func->getContext()->getStrEmit();
  assert(this->getSrcSize() == 1);
  // Push is currently only used for saving GPRs.
  const auto Var = llvm::cast<Variable>(this->getSrc(0));
  assert(Var->hasReg());
  Str << "\tpush\t";
  Var->emit(Func);
}

template <class Machine>
void InstX86Push<Machine>::emitIAS(const Cfg *Func) const {
  assert(this->getSrcSize() == 1);
  // Push is currently only used for saving GPRs.
  const auto Var = llvm::cast<Variable>(this->getSrc(0));
  assert(Var->hasReg());
  typename InstX86Base<Machine>::Traits::Assembler *Asm =
      Func->getAssembler<typename InstX86Base<Machine>::Traits::Assembler>();
  Asm->pushl(InstX86Base<Machine>::Traits::RegisterSet::getEncodedGPR(
      Var->getRegNum()));
}

template <class Machine>
void InstX86Push<Machine>::dump(const Cfg *Func) const {
  if (!BuildDefs::dump())
    return;
  Ostream &Str = Func->getContext()->getStrDump();
  Str << "push." << this->getSrc(0)->getType() << " ";
  this->dumpSources(Func);
}

template <class Machine>
void InstX86Psll<Machine>::emit(const Cfg *Func) const {
  if (!BuildDefs::dump())
    return;
  assert(this->getDest()->getType() == IceType_v8i16 ||
         this->getDest()->getType() == IceType_v8i1 ||
         this->getDest()->getType() == IceType_v4i32 ||
         this->getDest()->getType() == IceType_v4i1);
  char buf[30];
  snprintf(
      buf, llvm::array_lengthof(buf), "psll%s",
      InstX86Base<Machine>::Traits::TypeAttributes[this->getDest()->getType()]
          .PackString);
  this->emitTwoAddress(buf, this, Func);
}

template <class Machine>
void InstX86Psra<Machine>::emit(const Cfg *Func) const {
  if (!BuildDefs::dump())
    return;
  assert(this->getDest()->getType() == IceType_v8i16 ||
         this->getDest()->getType() == IceType_v8i1 ||
         this->getDest()->getType() == IceType_v4i32 ||
         this->getDest()->getType() == IceType_v4i1);
  char buf[30];
  snprintf(
      buf, llvm::array_lengthof(buf), "psra%s",
      InstX86Base<Machine>::Traits::TypeAttributes[this->getDest()->getType()]
          .PackString);
  this->emitTwoAddress(buf, this, Func);
}

template <class Machine>
void InstX86Psrl<Machine>::emit(const Cfg *Func) const {
  if (!BuildDefs::dump())
    return;
  char buf[30];
  snprintf(
      buf, llvm::array_lengthof(buf), "psrl%s",
      InstX86Base<Machine>::Traits::TypeAttributes[this->getDest()->getType()]
          .PackString);
  this->emitTwoAddress(buf, this, Func);
}

template <class Machine> void InstX86Ret<Machine>::emit(const Cfg *Func) const {
  if (!BuildDefs::dump())
    return;
  Ostream &Str = Func->getContext()->getStrEmit();
  Str << "\tret";
}

template <class Machine>
void InstX86Ret<Machine>::emitIAS(const Cfg *Func) const {
  typename InstX86Base<Machine>::Traits::Assembler *Asm =
      Func->getAssembler<typename InstX86Base<Machine>::Traits::Assembler>();
  Asm->ret();
}

template <class Machine> void InstX86Ret<Machine>::dump(const Cfg *Func) const {
  if (!BuildDefs::dump())
    return;
  Ostream &Str = Func->getContext()->getStrDump();
  Type Ty =
      (this->getSrcSize() == 0 ? IceType_void : this->getSrc(0)->getType());
  Str << "ret." << Ty << " ";
  this->dumpSources(Func);
}

template <class Machine>
void InstX86Setcc<Machine>::emit(const Cfg *Func) const {
  if (!BuildDefs::dump())
    return;
  Ostream &Str = Func->getContext()->getStrEmit();
  Str << "\tset"
      << InstX86Base<Machine>::Traits::InstBrAttributes[Condition].DisplayString
      << "\t";
  this->Dest->emit(Func);
}

template <class Machine>
void InstX86Setcc<Machine>::emitIAS(const Cfg *Func) const {
  assert(Condition != InstX86Base<Machine>::Traits::Cond::Br_None);
  assert(this->getDest()->getType() == IceType_i1);
  assert(this->getSrcSize() == 0);
  typename InstX86Base<Machine>::Traits::Assembler *Asm =
      Func->getAssembler<typename InstX86Base<Machine>::Traits::Assembler>();
  if (this->getDest()->hasReg())
    Asm->setcc(Condition,
               InstX86Base<Machine>::Traits::RegisterSet::getEncodedByteReg(
                   this->getDest()->getRegNum()));
  else
    Asm->setcc(
        Condition,
        static_cast<typename InstX86Base<Machine>::Traits::TargetLowering *>(
            Func->getTarget())
            ->stackVarToAsmOperand(this->getDest()));
  return;
}

template <class Machine>
void InstX86Setcc<Machine>::dump(const Cfg *Func) const {
  if (!BuildDefs::dump())
    return;
  Ostream &Str = Func->getContext()->getStrDump();
  Str << "setcc."
      << InstX86Base<Machine>::Traits::InstBrAttributes[Condition].DisplayString
      << " ";
  this->dumpDest(Func);
}

template <class Machine>
void InstX86Xadd<Machine>::emit(const Cfg *Func) const {
  if (!BuildDefs::dump())
    return;
  Ostream &Str = Func->getContext()->getStrEmit();
  if (this->Locked) {
    Str << "\tlock";
  }
  Str << "\txadd" << this->getWidthString(this->getSrc(0)->getType()) << "\t";
  this->getSrc(1)->emit(Func);
  Str << ", ";
  this->getSrc(0)->emit(Func);
}

template <class Machine>
void InstX86Xadd<Machine>::emitIAS(const Cfg *Func) const {
  assert(this->getSrcSize() == 2);
  typename InstX86Base<Machine>::Traits::Assembler *Asm =
      Func->getAssembler<typename InstX86Base<Machine>::Traits::Assembler>();
  Type Ty = this->getSrc(0)->getType();
  const auto Mem =
      llvm::cast<typename InstX86Base<Machine>::Traits::X86OperandMem>(
          this->getSrc(0));
  assert(Mem->getSegmentRegister() ==
         InstX86Base<Machine>::Traits::X86OperandMem::DefaultSegment);
  const typename InstX86Base<Machine>::Traits::Address Addr =
      Mem->toAsmAddress(Asm);
  const auto VarReg = llvm::cast<Variable>(this->getSrc(1));
  assert(VarReg->hasReg());
  const typename InstX86Base<Machine>::Traits::RegisterSet::GPRRegister Reg =
      InstX86Base<Machine>::Traits::RegisterSet::getEncodedGPR(
          VarReg->getRegNum());
  Asm->xadd(Ty, Addr, Reg, this->Locked);
}

template <class Machine>
void InstX86Xadd<Machine>::dump(const Cfg *Func) const {
  if (!BuildDefs::dump())
    return;
  Ostream &Str = Func->getContext()->getStrDump();
  if (this->Locked) {
    Str << "lock ";
  }
  Type Ty = this->getSrc(0)->getType();
  Str << "xadd." << Ty << " ";
  this->dumpSources(Func);
}

template <class Machine>
void InstX86Xchg<Machine>::emit(const Cfg *Func) const {
  if (!BuildDefs::dump())
    return;
  Ostream &Str = Func->getContext()->getStrEmit();
  Str << "\txchg" << this->getWidthString(this->getSrc(0)->getType()) << "\t";
  this->getSrc(1)->emit(Func);
  Str << ", ";
  this->getSrc(0)->emit(Func);
}

template <class Machine>
void InstX86Xchg<Machine>::emitIAS(const Cfg *Func) const {
  assert(this->getSrcSize() == 2);
  typename InstX86Base<Machine>::Traits::Assembler *Asm =
      Func->getAssembler<typename InstX86Base<Machine>::Traits::Assembler>();
  Type Ty = this->getSrc(0)->getType();
  const auto Mem =
      llvm::cast<typename InstX86Base<Machine>::Traits::X86OperandMem>(
          this->getSrc(0));
  assert(Mem->getSegmentRegister() ==
         InstX86Base<Machine>::Traits::X86OperandMem::DefaultSegment);
  const typename InstX86Base<Machine>::Traits::Address Addr =
      Mem->toAsmAddress(Asm);
  const auto VarReg = llvm::cast<Variable>(this->getSrc(1));
  assert(VarReg->hasReg());
  const typename InstX86Base<Machine>::Traits::RegisterSet::GPRRegister Reg =
      InstX86Base<Machine>::Traits::RegisterSet::getEncodedGPR(
          VarReg->getRegNum());
  Asm->xchg(Ty, Addr, Reg);
}

template <class Machine>
void InstX86Xchg<Machine>::dump(const Cfg *Func) const {
  if (!BuildDefs::dump())
    return;
  Ostream &Str = Func->getContext()->getStrDump();
  Type Ty = this->getSrc(0)->getType();
  Str << "xchg." << Ty << " ";
  this->dumpSources(Func);
}

} // end of namespace X86Internal

} // end of namespace Ice

#endif // SUBZERO_SRC_ICEINSTX86BASEIMPL_H
