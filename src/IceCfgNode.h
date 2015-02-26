//===- subzero/src/IceCfgNode.h - Control flow graph node -------*- C++ -*-===//
//
//                        The Subzero Code Generator
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares the CfgNode class, which represents a single
// basic block as its instruction list, in-edge list, and out-edge
// list.
//
//===----------------------------------------------------------------------===//

#ifndef SUBZERO_SRC_ICECFGNODE_H
#define SUBZERO_SRC_ICECFGNODE_H

#include "IceDefs.h"
#include "IceInst.h" // InstList traits

namespace Ice {

class CfgNode {
  CfgNode() = delete;
  CfgNode(const CfgNode &) = delete;
  CfgNode &operator=(const CfgNode &) = delete;

public:
  static CfgNode *create(Cfg *Func, SizeT LabelIndex) {
    return new (Func->allocate<CfgNode>()) CfgNode(Func, LabelIndex);
  }

  // Access the label number and name for this node.
  SizeT getIndex() const { return Number; }
  IceString getName() const;
  void setName(const IceString &NewName) {
    // Make sure that the name can only be set once.
    assert(NameIndex == Cfg::IdentifierIndexInvalid);
    if (!NewName.empty())
      NameIndex = Func->addIdentifierName(NewName);
  }
  IceString getAsmName() const {
    return ".L" + Func->getFunctionName() + "$" + getName();
  }

  // The HasReturn flag indicates that this node contains a return
  // instruction and therefore needs an epilog.
  void setHasReturn() { HasReturn = true; }
  bool getHasReturn() const { return HasReturn; }

  void setNeedsPlacement(bool Value) { NeedsPlacement = Value; }
  bool needsPlacement() const { return NeedsPlacement; }

  // Access predecessor and successor edge lists.
  const NodeList &getInEdges() const { return InEdges; }
  const NodeList &getOutEdges() const { return OutEdges; }

  // Manage the instruction list.
  InstList &getInsts() { return Insts; }
  PhiList &getPhis() { return Phis; }
  void appendInst(Inst *Inst);
  void renumberInstructions();
  // Rough and generally conservative estimate of the number of
  // instructions in the block.  It is updated when an instruction is
  // added, but not when deleted.  It is recomputed during
  // renumberInstructions().
  InstNumberT getInstCountEstimate() const { return InstCountEstimate; }

  // Add a predecessor edge to the InEdges list for each of this
  // node's successors.
  void computePredecessors();
  CfgNode *splitIncomingEdge(CfgNode *Pred, SizeT InEdgeIndex);

  void placePhiLoads();
  void placePhiStores();
  void deletePhis();
  void advancedPhiLowering();
  void doAddressOpt();
  void doNopInsertion();
  void genCode();
  void livenessLightweight();
  bool liveness(Liveness *Liveness);
  void livenessAddIntervals(Liveness *Liveness, InstNumberT FirstInstNum,
                            InstNumberT LastInstNum);
  void contractIfEmpty();
  void doBranchOpt(const CfgNode *NextNode);
  void emit(Cfg *Func) const;
  void emitIAS(Cfg *Func) const;
  void dump(Cfg *Func) const;

private:
  CfgNode(Cfg *Func, SizeT LabelIndex);
  Cfg *const Func;
  const SizeT Number;                 // label index
  Cfg::IdentifierIndexType NameIndex; // index into Cfg::NodeNames table
  bool HasReturn;                     // does this block need an epilog?
  bool NeedsPlacement;
  InstNumberT InstCountEstimate; // rough instruction count estimate
  NodeList InEdges;              // in no particular order
  NodeList OutEdges;             // in no particular order
  PhiList Phis;                  // unordered set of phi instructions
  InstList Insts;                // ordered list of non-phi instructions
};

} // end of namespace Ice

#endif // SUBZERO_SRC_ICECFGNODE_H
