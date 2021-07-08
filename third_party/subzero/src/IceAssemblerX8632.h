//===- subzero/src/IceAssemblerX8632.h - Assembler for x86-32 ---*- C++ -*-===//
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
/// \brief Declares the Assembler class for X86-32.
///
//===----------------------------------------------------------------------===//

#ifndef SUBZERO_SRC_ICEASSEMBLERX8632_H
#define SUBZERO_SRC_ICEASSEMBLERX8632_H

#include "IceAssembler.h"
#include "IceDefs.h"
#include "IceOperand.h"
#include "IceTypes.h"
#include "IceUtils.h"

#include "IceTargetLoweringX8632Traits.h"

namespace Ice {
namespace X8632 {

class AssemblerX8632 : public ::Ice::Assembler {
  AssemblerX8632(const AssemblerX8632 &) = delete;
  AssemblerX8632 &operator=(const AssemblerX8632 &) = delete;

protected:
  explicit AssemblerX8632() : Assembler(Traits::AsmKind) {}

public:
  using Traits = TargetX8632Traits;
  using Address = typename Traits::Address;
  using ByteRegister = typename Traits::ByteRegister;
  using BrCond = CondX86::BrCond;
  using CmppsCond = CondX86::CmppsCond;
  using GPRRegister = typename Traits::GPRRegister;
  using Operand = typename Traits::Operand;
  using XmmRegister = typename Traits::XmmRegister;

  static constexpr int MAX_NOP_SIZE = 8;

  static bool classof(const Assembler *Asm) {
    return Asm->getKind() == Traits::AsmKind;
  }

  class Immediate {
    Immediate(const Immediate &) = delete;
    Immediate &operator=(const Immediate &) = delete;

  public:
    explicit Immediate(int32_t value) : value_(value) {}

    explicit Immediate(AssemblerFixup *fixup) : fixup_(fixup) {}

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
    const int32_t value_ = 0;
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
    friend class AssemblerX8632;

    void nearLinkTo(const Assembler &Asm, intptr_t position) {
      if (Asm.getPreliminary())
        return;
      assert(!isBound());
      UnresolvedNearPositions.push_back(position);
    }

    llvm::SmallVector<intptr_t, 20> UnresolvedNearPositions;
  };

public:
  ~AssemblerX8632() override;

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
    return Kind == Traits::FK_PcRel;
  }

  // Operations to emit GPR instructions (and dispatch on operand type).
  using TypedEmitGPR = void (AssemblerX8632::*)(Type, GPRRegister);
  using TypedEmitAddr = void (AssemblerX8632::*)(Type, const Address &);
  struct GPREmitterOneOp {
    TypedEmitGPR Reg;
    TypedEmitAddr Addr;
  };

  using TypedEmitGPRGPR = void (AssemblerX8632::*)(Type, GPRRegister,
                                                   GPRRegister);
  using TypedEmitGPRAddr = void (AssemblerX8632::*)(Type, GPRRegister,
                                                    const Address &);
  using TypedEmitGPRImm = void (AssemblerX8632::*)(Type, GPRRegister,
                                                   const Immediate &);
  struct GPREmitterRegOp {
    TypedEmitGPRGPR GPRGPR;
    TypedEmitGPRAddr GPRAddr;
    TypedEmitGPRImm GPRImm;
  };

  struct GPREmitterShiftOp {
    // Technically, Addr/GPR and Addr/Imm are also allowed, but */Addr are
    // not. In practice, we always normalize the Dest to a Register first.
    TypedEmitGPRGPR GPRGPR;
    TypedEmitGPRImm GPRImm;
  };

  using TypedEmitGPRGPRImm = void (AssemblerX8632::*)(Type, GPRRegister,
                                                      GPRRegister,
                                                      const Immediate &);
  struct GPREmitterShiftD {
    // Technically AddrGPR and AddrGPRImm are also allowed, but in practice we
    // always normalize Dest to a Register first.
    TypedEmitGPRGPR GPRGPR;
    TypedEmitGPRGPRImm GPRGPRImm;
  };

  using TypedEmitAddrGPR = void (AssemblerX8632::*)(Type, const Address &,
                                                    GPRRegister);
  using TypedEmitAddrImm = void (AssemblerX8632::*)(Type, const Address &,
                                                    const Immediate &);
  struct GPREmitterAddrOp {
    TypedEmitAddrGPR AddrGPR;
    TypedEmitAddrImm AddrImm;
  };

  // Operations to emit XMM instructions (and dispatch on operand type).
  using TypedEmitXmmXmm = void (AssemblerX8632::*)(Type, XmmRegister,
                                                   XmmRegister);
  using TypedEmitXmmAddr = void (AssemblerX8632::*)(Type, XmmRegister,
                                                    const Address &);
  struct XmmEmitterRegOp {
    TypedEmitXmmXmm XmmXmm;
    TypedEmitXmmAddr XmmAddr;
  };

  using EmitXmmXmm = void (AssemblerX8632::*)(XmmRegister, XmmRegister);
  using EmitXmmAddr = void (AssemblerX8632::*)(XmmRegister, const Address &);
  using EmitAddrXmm = void (AssemblerX8632::*)(const Address &, XmmRegister);
  struct XmmEmitterMovOps {
    EmitXmmXmm XmmXmm;
    EmitXmmAddr XmmAddr;
    EmitAddrXmm AddrXmm;
  };

  using TypedEmitXmmImm = void (AssemblerX8632::*)(Type, XmmRegister,
                                                   const Immediate &);

  struct XmmEmitterShiftOp {
    TypedEmitXmmXmm XmmXmm;
    TypedEmitXmmAddr XmmAddr;
    TypedEmitXmmImm XmmImm;
  };

  // Cross Xmm/GPR cast instructions.
  template <typename DReg_t, typename SReg_t> struct CastEmitterRegOp {
    using TypedEmitRegs = void (AssemblerX8632::*)(Type, DReg_t, Type, SReg_t);
    using TypedEmitAddr = void (AssemblerX8632::*)(Type, DReg_t, Type,
                                                   const Address &);

    TypedEmitRegs RegReg;
    TypedEmitAddr RegAddr;
  };

  // Three operand (potentially) cross Xmm/GPR instructions. The last operand
  // must be an immediate.
  template <typename DReg_t, typename SReg_t> struct ThreeOpImmEmitter {
    using TypedEmitRegRegImm = void (AssemblerX8632::*)(Type, DReg_t, SReg_t,
                                                        const Immediate &);
    using TypedEmitRegAddrImm = void (AssemblerX8632::*)(Type, DReg_t,
                                                         const Address &,
                                                         const Immediate &);

    TypedEmitRegRegImm RegRegImm;
    TypedEmitRegAddrImm RegAddrImm;
  };

  /*
   * Emit Machine Instructions.
   */
  void call(GPRRegister reg);
  void call(const Address &address);
  void call(const ConstantRelocatable *label); // not testable.
  void call(const Immediate &abs_address);

  static const intptr_t kCallExternalLabelSize = 5;

  void pushl(GPRRegister reg);
  void pushl(const Immediate &Imm);
  void pushl(const ConstantRelocatable *Label);

  void popl(GPRRegister reg);
  void popl(const Address &address);

  void pushal();
  void popal();

  void setcc(BrCond condition, ByteRegister dst);
  void setcc(BrCond condition, const Address &address);

  void mov(Type Ty, GPRRegister dst, const Immediate &src);
  void mov(Type Ty, GPRRegister dst, GPRRegister src);
  void mov(Type Ty, GPRRegister dst, const Address &src);
  void mov(Type Ty, const Address &dst, GPRRegister src);
  void mov(Type Ty, const Address &dst, const Immediate &imm);

  void movzx(Type Ty, GPRRegister dst, GPRRegister src);
  void movzx(Type Ty, GPRRegister dst, const Address &src);
  void movsx(Type Ty, GPRRegister dst, GPRRegister src);
  void movsx(Type Ty, GPRRegister dst, const Address &src);

  void lea(Type Ty, GPRRegister dst, const Address &src);

  void cmov(Type Ty, BrCond cond, GPRRegister dst, GPRRegister src);
  void cmov(Type Ty, BrCond cond, GPRRegister dst, const Address &src);

  void rep_movsb();

  void movss(Type Ty, XmmRegister dst, const Address &src);
  void movss(Type Ty, const Address &dst, XmmRegister src);
  void movss(Type Ty, XmmRegister dst, XmmRegister src);

  void movd(Type SrcTy, XmmRegister dst, GPRRegister src);
  void movd(Type SrcTy, XmmRegister dst, const Address &src);
  void movd(Type DestTy, GPRRegister dst, XmmRegister src);
  void movd(Type DestTy, const Address &dst, XmmRegister src);

  void movq(XmmRegister dst, XmmRegister src);
  void movq(const Address &dst, XmmRegister src);
  void movq(XmmRegister dst, const Address &src);

  void addss(Type Ty, XmmRegister dst, XmmRegister src);
  void addss(Type Ty, XmmRegister dst, const Address &src);
  void subss(Type Ty, XmmRegister dst, XmmRegister src);
  void subss(Type Ty, XmmRegister dst, const Address &src);
  void mulss(Type Ty, XmmRegister dst, XmmRegister src);
  void mulss(Type Ty, XmmRegister dst, const Address &src);
  void divss(Type Ty, XmmRegister dst, XmmRegister src);
  void divss(Type Ty, XmmRegister dst, const Address &src);

  void movaps(XmmRegister dst, XmmRegister src);

  void movups(XmmRegister dst, XmmRegister src);
  void movups(XmmRegister dst, const Address &src);
  void movups(const Address &dst, XmmRegister src);

  void padd(Type Ty, XmmRegister dst, XmmRegister src);
  void padd(Type Ty, XmmRegister dst, const Address &src);
  void padds(Type Ty, XmmRegister dst, XmmRegister src);
  void padds(Type Ty, XmmRegister dst, const Address &src);
  void paddus(Type Ty, XmmRegister dst, XmmRegister src);
  void paddus(Type Ty, XmmRegister dst, const Address &src);
  void pand(Type Ty, XmmRegister dst, XmmRegister src);
  void pand(Type Ty, XmmRegister dst, const Address &src);
  void pandn(Type Ty, XmmRegister dst, XmmRegister src);
  void pandn(Type Ty, XmmRegister dst, const Address &src);
  void pmull(Type Ty, XmmRegister dst, XmmRegister src);
  void pmull(Type Ty, XmmRegister dst, const Address &src);
  void pmulhw(Type Ty, XmmRegister dst, XmmRegister src);
  void pmulhw(Type Ty, XmmRegister dst, const Address &src);
  void pmulhuw(Type Ty, XmmRegister dst, XmmRegister src);
  void pmulhuw(Type Ty, XmmRegister dst, const Address &src);
  void pmaddwd(Type Ty, XmmRegister dst, XmmRegister src);
  void pmaddwd(Type Ty, XmmRegister dst, const Address &src);
  void pmuludq(Type Ty, XmmRegister dst, XmmRegister src);
  void pmuludq(Type Ty, XmmRegister dst, const Address &src);
  void por(Type Ty, XmmRegister dst, XmmRegister src);
  void por(Type Ty, XmmRegister dst, const Address &src);
  void psub(Type Ty, XmmRegister dst, XmmRegister src);
  void psub(Type Ty, XmmRegister dst, const Address &src);
  void psubs(Type Ty, XmmRegister dst, XmmRegister src);
  void psubs(Type Ty, XmmRegister dst, const Address &src);
  void psubus(Type Ty, XmmRegister dst, XmmRegister src);
  void psubus(Type Ty, XmmRegister dst, const Address &src);
  void pxor(Type Ty, XmmRegister dst, XmmRegister src);
  void pxor(Type Ty, XmmRegister dst, const Address &src);

  void psll(Type Ty, XmmRegister dst, XmmRegister src);
  void psll(Type Ty, XmmRegister dst, const Address &src);
  void psll(Type Ty, XmmRegister dst, const Immediate &src);

  void psra(Type Ty, XmmRegister dst, XmmRegister src);
  void psra(Type Ty, XmmRegister dst, const Address &src);
  void psra(Type Ty, XmmRegister dst, const Immediate &src);
  void psrl(Type Ty, XmmRegister dst, XmmRegister src);
  void psrl(Type Ty, XmmRegister dst, const Address &src);
  void psrl(Type Ty, XmmRegister dst, const Immediate &src);

  void addps(Type Ty, XmmRegister dst, XmmRegister src);
  void addps(Type Ty, XmmRegister dst, const Address &src);
  void subps(Type Ty, XmmRegister dst, XmmRegister src);
  void subps(Type Ty, XmmRegister dst, const Address &src);
  void divps(Type Ty, XmmRegister dst, XmmRegister src);
  void divps(Type Ty, XmmRegister dst, const Address &src);
  void mulps(Type Ty, XmmRegister dst, XmmRegister src);
  void mulps(Type Ty, XmmRegister dst, const Address &src);
  void minps(Type Ty, XmmRegister dst, const Address &src);
  void minps(Type Ty, XmmRegister dst, XmmRegister src);
  void minss(Type Ty, XmmRegister dst, const Address &src);
  void minss(Type Ty, XmmRegister dst, XmmRegister src);
  void maxps(Type Ty, XmmRegister dst, const Address &src);
  void maxps(Type Ty, XmmRegister dst, XmmRegister src);
  void maxss(Type Ty, XmmRegister dst, const Address &src);
  void maxss(Type Ty, XmmRegister dst, XmmRegister src);
  void andnps(Type Ty, XmmRegister dst, const Address &src);
  void andnps(Type Ty, XmmRegister dst, XmmRegister src);
  void andps(Type Ty, XmmRegister dst, const Address &src);
  void andps(Type Ty, XmmRegister dst, XmmRegister src);
  void orps(Type Ty, XmmRegister dst, const Address &src);
  void orps(Type Ty, XmmRegister dst, XmmRegister src);

  void blendvps(Type Ty, XmmRegister dst, XmmRegister src);
  void blendvps(Type Ty, XmmRegister dst, const Address &src);
  void pblendvb(Type Ty, XmmRegister dst, XmmRegister src);
  void pblendvb(Type Ty, XmmRegister dst, const Address &src);

  void cmpps(Type Ty, XmmRegister dst, XmmRegister src, CmppsCond CmpCondition);
  void cmpps(Type Ty, XmmRegister dst, const Address &src,
             CmppsCond CmpCondition);

  void sqrtps(XmmRegister dst);
  void rsqrtps(XmmRegister dst);
  void reciprocalps(XmmRegister dst);

  void movhlps(XmmRegister dst, XmmRegister src);
  void movlhps(XmmRegister dst, XmmRegister src);
  void unpcklps(XmmRegister dst, XmmRegister src);
  void unpckhps(XmmRegister dst, XmmRegister src);
  void unpcklpd(XmmRegister dst, XmmRegister src);
  void unpckhpd(XmmRegister dst, XmmRegister src);

  void set1ps(XmmRegister dst, GPRRegister tmp, const Immediate &imm);

  void sqrtpd(XmmRegister dst);

  void pshufb(Type Ty, XmmRegister dst, XmmRegister src);
  void pshufb(Type Ty, XmmRegister dst, const Address &src);
  void pshufd(Type Ty, XmmRegister dst, XmmRegister src, const Immediate &mask);
  void pshufd(Type Ty, XmmRegister dst, const Address &src,
              const Immediate &mask);
  void punpckl(Type Ty, XmmRegister Dst, XmmRegister Src);
  void punpckl(Type Ty, XmmRegister Dst, const Address &Src);
  void punpckh(Type Ty, XmmRegister Dst, XmmRegister Src);
  void punpckh(Type Ty, XmmRegister Dst, const Address &Src);
  void packss(Type Ty, XmmRegister Dst, XmmRegister Src);
  void packss(Type Ty, XmmRegister Dst, const Address &Src);
  void packus(Type Ty, XmmRegister Dst, XmmRegister Src);
  void packus(Type Ty, XmmRegister Dst, const Address &Src);
  void shufps(Type Ty, XmmRegister dst, XmmRegister src, const Immediate &mask);
  void shufps(Type Ty, XmmRegister dst, const Address &src,
              const Immediate &mask);

  void cvtdq2ps(Type, XmmRegister dst, XmmRegister src);
  void cvtdq2ps(Type, XmmRegister dst, const Address &src);

  void cvttps2dq(Type, XmmRegister dst, XmmRegister src);
  void cvttps2dq(Type, XmmRegister dst, const Address &src);

  void cvtps2dq(Type, XmmRegister dst, XmmRegister src);
  void cvtps2dq(Type, XmmRegister dst, const Address &src);

  void cvtsi2ss(Type DestTy, XmmRegister dst, Type SrcTy, GPRRegister src);
  void cvtsi2ss(Type DestTy, XmmRegister dst, Type SrcTy, const Address &src);

  void cvtfloat2float(Type SrcTy, XmmRegister dst, XmmRegister src);
  void cvtfloat2float(Type SrcTy, XmmRegister dst, const Address &src);

  void cvttss2si(Type DestTy, GPRRegister dst, Type SrcTy, XmmRegister src);
  void cvttss2si(Type DestTy, GPRRegister dst, Type SrcTy, const Address &src);

  void cvtss2si(Type DestTy, GPRRegister dst, Type SrcTy, XmmRegister src);
  void cvtss2si(Type DestTy, GPRRegister dst, Type SrcTy, const Address &src);

  void ucomiss(Type Ty, XmmRegister a, XmmRegister b);
  void ucomiss(Type Ty, XmmRegister a, const Address &b);

  void movmsk(Type Ty, GPRRegister dst, XmmRegister src);

  void sqrt(Type Ty, XmmRegister dst, const Address &src);
  void sqrt(Type Ty, XmmRegister dst, XmmRegister src);

  void xorps(Type Ty, XmmRegister dst, const Address &src);
  void xorps(Type Ty, XmmRegister dst, XmmRegister src);

  void insertps(Type Ty, XmmRegister dst, XmmRegister src,
                const Immediate &imm);
  void insertps(Type Ty, XmmRegister dst, const Address &src,
                const Immediate &imm);

  void pinsr(Type Ty, XmmRegister dst, GPRRegister src, const Immediate &imm);
  void pinsr(Type Ty, XmmRegister dst, const Address &src,
             const Immediate &imm);

  void pextr(Type Ty, GPRRegister dst, XmmRegister src, const Immediate &imm);

  void pmovsxdq(XmmRegister dst, XmmRegister src);

  void pcmpeq(Type Ty, XmmRegister dst, XmmRegister src);
  void pcmpeq(Type Ty, XmmRegister dst, const Address &src);
  void pcmpgt(Type Ty, XmmRegister dst, XmmRegister src);
  void pcmpgt(Type Ty, XmmRegister dst, const Address &src);

  enum RoundingMode {
    kRoundToNearest = 0x0,
    kRoundDown = 0x1,
    kRoundUp = 0x2,
    kRoundToZero = 0x3
  };
  void round(Type Ty, XmmRegister dst, XmmRegister src, const Immediate &mode);
  void round(Type Ty, XmmRegister dst, const Address &src,
             const Immediate &mode);

  //----------------------------------------------------------------------------
  //
  // Begin: X87 instructions.
  //
  //----------------------------------------------------------------------------
  void fld(Type Ty, const Address &src);
  void fstp(Type Ty, const Address &dst);
  void fstp(RegX8632::X87STRegister st);

  void fnstcw(const Address &dst);
  void fldcw(const Address &src);

  void fistpl(const Address &dst);
  void fistps(const Address &dst);
  void fildl(const Address &src);
  void filds(const Address &src);

  void fincstp();
  //----------------------------------------------------------------------------
  //
  // End: X87 instructions.
  //
  //----------------------------------------------------------------------------

  void cmp(Type Ty, GPRRegister reg0, GPRRegister reg1);
  void cmp(Type Ty, GPRRegister reg, const Address &address);
  void cmp(Type Ty, GPRRegister reg, const Immediate &imm);
  void cmp(Type Ty, const Address &address, GPRRegister reg);
  void cmp(Type Ty, const Address &address, const Immediate &imm);

  void test(Type Ty, GPRRegister reg0, GPRRegister reg1);
  void test(Type Ty, GPRRegister reg, const Immediate &imm);
  void test(Type Ty, const Address &address, GPRRegister reg);
  void test(Type Ty, const Address &address, const Immediate &imm);

  void And(Type Ty, GPRRegister dst, GPRRegister src);
  void And(Type Ty, GPRRegister dst, const Address &address);
  void And(Type Ty, GPRRegister dst, const Immediate &imm);
  void And(Type Ty, const Address &address, GPRRegister reg);
  void And(Type Ty, const Address &address, const Immediate &imm);

  void Or(Type Ty, GPRRegister dst, GPRRegister src);
  void Or(Type Ty, GPRRegister dst, const Address &address);
  void Or(Type Ty, GPRRegister dst, const Immediate &imm);
  void Or(Type Ty, const Address &address, GPRRegister reg);
  void Or(Type Ty, const Address &address, const Immediate &imm);

  void Xor(Type Ty, GPRRegister dst, GPRRegister src);
  void Xor(Type Ty, GPRRegister dst, const Address &address);
  void Xor(Type Ty, GPRRegister dst, const Immediate &imm);
  void Xor(Type Ty, const Address &address, GPRRegister reg);
  void Xor(Type Ty, const Address &address, const Immediate &imm);

  void add(Type Ty, GPRRegister dst, GPRRegister src);
  void add(Type Ty, GPRRegister reg, const Address &address);
  void add(Type Ty, GPRRegister reg, const Immediate &imm);
  void add(Type Ty, const Address &address, GPRRegister reg);
  void add(Type Ty, const Address &address, const Immediate &imm);

  void adc(Type Ty, GPRRegister dst, GPRRegister src);
  void adc(Type Ty, GPRRegister dst, const Address &address);
  void adc(Type Ty, GPRRegister reg, const Immediate &imm);
  void adc(Type Ty, const Address &address, GPRRegister reg);
  void adc(Type Ty, const Address &address, const Immediate &imm);

  void sub(Type Ty, GPRRegister dst, GPRRegister src);
  void sub(Type Ty, GPRRegister reg, const Address &address);
  void sub(Type Ty, GPRRegister reg, const Immediate &imm);
  void sub(Type Ty, const Address &address, GPRRegister reg);
  void sub(Type Ty, const Address &address, const Immediate &imm);

  void sbb(Type Ty, GPRRegister dst, GPRRegister src);
  void sbb(Type Ty, GPRRegister reg, const Address &address);
  void sbb(Type Ty, GPRRegister reg, const Immediate &imm);
  void sbb(Type Ty, const Address &address, GPRRegister reg);
  void sbb(Type Ty, const Address &address, const Immediate &imm);

  void cbw();
  void cwd();
  void cdq();
  template <typename T = Traits>
  typename std::enable_if<T::Is64Bit, void>::type cqo();
  template <typename T = Traits>
  typename std::enable_if<!T::Is64Bit, void>::type cqo() {
    llvm::report_fatal_error("CQO is only available in 64-bit x86 backends.");
  }

  void div(Type Ty, GPRRegister reg);
  void div(Type Ty, const Address &address);

  void idiv(Type Ty, GPRRegister reg);
  void idiv(Type Ty, const Address &address);

  void imul(Type Ty, GPRRegister dst, GPRRegister src);
  void imul(Type Ty, GPRRegister reg, const Immediate &imm);
  void imul(Type Ty, GPRRegister reg, const Address &address);

  void imul(Type Ty, GPRRegister reg);
  void imul(Type Ty, const Address &address);

  void imul(Type Ty, GPRRegister dst, GPRRegister src, const Immediate &imm);
  void imul(Type Ty, GPRRegister dst, const Address &address,
            const Immediate &imm);

  void mul(Type Ty, GPRRegister reg);
  void mul(Type Ty, const Address &address);

  template <class T = Traits,
            typename = typename std::enable_if<!T::Is64Bit>::type>
  void incl(GPRRegister reg);
  void incl(const Address &address);

  template <class T = Traits,
            typename = typename std::enable_if<!T::Is64Bit>::type>
  void decl(GPRRegister reg);
  void decl(const Address &address);

  void rol(Type Ty, GPRRegister reg, const Immediate &imm);
  void rol(Type Ty, GPRRegister operand, GPRRegister shifter);
  void rol(Type Ty, const Address &operand, GPRRegister shifter);

  void shl(Type Ty, GPRRegister reg, const Immediate &imm);
  void shl(Type Ty, GPRRegister operand, GPRRegister shifter);
  void shl(Type Ty, const Address &operand, GPRRegister shifter);

  void shr(Type Ty, GPRRegister reg, const Immediate &imm);
  void shr(Type Ty, GPRRegister operand, GPRRegister shifter);
  void shr(Type Ty, const Address &operand, GPRRegister shifter);

  void sar(Type Ty, GPRRegister reg, const Immediate &imm);
  void sar(Type Ty, GPRRegister operand, GPRRegister shifter);
  void sar(Type Ty, const Address &address, GPRRegister shifter);

  void shld(Type Ty, GPRRegister dst, GPRRegister src);
  void shld(Type Ty, GPRRegister dst, GPRRegister src, const Immediate &imm);
  void shld(Type Ty, const Address &operand, GPRRegister src);
  void shrd(Type Ty, GPRRegister dst, GPRRegister src);
  void shrd(Type Ty, GPRRegister dst, GPRRegister src, const Immediate &imm);
  void shrd(Type Ty, const Address &dst, GPRRegister src);

  void neg(Type Ty, GPRRegister reg);
  void neg(Type Ty, const Address &addr);
  void notl(GPRRegister reg);

  void bsf(Type Ty, GPRRegister dst, GPRRegister src);
  void bsf(Type Ty, GPRRegister dst, const Address &src);
  void bsr(Type Ty, GPRRegister dst, GPRRegister src);
  void bsr(Type Ty, GPRRegister dst, const Address &src);

  void bswap(Type Ty, GPRRegister reg);

  void bt(GPRRegister base, GPRRegister offset);

  void ret();
  void ret(const Immediate &imm);

  // 'size' indicates size in bytes and must be in the range 1..8.
  void nop(int size = 1);
  void int3();
  void hlt();
  void ud2();

  // j(Label) is fully tested.
  void j(BrCond condition, Label *label, bool near = kFarJump);
  void j(BrCond condition, const ConstantRelocatable *label); // not testable.

  void jmp(GPRRegister reg);
  void jmp(Label *label, bool near = kFarJump);
  void jmp(const ConstantRelocatable *label); // not testable.
  void jmp(const Immediate &abs_address);

  void mfence();

  void lock();
  void cmpxchg(Type Ty, const Address &address, GPRRegister reg, bool Locked);
  void cmpxchg8b(const Address &address, bool Locked);
  void xadd(Type Ty, const Address &address, GPRRegister reg, bool Locked);
  void xchg(Type Ty, GPRRegister reg0, GPRRegister reg1);
  void xchg(Type Ty, const Address &address, GPRRegister reg);

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
  ENABLE_MAKE_UNIQUE;

  static constexpr Type RexTypeIrrelevant = IceType_i32;
  static constexpr Type RexTypeForceRexW = IceType_i64;
  static constexpr GPRRegister RexRegIrrelevant =
      Traits::GPRRegister::Encoded_Reg_eax;

  inline void emitInt16(int16_t value);
  inline void emitInt32(int32_t value);
  inline void emitRegisterOperand(int rm, int reg);
  template <typename RegType, typename RmType>
  inline void emitXmmRegisterOperand(RegType reg, RmType rm);
  inline void emitOperandSizeOverride();

  void emitOperand(int rm, const Operand &operand, RelocOffsetT Addend = 0);
  void emitImmediate(Type ty, const Immediate &imm);
  void emitComplexI8(int rm, const Operand &operand,
                     const Immediate &immediate);
  void emitComplex(Type Ty, int rm, const Operand &operand,
                   const Immediate &immediate);
  void emitLabel(Label *label, intptr_t instruction_size);
  void emitLabelLink(Label *label);
  void emitNearLabelLink(Label *label);

  void emitGenericShift(int rm, Type Ty, GPRRegister reg, const Immediate &imm);
  void emitGenericShift(int rm, Type Ty, const Operand &operand,
                        GPRRegister shifter);

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
  void arith_int(Type Ty, GPRRegister reg, const Immediate &imm);

  template <uint32_t Tag>
  void arith_int(Type Ty, GPRRegister reg0, GPRRegister reg1);

  template <uint32_t Tag>
  void arith_int(Type Ty, GPRRegister reg, const Address &address);

  template <uint32_t Tag>
  void arith_int(Type Ty, const Address &address, GPRRegister reg);

  template <uint32_t Tag>
  void arith_int(Type Ty, const Address &address, const Immediate &imm);

  // gprEncoding returns Reg encoding for operand emission. For x86-64 we mask
  // out the 4th bit as it is encoded in the REX.[RXB] bits. No other bits are
  // touched because we don't want to mask errors.
  template <typename RegType, typename T = Traits>
  typename std::enable_if<T::Is64Bit, typename T::GPRRegister>::type
  gprEncoding(const RegType Reg) {
    return static_cast<GPRRegister>(static_cast<uint8_t>(Reg) & ~0x08);
  }

  template <typename RegType, typename T = Traits>
  typename std::enable_if<!T::Is64Bit, typename T::GPRRegister>::type
  gprEncoding(const RegType Reg) {
    return static_cast<typename T::GPRRegister>(Reg);
  }
};

inline void AssemblerX8632::emitUint8(uint8_t value) {
  Buffer.emit<uint8_t>(value);
}

inline void AssemblerX8632::emitInt16(int16_t value) {
  Buffer.emit<int16_t>(value);
}

inline void AssemblerX8632::emitInt32(int32_t value) {
  Buffer.emit<int32_t>(value);
}

inline void AssemblerX8632::emitRegisterOperand(int reg, int rm) {
  assert(reg >= 0 && reg < 8);
  assert(rm >= 0 && rm < 8);
  Buffer.emit<uint8_t>(0xC0 + (reg << 3) + rm);
}

template <typename RegType, typename RmType>
inline void AssemblerX8632::emitXmmRegisterOperand(RegType reg, RmType rm) {
  emitRegisterOperand(gprEncoding(reg), gprEncoding(rm));
}

inline void AssemblerX8632::emitOperandSizeOverride() { emitUint8(0x66); }

using Label = AssemblerX8632::Label;
using Immediate = AssemblerX8632::Immediate;

} // end of namespace X8632
} // end of namespace Ice

#endif // SUBZERO_SRC_ICEASSEMBLERX8632_H
