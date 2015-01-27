//===- subzero/src/IceOperand.h - High-level operands -----------*- C++ -*-===//
//
//                        The Subzero Code Generator
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares the Operand class and its target-independent
// subclasses.  The main classes are Variable, which represents an
// LLVM variable that is either register- or stack-allocated, and the
// Constant hierarchy, which represents integer, floating-point,
// and/or symbolic constants.
//
//===----------------------------------------------------------------------===//

#ifndef SUBZERO_SRC_ICEOPERAND_H
#define SUBZERO_SRC_ICEOPERAND_H

#include "IceCfg.h"
#include "IceDefs.h"
#include "IceGlobalContext.h"
#include "IceTypes.h"

namespace Ice {

class Operand {
  Operand(const Operand &) = delete;
  Operand &operator=(const Operand &) = delete;

public:
  static const size_t MaxTargetKinds = 10;
  enum OperandKind {
    kConst_Base,
    kConstInteger32,
    kConstInteger64,
    kConstFloat,
    kConstDouble,
    kConstRelocatable,
    kConstUndef,
    kConst_Target, // leave space for target-specific constant kinds
    kConst_Num = kConst_Target + MaxTargetKinds,
    kVariable,
    kVariable_Target, // leave space for target-specific variable kinds
    kVariable_Num = kVariable_Target + MaxTargetKinds,
    // Target-specific operand classes use kTarget as the starting
    // point for their Kind enum space.
    kTarget
  };
  OperandKind getKind() const { return Kind; }
  Type getType() const { return Ty; }

  // Every Operand keeps an array of the Variables referenced in
  // the operand.  This is so that the liveness operations can get
  // quick access to the variables of interest, without having to dig
  // so far into the operand.
  SizeT getNumVars() const { return NumVars; }
  Variable *getVar(SizeT I) const {
    assert(I < getNumVars());
    return Vars[I];
  }
  virtual void emit(const Cfg *Func) const = 0;
  // The dump(Func,Str) implementation must be sure to handle the
  // situation where Func==nullptr.
  virtual void dump(const Cfg *Func, Ostream &Str) const = 0;
  void dump(const Cfg *Func) const {
    if (!ALLOW_DUMP)
      return;
    assert(Func);
    dump(Func, Func->getContext()->getStrDump());
  }
  void dump(Ostream &Str) const {
    if (ALLOW_DUMP)
      dump(nullptr, Str);
  }

  virtual ~Operand() {}

protected:
  Operand(OperandKind Kind, Type Ty)
      : Ty(Ty), Kind(Kind), NumVars(0), Vars(nullptr) {}

  const Type Ty;
  const OperandKind Kind;
  // Vars and NumVars are initialized by the derived class.
  SizeT NumVars;
  Variable **Vars;
};

template <class StreamType>
inline StreamType &operator<<(StreamType &Str, const Operand &Op) {
  Op.dump(Str);
  return Str;
}

// Constant is the abstract base class for constants.  All
// constants are allocated from a global arena and are pooled.
class Constant : public Operand {
  Constant(const Constant &) = delete;
  Constant &operator=(const Constant &) = delete;

public:
  void emitPoolLabel(Ostream &Str) const {
    Str << ".L$" << getType() << "$" << PoolEntryID;
  }
  using Operand::dump;
  void emit(const Cfg *Func) const override { emit(Func->getContext()); }
  virtual void emit(GlobalContext *Ctx) const = 0;
  void dump(const Cfg *Func, Ostream &Str) const = 0;

  static bool classof(const Operand *Operand) {
    OperandKind Kind = Operand->getKind();
    return Kind >= kConst_Base && Kind <= kConst_Num;
  }

protected:
  Constant(OperandKind Kind, Type Ty, uint32_t PoolEntryID)
      : Operand(Kind, Ty), PoolEntryID(PoolEntryID) {
    Vars = nullptr;
    NumVars = 0;
  }
  ~Constant() override {}
  // PoolEntryID is an integer that uniquely identifies the constant
  // within its constant pool.  It is used for building the constant
  // pool in the object code and for referencing its entries.
  const uint32_t PoolEntryID;
};

// ConstantPrimitive<> wraps a primitive type.
template <typename T, Operand::OperandKind K>
class ConstantPrimitive : public Constant {
  ConstantPrimitive(const ConstantPrimitive &) = delete;
  ConstantPrimitive &operator=(const ConstantPrimitive &) = delete;

public:
  typedef T PrimType;

  static ConstantPrimitive *create(GlobalContext *Ctx, Type Ty, PrimType Value,
                                   uint32_t PoolEntryID) {
    assert(!Ctx->isIRGenerationDisabled() &&
           "Attempt to build primitive constant when IR generation disabled");
    return new (Ctx->allocate<ConstantPrimitive>())
        ConstantPrimitive(Ty, Value, PoolEntryID);
  }
  PrimType getValue() const { return Value; }
  using Constant::emit;
  // The target needs to implement this for each ConstantPrimitive
  // specialization.
  void emit(GlobalContext *Ctx) const override;
  using Constant::dump;
  void dump(const Cfg *, Ostream &Str) const override {
    if (ALLOW_DUMP)
      Str << getValue();
  }

  static bool classof(const Operand *Operand) {
    return Operand->getKind() == K;
  }

private:
  ConstantPrimitive(Type Ty, PrimType Value, uint32_t PoolEntryID)
      : Constant(K, Ty, PoolEntryID), Value(Value) {}
  ~ConstantPrimitive() override {}
  const PrimType Value;
};

typedef ConstantPrimitive<int32_t, Operand::kConstInteger32> ConstantInteger32;
typedef ConstantPrimitive<int64_t, Operand::kConstInteger64> ConstantInteger64;
typedef ConstantPrimitive<float, Operand::kConstFloat> ConstantFloat;
typedef ConstantPrimitive<double, Operand::kConstDouble> ConstantDouble;

template <>
inline void ConstantInteger32::dump(const Cfg *, Ostream &Str) const {
  if (!ALLOW_DUMP)
    return;
  if (getType() == IceType_i1)
    Str << (getValue() ? "true" : "false");
  else
    Str << static_cast<int32_t>(getValue());
}

template <>
inline void ConstantInteger64::dump(const Cfg *, Ostream &Str) const {
  if (!ALLOW_DUMP)
    return;
  assert(getType() == IceType_i64);
  Str << static_cast<int64_t>(getValue());
}

// RelocatableTuple bundles the parameters that are used to
// construct an ConstantRelocatable.  It is done this way so that
// ConstantRelocatable can fit into the global constant pool
// template mechanism.
class RelocatableTuple {
  RelocatableTuple &operator=(const RelocatableTuple &) = delete;

public:
  RelocatableTuple(const RelocOffsetT Offset, const IceString &Name,
                   bool SuppressMangling)
      : Offset(Offset), Name(Name), SuppressMangling(SuppressMangling) {}
  RelocatableTuple(const RelocatableTuple &) = default;

  const RelocOffsetT Offset;
  const IceString Name;
  bool SuppressMangling;
};

bool operator==(const RelocatableTuple &A, const RelocatableTuple &B);

// ConstantRelocatable represents a symbolic constant combined with
// a fixed offset.
class ConstantRelocatable : public Constant {
  ConstantRelocatable(const ConstantRelocatable &) = delete;
  ConstantRelocatable &operator=(const ConstantRelocatable &) = delete;

public:
  static ConstantRelocatable *create(GlobalContext *Ctx, Type Ty,
                                     const RelocatableTuple &Tuple,
                                     uint32_t PoolEntryID) {
    assert(!Ctx->isIRGenerationDisabled() &&
           "Attempt to build relocatable constant when IR generation disabled");
    return new (Ctx->allocate<ConstantRelocatable>()) ConstantRelocatable(
        Ty, Tuple.Offset, Tuple.Name, Tuple.SuppressMangling, PoolEntryID);
  }

  RelocOffsetT getOffset() const { return Offset; }
  IceString getName() const { return Name; }
  void setSuppressMangling(bool Value) { SuppressMangling = Value; }
  bool getSuppressMangling() const { return SuppressMangling; }
  using Constant::emit;
  using Constant::dump;
  void emit(GlobalContext *Ctx) const override;
  void emitWithoutDollar(GlobalContext *Ctx) const;
  void dump(const Cfg *Func, Ostream &Str) const override;

  static bool classof(const Operand *Operand) {
    OperandKind Kind = Operand->getKind();
    return Kind == kConstRelocatable;
  }

private:
  ConstantRelocatable(Type Ty, RelocOffsetT Offset, const IceString &Name,
                      bool SuppressMangling, uint32_t PoolEntryID)
      : Constant(kConstRelocatable, Ty, PoolEntryID), Offset(Offset),
        Name(Name), SuppressMangling(SuppressMangling) {}
  ~ConstantRelocatable() override {}
  const RelocOffsetT Offset; // fixed offset to add
  const IceString Name;      // optional for debug/dump
  bool SuppressMangling;
};

// ConstantUndef represents an unspecified bit pattern. Although it is
// legal to lower ConstantUndef to any value, backends should try to
// make code generation deterministic by lowering ConstantUndefs to 0.
class ConstantUndef : public Constant {
  ConstantUndef(const ConstantUndef &) = delete;
  ConstantUndef &operator=(const ConstantUndef &) = delete;

public:
  static ConstantUndef *create(GlobalContext *Ctx, Type Ty,
                               uint32_t PoolEntryID) {
    assert(!Ctx->isIRGenerationDisabled() &&
           "Attempt to build undefined constant when IR generation disabled");
    return new (Ctx->allocate<ConstantUndef>()) ConstantUndef(Ty, PoolEntryID);
  }

  using Constant::emit;
  using Constant::dump;
  // The target needs to implement this.
  void emit(GlobalContext *Ctx) const override;
  void dump(const Cfg *, Ostream &Str) const override {
    if (ALLOW_DUMP)
      Str << "undef";
  }

  static bool classof(const Operand *Operand) {
    return Operand->getKind() == kConstUndef;
  }

private:
  ConstantUndef(Type Ty, uint32_t PoolEntryID)
      : Constant(kConstUndef, Ty, PoolEntryID) {}
  ~ConstantUndef() override {}
};

// RegWeight is a wrapper for a uint32_t weight value, with a
// special value that represents infinite weight, and an addWeight()
// method that ensures that W+infinity=infinity.
class RegWeight {

public:
  RegWeight() : Weight(0) {}
  RegWeight(uint32_t Weight) : Weight(Weight) {}
  RegWeight(const RegWeight &) = default;
  RegWeight &operator=(const RegWeight &) = default;
  const static uint32_t Inf = ~0; // Force regalloc to give a register
  const static uint32_t Zero = 0; // Force regalloc NOT to give a register
  void addWeight(uint32_t Delta) {
    if (Delta == Inf)
      Weight = Inf;
    else if (Weight != Inf)
      Weight += Delta;
  }
  void addWeight(const RegWeight &Other) { addWeight(Other.Weight); }
  void setWeight(uint32_t Val) { Weight = Val; }
  uint32_t getWeight() const { return Weight; }
  bool isInf() const { return Weight == Inf; }

private:
  uint32_t Weight;
};
Ostream &operator<<(Ostream &Str, const RegWeight &W);
bool operator<(const RegWeight &A, const RegWeight &B);
bool operator<=(const RegWeight &A, const RegWeight &B);
bool operator==(const RegWeight &A, const RegWeight &B);

// LiveRange is a set of instruction number intervals representing
// a variable's live range.  Generally there is one interval per basic
// block where the variable is live, but adjacent intervals get
// coalesced into a single interval.  LiveRange also includes a
// weight, in case e.g. we want a live range to have higher weight
// inside a loop.
class LiveRange {
public:
  LiveRange() : Weight(0) {}
  // Special constructor for building a kill set.  The advantage is
  // that we can reserve the right amount of space in advance.
  LiveRange(const std::vector<InstNumberT> &Kills) : Weight(0) {
    Range.reserve(Kills.size());
    for (InstNumberT I : Kills)
      addSegment(I, I);
  }
  LiveRange(const LiveRange &) = default;
  LiveRange &operator=(const LiveRange &) = default;

  void reset() {
    Range.clear();
    Weight.setWeight(0);
    untrim();
  }
  void addSegment(InstNumberT Start, InstNumberT End);

  bool endsBefore(const LiveRange &Other) const;
  bool overlaps(const LiveRange &Other, bool UseTrimmed = false) const;
  bool overlapsInst(InstNumberT OtherBegin, bool UseTrimmed = false) const;
  bool containsValue(InstNumberT Value, bool IsDest) const;
  bool isEmpty() const { return Range.empty(); }
  InstNumberT getStart() const {
    return Range.empty() ? -1 : Range.begin()->first;
  }

  void untrim() { TrimmedBegin = Range.begin(); }
  void trim(InstNumberT Lower);

  RegWeight getWeight() const { return Weight; }
  void setWeight(const RegWeight &NewWeight) { Weight = NewWeight; }
  void addWeight(uint32_t Delta) { Weight.addWeight(Delta); }
  void dump(Ostream &Str) const;

private:
  typedef std::pair<InstNumberT, InstNumberT> RangeElementType;
  // RangeType is arena-allocated from the Cfg's allocator.
  typedef std::vector<RangeElementType, CfgLocalAllocator<RangeElementType>>
      RangeType;
  RangeType Range;
  RegWeight Weight;
  // TrimmedBegin is an optimization for the overlaps() computation.
  // Since the linear-scan algorithm always calls it as overlaps(Cur)
  // and Cur advances monotonically according to live range start, we
  // can optimize overlaps() by ignoring all segments that end before
  // the start of Cur's range.  The linear-scan code enables this by
  // calling trim() on the ranges of interest as Cur advances.  Note
  // that linear-scan also has to initialize TrimmedBegin at the
  // beginning by calling untrim().
  RangeType::const_iterator TrimmedBegin;
};

Ostream &operator<<(Ostream &Str, const LiveRange &L);

// Variable represents an operand that is register-allocated or
// stack-allocated.  If it is register-allocated, it will ultimately
// have a non-negative RegNum field.
class Variable : public Operand {
  Variable(const Variable &) = delete;
  Variable &operator=(const Variable &) = delete;

public:
  static Variable *create(Cfg *Func, Type Ty, SizeT Index) {
    return new (Func->allocate<Variable>()) Variable(kVariable, Ty, Index);
  }

  SizeT getIndex() const { return Number; }
  IceString getName(const Cfg *Func) const;
  void setName(Cfg *Func, const IceString &NewName) {
    // Make sure that the name can only be set once.
    assert(NameIndex == Cfg::IdentifierIndexInvalid);
    if (!NewName.empty())
      NameIndex = Func->addIdentifierName(NewName);
  }

  bool getIsArg() const { return IsArgument; }
  void setIsArg(bool Val = true) { IsArgument = Val; }
  bool getIsImplicitArg() const { return IsImplicitArgument; }
  void setIsImplicitArg(bool Val = true) { IsImplicitArgument = Val; }

  void setIgnoreLiveness() { IgnoreLiveness = true; }
  bool getIgnoreLiveness() const { return IgnoreLiveness; }

  int32_t getStackOffset() const { return StackOffset; }
  void setStackOffset(int32_t Offset) { StackOffset = Offset; }

  static const int32_t NoRegister = -1;
  bool hasReg() const { return getRegNum() != NoRegister; }
  int32_t getRegNum() const { return RegNum; }
  void setRegNum(int32_t NewRegNum) {
    // Regnum shouldn't be set more than once.
    assert(!hasReg() || RegNum == NewRegNum);
    RegNum = NewRegNum;
  }
  bool hasRegTmp() const { return getRegNumTmp() != NoRegister; }
  int32_t getRegNumTmp() const { return RegNumTmp; }
  void setRegNumTmp(int32_t NewRegNum) { RegNumTmp = NewRegNum; }

  RegWeight getWeight() const { return Weight; }
  void setWeight(uint32_t NewWeight) { Weight = NewWeight; }
  void setWeightInfinite() { Weight = RegWeight::Inf; }

  LiveRange &getLiveRange() { return Live; }
  const LiveRange &getLiveRange() const { return Live; }
  void setLiveRange(const LiveRange &Range) { Live = Range; }
  void resetLiveRange() { Live.reset(); }
  void addLiveRange(InstNumberT Start, InstNumberT End, uint32_t WeightDelta) {
    assert(WeightDelta != RegWeight::Inf);
    Live.addSegment(Start, End);
    if (Weight.isInf())
      Live.setWeight(RegWeight::Inf);
    else
      Live.addWeight(WeightDelta * Weight.getWeight());
  }
  void setLiveRangeInfiniteWeight() { Live.setWeight(RegWeight::Inf); }
  void trimLiveRange(InstNumberT Start) { Live.trim(Start); }
  void untrimLiveRange() { Live.untrim(); }
  bool rangeEndsBefore(const Variable *Other) const {
    return Live.endsBefore(Other->Live);
  }
  bool rangeOverlaps(const Variable *Other) const {
    const bool UseTrimmed = true;
    return Live.overlaps(Other->Live, UseTrimmed);
  }
  bool rangeOverlapsStart(const Variable *Other) const {
    const bool UseTrimmed = true;
    return Live.overlapsInst(Other->Live.getStart(), UseTrimmed);
  }

  Variable *getLo() const { return LoVar; }
  Variable *getHi() const { return HiVar; }
  void setLoHi(Variable *Lo, Variable *Hi) {
    assert(LoVar == nullptr);
    assert(HiVar == nullptr);
    LoVar = Lo;
    HiVar = Hi;
  }
  // Creates a temporary copy of the variable with a different type.
  // Used primarily for syntactic correctness of textual assembly
  // emission.  Note that only basic information is copied, in
  // particular not IsArgument, IsImplicitArgument, IgnoreLiveness,
  // RegNumTmp, Weight, Live, LoVar, HiVar, VarsReal.
  Variable *asType(Type Ty);

  void emit(const Cfg *Func) const override;
  using Operand::dump;
  void dump(const Cfg *Func, Ostream &Str) const override;

  static bool classof(const Operand *Operand) {
    OperandKind Kind = Operand->getKind();
    return Kind >= kVariable && Kind <= kVariable_Num;
  }

protected:
  Variable(OperandKind K, Type Ty, SizeT Index)
      : Operand(K, Ty), Number(Index), NameIndex(Cfg::IdentifierIndexInvalid),
        IsArgument(false), IsImplicitArgument(false), IgnoreLiveness(false),
        StackOffset(0), RegNum(NoRegister), RegNumTmp(NoRegister), Weight(1),
        LoVar(nullptr), HiVar(nullptr) {
    Vars = VarsReal;
    Vars[0] = this;
    NumVars = 1;
  }
  ~Variable() override {}
  // Number is unique across all variables, and is used as a
  // (bit)vector index for liveness analysis.
  const SizeT Number;
  Cfg::IdentifierIndexType NameIndex;
  bool IsArgument;
  bool IsImplicitArgument;
  // IgnoreLiveness means that the variable should be ignored when
  // constructing and validating live ranges.  This is usually
  // reserved for the stack pointer.
  bool IgnoreLiveness;
  // StackOffset is the canonical location on stack (only if
  // RegNum==NoRegister || IsArgument).
  int32_t StackOffset;
  // RegNum is the allocated register, or NoRegister if it isn't
  // register-allocated.
  int32_t RegNum;
  // RegNumTmp is the tentative assignment during register allocation.
  int32_t RegNumTmp;
  RegWeight Weight; // Register allocation priority
  LiveRange Live;
  // LoVar and HiVar are needed for lowering from 64 to 32 bits.  When
  // lowering from I64 to I32 on a 32-bit architecture, we split the
  // variable into two machine-size pieces.  LoVar is the low-order
  // machine-size portion, and HiVar is the remaining high-order
  // portion.  TODO: It's wasteful to penalize all variables on all
  // targets this way; use a sparser representation.  It's also
  // wasteful for a 64-bit target.
  Variable *LoVar;
  Variable *HiVar;
  // VarsReal (and Operand::Vars) are set up such that Vars[0] ==
  // this.
  Variable *VarsReal[1];
};

enum MetadataKind {
  VMK_Uses,       // Track only uses, not defs
  VMK_SingleDefs, // Track uses+defs, but only record single def
  VMK_All         // Track uses+defs, including full def list
};
typedef std::vector<const Inst *, CfgLocalAllocator<const Inst *>> InstDefList;

// VariableTracking tracks the metadata for a single variable.  It is
// only meant to be used internally by VariablesMetadata.
class VariableTracking {
  VariableTracking &operator=(const VariableTracking &) = delete;

public:
  enum MultiDefState {
    // TODO(stichnot): Consider using just a simple counter.
    MDS_Unknown,
    MDS_SingleDef,
    MDS_MultiDefSingleBlock,
    MDS_MultiDefMultiBlock
  };
  enum MultiBlockState { MBS_Unknown, MBS_SingleBlock, MBS_MultiBlock };
  VariableTracking()
      : MultiDef(MDS_Unknown), MultiBlock(MBS_Unknown), SingleUseNode(nullptr),
        SingleDefNode(nullptr), FirstOrSingleDefinition(nullptr) {}
  VariableTracking(const VariableTracking &) = default;
  MultiDefState getMultiDef() const { return MultiDef; }
  MultiBlockState getMultiBlock() const { return MultiBlock; }
  const Inst *getFirstDefinition() const;
  const Inst *getSingleDefinition() const;
  const InstDefList &getLatterDefinitions() const { return Definitions; }
  const CfgNode *getNode() const { return SingleUseNode; }
  void markUse(MetadataKind TrackingKind, const Inst *Instr,
               const CfgNode *Node, bool IsFromDef, bool IsImplicit);
  void markDef(MetadataKind TrackingKind, const Inst *Instr,
               const CfgNode *Node);

private:
  MultiDefState MultiDef;
  MultiBlockState MultiBlock;
  const CfgNode *SingleUseNode;
  const CfgNode *SingleDefNode;
  // All definitions of the variable are collected here, in increasing
  // order of instruction number.
  InstDefList Definitions;             // Only used if Kind==VMK_All
  const Inst *FirstOrSingleDefinition; // == Definitions[0] if Kind==VMK_All
};

// VariablesMetadata analyzes and summarizes the metadata for the
// complete set of Variables.
class VariablesMetadata {
  VariablesMetadata(const VariablesMetadata &) = delete;
  VariablesMetadata &operator=(const VariablesMetadata &) = delete;

public:
  VariablesMetadata(const Cfg *Func) : Func(Func) {}
  // Initialize the state by traversing all instructions/variables in
  // the CFG.
  void init(MetadataKind TrackingKind);
  // Add a single node.  This is called by init(), and can be called
  // incrementally from elsewhere, e.g. after edge-splitting.
  void addNode(CfgNode *Node);
  // Returns whether the given Variable is tracked in this object.  It
  // should only return false if changes were made to the CFG after
  // running init(), in which case the state is stale and the results
  // shouldn't be trusted (but it may be OK e.g. for dumping).
  bool isTracked(const Variable *Var) const {
    return Var->getIndex() < Metadata.size();
  }

  // Returns whether the given Variable has multiple definitions.
  bool isMultiDef(const Variable *Var) const;
  // Returns the first definition instruction of the given Variable.
  // This is only valid for variables whose definitions are all within
  // the same block, e.g. T after the lowered sequence "T=B; T+=C;
  // A=T", for which getFirstDefinition(T) would return the "T=B"
  // instruction.  For variables with definitions span multiple
  // blocks, nullptr is returned.
  const Inst *getFirstDefinition(const Variable *Var) const;
  // Returns the definition instruction of the given Variable, when
  // the variable has exactly one definition.  Otherwise, nullptr is
  // returned.
  const Inst *getSingleDefinition(const Variable *Var) const;
  // Returns the list of all definition instructions of the given
  // Variable.
  const InstDefList &getLatterDefinitions(const Variable *Var) const;

  // Returns whether the given Variable is live across multiple
  // blocks.  Mainly, this is used to partition Variables into
  // single-block versus multi-block sets for leveraging sparsity in
  // liveness analysis, and for implementing simple stack slot
  // coalescing.  As a special case, function arguments are always
  // considered multi-block because they are live coming into the
  // entry block.
  bool isMultiBlock(const Variable *Var) const;
  // Returns the node that the given Variable is used in, assuming
  // isMultiBlock() returns false.  Otherwise, nullptr is returned.
  const CfgNode *getLocalUseNode(const Variable *Var) const;

private:
  const Cfg *Func;
  MetadataKind Kind;
  std::vector<VariableTracking> Metadata;
  const static InstDefList NoDefinitions;
};

} // end of namespace Ice

#endif // SUBZERO_SRC_ICEOPERAND_H
