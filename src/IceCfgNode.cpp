//===- subzero/src/IceCfgNode.cpp - Basic block (node) implementation -----===//
//
//                        The Subzero Code Generator
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements the CfgNode class, including the
// complexities of instruction insertion and in-edge calculation.
//
//===----------------------------------------------------------------------===//

#include "IceCfg.h"
#include "IceCfgNode.h"
#include "IceInst.h"
#include "IceOperand.h"

namespace Ice {

CfgNode::CfgNode(Cfg *Func, SizeT LabelNumber, IceString Name)
    : Func(Func), Number(LabelNumber), Name(Name) {}

// Returns the name the node was created with.  If no name was given,
// it synthesizes a (hopefully) unique name.
IceString CfgNode::getName() const {
  if (!Name.empty())
    return Name;
  char buf[30];
  snprintf(buf, llvm::array_lengthof(buf), "__%u", getIndex());
  return buf;
}

// Adds an instruction to either the Phi list or the regular
// instruction list.  Validates that all Phis are added before all
// regular instructions.
void CfgNode::appendInst(Inst *Inst) {
  if (InstPhi *Phi = llvm::dyn_cast<InstPhi>(Inst)) {
    if (!Insts.empty()) {
      Func->setError("Phi instruction added to the middle of a block");
      return;
    }
    Phis.push_back(Phi);
  } else {
    Insts.push_back(Inst);
  }
  Inst->updateVars(this);
}

// When a node is created, the OutEdges are immediately knows, but the
// InEdges have to be built up incrementally.  After the CFG has been
// constructed, the computePredecessors() pass finalizes it by
// creating the InEdges list.
void CfgNode::computePredecessors() {
  OutEdges = (*Insts.rbegin())->getTerminatorEdges();
  for (NodeList::const_iterator I = OutEdges.begin(), E = OutEdges.end();
       I != E; ++I) {
    CfgNode *Node = *I;
    Node->InEdges.push_back(this);
  }
}

// ======================== Dump routines ======================== //

void CfgNode::dump(Cfg *Func) const {
  Func->setCurrentNode(this);
  Ostream &Str = Func->getContext()->getStrDump();
  if (Func->getContext()->isVerbose(IceV_Instructions)) {
    Str << getName() << ":\n";
  }
  // Dump list of predecessor nodes.
  if (Func->getContext()->isVerbose(IceV_Preds) && !InEdges.empty()) {
    Str << "    // preds = ";
    for (NodeList::const_iterator I = InEdges.begin(), E = InEdges.end();
         I != E; ++I) {
      if (I != InEdges.begin())
        Str << ", ";
      Str << "%" << (*I)->getName();
    }
    Str << "\n";
  }
  // Dump each instruction.
  if (Func->getContext()->isVerbose(IceV_Instructions)) {
    for (PhiList::const_iterator I = Phis.begin(), E = Phis.end(); I != E;
         ++I) {
      const Inst *Inst = *I;
      Inst->dumpDecorated(Func);
    }
    InstList::const_iterator I = Insts.begin(), E = Insts.end();
    while (I != E) {
      Inst *Inst = *I++;
      Inst->dumpDecorated(Func);
    }
  }
  // Dump list of successor nodes.
  if (Func->getContext()->isVerbose(IceV_Succs)) {
    Str << "    // succs = ";
    for (NodeList::const_iterator I = OutEdges.begin(), E = OutEdges.end();
         I != E; ++I) {
      if (I != OutEdges.begin())
        Str << ", ";
      Str << "%" << (*I)->getName();
    }
    Str << "\n";
  }
}

} // end of namespace Ice
