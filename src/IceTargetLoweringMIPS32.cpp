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
  return ::Ice::TargetMIPS32::create(Func);
}

std::unique_ptr<::Ice::TargetDataLowering>
createTargetDataLowering(::Ice::GlobalContext *Ctx) {
  return ::Ice::TargetDataMIPS32::create(Ctx);
}

std::unique_ptr<::Ice::TargetHeaderLowering>
createTargetHeaderLowering(::Ice::GlobalContext *Ctx) {
  return ::Ice::TargetHeaderMIPS32::create(Ctx);
}

void staticInit() { ::Ice::TargetMIPS32::staticInit(); }
} // end of namespace MIPS32

namespace Ice {

using llvm::isInt;

namespace {

// The maximum number of arguments to pass in GPR registers.
constexpr uint32_t MIPS32_MAX_GPR_ARG = 4;

} // end of anonymous namespace

TargetMIPS32::TargetMIPS32(Cfg *Func) : TargetLowering(Func) {}

void TargetMIPS32::staticInit() {
  llvm::SmallBitVector IntegerRegisters(RegMIPS32::Reg_NUM);
  llvm::SmallBitVector I64PairRegisters(RegMIPS32::Reg_NUM);
  llvm::SmallBitVector Float32Registers(RegMIPS32::Reg_NUM);
  llvm::SmallBitVector Float64Registers(RegMIPS32::Reg_NUM);
  llvm::SmallBitVector VectorRegisters(RegMIPS32::Reg_NUM);
  llvm::SmallBitVector InvalidRegisters(RegMIPS32::Reg_NUM);
  ScratchRegs.resize(RegMIPS32::Reg_NUM);
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
  assert(RegisterAliases[RegMIPS32::val][RegMIPS32::val]);                     \
  ScratchRegs[RegMIPS32::val] = scratch;
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

bool TargetMIPS32::doBranchOpt(Inst *I, const CfgNode *NextNode) {
  (void)I;
  (void)NextNode;
  UnimplementedError(Func->getContext()->getFlags());
  return false;
}

IceString TargetMIPS32::getRegName(SizeT RegNum, Type Ty) const {
  assert(RegNum < RegMIPS32::Reg_NUM);
  (void)Ty;
  static const char *RegNames[] = {
#define X(val, encode, name, scratch, preserved, stackptr, frameptr, isInt,    \
          isI64Pair, isFP32, isFP64, isVec128, alias_init)                     \
  name,
      REGMIPS32_TABLE
#undef X
  };
  return RegNames[RegNum];
}

Variable *TargetMIPS32::getPhysicalRegister(SizeT RegNum, Type Ty) {
  if (Ty == IceType_void)
    Ty = IceType_i32;
  if (PhysicalRegisters[Ty].empty())
    PhysicalRegisters[Ty].resize(RegMIPS32::Reg_NUM);
  assert(RegNum < PhysicalRegisters[Ty].size());
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
Variable *TargetMIPS32::legalizeToReg(Operand *From, int32_t RegNum) {
  return llvm::cast<Variable>(legalize(From, Legal_Reg, RegNum));
}

/// Legalize undef values to concrete values.
Operand *TargetMIPS32::legalizeUndef(Operand *From, int32_t RegNum) {
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

Variable *TargetMIPS32::makeReg(Type Type, int32_t RegNum) {
  // There aren't any 64-bit integer registers for Mips32.
  assert(Type != IceType_i64);
  Variable *Reg = Func->makeVariable(Type);
  if (RegNum == Variable::NoRegister)
    Reg->setMustHaveReg();
  else
    Reg->setRegNum(RegNum);
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
      int32_t RegLo = RegMIPS32::Reg_A0 + NumGPRRegsUsed;
      int32_t RegHi = RegLo + 1;
      ++NumGPRRegsUsed;
      // Always start i64 registers at an even register, so this may end
      // up padding away a register.
      if (RegLo % 2 != 0) {
        ++RegLo;
        ++NumGPRRegsUsed;
      }
      // If this leaves us without room to consume another register,
      // leave any previously speculatively consumed registers as consumed.
      if (NumGPRRegsUsed >= MIPS32_MAX_GPR_ARG)
        continue;
      // RegHi = RegMIPS32::Reg_A0 + NumGPRRegsUsed;
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
      int32_t RegNum = RegMIPS32::Reg_A0 + NumGPRRegsUsed;
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

void TargetMIPS32::lowerAlloca(const InstAlloca *Inst) {
  UsesFramePointer = true;
  // Conservatively require the stack to be aligned. Some stack adjustment
  // operations implemented below assume that the stack is aligned before the
  // alloca. All the alloca code ensures that the stack alignment is preserved
  // after the alloca. The stack alignment restriction can be relaxed in some
  // cases.
  NeedsStackAlignment = true;
  (void)Inst;
  UnimplementedError(Func->getContext()->getFlags());
}

void TargetMIPS32::lowerArithmetic(const InstArithmetic *Inst) {
  Variable *Dest = Inst->getDest();
  Operand *Src0 = legalizeUndef(Inst->getSrc(0));
  Operand *Src1 = legalizeUndef(Inst->getSrc(1));
  if (Dest->getType() == IceType_i64) {
    // TODO(reed kotler): fakedef needed for now until all cases are implemented
    auto *DestLo = llvm::cast<Variable>(loOperand(Dest));
    auto *DestHi = llvm::cast<Variable>(hiOperand(Dest));
    Context.insert<InstFakeDef>(DestLo);
    Context.insert<InstFakeDef>(DestHi);
    UnimplementedError(Func->getContext()->getFlags());
    return;
  }
  if (isVectorType(Dest->getType())) {
    Context.insert<InstFakeDef>(Dest);
    UnimplementedError(Func->getContext()->getFlags());
    return;
  }
  // Dest->getType() is non-i64 scalar
  Variable *T = makeReg(Dest->getType());
  Variable *Src0R = legalizeToReg(Src0);
  Variable *Src1R = legalizeToReg(Src1);
  switch (Inst->getOp()) {
  case InstArithmetic::_num:
    break;
  case InstArithmetic::Add:
    _add(T, Src0R, Src1R);
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
    _sub(T, Src0R, Src1R);
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
  // TODO(reed kotler):
  // fakedef and fakeuse needed for now until all cases are implemented
  Context.insert<InstFakeUse>(Src0R);
  Context.insert<InstFakeUse>(Src1R);
  Context.insert<InstFakeDef>(Dest);
  UnimplementedError(Func->getContext()->getFlags());
}

void TargetMIPS32::lowerAssign(const InstAssign *Inst) {
  Variable *Dest = Inst->getDest();
  Operand *Src0 = Inst->getSrc(0);
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
      UnimplementedError(Func->getContext()->getFlags());
    } else {
      _mov(Dest, SrcR);
    }
  }
}

void TargetMIPS32::lowerBr(const InstBr *Inst) {
  (void)Inst;
  UnimplementedError(Func->getContext()->getFlags());
}

void TargetMIPS32::lowerCall(const InstCall *Inst) {
  (void)Inst;
  UnimplementedError(Func->getContext()->getFlags());
}

void TargetMIPS32::lowerCast(const InstCast *Inst) {
  InstCast::OpKind CastKind = Inst->getCastKind();
  switch (CastKind) {
  default:
    Func->setError("Cast type not supported");
    return;
  case InstCast::Sext: {
    UnimplementedError(Func->getContext()->getFlags());
    break;
  }
  case InstCast::Zext: {
    UnimplementedError(Func->getContext()->getFlags());
    break;
  }
  case InstCast::Trunc: {
    UnimplementedError(Func->getContext()->getFlags());
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
    UnimplementedError(Func->getContext()->getFlags());
    break;
  }
  }
}

void TargetMIPS32::lowerExtractElement(const InstExtractElement *Inst) {
  (void)Inst;
  UnimplementedError(Func->getContext()->getFlags());
}

void TargetMIPS32::lowerFcmp(const InstFcmp *Inst) {
  (void)Inst;
  UnimplementedError(Func->getContext()->getFlags());
}

void TargetMIPS32::lowerIcmp(const InstIcmp *Inst) {
  (void)Inst;
  UnimplementedError(Func->getContext()->getFlags());
}

void TargetMIPS32::lowerInsertElement(const InstInsertElement *Inst) {
  (void)Inst;
  UnimplementedError(Func->getContext()->getFlags());
}

void TargetMIPS32::lowerIntrinsicCall(const InstIntrinsicCall *Instr) {
  switch (Instr->getIntrinsicInfo().ID) {
  case Intrinsics::AtomicCmpxchg: {
    UnimplementedError(Func->getContext()->getFlags());
    return;
  }
  case Intrinsics::AtomicFence:
    UnimplementedError(Func->getContext()->getFlags());
    return;
  case Intrinsics::AtomicFenceAll:
    // NOTE: FenceAll should prevent and load/store from being moved across the
    // fence (both atomic and non-atomic). The InstMIPS32Mfence instruction is
    // currently marked coarsely as "HasSideEffects".
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
    UnimplementedError(Func->getContext()->getFlags());
    return;
  }
  case Intrinsics::Ctpop: {
    UnimplementedError(Func->getContext()->getFlags());
    return;
  }
  case Intrinsics::Ctlz: {
    UnimplementedError(Func->getContext()->getFlags());
    return;
  }
  case Intrinsics::Cttz: {
    UnimplementedError(Func->getContext()->getFlags());
    return;
  }
  case Intrinsics::Fabs: {
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
    UnimplementedError(Func->getContext()->getFlags());
    return;
  }
  case Intrinsics::Stacksave: {
    UnimplementedError(Func->getContext()->getFlags());
    return;
  }
  case Intrinsics::Stackrestore: {
    UnimplementedError(Func->getContext()->getFlags());
    return;
  }
  case Intrinsics::Trap:
    UnimplementedError(Func->getContext()->getFlags());
    return;
  case Intrinsics::UnknownIntrinsic:
    Func->setError("Should not be lowering UnknownIntrinsic");
    return;
  }
  return;
}

void TargetMIPS32::lowerLoad(const InstLoad *Inst) {
  (void)Inst;
  UnimplementedError(Func->getContext()->getFlags());
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

void TargetMIPS32::lowerPhi(const InstPhi * /*Inst*/) {
  Func->setError("Phi found in regular instruction list");
}

void TargetMIPS32::lowerRet(const InstRet *Inst) {
  Variable *Reg = nullptr;
  if (Inst->hasRetValue()) {
    Operand *Src0 = Inst->getRetValue();
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
      UnimplementedError(Func->getContext()->getFlags());
    }
  }
  _ret(getPhysicalRegister(RegMIPS32::Reg_RA), Reg);
}

void TargetMIPS32::lowerSelect(const InstSelect *Inst) {
  (void)Inst;
  UnimplementedError(Func->getContext()->getFlags());
}

void TargetMIPS32::lowerStore(const InstStore *Inst) {
  (void)Inst;
  UnimplementedError(Func->getContext()->getFlags());
}

void TargetMIPS32::doAddressOptStore() {
  UnimplementedError(Func->getContext()->getFlags());
}

void TargetMIPS32::lowerSwitch(const InstSwitch *Inst) {
  (void)Inst;
  UnimplementedError(Func->getContext()->getFlags());
}

void TargetMIPS32::lowerUnreachable(const InstUnreachable * /*Inst*/) {
  UnimplementedError(Func->getContext()->getFlags());
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
  // Find two-address non-SSA instructions where Dest==Src0, and set the
  // IsDestRedefined flag to keep liveness analysis consistent.
  UnimplementedError(Func->getContext()->getFlags());
}

void TargetMIPS32::makeRandomRegisterPermutation(
    llvm::SmallVectorImpl<int32_t> &Permutation,
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
  switch (Ctx->getFlags().getOutFileType()) {
  case FT_Elf: {
    ELFObjectWriter *Writer = Ctx->getObjectWriter();
    Writer->writeDataSection(Vars, llvm::ELF::R_MIPS_GLOB_DAT, SectionSuffix);
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
Variable *TargetMIPS32::copyToReg(Operand *Src, int32_t RegNum) {
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
                                int32_t RegNum) {
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
    uint32_t Value = static_cast<uint32_t>(C32->getValue());
    // Check if the immediate will fit in a Flexible second operand,
    // if a Flexible second operand is allowed. We need to know the exact
    // value, so that rules out relocatable constants.
    // Also try the inverse and use MVN if possible.
    // Do a movw/movt to a register.
    Variable *Reg;
    if (RegNum == Variable::NoRegister)
      Reg = makeReg(Ty, RegNum);
    else
      Reg = getPhysicalRegister(RegNum);
    if (isInt<16>(int32_t(Value))) {
      _addiu(Reg, getPhysicalRegister(RegMIPS32::Reg_ZERO, Ty), Value);
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
        (RegNum != Variable::NoRegister && RegNum != Var->getRegNum())) {
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

llvm::SmallBitVector TargetMIPS32::TypeToRegisterSet[IceType_NUM];
llvm::SmallBitVector TargetMIPS32::RegisterAliases[RegMIPS32::Reg_NUM];
llvm::SmallBitVector TargetMIPS32::ScratchRegs;

} // end of namespace Ice
