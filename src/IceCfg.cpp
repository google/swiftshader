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

namespace Ice {

Cfg::Cfg(GlobalContext *Ctx)
    : Ctx(Ctx), FunctionName(""), ReturnType(IceType_void),
      IsInternalLinkage(false), HasError(false), ErrorMessage(""), Entry(NULL),
      NextInstNumber(1), CurrentNode(NULL) {}

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

void Cfg::computePredecessors() {
  for (NodeList::iterator I = Nodes.begin(), E = Nodes.end(); I != E; ++I) {
    (*I)->computePredecessors();
  }
}

// ======================== Dump routines ======================== //

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
