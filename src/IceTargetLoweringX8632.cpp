//===- subzero/src/IceTargetLoweringX8632.cpp - x86-32 lowering -----------===//
//
//                        The Subzero Code Generator
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file implements the TargetLoweringX8632 class, which consists almost
/// entirely of the lowering sequence for each high-level instruction.
///
//===----------------------------------------------------------------------===//

#include "IceTargetLoweringX8632.h"

#include "IceTargetLoweringX8632Traits.h"
#include "IceTargetLoweringX86Base.h"

namespace Ice {

//------------------------------------------------------------------------------
//      ______   ______     ______     __     ______   ______
//     /\__  _\ /\  == \   /\  __ \   /\ \   /\__  _\ /\  ___\
//     \/_/\ \/ \ \  __<   \ \  __ \  \ \ \  \/_/\ \/ \ \___  \
//        \ \_\  \ \_\ \_\  \ \_\ \_\  \ \_\    \ \_\  \/\_____\
//         \/_/   \/_/ /_/   \/_/\/_/   \/_/     \/_/   \/_____/
//
//------------------------------------------------------------------------------
namespace X86Internal {
const MachineTraits<TargetX8632>::TableFcmpType
    MachineTraits<TargetX8632>::TableFcmp[] = {
#define X(val, dflt, swapS, C1, C2, swapV, pred)                               \
  {                                                                            \
    dflt, swapS, X8632::Traits::Cond::C1, X8632::Traits::Cond::C2, swapV,      \
        X8632::Traits::Cond::pred                                              \
  }                                                                            \
  ,
        FCMPX8632_TABLE
#undef X
};

const size_t MachineTraits<TargetX8632>::TableFcmpSize =
    llvm::array_lengthof(TableFcmp);

const MachineTraits<TargetX8632>::TableIcmp32Type
    MachineTraits<TargetX8632>::TableIcmp32[] = {
#define X(val, C_32, C1_64, C2_64, C3_64)                                      \
  { X8632::Traits::Cond::C_32 }                                                \
  ,
        ICMPX8632_TABLE
#undef X
};

const size_t MachineTraits<TargetX8632>::TableIcmp32Size =
    llvm::array_lengthof(TableIcmp32);

const MachineTraits<TargetX8632>::TableIcmp64Type
    MachineTraits<TargetX8632>::TableIcmp64[] = {
#define X(val, C_32, C1_64, C2_64, C3_64)                                      \
  {                                                                            \
    X8632::Traits::Cond::C1_64, X8632::Traits::Cond::C2_64,                    \
        X8632::Traits::Cond::C3_64                                             \
  }                                                                            \
  ,
        ICMPX8632_TABLE
#undef X
};

const size_t MachineTraits<TargetX8632>::TableIcmp64Size =
    llvm::array_lengthof(TableIcmp64);

const MachineTraits<TargetX8632>::TableTypeX8632AttributesType
    MachineTraits<TargetX8632>::TableTypeX8632Attributes[] = {
#define X(tag, elementty, cvt, sdss, pack, width, fld)                         \
  { IceType_##elementty }                                                      \
  ,
        ICETYPEX8632_TABLE
#undef X
};

const size_t MachineTraits<TargetX8632>::TableTypeX8632AttributesSize =
    llvm::array_lengthof(TableTypeX8632Attributes);

const uint32_t MachineTraits<TargetX8632>::X86_STACK_ALIGNMENT_BYTES = 16;
const char *MachineTraits<TargetX8632>::TargetName = "X8632";

template <>
std::array<llvm::SmallBitVector, RCX86_NUM>
    TargetX86Base<TargetX8632>::TypeToRegisterSet = {};

template <>
std::array<llvm::SmallBitVector,
           TargetX86Base<TargetX8632>::Traits::RegisterSet::Reg_NUM>
    TargetX86Base<TargetX8632>::RegisterAliases = {};

template <>
llvm::SmallBitVector
    TargetX86Base<TargetX8632>::ScratchRegs = llvm::SmallBitVector();

} // end of namespace X86Internal

//------------------------------------------------------------------------------
//     __      ______  __     __  ______  ______  __  __   __  ______
//    /\ \    /\  __ \/\ \  _ \ \/\  ___\/\  == \/\ \/\ "-.\ \/\  ___\
//    \ \ \___\ \ \/\ \ \ \/ ".\ \ \  __\\ \  __<\ \ \ \ \-.  \ \ \__ \
//     \ \_____\ \_____\ \__/".~\_\ \_____\ \_\ \_\ \_\ \_\\"\_\ \_____\
//      \/_____/\/_____/\/_/   \/_/\/_____/\/_/ /_/\/_/\/_/ \/_/\/_____/
//
//------------------------------------------------------------------------------
void TargetX8632::lowerCall(const InstCall *Instr) {
  // x86-32 calling convention:
  //
  // * At the point before the call, the stack must be aligned to 16 bytes.
  //
  // * The first four arguments of vector type, regardless of their position
  // relative to the other arguments in the argument list, are placed in
  // registers xmm0 - xmm3.
  //
  // * Other arguments are pushed onto the stack in right-to-left order, such
  // that the left-most argument ends up on the top of the stack at the lowest
  // memory address.
  //
  // * Stack arguments of vector type are aligned to start at the next highest
  // multiple of 16 bytes. Other stack arguments are aligned to 4 bytes.
  //
  // This intends to match the section "IA-32 Function Calling Convention" of
  // the document "OS X ABI Function Call Guide" by Apple.
  NeedsStackAlignment = true;

  OperandList XmmArgs;
  OperandList StackArgs, StackArgLocations;
  uint32_t ParameterAreaSizeBytes = 0;

  // Classify each argument operand according to the location where the
  // argument is passed.
  for (SizeT i = 0, NumArgs = Instr->getNumArgs(); i < NumArgs; ++i) {
    Operand *Arg = Instr->getArg(i);
    Type Ty = Arg->getType();
    // The PNaCl ABI requires the width of arguments to be at least 32 bits.
    assert(typeWidthInBytes(Ty) >= 4);
    if (isVectorType(Ty) && XmmArgs.size() < Traits::X86_MAX_XMM_ARGS) {
      XmmArgs.push_back(Arg);
    } else {
      StackArgs.push_back(Arg);
      if (isVectorType(Arg->getType())) {
        ParameterAreaSizeBytes =
            Traits::applyStackAlignment(ParameterAreaSizeBytes);
      }
      Variable *esp =
          Func->getTarget()->getPhysicalRegister(Traits::RegisterSet::Reg_esp);
      Constant *Loc = Ctx->getConstantInt32(ParameterAreaSizeBytes);
      StackArgLocations.push_back(
          Traits::X86OperandMem::create(Func, Ty, esp, Loc));
      ParameterAreaSizeBytes += typeWidthInBytesOnStack(Arg->getType());
    }
  }

  // Adjust the parameter area so that the stack is aligned. It is assumed that
  // the stack is already aligned at the start of the calling sequence.
  ParameterAreaSizeBytes = Traits::applyStackAlignment(ParameterAreaSizeBytes);

  // Subtract the appropriate amount for the argument area. This also takes
  // care of setting the stack adjustment during emission.
  //
  // TODO: If for some reason the call instruction gets dead-code eliminated
  // after lowering, we would need to ensure that the pre-call and the
  // post-call esp adjustment get eliminated as well.
  if (ParameterAreaSizeBytes) {
    _adjust_stack(ParameterAreaSizeBytes);
  }

  // Copy arguments that are passed on the stack to the appropriate stack
  // locations.
  for (SizeT i = 0, e = StackArgs.size(); i < e; ++i) {
    lowerStore(InstStore::create(Func, StackArgs[i], StackArgLocations[i]));
  }

  // Copy arguments to be passed in registers to the appropriate registers.
  // TODO: Investigate the impact of lowering arguments passed in registers
  // after lowering stack arguments as opposed to the other way around.
  // Lowering register arguments after stack arguments may reduce register
  // pressure. On the other hand, lowering register arguments first (before
  // stack arguments) may result in more compact code, as the memory operand
  // displacements may end up being smaller before any stack adjustment is
  // done.
  for (SizeT i = 0, NumXmmArgs = XmmArgs.size(); i < NumXmmArgs; ++i) {
    Variable *Reg =
        legalizeToReg(XmmArgs[i], Traits::RegisterSet::Reg_xmm0 + i);
    // Generate a FakeUse of register arguments so that they do not get dead
    // code eliminated as a result of the FakeKill of scratch registers after
    // the call.
    Context.insert(InstFakeUse::create(Func, Reg));
  }
  // Generate the call instruction. Assign its result to a temporary with high
  // register allocation weight.
  Variable *Dest = Instr->getDest();
  // ReturnReg doubles as ReturnRegLo as necessary.
  Variable *ReturnReg = nullptr;
  Variable *ReturnRegHi = nullptr;
  if (Dest) {
    switch (Dest->getType()) {
    case IceType_NUM:
    case IceType_void:
      llvm::report_fatal_error("Invalid Call dest type");
      break;
    case IceType_i1:
    case IceType_i8:
    case IceType_i16:
    case IceType_i32:
      ReturnReg = makeReg(Dest->getType(), Traits::RegisterSet::Reg_eax);
      break;
    case IceType_i64:
      ReturnReg = makeReg(IceType_i32, Traits::RegisterSet::Reg_eax);
      ReturnRegHi = makeReg(IceType_i32, Traits::RegisterSet::Reg_edx);
      break;
    case IceType_f32:
    case IceType_f64:
      // Leave ReturnReg==ReturnRegHi==nullptr, and capture the result with the
      // fstp instruction.
      break;
    case IceType_v4i1:
    case IceType_v8i1:
    case IceType_v16i1:
    case IceType_v16i8:
    case IceType_v8i16:
    case IceType_v4i32:
    case IceType_v4f32:
      ReturnReg = makeReg(Dest->getType(), Traits::RegisterSet::Reg_xmm0);
      break;
    }
  }
  Operand *CallTarget = legalize(Instr->getCallTarget());
  const bool NeedSandboxing = Ctx->getFlags().getUseSandboxing();
  if (NeedSandboxing) {
    if (llvm::isa<Constant>(CallTarget)) {
      _bundle_lock(InstBundleLock::Opt_AlignToEnd);
    } else {
      Variable *CallTargetVar = nullptr;
      _mov(CallTargetVar, CallTarget);
      _bundle_lock(InstBundleLock::Opt_AlignToEnd);
      const SizeT BundleSize =
          1 << Func->getAssembler<>()->getBundleAlignLog2Bytes();
      _and(CallTargetVar, Ctx->getConstantInt32(~(BundleSize - 1)));
      CallTarget = CallTargetVar;
    }
  }
  Inst *NewCall = Traits::Insts::Call::create(Func, ReturnReg, CallTarget);
  Context.insert(NewCall);
  if (NeedSandboxing)
    _bundle_unlock();
  if (ReturnRegHi)
    Context.insert(InstFakeDef::create(Func, ReturnRegHi));

  // Add the appropriate offset to esp. The call instruction takes care of
  // resetting the stack offset during emission.
  if (ParameterAreaSizeBytes) {
    Variable *esp =
        Func->getTarget()->getPhysicalRegister(Traits::RegisterSet::Reg_esp);
    _add(esp, Ctx->getConstantInt32(ParameterAreaSizeBytes));
  }

  // Insert a register-kill pseudo instruction.
  Context.insert(InstFakeKill::create(Func, NewCall));

  // Generate a FakeUse to keep the call live if necessary.
  if (Instr->hasSideEffects() && ReturnReg) {
    Inst *FakeUse = InstFakeUse::create(Func, ReturnReg);
    Context.insert(FakeUse);
  }

  if (!Dest)
    return;

  // Assign the result of the call to Dest.
  if (ReturnReg) {
    if (ReturnRegHi) {
      auto *Dest64On32 = llvm::cast<Variable64On32>(Dest);
      Variable *DestLo = Dest64On32->getLo();
      Variable *DestHi = Dest64On32->getHi();
      _mov(DestLo, ReturnReg);
      _mov(DestHi, ReturnRegHi);
    } else {
      assert(Dest->getType() == IceType_i32 || Dest->getType() == IceType_i16 ||
             Dest->getType() == IceType_i8 || Dest->getType() == IceType_i1 ||
             isVectorType(Dest->getType()));
      if (isVectorType(Dest->getType())) {
        _movp(Dest, ReturnReg);
      } else {
        _mov(Dest, ReturnReg);
      }
    }
  } else if (isScalarFloatingType(Dest->getType())) {
    // Special treatment for an FP function which returns its result in st(0).
    // If Dest ends up being a physical xmm register, the fstp emit code will
    // route st(0) through a temporary stack slot.
    _fstp(Dest);
    // Create a fake use of Dest in case it actually isn't used, because st(0)
    // still needs to be popped.
    Context.insert(InstFakeUse::create(Func, Dest));
  }
}

void TargetX8632::lowerArguments() {
  VarList &Args = Func->getArgs();
  // The first four arguments of vector type, regardless of their position
  // relative to the other arguments in the argument list, are passed in
  // registers xmm0 - xmm3.
  unsigned NumXmmArgs = 0;

  Context.init(Func->getEntryNode());
  Context.setInsertPoint(Context.getCur());

  for (SizeT I = 0, E = Args.size();
       I < E && NumXmmArgs < Traits::X86_MAX_XMM_ARGS; ++I) {
    Variable *Arg = Args[I];
    Type Ty = Arg->getType();
    if (!isVectorType(Ty))
      continue;
    // Replace Arg in the argument list with the home register. Then generate
    // an instruction in the prolog to copy the home register to the assigned
    // location of Arg.
    int32_t RegNum = Traits::RegisterSet::Reg_xmm0 + NumXmmArgs;
    ++NumXmmArgs;
    Variable *RegisterArg = Func->makeVariable(Ty);
    if (BuildDefs::dump())
      RegisterArg->setName(Func, "home_reg:" + Arg->getName(Func));
    RegisterArg->setRegNum(RegNum);
    RegisterArg->setIsArg();
    Arg->setIsArg(false);

    Args[I] = RegisterArg;
    Context.insert(InstAssign::create(Func, Arg, RegisterArg));
  }
}

void TargetX8632::lowerRet(const InstRet *Inst) {
  Variable *Reg = nullptr;
  if (Inst->hasRetValue()) {
    Operand *Src0 = legalize(Inst->getRetValue());
    // TODO(jpp): this is not needed.
    if (Src0->getType() == IceType_i64) {
      Variable *eax =
          legalizeToReg(loOperand(Src0), Traits::RegisterSet::Reg_eax);
      Variable *edx =
          legalizeToReg(hiOperand(Src0), Traits::RegisterSet::Reg_edx);
      Reg = eax;
      Context.insert(InstFakeUse::create(Func, edx));
    } else if (isScalarFloatingType(Src0->getType())) {
      _fld(Src0);
    } else if (isVectorType(Src0->getType())) {
      Reg = legalizeToReg(Src0, Traits::RegisterSet::Reg_xmm0);
    } else {
      _mov(Reg, Src0, Traits::RegisterSet::Reg_eax);
    }
  }
  // Add a ret instruction even if sandboxing is enabled, because addEpilog
  // explicitly looks for a ret instruction as a marker for where to insert the
  // frame removal instructions.
  _ret(Reg);
  // Add a fake use of esp to make sure esp stays alive for the entire
  // function. Otherwise post-call esp adjustments get dead-code eliminated.
  // TODO: Are there more places where the fake use should be inserted? E.g.
  // "void f(int n){while(1) g(n);}" may not have a ret instruction.
  Variable *esp =
      Func->getTarget()->getPhysicalRegister(Traits::RegisterSet::Reg_esp);
  Context.insert(InstFakeUse::create(Func, esp));
}

void TargetX8632::addProlog(CfgNode *Node) {
  // Stack frame layout:
  //
  // +------------------------+
  // | 1. return address      |
  // +------------------------+
  // | 2. preserved registers |
  // +------------------------+
  // | 3. padding             |
  // +------------------------+
  // | 4. global spill area   |
  // +------------------------+
  // | 5. padding             |
  // +------------------------+
  // | 6. local spill area    |
  // +------------------------+
  // | 7. padding             |
  // +------------------------+
  // | 8. allocas             |
  // +------------------------+
  //
  // The following variables record the size in bytes of the given areas:
  //  * X86_RET_IP_SIZE_BYTES:  area 1
  //  * PreservedRegsSizeBytes: area 2
  //  * SpillAreaPaddingBytes:  area 3
  //  * GlobalsSize:            area 4
  //  * GlobalsAndSubsequentPaddingSize: areas 4 - 5
  //  * LocalsSpillAreaSize:    area 6
  //  * SpillAreaSizeBytes:     areas 3 - 7

  // Determine stack frame offsets for each Variable without a register
  // assignment. This can be done as one variable per stack slot. Or, do
  // coalescing by running the register allocator again with an infinite set of
  // registers (as a side effect, this gives variables a second chance at
  // physical register assignment).
  //
  // A middle ground approach is to leverage sparsity and allocate one block of
  // space on the frame for globals (variables with multi-block lifetime), and
  // one block to share for locals (single-block lifetime).

  Context.init(Node);
  Context.setInsertPoint(Context.getCur());

  llvm::SmallBitVector CalleeSaves =
      getRegisterSet(RegSet_CalleeSave, RegSet_None);
  RegsUsed = llvm::SmallBitVector(CalleeSaves.size());
  VarList SortedSpilledVariables, VariablesLinkedToSpillSlots;
  size_t GlobalsSize = 0;
  // If there is a separate locals area, this represents that area. Otherwise
  // it counts any variable not counted by GlobalsSize.
  SpillAreaSizeBytes = 0;
  // If there is a separate locals area, this specifies the alignment for it.
  uint32_t LocalsSlotsAlignmentBytes = 0;
  // The entire spill locations area gets aligned to largest natural alignment
  // of the variables that have a spill slot.
  uint32_t SpillAreaAlignmentBytes = 0;
  // A spill slot linked to a variable with a stack slot should reuse that
  // stack slot.
  std::function<bool(Variable *)> TargetVarHook =
      [&VariablesLinkedToSpillSlots](Variable *Var) {
        if (auto *SpillVar =
                llvm::dyn_cast<typename Traits::SpillVariable>(Var)) {
          assert(Var->mustNotHaveReg());
          if (SpillVar->getLinkedTo() && !SpillVar->getLinkedTo()->hasReg()) {
            VariablesLinkedToSpillSlots.push_back(Var);
            return true;
          }
        }
        return false;
      };

  // Compute the list of spilled variables and bounds for GlobalsSize, etc.
  getVarStackSlotParams(SortedSpilledVariables, RegsUsed, &GlobalsSize,
                        &SpillAreaSizeBytes, &SpillAreaAlignmentBytes,
                        &LocalsSlotsAlignmentBytes, TargetVarHook);
  uint32_t LocalsSpillAreaSize = SpillAreaSizeBytes;
  SpillAreaSizeBytes += GlobalsSize;

  // Add push instructions for preserved registers.
  uint32_t NumCallee = 0;
  size_t PreservedRegsSizeBytes = 0;
  llvm::SmallBitVector Pushed(CalleeSaves.size());
  for (SizeT i = 0; i < CalleeSaves.size(); ++i) {
    SizeT Canonical = Traits::getBaseReg(i);
    if (CalleeSaves[i] && RegsUsed[i]) {
      Pushed[Canonical] = true;
    }
  }
  for (SizeT i = 0; i < Pushed.size(); ++i) {
    if (Pushed[i]) {
      ++NumCallee;
      PreservedRegsSizeBytes += typeWidthInBytes(IceType_i32);
      _push(getPhysicalRegister(i));
    }
  }
  Ctx->statsUpdateRegistersSaved(NumCallee);

  // Generate "push ebp; mov ebp, esp"
  if (IsEbpBasedFrame) {
    assert((RegsUsed & getRegisterSet(RegSet_FramePointer, RegSet_None))
               .count() == 0);
    PreservedRegsSizeBytes += typeWidthInBytes(IceType_i32);
    Variable *ebp = getPhysicalRegister(Traits::RegisterSet::Reg_ebp);
    Variable *esp = getPhysicalRegister(Traits::RegisterSet::Reg_esp);
    _push(ebp);
    _mov(ebp, esp);
    // Keep ebp live for late-stage liveness analysis (e.g. asm-verbose mode).
    Context.insert(InstFakeUse::create(Func, ebp));
  }

  // Align the variables area. SpillAreaPaddingBytes is the size of the region
  // after the preserved registers and before the spill areas.
  // LocalsSlotsPaddingBytes is the amount of padding between the globals and
  // locals area if they are separate.
  assert(SpillAreaAlignmentBytes <= Traits::X86_STACK_ALIGNMENT_BYTES);
  assert(LocalsSlotsAlignmentBytes <= SpillAreaAlignmentBytes);
  uint32_t SpillAreaPaddingBytes = 0;
  uint32_t LocalsSlotsPaddingBytes = 0;
  alignStackSpillAreas(Traits::X86_RET_IP_SIZE_BYTES + PreservedRegsSizeBytes,
                       SpillAreaAlignmentBytes, GlobalsSize,
                       LocalsSlotsAlignmentBytes, &SpillAreaPaddingBytes,
                       &LocalsSlotsPaddingBytes);
  SpillAreaSizeBytes += SpillAreaPaddingBytes + LocalsSlotsPaddingBytes;
  uint32_t GlobalsAndSubsequentPaddingSize =
      GlobalsSize + LocalsSlotsPaddingBytes;

  // Align esp if necessary.
  if (NeedsStackAlignment) {
    uint32_t StackOffset =
        Traits::X86_RET_IP_SIZE_BYTES + PreservedRegsSizeBytes;
    uint32_t StackSize =
        Traits::applyStackAlignment(StackOffset + SpillAreaSizeBytes);
    SpillAreaSizeBytes = StackSize - StackOffset;
  }

  // Generate "sub esp, SpillAreaSizeBytes"
  if (SpillAreaSizeBytes)
    _sub(getPhysicalRegister(Traits::RegisterSet::Reg_esp),
         Ctx->getConstantInt32(SpillAreaSizeBytes));

  // Account for alloca instructions with known frame offsets.
  SpillAreaSizeBytes += FixedAllocaSizeBytes;

  Ctx->statsUpdateFrameBytes(SpillAreaSizeBytes);

  // Initialize the stack adjustment so that after all the known-frame-offset
  // alloca instructions are emitted, the stack adjustment will reach zero.
  resetStackAdjustment();
  updateStackAdjustment(-FixedAllocaSizeBytes);

  // Fill in stack offsets for stack args, and copy args into registers for
  // those that were register-allocated. Args are pushed right to left, so
  // Arg[0] is closest to the stack/frame pointer.
  Variable *FramePtr = getPhysicalRegister(getFrameOrStackReg());
  size_t BasicFrameOffset =
      PreservedRegsSizeBytes + Traits::X86_RET_IP_SIZE_BYTES;
  if (!IsEbpBasedFrame)
    BasicFrameOffset += SpillAreaSizeBytes;

  const VarList &Args = Func->getArgs();
  size_t InArgsSizeBytes = 0;
  unsigned NumXmmArgs = 0;
  for (Variable *Arg : Args) {
    // Skip arguments passed in registers.
    if (isVectorType(Arg->getType()) && NumXmmArgs < Traits::X86_MAX_XMM_ARGS) {
      ++NumXmmArgs;
      continue;
    }
    // For esp-based frames, the esp value may not stabilize to its home value
    // until after all the fixed-size alloca instructions have executed.  In
    // this case, a stack adjustment is needed when accessing in-args in order
    // to copy them into registers.
    size_t StackAdjBytes = IsEbpBasedFrame ? 0 : -FixedAllocaSizeBytes;
    finishArgumentLowering(Arg, FramePtr, BasicFrameOffset, StackAdjBytes,
                           InArgsSizeBytes);
  }

  // Fill in stack offsets for locals.
  assignVarStackSlots(SortedSpilledVariables, SpillAreaPaddingBytes,
                      SpillAreaSizeBytes, GlobalsAndSubsequentPaddingSize,
                      IsEbpBasedFrame);
  // Assign stack offsets to variables that have been linked to spilled
  // variables.
  for (Variable *Var : VariablesLinkedToSpillSlots) {
    Variable *Linked =
        (llvm::cast<typename Traits::SpillVariable>(Var))->getLinkedTo();
    Var->setStackOffset(Linked->getStackOffset());
  }
  this->HasComputedFrame = true;

  if (BuildDefs::dump() && Func->isVerbose(IceV_Frame)) {
    OstreamLocker L(Func->getContext());
    Ostream &Str = Func->getContext()->getStrDump();

    Str << "Stack layout:\n";
    uint32_t EspAdjustmentPaddingSize =
        SpillAreaSizeBytes - LocalsSpillAreaSize -
        GlobalsAndSubsequentPaddingSize - SpillAreaPaddingBytes;
    Str << " in-args = " << InArgsSizeBytes << " bytes\n"
        << " return address = " << Traits::X86_RET_IP_SIZE_BYTES << " bytes\n"
        << " preserved registers = " << PreservedRegsSizeBytes << " bytes\n"
        << " spill area padding = " << SpillAreaPaddingBytes << " bytes\n"
        << " globals spill area = " << GlobalsSize << " bytes\n"
        << " globals-locals spill areas intermediate padding = "
        << GlobalsAndSubsequentPaddingSize - GlobalsSize << " bytes\n"
        << " locals spill area = " << LocalsSpillAreaSize << " bytes\n"
        << " esp alignment padding = " << EspAdjustmentPaddingSize
        << " bytes\n";

    Str << "Stack details:\n"
        << " esp adjustment = " << SpillAreaSizeBytes << " bytes\n"
        << " spill area alignment = " << SpillAreaAlignmentBytes << " bytes\n"
        << " locals spill area alignment = " << LocalsSlotsAlignmentBytes
        << " bytes\n"
        << " is ebp based = " << IsEbpBasedFrame << "\n";
  }
}

void TargetX8632::addEpilog(CfgNode *Node) {
  InstList &Insts = Node->getInsts();
  InstList::reverse_iterator RI, E;
  for (RI = Insts.rbegin(), E = Insts.rend(); RI != E; ++RI) {
    if (llvm::isa<typename Traits::Insts::Ret>(*RI))
      break;
  }
  if (RI == E)
    return;

  // Convert the reverse_iterator position into its corresponding (forward)
  // iterator position.
  InstList::iterator InsertPoint = RI.base();
  --InsertPoint;
  Context.init(Node);
  Context.setInsertPoint(InsertPoint);

  Variable *esp = getPhysicalRegister(Traits::RegisterSet::Reg_esp);
  if (IsEbpBasedFrame) {
    Variable *ebp = getPhysicalRegister(Traits::RegisterSet::Reg_ebp);
    // For late-stage liveness analysis (e.g. asm-verbose mode), adding a fake
    // use of esp before the assignment of esp=ebp keeps previous esp
    // adjustments from being dead-code eliminated.
    Context.insert(InstFakeUse::create(Func, esp));
    _mov(esp, ebp);
    _pop(ebp);
  } else {
    // add esp, SpillAreaSizeBytes
    if (SpillAreaSizeBytes)
      _add(esp, Ctx->getConstantInt32(SpillAreaSizeBytes));
  }

  // Add pop instructions for preserved registers.
  llvm::SmallBitVector CalleeSaves =
      getRegisterSet(RegSet_CalleeSave, RegSet_None);
  llvm::SmallBitVector Popped(CalleeSaves.size());
  for (SizeT i = 0; i < CalleeSaves.size(); ++i) {
    SizeT Canonical = Traits::getBaseReg(i);
    if (CalleeSaves[i] && RegsUsed[i]) {
      Popped[Canonical] = true;
    }
  }
  for (SizeT i = 0; i < Popped.size(); ++i) {
    SizeT j = Popped.size() - i - 1;
    SizeT Canonical = Traits::getBaseReg(j);
    if (j == Traits::RegisterSet::Reg_ebp && IsEbpBasedFrame)
      continue;
    if (Popped[j]) {
      _pop(getPhysicalRegister(Canonical));
    }
  }

  if (!Ctx->getFlags().getUseSandboxing())
    return;
  // Change the original ret instruction into a sandboxed return sequence.
  // t:ecx = pop
  // bundle_lock
  // and t, ~31
  // jmp *t
  // bundle_unlock
  // FakeUse <original_ret_operand>
  Variable *T_ecx = makeReg(IceType_i32, Traits::RegisterSet::Reg_ecx);
  _pop(T_ecx);
  lowerIndirectJump(T_ecx);
  if (RI->getSrcSize()) {
    Variable *RetValue = llvm::cast<Variable>(RI->getSrc(0));
    Context.insert(InstFakeUse::create(Func, RetValue));
  }
  RI->setDeleted();
}

void TargetX8632::emitJumpTable(const Cfg *Func,
                                const InstJumpTable *JumpTable) const {
  if (!BuildDefs::dump())
    return;
  Ostream &Str = Ctx->getStrEmit();
  IceString MangledName = Ctx->mangleName(Func->getFunctionName());
  Str << "\t.section\t.rodata." << MangledName
      << "$jumptable,\"a\",@progbits\n";
  Str << "\t.align\t" << typeWidthInBytes(getPointerType()) << "\n";
  Str << InstJumpTable::makeName(MangledName, JumpTable->getId()) << ":";

  // On X8632 pointers are 32-bit hence the use of .long
  for (SizeT I = 0; I < JumpTable->getNumTargets(); ++I)
    Str << "\n\t.long\t" << JumpTable->getTarget(I)->getAsmName();
  Str << "\n";
}

TargetDataX8632::TargetDataX8632(GlobalContext *Ctx)
    : TargetDataLowering(Ctx) {}

namespace {
template <typename T> struct PoolTypeConverter {};

template <> struct PoolTypeConverter<float> {
  using PrimitiveIntType = uint32_t;
  using IceType = ConstantFloat;
  static const Type Ty = IceType_f32;
  static const char *TypeName;
  static const char *AsmTag;
  static const char *PrintfString;
};
const char *PoolTypeConverter<float>::TypeName = "float";
const char *PoolTypeConverter<float>::AsmTag = ".long";
const char *PoolTypeConverter<float>::PrintfString = "0x%x";

template <> struct PoolTypeConverter<double> {
  using PrimitiveIntType = uint64_t;
  using IceType = ConstantDouble;
  static const Type Ty = IceType_f64;
  static const char *TypeName;
  static const char *AsmTag;
  static const char *PrintfString;
};
const char *PoolTypeConverter<double>::TypeName = "double";
const char *PoolTypeConverter<double>::AsmTag = ".quad";
const char *PoolTypeConverter<double>::PrintfString = "0x%llx";

// Add converter for int type constant pooling
template <> struct PoolTypeConverter<uint32_t> {
  using PrimitiveIntType = uint32_t;
  using IceType = ConstantInteger32;
  static const Type Ty = IceType_i32;
  static const char *TypeName;
  static const char *AsmTag;
  static const char *PrintfString;
};
const char *PoolTypeConverter<uint32_t>::TypeName = "i32";
const char *PoolTypeConverter<uint32_t>::AsmTag = ".long";
const char *PoolTypeConverter<uint32_t>::PrintfString = "0x%x";

// Add converter for int type constant pooling
template <> struct PoolTypeConverter<uint16_t> {
  using PrimitiveIntType = uint32_t;
  using IceType = ConstantInteger32;
  static const Type Ty = IceType_i16;
  static const char *TypeName;
  static const char *AsmTag;
  static const char *PrintfString;
};
const char *PoolTypeConverter<uint16_t>::TypeName = "i16";
const char *PoolTypeConverter<uint16_t>::AsmTag = ".short";
const char *PoolTypeConverter<uint16_t>::PrintfString = "0x%x";

// Add converter for int type constant pooling
template <> struct PoolTypeConverter<uint8_t> {
  using PrimitiveIntType = uint32_t;
  using IceType = ConstantInteger32;
  static const Type Ty = IceType_i8;
  static const char *TypeName;
  static const char *AsmTag;
  static const char *PrintfString;
};
const char *PoolTypeConverter<uint8_t>::TypeName = "i8";
const char *PoolTypeConverter<uint8_t>::AsmTag = ".byte";
const char *PoolTypeConverter<uint8_t>::PrintfString = "0x%x";
} // end of anonymous namespace

template <typename T>
void TargetDataX8632::emitConstantPool(GlobalContext *Ctx) {
  if (!BuildDefs::dump())
    return;
  Ostream &Str = Ctx->getStrEmit();
  Type Ty = T::Ty;
  SizeT Align = typeAlignInBytes(Ty);
  ConstantList Pool = Ctx->getConstantPool(Ty);

  Str << "\t.section\t.rodata.cst" << Align << ",\"aM\",@progbits," << Align
      << "\n";
  Str << "\t.align\t" << Align << "\n";

  // If reorder-pooled-constants option is set to true, we need to shuffle the
  // constant pool before emitting it.
  if (Ctx->getFlags().shouldReorderPooledConstants() && !Pool.empty()) {
    // Use the constant's kind value as the salt for creating random number
    // generator.
    Operand::OperandKind K = (*Pool.begin())->getKind();

    RandomNumberGenerator RNG(Ctx->getFlags().getRandomSeed(),
                              RPE_PooledConstantReordering, K);
    RandomShuffle(Pool.begin(), Pool.end(),
                  [&RNG](uint64_t N) { return (uint32_t)RNG.next(N); });
  }

  for (Constant *C : Pool) {
    if (!C->getShouldBePooled())
      continue;
    typename T::IceType *Const = llvm::cast<typename T::IceType>(C);
    typename T::IceType::PrimType Value = Const->getValue();
    // Use memcpy() to copy bits from Value into RawValue in a way that avoids
    // breaking strict-aliasing rules.
    typename T::PrimitiveIntType RawValue;
    memcpy(&RawValue, &Value, sizeof(Value));
    char buf[30];
    int CharsPrinted =
        snprintf(buf, llvm::array_lengthof(buf), T::PrintfString, RawValue);
    assert(CharsPrinted >= 0 &&
           (size_t)CharsPrinted < llvm::array_lengthof(buf));
    (void)CharsPrinted; // avoid warnings if asserts are disabled
    Const->emitPoolLabel(Str, Ctx);
    Str << ":\n\t" << T::AsmTag << "\t" << buf << "\t# " << T::TypeName << " "
        << Value << "\n";
  }
}

void TargetDataX8632::lowerConstants() {
  if (Ctx->getFlags().getDisableTranslation())
    return;
  // No need to emit constants from the int pool since (for x86) they are
  // embedded as immediates in the instructions, just emit float/double.
  switch (Ctx->getFlags().getOutFileType()) {
  case FT_Elf: {
    ELFObjectWriter *Writer = Ctx->getObjectWriter();

    Writer->writeConstantPool<ConstantInteger32>(IceType_i8);
    Writer->writeConstantPool<ConstantInteger32>(IceType_i16);
    Writer->writeConstantPool<ConstantInteger32>(IceType_i32);

    Writer->writeConstantPool<ConstantFloat>(IceType_f32);
    Writer->writeConstantPool<ConstantDouble>(IceType_f64);
  } break;
  case FT_Asm:
  case FT_Iasm: {
    OstreamLocker L(Ctx);

    emitConstantPool<PoolTypeConverter<uint8_t>>(Ctx);
    emitConstantPool<PoolTypeConverter<uint16_t>>(Ctx);
    emitConstantPool<PoolTypeConverter<uint32_t>>(Ctx);

    emitConstantPool<PoolTypeConverter<float>>(Ctx);
    emitConstantPool<PoolTypeConverter<double>>(Ctx);
  } break;
  }
}

void TargetDataX8632::lowerJumpTables() {
  switch (Ctx->getFlags().getOutFileType()) {
  case FT_Elf: {
    ELFObjectWriter *Writer = Ctx->getObjectWriter();
    for (const JumpTableData &JT : Ctx->getJumpTables())
      Writer->writeJumpTable(JT, TargetX8632::Traits::RelFixup);
  } break;
  case FT_Asm:
    // Already emitted from Cfg
    break;
  case FT_Iasm: {
    if (!BuildDefs::dump())
      return;
    Ostream &Str = Ctx->getStrEmit();
    for (const JumpTableData &JT : Ctx->getJumpTables()) {
      Str << "\t.section\t.rodata." << JT.getFunctionName()
          << "$jumptable,\"a\",@progbits\n";
      Str << "\t.align\t" << typeWidthInBytes(getPointerType()) << "\n";
      Str << InstJumpTable::makeName(JT.getFunctionName(), JT.getId()) << ":";

      // On X8632 pointers are 32-bit hence the use of .long
      for (intptr_t TargetOffset : JT.getTargetOffsets())
        Str << "\n\t.long\t" << JT.getFunctionName() << "+" << TargetOffset;
      Str << "\n";
    }
  } break;
  }
}

void TargetDataX8632::lowerGlobals(const VariableDeclarationList &Vars,
                                   const IceString &SectionSuffix) {
  switch (Ctx->getFlags().getOutFileType()) {
  case FT_Elf: {
    ELFObjectWriter *Writer = Ctx->getObjectWriter();
    Writer->writeDataSection(Vars, TargetX8632::Traits::RelFixup,
                             SectionSuffix);
  } break;
  case FT_Asm:
  case FT_Iasm: {
    const IceString &TranslateOnly = Ctx->getFlags().getTranslateOnly();
    OstreamLocker L(Ctx);
    for (const VariableDeclaration *Var : Vars) {
      if (GlobalContext::matchSymbolName(Var->getName(), TranslateOnly)) {
        emitGlobal(*Var, SectionSuffix);
      }
    }
  } break;
  }
}

TargetHeaderX8632::TargetHeaderX8632(GlobalContext *Ctx)
    : TargetHeaderLowering(Ctx) {}

// In some cases, there are x-macros tables for both high-level and low-level
// instructions/operands that use the same enum key value. The tables are kept
// separate to maintain a proper separation between abstraction layers. There
// is a risk that the tables could get out of sync if enum values are reordered
// or if entries are added or deleted. The following dummy namespaces use
// static_asserts to ensure everything is kept in sync.

namespace {
// Validate the enum values in FCMPX8632_TABLE.
namespace dummy1 {
// Define a temporary set of enum values based on low-level table entries.
enum _tmp_enum {
#define X(val, dflt, swapS, C1, C2, swapV, pred) _tmp_##val,
  FCMPX8632_TABLE
#undef X
      _num
};
// Define a set of constants based on high-level table entries.
#define X(tag, str) static const int _table1_##tag = InstFcmp::tag;
ICEINSTFCMP_TABLE
#undef X
// Define a set of constants based on low-level table entries, and ensure the
// table entry keys are consistent.
#define X(val, dflt, swapS, C1, C2, swapV, pred)                               \
  static const int _table2_##val = _tmp_##val;                                 \
  static_assert(                                                               \
      _table1_##val == _table2_##val,                                          \
      "Inconsistency between FCMPX8632_TABLE and ICEINSTFCMP_TABLE");
FCMPX8632_TABLE
#undef X
// Repeat the static asserts with respect to the high-level table entries in
// case the high-level table has extra entries.
#define X(tag, str)                                                            \
  static_assert(                                                               \
      _table1_##tag == _table2_##tag,                                          \
      "Inconsistency between FCMPX8632_TABLE and ICEINSTFCMP_TABLE");
ICEINSTFCMP_TABLE
#undef X
} // end of namespace dummy1

// Validate the enum values in ICMPX8632_TABLE.
namespace dummy2 {
// Define a temporary set of enum values based on low-level table entries.
enum _tmp_enum {
#define X(val, C_32, C1_64, C2_64, C3_64) _tmp_##val,
  ICMPX8632_TABLE
#undef X
      _num
};
// Define a set of constants based on high-level table entries.
#define X(tag, str) static const int _table1_##tag = InstIcmp::tag;
ICEINSTICMP_TABLE
#undef X
// Define a set of constants based on low-level table entries, and ensure the
// table entry keys are consistent.
#define X(val, C_32, C1_64, C2_64, C3_64)                                      \
  static const int _table2_##val = _tmp_##val;                                 \
  static_assert(                                                               \
      _table1_##val == _table2_##val,                                          \
      "Inconsistency between ICMPX8632_TABLE and ICEINSTICMP_TABLE");
ICMPX8632_TABLE
#undef X
// Repeat the static asserts with respect to the high-level table entries in
// case the high-level table has extra entries.
#define X(tag, str)                                                            \
  static_assert(                                                               \
      _table1_##tag == _table2_##tag,                                          \
      "Inconsistency between ICMPX8632_TABLE and ICEINSTICMP_TABLE");
ICEINSTICMP_TABLE
#undef X
} // end of namespace dummy2

// Validate the enum values in ICETYPEX8632_TABLE.
namespace dummy3 {
// Define a temporary set of enum values based on low-level table entries.
enum _tmp_enum {
#define X(tag, elementty, cvt, sdss, pack, width, fld) _tmp_##tag,
  ICETYPEX8632_TABLE
#undef X
      _num
};
// Define a set of constants based on high-level table entries.
#define X(tag, sizeLog2, align, elts, elty, str)                               \
  static const int _table1_##tag = IceType_##tag;
ICETYPE_TABLE
#undef X
// Define a set of constants based on low-level table entries, and ensure the
// table entry keys are consistent.
#define X(tag, elementty, cvt, sdss, pack, width, fld)                         \
  static const int _table2_##tag = _tmp_##tag;                                 \
  static_assert(_table1_##tag == _table2_##tag,                                \
                "Inconsistency between ICETYPEX8632_TABLE and ICETYPE_TABLE");
ICETYPEX8632_TABLE
#undef X
// Repeat the static asserts with respect to the high-level table entries in
// case the high-level table has extra entries.
#define X(tag, sizeLog2, align, elts, elty, str)                               \
  static_assert(_table1_##tag == _table2_##tag,                                \
                "Inconsistency between ICETYPEX8632_TABLE and ICETYPE_TABLE");
ICETYPE_TABLE
#undef X
} // end of namespace dummy3
} // end of anonymous namespace

} // end of namespace Ice
