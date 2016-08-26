//
//                        The Subzero Code Generator
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief Implements the TargetLoweringMIPS32 class, which consists almost
/// entirely of the lowering sequence for each high-level instruction.
///
//===----------------------------------------------------------------------===//

#include "IceTargetLoweringMIPS32.h"

#include "IceCfg.h"
#include "IceCfgNode.h"
#include "IceClFlags.h"
#include "IceDefs.h"
#include "IceELFObjectWriter.h"
#include "IceGlobalInits.h"
#include "IceInstMIPS32.h"
#include "IceInstVarIter.h"
#include "IceLiveness.h"
#include "IceOperand.h"
#include "IcePhiLoweringImpl.h"
#include "IceRegistersMIPS32.h"
#include "IceTargetLoweringMIPS32.def"
#include "IceUtils.h"
#include "llvm/Support/MathExtras.h"

namespace MIPS32 {
std::unique_ptr<::Ice::TargetLowering> createTargetLowering(::Ice::Cfg *Func) {
  return ::Ice::MIPS32::TargetMIPS32::create(Func);
}

std::unique_ptr<::Ice::TargetDataLowering>
createTargetDataLowering(::Ice::GlobalContext *Ctx) {
  return ::Ice::MIPS32::TargetDataMIPS32::create(Ctx);
}

std::unique_ptr<::Ice::TargetHeaderLowering>
createTargetHeaderLowering(::Ice::GlobalContext *Ctx) {
  return ::Ice::MIPS32::TargetHeaderMIPS32::create(Ctx);
}

void staticInit(::Ice::GlobalContext *Ctx) {
  ::Ice::MIPS32::TargetMIPS32::staticInit(Ctx);
}

bool shouldBePooled(const ::Ice::Constant *C) {
  return ::Ice::MIPS32::TargetMIPS32::shouldBePooled(C);
}
} // end of namespace MIPS32

namespace Ice {
namespace MIPS32 {

using llvm::isInt;

namespace {

// The maximum number of arguments to pass in GPR registers.
constexpr uint32_t MIPS32_MAX_GPR_ARG = 4;

std::array<RegNumT, MIPS32_MAX_GPR_ARG> GPRArgInitializer;
std::array<RegNumT, MIPS32_MAX_GPR_ARG / 2> I64ArgInitializer;

constexpr uint32_t MIPS32_MAX_FP_ARG = 2;

std::array<RegNumT, MIPS32_MAX_FP_ARG> FP32ArgInitializer;
std::array<RegNumT, MIPS32_MAX_FP_ARG> FP64ArgInitializer;

const char *getRegClassName(RegClass C) {
  auto ClassNum = static_cast<RegClassMIPS32>(C);
  assert(ClassNum < RCMIPS32_NUM);
  switch (ClassNum) {
  default:
    assert(C < RC_Target);
    return regClassString(C);
    // Add handling of new register classes below.
  }
}

// Stack alignment
constexpr uint32_t MIPS32_STACK_ALIGNMENT_BYTES = 8;

// Value is in bytes. Return Value adjusted to the next highest multiple of the
// stack alignment required for the given type.
uint32_t applyStackAlignmentTy(uint32_t Value, Type Ty) {
  size_t typeAlignInBytes = typeWidthInBytes(Ty);
  if (isVectorType(Ty))
    UnimplementedError(getFlags());
  return Utils::applyAlignment(Value, typeAlignInBytes);
}

// Value is in bytes. Return Value adjusted to the next highest multiple of the
// stack alignment.
uint32_t applyStackAlignment(uint32_t Value) {
  return Utils::applyAlignment(Value, MIPS32_STACK_ALIGNMENT_BYTES);
}

} // end of anonymous namespace

TargetMIPS32::TargetMIPS32(Cfg *Func) : TargetLowering(Func) {}

void TargetMIPS32::assignVarStackSlots(VarList &SortedSpilledVariables,
                                       size_t SpillAreaPaddingBytes,
                                       size_t SpillAreaSizeBytes,
                                       size_t GlobalsAndSubsequentPaddingSize) {
  const VariablesMetadata *VMetadata = Func->getVMetadata();
  size_t GlobalsSpaceUsed = SpillAreaPaddingBytes;
  size_t NextStackOffset = SpillAreaPaddingBytes;
  CfgVector<size_t> LocalsSize(Func->getNumNodes());
  const bool SimpleCoalescing = !callsReturnsTwice();

  for (Variable *Var : SortedSpilledVariables) {
    size_t Increment = typeWidthInBytesOnStack(Var->getType());
    if (SimpleCoalescing && VMetadata->isTracked(Var)) {
      if (VMetadata->isMultiBlock(Var)) {
        GlobalsSpaceUsed += Increment;
        NextStackOffset = GlobalsSpaceUsed;
      } else {
        SizeT NodeIndex = VMetadata->getLocalUseNode(Var)->getIndex();
        LocalsSize[NodeIndex] += Increment;
        NextStackOffset = SpillAreaPaddingBytes +
                          GlobalsAndSubsequentPaddingSize +
                          LocalsSize[NodeIndex];
      }
    } else {
      NextStackOffset += Increment;
    }
    Var->setStackOffset(SpillAreaSizeBytes - NextStackOffset);
  }
}

void TargetMIPS32::staticInit(GlobalContext *Ctx) {
  (void)Ctx;
  RegNumT::setLimit(RegMIPS32::Reg_NUM);
  SmallBitVector IntegerRegisters(RegMIPS32::Reg_NUM);
  SmallBitVector I64PairRegisters(RegMIPS32::Reg_NUM);
  SmallBitVector Float32Registers(RegMIPS32::Reg_NUM);
  SmallBitVector Float64Registers(RegMIPS32::Reg_NUM);
  SmallBitVector VectorRegisters(RegMIPS32::Reg_NUM);
  SmallBitVector InvalidRegisters(RegMIPS32::Reg_NUM);
#define X(val, encode, name, scratch, preserved, stackptr, frameptr, isInt,    \
          isI64Pair, isFP32, isFP64, isVec128, alias_init)                     \
  IntegerRegisters[RegMIPS32::val] = isInt;                                    \
  I64PairRegisters[RegMIPS32::val] = isI64Pair;                                \
  Float32Registers[RegMIPS32::val] = isFP32;                                   \
  Float64Registers[RegMIPS32::val] = isFP64;                                   \
  VectorRegisters[RegMIPS32::val] = isVec128;                                  \
  RegisterAliases[RegMIPS32::val].resize(RegMIPS32::Reg_NUM);                  \
  for (SizeT RegAlias : alias_init) {                                          \
    assert(!RegisterAliases[RegMIPS32::val][RegAlias] &&                       \
           "Duplicate alias for " #val);                                       \
    RegisterAliases[RegMIPS32::val].set(RegAlias);                             \
  }                                                                            \
  RegisterAliases[RegMIPS32::val].resize(RegMIPS32::Reg_NUM);                  \
  assert(RegisterAliases[RegMIPS32::val][RegMIPS32::val]);
  REGMIPS32_TABLE;
#undef X

  // TODO(mohit.bhakkad): Change these inits once we provide argument related
  // field in register tables
  for (size_t i = 0; i < MIPS32_MAX_GPR_ARG; i++)
    GPRArgInitializer[i] = RegNumT::fixme(RegMIPS32::Reg_A0 + i);

  for (size_t i = 0; i < MIPS32_MAX_GPR_ARG / 2; i++)
    I64ArgInitializer[i] = RegNumT::fixme(RegMIPS32::Reg_A0A1 + i);

  for (size_t i = 0; i < MIPS32_MAX_FP_ARG; i++) {
    FP32ArgInitializer[i] = RegNumT::fixme(RegMIPS32::Reg_F12 + i * 2);
    FP64ArgInitializer[i] = RegNumT::fixme(RegMIPS32::Reg_F12F13 + i);
  }

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

  for (size_t i = 0; i < llvm::array_lengthof(TypeToRegisterSet); ++i)
    TypeToRegisterSetUnfiltered[i] = TypeToRegisterSet[i];

  filterTypeToRegisterSet(Ctx, RegMIPS32::Reg_NUM, TypeToRegisterSet,
                          llvm::array_lengthof(TypeToRegisterSet),
                          RegMIPS32::getRegName, getRegClassName);
}

void TargetMIPS32::unsetIfNonLeafFunc() {
  for (CfgNode *Node : Func->getNodes()) {
    for (Inst &Instr : Node->getInsts()) {
      if (llvm::isa<InstCall>(&Instr)) {
        // Unset MaybeLeafFunc if call instruction exists.
        MaybeLeafFunc = false;
        return;
      }
    }
  }
}

uint32_t TargetMIPS32::getStackAlignment() const {
  return MIPS32_STACK_ALIGNMENT_BYTES;
}

void TargetMIPS32::findMaxStackOutArgsSize() {
  // MinNeededOutArgsBytes should be updated if the Target ever creates a
  // high-level InstCall that requires more stack bytes.
  size_t MinNeededOutArgsBytes = 0;
  if (!MaybeLeafFunc)
    MinNeededOutArgsBytes = MIPS32_MAX_GPR_ARG * 4;
  MaxOutArgsSizeBytes = MinNeededOutArgsBytes;
  for (CfgNode *Node : Func->getNodes()) {
    Context.init(Node);
    while (!Context.atEnd()) {
      PostIncrLoweringContext PostIncrement(Context);
      Inst *CurInstr = iteratorToInst(Context.getCur());
      if (auto *Call = llvm::dyn_cast<InstCall>(CurInstr)) {
        SizeT OutArgsSizeBytes = getCallStackArgumentsSizeBytes(Call);
        MaxOutArgsSizeBytes = std::max(MaxOutArgsSizeBytes, OutArgsSizeBytes);
      }
    }
  }
}

void TargetMIPS32::translateO2() {
  TimerMarker T(TimerStack::TT_O2, Func);

  // TODO(stichnot): share passes with X86?
  // https://code.google.com/p/nativeclient/issues/detail?id=4094
  genTargetHelperCalls();

  unsetIfNonLeafFunc();

  findMaxStackOutArgsSize();

  // Merge Alloca instructions, and lay out the stack.
  static constexpr bool SortAndCombineAllocas = true;
  Func->processAllocas(SortAndCombineAllocas);
  Func->dump("After Alloca processing");

  if (!getFlags().getEnablePhiEdgeSplit()) {
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

  // Target lowering. This requires liveness analysis for some parts of the
  // lowering decisions, such as compare/branch fusing. If non-lightweight
  // liveness analysis is used, the instructions need to be renumbered first.
  // TODO: This renumbering should only be necessary if we're actually
  // calculating live intervals, which we only do for register allocation.
  Func->renumberInstructions();
  if (Func->hasError())
    return;

  // TODO: It should be sufficient to use the fastest liveness calculation,
  // i.e. livenessLightweight(). However, for some reason that slows down the
  // rest of the translation. Investigate.
  Func->liveness(Liveness_Basic);
  if (Func->hasError())
    return;
  Func->dump("After MIPS32 address mode opt");

  Func->genCode();
  if (Func->hasError())
    return;
  Func->dump("After MIPS32 codegen");

  // Register allocation. This requires instruction renumbering and full
  // liveness analysis.
  Func->renumberInstructions();
  if (Func->hasError())
    return;
  Func->liveness(Liveness_Intervals);
  if (Func->hasError())
    return;
  // The post-codegen dump is done here, after liveness analysis and associated
  // cleanup, to make the dump cleaner and more useful.
  Func->dump("After initial MIPS32 codegen");
  // Validate the live range computations. The expensive validation call is
  // deliberately only made when assertions are enabled.
  assert(Func->validateLiveness());
  Func->getVMetadata()->init(VMK_All);
  regAlloc(RAK_Global);
  if (Func->hasError())
    return;
  Func->dump("After linear scan regalloc");

  if (getFlags().getEnablePhiEdgeSplit()) {
    Func->advancedPhiLowering();
    Func->dump("After advanced Phi lowering");
  }

  // Stack frame mapping.
  Func->genFrame();
  if (Func->hasError())
    return;
  Func->dump("After stack frame mapping");

  postLowerLegalization();
  if (Func->hasError())
    return;
  Func->dump("After postLowerLegalization");

  Func->contractEmptyNodes();
  Func->reorderNodes();

  // Branch optimization. This needs to be done just before code emission. In
  // particular, no transformations that insert or reorder CfgNodes should be
  // done after branch optimization. We go ahead and do it before nop insertion
  // to reduce the amount of work needed for searching for opportunities.
  Func->doBranchOpt();
  Func->dump("After branch optimization");

  // Nop insertion
  if (getFlags().getShouldDoNopInsertion()) {
    Func->doNopInsertion();
  }
}

void TargetMIPS32::translateOm1() {
  TimerMarker T(TimerStack::TT_Om1, Func);

  // TODO: share passes with X86?
  genTargetHelperCalls();

  unsetIfNonLeafFunc();

  findMaxStackOutArgsSize();

  // Do not merge Alloca instructions, and lay out the stack.
  static constexpr bool SortAndCombineAllocas = false;
  Func->processAllocas(SortAndCombineAllocas);
  Func->dump("After Alloca processing");

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
  Func->dump("After initial MIPS32 codegen");

  regAlloc(RAK_InfOnly);
  if (Func->hasError())
    return;
  Func->dump("After regalloc of infinite-weight variables");

  Func->genFrame();
  if (Func->hasError())
    return;
  Func->dump("After stack frame mapping");

  // Nop insertion
  if (getFlags().getShouldDoNopInsertion()) {
    Func->doNopInsertion();
  }
}

bool TargetMIPS32::doBranchOpt(Inst *Instr, const CfgNode *NextNode) {
  if (auto *Br = llvm::dyn_cast<InstMIPS32Br>(Instr)) {
    return Br->optimizeBranch(NextNode);
  }
  return false;
}

namespace {

const char *RegNames[RegMIPS32::Reg_NUM] = {
#define X(val, encode, name, scratch, preserved, stackptr, frameptr, isInt,    \
          isI64Pair, isFP32, isFP64, isVec128, alias_init)                     \
  name,
    REGMIPS32_TABLE
#undef X
};

} // end of anonymous namespace

const char *RegMIPS32::getRegName(RegNumT RegNum) {
  RegNum.assertIsValid();
  return RegNames[RegNum];
}

const char *TargetMIPS32::getRegName(RegNumT RegNum, Type Ty) const {
  (void)Ty;
  return RegMIPS32::getRegName(RegNum);
}

Variable *TargetMIPS32::getPhysicalRegister(RegNumT RegNum, Type Ty) {
  if (Ty == IceType_void)
    Ty = IceType_i32;
  if (PhysicalRegisters[Ty].empty())
    PhysicalRegisters[Ty].resize(RegMIPS32::Reg_NUM);
  RegNum.assertIsValid();
  Variable *Reg = PhysicalRegisters[Ty][RegNum];
  if (Reg == nullptr) {
    Reg = Func->makeVariable(Ty);
    Reg->setRegNum(RegNum);
    PhysicalRegisters[Ty][RegNum] = Reg;
    // Specially mark a named physical register as an "argument" so that it is
    // considered live upon function entry.  Otherwise it's possible to get
    // liveness validation errors for saving callee-save registers.
    Func->addImplicitArg(Reg);
    // Don't bother tracking the live range of a named physical register.
    Reg->setIgnoreLiveness();
  }
  return Reg;
}

void TargetMIPS32::emitJumpTable(const Cfg *Func,
                                 const InstJumpTable *JumpTable) const {
  (void)Func;
  (void)JumpTable;
  UnimplementedError(getFlags());
}

/// Provide a trivial wrapper to legalize() for this common usage.
Variable *TargetMIPS32::legalizeToReg(Operand *From, RegNumT RegNum) {
  return llvm::cast<Variable>(legalize(From, Legal_Reg, RegNum));
}

/// Legalize undef values to concrete values.
Operand *TargetMIPS32::legalizeUndef(Operand *From, RegNumT RegNum) {
  (void)RegNum;
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
      UnimplementedError(getFlags());
    return Ctx->getConstantZero(Ty);
  }
  return From;
}

Variable *TargetMIPS32::makeReg(Type Type, RegNumT RegNum) {
  // There aren't any 64-bit integer registers for Mips32.
  assert(Type != IceType_i64);
  Variable *Reg = Func->makeVariable(Type);
  if (RegNum.hasValue())
    Reg->setRegNum(RegNum);
  else
    Reg->setMustHaveReg();
  return Reg;
}

OperandMIPS32Mem *TargetMIPS32::formMemoryOperand(Operand *Operand, Type Ty) {
  // It may be the case that address mode optimization already creates an
  // OperandMIPS32Mem, so in that case it wouldn't need another level of
  // transformation.
  if (auto *Mem = llvm::dyn_cast<OperandMIPS32Mem>(Operand)) {
    return Mem;
  }

  // If we didn't do address mode optimization, then we only have a base/offset
  // to work with. MIPS always requires a base register, so just use that to
  // hold the operand.
  auto *Base = llvm::cast<Variable>(legalize(Operand, Legal_Reg));
  const int32_t Offset = Base->hasStackOffset() ? Base->getStackOffset() : 0;
  return OperandMIPS32Mem::create(
      Func, Ty, Base,
      llvm::cast<ConstantInteger32>(Ctx->getConstantInt32(Offset)));
}

void TargetMIPS32::emitVariable(const Variable *Var) const {
  if (!BuildDefs::dump())
    return;
  Ostream &Str = Ctx->getStrEmit();
  const Type FrameSPTy = IceType_i32;
  if (Var->hasReg()) {
    Str << '$' << getRegName(Var->getRegNum(), Var->getType());
  } else {
    int32_t Offset = Var->getStackOffset();
    Str << Offset;
    Str << "($" << getRegName(getFrameOrStackReg(), FrameSPTy);
    Str << ")";
  }
}

TargetMIPS32::CallingConv::CallingConv()
    : GPRegsUsed(RegMIPS32::Reg_NUM),
      GPRArgs(GPRArgInitializer.rbegin(), GPRArgInitializer.rend()),
      I64Args(I64ArgInitializer.rbegin(), I64ArgInitializer.rend()),
      VFPRegsUsed(RegMIPS32::Reg_NUM),
      FP32Args(FP32ArgInitializer.rbegin(), FP32ArgInitializer.rend()),
      FP64Args(FP64ArgInitializer.rbegin(), FP64ArgInitializer.rend()) {}

// In MIPS O32 abi FP argument registers can be used only if first argument is
// of type float/double. UseFPRegs flag is used to care of that. Also FP arg
// registers can be used only for first 2 arguments, so we require argument
// number to make register allocation decisions.
bool TargetMIPS32::CallingConv::argInReg(Type Ty, uint32_t ArgNo,
                                         RegNumT *Reg) {
  if (isScalarIntegerType(Ty))
    return argInGPR(Ty, Reg);
  if (isScalarFloatingType(Ty)) {
    if (ArgNo == 0) {
      UseFPRegs = true;
      return argInVFP(Ty, Reg);
    }
    if (UseFPRegs && ArgNo == 1) {
      UseFPRegs = false;
      return argInVFP(Ty, Reg);
    }
    return argInGPR(Ty, Reg);
  }
  UnimplementedError(getFlags());
  return false;
}

bool TargetMIPS32::CallingConv::argInGPR(Type Ty, RegNumT *Reg) {
  CfgVector<RegNumT> *Source;

  switch (Ty) {
  default: {
    UnimplementedError(getFlags());
    return false;
  } break;
  case IceType_i32:
  case IceType_f32: {
    Source = &GPRArgs;
  } break;
  case IceType_i64:
  case IceType_f64: {
    Source = &I64Args;
  } break;
  }

  discardUnavailableGPRsAndTheirAliases(Source);

  if (Source->empty()) {
    GPRegsUsed.set();
    return false;
  }

  *Reg = Source->back();
  // Note that we don't Source->pop_back() here. This is intentional. Notice how
  // we mark all of Reg's aliases as Used. So, for the next argument,
  // Source->back() is marked as unavailable, and it is thus implicitly popped
  // from the stack.
  GPRegsUsed |= RegisterAliases[*Reg];
  return true;
}

inline void TargetMIPS32::CallingConv::discardNextGPRAndItsAliases(
    CfgVector<RegNumT> *Regs) {
  GPRegsUsed |= RegisterAliases[Regs->back()];
  Regs->pop_back();
}

inline void TargetMIPS32::CallingConv::alignGPR(CfgVector<RegNumT> *Regs) {
  if (Regs->back() == RegMIPS32::Reg_A1 || Regs->back() == RegMIPS32::Reg_A3)
    discardNextGPRAndItsAliases(Regs);
}

// GPR are not packed when passing parameters. Thus, a function foo(i32, i64,
// i32) will have the first argument in a0, the second in a2-a3, and the third
// on the stack. To model this behavior, whenever we pop a register from Regs,
// we remove all of its aliases from the pool of available GPRs. This has the
// effect of computing the "closure" on the GPR registers.
void TargetMIPS32::CallingConv::discardUnavailableGPRsAndTheirAliases(
    CfgVector<RegNumT> *Regs) {
  while (!Regs->empty() && GPRegsUsed[Regs->back()]) {
    discardNextGPRAndItsAliases(Regs);
  }
}

bool TargetMIPS32::CallingConv::argInVFP(Type Ty, RegNumT *Reg) {
  CfgVector<RegNumT> *Source;

  switch (Ty) {
  default: {
    UnimplementedError(getFlags());
    return false;
  } break;
  case IceType_f32: {
    Source = &FP32Args;
  } break;
  case IceType_f64: {
    Source = &FP64Args;
  } break;
  }

  discardUnavailableVFPRegsAndTheirAliases(Source);

  if (Source->empty()) {
    VFPRegsUsed.set();
    return false;
  }

  *Reg = Source->back();
  VFPRegsUsed |= RegisterAliases[*Reg];

  // In MIPS O32 abi if fun arguments are (f32, i32) then one can not use reg_a0
  // for second argument even though it's free. f32 arg goes in reg_f12, i32 arg
  // goes in reg_a1. Similarly if arguments are (f64, i32) second argument goes
  // in reg_a3 and a0, a1 are not used.
  Source = &GPRArgs;
  // Discard one GPR reg for f32(4 bytes), two for f64(4 + 4 bytes)
  if (Ty == IceType_f64) {
    // In MIPS o32 abi, when we use GPR argument pairs to store F64 values, pair
    // must be aligned at even register. Similarly when we discard GPR registers
    // when some arguments from starting 16 bytes goes in FPR, we must take care
    // of alignment. For example if fun args are (f32, f64, f32), for first f32
    // we discard a0, now for f64 argument, which will go in F14F15, we must
    // first align GPR vector to even register by discarding a1, then discard
    // two GPRs a2 and a3. Now last f32 argument will go on stack.
    alignGPR(Source);
    discardNextGPRAndItsAliases(Source);
  }
  discardNextGPRAndItsAliases(Source);
  return true;
}

void TargetMIPS32::CallingConv::discardUnavailableVFPRegsAndTheirAliases(
    CfgVector<RegNumT> *Regs) {
  while (!Regs->empty() && VFPRegsUsed[Regs->back()]) {
    Regs->pop_back();
  }
}

void TargetMIPS32::lowerArguments() {
  VarList &Args = Func->getArgs();
  TargetMIPS32::CallingConv CC;

  // For each register argument, replace Arg in the argument list with the home
  // register. Then generate an instruction in the prolog to copy the home
  // register to the assigned location of Arg.
  Context.init(Func->getEntryNode());
  Context.setInsertPoint(Context.getCur());

  for (SizeT I = 0, E = Args.size(); I < E; ++I) {
    Variable *Arg = Args[I];
    Type Ty = Arg->getType();
    RegNumT RegNum;
    if (!CC.argInReg(Ty, I, &RegNum)) {
      continue;
    }
    Variable *RegisterArg = Func->makeVariable(Ty);
    if (BuildDefs::dump()) {
      RegisterArg->setName(Func, "home_reg:" + Arg->getName());
    }
    RegisterArg->setIsArg();
    Arg->setIsArg(false);
    Args[I] = RegisterArg;
    switch (Ty) {
    default: { RegisterArg->setRegNum(RegNum); } break;
    case IceType_i64: {
      auto *RegisterArg64 = llvm::cast<Variable64On32>(RegisterArg);
      RegisterArg64->initHiLo(Func);
      RegisterArg64->getLo()->setRegNum(
          RegNumT::fixme(RegMIPS32::get64PairFirstRegNum(RegNum)));
      RegisterArg64->getHi()->setRegNum(
          RegNumT::fixme(RegMIPS32::get64PairSecondRegNum(RegNum)));
    } break;
    }
    Context.insert<InstAssign>(Arg, RegisterArg);
  }
}

Type TargetMIPS32::stackSlotType() { return IceType_i32; }

// Helper function for addProlog().
//
// This assumes Arg is an argument passed on the stack. This sets the frame
// offset for Arg and updates InArgsSizeBytes according to Arg's width. For an
// I64 arg that has been split into Lo and Hi components, it calls itself
// recursively on the components, taking care to handle Lo first because of the
// little-endian architecture. Lastly, this function generates an instruction
// to copy Arg into its assigned register if applicable.
void TargetMIPS32::finishArgumentLowering(Variable *Arg, Variable *FramePtr,
                                          size_t BasicFrameOffset,
                                          size_t *InArgsSizeBytes) {
  const Type Ty = Arg->getType();
  *InArgsSizeBytes = applyStackAlignmentTy(*InArgsSizeBytes, Ty);

  if (auto *Arg64On32 = llvm::dyn_cast<Variable64On32>(Arg)) {
    Variable *const Lo = Arg64On32->getLo();
    Variable *const Hi = Arg64On32->getHi();
    finishArgumentLowering(Lo, FramePtr, BasicFrameOffset, InArgsSizeBytes);
    finishArgumentLowering(Hi, FramePtr, BasicFrameOffset, InArgsSizeBytes);
    return;
  }
  assert(Ty != IceType_i64);

  const int32_t ArgStackOffset = BasicFrameOffset + *InArgsSizeBytes;
  *InArgsSizeBytes += typeWidthInBytesOnStack(Ty);

  if (!Arg->hasReg()) {
    Arg->setStackOffset(ArgStackOffset);
    return;
  }

  // If the argument variable has been assigned a register, we need to copy the
  // value from the stack slot.
  Variable *Parameter = Func->makeVariable(Ty);
  Parameter->setMustNotHaveReg();
  Parameter->setStackOffset(ArgStackOffset);
  _mov(Arg, Parameter);
}

void TargetMIPS32::addProlog(CfgNode *Node) {
  // Stack frame layout:
  //
  // +------------------------+
  // | 1. preserved registers |
  // +------------------------+
  // | 2. padding             |
  // +------------------------+
  // | 3. global spill area   |
  // +------------------------+
  // | 4. padding             |
  // +------------------------+
  // | 5. local spill area    |
  // +------------------------+
  // | 6. padding             |
  // +------------------------+
  // | 7. allocas             |
  // +------------------------+
  // | 8. padding             |
  // +------------------------+
  // | 9. out args           |
  // +------------------------+ <--- StackPointer
  //
  // The following variables record the size in bytes of the given areas:
  //  * PreservedRegsSizeBytes: area 1
  //  * SpillAreaPaddingBytes:  area 2
  //  * GlobalsSize:            area 3
  //  * GlobalsAndSubsequentPaddingSize: areas 3 - 4
  //  * LocalsSpillAreaSize:    area 5
  //  * SpillAreaSizeBytes:     areas 2 - 9
  //  * maxOutArgsSizeBytes():  area 9

  Context.init(Node);
  Context.setInsertPoint(Context.getCur());

  SmallBitVector CalleeSaves = getRegisterSet(RegSet_CalleeSave, RegSet_None);
  RegsUsed = SmallBitVector(CalleeSaves.size());

  VarList SortedSpilledVariables;

  size_t GlobalsSize = 0;
  // If there is a separate locals area, this represents that area. Otherwise
  // it counts any variable not counted by GlobalsSize.
  SpillAreaSizeBytes = 0;
  // If there is a separate locals area, this specifies the alignment for it.
  uint32_t LocalsSlotsAlignmentBytes = 0;
  // The entire spill locations area gets aligned to largest natural alignment
  // of the variables that have a spill slot.
  uint32_t SpillAreaAlignmentBytes = 0;
  // For now, we don't have target-specific variables that need special
  // treatment (no stack-slot-linked SpillVariable type).
  std::function<bool(Variable *)> TargetVarHook = [](Variable *Var) {
    static constexpr bool AssignStackSlot = false;
    static constexpr bool DontAssignStackSlot = !AssignStackSlot;
    if (llvm::isa<Variable64On32>(Var)) {
      return DontAssignStackSlot;
    }
    return AssignStackSlot;
  };

  // Compute the list of spilled variables and bounds for GlobalsSize, etc.
  getVarStackSlotParams(SortedSpilledVariables, RegsUsed, &GlobalsSize,
                        &SpillAreaSizeBytes, &SpillAreaAlignmentBytes,
                        &LocalsSlotsAlignmentBytes, TargetVarHook);
  uint32_t LocalsSpillAreaSize = SpillAreaSizeBytes;
  SpillAreaSizeBytes += GlobalsSize;

  PreservedGPRs.reserve(CalleeSaves.size());

  // Consider FP and RA as callee-save / used as needed.
  if (UsesFramePointer) {
    if (RegsUsed[RegMIPS32::Reg_FP]) {
      llvm::report_fatal_error("Frame pointer has been used.");
    }
    CalleeSaves[RegMIPS32::Reg_FP] = true;
    RegsUsed[RegMIPS32::Reg_FP] = true;
  }
  if (!MaybeLeafFunc) {
    CalleeSaves[RegMIPS32::Reg_RA] = true;
    RegsUsed[RegMIPS32::Reg_RA] = true;
  }

  // Make two passes over the used registers. The first pass records all the
  // used registers -- and their aliases. Then, we figure out which GPR
  // registers should be saved.
  SmallBitVector ToPreserve(RegMIPS32::Reg_NUM);
  for (SizeT i = 0; i < CalleeSaves.size(); ++i) {
    if (CalleeSaves[i] && RegsUsed[i]) {
      ToPreserve |= RegisterAliases[i];
    }
  }

  uint32_t NumCallee = 0;

  // RegClasses is a tuple of
  //
  // <First Register in Class, Last Register in Class, Vector of Save Registers>
  //
  // We use this tuple to figure out which register we should save/restore
  // during
  // prolog/epilog.
  using RegClassType = std::tuple<uint32_t, uint32_t, VarList *>;
  const RegClassType RegClass = RegClassType(
      RegMIPS32::Reg_GPR_First, RegMIPS32::Reg_GPR_Last, &PreservedGPRs);
  const uint32_t FirstRegInClass = std::get<0>(RegClass);
  const uint32_t LastRegInClass = std::get<1>(RegClass);
  VarList *const PreservedRegsInClass = std::get<2>(RegClass);
  for (uint32_t Reg = LastRegInClass; Reg > FirstRegInClass; Reg--) {
    if (!ToPreserve[Reg]) {
      continue;
    }
    ++NumCallee;
    Variable *PhysicalRegister = getPhysicalRegister(RegNumT::fromInt(Reg));
    PreservedRegsSizeBytes +=
        typeWidthInBytesOnStack(PhysicalRegister->getType());
    PreservedRegsInClass->push_back(PhysicalRegister);
  }

  Ctx->statsUpdateRegistersSaved(NumCallee);

  // Align the variables area. SpillAreaPaddingBytes is the size of the region
  // after the preserved registers and before the spill areas.
  // LocalsSlotsPaddingBytes is the amount of padding between the globals and
  // locals area if they are separate.
  assert(SpillAreaAlignmentBytes <= MIPS32_STACK_ALIGNMENT_BYTES);
  (void)MIPS32_STACK_ALIGNMENT_BYTES;
  assert(LocalsSlotsAlignmentBytes <= SpillAreaAlignmentBytes);
  uint32_t SpillAreaPaddingBytes = 0;
  uint32_t LocalsSlotsPaddingBytes = 0;
  alignStackSpillAreas(PreservedRegsSizeBytes, SpillAreaAlignmentBytes,
                       GlobalsSize, LocalsSlotsAlignmentBytes,
                       &SpillAreaPaddingBytes, &LocalsSlotsPaddingBytes);
  SpillAreaSizeBytes += SpillAreaPaddingBytes + LocalsSlotsPaddingBytes;
  uint32_t GlobalsAndSubsequentPaddingSize =
      GlobalsSize + LocalsSlotsPaddingBytes;

  // Adds the out args space to the stack, and align SP if necessary.
  if (!NeedsStackAlignment) {
    SpillAreaSizeBytes += MaxOutArgsSizeBytes * (VariableAllocaUsed ? 0 : 1);
  } else {
    uint32_t StackOffset = PreservedRegsSizeBytes;
    uint32_t StackSize = applyStackAlignment(StackOffset + SpillAreaSizeBytes);
    if (!VariableAllocaUsed)
      StackSize = applyStackAlignment(StackSize + MaxOutArgsSizeBytes);
    SpillAreaSizeBytes = StackSize - StackOffset;
  }

  // Combine fixed alloca with SpillAreaSize.
  SpillAreaSizeBytes += FixedAllocaSizeBytes;

  TotalStackSizeBytes = PreservedRegsSizeBytes + SpillAreaSizeBytes;

  // Generate "addiu sp, sp, -TotalStackSizeBytes"
  if (TotalStackSizeBytes) {
    // Use the scratch register if needed to legalize the immediate.
    Variable *SP = getPhysicalRegister(RegMIPS32::Reg_SP);
    _addiu(SP, SP, -(TotalStackSizeBytes));
  }

  Ctx->statsUpdateFrameBytes(TotalStackSizeBytes);

  if (!PreservedGPRs.empty()) {
    uint32_t StackOffset = TotalStackSizeBytes;
    for (Variable *Var : *PreservedRegsInClass) {
      Variable *PhysicalRegister = getPhysicalRegister(Var->getRegNum());
      StackOffset -= typeWidthInBytesOnStack(PhysicalRegister->getType());
      Variable *SP = getPhysicalRegister(RegMIPS32::Reg_SP);
      OperandMIPS32Mem *MemoryLocation = OperandMIPS32Mem::create(
          Func, IceType_i32, SP,
          llvm::cast<ConstantInteger32>(Ctx->getConstantInt32(StackOffset)));
      _sw(PhysicalRegister, MemoryLocation);
    }
  }

  Variable *FP = getPhysicalRegister(RegMIPS32::Reg_FP);

  // Generate "mov FP, SP" if needed.
  if (UsesFramePointer) {
    Variable *SP = getPhysicalRegister(RegMIPS32::Reg_SP);
    _mov(FP, SP);
    // Keep FP live for late-stage liveness analysis (e.g. asm-verbose mode).
    Context.insert<InstFakeUse>(FP);
  }

  // Fill in stack offsets for stack args, and copy args into registers for
  // those that were register-allocated. Args are pushed right to left, so
  // Arg[0] is closest to the stack/frame pointer.
  const VarList &Args = Func->getArgs();
  size_t InArgsSizeBytes = MIPS32_MAX_GPR_ARG * 4;
  TargetMIPS32::CallingConv CC;
  uint32_t ArgNo = 0;

  for (Variable *Arg : Args) {
    RegNumT DummyReg;
    const Type Ty = Arg->getType();
    // Skip arguments passed in registers.
    if (CC.argInReg(Ty, ArgNo, &DummyReg)) {
      ArgNo++;
      continue;
    } else {
      finishArgumentLowering(Arg, FP, TotalStackSizeBytes, &InArgsSizeBytes);
    }
  }

  // Fill in stack offsets for locals.
  assignVarStackSlots(SortedSpilledVariables, SpillAreaPaddingBytes,
                      SpillAreaSizeBytes, GlobalsAndSubsequentPaddingSize);
  this->HasComputedFrame = true;

  if (BuildDefs::dump() && Func->isVerbose(IceV_Frame)) {
    OstreamLocker _(Func->getContext());
    Ostream &Str = Func->getContext()->getStrDump();

    Str << "Stack layout:\n";
    uint32_t SPAdjustmentPaddingSize =
        SpillAreaSizeBytes - LocalsSpillAreaSize -
        GlobalsAndSubsequentPaddingSize - SpillAreaPaddingBytes -
        MaxOutArgsSizeBytes;
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
        << " outgoing args size = " << MaxOutArgsSizeBytes << " bytes\n"
        << " locals spill area alignment = " << LocalsSlotsAlignmentBytes
        << " bytes\n"
        << " is FP based = " << 1 << "\n";
  }
  return;
}

void TargetMIPS32::addEpilog(CfgNode *Node) {
  InstList &Insts = Node->getInsts();
  InstList::reverse_iterator RI, E;
  for (RI = Insts.rbegin(), E = Insts.rend(); RI != E; ++RI) {
    if (llvm::isa<InstMIPS32Ret>(*RI))
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

  Variable *SP = getPhysicalRegister(RegMIPS32::Reg_SP);
  if (UsesFramePointer) {
    Variable *FP = getPhysicalRegister(RegMIPS32::Reg_FP);
    // For late-stage liveness analysis (e.g. asm-verbose mode), adding a fake
    // use of SP before the assignment of SP=FP keeps previous SP adjustments
    // from being dead-code eliminated.
    Context.insert<InstFakeUse>(SP);
    _mov(SP, FP);
  }

  VarList::reverse_iterator RIter, END;

  if (!PreservedGPRs.empty()) {
    uint32_t StackOffset = TotalStackSizeBytes - PreservedRegsSizeBytes;
    for (RIter = PreservedGPRs.rbegin(), END = PreservedGPRs.rend();
         RIter != END; ++RIter) {
      Variable *PhysicalRegister = getPhysicalRegister((*RIter)->getRegNum());
      Variable *SP = getPhysicalRegister(RegMIPS32::Reg_SP);
      OperandMIPS32Mem *MemoryLocation = OperandMIPS32Mem::create(
          Func, IceType_i32, SP,
          llvm::cast<ConstantInteger32>(Ctx->getConstantInt32(StackOffset)));
      _lw(PhysicalRegister, MemoryLocation);
      StackOffset += typeWidthInBytesOnStack(PhysicalRegister->getType());
    }
  }

  if (TotalStackSizeBytes) {
    _addiu(SP, SP, TotalStackSizeBytes);
  }

  return;
}

Variable *TargetMIPS32::PostLoweringLegalizer::newBaseRegister(
    Variable *Base, int32_t Offset, RegNumT ScratchRegNum) {
  // Legalize will likely need a lui/ori combination, but if the top bits are
  // all 0 from negating the offset and subtracting, we could use that instead.
  const bool ShouldSub = Offset != 0 && (-Offset & 0xFFFF0000) == 0;
  Variable *ScratchReg = Target->makeReg(IceType_i32, ScratchRegNum);
  if (ShouldSub) {
    Variable *OffsetVal =
        Target->legalizeToReg(Target->Ctx->getConstantInt32(-Offset));
    Target->_sub(ScratchReg, Base, OffsetVal);
  } else {
    Target->_addiu(ScratchReg, Base, Offset);
  }

  return ScratchReg;
}

void TargetMIPS32::PostLoweringLegalizer::legalizeMov(InstMIPS32Mov *MovInstr) {
  Variable *Dest = MovInstr->getDest();
  assert(Dest != nullptr);
  const Type DestTy = Dest->getType();
  (void)DestTy;
  assert(DestTy != IceType_i64);

  Operand *Src = MovInstr->getSrc(0);
  const Type SrcTy = Src->getType();
  (void)SrcTy;
  assert(SrcTy != IceType_i64);

  if (MovInstr->isMultiDest() || MovInstr->isMultiSource())
    return;

  bool Legalized = false;
  if (Dest->hasReg()) {
    if (auto *Var = llvm::dyn_cast<Variable>(Src)) {
      if (Var->isRematerializable()) {
        // This is equivalent to an x86 _lea(RematOffset(%esp/%ebp), Variable).

        // ExtraOffset is only needed for frame-pointer based frames as we have
        // to account for spill storage.
        const int32_t ExtraOffset = (Var->getRegNum() == Target->getFrameReg())
                                        ? Target->getFrameFixedAllocaOffset()
                                        : 0;

        const int32_t Offset = Var->getStackOffset() + ExtraOffset;
        Variable *Base = Target->getPhysicalRegister(Var->getRegNum());
        Variable *T = newBaseRegister(Base, Offset, Dest->getRegNum());
        Target->_mov(Dest, T);
        Legalized = true;
      } else if (!Var->hasReg()) {
        UnimplementedError(getFlags());
        return;
      }
    }
  } else {
    UnimplementedError(getFlags());
    return;
  }

  if (Legalized) {
    if (MovInstr->isDestRedefined()) {
      Target->_set_dest_redefined();
    }
    MovInstr->setDeleted();
  }
}

void TargetMIPS32::postLowerLegalization() {
  Func->dump("Before postLowerLegalization");
  assert(hasComputedFrame());
  for (CfgNode *Node : Func->getNodes()) {
    Context.init(Node);
    PostLoweringLegalizer Legalizer(this);
    while (!Context.atEnd()) {
      PostIncrLoweringContext PostIncrement(Context);
      Inst *CurInstr = iteratorToInst(Context.getCur());

      // TODO(sagar.thakur): Add remaining cases of legalization.

      if (auto *MovInstr = llvm::dyn_cast<InstMIPS32Mov>(CurInstr)) {
        Legalizer.legalizeMov(MovInstr);
      }
    }
  }
}

Operand *TargetMIPS32::loOperand(Operand *Operand) {
  assert(Operand->getType() == IceType_i64);
  if (auto *Var64On32 = llvm::dyn_cast<Variable64On32>(Operand))
    return Var64On32->getLo();
  if (auto *Const = llvm::dyn_cast<ConstantInteger64>(Operand)) {
    return Ctx->getConstantInt32(static_cast<uint32_t>(Const->getValue()));
  }
  if (auto *Mem = llvm::dyn_cast<OperandMIPS32Mem>(Operand)) {
    // Conservatively disallow memory operands with side-effects (pre/post
    // increment) in case of duplication.
    assert(Mem->getAddrMode() == OperandMIPS32Mem::Offset);
    return OperandMIPS32Mem::create(Func, IceType_i32, Mem->getBase(),
                                    Mem->getOffset(), Mem->getAddrMode());
  }
  llvm_unreachable("Unsupported operand type");
  return nullptr;
}

Operand *TargetMIPS32::hiOperand(Operand *Operand) {
  assert(Operand->getType() == IceType_i64);
  if (Operand->getType() != IceType_i64)
    return Operand;
  if (auto *Var64On32 = llvm::dyn_cast<Variable64On32>(Operand))
    return Var64On32->getHi();
  if (auto *Const = llvm::dyn_cast<ConstantInteger64>(Operand)) {
    return Ctx->getConstantInt32(
        static_cast<uint32_t>(Const->getValue() >> 32));
  }
  if (auto *Mem = llvm::dyn_cast<OperandMIPS32Mem>(Operand)) {
    // Conservatively disallow memory operands with side-effects
    // in case of duplication.
    assert(Mem->getAddrMode() == OperandMIPS32Mem::Offset);
    const Type SplitType = IceType_i32;
    Variable *Base = Mem->getBase();
    auto *Offset = llvm::cast<ConstantInteger32>(Mem->getOffset());
    assert(!Utils::WouldOverflowAdd(Offset->getValue(), 4));
    int32_t NextOffsetVal = Offset->getValue() + 4;
    constexpr bool SignExt = false;
    if (!OperandMIPS32Mem::canHoldOffset(SplitType, SignExt, NextOffsetVal)) {
      // We have to make a temp variable and add 4 to either Base or Offset.
      // If we add 4 to Offset, this will convert a non-RegReg addressing
      // mode into a RegReg addressing mode. Since NaCl sandboxing disallows
      // RegReg addressing modes, prefer adding to base and replacing instead.
      // Thus we leave the old offset alone.
      Constant *Four = Ctx->getConstantInt32(4);
      Variable *NewBase = Func->makeVariable(Base->getType());
      lowerArithmetic(InstArithmetic::create(Func, InstArithmetic::Add, NewBase,
                                             Base, Four));
      Base = NewBase;
    } else {
      Offset =
          llvm::cast<ConstantInteger32>(Ctx->getConstantInt32(NextOffsetVal));
    }
    return OperandMIPS32Mem::create(Func, SplitType, Base, Offset,
                                    Mem->getAddrMode());
  }
  llvm_unreachable("Unsupported operand type");
  return nullptr;
}

SmallBitVector TargetMIPS32::getRegisterSet(RegSetMask Include,
                                            RegSetMask Exclude) const {
  SmallBitVector Registers(RegMIPS32::Reg_NUM);

#define X(val, encode, name, scratch, preserved, stackptr, frameptr, isInt,    \
          isI64Pair, isFP32, isFP64, isVec128, alias_init)                     \
  if (scratch && (Include & RegSet_CallerSave))                                \
    Registers[RegMIPS32::val] = true;                                          \
  if (preserved && (Include & RegSet_CalleeSave))                              \
    Registers[RegMIPS32::val] = true;                                          \
  if (stackptr && (Include & RegSet_StackPointer))                             \
    Registers[RegMIPS32::val] = true;                                          \
  if (frameptr && (Include & RegSet_FramePointer))                             \
    Registers[RegMIPS32::val] = true;                                          \
  if (scratch && (Exclude & RegSet_CallerSave))                                \
    Registers[RegMIPS32::val] = false;                                         \
  if (preserved && (Exclude & RegSet_CalleeSave))                              \
    Registers[RegMIPS32::val] = false;                                         \
  if (stackptr && (Exclude & RegSet_StackPointer))                             \
    Registers[RegMIPS32::val] = false;                                         \
  if (frameptr && (Exclude & RegSet_FramePointer))                             \
    Registers[RegMIPS32::val] = false;

  REGMIPS32_TABLE

#undef X

  return Registers;
}

void TargetMIPS32::lowerAlloca(const InstAlloca *Instr) {
  // Conservatively require the stack to be aligned. Some stack adjustment
  // operations implemented below assume that the stack is aligned before the
  // alloca. All the alloca code ensures that the stack alignment is preserved
  // after the alloca. The stack alignment restriction can be relaxed in some
  // cases.
  NeedsStackAlignment = true;

  // For default align=0, set it to the real value 1, to avoid any
  // bit-manipulation problems below.
  const uint32_t AlignmentParam = std::max(1u, Instr->getAlignInBytes());

  // LLVM enforces power of 2 alignment.
  assert(llvm::isPowerOf2_32(AlignmentParam));
  assert(llvm::isPowerOf2_32(MIPS32_STACK_ALIGNMENT_BYTES));

  const uint32_t Alignment =
      std::max(AlignmentParam, MIPS32_STACK_ALIGNMENT_BYTES);
  const bool OverAligned = Alignment > MIPS32_STACK_ALIGNMENT_BYTES;
  const bool OptM1 = Func->getOptLevel() == Opt_m1;
  const bool AllocaWithKnownOffset = Instr->getKnownFrameOffset();
  const bool UseFramePointer =
      hasFramePointer() || OverAligned || !AllocaWithKnownOffset || OptM1;

  if (UseFramePointer)
    setHasFramePointer();

  Variable *SP = getPhysicalRegister(RegMIPS32::Reg_SP);

  Variable *Dest = Instr->getDest();
  Operand *TotalSize = Instr->getSizeInBytes();

  if (const auto *ConstantTotalSize =
          llvm::dyn_cast<ConstantInteger32>(TotalSize)) {
    const uint32_t Value =
        Utils::applyAlignment(ConstantTotalSize->getValue(), Alignment);
    FixedAllocaSizeBytes += Value;
    // Constant size alloca.
    if (!UseFramePointer) {
      // If we don't need a Frame Pointer, this alloca has a known offset to the
      // stack pointer. We don't need adjust the stack pointer, nor assign any
      // value to Dest, as Dest is rematerializable.
      assert(Dest->isRematerializable());
      Context.insert<InstFakeDef>(Dest);
      return;
    }
  } else {
    // Non-constant sizes need to be adjusted to the next highest multiple of
    // the required alignment at runtime.
    VariableAllocaUsed = true;
    Variable *AlignAmount;
    auto *TotalSizeR = legalizeToReg(TotalSize, Legal_Reg);
    auto *T1 = I32Reg();
    auto *T2 = I32Reg();
    auto *T3 = I32Reg();
    auto *T4 = I32Reg();
    auto *T5 = I32Reg();
    _addiu(T1, TotalSizeR, MIPS32_STACK_ALIGNMENT_BYTES - 1);
    _addiu(T2, getZero(), -MIPS32_STACK_ALIGNMENT_BYTES);
    _and(T3, T1, T2);
    _subu(T4, SP, T3);
    if (Instr->getAlignInBytes()) {
      AlignAmount =
          legalizeToReg(Ctx->getConstantInt32(-AlignmentParam), Legal_Reg);
      _and(T5, T4, AlignAmount);
      _mov(Dest, T5);
    } else {
      _mov(Dest, T4);
    }
    _mov(SP, Dest);
    return;
  }

  // Add enough to the returned address to account for the out args area.
  if (MaxOutArgsSizeBytes > 0) {
    Variable *T = makeReg(getPointerType());
    _addiu(T, SP, MaxOutArgsSizeBytes);
    _mov(Dest, T);
  } else {
    _mov(Dest, SP);
  }
}

void TargetMIPS32::lowerInt64Arithmetic(const InstArithmetic *Instr,
                                        Variable *Dest, Operand *Src0,
                                        Operand *Src1) {
  InstArithmetic::OpKind Op = Instr->getOp();
  switch (Op) {
  case InstArithmetic::Add:
  case InstArithmetic::And:
  case InstArithmetic::Or:
  case InstArithmetic::Sub:
  case InstArithmetic::Xor:
  case InstArithmetic::Mul:
    break;
  default:
    UnimplementedLoweringError(this, Instr);
    return;
  }
  auto *DestLo = llvm::cast<Variable>(loOperand(Dest));
  auto *DestHi = llvm::cast<Variable>(hiOperand(Dest));
  Variable *Src0LoR = legalizeToReg(loOperand(Src0));
  Variable *Src1LoR = legalizeToReg(loOperand(Src1));
  Variable *Src0HiR = legalizeToReg(hiOperand(Src0));
  Variable *Src1HiR = legalizeToReg(hiOperand(Src1));

  switch (Op) {
  case InstArithmetic::_num:
    llvm::report_fatal_error("Unknown arithmetic operator");
    return;
  case InstArithmetic::Add: {
    auto *T_Carry = I32Reg(), *T_Lo = I32Reg(), *T_Hi = I32Reg(),
         *T_Hi2 = I32Reg();
    _addu(T_Lo, Src0LoR, Src1LoR);
    _mov(DestLo, T_Lo);
    _sltu(T_Carry, T_Lo, Src0LoR);
    _addu(T_Hi, T_Carry, Src0HiR);
    _addu(T_Hi2, Src1HiR, T_Hi);
    _mov(DestHi, T_Hi2);
    return;
  }
  case InstArithmetic::And: {
    auto *T_Lo = I32Reg(), *T_Hi = I32Reg();
    _and(T_Lo, Src0LoR, Src1LoR);
    _mov(DestLo, T_Lo);
    _and(T_Hi, Src0HiR, Src1HiR);
    _mov(DestHi, T_Hi);
    return;
  }
  case InstArithmetic::Sub: {
    auto *T_Borrow = I32Reg(), *T_Lo = I32Reg(), *T_Hi = I32Reg(),
         *T_Hi2 = I32Reg();
    _subu(T_Lo, Src0LoR, Src1LoR);
    _mov(DestLo, T_Lo);
    _sltu(T_Borrow, Src0LoR, Src1LoR);
    _addu(T_Hi, T_Borrow, Src1HiR);
    _subu(T_Hi2, Src0HiR, T_Hi);
    _mov(DestHi, T_Hi2);
    return;
  }
  case InstArithmetic::Or: {
    auto *T_Lo = I32Reg(), *T_Hi = I32Reg();
    _or(T_Lo, Src0LoR, Src1LoR);
    _mov(DestLo, T_Lo);
    _or(T_Hi, Src0HiR, Src1HiR);
    _mov(DestHi, T_Hi);
    return;
  }
  case InstArithmetic::Xor: {
    auto *T_Lo = I32Reg(), *T_Hi = I32Reg();
    _xor(T_Lo, Src0LoR, Src1LoR);
    _mov(DestLo, T_Lo);
    _xor(T_Hi, Src0HiR, Src1HiR);
    _mov(DestHi, T_Hi);
    return;
  }
  case InstArithmetic::Mul: {
    // TODO(rkotler): Make sure that mul has the side effect of clobbering
    // LO, HI. Check for any other LO, HI quirkiness in this section.
    auto *T_Lo = I32Reg(RegMIPS32::Reg_LO), *T_Hi = I32Reg(RegMIPS32::Reg_HI);
    auto *T1 = I32Reg(), *T2 = I32Reg();
    auto *TM1 = I32Reg(), *TM2 = I32Reg(), *TM3 = I32Reg(), *TM4 = I32Reg();
    _multu(T_Lo, Src0LoR, Src1LoR);
    Context.insert<InstFakeDef>(T_Hi, T_Lo);
    _mflo(T1, T_Lo);
    _mfhi(T2, T_Hi);
    _mov(DestLo, T1);
    _mul(TM1, Src0HiR, Src1LoR);
    _mul(TM2, Src0LoR, Src1HiR);
    _addu(TM3, TM1, T2);
    _addu(TM4, TM3, TM2);
    _mov(DestHi, TM4);
    return;
  }
  default:
    UnimplementedLoweringError(this, Instr);
    return;
  }
}

void TargetMIPS32::lowerArithmetic(const InstArithmetic *Instr) {
  Variable *Dest = Instr->getDest();
  // We need to signal all the UnimplementedLoweringError errors before any
  // legalization into new variables, otherwise Om1 register allocation may fail
  // when it sees variables that are defined but not used.
  Type DestTy = Dest->getType();
  Operand *Src0 = legalizeUndef(Instr->getSrc(0));
  Operand *Src1 = legalizeUndef(Instr->getSrc(1));
  if (DestTy == IceType_i64) {
    lowerInt64Arithmetic(Instr, Instr->getDest(), Src0, Src1);
    return;
  }
  if (isVectorType(Dest->getType())) {
    UnimplementedLoweringError(this, Instr);
    return;
  }
  switch (Instr->getOp()) {
  default:
    break;
  case InstArithmetic::Frem:
    UnimplementedLoweringError(this, Instr);
    return;
  }

  // At this point Dest->getType() is non-i64 scalar

  Variable *T = makeReg(Dest->getType());
  Variable *Src0R = legalizeToReg(Src0);
  Variable *Src1R = legalizeToReg(Src1);

  switch (Instr->getOp()) {
  case InstArithmetic::_num:
    break;
  case InstArithmetic::Add:
    _addu(T, Src0R, Src1R);
    _mov(Dest, T);
    return;
  case InstArithmetic::And:
    _and(T, Src0R, Src1R);
    _mov(Dest, T);
    return;
  case InstArithmetic::Or:
    _or(T, Src0R, Src1R);
    _mov(Dest, T);
    return;
  case InstArithmetic::Xor:
    _xor(T, Src0R, Src1R);
    _mov(Dest, T);
    return;
  case InstArithmetic::Sub:
    _subu(T, Src0R, Src1R);
    _mov(Dest, T);
    return;
  case InstArithmetic::Mul: {
    _mul(T, Src0R, Src1R);
    _mov(Dest, T);
    return;
  }
  case InstArithmetic::Shl: {
    _sllv(T, Src0R, Src1R);
    _mov(Dest, T);
    return;
  }
  case InstArithmetic::Lshr: {
    _srlv(T, Src0R, Src1R);
    _mov(Dest, T);
    return;
  }
  case InstArithmetic::Ashr: {
    _srav(T, Src0R, Src1R);
    _mov(Dest, T);
    return;
  }
  case InstArithmetic::Udiv: {
    auto *T_Zero = I32Reg(RegMIPS32::Reg_ZERO);
    _divu(T_Zero, Src0R, Src1R);
    _mflo(T, T_Zero);
    _mov(Dest, T);
    return;
  }
  case InstArithmetic::Sdiv: {
    auto *T_Zero = I32Reg(RegMIPS32::Reg_ZERO);
    _div(T_Zero, Src0R, Src1R);
    _mflo(T, T_Zero);
    _mov(Dest, T);
    return;
  }
  case InstArithmetic::Urem: {
    auto *T_Zero = I32Reg(RegMIPS32::Reg_ZERO);
    _divu(T_Zero, Src0R, Src1R);
    _mfhi(T, T_Zero);
    _mov(Dest, T);
    return;
  }
  case InstArithmetic::Srem: {
    auto *T_Zero = I32Reg(RegMIPS32::Reg_ZERO);
    _div(T_Zero, Src0R, Src1R);
    _mfhi(T, T_Zero);
    _mov(Dest, T);
    return;
  }
  case InstArithmetic::Fadd: {
    if (DestTy == IceType_f32) {
      _add_s(T, Src0R, Src1R);
      _mov(Dest, T);
      return;
    }
    if (DestTy == IceType_f64) {
      _add_d(T, Src0R, Src1R);
      _mov(Dest, T);
      return;
    }
    break;
  }
  case InstArithmetic::Fsub:
    if (DestTy == IceType_f32) {
      _sub_s(T, Src0R, Src1R);
      _mov(Dest, T);
      return;
    }
    if (DestTy == IceType_f64) {
      _sub_d(T, Src0R, Src1R);
      _mov(Dest, T);
      return;
    }
    break;
  case InstArithmetic::Fmul:
    if (DestTy == IceType_f32) {
      _mul_s(T, Src0R, Src1R);
      _mov(Dest, T);
      return;
    }
    if (DestTy == IceType_f64) {
      _mul_d(T, Src0R, Src1R);
      _mov(Dest, T);
      return;
    }
    break;
  case InstArithmetic::Fdiv:
    if (DestTy == IceType_f32) {
      _div_s(T, Src0R, Src1R);
      _mov(Dest, T);
      return;
    }
    if (DestTy == IceType_f64) {
      _div_d(T, Src0R, Src1R);
      _mov(Dest, T);
      return;
    }
    break;
  case InstArithmetic::Frem:
    break;
  }
  UnimplementedLoweringError(this, Instr);
}

void TargetMIPS32::lowerAssign(const InstAssign *Instr) {
  Variable *Dest = Instr->getDest();

  if (Dest->isRematerializable()) {
    Context.insert<InstFakeDef>(Dest);
    return;
  }

  Operand *Src0 = Instr->getSrc(0);
  assert(Dest->getType() == Src0->getType());
  if (Dest->getType() == IceType_i64) {
    Src0 = legalizeUndef(Src0);
    Operand *Src0Lo = legalize(loOperand(Src0), Legal_Reg);
    Operand *Src0Hi = legalize(hiOperand(Src0), Legal_Reg);
    auto *DestLo = llvm::cast<Variable>(loOperand(Dest));
    auto *DestHi = llvm::cast<Variable>(hiOperand(Dest));
    // Variable *T_Lo = nullptr, *T_Hi = nullptr;
    auto *T_Lo = I32Reg(), *T_Hi = I32Reg();
    _mov(T_Lo, Src0Lo);
    _mov(DestLo, T_Lo);
    _mov(T_Hi, Src0Hi);
    _mov(DestHi, T_Hi);
  } else {
    Operand *SrcR;
    if (Dest->hasReg()) {
      // If Dest already has a physical register, then legalize the Src operand
      // into a Variable with the same register assignment.  This especially
      // helps allow the use of Flex operands.
      SrcR = legalize(Src0, Legal_Reg, Dest->getRegNum());
    } else {
      // Dest could be a stack operand. Since we could potentially need
      // to do a Store (and store can only have Register operands),
      // legalize this to a register.
      SrcR = legalize(Src0, Legal_Reg);
    }
    if (isVectorType(Dest->getType())) {
      UnimplementedLoweringError(this, Instr);
    } else {
      _mov(Dest, SrcR);
    }
  }
}

void TargetMIPS32::lowerBr(const InstBr *Instr) {
  if (Instr->isUnconditional()) {
    _br(Instr->getTargetUnconditional());
    return;
  }
  CfgNode *TargetTrue = Instr->getTargetTrue();
  CfgNode *TargetFalse = Instr->getTargetFalse();
  Operand *Boolean = Instr->getCondition();
  const Inst *Producer = Computations.getProducerOf(Boolean);
  if (Producer == nullptr) {
    // Since we don't know the producer of this boolean we will assume its
    // producer will keep it in positive logic and just emit beqz with this
    // Boolean as an operand.
    auto *BooleanR = legalizeToReg(Boolean);
    _br(TargetTrue, TargetFalse, BooleanR, CondMIPS32::Cond::EQZ);
    return;
  }
  if (Producer->getKind() == Inst::Icmp) {
    const InstIcmp *CompareInst = llvm::cast<InstIcmp>(Producer);
    Operand *Src0 = CompareInst->getSrc(0);
    Operand *Src1 = CompareInst->getSrc(1);
    const Type Src0Ty = Src0->getType();
    assert(Src0Ty == Src1->getType());
    if (Src0Ty == IceType_i64) {
      UnimplementedLoweringError(this, Instr);
      return;
    }
    auto *Src0R = legalizeToReg(Src0);
    auto *Src1R = legalizeToReg(Src1);
    auto *DestT = makeReg(Src0Ty);
    switch (CompareInst->getCondition()) {
    default:
      break;
    case InstIcmp::Eq: {
      _br(TargetTrue, TargetFalse, Src0R, Src1R, CondMIPS32::Cond::NE);
      break;
    }
    case InstIcmp::Ne: {
      _br(TargetTrue, TargetFalse, Src0R, Src1R, CondMIPS32::Cond::EQ);
      break;
    }
    case InstIcmp::Ugt: {
      _sltu(DestT, Src1R, Src0R);
      _br(TargetTrue, TargetFalse, DestT, CondMIPS32::Cond::EQZ);
      break;
    }
    case InstIcmp::Uge: {
      _sltu(DestT, Src0R, Src1R);
      _br(TargetTrue, TargetFalse, DestT, CondMIPS32::Cond::NEZ);
      break;
    }
    case InstIcmp::Ult: {
      _sltu(DestT, Src0R, Src1R);
      _br(TargetTrue, TargetFalse, DestT, CondMIPS32::Cond::EQZ);
      break;
    }
    case InstIcmp::Ule: {
      _sltu(DestT, Src1R, Src0R);
      _br(TargetTrue, TargetFalse, DestT, CondMIPS32::Cond::NEZ);
      break;
    }
    case InstIcmp::Sgt: {
      _slt(DestT, Src1R, Src0R);
      _br(TargetTrue, TargetFalse, DestT, CondMIPS32::Cond::EQZ);
      break;
    }
    case InstIcmp::Sge: {
      _slt(DestT, Src0R, Src1R);
      _br(TargetTrue, TargetFalse, DestT, CondMIPS32::Cond::NEZ);
      break;
    }
    case InstIcmp::Slt: {
      _slt(DestT, Src0R, Src1R);
      _br(TargetTrue, TargetFalse, DestT, CondMIPS32::Cond::EQZ);
      break;
    }
    case InstIcmp::Sle: {
      _slt(DestT, Src1R, Src0R);
      _br(TargetTrue, TargetFalse, DestT, CondMIPS32::Cond::NEZ);
      break;
    }
    }
  }
}

void TargetMIPS32::lowerCall(const InstCall *Instr) {
  NeedsStackAlignment = true;

  //  Assign arguments to registers and stack. Also reserve stack.
  TargetMIPS32::CallingConv CC;

  // Pair of Arg Operand -> GPR number assignments.
  llvm::SmallVector<std::pair<Operand *, RegNumT>, MIPS32_MAX_GPR_ARG> GPRArgs;
  llvm::SmallVector<std::pair<Operand *, RegNumT>, MIPS32_MAX_FP_ARG> FPArgs;
  // Pair of Arg Operand -> stack offset.
  llvm::SmallVector<std::pair<Operand *, int32_t>, 8> StackArgs;
  size_t ParameterAreaSizeBytes = 16;

  // Classify each argument operand according to the location where the
  // argument is passed.

  for (SizeT i = 0, NumArgs = Instr->getNumArgs(); i < NumArgs; ++i) {
    Operand *Arg = legalizeUndef(Instr->getArg(i));
    const Type Ty = Arg->getType();
    bool InReg = false;
    RegNumT Reg;

    InReg = CC.argInReg(Ty, i, &Reg);

    if (!InReg) {
      ParameterAreaSizeBytes =
          applyStackAlignmentTy(ParameterAreaSizeBytes, Ty);
      StackArgs.push_back(std::make_pair(Arg, ParameterAreaSizeBytes));
      ParameterAreaSizeBytes += typeWidthInBytesOnStack(Ty);
      continue;
    }

    if (Ty == IceType_i64) {
      Operand *Lo = loOperand(Arg);
      Operand *Hi = hiOperand(Arg);
      GPRArgs.push_back(
          std::make_pair(Lo, RegMIPS32::get64PairFirstRegNum(Reg)));
      GPRArgs.push_back(
          std::make_pair(Hi, RegMIPS32::get64PairSecondRegNum(Reg)));
    } else if (isScalarIntegerType(Ty)) {
      GPRArgs.push_back(std::make_pair(Arg, Reg));
    } else {
      FPArgs.push_back(std::make_pair(Arg, Reg));
    }
  }

  // Adjust the parameter area so that the stack is aligned. It is assumed that
  // the stack is already aligned at the start of the calling sequence.
  ParameterAreaSizeBytes = applyStackAlignment(ParameterAreaSizeBytes);

  // Copy arguments that are passed on the stack to the appropriate stack
  // locations.
  Variable *SP = getPhysicalRegister(RegMIPS32::Reg_SP);
  for (auto &StackArg : StackArgs) {
    ConstantInteger32 *Loc =
        llvm::cast<ConstantInteger32>(Ctx->getConstantInt32(StackArg.second));
    Type Ty = StackArg.first->getType();
    OperandMIPS32Mem *Addr;
    constexpr bool SignExt = false;
    if (OperandMIPS32Mem::canHoldOffset(Ty, SignExt, StackArg.second)) {
      Addr = OperandMIPS32Mem::create(Func, Ty, SP, Loc);
    } else {
      Variable *NewBase = Func->makeVariable(SP->getType());
      lowerArithmetic(
          InstArithmetic::create(Func, InstArithmetic::Add, NewBase, SP, Loc));
      Addr = formMemoryOperand(NewBase, Ty);
    }
    lowerStore(InstStore::create(Func, StackArg.first, Addr));
  }

  // Generate the call instruction.  Assign its result to a temporary with high
  // register allocation weight.
  Variable *Dest = Instr->getDest();
  // ReturnReg doubles as ReturnRegLo as necessary.
  Variable *ReturnReg = nullptr;
  Variable *ReturnRegHi = nullptr;
  if (Dest) {
    switch (Dest->getType()) {
    case IceType_NUM:
      llvm_unreachable("Invalid Call dest type");
      return;
    case IceType_void:
      break;
    case IceType_i1:
    case IceType_i8:
    case IceType_i16:
    case IceType_i32:
      ReturnReg = makeReg(Dest->getType(), RegMIPS32::Reg_V0);
      break;
    case IceType_i64:
      ReturnReg = I32Reg(RegMIPS32::Reg_V0);
      ReturnRegHi = I32Reg(RegMIPS32::Reg_V1);
      break;
    case IceType_f32:
      ReturnReg = makeReg(Dest->getType(), RegMIPS32::Reg_F0);
      break;
    case IceType_f64:
      ReturnReg = makeReg(IceType_f32, RegMIPS32::Reg_F0);
      ReturnRegHi = makeReg(IceType_f32, RegMIPS32::Reg_F1);
      break;
    case IceType_v4i1:
    case IceType_v8i1:
    case IceType_v16i1:
    case IceType_v16i8:
    case IceType_v8i16:
    case IceType_v4i32:
    case IceType_v4f32:
      UnimplementedLoweringError(this, Instr);
      return;
    }
  }
  Operand *CallTarget = Instr->getCallTarget();
  // Allow ConstantRelocatable to be left alone as a direct call,
  // but force other constants like ConstantInteger32 to be in
  // a register and make it an indirect call.
  if (!llvm::isa<ConstantRelocatable>(CallTarget)) {
    CallTarget = legalize(CallTarget, Legal_Reg);
  }

  // Copy arguments to be passed in registers to the appropriate registers.
  CfgVector<Variable *> RegArgs;
  for (auto &FPArg : FPArgs) {
    RegArgs.emplace_back(legalizeToReg(FPArg.first, FPArg.second));
  }
  for (auto &GPRArg : GPRArgs) {
    RegArgs.emplace_back(legalizeToReg(GPRArg.first, GPRArg.second));
  }

  // Generate a FakeUse of register arguments so that they do not get dead code
  // eliminated as a result of the FakeKill of scratch registers after the call.
  // These fake-uses need to be placed here to avoid argument registers from
  // being used during the legalizeToReg() calls above.
  for (auto *RegArg : RegArgs) {
    Context.insert<InstFakeUse>(RegArg);
  }

  // If variable alloca is used the extra 16 bytes for argument build area
  // will be allocated on stack before a call.
  if (VariableAllocaUsed)
    _addiu(SP, SP, -MaxOutArgsSizeBytes);

  Inst *NewCall = InstMIPS32Call::create(Func, ReturnReg, CallTarget);
  Context.insert(NewCall);

  if (VariableAllocaUsed)
    _addiu(SP, SP, MaxOutArgsSizeBytes);

  // Insert a fake use of stack pointer to avoid dead code elimination of addiu
  // instruction.
  Context.insert<InstFakeUse>(SP);

  if (ReturnRegHi)
    Context.insert(InstFakeDef::create(Func, ReturnRegHi));
  // Insert a register-kill pseudo instruction.
  Context.insert(InstFakeKill::create(Func, NewCall));
  // Generate a FakeUse to keep the call live if necessary.
  if (Instr->hasSideEffects() && ReturnReg) {
    Context.insert<InstFakeUse>(ReturnReg);
  }
  if (Dest == nullptr)
    return;

  // Assign the result of the call to Dest.
  if (ReturnReg) {
    if (ReturnRegHi) {
      assert(Dest->getType() == IceType_i64);
      auto *Dest64On32 = llvm::cast<Variable64On32>(Dest);
      Variable *DestLo = Dest64On32->getLo();
      Variable *DestHi = Dest64On32->getHi();
      _mov(DestLo, ReturnReg);
      _mov(DestHi, ReturnRegHi);
    } else {
      assert(Dest->getType() == IceType_i32 || Dest->getType() == IceType_i16 ||
             Dest->getType() == IceType_i8 || Dest->getType() == IceType_i1 ||
             isVectorType(Dest->getType()));
      if (isFloatingType(Dest->getType()) || isVectorType(Dest->getType())) {
        UnimplementedLoweringError(this, Instr);
        return;
      } else {
        _mov(Dest, ReturnReg);
      }
    }
  }
}

void TargetMIPS32::lowerCast(const InstCast *Instr) {
  InstCast::OpKind CastKind = Instr->getCastKind();
  Variable *Dest = Instr->getDest();
  Operand *Src0 = legalizeUndef(Instr->getSrc(0));
  const Type DestTy = Dest->getType();
  const Type Src0Ty = Src0->getType();
  const uint32_t ShiftAmount =
      (Src0Ty == IceType_i1
           ? INT32_BITS - 1
           : INT32_BITS - (CHAR_BITS * typeWidthInBytes(Src0Ty)));
  const uint32_t Mask =
      (Src0Ty == IceType_i1
           ? 1
           : (1 << (CHAR_BITS * typeWidthInBytes(Src0Ty))) - 1);

  if (isVectorType(DestTy)) {
    UnimplementedLoweringError(this, Instr);
    return;
  }
  switch (CastKind) {
  default:
    Func->setError("Cast type not supported");
    return;
  case InstCast::Sext: {
    if (DestTy == IceType_i64) {
      auto *DestLo = llvm::cast<Variable>(loOperand(Dest));
      auto *DestHi = llvm::cast<Variable>(hiOperand(Dest));
      Variable *Src0R = legalizeToReg(Src0);
      Variable *T1_Lo = I32Reg();
      Variable *T2_Lo = I32Reg();
      Variable *T_Hi = I32Reg();
      if (Src0Ty == IceType_i1) {
        _sll(T1_Lo, Src0R, INT32_BITS - 1);
        _sra(T2_Lo, T1_Lo, INT32_BITS - 1);
        _mov(DestHi, T2_Lo);
        _mov(DestLo, T2_Lo);
      } else if (Src0Ty == IceType_i8 || Src0Ty == IceType_i16) {
        _sll(T1_Lo, Src0R, ShiftAmount);
        _sra(T2_Lo, T1_Lo, ShiftAmount);
        _sra(T_Hi, T2_Lo, INT32_BITS - 1);
        _mov(DestHi, T_Hi);
        _mov(DestLo, T2_Lo);
      } else if (Src0Ty == IceType_i32) {
        _mov(T1_Lo, Src0R);
        _sra(T_Hi, T1_Lo, INT32_BITS - 1);
        _mov(DestHi, T_Hi);
        _mov(DestLo, T1_Lo);
      }
    } else {
      Variable *Src0R = legalizeToReg(Src0);
      Variable *T1 = makeReg(DestTy);
      Variable *T2 = makeReg(DestTy);
      if (Src0Ty == IceType_i1 || Src0Ty == IceType_i8 ||
          Src0Ty == IceType_i16) {
        _sll(T1, Src0R, ShiftAmount);
        _sra(T2, T1, ShiftAmount);
        _mov(Dest, T2);
      }
    }
    break;
  }
  case InstCast::Zext: {
    if (DestTy == IceType_i64) {
      auto *DestLo = llvm::cast<Variable>(loOperand(Dest));
      auto *DestHi = llvm::cast<Variable>(hiOperand(Dest));
      Variable *Src0R = legalizeToReg(Src0);
      Variable *T_Lo = I32Reg();
      Variable *T_Hi = I32Reg();

      if (Src0Ty == IceType_i1 || Src0Ty == IceType_i8 || Src0Ty == IceType_i16)
        _andi(T_Lo, Src0R, Mask);
      else if (Src0Ty == IceType_i32)
        _mov(T_Lo, Src0R);
      else
        assert(Src0Ty != IceType_i64);
      _mov(DestLo, T_Lo);

      auto *Zero = getZero();
      _addiu(T_Hi, Zero, 0);
      _mov(DestHi, T_Hi);
    } else {
      Variable *Src0R = legalizeToReg(Src0);
      Variable *T = makeReg(DestTy);
      if (Src0Ty == IceType_i1 || Src0Ty == IceType_i8 ||
          Src0Ty == IceType_i16) {
        _andi(T, Src0R, Mask);
        _mov(Dest, T);
      }
    }
    break;
  }
  case InstCast::Trunc: {
    if (Src0Ty == IceType_i64)
      Src0 = loOperand(Src0);
    Variable *Src0R = legalizeToReg(Src0);
    Variable *T = makeReg(DestTy);
    _mov(T, Src0R);
    _mov(Dest, T);
    break;
  }
  case InstCast::Fptrunc:
    // Use _cvt_d_s
    UnimplementedLoweringError(this, Instr);
    break;
  case InstCast::Fpext: {
    // Use _cvt_s_d
    UnimplementedLoweringError(this, Instr);
    break;
  }
  case InstCast::Fptosi:
    UnimplementedLoweringError(this, Instr);
    break;
  case InstCast::Fptoui:
    UnimplementedLoweringError(this, Instr);
    break;
  case InstCast::Sitofp:
    UnimplementedLoweringError(this, Instr);
    break;
  case InstCast::Uitofp: {
    UnimplementedLoweringError(this, Instr);
    break;
  }
  case InstCast::Bitcast: {
    UnimplementedLoweringError(this, Instr);
    break;
  }
  }
}

void TargetMIPS32::lowerExtractElement(const InstExtractElement *Instr) {
  UnimplementedLoweringError(this, Instr);
}

void TargetMIPS32::lowerFcmp(const InstFcmp *Instr) {
  UnimplementedLoweringError(this, Instr);
}

void TargetMIPS32::lower64Icmp(const InstIcmp *Instr) {
  UnimplementedLoweringError(this, Instr);
  return;
}

void TargetMIPS32::lowerIcmp(const InstIcmp *Instr) {
  auto *Src0 = Instr->getSrc(0);
  auto *Src1 = Instr->getSrc(1);
  if (Src0->getType() == IceType_i64) {
    lower64Icmp(Instr);
    return;
  }
  Variable *Dest = Instr->getDest();
  if (isVectorType(Dest->getType())) {
    UnimplementedLoweringError(this, Instr);
    return;
  }
  InstIcmp::ICond Cond = Instr->getCondition();
  auto *Src0R = legalizeToReg(Src0);
  auto *Src1R = legalizeToReg(Src1);
  const Type Src0Ty = Src0R->getType();
  const uint32_t ShAmt = INT32_BITS - getScalarIntBitWidth(Src0->getType());
  Variable *Src0RT = I32Reg();
  Variable *Src1RT = I32Reg();

  if (Src0Ty != IceType_i32) {
    _sll(Src0RT, Src0R, ShAmt);
    _sll(Src1RT, Src1R, ShAmt);
  } else {
    _mov(Src0RT, Src0R);
    _mov(Src1RT, Src1R);
  }

  switch (Cond) {
  case InstIcmp::Eq: {
    auto *DestT = I32Reg();
    auto *T = I32Reg();
    _xor(T, Src0RT, Src1RT);
    _sltiu(DestT, T, 1);
    _mov(Dest, DestT);
    return;
  }
  case InstIcmp::Ne: {
    auto *DestT = I32Reg();
    auto *T = I32Reg();
    auto *Zero = getZero();
    _xor(T, Src0RT, Src1RT);
    _sltu(DestT, Zero, T);
    _mov(Dest, DestT);
    return;
  }
  case InstIcmp::Ugt: {
    auto *DestT = I32Reg();
    _sltu(DestT, Src1RT, Src0RT);
    _mov(Dest, DestT);
    return;
  }
  case InstIcmp::Uge: {
    auto *DestT = I32Reg();
    auto *T = I32Reg();
    _sltu(T, Src0RT, Src1RT);
    _xori(DestT, T, 1);
    _mov(Dest, DestT);
    return;
  }
  case InstIcmp::Ult: {
    auto *DestT = I32Reg();
    _sltu(DestT, Src0RT, Src1RT);
    _mov(Dest, DestT);
    return;
  }
  case InstIcmp::Ule: {
    auto *DestT = I32Reg();
    auto *T = I32Reg();
    _sltu(T, Src1RT, Src0RT);
    _xori(DestT, T, 1);
    _mov(Dest, DestT);
    return;
  }
  case InstIcmp::Sgt: {
    auto *DestT = I32Reg();
    _slt(DestT, Src1RT, Src0RT);
    _mov(Dest, DestT);
    return;
  }
  case InstIcmp::Sge: {
    auto *DestT = I32Reg();
    auto *T = I32Reg();
    _slt(T, Src0RT, Src1RT);
    _xori(DestT, T, 1);
    _mov(Dest, DestT);
    return;
  }
  case InstIcmp::Slt: {
    auto *DestT = I32Reg();
    _slt(DestT, Src0RT, Src1RT);
    _mov(Dest, DestT);
    return;
  }
  case InstIcmp::Sle: {
    auto *DestT = I32Reg();
    auto *T = I32Reg();
    _slt(T, Src1RT, Src0RT);
    _xori(DestT, T, 1);
    _mov(Dest, DestT);
    return;
  }
  default:
    llvm_unreachable("Invalid ICmp operator");
    return;
  }
}

void TargetMIPS32::lowerInsertElement(const InstInsertElement *Instr) {
  UnimplementedLoweringError(this, Instr);
}

void TargetMIPS32::lowerIntrinsicCall(const InstIntrinsicCall *Instr) {
  switch (Instr->getIntrinsicInfo().ID) {
  case Intrinsics::AtomicCmpxchg: {
    UnimplementedLoweringError(this, Instr);
    return;
  }
  case Intrinsics::AtomicFence:
    UnimplementedLoweringError(this, Instr);
    return;
  case Intrinsics::AtomicFenceAll:
    // NOTE: FenceAll should prevent and load/store from being moved across the
    // fence (both atomic and non-atomic). The InstMIPS32Mfence instruction is
    // currently marked coarsely as "HasSideEffects".
    UnimplementedLoweringError(this, Instr);
    return;
  case Intrinsics::AtomicIsLockFree: {
    UnimplementedLoweringError(this, Instr);
    return;
  }
  case Intrinsics::AtomicLoad: {
    UnimplementedLoweringError(this, Instr);
    return;
  }
  case Intrinsics::AtomicRMW:
    UnimplementedLoweringError(this, Instr);
    return;
  case Intrinsics::AtomicStore: {
    UnimplementedLoweringError(this, Instr);
    return;
  }
  case Intrinsics::Bswap: {
    UnimplementedLoweringError(this, Instr);
    return;
  }
  case Intrinsics::Ctpop: {
    UnimplementedLoweringError(this, Instr);
    return;
  }
  case Intrinsics::Ctlz: {
    UnimplementedLoweringError(this, Instr);
    return;
  }
  case Intrinsics::Cttz: {
    UnimplementedLoweringError(this, Instr);
    return;
  }
  case Intrinsics::Fabs: {
    UnimplementedLoweringError(this, Instr);
    return;
  }
  case Intrinsics::Longjmp: {
    InstCall *Call = makeHelperCall(RuntimeHelper::H_call_longjmp, nullptr, 2);
    Call->addArg(Instr->getArg(0));
    Call->addArg(Instr->getArg(1));
    lowerCall(Call);
    return;
  }
  case Intrinsics::Memcpy: {
    // In the future, we could potentially emit an inline memcpy/memset, etc.
    // for intrinsic calls w/ a known length.
    InstCall *Call = makeHelperCall(RuntimeHelper::H_call_memcpy, nullptr, 3);
    Call->addArg(Instr->getArg(0));
    Call->addArg(Instr->getArg(1));
    Call->addArg(Instr->getArg(2));
    lowerCall(Call);
    return;
  }
  case Intrinsics::Memmove: {
    InstCall *Call = makeHelperCall(RuntimeHelper::H_call_memmove, nullptr, 3);
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
    InstCall *Call = makeHelperCall(RuntimeHelper::H_call_memset, nullptr, 3);
    Call->addArg(Instr->getArg(0));
    Call->addArg(ValExt);
    Call->addArg(Instr->getArg(2));
    lowerCall(Call);
    return;
  }
  case Intrinsics::NaClReadTP: {
    if (getFlags().getUseSandboxing()) {
      UnimplementedLoweringError(this, Instr);
    } else {
      InstCall *Call =
          makeHelperCall(RuntimeHelper::H_call_read_tp, Instr->getDest(), 0);
      lowerCall(Call);
    }
    return;
  }
  case Intrinsics::Setjmp: {
    InstCall *Call =
        makeHelperCall(RuntimeHelper::H_call_setjmp, Instr->getDest(), 1);
    Call->addArg(Instr->getArg(0));
    lowerCall(Call);
    return;
  }
  case Intrinsics::Sqrt: {
    UnimplementedLoweringError(this, Instr);
    return;
  }
  case Intrinsics::Stacksave: {
    UnimplementedLoweringError(this, Instr);
    return;
  }
  case Intrinsics::Stackrestore: {
    UnimplementedLoweringError(this, Instr);
    return;
  }
  case Intrinsics::Trap:
    UnimplementedLoweringError(this, Instr);
    return;
  case Intrinsics::UnknownIntrinsic:
    Func->setError("Should not be lowering UnknownIntrinsic");
    return;
  }
  return;
}

void TargetMIPS32::lowerLoad(const InstLoad *Instr) {
  UnimplementedLoweringError(this, Instr);
}

void TargetMIPS32::doAddressOptLoad() { UnimplementedError(getFlags()); }

void TargetMIPS32::randomlyInsertNop(float Probability,
                                     RandomNumberGenerator &RNG) {
  RandomNumberGeneratorWrapper RNGW(RNG);
  if (RNGW.getTrueWithProbability(Probability)) {
    UnimplementedError(getFlags());
  }
}

void TargetMIPS32::lowerPhi(const InstPhi * /*Instr*/) {
  Func->setError("Phi found in regular instruction list");
}

void TargetMIPS32::lowerRet(const InstRet *Instr) {
  Variable *Reg = nullptr;
  if (Instr->hasRetValue()) {
    Operand *Src0 = Instr->getRetValue();
    switch (Src0->getType()) {
    case IceType_i1:
    case IceType_i8:
    case IceType_i16:
    case IceType_i32: {
      // Reg = legalizeToReg(Src0, RegMIPS32::Reg_V0);
      Operand *Src0F = legalize(Src0, Legal_Reg);
      Reg = makeReg(Src0F->getType(), RegMIPS32::Reg_V0);
      _mov(Reg, Src0F);
      break;
    }
    case IceType_i64: {
      Src0 = legalizeUndef(Src0);
      Variable *R0 = legalizeToReg(loOperand(Src0), RegMIPS32::Reg_V0);
      Variable *R1 = legalizeToReg(hiOperand(Src0), RegMIPS32::Reg_V1);
      Reg = R0;
      Context.insert<InstFakeUse>(R1);
      break;
    }
    default:
      UnimplementedLoweringError(this, Instr);
    }
  }
  _ret(getPhysicalRegister(RegMIPS32::Reg_RA), Reg);
}

void TargetMIPS32::lowerSelect(const InstSelect *Instr) {
  UnimplementedLoweringError(this, Instr);
}

void TargetMIPS32::lowerShuffleVector(const InstShuffleVector *Instr) {
  UnimplementedLoweringError(this, Instr);
}

void TargetMIPS32::lowerStore(const InstStore *Instr) {
  Operand *Value = Instr->getData();
  Operand *Addr = Instr->getAddr();
  OperandMIPS32Mem *NewAddr = formMemoryOperand(Addr, Value->getType());
  Type Ty = NewAddr->getType();

  if (Ty == IceType_i64) {
    Value = legalizeUndef(Value);
    Variable *ValueHi = legalizeToReg(hiOperand(Value));
    Variable *ValueLo = legalizeToReg(loOperand(Value));
    _sw(ValueHi, llvm::cast<OperandMIPS32Mem>(hiOperand(NewAddr)));
    _sw(ValueLo, llvm::cast<OperandMIPS32Mem>(loOperand(NewAddr)));
  } else {
    Variable *ValueR = legalizeToReg(Value);
    _sw(ValueR, NewAddr);
  }
}

void TargetMIPS32::doAddressOptStore() { UnimplementedError(getFlags()); }

void TargetMIPS32::lowerSwitch(const InstSwitch *Instr) {
  UnimplementedLoweringError(this, Instr);
}

void TargetMIPS32::lowerBreakpoint(const InstBreakpoint *Instr) {
  UnimplementedLoweringError(this, Instr);
}

void TargetMIPS32::lowerUnreachable(const InstUnreachable *Instr) {
  UnimplementedLoweringError(this, Instr);
}

// Turn an i64 Phi instruction into a pair of i32 Phi instructions, to preserve
// integrity of liveness analysis. Undef values are also turned into zeroes,
// since loOperand() and hiOperand() don't expect Undef input.
void TargetMIPS32::prelowerPhis() {
  PhiLowering::prelowerPhis32Bit<TargetMIPS32>(this, Context.getNode(), Func);
}

void TargetMIPS32::postLower() {
  if (Func->getOptLevel() == Opt_m1)
    return;
  // TODO(rkotler): Find two-address non-SSA instructions where Dest==Src0,
  // and set the IsDestRedefined flag to keep liveness analysis consistent.
  UnimplementedError(getFlags());
}

void TargetMIPS32::makeRandomRegisterPermutation(
    llvm::SmallVectorImpl<RegNumT> &Permutation,
    const SmallBitVector &ExcludeRegisters, uint64_t Salt) const {
  (void)Permutation;
  (void)ExcludeRegisters;
  (void)Salt;
  UnimplementedError(getFlags());
}

/* TODO(jvoung): avoid duplicate symbols with multiple targets.
void ConstantUndef::emitWithoutDollar(GlobalContext *) const {
  llvm_unreachable("Not expecting to emitWithoutDollar undef");
}

void ConstantUndef::emit(GlobalContext *) const {
  llvm_unreachable("undef value encountered by emitter.");
}
*/

TargetDataMIPS32::TargetDataMIPS32(GlobalContext *Ctx)
    : TargetDataLowering(Ctx) {}

void TargetDataMIPS32::lowerGlobals(const VariableDeclarationList &Vars,
                                    const std::string &SectionSuffix) {
  const bool IsPIC = getFlags().getUseNonsfi();
  switch (getFlags().getOutFileType()) {
  case FT_Elf: {
    ELFObjectWriter *Writer = Ctx->getObjectWriter();
    Writer->writeDataSection(Vars, llvm::ELF::R_MIPS_GLOB_DAT, SectionSuffix,
                             IsPIC);
  } break;
  case FT_Asm:
  case FT_Iasm: {
    OstreamLocker L(Ctx);
    for (const VariableDeclaration *Var : Vars) {
      if (getFlags().matchTranslateOnly(Var->getName(), 0)) {
        emitGlobal(*Var, SectionSuffix);
      }
    }
  } break;
  }
}

void TargetDataMIPS32::lowerConstants() {
  if (getFlags().getDisableTranslation())
    return;
}

void TargetDataMIPS32::lowerJumpTables() {
  if (getFlags().getDisableTranslation())
    return;
}

// Helper for legalize() to emit the right code to lower an operand to a
// register of the appropriate type.
Variable *TargetMIPS32::copyToReg(Operand *Src, RegNumT RegNum) {
  Type Ty = Src->getType();
  Variable *Reg = makeReg(Ty, RegNum);
  if (isVectorType(Ty)) {
    UnimplementedError(getFlags());
  } else {
    // Mov's Src operand can really only be the flexible second operand type
    // or a register. Users should guarantee that.
    _mov(Reg, Src);
  }
  return Reg;
}

Operand *TargetMIPS32::legalize(Operand *From, LegalMask Allowed,
                                RegNumT RegNum) {
  Type Ty = From->getType();
  // Assert that a physical register is allowed.  To date, all calls
  // to legalize() allow a physical register. Legal_Flex converts
  // registers to the right type OperandMIPS32FlexReg as needed.
  assert(Allowed & Legal_Reg);
  // Go through the various types of operands:
  // OperandMIPS32Mem, Constant, and Variable.
  // Given the above assertion, if type of operand is not legal
  // (e.g., OperandMIPS32Mem and !Legal_Mem), we can always copy
  // to a register.
  if (llvm::isa<Constant>(From)) {
    if (auto *C = llvm::dyn_cast<ConstantRelocatable>(From)) {
      (void)C;
      // TODO(reed kotler): complete this case for proper implementation
      Variable *Reg = makeReg(Ty, RegNum);
      Context.insert<InstFakeDef>(Reg);
      return Reg;
    } else if (auto *C32 = llvm::dyn_cast<ConstantInteger32>(From)) {
      const uint32_t Value = C32->getValue();
      // Check if the immediate will fit in a Flexible second operand,
      // if a Flexible second operand is allowed. We need to know the exact
      // value, so that rules out relocatable constants.
      // Also try the inverse and use MVN if possible.
      // Do a movw/movt to a register.
      Variable *Reg;
      if (RegNum.hasValue())
        Reg = getPhysicalRegister(RegNum);
      else
        Reg = makeReg(Ty, RegNum);
      if (isInt<16>(int32_t(Value))) {
        Variable *Zero = getPhysicalRegister(RegMIPS32::Reg_ZERO, Ty);
        Context.insert<InstFakeDef>(Zero);
        _addiu(Reg, Zero, Value);
      } else {
        uint32_t UpperBits = (Value >> 16) & 0xFFFF;
        (void)UpperBits;
        uint32_t LowerBits = Value & 0xFFFF;
        Variable *TReg = makeReg(Ty, RegNum);
        if (LowerBits) {
          _lui(TReg, Ctx->getConstantInt32(UpperBits));
          _ori(Reg, TReg, LowerBits);
        } else {
          _lui(Reg, Ctx->getConstantInt32(UpperBits));
        }
      }
      return Reg;
    } else if (isScalarFloatingType(Ty)) {
      // Load floats/doubles from literal pool.
      auto *CFrom = llvm::cast<Constant>(From);
      assert(CFrom->getShouldBePooled());
      Constant *Offset = Ctx->getConstantSym(0, CFrom->getLabelName());
      Variable *TReg1 = makeReg(getPointerType());
      Variable *TReg2 = makeReg(Ty);
      Context.insert<InstFakeDef>(TReg2);
      _lui(TReg1, Offset, RO_Hi);
      OperandMIPS32Mem *Addr =
          OperandMIPS32Mem::create(Func, Ty, TReg1, Offset);
      if (Ty == IceType_f32)
        _lwc1(TReg2, Addr, RO_Lo);
      else
        _ldc1(TReg2, Addr, RO_Lo);
      return copyToReg(TReg2, RegNum);
    }
  }

  if (auto *Var = llvm::dyn_cast<Variable>(From)) {
    // Check if the variable is guaranteed a physical register.  This
    // can happen either when the variable is pre-colored or when it is
    // assigned infinite weight.
    bool MustHaveRegister = (Var->hasReg() || Var->mustHaveReg());
    // We need a new physical register for the operand if:
    //   Mem is not allowed and Var isn't guaranteed a physical
    //   register, or
    //   RegNum is required and Var->getRegNum() doesn't match.
    if ((!(Allowed & Legal_Mem) && !MustHaveRegister) ||
        (RegNum.hasValue() && RegNum != Var->getRegNum())) {
      From = copyToReg(From, RegNum);
    }
    return From;
  }
  return From;
}

namespace BoolFolding {
// TODO(sagar.thakur): Add remaining instruction kinds to shouldTrackProducer()
// and isValidConsumer()
bool shouldTrackProducer(const Inst &Instr) {
  return Instr.getKind() == Inst::Icmp;
}

bool isValidConsumer(const Inst &Instr) { return Instr.getKind() == Inst::Br; }
} // end of namespace BoolFolding

void TargetMIPS32::ComputationTracker::recordProducers(CfgNode *Node) {
  for (Inst &Instr : Node->getInsts()) {
    if (Instr.isDeleted())
      continue;
    // Check whether Instr is a valid producer.
    Variable *Dest = Instr.getDest();
    if (Dest // only consider instructions with an actual dest var; and
        && Dest->getType() == IceType_i1 // only bool-type dest vars; and
        && BoolFolding::shouldTrackProducer(Instr)) { // white-listed instr.
      KnownComputations.emplace(Dest->getIndex(),
                                ComputationEntry(&Instr, IceType_i1));
    }
    // Check each src variable against the map.
    FOREACH_VAR_IN_INST(Var, Instr) {
      SizeT VarNum = Var->getIndex();
      auto ComputationIter = KnownComputations.find(VarNum);
      if (ComputationIter == KnownComputations.end()) {
        continue;
      }

      ++ComputationIter->second.NumUses;
      switch (ComputationIter->second.ComputationType) {
      default:
        KnownComputations.erase(VarNum);
        continue;
      case IceType_i1:
        if (!BoolFolding::isValidConsumer(Instr)) {
          KnownComputations.erase(VarNum);
          continue;
        }
        break;
      }

      if (Instr.isLastUse(Var)) {
        ComputationIter->second.IsLiveOut = false;
      }
    }
  }

  for (auto Iter = KnownComputations.begin(), End = KnownComputations.end();
       Iter != End;) {
    // Disable the folding if its dest may be live beyond this block.
    if (Iter->second.IsLiveOut || Iter->second.NumUses > 1) {
      Iter = KnownComputations.erase(Iter);
      continue;
    }

    // Mark as "dead" rather than outright deleting. This is so that other
    // peephole style optimizations during or before lowering have access to
    // this instruction in undeleted form. See for example
    // tryOptimizedCmpxchgCmpBr().
    Iter->second.Instr->setDead();
    ++Iter;
  }
}

TargetHeaderMIPS32::TargetHeaderMIPS32(GlobalContext *Ctx)
    : TargetHeaderLowering(Ctx) {}

void TargetHeaderMIPS32::lower() {
  OstreamLocker L(Ctx);
  Ostream &Str = Ctx->getStrEmit();
  Str << "\t.set\t"
      << "nomicromips\n";
  Str << "\t.set\t"
      << "nomips16\n";
}

SmallBitVector TargetMIPS32::TypeToRegisterSet[RCMIPS32_NUM];
SmallBitVector TargetMIPS32::TypeToRegisterSetUnfiltered[RCMIPS32_NUM];
SmallBitVector TargetMIPS32::RegisterAliases[RegMIPS32::Reg_NUM];

} // end of namespace MIPS32
} // end of namespace Ice
