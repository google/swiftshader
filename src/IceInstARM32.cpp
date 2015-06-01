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

const struct InstARM32ShiftAttributes_ {
  const char *EmitString;
} InstARM32ShiftAttributes[] = {
#define X(tag, emit)                                                           \
  { emit }                                                                     \
  ,
    ICEINSTARM32SHIFT_TABLE
#undef X
};

const struct InstARM32CondAttributes_ {
  CondARM32::Cond Opposite;
  const char *EmitString;
} InstARM32CondAttributes[] = {
#define X(tag, encode, opp, emit)                                              \
  { CondARM32::opp, emit }                                                     \
  ,
    ICEINSTARM32COND_TABLE
#undef X
};

} // end of anonymous namespace

const char *InstARM32::getWidthString(Type Ty) {
  return TypeARM32Attributes[Ty].WidthString;
}

const char *InstARM32Pred::predString(CondARM32::Cond Pred) {
  return InstARM32CondAttributes[Pred].EmitString;
}

void InstARM32Pred::dumpOpcodePred(Ostream &Str, const char *Opcode,
                                   Type Ty) const {
  Str << Opcode << getPredicate() << "." << Ty;
}

CondARM32::Cond InstARM32::getOppositeCondition(CondARM32::Cond Cond) {
  return InstARM32CondAttributes[Cond].Opposite;
}

void InstARM32Pred::emitTwoAddr(const char *Opcode, const InstARM32Pred *Inst,
                                const Cfg *Func) {
  if (!ALLOW_DUMP)
    return;
  Ostream &Str = Func->getContext()->getStrEmit();
  assert(Inst->getSrcSize() == 2);
  Variable *Dest = Inst->getDest();
  assert(Dest == Inst->getSrc(0));
  Str << "\t" << Opcode << Inst->getPredicate() << "\t";
  Dest->emit(Func);
  Str << ", ";
  Inst->getSrc(1)->emit(Func);
}

void InstARM32Pred::emitThreeAddr(const char *Opcode, const InstARM32Pred *Inst,
                                  const Cfg *Func, bool SetFlags) {
  if (!ALLOW_DUMP)
    return;
  Ostream &Str = Func->getContext()->getStrEmit();
  assert(Inst->getSrcSize() == 2);
  Str << "\t" << Opcode << (SetFlags ? "s" : "") << Inst->getPredicate()
      << "\t";
  Inst->getDest()->emit(Func);
  Str << ", ";
  Inst->getSrc(0)->emit(Func);
  Str << ", ";
  Inst->getSrc(1)->emit(Func);
}

OperandARM32Mem::OperandARM32Mem(Cfg * /* Func */, Type Ty, Variable *Base,
                                 ConstantInteger32 *ImmOffset, AddrMode Mode)
    : OperandARM32(kMem, Ty), Base(Base), ImmOffset(ImmOffset), Index(nullptr),
      ShiftOp(kNoShift), ShiftAmt(0), Mode(Mode) {
  // The Neg modes are only needed for Reg +/- Reg.
  assert(!isNegAddrMode());
  NumVars = 1;
  Vars = &this->Base;
}

OperandARM32Mem::OperandARM32Mem(Cfg *Func, Type Ty, Variable *Base,
                                 Variable *Index, ShiftKind ShiftOp,
                                 uint16_t ShiftAmt, AddrMode Mode)
    : OperandARM32(kMem, Ty), Base(Base), ImmOffset(0), Index(Index),
      ShiftOp(ShiftOp), ShiftAmt(ShiftAmt), Mode(Mode) {
  NumVars = 2;
  Vars = Func->allocateArrayOf<Variable *>(2);
  Vars[0] = Base;
  Vars[1] = Index;
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

OperandARM32FlexImm::OperandARM32FlexImm(Cfg * /* Func */, Type Ty,
                                         uint32_t Imm, uint32_t RotateAmt)
    : OperandARM32Flex(kFlexImm, Ty), Imm(Imm), RotateAmt(RotateAmt) {
  NumVars = 0;
  Vars = nullptr;
}

bool OperandARM32FlexImm::canHoldImm(uint32_t Immediate, uint32_t *RotateAmt,
                                     uint32_t *Immed_8) {
  // Avoid the more expensive test for frequent small immediate values.
  if (Immediate <= 0xFF) {
    *RotateAmt = 0;
    *Immed_8 = Immediate;
    return true;
  }
  // Note that immediate must be unsigned for the test to work correctly.
  for (int Rot = 1; Rot < 16; Rot++) {
    uint32_t Imm8 = Utils::rotateLeft32(Immediate, 2 * Rot);
    if (Imm8 <= 0xFF) {
      *RotateAmt = Rot;
      *Immed_8 = Imm8;
      return true;
    }
  }
  return false;
}

OperandARM32FlexReg::OperandARM32FlexReg(Cfg *Func, Type Ty, Variable *Reg,
                                         ShiftKind ShiftOp, Operand *ShiftAmt)
    : OperandARM32Flex(kFlexReg, Ty), Reg(Reg), ShiftOp(ShiftOp),
      ShiftAmt(ShiftAmt) {
  NumVars = 1;
  Variable *ShiftVar = llvm::dyn_cast_or_null<Variable>(ShiftAmt);
  if (ShiftVar)
    ++NumVars;
  Vars = Func->allocateArrayOf<Variable *>(NumVars);
  Vars[0] = Reg;
  if (ShiftVar)
    Vars[1] = ShiftVar;
}

InstARM32Br::InstARM32Br(Cfg *Func, const CfgNode *TargetTrue,
                         const CfgNode *TargetFalse, CondARM32::Cond Pred)
    : InstARM32Pred(Func, InstARM32::Br, 0, nullptr, Pred),
      TargetTrue(TargetTrue), TargetFalse(TargetFalse) {}

bool InstARM32Br::optimizeBranch(const CfgNode *NextNode) {
  // If there is no next block, then there can be no fallthrough to
  // optimize.
  if (NextNode == nullptr)
    return false;
  // If there is no fallthrough node, such as a non-default case label
  // for a switch instruction, then there is no opportunity to
  // optimize.
  if (getTargetFalse() == nullptr)
    return false;

  // Unconditional branch to the next node can be removed.
  if (isUnconditionalBranch() && getTargetFalse() == NextNode) {
    assert(getTargetTrue() == nullptr);
    setDeleted();
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
    assert(Predicate != CondARM32::AL);
    setPredicate(getOppositeCondition(getPredicate()));
    TargetTrue = getTargetFalse();
    TargetFalse = nullptr;
    return true;
  }
  return false;
}

bool InstARM32Br::repointEdge(CfgNode *OldNode, CfgNode *NewNode) {
  if (TargetFalse == OldNode) {
    TargetFalse = NewNode;
    return true;
  } else if (TargetTrue == OldNode) {
    TargetTrue = NewNode;
    return true;
  }
  return false;
}

InstARM32Call::InstARM32Call(Cfg *Func, Variable *Dest, Operand *CallTarget)
    : InstARM32(Func, InstARM32::Call, 1, Dest) {
  HasSideEffects = true;
  addSource(CallTarget);
}

InstARM32Cmp::InstARM32Cmp(Cfg *Func, Variable *Src1, Operand *Src2,
                           CondARM32::Cond Predicate)
    : InstARM32Pred(Func, InstARM32::Cmp, 2, nullptr, Predicate) {
  addSource(Src1);
  addSource(Src2);
}

InstARM32Ldr::InstARM32Ldr(Cfg *Func, Variable *Dest, OperandARM32Mem *Mem,
                           CondARM32::Cond Predicate)
    : InstARM32Pred(Func, InstARM32::Ldr, 1, Dest, Predicate) {
  addSource(Mem);
}

InstARM32Mla::InstARM32Mla(Cfg *Func, Variable *Dest, Variable *Src0,
                           Variable *Src1, Variable *Acc,
                           CondARM32::Cond Predicate)
    : InstARM32Pred(Func, InstARM32::Mla, 3, Dest, Predicate) {
  addSource(Src0);
  addSource(Src1);
  addSource(Acc);
}

InstARM32Pop::InstARM32Pop(Cfg *Func, const VarList &Dests)
    : InstARM32(Func, InstARM32::Pop, 0, nullptr), Dests(Dests) {
  // Track modifications to Dests separately via FakeDefs.
  // Also, a pop instruction affects the stack pointer and so it should not
  // be allowed to be automatically dead-code eliminated. This is automatic
  // since we leave the Dest as nullptr.
}

InstARM32Push::InstARM32Push(Cfg *Func, const VarList &Srcs)
    : InstARM32(Func, InstARM32::Push, Srcs.size(), nullptr) {
  for (Variable *Source : Srcs)
    addSource(Source);
}

InstARM32Ret::InstARM32Ret(Cfg *Func, Variable *LR, Variable *Source)
    : InstARM32(Func, InstARM32::Ret, Source ? 2 : 1, nullptr) {
  addSource(LR);
  if (Source)
    addSource(Source);
}

InstARM32Umull::InstARM32Umull(Cfg *Func, Variable *DestLo, Variable *DestHi,
                               Variable *Src0, Variable *Src1,
                               CondARM32::Cond Predicate)
    : InstARM32Pred(Func, InstARM32::Umull, 2, DestLo, Predicate),
      // DestHi is expected to have a FakeDef inserted by the lowering code.
      DestHi(DestHi) {
  addSource(Src0);
  addSource(Src1);
}

// ======================== Dump routines ======================== //

// Two-addr ops
template <> const char *InstARM32Movt::Opcode = "movt";
// Unary ops
template <> const char *InstARM32Movw::Opcode = "movw";
template <> const char *InstARM32Mvn::Opcode = "mvn";
// Mov-like ops
template <> const char *InstARM32Mov::Opcode = "mov";
// Three-addr ops
template <> const char *InstARM32Adc::Opcode = "adc";
template <> const char *InstARM32Add::Opcode = "add";
template <> const char *InstARM32And::Opcode = "and";
template <> const char *InstARM32Bic::Opcode = "bic";
template <> const char *InstARM32Eor::Opcode = "eor";
template <> const char *InstARM32Lsl::Opcode = "lsl";
template <> const char *InstARM32Mul::Opcode = "mul";
template <> const char *InstARM32Orr::Opcode = "orr";
template <> const char *InstARM32Sbc::Opcode = "sbc";
template <> const char *InstARM32Sub::Opcode = "sub";

void InstARM32::dump(const Cfg *Func) const {
  if (!ALLOW_DUMP)
    return;
  Ostream &Str = Func->getContext()->getStrDump();
  Str << "[ARM32] ";
  Inst::dump(Func);
}

template <> void InstARM32Mov::emit(const Cfg *Func) const {
  if (!ALLOW_DUMP)
    return;
  Ostream &Str = Func->getContext()->getStrEmit();
  assert(getSrcSize() == 1);
  Variable *Dest = getDest();
  if (Dest->hasReg()) {
    const char *Opcode = "mov";
    Operand *Src0 = getSrc(0);
    if (const auto *Src0V = llvm::dyn_cast<Variable>(Src0)) {
      if (!Src0V->hasReg()) {
        Opcode = "ldr"; // Always load the full stack slot (vs ldrb, ldrh).
      }
    } else {
      // If Src isn't a variable, it shouldn't be a memory operand either
      // (otherwise Opcode will have to be ldr).
      assert(!llvm::isa<OperandARM32Mem>(Src0));
    }
    Str << "\t" << Opcode << getPredicate() << "\t";
    getDest()->emit(Func);
    Str << ", ";
    getSrc(0)->emit(Func);
  } else {
    Variable *Src0 = llvm::cast<Variable>(getSrc(0));
    assert(Src0->hasReg());
    Str << "\t"
        << "str" << getPredicate() << "\t";
    Src0->emit(Func);
    Str << ", ";
    Dest->emit(Func);
  }
}

template <> void InstARM32Mov::emitIAS(const Cfg *Func) const {
  assert(getSrcSize() == 1);
  (void)Func;
  llvm_unreachable("Not yet implemented");
}

void InstARM32Br::emit(const Cfg *Func) const {
  if (!ALLOW_DUMP)
    return;
  Ostream &Str = Func->getContext()->getStrEmit();
  Str << "\t"
      << "b" << getPredicate() << "\t";
  if (isUnconditionalBranch()) {
    Str << getTargetFalse()->getAsmName();
  } else {
    Str << getTargetTrue()->getAsmName();
    if (getTargetFalse()) {
      Str << "\n\t"
          << "b"
          << "\t" << getTargetFalse()->getAsmName();
    }
  }
}

void InstARM32Br::emitIAS(const Cfg *Func) const {
  (void)Func;
  llvm_unreachable("Not yet implemented");
}

void InstARM32Br::dump(const Cfg *Func) const {
  if (!ALLOW_DUMP)
    return;
  Ostream &Str = Func->getContext()->getStrDump();
  Str << "br ";

  if (getPredicate() == CondARM32::AL) {
    Str << "label %" << getTargetFalse()->getName();
    return;
  }

  Str << getPredicate() << ", label %" << getTargetTrue()->getName();
  if (getTargetFalse()) {
    Str << ", label %" << getTargetFalse()->getName();
  }
}

void InstARM32Call::emit(const Cfg *Func) const {
  if (!ALLOW_DUMP)
    return;
  Ostream &Str = Func->getContext()->getStrEmit();
  assert(getSrcSize() == 1);
  if (llvm::isa<ConstantInteger32>(getCallTarget())) {
    // This shouldn't happen (typically have to copy the full 32-bits
    // to a register and do an indirect jump).
    llvm::report_fatal_error("ARM32Call to ConstantInteger32");
  } else if (const auto CallTarget =
                 llvm::dyn_cast<ConstantRelocatable>(getCallTarget())) {
    // Calls only have 24-bits, but the linker should insert veneers to
    // extend the range if needed.
    Str << "\t"
        << "bl"
        << "\t";
    CallTarget->emitWithoutPrefix(Func->getTarget());
  } else {
    Str << "\t"
        << "blx"
        << "\t";
    getCallTarget()->emit(Func);
  }
  Func->getTarget()->resetStackAdjustment();
}

void InstARM32Call::emitIAS(const Cfg *Func) const {
  (void)Func;
  llvm_unreachable("Not yet implemented");
}

void InstARM32Call::dump(const Cfg *Func) const {
  if (!ALLOW_DUMP)
    return;
  Ostream &Str = Func->getContext()->getStrDump();
  if (getDest()) {
    dumpDest(Func);
    Str << " = ";
  }
  Str << "call ";
  getCallTarget()->dump(Func);
}

void InstARM32Cmp::emit(const Cfg *Func) const {
  if (!ALLOW_DUMP)
    return;
  Ostream &Str = Func->getContext()->getStrEmit();
  assert(getSrcSize() == 2);
  Str << "\t"
      << "cmp" << getPredicate() << "\t";
  getSrc(0)->emit(Func);
  Str << ", ";
  getSrc(1)->emit(Func);
}

void InstARM32Cmp::emitIAS(const Cfg *Func) const {
  assert(getSrcSize() == 2);
  (void)Func;
  llvm_unreachable("Not yet implemented");
}

void InstARM32Cmp::dump(const Cfg *Func) const {
  if (!ALLOW_DUMP)
    return;
  Ostream &Str = Func->getContext()->getStrDump();
  dumpOpcodePred(Str, "cmp", getSrc(0)->getType());
  dumpSources(Func);
}

void InstARM32Ldr::emit(const Cfg *Func) const {
  if (!ALLOW_DUMP)
    return;
  Ostream &Str = Func->getContext()->getStrEmit();
  assert(getSrcSize() == 1);
  assert(getDest()->hasReg());
  Type Ty = getSrc(0)->getType();
  Str << "\t"
      << "ldr" << getWidthString(Ty) << getPredicate() << "\t";
  getDest()->emit(Func);
  Str << ", ";
  getSrc(0)->emit(Func);
}

void InstARM32Ldr::emitIAS(const Cfg *Func) const {
  assert(getSrcSize() == 1);
  (void)Func;
  llvm_unreachable("Not yet implemented");
}

void InstARM32Ldr::dump(const Cfg *Func) const {
  if (!ALLOW_DUMP)
    return;
  Ostream &Str = Func->getContext()->getStrDump();
  dumpDest(Func);
  Str << " = ";
  dumpOpcodePred(Str, "ldr", getDest()->getType());
  Str << " ";
  dumpSources(Func);
}

void InstARM32Mla::emit(const Cfg *Func) const {
  if (!ALLOW_DUMP)
    return;
  Ostream &Str = Func->getContext()->getStrEmit();
  assert(getSrcSize() == 3);
  assert(getDest()->hasReg());
  Str << "\t"
      << "mla" << getPredicate() << "\t";
  getDest()->emit(Func);
  Str << ", ";
  getSrc(0)->emit(Func);
  Str << ", ";
  getSrc(1)->emit(Func);
  Str << ", ";
  getSrc(2)->emit(Func);
}

void InstARM32Mla::emitIAS(const Cfg *Func) const {
  assert(getSrcSize() == 3);
  (void)Func;
  llvm_unreachable("Not yet implemented");
}

void InstARM32Mla::dump(const Cfg *Func) const {
  if (!ALLOW_DUMP)
    return;
  Ostream &Str = Func->getContext()->getStrDump();
  dumpDest(Func);
  Str << " = ";
  dumpOpcodePred(Str, "mla", getDest()->getType());
  Str << " ";
  dumpSources(Func);
}

template <> void InstARM32Movw::emit(const Cfg *Func) const {
  if (!ALLOW_DUMP)
    return;
  Ostream &Str = Func->getContext()->getStrEmit();
  assert(getSrcSize() == 1);
  Str << "\t" << Opcode << getPredicate() << "\t";
  getDest()->emit(Func);
  Str << ", ";
  Constant *Src0 = llvm::cast<Constant>(getSrc(0));
  if (auto CR = llvm::dyn_cast<ConstantRelocatable>(Src0)) {
    Str << "#:lower16:";
    CR->emitWithoutPrefix(Func->getTarget());
  } else {
    Src0->emit(Func);
  }
}

template <> void InstARM32Movt::emit(const Cfg *Func) const {
  if (!ALLOW_DUMP)
    return;
  Ostream &Str = Func->getContext()->getStrEmit();
  assert(getSrcSize() == 2);
  Variable *Dest = getDest();
  Constant *Src1 = llvm::cast<Constant>(getSrc(1));
  Str << "\t" << Opcode << getPredicate() << "\t";
  Dest->emit(Func);
  Str << ", ";
  if (auto CR = llvm::dyn_cast<ConstantRelocatable>(Src1)) {
    Str << "#:upper16:";
    CR->emitWithoutPrefix(Func->getTarget());
  } else {
    Src1->emit(Func);
  }
}

void InstARM32Pop::emit(const Cfg *Func) const {
  if (!ALLOW_DUMP)
    return;
  assert(Dests.size() > 0);
  Ostream &Str = Func->getContext()->getStrEmit();
  Str << "\t"
      << "pop"
      << "\t{";
  for (SizeT I = 0; I < Dests.size(); ++I) {
    if (I > 0)
      Str << ", ";
    Dests[I]->emit(Func);
  }
  Str << "}";
}

void InstARM32Pop::emitIAS(const Cfg *Func) const {
  (void)Func;
  llvm_unreachable("Not yet implemented");
}

void InstARM32Pop::dump(const Cfg *Func) const {
  if (!ALLOW_DUMP)
    return;
  Ostream &Str = Func->getContext()->getStrDump();
  Str << "pop"
      << " ";
  for (SizeT I = 0; I < Dests.size(); ++I) {
    if (I > 0)
      Str << ", ";
    Dests[I]->dump(Func);
  }
}

void InstARM32Push::emit(const Cfg *Func) const {
  if (!ALLOW_DUMP)
    return;
  assert(getSrcSize() > 0);
  Ostream &Str = Func->getContext()->getStrEmit();
  Str << "\t"
      << "push"
      << "\t{";
  emitSources(Func);
  Str << "}";
}

void InstARM32Push::emitIAS(const Cfg *Func) const {
  (void)Func;
  llvm_unreachable("Not yet implemented");
}

void InstARM32Push::dump(const Cfg *Func) const {
  if (!ALLOW_DUMP)
    return;
  Ostream &Str = Func->getContext()->getStrDump();
  Str << "push"
      << " ";
  dumpSources(Func);
}

void InstARM32Ret::emit(const Cfg *Func) const {
  if (!ALLOW_DUMP)
    return;
  assert(getSrcSize() > 0);
  Variable *LR = llvm::cast<Variable>(getSrc(0));
  assert(LR->hasReg());
  assert(LR->getRegNum() == RegARM32::Reg_lr);
  Ostream &Str = Func->getContext()->getStrEmit();
  Str << "\t"
      << "bx"
      << "\t";
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

void InstARM32Umull::emit(const Cfg *Func) const {
  if (!ALLOW_DUMP)
    return;
  Ostream &Str = Func->getContext()->getStrEmit();
  assert(getSrcSize() == 2);
  assert(getDest()->hasReg());
  Str << "\t"
      << "umull" << getPredicate() << "\t";
  getDest()->emit(Func);
  Str << ", ";
  DestHi->emit(Func);
  Str << ", ";
  getSrc(0)->emit(Func);
  Str << ", ";
  getSrc(1)->emit(Func);
}

void InstARM32Umull::emitIAS(const Cfg *Func) const {
  assert(getSrcSize() == 2);
  (void)Func;
  llvm_unreachable("Not yet implemented");
}

void InstARM32Umull::dump(const Cfg *Func) const {
  if (!ALLOW_DUMP)
    return;
  Ostream &Str = Func->getContext()->getStrDump();
  dumpDest(Func);
  Str << " = ";
  dumpOpcodePred(Str, "umull", getDest()->getType());
  Str << " ";
  dumpSources(Func);
}

void OperandARM32Mem::emit(const Cfg *Func) const {
  if (!ALLOW_DUMP)
    return;
  Ostream &Str = Func->getContext()->getStrEmit();
  Str << "[";
  getBase()->emit(Func);
  switch (getAddrMode()) {
  case PostIndex:
  case NegPostIndex:
    Str << "], ";
    break;
  default:
    Str << ", ";
    break;
  }
  if (isRegReg()) {
    if (isNegAddrMode()) {
      Str << "-";
    }
    getIndex()->emit(Func);
    if (getShiftOp() != kNoShift) {
      Str << ", " << InstARM32ShiftAttributes[getShiftOp()].EmitString << " #"
          << getShiftAmt();
    }
  } else {
    getOffset()->emit(Func);
  }
  switch (getAddrMode()) {
  case Offset:
  case NegOffset:
    Str << "]";
    break;
  case PreIndex:
  case NegPreIndex:
    Str << "]!";
    break;
  case PostIndex:
  case NegPostIndex:
    // Brace is already closed off.
    break;
  }
}

void OperandARM32Mem::dump(const Cfg *Func, Ostream &Str) const {
  if (!ALLOW_DUMP)
    return;
  Str << "[";
  if (Func)
    getBase()->dump(Func);
  else
    getBase()->dump(Str);
  Str << ", ";
  if (isRegReg()) {
    if (isNegAddrMode()) {
      Str << "-";
    }
    if (Func)
      getIndex()->dump(Func);
    else
      getIndex()->dump(Str);
    if (getShiftOp() != kNoShift) {
      Str << ", " << InstARM32ShiftAttributes[getShiftOp()].EmitString << " #"
          << getShiftAmt();
    }
  } else {
    getOffset()->dump(Func, Str);
  }
  Str << "] AddrMode==" << getAddrMode();
}

void OperandARM32FlexImm::emit(const Cfg *Func) const {
  if (!ALLOW_DUMP)
    return;
  Ostream &Str = Func->getContext()->getStrEmit();
  uint32_t Imm = getImm();
  uint32_t RotateAmt = getRotateAmt();
  Str << "#" << Utils::rotateRight32(Imm, 2 * RotateAmt);
}

void OperandARM32FlexImm::dump(const Cfg * /* Func */, Ostream &Str) const {
  if (!ALLOW_DUMP)
    return;
  uint32_t Imm = getImm();
  uint32_t RotateAmt = getRotateAmt();
  Str << "#(" << Imm << " ror 2*" << RotateAmt << ")";
}

void OperandARM32FlexReg::emit(const Cfg *Func) const {
  if (!ALLOW_DUMP)
    return;
  Ostream &Str = Func->getContext()->getStrEmit();
  getReg()->emit(Func);
  if (getShiftOp() != kNoShift) {
    Str << ", " << InstARM32ShiftAttributes[getShiftOp()].EmitString << " ";
    getShiftAmt()->emit(Func);
  }
}

void OperandARM32FlexReg::dump(const Cfg *Func, Ostream &Str) const {
  if (!ALLOW_DUMP)
    return;
  Variable *Reg = getReg();
  if (Func)
    Reg->dump(Func);
  else
    Reg->dump(Str);
  if (getShiftOp() != kNoShift) {
    Str << ", " << InstARM32ShiftAttributes[getShiftOp()].EmitString << " ";
    if (Func)
      getShiftAmt()->dump(Func);
    else
      getShiftAmt()->dump(Str);
  }
}

} // end of namespace Ice
