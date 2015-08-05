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
/// This file declares the TargetLoweringX8664 class, which implements the
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
#include "IceTargetLoweringX8664Traits.h"
#include "IceTargetLoweringX86Base.h"

namespace Ice {

class TargetX8664 final
    : public ::Ice::X86Internal::TargetX86Base<TargetX8664> {
  TargetX8664() = delete;
  TargetX8664(const TargetX8664 &) = delete;
  TargetX8664 &operator=(const TargetX8664 &) = delete;

  void emitJumpTable(const Cfg *Func,
                     const InstJumpTable *JumpTable) const override;

public:
  static TargetX8664 *create(Cfg *Func) { return new TargetX8664(Func); }

protected:
  void lowerCall(const InstCall *Instr) override;

private:
  friend class ::Ice::X86Internal::TargetX86Base<TargetX8664>;

  explicit TargetX8664(Cfg *Func)
      : ::Ice::X86Internal::TargetX86Base<TargetX8664>(Func) {}

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
} // end of namespace Ice

#endif // SUBZERO_SRC_ICETARGETLOWERINGX8664_H
