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

namespace Ice {

class CfgNode {
public:
  static CfgNode *create(Cfg *Func, SizeT LabelIndex, IceString Name = "") {
    return new (Func->allocate<CfgNode>()) CfgNode(Func, LabelIndex, Name);
  }

  // Access the label number and name for this node.
  SizeT getIndex() const { return Number; }
  IceString getName() const;
  IceString getAsmName() const {
    return ".L" + Func->getFunctionName() + "$" + getName();
  }

  // The HasReturn flag indicates that this node contains a return
  // instruction and therefore needs an epilog.
  void setHasReturn() { HasReturn = true; }
  bool getHasReturn() const { return HasReturn; }

  // Access predecessor and successor edge lists.
  const NodeList &getInEdges() const { return InEdges; }
  const NodeList &getOutEdges() const { return OutEdges; }

  // Manage the instruction list.
  InstList &getInsts() { return Insts; }
  void appendInst(Inst *Inst);
  void renumberInstructions();

  // Add a predecessor edge to the InEdges list for each of this
  // node's successors.
  void computePredecessors();

  void placePhiLoads();
  void placePhiStores();
  void deletePhis();
  void doAddressOpt();
  void doNopInsertion();
  void genCode();
  void livenessLightweight();
  bool liveness(Liveness *Liveness);
  void livenessPostprocess(LivenessMode Mode, Liveness *Liveness);
  void emit(Cfg *Func) const;
  void dump(Cfg *Func) const;

private:
  CfgNode(Cfg *Func, SizeT LabelIndex, IceString Name);
  CfgNode(const CfgNode &) LLVM_DELETED_FUNCTION;
  CfgNode &operator=(const CfgNode &) LLVM_DELETED_FUNCTION;
  Cfg *const Func;
  const SizeT Number; // label index
  IceString Name;     // for dumping only
  bool HasReturn;     // does this block need an epilog?
  NodeList InEdges;   // in no particular order
  NodeList OutEdges;  // in no particular order
  PhiList Phis;       // unordered set of phi instructions
  InstList Insts;     // ordered list of non-phi instructions
};

} // end of namespace Ice

#endif // SUBZERO_SRC_ICECFGNODE_H
