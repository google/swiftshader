//===- subzero/src/IceLiveness.cpp - Liveness analysis implementation -----===//
//
//                        The Subzero Code Generator
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file provides some of the support for the Liveness class.  In
// particular, it handles the sparsity representation of the mapping
// between Variables and CfgNodes.  The idea is that since most
// variables are used only within a single basic block, we can
// partition the variables into "local" and "global" sets.  Instead of
// sizing and indexing vectors according to Variable::Number, we
// create a mapping such that global variables are mapped to low
// indexes that are common across nodes, and local variables are
// mapped to a higher index space that is shared across nodes.
//
//===----------------------------------------------------------------------===//

#include "IceCfg.h"
#include "IceCfgNode.h"
#include "IceDefs.h"
#include "IceInst.h"
#include "IceLiveness.h"
#include "IceOperand.h"

namespace Ice {

void Liveness::init() {
  // Initialize most of the container sizes.
  SizeT NumVars = Func->getVariables().size();
  SizeT NumNodes = Func->getNumNodes();
  VariablesMetadata *VMetadata = Func->getVMetadata();
  Nodes.resize(NumNodes);
  VarToLiveMap.resize(NumVars);

  // Count the number of globals, and the number of locals for each
  // block.
  for (SizeT i = 0; i < NumVars; ++i) {
    Variable *Var = Func->getVariables()[i];
    if (VMetadata->isMultiBlock(Var)) {
      ++NumGlobals;
    } else {
      SizeT Index = VMetadata->getLocalUseNode(Var)->getIndex();
      ++Nodes[Index].NumLocals;
    }
  }

  // Resize each LivenessNode::LiveToVarMap, and the global
  // LiveToVarMap.  Reset the counts to 0.
  for (SizeT i = 0; i < NumNodes; ++i) {
    Nodes[i].LiveToVarMap.assign(Nodes[i].NumLocals, NULL);
    Nodes[i].NumLocals = 0;
  }
  LiveToVarMap.assign(NumGlobals, NULL);

  // Sort each variable into the appropriate LiveToVarMap.  Also set
  // VarToLiveMap.
  SizeT TmpNumGlobals = 0;
  for (SizeT i = 0; i < NumVars; ++i) {
    Variable *Var = Func->getVariables()[i];
    SizeT VarIndex = Var->getIndex();
    SizeT LiveIndex;
    if (VMetadata->isMultiBlock(Var)) {
      LiveIndex = TmpNumGlobals++;
      LiveToVarMap[LiveIndex] = Var;
    } else {
      SizeT NodeIndex = VMetadata->getLocalUseNode(Var)->getIndex();
      LiveIndex = Nodes[NodeIndex].NumLocals++;
      Nodes[NodeIndex].LiveToVarMap[LiveIndex] = Var;
      LiveIndex += NumGlobals;
    }
    VarToLiveMap[VarIndex] = LiveIndex;
  }
  assert(NumGlobals == TmpNumGlobals);

  // Process each node.
  for (const CfgNode *LNode : Func->getNodes()) {
    LivenessNode &Node = Nodes[LNode->getIndex()];
    // NumLocals, LiveToVarMap already initialized
    Node.LiveIn.resize(NumGlobals);
    Node.LiveOut.resize(NumGlobals);
    // LiveBegin and LiveEnd are reinitialized before each pass over
    // the block.
  }
}

Variable *Liveness::getVariable(SizeT LiveIndex, const CfgNode *Node) const {
  if (LiveIndex < NumGlobals)
    return LiveToVarMap[LiveIndex];
  SizeT NodeIndex = Node->getIndex();
  return Nodes[NodeIndex].LiveToVarMap[LiveIndex - NumGlobals];
}

} // end of namespace Ice
