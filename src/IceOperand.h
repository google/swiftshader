//===- subzero/src/IceOperand.h - High-level operands -----------*- C++ -*-===//
//
//                        The Subzero Code Generator
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file declares the Operand class and its target-independent subclasses.
/// The main classes are Variable, which represents an LLVM variable that is
/// either register- or stack-allocated, and the Constant hierarchy, which
/// represents integer, floating-point, and/or symbolic constants.
///
//===----------------------------------------------------------------------===//

#ifndef SUBZERO_SRC_ICEOPERAND_H
#define SUBZERO_SRC_ICEOPERAND_H

#include "IceCfg.h"
#include "IceDefs.h"
#include "IceGlobalContext.h"
#include "IceTypes.h"

#include "llvm/Support/Format.h"

namespace Ice {

class Operand {
  Operand() = delete;
  Operand(const Operand &) = delete;
  Operand &operator=(const Operand &) = delete;

public:
  static constexpr size_t MaxTargetKinds = 10;
  enum OperandKind {
    kConst_Base,
    kConstInteger32,
    kConstInteger64,
    kConstFloat,
    kConstDouble,
    kConstRelocatable,
    kConstUndef,
    kConst_Target, // leave space for target-specific constant kinds
    kConst_Max = kConst_Target + MaxTargetKinds,
    kVariable,
    kVariable64On32,
    kVariable_Target, // leave space for target-specific variable kinds
    kVariable_Max = kVariable_Target + MaxTargetKinds,
    // Target-specific operand classes use kTarget as the starting point for
    // their Kind enum space. Note that the value-spaces are shared across
    // targets. To avoid confusion over the definition of shared values, an
    // object specific to one target should never be passed to a different
    // target.
    kTarget,
    kTarget_Max = std::numeric_limits<uint8_t>::max(),
  };
  static_assert(kTarget <= kTarget_Max, "Must not be above max.");
  OperandKind getKind() const { return Kind; }
  Type getType() const { return Ty; }

  /// Every Operand keeps an array of the Variables referenced in the operand.
  /// This is so that the liveness operations can get quick access to the
  /// variables of interest, without having to dig so far into the operand.
  SizeT getNumVars() const { return NumVars; }
  Variable *getVar(SizeT I) const {
    assert(I < getNumVars());
    return Vars[I];
  }
  virtual void emit(const Cfg *Func) const = 0;

  /// \name Dumping functions.
  /// @{

  /// The dump(Func,Str) implementation must be sure to handle the situation
  /// where Func==nullptr.
  virtual void dump(const Cfg *Func, Ostream &Str) const = 0;
  void dump(const Cfg *Func) const {
    if (!BuildDefs::dump())
      return;
    assert(Func);
    dump(Func, Func->getContext()->getStrDump());
  }
  void dump(Ostream &Str) const {
    if (BuildDefs::dump())
      dump(nullptr, Str);
  }
  /// @}

  ~Operand() = default;

protected:
  Operand(OperandKind Kind, Type Ty) : Ty(Ty), Kind(Kind) {
    // It is undefined behavior to have a larger value in the enum
    assert(Kind <= kTarget_Max);
  }

  const Type Ty;
  const OperandKind Kind;
  /// Vars and NumVars are initialized by the derived class.
  SizeT NumVars = 0;
  Variable **Vars = nullptr;
};

template <class StreamType>
inline StreamType &operator<<(StreamType &Str, const Operand &Op) {
  Op.dump(Str);
  return Str;
}

/// Constant is the abstract base class for constants. All constants are
/// allocated from a global arena and are pooled.
class Constant : public Operand {
  Constant() = delete;
  Constant(const Constant &) = delete;
  Constant &operator=(const Constant &) = delete;

public:
  virtual void emitPoolLabel(Ostream &Str, const GlobalContext *Ctx) const {
    (void)Str;
    (void)Ctx;
    llvm::report_fatal_error("emitPoolLabel not defined for type");
  };
  void emit(const Cfg *Func) const override { emit(Func->getTarget()); }
  virtual void emit(TargetLowering *Target) const = 0;

  static bool classof(const Operand *Operand) {
    OperandKind Kind = Operand->getKind();
    return Kind >= kConst_Base && Kind <= kConst_Max;
  }

  /// Judge if this given immediate should be randomized or pooled By default
  /// should return false, only constant integers should truly go through this
  /// method.
  virtual bool shouldBeRandomizedOrPooled(const GlobalContext *Ctx) {
    (void)Ctx;
    return false;
  }

  void setShouldBePooled(bool R) { shouldBePooled = R; }

  bool getShouldBePooled() const { return shouldBePooled; }

protected:
  Constant(OperandKind Kind, Type Ty)
      : Operand(Kind, Ty), shouldBePooled(false) {
    Vars = nullptr;
    NumVars = 0;
  }
  /// Whether we should pool this constant. Usually Float/Double and pooled
  /// Integers should be flagged true.
  bool shouldBePooled;
};

/// ConstantPrimitive<> wraps a primitive type.
template <typename T, Operand::OperandKind K>
class ConstantPrimitive : public Constant {
  ConstantPrimitive() = delete;
  ConstantPrimitive(const ConstantPrimitive &) = delete;
  ConstantPrimitive &operator=(const ConstantPrimitive &) = delete;

public:
  using PrimType = T;

  static ConstantPrimitive *create(GlobalContext *Ctx, Type Ty,
                                   PrimType Value) {
    assert(!Ctx->isIRGenerationDisabled() &&
           "Attempt to build primitive constant when IR generation disabled");
    return new (Ctx->allocate<ConstantPrimitive>())
        ConstantPrimitive(Ty, Value);
  }
  PrimType getValue() const { return Value; }
  void emitPoolLabel(Ostream &Str, const GlobalContext *Ctx) const final {
    Str << ".L$" << getType() << "$";
    // Print hex characters byte by byte, starting from the most significant
    // byte.  NOTE: This ordering assumes Subzero runs on a little-endian
    // platform.  That means the possibility of different label names depending
    // on the endian-ness of the platform where Subzero runs.
    for (unsigned i = 0; i < sizeof(Value); ++i) {
      constexpr unsigned HexWidthChars = 2;
      unsigned Offset = sizeof(Value) - 1 - i;
      Str << llvm::format_hex_no_prefix(
          *(Offset + (const unsigned char *)&Value), HexWidthChars);
    }
    // For a floating-point value in DecorateAsm mode, also append the value in
    // human-readable sprintf form, changing '+' to 'p' and '-' to 'm' to
    // maintain valid asm labels.
    if (std::is_floating_point<PrimType>::value && !BuildDefs::minimal() &&
        Ctx->getFlags().getDecorateAsm()) {
      char Buf[30];
      snprintf(Buf, llvm::array_lengthof(Buf), "$%g", (double)Value);
      for (unsigned i = 0; i < llvm::array_lengthof(Buf) && Buf[i]; ++i) {
        if (Buf[i] == '-')
          Buf[i] = 'm';
        else if (Buf[i] == '+')
          Buf[i] = 'p';
      }
      Str << Buf;
    }
  }
  using Constant::emit;
  void emit(TargetLowering *Target) const final;
  using Constant::dump;
  void dump(const Cfg *, Ostream &Str) const override {
    if (BuildDefs::dump())
      Str << getValue();
  }

  static bool classof(const Operand *Operand) {
    return Operand->getKind() == K;
  }

  virtual bool shouldBeRandomizedOrPooled(const GlobalContext *Ctx) override {
    (void)Ctx;
    return false;
  }

private:
  ConstantPrimitive(Type Ty, PrimType Value) : Constant(K, Ty), Value(Value) {}
  const PrimType Value;
};

using ConstantInteger32 = ConstantPrimitive<int32_t, Operand::kConstInteger32>;
using ConstantInteger64 = ConstantPrimitive<int64_t, Operand::kConstInteger64>;
using ConstantFloat = ConstantPrimitive<float, Operand::kConstFloat>;
using ConstantDouble = ConstantPrimitive<double, Operand::kConstDouble>;

template <>
inline void ConstantInteger32::dump(const Cfg *, Ostream &Str) const {
  if (!BuildDefs::dump())
    return;
  if (getType() == IceType_i1)
    Str << (getValue() ? "true" : "false");
  else
    Str << static_cast<int32_t>(getValue());
}

/// Specialization of the template member function for ConstantInteger32
template <>
bool ConstantInteger32::shouldBeRandomizedOrPooled(const GlobalContext *Ctx);

template <>
inline void ConstantInteger64::dump(const Cfg *, Ostream &Str) const {
  if (!BuildDefs::dump())
    return;
  assert(getType() == IceType_i64);
  Str << static_cast<int64_t>(getValue());
}

/// RelocatableTuple bundles the parameters that are used to construct an
/// ConstantRelocatable. It is done this way so that ConstantRelocatable can fit
/// into the global constant pool template mechanism.
class RelocatableTuple {
  RelocatableTuple() = delete;
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

/// ConstantRelocatable represents a symbolic constant combined with a fixed
/// offset.
class ConstantRelocatable : public Constant {
  ConstantRelocatable() = delete;
  ConstantRelocatable(const ConstantRelocatable &) = delete;
  ConstantRelocatable &operator=(const ConstantRelocatable &) = delete;

public:
  static ConstantRelocatable *create(GlobalContext *Ctx, Type Ty,
                                     const RelocatableTuple &Tuple) {
    assert(!Ctx->isIRGenerationDisabled() &&
           "Attempt to build relocatable constant when IR generation disabled");
    return new (Ctx->allocate<ConstantRelocatable>()) ConstantRelocatable(
        Ty, Tuple.Offset, Tuple.Name, Tuple.SuppressMangling);
  }

  RelocOffsetT getOffset() const { return Offset; }
  const IceString &getName() const { return Name; }
  void setSuppressMangling(bool Value) { SuppressMangling = Value; }
  bool getSuppressMangling() const { return SuppressMangling; }
  using Constant::emit;
  void emit(TargetLowering *Target) const final;
  void emitWithoutPrefix(TargetLowering *Target) const;
  using Constant::dump;
  void dump(const Cfg *Func, Ostream &Str) const override;

  static bool classof(const Operand *Operand) {
    OperandKind Kind = Operand->getKind();
    return Kind == kConstRelocatable;
  }

private:
  ConstantRelocatable(Type Ty, RelocOffsetT Offset, const IceString &Name,
                      bool SuppressMangling)
      : Constant(kConstRelocatable, Ty), Offset(Offset), Name(Name),
        SuppressMangling(SuppressMangling) {}
  const RelocOffsetT Offset; /// fixed offset to add
  const IceString Name;      /// optional for debug/dump
  bool SuppressMangling;
};

/// ConstantUndef represents an unspecified bit pattern. Although it is legal to
/// lower ConstantUndef to any value, backends should try to make code
/// generation deterministic by lowering ConstantUndefs to 0.
class ConstantUndef : public Constant {
  ConstantUndef() = delete;
  ConstantUndef(const ConstantUndef &) = delete;
  ConstantUndef &operator=(const ConstantUndef &) = delete;

public:
  static ConstantUndef *create(GlobalContext *Ctx, Type Ty) {
    assert(!Ctx->isIRGenerationDisabled() &&
           "Attempt to build undefined constant when IR generation disabled");
    return new (Ctx->allocate<ConstantUndef>()) ConstantUndef(Ty);
  }

  using Constant::emit;
  void emit(TargetLowering *Target) const final;
  using Constant::dump;
  void dump(const Cfg *, Ostream &Str) const override {
    if (BuildDefs::dump())
      Str << "undef";
  }

  static bool classof(const Operand *Operand) {
    return Operand->getKind() == kConstUndef;
  }

private:
  ConstantUndef(Type Ty) : Constant(kConstUndef, Ty) {}
};

/// RegWeight is a wrapper for a uint32_t weight value, with a special value
/// that represents infinite weight, and an addWeight() method that ensures that
/// W+infinity=infinity.
class RegWeight {
public:
  RegWeight() = default;
  explicit RegWeight(uint32_t Weight) : Weight(Weight) {}
  RegWeight(const RegWeight &) = default;
  RegWeight &operator=(const RegWeight &) = default;
  constexpr static uint32_t Inf = ~0; /// Force regalloc to give a register
  constexpr static uint32_t Zero = 0; /// Force regalloc NOT to give a register
  constexpr static uint32_t Max = Inf - 1; /// Max natural weight.
  void addWeight(uint32_t Delta) {
    if (Delta == Inf)
      Weight = Inf;
    else if (Weight != Inf)
      if (Utils::add_overflow(Weight, Delta, &Weight) || Weight == Inf)
        Weight = Max;
  }
  void addWeight(const RegWeight &Other) { addWeight(Other.Weight); }
  void setWeight(uint32_t Val) { Weight = Val; }
  uint32_t getWeight() const { return Weight; }

private:
  uint32_t Weight = 0;
};
Ostream &operator<<(Ostream &Str, const RegWeight &W);
bool operator<(const RegWeight &A, const RegWeight &B);
bool operator<=(const RegWeight &A, const RegWeight &B);
bool operator==(const RegWeight &A, const RegWeight &B);

/// LiveRange is a set of instruction number intervals representing a variable's
/// live range. Generally there is one interval per basic block where the
/// variable is live, but adjacent intervals get coalesced into a single
/// interval.
class LiveRange {
public:
  LiveRange() = default;
  /// Special constructor for building a kill set. The advantage is that we can
  /// reserve the right amount of space in advance.
  explicit LiveRange(const CfgVector<InstNumberT> &Kills) {
    Range.reserve(Kills.size());
    for (InstNumberT I : Kills)
      addSegment(I, I);
  }
  LiveRange(const LiveRange &) = default;
  LiveRange &operator=(const LiveRange &) = default;

  void reset() {
    Range.clear();
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
  InstNumberT getEnd() const {
    return Range.empty() ? -1 : Range.rbegin()->second;
  }

  void untrim() { TrimmedBegin = Range.begin(); }
  void trim(InstNumberT Lower);

  void dump(Ostream &Str) const;

private:
  using RangeElementType = std::pair<InstNumberT, InstNumberT>;
  /// RangeType is arena-allocated from the Cfg's allocator.
  using RangeType = CfgVector<RangeElementType>;
  RangeType Range;
  /// TrimmedBegin is an optimization for the overlaps() computation. Since the
  /// linear-scan algorithm always calls it as overlaps(Cur) and Cur advances
  /// monotonically according to live range start, we can optimize overlaps() by
  /// ignoring all segments that end before the start of Cur's range. The
  /// linear-scan code enables this by calling trim() on the ranges of interest
  /// as Cur advances. Note that linear-scan also has to initialize TrimmedBegin
  /// at the beginning by calling untrim().
  RangeType::const_iterator TrimmedBegin;
};

Ostream &operator<<(Ostream &Str, const LiveRange &L);

/// RegClass indicates the physical register class that a Variable may be
/// register-allocated from.  By default, a variable's register class is
/// directly associated with its type.  However, the target lowering may define
/// additional target-specific register classes by extending the set of enum
/// values.
enum RegClass : uint8_t {
// Define RC_void, RC_i1, RC_i8, etc.
#define X(tag, sizeLog2, align, elts, elty, str) RC_##tag = IceType_##tag,
  ICETYPE_TABLE
#undef X
      RC_Target,
  // Leave plenty of space for target-specific values.
  RC_Max = std::numeric_limits<uint8_t>::max()
};
static_assert(RC_Target == static_cast<RegClass>(IceType_NUM),
              "Expected RC_Target and IceType_NUM to be the same");

/// Variable represents an operand that is register-allocated or
/// stack-allocated. If it is register-allocated, it will ultimately have a
/// non-negative RegNum field.
class Variable : public Operand {
  Variable() = delete;
  Variable(const Variable &) = delete;
  Variable &operator=(const Variable &) = delete;

  enum RegRequirement : uint8_t {
    RR_MayHaveRegister,
    RR_MustHaveRegister,
    RR_MustNotHaveRegister,
  };

public:
  static Variable *create(Cfg *Func, Type Ty, SizeT Index) {
    return new (Func->allocate<Variable>()) Variable(kVariable, Ty, Index);
  }

  SizeT getIndex() const { return Number; }
  IceString getName(const Cfg *Func) const;
  virtual void setName(Cfg *Func, const IceString &NewName) {
    // Make sure that the name can only be set once.
    assert(NameIndex == Cfg::IdentifierIndexInvalid);
    if (!NewName.empty())
      NameIndex = Func->addIdentifierName(NewName);
  }

  bool getIsArg() const { return IsArgument; }
  virtual void setIsArg(bool Val = true) { IsArgument = Val; }
  bool getIsImplicitArg() const { return IsImplicitArgument; }
  void setIsImplicitArg(bool Val = true) { IsImplicitArgument = Val; }

  void setIgnoreLiveness() { IgnoreLiveness = true; }
  bool getIgnoreLiveness() const { return IgnoreLiveness; }

  int32_t getStackOffset() const { return StackOffset; }
  void setStackOffset(int32_t Offset) { StackOffset = Offset; }
  /// Returns the variable's stack offset in symbolic form, to improve
  /// readability in DecorateAsm mode.
  IceString getSymbolicStackOffset(const Cfg *Func) const {
    return "lv$" + getName(Func);
  }

  static constexpr int32_t NoRegister = -1;
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

  RegWeight getWeight(const Cfg *Func) const;

  void setMustHaveReg() { RegRequirement = RR_MustHaveRegister; }
  bool mustHaveReg() const { return RegRequirement == RR_MustHaveRegister; }
  void setMustNotHaveReg() { RegRequirement = RR_MustNotHaveRegister; }
  bool mustNotHaveReg() const {
    return RegRequirement == RR_MustNotHaveRegister;
  }
  void setRematerializable(int32_t NewRegNum, int32_t NewOffset) {
    IsRematerializable = true;
    setRegNum(NewRegNum);
    setStackOffset(NewOffset);
    setMustHaveReg();
  }
  bool isRematerializable() const { return IsRematerializable; }

  void setRegClass(uint8_t RC) { RegisterClass = static_cast<RegClass>(RC); }
  RegClass getRegClass() const { return RegisterClass; }

  LiveRange &getLiveRange() { return Live; }
  const LiveRange &getLiveRange() const { return Live; }
  void setLiveRange(const LiveRange &Range) { Live = Range; }
  void resetLiveRange() { Live.reset(); }
  void addLiveRange(InstNumberT Start, InstNumberT End) {
    assert(!getIgnoreLiveness());
    Live.addSegment(Start, End);
  }
  void trimLiveRange(InstNumberT Start) { Live.trim(Start); }
  void untrimLiveRange() { Live.untrim(); }
  bool rangeEndsBefore(const Variable *Other) const {
    return Live.endsBefore(Other->Live);
  }
  bool rangeOverlaps(const Variable *Other) const {
    constexpr bool UseTrimmed = true;
    return Live.overlaps(Other->Live, UseTrimmed);
  }
  bool rangeOverlapsStart(const Variable *Other) const {
    constexpr bool UseTrimmed = true;
    return Live.overlapsInst(Other->Live.getStart(), UseTrimmed);
  }

  /// Creates a temporary copy of the variable with a different type. Used
  /// primarily for syntactic correctness of textual assembly emission. Note
  /// that only basic information is copied, in particular not IsArgument,
  /// IsImplicitArgument, IgnoreLiveness, RegNumTmp, Live, LoVar, HiVar,
  /// VarsReal.  If NewRegNum!=NoRegister, then that register assignment is made
  /// instead of copying the existing assignment.
  const Variable *asType(Type Ty, int32_t NewRegNum) const;

  void emit(const Cfg *Func) const override;
  using Operand::dump;
  void dump(const Cfg *Func, Ostream &Str) const override;

  /// Return reg num of base register, if different from stack/frame register.
  virtual int32_t getBaseRegNum() const { return NoRegister; }

  static bool classof(const Operand *Operand) {
    OperandKind Kind = Operand->getKind();
    return Kind >= kVariable && Kind <= kVariable_Max;
  }

protected:
  Variable(OperandKind K, Type Ty, SizeT Index)
      : Operand(K, Ty), Number(Index),
        RegisterClass(static_cast<RegClass>(Ty)) {
    Vars = VarsReal;
    Vars[0] = this;
    NumVars = 1;
  }
  /// Number is unique across all variables, and is used as a (bit)vector index
  /// for liveness analysis.
  const SizeT Number;
  Cfg::IdentifierIndexType NameIndex = Cfg::IdentifierIndexInvalid;
  bool IsArgument = false;
  bool IsImplicitArgument = false;
  /// IgnoreLiveness means that the variable should be ignored when constructing
  /// and validating live ranges. This is usually reserved for the stack
  /// pointer and other physical registers specifically referenced by name.
  bool IgnoreLiveness = false;
  // If IsRematerializable, RegNum keeps track of which register (stack or frame
  // pointer), and StackOffset is the known offset from that register.
  bool IsRematerializable = false;
  RegRequirement RegRequirement = RR_MayHaveRegister;
  RegClass RegisterClass;
  /// RegNum is the allocated register, or NoRegister if it isn't
  /// register-allocated.
  int32_t RegNum = NoRegister;
  /// RegNumTmp is the tentative assignment during register allocation.
  int32_t RegNumTmp = NoRegister;
  /// StackOffset is the canonical location on stack (only if
  /// RegNum==NoRegister || IsArgument).
  int32_t StackOffset = 0;
  LiveRange Live;
  /// VarsReal (and Operand::Vars) are set up such that Vars[0] == this.
  Variable *VarsReal[1];
};

// Variable64On32 represents a 64-bit variable on a 32-bit architecture. In
// this situation the variable must be split into a low and a high word.
class Variable64On32 : public Variable {
  Variable64On32() = delete;
  Variable64On32(const Variable64On32 &) = delete;
  Variable64On32 &operator=(const Variable64On32 &) = delete;

public:
  static Variable64On32 *create(Cfg *Func, Type Ty, SizeT Index) {
    return new (Func->allocate<Variable64On32>())
        Variable64On32(kVariable64On32, Ty, Index);
  }

  void setName(Cfg *Func, const IceString &NewName) override {
    Variable::setName(Func, NewName);
    if (LoVar && HiVar) {
      LoVar->setName(Func, getName(Func) + "__lo");
      HiVar->setName(Func, getName(Func) + "__hi");
    }
  }

  void setIsArg(bool Val = true) override {
    Variable::setIsArg(Val);
    if (LoVar && HiVar) {
      LoVar->setIsArg(Val);
      HiVar->setIsArg(Val);
    }
  }

  Variable *getLo() const {
    assert(LoVar != nullptr);
    return LoVar;
  }
  Variable *getHi() const {
    assert(HiVar != nullptr);
    return HiVar;
  }

  void initHiLo(Cfg *Func) {
    assert(LoVar == nullptr);
    assert(HiVar == nullptr);
    LoVar = Func->makeVariable(IceType_i32);
    HiVar = Func->makeVariable(IceType_i32);
    LoVar->setIsArg(getIsArg());
    HiVar->setIsArg(getIsArg());
    LoVar->setName(Func, getName(Func) + "__lo");
    HiVar->setName(Func, getName(Func) + "__hi");
  }

  static bool classof(const Operand *Operand) {
    OperandKind Kind = Operand->getKind();
    return Kind == kVariable64On32;
  }

protected:
  Variable64On32(OperandKind K, Type Ty, SizeT Index) : Variable(K, Ty, Index) {
    assert(typeWidthInBytes(Ty) == 8);
  }

  Variable *LoVar = nullptr;
  Variable *HiVar = nullptr;
};

enum MetadataKind {
  VMK_Uses,       /// Track only uses, not defs
  VMK_SingleDefs, /// Track uses+defs, but only record single def
  VMK_All         /// Track uses+defs, including full def list
};
using InstDefList = CfgVector<const Inst *>;

/// VariableTracking tracks the metadata for a single variable.  It is
/// only meant to be used internally by VariablesMetadata.
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
  VariableTracking() = default;
  VariableTracking(const VariableTracking &) = default;
  MultiDefState getMultiDef() const { return MultiDef; }
  MultiBlockState getMultiBlock() const { return MultiBlock; }
  const Inst *getFirstDefinitionSingleBlock() const;
  const Inst *getSingleDefinition() const;
  const Inst *getFirstDefinition() const;
  const InstDefList &getLatterDefinitions() const { return Definitions; }
  CfgNode *getNode() const { return SingleUseNode; }
  RegWeight getUseWeight() const { return UseWeight; }
  void markUse(MetadataKind TrackingKind, const Inst *Instr, CfgNode *Node,
               bool IsImplicit);
  void markDef(MetadataKind TrackingKind, const Inst *Instr, CfgNode *Node);

private:
  MultiDefState MultiDef = MDS_Unknown;
  MultiBlockState MultiBlock = MBS_Unknown;
  CfgNode *SingleUseNode = nullptr;
  CfgNode *SingleDefNode = nullptr;
  /// All definitions of the variable are collected in Definitions[] (except for
  /// the earliest definition), in increasing order of instruction number.
  InstDefList Definitions; /// Only used if Kind==VMK_All
  const Inst *FirstOrSingleDefinition = nullptr;
  RegWeight UseWeight;
};

/// VariablesMetadata analyzes and summarizes the metadata for the complete set
/// of Variables.
class VariablesMetadata {
  VariablesMetadata() = delete;
  VariablesMetadata(const VariablesMetadata &) = delete;
  VariablesMetadata &operator=(const VariablesMetadata &) = delete;

public:
  explicit VariablesMetadata(const Cfg *Func) : Func(Func) {}
  /// Initialize the state by traversing all instructions/variables in the CFG.
  void init(MetadataKind TrackingKind);
  /// Add a single node. This is called by init(), and can be called
  /// incrementally from elsewhere, e.g. after edge-splitting.
  void addNode(CfgNode *Node);
  MetadataKind getKind() const { return Kind; }
  /// Returns whether the given Variable is tracked in this object. It should
  /// only return false if changes were made to the CFG after running init(), in
  /// which case the state is stale and the results shouldn't be trusted (but it
  /// may be OK e.g. for dumping).
  bool isTracked(const Variable *Var) const {
    return Var->getIndex() < Metadata.size();
  }

  /// Returns whether the given Variable has multiple definitions.
  bool isMultiDef(const Variable *Var) const;
  /// Returns the first definition instruction of the given Variable. This is
  /// only valid for variables whose definitions are all within the same block,
  /// e.g. T after the lowered sequence "T=B; T+=C; A=T", for which
  /// getFirstDefinitionSingleBlock(T) would return the "T=B" instruction. For
  /// variables with definitions span multiple blocks, nullptr is returned.
  const Inst *getFirstDefinitionSingleBlock(const Variable *Var) const;
  /// Returns the definition instruction of the given Variable, when the
  /// variable has exactly one definition. Otherwise, nullptr is returned.
  const Inst *getSingleDefinition(const Variable *Var) const;
  /// getFirstDefinition() and getLatterDefinitions() are used together to
  /// return the complete set of instructions that define the given Variable,
  /// regardless of whether the definitions are within the same block (in
  /// contrast to getFirstDefinitionSingleBlock).
  const Inst *getFirstDefinition(const Variable *Var) const;
  const InstDefList &getLatterDefinitions(const Variable *Var) const;

  /// Returns whether the given Variable is live across multiple blocks. Mainly,
  /// this is used to partition Variables into single-block versus multi-block
  /// sets for leveraging sparsity in liveness analysis, and for implementing
  /// simple stack slot coalescing. As a special case, function arguments are
  /// always considered multi-block because they are live coming into the entry
  /// block.
  bool isMultiBlock(const Variable *Var) const;
  /// Returns the node that the given Variable is used in, assuming
  /// isMultiBlock() returns false. Otherwise, nullptr is returned.
  CfgNode *getLocalUseNode(const Variable *Var) const;

  /// Returns the total use weight computed as the sum of uses multiplied by a
  /// loop nest depth factor for each use.
  RegWeight getUseWeight(const Variable *Var) const;

private:
  const Cfg *Func;
  MetadataKind Kind;
  CfgVector<VariableTracking> Metadata;
  const static InstDefList NoDefinitions;
};

} // end of namespace Ice

#endif // SUBZERO_SRC_ICEOPERAND_H
