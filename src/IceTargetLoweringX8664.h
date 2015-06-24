//===- subzero/src/IceTargetLoweringX8664.h - lowering for x86-64 -*- C++ -*-=//
//
//                        The Subzero Code Generator
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares the TargetLoweringX8664 class, which implements the
// TargetLowering interface for the X86 64-bit architecture.
//
//===----------------------------------------------------------------------===//

#ifndef SUBZERO_SRC_ICETARGETLOWERINGX8664_H
#define SUBZERO_SRC_ICETARGETLOWERINGX8664_H

#include "IceCfg.h"
#include "IceGlobalContext.h"
#include "IceTargetLowering.h"

namespace Ice {

class TargetX8664 : public TargetLowering {
  TargetX8664() = delete;
  TargetX8664(const TargetX8664 &) = delete;
  TargetX8664 &operator=(const TargetX8664 &) = delete;

public:
  static TargetX8664 *create(Cfg *Func);

private:
  explicit TargetX8664(Cfg *Func) : TargetLowering(Func) {}
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

private:
  ENABLE_MAKE_UNIQUE;

  explicit TargetDataX8664(GlobalContext *Ctx) : TargetDataLowering(Ctx) {}
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
