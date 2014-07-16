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

#include "IceDefs.h"
#include "IceInst.h"
#include "IceInstX8632.def"
#include "IceOperand.h"

namespace Ice {

class TargetX8632;

// OperandX8632 extends the Operand hierarchy.  Its subclasses are
// OperandX8632Mem and VariableSplit.
class OperandX8632 : public Operand {
public:
  enum OperandKindX8632 {
    k__Start = Operand::kTarget,
    kMem,
    kSplit
  };
  virtual void emit(const Cfg *Func) const = 0;
  void dump(const Cfg *Func) const;

protected:
  OperandX8632(OperandKindX8632 Kind, Type Ty)
      : Operand(static_cast<OperandKind>(Kind), Ty) {}
  virtual ~OperandX8632() {}

private:
  OperandX8632(const OperandX8632 &) LLVM_DELETED_FUNCTION;
  OperandX8632 &operator=(const OperandX8632 &) LLVM_DELETED_FUNCTION;
};

// OperandX8632Mem represents the m32 addressing mode, with optional
// base and index registers, a constant offset, and a fixed shift
// value for the index register.
class OperandX8632Mem : public OperandX8632 {
public:
  enum SegmentRegisters {
    DefaultSegment = -1,
#define X(val, name) val,
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
  virtual void emit(const Cfg *Func) const;
  virtual void dump(const Cfg *Func) const;

  static bool classof(const Operand *Operand) {
    return Operand->getKind() == static_cast<OperandKind>(kMem);
  }

private:
  OperandX8632Mem(Cfg *Func, Type Ty, Variable *Base, Constant *Offset,
                  Variable *Index, uint16_t Shift, SegmentRegisters SegmentReg);
  OperandX8632Mem(const OperandX8632Mem &) LLVM_DELETED_FUNCTION;
  OperandX8632Mem &operator=(const OperandX8632Mem &) LLVM_DELETED_FUNCTION;
  virtual ~OperandX8632Mem() {}
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
public:
  enum Portion {
    Low,
    High
  };
  static VariableSplit *create(Cfg *Func, Variable *Var, Portion Part) {
    return new (Func->allocate<VariableSplit>()) VariableSplit(Func, Var, Part);
  }
  virtual void emit(const Cfg *Func) const;
  virtual void dump(const Cfg *Func) const;

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
  VariableSplit(const VariableSplit &) LLVM_DELETED_FUNCTION;
  VariableSplit &operator=(const VariableSplit &) LLVM_DELETED_FUNCTION;
  virtual ~VariableSplit() { Func->deallocateArrayOf<Variable *>(Vars); }
  Cfg *Func; // Held only for the destructor.
  Variable *Var;
  Portion Part;
};

class InstX8632 : public InstTarget {
public:
  enum InstKindX8632 {
    k__Start = Inst::Target,
    Adc,
    Add,
    Addps,
    Addss,
    And,
    Br,
    Bsf,
    Bsr,
    Call,
    Cdq,
    Cmov,
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
    Label,
    Load,
    Mfence,
    Mov,
    Movp,
    Movq,
    Movsx,
    Movzx,
    Mul,
    Mulps,
    Mulss,
    Neg,
    Or,
    Pand,
    Pcmpeq,
    Pcmpgt,
    Pop,
    Push,
    Psll,
    Psra,
    Psub,
    Pxor,
    Ret,
    Sar,
    Sbb,
    Shl,
    Shld,
    Shr,
    Shrd,
    Sqrtss,
    Store,
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

  enum BrCond {
#define X(tag, dump, emit) tag,
    ICEINSTX8632BR_TABLE
#undef X
        Br_None
  };

  static const char *getWidthString(Type Ty);
  virtual void emit(const Cfg *Func) const = 0;
  virtual void dump(const Cfg *Func) const;

protected:
  InstX8632(Cfg *Func, InstKindX8632 Kind, SizeT Maxsrcs, Variable *Dest)
      : InstTarget(Func, static_cast<InstKind>(Kind), Maxsrcs, Dest) {}
  virtual ~InstX8632() {}
  static bool isClassof(const Inst *Inst, InstKindX8632 MyKind) {
    return Inst->getKind() == static_cast<InstKind>(MyKind);
  }

private:
  InstX8632(const InstX8632 &) LLVM_DELETED_FUNCTION;
  InstX8632 &operator=(const InstX8632 &) LLVM_DELETED_FUNCTION;
};

// InstX8632Label represents an intra-block label that is the
// target of an intra-block branch.  These are used for lowering i1
// calculations, Select instructions, and 64-bit compares on a 32-bit
// architecture, without basic block splitting.  Basic block splitting
// is not so desirable for several reasons, one of which is the impact
// on decisions based on whether a variable's live range spans
// multiple basic blocks.
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
public:
  static InstX8632Label *create(Cfg *Func, TargetX8632 *Target) {
    return new (Func->allocate<InstX8632Label>()) InstX8632Label(Func, Target);
  }
  IceString getName(const Cfg *Func) const;
  virtual void emit(const Cfg *Func) const;
  virtual void dump(const Cfg *Func) const;

private:
  InstX8632Label(Cfg *Func, TargetX8632 *Target);
  InstX8632Label(const InstX8632Label &) LLVM_DELETED_FUNCTION;
  InstX8632Label &operator=(const InstX8632Label &) LLVM_DELETED_FUNCTION;
  virtual ~InstX8632Label() {}
  SizeT Number; // used only for unique label string generation
};

// Conditional and unconditional branch instruction.
class InstX8632Br : public InstX8632 {
public:
  // Create a conditional branch to a node.
  static InstX8632Br *create(Cfg *Func, CfgNode *TargetTrue,
                             CfgNode *TargetFalse, BrCond Condition) {
    return new (Func->allocate<InstX8632Br>())
        InstX8632Br(Func, TargetTrue, TargetFalse, NULL, Condition);
  }
  // Create an unconditional branch to a node.
  static InstX8632Br *create(Cfg *Func, CfgNode *Target) {
    return new (Func->allocate<InstX8632Br>())
        InstX8632Br(Func, NULL, Target, NULL, Br_None);
  }
  // Create a non-terminator conditional branch to a node, with a
  // fallthrough to the next instruction in the current node.  This is
  // used for switch lowering.
  static InstX8632Br *create(Cfg *Func, CfgNode *Target, BrCond Condition) {
    return new (Func->allocate<InstX8632Br>())
        InstX8632Br(Func, Target, NULL, NULL, Condition);
  }
  // Create a conditional intra-block branch (or unconditional, if
  // Condition==None) to a label in the current block.
  static InstX8632Br *create(Cfg *Func, InstX8632Label *Label,
                             BrCond Condition) {
    return new (Func->allocate<InstX8632Br>())
        InstX8632Br(Func, NULL, NULL, Label, Condition);
  }
  CfgNode *getTargetTrue() const { return TargetTrue; }
  CfgNode *getTargetFalse() const { return TargetFalse; }
  virtual void emit(const Cfg *Func) const;
  virtual void dump(const Cfg *Func) const;
  static bool classof(const Inst *Inst) { return isClassof(Inst, Br); }

private:
  InstX8632Br(Cfg *Func, CfgNode *TargetTrue, CfgNode *TargetFalse,
              InstX8632Label *Label, BrCond Condition);
  InstX8632Br(const InstX8632Br &) LLVM_DELETED_FUNCTION;
  InstX8632Br &operator=(const InstX8632Br &) LLVM_DELETED_FUNCTION;
  virtual ~InstX8632Br() {}
  BrCond Condition;
  CfgNode *TargetTrue;
  CfgNode *TargetFalse;
  InstX8632Label *Label; // Intra-block branch target
};

// Call instruction.  Arguments should have already been pushed.
class InstX8632Call : public InstX8632 {
public:
  static InstX8632Call *create(Cfg *Func, Variable *Dest, Operand *CallTarget) {
    return new (Func->allocate<InstX8632Call>())
        InstX8632Call(Func, Dest, CallTarget);
  }
  Operand *getCallTarget() const { return getSrc(0); }
  virtual void emit(const Cfg *Func) const;
  virtual void dump(const Cfg *Func) const;
  static bool classof(const Inst *Inst) { return isClassof(Inst, Call); }

private:
  InstX8632Call(Cfg *Func, Variable *Dest, Operand *CallTarget);
  InstX8632Call(const InstX8632Call &) LLVM_DELETED_FUNCTION;
  InstX8632Call &operator=(const InstX8632Call &) LLVM_DELETED_FUNCTION;
  virtual ~InstX8632Call() {}
};

template <InstX8632::InstKindX8632 K>
class InstX8632Unaryop : public InstX8632 {
public:
  static InstX8632Unaryop *create(Cfg *Func, Variable *Dest, Operand *Src) {
    return new (Func->allocate<InstX8632Unaryop>())
        InstX8632Unaryop(Func, Dest, Src);
  }
  virtual void emit(const Cfg *Func) const {
    Ostream &Str = Func->getContext()->getStrEmit();
    assert(getSrcSize() == 1);
    Str << "\t" << Opcode << "\t";
    getDest()->emit(Func);
    Str << ", ";
    getSrc(0)->emit(Func);
    Str << "\n";
  }
  virtual void dump(const Cfg *Func) const {
    Ostream &Str = Func->getContext()->getStrDump();
    dumpDest(Func);
    Str << " = " << Opcode << "." << getDest()->getType() << " ";
    dumpSources(Func);
  }
  static bool classof(const Inst *Inst) { return isClassof(Inst, K); }

private:
  InstX8632Unaryop(Cfg *Func, Variable *Dest, Operand *Src)
      : InstX8632(Func, K, 1, Dest) {
    addSource(Src);
  }
  InstX8632Unaryop(const InstX8632Unaryop &) LLVM_DELETED_FUNCTION;
  InstX8632Unaryop &operator=(const InstX8632Unaryop &) LLVM_DELETED_FUNCTION;
  virtual ~InstX8632Unaryop() {}
  static const char *Opcode;
};

// See the definition of emitTwoAddress() for a description of
// ShiftHack.
void emitTwoAddress(const char *Opcode, const Inst *Inst, const Cfg *Func,
                    bool ShiftHack = false);

template <InstX8632::InstKindX8632 K, bool ShiftHack = false>
class InstX8632Binop : public InstX8632 {
public:
  // Create an ordinary binary-op instruction like add or sub.
  static InstX8632Binop *create(Cfg *Func, Variable *Dest, Operand *Source) {
    return new (Func->allocate<InstX8632Binop>())
        InstX8632Binop(Func, Dest, Source);
  }
  virtual void emit(const Cfg *Func) const {
    emitTwoAddress(Opcode, this, Func, ShiftHack);
  }
  virtual void dump(const Cfg *Func) const {
    Ostream &Str = Func->getContext()->getStrDump();
    dumpDest(Func);
    Str << " = " << Opcode << "." << getDest()->getType() << " ";
    dumpSources(Func);
  }
  static bool classof(const Inst *Inst) { return isClassof(Inst, K); }

private:
  InstX8632Binop(Cfg *Func, Variable *Dest, Operand *Source)
      : InstX8632(Func, K, 2, Dest) {
    addSource(Dest);
    addSource(Source);
  }
  InstX8632Binop(const InstX8632Binop &) LLVM_DELETED_FUNCTION;
  InstX8632Binop &operator=(const InstX8632Binop &) LLVM_DELETED_FUNCTION;
  virtual ~InstX8632Binop() {}
  static const char *Opcode;
};

template <InstX8632::InstKindX8632 K> class InstX8632Ternop : public InstX8632 {
public:
  // Create a ternary-op instruction like div or idiv.
  static InstX8632Ternop *create(Cfg *Func, Variable *Dest, Operand *Source1,
                                 Operand *Source2) {
    return new (Func->allocate<InstX8632Ternop>())
        InstX8632Ternop(Func, Dest, Source1, Source2);
  }
  virtual void emit(const Cfg *Func) const {
    Ostream &Str = Func->getContext()->getStrEmit();
    assert(getSrcSize() == 3);
    Str << "\t" << Opcode << "\t";
    getSrc(1)->emit(Func);
    Str << "\n";
  }
  virtual void dump(const Cfg *Func) const {
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
  InstX8632Ternop(const InstX8632Ternop &) LLVM_DELETED_FUNCTION;
  InstX8632Ternop &operator=(const InstX8632Ternop &) LLVM_DELETED_FUNCTION;
  virtual ~InstX8632Ternop() {}
  static const char *Opcode;
};

typedef InstX8632Unaryop<InstX8632::Bsf> InstX8632Bsf;
typedef InstX8632Unaryop<InstX8632::Bsr> InstX8632Bsr;
typedef InstX8632Unaryop<InstX8632::Sqrtss> InstX8632Sqrtss;
typedef InstX8632Binop<InstX8632::Add> InstX8632Add;
typedef InstX8632Binop<InstX8632::Addps> InstX8632Addps;
typedef InstX8632Binop<InstX8632::Adc> InstX8632Adc;
typedef InstX8632Binop<InstX8632::Addss> InstX8632Addss;
typedef InstX8632Binop<InstX8632::Sub> InstX8632Sub;
typedef InstX8632Binop<InstX8632::Subps> InstX8632Subps;
typedef InstX8632Binop<InstX8632::Subss> InstX8632Subss;
typedef InstX8632Binop<InstX8632::Sbb> InstX8632Sbb;
typedef InstX8632Binop<InstX8632::Psub> InstX8632Psub;
typedef InstX8632Binop<InstX8632::And> InstX8632And;
typedef InstX8632Binop<InstX8632::Pand> InstX8632Pand;
typedef InstX8632Binop<InstX8632::Or> InstX8632Or;
typedef InstX8632Binop<InstX8632::Xor> InstX8632Xor;
typedef InstX8632Binop<InstX8632::Pxor> InstX8632Pxor;
typedef InstX8632Binop<InstX8632::Imul> InstX8632Imul;
typedef InstX8632Binop<InstX8632::Mulps> InstX8632Mulps;
typedef InstX8632Binop<InstX8632::Mulss> InstX8632Mulss;
typedef InstX8632Binop<InstX8632::Divps> InstX8632Divps;
typedef InstX8632Binop<InstX8632::Divss> InstX8632Divss;
typedef InstX8632Binop<InstX8632::Shl, true> InstX8632Shl;
typedef InstX8632Binop<InstX8632::Psll> InstX8632Psll;
typedef InstX8632Binop<InstX8632::Shr, true> InstX8632Shr;
typedef InstX8632Binop<InstX8632::Sar, true> InstX8632Sar;
typedef InstX8632Binop<InstX8632::Psra> InstX8632Psra;
typedef InstX8632Binop<InstX8632::Pcmpeq> InstX8632Pcmpeq;
typedef InstX8632Binop<InstX8632::Pcmpgt> InstX8632Pcmpgt;
typedef InstX8632Ternop<InstX8632::Idiv> InstX8632Idiv;
typedef InstX8632Ternop<InstX8632::Div> InstX8632Div;

// Base class for a lockable x86-32 instruction (emits a locked prefix).
class InstX8632Lockable : public InstX8632 {
public:
  virtual void emit(const Cfg *Func) const = 0;
  virtual void dump(const Cfg *Func) const;

protected:
  bool Locked;

  InstX8632Lockable(Cfg *Func, InstKindX8632 Kind, SizeT Maxsrcs,
                    Variable *Dest, bool Locked)
      : InstX8632(Func, Kind, Maxsrcs, Dest), Locked(Locked) {
    // Assume that such instructions are used for Atomics and be careful
    // with optimizations.
    HasSideEffects = Locked;
  }

private:
  InstX8632Lockable(const InstX8632Lockable &) LLVM_DELETED_FUNCTION;
  InstX8632Lockable &operator=(const InstX8632Lockable &) LLVM_DELETED_FUNCTION;
};

// Mul instruction - unsigned multiply.
class InstX8632Mul : public InstX8632 {
public:
  static InstX8632Mul *create(Cfg *Func, Variable *Dest, Variable *Source1,
                              Operand *Source2) {
    return new (Func->allocate<InstX8632Mul>())
        InstX8632Mul(Func, Dest, Source1, Source2);
  }
  virtual void emit(const Cfg *Func) const;
  virtual void dump(const Cfg *Func) const;
  static bool classof(const Inst *Inst) { return isClassof(Inst, Mul); }

private:
  InstX8632Mul(Cfg *Func, Variable *Dest, Variable *Source1, Operand *Source2);
  InstX8632Mul(const InstX8632Mul &) LLVM_DELETED_FUNCTION;
  InstX8632Mul &operator=(const InstX8632Mul &) LLVM_DELETED_FUNCTION;
  virtual ~InstX8632Mul() {}
};

// Neg instruction - Two's complement negation.
class InstX8632Neg : public InstX8632 {
public:
  static InstX8632Neg *create(Cfg *Func, Operand *SrcDest) {
    return new (Func->allocate<InstX8632Neg>()) InstX8632Neg(Func, SrcDest);
  }
  virtual void emit(const Cfg *Func) const;
  virtual void dump(const Cfg *Func) const;
  static bool classof(const Inst *Inst) { return isClassof(Inst, Neg); }

private:
  InstX8632Neg(Cfg *Func, Operand *SrcDest);
  InstX8632Neg(const InstX8632Neg &) LLVM_DELETED_FUNCTION;
  InstX8632Neg &operator=(const InstX8632Neg &) LLVM_DELETED_FUNCTION;
  virtual ~InstX8632Neg() {}
};

// Shld instruction - shift across a pair of operands.  TODO: Verify
// that the validator accepts the shld instruction.
class InstX8632Shld : public InstX8632 {
public:
  static InstX8632Shld *create(Cfg *Func, Variable *Dest, Variable *Source1,
                               Variable *Source2) {
    return new (Func->allocate<InstX8632Shld>())
        InstX8632Shld(Func, Dest, Source1, Source2);
  }
  virtual void emit(const Cfg *Func) const;
  virtual void dump(const Cfg *Func) const;
  static bool classof(const Inst *Inst) { return isClassof(Inst, Shld); }

private:
  InstX8632Shld(Cfg *Func, Variable *Dest, Variable *Source1,
                Variable *Source2);
  InstX8632Shld(const InstX8632Shld &) LLVM_DELETED_FUNCTION;
  InstX8632Shld &operator=(const InstX8632Shld &) LLVM_DELETED_FUNCTION;
  virtual ~InstX8632Shld() {}
};

// Shrd instruction - shift across a pair of operands.  TODO: Verify
// that the validator accepts the shrd instruction.
class InstX8632Shrd : public InstX8632 {
public:
  static InstX8632Shrd *create(Cfg *Func, Variable *Dest, Variable *Source1,
                               Variable *Source2) {
    return new (Func->allocate<InstX8632Shrd>())
        InstX8632Shrd(Func, Dest, Source1, Source2);
  }
  virtual void emit(const Cfg *Func) const;
  virtual void dump(const Cfg *Func) const;
  static bool classof(const Inst *Inst) { return isClassof(Inst, Shrd); }

private:
  InstX8632Shrd(Cfg *Func, Variable *Dest, Variable *Source1,
                Variable *Source2);
  InstX8632Shrd(const InstX8632Shrd &) LLVM_DELETED_FUNCTION;
  InstX8632Shrd &operator=(const InstX8632Shrd &) LLVM_DELETED_FUNCTION;
  virtual ~InstX8632Shrd() {}
};

// Cdq instruction - sign-extend eax into edx
class InstX8632Cdq : public InstX8632 {
public:
  static InstX8632Cdq *create(Cfg *Func, Variable *Dest, Operand *Source) {
    return new (Func->allocate<InstX8632Cdq>())
        InstX8632Cdq(Func, Dest, Source);
  }
  virtual void emit(const Cfg *Func) const;
  virtual void dump(const Cfg *Func) const;
  static bool classof(const Inst *Inst) { return isClassof(Inst, Cdq); }

private:
  InstX8632Cdq(Cfg *Func, Variable *Dest, Operand *Source);
  InstX8632Cdq(const InstX8632Cdq &) LLVM_DELETED_FUNCTION;
  InstX8632Cdq &operator=(const InstX8632Cdq &) LLVM_DELETED_FUNCTION;
  virtual ~InstX8632Cdq() {}
};

// Conditional move instruction.
class InstX8632Cmov : public InstX8632 {
public:
  static InstX8632Cmov *create(Cfg *Func, Variable *Dest, Operand *Source,
                               BrCond Cond) {
    return new (Func->allocate<InstX8632Cmov>())
        InstX8632Cmov(Func, Dest, Source, Cond);
  }
  virtual void emit(const Cfg *Func) const;
  virtual void dump(const Cfg *Func) const;
  static bool classof(const Inst *Inst) { return isClassof(Inst, Cmov); }

private:
  InstX8632Cmov(Cfg *Func, Variable *Dest, Operand *Source, BrCond Cond);
  InstX8632Cmov(const InstX8632Cmov &) LLVM_DELETED_FUNCTION;
  InstX8632Cmov &operator=(const InstX8632Cmov &) LLVM_DELETED_FUNCTION;
  virtual ~InstX8632Cmov() {}

  BrCond Condition;
};

// Cmpxchg instruction - cmpxchg <dest>, <desired> will compare if <dest>
// equals eax. If so, the ZF is set and <desired> is stored in <dest>.
// If not, ZF is cleared and <dest> is copied to eax (or subregister).
// <dest> can be a register or memory, while <desired> must be a register.
// It is the user's responsiblity to mark eax with a FakeDef.
class InstX8632Cmpxchg : public InstX8632Lockable {
public:
  static InstX8632Cmpxchg *create(Cfg *Func, Operand *DestOrAddr, Variable *Eax,
                                  Variable *Desired, bool Locked) {
    return new (Func->allocate<InstX8632Cmpxchg>())
        InstX8632Cmpxchg(Func, DestOrAddr, Eax, Desired, Locked);
  }
  virtual void emit(const Cfg *Func) const;
  virtual void dump(const Cfg *Func) const;
  static bool classof(const Inst *Inst) { return isClassof(Inst, Cmpxchg); }

private:
  InstX8632Cmpxchg(Cfg *Func, Operand *DestOrAddr, Variable *Eax,
                   Variable *Desired, bool Locked);
  InstX8632Cmpxchg(const InstX8632Cmpxchg &) LLVM_DELETED_FUNCTION;
  InstX8632Cmpxchg &operator=(const InstX8632Cmpxchg &) LLVM_DELETED_FUNCTION;
  virtual ~InstX8632Cmpxchg() {}
};

// Cmpxchg8b instruction - cmpxchg8b <m64> will compare if <m64>
// equals edx:eax. If so, the ZF is set and ecx:ebx is stored in <m64>.
// If not, ZF is cleared and <m64> is copied to edx:eax.
// The caller is responsible for inserting FakeDefs to mark edx
// and eax as modified.
// <m64> must be a memory operand.
class InstX8632Cmpxchg8b : public InstX8632Lockable {
public:
  static InstX8632Cmpxchg8b *create(Cfg *Func, OperandX8632 *Dest,
                                    Variable *Edx, Variable *Eax, Variable *Ecx,
                                    Variable *Ebx, bool Locked) {
    return new (Func->allocate<InstX8632Cmpxchg8b>())
        InstX8632Cmpxchg8b(Func, Dest, Edx, Eax, Ecx, Ebx, Locked);
  }
  virtual void emit(const Cfg *Func) const;
  virtual void dump(const Cfg *Func) const;
  static bool classof(const Inst *Inst) { return isClassof(Inst, Cmpxchg8b); }

private:
  InstX8632Cmpxchg8b(Cfg *Func, OperandX8632 *Dest, Variable *Edx,
                     Variable *Eax, Variable *Ecx, Variable *Ebx, bool Locked);
  InstX8632Cmpxchg8b(const InstX8632Cmpxchg8b &) LLVM_DELETED_FUNCTION;
  InstX8632Cmpxchg8b &
  operator=(const InstX8632Cmpxchg8b &) LLVM_DELETED_FUNCTION;
  virtual ~InstX8632Cmpxchg8b() {}
};

// Cvt instruction - wrapper for cvtsX2sY where X and Y are in {s,d,i}
// as appropriate.  s=float, d=double, i=int.  X and Y are determined
// from dest/src types.  Sign and zero extension on the integer
// operand needs to be done separately.
class InstX8632Cvt : public InstX8632 {
public:
  static InstX8632Cvt *create(Cfg *Func, Variable *Dest, Operand *Source) {
    return new (Func->allocate<InstX8632Cvt>())
        InstX8632Cvt(Func, Dest, Source);
  }
  virtual void emit(const Cfg *Func) const;
  virtual void dump(const Cfg *Func) const;
  static bool classof(const Inst *Inst) { return isClassof(Inst, Cvt); }

private:
  InstX8632Cvt(Cfg *Func, Variable *Dest, Operand *Source);
  InstX8632Cvt(const InstX8632Cvt &) LLVM_DELETED_FUNCTION;
  InstX8632Cvt &operator=(const InstX8632Cvt &) LLVM_DELETED_FUNCTION;
  virtual ~InstX8632Cvt() {}
};

// cmp - Integer compare instruction.
class InstX8632Icmp : public InstX8632 {
public:
  static InstX8632Icmp *create(Cfg *Func, Operand *Src1, Operand *Src2) {
    return new (Func->allocate<InstX8632Icmp>())
        InstX8632Icmp(Func, Src1, Src2);
  }
  virtual void emit(const Cfg *Func) const;
  virtual void dump(const Cfg *Func) const;
  static bool classof(const Inst *Inst) { return isClassof(Inst, Icmp); }

private:
  InstX8632Icmp(Cfg *Func, Operand *Src1, Operand *Src2);
  InstX8632Icmp(const InstX8632Icmp &) LLVM_DELETED_FUNCTION;
  InstX8632Icmp &operator=(const InstX8632Icmp &) LLVM_DELETED_FUNCTION;
  virtual ~InstX8632Icmp() {}
};

// ucomiss/ucomisd - floating-point compare instruction.
class InstX8632Ucomiss : public InstX8632 {
public:
  static InstX8632Ucomiss *create(Cfg *Func, Operand *Src1, Operand *Src2) {
    return new (Func->allocate<InstX8632Ucomiss>())
        InstX8632Ucomiss(Func, Src1, Src2);
  }
  virtual void emit(const Cfg *Func) const;
  virtual void dump(const Cfg *Func) const;
  static bool classof(const Inst *Inst) { return isClassof(Inst, Ucomiss); }

private:
  InstX8632Ucomiss(Cfg *Func, Operand *Src1, Operand *Src2);
  InstX8632Ucomiss(const InstX8632Ucomiss &) LLVM_DELETED_FUNCTION;
  InstX8632Ucomiss &operator=(const InstX8632Ucomiss &) LLVM_DELETED_FUNCTION;
  virtual ~InstX8632Ucomiss() {}
};

// UD2 instruction.
class InstX8632UD2 : public InstX8632 {
public:
  static InstX8632UD2 *create(Cfg *Func) {
    return new (Func->allocate<InstX8632UD2>()) InstX8632UD2(Func);
  }
  virtual void emit(const Cfg *Func) const;
  virtual void dump(const Cfg *Func) const;
  static bool classof(const Inst *Inst) { return isClassof(Inst, UD2); }

private:
  InstX8632UD2(Cfg *Func);
  InstX8632UD2(const InstX8632UD2 &) LLVM_DELETED_FUNCTION;
  InstX8632UD2 &operator=(const InstX8632UD2 &) LLVM_DELETED_FUNCTION;
  virtual ~InstX8632UD2() {}
};

// Test instruction.
class InstX8632Test : public InstX8632 {
public:
  static InstX8632Test *create(Cfg *Func, Operand *Source1, Operand *Source2) {
    return new (Func->allocate<InstX8632Test>())
        InstX8632Test(Func, Source1, Source2);
  }
  virtual void emit(const Cfg *Func) const;
  virtual void dump(const Cfg *Func) const;
  static bool classof(const Inst *Inst) { return isClassof(Inst, Test); }

private:
  InstX8632Test(Cfg *Func, Operand *Source1, Operand *Source2);
  InstX8632Test(const InstX8632Test &) LLVM_DELETED_FUNCTION;
  InstX8632Test &operator=(const InstX8632Test &) LLVM_DELETED_FUNCTION;
  virtual ~InstX8632Test() {}
};

// Mfence instruction.
class InstX8632Mfence : public InstX8632 {
public:
  static InstX8632Mfence *create(Cfg *Func) {
    return new (Func->allocate<InstX8632Mfence>()) InstX8632Mfence(Func);
  }
  virtual void emit(const Cfg *Func) const;
  virtual void dump(const Cfg *Func) const;
  static bool classof(const Inst *Inst) { return isClassof(Inst, Mfence); }

private:
  InstX8632Mfence(Cfg *Func);
  InstX8632Mfence(const InstX8632Mfence &) LLVM_DELETED_FUNCTION;
  InstX8632Mfence &operator=(const InstX8632Mfence &) LLVM_DELETED_FUNCTION;
  virtual ~InstX8632Mfence() {}
};

// This is essentially a "mov" instruction with an OperandX8632Mem
// operand instead of Variable as the destination.  It's important
// for liveness that there is no Dest operand.
class InstX8632Store : public InstX8632 {
public:
  static InstX8632Store *create(Cfg *Func, Operand *Value, OperandX8632 *Mem) {
    return new (Func->allocate<InstX8632Store>())
        InstX8632Store(Func, Value, Mem);
  }
  virtual void emit(const Cfg *Func) const;
  virtual void dump(const Cfg *Func) const;
  static bool classof(const Inst *Inst) { return isClassof(Inst, Store); }

private:
  InstX8632Store(Cfg *Func, Operand *Value, OperandX8632 *Mem);
  InstX8632Store(const InstX8632Store &) LLVM_DELETED_FUNCTION;
  InstX8632Store &operator=(const InstX8632Store &) LLVM_DELETED_FUNCTION;
  virtual ~InstX8632Store() {}
};

// Move/assignment instruction - wrapper for mov/movss/movsd.
class InstX8632Mov : public InstX8632 {
public:
  static InstX8632Mov *create(Cfg *Func, Variable *Dest, Operand *Source) {
    return new (Func->allocate<InstX8632Mov>())
        InstX8632Mov(Func, Dest, Source);
  }
  virtual bool isRedundantAssign() const;
  virtual void emit(const Cfg *Func) const;
  virtual void dump(const Cfg *Func) const;
  static bool classof(const Inst *Inst) { return isClassof(Inst, Mov); }

private:
  InstX8632Mov(Cfg *Func, Variable *Dest, Operand *Source);
  InstX8632Mov(const InstX8632Mov &) LLVM_DELETED_FUNCTION;
  InstX8632Mov &operator=(const InstX8632Mov &) LLVM_DELETED_FUNCTION;
  virtual ~InstX8632Mov() {}
};

// Move packed - copy 128 bit values between XMM registers or mem128 and
// XMM registers
class InstX8632Movp : public InstX8632 {
public:
  static InstX8632Movp *create(Cfg *Func, Variable *Dest, Operand *Source) {
    return new (Func->allocate<InstX8632Movp>())
        InstX8632Movp(Func, Dest, Source);
  }
  virtual bool isRedundantAssign() const;
  virtual void emit(const Cfg *Func) const;
  virtual void dump(const Cfg *Func) const;
  static bool classof(const Inst *Inst) { return isClassof(Inst, Movp); }

private:
  InstX8632Movp(Cfg *Func, Variable *Dest, Operand *Source);
  InstX8632Movp(const InstX8632Movp &) LLVM_DELETED_FUNCTION;
  InstX8632Movp &operator=(const InstX8632Movp &) LLVM_DELETED_FUNCTION;
  virtual ~InstX8632Movp() {}
};

// This is essentially a "movq" instruction with an OperandX8632Mem
// operand instead of Variable as the destination.  It's important
// for liveness that there is no Dest operand.
class InstX8632StoreQ : public InstX8632 {
public:
  static InstX8632StoreQ *create(Cfg *Func, Operand *Value, OperandX8632 *Mem) {
    return new (Func->allocate<InstX8632StoreQ>())
        InstX8632StoreQ(Func, Value, Mem);
  }
  virtual void emit(const Cfg *Func) const;
  virtual void dump(const Cfg *Func) const;
  static bool classof(const Inst *Inst) { return isClassof(Inst, StoreQ); }

private:
  InstX8632StoreQ(Cfg *Func, Operand *Value, OperandX8632 *Mem);
  InstX8632StoreQ(const InstX8632StoreQ &) LLVM_DELETED_FUNCTION;
  InstX8632StoreQ &operator=(const InstX8632StoreQ &) LLVM_DELETED_FUNCTION;
  virtual ~InstX8632StoreQ() {}
};

// Movq - copy between XMM registers, or mem64 and XMM registers.
class InstX8632Movq : public InstX8632 {
public:
  static InstX8632Movq *create(Cfg *Func, Variable *Dest, Operand *Source) {
    return new (Func->allocate<InstX8632Movq>())
        InstX8632Movq(Func, Dest, Source);
  }
  virtual bool isRedundantAssign() const;
  virtual void emit(const Cfg *Func) const;
  virtual void dump(const Cfg *Func) const;
  static bool classof(const Inst *Inst) { return isClassof(Inst, Movq); }

private:
  InstX8632Movq(Cfg *Func, Variable *Dest, Operand *Source);
  InstX8632Movq(const InstX8632Movq &) LLVM_DELETED_FUNCTION;
  InstX8632Movq &operator=(const InstX8632Movq &) LLVM_DELETED_FUNCTION;
  virtual ~InstX8632Movq() {}
};

// Movsx - copy from a narrower integer type to a wider integer
// type, with sign extension.
class InstX8632Movsx : public InstX8632 {
public:
  static InstX8632Movsx *create(Cfg *Func, Variable *Dest, Operand *Source) {
    return new (Func->allocate<InstX8632Movsx>())
        InstX8632Movsx(Func, Dest, Source);
  }
  virtual void emit(const Cfg *Func) const;
  virtual void dump(const Cfg *Func) const;
  static bool classof(const Inst *Inst) { return isClassof(Inst, Movsx); }

private:
  InstX8632Movsx(Cfg *Func, Variable *Dest, Operand *Source);
  InstX8632Movsx(const InstX8632Movsx &) LLVM_DELETED_FUNCTION;
  InstX8632Movsx &operator=(const InstX8632Movsx &) LLVM_DELETED_FUNCTION;
  virtual ~InstX8632Movsx() {}
};

// Movsx - copy from a narrower integer type to a wider integer
// type, with zero extension.
class InstX8632Movzx : public InstX8632 {
public:
  static InstX8632Movzx *create(Cfg *Func, Variable *Dest, Operand *Source) {
    return new (Func->allocate<InstX8632Movzx>())
        InstX8632Movzx(Func, Dest, Source);
  }
  virtual void emit(const Cfg *Func) const;
  virtual void dump(const Cfg *Func) const;
  static bool classof(const Inst *Inst) { return isClassof(Inst, Movzx); }

private:
  InstX8632Movzx(Cfg *Func, Variable *Dest, Operand *Source);
  InstX8632Movzx(const InstX8632Movzx &) LLVM_DELETED_FUNCTION;
  InstX8632Movzx &operator=(const InstX8632Movzx &) LLVM_DELETED_FUNCTION;
  virtual ~InstX8632Movzx() {}
};

// Fld - load a value onto the x87 FP stack.
class InstX8632Fld : public InstX8632 {
public:
  static InstX8632Fld *create(Cfg *Func, Operand *Src) {
    return new (Func->allocate<InstX8632Fld>()) InstX8632Fld(Func, Src);
  }
  virtual void emit(const Cfg *Func) const;
  virtual void dump(const Cfg *Func) const;
  static bool classof(const Inst *Inst) { return isClassof(Inst, Fld); }

private:
  InstX8632Fld(Cfg *Func, Operand *Src);
  InstX8632Fld(const InstX8632Fld &) LLVM_DELETED_FUNCTION;
  InstX8632Fld &operator=(const InstX8632Fld &) LLVM_DELETED_FUNCTION;
  virtual ~InstX8632Fld() {}
};

// Fstp - store x87 st(0) into memory and pop st(0).
class InstX8632Fstp : public InstX8632 {
public:
  static InstX8632Fstp *create(Cfg *Func, Variable *Dest) {
    return new (Func->allocate<InstX8632Fstp>()) InstX8632Fstp(Func, Dest);
  }
  virtual void emit(const Cfg *Func) const;
  virtual void dump(const Cfg *Func) const;
  static bool classof(const Inst *Inst) { return isClassof(Inst, Fstp); }

private:
  InstX8632Fstp(Cfg *Func, Variable *Dest);
  InstX8632Fstp(const InstX8632Fstp &) LLVM_DELETED_FUNCTION;
  InstX8632Fstp &operator=(const InstX8632Fstp &) LLVM_DELETED_FUNCTION;
  virtual ~InstX8632Fstp() {}
};

class InstX8632Pop : public InstX8632 {
public:
  static InstX8632Pop *create(Cfg *Func, Variable *Dest) {
    return new (Func->allocate<InstX8632Pop>()) InstX8632Pop(Func, Dest);
  }
  virtual void emit(const Cfg *Func) const;
  virtual void dump(const Cfg *Func) const;
  static bool classof(const Inst *Inst) { return isClassof(Inst, Pop); }

private:
  InstX8632Pop(Cfg *Func, Variable *Dest);
  InstX8632Pop(const InstX8632Pop &) LLVM_DELETED_FUNCTION;
  InstX8632Pop &operator=(const InstX8632Pop &) LLVM_DELETED_FUNCTION;
  virtual ~InstX8632Pop() {}
};

class InstX8632Push : public InstX8632 {
public:
  static InstX8632Push *create(Cfg *Func, Operand *Source,
                               bool SuppressStackAdjustment) {
    return new (Func->allocate<InstX8632Push>())
        InstX8632Push(Func, Source, SuppressStackAdjustment);
  }
  virtual void emit(const Cfg *Func) const;
  virtual void dump(const Cfg *Func) const;
  static bool classof(const Inst *Inst) { return isClassof(Inst, Push); }

private:
  InstX8632Push(Cfg *Func, Operand *Source, bool SuppressStackAdjustment);
  InstX8632Push(const InstX8632Push &) LLVM_DELETED_FUNCTION;
  InstX8632Push &operator=(const InstX8632Push &) LLVM_DELETED_FUNCTION;
  bool SuppressStackAdjustment;
  virtual ~InstX8632Push() {}
};

// Ret instruction.  Currently only supports the "ret" version that
// does not pop arguments.  This instruction takes a Source operand
// (for non-void returning functions) for liveness analysis, though
// a FakeUse before the ret would do just as well.
class InstX8632Ret : public InstX8632 {
public:
  static InstX8632Ret *create(Cfg *Func, Variable *Source = NULL) {
    return new (Func->allocate<InstX8632Ret>()) InstX8632Ret(Func, Source);
  }
  virtual void emit(const Cfg *Func) const;
  virtual void dump(const Cfg *Func) const;
  static bool classof(const Inst *Inst) { return isClassof(Inst, Ret); }

private:
  InstX8632Ret(Cfg *Func, Variable *Source);
  InstX8632Ret(const InstX8632Ret &) LLVM_DELETED_FUNCTION;
  InstX8632Ret &operator=(const InstX8632Ret &) LLVM_DELETED_FUNCTION;
  virtual ~InstX8632Ret() {}
};

// Exchanging Add instruction.  Exchanges the first operand (destination
// operand) with the second operand (source operand), then loads the sum
// of the two values into the destination operand. The destination may be
// a register or memory, while the source must be a register.
//
// Both the dest and source are updated. The caller should then insert a
// FakeDef to reflect the second udpate.
class InstX8632Xadd : public InstX8632Lockable {
public:
  static InstX8632Xadd *create(Cfg *Func, Operand *Dest, Variable *Source,
                               bool Locked) {
    return new (Func->allocate<InstX8632Xadd>())
        InstX8632Xadd(Func, Dest, Source, Locked);
  }
  virtual void emit(const Cfg *Func) const;
  virtual void dump(const Cfg *Func) const;
  static bool classof(const Inst *Inst) { return isClassof(Inst, Xadd); }

private:
  InstX8632Xadd(Cfg *Func, Operand *Dest, Variable *Source, bool Locked);
  InstX8632Xadd(const InstX8632Xadd &) LLVM_DELETED_FUNCTION;
  InstX8632Xadd &operator=(const InstX8632Xadd &) LLVM_DELETED_FUNCTION;
  virtual ~InstX8632Xadd() {}
};

// Exchange instruction.  Exchanges the first operand (destination
// operand) with the second operand (source operand). At least one of
// the operands must be a register (and the other can be reg or mem).
// Both the Dest and Source are updated. If there is a memory operand,
// then the instruction is automatically "locked" without the need for
// a lock prefix.
class InstX8632Xchg : public InstX8632 {
public:
  static InstX8632Xchg *create(Cfg *Func, Operand *Dest, Variable *Source) {
    return new (Func->allocate<InstX8632Xchg>())
        InstX8632Xchg(Func, Dest, Source);
  }
  virtual void emit(const Cfg *Func) const;
  virtual void dump(const Cfg *Func) const;
  static bool classof(const Inst *Inst) { return isClassof(Inst, Xchg); }

private:
  InstX8632Xchg(Cfg *Func, Operand *Dest, Variable *Source);
  InstX8632Xchg(const InstX8632Xchg &) LLVM_DELETED_FUNCTION;
  InstX8632Xchg &operator=(const InstX8632Xchg &) LLVM_DELETED_FUNCTION;
  virtual ~InstX8632Xchg() {}
};

} // end of namespace Ice

#endif // SUBZERO_SRC_ICEINSTX8632_H
