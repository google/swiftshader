//===- subzero/src/IceAssemblerX86BaseImpl.h - base x86 assembler -*- C++ -*-=//
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
// This file implements the AssemblerX86Base template class, which is the base
// Assembler class for X86 assemblers.
//
//===----------------------------------------------------------------------===//

#include "IceAssemblerX86Base.h"

#include "IceCfg.h"
#include "IceOperand.h"

namespace Ice {
namespace X86Internal {

template <class Machine>
AssemblerX86Base<Machine>::~AssemblerX86Base<Machine>() {
  if (BuildDefs::asserts()) {
    for (const Label *Label : CfgNodeLabels) {
      Label->finalCheck();
    }
    for (const Label *Label : LocalLabels) {
      Label->finalCheck();
    }
  }
}

template <class Machine> void AssemblerX86Base<Machine>::alignFunction() {
  const SizeT Align = 1 << getBundleAlignLog2Bytes();
  SizeT BytesNeeded = Utils::OffsetToAlignment(Buffer.getPosition(), Align);
  constexpr SizeT HltSize = 1;
  while (BytesNeeded > 0) {
    hlt();
    BytesNeeded -= HltSize;
  }
}

template <class Machine>
Label *AssemblerX86Base<Machine>::getOrCreateLabel(SizeT Number,
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

template <class Machine>
Label *AssemblerX86Base<Machine>::getOrCreateCfgNodeLabel(SizeT NodeNumber) {
  return getOrCreateLabel(NodeNumber, CfgNodeLabels);
}

template <class Machine>
Label *AssemblerX86Base<Machine>::getOrCreateLocalLabel(SizeT Number) {
  return getOrCreateLabel(Number, LocalLabels);
}

template <class Machine>
void AssemblerX86Base<Machine>::bindCfgNodeLabel(SizeT NodeNumber) {
  assert(!getPreliminary());
  Label *L = getOrCreateCfgNodeLabel(NodeNumber);
  this->bind(L);
}

template <class Machine>
void AssemblerX86Base<Machine>::bindLocalLabel(SizeT Number) {
  Label *L = getOrCreateLocalLabel(Number);
  if (!getPreliminary())
    this->bind(L);
}

template <class Machine>
void AssemblerX86Base<Machine>::call(typename Traits::GPRRegister reg) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitRexB(RexTypeIrrelevant, reg);
  emitUint8(0xFF);
  emitRegisterOperand(2, gprEncoding(reg));
}

template <class Machine>
void AssemblerX86Base<Machine>::call(const typename Traits::Address &address) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitRex(RexTypeIrrelevant, address, RexRegIrrelevant);
  emitUint8(0xFF);
  emitOperand(2, address);
}

template <class Machine>
void AssemblerX86Base<Machine>::call(const ConstantRelocatable *label) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  intptr_t call_start = Buffer.getPosition();
  emitUint8(0xE8);
  emitFixup(this->createFixup(Traits::PcRelFixup, label));
  emitInt32(-4);
  assert((Buffer.getPosition() - call_start) == kCallExternalLabelSize);
  (void)call_start;
}

template <class Machine>
void AssemblerX86Base<Machine>::call(const Immediate &abs_address) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  intptr_t call_start = Buffer.getPosition();
  emitUint8(0xE8);
  emitFixup(this->createFixup(Traits::PcRelFixup, AssemblerFixup::NullSymbol));
  emitInt32(abs_address.value() - 4);
  assert((Buffer.getPosition() - call_start) == kCallExternalLabelSize);
  (void)call_start;
}

template <class Machine>
void AssemblerX86Base<Machine>::pushl(typename Traits::GPRRegister reg) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitRexB(RexTypeIrrelevant, reg);
  emitUint8(0x50 + gprEncoding(reg));
}

template <class Machine>
void AssemblerX86Base<Machine>::popl(typename Traits::GPRRegister reg) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  // Any type that would not force a REX prefix to be emitted can be provided
  // here.
  emitRexB(RexTypeIrrelevant, reg);
  emitUint8(0x58 + gprEncoding(reg));
}

template <class Machine>
void AssemblerX86Base<Machine>::popl(const typename Traits::Address &address) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitRex(RexTypeIrrelevant, address, RexRegIrrelevant);
  emitUint8(0x8F);
  emitOperand(0, address);
}

template <class Machine>
template <typename, typename>
void AssemblerX86Base<Machine>::pushal() {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0x60);
}

template <class Machine>
template <typename, typename>
void AssemblerX86Base<Machine>::popal() {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0x61);
}

template <class Machine>
void AssemblerX86Base<Machine>::setcc(typename Traits::Cond::BrCond condition,
                                      typename Traits::ByteRegister dst) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitRexB(IceType_i8, dst);
  emitUint8(0x0F);
  emitUint8(0x90 + condition);
  emitUint8(0xC0 + gprEncoding(dst));
}

template <class Machine>
void AssemblerX86Base<Machine>::setcc(typename Traits::Cond::BrCond condition,
                                      const typename Traits::Address &address) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitRex(RexTypeIrrelevant, address, RexRegIrrelevant);
  emitUint8(0x0F);
  emitUint8(0x90 + condition);
  emitOperand(0, address);
}

template <class Machine>
void AssemblerX86Base<Machine>::mov(Type Ty, typename Traits::GPRRegister dst,
                                    const Immediate &imm) {
  assert(Ty != IceType_i64 && "i64 not supported yet.");
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  if (Ty == IceType_i16)
    emitOperandSizeOverride();
  emitRexB(Ty, dst);
  if (isByteSizedType(Ty)) {
    emitUint8(0xB0 + gprEncoding(dst));
    emitUint8(imm.value() & 0xFF);
  } else {
    emitUint8(0xB8 + gprEncoding(dst));
    emitImmediate(Ty, imm);
  }
}

template <class Machine>
void AssemblerX86Base<Machine>::mov(Type Ty, typename Traits::GPRRegister dst,
                                    typename Traits::GPRRegister src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  if (Ty == IceType_i16)
    emitOperandSizeOverride();
  emitRexRB(Ty, src, dst);
  if (isByteSizedType(Ty)) {
    emitUint8(0x88);
  } else {
    emitUint8(0x89);
  }
  emitRegisterOperand(gprEncoding(src), gprEncoding(dst));
}

template <class Machine>
void AssemblerX86Base<Machine>::mov(Type Ty, typename Traits::GPRRegister dst,
                                    const typename Traits::Address &src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  if (Ty == IceType_i16)
    emitOperandSizeOverride();
  emitRex(Ty, src, dst);
  if (isByteSizedType(Ty)) {
    emitUint8(0x8A);
  } else {
    emitUint8(0x8B);
  }
  emitOperand(gprEncoding(dst), src);
}

template <class Machine>
void AssemblerX86Base<Machine>::mov(Type Ty,
                                    const typename Traits::Address &dst,
                                    typename Traits::GPRRegister src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  if (Ty == IceType_i16)
    emitOperandSizeOverride();
  emitRex(Ty, dst, src);
  if (isByteSizedType(Ty)) {
    emitUint8(0x88);
  } else {
    emitUint8(0x89);
  }
  emitOperand(gprEncoding(src), dst);
}

template <class Machine>
void AssemblerX86Base<Machine>::mov(Type Ty,
                                    const typename Traits::Address &dst,
                                    const Immediate &imm) {
  assert(Ty != IceType_i64 && "i64 not supported yet.");
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  if (Ty == IceType_i16)
    emitOperandSizeOverride();
  emitRex(Ty, dst, RexRegIrrelevant);
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

template <class Machine>
void AssemblerX86Base<Machine>::movzx(Type SrcTy,
                                      typename Traits::GPRRegister dst,
                                      typename Traits::GPRRegister src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  bool ByteSized = isByteSizedType(SrcTy);
  assert(ByteSized || SrcTy == IceType_i16);
  emitRexRB(RexTypeIrrelevant, dst, SrcTy, src);
  emitUint8(0x0F);
  emitUint8(ByteSized ? 0xB6 : 0xB7);
  emitRegisterOperand(gprEncoding(dst), gprEncoding(src));
}

template <class Machine>
void AssemblerX86Base<Machine>::movzx(Type SrcTy,
                                      typename Traits::GPRRegister dst,
                                      const typename Traits::Address &src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  bool ByteSized = isByteSizedType(SrcTy);
  assert(ByteSized || SrcTy == IceType_i16);
  emitRex(SrcTy, src, RexTypeIrrelevant, dst);
  emitUint8(0x0F);
  emitUint8(ByteSized ? 0xB6 : 0xB7);
  emitOperand(gprEncoding(dst), src);
}

template <class Machine>
void AssemblerX86Base<Machine>::movsx(Type SrcTy,
                                      typename Traits::GPRRegister dst,
                                      typename Traits::GPRRegister src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  bool ByteSized = isByteSizedType(SrcTy);
  emitRexRB(RexTypeForceRexW, dst, SrcTy, src);
  if (ByteSized || SrcTy == IceType_i16) {
    emitUint8(0x0F);
    emitUint8(ByteSized ? 0xBE : 0xBF);
  } else {
    assert(Traits::Is64Bit && SrcTy == IceType_i32);
    emitUint8(0x63);
  }
  emitRegisterOperand(gprEncoding(dst), gprEncoding(src));
}

template <class Machine>
void AssemblerX86Base<Machine>::movsx(Type SrcTy,
                                      typename Traits::GPRRegister dst,
                                      const typename Traits::Address &src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  bool ByteSized = isByteSizedType(SrcTy);
  emitRex(SrcTy, src, RexTypeForceRexW, dst);
  if (ByteSized || SrcTy == IceType_i16) {
    emitUint8(0x0F);
    emitUint8(ByteSized ? 0xBE : 0xBF);
  } else {
    assert(Traits::Is64Bit && SrcTy == IceType_i32);
    emitUint8(0x63);
  }
  emitOperand(gprEncoding(dst), src);
}

template <class Machine>
void AssemblerX86Base<Machine>::lea(Type Ty, typename Traits::GPRRegister dst,
                                    const typename Traits::Address &src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  assert(Ty == IceType_i16 || Ty == IceType_i32);
  if (Ty == IceType_i16)
    emitOperandSizeOverride();
  emitRex(Ty, src, dst);
  emitUint8(0x8D);
  emitOperand(gprEncoding(dst), src);
}

template <class Machine>
void AssemblerX86Base<Machine>::cmov(Type Ty,
                                     typename Traits::Cond::BrCond cond,
                                     typename Traits::GPRRegister dst,
                                     typename Traits::GPRRegister src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  if (Ty == IceType_i16)
    emitOperandSizeOverride();
  else
    assert(Ty == IceType_i32);
  emitRexRB(Ty, dst, src);
  emitUint8(0x0F);
  emitUint8(0x40 + cond);
  emitRegisterOperand(gprEncoding(dst), gprEncoding(src));
}

template <class Machine>
void AssemblerX86Base<Machine>::cmov(Type Ty,
                                     typename Traits::Cond::BrCond cond,
                                     typename Traits::GPRRegister dst,
                                     const typename Traits::Address &src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  if (Ty == IceType_i16)
    emitOperandSizeOverride();
  else
    assert(Ty == IceType_i32);
  emitRex(Ty, src, dst);
  emitUint8(0x0F);
  emitUint8(0x40 + cond);
  emitOperand(gprEncoding(dst), src);
}

template <class Machine> void AssemblerX86Base<Machine>::rep_movsb() {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0xF3);
  emitUint8(0xA4);
}

template <class Machine>
void AssemblerX86Base<Machine>::movss(Type Ty, typename Traits::XmmRegister dst,
                                      const typename Traits::Address &src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(isFloat32Asserting32Or64(Ty) ? 0xF3 : 0xF2);
  emitRex(RexTypeIrrelevant, src, dst);
  emitUint8(0x0F);
  emitUint8(0x10);
  emitOperand(gprEncoding(dst), src);
}

template <class Machine>
void AssemblerX86Base<Machine>::movss(Type Ty,
                                      const typename Traits::Address &dst,
                                      typename Traits::XmmRegister src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(isFloat32Asserting32Or64(Ty) ? 0xF3 : 0xF2);
  emitRex(RexTypeIrrelevant, dst, src);
  emitUint8(0x0F);
  emitUint8(0x11);
  emitOperand(gprEncoding(src), dst);
}

template <class Machine>
void AssemblerX86Base<Machine>::movss(Type Ty, typename Traits::XmmRegister dst,
                                      typename Traits::XmmRegister src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(isFloat32Asserting32Or64(Ty) ? 0xF3 : 0xF2);
  emitRexRB(RexTypeIrrelevant, src, dst);
  emitUint8(0x0F);
  emitUint8(0x11);
  emitXmmRegisterOperand(src, dst);
}

template <class Machine>
void AssemblerX86Base<Machine>::movd(typename Traits::XmmRegister dst,
                                     typename Traits::GPRRegister src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0x66);
  emitRexRB(RexTypeIrrelevant, dst, src);
  emitUint8(0x0F);
  emitUint8(0x6E);
  emitRegisterOperand(gprEncoding(dst), gprEncoding(src));
}

template <class Machine>
void AssemblerX86Base<Machine>::movd(typename Traits::XmmRegister dst,
                                     const typename Traits::Address &src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0x66);
  emitRex(RexTypeIrrelevant, src, dst);
  emitUint8(0x0F);
  emitUint8(0x6E);
  emitOperand(gprEncoding(dst), src);
}

template <class Machine>
void AssemblerX86Base<Machine>::movd(typename Traits::GPRRegister dst,
                                     typename Traits::XmmRegister src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0x66);
  emitRexRB(RexTypeIrrelevant, src, dst);
  emitUint8(0x0F);
  emitUint8(0x7E);
  emitRegisterOperand(gprEncoding(src), gprEncoding(dst));
}

template <class Machine>
void AssemblerX86Base<Machine>::movd(const typename Traits::Address &dst,
                                     typename Traits::XmmRegister src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0x66);
  emitRex(RexTypeIrrelevant, dst, src);
  emitUint8(0x0F);
  emitUint8(0x7E);
  emitOperand(gprEncoding(src), dst);
}

template <class Machine>
void AssemblerX86Base<Machine>::movq(typename Traits::XmmRegister dst,
                                     typename Traits::XmmRegister src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0xF3);
  emitRexRB(RexTypeIrrelevant, dst, src);
  emitUint8(0x0F);
  emitUint8(0x7E);
  emitXmmRegisterOperand(dst, src);
}

template <class Machine>
void AssemblerX86Base<Machine>::movq(const typename Traits::Address &dst,
                                     typename Traits::XmmRegister src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0x66);
  emitRex(RexTypeIrrelevant, dst, src);
  emitUint8(0x0F);
  emitUint8(0xD6);
  emitOperand(gprEncoding(src), dst);
}

template <class Machine>
void AssemblerX86Base<Machine>::movq(typename Traits::XmmRegister dst,
                                     const typename Traits::Address &src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0xF3);
  emitRex(RexTypeIrrelevant, src, dst);
  emitUint8(0x0F);
  emitUint8(0x7E);
  emitOperand(gprEncoding(dst), src);
}

template <class Machine>
void AssemblerX86Base<Machine>::addss(Type Ty, typename Traits::XmmRegister dst,
                                      typename Traits::XmmRegister src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(isFloat32Asserting32Or64(Ty) ? 0xF3 : 0xF2);
  emitRexRB(RexTypeIrrelevant, dst, src);
  emitUint8(0x0F);
  emitUint8(0x58);
  emitXmmRegisterOperand(dst, src);
}

template <class Machine>
void AssemblerX86Base<Machine>::addss(Type Ty, typename Traits::XmmRegister dst,
                                      const typename Traits::Address &src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(isFloat32Asserting32Or64(Ty) ? 0xF3 : 0xF2);
  emitRex(RexTypeIrrelevant, src, dst);
  emitUint8(0x0F);
  emitUint8(0x58);
  emitOperand(gprEncoding(dst), src);
}

template <class Machine>
void AssemblerX86Base<Machine>::subss(Type Ty, typename Traits::XmmRegister dst,
                                      typename Traits::XmmRegister src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(isFloat32Asserting32Or64(Ty) ? 0xF3 : 0xF2);
  emitRexRB(RexTypeIrrelevant, dst, src);
  emitUint8(0x0F);
  emitUint8(0x5C);
  emitXmmRegisterOperand(dst, src);
}

template <class Machine>
void AssemblerX86Base<Machine>::subss(Type Ty, typename Traits::XmmRegister dst,
                                      const typename Traits::Address &src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(isFloat32Asserting32Or64(Ty) ? 0xF3 : 0xF2);
  emitRex(RexTypeIrrelevant, src, dst);
  emitUint8(0x0F);
  emitUint8(0x5C);
  emitOperand(gprEncoding(dst), src);
}

template <class Machine>
void AssemblerX86Base<Machine>::mulss(Type Ty, typename Traits::XmmRegister dst,
                                      typename Traits::XmmRegister src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(isFloat32Asserting32Or64(Ty) ? 0xF3 : 0xF2);
  emitRexRB(RexTypeIrrelevant, dst, src);
  emitUint8(0x0F);
  emitUint8(0x59);
  emitXmmRegisterOperand(dst, src);
}

template <class Machine>
void AssemblerX86Base<Machine>::mulss(Type Ty, typename Traits::XmmRegister dst,
                                      const typename Traits::Address &src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(isFloat32Asserting32Or64(Ty) ? 0xF3 : 0xF2);
  emitRex(RexTypeIrrelevant, src, dst);
  emitUint8(0x0F);
  emitUint8(0x59);
  emitOperand(gprEncoding(dst), src);
}

template <class Machine>
void AssemblerX86Base<Machine>::divss(Type Ty, typename Traits::XmmRegister dst,
                                      typename Traits::XmmRegister src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(isFloat32Asserting32Or64(Ty) ? 0xF3 : 0xF2);
  emitRexRB(RexTypeIrrelevant, dst, src);
  emitUint8(0x0F);
  emitUint8(0x5E);
  emitXmmRegisterOperand(dst, src);
}

template <class Machine>
void AssemblerX86Base<Machine>::divss(Type Ty, typename Traits::XmmRegister dst,
                                      const typename Traits::Address &src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(isFloat32Asserting32Or64(Ty) ? 0xF3 : 0xF2);
  emitRex(RexTypeIrrelevant, src, dst);
  emitUint8(0x0F);
  emitUint8(0x5E);
  emitOperand(gprEncoding(dst), src);
}

template <class Machine>
template <typename T, typename>
void AssemblerX86Base<Machine>::fld(Type Ty, const typename T::Address &src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(isFloat32Asserting32Or64(Ty) ? 0xD9 : 0xDD);
  emitOperand(0, src);
}

template <class Machine>
template <typename T, typename>
void AssemblerX86Base<Machine>::fstp(Type Ty, const typename T::Address &dst) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(isFloat32Asserting32Or64(Ty) ? 0xD9 : 0xDD);
  emitOperand(3, dst);
}

template <class Machine>
template <typename T, typename>
void AssemblerX86Base<Machine>::fstp(typename T::X87STRegister st) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0xDD);
  emitUint8(0xD8 + st);
}

template <class Machine>
void AssemblerX86Base<Machine>::movaps(typename Traits::XmmRegister dst,
                                       typename Traits::XmmRegister src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitRexRB(RexTypeIrrelevant, dst, src);
  emitUint8(0x0F);
  emitUint8(0x28);
  emitXmmRegisterOperand(dst, src);
}

template <class Machine>
void AssemblerX86Base<Machine>::movups(typename Traits::XmmRegister dst,
                                       typename Traits::XmmRegister src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitRexRB(RexTypeIrrelevant, dst, src);
  emitUint8(0x0F);
  emitUint8(0x10);
  emitXmmRegisterOperand(dst, src);
}

template <class Machine>
void AssemblerX86Base<Machine>::movups(typename Traits::XmmRegister dst,
                                       const typename Traits::Address &src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitRex(RexTypeIrrelevant, src, dst);
  emitUint8(0x0F);
  emitUint8(0x10);
  emitOperand(gprEncoding(dst), src);
}

template <class Machine>
void AssemblerX86Base<Machine>::movups(const typename Traits::Address &dst,
                                       typename Traits::XmmRegister src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitRex(RexTypeIrrelevant, dst, src);
  emitUint8(0x0F);
  emitUint8(0x11);
  emitOperand(gprEncoding(src), dst);
}

template <class Machine>
void AssemblerX86Base<Machine>::padd(Type Ty, typename Traits::XmmRegister dst,
                                     typename Traits::XmmRegister src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0x66);
  emitRexRB(RexTypeIrrelevant, dst, src);
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

template <class Machine>
void AssemblerX86Base<Machine>::padd(Type Ty, typename Traits::XmmRegister dst,
                                     const typename Traits::Address &src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0x66);
  emitRex(RexTypeIrrelevant, src, dst);
  emitUint8(0x0F);
  if (isByteSizedArithType(Ty)) {
    emitUint8(0xFC);
  } else if (Ty == IceType_i16) {
    emitUint8(0xFD);
  } else {
    emitUint8(0xFE);
  }
  emitOperand(gprEncoding(dst), src);
}

template <class Machine>
void AssemblerX86Base<Machine>::pand(Type /* Ty */,
                                     typename Traits::XmmRegister dst,
                                     typename Traits::XmmRegister src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0x66);
  emitRexRB(RexTypeIrrelevant, dst, src);
  emitUint8(0x0F);
  emitUint8(0xDB);
  emitXmmRegisterOperand(dst, src);
}

template <class Machine>
void AssemblerX86Base<Machine>::pand(Type /* Ty */,
                                     typename Traits::XmmRegister dst,
                                     const typename Traits::Address &src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0x66);
  emitRex(RexTypeIrrelevant, src, dst);
  emitUint8(0x0F);
  emitUint8(0xDB);
  emitOperand(gprEncoding(dst), src);
}

template <class Machine>
void AssemblerX86Base<Machine>::pandn(Type /* Ty */,
                                      typename Traits::XmmRegister dst,
                                      typename Traits::XmmRegister src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0x66);
  emitRexRB(RexTypeIrrelevant, dst, src);
  emitUint8(0x0F);
  emitUint8(0xDF);
  emitXmmRegisterOperand(dst, src);
}

template <class Machine>
void AssemblerX86Base<Machine>::pandn(Type /* Ty */,
                                      typename Traits::XmmRegister dst,
                                      const typename Traits::Address &src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0x66);
  emitRex(RexTypeIrrelevant, src, dst);
  emitUint8(0x0F);
  emitUint8(0xDF);
  emitOperand(gprEncoding(dst), src);
}

template <class Machine>
void AssemblerX86Base<Machine>::pmull(Type Ty, typename Traits::XmmRegister dst,
                                      typename Traits::XmmRegister src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0x66);
  emitRexRB(RexTypeIrrelevant, dst, src);
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

template <class Machine>
void AssemblerX86Base<Machine>::pmull(Type Ty, typename Traits::XmmRegister dst,
                                      const typename Traits::Address &src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0x66);
  emitRex(RexTypeIrrelevant, src, dst);
  emitUint8(0x0F);
  if (Ty == IceType_i16) {
    emitUint8(0xD5);
  } else {
    assert(Ty == IceType_i32);
    emitUint8(0x38);
    emitUint8(0x40);
  }
  emitOperand(gprEncoding(dst), src);
}

template <class Machine>
void AssemblerX86Base<Machine>::pmuludq(Type /* Ty */,
                                        typename Traits::XmmRegister dst,
                                        typename Traits::XmmRegister src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0x66);
  emitRexRB(RexTypeIrrelevant, dst, src);
  emitUint8(0x0F);
  emitUint8(0xF4);
  emitXmmRegisterOperand(dst, src);
}

template <class Machine>
void AssemblerX86Base<Machine>::pmuludq(Type /* Ty */,
                                        typename Traits::XmmRegister dst,
                                        const typename Traits::Address &src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0x66);
  emitRex(RexTypeIrrelevant, src, dst);
  emitUint8(0x0F);
  emitUint8(0xF4);
  emitOperand(gprEncoding(dst), src);
}

template <class Machine>
void AssemblerX86Base<Machine>::por(Type /* Ty */,
                                    typename Traits::XmmRegister dst,
                                    typename Traits::XmmRegister src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0x66);
  emitRexRB(RexTypeIrrelevant, dst, src);
  emitUint8(0x0F);
  emitUint8(0xEB);
  emitXmmRegisterOperand(dst, src);
}

template <class Machine>
void AssemblerX86Base<Machine>::por(Type /* Ty */,
                                    typename Traits::XmmRegister dst,
                                    const typename Traits::Address &src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0x66);
  emitRex(RexTypeIrrelevant, src, dst);
  emitUint8(0x0F);
  emitUint8(0xEB);
  emitOperand(gprEncoding(dst), src);
}

template <class Machine>
void AssemblerX86Base<Machine>::psub(Type Ty, typename Traits::XmmRegister dst,
                                     typename Traits::XmmRegister src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0x66);
  emitRexRB(RexTypeIrrelevant, dst, src);
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

template <class Machine>
void AssemblerX86Base<Machine>::psub(Type Ty, typename Traits::XmmRegister dst,
                                     const typename Traits::Address &src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0x66);
  emitRex(RexTypeIrrelevant, src, dst);
  emitUint8(0x0F);
  if (isByteSizedArithType(Ty)) {
    emitUint8(0xF8);
  } else if (Ty == IceType_i16) {
    emitUint8(0xF9);
  } else {
    emitUint8(0xFA);
  }
  emitOperand(gprEncoding(dst), src);
}

template <class Machine>
void AssemblerX86Base<Machine>::pxor(Type /* Ty */,
                                     typename Traits::XmmRegister dst,
                                     typename Traits::XmmRegister src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0x66);
  emitRexRB(RexTypeIrrelevant, dst, src);
  emitUint8(0x0F);
  emitUint8(0xEF);
  emitXmmRegisterOperand(dst, src);
}

template <class Machine>
void AssemblerX86Base<Machine>::pxor(Type /* Ty */,
                                     typename Traits::XmmRegister dst,
                                     const typename Traits::Address &src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0x66);
  emitRex(RexTypeIrrelevant, src, dst);
  emitUint8(0x0F);
  emitUint8(0xEF);
  emitOperand(gprEncoding(dst), src);
}

template <class Machine>
void AssemblerX86Base<Machine>::psll(Type Ty, typename Traits::XmmRegister dst,
                                     typename Traits::XmmRegister src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0x66);
  emitRexRB(RexTypeIrrelevant, dst, src);
  emitUint8(0x0F);
  if (Ty == IceType_i16) {
    emitUint8(0xF1);
  } else {
    assert(Ty == IceType_i32);
    emitUint8(0xF2);
  }
  emitXmmRegisterOperand(dst, src);
}

template <class Machine>
void AssemblerX86Base<Machine>::psll(Type Ty, typename Traits::XmmRegister dst,
                                     const typename Traits::Address &src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0x66);
  emitRex(RexTypeIrrelevant, src, dst);
  emitUint8(0x0F);
  if (Ty == IceType_i16) {
    emitUint8(0xF1);
  } else {
    assert(Ty == IceType_i32);
    emitUint8(0xF2);
  }
  emitOperand(gprEncoding(dst), src);
}

template <class Machine>
void AssemblerX86Base<Machine>::psll(Type Ty, typename Traits::XmmRegister dst,
                                     const Immediate &imm) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  assert(imm.is_int8());
  emitUint8(0x66);
  emitRexB(RexTypeIrrelevant, dst);
  emitUint8(0x0F);
  if (Ty == IceType_i16) {
    emitUint8(0x71);
  } else {
    assert(Ty == IceType_i32);
    emitUint8(0x72);
  }
  emitRegisterOperand(6, gprEncoding(dst));
  emitUint8(imm.value() & 0xFF);
}

template <class Machine>
void AssemblerX86Base<Machine>::psra(Type Ty, typename Traits::XmmRegister dst,
                                     typename Traits::XmmRegister src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0x66);
  emitRexRB(RexTypeIrrelevant, dst, src);
  emitUint8(0x0F);
  if (Ty == IceType_i16) {
    emitUint8(0xE1);
  } else {
    assert(Ty == IceType_i32);
    emitUint8(0xE2);
  }
  emitXmmRegisterOperand(dst, src);
}

template <class Machine>
void AssemblerX86Base<Machine>::psra(Type Ty, typename Traits::XmmRegister dst,
                                     const typename Traits::Address &src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0x66);
  emitRex(RexTypeIrrelevant, src, dst);
  emitUint8(0x0F);
  if (Ty == IceType_i16) {
    emitUint8(0xE1);
  } else {
    assert(Ty == IceType_i32);
    emitUint8(0xE2);
  }
  emitOperand(gprEncoding(dst), src);
}

template <class Machine>
void AssemblerX86Base<Machine>::psra(Type Ty, typename Traits::XmmRegister dst,
                                     const Immediate &imm) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  assert(imm.is_int8());
  emitUint8(0x66);
  emitRexB(RexTypeIrrelevant, dst);
  emitUint8(0x0F);
  if (Ty == IceType_i16) {
    emitUint8(0x71);
  } else {
    assert(Ty == IceType_i32);
    emitUint8(0x72);
  }
  emitRegisterOperand(4, gprEncoding(dst));
  emitUint8(imm.value() & 0xFF);
}

template <class Machine>
void AssemblerX86Base<Machine>::psrl(Type Ty, typename Traits::XmmRegister dst,
                                     typename Traits::XmmRegister src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0x66);
  emitRexRB(RexTypeIrrelevant, dst, src);
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

template <class Machine>
void AssemblerX86Base<Machine>::psrl(Type Ty, typename Traits::XmmRegister dst,
                                     const typename Traits::Address &src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0x66);
  emitRex(RexTypeIrrelevant, src, dst);
  emitUint8(0x0F);
  if (Ty == IceType_i16) {
    emitUint8(0xD1);
  } else if (Ty == IceType_f64) {
    emitUint8(0xD3);
  } else {
    assert(Ty == IceType_i32 || Ty == IceType_f32 || Ty == IceType_v4f32);
    emitUint8(0xD2);
  }
  emitOperand(gprEncoding(dst), src);
}

template <class Machine>
void AssemblerX86Base<Machine>::psrl(Type Ty, typename Traits::XmmRegister dst,
                                     const Immediate &imm) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  assert(imm.is_int8());
  emitUint8(0x66);
  emitRexB(RexTypeIrrelevant, dst);
  emitUint8(0x0F);
  if (Ty == IceType_i16) {
    emitUint8(0x71);
  } else if (Ty == IceType_f64) {
    emitUint8(0x73);
  } else {
    assert(Ty == IceType_i32 || Ty == IceType_f32 || Ty == IceType_v4f32);
    emitUint8(0x72);
  }
  emitRegisterOperand(2, gprEncoding(dst));
  emitUint8(imm.value() & 0xFF);
}

// {add,sub,mul,div}ps are given a Ty parameter for consistency with
// {add,sub,mul,div}ss. In the future, when the PNaCl ABI allows
// addpd, etc., we can use the Ty parameter to decide on adding
// a 0x66 prefix.
template <class Machine>
void AssemblerX86Base<Machine>::addps(Type /* Ty */,
                                      typename Traits::XmmRegister dst,
                                      typename Traits::XmmRegister src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitRexRB(RexTypeIrrelevant, dst, src);
  emitUint8(0x0F);
  emitUint8(0x58);
  emitXmmRegisterOperand(dst, src);
}

template <class Machine>
void AssemblerX86Base<Machine>::addps(Type /* Ty */,
                                      typename Traits::XmmRegister dst,
                                      const typename Traits::Address &src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitRex(RexTypeIrrelevant, src, dst);
  emitUint8(0x0F);
  emitUint8(0x58);
  emitOperand(gprEncoding(dst), src);
}

template <class Machine>
void AssemblerX86Base<Machine>::subps(Type /* Ty */,
                                      typename Traits::XmmRegister dst,
                                      typename Traits::XmmRegister src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitRexRB(RexTypeIrrelevant, dst, src);
  emitUint8(0x0F);
  emitUint8(0x5C);
  emitXmmRegisterOperand(dst, src);
}

template <class Machine>
void AssemblerX86Base<Machine>::subps(Type /* Ty */,
                                      typename Traits::XmmRegister dst,
                                      const typename Traits::Address &src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitRex(RexTypeIrrelevant, src, dst);
  emitUint8(0x0F);
  emitUint8(0x5C);
  emitOperand(gprEncoding(dst), src);
}

template <class Machine>
void AssemblerX86Base<Machine>::divps(Type /* Ty */,
                                      typename Traits::XmmRegister dst,
                                      typename Traits::XmmRegister src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitRexRB(RexTypeIrrelevant, dst, src);
  emitUint8(0x0F);
  emitUint8(0x5E);
  emitXmmRegisterOperand(dst, src);
}

template <class Machine>
void AssemblerX86Base<Machine>::divps(Type /* Ty */,
                                      typename Traits::XmmRegister dst,
                                      const typename Traits::Address &src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitRex(RexTypeIrrelevant, src, dst);
  emitUint8(0x0F);
  emitUint8(0x5E);
  emitOperand(gprEncoding(dst), src);
}

template <class Machine>
void AssemblerX86Base<Machine>::mulps(Type /* Ty */,
                                      typename Traits::XmmRegister dst,
                                      typename Traits::XmmRegister src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitRexRB(RexTypeIrrelevant, dst, src);
  emitUint8(0x0F);
  emitUint8(0x59);
  emitXmmRegisterOperand(dst, src);
}

template <class Machine>
void AssemblerX86Base<Machine>::mulps(Type /* Ty */,
                                      typename Traits::XmmRegister dst,
                                      const typename Traits::Address &src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitRex(RexTypeIrrelevant, src, dst);
  emitUint8(0x0F);
  emitUint8(0x59);
  emitOperand(gprEncoding(dst), src);
}

template <class Machine>
void AssemblerX86Base<Machine>::minps(typename Traits::XmmRegister dst,
                                      typename Traits::XmmRegister src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitRexRB(RexTypeIrrelevant, dst, src);
  emitUint8(0x0F);
  emitUint8(0x5D);
  emitXmmRegisterOperand(dst, src);
}

template <class Machine>
void AssemblerX86Base<Machine>::maxps(typename Traits::XmmRegister dst,
                                      typename Traits::XmmRegister src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitRexRB(RexTypeIrrelevant, dst, src);
  emitUint8(0x0F);
  emitUint8(0x5F);
  emitXmmRegisterOperand(dst, src);
}

template <class Machine>
void AssemblerX86Base<Machine>::andps(typename Traits::XmmRegister dst,
                                      typename Traits::XmmRegister src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitRexRB(RexTypeIrrelevant, dst, src);
  emitUint8(0x0F);
  emitUint8(0x54);
  emitXmmRegisterOperand(dst, src);
}

template <class Machine>
void AssemblerX86Base<Machine>::andps(typename Traits::XmmRegister dst,
                                      const typename Traits::Address &src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitRex(RexTypeIrrelevant, src, dst);
  emitUint8(0x0F);
  emitUint8(0x54);
  emitOperand(gprEncoding(dst), src);
}

template <class Machine>
void AssemblerX86Base<Machine>::orps(typename Traits::XmmRegister dst,
                                     typename Traits::XmmRegister src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitRexRB(RexTypeIrrelevant, dst, src);
  emitUint8(0x0F);
  emitUint8(0x56);
  emitXmmRegisterOperand(dst, src);
}

template <class Machine>
void AssemblerX86Base<Machine>::blendvps(Type /* Ty */,
                                         typename Traits::XmmRegister dst,
                                         typename Traits::XmmRegister src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0x66);
  emitRexRB(RexTypeIrrelevant, dst, src);
  emitUint8(0x0F);
  emitUint8(0x38);
  emitUint8(0x14);
  emitXmmRegisterOperand(dst, src);
}

template <class Machine>
void AssemblerX86Base<Machine>::blendvps(Type /* Ty */,
                                         typename Traits::XmmRegister dst,
                                         const typename Traits::Address &src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0x66);
  emitRex(RexTypeIrrelevant, src, dst);
  emitUint8(0x0F);
  emitUint8(0x38);
  emitUint8(0x14);
  emitOperand(gprEncoding(dst), src);
}

template <class Machine>
void AssemblerX86Base<Machine>::pblendvb(Type /* Ty */,
                                         typename Traits::XmmRegister dst,
                                         typename Traits::XmmRegister src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0x66);
  emitRexRB(RexTypeIrrelevant, dst, src);
  emitUint8(0x0F);
  emitUint8(0x38);
  emitUint8(0x10);
  emitXmmRegisterOperand(dst, src);
}

template <class Machine>
void AssemblerX86Base<Machine>::pblendvb(Type /* Ty */,
                                         typename Traits::XmmRegister dst,
                                         const typename Traits::Address &src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0x66);
  emitRex(RexTypeIrrelevant, src, dst);
  emitUint8(0x0F);
  emitUint8(0x38);
  emitUint8(0x10);
  emitOperand(gprEncoding(dst), src);
}

template <class Machine>
void AssemblerX86Base<Machine>::cmpps(
    typename Traits::XmmRegister dst, typename Traits::XmmRegister src,
    typename Traits::Cond::CmppsCond CmpCondition) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitRexRB(RexTypeIrrelevant, dst, src);
  emitUint8(0x0F);
  emitUint8(0xC2);
  emitXmmRegisterOperand(dst, src);
  emitUint8(CmpCondition);
}

template <class Machine>
void AssemblerX86Base<Machine>::cmpps(
    typename Traits::XmmRegister dst, const typename Traits::Address &src,
    typename Traits::Cond::CmppsCond CmpCondition) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitRex(RexTypeIrrelevant, src, dst);
  emitUint8(0x0F);
  emitUint8(0xC2);
  emitOperand(gprEncoding(dst), src);
  emitUint8(CmpCondition);
}

template <class Machine>
void AssemblerX86Base<Machine>::sqrtps(typename Traits::XmmRegister dst) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitRexRB(RexTypeIrrelevant, dst, dst);
  emitUint8(0x0F);
  emitUint8(0x51);
  emitXmmRegisterOperand(dst, dst);
}

template <class Machine>
void AssemblerX86Base<Machine>::rsqrtps(typename Traits::XmmRegister dst) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitRexRB(RexTypeIrrelevant, dst, dst);
  emitUint8(0x0F);
  emitUint8(0x52);
  emitXmmRegisterOperand(dst, dst);
}

template <class Machine>
void AssemblerX86Base<Machine>::reciprocalps(typename Traits::XmmRegister dst) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitRexRB(RexTypeIrrelevant, dst, dst);
  emitUint8(0x0F);
  emitUint8(0x53);
  emitXmmRegisterOperand(dst, dst);
}

template <class Machine>
void AssemblerX86Base<Machine>::movhlps(typename Traits::XmmRegister dst,
                                        typename Traits::XmmRegister src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitRexRB(RexTypeIrrelevant, dst, src);
  emitUint8(0x0F);
  emitUint8(0x12);
  emitXmmRegisterOperand(dst, src);
}

template <class Machine>
void AssemblerX86Base<Machine>::movlhps(typename Traits::XmmRegister dst,
                                        typename Traits::XmmRegister src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitRexRB(RexTypeIrrelevant, dst, src);
  emitUint8(0x0F);
  emitUint8(0x16);
  emitXmmRegisterOperand(dst, src);
}

template <class Machine>
void AssemblerX86Base<Machine>::unpcklps(typename Traits::XmmRegister dst,
                                         typename Traits::XmmRegister src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitRexRB(RexTypeIrrelevant, dst, src);
  emitUint8(0x0F);
  emitUint8(0x14);
  emitXmmRegisterOperand(dst, src);
}

template <class Machine>
void AssemblerX86Base<Machine>::unpckhps(typename Traits::XmmRegister dst,
                                         typename Traits::XmmRegister src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitRexRB(RexTypeIrrelevant, dst, src);
  emitUint8(0x0F);
  emitUint8(0x15);
  emitXmmRegisterOperand(dst, src);
}

template <class Machine>
void AssemblerX86Base<Machine>::unpcklpd(typename Traits::XmmRegister dst,
                                         typename Traits::XmmRegister src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0x66);
  emitRexRB(RexTypeIrrelevant, dst, src);
  emitUint8(0x0F);
  emitUint8(0x14);
  emitXmmRegisterOperand(dst, src);
}

template <class Machine>
void AssemblerX86Base<Machine>::unpckhpd(typename Traits::XmmRegister dst,
                                         typename Traits::XmmRegister src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0x66);
  emitRexRB(RexTypeIrrelevant, dst, src);
  emitUint8(0x0F);
  emitUint8(0x15);
  emitXmmRegisterOperand(dst, src);
}

template <class Machine>
void AssemblerX86Base<Machine>::set1ps(typename Traits::XmmRegister dst,
                                       typename Traits::GPRRegister tmp1,
                                       const Immediate &imm) {
  // Load 32-bit immediate value into tmp1.
  mov(IceType_i32, tmp1, imm);
  // Move value from tmp1 into dst.
  movd(dst, tmp1);
  // Broadcast low lane into other three lanes.
  shufps(RexTypeIrrelevant, dst, dst, Immediate(0x0));
}

template <class Machine>
void AssemblerX86Base<Machine>::pshufd(Type /* Ty */,
                                       typename Traits::XmmRegister dst,
                                       typename Traits::XmmRegister src,
                                       const Immediate &imm) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0x66);
  emitRexRB(RexTypeIrrelevant, dst, src);
  emitUint8(0x0F);
  emitUint8(0x70);
  emitXmmRegisterOperand(dst, src);
  assert(imm.is_uint8());
  emitUint8(imm.value());
}

template <class Machine>
void AssemblerX86Base<Machine>::pshufd(Type /* Ty */,
                                       typename Traits::XmmRegister dst,
                                       const typename Traits::Address &src,
                                       const Immediate &imm) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0x66);
  emitRex(RexTypeIrrelevant, src, dst);
  emitUint8(0x0F);
  emitUint8(0x70);
  emitOperand(gprEncoding(dst), src);
  assert(imm.is_uint8());
  emitUint8(imm.value());
}

template <class Machine>
void AssemblerX86Base<Machine>::shufps(Type /* Ty */,
                                       typename Traits::XmmRegister dst,
                                       typename Traits::XmmRegister src,
                                       const Immediate &imm) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitRexRB(RexTypeIrrelevant, dst, src);
  emitUint8(0x0F);
  emitUint8(0xC6);
  emitXmmRegisterOperand(dst, src);
  assert(imm.is_uint8());
  emitUint8(imm.value());
}

template <class Machine>
void AssemblerX86Base<Machine>::shufps(Type /* Ty */,
                                       typename Traits::XmmRegister dst,
                                       const typename Traits::Address &src,
                                       const Immediate &imm) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitRex(RexTypeIrrelevant, src, dst);
  emitUint8(0x0F);
  emitUint8(0xC6);
  emitOperand(gprEncoding(dst), src);
  assert(imm.is_uint8());
  emitUint8(imm.value());
}

template <class Machine>
void AssemblerX86Base<Machine>::minpd(typename Traits::XmmRegister dst,
                                      typename Traits::XmmRegister src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0x66);
  emitRexRB(RexTypeIrrelevant, dst, src);
  emitUint8(0x0F);
  emitUint8(0x5D);
  emitXmmRegisterOperand(dst, src);
}

template <class Machine>
void AssemblerX86Base<Machine>::maxpd(typename Traits::XmmRegister dst,
                                      typename Traits::XmmRegister src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0x66);
  emitRexRB(RexTypeIrrelevant, dst, src);
  emitUint8(0x0F);
  emitUint8(0x5F);
  emitXmmRegisterOperand(dst, src);
}

template <class Machine>
void AssemblerX86Base<Machine>::sqrtpd(typename Traits::XmmRegister dst) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0x66);
  emitRexRB(RexTypeIrrelevant, dst, dst);
  emitUint8(0x0F);
  emitUint8(0x51);
  emitXmmRegisterOperand(dst, dst);
}

template <class Machine>
void AssemblerX86Base<Machine>::cvtdq2ps(Type /* Ignore */,
                                         typename Traits::XmmRegister dst,
                                         typename Traits::XmmRegister src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitRexRB(RexTypeIrrelevant, dst, src);
  emitUint8(0x0F);
  emitUint8(0x5B);
  emitXmmRegisterOperand(dst, src);
}

template <class Machine>
void AssemblerX86Base<Machine>::cvtdq2ps(Type /* Ignore */,
                                         typename Traits::XmmRegister dst,
                                         const typename Traits::Address &src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitRex(RexTypeIrrelevant, src, dst);
  emitUint8(0x0F);
  emitUint8(0x5B);
  emitOperand(gprEncoding(dst), src);
}

template <class Machine>
void AssemblerX86Base<Machine>::cvttps2dq(Type /* Ignore */,
                                          typename Traits::XmmRegister dst,
                                          typename Traits::XmmRegister src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0xF3);
  emitRexRB(RexTypeIrrelevant, dst, src);
  emitUint8(0x0F);
  emitUint8(0x5B);
  emitXmmRegisterOperand(dst, src);
}

template <class Machine>
void AssemblerX86Base<Machine>::cvttps2dq(Type /* Ignore */,
                                          typename Traits::XmmRegister dst,
                                          const typename Traits::Address &src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0xF3);
  emitRex(RexTypeIrrelevant, src, dst);
  emitUint8(0x0F);
  emitUint8(0x5B);
  emitOperand(gprEncoding(dst), src);
}

template <class Machine>
void AssemblerX86Base<Machine>::cvtsi2ss(Type DestTy,
                                         typename Traits::XmmRegister dst,
                                         typename Traits::GPRRegister src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(isFloat32Asserting32Or64(DestTy) ? 0xF3 : 0xF2);
  emitRexRB(RexTypeIrrelevant, dst, src);
  emitUint8(0x0F);
  emitUint8(0x2A);
  emitXmmRegisterOperand(dst, src);
}

template <class Machine>
void AssemblerX86Base<Machine>::cvtsi2ss(Type DestTy,
                                         typename Traits::XmmRegister dst,
                                         const typename Traits::Address &src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(isFloat32Asserting32Or64(DestTy) ? 0xF3 : 0xF2);
  emitRex(RexTypeIrrelevant, src, dst);
  emitUint8(0x0F);
  emitUint8(0x2A);
  emitOperand(gprEncoding(dst), src);
}

template <class Machine>
void AssemblerX86Base<Machine>::cvtfloat2float(
    Type SrcTy, typename Traits::XmmRegister dst,
    typename Traits::XmmRegister src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  // ss2sd or sd2ss
  emitUint8(isFloat32Asserting32Or64(SrcTy) ? 0xF3 : 0xF2);
  emitRexRB(RexTypeIrrelevant, dst, src);
  emitUint8(0x0F);
  emitUint8(0x5A);
  emitXmmRegisterOperand(dst, src);
}

template <class Machine>
void AssemblerX86Base<Machine>::cvtfloat2float(
    Type SrcTy, typename Traits::XmmRegister dst,
    const typename Traits::Address &src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(isFloat32Asserting32Or64(SrcTy) ? 0xF3 : 0xF2);
  emitRex(RexTypeIrrelevant, src, dst);
  emitUint8(0x0F);
  emitUint8(0x5A);
  emitOperand(gprEncoding(dst), src);
}

template <class Machine>
void AssemblerX86Base<Machine>::cvttss2si(Type SrcTy,
                                          typename Traits::GPRRegister dst,
                                          typename Traits::XmmRegister src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(isFloat32Asserting32Or64(SrcTy) ? 0xF3 : 0xF2);
  emitRexRB(RexTypeIrrelevant, dst, src);
  emitUint8(0x0F);
  emitUint8(0x2C);
  emitXmmRegisterOperand(dst, src);
}

template <class Machine>
void AssemblerX86Base<Machine>::cvttss2si(Type SrcTy,
                                          typename Traits::GPRRegister dst,
                                          const typename Traits::Address &src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(isFloat32Asserting32Or64(SrcTy) ? 0xF3 : 0xF2);
  emitRex(RexTypeIrrelevant, src, dst);
  emitUint8(0x0F);
  emitUint8(0x2C);
  emitOperand(gprEncoding(dst), src);
}

template <class Machine>
void AssemblerX86Base<Machine>::ucomiss(Type Ty, typename Traits::XmmRegister a,
                                        typename Traits::XmmRegister b) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  if (Ty == IceType_f64)
    emitUint8(0x66);
  emitRexRB(RexTypeIrrelevant, a, b);
  emitUint8(0x0F);
  emitUint8(0x2E);
  emitXmmRegisterOperand(a, b);
}

template <class Machine>
void AssemblerX86Base<Machine>::ucomiss(Type Ty, typename Traits::XmmRegister a,
                                        const typename Traits::Address &b) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  if (Ty == IceType_f64)
    emitUint8(0x66);
  emitRex(RexTypeIrrelevant, b, a);
  emitUint8(0x0F);
  emitUint8(0x2E);
  emitOperand(gprEncoding(a), b);
}

template <class Machine>
void AssemblerX86Base<Machine>::movmskpd(typename Traits::GPRRegister dst,
                                         typename Traits::XmmRegister src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0x66);
  emitRexRB(RexTypeIrrelevant, dst, src);
  emitUint8(0x0F);
  emitUint8(0x50);
  emitXmmRegisterOperand(dst, src);
}

template <class Machine>
void AssemblerX86Base<Machine>::movmskps(typename Traits::GPRRegister dst,
                                         typename Traits::XmmRegister src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitRexRB(RexTypeIrrelevant, dst, src);
  emitUint8(0x0F);
  emitUint8(0x50);
  emitXmmRegisterOperand(dst, src);
}

template <class Machine>
void AssemblerX86Base<Machine>::sqrtss(Type Ty,
                                       typename Traits::XmmRegister dst,
                                       const typename Traits::Address &src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(isFloat32Asserting32Or64(Ty) ? 0xF3 : 0xF2);
  emitRex(RexTypeIrrelevant, src, dst);
  emitUint8(0x0F);
  emitUint8(0x51);
  emitOperand(gprEncoding(dst), src);
}

template <class Machine>
void AssemblerX86Base<Machine>::sqrtss(Type Ty,
                                       typename Traits::XmmRegister dst,
                                       typename Traits::XmmRegister src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(isFloat32Asserting32Or64(Ty) ? 0xF3 : 0xF2);
  emitRexRB(RexTypeIrrelevant, dst, src);
  emitUint8(0x0F);
  emitUint8(0x51);
  emitXmmRegisterOperand(dst, src);
}

template <class Machine>
void AssemblerX86Base<Machine>::xorpd(typename Traits::XmmRegister dst,
                                      const typename Traits::Address &src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0x66);
  emitRex(RexTypeIrrelevant, src, dst);
  emitUint8(0x0F);
  emitUint8(0x57);
  emitOperand(gprEncoding(dst), src);
}

template <class Machine>
void AssemblerX86Base<Machine>::xorpd(typename Traits::XmmRegister dst,
                                      typename Traits::XmmRegister src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0x66);
  emitRexRB(RexTypeIrrelevant, dst, src);
  emitUint8(0x0F);
  emitUint8(0x57);
  emitXmmRegisterOperand(dst, src);
}

template <class Machine>
void AssemblerX86Base<Machine>::orpd(typename Traits::XmmRegister dst,
                                     typename Traits::XmmRegister src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0x66);
  emitRexRB(RexTypeIrrelevant, dst, src);
  emitUint8(0x0F);
  emitUint8(0x56);
  emitXmmRegisterOperand(dst, src);
}

template <class Machine>
void AssemblerX86Base<Machine>::xorps(typename Traits::XmmRegister dst,
                                      const typename Traits::Address &src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitRex(RexTypeIrrelevant, src, dst);
  emitUint8(0x0F);
  emitUint8(0x57);
  emitOperand(gprEncoding(dst), src);
}

template <class Machine>
void AssemblerX86Base<Machine>::xorps(typename Traits::XmmRegister dst,
                                      typename Traits::XmmRegister src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitRexRB(RexTypeIrrelevant, dst, src);
  emitUint8(0x0F);
  emitUint8(0x57);
  emitXmmRegisterOperand(dst, src);
}

template <class Machine>
void AssemblerX86Base<Machine>::andpd(typename Traits::XmmRegister dst,
                                      const typename Traits::Address &src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0x66);
  emitRex(RexTypeIrrelevant, src, dst);
  emitUint8(0x0F);
  emitUint8(0x54);
  emitOperand(gprEncoding(dst), src);
}

template <class Machine>
void AssemblerX86Base<Machine>::andpd(typename Traits::XmmRegister dst,
                                      typename Traits::XmmRegister src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0x66);
  emitRexRB(RexTypeIrrelevant, dst, src);
  emitUint8(0x0F);
  emitUint8(0x54);
  emitXmmRegisterOperand(dst, src);
}

template <class Machine>
void AssemblerX86Base<Machine>::insertps(Type Ty,
                                         typename Traits::XmmRegister dst,
                                         typename Traits::XmmRegister src,
                                         const Immediate &imm) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  assert(imm.is_uint8());
  assert(isVectorFloatingType(Ty));
  (void)Ty;
  emitUint8(0x66);
  emitRexRB(RexTypeIrrelevant, dst, src);
  emitUint8(0x0F);
  emitUint8(0x3A);
  emitUint8(0x21);
  emitXmmRegisterOperand(dst, src);
  emitUint8(imm.value());
}

template <class Machine>
void AssemblerX86Base<Machine>::insertps(Type Ty,
                                         typename Traits::XmmRegister dst,
                                         const typename Traits::Address &src,
                                         const Immediate &imm) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  assert(imm.is_uint8());
  assert(isVectorFloatingType(Ty));
  (void)Ty;
  emitUint8(0x66);
  emitRex(RexTypeIrrelevant, src, dst);
  emitUint8(0x0F);
  emitUint8(0x3A);
  emitUint8(0x21);
  emitOperand(gprEncoding(dst), src);
  emitUint8(imm.value());
}

template <class Machine>
void AssemblerX86Base<Machine>::pinsr(Type Ty, typename Traits::XmmRegister dst,
                                      typename Traits::GPRRegister src,
                                      const Immediate &imm) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  assert(imm.is_uint8());
  emitUint8(0x66);
  emitRexRB(Ty, dst, src);
  emitUint8(0x0F);
  if (Ty == IceType_i16) {
    emitUint8(0xC4);
  } else {
    emitUint8(0x3A);
    emitUint8(isByteSizedType(Ty) ? 0x20 : 0x22);
  }
  emitXmmRegisterOperand(dst, src);
  emitUint8(imm.value());
}

template <class Machine>
void AssemblerX86Base<Machine>::pinsr(Type Ty, typename Traits::XmmRegister dst,
                                      const typename Traits::Address &src,
                                      const Immediate &imm) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  assert(imm.is_uint8());
  emitUint8(0x66);
  emitRex(RexTypeIrrelevant, src, dst);
  emitUint8(0x0F);
  if (Ty == IceType_i16) {
    emitUint8(0xC4);
  } else {
    emitUint8(0x3A);
    emitUint8(isByteSizedType(Ty) ? 0x20 : 0x22);
  }
  emitOperand(gprEncoding(dst), src);
  emitUint8(imm.value());
}

template <class Machine>
void AssemblerX86Base<Machine>::pextr(Type Ty, typename Traits::GPRRegister dst,
                                      typename Traits::XmmRegister src,
                                      const Immediate &imm) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  assert(imm.is_uint8());
  if (Ty == IceType_i16) {
    emitUint8(0x66);
    emitRexRB(Ty, dst, src);
    emitUint8(0x0F);
    emitUint8(0xC5);
    emitXmmRegisterOperand(dst, src);
    emitUint8(imm.value());
  } else {
    emitUint8(0x66);
    emitRexRB(Ty, src, dst);
    emitUint8(0x0F);
    emitUint8(0x3A);
    emitUint8(isByteSizedType(Ty) ? 0x14 : 0x16);
    // SSE 4.1 versions are "MRI" because dst can be mem, while
    // pextrw (SSE2) is RMI because dst must be reg.
    emitXmmRegisterOperand(src, dst);
    emitUint8(imm.value());
  }
}

template <class Machine>
void AssemblerX86Base<Machine>::pmovsxdq(typename Traits::XmmRegister dst,
                                         typename Traits::XmmRegister src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0x66);
  emitRexRB(RexTypeIrrelevant, dst, src);
  emitUint8(0x0F);
  emitUint8(0x38);
  emitUint8(0x25);
  emitXmmRegisterOperand(dst, src);
}

template <class Machine>
void AssemblerX86Base<Machine>::pcmpeq(Type Ty,
                                       typename Traits::XmmRegister dst,
                                       typename Traits::XmmRegister src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0x66);
  emitRexRB(RexTypeIrrelevant, dst, src);
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

template <class Machine>
void AssemblerX86Base<Machine>::pcmpeq(Type Ty,
                                       typename Traits::XmmRegister dst,
                                       const typename Traits::Address &src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0x66);
  emitRex(RexTypeIrrelevant, src, dst);
  emitUint8(0x0F);
  if (isByteSizedArithType(Ty)) {
    emitUint8(0x74);
  } else if (Ty == IceType_i16) {
    emitUint8(0x75);
  } else {
    emitUint8(0x76);
  }
  emitOperand(gprEncoding(dst), src);
}

template <class Machine>
void AssemblerX86Base<Machine>::pcmpgt(Type Ty,
                                       typename Traits::XmmRegister dst,
                                       typename Traits::XmmRegister src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0x66);
  emitRexRB(RexTypeIrrelevant, dst, src);
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

template <class Machine>
void AssemblerX86Base<Machine>::pcmpgt(Type Ty,
                                       typename Traits::XmmRegister dst,
                                       const typename Traits::Address &src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0x66);
  emitRex(RexTypeIrrelevant, src, dst);
  emitUint8(0x0F);
  if (isByteSizedArithType(Ty)) {
    emitUint8(0x64);
  } else if (Ty == IceType_i16) {
    emitUint8(0x65);
  } else {
    emitUint8(0x66);
  }
  emitOperand(gprEncoding(dst), src);
}

template <class Machine>
void AssemblerX86Base<Machine>::roundsd(typename Traits::XmmRegister dst,
                                        typename Traits::XmmRegister src,
                                        RoundingMode mode) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0x66);
  emitRexRB(RexTypeIrrelevant, dst, src);
  emitUint8(0x0F);
  emitUint8(0x3A);
  emitUint8(0x0B);
  emitXmmRegisterOperand(dst, src);
  // Mask precision exeption.
  emitUint8(static_cast<uint8_t>(mode) | 0x8);
}

template <class Machine>
template <typename T, typename>
void AssemblerX86Base<Machine>::fnstcw(const typename T::Address &dst) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0xD9);
  emitOperand(7, dst);
}

template <class Machine>
template <typename T, typename>
void AssemblerX86Base<Machine>::fldcw(const typename T::Address &src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0xD9);
  emitOperand(5, src);
}

template <class Machine>
template <typename T, typename>
void AssemblerX86Base<Machine>::fistpl(const typename T::Address &dst) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0xDF);
  emitOperand(7, dst);
}

template <class Machine>
template <typename T, typename>
void AssemblerX86Base<Machine>::fistps(const typename T::Address &dst) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0xDB);
  emitOperand(3, dst);
}

template <class Machine>
template <typename T, typename>
void AssemblerX86Base<Machine>::fildl(const typename T::Address &src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0xDF);
  emitOperand(5, src);
}

template <class Machine>
template <typename T, typename>
void AssemblerX86Base<Machine>::filds(const typename T::Address &src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0xDB);
  emitOperand(0, src);
}

template <class Machine>
template <typename, typename>
void AssemblerX86Base<Machine>::fincstp() {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0xD9);
  emitUint8(0xF7);
}

template <class Machine>
template <uint32_t Tag>
void AssemblerX86Base<Machine>::arith_int(Type Ty,
                                          typename Traits::GPRRegister reg,
                                          const Immediate &imm) {
  static_assert(Tag < 8, "Tag must be between 0..7");
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  if (Ty == IceType_i16)
    emitOperandSizeOverride();
  emitRexB(Ty, reg);
  if (isByteSizedType(Ty)) {
    emitComplexI8(Tag, typename Traits::Operand(reg), imm);
  } else {
    emitComplex(Ty, Tag, typename Traits::Operand(reg), imm);
  }
}

template <class Machine>
template <uint32_t Tag>
void AssemblerX86Base<Machine>::arith_int(Type Ty,
                                          typename Traits::GPRRegister reg0,
                                          typename Traits::GPRRegister reg1) {
  static_assert(Tag < 8, "Tag must be between 0..7");
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  if (Ty == IceType_i16)
    emitOperandSizeOverride();
  emitRexRB(Ty, reg0, reg1);
  if (isByteSizedType(Ty))
    emitUint8(Tag * 8 + 2);
  else
    emitUint8(Tag * 8 + 3);
  emitRegisterOperand(gprEncoding(reg0), gprEncoding(reg1));
}

template <class Machine>
template <uint32_t Tag>
void AssemblerX86Base<Machine>::arith_int(
    Type Ty, typename Traits::GPRRegister reg,
    const typename Traits::Address &address) {
  static_assert(Tag < 8, "Tag must be between 0..7");
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  if (Ty == IceType_i16)
    emitOperandSizeOverride();
  emitRex(Ty, address, reg);
  if (isByteSizedType(Ty))
    emitUint8(Tag * 8 + 2);
  else
    emitUint8(Tag * 8 + 3);
  emitOperand(gprEncoding(reg), address);
}

template <class Machine>
template <uint32_t Tag>
void AssemblerX86Base<Machine>::arith_int(
    Type Ty, const typename Traits::Address &address,
    typename Traits::GPRRegister reg) {
  static_assert(Tag < 8, "Tag must be between 0..7");
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  if (Ty == IceType_i16)
    emitOperandSizeOverride();
  emitRex(Ty, address, reg);
  if (isByteSizedType(Ty))
    emitUint8(Tag * 8 + 0);
  else
    emitUint8(Tag * 8 + 1);
  emitOperand(gprEncoding(reg), address);
}

template <class Machine>
template <uint32_t Tag>
void AssemblerX86Base<Machine>::arith_int(
    Type Ty, const typename Traits::Address &address, const Immediate &imm) {
  static_assert(Tag < 8, "Tag must be between 0..7");
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  if (Ty == IceType_i16)
    emitOperandSizeOverride();
  emitRex(Ty, address, RexRegIrrelevant);
  if (isByteSizedType(Ty)) {
    emitComplexI8(Tag, address, imm);
  } else {
    emitComplex(Ty, Tag, address, imm);
  }
}

template <class Machine>
void AssemblerX86Base<Machine>::cmp(Type Ty, typename Traits::GPRRegister reg,
                                    const Immediate &imm) {
  arith_int<7>(Ty, reg, imm);
}

template <class Machine>
void AssemblerX86Base<Machine>::cmp(Type Ty, typename Traits::GPRRegister reg0,
                                    typename Traits::GPRRegister reg1) {
  arith_int<7>(Ty, reg0, reg1);
}

template <class Machine>
void AssemblerX86Base<Machine>::cmp(Type Ty, typename Traits::GPRRegister reg,
                                    const typename Traits::Address &address) {
  arith_int<7>(Ty, reg, address);
}

template <class Machine>
void AssemblerX86Base<Machine>::cmp(Type Ty,
                                    const typename Traits::Address &address,
                                    typename Traits::GPRRegister reg) {
  arith_int<7>(Ty, address, reg);
}

template <class Machine>
void AssemblerX86Base<Machine>::cmp(Type Ty,
                                    const typename Traits::Address &address,
                                    const Immediate &imm) {
  arith_int<7>(Ty, address, imm);
}

template <class Machine>
void AssemblerX86Base<Machine>::test(Type Ty, typename Traits::GPRRegister reg1,
                                     typename Traits::GPRRegister reg2) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  if (Ty == IceType_i16)
    emitOperandSizeOverride();
  emitRexRB(Ty, reg1, reg2);
  if (isByteSizedType(Ty))
    emitUint8(0x84);
  else
    emitUint8(0x85);
  emitRegisterOperand(gprEncoding(reg1), gprEncoding(reg2));
}

template <class Machine>
void AssemblerX86Base<Machine>::test(Type Ty,
                                     const typename Traits::Address &addr,
                                     typename Traits::GPRRegister reg) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  if (Ty == IceType_i16)
    emitOperandSizeOverride();
  emitRex(Ty, addr, reg);
  if (isByteSizedType(Ty))
    emitUint8(0x84);
  else
    emitUint8(0x85);
  emitOperand(gprEncoding(reg), addr);
}

template <class Machine>
void AssemblerX86Base<Machine>::test(Type Ty, typename Traits::GPRRegister reg,
                                     const Immediate &immediate) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  // For registers that have a byte variant (EAX, EBX, ECX, and EDX)
  // we only test the byte register to keep the encoding short.
  // This is legal even if the register had high bits set since
  // this only sets flags registers based on the "AND" of the two operands,
  // and the immediate had zeros at those high bits.
  if (immediate.is_uint8() && reg <= Traits::Last8BitGPR) {
    // Use zero-extended 8-bit immediate.
    emitRexB(Ty, reg);
    if (reg == Traits::Encoded_Reg_Accumulator) {
      emitUint8(0xA8);
    } else {
      emitUint8(0xF6);
      emitUint8(0xC0 + gprEncoding(reg));
    }
    emitUint8(immediate.value() & 0xFF);
  } else if (reg == Traits::Encoded_Reg_Accumulator) {
    // Use short form if the destination is EAX.
    if (Ty == IceType_i16)
      emitOperandSizeOverride();
    emitUint8(0xA9);
    emitImmediate(Ty, immediate);
  } else {
    if (Ty == IceType_i16)
      emitOperandSizeOverride();
    emitRexB(Ty, reg);
    emitUint8(0xF7);
    emitRegisterOperand(0, gprEncoding(reg));
    emitImmediate(Ty, immediate);
  }
}

template <class Machine>
void AssemblerX86Base<Machine>::test(Type Ty,
                                     const typename Traits::Address &addr,
                                     const Immediate &immediate) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  // If the immediate is short, we only test the byte addr to keep the
  // encoding short.
  if (immediate.is_uint8()) {
    // Use zero-extended 8-bit immediate.
    emitRex(Ty, addr, RexRegIrrelevant);
    emitUint8(0xF6);
    emitOperand(0, addr);
    emitUint8(immediate.value() & 0xFF);
  } else {
    if (Ty == IceType_i16)
      emitOperandSizeOverride();
    emitRex(Ty, addr, RexRegIrrelevant);
    emitUint8(0xF7);
    emitOperand(0, addr);
    emitImmediate(Ty, immediate);
  }
}

template <class Machine>
void AssemblerX86Base<Machine>::And(Type Ty, typename Traits::GPRRegister dst,
                                    typename Traits::GPRRegister src) {
  arith_int<4>(Ty, dst, src);
}

template <class Machine>
void AssemblerX86Base<Machine>::And(Type Ty, typename Traits::GPRRegister dst,
                                    const typename Traits::Address &address) {
  arith_int<4>(Ty, dst, address);
}

template <class Machine>
void AssemblerX86Base<Machine>::And(Type Ty, typename Traits::GPRRegister dst,
                                    const Immediate &imm) {
  arith_int<4>(Ty, dst, imm);
}

template <class Machine>
void AssemblerX86Base<Machine>::And(Type Ty,
                                    const typename Traits::Address &address,
                                    typename Traits::GPRRegister reg) {
  arith_int<4>(Ty, address, reg);
}

template <class Machine>
void AssemblerX86Base<Machine>::And(Type Ty,
                                    const typename Traits::Address &address,
                                    const Immediate &imm) {
  arith_int<4>(Ty, address, imm);
}

template <class Machine>
void AssemblerX86Base<Machine>::Or(Type Ty, typename Traits::GPRRegister dst,
                                   typename Traits::GPRRegister src) {
  arith_int<1>(Ty, dst, src);
}

template <class Machine>
void AssemblerX86Base<Machine>::Or(Type Ty, typename Traits::GPRRegister dst,
                                   const typename Traits::Address &address) {
  arith_int<1>(Ty, dst, address);
}

template <class Machine>
void AssemblerX86Base<Machine>::Or(Type Ty, typename Traits::GPRRegister dst,
                                   const Immediate &imm) {
  arith_int<1>(Ty, dst, imm);
}

template <class Machine>
void AssemblerX86Base<Machine>::Or(Type Ty,
                                   const typename Traits::Address &address,
                                   typename Traits::GPRRegister reg) {
  arith_int<1>(Ty, address, reg);
}

template <class Machine>
void AssemblerX86Base<Machine>::Or(Type Ty,
                                   const typename Traits::Address &address,
                                   const Immediate &imm) {
  arith_int<1>(Ty, address, imm);
}

template <class Machine>
void AssemblerX86Base<Machine>::Xor(Type Ty, typename Traits::GPRRegister dst,
                                    typename Traits::GPRRegister src) {
  arith_int<6>(Ty, dst, src);
}

template <class Machine>
void AssemblerX86Base<Machine>::Xor(Type Ty, typename Traits::GPRRegister dst,
                                    const typename Traits::Address &address) {
  arith_int<6>(Ty, dst, address);
}

template <class Machine>
void AssemblerX86Base<Machine>::Xor(Type Ty, typename Traits::GPRRegister dst,
                                    const Immediate &imm) {
  arith_int<6>(Ty, dst, imm);
}

template <class Machine>
void AssemblerX86Base<Machine>::Xor(Type Ty,
                                    const typename Traits::Address &address,
                                    typename Traits::GPRRegister reg) {
  arith_int<6>(Ty, address, reg);
}

template <class Machine>
void AssemblerX86Base<Machine>::Xor(Type Ty,
                                    const typename Traits::Address &address,
                                    const Immediate &imm) {
  arith_int<6>(Ty, address, imm);
}

template <class Machine>
void AssemblerX86Base<Machine>::add(Type Ty, typename Traits::GPRRegister dst,
                                    typename Traits::GPRRegister src) {
  arith_int<0>(Ty, dst, src);
}

template <class Machine>
void AssemblerX86Base<Machine>::add(Type Ty, typename Traits::GPRRegister reg,
                                    const typename Traits::Address &address) {
  arith_int<0>(Ty, reg, address);
}

template <class Machine>
void AssemblerX86Base<Machine>::add(Type Ty, typename Traits::GPRRegister reg,
                                    const Immediate &imm) {
  arith_int<0>(Ty, reg, imm);
}

template <class Machine>
void AssemblerX86Base<Machine>::add(Type Ty,
                                    const typename Traits::Address &address,
                                    typename Traits::GPRRegister reg) {
  arith_int<0>(Ty, address, reg);
}

template <class Machine>
void AssemblerX86Base<Machine>::add(Type Ty,
                                    const typename Traits::Address &address,
                                    const Immediate &imm) {
  arith_int<0>(Ty, address, imm);
}

template <class Machine>
void AssemblerX86Base<Machine>::adc(Type Ty, typename Traits::GPRRegister dst,
                                    typename Traits::GPRRegister src) {
  arith_int<2>(Ty, dst, src);
}

template <class Machine>
void AssemblerX86Base<Machine>::adc(Type Ty, typename Traits::GPRRegister dst,
                                    const typename Traits::Address &address) {
  arith_int<2>(Ty, dst, address);
}

template <class Machine>
void AssemblerX86Base<Machine>::adc(Type Ty, typename Traits::GPRRegister reg,
                                    const Immediate &imm) {
  arith_int<2>(Ty, reg, imm);
}

template <class Machine>
void AssemblerX86Base<Machine>::adc(Type Ty,
                                    const typename Traits::Address &address,
                                    typename Traits::GPRRegister reg) {
  arith_int<2>(Ty, address, reg);
}

template <class Machine>
void AssemblerX86Base<Machine>::adc(Type Ty,
                                    const typename Traits::Address &address,
                                    const Immediate &imm) {
  arith_int<2>(Ty, address, imm);
}

template <class Machine>
void AssemblerX86Base<Machine>::sub(Type Ty, typename Traits::GPRRegister dst,
                                    typename Traits::GPRRegister src) {
  arith_int<5>(Ty, dst, src);
}

template <class Machine>
void AssemblerX86Base<Machine>::sub(Type Ty, typename Traits::GPRRegister reg,
                                    const typename Traits::Address &address) {
  arith_int<5>(Ty, reg, address);
}

template <class Machine>
void AssemblerX86Base<Machine>::sub(Type Ty, typename Traits::GPRRegister reg,
                                    const Immediate &imm) {
  arith_int<5>(Ty, reg, imm);
}

template <class Machine>
void AssemblerX86Base<Machine>::sub(Type Ty,
                                    const typename Traits::Address &address,
                                    typename Traits::GPRRegister reg) {
  arith_int<5>(Ty, address, reg);
}

template <class Machine>
void AssemblerX86Base<Machine>::sub(Type Ty,
                                    const typename Traits::Address &address,
                                    const Immediate &imm) {
  arith_int<5>(Ty, address, imm);
}

template <class Machine>
void AssemblerX86Base<Machine>::sbb(Type Ty, typename Traits::GPRRegister dst,
                                    typename Traits::GPRRegister src) {
  arith_int<3>(Ty, dst, src);
}

template <class Machine>
void AssemblerX86Base<Machine>::sbb(Type Ty, typename Traits::GPRRegister dst,
                                    const typename Traits::Address &address) {
  arith_int<3>(Ty, dst, address);
}

template <class Machine>
void AssemblerX86Base<Machine>::sbb(Type Ty, typename Traits::GPRRegister reg,
                                    const Immediate &imm) {
  arith_int<3>(Ty, reg, imm);
}

template <class Machine>
void AssemblerX86Base<Machine>::sbb(Type Ty,
                                    const typename Traits::Address &address,
                                    typename Traits::GPRRegister reg) {
  arith_int<3>(Ty, address, reg);
}

template <class Machine>
void AssemblerX86Base<Machine>::sbb(Type Ty,
                                    const typename Traits::Address &address,
                                    const Immediate &imm) {
  arith_int<3>(Ty, address, imm);
}

template <class Machine> void AssemblerX86Base<Machine>::cbw() {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitOperandSizeOverride();
  emitUint8(0x98);
}

template <class Machine> void AssemblerX86Base<Machine>::cwd() {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitOperandSizeOverride();
  emitUint8(0x99);
}

template <class Machine> void AssemblerX86Base<Machine>::cdq() {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0x99);
}

template <class Machine>
void AssemblerX86Base<Machine>::div(Type Ty, typename Traits::GPRRegister reg) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  if (Ty == IceType_i16)
    emitOperandSizeOverride();
  emitRexB(Ty, reg);
  if (isByteSizedArithType(Ty))
    emitUint8(0xF6);
  else
    emitUint8(0xF7);
  emitRegisterOperand(6, gprEncoding(reg));
}

template <class Machine>
void AssemblerX86Base<Machine>::div(Type Ty,
                                    const typename Traits::Address &addr) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  if (Ty == IceType_i16)
    emitOperandSizeOverride();
  emitRex(Ty, addr, RexRegIrrelevant);
  if (isByteSizedArithType(Ty))
    emitUint8(0xF6);
  else
    emitUint8(0xF7);
  emitOperand(6, addr);
}

template <class Machine>
void AssemblerX86Base<Machine>::idiv(Type Ty,
                                     typename Traits::GPRRegister reg) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  if (Ty == IceType_i16)
    emitOperandSizeOverride();
  emitRexB(Ty, reg);
  if (isByteSizedArithType(Ty))
    emitUint8(0xF6);
  else
    emitUint8(0xF7);
  emitRegisterOperand(7, gprEncoding(reg));
}

template <class Machine>
void AssemblerX86Base<Machine>::idiv(Type Ty,
                                     const typename Traits::Address &addr) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  if (Ty == IceType_i16)
    emitOperandSizeOverride();
  emitRex(Ty, addr, RexRegIrrelevant);
  if (isByteSizedArithType(Ty))
    emitUint8(0xF6);
  else
    emitUint8(0xF7);
  emitOperand(7, addr);
}

template <class Machine>
void AssemblerX86Base<Machine>::imul(Type Ty, typename Traits::GPRRegister dst,
                                     typename Traits::GPRRegister src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  assert(Ty == IceType_i16 || Ty == IceType_i32);
  if (Ty == IceType_i16)
    emitOperandSizeOverride();
  emitRexRB(Ty, dst, src);
  emitUint8(0x0F);
  emitUint8(0xAF);
  emitRegisterOperand(gprEncoding(dst), gprEncoding(src));
}

template <class Machine>
void AssemblerX86Base<Machine>::imul(Type Ty, typename Traits::GPRRegister reg,
                                     const typename Traits::Address &address) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  assert(Ty == IceType_i16 || Ty == IceType_i32);
  if (Ty == IceType_i16)
    emitOperandSizeOverride();
  emitRex(Ty, address, reg);
  emitUint8(0x0F);
  emitUint8(0xAF);
  emitOperand(gprEncoding(reg), address);
}

template <class Machine>
void AssemblerX86Base<Machine>::imul(Type Ty, typename Traits::GPRRegister reg,
                                     const Immediate &imm) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  assert(Ty == IceType_i16 || Ty == IceType_i32);
  if (Ty == IceType_i16)
    emitOperandSizeOverride();
  emitRexRB(Ty, reg, reg);
  if (imm.is_int8()) {
    emitUint8(0x6B);
    emitRegisterOperand(gprEncoding(reg), gprEncoding(reg));
    emitUint8(imm.value() & 0xFF);
  } else {
    emitUint8(0x69);
    emitRegisterOperand(gprEncoding(reg), gprEncoding(reg));
    emitImmediate(Ty, imm);
  }
}

template <class Machine>
void AssemblerX86Base<Machine>::imul(Type Ty,
                                     typename Traits::GPRRegister reg) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  if (Ty == IceType_i16)
    emitOperandSizeOverride();
  emitRexB(Ty, reg);
  if (isByteSizedArithType(Ty))
    emitUint8(0xF6);
  else
    emitUint8(0xF7);
  emitRegisterOperand(5, gprEncoding(reg));
}

template <class Machine>
void AssemblerX86Base<Machine>::imul(Type Ty,
                                     const typename Traits::Address &address) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  if (Ty == IceType_i16)
    emitOperandSizeOverride();
  emitRex(Ty, address, RexRegIrrelevant);
  if (isByteSizedArithType(Ty))
    emitUint8(0xF6);
  else
    emitUint8(0xF7);
  emitOperand(5, address);
}

template <class Machine>
void AssemblerX86Base<Machine>::mul(Type Ty, typename Traits::GPRRegister reg) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  if (Ty == IceType_i16)
    emitOperandSizeOverride();
  emitRexB(Ty, reg);
  if (isByteSizedArithType(Ty))
    emitUint8(0xF6);
  else
    emitUint8(0xF7);
  emitRegisterOperand(4, gprEncoding(reg));
}

template <class Machine>
void AssemblerX86Base<Machine>::mul(Type Ty,
                                    const typename Traits::Address &address) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  if (Ty == IceType_i16)
    emitOperandSizeOverride();
  emitRex(Ty, address, RexRegIrrelevant);
  if (isByteSizedArithType(Ty))
    emitUint8(0xF6);
  else
    emitUint8(0xF7);
  emitOperand(4, address);
}

template <class Machine>
template <typename, typename>
void AssemblerX86Base<Machine>::incl(typename Traits::GPRRegister reg) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0x40 + reg);
}

template <class Machine>
void AssemblerX86Base<Machine>::incl(const typename Traits::Address &address) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitRex(IceType_i32, address, RexRegIrrelevant);
  emitUint8(0xFF);
  emitOperand(0, address);
}

template <class Machine>
template <typename, typename>
void AssemblerX86Base<Machine>::decl(typename Traits::GPRRegister reg) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0x48 + reg);
}

template <class Machine>
void AssemblerX86Base<Machine>::decl(const typename Traits::Address &address) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitRex(IceType_i32, address, RexRegIrrelevant);
  emitUint8(0xFF);
  emitOperand(1, address);
}

template <class Machine>
void AssemblerX86Base<Machine>::rol(Type Ty, typename Traits::GPRRegister reg,
                                    const Immediate &imm) {
  emitGenericShift(0, Ty, reg, imm);
}

template <class Machine>
void AssemblerX86Base<Machine>::rol(Type Ty,
                                    typename Traits::GPRRegister operand,
                                    typename Traits::GPRRegister shifter) {
  emitGenericShift(0, Ty, typename Traits::Operand(operand), shifter);
}

template <class Machine>
void AssemblerX86Base<Machine>::rol(Type Ty,
                                    const typename Traits::Address &operand,
                                    typename Traits::GPRRegister shifter) {
  emitGenericShift(0, Ty, operand, shifter);
}

template <class Machine>
void AssemblerX86Base<Machine>::shl(Type Ty, typename Traits::GPRRegister reg,
                                    const Immediate &imm) {
  emitGenericShift(4, Ty, reg, imm);
}

template <class Machine>
void AssemblerX86Base<Machine>::shl(Type Ty,
                                    typename Traits::GPRRegister operand,
                                    typename Traits::GPRRegister shifter) {
  emitGenericShift(4, Ty, typename Traits::Operand(operand), shifter);
}

template <class Machine>
void AssemblerX86Base<Machine>::shl(Type Ty,
                                    const typename Traits::Address &operand,
                                    typename Traits::GPRRegister shifter) {
  emitGenericShift(4, Ty, operand, shifter);
}

template <class Machine>
void AssemblerX86Base<Machine>::shr(Type Ty, typename Traits::GPRRegister reg,
                                    const Immediate &imm) {
  emitGenericShift(5, Ty, reg, imm);
}

template <class Machine>
void AssemblerX86Base<Machine>::shr(Type Ty,
                                    typename Traits::GPRRegister operand,
                                    typename Traits::GPRRegister shifter) {
  emitGenericShift(5, Ty, typename Traits::Operand(operand), shifter);
}

template <class Machine>
void AssemblerX86Base<Machine>::shr(Type Ty,
                                    const typename Traits::Address &operand,
                                    typename Traits::GPRRegister shifter) {
  emitGenericShift(5, Ty, operand, shifter);
}

template <class Machine>
void AssemblerX86Base<Machine>::sar(Type Ty, typename Traits::GPRRegister reg,
                                    const Immediate &imm) {
  emitGenericShift(7, Ty, reg, imm);
}

template <class Machine>
void AssemblerX86Base<Machine>::sar(Type Ty,
                                    typename Traits::GPRRegister operand,
                                    typename Traits::GPRRegister shifter) {
  emitGenericShift(7, Ty, typename Traits::Operand(operand), shifter);
}

template <class Machine>
void AssemblerX86Base<Machine>::sar(Type Ty,
                                    const typename Traits::Address &address,
                                    typename Traits::GPRRegister shifter) {
  emitGenericShift(7, Ty, address, shifter);
}

template <class Machine>
void AssemblerX86Base<Machine>::shld(Type Ty, typename Traits::GPRRegister dst,
                                     typename Traits::GPRRegister src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  assert(Ty == IceType_i16 || Ty == IceType_i32);
  if (Ty == IceType_i16)
    emitOperandSizeOverride();
  emitRexRB(Ty, src, dst);
  emitUint8(0x0F);
  emitUint8(0xA5);
  emitRegisterOperand(gprEncoding(src), gprEncoding(dst));
}

template <class Machine>
void AssemblerX86Base<Machine>::shld(Type Ty, typename Traits::GPRRegister dst,
                                     typename Traits::GPRRegister src,
                                     const Immediate &imm) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  assert(Ty == IceType_i16 || Ty == IceType_i32);
  assert(imm.is_int8());
  if (Ty == IceType_i16)
    emitOperandSizeOverride();
  emitRexRB(Ty, src, dst);
  emitUint8(0x0F);
  emitUint8(0xA4);
  emitRegisterOperand(gprEncoding(src), gprEncoding(dst));
  emitUint8(imm.value() & 0xFF);
}

template <class Machine>
void AssemblerX86Base<Machine>::shld(Type Ty,
                                     const typename Traits::Address &operand,
                                     typename Traits::GPRRegister src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  assert(Ty == IceType_i16 || Ty == IceType_i32);
  if (Ty == IceType_i16)
    emitOperandSizeOverride();
  emitRex(Ty, operand, src);
  emitUint8(0x0F);
  emitUint8(0xA5);
  emitOperand(gprEncoding(src), operand);
}

template <class Machine>
void AssemblerX86Base<Machine>::shrd(Type Ty, typename Traits::GPRRegister dst,
                                     typename Traits::GPRRegister src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  assert(Ty == IceType_i16 || Ty == IceType_i32);
  if (Ty == IceType_i16)
    emitOperandSizeOverride();
  emitRexRB(Ty, src, dst);
  emitUint8(0x0F);
  emitUint8(0xAD);
  emitRegisterOperand(gprEncoding(src), gprEncoding(dst));
}

template <class Machine>
void AssemblerX86Base<Machine>::shrd(Type Ty, typename Traits::GPRRegister dst,
                                     typename Traits::GPRRegister src,
                                     const Immediate &imm) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  assert(Ty == IceType_i16 || Ty == IceType_i32);
  assert(imm.is_int8());
  if (Ty == IceType_i16)
    emitOperandSizeOverride();
  emitRexRB(Ty, src, dst);
  emitUint8(0x0F);
  emitUint8(0xAC);
  emitRegisterOperand(gprEncoding(src), gprEncoding(dst));
  emitUint8(imm.value() & 0xFF);
}

template <class Machine>
void AssemblerX86Base<Machine>::shrd(Type Ty,
                                     const typename Traits::Address &dst,
                                     typename Traits::GPRRegister src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  assert(Ty == IceType_i16 || Ty == IceType_i32);
  if (Ty == IceType_i16)
    emitOperandSizeOverride();
  emitRex(Ty, dst, src);
  emitUint8(0x0F);
  emitUint8(0xAD);
  emitOperand(gprEncoding(src), dst);
}

template <class Machine>
void AssemblerX86Base<Machine>::neg(Type Ty, typename Traits::GPRRegister reg) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  if (Ty == IceType_i16)
    emitOperandSizeOverride();
  emitRexB(Ty, reg);
  if (isByteSizedArithType(Ty))
    emitUint8(0xF6);
  else
    emitUint8(0xF7);
  emitRegisterOperand(3, gprEncoding(reg));
}

template <class Machine>
void AssemblerX86Base<Machine>::neg(Type Ty,
                                    const typename Traits::Address &addr) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  if (Ty == IceType_i16)
    emitOperandSizeOverride();
  emitRex(Ty, addr, RexRegIrrelevant);
  if (isByteSizedArithType(Ty))
    emitUint8(0xF6);
  else
    emitUint8(0xF7);
  emitOperand(3, addr);
}

template <class Machine>
void AssemblerX86Base<Machine>::notl(typename Traits::GPRRegister reg) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitRexB(IceType_i32, reg);
  emitUint8(0xF7);
  emitUint8(0xD0 | gprEncoding(reg));
}

template <class Machine>
void AssemblerX86Base<Machine>::bswap(Type Ty,
                                      typename Traits::GPRRegister reg) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  assert(Ty == IceType_i32);
  (void)Ty;
  emitRexB(Ty, reg);
  emitUint8(0x0F);
  emitUint8(0xC8 | gprEncoding(reg));
}

template <class Machine>
void AssemblerX86Base<Machine>::bsf(Type Ty, typename Traits::GPRRegister dst,
                                    typename Traits::GPRRegister src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  assert(Ty == IceType_i16 || Ty == IceType_i32);
  if (Ty == IceType_i16)
    emitOperandSizeOverride();
  emitRexRB(Ty, dst, src);
  emitUint8(0x0F);
  emitUint8(0xBC);
  emitRegisterOperand(gprEncoding(dst), gprEncoding(src));
}

template <class Machine>
void AssemblerX86Base<Machine>::bsf(Type Ty, typename Traits::GPRRegister dst,
                                    const typename Traits::Address &src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  assert(Ty == IceType_i16 || Ty == IceType_i32);
  if (Ty == IceType_i16)
    emitOperandSizeOverride();
  emitRex(Ty, src, dst);
  emitUint8(0x0F);
  emitUint8(0xBC);
  emitOperand(gprEncoding(dst), src);
}

template <class Machine>
void AssemblerX86Base<Machine>::bsr(Type Ty, typename Traits::GPRRegister dst,
                                    typename Traits::GPRRegister src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  assert(Ty == IceType_i16 || Ty == IceType_i32);
  if (Ty == IceType_i16)
    emitOperandSizeOverride();
  emitRexRB(Ty, dst, src);
  emitUint8(0x0F);
  emitUint8(0xBD);
  emitRegisterOperand(gprEncoding(dst), gprEncoding(src));
}

template <class Machine>
void AssemblerX86Base<Machine>::bsr(Type Ty, typename Traits::GPRRegister dst,
                                    const typename Traits::Address &src) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  assert(Ty == IceType_i16 || Ty == IceType_i32);
  if (Ty == IceType_i16)
    emitOperandSizeOverride();
  emitRex(Ty, src, dst);
  emitUint8(0x0F);
  emitUint8(0xBD);
  emitOperand(gprEncoding(dst), src);
}

template <class Machine>
void AssemblerX86Base<Machine>::bt(typename Traits::GPRRegister base,
                                   typename Traits::GPRRegister offset) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitRexRB(IceType_i32, offset, base);
  emitUint8(0x0F);
  emitUint8(0xA3);
  emitRegisterOperand(gprEncoding(offset), gprEncoding(base));
}

template <class Machine> void AssemblerX86Base<Machine>::ret() {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0xC3);
}

template <class Machine>
void AssemblerX86Base<Machine>::ret(const Immediate &imm) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0xC2);
  assert(imm.is_uint16());
  emitUint8(imm.value() & 0xFF);
  emitUint8((imm.value() >> 8) & 0xFF);
}

template <class Machine> void AssemblerX86Base<Machine>::nop(int size) {
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

template <class Machine> void AssemblerX86Base<Machine>::int3() {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0xCC);
}

template <class Machine> void AssemblerX86Base<Machine>::hlt() {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0xF4);
}

template <class Machine> void AssemblerX86Base<Machine>::ud2() {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0x0F);
  emitUint8(0x0B);
}

template <class Machine>
void AssemblerX86Base<Machine>::j(typename Traits::Cond::BrCond condition,
                                  Label *label, bool near) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  if (label->isBound()) {
    static const int kShortSize = 2;
    static const int kLongSize = 6;
    intptr_t offset = label->getPosition() - Buffer.size();
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

template <class Machine>
void AssemblerX86Base<Machine>::j(typename Traits::Cond::BrCond condition,
                                  const ConstantRelocatable *label) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0x0F);
  emitUint8(0x80 + condition);
  emitFixup(this->createFixup(Traits::PcRelFixup, label));
  emitInt32(-4);
}

template <class Machine>
void AssemblerX86Base<Machine>::jmp(typename Traits::GPRRegister reg) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitRexB(RexTypeIrrelevant, reg);
  emitUint8(0xFF);
  emitRegisterOperand(4, gprEncoding(reg));
}

template <class Machine>
void AssemblerX86Base<Machine>::jmp(Label *label, bool near) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  if (label->isBound()) {
    static const int kShortSize = 2;
    static const int kLongSize = 5;
    intptr_t offset = label->getPosition() - Buffer.size();
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

template <class Machine>
void AssemblerX86Base<Machine>::jmp(const ConstantRelocatable *label) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0xE9);
  emitFixup(this->createFixup(Traits::PcRelFixup, label));
  emitInt32(-4);
}

template <class Machine> void AssemblerX86Base<Machine>::mfence() {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0x0F);
  emitUint8(0xAE);
  emitUint8(0xF0);
}

template <class Machine> void AssemblerX86Base<Machine>::lock() {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(0xF0);
}

template <class Machine>
void AssemblerX86Base<Machine>::cmpxchg(Type Ty,
                                        const typename Traits::Address &address,
                                        typename Traits::GPRRegister reg,
                                        bool Locked) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  if (Ty == IceType_i16)
    emitOperandSizeOverride();
  if (Locked)
    emitUint8(0xF0);
  emitRex(Ty, address, reg);
  emitUint8(0x0F);
  if (isByteSizedArithType(Ty))
    emitUint8(0xB0);
  else
    emitUint8(0xB1);
  emitOperand(gprEncoding(reg), address);
}

template <class Machine>
void AssemblerX86Base<Machine>::cmpxchg8b(
    const typename Traits::Address &address, bool Locked) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  if (Locked)
    emitUint8(0xF0);
  emitRex(IceType_i32, address, RexRegIrrelevant);
  emitUint8(0x0F);
  emitUint8(0xC7);
  emitOperand(1, address);
}

template <class Machine>
void AssemblerX86Base<Machine>::xadd(Type Ty,
                                     const typename Traits::Address &addr,
                                     typename Traits::GPRRegister reg,
                                     bool Locked) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  if (Ty == IceType_i16)
    emitOperandSizeOverride();
  if (Locked)
    emitUint8(0xF0);
  emitRex(Ty, addr, reg);
  emitUint8(0x0F);
  if (isByteSizedArithType(Ty))
    emitUint8(0xC0);
  else
    emitUint8(0xC1);
  emitOperand(gprEncoding(reg), addr);
}

template <class Machine>
void AssemblerX86Base<Machine>::xchg(Type Ty,
                                     const typename Traits::Address &addr,
                                     typename Traits::GPRRegister reg) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  if (Ty == IceType_i16)
    emitOperandSizeOverride();
  emitRex(Ty, addr, reg);
  if (isByteSizedArithType(Ty))
    emitUint8(0x86);
  else
    emitUint8(0x87);
  emitOperand(gprEncoding(reg), addr);
}

template <class Machine>
void AssemblerX86Base<Machine>::emitSegmentOverride(uint8_t prefix) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  emitUint8(prefix);
}

template <class Machine>
void AssemblerX86Base<Machine>::align(intptr_t alignment, intptr_t offset) {
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

template <class Machine> void AssemblerX86Base<Machine>::bind(Label *label) {
  intptr_t bound = Buffer.size();
  assert(!label->isBound()); // Labels can only be bound once.
  while (label->isLinked()) {
    intptr_t position = label->getLinkPosition();
    intptr_t next = Buffer.load<int32_t>(position);
    Buffer.store<int32_t>(position, bound - (position + 4));
    label->Position = next;
  }
  while (label->hasNear()) {
    intptr_t position = label->getNearPosition();
    intptr_t offset = bound - (position + 1);
    assert(Utils::IsInt(8, offset));
    Buffer.store<int8_t>(position, offset);
  }
  label->bindTo(bound);
}

template <class Machine>
void AssemblerX86Base<Machine>::emitOperand(
    int rm, const typename Traits::Operand &operand) {
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

template <class Machine>
void AssemblerX86Base<Machine>::emitImmediate(Type Ty, const Immediate &imm) {
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

template <class Machine>
void AssemblerX86Base<Machine>::emitComplexI8(
    int rm, const typename Traits::Operand &operand,
    const Immediate &immediate) {
  assert(rm >= 0 && rm < 8);
  assert(immediate.is_int8());
  if (operand.IsRegister(Traits::Encoded_Reg_Accumulator)) {
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

template <class Machine>
void AssemblerX86Base<Machine>::emitComplex(
    Type Ty, int rm, const typename Traits::Operand &operand,
    const Immediate &immediate) {
  assert(rm >= 0 && rm < 8);
  if (immediate.is_int8()) {
    // Use sign-extended 8-bit immediate.
    emitUint8(0x83);
    emitOperand(rm, operand);
    emitUint8(immediate.value() & 0xFF);
  } else if (operand.IsRegister(Traits::Encoded_Reg_Accumulator)) {
    // Use short form if the destination is eax.
    emitUint8(0x05 + (rm << 3));
    emitImmediate(Ty, immediate);
  } else {
    emitUint8(0x81);
    emitOperand(rm, operand);
    emitImmediate(Ty, immediate);
  }
}

template <class Machine>
void AssemblerX86Base<Machine>::emitLabel(Label *label,
                                          intptr_t instruction_size) {
  if (label->isBound()) {
    intptr_t offset = label->getPosition() - Buffer.size();
    assert(offset <= 0);
    emitInt32(offset - instruction_size);
  } else {
    emitLabelLink(label);
  }
}

template <class Machine>
void AssemblerX86Base<Machine>::emitLabelLink(Label *Label) {
  assert(!Label->isBound());
  intptr_t Position = Buffer.size();
  emitInt32(Label->Position);
  if (!getPreliminary())
    Label->linkTo(Position);
}

template <class Machine>
void AssemblerX86Base<Machine>::emitNearLabelLink(Label *label) {
  assert(!label->isBound());
  intptr_t position = Buffer.size();
  emitUint8(0);
  if (!getPreliminary())
    label->nearLinkTo(position);
}

template <class Machine>
void AssemblerX86Base<Machine>::emitGenericShift(
    int rm, Type Ty, typename Traits::GPRRegister reg, const Immediate &imm) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  assert(imm.is_int8());
  if (Ty == IceType_i16)
    emitOperandSizeOverride();
  emitRexB(Ty, reg);
  if (imm.value() == 1) {
    emitUint8(isByteSizedArithType(Ty) ? 0xD0 : 0xD1);
    emitOperand(rm, typename Traits::Operand(reg));
  } else {
    emitUint8(isByteSizedArithType(Ty) ? 0xC0 : 0xC1);
    emitOperand(rm, typename Traits::Operand(reg));
    emitUint8(imm.value() & 0xFF);
  }
}

template <class Machine>
void AssemblerX86Base<Machine>::emitGenericShift(
    int rm, Type Ty, const typename Traits::Operand &operand,
    typename Traits::GPRRegister shifter) {
  AssemblerBuffer::EnsureCapacity ensured(&Buffer);
  assert(shifter == Traits::Encoded_Reg_Counter);
  (void)shifter;
  if (Ty == IceType_i16)
    emitOperandSizeOverride();
  emitRexB(Ty, operand.rm());
  emitUint8(isByteSizedArithType(Ty) ? 0xD2 : 0xD3);
  emitOperand(rm, operand);
}

} // end of namespace X86Internal
} // end of namespace Ice
