//===- subzero/src/IceAssemblerARM32.h - Assembler for ARM32 ----*- C++ -*-===//
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

#ifndef SUBZERO_SRC_ICEASSEMBLERARM32_H
#define SUBZERO_SRC_ICEASSEMBLERARM32_H

#include "IceAssembler.h"
#include "IceConditionCodesARM32.h"
#include "IceDefs.h"
#include "IceFixups.h"
#include "IceRegistersARM32.h"
#include "IceTargetLowering.h"

namespace Ice {
namespace ARM32 {

class AssemblerARM32 : public Assembler {
  AssemblerARM32(const AssemblerARM32 &) = delete;
  AssemblerARM32 &operator=(const AssemblerARM32 &) = delete;

public:
  explicit AssemblerARM32(GlobalContext *Ctx, bool use_far_branches = false)
      : Assembler(Asm_ARM32, Ctx) {
    // TODO(kschimpf): Add mode if needed when branches are handled.
    (void)use_far_branches;
  }
  ~AssemblerARM32() override = default;

  void alignFunction() override {
    const SizeT Align = 1 << getBundleAlignLog2Bytes();
    SizeT BytesNeeded = Utils::OffsetToAlignment(Buffer.getPosition(), Align);
    constexpr SizeT InstSize = sizeof(int32_t);
    assert(BytesNeeded % InstSize == 0);
    while (BytesNeeded > 0) {
      // TODO(kschimpf) Should this be NOP or some other instruction?
      bkpt(0);
      BytesNeeded -= InstSize;
    }
  }

  SizeT getBundleAlignLog2Bytes() const override { return 4; }

  const char *getAlignDirective() const override { return ".p2alignl"; }

  llvm::ArrayRef<uint8_t> getNonExecBundlePadding() const override {
    // Use a particular UDF encoding -- TRAPNaCl in LLVM: 0xE7FEDEF0
    // http://llvm.org/viewvc/llvm-project?view=revision&revision=173943
    static const uint8_t Padding[] = {0xE7, 0xFE, 0xDE, 0xF0};
    return llvm::ArrayRef<uint8_t>(Padding, 4);
  }

  void padWithNop(intptr_t Padding) override {
    (void)Padding;
    llvm_unreachable("Not yet implemented.");
  }

  Ice::Label *getCfgNodeLabel(SizeT NodeNumber) override {
    (void)NodeNumber;
    llvm_unreachable("Not yet implemented.");
  }

  void bindCfgNodeLabel(SizeT NodeNumber) override {
    assert(!getPreliminary());
    Label *L = getOrCreateCfgNodeLabel(NodeNumber);
    this->bind(L);
  }

  bool fixupIsPCRel(FixupKind Kind) const override {
    (void)Kind;
    llvm_unreachable("Not yet implemented.");
  }
  void bind(Label *label);

  void bkpt(uint16_t imm16);

  void bx(RegARM32::GPRRegister rm, CondARM32::Cond cond = CondARM32::AL);

  static bool classof(const Assembler *Asm) {
    return Asm->getKind() == Asm_ARM32;
  }

private:
  // Instruction encoding bits.

  // halfword (or byte)
  static constexpr uint32_t H = 1 << 5;
  // load (or store)
  static constexpr uint32_t L = 1 << 20;
  // set condition code (or leave unchanged)
  static constexpr uint32_t S = 1 << 20;
  // writeback base register (or leave unchanged)
  static constexpr uint32_t W = 1 << 21;
  // accumulate in multiply instruction (or not)
  static constexpr uint32_t A = 1 << 21;
  // unsigned byte (or word)
  static constexpr uint32_t B = 1 << 22;
  // high/lo bit of start of s/d register range
  static constexpr uint32_t D = 1 << 22;
  // long (or short)
  static constexpr uint32_t N = 1 << 22;
  // positive (or negative) offset/index
  static constexpr uint32_t U = 1 << 23;
  // offset/pre-indexed addressing (or post-indexed addressing)
  static constexpr uint32_t P = 1 << 24;
  // immediate shifter operand (or not)
  static constexpr uint32_t I = 1 << 25;

  // The following define individual bits.
  static constexpr uint32_t B0 = 1;
  static constexpr uint32_t B1 = 1 << 1;
  static constexpr uint32_t B2 = 1 << 2;
  static constexpr uint32_t B3 = 1 << 3;
  static constexpr uint32_t B4 = 1 << 4;
  static constexpr uint32_t B5 = 1 << 5;
  static constexpr uint32_t B6 = 1 << 6;
  static constexpr uint32_t B7 = 1 << 7;
  static constexpr uint32_t B8 = 1 << 8;
  static constexpr uint32_t B9 = 1 << 9;
  static constexpr uint32_t B10 = 1 << 10;
  static constexpr uint32_t B11 = 1 << 11;
  static constexpr uint32_t B12 = 1 << 12;
  static constexpr uint32_t B16 = 1 << 16;
  static constexpr uint32_t B17 = 1 << 17;
  static constexpr uint32_t B18 = 1 << 18;
  static constexpr uint32_t B19 = 1 << 19;
  static constexpr uint32_t B20 = 1 << 20;
  static constexpr uint32_t B21 = 1 << 21;
  static constexpr uint32_t B22 = 1 << 22;
  static constexpr uint32_t B23 = 1 << 23;
  static constexpr uint32_t B24 = 1 << 24;
  static constexpr uint32_t B25 = 1 << 25;
  static constexpr uint32_t B26 = 1 << 26;
  static constexpr uint32_t B27 = 1 << 27;

  // Constants used for the decoding or encoding of the individual fields of
  // instructions. Based on section A5.1 from the "ARM Architecture Reference
  // Manual, ARMv7-A and ARMv7-R edition". See:
  // http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.ddi0406c
  static constexpr uint32_t kConditionShift = 28;
  static constexpr uint32_t kConditionBits = 4;
  static constexpr uint32_t kTypeShift = 25;
  static constexpr uint32_t kTypeBits = 3;
  static constexpr uint32_t kLinkShift = 24;
  static constexpr uint32_t kLinkBits = 1;
  static constexpr uint32_t kUShift = 23;
  static constexpr uint32_t kUBits = 1;
  static constexpr uint32_t kOpcodeShift = 21;
  static constexpr uint32_t kOpcodeBits = 4;
  static constexpr uint32_t kSShift = 20;
  static constexpr uint32_t kSBits = 1;
  static constexpr uint32_t kRnShift = 16;
  static constexpr uint32_t kRnBits = 4;
  static constexpr uint32_t kRdShift = 12;
  static constexpr uint32_t kRdBits = 4;
  static constexpr uint32_t kRsShift = 8;
  static constexpr uint32_t kRsBits = 4;
  static constexpr uint32_t kRmShift = 0;
  static constexpr uint32_t kRmBits = 4;

  // Immediate instruction fields encoding.
  static constexpr uint32_t kRotateShift = 8;
  static constexpr uint32_t kRotateBits = 4;
  static constexpr uint32_t kImmed8Shift = 0;
  static constexpr uint32_t kImmed8Bits = 8;

  // Shift instruction register fields encodings.
  static constexpr uint32_t kShiftImmShift = 7;
  static constexpr uint32_t kShiftRegisterShift = 8;
  static constexpr uint32_t kShiftImmBits = 5;
  static constexpr uint32_t kShiftShift = 5;
  static constexpr uint32_t kShiftBits = 2;

  // Load/store instruction offset field encoding.
  static constexpr uint32_t kOffset12Shift = 0;
  static constexpr uint32_t kOffset12Bits = 12;
  static constexpr uint32_t kOffset12Mask = 0x00000fff;

  // Mul instruction register field encodings.
  static constexpr uint32_t kMulRdShift = 16;
  static constexpr uint32_t kMulRdBits = 4;
  static constexpr uint32_t kMulRnShift = 12;
  static constexpr uint32_t kMulRnBits = 4;

  // Div instruction register field encodings.
  static constexpr uint32_t kDivRdShift = 16;
  static constexpr uint32_t kDivRdBits = 4;
  static constexpr uint32_t kDivRmShift = 8;
  static constexpr uint32_t kDivRmBints = 4;
  static constexpr uint32_t kDivRnShift = 0;
  static constexpr uint32_t kDivRnBits = 4;

  // ldrex/strex register field encodings.
  static constexpr uint32_t kLdExRnShift = 16;
  static constexpr uint32_t kLdExRtShift = 12;
  static constexpr uint32_t kStrExRnShift = 16;
  static constexpr uint32_t kStrExRdShift = 12;
  static constexpr uint32_t kStrExRtShift = 0;

  // MRC instruction offset field encoding.
  static constexpr uint32_t kCRmShift = 0;
  static constexpr uint32_t kCRmBits = 4;
  static constexpr uint32_t kOpc2Shift = 5;
  static constexpr uint32_t kOpc2Bits = 3;
  static constexpr uint32_t kCoprocShift = 8;
  static constexpr uint32_t kCoprocBits = 4;
  static constexpr uint32_t kCRnShift = 16;
  static constexpr uint32_t kCRnBits = 4;
  static constexpr uint32_t kOpc1Shift = 21;
  static constexpr uint32_t kOpc1Bits = 3;

  static constexpr uint32_t kBranchOffsetMask = 0x00ffffff;

  // A vector of pool-allocated x86 labels for CFG nodes.
  using LabelVector = std::vector<Label *>;
  LabelVector CfgNodeLabels;

  Label *getOrCreateLabel(SizeT Number, LabelVector &Labels);
  Label *getOrCreateCfgNodeLabel(SizeT NodeNumber) {
    return getOrCreateLabel(NodeNumber, CfgNodeLabels);
  }

  void emitInt32(int32_t Value) { Buffer.emit<int32_t>(Value); }

  static int32_t BkptEncoding(uint16_t imm16) {
    // bkpt requires that the cond field is AL.
    // cccc00010010iiiiiiiiiiii0111iiii where cccc=AL and i in imm16
    return (CondARM32::AL << kConditionShift) | B24 | B21 |
           ((imm16 >> 4) << 8) | B6 | B5 | B4 | (imm16 & 0xf);
  }
};

} // end of namespace ARM32
} // end of namespace Ice

#endif // SUBZERO_SRC_ICEASSEMBLERARM32_H
