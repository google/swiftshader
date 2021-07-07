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
#include "IceTargetLoweringX8664Base.h"
#include "IceTargetLoweringX8664Traits.h"

namespace Ice {
namespace X8664 {

class TargetX8664 final : public X8664::TargetX86Base<X8664::Traits> {
  TargetX8664() = delete;
  TargetX8664(const TargetX8664 &) = delete;
  TargetX8664 &operator=(const TargetX8664 &) = delete;

public:
  ~TargetX8664() = default;

  static std::unique_ptr<::Ice::TargetLowering> create(Cfg *Func) {
    return makeUnique<TargetX8664>(Func);
  }

  std::unique_ptr<::Ice::Assembler> createAssembler() const override {
    return makeUnique<X8664::AssemblerX8664>();
  }

protected:
  void _add_sp(Operand *Adjustment);
  void _mov_sp(Operand *NewValue);
  void _sub_sp(Operand *Adjustment);
  void _link_bp();
  void _unlink_bp();
  void _push_reg(RegNumT RegNum);
  void _pop_reg(RegNumT RegNum);

  void emitStackProbe(size_t StackSizeBytes);
  void lowerIndirectJump(Variable *JumpTarget);
  Inst *emitCallToTarget(Operand *CallTarget, Variable *ReturnReg,
                         size_t NumVariadicFpArgs = 0) override;
  Variable *moveReturnValueToRegister(Operand *Value, Type ReturnType) override;

private:
  ENABLE_MAKE_UNIQUE;
  friend class X8664::TargetX86Base<X8664::Traits>;

  explicit TargetX8664(Cfg *Func) : TargetX86Base(Func) {}
};

// The -Wundefined-var-template warning requires to forward-declare static
// members of template class specializations. Note that "An explicit
// specialization of a static data member of a template is a definition if the
// declaration includes an initializer; otherwise, it is a declaration."
// Visual Studio has a bug which treats these declarations as definitions,
// leading to multiple definition errors. Since we only enable
// -Wundefined-var-template for Clang, omit these declarations on other
// compilers.
#if defined(__clang__)
template <>
std::array<SmallBitVector, RCX86_NUM>
    TargetX86Base<X8664::Traits>::TypeToRegisterSet;

template <>
std::array<SmallBitVector, RCX86_NUM>
    TargetX86Base<X8664::Traits>::TypeToRegisterSetUnfiltered;

template <>
std::array<SmallBitVector,
           TargetX86Base<X8664::Traits>::Traits::RegisterSet::Reg_NUM>
    TargetX86Base<X8664::Traits>::RegisterAliases;
#endif

} // end of namespace X8664
} // end of namespace Ice

#endif // SUBZERO_SRC_ICETARGETLOWERINGX8664_H
