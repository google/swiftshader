//===- subzero/src/IceInstX86Base.h - Generic x86 instructions -*- C++ -*--===//
//
//                        The Subzero Code Generator
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief This file defines the InstX86Base template class, as well as the
/// generic X86 Instruction class hierarchy.
///
/// Only X86 instructions common across all/most X86 targets should be defined
/// here, with target-specific instructions declared in the target's traits.
///
//===----------------------------------------------------------------------===//

#ifndef SUBZERO_SRC_ICEINSTX86BASE_H
#define SUBZERO_SRC_ICEINSTX86BASE_H

#include "IceDefs.h"
#include "IceInst.h"
#include "IceOperand.h"

namespace Ice {

namespace X86Internal {

template <class Machine> struct MachineTraits;

template <class Machine> class InstX86Base : public InstTarget {
  InstX86Base<Machine>() = delete;
  InstX86Base<Machine>(const InstX86Base &) = delete;
  InstX86Base &operator=(const InstX86Base &) = delete;

public:
  using Traits = MachineTraits<Machine>;
  using X86TargetLowering = typename Traits::TargetLowering;

  enum InstKindX86 {
    k__Start = Inst::Target,
    Adc,
    AdcRMW,
    Add,
    AddRMW,
    Addps,
    Addss,
    Adjuststack,
    And,
    Andnps,
    Andps,
    AndRMW,
    Blendvps,
    Br,
    Bsf,
    Bsr,
    Bswap,
    Call,
    Cbwdq,
    Cmov,
    Cmpps,
    Cmpxchg,
    Cmpxchg8b,
    Cvt,
    Div,
    Divps,
    Divss,
    FakeRMW,
    Fld,
    Fstp,
    Icmp,
    Idiv,
    Imul,
    ImulImm,
    Insertps,
    Jmp,
    Label,
    Lea,
    Load,
    Mfence,
    Minss,
    Maxss,
    Mov,
    Movd,
    Movp,
    Movq,
    MovssRegs,
    Movsx,
    Movzx,
    Mul,
    Mulps,
    Mulss,
    Neg,
    Nop,
    Or,
    Orps,
    OrRMW,
    Padd,
    Pand,
    Pandn,
    Pblendvb,
    Pcmpeq,
    Pcmpgt,
    Pextr,
    Pinsr,
    Pmull,
    Pmuludq,
    Pop,
    Por,
    Pshufd,
    Psll,
    Psra,
    Psrl,
    Psub,
    Push,
    Pxor,
    Ret,
    Rol,
    Sar,
    Sbb,
    SbbRMW,
    Setcc,
    Shl,
    Shld,
    Shr,
    Shrd,
    Shufps,
    Sqrtss,
    Store,
    StoreP,
    StoreQ,
    Sub,
    SubRMW,
    Subps,
    Subss,
    Test,
    Ucomiss,
    UD2,
    Xadd,
    Xchg,
    Xor,
    Xorps,
    XorRMW,

    /// Intel Architecture Code Analyzer markers. These are not executable so
    /// must only be used for analysis.
    IacaStart,
    IacaEnd
  };

  enum SseSuffix { None, Packed, Scalar, Integral };

  static const char *getWidthString(Type Ty);
  static const char *getFldString(Type Ty);
  static typename Traits::Cond::BrCond
  getOppositeCondition(typename Traits::Cond::BrCond Cond);
  void dump(const Cfg *Func) const override;

  // Shared emit routines for common forms of instructions.
  void emitTwoAddress(const Cfg *Func, const char *Opcode,
                      const char *Suffix = "") const;

  static void
  emitIASGPRShift(const Cfg *Func, Type Ty, const Variable *Var,
                  const Operand *Src,
                  const typename Traits::Assembler::GPREmitterShiftOp &Emitter);

  static X86TargetLowering *getTarget(const Cfg *Func) {
    return static_cast<X86TargetLowering *>(Func->getTarget());
  }

protected:
  InstX86Base<Machine>(Cfg *Func, InstKindX86 Kind, SizeT Maxsrcs,
                       Variable *Dest)
      : InstTarget(Func, static_cast<InstKind>(Kind), Maxsrcs, Dest) {}

  static bool isClassof(const Inst *Inst, InstKindX86 MyKind) {
    return Inst->getKind() == static_cast<InstKind>(MyKind);
  }
  // Most instructions that operate on vector arguments require vector memory
  // operands to be fully aligned (16-byte alignment for PNaCl vector types).
  // The stack frame layout and call ABI ensure proper alignment for stack
  // operands, but memory operands (originating from load/store bitcode
  // instructions) only have element-size alignment guarantees. This function
  // validates that none of the operands is a memory operand of vector type,
  // calling report_fatal_error() if one is found. This function should be
  // called during emission, and maybe also in the ctor (as long as that fits
  // the lowering style).
  void validateVectorAddrMode() const {
    if (this->getDest())
      this->validateVectorAddrModeOpnd(this->getDest());
    for (SizeT i = 0; i < this->getSrcSize(); ++i) {
      this->validateVectorAddrModeOpnd(this->getSrc(i));
    }
  }

private:
  static void validateVectorAddrModeOpnd(const Operand *Opnd) {
    if (llvm::isa<typename InstX86Base<Machine>::Traits::X86OperandMem>(Opnd) &&
        isVectorType(Opnd->getType())) {
      llvm::report_fatal_error("Possible misaligned vector memory operation");
    }
  }
};

/// InstX86FakeRMW represents a non-atomic read-modify-write operation on a
/// memory location. An InstX86FakeRMW is a "fake" instruction in that it still
/// needs to be lowered to some actual RMW instruction.
///
/// If A is some memory address, D is some data value to apply, and OP is an
/// arithmetic operator, the instruction operates as: (*A) = (*A) OP D
template <class Machine>
class InstX86FakeRMW final : public InstX86Base<Machine> {
  InstX86FakeRMW() = delete;
  InstX86FakeRMW(const InstX86FakeRMW &) = delete;
  InstX86FakeRMW &operator=(const InstX86FakeRMW &) = delete;

public:
  static InstX86FakeRMW *create(Cfg *Func, Operand *Data, Operand *Addr,
                                Variable *Beacon, InstArithmetic::OpKind Op,
                                uint32_t Align = 1) {
    // TODO(stichnot): Stop ignoring alignment specification.
    (void)Align;
    return new (Func->allocate<InstX86FakeRMW>())
        InstX86FakeRMW(Func, Data, Addr, Op, Beacon);
  }
  Operand *getAddr() const { return this->getSrc(1); }
  Operand *getData() const { return this->getSrc(0); }
  InstArithmetic::OpKind getOp() const { return Op; }
  Variable *getBeacon() const { return llvm::cast<Variable>(this->getSrc(2)); }
  void dump(const Cfg *Func) const override;
  static bool classof(const Inst *Inst) {
    return InstX86Base<Machine>::isClassof(Inst, InstX86Base<Machine>::FakeRMW);
  }

private:
  InstArithmetic::OpKind Op;
  InstX86FakeRMW(Cfg *Func, Operand *Data, Operand *Addr,
                 InstArithmetic::OpKind Op, Variable *Beacon);
};

/// InstX86Label represents an intra-block label that is the target of an
/// intra-block branch. The offset between the label and the branch must be fit
/// into one byte (considered "near"). These are used for lowering i1
/// calculations, Select instructions, and 64-bit compares on a 32-bit
/// architecture, without basic block splitting. Basic block splitting is not so
/// desirable for several reasons, one of which is the impact on decisions based
/// on whether a variable's live range spans multiple basic blocks.
///
/// Intra-block control flow must be used with caution. Consider the sequence
/// for "c = (a >= b ? x : y)".
///     cmp a, b
///     br lt, L1
///     mov c, x
///     jmp L2
///   L1:
///     mov c, y
///   L2:
///
/// Labels L1 and L2 are intra-block labels. Without knowledge of the
/// intra-block control flow, liveness analysis will determine the "mov c, x"
/// instruction to be dead. One way to prevent this is to insert a "FakeUse(c)"
/// instruction anywhere between the two "mov c, ..." instructions, e.g.:
///
///     cmp a, b
///     br lt, L1
///     mov c, x
///     jmp L2
///     FakeUse(c)
///   L1:
///     mov c, y
///   L2:
///
/// The down-side is that "mov c, x" can never be dead-code eliminated even if
/// there are no uses of c. As unlikely as this situation is, it may be
/// prevented by running dead code elimination before lowering.
template <class Machine>
class InstX86Label final : public InstX86Base<Machine> {
  InstX86Label() = delete;
  InstX86Label(const InstX86Label &) = delete;
  InstX86Label &operator=(const InstX86Label &) = delete;

public:
  static InstX86Label *
  create(Cfg *Func,
         typename InstX86Base<Machine>::Traits::TargetLowering *Target) {
    return new (Func->allocate<InstX86Label>()) InstX86Label(Func, Target);
  }
  uint32_t getEmitInstCount() const override { return 0; }
  IceString getName(const Cfg *Func) const;
  SizeT getNumber() const { return Number; }
  void emit(const Cfg *Func) const override;
  void emitIAS(const Cfg *Func) const override;
  void dump(const Cfg *Func) const override;

private:
  InstX86Label(Cfg *Func,
               typename InstX86Base<Machine>::Traits::TargetLowering *Target);

  SizeT Number; // used for unique label generation.
};

/// Conditional and unconditional branch instruction.
template <class Machine> class InstX86Br final : public InstX86Base<Machine> {
  InstX86Br() = delete;
  InstX86Br(const InstX86Br &) = delete;
  InstX86Br &operator=(const InstX86Br &) = delete;

public:
  enum Mode { Near, Far };

  /// Create a conditional branch to a node.
  static InstX86Br *
  create(Cfg *Func, CfgNode *TargetTrue, CfgNode *TargetFalse,
         typename InstX86Base<Machine>::Traits::Cond::BrCond Condition,
         Mode Kind) {
    assert(Condition != InstX86Base<Machine>::Traits::Cond::Br_None);
    constexpr InstX86Label<Machine> *NoLabel = nullptr;
    return new (Func->allocate<InstX86Br>())
        InstX86Br(Func, TargetTrue, TargetFalse, NoLabel, Condition, Kind);
  }
  /// Create an unconditional branch to a node.
  static InstX86Br *create(Cfg *Func, CfgNode *Target, Mode Kind) {
    constexpr CfgNode *NoCondTarget = nullptr;
    constexpr InstX86Label<Machine> *NoLabel = nullptr;
    return new (Func->allocate<InstX86Br>())
        InstX86Br(Func, NoCondTarget, Target, NoLabel,
                  InstX86Base<Machine>::Traits::Cond::Br_None, Kind);
  }
  /// Create a non-terminator conditional branch to a node, with a fallthrough
  /// to the next instruction in the current node. This is used for switch
  /// lowering.
  static InstX86Br *
  create(Cfg *Func, CfgNode *Target,
         typename InstX86Base<Machine>::Traits::Cond::BrCond Condition,
         Mode Kind) {
    assert(Condition != InstX86Base<Machine>::Traits::Cond::Br_None);
    constexpr CfgNode *NoUncondTarget = nullptr;
    constexpr InstX86Label<Machine> *NoLabel = nullptr;
    return new (Func->allocate<InstX86Br>())
        InstX86Br(Func, Target, NoUncondTarget, NoLabel, Condition, Kind);
  }
  /// Create a conditional intra-block branch (or unconditional, if
  /// Condition==Br_None) to a label in the current block.
  static InstX86Br *
  create(Cfg *Func, InstX86Label<Machine> *Label,
         typename InstX86Base<Machine>::Traits::Cond::BrCond Condition,
         Mode Kind) {
    constexpr CfgNode *NoCondTarget = nullptr;
    constexpr CfgNode *NoUncondTarget = nullptr;
    return new (Func->allocate<InstX86Br>())
        InstX86Br(Func, NoCondTarget, NoUncondTarget, Label, Condition, Kind);
  }
  const CfgNode *getTargetTrue() const { return TargetTrue; }
  const CfgNode *getTargetFalse() const { return TargetFalse; }
  bool isNear() const { return Kind == Near; }
  bool optimizeBranch(const CfgNode *NextNode);
  uint32_t getEmitInstCount() const override {
    uint32_t Sum = 0;
    if (Label)
      ++Sum;
    if (getTargetTrue())
      ++Sum;
    if (getTargetFalse())
      ++Sum;
    return Sum;
  }
  bool isUnconditionalBranch() const override {
    return !Label && Condition == InstX86Base<Machine>::Traits::Cond::Br_None;
  }
  bool repointEdges(CfgNode *OldNode, CfgNode *NewNode) override;
  void emit(const Cfg *Func) const override;
  void emitIAS(const Cfg *Func) const override;
  void dump(const Cfg *Func) const override;
  static bool classof(const Inst *Inst) {
    return InstX86Base<Machine>::isClassof(Inst, InstX86Base<Machine>::Br);
  }

private:
  InstX86Br(Cfg *Func, const CfgNode *TargetTrue, const CfgNode *TargetFalse,
            const InstX86Label<Machine> *Label,
            typename InstX86Base<Machine>::Traits::Cond::BrCond Condition,
            Mode Kind);

  typename InstX86Base<Machine>::Traits::Cond::BrCond Condition;
  const CfgNode *TargetTrue;
  const CfgNode *TargetFalse;
  const InstX86Label<Machine> *Label; // Intra-block branch target
  const Mode Kind;
};

/// Jump to a target outside this function, such as tailcall, nacljump, naclret,
/// unreachable. This is different from a Branch instruction in that there is no
/// intra-function control flow to represent.
template <class Machine> class InstX86Jmp final : public InstX86Base<Machine> {
  InstX86Jmp() = delete;
  InstX86Jmp(const InstX86Jmp &) = delete;
  InstX86Jmp &operator=(const InstX86Jmp &) = delete;

public:
  static InstX86Jmp *create(Cfg *Func, Operand *Target) {
    return new (Func->allocate<InstX86Jmp>()) InstX86Jmp(Func, Target);
  }
  Operand *getJmpTarget() const { return this->getSrc(0); }
  void emit(const Cfg *Func) const override;
  void emitIAS(const Cfg *Func) const override;
  void dump(const Cfg *Func) const override;
  static bool classof(const Inst *Inst) {
    return InstX86Base<Machine>::isClassof(Inst, InstX86Base<Machine>::Jmp);
  }

private:
  InstX86Jmp(Cfg *Func, Operand *Target);
};

/// Call instruction. Arguments should have already been pushed.
template <class Machine> class InstX86Call final : public InstX86Base<Machine> {
  InstX86Call() = delete;
  InstX86Call(const InstX86Call &) = delete;
  InstX86Call &operator=(const InstX86Call &) = delete;

public:
  static InstX86Call *create(Cfg *Func, Variable *Dest, Operand *CallTarget) {
    return new (Func->allocate<InstX86Call>())
        InstX86Call(Func, Dest, CallTarget);
  }
  Operand *getCallTarget() const { return this->getSrc(0); }
  void emit(const Cfg *Func) const override;
  void emitIAS(const Cfg *Func) const override;
  void dump(const Cfg *Func) const override;
  static bool classof(const Inst *Inst) {
    return InstX86Base<Machine>::isClassof(Inst, InstX86Base<Machine>::Call);
  }

private:
  InstX86Call(Cfg *Func, Variable *Dest, Operand *CallTarget);
};

/// Emit a one-operand (GPR) instruction.
template <class Machine>
void emitIASOpTyGPR(const Cfg *Func, Type Ty, const Operand *Var,
                    const typename InstX86Base<
                        Machine>::Traits::Assembler::GPREmitterOneOp &Emitter);

template <class Machine>
void emitIASAsAddrOpTyGPR(
    const Cfg *Func, Type Ty, const Operand *Op0, const Operand *Op1,
    const typename InstX86Base<Machine>::Traits::Assembler::GPREmitterAddrOp
        &Emitter);

/// Instructions of the form x := op(x).
template <class Machine, typename InstX86Base<Machine>::InstKindX86 K>
class InstX86BaseInplaceopGPR : public InstX86Base<Machine> {
  InstX86BaseInplaceopGPR() = delete;
  InstX86BaseInplaceopGPR(const InstX86BaseInplaceopGPR &) = delete;
  InstX86BaseInplaceopGPR &operator=(const InstX86BaseInplaceopGPR &) = delete;

public:
  using Base = InstX86BaseInplaceopGPR<Machine, K>;

  void emit(const Cfg *Func) const override {
    if (!BuildDefs::dump())
      return;
    Ostream &Str = Func->getContext()->getStrEmit();
    assert(this->getSrcSize() == 1);
    Str << "\t" << Opcode << "\t";
    this->getSrc(0)->emit(Func);
  }
  void emitIAS(const Cfg *Func) const override {
    assert(this->getSrcSize() == 1);
    const Variable *Var = this->getDest();
    Type Ty = Var->getType();
    emitIASOpTyGPR<Machine>(Func, Ty, Var, Emitter);
  }
  void dump(const Cfg *Func) const override {
    if (!BuildDefs::dump())
      return;
    Ostream &Str = Func->getContext()->getStrDump();
    this->dumpDest(Func);
    Str << " = " << Opcode << "." << this->getDest()->getType() << " ";
    this->dumpSources(Func);
  }
  static bool classof(const Inst *Inst) {
    return InstX86Base<Machine>::isClassof(Inst, InstX86Base<Machine>::K);
  }

protected:
  InstX86BaseInplaceopGPR(Cfg *Func, Operand *SrcDest)
      : InstX86Base<Machine>(Func, K, 1, llvm::dyn_cast<Variable>(SrcDest)) {
    this->addSource(SrcDest);
  }

private:
  static const char *Opcode;
  static const typename InstX86Base<Machine>::Traits::Assembler::GPREmitterOneOp
      Emitter;
};

/// Emit a two-operand (GPR) instruction, where the dest operand is a Variable
/// that's guaranteed to be a register.
template <class Machine, bool VarCanBeByte = true, bool SrcCanBeByte = true>
void emitIASRegOpTyGPR(
    const Cfg *Func, Type Ty, const Variable *Dst, const Operand *Src,
    const typename InstX86Base<Machine>::Traits::Assembler::GPREmitterRegOp
        &Emitter);

/// Instructions of the form x := op(y).
template <class Machine, typename InstX86Base<Machine>::InstKindX86 K>
class InstX86BaseUnaryopGPR : public InstX86Base<Machine> {
  InstX86BaseUnaryopGPR() = delete;
  InstX86BaseUnaryopGPR(const InstX86BaseUnaryopGPR &) = delete;
  InstX86BaseUnaryopGPR &operator=(const InstX86BaseUnaryopGPR &) = delete;

public:
  using Base = InstX86BaseUnaryopGPR<Machine, K>;

  void emit(const Cfg *Func) const override {
    if (!BuildDefs::dump())
      return;
    Ostream &Str = Func->getContext()->getStrEmit();
    assert(this->getSrcSize() == 1);
    Type SrcTy = this->getSrc(0)->getType();
    Type DestTy = this->getDest()->getType();
    Str << "\t" << Opcode << this->getWidthString(SrcTy);
    // Movsx and movzx need both the source and dest type width letter to
    // define the operation. The other unary operations have the same source
    // and dest type and as a result need only one letter.
    if (SrcTy != DestTy)
      Str << this->getWidthString(DestTy);
    Str << "\t";
    this->getSrc(0)->emit(Func);
    Str << ", ";
    this->getDest()->emit(Func);
  }
  void emitIAS(const Cfg *Func) const override {
    assert(this->getSrcSize() == 1);
    const Variable *Var = this->getDest();
    Type Ty = Var->getType();
    const Operand *Src = this->getSrc(0);
    emitIASRegOpTyGPR<Machine>(Func, Ty, Var, Src, Emitter);
  }
  void dump(const Cfg *Func) const override {
    if (!BuildDefs::dump())
      return;
    Ostream &Str = Func->getContext()->getStrDump();
    this->dumpDest(Func);
    Str << " = " << Opcode << "." << this->getSrc(0)->getType() << " ";
    this->dumpSources(Func);
  }
  static bool classof(const Inst *Inst) {
    return InstX86Base<Machine>::isClassof(Inst, InstX86Base<Machine>::K);
  }

protected:
  InstX86BaseUnaryopGPR(Cfg *Func, Variable *Dest, Operand *Src)
      : InstX86Base<Machine>(Func, K, 1, Dest) {
    this->addSource(Src);
  }

  static const char *Opcode;
  static const typename InstX86Base<Machine>::Traits::Assembler::GPREmitterRegOp
      Emitter;
};

template <class Machine>
void emitIASRegOpTyXMM(
    const Cfg *Func, Type Ty, const Variable *Var, const Operand *Src,
    const typename InstX86Base<Machine>::Traits::Assembler::XmmEmitterRegOp
        &Emitter);

template <class Machine, typename InstX86Base<Machine>::InstKindX86 K>
class InstX86BaseUnaryopXmm : public InstX86Base<Machine> {
  InstX86BaseUnaryopXmm() = delete;
  InstX86BaseUnaryopXmm(const InstX86BaseUnaryopXmm &) = delete;
  InstX86BaseUnaryopXmm &operator=(const InstX86BaseUnaryopXmm &) = delete;

public:
  using Base = InstX86BaseUnaryopXmm<Machine, K>;

  void emit(const Cfg *Func) const override {
    if (!BuildDefs::dump())
      return;
    Ostream &Str = Func->getContext()->getStrEmit();
    assert(this->getSrcSize() == 1);
    Str << "\t" << Opcode << "\t";
    this->getSrc(0)->emit(Func);
    Str << ", ";
    this->getDest()->emit(Func);
  }
  void emitIAS(const Cfg *Func) const override {
    Type Ty = this->getDest()->getType();
    assert(this->getSrcSize() == 1);
    emitIASRegOpTyXMM<Machine>(Func, Ty, this->getDest(), this->getSrc(0),
                               Emitter);
  }
  void dump(const Cfg *Func) const override {
    if (!BuildDefs::dump())
      return;
    Ostream &Str = Func->getContext()->getStrDump();
    this->dumpDest(Func);
    Str << " = " << Opcode << "." << this->getDest()->getType() << " ";
    this->dumpSources(Func);
  }
  static bool classof(const Inst *Inst) {
    return InstX86Base<Machine>::isClassof(Inst, InstX86Base<Machine>::K);
  }

protected:
  InstX86BaseUnaryopXmm(Cfg *Func, Variable *Dest, Operand *Src)
      : InstX86Base<Machine>(Func, K, 1, Dest) {
    this->addSource(Src);
  }

  static const char *Opcode;
  static const typename InstX86Base<Machine>::Traits::Assembler::XmmEmitterRegOp
      Emitter;
};

template <class Machine, typename InstX86Base<Machine>::InstKindX86 K>
class InstX86BaseBinopGPRShift : public InstX86Base<Machine> {
  InstX86BaseBinopGPRShift() = delete;
  InstX86BaseBinopGPRShift(const InstX86BaseBinopGPRShift &) = delete;
  InstX86BaseBinopGPRShift &
  operator=(const InstX86BaseBinopGPRShift &) = delete;

public:
  using Base = InstX86BaseBinopGPRShift<Machine, K>;

  void emit(const Cfg *Func) const override {
    if (!BuildDefs::dump())
      return;
    this->emitTwoAddress(Func, Opcode);
  }
  void emitIAS(const Cfg *Func) const override {
    Type Ty = this->getDest()->getType();
    assert(this->getSrcSize() == 2);
    this->emitIASGPRShift(Func, Ty, this->getDest(), this->getSrc(1), Emitter);
  }
  void dump(const Cfg *Func) const override {
    if (!BuildDefs::dump())
      return;
    Ostream &Str = Func->getContext()->getStrDump();
    this->dumpDest(Func);
    Str << " = " << Opcode << "." << this->getDest()->getType() << " ";
    this->dumpSources(Func);
  }
  static bool classof(const Inst *Inst) {
    return InstX86Base<Machine>::isClassof(Inst, InstX86Base<Machine>::K);
  }

protected:
  InstX86BaseBinopGPRShift(Cfg *Func, Variable *Dest, Operand *Source)
      : InstX86Base<Machine>(Func, K, 2, Dest) {
    this->addSource(Dest);
    this->addSource(Source);
  }

  static const char *Opcode;
  static const typename InstX86Base<
      Machine>::Traits::Assembler::GPREmitterShiftOp Emitter;
};

template <class Machine, typename InstX86Base<Machine>::InstKindX86 K>
class InstX86BaseBinopGPR : public InstX86Base<Machine> {
  InstX86BaseBinopGPR() = delete;
  InstX86BaseBinopGPR(const InstX86BaseBinopGPR &) = delete;
  InstX86BaseBinopGPR &operator=(const InstX86BaseBinopGPR &) = delete;

public:
  using Base = InstX86BaseBinopGPR<Machine, K>;

  void emit(const Cfg *Func) const override {
    if (!BuildDefs::dump())
      return;
    this->emitTwoAddress(Func, Opcode);
  }
  void emitIAS(const Cfg *Func) const override {
    Type Ty = this->getDest()->getType();
    assert(this->getSrcSize() == 2);
    emitIASRegOpTyGPR<Machine>(Func, Ty, this->getDest(), this->getSrc(1),
                               Emitter);
  }
  void dump(const Cfg *Func) const override {
    if (!BuildDefs::dump())
      return;
    Ostream &Str = Func->getContext()->getStrDump();
    this->dumpDest(Func);
    Str << " = " << Opcode << "." << this->getDest()->getType() << " ";
    this->dumpSources(Func);
  }
  static bool classof(const Inst *Inst) {
    return InstX86Base<Machine>::isClassof(Inst, InstX86Base<Machine>::K);
  }

protected:
  InstX86BaseBinopGPR(Cfg *Func, Variable *Dest, Operand *Source)
      : InstX86Base<Machine>(Func, K, 2, Dest) {
    this->addSource(Dest);
    this->addSource(Source);
  }

  static const char *Opcode;
  static const typename InstX86Base<Machine>::Traits::Assembler::GPREmitterRegOp
      Emitter;
};

template <class Machine, typename InstX86Base<Machine>::InstKindX86 K>
class InstX86BaseBinopRMW : public InstX86Base<Machine> {
  InstX86BaseBinopRMW() = delete;
  InstX86BaseBinopRMW(const InstX86BaseBinopRMW &) = delete;
  InstX86BaseBinopRMW &operator=(const InstX86BaseBinopRMW &) = delete;

public:
  using Base = InstX86BaseBinopRMW<Machine, K>;

  void emit(const Cfg *Func) const override {
    if (!BuildDefs::dump())
      return;
    this->emitTwoAddress(Func, Opcode);
  }
  void emitIAS(const Cfg *Func) const override {
    Type Ty = this->getSrc(0)->getType();
    assert(this->getSrcSize() == 2);
    emitIASAsAddrOpTyGPR<Machine>(Func, Ty, this->getSrc(0), this->getSrc(1),
                                  Emitter);
  }
  void dump(const Cfg *Func) const override {
    if (!BuildDefs::dump())
      return;
    Ostream &Str = Func->getContext()->getStrDump();
    Str << Opcode << "." << this->getSrc(0)->getType() << " ";
    this->dumpSources(Func);
  }
  static bool classof(const Inst *Inst) {
    return InstX86Base<Machine>::isClassof(Inst, InstX86Base<Machine>::K);
  }

protected:
  InstX86BaseBinopRMW(
      Cfg *Func, typename InstX86Base<Machine>::Traits::X86OperandMem *DestSrc0,
      Operand *Src1)
      : InstX86Base<Machine>(Func, K, 2, nullptr) {
    this->addSource(DestSrc0);
    this->addSource(Src1);
  }

  static const char *Opcode;
  static const typename InstX86Base<
      Machine>::Traits::Assembler::GPREmitterAddrOp Emitter;
};

template <class Machine, typename InstX86Base<Machine>::InstKindX86 K,
          bool NeedsElementType,
          typename InstX86Base<Machine>::SseSuffix Suffix>
class InstX86BaseBinopXmm : public InstX86Base<Machine> {
  InstX86BaseBinopXmm() = delete;
  InstX86BaseBinopXmm(const InstX86BaseBinopXmm &) = delete;
  InstX86BaseBinopXmm &operator=(const InstX86BaseBinopXmm &) = delete;

public:
  using Base = InstX86BaseBinopXmm<Machine, K, NeedsElementType, Suffix>;

  void emit(const Cfg *Func) const override {
    if (!BuildDefs::dump())
      return;
    this->validateVectorAddrMode();
    switch (Suffix) {
    case InstX86Base<Machine>::SseSuffix::None:
      this->emitTwoAddress(Func, Opcode);
      break;
    case InstX86Base<Machine>::SseSuffix::Packed: {
      const Type DestTy = this->getDest()->getType();
      this->emitTwoAddress(
          Func, this->Opcode,
          InstX86Base<Machine>::Traits::TypeAttributes[DestTy].PdPsString);
    } break;
    case InstX86Base<Machine>::SseSuffix::Scalar: {
      const Type DestTy = this->getDest()->getType();
      this->emitTwoAddress(
          Func, this->Opcode,
          InstX86Base<Machine>::Traits::TypeAttributes[DestTy].SdSsString);
    } break;
    case InstX86Base<Machine>::SseSuffix::Integral: {
      const Type DestTy = this->getDest()->getType();
      this->emitTwoAddress(
          Func, this->Opcode,
          InstX86Base<Machine>::Traits::TypeAttributes[DestTy].PackString);
    } break;
    }
  }
  void emitIAS(const Cfg *Func) const override {
    this->validateVectorAddrMode();
    Type Ty = this->getDest()->getType();
    if (NeedsElementType)
      Ty = typeElementType(Ty);
    assert(this->getSrcSize() == 2);
    emitIASRegOpTyXMM<Machine>(Func, Ty, this->getDest(), this->getSrc(1),
                               Emitter);
  }
  void dump(const Cfg *Func) const override {
    if (!BuildDefs::dump())
      return;
    Ostream &Str = Func->getContext()->getStrDump();
    this->dumpDest(Func);
    Str << " = " << Opcode << "." << this->getDest()->getType() << " ";
    this->dumpSources(Func);
  }
  static bool classof(const Inst *Inst) {
    return InstX86Base<Machine>::isClassof(Inst, InstX86Base<Machine>::K);
  }

protected:
  InstX86BaseBinopXmm(Cfg *Func, Variable *Dest, Operand *Source)
      : InstX86Base<Machine>(Func, K, 2, Dest) {
    this->addSource(Dest);
    this->addSource(Source);
  }

  static const char *Opcode;
  static const typename InstX86Base<Machine>::Traits::Assembler::XmmEmitterRegOp
      Emitter;
};

template <class Machine>
void emitIASXmmShift(
    const Cfg *Func, Type Ty, const Variable *Var, const Operand *Src,
    const typename InstX86Base<Machine>::Traits::Assembler::XmmEmitterShiftOp
        &Emitter);

template <class Machine, typename InstX86Base<Machine>::InstKindX86 K,
          bool AllowAllTypes = false>
class InstX86BaseBinopXmmShift : public InstX86Base<Machine> {
  InstX86BaseBinopXmmShift() = delete;
  InstX86BaseBinopXmmShift(const InstX86BaseBinopXmmShift &) = delete;
  InstX86BaseBinopXmmShift &
  operator=(const InstX86BaseBinopXmmShift &) = delete;

public:
  using Base = InstX86BaseBinopXmmShift<Machine, K, AllowAllTypes>;

  void emit(const Cfg *Func) const override {
    if (!BuildDefs::dump())
      return;
    this->validateVectorAddrMode();
    // Shift operations are always integral, and hence always need a suffix.
    const Type DestTy = this->getDest()->getType();
    this->emitTwoAddress(
        Func, this->Opcode,
        InstX86Base<Machine>::Traits::TypeAttributes[DestTy].PackString);
  }
  void emitIAS(const Cfg *Func) const override {
    this->validateVectorAddrMode();
    Type Ty = this->getDest()->getType();
    assert(AllowAllTypes || isVectorType(Ty));
    Type ElementTy = typeElementType(Ty);
    assert(this->getSrcSize() == 2);
    emitIASXmmShift<Machine>(Func, ElementTy, this->getDest(), this->getSrc(1),
                             Emitter);
  }
  void dump(const Cfg *Func) const override {
    if (!BuildDefs::dump())
      return;
    Ostream &Str = Func->getContext()->getStrDump();
    this->dumpDest(Func);
    Str << " = " << Opcode << "." << this->getDest()->getType() << " ";
    this->dumpSources(Func);
  }
  static bool classof(const Inst *Inst) {
    return InstX86Base<Machine>::isClassof(Inst, InstX86Base<Machine>::K);
  }

protected:
  InstX86BaseBinopXmmShift(Cfg *Func, Variable *Dest, Operand *Source)
      : InstX86Base<Machine>(Func, K, 2, Dest) {
    this->addSource(Dest);
    this->addSource(Source);
  }

  static const char *Opcode;
  static const typename InstX86Base<
      Machine>::Traits::Assembler::XmmEmitterShiftOp Emitter;
};

template <class Machine, typename InstX86Base<Machine>::InstKindX86 K>
class InstX86BaseTernop : public InstX86Base<Machine> {
  InstX86BaseTernop() = delete;
  InstX86BaseTernop(const InstX86BaseTernop &) = delete;
  InstX86BaseTernop &operator=(const InstX86BaseTernop &) = delete;

public:
  using Base = InstX86BaseTernop<Machine, K>;

  void emit(const Cfg *Func) const override {
    if (!BuildDefs::dump())
      return;
    Ostream &Str = Func->getContext()->getStrEmit();
    assert(this->getSrcSize() == 3);
    Str << "\t" << Opcode << "\t";
    this->getSrc(2)->emit(Func);
    Str << ", ";
    this->getSrc(1)->emit(Func);
    Str << ", ";
    this->getDest()->emit(Func);
  }
  void dump(const Cfg *Func) const override {
    if (!BuildDefs::dump())
      return;
    Ostream &Str = Func->getContext()->getStrDump();
    this->dumpDest(Func);
    Str << " = " << Opcode << "." << this->getDest()->getType() << " ";
    this->dumpSources(Func);
  }
  static bool classof(const Inst *Inst) {
    return InstX86Base<Machine>::isClassof(Inst, InstX86Base<Machine>::K);
  }

protected:
  InstX86BaseTernop(Cfg *Func, Variable *Dest, Operand *Source1,
                    Operand *Source2)
      : InstX86Base<Machine>(Func, K, 3, Dest) {
    this->addSource(Dest);
    this->addSource(Source1);
    this->addSource(Source2);
  }

  static const char *Opcode;
};

// Instructions of the form x := y op z
template <class Machine, typename InstX86Base<Machine>::InstKindX86 K>
class InstX86BaseThreeAddressop : public InstX86Base<Machine> {
  InstX86BaseThreeAddressop() = delete;
  InstX86BaseThreeAddressop(const InstX86BaseThreeAddressop &) = delete;
  InstX86BaseThreeAddressop &
  operator=(const InstX86BaseThreeAddressop &) = delete;

public:
  using Base = InstX86BaseThreeAddressop<Machine, K>;

  void emit(const Cfg *Func) const override {
    if (!BuildDefs::dump())
      return;
    Ostream &Str = Func->getContext()->getStrEmit();
    assert(this->getSrcSize() == 2);
    Str << "\t" << Opcode << "\t";
    this->getSrc(1)->emit(Func);
    Str << ", ";
    this->getSrc(0)->emit(Func);
    Str << ", ";
    this->getDest()->emit(Func);
  }
  void dump(const Cfg *Func) const override {
    if (!BuildDefs::dump())
      return;
    Ostream &Str = Func->getContext()->getStrDump();
    this->dumpDest(Func);
    Str << " = " << Opcode << "." << this->getDest()->getType() << " ";
    this->dumpSources(Func);
  }
  static bool classof(const Inst *Inst) {
    return InstX86Base<Machine>::isClassof(Inst, InstX86Base<Machine>::K);
  }

protected:
  InstX86BaseThreeAddressop(Cfg *Func, Variable *Dest, Operand *Source0,
                            Operand *Source1)
      : InstX86Base<Machine>(Func, K, 2, Dest) {
    this->addSource(Source0);
    this->addSource(Source1);
  }

  static const char *Opcode;
};

/// Base class for assignment instructions
template <class Machine, typename InstX86Base<Machine>::InstKindX86 K>
class InstX86BaseMovlike : public InstX86Base<Machine> {
  InstX86BaseMovlike() = delete;
  InstX86BaseMovlike(const InstX86BaseMovlike &) = delete;
  InstX86BaseMovlike &operator=(const InstX86BaseMovlike &) = delete;

public:
  using Base = InstX86BaseMovlike<Machine, K>;

  bool isRedundantAssign() const override {
    if (const auto *SrcVar = llvm::dyn_cast<const Variable>(this->getSrc(0))) {
      if (SrcVar->hasReg() && this->Dest->hasReg()) {
        // An assignment between physical registers is considered redundant if
        // they have the same base register and the same encoding. E.g.:
        //   mov cl, ecx ==> redundant
        //   mov ch, ecx ==> not redundant due to different encodings
        //   mov ch, ebp ==> not redundant due to different base registers
        // TODO(stichnot): Don't consider "mov eax, eax" to be redundant when
        // used in 64-bit mode to clear the upper half of rax.
        int32_t SrcReg = SrcVar->getRegNum();
        int32_t DestReg = this->Dest->getRegNum();
        return (InstX86Base<Machine>::Traits::getEncoding(SrcReg) ==
                InstX86Base<Machine>::Traits::getEncoding(DestReg)) &&
               (InstX86Base<Machine>::Traits::getBaseReg(SrcReg) ==
                InstX86Base<Machine>::Traits::getBaseReg(DestReg));
      }
    }
    return checkForRedundantAssign(this->getDest(), this->getSrc(0));
  }
  bool isVarAssign() const override {
    return llvm::isa<Variable>(this->getSrc(0));
  }
  void dump(const Cfg *Func) const override {
    if (!BuildDefs::dump())
      return;
    Ostream &Str = Func->getContext()->getStrDump();
    Str << Opcode << "." << this->getDest()->getType() << " ";
    this->dumpDest(Func);
    Str << ", ";
    this->dumpSources(Func);
  }
  static bool classof(const Inst *Inst) {
    return InstX86Base<Machine>::isClassof(Inst, InstX86Base<Machine>::K);
  }

protected:
  InstX86BaseMovlike(Cfg *Func, Variable *Dest, Operand *Source)
      : InstX86Base<Machine>(Func, K, 1, Dest) {
    this->addSource(Source);
    // For an integer assignment, make sure it's either a same-type assignment
    // or a truncation.
    assert(!isScalarIntegerType(Dest->getType()) ||
           (typeWidthInBytes(Dest->getType()) <=
            typeWidthInBytes(Source->getType())));
  }

  static const char *Opcode;
};

template <class Machine>
class InstX86Bswap
    : public InstX86BaseInplaceopGPR<Machine, InstX86Base<Machine>::Bswap> {
public:
  static InstX86Bswap *create(Cfg *Func, Operand *SrcDest) {
    return new (Func->allocate<InstX86Bswap>()) InstX86Bswap(Func, SrcDest);
  }

private:
  InstX86Bswap(Cfg *Func, Operand *SrcDest)
      : InstX86BaseInplaceopGPR<Machine, InstX86Base<Machine>::Bswap>(Func,
                                                                      SrcDest) {
  }
};

template <class Machine>
class InstX86Neg
    : public InstX86BaseInplaceopGPR<Machine, InstX86Base<Machine>::Neg> {
public:
  static InstX86Neg *create(Cfg *Func, Operand *SrcDest) {
    return new (Func->allocate<InstX86Neg>()) InstX86Neg(Func, SrcDest);
  }

private:
  InstX86Neg(Cfg *Func, Operand *SrcDest)
      : InstX86BaseInplaceopGPR<Machine, InstX86Base<Machine>::Neg>(Func,
                                                                    SrcDest) {}
};

template <class Machine>
class InstX86Bsf
    : public InstX86BaseUnaryopGPR<Machine, InstX86Base<Machine>::Bsf> {
public:
  static InstX86Bsf *create(Cfg *Func, Variable *Dest, Operand *Src) {
    return new (Func->allocate<InstX86Bsf>()) InstX86Bsf(Func, Dest, Src);
  }

private:
  InstX86Bsf(Cfg *Func, Variable *Dest, Operand *Src)
      : InstX86BaseUnaryopGPR<Machine, InstX86Base<Machine>::Bsf>(Func, Dest,
                                                                  Src) {}
};

template <class Machine>
class InstX86Bsr
    : public InstX86BaseUnaryopGPR<Machine, InstX86Base<Machine>::Bsr> {
public:
  static InstX86Bsr *create(Cfg *Func, Variable *Dest, Operand *Src) {
    return new (Func->allocate<InstX86Bsr>()) InstX86Bsr(Func, Dest, Src);
  }

private:
  InstX86Bsr(Cfg *Func, Variable *Dest, Operand *Src)
      : InstX86BaseUnaryopGPR<Machine, InstX86Base<Machine>::Bsr>(Func, Dest,
                                                                  Src) {}
};

template <class Machine>
class InstX86Lea
    : public InstX86BaseUnaryopGPR<Machine, InstX86Base<Machine>::Lea> {
public:
  static InstX86Lea *create(Cfg *Func, Variable *Dest, Operand *Src) {
    return new (Func->allocate<InstX86Lea>()) InstX86Lea(Func, Dest, Src);
  }

  void emit(const Cfg *Func) const override;

private:
  InstX86Lea(Cfg *Func, Variable *Dest, Operand *Src)
      : InstX86BaseUnaryopGPR<Machine, InstX86Base<Machine>::Lea>(Func, Dest,
                                                                  Src) {}
};

// Cbwdq instruction - wrapper for cbw, cwd, and cdq
template <class Machine>
class InstX86Cbwdq
    : public InstX86BaseUnaryopGPR<Machine, InstX86Base<Machine>::Cbwdq> {
public:
  static InstX86Cbwdq *create(Cfg *Func, Variable *Dest, Operand *Src) {
    return new (Func->allocate<InstX86Cbwdq>()) InstX86Cbwdq(Func, Dest, Src);
  }

  void emit(const Cfg *Func) const override;
  void emitIAS(const Cfg *Func) const override;

private:
  InstX86Cbwdq(Cfg *Func, Variable *Dest, Operand *Src)
      : InstX86BaseUnaryopGPR<Machine, InstX86Base<Machine>::Cbwdq>(Func, Dest,
                                                                    Src) {}
};

template <class Machine>
class InstX86Movsx
    : public InstX86BaseUnaryopGPR<Machine, InstX86Base<Machine>::Movsx> {
public:
  static InstX86Movsx *create(Cfg *Func, Variable *Dest, Operand *Src) {
    assert(typeWidthInBytes(Dest->getType()) >
           typeWidthInBytes(Src->getType()));
    return new (Func->allocate<InstX86Movsx>()) InstX86Movsx(Func, Dest, Src);
  }

  void emitIAS(const Cfg *Func) const override;

private:
  InstX86Movsx(Cfg *Func, Variable *Dest, Operand *Src)
      : InstX86BaseUnaryopGPR<Machine, InstX86Base<Machine>::Movsx>(Func, Dest,
                                                                    Src) {}
};

template <class Machine>
class InstX86Movzx
    : public InstX86BaseUnaryopGPR<Machine, InstX86Base<Machine>::Movzx> {
public:
  static InstX86Movzx *create(Cfg *Func, Variable *Dest, Operand *Src) {
    assert(typeWidthInBytes(Dest->getType()) >
           typeWidthInBytes(Src->getType()));
    return new (Func->allocate<InstX86Movzx>()) InstX86Movzx(Func, Dest, Src);
  }

  void emitIAS(const Cfg *Func) const override;

private:
  InstX86Movzx(Cfg *Func, Variable *Dest, Operand *Src)
      : InstX86BaseUnaryopGPR<Machine, InstX86Base<Machine>::Movzx>(Func, Dest,
                                                                    Src) {}
};

template <class Machine>
class InstX86Movd
    : public InstX86BaseUnaryopXmm<Machine, InstX86Base<Machine>::Movd> {
public:
  static InstX86Movd *create(Cfg *Func, Variable *Dest, Operand *Src) {
    return new (Func->allocate<InstX86Movd>()) InstX86Movd(Func, Dest, Src);
  }

  void emitIAS(const Cfg *Func) const override;

private:
  InstX86Movd(Cfg *Func, Variable *Dest, Operand *Src)
      : InstX86BaseUnaryopXmm<Machine, InstX86Base<Machine>::Movd>(Func, Dest,
                                                                   Src) {}
};

template <class Machine>
class InstX86Sqrtss
    : public InstX86BaseUnaryopXmm<Machine, InstX86Base<Machine>::Sqrtss> {
public:
  static InstX86Sqrtss *create(Cfg *Func, Variable *Dest, Operand *Src) {
    return new (Func->allocate<InstX86Sqrtss>()) InstX86Sqrtss(Func, Dest, Src);
  }

  virtual void emit(const Cfg *Func) const override;

private:
  InstX86Sqrtss(Cfg *Func, Variable *Dest, Operand *Src)
      : InstX86BaseUnaryopXmm<Machine, InstX86Base<Machine>::Sqrtss>(Func, Dest,
                                                                     Src) {}
};

/// Move/assignment instruction - wrapper for mov/movss/movsd.
template <class Machine>
class InstX86Mov
    : public InstX86BaseMovlike<Machine, InstX86Base<Machine>::Mov> {
public:
  static InstX86Mov *create(Cfg *Func, Variable *Dest, Operand *Source) {
    return new (Func->allocate<InstX86Mov>()) InstX86Mov(Func, Dest, Source);
  }

  void emit(const Cfg *Func) const override;
  void emitIAS(const Cfg *Func) const override;

private:
  InstX86Mov(Cfg *Func, Variable *Dest, Operand *Source)
      : InstX86BaseMovlike<Machine, InstX86Base<Machine>::Mov>(Func, Dest,
                                                               Source) {}
};

/// Move packed - copy 128 bit values between XMM registers, or mem128 and XMM
/// registers.
template <class Machine>
class InstX86Movp
    : public InstX86BaseMovlike<Machine, InstX86Base<Machine>::Movp> {
public:
  static InstX86Movp *create(Cfg *Func, Variable *Dest, Operand *Source) {
    return new (Func->allocate<InstX86Movp>()) InstX86Movp(Func, Dest, Source);
  }

  void emit(const Cfg *Func) const override;
  void emitIAS(const Cfg *Func) const override;

private:
  InstX86Movp(Cfg *Func, Variable *Dest, Operand *Source)
      : InstX86BaseMovlike<Machine, InstX86Base<Machine>::Movp>(Func, Dest,
                                                                Source) {}
};

/// Movq - copy between XMM registers, or mem64 and XMM registers.
template <class Machine>
class InstX86Movq
    : public InstX86BaseMovlike<Machine, InstX86Base<Machine>::Movq> {
public:
  static InstX86Movq *create(Cfg *Func, Variable *Dest, Operand *Source) {
    return new (Func->allocate<InstX86Movq>()) InstX86Movq(Func, Dest, Source);
  }

  void emit(const Cfg *Func) const override;
  void emitIAS(const Cfg *Func) const override;

private:
  InstX86Movq(Cfg *Func, Variable *Dest, Operand *Source)
      : InstX86BaseMovlike<Machine, InstX86Base<Machine>::Movq>(Func, Dest,
                                                                Source) {}
};

template <class Machine>
class InstX86Add
    : public InstX86BaseBinopGPR<Machine, InstX86Base<Machine>::Add> {
public:
  static InstX86Add *create(Cfg *Func, Variable *Dest, Operand *Source) {
    return new (Func->allocate<InstX86Add>()) InstX86Add(Func, Dest, Source);
  }

private:
  InstX86Add(Cfg *Func, Variable *Dest, Operand *Source)
      : InstX86BaseBinopGPR<Machine, InstX86Base<Machine>::Add>(Func, Dest,
                                                                Source) {}
};

template <class Machine>
class InstX86AddRMW
    : public InstX86BaseBinopRMW<Machine, InstX86Base<Machine>::AddRMW> {
public:
  static InstX86AddRMW *
  create(Cfg *Func,
         typename InstX86Base<Machine>::Traits::X86OperandMem *DestSrc0,
         Operand *Src1) {
    return new (Func->allocate<InstX86AddRMW>())
        InstX86AddRMW(Func, DestSrc0, Src1);
  }

private:
  InstX86AddRMW(Cfg *Func,
                typename InstX86Base<Machine>::Traits::X86OperandMem *DestSrc0,
                Operand *Src1)
      : InstX86BaseBinopRMW<Machine, InstX86Base<Machine>::AddRMW>(
            Func, DestSrc0, Src1) {}
};

template <class Machine>
class InstX86Addps
    : public InstX86BaseBinopXmm<Machine, InstX86Base<Machine>::Addps, true,
                                 InstX86Base<Machine>::SseSuffix::Packed> {
public:
  static InstX86Addps *create(Cfg *Func, Variable *Dest, Operand *Source) {
    return new (Func->allocate<InstX86Addps>())
        InstX86Addps(Func, Dest, Source);
  }

private:
  InstX86Addps(Cfg *Func, Variable *Dest, Operand *Source)
      : InstX86BaseBinopXmm<Machine, InstX86Base<Machine>::Addps, true,
                            InstX86Base<Machine>::SseSuffix::Packed>(Func, Dest,
                                                                     Source) {}
};

template <class Machine>
class InstX86Adc
    : public InstX86BaseBinopGPR<Machine, InstX86Base<Machine>::Adc> {
public:
  static InstX86Adc *create(Cfg *Func, Variable *Dest, Operand *Source) {
    return new (Func->allocate<InstX86Adc>()) InstX86Adc(Func, Dest, Source);
  }

private:
  InstX86Adc(Cfg *Func, Variable *Dest, Operand *Source)
      : InstX86BaseBinopGPR<Machine, InstX86Base<Machine>::Adc>(Func, Dest,
                                                                Source) {}
};

template <class Machine>
class InstX86AdcRMW
    : public InstX86BaseBinopRMW<Machine, InstX86Base<Machine>::AdcRMW> {
public:
  static InstX86AdcRMW *
  create(Cfg *Func,
         typename InstX86Base<Machine>::Traits::X86OperandMem *DestSrc0,
         Operand *Src1) {
    return new (Func->allocate<InstX86AdcRMW>())
        InstX86AdcRMW(Func, DestSrc0, Src1);
  }

private:
  InstX86AdcRMW(Cfg *Func,
                typename InstX86Base<Machine>::Traits::X86OperandMem *DestSrc0,
                Operand *Src1)
      : InstX86BaseBinopRMW<Machine, InstX86Base<Machine>::AdcRMW>(
            Func, DestSrc0, Src1) {}
};

template <class Machine>
class InstX86Addss
    : public InstX86BaseBinopXmm<Machine, InstX86Base<Machine>::Addss, false,
                                 InstX86Base<Machine>::SseSuffix::Scalar> {
public:
  static InstX86Addss *create(Cfg *Func, Variable *Dest, Operand *Source) {
    return new (Func->allocate<InstX86Addss>())
        InstX86Addss(Func, Dest, Source);
  }

private:
  InstX86Addss(Cfg *Func, Variable *Dest, Operand *Source)
      : InstX86BaseBinopXmm<Machine, InstX86Base<Machine>::Addss, false,
                            InstX86Base<Machine>::SseSuffix::Scalar>(Func, Dest,
                                                                     Source) {}
};

template <class Machine>
class InstX86Padd
    : public InstX86BaseBinopXmm<Machine, InstX86Base<Machine>::Padd, true,
                                 InstX86Base<Machine>::SseSuffix::Integral> {
public:
  static InstX86Padd *create(Cfg *Func, Variable *Dest, Operand *Source) {
    return new (Func->allocate<InstX86Padd>()) InstX86Padd(Func, Dest, Source);
  }

private:
  InstX86Padd(Cfg *Func, Variable *Dest, Operand *Source)
      : InstX86BaseBinopXmm<Machine, InstX86Base<Machine>::Padd, true,
                            InstX86Base<Machine>::SseSuffix::Integral>(
            Func, Dest, Source) {}
};

template <class Machine>
class InstX86Sub
    : public InstX86BaseBinopGPR<Machine, InstX86Base<Machine>::Sub> {
public:
  static InstX86Sub *create(Cfg *Func, Variable *Dest, Operand *Source) {
    return new (Func->allocate<InstX86Sub>()) InstX86Sub(Func, Dest, Source);
  }

private:
  InstX86Sub(Cfg *Func, Variable *Dest, Operand *Source)
      : InstX86BaseBinopGPR<Machine, InstX86Base<Machine>::Sub>(Func, Dest,
                                                                Source) {}
};

template <class Machine>
class InstX86SubRMW
    : public InstX86BaseBinopRMW<Machine, InstX86Base<Machine>::SubRMW> {
public:
  static InstX86SubRMW *
  create(Cfg *Func,
         typename InstX86Base<Machine>::Traits::X86OperandMem *DestSrc0,
         Operand *Src1) {
    return new (Func->allocate<InstX86SubRMW>())
        InstX86SubRMW(Func, DestSrc0, Src1);
  }

private:
  InstX86SubRMW(Cfg *Func,
                typename InstX86Base<Machine>::Traits::X86OperandMem *DestSrc0,
                Operand *Src1)
      : InstX86BaseBinopRMW<Machine, InstX86Base<Machine>::SubRMW>(
            Func, DestSrc0, Src1) {}
};

template <class Machine>
class InstX86Subps
    : public InstX86BaseBinopXmm<Machine, InstX86Base<Machine>::Subps, true,
                                 InstX86Base<Machine>::SseSuffix::Packed> {
public:
  static InstX86Subps *create(Cfg *Func, Variable *Dest, Operand *Source) {
    return new (Func->allocate<InstX86Subps>())
        InstX86Subps(Func, Dest, Source);
  }

private:
  InstX86Subps(Cfg *Func, Variable *Dest, Operand *Source)
      : InstX86BaseBinopXmm<Machine, InstX86Base<Machine>::Subps, true,
                            InstX86Base<Machine>::SseSuffix::Packed>(Func, Dest,
                                                                     Source) {}
};

template <class Machine>
class InstX86Subss
    : public InstX86BaseBinopXmm<Machine, InstX86Base<Machine>::Subss, false,
                                 InstX86Base<Machine>::SseSuffix::Scalar> {
public:
  static InstX86Subss *create(Cfg *Func, Variable *Dest, Operand *Source) {
    return new (Func->allocate<InstX86Subss>())
        InstX86Subss(Func, Dest, Source);
  }

private:
  InstX86Subss(Cfg *Func, Variable *Dest, Operand *Source)
      : InstX86BaseBinopXmm<Machine, InstX86Base<Machine>::Subss, false,
                            InstX86Base<Machine>::SseSuffix::Scalar>(Func, Dest,
                                                                     Source) {}
};

template <class Machine>
class InstX86Sbb
    : public InstX86BaseBinopGPR<Machine, InstX86Base<Machine>::Sbb> {
public:
  static InstX86Sbb *create(Cfg *Func, Variable *Dest, Operand *Source) {
    return new (Func->allocate<InstX86Sbb>()) InstX86Sbb(Func, Dest, Source);
  }

private:
  InstX86Sbb(Cfg *Func, Variable *Dest, Operand *Source)
      : InstX86BaseBinopGPR<Machine, InstX86Base<Machine>::Sbb>(Func, Dest,
                                                                Source) {}
};

template <class Machine>
class InstX86SbbRMW
    : public InstX86BaseBinopRMW<Machine, InstX86Base<Machine>::SbbRMW> {
public:
  static InstX86SbbRMW *
  create(Cfg *Func,
         typename InstX86Base<Machine>::Traits::X86OperandMem *DestSrc0,
         Operand *Src1) {
    return new (Func->allocate<InstX86SbbRMW>())
        InstX86SbbRMW(Func, DestSrc0, Src1);
  }

private:
  InstX86SbbRMW(Cfg *Func,
                typename InstX86Base<Machine>::Traits::X86OperandMem *DestSrc0,
                Operand *Src1)
      : InstX86BaseBinopRMW<Machine, InstX86Base<Machine>::SbbRMW>(
            Func, DestSrc0, Src1) {}
};

template <class Machine>
class InstX86Psub
    : public InstX86BaseBinopXmm<Machine, InstX86Base<Machine>::Psub, true,
                                 InstX86Base<Machine>::SseSuffix::Integral> {
public:
  static InstX86Psub *create(Cfg *Func, Variable *Dest, Operand *Source) {
    return new (Func->allocate<InstX86Psub>()) InstX86Psub(Func, Dest, Source);
  }

private:
  InstX86Psub(Cfg *Func, Variable *Dest, Operand *Source)
      : InstX86BaseBinopXmm<Machine, InstX86Base<Machine>::Psub, true,
                            InstX86Base<Machine>::SseSuffix::Integral>(
            Func, Dest, Source) {}
};

template <class Machine>
class InstX86And
    : public InstX86BaseBinopGPR<Machine, InstX86Base<Machine>::And> {
public:
  static InstX86And *create(Cfg *Func, Variable *Dest, Operand *Source) {
    return new (Func->allocate<InstX86And>()) InstX86And(Func, Dest, Source);
  }

private:
  InstX86And(Cfg *Func, Variable *Dest, Operand *Source)
      : InstX86BaseBinopGPR<Machine, InstX86Base<Machine>::And>(Func, Dest,
                                                                Source) {}
};

template <class Machine>
class InstX86Andnps
    : public InstX86BaseBinopXmm<Machine, InstX86Base<Machine>::Andnps, true,
                                 InstX86Base<Machine>::SseSuffix::Packed> {
public:
  static InstX86Andnps *create(Cfg *Func, Variable *Dest, Operand *Source) {
    return new (Func->allocate<InstX86Andnps>())
        InstX86Andnps(Func, Dest, Source);
  }

private:
  InstX86Andnps(Cfg *Func, Variable *Dest, Operand *Source)
      : InstX86BaseBinopXmm<Machine, InstX86Base<Machine>::Andnps, true,
                            InstX86Base<Machine>::SseSuffix::Packed>(Func, Dest,
                                                                     Source) {}
};

template <class Machine>
class InstX86Andps
    : public InstX86BaseBinopXmm<Machine, InstX86Base<Machine>::Andps, true,
                                 InstX86Base<Machine>::SseSuffix::Packed> {
public:
  static InstX86Andps *create(Cfg *Func, Variable *Dest, Operand *Source) {
    return new (Func->allocate<InstX86Andps>())
        InstX86Andps(Func, Dest, Source);
  }

private:
  InstX86Andps(Cfg *Func, Variable *Dest, Operand *Source)
      : InstX86BaseBinopXmm<Machine, InstX86Base<Machine>::Andps, true,
                            InstX86Base<Machine>::SseSuffix::Packed>(Func, Dest,
                                                                     Source) {}
};

template <class Machine>
class InstX86AndRMW
    : public InstX86BaseBinopRMW<Machine, InstX86Base<Machine>::AndRMW> {
public:
  static InstX86AndRMW *
  create(Cfg *Func,
         typename InstX86Base<Machine>::Traits::X86OperandMem *DestSrc0,
         Operand *Src1) {
    return new (Func->allocate<InstX86AndRMW>())
        InstX86AndRMW(Func, DestSrc0, Src1);
  }

private:
  InstX86AndRMW(Cfg *Func,
                typename InstX86Base<Machine>::Traits::X86OperandMem *DestSrc0,
                Operand *Src1)
      : InstX86BaseBinopRMW<Machine, InstX86Base<Machine>::AndRMW>(
            Func, DestSrc0, Src1) {}
};

template <class Machine>
class InstX86Pand
    : public InstX86BaseBinopXmm<Machine, InstX86Base<Machine>::Pand, false,
                                 InstX86Base<Machine>::SseSuffix::None> {
public:
  static InstX86Pand *create(Cfg *Func, Variable *Dest, Operand *Source) {
    return new (Func->allocate<InstX86Pand>()) InstX86Pand(Func, Dest, Source);
  }

private:
  InstX86Pand(Cfg *Func, Variable *Dest, Operand *Source)
      : InstX86BaseBinopXmm<Machine, InstX86Base<Machine>::Pand, false,
                            InstX86Base<Machine>::SseSuffix::None>(Func, Dest,
                                                                   Source) {}
};

template <class Machine>
class InstX86Pandn
    : public InstX86BaseBinopXmm<Machine, InstX86Base<Machine>::Pandn, false,
                                 InstX86Base<Machine>::SseSuffix::None> {
public:
  static InstX86Pandn *create(Cfg *Func, Variable *Dest, Operand *Source) {
    return new (Func->allocate<InstX86Pandn>())
        InstX86Pandn(Func, Dest, Source);
  }

private:
  InstX86Pandn(Cfg *Func, Variable *Dest, Operand *Source)
      : InstX86BaseBinopXmm<Machine, InstX86Base<Machine>::Pandn, false,
                            InstX86Base<Machine>::SseSuffix::None>(Func, Dest,
                                                                   Source) {}
};

template <class Machine>
class InstX86Maxss
    : public InstX86BaseBinopXmm<Machine, InstX86Base<Machine>::Maxss, true,
                                 InstX86Base<Machine>::SseSuffix::Scalar> {
public:
  static InstX86Maxss *create(Cfg *Func, Variable *Dest, Operand *Source) {
    return new (Func->allocate<InstX86Maxss>())
        InstX86Maxss(Func, Dest, Source);
  }

private:
  InstX86Maxss(Cfg *Func, Variable *Dest, Operand *Source)
      : InstX86BaseBinopXmm<Machine, InstX86Base<Machine>::Maxss, true,
                            InstX86Base<Machine>::SseSuffix::Scalar>(Func, Dest,
                                                                     Source) {}
};

template <class Machine>
class InstX86Minss
    : public InstX86BaseBinopXmm<Machine, InstX86Base<Machine>::Minss, true,
                                 InstX86Base<Machine>::SseSuffix::Scalar> {
public:
  static InstX86Minss *create(Cfg *Func, Variable *Dest, Operand *Source) {
    return new (Func->allocate<InstX86Minss>())
        InstX86Minss(Func, Dest, Source);
  }

private:
  InstX86Minss(Cfg *Func, Variable *Dest, Operand *Source)
      : InstX86BaseBinopXmm<Machine, InstX86Base<Machine>::Minss, true,
                            InstX86Base<Machine>::SseSuffix::Scalar>(Func, Dest,
                                                                     Source) {}
};

template <class Machine>
class InstX86Or
    : public InstX86BaseBinopGPR<Machine, InstX86Base<Machine>::Or> {
public:
  static InstX86Or *create(Cfg *Func, Variable *Dest, Operand *Source) {
    return new (Func->allocate<InstX86Or>()) InstX86Or(Func, Dest, Source);
  }

private:
  InstX86Or(Cfg *Func, Variable *Dest, Operand *Source)
      : InstX86BaseBinopGPR<Machine, InstX86Base<Machine>::Or>(Func, Dest,
                                                               Source) {}
};

template <class Machine>
class InstX86Orps
    : public InstX86BaseBinopXmm<Machine, InstX86Base<Machine>::Orps, true,
                                 InstX86Base<Machine>::SseSuffix::Packed> {
public:
  static InstX86Orps *create(Cfg *Func, Variable *Dest, Operand *Source) {
    return new (Func->allocate<InstX86Orps>()) InstX86Orps(Func, Dest, Source);
  }

private:
  InstX86Orps(Cfg *Func, Variable *Dest, Operand *Source)
      : InstX86BaseBinopXmm<Machine, InstX86Base<Machine>::Orps, true,
                            InstX86Base<Machine>::SseSuffix::Packed>(Func, Dest,
                                                                     Source) {}
};

template <class Machine>
class InstX86OrRMW
    : public InstX86BaseBinopRMW<Machine, InstX86Base<Machine>::OrRMW> {
public:
  static InstX86OrRMW *
  create(Cfg *Func,
         typename InstX86Base<Machine>::Traits::X86OperandMem *DestSrc0,
         Operand *Src1) {
    return new (Func->allocate<InstX86OrRMW>())
        InstX86OrRMW(Func, DestSrc0, Src1);
  }

private:
  InstX86OrRMW(Cfg *Func,
               typename InstX86Base<Machine>::Traits::X86OperandMem *DestSrc0,
               Operand *Src1)
      : InstX86BaseBinopRMW<Machine, InstX86Base<Machine>::OrRMW>(
            Func, DestSrc0, Src1) {}
};

template <class Machine>
class InstX86Por
    : public InstX86BaseBinopXmm<Machine, InstX86Base<Machine>::Por, false,
                                 InstX86Base<Machine>::SseSuffix::None> {
public:
  static InstX86Por *create(Cfg *Func, Variable *Dest, Operand *Source) {
    return new (Func->allocate<InstX86Por>()) InstX86Por(Func, Dest, Source);
  }

private:
  InstX86Por(Cfg *Func, Variable *Dest, Operand *Source)
      : InstX86BaseBinopXmm<Machine, InstX86Base<Machine>::Por, false,
                            InstX86Base<Machine>::SseSuffix::None>(Func, Dest,
                                                                   Source) {}
};

template <class Machine>
class InstX86Xor
    : public InstX86BaseBinopGPR<Machine, InstX86Base<Machine>::Xor> {
public:
  static InstX86Xor *create(Cfg *Func, Variable *Dest, Operand *Source) {
    return new (Func->allocate<InstX86Xor>()) InstX86Xor(Func, Dest, Source);
  }

private:
  InstX86Xor(Cfg *Func, Variable *Dest, Operand *Source)
      : InstX86BaseBinopGPR<Machine, InstX86Base<Machine>::Xor>(Func, Dest,
                                                                Source) {}
};

template <class Machine>
class InstX86Xorps
    : public InstX86BaseBinopXmm<Machine, InstX86Base<Machine>::Xorps, true,
                                 InstX86Base<Machine>::SseSuffix::Packed> {
public:
  static InstX86Xorps *create(Cfg *Func, Variable *Dest, Operand *Source) {
    return new (Func->allocate<InstX86Xorps>())
        InstX86Xorps(Func, Dest, Source);
  }

private:
  InstX86Xorps(Cfg *Func, Variable *Dest, Operand *Source)
      : InstX86BaseBinopXmm<Machine, InstX86Base<Machine>::Xorps, true,
                            InstX86Base<Machine>::SseSuffix::Packed>(Func, Dest,
                                                                     Source) {}
};

template <class Machine>
class InstX86XorRMW
    : public InstX86BaseBinopRMW<Machine, InstX86Base<Machine>::XorRMW> {
public:
  static InstX86XorRMW *
  create(Cfg *Func,
         typename InstX86Base<Machine>::Traits::X86OperandMem *DestSrc0,
         Operand *Src1) {
    return new (Func->allocate<InstX86XorRMW>())
        InstX86XorRMW(Func, DestSrc0, Src1);
  }

private:
  InstX86XorRMW(Cfg *Func,
                typename InstX86Base<Machine>::Traits::X86OperandMem *DestSrc0,
                Operand *Src1)
      : InstX86BaseBinopRMW<Machine, InstX86Base<Machine>::XorRMW>(
            Func, DestSrc0, Src1) {}
};

template <class Machine>
class InstX86Pxor
    : public InstX86BaseBinopXmm<Machine, InstX86Base<Machine>::Pxor, false,
                                 InstX86Base<Machine>::SseSuffix::None> {
public:
  static InstX86Pxor *create(Cfg *Func, Variable *Dest, Operand *Source) {
    return new (Func->allocate<InstX86Pxor>()) InstX86Pxor(Func, Dest, Source);
  }

private:
  InstX86Pxor(Cfg *Func, Variable *Dest, Operand *Source)
      : InstX86BaseBinopXmm<Machine, InstX86Base<Machine>::Pxor, false,
                            InstX86Base<Machine>::SseSuffix::None>(Func, Dest,
                                                                   Source) {}
};

template <class Machine>
class InstX86Imul
    : public InstX86BaseBinopGPR<Machine, InstX86Base<Machine>::Imul> {
public:
  static InstX86Imul *create(Cfg *Func, Variable *Dest, Operand *Source) {
    return new (Func->allocate<InstX86Imul>()) InstX86Imul(Func, Dest, Source);
  }

  void emit(const Cfg *Func) const override;
  void emitIAS(const Cfg *Func) const override;

private:
  InstX86Imul(Cfg *Func, Variable *Dest, Operand *Source)
      : InstX86BaseBinopGPR<Machine, InstX86Base<Machine>::Imul>(Func, Dest,
                                                                 Source) {}
};

template <class Machine>
class InstX86ImulImm
    : public InstX86BaseThreeAddressop<Machine, InstX86Base<Machine>::ImulImm> {
public:
  static InstX86ImulImm *create(Cfg *Func, Variable *Dest, Operand *Source0,
                                Operand *Source1) {
    return new (Func->allocate<InstX86ImulImm>())
        InstX86ImulImm(Func, Dest, Source0, Source1);
  }

  void emit(const Cfg *Func) const override;
  void emitIAS(const Cfg *Func) const override;

private:
  InstX86ImulImm(Cfg *Func, Variable *Dest, Operand *Source0, Operand *Source1)
      : InstX86BaseThreeAddressop<Machine, InstX86Base<Machine>::ImulImm>(
            Func, Dest, Source0, Source1) {}
};

template <class Machine>
class InstX86Mulps
    : public InstX86BaseBinopXmm<Machine, InstX86Base<Machine>::Mulps, true,
                                 InstX86Base<Machine>::SseSuffix::Packed> {
public:
  static InstX86Mulps *create(Cfg *Func, Variable *Dest, Operand *Source) {
    return new (Func->allocate<InstX86Mulps>())
        InstX86Mulps(Func, Dest, Source);
  }

private:
  InstX86Mulps(Cfg *Func, Variable *Dest, Operand *Source)
      : InstX86BaseBinopXmm<Machine, InstX86Base<Machine>::Mulps, true,
                            InstX86Base<Machine>::SseSuffix::Packed>(Func, Dest,
                                                                     Source) {}
};

template <class Machine>
class InstX86Mulss
    : public InstX86BaseBinopXmm<Machine, InstX86Base<Machine>::Mulss, false,
                                 InstX86Base<Machine>::SseSuffix::Scalar> {
public:
  static InstX86Mulss *create(Cfg *Func, Variable *Dest, Operand *Source) {
    return new (Func->allocate<InstX86Mulss>())
        InstX86Mulss(Func, Dest, Source);
  }

private:
  InstX86Mulss(Cfg *Func, Variable *Dest, Operand *Source)
      : InstX86BaseBinopXmm<Machine, InstX86Base<Machine>::Mulss, false,
                            InstX86Base<Machine>::SseSuffix::Scalar>(Func, Dest,
                                                                     Source) {}
};

template <class Machine>
class InstX86Pmull
    : public InstX86BaseBinopXmm<Machine, InstX86Base<Machine>::Pmull, true,
                                 InstX86Base<Machine>::SseSuffix::Integral> {
public:
  static InstX86Pmull *create(Cfg *Func, Variable *Dest, Operand *Source) {
    bool TypesAreValid =
        Dest->getType() == IceType_v4i32 || Dest->getType() == IceType_v8i16;
    auto *Target = InstX86Base<Machine>::getTarget(Func);
    bool InstructionSetIsValid =
        Dest->getType() == IceType_v8i16 ||
        Target->getInstructionSet() >= InstX86Base<Machine>::Traits::SSE4_1;
    (void)TypesAreValid;
    (void)InstructionSetIsValid;
    assert(TypesAreValid);
    assert(InstructionSetIsValid);
    return new (Func->allocate<InstX86Pmull>())
        InstX86Pmull(Func, Dest, Source);
  }

private:
  InstX86Pmull(Cfg *Func, Variable *Dest, Operand *Source)
      : InstX86BaseBinopXmm<Machine, InstX86Base<Machine>::Pmull, true,
                            InstX86Base<Machine>::SseSuffix::Integral>(
            Func, Dest, Source) {}
};

template <class Machine>
class InstX86Pmuludq
    : public InstX86BaseBinopXmm<Machine, InstX86Base<Machine>::Pmuludq, false,
                                 InstX86Base<Machine>::SseSuffix::None> {
public:
  static InstX86Pmuludq *create(Cfg *Func, Variable *Dest, Operand *Source) {
    assert(Dest->getType() == IceType_v4i32 &&
           Source->getType() == IceType_v4i32);
    return new (Func->allocate<InstX86Pmuludq>())
        InstX86Pmuludq(Func, Dest, Source);
  }

private:
  InstX86Pmuludq(Cfg *Func, Variable *Dest, Operand *Source)
      : InstX86BaseBinopXmm<Machine, InstX86Base<Machine>::Pmuludq, false,
                            InstX86Base<Machine>::SseSuffix::None>(Func, Dest,
                                                                   Source) {}
};

template <class Machine>
class InstX86Divps
    : public InstX86BaseBinopXmm<Machine, InstX86Base<Machine>::Divps, true,
                                 InstX86Base<Machine>::SseSuffix::Packed> {
public:
  static InstX86Divps *create(Cfg *Func, Variable *Dest, Operand *Source) {
    return new (Func->allocate<InstX86Divps>())
        InstX86Divps(Func, Dest, Source);
  }

private:
  InstX86Divps(Cfg *Func, Variable *Dest, Operand *Source)
      : InstX86BaseBinopXmm<Machine, InstX86Base<Machine>::Divps, true,
                            InstX86Base<Machine>::SseSuffix::Packed>(Func, Dest,
                                                                     Source) {}
};

template <class Machine>
class InstX86Divss
    : public InstX86BaseBinopXmm<Machine, InstX86Base<Machine>::Divss, false,
                                 InstX86Base<Machine>::SseSuffix::Scalar> {
public:
  static InstX86Divss *create(Cfg *Func, Variable *Dest, Operand *Source) {
    return new (Func->allocate<InstX86Divss>())
        InstX86Divss(Func, Dest, Source);
  }

private:
  InstX86Divss(Cfg *Func, Variable *Dest, Operand *Source)
      : InstX86BaseBinopXmm<Machine, InstX86Base<Machine>::Divss, false,
                            InstX86Base<Machine>::SseSuffix::Scalar>(Func, Dest,
                                                                     Source) {}
};

template <class Machine>
class InstX86Rol
    : public InstX86BaseBinopGPRShift<Machine, InstX86Base<Machine>::Rol> {
public:
  static InstX86Rol *create(Cfg *Func, Variable *Dest, Operand *Source) {
    return new (Func->allocate<InstX86Rol>()) InstX86Rol(Func, Dest, Source);
  }

private:
  InstX86Rol(Cfg *Func, Variable *Dest, Operand *Source)
      : InstX86BaseBinopGPRShift<Machine, InstX86Base<Machine>::Rol>(Func, Dest,
                                                                     Source) {}
};

template <class Machine>
class InstX86Shl
    : public InstX86BaseBinopGPRShift<Machine, InstX86Base<Machine>::Shl> {
public:
  static InstX86Shl *create(Cfg *Func, Variable *Dest, Operand *Source) {
    return new (Func->allocate<InstX86Shl>()) InstX86Shl(Func, Dest, Source);
  }

private:
  InstX86Shl(Cfg *Func, Variable *Dest, Operand *Source)
      : InstX86BaseBinopGPRShift<Machine, InstX86Base<Machine>::Shl>(Func, Dest,
                                                                     Source) {}
};

template <class Machine>
class InstX86Psll
    : public InstX86BaseBinopXmmShift<Machine, InstX86Base<Machine>::Psll> {
public:
  static InstX86Psll *create(Cfg *Func, Variable *Dest, Operand *Source) {
    assert(Dest->getType() == IceType_v8i16 ||
           Dest->getType() == IceType_v8i1 ||
           Dest->getType() == IceType_v4i32 || Dest->getType() == IceType_v4i1);
    return new (Func->allocate<InstX86Psll>()) InstX86Psll(Func, Dest, Source);
  }

private:
  InstX86Psll(Cfg *Func, Variable *Dest, Operand *Source)
      : InstX86BaseBinopXmmShift<Machine, InstX86Base<Machine>::Psll>(
            Func, Dest, Source) {}
};

template <class Machine>
class InstX86Psrl
    : public InstX86BaseBinopXmmShift<Machine, InstX86Base<Machine>::Psrl,
                                      true> {
public:
  static InstX86Psrl *create(Cfg *Func, Variable *Dest, Operand *Source) {
    return new (Func->allocate<InstX86Psrl>()) InstX86Psrl(Func, Dest, Source);
  }

private:
  InstX86Psrl(Cfg *Func, Variable *Dest, Operand *Source)
      : InstX86BaseBinopXmmShift<Machine, InstX86Base<Machine>::Psrl, true>(
            Func, Dest, Source) {}
};

template <class Machine>
class InstX86Shr
    : public InstX86BaseBinopGPRShift<Machine, InstX86Base<Machine>::Shr> {
public:
  static InstX86Shr *create(Cfg *Func, Variable *Dest, Operand *Source) {
    return new (Func->allocate<InstX86Shr>()) InstX86Shr(Func, Dest, Source);
  }

private:
  InstX86Shr(Cfg *Func, Variable *Dest, Operand *Source)
      : InstX86BaseBinopGPRShift<Machine, InstX86Base<Machine>::Shr>(Func, Dest,
                                                                     Source) {}
};

template <class Machine>
class InstX86Sar
    : public InstX86BaseBinopGPRShift<Machine, InstX86Base<Machine>::Sar> {
public:
  static InstX86Sar *create(Cfg *Func, Variable *Dest, Operand *Source) {
    return new (Func->allocate<InstX86Sar>()) InstX86Sar(Func, Dest, Source);
  }

private:
  InstX86Sar(Cfg *Func, Variable *Dest, Operand *Source)
      : InstX86BaseBinopGPRShift<Machine, InstX86Base<Machine>::Sar>(Func, Dest,
                                                                     Source) {}
};

template <class Machine>
class InstX86Psra
    : public InstX86BaseBinopXmmShift<Machine, InstX86Base<Machine>::Psra> {
public:
  static InstX86Psra *create(Cfg *Func, Variable *Dest, Operand *Source) {
    assert(Dest->getType() == IceType_v8i16 ||
           Dest->getType() == IceType_v8i1 ||
           Dest->getType() == IceType_v4i32 || Dest->getType() == IceType_v4i1);
    return new (Func->allocate<InstX86Psra>()) InstX86Psra(Func, Dest, Source);
  }

private:
  InstX86Psra(Cfg *Func, Variable *Dest, Operand *Source)
      : InstX86BaseBinopXmmShift<Machine, InstX86Base<Machine>::Psra>(
            Func, Dest, Source) {}
};

template <class Machine>
class InstX86Pcmpeq
    : public InstX86BaseBinopXmm<Machine, InstX86Base<Machine>::Pcmpeq, true,
                                 InstX86Base<Machine>::SseSuffix::Integral> {
public:
  static InstX86Pcmpeq *create(Cfg *Func, Variable *Dest, Operand *Source) {
    return new (Func->allocate<InstX86Pcmpeq>())
        InstX86Pcmpeq(Func, Dest, Source);
  }

private:
  InstX86Pcmpeq(Cfg *Func, Variable *Dest, Operand *Source)
      : InstX86BaseBinopXmm<Machine, InstX86Base<Machine>::Pcmpeq, true,
                            InstX86Base<Machine>::SseSuffix::Integral>(
            Func, Dest, Source) {}
};

template <class Machine>
class InstX86Pcmpgt
    : public InstX86BaseBinopXmm<Machine, InstX86Base<Machine>::Pcmpgt, true,
                                 InstX86Base<Machine>::SseSuffix::Integral> {
public:
  static InstX86Pcmpgt *create(Cfg *Func, Variable *Dest, Operand *Source) {
    return new (Func->allocate<InstX86Pcmpgt>())
        InstX86Pcmpgt(Func, Dest, Source);
  }

private:
  InstX86Pcmpgt(Cfg *Func, Variable *Dest, Operand *Source)
      : InstX86BaseBinopXmm<Machine, InstX86Base<Machine>::Pcmpgt, true,
                            InstX86Base<Machine>::SseSuffix::Integral>(
            Func, Dest, Source) {}
};

/// movss is only a binary operation when the source and dest operands are both
/// registers (the high bits of dest are left untouched). In other cases, it
/// behaves like a copy (mov-like) operation (and the high bits of dest are
/// cleared). InstX86Movss will assert that both its source and dest operands
/// are registers, so the lowering code should use _mov instead of _movss in
/// cases where a copy operation is intended.
template <class Machine>
class InstX86MovssRegs
    : public InstX86BaseBinopXmm<Machine, InstX86Base<Machine>::MovssRegs,
                                 false, InstX86Base<Machine>::SseSuffix::None> {
public:
  static InstX86MovssRegs *create(Cfg *Func, Variable *Dest, Operand *Source) {
    return new (Func->allocate<InstX86MovssRegs>())
        InstX86MovssRegs(Func, Dest, Source);
  }

  void emitIAS(const Cfg *Func) const override;

private:
  InstX86MovssRegs(Cfg *Func, Variable *Dest, Operand *Source)
      : InstX86BaseBinopXmm<Machine, InstX86Base<Machine>::MovssRegs, false,
                            InstX86Base<Machine>::SseSuffix::None>(Func, Dest,
                                                                   Source) {}
};

template <class Machine>
class InstX86Idiv
    : public InstX86BaseTernop<Machine, InstX86Base<Machine>::Idiv> {
public:
  static InstX86Idiv *create(Cfg *Func, Variable *Dest, Operand *Source1,
                             Operand *Source2) {
    return new (Func->allocate<InstX86Idiv>())
        InstX86Idiv(Func, Dest, Source1, Source2);
  }

  void emit(const Cfg *Func) const override;
  void emitIAS(const Cfg *Func) const override;

private:
  InstX86Idiv(Cfg *Func, Variable *Dest, Operand *Source1, Operand *Source2)
      : InstX86BaseTernop<Machine, InstX86Base<Machine>::Idiv>(
            Func, Dest, Source1, Source2) {}
};

template <class Machine>
class InstX86Div
    : public InstX86BaseTernop<Machine, InstX86Base<Machine>::Div> {
public:
  static InstX86Div *create(Cfg *Func, Variable *Dest, Operand *Source1,
                            Operand *Source2) {
    return new (Func->allocate<InstX86Div>())
        InstX86Div(Func, Dest, Source1, Source2);
  }

  void emit(const Cfg *Func) const override;
  void emitIAS(const Cfg *Func) const override;

private:
  InstX86Div(Cfg *Func, Variable *Dest, Operand *Source1, Operand *Source2)
      : InstX86BaseTernop<Machine, InstX86Base<Machine>::Div>(
            Func, Dest, Source1, Source2) {}
};

template <class Machine>
class InstX86Insertps
    : public InstX86BaseTernop<Machine, InstX86Base<Machine>::Insertps> {
public:
  static InstX86Insertps *create(Cfg *Func, Variable *Dest, Operand *Source1,
                                 Operand *Source2) {
    return new (Func->allocate<InstX86Insertps>())
        InstX86Insertps(Func, Dest, Source1, Source2);
  }

  void emitIAS(const Cfg *Func) const override;

private:
  InstX86Insertps(Cfg *Func, Variable *Dest, Operand *Source1, Operand *Source2)
      : InstX86BaseTernop<Machine, InstX86Base<Machine>::Insertps>(
            Func, Dest, Source1, Source2) {}
};

template <class Machine>
class InstX86Pinsr
    : public InstX86BaseTernop<Machine, InstX86Base<Machine>::Pinsr> {
public:
  static InstX86Pinsr *create(Cfg *Func, Variable *Dest, Operand *Source1,
                              Operand *Source2) {
    // pinsrb and pinsrd are SSE4.1 instructions.
    assert(Dest->getType() == IceType_v8i16 ||
           Dest->getType() == IceType_v8i1 ||
           InstX86Base<Machine>::getTarget(Func)->getInstructionSet() >=
               InstX86Base<Machine>::Traits::SSE4_1);
    return new (Func->allocate<InstX86Pinsr>())
        InstX86Pinsr(Func, Dest, Source1, Source2);
  }

  void emit(const Cfg *Func) const override;
  void emitIAS(const Cfg *Func) const override;

private:
  InstX86Pinsr(Cfg *Func, Variable *Dest, Operand *Source1, Operand *Source2)
      : InstX86BaseTernop<Machine, InstX86Base<Machine>::Pinsr>(
            Func, Dest, Source1, Source2) {}
};

template <class Machine>
class InstX86Shufps
    : public InstX86BaseTernop<Machine, InstX86Base<Machine>::Shufps> {
public:
  static InstX86Shufps *create(Cfg *Func, Variable *Dest, Operand *Source1,
                               Operand *Source2) {
    return new (Func->allocate<InstX86Shufps>())
        InstX86Shufps(Func, Dest, Source1, Source2);
  }

  void emitIAS(const Cfg *Func) const override;

private:
  InstX86Shufps(Cfg *Func, Variable *Dest, Operand *Source1, Operand *Source2)
      : InstX86BaseTernop<Machine, InstX86Base<Machine>::Shufps>(
            Func, Dest, Source1, Source2) {}
};

template <class Machine>
class InstX86Blendvps
    : public InstX86BaseTernop<Machine, InstX86Base<Machine>::Blendvps> {
public:
  static InstX86Blendvps *create(Cfg *Func, Variable *Dest, Operand *Source1,
                                 Operand *Source2) {
    assert(InstX86Base<Machine>::getTarget(Func)->getInstructionSet() >=
           InstX86Base<Machine>::Traits::SSE4_1);
    return new (Func->allocate<InstX86Blendvps>())
        InstX86Blendvps(Func, Dest, Source1, Source2);
  }

  void emit(const Cfg *Func) const override;
  void emitIAS(const Cfg *Fund) const override;

private:
  InstX86Blendvps(Cfg *Func, Variable *Dest, Operand *Source1, Operand *Source2)
      : InstX86BaseTernop<Machine, InstX86Base<Machine>::Blendvps>(
            Func, Dest, Source1, Source2) {}
};

template <class Machine>
class InstX86Pblendvb
    : public InstX86BaseTernop<Machine, InstX86Base<Machine>::Pblendvb> {
public:
  static InstX86Pblendvb *create(Cfg *Func, Variable *Dest, Operand *Source1,
                                 Operand *Source2) {
    assert(InstX86Base<Machine>::getTarget(Func)->getInstructionSet() >=
           InstX86Base<Machine>::Traits::SSE4_1);
    return new (Func->allocate<InstX86Pblendvb>())
        InstX86Pblendvb(Func, Dest, Source1, Source2);
  }

  void emit(const Cfg *Func) const override;
  void emitIAS(const Cfg *Func) const override;

private:
  InstX86Pblendvb(Cfg *Func, Variable *Dest, Operand *Source1, Operand *Source2)
      : InstX86BaseTernop<Machine, InstX86Base<Machine>::Pblendvb>(
            Func, Dest, Source1, Source2) {}
};

template <class Machine>
class InstX86Pextr
    : public InstX86BaseThreeAddressop<Machine, InstX86Base<Machine>::Pextr> {
public:
  static InstX86Pextr *create(Cfg *Func, Variable *Dest, Operand *Source0,
                              Operand *Source1) {
    assert(Source0->getType() == IceType_v8i16 ||
           Source0->getType() == IceType_v8i1 ||
           InstX86Base<Machine>::getTarget(Func)->getInstructionSet() >=
               InstX86Base<Machine>::Traits::SSE4_1);
    return new (Func->allocate<InstX86Pextr>())
        InstX86Pextr(Func, Dest, Source0, Source1);
  }

  void emit(const Cfg *Func) const override;
  void emitIAS(const Cfg *Func) const override;

private:
  InstX86Pextr(Cfg *Func, Variable *Dest, Operand *Source0, Operand *Source1)
      : InstX86BaseThreeAddressop<Machine, InstX86Base<Machine>::Pextr>(
            Func, Dest, Source0, Source1) {}
};

template <class Machine>
class InstX86Pshufd
    : public InstX86BaseThreeAddressop<Machine, InstX86Base<Machine>::Pshufd> {
public:
  static InstX86Pshufd *create(Cfg *Func, Variable *Dest, Operand *Source0,
                               Operand *Source1) {
    return new (Func->allocate<InstX86Pshufd>())
        InstX86Pshufd(Func, Dest, Source0, Source1);
  }

  void emitIAS(const Cfg *Func) const override;

private:
  InstX86Pshufd(Cfg *Func, Variable *Dest, Operand *Source0, Operand *Source1)
      : InstX86BaseThreeAddressop<Machine, InstX86Base<Machine>::Pshufd>(
            Func, Dest, Source0, Source1) {}
};

/// Base class for a lockable x86-32 instruction (emits a locked prefix).
template <class Machine>
class InstX86BaseLockable : public InstX86Base<Machine> {
  InstX86BaseLockable() = delete;
  InstX86BaseLockable(const InstX86BaseLockable &) = delete;
  InstX86BaseLockable &operator=(const InstX86BaseLockable &) = delete;

protected:
  bool Locked;

  InstX86BaseLockable(Cfg *Func,
                      typename InstX86Base<Machine>::InstKindX86 Kind,
                      SizeT Maxsrcs, Variable *Dest, bool Locked)
      : InstX86Base<Machine>(Func, Kind, Maxsrcs, Dest), Locked(Locked) {
    // Assume that such instructions are used for Atomics and be careful with
    // optimizations.
    this->HasSideEffects = Locked;
  }
};

/// Mul instruction - unsigned multiply.
template <class Machine> class InstX86Mul final : public InstX86Base<Machine> {
  InstX86Mul() = delete;
  InstX86Mul(const InstX86Mul &) = delete;
  InstX86Mul &operator=(const InstX86Mul &) = delete;

public:
  static InstX86Mul *create(Cfg *Func, Variable *Dest, Variable *Source1,
                            Operand *Source2) {
    return new (Func->allocate<InstX86Mul>())
        InstX86Mul(Func, Dest, Source1, Source2);
  }
  void emit(const Cfg *Func) const override;
  void emitIAS(const Cfg *Func) const override;
  void dump(const Cfg *Func) const override;
  static bool classof(const Inst *Inst) {
    return InstX86Base<Machine>::isClassof(Inst, InstX86Base<Machine>::Mul);
  }

private:
  InstX86Mul(Cfg *Func, Variable *Dest, Variable *Source1, Operand *Source2);
};

/// Shld instruction - shift across a pair of operands.
template <class Machine> class InstX86Shld final : public InstX86Base<Machine> {
  InstX86Shld() = delete;
  InstX86Shld(const InstX86Shld &) = delete;
  InstX86Shld &operator=(const InstX86Shld &) = delete;

public:
  static InstX86Shld *create(Cfg *Func, Variable *Dest, Variable *Source1,
                             Operand *Source2) {
    return new (Func->allocate<InstX86Shld>())
        InstX86Shld(Func, Dest, Source1, Source2);
  }
  void emit(const Cfg *Func) const override;
  void emitIAS(const Cfg *Func) const override;
  void dump(const Cfg *Func) const override;
  static bool classof(const Inst *Inst) {
    return InstX86Base<Machine>::isClassof(Inst, InstX86Base<Machine>::Shld);
  }

private:
  InstX86Shld(Cfg *Func, Variable *Dest, Variable *Source1, Operand *Source2);
};

/// Shrd instruction - shift across a pair of operands.
template <class Machine> class InstX86Shrd final : public InstX86Base<Machine> {
  InstX86Shrd() = delete;
  InstX86Shrd(const InstX86Shrd &) = delete;
  InstX86Shrd &operator=(const InstX86Shrd &) = delete;

public:
  static InstX86Shrd *create(Cfg *Func, Variable *Dest, Variable *Source1,
                             Operand *Source2) {
    return new (Func->allocate<InstX86Shrd>())
        InstX86Shrd(Func, Dest, Source1, Source2);
  }
  void emit(const Cfg *Func) const override;
  void emitIAS(const Cfg *Func) const override;
  void dump(const Cfg *Func) const override;
  static bool classof(const Inst *Inst) {
    return InstX86Base<Machine>::isClassof(Inst, InstX86Base<Machine>::Shrd);
  }

private:
  InstX86Shrd(Cfg *Func, Variable *Dest, Variable *Source1, Operand *Source2);
};

/// Conditional move instruction.
template <class Machine> class InstX86Cmov final : public InstX86Base<Machine> {
  InstX86Cmov() = delete;
  InstX86Cmov(const InstX86Cmov &) = delete;
  InstX86Cmov &operator=(const InstX86Cmov &) = delete;

public:
  static InstX86Cmov *
  create(Cfg *Func, Variable *Dest, Operand *Source,
         typename InstX86Base<Machine>::Traits::Cond::BrCond Cond) {
    return new (Func->allocate<InstX86Cmov>())
        InstX86Cmov(Func, Dest, Source, Cond);
  }
  void emit(const Cfg *Func) const override;
  void emitIAS(const Cfg *Func) const override;
  void dump(const Cfg *Func) const override;
  static bool classof(const Inst *Inst) {
    return InstX86Base<Machine>::isClassof(Inst, InstX86Base<Machine>::Cmov);
  }

private:
  InstX86Cmov(Cfg *Func, Variable *Dest, Operand *Source,
              typename InstX86Base<Machine>::Traits::Cond::BrCond Cond);

  typename InstX86Base<Machine>::Traits::Cond::BrCond Condition;
};

/// Cmpps instruction - compare packed singled-precision floating point values
template <class Machine>
class InstX86Cmpps final : public InstX86Base<Machine> {
  InstX86Cmpps() = delete;
  InstX86Cmpps(const InstX86Cmpps &) = delete;
  InstX86Cmpps &operator=(const InstX86Cmpps &) = delete;

public:
  static InstX86Cmpps *
  create(Cfg *Func, Variable *Dest, Operand *Source,
         typename InstX86Base<Machine>::Traits::Cond::CmppsCond Condition) {
    return new (Func->allocate<InstX86Cmpps>())
        InstX86Cmpps(Func, Dest, Source, Condition);
  }
  void emit(const Cfg *Func) const override;
  void emitIAS(const Cfg *Func) const override;
  void dump(const Cfg *Func) const override;
  static bool classof(const Inst *Inst) {
    return InstX86Base<Machine>::isClassof(Inst, InstX86Base<Machine>::Cmpps);
  }

private:
  InstX86Cmpps(Cfg *Func, Variable *Dest, Operand *Source,
               typename InstX86Base<Machine>::Traits::Cond::CmppsCond Cond);

  typename InstX86Base<Machine>::Traits::Cond::CmppsCond Condition;
};

/// Cmpxchg instruction - cmpxchg <dest>, <desired> will compare if <dest>
/// equals eax. If so, the ZF is set and <desired> is stored in <dest>. If not,
/// ZF is cleared and <dest> is copied to eax (or subregister). <dest> can be a
/// register or memory, while <desired> must be a register. It is the user's
/// responsibility to mark eax with a FakeDef.
template <class Machine>
class InstX86Cmpxchg final : public InstX86BaseLockable<Machine> {
  InstX86Cmpxchg() = delete;
  InstX86Cmpxchg(const InstX86Cmpxchg &) = delete;
  InstX86Cmpxchg &operator=(const InstX86Cmpxchg &) = delete;

public:
  static InstX86Cmpxchg *create(Cfg *Func, Operand *DestOrAddr, Variable *Eax,
                                Variable *Desired, bool Locked) {
    return new (Func->allocate<InstX86Cmpxchg>())
        InstX86Cmpxchg(Func, DestOrAddr, Eax, Desired, Locked);
  }
  void emit(const Cfg *Func) const override;
  void emitIAS(const Cfg *Func) const override;
  void dump(const Cfg *Func) const override;
  static bool classof(const Inst *Inst) {
    return InstX86Base<Machine>::isClassof(Inst, InstX86Base<Machine>::Cmpxchg);
  }

private:
  InstX86Cmpxchg(Cfg *Func, Operand *DestOrAddr, Variable *Eax,
                 Variable *Desired, bool Locked);
};

/// Cmpxchg8b instruction - cmpxchg8b <m64> will compare if <m64> equals
/// edx:eax. If so, the ZF is set and ecx:ebx is stored in <m64>. If not, ZF is
/// cleared and <m64> is copied to edx:eax. The caller is responsible for
/// inserting FakeDefs to mark edx and eax as modified. <m64> must be a memory
/// operand.
template <class Machine>
class InstX86Cmpxchg8b final : public InstX86BaseLockable<Machine> {
  InstX86Cmpxchg8b() = delete;
  InstX86Cmpxchg8b(const InstX86Cmpxchg8b &) = delete;
  InstX86Cmpxchg8b &operator=(const InstX86Cmpxchg8b &) = delete;

public:
  static InstX86Cmpxchg8b *
  create(Cfg *Func, typename InstX86Base<Machine>::Traits::X86OperandMem *Dest,
         Variable *Edx, Variable *Eax, Variable *Ecx, Variable *Ebx,
         bool Locked) {
    return new (Func->allocate<InstX86Cmpxchg8b>())
        InstX86Cmpxchg8b(Func, Dest, Edx, Eax, Ecx, Ebx, Locked);
  }
  void emit(const Cfg *Func) const override;
  void emitIAS(const Cfg *Func) const override;
  void dump(const Cfg *Func) const override;
  static bool classof(const Inst *Inst) {
    return InstX86Base<Machine>::isClassof(Inst,
                                           InstX86Base<Machine>::Cmpxchg8b);
  }

private:
  InstX86Cmpxchg8b(Cfg *Func,
                   typename InstX86Base<Machine>::Traits::X86OperandMem *Dest,
                   Variable *Edx, Variable *Eax, Variable *Ecx, Variable *Ebx,
                   bool Locked);
};

/// Cvt instruction - wrapper for cvtsX2sY where X and Y are in {s,d,i} as
/// appropriate.  s=float, d=double, i=int. X and Y are determined from dest/src
/// types. Sign and zero extension on the integer operand needs to be done
/// separately.
template <class Machine> class InstX86Cvt final : public InstX86Base<Machine> {
  InstX86Cvt() = delete;
  InstX86Cvt(const InstX86Cvt &) = delete;
  InstX86Cvt &operator=(const InstX86Cvt &) = delete;

public:
  enum CvtVariant { Si2ss, Tss2si, Float2float, Dq2ps, Tps2dq };
  static InstX86Cvt *create(Cfg *Func, Variable *Dest, Operand *Source,
                            CvtVariant Variant) {
    return new (Func->allocate<InstX86Cvt>())
        InstX86Cvt(Func, Dest, Source, Variant);
  }
  void emit(const Cfg *Func) const override;
  void emitIAS(const Cfg *Func) const override;
  void dump(const Cfg *Func) const override;
  static bool classof(const Inst *Inst) {
    return InstX86Base<Machine>::isClassof(Inst, InstX86Base<Machine>::Cvt);
  }
  bool isTruncating() const { return Variant == Tss2si || Variant == Tps2dq; }

private:
  CvtVariant Variant;
  InstX86Cvt(Cfg *Func, Variable *Dest, Operand *Source, CvtVariant Variant);
};

/// cmp - Integer compare instruction.
template <class Machine> class InstX86Icmp final : public InstX86Base<Machine> {
  InstX86Icmp() = delete;
  InstX86Icmp(const InstX86Icmp &) = delete;
  InstX86Icmp &operator=(const InstX86Icmp &) = delete;

public:
  static InstX86Icmp *create(Cfg *Func, Operand *Src1, Operand *Src2) {
    return new (Func->allocate<InstX86Icmp>()) InstX86Icmp(Func, Src1, Src2);
  }
  void emit(const Cfg *Func) const override;
  void emitIAS(const Cfg *Func) const override;
  void dump(const Cfg *Func) const override;
  static bool classof(const Inst *Inst) {
    return InstX86Base<Machine>::isClassof(Inst, InstX86Base<Machine>::Icmp);
  }

private:
  InstX86Icmp(Cfg *Func, Operand *Src1, Operand *Src2);
};

/// ucomiss/ucomisd - floating-point compare instruction.
template <class Machine>
class InstX86Ucomiss final : public InstX86Base<Machine> {
  InstX86Ucomiss() = delete;
  InstX86Ucomiss(const InstX86Ucomiss &) = delete;
  InstX86Ucomiss &operator=(const InstX86Ucomiss &) = delete;

public:
  static InstX86Ucomiss *create(Cfg *Func, Operand *Src1, Operand *Src2) {
    return new (Func->allocate<InstX86Ucomiss>())
        InstX86Ucomiss(Func, Src1, Src2);
  }
  void emit(const Cfg *Func) const override;
  void emitIAS(const Cfg *Func) const override;
  void dump(const Cfg *Func) const override;
  static bool classof(const Inst *Inst) {
    return InstX86Base<Machine>::isClassof(Inst, InstX86Base<Machine>::Ucomiss);
  }

private:
  InstX86Ucomiss(Cfg *Func, Operand *Src1, Operand *Src2);
};

/// UD2 instruction.
template <class Machine> class InstX86UD2 final : public InstX86Base<Machine> {
  InstX86UD2() = delete;
  InstX86UD2(const InstX86UD2 &) = delete;
  InstX86UD2 &operator=(const InstX86UD2 &) = delete;

public:
  static InstX86UD2 *create(Cfg *Func) {
    return new (Func->allocate<InstX86UD2>()) InstX86UD2(Func);
  }
  void emit(const Cfg *Func) const override;
  void emitIAS(const Cfg *Func) const override;
  void dump(const Cfg *Func) const override;
  static bool classof(const Inst *Inst) {
    return InstX86Base<Machine>::isClassof(Inst, InstX86Base<Machine>::UD2);
  }

private:
  explicit InstX86UD2(Cfg *Func);
};

/// Test instruction.
template <class Machine> class InstX86Test final : public InstX86Base<Machine> {
  InstX86Test() = delete;
  InstX86Test(const InstX86Test &) = delete;
  InstX86Test &operator=(const InstX86Test &) = delete;

public:
  static InstX86Test *create(Cfg *Func, Operand *Source1, Operand *Source2) {
    return new (Func->allocate<InstX86Test>())
        InstX86Test(Func, Source1, Source2);
  }
  void emit(const Cfg *Func) const override;
  void emitIAS(const Cfg *Func) const override;
  void dump(const Cfg *Func) const override;
  static bool classof(const Inst *Inst) {
    return InstX86Base<Machine>::isClassof(Inst, InstX86Base<Machine>::Test);
  }

private:
  InstX86Test(Cfg *Func, Operand *Source1, Operand *Source2);
};

/// Mfence instruction.
template <class Machine>
class InstX86Mfence final : public InstX86Base<Machine> {
  InstX86Mfence() = delete;
  InstX86Mfence(const InstX86Mfence &) = delete;
  InstX86Mfence &operator=(const InstX86Mfence &) = delete;

public:
  static InstX86Mfence *create(Cfg *Func) {
    return new (Func->allocate<InstX86Mfence>()) InstX86Mfence(Func);
  }
  void emit(const Cfg *Func) const override;
  void emitIAS(const Cfg *Func) const override;
  void dump(const Cfg *Func) const override;
  static bool classof(const Inst *Inst) {
    return InstX86Base<Machine>::isClassof(Inst, InstX86Base<Machine>::Mfence);
  }

private:
  explicit InstX86Mfence(Cfg *Func);
};

/// This is essentially a "mov" instruction with an
/// InstX86Base<Machine>::Traits::X86OperandMem operand instead of Variable as
/// the destination. It's important for liveness that there is no Dest operand.
template <class Machine>
class InstX86Store final : public InstX86Base<Machine> {
  InstX86Store() = delete;
  InstX86Store(const InstX86Store &) = delete;
  InstX86Store &operator=(const InstX86Store &) = delete;

public:
  static InstX86Store *
  create(Cfg *Func, Operand *Value,
         typename InstX86Base<Machine>::Traits::X86Operand *Mem) {
    return new (Func->allocate<InstX86Store>()) InstX86Store(Func, Value, Mem);
  }
  void emit(const Cfg *Func) const override;
  void emitIAS(const Cfg *Func) const override;
  void dump(const Cfg *Func) const override;
  static bool classof(const Inst *Inst) {
    return InstX86Base<Machine>::isClassof(Inst, InstX86Base<Machine>::Store);
  }

private:
  InstX86Store(Cfg *Func, Operand *Value,
               typename InstX86Base<Machine>::Traits::X86Operand *Mem);
};

/// This is essentially a vector "mov" instruction with an typename
/// InstX86Base<Machine>::Traits::X86OperandMem operand instead of Variable as
/// the destination. It's important for liveness that there is no Dest operand.
/// The source must be an Xmm register, since Dest is mem.
template <class Machine>
class InstX86StoreP final : public InstX86Base<Machine> {
  InstX86StoreP() = delete;
  InstX86StoreP(const InstX86StoreP &) = delete;
  InstX86StoreP &operator=(const InstX86StoreP &) = delete;

public:
  static InstX86StoreP *
  create(Cfg *Func, Variable *Value,
         typename InstX86Base<Machine>::Traits::X86OperandMem *Mem) {
    return new (Func->allocate<InstX86StoreP>())
        InstX86StoreP(Func, Value, Mem);
  }
  void emit(const Cfg *Func) const override;
  void emitIAS(const Cfg *Func) const override;
  void dump(const Cfg *Func) const override;
  static bool classof(const Inst *Inst) {
    return InstX86Base<Machine>::isClassof(Inst, InstX86Base<Machine>::StoreP);
  }

private:
  InstX86StoreP(Cfg *Func, Variable *Value,
                typename InstX86Base<Machine>::Traits::X86OperandMem *Mem);
};

template <class Machine>
class InstX86StoreQ final : public InstX86Base<Machine> {
  InstX86StoreQ() = delete;
  InstX86StoreQ(const InstX86StoreQ &) = delete;
  InstX86StoreQ &operator=(const InstX86StoreQ &) = delete;

public:
  static InstX86StoreQ *
  create(Cfg *Func, Variable *Value,
         typename InstX86Base<Machine>::Traits::X86OperandMem *Mem) {
    return new (Func->allocate<InstX86StoreQ>())
        InstX86StoreQ(Func, Value, Mem);
  }
  void emit(const Cfg *Func) const override;
  void emitIAS(const Cfg *Func) const override;
  void dump(const Cfg *Func) const override;
  static bool classof(const Inst *Inst) {
    return InstX86Base<Machine>::isClassof(Inst, InstX86Base<Machine>::StoreQ);
  }

private:
  InstX86StoreQ(Cfg *Func, Variable *Value,
                typename InstX86Base<Machine>::Traits::X86OperandMem *Mem);
};

/// Nop instructions of varying length
template <class Machine> class InstX86Nop final : public InstX86Base<Machine> {
  InstX86Nop() = delete;
  InstX86Nop(const InstX86Nop &) = delete;
  InstX86Nop &operator=(const InstX86Nop &) = delete;

public:
  // TODO: Replace with enum.
  using NopVariant = unsigned;

  static InstX86Nop *create(Cfg *Func, NopVariant Variant) {
    return new (Func->allocate<InstX86Nop>()) InstX86Nop(Func, Variant);
  }
  void emit(const Cfg *Func) const override;
  void emitIAS(const Cfg *Func) const override;
  void dump(const Cfg *Func) const override;
  static bool classof(const Inst *Inst) {
    return InstX86Base<Machine>::isClassof(Inst, InstX86Base<Machine>::Nop);
  }

private:
  InstX86Nop(Cfg *Func, SizeT Length);

  NopVariant Variant;
};

/// Fld - load a value onto the x87 FP stack.
template <class Machine> class InstX86Fld final : public InstX86Base<Machine> {
  InstX86Fld() = delete;
  InstX86Fld(const InstX86Fld &) = delete;
  InstX86Fld &operator=(const InstX86Fld &) = delete;

public:
  static InstX86Fld *create(Cfg *Func, Operand *Src) {
    return new (Func->allocate<InstX86Fld>()) InstX86Fld(Func, Src);
  }
  void emit(const Cfg *Func) const override;
  void emitIAS(const Cfg *Func) const override;
  void dump(const Cfg *Func) const override;
  static bool classof(const Inst *Inst) {
    return InstX86Base<Machine>::isClassof(Inst, InstX86Base<Machine>::Fld);
  }

private:
  InstX86Fld(Cfg *Func, Operand *Src);
};

/// Fstp - store x87 st(0) into memory and pop st(0).
template <class Machine> class InstX86Fstp final : public InstX86Base<Machine> {
  InstX86Fstp() = delete;
  InstX86Fstp(const InstX86Fstp &) = delete;
  InstX86Fstp &operator=(const InstX86Fstp &) = delete;

public:
  static InstX86Fstp *create(Cfg *Func, Variable *Dest) {
    return new (Func->allocate<InstX86Fstp>()) InstX86Fstp(Func, Dest);
  }
  void emit(const Cfg *Func) const override;
  void emitIAS(const Cfg *Func) const override;
  void dump(const Cfg *Func) const override;
  static bool classof(const Inst *Inst) {
    return InstX86Base<Machine>::isClassof(Inst, InstX86Base<Machine>::Fstp);
  }

private:
  InstX86Fstp(Cfg *Func, Variable *Dest);
};

template <class Machine> class InstX86Pop final : public InstX86Base<Machine> {
  InstX86Pop() = delete;
  InstX86Pop(const InstX86Pop &) = delete;
  InstX86Pop &operator=(const InstX86Pop &) = delete;

public:
  static InstX86Pop *create(Cfg *Func, Variable *Dest) {
    return new (Func->allocate<InstX86Pop>()) InstX86Pop(Func, Dest);
  }
  void emit(const Cfg *Func) const override;
  void emitIAS(const Cfg *Func) const override;
  void dump(const Cfg *Func) const override;
  static bool classof(const Inst *Inst) {
    return InstX86Base<Machine>::isClassof(Inst, InstX86Base<Machine>::Pop);
  }

private:
  InstX86Pop(Cfg *Func, Variable *Dest);
};

template <class Machine> class InstX86Push final : public InstX86Base<Machine> {
  InstX86Push() = delete;
  InstX86Push(const InstX86Push &) = delete;
  InstX86Push &operator=(const InstX86Push &) = delete;

public:
  static InstX86Push *create(Cfg *Func, Variable *Source) {
    return new (Func->allocate<InstX86Push>()) InstX86Push(Func, Source);
  }
  void emit(const Cfg *Func) const override;
  void emitIAS(const Cfg *Func) const override;
  void dump(const Cfg *Func) const override;
  static bool classof(const Inst *Inst) {
    return InstX86Base<Machine>::isClassof(Inst, InstX86Base<Machine>::Push);
  }

private:
  InstX86Push(Cfg *Func, Variable *Source);
};

/// Ret instruction. Currently only supports the "ret" version that does not pop
/// arguments. This instruction takes a Source operand (for non-void returning
/// functions) for liveness analysis, though a FakeUse before the ret would do
/// just as well.
template <class Machine> class InstX86Ret final : public InstX86Base<Machine> {
  InstX86Ret() = delete;
  InstX86Ret(const InstX86Ret &) = delete;
  InstX86Ret &operator=(const InstX86Ret &) = delete;

public:
  static InstX86Ret *create(Cfg *Func, Variable *Source = nullptr) {
    return new (Func->allocate<InstX86Ret>()) InstX86Ret(Func, Source);
  }
  void emit(const Cfg *Func) const override;
  void emitIAS(const Cfg *Func) const override;
  void dump(const Cfg *Func) const override;
  static bool classof(const Inst *Inst) {
    return InstX86Base<Machine>::isClassof(Inst, InstX86Base<Machine>::Ret);
  }

private:
  InstX86Ret(Cfg *Func, Variable *Source);
};

/// Conditional set-byte instruction.
template <class Machine>
class InstX86Setcc final : public InstX86Base<Machine> {
  InstX86Setcc() = delete;
  InstX86Setcc(const InstX86Cmov<Machine> &) = delete;
  InstX86Setcc &operator=(const InstX86Setcc &) = delete;

public:
  static InstX86Setcc *
  create(Cfg *Func, Variable *Dest,
         typename InstX86Base<Machine>::Traits::Cond::BrCond Cond) {
    return new (Func->allocate<InstX86Setcc>()) InstX86Setcc(Func, Dest, Cond);
  }
  void emit(const Cfg *Func) const override;
  void emitIAS(const Cfg *Func) const override;
  void dump(const Cfg *Func) const override;
  static bool classof(const Inst *Inst) {
    return InstX86Base<Machine>::isClassof(Inst, InstX86Base<Machine>::Setcc);
  }

private:
  InstX86Setcc(Cfg *Func, Variable *Dest,
               typename InstX86Base<Machine>::Traits::Cond::BrCond Cond);

  const typename InstX86Base<Machine>::Traits::Cond::BrCond Condition;
};

/// Exchanging Add instruction. Exchanges the first operand (destination
/// operand) with the second operand (source operand), then loads the sum of the
/// two values into the destination operand. The destination may be a register
/// or memory, while the source must be a register.
///
/// Both the dest and source are updated. The caller should then insert a
/// FakeDef to reflect the second udpate.
template <class Machine>
class InstX86Xadd final : public InstX86BaseLockable<Machine> {
  InstX86Xadd() = delete;
  InstX86Xadd(const InstX86Xadd &) = delete;
  InstX86Xadd &operator=(const InstX86Xadd &) = delete;

public:
  static InstX86Xadd *create(Cfg *Func, Operand *Dest, Variable *Source,
                             bool Locked) {
    return new (Func->allocate<InstX86Xadd>())
        InstX86Xadd(Func, Dest, Source, Locked);
  }
  void emit(const Cfg *Func) const override;
  void emitIAS(const Cfg *Func) const override;
  void dump(const Cfg *Func) const override;
  static bool classof(const Inst *Inst) {
    return InstX86Base<Machine>::isClassof(Inst, InstX86Base<Machine>::Xadd);
  }

private:
  InstX86Xadd(Cfg *Func, Operand *Dest, Variable *Source, bool Locked);
};

/// Exchange instruction. Exchanges the first operand (destination operand) with
/// the second operand (source operand). At least one of the operands must be a
/// register (and the other can be reg or mem). Both the Dest and Source are
/// updated. If there is a memory operand, then the instruction is automatically
/// "locked" without the need for a lock prefix.
template <class Machine> class InstX86Xchg final : public InstX86Base<Machine> {
  InstX86Xchg() = delete;
  InstX86Xchg(const InstX86Xchg &) = delete;
  InstX86Xchg &operator=(const InstX86Xchg &) = delete;

public:
  static InstX86Xchg *create(Cfg *Func, Operand *Dest, Variable *Source) {
    return new (Func->allocate<InstX86Xchg>()) InstX86Xchg(Func, Dest, Source);
  }
  void emit(const Cfg *Func) const override;
  void emitIAS(const Cfg *Func) const override;
  void dump(const Cfg *Func) const override;
  static bool classof(const Inst *Inst) {
    return InstX86Base<Machine>::isClassof(Inst, InstX86Base<Machine>::Xchg);
  }

private:
  InstX86Xchg(Cfg *Func, Operand *Dest, Variable *Source);
};

/// Start marker for the Intel Architecture Code Analyzer. This is not an
/// executable instruction and must only be used for analysis.
template <class Machine>
class InstX86IacaStart final : public InstX86Base<Machine> {
  InstX86IacaStart() = delete;
  InstX86IacaStart(const InstX86IacaStart &) = delete;
  InstX86IacaStart &operator=(const InstX86IacaStart &) = delete;

public:
  static InstX86IacaStart *create(Cfg *Func) {
    return new (Func->allocate<InstX86IacaStart>()) InstX86IacaStart(Func);
  }
  void emit(const Cfg *Func) const override;
  void emitIAS(const Cfg *Func) const override;
  void dump(const Cfg *Func) const override;
  static bool classof(const Inst *Inst) {
    return InstX86Base<Machine>::isClassof(Inst,
                                           InstX86Base<Machine>::IacaStart);
  }

private:
  InstX86IacaStart(Cfg *Func);
};

/// End marker for the Intel Architecture Code Analyzer. This is not an
/// executable instruction and must only be used for analysis.
template <class Machine>
class InstX86IacaEnd final : public InstX86Base<Machine> {
  InstX86IacaEnd() = delete;
  InstX86IacaEnd(const InstX86IacaEnd &) = delete;
  InstX86IacaEnd &operator=(const InstX86IacaEnd &) = delete;

public:
  static InstX86IacaEnd *create(Cfg *Func) {
    return new (Func->allocate<InstX86IacaEnd>()) InstX86IacaEnd(Func);
  }
  void emit(const Cfg *Func) const override;
  void emitIAS(const Cfg *Func) const override;
  void dump(const Cfg *Func) const override;
  static bool classof(const Inst *Inst) {
    return InstX86Base<Machine>::isClassof(Inst, InstX86Base<Machine>::IacaEnd);
  }

private:
  InstX86IacaEnd(Cfg *Func);
};

/// struct Insts is a template that can be used to instantiate all the X86
/// instructions for a target with a simple
///
/// using Insts = ::Ice::X86Internal::Insts<TargeT>;
template <class Machine> struct Insts {
  using FakeRMW = InstX86FakeRMW<Machine>;
  using Label = InstX86Label<Machine>;

  using Call = InstX86Call<Machine>;

  using Br = InstX86Br<Machine>;
  using Jmp = InstX86Jmp<Machine>;
  using Bswap = InstX86Bswap<Machine>;
  using Neg = InstX86Neg<Machine>;
  using Bsf = InstX86Bsf<Machine>;
  using Bsr = InstX86Bsr<Machine>;
  using Lea = InstX86Lea<Machine>;
  using Cbwdq = InstX86Cbwdq<Machine>;
  using Movsx = InstX86Movsx<Machine>;
  using Movzx = InstX86Movzx<Machine>;
  using Movd = InstX86Movd<Machine>;
  using Sqrtss = InstX86Sqrtss<Machine>;
  using Mov = InstX86Mov<Machine>;
  using Movp = InstX86Movp<Machine>;
  using Movq = InstX86Movq<Machine>;
  using Add = InstX86Add<Machine>;
  using AddRMW = InstX86AddRMW<Machine>;
  using Addps = InstX86Addps<Machine>;
  using Adc = InstX86Adc<Machine>;
  using AdcRMW = InstX86AdcRMW<Machine>;
  using Addss = InstX86Addss<Machine>;
  using Andnps = InstX86Andnps<Machine>;
  using Andps = InstX86Andps<Machine>;
  using Padd = InstX86Padd<Machine>;
  using Sub = InstX86Sub<Machine>;
  using SubRMW = InstX86SubRMW<Machine>;
  using Subps = InstX86Subps<Machine>;
  using Subss = InstX86Subss<Machine>;
  using Sbb = InstX86Sbb<Machine>;
  using SbbRMW = InstX86SbbRMW<Machine>;
  using Psub = InstX86Psub<Machine>;
  using And = InstX86And<Machine>;
  using AndRMW = InstX86AndRMW<Machine>;
  using Pand = InstX86Pand<Machine>;
  using Pandn = InstX86Pandn<Machine>;
  using Or = InstX86Or<Machine>;
  using Orps = InstX86Orps<Machine>;
  using OrRMW = InstX86OrRMW<Machine>;
  using Por = InstX86Por<Machine>;
  using Xor = InstX86Xor<Machine>;
  using Xorps = InstX86Xorps<Machine>;
  using XorRMW = InstX86XorRMW<Machine>;
  using Pxor = InstX86Pxor<Machine>;
  using Maxss = InstX86Maxss<Machine>;
  using Minss = InstX86Minss<Machine>;
  using Imul = InstX86Imul<Machine>;
  using ImulImm = InstX86ImulImm<Machine>;
  using Mulps = InstX86Mulps<Machine>;
  using Mulss = InstX86Mulss<Machine>;
  using Pmull = InstX86Pmull<Machine>;
  using Pmuludq = InstX86Pmuludq<Machine>;
  using Divps = InstX86Divps<Machine>;
  using Divss = InstX86Divss<Machine>;
  using Rol = InstX86Rol<Machine>;
  using Shl = InstX86Shl<Machine>;
  using Psll = InstX86Psll<Machine>;
  using Psrl = InstX86Psrl<Machine>;
  using Shr = InstX86Shr<Machine>;
  using Sar = InstX86Sar<Machine>;
  using Psra = InstX86Psra<Machine>;
  using Pcmpeq = InstX86Pcmpeq<Machine>;
  using Pcmpgt = InstX86Pcmpgt<Machine>;
  using MovssRegs = InstX86MovssRegs<Machine>;
  using Idiv = InstX86Idiv<Machine>;
  using Div = InstX86Div<Machine>;
  using Insertps = InstX86Insertps<Machine>;
  using Pinsr = InstX86Pinsr<Machine>;
  using Shufps = InstX86Shufps<Machine>;
  using Blendvps = InstX86Blendvps<Machine>;
  using Pblendvb = InstX86Pblendvb<Machine>;
  using Pextr = InstX86Pextr<Machine>;
  using Pshufd = InstX86Pshufd<Machine>;
  using Lockable = InstX86BaseLockable<Machine>;
  using Mul = InstX86Mul<Machine>;
  using Shld = InstX86Shld<Machine>;
  using Shrd = InstX86Shrd<Machine>;
  using Cmov = InstX86Cmov<Machine>;
  using Cmpps = InstX86Cmpps<Machine>;
  using Cmpxchg = InstX86Cmpxchg<Machine>;
  using Cmpxchg8b = InstX86Cmpxchg8b<Machine>;
  using Cvt = InstX86Cvt<Machine>;
  using Icmp = InstX86Icmp<Machine>;
  using Ucomiss = InstX86Ucomiss<Machine>;
  using UD2 = InstX86UD2<Machine>;
  using Test = InstX86Test<Machine>;
  using Mfence = InstX86Mfence<Machine>;
  using Store = InstX86Store<Machine>;
  using StoreP = InstX86StoreP<Machine>;
  using StoreQ = InstX86StoreQ<Machine>;
  using Nop = InstX86Nop<Machine>;
  template <typename T = typename InstX86Base<Machine>::Traits>
  using Fld = typename std::enable_if<T::UsesX87, InstX86Fld<Machine>>::type;
  template <typename T = typename InstX86Base<Machine>::Traits>
  using Fstp = typename std::enable_if<T::UsesX87, InstX86Fstp<Machine>>::type;
  using Pop = InstX86Pop<Machine>;
  using Push = InstX86Push<Machine>;
  using Ret = InstX86Ret<Machine>;
  using Setcc = InstX86Setcc<Machine>;
  using Xadd = InstX86Xadd<Machine>;
  using Xchg = InstX86Xchg<Machine>;

  using IacaStart = InstX86IacaStart<Machine>;
  using IacaEnd = InstX86IacaEnd<Machine>;
};

/// X86 Instructions have static data (particularly, opcodes and instruction
/// emitters). Each X86 target needs to define all of these, so this macro is
/// provided so that, if something changes, then all X86 targets will be updated
/// automatically.
#define X86INSTS_DEFINE_STATIC_DATA(Machine)                                   \
  namespace Ice {                                                              \
  namespace X86Internal {                                                      \
  /* In-place ops */                                                           \
  template <> const char *InstX86Bswap<Machine>::Base::Opcode = "bswap";       \
  template <> const char *InstX86Neg<Machine>::Base::Opcode = "neg";           \
  /* Unary ops */                                                              \
  template <> const char *InstX86Bsf<Machine>::Base::Opcode = "bsf";           \
  template <> const char *InstX86Bsr<Machine>::Base::Opcode = "bsr";           \
  template <> const char *InstX86Lea<Machine>::Base::Opcode = "lea";           \
  template <> const char *InstX86Movd<Machine>::Base::Opcode = "movd";         \
  template <> const char *InstX86Movsx<Machine>::Base::Opcode = "movs";        \
  template <> const char *InstX86Movzx<Machine>::Base::Opcode = "movz";        \
  template <> const char *InstX86Sqrtss<Machine>::Base::Opcode = "sqrtss";     \
  template <> const char *InstX86Cbwdq<Machine>::Base::Opcode = "cbw/cwd/cdq"; \
  /* Mov-like ops */                                                           \
  template <> const char *InstX86Mov<Machine>::Base::Opcode = "mov";           \
  template <> const char *InstX86Movp<Machine>::Base::Opcode = "movups";       \
  template <> const char *InstX86Movq<Machine>::Base::Opcode = "movq";         \
  /* Binary ops */                                                             \
  template <> const char *InstX86Add<Machine>::Base::Opcode = "add";           \
  template <> const char *InstX86AddRMW<Machine>::Base::Opcode = "add";        \
  template <> const char *InstX86Addps<Machine>::Base::Opcode = "add";         \
  template <> const char *InstX86Adc<Machine>::Base::Opcode = "adc";           \
  template <> const char *InstX86AdcRMW<Machine>::Base::Opcode = "adc";        \
  template <> const char *InstX86Addss<Machine>::Base::Opcode = "add";         \
  template <> const char *InstX86Andnps<Machine>::Base::Opcode = "andn";       \
  template <> const char *InstX86Andps<Machine>::Base::Opcode = "and";         \
  template <> const char *InstX86Maxss<Machine>::Base::Opcode = "max";         \
  template <> const char *InstX86Minss<Machine>::Base::Opcode = "min";         \
  template <> const char *InstX86Padd<Machine>::Base::Opcode = "padd";         \
  template <> const char *InstX86Sub<Machine>::Base::Opcode = "sub";           \
  template <> const char *InstX86SubRMW<Machine>::Base::Opcode = "sub";        \
  template <> const char *InstX86Subps<Machine>::Base::Opcode = "sub";         \
  template <> const char *InstX86Subss<Machine>::Base::Opcode = "sub";         \
  template <> const char *InstX86Sbb<Machine>::Base::Opcode = "sbb";           \
  template <> const char *InstX86SbbRMW<Machine>::Base::Opcode = "sbb";        \
  template <> const char *InstX86Psub<Machine>::Base::Opcode = "psub";         \
  template <> const char *InstX86And<Machine>::Base::Opcode = "and";           \
  template <> const char *InstX86AndRMW<Machine>::Base::Opcode = "and";        \
  template <> const char *InstX86Pand<Machine>::Base::Opcode = "pand";         \
  template <> const char *InstX86Pandn<Machine>::Base::Opcode = "pandn";       \
  template <> const char *InstX86Or<Machine>::Base::Opcode = "or";             \
  template <> const char *InstX86Orps<Machine>::Base::Opcode = "or";           \
  template <> const char *InstX86OrRMW<Machine>::Base::Opcode = "or";          \
  template <> const char *InstX86Por<Machine>::Base::Opcode = "por";           \
  template <> const char *InstX86Xor<Machine>::Base::Opcode = "xor";           \
  template <> const char *InstX86Xorps<Machine>::Base::Opcode = "xor";         \
  template <> const char *InstX86XorRMW<Machine>::Base::Opcode = "xor";        \
  template <> const char *InstX86Pxor<Machine>::Base::Opcode = "pxor";         \
  template <> const char *InstX86Imul<Machine>::Base::Opcode = "imul";         \
  template <> const char *InstX86ImulImm<Machine>::Base::Opcode = "imul";      \
  template <> const char *InstX86Mulps<Machine>::Base::Opcode = "mul";         \
  template <> const char *InstX86Mulss<Machine>::Base::Opcode = "mul";         \
  template <> const char *InstX86Pmull<Machine>::Base::Opcode = "pmull";       \
  template <> const char *InstX86Pmuludq<Machine>::Base::Opcode = "pmuludq";   \
  template <> const char *InstX86Div<Machine>::Base::Opcode = "div";           \
  template <> const char *InstX86Divps<Machine>::Base::Opcode = "div";         \
  template <> const char *InstX86Divss<Machine>::Base::Opcode = "div";         \
  template <> const char *InstX86Idiv<Machine>::Base::Opcode = "idiv";         \
  template <> const char *InstX86Rol<Machine>::Base::Opcode = "rol";           \
  template <> const char *InstX86Shl<Machine>::Base::Opcode = "shl";           \
  template <> const char *InstX86Psll<Machine>::Base::Opcode = "psll";         \
  template <> const char *InstX86Shr<Machine>::Base::Opcode = "shr";           \
  template <> const char *InstX86Sar<Machine>::Base::Opcode = "sar";           \
  template <> const char *InstX86Psra<Machine>::Base::Opcode = "psra";         \
  template <> const char *InstX86Psrl<Machine>::Base::Opcode = "psrl";         \
  template <> const char *InstX86Pcmpeq<Machine>::Base::Opcode = "pcmpeq";     \
  template <> const char *InstX86Pcmpgt<Machine>::Base::Opcode = "pcmpgt";     \
  template <> const char *InstX86MovssRegs<Machine>::Base::Opcode = "movss";   \
  /* Ternary ops */                                                            \
  template <> const char *InstX86Insertps<Machine>::Base::Opcode = "insertps"; \
  template <> const char *InstX86Shufps<Machine>::Base::Opcode = "shufps";     \
  template <> const char *InstX86Pinsr<Machine>::Base::Opcode = "pinsr";       \
  template <> const char *InstX86Blendvps<Machine>::Base::Opcode = "blendvps"; \
  template <> const char *InstX86Pblendvb<Machine>::Base::Opcode = "pblendvb"; \
  /* Three address ops */                                                      \
  template <> const char *InstX86Pextr<Machine>::Base::Opcode = "pextr";       \
  template <> const char *InstX86Pshufd<Machine>::Base::Opcode = "pshufd";     \
  /* Inplace GPR ops */                                                        \
  template <>                                                                  \
  const InstX86Base<Machine>::Traits::Assembler::GPREmitterOneOp               \
      InstX86Bswap<Machine>::Base::Emitter = {                                 \
          &InstX86Base<Machine>::Traits::Assembler::bswap,                     \
          nullptr /* only a reg form exists */                                 \
  };                                                                           \
  template <>                                                                  \
  const InstX86Base<Machine>::Traits::Assembler::GPREmitterOneOp               \
      InstX86Neg<Machine>::Base::Emitter = {                                   \
          &InstX86Base<Machine>::Traits::Assembler::neg,                       \
          &InstX86Base<Machine>::Traits::Assembler::neg};                      \
                                                                               \
  /* Unary GPR ops */                                                          \
  template <> /* uses specialized emitter. */                                  \
  const InstX86Base<Machine>::Traits::Assembler::GPREmitterRegOp               \
      InstX86Cbwdq<Machine>::Base::Emitter = {nullptr, nullptr, nullptr};      \
  template <>                                                                  \
  const InstX86Base<Machine>::Traits::Assembler::GPREmitterRegOp               \
      InstX86Bsf<Machine>::Base::Emitter = {                                   \
          &InstX86Base<Machine>::Traits::Assembler::bsf,                       \
          &InstX86Base<Machine>::Traits::Assembler::bsf, nullptr};             \
  template <>                                                                  \
  const InstX86Base<Machine>::Traits::Assembler::GPREmitterRegOp               \
      InstX86Bsr<Machine>::Base::Emitter = {                                   \
          &InstX86Base<Machine>::Traits::Assembler::bsr,                       \
          &InstX86Base<Machine>::Traits::Assembler::bsr, nullptr};             \
  template <>                                                                  \
  const InstX86Base<Machine>::Traits::Assembler::GPREmitterRegOp               \
      InstX86Lea<Machine>::Base::Emitter = {                                   \
          /* reg/reg and reg/imm are illegal */ nullptr,                       \
          &InstX86Base<Machine>::Traits::Assembler::lea, nullptr};             \
  template <>                                                                  \
  const InstX86Base<Machine>::Traits::Assembler::GPREmitterRegOp               \
      InstX86Movsx<Machine>::Base::Emitter = {                                 \
          &InstX86Base<Machine>::Traits::Assembler::movsx,                     \
          &InstX86Base<Machine>::Traits::Assembler::movsx, nullptr};           \
  template <>                                                                  \
  const InstX86Base<Machine>::Traits::Assembler::GPREmitterRegOp               \
      InstX86Movzx<Machine>::Base::Emitter = {                                 \
          &InstX86Base<Machine>::Traits::Assembler::movzx,                     \
          &InstX86Base<Machine>::Traits::Assembler::movzx, nullptr};           \
                                                                               \
  /* Unary XMM ops */                                                          \
  template <> /* uses specialized emitter. */                                  \
  const InstX86Base<Machine>::Traits::Assembler::XmmEmitterRegOp               \
      InstX86Movd<Machine>::Base::Emitter = {nullptr, nullptr};                \
  template <>                                                                  \
  const InstX86Base<Machine>::Traits::Assembler::XmmEmitterRegOp               \
      InstX86Sqrtss<Machine>::Base::Emitter = {                                \
          &InstX86Base<Machine>::Traits::Assembler::sqrtss,                    \
          &InstX86Base<Machine>::Traits::Assembler::sqrtss};                   \
                                                                               \
  /* Binary GPR ops */                                                         \
  template <> /* uses specialized emitter. */                                  \
  const InstX86Base<Machine>::Traits::Assembler::GPREmitterRegOp               \
      InstX86Imul<Machine>::Base::Emitter = {nullptr, nullptr, nullptr};       \
  template <>                                                                  \
  const InstX86Base<Machine>::Traits::Assembler::GPREmitterRegOp               \
      InstX86Add<Machine>::Base::Emitter = {                                   \
          &InstX86Base<Machine>::Traits::Assembler::add,                       \
          &InstX86Base<Machine>::Traits::Assembler::add,                       \
          &InstX86Base<Machine>::Traits::Assembler::add};                      \
  template <>                                                                  \
  const InstX86Base<Machine>::Traits::Assembler::GPREmitterAddrOp              \
      InstX86AddRMW<Machine>::Base::Emitter = {                                \
          &InstX86Base<Machine>::Traits::Assembler::add,                       \
          &InstX86Base<Machine>::Traits::Assembler::add};                      \
  template <>                                                                  \
  const InstX86Base<Machine>::Traits::Assembler::GPREmitterRegOp               \
      InstX86Adc<Machine>::Base::Emitter = {                                   \
          &InstX86Base<Machine>::Traits::Assembler::adc,                       \
          &InstX86Base<Machine>::Traits::Assembler::adc,                       \
          &InstX86Base<Machine>::Traits::Assembler::adc};                      \
  template <>                                                                  \
  const InstX86Base<Machine>::Traits::Assembler::GPREmitterAddrOp              \
      InstX86AdcRMW<Machine>::Base::Emitter = {                                \
          &InstX86Base<Machine>::Traits::Assembler::adc,                       \
          &InstX86Base<Machine>::Traits::Assembler::adc};                      \
  template <>                                                                  \
  const InstX86Base<Machine>::Traits::Assembler::GPREmitterRegOp               \
      InstX86And<Machine>::Base::Emitter = {                                   \
          &InstX86Base<Machine>::Traits::Assembler::And,                       \
          &InstX86Base<Machine>::Traits::Assembler::And,                       \
          &InstX86Base<Machine>::Traits::Assembler::And};                      \
  template <>                                                                  \
  const InstX86Base<Machine>::Traits::Assembler::GPREmitterAddrOp              \
      InstX86AndRMW<Machine>::Base::Emitter = {                                \
          &InstX86Base<Machine>::Traits::Assembler::And,                       \
          &InstX86Base<Machine>::Traits::Assembler::And};                      \
  template <>                                                                  \
  const InstX86Base<Machine>::Traits::Assembler::GPREmitterRegOp               \
      InstX86Or<Machine>::Base::Emitter = {                                    \
          &InstX86Base<Machine>::Traits::Assembler::Or,                        \
          &InstX86Base<Machine>::Traits::Assembler::Or,                        \
          &InstX86Base<Machine>::Traits::Assembler::Or};                       \
  template <>                                                                  \
  const InstX86Base<Machine>::Traits::Assembler::GPREmitterAddrOp              \
      InstX86OrRMW<Machine>::Base::Emitter = {                                 \
          &InstX86Base<Machine>::Traits::Assembler::Or,                        \
          &InstX86Base<Machine>::Traits::Assembler::Or};                       \
  template <>                                                                  \
  const InstX86Base<Machine>::Traits::Assembler::GPREmitterRegOp               \
      InstX86Sbb<Machine>::Base::Emitter = {                                   \
          &InstX86Base<Machine>::Traits::Assembler::sbb,                       \
          &InstX86Base<Machine>::Traits::Assembler::sbb,                       \
          &InstX86Base<Machine>::Traits::Assembler::sbb};                      \
  template <>                                                                  \
  const InstX86Base<Machine>::Traits::Assembler::GPREmitterAddrOp              \
      InstX86SbbRMW<Machine>::Base::Emitter = {                                \
          &InstX86Base<Machine>::Traits::Assembler::sbb,                       \
          &InstX86Base<Machine>::Traits::Assembler::sbb};                      \
  template <>                                                                  \
  const InstX86Base<Machine>::Traits::Assembler::GPREmitterRegOp               \
      InstX86Sub<Machine>::Base::Emitter = {                                   \
          &InstX86Base<Machine>::Traits::Assembler::sub,                       \
          &InstX86Base<Machine>::Traits::Assembler::sub,                       \
          &InstX86Base<Machine>::Traits::Assembler::sub};                      \
  template <>                                                                  \
  const InstX86Base<Machine>::Traits::Assembler::GPREmitterAddrOp              \
      InstX86SubRMW<Machine>::Base::Emitter = {                                \
          &InstX86Base<Machine>::Traits::Assembler::sub,                       \
          &InstX86Base<Machine>::Traits::Assembler::sub};                      \
  template <>                                                                  \
  const InstX86Base<Machine>::Traits::Assembler::GPREmitterRegOp               \
      InstX86Xor<Machine>::Base::Emitter = {                                   \
          &InstX86Base<Machine>::Traits::Assembler::Xor,                       \
          &InstX86Base<Machine>::Traits::Assembler::Xor,                       \
          &InstX86Base<Machine>::Traits::Assembler::Xor};                      \
  template <>                                                                  \
  const InstX86Base<Machine>::Traits::Assembler::GPREmitterAddrOp              \
      InstX86XorRMW<Machine>::Base::Emitter = {                                \
          &InstX86Base<Machine>::Traits::Assembler::Xor,                       \
          &InstX86Base<Machine>::Traits::Assembler::Xor};                      \
                                                                               \
  /* Binary Shift GPR ops */                                                   \
  template <>                                                                  \
  const InstX86Base<Machine>::Traits::Assembler::GPREmitterShiftOp             \
      InstX86Rol<Machine>::Base::Emitter = {                                   \
          &InstX86Base<Machine>::Traits::Assembler::rol,                       \
          &InstX86Base<Machine>::Traits::Assembler::rol};                      \
  template <>                                                                  \
  const InstX86Base<Machine>::Traits::Assembler::GPREmitterShiftOp             \
      InstX86Sar<Machine>::Base::Emitter = {                                   \
          &InstX86Base<Machine>::Traits::Assembler::sar,                       \
          &InstX86Base<Machine>::Traits::Assembler::sar};                      \
  template <>                                                                  \
  const InstX86Base<Machine>::Traits::Assembler::GPREmitterShiftOp             \
      InstX86Shl<Machine>::Base::Emitter = {                                   \
          &InstX86Base<Machine>::Traits::Assembler::shl,                       \
          &InstX86Base<Machine>::Traits::Assembler::shl};                      \
  template <>                                                                  \
  const InstX86Base<Machine>::Traits::Assembler::GPREmitterShiftOp             \
      InstX86Shr<Machine>::Base::Emitter = {                                   \
          &InstX86Base<Machine>::Traits::Assembler::shr,                       \
          &InstX86Base<Machine>::Traits::Assembler::shr};                      \
                                                                               \
  /* Binary XMM ops */                                                         \
  template <> /* uses specialized emitter. */                                  \
  const InstX86Base<Machine>::Traits::Assembler::XmmEmitterRegOp               \
      InstX86MovssRegs<Machine>::Base::Emitter = {nullptr, nullptr};           \
  template <>                                                                  \
  const InstX86Base<Machine>::Traits::Assembler::XmmEmitterRegOp               \
      InstX86Addss<Machine>::Base::Emitter = {                                 \
          &InstX86Base<Machine>::Traits::Assembler::addss,                     \
          &InstX86Base<Machine>::Traits::Assembler::addss};                    \
  template <>                                                                  \
  const InstX86Base<Machine>::Traits::Assembler::XmmEmitterRegOp               \
      InstX86Addps<Machine>::Base::Emitter = {                                 \
          &InstX86Base<Machine>::Traits::Assembler::addps,                     \
          &InstX86Base<Machine>::Traits::Assembler::addps};                    \
  template <>                                                                  \
  const InstX86Base<Machine>::Traits::Assembler::XmmEmitterRegOp               \
      InstX86Divss<Machine>::Base::Emitter = {                                 \
          &InstX86Base<Machine>::Traits::Assembler::divss,                     \
          &InstX86Base<Machine>::Traits::Assembler::divss};                    \
  template <>                                                                  \
  const InstX86Base<Machine>::Traits::Assembler::XmmEmitterRegOp               \
      InstX86Divps<Machine>::Base::Emitter = {                                 \
          &InstX86Base<Machine>::Traits::Assembler::divps,                     \
          &InstX86Base<Machine>::Traits::Assembler::divps};                    \
  template <>                                                                  \
  const InstX86Base<Machine>::Traits::Assembler::XmmEmitterRegOp               \
      InstX86Mulss<Machine>::Base::Emitter = {                                 \
          &InstX86Base<Machine>::Traits::Assembler::mulss,                     \
          &InstX86Base<Machine>::Traits::Assembler::mulss};                    \
  template <>                                                                  \
  const InstX86Base<Machine>::Traits::Assembler::XmmEmitterRegOp               \
      InstX86Mulps<Machine>::Base::Emitter = {                                 \
          &InstX86Base<Machine>::Traits::Assembler::mulps,                     \
          &InstX86Base<Machine>::Traits::Assembler::mulps};                    \
  template <>                                                                  \
  const InstX86Base<Machine>::Traits::Assembler::XmmEmitterRegOp               \
      InstX86Padd<Machine>::Base::Emitter = {                                  \
          &InstX86Base<Machine>::Traits::Assembler::padd,                      \
          &InstX86Base<Machine>::Traits::Assembler::padd};                     \
  template <>                                                                  \
  const InstX86Base<Machine>::Traits::Assembler::XmmEmitterRegOp               \
      InstX86Pand<Machine>::Base::Emitter = {                                  \
          &InstX86Base<Machine>::Traits::Assembler::pand,                      \
          &InstX86Base<Machine>::Traits::Assembler::pand};                     \
  template <>                                                                  \
  const InstX86Base<Machine>::Traits::Assembler::XmmEmitterRegOp               \
      InstX86Pandn<Machine>::Base::Emitter = {                                 \
          &InstX86Base<Machine>::Traits::Assembler::pandn,                     \
          &InstX86Base<Machine>::Traits::Assembler::pandn};                    \
  template <>                                                                  \
  const InstX86Base<Machine>::Traits::Assembler::XmmEmitterRegOp               \
      InstX86Pcmpeq<Machine>::Base::Emitter = {                                \
          &InstX86Base<Machine>::Traits::Assembler::pcmpeq,                    \
          &InstX86Base<Machine>::Traits::Assembler::pcmpeq};                   \
  template <>                                                                  \
  const InstX86Base<Machine>::Traits::Assembler::XmmEmitterRegOp               \
      InstX86Pcmpgt<Machine>::Base::Emitter = {                                \
          &InstX86Base<Machine>::Traits::Assembler::pcmpgt,                    \
          &InstX86Base<Machine>::Traits::Assembler::pcmpgt};                   \
  template <>                                                                  \
  const InstX86Base<Machine>::Traits::Assembler::XmmEmitterRegOp               \
      InstX86Pmull<Machine>::Base::Emitter = {                                 \
          &InstX86Base<Machine>::Traits::Assembler::pmull,                     \
          &InstX86Base<Machine>::Traits::Assembler::pmull};                    \
  template <>                                                                  \
  const InstX86Base<Machine>::Traits::Assembler::XmmEmitterRegOp               \
      InstX86Pmuludq<Machine>::Base::Emitter = {                               \
          &InstX86Base<Machine>::Traits::Assembler::pmuludq,                   \
          &InstX86Base<Machine>::Traits::Assembler::pmuludq};                  \
  template <>                                                                  \
  const InstX86Base<Machine>::Traits::Assembler::XmmEmitterRegOp               \
      InstX86Por<Machine>::Base::Emitter = {                                   \
          &InstX86Base<Machine>::Traits::Assembler::por,                       \
          &InstX86Base<Machine>::Traits::Assembler::por};                      \
  template <>                                                                  \
  const InstX86Base<Machine>::Traits::Assembler::XmmEmitterRegOp               \
      InstX86Psub<Machine>::Base::Emitter = {                                  \
          &InstX86Base<Machine>::Traits::Assembler::psub,                      \
          &InstX86Base<Machine>::Traits::Assembler::psub};                     \
  template <>                                                                  \
  const InstX86Base<Machine>::Traits::Assembler::XmmEmitterRegOp               \
      InstX86Pxor<Machine>::Base::Emitter = {                                  \
          &InstX86Base<Machine>::Traits::Assembler::pxor,                      \
          &InstX86Base<Machine>::Traits::Assembler::pxor};                     \
  template <>                                                                  \
  const InstX86Base<Machine>::Traits::Assembler::XmmEmitterRegOp               \
      InstX86Subss<Machine>::Base::Emitter = {                                 \
          &InstX86Base<Machine>::Traits::Assembler::subss,                     \
          &InstX86Base<Machine>::Traits::Assembler::subss};                    \
  template <>                                                                  \
  const InstX86Base<Machine>::Traits::Assembler::XmmEmitterRegOp               \
      InstX86Subps<Machine>::Base::Emitter = {                                 \
          &InstX86Base<Machine>::Traits::Assembler::subps,                     \
          &InstX86Base<Machine>::Traits::Assembler::subps};                    \
  template <>                                                                  \
  const InstX86Base<Machine>::Traits::Assembler::XmmEmitterRegOp               \
      InstX86Andnps<Machine>::Base::Emitter = {                                \
          &InstX86Base<Machine>::Traits::Assembler::andnps,                    \
          &InstX86Base<Machine>::Traits::Assembler::andnps};                   \
  template <>                                                                  \
  const InstX86Base<Machine>::Traits::Assembler::XmmEmitterRegOp               \
      InstX86Andps<Machine>::Base::Emitter = {                                 \
          &InstX86Base<Machine>::Traits::Assembler::andps,                     \
          &InstX86Base<Machine>::Traits::Assembler::andps};                    \
  template <>                                                                  \
  const InstX86Base<Machine>::Traits::Assembler::XmmEmitterRegOp               \
      InstX86Maxss<Machine>::Base::Emitter = {                                 \
          &InstX86Base<Machine>::Traits::Assembler::maxss,                     \
          &InstX86Base<Machine>::Traits::Assembler::maxss};                    \
  template <>                                                                  \
  const InstX86Base<Machine>::Traits::Assembler::XmmEmitterRegOp               \
      InstX86Minss<Machine>::Base::Emitter = {                                 \
          &InstX86Base<Machine>::Traits::Assembler::minss,                     \
          &InstX86Base<Machine>::Traits::Assembler::minss};                    \
  template <>                                                                  \
  const InstX86Base<Machine>::Traits::Assembler::XmmEmitterRegOp               \
      InstX86Orps<Machine>::Base::Emitter = {                                  \
          &InstX86Base<Machine>::Traits::Assembler::orps,                      \
          &InstX86Base<Machine>::Traits::Assembler::orps};                     \
  template <>                                                                  \
  const InstX86Base<Machine>::Traits::Assembler::XmmEmitterRegOp               \
      InstX86Xorps<Machine>::Base::Emitter = {                                 \
          &InstX86Base<Machine>::Traits::Assembler::xorps,                     \
          &InstX86Base<Machine>::Traits::Assembler::xorps};                    \
                                                                               \
  /* Binary XMM Shift ops */                                                   \
  template <>                                                                  \
  const InstX86Base<Machine>::Traits::Assembler::XmmEmitterShiftOp             \
      InstX86Psll<Machine>::Base::Emitter = {                                  \
          &InstX86Base<Machine>::Traits::Assembler::psll,                      \
          &InstX86Base<Machine>::Traits::Assembler::psll,                      \
          &InstX86Base<Machine>::Traits::Assembler::psll};                     \
  template <>                                                                  \
  const InstX86Base<Machine>::Traits::Assembler::XmmEmitterShiftOp             \
      InstX86Psra<Machine>::Base::Emitter = {                                  \
          &InstX86Base<Machine>::Traits::Assembler::psra,                      \
          &InstX86Base<Machine>::Traits::Assembler::psra,                      \
          &InstX86Base<Machine>::Traits::Assembler::psra};                     \
  template <>                                                                  \
  const InstX86Base<Machine>::Traits::Assembler::XmmEmitterShiftOp             \
      InstX86Psrl<Machine>::Base::Emitter = {                                  \
          &InstX86Base<Machine>::Traits::Assembler::psrl,                      \
          &InstX86Base<Machine>::Traits::Assembler::psrl,                      \
          &InstX86Base<Machine>::Traits::Assembler::psrl};                     \
  }                                                                            \
  }

} // end of namespace X86Internal
} // end of namespace Ice

#include "IceInstX86BaseImpl.h"

#endif // SUBZERO_SRC_ICEINSTX86BASE_H
