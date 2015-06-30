//===- subzero/src/IceAssemblerX8632.h - Assembler for x86-32 ---*- C++ -*-===//
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
// This file implements the Assembler class for x86-32.
//
//===----------------------------------------------------------------------===//

#ifndef SUBZERO_SRC_ICEASSEMBLERX8632_H
#define SUBZERO_SRC_ICEASSEMBLERX8632_H

#include "IceAssembler.h"
#include "IceAssemblerX86Base.h"
#include "IceDefs.h"
#include "IceOperand.h"
#include "IceTargetLoweringX8632Traits.h"
#include "IceTypes.h"
#include "IceUtils.h"

namespace Ice {

class TargetX8632;

namespace X8632 {

class AssemblerX8632 : public X86Internal::AssemblerX86Base<TargetX8632> {
  AssemblerX8632(const AssemblerX8632 &) = delete;
  AssemblerX8632 &operator=(const AssemblerX8632 &) = delete;

public:
  explicit AssemblerX8632(bool use_far_branches = false)
      : X86Internal::AssemblerX86Base<TargetX8632>(Asm_X8632,
                                                   use_far_branches) {}
  ~AssemblerX8632() override = default;

  static bool classof(const Assembler *Asm) {
    return Asm->getKind() == Asm_X8632;
  }
};

} // end of namespace X8632
} // end of namespace Ice

#endif // SUBZERO_SRC_ICEASSEMBLERX8632_H
