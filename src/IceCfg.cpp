//===- subzero/src/IceCfg.cpp - Control flow graph implementation ---------===//
//
//                        The Subzero Code Generator
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief Implements the Cfg class.
///
//===----------------------------------------------------------------------===//

#include "IceCfg.h"

#include "IceAssembler.h"
#include "IceBitVector.h"
#include "IceCfgNode.h"
#include "IceClFlags.h"
#include "IceDefs.h"
#include "IceELFObjectWriter.h"
#include "IceGlobalInits.h"
#include "IceInst.h"
#include "IceInstVarIter.h"
#include "IceLiveness.h"
#include "IceLoopAnalyzer.h"
#include "IceOperand.h"
#include "IceTargetLowering.h"

namespace Ice {

Cfg::Cfg(GlobalContext *Ctx, uint32_t SequenceNumber)
    : Ctx(Ctx), SequenceNumber(SequenceNumber),
      VMask(Ctx->getFlags().getVerbose()), NextInstNumber(Inst::NumberInitial),
      Live(nullptr) {
  Allocator.reset(new ArenaAllocator());
  CfgLocalAllocatorScope _(this);
  Target =
      TargetLowering::createLowering(Ctx->getFlags().getTargetArch(), this);
  VMetadata.reset(new VariablesMetadata(this));
  TargetAssembler = Target->createAssembler();

  if (Ctx->getFlags().getRandomizeAndPoolImmediatesOption() == RPI_Randomize) {
    // If -randomize-pool-immediates=randomize, create a random number
    // generator to generate a cookie for constant blinding.
    RandomNumberGenerator RNG(Ctx->getFlags().getRandomSeed(),
                              RPE_ConstantBlinding, this->SequenceNumber);
    ConstantBlindingCookie =
        (uint32_t)RNG.next((uint64_t)std::numeric_limits<uint32_t>::max() + 1);
  }
}

Cfg::~Cfg() { assert(CfgAllocatorTraits::current() == nullptr); }

/// Create a string like "foo(i=123:b=9)" indicating the function name, number
/// of high-level instructions, and number of basic blocks.  This string is only
/// used for dumping and other diagnostics, and the idea is that given a set of
/// functions to debug a problem on, it's easy to find the smallest or simplest
/// function to attack.  Note that the counts may change somewhat depending on
/// what point it is called during the translation passes.
IceString Cfg::getFunctionNameAndSize() const {
  if (!BuildDefs::dump())
    return getFunctionName();
  SizeT NodeCount = 0;
  SizeT InstCount = 0;
  for (CfgNode *Node : getNodes()) {
    ++NodeCount;
    // Note: deleted instructions are *not* ignored.
    InstCount += Node->getPhis().size();
    for (Inst &I : Node->getInsts()) {
      if (!llvm::isa<InstTarget>(&I))
        ++InstCount;
    }
  }
  return getFunctionName() + "(i=" + std::to_string(InstCount) + ":b=" +
         std::to_string(NodeCount) + ")";
}

void Cfg::setError(const IceString &Message) {
  HasError = true;
  ErrorMessage = Message;
}

CfgNode *Cfg::makeNode() {
  SizeT LabelIndex = Nodes.size();
  auto *Node = CfgNode::create(this, LabelIndex);
  Nodes.push_back(Node);
  return Node;
}

void Cfg::swapNodes(NodeList &NewNodes) {
  assert(Nodes.size() == NewNodes.size());
  Nodes.swap(NewNodes);
  for (SizeT I = 0, NumNodes = getNumNodes(); I < NumNodes; ++I)
    Nodes[I]->resetIndex(I);
}

template <> Variable *Cfg::makeVariable<Variable>(Type Ty) {
  SizeT Index = Variables.size();
  Variable *Var = Target->shouldSplitToVariable64On32(Ty)
                      ? Variable64On32::create(this, Ty, Index)
                      : Variable::create(this, Ty, Index);
  Variables.push_back(Var);
  return Var;
}

void Cfg::addArg(Variable *Arg) {
  Arg->setIsArg();
  Args.push_back(Arg);
}

void Cfg::addImplicitArg(Variable *Arg) {
  Arg->setIsImplicitArg();
  ImplicitArgs.push_back(Arg);
}

// Returns whether the stack frame layout has been computed yet. This is used
// for dumping the stack frame location of Variables.
bool Cfg::hasComputedFrame() const { return getTarget()->hasComputedFrame(); }

namespace {
constexpr char BlockNameGlobalPrefix[] = ".L$profiler$block_name$";
constexpr char BlockStatsGlobalPrefix[] = ".L$profiler$block_info$";

VariableDeclaration *nodeNameDeclaration(GlobalContext *Ctx,
                                         const IceString &NodeAsmName) {
  auto *Var = VariableDeclaration::create(Ctx);
  Var->setName(BlockNameGlobalPrefix + NodeAsmName);
  Var->setIsConstant(true);
  Var->addInitializer(VariableDeclaration::DataInitializer::create(
      NodeAsmName.data(), NodeAsmName.size() + 1));
  const SizeT Int64ByteSize = typeWidthInBytes(IceType_i64);
  Var->setAlignment(Int64ByteSize); // Wasteful, 32-bit could use 4 bytes.
  return Var;
}

VariableDeclaration *
blockProfilingInfoDeclaration(GlobalContext *Ctx, const IceString &NodeAsmName,
                              VariableDeclaration *NodeNameDeclaration) {
  auto *Var = VariableDeclaration::create(Ctx);
  Var->setName(BlockStatsGlobalPrefix + NodeAsmName);
  const SizeT Int64ByteSize = typeWidthInBytes(IceType_i64);
  Var->addInitializer(
      VariableDeclaration::ZeroInitializer::create(Int64ByteSize));

  const RelocOffsetT NodeNameDeclarationOffset = 0;
  Var->addInitializer(VariableDeclaration::RelocInitializer::create(
      NodeNameDeclaration,
      {RelocOffset::create(Ctx, NodeNameDeclarationOffset)}));
  Var->setAlignment(Int64ByteSize);
  return Var;
}
} // end of anonymous namespace

void Cfg::profileBlocks() {
  if (GlobalInits == nullptr)
    GlobalInits.reset(new VariableDeclarationList());

  for (CfgNode *Node : Nodes) {
    IceString NodeAsmName = Node->getAsmName();
    GlobalInits->push_back(nodeNameDeclaration(Ctx, NodeAsmName));
    GlobalInits->push_back(
        blockProfilingInfoDeclaration(Ctx, NodeAsmName, GlobalInits->back()));
    Node->profileExecutionCount(GlobalInits->back());
  }
}

bool Cfg::isProfileGlobal(const VariableDeclaration &Var) {
  return Var.getName().find(BlockStatsGlobalPrefix) == 0;
}

void Cfg::addCallToProfileSummary() {
  // The call(s) to __Sz_profile_summary are added by the profiler in functions
  // that cause the program to exit. This function is defined in
  // runtime/szrt_profiler.c.
  Constant *ProfileSummarySym =
      Ctx->getConstantExternSym("__Sz_profile_summary");
  constexpr SizeT NumArgs = 0;
  constexpr Variable *Void = nullptr;
  constexpr bool HasTailCall = false;
  auto *Call =
      InstCall::create(this, NumArgs, Void, ProfileSummarySym, HasTailCall);
  getEntryNode()->getInsts().push_front(Call);
}

void Cfg::translate() {
  if (hasError())
    return;
  // FunctionTimer conditionally pushes/pops a TimerMarker if TimeEachFunction
  // is enabled.
  std::unique_ptr<TimerMarker> FunctionTimer;
  if (BuildDefs::dump()) {
    const IceString &TimingFocusOn =
        getContext()->getFlags().getTimingFocusOn();
    const IceString &Name = getFunctionName();
    if (TimingFocusOn == "*" || TimingFocusOn == Name) {
      setFocusedTiming();
      getContext()->resetTimer(GlobalContext::TSK_Default);
      getContext()->setTimerName(GlobalContext::TSK_Default, Name);
    }
    if (getContext()->getFlags().getTimeEachFunction())
      FunctionTimer.reset(new TimerMarker(
          getContext()->getTimerID(GlobalContext::TSK_Funcs, Name),
          getContext(), GlobalContext::TSK_Funcs));
    if (isVerbose(IceV_Status)) {
      getContext()->getStrDump() << ">>>Translating "
                                 << getFunctionNameAndSize() << "\n";
    }
  }
  TimerMarker T(TimerStack::TT_translate, this);

  dump("Initial CFG");

  if (getContext()->getFlags().getEnableBlockProfile()) {
    profileBlocks();
    // TODO(jpp): this is fragile, at best. Figure out a better way of
    // detecting exit functions.
    if (GlobalContext::matchSymbolName(getFunctionName(), "exit")) {
      addCallToProfileSummary();
    }
    dump("Profiled CFG");
  }

  // Create the Hi and Lo variables where a split was needed
  for (Variable *Var : Variables)
    if (auto *Var64On32 = llvm::dyn_cast<Variable64On32>(Var))
      Var64On32->initHiLo(this);

  // The set of translation passes and their order are determined by the
  // target.
  getTarget()->translate();

  dump("Final output");
  if (getFocusedTiming())
    getContext()->dumpTimers();
}

void Cfg::computeInOutEdges() {
  // Compute the out-edges.
  for (CfgNode *Node : Nodes)
    Node->computeSuccessors();

  // Prune any unreachable nodes before computing in-edges.
  SizeT NumNodes = getNumNodes();
  BitVector Reachable(NumNodes);
  BitVector Pending(NumNodes);
  Pending.set(getEntryNode()->getIndex());
  while (true) {
    int Index = Pending.find_first();
    if (Index == -1)
      break;
    Pending.reset(Index);
    Reachable.set(Index);
    CfgNode *Node = Nodes[Index];
    assert(Node->getIndex() == (SizeT)Index);
    for (CfgNode *Succ : Node->getOutEdges()) {
      SizeT SuccIndex = Succ->getIndex();
      if (!Reachable.test(SuccIndex))
        Pending.set(SuccIndex);
    }
  }
  SizeT Dest = 0;
  for (SizeT Source = 0; Source < NumNodes; ++Source) {
    if (Reachable.test(Source)) {
      Nodes[Dest] = Nodes[Source];
      Nodes[Dest]->resetIndex(Dest);
      // Compute the in-edges.
      Nodes[Dest]->computePredecessors();
      ++Dest;
    }
  }
  Nodes.resize(Dest);

  TimerMarker T(TimerStack::TT_phiValidation, this);
  for (CfgNode *Node : Nodes)
    Node->validatePhis();
}

void Cfg::renumberInstructions() {
  TimerMarker T(TimerStack::TT_renumberInstructions, this);
  NextInstNumber = Inst::NumberInitial;
  for (CfgNode *Node : Nodes)
    Node->renumberInstructions();
  // Make sure the entry node is the first node and therefore got the lowest
  // instruction numbers, to facilitate live range computation of function
  // arguments.  We want to model function arguments as being live on entry to
  // the function, otherwise an argument whose only use is in the first
  // instruction will be assigned a trivial live range and the register
  // allocator will not recognize its live range as overlapping another
  // variable's live range.
  assert(Nodes.empty() || (*Nodes.begin() == getEntryNode()));
}

// placePhiLoads() must be called before placePhiStores().
void Cfg::placePhiLoads() {
  TimerMarker T(TimerStack::TT_placePhiLoads, this);
  for (CfgNode *Node : Nodes)
    Node->placePhiLoads();
}

// placePhiStores() must be called after placePhiLoads().
void Cfg::placePhiStores() {
  TimerMarker T(TimerStack::TT_placePhiStores, this);
  for (CfgNode *Node : Nodes)
    Node->placePhiStores();
}

void Cfg::deletePhis() {
  TimerMarker T(TimerStack::TT_deletePhis, this);
  for (CfgNode *Node : Nodes)
    Node->deletePhis();
}

void Cfg::advancedPhiLowering() {
  TimerMarker T(TimerStack::TT_advancedPhiLowering, this);
  // Clear all previously computed live ranges (but not live-in/live-out bit
  // vectors or last-use markers), because the follow-on register allocation is
  // only concerned with live ranges across the newly created blocks.
  for (Variable *Var : Variables) {
    Var->getLiveRange().reset();
  }
  // This splits edges and appends new nodes to the end of the node list. This
  // can invalidate iterators, so don't use an iterator.
  SizeT NumNodes = getNumNodes();
  SizeT NumVars = getNumVariables();
  for (SizeT I = 0; I < NumNodes; ++I)
    Nodes[I]->advancedPhiLowering();

  TimerMarker TT(TimerStack::TT_lowerPhiAssignments, this);
  if (true) {
    // The following code does an in-place update of liveness and live ranges
    // as a result of adding the new phi edge split nodes.
    getLiveness()->initPhiEdgeSplits(Nodes.begin() + NumNodes,
                                     Variables.begin() + NumVars);
    TimerMarker TTT(TimerStack::TT_liveness, this);
    // Iterate over the newly added nodes to add their liveness info.
    for (auto I = Nodes.begin() + NumNodes, E = Nodes.end(); I != E; ++I) {
      InstNumberT FirstInstNum = getNextInstNumber();
      (*I)->renumberInstructions();
      InstNumberT LastInstNum = getNextInstNumber() - 1;
      (*I)->liveness(getLiveness());
      (*I)->livenessAddIntervals(getLiveness(), FirstInstNum, LastInstNum);
    }
  } else {
    // The following code does a brute-force recalculation of live ranges as a
    // result of adding the new phi edge split nodes. The liveness calculation
    // is particularly expensive because the new nodes are not yet in a proper
    // topological order and so convergence is slower.
    //
    // This code is kept here for reference and can be temporarily enabled in
    // case the incremental code is under suspicion.
    renumberInstructions();
    liveness(Liveness_Intervals);
    getVMetadata()->init(VMK_All);
  }
  Target->regAlloc(RAK_Phi);
}

// Find a reasonable placement for nodes that have not yet been placed, while
// maintaining the same relative ordering among already placed nodes.
void Cfg::reorderNodes() {
  // TODO(ascull): it would be nice if the switch tests were always followed by
  // the default case to allow for fall through.
  using PlacedList = CfgList<CfgNode *>;
  PlacedList Placed;      // Nodes with relative placement locked down
  PlacedList Unreachable; // Unreachable nodes
  PlacedList::iterator NoPlace = Placed.end();
  // Keep track of where each node has been tentatively placed so that we can
  // manage insertions into the middle.
  CfgVector<PlacedList::iterator> PlaceIndex(Nodes.size(), NoPlace);
  for (CfgNode *Node : Nodes) {
    // The "do ... while(0);" construct is to factor out the --PlaceIndex and
    // assert() statements before moving to the next node.
    do {
      if (Node != getEntryNode() && Node->getInEdges().empty()) {
        // The node has essentially been deleted since it is not a successor of
        // any other node.
        Unreachable.push_back(Node);
        PlaceIndex[Node->getIndex()] = Unreachable.end();
        Node->setNeedsPlacement(false);
        continue;
      }
      if (!Node->needsPlacement()) {
        // Add to the end of the Placed list.
        Placed.push_back(Node);
        PlaceIndex[Node->getIndex()] = Placed.end();
        continue;
      }
      Node->setNeedsPlacement(false);
      // Assume for now that the unplaced node is from edge-splitting and
      // therefore has 1 in-edge and 1 out-edge (actually, possibly more than 1
      // in-edge if the predecessor node was contracted). If this changes in
      // the future, rethink the strategy.
      assert(Node->getInEdges().size() >= 1);
      assert(Node->getOutEdges().size() == 1);

      // If it's a (non-critical) edge where the successor has a single
      // in-edge, then place it before the successor.
      CfgNode *Succ = Node->getOutEdges().front();
      if (Succ->getInEdges().size() == 1 &&
          PlaceIndex[Succ->getIndex()] != NoPlace) {
        Placed.insert(PlaceIndex[Succ->getIndex()], Node);
        PlaceIndex[Node->getIndex()] = PlaceIndex[Succ->getIndex()];
        continue;
      }

      // Otherwise, place it after the (first) predecessor.
      CfgNode *Pred = Node->getInEdges().front();
      auto PredPosition = PlaceIndex[Pred->getIndex()];
      // It shouldn't be the case that PredPosition==NoPlace, but if that
      // somehow turns out to be true, we just insert Node before
      // PredPosition=NoPlace=Placed.end() .
      if (PredPosition != NoPlace)
        ++PredPosition;
      Placed.insert(PredPosition, Node);
      PlaceIndex[Node->getIndex()] = PredPosition;
    } while (0);

    --PlaceIndex[Node->getIndex()];
    assert(*PlaceIndex[Node->getIndex()] == Node);
  }

  // Reorder Nodes according to the built-up lists.
  NodeList Reordered;
  Reordered.reserve(Placed.size() + Unreachable.size());
  for (CfgNode *Node : Placed)
    Reordered.push_back(Node);
  for (CfgNode *Node : Unreachable)
    Reordered.push_back(Node);
  assert(getNumNodes() == Reordered.size());
  swapNodes(Reordered);
}

namespace {
void getRandomPostOrder(CfgNode *Node, BitVector &ToVisit,
                        Ice::NodeList &PostOrder,
                        Ice::RandomNumberGenerator *RNG) {
  assert(ToVisit[Node->getIndex()]);
  ToVisit[Node->getIndex()] = false;
  NodeList Outs = Node->getOutEdges();
  Ice::RandomShuffle(Outs.begin(), Outs.end(),
                     [RNG](int N) { return RNG->next(N); });
  for (CfgNode *Next : Outs) {
    if (ToVisit[Next->getIndex()])
      getRandomPostOrder(Next, ToVisit, PostOrder, RNG);
  }
  PostOrder.push_back(Node);
}
} // end of anonymous namespace

void Cfg::shuffleNodes() {
  if (!Ctx->getFlags().shouldReorderBasicBlocks())
    return;

  NodeList ReversedReachable;
  NodeList Unreachable;
  BitVector ToVisit(Nodes.size(), true);
  // Create Random number generator for function reordering
  RandomNumberGenerator RNG(Ctx->getFlags().getRandomSeed(),
                            RPE_BasicBlockReordering, SequenceNumber);
  // Traverse from entry node.
  getRandomPostOrder(getEntryNode(), ToVisit, ReversedReachable, &RNG);
  // Collect the unreachable nodes.
  for (CfgNode *Node : Nodes)
    if (ToVisit[Node->getIndex()])
      Unreachable.push_back(Node);
  // Copy the layout list to the Nodes.
  NodeList Shuffled;
  Shuffled.reserve(ReversedReachable.size() + Unreachable.size());
  for (CfgNode *Node : reverse_range(ReversedReachable))
    Shuffled.push_back(Node);
  for (CfgNode *Node : Unreachable)
    Shuffled.push_back(Node);
  assert(Nodes.size() == Shuffled.size());
  swapNodes(Shuffled);

  dump("After basic block shuffling");
}

void Cfg::doArgLowering() {
  TimerMarker T(TimerStack::TT_doArgLowering, this);
  getTarget()->lowerArguments();
}

void Cfg::sortAndCombineAllocas(CfgVector<Inst *> &Allocas,
                                uint32_t CombinedAlignment, InstList &Insts,
                                AllocaBaseVariableType BaseVariableType) {
  if (Allocas.empty())
    return;
  // Sort by decreasing alignment.
  std::sort(Allocas.begin(), Allocas.end(), [](Inst *I1, Inst *I2) {
    auto *A1 = llvm::dyn_cast<InstAlloca>(I1);
    auto *A2 = llvm::dyn_cast<InstAlloca>(I2);
    return A1->getAlignInBytes() > A2->getAlignInBytes();
  });
  // Process the allocas in order of decreasing stack alignment.  This allows
  // us to pack less-aligned pieces after more-aligned ones, resulting in less
  // stack growth.  It also allows there to be at most one stack alignment "and"
  // instruction for a whole list of allocas.
  uint32_t CurrentOffset = 0;
  CfgVector<int32_t> Offsets;
  for (Inst *Instr : Allocas) {
    auto *Alloca = llvm::cast<InstAlloca>(Instr);
    // Adjust the size of the allocation up to the next multiple of the
    // object's alignment.
    uint32_t Alignment = std::max(Alloca->getAlignInBytes(), 1u);
    auto *ConstSize =
        llvm::dyn_cast<ConstantInteger32>(Alloca->getSizeInBytes());
    uint32_t Size = Utils::applyAlignment(ConstSize->getValue(), Alignment);
    if (BaseVariableType == BVT_FramePointer) {
      // Addressing is relative to the frame pointer.  Subtract the offset after
      // adding the size of the alloca, because it grows downwards from the
      // frame pointer.
      Offsets.push_back(-(CurrentOffset + Size));
    } else {
      // Addressing is relative to the stack pointer or to a user pointer.  Add
      // the offset before adding the size of the object, because it grows
      // upwards from the stack pointer. In addition, if the addressing is
      // relative to the stack pointer, we need to add the pre-computed max out
      // args size bytes.
      const uint32_t OutArgsOffsetOrZero =
          (BaseVariableType == BVT_StackPointer)
              ? getTarget()->maxOutArgsSizeBytes()
              : 0;
      Offsets.push_back(CurrentOffset + OutArgsOffsetOrZero);
    }
    // Update the running offset of the fused alloca region.
    CurrentOffset += Size;
  }
  // Round the offset up to the alignment granularity to use as the size.
  uint32_t TotalSize = Utils::applyAlignment(CurrentOffset, CombinedAlignment);
  // Ensure every alloca was assigned an offset.
  assert(Allocas.size() == Offsets.size());

  switch (BaseVariableType) {
  case BVT_UserPointer: {
    Variable *BaseVariable = makeVariable(IceType_i32);
    for (SizeT i = 0; i < Allocas.size(); ++i) {
      auto *Alloca = llvm::cast<InstAlloca>(Allocas[i]);
      // Emit a new addition operation to replace the alloca.
      Operand *AllocaOffset = Ctx->getConstantInt32(Offsets[i]);
      InstArithmetic *Add =
          InstArithmetic::create(this, InstArithmetic::Add, Alloca->getDest(),
                                 BaseVariable, AllocaOffset);
      Insts.push_front(Add);
      Alloca->setDeleted();
    }
    Operand *AllocaSize = Ctx->getConstantInt32(TotalSize);
    InstAlloca *CombinedAlloca =
        InstAlloca::create(this, BaseVariable, AllocaSize, CombinedAlignment);
    CombinedAlloca->setKnownFrameOffset();
    Insts.push_front(CombinedAlloca);
  } break;
  case BVT_StackPointer:
  case BVT_FramePointer: {
    for (SizeT i = 0; i < Allocas.size(); ++i) {
      auto *Alloca = llvm::cast<InstAlloca>(Allocas[i]);
      // Emit a fake definition of the rematerializable variable.
      Variable *Dest = Alloca->getDest();
      auto *Def = InstFakeDef::create(this, Dest);
      if (BaseVariableType == BVT_StackPointer)
        Dest->setRematerializable(getTarget()->getStackReg(), Offsets[i]);
      else
        Dest->setRematerializable(getTarget()->getFrameReg(), Offsets[i]);
      Insts.push_front(Def);
      Alloca->setDeleted();
    }
    // Allocate the fixed area in the function prolog.
    getTarget()->reserveFixedAllocaArea(TotalSize, CombinedAlignment);
  } break;
  }
}

void Cfg::processAllocas(bool SortAndCombine) {
  const uint32_t StackAlignment = getTarget()->getStackAlignment();
  CfgNode *EntryNode = getEntryNode();
  // LLVM enforces power of 2 alignment.
  assert(llvm::isPowerOf2_32(StackAlignment));
  // Determine if there are large alignment allocations in the entry block or
  // dynamic allocations (variable size in the entry block).
  bool HasLargeAlignment = false;
  bool HasDynamicAllocation = false;
  for (Inst &Instr : EntryNode->getInsts()) {
    if (auto *Alloca = llvm::dyn_cast<InstAlloca>(&Instr)) {
      uint32_t AlignmentParam = Alloca->getAlignInBytes();
      if (AlignmentParam > StackAlignment)
        HasLargeAlignment = true;
      if (llvm::isa<Constant>(Alloca->getSizeInBytes()))
        Alloca->setKnownFrameOffset();
      else {
        HasDynamicAllocation = true;
        // If Allocas are not sorted, the first dynamic allocation causes
        // later Allocas to be at unknown offsets relative to the stack/frame.
        if (!SortAndCombine)
          break;
      }
    }
  }
  // Don't do the heavyweight sorting and layout for low optimization levels.
  if (!SortAndCombine)
    return;
  // Any alloca outside the entry block is a dynamic allocation.
  for (CfgNode *Node : Nodes) {
    if (Node == EntryNode)
      continue;
    for (Inst &Instr : Node->getInsts()) {
      if (llvm::isa<InstAlloca>(&Instr)) {
        // Allocations outside the entry block require a frame pointer.
        HasDynamicAllocation = true;
        break;
      }
    }
    if (HasDynamicAllocation)
      break;
  }
  // Mark the target as requiring a frame pointer.
  if (HasLargeAlignment || HasDynamicAllocation)
    getTarget()->setHasFramePointer();
  // Collect the Allocas into the two vectors.
  // Allocas in the entry block that have constant size and alignment less
  // than or equal to the function's stack alignment.
  CfgVector<Inst *> FixedAllocas;
  // Allocas in the entry block that have constant size and alignment greater
  // than the function's stack alignment.
  CfgVector<Inst *> AlignedAllocas;
  // Maximum alignment used by any alloca.
  uint32_t MaxAlignment = StackAlignment;
  for (Inst &Instr : EntryNode->getInsts()) {
    if (auto *Alloca = llvm::dyn_cast<InstAlloca>(&Instr)) {
      if (!llvm::isa<Constant>(Alloca->getSizeInBytes()))
        continue;
      uint32_t AlignmentParam = Alloca->getAlignInBytes();
      // For default align=0, set it to the real value 1, to avoid any
      // bit-manipulation problems below.
      AlignmentParam = std::max(AlignmentParam, 1u);
      assert(llvm::isPowerOf2_32(AlignmentParam));
      if (HasDynamicAllocation && AlignmentParam > StackAlignment) {
        // If we have both dynamic allocations and large stack alignments,
        // high-alignment allocations are pulled out with their own base.
        AlignedAllocas.push_back(Alloca);
      } else {
        FixedAllocas.push_back(Alloca);
      }
      MaxAlignment = std::max(AlignmentParam, MaxAlignment);
    }
  }
  // Add instructions to the head of the entry block in reverse order.
  InstList &Insts = getEntryNode()->getInsts();
  if (HasDynamicAllocation && HasLargeAlignment) {
    // We are using a frame pointer, but fixed large-alignment alloca addresses,
    // do not have a known offset from either the stack or frame pointer.
    // They grow up from a user pointer from an alloca.
    sortAndCombineAllocas(AlignedAllocas, MaxAlignment, Insts, BVT_UserPointer);
    // Fixed size allocas are addressed relative to the frame pointer.
    sortAndCombineAllocas(FixedAllocas, StackAlignment, Insts,
                          BVT_FramePointer);
  } else {
    // Otherwise, fixed size allocas are addressed relative to the stack unless
    // there are dynamic allocas.
    const AllocaBaseVariableType BasePointerType =
        (HasDynamicAllocation ? BVT_FramePointer : BVT_StackPointer);
    sortAndCombineAllocas(FixedAllocas, MaxAlignment, Insts, BasePointerType);
  }
  if (!FixedAllocas.empty() || !AlignedAllocas.empty())
    // No use calling findRematerializable() unless there is some
    // rematerializable alloca instruction to seed it.
    findRematerializable();
}

namespace {

// Helpers for findRematerializable().  For each of them, if a suitable
// rematerialization is found, the instruction's Dest variable is set to be
// rematerializable and it returns true, otherwise it returns false.

bool rematerializeArithmetic(const Inst *Instr) {
  // Check that it's an Arithmetic instruction with an Add operation.
  auto *Arith = llvm::dyn_cast<InstArithmetic>(Instr);
  if (Arith == nullptr || Arith->getOp() != InstArithmetic::Add)
    return false;
  // Check that Src(0) is rematerializable.
  auto *Src0Var = llvm::dyn_cast<Variable>(Arith->getSrc(0));
  if (Src0Var == nullptr || !Src0Var->isRematerializable())
    return false;
  // Check that Src(1) is an immediate.
  auto *Src1Imm = llvm::dyn_cast<ConstantInteger32>(Arith->getSrc(1));
  if (Src1Imm == nullptr)
    return false;
  Arith->getDest()->setRematerializable(
      Src0Var->getRegNum(), Src0Var->getStackOffset() + Src1Imm->getValue());
  return true;
}

bool rematerializeAssign(const Inst *Instr) {
  // An InstAssign only originates from an inttoptr or ptrtoint instruction,
  // which never occurs in a MINIMAL build.
  if (BuildDefs::minimal())
    return false;
  // Check that it's an Assign instruction.
  if (!llvm::isa<InstAssign>(Instr))
    return false;
  // Check that Src(0) is rematerializable.
  auto *Src0Var = llvm::dyn_cast<Variable>(Instr->getSrc(0));
  if (Src0Var == nullptr || !Src0Var->isRematerializable())
    return false;
  Instr->getDest()->setRematerializable(Src0Var->getRegNum(),
                                        Src0Var->getStackOffset());
  return true;
}

bool rematerializeCast(const Inst *Instr) {
  // An pointer-type bitcast never occurs in a MINIMAL build.
  if (BuildDefs::minimal())
    return false;
  // Check that it's a Cast instruction with a Bitcast operation.
  auto *Cast = llvm::dyn_cast<InstCast>(Instr);
  if (Cast == nullptr || Cast->getCastKind() != InstCast::Bitcast)
    return false;
  // Check that Src(0) is rematerializable.
  auto *Src0Var = llvm::dyn_cast<Variable>(Cast->getSrc(0));
  if (Src0Var == nullptr || !Src0Var->isRematerializable())
    return false;
  // Check that Dest and Src(0) have the same type.
  Variable *Dest = Cast->getDest();
  if (Dest->getType() != Src0Var->getType())
    return false;
  Dest->setRematerializable(Src0Var->getRegNum(), Src0Var->getStackOffset());
  return true;
}

} // end of anonymous namespace

/// Scan the function to find additional rematerializable variables.  This is
/// possible when the source operand of an InstAssignment is a rematerializable
/// variable, or the same for a pointer-type InstCast::Bitcast, or when an
/// InstArithmetic is an add of a rematerializable variable and an immediate.
/// Note that InstAssignment instructions and pointer-type InstCast::Bitcast
/// instructions generally only come about from the IceConverter's treatment of
/// inttoptr, ptrtoint, and bitcast instructions.  TODO(stichnot): Consider
/// other possibilities, however unlikely, such as InstArithmetic::Sub, or
/// commutativity.
void Cfg::findRematerializable() {
  // Scan the instructions in order, and repeat until no new opportunities are
  // found.  It may take more than one iteration because a variable's defining
  // block may happen to come after a block where it is used, depending on the
  // CfgNode linearization order.
  bool FoundNewAssignment;
  do {
    FoundNewAssignment = false;
    for (CfgNode *Node : getNodes()) {
      // No need to process Phi instructions.
      for (Inst &Instr : Node->getInsts()) {
        if (Instr.isDeleted())
          continue;
        Variable *Dest = Instr.getDest();
        if (Dest == nullptr || Dest->isRematerializable())
          continue;
        if (rematerializeArithmetic(&Instr) || rematerializeAssign(&Instr) ||
            rematerializeCast(&Instr)) {
          FoundNewAssignment = true;
        }
      }
    }
  } while (FoundNewAssignment);
}

void Cfg::doAddressOpt() {
  TimerMarker T(TimerStack::TT_doAddressOpt, this);
  for (CfgNode *Node : Nodes)
    Node->doAddressOpt();
}

void Cfg::doNopInsertion() {
  if (!Ctx->getFlags().shouldDoNopInsertion())
    return;
  TimerMarker T(TimerStack::TT_doNopInsertion, this);
  RandomNumberGenerator RNG(Ctx->getFlags().getRandomSeed(), RPE_NopInsertion,
                            SequenceNumber);
  for (CfgNode *Node : Nodes)
    Node->doNopInsertion(RNG);
}

void Cfg::genCode() {
  TimerMarker T(TimerStack::TT_genCode, this);
  for (CfgNode *Node : Nodes)
    Node->genCode();
}

// Compute the stack frame layout.
void Cfg::genFrame() {
  TimerMarker T(TimerStack::TT_genFrame, this);
  getTarget()->addProlog(Entry);
  for (CfgNode *Node : Nodes)
    if (Node->getHasReturn())
      getTarget()->addEpilog(Node);
}

void Cfg::computeLoopNestDepth() {
  TimerMarker T(TimerStack::TT_computeLoopNestDepth, this);
  LoopAnalyzer LA(this);
  LA.computeLoopNestDepth();
}

// This is a lightweight version of live-range-end calculation. Marks the last
// use of only those variables whose definition and uses are completely with a
// single block. It is a quick single pass and doesn't need to iterate until
// convergence.
void Cfg::livenessLightweight() {
  TimerMarker T(TimerStack::TT_livenessLightweight, this);
  getVMetadata()->init(VMK_Uses);
  for (CfgNode *Node : Nodes)
    Node->livenessLightweight();
}

void Cfg::liveness(LivenessMode Mode) {
  TimerMarker T(TimerStack::TT_liveness, this);
  Live.reset(new Liveness(this, Mode));
  getVMetadata()->init(VMK_Uses);
  Live->init();
  // Initialize with all nodes needing to be processed.
  BitVector NeedToProcess(Nodes.size(), true);
  while (NeedToProcess.any()) {
    // Iterate in reverse topological order to speed up convergence.
    for (CfgNode *Node : reverse_range(Nodes)) {
      if (NeedToProcess[Node->getIndex()]) {
        NeedToProcess[Node->getIndex()] = false;
        bool Changed = Node->liveness(getLiveness());
        if (Changed) {
          // If the beginning-of-block liveness changed since the last
          // iteration, mark all in-edges as needing to be processed.
          for (CfgNode *Pred : Node->getInEdges())
            NeedToProcess[Pred->getIndex()] = true;
        }
      }
    }
  }
  if (Mode == Liveness_Intervals) {
    // Reset each variable's live range.
    for (Variable *Var : Variables)
      Var->resetLiveRange();
  }
  // Make a final pass over each node to delete dead instructions, collect the
  // first and last instruction numbers, and add live range segments for that
  // node.
  for (CfgNode *Node : Nodes) {
    InstNumberT FirstInstNum = Inst::NumberSentinel;
    InstNumberT LastInstNum = Inst::NumberSentinel;
    for (Inst &I : Node->getPhis()) {
      I.deleteIfDead();
      if (Mode == Liveness_Intervals && !I.isDeleted()) {
        if (FirstInstNum == Inst::NumberSentinel)
          FirstInstNum = I.getNumber();
        assert(I.getNumber() > LastInstNum);
        LastInstNum = I.getNumber();
      }
    }
    for (Inst &I : Node->getInsts()) {
      I.deleteIfDead();
      if (Mode == Liveness_Intervals && !I.isDeleted()) {
        if (FirstInstNum == Inst::NumberSentinel)
          FirstInstNum = I.getNumber();
        assert(I.getNumber() > LastInstNum);
        LastInstNum = I.getNumber();
      }
    }
    if (Mode == Liveness_Intervals) {
      // Special treatment for live in-args. Their liveness needs to extend
      // beyond the beginning of the function, otherwise an arg whose only use
      // is in the first instruction will end up having the trivial live range
      // [2,2) and will *not* interfere with other arguments. So if the first
      // instruction of the method is "r=arg1+arg2", both args may be assigned
      // the same register. This is accomplished by extending the entry block's
      // instruction range from [2,n) to [1,n) which will transform the
      // problematic [2,2) live ranges into [1,2).  This extension works because
      // the entry node is guaranteed to have the lowest instruction numbers.
      if (Node == getEntryNode()) {
        FirstInstNum = Inst::NumberExtended;
        // Just in case the entry node somehow contains no instructions...
        if (LastInstNum == Inst::NumberSentinel)
          LastInstNum = FirstInstNum;
      }
      // If this node somehow contains no instructions, don't bother trying to
      // add liveness intervals for it, because variables that are live-in and
      // live-out will have a bogus interval added.
      if (FirstInstNum != Inst::NumberSentinel)
        Node->livenessAddIntervals(getLiveness(), FirstInstNum, LastInstNum);
    }
  }
}

// Traverse every Variable of every Inst and verify that it appears within the
// Variable's computed live range.
bool Cfg::validateLiveness() const {
  TimerMarker T(TimerStack::TT_validateLiveness, this);
  bool Valid = true;
  OstreamLocker L(Ctx);
  Ostream &Str = Ctx->getStrDump();
  for (CfgNode *Node : Nodes) {
    Inst *FirstInst = nullptr;
    for (Inst &Instr : Node->getInsts()) {
      if (Instr.isDeleted())
        continue;
      if (FirstInst == nullptr)
        FirstInst = &Instr;
      InstNumberT InstNumber = Instr.getNumber();
      if (Variable *Dest = Instr.getDest()) {
        if (!Dest->getIgnoreLiveness()) {
          bool Invalid = false;
          constexpr bool IsDest = true;
          if (!Dest->getLiveRange().containsValue(InstNumber, IsDest))
            Invalid = true;
          // Check that this instruction actually *begins* Dest's live range,
          // by checking that Dest is not live in the previous instruction. As
          // a special exception, we don't check this for the first instruction
          // of the block, because a Phi temporary may be live at the end of
          // the previous block, and if it is also assigned in the first
          // instruction of this block, the adjacent live ranges get merged.
          if (&Instr != FirstInst && !Instr.isDestRedefined() &&
              Dest->getLiveRange().containsValue(InstNumber - 1, IsDest))
            Invalid = true;
          if (Invalid) {
            Valid = false;
            Str << "Liveness error: inst " << Instr.getNumber() << " dest ";
            Dest->dump(this);
            Str << " live range " << Dest->getLiveRange() << "\n";
          }
        }
      }
      FOREACH_VAR_IN_INST(Var, Instr) {
        static constexpr bool IsDest = false;
        if (!Var->getIgnoreLiveness() &&
            !Var->getLiveRange().containsValue(InstNumber, IsDest)) {
          Valid = false;
          Str << "Liveness error: inst " << Instr.getNumber() << " var ";
          Var->dump(this);
          Str << " live range " << Var->getLiveRange() << "\n";
        }
      }
    }
  }
  return Valid;
}

void Cfg::contractEmptyNodes() {
  // If we're decorating the asm output with register liveness info, this
  // information may become corrupted or incorrect after contracting nodes that
  // contain only redundant assignments. As such, we disable this pass when
  // DecorateAsm is specified. This may make the resulting code look more
  // branchy, but it should have no effect on the register assignments.
  if (Ctx->getFlags().getDecorateAsm())
    return;
  for (CfgNode *Node : Nodes) {
    Node->contractIfEmpty();
  }
}

void Cfg::doBranchOpt() {
  TimerMarker T(TimerStack::TT_doBranchOpt, this);
  for (auto I = Nodes.begin(), E = Nodes.end(); I != E; ++I) {
    auto NextNode = I + 1;
    (*I)->doBranchOpt(NextNode == E ? nullptr : *NextNode);
  }
}

void Cfg::markNodesForSandboxing() {
  for (const InstJumpTable *JT : JumpTables)
    for (SizeT I = 0; I < JT->getNumTargets(); ++I)
      JT->getTarget(I)->setNeedsAlignment();
}

// ======================== Dump routines ======================== //

// emitTextHeader() is not target-specific (apart from what is abstracted by
// the Assembler), so it is defined here rather than in the target lowering
// class.
void Cfg::emitTextHeader(const IceString &MangledName, GlobalContext *Ctx,
                         const Assembler *Asm) {
  if (!BuildDefs::dump())
    return;
  Ostream &Str = Ctx->getStrEmit();
  Str << "\t.text\n";
  if (Ctx->getFlags().getFunctionSections())
    Str << "\t.section\t.text." << MangledName << ",\"ax\",%progbits\n";
  if (!Asm->getInternal() || Ctx->getFlags().getDisableInternal()) {
    Str << "\t.globl\t" << MangledName << "\n";
    Str << "\t.type\t" << MangledName << ",%function\n";
  }
  Str << "\t" << Asm->getAlignDirective() << " "
      << Asm->getBundleAlignLog2Bytes() << ",0x";
  for (uint8_t I : Asm->getNonExecBundlePadding())
    Str.write_hex(I);
  Str << "\n";
  Str << MangledName << ":\n";
}

void Cfg::deleteJumpTableInsts() {
  for (InstJumpTable *JumpTable : JumpTables)
    JumpTable->setDeleted();
}

void Cfg::emitJumpTables() {
  switch (Ctx->getFlags().getOutFileType()) {
  case FT_Elf:
  case FT_Iasm: {
    // The emission needs to be delayed until the after the text section so
    // save the offsets in the global context.
    IceString MangledName = Ctx->mangleName(getFunctionName());
    for (const InstJumpTable *JumpTable : JumpTables) {
      SizeT NumTargets = JumpTable->getNumTargets();
      JumpTableData::TargetList TargetList;
      for (SizeT I = 0; I < NumTargets; ++I) {
        SizeT Index = JumpTable->getTarget(I)->getIndex();
        TargetList.emplace_back(
            getAssembler()->getCfgNodeLabel(Index)->getPosition());
      }
      Ctx->addJumpTable(MangledName, JumpTable->getId(), TargetList);
    }
  } break;
  case FT_Asm: {
    // Emit the assembly directly so we don't need to hang on to all the names
    for (const InstJumpTable *JumpTable : JumpTables)
      getTarget()->emitJumpTable(this, JumpTable);
  } break;
  }
}

void Cfg::emit() {
  if (!BuildDefs::dump())
    return;
  TimerMarker T(TimerStack::TT_emit, this);
  if (Ctx->getFlags().getDecorateAsm()) {
    renumberInstructions();
    getVMetadata()->init(VMK_Uses);
    liveness(Liveness_Basic);
    dump("After recomputing liveness for -decorate-asm");
  }
  OstreamLocker L(Ctx);
  Ostream &Str = Ctx->getStrEmit();
  IceString MangledName = Ctx->mangleName(getFunctionName());
  const Assembler *Asm = getAssembler<>();
  const bool NeedSandboxing = Ctx->getFlags().getUseSandboxing();

  emitTextHeader(MangledName, Ctx, Asm);
  deleteJumpTableInsts();
  if (Ctx->getFlags().getDecorateAsm()) {
    for (Variable *Var : getVariables()) {
      if (Var->getStackOffset() && !Var->isRematerializable()) {
        Str << "\t" << Var->getSymbolicStackOffset(this) << " = "
            << Var->getStackOffset() << "\n";
      }
    }
  }
  for (CfgNode *Node : Nodes) {
    if (NeedSandboxing && Node->needsAlignment()) {
      Str << "\t" << Asm->getAlignDirective() << " "
          << Asm->getBundleAlignLog2Bytes() << "\n";
    }
    Node->emit(this);
  }
  emitJumpTables();
  Str << "\n";
}

void Cfg::emitIAS() {
  TimerMarker T(TimerStack::TT_emit, this);
  // The emitIAS() routines emit into the internal assembler buffer, so there's
  // no need to lock the streams.
  deleteJumpTableInsts();
  const bool NeedSandboxing = Ctx->getFlags().getUseSandboxing();
  for (CfgNode *Node : Nodes) {
    if (NeedSandboxing && Node->needsAlignment())
      getAssembler()->alignCfgNode();
    Node->emitIAS(this);
  }
  emitJumpTables();
}

size_t Cfg::getTotalMemoryMB() {
  constexpr size_t OneMB = 1024 * 1024;
  using ArbitraryType = int;
  // CfgLocalAllocator draws from the same memory pool regardless of allocated
  // object type, so pick an arbitrary type for the template parameter.
  return CfgLocalAllocator<ArbitraryType>().current()->getTotalMemory() / OneMB;
}

// Dumps the IR with an optional introductory message.
void Cfg::dump(const IceString &Message) {
  if (!BuildDefs::dump())
    return;
  if (!isVerbose())
    return;
  OstreamLocker L(Ctx);
  Ostream &Str = Ctx->getStrDump();
  if (!Message.empty())
    Str << "================ " << Message << " ================\n";
  if (isVerbose(IceV_Mem)) {
    Str << "Memory size = " << getTotalMemoryMB() << " MB\n";
  }
  setCurrentNode(getEntryNode());
  // Print function name+args
  if (isVerbose(IceV_Instructions)) {
    Str << "define ";
    if (getInternal() && !Ctx->getFlags().getDisableInternal())
      Str << "internal ";
    Str << ReturnType << " @" << Ctx->mangleName(getFunctionName()) << "(";
    for (SizeT i = 0; i < Args.size(); ++i) {
      if (i > 0)
        Str << ", ";
      Str << Args[i]->getType() << " ";
      Args[i]->dump(this);
    }
    // Append an extra copy of the function name here, in order to print its
    // size stats but not mess up lit tests.
    Str << ") { # " << getFunctionNameAndSize() << "\n";
  }
  resetCurrentNode();
  if (isVerbose(IceV_Liveness)) {
    // Print summary info about variables
    for (Variable *Var : Variables) {
      Str << "// multiblock=";
      if (getVMetadata()->isTracked(Var))
        Str << getVMetadata()->isMultiBlock(Var);
      else
        Str << "?";
      Str << " defs=";
      bool FirstPrint = true;
      if (VMetadata->getKind() != VMK_Uses) {
        if (const Inst *FirstDef = VMetadata->getFirstDefinition(Var)) {
          Str << FirstDef->getNumber();
          FirstPrint = false;
        }
      }
      if (VMetadata->getKind() == VMK_All) {
        for (const Inst *Instr : VMetadata->getLatterDefinitions(Var)) {
          if (!FirstPrint)
            Str << ",";
          Str << Instr->getNumber();
          FirstPrint = false;
        }
      }
      Str << " weight=" << Var->getWeight(this) << " ";
      Var->dump(this);
      Str << " LIVE=" << Var->getLiveRange() << "\n";
    }
  }
  // Print each basic block
  for (CfgNode *Node : Nodes)
    Node->dump(this);
  if (isVerbose(IceV_Instructions))
    Str << "}\n";
}

} // end of namespace Ice
