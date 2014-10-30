//===- subzero/src/IceLiveness.h - Liveness analysis ------------*- C++ -*-===//
//
//                        The Subzero Code Generator
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares the Liveness and LivenessNode classes,
// which are used for liveness analysis.  The node-specific
// information tracked for each Variable includes whether it is
// live on entry, whether it is live on exit, the instruction number
// that starts its live range, and the instruction number that ends
// its live range.  At the Cfg level, the actual live intervals are
// recorded.
//
//===----------------------------------------------------------------------===//

#ifndef SUBZERO_SRC_ICELIVENESS_H
#define SUBZERO_SRC_ICELIVENESS_H

#include "IceDefs.h"
#include "IceTypes.h"

namespace Ice {

class LivenessNode {
  // TODO: Disable these constructors when Liveness::Nodes is no
  // longer an STL container.
  // LivenessNode(const LivenessNode &) = delete;
  // LivenessNode &operator=(const LivenessNode &) = delete;

public:
  LivenessNode() : NumLocals(0), NumNonDeadPhis(0) {}
  // NumLocals is the number of Variables local to this block.
  SizeT NumLocals;
  // NumNonDeadPhis tracks the number of Phi instructions that
  // Inst::liveness() identified as tentatively live.  If
  // NumNonDeadPhis changes from the last liveness pass, then liveness
  // has not yet converged.
  SizeT NumNonDeadPhis;
  // LiveToVarMap maps a liveness bitvector index to a Variable.  This
  // is generally just for printing/dumping.  The index should be less
  // than NumLocals + Liveness::NumGlobals.
  std::vector<Variable *> LiveToVarMap;
  // LiveIn and LiveOut track the in- and out-liveness of the global
  // variables.  The size of each vector is
  // LivenessNode::NumGlobals.
  LivenessBV LiveIn, LiveOut;
  // LiveBegin and LiveEnd track the instruction numbers of the start
  // and end of each variable's live range within this block.  The
  // index/key of each element is less than NumLocals +
  // Liveness::NumGlobals.
  LiveBeginEndMap LiveBegin, LiveEnd;
};

class Liveness {
  Liveness(const Liveness &) = delete;
  Liveness &operator=(const Liveness &) = delete;

public:
  Liveness(Cfg *Func, LivenessMode Mode)
      : Func(Func), Mode(Mode), NumGlobals(0) {}
  void init();
  Cfg *getFunc() const { return Func; }
  LivenessMode getMode() const { return Mode; }
  Variable *getVariable(SizeT LiveIndex, const CfgNode *Node) const;
  SizeT getLiveIndex(SizeT VarIndex) const { return VarToLiveMap[VarIndex]; }
  SizeT getNumGlobalVars() const { return NumGlobals; }
  SizeT getNumVarsInNode(const CfgNode *Node) const {
    return NumGlobals + Nodes[Node->getIndex()].NumLocals;
  }
  SizeT &getNumNonDeadPhis(const CfgNode *Node) {
    return Nodes[Node->getIndex()].NumNonDeadPhis;
  }
  LivenessBV &getLiveIn(const CfgNode *Node) {
    SizeT Index = Node->getIndex();
    resize(Index);
    return Nodes[Index].LiveIn;
  }
  LivenessBV &getLiveOut(const CfgNode *Node) {
    SizeT Index = Node->getIndex();
    resize(Index);
    return Nodes[Index].LiveOut;
  }
  LiveBeginEndMap *getLiveBegin(const CfgNode *Node) {
    SizeT Index = Node->getIndex();
    resize(Index);
    return &Nodes[Index].LiveBegin;
  }
  LiveBeginEndMap *getLiveEnd(const CfgNode *Node) {
    SizeT Index = Node->getIndex();
    resize(Index);
    return &Nodes[Index].LiveEnd;
  }

private:
  // Resize Nodes so that Nodes[Index] is valid.
  void resize(SizeT Index) {
    if (Index >= Nodes.size())
      Nodes.resize(Index + 1);
  }
  Cfg *Func;
  LivenessMode Mode;
  SizeT NumGlobals;
  // Size of Nodes is Cfg::Nodes.size().
  std::vector<LivenessNode> Nodes;
  // VarToLiveMap maps a Variable's Variable::Number to its live index
  // within its basic block.
  std::vector<SizeT> VarToLiveMap;
  // LiveToVarMap is analogous to LivenessNode::LiveToVarMap, but for
  // non-local variables.
  std::vector<Variable *> LiveToVarMap;
};

} // end of namespace Ice

#endif // SUBZERO_SRC_ICELIVENESS_H
