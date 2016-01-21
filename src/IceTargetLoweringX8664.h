//===- subzero/src/IceTargetLoweringX8664.h - lowering for x86-64 -*- C++ -*-=//
//
//                        The Subzero Code Generator
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief Declares the TargetLoweringX8664 class, which implements the
/// TargetLowering interface for the X86 64-bit architecture.
///
//===----------------------------------------------------------------------===//

#ifndef SUBZERO_SRC_ICETARGETLOWERINGX8664_H
#define SUBZERO_SRC_ICETARGETLOWERINGX8664_H

#include "IceAssemblerX8664.h"
#include "IceCfg.h"
#include "IceGlobalContext.h"
#include "IceInstX8664.h"
#include "IceTargetLowering.h"
#define X86NAMESPACE X8664
#include "IceTargetLoweringX86Base.h"
#undef X86NAMESPACE
#include "IceTargetLoweringX8664Traits.h"

namespace Ice {
namespace X8664 {

class TargetX8664 final : public X8664::TargetX86Base<X8664::Traits> {
  TargetX8664() = delete;
  TargetX8664(const TargetX8664 &) = delete;
  TargetX8664 &operator=(const TargetX8664 &) = delete;

  void emitJumpTable(const Cfg *Func,
                     const InstJumpTable *JumpTable) const override;

public:
  ~TargetX8664() = default;

  static std::unique_ptr<::Ice::TargetLowering> create(Cfg *Func) {
    return makeUnique<TargetX8664>(Func);
  }

  std::unique_ptr<::Ice::Assembler> createAssembler() const override {
    const bool EmitAddrSizeOverridePrefix = !NeedSandboxing;
    return makeUnique<X8664::AssemblerX8664>(EmitAddrSizeOverridePrefix);
  }

  bool needSandboxing() const { return NeedSandboxing; }

protected:
  void _add_sp(Operand *Adjustment);
  void _mov_sp(Operand *NewValue);
  void _push_rbp();
  Traits::X86OperandMem *_sandbox_mem_reference(X86OperandMem *Mem);
  void _sub_sp(Operand *Adjustment);
  void _link_bp();
  void _unlink_bp();
  void _push_reg(Variable *Reg);

  void initSandbox();
  void emitSandboxedReturn();
  void lowerIndirectJump(Variable *JumpTarget);
  void emitGetIP(CfgNode *Node);
  Inst *emitCallToTarget(Operand *CallTarget, Variable *ReturnReg) override;
  Variable *moveReturnValueToRegister(Operand *Value, Type ReturnType) override;

private:
  ENABLE_MAKE_UNIQUE;
  friend class X8664::TargetX86Base<X8664::Traits>;

  explicit TargetX8664(Cfg *Func)
      : ::Ice::X8664::TargetX86Base<X8664::Traits>(Func) {}

  Operand *createNaClReadTPSrcOperand() {
    Variable *TDB = makeReg(IceType_i32);
    InstCall *Call = makeHelperCall(H_call_read_tp, TDB, 0);
    lowerCall(Call);
    return TDB;
  }
};

class TargetDataX8664 : public TargetDataLowering {
  TargetDataX8664() = delete;
  TargetDataX8664(const TargetDataX8664 &) = delete;
  TargetDataX8664 &operator=(const TargetDataX8664 &) = delete;

public:
  ~TargetDataX8664() override = default;

  static std::unique_ptr<TargetDataLowering> create(GlobalContext *Ctx) {
    return makeUnique<TargetDataX8664>(Ctx);
  }

  void lowerGlobals(const VariableDeclarationList &Vars,
                    const IceString &SectionSuffix) override;

  void lowerConstants() override;
  void lowerJumpTables() override;

private:
  ENABLE_MAKE_UNIQUE;

  explicit TargetDataX8664(GlobalContext *Ctx) : TargetDataLowering(Ctx) {}
  template <typename T> static void emitConstantPool(GlobalContext *Ctx);
};

class TargetHeaderX8664 : public TargetHeaderLowering {
  TargetHeaderX8664() = delete;
  TargetHeaderX8664(const TargetHeaderX8664 &) = delete;
  TargetHeaderX8664 &operator=(const TargetHeaderX8664 &) = delete;

public:
  ~TargetHeaderX8664() = default;

  static std::unique_ptr<TargetHeaderLowering> create(GlobalContext *Ctx) {
    return makeUnique<TargetHeaderX8664>(Ctx);
  }

private:
  ENABLE_MAKE_UNIQUE;

  explicit TargetHeaderX8664(GlobalContext *Ctx) : TargetHeaderLowering(Ctx) {}
};

} // end of namespace X8664
} // end of namespace Ice

#endif // SUBZERO_SRC_ICETARGETLOWERINGX8664_H
