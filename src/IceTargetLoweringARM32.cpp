//===- subzero/src/IceTargetLoweringARM32.cpp - ARM32 lowering ------------===//
//
//                        The Subzero Code Generator
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements the TargetLoweringARM32 class, which consists almost
// entirely of the lowering sequence for each high-level instruction.
//
//===----------------------------------------------------------------------===//

#include "llvm/Support/MathExtras.h"

#include "IceCfg.h"
#include "IceCfgNode.h"
#include "IceClFlags.h"
#include "IceDefs.h"
#include "IceELFObjectWriter.h"
#include "IceGlobalInits.h"
#include "IceInstARM32.h"
#include "IceLiveness.h"
#include "IceOperand.h"
#include "IceRegistersARM32.h"
#include "IceTargetLoweringARM32.def"
#include "IceTargetLoweringARM32.h"
#include "IceUtils.h"

namespace Ice {

TargetARM32::TargetARM32(Cfg *Func)
    : TargetLowering(Func), UsesFramePointer(false) {
  // TODO: Don't initialize IntegerRegisters and friends every time.
  // Instead, initialize in some sort of static initializer for the
  // class.
  llvm::SmallBitVector IntegerRegisters(RegARM32::Reg_NUM);
  llvm::SmallBitVector FloatRegisters(RegARM32::Reg_NUM);
  llvm::SmallBitVector VectorRegisters(RegARM32::Reg_NUM);
  llvm::SmallBitVector InvalidRegisters(RegARM32::Reg_NUM);
  ScratchRegs.resize(RegARM32::Reg_NUM);
#define X(val, encode, name, scratch, preserved, stackptr, frameptr, isInt,    \
          isFP)                                                                \
  IntegerRegisters[RegARM32::val] = isInt;                                     \
  FloatRegisters[RegARM32::val] = isFP;                                        \
  VectorRegisters[RegARM32::val] = isFP;                                       \
  ScratchRegs[RegARM32::val] = scratch;
  REGARM32_TABLE;
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

  // Nop insertion
  if (Ctx->getFlags().shouldDoNopInsertion()) {
    Func->doNopInsertion();
  }
}

bool TargetARM32::doBranchOpt(Inst *I, const CfgNode *NextNode) {
  (void)I;
  (void)NextNode;
  llvm::report_fatal_error("Not yet implemented");
}

IceString TargetARM32::RegNames[] = {
#define X(val, encode, name, scratch, preserved, stackptr, frameptr, isInt,    \
          isFP)                                                                \
  name,
    REGARM32_TABLE
#undef X
};

IceString TargetARM32::getRegName(SizeT RegNum, Type Ty) const {
  assert(RegNum < RegARM32::Reg_NUM);
  (void)Ty;
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
    // Specially mark SP as an "argument" so that it is considered
    // live upon function entry.
    if (RegNum == RegARM32::Reg_sp) {
      Func->addImplicitArg(Reg);
      Reg->setIgnoreLiveness();
    }
  }
  return Reg;
}

void TargetARM32::emitVariable(const Variable *Var) const {
  Ostream &Str = Ctx->getStrEmit();
  (void)Var;
  (void)Str;
  llvm::report_fatal_error("emitVariable: Not yet implemented");
}

void TargetARM32::lowerArguments() {
  llvm::report_fatal_error("lowerArguments: Not yet implemented");
}

Type TargetARM32::stackSlotType() { return IceType_i32; }

void TargetARM32::addProlog(CfgNode *Node) {
  (void)Node;
  llvm::report_fatal_error("addProlog: Not yet implemented");
}

void TargetARM32::addEpilog(CfgNode *Node) {
  (void)Node;
  llvm::report_fatal_error("addEpilog: Not yet implemented");
}

llvm::SmallBitVector TargetARM32::getRegisterSet(RegSetMask Include,
                                                 RegSetMask Exclude) const {
  llvm::SmallBitVector Registers(RegARM32::Reg_NUM);

#define X(val, encode, name, scratch, preserved, stackptr, frameptr, isInt,    \
          isFP)                                                                \
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
  (void)Inst;
  llvm::report_fatal_error("Not yet implemented");
}

void TargetARM32::lowerArithmetic(const InstArithmetic *Inst) {
  switch (Inst->getOp()) {
  case InstArithmetic::_num:
    llvm_unreachable("Unknown arithmetic operator");
    break;
  case InstArithmetic::Add:
    llvm::report_fatal_error("Not yet implemented");
    break;
  case InstArithmetic::And:
    llvm::report_fatal_error("Not yet implemented");
    break;
  case InstArithmetic::Or:
    llvm::report_fatal_error("Not yet implemented");
    break;
  case InstArithmetic::Xor:
    llvm::report_fatal_error("Not yet implemented");
    break;
  case InstArithmetic::Sub:
    llvm::report_fatal_error("Not yet implemented");
    break;
  case InstArithmetic::Mul:
    llvm::report_fatal_error("Not yet implemented");
    break;
  case InstArithmetic::Shl:
    llvm::report_fatal_error("Not yet implemented");
    break;
  case InstArithmetic::Lshr:
    llvm::report_fatal_error("Not yet implemented");
    break;
  case InstArithmetic::Ashr:
    llvm::report_fatal_error("Not yet implemented");
    break;
  case InstArithmetic::Udiv:
    llvm::report_fatal_error("Not yet implemented");
    break;
  case InstArithmetic::Sdiv:
    llvm::report_fatal_error("Not yet implemented");
    break;
  case InstArithmetic::Urem:
    llvm::report_fatal_error("Not yet implemented");
    break;
  case InstArithmetic::Srem:
    llvm::report_fatal_error("Not yet implemented");
    break;
  case InstArithmetic::Fadd:
    llvm::report_fatal_error("Not yet implemented");
    break;
  case InstArithmetic::Fsub:
    llvm::report_fatal_error("Not yet implemented");
    break;
  case InstArithmetic::Fmul:
    llvm::report_fatal_error("Not yet implemented");
    break;
  case InstArithmetic::Fdiv:
    llvm::report_fatal_error("Not yet implemented");
    break;
  case InstArithmetic::Frem:
    llvm::report_fatal_error("Not yet implemented");
    break;
  }
}

void TargetARM32::lowerAssign(const InstAssign *Inst) {
  (void)Inst;
  llvm::report_fatal_error("Not yet implemented");
}

void TargetARM32::lowerBr(const InstBr *Inst) {
  (void)Inst;
  llvm::report_fatal_error("Not yet implemented");
}

void TargetARM32::lowerCall(const InstCall *Inst) {
  (void)Inst;
  llvm::report_fatal_error("Not yet implemented");
}

void TargetARM32::lowerCast(const InstCast *Inst) {
  InstCast::OpKind CastKind = Inst->getCastKind();
  switch (CastKind) {
  default:
    Func->setError("Cast type not supported");
    return;
  case InstCast::Sext: {
    llvm::report_fatal_error("Not yet implemented");
    break;
  }
  case InstCast::Zext: {
    llvm::report_fatal_error("Not yet implemented");
    break;
  }
  case InstCast::Trunc: {
    llvm::report_fatal_error("Not yet implemented");
    break;
  }
  case InstCast::Fptrunc:
    llvm::report_fatal_error("Not yet implemented");
    break;
  case InstCast::Fpext: {
    llvm::report_fatal_error("Not yet implemented");
    break;
  }
  case InstCast::Fptosi:
    llvm::report_fatal_error("Not yet implemented");
    break;
  case InstCast::Fptoui:
    llvm::report_fatal_error("Not yet implemented");
    break;
  case InstCast::Sitofp:
    llvm::report_fatal_error("Not yet implemented");
    break;
  case InstCast::Uitofp: {
    llvm::report_fatal_error("Not yet implemented");
    break;
  }
  case InstCast::Bitcast: {
    llvm::report_fatal_error("Not yet implemented");
    break;
  }
  }
}

void TargetARM32::lowerExtractElement(const InstExtractElement *Inst) {
  (void)Inst;
  llvm::report_fatal_error("Not yet implemented");
}

void TargetARM32::lowerFcmp(const InstFcmp *Inst) {
  (void)Inst;
  llvm::report_fatal_error("Not yet implemented");
}

void TargetARM32::lowerIcmp(const InstIcmp *Inst) {
  (void)Inst;
  llvm::report_fatal_error("Not yet implemented");
}

void TargetARM32::lowerInsertElement(const InstInsertElement *Inst) {
  (void)Inst;
  llvm::report_fatal_error("Not yet implemented");
}

void TargetARM32::lowerIntrinsicCall(const InstIntrinsicCall *Instr) {
  switch (Intrinsics::IntrinsicID ID = Instr->getIntrinsicInfo().ID) {
  case Intrinsics::AtomicCmpxchg: {
    llvm::report_fatal_error("Not yet implemented");
    return;
  }
  case Intrinsics::AtomicFence:
    llvm::report_fatal_error("Not yet implemented");
    return;
  case Intrinsics::AtomicFenceAll:
    // NOTE: FenceAll should prevent and load/store from being moved
    // across the fence (both atomic and non-atomic). The InstARM32Mfence
    // instruction is currently marked coarsely as "HasSideEffects".
    llvm::report_fatal_error("Not yet implemented");
    return;
  case Intrinsics::AtomicIsLockFree: {
    llvm::report_fatal_error("Not yet implemented");
    return;
  }
  case Intrinsics::AtomicLoad: {
    llvm::report_fatal_error("Not yet implemented");
    return;
  }
  case Intrinsics::AtomicRMW:
    llvm::report_fatal_error("Not yet implemented");
    return;
  case Intrinsics::AtomicStore: {
    llvm::report_fatal_error("Not yet implemented");
    return;
  }
  case Intrinsics::Bswap: {
    llvm::report_fatal_error("Not yet implemented");
    return;
  }
  case Intrinsics::Ctpop: {
    llvm::report_fatal_error("Not yet implemented");
    return;
  }
  case Intrinsics::Ctlz: {
    llvm::report_fatal_error("Not yet implemented");
    return;
  }
  case Intrinsics::Cttz: {
    llvm::report_fatal_error("Not yet implemented");
    return;
  }
  case Intrinsics::Fabs: {
    llvm::report_fatal_error("Not yet implemented");
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
    // The value operand needs to be extended to a stack slot size
    // because the PNaCl ABI requires arguments to be at least 32 bits
    // wide.
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
      llvm::report_fatal_error("Not yet implemented");
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
    llvm::report_fatal_error("Not yet implemented");
    return;
  }
  case Intrinsics::Stacksave: {
    llvm::report_fatal_error("Not yet implemented");
    return;
  }
  case Intrinsics::Stackrestore: {
    llvm::report_fatal_error("Not yet implemented");
    return;
  }
  case Intrinsics::Trap:
    llvm::report_fatal_error("Not yet implemented");
    return;
  case Intrinsics::UnknownIntrinsic:
    Func->setError("Should not be lowering UnknownIntrinsic");
    return;
  }
  return;
}

void TargetARM32::lowerLoad(const InstLoad *Inst) {
  (void)Inst;
  llvm::report_fatal_error("Not yet implemented");
}

void TargetARM32::doAddressOptLoad() {
  llvm::report_fatal_error("Not yet implemented");
}

void TargetARM32::randomlyInsertNop(float Probability) {
  RandomNumberGeneratorWrapper RNG(Ctx->getRNG());
  if (RNG.getTrueWithProbability(Probability)) {
    llvm::report_fatal_error("Not yet implemented");
  }
}

void TargetARM32::lowerPhi(const InstPhi * /*Inst*/) {
  Func->setError("Phi found in regular instruction list");
}

void TargetARM32::lowerRet(const InstRet *Inst) {
  (void)Inst;
  llvm::report_fatal_error("Not yet implemented");
}

void TargetARM32::lowerSelect(const InstSelect *Inst) {
  (void)Inst;
  llvm::report_fatal_error("Not yet implemented");
}

void TargetARM32::lowerStore(const InstStore *Inst) {
  (void)Inst;
  llvm::report_fatal_error("Not yet implemented");
}

void TargetARM32::doAddressOptStore() {
  llvm::report_fatal_error("Not yet implemented");
}

void TargetARM32::lowerSwitch(const InstSwitch *Inst) {
  (void)Inst;
  llvm::report_fatal_error("Not yet implemented");
}

void TargetARM32::lowerUnreachable(const InstUnreachable * /*Inst*/) {
  llvm_unreachable("Not yet implemented");
}

// Turn an i64 Phi instruction into a pair of i32 Phi instructions, to
// preserve integrity of liveness analysis.  Undef values are also
// turned into zeroes, since loOperand() and hiOperand() don't expect
// Undef input.
void TargetARM32::prelowerPhis() {
  llvm::report_fatal_error("Not yet implemented");
}

// Lower the pre-ordered list of assignments into mov instructions.
// Also has to do some ad-hoc register allocation as necessary.
void TargetARM32::lowerPhiAssignments(CfgNode *Node,
                                      const AssignList &Assignments) {
  (void)Node;
  (void)Assignments;
  llvm::report_fatal_error("Not yet implemented");
}

void TargetARM32::postLower() {
  if (Ctx->getFlags().getOptLevel() == Opt_m1)
    return;
  // Find two-address non-SSA instructions where Dest==Src0, and set
  // the DestNonKillable flag to keep liveness analysis consistent.
  llvm::report_fatal_error("Not yet implemented");
}

void TargetARM32::makeRandomRegisterPermutation(
    llvm::SmallVectorImpl<int32_t> &Permutation,
    const llvm::SmallBitVector &ExcludeRegisters) const {
  (void)Permutation;
  (void)ExcludeRegisters;
  llvm::report_fatal_error("Not yet implemented");
}

/* TODO(jvoung): avoid duplicate symbols with multiple targets.
void ConstantUndef::emitWithoutDollar(GlobalContext *) const {
  llvm_unreachable("Not expecting to emitWithoutDollar undef");
}

void ConstantUndef::emit(GlobalContext *) const {
  llvm_unreachable("undef value encountered by emitter.");
}
*/

TargetDataARM32::TargetDataARM32(GlobalContext *Ctx)
    : TargetDataLowering(Ctx) {}

void TargetDataARM32::lowerGlobal(const VariableDeclaration &Var) const {
  (void)Var;
  llvm::report_fatal_error("Not yet implemented");
}

void TargetDataARM32::lowerGlobals(
    std::unique_ptr<VariableDeclarationList> Vars) const {
  switch (Ctx->getFlags().getOutFileType()) {
  case FT_Elf: {
    ELFObjectWriter *Writer = Ctx->getObjectWriter();
    Writer->writeDataSection(*Vars, llvm::ELF::R_ARM_ABS32);
  } break;
  case FT_Asm:
  case FT_Iasm: {
    const IceString &TranslateOnly = Ctx->getFlags().getTranslateOnly();
    OstreamLocker L(Ctx);
    for (const VariableDeclaration *Var : *Vars) {
      if (GlobalContext::matchSymbolName(Var->getName(), TranslateOnly)) {
        lowerGlobal(*Var);
      }
    }
  } break;
  }
}

void TargetDataARM32::lowerConstants() const {
  if (Ctx->getFlags().getDisableTranslation())
    return;
  llvm::report_fatal_error("Not yet implemented");
}

} // end of namespace Ice
