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
/// \brief Implements the TargetLoweringX8632 class, which consists almost
/// entirely of the lowering sequence for each high-level instruction.
///
//===----------------------------------------------------------------------===//

#include "IceTargetLoweringX8632.h"

#include "IceTargetLoweringX8632Traits.h"

namespace X8632 {
std::unique_ptr<::Ice::TargetLowering> createTargetLowering(::Ice::Cfg *Func) {
  return ::Ice::X8632::TargetX8632::create(Func);
}

std::unique_ptr<::Ice::TargetDataLowering>
createTargetDataLowering(::Ice::GlobalContext *Ctx) {
  return ::Ice::X8632::TargetDataX8632::create(Ctx);
}

std::unique_ptr<::Ice::TargetHeaderLowering>
createTargetHeaderLowering(::Ice::GlobalContext *Ctx) {
  return ::Ice::X8632::TargetHeaderX8632::create(Ctx);
}

void staticInit(::Ice::GlobalContext *Ctx) {
  ::Ice::X8632::TargetX8632::staticInit(Ctx);
}
} // end of namespace X8632

namespace Ice {
namespace X8632 {

//------------------------------------------------------------------------------
//      ______   ______     ______     __     ______   ______
//     /\__  _\ /\  == \   /\  __ \   /\ \   /\__  _\ /\  ___\
//     \/_/\ \/ \ \  __<   \ \  __ \  \ \ \  \/_/\ \/ \ \___  \
//        \ \_\  \ \_\ \_\  \ \_\ \_\  \ \_\    \ \_\  \/\_____\
//         \/_/   \/_/ /_/   \/_/\/_/   \/_/     \/_/   \/_____/
//
//------------------------------------------------------------------------------
const TargetX8632Traits::TableFcmpType TargetX8632Traits::TableFcmp[] = {
#define X(val, dflt, swapS, C1, C2, swapV, pred)                               \
  {                                                                            \
    dflt, swapS, X8632::Traits::Cond::C1, X8632::Traits::Cond::C2, swapV,      \
        X8632::Traits::Cond::pred                                              \
  }                                                                            \
  ,
    FCMPX8632_TABLE
#undef X
};

const size_t TargetX8632Traits::TableFcmpSize = llvm::array_lengthof(TableFcmp);

const TargetX8632Traits::TableIcmp32Type TargetX8632Traits::TableIcmp32[] = {
#define X(val, C_32, C1_64, C2_64, C3_64)                                      \
  { X8632::Traits::Cond::C_32 }                                                \
  ,
    ICMPX8632_TABLE
#undef X
};

const size_t TargetX8632Traits::TableIcmp32Size =
    llvm::array_lengthof(TableIcmp32);

const TargetX8632Traits::TableIcmp64Type TargetX8632Traits::TableIcmp64[] = {
#define X(val, C_32, C1_64, C2_64, C3_64)                                      \
  {                                                                            \
    X8632::Traits::Cond::C1_64, X8632::Traits::Cond::C2_64,                    \
        X8632::Traits::Cond::C3_64                                             \
  }                                                                            \
  ,
    ICMPX8632_TABLE
#undef X
};

const size_t TargetX8632Traits::TableIcmp64Size =
    llvm::array_lengthof(TableIcmp64);

const TargetX8632Traits::TableTypeX8632AttributesType
    TargetX8632Traits::TableTypeX8632Attributes[] = {
#define X(tag, elementty, cvt, sdss, pdps, spsd, pack, width, fld)             \
  { IceType_##elementty }                                                      \
  ,
        ICETYPEX8632_TABLE
#undef X
};

const size_t TargetX8632Traits::TableTypeX8632AttributesSize =
    llvm::array_lengthof(TableTypeX8632Attributes);

const uint32_t TargetX8632Traits::X86_STACK_ALIGNMENT_BYTES = 16;
const char *TargetX8632Traits::TargetName = "X8632";

template <>
std::array<llvm::SmallBitVector, RCX86_NUM>
    TargetX86Base<X8632::Traits>::TypeToRegisterSet = {{}};

template <>
std::array<llvm::SmallBitVector,
           TargetX86Base<X8632::Traits>::Traits::RegisterSet::Reg_NUM>
    TargetX86Base<X8632::Traits>::RegisterAliases = {{}};

template <>
FixupKind TargetX86Base<X8632::Traits>::PcRelFixup =
    TargetX86Base<X8632::Traits>::Traits::FK_PcRel;

template <>
FixupKind TargetX86Base<X8632::Traits>::AbsFixup =
    TargetX86Base<X8632::Traits>::Traits::FK_Abs;

//------------------------------------------------------------------------------
//     __      ______  __     __  ______  ______  __  __   __  ______
//    /\ \    /\  __ \/\ \  _ \ \/\  ___\/\  == \/\ \/\ "-.\ \/\  ___\
//    \ \ \___\ \ \/\ \ \ \/ ".\ \ \  __\\ \  __<\ \ \ \ \-.  \ \ \__ \
//     \ \_____\ \_____\ \__/".~\_\ \_____\ \_\ \_\ \_\ \_\\"\_\ \_____\
//      \/_____/\/_____/\/_/   \/_/\/_____/\/_/ /_/\/_/\/_/ \/_/\/_____/
//
//------------------------------------------------------------------------------
void TargetX8632::_add_sp(Operand *Adjustment) {
  Variable *esp = getPhysicalRegister(Traits::RegisterSet::Reg_esp);
  _add(esp, Adjustment);
}

void TargetX8632::_mov_sp(Operand *NewValue) {
  Variable *esp = getPhysicalRegister(Traits::RegisterSet::Reg_esp);
  _redefined(_mov(esp, NewValue));
}

void TargetX8632::_sub_sp(Operand *Adjustment) {
  Variable *esp = getPhysicalRegister(Traits::RegisterSet::Reg_esp);
  _sub(esp, Adjustment);
}

void TargetX8632::lowerIndirectJump(Variable *JumpTarget) {
  AutoBundle _(this);

  if (NeedSandboxing) {
    const SizeT BundleSize =
        1 << Func->getAssembler<>()->getBundleAlignLog2Bytes();
    _and(JumpTarget, Ctx->getConstantInt32(~(BundleSize - 1)));
  }

  _jmp(JumpTarget);
}

Inst *TargetX8632::emitCallToTarget(Operand *CallTarget, Variable *ReturnReg) {
  std::unique_ptr<AutoBundle> Bundle;
  if (NeedSandboxing) {
    if (llvm::isa<Constant>(CallTarget)) {
      Bundle = makeUnique<AutoBundle>(this, InstBundleLock::Opt_AlignToEnd);
    } else {
      Variable *CallTargetVar = nullptr;
      _mov(CallTargetVar, CallTarget);
      Bundle = makeUnique<AutoBundle>(this, InstBundleLock::Opt_AlignToEnd);
      const SizeT BundleSize =
          1 << Func->getAssembler<>()->getBundleAlignLog2Bytes();
      _and(CallTargetVar, Ctx->getConstantInt32(~(BundleSize - 1)));
      CallTarget = CallTargetVar;
    }
  }
  return Context.insert<Traits::Insts::Call>(ReturnReg, CallTarget);
}

Variable *TargetX8632::moveReturnValueToRegister(Operand *Value,
                                                 Type ReturnType) {
  if (isVectorType(ReturnType)) {
    return legalizeToReg(Value, Traits::RegisterSet::Reg_xmm0);
  } else if (isScalarFloatingType(ReturnType)) {
    _fld(Value);
    return nullptr;
  } else {
    assert(ReturnType == IceType_i32 || ReturnType == IceType_i64);
    if (ReturnType == IceType_i64) {
      Variable *eax =
          legalizeToReg(loOperand(Value), Traits::RegisterSet::Reg_eax);
      Variable *edx =
          legalizeToReg(hiOperand(Value), Traits::RegisterSet::Reg_edx);
      Context.insert<InstFakeUse>(edx);
      return eax;
    } else {
      Variable *Reg = nullptr;
      _mov(Reg, Value, Traits::RegisterSet::Reg_eax);
      return Reg;
    }
  }
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
  // | 9. padding             |
  // +------------------------+
  // | 10. out args           |
  // +------------------------+ <--- StackPointer
  //
  // The following variables record the size in bytes of the given areas:
  //  * X86_RET_IP_SIZE_BYTES:  area 1
  //  * PreservedRegsSizeBytes: area 2
  //  * SpillAreaPaddingBytes:  area 3
  //  * GlobalsSize:            area 4
  //  * GlobalsAndSubsequentPaddingSize: areas 4 - 5
  //  * LocalsSpillAreaSize:    area 6
  //  * SpillAreaSizeBytes:     areas 3 - 10
  //  * maxOutArgsSizeBytes():  area 10

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
    Context.insert<InstFakeUse>(ebp);
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

  // Functions returning scalar floating point types may need to convert values
  // from an in-register xmm value to the top of the x87 floating point stack.
  // This is done by a movp[sd] and an fld[sd].  Ensure there is enough scratch
  // space on the stack for this.
  const Type ReturnType = Func->getReturnType();
  if (isScalarFloatingType(ReturnType)) {
    // Avoid misaligned double-precicion load/store.
    NeedsStackAlignment = true;
    SpillAreaSizeBytes =
        std::max(typeWidthInBytesOnStack(ReturnType), SpillAreaSizeBytes);
  }

  // Align esp if necessary.
  if (NeedsStackAlignment) {
    uint32_t StackOffset =
        Traits::X86_RET_IP_SIZE_BYTES + PreservedRegsSizeBytes;
    uint32_t StackSize =
        Traits::applyStackAlignment(StackOffset + SpillAreaSizeBytes);
    StackSize = Traits::applyStackAlignment(StackSize + maxOutArgsSizeBytes());
    SpillAreaSizeBytes = StackSize - StackOffset;
  } else {
    SpillAreaSizeBytes += maxOutArgsSizeBytes();
  }

  // Combine fixed allocations into SpillAreaSizeBytes if we are emitting the
  // fixed allocations in the prolog.
  if (PrologEmitsFixedAllocas)
    SpillAreaSizeBytes += FixedAllocaSizeBytes;
  if (SpillAreaSizeBytes) {
    // Generate "sub esp, SpillAreaSizeBytes"
    _sub(getPhysicalRegister(Traits::RegisterSet::Reg_esp),
         Ctx->getConstantInt32(SpillAreaSizeBytes));
    // If the fixed allocas are aligned more than the stack frame, align the
    // stack pointer accordingly.
    if (PrologEmitsFixedAllocas &&
        FixedAllocaAlignBytes > Traits::X86_STACK_ALIGNMENT_BYTES) {
      assert(IsEbpBasedFrame);
      _and(getPhysicalRegister(Traits::RegisterSet::Reg_esp),
           Ctx->getConstantInt32(-FixedAllocaAlignBytes));
    }
  }

  // Account for known-frame-offset alloca instructions that were not already
  // combined into the prolog.
  if (!PrologEmitsFixedAllocas)
    SpillAreaSizeBytes += FixedAllocaSizeBytes;

  Ctx->statsUpdateFrameBytes(SpillAreaSizeBytes);

  // Fill in stack offsets for stack args, and copy args into registers for
  // those that were register-allocated. Args are pushed right to left, so
  // Arg[0] is closest to the stack/frame pointer.
  Variable *FramePtr = getPhysicalRegister(getFrameOrStackReg());
  size_t BasicFrameOffset =
      PreservedRegsSizeBytes + Traits::X86_RET_IP_SIZE_BYTES;
  if (!IsEbpBasedFrame)
    BasicFrameOffset += SpillAreaSizeBytes;

  // If there is a non-deleted InstX86GetIP instruction, we need to move it to
  // the point after the stack frame has stabilized but before
  // register-allocated in-args are copied into their home registers.  It would
  // be slightly faster to search for the GetIP instruction before other prolog
  // instructions are inserted, but it's more clear to do the whole
  // transformation in a single place.
  Traits::Insts::GetIP *GetIPInst = nullptr;
  if (Ctx->getFlags().getUseNonsfi()) {
    for (Inst &Instr : Node->getInsts()) {
      if (auto *GetIP = llvm::dyn_cast<Traits::Insts::GetIP>(&Instr)) {
        if (!Instr.isDeleted())
          GetIPInst = GetIP;
        break;
      }
    }
  }
  // Delete any existing InstX86GetIP instruction and reinsert it here.  Also,
  // insert the call to the helper function and the spill to the stack, to
  // simplify emission.
  if (GetIPInst) {
    GetIPInst->setDeleted();
    Variable *Dest = GetIPInst->getDest();
    Variable *CallDest =
        Dest->hasReg() ? Dest
                       : getPhysicalRegister(Traits::RegisterSet::Reg_eax);
    // Call the getIP_<reg> helper.
    IceString RegName = Traits::getRegName(CallDest->getRegNum());
    Constant *CallTarget = Ctx->getConstantExternSym(H_getIP_prefix + RegName);
    Context.insert<Traits::Insts::Call>(CallDest, CallTarget);
    // Insert a new version of InstX86GetIP.
    Context.insert<Traits::Insts::GetIP>(CallDest);
    // Spill the register to its home stack location if necessary.
    if (!Dest->hasReg()) {
      _mov(Dest, CallDest);
    }
  }

  const VarList &Args = Func->getArgs();
  size_t InArgsSizeBytes = 0;
  unsigned NumXmmArgs = 0;
  for (Variable *Arg : Args) {
    // Skip arguments passed in registers.
    if (isVectorType(Arg->getType()) && NumXmmArgs < Traits::X86_MAX_XMM_ARGS) {
      ++NumXmmArgs;
      continue;
    }
    // For esp-based frames where the allocas are done outside the prolog, the
    // esp value may not stabilize to its home value until after all the
    // fixed-size alloca instructions have executed.  In this case, a stack
    // adjustment is needed when accessing in-args in order to copy them into
    // registers.
    size_t StackAdjBytes = 0;
    if (!IsEbpBasedFrame && !PrologEmitsFixedAllocas)
      StackAdjBytes -= FixedAllocaSizeBytes;
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
        GlobalsAndSubsequentPaddingSize - SpillAreaPaddingBytes -
        maxOutArgsSizeBytes();
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
        << " outgoing args size = " << maxOutArgsSizeBytes() << " bytes\n"
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
    Context.insert<InstFakeUse>(esp);
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

  if (!NeedSandboxing) {
    return;
  }

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
    auto *RetValue = llvm::cast<Variable>(RI->getSrc(0));
    Context.insert<InstFakeUse>(RetValue);
  }
  RI->setDeleted();
}

void TargetX8632::emitJumpTable(const Cfg *Func,
                                const InstJumpTable *JumpTable) const {
  if (!BuildDefs::dump())
    return;
  Ostream &Str = Ctx->getStrEmit();
  const bool UseNonsfi = Ctx->getFlags().getUseNonsfi();
  const IceString MangledName = Ctx->mangleName(Func->getFunctionName());
  const IceString Prefix = UseNonsfi ? ".data.rel.ro." : ".rodata.";
  Str << "\t.section\t" << Prefix << MangledName
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
    auto *Const = llvm::cast<typename T::IceType>(C);
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
    Str << ":\n\t" << T::AsmTag << "\t" << buf << "\t/* " << T::TypeName << " "
        << Value << " */\n";
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
  const bool IsPIC = Ctx->getFlags().getUseNonsfi();
  switch (Ctx->getFlags().getOutFileType()) {
  case FT_Elf: {
    ELFObjectWriter *Writer = Ctx->getObjectWriter();
    for (const JumpTableData &JT : Ctx->getJumpTables())
      Writer->writeJumpTable(JT, TargetX8632::Traits::FK_Abs, IsPIC);
  } break;
  case FT_Asm:
    // Already emitted from Cfg
    break;
  case FT_Iasm: {
    if (!BuildDefs::dump())
      return;
    Ostream &Str = Ctx->getStrEmit();
    const IceString Prefix = IsPIC ? ".data.rel.ro." : ".rodata.";
    for (const JumpTableData &JT : Ctx->getJumpTables()) {
      Str << "\t.section\t" << Prefix << JT.getFunctionName()
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
  const bool IsPIC = Ctx->getFlags().getUseNonsfi();
  switch (Ctx->getFlags().getOutFileType()) {
  case FT_Elf: {
    ELFObjectWriter *Writer = Ctx->getObjectWriter();
    Writer->writeDataSection(Vars, TargetX8632::Traits::FK_Abs, SectionSuffix,
                             IsPIC);
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
#define X(tag, elementty, cvt, sdss, pdps, spsd, pack, width, fld) _tmp_##tag,
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
#define X(tag, elementty, cvt, sdss, pdps, spsd, pack, width, fld)             \
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

} // end of namespace X8632
} // end of namespace Ice
