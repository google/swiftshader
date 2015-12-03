//===- subzero/src/IceAssemblerX86Base.h - base x86 assembler -*- C++ -*---===//
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
//
/// \file
/// \brief Defines the AssemblerX86 template class for x86, the base of all X86
/// assemblers.
//
//===----------------------------------------------------------------------===//

#ifndef SUBZERO_SRC_ICEASSEMBLERX86BASE_H
#define SUBZERO_SRC_ICEASSEMBLERX86BASE_H

#include "IceAssembler.h"
#include "IceDefs.h"
#include "IceOperand.h"
#include "IceTypes.h"
#include "IceUtils.h"

namespace Ice {

namespace X86Internal {

template <class Machine> class AssemblerX86Base;
template <class Machine> struct MachineTraits;

constexpr int MAX_NOP_SIZE = 8;

class Immediate {
  Immediate(const Immediate &) = delete;
  Immediate &operator=(const Immediate &) = delete;

public:
  explicit Immediate(int32_t value) : value_(value) {}

  Immediate(RelocOffsetT offset, AssemblerFixup *fixup)
      : value_(offset), fixup_(fixup) {
    // Use the Offset in the "value" for now. If we decide to process fixups,
    // we'll need to patch that offset with the true value.
  }

  int32_t value() const { return value_; }
  AssemblerFixup *fixup() const { return fixup_; }

  bool is_int8() const {
    // We currently only allow 32-bit fixups, and they usually have value = 0,
    // so if fixup_ != nullptr, it shouldn't be classified as int8/16.
    return fixup_ == nullptr && Utils::IsInt(8, value_);
  }
  bool is_uint8() const {
    return fixup_ == nullptr && Utils::IsUint(8, value_);
  }
  bool is_uint16() const {
    return fixup_ == nullptr && Utils::IsUint(16, value_);
  }

private:
  const int32_t value_;
  AssemblerFixup *fixup_ = nullptr;
};

/// X86 allows near and far jumps.
class Label final : public Ice::Label {
  Label(const Label &) = delete;
  Label &operator=(const Label &) = delete;

public:
  Label() = default;
  ~Label() = default;

  void finalCheck() const override {
    Ice::Label::finalCheck();
    assert(!hasNear());
  }

  /// Returns the position of an earlier branch instruction which assumes that
  /// this label is "near", and bumps iterator to the next near position.
  intptr_t getNearPosition() {
    assert(hasNear());
    intptr_t Pos = UnresolvedNearPositions.back();
    UnresolvedNearPositions.pop_back();
    return Pos;
  }

  bool hasNear() const { return !UnresolvedNearPositions.empty(); }
  bool isUnused() const override {
    return Ice::Label::isUnused() && !hasNear();
  }

private:
  void nearLinkTo(intptr_t position) {
    assert(!isBound());
    UnresolvedNearPositions.push_back(position);
  }

  llvm::SmallVector<intptr_t, 20> UnresolvedNearPositions;

  template <class> friend class AssemblerX86Base;
};

template <class Machine> class AssemblerX86Base : public Assembler {
  AssemblerX86Base(const AssemblerX86Base &) = delete;
  AssemblerX86Base &operator=(const AssemblerX86Base &) = delete;

protected:
  AssemblerX86Base(AssemblerKind Kind, bool use_far_branches)
      : Assembler(Kind) {
    // This mode is only needed and implemented for MIPS and ARM.
    assert(!use_far_branches);
    (void)use_far_branches;
  }

public:
  using Traits = MachineTraits<Machine>;

  ~AssemblerX86Base() override;

  static const bool kNearJump = true;
  static const bool kFarJump = false;

  void alignFunction() override;

  SizeT getBundleAlignLog2Bytes() const override { return 5; }

  const char *getAlignDirective() const override { return ".p2align"; }

  llvm::ArrayRef<uint8_t> getNonExecBundlePadding() const override {
    static const uint8_t Padding[] = {0xF4};
    return llvm::ArrayRef<uint8_t>(Padding, 1);
  }

  void padWithNop(intptr_t Padding) override {
    while (Padding > MAX_NOP_SIZE) {
      nop(MAX_NOP_SIZE);
      Padding -= MAX_NOP_SIZE;
    }
    if (Padding)
      nop(Padding);
  }

  Ice::Label *getCfgNodeLabel(SizeT NodeNumber) override;
  void bindCfgNodeLabel(const CfgNode *Node) override;
  Label *getOrCreateCfgNodeLabel(SizeT Number);
  Label *getOrCreateLocalLabel(SizeT Number);
  void bindLocalLabel(SizeT Number);

  bool fixupIsPCRel(FixupKind Kind) const override {
    // Currently assuming this is the only PC-rel relocation type used.
    // TODO(jpp): Traits.PcRelTypes.count(Kind) != 0
    return Kind == Traits::PcRelFixup;
  }

  // Operations to emit GPR instructions (and dispatch on operand type).
  using TypedEmitGPR = void (AssemblerX86Base::*)(Type,
                                                  typename Traits::GPRRegister);
  using TypedEmitAddr =
      void (AssemblerX86Base::*)(Type, const typename Traits::Address &);
  struct GPREmitterOneOp {
    TypedEmitGPR Reg;
    TypedEmitAddr Addr;
  };

  using TypedEmitGPRGPR = void (AssemblerX86Base::*)(
      Type, typename Traits::GPRRegister, typename Traits::GPRRegister);
  using TypedEmitGPRAddr = void (AssemblerX86Base::*)(
      Type, typename Traits::GPRRegister, const typename Traits::Address &);
  using TypedEmitGPRImm = void (AssemblerX86Base::*)(
      Type, typename Traits::GPRRegister, const Immediate &);
  struct GPREmitterRegOp {
    TypedEmitGPRGPR GPRGPR;
    TypedEmitGPRAddr GPRAddr;
    TypedEmitGPRImm GPRImm;
  };

  struct GPREmitterShiftOp {
    // Technically, Addr/GPR and Addr/Imm are also allowed, but */Addr are not.
    // In practice, we always normalize the Dest to a Register first.
    TypedEmitGPRGPR GPRGPR;
    TypedEmitGPRImm GPRImm;
  };

  using TypedEmitGPRGPRImm = void (AssemblerX86Base::*)(
      Type, typename Traits::GPRRegister, typename Traits::GPRRegister,
      const Immediate &);
  struct GPREmitterShiftD {
    // Technically AddrGPR and AddrGPRImm are also allowed, but in practice we
    // always normalize Dest to a Register first.
    TypedEmitGPRGPR GPRGPR;
    TypedEmitGPRGPRImm GPRGPRImm;
  };

  using TypedEmitAddrGPR = void (AssemblerX86Base::*)(
      Type, const typename Traits::Address &, typename Traits::GPRRegister);
  using TypedEmitAddrImm = void (AssemblerX86Base::*)(
      Type, const typename Traits::Address &, const Immediate &);
  struct GPREmitterAddrOp {
    TypedEmitAddrGPR AddrGPR;
    TypedEmitAddrImm AddrImm;
  };

  // Operations to emit XMM instructions (and dispatch on operand type).
  using TypedEmitXmmXmm = void (AssemblerX86Base::*)(
      Type, typename Traits::XmmRegister, typename Traits::XmmRegister);
  using TypedEmitXmmAddr = void (AssemblerX86Base::*)(
      Type, typename Traits::XmmRegister, const typename Traits::Address &);
  struct XmmEmitterRegOp {
    TypedEmitXmmXmm XmmXmm;
    TypedEmitXmmAddr XmmAddr;
  };

  using EmitXmmXmm = void (AssemblerX86Base::*)(typename Traits::XmmRegister,
                                                typename Traits::XmmRegister);
  using EmitXmmAddr = void (AssemblerX86Base::*)(
      typename Traits::XmmRegister, const typename Traits::Address &);
  using EmitAddrXmm = void (AssemblerX86Base::*)(
      const typename Traits::Address &, typename Traits::XmmRegister);
  struct XmmEmitterMovOps {
    EmitXmmXmm XmmXmm;
    EmitXmmAddr XmmAddr;
    EmitAddrXmm AddrXmm;
  };

  using TypedEmitXmmImm = void (AssemblerX86Base::*)(
      Type, typename Traits::XmmRegister, const Immediate &);

  struct XmmEmitterShiftOp {
    TypedEmitXmmXmm XmmXmm;
    TypedEmitXmmAddr XmmAddr;
    TypedEmitXmmImm XmmImm;
  };

  // Cross Xmm/GPR cast instructions.
  template <typename DReg_t, typename SReg_t> struct CastEmitterRegOp {
    using TypedEmitRegs = void (AssemblerX86Base::*)(Type, DReg_t, Type,
                                                     SReg_t);
    using TypedEmitAddr = void (AssemblerX86Base::*)(
        Type, DReg_t, Type, const typename Traits::Address &);

    TypedEmitRegs RegReg;
    TypedEmitAddr RegAddr;
  };

  // Three operand (potentially) cross Xmm/GPR instructions. The last operand
  // must be an immediate.
  template <typename DReg_t, typename SReg_t> struct ThreeOpImmEmitter {
    using TypedEmitRegRegImm = void (AssemblerX86Base::*)(Type, DReg_t, SReg_t,
                                                          const Immediate &);
    using TypedEmitRegAddrImm = void (AssemblerX86Base::*)(
        Type, DReg_t, const typename Traits::Address &, const Immediate &);

    TypedEmitRegRegImm RegRegImm;
    TypedEmitRegAddrImm RegAddrImm;
  };

  /*
   * Emit Machine Instructions.
   */
  void call(typename Traits::GPRRegister reg);
  void call(const typename Traits::Address &address);
  void call(const ConstantRelocatable *label); // not testable.
  void call(const Immediate &abs_address);

  static const intptr_t kCallExternalLabelSize = 5;

  void pushl(typename Traits::GPRRegister reg);

  void popl(typename Traits::GPRRegister reg);
  void popl(const typename Traits::Address &address);

  template <typename T = Traits,
            typename = typename std::enable_if<T::HasPusha>::type>
  void pushal();
  template <typename T = Traits,
            typename = typename std::enable_if<T::HasPopa>::type>
  void popal();

  void setcc(typename Traits::Cond::BrCond condition,
             typename Traits::ByteRegister dst);
  void setcc(typename Traits::Cond::BrCond condition,
             const typename Traits::Address &address);

  void mov(Type Ty, typename Traits::GPRRegister dst, const Immediate &src);
  void mov(Type Ty, typename Traits::GPRRegister dst,
           typename Traits::GPRRegister src);
  void mov(Type Ty, typename Traits::GPRRegister dst,
           const typename Traits::Address &src);
  void mov(Type Ty, const typename Traits::Address &dst,
           typename Traits::GPRRegister src);
  void mov(Type Ty, const typename Traits::Address &dst, const Immediate &imm);

  template <typename T = Traits>
  typename std::enable_if<T::Is64Bit, void>::type
  movabs(const typename Traits::GPRRegister Dst, uint64_t Imm64);
  template <typename T = Traits>
  typename std::enable_if<!T::Is64Bit, void>::type
  movabs(const typename Traits::GPRRegister, uint64_t) {
    llvm::report_fatal_error("movabs is only supported in 64-bit x86 targets.");
  }

  void movzx(Type Ty, typename Traits::GPRRegister dst,
             typename Traits::GPRRegister src);
  void movzx(Type Ty, typename Traits::GPRRegister dst,
             const typename Traits::Address &src);
  void movsx(Type Ty, typename Traits::GPRRegister dst,
             typename Traits::GPRRegister src);
  void movsx(Type Ty, typename Traits::GPRRegister dst,
             const typename Traits::Address &src);

  void lea(Type Ty, typename Traits::GPRRegister dst,
           const typename Traits::Address &src);

  void cmov(Type Ty, typename Traits::Cond::BrCond cond,
            typename Traits::GPRRegister dst, typename Traits::GPRRegister src);
  void cmov(Type Ty, typename Traits::Cond::BrCond cond,
            typename Traits::GPRRegister dst,
            const typename Traits::Address &src);

  void rep_movsb();

  void movss(Type Ty, typename Traits::XmmRegister dst,
             const typename Traits::Address &src);
  void movss(Type Ty, const typename Traits::Address &dst,
             typename Traits::XmmRegister src);
  void movss(Type Ty, typename Traits::XmmRegister dst,
             typename Traits::XmmRegister src);

  void movd(Type SrcTy, typename Traits::XmmRegister dst,
            typename Traits::GPRRegister src);
  void movd(Type SrcTy, typename Traits::XmmRegister dst,
            const typename Traits::Address &src);
  void movd(Type DestTy, typename Traits::GPRRegister dst,
            typename Traits::XmmRegister src);
  void movd(Type DestTy, const typename Traits::Address &dst,
            typename Traits::XmmRegister src);

  void movq(typename Traits::XmmRegister dst, typename Traits::XmmRegister src);
  void movq(const typename Traits::Address &dst,
            typename Traits::XmmRegister src);
  void movq(typename Traits::XmmRegister dst,
            const typename Traits::Address &src);

  void addss(Type Ty, typename Traits::XmmRegister dst,
             typename Traits::XmmRegister src);
  void addss(Type Ty, typename Traits::XmmRegister dst,
             const typename Traits::Address &src);
  void subss(Type Ty, typename Traits::XmmRegister dst,
             typename Traits::XmmRegister src);
  void subss(Type Ty, typename Traits::XmmRegister dst,
             const typename Traits::Address &src);
  void mulss(Type Ty, typename Traits::XmmRegister dst,
             typename Traits::XmmRegister src);
  void mulss(Type Ty, typename Traits::XmmRegister dst,
             const typename Traits::Address &src);
  void divss(Type Ty, typename Traits::XmmRegister dst,
             typename Traits::XmmRegister src);
  void divss(Type Ty, typename Traits::XmmRegister dst,
             const typename Traits::Address &src);

  void movaps(typename Traits::XmmRegister dst,
              typename Traits::XmmRegister src);

  void movups(typename Traits::XmmRegister dst,
              typename Traits::XmmRegister src);
  void movups(typename Traits::XmmRegister dst,
              const typename Traits::Address &src);
  void movups(const typename Traits::Address &dst,
              typename Traits::XmmRegister src);

  void padd(Type Ty, typename Traits::XmmRegister dst,
            typename Traits::XmmRegister src);
  void padd(Type Ty, typename Traits::XmmRegister dst,
            const typename Traits::Address &src);
  void pand(Type Ty, typename Traits::XmmRegister dst,
            typename Traits::XmmRegister src);
  void pand(Type Ty, typename Traits::XmmRegister dst,
            const typename Traits::Address &src);
  void pandn(Type Ty, typename Traits::XmmRegister dst,
             typename Traits::XmmRegister src);
  void pandn(Type Ty, typename Traits::XmmRegister dst,
             const typename Traits::Address &src);
  void pmull(Type Ty, typename Traits::XmmRegister dst,
             typename Traits::XmmRegister src);
  void pmull(Type Ty, typename Traits::XmmRegister dst,
             const typename Traits::Address &src);
  void pmuludq(Type Ty, typename Traits::XmmRegister dst,
               typename Traits::XmmRegister src);
  void pmuludq(Type Ty, typename Traits::XmmRegister dst,
               const typename Traits::Address &src);
  void por(Type Ty, typename Traits::XmmRegister dst,
           typename Traits::XmmRegister src);
  void por(Type Ty, typename Traits::XmmRegister dst,
           const typename Traits::Address &src);
  void psub(Type Ty, typename Traits::XmmRegister dst,
            typename Traits::XmmRegister src);
  void psub(Type Ty, typename Traits::XmmRegister dst,
            const typename Traits::Address &src);
  void pxor(Type Ty, typename Traits::XmmRegister dst,
            typename Traits::XmmRegister src);
  void pxor(Type Ty, typename Traits::XmmRegister dst,
            const typename Traits::Address &src);

  void psll(Type Ty, typename Traits::XmmRegister dst,
            typename Traits::XmmRegister src);
  void psll(Type Ty, typename Traits::XmmRegister dst,
            const typename Traits::Address &src);
  void psll(Type Ty, typename Traits::XmmRegister dst, const Immediate &src);

  void psra(Type Ty, typename Traits::XmmRegister dst,
            typename Traits::XmmRegister src);
  void psra(Type Ty, typename Traits::XmmRegister dst,
            const typename Traits::Address &src);
  void psra(Type Ty, typename Traits::XmmRegister dst, const Immediate &src);
  void psrl(Type Ty, typename Traits::XmmRegister dst,
            typename Traits::XmmRegister src);
  void psrl(Type Ty, typename Traits::XmmRegister dst,
            const typename Traits::Address &src);
  void psrl(Type Ty, typename Traits::XmmRegister dst, const Immediate &src);

  void addps(Type Ty, typename Traits::XmmRegister dst,
             typename Traits::XmmRegister src);
  void addps(Type Ty, typename Traits::XmmRegister dst,
             const typename Traits::Address &src);
  void subps(Type Ty, typename Traits::XmmRegister dst,
             typename Traits::XmmRegister src);
  void subps(Type Ty, typename Traits::XmmRegister dst,
             const typename Traits::Address &src);
  void divps(Type Ty, typename Traits::XmmRegister dst,
             typename Traits::XmmRegister src);
  void divps(Type Ty, typename Traits::XmmRegister dst,
             const typename Traits::Address &src);
  void mulps(Type Ty, typename Traits::XmmRegister dst,
             typename Traits::XmmRegister src);
  void mulps(Type Ty, typename Traits::XmmRegister dst,
             const typename Traits::Address &src);
  void minps(typename Traits::XmmRegister dst,
             typename Traits::XmmRegister src);
  void maxps(typename Traits::XmmRegister dst,
             typename Traits::XmmRegister src);
  void andps(typename Traits::XmmRegister dst,
             typename Traits::XmmRegister src);
  void andps(typename Traits::XmmRegister dst,
             const typename Traits::Address &src);
  void orps(typename Traits::XmmRegister dst, typename Traits::XmmRegister src);

  void blendvps(Type Ty, typename Traits::XmmRegister dst,
                typename Traits::XmmRegister src);
  void blendvps(Type Ty, typename Traits::XmmRegister dst,
                const typename Traits::Address &src);
  void pblendvb(Type Ty, typename Traits::XmmRegister dst,
                typename Traits::XmmRegister src);
  void pblendvb(Type Ty, typename Traits::XmmRegister dst,
                const typename Traits::Address &src);

  void cmpps(typename Traits::XmmRegister dst, typename Traits::XmmRegister src,
             typename Traits::Cond::CmppsCond CmpCondition);
  void cmpps(typename Traits::XmmRegister dst,
             const typename Traits::Address &src,
             typename Traits::Cond::CmppsCond CmpCondition);

  void sqrtps(typename Traits::XmmRegister dst);
  void rsqrtps(typename Traits::XmmRegister dst);
  void reciprocalps(typename Traits::XmmRegister dst);

  void movhlps(typename Traits::XmmRegister dst,
               typename Traits::XmmRegister src);
  void movlhps(typename Traits::XmmRegister dst,
               typename Traits::XmmRegister src);
  void unpcklps(typename Traits::XmmRegister dst,
                typename Traits::XmmRegister src);
  void unpckhps(typename Traits::XmmRegister dst,
                typename Traits::XmmRegister src);
  void unpcklpd(typename Traits::XmmRegister dst,
                typename Traits::XmmRegister src);
  void unpckhpd(typename Traits::XmmRegister dst,
                typename Traits::XmmRegister src);

  void set1ps(typename Traits::XmmRegister dst,
              typename Traits::GPRRegister tmp, const Immediate &imm);

  void minpd(typename Traits::XmmRegister dst,
             typename Traits::XmmRegister src);
  void maxpd(typename Traits::XmmRegister dst,
             typename Traits::XmmRegister src);
  void sqrtpd(typename Traits::XmmRegister dst);

  void pshufd(Type Ty, typename Traits::XmmRegister dst,
              typename Traits::XmmRegister src, const Immediate &mask);
  void pshufd(Type Ty, typename Traits::XmmRegister dst,
              const typename Traits::Address &src, const Immediate &mask);
  void shufps(Type Ty, typename Traits::XmmRegister dst,
              typename Traits::XmmRegister src, const Immediate &mask);
  void shufps(Type Ty, typename Traits::XmmRegister dst,
              const typename Traits::Address &src, const Immediate &mask);

  void cvtdq2ps(Type, typename Traits::XmmRegister dst,
                typename Traits::XmmRegister src);
  void cvtdq2ps(Type, typename Traits::XmmRegister dst,
                const typename Traits::Address &src);

  void cvttps2dq(Type, typename Traits::XmmRegister dst,
                 typename Traits::XmmRegister src);
  void cvttps2dq(Type, typename Traits::XmmRegister dst,
                 const typename Traits::Address &src);

  void cvtsi2ss(Type DestTy, typename Traits::XmmRegister dst, Type SrcTy,
                typename Traits::GPRRegister src);
  void cvtsi2ss(Type DestTy, typename Traits::XmmRegister dst, Type SrcTy,
                const typename Traits::Address &src);

  void cvtfloat2float(Type SrcTy, typename Traits::XmmRegister dst,
                      typename Traits::XmmRegister src);
  void cvtfloat2float(Type SrcTy, typename Traits::XmmRegister dst,
                      const typename Traits::Address &src);

  void cvttss2si(Type DestTy, typename Traits::GPRRegister dst, Type SrcTy,
                 typename Traits::XmmRegister src);
  void cvttss2si(Type DestTy, typename Traits::GPRRegister dst, Type SrcTy,
                 const typename Traits::Address &src);

  void ucomiss(Type Ty, typename Traits::XmmRegister a,
               typename Traits::XmmRegister b);
  void ucomiss(Type Ty, typename Traits::XmmRegister a,
               const typename Traits::Address &b);

  void movmskpd(typename Traits::GPRRegister dst,
                typename Traits::XmmRegister src);
  void movmskps(typename Traits::GPRRegister dst,
                typename Traits::XmmRegister src);

  void sqrtss(Type Ty, typename Traits::XmmRegister dst,
              const typename Traits::Address &src);
  void sqrtss(Type Ty, typename Traits::XmmRegister dst,
              typename Traits::XmmRegister src);

  void xorpd(typename Traits::XmmRegister dst,
             const typename Traits::Address &src);
  void xorpd(typename Traits::XmmRegister dst,
             typename Traits::XmmRegister src);
  void xorps(typename Traits::XmmRegister dst,
             const typename Traits::Address &src);
  void xorps(typename Traits::XmmRegister dst,
             typename Traits::XmmRegister src);

  void andpd(typename Traits::XmmRegister dst,
             const typename Traits::Address &src);
  void andpd(typename Traits::XmmRegister dst,
             typename Traits::XmmRegister src);

  void orpd(typename Traits::XmmRegister dst, typename Traits::XmmRegister src);

  void insertps(Type Ty, typename Traits::XmmRegister dst,
                typename Traits::XmmRegister src, const Immediate &imm);
  void insertps(Type Ty, typename Traits::XmmRegister dst,
                const typename Traits::Address &src, const Immediate &imm);

  void pinsr(Type Ty, typename Traits::XmmRegister dst,
             typename Traits::GPRRegister src, const Immediate &imm);
  void pinsr(Type Ty, typename Traits::XmmRegister dst,
             const typename Traits::Address &src, const Immediate &imm);

  void pextr(Type Ty, typename Traits::GPRRegister dst,
             typename Traits::XmmRegister src, const Immediate &imm);

  void pmovsxdq(typename Traits::XmmRegister dst,
                typename Traits::XmmRegister src);

  void pcmpeq(Type Ty, typename Traits::XmmRegister dst,
              typename Traits::XmmRegister src);
  void pcmpeq(Type Ty, typename Traits::XmmRegister dst,
              const typename Traits::Address &src);
  void pcmpgt(Type Ty, typename Traits::XmmRegister dst,
              typename Traits::XmmRegister src);
  void pcmpgt(Type Ty, typename Traits::XmmRegister dst,
              const typename Traits::Address &src);

  enum RoundingMode {
    kRoundToNearest = 0x0,
    kRoundDown = 0x1,
    kRoundUp = 0x2,
    kRoundToZero = 0x3
  };
  void roundsd(typename Traits::XmmRegister dst,
               typename Traits::XmmRegister src, RoundingMode mode);

  //----------------------------------------------------------------------------
  //
  // Begin: X87 instructions. Only available when Traits::UsesX87.
  //
  //----------------------------------------------------------------------------
  template <typename T = Traits,
            typename = typename std::enable_if<T::UsesX87>::type>
  void fld(Type Ty, const typename T::Address &src);
  template <typename T = Traits,
            typename = typename std::enable_if<T::UsesX87>::type>
  void fstp(Type Ty, const typename T::Address &dst);
  template <typename T = Traits,
            typename = typename std::enable_if<T::UsesX87>::type>
  void fstp(typename T::X87STRegister st);

  template <typename T = Traits,
            typename = typename std::enable_if<T::UsesX87>::type>
  void fnstcw(const typename T::Address &dst);
  template <typename T = Traits,
            typename = typename std::enable_if<T::UsesX87>::type>
  void fldcw(const typename T::Address &src);

  template <typename T = Traits,
            typename = typename std::enable_if<T::UsesX87>::type>
  void fistpl(const typename T::Address &dst);
  template <typename T = Traits,
            typename = typename std::enable_if<T::UsesX87>::type>
  void fistps(const typename T::Address &dst);
  template <typename T = Traits,
            typename = typename std::enable_if<T::UsesX87>::type>
  void fildl(const typename T::Address &src);
  template <typename T = Traits,
            typename = typename std::enable_if<T::UsesX87>::type>
  void filds(const typename T::Address &src);

  template <typename T = Traits,
            typename = typename std::enable_if<T::UsesX87>::type>
  void fincstp();
  //----------------------------------------------------------------------------
  //
  // End: X87 instructions.
  //
  //----------------------------------------------------------------------------

  void cmp(Type Ty, typename Traits::GPRRegister reg0,
           typename Traits::GPRRegister reg1);
  void cmp(Type Ty, typename Traits::GPRRegister reg,
           const typename Traits::Address &address);
  void cmp(Type Ty, typename Traits::GPRRegister reg, const Immediate &imm);
  void cmp(Type Ty, const typename Traits::Address &address,
           typename Traits::GPRRegister reg);
  void cmp(Type Ty, const typename Traits::Address &address,
           const Immediate &imm);

  void test(Type Ty, typename Traits::GPRRegister reg0,
            typename Traits::GPRRegister reg1);
  void test(Type Ty, typename Traits::GPRRegister reg, const Immediate &imm);
  void test(Type Ty, const typename Traits::Address &address,
            typename Traits::GPRRegister reg);
  void test(Type Ty, const typename Traits::Address &address,
            const Immediate &imm);

  void And(Type Ty, typename Traits::GPRRegister dst,
           typename Traits::GPRRegister src);
  void And(Type Ty, typename Traits::GPRRegister dst,
           const typename Traits::Address &address);
  void And(Type Ty, typename Traits::GPRRegister dst, const Immediate &imm);
  void And(Type Ty, const typename Traits::Address &address,
           typename Traits::GPRRegister reg);
  void And(Type Ty, const typename Traits::Address &address,
           const Immediate &imm);

  void Or(Type Ty, typename Traits::GPRRegister dst,
          typename Traits::GPRRegister src);
  void Or(Type Ty, typename Traits::GPRRegister dst,
          const typename Traits::Address &address);
  void Or(Type Ty, typename Traits::GPRRegister dst, const Immediate &imm);
  void Or(Type Ty, const typename Traits::Address &address,
          typename Traits::GPRRegister reg);
  void Or(Type Ty, const typename Traits::Address &address,
          const Immediate &imm);

  void Xor(Type Ty, typename Traits::GPRRegister dst,
           typename Traits::GPRRegister src);
  void Xor(Type Ty, typename Traits::GPRRegister dst,
           const typename Traits::Address &address);
  void Xor(Type Ty, typename Traits::GPRRegister dst, const Immediate &imm);
  void Xor(Type Ty, const typename Traits::Address &address,
           typename Traits::GPRRegister reg);
  void Xor(Type Ty, const typename Traits::Address &address,
           const Immediate &imm);

  void add(Type Ty, typename Traits::GPRRegister dst,
           typename Traits::GPRRegister src);
  void add(Type Ty, typename Traits::GPRRegister reg,
           const typename Traits::Address &address);
  void add(Type Ty, typename Traits::GPRRegister reg, const Immediate &imm);
  void add(Type Ty, const typename Traits::Address &address,
           typename Traits::GPRRegister reg);
  void add(Type Ty, const typename Traits::Address &address,
           const Immediate &imm);

  void adc(Type Ty, typename Traits::GPRRegister dst,
           typename Traits::GPRRegister src);
  void adc(Type Ty, typename Traits::GPRRegister dst,
           const typename Traits::Address &address);
  void adc(Type Ty, typename Traits::GPRRegister reg, const Immediate &imm);
  void adc(Type Ty, const typename Traits::Address &address,
           typename Traits::GPRRegister reg);
  void adc(Type Ty, const typename Traits::Address &address,
           const Immediate &imm);

  void sub(Type Ty, typename Traits::GPRRegister dst,
           typename Traits::GPRRegister src);
  void sub(Type Ty, typename Traits::GPRRegister reg,
           const typename Traits::Address &address);
  void sub(Type Ty, typename Traits::GPRRegister reg, const Immediate &imm);
  void sub(Type Ty, const typename Traits::Address &address,
           typename Traits::GPRRegister reg);
  void sub(Type Ty, const typename Traits::Address &address,
           const Immediate &imm);

  void sbb(Type Ty, typename Traits::GPRRegister dst,
           typename Traits::GPRRegister src);
  void sbb(Type Ty, typename Traits::GPRRegister reg,
           const typename Traits::Address &address);
  void sbb(Type Ty, typename Traits::GPRRegister reg, const Immediate &imm);
  void sbb(Type Ty, const typename Traits::Address &address,
           typename Traits::GPRRegister reg);
  void sbb(Type Ty, const typename Traits::Address &address,
           const Immediate &imm);

  void cbw();
  void cwd();
  void cdq();
  template <typename T = Traits>
  typename std::enable_if<T::Is64Bit, void>::type cqo();
  template <typename T = Traits>
  typename std::enable_if<!T::Is64Bit, void>::type cqo() {
    llvm::report_fatal_error("CQO is only available in 64-bit x86 backends.");
  }

  void div(Type Ty, typename Traits::GPRRegister reg);
  void div(Type Ty, const typename Traits::Address &address);

  void idiv(Type Ty, typename Traits::GPRRegister reg);
  void idiv(Type Ty, const typename Traits::Address &address);

  void imul(Type Ty, typename Traits::GPRRegister dst,
            typename Traits::GPRRegister src);
  void imul(Type Ty, typename Traits::GPRRegister reg, const Immediate &imm);
  void imul(Type Ty, typename Traits::GPRRegister reg,
            const typename Traits::Address &address);

  void imul(Type Ty, typename Traits::GPRRegister reg);
  void imul(Type Ty, const typename Traits::Address &address);

  void imul(Type Ty, typename Traits::GPRRegister dst,
            typename Traits::GPRRegister src, const Immediate &imm);
  void imul(Type Ty, typename Traits::GPRRegister dst,
            const typename Traits::Address &address, const Immediate &imm);

  void mul(Type Ty, typename Traits::GPRRegister reg);
  void mul(Type Ty, const typename Traits::Address &address);

  template <class T = Traits,
            typename = typename std::enable_if<!T::Is64Bit>::type>
  void incl(typename Traits::GPRRegister reg);
  void incl(const typename Traits::Address &address);

  template <class T = Traits,
            typename = typename std::enable_if<!T::Is64Bit>::type>
  void decl(typename Traits::GPRRegister reg);
  void decl(const typename Traits::Address &address);

  void rol(Type Ty, typename Traits::GPRRegister reg, const Immediate &imm);
  void rol(Type Ty, typename Traits::GPRRegister operand,
           typename Traits::GPRRegister shifter);
  void rol(Type Ty, const typename Traits::Address &operand,
           typename Traits::GPRRegister shifter);

  void shl(Type Ty, typename Traits::GPRRegister reg, const Immediate &imm);
  void shl(Type Ty, typename Traits::GPRRegister operand,
           typename Traits::GPRRegister shifter);
  void shl(Type Ty, const typename Traits::Address &operand,
           typename Traits::GPRRegister shifter);

  void shr(Type Ty, typename Traits::GPRRegister reg, const Immediate &imm);
  void shr(Type Ty, typename Traits::GPRRegister operand,
           typename Traits::GPRRegister shifter);
  void shr(Type Ty, const typename Traits::Address &operand,
           typename Traits::GPRRegister shifter);

  void sar(Type Ty, typename Traits::GPRRegister reg, const Immediate &imm);
  void sar(Type Ty, typename Traits::GPRRegister operand,
           typename Traits::GPRRegister shifter);
  void sar(Type Ty, const typename Traits::Address &address,
           typename Traits::GPRRegister shifter);

  void shld(Type Ty, typename Traits::GPRRegister dst,
            typename Traits::GPRRegister src);
  void shld(Type Ty, typename Traits::GPRRegister dst,
            typename Traits::GPRRegister src, const Immediate &imm);
  void shld(Type Ty, const typename Traits::Address &operand,
            typename Traits::GPRRegister src);
  void shrd(Type Ty, typename Traits::GPRRegister dst,
            typename Traits::GPRRegister src);
  void shrd(Type Ty, typename Traits::GPRRegister dst,
            typename Traits::GPRRegister src, const Immediate &imm);
  void shrd(Type Ty, const typename Traits::Address &dst,
            typename Traits::GPRRegister src);

  void neg(Type Ty, typename Traits::GPRRegister reg);
  void neg(Type Ty, const typename Traits::Address &addr);
  void notl(typename Traits::GPRRegister reg);

  void bsf(Type Ty, typename Traits::GPRRegister dst,
           typename Traits::GPRRegister src);
  void bsf(Type Ty, typename Traits::GPRRegister dst,
           const typename Traits::Address &src);
  void bsr(Type Ty, typename Traits::GPRRegister dst,
           typename Traits::GPRRegister src);
  void bsr(Type Ty, typename Traits::GPRRegister dst,
           const typename Traits::Address &src);

  void bswap(Type Ty, typename Traits::GPRRegister reg);

  void bt(typename Traits::GPRRegister base,
          typename Traits::GPRRegister offset);

  void ret();
  void ret(const Immediate &imm);

  // 'size' indicates size in bytes and must be in the range 1..8.
  void nop(int size = 1);
  void int3();
  void hlt();
  void ud2();

  // j(Label) is fully tested.
  void j(typename Traits::Cond::BrCond condition, Label *label,
         bool near = kFarJump);
  void j(typename Traits::Cond::BrCond condition,
         const ConstantRelocatable *label); // not testable.

  void jmp(typename Traits::GPRRegister reg);
  void jmp(Label *label, bool near = kFarJump);
  void jmp(const ConstantRelocatable *label); // not testable.

  void mfence();

  void lock();
  void cmpxchg(Type Ty, const typename Traits::Address &address,
               typename Traits::GPRRegister reg, bool Locked);
  void cmpxchg8b(const typename Traits::Address &address, bool Locked);
  void xadd(Type Ty, const typename Traits::Address &address,
            typename Traits::GPRRegister reg, bool Locked);
  void xchg(Type Ty, typename Traits::GPRRegister reg0,
            typename Traits::GPRRegister reg1);
  void xchg(Type Ty, const typename Traits::Address &address,
            typename Traits::GPRRegister reg);

  /// \name Intel Architecture Code Analyzer markers.
  /// @{
  void iaca_start();
  void iaca_end();
  /// @}

  void emitSegmentOverride(uint8_t prefix);

  intptr_t preferredLoopAlignment() { return 16; }
  void align(intptr_t alignment, intptr_t offset);
  void bind(Label *label);

  intptr_t CodeSize() const { return Buffer.size(); }

protected:
  inline void emitUint8(uint8_t value);

private:
  static constexpr Type RexTypeIrrelevant = IceType_i32;
  static constexpr Type RexTypeForceRexW = IceType_i64;
  static constexpr typename Traits::GPRRegister RexRegIrrelevant =
      Traits::GPRRegister::Encoded_Reg_eax;

  inline void emitInt16(int16_t value);
  inline void emitInt32(int32_t value);
  inline void emitRegisterOperand(int rm, int reg);
  template <typename RegType, typename RmType>
  inline void emitXmmRegisterOperand(RegType reg, RmType rm);
  inline void emitOperandSizeOverride();

  void emitOperand(int rm, const typename Traits::Operand &operand);
  void emitImmediate(Type ty, const Immediate &imm);
  void emitComplexI8(int rm, const typename Traits::Operand &operand,
                     const Immediate &immediate);
  void emitComplex(Type Ty, int rm, const typename Traits::Operand &operand,
                   const Immediate &immediate);
  void emitLabel(Label *label, intptr_t instruction_size);
  void emitLabelLink(Label *label);
  void emitNearLabelLink(Label *label);

  void emitGenericShift(int rm, Type Ty, typename Traits::GPRRegister reg,
                        const Immediate &imm);
  void emitGenericShift(int rm, Type Ty,
                        const typename Traits::Operand &operand,
                        typename Traits::GPRRegister shifter);

  using LabelVector = std::vector<Label *>;
  // A vector of pool-allocated x86 labels for CFG nodes.
  LabelVector CfgNodeLabels;
  // A vector of pool-allocated x86 labels for Local labels.
  LabelVector LocalLabels;

  Label *getOrCreateLabel(SizeT Number, LabelVector &Labels);

  // The arith_int() methods factor out the commonality between the encodings
  // of add(), Or(), adc(), sbb(), And(), sub(), Xor(), and cmp(). The Tag
  // parameter is statically asserted to be less than 8.
  template <uint32_t Tag>
  void arith_int(Type Ty, typename Traits::GPRRegister reg,
                 const Immediate &imm);

  template <uint32_t Tag>
  void arith_int(Type Ty, typename Traits::GPRRegister reg0,
                 typename Traits::GPRRegister reg1);

  template <uint32_t Tag>
  void arith_int(Type Ty, typename Traits::GPRRegister reg,
                 const typename Traits::Address &address);

  template <uint32_t Tag>
  void arith_int(Type Ty, const typename Traits::Address &address,
                 typename Traits::GPRRegister reg);

  template <uint32_t Tag>
  void arith_int(Type Ty, const typename Traits::Address &address,
                 const Immediate &imm);

  // gprEncoding returns Reg encoding for operand emission. For x86-64 we mask
  // out the 4th bit as it is encoded in the REX.[RXB] bits. No other bits are
  // touched because we don't want to mask errors.
  template <typename RegType, typename T = Traits>
  typename std::enable_if<T::Is64Bit, typename T::GPRRegister>::type
  gprEncoding(const RegType Reg) {
    return static_cast<typename Traits::GPRRegister>(static_cast<uint8_t>(Reg) &
                                                     ~0x08);
  }

  template <typename RegType, typename T = Traits>
  typename std::enable_if<!T::Is64Bit, typename T::GPRRegister>::type
  gprEncoding(const RegType Reg) {
    return static_cast<typename T::GPRRegister>(Reg);
  }

  template <typename RegType>
  bool is8BitRegisterRequiringRex(const Type Ty, const RegType Reg) {
    static constexpr bool IsGPR =
        std::is_same<typename std::decay<RegType>::type,
                     typename Traits::ByteRegister>::value ||
        std::is_same<typename std::decay<RegType>::type,
                     typename Traits::GPRRegister>::value;

    return IsGPR && (Reg & 0x04) != 0 && (Reg & 0x08) == 0 &&
           isByteSizedType(Ty);
  }

  // assembleAndEmitRex is used for determining which (if any) rex prefix
  // should be emitted for the current instruction. It allows different types
  // for Reg and Rm because they could be of different types (e.g., in mov[sz]x
  // instructions.) If Addr is not nullptr, then Rm is ignored, and Rex.B is
  // determined by Addr instead. TyRm is still used to determine Addr's size.
  template <typename RegType, typename RmType, typename T = Traits>
  typename std::enable_if<T::Is64Bit, void>::type
  assembleAndEmitRex(const Type TyReg, const RegType Reg, const Type TyRm,
                     const RmType Rm,
                     const typename T::Address *Addr = nullptr) {
    const uint8_t W = (TyReg == IceType_i64 || TyRm == IceType_i64)
                          ? T::Operand::RexW
                          : T::Operand::RexNone;
    const uint8_t R = (Reg & 0x08) ? T::Operand::RexR : T::Operand::RexNone;
    const uint8_t X = (Addr != nullptr)
                          ? (typename T::Operand::RexBits)Addr->rexX()
                          : T::Operand::RexNone;
    const uint8_t B =
        (Addr != nullptr)
            ? (typename T::Operand::RexBits)Addr->rexB()
            : (Rm & 0x08) ? T::Operand::RexB : T::Operand::RexNone;
    const uint8_t Prefix = W | R | X | B;
    if (Prefix != T::Operand::RexNone) {
      emitUint8(Prefix);
    } else if (is8BitRegisterRequiringRex(TyReg, Reg) ||
               (Addr == nullptr && is8BitRegisterRequiringRex(TyRm, Rm))) {
      emitUint8(T::Operand::RexBase);
    }
  }

  template <typename RegType, typename RmType, typename T = Traits>
  typename std::enable_if<!T::Is64Bit, void>::type
  assembleAndEmitRex(const Type, const RegType, const Type, const RmType,
                     const typename T::Address * = nullptr) {}

  // emitRexRB is used for emitting a Rex prefix instructions with two explicit
  // register operands in its mod-rm byte.
  template <typename RegType, typename RmType>
  void emitRexRB(const Type Ty, const RegType Reg, const RmType Rm) {
    assembleAndEmitRex(Ty, Reg, Ty, Rm);
  }

  template <typename RegType, typename RmType>
  void emitRexRB(const Type TyReg, const RegType Reg, const Type TyRm,
                 const RmType Rm) {
    assembleAndEmitRex(TyReg, Reg, TyRm, Rm);
  }

  // emitRexB is used for emitting a Rex prefix if one is needed on encoding
  // the Reg field in an x86 instruction. It is invoked by the template when
  // Reg is the single register operand in the instruction (e.g., push Reg.)
  template <typename RmType> void emitRexB(const Type Ty, const RmType Rm) {
    emitRexRB(Ty, RexRegIrrelevant, Ty, Rm);
  }

  // emitRex is used for emitting a Rex prefix for an address and a GPR. The
  // address may contain zero, one, or two registers.
  template <typename RegType>
  void emitRex(const Type Ty, const typename Traits::Address &Addr,
               const RegType Reg) {
    assembleAndEmitRex(Ty, Reg, Ty, RexRegIrrelevant, &Addr);
  }

  template <typename RegType>
  void emitRex(const Type AddrTy, const typename Traits::Address &Addr,
               const Type TyReg, const RegType Reg) {
    assembleAndEmitRex(TyReg, Reg, AddrTy, RexRegIrrelevant, &Addr);
  }
};

template <class Machine>
inline void AssemblerX86Base<Machine>::emitUint8(uint8_t value) {
  Buffer.emit<uint8_t>(value);
}

template <class Machine>
inline void AssemblerX86Base<Machine>::emitInt16(int16_t value) {
  Buffer.emit<int16_t>(value);
}

template <class Machine>
inline void AssemblerX86Base<Machine>::emitInt32(int32_t value) {
  Buffer.emit<int32_t>(value);
}

template <class Machine>
inline void AssemblerX86Base<Machine>::emitRegisterOperand(int reg, int rm) {
  assert(reg >= 0 && reg < 8);
  assert(rm >= 0 && rm < 8);
  Buffer.emit<uint8_t>(0xC0 + (reg << 3) + rm);
}

template <class Machine>
template <typename RegType, typename RmType>
inline void AssemblerX86Base<Machine>::emitXmmRegisterOperand(RegType reg,
                                                              RmType rm) {
  emitRegisterOperand(gprEncoding(reg), gprEncoding(rm));
}

template <class Machine>
inline void AssemblerX86Base<Machine>::emitOperandSizeOverride() {
  emitUint8(0x66);
}

} // end of namespace X86Internal

} // end of namespace Ice

#include "IceAssemblerX86BaseImpl.h"

#endif // SUBZERO_SRC_ICEASSEMBLERX86BASE_H
