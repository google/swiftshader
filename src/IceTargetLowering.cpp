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

#include "assembler_ia32.h"
#include "IceCfg.h" // setError()
#include "IceCfgNode.h"
#include "IceOperand.h"
#include "IceRegAlloc.h"
#include "IceTargetLowering.h"
#include "IceTargetLoweringX8632.h"

namespace Ice {

void LoweringContext::init(CfgNode *N) {
  Node = N;
  End = getNode()->getInsts().end();
  rewind();
  advanceForward(Next);
}

void LoweringContext::rewind() {
  Begin = getNode()->getInsts().begin();
  Cur = Begin;
  skipDeleted(Cur);
  Next = Cur;
}

void LoweringContext::insert(Inst *Inst) {
  getNode()->getInsts().insert(Next, Inst);
  LastInserted = Inst;
}

void LoweringContext::skipDeleted(InstList::iterator &I) const {
  while (I != End && I->isDeleted())
    ++I;
}

void LoweringContext::advanceForward(InstList::iterator &I) const {
  if (I != End) {
    ++I;
    skipDeleted(I);
  }
}

Inst *LoweringContext::getLastInserted() const {
  assert(LastInserted);
  return LastInserted;
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
  return nullptr;
}

TargetLowering::TargetLowering(Cfg *Func)
    : Func(Func), Ctx(Func->getContext()), HasComputedFrame(false),
      CallsReturnsTwice(false), StackAdjustment(0), Context(),
      SnapshotStackAdjustment(0) {}

std::unique_ptr<Assembler> TargetLowering::createAssembler(TargetArch Target,
                                                           Cfg *Func) {
  // These statements can be #ifdef'd to specialize the assembler
  // to a subset of the available targets.  TODO: use CRTP.
  if (Target == Target_X8632)
    return std::unique_ptr<Assembler>(new x86::AssemblerX86());
  Func->setError("Unsupported target");
  return nullptr;
}

void TargetLowering::doAddressOpt() {
  if (llvm::isa<InstLoad>(*Context.getCur()))
    doAddressOptLoad();
  else if (llvm::isa<InstStore>(*Context.getCur()))
    doAddressOptStore();
  Context.advanceCur();
  Context.advanceNext();
}

void TargetLowering::doNopInsertion() {
  Inst *I = Context.getCur();
  bool ShouldSkip = llvm::isa<InstFakeUse>(I) || llvm::isa<InstFakeDef>(I) ||
                    llvm::isa<InstFakeKill>(I) || I->isRedundantAssign() ||
                    I->isDeleted();
  if (!ShouldSkip) {
    int Probability = Ctx->getFlags().getNopProbabilityAsPercentage();
    for (int I = 0; I < Ctx->getFlags().getMaxNopsPerInstruction(); ++I) {
      randomlyInsertNop(Probability / 100.0);
    }
  }
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
  Inst *Inst = Context.getCur();
  // Mark the current instruction as deleted before lowering,
  // otherwise the Dest variable will likely get marked as non-SSA.
  // See Variable::setDefinition().
  Inst->setDeleted();
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
  case Inst::IntrinsicCall: {
    InstIntrinsicCall *Call = llvm::dyn_cast<InstIntrinsicCall>(Inst);
    if (Call->getIntrinsicInfo().ReturnsTwice)
      setCallsReturnsTwice(true);
    lowerIntrinsicCall(Call);
    break;
  }
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
  case Inst::BundleLock:
  case Inst::BundleUnlock:
  case Inst::FakeDef:
  case Inst::FakeUse:
  case Inst::FakeKill:
  case Inst::Target:
    // These are all Target instruction types and shouldn't be
    // encountered at this stage.
    Func->setError("Can't lower unsupported instruction type");
    break;
  }

  postLower();

  Context.advanceCur();
  Context.advanceNext();
}

// Drives register allocation, allowing all physical registers (except
// perhaps for the frame pointer) to be allocated.  This set of
// registers could potentially be parameterized if we want to restrict
// registers e.g. for performance testing.
void TargetLowering::regAlloc(RegAllocKind Kind) {
  TimerMarker T(TimerStack::TT_regAlloc, Func);
  LinearScan LinearScan(Func);
  RegSetMask RegInclude = RegSet_None;
  RegSetMask RegExclude = RegSet_None;
  RegInclude |= RegSet_CallerSave;
  RegInclude |= RegSet_CalleeSave;
  if (hasFramePointer())
    RegExclude |= RegSet_FramePointer;
  LinearScan.init(Kind);
  llvm::SmallBitVector RegMask = getRegisterSet(RegInclude, RegExclude);
  LinearScan.scan(RegMask, Ctx->getFlags().shouldRandomizeRegAlloc());
}

std::unique_ptr<TargetDataLowering>
TargetDataLowering::createLowering(GlobalContext *Ctx) {
  // These statements can be #ifdef'd to specialize the code generator
  // to a subset of the available targets.  TODO: use CRTP.
  TargetArch Target = Ctx->getFlags().getTargetArch();
  if (Target == Target_X8632)
    return std::unique_ptr<TargetDataLowering>(TargetDataX8632::create(Ctx));
#if 0
  if (Target == Target_X8664)
    return std::unique_ptr<TargetDataLowering>(TargetDataX8664::create(Ctx));
  if (Target == Target_ARM32)
    return std::unique_ptr<TargetDataLowering>(TargetDataARM32::create(Ctx));
  if (Target == Target_ARM64)
    return std::unique_ptr<TargetDataLowering>(TargetDataARM64::create(Ctx));
#endif
  llvm_unreachable("Unsupported target");
  return nullptr;
}

TargetDataLowering::~TargetDataLowering() {}

} // end of namespace Ice
