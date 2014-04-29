//===- subzero/src/IceOperand.cpp - High-level operand implementation -----===//
//
//                        The Subzero Code Generator
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements the Operand class and its
// target-independent subclasses, primarily for the methods of the
// Variable class.
//
//===----------------------------------------------------------------------===//

#include "IceCfg.h"
#include "IceInst.h"
#include "IceOperand.h"

namespace Ice {

bool operator<(const RelocatableTuple &A, const RelocatableTuple &B) {
  if (A.Offset != B.Offset)
    return A.Offset < B.Offset;
  if (A.SuppressMangling != B.SuppressMangling)
    return A.SuppressMangling < B.SuppressMangling;
  return A.Name < B.Name;
}

void Variable::setUse(const Inst *Inst, const CfgNode *Node) {
  if (DefNode == NULL)
    return;
  if (llvm::isa<InstPhi>(Inst) || Node != DefNode)
    DefNode = NULL;
}

void Variable::setDefinition(Inst *Inst, const CfgNode *Node) {
  if (DefNode == NULL)
    return;
  // Can first check preexisting DefInst if we care about multi-def vars.
  DefInst = Inst;
  if (Node != DefNode)
    DefNode = NULL;
}

void Variable::replaceDefinition(Inst *Inst, const CfgNode *Node) {
  DefInst = NULL;
  setDefinition(Inst, Node);
}

void Variable::setIsArg(Cfg *Func) {
  IsArgument = true;
  if (DefNode == NULL)
    return;
  CfgNode *Entry = Func->getEntryNode();
  if (DefNode == Entry)
    return;
  DefNode = NULL;
}

IceString Variable::getName() const {
  if (!Name.empty())
    return Name;
  char buf[30];
  snprintf(buf, llvm::array_lengthof(buf), "__%u", getIndex());
  return buf;
}

// ======================== dump routines ======================== //

void Variable::dump(const Cfg *Func) const {
  Ostream &Str = Func->getContext()->getStrDump();
  const CfgNode *CurrentNode = Func->getCurrentNode();
  (void)CurrentNode; // used only in assert()
  assert(CurrentNode == NULL || DefNode == NULL || DefNode == CurrentNode);
  Str << "%" << getName();
}

void Operand::dump(const Cfg *Func) const {
  Ostream &Str = Func->getContext()->getStrDump();
  Str << "Operand<?>";
}

void ConstantRelocatable::dump(const Cfg *Func) const {
  Ostream &Str = Func->getContext()->getStrDump();
  Str << "@" << Name;
  if (Offset)
    Str << "+" << Offset;
}

} // end of namespace Ice
