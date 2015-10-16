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
#include "IceUtils.h"

namespace {

using namespace Ice;

// The following define individual bits.
static constexpr uint32_t B0 = 1;
static constexpr uint32_t B1 = 1 << 1;
static constexpr uint32_t B2 = 1 << 2;
static constexpr uint32_t B3 = 1 << 3;
static constexpr uint32_t B4 = 1 << 4;
static constexpr uint32_t B5 = 1 << 5;
static constexpr uint32_t B6 = 1 << 6;
static constexpr uint32_t B21 = 1 << 21;
static constexpr uint32_t B24 = 1 << 24;

// Constants used for the decoding or encoding of the individual fields of
// instructions. Based on ARM section A5.1.
static constexpr uint32_t L = 1 << 20; // load (or store)
static constexpr uint32_t W = 1 << 21; // writeback base register (or leave
                                       // unchanged)
static constexpr uint32_t B = 1 << 22; // unsigned byte (or word)
static constexpr uint32_t U = 1 << 23; // positive (or negative) offset/index
static constexpr uint32_t P = 1 << 24; // offset/pre-indexed addressing (or
                                       // post-indexed addressing)

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

static constexpr uint32_t kImmed12Bits = 12;
static constexpr uint32_t kImm12Shift = 0;

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

// Returns the bits in the corresponding masked value.
inline uint32_t mask(uint32_t Value, uint32_t Shift, uint32_t Bits) {
  return (Value >> Shift) & ((1 << Bits) - 1);
}

// Extract out a Bit in Value.
inline bool isBitSet(uint32_t Bit, uint32_t Value) {
  return (Value & Bit) == Bit;
}

// Returns the GPR register at given Shift in Value.
inline RegARM32::GPRRegister getGPRReg(uint32_t Shift, uint32_t Value) {
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

DecodedResult decodeOperand(const Operand *Opnd, uint32_t &Value) {
  if (const auto *Var = llvm::dyn_cast<Variable>(Opnd)) {
    if (Var->hasReg()) {
      Value = Var->getRegNum();
      return DecodedAsRegister;
    }
  } else if (const auto *FlexImm = llvm::dyn_cast<OperandARM32FlexImm>(Opnd)) {
    const uint32_t Immed8 = FlexImm->getImm();
    const uint32_t Rotate = FlexImm->getRotateAmt();
    assert((Rotate < (1 << kRotateBits)) && (Immed8 < (1 << kImmed8Bits)));
    // TODO(kschimpf): Remove void casts when MINIMAL build allows.
    (void) kRotateBits;
    (void) kImmed8Bits;
    Value = (Rotate << kRotateShift) | (Immed8 << kImmed8Shift);
    return DecodedAsRotatedImm8;
  }
  return CantDecode;
}

uint32_t decodeImmRegOffset(RegARM32::GPRRegister Reg, int32_t Offset,
                            OperandARM32Mem::AddrMode Mode) {
  uint32_t Value = Mode | (encodeGPRRegister(Reg) << kRnShift);
  if (Offset < 0) {
    Value = (Value ^ U) | -Offset; // Flip U to adjust sign.
  } else {
    Value |= Offset;
  }
  return Value;
}

// Decodes memory address Opnd, and encodes that information into Value,
// based on how ARM represents the address. Returns how the value was encoded.
DecodedResult decodeAddress(const Operand *Opnd, uint32_t &Value) {
  if (const auto *Var = llvm::dyn_cast<Variable>(Opnd)) {
    // Should be a stack variable, with an offset.
    if (Var->hasReg())
      return CantDecode;
    const int32_t Offset = Var->getStackOffset();
    if (!Utils::IsAbsoluteUint(12, Offset))
      return CantDecode;
    Value = decodeImmRegOffset(RegARM32::Encoded_Reg_sp, Offset,
                               OperandARM32Mem::Offset);
    return DecodedAsImmRegOffset;
  }
  return CantDecode;
}

} // end of anonymous namespace

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

void ARM32::AssemblerARM32::emitType01(CondARM32::Cond Cond, uint32_t Type,
                                       uint32_t Opcode, bool SetCc, uint32_t Rn,
                                       uint32_t Rd, uint32_t Imm12) {
  assert(isGPRRegisterDefined(Rd));
  // TODO(kschimpf): Remove void cast when MINIMAL build allows.
  (void) isGPRRegisterDefined(Rd);
  assert(Cond != CondARM32::kNone);
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  const uint32_t Encoding = (encodeCondition(Cond) << kConditionShift) |
                            (Type << kTypeShift) | (Opcode << kOpcodeShift) |
                            (encodeBool(SetCc) << kSShift) | (Rn << kRnShift) |
                            (Rd << kRdShift) | Imm12;
  emitInst(Encoding);
}

void ARM32::AssemblerARM32::emitMemOp(CondARM32::Cond Cond, uint32_t InstType,
                                      bool IsLoad, bool IsByte, uint32_t Rt,
                                      uint32_t Address) {
  assert(isGPRRegisterDefined(Rt));
  assert(Cond != CondARM32::kNone);
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  const uint32_t Encoding = (encodeCondition(Cond) << kConditionShift) |
                            (InstType << kTypeShift) | (IsLoad ? L : 0) |
                            (IsByte ? B : 0) | (Rt << kRdShift) | Address;
  emitInst(Encoding);
}

void ARM32::AssemblerARM32::add(const Operand *OpRd, const Operand *OpRn,
                                const Operand *OpSrc1, bool SetFlags,
                                CondARM32::Cond Cond) {
  // Note: Loop is used so that we can short circuit using break;
  do {
    uint32_t Rd;
    if (decodeOperand(OpRd, Rd) != DecodedAsRegister)
      break;
    uint32_t Rn;
    if (decodeOperand(OpRn, Rn) != DecodedAsRegister)
      break;
    uint32_t Src1Value;
    // TODO(kschimpf) Other possible decodings of add.
    if (decodeOperand(OpSrc1, Src1Value) == DecodedAsRotatedImm8) {
      // ADD (Immediate): See ARM section A8.8.5, rule A1.
      // cccc0010100snnnnddddiiiiiiiiiiii where cccc=Cond, dddd=Rd, nnnn=Rn,
      // s=SetFlags and iiiiiiiiiiii=Src1Value
      if (!isConditionDefined(Cond) || (Rd == RegARM32::Reg_pc && SetFlags) ||
          (Rn == RegARM32::Reg_lr) || (Rn == RegARM32::Reg_pc && SetFlags))
        // Conditions of rule violated.
        break;
      constexpr uint32_t Add = B2; // 0100
      constexpr uint32_t InstType = 1;
      emitType01(Cond, InstType, Add, SetFlags, Rn, Rd, Src1Value);
      return;
    }
  } while (0);
  UnimplementedError(Ctx->getFlags());
}

void ARM32::AssemblerARM32::bkpt(uint16_t imm16) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  const uint32_t Encoding = (CondARM32::AL << kConditionShift) | B24 | B21 |
                            ((imm16 >> 4) << 8) | B6 | B5 | B4 | (imm16 & 0xf);
  emitInst(Encoding);
}

void ARM32::AssemblerARM32::bx(RegARM32::GPRRegister Rm, CondARM32::Cond Cond) {
  // cccc000100101111111111110001mmmm where mmmm=rm and cccc=Cond.
  // (ARM section A8.8.27, encoding A1).
  assert(isGPRRegisterDefined(Rm));
  // TODO(kschimpf): Remove void cast when MINIMAL build allows.
  (void) isGPRRegisterDefined(Rm);
  assert(isConditionDefined(Cond));
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  const uint32_t Encoding = (encodeCondition(Cond) << kConditionShift) | B24 |
                            B21 | (0xfff << 8) | B4 |
                            (encodeGPRRegister(Rm) << kRmShift);
  emitInst(Encoding);
}

void ARM32::AssemblerARM32::ldr(const Operand *OpRt, const Operand *OpAddress,
                                CondARM32::Cond Cond) {
  // Note: Loop is used so that we can short ciruit using break;
  do {
    uint32_t Rt;
    if (decodeOperand(OpRt, Rt) != DecodedAsRegister)
      break;
    uint32_t Address;
    if (decodeAddress(OpAddress, Address) != DecodedAsImmRegOffset)
      break;
    // cccc010pu0w1nnnnttttiiiiiiiiiiii (ARM section A8.8.63, encoding A1; and
    // section A8.6.68, encoding A1).
    constexpr uint32_t InstType = B1; // 010
    constexpr bool IsLoad = true;
    const Type Ty = OpRt->getType();
    if (!(Ty == IceType_i32 || Ty == IceType_i8)) // TODO(kschimpf) Expand?
      break;
    const bool IsByte = typeWidthInBytes(Ty) == 1;
    if ((getGPRReg(kRnShift, Address) == RegARM32::Encoded_Reg_pc) ||
        (!IsByte && !isBitSet(P, Address) && isBitSet(W, Address)) ||
        ((getGPRReg(kRnShift, Address) == RegARM32::Encoded_Reg_sp) &&
         !isBitSet(P, Address) &&
         isBitSet(U, Address) & !isBitSet(W, Address) &&
         (mask(Address, kImm12Shift, kImmed12Bits) == 0x8 /* 000000000100 */)))
      break;
    emitMemOp(Cond, InstType, IsLoad, IsByte, Rt, Address);
    return;
  } while (0);
  UnimplementedError(Ctx->getFlags());
}

void ARM32::AssemblerARM32::mov(const Operand *OpRd, const Operand *OpSrc,
                                CondARM32::Cond Cond) {
  // Note: Loop is used so that we can short ciruit using break;
  do {
    uint32_t Rd;
    if (decodeOperand(OpRd, Rd) != DecodedAsRegister)
      break;
    uint32_t Src;
    // TODO(kschimpf) Handle other forms of mov.
    if (decodeOperand(OpSrc, Src) == DecodedAsRotatedImm8) {
      // cccc0011101s0000ddddiiiiiiiiiiii (ARM section A8.8.102, encoding A1)
      // Note: We don't use movs in this assembler.
      constexpr bool SetFlags = false;
      if (!isConditionDefined(Cond) || (Rd == RegARM32::Reg_pc && SetFlags))
        // Conditions of rule violated.
        break;
      constexpr uint32_t Rn = 0;
      constexpr uint32_t Mov = B3 | B2 | B0; // 1101.
      constexpr uint32_t InstType = 1;
      emitType01(Cond, InstType, Mov, SetFlags, Rn, Rd, Src);
      return;
    }
  } while (0);
  UnimplementedError(Ctx->getFlags());
}

void ARM32::AssemblerARM32::str(const Operand *OpRt, const Operand *OpAddress,
                                CondARM32::Cond Cond) {
  // Note: Loop is used so that we can short ciruit using break;
  do {
    uint32_t Rt;
    if (decodeOperand(OpRt, Rt) != DecodedAsRegister)
      break;
    uint32_t Address;
    if (decodeAddress(OpAddress, Address) != DecodedAsImmRegOffset)
      break;
    // cccc010pub0nnnnttttiiiiiiiiiiii (ARM section A8.8.204, encoding A1; and
    // section 18.8.207, encoding A1).
    constexpr uint32_t InstType = B1; // 010
    constexpr bool IsLoad = false;
    const Type Ty = OpRt->getType();
    if (!(Ty == IceType_i32 || Ty == IceType_i8)) // TODO(kschimpf) Expand?
      break;
    const bool IsByte = typeWidthInBytes(Ty) == 1;
    // Check for rule violations.
    if ((getGPRReg(kRnShift, Address) == RegARM32::Encoded_Reg_pc) ||
        (!isBitSet(P, Address) && isBitSet(W, Address)) ||
        (!IsByte &&
         (getGPRReg(kRnShift, Address) == RegARM32::Encoded_Reg_sp) &&
         isBitSet(P, Address) && !isBitSet(U, Address) &&
         isBitSet(W, Address) &&
         (mask(Address, kImm12Shift, kImmed12Bits) == 0x8 /* 000000000100 */)))
      break;
    emitMemOp(Cond, InstType, IsLoad, IsByte, Rt, Address);
    return;
  } while (0);
  UnimplementedError(Ctx->getFlags());
}

void ARM32::AssemblerARM32::sub(const Operand *OpRd, const Operand *OpRn,
                                const Operand *OpSrc1, bool SetFlags,
                                CondARM32::Cond Cond) {
  // Note: Loop is used so that we can short circuit using break;
  do {
    uint32_t Rd;
    if (decodeOperand(OpRd, Rd) != DecodedAsRegister)
      break;
    uint32_t Rn;
    if (decodeOperand(OpRn, Rn) != DecodedAsRegister)
      break;
    uint32_t Src1Value;
    // TODO(kschimpf) Other possible decodings of add.
    if (decodeOperand(OpSrc1, Src1Value) == DecodedAsRotatedImm8) {
      // Sub (Immediate): See ARM section A8.8.222, rule A1.
      // cccc0010010snnnnddddiiiiiiiiiiii where cccc=Cond, dddd=Rd, nnnn=Rn,
      // s=SetFlags and iiiiiiiiiiii=Src1Value
      if (!isConditionDefined(Cond) || (Rd == RegARM32::Reg_pc && SetFlags) ||
          (Rn == RegARM32::Reg_lr) || (Rn == RegARM32::Reg_pc && SetFlags))
        // Conditions of rule violated.
        break;
      constexpr uint32_t Add = B1; // 0010
      constexpr uint32_t InstType = 1;
      emitType01(Cond, InstType, Add, SetFlags, Rn, Rd, Src1Value);
      return;
    }
  } while (0);
  UnimplementedError(Ctx->getFlags());
}

} // end of namespace Ice
