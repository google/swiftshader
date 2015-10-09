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
/// This file implements the TargetLoweringMIPS32 class, which consists almost
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
#include "IceRegistersMIPS32.h"
#include "IceTargetLoweringMIPS32.def"
#include "IceUtils.h"
#include "llvm/Support/MathExtras.h"

namespace Ice {

TargetMIPS32::TargetMIPS32(Cfg *Func) : TargetLowering(Func) {
  // TODO: Don't initialize IntegerRegisters and friends every time. Instead,
  // initialize in some sort of static initializer for the class.
  llvm::SmallBitVector IntegerRegisters(RegMIPS32::Reg_NUM);
  llvm::SmallBitVector FloatRegisters(RegMIPS32::Reg_NUM);
  llvm::SmallBitVector VectorRegisters(RegMIPS32::Reg_NUM);
  llvm::SmallBitVector InvalidRegisters(RegMIPS32::Reg_NUM);
  ScratchRegs.resize(RegMIPS32::Reg_NUM);
#define X(val, encode, name, scratch, preserved, stackptr, frameptr, isInt,    \
          isFP)                                                                \
  IntegerRegisters[RegMIPS32::val] = isInt;                                    \
  FloatRegisters[RegMIPS32::val] = isFP;                                       \
  VectorRegisters[RegMIPS32::val] = isFP;                                      \
  RegisterAliases[RegMIPS32::val].resize(RegMIPS32::Reg_NUM);                  \
  RegisterAliases[RegMIPS32::val].set(RegMIPS32::val);                         \
  ScratchRegs[RegMIPS32::val] = scratch;
  REGMIPS32_TABLE;
#undef X
  TypeToRegisterSet[IceType_void] = InvalidRegisters;
  TypeToRegisterSet[IceType_i1] = IntegerRegisters;
  TypeToRegisterSet[IceType_i8] = IntegerRegisters;
  TypeToRegisterSet[IceType_i16] = IntegerRegisters;
  TypeToRegisterSet[IceType_i32] = IntegerRegisters;
  TypeToRegisterSet[IceType_i64] = IntegerRegisters;
  TypeToRegisterSet[IceType_f32] = FloatRegisters;
  TypeToRegisterSet[IceType_f64] = FloatRegisters;
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
          isFP)                                                                \
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

void TargetMIPS32::emitVariable(const Variable *Var) const {
  if (!BuildDefs::dump())
    return;
  Ostream &Str = Ctx->getStrEmit();
  (void)Var;
  (void)Str;
  UnimplementedError(Func->getContext()->getFlags());
}

void TargetMIPS32::lowerArguments() {
  VarList &Args = Func->getArgs();
  if (Args.size() > 0)
    UnimplementedError(Func->getContext()->getFlags());
}

Type TargetMIPS32::stackSlotType() { return IceType_i32; }

void TargetMIPS32::addProlog(CfgNode *Node) {
  (void)Node;
  UnimplementedError(Func->getContext()->getFlags());
}

void TargetMIPS32::addEpilog(CfgNode *Node) {
  (void)Node;
  UnimplementedError(Func->getContext()->getFlags());
}

llvm::SmallBitVector TargetMIPS32::getRegisterSet(RegSetMask Include,
                                                  RegSetMask Exclude) const {
  llvm::SmallBitVector Registers(RegMIPS32::Reg_NUM);

#define X(val, encode, name, scratch, preserved, stackptr, frameptr, isInt,    \
          isFP)                                                                \
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
  switch (Inst->getOp()) {
  case InstArithmetic::_num:
    UnimplementedError(Func->getContext()->getFlags());
    break;
  case InstArithmetic::Add:
    UnimplementedError(Func->getContext()->getFlags());
    break;
  case InstArithmetic::And:
    UnimplementedError(Func->getContext()->getFlags());
    break;
  case InstArithmetic::Or:
    UnimplementedError(Func->getContext()->getFlags());
    break;
  case InstArithmetic::Xor:
    UnimplementedError(Func->getContext()->getFlags());
    break;
  case InstArithmetic::Sub:
    UnimplementedError(Func->getContext()->getFlags());
    break;
  case InstArithmetic::Mul:
    UnimplementedError(Func->getContext()->getFlags());
    break;
  case InstArithmetic::Shl:
    UnimplementedError(Func->getContext()->getFlags());
    break;
  case InstArithmetic::Lshr:
    UnimplementedError(Func->getContext()->getFlags());
    break;
  case InstArithmetic::Ashr:
    UnimplementedError(Func->getContext()->getFlags());
    break;
  case InstArithmetic::Udiv:
    UnimplementedError(Func->getContext()->getFlags());
    break;
  case InstArithmetic::Sdiv:
    UnimplementedError(Func->getContext()->getFlags());
    break;
  case InstArithmetic::Urem:
    UnimplementedError(Func->getContext()->getFlags());
    break;
  case InstArithmetic::Srem:
    UnimplementedError(Func->getContext()->getFlags());
    break;
  case InstArithmetic::Fadd:
    UnimplementedError(Func->getContext()->getFlags());
    break;
  case InstArithmetic::Fsub:
    UnimplementedError(Func->getContext()->getFlags());
    break;
  case InstArithmetic::Fmul:
    UnimplementedError(Func->getContext()->getFlags());
    break;
  case InstArithmetic::Fdiv:
    UnimplementedError(Func->getContext()->getFlags());
    break;
  case InstArithmetic::Frem:
    UnimplementedError(Func->getContext()->getFlags());
    break;
  }
}

void TargetMIPS32::lowerAssign(const InstAssign *Inst) {
  (void)Inst;
  UnimplementedError(Func->getContext()->getFlags());
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
  if (Inst->hasRetValue())
    UnimplementedError(Func->getContext()->getFlags());
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
  UnimplementedError(Func->getContext()->getFlags());
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

TargetHeaderMIPS32::TargetHeaderMIPS32(GlobalContext *Ctx)
    : TargetHeaderLowering(Ctx) {}

} // end of namespace Ice
