//===- subzero/src/IceTargetLoweringX86Base.h - x86 lowering ----*- C++ -*-===//
//
//                        The Subzero Code Generator
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file declares the TargetLoweringX86 template class, which
/// implements the TargetLowering base interface for the x86
/// architecture.
///
//===----------------------------------------------------------------------===//

#ifndef SUBZERO_SRC_ICETARGETLOWERINGX86BASE_H
#define SUBZERO_SRC_ICETARGETLOWERINGX86BASE_H

#include "IceDefs.h"
#include "IceInst.h"
#include "IceSwitchLowering.h"
#include "IceTargetLowering.h"

#include <type_traits>
#include <unordered_map>

namespace Ice {
namespace X86Internal {

template <class MachineTraits> class BoolFolding;

template <class Machine> struct MachineTraits {};

template <class Machine> class TargetX86Base : public Machine {
  static_assert(std::is_base_of<::Ice::TargetLowering, Machine>::value,
                "Machine template parameter must be a TargetLowering.");

  TargetX86Base() = delete;
  TargetX86Base(const TargetX86Base &) = delete;
  TargetX86Base &operator=(const TargetX86Base &) = delete;

protected:
  using Machine::H_bitcast_16xi1_i16;
  using Machine::H_bitcast_8xi1_i8;
  using Machine::H_bitcast_i16_16xi1;
  using Machine::H_bitcast_i8_8xi1;
  using Machine::H_call_ctpop_i32;
  using Machine::H_call_ctpop_i64;
  using Machine::H_call_longjmp;
  using Machine::H_call_memcpy;
  using Machine::H_call_memmove;
  using Machine::H_call_memset;
  using Machine::H_call_read_tp;
  using Machine::H_call_setjmp;
  using Machine::H_fptosi_f32_i64;
  using Machine::H_fptosi_f64_i64;
  using Machine::H_fptoui_4xi32_f32;
  using Machine::H_fptoui_f32_i32;
  using Machine::H_fptoui_f32_i64;
  using Machine::H_fptoui_f64_i32;
  using Machine::H_fptoui_f64_i64;
  using Machine::H_frem_f32;
  using Machine::H_frem_f64;
  using Machine::H_sdiv_i64;
  using Machine::H_sitofp_i64_f32;
  using Machine::H_sitofp_i64_f64;
  using Machine::H_srem_i64;
  using Machine::H_udiv_i64;
  using Machine::H_uitofp_4xi32_4xf32;
  using Machine::H_uitofp_i32_f32;
  using Machine::H_uitofp_i32_f64;
  using Machine::H_uitofp_i64_f32;
  using Machine::H_uitofp_i64_f64;
  using Machine::H_urem_i64;

  using Machine::alignStackSpillAreas;
  using Machine::assignVarStackSlots;
  using Machine::inferTwoAddress;
  using Machine::makeHelperCall;
  using Machine::getVarStackSlotParams;

public:
  using Traits = MachineTraits<Machine>;
  using BoolFolding = ::Ice::X86Internal::BoolFolding<Traits>;

  using Machine::RegSet_All;
  using Machine::RegSet_CalleeSave;
  using Machine::RegSet_CallerSave;
  using Machine::RegSet_FramePointer;
  using Machine::RegSet_None;
  using Machine::RegSet_StackPointer;
  using Machine::Context;
  using Machine::Ctx;
  using Machine::Func;
  using RegSetMask = typename Machine::RegSetMask;

  using Machine::_bundle_lock;
  using Machine::_bundle_unlock;
  using Machine::_set_dest_nonkillable;
  using Machine::getContext;
  using Machine::getStackAdjustment;
  using Machine::regAlloc;
  using Machine::resetStackAdjustment;

  static TargetX86Base *create(Cfg *Func) { return new TargetX86Base(Func); }

  void translateOm1() override;
  void translateO2() override;
  void doLoadOpt();
  bool doBranchOpt(Inst *I, const CfgNode *NextNode) override;

  SizeT getNumRegisters() const override {
    return Traits::RegisterSet::Reg_NUM;
  }
  Variable *getPhysicalRegister(SizeT RegNum, Type Ty = IceType_void) override;
  IceString getRegName(SizeT RegNum, Type Ty) const override;
  llvm::SmallBitVector getRegisterSet(RegSetMask Include,
                                      RegSetMask Exclude) const override;
  const llvm::SmallBitVector &getRegisterSetForType(Type Ty) const override {
    return TypeToRegisterSet[Ty];
  }
  bool hasFramePointer() const override { return IsEbpBasedFrame; }
  SizeT getFrameOrStackReg() const override {
    return IsEbpBasedFrame ? Traits::RegisterSet::Reg_ebp
                           : Traits::RegisterSet::Reg_esp;
  }
  size_t typeWidthInBytesOnStack(Type Ty) const override {
    // Round up to the next multiple of 4 bytes.  In particular, i1,
    // i8, and i16 are rounded up to 4 bytes.
    return (typeWidthInBytes(Ty) + 3) & ~3;
  }

  SizeT getMinJumpTableSize() const override { return 4; }

  void emitVariable(const Variable *Var) const override;

  const char *getConstantPrefix() const final { return "$"; }
  void emit(const ConstantUndef *C) const final;
  void emit(const ConstantInteger32 *C) const final;
  void emit(const ConstantInteger64 *C) const final;
  void emit(const ConstantFloat *C) const final;
  void emit(const ConstantDouble *C) const final;

  void lowerArguments() override;
  void initNodeForLowering(CfgNode *Node) override;
  void addProlog(CfgNode *Node) override;
  void addEpilog(CfgNode *Node) override;
  /// Ensure that a 64-bit Variable has been split into 2 32-bit
  /// Variables, creating them if necessary.  This is needed for all
  /// I64 operations, and it is needed for pushing F64 arguments for
  /// function calls using the 32-bit push instruction (though the
  /// latter could be done by directly writing to the stack).
  void split64(Variable *Var);
  Operand *loOperand(Operand *Operand);
  Operand *hiOperand(Operand *Operand);
  void finishArgumentLowering(Variable *Arg, Variable *FramePtr,
                              size_t BasicFrameOffset, size_t &InArgsSizeBytes);
  typename Traits::Address
  stackVarToAsmOperand(const Variable *Var) const final;

  typename Traits::InstructionSet getInstructionSet() const final {
    return InstructionSet;
  }
  Operand *legalizeUndef(Operand *From, int32_t RegNum = Variable::NoRegister);

protected:
  explicit TargetX86Base(Cfg *Func);

  void postLower() override;

  void lowerAlloca(const InstAlloca *Inst) override;
  void lowerArithmetic(const InstArithmetic *Inst) override;
  void lowerAssign(const InstAssign *Inst) override;
  void lowerBr(const InstBr *Inst) override;
  void lowerCall(const InstCall *Inst) override;
  void lowerCast(const InstCast *Inst) override;
  void lowerExtractElement(const InstExtractElement *Inst) override;
  void lowerFcmp(const InstFcmp *Inst) override;
  void lowerIcmp(const InstIcmp *Inst) override;
  void lowerIntrinsicCall(const InstIntrinsicCall *Inst) override;
  void lowerInsertElement(const InstInsertElement *Inst) override;
  void lowerLoad(const InstLoad *Inst) override;
  void lowerPhi(const InstPhi *Inst) override;
  void lowerRet(const InstRet *Inst) override;
  void lowerSelect(const InstSelect *Inst) override;
  void lowerStore(const InstStore *Inst) override;
  void lowerSwitch(const InstSwitch *Inst) override;
  void lowerUnreachable(const InstUnreachable *Inst) override;
  void lowerOther(const Inst *Instr) override;
  void lowerRMW(const typename Traits::Insts::FakeRMW *RMW);
  void prelowerPhis() override;
  void lowerPhiAssignments(CfgNode *Node,
                           const AssignList &Assignments) override;
  void doAddressOptLoad() override;
  void doAddressOptStore() override;
  void randomlyInsertNop(float Probability) override;

  /// Naive lowering of cmpxchg.
  void lowerAtomicCmpxchg(Variable *DestPrev, Operand *Ptr, Operand *Expected,
                          Operand *Desired);
  /// Attempt a more optimized lowering of cmpxchg. Returns true if optimized.
  bool tryOptimizedCmpxchgCmpBr(Variable *DestPrev, Operand *Ptr,
                                Operand *Expected, Operand *Desired);
  void lowerAtomicRMW(Variable *Dest, uint32_t Operation, Operand *Ptr,
                      Operand *Val);
  void lowerCountZeros(bool Cttz, Type Ty, Variable *Dest, Operand *FirstVal,
                       Operand *SecondVal);

  /// Check the comparison is in [Min,Max]. The flags register will be modified
  /// with:
  ///   - below equal, if in range
  ///   - above, set if not in range
  /// The index into the range is returned.
  Operand *lowerCmpRange(Operand *Comparison, uint64_t Min, uint64_t Max);
  /// Lowering of a cluster of switch cases. If the case is not matched control
  /// will pass to the default label provided. If the default label is nullptr
  /// then control will fall through to the next instruction. DoneCmp should be
  /// true if the flags contain the result of a comparison with the Comparison.
  void lowerCaseCluster(const CaseCluster &Case, Operand *Src0, bool DoneCmp,
                        CfgNode *DefaultLabel = nullptr);

  typedef void (TargetX86Base::*LowerBinOp)(Variable *, Operand *);
  void expandAtomicRMWAsCmpxchg(LowerBinOp op_lo, LowerBinOp op_hi,
                                Variable *Dest, Operand *Ptr, Operand *Val);

  void eliminateNextVectorSextInstruction(Variable *SignExtendedResult);

  void scalarizeArithmetic(InstArithmetic::OpKind K, Variable *Dest,
                           Operand *Src0, Operand *Src1);

  /// Operand legalization helpers.  To deal with address mode
  /// constraints, the helpers will create a new Operand and emit
  /// instructions that guarantee that the Operand kind is one of those
  /// indicated by the LegalMask (a bitmask of allowed kinds).  If the
  /// input Operand is known to already meet the constraints, it may be
  /// simply returned as the result, without creating any new
  /// instructions or operands.
  enum OperandLegalization {
    Legal_None = 0,
    Legal_Reg = 1 << 0, // physical register, not stack location
    Legal_Imm = 1 << 1,
    Legal_Mem = 1 << 2, // includes [eax+4*ecx] as well as [esp+12]
    Legal_All = ~Legal_None
  };
  typedef uint32_t LegalMask;
  Operand *legalize(Operand *From, LegalMask Allowed = Legal_All,
                    int32_t RegNum = Variable::NoRegister);
  Variable *legalizeToReg(Operand *From, int32_t RegNum = Variable::NoRegister);
  /// Legalize the first source operand for use in the cmp instruction.
  Operand *legalizeSrc0ForCmp(Operand *Src0, Operand *Src1);
  /// Turn a pointer operand into a memory operand that can be
  /// used by a real load/store operation. Legalizes the operand as well.
  /// This is a nop if the operand is already a legal memory operand.
  typename Traits::X86OperandMem *formMemoryOperand(Operand *Ptr, Type Ty,
                                                    bool DoLegalize = true);

  Variable *makeReg(Type Ty, int32_t RegNum = Variable::NoRegister);
  static Type stackSlotType();

  Variable *copyToReg(Operand *Src, int32_t RegNum = Variable::NoRegister);

  /// Returns a vector in a register with the given constant entries.
  Variable *makeVectorOfZeros(Type Ty, int32_t RegNum = Variable::NoRegister);
  Variable *makeVectorOfOnes(Type Ty, int32_t RegNum = Variable::NoRegister);
  Variable *makeVectorOfMinusOnes(Type Ty,
                                  int32_t RegNum = Variable::NoRegister);
  Variable *makeVectorOfHighOrderBits(Type Ty,
                                      int32_t RegNum = Variable::NoRegister);
  Variable *makeVectorOfFabsMask(Type Ty,
                                 int32_t RegNum = Variable::NoRegister);

  /// Return a memory operand corresponding to a stack allocated Variable.
  typename Traits::X86OperandMem *
  getMemoryOperandForStackSlot(Type Ty, Variable *Slot, uint32_t Offset = 0);

  void makeRandomRegisterPermutation(
      llvm::SmallVectorImpl<int32_t> &Permutation,
      const llvm::SmallBitVector &ExcludeRegisters) const override;

  /// The following are helpers that insert lowered x86 instructions
  /// with minimal syntactic overhead, so that the lowering code can
  /// look as close to assembly as practical.
  void _adc(Variable *Dest, Operand *Src0) {
    Context.insert(Traits::Insts::Adc::create(Func, Dest, Src0));
  }
  void _adc_rmw(typename Traits::X86OperandMem *DestSrc0, Operand *Src1) {
    Context.insert(Traits::Insts::AdcRMW::create(Func, DestSrc0, Src1));
  }
  void _add(Variable *Dest, Operand *Src0) {
    Context.insert(Traits::Insts::Add::create(Func, Dest, Src0));
  }
  void _add_rmw(typename Traits::X86OperandMem *DestSrc0, Operand *Src1) {
    Context.insert(Traits::Insts::AddRMW::create(Func, DestSrc0, Src1));
  }
  void _adjust_stack(int32_t Amount) {
    Context.insert(Traits::Insts::AdjustStack::create(
        Func, Amount, getPhysicalRegister(Traits::RegisterSet::Reg_esp)));
  }
  void _addps(Variable *Dest, Operand *Src0) {
    Context.insert(Traits::Insts::Addps::create(Func, Dest, Src0));
  }
  void _addss(Variable *Dest, Operand *Src0) {
    Context.insert(Traits::Insts::Addss::create(Func, Dest, Src0));
  }
  void _and(Variable *Dest, Operand *Src0) {
    Context.insert(Traits::Insts::And::create(Func, Dest, Src0));
  }
  void _and_rmw(typename Traits::X86OperandMem *DestSrc0, Operand *Src1) {
    Context.insert(Traits::Insts::AndRMW::create(Func, DestSrc0, Src1));
  }
  void _blendvps(Variable *Dest, Operand *Src0, Operand *Src1) {
    Context.insert(Traits::Insts::Blendvps::create(Func, Dest, Src0, Src1));
  }
  void _br(typename Traits::Cond::BrCond Condition, CfgNode *TargetTrue,
           CfgNode *TargetFalse) {
    Context.insert(
        Traits::Insts::Br::create(Func, TargetTrue, TargetFalse, Condition));
  }
  void _br(CfgNode *Target) {
    Context.insert(Traits::Insts::Br::create(Func, Target));
  }
  void _br(typename Traits::Cond::BrCond Condition, CfgNode *Target) {
    Context.insert(Traits::Insts::Br::create(Func, Target, Condition));
  }
  void _br(typename Traits::Cond::BrCond Condition,
           typename Traits::Insts::Label *Label) {
    Context.insert(Traits::Insts::Br::create(Func, Label, Condition));
  }
  void _bsf(Variable *Dest, Operand *Src0) {
    Context.insert(Traits::Insts::Bsf::create(Func, Dest, Src0));
  }
  void _bsr(Variable *Dest, Operand *Src0) {
    Context.insert(Traits::Insts::Bsr::create(Func, Dest, Src0));
  }
  void _bswap(Variable *SrcDest) {
    Context.insert(Traits::Insts::Bswap::create(Func, SrcDest));
  }
  void _cbwdq(Variable *Dest, Operand *Src0) {
    Context.insert(Traits::Insts::Cbwdq::create(Func, Dest, Src0));
  }
  void _cmov(Variable *Dest, Operand *Src0,
             typename Traits::Cond::BrCond Condition) {
    Context.insert(Traits::Insts::Cmov::create(Func, Dest, Src0, Condition));
  }
  void _cmp(Operand *Src0, Operand *Src1) {
    Context.insert(Traits::Insts::Icmp::create(Func, Src0, Src1));
  }
  void _cmpps(Variable *Dest, Operand *Src0,
              typename Traits::Cond::CmppsCond Condition) {
    Context.insert(Traits::Insts::Cmpps::create(Func, Dest, Src0, Condition));
  }
  void _cmpxchg(Operand *DestOrAddr, Variable *Eax, Variable *Desired,
                bool Locked) {
    Context.insert(
        Traits::Insts::Cmpxchg::create(Func, DestOrAddr, Eax, Desired, Locked));
    // Mark eax as possibly modified by cmpxchg.
    Context.insert(
        InstFakeDef::create(Func, Eax, llvm::dyn_cast<Variable>(DestOrAddr)));
    _set_dest_nonkillable();
    Context.insert(InstFakeUse::create(Func, Eax));
  }
  void _cmpxchg8b(typename Traits::X86OperandMem *Addr, Variable *Edx,
                  Variable *Eax, Variable *Ecx, Variable *Ebx, bool Locked) {
    Context.insert(Traits::Insts::Cmpxchg8b::create(Func, Addr, Edx, Eax, Ecx,
                                                    Ebx, Locked));
    // Mark edx, and eax as possibly modified by cmpxchg8b.
    Context.insert(InstFakeDef::create(Func, Edx));
    _set_dest_nonkillable();
    Context.insert(InstFakeUse::create(Func, Edx));
    Context.insert(InstFakeDef::create(Func, Eax));
    _set_dest_nonkillable();
    Context.insert(InstFakeUse::create(Func, Eax));
  }
  void _cvt(Variable *Dest, Operand *Src0,
            typename Traits::Insts::Cvt::CvtVariant Variant) {
    Context.insert(Traits::Insts::Cvt::create(Func, Dest, Src0, Variant));
  }
  void _div(Variable *Dest, Operand *Src0, Operand *Src1) {
    Context.insert(Traits::Insts::Div::create(Func, Dest, Src0, Src1));
  }
  void _divps(Variable *Dest, Operand *Src0) {
    Context.insert(Traits::Insts::Divps::create(Func, Dest, Src0));
  }
  void _divss(Variable *Dest, Operand *Src0) {
    Context.insert(Traits::Insts::Divss::create(Func, Dest, Src0));
  }
  void _fld(Operand *Src0) {
    Context.insert(Traits::Insts::Fld::create(Func, Src0));
  }
  void _fstp(Variable *Dest) {
    Context.insert(Traits::Insts::Fstp::create(Func, Dest));
  }
  void _idiv(Variable *Dest, Operand *Src0, Operand *Src1) {
    Context.insert(Traits::Insts::Idiv::create(Func, Dest, Src0, Src1));
  }
  void _imul(Variable *Dest, Operand *Src0) {
    Context.insert(Traits::Insts::Imul::create(Func, Dest, Src0));
  }
  void _insertps(Variable *Dest, Operand *Src0, Operand *Src1) {
    Context.insert(Traits::Insts::Insertps::create(Func, Dest, Src0, Src1));
  }
  void _jmp(Operand *Target) {
    Context.insert(Traits::Insts::Jmp::create(Func, Target));
  }
  void _lea(Variable *Dest, Operand *Src0) {
    Context.insert(Traits::Insts::Lea::create(Func, Dest, Src0));
  }
  void _mfence() { Context.insert(Traits::Insts::Mfence::create(Func)); }
  /// If Dest=nullptr is passed in, then a new variable is created,
  /// marked as infinite register allocation weight, and returned
  /// through the in/out Dest argument.
  void _mov(Variable *&Dest, Operand *Src0,
            int32_t RegNum = Variable::NoRegister) {
    if (Dest == nullptr)
      Dest = makeReg(Src0->getType(), RegNum);
    Context.insert(Traits::Insts::Mov::create(Func, Dest, Src0));
  }
  void _mov_nonkillable(Variable *Dest, Operand *Src0) {
    Inst *NewInst = Traits::Insts::Mov::create(Func, Dest, Src0);
    NewInst->setDestNonKillable();
    Context.insert(NewInst);
  }
  void _movd(Variable *Dest, Operand *Src0) {
    Context.insert(Traits::Insts::Movd::create(Func, Dest, Src0));
  }
  void _movp(Variable *Dest, Operand *Src0) {
    Context.insert(Traits::Insts::Movp::create(Func, Dest, Src0));
  }
  void _movq(Variable *Dest, Operand *Src0) {
    Context.insert(Traits::Insts::Movq::create(Func, Dest, Src0));
  }
  void _movss(Variable *Dest, Variable *Src0) {
    Context.insert(Traits::Insts::MovssRegs::create(Func, Dest, Src0));
  }
  void _movsx(Variable *Dest, Operand *Src0) {
    Context.insert(Traits::Insts::Movsx::create(Func, Dest, Src0));
  }
  void _movzx(Variable *Dest, Operand *Src0) {
    Context.insert(Traits::Insts::Movzx::create(Func, Dest, Src0));
  }
  void _mul(Variable *Dest, Variable *Src0, Operand *Src1) {
    Context.insert(Traits::Insts::Mul::create(Func, Dest, Src0, Src1));
  }
  void _mulps(Variable *Dest, Operand *Src0) {
    Context.insert(Traits::Insts::Mulps::create(Func, Dest, Src0));
  }
  void _mulss(Variable *Dest, Operand *Src0) {
    Context.insert(Traits::Insts::Mulss::create(Func, Dest, Src0));
  }
  void _neg(Variable *SrcDest) {
    Context.insert(Traits::Insts::Neg::create(Func, SrcDest));
  }
  void _nop(SizeT Variant) {
    Context.insert(Traits::Insts::Nop::create(Func, Variant));
  }
  void _or(Variable *Dest, Operand *Src0) {
    Context.insert(Traits::Insts::Or::create(Func, Dest, Src0));
  }
  void _or_rmw(typename Traits::X86OperandMem *DestSrc0, Operand *Src1) {
    Context.insert(Traits::Insts::OrRMW::create(Func, DestSrc0, Src1));
  }
  void _padd(Variable *Dest, Operand *Src0) {
    Context.insert(Traits::Insts::Padd::create(Func, Dest, Src0));
  }
  void _pand(Variable *Dest, Operand *Src0) {
    Context.insert(Traits::Insts::Pand::create(Func, Dest, Src0));
  }
  void _pandn(Variable *Dest, Operand *Src0) {
    Context.insert(Traits::Insts::Pandn::create(Func, Dest, Src0));
  }
  void _pblendvb(Variable *Dest, Operand *Src0, Operand *Src1) {
    Context.insert(Traits::Insts::Pblendvb::create(Func, Dest, Src0, Src1));
  }
  void _pcmpeq(Variable *Dest, Operand *Src0) {
    Context.insert(Traits::Insts::Pcmpeq::create(Func, Dest, Src0));
  }
  void _pcmpgt(Variable *Dest, Operand *Src0) {
    Context.insert(Traits::Insts::Pcmpgt::create(Func, Dest, Src0));
  }
  void _pextr(Variable *Dest, Operand *Src0, Operand *Src1) {
    Context.insert(Traits::Insts::Pextr::create(Func, Dest, Src0, Src1));
  }
  void _pinsr(Variable *Dest, Operand *Src0, Operand *Src1) {
    Context.insert(Traits::Insts::Pinsr::create(Func, Dest, Src0, Src1));
  }
  void _pmull(Variable *Dest, Operand *Src0) {
    Context.insert(Traits::Insts::Pmull::create(Func, Dest, Src0));
  }
  void _pmuludq(Variable *Dest, Operand *Src0) {
    Context.insert(Traits::Insts::Pmuludq::create(Func, Dest, Src0));
  }
  void _pop(Variable *Dest) {
    Context.insert(Traits::Insts::Pop::create(Func, Dest));
  }
  void _por(Variable *Dest, Operand *Src0) {
    Context.insert(Traits::Insts::Por::create(Func, Dest, Src0));
  }
  void _pshufd(Variable *Dest, Operand *Src0, Operand *Src1) {
    Context.insert(Traits::Insts::Pshufd::create(Func, Dest, Src0, Src1));
  }
  void _psll(Variable *Dest, Operand *Src0) {
    Context.insert(Traits::Insts::Psll::create(Func, Dest, Src0));
  }
  void _psra(Variable *Dest, Operand *Src0) {
    Context.insert(Traits::Insts::Psra::create(Func, Dest, Src0));
  }
  void _psrl(Variable *Dest, Operand *Src0) {
    Context.insert(Traits::Insts::Psrl::create(Func, Dest, Src0));
  }
  void _psub(Variable *Dest, Operand *Src0) {
    Context.insert(Traits::Insts::Psub::create(Func, Dest, Src0));
  }
  void _push(Variable *Src0) {
    Context.insert(Traits::Insts::Push::create(Func, Src0));
  }
  void _pxor(Variable *Dest, Operand *Src0) {
    Context.insert(Traits::Insts::Pxor::create(Func, Dest, Src0));
  }
  void _ret(Variable *Src0 = nullptr) {
    Context.insert(Traits::Insts::Ret::create(Func, Src0));
  }
  void _rol(Variable *Dest, Operand *Src0) {
    Context.insert(Traits::Insts::Rol::create(Func, Dest, Src0));
  }
  void _sar(Variable *Dest, Operand *Src0) {
    Context.insert(Traits::Insts::Sar::create(Func, Dest, Src0));
  }
  void _sbb(Variable *Dest, Operand *Src0) {
    Context.insert(Traits::Insts::Sbb::create(Func, Dest, Src0));
  }
  void _sbb_rmw(typename Traits::X86OperandMem *DestSrc0, Operand *Src1) {
    Context.insert(Traits::Insts::SbbRMW::create(Func, DestSrc0, Src1));
  }
  void _setcc(Variable *Dest, typename Traits::Cond::BrCond Condition) {
    Context.insert(Traits::Insts::Setcc::create(Func, Dest, Condition));
  }
  void _shl(Variable *Dest, Operand *Src0) {
    Context.insert(Traits::Insts::Shl::create(Func, Dest, Src0));
  }
  void _shld(Variable *Dest, Variable *Src0, Variable *Src1) {
    Context.insert(Traits::Insts::Shld::create(Func, Dest, Src0, Src1));
  }
  void _shr(Variable *Dest, Operand *Src0) {
    Context.insert(Traits::Insts::Shr::create(Func, Dest, Src0));
  }
  void _shrd(Variable *Dest, Variable *Src0, Variable *Src1) {
    Context.insert(Traits::Insts::Shrd::create(Func, Dest, Src0, Src1));
  }
  void _shufps(Variable *Dest, Operand *Src0, Operand *Src1) {
    Context.insert(Traits::Insts::Shufps::create(Func, Dest, Src0, Src1));
  }
  void _sqrtss(Variable *Dest, Operand *Src0) {
    Context.insert(Traits::Insts::Sqrtss::create(Func, Dest, Src0));
  }
  void _store(Operand *Value, typename Traits::X86Operand *Mem) {
    Context.insert(Traits::Insts::Store::create(Func, Value, Mem));
  }
  void _storep(Variable *Value, typename Traits::X86OperandMem *Mem) {
    Context.insert(Traits::Insts::StoreP::create(Func, Value, Mem));
  }
  void _storeq(Variable *Value, typename Traits::X86OperandMem *Mem) {
    Context.insert(Traits::Insts::StoreQ::create(Func, Value, Mem));
  }
  void _sub(Variable *Dest, Operand *Src0) {
    Context.insert(Traits::Insts::Sub::create(Func, Dest, Src0));
  }
  void _sub_rmw(typename Traits::X86OperandMem *DestSrc0, Operand *Src1) {
    Context.insert(Traits::Insts::SubRMW::create(Func, DestSrc0, Src1));
  }
  void _subps(Variable *Dest, Operand *Src0) {
    Context.insert(Traits::Insts::Subps::create(Func, Dest, Src0));
  }
  void _subss(Variable *Dest, Operand *Src0) {
    Context.insert(Traits::Insts::Subss::create(Func, Dest, Src0));
  }
  void _test(Operand *Src0, Operand *Src1) {
    Context.insert(Traits::Insts::Test::create(Func, Src0, Src1));
  }
  void _ucomiss(Operand *Src0, Operand *Src1) {
    Context.insert(Traits::Insts::Ucomiss::create(Func, Src0, Src1));
  }
  void _ud2() { Context.insert(Traits::Insts::UD2::create(Func)); }
  void _xadd(Operand *Dest, Variable *Src, bool Locked) {
    Context.insert(Traits::Insts::Xadd::create(Func, Dest, Src, Locked));
    // The xadd exchanges Dest and Src (modifying Src).
    // Model that update with a FakeDef followed by a FakeUse.
    Context.insert(
        InstFakeDef::create(Func, Src, llvm::dyn_cast<Variable>(Dest)));
    _set_dest_nonkillable();
    Context.insert(InstFakeUse::create(Func, Src));
  }
  void _xchg(Operand *Dest, Variable *Src) {
    Context.insert(Traits::Insts::Xchg::create(Func, Dest, Src));
    // The xchg modifies Dest and Src -- model that update with a
    // FakeDef/FakeUse.
    Context.insert(
        InstFakeDef::create(Func, Src, llvm::dyn_cast<Variable>(Dest)));
    _set_dest_nonkillable();
    Context.insert(InstFakeUse::create(Func, Src));
  }
  void _xor(Variable *Dest, Operand *Src0) {
    Context.insert(Traits::Insts::Xor::create(Func, Dest, Src0));
  }
  void _xor_rmw(typename Traits::X86OperandMem *DestSrc0, Operand *Src1) {
    Context.insert(Traits::Insts::XorRMW::create(Func, DestSrc0, Src1));
  }

  bool optimizeScalarMul(Variable *Dest, Operand *Src0, int32_t Src1);
  void findRMW();

  typename Traits::InstructionSet InstructionSet =
      Traits::InstructionSet::Begin;
  bool IsEbpBasedFrame = false;
  bool NeedsStackAlignment = false;
  size_t SpillAreaSizeBytes = 0;
  llvm::SmallBitVector TypeToRegisterSet[IceType_NUM];
  llvm::SmallBitVector ScratchRegs;
  llvm::SmallBitVector RegsUsed;
  VarList PhysicalRegisters[IceType_NUM];

  /// Randomize a given immediate operand
  Operand *randomizeOrPoolImmediate(Constant *Immediate,
                                    int32_t RegNum = Variable::NoRegister);
  typename Traits::X86OperandMem *
  randomizeOrPoolImmediate(typename Traits::X86OperandMem *MemOperand,
                           int32_t RegNum = Variable::NoRegister);
  bool RandomizationPoolingPaused = false;

private:
  ~TargetX86Base() override {}
  BoolFolding FoldingInfo;
};
} // end of namespace X86Internal
} // end of namespace Ice

#include "IceTargetLoweringX86BaseImpl.h"

#endif // SUBZERO_SRC_ICETARGETLOWERINGX86BASE_H
