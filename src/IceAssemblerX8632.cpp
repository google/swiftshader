//===- subzero/src/IceAssemblerX8632.cpp - Assembler for x86-32  ----------===//
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

#include "IceAssemblerX8632.h"
#include "IceCfg.h"
#include "IceOperand.h"

namespace Ice {
namespace X8632 {

Address Address::ofConstPool(Assembler *Asm, const Constant *Imm) {
  AssemblerFixup *Fixup = Asm->createFixup(llvm::ELF::R_386_32, Imm);
  const RelocOffsetT Offset = 0;
  return Address::Absolute(Offset, Fixup);
}

AssemblerX8632::~AssemblerX8632() {
#ifndef NDEBUG
  for (const Label *Label : CfgNodeLabels) {
    Label->FinalCheck();
  }
  for (const Label *Label : LocalLabels) {
    Label->FinalCheck();
  }
#endif
}

void AssemblerX8632::alignFunction() {
  SizeT Align = 1 << getBundleAlignLog2Bytes();
  SizeT BytesNeeded = Utils::OffsetToAlignment(Buffer.getPosition(), Align);
  const SizeT HltSize = 1;
  while (BytesNeeded > 0) {
    hlt();
    BytesNeeded -= HltSize;
  }
}

Label *AssemblerX8632::GetOrCreateLabel(SizeT Number, LabelVector &Labels) {
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

Label *AssemblerX8632::GetOrCreateCfgNodeLabel(SizeT NodeNumber) {
  return GetOrCreateLabel(NodeNumber, CfgNodeLabels);
}

Label *AssemblerX8632::GetOrCreateLocalLabel(SizeT Number) {
  return GetOrCreateLabel(Number, LocalLabels);
}

void AssemblerX8632::bindCfgNodeLabel(SizeT NodeNumber) {
  assert(!getPreliminary());
  Label *L = GetOrCreateCfgNodeLabel(NodeNumber);
  this->bind(L);
}

void AssemblerX8632::BindLocalLabel(SizeT Number) {
  Label *L = GetOrCreateLocalLabel(Number);
  if (!getPreliminary())
    this->bind(L);
}

void AssemblerX8632::call(GPRRegister reg) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0xFF);
  emitRegisterOperand(2, reg);
}

void AssemblerX8632::call(const Address &address) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0xFF);
  emitOperand(2, address);
}

void AssemblerX8632::call(const ConstantRelocatable *label) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  intptr_t call_start = Buffer.getPosition();
  emitUint8(0xE8);
  emitFixup(this->createFixup(llvm::ELF::R_386_PC32, label));
  emitInt32(-4);
  assert((Buffer.getPosition() - call_start) == kCallExternalLabelSize);
  (void)call_start;
}

void AssemblerX8632::call(const Immediate &abs_address) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  intptr_t call_start = Buffer.getPosition();
  emitUint8(0xE8);
  emitFixup(
      this->createFixup(llvm::ELF::R_386_PC32, AssemblerFixup::NullSymbol));
  emitInt32(abs_address.value() - 4);
  assert((Buffer.getPosition() - call_start) == kCallExternalLabelSize);
  (void)call_start;
}

void AssemblerX8632::pushl(GPRRegister reg) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0x50 + reg);
}

void AssemblerX8632::popl(GPRRegister reg) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0x58 + reg);
}

void AssemblerX8632::popl(const Address &address) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0x8F);
  emitOperand(0, address);
}

void AssemblerX8632::pushal() {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0x60);
}

void AssemblerX8632::popal() {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0x61);
}

void AssemblerX8632::setcc(CondX86::BrCond condition, ByteRegister dst) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0x0F);
  emitUint8(0x90 + condition);
  emitUint8(0xC0 + dst);
}

void AssemblerX8632::setcc(CondX86::BrCond condition, const Address &address) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0x0F);
  emitUint8(0x90 + condition);
  emitOperand(0, address);
}

void AssemblerX8632::mov(Type Ty, GPRRegister dst, const Immediate &imm) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  if (isByteSizedType(Ty)) {
    emitUint8(0xB0 + dst);
    emitUint8(imm.value() & 0xFF);
    return;
  }
  if (Ty == IceType_i16)
    emitOperandSizeOverride();
  emitUint8(0xB8 + dst);
  emitImmediate(Ty, imm);
}

void AssemblerX8632::mov(Type Ty, GPRRegister dst, GPRRegister src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  if (Ty == IceType_i16)
    emitOperandSizeOverride();
  if (isByteSizedType(Ty)) {
    emitUint8(0x88);
  } else {
    emitUint8(0x89);
  }
  emitRegisterOperand(src, dst);
}

void AssemblerX8632::mov(Type Ty, GPRRegister dst, const Address &src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  if (Ty == IceType_i16)
    emitOperandSizeOverride();
  if (isByteSizedType(Ty)) {
    emitUint8(0x8A);
  } else {
    emitUint8(0x8B);
  }
  emitOperand(dst, src);
}

void AssemblerX8632::mov(Type Ty, const Address &dst, GPRRegister src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  if (Ty == IceType_i16)
    emitOperandSizeOverride();
  if (isByteSizedType(Ty)) {
    emitUint8(0x88);
  } else {
    emitUint8(0x89);
  }
  emitOperand(src, dst);
}

void AssemblerX8632::mov(Type Ty, const Address &dst, const Immediate &imm) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  if (Ty == IceType_i16)
    emitOperandSizeOverride();
  if (isByteSizedType(Ty)) {
    emitUint8(0xC6);
    emitOperand(0, dst);
    emitUint8(imm.value() & 0xFF);
  } else {
    emitUint8(0xC7);
    emitOperand(0, dst);
    emitImmediate(Ty, imm);
  }
}

void AssemblerX8632::movzx(Type SrcTy, GPRRegister dst, GPRRegister src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  bool ByteSized = isByteSizedType(SrcTy);
  assert(ByteSized || SrcTy == IceType_i16);
  emitUint8(0x0F);
  emitUint8(ByteSized ? 0xB6 : 0xB7);
  emitRegisterOperand(dst, src);
}

void AssemblerX8632::movzx(Type SrcTy, GPRRegister dst, const Address &src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  bool ByteSized = isByteSizedType(SrcTy);
  assert(ByteSized || SrcTy == IceType_i16);
  emitUint8(0x0F);
  emitUint8(ByteSized ? 0xB6 : 0xB7);
  emitOperand(dst, src);
}

void AssemblerX8632::movsx(Type SrcTy, GPRRegister dst, GPRRegister src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  bool ByteSized = isByteSizedType(SrcTy);
  assert(ByteSized || SrcTy == IceType_i16);
  emitUint8(0x0F);
  emitUint8(ByteSized ? 0xBE : 0xBF);
  emitRegisterOperand(dst, src);
}

void AssemblerX8632::movsx(Type SrcTy, GPRRegister dst, const Address &src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  bool ByteSized = isByteSizedType(SrcTy);
  assert(ByteSized || SrcTy == IceType_i16);
  emitUint8(0x0F);
  emitUint8(ByteSized ? 0xBE : 0xBF);
  emitOperand(dst, src);
}

void AssemblerX8632::lea(Type Ty, GPRRegister dst, const Address &src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  assert(Ty == IceType_i16 || Ty == IceType_i32);
  if (Ty == IceType_i16)
    emitOperandSizeOverride();
  emitUint8(0x8D);
  emitOperand(dst, src);
}

void AssemblerX8632::cmov(Type Ty, CondX86::BrCond cond, GPRRegister dst,
                          GPRRegister src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  if (Ty == IceType_i16)
    emitOperandSizeOverride();
  else
    assert(Ty == IceType_i32);
  emitUint8(0x0F);
  emitUint8(0x40 + cond);
  emitRegisterOperand(dst, src);
}

void AssemblerX8632::cmov(Type Ty, CondX86::BrCond cond, GPRRegister dst,
                          const Address &src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  if (Ty == IceType_i16)
    emitOperandSizeOverride();
  else
    assert(Ty == IceType_i32);
  emitUint8(0x0F);
  emitUint8(0x40 + cond);
  emitOperand(dst, src);
}

void AssemblerX8632::rep_movsb() {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0xF3);
  emitUint8(0xA4);
}

void AssemblerX8632::movss(Type Ty, XmmRegister dst, const Address &src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(isFloat32Asserting32Or64(Ty) ? 0xF3 : 0xF2);
  emitUint8(0x0F);
  emitUint8(0x10);
  emitOperand(dst, src);
}

void AssemblerX8632::movss(Type Ty, const Address &dst, XmmRegister src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(isFloat32Asserting32Or64(Ty) ? 0xF3 : 0xF2);
  emitUint8(0x0F);
  emitUint8(0x11);
  emitOperand(src, dst);
}

void AssemblerX8632::movss(Type Ty, XmmRegister dst, XmmRegister src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(isFloat32Asserting32Or64(Ty) ? 0xF3 : 0xF2);
  emitUint8(0x0F);
  emitUint8(0x11);
  emitXmmRegisterOperand(src, dst);
}

void AssemblerX8632::movd(XmmRegister dst, GPRRegister src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0x66);
  emitUint8(0x0F);
  emitUint8(0x6E);
  emitRegisterOperand(dst, src);
}

void AssemblerX8632::movd(XmmRegister dst, const Address &src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0x66);
  emitUint8(0x0F);
  emitUint8(0x6E);
  emitOperand(dst, src);
}

void AssemblerX8632::movd(GPRRegister dst, XmmRegister src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0x66);
  emitUint8(0x0F);
  emitUint8(0x7E);
  emitRegisterOperand(src, dst);
}

void AssemblerX8632::movd(const Address &dst, XmmRegister src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0x66);
  emitUint8(0x0F);
  emitUint8(0x7E);
  emitOperand(src, dst);
}

void AssemblerX8632::movq(XmmRegister dst, XmmRegister src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0xF3);
  emitUint8(0x0F);
  emitUint8(0x7E);
  emitRegisterOperand(dst, src);
}

void AssemblerX8632::movq(const Address &dst, XmmRegister src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0x66);
  emitUint8(0x0F);
  emitUint8(0xD6);
  emitOperand(src, dst);
}

void AssemblerX8632::movq(XmmRegister dst, const Address &src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0xF3);
  emitUint8(0x0F);
  emitUint8(0x7E);
  emitOperand(dst, src);
}

void AssemblerX8632::addss(Type Ty, XmmRegister dst, XmmRegister src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(isFloat32Asserting32Or64(Ty) ? 0xF3 : 0xF2);
  emitUint8(0x0F);
  emitUint8(0x58);
  emitXmmRegisterOperand(dst, src);
}

void AssemblerX8632::addss(Type Ty, XmmRegister dst, const Address &src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(isFloat32Asserting32Or64(Ty) ? 0xF3 : 0xF2);
  emitUint8(0x0F);
  emitUint8(0x58);
  emitOperand(dst, src);
}

void AssemblerX8632::subss(Type Ty, XmmRegister dst, XmmRegister src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(isFloat32Asserting32Or64(Ty) ? 0xF3 : 0xF2);
  emitUint8(0x0F);
  emitUint8(0x5C);
  emitXmmRegisterOperand(dst, src);
}

void AssemblerX8632::subss(Type Ty, XmmRegister dst, const Address &src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(isFloat32Asserting32Or64(Ty) ? 0xF3 : 0xF2);
  emitUint8(0x0F);
  emitUint8(0x5C);
  emitOperand(dst, src);
}

void AssemblerX8632::mulss(Type Ty, XmmRegister dst, XmmRegister src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(isFloat32Asserting32Or64(Ty) ? 0xF3 : 0xF2);
  emitUint8(0x0F);
  emitUint8(0x59);
  emitXmmRegisterOperand(dst, src);
}

void AssemblerX8632::mulss(Type Ty, XmmRegister dst, const Address &src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(isFloat32Asserting32Or64(Ty) ? 0xF3 : 0xF2);
  emitUint8(0x0F);
  emitUint8(0x59);
  emitOperand(dst, src);
}

void AssemblerX8632::divss(Type Ty, XmmRegister dst, XmmRegister src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(isFloat32Asserting32Or64(Ty) ? 0xF3 : 0xF2);
  emitUint8(0x0F);
  emitUint8(0x5E);
  emitXmmRegisterOperand(dst, src);
}

void AssemblerX8632::divss(Type Ty, XmmRegister dst, const Address &src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(isFloat32Asserting32Or64(Ty) ? 0xF3 : 0xF2);
  emitUint8(0x0F);
  emitUint8(0x5E);
  emitOperand(dst, src);
}

void AssemblerX8632::fld(Type Ty, const Address &src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(isFloat32Asserting32Or64(Ty) ? 0xD9 : 0xDD);
  emitOperand(0, src);
}

void AssemblerX8632::fstp(Type Ty, const Address &dst) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(isFloat32Asserting32Or64(Ty) ? 0xD9 : 0xDD);
  emitOperand(3, dst);
}

void AssemblerX8632::fstp(X87STRegister st) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0xDD);
  emitUint8(0xD8 + st);
}

void AssemblerX8632::movaps(XmmRegister dst, XmmRegister src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0x0F);
  emitUint8(0x28);
  emitXmmRegisterOperand(dst, src);
}

void AssemblerX8632::movups(XmmRegister dst, XmmRegister src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0x0F);
  emitUint8(0x10);
  emitRegisterOperand(dst, src);
}

void AssemblerX8632::movups(XmmRegister dst, const Address &src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0x0F);
  emitUint8(0x10);
  emitOperand(dst, src);
}

void AssemblerX8632::movups(const Address &dst, XmmRegister src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0x0F);
  emitUint8(0x11);
  emitOperand(src, dst);
}

void AssemblerX8632::padd(Type Ty, XmmRegister dst, XmmRegister src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0x66);
  emitUint8(0x0F);
  if (isByteSizedArithType(Ty)) {
    emitUint8(0xFC);
  } else if (Ty == IceType_i16) {
    emitUint8(0xFD);
  } else {
    emitUint8(0xFE);
  }
  emitXmmRegisterOperand(dst, src);
}

void AssemblerX8632::padd(Type Ty, XmmRegister dst, const Address &src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0x66);
  emitUint8(0x0F);
  if (isByteSizedArithType(Ty)) {
    emitUint8(0xFC);
  } else if (Ty == IceType_i16) {
    emitUint8(0xFD);
  } else {
    emitUint8(0xFE);
  }
  emitOperand(dst, src);
}

void AssemblerX8632::pand(Type /* Ty */, XmmRegister dst, XmmRegister src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0x66);
  emitUint8(0x0F);
  emitUint8(0xDB);
  emitXmmRegisterOperand(dst, src);
}

void AssemblerX8632::pand(Type /* Ty */, XmmRegister dst, const Address &src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0x66);
  emitUint8(0x0F);
  emitUint8(0xDB);
  emitOperand(dst, src);
}

void AssemblerX8632::pandn(Type /* Ty */, XmmRegister dst, XmmRegister src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0x66);
  emitUint8(0x0F);
  emitUint8(0xDF);
  emitXmmRegisterOperand(dst, src);
}

void AssemblerX8632::pandn(Type /* Ty */, XmmRegister dst, const Address &src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0x66);
  emitUint8(0x0F);
  emitUint8(0xDF);
  emitOperand(dst, src);
}

void AssemblerX8632::pmull(Type Ty, XmmRegister dst, XmmRegister src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0x66);
  emitUint8(0x0F);
  if (Ty == IceType_i16) {
    emitUint8(0xD5);
  } else {
    assert(Ty == IceType_i32);
    emitUint8(0x38);
    emitUint8(0x40);
  }
  emitXmmRegisterOperand(dst, src);
}

void AssemblerX8632::pmull(Type Ty, XmmRegister dst, const Address &src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0x66);
  emitUint8(0x0F);
  if (Ty == IceType_i16) {
    emitUint8(0xD5);
  } else {
    assert(Ty == IceType_i32);
    emitUint8(0x38);
    emitUint8(0x40);
  }
  emitOperand(dst, src);
}

void AssemblerX8632::pmuludq(Type /* Ty */, XmmRegister dst, XmmRegister src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0x66);
  emitUint8(0x0F);
  emitUint8(0xF4);
  emitXmmRegisterOperand(dst, src);
}

void AssemblerX8632::pmuludq(Type /* Ty */, XmmRegister dst,
                             const Address &src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0x66);
  emitUint8(0x0F);
  emitUint8(0xF4);
  emitOperand(dst, src);
}

void AssemblerX8632::por(Type /* Ty */, XmmRegister dst, XmmRegister src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0x66);
  emitUint8(0x0F);
  emitUint8(0xEB);
  emitXmmRegisterOperand(dst, src);
}

void AssemblerX8632::por(Type /* Ty */, XmmRegister dst, const Address &src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0x66);
  emitUint8(0x0F);
  emitUint8(0xEB);
  emitOperand(dst, src);
}

void AssemblerX8632::psub(Type Ty, XmmRegister dst, XmmRegister src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0x66);
  emitUint8(0x0F);
  if (isByteSizedArithType(Ty)) {
    emitUint8(0xF8);
  } else if (Ty == IceType_i16) {
    emitUint8(0xF9);
  } else {
    emitUint8(0xFA);
  }
  emitXmmRegisterOperand(dst, src);
}

void AssemblerX8632::psub(Type Ty, XmmRegister dst, const Address &src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0x66);
  emitUint8(0x0F);
  if (isByteSizedArithType(Ty)) {
    emitUint8(0xF8);
  } else if (Ty == IceType_i16) {
    emitUint8(0xF9);
  } else {
    emitUint8(0xFA);
  }
  emitOperand(dst, src);
}

void AssemblerX8632::pxor(Type /* Ty */, XmmRegister dst, XmmRegister src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0x66);
  emitUint8(0x0F);
  emitUint8(0xEF);
  emitXmmRegisterOperand(dst, src);
}

void AssemblerX8632::pxor(Type /* Ty */, XmmRegister dst, const Address &src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0x66);
  emitUint8(0x0F);
  emitUint8(0xEF);
  emitOperand(dst, src);
}

void AssemblerX8632::psll(Type Ty, XmmRegister dst, XmmRegister src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0x66);
  emitUint8(0x0F);
  if (Ty == IceType_i16) {
    emitUint8(0xF1);
  } else {
    assert(Ty == IceType_i32);
    emitUint8(0xF2);
  }
  emitXmmRegisterOperand(dst, src);
}

void AssemblerX8632::psll(Type Ty, XmmRegister dst, const Address &src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0x66);
  emitUint8(0x0F);
  if (Ty == IceType_i16) {
    emitUint8(0xF1);
  } else {
    assert(Ty == IceType_i32);
    emitUint8(0xF2);
  }
  emitOperand(dst, src);
}

void AssemblerX8632::psll(Type Ty, XmmRegister dst, const Immediate &imm) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  assert(imm.is_int8());
  emitUint8(0x66);
  emitUint8(0x0F);
  if (Ty == IceType_i16) {
    emitUint8(0x71);
  } else {
    assert(Ty == IceType_i32);
    emitUint8(0x72);
  }
  emitRegisterOperand(6, dst);
  emitUint8(imm.value() & 0xFF);
}

void AssemblerX8632::psra(Type Ty, XmmRegister dst, XmmRegister src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0x66);
  emitUint8(0x0F);
  if (Ty == IceType_i16) {
    emitUint8(0xE1);
  } else {
    assert(Ty == IceType_i32);
    emitUint8(0xE2);
  }
  emitXmmRegisterOperand(dst, src);
}

void AssemblerX8632::psra(Type Ty, XmmRegister dst, const Address &src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0x66);
  emitUint8(0x0F);
  if (Ty == IceType_i16) {
    emitUint8(0xE1);
  } else {
    assert(Ty == IceType_i32);
    emitUint8(0xE2);
  }
  emitOperand(dst, src);
}

void AssemblerX8632::psra(Type Ty, XmmRegister dst, const Immediate &imm) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  assert(imm.is_int8());
  emitUint8(0x66);
  emitUint8(0x0F);
  if (Ty == IceType_i16) {
    emitUint8(0x71);
  } else {
    assert(Ty == IceType_i32);
    emitUint8(0x72);
  }
  emitRegisterOperand(4, dst);
  emitUint8(imm.value() & 0xFF);
}

void AssemblerX8632::psrl(Type Ty, XmmRegister dst, XmmRegister src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0x66);
  emitUint8(0x0F);
  if (Ty == IceType_i16) {
    emitUint8(0xD1);
  } else if (Ty == IceType_f64) {
    emitUint8(0xD3);
  } else {
    assert(Ty == IceType_i32 || Ty == IceType_f32 || Ty == IceType_v4f32);
    emitUint8(0xD2);
  }
  emitXmmRegisterOperand(dst, src);
}

void AssemblerX8632::psrl(Type Ty, XmmRegister dst, const Address &src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0x66);
  emitUint8(0x0F);
  if (Ty == IceType_i16) {
    emitUint8(0xD1);
  } else if (Ty == IceType_f64) {
    emitUint8(0xD3);
  } else {
    assert(Ty == IceType_i32 || Ty == IceType_f32 || Ty == IceType_v4f32);
    emitUint8(0xD2);
  }
  emitOperand(dst, src);
}

void AssemblerX8632::psrl(Type Ty, XmmRegister dst, const Immediate &imm) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  assert(imm.is_int8());
  emitUint8(0x66);
  emitUint8(0x0F);
  if (Ty == IceType_i16) {
    emitUint8(0x71);
  } else if (Ty == IceType_f64) {
    emitUint8(0x73);
  } else {
    assert(Ty == IceType_i32 || Ty == IceType_f32 || Ty == IceType_v4f32);
    emitUint8(0x72);
  }
  emitRegisterOperand(2, dst);
  emitUint8(imm.value() & 0xFF);
}

// {add,sub,mul,div}ps are given a Ty parameter for consistency with
// {add,sub,mul,div}ss. In the future, when the PNaCl ABI allows
// addpd, etc., we can use the Ty parameter to decide on adding
// a 0x66 prefix.
void AssemblerX8632::addps(Type /* Ty */, XmmRegister dst, XmmRegister src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0x0F);
  emitUint8(0x58);
  emitXmmRegisterOperand(dst, src);
}

void AssemblerX8632::addps(Type /* Ty */, XmmRegister dst, const Address &src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0x0F);
  emitUint8(0x58);
  emitOperand(dst, src);
}

void AssemblerX8632::subps(Type /* Ty */, XmmRegister dst, XmmRegister src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0x0F);
  emitUint8(0x5C);
  emitXmmRegisterOperand(dst, src);
}

void AssemblerX8632::subps(Type /* Ty */, XmmRegister dst, const Address &src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0x0F);
  emitUint8(0x5C);
  emitOperand(dst, src);
}

void AssemblerX8632::divps(Type /* Ty */, XmmRegister dst, XmmRegister src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0x0F);
  emitUint8(0x5E);
  emitXmmRegisterOperand(dst, src);
}

void AssemblerX8632::divps(Type /* Ty */, XmmRegister dst, const Address &src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0x0F);
  emitUint8(0x5E);
  emitOperand(dst, src);
}

void AssemblerX8632::mulps(Type /* Ty */, XmmRegister dst, XmmRegister src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0x0F);
  emitUint8(0x59);
  emitXmmRegisterOperand(dst, src);
}

void AssemblerX8632::mulps(Type /* Ty */, XmmRegister dst, const Address &src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0x0F);
  emitUint8(0x59);
  emitOperand(dst, src);
}

void AssemblerX8632::minps(XmmRegister dst, XmmRegister src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0x0F);
  emitUint8(0x5D);
  emitXmmRegisterOperand(dst, src);
}

void AssemblerX8632::maxps(XmmRegister dst, XmmRegister src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0x0F);
  emitUint8(0x5F);
  emitXmmRegisterOperand(dst, src);
}

void AssemblerX8632::andps(XmmRegister dst, XmmRegister src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0x0F);
  emitUint8(0x54);
  emitXmmRegisterOperand(dst, src);
}

void AssemblerX8632::andps(XmmRegister dst, const Address &src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0x0F);
  emitUint8(0x54);
  emitOperand(dst, src);
}

void AssemblerX8632::orps(XmmRegister dst, XmmRegister src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0x0F);
  emitUint8(0x56);
  emitXmmRegisterOperand(dst, src);
}

void AssemblerX8632::blendvps(Type /* Ty */, XmmRegister dst, XmmRegister src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0x66);
  emitUint8(0x0F);
  emitUint8(0x38);
  emitUint8(0x14);
  emitXmmRegisterOperand(dst, src);
}

void AssemblerX8632::blendvps(Type /* Ty */, XmmRegister dst,
                              const Address &src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0x66);
  emitUint8(0x0F);
  emitUint8(0x38);
  emitUint8(0x14);
  emitOperand(dst, src);
}

void AssemblerX8632::pblendvb(Type /* Ty */, XmmRegister dst, XmmRegister src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0x66);
  emitUint8(0x0F);
  emitUint8(0x38);
  emitUint8(0x10);
  emitXmmRegisterOperand(dst, src);
}

void AssemblerX8632::pblendvb(Type /* Ty */, XmmRegister dst,
                              const Address &src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0x66);
  emitUint8(0x0F);
  emitUint8(0x38);
  emitUint8(0x10);
  emitOperand(dst, src);
}

void AssemblerX8632::cmpps(XmmRegister dst, XmmRegister src,
                           CondX86::CmppsCond CmpCondition) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0x0F);
  emitUint8(0xC2);
  emitXmmRegisterOperand(dst, src);
  emitUint8(CmpCondition);
}

void AssemblerX8632::cmpps(XmmRegister dst, const Address &src,
                           CondX86::CmppsCond CmpCondition) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0x0F);
  emitUint8(0xC2);
  emitOperand(dst, src);
  emitUint8(CmpCondition);
}

void AssemblerX8632::sqrtps(XmmRegister dst) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0x0F);
  emitUint8(0x51);
  emitXmmRegisterOperand(dst, dst);
}

void AssemblerX8632::rsqrtps(XmmRegister dst) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0x0F);
  emitUint8(0x52);
  emitXmmRegisterOperand(dst, dst);
}

void AssemblerX8632::reciprocalps(XmmRegister dst) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0x0F);
  emitUint8(0x53);
  emitXmmRegisterOperand(dst, dst);
}

void AssemblerX8632::movhlps(XmmRegister dst, XmmRegister src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0x0F);
  emitUint8(0x12);
  emitXmmRegisterOperand(dst, src);
}

void AssemblerX8632::movlhps(XmmRegister dst, XmmRegister src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0x0F);
  emitUint8(0x16);
  emitXmmRegisterOperand(dst, src);
}

void AssemblerX8632::unpcklps(XmmRegister dst, XmmRegister src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0x0F);
  emitUint8(0x14);
  emitXmmRegisterOperand(dst, src);
}

void AssemblerX8632::unpckhps(XmmRegister dst, XmmRegister src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0x0F);
  emitUint8(0x15);
  emitXmmRegisterOperand(dst, src);
}

void AssemblerX8632::unpcklpd(XmmRegister dst, XmmRegister src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0x66);
  emitUint8(0x0F);
  emitUint8(0x14);
  emitXmmRegisterOperand(dst, src);
}

void AssemblerX8632::unpckhpd(XmmRegister dst, XmmRegister src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0x66);
  emitUint8(0x0F);
  emitUint8(0x15);
  emitXmmRegisterOperand(dst, src);
}

void AssemblerX8632::set1ps(XmmRegister dst, GPRRegister tmp1,
                            const Immediate &imm) {
  // Load 32-bit immediate value into tmp1.
  mov(IceType_i32, tmp1, imm);
  // Move value from tmp1 into dst.
  movd(dst, tmp1);
  // Broadcast low lane into other three lanes.
  shufps(dst, dst, Immediate(0x0));
}

void AssemblerX8632::shufps(XmmRegister dst, XmmRegister src,
                            const Immediate &imm) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0x0F);
  emitUint8(0xC6);
  emitXmmRegisterOperand(dst, src);
  assert(imm.is_uint8());
  emitUint8(imm.value());
}

void AssemblerX8632::pshufd(Type /* Ty */, XmmRegister dst, XmmRegister src,
                            const Immediate &imm) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0x66);
  emitUint8(0x0F);
  emitUint8(0x70);
  emitXmmRegisterOperand(dst, src);
  assert(imm.is_uint8());
  emitUint8(imm.value());
}

void AssemblerX8632::pshufd(Type /* Ty */, XmmRegister dst, const Address &src,
                            const Immediate &imm) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0x66);
  emitUint8(0x0F);
  emitUint8(0x70);
  emitOperand(dst, src);
  assert(imm.is_uint8());
  emitUint8(imm.value());
}

void AssemblerX8632::shufps(Type /* Ty */, XmmRegister dst, XmmRegister src,
                            const Immediate &imm) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0x0F);
  emitUint8(0xC6);
  emitXmmRegisterOperand(dst, src);
  assert(imm.is_uint8());
  emitUint8(imm.value());
}

void AssemblerX8632::shufps(Type /* Ty */, XmmRegister dst, const Address &src,
                            const Immediate &imm) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0x0F);
  emitUint8(0xC6);
  emitOperand(dst, src);
  assert(imm.is_uint8());
  emitUint8(imm.value());
}

void AssemblerX8632::minpd(XmmRegister dst, XmmRegister src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0x66);
  emitUint8(0x0F);
  emitUint8(0x5D);
  emitXmmRegisterOperand(dst, src);
}

void AssemblerX8632::maxpd(XmmRegister dst, XmmRegister src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0x66);
  emitUint8(0x0F);
  emitUint8(0x5F);
  emitXmmRegisterOperand(dst, src);
}

void AssemblerX8632::sqrtpd(XmmRegister dst) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0x66);
  emitUint8(0x0F);
  emitUint8(0x51);
  emitXmmRegisterOperand(dst, dst);
}

void AssemblerX8632::shufpd(XmmRegister dst, XmmRegister src,
                            const Immediate &imm) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0x66);
  emitUint8(0x0F);
  emitUint8(0xC6);
  emitXmmRegisterOperand(dst, src);
  assert(imm.is_uint8());
  emitUint8(imm.value());
}

void AssemblerX8632::cvtdq2ps(Type /* Ignore */, XmmRegister dst,
                              XmmRegister src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0x0F);
  emitUint8(0x5B);
  emitXmmRegisterOperand(dst, src);
}

void AssemblerX8632::cvtdq2ps(Type /* Ignore */, XmmRegister dst,
                              const Address &src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0x0F);
  emitUint8(0x5B);
  emitOperand(dst, src);
}

void AssemblerX8632::cvttps2dq(Type /* Ignore */, XmmRegister dst,
                               XmmRegister src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0xF3);
  emitUint8(0x0F);
  emitUint8(0x5B);
  emitXmmRegisterOperand(dst, src);
}

void AssemblerX8632::cvttps2dq(Type /* Ignore */, XmmRegister dst,
                               const Address &src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0xF3);
  emitUint8(0x0F);
  emitUint8(0x5B);
  emitOperand(dst, src);
}

void AssemblerX8632::cvtsi2ss(Type DestTy, XmmRegister dst, GPRRegister src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(isFloat32Asserting32Or64(DestTy) ? 0xF3 : 0xF2);
  emitUint8(0x0F);
  emitUint8(0x2A);
  emitRegisterOperand(dst, src);
}

void AssemblerX8632::cvtsi2ss(Type DestTy, XmmRegister dst,
                              const Address &src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(isFloat32Asserting32Or64(DestTy) ? 0xF3 : 0xF2);
  emitUint8(0x0F);
  emitUint8(0x2A);
  emitOperand(dst, src);
}

void AssemblerX8632::cvtfloat2float(Type SrcTy, XmmRegister dst,
                                    XmmRegister src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  // ss2sd or sd2ss
  emitUint8(isFloat32Asserting32Or64(SrcTy) ? 0xF3 : 0xF2);
  emitUint8(0x0F);
  emitUint8(0x5A);
  emitXmmRegisterOperand(dst, src);
}

void AssemblerX8632::cvtfloat2float(Type SrcTy, XmmRegister dst,
                                    const Address &src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(isFloat32Asserting32Or64(SrcTy) ? 0xF3 : 0xF2);
  emitUint8(0x0F);
  emitUint8(0x5A);
  emitOperand(dst, src);
}

void AssemblerX8632::cvttss2si(Type SrcTy, GPRRegister dst, XmmRegister src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(isFloat32Asserting32Or64(SrcTy) ? 0xF3 : 0xF2);
  emitUint8(0x0F);
  emitUint8(0x2C);
  emitXmmRegisterOperand(dst, src);
}

void AssemblerX8632::cvttss2si(Type SrcTy, GPRRegister dst,
                               const Address &src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(isFloat32Asserting32Or64(SrcTy) ? 0xF3 : 0xF2);
  emitUint8(0x0F);
  emitUint8(0x2C);
  emitOperand(dst, src);
}

void AssemblerX8632::ucomiss(Type Ty, XmmRegister a, XmmRegister b) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  if (Ty == IceType_f64)
    emitUint8(0x66);
  emitUint8(0x0F);
  emitUint8(0x2E);
  emitXmmRegisterOperand(a, b);
}

void AssemblerX8632::ucomiss(Type Ty, XmmRegister a, const Address &b) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  if (Ty == IceType_f64)
    emitUint8(0x66);
  emitUint8(0x0F);
  emitUint8(0x2E);
  emitOperand(a, b);
}

void AssemblerX8632::movmskpd(GPRRegister dst, XmmRegister src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0x66);
  emitUint8(0x0F);
  emitUint8(0x50);
  emitXmmRegisterOperand(dst, src);
}

void AssemblerX8632::movmskps(GPRRegister dst, XmmRegister src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0x0F);
  emitUint8(0x50);
  emitXmmRegisterOperand(dst, src);
}

void AssemblerX8632::sqrtss(Type Ty, XmmRegister dst, const Address &src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(isFloat32Asserting32Or64(Ty) ? 0xF3 : 0xF2);
  emitUint8(0x0F);
  emitUint8(0x51);
  emitOperand(dst, src);
}

void AssemblerX8632::sqrtss(Type Ty, XmmRegister dst, XmmRegister src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(isFloat32Asserting32Or64(Ty) ? 0xF3 : 0xF2);
  emitUint8(0x0F);
  emitUint8(0x51);
  emitXmmRegisterOperand(dst, src);
}

void AssemblerX8632::xorpd(XmmRegister dst, const Address &src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0x66);
  emitUint8(0x0F);
  emitUint8(0x57);
  emitOperand(dst, src);
}

void AssemblerX8632::xorpd(XmmRegister dst, XmmRegister src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0x66);
  emitUint8(0x0F);
  emitUint8(0x57);
  emitXmmRegisterOperand(dst, src);
}

void AssemblerX8632::orpd(XmmRegister dst, XmmRegister src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0x66);
  emitUint8(0x0F);
  emitUint8(0x56);
  emitXmmRegisterOperand(dst, src);
}

void AssemblerX8632::xorps(XmmRegister dst, const Address &src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0x0F);
  emitUint8(0x57);
  emitOperand(dst, src);
}

void AssemblerX8632::xorps(XmmRegister dst, XmmRegister src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0x0F);
  emitUint8(0x57);
  emitXmmRegisterOperand(dst, src);
}

void AssemblerX8632::andpd(XmmRegister dst, const Address &src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0x66);
  emitUint8(0x0F);
  emitUint8(0x54);
  emitOperand(dst, src);
}

void AssemblerX8632::andpd(XmmRegister dst, XmmRegister src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0x66);
  emitUint8(0x0F);
  emitUint8(0x54);
  emitXmmRegisterOperand(dst, src);
}

void AssemblerX8632::insertps(Type Ty, XmmRegister dst, XmmRegister src,
                              const Immediate &imm) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  assert(imm.is_uint8());
  assert(isVectorFloatingType(Ty));
  (void)Ty;
  emitUint8(0x66);
  emitUint8(0x0F);
  emitUint8(0x3A);
  emitUint8(0x21);
  emitXmmRegisterOperand(dst, src);
  emitUint8(imm.value());
}

void AssemblerX8632::insertps(Type Ty, XmmRegister dst, const Address &src,
                              const Immediate &imm) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  assert(imm.is_uint8());
  assert(isVectorFloatingType(Ty));
  (void)Ty;
  emitUint8(0x66);
  emitUint8(0x0F);
  emitUint8(0x3A);
  emitUint8(0x21);
  emitOperand(dst, src);
  emitUint8(imm.value());
}

void AssemblerX8632::pinsr(Type Ty, XmmRegister dst, GPRRegister src,
                           const Immediate &imm) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  assert(imm.is_uint8());
  if (Ty == IceType_i16) {
    emitUint8(0x66);
    emitUint8(0x0F);
    emitUint8(0xC4);
    emitXmmRegisterOperand(dst, XmmRegister(src));
    emitUint8(imm.value());
  } else {
    emitUint8(0x66);
    emitUint8(0x0F);
    emitUint8(0x3A);
    emitUint8(isByteSizedType(Ty) ? 0x20 : 0x22);
    emitXmmRegisterOperand(dst, XmmRegister(src));
    emitUint8(imm.value());
  }
}

void AssemblerX8632::pinsr(Type Ty, XmmRegister dst, const Address &src,
                           const Immediate &imm) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  assert(imm.is_uint8());
  if (Ty == IceType_i16) {
    emitUint8(0x66);
    emitUint8(0x0F);
    emitUint8(0xC4);
    emitOperand(dst, src);
    emitUint8(imm.value());
  } else {
    emitUint8(0x66);
    emitUint8(0x0F);
    emitUint8(0x3A);
    emitUint8(isByteSizedType(Ty) ? 0x20 : 0x22);
    emitOperand(dst, src);
    emitUint8(imm.value());
  }
}

void AssemblerX8632::pextr(Type Ty, GPRRegister dst, XmmRegister src,
                           const Immediate &imm) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  assert(imm.is_uint8());
  if (Ty == IceType_i16) {
    emitUint8(0x66);
    emitUint8(0x0F);
    emitUint8(0xC5);
    emitXmmRegisterOperand(XmmRegister(dst), src);
    emitUint8(imm.value());
  } else {
    emitUint8(0x66);
    emitUint8(0x0F);
    emitUint8(0x3A);
    emitUint8(isByteSizedType(Ty) ? 0x14 : 0x16);
    // SSE 4.1 versions are "MRI" because dst can be mem, while
    // pextrw (SSE2) is RMI because dst must be reg.
    emitXmmRegisterOperand(src, XmmRegister(dst));
    emitUint8(imm.value());
  }
}

void AssemblerX8632::pmovsxdq(XmmRegister dst, XmmRegister src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0x66);
  emitUint8(0x0F);
  emitUint8(0x38);
  emitUint8(0x25);
  emitXmmRegisterOperand(dst, src);
}

void AssemblerX8632::pcmpeq(Type Ty, XmmRegister dst, XmmRegister src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0x66);
  emitUint8(0x0F);
  if (isByteSizedArithType(Ty)) {
    emitUint8(0x74);
  } else if (Ty == IceType_i16) {
    emitUint8(0x75);
  } else {
    emitUint8(0x76);
  }
  emitXmmRegisterOperand(dst, src);
}

void AssemblerX8632::pcmpeq(Type Ty, XmmRegister dst, const Address &src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0x66);
  emitUint8(0x0F);
  if (isByteSizedArithType(Ty)) {
    emitUint8(0x74);
  } else if (Ty == IceType_i16) {
    emitUint8(0x75);
  } else {
    emitUint8(0x76);
  }
  emitOperand(dst, src);
}

void AssemblerX8632::pcmpgt(Type Ty, XmmRegister dst, XmmRegister src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0x66);
  emitUint8(0x0F);
  if (isByteSizedArithType(Ty)) {
    emitUint8(0x64);
  } else if (Ty == IceType_i16) {
    emitUint8(0x65);
  } else {
    emitUint8(0x66);
  }
  emitXmmRegisterOperand(dst, src);
}

void AssemblerX8632::pcmpgt(Type Ty, XmmRegister dst, const Address &src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0x66);
  emitUint8(0x0F);
  if (isByteSizedArithType(Ty)) {
    emitUint8(0x64);
  } else if (Ty == IceType_i16) {
    emitUint8(0x65);
  } else {
    emitUint8(0x66);
  }
  emitOperand(dst, src);
}

void AssemblerX8632::roundsd(XmmRegister dst, XmmRegister src,
                             RoundingMode mode) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0x66);
  emitUint8(0x0F);
  emitUint8(0x3A);
  emitUint8(0x0B);
  emitXmmRegisterOperand(dst, src);
  // Mask precision exeption.
  emitUint8(static_cast<uint8_t>(mode) | 0x8);
}

void AssemblerX8632::fnstcw(const Address &dst) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0xD9);
  emitOperand(7, dst);
}

void AssemblerX8632::fldcw(const Address &src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0xD9);
  emitOperand(5, src);
}

void AssemblerX8632::fistpl(const Address &dst) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0xDF);
  emitOperand(7, dst);
}

void AssemblerX8632::fistps(const Address &dst) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0xDB);
  emitOperand(3, dst);
}

void AssemblerX8632::fildl(const Address &src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0xDF);
  emitOperand(5, src);
}

void AssemblerX8632::filds(const Address &src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0xDB);
  emitOperand(0, src);
}

void AssemblerX8632::fincstp() {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0xD9);
  emitUint8(0xF7);
}

template <uint32_t Tag>
void AssemblerX8632::arith_int(Type Ty, GPRRegister reg, const Immediate &imm) {
  static_assert(Tag < 8, "Tag must be between 0..7");
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  if (isByteSizedType(Ty)) {
    emitComplexI8(Tag, Operand(reg), imm);
    return;
  }
  if (Ty == IceType_i16)
    emitOperandSizeOverride();
  emitComplex(Ty, Tag, Operand(reg), imm);
}

template <uint32_t Tag>
void AssemblerX8632::arith_int(Type Ty, GPRRegister reg0, GPRRegister reg1) {
  static_assert(Tag < 8, "Tag must be between 0..7");
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  if (Ty == IceType_i16)
    emitOperandSizeOverride();
  if (isByteSizedType(Ty))
    emitUint8(Tag * 8 + 2);
  else
    emitUint8(Tag * 8 + 3);
  emitRegisterOperand(reg0, reg1);
}

template <uint32_t Tag>
void AssemblerX8632::arith_int(Type Ty, GPRRegister reg,
                               const Address &address) {
  static_assert(Tag < 8, "Tag must be between 0..7");
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  if (Ty == IceType_i16)
    emitOperandSizeOverride();
  if (isByteSizedType(Ty))
    emitUint8(Tag * 8 + 2);
  else
    emitUint8(Tag * 8 + 3);
  emitOperand(reg, address);
}

template <uint32_t Tag>
void AssemblerX8632::arith_int(Type Ty, const Address &address,
                               GPRRegister reg) {
  static_assert(Tag < 8, "Tag must be between 0..7");
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  if (Ty == IceType_i16)
    emitOperandSizeOverride();
  if (isByteSizedType(Ty))
    emitUint8(Tag * 8 + 0);
  else
    emitUint8(Tag * 8 + 1);
  emitOperand(reg, address);
}

template <uint32_t Tag>
void AssemblerX8632::arith_int(Type Ty, const Address &address,
                               const Immediate &imm) {
  static_assert(Tag < 8, "Tag must be between 0..7");
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  if (isByteSizedType(Ty)) {
    emitComplexI8(Tag, address, imm);
    return;
  }
  if (Ty == IceType_i16)
    emitOperandSizeOverride();
  emitComplex(Ty, Tag, address, imm);
}

void AssemblerX8632::cmp(Type Ty, GPRRegister reg, const Immediate &imm) {
  arith_int<7>(Ty, reg, imm);
}

void AssemblerX8632::cmp(Type Ty, GPRRegister reg0, GPRRegister reg1) {
  arith_int<7>(Ty, reg0, reg1);
}

void AssemblerX8632::cmp(Type Ty, GPRRegister reg, const Address &address) {
  arith_int<7>(Ty, reg, address);
}

void AssemblerX8632::cmp(Type Ty, const Address &address, GPRRegister reg) {
  arith_int<7>(Ty, address, reg);
}

void AssemblerX8632::cmp(Type Ty, const Address &address,
                         const Immediate &imm) {
  arith_int<7>(Ty, address, imm);
}

void AssemblerX8632::test(Type Ty, GPRRegister reg1, GPRRegister reg2) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  if (Ty == IceType_i16)
    emitOperandSizeOverride();
  if (isByteSizedType(Ty))
    emitUint8(0x84);
  else
    emitUint8(0x85);
  emitRegisterOperand(reg1, reg2);
}

void AssemblerX8632::test(Type Ty, const Address &addr, GPRRegister reg) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  if (Ty == IceType_i16)
    emitOperandSizeOverride();
  if (isByteSizedType(Ty))
    emitUint8(0x84);
  else
    emitUint8(0x85);
  emitOperand(reg, addr);
}

void AssemblerX8632::test(Type Ty, GPRRegister reg,
                          const Immediate &immediate) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  // For registers that have a byte variant (EAX, EBX, ECX, and EDX)
  // we only test the byte register to keep the encoding short.
  // This is legal even if the register had high bits set since
  // this only sets flags registers based on the "AND" of the two operands,
  // and the immediate had zeros at those high bits.
  if (immediate.is_uint8() && reg < 4) {
    // Use zero-extended 8-bit immediate.
    if (reg == RegX8632::Encoded_Reg_eax) {
      emitUint8(0xA8);
    } else {
      emitUint8(0xF6);
      emitUint8(0xC0 + reg);
    }
    emitUint8(immediate.value() & 0xFF);
  } else if (reg == RegX8632::Encoded_Reg_eax) {
    // Use short form if the destination is EAX.
    if (Ty == IceType_i16)
      emitOperandSizeOverride();
    emitUint8(0xA9);
    emitImmediate(Ty, immediate);
  } else {
    if (Ty == IceType_i16)
      emitOperandSizeOverride();
    emitUint8(0xF7);
    emitRegisterOperand(0, reg);
    emitImmediate(Ty, immediate);
  }
}

void AssemblerX8632::test(Type Ty, const Address &addr,
                          const Immediate &immediate) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  // If the immediate is short, we only test the byte addr to keep the
  // encoding short.
  if (immediate.is_uint8()) {
    // Use zero-extended 8-bit immediate.
    emitUint8(0xF6);
    emitOperand(0, addr);
    emitUint8(immediate.value() & 0xFF);
  } else {
    if (Ty == IceType_i16)
      emitOperandSizeOverride();
    emitUint8(0xF7);
    emitOperand(0, addr);
    emitImmediate(Ty, immediate);
  }
}

void AssemblerX8632::And(Type Ty, GPRRegister dst, GPRRegister src) {
  arith_int<4>(Ty, dst, src);
}

void AssemblerX8632::And(Type Ty, GPRRegister dst, const Address &address) {
  arith_int<4>(Ty, dst, address);
}

void AssemblerX8632::And(Type Ty, GPRRegister dst, const Immediate &imm) {
  arith_int<4>(Ty, dst, imm);
}

void AssemblerX8632::And(Type Ty, const Address &address, GPRRegister reg) {
  arith_int<4>(Ty, address, reg);
}

void AssemblerX8632::And(Type Ty, const Address &address,
                         const Immediate &imm) {
  arith_int<4>(Ty, address, imm);
}

void AssemblerX8632::Or(Type Ty, GPRRegister dst, GPRRegister src) {
  arith_int<1>(Ty, dst, src);
}

void AssemblerX8632::Or(Type Ty, GPRRegister dst, const Address &address) {
  arith_int<1>(Ty, dst, address);
}

void AssemblerX8632::Or(Type Ty, GPRRegister dst, const Immediate &imm) {
  arith_int<1>(Ty, dst, imm);
}

void AssemblerX8632::Or(Type Ty, const Address &address, GPRRegister reg) {
  arith_int<1>(Ty, address, reg);
}

void AssemblerX8632::Or(Type Ty, const Address &address, const Immediate &imm) {
  arith_int<1>(Ty, address, imm);
}

void AssemblerX8632::Xor(Type Ty, GPRRegister dst, GPRRegister src) {
  arith_int<6>(Ty, dst, src);
}

void AssemblerX8632::Xor(Type Ty, GPRRegister dst, const Address &address) {
  arith_int<6>(Ty, dst, address);
}

void AssemblerX8632::Xor(Type Ty, GPRRegister dst, const Immediate &imm) {
  arith_int<6>(Ty, dst, imm);
}

void AssemblerX8632::Xor(Type Ty, const Address &address, GPRRegister reg) {
  arith_int<6>(Ty, address, reg);
}

void AssemblerX8632::Xor(Type Ty, const Address &address,
                         const Immediate &imm) {
  arith_int<6>(Ty, address, imm);
}

void AssemblerX8632::add(Type Ty, GPRRegister dst, GPRRegister src) {
  arith_int<0>(Ty, dst, src);
}

void AssemblerX8632::add(Type Ty, GPRRegister reg, const Address &address) {
  arith_int<0>(Ty, reg, address);
}

void AssemblerX8632::add(Type Ty, GPRRegister reg, const Immediate &imm) {
  arith_int<0>(Ty, reg, imm);
}

void AssemblerX8632::add(Type Ty, const Address &address, GPRRegister reg) {
  arith_int<0>(Ty, address, reg);
}

void AssemblerX8632::add(Type Ty, const Address &address,
                         const Immediate &imm) {
  arith_int<0>(Ty, address, imm);
}

void AssemblerX8632::adc(Type Ty, GPRRegister dst, GPRRegister src) {
  arith_int<2>(Ty, dst, src);
}

void AssemblerX8632::adc(Type Ty, GPRRegister dst, const Address &address) {
  arith_int<2>(Ty, dst, address);
}

void AssemblerX8632::adc(Type Ty, GPRRegister reg, const Immediate &imm) {
  arith_int<2>(Ty, reg, imm);
}

void AssemblerX8632::adc(Type Ty, const Address &address, GPRRegister reg) {
  arith_int<2>(Ty, address, reg);
}

void AssemblerX8632::adc(Type Ty, const Address &address,
                         const Immediate &imm) {
  arith_int<2>(Ty, address, imm);
}

void AssemblerX8632::sub(Type Ty, GPRRegister dst, GPRRegister src) {
  arith_int<5>(Ty, dst, src);
}

void AssemblerX8632::sub(Type Ty, GPRRegister reg, const Address &address) {
  arith_int<5>(Ty, reg, address);
}

void AssemblerX8632::sub(Type Ty, GPRRegister reg, const Immediate &imm) {
  arith_int<5>(Ty, reg, imm);
}

void AssemblerX8632::sub(Type Ty, const Address &address, GPRRegister reg) {
  arith_int<5>(Ty, address, reg);
}

void AssemblerX8632::sub(Type Ty, const Address &address,
                         const Immediate &imm) {
  arith_int<5>(Ty, address, imm);
}

void AssemblerX8632::sbb(Type Ty, GPRRegister dst, GPRRegister src) {
  arith_int<3>(Ty, dst, src);
}

void AssemblerX8632::sbb(Type Ty, GPRRegister dst, const Address &address) {
  arith_int<3>(Ty, dst, address);
}

void AssemblerX8632::sbb(Type Ty, GPRRegister reg, const Immediate &imm) {
  arith_int<3>(Ty, reg, imm);
}

void AssemblerX8632::sbb(Type Ty, const Address &address, GPRRegister reg) {
  arith_int<3>(Ty, address, reg);
}

void AssemblerX8632::sbb(Type Ty, const Address &address,
                         const Immediate &imm) {
  arith_int<3>(Ty, address, imm);
}

void AssemblerX8632::cbw() {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitOperandSizeOverride();
  emitUint8(0x98);
}

void AssemblerX8632::cwd() {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitOperandSizeOverride();
  emitUint8(0x99);
}

void AssemblerX8632::cdq() {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0x99);
}

void AssemblerX8632::div(Type Ty, GPRRegister reg) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  if (Ty == IceType_i16)
    emitOperandSizeOverride();
  if (isByteSizedArithType(Ty))
    emitUint8(0xF6);
  else
    emitUint8(0xF7);
  emitRegisterOperand(6, reg);
}

void AssemblerX8632::div(Type Ty, const Address &addr) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  if (Ty == IceType_i16)
    emitOperandSizeOverride();
  if (isByteSizedArithType(Ty))
    emitUint8(0xF6);
  else
    emitUint8(0xF7);
  emitOperand(6, addr);
}

void AssemblerX8632::idiv(Type Ty, GPRRegister reg) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  if (Ty == IceType_i16)
    emitOperandSizeOverride();
  if (isByteSizedArithType(Ty))
    emitUint8(0xF6);
  else
    emitUint8(0xF7);
  emitRegisterOperand(7, reg);
}

void AssemblerX8632::idiv(Type Ty, const Address &addr) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  if (Ty == IceType_i16)
    emitOperandSizeOverride();
  if (isByteSizedArithType(Ty))
    emitUint8(0xF6);
  else
    emitUint8(0xF7);
  emitOperand(7, addr);
}

void AssemblerX8632::imul(Type Ty, GPRRegister dst, GPRRegister src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  assert(Ty == IceType_i16 || Ty == IceType_i32);
  if (Ty == IceType_i16)
    emitOperandSizeOverride();
  emitUint8(0x0F);
  emitUint8(0xAF);
  emitRegisterOperand(dst, src);
}

void AssemblerX8632::imul(Type Ty, GPRRegister reg, const Address &address) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  assert(Ty == IceType_i16 || Ty == IceType_i32);
  if (Ty == IceType_i16)
    emitOperandSizeOverride();
  emitUint8(0x0F);
  emitUint8(0xAF);
  emitOperand(reg, address);
}

void AssemblerX8632::imul(Type Ty, GPRRegister reg, const Immediate &imm) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  assert(Ty == IceType_i16 || Ty == IceType_i32);
  if (Ty == IceType_i16)
    emitOperandSizeOverride();
  if (imm.is_int8()) {
    emitUint8(0x6B);
    emitRegisterOperand(reg, reg);
    emitUint8(imm.value() & 0xFF);
  } else {
    emitUint8(0x69);
    emitRegisterOperand(reg, reg);
    emitImmediate(Ty, imm);
  }
}

void AssemblerX8632::imul(Type Ty, GPRRegister reg) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  if (Ty == IceType_i16)
    emitOperandSizeOverride();
  if (isByteSizedArithType(Ty))
    emitUint8(0xF6);
  else
    emitUint8(0xF7);
  emitRegisterOperand(5, reg);
}

void AssemblerX8632::imul(Type Ty, const Address &address) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  if (Ty == IceType_i16)
    emitOperandSizeOverride();
  if (isByteSizedArithType(Ty))
    emitUint8(0xF6);
  else
    emitUint8(0xF7);
  emitOperand(5, address);
}

void AssemblerX8632::mul(Type Ty, GPRRegister reg) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  if (Ty == IceType_i16)
    emitOperandSizeOverride();
  if (isByteSizedArithType(Ty))
    emitUint8(0xF6);
  else
    emitUint8(0xF7);
  emitRegisterOperand(4, reg);
}

void AssemblerX8632::mul(Type Ty, const Address &address) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  if (Ty == IceType_i16)
    emitOperandSizeOverride();
  if (isByteSizedArithType(Ty))
    emitUint8(0xF6);
  else
    emitUint8(0xF7);
  emitOperand(4, address);
}

void AssemblerX8632::incl(GPRRegister reg) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0x40 + reg);
}

void AssemblerX8632::incl(const Address &address) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0xFF);
  emitOperand(0, address);
}

void AssemblerX8632::decl(GPRRegister reg) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0x48 + reg);
}

void AssemblerX8632::decl(const Address &address) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0xFF);
  emitOperand(1, address);
}

void AssemblerX8632::rol(Type Ty, GPRRegister reg, const Immediate &imm) {
  emitGenericShift(0, Ty, reg, imm);
}

void AssemblerX8632::rol(Type Ty, GPRRegister operand, GPRRegister shifter) {
  emitGenericShift(0, Ty, Operand(operand), shifter);
}

void AssemblerX8632::rol(Type Ty, const Address &operand, GPRRegister shifter) {
  emitGenericShift(0, Ty, operand, shifter);
}

void AssemblerX8632::shl(Type Ty, GPRRegister reg, const Immediate &imm) {
  emitGenericShift(4, Ty, reg, imm);
}

void AssemblerX8632::shl(Type Ty, GPRRegister operand, GPRRegister shifter) {
  emitGenericShift(4, Ty, Operand(operand), shifter);
}

void AssemblerX8632::shl(Type Ty, const Address &operand, GPRRegister shifter) {
  emitGenericShift(4, Ty, operand, shifter);
}

void AssemblerX8632::shr(Type Ty, GPRRegister reg, const Immediate &imm) {
  emitGenericShift(5, Ty, reg, imm);
}

void AssemblerX8632::shr(Type Ty, GPRRegister operand, GPRRegister shifter) {
  emitGenericShift(5, Ty, Operand(operand), shifter);
}

void AssemblerX8632::shr(Type Ty, const Address &operand, GPRRegister shifter) {
  emitGenericShift(5, Ty, operand, shifter);
}

void AssemblerX8632::sar(Type Ty, GPRRegister reg, const Immediate &imm) {
  emitGenericShift(7, Ty, reg, imm);
}

void AssemblerX8632::sar(Type Ty, GPRRegister operand, GPRRegister shifter) {
  emitGenericShift(7, Ty, Operand(operand), shifter);
}

void AssemblerX8632::sar(Type Ty, const Address &address, GPRRegister shifter) {
  emitGenericShift(7, Ty, address, shifter);
}

void AssemblerX8632::shld(Type Ty, GPRRegister dst, GPRRegister src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  assert(Ty == IceType_i16 || Ty == IceType_i32);
  if (Ty == IceType_i16)
    emitOperandSizeOverride();
  emitUint8(0x0F);
  emitUint8(0xA5);
  emitRegisterOperand(src, dst);
}

void AssemblerX8632::shld(Type Ty, GPRRegister dst, GPRRegister src,
                          const Immediate &imm) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  assert(Ty == IceType_i16 || Ty == IceType_i32);
  assert(imm.is_int8());
  if (Ty == IceType_i16)
    emitOperandSizeOverride();
  emitUint8(0x0F);
  emitUint8(0xA4);
  emitRegisterOperand(src, dst);
  emitUint8(imm.value() & 0xFF);
}

void AssemblerX8632::shld(Type Ty, const Address &operand, GPRRegister src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  assert(Ty == IceType_i16 || Ty == IceType_i32);
  if (Ty == IceType_i16)
    emitOperandSizeOverride();
  emitUint8(0x0F);
  emitUint8(0xA5);
  emitOperand(src, operand);
}

void AssemblerX8632::shrd(Type Ty, GPRRegister dst, GPRRegister src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  assert(Ty == IceType_i16 || Ty == IceType_i32);
  if (Ty == IceType_i16)
    emitOperandSizeOverride();
  emitUint8(0x0F);
  emitUint8(0xAD);
  emitRegisterOperand(src, dst);
}

void AssemblerX8632::shrd(Type Ty, GPRRegister dst, GPRRegister src,
                          const Immediate &imm) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  assert(Ty == IceType_i16 || Ty == IceType_i32);
  assert(imm.is_int8());
  if (Ty == IceType_i16)
    emitOperandSizeOverride();
  emitUint8(0x0F);
  emitUint8(0xAC);
  emitRegisterOperand(src, dst);
  emitUint8(imm.value() & 0xFF);
}

void AssemblerX8632::shrd(Type Ty, const Address &dst, GPRRegister src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  assert(Ty == IceType_i16 || Ty == IceType_i32);
  if (Ty == IceType_i16)
    emitOperandSizeOverride();
  emitUint8(0x0F);
  emitUint8(0xAD);
  emitOperand(src, dst);
}

void AssemblerX8632::neg(Type Ty, GPRRegister reg) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  if (Ty == IceType_i16)
    emitOperandSizeOverride();
  if (isByteSizedArithType(Ty))
    emitUint8(0xF6);
  else
    emitUint8(0xF7);
  emitRegisterOperand(3, reg);
}

void AssemblerX8632::neg(Type Ty, const Address &addr) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  if (Ty == IceType_i16)
    emitOperandSizeOverride();
  if (isByteSizedArithType(Ty))
    emitUint8(0xF6);
  else
    emitUint8(0xF7);
  emitOperand(3, addr);
}

void AssemblerX8632::notl(GPRRegister reg) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0xF7);
  emitUint8(0xD0 | reg);
}

void AssemblerX8632::bswap(Type Ty, GPRRegister reg) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  assert(Ty == IceType_i32);
  (void)Ty;
  emitUint8(0x0F);
  emitUint8(0xC8 | reg);
}

void AssemblerX8632::bsf(Type Ty, GPRRegister dst, GPRRegister src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  assert(Ty == IceType_i16 || Ty == IceType_i32);
  if (Ty == IceType_i16)
    emitOperandSizeOverride();
  emitUint8(0x0F);
  emitUint8(0xBC);
  emitRegisterOperand(dst, src);
}

void AssemblerX8632::bsf(Type Ty, GPRRegister dst, const Address &src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  assert(Ty == IceType_i16 || Ty == IceType_i32);
  if (Ty == IceType_i16)
    emitOperandSizeOverride();
  emitUint8(0x0F);
  emitUint8(0xBC);
  emitOperand(dst, src);
}

void AssemblerX8632::bsr(Type Ty, GPRRegister dst, GPRRegister src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  assert(Ty == IceType_i16 || Ty == IceType_i32);
  if (Ty == IceType_i16)
    emitOperandSizeOverride();
  emitUint8(0x0F);
  emitUint8(0xBD);
  emitRegisterOperand(dst, src);
}

void AssemblerX8632::bsr(Type Ty, GPRRegister dst, const Address &src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  assert(Ty == IceType_i16 || Ty == IceType_i32);
  if (Ty == IceType_i16)
    emitOperandSizeOverride();
  emitUint8(0x0F);
  emitUint8(0xBD);
  emitOperand(dst, src);
}

void AssemblerX8632::bt(GPRRegister base, GPRRegister offset) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0x0F);
  emitUint8(0xA3);
  emitRegisterOperand(offset, base);
}

void AssemblerX8632::ret() {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0xC3);
}

void AssemblerX8632::ret(const Immediate &imm) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0xC2);
  assert(imm.is_uint16());
  emitUint8(imm.value() & 0xFF);
  emitUint8((imm.value() >> 8) & 0xFF);
}

void AssemblerX8632::nop(int size) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  // There are nops up to size 15, but for now just provide up to size 8.
  assert(0 < size && size <= MAX_NOP_SIZE);
  switch (size) {
  case 1:
    emitUint8(0x90);
    break;
  case 2:
    emitUint8(0x66);
    emitUint8(0x90);
    break;
  case 3:
    emitUint8(0x0F);
    emitUint8(0x1F);
    emitUint8(0x00);
    break;
  case 4:
    emitUint8(0x0F);
    emitUint8(0x1F);
    emitUint8(0x40);
    emitUint8(0x00);
    break;
  case 5:
    emitUint8(0x0F);
    emitUint8(0x1F);
    emitUint8(0x44);
    emitUint8(0x00);
    emitUint8(0x00);
    break;
  case 6:
    emitUint8(0x66);
    emitUint8(0x0F);
    emitUint8(0x1F);
    emitUint8(0x44);
    emitUint8(0x00);
    emitUint8(0x00);
    break;
  case 7:
    emitUint8(0x0F);
    emitUint8(0x1F);
    emitUint8(0x80);
    emitUint8(0x00);
    emitUint8(0x00);
    emitUint8(0x00);
    emitUint8(0x00);
    break;
  case 8:
    emitUint8(0x0F);
    emitUint8(0x1F);
    emitUint8(0x84);
    emitUint8(0x00);
    emitUint8(0x00);
    emitUint8(0x00);
    emitUint8(0x00);
    emitUint8(0x00);
    break;
  default:
    llvm_unreachable("Unimplemented");
  }
}

void AssemblerX8632::int3() {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0xCC);
}

void AssemblerX8632::hlt() {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0xF4);
}

void AssemblerX8632::ud2() {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0x0F);
  emitUint8(0x0B);
}

void AssemblerX8632::j(CondX86::BrCond condition, Label *label, bool near) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  if (label->IsBound()) {
    static const int kShortSize = 2;
    static const int kLongSize = 6;
    intptr_t offset = label->Position() - Buffer.size();
    assert(offset <= 0);
    if (Utils::IsInt(8, offset - kShortSize)) {
      // TODO(stichnot): Here and in jmp(), we may need to be more
      // conservative about the backward branch distance if the branch
      // instruction is within a bundle_lock sequence, because the
      // distance may increase when padding is added.  This isn't an
      // issue for branches outside a bundle_lock, because if padding
      // is added, the retry may change it to a long backward branch
      // without affecting any of the bookkeeping.
      emitUint8(0x70 + condition);
      emitUint8((offset - kShortSize) & 0xFF);
    } else {
      emitUint8(0x0F);
      emitUint8(0x80 + condition);
      emitInt32(offset - kLongSize);
    }
  } else if (near) {
    emitUint8(0x70 + condition);
    emitNearLabelLink(label);
  } else {
    emitUint8(0x0F);
    emitUint8(0x80 + condition);
    emitLabelLink(label);
  }
}

void AssemblerX8632::j(CondX86::BrCond condition,
                       const ConstantRelocatable *label) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0x0F);
  emitUint8(0x80 + condition);
  emitFixup(this->createFixup(llvm::ELF::R_386_PC32, label));
  emitInt32(-4);
}

void AssemblerX8632::jmp(GPRRegister reg) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0xFF);
  emitRegisterOperand(4, reg);
}

void AssemblerX8632::jmp(Label *label, bool near) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  if (label->IsBound()) {
    static const int kShortSize = 2;
    static const int kLongSize = 5;
    intptr_t offset = label->Position() - Buffer.size();
    assert(offset <= 0);
    if (Utils::IsInt(8, offset - kShortSize)) {
      emitUint8(0xEB);
      emitUint8((offset - kShortSize) & 0xFF);
    } else {
      emitUint8(0xE9);
      emitInt32(offset - kLongSize);
    }
  } else if (near) {
    emitUint8(0xEB);
    emitNearLabelLink(label);
  } else {
    emitUint8(0xE9);
    emitLabelLink(label);
  }
}

void AssemblerX8632::jmp(const ConstantRelocatable *label) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0xE9);
  emitFixup(this->createFixup(llvm::ELF::R_386_PC32, label));
  emitInt32(-4);
}

void AssemblerX8632::mfence() {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0x0F);
  emitUint8(0xAE);
  emitUint8(0xF0);
}

void AssemblerX8632::lock() {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0xF0);
}

void AssemblerX8632::cmpxchg(Type Ty, const Address &address, GPRRegister reg,
                             bool Locked) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  if (Ty == IceType_i16)
    emitOperandSizeOverride();
  if (Locked)
    emitUint8(0xF0);
  emitUint8(0x0F);
  if (isByteSizedArithType(Ty))
    emitUint8(0xB0);
  else
    emitUint8(0xB1);
  emitOperand(reg, address);
}

void AssemblerX8632::cmpxchg8b(const Address &address, bool Locked) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  if (Locked)
    emitUint8(0xF0);
  emitUint8(0x0F);
  emitUint8(0xC7);
  emitOperand(1, address);
}

void AssemblerX8632::xadd(Type Ty, const Address &addr, GPRRegister reg,
                          bool Locked) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  if (Ty == IceType_i16)
    emitOperandSizeOverride();
  if (Locked)
    emitUint8(0xF0);
  emitUint8(0x0F);
  if (isByteSizedArithType(Ty))
    emitUint8(0xC0);
  else
    emitUint8(0xC1);
  emitOperand(reg, addr);
}

void AssemblerX8632::xchg(Type Ty, const Address &addr, GPRRegister reg) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  if (Ty == IceType_i16)
    emitOperandSizeOverride();
  if (isByteSizedArithType(Ty))
    emitUint8(0x86);
  else
    emitUint8(0x87);
  emitOperand(reg, addr);
}

void AssemblerX8632::emitSegmentOverride(uint8_t prefix) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(prefix);
}

void AssemblerX8632::align(intptr_t alignment, intptr_t offset) {
  assert(llvm::isPowerOf2_32(alignment));
  intptr_t pos = offset + Buffer.getPosition();
  intptr_t mod = pos & (alignment - 1);
  if (mod == 0) {
    return;
  }
  intptr_t bytes_needed = alignment - mod;
  while (bytes_needed > MAX_NOP_SIZE) {
    nop(MAX_NOP_SIZE);
    bytes_needed -= MAX_NOP_SIZE;
  }
  if (bytes_needed) {
    nop(bytes_needed);
  }
  assert(((offset + Buffer.getPosition()) & (alignment - 1)) == 0);
}

void AssemblerX8632::bind(Label *label) {
  intptr_t bound = Buffer.size();
  assert(!label->IsBound()); // Labels can only be bound once.
  while (label->IsLinked()) {
    intptr_t position = label->LinkPosition();
    intptr_t next = Buffer.load<int32_t>(position);
    Buffer.store<int32_t>(position, bound - (position + 4));
    label->position_ = next;
  }
  while (label->HasNear()) {
    intptr_t position = label->NearPosition();
    intptr_t offset = bound - (position + 1);
    assert(Utils::IsInt(8, offset));
    Buffer.store<int8_t>(position, offset);
  }
  label->BindTo(bound);
}

void AssemblerX8632::emitOperand(int rm, const Operand &operand) {
  assert(rm >= 0 && rm < 8);
  const intptr_t length = operand.length_;
  assert(length > 0);
  // Emit the ModRM byte updated with the given RM value.
  assert((operand.encoding_[0] & 0x38) == 0);
  emitUint8(operand.encoding_[0] + (rm << 3));
  if (operand.fixup()) {
    emitFixup(operand.fixup());
  }
  // Emit the rest of the encoded operand.
  for (intptr_t i = 1; i < length; i++) {
    emitUint8(operand.encoding_[i]);
  }
}

void AssemblerX8632::emitImmediate(Type Ty, const Immediate &imm) {
  if (Ty == IceType_i16) {
    assert(!imm.fixup());
    emitInt16(imm.value());
  } else {
    if (imm.fixup()) {
      emitFixup(imm.fixup());
    }
    emitInt32(imm.value());
  }
}

void AssemblerX8632::emitComplexI8(int rm, const Operand &operand,
                                   const Immediate &immediate) {
  assert(rm >= 0 && rm < 8);
  assert(immediate.is_int8());
  if (operand.IsRegister(RegX8632::Encoded_Reg_eax)) {
    // Use short form if the destination is al.
    emitUint8(0x04 + (rm << 3));
    emitUint8(immediate.value() & 0xFF);
  } else {
    // Use sign-extended 8-bit immediate.
    emitUint8(0x80);
    emitOperand(rm, operand);
    emitUint8(immediate.value() & 0xFF);
  }
}

void AssemblerX8632::emitComplex(Type Ty, int rm, const Operand &operand,
                                 const Immediate &immediate) {
  assert(rm >= 0 && rm < 8);
  if (immediate.is_int8()) {
    // Use sign-extended 8-bit immediate.
    emitUint8(0x83);
    emitOperand(rm, operand);
    emitUint8(immediate.value() & 0xFF);
  } else if (operand.IsRegister(RegX8632::Encoded_Reg_eax)) {
    // Use short form if the destination is eax.
    emitUint8(0x05 + (rm << 3));
    emitImmediate(Ty, immediate);
  } else {
    emitUint8(0x81);
    emitOperand(rm, operand);
    emitImmediate(Ty, immediate);
  }
}

void AssemblerX8632::emitLabel(Label *label, intptr_t instruction_size) {
  if (label->IsBound()) {
    intptr_t offset = label->Position() - Buffer.size();
    assert(offset <= 0);
    emitInt32(offset - instruction_size);
  } else {
    emitLabelLink(label);
  }
}

void AssemblerX8632::emitLabelLink(Label *Label) {
  assert(!Label->IsBound());
  intptr_t Position = Buffer.size();
  emitInt32(Label->position_);
  if (!getPreliminary())
    Label->LinkTo(Position);
}

void AssemblerX8632::emitNearLabelLink(Label *label) {
  assert(!label->IsBound());
  intptr_t position = Buffer.size();
  emitUint8(0);
  if (!getPreliminary())
    label->NearLinkTo(position);
}

void AssemblerX8632::emitGenericShift(int rm, Type Ty, GPRRegister reg,
                                      const Immediate &imm) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  assert(imm.is_int8());
  if (Ty == IceType_i16)
    emitOperandSizeOverride();
  if (imm.value() == 1) {
    emitUint8(isByteSizedArithType(Ty) ? 0xD0 : 0xD1);
    emitOperand(rm, Operand(reg));
  } else {
    emitUint8(isByteSizedArithType(Ty) ? 0xC0 : 0xC1);
    emitOperand(rm, Operand(reg));
    emitUint8(imm.value() & 0xFF);
  }
}

void AssemblerX8632::emitGenericShift(int rm, Type Ty, const Operand &operand,
                                      GPRRegister shifter) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  assert(shifter == RegX8632::Encoded_Reg_ecx);
  (void)shifter;
  if (Ty == IceType_i16)
    emitOperandSizeOverride();
  emitUint8(isByteSizedArithType(Ty) ? 0xD2 : 0xD3);
  emitOperand(rm, operand);
}

} // end of namespace X8632
} // end of namespace Ice
