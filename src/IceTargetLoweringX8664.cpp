//===- subzero/src/IceTargetLoweringX8664.cpp - x86-64 lowering -----------===//
//
//                        The Subzero Code Generator
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file implements the TargetLoweringX8664 class, which
/// consists almost entirely of the lowering sequence for each
/// high-level instruction.
///
//===----------------------------------------------------------------------===//

#include "IceTargetLoweringX8664.h"

#include "IceTargetLoweringX8664Traits.h"
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
const MachineTraits<TargetX8664>::TableFcmpType
    MachineTraits<TargetX8664>::TableFcmp[] = {
#define X(val, dflt, swapS, C1, C2, swapV, pred)                               \
  {                                                                            \
    dflt, swapS, X8664::Traits::Cond::C1, X8664::Traits::Cond::C2, swapV,      \
        X8664::Traits::Cond::pred                                              \
  }                                                                            \
  ,
        FCMPX8664_TABLE
#undef X
};

const size_t MachineTraits<TargetX8664>::TableFcmpSize =
    llvm::array_lengthof(TableFcmp);

const MachineTraits<TargetX8664>::TableIcmp32Type
    MachineTraits<TargetX8664>::TableIcmp32[] = {
#define X(val, C_32, C1_64, C2_64, C3_64)                                      \
  { X8664::Traits::Cond::C_32 }                                                \
  ,
        ICMPX8664_TABLE
#undef X
};

const size_t MachineTraits<TargetX8664>::TableIcmp32Size =
    llvm::array_lengthof(TableIcmp32);

const MachineTraits<TargetX8664>::TableIcmp64Type
    MachineTraits<TargetX8664>::TableIcmp64[] = {
#define X(val, C_32, C1_64, C2_64, C3_64)                                      \
  {                                                                            \
    X8664::Traits::Cond::C1_64, X8664::Traits::Cond::C2_64,                    \
        X8664::Traits::Cond::C3_64                                             \
  }                                                                            \
  ,
        ICMPX8664_TABLE
#undef X
};

const size_t MachineTraits<TargetX8664>::TableIcmp64Size =
    llvm::array_lengthof(TableIcmp64);

const MachineTraits<TargetX8664>::TableTypeX8664AttributesType
    MachineTraits<TargetX8664>::TableTypeX8664Attributes[] = {
#define X(tag, elementty, cvt, sdss, pack, width, fld)                         \
  { elementty }                                                                \
  ,
        ICETYPEX8664_TABLE
#undef X
};

const size_t MachineTraits<TargetX8664>::TableTypeX8664AttributesSize =
    llvm::array_lengthof(TableTypeX8664Attributes);

const uint32_t MachineTraits<TargetX8664>::X86_STACK_ALIGNMENT_BYTES = 16;
const char *MachineTraits<TargetX8664>::TargetName = "X8664";

} // end of namespace X86Internal

//------------------------------------------------------------------------------
//     __      ______  __     __  ______  ______  __  __   __  ______
//    /\ \    /\  __ \/\ \  _ \ \/\  ___\/\  == \/\ \/\ "-.\ \/\  ___\
//    \ \ \___\ \ \/\ \ \ \/ ".\ \ \  __\\ \  __<\ \ \ \ \-.  \ \ \__ \
//     \ \_____\ \_____\ \__/".~\_\ \_____\ \_\ \_\ \_\ \_\\"\_\ \_____\
//      \/_____/\/_____/\/_/   \/_/\/_____/\/_/ /_/\/_/\/_/ \/_/\/_____/
//
//------------------------------------------------------------------------------
namespace {
static inline TargetX8664::Traits::RegisterSet::AllRegisters
getRegisterForXmmArgNum(uint32_t ArgNum) {
  assert(ArgNum < TargetX8664::Traits::X86_MAX_XMM_ARGS);
  return static_cast<TargetX8664::Traits::RegisterSet::AllRegisters>(
      TargetX8664::Traits::RegisterSet::Reg_xmm0 + ArgNum);
}

static inline TargetX8664::Traits::RegisterSet::AllRegisters
getRegisterForGprArgNum(uint32_t ArgNum) {
  assert(ArgNum < TargetX8664::Traits::X86_MAX_GPR_ARGS);
  static const TargetX8664::Traits::RegisterSet::AllRegisters GprForArgNum[] = {
      TargetX8664::Traits::RegisterSet::Reg_edi,
      TargetX8664::Traits::RegisterSet::Reg_esi,
      TargetX8664::Traits::RegisterSet::Reg_edx,
      TargetX8664::Traits::RegisterSet::Reg_ecx,
      TargetX8664::Traits::RegisterSet::Reg_r8d,
      TargetX8664::Traits::RegisterSet::Reg_r9d,
  };
  static_assert(llvm::array_lengthof(GprForArgNum) ==
                    TargetX8664::TargetX8664::Traits::X86_MAX_GPR_ARGS,
                "Mismatch between MAX_GPR_ARGS and GprForArgNum.");
  return GprForArgNum[ArgNum];
}

// constexprMax returns a (constexpr) max(S0, S1), and it is used for defining
// OperandList in lowerCall. std::max() is supposed to work, but it doesn't.
constexpr SizeT constexprMax(SizeT S0, SizeT S1) { return S0 < S1 ? S1 : S0; }

} // end of anonymous namespace

void TargetX8664::lowerCall(const InstCall *Instr) {
  // x86-64 calling convention:
  //
  // * At the point before the call, the stack must be aligned to 16
  // bytes.
  //
  // * The first eight arguments of vector/fp type, regardless of their
  // position relative to the other arguments in the argument list, are
  // placed in registers %xmm0 - %xmm7.
  //
  // * The first six arguments of integer types, regardless of their
  // position relative to the other arguments in the argument list, are
  // placed in registers %rdi, %rsi, %rdx, %rcx, %r8, and %r9.
  //
  // * Other arguments are pushed onto the stack in right-to-left order,
  // such that the left-most argument ends up on the top of the stack at
  // the lowest memory address.
  //
  // * Stack arguments of vector type are aligned to start at the next
  // highest multiple of 16 bytes.  Other stack arguments are aligned to
  // 8 bytes.
  //
  // This intends to match the section "Function Calling Sequence" of the
  // document "System V Application Binary Interface."
  NeedsStackAlignment = true;

  using OperandList =
      llvm::SmallVector<Operand *, constexprMax(Traits::X86_MAX_XMM_ARGS,
                                                Traits::X86_MAX_GPR_ARGS)>;
  OperandList XmmArgs;
  OperandList GprArgs;
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
    } else if (isScalarFloatingType(Ty) &&
               XmmArgs.size() < Traits::X86_MAX_XMM_ARGS) {
      XmmArgs.push_back(Arg);
    } else if (isScalarIntegerType(Ty) &&
               GprArgs.size() < Traits::X86_MAX_GPR_ARGS) {
      GprArgs.push_back(Arg);
    } else {
      StackArgs.push_back(Arg);
      if (isVectorType(Arg->getType())) {
        ParameterAreaSizeBytes =
            Traits::applyStackAlignment(ParameterAreaSizeBytes);
      }
      Variable *esp = getPhysicalRegister(Traits::RegisterSet::Reg_esp);
      Constant *Loc = Ctx->getConstantInt32(ParameterAreaSizeBytes);
      StackArgLocations.push_back(
          Traits::X86OperandMem::create(Func, Ty, esp, Loc));
      ParameterAreaSizeBytes += typeWidthInBytesOnStack(Arg->getType());
    }
  }

  // Adjust the parameter area so that the stack is aligned.  It is
  // assumed that the stack is already aligned at the start of the
  // calling sequence.
  ParameterAreaSizeBytes = Traits::applyStackAlignment(ParameterAreaSizeBytes);

  // Subtract the appropriate amount for the argument area.  This also
  // takes care of setting the stack adjustment during emission.
  //
  // TODO: If for some reason the call instruction gets dead-code
  // eliminated after lowering, we would need to ensure that the
  // pre-call and the post-call esp adjustment get eliminated as well.
  if (ParameterAreaSizeBytes) {
    _adjust_stack(ParameterAreaSizeBytes);
  }

  // Copy arguments that are passed on the stack to the appropriate
  // stack locations.
  for (SizeT i = 0, e = StackArgs.size(); i < e; ++i) {
    lowerStore(InstStore::create(Func, StackArgs[i], StackArgLocations[i]));
  }

  // Copy arguments to be passed in registers to the appropriate
  // registers.
  // TODO: Investigate the impact of lowering arguments passed in
  // registers after lowering stack arguments as opposed to the other
  // way around.  Lowering register arguments after stack arguments may
  // reduce register pressure.  On the other hand, lowering register
  // arguments first (before stack arguments) may result in more compact
  // code, as the memory operand displacements may end up being smaller
  // before any stack adjustment is done.
  for (SizeT i = 0, NumXmmArgs = XmmArgs.size(); i < NumXmmArgs; ++i) {
    Variable *Reg = legalizeToReg(XmmArgs[i], getRegisterForXmmArgNum(i));
    // Generate a FakeUse of register arguments so that they do not get
    // dead code eliminated as a result of the FakeKill of scratch
    // registers after the call.
    Context.insert(InstFakeUse::create(Func, Reg));
  }

  for (SizeT i = 0, NumGprArgs = GprArgs.size(); i < NumGprArgs; ++i) {
    Variable *Reg = legalizeToReg(GprArgs[i], getRegisterForGprArgNum(i));
    Context.insert(InstFakeUse::create(Func, Reg));
  }

  // Generate the call instruction.  Assign its result to a temporary
  // with high register allocation weight.
  Variable *Dest = Instr->getDest();
  // ReturnReg doubles as ReturnRegLo as necessary.
  Variable *ReturnReg = nullptr;
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
    case IceType_i64:
      ReturnReg = makeReg(Dest->getType(), Traits::RegisterSet::Reg_eax);
      break;
    case IceType_f32:
    case IceType_f64:
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

  Operand *CallTarget = legalize(Instr->getCallTarget(), Legal_Reg | Legal_Imm);
  const bool NeedSandboxing = Ctx->getFlags().getUseSandboxing();
  if (NeedSandboxing) {
    llvm_unreachable("X86-64 Sandboxing codegen not implemented.");
  }
  Inst *NewCall = Traits::Insts::Call::create(Func, ReturnReg, CallTarget);
  Context.insert(NewCall);
  if (NeedSandboxing) {
    llvm_unreachable("X86-64 Sandboxing codegen not implemented.");
  }

  // Add the appropriate offset to esp.  The call instruction takes care
  // of resetting the stack offset during emission.
  if (ParameterAreaSizeBytes) {
    Variable *Esp =
        Func->getTarget()->getPhysicalRegister(Traits::RegisterSet::Reg_esp);
    _add(Esp, Ctx->getConstantInt32(ParameterAreaSizeBytes));
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

  assert(ReturnReg && "x86-64 always returns value on registers.");

  if (isVectorType(Dest->getType())) {
    _movp(Dest, ReturnReg);
  } else {
    assert(isScalarFloatingType(Dest->getType()) ||
           isScalarIntegerType(Dest->getType()));
    _mov(Dest, ReturnReg);
  }
}

void TargetX8664::lowerArguments() {
  VarList &Args = Func->getArgs();
  // The first eight vetcor typed arguments (as well as fp arguments) are passed
  // in %xmm0 through %xmm7 regardless of their position in the argument list.
  unsigned NumXmmArgs = 0;
  // The first six integer typed arguments are passed in %rdi, %rsi, %rdx, %rcx,
  // %r8, and %r9 regardless of their position in the argument list.
  unsigned NumGprArgs = 0;

  Context.init(Func->getEntryNode());
  Context.setInsertPoint(Context.getCur());

  for (SizeT i = 0, End = Args.size();
       i < End && (NumXmmArgs < Traits::X86_MAX_XMM_ARGS ||
                   NumGprArgs < Traits::X86_MAX_XMM_ARGS);
       ++i) {
    Variable *Arg = Args[i];
    Type Ty = Arg->getType();
    Variable *RegisterArg = nullptr;
    int32_t RegNum = Variable::NoRegister;
    if ((isVectorType(Ty) || isScalarFloatingType(Ty))) {
      if (NumXmmArgs >= Traits::X86_MAX_XMM_ARGS) {
        continue;
      }
      RegNum = getRegisterForXmmArgNum(NumXmmArgs);
      ++NumXmmArgs;
      RegisterArg = Func->makeVariable(Ty);
    } else if (isScalarIntegerType(Ty)) {
      if (NumGprArgs >= Traits::X86_MAX_GPR_ARGS) {
        continue;
      }
      RegNum = getRegisterForGprArgNum(NumGprArgs);
      ++NumGprArgs;
      RegisterArg = Func->makeVariable(Ty);
    }
    assert(RegNum != Variable::NoRegister);
    assert(RegisterArg != nullptr);
    // Replace Arg in the argument list with the home register.  Then
    // generate an instruction in the prolog to copy the home register
    // to the assigned location of Arg.
    if (BuildDefs::dump())
      RegisterArg->setName(Func, "home_reg:" + Arg->getName(Func));
    RegisterArg->setRegNum(RegNum);
    RegisterArg->setIsArg();
    Arg->setIsArg(false);

    Args[i] = RegisterArg;
    Context.insert(InstAssign::create(Func, Arg, RegisterArg));
  }
}

void TargetX8664::lowerRet(const InstRet *Inst) {
  Variable *Reg = nullptr;
  if (Inst->hasRetValue()) {
    Operand *Src0 = legalize(Inst->getRetValue());
    if (isVectorType(Src0->getType()) ||
        isScalarFloatingType(Src0->getType())) {
      Reg = legalizeToReg(Src0, Traits::RegisterSet::Reg_xmm0);
    } else {
      assert(isScalarIntegerType(Src0->getType()));
      _mov(Reg, Src0, Traits::RegisterSet::Reg_eax);
    }
  }
  // Add a ret instruction even if sandboxing is enabled, because
  // addEpilog explicitly looks for a ret instruction as a marker for
  // where to insert the frame removal instructions.
  _ret(Reg);
  // Add a fake use of esp to make sure esp stays alive for the entire
  // function.  Otherwise post-call esp adjustments get dead-code
  // eliminated.  TODO: Are there more places where the fake use
  // should be inserted?  E.g. "void f(int n){while(1) g(n);}" may not
  // have a ret instruction.
  Variable *esp =
      Func->getTarget()->getPhysicalRegister(Traits::RegisterSet::Reg_esp);
  Context.insert(InstFakeUse::create(Func, esp));
}

void TargetX8664::addProlog(CfgNode *Node) {
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

  // Determine stack frame offsets for each Variable without a
  // register assignment.  This can be done as one variable per stack
  // slot.  Or, do coalescing by running the register allocator again
  // with an infinite set of registers (as a side effect, this gives
  // variables a second chance at physical register assignment).
  //
  // A middle ground approach is to leverage sparsity and allocate one
  // block of space on the frame for globals (variables with
  // multi-block lifetime), and one block to share for locals
  // (single-block lifetime).

  Context.init(Node);
  Context.setInsertPoint(Context.getCur());

  llvm::SmallBitVector CalleeSaves =
      getRegisterSet(RegSet_CalleeSave, RegSet_None);
  RegsUsed = llvm::SmallBitVector(CalleeSaves.size());
  VarList SortedSpilledVariables, VariablesLinkedToSpillSlots;
  size_t GlobalsSize = 0;
  // If there is a separate locals area, this represents that area.
  // Otherwise it counts any variable not counted by GlobalsSize.
  SpillAreaSizeBytes = 0;
  // If there is a separate locals area, this specifies the alignment
  // for it.
  uint32_t LocalsSlotsAlignmentBytes = 0;
  // The entire spill locations area gets aligned to largest natural
  // alignment of the variables that have a spill slot.
  uint32_t SpillAreaAlignmentBytes = 0;
  // A spill slot linked to a variable with a stack slot should reuse
  // that stack slot.
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
  for (SizeT i = 0; i < CalleeSaves.size(); ++i) {
    if (CalleeSaves[i] && RegsUsed[i]) {
      ++NumCallee;
      PreservedRegsSizeBytes += typeWidthInBytes(IceType_i64);
      _push(getPhysicalRegister(i));
    }
  }
  Ctx->statsUpdateRegistersSaved(NumCallee);

  // Generate "push ebp; mov ebp, esp"
  if (IsEbpBasedFrame) {
    assert((RegsUsed & getRegisterSet(RegSet_FramePointer, RegSet_None))
               .count() == 0);
    PreservedRegsSizeBytes += typeWidthInBytes(IceType_i64);
    Variable *ebp = getPhysicalRegister(Traits::RegisterSet::Reg_ebp);
    Variable *esp = getPhysicalRegister(Traits::RegisterSet::Reg_esp);
    _push(ebp);
    _mov(ebp, esp);
    // Keep ebp live for late-stage liveness analysis
    // (e.g. asm-verbose mode).
    Context.insert(InstFakeUse::create(Func, ebp));
  }

  // Align the variables area. SpillAreaPaddingBytes is the size of
  // the region after the preserved registers and before the spill areas.
  // LocalsSlotsPaddingBytes is the amount of padding between the globals
  // and locals area if they are separate.
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
  Ctx->statsUpdateFrameBytes(SpillAreaSizeBytes);

  resetStackAdjustment();

  // Fill in stack offsets for stack args, and copy args into registers
  // for those that were register-allocated.  Args are pushed right to
  // left, so Arg[0] is closest to the stack/frame pointer.
  Variable *FramePtr = getPhysicalRegister(getFrameOrStackReg());
  size_t BasicFrameOffset =
      PreservedRegsSizeBytes + Traits::X86_RET_IP_SIZE_BYTES;
  if (!IsEbpBasedFrame)
    BasicFrameOffset += SpillAreaSizeBytes;

  const VarList &Args = Func->getArgs();
  size_t InArgsSizeBytes = 0;
  unsigned NumXmmArgs = 0;
  unsigned NumGPRArgs = 0;
  for (Variable *Arg : Args) {
    // Skip arguments passed in registers.
    if (isVectorType(Arg->getType()) || isScalarFloatingType(Arg->getType())) {
      if (NumXmmArgs < Traits::X86_MAX_XMM_ARGS) {
        ++NumXmmArgs;
        continue;
      }
    } else {
      assert(isScalarIntegerType(Arg->getType()));
      if (NumGPRArgs < Traits::X86_MAX_GPR_ARGS) {
        ++NumGPRArgs;
        continue;
      }
    }
    finishArgumentLowering(Arg, FramePtr, BasicFrameOffset, InArgsSizeBytes);
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

void TargetX8664::addEpilog(CfgNode *Node) {
  InstList &Insts = Node->getInsts();
  InstList::reverse_iterator RI, E;
  for (RI = Insts.rbegin(), E = Insts.rend(); RI != E; ++RI) {
    if (llvm::isa<typename Traits::Insts::Ret>(*RI))
      break;
  }
  if (RI == E)
    return;

  // Convert the reverse_iterator position into its corresponding
  // (forward) iterator position.
  InstList::iterator InsertPoint = RI.base();
  --InsertPoint;
  Context.init(Node);
  Context.setInsertPoint(InsertPoint);

  Variable *esp = getPhysicalRegister(Traits::RegisterSet::Reg_esp);
  if (IsEbpBasedFrame) {
    Variable *ebp = getPhysicalRegister(Traits::RegisterSet::Reg_ebp);
    // For late-stage liveness analysis (e.g. asm-verbose mode),
    // adding a fake use of esp before the assignment of esp=ebp keeps
    // previous esp adjustments from being dead-code eliminated.
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
  for (SizeT i = 0; i < CalleeSaves.size(); ++i) {
    SizeT j = CalleeSaves.size() - i - 1;
    if (j == Traits::RegisterSet::Reg_ebp && IsEbpBasedFrame)
      continue;
    if (CalleeSaves[j] && RegsUsed[j]) {
      _pop(getPhysicalRegister(j));
    }
  }

  if (Ctx->getFlags().getUseSandboxing()) {
    llvm_unreachable("X86-64 Sandboxing codegen not implemented.");
  }
}

void TargetX8664::emitJumpTable(const Cfg *Func,
                                const InstJumpTable *JumpTable) const {
  if (!BuildDefs::dump())
    return;
  Ostream &Str = Ctx->getStrEmit();
  IceString MangledName = Ctx->mangleName(Func->getFunctionName());
  Str << "\t.section\t.rodata." << MangledName
      << "$jumptable,\"a\",@progbits\n";
  Str << "\t.align\t" << typeWidthInBytes(getPointerType()) << "\n";
  Str << InstJumpTable::makeName(MangledName, JumpTable->getId()) << ":";

  // On X8664 ILP32 pointers are 32-bit hence the use of .long
  for (SizeT I = 0; I < JumpTable->getNumTargets(); ++I)
    Str << "\n\t.long\t" << JumpTable->getTarget(I)->getAsmName();
  Str << "\n";
}

namespace {
template <typename T> struct PoolTypeConverter {};

template <> struct PoolTypeConverter<float> {
  typedef uint32_t PrimitiveIntType;
  typedef ConstantFloat IceType;
  static const Type Ty = IceType_f32;
  static const char *TypeName;
  static const char *AsmTag;
  static const char *PrintfString;
};
const char *PoolTypeConverter<float>::TypeName = "float";
const char *PoolTypeConverter<float>::AsmTag = ".long";
const char *PoolTypeConverter<float>::PrintfString = "0x%x";

template <> struct PoolTypeConverter<double> {
  typedef uint64_t PrimitiveIntType;
  typedef ConstantDouble IceType;
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
  typedef uint32_t PrimitiveIntType;
  typedef ConstantInteger32 IceType;
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
  typedef uint32_t PrimitiveIntType;
  typedef ConstantInteger32 IceType;
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
  typedef uint32_t PrimitiveIntType;
  typedef ConstantInteger32 IceType;
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
void TargetDataX8664::emitConstantPool(GlobalContext *Ctx) {
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
  if (Ctx->getFlags().shouldReorderPooledConstants()) {
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
    // Use memcpy() to copy bits from Value into RawValue in a way
    // that avoids breaking strict-aliasing rules.
    typename T::PrimitiveIntType RawValue;
    memcpy(&RawValue, &Value, sizeof(Value));
    char buf[30];
    int CharsPrinted =
        snprintf(buf, llvm::array_lengthof(buf), T::PrintfString, RawValue);
    assert(CharsPrinted >= 0 &&
           (size_t)CharsPrinted < llvm::array_lengthof(buf));
    (void)CharsPrinted; // avoid warnings if asserts are disabled
    Const->emitPoolLabel(Str);
    Str << ":\n\t" << T::AsmTag << "\t" << buf << "\t# " << T::TypeName << " "
        << Value << "\n";
  }
}

void TargetDataX8664::lowerConstants() {
  if (Ctx->getFlags().getDisableTranslation())
    return;
  // No need to emit constants from the int pool since (for x86) they
  // are embedded as immediates in the instructions, just emit float/double.
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

void TargetDataX8664::lowerJumpTables() {
  switch (Ctx->getFlags().getOutFileType()) {
  case FT_Elf: {
    ELFObjectWriter *Writer = Ctx->getObjectWriter();
    for (const JumpTableData &JumpTable : Ctx->getJumpTables())
      Writer->writeJumpTable(JumpTable, TargetX8664::Traits::RelFixup);
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

      // On X8664 ILP32 pointers are 32-bit hence the use of .long
      for (intptr_t TargetOffset : JT.getTargetOffsets())
        Str << "\n\t.long\t" << JT.getFunctionName() << "+" << TargetOffset;
      Str << "\n";
    }
  } break;
  }
}

void TargetDataX8664::lowerGlobals(const VariableDeclarationList &Vars,
                                   const IceString &SectionSuffix) {
  switch (Ctx->getFlags().getOutFileType()) {
  case FT_Elf: {
    ELFObjectWriter *Writer = Ctx->getObjectWriter();
    Writer->writeDataSection(Vars, TargetX8664::Traits::RelFixup,
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

// In some cases, there are x-macros tables for both high-level and
// low-level instructions/operands that use the same enum key value.
// The tables are kept separate to maintain a proper separation
// between abstraction layers.  There is a risk that the tables could
// get out of sync if enum values are reordered or if entries are
// added or deleted.  The following dummy namespaces use
// static_asserts to ensure everything is kept in sync.

namespace {
// Validate the enum values in FCMPX8664_TABLE.
namespace dummy1 {
// Define a temporary set of enum values based on low-level table
// entries.
enum _tmp_enum {
#define X(val, dflt, swapS, C1, C2, swapV, pred) _tmp_##val,
  FCMPX8664_TABLE
#undef X
      _num
};
// Define a set of constants based on high-level table entries.
#define X(tag, str) static const int _table1_##tag = InstFcmp::tag;
ICEINSTFCMP_TABLE
#undef X
// Define a set of constants based on low-level table entries, and
// ensure the table entry keys are consistent.
#define X(val, dflt, swapS, C1, C2, swapV, pred)                               \
  static const int _table2_##val = _tmp_##val;                                 \
  static_assert(                                                               \
      _table1_##val == _table2_##val,                                          \
      "Inconsistency between FCMPX8664_TABLE and ICEINSTFCMP_TABLE");
FCMPX8664_TABLE
#undef X
// Repeat the static asserts with respect to the high-level table
// entries in case the high-level table has extra entries.
#define X(tag, str)                                                            \
  static_assert(                                                               \
      _table1_##tag == _table2_##tag,                                          \
      "Inconsistency between FCMPX8664_TABLE and ICEINSTFCMP_TABLE");
ICEINSTFCMP_TABLE
#undef X
} // end of namespace dummy1

// Validate the enum values in ICMPX8664_TABLE.
namespace dummy2 {
// Define a temporary set of enum values based on low-level table
// entries.
enum _tmp_enum {
#define X(val, C_32, C1_64, C2_64, C3_64) _tmp_##val,
  ICMPX8664_TABLE
#undef X
      _num
};
// Define a set of constants based on high-level table entries.
#define X(tag, str) static const int _table1_##tag = InstIcmp::tag;
ICEINSTICMP_TABLE
#undef X
// Define a set of constants based on low-level table entries, and
// ensure the table entry keys are consistent.
#define X(val, C_32, C1_64, C2_64, C3_64)                                      \
  static const int _table2_##val = _tmp_##val;                                 \
  static_assert(                                                               \
      _table1_##val == _table2_##val,                                          \
      "Inconsistency between ICMPX8664_TABLE and ICEINSTICMP_TABLE");
ICMPX8664_TABLE
#undef X
// Repeat the static asserts with respect to the high-level table
// entries in case the high-level table has extra entries.
#define X(tag, str)                                                            \
  static_assert(                                                               \
      _table1_##tag == _table2_##tag,                                          \
      "Inconsistency between ICMPX8664_TABLE and ICEINSTICMP_TABLE");
ICEINSTICMP_TABLE
#undef X
} // end of namespace dummy2

// Validate the enum values in ICETYPEX8664_TABLE.
namespace dummy3 {
// Define a temporary set of enum values based on low-level table
// entries.
enum _tmp_enum {
#define X(tag, elementty, cvt, sdss, pack, width, fld) _tmp_##tag,
  ICETYPEX8664_TABLE
#undef X
      _num
};
// Define a set of constants based on high-level table entries.
#define X(tag, sizeLog2, align, elts, elty, str)                               \
  static const int _table1_##tag = tag;
ICETYPE_TABLE
#undef X
// Define a set of constants based on low-level table entries, and
// ensure the table entry keys are consistent.
#define X(tag, elementty, cvt, sdss, pack, width, fld)                         \
  static const int _table2_##tag = _tmp_##tag;                                 \
  static_assert(_table1_##tag == _table2_##tag,                                \
                "Inconsistency between ICETYPEX8664_TABLE and ICETYPE_TABLE");
ICETYPEX8664_TABLE
#undef X
// Repeat the static asserts with respect to the high-level table
// entries in case the high-level table has extra entries.
#define X(tag, sizeLog2, align, elts, elty, str)                               \
  static_assert(_table1_##tag == _table2_##tag,                                \
                "Inconsistency between ICETYPEX8664_TABLE and ICETYPE_TABLE");
ICETYPE_TABLE
#undef X
} // end of namespace dummy3
} // end of anonymous namespace

} // end of namespace Ice
