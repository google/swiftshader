//===- subzero/src/IceCfg.cpp - Control flow graph implementation ---------===//
//
//                        The Subzero Code Generator
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements the Cfg class, including constant pool
// management.
//
//===----------------------------------------------------------------------===//

#include "IceAssembler.h"
#include "IceCfg.h"
#include "IceCfgNode.h"
#include "IceClFlags.h"
#include "IceDefs.h"
#include "IceELFObjectWriter.h"
#include "IceGlobalInits.h"
#include "IceInst.h"
#include "IceLiveness.h"
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
  assert(!Ctx->isIRGenerationDisabled() &&
         "Attempt to build cfg when IR generation disabled");
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

void Cfg::addArg(Variable *Arg) {
  Arg->setIsArg();
  Args.push_back(Arg);
}

void Cfg::addImplicitArg(Variable *Arg) {
  Arg->setIsImplicitArg();
  ImplicitArgs.push_back(Arg);
}

// Returns whether the stack frame layout has been computed yet.  This
// is used for dumping the stack frame location of Variables.
bool Cfg::hasComputedFrame() const { return getTarget()->hasComputedFrame(); }

namespace {
constexpr char BlockNameGlobalPrefix[] = ".L$profiler$block_name$";
constexpr char BlockStatsGlobalPrefix[] = ".L$profiler$block_info$";

VariableDeclaration *nodeNameDeclaration(const IceString &NodeAsmName) {
  VariableDeclaration *Var = VariableDeclaration::create();
  Var->setName(BlockNameGlobalPrefix + NodeAsmName);
  Var->setIsConstant(true);
  Var->addInitializer(new VariableDeclaration::DataInitializer(
      NodeAsmName.data(), NodeAsmName.size() + 1));
  const SizeT Int64ByteSize = typeWidthInBytes(IceType_i64);
  Var->setAlignment(Int64ByteSize); // Wasteful, 32-bit could use 4 bytes.
  return Var;
}

VariableDeclaration *
blockProfilingInfoDeclaration(const IceString &NodeAsmName,
                              VariableDeclaration *NodeNameDeclaration) {
  VariableDeclaration *Var = VariableDeclaration::create();
  Var->setName(BlockStatsGlobalPrefix + NodeAsmName);
  const SizeT Int64ByteSize = typeWidthInBytes(IceType_i64);
  Var->addInitializer(new VariableDeclaration::ZeroInitializer(Int64ByteSize));

  const RelocOffsetT NodeNameDeclarationOffset = 0;
  Var->addInitializer(new VariableDeclaration::RelocInitializer(
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
    GlobalInits->push_back(nodeNameDeclaration(NodeAsmName));
    GlobalInits->push_back(
        blockProfilingInfoDeclaration(NodeAsmName, GlobalInits->back()));
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
  // FunctionTimer conditionally pushes/pops a TimerMarker if
  // TimeEachFunction is enabled.
  std::unique_ptr<TimerMarker> FunctionTimer;
  if (ALLOW_DUMP) {
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
    // TODO(jpp): this is fragile, at best. Figure out a better way of detecting
    // exit functions.
    if (GlobalContext::matchSymbolName(getFunctionName(), "exit")) {
      addCallToProfileSummary();
    }
    dump("Profiled CFG");
  }

  // The set of translation passes and their order are determined by
  // the target.
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
  // This splits edges and appends new nodes to the end of the node
  // list.  This can invalidate iterators, so don't use an iterator.
  SizeT NumNodes = getNumNodes();
  for (SizeT I = 0; I < NumNodes; ++I)
    Nodes[I]->advancedPhiLowering();
}

// Find a reasonable placement for nodes that have not yet been
// placed, while maintaining the same relative ordering among already
// placed nodes.
void Cfg::reorderNodes() {
  typedef std::list<CfgNode *> PlacedList;
  PlacedList Placed;      // Nodes with relative placement locked down
  PlacedList Unreachable; // Unreachable nodes
  PlacedList::iterator NoPlace = Placed.end();
  // Keep track of where each node has been tentatively placed so that
  // we can manage insertions into the middle.
  std::vector<PlacedList::iterator> PlaceIndex(Nodes.size(), NoPlace);
  for (CfgNode *Node : Nodes) {
    // The "do ... while(0);" construct is to factor out the
    // --PlaceIndex and assert() statements before moving to the next
    // node.
    do {
      if (!Node->needsPlacement()) {
        // Add to the end of the Placed list.
        Placed.push_back(Node);
        PlaceIndex[Node->getIndex()] = Placed.end();
        continue;
      }
      Node->setNeedsPlacement(false);
      if (Node != getEntryNode() && Node->getInEdges().empty()) {
        // The node has essentially been deleted since it is not a
        // successor of any other node.
        Unreachable.push_back(Node);
        PlaceIndex[Node->getIndex()] = Unreachable.end();
        continue;
      }
      // Assume for now that the unplaced node is from edge-splitting
      // and therefore has 1 in-edge and 1 out-edge (actually, possibly
      // more than 1 in-edge if the predecessor node was contracted).
      // If this changes in the future, rethink the strategy.
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
      // It shouldn't be the case that PredPosition==NoPlace, but if
      // that somehow turns out to be true, we just insert Node before
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
  SizeT OldSize = Nodes.size();
  (void)OldSize;
  Nodes.clear();
  for (CfgNode *Node : Placed)
    Nodes.push_back(Node);
  for (CfgNode *Node : Unreachable)
    Nodes.push_back(Node);
  assert(Nodes.size() == OldSize);
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
  TimerMarker T(TimerStack::TT_doNopInsertion, this);
  for (CfgNode *Node : Nodes)
    Node->doNopInsertion();
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

// This is a lightweight version of live-range-end calculation.  Marks
// the last use of only those variables whose definition and uses are
// completely with a single block.  It is a quick single pass and
// doesn't need to iterate until convergence.
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
  // Make a final pass over each node to delete dead instructions,
  // collect the first and last instruction numbers, and add live
  // range segments for that node.
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
      // Special treatment for live in-args.  Their liveness needs to
      // extend beyond the beginning of the function, otherwise an arg
      // whose only use is in the first instruction will end up having
      // the trivial live range [2,2) and will *not* interfere with
      // other arguments.  So if the first instruction of the method
      // is "r=arg1+arg2", both args may be assigned the same
      // register.  This is accomplished by extending the entry
      // block's instruction range from [2,n) to [1,n) which will
      // transform the problematic [2,2) live ranges into [1,2).
      if (Node == getEntryNode()) {
        // TODO(stichnot): Make it a strict requirement that the entry
        // node gets the lowest instruction numbers, so that extending
        // the live range for in-args is guaranteed to work.
        FirstInstNum = Inst::NumberExtended;
      }
      Node->livenessAddIntervals(getLiveness(), FirstInstNum, LastInstNum);
    }
  }
}

// Traverse every Variable of every Inst and verify that it
// appears within the Variable's computed live range.
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
          const bool IsDest = true;
          if (!Dest->getLiveRange().containsValue(InstNumber, IsDest))
            Invalid = true;
          // Check that this instruction actually *begins* Dest's live
          // range, by checking that Dest is not live in the previous
          // instruction.  As a special exception, we don't check this
          // for the first instruction of the block, because a Phi
          // temporary may be live at the end of the previous block,
          // and if it is also assigned in the first instruction of
          // this block, the adjacent live ranges get merged.
          if (static_cast<class Inst *>(&Inst) != FirstInst &&
              !Inst.isDestNonKillable() &&
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
      for (SizeT I = 0; I < Inst.getSrcSize(); ++I) {
        Operand *Src = Inst.getSrc(I);
        SizeT NumVars = Src->getNumVars();
        for (SizeT J = 0; J < NumVars; ++J) {
          const Variable *Var = Src->getVar(J);
          const bool IsDest = false;
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
  }
  return Valid;
}

void Cfg::contractEmptyNodes() {
  // If we're decorating the asm output with register liveness info,
  // this information may become corrupted or incorrect after
  // contracting nodes that contain only redundant assignments.  As
  // such, we disable this pass when DecorateAsm is specified.  This
  // may make the resulting code look more branchy, but it should have
  // no effect on the register assignments.
  if (Ctx->getFlags().getDecorateAsm())
    return;
  for (CfgNode *Node : Nodes) {
    Node->contractIfEmpty();
  }
}

void Cfg::doBranchOpt() {
  TimerMarker T(TimerStack::TT_doBranchOpt, this);
  for (auto I = Nodes.begin(), E = Nodes.end(); I != E; ++I) {
    auto NextNode = I;
    ++NextNode;
    (*I)->doBranchOpt(NextNode == E ? nullptr : *NextNode);
  }
}

// ======================== Dump routines ======================== //

// emitTextHeader() is not target-specific (apart from what is
// abstracted by the Assembler), so it is defined here rather than in
// the target lowering class.
void Cfg::emitTextHeader(const IceString &MangledName, GlobalContext *Ctx,
                         const Assembler *Asm) {
  if (!ALLOW_DUMP)
    return;
  Ostream &Str = Ctx->getStrEmit();
  Str << "\t.text\n";
  if (Ctx->getFlags().getFunctionSections())
    Str << "\t.section\t.text." << MangledName << ",\"ax\",@progbits\n";
  if (!Asm->getInternal() || Ctx->getFlags().getDisableInternal()) {
    Str << "\t.globl\t" << MangledName << "\n";
    Str << "\t.type\t" << MangledName << ",%function\n";
  }
  Str << "\t" << Asm->getNonExecPadDirective() << " "
      << Asm->getBundleAlignLog2Bytes() << ",0x";
  for (uint8_t I : Asm->getNonExecBundlePadding())
    Str.write_hex(I);
  Str << "\n";
  Str << MangledName << ":\n";
}

void Cfg::emit() {
  if (!ALLOW_DUMP)
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
  IceString MangledName = getContext()->mangleName(getFunctionName());
  emitTextHeader(MangledName, Ctx, getAssembler<>());
  for (CfgNode *Node : Nodes)
    Node->emit(this);
  Str << "\n";
}

void Cfg::emitIAS() {
  TimerMarker T(TimerStack::TT_emit, this);
  // The emitIAS() routines emit into the internal assembler buffer,
  // so there's no need to lock the streams.
  for (CfgNode *Node : Nodes)
    Node->emitIAS(this);
}

// Dumps the IR with an optional introductory message.
void Cfg::dump(const IceString &Message) {
  if (!ALLOW_DUMP)
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
      Str << " weight=" << Var->getWeight() << " ";
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
