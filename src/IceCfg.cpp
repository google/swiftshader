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
      NextInstNumber(1), Live(NULL),
      Target(TargetLowering::createLowering(Ctx->getTargetArch(), this)),
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

// Create a new Variable with a particular type and an optional
// name.  The Node argument is the node where the variable is defined.
Variable *Cfg::makeVariable(Type Ty, const CfgNode *Node,
                            const IceString &Name) {
  SizeT Index = Variables.size();
  Variables.push_back(Variable::create(this, Ty, Node, Index, Name));
  return Variables[Index];
}

void Cfg::addArg(Variable *Arg) {
  Arg->setIsArg(this);
  Args.push_back(Arg);
}

// Returns whether the stack frame layout has been computed yet.  This
// is used for dumping the stack frame location of Variables.
bool Cfg::hasComputedFrame() const { return getTarget()->hasComputedFrame(); }

void Cfg::translate() {
  if (hasError())
    return;

  dump("Initial CFG");

  Timer T_translate;
  // The set of translation passes and their order are determined by
  // the target.
  getTarget()->translate();
  T_translate.printElapsedUs(getContext(), "translate()");

  dump("Final output");
}

void Cfg::computePredecessors() {
  for (NodeList::iterator I = Nodes.begin(), E = Nodes.end(); I != E; ++I) {
    (*I)->computePredecessors();
  }
}

void Cfg::renumberInstructions() {
  NextInstNumber = 1;
  for (NodeList::iterator I = Nodes.begin(), E = Nodes.end(); I != E; ++I) {
    (*I)->renumberInstructions();
  }
}

// placePhiLoads() must be called before placePhiStores().
void Cfg::placePhiLoads() {
  for (NodeList::iterator I = Nodes.begin(), E = Nodes.end(); I != E; ++I) {
    (*I)->placePhiLoads();
  }
}

// placePhiStores() must be called after placePhiLoads().
void Cfg::placePhiStores() {
  for (NodeList::iterator I = Nodes.begin(), E = Nodes.end(); I != E; ++I) {
    (*I)->placePhiStores();
  }
}

void Cfg::deletePhis() {
  for (NodeList::iterator I = Nodes.begin(), E = Nodes.end(); I != E; ++I) {
    (*I)->deletePhis();
  }
}

void Cfg::doArgLowering() {
  getTarget()->lowerArguments();
}

void Cfg::doAddressOpt() {
  for (NodeList::iterator I = Nodes.begin(), E = Nodes.end(); I != E; ++I) {
    (*I)->doAddressOpt();
  }
}

void Cfg::doNopInsertion() {
  for (NodeList::iterator I = Nodes.begin(), E = Nodes.end(); I != E; ++I) {
    (*I)->doNopInsertion();
  }
}

void Cfg::genCode() {
  for (NodeList::iterator I = Nodes.begin(), E = Nodes.end(); I != E; ++I) {
    (*I)->genCode();
  }
}

// Compute the stack frame layout.
void Cfg::genFrame() {
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
  for (NodeList::iterator I = Nodes.begin(), E = Nodes.end(); I != E; ++I) {
    (*I)->livenessLightweight();
  }
}

void Cfg::liveness(LivenessMode Mode) {
  Live.reset(new Liveness(this, Mode));
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
  Timer T_liveRange;
  // Make a final pass over instructions to delete dead instructions
  // and build each Variable's live range.
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
    T_liveRange.printElapsedUs(getContext(), "live range construction");
    dump();
  }
}

// Traverse every Variable of every Inst and verify that it
// appears within the Variable's computed live range.
bool Cfg::validateLiveness() const {
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

// ======================== Dump routines ======================== //

void Cfg::emit() {
  Ostream &Str = Ctx->getStrEmit();
  Timer T_emit;
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
  for (NodeList::const_iterator I = Nodes.begin(), E = Nodes.end(); I != E;
       ++I) {
    (*I)->emit(this);
  }
  Str << "\n";
  T_emit.printElapsedUs(Ctx, "emit()");
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
  setCurrentNode(NULL);
  if (getContext()->isVerbose(IceV_Liveness)) {
    // Print summary info about variables
    for (VarList::const_iterator I = Variables.begin(), E = Variables.end();
         I != E; ++I) {
      Variable *Var = *I;
      Str << "//"
          << " multiblock=" << Var->isMultiblockLife() << " "
          << " weight=" << Var->getWeight() << " ";
      Var->dump(this);
      if (Variable *Pref = Var->getPreferredRegister()) {
        Str << " pref=";
        Pref->dump(this);
        if (Var->getRegisterOverlap())
          Str << ",overlap";
        Str << " ";
      }
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
