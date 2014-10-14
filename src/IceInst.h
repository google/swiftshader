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
#include "IceIntrinsics.h"
#include "IceTypes.h"

// TODO: The Cfg structure, and instructions in particular, need to be
// validated for things like valid operand types, valid branch
// targets, proper ordering of Phi and non-Phi instructions, etc.
// Most of the validity checking will be done in the bitcode reader.
// We need a list of everything that should be validated, and tests
// for each.

namespace Ice {

// Base instruction class for ICE.  Inst has two subclasses:
// InstHighLevel and InstTarget.  High-level ICE instructions inherit
// from InstHighLevel, and low-level (target-specific) ICE
// instructions inherit from InstTarget.
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
    ExtractElement,
    Fcmp,
    Icmp,
    IntrinsicCall,
    InsertElement,
    Load,
    Phi,
    Ret,
    Select,
    Store,
    Switch,
    FakeDef,  // not part of LLVM/PNaCl bitcode
    FakeUse,  // not part of LLVM/PNaCl bitcode
    FakeKill, // not part of LLVM/PNaCl bitcode
    Target    // target-specific low-level ICE
              // Anything >= Target is an InstTarget subclass.
  };
  InstKind getKind() const { return Kind; }

  InstNumberT getNumber() const { return Number; }
  void renumber(Cfg *Func);
  enum { NumberDeleted = -1, NumberSentinel = 0 };

  bool isDeleted() const { return Deleted; }
  void setDeleted() { Deleted = true; }
  void deleteIfDead();

  bool hasSideEffects() const { return HasSideEffects; }

  bool isDestNonKillable() const { return IsDestNonKillable; }
  void setDestNonKillable() { IsDestNonKillable = true; }

  Variable *getDest() const { return Dest; }

  SizeT getSrcSize() const { return NumSrcs; }
  Operand *getSrc(SizeT I) const {
    assert(I < getSrcSize());
    return Srcs[I];
  }

  bool isLastUse(const Operand *Src) const;

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

  virtual bool isSimpleAssign() const { return false; }

  void livenessLightweight(Cfg *Func, LivenessBV &Live);
  void liveness(InstNumberT InstNumber, LivenessBV &Live, Liveness *Liveness,
                LiveBeginEndMap *LiveBegin, LiveBeginEndMap *LiveEnd);

  // Get the number of native instructions that this instruction
  // ultimately emits.  By default, high-level instructions don't
  // result in any native instructions, and a target-specific
  // instruction results in a single native instruction.
  virtual uint32_t getEmitInstCount() const { return 0; }
  virtual void emit(const Cfg *Func) const = 0;
  virtual void emitIAS(const Cfg *Func) const = 0;
  virtual void dump(const Cfg *Func) const;
  virtual void dumpExtras(const Cfg *Func) const;
  void dumpDecorated(const Cfg *Func) const;
  void emitSources(const Cfg *Func) const;
  void dumpSources(const Cfg *Func) const;
  void dumpDest(const Cfg *Func) const;
  virtual bool isRedundantAssign() const { return false; }

  virtual ~Inst() {}

protected:
  Inst(Cfg *Func, InstKind Kind, SizeT MaxSrcs, Variable *Dest);
  void addSource(Operand *Src) {
    assert(Src);
    assert(NumSrcs < MaxSrcs);
    Srcs[NumSrcs++] = Src;
  }
  void setLastUse(SizeT VarIndex) {
    if (VarIndex < CHAR_BIT * sizeof(LiveRangesEnded))
      LiveRangesEnded |= (((LREndedBits)1u) << VarIndex);
  }
  void resetLastUses() { LiveRangesEnded = 0; }
  // The destroy() method lets the instruction cleanly release any
  // memory that was allocated via the Cfg's allocator.
  virtual void destroy(Cfg *Func) { Func->deallocateArrayOf<Operand *>(Srcs); }

  const InstKind Kind;
  // Number is the instruction number for describing live ranges.
  InstNumberT Number;
  // Deleted means irrevocably deleted.
  bool Deleted;
  // Dead means pending deletion after liveness analysis converges.
  bool Dead;
  // HasSideEffects means the instruction is something like a function
  // call or a volatile load that can't be removed even if its Dest
  // variable is not live.
  bool HasSideEffects;
  // IsDestNonKillable means that liveness analysis shouldn't consider
  // this instruction to kill the Dest variable.  This is used when
  // lowering produces two assignments to the same variable.
  bool IsDestNonKillable;

  Variable *Dest;
  const SizeT MaxSrcs; // only used for assert
  SizeT NumSrcs;
  Operand **Srcs;

  // LiveRangesEnded marks which Variables' live ranges end in this
  // instruction.  An instruction can have an arbitrary number of
  // source operands (e.g. a call instruction), and each source
  // operand can contain 0 or 1 Variable (and target-specific operands
  // could contain more than 1 Variable).  All the variables in an
  // instruction are conceptually flattened and each variable is
  // mapped to one bit position of the LiveRangesEnded bit vector.
  // Only the first CHAR_BIT * sizeof(LREndedBits) variables are
  // tracked this way.
  typedef uint32_t LREndedBits; // only first 32 src operands tracked, sorry
  LREndedBits LiveRangesEnded;

private:
  Inst(const Inst &) = delete;
  Inst &operator=(const Inst &) = delete;
};

class InstHighLevel : public Inst {
  InstHighLevel(const InstHighLevel &) = delete;
  InstHighLevel &operator=(const InstHighLevel &) = delete;

protected:
  InstHighLevel(Cfg *Func, InstKind Kind, SizeT MaxSrcs, Variable *Dest)
      : Inst(Func, Kind, MaxSrcs, Dest) {}
  void emit(const Cfg * /*Func*/) const override {
    llvm_unreachable("emit() called on a non-lowered instruction");
  }
  void emitIAS(const Cfg * /*Func*/) const override {
    llvm_unreachable("emitIAS() called on a non-lowered instruction");
  }
  ~InstHighLevel() override {}
};

// Alloca instruction.  This captures the size in bytes as getSrc(0),
// and the required alignment in bytes.  The alignment must be either
// 0 (no alignment required) or a power of 2.
class InstAlloca : public InstHighLevel {
public:
  static InstAlloca *create(Cfg *Func, Operand *ByteCount,
                            uint32_t AlignInBytes, Variable *Dest) {
    return new (Func->allocateInst<InstAlloca>())
        InstAlloca(Func, ByteCount, AlignInBytes, Dest);
  }
  uint32_t getAlignInBytes() const { return AlignInBytes; }
  Operand *getSizeInBytes() const { return getSrc(0); }
  void dump(const Cfg *Func) const override;
  static bool classof(const Inst *Inst) { return Inst->getKind() == Alloca; }

private:
  InstAlloca(Cfg *Func, Operand *ByteCount, uint32_t AlignInBytes,
             Variable *Dest);
  InstAlloca(const InstAlloca &) = delete;
  InstAlloca &operator=(const InstAlloca &) = delete;
  ~InstAlloca() override {}
  const uint32_t AlignInBytes;
};

// Binary arithmetic instruction.  The source operands are captured in
// getSrc(0) and getSrc(1).
class InstArithmetic : public InstHighLevel {
public:
  enum OpKind {
#define X(tag, str, commutative) tag,
    ICEINSTARITHMETIC_TABLE
#undef X
        _num
  };

  static InstArithmetic *create(Cfg *Func, OpKind Op, Variable *Dest,
                                Operand *Source1, Operand *Source2) {
    return new (Func->allocateInst<InstArithmetic>())
        InstArithmetic(Func, Op, Dest, Source1, Source2);
  }
  OpKind getOp() const { return Op; }
  static const char *getOpName(OpKind Op);
  bool isCommutative() const;
  void dump(const Cfg *Func) const override;
  static bool classof(const Inst *Inst) {
    return Inst->getKind() == Arithmetic;
  }

private:
  InstArithmetic(Cfg *Func, OpKind Op, Variable *Dest, Operand *Source1,
                 Operand *Source2);
  InstArithmetic(const InstArithmetic &) = delete;
  InstArithmetic &operator=(const InstArithmetic &) = delete;
  ~InstArithmetic() override {}

  const OpKind Op;
};

// Assignment instruction.  The source operand is captured in
// getSrc(0).  This is not part of the LLVM bitcode, but is a useful
// abstraction for some of the lowering.  E.g., if Phi instruction
// lowering happens before target lowering, or for representing an
// Inttoptr instruction, or as an intermediate step for lowering a
// Load instruction.
class InstAssign : public InstHighLevel {
public:
  static InstAssign *create(Cfg *Func, Variable *Dest, Operand *Source) {
    return new (Func->allocateInst<InstAssign>())
        InstAssign(Func, Dest, Source);
  }
  bool isSimpleAssign() const override { return true; }
  void dump(const Cfg *Func) const override;
  static bool classof(const Inst *Inst) { return Inst->getKind() == Assign; }

private:
  InstAssign(Cfg *Func, Variable *Dest, Operand *Source);
  InstAssign(const InstAssign &) = delete;
  InstAssign &operator=(const InstAssign &) = delete;
  ~InstAssign() override {}
};

// Branch instruction.  This represents both conditional and
// unconditional branches.
class InstBr : public InstHighLevel {
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
  NodeList getTerminatorEdges() const override;
  void dump(const Cfg *Func) const override;
  static bool classof(const Inst *Inst) { return Inst->getKind() == Br; }

private:
  // Conditional branch
  InstBr(Cfg *Func, Operand *Source, CfgNode *TargetTrue, CfgNode *TargetFalse);
  // Unconditional branch
  InstBr(Cfg *Func, CfgNode *Target);
  InstBr(const InstBr &) = delete;
  InstBr &operator=(const InstBr &) = delete;
  ~InstBr() override {}

  CfgNode *const TargetFalse; // Doubles as unconditional branch target
  CfgNode *const TargetTrue;  // NULL if unconditional branch
};

// Call instruction.  The call target is captured as getSrc(0), and
// arg I is captured as getSrc(I+1).
class InstCall : public InstHighLevel {
public:
  static InstCall *create(Cfg *Func, SizeT NumArgs, Variable *Dest,
                          Operand *CallTarget, bool HasTailCall) {
    // Set HasSideEffects to true so that the call instruction can't be
    // dead-code eliminated. IntrinsicCalls can override this if the
    // particular intrinsic is deletable and has no side-effects.
    const bool HasSideEffects = true;
    const InstKind Kind = Inst::Call;
    return new (Func->allocateInst<InstCall>()) InstCall(
        Func, NumArgs, Dest, CallTarget, HasTailCall, HasSideEffects, Kind);
  }
  void addArg(Operand *Arg) { addSource(Arg); }
  Operand *getCallTarget() const { return getSrc(0); }
  Operand *getArg(SizeT I) const { return getSrc(I + 1); }
  SizeT getNumArgs() const { return getSrcSize() - 1; }
  bool isTailcall() const { return HasTailCall; }
  void dump(const Cfg *Func) const;
  static bool classof(const Inst *Inst) { return Inst->getKind() == Call; }
  Type getReturnType() const;

protected:
  InstCall(Cfg *Func, SizeT NumArgs, Variable *Dest, Operand *CallTarget,
           bool HasTailCall, bool HasSideEff, InstKind Kind)
      : InstHighLevel(Func, Kind, NumArgs + 1, Dest), HasTailCall(HasTailCall) {
    HasSideEffects = HasSideEff;
    addSource(CallTarget);
  }
  ~InstCall() override {}

private:
  bool HasTailCall;
  InstCall(const InstCall &) = delete;
  InstCall &operator=(const InstCall &) = delete;
};

// Cast instruction (a.k.a. conversion operation).
class InstCast : public InstHighLevel {
public:
  enum OpKind {
#define X(tag, str) tag,
    ICEINSTCAST_TABLE
#undef X
        _num
  };

  static InstCast *create(Cfg *Func, OpKind CastKind, Variable *Dest,
                          Operand *Source) {
    return new (Func->allocateInst<InstCast>())
        InstCast(Func, CastKind, Dest, Source);
  }
  OpKind getCastKind() const { return CastKind; }
  void dump(const Cfg *Func) const override;
  static bool classof(const Inst *Inst) { return Inst->getKind() == Cast; }

private:
  InstCast(Cfg *Func, OpKind CastKind, Variable *Dest, Operand *Source);
  InstCast(const InstCast &) = delete;
  InstCast &operator=(const InstCast &) = delete;
  ~InstCast() override {}
  const OpKind CastKind;
};

// ExtractElement instruction.
class InstExtractElement : public InstHighLevel {
public:
  static InstExtractElement *create(Cfg *Func, Variable *Dest, Operand *Source1,
                                    Operand *Source2) {
    return new (Func->allocateInst<InstExtractElement>())
        InstExtractElement(Func, Dest, Source1, Source2);
  }

  void dump(const Cfg *Func) const override;
  static bool classof(const Inst *Inst) {
    return Inst->getKind() == ExtractElement;
  }

private:
  InstExtractElement(Cfg *Func, Variable *Dest, Operand *Source1,
                     Operand *Source2);
  InstExtractElement(const InstExtractElement &) = delete;
  InstExtractElement &operator=(const InstExtractElement &) = delete;
  ~InstExtractElement() override {}
};

// Floating-point comparison instruction.  The source operands are
// captured in getSrc(0) and getSrc(1).
class InstFcmp : public InstHighLevel {
public:
  enum FCond {
#define X(tag, str) tag,
    ICEINSTFCMP_TABLE
#undef X
        _num
  };

  static InstFcmp *create(Cfg *Func, FCond Condition, Variable *Dest,
                          Operand *Source1, Operand *Source2) {
    return new (Func->allocateInst<InstFcmp>())
        InstFcmp(Func, Condition, Dest, Source1, Source2);
  }
  FCond getCondition() const { return Condition; }
  void dump(const Cfg *Func) const override;
  static bool classof(const Inst *Inst) { return Inst->getKind() == Fcmp; }

private:
  InstFcmp(Cfg *Func, FCond Condition, Variable *Dest, Operand *Source1,
           Operand *Source2);
  InstFcmp(const InstFcmp &) = delete;
  InstFcmp &operator=(const InstFcmp &) = delete;
  ~InstFcmp() override {}
  const FCond Condition;
};

// Integer comparison instruction.  The source operands are captured
// in getSrc(0) and getSrc(1).
class InstIcmp : public InstHighLevel {
public:
  enum ICond {
#define X(tag, str) tag,
    ICEINSTICMP_TABLE
#undef X
        _num
  };

  static InstIcmp *create(Cfg *Func, ICond Condition, Variable *Dest,
                          Operand *Source1, Operand *Source2) {
    return new (Func->allocateInst<InstIcmp>())
        InstIcmp(Func, Condition, Dest, Source1, Source2);
  }
  ICond getCondition() const { return Condition; }
  void dump(const Cfg *Func) const override;
  static bool classof(const Inst *Inst) { return Inst->getKind() == Icmp; }

private:
  InstIcmp(Cfg *Func, ICond Condition, Variable *Dest, Operand *Source1,
           Operand *Source2);
  InstIcmp(const InstIcmp &) = delete;
  InstIcmp &operator=(const InstIcmp &) = delete;
  ~InstIcmp() override {}
  const ICond Condition;
};

// InsertElement instruction.
class InstInsertElement : public InstHighLevel {
public:
  static InstInsertElement *create(Cfg *Func, Variable *Dest, Operand *Source1,
                                   Operand *Source2, Operand *Source3) {
    return new (Func->allocateInst<InstInsertElement>())
        InstInsertElement(Func, Dest, Source1, Source2, Source3);
  }

  void dump(const Cfg *Func) const override;
  static bool classof(const Inst *Inst) {
    return Inst->getKind() == InsertElement;
  }

private:
  InstInsertElement(Cfg *Func, Variable *Dest, Operand *Source1,
                    Operand *Source2, Operand *Source3);
  InstInsertElement(const InstInsertElement &) = delete;
  InstInsertElement &operator=(const InstInsertElement &) = delete;
  ~InstInsertElement() override {}
};

// Call to an intrinsic function.  The call target is captured as getSrc(0),
// and arg I is captured as getSrc(I+1).
class InstIntrinsicCall : public InstCall {
public:
  static InstIntrinsicCall *create(Cfg *Func, SizeT NumArgs, Variable *Dest,
                                   Operand *CallTarget,
                                   const Intrinsics::IntrinsicInfo &Info) {
    return new (Func->allocateInst<InstIntrinsicCall>())
        InstIntrinsicCall(Func, NumArgs, Dest, CallTarget, Info);
  }
  static bool classof(const Inst *Inst) {
    return Inst->getKind() == IntrinsicCall;
  }

  Intrinsics::IntrinsicInfo getIntrinsicInfo() const { return Info; }

private:
  InstIntrinsicCall(Cfg *Func, SizeT NumArgs, Variable *Dest,
                    Operand *CallTarget, const Intrinsics::IntrinsicInfo &Info)
      : InstCall(Func, NumArgs, Dest, CallTarget, false, Info.HasSideEffects,
                 Inst::IntrinsicCall),
        Info(Info) {}
  InstIntrinsicCall(const InstIntrinsicCall &) = delete;
  InstIntrinsicCall &operator=(const InstIntrinsicCall &) = delete;
  ~InstIntrinsicCall() override {}
  const Intrinsics::IntrinsicInfo Info;
};

// Load instruction.  The source address is captured in getSrc(0).
class InstLoad : public InstHighLevel {
public:
  static InstLoad *create(Cfg *Func, Variable *Dest, Operand *SourceAddr,
                          uint32_t Align = 1) {
    // TODO(kschimpf) Stop ignoring alignment specification.
    (void)Align;
    return new (Func->allocateInst<InstLoad>())
        InstLoad(Func, Dest, SourceAddr);
  }
  Operand *getSourceAddress() const { return getSrc(0); }
  void dump(const Cfg *Func) const override;
  static bool classof(const Inst *Inst) { return Inst->getKind() == Load; }

private:
  InstLoad(Cfg *Func, Variable *Dest, Operand *SourceAddr);
  InstLoad(const InstLoad &) = delete;
  InstLoad &operator=(const InstLoad &) = delete;
  ~InstLoad() override {}
};

// Phi instruction.  For incoming edge I, the node is Labels[I] and
// the Phi source operand is getSrc(I).
class InstPhi : public InstHighLevel {
public:
  static InstPhi *create(Cfg *Func, SizeT MaxSrcs, Variable *Dest) {
    return new (Func->allocateInst<InstPhi>()) InstPhi(Func, MaxSrcs, Dest);
  }
  void addArgument(Operand *Source, CfgNode *Label);
  Operand *getOperandForTarget(CfgNode *Target) const;
  void livenessPhiOperand(LivenessBV &Live, CfgNode *Target,
                          Liveness *Liveness);
  Inst *lower(Cfg *Func);
  void dump(const Cfg *Func) const override;
  static bool classof(const Inst *Inst) { return Inst->getKind() == Phi; }

private:
  InstPhi(Cfg *Func, SizeT MaxSrcs, Variable *Dest);
  InstPhi(const InstPhi &) = delete;
  InstPhi &operator=(const InstPhi &) = delete;
  void destroy(Cfg *Func) override {
    Func->deallocateArrayOf<CfgNode *>(Labels);
    Inst::destroy(Func);
  }
  ~InstPhi() override {}

  // Labels[] duplicates the InEdges[] information in the enclosing
  // CfgNode, but the Phi instruction is created before InEdges[]
  // is available, so it's more complicated to share the list.
  CfgNode **Labels;
};

// Ret instruction.  The return value is captured in getSrc(0), but if
// there is no return value (void-type function), then
// getSrcSize()==0 and hasRetValue()==false.
class InstRet : public InstHighLevel {
public:
  static InstRet *create(Cfg *Func, Operand *RetValue = NULL) {
    return new (Func->allocateInst<InstRet>()) InstRet(Func, RetValue);
  }
  bool hasRetValue() const { return getSrcSize(); }
  Operand *getRetValue() const {
    assert(hasRetValue());
    return getSrc(0);
  }
  NodeList getTerminatorEdges() const override { return NodeList(); }
  void dump(const Cfg *Func) const override;
  static bool classof(const Inst *Inst) { return Inst->getKind() == Ret; }

private:
  InstRet(Cfg *Func, Operand *RetValue);
  InstRet(const InstRet &) = delete;
  InstRet &operator=(const InstRet &) = delete;
  ~InstRet() override {}
};

// Select instruction.  The condition, true, and false operands are captured.
class InstSelect : public InstHighLevel {
public:
  static InstSelect *create(Cfg *Func, Variable *Dest, Operand *Condition,
                            Operand *SourceTrue, Operand *SourceFalse) {
    return new (Func->allocateInst<InstSelect>())
        InstSelect(Func, Dest, Condition, SourceTrue, SourceFalse);
  }
  Operand *getCondition() const { return getSrc(0); }
  Operand *getTrueOperand() const { return getSrc(1); }
  Operand *getFalseOperand() const { return getSrc(2); }
  void dump(const Cfg *Func) const override;
  static bool classof(const Inst *Inst) { return Inst->getKind() == Select; }

private:
  InstSelect(Cfg *Func, Variable *Dest, Operand *Condition, Operand *Source1,
             Operand *Source2);
  InstSelect(const InstSelect &) = delete;
  InstSelect &operator=(const InstSelect &) = delete;
  ~InstSelect() override {}
};

// Store instruction.  The address operand is captured, along with the
// data operand to be stored into the address.
class InstStore : public InstHighLevel {
public:
  static InstStore *create(Cfg *Func, Operand *Data, Operand *Addr,
                           uint32_t align = 1) {
    // TODO(kschimpf) Stop ignoring alignment specification.
    (void)align;
    return new (Func->allocateInst<InstStore>()) InstStore(Func, Data, Addr);
  }
  Operand *getAddr() const { return getSrc(1); }
  Operand *getData() const { return getSrc(0); }
  void dump(const Cfg *Func) const override;
  static bool classof(const Inst *Inst) { return Inst->getKind() == Store; }

private:
  InstStore(Cfg *Func, Operand *Data, Operand *Addr);
  InstStore(const InstStore &) = delete;
  InstStore &operator=(const InstStore &) = delete;
  ~InstStore() override {}
};

// Switch instruction.  The single source operand is captured as
// getSrc(0).
class InstSwitch : public InstHighLevel {
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
  NodeList getTerminatorEdges() const override;
  void dump(const Cfg *Func) const override;
  static bool classof(const Inst *Inst) { return Inst->getKind() == Switch; }

private:
  InstSwitch(Cfg *Func, SizeT NumCases, Operand *Source, CfgNode *LabelDefault);
  InstSwitch(const InstSwitch &) = delete;
  InstSwitch &operator=(const InstSwitch &) = delete;
  void destroy(Cfg *Func) override {
    Func->deallocateArrayOf<uint64_t>(Values);
    Func->deallocateArrayOf<CfgNode *>(Labels);
    Inst::destroy(Func);
  }
  ~InstSwitch() override {}

  CfgNode *LabelDefault;
  SizeT NumCases;   // not including the default case
  uint64_t *Values; // size is NumCases
  CfgNode **Labels; // size is NumCases
};

// Unreachable instruction.  This is a terminator instruction with no
// operands.
class InstUnreachable : public InstHighLevel {
public:
  static InstUnreachable *create(Cfg *Func) {
    return new (Func->allocateInst<InstUnreachable>()) InstUnreachable(Func);
  }
  NodeList getTerminatorEdges() const override { return NodeList(); }
  void dump(const Cfg *Func) const override;
  static bool classof(const Inst *Inst) {
    return Inst->getKind() == Unreachable;
  }

private:
  InstUnreachable(Cfg *Func);
  InstUnreachable(const InstUnreachable &) = delete;
  InstUnreachable &operator=(const InstUnreachable &) = delete;
  ~InstUnreachable() override {}
};

// FakeDef instruction.  This creates a fake definition of a variable,
// which is how we represent the case when an instruction produces
// multiple results.  This doesn't happen with high-level ICE
// instructions, but might with lowered instructions.  For example,
// this would be a way to represent condition flags being modified by
// an instruction.
//
// It's generally useful to set the optional source operand to be the
// dest variable of the instruction that actually produces the FakeDef
// dest.  Otherwise, the original instruction could be dead-code
// eliminated if its dest operand is unused, and therefore the FakeDef
// dest wouldn't be properly initialized.
class InstFakeDef : public InstHighLevel {
public:
  static InstFakeDef *create(Cfg *Func, Variable *Dest, Variable *Src = NULL) {
    return new (Func->allocateInst<InstFakeDef>()) InstFakeDef(Func, Dest, Src);
  }
  void emit(const Cfg *Func) const override;
  void emitIAS(const Cfg *Func) const override { emit(Func); }
  void dump(const Cfg *Func) const override;
  static bool classof(const Inst *Inst) { return Inst->getKind() == FakeDef; }

private:
  InstFakeDef(Cfg *Func, Variable *Dest, Variable *Src);
  InstFakeDef(const InstFakeDef &) = delete;
  InstFakeDef &operator=(const InstFakeDef &) = delete;
  ~InstFakeDef() override {}
};

// FakeUse instruction.  This creates a fake use of a variable, to
// keep the instruction that produces that variable from being
// dead-code eliminated.  This is useful in a variety of lowering
// situations.  The FakeUse instruction has no dest, so it can itself
// never be dead-code eliminated.
class InstFakeUse : public InstHighLevel {
public:
  static InstFakeUse *create(Cfg *Func, Variable *Src) {
    return new (Func->allocateInst<InstFakeUse>()) InstFakeUse(Func, Src);
  }
  void emit(const Cfg *Func) const override;
  void emitIAS(const Cfg *Func) const override { emit(Func); }
  void dump(const Cfg *Func) const override;
  static bool classof(const Inst *Inst) { return Inst->getKind() == FakeUse; }

private:
  InstFakeUse(Cfg *Func, Variable *Src);
  InstFakeUse(const InstFakeUse &) = delete;
  InstFakeUse &operator=(const InstFakeUse &) = delete;
  ~InstFakeUse() override {}
};

// FakeKill instruction.  This "kills" a set of variables by adding a
// trivial live range at this instruction to each variable.  The
// primary use is to indicate that scratch registers are killed after
// a call, so that the register allocator won't assign a scratch
// register to a variable whose live range spans a call.
//
// The FakeKill instruction also holds a pointer to the instruction
// that kills the set of variables, so that if that linked instruction
// gets dead-code eliminated, the FakeKill instruction will as well.
class InstFakeKill : public InstHighLevel {
public:
  static InstFakeKill *create(Cfg *Func, const VarList &KilledRegs,
                              const Inst *Linked) {
    return new (Func->allocateInst<InstFakeKill>())
        InstFakeKill(Func, KilledRegs, Linked);
  }
  const Inst *getLinked() const { return Linked; }
  void emit(const Cfg *Func) const override;
  void emitIAS(const Cfg *Func) const override { emit(Func); }
  void dump(const Cfg *Func) const override;
  static bool classof(const Inst *Inst) { return Inst->getKind() == FakeKill; }

private:
  InstFakeKill(Cfg *Func, const VarList &KilledRegs, const Inst *Linked);
  InstFakeKill(const InstFakeKill &) = delete;
  InstFakeKill &operator=(const InstFakeKill &) = delete;
  ~InstFakeKill() override {}

  // This instruction is ignored if Linked->isDeleted() is true.
  const Inst *Linked;
};

// The Target instruction is the base class for all target-specific
// instructions.
class InstTarget : public Inst {
  InstTarget(const InstTarget &) = delete;
  InstTarget &operator=(const InstTarget &) = delete;

public:
  uint32_t getEmitInstCount() const override { return 1; }
  void dump(const Cfg *Func) const override;
  static bool classof(const Inst *Inst) { return Inst->getKind() >= Target; }

protected:
  InstTarget(Cfg *Func, InstKind Kind, SizeT MaxSrcs, Variable *Dest)
      : Inst(Func, Kind, MaxSrcs, Dest) {
    assert(Kind >= Target);
  }
  void emitIAS(const Cfg *Func) const override { emit(Func); }
  ~InstTarget() override {}
};

} // end of namespace Ice

#endif // SUBZERO_SRC_ICEINST_H
