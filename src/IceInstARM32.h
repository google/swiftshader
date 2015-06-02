//===- subzero/src/IceInstARM32.h - ARM32 machine instructions --*- C++ -*-===//
//
//                        The Subzero Code Generator
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares the InstARM32 and OperandARM32 classes and
// their subclasses.  This represents the machine instructions and
// operands used for ARM32 code selection.
//
//===----------------------------------------------------------------------===//

#ifndef SUBZERO_SRC_ICEINSTARM32_H
#define SUBZERO_SRC_ICEINSTARM32_H

#include "IceConditionCodesARM32.h"
#include "IceDefs.h"
#include "IceInst.h"
#include "IceInstARM32.def"
#include "IceOperand.h"

namespace Ice {

class TargetARM32;

// OperandARM32 extends the Operand hierarchy.  Its subclasses are
// OperandARM32Mem and OperandARM32Flex.
class OperandARM32 : public Operand {
  OperandARM32() = delete;
  OperandARM32(const OperandARM32 &) = delete;
  OperandARM32 &operator=(const OperandARM32 &) = delete;

public:
  enum OperandKindARM32 {
    k__Start = Operand::kTarget,
    kMem,
    kFlexStart,
    kFlexImm = kFlexStart,
    kFlexReg,
    kFlexEnd = kFlexReg
  };

  enum ShiftKind {
    kNoShift = -1,
#define X(enum, emit) enum,
    ICEINSTARM32SHIFT_TABLE
#undef X
  };

  using Operand::dump;
  void dump(const Cfg *, Ostream &Str) const override {
    if (ALLOW_DUMP)
      Str << "<OperandARM32>";
  }

protected:
  OperandARM32(OperandKindARM32 Kind, Type Ty)
      : Operand(static_cast<OperandKind>(Kind), Ty) {}
  ~OperandARM32() override {}
};

// OperandARM32Mem represents a memory operand in any of the various ARM32
// addressing modes.
class OperandARM32Mem : public OperandARM32 {
  OperandARM32Mem() = delete;
  OperandARM32Mem(const OperandARM32Mem &) = delete;
  OperandARM32Mem &operator=(const OperandARM32Mem &) = delete;

public:
  // Memory operand addressing mode.
  // The enum value also carries the encoding.
  // TODO(jvoung): unify with the assembler.
  enum AddrMode {
    // bit encoding P U W
    Offset = (8 | 4 | 0) << 21,      // offset (w/o writeback to base)
    PreIndex = (8 | 4 | 1) << 21,    // pre-indexed addressing with writeback
    PostIndex = (0 | 4 | 0) << 21,   // post-indexed addressing with writeback
    NegOffset = (8 | 0 | 0) << 21,   // negative offset (w/o writeback to base)
    NegPreIndex = (8 | 0 | 1) << 21, // negative pre-indexed with writeback
    NegPostIndex = (0 | 0 | 0) << 21 // negative post-indexed with writeback
  };

  // Provide two constructors.
  // NOTE: The Variable-typed operands have to be registers.
  //
  // (1) Reg + Imm. The Immediate actually has a limited number of bits
  // for encoding, so check canHoldOffset first. It cannot handle
  // general Constant operands like ConstantRelocatable, since a relocatable
  // can potentially take up too many bits.
  static OperandARM32Mem *create(Cfg *Func, Type Ty, Variable *Base,
                                 ConstantInteger32 *ImmOffset,
                                 AddrMode Mode = Offset) {
    return new (Func->allocate<OperandARM32Mem>())
        OperandARM32Mem(Func, Ty, Base, ImmOffset, Mode);
  }
  // (2) Reg +/- Reg with an optional shift of some kind and amount.
  // Note that this mode is disallowed in the NaCl sandbox.
  static OperandARM32Mem *create(Cfg *Func, Type Ty, Variable *Base,
                                 Variable *Index, ShiftKind ShiftOp = kNoShift,
                                 uint16_t ShiftAmt = 0,
                                 AddrMode Mode = Offset) {
    return new (Func->allocate<OperandARM32Mem>())
        OperandARM32Mem(Func, Ty, Base, Index, ShiftOp, ShiftAmt, Mode);
  }
  Variable *getBase() const { return Base; }
  ConstantInteger32 *getOffset() const { return ImmOffset; }
  Variable *getIndex() const { return Index; }
  ShiftKind getShiftOp() const { return ShiftOp; }
  uint16_t getShiftAmt() const { return ShiftAmt; }
  AddrMode getAddrMode() const { return Mode; }

  bool isRegReg() const { return Index != nullptr; }
  bool isNegAddrMode() const {
    // Positive address modes have the "U" bit set, and negative modes don't.
    static_assert((PreIndex & (4 << 21)) != 0,
                  "Positive addr modes should have U bit set.");
    static_assert((NegPreIndex & (4 << 21)) == 0,
                  "Negative addr modes should have U bit clear.");
    return (Mode & (4 << 21)) == 0;
  }

  void emit(const Cfg *Func) const override;
  using OperandARM32::dump;
  void dump(const Cfg *Func, Ostream &Str) const override;

  static bool classof(const Operand *Operand) {
    return Operand->getKind() == static_cast<OperandKind>(kMem);
  }

  // Return true if a load/store instruction for an element of type Ty
  // can encode the Offset directly in the immediate field of the 32-bit
  // ARM instruction. For some types, if the load is Sign extending, then
  // the range is reduced.
  static bool canHoldOffset(Type Ty, bool SignExt, int32_t Offset);

private:
  OperandARM32Mem(Cfg *Func, Type Ty, Variable *Base,
                  ConstantInteger32 *ImmOffset, AddrMode Mode);
  OperandARM32Mem(Cfg *Func, Type Ty, Variable *Base, Variable *Index,
                  ShiftKind ShiftOp, uint16_t ShiftAmt, AddrMode Mode);
  ~OperandARM32Mem() override {}
  Variable *Base;
  ConstantInteger32 *ImmOffset;
  Variable *Index;
  ShiftKind ShiftOp;
  uint16_t ShiftAmt;
  AddrMode Mode;
};

// OperandARM32Flex represent the "flexible second operand" for
// data-processing instructions. It can be a rotatable 8-bit constant, or
// a register with an optional shift operand. The shift amount can even be
// a third register.
class OperandARM32Flex : public OperandARM32 {
  OperandARM32Flex() = delete;
  OperandARM32Flex(const OperandARM32Flex &) = delete;
  OperandARM32Flex &operator=(const OperandARM32Flex &) = delete;

public:
  static bool classof(const Operand *Operand) {
    return static_cast<OperandKind>(kFlexStart) <= Operand->getKind() &&
           Operand->getKind() <= static_cast<OperandKind>(kFlexEnd);
  }

protected:
  OperandARM32Flex(OperandKindARM32 Kind, Type Ty) : OperandARM32(Kind, Ty) {}
  ~OperandARM32Flex() override {}
};

// Rotated immediate variant.
class OperandARM32FlexImm : public OperandARM32Flex {
  OperandARM32FlexImm() = delete;
  OperandARM32FlexImm(const OperandARM32FlexImm &) = delete;
  OperandARM32FlexImm &operator=(const OperandARM32FlexImm &) = delete;

public:
  // Immed_8 rotated by an even number of bits (2 * RotateAmt).
  static OperandARM32FlexImm *create(Cfg *Func, Type Ty, uint32_t Imm,
                                     uint32_t RotateAmt) {
    return new (Func->allocate<OperandARM32FlexImm>())
        OperandARM32FlexImm(Func, Ty, Imm, RotateAmt);
  }

  void emit(const Cfg *Func) const override;
  using OperandARM32::dump;
  void dump(const Cfg *Func, Ostream &Str) const override;

  static bool classof(const Operand *Operand) {
    return Operand->getKind() == static_cast<OperandKind>(kFlexImm);
  }

  // Return true if the Immediate can fit in the ARM flexible operand.
  // Fills in the out-params RotateAmt and Immed_8 if Immediate fits.
  static bool canHoldImm(uint32_t Immediate, uint32_t *RotateAmt,
                         uint32_t *Immed_8);

  uint32_t getImm() const { return Imm; }
  uint32_t getRotateAmt() const { return RotateAmt; }

private:
  OperandARM32FlexImm(Cfg *Func, Type Ty, uint32_t Imm, uint32_t RotateAmt);
  ~OperandARM32FlexImm() override {}

  uint32_t Imm;
  uint32_t RotateAmt;
};

// Shifted register variant.
class OperandARM32FlexReg : public OperandARM32Flex {
  OperandARM32FlexReg() = delete;
  OperandARM32FlexReg(const OperandARM32FlexReg &) = delete;
  OperandARM32FlexReg &operator=(const OperandARM32FlexReg &) = delete;

public:
  // Register with immediate/reg shift amount and shift operation.
  static OperandARM32FlexReg *create(Cfg *Func, Type Ty, Variable *Reg,
                                     ShiftKind ShiftOp, Operand *ShiftAmt) {
    return new (Func->allocate<OperandARM32FlexReg>())
        OperandARM32FlexReg(Func, Ty, Reg, ShiftOp, ShiftAmt);
  }

  void emit(const Cfg *Func) const override;
  using OperandARM32::dump;
  void dump(const Cfg *Func, Ostream &Str) const override;

  static bool classof(const Operand *Operand) {
    return Operand->getKind() == static_cast<OperandKind>(kFlexReg);
  }

  Variable *getReg() const { return Reg; }
  ShiftKind getShiftOp() const { return ShiftOp; }
  // ShiftAmt can represent an immediate or a register.
  Operand *getShiftAmt() const { return ShiftAmt; }

private:
  OperandARM32FlexReg(Cfg *Func, Type Ty, Variable *Reg, ShiftKind ShiftOp,
                      Operand *ShiftAmt);
  ~OperandARM32FlexReg() override {}

  Variable *Reg;
  ShiftKind ShiftOp;
  Operand *ShiftAmt;
};

// Base class for ARM instructions. While most ARM instructions can be
// conditionally executed, a few of them are not predicable (halt,
// memory barriers, etc.).
class InstARM32 : public InstTarget {
  InstARM32() = delete;
  InstARM32(const InstARM32 &) = delete;
  InstARM32 &operator=(const InstARM32 &) = delete;

public:
  enum InstKindARM32 {
    k__Start = Inst::Target,
    Adc,
    Add,
    And,
    Bic,
    Br,
    Call,
    Cmp,
    Eor,
    Ldr,
    Lsl,
    Mla,
    Mov,
    Movt,
    Movw,
    Mul,
    Mvn,
    Orr,
    Pop,
    Push,
    Ret,
    Sbc,
    Str,
    Sub,
    Umull
  };

  static const char *getWidthString(Type Ty);
  static CondARM32::Cond getOppositeCondition(CondARM32::Cond Cond);

  void dump(const Cfg *Func) const override;

protected:
  InstARM32(Cfg *Func, InstKindARM32 Kind, SizeT Maxsrcs, Variable *Dest)
      : InstTarget(Func, static_cast<InstKind>(Kind), Maxsrcs, Dest) {}
  ~InstARM32() override {}
  static bool isClassof(const Inst *Inst, InstKindARM32 MyKind) {
    return Inst->getKind() == static_cast<InstKind>(MyKind);
  }
};

// A predicable ARM instruction.
class InstARM32Pred : public InstARM32 {
  InstARM32Pred() = delete;
  InstARM32Pred(const InstARM32Pred &) = delete;
  InstARM32Pred &operator=(const InstARM32Pred &) = delete;

public:
  InstARM32Pred(Cfg *Func, InstKindARM32 Kind, SizeT Maxsrcs, Variable *Dest,
                CondARM32::Cond Predicate)
      : InstARM32(Func, Kind, Maxsrcs, Dest), Predicate(Predicate) {}

  CondARM32::Cond getPredicate() const { return Predicate; }
  void setPredicate(CondARM32::Cond Pred) { Predicate = Pred; }

  static const char *predString(CondARM32::Cond Predicate);
  void dumpOpcodePred(Ostream &Str, const char *Opcode, Type Ty) const;

  // Shared emit routines for common forms of instructions.
  static void emitTwoAddr(const char *Opcode, const InstARM32Pred *Inst,
                          const Cfg *Func);
  static void emitThreeAddr(const char *Opcode, const InstARM32Pred *Inst,
                            const Cfg *Func, bool SetFlags);

protected:
  CondARM32::Cond Predicate;
};

template <typename StreamType>
inline StreamType &operator<<(StreamType &Stream, CondARM32::Cond Predicate) {
  Stream << InstARM32Pred::predString(Predicate);
  return Stream;
}

// Instructions of the form x := op(y).
template <InstARM32::InstKindARM32 K>
class InstARM32UnaryopGPR : public InstARM32Pred {
  InstARM32UnaryopGPR() = delete;
  InstARM32UnaryopGPR(const InstARM32UnaryopGPR &) = delete;
  InstARM32UnaryopGPR &operator=(const InstARM32UnaryopGPR &) = delete;

public:
  static InstARM32UnaryopGPR *create(Cfg *Func, Variable *Dest, Operand *Src,
                                     CondARM32::Cond Predicate) {
    return new (Func->allocate<InstARM32UnaryopGPR>())
        InstARM32UnaryopGPR(Func, Dest, Src, Predicate);
  }
  void emit(const Cfg *Func) const override {
    if (!ALLOW_DUMP)
      return;
    Ostream &Str = Func->getContext()->getStrEmit();
    assert(getSrcSize() == 1);
    Str << "\t" << Opcode << "\t";
    getDest()->emit(Func);
    Str << ", ";
    getSrc(0)->emit(Func);
  }
  void emitIAS(const Cfg *Func) const override {
    (void)Func;
    llvm_unreachable("Not yet implemented");
  }
  void dump(const Cfg *Func) const override {
    if (!ALLOW_DUMP)
      return;
    Ostream &Str = Func->getContext()->getStrDump();
    dumpDest(Func);
    Str << " = ";
    dumpOpcodePred(Str, Opcode, getDest()->getType());
    Str << " ";
    dumpSources(Func);
  }
  static bool classof(const Inst *Inst) { return isClassof(Inst, K); }

private:
  InstARM32UnaryopGPR(Cfg *Func, Variable *Dest, Operand *Src,
                      CondARM32::Cond Predicate)
      : InstARM32Pred(Func, K, 1, Dest, Predicate) {
    addSource(Src);
  }
  ~InstARM32UnaryopGPR() override {}
  static const char *Opcode;
};

// Instructions of the form x := x op y.
template <InstARM32::InstKindARM32 K>
class InstARM32TwoAddrGPR : public InstARM32Pred {
  InstARM32TwoAddrGPR() = delete;
  InstARM32TwoAddrGPR(const InstARM32TwoAddrGPR &) = delete;
  InstARM32TwoAddrGPR &operator=(const InstARM32TwoAddrGPR &) = delete;

public:
  // Dest must be a register.
  static InstARM32TwoAddrGPR *create(Cfg *Func, Variable *Dest, Operand *Src,
                                     CondARM32::Cond Predicate) {
    return new (Func->allocate<InstARM32TwoAddrGPR>())
        InstARM32TwoAddrGPR(Func, Dest, Src, Predicate);
  }
  void emit(const Cfg *Func) const override {
    if (!ALLOW_DUMP)
      return;
    emitTwoAddr(Opcode, this, Func);
  }
  void emitIAS(const Cfg *Func) const override {
    (void)Func;
    llvm::report_fatal_error("Not yet implemented");
  }
  void dump(const Cfg *Func) const override {
    if (!ALLOW_DUMP)
      return;
    Ostream &Str = Func->getContext()->getStrDump();
    dumpDest(Func);
    Str << " = ";
    dumpOpcodePred(Str, Opcode, getDest()->getType());
    Str << " ";
    dumpSources(Func);
  }
  static bool classof(const Inst *Inst) { return isClassof(Inst, K); }

private:
  InstARM32TwoAddrGPR(Cfg *Func, Variable *Dest, Operand *Src,
                      CondARM32::Cond Predicate)
      : InstARM32Pred(Func, K, 2, Dest, Predicate) {
    addSource(Dest);
    addSource(Src);
  }
  ~InstARM32TwoAddrGPR() override {}
  static const char *Opcode;
};

// Base class for assignment instructions.
// These can be tested for redundancy (and elided if redundant).
template <InstARM32::InstKindARM32 K>
class InstARM32Movlike : public InstARM32Pred {
  InstARM32Movlike() = delete;
  InstARM32Movlike(const InstARM32Movlike &) = delete;
  InstARM32Movlike &operator=(const InstARM32Movlike &) = delete;

public:
  static InstARM32Movlike *create(Cfg *Func, Variable *Dest, Operand *Source,
                                  CondARM32::Cond Predicate) {
    return new (Func->allocate<InstARM32Movlike>())
        InstARM32Movlike(Func, Dest, Source, Predicate);
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
    dumpOpcodePred(Str, Opcode, getDest()->getType());
    Str << " ";
    dumpDest(Func);
    Str << ", ";
    dumpSources(Func);
  }
  static bool classof(const Inst *Inst) { return isClassof(Inst, K); }

private:
  InstARM32Movlike(Cfg *Func, Variable *Dest, Operand *Source,
                   CondARM32::Cond Predicate)
      : InstARM32Pred(Func, K, 1, Dest, Predicate) {
    addSource(Source);
  }
  ~InstARM32Movlike() override {}

  static const char *Opcode;
};

// Instructions of the form x := y op z. May have the side-effect of setting
// status flags.
template <InstARM32::InstKindARM32 K>
class InstARM32ThreeAddrGPR : public InstARM32Pred {
  InstARM32ThreeAddrGPR() = delete;
  InstARM32ThreeAddrGPR(const InstARM32ThreeAddrGPR &) = delete;
  InstARM32ThreeAddrGPR &operator=(const InstARM32ThreeAddrGPR &) = delete;

public:
  // Create an ordinary binary-op instruction like add, and sub.
  // Dest and Src1 must be registers.
  static InstARM32ThreeAddrGPR *create(Cfg *Func, Variable *Dest,
                                       Variable *Src1, Operand *Src2,
                                       CondARM32::Cond Predicate,
                                       bool SetFlags = false) {
    return new (Func->allocate<InstARM32ThreeAddrGPR>())
        InstARM32ThreeAddrGPR(Func, Dest, Src1, Src2, Predicate, SetFlags);
  }
  void emit(const Cfg *Func) const override {
    if (!ALLOW_DUMP)
      return;
    emitThreeAddr(Opcode, this, Func, SetFlags);
  }
  void emitIAS(const Cfg *Func) const override {
    (void)Func;
    llvm::report_fatal_error("Not yet implemented");
  }
  void dump(const Cfg *Func) const override {
    if (!ALLOW_DUMP)
      return;
    Ostream &Str = Func->getContext()->getStrDump();
    dumpDest(Func);
    Str << " = ";
    dumpOpcodePred(Str, Opcode, getDest()->getType());
    Str << (SetFlags ? ".s " : " ");
    dumpSources(Func);
  }
  static bool classof(const Inst *Inst) { return isClassof(Inst, K); }

private:
  InstARM32ThreeAddrGPR(Cfg *Func, Variable *Dest, Variable *Src1,
                        Operand *Src2, CondARM32::Cond Predicate, bool SetFlags)
      : InstARM32Pred(Func, K, 2, Dest, Predicate), SetFlags(SetFlags) {
    addSource(Src1);
    addSource(Src2);
  }
  ~InstARM32ThreeAddrGPR() override {}
  static const char *Opcode;
  bool SetFlags;
};

typedef InstARM32ThreeAddrGPR<InstARM32::Adc> InstARM32Adc;
typedef InstARM32ThreeAddrGPR<InstARM32::Add> InstARM32Add;
typedef InstARM32ThreeAddrGPR<InstARM32::And> InstARM32And;
typedef InstARM32ThreeAddrGPR<InstARM32::Bic> InstARM32Bic;
typedef InstARM32ThreeAddrGPR<InstARM32::Eor> InstARM32Eor;
typedef InstARM32ThreeAddrGPR<InstARM32::Lsl> InstARM32Lsl;
typedef InstARM32ThreeAddrGPR<InstARM32::Mul> InstARM32Mul;
typedef InstARM32ThreeAddrGPR<InstARM32::Orr> InstARM32Orr;
typedef InstARM32ThreeAddrGPR<InstARM32::Sbc> InstARM32Sbc;
typedef InstARM32ThreeAddrGPR<InstARM32::Sub> InstARM32Sub;
// Move instruction (variable <- flex). This is more of a pseudo-inst.
// If var is a register, then we use "mov". If var is stack, then we use
// "str" to store to the stack.
typedef InstARM32Movlike<InstARM32::Mov> InstARM32Mov;
// MovT leaves the bottom bits alone so dest is also a source.
// This helps indicate that a previous MovW setting dest is not dead code.
typedef InstARM32TwoAddrGPR<InstARM32::Movt> InstARM32Movt;
typedef InstARM32UnaryopGPR<InstARM32::Movw> InstARM32Movw;
typedef InstARM32UnaryopGPR<InstARM32::Mvn> InstARM32Mvn;

// Direct branch instruction.
class InstARM32Br : public InstARM32Pred {
  InstARM32Br() = delete;
  InstARM32Br(const InstARM32Br &) = delete;
  InstARM32Br &operator=(const InstARM32Br &) = delete;

public:
  // Create a conditional branch to one of two nodes.
  static InstARM32Br *create(Cfg *Func, CfgNode *TargetTrue,
                             CfgNode *TargetFalse, CondARM32::Cond Predicate) {
    assert(Predicate != CondARM32::AL);
    return new (Func->allocate<InstARM32Br>())
        InstARM32Br(Func, TargetTrue, TargetFalse, Predicate);
  }
  // Create an unconditional branch to a node.
  static InstARM32Br *create(Cfg *Func, CfgNode *Target) {
    const CfgNode *NoCondTarget = nullptr;
    return new (Func->allocate<InstARM32Br>())
        InstARM32Br(Func, NoCondTarget, Target, CondARM32::AL);
  }
  // Create a non-terminator conditional branch to a node, with a
  // fallthrough to the next instruction in the current node.  This is
  // used for switch lowering.
  static InstARM32Br *create(Cfg *Func, CfgNode *Target,
                             CondARM32::Cond Predicate) {
    assert(Predicate != CondARM32::AL);
    const CfgNode *NoUncondTarget = nullptr;
    return new (Func->allocate<InstARM32Br>())
        InstARM32Br(Func, Target, NoUncondTarget, Predicate);
  }
  const CfgNode *getTargetTrue() const { return TargetTrue; }
  const CfgNode *getTargetFalse() const { return TargetFalse; }
  bool optimizeBranch(const CfgNode *NextNode);
  uint32_t getEmitInstCount() const override {
    uint32_t Sum = 0;
    if (getTargetTrue())
      ++Sum;
    if (getTargetFalse())
      ++Sum;
    return Sum;
  }
  bool isUnconditionalBranch() const override {
    return getPredicate() == CondARM32::AL;
  }
  bool repointEdge(CfgNode *OldNode, CfgNode *NewNode) override;
  void emit(const Cfg *Func) const override;
  void emitIAS(const Cfg *Func) const override;
  void dump(const Cfg *Func) const override;
  static bool classof(const Inst *Inst) { return isClassof(Inst, Br); }

private:
  InstARM32Br(Cfg *Func, const CfgNode *TargetTrue, const CfgNode *TargetFalse,
              CondARM32::Cond Predicate);
  ~InstARM32Br() override {}
  const CfgNode *TargetTrue;
  const CfgNode *TargetFalse;
};

// Call instruction (bl/blx).  Arguments should have already been pushed.
// Technically bl and the register form of blx can be predicated, but we'll
// leave that out until needed.
class InstARM32Call : public InstARM32 {
  InstARM32Call() = delete;
  InstARM32Call(const InstARM32Call &) = delete;
  InstARM32Call &operator=(const InstARM32Call &) = delete;

public:
  static InstARM32Call *create(Cfg *Func, Variable *Dest, Operand *CallTarget) {
    return new (Func->allocate<InstARM32Call>())
        InstARM32Call(Func, Dest, CallTarget);
  }
  Operand *getCallTarget() const { return getSrc(0); }
  void emit(const Cfg *Func) const override;
  void emitIAS(const Cfg *Func) const override;
  void dump(const Cfg *Func) const override;
  static bool classof(const Inst *Inst) { return isClassof(Inst, Call); }

private:
  InstARM32Call(Cfg *Func, Variable *Dest, Operand *CallTarget);
  ~InstARM32Call() override {}
};

// Integer compare instruction.
class InstARM32Cmp : public InstARM32Pred {
  InstARM32Cmp() = delete;
  InstARM32Cmp(const InstARM32Cmp &) = delete;
  InstARM32Cmp &operator=(const InstARM32Cmp &) = delete;

public:
  static InstARM32Cmp *create(Cfg *Func, Variable *Src1, Operand *Src2,
                              CondARM32::Cond Predicate) {
    return new (Func->allocate<InstARM32Cmp>())
        InstARM32Cmp(Func, Src1, Src2, Predicate);
  }
  void emit(const Cfg *Func) const override;
  void emitIAS(const Cfg *Func) const override;
  void dump(const Cfg *Func) const override;
  static bool classof(const Inst *Inst) { return isClassof(Inst, Cmp); }

private:
  InstARM32Cmp(Cfg *Func, Variable *Src1, Operand *Src2,
               CondARM32::Cond Predicate);
  ~InstARM32Cmp() override {}
};

// Load instruction.
class InstARM32Ldr : public InstARM32Pred {
  InstARM32Ldr() = delete;
  InstARM32Ldr(const InstARM32Ldr &) = delete;
  InstARM32Ldr &operator=(const InstARM32Ldr &) = delete;

public:
  // Dest must be a register.
  static InstARM32Ldr *create(Cfg *Func, Variable *Dest, OperandARM32Mem *Mem,
                              CondARM32::Cond Predicate) {
    return new (Func->allocate<InstARM32Ldr>())
        InstARM32Ldr(Func, Dest, Mem, Predicate);
  }
  void emit(const Cfg *Func) const override;
  void emitIAS(const Cfg *Func) const override;
  void dump(const Cfg *Func) const override;
  static bool classof(const Inst *Inst) { return isClassof(Inst, Ldr); }

private:
  InstARM32Ldr(Cfg *Func, Variable *Dest, OperandARM32Mem *Mem,
               CondARM32::Cond Predicate);
  ~InstARM32Ldr() override {}
};

// Multiply Accumulate: d := x * y + a
class InstARM32Mla : public InstARM32Pred {
  InstARM32Mla() = delete;
  InstARM32Mla(const InstARM32Mla &) = delete;
  InstARM32Mla &operator=(const InstARM32Mla &) = delete;

public:
  // Everything must be a register.
  static InstARM32Mla *create(Cfg *Func, Variable *Dest, Variable *Src0,
                              Variable *Src1, Variable *Acc,
                              CondARM32::Cond Predicate) {
    return new (Func->allocate<InstARM32Mla>())
        InstARM32Mla(Func, Dest, Src0, Src1, Acc, Predicate);
  }
  void emit(const Cfg *Func) const override;
  void emitIAS(const Cfg *Func) const override;
  void dump(const Cfg *Func) const override;
  static bool classof(const Inst *Inst) { return isClassof(Inst, Mla); }

private:
  InstARM32Mla(Cfg *Func, Variable *Dest, Variable *Src0, Variable *Src1,
               Variable *Acc, CondARM32::Cond Predicate);
  ~InstARM32Mla() override {}
};

// Pop into a list of GPRs. Technically this can be predicated, but we don't
// need that functionality.
class InstARM32Pop : public InstARM32 {
  InstARM32Pop() = delete;
  InstARM32Pop(const InstARM32Pop &) = delete;
  InstARM32Pop &operator=(const InstARM32Pop &) = delete;

public:
  static InstARM32Pop *create(Cfg *Func, const VarList &Dests) {
    return new (Func->allocate<InstARM32Pop>()) InstARM32Pop(Func, Dests);
  }
  void emit(const Cfg *Func) const override;
  void emitIAS(const Cfg *Func) const override;
  void dump(const Cfg *Func) const override;
  static bool classof(const Inst *Inst) { return isClassof(Inst, Pop); }

private:
  InstARM32Pop(Cfg *Func, const VarList &Dests);
  ~InstARM32Pop() override {}
  VarList Dests;
};

// Push a list of GPRs. Technically this can be predicated, but we don't
// need that functionality.
class InstARM32Push : public InstARM32 {
  InstARM32Push() = delete;
  InstARM32Push(const InstARM32Push &) = delete;
  InstARM32Push &operator=(const InstARM32Push &) = delete;

public:
  static InstARM32Push *create(Cfg *Func, const VarList &Srcs) {
    return new (Func->allocate<InstARM32Push>()) InstARM32Push(Func, Srcs);
  }
  void emit(const Cfg *Func) const override;
  void emitIAS(const Cfg *Func) const override;
  void dump(const Cfg *Func) const override;
  static bool classof(const Inst *Inst) { return isClassof(Inst, Push); }

private:
  InstARM32Push(Cfg *Func, const VarList &Srcs);
  ~InstARM32Push() override {}
};

// Ret pseudo-instruction.  This is actually a "bx" instruction with
// an "lr" register operand, but epilogue lowering will search for a Ret
// instead of a generic "bx". This instruction also takes a Source
// operand (for non-void returning functions) for liveness analysis, though
// a FakeUse before the ret would do just as well.
//
// NOTE: Even though "bx" can be predicated, for now leave out the predication
// since it's not yet known to be useful for Ret. That may complicate finding
// the terminator instruction if it's not guaranteed to be executed.
class InstARM32Ret : public InstARM32 {
  InstARM32Ret() = delete;
  InstARM32Ret(const InstARM32Ret &) = delete;
  InstARM32Ret &operator=(const InstARM32Ret &) = delete;

public:
  static InstARM32Ret *create(Cfg *Func, Variable *LR,
                              Variable *Source = nullptr) {
    return new (Func->allocate<InstARM32Ret>()) InstARM32Ret(Func, LR, Source);
  }
  void emit(const Cfg *Func) const override;
  void emitIAS(const Cfg *Func) const override;
  void dump(const Cfg *Func) const override;
  static bool classof(const Inst *Inst) { return isClassof(Inst, Ret); }

private:
  InstARM32Ret(Cfg *Func, Variable *LR, Variable *Source);
  ~InstARM32Ret() override {}
};

// Store instruction. It's important for liveness that there is no Dest
// operand (OperandARM32Mem instead of Dest Variable).
class InstARM32Str : public InstARM32Pred {
  InstARM32Str() = delete;
  InstARM32Str(const InstARM32Str &) = delete;
  InstARM32Str &operator=(const InstARM32Str &) = delete;

public:
  // Value must be a register.
  static InstARM32Str *create(Cfg *Func, Variable *Value, OperandARM32Mem *Mem,
                              CondARM32::Cond Predicate) {
    return new (Func->allocate<InstARM32Str>())
        InstARM32Str(Func, Value, Mem, Predicate);
  }
  void emit(const Cfg *Func) const override;
  void emitIAS(const Cfg *Func) const override;
  void dump(const Cfg *Func) const override;
  static bool classof(const Inst *Inst) { return isClassof(Inst, Str); }

private:
  InstARM32Str(Cfg *Func, Variable *Value, OperandARM32Mem *Mem,
               CondARM32::Cond Predicate);
  ~InstARM32Str() override {}
};

// Unsigned Multiply Long: d.lo, d.hi := x * y
class InstARM32Umull : public InstARM32Pred {
  InstARM32Umull() = delete;
  InstARM32Umull(const InstARM32Umull &) = delete;
  InstARM32Umull &operator=(const InstARM32Umull &) = delete;

public:
  // Everything must be a register.
  static InstARM32Umull *create(Cfg *Func, Variable *DestLo, Variable *DestHi,
                                Variable *Src0, Variable *Src1,
                                CondARM32::Cond Predicate) {
    return new (Func->allocate<InstARM32Umull>())
        InstARM32Umull(Func, DestLo, DestHi, Src0, Src1, Predicate);
  }
  void emit(const Cfg *Func) const override;
  void emitIAS(const Cfg *Func) const override;
  void dump(const Cfg *Func) const override;
  static bool classof(const Inst *Inst) { return isClassof(Inst, Umull); }

private:
  InstARM32Umull(Cfg *Func, Variable *DestLo, Variable *DestHi, Variable *Src0,
                 Variable *Src1, CondARM32::Cond Predicate);
  ~InstARM32Umull() override {}
  Variable *DestHi;
};

// Declare partial template specializations of emit() methods that
// already have default implementations.  Without this, there is the
// possibility of ODR violations and link errors.

template <> void InstARM32Movw::emit(const Cfg *Func) const;
template <> void InstARM32Movt::emit(const Cfg *Func) const;

} // end of namespace Ice

#endif // SUBZERO_SRC_ICEINSTARM32_H
