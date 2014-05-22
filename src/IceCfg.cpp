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
#include "IceDefs.h"
#include "IceInst.h"
#include "IceOperand.h"
#include "IceTargetLowering.h"

namespace Ice {

Cfg::Cfg(GlobalContext *Ctx)
    : Ctx(Ctx), FunctionName(""), ReturnType(IceType_void),
      IsInternalLinkage(false), HasError(false), ErrorMessage(""), Entry(NULL),
      NextInstNumber(1),
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
  Ostream &Str = Ctx->getStrDump();
  if (hasError())
    return;

  if (Ctx->isVerbose()) {
    Str << "================ Initial CFG ================\n";
    dump();
  }

  Timer T_translate;
  // The set of translation passes and their order are determined by
  // the target.
  getTarget()->translate();
  T_translate.printElapsedUs(getContext(), "translate()");

  if (Ctx->isVerbose()) {
    Str << "================ Final output ================\n";
    dump();
  }
}

void Cfg::computePredecessors() {
  for (NodeList::iterator I = Nodes.begin(), E = Nodes.end(); I != E; ++I) {
    (*I)->computePredecessors();
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
  if (!getInternal()) {
    IceString MangledName = getContext()->mangleName(getFunctionName());
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

void Cfg::dump() {
  Ostream &Str = Ctx->getStrDump();
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
