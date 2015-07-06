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
/// This file declares the TargetLoweringX8632 class, which
/// implements the TargetLowering interface for the x86-32
/// architecture.
///
//===----------------------------------------------------------------------===//

#ifndef SUBZERO_SRC_ICETARGETLOWERINGX8632_H
#define SUBZERO_SRC_ICETARGETLOWERINGX8632_H

#include "IceAssemblerX8632.h"
#include "IceDefs.h"
#include "IceInstX8632.h"
#include "IceRegistersX8632.h"
#include "IceTargetLowering.h"
#include "IceTargetLoweringX8632Traits.h"

namespace Ice {

class TargetX8632 : public TargetLowering {
  TargetX8632() = delete;
  TargetX8632(const TargetX8632 &) = delete;
  TargetX8632 &operator=(const TargetX8632 &) = delete;

public:
  using X86InstructionSet = X8632::Traits::InstructionSet;

  static TargetX8632 *create(Cfg *Func);
  virtual X8632::Traits::Address
  stackVarToAsmOperand(const Variable *Var) const = 0;
  virtual X86InstructionSet getInstructionSet() const = 0;

protected:
  explicit TargetX8632(Cfg *Func) : TargetLowering(Func) {}
};

class TargetDataX8632 final : public TargetDataLowering {
  TargetDataX8632() = delete;
  TargetDataX8632(const TargetDataX8632 &) = delete;
  TargetDataX8632 &operator=(const TargetDataX8632 &) = delete;

public:
  static std::unique_ptr<TargetDataLowering> create(GlobalContext *Ctx) {
    return std::unique_ptr<TargetDataLowering>(new TargetDataX8632(Ctx));
  }

  void lowerGlobals(const VariableDeclarationList &Vars,
                    const IceString &SectionSuffix) override;
  void lowerConstants() override;

protected:
  explicit TargetDataX8632(GlobalContext *Ctx);

private:
  ~TargetDataX8632() override = default;
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

} // end of namespace Ice

#endif // SUBZERO_SRC_ICETARGETLOWERINGX8632_H
