//===- subzero/src/IceCfgNode.cpp - Basic block (node) implementation -----===//
//
//                        The Subzero Code Generator
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements the CfgNode class, including the complexities
// of instruction insertion and in-edge calculation.
//
//===----------------------------------------------------------------------===//

#include "IceCfg.h"
#include "IceCfgNode.h"
#include "IceInst.h"
#include "IceOperand.h"
#include "IceTargetLowering.h"

namespace Ice {

CfgNode::CfgNode(Cfg *Func, SizeT LabelNumber, IceString Name)
    : Func(Func), Number(LabelNumber), Name(Name), HasReturn(false) {}

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

// This does part 1 of Phi lowering, by creating a new dest variable
// for each Phi instruction, replacing the Phi instruction's dest with
// that variable, and adding an explicit assignment of the old dest to
// the new dest.  For example,
//   a=phi(...)
// changes to
//   "a_phi=phi(...); a=a_phi".
//
// This is in preparation for part 2 which deletes the Phi
// instructions and appends assignment instructions to predecessor
// blocks.  Note that this transformation preserves SSA form.
void CfgNode::placePhiLoads() {
  for (PhiList::iterator I = Phis.begin(), E = Phis.end(); I != E; ++I) {
    Inst *Inst = (*I)->lower(Func, this);
    Insts.insert(Insts.begin(), Inst);
    Inst->updateVars(this);
  }
}

// This does part 2 of Phi lowering.  For each Phi instruction at each
// out-edge, create a corresponding assignment instruction, and add
// all the assignments near the end of this block.  They need to be
// added before any branch instruction, and also if the block ends
// with a compare instruction followed by a branch instruction that we
// may want to fuse, it's better to insert the new assignments before
// the compare instruction.
//
// Note that this transformation takes the Phi dest variables out of
// SSA form, as there may be assignments to the dest variable in
// multiple blocks.
//
// TODO: Defer this pass until after register allocation, then split
// critical edges, add the assignments, and lower them.  This should
// reduce the amount of shuffling at the end of each block.
void CfgNode::placePhiStores() {
  // Find the insertion point.  TODO: After branch/compare fusing is
  // implemented, try not to insert Phi stores between the compare and
  // conditional branch instructions, otherwise the branch/compare
  // pattern matching may fail.  However, the branch/compare sequence
  // will have to be broken if the compare result is read (by the
  // assignment) before it is written (by the compare).
  InstList::iterator InsertionPoint = Insts.end();
  // Every block must end in a terminator instruction.
  assert(InsertionPoint != Insts.begin());
  --InsertionPoint;
  // Confirm via assert() that InsertionPoint is a terminator
  // instruction.  Calling getTerminatorEdges() on a non-terminator
  // instruction will cause an llvm_unreachable().
  assert(((*InsertionPoint)->getTerminatorEdges(), true));

  // Consider every out-edge.
  for (NodeList::const_iterator I1 = OutEdges.begin(), E1 = OutEdges.end();
       I1 != E1; ++I1) {
    CfgNode *Target = *I1;
    // Consider every Phi instruction at the out-edge.
    for (PhiList::const_iterator I2 = Target->Phis.begin(),
                                 E2 = Target->Phis.end();
         I2 != E2; ++I2) {
      Operand *Operand = (*I2)->getOperandForTarget(this);
      assert(Operand);
      Variable *Dest = (*I2)->getDest();
      assert(Dest);
      InstAssign *NewInst = InstAssign::create(Func, Dest, Operand);
      // If Src is a variable, set the Src and Dest variables to
      // prefer each other for register allocation.
      if (Variable *Src = llvm::dyn_cast<Variable>(Operand)) {
        bool AllowOverlap = false;
        Dest->setPreferredRegister(Src, AllowOverlap);
        Src->setPreferredRegister(Dest, AllowOverlap);
      }
      Insts.insert(InsertionPoint, NewInst);
      NewInst->updateVars(this);
    }
  }
}

// Deletes the phi instructions after the loads and stores are placed.
void CfgNode::deletePhis() {
  for (PhiList::iterator I = Phis.begin(), E = Phis.end(); I != E; ++I) {
    (*I)->setDeleted();
  }
}

// Drives the target lowering.  Passes the current instruction and the
// next non-deleted instruction for target lowering.
void CfgNode::genCode() {
  TargetLowering *Target = Func->getTarget();
  LoweringContext &Context = Target->getContext();
  // Lower only the regular instructions.  Defer the Phi instructions.
  Context.init(this);
  while (!Context.atEnd()) {
    InstList::iterator Orig = Context.getCur();
    if (llvm::isa<InstRet>(*Orig))
      setHasReturn();
    Target->lower();
    // Ensure target lowering actually moved the cursor.
    assert(Context.getCur() != Orig);
  }
}

// ======================== Dump routines ======================== //

void CfgNode::emit(Cfg *Func) const {
  Func->setCurrentNode(this);
  Ostream &Str = Func->getContext()->getStrEmit();
  if (Func->getEntryNode() == this) {
    Str << Func->getContext()->mangleName(Func->getFunctionName()) << ":\n";
  }
  Str << getAsmName() << ":\n";
  for (PhiList::const_iterator I = Phis.begin(), E = Phis.end(); I != E; ++I) {
    InstPhi *Inst = *I;
    if (Inst->isDeleted())
      continue;
    // Emitting a Phi instruction should cause an error.
    Inst->emit(Func);
  }
  for (InstList::const_iterator I = Insts.begin(), E = Insts.end(); I != E;
       ++I) {
    Inst *Inst = *I;
    if (Inst->isDeleted())
      continue;
    // Here we detect redundant assignments like "mov eax, eax" and
    // suppress them.
    if (Inst->isRedundantAssign())
      continue;
    (*I)->emit(Func);
  }
}

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
