//===- subzero/src/IceTargetLoweringARM32.h - ARM32 lowering ----*- C++ -*-===//
//
//                        The Subzero Code Generator
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares the TargetLoweringARM32 class, which implements the
// TargetLowering interface for the ARM 32-bit architecture.
//
//===----------------------------------------------------------------------===//

#ifndef SUBZERO_SRC_ICETARGETLOWERINGARM32_H
#define SUBZERO_SRC_ICETARGETLOWERINGARM32_H

#include "IceDefs.h"
#include "IceInstARM32.h"
#include "IceRegistersARM32.h"
#include "IceTargetLowering.h"

namespace Ice {

class TargetARM32 : public TargetLowering {
  TargetARM32() = delete;
  TargetARM32(const TargetARM32 &) = delete;
  TargetARM32 &operator=(const TargetARM32 &) = delete;

public:
  // TODO(jvoung): return a unique_ptr.
  static TargetARM32 *create(Cfg *Func) { return new TargetARM32(Func); }

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
  bool hasFramePointer() const override { return UsesFramePointer; }
  SizeT getFrameOrStackReg() const override {
    return UsesFramePointer ? RegARM32::Reg_fp : RegARM32::Reg_sp;
  }
  size_t typeWidthInBytesOnStack(Type Ty) const override {
    // Round up to the next multiple of 4 bytes.  In particular, i1,
    // i8, and i16 are rounded up to 4 bytes.
    return (typeWidthInBytes(Ty) + 3) & ~3;
  }

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

  // Ensure that a 64-bit Variable has been split into 2 32-bit
  // Variables, creating them if necessary.  This is needed for all
  // I64 operations.
  void split64(Variable *Var);
  Operand *loOperand(Operand *Operand);
  Operand *hiOperand(Operand *Operand);
  void finishArgumentLowering(Variable *Arg, Variable *FramePtr,
                              size_t BasicFrameOffset, size_t &InArgsSizeBytes);

  enum ARM32InstructionSet {
    Begin,
    // Neon is the PNaCl baseline instruction set.
    Neon = Begin,
    HWDivArm, // HW divide in ARM mode (not just Thumb mode).
    End
  };

  ARM32InstructionSet getInstructionSet() const { return InstructionSet; }

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
  void prelowerPhis() override;
  void lowerPhiAssignments(CfgNode *Node,
                           const AssignList &Assignments) override;
  void doAddressOptLoad() override;
  void doAddressOptStore() override;
  void randomlyInsertNop(float Probability) override;

  enum OperandLegalization {
    Legal_None = 0,
    Legal_Reg = 1 << 0,  // physical register, not stack location
    Legal_Flex = 1 << 1, // A flexible operand2, which can hold rotated
                         // small immediates, or shifted registers.
    Legal_Mem = 1 << 2,  // includes [r0, r1 lsl #2] as well as [sp, #12]
    Legal_All = ~Legal_None
  };
  typedef uint32_t LegalMask;
  Operand *legalize(Operand *From, LegalMask Allowed = Legal_All,
                    int32_t RegNum = Variable::NoRegister);
  Variable *legalizeToVar(Operand *From, int32_t RegNum = Variable::NoRegister);
  OperandARM32Mem *formMemoryOperand(Operand *Ptr, Type Ty);

  Variable *makeReg(Type Ty, int32_t RegNum = Variable::NoRegister);
  static Type stackSlotType();
  Variable *copyToReg(Operand *Src, int32_t RegNum = Variable::NoRegister);
  void alignRegisterPow2(Variable *Reg, uint32_t Align);

  // Returns a vector in a register with the given constant entries.
  Variable *makeVectorOfZeros(Type Ty, int32_t RegNum = Variable::NoRegister);

  void makeRandomRegisterPermutation(
      llvm::SmallVectorImpl<int32_t> &Permutation,
      const llvm::SmallBitVector &ExcludeRegisters) const override;

  // The following are helpers that insert lowered ARM32 instructions
  // with minimal syntactic overhead, so that the lowering code can
  // look as close to assembly as practical.

  void _add(Variable *Dest, Variable *Src0, Operand *Src1,
            CondARM32::Cond Pred = CondARM32::AL) {
    Context.insert(InstARM32Add::create(Func, Dest, Src0, Src1, Pred));
  }
  void _adds(Variable *Dest, Variable *Src0, Operand *Src1,
             CondARM32::Cond Pred = CondARM32::AL) {
    const bool SetFlags = true;
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
  void _br(CondARM32::Cond Condition, CfgNode *TargetTrue,
           CfgNode *TargetFalse) {
    Context.insert(
        InstARM32Br::create(Func, TargetTrue, TargetFalse, Condition));
  }
  void _br(CfgNode *Target) {
    Context.insert(InstARM32Br::create(Func, Target));
  }
  void _cmp(Variable *Src0, Operand *Src1,
            CondARM32::Cond Pred = CondARM32::AL) {
    Context.insert(InstARM32Cmp::create(Func, Src0, Src1, Pred));
  }
  void _eor(Variable *Dest, Variable *Src0, Operand *Src1,
            CondARM32::Cond Pred = CondARM32::AL) {
    Context.insert(InstARM32Eor::create(Func, Dest, Src0, Src1, Pred));
  }
  void _ldr(Variable *Dest, OperandARM32Mem *Addr,
            CondARM32::Cond Pred = CondARM32::AL) {
    Context.insert(InstARM32Ldr::create(Func, Dest, Addr, Pred));
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
  // If Dest=nullptr is passed in, then a new variable is created,
  // marked as infinite register allocation weight, and returned
  // through the in/out Dest argument.
  void _mov(Variable *&Dest, Operand *Src0,
            CondARM32::Cond Pred = CondARM32::AL,
            int32_t RegNum = Variable::NoRegister) {
    if (Dest == nullptr)
      Dest = makeReg(Src0->getType(), RegNum);
    Context.insert(InstARM32Mov::create(Func, Dest, Src0, Pred));
  }
  void _mov_nonkillable(Variable *Dest, Operand *Src0,
                        CondARM32::Cond Pred = CondARM32::AL) {
    Inst *NewInst = InstARM32Mov::create(Func, Dest, Src0, Pred);
    NewInst->setDestNonKillable();
    Context.insert(NewInst);
  }
  // The Operand can only be a 16-bit immediate or a ConstantRelocatable
  // (with an upper16 relocation).
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
  void _push(const VarList &Sources) {
    Context.insert(InstARM32Push::create(Func, Sources));
  }
  void _pop(const VarList &Dests) {
    Context.insert(InstARM32Pop::create(Func, Dests));
    // Mark dests as modified.
    for (Variable *Dest : Dests)
      Context.insert(InstFakeDef::create(Func, Dest));
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
    const bool SetFlags = true;
    Context.insert(
        InstARM32Sbc::create(Func, Dest, Src0, Src1, Pred, SetFlags));
  }
  void _str(Variable *Value, OperandARM32Mem *Addr,
            CondARM32::Cond Pred = CondARM32::AL) {
    Context.insert(InstARM32Str::create(Func, Value, Addr, Pred));
  }
  void _sub(Variable *Dest, Variable *Src0, Operand *Src1,
            CondARM32::Cond Pred = CondARM32::AL) {
    Context.insert(InstARM32Sub::create(Func, Dest, Src0, Src1, Pred));
  }
  void _subs(Variable *Dest, Variable *Src0, Operand *Src1,
             CondARM32::Cond Pred = CondARM32::AL) {
    const bool SetFlags = true;
    Context.insert(
        InstARM32Sub::create(Func, Dest, Src0, Src1, Pred, SetFlags));
  }
  void _sxt(Variable *Dest, Variable *Src0,
            CondARM32::Cond Pred = CondARM32::AL) {
    Context.insert(InstARM32Sxt::create(Func, Dest, Src0, Pred));
  }
  void _ret(Variable *LR, Variable *Src0 = nullptr) {
    Context.insert(InstARM32Ret::create(Func, LR, Src0));
  }
  void _umull(Variable *DestLo, Variable *DestHi, Variable *Src0,
              Variable *Src1, CondARM32::Cond Pred = CondARM32::AL) {
    Context.insert(
        InstARM32Umull::create(Func, DestLo, DestHi, Src0, Src1, Pred));
    // Model the modification to the second dest as a fake def.
    // Note that the def is not predicated.
    Context.insert(InstFakeDef::create(Func, DestHi, DestLo));
  }
  void _uxt(Variable *Dest, Variable *Src0,
            CondARM32::Cond Pred = CondARM32::AL) {
    Context.insert(InstARM32Uxt::create(Func, Dest, Src0, Pred));
  }

  ARM32InstructionSet InstructionSet = ARM32InstructionSet::Begin;
  bool UsesFramePointer = false;
  bool NeedsStackAlignment = false;
  bool MaybeLeafFunc = true;
  size_t SpillAreaSizeBytes = 0;
  llvm::SmallBitVector TypeToRegisterSet[IceType_NUM];
  llvm::SmallBitVector ScratchRegs;
  llvm::SmallBitVector RegsUsed;
  VarList PhysicalRegisters[IceType_NUM];
  static IceString RegNames[];

  // Helper class that understands the Calling Convention and register
  // assignments. The first few integer type parameters can use r0-r3,
  // regardless of their position relative to the floating-point/vector
  // arguments in the argument list. Floating-point and vector arguments
  // can use q0-q3 (aka d0-d7, s0-s15). Technically, arguments that can
  // start with registers but extend beyond the available registers can be
  // split between the registers and the stack. However, this is typically
  // for passing GPR structs by value, and PNaCl transforms expand this out.
  //
  // Also, at the point before the call, the stack must be aligned.
  class CallingConv {
    CallingConv(const CallingConv &) = delete;
    CallingConv &operator=(const CallingConv &) = delete;

  public:
    CallingConv() : NumGPRRegsUsed(0) {}
    ~CallingConv() = default;

    bool I64InRegs(std::pair<int32_t, int32_t> *Regs);
    bool I32InReg(int32_t *Reg);

    static constexpr uint32_t ARM32_MAX_GPR_ARG = 4;

  private:
    uint32_t NumGPRRegsUsed;
  };

private:
  ~TargetARM32() override = default;
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

protected:
  explicit TargetDataARM32(GlobalContext *Ctx);

private:
  ~TargetDataARM32() override = default;
  template <typename T> static void emitConstantPool(GlobalContext *Ctx);
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
};

} // end of namespace Ice

#endif // SUBZERO_SRC_ICETARGETLOWERINGARM32_H
