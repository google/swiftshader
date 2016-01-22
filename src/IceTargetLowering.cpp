//===- subzero/src/IceTargetLowering.cpp - Basic lowering implementation --===//
//
//                        The Subzero Code Generator
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief Implements the skeleton of the TargetLowering class.
///
/// Specifically this invokes the appropriate lowering method for a given
/// instruction kind and driving global register allocation. It also implements
/// the non-deleted instruction iteration in LoweringContext.
///
//===----------------------------------------------------------------------===//

#include "IceTargetLowering.h"

#include "IceCfg.h" // setError()
#include "IceCfgNode.h"
#include "IceGlobalContext.h"
#include "IceGlobalInits.h"
#include "IceInstVarIter.h"
#include "IceOperand.h"
#include "IceRegAlloc.h"

// We prevent target-specific implementation details from leaking outside their
// implementations by forbidding #include of target-specific header files
// anywhere outside their own files. To create target-specific objects
// (TargetLowering, TargetDataLowering, and TargetHeaderLowering) we use the
// following named constructors. For reference, each target Foo needs to
// implement the following named constructors and initializer:
//
// namespace Foo {
//   unique_ptr<Ice::TargetLowering> createTargetLowering(Ice::Cfg *);
//   unique_ptr<Ice::TargetDataLowering>
//       createTargetDataLowering(Ice::GlobalContext*);
//   unique_ptr<Ice::TargetHeaderLowering>
//       createTargetHeaderLowering(Ice::GlobalContext *);
//   void staticInit(::Ice::GlobalContext *);
// }
#define SUBZERO_TARGET(X)                                                      \
  namespace X {                                                                \
  std::unique_ptr<::Ice::TargetLowering>                                       \
  createTargetLowering(::Ice::Cfg *Func);                                      \
  std::unique_ptr<::Ice::TargetDataLowering>                                   \
  createTargetDataLowering(::Ice::GlobalContext *Ctx);                         \
  std::unique_ptr<::Ice::TargetHeaderLowering>                                 \
  createTargetHeaderLowering(::Ice::GlobalContext *Ctx);                       \
  void staticInit(::Ice::GlobalContext *Ctx);                                  \
  } // end of namespace X
#include "llvm/Config/SZTargets.def"
#undef SUBZERO_TARGET

namespace Ice {
void LoweringContext::init(CfgNode *N) {
  Node = N;
  End = getNode()->getInsts().end();
  rewind();
  advanceForward(Next);
}

void LoweringContext::rewind() {
  Begin = getNode()->getInsts().begin();
  Cur = Begin;
  skipDeleted(Cur);
  Next = Cur;
  availabilityReset();
}

void LoweringContext::insert(Inst *Inst) {
  getNode()->getInsts().insert(Next, Inst);
  LastInserted = Inst;
}

void LoweringContext::skipDeleted(InstList::iterator &I) const {
  while (I != End && I->isDeleted())
    ++I;
}

void LoweringContext::advanceForward(InstList::iterator &I) const {
  if (I != End) {
    ++I;
    skipDeleted(I);
  }
}

Inst *LoweringContext::getLastInserted() const {
  assert(LastInserted);
  return LastInserted;
}

void LoweringContext::availabilityReset() {
  LastDest = nullptr;
  LastSrc = nullptr;
}

void LoweringContext::availabilityUpdate() {
  availabilityReset();
  Inst *Instr = LastInserted;
  if (Instr == nullptr)
    return;
  if (!Instr->isVarAssign())
    return;
  // Since isVarAssign() is true, the source operand must be a Variable.
  LastDest = Instr->getDest();
  LastSrc = llvm::cast<Variable>(Instr->getSrc(0));
}

Variable *LoweringContext::availabilityGet(Operand *Src) const {
  assert(Src);
  if (Src == LastDest)
    return LastSrc;
  return nullptr;
}

namespace {

void printRegisterSet(Ostream &Str, const llvm::SmallBitVector &Bitset,
                      std::function<IceString(int32_t)> getRegName,
                      const IceString &LineIndentString) {
  constexpr size_t RegistersPerLine = 16;
  size_t Count = 0;
  for (int i = Bitset.find_first(); i != -1; i = Bitset.find_next(i)) {
    if (Count == 0) {
      Str << LineIndentString;
    } else {
      Str << ",";
    }
    if (Count > 0 && Count % RegistersPerLine == 0)
      Str << "\n" << LineIndentString;
    ++Count;
    Str << getRegName(i);
  }
  if (Count)
    Str << "\n";
}

// Splits "<class>:<reg>" into "<class>" plus "<reg>".  If there is no <class>
// component, the result is "" plus "<reg>".
void splitToClassAndName(const IceString &RegName, IceString *SplitRegClass,
                         IceString *SplitRegName) {
  constexpr const char Separator[] = ":";
  constexpr size_t SeparatorWidth = llvm::array_lengthof(Separator) - 1;
  size_t Pos = RegName.find(Separator);
  if (Pos == std::string::npos) {
    *SplitRegClass = "";
    *SplitRegName = RegName;
  } else {
    *SplitRegClass = RegName.substr(0, Pos);
    *SplitRegName = RegName.substr(Pos + SeparatorWidth);
  }
}

} // end of anonymous namespace

void TargetLowering::filterTypeToRegisterSet(
    GlobalContext *Ctx, int32_t NumRegs,
    llvm::SmallBitVector TypeToRegisterSet[], size_t TypeToRegisterSetSize,
    std::function<IceString(int32_t)> getRegName,
    std::function<IceString(RegClass)> getRegClassName) {
  std::vector<llvm::SmallBitVector> UseSet(TypeToRegisterSetSize,
                                           llvm::SmallBitVector(NumRegs));
  std::vector<llvm::SmallBitVector> ExcludeSet(TypeToRegisterSetSize,
                                               llvm::SmallBitVector(NumRegs));

  std::unordered_map<IceString, int32_t> RegNameToIndex;
  for (int32_t RegIndex = 0; RegIndex < NumRegs; ++RegIndex)
    RegNameToIndex[getRegName(RegIndex)] = RegIndex;

  ClFlags::StringVector BadRegNames;

  // The processRegList function iterates across the RegNames vector.  Each
  // entry in the vector is a string of the form "<reg>" or "<class>:<reg>".
  // The register class and register number are computed, and the corresponding
  // bit is set in RegSet[][].  If "<class>:" is missing, then the bit is set
  // for all classes.
  auto processRegList = [&](const ClFlags::StringVector &RegNames,
                            std::vector<llvm::SmallBitVector> &RegSet) {
    for (const IceString &RegClassAndName : RegNames) {
      IceString RClass;
      IceString RName;
      splitToClassAndName(RegClassAndName, &RClass, &RName);
      if (!RegNameToIndex.count(RName)) {
        BadRegNames.push_back(RName);
        continue;
      }
      const int32_t RegIndex = RegNameToIndex.at(RName);
      for (SizeT TypeIndex = 0; TypeIndex < TypeToRegisterSetSize;
           ++TypeIndex) {
        if (RClass.empty() ||
            RClass == getRegClassName(static_cast<RegClass>(TypeIndex))) {
          RegSet[TypeIndex][RegIndex] = TypeToRegisterSet[TypeIndex][RegIndex];
        }
      }
    }
  };

  processRegList(Ctx->getFlags().getUseRestrictedRegisters(), UseSet);
  processRegList(Ctx->getFlags().getExcludedRegisters(), ExcludeSet);

  if (!BadRegNames.empty()) {
    std::string Buffer;
    llvm::raw_string_ostream StrBuf(Buffer);
    StrBuf << "Unrecognized use/exclude registers:";
    for (const auto &RegName : BadRegNames)
      StrBuf << " " << RegName;
    llvm::report_fatal_error(StrBuf.str());
  }

  // Apply filters.
  for (size_t TypeIndex = 0; TypeIndex < TypeToRegisterSetSize; ++TypeIndex) {
    llvm::SmallBitVector *TypeBitSet = &TypeToRegisterSet[TypeIndex];
    llvm::SmallBitVector *UseBitSet = &UseSet[TypeIndex];
    llvm::SmallBitVector *ExcludeBitSet = &ExcludeSet[TypeIndex];
    if (UseBitSet->any())
      *TypeBitSet = *UseBitSet;
    (*TypeBitSet).reset(*ExcludeBitSet);
  }

  // Display filtered register sets, if requested.
  if (BuildDefs::dump() && NumRegs &&
      (Ctx->getFlags().getVerbose() & IceV_AvailableRegs)) {
    Ostream &Str = Ctx->getStrDump();
    const IceString Indent = "  ";
    const IceString IndentTwice = Indent + Indent;
    Str << "Registers available for register allocation:\n";
    for (size_t TypeIndex = 0; TypeIndex < TypeToRegisterSetSize; ++TypeIndex) {
      Str << Indent << getRegClassName(static_cast<RegClass>(TypeIndex))
          << ":\n";
      printRegisterSet(Str, TypeToRegisterSet[TypeIndex], getRegName,
                       IndentTwice);
    }
    Str << "\n";
  }
}

std::unique_ptr<TargetLowering>
TargetLowering::createLowering(TargetArch Target, Cfg *Func) {
  switch (Target) {
  default:
    llvm::report_fatal_error("Unsupported target");
#define SUBZERO_TARGET(X)                                                      \
  case Target_##X:                                                             \
    return ::X::createTargetLowering(Func);
#include "llvm/Config/SZTargets.def"
#undef SUBZERO_TARGET
  }
}

void TargetLowering::staticInit(GlobalContext *Ctx) {
  const TargetArch Target = Ctx->getFlags().getTargetArch();
  // Call the specified target's static initializer.
  switch (Target) {
  default:
    llvm::report_fatal_error("Unsupported target");
#define SUBZERO_TARGET(X)                                                      \
  case Target_##X: {                                                           \
    static bool InitGuard##X = false;                                          \
    if (InitGuard##X) {                                                        \
      return;                                                                  \
    }                                                                          \
    InitGuard##X = true;                                                       \
    ::X::staticInit(Ctx);                                                      \
  } break;
#include "llvm/Config/SZTargets.def"
#undef SUBZERO_TARGET
  }
}

TargetLowering::SandboxType
TargetLowering::determineSandboxTypeFromFlags(const ClFlags &Flags) {
  assert(!Flags.getUseSandboxing() || !Flags.getUseNonsfi());
  if (Flags.getUseNonsfi()) {
    return TargetLowering::ST_Nonsfi;
  }
  if (Flags.getUseSandboxing()) {
    return TargetLowering::ST_NaCl;
  }
  return TargetLowering::ST_None;
}

TargetLowering::TargetLowering(Cfg *Func)
    : Func(Func), Ctx(Func->getContext()),
      SandboxingType(determineSandboxTypeFromFlags(Ctx->getFlags())) {}

TargetLowering::AutoBundle::AutoBundle(TargetLowering *Target,
                                       InstBundleLock::Option Option)
    : Target(Target),
      NeedSandboxing(Target->Ctx->getFlags().getUseSandboxing()) {
  assert(!Target->AutoBundling);
  Target->AutoBundling = true;
  if (NeedSandboxing) {
    Target->_bundle_lock(Option);
  }
}

TargetLowering::AutoBundle::~AutoBundle() {
  assert(Target->AutoBundling);
  Target->AutoBundling = false;
  if (NeedSandboxing) {
    Target->_bundle_unlock();
  }
}

void TargetLowering::genTargetHelperCalls() {
  for (CfgNode *Node : Func->getNodes()) {
    Context.init(Node);
    while (!Context.atEnd()) {
      PostIncrLoweringContext _(Context);
      genTargetHelperCallFor(Context.getCur());
    }
  }
}

void TargetLowering::doAddressOpt() {
  if (llvm::isa<InstLoad>(*Context.getCur()))
    doAddressOptLoad();
  else if (llvm::isa<InstStore>(*Context.getCur()))
    doAddressOptStore();
  Context.advanceCur();
  Context.advanceNext();
}

void TargetLowering::doNopInsertion(RandomNumberGenerator &RNG) {
  Inst *I = Context.getCur();
  bool ShouldSkip = llvm::isa<InstFakeUse>(I) || llvm::isa<InstFakeDef>(I) ||
                    llvm::isa<InstFakeKill>(I) || I->isRedundantAssign() ||
                    I->isDeleted();
  if (!ShouldSkip) {
    int Probability = Ctx->getFlags().getNopProbabilityAsPercentage();
    for (int I = 0; I < Ctx->getFlags().getMaxNopsPerInstruction(); ++I) {
      randomlyInsertNop(Probability / 100.0, RNG);
    }
  }
}

// Lowers a single instruction according to the information in Context, by
// checking the Context.Cur instruction kind and calling the appropriate
// lowering method. The lowering method should insert target instructions at
// the Cur.Next insertion point, and should not delete the Context.Cur
// instruction or advance Context.Cur.
//
// The lowering method may look ahead in the instruction stream as desired, and
// lower additional instructions in conjunction with the current one, for
// example fusing a compare and branch. If it does, it should advance
// Context.Cur to point to the next non-deleted instruction to process, and it
// should delete any additional instructions it consumes.
void TargetLowering::lower() {
  assert(!Context.atEnd());
  Inst *Inst = Context.getCur();
  Inst->deleteIfDead();
  if (!Inst->isDeleted() && !llvm::isa<InstFakeDef>(Inst) &&
      !llvm::isa<InstFakeUse>(Inst)) {
    // Mark the current instruction as deleted before lowering, otherwise the
    // Dest variable will likely get marked as non-SSA. See
    // Variable::setDefinition(). However, just pass-through FakeDef and
    // FakeUse instructions that might have been inserted prior to lowering.
    Inst->setDeleted();
    switch (Inst->getKind()) {
    case Inst::Alloca:
      lowerAlloca(llvm::cast<InstAlloca>(Inst));
      break;
    case Inst::Arithmetic:
      lowerArithmetic(llvm::cast<InstArithmetic>(Inst));
      break;
    case Inst::Assign:
      lowerAssign(llvm::cast<InstAssign>(Inst));
      break;
    case Inst::Br:
      lowerBr(llvm::cast<InstBr>(Inst));
      break;
    case Inst::Call:
      lowerCall(llvm::cast<InstCall>(Inst));
      break;
    case Inst::Cast:
      lowerCast(llvm::cast<InstCast>(Inst));
      break;
    case Inst::ExtractElement:
      lowerExtractElement(llvm::cast<InstExtractElement>(Inst));
      break;
    case Inst::Fcmp:
      lowerFcmp(llvm::cast<InstFcmp>(Inst));
      break;
    case Inst::Icmp:
      lowerIcmp(llvm::cast<InstIcmp>(Inst));
      break;
    case Inst::InsertElement:
      lowerInsertElement(llvm::cast<InstInsertElement>(Inst));
      break;
    case Inst::IntrinsicCall: {
      auto *Call = llvm::cast<InstIntrinsicCall>(Inst);
      if (Call->getIntrinsicInfo().ReturnsTwice)
        setCallsReturnsTwice(true);
      lowerIntrinsicCall(Call);
      break;
    }
    case Inst::Load:
      lowerLoad(llvm::cast<InstLoad>(Inst));
      break;
    case Inst::Phi:
      lowerPhi(llvm::cast<InstPhi>(Inst));
      break;
    case Inst::Ret:
      lowerRet(llvm::cast<InstRet>(Inst));
      break;
    case Inst::Select:
      lowerSelect(llvm::cast<InstSelect>(Inst));
      break;
    case Inst::Store:
      lowerStore(llvm::cast<InstStore>(Inst));
      break;
    case Inst::Switch:
      lowerSwitch(llvm::cast<InstSwitch>(Inst));
      break;
    case Inst::Unreachable:
      lowerUnreachable(llvm::cast<InstUnreachable>(Inst));
      break;
    default:
      lowerOther(Inst);
      break;
    }

    postLower();
  }

  Context.advanceCur();
  Context.advanceNext();
}

void TargetLowering::lowerInst(CfgNode *Node, InstList::iterator Next,
                               InstHighLevel *Instr) {
  // TODO(stichnot): Consider modifying the design/implementation to avoid
  // multiple init() calls when using lowerInst() to lower several instructions
  // in the same node.
  Context.init(Node);
  Context.setNext(Next);
  Context.insert(Instr);
  --Next;
  assert(&*Next == Instr);
  Context.setCur(Next);
  lower();
}

void TargetLowering::lowerOther(const Inst *Instr) {
  (void)Instr;
  Func->setError("Can't lower unsupported instruction type");
}

// Drives register allocation, allowing all physical registers (except perhaps
// for the frame pointer) to be allocated. This set of registers could
// potentially be parameterized if we want to restrict registers e.g. for
// performance testing.
void TargetLowering::regAlloc(RegAllocKind Kind) {
  TimerMarker T(TimerStack::TT_regAlloc, Func);
  LinearScan LinearScan(Func);
  RegSetMask RegInclude = RegSet_None;
  RegSetMask RegExclude = RegSet_None;
  RegInclude |= RegSet_CallerSave;
  RegInclude |= RegSet_CalleeSave;
  if (hasFramePointer())
    RegExclude |= RegSet_FramePointer;
  llvm::SmallBitVector RegMask = getRegisterSet(RegInclude, RegExclude);
  bool Repeat = (Kind == RAK_Global && Ctx->getFlags().shouldRepeatRegAlloc());
  do {
    LinearScan.init(Kind);
    LinearScan.scan(RegMask, Ctx->getFlags().shouldRandomizeRegAlloc());
    if (!LinearScan.hasEvictions())
      Repeat = false;
    Kind = RAK_SecondChance;
  } while (Repeat);
  // TODO(stichnot): Run the register allocator one more time to do stack slot
  // coalescing.  The idea would be to initialize the Unhandled list with the
  // set of Variables that have no register and a non-empty live range, and
  // model an infinite number of registers.  Maybe use the register aliasing
  // mechanism to get better packing of narrower slots.
}

void TargetLowering::markRedefinitions() {
  // Find (non-SSA) instructions where the Dest variable appears in some source
  // operand, and set the IsDestRedefined flag to keep liveness analysis
  // consistent.
  for (auto Inst = Context.getCur(), E = Context.getNext(); Inst != E; ++Inst) {
    if (Inst->isDeleted())
      continue;
    Variable *Dest = Inst->getDest();
    if (Dest == nullptr)
      continue;
    FOREACH_VAR_IN_INST(Var, *Inst) {
      if (Var == Dest) {
        Inst->setDestRedefined();
        break;
      }
    }
  }
}

void TargetLowering::addFakeDefUses(const Inst *Instr) {
  FOREACH_VAR_IN_INST(Var, *Instr) {
    if (auto *Var64 = llvm::dyn_cast<Variable64On32>(Var)) {
      Context.insert<InstFakeUse>(Var64->getLo());
      Context.insert<InstFakeUse>(Var64->getHi());
    } else {
      Context.insert<InstFakeUse>(Var);
    }
  }
  Variable *Dest = Instr->getDest();
  if (Dest == nullptr)
    return;
  if (auto *Var64 = llvm::dyn_cast<Variable64On32>(Dest)) {
    Context.insert<InstFakeDef>(Var64->getLo());
    Context.insert<InstFakeDef>(Var64->getHi());
  } else {
    Context.insert<InstFakeDef>(Dest);
  }
}

void TargetLowering::sortVarsByAlignment(VarList &Dest,
                                         const VarList &Source) const {
  Dest = Source;
  // Instead of std::sort, we could do a bucket sort with log2(alignment) as
  // the buckets, if performance is an issue.
  std::sort(Dest.begin(), Dest.end(),
            [this](const Variable *V1, const Variable *V2) {
              return typeWidthInBytesOnStack(V1->getType()) >
                     typeWidthInBytesOnStack(V2->getType());
            });
}

void TargetLowering::getVarStackSlotParams(
    VarList &SortedSpilledVariables, llvm::SmallBitVector &RegsUsed,
    size_t *GlobalsSize, size_t *SpillAreaSizeBytes,
    uint32_t *SpillAreaAlignmentBytes, uint32_t *LocalsSlotsAlignmentBytes,
    std::function<bool(Variable *)> TargetVarHook) {
  const VariablesMetadata *VMetadata = Func->getVMetadata();
  llvm::BitVector IsVarReferenced(Func->getNumVariables());
  for (CfgNode *Node : Func->getNodes()) {
    for (Inst &Inst : Node->getInsts()) {
      if (Inst.isDeleted())
        continue;
      if (const Variable *Var = Inst.getDest())
        IsVarReferenced[Var->getIndex()] = true;
      FOREACH_VAR_IN_INST(Var, Inst) {
        IsVarReferenced[Var->getIndex()] = true;
      }
    }
  }

  // If SimpleCoalescing is false, each variable without a register gets its
  // own unique stack slot, which leads to large stack frames. If
  // SimpleCoalescing is true, then each "global" variable without a register
  // gets its own slot, but "local" variable slots are reused across basic
  // blocks. E.g., if A and B are local to block 1 and C is local to block 2,
  // then C may share a slot with A or B.
  //
  // We cannot coalesce stack slots if this function calls a "returns twice"
  // function. In that case, basic blocks may be revisited, and variables local
  // to those basic blocks are actually live until after the called function
  // returns a second time.
  const bool SimpleCoalescing = !callsReturnsTwice();

  std::vector<size_t> LocalsSize(Func->getNumNodes());
  const VarList &Variables = Func->getVariables();
  VarList SpilledVariables;
  for (Variable *Var : Variables) {
    if (Var->hasReg()) {
      // Don't consider a rematerializable variable to be an actual register use
      // (specifically of the frame pointer).  Otherwise, the prolog may decide
      // to save the frame pointer twice - once because of the explicit need for
      // a frame pointer, and once because of an active use of a callee-save
      // register.
      if (!Var->isRematerializable())
        RegsUsed[Var->getRegNum()] = true;
      continue;
    }
    // An argument either does not need a stack slot (if passed in a register)
    // or already has one (if passed on the stack).
    if (Var->getIsArg())
      continue;
    // An unreferenced variable doesn't need a stack slot.
    if (!IsVarReferenced[Var->getIndex()])
      continue;
    // Check a target-specific variable (it may end up sharing stack slots) and
    // not need accounting here.
    if (TargetVarHook(Var))
      continue;
    SpilledVariables.push_back(Var);
  }

  SortedSpilledVariables.reserve(SpilledVariables.size());
  sortVarsByAlignment(SortedSpilledVariables, SpilledVariables);

  for (Variable *Var : SortedSpilledVariables) {
    size_t Increment = typeWidthInBytesOnStack(Var->getType());
    // We have sorted by alignment, so the first variable we encounter that is
    // located in each area determines the max alignment for the area.
    if (!*SpillAreaAlignmentBytes)
      *SpillAreaAlignmentBytes = Increment;
    if (SimpleCoalescing && VMetadata->isTracked(Var)) {
      if (VMetadata->isMultiBlock(Var)) {
        *GlobalsSize += Increment;
      } else {
        SizeT NodeIndex = VMetadata->getLocalUseNode(Var)->getIndex();
        LocalsSize[NodeIndex] += Increment;
        if (LocalsSize[NodeIndex] > *SpillAreaSizeBytes)
          *SpillAreaSizeBytes = LocalsSize[NodeIndex];
        if (!*LocalsSlotsAlignmentBytes)
          *LocalsSlotsAlignmentBytes = Increment;
      }
    } else {
      *SpillAreaSizeBytes += Increment;
    }
  }
  // For testing legalization of large stack offsets on targets with limited
  // offset bits in instruction encodings, add some padding.
  *SpillAreaSizeBytes += Ctx->getFlags().getTestStackExtra();
}

void TargetLowering::alignStackSpillAreas(uint32_t SpillAreaStartOffset,
                                          uint32_t SpillAreaAlignmentBytes,
                                          size_t GlobalsSize,
                                          uint32_t LocalsSlotsAlignmentBytes,
                                          uint32_t *SpillAreaPaddingBytes,
                                          uint32_t *LocalsSlotsPaddingBytes) {
  if (SpillAreaAlignmentBytes) {
    uint32_t PaddingStart = SpillAreaStartOffset;
    uint32_t SpillAreaStart =
        Utils::applyAlignment(PaddingStart, SpillAreaAlignmentBytes);
    *SpillAreaPaddingBytes = SpillAreaStart - PaddingStart;
  }

  // If there are separate globals and locals areas, make sure the locals area
  // is aligned by padding the end of the globals area.
  if (LocalsSlotsAlignmentBytes) {
    uint32_t GlobalsAndSubsequentPaddingSize = GlobalsSize;
    GlobalsAndSubsequentPaddingSize =
        Utils::applyAlignment(GlobalsSize, LocalsSlotsAlignmentBytes);
    *LocalsSlotsPaddingBytes = GlobalsAndSubsequentPaddingSize - GlobalsSize;
  }
}

void TargetLowering::assignVarStackSlots(VarList &SortedSpilledVariables,
                                         size_t SpillAreaPaddingBytes,
                                         size_t SpillAreaSizeBytes,
                                         size_t GlobalsAndSubsequentPaddingSize,
                                         bool UsesFramePointer) {
  const VariablesMetadata *VMetadata = Func->getVMetadata();
  // For testing legalization of large stack offsets on targets with limited
  // offset bits in instruction encodings, add some padding. This assumes that
  // SpillAreaSizeBytes has accounted for the extra test padding. When
  // UseFramePointer is true, the offset depends on the padding, not just the
  // SpillAreaSizeBytes. On the other hand, when UseFramePointer is false, the
  // offsets depend on the gap between SpillAreaSizeBytes and
  // SpillAreaPaddingBytes, so we don't increment that.
  size_t TestPadding = Ctx->getFlags().getTestStackExtra();
  if (UsesFramePointer)
    SpillAreaPaddingBytes += TestPadding;
  size_t GlobalsSpaceUsed = SpillAreaPaddingBytes;
  size_t NextStackOffset = SpillAreaPaddingBytes;
  std::vector<size_t> LocalsSize(Func->getNumNodes());
  const bool SimpleCoalescing = !callsReturnsTwice();

  for (Variable *Var : SortedSpilledVariables) {
    size_t Increment = typeWidthInBytesOnStack(Var->getType());
    if (SimpleCoalescing && VMetadata->isTracked(Var)) {
      if (VMetadata->isMultiBlock(Var)) {
        GlobalsSpaceUsed += Increment;
        NextStackOffset = GlobalsSpaceUsed;
      } else {
        SizeT NodeIndex = VMetadata->getLocalUseNode(Var)->getIndex();
        LocalsSize[NodeIndex] += Increment;
        NextStackOffset = SpillAreaPaddingBytes +
                          GlobalsAndSubsequentPaddingSize +
                          LocalsSize[NodeIndex];
      }
    } else {
      NextStackOffset += Increment;
    }
    if (UsesFramePointer)
      Var->setStackOffset(-NextStackOffset);
    else
      Var->setStackOffset(SpillAreaSizeBytes - NextStackOffset);
  }
}

InstCall *TargetLowering::makeHelperCall(const IceString &Name, Variable *Dest,
                                         SizeT MaxSrcs) {
  constexpr bool HasTailCall = false;
  Constant *CallTarget = Ctx->getConstantExternSym(Name);
  InstCall *Call =
      InstCall::create(Func, MaxSrcs, Dest, CallTarget, HasTailCall);
  return Call;
}

bool TargetLowering::shouldOptimizeMemIntrins() {
  return Ctx->getFlags().getOptLevel() >= Opt_1 ||
         Ctx->getFlags().getForceMemIntrinOpt();
}

void TargetLowering::emitWithoutPrefix(const ConstantRelocatable *C,
                                       const char *Suffix) const {
  if (!BuildDefs::dump())
    return;
  Ostream &Str = Ctx->getStrEmit();
  if (C->getSuppressMangling())
    Str << C->getName();
  else
    Str << Ctx->mangleName(C->getName());
  Str << Suffix;
  RelocOffsetT Offset = C->getOffset();
  if (Offset) {
    if (Offset > 0)
      Str << "+";
    Str << Offset;
  }
}

std::unique_ptr<TargetDataLowering>
TargetDataLowering::createLowering(GlobalContext *Ctx) {
  TargetArch Target = Ctx->getFlags().getTargetArch();
  switch (Target) {
  default:
    llvm::report_fatal_error("Unsupported target");
#define SUBZERO_TARGET(X)                                                      \
  case Target_##X:                                                             \
    return ::X::createTargetDataLowering(Ctx);
#include "llvm/Config/SZTargets.def"
#undef SUBZERO_TARGET
  }
}

TargetDataLowering::~TargetDataLowering() = default;

namespace {

// dataSectionSuffix decides whether to use SectionSuffix or MangledVarName as
// data section suffix. Essentially, when using separate data sections for
// globals SectionSuffix is not necessary.
IceString dataSectionSuffix(const IceString &SectionSuffix,
                            const IceString &MangledVarName,
                            const bool DataSections) {
  if (SectionSuffix.empty() && !DataSections) {
    return "";
  }

  if (DataSections) {
    // With data sections we don't need to use the SectionSuffix.
    return "." + MangledVarName;
  }

  assert(!SectionSuffix.empty());
  return "." + SectionSuffix;
}

} // end of anonymous namespace

void TargetDataLowering::emitGlobal(const VariableDeclaration &Var,
                                    const IceString &SectionSuffix) {
  if (!BuildDefs::dump())
    return;

  // If external and not initialized, this must be a cross test. Don't generate
  // a declaration for such cases.
  const bool IsExternal =
      Var.isExternal() || Ctx->getFlags().getDisableInternal();
  if (IsExternal && !Var.hasInitializer())
    return;

  Ostream &Str = Ctx->getStrEmit();
  const bool HasNonzeroInitializer = Var.hasNonzeroInitializer();
  const bool IsConstant = Var.getIsConstant();
  const SizeT Size = Var.getNumBytes();
  const IceString MangledName = Var.mangleName(Ctx);

  Str << "\t.type\t" << MangledName << ",%object\n";

  const bool UseDataSections = Ctx->getFlags().getDataSections();
  const bool UseNonsfi = Ctx->getFlags().getUseNonsfi();
  const IceString Suffix =
      dataSectionSuffix(SectionSuffix, MangledName, UseDataSections);
  if (IsConstant && UseNonsfi)
    Str << "\t.section\t.data.rel.ro" << Suffix << ",\"aw\",%progbits\n";
  else if (IsConstant)
    Str << "\t.section\t.rodata" << Suffix << ",\"a\",%progbits\n";
  else if (HasNonzeroInitializer)
    Str << "\t.section\t.data" << Suffix << ",\"aw\",%progbits\n";
  else
    Str << "\t.section\t.bss" << Suffix << ",\"aw\",%nobits\n";

  if (IsExternal)
    Str << "\t.globl\t" << MangledName << "\n";

  const uint32_t Align = Var.getAlignment();
  if (Align > 1) {
    assert(llvm::isPowerOf2_32(Align));
    // Use the .p2align directive, since the .align N directive can either
    // interpret N as bytes, or power of 2 bytes, depending on the target.
    Str << "\t.p2align\t" << llvm::Log2_32(Align) << "\n";
  }

  Str << MangledName << ":\n";

  if (HasNonzeroInitializer) {
    for (const std::unique_ptr<VariableDeclaration::Initializer> &Init :
         Var.getInitializers()) {
      switch (Init->getKind()) {
      case VariableDeclaration::Initializer::DataInitializerKind: {
        const auto &Data =
            llvm::cast<VariableDeclaration::DataInitializer>(Init.get())
                ->getContents();
        for (SizeT i = 0; i < Init->getNumBytes(); ++i) {
          Str << "\t.byte\t" << (((unsigned)Data[i]) & 0xff) << "\n";
        }
        break;
      }
      case VariableDeclaration::Initializer::ZeroInitializerKind:
        Str << "\t.zero\t" << Init->getNumBytes() << "\n";
        break;
      case VariableDeclaration::Initializer::RelocInitializerKind: {
        const auto *Reloc =
            llvm::cast<VariableDeclaration::RelocInitializer>(Init.get());
        Str << "\t" << getEmit32Directive() << "\t";
        Str << Reloc->getDeclaration()->mangleName(Ctx);
        if (RelocOffsetT Offset = Reloc->getOffset()) {
          if (Offset >= 0 || (Offset == INT32_MIN))
            Str << " + " << Offset;
          else
            Str << " - " << -Offset;
        }
        Str << "\n";
        break;
      }
      }
    }
  } else {
    // NOTE: for non-constant zero initializers, this is BSS (no bits), so an
    // ELF writer would not write to the file, and only track virtual offsets,
    // but the .s writer still needs this .zero and cannot simply use the .size
    // to advance offsets.
    Str << "\t.zero\t" << Size << "\n";
  }

  Str << "\t.size\t" << MangledName << ", " << Size << "\n";
}

std::unique_ptr<TargetHeaderLowering>
TargetHeaderLowering::createLowering(GlobalContext *Ctx) {
  TargetArch Target = Ctx->getFlags().getTargetArch();
  switch (Target) {
  default:
    llvm::report_fatal_error("Unsupported target");
#define SUBZERO_TARGET(X)                                                      \
  case Target_##X:                                                             \
    return ::X::createTargetHeaderLowering(Ctx);
#include "llvm/Config/SZTargets.def"
#undef SUBZERO_TARGET
  }
}

TargetHeaderLowering::~TargetHeaderLowering() = default;

} // end of namespace Ice
