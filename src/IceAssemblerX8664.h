//===- subzero/src/IceAssemberX8664.h - Assembler for x86-64 ----*- C++ -*-===//
//
//                        The Subzero Code Generator
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements the Assembler class for x86-64.h.
//
//===----------------------------------------------------------------------===//

#ifndef SUBZERO_SRC_ICEASSEMBLERX8664_H
#define SUBZERO_SRC_ICEASSEMBLERX8664_H

#include "IceAssembler.h"

namespace Ice {
namespace X8664 {

class AssemblerX8664 final : public Assembler {
  AssemblerX8664(const AssemblerX8664 &) = delete;
  AssemblerX8664 &operator=(const AssemblerX8664 &) = delete;

public:
  explicit AssemblerX8664(bool use_far_branches = false) : Assembler() {
    llvm::report_fatal_error("Not yet implemented");
  }

  void alignFunction() override {
    llvm::report_fatal_error("Not yet implemented");
  }

  void padWithNop(intptr_t Padding) override {
    llvm::report_fatal_error("Not yet implemented");
  }

  SizeT getBundleAlignLog2Bytes() const override {
    llvm::report_fatal_error("Not yet implemented");
  }

  const char *getNonExecPadDirective() const override {
    llvm::report_fatal_error("Not yet implemented");
  }

  llvm::ArrayRef<uint8_t> getNonExecBundlePadding() const override {
    llvm::report_fatal_error("Not yet implemented");
  }

  void bindCfgNodeLabel(SizeT NodeNumber) override {
    llvm::report_fatal_error("Not yet implemented");
  }

  bool fixupIsPCRel(FixupKind Kind) const override {
    llvm::report_fatal_error("Not yet implemented");
  }
};

} // end of namespace X8664
} // end of namespace Ice

#endif // SUBZERO_SRC_ICEASSEMBLERX8664_H
