//===- subzero/src/IceTargetLoweringMIPS32.h - MIPS32 lowering ---*- C++-*-===//
//
//                        The Subzero Code Generator
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief Declares the TargetLoweringMIPS32 class, which implements the
/// TargetLowering interface for the MIPS 32-bit architecture.
///
//===----------------------------------------------------------------------===//

#ifndef SUBZERO_SRC_ICETARGETLOWERINGMIPS32_H
#define SUBZERO_SRC_ICETARGETLOWERINGMIPS32_H

#include "IceAssemblerMIPS32.h"
#include "IceDefs.h"
#include "IceInstMIPS32.h"
#include "IceRegistersMIPS32.h"
#include "IceTargetLowering.h"

namespace Ice {
namespace MIPS32 {

class TargetMIPS32 : public TargetLowering {
  TargetMIPS32() = delete;
  TargetMIPS32(const TargetMIPS32 &) = delete;
  TargetMIPS32 &operator=(const TargetMIPS32 &) = delete;

public:
  ~TargetMIPS32() override = default;

  static void staticInit(GlobalContext *Ctx);
  static std::unique_ptr<::Ice::TargetLowering> create(Cfg *Func) {
    return makeUnique<TargetMIPS32>(Func);
  }

  std::unique_ptr<::Ice::Assembler> createAssembler() const override {
    return makeUnique<MIPS32::AssemblerMIPS32>();
  }

  void translateOm1() override;
  void translateO2() override;
  bool doBranchOpt(Inst *I, const CfgNode *NextNode) override;

  SizeT getNumRegisters() const override { return RegMIPS32::Reg_NUM; }
  Variable *getPhysicalRegister(SizeT RegNum, Type Ty = IceType_void) override;
  IceString getRegName(SizeT RegNum, Type Ty) const override;
  llvm::SmallBitVector getRegisterSet(RegSetMask Include,
                                      RegSetMask Exclude) const override;
  const llvm::SmallBitVector &
  getRegistersForVariable(const Variable *Var) const override {
    RegClass RC = Var->getRegClass();
    assert(RC < RC_Target);
    return TypeToRegisterSet[RC];
  }
  const llvm::SmallBitVector &getAliasesForRegister(SizeT Reg) const override {
    return RegisterAliases[Reg];
  }
  bool hasFramePointer() const override { return UsesFramePointer; }
  void setHasFramePointer() override { UsesFramePointer = true; }
  SizeT getStackReg() const override { return RegMIPS32::Reg_SP; }
  SizeT getFrameReg() const override { return RegMIPS32::Reg_FP; }
  SizeT getFrameOrStackReg() const override {
    return UsesFramePointer ? getFrameReg() : getStackReg();
  }
  size_t typeWidthInBytesOnStack(Type Ty) const override {
    // Round up to the next multiple of 4 bytes. In particular, i1, i8, and i16
    // are rounded up to 4 bytes.
    return (typeWidthInBytes(Ty) + 3) & ~3;
  }
  uint32_t getStackAlignment() const override {
    // TODO(sehr): what is the stack alignment?
    return 1;
  }
  void reserveFixedAllocaArea(size_t Size, size_t Align) override {
    // TODO(sehr): Implement fixed stack layout.
    (void)Size;
    (void)Align;
    llvm::report_fatal_error("Not yet implemented");
  }
  int32_t getFrameFixedAllocaOffset() const override {
    // TODO(sehr): Implement fixed stack layout.
    llvm::report_fatal_error("Not yet implemented");
    return 0;
  }

  bool shouldSplitToVariable64On32(Type Ty) const override {
    return Ty == IceType_i64;
  }

  // TODO(ascull): what is the best size of MIPS?
  SizeT getMinJumpTableSize() const override { return 3; }
  void emitJumpTable(const Cfg *Func,
                     const InstJumpTable *JumpTable) const override;

  void emitVariable(const Variable *Var) const override;

  void emit(const ConstantInteger32 *C) const final {
    (void)C;
    llvm::report_fatal_error("Not yet implemented");
  }
  void emit(const ConstantInteger64 *C) const final {
    (void)C;
    llvm::report_fatal_error("Not yet implemented");
  }
  void emit(const ConstantFloat *C) const final {
    (void)C;
    llvm::report_fatal_error("Not yet implemented");
  }
  void emit(const ConstantDouble *C) const final {
    (void)C;
    llvm::report_fatal_error("Not yet implemented");
  }
  void emit(const ConstantUndef *C) const final {
    (void)C;
    llvm::report_fatal_error("Not yet implemented");
  }
  void emit(const ConstantRelocatable *C) const final {
    (void)C;
    llvm::report_fatal_error("Not yet implemented");
  }

  // The following are helpers that insert lowered MIPS32 instructions with
  // minimal syntactic overhead, so that the lowering code can look as close to
  // assembly as practical.
  void _add(Variable *Dest, Variable *Src0, Variable *Src1) {
    Context.insert<InstMIPS32Add>(Dest, Src0, Src1);
  }

  void _and(Variable *Dest, Variable *Src0, Variable *Src1) {
    Context.insert<InstMIPS32And>(Dest, Src0, Src1);
  }

  void _ret(Variable *RA, Variable *Src0 = nullptr) {
    Context.insert<InstMIPS32Ret>(RA, Src0);
  }

  void _addiu(Variable *Dest, Variable *Src, uint32_t Imm) {
    Context.insert<InstMIPS32Addiu>(Dest, Src, Imm);
  }

  void _lui(Variable *Dest, uint32_t Imm) {
    Context.insert<InstMIPS32Lui>(Dest, Imm);
  }

  void _mov(Variable *Dest, Operand *Src0) {
    assert(Dest != nullptr);
    // Variable* Src0_ = llvm::dyn_cast<Variable>(Src0);
    if (llvm::isa<ConstantRelocatable>(Src0)) {
      Context.insert<InstMIPS32La>(Dest, Src0);
    } else {
      auto *Instr = Context.insert<InstMIPS32Mov>(Dest, Src0);
      if (Instr->isMultiDest()) {
        // If Instr is multi-dest, then Dest must be a Variable64On32. We add a
        // fake-def for Instr.DestHi here.
        assert(llvm::isa<Variable64On32>(Dest));
        Context.insert<InstFakeDef>(Instr->getDestHi());
      }
    }
  }

  void _mul(Variable *Dest, Variable *Src0, Variable *Src1) {
    Context.insert<InstMIPS32Mul>(Dest, Src0, Src1);
  }

  void _or(Variable *Dest, Variable *Src0, Variable *Src1) {
    Context.insert<InstMIPS32Or>(Dest, Src0, Src1);
  }

  void _ori(Variable *Dest, Variable *Src, uint32_t Imm) {
    Context.insert<InstMIPS32Ori>(Dest, Src, Imm);
  }

  void _sub(Variable *Dest, Variable *Src0, Variable *Src1) {
    Context.insert<InstMIPS32Sub>(Dest, Src0, Src1);
  }

  void _xor(Variable *Dest, Variable *Src0, Variable *Src1) {
    Context.insert<InstMIPS32Xor>(Dest, Src0, Src1);
  }

  void lowerArguments() override;

  /// Operand legalization helpers.  To deal with address mode constraints,
  /// the helpers will create a new Operand and emit instructions that
  /// guarantee that the Operand kind is one of those indicated by the
  /// LegalMask (a bitmask of allowed kinds).  If the input Operand is known
  /// to already meet the constraints, it may be simply returned as the result,
  /// without creating any new instructions or operands.
  enum OperandLegalization {
    Legal_None = 0,
    Legal_Reg = 1 << 0, // physical register, not stack location
    Legal_Imm = 1 << 1,
    Legal_Mem = 1 << 2,
    Legal_Default = ~Legal_None
  };
  typedef uint32_t LegalMask;
  Operand *legalize(Operand *From, LegalMask Allowed = Legal_Default,
                    int32_t RegNum = Variable::NoRegister);

  Variable *legalizeToVar(Operand *From, int32_t RegNum = Variable::NoRegister);

  Variable *legalizeToReg(Operand *From, int32_t RegNum = Variable::NoRegister);

  Variable *makeReg(Type Ty, int32_t RegNum = Variable::NoRegister);
  static Type stackSlotType();
  Variable *copyToReg(Operand *Src, int32_t RegNum = Variable::NoRegister);

  void addProlog(CfgNode *Node) override;
  void addEpilog(CfgNode *Node) override;

  // Ensure that a 64-bit Variable has been split into 2 32-bit
  // Variables, creating them if necessary.  This is needed for all
  // I64 operations.
  void split64(Variable *Var);
  Operand *loOperand(Operand *Operand);
  Operand *hiOperand(Operand *Operand);

  Operand *legalizeUndef(Operand *From, int32_t RegNum = Variable::NoRegister);

protected:
  explicit TargetMIPS32(Cfg *Func);

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
  uint32_t getCallStackArgumentsSizeBytes(const InstCall *Instr) override {
    (void)Instr;
    return 0;
  }
  void genTargetHelperCallFor(Inst *Instr) override { (void)Instr; }
  void doAddressOptLoad() override;
  void doAddressOptStore() override;
  void randomlyInsertNop(float Probability,
                         RandomNumberGenerator &RNG) override;
  void
  makeRandomRegisterPermutation(llvm::SmallVectorImpl<int32_t> &Permutation,
                                const llvm::SmallBitVector &ExcludeRegisters,
                                uint64_t Salt) const override;

  bool UsesFramePointer = false;
  bool NeedsStackAlignment = false;
  static llvm::SmallBitVector TypeToRegisterSet[RCMIPS32_NUM];
  static llvm::SmallBitVector RegisterAliases[RegMIPS32::Reg_NUM];
  llvm::SmallBitVector RegsUsed;
  VarList PhysicalRegisters[IceType_NUM];

private:
  ENABLE_MAKE_UNIQUE;
};

class TargetDataMIPS32 final : public TargetDataLowering {
  TargetDataMIPS32() = delete;
  TargetDataMIPS32(const TargetDataMIPS32 &) = delete;
  TargetDataMIPS32 &operator=(const TargetDataMIPS32 &) = delete;

public:
  static std::unique_ptr<TargetDataLowering> create(GlobalContext *Ctx) {
    return std::unique_ptr<TargetDataLowering>(new TargetDataMIPS32(Ctx));
  }

  void lowerGlobals(const VariableDeclarationList &Vars,
                    const IceString &SectionSuffix) override;
  void lowerConstants() override;
  void lowerJumpTables() override;

protected:
  explicit TargetDataMIPS32(GlobalContext *Ctx);

private:
  ~TargetDataMIPS32() override = default;
  template <typename T> static void emitConstantPool(GlobalContext *Ctx);
};

class TargetHeaderMIPS32 final : public TargetHeaderLowering {
  TargetHeaderMIPS32() = delete;
  TargetHeaderMIPS32(const TargetHeaderMIPS32 &) = delete;
  TargetHeaderMIPS32 &operator=(const TargetHeaderMIPS32 &) = delete;

public:
  static std::unique_ptr<TargetHeaderLowering> create(GlobalContext *Ctx) {
    return std::unique_ptr<TargetHeaderLowering>(new TargetHeaderMIPS32(Ctx));
  }

  void lower() override;

protected:
  explicit TargetHeaderMIPS32(GlobalContext *Ctx);

private:
  ~TargetHeaderMIPS32() = default;
};

} // end of namespace MIPS32
} // end of namespace Ice

#endif // SUBZERO_SRC_ICETARGETLOWERINGMIPS32_H
