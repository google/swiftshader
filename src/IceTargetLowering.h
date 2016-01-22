//===- subzero/src/IceTargetLowering.h - Lowering interface -----*- C++ -*-===//
//
//                        The Subzero Code Generator
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief Declares the TargetLowering, LoweringContext, and TargetDataLowering
/// classes.
///
/// TargetLowering is an abstract class used to drive the translation/lowering
/// process. LoweringContext maintains a context for lowering each instruction,
/// offering conveniences such as iterating over non-deleted instructions.
/// TargetDataLowering is an abstract class used to drive the lowering/emission
/// of global initializers, external global declarations, and internal constant
/// pools.
///
//===----------------------------------------------------------------------===//

#ifndef SUBZERO_SRC_ICETARGETLOWERING_H
#define SUBZERO_SRC_ICETARGETLOWERING_H

#include "IceCfgNode.h"
#include "IceDefs.h"
#include "IceInst.h" // for the names of the Inst subtypes
#include "IceOperand.h"
#include "IceTypes.h"

#include <utility>

namespace Ice {

// UnimplementedError is defined as a macro so that we can get actual line
// numbers.
#define UnimplementedError(Flags)                                              \
  do {                                                                         \
    if (!static_cast<const ClFlags &>(Flags).getSkipUnimplemented()) {         \
      /* Use llvm_unreachable instead of report_fatal_error, which gives       \
         better stack traces. */                                               \
      llvm_unreachable("Not yet implemented");                                 \
      abort();                                                                 \
    }                                                                          \
  } while (0)

// UnimplementedLoweringError is similar in style to UnimplementedError.  Given
// a TargetLowering object pointer and an Inst pointer, it adds appropriate
// FakeDef and FakeUse instructions to try maintain liveness consistency.
#define UnimplementedLoweringError(Target, Instr)                              \
  do {                                                                         \
    if ((Target)->Ctx->getFlags().getSkipUnimplemented()) {                    \
      (Target)->addFakeDefUses(Instr);                                         \
    } else {                                                                   \
      /* Use llvm_unreachable instead of report_fatal_error, which gives       \
         better stack traces. */                                               \
      llvm_unreachable("Not yet implemented");                                 \
      abort();                                                                 \
    }                                                                          \
  } while (0)

/// LoweringContext makes it easy to iterate through non-deleted instructions in
/// a node, and insert new (lowered) instructions at the current point. Along
/// with the instruction list container and associated iterators, it holds the
/// current node, which is needed when inserting new instructions in order to
/// track whether variables are used as single-block or multi-block.
class LoweringContext {
  LoweringContext(const LoweringContext &) = delete;
  LoweringContext &operator=(const LoweringContext &) = delete;

public:
  LoweringContext() = default;
  ~LoweringContext() = default;
  void init(CfgNode *Node);
  Inst *getNextInst() const {
    if (Next == End)
      return nullptr;
    return Next;
  }
  Inst *getNextInst(InstList::iterator &Iter) const {
    advanceForward(Iter);
    if (Iter == End)
      return nullptr;
    return Iter;
  }
  CfgNode *getNode() const { return Node; }
  bool atEnd() const { return Cur == End; }
  InstList::iterator getCur() const { return Cur; }
  InstList::iterator getNext() const { return Next; }
  InstList::iterator getEnd() const { return End; }
  void insert(Inst *Inst);
  template <typename Inst, typename... Args> Inst *insert(Args &&... A) {
    auto *New = Inst::create(Node->getCfg(), std::forward<Args>(A)...);
    insert(New);
    return New;
  }
  Inst *getLastInserted() const;
  void advanceCur() { Cur = Next; }
  void advanceNext() { advanceForward(Next); }
  void setCur(InstList::iterator C) { Cur = C; }
  void setNext(InstList::iterator N) { Next = N; }
  void rewind();
  void setInsertPoint(const InstList::iterator &Position) { Next = Position; }
  void availabilityReset();
  void availabilityUpdate();
  Variable *availabilityGet(Operand *Src) const;

private:
  /// Node is the argument to Inst::updateVars().
  CfgNode *Node = nullptr;
  Inst *LastInserted = nullptr;
  /// Cur points to the current instruction being considered. It is guaranteed
  /// to point to a non-deleted instruction, or to be End.
  InstList::iterator Cur;
  /// Next doubles as a pointer to the next valid instruction (if any), and the
  /// new-instruction insertion point. It is also updated for the caller in case
  /// the lowering consumes more than one high-level instruction. It is
  /// guaranteed to point to a non-deleted instruction after Cur, or to be End.
  // TODO: Consider separating the notion of "next valid instruction" and "new
  // instruction insertion point", to avoid confusion when previously-deleted
  // instructions come between the two points.
  InstList::iterator Next;
  /// Begin is a copy of Insts.begin(), used if iterators are moved backward.
  InstList::iterator Begin;
  /// End is a copy of Insts.end(), used if Next needs to be advanced.
  InstList::iterator End;
  /// LastDest and LastSrc capture the parameters of the last "Dest=Src" simple
  /// assignment inserted (provided Src is a variable).  This is used for simple
  /// availability analysis.
  Variable *LastDest = nullptr;
  Variable *LastSrc = nullptr;

  void skipDeleted(InstList::iterator &I) const;
  void advanceForward(InstList::iterator &I) const;
};

/// A helper class to advance the LoweringContext at each loop iteration.
class PostIncrLoweringContext {
  PostIncrLoweringContext() = delete;
  PostIncrLoweringContext(const PostIncrLoweringContext &) = delete;
  PostIncrLoweringContext &operator=(const PostIncrLoweringContext &) = delete;

public:
  explicit PostIncrLoweringContext(LoweringContext &Context)
      : Context(Context) {}
  ~PostIncrLoweringContext() {
    Context.advanceCur();
    Context.advanceNext();
  }

private:
  LoweringContext &Context;
};

/// TargetLowering is the base class for all backends in Subzero. In addition to
/// implementing the abstract methods in this class, each concrete target must
/// also implement a named constructor in its own namespace. For instance, for
/// X8632 we have:
///
///  namespace X8632 {
///    void createTargetLowering(Cfg *Func);
///  }
class TargetLowering {
  TargetLowering() = delete;
  TargetLowering(const TargetLowering &) = delete;
  TargetLowering &operator=(const TargetLowering &) = delete;

public:
  static void staticInit(GlobalContext *Ctx);
  // Each target must define a public static method:
  //   static void staticInit(GlobalContext *Ctx);

  static std::unique_ptr<TargetLowering> createLowering(TargetArch Target,
                                                        Cfg *Func);

  virtual std::unique_ptr<Assembler> createAssembler() const = 0;

  void translate() {
    switch (Ctx->getFlags().getOptLevel()) {
    case Opt_m1:
      translateOm1();
      break;
    case Opt_0:
      translateO0();
      break;
    case Opt_1:
      translateO1();
      break;
    case Opt_2:
      translateO2();
      break;
    }
  }
  virtual void translateOm1() {
    Func->setError("Target doesn't specify Om1 lowering steps.");
  }
  virtual void translateO0() {
    Func->setError("Target doesn't specify O0 lowering steps.");
  }
  virtual void translateO1() {
    Func->setError("Target doesn't specify O1 lowering steps.");
  }
  virtual void translateO2() {
    Func->setError("Target doesn't specify O2 lowering steps.");
  }

  /// Generates calls to intrinsics for operations the Target can't handle.
  void genTargetHelperCalls();
  /// Tries to do address mode optimization on a single instruction.
  void doAddressOpt();
  /// Randomly insert NOPs.
  void doNopInsertion(RandomNumberGenerator &RNG);
  /// Lowers a single non-Phi instruction.
  void lower();
  /// Inserts and lowers a single high-level instruction at a specific insertion
  /// point.
  void lowerInst(CfgNode *Node, InstList::iterator Next, InstHighLevel *Instr);
  /// Does preliminary lowering of the set of Phi instructions in the current
  /// node. The main intention is to do what's needed to keep the unlowered Phi
  /// instructions consistent with the lowered non-Phi instructions, e.g. to
  /// lower 64-bit operands on a 32-bit target.
  virtual void prelowerPhis() {}
  /// Tries to do branch optimization on a single instruction. Returns true if
  /// some optimization was done.
  virtual bool doBranchOpt(Inst * /*I*/, const CfgNode * /*NextNode*/) {
    return false;
  }

  virtual SizeT getNumRegisters() const = 0;
  /// Returns a variable pre-colored to the specified physical register. This is
  /// generally used to get very direct access to the register such as in the
  /// prolog or epilog or for marking scratch registers as killed by a call. If
  /// a Type is not provided, a target-specific default type is used.
  virtual Variable *getPhysicalRegister(SizeT RegNum,
                                        Type Ty = IceType_void) = 0;
  /// Returns a printable name for the register.
  virtual IceString getRegName(SizeT RegNum, Type Ty) const = 0;

  virtual bool hasFramePointer() const { return false; }
  virtual void setHasFramePointer() = 0;
  virtual SizeT getStackReg() const = 0;
  virtual SizeT getFrameReg() const = 0;
  virtual SizeT getFrameOrStackReg() const = 0;
  virtual size_t typeWidthInBytesOnStack(Type Ty) const = 0;
  virtual uint32_t getStackAlignment() const = 0;
  virtual void reserveFixedAllocaArea(size_t Size, size_t Align) = 0;
  virtual int32_t getFrameFixedAllocaOffset() const = 0;
  virtual uint32_t maxOutArgsSizeBytes() const { return 0; }

  /// Return whether a 64-bit Variable should be split into a Variable64On32.
  virtual bool shouldSplitToVariable64On32(Type Ty) const = 0;

  bool hasComputedFrame() const { return HasComputedFrame; }
  /// Returns true if this function calls a function that has the "returns
  /// twice" attribute.
  bool callsReturnsTwice() const { return CallsReturnsTwice; }
  void setCallsReturnsTwice(bool RetTwice) { CallsReturnsTwice = RetTwice; }
  SizeT makeNextLabelNumber() { return NextLabelNumber++; }
  SizeT makeNextJumpTableNumber() { return NextJumpTableNumber++; }
  LoweringContext &getContext() { return Context; }
  Cfg *getFunc() const { return Func; }
  GlobalContext *getGlobalContext() const { return Ctx; }

  enum RegSet {
    RegSet_None = 0,
    RegSet_CallerSave = 1 << 0,
    RegSet_CalleeSave = 1 << 1,
    RegSet_StackPointer = 1 << 2,
    RegSet_FramePointer = 1 << 3,
    RegSet_All = ~RegSet_None
  };
  using RegSetMask = uint32_t;

  virtual llvm::SmallBitVector getRegisterSet(RegSetMask Include,
                                              RegSetMask Exclude) const = 0;
  virtual const llvm::SmallBitVector &
  getRegistersForVariable(const Variable *Var) const = 0;
  virtual const llvm::SmallBitVector &getAliasesForRegister(SizeT) const = 0;

  void regAlloc(RegAllocKind Kind);

  virtual void
  makeRandomRegisterPermutation(llvm::SmallVectorImpl<int32_t> &Permutation,
                                const llvm::SmallBitVector &ExcludeRegisters,
                                uint64_t Salt) const = 0;

  /// Get the minimum number of clusters required for a jump table to be
  /// considered.
  virtual SizeT getMinJumpTableSize() const = 0;
  virtual void emitJumpTable(const Cfg *Func,
                             const InstJumpTable *JumpTable) const = 0;

  virtual void emitVariable(const Variable *Var) const = 0;

  void emitWithoutPrefix(const ConstantRelocatable *CR,
                         const char *Suffix = "") const;

  virtual void emit(const ConstantInteger32 *C) const = 0;
  virtual void emit(const ConstantInteger64 *C) const = 0;
  virtual void emit(const ConstantFloat *C) const = 0;
  virtual void emit(const ConstantDouble *C) const = 0;
  virtual void emit(const ConstantUndef *C) const = 0;
  virtual void emit(const ConstantRelocatable *CR) const = 0;

  /// Performs target-specific argument lowering.
  virtual void lowerArguments() = 0;

  virtual void initNodeForLowering(CfgNode *) {}
  virtual void addProlog(CfgNode *Node) = 0;
  virtual void addEpilog(CfgNode *Node) = 0;

  virtual ~TargetLowering() = default;

private:
  // This control variable is used by AutoBundle (RAII-style bundle
  // locking/unlocking) to prevent nested bundles.
  bool AutoBundling = false;

  // _bundle_lock(), and _bundle_unlock(), were made private to force subtargets
  // to use the AutoBundle helper.
  void
  _bundle_lock(InstBundleLock::Option BundleOption = InstBundleLock::Opt_None) {
    Context.insert<InstBundleLock>(BundleOption);
  }
  void _bundle_unlock() { Context.insert<InstBundleUnlock>(); }

protected:
  /// AutoBundle provides RIAA-style bundling. Sub-targets are expected to use
  /// it when emitting NaCl Bundles to ensure proper bundle_unlocking, and
  /// prevent nested bundles.
  ///
  /// AutoBundle objects will emit a _bundle_lock during construction (but only
  /// if sandboxed code generation was requested), and a bundle_unlock() during
  /// destruction. By carefully scoping objects of this type, Subtargets can
  /// ensure proper bundle emission.
  class AutoBundle {
    AutoBundle() = delete;
    AutoBundle(const AutoBundle &) = delete;
    AutoBundle &operator=(const AutoBundle &) = delete;

  public:
    explicit AutoBundle(TargetLowering *Target, InstBundleLock::Option Option =
                                                    InstBundleLock::Opt_None);
    ~AutoBundle();

  private:
    TargetLowering *const Target;
    const bool NeedSandboxing;
  };

  explicit TargetLowering(Cfg *Func);
  // Applies command line filters to TypeToRegisterSet array.
  static void
  filterTypeToRegisterSet(GlobalContext *Ctx, int32_t NumRegs,
                          llvm::SmallBitVector TypeToRegisterSet[],
                          size_t TypeToRegisterSetSize,
                          std::function<IceString(int32_t)> getRegName,
                          std::function<IceString(RegClass)> getRegClassName);
  virtual void lowerAlloca(const InstAlloca *Inst) = 0;
  virtual void lowerArithmetic(const InstArithmetic *Inst) = 0;
  virtual void lowerAssign(const InstAssign *Inst) = 0;
  virtual void lowerBr(const InstBr *Inst) = 0;
  virtual void lowerCall(const InstCall *Inst) = 0;
  virtual void lowerCast(const InstCast *Inst) = 0;
  virtual void lowerFcmp(const InstFcmp *Inst) = 0;
  virtual void lowerExtractElement(const InstExtractElement *Inst) = 0;
  virtual void lowerIcmp(const InstIcmp *Inst) = 0;
  virtual void lowerInsertElement(const InstInsertElement *Inst) = 0;
  virtual void lowerIntrinsicCall(const InstIntrinsicCall *Inst) = 0;
  virtual void lowerLoad(const InstLoad *Inst) = 0;
  virtual void lowerPhi(const InstPhi *Inst) = 0;
  virtual void lowerRet(const InstRet *Inst) = 0;
  virtual void lowerSelect(const InstSelect *Inst) = 0;
  virtual void lowerStore(const InstStore *Inst) = 0;
  virtual void lowerSwitch(const InstSwitch *Inst) = 0;
  virtual void lowerUnreachable(const InstUnreachable *Inst) = 0;
  virtual void lowerOther(const Inst *Instr);

  virtual void genTargetHelperCallFor(Inst *Instr) = 0;
  virtual uint32_t getCallStackArgumentsSizeBytes(const InstCall *Instr) = 0;

  virtual void doAddressOptLoad() {}
  virtual void doAddressOptStore() {}
  virtual void doMockBoundsCheck(Operand *) {}
  virtual void randomlyInsertNop(float Probability,
                                 RandomNumberGenerator &RNG) = 0;
  /// This gives the target an opportunity to post-process the lowered expansion
  /// before returning.
  virtual void postLower() {}

  /// When the SkipUnimplemented flag is set, addFakeDefUses() gets invoked by
  /// the UnimplementedLoweringError macro to insert fake uses of all the
  /// instruction variables and a fake def of the instruction dest, in order to
  /// preserve integrity of liveness analysis.
  void addFakeDefUses(const Inst *Instr);

  /// Find (non-SSA) instructions where the Dest variable appears in some source
  /// operand, and set the IsDestRedefined flag.  This keeps liveness analysis
  /// consistent.
  void markRedefinitions();

  /// Make a pass over the Cfg to determine which variables need stack slots and
  /// place them in a sorted list (SortedSpilledVariables). Among those, vars,
  /// classify the spill variables as local to the basic block vs global
  /// (multi-block) in order to compute the parameters GlobalsSize and
  /// SpillAreaSizeBytes (represents locals or general vars if the coalescing of
  /// locals is disallowed) along with alignments required for variables in each
  /// area. We rely on accurate VMetadata in order to classify a variable as
  /// global vs local (otherwise the variable is conservatively global). The
  /// in-args should be initialized to 0.
  ///
  /// This is only a pre-pass and the actual stack slot assignment is handled
  /// separately.
  ///
  /// There may be target-specific Variable types, which will be handled by
  /// TargetVarHook. If the TargetVarHook returns true, then the variable is
  /// skipped and not considered with the rest of the spilled variables.
  void getVarStackSlotParams(VarList &SortedSpilledVariables,
                             llvm::SmallBitVector &RegsUsed,
                             size_t *GlobalsSize, size_t *SpillAreaSizeBytes,
                             uint32_t *SpillAreaAlignmentBytes,
                             uint32_t *LocalsSlotsAlignmentBytes,
                             std::function<bool(Variable *)> TargetVarHook);

  /// Calculate the amount of padding needed to align the local and global areas
  /// to the required alignment. This assumes the globals/locals layout used by
  /// getVarStackSlotParams and assignVarStackSlots.
  void alignStackSpillAreas(uint32_t SpillAreaStartOffset,
                            uint32_t SpillAreaAlignmentBytes,
                            size_t GlobalsSize,
                            uint32_t LocalsSlotsAlignmentBytes,
                            uint32_t *SpillAreaPaddingBytes,
                            uint32_t *LocalsSlotsPaddingBytes);

  /// Make a pass through the SortedSpilledVariables and actually assign stack
  /// slots. SpillAreaPaddingBytes takes into account stack alignment padding.
  /// The SpillArea starts after that amount of padding. This matches the scheme
  /// in getVarStackSlotParams, where there may be a separate multi-block global
  /// var spill area and a local var spill area.
  void assignVarStackSlots(VarList &SortedSpilledVariables,
                           size_t SpillAreaPaddingBytes,
                           size_t SpillAreaSizeBytes,
                           size_t GlobalsAndSubsequentPaddingSize,
                           bool UsesFramePointer);

  /// Sort the variables in Source based on required alignment. The variables
  /// with the largest alignment need are placed in the front of the Dest list.
  void sortVarsByAlignment(VarList &Dest, const VarList &Source) const;

  /// Make a call to an external helper function.
  InstCall *makeHelperCall(const IceString &Name, Variable *Dest,
                           SizeT MaxSrcs);

  void _set_dest_redefined() { Context.getLastInserted()->setDestRedefined(); }

  bool shouldOptimizeMemIntrins();

  /// SandboxType enumerates all possible sandboxing strategies that
  enum SandboxType {
    ST_None,
    ST_NaCl,
    ST_Nonsfi,
  };

  static SandboxType determineSandboxTypeFromFlags(const ClFlags &Flags);

  Cfg *Func;
  GlobalContext *Ctx;
  bool HasComputedFrame = false;
  bool CallsReturnsTwice = false;
  SizeT NextLabelNumber = 0;
  SizeT NextJumpTableNumber = 0;
  LoweringContext Context;
  const SandboxType SandboxingType = ST_None;

  // Runtime helper function names
  const static constexpr char *H_bitcast_16xi1_i16 = "__Sz_bitcast_16xi1_i16";
  const static constexpr char *H_bitcast_8xi1_i8 = "__Sz_bitcast_8xi1_i8";
  const static constexpr char *H_bitcast_i16_16xi1 = "__Sz_bitcast_i16_16xi1";
  const static constexpr char *H_bitcast_i8_8xi1 = "__Sz_bitcast_i8_8xi1";
  const static constexpr char *H_call_ctpop_i32 = "__popcountsi2";
  const static constexpr char *H_call_ctpop_i64 = "__popcountdi2";
  const static constexpr char *H_call_longjmp = "longjmp";
  const static constexpr char *H_call_memcpy = "memcpy";
  const static constexpr char *H_call_memmove = "memmove";
  const static constexpr char *H_call_memset = "memset";
  const static constexpr char *H_call_read_tp = "__nacl_read_tp";
  const static constexpr char *H_call_setjmp = "setjmp";
  const static constexpr char *H_fptosi_f32_i64 = "__Sz_fptosi_f32_i64";
  const static constexpr char *H_fptosi_f64_i64 = "__Sz_fptosi_f64_i64";
  const static constexpr char *H_fptoui_4xi32_f32 = "__Sz_fptoui_4xi32_f32";
  const static constexpr char *H_fptoui_f32_i32 = "__Sz_fptoui_f32_i32";
  const static constexpr char *H_fptoui_f32_i64 = "__Sz_fptoui_f32_i64";
  const static constexpr char *H_fptoui_f64_i32 = "__Sz_fptoui_f64_i32";
  const static constexpr char *H_fptoui_f64_i64 = "__Sz_fptoui_f64_i64";
  const static constexpr char *H_frem_f32 = "fmodf";
  const static constexpr char *H_frem_f64 = "fmod";
  const static constexpr char *H_getIP_prefix = "__Sz_getIP_";
  const static constexpr char *H_sdiv_i32 = "__divsi3";
  const static constexpr char *H_sdiv_i64 = "__divdi3";
  const static constexpr char *H_sitofp_i64_f32 = "__Sz_sitofp_i64_f32";
  const static constexpr char *H_sitofp_i64_f64 = "__Sz_sitofp_i64_f64";
  const static constexpr char *H_srem_i32 = "__modsi3";
  const static constexpr char *H_srem_i64 = "__moddi3";
  const static constexpr char *H_udiv_i32 = "__udivsi3";
  const static constexpr char *H_udiv_i64 = "__udivdi3";
  const static constexpr char *H_uitofp_4xi32_4xf32 = "__Sz_uitofp_4xi32_4xf32";
  const static constexpr char *H_uitofp_i32_f32 = "__Sz_uitofp_i32_f32";
  const static constexpr char *H_uitofp_i32_f64 = "__Sz_uitofp_i32_f64";
  const static constexpr char *H_uitofp_i64_f32 = "__Sz_uitofp_i64_f32";
  const static constexpr char *H_uitofp_i64_f64 = "__Sz_uitofp_i64_f64";
  const static constexpr char *H_urem_i32 = "__umodsi3";
  const static constexpr char *H_urem_i64 = "__umoddi3";
};

/// TargetDataLowering is used for "lowering" data including initializers for
/// global variables, and the internal constant pools. It is separated out from
/// TargetLowering because it does not require a Cfg.
class TargetDataLowering {
  TargetDataLowering() = delete;
  TargetDataLowering(const TargetDataLowering &) = delete;
  TargetDataLowering &operator=(const TargetDataLowering &) = delete;

public:
  static std::unique_ptr<TargetDataLowering> createLowering(GlobalContext *Ctx);
  virtual ~TargetDataLowering();

  virtual void lowerGlobals(const VariableDeclarationList &Vars,
                            const IceString &SectionSuffix) = 0;
  virtual void lowerConstants() = 0;
  virtual void lowerJumpTables() = 0;

protected:
  void emitGlobal(const VariableDeclaration &Var,
                  const IceString &SectionSuffix);

  /// For now, we assume .long is the right directive for emitting 4 byte emit
  /// global relocations. However, LLVM MIPS usually uses .4byte instead.
  /// Perhaps there is some difference when the location is unaligned.
  static const char *getEmit32Directive() { return ".long"; }

  explicit TargetDataLowering(GlobalContext *Ctx) : Ctx(Ctx) {}
  GlobalContext *Ctx;
};

/// TargetHeaderLowering is used to "lower" the header of an output file. It
/// writes out the target-specific header attributes. E.g., for ARM this writes
/// out the build attributes (float ABI, etc.).
class TargetHeaderLowering {
  TargetHeaderLowering() = delete;
  TargetHeaderLowering(const TargetHeaderLowering &) = delete;
  TargetHeaderLowering &operator=(const TargetHeaderLowering &) = delete;

public:
  static std::unique_ptr<TargetHeaderLowering>
  createLowering(GlobalContext *Ctx);
  virtual ~TargetHeaderLowering();

  virtual void lower() {}

protected:
  explicit TargetHeaderLowering(GlobalContext *Ctx) : Ctx(Ctx) {}
  GlobalContext *Ctx;
};

} // end of namespace Ice

#endif // SUBZERO_SRC_ICETARGETLOWERING_H
