//===- subzero/src/IceLoopAnalyzer.h - Loop Analysis ------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief This analysis identifies loops in the CFG.
//===----------------------------------------------------------------------===//

#ifndef SUBZERO_SRC_ICELOOPANALYZER_H
#define SUBZERO_SRC_ICELOOPANALYZER_H

#include "IceDefs.h"

namespace Ice {

/// Analyze a function's CFG for loops. The CFG must not change during the
/// lifetime of this object.
class LoopAnalyzer {
  LoopAnalyzer() = delete;
  LoopAnalyzer(const LoopAnalyzer &) = delete;
  LoopAnalyzer &operator=(const LoopAnalyzer &) = delete;

public:
  explicit LoopAnalyzer(Cfg *Func);

  /// Use Tarjan's strongly connected components algorithm to identify outermost
  /// to innermost loops. By deleting the head of the loop from the graph, inner
  /// loops can be found. This assumes that the head node is not shared between
  /// loops but instead all paths to the head come from 'continue' constructs.
  ///
  /// This only computes the loop nest depth within the function and does not
  /// take into account whether the function was called from within a loop.
  // TODO(ascull): this currently uses a extension of Tarjan's algorithm with
  // is bounded linear. ncbray suggests another algorithm which is linear in
  // practice but not bounded linear. I think it also finds dominators.
  // http://lenx.100871.net/papers/loop-SAS.pdf
  CfgUnorderedMap<SizeT, CfgVector<SizeT>> getLoopInfo() { return Loops; }

private:
  void computeLoopNestDepth();
  using IndexT = uint32_t;
  static constexpr IndexT UndefinedIndex = 0;
  static constexpr IndexT FirstDefinedIndex = 1;

  // TODO(ascull): classify the other fields
  class LoopNode {
    LoopNode() = delete;
    LoopNode operator=(const LoopNode &) = delete;

  public:
    explicit LoopNode(CfgNode *BB) : BB(BB) { reset(); }
    LoopNode(const LoopNode &) = default;

    void reset();

    NodeList::const_iterator successorsEnd() const;
    NodeList::const_iterator currentSuccessor() const { return Succ; }
    void nextSuccessor() { ++Succ; }

    void visit(IndexT VisitIndex) { Index = LowLink = VisitIndex; }
    bool isVisited() const { return Index != UndefinedIndex; }
    IndexT getIndex() const { return Index; }

    void tryLink(IndexT NewLink) {
      if (NewLink < LowLink)
        LowLink = NewLink;
    }
    IndexT getLowLink() const { return LowLink; }

    void setOnStack(bool NewValue = true) { OnStack = NewValue; }
    bool isOnStack() const { return OnStack; }

    void setDeleted() { Deleted = true; }
    bool isDeleted() const { return Deleted; }

    void incrementLoopNestDepth();
    bool hasSelfEdge() const;

    CfgNode *getNode() { return BB; }

  private:
    CfgNode *BB;
    NodeList::const_iterator Succ;
    IndexT Index;
    IndexT LowLink;
    bool OnStack;
    bool Deleted = false;
  };

  using LoopNodeList = CfgVector<LoopNode>;
  using LoopNodePtrList = CfgVector<LoopNode *>;

  /// Process the node as part as part of Tarjan's algorithm and return either a
  /// node to recurse into or nullptr when the node has been fully processed.
  LoopNode *processNode(LoopNode &Node);

  /// The function to analyze for loops.
  Cfg *const Func;
  /// A list of decorated nodes in the same order as Func->getNodes() which
  /// means the node's index will also be valid in this list.
  LoopNodeList AllNodes;
  /// This is used as a replacement for the call stack.
  LoopNodePtrList WorkStack;
  /// Track which loop a node belongs to.
  LoopNodePtrList LoopStack;
  /// The index to assign to the next visited node.
  IndexT NextIndex = FirstDefinedIndex;
  /// The number of nodes which have been marked deleted. This is used to track
  /// when the iteration should end.
  LoopNodePtrList::size_type NumDeletedNodes = 0;
  /// Detailed loop information
  CfgUnorderedMap<SizeT, CfgVector<SizeT>> Loops;
};

} // end of namespace Ice

#endif //  SUBZERO_SRC_ICELOOPANALYZER_H
