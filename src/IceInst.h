//===- subzero/src/IceInst.h - High-level instructions ----------*- C++ -*-===//
//
//                        The Subzero Code Generator
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares the Inst class and its target-independent
// subclasses, which represent the high-level Vanilla ICE instructions
// and map roughly 1:1 to LLVM instructions.
//
//===----------------------------------------------------------------------===//

#ifndef SUBZERO_SRC_ICEINST_H
#define SUBZERO_SRC_ICEINST_H

#include "IceDefs.h"
#include "IceInst.def"
#include "IceTypes.h"

// TODO: The Cfg structure, and instructions in particular, need to be
// validated for things like valid operand types, valid branch
// targets, proper ordering of Phi and non-Phi instructions, etc.
// Most of the validity checking will be done in the bitcode reader.
// We need a list of everything that should be validated, and tests
// for each.

namespace Ice {

class Inst {
public:
  enum InstKind {
    // Arbitrary (alphabetical) order, except put Unreachable first.
    Unreachable,
    Alloca,
    Arithmetic,
    Assign, // not part of LLVM/PNaCl bitcode
    Br,
    Call,
    Cast,
    Fcmp,
    Icmp,
    Load,
    Phi,
    Ret,
    Select,
    Store,
    Switch
  };
  InstKind getKind() const { return Kind; }

  int32_t getNumber() const { return Number; }

  bool isDeleted() const { return Deleted; }
  void setDeleted() { Deleted = true; }

  bool hasSideEffects() const { return HasSideEffects; }

  Variable *getDest() const { return Dest; }

  SizeT getSrcSize() const { return NumSrcs; }
  Operand *getSrc(SizeT I) const {
    assert(I < getSrcSize());
    return Srcs[I];
  }

  // Returns a list of out-edges corresponding to a terminator
  // instruction, which is the last instruction of the block.
  virtual NodeList getTerminatorEdges() const {
    // All valid terminator instructions override this method.  For
    // the default implementation, we assert in case some CfgNode
    // is constructed without a terminator instruction at the end.
    llvm_unreachable(
        "getTerminatorEdges() called on a non-terminator instruction");
    return NodeList();
  }

  // Updates the status of the Variables contained within the
  // instruction.  In particular, it marks where the Dest variable is
  // first assigned, and it tracks whether variables are live across
  // basic blocks, i.e. used in a different block from their definition.
  void updateVars(CfgNode *Node);

  virtual void dump(const Cfg *Func) const;
  void dumpDecorated(const Cfg *Func) const;
  void dumpSources(const Cfg *Func) const;
  void dumpDest(const Cfg *Func) const;

  virtual ~Inst() {}

protected:
  Inst(Cfg *Func, InstKind Kind, SizeT MaxSrcs, Variable *Dest);
  void addSource(Operand *Src) {
    assert(Src);
    assert(NumSrcs < MaxSrcs);
    Srcs[NumSrcs++] = Src;
  }
  // The destroy() method lets the instruction cleanly release any
  // memory that was allocated via the Cfg's allocator.
  virtual void destroy(Cfg *Func) { Func->deallocateArrayOf<Operand *>(Srcs); }

  const InstKind Kind;
  // Number is the instruction number for describing live ranges.
  int32_t Number;
  // Deleted means irrevocably deleted.
  bool Deleted;
  // HasSideEffects means the instruction is something like a function
  // call or a volatile load that can't be removed even if its Dest
  // variable is not live.
  bool HasSideEffects;

  Variable *Dest;
  const SizeT MaxSrcs; // only used for assert
  SizeT NumSrcs;
  Operand **Srcs;

private:
  Inst(const Inst &) LLVM_DELETED_FUNCTION;
  Inst &operator=(const Inst &) LLVM_DELETED_FUNCTION;
};

// Alloca instruction.  This captures the size in bytes as getSrc(0),
// and the required alignment in bytes.  The alignment must be either
// 0 (no alignment required) or a power of 2.
class InstAlloca : public Inst {
public:
  static InstAlloca *create(Cfg *Func, Operand *ByteCount,
                            uint32_t AlignInBytes, Variable *Dest) {
    return new (Func->allocateInst<InstAlloca>())
        InstAlloca(Func, ByteCount, AlignInBytes, Dest);
  }
  uint32_t getAlignInBytes() const { return AlignInBytes; }
  Operand *getSizeInBytes() const { return getSrc(0); }
  virtual void dump(const Cfg *Func) const;
  static bool classof(const Inst *Inst) { return Inst->getKind() == Alloca; }

private:
  InstAlloca(Cfg *Func, Operand *ByteCount, uint32_t AlignInBytes,
             Variable *Dest);
  InstAlloca(const InstAlloca &) LLVM_DELETED_FUNCTION;
  InstAlloca &operator=(const InstAlloca &) LLVM_DELETED_FUNCTION;
  virtual ~InstAlloca() {}
  const uint32_t AlignInBytes;
};

// Binary arithmetic instruction.  The source operands are captured in
// getSrc(0) and getSrc(1).
class InstArithmetic : public Inst {
public:
  enum OpKind {
#define X(tag, str, commutative) tag,
    ICEINSTARITHMETIC_TABLE
#undef X
  };
  static InstArithmetic *create(Cfg *Func, OpKind Op, Variable *Dest,
                                Operand *Source1, Operand *Source2) {
    return new (Func->allocateInst<InstArithmetic>())
        InstArithmetic(Func, Op, Dest, Source1, Source2);
  }
  OpKind getOp() const { return Op; }
  bool isCommutative() const;
  virtual void dump(const Cfg *Func) const;
  static bool classof(const Inst *Inst) {
    return Inst->getKind() == Arithmetic;
  }

private:
  InstArithmetic(Cfg *Func, OpKind Op, Variable *Dest, Operand *Source1,
                 Operand *Source2);
  InstArithmetic(const InstArithmetic &) LLVM_DELETED_FUNCTION;
  InstArithmetic &operator=(const InstArithmetic &) LLVM_DELETED_FUNCTION;
  virtual ~InstArithmetic() {}

  const OpKind Op;
};

// Assignment instruction.  The source operand is captured in
// getSrc(0).  This is not part of the LLVM bitcode, but is a useful
// abstraction for some of the lowering.  E.g., if Phi instruction
// lowering happens before target lowering, or for representing an
// Inttoptr instruction, or as an intermediate step for lowering a
// Load instruction.
class InstAssign : public Inst {
public:
  static InstAssign *create(Cfg *Func, Variable *Dest, Operand *Source) {
    return new (Func->allocateInst<InstAssign>())
        InstAssign(Func, Dest, Source);
  }
  virtual void dump(const Cfg *Func) const;
  static bool classof(const Inst *Inst) { return Inst->getKind() == Assign; }

private:
  InstAssign(Cfg *Func, Variable *Dest, Operand *Source);
  InstAssign(const InstAssign &) LLVM_DELETED_FUNCTION;
  InstAssign &operator=(const InstAssign &) LLVM_DELETED_FUNCTION;
  virtual ~InstAssign() {}
};

// Branch instruction.  This represents both conditional and
// unconditional branches.
class InstBr : public Inst {
public:
  // Create a conditional branch.  If TargetTrue==TargetFalse, it is
  // optimized to an unconditional branch.
  static InstBr *create(Cfg *Func, Operand *Source, CfgNode *TargetTrue,
                        CfgNode *TargetFalse) {
    return new (Func->allocateInst<InstBr>())
        InstBr(Func, Source, TargetTrue, TargetFalse);
  }
  // Create an unconditional branch.
  static InstBr *create(Cfg *Func, CfgNode *Target) {
    return new (Func->allocateInst<InstBr>()) InstBr(Func, Target);
  }
  bool isUnconditional() const { return getTargetTrue() == NULL; }
  Operand *getCondition() const {
    assert(!isUnconditional());
    return getSrc(0);
  }
  CfgNode *getTargetTrue() const { return TargetTrue; }
  CfgNode *getTargetFalse() const { return TargetFalse; }
  CfgNode *getTargetUnconditional() const {
    assert(isUnconditional());
    return getTargetFalse();
  }
  virtual NodeList getTerminatorEdges() const;
  virtual void dump(const Cfg *Func) const;
  static bool classof(const Inst *Inst) { return Inst->getKind() == Br; }

private:
  // Conditional branch
  InstBr(Cfg *Func, Operand *Source, CfgNode *TargetTrue, CfgNode *TargetFalse);
  // Unconditional branch
  InstBr(Cfg *Func, CfgNode *Target);
  InstBr(const InstBr &) LLVM_DELETED_FUNCTION;
  InstBr &operator=(const InstBr &) LLVM_DELETED_FUNCTION;
  virtual ~InstBr() {}

  CfgNode *const TargetFalse; // Doubles as unconditional branch target
  CfgNode *const TargetTrue;  // NULL if unconditional branch
};

// Call instruction.  The call target is captured as getSrc(0), and
// arg I is captured as getSrc(I+1).
class InstCall : public Inst {
public:
  static InstCall *create(Cfg *Func, SizeT NumArgs, Variable *Dest,
                          Operand *CallTarget) {
    return new (Func->allocateInst<InstCall>())
        InstCall(Func, NumArgs, Dest, CallTarget);
  }
  void addArg(Operand *Arg) { addSource(Arg); }
  Operand *getCallTarget() const { return getSrc(0); }
  Operand *getArg(SizeT I) const { return getSrc(I + 1); }
  SizeT getNumArgs() const { return getSrcSize() - 1; }
  virtual void dump(const Cfg *Func) const;
  static bool classof(const Inst *Inst) { return Inst->getKind() == Call; }

private:
  InstCall(Cfg *Func, SizeT NumArgs, Variable *Dest, Operand *CallTarget)
      : Inst(Func, Inst::Call, NumArgs + 1, Dest) {
    // Set HasSideEffects so that the call instruction can't be
    // dead-code eliminated.  Don't set this for a deletable intrinsic
    // call.
    HasSideEffects = true;
    addSource(CallTarget);
  }
  InstCall(const InstCall &) LLVM_DELETED_FUNCTION;
  InstCall &operator=(const InstCall &) LLVM_DELETED_FUNCTION;
  virtual ~InstCall() {}
};

// Cast instruction (a.k.a. conversion operation).
class InstCast : public Inst {
public:
  enum OpKind {
#define X(tag, str) tag,
    ICEINSTCAST_TABLE
#undef X
  };
  static InstCast *create(Cfg *Func, OpKind CastKind, Variable *Dest,
                          Operand *Source) {
    return new (Func->allocateInst<InstCast>())
        InstCast(Func, CastKind, Dest, Source);
  }
  OpKind getCastKind() const { return CastKind; }
  virtual void dump(const Cfg *Func) const;
  static bool classof(const Inst *Inst) { return Inst->getKind() == Cast; }

private:
  InstCast(Cfg *Func, OpKind CastKind, Variable *Dest, Operand *Source);
  InstCast(const InstCast &) LLVM_DELETED_FUNCTION;
  InstCast &operator=(const InstCast &) LLVM_DELETED_FUNCTION;
  virtual ~InstCast() {}
  const OpKind CastKind;
};

// Floating-point comparison instruction.  The source operands are
// captured in getSrc(0) and getSrc(1).
class InstFcmp : public Inst {
public:
  enum FCond {
#define X(tag, str) tag,
    ICEINSTFCMP_TABLE
#undef X
  };
  static InstFcmp *create(Cfg *Func, FCond Condition, Variable *Dest,
                          Operand *Source1, Operand *Source2) {
    return new (Func->allocateInst<InstFcmp>())
        InstFcmp(Func, Condition, Dest, Source1, Source2);
  }
  FCond getCondition() const { return Condition; }
  virtual void dump(const Cfg *Func) const;
  static bool classof(const Inst *Inst) { return Inst->getKind() == Fcmp; }

private:
  InstFcmp(Cfg *Func, FCond Condition, Variable *Dest, Operand *Source1,
           Operand *Source2);
  InstFcmp(const InstFcmp &) LLVM_DELETED_FUNCTION;
  InstFcmp &operator=(const InstFcmp &) LLVM_DELETED_FUNCTION;
  virtual ~InstFcmp() {}
  const FCond Condition;
};

// Integer comparison instruction.  The source operands are captured
// in getSrc(0) and getSrc(1).
class InstIcmp : public Inst {
public:
  enum ICond {
#define X(tag, str) tag,
    ICEINSTICMP_TABLE
#undef X
  };
  static InstIcmp *create(Cfg *Func, ICond Condition, Variable *Dest,
                          Operand *Source1, Operand *Source2) {
    return new (Func->allocateInst<InstIcmp>())
        InstIcmp(Func, Condition, Dest, Source1, Source2);
  }
  ICond getCondition() const { return Condition; }
  virtual void dump(const Cfg *Func) const;
  static bool classof(const Inst *Inst) { return Inst->getKind() == Icmp; }

private:
  InstIcmp(Cfg *Func, ICond Condition, Variable *Dest, Operand *Source1,
           Operand *Source2);
  InstIcmp(const InstIcmp &) LLVM_DELETED_FUNCTION;
  InstIcmp &operator=(const InstIcmp &) LLVM_DELETED_FUNCTION;
  virtual ~InstIcmp() {}
  const ICond Condition;
};

// Load instruction.  The source address is captured in getSrc(0).
class InstLoad : public Inst {
public:
  static InstLoad *create(Cfg *Func, Variable *Dest, Operand *SourceAddr) {
    return new (Func->allocateInst<InstLoad>())
        InstLoad(Func, Dest, SourceAddr);
  }
  Operand *getSourceAddress() const { return getSrc(0); }
  virtual void dump(const Cfg *Func) const;
  static bool classof(const Inst *Inst) { return Inst->getKind() == Load; }

private:
  InstLoad(Cfg *Func, Variable *Dest, Operand *SourceAddr);
  InstLoad(const InstLoad &) LLVM_DELETED_FUNCTION;
  InstLoad &operator=(const InstLoad &) LLVM_DELETED_FUNCTION;
  virtual ~InstLoad() {}
};

// Phi instruction.  For incoming edge I, the node is Labels[I] and
// the Phi source operand is getSrc(I).
class InstPhi : public Inst {
public:
  static InstPhi *create(Cfg *Func, SizeT MaxSrcs, Variable *Dest) {
    return new (Func->allocateInst<InstPhi>()) InstPhi(Func, MaxSrcs, Dest);
  }
  void addArgument(Operand *Source, CfgNode *Label);
  virtual void dump(const Cfg *Func) const;
  static bool classof(const Inst *Inst) { return Inst->getKind() == Phi; }

private:
  InstPhi(Cfg *Func, SizeT MaxSrcs, Variable *Dest);
  InstPhi(const InstPhi &) LLVM_DELETED_FUNCTION;
  InstPhi &operator=(const InstPhi &) LLVM_DELETED_FUNCTION;
  virtual void destroy(Cfg *Func) {
    Func->deallocateArrayOf<CfgNode *>(Labels);
    Inst::destroy(Func);
  }
  virtual ~InstPhi() {}

  // Labels[] duplicates the InEdges[] information in the enclosing
  // CfgNode, but the Phi instruction is created before InEdges[]
  // is available, so it's more complicated to share the list.
  CfgNode **Labels;
};

// Ret instruction.  The return value is captured in getSrc(0), but if
// there is no return value (void-type function), then
// getSrcSize()==0 and hasRetValue()==false.
class InstRet : public Inst {
public:
  static InstRet *create(Cfg *Func, Operand *RetValue = NULL) {
    return new (Func->allocateInst<InstRet>()) InstRet(Func, RetValue);
  }
  bool hasRetValue() const { return getSrcSize(); }
  Operand *getRetValue() const {
    assert(hasRetValue());
    return getSrc(0);
  }
  virtual NodeList getTerminatorEdges() const { return NodeList(); }
  virtual void dump(const Cfg *Func) const;
  static bool classof(const Inst *Inst) { return Inst->getKind() == Ret; }

private:
  InstRet(Cfg *Func, Operand *RetValue);
  InstRet(const InstRet &) LLVM_DELETED_FUNCTION;
  InstRet &operator=(const InstRet &) LLVM_DELETED_FUNCTION;
  virtual ~InstRet() {}
};

// Select instruction.  The condition, true, and false operands are captured.
class InstSelect : public Inst {
public:
  static InstSelect *create(Cfg *Func, Variable *Dest, Operand *Condition,
                            Operand *SourceTrue, Operand *SourceFalse) {
    return new (Func->allocateInst<InstSelect>())
        InstSelect(Func, Dest, Condition, SourceTrue, SourceFalse);
  }
  Operand *getCondition() const { return getSrc(0); }
  Operand *getTrueOperand() const { return getSrc(1); }
  Operand *getFalseOperand() const { return getSrc(2); }
  virtual void dump(const Cfg *Func) const;
  static bool classof(const Inst *Inst) { return Inst->getKind() == Select; }

private:
  InstSelect(Cfg *Func, Variable *Dest, Operand *Condition, Operand *Source1,
             Operand *Source2);
  InstSelect(const InstSelect &) LLVM_DELETED_FUNCTION;
  InstSelect &operator=(const InstSelect &) LLVM_DELETED_FUNCTION;
  virtual ~InstSelect() {}
};

// Store instruction.  The address operand is captured, along with the
// data operand to be stored into the address.
class InstStore : public Inst {
public:
  static InstStore *create(Cfg *Func, Operand *Data, Operand *Addr) {
    return new (Func->allocateInst<InstStore>()) InstStore(Func, Data, Addr);
  }
  Operand *getAddr() const { return getSrc(1); }
  Operand *getData() const { return getSrc(0); }
  virtual void dump(const Cfg *Func) const;
  static bool classof(const Inst *Inst) { return Inst->getKind() == Store; }

private:
  InstStore(Cfg *Func, Operand *Data, Operand *Addr);
  InstStore(const InstStore &) LLVM_DELETED_FUNCTION;
  InstStore &operator=(const InstStore &) LLVM_DELETED_FUNCTION;
  virtual ~InstStore() {}
};

// Switch instruction.  The single source operand is captured as
// getSrc(0).
class InstSwitch : public Inst {
public:
  static InstSwitch *create(Cfg *Func, SizeT NumCases, Operand *Source,
                            CfgNode *LabelDefault) {
    return new (Func->allocateInst<InstSwitch>())
        InstSwitch(Func, NumCases, Source, LabelDefault);
  }
  Operand *getComparison() const { return getSrc(0); }
  CfgNode *getLabelDefault() const { return LabelDefault; }
  SizeT getNumCases() const { return NumCases; }
  uint64_t getValue(SizeT I) const {
    assert(I < NumCases);
    return Values[I];
  }
  CfgNode *getLabel(SizeT I) const {
    assert(I < NumCases);
    return Labels[I];
  }
  void addBranch(SizeT CaseIndex, uint64_t Value, CfgNode *Label);
  virtual NodeList getTerminatorEdges() const;
  virtual void dump(const Cfg *Func) const;
  static bool classof(const Inst *Inst) { return Inst->getKind() == Switch; }

private:
  InstSwitch(Cfg *Func, SizeT NumCases, Operand *Source, CfgNode *LabelDefault);
  InstSwitch(const InstSwitch &) LLVM_DELETED_FUNCTION;
  InstSwitch &operator=(const InstSwitch &) LLVM_DELETED_FUNCTION;
  virtual void destroy(Cfg *Func) {
    Func->deallocateArrayOf<uint64_t>(Values);
    Func->deallocateArrayOf<CfgNode *>(Labels);
    Inst::destroy(Func);
  }
  virtual ~InstSwitch() {}

  CfgNode *LabelDefault;
  SizeT NumCases;   // not including the default case
  uint64_t *Values; // size is NumCases
  CfgNode **Labels; // size is NumCases
};

// Unreachable instruction.  This is a terminator instruction with no
// operands.
class InstUnreachable : public Inst {
public:
  static InstUnreachable *create(Cfg *Func) {
    return new (Func->allocateInst<InstUnreachable>()) InstUnreachable(Func);
  }
  virtual NodeList getTerminatorEdges() const { return NodeList(); }
  virtual void dump(const Cfg *Func) const;
  static bool classof(const Inst *Inst) {
    return Inst->getKind() == Unreachable;
  }

private:
  InstUnreachable(Cfg *Func);
  InstUnreachable(const InstUnreachable &) LLVM_DELETED_FUNCTION;
  InstUnreachable &operator=(const InstUnreachable &) LLVM_DELETED_FUNCTION;
  virtual ~InstUnreachable() {}
};

} // end of namespace Ice

#endif // SUBZERO_SRC_ICEINST_H
