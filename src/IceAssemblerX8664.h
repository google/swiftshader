//===- subzero/src/IceAssemblerX8664.h - Assembler for x86-64 -*- C++ -*---===//
//
//                        The Subzero Code Generator
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements the Assembler class for x86-64.
//
//===----------------------------------------------------------------------===//

#ifndef SUBZERO_SRC_ICEASSEMBLERX8664_H
#define SUBZERO_SRC_ICEASSEMBLERX8664_H

#include "IceAssembler.h"
#include "IceDefs.h"

namespace Ice {
namespace X8664 {

class AssemblerX8664 final : public Assembler {
  AssemblerX8664(const AssemblerX8664 &) = delete;
  AssemblerX8664 &operator=(const AssemblerX8664 &) = delete;

public:
  explicit AssemblerX8664(bool use_far_branches = false) : Assembler() {
    assert(!use_far_branches);
    (void)use_far_branches;
    llvm::report_fatal_error("Not yet implemented");
  }

  ~AssemblerX8664() override = default;

  void alignFunction() override;
  void padWithNop(intptr_t Padding) override;
  SizeT getBundleAlignLog2Bytes() const override;
  const char *getNonExecPadDirective() const override;
  llvm::ArrayRef<uint8_t> getNonExecBundlePadding() const override;
  void bindCfgNodeLabel(SizeT NodeNumber) override;
  bool fixupIsPCRel(FixupKind Kind) const override;
};

} // end of namespace X8664
} // end of namespace Ice

#endif // SUBZERO_SRC_ICEASSEMBLERX8664_H
