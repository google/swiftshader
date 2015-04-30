//===- subzero/src/assembler_arm32.h - Assembler for ARM32 ------*- C++ -*-===//
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
//
// This file implements the Assembler class for ARM32.
//
//===----------------------------------------------------------------------===//

#ifndef SUBZERO_SRC_ASSEMBLER_ARM32_H
#define SUBZERO_SRC_ASSEMBLER_ARM32_H

#include "IceDefs.h"
#include "IceFixups.h"

#include "assembler.h"

namespace Ice {
namespace ARM32 {

class AssemblerARM32 : public Assembler {
  AssemblerARM32(const AssemblerARM32 &) = delete;
  AssemblerARM32 &operator=(const AssemblerARM32 &) = delete;

public:
  explicit AssemblerARM32(bool use_far_branches = false) : Assembler() {
    // This mode is only needed and implemented for MIPS and ARM.
    assert(!use_far_branches);
    (void)use_far_branches;
  }
  ~AssemblerARM32() override = default;

  void alignFunction() override {
    llvm::report_fatal_error("Not yet implemented.");
  }

  SizeT getBundleAlignLog2Bytes() const override { return 4; }

  llvm::ArrayRef<uint8_t> getNonExecBundlePadding() const override {
    llvm::report_fatal_error("Not yet implemented.");
  }

  void padWithNop(intptr_t Padding) override {
    (void)Padding;
    llvm::report_fatal_error("Not yet implemented.");
  }

  void BindCfgNodeLabel(SizeT NodeNumber) override {
    (void)NodeNumber;
    llvm::report_fatal_error("Not yet implemented.");
  }

  bool fixupIsPCRel(FixupKind Kind) const override {
    (void)Kind;
    llvm::report_fatal_error("Not yet implemented.");
  }
};

} // end of namespace ARM32
} // end of namespace Ice

#endif // SUBZERO_SRC_ASSEMBLER_ARM32_H
