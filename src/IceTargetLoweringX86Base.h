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
#include "IceUtils.h"

#include <array>
#include <type_traits>
#include <utility>

namespace Ice {
namespace X86Internal {

template <class MachineTraits> class BoolFolding;

template <class Machine> struct MachineTraits {};

/// TargetX86Base is a template for all X86 Targets, and it relies on the CRT
/// pattern for generating code, delegating to actual backends target-specific
/// lowerings (e.g., call, ret, and intrinsics.) Backends are expected to
/// implement the following methods (which should be accessible from
/// TargetX86Base):
///
/// Operand *createNaClReadTPSrcOperand()
///
/// Note: Ideally, we should be able to
///
///   static_assert(std::is_base_of<TargetX86Base<Machine>, Machine>::value);
///
/// but that does not work: the compiler does not know that Machine inherits
/// from TargetX86Base at this point in translation.
template <class Machine> class TargetX86Base : public TargetLowering {
  TargetX86Base() = delete;
  TargetX86Base(const TargetX86Base &) = delete;
  TargetX86Base &operator=(const TargetX86Base &) = delete;

public:
  using Traits = MachineTraits<Machine>;
  using BoolFolding = ::Ice::X86Internal::BoolFolding<Traits>;

  ~TargetX86Base() override = default;

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

  const llvm::SmallBitVector &getAliasesForRegister(SizeT Reg) const override {
    assert(Reg < Traits::RegisterSet::Reg_NUM);
    return RegisterAliases[Reg];
  }

  bool hasFramePointer() const override { return IsEbpBasedFrame; }
  SizeT getFrameOrStackReg() const override {
    return IsEbpBasedFrame ? Traits::RegisterSet::Reg_ebp
                           : Traits::RegisterSet::Reg_esp;
  }
  size_t typeWidthInBytesOnStack(Type Ty) const override {
    // Round up to the next multiple of WordType bytes.
    const uint32_t WordSizeInBytes = typeWidthInBytes(Traits::WordType);
    return Utils::applyAlignment(typeWidthInBytes(Ty), WordSizeInBytes);
  }

  SizeT getMinJumpTableSize() const override { return 4; }

  void emitVariable(const Variable *Var) const override;

  const char *getConstantPrefix() const final { return "$"; }
  void emit(const ConstantUndef *C) const final;
  void emit(const ConstantInteger32 *C) const final;
  void emit(const ConstantInteger64 *C) const final;
  void emit(const ConstantFloat *C) const final;
  void emit(const ConstantDouble *C) const final;

  void initNodeForLowering(CfgNode *Node) override;
  /// x86-32: Ensure that a 64-bit Variable has been split into 2 32-bit
  /// Variables, creating them if necessary.  This is needed for all
  /// I64 operations, and it is needed for pushing F64 arguments for
  /// function calls using the 32-bit push instruction (though the
  /// latter could be done by directly writing to the stack).
  ///
  /// x86-64: Complains loudly if invoked because the cpu can handle
  /// 64-bit types natively.
  template <typename T = Traits>
  typename std::enable_if<!T::Is64Bit, void>::type split64(Variable *Var);
  template <typename T = Traits>
  typename std::enable_if<T::Is64Bit, void>::type split64(Variable *) {
    llvm::report_fatal_error(
        "Hey, yo! This is x86-64. Watcha doin'? (split64)");
  }

  template <typename T = Traits>
  typename std::enable_if<!T::Is64Bit, Operand>::type *
  loOperand(Operand *Operand);
  template <typename T = Traits>
  typename std::enable_if<T::Is64Bit, Operand>::type *loOperand(Operand *) {
    llvm::report_fatal_error(
        "Hey, yo! This is x86-64. Watcha doin'? (loOperand)");
  }

  template <typename T = Traits>
  typename std::enable_if<!T::Is64Bit, Operand>::type *
  hiOperand(Operand *Operand);
  template <typename T = Traits>
  typename std::enable_if<T::Is64Bit, Operand>::type *hiOperand(Operand *) {
    llvm::report_fatal_error(
        "Hey, yo! This is x86-64. Watcha doin'? (hiOperand)");
  }

  void finishArgumentLowering(Variable *Arg, Variable *FramePtr,
                              size_t BasicFrameOffset, size_t &InArgsSizeBytes);
  typename Traits::Address stackVarToAsmOperand(const Variable *Var) const;

  typename Traits::InstructionSet getInstructionSet() const {
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
  void lowerCast(const InstCast *Inst) override;
  void lowerExtractElement(const InstExtractElement *Inst) override;
  void lowerFcmp(const InstFcmp *Inst) override;
  void lowerIcmp(const InstIcmp *Inst) override;
  /// Complains loudly if invoked because the cpu can handle 64-bit types
  /// natively.
  template <typename T = Traits>
  typename std::enable_if<T::Is64Bit, void>::type
  lowerIcmp64(const InstIcmp *) {
    llvm::report_fatal_error(
        "Hey, yo! This is x86-64. Watcha doin'? (lowerIcmp64)");
  }
  /// x86lowerIcmp64 handles 64-bit icmp lowering.
  template <typename T = Traits>
  typename std::enable_if<!T::Is64Bit, void>::type
  lowerIcmp64(const InstIcmp *Inst);

  void lowerIntrinsicCall(const InstIntrinsicCall *Inst) override;
  void lowerInsertElement(const InstInsertElement *Inst) override;
  void lowerLoad(const InstLoad *Inst) override;
  void lowerPhi(const InstPhi *Inst) override;
  void lowerSelect(const InstSelect *Inst) override;
  void lowerStore(const InstStore *Inst) override;
  void lowerSwitch(const InstSwitch *Inst) override;
  void lowerUnreachable(const InstUnreachable *Inst) override;
  void lowerOther(const Inst *Instr) override;
  void lowerRMW(const typename Traits::Insts::FakeRMW *RMW);
  void prelowerPhis() override;
  void doAddressOptLoad() override;
  void doAddressOptStore() override;
  void randomlyInsertNop(float Probability,
                         RandomNumberGenerator &RNG) override;

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
  /// Load from memory for a given type.
  void typedLoad(Type Ty, Variable *Dest, Variable *Base, Constant *Offset);
  /// Store to memory for a given type.
  void typedStore(Type Ty, Variable *Value, Variable *Base, Constant *Offset);
  /// Copy memory of given type from Src to Dest using OffsetAmt on both.
  void copyMemory(Type Ty, Variable *Dest, Variable *Src, int32_t OffsetAmt);
  /// Replace some calls to memcpy with inline instructions.
  void lowerMemcpy(Operand *Dest, Operand *Src, Operand *Count);
  /// Replace some calls to memmove with inline instructions.
  void lowerMemmove(Operand *Dest, Operand *Src, Operand *Count);
  /// Replace some calls to memset with inline instructions.
  void lowerMemset(Operand *Dest, Operand *Val, Operand *Count);

  /// Lower an indirect jump adding sandboxing when needed.
  void lowerIndirectJump(Variable *Target);

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

  using LowerBinOp = void (TargetX86Base::*)(Variable *, Operand *);
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
  using LegalMask = uint32_t;
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

  static constexpr uint32_t NoSizeLimit = 0;
  static const Type TypeForSize[];
  /// Returns the largest type which is equal to or larger than Size bytes. The
  /// type is suitable for copying memory i.e. a load and store will be a
  /// single instruction (for example x86 will get f64 not i64).
  static Type largestTypeInSize(uint32_t Size, uint32_t MaxSize = NoSizeLimit);
  /// Returns the smallest type which is equal to or larger than Size bytes. If
  /// one doesn't exist then the largest type smaller than Size bytes is
  /// returned. The type is suitable for memory copies as described at
  /// largestTypeInSize.
  static Type firstTypeThatFitsSize(uint32_t Size,
                                    uint32_t MaxSize = NoSizeLimit);

  Variable *copyToReg(Operand *Src, int32_t RegNum = Variable::NoRegister);

  /// \name Returns a vector in a register with the given constant entries.
  /// @{
  Variable *makeVectorOfZeros(Type Ty, int32_t RegNum = Variable::NoRegister);
  Variable *makeVectorOfOnes(Type Ty, int32_t RegNum = Variable::NoRegister);
  Variable *makeVectorOfMinusOnes(Type Ty,
                                  int32_t RegNum = Variable::NoRegister);
  Variable *makeVectorOfHighOrderBits(Type Ty,
                                      int32_t RegNum = Variable::NoRegister);
  Variable *makeVectorOfFabsMask(Type Ty,
                                 int32_t RegNum = Variable::NoRegister);
  /// @}

  /// Return a memory operand corresponding to a stack allocated Variable.
  typename Traits::X86OperandMem *
  getMemoryOperandForStackSlot(Type Ty, Variable *Slot, uint32_t Offset = 0);

  void
  makeRandomRegisterPermutation(llvm::SmallVectorImpl<int32_t> &Permutation,
                                const llvm::SmallBitVector &ExcludeRegisters,
                                uint64_t Salt) const override;

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
    Context.insert(Traits::Insts::Br::create(
        Func, TargetTrue, TargetFalse, Condition, Traits::Insts::Br::Far));
  }
  void _br(CfgNode *Target) {
    Context.insert(
        Traits::Insts::Br::create(Func, Target, Traits::Insts::Br::Far));
  }
  void _br(typename Traits::Cond::BrCond Condition, CfgNode *Target) {
    Context.insert(Traits::Insts::Br::create(Func, Target, Condition,
                                             Traits::Insts::Br::Far));
  }
  void _br(typename Traits::Cond::BrCond Condition,
           typename Traits::Insts::Label *Label,
           typename Traits::Insts::Br::Mode Kind = Traits::Insts::Br::Near) {
    Context.insert(Traits::Insts::Br::create(Func, Label, Condition, Kind));
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
  template <typename T = Traits>
  typename std::enable_if<T::UsesX87, void>::type _fld(Operand *Src0) {
    Context.insert(Traits::Insts::template Fld<>::create(Func, Src0));
  }
  // TODO(jpp): when implementing the X8664 calling convention, make sure x8664
  // does not invoke this method, and remove it.
  template <typename T = Traits>
  typename std::enable_if<!T::UsesX87, void>::type _fld(Operand *) {
    llvm::report_fatal_error("fld is not available in x86-64");
  }
  template <typename T = Traits>
  typename std::enable_if<T::UsesX87, void>::type _fstp(Variable *Dest) {
    Context.insert(Traits::Insts::template Fstp<>::create(Func, Dest));
  }
  // TODO(jpp): when implementing the X8664 calling convention, make sure x8664
  // does not invoke this method, and remove it.
  template <typename T = Traits>
  typename std::enable_if<!T::UsesX87, void>::type _fstp(Variable *) {
    llvm::report_fatal_error("fstp is not available in x86-64");
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

  void _iaca_start() {
    if (!BuildDefs::minimal())
      Context.insert(Traits::Insts::IacaStart::create(Func));
  }
  void _iaca_end() {
    if (!BuildDefs::minimal())
      Context.insert(Traits::Insts::IacaEnd::create(Func));
  }

  /// This class helps wrap IACA markers around the code generated by the
  /// current scope. It means you don't need to put an end before each return.
  class ScopedIacaMark {
    ScopedIacaMark(const ScopedIacaMark &) = delete;
    ScopedIacaMark &operator=(const ScopedIacaMark &) = delete;

  public:
    ScopedIacaMark(TargetX86Base *Lowering) : Lowering(Lowering) {
      Lowering->_iaca_start();
    }
    ~ScopedIacaMark() { end(); }
    void end() {
      if (!Lowering)
        return;
      Lowering->_iaca_end();
      Lowering = nullptr;
    }

  private:
    TargetX86Base *Lowering;
  };

  bool optimizeScalarMul(Variable *Dest, Operand *Src0, int32_t Src1);
  void findRMW();

  typename Traits::InstructionSet InstructionSet =
      Traits::InstructionSet::Begin;
  bool IsEbpBasedFrame = false;
  bool NeedsStackAlignment = false;
  size_t SpillAreaSizeBytes = 0;
  std::array<llvm::SmallBitVector, IceType_NUM> TypeToRegisterSet;
  std::array<llvm::SmallBitVector, Traits::RegisterSet::Reg_NUM>
      RegisterAliases;
  llvm::SmallBitVector ScratchRegs;
  llvm::SmallBitVector RegsUsed;
  std::array<VarList, IceType_NUM> PhysicalRegisters;

  /// Randomize a given immediate operand
  Operand *randomizeOrPoolImmediate(Constant *Immediate,
                                    int32_t RegNum = Variable::NoRegister);
  typename Traits::X86OperandMem *
  randomizeOrPoolImmediate(typename Traits::X86OperandMem *MemOperand,
                           int32_t RegNum = Variable::NoRegister);
  bool RandomizationPoolingPaused = false;

private:
  /// dispatchToConcrete is the template voodoo that allows TargetX86Base to
  /// invoke methods in Machine (which inherits from TargetX86Base) without
  /// having to rely on virtual method calls. There are two overloads, one for
  /// non-void types, and one for void types. We need this becase, for non-void
  /// types, we need to return the method result, where as for void, we don't.
  /// While it is true that the code compiles without the void "version", there
  /// used to be a time when compilers would reject such code.
  ///
  /// This machinery is far from perfect. Note that, in particular, the
  /// arguments provided to dispatchToConcrete() need to match the arguments for
  /// Method **exactly** (i.e., no argument promotion is performed.)
  template <typename Ret, typename... Args>
  typename std::enable_if<!std::is_void<Ret>::value, Ret>::type
  dispatchToConcrete(Ret (Machine::*Method)(Args...), Args &&... args) {
    return (static_cast<Machine *>(this)->*Method)(std::forward<Args>(args)...);
  }

  template <typename... Args>
  void dispatchToConcrete(void (Machine::*Method)(Args...), Args &&... args) {
    (static_cast<Machine *>(this)->*Method)(std::forward<Args>(args)...);
  }

  BoolFolding FoldingInfo;
};
} // end of namespace X86Internal
} // end of namespace Ice

#include "IceTargetLoweringX86BaseImpl.h"

#endif // SUBZERO_SRC_ICETARGETLOWERINGX86BASE_H
