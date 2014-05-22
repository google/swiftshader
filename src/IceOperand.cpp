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
#include "IceTargetLowering.h" // dumping stack/frame pointer register

namespace Ice {

bool operator<(const RelocatableTuple &A, const RelocatableTuple &B) {
  if (A.Offset != B.Offset)
    return A.Offset < B.Offset;
  if (A.SuppressMangling != B.SuppressMangling)
    return A.SuppressMangling < B.SuppressMangling;
  return A.Name < B.Name;
}

bool operator<(const RegWeight &A, const RegWeight &B) {
  return A.getWeight() < B.getWeight();
}
bool operator<=(const RegWeight &A, const RegWeight &B) { return !(B < A); }
bool operator==(const RegWeight &A, const RegWeight &B) {
  return !(B < A) && !(A < B);
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

Variable Variable::asType(Type Ty) {
  Variable V(Ty, DefNode, Number, Name);
  V.RegNum = RegNum;
  V.StackOffset = StackOffset;
  return V;
}

// ======================== dump routines ======================== //

void Variable::emit(const Cfg *Func) const {
  Func->getTarget()->emitVariable(this, Func);
}

void Variable::dump(const Cfg *Func) const {
  Ostream &Str = Func->getContext()->getStrDump();
  const CfgNode *CurrentNode = Func->getCurrentNode();
  (void)CurrentNode; // used only in assert()
  assert(CurrentNode == NULL || DefNode == NULL || DefNode == CurrentNode);
  if (Func->getContext()->isVerbose(IceV_RegOrigins) ||
      (!hasReg() && !Func->getTarget()->hasComputedFrame()))
    Str << "%" << getName();
  if (hasReg()) {
    if (Func->getContext()->isVerbose(IceV_RegOrigins))
      Str << ":";
    Str << Func->getTarget()->getRegName(RegNum, getType());
  } else if (Func->getTarget()->hasComputedFrame()) {
    if (Func->getContext()->isVerbose(IceV_RegOrigins))
      Str << ":";
    Str << "[" << Func->getTarget()->getRegName(
                      Func->getTarget()->getFrameOrStackReg(), IceType_i32);
    int32_t Offset = getStackOffset();
    if (Offset) {
      if (Offset > 0)
        Str << "+";
      Str << Offset;
    }
    Str << "]";
  }
}

void ConstantRelocatable::emit(const Cfg *Func) const {
  Ostream &Str = Func->getContext()->getStrEmit();
  if (SuppressMangling)
    Str << Name;
  else
    Str << Func->getContext()->mangleName(Name);
  if (Offset) {
    if (Offset > 0)
      Str << "+";
    Str << Offset;
  }
}

void ConstantRelocatable::dump(const Cfg *Func) const {
  Ostream &Str = Func->getContext()->getStrDump();
  Str << "@" << Name;
  if (Offset)
    Str << "+" << Offset;
}

Ostream &operator<<(Ostream &Str, const RegWeight &W) {
  if (W.getWeight() == RegWeight::Inf)
    Str << "Inf";
  else
    Str << W.getWeight();
  return Str;
}

} // end of namespace Ice
