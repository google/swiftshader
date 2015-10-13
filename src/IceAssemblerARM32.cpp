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

// The following define individual bits.
static constexpr uint32_t B0 = 1;
static constexpr uint32_t B2 = 1 << 2;
static constexpr uint32_t B3 = 1 << 3;
static constexpr uint32_t B4 = 1 << 4;
static constexpr uint32_t B5 = 1 << 5;
static constexpr uint32_t B6 = 1 << 6;
static constexpr uint32_t B21 = 1 << 21;
static constexpr uint32_t B24 = 1 << 24;

// Constants used for the decoding or encoding of the individual fields of
// instructions. Based on ARM section A5.1.
static constexpr uint32_t kConditionShift = 28;
static constexpr uint32_t kOpcodeShift = 21;
static constexpr uint32_t kRdShift = 12;
static constexpr uint32_t kRmShift = 0;
static constexpr uint32_t kRnShift = 16;
static constexpr uint32_t kSShift = 20;
static constexpr uint32_t kTypeShift = 25;

// Immediate instruction fields encoding.
static constexpr uint32_t kImmed8Bits = 8;
static constexpr uint32_t kImmed8Shift = 0;
static constexpr uint32_t kRotateBits = 4;
static constexpr uint32_t kRotateShift = 8;

// Types of instructions.
static constexpr uint32_t kInstTypeImmediate = 1;

inline uint32_t encodeBool(bool b) { return b ? 1 : 0; }

inline uint32_t encodeGPRRegister(RegARM32::GPRRegister Rn) {
  return static_cast<uint32_t>(Rn);
}

inline bool isGPRRegisterDefined(RegARM32::GPRRegister R) {
  return R != RegARM32::Encoded_Not_GPR;
}

inline bool isGPRRegisterDefined(uint32_t R) {
  return R != encodeGPRRegister(RegARM32::Encoded_Not_GPR);
}

inline bool isConditionDefined(CondARM32::Cond Cond) {
  return Cond != CondARM32::kNone;
}

inline uint32_t encodeCondition(CondARM32::Cond Cond) {
  return static_cast<uint32_t>(Cond);
}

// Converts rotated immediate into imm12.
inline uint32_t encodeImm12FromFlexImm(const OperandARM32FlexImm &FlexImm) {
  uint32_t Immed8 = FlexImm.getImm();
  uint32_t Rotate = FlexImm.getRotateAmt();
  assert((Rotate < (1 << kRotateBits)) && (Immed8 < (1 << kImmed8Bits)));
  return (Rotate << kRotateShift) | (Immed8 << kImmed8Shift);
}

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

void ARM32::AssemblerARM32::emitType01(CondARM32::Cond Cond, uint32_t Type,
                                       uint32_t Opcode, bool SetCc, uint32_t Rn,
                                       uint32_t Rd, uint32_t Imm12) {
  assert(isGPRRegisterDefined(Rd));
  assert(Cond != CondARM32::kNone);
  uint32_t Encoding = encodeCondition(Cond) << kConditionShift |
                      (Type << kTypeShift) | (Opcode << kOpcodeShift) |
                      (encodeBool(SetCc) << kSShift) | (Rn << kRnShift) |
                      (Rd << kRdShift) | Imm12;
  emitInst(Encoding);
}

void ARM32::AssemblerARM32::bkpt(uint16_t imm16) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  uint32_t Encoding = (CondARM32::AL << kConditionShift) | B24 | B21 |
                      ((imm16 >> 4) << 8) | B6 | B5 | B4 | (imm16 & 0xf);
  emitInst(Encoding);
}

void ARM32::AssemblerARM32::bx(RegARM32::GPRRegister Rm, CondARM32::Cond Cond) {
  // cccc000100101111111111110001mmmm where mmmm=rm and cccc=Cond.
  // (ARM section A8.8.27, encoding A1).
  assert(isGPRRegisterDefined(Rm));
  assert(isConditionDefined(Cond));
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  uint32_t Encoding = (encodeCondition(Cond) << kConditionShift) | B24 | B21 |
                      (0xfff << 8) | B4 | (encodeGPRRegister(Rm) << kRmShift);
  emitInst(Encoding);
}

void ARM32::AssemblerARM32::mov(RegARM32::GPRRegister Rd,
                                const OperandARM32FlexImm &FlexImm,
                                CondARM32::Cond Cond) {
  // cccc0011101s0000ddddiiiiiiiiiiii (ARM section A8.8.102, encoding A1)
  assert(isConditionDefined(Cond));
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  bool SetCc = false; // Note: We don't use movs in this assembler.
  uint32_t Rn = 0;
  uint32_t Mov = B3 | B2 | B0; // 1101.
  emitType01(Cond, kInstTypeImmediate, Mov, SetCc, Rn, encodeGPRRegister(Rd),
             encodeImm12FromFlexImm(FlexImm));
}

} // end of namespace Ice
