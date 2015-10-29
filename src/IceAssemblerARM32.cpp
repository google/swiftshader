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
#include "IceCfgNode.h"
#include "IceUtils.h"

namespace {

using namespace Ice;
using namespace Ice::ARM32;

// The following define individual bits.
static constexpr IValueT B0 = 1;
static constexpr IValueT B1 = 1 << 1;
static constexpr IValueT B2 = 1 << 2;
static constexpr IValueT B3 = 1 << 3;
static constexpr IValueT B4 = 1 << 4;
static constexpr IValueT B5 = 1 << 5;
static constexpr IValueT B6 = 1 << 6;
static constexpr IValueT B21 = 1 << 21;
static constexpr IValueT B24 = 1 << 24;

// Constants used for the decoding or encoding of the individual fields of
// instructions. Based on ARM section A5.1.
static constexpr IValueT L = 1 << 20; // load (or store)
static constexpr IValueT W = 1 << 21; // writeback base register
                                      // (or leave unchanged)
static constexpr IValueT B = 1 << 22; // unsigned byte (or word)
static constexpr IValueT U = 1 << 23; // positive (or negative)
                                      // offset/index
static constexpr IValueT P = 1 << 24; // offset/pre-indexed
                                      // addressing (or
                                      // post-indexed addressing)

static constexpr IValueT kConditionShift = 28;
static constexpr IValueT kLinkShift = 24;
static constexpr IValueT kOpcodeShift = 21;
static constexpr IValueT kRdShift = 12;
static constexpr IValueT kRmShift = 0;
static constexpr IValueT kRnShift = 16;
static constexpr IValueT kSShift = 20;
static constexpr IValueT kTypeShift = 25;

// Immediate instruction fields encoding.
static constexpr IValueT kImmed8Bits = 8;
static constexpr IValueT kImmed8Shift = 0;
static constexpr IValueT kRotateBits = 4;
static constexpr IValueT kRotateShift = 8;

// Shift instruction register fields encodings.
static constexpr IValueT kShiftImmShift = 7;
static constexpr IValueT kShiftImmBits = 5;
static constexpr IValueT kShiftShift = 5;

static constexpr IValueT kImmed12Bits = 12;
static constexpr IValueT kImm12Shift = 0;

// Type of instruction encoding (bits 25-27). See ARM section A5.1
static constexpr IValueT kInstTypeDataRegister = 0;  // i.e. 000
static constexpr IValueT kInstTypeDataImmediate = 1; // i.e. 001
static constexpr IValueT kInstTypeMemImmediate = 2;  // i.e. 010

// Offset modifier to current PC for next instruction.  The offset is off by 8
// due to the way the ARM CPUs read PC.
static constexpr IOffsetT kPCReadOffset = 8;

// Mask to pull out PC offset from branch (b) instruction.
static constexpr int kBranchOffsetBits = 24;
static constexpr IOffsetT kBranchOffsetMask = 0x00ffffff;

inline IValueT encodeBool(bool B) { return B ? 1 : 0; }

inline IValueT encodeGPRRegister(RegARM32::GPRRegister Rn) {
  return static_cast<IValueT>(Rn);
}

inline bool isGPRRegisterDefined(RegARM32::GPRRegister R) {
  return R != RegARM32::Encoded_Not_GPR;
}

inline bool isGPRRegisterDefined(IValueT R) {
  return R != encodeGPRRegister(RegARM32::Encoded_Not_GPR);
}

inline bool isConditionDefined(CondARM32::Cond Cond) {
  return Cond != CondARM32::kNone;
}

inline IValueT encodeCondition(CondARM32::Cond Cond) {
  return static_cast<IValueT>(Cond);
}

IValueT encodeShift(OperandARM32::ShiftKind Shift) {
  // Follows encoding in ARM section A8.4.1 "Constant shifts".
  switch (Shift) {
  case OperandARM32::kNoShift:
  case OperandARM32::LSL:
    return 0; // 0b00
  case OperandARM32::LSR:
    return 1; // 0b01
  case OperandARM32::ASR:
    return 2; // 0b10
  case OperandARM32::ROR:
  case OperandARM32::RRX:
    return 3; // 0b11
  }
}

// Returns the bits in the corresponding masked value.
inline IValueT mask(IValueT Value, IValueT Shift, IValueT Bits) {
  return (Value >> Shift) & ((1 << Bits) - 1);
}

// Extract out a Bit in Value.
inline bool isBitSet(IValueT Bit, IValueT Value) {
  return (Value & Bit) == Bit;
}

// Returns the GPR register at given Shift in Value.
inline RegARM32::GPRRegister getGPRReg(IValueT Shift, IValueT Value) {
  return static_cast<RegARM32::GPRRegister>((Value >> Shift) & 0xF);
}

// The way an operand was decoded in functions decodeOperand and decodeAddress
// below.
enum DecodedResult {
  // Unable to decode, value left undefined.
  CantDecode = 0,
  // Value is register found.
  DecodedAsRegister,
  // Value=rrrriiiiiiii where rrrr is the rotation, and iiiiiiii is the imm8
  // value.
  DecodedAsRotatedImm8,
  // i.e. 0000000pu0w0nnnn0000iiiiiiiiiiii where nnnn is the base register Rn,
  // p=1 if pre-indexed addressing, u=1 if offset positive, w=1 if writeback to
  // Rn should be used, and iiiiiiiiiiii is the offset.
  DecodedAsImmRegOffset
};

// Encodes iiiiitt0mmmm for data-processing (2nd) operands where iiiii=Imm5,
// tt=Shift, and mmmm=Rm.
IValueT encodeShiftRotateImm5(IValueT Rm, OperandARM32::ShiftKind Shift,
                              IValueT imm5) {
  (void)kShiftImmBits;
  assert(imm5 < (1 << kShiftImmBits));
  return (imm5 << kShiftImmShift) | (encodeShift(Shift) << kShiftShift) | Rm;
}

DecodedResult decodeOperand(const Operand *Opnd, IValueT &Value) {
  if (const auto *Var = llvm::dyn_cast<Variable>(Opnd)) {
    if (Var->hasReg()) {
      Value = Var->getRegNum();
      return DecodedAsRegister;
    }
  } else if (const auto *FlexImm = llvm::dyn_cast<OperandARM32FlexImm>(Opnd)) {
    const IValueT Immed8 = FlexImm->getImm();
    const IValueT Rotate = FlexImm->getRotateAmt();
    if (!((Rotate < (1 << kRotateBits)) && (Immed8 < (1 << kImmed8Bits))))
      return CantDecode;
    Value = (Rotate << kRotateShift) | (Immed8 << kImmed8Shift);
    return DecodedAsRotatedImm8;
  }
  return CantDecode;
}

IValueT decodeImmRegOffset(RegARM32::GPRRegister Reg, IOffsetT Offset,
                           OperandARM32Mem::AddrMode Mode) {
  IValueT Value = Mode | (encodeGPRRegister(Reg) << kRnShift);
  if (Offset < 0) {
    Value = (Value ^ U) | -Offset; // Flip U to adjust sign.
  } else {
    Value |= Offset;
  }
  return Value;
}

// Decodes memory address Opnd, and encodes that information into Value,
// based on how ARM represents the address. Returns how the value was encoded.
DecodedResult decodeAddress(const Operand *Opnd, IValueT &Value) {
  if (const auto *Var = llvm::dyn_cast<Variable>(Opnd)) {
    // Should be a stack variable, with an offset.
    if (Var->hasReg())
      return CantDecode;
    const IOffsetT Offset = Var->getStackOffset();
    if (!Utils::IsAbsoluteUint(12, Offset))
      return CantDecode;
    Value = decodeImmRegOffset(RegARM32::Encoded_Reg_sp, Offset,
                               OperandARM32Mem::Offset);
    return DecodedAsImmRegOffset;
  }
  return CantDecode;
}

// Checks that Offset can fit in imm24 constant of branch (b) instruction.
bool canEncodeBranchOffset(IOffsetT Offset) {
  return Utils::IsAligned(Offset, 4) &&
         Utils::IsInt(kBranchOffsetBits, Offset >> 2);
}

} // end of anonymous namespace

namespace Ice {
namespace ARM32 {

void AssemblerARM32::bindCfgNodeLabel(const CfgNode *Node) {
  GlobalContext *Ctx = Node->getCfg()->getContext();
  if (BuildDefs::dump() && !Ctx->getFlags().getDisableHybridAssembly()) {
    // Generate label name so that branches can find it.
    constexpr SizeT InstSize = 0;
    emitTextInst(Node->getAsmName() + ":", InstSize);
  }
  SizeT NodeNumber = Node->getIndex();
  assert(!getPreliminary());
  Label *L = getOrCreateCfgNodeLabel(NodeNumber);
  this->bind(L);
}

Label *AssemblerARM32::getOrCreateLabel(SizeT Number, LabelVector &Labels) {
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

IValueT AssemblerARM32::encodeBranchOffset(IOffsetT Offset, IValueT Inst) {
  // Adjust offset to the way ARM CPUs read PC.
  Offset -= kPCReadOffset;

  bool IsGoodOffset = canEncodeBranchOffset(Offset);
  assert(IsGoodOffset);
  // Note: Following cast is for MINIMAL build.
  (void)IsGoodOffset;

  // Properly preserve only the bits supported in the instruction.
  Offset >>= 2;
  Offset &= kBranchOffsetMask;
  return (Inst & ~kBranchOffsetMask) | Offset;
}

IOffsetT AssemblerARM32::decodeBranchOffset(IValueT Inst) {
  // Sign-extend, left-shift by 2, and adjust to the way ARM CPUs read PC.
  IOffsetT Offset = static_cast<IOffsetT>((Inst & kBranchOffsetMask) << 8);
  return (Offset >> 6) + kPCReadOffset;
}

void AssemblerARM32::bind(Label *L) {
  IOffsetT BoundPc = Buffer.size();
  assert(!L->isBound()); // Labels can only be bound once.
  while (L->isLinked()) {
    IOffsetT Position = L->getLinkPosition();
    IOffsetT Dest = BoundPc - Position;
    IValueT Inst = Buffer.load<IValueT>(Position);
    Buffer.store<IValueT>(Position, encodeBranchOffset(Dest, Inst));
    L->setPosition(decodeBranchOffset(Inst));
  }
  L->bindTo(BoundPc);
}

void AssemblerARM32::emitTextInst(const std::string &Text, SizeT InstSize) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  AssemblerFixup *F = createTextFixup(Text, InstSize);
  emitFixup(F);
  for (SizeT I = 0; I < InstSize; ++I)
    Buffer.emit<char>(0);
}

void AssemblerARM32::emitType01(CondARM32::Cond Cond, IValueT Type,
                                IValueT Opcode, bool SetCc, IValueT Rn,
                                IValueT Rd, IValueT Imm12) {
  if (!isGPRRegisterDefined(Rd) || !isConditionDefined(Cond))
    return setNeedsTextFixup();
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  const IValueT Encoding = (encodeCondition(Cond) << kConditionShift) |
                           (Type << kTypeShift) | (Opcode << kOpcodeShift) |
                           (encodeBool(SetCc) << kSShift) | (Rn << kRnShift) |
                           (Rd << kRdShift) | Imm12;
  emitInst(Encoding);
}

void AssemblerARM32::emitType05(CondARM32::Cond Cond, IOffsetT Offset,
                                bool Link) {
  // cccc101liiiiiiiiiiiiiiiiiiiiiiii where cccc=Cond, l=Link, and
  // iiiiiiiiiiiiiiiiiiiiiiii=
  // EncodedBranchOffset(cccc101l000000000000000000000000, Offset);
  if (!isConditionDefined(Cond))
    return setNeedsTextFixup();
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  IValueT Encoding = static_cast<int32_t>(Cond) << kConditionShift |
                     5 << kTypeShift | (Link ? 1 : 0) << kLinkShift;
  Encoding = encodeBranchOffset(Offset, Encoding);
  emitInst(Encoding);
}

void AssemblerARM32::emitBranch(Label *L, CondARM32::Cond Cond, bool Link) {
  // TODO(kschimpf): Handle far jumps.
  if (L->isBound()) {
    const int32_t Dest = L->getPosition() - Buffer.size();
    emitType05(Cond, Dest, Link);
    return;
  }
  const IOffsetT Position = Buffer.size();
  // Use the offset field of the branch instruction for linking the sites.
  emitType05(Cond, L->getEncodedPosition(), Link);
  if (!needsTextFixup())
    L->linkTo(Position);
}

void AssemblerARM32::emitMemOp(CondARM32::Cond Cond, IValueT InstType,
                               bool IsLoad, bool IsByte, IValueT Rt,
                               IValueT Address) {
  if (!isGPRRegisterDefined(Rt) || !isConditionDefined(Cond))
    return setNeedsTextFixup();
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  const IValueT Encoding = (encodeCondition(Cond) << kConditionShift) |
                           (InstType << kTypeShift) | (IsLoad ? L : 0) |
                           (IsByte ? B : 0) | (Rt << kRdShift) | Address;
  emitInst(Encoding);
}

void AssemblerARM32::adc(const Operand *OpRd, const Operand *OpRn,
                         const Operand *OpSrc1, bool SetFlags,
                         CondARM32::Cond Cond) {
  IValueT Rd;
  if (decodeOperand(OpRd, Rd) != DecodedAsRegister)
    return setNeedsTextFixup();
  IValueT Rn;
  if (decodeOperand(OpRn, Rn) != DecodedAsRegister)
    return setNeedsTextFixup();
  constexpr IValueT Adc = B2 | B0; // 0101
  IValueT Src1Value;
  // TODO(kschimpf) Other possible decodings of adc.
  switch (decodeOperand(OpSrc1, Src1Value)) {
  default:
    return setNeedsTextFixup();
  case DecodedAsRegister: {
    // ADC (register) - ARM section 18.8.2, encoding A1:
    //   adc{s}<c> <Rd>, <Rn>, <Rm>{, <shift>}
    //
    // cccc0000101snnnnddddiiiiitt0mmmm where cccc=Cond, dddd=Rd, nnnn=Rn,
    // mmmm=Rm, iiiii=Shift, tt=ShiftKind, and s=SetFlags.
    constexpr IValueT Imm5 = 0;
    Src1Value = encodeShiftRotateImm5(Src1Value, OperandARM32::kNoShift, Imm5);
    if (((Rd == RegARM32::Encoded_Reg_pc) && SetFlags))
      // Conditions of rule violated.
      return setNeedsTextFixup();
    emitType01(Cond, kInstTypeDataRegister, Adc, SetFlags, Rn, Rd, Src1Value);
    return;
  }
  case DecodedAsRotatedImm8: {
    // ADC (Immediate) - ARM section A8.8.1, encoding A1:
    //   adc{s}<c> <Rd>, <Rn>, #<RotatedImm8>
    //
    // cccc0010101snnnnddddiiiiiiiiiiii where cccc=Cond, dddd=Rd, nnnn=Rn,
    // s=SetFlags and iiiiiiiiiiii=Src1Value=RotatedImm8.
    if ((Rd == RegARM32::Encoded_Reg_pc && SetFlags))
      // Conditions of rule violated.
      return setNeedsTextFixup();
    emitType01(Cond, kInstTypeDataImmediate, Adc, SetFlags, Rn, Rd, Src1Value);
    return;
  }
  };
}

void AssemblerARM32::add(const Operand *OpRd, const Operand *OpRn,
                         const Operand *OpSrc1, bool SetFlags,
                         CondARM32::Cond Cond) {
  IValueT Rd;
  if (decodeOperand(OpRd, Rd) != DecodedAsRegister)
    return setNeedsTextFixup();
  IValueT Rn;
  if (decodeOperand(OpRn, Rn) != DecodedAsRegister)
    return setNeedsTextFixup();
  constexpr IValueT Add = B2; // 0100
  IValueT Src1Value;
  // TODO(kschimpf) Other possible decodings of add.
  switch (decodeOperand(OpSrc1, Src1Value)) {
  default:
    return setNeedsTextFixup();
  case DecodedAsRegister: {
    // ADD (register) - ARM section A8.8.7, encoding A1:
    //   add{s}<c> <Rd>, <Rn>, <Rm>{, <shiff>}
    // ADD (Sp plus register) - ARM section A8.8.11, encoding A1:
    //   add{s}<c> sp, <Rn>, <Rm>{, <shiff>}
    //
    // cccc0000100snnnnddddiiiiitt0mmmm where cccc=Cond, dddd=Rd, nnnn=Rn,
    // mmmm=Rm, iiiii=Shift, tt=ShiftKind, and s=SetFlags.
    constexpr IValueT Imm5 = 0;
    Src1Value = encodeShiftRotateImm5(Src1Value, OperandARM32::kNoShift, Imm5);
    if (((Rd == RegARM32::Encoded_Reg_pc) && SetFlags))
      // Conditions of rule violated.
      return setNeedsTextFixup();
    emitType01(Cond, kInstTypeDataRegister, Add, SetFlags, Rn, Rd, Src1Value);
    return;
  }
  case DecodedAsRotatedImm8: {
    // ADD (Immediate) - ARM section A8.8.5, encoding A1:
    //   add{s}<c> <Rd>, <Rn>, #<RotatedImm8>
    // ADD (SP plus immediate) - ARM section A8.8.9, encoding A1.
    //   add{s}<c> <Rd>, sp, #<RotatedImm8>
    //
    // cccc0010100snnnnddddiiiiiiiiiiii where cccc=Cond, dddd=Rd, nnnn=Rn,
    // s=SetFlags and iiiiiiiiiiii=Src1Value=RotatedImm8.
    if ((Rd == RegARM32::Encoded_Reg_pc && SetFlags))
      // Conditions of rule violated.
      return setNeedsTextFixup();
    emitType01(Cond, kInstTypeDataImmediate, Add, SetFlags, Rn, Rd, Src1Value);
    return;
  }
  };
}

void AssemblerARM32::b(Label *L, CondARM32::Cond Cond) {
  emitBranch(L, Cond, false);
}

void AssemblerARM32::bkpt(uint16_t Imm16) {
  // BKPT - ARM section A*.8.24 - encoding A1:
  //   bkpt #<Imm16>
  //
  // cccc00010010iiiiiiiiiiii0111iiii where cccc=AL and iiiiiiiiiiiiiiii=Imm16
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  const IValueT Encoding = (CondARM32::AL << kConditionShift) | B24 | B21 |
                           ((Imm16 >> 4) << 8) | B6 | B5 | B4 | (Imm16 & 0xf);
  emitInst(Encoding);
}

void AssemblerARM32::bx(RegARM32::GPRRegister Rm, CondARM32::Cond Cond) {
  // BX - ARM section A8.8.27, encoding A1:
  //   bx<c> <Rm>
  //
  // cccc000100101111111111110001mmmm where mmmm=rm and cccc=Cond.
  if (!(isGPRRegisterDefined(Rm) && isConditionDefined(Cond)))
    return setNeedsTextFixup();
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  const IValueT Encoding = (encodeCondition(Cond) << kConditionShift) | B24 |
                           B21 | (0xfff << 8) | B4 |
                           (encodeGPRRegister(Rm) << kRmShift);
  emitInst(Encoding);
}

void AssemblerARM32::ldr(const Operand *OpRt, const Operand *OpAddress,
                         CondARM32::Cond Cond) {
  IValueT Rt;
  if (decodeOperand(OpRt, Rt) != DecodedAsRegister)
    return setNeedsTextFixup();
  IValueT Address;
  if (decodeAddress(OpAddress, Address) != DecodedAsImmRegOffset)
    return setNeedsTextFixup();
  // LDR (immediate) - ARM section A8.8.63, encoding A1:
  //   ldr<c> <Rt>, [<Rn>{, #+/-<imm12>}]      ; p=1, w=0
  //   ldr<c> <Rt>, [<Rn>], #+/-<imm12>        ; p=1, w=1
  //   ldr<c> <Rt>, [<Rn>, #+/-<imm12>]!       ; p=0, w=1
  // LDRB (immediate) - ARM section A8.8.68, encoding A1:
  //   ldrb<c> <Rt>, [<Rn>{, #+/-<imm12>}]     ; p=1, w=0
  //   ldrb<c> <Rt>, [<Rn>], #+/-<imm12>       ; p=1, w=1
  //   ldrb<c> <Rt>, [<Rn>, #+/-<imm12>]!      ; p=0, w=1
  //
  // cccc010pubw1nnnnttttiiiiiiiiiiii where cccc=Cond, tttt=Rt, nnnn=Rn,
  // iiiiiiiiiiii=imm12, b=1 if STRB, u=1 if +.
  constexpr bool IsLoad = true;
  const Type Ty = OpRt->getType();
  if (!(Ty == IceType_i32 || Ty == IceType_i8)) // TODO(kschimpf) Expand?
    return setNeedsTextFixup();
  const bool IsByte = typeWidthInBytes(Ty) == 1;
  // Check conditions of rules violated.
  if (getGPRReg(kRnShift, Address) == RegARM32::Encoded_Reg_pc)
    return setNeedsTextFixup();
  if (!isBitSet(P, Address) && isBitSet(W, Address))
    return setNeedsTextFixup();
  if (!IsByte && (getGPRReg(kRnShift, Address) == RegARM32::Encoded_Reg_sp) &&
      !isBitSet(P, Address) && isBitSet(U, Address) & !isBitSet(W, Address) &&
      (mask(Address, kImm12Shift, kImmed12Bits) == 0x8 /* 000000000100 */))
    return setNeedsTextFixup();
  emitMemOp(Cond, kInstTypeMemImmediate, IsLoad, IsByte, Rt, Address);
}

void AssemblerARM32::mov(const Operand *OpRd, const Operand *OpSrc,
                         CondARM32::Cond Cond) {
  IValueT Rd;
  if (decodeOperand(OpRd, Rd) != DecodedAsRegister)
    return setNeedsTextFixup();
  IValueT Src;
  // TODO(kschimpf) Handle other forms of mov.
  if (decodeOperand(OpSrc, Src) != DecodedAsRotatedImm8)
    return setNeedsTextFixup();
  // MOV (immediate) - ARM section A8.8.102, encoding A1:
  //   mov{S}<c> <Rd>, #<RotatedImm8>
  //
  // cccc0011101s0000ddddiiiiiiiiiiii where cccc=Cond, s=SetFlags, dddd=Rd,
  // and iiiiiiiiiiii=RotatedImm8=Src.  Note: We don't use movs in this
  // assembler.
  constexpr bool SetFlags = false;
  if ((Rd == RegARM32::Encoded_Reg_pc && SetFlags))
    // Conditions of rule violated.
    return setNeedsTextFixup();
  constexpr IValueT Rn = 0;
  constexpr IValueT Mov = B3 | B2 | B0; // 1101.
  emitType01(Cond, kInstTypeDataImmediate, Mov, SetFlags, Rn, Rd, Src);
}

void AssemblerARM32::sbc(const Operand *OpRd, const Operand *OpRn,
                         const Operand *OpSrc1, bool SetFlags,
                         CondARM32::Cond Cond) {
  IValueT Rd;
  if (decodeOperand(OpRd, Rd) != DecodedAsRegister)
    return setNeedsTextFixup();
  IValueT Rn;
  if (decodeOperand(OpRn, Rn) != DecodedAsRegister)
    return setNeedsTextFixup();
  constexpr IValueT Sbc = B2 | B1; // 0110
  IValueT Src1Value;
  // TODO(kschimpf) Other possible decodings of sbc.
  switch (decodeOperand(OpSrc1, Src1Value)) {
  default:
    return setNeedsTextFixup();
  case DecodedAsRegister: {
    // SBC (register) - ARM section 18.8.162, encoding A1:
    //   sbc{s}<c> <Rd>, <Rn>, <Rm>{, <shift>}
    //
    // cccc0000110snnnnddddiiiiitt0mmmm where cccc=Cond, dddd=Rd, nnnn=Rn,
    // mmmm=Rm, iiiii=Shift, tt=ShiftKind, and s=SetFlags.
    constexpr IValueT Imm5 = 0;
    Src1Value = encodeShiftRotateImm5(Src1Value, OperandARM32::kNoShift, Imm5);
    if (((Rd == RegARM32::Encoded_Reg_pc) && SetFlags))
      // Conditions of rule violated.
      return setNeedsTextFixup();
    emitType01(Cond, kInstTypeDataRegister, Sbc, SetFlags, Rn, Rd, Src1Value);
    return;
  }
  case DecodedAsRotatedImm8: {
    // SBC (Immediate) - ARM section A8.8.161, encoding A1:
    //   sbc{s}<c> <Rd>, <Rn>, #<RotatedImm8>
    //
    // cccc0010110snnnnddddiiiiiiiiiiii where cccc=Cond, dddd=Rd, nnnn=Rn,
    // s=SetFlags and iiiiiiiiiiii=Src1Value=RotatedImm8.
    if ((Rd == RegARM32::Encoded_Reg_pc && SetFlags))
      // Conditions of rule violated.
      return setNeedsTextFixup();
    emitType01(Cond, kInstTypeDataImmediate, Sbc, SetFlags, Rn, Rd, Src1Value);
    return;
  }
  };
}

void AssemblerARM32::str(const Operand *OpRt, const Operand *OpAddress,
                         CondARM32::Cond Cond) {
  IValueT Rt;
  if (decodeOperand(OpRt, Rt) != DecodedAsRegister)
    return setNeedsTextFixup();
  IValueT Address;
  if (decodeAddress(OpAddress, Address) != DecodedAsImmRegOffset)
    return setNeedsTextFixup();
  // STR (immediate) - ARM section A8.8.204, encoding A1:
  //   str<c> <Rt>, [<Rn>{, #+/-<imm12>}]      ; p=1, w=0
  //   str<c> <Rt>, [<Rn>], #+/-<imm12>        ; p=1, w=1
  //   str<c> <Rt>, [<Rn>, #+/-<imm12>]!       ; p=0, w=1
  // STRB (immediate) - ARM section A8.8.207, encoding A1:
  //   strb<c> <Rt>, [<Rn>{, #+/-<imm12>}]     ; p=1, w=0
  //   strb<c> <Rt>, [<Rn>], #+/-<imm12>       ; p=1, w=1
  //   strb<c> <Rt>, [<Rn>, #+/-<imm12>]!      ; p=0, w=1
  //
  // cccc010pubw0nnnnttttiiiiiiiiiiii where cccc=Cond, tttt=Rt, nnnn=Rn,
  // iiiiiiiiiiii=imm12, b=1 if STRB, u=1 if +.
  constexpr bool IsLoad = false;
  const Type Ty = OpRt->getType();
  if (!(Ty == IceType_i32 || Ty == IceType_i8)) // TODO(kschimpf) Expand?
    return setNeedsTextFixup();
  const bool IsByte = typeWidthInBytes(Ty) == 1;
  // Check for rule violations.
  if ((getGPRReg(kRnShift, Address) == RegARM32::Encoded_Reg_pc))
    return setNeedsTextFixup();
  if (!isBitSet(P, Address) && isBitSet(W, Address))
    return setNeedsTextFixup();
  if (!IsByte && (getGPRReg(kRnShift, Address) == RegARM32::Encoded_Reg_sp) &&
      isBitSet(P, Address) && !isBitSet(U, Address) && isBitSet(W, Address) &&
      (mask(Address, kImm12Shift, kImmed12Bits) == 0x8 /* 000000000100 */))
    return setNeedsTextFixup();
  emitMemOp(Cond, kInstTypeMemImmediate, IsLoad, IsByte, Rt, Address);
}

void AssemblerARM32::sub(const Operand *OpRd, const Operand *OpRn,
                         const Operand *OpSrc1, bool SetFlags,
                         CondARM32::Cond Cond) {
  IValueT Rd;
  if (decodeOperand(OpRd, Rd) != DecodedAsRegister)
    return setNeedsTextFixup();
  IValueT Rn;
  if (decodeOperand(OpRn, Rn) != DecodedAsRegister)
    return setNeedsTextFixup();
  constexpr IValueT Sub = B1; // 0010
  IValueT Src1Value;
  // TODO(kschimpf) Other possible decodings of sub.
  switch (decodeOperand(OpSrc1, Src1Value)) {
  default:
    return setNeedsTextFixup();
  case DecodedAsRegister: {
    // SUB (register) - ARM section A8.8.223, encoding A1:
    //   sub{s}<c> <Rd>, <Rn>, <Rm>{, <shift>}
    // SUB (SP minus register): See ARM section 8.8.226, encoding A1:
    //   sub{s}<c> <Rd>, sp, <Rm>{, <Shift>}
    //
    // cccc0000010snnnnddddiiiiitt0mmmm where cccc=Cond, dddd=Rd, nnnn=Rn,
    // mmmm=Rm, iiiiii=shift, tt=ShiftKind, and s=SetFlags.
    Src1Value = encodeShiftRotateImm5(Src1Value, OperandARM32::kNoShift, 0);
    if (((Rd == RegARM32::Encoded_Reg_pc) && SetFlags))
      // Conditions of rule violated.
      return setNeedsTextFixup();
    emitType01(Cond, kInstTypeDataRegister, Sub, SetFlags, Rn, Rd, Src1Value);
    return;
  }
  case DecodedAsRotatedImm8: {
    // Sub (Immediate) - ARM section A8.8.222, encoding A1:
    //    sub{s}<c> <Rd>, <Rn>, #<RotatedImm8>
    // Sub (Sp minus immediate) - ARM section A8.*.225, encoding A1:
    //    sub{s}<c> sp, <Rn>, #<RotatedImm8>
    //
    // cccc0010010snnnnddddiiiiiiiiiiii where cccc=Cond, dddd=Rd, nnnn=Rn,
    // s=SetFlags and iiiiiiiiiiii=Src1Value=RotatedImm8
    if (Rd == RegARM32::Encoded_Reg_pc)
      // Conditions of rule violated.
      return setNeedsTextFixup();
    emitType01(Cond, kInstTypeDataImmediate, Sub, SetFlags, Rn, Rd, Src1Value);
    return;
  }
  }
}

} // end of namespace ARM32
} // end of namespace Ice
