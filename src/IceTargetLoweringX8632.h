//===- subzero/src/IceTargetLoweringX8632.h - x86-32 lowering ---*- C++ -*-===//
//
//                        The Subzero Code Generator
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief Declares the TargetLoweringX8632 class, which implements the
/// TargetLowering interface for the x86-32 architecture.
///
//===----------------------------------------------------------------------===//

#ifndef SUBZERO_SRC_ICETARGETLOWERINGX8632_H
#define SUBZERO_SRC_ICETARGETLOWERINGX8632_H

#include "IceAssemblerX8632.h"
#include "IceDefs.h"
#include "IceRegistersX8632.h"
#include "IceTargetLowering.h"
#include "IceInstX8632.h"
#define X86NAMESPACE X8632
#include "IceTargetLoweringX86Base.h"
#undef X86NAMESPACE
#include "IceTargetLoweringX8632Traits.h"

namespace Ice {
namespace X8632 {

class TargetX8632 final : public ::Ice::X8632::TargetX86Base<X8632::Traits> {
  TargetX8632() = delete;
  TargetX8632(const TargetX8632 &) = delete;
  TargetX8632 &operator=(const TargetX8632 &) = delete;

  void emitJumpTable(const Cfg *Func,
                     const InstJumpTable *JumpTable) const override;

public:
  ~TargetX8632() = default;

  static std::unique_ptr<::Ice::TargetLowering> create(Cfg *Func) {
    return makeUnique<TargetX8632>(Func);
  }

  std::unique_ptr<::Ice::Assembler> createAssembler() const override {
    return makeUnique<X8632::AssemblerX8632>();
  }

protected:
  void _add_sp(Operand *Adjustment);
  void _mov_sp(Operand *NewValue);
  Traits::X86OperandMem *_sandbox_mem_reference(X86OperandMem *) {
    llvm::report_fatal_error("sandbox mem reference for x86-32.");
  }
  void _sub_sp(Operand *Adjustment);

  void initSandbox() {}
  void lowerIndirectJump(Variable *JumpTarget);
  void lowerCall(const InstCall *Instr) override;
  void lowerArguments() override;
  void lowerRet(const InstRet *Inst) override;
  void addProlog(CfgNode *Node) override;
  void addEpilog(CfgNode *Node) override;

private:
  ENABLE_MAKE_UNIQUE;
  friend class X8632::TargetX86Base<X8632::Traits>;

  Operand *createNaClReadTPSrcOperand() {
    Constant *Zero = Ctx->getConstantZero(IceType_i32);
    return Traits::X86OperandMem::create(Func, IceType_i32, nullptr, Zero,
                                         nullptr, 0,
                                         Traits::X86OperandMem::SegReg_GS);
  }

  explicit TargetX8632(Cfg *Func) : TargetX86Base(Func) {}
};

class TargetDataX8632 final : public TargetDataLowering {
  TargetDataX8632() = delete;
  TargetDataX8632(const TargetDataX8632 &) = delete;
  TargetDataX8632 &operator=(const TargetDataX8632 &) = delete;

public:
  ~TargetDataX8632() override = default;

  static std::unique_ptr<TargetDataLowering> create(GlobalContext *Ctx) {
    return makeUnique<TargetDataX8632>(Ctx);
  }

  void lowerGlobals(const VariableDeclarationList &Vars,
                    const IceString &SectionSuffix) override;
  void lowerConstants() override;
  void lowerJumpTables() override;

private:
  ENABLE_MAKE_UNIQUE;

  explicit TargetDataX8632(GlobalContext *Ctx);
  template <typename T> static void emitConstantPool(GlobalContext *Ctx);
};

class TargetHeaderX8632 final : public TargetHeaderLowering {
  TargetHeaderX8632() = delete;
  TargetHeaderX8632(const TargetHeaderX8632 &) = delete;
  TargetHeaderX8632 &operator=(const TargetHeaderX8632 &) = delete;

public:
  static std::unique_ptr<TargetHeaderLowering> create(GlobalContext *Ctx) {
    return std::unique_ptr<TargetHeaderLowering>(new TargetHeaderX8632(Ctx));
  }

protected:
  explicit TargetHeaderX8632(GlobalContext *Ctx);

private:
  ~TargetHeaderX8632() = default;
};

} // end of namespace X8632
} // end of namespace Ice

#endif // SUBZERO_SRC_ICETARGETLOWERINGX8632_H
