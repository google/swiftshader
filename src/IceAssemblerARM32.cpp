//===- subzero/src/IceAssemblerARM32.cpp - Assembler for ARM32 --*- C++ -*-===//
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
/// This file implements the Assembler class for ARM32.
///
//===----------------------------------------------------------------------===//

#include "IceAssemblerARM32.h"

namespace Ice {

Label *ARM32::AssemblerARM32::getOrCreateLabel(SizeT Number,
                                               LabelVector &Labels) {
  Label *L = nullptr;
  if (Number == Labels.size()) {
    L = new (this->allocate<Label>()) Label();
    Labels.push_back(L);
    return L;
  }
  if (Number > Labels.size()) {
    Labels.resize(Number + 1);
  }
  L = Labels[Number];
  if (!L) {
    L = new (this->allocate<Label>()) Label();
    Labels[Number] = L;
  }
  return L;
}

void ARM32::AssemblerARM32::bind(Label *label) {
  intptr_t bound = Buffer.size();
  assert(!label->isBound()); // Labels can only be bound once.
  while (label->isLinked()) {
    intptr_t position = label->getLinkPosition();
    intptr_t next = Buffer.load<int32_t>(position);
    Buffer.store<int32_t>(position, bound - (position + 4));
    label->setPosition(next);
  }
  // TODO(kschimpf) Decide if we have near jumps.
  label->bindTo(bound);
}

void ARM32::AssemblerARM32::bkpt(uint16_t imm16) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitInt32(BkptEncoding(imm16));
}

void ARM32::AssemblerARM32::bx(RegARM32::GPRRegister rm, CondARM32::Cond cond) {
  // cccc000100101111111111110001mmmm where mmmm=rm and cccc=Cond.
  assert(rm != RegARM32::Encoded_Not_GPR);
  assert(cond != CondARM32::kNone);
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  int32_t encoding = (static_cast<int32_t>(cond) << kConditionShift) | B24 |
                     B21 | (0xfff << 8) | B4 |
                     (static_cast<int32_t>(rm) << kRmShift);
  emitInt32(encoding);
}

} // end of namespace Ice
