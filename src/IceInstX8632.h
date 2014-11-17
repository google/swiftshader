//===- subzero/src/IceInstX8632.h - Low-level x86 instructions --*- C++ -*-===//
//
//                        The Subzero Code Generator
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares the InstX8632 and OperandX8632 classes and
// their subclasses.  This represents the machine instructions and
// operands used for x86-32 code selection.
//
//===----------------------------------------------------------------------===//

#ifndef SUBZERO_SRC_ICEINSTX8632_H
#define SUBZERO_SRC_ICEINSTX8632_H

#include "assembler_ia32.h"
#include "IceDefs.h"
#include "IceInst.h"
#include "IceConditionCodesX8632.h"
#include "IceInstX8632.def"
#include "IceOperand.h"

namespace Ice {

class TargetX8632;

// OperandX8632 extends the Operand hierarchy.  Its subclasses are
// OperandX8632Mem and VariableSplit.
class OperandX8632 : public Operand {
  OperandX8632(const OperandX8632 &) = delete;
  OperandX8632 &operator=(const OperandX8632 &) = delete;

public:
  enum OperandKindX8632 {
    k__Start = Operand::kTarget,
    kMem,
    kSplit
  };
  using Operand::dump;
  void dump(const Cfg *, Ostream &Str) const override {
    if (ALLOW_DUMP)
      Str << "<OperandX8632>";
  }

protected:
  OperandX8632(OperandKindX8632 Kind, Type Ty)
      : Operand(static_cast<OperandKind>(Kind), Ty) {}
  ~OperandX8632() override {}
};

// OperandX8632Mem represents the m32 addressing mode, with optional
// base and index registers, a constant offset, and a fixed shift
// value for the index register.
class OperandX8632Mem : public OperandX8632 {
  OperandX8632Mem(const OperandX8632Mem &) = delete;
  OperandX8632Mem &operator=(const OperandX8632Mem &) = delete;

public:
  enum SegmentRegisters {
    DefaultSegment = -1,
#define X(val, name, prefix) val,
    SEG_REGX8632_TABLE
#undef X
        SegReg_NUM
  };
  static OperandX8632Mem *create(Cfg *Func, Type Ty, Variable *Base,
                                 Constant *Offset, Variable *Index = NULL,
                                 uint16_t Shift = 0,
                                 SegmentRegisters SegmentReg = DefaultSegment) {
    return new (Func->allocate<OperandX8632Mem>())
        OperandX8632Mem(Func, Ty, Base, Offset, Index, Shift, SegmentReg);
  }
  Variable *getBase() const { return Base; }
  Constant *getOffset() const { return Offset; }
  Variable *getIndex() const { return Index; }
  uint16_t getShift() const { return Shift; }
  SegmentRegisters getSegmentRegister() const { return SegmentReg; }
  void emitSegmentOverride(x86::AssemblerX86 *Asm) const;
  x86::Address toAsmAddress(Assembler *Asm) const;
  void emit(const Cfg *Func) const override;
  using OperandX8632::dump;
  void dump(const Cfg *Func, Ostream &Str) const override;

  static bool classof(const Operand *Operand) {
    return Operand->getKind() == static_cast<OperandKind>(kMem);
  }

private:
  OperandX8632Mem(Cfg *Func, Type Ty, Variable *Base, Constant *Offset,
                  Variable *Index, uint16_t Shift, SegmentRegisters SegmentReg);
  ~OperandX8632Mem() override {}
  Variable *Base;
  Constant *Offset;
  Variable *Index;
  uint16_t Shift;
  SegmentRegisters SegmentReg : 16;
};

// VariableSplit is a way to treat an f64 memory location as a pair
// of i32 locations (Low and High).  This is needed for some cases
// of the Bitcast instruction.  Since it's not possible for integer
// registers to access the XMM registers and vice versa, the
// lowering forces the f64 to be spilled to the stack and then
// accesses through the VariableSplit.
class VariableSplit : public OperandX8632 {
  VariableSplit(const VariableSplit &) = delete;
  VariableSplit &operator=(const VariableSplit &) = delete;

public:
  enum Portion {
    Low,
    High
  };
  static VariableSplit *create(Cfg *Func, Variable *Var, Portion Part) {
    return new (Func->allocate<VariableSplit>()) VariableSplit(Func, Var, Part);
  }
  int32_t getOffset() const { return Part == High ? 4 : 0; }

  x86::Address toAsmAddress(const Cfg *Func) const;
  void emit(const Cfg *Func) const override;
  using OperandX8632::dump;
  void dump(const Cfg *Func, Ostream &Str) const override;

  static bool classof(const Operand *Operand) {
    return Operand->getKind() == static_cast<OperandKind>(kSplit);
  }

private:
  VariableSplit(Cfg *Func, Variable *Var, Portion Part)
      : OperandX8632(kSplit, IceType_i32), Func(Func), Var(Var), Part(Part) {
    assert(Var->getType() == IceType_f64);
    Vars = Func->allocateArrayOf<Variable *>(1);
    Vars[0] = Var;
    NumVars = 1;
  }
  ~VariableSplit() override { Func->deallocateArrayOf<Variable *>(Vars); }
  Cfg *Func; // Held only for the destructor.
  Variable *Var;
  Portion Part;
};

// SpillVariable decorates a Variable by linking it to another
// Variable.  When stack frame offsets are computed, the SpillVariable
// is given a distinct stack slot only if its linked Variable has a
// register.  If the linked Variable has a stack slot, then the
// Variable and SpillVariable share that slot.
class SpillVariable : public Variable {
  SpillVariable(const SpillVariable &) = delete;
  SpillVariable &operator=(const SpillVariable &) = delete;

public:
  static SpillVariable *create(Cfg *Func, Type Ty, SizeT Index,
                               const IceString &Name) {
    return new (Func->allocate<SpillVariable>()) SpillVariable(Ty, Index, Name);
  }
  const static OperandKind SpillVariableKind =
      static_cast<OperandKind>(kVariable_Target);
  static bool classof(const Operand *Operand) {
    return Operand->getKind() == SpillVariableKind;
  }
  void setLinkedTo(Variable *Var) { LinkedTo = Var; }
  Variable *getLinkedTo() const { return LinkedTo; }
  // Inherit dump() and emit() from Variable.
private:
  SpillVariable(Type Ty, SizeT Index, const IceString &Name)
      : Variable(SpillVariableKind, Ty, Index, Name), LinkedTo(NULL) {}
  Variable *LinkedTo;
};

class InstX8632 : public InstTarget {
  InstX8632(const InstX8632 &) = delete;
  InstX8632 &operator=(const InstX8632 &) = delete;

public:
  enum InstKindX8632 {
    k__Start = Inst::Target,
    Adc,
    Add,
    Addps,
    Addss,
    Adjuststack,
    And,
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
    Fld,
    Fstp,
    Icmp,
    Idiv,
    Imul,
    Insertps,
    Label,
    Lea,
    Load,
    Mfence,
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
    Psub,
    Push,
    Pxor,
    Ret,
    Rol,
    Sar,
    Sbb,
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
    Subps,
    Subss,
    Test,
    Ucomiss,
    UD2,
    Xadd,
    Xchg,
    Xor
  };

  static const char *getWidthString(Type Ty);
  static const char *getFldString(Type Ty);
  void dump(const Cfg *Func) const override;

protected:
  InstX8632(Cfg *Func, InstKindX8632 Kind, SizeT Maxsrcs, Variable *Dest)
      : InstTarget(Func, static_cast<InstKind>(Kind), Maxsrcs, Dest) {}
  ~InstX8632() override {}
  static bool isClassof(const Inst *Inst, InstKindX8632 MyKind) {
    return Inst->getKind() == static_cast<InstKind>(MyKind);
  }
};

// InstX8632Label represents an intra-block label that is the target
// of an intra-block branch.  The offset between the label and the
// branch must be fit into one byte (considered "near").  These are
// used for lowering i1 calculations, Select instructions, and 64-bit
// compares on a 32-bit architecture, without basic block splitting.
// Basic block splitting is not so desirable for several reasons, one
// of which is the impact on decisions based on whether a variable's
// live range spans multiple basic blocks.
//
// Intra-block control flow must be used with caution.  Consider the
// sequence for "c = (a >= b ? x : y)".
//     cmp a, b
//     br lt, L1
//     mov c, x
//     jmp L2
//   L1:
//     mov c, y
//   L2:
//
// Labels L1 and L2 are intra-block labels.  Without knowledge of the
// intra-block control flow, liveness analysis will determine the "mov
// c, x" instruction to be dead.  One way to prevent this is to insert
// a "FakeUse(c)" instruction anywhere between the two "mov c, ..."
// instructions, e.g.:
//
//     cmp a, b
//     br lt, L1
//     mov c, x
//     jmp L2
//     FakeUse(c)
//   L1:
//     mov c, y
//   L2:
//
// The down-side is that "mov c, x" can never be dead-code eliminated
// even if there are no uses of c.  As unlikely as this situation is,
// it may be prevented by running dead code elimination before
// lowering.
class InstX8632Label : public InstX8632 {
  InstX8632Label(const InstX8632Label &) = delete;
  InstX8632Label &operator=(const InstX8632Label &) = delete;

public:
  static InstX8632Label *create(Cfg *Func, TargetX8632 *Target) {
    return new (Func->allocate<InstX8632Label>()) InstX8632Label(Func, Target);
  }
  uint32_t getEmitInstCount() const override { return 0; }
  IceString getName(const Cfg *Func) const;
  SizeT getNumber() const { return Number; }
  void emit(const Cfg *Func) const override;
  void emitIAS(const Cfg *Func) const override;
  void dump(const Cfg *Func) const override;

private:
  InstX8632Label(Cfg *Func, TargetX8632 *Target);
  ~InstX8632Label() override {}
  SizeT Number; // used for unique label generation.
};

// Conditional and unconditional branch instruction.
class InstX8632Br : public InstX8632 {
  InstX8632Br(const InstX8632Br &) = delete;
  InstX8632Br &operator=(const InstX8632Br &) = delete;

public:
  // Create a conditional branch to a node.
  static InstX8632Br *create(Cfg *Func, CfgNode *TargetTrue,
                             CfgNode *TargetFalse, CondX86::BrCond Condition) {
    assert(Condition != CondX86::Br_None);
    const InstX8632Label *NoLabel = NULL;
    return new (Func->allocate<InstX8632Br>())
        InstX8632Br(Func, TargetTrue, TargetFalse, NoLabel, Condition);
  }
  // Create an unconditional branch to a node.
  static InstX8632Br *create(Cfg *Func, CfgNode *Target) {
    const CfgNode *NoCondTarget = NULL;
    const InstX8632Label *NoLabel = NULL;
    return new (Func->allocate<InstX8632Br>())
        InstX8632Br(Func, NoCondTarget, Target, NoLabel, CondX86::Br_None);
  }
  // Create a non-terminator conditional branch to a node, with a
  // fallthrough to the next instruction in the current node.  This is
  // used for switch lowering.
  static InstX8632Br *create(Cfg *Func, CfgNode *Target,
                             CondX86::BrCond Condition) {
    assert(Condition != CondX86::Br_None);
    const CfgNode *NoUncondTarget = NULL;
    const InstX8632Label *NoLabel = NULL;
    return new (Func->allocate<InstX8632Br>())
        InstX8632Br(Func, Target, NoUncondTarget, NoLabel, Condition);
  }
  // Create a conditional intra-block branch (or unconditional, if
  // Condition==Br_None) to a label in the current block.
  static InstX8632Br *create(Cfg *Func, InstX8632Label *Label,
                             CondX86::BrCond Condition) {
    const CfgNode *NoCondTarget = NULL;
    const CfgNode *NoUncondTarget = NULL;
    return new (Func->allocate<InstX8632Br>())
        InstX8632Br(Func, NoCondTarget, NoUncondTarget, Label, Condition);
  }
  const CfgNode *getTargetTrue() const { return TargetTrue; }
  const CfgNode *getTargetFalse() const { return TargetFalse; }
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
    return !Label && Condition == CondX86::Br_None;
  }
  bool repointEdge(CfgNode *OldNode, CfgNode *NewNode) override;
  void emit(const Cfg *Func) const override;
  void emitIAS(const Cfg *Func) const override;
  void dump(const Cfg *Func) const override;
  static bool classof(const Inst *Inst) { return isClassof(Inst, Br); }

private:
  InstX8632Br(Cfg *Func, const CfgNode *TargetTrue, const CfgNode *TargetFalse,
              const InstX8632Label *Label, CondX86::BrCond Condition);
  ~InstX8632Br() override {}
  CondX86::BrCond Condition;
  const CfgNode *TargetTrue;
  const CfgNode *TargetFalse;
  const InstX8632Label *Label; // Intra-block branch target
};

// AdjustStack instruction - subtracts esp by the given amount and
// updates the stack offset during code emission.
class InstX8632AdjustStack : public InstX8632 {
  InstX8632AdjustStack(const InstX8632AdjustStack &) = delete;
  InstX8632AdjustStack &operator=(const InstX8632AdjustStack &) = delete;

public:
  static InstX8632AdjustStack *create(Cfg *Func, SizeT Amount, Variable *Esp) {
    return new (Func->allocate<InstX8632AdjustStack>())
        InstX8632AdjustStack(Func, Amount, Esp);
  }
  void emit(const Cfg *Func) const override;
  void emitIAS(const Cfg *Func) const override;
  void dump(const Cfg *Func) const override;
  static bool classof(const Inst *Inst) { return isClassof(Inst, Adjuststack); }

private:
  InstX8632AdjustStack(Cfg *Func, SizeT Amount, Variable *Esp);
  SizeT Amount;
};

// Call instruction.  Arguments should have already been pushed.
class InstX8632Call : public InstX8632 {
  InstX8632Call(const InstX8632Call &) = delete;
  InstX8632Call &operator=(const InstX8632Call &) = delete;

public:
  static InstX8632Call *create(Cfg *Func, Variable *Dest, Operand *CallTarget) {
    return new (Func->allocate<InstX8632Call>())
        InstX8632Call(Func, Dest, CallTarget);
  }
  Operand *getCallTarget() const { return getSrc(0); }
  void emit(const Cfg *Func) const override;
  void emitIAS(const Cfg *Func) const override;
  void dump(const Cfg *Func) const override;
  static bool classof(const Inst *Inst) { return isClassof(Inst, Call); }

private:
  InstX8632Call(Cfg *Func, Variable *Dest, Operand *CallTarget);
  ~InstX8632Call() override {}
};

// Emit a one-operand (GPR) instruction.
void emitIASOpTyGPR(const Cfg *Func, Type Ty, const Operand *Var,
                    const x86::AssemblerX86::GPREmitterOneOp &Emitter);

// Instructions of the form x := op(x).
template <InstX8632::InstKindX8632 K>
class InstX8632InplaceopGPR : public InstX8632 {
  InstX8632InplaceopGPR(const InstX8632InplaceopGPR &) = delete;
  InstX8632InplaceopGPR &operator=(const InstX8632InplaceopGPR &) = delete;

public:
  static InstX8632InplaceopGPR *create(Cfg *Func, Operand *SrcDest) {
    return new (Func->allocate<InstX8632InplaceopGPR>())
        InstX8632InplaceopGPR(Func, SrcDest);
  }
  void emit(const Cfg *Func) const override {
    if (!ALLOW_DUMP)
      return;
    Ostream &Str = Func->getContext()->getStrEmit();
    assert(getSrcSize() == 1);
    Str << "\t" << Opcode << "\t";
    getSrc(0)->emit(Func);
  }
  void emitIAS(const Cfg *Func) const override {
    assert(getSrcSize() == 1);
    const Variable *Var = getDest();
    Type Ty = Var->getType();
    emitIASOpTyGPR(Func, Ty, Var, Emitter);
  }
  void dump(const Cfg *Func) const override {
    if (!ALLOW_DUMP)
      return;
    Ostream &Str = Func->getContext()->getStrDump();
    dumpDest(Func);
    Str << " = " << Opcode << "." << getDest()->getType() << " ";
    dumpSources(Func);
  }
  static bool classof(const Inst *Inst) { return isClassof(Inst, K); }

private:
  InstX8632InplaceopGPR(Cfg *Func, Operand *SrcDest)
      : InstX8632(Func, K, 1, llvm::dyn_cast<Variable>(SrcDest)) {
    addSource(SrcDest);
  }
  ~InstX8632InplaceopGPR() override {}
  static const char *Opcode;
  static const x86::AssemblerX86::GPREmitterOneOp Emitter;
};

// Emit a two-operand (GPR) instruction, where the dest operand is a
// Variable that's guaranteed to be a register.
template <bool VarCanBeByte = true, bool SrcCanBeByte = true>
void emitIASRegOpTyGPR(const Cfg *Func, Type Ty, const Variable *Dst,
                       const Operand *Src,
                       const x86::AssemblerX86::GPREmitterRegOp &Emitter);

// Instructions of the form x := op(y).
template <InstX8632::InstKindX8632 K>
class InstX8632UnaryopGPR : public InstX8632 {
  InstX8632UnaryopGPR(const InstX8632UnaryopGPR &) = delete;
  InstX8632UnaryopGPR &operator=(const InstX8632UnaryopGPR &) = delete;

public:
  static InstX8632UnaryopGPR *create(Cfg *Func, Variable *Dest, Operand *Src) {
    return new (Func->allocate<InstX8632UnaryopGPR>())
        InstX8632UnaryopGPR(Func, Dest, Src);
  }
  void emit(const Cfg *Func) const override {
    if (!ALLOW_DUMP)
      return;
    Ostream &Str = Func->getContext()->getStrEmit();
    assert(getSrcSize() == 1);
    Type SrcTy = getSrc(0)->getType();
    Type DestTy = getDest()->getType();
    Str << "\t" << Opcode << getWidthString(SrcTy);
    // Movsx and movzx need both the source and dest type width letter
    // to define the operation.  The other unary operations have the
    // same source and dest type and as a result need only one letter.
    if (SrcTy != DestTy)
      Str << getWidthString(DestTy);
    Str << "\t";
    getSrc(0)->emit(Func);
    Str << ", ";
    getDest()->emit(Func);
  }
  void emitIAS(const Cfg *Func) const override {
    assert(getSrcSize() == 1);
    const Variable *Var = getDest();
    Type Ty = Var->getType();
    const Operand *Src = getSrc(0);
    emitIASRegOpTyGPR(Func, Ty, Var, Src, Emitter);
  }
  void dump(const Cfg *Func) const override {
    if (!ALLOW_DUMP)
      return;
    Ostream &Str = Func->getContext()->getStrDump();
    dumpDest(Func);
    Str << " = " << Opcode << "." << getSrc(0)->getType() << " ";
    dumpSources(Func);
  }
  static bool classof(const Inst *Inst) { return isClassof(Inst, K); }

private:
  InstX8632UnaryopGPR(Cfg *Func, Variable *Dest, Operand *Src)
      : InstX8632(Func, K, 1, Dest) {
    addSource(Src);
  }
  ~InstX8632UnaryopGPR() override {}
  static const char *Opcode;
  static const x86::AssemblerX86::GPREmitterRegOp Emitter;
};

void emitIASRegOpTyXMM(const Cfg *Func, Type Ty, const Variable *Var,
                       const Operand *Src,
                       const x86::AssemblerX86::XmmEmitterRegOp &Emitter);

template <InstX8632::InstKindX8632 K>
class InstX8632UnaryopXmm : public InstX8632 {
  InstX8632UnaryopXmm(const InstX8632UnaryopXmm &) = delete;
  InstX8632UnaryopXmm &operator=(const InstX8632UnaryopXmm &) = delete;

public:
  static InstX8632UnaryopXmm *create(Cfg *Func, Variable *Dest, Operand *Src) {
    return new (Func->allocate<InstX8632UnaryopXmm>())
        InstX8632UnaryopXmm(Func, Dest, Src);
  }
  void emit(const Cfg *Func) const override {
    if (!ALLOW_DUMP)
      return;
    Ostream &Str = Func->getContext()->getStrEmit();
    assert(getSrcSize() == 1);
    Str << "\t" << Opcode << "\t";
    getSrc(0)->emit(Func);
    Str << ", ";
    getDest()->emit(Func);
  }
  void emitIAS(const Cfg *Func) const override {
    Type Ty = getDest()->getType();
    assert(getSrcSize() == 1);
    emitIASRegOpTyXMM(Func, Ty, getDest(), getSrc(0), Emitter);
  }
  void dump(const Cfg *Func) const override {
    if (!ALLOW_DUMP)
      return;
    Ostream &Str = Func->getContext()->getStrDump();
    dumpDest(Func);
    Str << " = " << Opcode << "." << getDest()->getType() << " ";
    dumpSources(Func);
  }
  static bool classof(const Inst *Inst) { return isClassof(Inst, K); }

private:
  InstX8632UnaryopXmm(Cfg *Func, Variable *Dest, Operand *Src)
      : InstX8632(Func, K, 1, Dest) {
    addSource(Src);
  }
  ~InstX8632UnaryopXmm() override {}
  static const char *Opcode;
  static const x86::AssemblerX86::XmmEmitterRegOp Emitter;
};

// See the definition of emitTwoAddress() for a description of
// ShiftHack.
void emitTwoAddress(const char *Opcode, const Inst *Inst, const Cfg *Func,
                    bool ShiftHack = false);

void emitIASGPRShift(const Cfg *Func, Type Ty, const Variable *Var,
                     const Operand *Src,
                     const x86::AssemblerX86::GPREmitterShiftOp &Emitter);

template <InstX8632::InstKindX8632 K>
class InstX8632BinopGPRShift : public InstX8632 {
  InstX8632BinopGPRShift(const InstX8632BinopGPRShift &) = delete;
  InstX8632BinopGPRShift &operator=(const InstX8632BinopGPRShift &) = delete;

public:
  // Create a binary-op GPR shift instruction.
  static InstX8632BinopGPRShift *create(Cfg *Func, Variable *Dest,
                                        Operand *Source) {
    return new (Func->allocate<InstX8632BinopGPRShift>())
        InstX8632BinopGPRShift(Func, Dest, Source);
  }
  void emit(const Cfg *Func) const override {
    if (!ALLOW_DUMP)
      return;
    const bool ShiftHack = true;
    emitTwoAddress(Opcode, this, Func, ShiftHack);
  }
  void emitIAS(const Cfg *Func) const override {
    Type Ty = getDest()->getType();
    assert(getSrcSize() == 2);
    emitIASGPRShift(Func, Ty, getDest(), getSrc(1), Emitter);
  }
  void dump(const Cfg *Func) const override {
    if (!ALLOW_DUMP)
      return;
    Ostream &Str = Func->getContext()->getStrDump();
    dumpDest(Func);
    Str << " = " << Opcode << "." << getDest()->getType() << " ";
    dumpSources(Func);
  }
  static bool classof(const Inst *Inst) { return isClassof(Inst, K); }

private:
  InstX8632BinopGPRShift(Cfg *Func, Variable *Dest, Operand *Source)
      : InstX8632(Func, K, 2, Dest) {
    addSource(Dest);
    addSource(Source);
  }
  ~InstX8632BinopGPRShift() override {}
  static const char *Opcode;
  static const x86::AssemblerX86::GPREmitterShiftOp Emitter;
};

template <InstX8632::InstKindX8632 K>
class InstX8632BinopGPR : public InstX8632 {
  InstX8632BinopGPR(const InstX8632BinopGPR &) = delete;
  InstX8632BinopGPR &operator=(const InstX8632BinopGPR &) = delete;

public:
  // Create an ordinary binary-op instruction like add or sub.
  static InstX8632BinopGPR *create(Cfg *Func, Variable *Dest, Operand *Source) {
    return new (Func->allocate<InstX8632BinopGPR>())
        InstX8632BinopGPR(Func, Dest, Source);
  }
  void emit(const Cfg *Func) const override {
    if (!ALLOW_DUMP)
      return;
    const bool ShiftHack = false;
    emitTwoAddress(Opcode, this, Func, ShiftHack);
  }
  void emitIAS(const Cfg *Func) const override {
    Type Ty = getDest()->getType();
    assert(getSrcSize() == 2);
    emitIASRegOpTyGPR(Func, Ty, getDest(), getSrc(1), Emitter);
  }
  void dump(const Cfg *Func) const override {
    if (!ALLOW_DUMP)
      return;
    Ostream &Str = Func->getContext()->getStrDump();
    dumpDest(Func);
    Str << " = " << Opcode << "." << getDest()->getType() << " ";
    dumpSources(Func);
  }
  static bool classof(const Inst *Inst) { return isClassof(Inst, K); }

private:
  InstX8632BinopGPR(Cfg *Func, Variable *Dest, Operand *Source)
      : InstX8632(Func, K, 2, Dest) {
    addSource(Dest);
    addSource(Source);
  }
  ~InstX8632BinopGPR() override {}
  static const char *Opcode;
  static const x86::AssemblerX86::GPREmitterRegOp Emitter;
};

template <InstX8632::InstKindX8632 K, bool NeedsElementType>
class InstX8632BinopXmm : public InstX8632 {
  InstX8632BinopXmm(const InstX8632BinopXmm &) = delete;
  InstX8632BinopXmm &operator=(const InstX8632BinopXmm &) = delete;

public:
  // Create an XMM binary-op instruction like addss or addps.
  static InstX8632BinopXmm *create(Cfg *Func, Variable *Dest, Operand *Source) {
    return new (Func->allocate<InstX8632BinopXmm>())
        InstX8632BinopXmm(Func, Dest, Source);
  }
  void emit(const Cfg *Func) const override {
    if (!ALLOW_DUMP)
      return;
    const bool ShiftHack = false;
    emitTwoAddress(Opcode, this, Func, ShiftHack);
  }
  void emitIAS(const Cfg *Func) const override {
    Type Ty = getDest()->getType();
    if (NeedsElementType)
      Ty = typeElementType(Ty);
    assert(getSrcSize() == 2);
    emitIASRegOpTyXMM(Func, Ty, getDest(), getSrc(1), Emitter);
  }
  void dump(const Cfg *Func) const override {
    if (!ALLOW_DUMP)
      return;
    Ostream &Str = Func->getContext()->getStrDump();
    dumpDest(Func);
    Str << " = " << Opcode << "." << getDest()->getType() << " ";
    dumpSources(Func);
  }
  static bool classof(const Inst *Inst) { return isClassof(Inst, K); }

private:
  InstX8632BinopXmm(Cfg *Func, Variable *Dest, Operand *Source)
      : InstX8632(Func, K, 2, Dest) {
    addSource(Dest);
    addSource(Source);
  }
  ~InstX8632BinopXmm() override {}
  static const char *Opcode;
  static const x86::AssemblerX86::XmmEmitterRegOp Emitter;
};

void emitIASXmmShift(const Cfg *Func, Type Ty, const Variable *Var,
                     const Operand *Src,
                     const x86::AssemblerX86::XmmEmitterShiftOp &Emitter);

template <InstX8632::InstKindX8632 K>
class InstX8632BinopXmmShift : public InstX8632 {
  InstX8632BinopXmmShift(const InstX8632BinopXmmShift &) = delete;
  InstX8632BinopXmmShift &operator=(const InstX8632BinopXmmShift &) = delete;

public:
  // Create an XMM binary-op shift operation.
  static InstX8632BinopXmmShift *create(Cfg *Func, Variable *Dest,
                                        Operand *Source) {
    return new (Func->allocate<InstX8632BinopXmmShift>())
        InstX8632BinopXmmShift(Func, Dest, Source);
  }
  void emit(const Cfg *Func) const override {
    if (!ALLOW_DUMP)
      return;
    const bool ShiftHack = false;
    emitTwoAddress(Opcode, this, Func, ShiftHack);
  }
  void emitIAS(const Cfg *Func) const override {
    Type Ty = getDest()->getType();
    assert(Ty == IceType_v8i16 || Ty == IceType_v8i1 || Ty == IceType_v4i32 ||
           Ty == IceType_v4i1);
    Type ElementTy = typeElementType(Ty);
    assert(getSrcSize() == 2);
    emitIASXmmShift(Func, ElementTy, getDest(), getSrc(1), Emitter);
  }
  void dump(const Cfg *Func) const override {
    if (!ALLOW_DUMP)
      return;
    Ostream &Str = Func->getContext()->getStrDump();
    dumpDest(Func);
    Str << " = " << Opcode << "." << getDest()->getType() << " ";
    dumpSources(Func);
  }
  static bool classof(const Inst *Inst) { return isClassof(Inst, K); }

private:
  InstX8632BinopXmmShift(Cfg *Func, Variable *Dest, Operand *Source)
      : InstX8632(Func, K, 2, Dest) {
    addSource(Dest);
    addSource(Source);
  }
  ~InstX8632BinopXmmShift() override {}
  static const char *Opcode;
  static const x86::AssemblerX86::XmmEmitterShiftOp Emitter;
};

template <InstX8632::InstKindX8632 K> class InstX8632Ternop : public InstX8632 {
  InstX8632Ternop(const InstX8632Ternop &) = delete;
  InstX8632Ternop &operator=(const InstX8632Ternop &) = delete;

public:
  // Create a ternary-op instruction like div or idiv.
  static InstX8632Ternop *create(Cfg *Func, Variable *Dest, Operand *Source1,
                                 Operand *Source2) {
    return new (Func->allocate<InstX8632Ternop>())
        InstX8632Ternop(Func, Dest, Source1, Source2);
  }
  void emit(const Cfg *Func) const override {
    if (!ALLOW_DUMP)
      return;
    Ostream &Str = Func->getContext()->getStrEmit();
    assert(getSrcSize() == 3);
    Str << "\t" << Opcode << "\t";
    getSrc(2)->emit(Func);
    Str << ", ";
    getSrc(1)->emit(Func);
    Str << ", ";
    getDest()->emit(Func);
  }
  void emitIAS(const Cfg *Func) const override;
  void dump(const Cfg *Func) const override {
    if (!ALLOW_DUMP)
      return;
    Ostream &Str = Func->getContext()->getStrDump();
    dumpDest(Func);
    Str << " = " << Opcode << "." << getDest()->getType() << " ";
    dumpSources(Func);
  }
  static bool classof(const Inst *Inst) { return isClassof(Inst, K); }

private:
  InstX8632Ternop(Cfg *Func, Variable *Dest, Operand *Source1, Operand *Source2)
      : InstX8632(Func, K, 3, Dest) {
    addSource(Dest);
    addSource(Source1);
    addSource(Source2);
  }
  ~InstX8632Ternop() override {}
  static const char *Opcode;
};

// Instructions of the form x := y op z
template <InstX8632::InstKindX8632 K>
class InstX8632ThreeAddressop : public InstX8632 {
  InstX8632ThreeAddressop(const InstX8632ThreeAddressop &) = delete;
  InstX8632ThreeAddressop &operator=(const InstX8632ThreeAddressop &) = delete;

public:
  static InstX8632ThreeAddressop *create(Cfg *Func, Variable *Dest,
                                         Operand *Source0, Operand *Source1) {
    return new (Func->allocate<InstX8632ThreeAddressop>())
        InstX8632ThreeAddressop(Func, Dest, Source0, Source1);
  }
  void emit(const Cfg *Func) const override {
    if (!ALLOW_DUMP)
      return;
    Ostream &Str = Func->getContext()->getStrEmit();
    assert(getSrcSize() == 2);
    Str << "\t" << Opcode << "\t";
    getSrc(1)->emit(Func);
    Str << ", ";
    getSrc(0)->emit(Func);
    Str << ", ";
    getDest()->emit(Func);
  }
  void emitIAS(const Cfg *Func) const override;
  void dump(const Cfg *Func) const override {
    if (!ALLOW_DUMP)
      return;
    Ostream &Str = Func->getContext()->getStrDump();
    dumpDest(Func);
    Str << " = " << Opcode << "." << getDest()->getType() << " ";
    dumpSources(Func);
  }
  static bool classof(const Inst *Inst) { return isClassof(Inst, K); }

private:
  InstX8632ThreeAddressop(Cfg *Func, Variable *Dest, Operand *Source0,
                          Operand *Source1)
      : InstX8632(Func, K, 2, Dest) {
    addSource(Source0);
    addSource(Source1);
  }
  ~InstX8632ThreeAddressop() override {}
  static const char *Opcode;
};

bool checkForRedundantAssign(const Variable *Dest, const Operand *Source);

// Base class for assignment instructions
template <InstX8632::InstKindX8632 K>
class InstX8632Movlike : public InstX8632 {
  InstX8632Movlike(const InstX8632Movlike &) = delete;
  InstX8632Movlike &operator=(const InstX8632Movlike &) = delete;

public:
  static InstX8632Movlike *create(Cfg *Func, Variable *Dest, Operand *Source) {
    return new (Func->allocate<InstX8632Movlike>())
        InstX8632Movlike(Func, Dest, Source);
  }
  bool isRedundantAssign() const override {
    return checkForRedundantAssign(getDest(), getSrc(0));
  }
  bool isSimpleAssign() const override { return true; }
  void emit(const Cfg *Func) const override;
  void emitIAS(const Cfg *Func) const override;
  void dump(const Cfg *Func) const override {
    if (!ALLOW_DUMP)
      return;
    Ostream &Str = Func->getContext()->getStrDump();
    Str << Opcode << "." << getDest()->getType() << " ";
    dumpDest(Func);
    Str << ", ";
    dumpSources(Func);
  }
  static bool classof(const Inst *Inst) { return isClassof(Inst, K); }

private:
  InstX8632Movlike(Cfg *Func, Variable *Dest, Operand *Source)
      : InstX8632(Func, K, 1, Dest) {
    addSource(Source);
  }
  ~InstX8632Movlike() override {}

  static const char *Opcode;
};

typedef InstX8632InplaceopGPR<InstX8632::Bswap> InstX8632Bswap;
typedef InstX8632InplaceopGPR<InstX8632::Neg> InstX8632Neg;
typedef InstX8632UnaryopGPR<InstX8632::Bsf> InstX8632Bsf;
typedef InstX8632UnaryopGPR<InstX8632::Bsr> InstX8632Bsr;
typedef InstX8632UnaryopGPR<InstX8632::Lea> InstX8632Lea;
// Cbwdq instruction - wrapper for cbw, cwd, and cdq
typedef InstX8632UnaryopGPR<InstX8632::Cbwdq> InstX8632Cbwdq;
typedef InstX8632UnaryopGPR<InstX8632::Movsx> InstX8632Movsx;
typedef InstX8632UnaryopGPR<InstX8632::Movzx> InstX8632Movzx;
typedef InstX8632UnaryopXmm<InstX8632::Movd> InstX8632Movd;
typedef InstX8632UnaryopXmm<InstX8632::Sqrtss> InstX8632Sqrtss;
// Move/assignment instruction - wrapper for mov/movss/movsd.
typedef InstX8632Movlike<InstX8632::Mov> InstX8632Mov;
// Move packed - copy 128 bit values between XMM registers, or mem128
// and XMM registers.
typedef InstX8632Movlike<InstX8632::Movp> InstX8632Movp;
// Movq - copy between XMM registers, or mem64 and XMM registers.
typedef InstX8632Movlike<InstX8632::Movq> InstX8632Movq;
typedef InstX8632BinopGPR<InstX8632::Add> InstX8632Add;
typedef InstX8632BinopXmm<InstX8632::Addps, true> InstX8632Addps;
typedef InstX8632BinopGPR<InstX8632::Adc> InstX8632Adc;
typedef InstX8632BinopXmm<InstX8632::Addss, false> InstX8632Addss;
typedef InstX8632BinopXmm<InstX8632::Padd, true> InstX8632Padd;
typedef InstX8632BinopGPR<InstX8632::Sub> InstX8632Sub;
typedef InstX8632BinopXmm<InstX8632::Subps, true> InstX8632Subps;
typedef InstX8632BinopXmm<InstX8632::Subss, false> InstX8632Subss;
typedef InstX8632BinopGPR<InstX8632::Sbb> InstX8632Sbb;
typedef InstX8632BinopXmm<InstX8632::Psub, true> InstX8632Psub;
typedef InstX8632BinopGPR<InstX8632::And> InstX8632And;
typedef InstX8632BinopXmm<InstX8632::Pand, false> InstX8632Pand;
typedef InstX8632BinopXmm<InstX8632::Pandn, false> InstX8632Pandn;
typedef InstX8632BinopGPR<InstX8632::Or> InstX8632Or;
typedef InstX8632BinopXmm<InstX8632::Por, false> InstX8632Por;
typedef InstX8632BinopGPR<InstX8632::Xor> InstX8632Xor;
typedef InstX8632BinopXmm<InstX8632::Pxor, false> InstX8632Pxor;
typedef InstX8632BinopGPR<InstX8632::Imul> InstX8632Imul;
typedef InstX8632BinopXmm<InstX8632::Mulps, true> InstX8632Mulps;
typedef InstX8632BinopXmm<InstX8632::Mulss, false> InstX8632Mulss;
typedef InstX8632BinopXmm<InstX8632::Pmull, true> InstX8632Pmull;
typedef InstX8632BinopXmm<InstX8632::Pmuludq, false> InstX8632Pmuludq;
typedef InstX8632BinopXmm<InstX8632::Divps, true> InstX8632Divps;
typedef InstX8632BinopXmm<InstX8632::Divss, false> InstX8632Divss;
typedef InstX8632BinopGPRShift<InstX8632::Rol> InstX8632Rol;
typedef InstX8632BinopGPRShift<InstX8632::Shl> InstX8632Shl;
typedef InstX8632BinopXmmShift<InstX8632::Psll> InstX8632Psll;
typedef InstX8632BinopGPRShift<InstX8632::Shr> InstX8632Shr;
typedef InstX8632BinopGPRShift<InstX8632::Sar> InstX8632Sar;
typedef InstX8632BinopXmmShift<InstX8632::Psra> InstX8632Psra;
typedef InstX8632BinopXmm<InstX8632::Pcmpeq, true> InstX8632Pcmpeq;
typedef InstX8632BinopXmm<InstX8632::Pcmpgt, true> InstX8632Pcmpgt;
// movss is only a binary operation when the source and dest
// operands are both registers (the high bits of dest are left untouched).
// In other cases, it behaves like a copy (mov-like) operation (and the
// high bits of dest are cleared).
// InstX8632Movss will assert that both its source and dest operands are
// registers, so the lowering code should use _mov instead of _movss
// in cases where a copy operation is intended.
typedef InstX8632BinopXmm<InstX8632::MovssRegs, false> InstX8632MovssRegs;
typedef InstX8632Ternop<InstX8632::Idiv> InstX8632Idiv;
typedef InstX8632Ternop<InstX8632::Div> InstX8632Div;
typedef InstX8632Ternop<InstX8632::Insertps> InstX8632Insertps;
typedef InstX8632Ternop<InstX8632::Pinsr> InstX8632Pinsr;
typedef InstX8632Ternop<InstX8632::Shufps> InstX8632Shufps;
typedef InstX8632Ternop<InstX8632::Blendvps> InstX8632Blendvps;
typedef InstX8632Ternop<InstX8632::Pblendvb> InstX8632Pblendvb;
typedef InstX8632ThreeAddressop<InstX8632::Pextr> InstX8632Pextr;
typedef InstX8632ThreeAddressop<InstX8632::Pshufd> InstX8632Pshufd;

// Base class for a lockable x86-32 instruction (emits a locked prefix).
class InstX8632Lockable : public InstX8632 {
  InstX8632Lockable(const InstX8632Lockable &) = delete;
  InstX8632Lockable &operator=(const InstX8632Lockable &) = delete;

protected:
  bool Locked;

  InstX8632Lockable(Cfg *Func, InstKindX8632 Kind, SizeT Maxsrcs,
                    Variable *Dest, bool Locked)
      : InstX8632(Func, Kind, Maxsrcs, Dest), Locked(Locked) {
    // Assume that such instructions are used for Atomics and be careful
    // with optimizations.
    HasSideEffects = Locked;
  }
  ~InstX8632Lockable() override {}
};

// Mul instruction - unsigned multiply.
class InstX8632Mul : public InstX8632 {
  InstX8632Mul(const InstX8632Mul &) = delete;
  InstX8632Mul &operator=(const InstX8632Mul &) = delete;

public:
  static InstX8632Mul *create(Cfg *Func, Variable *Dest, Variable *Source1,
                              Operand *Source2) {
    return new (Func->allocate<InstX8632Mul>())
        InstX8632Mul(Func, Dest, Source1, Source2);
  }
  void emit(const Cfg *Func) const override;
  void emitIAS(const Cfg *Func) const override;
  void dump(const Cfg *Func) const override;
  static bool classof(const Inst *Inst) { return isClassof(Inst, Mul); }

private:
  InstX8632Mul(Cfg *Func, Variable *Dest, Variable *Source1, Operand *Source2);
  ~InstX8632Mul() override {}
};

// Shld instruction - shift across a pair of operands.
class InstX8632Shld : public InstX8632 {
  InstX8632Shld(const InstX8632Shld &) = delete;
  InstX8632Shld &operator=(const InstX8632Shld &) = delete;

public:
  static InstX8632Shld *create(Cfg *Func, Variable *Dest, Variable *Source1,
                               Variable *Source2) {
    return new (Func->allocate<InstX8632Shld>())
        InstX8632Shld(Func, Dest, Source1, Source2);
  }
  void emit(const Cfg *Func) const override;
  void emitIAS(const Cfg *Func) const override;
  void dump(const Cfg *Func) const override;
  static bool classof(const Inst *Inst) { return isClassof(Inst, Shld); }

private:
  InstX8632Shld(Cfg *Func, Variable *Dest, Variable *Source1,
                Variable *Source2);
  ~InstX8632Shld() override {}
};

// Shrd instruction - shift across a pair of operands.
class InstX8632Shrd : public InstX8632 {
  InstX8632Shrd(const InstX8632Shrd &) = delete;
  InstX8632Shrd &operator=(const InstX8632Shrd &) = delete;

public:
  static InstX8632Shrd *create(Cfg *Func, Variable *Dest, Variable *Source1,
                               Variable *Source2) {
    return new (Func->allocate<InstX8632Shrd>())
        InstX8632Shrd(Func, Dest, Source1, Source2);
  }
  void emit(const Cfg *Func) const override;
  void emitIAS(const Cfg *Func) const override;
  void dump(const Cfg *Func) const override;
  static bool classof(const Inst *Inst) { return isClassof(Inst, Shrd); }

private:
  InstX8632Shrd(Cfg *Func, Variable *Dest, Variable *Source1,
                Variable *Source2);
  ~InstX8632Shrd() override {}
};

// Conditional move instruction.
class InstX8632Cmov : public InstX8632 {
  InstX8632Cmov(const InstX8632Cmov &) = delete;
  InstX8632Cmov &operator=(const InstX8632Cmov &) = delete;

public:
  static InstX8632Cmov *create(Cfg *Func, Variable *Dest, Operand *Source,
                               CondX86::BrCond Cond) {
    return new (Func->allocate<InstX8632Cmov>())
        InstX8632Cmov(Func, Dest, Source, Cond);
  }
  void emit(const Cfg *Func) const override;
  void emitIAS(const Cfg *Func) const override;
  void dump(const Cfg *Func) const override;
  static bool classof(const Inst *Inst) { return isClassof(Inst, Cmov); }

private:
  InstX8632Cmov(Cfg *Func, Variable *Dest, Operand *Source,
                CondX86::BrCond Cond);
  ~InstX8632Cmov() override {}

  CondX86::BrCond Condition;
};

// Cmpps instruction - compare packed singled-precision floating point
// values
class InstX8632Cmpps : public InstX8632 {
  InstX8632Cmpps(const InstX8632Cmpps &) = delete;
  InstX8632Cmpps &operator=(const InstX8632Cmpps &) = delete;

public:
  static InstX8632Cmpps *create(Cfg *Func, Variable *Dest, Operand *Source,
                                CondX86::CmppsCond Condition) {
    return new (Func->allocate<InstX8632Cmpps>())
        InstX8632Cmpps(Func, Dest, Source, Condition);
  }
  void emit(const Cfg *Func) const override;
  void emitIAS(const Cfg *Func) const override;
  void dump(const Cfg *Func) const override;
  static bool classof(const Inst *Inst) { return isClassof(Inst, Cmpps); }

private:
  InstX8632Cmpps(Cfg *Func, Variable *Dest, Operand *Source,
                 CondX86::CmppsCond Cond);
  ~InstX8632Cmpps() override {}

  CondX86::CmppsCond Condition;
};

// Cmpxchg instruction - cmpxchg <dest>, <desired> will compare if <dest>
// equals eax. If so, the ZF is set and <desired> is stored in <dest>.
// If not, ZF is cleared and <dest> is copied to eax (or subregister).
// <dest> can be a register or memory, while <desired> must be a register.
// It is the user's responsiblity to mark eax with a FakeDef.
class InstX8632Cmpxchg : public InstX8632Lockable {
  InstX8632Cmpxchg(const InstX8632Cmpxchg &) = delete;
  InstX8632Cmpxchg &operator=(const InstX8632Cmpxchg &) = delete;

public:
  static InstX8632Cmpxchg *create(Cfg *Func, Operand *DestOrAddr, Variable *Eax,
                                  Variable *Desired, bool Locked) {
    return new (Func->allocate<InstX8632Cmpxchg>())
        InstX8632Cmpxchg(Func, DestOrAddr, Eax, Desired, Locked);
  }
  void emit(const Cfg *Func) const override;
  void emitIAS(const Cfg *Func) const override;
  void dump(const Cfg *Func) const override;
  static bool classof(const Inst *Inst) { return isClassof(Inst, Cmpxchg); }

private:
  InstX8632Cmpxchg(Cfg *Func, Operand *DestOrAddr, Variable *Eax,
                   Variable *Desired, bool Locked);
  ~InstX8632Cmpxchg() override {}
};

// Cmpxchg8b instruction - cmpxchg8b <m64> will compare if <m64>
// equals edx:eax. If so, the ZF is set and ecx:ebx is stored in <m64>.
// If not, ZF is cleared and <m64> is copied to edx:eax.
// The caller is responsible for inserting FakeDefs to mark edx
// and eax as modified.
// <m64> must be a memory operand.
class InstX8632Cmpxchg8b : public InstX8632Lockable {
  InstX8632Cmpxchg8b(const InstX8632Cmpxchg8b &) = delete;
  InstX8632Cmpxchg8b &operator=(const InstX8632Cmpxchg8b &) = delete;

public:
  static InstX8632Cmpxchg8b *create(Cfg *Func, OperandX8632Mem *Dest,
                                    Variable *Edx, Variable *Eax, Variable *Ecx,
                                    Variable *Ebx, bool Locked) {
    return new (Func->allocate<InstX8632Cmpxchg8b>())
        InstX8632Cmpxchg8b(Func, Dest, Edx, Eax, Ecx, Ebx, Locked);
  }
  void emit(const Cfg *Func) const override;
  void emitIAS(const Cfg *Func) const override;
  void dump(const Cfg *Func) const override;
  static bool classof(const Inst *Inst) { return isClassof(Inst, Cmpxchg8b); }

private:
  InstX8632Cmpxchg8b(Cfg *Func, OperandX8632Mem *Dest, Variable *Edx,
                     Variable *Eax, Variable *Ecx, Variable *Ebx, bool Locked);
  ~InstX8632Cmpxchg8b() override {}
};

// Cvt instruction - wrapper for cvtsX2sY where X and Y are in {s,d,i}
// as appropriate.  s=float, d=double, i=int.  X and Y are determined
// from dest/src types.  Sign and zero extension on the integer
// operand needs to be done separately.
class InstX8632Cvt : public InstX8632 {
  InstX8632Cvt(const InstX8632Cvt &) = delete;
  InstX8632Cvt &operator=(const InstX8632Cvt &) = delete;

public:
  enum CvtVariant { Si2ss, Tss2si, Float2float, Dq2ps, Tps2dq };
  static InstX8632Cvt *create(Cfg *Func, Variable *Dest, Operand *Source,
                              CvtVariant Variant) {
    return new (Func->allocate<InstX8632Cvt>())
        InstX8632Cvt(Func, Dest, Source, Variant);
  }
  void emit(const Cfg *Func) const override;
  void emitIAS(const Cfg *Func) const override;
  void dump(const Cfg *Func) const override;
  static bool classof(const Inst *Inst) { return isClassof(Inst, Cvt); }
  bool isTruncating() const { return Variant == Tss2si || Variant == Tps2dq; }

private:
  CvtVariant Variant;
  InstX8632Cvt(Cfg *Func, Variable *Dest, Operand *Source, CvtVariant Variant);
  ~InstX8632Cvt() override {}
};

// cmp - Integer compare instruction.
class InstX8632Icmp : public InstX8632 {
  InstX8632Icmp(const InstX8632Icmp &) = delete;
  InstX8632Icmp &operator=(const InstX8632Icmp &) = delete;

public:
  static InstX8632Icmp *create(Cfg *Func, Operand *Src1, Operand *Src2) {
    return new (Func->allocate<InstX8632Icmp>())
        InstX8632Icmp(Func, Src1, Src2);
  }
  void emit(const Cfg *Func) const override;
  void emitIAS(const Cfg *Func) const override;
  void dump(const Cfg *Func) const override;
  static bool classof(const Inst *Inst) { return isClassof(Inst, Icmp); }

private:
  InstX8632Icmp(Cfg *Func, Operand *Src1, Operand *Src2);
  ~InstX8632Icmp() override {}
};

// ucomiss/ucomisd - floating-point compare instruction.
class InstX8632Ucomiss : public InstX8632 {
  InstX8632Ucomiss(const InstX8632Ucomiss &) = delete;
  InstX8632Ucomiss &operator=(const InstX8632Ucomiss &) = delete;

public:
  static InstX8632Ucomiss *create(Cfg *Func, Operand *Src1, Operand *Src2) {
    return new (Func->allocate<InstX8632Ucomiss>())
        InstX8632Ucomiss(Func, Src1, Src2);
  }
  void emit(const Cfg *Func) const override;
  void emitIAS(const Cfg *Func) const override;
  void dump(const Cfg *Func) const override;
  static bool classof(const Inst *Inst) { return isClassof(Inst, Ucomiss); }

private:
  InstX8632Ucomiss(Cfg *Func, Operand *Src1, Operand *Src2);
  ~InstX8632Ucomiss() override {}
};

// UD2 instruction.
class InstX8632UD2 : public InstX8632 {
  InstX8632UD2(const InstX8632UD2 &) = delete;
  InstX8632UD2 &operator=(const InstX8632UD2 &) = delete;

public:
  static InstX8632UD2 *create(Cfg *Func) {
    return new (Func->allocate<InstX8632UD2>()) InstX8632UD2(Func);
  }
  void emit(const Cfg *Func) const override;
  void emitIAS(const Cfg *Func) const override;
  void dump(const Cfg *Func) const override;
  static bool classof(const Inst *Inst) { return isClassof(Inst, UD2); }

private:
  InstX8632UD2(Cfg *Func);
  ~InstX8632UD2() override {}
};

// Test instruction.
class InstX8632Test : public InstX8632 {
  InstX8632Test(const InstX8632Test &) = delete;
  InstX8632Test &operator=(const InstX8632Test &) = delete;

public:
  static InstX8632Test *create(Cfg *Func, Operand *Source1, Operand *Source2) {
    return new (Func->allocate<InstX8632Test>())
        InstX8632Test(Func, Source1, Source2);
  }
  void emit(const Cfg *Func) const override;
  void emitIAS(const Cfg *Func) const override;
  void dump(const Cfg *Func) const override;
  static bool classof(const Inst *Inst) { return isClassof(Inst, Test); }

private:
  InstX8632Test(Cfg *Func, Operand *Source1, Operand *Source2);
  ~InstX8632Test() override {}
};

// Mfence instruction.
class InstX8632Mfence : public InstX8632 {
  InstX8632Mfence(const InstX8632Mfence &) = delete;
  InstX8632Mfence &operator=(const InstX8632Mfence &) = delete;

public:
  static InstX8632Mfence *create(Cfg *Func) {
    return new (Func->allocate<InstX8632Mfence>()) InstX8632Mfence(Func);
  }
  void emit(const Cfg *Func) const override;
  void emitIAS(const Cfg *Func) const override;
  void dump(const Cfg *Func) const override;
  static bool classof(const Inst *Inst) { return isClassof(Inst, Mfence); }

private:
  InstX8632Mfence(Cfg *Func);
  ~InstX8632Mfence() override {}
};

// This is essentially a "mov" instruction with an OperandX8632Mem
// operand instead of Variable as the destination.  It's important
// for liveness that there is no Dest operand.
class InstX8632Store : public InstX8632 {
  InstX8632Store(const InstX8632Store &) = delete;
  InstX8632Store &operator=(const InstX8632Store &) = delete;

public:
  static InstX8632Store *create(Cfg *Func, Operand *Value, OperandX8632 *Mem) {
    return new (Func->allocate<InstX8632Store>())
        InstX8632Store(Func, Value, Mem);
  }
  void emit(const Cfg *Func) const override;
  void emitIAS(const Cfg *Func) const override;
  void dump(const Cfg *Func) const override;
  static bool classof(const Inst *Inst) { return isClassof(Inst, Store); }

private:
  InstX8632Store(Cfg *Func, Operand *Value, OperandX8632 *Mem);
  ~InstX8632Store() override {}
};

// This is essentially a vector "mov" instruction with an OperandX8632Mem
// operand instead of Variable as the destination.  It's important
// for liveness that there is no Dest operand. The source must be an
// Xmm register, since Dest is mem.
class InstX8632StoreP : public InstX8632 {
  InstX8632StoreP(const InstX8632StoreP &) = delete;
  InstX8632StoreP &operator=(const InstX8632StoreP &) = delete;

public:
  static InstX8632StoreP *create(Cfg *Func, Variable *Value,
                                 OperandX8632Mem *Mem) {
    return new (Func->allocate<InstX8632StoreP>())
        InstX8632StoreP(Func, Value, Mem);
  }
  void emit(const Cfg *Func) const override;
  void emitIAS(const Cfg *Func) const override;
  void dump(const Cfg *Func) const override;
  static bool classof(const Inst *Inst) { return isClassof(Inst, StoreP); }

private:
  InstX8632StoreP(Cfg *Func, Variable *Value, OperandX8632Mem *Mem);
  ~InstX8632StoreP() override {}
};

class InstX8632StoreQ : public InstX8632 {
  InstX8632StoreQ(const InstX8632StoreQ &) = delete;
  InstX8632StoreQ &operator=(const InstX8632StoreQ &) = delete;

public:
  static InstX8632StoreQ *create(Cfg *Func, Variable *Value,
                                 OperandX8632Mem *Mem) {
    return new (Func->allocate<InstX8632StoreQ>())
        InstX8632StoreQ(Func, Value, Mem);
  }
  void emit(const Cfg *Func) const override;
  void emitIAS(const Cfg *Func) const override;
  void dump(const Cfg *Func) const override;
  static bool classof(const Inst *Inst) { return isClassof(Inst, StoreQ); }

private:
  InstX8632StoreQ(Cfg *Func, Variable *Value, OperandX8632Mem *Mem);
  ~InstX8632StoreQ() override {}
};

// Nop instructions of varying length
class InstX8632Nop : public InstX8632 {
  InstX8632Nop(const InstX8632Nop &) = delete;
  InstX8632Nop &operator=(const InstX8632Nop &) = delete;

public:
  // TODO: Replace with enum.
  typedef unsigned NopVariant;

  static InstX8632Nop *create(Cfg *Func, NopVariant Variant) {
    return new (Func->allocate<InstX8632Nop>()) InstX8632Nop(Func, Variant);
  }
  void emit(const Cfg *Func) const override;
  void emitIAS(const Cfg *Func) const override;
  void dump(const Cfg *Func) const override;
  static bool classof(const Inst *Inst) { return isClassof(Inst, Nop); }

private:
  InstX8632Nop(Cfg *Func, SizeT Length);
  ~InstX8632Nop() override {}

  NopVariant Variant;
};

// Fld - load a value onto the x87 FP stack.
class InstX8632Fld : public InstX8632 {
  InstX8632Fld(const InstX8632Fld &) = delete;
  InstX8632Fld &operator=(const InstX8632Fld &) = delete;

public:
  static InstX8632Fld *create(Cfg *Func, Operand *Src) {
    return new (Func->allocate<InstX8632Fld>()) InstX8632Fld(Func, Src);
  }
  void emit(const Cfg *Func) const override;
  void emitIAS(const Cfg *Func) const override;
  void dump(const Cfg *Func) const override;
  static bool classof(const Inst *Inst) { return isClassof(Inst, Fld); }

private:
  InstX8632Fld(Cfg *Func, Operand *Src);
  ~InstX8632Fld() override {}
};

// Fstp - store x87 st(0) into memory and pop st(0).
class InstX8632Fstp : public InstX8632 {
  InstX8632Fstp(const InstX8632Fstp &) = delete;
  InstX8632Fstp &operator=(const InstX8632Fstp &) = delete;

public:
  static InstX8632Fstp *create(Cfg *Func, Variable *Dest) {
    return new (Func->allocate<InstX8632Fstp>()) InstX8632Fstp(Func, Dest);
  }
  void emit(const Cfg *Func) const override;
  void emitIAS(const Cfg *Func) const override;
  void dump(const Cfg *Func) const override;
  static bool classof(const Inst *Inst) { return isClassof(Inst, Fstp); }

private:
  InstX8632Fstp(Cfg *Func, Variable *Dest);
  ~InstX8632Fstp() override {}
};

class InstX8632Pop : public InstX8632 {
  InstX8632Pop(const InstX8632Pop &) = delete;
  InstX8632Pop &operator=(const InstX8632Pop &) = delete;

public:
  static InstX8632Pop *create(Cfg *Func, Variable *Dest) {
    return new (Func->allocate<InstX8632Pop>()) InstX8632Pop(Func, Dest);
  }
  void emit(const Cfg *Func) const override;
  void emitIAS(const Cfg *Func) const override;
  void dump(const Cfg *Func) const override;
  static bool classof(const Inst *Inst) { return isClassof(Inst, Pop); }

private:
  InstX8632Pop(Cfg *Func, Variable *Dest);
  ~InstX8632Pop() override {}
};

class InstX8632Push : public InstX8632 {
  InstX8632Push(const InstX8632Push &) = delete;
  InstX8632Push &operator=(const InstX8632Push &) = delete;

public:
  static InstX8632Push *create(Cfg *Func, Variable *Source) {
    return new (Func->allocate<InstX8632Push>())
        InstX8632Push(Func, Source);
  }
  void emit(const Cfg *Func) const override;
  void emitIAS(const Cfg *Func) const override;
  void dump(const Cfg *Func) const override;
  static bool classof(const Inst *Inst) { return isClassof(Inst, Push); }

private:
  InstX8632Push(Cfg *Func, Variable *Source);
  ~InstX8632Push() override {}
};

// Ret instruction.  Currently only supports the "ret" version that
// does not pop arguments.  This instruction takes a Source operand
// (for non-void returning functions) for liveness analysis, though
// a FakeUse before the ret would do just as well.
class InstX8632Ret : public InstX8632 {
  InstX8632Ret(const InstX8632Ret &) = delete;
  InstX8632Ret &operator=(const InstX8632Ret &) = delete;

public:
  static InstX8632Ret *create(Cfg *Func, Variable *Source = NULL) {
    return new (Func->allocate<InstX8632Ret>()) InstX8632Ret(Func, Source);
  }
  void emit(const Cfg *Func) const override;
  void emitIAS(const Cfg *Func) const override;
  void dump(const Cfg *Func) const override;
  static bool classof(const Inst *Inst) { return isClassof(Inst, Ret); }

private:
  InstX8632Ret(Cfg *Func, Variable *Source);
  ~InstX8632Ret() override {}
};

// Exchanging Add instruction.  Exchanges the first operand (destination
// operand) with the second operand (source operand), then loads the sum
// of the two values into the destination operand. The destination may be
// a register or memory, while the source must be a register.
//
// Both the dest and source are updated. The caller should then insert a
// FakeDef to reflect the second udpate.
class InstX8632Xadd : public InstX8632Lockable {
  InstX8632Xadd(const InstX8632Xadd &) = delete;
  InstX8632Xadd &operator=(const InstX8632Xadd &) = delete;

public:
  static InstX8632Xadd *create(Cfg *Func, Operand *Dest, Variable *Source,
                               bool Locked) {
    return new (Func->allocate<InstX8632Xadd>())
        InstX8632Xadd(Func, Dest, Source, Locked);
  }
  void emit(const Cfg *Func) const override;
  void emitIAS(const Cfg *Func) const override;
  void dump(const Cfg *Func) const override;
  static bool classof(const Inst *Inst) { return isClassof(Inst, Xadd); }

private:
  InstX8632Xadd(Cfg *Func, Operand *Dest, Variable *Source, bool Locked);
  ~InstX8632Xadd() override {}
};

// Exchange instruction.  Exchanges the first operand (destination
// operand) with the second operand (source operand). At least one of
// the operands must be a register (and the other can be reg or mem).
// Both the Dest and Source are updated. If there is a memory operand,
// then the instruction is automatically "locked" without the need for
// a lock prefix.
class InstX8632Xchg : public InstX8632 {
  InstX8632Xchg(const InstX8632Xchg &) = delete;
  InstX8632Xchg &operator=(const InstX8632Xchg &) = delete;

public:
  static InstX8632Xchg *create(Cfg *Func, Operand *Dest, Variable *Source) {
    return new (Func->allocate<InstX8632Xchg>())
        InstX8632Xchg(Func, Dest, Source);
  }
  void emit(const Cfg *Func) const override;
  void emitIAS(const Cfg *Func) const override;
  void dump(const Cfg *Func) const override;
  static bool classof(const Inst *Inst) { return isClassof(Inst, Xchg); }

private:
  InstX8632Xchg(Cfg *Func, Operand *Dest, Variable *Source);
  ~InstX8632Xchg() override {}
};

// Declare partial template specializations of emit() methods that
// already have default implementations.  Without this, there is the
// possibility of ODR violations and link errors.
template <> void InstX8632Addss::emit(const Cfg *Func) const;
template <> void InstX8632Blendvps::emit(const Cfg *Func) const;
template <> void InstX8632Cbwdq::emit(const Cfg *Func) const;
template <> void InstX8632Div::emit(const Cfg *Func) const;
template <> void InstX8632Divss::emit(const Cfg *Func) const;
template <> void InstX8632Idiv::emit(const Cfg *Func) const;
template <> void InstX8632Imul::emit(const Cfg *Func) const;
template <> void InstX8632Lea::emit(const Cfg *Func) const;
template <> void InstX8632Mulss::emit(const Cfg *Func) const;
template <> void InstX8632Padd::emit(const Cfg *Func) const;
template <> void InstX8632Pblendvb::emit(const Cfg *Func) const;
template <> void InstX8632Pcmpeq::emit(const Cfg *Func) const;
template <> void InstX8632Pcmpgt::emit(const Cfg *Func) const;
template <> void InstX8632Pextr::emit(const Cfg *Func) const;
template <> void InstX8632Pinsr::emit(const Cfg *Func) const;
template <> void InstX8632Pmull::emit(const Cfg *Func) const;
template <> void InstX8632Pmuludq::emit(const Cfg *Func) const;
template <> void InstX8632Psll::emit(const Cfg *Func) const;
template <> void InstX8632Psra::emit(const Cfg *Func) const;
template <> void InstX8632Psub::emit(const Cfg *Func) const;
template <> void InstX8632Sqrtss::emit(const Cfg *Func) const;
template <> void InstX8632Subss::emit(const Cfg *Func) const;

template <> void InstX8632Blendvps::emitIAS(const Cfg *Func) const;
template <> void InstX8632Cbwdq::emitIAS(const Cfg *Func) const;
template <> void InstX8632Div::emitIAS(const Cfg *Func) const;
template <> void InstX8632Idiv::emitIAS(const Cfg *Func) const;
template <> void InstX8632Imul::emitIAS(const Cfg *Func) const;
template <> void InstX8632Insertps::emitIAS(const Cfg *Func) const;
template <> void InstX8632Movd::emitIAS(const Cfg *Func) const;
template <> void InstX8632MovssRegs::emitIAS(const Cfg *Func) const;
template <> void InstX8632Pblendvb::emitIAS(const Cfg *Func) const;
template <> void InstX8632Pextr::emitIAS(const Cfg *Func) const;
template <> void InstX8632Pinsr::emitIAS(const Cfg *Func) const;
template <> void InstX8632Movsx::emitIAS(const Cfg *Func) const;
template <> void InstX8632Movzx::emitIAS(const Cfg *Func) const;
template <> void InstX8632Pmull::emitIAS(const Cfg *Func) const;
template <> void InstX8632Pshufd::emitIAS(const Cfg *Func) const;
template <> void InstX8632Shufps::emitIAS(const Cfg *Func) const;

} // end of namespace Ice

#endif // SUBZERO_SRC_ICEINSTX8632_H
