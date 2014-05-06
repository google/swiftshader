//===-- FunctionLoweringInfo.cpp ------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This implements routines for translating functions from LLVM IR into
// Machine IR.
//
//===----------------------------------------------------------------------===//

#define DEBUG_TYPE "function-lowering-info"
#include "llvm/CodeGen/FunctionLoweringInfo.h"
#include "llvm/DerivedTypes.h"
#include "llvm/Function.h"
#include "llvm/Instructions.h"
#include "llvm/IntrinsicInst.h"
#include "llvm/LLVMContext.h"
#include "llvm/Module.h"
#include "llvm/Analysis/DebugInfo.h"
#include "llvm/CodeGen/Analysis.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineModuleInfo.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/Target/TargetRegisterInfo.h"
#include "llvm/Target/TargetData.h"
#include "llvm/Target/TargetFrameInfo.h"
#include "llvm/Target/TargetInstrInfo.h"
#include "llvm/Target/TargetLowering.h"
#include "llvm/Target/TargetOptions.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/MathExtras.h"
#include <algorithm>
using namespace llvm;

/// isUsedOutsideOfDefiningBlock - Return true if this instruction is used by
/// PHI nodes or outside of the basic block that defines it, or used by a
/// switch or atomic instruction, which may expand to multiple basic blocks.
static bool isUsedOutsideOfDefiningBlock(const Instruction *I) {
  if (I->use_empty()) return false;
  if (isa<PHINode>(I)) return true;
  const BasicBlock *BB = I->getParent();
  for (Value::const_use_iterator UI = I->use_begin(), E = I->use_end();
        UI != E; ++UI) {
    const User *U = *UI;
    if (cast<Instruction>(U)->getParent() != BB || isa<PHINode>(U))
      return true;
  }
  return false;
}

/// isOnlyUsedInEntryBlock - If the specified argument is only used in the
/// entry block, return true.  This includes arguments used by switches, since
/// the switch may expand into multiple basic blocks.
static bool isOnlyUsedInEntryBlock(const Argument *A) {
  const BasicBlock *Entry = A->getParent()->begin();
  for (Value::const_use_iterator UI = A->use_begin(), E = A->use_end();
       UI != E; ++UI) {
    const User *U = *UI;
    if (cast<Instruction>(U)->getParent() != Entry || isa<SwitchInst>(U))
      return false;  // Use not in entry block.
  }
  return true;
}

FunctionLoweringInfo::FunctionLoweringInfo(const TargetLowering &tli)
  : TLI(tli) {
}

void FunctionLoweringInfo::set(const Function &fn, MachineFunction &mf) {
  Fn = &fn;
  MF = &mf;
  RegInfo = &MF->getRegInfo();

  // Check whether the function can return without sret-demotion.
  SmallVector<ISD::OutputArg, 4> Outs;
  GetReturnInfo(Fn->getReturnType(),
                Fn->getAttributes().getRetAttributes(), Outs, TLI);
  CanLowerReturn = TLI.CanLowerReturn(Fn->getCallingConv(), Fn->isVarArg(),
                                      Outs, Fn->getContext());

  // Create a vreg for each argument register that is not dead and is used
  // outside of the entry block for the function.
  for (Function::const_arg_iterator AI = Fn->arg_begin(), E = Fn->arg_end();
       AI != E; ++AI)
    if (!isOnlyUsedInEntryBlock(AI))
      InitializeRegForValue(AI);

  // Initialize the mapping of values to registers.  This is only set up for
  // instruction values that are used outside of the block that defines
  // them.
  Function::const_iterator BB = Fn->begin(), EB = Fn->end();
  for (BasicBlock::const_iterator I = BB->begin(), E = BB->end(); I != E; ++I)
    if (const AllocaInst *AI = dyn_cast<AllocaInst>(I))
      if (const ConstantInt *CUI = dyn_cast<ConstantInt>(AI->getArraySize())) {
        const Type *Ty = AI->getAllocatedType();
        uint64_t TySize = TLI.getTargetData()->getTypeAllocSize(Ty);
        unsigned Align =
          std::max((unsigned)TLI.getTargetData()->getPrefTypeAlignment(Ty),
                   AI->getAlignment());

        TySize *= CUI->getZExtValue();   // Get total allocated size.
        if (TySize == 0) TySize = 1; // Don't create zero-sized stack objects.

        // The object may need to be placed onto the stack near the stack
        // protector if one exists. Determine here if this object is a suitable
        // candidate. I.e., it would trigger the creation of a stack protector.
        bool MayNeedSP =
          (AI->isArrayAllocation() ||
           (TySize > 8 && isa<ArrayType>(Ty) &&
            cast<ArrayType>(Ty)->getElementType()->isIntegerTy(8)));
        StaticAllocaMap[AI] =
          MF->getFrameInfo()->CreateStackObject(TySize, Align, false, MayNeedSP);
      }

  for (; BB != EB; ++BB)
    for (BasicBlock::const_iterator I = BB->begin(), E = BB->end(); I != E; ++I) {
      // Mark values used outside their block as exported, by allocating
      // a virtual register for them.
      if (isUsedOutsideOfDefiningBlock(I))
        if (!isa<AllocaInst>(I) ||
            !StaticAllocaMap.count(cast<AllocaInst>(I)))
          InitializeRegForValue(I);
    }

  // Create an initial MachineBasicBlock for each LLVM BasicBlock in F.  This
  // also creates the initial PHI MachineInstrs, though none of the input
  // operands are populated.
  for (BB = Fn->begin(); BB != EB; ++BB) {
    MachineBasicBlock *MBB = mf.CreateMachineBasicBlock(BB);
    MBBMap[BB] = MBB;
    MF->push_back(MBB);

    // Transfer the address-taken flag. This is necessary because there could
    // be multiple MachineBasicBlocks corresponding to one BasicBlock, and only
    // the first one should be marked.
    if (BB->hasAddressTaken())
      MBB->setHasAddressTaken();

    // Create Machine PHI nodes for LLVM PHI nodes, lowering them as
    // appropriate.
    for (BasicBlock::const_iterator I = BB->begin();
         const PHINode *PN = dyn_cast<PHINode>(I); ++I) {
      if (PN->use_empty()) continue;

      DebugLoc DL = PN->getDebugLoc();
      unsigned PHIReg = ValueMap[PN];
      assert(PHIReg && "PHI node does not have an assigned virtual register!");

      SmallVector<EVT, 4> ValueVTs;
      ComputeValueVTs(TLI, PN->getType(), ValueVTs);
      for (unsigned vti = 0, vte = ValueVTs.size(); vti != vte; ++vti) {
        EVT VT = ValueVTs[vti];
        unsigned NumRegisters = TLI.getNumRegisters(Fn->getContext(), VT);
        const TargetInstrInfo *TII = MF->getTarget().getInstrInfo();
        for (unsigned i = 0; i != NumRegisters; ++i)
          BuildMI(MBB, DL, TII->get(TargetOpcode::PHI), PHIReg + i);
        PHIReg += NumRegisters;
      }
    }
  }
}

/// clear - Clear out all the function-specific state. This returns this
/// FunctionLoweringInfo to an empty state, ready to be used for a
/// different function.
void FunctionLoweringInfo::clear() {
  assert(CatchInfoFound.size() == CatchInfoLost.size() &&
         "Not all catch info was assigned to a landing pad!");

  MBBMap.clear();
  ValueMap.clear();
  StaticAllocaMap.clear();
#ifndef NDEBUG
  CatchInfoLost.clear();
  CatchInfoFound.clear();
#endif
  LiveOutRegInfo.clear();
  ArgDbgValues.clear();
  RegFixups.clear();
}

/// CreateReg - Allocate a single virtual register for the given type.
unsigned FunctionLoweringInfo::CreateReg(EVT VT) {
  return RegInfo->createVirtualRegister(TLI.getRegClassFor(VT));
}

/// CreateRegs - Allocate the appropriate number of virtual registers of
/// the correctly promoted or expanded types.  Assign these registers
/// consecutive vreg numbers and return the first assigned number.
///
/// In the case that the given value has struct or array type, this function
/// will assign registers for each member or element.
///
unsigned FunctionLoweringInfo::CreateRegs(const Type *Ty) {
  SmallVector<EVT, 4> ValueVTs;
  ComputeValueVTs(TLI, Ty, ValueVTs);

  unsigned FirstReg = 0;
  for (unsigned Value = 0, e = ValueVTs.size(); Value != e; ++Value) {
    EVT ValueVT = ValueVTs[Value];
    EVT RegisterVT = TLI.getRegisterType(Ty->getContext(), ValueVT);

    unsigned NumRegs = TLI.getNumRegisters(Ty->getContext(), ValueVT);
    for (unsigned i = 0; i != NumRegs; ++i) {
      unsigned R = CreateReg(RegisterVT);
      if (!FirstReg) FirstReg = R;
    }
  }
  return FirstReg;
}

