//===- subzero/src/IceTargetLoweringMIPS32.h - MIPS32 lowering ---*- C++-*-===//
//
//                        The Subzero Code Generator
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares the TargetLoweringMIPS32 class, which implements the
// TargetLowering interface for the MIPS 32-bit architecture.
//
//===----------------------------------------------------------------------===//

#ifndef SUBZERO_SRC_ICETARGETLOWERINGMIPS32_H
#define SUBZERO_SRC_ICETARGETLOWERINGMIPS32_H

#include "IceDefs.h"
#include "IceInstMIPS32.h"
#include "IceRegistersMIPS32.h"
#include "IceTargetLowering.h"

namespace Ice {

class TargetMIPS32 : public TargetLowering {
  TargetMIPS32() = delete;
  TargetMIPS32(const TargetMIPS32 &) = delete;
  TargetMIPS32 &operator=(const TargetMIPS32 &) = delete;

public:
  // TODO(jvoung): return a unique_ptr.
  static TargetMIPS32 *create(Cfg *Func) { return new TargetMIPS32(Func); }

  void translateOm1() override;
  void translateO2() override;
  bool doBranchOpt(Inst *I, const CfgNode *NextNode) override;

  SizeT getNumRegisters() const override { return RegMIPS32::Reg_NUM; }
  Variable *getPhysicalRegister(SizeT RegNum, Type Ty = IceType_void) override;
  IceString getRegName(SizeT RegNum, Type Ty) const override;
  llvm::SmallBitVector getRegisterSet(RegSetMask Include,
                                      RegSetMask Exclude) const override;
  const llvm::SmallBitVector &getRegisterSetForType(Type Ty) const override {
    return TypeToRegisterSet[Ty];
  }
  bool hasFramePointer() const override { return UsesFramePointer; }
  SizeT getFrameOrStackReg() const override {
    return UsesFramePointer ? RegMIPS32::Reg_FP : RegMIPS32::Reg_SP;
  }
  size_t typeWidthInBytesOnStack(Type Ty) const override {
    // Round up to the next multiple of 4 bytes.  In particular, i1,
    // i8, and i16 are rounded up to 4 bytes.
    return (typeWidthInBytes(Ty) + 3) & ~3;
  }
  void emitVariable(const Variable *Var) const override;

  const char *getConstantPrefix() const final { return ""; }
  void emit(const ConstantUndef *C) const final {
    (void)C;
    llvm::report_fatal_error("Not yet implemented");
  }
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

  void lowerArguments() override;
  void addProlog(CfgNode *Node) override;
  void addEpilog(CfgNode *Node) override;

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
  void lowerPhiAssignments(CfgNode *Node,
                           const AssignList &Assignments) override;
  void doAddressOptLoad() override;
  void doAddressOptStore() override;
  void randomlyInsertNop(float Probability) override;
  void makeRandomRegisterPermutation(
      llvm::SmallVectorImpl<int32_t> &Permutation,
      const llvm::SmallBitVector &ExcludeRegisters) const override;

  static Type stackSlotType();

  bool UsesFramePointer;
  bool NeedsStackAlignment;
  llvm::SmallBitVector TypeToRegisterSet[IceType_NUM];
  llvm::SmallBitVector ScratchRegs;
  llvm::SmallBitVector RegsUsed;
  VarList PhysicalRegisters[IceType_NUM];
  static IceString RegNames[];

private:
  ~TargetMIPS32() override {}
};

class TargetDataMIPS32 final : public TargetDataLowering {
  TargetDataMIPS32() = delete;
  TargetDataMIPS32(const TargetDataMIPS32 &) = delete;
  TargetDataMIPS32 &operator=(const TargetDataMIPS32 &) = delete;

public:
  static std::unique_ptr<TargetDataLowering> create(GlobalContext *Ctx) {
    return std::unique_ptr<TargetDataLowering>(new TargetDataMIPS32(Ctx));
  }

  void lowerGlobals(std::unique_ptr<VariableDeclarationList> Vars) override;
  void lowerConstants() override;

protected:
  explicit TargetDataMIPS32(GlobalContext *Ctx);

private:
  void lowerGlobal(const VariableDeclaration &Var) const;
  ~TargetDataMIPS32() override {}
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

protected:
  explicit TargetHeaderMIPS32(GlobalContext *Ctx);

private:
  ~TargetHeaderMIPS32() = default;
};

} // end of namespace Ice

#endif // SUBZERO_SRC_ICETARGETLOWERINGMIPS32_H
