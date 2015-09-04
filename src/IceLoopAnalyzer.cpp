//===- subzero/src/IceLoopAnalyzer.cpp - Loop Analysis --------------------===//
//
//                        The Subzero Code Generator
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file implements the loop analysis on the CFG.
///
//===----------------------------------------------------------------------===//
#include "IceLoopAnalyzer.h"

#include "IceCfg.h"
#include "IceCfgNode.h"

namespace Ice {

void LoopAnalyzer::LoopNode::reset() {
  if (Deleted)
    return;
  Succ = BB->getOutEdges().begin();
  Index = LowLink = UndefinedIndex;
  OnStack = false;
}

NodeList::const_iterator LoopAnalyzer::LoopNode::successorsEnd() const {
  return BB->getOutEdges().end();
}

void LoopAnalyzer::LoopNode::incrementLoopNestDepth() {
  BB->incrementLoopNestDepth();
}

LoopAnalyzer::LoopAnalyzer(Cfg *Fn) : Func(Fn) {
  const NodeList &Nodes = Func->getNodes();

  // Allocate memory ahead of time. This is why a vector is used instead of a
  // stack which doesn't support reserving (or bulk erasure used below).
  AllNodes.reserve(Nodes.size());
  WorkStack.reserve(Nodes.size());
  LoopStack.reserve(Nodes.size());

  // Create the LoopNodes from the function's CFG
  for (CfgNode *Node : Nodes)
    AllNodes.emplace_back(Node);
}

void LoopAnalyzer::computeLoopNestDepth() {
  assert(AllNodes.size() == Func->getNodes().size());
  assert(NextIndex == FirstDefinedIndex);
  assert(NumDeletedNodes == 0);

  while (NumDeletedNodes < AllNodes.size()) {
    // Prepare to run Tarjan's
    for (LoopNode &Node : AllNodes)
      Node.reset();

    assert(WorkStack.empty());
    assert(LoopStack.empty());

    for (LoopNode &Node : AllNodes) {
      if (Node.isDeleted() || Node.isVisited())
        continue;

      WorkStack.push_back(&Node);

      while (!WorkStack.empty()) {
        LoopNode &WorkNode = *WorkStack.back();
        if (LoopNode *Succ = processNode(WorkNode))
          WorkStack.push_back(Succ);
        else
          WorkStack.pop_back();
      }
    }
  }
}

LoopAnalyzer::LoopNode *
LoopAnalyzer::processNode(LoopAnalyzer::LoopNode &Node) {
  if (!Node.isVisited()) {
    Node.visit(NextIndex++);
    LoopStack.push_back(&Node);
    Node.setOnStack();
  } else {
    // Returning to a node after having recursed into Succ so continue
    // iterating through successors after using the Succ.LowLink value that was
    // computed in the recursion.
    LoopNode &Succ = AllNodes[(*Node.currentSuccessor())->getIndex()];
    Node.tryLink(Succ.getLowLink());
    Node.nextSuccessor();
  }

  // Visit the successors and recurse into unvisited nodes. The recursion could
  // cause the iteration to be suspended but it will resume as the stack is
  // unwound.
  auto SuccEnd = Node.successorsEnd();
  for (; Node.currentSuccessor() != SuccEnd; Node.nextSuccessor()) {
    LoopNode &Succ = AllNodes[(*Node.currentSuccessor())->getIndex()];

    if (Succ.isDeleted())
      continue;

    if (!Succ.isVisited())
      return &Succ;
    else if (Succ.isOnStack())
      Node.tryLink(Succ.getIndex());
  }

  if (Node.getLowLink() != Node.getIndex())
    return nullptr;

  // Single node means no loop in the CFG
  if (LoopStack.back() == &Node) {
    LoopStack.back()->setOnStack(false);
    LoopStack.back()->setDeleted();
    ++NumDeletedNodes;
    LoopStack.pop_back();
    return nullptr;
  }

  // Reaching here means a loop has been found! It consists of the nodes on
  // the top of the stack, down until the current node being processed, Node,
  // is found.
  for (auto It = LoopStack.rbegin(); It != LoopStack.rend(); ++It) {
    (*It)->setOnStack(false);
    (*It)->incrementLoopNestDepth();
    // Remove the loop from the stack and delete the head node
    if (*It == &Node) {
      (*It)->setDeleted();
      ++NumDeletedNodes;
      LoopStack.erase(It.base() - 1, LoopStack.end());
      break;
    }
  }

  return nullptr;
}

} // end of namespace Ice
