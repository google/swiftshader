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
/// Note: All references to ARM "section" documentation refers to the "ARM
/// Architecture Reference Manual, ARMv7-A and ARMv7-R edition". See:
/// http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.ddi0406c
///
//===----------------------------------------------------------------------===//

#ifndef SUBZERO_SRC_ICEASSEMBLERARM32_H
#define SUBZERO_SRC_ICEASSEMBLERARM32_H

#include "IceAssembler.h"
#include "IceConditionCodesARM32.h"
#include "IceDefs.h"
#include "IceFixups.h"
#include "IceInstARM32.h"
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
    assert(NodeNumber < CfgNodeLabels.size());
    return CfgNodeLabels[NodeNumber];
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

  // List of instructions implemented by integrated assembler.

  void add(const Operand *OpRd, const Operand *OpRn, const Operand *OpSrc1,
           bool SetFlags, CondARM32::Cond Cond);

  void bkpt(uint16_t Imm16);

  void ldr(const Operand *OpRt, const Operand *OpAddress, CondARM32::Cond Cond);

  void mov(const Operand *OpRd, const Operand *OpSrc, CondARM32::Cond Cond);

  void bx(RegARM32::GPRRegister Rm, CondARM32::Cond Cond = CondARM32::AL);

  void str(const Operand *OpRt, const Operand *OpAddress, CondARM32::Cond Cond);

  void sub(const Operand *OpRd, const Operand *OpRn, const Operand *OpSrc1,
           bool SetFlags, CondARM32::Cond Cond);

  static bool classof(const Assembler *Asm) {
    return Asm->getKind() == Asm_ARM32;
  }

private:
  // A vector of pool-allocated x86 labels for CFG nodes.
  using LabelVector = std::vector<Label *>;
  LabelVector CfgNodeLabels;

  Label *getOrCreateLabel(SizeT Number, LabelVector &Labels);
  Label *getOrCreateCfgNodeLabel(SizeT NodeNumber) {
    return getOrCreateLabel(NodeNumber, CfgNodeLabels);
  }

  void emitInst(uint32_t Value) { Buffer.emit<uint32_t>(Value); }

  // Pattern cccctttoooosnnnnddddiiiiiiiiiiii where cccc=Cond, ttt=Type,
  // oooo=Opcode, nnnn=Rn, dddd=Rd, iiiiiiiiiiii=imm12 (See ARM section A5.2.3).
  void emitType01(CondARM32::Cond Cond, uint32_t Type, uint32_t Opcode,
                  bool SetCc, uint32_t Rn, uint32_t Rd, uint32_t imm12);

  // Pattern ccccoooaabalnnnnttttaaaaaaaaaaaa where cccc=Cond, ooo=InstType,
  // l=isLoad, b=isByte, and aaa0a0aaaa0000aaaaaaaaaaaa=Address. Note that
  // Address is assumed to be defined by decodeAddress() in
  // IceAssemblerARM32.cpp.
  void emitMemOp(CondARM32::Cond Cond, uint32_t InstType, bool IsLoad,
                 bool IsByte, uint32_t Rt, uint32_t Address);
};

} // end of namespace ARM32
} // end of namespace Ice

#endif // SUBZERO_SRC_ICEASSEMBLERARM32_H
