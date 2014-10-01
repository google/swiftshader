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
      IsInternalLinkage(false), HasError(false), ErrorMessage(""), Entry(NULL),
      NextInstNumber(1), Live(nullptr),
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

Variable *Cfg::makeVariable(Type Ty, const IceString &Name) {
  return makeVariable<Variable>(Ty, Name);
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
  static TimerIdT IDtranslate = GlobalContext::getTimerID("translate");
  TimerMarker T(IDtranslate, getContext());

  dump("Initial CFG");

  // The set of translation passes and their order are determined by
  // the target.
  getTarget()->translate();

  dump("Final output");
}

void Cfg::computePredecessors() {
  for (NodeList::iterator I = Nodes.begin(), E = Nodes.end(); I != E; ++I) {
    (*I)->computePredecessors();
  }
}

void Cfg::renumberInstructions() {
  static TimerIdT IDrenumberInstructions =
      GlobalContext::getTimerID("renumberInstructions");
  TimerMarker T(IDrenumberInstructions, getContext());
  NextInstNumber = 1;
  for (NodeList::iterator I = Nodes.begin(), E = Nodes.end(); I != E; ++I) {
    (*I)->renumberInstructions();
  }
}

// placePhiLoads() must be called before placePhiStores().
void Cfg::placePhiLoads() {
  static TimerIdT IDplacePhiLoads = GlobalContext::getTimerID("placePhiLoads");
  TimerMarker T(IDplacePhiLoads, getContext());
  for (NodeList::iterator I = Nodes.begin(), E = Nodes.end(); I != E; ++I) {
    (*I)->placePhiLoads();
  }
}

// placePhiStores() must be called after placePhiLoads().
void Cfg::placePhiStores() {
  static TimerIdT IDplacePhiStores =
      GlobalContext::getTimerID("placePhiStores");
  TimerMarker T(IDplacePhiStores, getContext());
  for (NodeList::iterator I = Nodes.begin(), E = Nodes.end(); I != E; ++I) {
    (*I)->placePhiStores();
  }
}

void Cfg::deletePhis() {
  static TimerIdT IDdeletePhis = GlobalContext::getTimerID("deletePhis");
  TimerMarker T(IDdeletePhis, getContext());
  for (NodeList::iterator I = Nodes.begin(), E = Nodes.end(); I != E; ++I) {
    (*I)->deletePhis();
  }
}

void Cfg::doArgLowering() {
  static TimerIdT IDdoArgLowering = GlobalContext::getTimerID("doArgLowering");
  TimerMarker T(IDdoArgLowering, getContext());
  getTarget()->lowerArguments();
}

void Cfg::doAddressOpt() {
  static TimerIdT IDdoAddressOpt = GlobalContext::getTimerID("doAddressOpt");
  TimerMarker T(IDdoAddressOpt, getContext());
  for (NodeList::iterator I = Nodes.begin(), E = Nodes.end(); I != E; ++I) {
    (*I)->doAddressOpt();
  }
}

void Cfg::doNopInsertion() {
  static TimerIdT IDdoNopInsertion =
      GlobalContext::getTimerID("doNopInsertion");
  TimerMarker T(IDdoNopInsertion, getContext());
  for (NodeList::iterator I = Nodes.begin(), E = Nodes.end(); I != E; ++I) {
    (*I)->doNopInsertion();
  }
}

void Cfg::genCode() {
  static TimerIdT IDgenCode = GlobalContext::getTimerID("genCode");
  TimerMarker T(IDgenCode, getContext());
  for (NodeList::iterator I = Nodes.begin(), E = Nodes.end(); I != E; ++I) {
    (*I)->genCode();
  }
}

// Compute the stack frame layout.
void Cfg::genFrame() {
  static TimerIdT IDgenFrame = GlobalContext::getTimerID("genFrame");
  TimerMarker T(IDgenFrame, getContext());
  getTarget()->addProlog(Entry);
  // TODO: Consider folding epilog generation into the final
  // emission/assembly pass to avoid an extra iteration over the node
  // list.  Or keep a separate list of exit nodes.
  for (NodeList::iterator I = Nodes.begin(), E = Nodes.end(); I != E; ++I) {
    CfgNode *Node = *I;
    if (Node->getHasReturn())
      getTarget()->addEpilog(Node);
  }
}

// This is a lightweight version of live-range-end calculation.  Marks
// the last use of only those variables whose definition and uses are
// completely with a single block.  It is a quick single pass and
// doesn't need to iterate until convergence.
void Cfg::livenessLightweight() {
  static TimerIdT IDlivenessLightweight =
      GlobalContext::getTimerID("livenessLightweight");
  TimerMarker T(IDlivenessLightweight, getContext());
  getVMetadata()->init();
  for (NodeList::iterator I = Nodes.begin(), E = Nodes.end(); I != E; ++I) {
    (*I)->livenessLightweight();
  }
}

void Cfg::liveness(LivenessMode Mode) {
  static TimerIdT IDliveness = GlobalContext::getTimerID("liveness");
  TimerMarker T(IDliveness, getContext());
  Live.reset(new Liveness(this, Mode));
  getVMetadata()->init();
  Live->init();
  // Initialize with all nodes needing to be processed.
  llvm::BitVector NeedToProcess(Nodes.size(), true);
  while (NeedToProcess.any()) {
    // Iterate in reverse topological order to speed up convergence.
    for (NodeList::reverse_iterator I = Nodes.rbegin(), E = Nodes.rend();
         I != E; ++I) {
      CfgNode *Node = *I;
      if (NeedToProcess[Node->getIndex()]) {
        NeedToProcess[Node->getIndex()] = false;
        bool Changed = Node->liveness(getLiveness());
        if (Changed) {
          // If the beginning-of-block liveness changed since the last
          // iteration, mark all in-edges as needing to be processed.
          const NodeList &InEdges = Node->getInEdges();
          for (NodeList::const_iterator I1 = InEdges.begin(),
                                        E1 = InEdges.end();
               I1 != E1; ++I1) {
            CfgNode *Pred = *I1;
            NeedToProcess[Pred->getIndex()] = true;
          }
        }
      }
    }
  }
  if (Mode == Liveness_Intervals) {
    // Reset each variable's live range.
    for (VarList::const_iterator I = Variables.begin(), E = Variables.end();
         I != E; ++I) {
      if (Variable *Var = *I)
        Var->resetLiveRange();
    }
  }
  // Collect timing for just the portion that constructs the live
  // range intervals based on the end-of-live-range computation, for a
  // finer breakdown of the cost.
  // Make a final pass over instructions to delete dead instructions
  // and build each Variable's live range.
  static TimerIdT IDliveRange = GlobalContext::getTimerID("liveRange");
  TimerMarker T1(IDliveRange, getContext());
  for (NodeList::iterator I = Nodes.begin(), E = Nodes.end(); I != E; ++I) {
    (*I)->livenessPostprocess(Mode, getLiveness());
  }
  if (Mode == Liveness_Intervals) {
    // Special treatment for live in-args.  Their liveness needs to
    // extend beyond the beginning of the function, otherwise an arg
    // whose only use is in the first instruction will end up having
    // the trivial live range [1,1) and will *not* interfere with
    // other arguments.  So if the first instruction of the method is
    // "r=arg1+arg2", both args may be assigned the same register.
    for (SizeT I = 0; I < Args.size(); ++I) {
      Variable *Arg = Args[I];
      if (!Live->getLiveRange(Arg).isEmpty()) {
        // Add live range [-1,0) with weight 0.  TODO: Here and below,
        // use better symbolic constants along the lines of
        // Inst::NumberDeleted and Inst::NumberSentinel instead of -1
        // and 0.
        Live->addLiveRange(Arg, -1, 0, 0);
      }
      // Do the same for i64 args that may have been lowered into i32
      // Lo and Hi components.
      Variable *Lo = Arg->getLo();
      if (Lo && !Live->getLiveRange(Lo).isEmpty())
        Live->addLiveRange(Lo, -1, 0, 0);
      Variable *Hi = Arg->getHi();
      if (Hi && !Live->getLiveRange(Hi).isEmpty())
        Live->addLiveRange(Hi, -1, 0, 0);
    }
    // Copy Liveness::LiveRanges into individual variables.  TODO:
    // Remove Variable::LiveRange and redirect to
    // Liveness::LiveRanges.  TODO: make sure Variable weights
    // are applied properly.
    SizeT NumVars = Variables.size();
    for (SizeT i = 0; i < NumVars; ++i) {
      Variable *Var = Variables[i];
      Var->setLiveRange(Live->getLiveRange(Var));
      if (Var->getWeight().isInf())
        Var->setLiveRangeInfiniteWeight();
    }
    dump();
  }
}

// Traverse every Variable of every Inst and verify that it
// appears within the Variable's computed live range.
bool Cfg::validateLiveness() const {
  static TimerIdT IDvalidateLiveness =
      GlobalContext::getTimerID("validateLiveness");
  TimerMarker T(IDvalidateLiveness, getContext());
  bool Valid = true;
  Ostream &Str = Ctx->getStrDump();
  for (NodeList::const_iterator I1 = Nodes.begin(), E1 = Nodes.end(); I1 != E1;
       ++I1) {
    CfgNode *Node = *I1;
    InstList &Insts = Node->getInsts();
    for (InstList::const_iterator I2 = Insts.begin(), E2 = Insts.end();
         I2 != E2; ++I2) {
      Inst *Inst = *I2;
      if (Inst->isDeleted())
        continue;
      if (llvm::isa<InstFakeKill>(Inst))
        continue;
      InstNumberT InstNumber = Inst->getNumber();
      Variable *Dest = Inst->getDest();
      if (Dest) {
        // TODO: This instruction should actually begin Dest's live
        // range, so we could probably test that this instruction is
        // the beginning of some segment of Dest's live range.  But
        // this wouldn't work with non-SSA temporaries during
        // lowering.
        if (!Dest->getLiveRange().containsValue(InstNumber)) {
          Valid = false;
          Str << "Liveness error: inst " << Inst->getNumber() << " dest ";
          Dest->dump(this);
          Str << " live range " << Dest->getLiveRange() << "\n";
        }
      }
      for (SizeT I = 0; I < Inst->getSrcSize(); ++I) {
        Operand *Src = Inst->getSrc(I);
        SizeT NumVars = Src->getNumVars();
        for (SizeT J = 0; J < NumVars; ++J) {
          const Variable *Var = Src->getVar(J);
          if (!Var->getLiveRange().containsValue(InstNumber)) {
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

void Cfg::doBranchOpt() {
  static TimerIdT IDdoBranchOpt = GlobalContext::getTimerID("doBranchOpt");
  TimerMarker T(IDdoBranchOpt, getContext());
  for (NodeList::iterator I = Nodes.begin(), E = Nodes.end(); I != E; ++I) {
    NodeList::iterator NextNode = I;
    ++NextNode;
    (*I)->doBranchOpt(NextNode == E ? NULL : *NextNode);
  }
}

// ======================== Dump routines ======================== //

void Cfg::emit() {
  static TimerIdT IDemit = GlobalContext::getTimerID("emit");
  TimerMarker T(IDemit, getContext());
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
  if (!getInternal()) {
    Str << "\t.globl\t" << MangledName << "\n";
    Str << "\t.type\t" << MangledName << ",@function\n";
  }
  Str << "\t.p2align " << getTarget()->getBundleAlignLog2Bytes() << ",0x";
  llvm::ArrayRef<uint8_t> Pad = getTarget()->getNonExecBundlePadding();
  for (llvm::ArrayRef<uint8_t>::iterator I = Pad.begin(), E = Pad.end();
       I != E; ++I) {
    Str.write_hex(*I);
  }
  Str << "\n";
  for (NodeList::const_iterator I = Nodes.begin(), E = Nodes.end(); I != E;
       ++I) {
    (*I)->emit(this);
  }
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
    if (getInternal())
      Str << "internal ";
    Str << ReturnType << " @" << getFunctionName() << "(";
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
    for (VarList::const_iterator I = Variables.begin(), E = Variables.end();
         I != E; ++I) {
      Variable *Var = *I;
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
  for (NodeList::const_iterator I = Nodes.begin(), E = Nodes.end(); I != E;
       ++I) {
    (*I)->dump(this);
  }
  if (getContext()->isVerbose(IceV_Instructions)) {
    Str << "}\n";
  }
}

} // end of namespace Ice
