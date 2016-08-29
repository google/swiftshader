//===- subzero/src/IceAssemblerMIPS32.h - Assembler for MIPS ----*- C++ -*-===//
//
//                        The Subzero Code Generator
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief Declares the Assembler class for MIPS32.
///
//===----------------------------------------------------------------------===//

#ifndef SUBZERO_SRC_ICEASSEMBLERMIPS32_H
#define SUBZERO_SRC_ICEASSEMBLERMIPS32_H

#include "IceAssembler.h"
#include "IceDefs.h"
#include "IceFixups.h"
#include "IceInstMIPS32.h"
#include "IceTargetLowering.h"

namespace Ice {
namespace MIPS32 {

using IValueT = uint32_t;
using IOffsetT = int32_t;

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
  ~AssemblerMIPS32() override {
    if (BuildDefs::asserts()) {
      for (const Label *Label : CfgNodeLabels) {
        Label->finalCheck();
      }
      for (const Label *Label : LocalLabels) {
        Label->finalCheck();
      }
    }
  }

  void trap();

  void nop();

  void emitRtRsImm16(IValueT Opcode, const Operand *OpRt, const Operand *OpRs,
                     const uint32_t Imm, const char *InsnName);

  void emitRdRtSa(IValueT Opcode, const Operand *OpRd, const Operand *OpRt,
                  const uint32_t Sa, const char *InsnName);

  void emitRdRsRt(IValueT Opcode, const Operand *OpRd, const Operand *OpRs,
                  const Operand *OpRt, const char *InsnName);

  void emitBr(const CondMIPS32::Cond Cond, const Operand *OpRs,
              const Operand *OpRt, IOffsetT Offset);

  void addiu(const Operand *OpRt, const Operand *OpRs, const uint32_t Imm);

  void slti(const Operand *OpRt, const Operand *OpRs, const uint32_t Imm);

  void sltiu(const Operand *OpRt, const Operand *OpRs, const uint32_t Imm);

  void and_(const Operand *OpRd, const Operand *OpRs, const Operand *OpRt);

  void andi(const Operand *OpRt, const Operand *OpRs, const uint32_t Imm);

  void or_(const Operand *OpRd, const Operand *OpRs, const Operand *OpRt);

  void ori(const Operand *OpRt, const Operand *OpRs, const uint32_t Imm);

  void xor_(const Operand *OpRd, const Operand *OpRs, const Operand *OpRt);

  void xori(const Operand *OpRt, const Operand *OpRs, const uint32_t Imm);

  void sll(const Operand *OpRd, const Operand *OpRt, const uint32_t Sa);

  void srl(const Operand *OpRd, const Operand *OpRt, const uint32_t Sa);

  void sra(const Operand *OpRd, const Operand *OpRt, const uint32_t Sa);

  void move(const Operand *OpRd, const Operand *OpRs);

  void addu(const Operand *OpRd, const Operand *OpRs, const Operand *OpRt);

  void slt(const Operand *OpRd, const Operand *OpRs, const Operand *OpRt);

  void sltu(const Operand *OpRd, const Operand *OpRs, const Operand *OpRt);

  void sw(const Operand *OpRt, const Operand *OpBase, const uint32_t Offset);

  void lw(const Operand *OpRt, const Operand *OpBase, const uint32_t Offset);

  void ret(void);

  void b(Label *TargetLabel);

  void bcc(const CondMIPS32::Cond Cond, const Operand *OpRs,
           const Operand *OpRt, Label *TargetLabel);

  void bzc(const CondMIPS32::Cond Cond, const Operand *OpRs,
           Label *TargetLabel);

  void alignFunction() override {
    const SizeT Align = 1 << getBundleAlignLog2Bytes();
    SizeT BytesNeeded = Utils::OffsetToAlignment(Buffer.getPosition(), Align);
    constexpr SizeT InstSize = sizeof(IValueT);
    assert(BytesNeeded % InstMIPS32::InstSize == 0);
    while (BytesNeeded > 0) {
      trap();
      BytesNeeded -= InstSize;
    }
  }

  SizeT getBundleAlignLog2Bytes() const override { return 4; }

  const char *getAlignDirective() const override { return ".p2alignl"; }

  llvm::ArrayRef<uint8_t> getNonExecBundlePadding() const override;

  void padWithNop(intptr_t Padding) override;

  void bind(Label *label);

  void emitTextInst(const std::string &Text, SizeT InstSize);

  Ice::Label *getCfgNodeLabel(SizeT NodeNumber) override {
    assert(NodeNumber < CfgNodeLabels.size());
    return CfgNodeLabels[NodeNumber];
  }

  Label *getOrCreateCfgNodeLabel(SizeT NodeNumber) {
    return getOrCreateLabel(NodeNumber, CfgNodeLabels);
  }

  Label *getOrCreateLocalLabel(SizeT Number) {
    return getOrCreateLabel(Number, LocalLabels);
  }

  bool fixupIsPCRel(FixupKind Kind) const override {
    (void)Kind;
    llvm::report_fatal_error("Not yet implemented.");
  }

  static bool classof(const Assembler *Asm) {
    return Asm->getKind() == Asm_MIPS32;
  }

private:
  ENABLE_MAKE_UNIQUE;

  using LabelVector = std::vector<Label *>;
  LabelVector CfgNodeLabels;
  LabelVector LocalLabels;

  // Returns the offset encoded in the branch instruction Inst.
  static IOffsetT decodeBranchOffset(IValueT Inst);

  Label *getOrCreateLabel(SizeT Number, LabelVector &Labels);

  void bindCfgNodeLabel(const CfgNode *) override;

  void emitInst(IValueT Value) {
    AssemblerBuffer::EnsureCapacity _(&Buffer);
    Buffer.emit<IValueT>(Value);
  }
};

} // end of namespace MIPS32
} // end of namespace Ice

#endif // SUBZERO_SRC_ICEASSEMBLERMIPS32_H
