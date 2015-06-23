//===- subzero/src/IceTargetLowering.cpp - Basic lowering implementation --===//
//
//                        The Subzero Code Generator
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements the skeleton of the TargetLowering class,
// specifically invoking the appropriate lowering method for a given
// instruction kind and driving global register allocation.  It also
// implements the non-deleted instruction iteration in
// LoweringContext.
//
//===----------------------------------------------------------------------===//

#include "IceAssemblerARM32.h"
#include "IceAssemblerX8632.h"
#include "IceAssemblerX8664.h"
#include "assembler_mips32.h"
#include "IceCfg.h" // setError()
#include "IceCfgNode.h"
#include "IceGlobalInits.h"
#include "IceOperand.h"
#include "IceRegAlloc.h"
#include "IceTargetLowering.h"
#include "IceTargetLoweringARM32.h"
#include "IceTargetLoweringMIPS32.h"
#include "IceTargetLoweringX8632.h"
#include "IceTargetLoweringX8664.h"

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

TargetLowering *TargetLowering::createLowering(TargetArch Target, Cfg *Func) {
#define SUBZERO_TARGET(X)                                                      \
  if (Target == Target_##X)                                                    \
    return Target##X::create(Func);
#include "llvm/Config/SZTargets.def"

  Func->setError("Unsupported target");
  return nullptr;
}

TargetLowering::TargetLowering(Cfg *Func)
    : Func(Func), Ctx(Func->getContext()), Context() {}

std::unique_ptr<Assembler> TargetLowering::createAssembler(TargetArch Target,
                                                           Cfg *Func) {
#define SUBZERO_TARGET(X)                                                      \
  if (Target == Target_##X)                                                    \
    return std::unique_ptr<Assembler>(new X::Assembler##X());
#include "llvm/Config/SZTargets.def"

  Func->setError("Unsupported target assembler");
  return nullptr;
}

void TargetLowering::doAddressOpt() {
  if (llvm::isa<InstLoad>(*Context.getCur()))
    doAddressOptLoad();
  else if (llvm::isa<InstStore>(*Context.getCur()))
    doAddressOptStore();
  Context.advanceCur();
  Context.advanceNext();
}

void TargetLowering::doNopInsertion() {
  Inst *I = Context.getCur();
  bool ShouldSkip = llvm::isa<InstFakeUse>(I) || llvm::isa<InstFakeDef>(I) ||
                    llvm::isa<InstFakeKill>(I) || I->isRedundantAssign() ||
                    I->isDeleted();
  if (!ShouldSkip) {
    int Probability = Ctx->getFlags().getNopProbabilityAsPercentage();
    for (int I = 0; I < Ctx->getFlags().getMaxNopsPerInstruction(); ++I) {
      randomlyInsertNop(Probability / 100.0);
    }
  }
}

// Lowers a single instruction according to the information in
// Context, by checking the Context.Cur instruction kind and calling
// the appropriate lowering method.  The lowering method should insert
// target instructions at the Cur.Next insertion point, and should not
// delete the Context.Cur instruction or advance Context.Cur.
//
// The lowering method may look ahead in the instruction stream as
// desired, and lower additional instructions in conjunction with the
// current one, for example fusing a compare and branch.  If it does,
// it should advance Context.Cur to point to the next non-deleted
// instruction to process, and it should delete any additional
// instructions it consumes.
void TargetLowering::lower() {
  assert(!Context.atEnd());
  Inst *Inst = Context.getCur();
  Inst->deleteIfDead();
  if (!Inst->isDeleted() && !llvm::isa<InstFakeDef>(Inst) &&
      !llvm::isa<InstFakeUse>(Inst)) {
    // Mark the current instruction as deleted before lowering,
    // otherwise the Dest variable will likely get marked as non-SSA.
    // See Variable::setDefinition().  However, just pass-through
    // FakeDef and FakeUse instructions that might have been inserted
    // prior to lowering.
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
      InstIntrinsicCall *Call = llvm::cast<InstIntrinsicCall>(Inst);
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

void TargetLowering::lowerOther(const Inst *Instr) {
  (void)Instr;
  Func->setError("Can't lower unsupported instruction type");
}

// Drives register allocation, allowing all physical registers (except
// perhaps for the frame pointer) to be allocated.  This set of
// registers could potentially be parameterized if we want to restrict
// registers e.g. for performance testing.
void TargetLowering::regAlloc(RegAllocKind Kind) {
  TimerMarker T(TimerStack::TT_regAlloc, Func);
  LinearScan LinearScan(Func);
  RegSetMask RegInclude = RegSet_None;
  RegSetMask RegExclude = RegSet_None;
  RegInclude |= RegSet_CallerSave;
  RegInclude |= RegSet_CalleeSave;
  if (hasFramePointer())
    RegExclude |= RegSet_FramePointer;
  LinearScan.init(Kind);
  llvm::SmallBitVector RegMask = getRegisterSet(RegInclude, RegExclude);
  LinearScan.scan(RegMask, Ctx->getFlags().shouldRandomizeRegAlloc());
}

void TargetLowering::inferTwoAddress() {
  // Find two-address non-SSA instructions where Dest==Src0, and set
  // the DestNonKillable flag to keep liveness analysis consistent.
  for (auto Inst = Context.getCur(), E = Context.getNext(); Inst != E; ++Inst) {
    if (Inst->isDeleted())
      continue;
    if (Variable *Dest = Inst->getDest()) {
      // TODO(stichnot): We may need to consider all source
      // operands, not just the first one, if using 3-address
      // instructions.
      if (Inst->getSrcSize() > 0 && Inst->getSrc(0) == Dest)
        Inst->setDestNonKillable();
    }
  }
}

void TargetLowering::sortVarsByAlignment(VarList &Dest,
                                         const VarList &Source) const {
  Dest = Source;
  // Instead of std::sort, we could do a bucket sort with log2(alignment)
  // as the buckets, if performance is an issue.
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
      for (SizeT I = 0; I < Inst.getSrcSize(); ++I) {
        Operand *Src = Inst.getSrc(I);
        SizeT NumVars = Src->getNumVars();
        for (SizeT J = 0; J < NumVars; ++J) {
          const Variable *Var = Src->getVar(J);
          IsVarReferenced[Var->getIndex()] = true;
        }
      }
    }
  }

  // If SimpleCoalescing is false, each variable without a register
  // gets its own unique stack slot, which leads to large stack
  // frames.  If SimpleCoalescing is true, then each "global" variable
  // without a register gets its own slot, but "local" variable slots
  // are reused across basic blocks.  E.g., if A and B are local to
  // block 1 and C is local to block 2, then C may share a slot with A or B.
  //
  // We cannot coalesce stack slots if this function calls a "returns twice"
  // function. In that case, basic blocks may be revisited, and variables
  // local to those basic blocks are actually live until after the
  // called function returns a second time.
  const bool SimpleCoalescing = !callsReturnsTwice();

  std::vector<size_t> LocalsSize(Func->getNumNodes());
  const VarList &Variables = Func->getVariables();
  VarList SpilledVariables;
  for (Variable *Var : Variables) {
    if (Var->hasReg()) {
      RegsUsed[Var->getRegNum()] = true;
      continue;
    }
    // An argument either does not need a stack slot (if passed in a
    // register) or already has one (if passed on the stack).
    if (Var->getIsArg())
      continue;
    // An unreferenced variable doesn't need a stack slot.
    if (!IsVarReferenced[Var->getIndex()])
      continue;
    // Check a target-specific variable (it may end up sharing stack slots)
    // and not need accounting here.
    if (TargetVarHook(Var))
      continue;
    SpilledVariables.push_back(Var);
  }

  SortedSpilledVariables.reserve(SpilledVariables.size());
  sortVarsByAlignment(SortedSpilledVariables, SpilledVariables);

  for (Variable *Var : SortedSpilledVariables) {
    size_t Increment = typeWidthInBytesOnStack(Var->getType());
    // We have sorted by alignment, so the first variable we encounter that
    // is located in each area determines the max alignment for the area.
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

  // If there are separate globals and locals areas, make sure the
  // locals area is aligned by padding the end of the globals area.
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
  const bool HasTailCall = false;
  Constant *CallTarget = Ctx->getConstantExternSym(Name);
  InstCall *Call =
      InstCall::create(Func, MaxSrcs, Dest, CallTarget, HasTailCall);
  return Call;
}

void TargetLowering::emitWithoutPrefix(const ConstantRelocatable *C) const {
  if (!ALLOW_DUMP)
    return;
  Ostream &Str = Ctx->getStrEmit();
  if (C->getSuppressMangling())
    Str << C->getName();
  else
    Str << Ctx->mangleName(C->getName());
  RelocOffsetT Offset = C->getOffset();
  if (Offset) {
    if (Offset > 0)
      Str << "+";
    Str << Offset;
  }
}

void TargetLowering::emit(const ConstantRelocatable *C) const {
  if (!ALLOW_DUMP)
    return;
  Ostream &Str = Ctx->getStrEmit();
  Str << getConstantPrefix();
  emitWithoutPrefix(C);
}

std::unique_ptr<TargetDataLowering>
TargetDataLowering::createLowering(GlobalContext *Ctx) {
  TargetArch Target = Ctx->getFlags().getTargetArch();
#define SUBZERO_TARGET(X)                                                      \
  if (Target == Target_##X)                                                    \
    return TargetData##X::create(Ctx);
#include "llvm/Config/SZTargets.def"

  llvm::report_fatal_error("Unsupported target data lowering");
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
  if (!ALLOW_DUMP)
    return;

  // If external and not initialized, this must be a cross test.
  // Don't generate a declaration for such cases.
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
  const IceString Suffix =
      dataSectionSuffix(SectionSuffix, MangledName, UseDataSections);
  if (IsConstant)
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
        const auto &Data = llvm::cast<VariableDeclaration::DataInitializer>(
                               Init.get())->getContents();
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
    // NOTE: for non-constant zero initializers, this is BSS (no bits),
    // so an ELF writer would not write to the file, and only track
    // virtual offsets, but the .s writer still needs this .zero and
    // cannot simply use the .size to advance offsets.
    Str << "\t.zero\t" << Size << "\n";
  }

  Str << "\t.size\t" << MangledName << ", " << Size << "\n";
}

std::unique_ptr<TargetHeaderLowering>
TargetHeaderLowering::createLowering(GlobalContext *Ctx) {
  TargetArch Target = Ctx->getFlags().getTargetArch();
#define SUBZERO_TARGET(X)                                                      \
  if (Target == Target_##X)                                                    \
    return TargetHeader##X::create(Ctx);
#include "llvm/Config/SZTargets.def"

  llvm::report_fatal_error("Unsupported target header lowering");
}

TargetHeaderLowering::~TargetHeaderLowering() = default;

} // end of namespace Ice
