//===- subzero/src/IceAssemblerX8664.h - Assembler for x86-64 ---*- C++ -*-===//
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
/// This file implements the Assembler class for x86-64.
///
//===----------------------------------------------------------------------===//

#ifndef SUBZERO_SRC_ICEASSEMBLERX8664_H
#define SUBZERO_SRC_ICEASSEMBLERX8664_H

#include "IceAssembler.h"
#include "IceAssemblerX86Base.h"
#include "IceDefs.h"
#include "IceOperand.h"
#include "IceTargetLoweringX8664Traits.h"
#include "IceTypes.h"
#include "IceUtils.h"

namespace Ice {

class TargetX8664;

namespace X8664 {

using Immediate = ::Ice::X86Internal::Immediate;
using Label = ::Ice::X86Internal::Label;

class AssemblerX8664 : public X86Internal::AssemblerX86Base<TargetX8664> {
  AssemblerX8664(const AssemblerX8664 &) = delete;
  AssemblerX8664 &operator=(const AssemblerX8664 &) = delete;

public:
  explicit AssemblerX8664(bool use_far_branches = false)
      : X86Internal::AssemblerX86Base<TargetX8664>(Asm_X8664,
                                                   use_far_branches) {}
  ~AssemblerX8664() override = default;

  static bool classof(const Assembler *Asm) {
    return Asm->getKind() == Asm_X8664;
  }
};

} // end of namespace X8664
} // end of namespace Ice

#endif // SUBZERO_SRC_ICEASSEMBLERX8664_H
