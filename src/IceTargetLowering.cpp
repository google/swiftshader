//===- subzero/src/IceTargetLowering.cpp - Basic lowering implementation --===//
//
//                        The Subzero Code Generator
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements the skeleton of the TargetLowering class,
// specifically invoking the appropriate lowering method for a given
// instruction kind and driving global register allocation.  It also
// implements the non-deleted instruction iteration in
// LoweringContext.
//
//===----------------------------------------------------------------------===//

#include "IceCfg.h" // setError()
#include "IceCfgNode.h"
#include "IceOperand.h"
#include "IceRegAlloc.h"
#include "IceTargetLowering.h"
#include "IceTargetLoweringX8632.h"

namespace Ice {

void LoweringContext::init(CfgNode *N) {
  Node = N;
  Cur = getNode()->getInsts().begin();
  End = getNode()->getInsts().end();
  skipDeleted(Cur);
  Next = Cur;
  advance(Next);
}

void LoweringContext::insert(Inst *Inst) {
  getNode()->getInsts().insert(Next, Inst);
  Inst->updateVars(getNode());
}

void LoweringContext::skipDeleted(InstList::iterator &I) {
  while (I != End && (*I)->isDeleted())
    ++I;
}

void LoweringContext::advance(InstList::iterator &I) {
  if (I != End) {
    ++I;
    skipDeleted(I);
  }
}

TargetLowering *TargetLowering::createLowering(TargetArch Target, Cfg *Func) {
  // These statements can be #ifdef'd to specialize the code generator
  // to a subset of the available targets.  TODO: use CRTP.
  if (Target == Target_X8632)
    return TargetX8632::create(Func);
#if 0
  if (Target == Target_X8664)
    return IceTargetX8664::create(Func);
  if (Target == Target_ARM32)
    return IceTargetARM32::create(Func);
  if (Target == Target_ARM64)
    return IceTargetARM64::create(Func);
#endif
  Func->setError("Unsupported target");
  return NULL;
}

void TargetLowering::doAddressOpt() {
  if (llvm::isa<InstLoad>(*Context.getCur()))
    doAddressOptLoad();
  else if (llvm::isa<InstStore>(*Context.getCur()))
    doAddressOptStore();
  Context.advanceCur();
  Context.advanceNext();
}

// Lowers a single instruction according to the information in
// Context, by checking the Context.Cur instruction kind and calling
// the appropriate lowering method.  The lowering method should insert
// target instructions at the Cur.Next insertion point, and should not
// delete the Context.Cur instruction or advance Context.Cur.
//
// The lowering method may look ahead in the instruction stream as
// desired, and lower additional instructions in conjunction with the
// current one, for example fusing a compare and branch.  If it does,
// it should advance Context.Cur to point to the next non-deleted
// instruction to process, and it should delete any additional
// instructions it consumes.
void TargetLowering::lower() {
  assert(!Context.atEnd());
  Inst *Inst = *Context.getCur();
  switch (Inst->getKind()) {
  case Inst::Alloca:
    lowerAlloca(llvm::dyn_cast<InstAlloca>(Inst));
    break;
  case Inst::Arithmetic:
    lowerArithmetic(llvm::dyn_cast<InstArithmetic>(Inst));
    break;
  case Inst::Assign:
    lowerAssign(llvm::dyn_cast<InstAssign>(Inst));
    break;
  case Inst::Br:
    lowerBr(llvm::dyn_cast<InstBr>(Inst));
    break;
  case Inst::Call:
    lowerCall(llvm::dyn_cast<InstCall>(Inst));
    break;
  case Inst::Cast:
    lowerCast(llvm::dyn_cast<InstCast>(Inst));
    break;
  case Inst::ExtractElement:
    lowerExtractElement(llvm::dyn_cast<InstExtractElement>(Inst));
    break;
  case Inst::Fcmp:
    lowerFcmp(llvm::dyn_cast<InstFcmp>(Inst));
    break;
  case Inst::Icmp:
    lowerIcmp(llvm::dyn_cast<InstIcmp>(Inst));
    break;
  case Inst::InsertElement:
    lowerInsertElement(llvm::dyn_cast<InstInsertElement>(Inst));
    break;
  case Inst::IntrinsicCall:
    lowerIntrinsicCall(llvm::dyn_cast<InstIntrinsicCall>(Inst));
    break;
  case Inst::Load:
    lowerLoad(llvm::dyn_cast<InstLoad>(Inst));
    break;
  case Inst::Phi:
    lowerPhi(llvm::dyn_cast<InstPhi>(Inst));
    break;
  case Inst::Ret:
    lowerRet(llvm::dyn_cast<InstRet>(Inst));
    break;
  case Inst::Select:
    lowerSelect(llvm::dyn_cast<InstSelect>(Inst));
    break;
  case Inst::Store:
    lowerStore(llvm::dyn_cast<InstStore>(Inst));
    break;
  case Inst::Switch:
    lowerSwitch(llvm::dyn_cast<InstSwitch>(Inst));
    break;
  case Inst::Unreachable:
    lowerUnreachable(llvm::dyn_cast<InstUnreachable>(Inst));
    break;
  case Inst::FakeDef:
  case Inst::FakeUse:
  case Inst::FakeKill:
  case Inst::Target:
    // These are all Target instruction types and shouldn't be
    // encountered at this stage.
    Func->setError("Can't lower unsupported instruction type");
    break;
  }
  Inst->setDeleted();

  postLower();

  Context.advanceCur();
  Context.advanceNext();
}

// Drives register allocation, allowing all physical registers (except
// perhaps for the frame pointer) to be allocated.  This set of
// registers could potentially be parameterized if we want to restrict
// registers e.g. for performance testing.
void TargetLowering::regAlloc() {
  LinearScan LinearScan(Func);
  RegSetMask RegInclude = RegSet_None;
  RegSetMask RegExclude = RegSet_None;
  RegInclude |= RegSet_CallerSave;
  RegInclude |= RegSet_CalleeSave;
  if (hasFramePointer())
    RegExclude |= RegSet_FramePointer;
  llvm::SmallBitVector RegMask = getRegisterSet(RegInclude, RegExclude);
  LinearScan.scan(RegMask);
}

TargetGlobalInitLowering *
TargetGlobalInitLowering::createLowering(TargetArch Target,
                                         GlobalContext *Ctx) {
  // These statements can be #ifdef'd to specialize the code generator
  // to a subset of the available targets.  TODO: use CRTP.
  if (Target == Target_X8632)
    return TargetGlobalInitX8632::create(Ctx);
#if 0
  if (Target == Target_X8664)
    return IceTargetGlobalInitX8664::create(Ctx);
  if (Target == Target_ARM32)
    return IceTargetGlobalInitARM32::create(Ctx);
  if (Target == Target_ARM64)
    return IceTargetGlobalInitARM64::create(Ctx);
#endif
  llvm_unreachable("Unsupported target");
  return NULL;
}

} // end of namespace Ice
