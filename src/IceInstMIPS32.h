//===- subzero/src/IceInstMIPS32.h - MIPS32 machine instrs --*- C++ -*-===    //
//
//                        The Subzero Code Generator
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief Declares the InstMIPS32 and OperandMIPS32 classes and their
/// subclasses.
///
/// This represents the machine instructions and operands used for MIPS32 code
/// selection.
///
//===----------------------------------------------------------------------===//

#ifndef SUBZERO_SRC_ICEINSTMIPS32_H
#define SUBZERO_SRC_ICEINSTMIPS32_H

#include "IceDefs.h"
#include "IceInst.h"
#include "IceInstMIPS32.def"
#include "IceOperand.h"

namespace Ice {
namespace MIPS32 {

class TargetMIPS32;

/// OperandMips32 extends the Operand hierarchy.
//
class OperandMIPS32 : public Operand {
  OperandMIPS32() = delete;
  OperandMIPS32(const OperandMIPS32 &) = delete;
  OperandMIPS32 &operator=(const OperandMIPS32 &) = delete;

public:
  enum OperandKindMIPS32 {
    k__Start = Operand::kTarget,
    kMem,
  };

  using Operand::dump;
  void dump(const Cfg *, Ostream &Str) const override {
    if (BuildDefs::dump())
      Str << "<OperandMIPS32>";
  }

protected:
  OperandMIPS32(OperandKindMIPS32 Kind, Type Ty)
      : Operand(static_cast<OperandKind>(Kind), Ty) {}
};

class OperandMIPS32Mem : public OperandMIPS32 {
  OperandMIPS32Mem() = delete;
  OperandMIPS32Mem(const OperandMIPS32Mem &) = delete;
  OperandMIPS32Mem &operator=(const OperandMIPS32Mem &) = delete;

public:
  /// Memory operand addressing mode.
  /// The enum value also carries the encoding.
  // TODO(jvoung): unify with the assembler.
  enum AddrMode { Offset };

  /// NOTE: The Variable-typed operands have to be registers.
  ///
  /// Reg + Imm. The Immediate actually has a limited number of bits
  /// for encoding, so check canHoldOffset first. It cannot handle
  /// general Constant operands like ConstantRelocatable, since a relocatable
  /// can potentially take up too many bits.
  static OperandMIPS32Mem *create(Cfg *Func, Type Ty, Variable *Base,
                                  ConstantInteger32 *ImmOffset,
                                  AddrMode Mode = Offset) {
    return new (Func->allocate<OperandMIPS32Mem>())
        OperandMIPS32Mem(Func, Ty, Base, ImmOffset, Mode);
  }

  Variable *getBase() const { return Base; }
  ConstantInteger32 *getOffset() const { return ImmOffset; }
  AddrMode getAddrMode() const { return Mode; }

  void emit(const Cfg *Func) const override;
  using OperandMIPS32::dump;

  static bool classof(const Operand *Operand) {
    return Operand->getKind() == static_cast<OperandKind>(kMem);
  }

  /// Return true if a load/store instruction for an element of type Ty
  /// can encode the Offset directly in the immediate field of the 32-bit
  /// MIPS instruction. For some types, if the load is Sign extending, then
  /// the range is reduced.
  static bool canHoldOffset(Type Ty, bool SignExt, int32_t Offset);

  void dump(const Cfg *Func, Ostream &Str) const override {
    (void)Func;
    (void)Str;
  }

private:
  OperandMIPS32Mem(Cfg *Func, Type Ty, Variable *Base,
                   ConstantInteger32 *ImmOffset, AddrMode Mode);

  Variable *Base;
  ConstantInteger32 *ImmOffset;
  AddrMode Mode;
};

/// Base class for Mips instructions.
class InstMIPS32 : public InstTarget {
  InstMIPS32() = delete;
  InstMIPS32(const InstMIPS32 &) = delete;
  InstMIPS32 &operator=(const InstMIPS32 &) = delete;

public:
  enum InstKindMIPS32 {
    k__Start = Inst::Target,
    Add,
    Addu,
    And,
    Addiu,
    Call,
    La,
    Lui,
    Mfhi,
    Mflo,
    Mov, // actually a pseudo op for addi rd, rs, 0
    Mthi,
    Mtlo,
    Mul,
    Mult,
    Multu,
    Or,
    Ori,
    Ret,
    Sub,
    Subu,
    Sltu,
    Xor
  };

  static const char *getWidthString(Type Ty);

  void dump(const Cfg *Func) const override;

  void dumpOpcode(Ostream &Str, const char *Opcode, Type Ty) const {
    Str << Opcode << "." << Ty;
  }

  // TODO(rkotler): while branching is not implemented
  bool repointEdges(CfgNode *, CfgNode *) override { return true; }

  /// Shared emit routines for common forms of instructions.
  static void emitUnaryopGPR(const char *Opcode, const InstMIPS32 *Inst,
                             const Cfg *Func);
  static void emitUnaryopGPRFLoHi(const char *Opcode, const InstMIPS32 *Inst,
                                  const Cfg *Func);
  static void emitUnaryopGPRTLoHi(const char *Opcode, const InstMIPS32 *Inst,
                                  const Cfg *Func);
  static void emitThreeAddr(const char *Opcode, const InstMIPS32 *Inst,
                            const Cfg *Func);
  static void emitThreeAddrLoHi(const char *Opcode, const InstMIPS32 *Inst,
                                const Cfg *Func);

protected:
  InstMIPS32(Cfg *Func, InstKindMIPS32 Kind, SizeT Maxsrcs, Variable *Dest)
      : InstTarget(Func, static_cast<InstKind>(Kind), Maxsrcs, Dest) {}
  static bool isClassof(const Inst *Inst, InstKindMIPS32 MyKind) {
    return Inst->getKind() == static_cast<InstKind>(MyKind);
  }
};

/// Ret pseudo-instruction. This is actually a "jr" instruction with an "ra"
/// register operand, but epilogue lowering will search for a Ret instead of a
/// generic "jr". This instruction also takes a Source operand (for non-void
/// returning functions) for liveness analysis, though a FakeUse before the ret
/// would do just as well.
// TODO(reed kotler): This needs was take from the ARM port and needs to be
// scrubbed in the future.
class InstMIPS32Ret : public InstMIPS32 {

  InstMIPS32Ret() = delete;
  InstMIPS32Ret(const InstMIPS32Ret &) = delete;
  InstMIPS32Ret &operator=(const InstMIPS32Ret &) = delete;

public:
  static InstMIPS32Ret *create(Cfg *Func, Variable *RA,
                               Variable *Source = nullptr) {
    return new (Func->allocate<InstMIPS32Ret>())
        InstMIPS32Ret(Func, RA, Source);
  }
  void emit(const Cfg *Func) const override;
  void emitIAS(const Cfg *Func) const override;
  void dump(const Cfg *Func) const override;
  static bool classof(const Inst *Inst) { return isClassof(Inst, Ret); }

private:
  InstMIPS32Ret(Cfg *Func, Variable *RA, Variable *Source);
};

/// Instructions of the form x := op(y).
template <InstMIPS32::InstKindMIPS32 K>
class InstMIPS32UnaryopGPR : public InstMIPS32 {
  InstMIPS32UnaryopGPR() = delete;
  InstMIPS32UnaryopGPR(const InstMIPS32UnaryopGPR &) = delete;
  InstMIPS32UnaryopGPR &operator=(const InstMIPS32UnaryopGPR &) = delete;

public:
  static InstMIPS32UnaryopGPR *create(Cfg *Func, Variable *Dest, Operand *Src) {
    return new (Func->allocate<InstMIPS32UnaryopGPR>())
        InstMIPS32UnaryopGPR(Func, Dest, Src);
  }
  void emit(const Cfg *Func) const override {
    if (!BuildDefs::dump())
      return;
    emitUnaryopGPR(Opcode, this, Func);
  }
  void emitIAS(const Cfg *Func) const override {
    (void)Func;
    llvm_unreachable("Not yet implemented");
  }
  void dump(const Cfg *Func) const override {
    if (!BuildDefs::dump())
      return;
    Ostream &Str = Func->getContext()->getStrDump();
    dumpOpcode(Str, Opcode, getDest()->getType());
    Str << " ";
    dumpDest(Func);
    Str << ", ";
    dumpSources(Func);
  }
  static bool classof(const Inst *Inst) { return isClassof(Inst, K); }

protected:
  InstMIPS32UnaryopGPR(Cfg *Func, Variable *Dest, Operand *Src)
      : InstMIPS32(Func, K, 1, Dest) {
    addSource(Src);
  }

private:
  static const char *Opcode;
};

/// Instructions of the form x := y op z. May have the side-effect of setting
/// status flags.
template <InstMIPS32::InstKindMIPS32 K>
class InstMIPS32ThreeAddrGPR : public InstMIPS32 {
  InstMIPS32ThreeAddrGPR() = delete;
  InstMIPS32ThreeAddrGPR(const InstMIPS32ThreeAddrGPR &) = delete;
  InstMIPS32ThreeAddrGPR &operator=(const InstMIPS32ThreeAddrGPR &) = delete;

public:
  /// Create an ordinary binary-op instruction like add, and sub. Dest and Src1
  /// must be registers.
  static InstMIPS32ThreeAddrGPR *create(Cfg *Func, Variable *Dest,
                                        Variable *Src0, Variable *Src1) {
    return new (Func->allocate<InstMIPS32ThreeAddrGPR>())
        InstMIPS32ThreeAddrGPR(Func, Dest, Src0, Src1);
  }
  void emit(const Cfg *Func) const override {
    if (!BuildDefs::dump())
      return;
    emitThreeAddr(Opcode, this, Func);
  }
  void emitIAS(const Cfg *Func) const override {
    (void)Func;
    llvm_unreachable("Not yet implemented");
  }

  void dump(const Cfg *Func) const override {
    if (!BuildDefs::dump())
      return;
    Ostream &Str = Func->getContext()->getStrDump();
    dumpDest(Func);
    Str << " = ";
    dumpOpcode(Str, Opcode, getDest()->getType());
    Str << " ";
    dumpSources(Func);
  }
  static bool classof(const Inst *Inst) { return isClassof(Inst, K); }

private:
  InstMIPS32ThreeAddrGPR(Cfg *Func, Variable *Dest, Variable *Src0,
                         Variable *Src1)
      : InstMIPS32(Func, K, 2, Dest) {
    addSource(Src0);
    addSource(Src1);
  }

  static const char *Opcode;
};

class InstMIPS32Call : public InstMIPS32 {
  InstMIPS32Call() = delete;
  InstMIPS32Call(const InstMIPS32Call &) = delete;
  InstMIPS32Call &operator=(const InstMIPS32Call &) = delete;

public:
  static InstMIPS32Call *create(Cfg *Func, Variable *Dest,
                                Operand *CallTarget) {
    return new (Func->allocate<InstMIPS32Call>())
        InstMIPS32Call(Func, Dest, CallTarget);
  }
  Operand *getCallTarget() const { return getSrc(0); }
  void emit(const Cfg *Func) const override;
  void emitIAS(const Cfg *Func) const override;
  void dump(const Cfg *Func) const override;
  static bool classof(const Inst *Inst) { return isClassof(Inst, Call); }

private:
  InstMIPS32Call(Cfg *Func, Variable *Dest, Operand *CallTarget);
};

template <InstMIPS32::InstKindMIPS32 K, bool Signed = false>
class InstMIPS32Imm16 : public InstMIPS32 {
  InstMIPS32Imm16() = delete;
  InstMIPS32Imm16(const InstMIPS32Imm16 &) = delete;
  InstMIPS32Imm16 &operator=(const InstMIPS32Imm16 &) = delete;

public:
  static InstMIPS32Imm16 *create(Cfg *Func, Variable *Dest, Operand *Source,
                                 uint32_t Imm) {
    return new (Func->allocate<InstMIPS32Imm16>())
        InstMIPS32Imm16(Func, Dest, Source, Imm);
  }

  static InstMIPS32Imm16 *create(Cfg *Func, Variable *Dest, uint32_t Imm) {
    return new (Func->allocate<InstMIPS32Imm16>())
        InstMIPS32Imm16(Func, Dest, Imm);
  }

  void emit(const Cfg *Func) const override {
    if (!BuildDefs::dump())
      return;
    Ostream &Str = Func->getContext()->getStrEmit();
    Str << "\t" << Opcode << "\t";
    getDest()->emit(Func);
    if (getSrcSize() > 0) {
      Str << ", ";
      getSrc(0)->emit(Func);
    }
    Str << ", ";
    if (Signed)
      Str << (int32_t)Imm;
    else
      Str << Imm;
    Str << "\n";
  }

  void emitIAS(const Cfg *Func) const override {
    (void)Func;
    llvm_unreachable("Not yet implemented");
  }
  void dump(const Cfg *Func) const override {
    if (!BuildDefs::dump())
      return;
    Ostream &Str = Func->getContext()->getStrDump();
    Str << " ";
    Str << "\t" << Opcode << "\t";
    dumpDest(Func);
    Str << ", ";
    dumpSources(Func);
    if (Signed)
      Str << (int32_t)Imm;
    else
      Str << Imm;
    Str << "\n";
  }

  static bool classof(const Inst *Inst) { return isClassof(Inst, K); }

private:
  InstMIPS32Imm16(Cfg *Func, Variable *Dest, Operand *Source, uint32_t Imm)
      : InstMIPS32(Func, K, 1, Dest), Imm(Imm) {
    addSource(Source);
  }

  InstMIPS32Imm16(Cfg *Func, Variable *Dest, uint32_t Imm)
      : InstMIPS32(Func, K, 0, Dest), Imm(Imm) {}

  static const char *Opcode;

  const uint32_t Imm;
};

using InstMIPS32Add = InstMIPS32ThreeAddrGPR<InstMIPS32::Add>;
using InstMIPS32Addu = InstMIPS32ThreeAddrGPR<InstMIPS32::Addu>;
using InstMIPS32Addiu = InstMIPS32Imm16<InstMIPS32::Addiu, true>;
using InstMIPS32And = InstMIPS32ThreeAddrGPR<InstMIPS32::And>;
using InstMIPS32Lui = InstMIPS32Imm16<InstMIPS32::Lui>;
using InstMIPS32La = InstMIPS32UnaryopGPR<InstMIPS32::La>;
using InstMIPS32Mfhi = InstMIPS32UnaryopGPR<InstMIPS32::Mfhi>;
using InstMIPS32Mflo = InstMIPS32UnaryopGPR<InstMIPS32::Mflo>;
using InstMIPS32Mthi = InstMIPS32UnaryopGPR<InstMIPS32::Mthi>;
using InstMIPS32Mtlo = InstMIPS32UnaryopGPR<InstMIPS32::Mtlo>;
using InstMIPS32Mul = InstMIPS32ThreeAddrGPR<InstMIPS32::Mul>;
using InstMIPS32Mult = InstMIPS32ThreeAddrGPR<InstMIPS32::Mult>;
using InstMIPS32Multu = InstMIPS32ThreeAddrGPR<InstMIPS32::Multu>;
using InstMIPS32Or = InstMIPS32ThreeAddrGPR<InstMIPS32::Or>;
using InstMIPS32Ori = InstMIPS32Imm16<InstMIPS32::Ori>;
using InstMIPS32Sltu = InstMIPS32ThreeAddrGPR<InstMIPS32::Sltu>;
using InstMIPS32Sub = InstMIPS32ThreeAddrGPR<InstMIPS32::Sub>;
using InstMIPS32Subu = InstMIPS32ThreeAddrGPR<InstMIPS32::Subu>;
using InstMIPS32Ori = InstMIPS32Imm16<InstMIPS32::Ori>;
using InstMIPS32Xor = InstMIPS32ThreeAddrGPR<InstMIPS32::Xor>;

/// Handles (some of) vmov's various formats.
class InstMIPS32Mov final : public InstMIPS32 {
  InstMIPS32Mov() = delete;
  InstMIPS32Mov(const InstMIPS32Mov &) = delete;
  InstMIPS32Mov &operator=(const InstMIPS32Mov &) = delete;

public:
  static InstMIPS32Mov *create(Cfg *Func, Variable *Dest, Operand *Src) {
    return new (Func->allocate<InstMIPS32Mov>()) InstMIPS32Mov(Func, Dest, Src);
  }
  bool isRedundantAssign() const override {
    return !isMultiDest() && !isMultiSource() &&
           checkForRedundantAssign(getDest(), getSrc(0));
  }
  // bool isSimpleAssign() const override { return true; }
  void emit(const Cfg *Func) const override;
  void emitIAS(const Cfg *Func) const override;
  void dump(const Cfg *Func) const override;
  static bool classof(const Inst *Inst) { return isClassof(Inst, Mov); }

  bool isMultiDest() const { return DestHi != nullptr; }

  bool isMultiSource() const {
    assert(getSrcSize() == 1 || getSrcSize() == 2);
    return getSrcSize() == 2;
  }

  Variable *getDestHi() const { return DestHi; }

private:
  InstMIPS32Mov(Cfg *Func, Variable *Dest, Operand *Src);

  void emitMultiDestSingleSource(const Cfg *Func) const;
  void emitSingleDestMultiSource(const Cfg *Func) const;
  void emitSingleDestSingleSource(const Cfg *Func) const;

  Variable *DestHi = nullptr;
};

} // end of namespace MIPS32
} // end of namespace Ice

#endif // SUBZERO_SRC_ICEINSTMIPS32_H
