//===- subzero/src/IceTargetLoweringX8664.h - x86-64 lowering ---*- C++ -*-===//
//
//                        The Subzero Code Generator
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares the TargetLoweringX8664 class, which
// implements the TargetLowering interface for the x86-64
// architecture.
//
//===----------------------------------------------------------------------===//
#ifndef SUBZERO_SRC_ICETARGETLOWERINGX8664_H
#define SUBZERO_SRC_ICETARGETLOWERINGX8664_H

#include "IceDefs.h"
#include "IceTargetLowering.h"

namespace Ice {

class TargetX8664 : public TargetLowering {
  TargetX8664() = delete;
  TargetX8664(const TargetX8664 &) = delete;
  TargetX8664 &operator=(const TargetX8664 &) = delete;

public:
  static TargetX8664 *create(Cfg *) {
    llvm::report_fatal_error("Not yet implemented");
  }
};

class TargetDataX8664 : public TargetDataLowering {
  TargetDataX8664() = delete;
  TargetDataX8664(const TargetDataX8664 &) = delete;
  TargetDataX8664 &operator=(const TargetDataX8664 &) = delete;

public:
  static std::unique_ptr<TargetDataLowering> create(GlobalContext *Ctx) {
    llvm::report_fatal_error("Not yet implemented");
  }
};

class TargetHeaderX8664 : public TargetHeaderLowering {
  TargetHeaderX8664() = delete;
  TargetHeaderX8664(const TargetHeaderX8664 &) = delete;
  TargetHeaderX8664 &operator=(const TargetHeaderX8664 &) = delete;

public:
  static std::unique_ptr<TargetHeaderLowering> create(GlobalContext *Ctx) {
    llvm::report_fatal_error("Not yet implemented");
  }
};

} // end of namespace Ice

#endif // SUBZERO_SRC_ICETARGETLOWERINGX8664_H
