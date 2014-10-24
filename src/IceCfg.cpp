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

#include "IceCfg.h"
#include "IceCfgNode.h"
#include "IceClFlags.h"
#include "IceDefs.h"
#include "IceInst.h"
#include "IceLiveness.h"
#include "IceOperand.h"
#include "IceTargetLowering.h"

namespace Ice {

Cfg::Cfg(GlobalContext *Ctx)
    : Ctx(Ctx), FunctionName(""), ReturnType(IceType_void),
      IsInternalLinkage(false), HasError(false), FocusedTiming(false),
      ErrorMessage(""), Entry(NULL), NextInstNumber(1), Live(nullptr),
      Target(TargetLowering::createLowering(Ctx->getTargetArch(), this)),
      VMetadata(new VariablesMetadata(this)),
      TargetAssembler(
          TargetLowering::createAssembler(Ctx->getTargetArch(), this)),
      CurrentNode(NULL) {}

Cfg::~Cfg() {}

void Cfg::setError(const IceString &Message) {
  HasError = true;
  ErrorMessage = Message;
  Ctx->getStrDump() << "ICE translation error: " << ErrorMessage << "\n";
}

CfgNode *Cfg::makeNode(const IceString &Name) {
  SizeT LabelIndex = Nodes.size();
  CfgNode *Node = CfgNode::create(this, LabelIndex, Name);
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

void Cfg::translate() {
  if (hasError())
    return;
  const IceString &TimingFocusOn = getContext()->getFlags().TimingFocusOn;
  if (TimingFocusOn == "*" || TimingFocusOn == getFunctionName()) {
    setFocusedTiming();
    getContext()->resetTimer(GlobalContext::TSK_Default);
    getContext()->setTimerName(GlobalContext::TSK_Default, getFunctionName());
  }
  TimerMarker T(TimerStack::TT_translate, this);

  dump("Initial CFG");

  // The set of translation passes and their order are determined by
  // the target.
  getTarget()->translate();

  dump("Final output");
  if (getFocusedTiming())
    getContext()->dumpTimers();
}

void Cfg::computePredecessors() {
  for (CfgNode *Node : Nodes)
    Node->computePredecessors();
}

void Cfg::renumberInstructions() {
  TimerMarker T(TimerStack::TT_renumberInstructions, this);
  NextInstNumber = 1;
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
    // TODO(stichnot): Use llvm::make_range with LLVM 3.5.
    for (auto I = Nodes.rbegin(), E = Nodes.rend(); I != E; ++I) {
      CfgNode *Node = *I;
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
  // Collect timing for just the portion that constructs the live
  // range intervals based on the end-of-live-range computation, for a
  // finer breakdown of the cost.
  TimerMarker T1(TimerStack::TT_liveRange, this);
  // Make a final pass over instructions to delete dead instructions
  // and build each Variable's live range.
  for (CfgNode *Node : Nodes)
    Node->livenessPostprocess(Mode, getLiveness());
  if (Mode == Liveness_Intervals) {
    // Special treatment for live in-args.  Their liveness needs to
    // extend beyond the beginning of the function, otherwise an arg
    // whose only use is in the first instruction will end up having
    // the trivial live range [1,1) and will *not* interfere with
    // other arguments.  So if the first instruction of the method is
    // "r=arg1+arg2", both args may be assigned the same register.
    for (SizeT I = 0; I < Args.size(); ++I) {
      Variable *Arg = Args[I];
      if (!Arg->getLiveRange().isEmpty()) {
        // Add live range [-1,0) with weight 0.  TODO: Here and below,
        // use better symbolic constants along the lines of
        // Inst::NumberDeleted and Inst::NumberSentinel instead of -1
        // and 0.
        Arg->addLiveRange(-1, 0, 0);
      }
      // Do the same for i64 args that may have been lowered into i32
      // Lo and Hi components.
      Variable *Lo = Arg->getLo();
      if (Lo && !Lo->getLiveRange().isEmpty())
        Lo->addLiveRange(-1, 0, 0);
      Variable *Hi = Arg->getHi();
      if (Hi && !Hi->getLiveRange().isEmpty())
        Hi->addLiveRange(-1, 0, 0);
    }
  }
}

// Traverse every Variable of every Inst and verify that it
// appears within the Variable's computed live range.
bool Cfg::validateLiveness() const {
  TimerMarker T(TimerStack::TT_validateLiveness, this);
  bool Valid = true;
  Ostream &Str = Ctx->getStrDump();
  for (CfgNode *Node : Nodes) {
    Inst *FirstInst = NULL;
    for (Inst *Inst : Node->getInsts()) {
      if (Inst->isDeleted())
        continue;
      if (llvm::isa<InstFakeKill>(Inst))
        continue;
      if (FirstInst == NULL)
        FirstInst = Inst;
      InstNumberT InstNumber = Inst->getNumber();
      if (Variable *Dest = Inst->getDest()) {
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
          if (Inst != FirstInst && !Inst->isDestNonKillable() &&
              Dest->getLiveRange().containsValue(InstNumber - 1, IsDest))
            Invalid = true;
          if (Invalid) {
            Valid = false;
            Str << "Liveness error: inst " << Inst->getNumber() << " dest ";
            Dest->dump(this);
            Str << " live range " << Dest->getLiveRange() << "\n";
          }
        }
      }
      for (SizeT I = 0; I < Inst->getSrcSize(); ++I) {
        Operand *Src = Inst->getSrc(I);
        SizeT NumVars = Src->getNumVars();
        for (SizeT J = 0; J < NumVars; ++J) {
          const Variable *Var = Src->getVar(J);
          const bool IsDest = false;
          if (!Var->getIgnoreLiveness() &&
              !Var->getLiveRange().containsValue(InstNumber, IsDest)) {
            Valid = false;
            Str << "Liveness error: inst " << Inst->getNumber() << " var ";
            Var->dump(this);
            Str << " live range " << Var->getLiveRange() << "\n";
          }
        }
      }
    }
  }
  return Valid;
}

// Deletes redundant assignments like "var=var".  This includes
// architecturally redundant moves like "var1:eax=var2:eax".  As such,
// this needs to be done very late in the translation to avoid
// liveness inconsistencies.
void Cfg::deleteRedundantAssignments() {
  for (CfgNode *Node : Nodes) {
    // Ignore Phi instructions.
    for (Inst *I : Node->getInsts())
      if (I->isRedundantAssign())
        I->setDeleted();
  }
}

void Cfg::doBranchOpt() {
  TimerMarker T(TimerStack::TT_doBranchOpt, this);
  for (auto I = Nodes.begin(), E = Nodes.end(); I != E; ++I) {
    auto NextNode = I;
    ++NextNode;
    (*I)->doBranchOpt(NextNode == E ? NULL : *NextNode);
  }
}

// ======================== Dump routines ======================== //

void Cfg::emit() {
  TimerMarker T(TimerStack::TT_emit, this);
  Ostream &Str = Ctx->getStrEmit();
  if (!Ctx->testAndSetHasEmittedFirstMethod()) {
    // Print a helpful command for assembling the output.
    // TODO: have the Target emit the header
    // TODO: need a per-file emit in addition to per-CFG
    Str << "# $LLVM_BIN_PATH/llvm-mc"
        << " -arch=x86"
        << " -x86-asm-syntax=intel"
        << " -filetype=obj"
        << " -o=MyObj.o"
        << "\n\n";
  }
  Str << "\t.text\n";
  IceString MangledName = getContext()->mangleName(getFunctionName());
  if (Ctx->getFlags().FunctionSections)
    Str << "\t.section\t.text." << MangledName << ",\"ax\",@progbits\n";
  if (!getInternal() || Ctx->getFlags().DisableInternal) {
    Str << "\t.globl\t" << MangledName << "\n";
    Str << "\t.type\t" << MangledName << ",@function\n";
  }
  Str << "\t.p2align " << getTarget()->getBundleAlignLog2Bytes() << ",0x";
  for (AsmCodeByte I : getTarget()->getNonExecBundlePadding())
    Str.write_hex(I);
  Str << "\n";
  for (CfgNode *Node : Nodes)
    Node->emit(this);
  Str << "\n";
}

// Dumps the IR with an optional introductory message.
void Cfg::dump(const IceString &Message) {
  if (!Ctx->isVerbose())
    return;
  Ostream &Str = Ctx->getStrDump();
  if (!Message.empty())
    Str << "================ " << Message << " ================\n";
  setCurrentNode(getEntryNode());
  // Print function name+args
  if (getContext()->isVerbose(IceV_Instructions)) {
    Str << "define ";
    if (getInternal() && !Ctx->getFlags().DisableInternal)
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
  if (getContext()->isVerbose(IceV_Liveness)) {
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
  if (getContext()->isVerbose(IceV_Instructions))
    Str << "}\n";
}

} // end of namespace Ice
