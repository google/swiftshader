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
// This file defines the AssemblerX86 template class for x86, the base of all
// X86 assemblers.
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

class Label {
  Label(const Label &) = delete;
  Label &operator=(const Label &) = delete;

public:
  Label() {
    if (BuildDefs::asserts()) {
      for (int i = 0; i < kMaxUnresolvedBranches; i++) {
        unresolved_near_positions_[i] = -1;
      }
    }
  }

  ~Label() = default;

  void FinalCheck() const {
    // Assert if label is being destroyed with unresolved branches pending.
    assert(!IsLinked());
    assert(!HasNear());
  }

  // TODO(jvoung): why are labels offset by this?
  static const uint32_t kWordSize = sizeof(uint32_t);

  // Returns the position for bound labels (branches that come after this
  // are considered backward branches). Cannot be used for unused or linked
  // labels.
  intptr_t Position() const {
    assert(IsBound());
    return -position_ - kWordSize;
  }

  // Returns the position of an earlier branch instruction that was linked
  // to this label (branches that use this are considered forward branches).
  // The linked instructions form a linked list, of sorts, using the
  // instruction's displacement field for the location of the next
  // instruction that is also linked to this label.
  intptr_t LinkPosition() const {
    assert(IsLinked());
    return position_ - kWordSize;
  }

  // Returns the position of an earlier branch instruction which
  // assumes that this label is "near", and bumps iterator to the
  // next near position.
  intptr_t NearPosition() {
    assert(HasNear());
    return unresolved_near_positions_[--num_unresolved_];
  }

  bool IsBound() const { return position_ < 0; }
  bool IsLinked() const { return position_ > 0; }
  bool IsUnused() const { return (position_ == 0) && (num_unresolved_ == 0); }
  bool HasNear() const { return num_unresolved_ != 0; }

private:
  void BindTo(intptr_t position) {
    assert(!IsBound());
    assert(!HasNear());
    position_ = -position - kWordSize;
    assert(IsBound());
  }

  void LinkTo(intptr_t position) {
    assert(!IsBound());
    position_ = position + kWordSize;
    assert(IsLinked());
  }

  void NearLinkTo(intptr_t position) {
    assert(!IsBound());
    assert(num_unresolved_ < kMaxUnresolvedBranches);
    unresolved_near_positions_[num_unresolved_++] = position;
  }

  static constexpr int kMaxUnresolvedBranches = 20;

  intptr_t position_ = 0;
  intptr_t num_unresolved_ = 0;
  // TODO(stichnot,jvoung): Can this instead be
  // llvm::SmallVector<intptr_t, kMaxUnresolvedBranches> ?
  intptr_t unresolved_near_positions_[kMaxUnresolvedBranches];

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

  const char *getNonExecPadDirective() const override { return ".p2align"; }

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

  Label *GetOrCreateCfgNodeLabel(SizeT NodeNumber);
  void bindCfgNodeLabel(SizeT NodeNumber) override;
  Label *GetOrCreateLocalLabel(SizeT Number);
  void BindLocalLabel(SizeT Number);

  bool fixupIsPCRel(FixupKind Kind) const override {
    // Currently assuming this is the only PC-rel relocation type used.
    // TODO(jpp): Traits.PcRelTypes.count(Kind) != 0
    return Kind == Traits::PcRelFixup;
  }

  // Operations to emit GPR instructions (and dispatch on operand type).
  typedef void (AssemblerX86Base::*TypedEmitGPR)(Type,
                                                 typename Traits::GPRRegister);
  typedef void (AssemblerX86Base::*TypedEmitAddr)(
      Type, const typename Traits::Address &);
  struct GPREmitterOneOp {
    TypedEmitGPR Reg;
    TypedEmitAddr Addr;
  };

  typedef void (AssemblerX86Base::*TypedEmitGPRGPR)(
      Type, typename Traits::GPRRegister, typename Traits::GPRRegister);
  typedef void (AssemblerX86Base::*TypedEmitGPRAddr)(
      Type, typename Traits::GPRRegister, const typename Traits::Address &);
  typedef void (AssemblerX86Base::*TypedEmitGPRImm)(
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

  typedef void (AssemblerX86Base::*TypedEmitGPRGPRImm)(
      Type, typename Traits::GPRRegister, typename Traits::GPRRegister,
      const Immediate &);
  struct GPREmitterShiftD {
    // Technically AddrGPR and AddrGPRImm are also allowed, but in practice
    // we always normalize Dest to a Register first.
    TypedEmitGPRGPR GPRGPR;
    TypedEmitGPRGPRImm GPRGPRImm;
  };

  typedef void (AssemblerX86Base::*TypedEmitAddrGPR)(
      Type, const typename Traits::Address &, typename Traits::GPRRegister);
  typedef void (AssemblerX86Base::*TypedEmitAddrImm)(
      Type, const typename Traits::Address &, const Immediate &);
  struct GPREmitterAddrOp {
    TypedEmitAddrGPR AddrGPR;
    TypedEmitAddrImm AddrImm;
  };

  // Operations to emit XMM instructions (and dispatch on operand type).
  typedef void (AssemblerX86Base::*TypedEmitXmmXmm)(
      Type, typename Traits::XmmRegister, typename Traits::XmmRegister);
  typedef void (AssemblerX86Base::*TypedEmitXmmAddr)(
      Type, typename Traits::XmmRegister, const typename Traits::Address &);
  struct XmmEmitterRegOp {
    TypedEmitXmmXmm XmmXmm;
    TypedEmitXmmAddr XmmAddr;
  };

  typedef void (AssemblerX86Base::*EmitXmmXmm)(typename Traits::XmmRegister,
                                               typename Traits::XmmRegister);
  typedef void (AssemblerX86Base::*EmitXmmAddr)(
      typename Traits::XmmRegister, const typename Traits::Address &);
  typedef void (AssemblerX86Base::*EmitAddrXmm)(
      const typename Traits::Address &, typename Traits::XmmRegister);
  struct XmmEmitterMovOps {
    EmitXmmXmm XmmXmm;
    EmitXmmAddr XmmAddr;
    EmitAddrXmm AddrXmm;
  };

  typedef void (AssemblerX86Base::*TypedEmitXmmImm)(
      Type, typename Traits::XmmRegister, const Immediate &);

  struct XmmEmitterShiftOp {
    TypedEmitXmmXmm XmmXmm;
    TypedEmitXmmAddr XmmAddr;
    TypedEmitXmmImm XmmImm;
  };

  // Cross Xmm/GPR cast instructions.
  template <typename DReg_t, typename SReg_t> struct CastEmitterRegOp {
    typedef void (AssemblerX86Base::*TypedEmitRegs)(Type, DReg_t, SReg_t);
    typedef void (AssemblerX86Base::*TypedEmitAddr)(
        Type, DReg_t, const typename Traits::Address &);

    TypedEmitRegs RegReg;
    TypedEmitAddr RegAddr;
  };

  // Three operand (potentially) cross Xmm/GPR instructions.
  // The last operand must be an immediate.
  template <typename DReg_t, typename SReg_t> struct ThreeOpImmEmitter {
    typedef void (AssemblerX86Base::*TypedEmitRegRegImm)(Type, DReg_t, SReg_t,
                                                         const Immediate &);
    typedef void (AssemblerX86Base::*TypedEmitRegAddrImm)(
        Type, DReg_t, const typename Traits::Address &, const Immediate &);

    TypedEmitRegRegImm RegRegImm;
    TypedEmitRegAddrImm RegAddrImm;
  };

  /*
   * Emit Machine Instructions.
   */
  void call(typename Traits::GPRRegister reg);
  void call(const typename Traits::Address &address);
  void call(const ConstantRelocatable *label);
  void call(const Immediate &abs_address);

  static const intptr_t kCallExternalLabelSize = 5;

  void pushl(typename Traits::GPRRegister reg);

  void popl(typename Traits::GPRRegister reg);
  void popl(const typename Traits::Address &address);

  void pushal();
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

  void movd(typename Traits::XmmRegister dst, typename Traits::GPRRegister src);
  void movd(typename Traits::XmmRegister dst,
            const typename Traits::Address &src);
  void movd(typename Traits::GPRRegister dst, typename Traits::XmmRegister src);
  void movd(const typename Traits::Address &dst,
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
  void shufps(typename Traits::XmmRegister dst,
              typename Traits::XmmRegister src, const Immediate &mask);

  void minpd(typename Traits::XmmRegister dst,
             typename Traits::XmmRegister src);
  void maxpd(typename Traits::XmmRegister dst,
             typename Traits::XmmRegister src);
  void sqrtpd(typename Traits::XmmRegister dst);
  void shufpd(typename Traits::XmmRegister dst,
              typename Traits::XmmRegister src, const Immediate &mask);

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

  void cvtsi2ss(Type DestTy, typename Traits::XmmRegister dst,
                typename Traits::GPRRegister src);
  void cvtsi2ss(Type DestTy, typename Traits::XmmRegister dst,
                const typename Traits::Address &src);

  void cvtfloat2float(Type SrcTy, typename Traits::XmmRegister dst,
                      typename Traits::XmmRegister src);
  void cvtfloat2float(Type SrcTy, typename Traits::XmmRegister dst,
                      const typename Traits::Address &src);

  void cvttss2si(Type SrcTy, typename Traits::GPRRegister dst,
                 typename Traits::XmmRegister src);
  void cvttss2si(Type SrcTy, typename Traits::GPRRegister dst,
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
  void pextr(Type Ty, typename Traits::GPRRegister dst,
             const typename Traits::Address &src, const Immediate &imm);

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

  void fld(Type Ty, const typename Traits::Address &src);
  void fstp(Type Ty, const typename Traits::Address &dst);
  void fstp(typename Traits::X87STRegister st);

  void fnstcw(const typename Traits::Address &dst);
  void fldcw(const typename Traits::Address &src);

  void fistpl(const typename Traits::Address &dst);
  void fistps(const typename Traits::Address &dst);
  void fildl(const typename Traits::Address &src);
  void filds(const typename Traits::Address &src);

  void fincstp();

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

  void mul(Type Ty, typename Traits::GPRRegister reg);
  void mul(Type Ty, const typename Traits::Address &address);

  void incl(typename Traits::GPRRegister reg);
  void incl(const typename Traits::Address &address);

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

  void j(typename Traits::Cond::BrCond condition, Label *label,
         bool near = kFarJump);
  void j(typename Traits::Cond::BrCond condition,
         const ConstantRelocatable *label);

  void jmp(typename Traits::GPRRegister reg);
  void jmp(Label *label, bool near = kFarJump);
  void jmp(const ConstantRelocatable *label);

  void mfence();

  void lock();
  void cmpxchg(Type Ty, const typename Traits::Address &address,
               typename Traits::GPRRegister reg, bool Locked);
  void cmpxchg8b(const typename Traits::Address &address, bool Locked);
  void xadd(Type Ty, const typename Traits::Address &address,
            typename Traits::GPRRegister reg, bool Locked);
  void xchg(Type Ty, const typename Traits::Address &address,
            typename Traits::GPRRegister reg);

  void emitSegmentOverride(uint8_t prefix);

  intptr_t preferredLoopAlignment() { return 16; }
  void align(intptr_t alignment, intptr_t offset);
  void bind(Label *label);

  intptr_t CodeSize() const { return Buffer.size(); }

private:
  inline void emitUint8(uint8_t value);
  inline void emitInt16(int16_t value);
  inline void emitInt32(int32_t value);
  inline void emitRegisterOperand(int rm, int reg);
  inline void emitXmmRegisterOperand(int rm, typename Traits::XmmRegister reg);
  inline void emitFixup(AssemblerFixup *fixup);
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

  typedef std::vector<Label *> LabelVector;
  // A vector of pool-allocated x86 labels for CFG nodes.
  LabelVector CfgNodeLabels;
  // A vector of pool-allocated x86 labels for Local labels.
  LabelVector LocalLabels;

  Label *GetOrCreateLabel(SizeT Number, LabelVector &Labels);

  // The arith_int() methods factor out the commonality between the encodings of
  // add(), Or(), adc(), sbb(), And(), sub(), Xor(), and cmp().  The Tag
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
inline void AssemblerX86Base<Machine>::emitRegisterOperand(int rm, int reg) {
  assert(rm >= 0 && rm < 8);
  Buffer.emit<uint8_t>(0xC0 + (rm << 3) + reg);
}

template <class Machine>
inline void AssemblerX86Base<Machine>::emitXmmRegisterOperand(
    int rm, typename Traits::XmmRegister reg) {
  emitRegisterOperand(rm, static_cast<typename Traits::GPRRegister>(reg));
}

template <class Machine>
inline void AssemblerX86Base<Machine>::emitFixup(AssemblerFixup *fixup) {
  Buffer.emitFixup(fixup);
}

template <class Machine>
inline void AssemblerX86Base<Machine>::emitOperandSizeOverride() {
  emitUint8(0x66);
}

} // end of namespace X86Internal

} // end of namespace Ice

#include "IceAssemblerX86BaseImpl.h"

#endif // SUBZERO_SRC_ICEASSEMBLERX86BASE_H
