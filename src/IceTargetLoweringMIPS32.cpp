//===- subzero/src/IceTargetLoweringMIPS32.cpp - MIPS32 lowering ----------===//
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
} // end of namespace MIPS32

namespace Ice {
namespace MIPS32 {

using llvm::isInt;

namespace {

// The maximum number of arguments to pass in GPR registers.
constexpr uint32_t MIPS32_MAX_GPR_ARG = 4;

IceString getRegClassName(RegClass C) {
  auto ClassNum = static_cast<RegClassMIPS32>(C);
  assert(ClassNum < RCMIPS32_NUM);
  switch (ClassNum) {
  default:
    assert(C < RC_Target);
    return regClassString(C);
    // Add handling of new register classes below.
  }
}

} // end of anonymous namespace

TargetMIPS32::TargetMIPS32(Cfg *Func) : TargetLowering(Func) {}

void TargetMIPS32::staticInit(GlobalContext *Ctx) {
  (void)Ctx;
  RegNumT::setLimit(RegMIPS32::Reg_NUM);
  llvm::SmallBitVector IntegerRegisters(RegMIPS32::Reg_NUM);
  llvm::SmallBitVector I64PairRegisters(RegMIPS32::Reg_NUM);
  llvm::SmallBitVector Float32Registers(RegMIPS32::Reg_NUM);
  llvm::SmallBitVector Float64Registers(RegMIPS32::Reg_NUM);
  llvm::SmallBitVector VectorRegisters(RegMIPS32::Reg_NUM);
  llvm::SmallBitVector InvalidRegisters(RegMIPS32::Reg_NUM);
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

void TargetMIPS32::translateO2() {
  TimerMarker T(TimerStack::TT_O2, Func);

  // TODO(stichnot): share passes with X86?
  // https://code.google.com/p/nativeclient/issues/detail?id=4094
  genTargetHelperCalls();

  // Merge Alloca instructions, and lay out the stack.
  static constexpr bool SortAndCombineAllocas = false;
  Func->processAllocas(SortAndCombineAllocas);
  Func->dump("After Alloca processing");

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
  // Validate the live range computations. The expensive validation call is
  // deliberately only made when assertions are enabled.
  assert(Func->validateLiveness());
  // The post-codegen dump is done here, after liveness analysis and associated
  // cleanup, to make the dump cleaner and more useful.
  Func->dump("After initial MIPS32 codegen");
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

  Func->contractEmptyNodes();
  Func->reorderNodes();

  // Branch optimization. This needs to be done just before code emission. In
  // particular, no transformations that insert or reorder CfgNodes should be
  // done after branch optimization. We go ahead and do it before nop insertion
  // to reduce the amount of work needed for searching for opportunities.
  Func->doBranchOpt();
  Func->dump("After branch optimization");

  // Nop insertion
  if (Ctx->getFlags().shouldDoNopInsertion()) {
    Func->doNopInsertion();
  }
}

void TargetMIPS32::translateOm1() {
  TimerMarker T(TimerStack::TT_Om1, Func);

  // TODO: share passes with X86?
  genTargetHelperCalls();

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
  if (Ctx->getFlags().shouldDoNopInsertion()) {
    Func->doNopInsertion();
  }
}

bool TargetMIPS32::doBranchOpt(Inst *Instr, const CfgNode *NextNode) {
  (void)Instr;
  (void)NextNode;
  UnimplementedError(Func->getContext()->getFlags());
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

IceString TargetMIPS32::getRegName(RegNumT RegNum, Type Ty) const {
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
  (void)JumpTable;
  UnimplementedError(Func->getContext()->getFlags());
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
      UnimplementedError(Func->getContext()->getFlags());
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

void TargetMIPS32::emitVariable(const Variable *Var) const {
  if (!BuildDefs::dump())
    return;
  Ostream &Str = Ctx->getStrEmit();
  const Type FrameSPTy = IceType_i32;
  if (Var->hasReg()) {
    Str << '$' << getRegName(Var->getRegNum(), Var->getType());
    return;
  } else {
    int32_t Offset = Var->getStackOffset();
    Str << Offset;
    Str << "($" << getRegName(getFrameOrStackReg(), FrameSPTy);
    Str << ")";
  }
  UnimplementedError(Func->getContext()->getFlags());
}

void TargetMIPS32::lowerArguments() {
  VarList &Args = Func->getArgs();
  // We are only handling integer registers for now. The Mips o32 ABI is
  // somewhat complex but will be implemented in its totality through follow
  // on patches.
  //
  unsigned NumGPRRegsUsed = 0;
  // For each register argument, replace Arg in the argument list with the
  // home register.  Then generate an instruction in the prolog to copy the
  // home register to the assigned location of Arg.
  Context.init(Func->getEntryNode());
  Context.setInsertPoint(Context.getCur());
  for (SizeT I = 0, E = Args.size(); I < E; ++I) {
    Variable *Arg = Args[I];
    Type Ty = Arg->getType();
    // TODO(rkotler): handle float/vector types.
    if (isVectorType(Ty)) {
      UnimplementedError(Func->getContext()->getFlags());
      continue;
    }
    if (isFloatingType(Ty)) {
      UnimplementedError(Func->getContext()->getFlags());
      continue;
    }
    if (Ty == IceType_i64) {
      if (NumGPRRegsUsed >= MIPS32_MAX_GPR_ARG)
        continue;
      auto RegLo = RegNumT::fixme(RegMIPS32::Reg_A0 + NumGPRRegsUsed);
      auto RegHi = RegNumT::fixme(RegLo + 1);
      ++NumGPRRegsUsed;
      // Always start i64 registers at an even register, so this may end
      // up padding away a register.
      if (RegLo % 2 != 0) {
        RegLo = RegNumT::fixme(RegLo + 1);
        ++NumGPRRegsUsed;
      }
      // If this leaves us without room to consume another register,
      // leave any previously speculatively consumed registers as consumed.
      if (NumGPRRegsUsed >= MIPS32_MAX_GPR_ARG)
        continue;
      // RegHi = RegNumT::fixme(RegMIPS32::Reg_A0 + NumGPRRegsUsed);
      ++NumGPRRegsUsed;
      Variable *RegisterArg = Func->makeVariable(Ty);
      auto *RegisterArg64On32 = llvm::cast<Variable64On32>(RegisterArg);
      if (BuildDefs::dump())
        RegisterArg64On32->setName(Func, "home_reg:" + Arg->getName(Func));
      RegisterArg64On32->initHiLo(Func);
      RegisterArg64On32->setIsArg();
      RegisterArg64On32->getLo()->setRegNum(RegLo);
      RegisterArg64On32->getHi()->setRegNum(RegHi);
      Arg->setIsArg(false);
      Args[I] = RegisterArg64On32;
      Context.insert<InstAssign>(Arg, RegisterArg);
      continue;
    } else {
      assert(Ty == IceType_i32);
      if (NumGPRRegsUsed >= MIPS32_MAX_GPR_ARG)
        continue;
      const auto RegNum = RegNumT::fixme(RegMIPS32::Reg_A0 + NumGPRRegsUsed);
      ++NumGPRRegsUsed;
      Variable *RegisterArg = Func->makeVariable(Ty);
      if (BuildDefs::dump()) {
        RegisterArg->setName(Func, "home_reg:" + Arg->getName(Func));
      }
      RegisterArg->setRegNum(RegNum);
      RegisterArg->setIsArg();
      Arg->setIsArg(false);
      Args[I] = RegisterArg;
      Context.insert<InstAssign>(Arg, RegisterArg);
    }
  }
}

Type TargetMIPS32::stackSlotType() { return IceType_i32; }

void TargetMIPS32::addProlog(CfgNode *Node) {
  (void)Node;
  return;
  UnimplementedError(Func->getContext()->getFlags());
}

void TargetMIPS32::addEpilog(CfgNode *Node) {
  (void)Node;
  return;
  UnimplementedError(Func->getContext()->getFlags());
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
    ConstantInteger32 *Offset = Mem->getOffset();
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

llvm::SmallBitVector TargetMIPS32::getRegisterSet(RegSetMask Include,
                                                  RegSetMask Exclude) const {
  llvm::SmallBitVector Registers(RegMIPS32::Reg_NUM);

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
  UsesFramePointer = true;
  // Conservatively require the stack to be aligned. Some stack adjustment
  // operations implemented below assume that the stack is aligned before the
  // alloca. All the alloca code ensures that the stack alignment is preserved
  // after the alloca. The stack alignment restriction can be relaxed in some
  // cases.
  NeedsStackAlignment = true;
  UnimplementedLoweringError(this, Instr);
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
    Variable *T_Carry = makeReg(IceType_i32);
    Variable *T_Lo = makeReg(IceType_i32);
    Variable *T_Hi = makeReg(IceType_i32);
    Variable *T_Hi2 = makeReg(IceType_i32);
    _addu(T_Lo, Src0LoR, Src1LoR);
    _mov(DestLo, T_Lo);
    _sltu(T_Carry, T_Lo, Src0LoR);
    _addu(T_Hi, T_Carry, Src0HiR);
    _addu(T_Hi2, Src1HiR, T_Hi);
    _mov(DestHi, T_Hi2);
    return;
  }
  case InstArithmetic::And: {
    Variable *T_Lo = makeReg(IceType_i32);
    Variable *T_Hi = makeReg(IceType_i32);
    _and(T_Lo, Src0LoR, Src1LoR);
    _mov(DestLo, T_Lo);
    _and(T_Hi, Src0HiR, Src1HiR);
    _mov(DestHi, T_Hi);
    return;
  }
  case InstArithmetic::Sub: {
    Variable *T_Borrow = makeReg(IceType_i32);
    Variable *T_Lo = makeReg(IceType_i32);
    Variable *T_Hi = makeReg(IceType_i32);
    Variable *T_Hi2 = makeReg(IceType_i32);
    _subu(T_Lo, Src0LoR, Src1LoR);
    _mov(DestLo, T_Lo);
    _sltu(T_Borrow, Src0LoR, Src1LoR);
    _addu(T_Hi, T_Borrow, Src1HiR);
    _subu(T_Hi2, Src0HiR, T_Hi);
    _mov(DestHi, T_Hi2);
    return;
  }
  case InstArithmetic::Or: {
    Variable *T_Lo = makeReg(IceType_i32);
    Variable *T_Hi = makeReg(IceType_i32);
    _or(T_Lo, Src0LoR, Src1LoR);
    _mov(DestLo, T_Lo);
    _or(T_Hi, Src0HiR, Src1HiR);
    _mov(DestHi, T_Hi);
    return;
  }
  case InstArithmetic::Xor: {
    Variable *T_Lo = makeReg(IceType_i32);
    Variable *T_Hi = makeReg(IceType_i32);
    _xor(T_Lo, Src0LoR, Src1LoR);
    _mov(DestLo, T_Lo);
    _xor(T_Hi, Src0HiR, Src1HiR);
    _mov(DestHi, T_Hi);
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
  case InstArithmetic::Shl:
  case InstArithmetic::Lshr:
  case InstArithmetic::Ashr:
  case InstArithmetic::Udiv:
  case InstArithmetic::Sdiv:
  case InstArithmetic::Urem:
  case InstArithmetic::Srem:
  case InstArithmetic::Fadd:
  case InstArithmetic::Fsub:
  case InstArithmetic::Fmul:
  case InstArithmetic::Fdiv:
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
  case InstArithmetic::Shl:
    break;
  case InstArithmetic::Lshr:
    break;
  case InstArithmetic::Ashr:
    break;
  case InstArithmetic::Udiv:
    break;
  case InstArithmetic::Sdiv:
    break;
  case InstArithmetic::Urem:
    break;
  case InstArithmetic::Srem:
    break;
  case InstArithmetic::Fadd:
    break;
  case InstArithmetic::Fsub:
    break;
  case InstArithmetic::Fmul:
    break;
  case InstArithmetic::Fdiv:
    break;
  case InstArithmetic::Frem:
    break;
  }
  UnimplementedLoweringError(this, Instr);
}

void TargetMIPS32::lowerAssign(const InstAssign *Instr) {
  Variable *Dest = Instr->getDest();
  Operand *Src0 = Instr->getSrc(0);
  assert(Dest->getType() == Src0->getType());
  if (Dest->getType() == IceType_i64) {
    Src0 = legalizeUndef(Src0);
    Operand *Src0Lo = legalize(loOperand(Src0), Legal_Reg);
    Operand *Src0Hi = legalize(hiOperand(Src0), Legal_Reg);
    auto *DestLo = llvm::cast<Variable>(loOperand(Dest));
    auto *DestHi = llvm::cast<Variable>(hiOperand(Dest));
    // Variable *T_Lo = nullptr, *T_Hi = nullptr;
    Variable *T_Lo = makeReg(IceType_i32);
    Variable *T_Hi = makeReg(IceType_i32);
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
  UnimplementedLoweringError(this, Instr);
}

void TargetMIPS32::lowerCall(const InstCall *Instr) {
  // TODO(rkotler): assign arguments to registers and stack. Also reserve stack.
  if (Instr->getNumArgs()) {
    UnimplementedLoweringError(this, Instr);
    return;
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
      ReturnReg = makeReg(IceType_i32, RegMIPS32::Reg_V0);
      ReturnRegHi = makeReg(IceType_i32, RegMIPS32::Reg_V1);
      break;
    case IceType_f32:
    case IceType_f64:
      UnimplementedLoweringError(this, Instr);
      return;
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
  Inst *NewCall = InstMIPS32Call::create(Func, ReturnReg, CallTarget);
  Context.insert(NewCall);
  if (ReturnRegHi)
    Context.insert(InstFakeDef::create(Func, ReturnRegHi));
  // Insert a register-kill pseudo instruction.
  Context.insert(InstFakeKill::create(Func, NewCall));
  // Generate a FakeUse to keep the call live if necessary.
  if (Instr->hasSideEffects() && ReturnReg) {
    Context.insert<InstFakeDef>(ReturnReg);
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
  switch (CastKind) {
  default:
    Func->setError("Cast type not supported");
    return;
  case InstCast::Sext: {
    UnimplementedLoweringError(this, Instr);
    break;
  }
  case InstCast::Zext: {
    UnimplementedLoweringError(this, Instr);
    break;
  }
  case InstCast::Trunc: {
    UnimplementedLoweringError(this, Instr);
    break;
  }
  case InstCast::Fptrunc:
    UnimplementedLoweringError(this, Instr);
    break;
  case InstCast::Fpext: {
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

void TargetMIPS32::lowerIcmp(const InstIcmp *Instr) {
  UnimplementedLoweringError(this, Instr);
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
    InstCall *Call = makeHelperCall(H_call_memset, nullptr, 3);
    Call->addArg(Instr->getArg(0));
    Call->addArg(ValExt);
    Call->addArg(Instr->getArg(2));
    lowerCall(Call);
    return;
  }
  case Intrinsics::NaClReadTP: {
    if (Ctx->getFlags().getUseSandboxing()) {
      UnimplementedLoweringError(this, Instr);
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

void TargetMIPS32::doAddressOptLoad() {
  UnimplementedError(Func->getContext()->getFlags());
}

void TargetMIPS32::randomlyInsertNop(float Probability,
                                     RandomNumberGenerator &RNG) {
  RandomNumberGeneratorWrapper RNGW(RNG);
  if (RNGW.getTrueWithProbability(Probability)) {
    UnimplementedError(Func->getContext()->getFlags());
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

void TargetMIPS32::lowerStore(const InstStore *Instr) {
  UnimplementedLoweringError(this, Instr);
}

void TargetMIPS32::doAddressOptStore() {
  UnimplementedError(Func->getContext()->getFlags());
}

void TargetMIPS32::lowerSwitch(const InstSwitch *Instr) {
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
  if (Ctx->getFlags().getOptLevel() == Opt_m1)
    return;
  // TODO(rkotler): Find two-address non-SSA instructions where Dest==Src0,
  // and set the IsDestRedefined flag to keep liveness analysis consistent.
  UnimplementedError(Func->getContext()->getFlags());
}

void TargetMIPS32::makeRandomRegisterPermutation(
    llvm::SmallVectorImpl<RegNumT> &Permutation,
    const llvm::SmallBitVector &ExcludeRegisters, uint64_t Salt) const {
  (void)Permutation;
  (void)ExcludeRegisters;
  (void)Salt;
  UnimplementedError(Func->getContext()->getFlags());
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
                                    const IceString &SectionSuffix) {
  const bool IsPIC = Ctx->getFlags().getUseNonsfi();
  switch (Ctx->getFlags().getOutFileType()) {
  case FT_Elf: {
    ELFObjectWriter *Writer = Ctx->getObjectWriter();
    Writer->writeDataSection(Vars, llvm::ELF::R_MIPS_GLOB_DAT, SectionSuffix,
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

void TargetDataMIPS32::lowerConstants() {
  if (Ctx->getFlags().getDisableTranslation())
    return;
  UnimplementedError(Ctx->getFlags());
}

void TargetDataMIPS32::lowerJumpTables() {
  if (Ctx->getFlags().getDisableTranslation())
    return;
  UnimplementedError(Ctx->getFlags());
}

// Helper for legalize() to emit the right code to lower an operand to a
// register of the appropriate type.
Variable *TargetMIPS32::copyToReg(Operand *Src, RegNumT RegNum) {
  Type Ty = Src->getType();
  Variable *Reg = makeReg(Ty, RegNum);
  if (isVectorType(Ty) || isFloatingType(Ty)) {
    UnimplementedError(Ctx->getFlags());
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
      _lui(TReg, UpperBits);
      _ori(Reg, TReg, LowerBits);
    }
    return Reg;
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

llvm::SmallBitVector TargetMIPS32::TypeToRegisterSet[RCMIPS32_NUM];
llvm::SmallBitVector TargetMIPS32::TypeToRegisterSetUnfiltered[RCMIPS32_NUM];
llvm::SmallBitVector TargetMIPS32::RegisterAliases[RegMIPS32::Reg_NUM];

} // end of namespace MIPS32
} // end of namespace Ice
