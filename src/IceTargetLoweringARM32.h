//===- subzero/src/IceTargetLoweringARM32.h - ARM32 lowering ----*- C++ -*-===//
//
//                        The Subzero Code Generator
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file declares the TargetLoweringARM32 class, which implements the
/// TargetLowering interface for the ARM 32-bit architecture.
///
//===----------------------------------------------------------------------===//

#ifndef SUBZERO_SRC_ICETARGETLOWERINGARM32_H
#define SUBZERO_SRC_ICETARGETLOWERINGARM32_H

#include "IceDefs.h"
#include "IceInstARM32.h"
#include "IceRegistersARM32.h"
#include "IceTargetLowering.h"

#include "llvm/ADT/SmallBitVector.h"

namespace Ice {

// Class encapsulating ARM cpu features / instruction set.
class TargetARM32Features {
  TargetARM32Features() = delete;
  TargetARM32Features(const TargetARM32Features &) = delete;
  TargetARM32Features &operator=(const TargetARM32Features &) = delete;

public:
  explicit TargetARM32Features(const ClFlags &Flags);

  enum ARM32InstructionSet {
    Begin,
    // Neon is the PNaCl baseline instruction set.
    Neon = Begin,
    HWDivArm, // HW divide in ARM mode (not just Thumb mode).
    End
  };

  bool hasFeature(ARM32InstructionSet I) const { return I <= InstructionSet; }

private:
  ARM32InstructionSet InstructionSet = ARM32InstructionSet::Begin;
};

// The target lowering logic for ARM32.
class TargetARM32 : public TargetLowering {
  TargetARM32() = delete;
  TargetARM32(const TargetARM32 &) = delete;
  TargetARM32 &operator=(const TargetARM32 &) = delete;

public:
  static void staticInit();
  // TODO(jvoung): return a unique_ptr.
  static TargetARM32 *create(Cfg *Func) { return new TargetARM32(Func); }

  void initNodeForLowering(CfgNode *Node) override {
    BoolComputations.forgetProducers();
    BoolComputations.recordProducers(Node);
    BoolComputations.dump(Func);
  }

  void translateOm1() override;
  void translateO2() override;
  bool doBranchOpt(Inst *I, const CfgNode *NextNode) override;

  SizeT getNumRegisters() const override { return RegARM32::Reg_NUM; }
  Variable *getPhysicalRegister(SizeT RegNum, Type Ty = IceType_void) override;
  IceString getRegName(SizeT RegNum, Type Ty) const override;
  llvm::SmallBitVector getRegisterSet(RegSetMask Include,
                                      RegSetMask Exclude) const override;
  const llvm::SmallBitVector &getRegisterSetForType(Type Ty) const override {
    return TypeToRegisterSet[Ty];
  }
  const llvm::SmallBitVector &getAliasesForRegister(SizeT Reg) const override {
    return RegisterAliases[Reg];
  }
  bool hasFramePointer() const override { return UsesFramePointer; }
  SizeT getStackReg() const override { return RegARM32::Reg_sp; }
  SizeT getFrameOrStackReg() const override {
    return UsesFramePointer ? RegARM32::Reg_fp : RegARM32::Reg_sp;
  }
  SizeT getReservedTmpReg() const { return RegARM32::Reg_ip; }

  size_t typeWidthInBytesOnStack(Type Ty) const override {
    // Round up to the next multiple of 4 bytes. In particular, i1, i8, and i16
    // are rounded up to 4 bytes.
    return (typeWidthInBytes(Ty) + 3) & ~3;
  }

  bool shouldSplitToVariable64On32(Type Ty) const override {
    return Ty == IceType_i64;
  }

  // TODO(ascull): what size is best for ARM?
  SizeT getMinJumpTableSize() const override { return 3; }
  void emitJumpTable(const Cfg *Func,
                     const InstJumpTable *JumpTable) const override;

  void emitVariable(const Variable *Var) const override;

  const char *getConstantPrefix() const final { return "#"; }
  void emit(const ConstantUndef *C) const final;
  void emit(const ConstantInteger32 *C) const final;
  void emit(const ConstantInteger64 *C) const final;
  void emit(const ConstantFloat *C) const final;
  void emit(const ConstantDouble *C) const final;

  void lowerArguments() override;
  void addProlog(CfgNode *Node) override;
  void addEpilog(CfgNode *Node) override;

  Operand *loOperand(Operand *Operand);
  Operand *hiOperand(Operand *Operand);
  void finishArgumentLowering(Variable *Arg, Variable *FramePtr,
                              size_t BasicFrameOffset, size_t &InArgsSizeBytes);

  bool hasCPUFeature(TargetARM32Features::ARM32InstructionSet I) const {
    return CPUFeatures.hasFeature(I);
  }
  Operand *legalizeUndef(Operand *From, int32_t RegNum = Variable::NoRegister);

protected:
  explicit TargetARM32(Cfg *Func);

  void postLower() override;

  void lowerAlloca(const InstAlloca *Inst) override;
  void lowerArithmetic(const InstArithmetic *Inst) override;
  void lowerAssign(const InstAssign *Inst) override;
  void lowerBr(const InstBr *Inst) override;
  void lowerCall(const InstCall *Inst) override;
  void lowerCast(const InstCast *Inst) override;
  void lowerExtractElement(const InstExtractElement *Inst) override;
  void lowerFcmpCond(const InstFcmp *Instr, CondARM32::Cond *CondIfTrue0,
                     CondARM32::Cond *CondIfTrue1,
                     CondARM32::Cond *CondIfFalse);
  void lowerFcmp(const InstFcmp *Instr) override;
  void lowerIcmpCond(const InstIcmp *Instr, CondARM32::Cond *CondIfTrue,
                     CondARM32::Cond *CondIfFalse);
  void lowerIcmp(const InstIcmp *Instr) override;
  void lowerAtomicRMW(Variable *Dest, uint32_t Operation, Operand *Ptr,
                      Operand *Val);
  void lowerIntrinsicCall(const InstIntrinsicCall *Inst) override;
  void lowerInsertElement(const InstInsertElement *Inst) override;
  void lowerLoad(const InstLoad *Inst) override;
  void lowerPhi(const InstPhi *Inst) override;
  void lowerRet(const InstRet *Inst) override;
  void lowerSelect(const InstSelect *Inst) override;
  void lowerStore(const InstStore *Inst) override;
  void lowerSwitch(const InstSwitch *Inst) override;
  void lowerUnreachable(const InstUnreachable *Inst) override;
  void prelowerPhis() override;
  void doAddressOptLoad() override;
  void doAddressOptStore() override;
  void randomlyInsertNop(float Probability,
                         RandomNumberGenerator &RNG) override;

  enum OperandLegalization {
    Legal_None = 0,
    Legal_Reg = 1 << 0,  /// physical register, not stack location
    Legal_Flex = 1 << 1, /// A flexible operand2, which can hold rotated small
                         /// immediates, or shifted registers.
    Legal_Mem = 1 << 2,  /// includes [r0, r1 lsl #2] as well as [sp, #12]
    Legal_All = ~Legal_None
  };
  using LegalMask = uint32_t;
  Operand *legalize(Operand *From, LegalMask Allowed = Legal_All,
                    int32_t RegNum = Variable::NoRegister);
  Variable *legalizeToReg(Operand *From, int32_t RegNum = Variable::NoRegister);
  OperandARM32Mem *formMemoryOperand(Operand *Ptr, Type Ty);

  Variable64On32 *makeI64RegPair();
  Variable *makeReg(Type Ty, int32_t RegNum = Variable::NoRegister);
  static Type stackSlotType();
  Variable *copyToReg(Operand *Src, int32_t RegNum = Variable::NoRegister);
  void alignRegisterPow2(Variable *Reg, uint32_t Align);

  /// Returns a vector in a register with the given constant entries.
  Variable *makeVectorOfZeros(Type Ty, int32_t RegNum = Variable::NoRegister);

  void
  makeRandomRegisterPermutation(llvm::SmallVectorImpl<int32_t> &Permutation,
                                const llvm::SmallBitVector &ExcludeRegisters,
                                uint64_t Salt) const override;

  // If a divide-by-zero check is needed, inserts a: test; branch .LSKIP; trap;
  // .LSKIP: <continuation>. If no check is needed nothing is inserted.
  void div0Check(Type Ty, Operand *SrcLo, Operand *SrcHi);
  using ExtInstr = void (TargetARM32::*)(Variable *, Variable *,
                                         CondARM32::Cond);
  using DivInstr = void (TargetARM32::*)(Variable *, Variable *, Variable *,
                                         CondARM32::Cond);
  void lowerIDivRem(Variable *Dest, Variable *T, Variable *Src0R, Operand *Src1,
                    ExtInstr ExtFunc, DivInstr DivFunc,
                    const char *DivHelperName, bool IsRemainder);

  void lowerCLZ(Variable *Dest, Variable *ValLo, Variable *ValHi);

  // The following are helpers that insert lowered ARM32 instructions with
  // minimal syntactic overhead, so that the lowering code can look as close to
  // assembly as practical.
  void _add(Variable *Dest, Variable *Src0, Operand *Src1,
            CondARM32::Cond Pred = CondARM32::AL) {
    Context.insert(InstARM32Add::create(Func, Dest, Src0, Src1, Pred));
  }
  void _adds(Variable *Dest, Variable *Src0, Operand *Src1,
             CondARM32::Cond Pred = CondARM32::AL) {
    constexpr bool SetFlags = true;
    Context.insert(
        InstARM32Add::create(Func, Dest, Src0, Src1, Pred, SetFlags));
  }
  void _adc(Variable *Dest, Variable *Src0, Operand *Src1,
            CondARM32::Cond Pred = CondARM32::AL) {
    Context.insert(InstARM32Adc::create(Func, Dest, Src0, Src1, Pred));
  }
  void _adjust_stack(int32_t Amount, Operand *SrcAmount) {
    Context.insert(InstARM32AdjustStack::create(
        Func, getPhysicalRegister(RegARM32::Reg_sp), Amount, SrcAmount));
  }
  void _and(Variable *Dest, Variable *Src0, Operand *Src1,
            CondARM32::Cond Pred = CondARM32::AL) {
    Context.insert(InstARM32And::create(Func, Dest, Src0, Src1, Pred));
  }
  void _asr(Variable *Dest, Variable *Src0, Operand *Src1,
            CondARM32::Cond Pred = CondARM32::AL) {
    Context.insert(InstARM32Asr::create(Func, Dest, Src0, Src1, Pred));
  }
  void _bic(Variable *Dest, Variable *Src0, Operand *Src1,
            CondARM32::Cond Pred = CondARM32::AL) {
    Context.insert(InstARM32Bic::create(Func, Dest, Src0, Src1, Pred));
  }
  void _br(CfgNode *TargetTrue, CfgNode *TargetFalse,
           CondARM32::Cond Condition) {
    Context.insert(
        InstARM32Br::create(Func, TargetTrue, TargetFalse, Condition));
  }
  void _br(CfgNode *Target) {
    Context.insert(InstARM32Br::create(Func, Target));
  }
  void _br(CfgNode *Target, CondARM32::Cond Condition) {
    Context.insert(InstARM32Br::create(Func, Target, Condition));
  }
  void _br(InstARM32Label *Label, CondARM32::Cond Condition) {
    Context.insert(InstARM32Br::create(Func, Label, Condition));
  }
  void _cmp(Variable *Src0, Operand *Src1,
            CondARM32::Cond Pred = CondARM32::AL) {
    Context.insert(InstARM32Cmp::create(Func, Src0, Src1, Pred));
  }
  void _clz(Variable *Dest, Variable *Src0,
            CondARM32::Cond Pred = CondARM32::AL) {
    Context.insert(InstARM32Clz::create(Func, Dest, Src0, Pred));
  }
  void _dmb() { Context.insert(InstARM32Dmb::create(Func)); }
  void _eor(Variable *Dest, Variable *Src0, Operand *Src1,
            CondARM32::Cond Pred = CondARM32::AL) {
    Context.insert(InstARM32Eor::create(Func, Dest, Src0, Src1, Pred));
  }
  /// _ldr, for all your memory to Variable data moves. It handles all types
  /// (integer, floating point, and vectors.) Addr needs to be valid for Dest's
  /// type (e.g., no immediates for vector loads, and no index registers for fp
  /// loads.)
  void _ldr(Variable *Dest, OperandARM32Mem *Addr,
            CondARM32::Cond Pred = CondARM32::AL) {
    Context.insert(InstARM32Ldr::create(Func, Dest, Addr, Pred));
  }
  void _ldrex(Variable *Dest, OperandARM32Mem *Addr,
              CondARM32::Cond Pred = CondARM32::AL) {
    Context.insert(InstARM32Ldrex::create(Func, Dest, Addr, Pred));
    if (auto *Dest64 = llvm::dyn_cast<Variable64On32>(Dest)) {
      Context.insert(InstFakeDef::create(Func, Dest64->getLo(), Dest));
      Context.insert(InstFakeDef::create(Func, Dest64->getHi(), Dest));
    }
  }
  void _lsl(Variable *Dest, Variable *Src0, Operand *Src1,
            CondARM32::Cond Pred = CondARM32::AL) {
    Context.insert(InstARM32Lsl::create(Func, Dest, Src0, Src1, Pred));
  }
  void _lsr(Variable *Dest, Variable *Src0, Operand *Src1,
            CondARM32::Cond Pred = CondARM32::AL) {
    Context.insert(InstARM32Lsr::create(Func, Dest, Src0, Src1, Pred));
  }
  void _mla(Variable *Dest, Variable *Src0, Variable *Src1, Variable *Acc,
            CondARM32::Cond Pred = CondARM32::AL) {
    Context.insert(InstARM32Mla::create(Func, Dest, Src0, Src1, Acc, Pred));
  }
  void _mls(Variable *Dest, Variable *Src0, Variable *Src1, Variable *Acc,
            CondARM32::Cond Pred = CondARM32::AL) {
    Context.insert(InstARM32Mls::create(Func, Dest, Src0, Src1, Acc, Pred));
  }
  /// _mov, for all your Variable to Variable data movement needs. It handles
  /// all types (integer, floating point, and vectors), as well as moves between
  /// Core and VFP registers. This is not a panacea: you must obey the (weird,
  /// confusing, non-uniform) rules for data moves in ARM.
  void _mov(Variable *Dest, Operand *Src0,
            CondARM32::Cond Pred = CondARM32::AL) {
    // _mov used to be unique in the sense that it would create a temporary
    // automagically if Dest was nullptr. It won't do that anymore, so we keep
    // an assert around just in case there is some untested code path where Dest
    // is nullptr.
    assert(Dest != nullptr);
    auto *Instr = InstARM32Mov::create(Func, Dest, Src0, Pred);

    Context.insert(Instr);
    if (Instr->isMultiDest()) {
      // If Instr is multi-dest, then Dest must be a Variable64On32. We add a
      // fake-def for Instr.DestHi here.
      assert(llvm::isa<Variable64On32>(Dest));
      Context.insert(InstFakeDef::create(Func, Instr->getDestHi()));
    }
  }

  void _mov_redefined(Variable *Dest, Operand *Src0,
                      CondARM32::Cond Pred = CondARM32::AL) {
    auto *Instr = InstARM32Mov::create(Func, Dest, Src0, Pred);
    Instr->setDestRedefined();
    Context.insert(Instr);
    if (Instr->isMultiDest()) {
      // If Instr is multi-dest, then Dest must be a Variable64On32. We add a
      // fake-def for Instr.DestHi here.
      assert(llvm::isa<Variable64On32>(Dest));
      Context.insert(InstFakeDef::create(Func, Instr->getDestHi()));
    }
  }

  // _mov_i1_to_flags is used for bool folding. If "Boolean" is folded, this
  // method returns true, and sets "CondIfTrue0" and "CondIfTrue1" to the
  // appropriate ARM condition codes. If "Boolean" is not to be folded, then
  // this
  // method returns false.
  bool _mov_i1_to_flags(Operand *Boolean, CondARM32::Cond *CondIfTrue0,
                        CondARM32::Cond *CondIfTrue1,
                        CondARM32::Cond *CondIfFalse);

  // _cmov is a pseudo instruction that is used for boolean folding. It emits
  // code that moves "SrcIfTrue" to dest if either "CondIfTrue0" or
  // "CondIfTrue1" holds, and "SrcIfFalse", if "CondIfFalse" holds. It requires
  // "Dest" to be an infinite-weight temporary.
  void _cmov(Variable *Dest, Operand *SrcIfTrue, CondARM32::Cond CondIfTrue0,
             CondARM32::Cond CondIfTrue1, Operand *SrcIfFalse,
             CondARM32::Cond CondIfFalse) {
    assert(Dest->mustHaveReg());

    if (CondIfFalse == CondARM32::kNone) {
      assert(CondIfTrue0 == CondARM32::AL);
      assert(CondIfTrue1 == CondARM32::kNone);
    }

    if (CondIfTrue0 == CondARM32::kNone) {
      assert(CondIfFalse == CondARM32::AL);
      assert(CondIfTrue1 == CondARM32::kNone);
    }

    if (CondIfTrue1 != CondARM32::kNone) {
      assert(CondIfFalse == CondARM32::AL);
      assert(CondIfTrue1 != CondARM32::kNone);
    }

    bool RedefineT = false;
    if (CondIfFalse != CondARM32::kNone) {
      _mov(Dest, SrcIfFalse, CondIfFalse);
      RedefineT = true;
    }

    if (CondIfTrue0 != CondARM32::kNone) {
      if (RedefineT) {
        _mov_redefined(Dest, SrcIfTrue, CondIfTrue0);
      } else {
        _mov(Dest, SrcIfTrue, CondIfTrue0);
      }
      RedefineT = true;
    }

    if (CondIfTrue1 != CondARM32::kNone) {
      assert(RedefineT);
      _mov_redefined(Dest, SrcIfTrue, CondIfTrue1);
    }
  }

  /// The Operand can only be a 16-bit immediate or a ConstantRelocatable (with
  /// an upper16 relocation).
  void _movt(Variable *Dest, Operand *Src0,
             CondARM32::Cond Pred = CondARM32::AL) {
    Context.insert(InstARM32Movt::create(Func, Dest, Src0, Pred));
  }
  void _movw(Variable *Dest, Operand *Src0,
             CondARM32::Cond Pred = CondARM32::AL) {
    Context.insert(InstARM32Movw::create(Func, Dest, Src0, Pred));
  }
  void _mul(Variable *Dest, Variable *Src0, Variable *Src1,
            CondARM32::Cond Pred = CondARM32::AL) {
    Context.insert(InstARM32Mul::create(Func, Dest, Src0, Src1, Pred));
  }
  void _mvn(Variable *Dest, Operand *Src0,
            CondARM32::Cond Pred = CondARM32::AL) {
    Context.insert(InstARM32Mvn::create(Func, Dest, Src0, Pred));
  }
  void _orr(Variable *Dest, Variable *Src0, Operand *Src1,
            CondARM32::Cond Pred = CondARM32::AL) {
    Context.insert(InstARM32Orr::create(Func, Dest, Src0, Src1, Pred));
  }
  void _orrs(Variable *Dest, Variable *Src0, Operand *Src1,
             CondARM32::Cond Pred = CondARM32::AL) {
    constexpr bool SetFlags = true;
    Context.insert(
        InstARM32Orr::create(Func, Dest, Src0, Src1, Pred, SetFlags));
  }
  void _push(const VarList &Sources) {
    Context.insert(InstARM32Push::create(Func, Sources));
  }
  void _pop(const VarList &Dests) {
    Context.insert(InstARM32Pop::create(Func, Dests));
    // Mark dests as modified.
    for (Variable *Dest : Dests)
      Context.insert(InstFakeDef::create(Func, Dest));
  }
  void _rbit(Variable *Dest, Variable *Src0,
             CondARM32::Cond Pred = CondARM32::AL) {
    Context.insert(InstARM32Rbit::create(Func, Dest, Src0, Pred));
  }
  void _rev(Variable *Dest, Variable *Src0,
            CondARM32::Cond Pred = CondARM32::AL) {
    Context.insert(InstARM32Rev::create(Func, Dest, Src0, Pred));
  }
  void _ret(Variable *LR, Variable *Src0 = nullptr) {
    Context.insert(InstARM32Ret::create(Func, LR, Src0));
  }
  void _rsb(Variable *Dest, Variable *Src0, Operand *Src1,
            CondARM32::Cond Pred = CondARM32::AL) {
    Context.insert(InstARM32Rsb::create(Func, Dest, Src0, Src1, Pred));
  }
  void _sbc(Variable *Dest, Variable *Src0, Operand *Src1,
            CondARM32::Cond Pred = CondARM32::AL) {
    Context.insert(InstARM32Sbc::create(Func, Dest, Src0, Src1, Pred));
  }
  void _sbcs(Variable *Dest, Variable *Src0, Operand *Src1,
             CondARM32::Cond Pred = CondARM32::AL) {
    constexpr bool SetFlags = true;
    Context.insert(
        InstARM32Sbc::create(Func, Dest, Src0, Src1, Pred, SetFlags));
  }
  void _sdiv(Variable *Dest, Variable *Src0, Variable *Src1,
             CondARM32::Cond Pred = CondARM32::AL) {
    Context.insert(InstARM32Sdiv::create(Func, Dest, Src0, Src1, Pred));
  }
  /// _str, for all your Variable to memory transfers. Addr has the same
  /// restrictions that it does in _ldr.
  void _str(Variable *Value, OperandARM32Mem *Addr,
            CondARM32::Cond Pred = CondARM32::AL) {
    Context.insert(InstARM32Str::create(Func, Value, Addr, Pred));
  }
  void _strex(Variable *Dest, Variable *Value, OperandARM32Mem *Addr,
              CondARM32::Cond Pred = CondARM32::AL) {
    // strex requires Dest to be a register other than Value or Addr. This
    // restriction is cleanly represented by adding an "early" definition of
    // Dest (or a latter use of all the sources.)
    Context.insert(InstFakeDef::create(Func, Dest));
    if (auto *Value64 = llvm::dyn_cast<Variable64On32>(Value)) {
      Context.insert(InstFakeUse::create(Func, Value64->getLo()));
      Context.insert(InstFakeUse::create(Func, Value64->getHi()));
    }
    auto *Instr = InstARM32Strex::create(Func, Dest, Value, Addr, Pred);
    Context.insert(Instr);
    Instr->setDestRedefined();
  }
  void _sub(Variable *Dest, Variable *Src0, Operand *Src1,
            CondARM32::Cond Pred = CondARM32::AL) {
    Context.insert(InstARM32Sub::create(Func, Dest, Src0, Src1, Pred));
  }
  void _subs(Variable *Dest, Variable *Src0, Operand *Src1,
             CondARM32::Cond Pred = CondARM32::AL) {
    constexpr bool SetFlags = true;
    Context.insert(
        InstARM32Sub::create(Func, Dest, Src0, Src1, Pred, SetFlags));
  }
  void _sxt(Variable *Dest, Variable *Src0,
            CondARM32::Cond Pred = CondARM32::AL) {
    Context.insert(InstARM32Sxt::create(Func, Dest, Src0, Pred));
  }
  void _tst(Variable *Src0, Operand *Src1,
            CondARM32::Cond Pred = CondARM32::AL) {
    Context.insert(InstARM32Tst::create(Func, Src0, Src1, Pred));
  }
  void _trap() { Context.insert(InstARM32Trap::create(Func)); }
  void _udiv(Variable *Dest, Variable *Src0, Variable *Src1,
             CondARM32::Cond Pred = CondARM32::AL) {
    Context.insert(InstARM32Udiv::create(Func, Dest, Src0, Src1, Pred));
  }
  void _umull(Variable *DestLo, Variable *DestHi, Variable *Src0,
              Variable *Src1, CondARM32::Cond Pred = CondARM32::AL) {
    Context.insert(
        InstARM32Umull::create(Func, DestLo, DestHi, Src0, Src1, Pred));
    // Model the modification to the second dest as a fake def. Note that the
    // def is not predicated.
    Context.insert(InstFakeDef::create(Func, DestHi, DestLo));
  }
  void _uxt(Variable *Dest, Variable *Src0,
            CondARM32::Cond Pred = CondARM32::AL) {
    Context.insert(InstARM32Uxt::create(Func, Dest, Src0, Pred));
  }
  void _vabs(Variable *Dest, Variable *Src,
             CondARM32::Cond Pred = CondARM32::AL) {
    Context.insert(InstARM32Vabs::create(Func, Dest, Src, Pred));
  }
  void _vadd(Variable *Dest, Variable *Src0, Variable *Src1) {
    Context.insert(InstARM32Vadd::create(Func, Dest, Src0, Src1));
  }
  void _vcvt(Variable *Dest, Variable *Src, InstARM32Vcvt::VcvtVariant Variant,
             CondARM32::Cond Pred = CondARM32::AL) {
    Context.insert(InstARM32Vcvt::create(Func, Dest, Src, Variant, Pred));
  }
  void _vdiv(Variable *Dest, Variable *Src0, Variable *Src1) {
    Context.insert(InstARM32Vdiv::create(Func, Dest, Src0, Src1));
  }
  void _vcmp(Variable *Src0, Variable *Src1,
             CondARM32::Cond Pred = CondARM32::AL) {
    Context.insert(InstARM32Vcmp::create(Func, Src0, Src1, Pred));
  }
  void _vmrs(CondARM32::Cond Pred = CondARM32::AL) {
    Context.insert(InstARM32Vmrs::create(Func, Pred));
  }
  void _vmul(Variable *Dest, Variable *Src0, Variable *Src1) {
    Context.insert(InstARM32Vmul::create(Func, Dest, Src0, Src1));
  }
  void _vsqrt(Variable *Dest, Variable *Src,
              CondARM32::Cond Pred = CondARM32::AL) {
    Context.insert(InstARM32Vsqrt::create(Func, Dest, Src, Pred));
  }
  void _vsub(Variable *Dest, Variable *Src0, Variable *Src1) {
    Context.insert(InstARM32Vsub::create(Func, Dest, Src0, Src1));
  }

  /// Run a pass through stack variables and ensure that the offsets are legal.
  /// If the offset is not legal, use a new base register that accounts for the
  /// offset, such that the addressing mode offset bits are now legal.
  void legalizeStackSlots();
  /// Returns true if the given Offset can be represented in a stack ldr/str.
  bool isLegalVariableStackOffset(Type Ty, int32_t Offset) const;
  /// Assuming Var needs its offset legalized, define a new base register
  /// centered on the given Var's offset plus StackAdjust, and use it.
  StackVariable *legalizeVariableSlot(Variable *Var, int32_t StackAdjust,
                                      Variable *OrigBaseReg);

  TargetARM32Features CPUFeatures;
  bool UsesFramePointer = false;
  bool NeedsStackAlignment = false;
  bool MaybeLeafFunc = true;
  size_t SpillAreaSizeBytes = 0;
  // TODO(jpp): std::array instead of array.
  static llvm::SmallBitVector TypeToRegisterSet[IceType_NUM];
  static llvm::SmallBitVector RegisterAliases[RegARM32::Reg_NUM];
  static llvm::SmallBitVector ScratchRegs;
  llvm::SmallBitVector RegsUsed;
  VarList PhysicalRegisters[IceType_NUM];

  /// Helper class that understands the Calling Convention and register
  /// assignments. The first few integer type parameters can use r0-r3,
  /// regardless of their position relative to the floating-point/vector
  /// arguments in the argument list. Floating-point and vector arguments
  /// can use q0-q3 (aka d0-d7, s0-s15). For more information on the topic,
  /// see the ARM Architecture Procedure Calling Standards (AAPCS).
  ///
  /// Technically, arguments that can start with registers but extend beyond the
  /// available registers can be split between the registers and the stack.
  /// However, this is typically  for passing GPR structs by value, and PNaCl
  /// transforms expand this out.
  ///
  /// At (public) function entry, the stack must be 8-byte aligned.
  class CallingConv {
    CallingConv(const CallingConv &) = delete;
    CallingConv &operator=(const CallingConv &) = delete;

  public:
    CallingConv()
        : VFPRegsFree(ARM32_MAX_FP_REG_UNITS, true),
          ValidF64Regs(ARM32_MAX_FP_REG_UNITS),
          ValidV128Regs(ARM32_MAX_FP_REG_UNITS) {
      for (uint32_t i = 0; i < ARM32_MAX_FP_REG_UNITS; ++i) {
        if ((i % 2) == 0) {
          ValidF64Regs[i] = true;
        }
        if ((i % 4) == 0) {
          ValidV128Regs[i] = true;
        }
      }
    }
    ~CallingConv() = default;

    bool I64InRegs(std::pair<int32_t, int32_t> *Regs);
    bool I32InReg(int32_t *Reg);
    bool FPInReg(Type Ty, int32_t *Reg);

    static constexpr uint32_t ARM32_MAX_GPR_ARG = 4;
    // TODO(jpp): comment.
    static constexpr uint32_t ARM32_MAX_FP_REG_UNITS = 16;

  private:
    uint32_t NumGPRRegsUsed = 0;
    llvm::SmallBitVector VFPRegsFree;
    llvm::SmallBitVector ValidF64Regs;
    llvm::SmallBitVector ValidV128Regs;
  };

private:
  ~TargetARM32() override = default;

  void lowerTruncToFlags(Operand *Src, CondARM32::Cond *CondIfTrue,
                         CondARM32::Cond *CondIfFalse);
  class BoolComputationTracker {
  public:
    BoolComputationTracker() = default;
    ~BoolComputationTracker() = default;

    void forgetProducers() { KnownComputations.clear(); }
    void recordProducers(CfgNode *Node);

    const Inst *getProducerOf(const Operand *Opnd) const {
      auto *Var = llvm::dyn_cast<Variable>(Opnd);
      if (Var == nullptr) {
        return nullptr;
      }

      auto Iter = KnownComputations.find(Var->getIndex());
      if (Iter == KnownComputations.end()) {
        return nullptr;
      }

      return Iter->second.Instr;
    }

    void dump(const Cfg *Func) const {
      if (!BuildDefs::dump() || !Func->isVerbose(IceV_Folding))
        return;
      OstreamLocker L(Func->getContext());
      Ostream &Str = Func->getContext()->getStrDump();
      Str << "foldable producer:\n  ";
      for (const auto &Computation : KnownComputations) {
        Str << "    ";
        Computation.second.Instr->dump(Func);
        Str << "\n";
      }
      Str << "\n";
    }

  private:
    class BoolComputationEntry {
    public:
      explicit BoolComputationEntry(Inst *I) : Instr(I) {}
      Inst *const Instr;
      // Boolean folding is disabled for variables whose live range is multi
      // block. We conservatively initialize IsLiveOut to true, and set it to
      // false once we find the end of the live range for the variable defined
      // by this instruction. If liveness analysis is not performed (e.g., in
      // Om1 mode) IsLiveOut will never be set to false, and folding will be
      // disabled.
      bool IsLiveOut = true;
    };

    using BoolComputationMap = std::unordered_map<SizeT, BoolComputationEntry>;
    BoolComputationMap KnownComputations;
  };

  BoolComputationTracker BoolComputations;
};

class TargetDataARM32 final : public TargetDataLowering {
  TargetDataARM32() = delete;
  TargetDataARM32(const TargetDataARM32 &) = delete;
  TargetDataARM32 &operator=(const TargetDataARM32 &) = delete;

public:
  static std::unique_ptr<TargetDataLowering> create(GlobalContext *Ctx) {
    return std::unique_ptr<TargetDataLowering>(new TargetDataARM32(Ctx));
  }

  void lowerGlobals(const VariableDeclarationList &Vars,
                    const IceString &SectionSuffix) override;
  void lowerConstants() override;
  void lowerJumpTables() override;

protected:
  explicit TargetDataARM32(GlobalContext *Ctx);

private:
  ~TargetDataARM32() override = default;
};

class TargetHeaderARM32 final : public TargetHeaderLowering {
  TargetHeaderARM32() = delete;
  TargetHeaderARM32(const TargetHeaderARM32 &) = delete;
  TargetHeaderARM32 &operator=(const TargetHeaderARM32 &) = delete;

public:
  static std::unique_ptr<TargetHeaderLowering> create(GlobalContext *Ctx) {
    return std::unique_ptr<TargetHeaderLowering>(new TargetHeaderARM32(Ctx));
  }

  void lower() override;

protected:
  explicit TargetHeaderARM32(GlobalContext *Ctx);

private:
  ~TargetHeaderARM32() = default;

  TargetARM32Features CPUFeatures;
};

} // end of namespace Ice

#endif // SUBZERO_SRC_ICETARGETLOWERINGARM32_H
