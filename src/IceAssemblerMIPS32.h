//===- subzero/src/IceAssemblerMIPS32.h - Assembler for MIPS ----*- C++ -*-===//
//
// Copyright (c) 2013, the Dart project authors.  Please see the AUTHORS file
// for details. All rights reserved. Use of this source code is governed by a
// BSD-style license that can be found in the LICENSE file.
//
// Modified by the Subzero authors.
//
//===----------------------------------------------------------------------===//
//
//                        The Subzero Code Generator
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file implements the Assembler class for MIPS32.
///
//===----------------------------------------------------------------------===//

#ifndef SUBZERO_SRC_ICEASSEMBLERMIPS32_H
#define SUBZERO_SRC_ICEASSEMBLERMIPS32_H

#include "IceAssembler.h"
#include "IceDefs.h"
#include "IceFixups.h"

namespace Ice {
namespace MIPS32 {

class AssemblerMIPS32 : public Assembler {
  AssemblerMIPS32(const AssemblerMIPS32 &) = delete;
  AssemblerMIPS32 &operator=(const AssemblerMIPS32 &) = delete;

public:
  explicit AssemblerMIPS32(bool use_far_branches = false)
      : Assembler(Asm_MIPS32) {
    // This mode is only needed and implemented for MIPS32 and ARM.
    assert(!use_far_branches);
    (void)use_far_branches;
  }
  ~AssemblerMIPS32() override = default;

  void alignFunction() override {
    llvm::report_fatal_error("Not yet implemented.");
  }

  SizeT getBundleAlignLog2Bytes() const override { return 4; }

  const char *getAlignDirective() const override { return ".p2alignl"; }

  llvm::ArrayRef<uint8_t> getNonExecBundlePadding() const override {
    // TODO(reed kotler) . Find out what this should be.
    static const uint8_t Padding[] = {0xE7, 0xFE, 0xDE, 0xF0};
    return llvm::ArrayRef<uint8_t>(Padding, 4);
  }

  void padWithNop(intptr_t Padding) override {
    (void)Padding;
    llvm::report_fatal_error("Not yet implemented.");
  }

  Ice::Label *getCfgNodeLabel(SizeT NodeNumber) override {
    (void)NodeNumber;
    llvm_unreachable("Not yet implemented.");
  }

  void bindCfgNodeLabel(const CfgNode *) override {
    llvm::report_fatal_error("Not yet implemented.");
  }

  bool fixupIsPCRel(FixupKind Kind) const override {
    (void)Kind;
    llvm::report_fatal_error("Not yet implemented.");
  }

  static bool classof(const Assembler *Asm) {
    return Asm->getKind() == Asm_MIPS32;
  }
};

} // end of namespace MIPS32
} // end of namespace Ice

#endif // SUBZERO_SRC_ICEASSEMBLERMIPS32_H
