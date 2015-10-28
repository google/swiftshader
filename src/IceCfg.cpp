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
/// This file implements the Cfg class, including constant pool management.
///
//===----------------------------------------------------------------------===//

#include "IceCfg.h"

#include "IceAssembler.h"
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

ICE_TLS_DEFINE_FIELD(const Cfg *, Cfg, CurrentCfg);

ArenaAllocator<> *getCurrentCfgAllocator() {
  return Cfg::getCurrentCfgAllocator();
}

Cfg::Cfg(GlobalContext *Ctx, uint32_t SequenceNumber)
    : Ctx(Ctx), SequenceNumber(SequenceNumber),
      VMask(Ctx->getFlags().getVerbose()), NextInstNumber(Inst::NumberInitial),
      Allocator(new ArenaAllocator<>()), Live(nullptr),
      Target(TargetLowering::createLowering(Ctx->getFlags().getTargetArch(),
                                            this)),
      VMetadata(new VariablesMetadata(this)),
      TargetAssembler(TargetLowering::createAssembler(
          Ctx->getFlags().getTargetArch(), this)) {
  if (Ctx->getFlags().getRandomizeAndPoolImmediatesOption() == RPI_Randomize) {
    // If -randomize-pool-immediates=randomize, create a random number
    // generator to generate a cookie for constant blinding.
    RandomNumberGenerator RNG(Ctx->getFlags().getRandomSeed(),
                              RPE_ConstantBlinding, this->SequenceNumber);
    ConstantBlindingCookie =
        (uint32_t)RNG.next((uint64_t)std::numeric_limits<uint32_t>::max() + 1);
  }
}

Cfg::~Cfg() { assert(ICE_TLS_GET_FIELD(CurrentCfg) == nullptr); }

void Cfg::setError(const IceString &Message) {
  HasError = true;
  ErrorMessage = Message;
}

CfgNode *Cfg::makeNode() {
  SizeT LabelIndex = Nodes.size();
  CfgNode *Node = CfgNode::create(this, LabelIndex);
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
  VariableDeclaration *Var = VariableDeclaration::create(Ctx);
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
  VariableDeclaration *Var = VariableDeclaration::create(Ctx);
  Var->setName(BlockStatsGlobalPrefix + NodeAsmName);
  const SizeT Int64ByteSize = typeWidthInBytes(IceType_i64);
  Var->addInitializer(
      VariableDeclaration::ZeroInitializer::create(Int64ByteSize));

  const RelocOffsetT NodeNameDeclarationOffset = 0;
  Var->addInitializer(VariableDeclaration::RelocInitializer::create(
      NodeNameDeclaration, NodeNameDeclarationOffset));
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

  // Figure out which alloca instructions result in storage at known stack frame
  // offsets.  If this is true for all alloca instructions, then a stack pointer
  // can still be used instead of a frame pointer, freeing up the frame pointer
  // for normal register allocation.  Additionally, for each such alloca, its
  // address could be rematerialized at each use in terms of the stack/frame
  // pointer, saving a stack slot and a load from that stack slot.
  //
  // This simple implementation is limited to alloca instructions at the start
  // of the entry node.
  for (Inst &Instr : getEntryNode()->getInsts()) {
    if (auto *Alloca = llvm::dyn_cast<InstAlloca>(&Instr)) {
      if (llvm::isa<Constant>(Alloca->getSizeInBytes())) {
        Alloca->setKnownFrameOffset();
        continue;
      }
    }
    // The first instruction that is not an alloca with a constant size stops
    // the search.
    break;
  }

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
  llvm::BitVector Reachable(NumNodes);
  llvm::BitVector Pending(NumNodes);
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
void getRandomPostOrder(CfgNode *Node, llvm::BitVector &ToVisit,
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
  llvm::BitVector ToVisit(Nodes.size(), true);
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
  llvm::BitVector NeedToProcess(Nodes.size(), true);
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
      // problematic [2,2) live ranges into [1,2).
      if (Node == getEntryNode()) {
        // TODO(stichnot): Make it a strict requirement that the entry node
        // gets the lowest instruction numbers, so that extending the live
        // range for in-args is guaranteed to work.
        FirstInstNum = Inst::NumberExtended;
      }
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
    for (Inst &Inst : Node->getInsts()) {
      if (Inst.isDeleted())
        continue;
      if (FirstInst == nullptr)
        FirstInst = &Inst;
      InstNumberT InstNumber = Inst.getNumber();
      if (Variable *Dest = Inst.getDest()) {
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
          if (static_cast<class Inst *>(&Inst) != FirstInst &&
              !Inst.isDestRedefined() &&
              Dest->getLiveRange().containsValue(InstNumber - 1, IsDest))
            Invalid = true;
          if (Invalid) {
            Valid = false;
            Str << "Liveness error: inst " << Inst.getNumber() << " dest ";
            Dest->dump(this);
            Str << " live range " << Dest->getLiveRange() << "\n";
          }
        }
      }
      FOREACH_VAR_IN_INST(Var, Inst) {
        static constexpr bool IsDest = false;
        if (!Var->getIgnoreLiveness() &&
            !Var->getLiveRange().containsValue(InstNumber, IsDest)) {
          Valid = false;
          Str << "Liveness error: inst " << Inst.getNumber() << " var ";
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
      JumpTableData &JT =
          Ctx->addJumpTable(MangledName, JumpTable->getId(), NumTargets);
      for (SizeT I = 0; I < NumTargets; ++I) {
        SizeT Index = JumpTable->getTarget(I)->getIndex();
        JT.pushTarget(getAssembler()->getCfgNodeLabel(Index)->getPosition());
      }
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
      if (Var->getStackOffset()) {
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
    Str << ") {\n";
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
