//===- subzero/src/IceTargetLoweringARM32.cpp - ARM32 lowering ------------===//
//
//                        The Subzero Code Generator
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file implements the TargetLoweringARM32 class, which consists almost
/// entirely of the lowering sequence for each high-level instruction.
///
//===----------------------------------------------------------------------===//

#include "IceTargetLoweringARM32.h"

#include "IceCfg.h"
#include "IceCfgNode.h"
#include "IceClFlags.h"
#include "IceDefs.h"
#include "IceELFObjectWriter.h"
#include "IceGlobalInits.h"
#include "IceInstARM32.h"
#include "IceLiveness.h"
#include "IceOperand.h"
#include "IcePhiLoweringImpl.h"
#include "IceRegistersARM32.h"
#include "IceTargetLoweringARM32.def"
#include "IceUtils.h"
#include "llvm/Support/MathExtras.h"

namespace Ice {

namespace {

void UnimplementedError(const ClFlags &Flags) {
  if (!Flags.getSkipUnimplemented()) {
    // Use llvm_unreachable instead of report_fatal_error, which gives better
    // stack traces.
    llvm_unreachable("Not yet implemented");
    abort();
  }
}

// The following table summarizes the logic for lowering the icmp instruction
// for i32 and narrower types.  Each icmp condition has a clear mapping to an
// ARM32 conditional move instruction.

const struct TableIcmp32_ {
  CondARM32::Cond Mapping;
} TableIcmp32[] = {
#define X(val, is_signed, swapped64, C_32, C1_64, C2_64)                       \
  { CondARM32::C_32 }                                                          \
  ,
    ICMPARM32_TABLE
#undef X
};

// The following table summarizes the logic for lowering the icmp instruction
// for the i64 type. Two conditional moves are needed for setting to 1 or 0.
// The operands may need to be swapped, and there is a slight difference
// for signed vs unsigned (comparing hi vs lo first, and using cmp vs sbc).
const struct TableIcmp64_ {
  bool IsSigned;
  bool Swapped;
  CondARM32::Cond C1, C2;
} TableIcmp64[] = {
#define X(val, is_signed, swapped64, C_32, C1_64, C2_64)                       \
  { is_signed, swapped64, CondARM32::C1_64, CondARM32::C2_64 }                 \
  ,
    ICMPARM32_TABLE
#undef X
};

CondARM32::Cond getIcmp32Mapping(InstIcmp::ICond Cond) {
  size_t Index = static_cast<size_t>(Cond);
  assert(Index < llvm::array_lengthof(TableIcmp32));
  return TableIcmp32[Index].Mapping;
}

// In some cases, there are x-macros tables for both high-level and
// low-level instructions/operands that use the same enum key value.
// The tables are kept separate to maintain a proper separation
// between abstraction layers.  There is a risk that the tables could
// get out of sync if enum values are reordered or if entries are
// added or deleted.  The following dummy namespaces use
// static_asserts to ensure everything is kept in sync.

// Validate the enum values in ICMPARM32_TABLE.
namespace dummy1 {
// Define a temporary set of enum values based on low-level table
// entries.
enum _tmp_enum {
#define X(val, signed, swapped64, C_32, C1_64, C2_64) _tmp_##val,
  ICMPARM32_TABLE
#undef X
      _num
};
// Define a set of constants based on high-level table entries.
#define X(tag, str) static const int _table1_##tag = InstIcmp::tag;
ICEINSTICMP_TABLE
#undef X
// Define a set of constants based on low-level table entries, and
// ensure the table entry keys are consistent.
#define X(val, signed, swapped64, C_32, C1_64, C2_64)                          \
  static const int _table2_##val = _tmp_##val;                                 \
  static_assert(                                                               \
      _table1_##val == _table2_##val,                                          \
      "Inconsistency between ICMPARM32_TABLE and ICEINSTICMP_TABLE");
ICMPARM32_TABLE
#undef X
// Repeat the static asserts with respect to the high-level table
// entries in case the high-level table has extra entries.
#define X(tag, str)                                                            \
  static_assert(                                                               \
      _table1_##tag == _table2_##tag,                                          \
      "Inconsistency between ICMPARM32_TABLE and ICEINSTICMP_TABLE");
ICEINSTICMP_TABLE
#undef X
} // end of namespace dummy1

// Stack alignment
const uint32_t ARM32_STACK_ALIGNMENT_BYTES = 16;

// Value is in bytes. Return Value adjusted to the next highest multiple
// of the stack alignment.
uint32_t applyStackAlignment(uint32_t Value) {
  return Utils::applyAlignment(Value, ARM32_STACK_ALIGNMENT_BYTES);
}

// Value is in bytes. Return Value adjusted to the next highest multiple
// of the stack alignment required for the given type.
uint32_t applyStackAlignmentTy(uint32_t Value, Type Ty) {
  // Use natural alignment, except that normally (non-NaCl) ARM only
  // aligns vectors to 8 bytes.
  // TODO(jvoung): Check this ...
  size_t typeAlignInBytes = typeWidthInBytes(Ty);
  if (isVectorType(Ty))
    typeAlignInBytes = 8;
  return Utils::applyAlignment(Value, typeAlignInBytes);
}

// Conservatively check if at compile time we know that the operand is
// definitely a non-zero integer.
bool isGuaranteedNonzeroInt(const Operand *Op) {
  if (auto *Const = llvm::dyn_cast_or_null<ConstantInteger32>(Op)) {
    return Const->getValue() != 0;
  }
  return false;
}

} // end of anonymous namespace

TargetARM32Features::TargetARM32Features(const ClFlags &Flags) {
  static_assert(
      (ARM32InstructionSet::End - ARM32InstructionSet::Begin) ==
          (TargetInstructionSet::ARM32InstructionSet_End -
           TargetInstructionSet::ARM32InstructionSet_Begin),
      "ARM32InstructionSet range different from TargetInstructionSet");
  if (Flags.getTargetInstructionSet() !=
      TargetInstructionSet::BaseInstructionSet) {
    InstructionSet = static_cast<ARM32InstructionSet>(
        (Flags.getTargetInstructionSet() -
         TargetInstructionSet::ARM32InstructionSet_Begin) +
        ARM32InstructionSet::Begin);
  }
}

TargetARM32::TargetARM32(Cfg *Func)
    : TargetLowering(Func), CPUFeatures(Func->getContext()->getFlags()) {
  // TODO: Don't initialize IntegerRegisters and friends every time.
  // Instead, initialize in some sort of static initializer for the
  // class.
  // Limit this size (or do all bitsets need to be the same width)???
  llvm::SmallBitVector IntegerRegisters(RegARM32::Reg_NUM);
  llvm::SmallBitVector Float32Registers(RegARM32::Reg_NUM);
  llvm::SmallBitVector Float64Registers(RegARM32::Reg_NUM);
  llvm::SmallBitVector VectorRegisters(RegARM32::Reg_NUM);
  llvm::SmallBitVector InvalidRegisters(RegARM32::Reg_NUM);
  ScratchRegs.resize(RegARM32::Reg_NUM);
#define X(val, encode, name, scratch, preserved, stackptr, frameptr, isInt,    \
          isFP32, isFP64, isVec128, alias_init)                                \
  IntegerRegisters[RegARM32::val] = isInt;                                     \
  Float32Registers[RegARM32::val] = isFP32;                                    \
  Float64Registers[RegARM32::val] = isFP64;                                    \
  VectorRegisters[RegARM32::val] = isVec128;                                   \
  RegisterAliases[RegARM32::val].resize(RegARM32::Reg_NUM);                    \
  for (SizeT RegAlias : alias_init) {                                          \
    assert(!RegisterAliases[RegARM32::val][RegAlias] &&                        \
           "Duplicate alias for " #val);                                       \
    RegisterAliases[RegARM32::val].set(RegAlias);                              \
  }                                                                            \
  RegisterAliases[RegARM32::val].resize(RegARM32::Reg_NUM);                    \
  assert(RegisterAliases[RegARM32::val][RegARM32::val]);                       \
  ScratchRegs[RegARM32::val] = scratch;
  REGARM32_TABLE;
#undef X
  TypeToRegisterSet[IceType_void] = InvalidRegisters;
  TypeToRegisterSet[IceType_i1] = IntegerRegisters;
  TypeToRegisterSet[IceType_i8] = IntegerRegisters;
  TypeToRegisterSet[IceType_i16] = IntegerRegisters;
  TypeToRegisterSet[IceType_i32] = IntegerRegisters;
  TypeToRegisterSet[IceType_i64] = IntegerRegisters;
  TypeToRegisterSet[IceType_f32] = Float32Registers;
  TypeToRegisterSet[IceType_f64] = Float64Registers;
  TypeToRegisterSet[IceType_v4i1] = VectorRegisters;
  TypeToRegisterSet[IceType_v8i1] = VectorRegisters;
  TypeToRegisterSet[IceType_v16i1] = VectorRegisters;
  TypeToRegisterSet[IceType_v16i8] = VectorRegisters;
  TypeToRegisterSet[IceType_v8i16] = VectorRegisters;
  TypeToRegisterSet[IceType_v4i32] = VectorRegisters;
  TypeToRegisterSet[IceType_v4f32] = VectorRegisters;
}

void TargetARM32::translateO2() {
  TimerMarker T(TimerStack::TT_O2, Func);

  // TODO(stichnot): share passes with X86?
  // https://code.google.com/p/nativeclient/issues/detail?id=4094

  if (!Ctx->getFlags().getPhiEdgeSplit()) {
    // Lower Phi instructions.
    Func->placePhiLoads();
    if (Func->hasError())
      return;
    Func->placePhiStores();
    if (Func->hasError())
      return;
    Func->deletePhis();
    if (Func->hasError())
      return;
    Func->dump("After Phi lowering");
  }

  // Address mode optimization.
  Func->getVMetadata()->init(VMK_SingleDefs);
  Func->doAddressOpt();

  // Argument lowering
  Func->doArgLowering();

  // Target lowering.  This requires liveness analysis for some parts
  // of the lowering decisions, such as compare/branch fusing.  If
  // non-lightweight liveness analysis is used, the instructions need
  // to be renumbered first.  TODO: This renumbering should only be
  // necessary if we're actually calculating live intervals, which we
  // only do for register allocation.
  Func->renumberInstructions();
  if (Func->hasError())
    return;

  // TODO: It should be sufficient to use the fastest liveness
  // calculation, i.e. livenessLightweight().  However, for some
  // reason that slows down the rest of the translation.  Investigate.
  Func->liveness(Liveness_Basic);
  if (Func->hasError())
    return;
  Func->dump("After ARM32 address mode opt");

  Func->genCode();
  if (Func->hasError())
    return;
  Func->dump("After ARM32 codegen");

  // Register allocation.  This requires instruction renumbering and
  // full liveness analysis.
  Func->renumberInstructions();
  if (Func->hasError())
    return;
  Func->liveness(Liveness_Intervals);
  if (Func->hasError())
    return;
  // Validate the live range computations.  The expensive validation
  // call is deliberately only made when assertions are enabled.
  assert(Func->validateLiveness());
  // The post-codegen dump is done here, after liveness analysis and
  // associated cleanup, to make the dump cleaner and more useful.
  Func->dump("After initial ARM32 codegen");
  Func->getVMetadata()->init(VMK_All);
  regAlloc(RAK_Global);
  if (Func->hasError())
    return;
  Func->dump("After linear scan regalloc");

  if (Ctx->getFlags().getPhiEdgeSplit()) {
    Func->advancedPhiLowering();
    Func->dump("After advanced Phi lowering");
  }

  // Stack frame mapping.
  Func->genFrame();
  if (Func->hasError())
    return;
  Func->dump("After stack frame mapping");

  legalizeStackSlots();
  if (Func->hasError())
    return;
  Func->dump("After legalizeStackSlots");

  Func->contractEmptyNodes();
  Func->reorderNodes();

  // Branch optimization.  This needs to be done just before code
  // emission.  In particular, no transformations that insert or
  // reorder CfgNodes should be done after branch optimization.  We go
  // ahead and do it before nop insertion to reduce the amount of work
  // needed for searching for opportunities.
  Func->doBranchOpt();
  Func->dump("After branch optimization");

  // Nop insertion
  if (Ctx->getFlags().shouldDoNopInsertion()) {
    Func->doNopInsertion();
  }
}

void TargetARM32::translateOm1() {
  TimerMarker T(TimerStack::TT_Om1, Func);

  // TODO: share passes with X86?

  Func->placePhiLoads();
  if (Func->hasError())
    return;
  Func->placePhiStores();
  if (Func->hasError())
    return;
  Func->deletePhis();
  if (Func->hasError())
    return;
  Func->dump("After Phi lowering");

  Func->doArgLowering();

  Func->genCode();
  if (Func->hasError())
    return;
  Func->dump("After initial ARM32 codegen");

  regAlloc(RAK_InfOnly);
  if (Func->hasError())
    return;
  Func->dump("After regalloc of infinite-weight variables");

  Func->genFrame();
  if (Func->hasError())
    return;
  Func->dump("After stack frame mapping");

  legalizeStackSlots();
  if (Func->hasError())
    return;
  Func->dump("After legalizeStackSlots");

  // Nop insertion
  if (Ctx->getFlags().shouldDoNopInsertion()) {
    Func->doNopInsertion();
  }
}

bool TargetARM32::doBranchOpt(Inst *I, const CfgNode *NextNode) {
  if (InstARM32Br *Br = llvm::dyn_cast<InstARM32Br>(I)) {
    return Br->optimizeBranch(NextNode);
  }
  return false;
}

IceString TargetARM32::getRegName(SizeT RegNum, Type Ty) const {
  assert(RegNum < RegARM32::Reg_NUM);
  (void)Ty;
  static const char *RegNames[] = {
#define X(val, encode, name, scratch, preserved, stackptr, frameptr, isInt,    \
          isFP32, isFP64, isVec128, alias_init)                                \
  name,
      REGARM32_TABLE
#undef X
  };

  return RegNames[RegNum];
}

Variable *TargetARM32::getPhysicalRegister(SizeT RegNum, Type Ty) {
  if (Ty == IceType_void)
    Ty = IceType_i32;
  if (PhysicalRegisters[Ty].empty())
    PhysicalRegisters[Ty].resize(RegARM32::Reg_NUM);
  assert(RegNum < PhysicalRegisters[Ty].size());
  Variable *Reg = PhysicalRegisters[Ty][RegNum];
  if (Reg == nullptr) {
    Reg = Func->makeVariable(Ty);
    Reg->setRegNum(RegNum);
    PhysicalRegisters[Ty][RegNum] = Reg;
    // Specially mark SP and LR as an "argument" so that it is considered
    // live upon function entry.
    if (RegNum == RegARM32::Reg_sp || RegNum == RegARM32::Reg_lr) {
      Func->addImplicitArg(Reg);
      Reg->setIgnoreLiveness();
    }
  }
  return Reg;
}

void TargetARM32::emitJumpTable(const Cfg *Func,
                                const InstJumpTable *JumpTable) const {
  (void)JumpTable;
  UnimplementedError(Func->getContext()->getFlags());
}

void TargetARM32::emitVariable(const Variable *Var) const {
  if (!BuildDefs::dump())
    return;
  Ostream &Str = Ctx->getStrEmit();
  if (Var->hasReg()) {
    Str << getRegName(Var->getRegNum(), Var->getType());
    return;
  }
  if (Var->mustHaveReg()) {
    llvm::report_fatal_error(
        "Infinite-weight Variable has no register assigned");
  }
  int32_t Offset = Var->getStackOffset();
  int32_t BaseRegNum = Var->getBaseRegNum();
  if (BaseRegNum == Variable::NoRegister) {
    BaseRegNum = getFrameOrStackReg();
    if (!hasFramePointer())
      Offset += getStackAdjustment();
  }
  if (!isLegalVariableStackOffset(Offset)) {
    llvm::report_fatal_error("Illegal stack offset");
  }
  const Type FrameSPTy = stackSlotType();
  Str << "[" << getRegName(BaseRegNum, FrameSPTy);
  if (Offset != 0) {
    Str << ", " << getConstantPrefix() << Offset;
  }
  Str << "]";
}

bool TargetARM32::CallingConv::I64InRegs(std::pair<int32_t, int32_t> *Regs) {
  if (NumGPRRegsUsed >= ARM32_MAX_GPR_ARG)
    return false;
  int32_t RegLo, RegHi;
  // Always start i64 registers at an even register, so this may end
  // up padding away a register.
  NumGPRRegsUsed = Utils::applyAlignment(NumGPRRegsUsed, 2);
  RegLo = RegARM32::Reg_r0 + NumGPRRegsUsed;
  ++NumGPRRegsUsed;
  RegHi = RegARM32::Reg_r0 + NumGPRRegsUsed;
  ++NumGPRRegsUsed;
  // If this bumps us past the boundary, don't allocate to a register
  // and leave any previously speculatively consumed registers as consumed.
  if (NumGPRRegsUsed > ARM32_MAX_GPR_ARG)
    return false;
  Regs->first = RegLo;
  Regs->second = RegHi;
  return true;
}

bool TargetARM32::CallingConv::I32InReg(int32_t *Reg) {
  if (NumGPRRegsUsed >= ARM32_MAX_GPR_ARG)
    return false;
  *Reg = RegARM32::Reg_r0 + NumGPRRegsUsed;
  ++NumGPRRegsUsed;
  return true;
}

bool TargetARM32::CallingConv::FPInReg(Type Ty, int32_t *Reg) {
  if (NumFPRegUnits >= ARM32_MAX_FP_REG_UNITS)
    return false;
  if (isVectorType(Ty)) {
    NumFPRegUnits = Utils::applyAlignment(NumFPRegUnits, 4);
    // Q registers are declared in reverse order, so
    // RegARM32::Reg_q0 > RegARM32::Reg_q1. Therefore, we need to subtract
    // NumFPRegUnits from Reg_q0. Same thing goes for D registers.
    static_assert(RegARM32::Reg_q0 > RegARM32::Reg_q1,
                  "ARM32 Q registers are possibly declared incorrectly.");
    *Reg = RegARM32::Reg_q0 - (NumFPRegUnits / 4);
    NumFPRegUnits += 4;
    // If this bumps us past the boundary, don't allocate to a register
    // and leave any previously speculatively consumed registers as consumed.
    if (NumFPRegUnits > ARM32_MAX_FP_REG_UNITS)
      return false;
  } else if (Ty == IceType_f64) {
    static_assert(RegARM32::Reg_d0 > RegARM32::Reg_d1,
                  "ARM32 D registers are possibly declared incorrectly.");
    NumFPRegUnits = Utils::applyAlignment(NumFPRegUnits, 2);
    *Reg = RegARM32::Reg_d0 - (NumFPRegUnits / 2);
    NumFPRegUnits += 2;
    // If this bumps us past the boundary, don't allocate to a register
    // and leave any previously speculatively consumed registers as consumed.
    if (NumFPRegUnits > ARM32_MAX_FP_REG_UNITS)
      return false;
  } else {
    static_assert(RegARM32::Reg_s0 < RegARM32::Reg_s1,
                  "ARM32 S registers are possibly declared incorrectly.");
    assert(Ty == IceType_f32);
    *Reg = RegARM32::Reg_s0 + NumFPRegUnits;
    ++NumFPRegUnits;
  }
  return true;
}

void TargetARM32::lowerArguments() {
  VarList &Args = Func->getArgs();
  TargetARM32::CallingConv CC;

  // For each register argument, replace Arg in the argument list with the
  // home register.  Then generate an instruction in the prolog to copy the
  // home register to the assigned location of Arg.
  Context.init(Func->getEntryNode());
  Context.setInsertPoint(Context.getCur());

  for (SizeT I = 0, E = Args.size(); I < E; ++I) {
    Variable *Arg = Args[I];
    Type Ty = Arg->getType();
    if (Ty == IceType_i64) {
      std::pair<int32_t, int32_t> RegPair;
      if (!CC.I64InRegs(&RegPair))
        continue;
      Variable *RegisterArg = Func->makeVariable(Ty);
      Variable *RegisterLo = Func->makeVariable(IceType_i32);
      Variable *RegisterHi = Func->makeVariable(IceType_i32);
      if (BuildDefs::dump()) {
        RegisterArg->setName(Func, "home_reg:" + Arg->getName(Func));
        RegisterLo->setName(Func, "home_reg_lo:" + Arg->getName(Func));
        RegisterHi->setName(Func, "home_reg_hi:" + Arg->getName(Func));
      }
      RegisterLo->setRegNum(RegPair.first);
      RegisterLo->setIsArg();
      RegisterHi->setRegNum(RegPair.second);
      RegisterHi->setIsArg();
      RegisterArg->setLoHi(RegisterLo, RegisterHi);
      RegisterArg->setIsArg();
      Arg->setIsArg(false);

      Args[I] = RegisterArg;
      Context.insert(InstAssign::create(Func, Arg, RegisterArg));
      continue;
    } else {
      int32_t RegNum;
      if (isVectorType(Ty) || isFloatingType(Ty)) {
        if (!CC.FPInReg(Ty, &RegNum))
          continue;
      } else {
        assert(Ty == IceType_i32);
        if (!CC.I32InReg(&RegNum))
          continue;
      }
      Variable *RegisterArg = Func->makeVariable(Ty);
      if (BuildDefs::dump()) {
        RegisterArg->setName(Func, "home_reg:" + Arg->getName(Func));
      }
      RegisterArg->setRegNum(RegNum);
      RegisterArg->setIsArg();
      Arg->setIsArg(false);

      Args[I] = RegisterArg;
      Context.insert(InstAssign::create(Func, Arg, RegisterArg));
      continue;
    }
  }
}

// Helper function for addProlog().
//
// This assumes Arg is an argument passed on the stack.  This sets the
// frame offset for Arg and updates InArgsSizeBytes according to Arg's
// width.  For an I64 arg that has been split into Lo and Hi components,
// it calls itself recursively on the components, taking care to handle
// Lo first because of the little-endian architecture.  Lastly, this
// function generates an instruction to copy Arg into its assigned
// register if applicable.
void TargetARM32::finishArgumentLowering(Variable *Arg, Variable *FramePtr,
                                         size_t BasicFrameOffset,
                                         size_t &InArgsSizeBytes) {
  Variable *Lo = Arg->getLo();
  Variable *Hi = Arg->getHi();
  Type Ty = Arg->getType();
  if (Lo && Hi && Ty == IceType_i64) {
    assert(Lo->getType() != IceType_i64); // don't want infinite recursion
    assert(Hi->getType() != IceType_i64); // don't want infinite recursion
    finishArgumentLowering(Lo, FramePtr, BasicFrameOffset, InArgsSizeBytes);
    finishArgumentLowering(Hi, FramePtr, BasicFrameOffset, InArgsSizeBytes);
    return;
  }
  InArgsSizeBytes = applyStackAlignmentTy(InArgsSizeBytes, Ty);
  Arg->setStackOffset(BasicFrameOffset + InArgsSizeBytes);
  InArgsSizeBytes += typeWidthInBytesOnStack(Ty);
  // If the argument variable has been assigned a register, we need to load
  // the value from the stack slot.
  if (Arg->hasReg()) {
    assert(Ty != IceType_i64);
    OperandARM32Mem *Mem = OperandARM32Mem::create(
        Func, Ty, FramePtr, llvm::cast<ConstantInteger32>(
                                Ctx->getConstantInt32(Arg->getStackOffset())));
    if (isVectorType(Arg->getType())) {
      // Use vld1.$elem or something?
      UnimplementedError(Func->getContext()->getFlags());
    } else if (isFloatingType(Arg->getType())) {
      _vldr(Arg, Mem);
    } else {
      _ldr(Arg, Mem);
    }
    // This argument-copying instruction uses an explicit
    // OperandARM32Mem operand instead of a Variable, so its
    // fill-from-stack operation has to be tracked separately for
    // statistics.
    Ctx->statsUpdateFills();
  }
}

Type TargetARM32::stackSlotType() { return IceType_i32; }

void TargetARM32::addProlog(CfgNode *Node) {
  // Stack frame layout:
  //
  // +------------------------+
  // | 1. preserved registers |
  // +------------------------+
  // | 2. padding             |
  // +------------------------+ <--- FramePointer (if used)
  // | 3. global spill area   |
  // +------------------------+
  // | 4. padding             |
  // +------------------------+
  // | 5. local spill area    |
  // +------------------------+
  // | 6. padding             |
  // +------------------------+
  // | 7. allocas             |
  // +------------------------+ <--- StackPointer
  //
  // The following variables record the size in bytes of the given areas:
  //  * PreservedRegsSizeBytes: area 1
  //  * SpillAreaPaddingBytes:  area 2
  //  * GlobalsSize:            area 3
  //  * GlobalsAndSubsequentPaddingSize: areas 3 - 4
  //  * LocalsSpillAreaSize:    area 5
  //  * SpillAreaSizeBytes:     areas 2 - 6
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
  VarList SortedSpilledVariables;
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
  // For now, we don't have target-specific variables that need special
  // treatment (no stack-slot-linked SpillVariable type).
  std::function<bool(Variable *)> TargetVarHook =
      [](Variable *) { return false; };

  // Compute the list of spilled variables and bounds for GlobalsSize, etc.
  getVarStackSlotParams(SortedSpilledVariables, RegsUsed, &GlobalsSize,
                        &SpillAreaSizeBytes, &SpillAreaAlignmentBytes,
                        &LocalsSlotsAlignmentBytes, TargetVarHook);
  uint32_t LocalsSpillAreaSize = SpillAreaSizeBytes;
  SpillAreaSizeBytes += GlobalsSize;

  // Add push instructions for preserved registers.
  // On ARM, "push" can push a whole list of GPRs via a bitmask (0-15).
  // Unlike x86, ARM also has callee-saved float/vector registers.
  // The "vpush" instruction can handle a whole list of float/vector
  // registers, but it only handles contiguous sequences of registers
  // by specifying the start and the length.
  VarList GPRsToPreserve;
  GPRsToPreserve.reserve(CalleeSaves.size());
  uint32_t NumCallee = 0;
  size_t PreservedRegsSizeBytes = 0;
  // Consider FP and LR as callee-save / used as needed.
  if (UsesFramePointer) {
    CalleeSaves[RegARM32::Reg_fp] = true;
    assert(RegsUsed[RegARM32::Reg_fp] == false);
    RegsUsed[RegARM32::Reg_fp] = true;
  }
  if (!MaybeLeafFunc) {
    CalleeSaves[RegARM32::Reg_lr] = true;
    RegsUsed[RegARM32::Reg_lr] = true;
  }
  for (SizeT i = 0; i < CalleeSaves.size(); ++i) {
    if (CalleeSaves[i] && RegsUsed[i]) {
      // TODO(jvoung): do separate vpush for each floating point
      // register segment and += 4, or 8 depending on type.
      ++NumCallee;
      PreservedRegsSizeBytes += 4;
      GPRsToPreserve.push_back(getPhysicalRegister(i));
    }
  }
  Ctx->statsUpdateRegistersSaved(NumCallee);
  if (!GPRsToPreserve.empty())
    _push(GPRsToPreserve);

  // Generate "mov FP, SP" if needed.
  if (UsesFramePointer) {
    Variable *FP = getPhysicalRegister(RegARM32::Reg_fp);
    Variable *SP = getPhysicalRegister(RegARM32::Reg_sp);
    _mov(FP, SP);
    // Keep FP live for late-stage liveness analysis (e.g. asm-verbose mode).
    Context.insert(InstFakeUse::create(Func, FP));
  }

  // Align the variables area. SpillAreaPaddingBytes is the size of
  // the region after the preserved registers and before the spill areas.
  // LocalsSlotsPaddingBytes is the amount of padding between the globals
  // and locals area if they are separate.
  assert(SpillAreaAlignmentBytes <= ARM32_STACK_ALIGNMENT_BYTES);
  assert(LocalsSlotsAlignmentBytes <= SpillAreaAlignmentBytes);
  uint32_t SpillAreaPaddingBytes = 0;
  uint32_t LocalsSlotsPaddingBytes = 0;
  alignStackSpillAreas(PreservedRegsSizeBytes, SpillAreaAlignmentBytes,
                       GlobalsSize, LocalsSlotsAlignmentBytes,
                       &SpillAreaPaddingBytes, &LocalsSlotsPaddingBytes);
  SpillAreaSizeBytes += SpillAreaPaddingBytes + LocalsSlotsPaddingBytes;
  uint32_t GlobalsAndSubsequentPaddingSize =
      GlobalsSize + LocalsSlotsPaddingBytes;

  // Align SP if necessary.
  if (NeedsStackAlignment) {
    uint32_t StackOffset = PreservedRegsSizeBytes;
    uint32_t StackSize = applyStackAlignment(StackOffset + SpillAreaSizeBytes);
    SpillAreaSizeBytes = StackSize - StackOffset;
  }

  // Generate "sub sp, SpillAreaSizeBytes"
  if (SpillAreaSizeBytes) {
    // Use the scratch register if needed to legalize the immediate.
    Operand *SubAmount = legalize(Ctx->getConstantInt32(SpillAreaSizeBytes),
                                  Legal_Reg | Legal_Flex, getReservedTmpReg());
    Variable *SP = getPhysicalRegister(RegARM32::Reg_sp);
    _sub(SP, SP, SubAmount);
  }
  Ctx->statsUpdateFrameBytes(SpillAreaSizeBytes);

  resetStackAdjustment();

  // Fill in stack offsets for stack args, and copy args into registers
  // for those that were register-allocated.  Args are pushed right to
  // left, so Arg[0] is closest to the stack/frame pointer.
  Variable *FramePtr = getPhysicalRegister(getFrameOrStackReg());
  size_t BasicFrameOffset = PreservedRegsSizeBytes;
  if (!UsesFramePointer)
    BasicFrameOffset += SpillAreaSizeBytes;

  const VarList &Args = Func->getArgs();
  size_t InArgsSizeBytes = 0;
  TargetARM32::CallingConv CC;
  for (Variable *Arg : Args) {
    Type Ty = Arg->getType();
    bool InRegs = false;
    // Skip arguments passed in registers.
    if (isVectorType(Ty) || isFloatingType(Ty)) {
      int32_t DummyReg;
      InRegs = CC.FPInReg(Ty, &DummyReg);
    } else if (Ty == IceType_i64) {
      std::pair<int32_t, int32_t> DummyRegs;
      InRegs = CC.I64InRegs(&DummyRegs);
    } else {
      assert(Ty == IceType_i32);
      int32_t DummyReg;
      InRegs = CC.I32InReg(&DummyReg);
    }
    if (!InRegs)
      finishArgumentLowering(Arg, FramePtr, BasicFrameOffset, InArgsSizeBytes);
  }

  // Fill in stack offsets for locals.
  assignVarStackSlots(SortedSpilledVariables, SpillAreaPaddingBytes,
                      SpillAreaSizeBytes, GlobalsAndSubsequentPaddingSize,
                      UsesFramePointer);
  this->HasComputedFrame = true;

  if (BuildDefs::dump() && Func->isVerbose(IceV_Frame)) {
    OstreamLocker L(Func->getContext());
    Ostream &Str = Func->getContext()->getStrDump();

    Str << "Stack layout:\n";
    uint32_t SPAdjustmentPaddingSize =
        SpillAreaSizeBytes - LocalsSpillAreaSize -
        GlobalsAndSubsequentPaddingSize - SpillAreaPaddingBytes;
    Str << " in-args = " << InArgsSizeBytes << " bytes\n"
        << " preserved registers = " << PreservedRegsSizeBytes << " bytes\n"
        << " spill area padding = " << SpillAreaPaddingBytes << " bytes\n"
        << " globals spill area = " << GlobalsSize << " bytes\n"
        << " globals-locals spill areas intermediate padding = "
        << GlobalsAndSubsequentPaddingSize - GlobalsSize << " bytes\n"
        << " locals spill area = " << LocalsSpillAreaSize << " bytes\n"
        << " SP alignment padding = " << SPAdjustmentPaddingSize << " bytes\n";

    Str << "Stack details:\n"
        << " SP adjustment = " << SpillAreaSizeBytes << " bytes\n"
        << " spill area alignment = " << SpillAreaAlignmentBytes << " bytes\n"
        << " locals spill area alignment = " << LocalsSlotsAlignmentBytes
        << " bytes\n"
        << " is FP based = " << UsesFramePointer << "\n";
  }
}

void TargetARM32::addEpilog(CfgNode *Node) {
  InstList &Insts = Node->getInsts();
  InstList::reverse_iterator RI, E;
  for (RI = Insts.rbegin(), E = Insts.rend(); RI != E; ++RI) {
    if (llvm::isa<InstARM32Ret>(*RI))
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

  Variable *SP = getPhysicalRegister(RegARM32::Reg_sp);
  if (UsesFramePointer) {
    Variable *FP = getPhysicalRegister(RegARM32::Reg_fp);
    // For late-stage liveness analysis (e.g. asm-verbose mode),
    // adding a fake use of SP before the assignment of SP=FP keeps
    // previous SP adjustments from being dead-code eliminated.
    Context.insert(InstFakeUse::create(Func, SP));
    _mov(SP, FP);
  } else {
    // add SP, SpillAreaSizeBytes
    if (SpillAreaSizeBytes) {
      // Use the scratch register if needed to legalize the immediate.
      Operand *AddAmount =
          legalize(Ctx->getConstantInt32(SpillAreaSizeBytes),
                   Legal_Reg | Legal_Flex, getReservedTmpReg());
      _add(SP, SP, AddAmount);
    }
  }

  // Add pop instructions for preserved registers.
  llvm::SmallBitVector CalleeSaves =
      getRegisterSet(RegSet_CalleeSave, RegSet_None);
  VarList GPRsToRestore;
  GPRsToRestore.reserve(CalleeSaves.size());
  // Consider FP and LR as callee-save / used as needed.
  if (UsesFramePointer) {
    CalleeSaves[RegARM32::Reg_fp] = true;
  }
  if (!MaybeLeafFunc) {
    CalleeSaves[RegARM32::Reg_lr] = true;
  }
  // Pop registers in ascending order just like push
  // (instead of in reverse order).
  for (SizeT i = 0; i < CalleeSaves.size(); ++i) {
    if (CalleeSaves[i] && RegsUsed[i]) {
      GPRsToRestore.push_back(getPhysicalRegister(i));
    }
  }
  if (!GPRsToRestore.empty())
    _pop(GPRsToRestore);

  if (!Ctx->getFlags().getUseSandboxing())
    return;

  // Change the original ret instruction into a sandboxed return sequence.
  // bundle_lock
  // bic lr, #0xc000000f
  // bx lr
  // bundle_unlock
  // This isn't just aligning to the getBundleAlignLog2Bytes(). It needs to
  // restrict to the lower 1GB as well.
  Operand *RetMask =
      legalize(Ctx->getConstantInt32(0xc000000f), Legal_Reg | Legal_Flex);
  Variable *LR = makeReg(IceType_i32, RegARM32::Reg_lr);
  Variable *RetValue = nullptr;
  if (RI->getSrcSize())
    RetValue = llvm::cast<Variable>(RI->getSrc(0));
  _bundle_lock();
  _bic(LR, LR, RetMask);
  _ret(LR, RetValue);
  _bundle_unlock();
  RI->setDeleted();
}

bool TargetARM32::isLegalVariableStackOffset(int32_t Offset) const {
  constexpr bool SignExt = false;
  // TODO(jvoung): vldr of FP stack slots has a different limit from the
  // plain stackSlotType().
  return OperandARM32Mem::canHoldOffset(stackSlotType(), SignExt, Offset);
}

StackVariable *TargetARM32::legalizeVariableSlot(Variable *Var,
                                                 Variable *OrigBaseReg) {
  int32_t Offset = Var->getStackOffset();
  // Legalize will likely need a movw/movt combination, but if the top
  // bits are all 0 from negating the offset and subtracting, we could
  // use that instead.
  bool ShouldSub = (-Offset & 0xFFFF0000) == 0;
  if (ShouldSub)
    Offset = -Offset;
  Operand *OffsetVal = legalize(Ctx->getConstantInt32(Offset),
                                Legal_Reg | Legal_Flex, getReservedTmpReg());
  Variable *ScratchReg = makeReg(IceType_i32, getReservedTmpReg());
  if (ShouldSub)
    _sub(ScratchReg, OrigBaseReg, OffsetVal);
  else
    _add(ScratchReg, OrigBaseReg, OffsetVal);
  StackVariable *NewVar = Func->makeVariable<StackVariable>(stackSlotType());
  NewVar->setMustNotHaveReg();
  NewVar->setBaseRegNum(ScratchReg->getRegNum());
  constexpr int32_t NewOffset = 0;
  NewVar->setStackOffset(NewOffset);
  return NewVar;
}

void TargetARM32::legalizeStackSlots() {
  // If a stack variable's frame offset doesn't fit, convert from:
  //   ldr X, OFF[SP]
  // to:
  //   movw/movt TMP, OFF_PART
  //   add TMP, TMP, SP
  //   ldr X, OFF_MORE[TMP]
  //
  // This is safe because we have reserved TMP, and add for ARM does not
  // clobber the flags register.
  Func->dump("Before legalizeStackSlots");
  assert(hasComputedFrame());
  // Early exit, if SpillAreaSizeBytes is really small.
  if (isLegalVariableStackOffset(SpillAreaSizeBytes))
    return;
  Variable *OrigBaseReg = getPhysicalRegister(getFrameOrStackReg());
  int32_t StackAdjust = 0;
  // Do a fairly naive greedy clustering for now.  Pick the first stack slot
  // that's out of bounds and make a new base reg using the architecture's temp
  // register. If that works for the next slot, then great. Otherwise, create
  // a new base register, clobbering the previous base register.  Never share a
  // base reg across different basic blocks.  This isn't ideal if local and
  // multi-block variables are far apart and their references are interspersed.
  // It may help to be more coordinated about assign stack slot numbers
  // and may help to assign smaller offsets to higher-weight variables
  // so that they don't depend on this legalization.
  for (CfgNode *Node : Func->getNodes()) {
    Context.init(Node);
    StackVariable *NewBaseReg = nullptr;
    int32_t NewBaseOffset = 0;
    while (!Context.atEnd()) {
      PostIncrLoweringContext PostIncrement(Context);
      Inst *CurInstr = Context.getCur();
      Variable *Dest = CurInstr->getDest();
      // Check if the previous NewBaseReg is clobbered, and reset if needed.
      if ((Dest && NewBaseReg && Dest->hasReg() &&
           Dest->getRegNum() == NewBaseReg->getBaseRegNum()) ||
          llvm::isa<InstFakeKill>(CurInstr)) {
        NewBaseReg = nullptr;
        NewBaseOffset = 0;
      }
      // The stack adjustment only matters if we are using SP instead of FP.
      if (!hasFramePointer()) {
        if (auto *AdjInst = llvm::dyn_cast<InstARM32AdjustStack>(CurInstr)) {
          StackAdjust += AdjInst->getAmount();
          NewBaseOffset += AdjInst->getAmount();
          continue;
        }
        if (llvm::isa<InstARM32Call>(CurInstr)) {
          NewBaseOffset -= StackAdjust;
          StackAdjust = 0;
          continue;
        }
      }
      // For now, only Mov instructions can have stack variables.  We need to
      // know the type of instruction because we currently create a fresh one
      // to replace Dest/Source, rather than mutate in place.
      auto *MovInst = llvm::dyn_cast<InstARM32Mov>(CurInstr);
      if (!MovInst) {
        continue;
      }
      if (!Dest->hasReg()) {
        int32_t Offset = Dest->getStackOffset();
        Offset += StackAdjust;
        if (!isLegalVariableStackOffset(Offset)) {
          if (NewBaseReg) {
            int32_t OffsetDiff = Offset - NewBaseOffset;
            if (isLegalVariableStackOffset(OffsetDiff)) {
              StackVariable *NewDest =
                  Func->makeVariable<StackVariable>(stackSlotType());
              NewDest->setMustNotHaveReg();
              NewDest->setBaseRegNum(NewBaseReg->getBaseRegNum());
              NewDest->setStackOffset(OffsetDiff);
              Variable *NewDestVar = NewDest;
              _mov(NewDestVar, MovInst->getSrc(0));
              MovInst->setDeleted();
              continue;
            }
          }
          StackVariable *LegalDest = legalizeVariableSlot(Dest, OrigBaseReg);
          assert(LegalDest != Dest);
          Variable *LegalDestVar = LegalDest;
          _mov(LegalDestVar, MovInst->getSrc(0));
          MovInst->setDeleted();
          NewBaseReg = LegalDest;
          NewBaseOffset = Offset;
          continue;
        }
      }
      assert(MovInst->getSrcSize() == 1);
      Variable *Var = llvm::dyn_cast<Variable>(MovInst->getSrc(0));
      if (Var && !Var->hasReg()) {
        int32_t Offset = Var->getStackOffset();
        Offset += StackAdjust;
        if (!isLegalVariableStackOffset(Offset)) {
          if (NewBaseReg) {
            int32_t OffsetDiff = Offset - NewBaseOffset;
            if (isLegalVariableStackOffset(OffsetDiff)) {
              StackVariable *NewVar =
                  Func->makeVariable<StackVariable>(stackSlotType());
              NewVar->setMustNotHaveReg();
              NewVar->setBaseRegNum(NewBaseReg->getBaseRegNum());
              NewVar->setStackOffset(OffsetDiff);
              _mov(Dest, NewVar);
              MovInst->setDeleted();
              continue;
            }
          }
          StackVariable *LegalVar = legalizeVariableSlot(Var, OrigBaseReg);
          assert(LegalVar != Var);
          _mov(Dest, LegalVar);
          MovInst->setDeleted();
          NewBaseReg = LegalVar;
          NewBaseOffset = Offset;
          continue;
        }
      }
    }
  }
}

void TargetARM32::split64(Variable *Var) {
  assert(Var->getType() == IceType_i64);
  Variable *Lo = Var->getLo();
  Variable *Hi = Var->getHi();
  if (Lo) {
    assert(Hi);
    return;
  }
  assert(Hi == nullptr);
  Lo = Func->makeVariable(IceType_i32);
  Hi = Func->makeVariable(IceType_i32);
  if (BuildDefs::dump()) {
    Lo->setName(Func, Var->getName(Func) + "__lo");
    Hi->setName(Func, Var->getName(Func) + "__hi");
  }
  Var->setLoHi(Lo, Hi);
  if (Var->getIsArg()) {
    Lo->setIsArg();
    Hi->setIsArg();
  }
}

Operand *TargetARM32::loOperand(Operand *Operand) {
  assert(Operand->getType() == IceType_i64);
  if (Operand->getType() != IceType_i64)
    return Operand;
  if (auto *Var = llvm::dyn_cast<Variable>(Operand)) {
    split64(Var);
    return Var->getLo();
  }
  if (auto *Const = llvm::dyn_cast<ConstantInteger64>(Operand)) {
    return Ctx->getConstantInt32(static_cast<uint32_t>(Const->getValue()));
  }
  if (auto *Mem = llvm::dyn_cast<OperandARM32Mem>(Operand)) {
    // Conservatively disallow memory operands with side-effects (pre/post
    // increment) in case of duplication.
    assert(Mem->getAddrMode() == OperandARM32Mem::Offset ||
           Mem->getAddrMode() == OperandARM32Mem::NegOffset);
    if (Mem->isRegReg()) {
      return OperandARM32Mem::create(Func, IceType_i32, Mem->getBase(),
                                     Mem->getIndex(), Mem->getShiftOp(),
                                     Mem->getShiftAmt(), Mem->getAddrMode());
    } else {
      return OperandARM32Mem::create(Func, IceType_i32, Mem->getBase(),
                                     Mem->getOffset(), Mem->getAddrMode());
    }
  }
  llvm_unreachable("Unsupported operand type");
  return nullptr;
}

Operand *TargetARM32::hiOperand(Operand *Operand) {
  assert(Operand->getType() == IceType_i64);
  if (Operand->getType() != IceType_i64)
    return Operand;
  if (auto *Var = llvm::dyn_cast<Variable>(Operand)) {
    split64(Var);
    return Var->getHi();
  }
  if (auto *Const = llvm::dyn_cast<ConstantInteger64>(Operand)) {
    return Ctx->getConstantInt32(
        static_cast<uint32_t>(Const->getValue() >> 32));
  }
  if (auto *Mem = llvm::dyn_cast<OperandARM32Mem>(Operand)) {
    // Conservatively disallow memory operands with side-effects
    // in case of duplication.
    assert(Mem->getAddrMode() == OperandARM32Mem::Offset ||
           Mem->getAddrMode() == OperandARM32Mem::NegOffset);
    const Type SplitType = IceType_i32;
    if (Mem->isRegReg()) {
      // We have to make a temp variable T, and add 4 to either Base or Index.
      // The Index may be shifted, so adding 4 can mean something else.
      // Thus, prefer T := Base + 4, and use T as the new Base.
      Variable *Base = Mem->getBase();
      Constant *Four = Ctx->getConstantInt32(4);
      Variable *NewBase = Func->makeVariable(Base->getType());
      lowerArithmetic(InstArithmetic::create(Func, InstArithmetic::Add, NewBase,
                                             Base, Four));
      return OperandARM32Mem::create(Func, SplitType, NewBase, Mem->getIndex(),
                                     Mem->getShiftOp(), Mem->getShiftAmt(),
                                     Mem->getAddrMode());
    } else {
      Variable *Base = Mem->getBase();
      ConstantInteger32 *Offset = Mem->getOffset();
      assert(!Utils::WouldOverflowAdd(Offset->getValue(), 4));
      int32_t NextOffsetVal = Offset->getValue() + 4;
      const bool SignExt = false;
      if (!OperandARM32Mem::canHoldOffset(SplitType, SignExt, NextOffsetVal)) {
        // We have to make a temp variable and add 4 to either Base or Offset.
        // If we add 4 to Offset, this will convert a non-RegReg addressing
        // mode into a RegReg addressing mode. Since NaCl sandboxing disallows
        // RegReg addressing modes, prefer adding to base and replacing instead.
        // Thus we leave the old offset alone.
        Constant *Four = Ctx->getConstantInt32(4);
        Variable *NewBase = Func->makeVariable(Base->getType());
        lowerArithmetic(InstArithmetic::create(Func, InstArithmetic::Add,
                                               NewBase, Base, Four));
        Base = NewBase;
      } else {
        Offset =
            llvm::cast<ConstantInteger32>(Ctx->getConstantInt32(NextOffsetVal));
      }
      return OperandARM32Mem::create(Func, SplitType, Base, Offset,
                                     Mem->getAddrMode());
    }
  }
  llvm_unreachable("Unsupported operand type");
  return nullptr;
}

llvm::SmallBitVector TargetARM32::getRegisterSet(RegSetMask Include,
                                                 RegSetMask Exclude) const {
  llvm::SmallBitVector Registers(RegARM32::Reg_NUM);

#define X(val, encode, name, scratch, preserved, stackptr, frameptr, isInt,    \
          isFP32, isFP64, isVec128, alias_init)                                \
  if (scratch && (Include & RegSet_CallerSave))                                \
    Registers[RegARM32::val] = true;                                           \
  if (preserved && (Include & RegSet_CalleeSave))                              \
    Registers[RegARM32::val] = true;                                           \
  if (stackptr && (Include & RegSet_StackPointer))                             \
    Registers[RegARM32::val] = true;                                           \
  if (frameptr && (Include & RegSet_FramePointer))                             \
    Registers[RegARM32::val] = true;                                           \
  if (scratch && (Exclude & RegSet_CallerSave))                                \
    Registers[RegARM32::val] = false;                                          \
  if (preserved && (Exclude & RegSet_CalleeSave))                              \
    Registers[RegARM32::val] = false;                                          \
  if (stackptr && (Exclude & RegSet_StackPointer))                             \
    Registers[RegARM32::val] = false;                                          \
  if (frameptr && (Exclude & RegSet_FramePointer))                             \
    Registers[RegARM32::val] = false;

  REGARM32_TABLE

#undef X

  return Registers;
}

void TargetARM32::lowerAlloca(const InstAlloca *Inst) {
  UsesFramePointer = true;
  // Conservatively require the stack to be aligned.  Some stack
  // adjustment operations implemented below assume that the stack is
  // aligned before the alloca.  All the alloca code ensures that the
  // stack alignment is preserved after the alloca.  The stack alignment
  // restriction can be relaxed in some cases.
  NeedsStackAlignment = true;

  // TODO(stichnot): minimize the number of adjustments of SP, etc.
  Variable *SP = getPhysicalRegister(RegARM32::Reg_sp);
  Variable *Dest = Inst->getDest();
  uint32_t AlignmentParam = Inst->getAlignInBytes();
  // For default align=0, set it to the real value 1, to avoid any
  // bit-manipulation problems below.
  AlignmentParam = std::max(AlignmentParam, 1u);

  // LLVM enforces power of 2 alignment.
  assert(llvm::isPowerOf2_32(AlignmentParam));
  assert(llvm::isPowerOf2_32(ARM32_STACK_ALIGNMENT_BYTES));

  uint32_t Alignment = std::max(AlignmentParam, ARM32_STACK_ALIGNMENT_BYTES);
  if (Alignment > ARM32_STACK_ALIGNMENT_BYTES) {
    alignRegisterPow2(SP, Alignment);
  }
  Operand *TotalSize = Inst->getSizeInBytes();
  if (const auto *ConstantTotalSize =
          llvm::dyn_cast<ConstantInteger32>(TotalSize)) {
    uint32_t Value = ConstantTotalSize->getValue();
    Value = Utils::applyAlignment(Value, Alignment);
    Operand *SubAmount = legalize(Ctx->getConstantInt32(Value));
    _sub(SP, SP, SubAmount);
  } else {
    // Non-constant sizes need to be adjusted to the next highest
    // multiple of the required alignment at runtime.
    TotalSize = legalize(TotalSize, Legal_Reg | Legal_Flex);
    Variable *T = makeReg(IceType_i32);
    _mov(T, TotalSize);
    Operand *AddAmount = legalize(Ctx->getConstantInt32(Alignment - 1));
    _add(T, T, AddAmount);
    alignRegisterPow2(T, Alignment);
    _sub(SP, SP, T);
  }
  _mov(Dest, SP);
}

void TargetARM32::div0Check(Type Ty, Operand *SrcLo, Operand *SrcHi) {
  if (isGuaranteedNonzeroInt(SrcLo) || isGuaranteedNonzeroInt(SrcHi))
    return;
  Variable *SrcLoReg = legalizeToReg(SrcLo);
  switch (Ty) {
  default:
    llvm_unreachable("Unexpected type");
  case IceType_i8: {
    Operand *Mask =
        legalize(Ctx->getConstantInt32(0xFF), Legal_Reg | Legal_Flex);
    _tst(SrcLoReg, Mask);
    break;
  }
  case IceType_i16: {
    Operand *Mask =
        legalize(Ctx->getConstantInt32(0xFFFF), Legal_Reg | Legal_Flex);
    _tst(SrcLoReg, Mask);
    break;
  }
  case IceType_i32: {
    _tst(SrcLoReg, SrcLoReg);
    break;
  }
  case IceType_i64: {
    Variable *ScratchReg = makeReg(IceType_i32);
    _orrs(ScratchReg, SrcLoReg, SrcHi);
    // ScratchReg isn't going to be used, but we need the
    // side-effect of setting flags from this operation.
    Context.insert(InstFakeUse::create(Func, ScratchReg));
  }
  }
  InstARM32Label *Label = InstARM32Label::create(Func, this);
  _br(Label, CondARM32::NE);
  _trap();
  Context.insert(Label);
}

void TargetARM32::lowerIDivRem(Variable *Dest, Variable *T, Variable *Src0R,
                               Operand *Src1, ExtInstr ExtFunc,
                               DivInstr DivFunc, const char *DivHelperName,
                               bool IsRemainder) {
  div0Check(Dest->getType(), Src1, nullptr);
  Variable *Src1R = legalizeToReg(Src1);
  Variable *T0R = Src0R;
  Variable *T1R = Src1R;
  if (Dest->getType() != IceType_i32) {
    T0R = makeReg(IceType_i32);
    (this->*ExtFunc)(T0R, Src0R, CondARM32::AL);
    T1R = makeReg(IceType_i32);
    (this->*ExtFunc)(T1R, Src1R, CondARM32::AL);
  }
  if (hasCPUFeature(TargetARM32Features::HWDivArm)) {
    (this->*DivFunc)(T, T0R, T1R, CondARM32::AL);
    if (IsRemainder) {
      Variable *T2 = makeReg(IceType_i32);
      _mls(T2, T, T1R, T0R);
      T = T2;
    }
    _mov(Dest, T);
  } else {
    constexpr SizeT MaxSrcs = 2;
    InstCall *Call = makeHelperCall(DivHelperName, Dest, MaxSrcs);
    Call->addArg(T0R);
    Call->addArg(T1R);
    lowerCall(Call);
  }
  return;
}

void TargetARM32::lowerArithmetic(const InstArithmetic *Inst) {
  Variable *Dest = Inst->getDest();
  // TODO(jvoung): Should be able to flip Src0 and Src1 if it is easier
  // to legalize Src0 to flex or Src1 to flex and there is a reversible
  // instruction. E.g., reverse subtract with immediate, register vs
  // register, immediate.
  // Or it may be the case that the operands aren't swapped, but the
  // bits can be flipped and a different operation applied.
  // E.g., use BIC (bit clear) instead of AND for some masks.
  Operand *Src0 = legalizeUndef(Inst->getSrc(0));
  Operand *Src1 = legalizeUndef(Inst->getSrc(1));
  if (Dest->getType() == IceType_i64) {
    // These helper-call-involved instructions are lowered in this
    // separate switch. This is because we would otherwise assume that
    // we need to legalize Src0 to Src0RLo and Src0Hi. However, those go unused
    // with helper calls, and such unused/redundant instructions will fail
    // liveness analysis under -Om1 setting.
    switch (Inst->getOp()) {
    default:
      break;
    case InstArithmetic::Udiv:
    case InstArithmetic::Sdiv:
    case InstArithmetic::Urem:
    case InstArithmetic::Srem: {
      // Check for divide by 0 (ARM normally doesn't trap, but we want it
      // to trap for NaCl). Src1Lo and Src1Hi may have already been legalized
      // to a register, which will hide a constant source operand.
      // Instead, check the not-yet-legalized Src1 to optimize-out a divide
      // by 0 check.
      if (auto *C64 = llvm::dyn_cast<ConstantInteger64>(Src1)) {
        if (C64->getValue() == 0) {
          _trap();
          return;
        }
      } else {
        Operand *Src1Lo = legalize(loOperand(Src1), Legal_Reg | Legal_Flex);
        Operand *Src1Hi = legalize(hiOperand(Src1), Legal_Reg | Legal_Flex);
        div0Check(IceType_i64, Src1Lo, Src1Hi);
      }
      // Technically, ARM has their own aeabi routines, but we can use the
      // non-aeabi routine as well.  LLVM uses __aeabi_ldivmod for div,
      // but uses the more standard __moddi3 for rem.
      const char *HelperName = "";
      switch (Inst->getOp()) {
      default:
        llvm_unreachable("Should have only matched div ops.");
        break;
      case InstArithmetic::Udiv:
        HelperName = H_udiv_i64;
        break;
      case InstArithmetic::Sdiv:
        HelperName = H_sdiv_i64;
        break;
      case InstArithmetic::Urem:
        HelperName = H_urem_i64;
        break;
      case InstArithmetic::Srem:
        HelperName = H_srem_i64;
        break;
      }
      constexpr SizeT MaxSrcs = 2;
      InstCall *Call = makeHelperCall(HelperName, Dest, MaxSrcs);
      Call->addArg(Src0);
      Call->addArg(Src1);
      lowerCall(Call);
      return;
    }
    }
    Variable *DestLo = llvm::cast<Variable>(loOperand(Dest));
    Variable *DestHi = llvm::cast<Variable>(hiOperand(Dest));
    Variable *Src0RLo = legalizeToReg(loOperand(Src0));
    Variable *Src0RHi = legalizeToReg(hiOperand(Src0));
    Operand *Src1Lo = loOperand(Src1);
    Operand *Src1Hi = hiOperand(Src1);
    Variable *T_Lo = makeReg(DestLo->getType());
    Variable *T_Hi = makeReg(DestHi->getType());
    switch (Inst->getOp()) {
    case InstArithmetic::_num:
      llvm_unreachable("Unknown arithmetic operator");
      return;
    case InstArithmetic::Add:
      Src1Lo = legalize(Src1Lo, Legal_Reg | Legal_Flex);
      Src1Hi = legalize(Src1Hi, Legal_Reg | Legal_Flex);
      _adds(T_Lo, Src0RLo, Src1Lo);
      _mov(DestLo, T_Lo);
      _adc(T_Hi, Src0RHi, Src1Hi);
      _mov(DestHi, T_Hi);
      return;
    case InstArithmetic::And:
      Src1Lo = legalize(Src1Lo, Legal_Reg | Legal_Flex);
      Src1Hi = legalize(Src1Hi, Legal_Reg | Legal_Flex);
      _and(T_Lo, Src0RLo, Src1Lo);
      _mov(DestLo, T_Lo);
      _and(T_Hi, Src0RHi, Src1Hi);
      _mov(DestHi, T_Hi);
      return;
    case InstArithmetic::Or:
      Src1Lo = legalize(Src1Lo, Legal_Reg | Legal_Flex);
      Src1Hi = legalize(Src1Hi, Legal_Reg | Legal_Flex);
      _orr(T_Lo, Src0RLo, Src1Lo);
      _mov(DestLo, T_Lo);
      _orr(T_Hi, Src0RHi, Src1Hi);
      _mov(DestHi, T_Hi);
      return;
    case InstArithmetic::Xor:
      Src1Lo = legalize(Src1Lo, Legal_Reg | Legal_Flex);
      Src1Hi = legalize(Src1Hi, Legal_Reg | Legal_Flex);
      _eor(T_Lo, Src0RLo, Src1Lo);
      _mov(DestLo, T_Lo);
      _eor(T_Hi, Src0RHi, Src1Hi);
      _mov(DestHi, T_Hi);
      return;
    case InstArithmetic::Sub:
      Src1Lo = legalize(Src1Lo, Legal_Reg | Legal_Flex);
      Src1Hi = legalize(Src1Hi, Legal_Reg | Legal_Flex);
      _subs(T_Lo, Src0RLo, Src1Lo);
      _mov(DestLo, T_Lo);
      _sbc(T_Hi, Src0RHi, Src1Hi);
      _mov(DestHi, T_Hi);
      return;
    case InstArithmetic::Mul: {
      // GCC 4.8 does:
      // a=b*c ==>
      //   t_acc =(mul) (b.lo * c.hi)
      //   t_acc =(mla) (c.lo * b.hi) + t_acc
      //   t.hi,t.lo =(umull) b.lo * c.lo
      //   t.hi += t_acc
      //   a.lo = t.lo
      //   a.hi = t.hi
      //
      // LLVM does:
      //   t.hi,t.lo =(umull) b.lo * c.lo
      //   t.hi =(mla) (b.lo * c.hi) + t.hi
      //   t.hi =(mla) (b.hi * c.lo) + t.hi
      //   a.lo = t.lo
      //   a.hi = t.hi
      //
      // LLVM's lowering has fewer instructions, but more register pressure:
      // t.lo is live from beginning to end, while GCC delays the two-dest
      // instruction till the end, and kills c.hi immediately.
      Variable *T_Acc = makeReg(IceType_i32);
      Variable *T_Acc1 = makeReg(IceType_i32);
      Variable *T_Hi1 = makeReg(IceType_i32);
      Variable *Src1RLo = legalizeToReg(Src1Lo);
      Variable *Src1RHi = legalizeToReg(Src1Hi);
      _mul(T_Acc, Src0RLo, Src1RHi);
      _mla(T_Acc1, Src1RLo, Src0RHi, T_Acc);
      _umull(T_Lo, T_Hi1, Src0RLo, Src1RLo);
      _add(T_Hi, T_Hi1, T_Acc1);
      _mov(DestLo, T_Lo);
      _mov(DestHi, T_Hi);
      return;
    }
    case InstArithmetic::Shl: {
      // a=b<<c ==>
      // GCC 4.8 does:
      // sub t_c1, c.lo, #32
      // lsl t_hi, b.hi, c.lo
      // orr t_hi, t_hi, b.lo, lsl t_c1
      // rsb t_c2, c.lo, #32
      // orr t_hi, t_hi, b.lo, lsr t_c2
      // lsl t_lo, b.lo, c.lo
      // a.lo = t_lo
      // a.hi = t_hi
      // Can be strength-reduced for constant-shifts, but we don't do
      // that for now.
      // Given the sub/rsb T_C, C.lo, #32, one of the T_C will be negative.
      // On ARM, shifts only take the lower 8 bits of the shift register,
      // and saturate to the range 0-32, so the negative value will
      // saturate to 32.
      Variable *T_Hi = makeReg(IceType_i32);
      Variable *Src1RLo = legalizeToReg(Src1Lo);
      Constant *ThirtyTwo = Ctx->getConstantInt32(32);
      Variable *T_C1 = makeReg(IceType_i32);
      Variable *T_C2 = makeReg(IceType_i32);
      _sub(T_C1, Src1RLo, ThirtyTwo);
      _lsl(T_Hi, Src0RHi, Src1RLo);
      _orr(T_Hi, T_Hi, OperandARM32FlexReg::create(Func, IceType_i32, Src0RLo,
                                                   OperandARM32::LSL, T_C1));
      _rsb(T_C2, Src1RLo, ThirtyTwo);
      _orr(T_Hi, T_Hi, OperandARM32FlexReg::create(Func, IceType_i32, Src0RLo,
                                                   OperandARM32::LSR, T_C2));
      _mov(DestHi, T_Hi);
      Variable *T_Lo = makeReg(IceType_i32);
      // _mov seems to sometimes have better register preferencing than lsl.
      // Otherwise mov w/ lsl shifted register is a pseudo-instruction
      // that maps to lsl.
      _mov(T_Lo, OperandARM32FlexReg::create(Func, IceType_i32, Src0RLo,
                                             OperandARM32::LSL, Src1RLo));
      _mov(DestLo, T_Lo);
      return;
    }
    case InstArithmetic::Lshr:
    // a=b>>c (unsigned) ==>
    // GCC 4.8 does:
    // rsb t_c1, c.lo, #32
    // lsr t_lo, b.lo, c.lo
    // orr t_lo, t_lo, b.hi, lsl t_c1
    // sub t_c2, c.lo, #32
    // orr t_lo, t_lo, b.hi, lsr t_c2
    // lsr t_hi, b.hi, c.lo
    // a.lo = t_lo
    // a.hi = t_hi
    case InstArithmetic::Ashr: {
      // a=b>>c (signed) ==> ...
      // Ashr is similar, but the sub t_c2, c.lo, #32 should set flags,
      // and the next orr should be conditioned on PLUS. The last two
      // right shifts should also be arithmetic.
      bool IsAshr = Inst->getOp() == InstArithmetic::Ashr;
      Variable *T_Lo = makeReg(IceType_i32);
      Variable *Src1RLo = legalizeToReg(Src1Lo);
      Constant *ThirtyTwo = Ctx->getConstantInt32(32);
      Variable *T_C1 = makeReg(IceType_i32);
      Variable *T_C2 = makeReg(IceType_i32);
      _rsb(T_C1, Src1RLo, ThirtyTwo);
      _lsr(T_Lo, Src0RLo, Src1RLo);
      _orr(T_Lo, T_Lo, OperandARM32FlexReg::create(Func, IceType_i32, Src0RHi,
                                                   OperandARM32::LSL, T_C1));
      OperandARM32::ShiftKind RShiftKind;
      CondARM32::Cond Pred;
      if (IsAshr) {
        _subs(T_C2, Src1RLo, ThirtyTwo);
        RShiftKind = OperandARM32::ASR;
        Pred = CondARM32::PL;
      } else {
        _sub(T_C2, Src1RLo, ThirtyTwo);
        RShiftKind = OperandARM32::LSR;
        Pred = CondARM32::AL;
      }
      _orr(T_Lo, T_Lo, OperandARM32FlexReg::create(Func, IceType_i32, Src0RHi,
                                                   RShiftKind, T_C2),
           Pred);
      _mov(DestLo, T_Lo);
      Variable *T_Hi = makeReg(IceType_i32);
      _mov(T_Hi, OperandARM32FlexReg::create(Func, IceType_i32, Src0RHi,
                                             RShiftKind, Src1RLo));
      _mov(DestHi, T_Hi);
      return;
    }
    case InstArithmetic::Fadd:
    case InstArithmetic::Fsub:
    case InstArithmetic::Fmul:
    case InstArithmetic::Fdiv:
    case InstArithmetic::Frem:
      llvm_unreachable("FP instruction with i64 type");
      return;
    case InstArithmetic::Udiv:
    case InstArithmetic::Sdiv:
    case InstArithmetic::Urem:
    case InstArithmetic::Srem:
      llvm_unreachable("Call-helper-involved instruction for i64 type "
                       "should have already been handled before");
      return;
    }
    return;
  } else if (isVectorType(Dest->getType())) {
    UnimplementedError(Func->getContext()->getFlags());
    // Add a fake def to keep liveness consistent in the meantime.
    Context.insert(InstFakeDef::create(Func, Dest));
    return;
  }
  // Dest->getType() is a non-i64 scalar.
  Variable *Src0R = legalizeToReg(Src0);
  Variable *T = makeReg(Dest->getType());
  // Handle div/rem separately. They require a non-legalized Src1 to inspect
  // whether or not Src1 is a non-zero constant. Once legalized it is more
  // difficult to determine (constant may be moved to a register).
  switch (Inst->getOp()) {
  default:
    break;
  case InstArithmetic::Udiv: {
    constexpr bool IsRemainder = false;
    lowerIDivRem(Dest, T, Src0R, Src1, &TargetARM32::_uxt, &TargetARM32::_udiv,
                 H_udiv_i32, IsRemainder);
    return;
  }
  case InstArithmetic::Sdiv: {
    constexpr bool IsRemainder = false;
    lowerIDivRem(Dest, T, Src0R, Src1, &TargetARM32::_sxt, &TargetARM32::_sdiv,
                 H_sdiv_i32, IsRemainder);
    return;
  }
  case InstArithmetic::Urem: {
    constexpr bool IsRemainder = true;
    lowerIDivRem(Dest, T, Src0R, Src1, &TargetARM32::_uxt, &TargetARM32::_udiv,
                 H_urem_i32, IsRemainder);
    return;
  }
  case InstArithmetic::Srem: {
    constexpr bool IsRemainder = true;
    lowerIDivRem(Dest, T, Src0R, Src1, &TargetARM32::_sxt, &TargetARM32::_sdiv,
                 H_srem_i32, IsRemainder);
    return;
  }
  case InstArithmetic::Frem: {
    const SizeT MaxSrcs = 2;
    Type Ty = Dest->getType();
    InstCall *Call = makeHelperCall(
        isFloat32Asserting32Or64(Ty) ? H_frem_f32 : H_frem_f64, Dest, MaxSrcs);
    Call->addArg(Src0R);
    Call->addArg(Src1);
    lowerCall(Call);
    return;
  }
  }

  // Handle floating point arithmetic separately: they require Src1 to be
  // legalized to a register.
  switch (Inst->getOp()) {
  default:
    break;
  case InstArithmetic::Fadd: {
    Variable *Src1R = legalizeToReg(Src1);
    _vadd(T, Src0R, Src1R);
    _vmov(Dest, T);
    return;
  }
  case InstArithmetic::Fsub: {
    Variable *Src1R = legalizeToReg(Src1);
    _vsub(T, Src0R, Src1R);
    _vmov(Dest, T);
    return;
  }
  case InstArithmetic::Fmul: {
    Variable *Src1R = legalizeToReg(Src1);
    _vmul(T, Src0R, Src1R);
    _vmov(Dest, T);
    return;
  }
  case InstArithmetic::Fdiv: {
    Variable *Src1R = legalizeToReg(Src1);
    _vdiv(T, Src0R, Src1R);
    _vmov(Dest, T);
    return;
  }
  }

  Operand *Src1RF = legalize(Src1, Legal_Reg | Legal_Flex);
  switch (Inst->getOp()) {
  case InstArithmetic::_num:
    llvm_unreachable("Unknown arithmetic operator");
    return;
  case InstArithmetic::Add:
    _add(T, Src0R, Src1RF);
    _mov(Dest, T);
    return;
  case InstArithmetic::And:
    _and(T, Src0R, Src1RF);
    _mov(Dest, T);
    return;
  case InstArithmetic::Or:
    _orr(T, Src0R, Src1RF);
    _mov(Dest, T);
    return;
  case InstArithmetic::Xor:
    _eor(T, Src0R, Src1RF);
    _mov(Dest, T);
    return;
  case InstArithmetic::Sub:
    _sub(T, Src0R, Src1RF);
    _mov(Dest, T);
    return;
  case InstArithmetic::Mul: {
    Variable *Src1R = legalizeToReg(Src1RF);
    _mul(T, Src0R, Src1R);
    _mov(Dest, T);
    return;
  }
  case InstArithmetic::Shl:
    _lsl(T, Src0R, Src1RF);
    _mov(Dest, T);
    return;
  case InstArithmetic::Lshr:
    _lsr(T, Src0R, Src1RF);
    _mov(Dest, T);
    return;
  case InstArithmetic::Ashr:
    _asr(T, Src0R, Src1RF);
    _mov(Dest, T);
    return;
  case InstArithmetic::Udiv:
  case InstArithmetic::Sdiv:
  case InstArithmetic::Urem:
  case InstArithmetic::Srem:
    llvm_unreachable("Integer div/rem should have been handled earlier.");
    return;
  case InstArithmetic::Fadd:
  case InstArithmetic::Fsub:
  case InstArithmetic::Fmul:
  case InstArithmetic::Fdiv:
  case InstArithmetic::Frem:
    llvm_unreachable("Floating point arith should have been handled earlier.");
    return;
  }
}

void TargetARM32::lowerAssign(const InstAssign *Inst) {
  Variable *Dest = Inst->getDest();
  Operand *Src0 = Inst->getSrc(0);
  assert(Dest->getType() == Src0->getType());
  if (Dest->getType() == IceType_i64) {
    Src0 = legalizeUndef(Src0);
    Operand *Src0Lo = legalize(loOperand(Src0), Legal_Reg | Legal_Flex);
    Operand *Src0Hi = legalize(hiOperand(Src0), Legal_Reg | Legal_Flex);
    Variable *DestLo = llvm::cast<Variable>(loOperand(Dest));
    Variable *DestHi = llvm::cast<Variable>(hiOperand(Dest));
    Variable *T_Lo = nullptr, *T_Hi = nullptr;
    _mov(T_Lo, Src0Lo);
    _mov(DestLo, T_Lo);
    _mov(T_Hi, Src0Hi);
    _mov(DestHi, T_Hi);
  } else {
    Operand *NewSrc;
    if (Dest->hasReg()) {
      // If Dest already has a physical register, then legalize the Src operand
      // into a Variable with the same register assignment.  This especially
      // helps allow the use of Flex operands.
      NewSrc = legalize(Src0, Legal_Reg | Legal_Flex, Dest->getRegNum());
    } else {
      // Dest could be a stack operand. Since we could potentially need
      // to do a Store (and store can only have Register operands),
      // legalize this to a register.
      NewSrc = legalize(Src0, Legal_Reg);
    }
    if (isVectorType(Dest->getType())) {
      UnimplementedError(Func->getContext()->getFlags());
    } else if (isFloatingType(Dest->getType())) {
      Variable *SrcR = legalizeToReg(NewSrc);
      _vmov(Dest, SrcR);
    } else {
      _mov(Dest, NewSrc);
    }
  }
}

void TargetARM32::lowerBr(const InstBr *Inst) {
  if (Inst->isUnconditional()) {
    _br(Inst->getTargetUnconditional());
    return;
  }
  Operand *Cond = Inst->getCondition();
  // TODO(jvoung): Handle folding opportunities.

  Variable *Src0R = legalizeToReg(Cond);
  Constant *Zero = Ctx->getConstantZero(IceType_i32);
  _cmp(Src0R, Zero);
  _br(Inst->getTargetTrue(), Inst->getTargetFalse(), CondARM32::NE);
}

void TargetARM32::lowerCall(const InstCall *Instr) {
  MaybeLeafFunc = false;
  NeedsStackAlignment = true;

  // Assign arguments to registers and stack. Also reserve stack.
  TargetARM32::CallingConv CC;
  // Pair of Arg Operand -> GPR number assignments.
  llvm::SmallVector<std::pair<Operand *, int32_t>,
                    TargetARM32::CallingConv::ARM32_MAX_GPR_ARG> GPRArgs;
  llvm::SmallVector<std::pair<Operand *, int32_t>,
                    TargetARM32::CallingConv::ARM32_MAX_FP_REG_UNITS> FPArgs;
  // Pair of Arg Operand -> stack offset.
  llvm::SmallVector<std::pair<Operand *, int32_t>, 8> StackArgs;
  int32_t ParameterAreaSizeBytes = 0;

  // Classify each argument operand according to the location where the
  // argument is passed.
  for (SizeT i = 0, NumArgs = Instr->getNumArgs(); i < NumArgs; ++i) {
    Operand *Arg = legalizeUndef(Instr->getArg(i));
    Type Ty = Arg->getType();
    bool InRegs = false;
    if (Ty == IceType_i64) {
      std::pair<int32_t, int32_t> Regs;
      if (CC.I64InRegs(&Regs)) {
        InRegs = true;
        Operand *Lo = loOperand(Arg);
        Operand *Hi = hiOperand(Arg);
        GPRArgs.push_back(std::make_pair(Lo, Regs.first));
        GPRArgs.push_back(std::make_pair(Hi, Regs.second));
      }
    } else if (isVectorType(Ty) || isFloatingType(Ty)) {
      int32_t Reg;
      if (CC.FPInReg(Ty, &Reg)) {
        InRegs = true;
        FPArgs.push_back(std::make_pair(Arg, Reg));
      }
    } else {
      assert(Ty == IceType_i32);
      int32_t Reg;
      if (CC.I32InReg(&Reg)) {
        InRegs = true;
        GPRArgs.push_back(std::make_pair(Arg, Reg));
      }
    }

    if (!InRegs) {
      ParameterAreaSizeBytes =
          applyStackAlignmentTy(ParameterAreaSizeBytes, Ty);
      StackArgs.push_back(std::make_pair(Arg, ParameterAreaSizeBytes));
      ParameterAreaSizeBytes += typeWidthInBytesOnStack(Arg->getType());
    }
  }

  // Adjust the parameter area so that the stack is aligned.  It is
  // assumed that the stack is already aligned at the start of the
  // calling sequence.
  ParameterAreaSizeBytes = applyStackAlignment(ParameterAreaSizeBytes);

  // Subtract the appropriate amount for the argument area.  This also
  // takes care of setting the stack adjustment during emission.
  //
  // TODO: If for some reason the call instruction gets dead-code
  // eliminated after lowering, we would need to ensure that the
  // pre-call and the post-call esp adjustment get eliminated as well.
  if (ParameterAreaSizeBytes) {
    Operand *SubAmount = legalize(Ctx->getConstantInt32(ParameterAreaSizeBytes),
                                  Legal_Reg | Legal_Flex);
    _adjust_stack(ParameterAreaSizeBytes, SubAmount);
  }

  // Copy arguments that are passed on the stack to the appropriate
  // stack locations.
  Variable *SP = getPhysicalRegister(RegARM32::Reg_sp);
  for (auto &StackArg : StackArgs) {
    ConstantInteger32 *Loc =
        llvm::cast<ConstantInteger32>(Ctx->getConstantInt32(StackArg.second));
    Type Ty = StackArg.first->getType();
    OperandARM32Mem *Addr;
    constexpr bool SignExt = false;
    if (OperandARM32Mem::canHoldOffset(Ty, SignExt, StackArg.second)) {
      Addr = OperandARM32Mem::create(Func, Ty, SP, Loc);
    } else {
      Variable *NewBase = Func->makeVariable(SP->getType());
      lowerArithmetic(
          InstArithmetic::create(Func, InstArithmetic::Add, NewBase, SP, Loc));
      Addr = formMemoryOperand(NewBase, Ty);
    }
    lowerStore(InstStore::create(Func, StackArg.first, Addr));
  }

  // Copy arguments to be passed in registers to the appropriate registers.
  for (auto &GPRArg : GPRArgs) {
    Variable *Reg = legalizeToReg(GPRArg.first, GPRArg.second);
    // Generate a FakeUse of register arguments so that they do not get
    // dead code eliminated as a result of the FakeKill of scratch
    // registers after the call.
    Context.insert(InstFakeUse::create(Func, Reg));
  }
  for (auto &FPArg : FPArgs) {
    Variable *Reg = legalizeToReg(FPArg.first, FPArg.second);
    Context.insert(InstFakeUse::create(Func, Reg));
  }

  // Generate the call instruction.  Assign its result to a temporary
  // with high register allocation weight.
  Variable *Dest = Instr->getDest();
  // ReturnReg doubles as ReturnRegLo as necessary.
  Variable *ReturnReg = nullptr;
  Variable *ReturnRegHi = nullptr;
  if (Dest) {
    switch (Dest->getType()) {
    case IceType_NUM:
      llvm_unreachable("Invalid Call dest type");
      break;
    case IceType_void:
      break;
    case IceType_i1:
    case IceType_i8:
    case IceType_i16:
    case IceType_i32:
      ReturnReg = makeReg(Dest->getType(), RegARM32::Reg_r0);
      break;
    case IceType_i64:
      ReturnReg = makeReg(IceType_i32, RegARM32::Reg_r0);
      ReturnRegHi = makeReg(IceType_i32, RegARM32::Reg_r1);
      break;
    case IceType_f32:
      ReturnReg = makeReg(Dest->getType(), RegARM32::Reg_s0);
      break;
    case IceType_f64:
      ReturnReg = makeReg(Dest->getType(), RegARM32::Reg_d0);
      break;
    case IceType_v4i1:
    case IceType_v8i1:
    case IceType_v16i1:
    case IceType_v16i8:
    case IceType_v8i16:
    case IceType_v4i32:
    case IceType_v4f32:
      ReturnReg = makeReg(Dest->getType(), RegARM32::Reg_q0);
      break;
    }
  }
  Operand *CallTarget = Instr->getCallTarget();
  // TODO(jvoung): Handle sandboxing.
  // const bool NeedSandboxing = Ctx->getFlags().getUseSandboxing();

  // Allow ConstantRelocatable to be left alone as a direct call,
  // but force other constants like ConstantInteger32 to be in
  // a register and make it an indirect call.
  if (!llvm::isa<ConstantRelocatable>(CallTarget)) {
    CallTarget = legalize(CallTarget, Legal_Reg);
  }
  Inst *NewCall = InstARM32Call::create(Func, ReturnReg, CallTarget);
  Context.insert(NewCall);
  if (ReturnRegHi)
    Context.insert(InstFakeDef::create(Func, ReturnRegHi));

  // Add the appropriate offset to SP.  The call instruction takes care
  // of resetting the stack offset during emission.
  if (ParameterAreaSizeBytes) {
    Operand *AddAmount = legalize(Ctx->getConstantInt32(ParameterAreaSizeBytes),
                                  Legal_Reg | Legal_Flex);
    Variable *SP = getPhysicalRegister(RegARM32::Reg_sp);
    _add(SP, SP, AddAmount);
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
      assert(Dest->getType() == IceType_i64);
      split64(Dest);
      Variable *DestLo = Dest->getLo();
      Variable *DestHi = Dest->getHi();
      _mov(DestLo, ReturnReg);
      _mov(DestHi, ReturnRegHi);
    } else {
      if (isFloatingType(Dest->getType()) || isVectorType(Dest->getType())) {
        _vmov(Dest, ReturnReg);
      } else {
        assert(isIntegerType(Dest->getType()) &&
               typeWidthInBytes(Dest->getType()) <= 4);
        _mov(Dest, ReturnReg);
      }
    }
  }
}

void TargetARM32::lowerCast(const InstCast *Inst) {
  InstCast::OpKind CastKind = Inst->getCastKind();
  Variable *Dest = Inst->getDest();
  Operand *Src0 = legalizeUndef(Inst->getSrc(0));
  switch (CastKind) {
  default:
    Func->setError("Cast type not supported");
    return;
  case InstCast::Sext: {
    if (isVectorType(Dest->getType())) {
      UnimplementedError(Func->getContext()->getFlags());
    } else if (Dest->getType() == IceType_i64) {
      // t1=sxtb src; t2= mov t1 asr #31; dst.lo=t1; dst.hi=t2
      Constant *ShiftAmt = Ctx->getConstantInt32(31);
      Variable *DestLo = llvm::cast<Variable>(loOperand(Dest));
      Variable *DestHi = llvm::cast<Variable>(hiOperand(Dest));
      Variable *T_Lo = makeReg(DestLo->getType());
      if (Src0->getType() == IceType_i32) {
        Operand *Src0RF = legalize(Src0, Legal_Reg | Legal_Flex);
        _mov(T_Lo, Src0RF);
      } else if (Src0->getType() == IceType_i1) {
        Variable *Src0R = legalizeToReg(Src0);
        _lsl(T_Lo, Src0R, ShiftAmt);
        _asr(T_Lo, T_Lo, ShiftAmt);
      } else {
        Variable *Src0R = legalizeToReg(Src0);
        _sxt(T_Lo, Src0R);
      }
      _mov(DestLo, T_Lo);
      Variable *T_Hi = makeReg(DestHi->getType());
      if (Src0->getType() != IceType_i1) {
        _mov(T_Hi, OperandARM32FlexReg::create(Func, IceType_i32, T_Lo,
                                               OperandARM32::ASR, ShiftAmt));
      } else {
        // For i1, the asr instruction is already done above.
        _mov(T_Hi, T_Lo);
      }
      _mov(DestHi, T_Hi);
    } else if (Src0->getType() == IceType_i1) {
      // GPR registers are 32-bit, so just use 31 as dst_bitwidth - 1.
      // lsl t1, src_reg, 31
      // asr t1, t1, 31
      // dst = t1
      Variable *Src0R = legalizeToReg(Src0);
      Constant *ShiftAmt = Ctx->getConstantInt32(31);
      Variable *T = makeReg(Dest->getType());
      _lsl(T, Src0R, ShiftAmt);
      _asr(T, T, ShiftAmt);
      _mov(Dest, T);
    } else {
      // t1 = sxt src; dst = t1
      Variable *Src0R = legalizeToReg(Src0);
      Variable *T = makeReg(Dest->getType());
      _sxt(T, Src0R);
      _mov(Dest, T);
    }
    break;
  }
  case InstCast::Zext: {
    if (isVectorType(Dest->getType())) {
      UnimplementedError(Func->getContext()->getFlags());
    } else if (Dest->getType() == IceType_i64) {
      // t1=uxtb src; dst.lo=t1; dst.hi=0
      Constant *Zero = Ctx->getConstantZero(IceType_i32);
      Variable *DestLo = llvm::cast<Variable>(loOperand(Dest));
      Variable *DestHi = llvm::cast<Variable>(hiOperand(Dest));
      Variable *T_Lo = makeReg(DestLo->getType());
      // i32 and i1 can just take up the whole register.
      // i32 doesn't need uxt, while i1 will have an and mask later anyway.
      if (Src0->getType() == IceType_i32 || Src0->getType() == IceType_i1) {
        Operand *Src0RF = legalize(Src0, Legal_Reg | Legal_Flex);
        _mov(T_Lo, Src0RF);
      } else {
        Variable *Src0R = legalizeToReg(Src0);
        _uxt(T_Lo, Src0R);
      }
      if (Src0->getType() == IceType_i1) {
        Constant *One = Ctx->getConstantInt32(1);
        _and(T_Lo, T_Lo, One);
      }
      _mov(DestLo, T_Lo);
      Variable *T_Hi = makeReg(DestLo->getType());
      _mov(T_Hi, Zero);
      _mov(DestHi, T_Hi);
    } else if (Src0->getType() == IceType_i1) {
      // t = Src0; t &= 1; Dest = t
      Operand *Src0RF = legalize(Src0, Legal_Reg | Legal_Flex);
      Constant *One = Ctx->getConstantInt32(1);
      Variable *T = makeReg(Dest->getType());
      // Just use _mov instead of _uxt since all registers are 32-bit.
      // _uxt requires the source to be a register so could have required
      // a _mov from legalize anyway.
      _mov(T, Src0RF);
      _and(T, T, One);
      _mov(Dest, T);
    } else {
      // t1 = uxt src; dst = t1
      Variable *Src0R = legalizeToReg(Src0);
      Variable *T = makeReg(Dest->getType());
      _uxt(T, Src0R);
      _mov(Dest, T);
    }
    break;
  }
  case InstCast::Trunc: {
    if (isVectorType(Dest->getType())) {
      UnimplementedError(Func->getContext()->getFlags());
    } else {
      if (Src0->getType() == IceType_i64)
        Src0 = loOperand(Src0);
      Operand *Src0RF = legalize(Src0, Legal_Reg | Legal_Flex);
      // t1 = trunc Src0RF; Dest = t1
      Variable *T = makeReg(Dest->getType());
      _mov(T, Src0RF);
      if (Dest->getType() == IceType_i1)
        _and(T, T, Ctx->getConstantInt1(1));
      _mov(Dest, T);
    }
    break;
  }
  case InstCast::Fptrunc:
    UnimplementedError(Func->getContext()->getFlags());
    break;
  case InstCast::Fpext: {
    UnimplementedError(Func->getContext()->getFlags());
    break;
  }
  case InstCast::Fptosi:
    UnimplementedError(Func->getContext()->getFlags());
    // Add a fake def to keep liveness consistent in the meantime.
    Context.insert(InstFakeDef::create(Func, Dest));
    break;
  case InstCast::Fptoui:
    UnimplementedError(Func->getContext()->getFlags());
    break;
  case InstCast::Sitofp:
    UnimplementedError(Func->getContext()->getFlags());
    break;
  case InstCast::Uitofp: {
    UnimplementedError(Func->getContext()->getFlags());
    break;
  }
  case InstCast::Bitcast: {
    Operand *Src0 = Inst->getSrc(0);
    if (Dest->getType() == Src0->getType()) {
      InstAssign *Assign = InstAssign::create(Func, Dest, Src0);
      lowerAssign(Assign);
      return;
    }
    UnimplementedError(Func->getContext()->getFlags());
    break;
  }
  }
}

void TargetARM32::lowerExtractElement(const InstExtractElement *Inst) {
  (void)Inst;
  UnimplementedError(Func->getContext()->getFlags());
}

void TargetARM32::lowerFcmp(const InstFcmp *Inst) {
  (void)Inst;
  UnimplementedError(Func->getContext()->getFlags());
}

void TargetARM32::lowerIcmp(const InstIcmp *Inst) {
  Variable *Dest = Inst->getDest();
  Operand *Src0 = legalizeUndef(Inst->getSrc(0));
  Operand *Src1 = legalizeUndef(Inst->getSrc(1));

  if (isVectorType(Dest->getType())) {
    UnimplementedError(Func->getContext()->getFlags());
    return;
  }

  // a=icmp cond, b, c ==>
  // GCC does:
  //   cmp      b.hi, c.hi     or  cmp      b.lo, c.lo
  //   cmp.eq   b.lo, c.lo         sbcs t1, b.hi, c.hi
  //   mov.<C1> t, #1              mov.<C1> t, #1
  //   mov.<C2> t, #0              mov.<C2> t, #0
  //   mov      a, t               mov      a, t
  // where the "cmp.eq b.lo, c.lo" is used for unsigned and "sbcs t1, hi, hi"
  // is used for signed compares. In some cases, b and c need to be swapped
  // as well.
  //
  // LLVM does:
  // for EQ and NE:
  //   eor  t1, b.hi, c.hi
  //   eor  t2, b.lo, c.hi
  //   orrs t, t1, t2
  //   mov.<C> t, #1
  //   mov  a, t
  //
  // that's nice in that it's just as short but has fewer dependencies
  // for better ILP at the cost of more registers.
  //
  // Otherwise for signed/unsigned <, <=, etc. LLVM uses a sequence with
  // two unconditional mov #0, two cmps, two conditional mov #1,
  // and one conditonal reg mov. That has few dependencies for good ILP,
  // but is a longer sequence.
  //
  // So, we are going with the GCC version since it's usually better (except
  // perhaps for eq/ne). We could revisit special-casing eq/ne later.
  Constant *Zero = Ctx->getConstantZero(IceType_i32);
  Constant *One = Ctx->getConstantInt32(1);
  if (Src0->getType() == IceType_i64) {
    InstIcmp::ICond Conditon = Inst->getCondition();
    size_t Index = static_cast<size_t>(Conditon);
    assert(Index < llvm::array_lengthof(TableIcmp64));
    Variable *Src0Lo, *Src0Hi;
    Operand *Src1LoRF, *Src1HiRF;
    if (TableIcmp64[Index].Swapped) {
      Src0Lo = legalizeToReg(loOperand(Src1));
      Src0Hi = legalizeToReg(hiOperand(Src1));
      Src1LoRF = legalize(loOperand(Src0), Legal_Reg | Legal_Flex);
      Src1HiRF = legalize(hiOperand(Src0), Legal_Reg | Legal_Flex);
    } else {
      Src0Lo = legalizeToReg(loOperand(Src0));
      Src0Hi = legalizeToReg(hiOperand(Src0));
      Src1LoRF = legalize(loOperand(Src1), Legal_Reg | Legal_Flex);
      Src1HiRF = legalize(hiOperand(Src1), Legal_Reg | Legal_Flex);
    }
    Variable *T = makeReg(IceType_i32);
    if (TableIcmp64[Index].IsSigned) {
      Variable *ScratchReg = makeReg(IceType_i32);
      _cmp(Src0Lo, Src1LoRF);
      _sbcs(ScratchReg, Src0Hi, Src1HiRF);
      // ScratchReg isn't going to be used, but we need the
      // side-effect of setting flags from this operation.
      Context.insert(InstFakeUse::create(Func, ScratchReg));
    } else {
      _cmp(Src0Hi, Src1HiRF);
      _cmp(Src0Lo, Src1LoRF, CondARM32::EQ);
    }
    _mov(T, One, TableIcmp64[Index].C1);
    _mov_nonkillable(T, Zero, TableIcmp64[Index].C2);
    _mov(Dest, T);
    return;
  }

  // a=icmp cond b, c ==>
  // GCC does:
  //   <u/s>xtb tb, b
  //   <u/s>xtb tc, c
  //   cmp      tb, tc
  //   mov.C1   t, #0
  //   mov.C2   t, #1
  //   mov      a, t
  // where the unsigned/sign extension is not needed for 32-bit.
  // They also have special cases for EQ and NE. E.g., for NE:
  //   <extend to tb, tc>
  //   subs     t, tb, tc
  //   movne    t, #1
  //   mov      a, t
  //
  // LLVM does:
  //   lsl     tb, b, #<N>
  //   mov     t, #0
  //   cmp     tb, c, lsl #<N>
  //   mov.<C> t, #1
  //   mov     a, t
  //
  // the left shift is by 0, 16, or 24, which allows the comparison to focus
  // on the digits that actually matter (for 16-bit or 8-bit signed/unsigned).
  // For the unsigned case, for some reason it does similar to GCC and does
  // a uxtb first. It's not clear to me why that special-casing is needed.
  //
  // We'll go with the LLVM way for now, since it's shorter and has just as
  // few dependencies.
  int32_t ShiftAmt = 32 - getScalarIntBitWidth(Src0->getType());
  assert(ShiftAmt >= 0);
  Constant *ShiftConst = nullptr;
  Variable *Src0R = nullptr;
  Variable *T = makeReg(IceType_i32);
  if (ShiftAmt) {
    ShiftConst = Ctx->getConstantInt32(ShiftAmt);
    Src0R = makeReg(IceType_i32);
    _lsl(Src0R, legalizeToReg(Src0), ShiftConst);
  } else {
    Src0R = legalizeToReg(Src0);
  }
  _mov(T, Zero);
  if (ShiftAmt) {
    Variable *Src1R = legalizeToReg(Src1);
    OperandARM32FlexReg *Src1RShifted = OperandARM32FlexReg::create(
        Func, IceType_i32, Src1R, OperandARM32::LSL, ShiftConst);
    _cmp(Src0R, Src1RShifted);
  } else {
    Operand *Src1RF = legalize(Src1, Legal_Reg | Legal_Flex);
    _cmp(Src0R, Src1RF);
  }
  _mov_nonkillable(T, One, getIcmp32Mapping(Inst->getCondition()));
  _mov(Dest, T);
  return;
}

void TargetARM32::lowerInsertElement(const InstInsertElement *Inst) {
  (void)Inst;
  UnimplementedError(Func->getContext()->getFlags());
}

void TargetARM32::lowerIntrinsicCall(const InstIntrinsicCall *Instr) {
  switch (Instr->getIntrinsicInfo().ID) {
  case Intrinsics::AtomicCmpxchg: {
    UnimplementedError(Func->getContext()->getFlags());
    return;
  }
  case Intrinsics::AtomicFence:
    UnimplementedError(Func->getContext()->getFlags());
    return;
  case Intrinsics::AtomicFenceAll:
    // NOTE: FenceAll should prevent and load/store from being moved
    // across the fence (both atomic and non-atomic). The InstARM32Mfence
    // instruction is currently marked coarsely as "HasSideEffects".
    UnimplementedError(Func->getContext()->getFlags());
    return;
  case Intrinsics::AtomicIsLockFree: {
    UnimplementedError(Func->getContext()->getFlags());
    return;
  }
  case Intrinsics::AtomicLoad: {
    UnimplementedError(Func->getContext()->getFlags());
    return;
  }
  case Intrinsics::AtomicRMW:
    UnimplementedError(Func->getContext()->getFlags());
    return;
  case Intrinsics::AtomicStore: {
    UnimplementedError(Func->getContext()->getFlags());
    return;
  }
  case Intrinsics::Bswap: {
    Variable *Dest = Instr->getDest();
    Operand *Val = Instr->getArg(0);
    Type Ty = Val->getType();
    if (Ty == IceType_i64) {
      Val = legalizeUndef(Val);
      Variable *Val_Lo = legalizeToReg(loOperand(Val));
      Variable *Val_Hi = legalizeToReg(hiOperand(Val));
      Variable *T_Lo = makeReg(IceType_i32);
      Variable *T_Hi = makeReg(IceType_i32);
      Variable *DestLo = llvm::cast<Variable>(loOperand(Dest));
      Variable *DestHi = llvm::cast<Variable>(hiOperand(Dest));
      _rev(T_Lo, Val_Lo);
      _rev(T_Hi, Val_Hi);
      _mov(DestLo, T_Hi);
      _mov(DestHi, T_Lo);
    } else {
      assert(Ty == IceType_i32 || Ty == IceType_i16);
      Variable *ValR = legalizeToReg(Val);
      Variable *T = makeReg(Ty);
      _rev(T, ValR);
      if (Val->getType() == IceType_i16) {
        Operand *Sixteen =
            legalize(Ctx->getConstantInt32(16), Legal_Reg | Legal_Flex);
        _lsr(T, T, Sixteen);
      }
      _mov(Dest, T);
    }
    return;
  }
  case Intrinsics::Ctpop: {
    Variable *Dest = Instr->getDest();
    Operand *Val = Instr->getArg(0);
    InstCall *Call = makeHelperCall(isInt32Asserting32Or64(Val->getType())
                                        ? H_call_ctpop_i32
                                        : H_call_ctpop_i64,
                                    Dest, 1);
    Call->addArg(Val);
    lowerCall(Call);
    // The popcount helpers always return 32-bit values, while the intrinsic's
    // signature matches some 64-bit platform's native instructions and
    // expect to fill a 64-bit reg. Thus, clear the upper bits of the dest
    // just in case the user doesn't do that in the IR or doesn't toss the bits
    // via truncate.
    if (Val->getType() == IceType_i64) {
      Variable *DestHi = llvm::cast<Variable>(hiOperand(Dest));
      Constant *Zero = Ctx->getConstantZero(IceType_i32);
      Variable *T = nullptr;
      _mov(T, Zero);
      _mov(DestHi, T);
    }
    return;
  }
  case Intrinsics::Ctlz: {
    // The "is zero undef" parameter is ignored and we always return
    // a well-defined value.
    Operand *Val = Instr->getArg(0);
    Variable *ValLoR;
    Variable *ValHiR = nullptr;
    if (Val->getType() == IceType_i64) {
      Val = legalizeUndef(Val);
      ValLoR = legalizeToReg(loOperand(Val));
      ValHiR = legalizeToReg(hiOperand(Val));
    } else {
      ValLoR = legalizeToReg(Val);
    }
    lowerCLZ(Instr->getDest(), ValLoR, ValHiR);
    return;
  }
  case Intrinsics::Cttz: {
    // Essentially like Clz, but reverse the bits first.
    Operand *Val = Instr->getArg(0);
    Variable *ValLoR;
    Variable *ValHiR = nullptr;
    if (Val->getType() == IceType_i64) {
      Val = legalizeUndef(Val);
      ValLoR = legalizeToReg(loOperand(Val));
      ValHiR = legalizeToReg(hiOperand(Val));
      Variable *TLo = makeReg(IceType_i32);
      Variable *THi = makeReg(IceType_i32);
      _rbit(TLo, ValLoR);
      _rbit(THi, ValHiR);
      ValLoR = THi;
      ValHiR = TLo;
    } else {
      ValLoR = legalizeToReg(Val);
      Variable *T = makeReg(IceType_i32);
      _rbit(T, ValLoR);
      ValLoR = T;
    }
    lowerCLZ(Instr->getDest(), ValLoR, ValHiR);
    return;
  }
  case Intrinsics::Fabs: {
    // Add a fake def to keep liveness consistent in the meantime.
    Context.insert(InstFakeDef::create(Func, Instr->getDest()));
    UnimplementedError(Func->getContext()->getFlags());
    return;
  }
  case Intrinsics::Longjmp: {
    InstCall *Call = makeHelperCall(H_call_longjmp, nullptr, 2);
    Call->addArg(Instr->getArg(0));
    Call->addArg(Instr->getArg(1));
    lowerCall(Call);
    return;
  }
  case Intrinsics::Memcpy: {
    // In the future, we could potentially emit an inline memcpy/memset, etc.
    // for intrinsic calls w/ a known length.
    InstCall *Call = makeHelperCall(H_call_memcpy, nullptr, 3);
    Call->addArg(Instr->getArg(0));
    Call->addArg(Instr->getArg(1));
    Call->addArg(Instr->getArg(2));
    lowerCall(Call);
    return;
  }
  case Intrinsics::Memmove: {
    InstCall *Call = makeHelperCall(H_call_memmove, nullptr, 3);
    Call->addArg(Instr->getArg(0));
    Call->addArg(Instr->getArg(1));
    Call->addArg(Instr->getArg(2));
    lowerCall(Call);
    return;
  }
  case Intrinsics::Memset: {
    // The value operand needs to be extended to a stack slot size because the
    // PNaCl ABI requires arguments to be at least 32 bits wide.
    Operand *ValOp = Instr->getArg(1);
    assert(ValOp->getType() == IceType_i8);
    Variable *ValExt = Func->makeVariable(stackSlotType());
    lowerCast(InstCast::create(Func, InstCast::Zext, ValExt, ValOp));
    // Technically, ARM has their own __aeabi_memset, but we can use plain
    // memset too. The value and size argument need to be flipped if we ever
    // decide to use __aeabi_memset.
    InstCall *Call = makeHelperCall(H_call_memset, nullptr, 3);
    Call->addArg(Instr->getArg(0));
    Call->addArg(ValExt);
    Call->addArg(Instr->getArg(2));
    lowerCall(Call);
    return;
  }
  case Intrinsics::NaClReadTP: {
    if (Ctx->getFlags().getUseSandboxing()) {
      UnimplementedError(Func->getContext()->getFlags());
    } else {
      InstCall *Call = makeHelperCall(H_call_read_tp, Instr->getDest(), 0);
      lowerCall(Call);
    }
    return;
  }
  case Intrinsics::Setjmp: {
    InstCall *Call = makeHelperCall(H_call_setjmp, Instr->getDest(), 1);
    Call->addArg(Instr->getArg(0));
    lowerCall(Call);
    return;
  }
  case Intrinsics::Sqrt: {
    Variable *Src = legalizeToReg(Instr->getArg(0));
    Variable *Dest = Instr->getDest();
    Variable *T = makeReg(Dest->getType());
    _vsqrt(T, Src);
    _vmov(Dest, T);
    return;
  }
  case Intrinsics::Stacksave: {
    Variable *SP = getPhysicalRegister(RegARM32::Reg_sp);
    Variable *Dest = Instr->getDest();
    _mov(Dest, SP);
    return;
  }
  case Intrinsics::Stackrestore: {
    Variable *SP = getPhysicalRegister(RegARM32::Reg_sp);
    Operand *Val = legalize(Instr->getArg(0), Legal_Reg | Legal_Flex);
    _mov_nonkillable(SP, Val);
    return;
  }
  case Intrinsics::Trap:
    _trap();
    return;
  case Intrinsics::UnknownIntrinsic:
    Func->setError("Should not be lowering UnknownIntrinsic");
    return;
  }
  return;
}

void TargetARM32::lowerCLZ(Variable *Dest, Variable *ValLoR, Variable *ValHiR) {
  Type Ty = Dest->getType();
  assert(Ty == IceType_i32 || Ty == IceType_i64);
  Variable *T = makeReg(IceType_i32);
  _clz(T, ValLoR);
  if (Ty == IceType_i64) {
    Variable *DestLo = llvm::cast<Variable>(loOperand(Dest));
    Variable *DestHi = llvm::cast<Variable>(hiOperand(Dest));
    Operand *Zero =
        legalize(Ctx->getConstantZero(IceType_i32), Legal_Reg | Legal_Flex);
    Operand *ThirtyTwo =
        legalize(Ctx->getConstantInt32(32), Legal_Reg | Legal_Flex);
    _cmp(ValHiR, Zero);
    Variable *T2 = makeReg(IceType_i32);
    _add(T2, T, ThirtyTwo);
    _clz(T2, ValHiR, CondARM32::NE);
    // T2 is actually a source as well when the predicate is not AL
    // (since it may leave T2 alone). We use set_dest_nonkillable to
    // prolong the liveness of T2 as if it was used as a source.
    _set_dest_nonkillable();
    _mov(DestLo, T2);
    Variable *T3 = nullptr;
    _mov(T3, Zero);
    _mov(DestHi, T3);
    return;
  }
  _mov(Dest, T);
  return;
}

void TargetARM32::lowerLoad(const InstLoad *Load) {
  // A Load instruction can be treated the same as an Assign
  // instruction, after the source operand is transformed into an
  // OperandARM32Mem operand.
  Type Ty = Load->getDest()->getType();
  Operand *Src0 = formMemoryOperand(Load->getSourceAddress(), Ty);
  Variable *DestLoad = Load->getDest();

  // TODO(jvoung): handled folding opportunities. Sign and zero extension
  // can be folded into a load.
  InstAssign *Assign = InstAssign::create(Func, DestLoad, Src0);
  lowerAssign(Assign);
}

void TargetARM32::doAddressOptLoad() {
  UnimplementedError(Func->getContext()->getFlags());
}

void TargetARM32::randomlyInsertNop(float Probability,
                                    RandomNumberGenerator &RNG) {
  RandomNumberGeneratorWrapper RNGW(RNG);
  if (RNGW.getTrueWithProbability(Probability)) {
    UnimplementedError(Func->getContext()->getFlags());
  }
}

void TargetARM32::lowerPhi(const InstPhi * /*Inst*/) {
  Func->setError("Phi found in regular instruction list");
}

void TargetARM32::lowerRet(const InstRet *Inst) {
  Variable *Reg = nullptr;
  if (Inst->hasRetValue()) {
    Operand *Src0 = Inst->getRetValue();
    Type Ty = Src0->getType();
    if (Ty == IceType_i64) {
      Src0 = legalizeUndef(Src0);
      Variable *R0 = legalizeToReg(loOperand(Src0), RegARM32::Reg_r0);
      Variable *R1 = legalizeToReg(hiOperand(Src0), RegARM32::Reg_r1);
      Reg = R0;
      Context.insert(InstFakeUse::create(Func, R1));
    } else if (Ty == IceType_f32) {
      Variable *S0 = legalizeToReg(Src0, RegARM32::Reg_s0);
      Reg = S0;
    } else if (Ty == IceType_f64) {
      Variable *D0 = legalizeToReg(Src0, RegARM32::Reg_d0);
      Reg = D0;
    } else if (isVectorType(Src0->getType())) {
      Variable *Q0 = legalizeToReg(Src0, RegARM32::Reg_q0);
      Reg = Q0;
    } else {
      Operand *Src0F = legalize(Src0, Legal_Reg | Legal_Flex);
      _mov(Reg, Src0F, CondARM32::AL, RegARM32::Reg_r0);
    }
  }
  // Add a ret instruction even if sandboxing is enabled, because
  // addEpilog explicitly looks for a ret instruction as a marker for
  // where to insert the frame removal instructions.
  // addEpilog is responsible for restoring the "lr" register as needed
  // prior to this ret instruction.
  _ret(getPhysicalRegister(RegARM32::Reg_lr), Reg);
  // Add a fake use of sp to make sure sp stays alive for the entire
  // function.  Otherwise post-call sp adjustments get dead-code
  // eliminated.  TODO: Are there more places where the fake use
  // should be inserted?  E.g. "void f(int n){while(1) g(n);}" may not
  // have a ret instruction.
  Variable *SP = getPhysicalRegister(RegARM32::Reg_sp);
  Context.insert(InstFakeUse::create(Func, SP));
}

void TargetARM32::lowerSelect(const InstSelect *Inst) {
  Variable *Dest = Inst->getDest();
  Type DestTy = Dest->getType();
  Operand *SrcT = Inst->getTrueOperand();
  Operand *SrcF = Inst->getFalseOperand();
  Operand *Condition = Inst->getCondition();

  if (isVectorType(DestTy)) {
    UnimplementedError(Func->getContext()->getFlags());
    return;
  }
  if (isFloatingType(DestTy)) {
    UnimplementedError(Func->getContext()->getFlags());
    return;
  }
  // TODO(jvoung): handle folding opportunities.
  // cmp cond, #0; mov t, SrcF; mov_cond t, SrcT; mov dest, t
  Variable *CmpOpnd0 = legalizeToReg(Condition);
  Operand *CmpOpnd1 = Ctx->getConstantZero(IceType_i32);
  _cmp(CmpOpnd0, CmpOpnd1);
  CondARM32::Cond Cond = CondARM32::NE;
  if (DestTy == IceType_i64) {
    SrcT = legalizeUndef(SrcT);
    SrcF = legalizeUndef(SrcF);
    // Set the low portion.
    Variable *DestLo = llvm::cast<Variable>(loOperand(Dest));
    Variable *TLo = nullptr;
    Operand *SrcFLo = legalize(loOperand(SrcF), Legal_Reg | Legal_Flex);
    _mov(TLo, SrcFLo);
    Operand *SrcTLo = legalize(loOperand(SrcT), Legal_Reg | Legal_Flex);
    _mov_nonkillable(TLo, SrcTLo, Cond);
    _mov(DestLo, TLo);
    // Set the high portion.
    Variable *DestHi = llvm::cast<Variable>(hiOperand(Dest));
    Variable *THi = nullptr;
    Operand *SrcFHi = legalize(hiOperand(SrcF), Legal_Reg | Legal_Flex);
    _mov(THi, SrcFHi);
    Operand *SrcTHi = legalize(hiOperand(SrcT), Legal_Reg | Legal_Flex);
    _mov_nonkillable(THi, SrcTHi, Cond);
    _mov(DestHi, THi);
    return;
  }
  Variable *T = nullptr;
  SrcF = legalize(SrcF, Legal_Reg | Legal_Flex);
  _mov(T, SrcF);
  SrcT = legalize(SrcT, Legal_Reg | Legal_Flex);
  _mov_nonkillable(T, SrcT, Cond);
  _mov(Dest, T);
}

void TargetARM32::lowerStore(const InstStore *Inst) {
  Operand *Value = Inst->getData();
  Operand *Addr = Inst->getAddr();
  OperandARM32Mem *NewAddr = formMemoryOperand(Addr, Value->getType());
  Type Ty = NewAddr->getType();

  if (Ty == IceType_i64) {
    Value = legalizeUndef(Value);
    Variable *ValueHi = legalizeToReg(hiOperand(Value));
    Variable *ValueLo = legalizeToReg(loOperand(Value));
    _str(ValueHi, llvm::cast<OperandARM32Mem>(hiOperand(NewAddr)));
    _str(ValueLo, llvm::cast<OperandARM32Mem>(loOperand(NewAddr)));
  } else if (isVectorType(Ty)) {
    UnimplementedError(Func->getContext()->getFlags());
  } else {
    Variable *ValueR = legalizeToReg(Value);
    _str(ValueR, NewAddr);
  }
}

void TargetARM32::doAddressOptStore() {
  UnimplementedError(Func->getContext()->getFlags());
}

void TargetARM32::lowerSwitch(const InstSwitch *Inst) {
  // This implements the most naive possible lowering.
  // cmp a,val[0]; jeq label[0]; cmp a,val[1]; jeq label[1]; ... jmp default
  Operand *Src0 = Inst->getComparison();
  SizeT NumCases = Inst->getNumCases();
  if (Src0->getType() == IceType_i64) {
    Src0 = legalizeUndef(Src0);
    Variable *Src0Lo = legalizeToReg(loOperand(Src0));
    Variable *Src0Hi = legalizeToReg(hiOperand(Src0));
    for (SizeT I = 0; I < NumCases; ++I) {
      Operand *ValueLo = Ctx->getConstantInt32(Inst->getValue(I));
      Operand *ValueHi = Ctx->getConstantInt32(Inst->getValue(I) >> 32);
      ValueLo = legalize(ValueLo, Legal_Reg | Legal_Flex);
      ValueHi = legalize(ValueHi, Legal_Reg | Legal_Flex);
      _cmp(Src0Lo, ValueLo);
      _cmp(Src0Hi, ValueHi, CondARM32::EQ);
      _br(Inst->getLabel(I), CondARM32::EQ);
    }
    _br(Inst->getLabelDefault());
    return;
  }

  // 32 bit integer
  Variable *Src0Var = legalizeToReg(Src0);
  for (SizeT I = 0; I < NumCases; ++I) {
    Operand *Value = Ctx->getConstantInt32(Inst->getValue(I));
    Value = legalize(Value, Legal_Reg | Legal_Flex);
    _cmp(Src0Var, Value);
    _br(Inst->getLabel(I), CondARM32::EQ);
  }
  _br(Inst->getLabelDefault());
}

void TargetARM32::lowerUnreachable(const InstUnreachable * /*Inst*/) {
  _trap();
}

void TargetARM32::prelowerPhis() {
  PhiLowering::prelowerPhis32Bit<TargetARM32>(this, Context.getNode(), Func);
}

Variable *TargetARM32::makeVectorOfZeros(Type Ty, int32_t RegNum) {
  Variable *Reg = makeReg(Ty, RegNum);
  UnimplementedError(Func->getContext()->getFlags());
  return Reg;
}

// Helper for legalize() to emit the right code to lower an operand to a
// register of the appropriate type.
Variable *TargetARM32::copyToReg(Operand *Src, int32_t RegNum) {
  Type Ty = Src->getType();
  Variable *Reg = makeReg(Ty, RegNum);
  if (isVectorType(Ty) || isFloatingType(Ty)) {
    _vmov(Reg, Src);
  } else {
    // Mov's Src operand can really only be the flexible second operand type
    // or a register. Users should guarantee that.
    _mov(Reg, Src);
  }
  return Reg;
}

Operand *TargetARM32::legalize(Operand *From, LegalMask Allowed,
                               int32_t RegNum) {
  Type Ty = From->getType();
  // Assert that a physical register is allowed.  To date, all calls
  // to legalize() allow a physical register. Legal_Flex converts
  // registers to the right type OperandARM32FlexReg as needed.
  assert(Allowed & Legal_Reg);
  // Go through the various types of operands:
  // OperandARM32Mem, OperandARM32Flex, Constant, and Variable.
  // Given the above assertion, if type of operand is not legal
  // (e.g., OperandARM32Mem and !Legal_Mem), we can always copy
  // to a register.
  if (auto Mem = llvm::dyn_cast<OperandARM32Mem>(From)) {
    // Before doing anything with a Mem operand, we need to ensure
    // that the Base and Index components are in physical registers.
    Variable *Base = Mem->getBase();
    Variable *Index = Mem->getIndex();
    Variable *RegBase = nullptr;
    Variable *RegIndex = nullptr;
    if (Base) {
      RegBase = legalizeToReg(Base);
    }
    if (Index) {
      RegIndex = legalizeToReg(Index);
    }
    // Create a new operand if there was a change.
    if (Base != RegBase || Index != RegIndex) {
      // There is only a reg +/- reg or reg + imm form.
      // Figure out which to re-create.
      if (Mem->isRegReg()) {
        Mem = OperandARM32Mem::create(Func, Ty, RegBase, RegIndex,
                                      Mem->getShiftOp(), Mem->getShiftAmt(),
                                      Mem->getAddrMode());
      } else {
        Mem = OperandARM32Mem::create(Func, Ty, RegBase, Mem->getOffset(),
                                      Mem->getAddrMode());
      }
    }
    if (!(Allowed & Legal_Mem)) {
      Variable *Reg = makeReg(Ty, RegNum);
      if (isVectorType(Ty)) {
        UnimplementedError(Func->getContext()->getFlags());
      } else if (isFloatingType(Ty)) {
        _vldr(Reg, Mem);
      } else {
        _ldr(Reg, Mem);
      }
      From = Reg;
    } else {
      From = Mem;
    }
    return From;
  }

  if (auto Flex = llvm::dyn_cast<OperandARM32Flex>(From)) {
    if (!(Allowed & Legal_Flex)) {
      if (auto FlexReg = llvm::dyn_cast<OperandARM32FlexReg>(Flex)) {
        if (FlexReg->getShiftOp() == OperandARM32::kNoShift) {
          From = FlexReg->getReg();
          // Fall through and let From be checked as a Variable below,
          // where it may or may not need a register.
        } else {
          return copyToReg(Flex, RegNum);
        }
      } else {
        return copyToReg(Flex, RegNum);
      }
    } else {
      return From;
    }
  }

  if (llvm::isa<Constant>(From)) {
    if (llvm::isa<ConstantUndef>(From)) {
      From = legalizeUndef(From, RegNum);
      if (isVectorType(Ty))
        return From;
    }
    // There should be no constants of vector type (other than undef).
    assert(!isVectorType(Ty));
    bool CanBeFlex = Allowed & Legal_Flex;
    if (auto *C32 = llvm::dyn_cast<ConstantInteger32>(From)) {
      uint32_t RotateAmt;
      uint32_t Immed_8;
      uint32_t Value = static_cast<uint32_t>(C32->getValue());
      // Check if the immediate will fit in a Flexible second operand,
      // if a Flexible second operand is allowed. We need to know the exact
      // value, so that rules out relocatable constants.
      // Also try the inverse and use MVN if possible.
      if (CanBeFlex &&
          OperandARM32FlexImm::canHoldImm(Value, &RotateAmt, &Immed_8)) {
        return OperandARM32FlexImm::create(Func, Ty, Immed_8, RotateAmt);
      } else if (CanBeFlex && OperandARM32FlexImm::canHoldImm(
                                  ~Value, &RotateAmt, &Immed_8)) {
        auto InvertedFlex =
            OperandARM32FlexImm::create(Func, Ty, Immed_8, RotateAmt);
        Variable *Reg = makeReg(Ty, RegNum);
        _mvn(Reg, InvertedFlex);
        return Reg;
      } else {
        // Do a movw/movt to a register.
        Variable *Reg = makeReg(Ty, RegNum);
        uint32_t UpperBits = (Value >> 16) & 0xFFFF;
        _movw(Reg,
              UpperBits != 0 ? Ctx->getConstantInt32(Value & 0xFFFF) : C32);
        if (UpperBits != 0) {
          _movt(Reg, Ctx->getConstantInt32(UpperBits));
        }
        return Reg;
      }
    } else if (auto *C = llvm::dyn_cast<ConstantRelocatable>(From)) {
      Variable *Reg = makeReg(Ty, RegNum);
      _movw(Reg, C);
      _movt(Reg, C);
      return Reg;
    } else {
      assert(isScalarFloatingType(Ty));
      // Load floats/doubles from literal pool.
      // TODO(jvoung): Allow certain immediates to be encoded directly in
      // an operand. See Table A7-18 of the ARM manual:
      // "Floating-point modified immediate constants".
      // Or, for 32-bit floating point numbers, just encode the raw bits
      // into a movw/movt pair to GPR, and vmov to an SREG, instead of using
      // a movw/movt pair to get the const-pool address then loading to SREG.
      std::string Buffer;
      llvm::raw_string_ostream StrBuf(Buffer);
      llvm::cast<Constant>(From)->emitPoolLabel(StrBuf);
      llvm::cast<Constant>(From)->setShouldBePooled(true);
      Constant *Offset = Ctx->getConstantSym(0, StrBuf.str(), true);
      Variable *BaseReg = makeReg(getPointerType());
      _movw(BaseReg, Offset);
      _movt(BaseReg, Offset);
      From = formMemoryOperand(BaseReg, Ty);
      return copyToReg(From, RegNum);
    }
  }

  if (auto Var = llvm::dyn_cast<Variable>(From)) {
    // Check if the variable is guaranteed a physical register.  This
    // can happen either when the variable is pre-colored or when it is
    // assigned infinite weight.
    bool MustHaveRegister = (Var->hasReg() || Var->mustHaveReg());
    // We need a new physical register for the operand if:
    //   Mem is not allowed and Var isn't guaranteed a physical
    //   register, or
    //   RegNum is required and Var->getRegNum() doesn't match.
    if ((!(Allowed & Legal_Mem) && !MustHaveRegister) ||
        (RegNum != Variable::NoRegister && RegNum != Var->getRegNum())) {
      From = copyToReg(From, RegNum);
    }
    return From;
  }
  llvm_unreachable("Unhandled operand kind in legalize()");

  return From;
}

/// Provide a trivial wrapper to legalize() for this common usage.
Variable *TargetARM32::legalizeToReg(Operand *From, int32_t RegNum) {
  return llvm::cast<Variable>(legalize(From, Legal_Reg, RegNum));
}

/// Legalize undef values to concrete values.
Operand *TargetARM32::legalizeUndef(Operand *From, int32_t RegNum) {
  Type Ty = From->getType();
  if (llvm::isa<ConstantUndef>(From)) {
    // Lower undefs to zero.  Another option is to lower undefs to an
    // uninitialized register; however, using an uninitialized register
    // results in less predictable code.
    //
    // If in the future the implementation is changed to lower undef
    // values to uninitialized registers, a FakeDef will be needed:
    //     Context.insert(InstFakeDef::create(Func, Reg));
    // This is in order to ensure that the live range of Reg is not
    // overestimated.  If the constant being lowered is a 64 bit value,
    // then the result should be split and the lo and hi components will
    // need to go in uninitialized registers.
    if (isVectorType(Ty))
      return makeVectorOfZeros(Ty, RegNum);
    return Ctx->getConstantZero(Ty);
  }
  return From;
}

OperandARM32Mem *TargetARM32::formMemoryOperand(Operand *Operand, Type Ty) {
  OperandARM32Mem *Mem = llvm::dyn_cast<OperandARM32Mem>(Operand);
  // It may be the case that address mode optimization already creates
  // an OperandARM32Mem, so in that case it wouldn't need another level
  // of transformation.
  if (Mem) {
    return llvm::cast<OperandARM32Mem>(legalize(Mem));
  }
  // If we didn't do address mode optimization, then we only
  // have a base/offset to work with. ARM always requires a base
  // register, so just use that to hold the operand.
  Variable *Base = legalizeToReg(Operand);
  return OperandARM32Mem::create(
      Func, Ty, Base,
      llvm::cast<ConstantInteger32>(Ctx->getConstantZero(IceType_i32)));
}

Variable *TargetARM32::makeReg(Type Type, int32_t RegNum) {
  // There aren't any 64-bit integer registers for ARM32.
  assert(Type != IceType_i64);
  Variable *Reg = Func->makeVariable(Type);
  if (RegNum == Variable::NoRegister)
    Reg->setMustHaveReg();
  else
    Reg->setRegNum(RegNum);
  return Reg;
}

void TargetARM32::alignRegisterPow2(Variable *Reg, uint32_t Align) {
  assert(llvm::isPowerOf2_32(Align));
  uint32_t RotateAmt;
  uint32_t Immed_8;
  Operand *Mask;
  // Use AND or BIC to mask off the bits, depending on which immediate fits
  // (if it fits at all). Assume Align is usually small, in which case BIC
  // works better. Thus, this rounds down to the alignment.
  if (OperandARM32FlexImm::canHoldImm(Align - 1, &RotateAmt, &Immed_8)) {
    Mask = legalize(Ctx->getConstantInt32(Align - 1), Legal_Reg | Legal_Flex);
    _bic(Reg, Reg, Mask);
  } else {
    Mask = legalize(Ctx->getConstantInt32(-Align), Legal_Reg | Legal_Flex);
    _and(Reg, Reg, Mask);
  }
}

void TargetARM32::postLower() {
  if (Ctx->getFlags().getOptLevel() == Opt_m1)
    return;
  inferTwoAddress();
}

void TargetARM32::makeRandomRegisterPermutation(
    llvm::SmallVectorImpl<int32_t> &Permutation,
    const llvm::SmallBitVector &ExcludeRegisters, uint64_t Salt) const {
  (void)Permutation;
  (void)ExcludeRegisters;
  (void)Salt;
  UnimplementedError(Func->getContext()->getFlags());
}

void TargetARM32::emit(const ConstantInteger32 *C) const {
  if (!BuildDefs::dump())
    return;
  Ostream &Str = Ctx->getStrEmit();
  Str << getConstantPrefix() << C->getValue();
}

void TargetARM32::emit(const ConstantInteger64 *) const {
  llvm::report_fatal_error("Not expecting to emit 64-bit integers");
}

void TargetARM32::emit(const ConstantFloat *C) const {
  (void)C;
  UnimplementedError(Ctx->getFlags());
}

void TargetARM32::emit(const ConstantDouble *C) const {
  (void)C;
  UnimplementedError(Ctx->getFlags());
}

void TargetARM32::emit(const ConstantUndef *) const {
  llvm::report_fatal_error("undef value encountered by emitter.");
}

TargetDataARM32::TargetDataARM32(GlobalContext *Ctx)
    : TargetDataLowering(Ctx) {}

void TargetDataARM32::lowerGlobals(const VariableDeclarationList &Vars,
                                   const IceString &SectionSuffix) {
  switch (Ctx->getFlags().getOutFileType()) {
  case FT_Elf: {
    ELFObjectWriter *Writer = Ctx->getObjectWriter();
    Writer->writeDataSection(Vars, llvm::ELF::R_ARM_ABS32, SectionSuffix);
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

void TargetDataARM32::lowerConstants() {
  if (Ctx->getFlags().getDisableTranslation())
    return;
  UnimplementedError(Ctx->getFlags());
}

void TargetDataARM32::lowerJumpTables() {
  if (Ctx->getFlags().getDisableTranslation())
    return;
  UnimplementedError(Ctx->getFlags());
}

TargetHeaderARM32::TargetHeaderARM32(GlobalContext *Ctx)
    : TargetHeaderLowering(Ctx), CPUFeatures(Ctx->getFlags()) {}

void TargetHeaderARM32::lower() {
  OstreamLocker L(Ctx);
  Ostream &Str = Ctx->getStrEmit();
  Str << ".syntax unified\n";
  // Emit build attributes in format: .eabi_attribute TAG, VALUE.
  // See Sec. 2 of "Addenda to, and Errata in the ABI for the ARM architecture"
  // http://infocenter.arm.com/help/topic/com.arm.doc.ihi0045d/IHI0045D_ABI_addenda.pdf
  //
  // Tag_conformance should be be emitted first in a file-scope
  // sub-subsection of the first public subsection of the attributes.
  Str << ".eabi_attribute 67, \"2.09\"      @ Tag_conformance\n";
  // Chromebooks are at least A15, but do A9 for higher compat.
  // For some reason, the LLVM ARM asm parser has the .cpu directive override
  // the mattr specified on the commandline. So to test hwdiv, we need to set
  // the .cpu directive higher (can't just rely on --mattr=...).
  if (CPUFeatures.hasFeature(TargetARM32Features::HWDivArm)) {
    Str << ".cpu    cortex-a15\n";
  } else {
    Str << ".cpu    cortex-a9\n";
  }
  Str << ".eabi_attribute 6, 10   @ Tag_CPU_arch: ARMv7\n"
      << ".eabi_attribute 7, 65   @ Tag_CPU_arch_profile: App profile\n";
  Str << ".eabi_attribute 8, 1    @ Tag_ARM_ISA_use: Yes\n"
      << ".eabi_attribute 9, 2    @ Tag_THUMB_ISA_use: Thumb-2\n";
  Str << ".fpu    neon\n"
      << ".eabi_attribute 17, 1   @ Tag_ABI_PCS_GOT_use: permit directly\n"
      << ".eabi_attribute 20, 1   @ Tag_ABI_FP_denormal\n"
      << ".eabi_attribute 21, 1   @ Tag_ABI_FP_exceptions\n"
      << ".eabi_attribute 23, 3   @ Tag_ABI_FP_number_model: IEEE 754\n"
      << ".eabi_attribute 34, 1   @ Tag_CPU_unaligned_access\n"
      << ".eabi_attribute 24, 1   @ Tag_ABI_align_needed: 8-byte\n"
      << ".eabi_attribute 25, 1   @ Tag_ABI_align_preserved: 8-byte\n"
      << ".eabi_attribute 28, 1   @ Tag_ABI_VFP_args\n"
      << ".eabi_attribute 36, 1   @ Tag_FP_HP_extension\n"
      << ".eabi_attribute 38, 1   @ Tag_ABI_FP_16bit_format\n"
      << ".eabi_attribute 42, 1   @ Tag_MPextension_use\n"
      << ".eabi_attribute 68, 1   @ Tag_Virtualization_use\n";
  if (CPUFeatures.hasFeature(TargetARM32Features::HWDivArm)) {
    Str << ".eabi_attribute 44, 2   @ Tag_DIV_use\n";
  }
  // Technically R9 is used for TLS with Sandboxing, and we reserve it.
  // However, for compatibility with current NaCl LLVM, don't claim that.
  Str << ".eabi_attribute 14, 3   @ Tag_ABI_PCS_R9_use: Not used\n";
}

} // end of namespace Ice
