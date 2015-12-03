//===- subzero/src/IceLiveness.h - Liveness analysis ------------*- C++ -*-===//
//
//                        The Subzero Code Generator
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief Declares the Liveness and LivenessNode classes, which are used for
/// liveness analysis.
///
/// The node-specific information tracked for each Variable includes whether it
/// is live on entry, whether it is live on exit, the instruction number that
/// starts its live range, and the instruction number that ends its live range.
/// At the Cfg level, the actual live intervals are recorded.
///
//===----------------------------------------------------------------------===//

#ifndef SUBZERO_SRC_ICELIVENESS_H
#define SUBZERO_SRC_ICELIVENESS_H

#include "IceCfgNode.h"
#include "IceDefs.h"
#include "IceTypes.h"

namespace Ice {

class Liveness {
  Liveness() = delete;
  Liveness(const Liveness &) = delete;
  Liveness &operator=(const Liveness &) = delete;

  class LivenessNode {
    LivenessNode &operator=(const LivenessNode &) = delete;

  public:
    LivenessNode() = default;
    LivenessNode(const LivenessNode &) = default;
    /// NumLocals is the number of Variables local to this block.
    SizeT NumLocals = 0;
    /// NumNonDeadPhis tracks the number of Phi instructions that
    /// Inst::liveness() identified as tentatively live. If NumNonDeadPhis
    /// changes from the last liveness pass, then liveness has not yet
    /// converged.
    SizeT NumNonDeadPhis = 0;
    // LiveToVarMap maps a liveness bitvector index to a Variable. This is
    // generally just for printing/dumping. The index should be less than
    // NumLocals + Liveness::NumGlobals.
    CfgVector<Variable *> LiveToVarMap;
    // LiveIn and LiveOut track the in- and out-liveness of the global
    // variables. The size of each vector is LivenessNode::NumGlobals.
    LivenessBV LiveIn, LiveOut;
    // LiveBegin and LiveEnd track the instruction numbers of the start and end
    // of each variable's live range within this block. The index/key of each
    // element is less than NumLocals + Liveness::NumGlobals.
    LiveBeginEndMap LiveBegin, LiveEnd;
  };

public:
  Liveness(Cfg *Func, LivenessMode Mode) : Func(Func), Mode(Mode) {}
  void init();
  void initPhiEdgeSplits(NodeList::const_iterator FirstNode,
                         VarList::const_iterator FirstVar);
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
  bool getRangeMask(SizeT Index) const { return RangeMask[Index]; }

private:
  void initInternal(NodeList::const_iterator FirstNode,
                    VarList::const_iterator FirstVar, bool IsFullInit);
  /// Resize Nodes so that Nodes[Index] is valid.
  void resize(SizeT Index) {
    if (Index >= Nodes.size())
      Nodes.resize(Index + 1);
  }
  Cfg *Func;
  LivenessMode Mode;
  SizeT NumGlobals = 0;
  /// Size of Nodes is Cfg::Nodes.size().
  CfgVector<LivenessNode> Nodes;
  /// VarToLiveMap maps a Variable's Variable::Number to its live index within
  /// its basic block.
  CfgVector<SizeT> VarToLiveMap;
  /// LiveToVarMap is analogous to LivenessNode::LiveToVarMap, but for non-local
  /// variables.
  CfgVector<Variable *> LiveToVarMap;
  /// RangeMask[Variable::Number] indicates whether we want to track that
  /// Variable's live range.
  llvm::BitVector RangeMask;
};

} // end of namespace Ice

#endif // SUBZERO_SRC_ICELIVENESS_H
