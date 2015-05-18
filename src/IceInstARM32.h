//===- subzero/src/IceInstARM32.h - ARM32 machine instructions --*- C++ -*-===//
//
//                        The Subzero Code Generator
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares the InstARM32 and OperandARM32 classes and
// their subclasses.  This represents the machine instructions and
// operands used for ARM32 code selection.
//
//===----------------------------------------------------------------------===//

#ifndef SUBZERO_SRC_ICEINSTARM32_H
#define SUBZERO_SRC_ICEINSTARM32_H

#include "IceDefs.h"
#include "IceInst.h"
#include "IceInstARM32.def"
#include "IceOperand.h"

namespace Ice {

class TargetARM32;

// OperandARM32 extends the Operand hierarchy.  Its subclasses are
// OperandARM32Mem and OperandARM32Flex.
class OperandARM32 : public Operand {
  OperandARM32() = delete;
  OperandARM32(const OperandARM32 &) = delete;
  OperandARM32 &operator=(const OperandARM32 &) = delete;

public:
  enum OperandKindARM32 {
    k__Start = Operand::kTarget,
    kMem,
    kFlexStart,
    kFlexImm = kFlexStart,
    kFlexReg,
    kFlexEnd = kFlexReg
  };

  enum ShiftKind {
    kNoShift = -1,
#define X(enum, emit) enum,
    ICEINSTARM32SHIFT_TABLE
#undef X
  };

  using Operand::dump;
  void dump(const Cfg *, Ostream &Str) const override {
    if (ALLOW_DUMP)
      Str << "<OperandARM32>";
  }

protected:
  OperandARM32(OperandKindARM32 Kind, Type Ty)
      : Operand(static_cast<OperandKind>(Kind), Ty) {}
  ~OperandARM32() override {}
};

// OperandARM32Mem represents a memory operand in any of the various ARM32
// addressing modes.
class OperandARM32Mem : public OperandARM32 {
  OperandARM32Mem() = delete;
  OperandARM32Mem(const OperandARM32Mem &) = delete;
  OperandARM32Mem &operator=(const OperandARM32Mem &) = delete;

public:
  // Memory operand addressing mode.
  // The enum value also carries the encoding.
  // TODO(jvoung): unify with the assembler.
  enum AddrMode {
    // bit encoding P U W
    Offset = (8 | 4 | 0) << 21,      // offset (w/o writeback to base)
    PreIndex = (8 | 4 | 1) << 21,    // pre-indexed addressing with writeback
    PostIndex = (0 | 4 | 0) << 21,   // post-indexed addressing with writeback
    NegOffset = (8 | 0 | 0) << 21,   // negative offset (w/o writeback to base)
    NegPreIndex = (8 | 0 | 1) << 21, // negative pre-indexed with writeback
    NegPostIndex = (0 | 0 | 0) << 21 // negative post-indexed with writeback
  };

  // Provide two constructors.
  // NOTE: The Variable-typed operands have to be registers.
  //
  // (1) Reg + Imm. The Immediate actually has a limited number of bits
  // for encoding, so check canHoldOffset first. It cannot handle
  // general Constant operands like ConstantRelocatable, since a relocatable
  // can potentially take up too many bits.
  static OperandARM32Mem *create(Cfg *Func, Type Ty, Variable *Base,
                                 ConstantInteger32 *ImmOffset = nullptr,
                                 AddrMode Mode = Offset) {
    return new (Func->allocate<OperandARM32Mem>())
        OperandARM32Mem(Func, Ty, Base, ImmOffset, Mode);
  }
  // (2) Reg +/- Reg with an optional shift of some kind and amount.
  // Note that this mode is disallowed in the NaCl sandbox.
  static OperandARM32Mem *create(Cfg *Func, Type Ty, Variable *Base,
                                 Variable *Index, ShiftKind ShiftOp = kNoShift,
                                 uint16_t ShiftAmt = 0,
                                 AddrMode Mode = Offset) {
    return new (Func->allocate<OperandARM32Mem>())
        OperandARM32Mem(Func, Ty, Base, Index, ShiftOp, ShiftAmt, Mode);
  }
  Variable *getBase() const { return Base; }
  ConstantInteger32 *getOffset() const { return ImmOffset; }
  Variable *getIndex() const { return Index; }
  ShiftKind getShiftOp() const { return ShiftOp; }
  uint16_t getShiftAmt() const { return ShiftAmt; }
  AddrMode getAddrMode() const { return Mode; }

  bool isRegReg() const { return Index != nullptr; }
  bool isNegAddrMode() const { return Mode >= NegOffset; }

  void emit(const Cfg *Func) const override;
  using OperandARM32::dump;
  void dump(const Cfg *Func, Ostream &Str) const override;

  static bool classof(const Operand *Operand) {
    return Operand->getKind() == static_cast<OperandKind>(kMem);
  }

  // Return true if a load/store instruction for an element of type Ty
  // can encode the Offset directly in the immediate field of the 32-bit
  // ARM instruction. For some types, if the load is Sign extending, then
  // the range is reduced.
  static bool canHoldOffset(Type Ty, bool SignExt, int32_t Offset);

private:
  OperandARM32Mem(Cfg *Func, Type Ty, Variable *Base,
                  ConstantInteger32 *ImmOffset, AddrMode Mode);
  OperandARM32Mem(Cfg *Func, Type Ty, Variable *Base, Variable *Index,
                  ShiftKind ShiftOp, uint16_t ShiftAmt, AddrMode Mode);
  ~OperandARM32Mem() override {}
  Variable *Base;
  ConstantInteger32 *ImmOffset;
  Variable *Index;
  ShiftKind ShiftOp;
  uint16_t ShiftAmt;
  AddrMode Mode;
};

// OperandARM32Flex represent the "flexible second operand" for
// data-processing instructions. It can be a rotatable 8-bit constant, or
// a register with an optional shift operand. The shift amount can even be
// a third register.
class OperandARM32Flex : public OperandARM32 {
  OperandARM32Flex() = delete;
  OperandARM32Flex(const OperandARM32Flex &) = delete;
  OperandARM32Flex &operator=(const OperandARM32Flex &) = delete;

public:
  static bool classof(const Operand *Operand) {
    return static_cast<OperandKind>(kFlexStart) <= Operand->getKind() &&
           Operand->getKind() <= static_cast<OperandKind>(kFlexEnd);
  }

protected:
  OperandARM32Flex(OperandKindARM32 Kind, Type Ty) : OperandARM32(Kind, Ty) {}
  ~OperandARM32Flex() override {}
};

// Rotated immediate variant.
class OperandARM32FlexImm : public OperandARM32Flex {
  OperandARM32FlexImm() = delete;
  OperandARM32FlexImm(const OperandARM32FlexImm &) = delete;
  OperandARM32FlexImm &operator=(const OperandARM32FlexImm &) = delete;

public:
  // Immed_8 rotated by an even number of bits (2 * RotateAmt).
  static OperandARM32FlexImm *create(Cfg *Func, Type Ty, uint32_t Imm,
                                     uint32_t RotateAmt) {
    return new (Func->allocate<OperandARM32FlexImm>())
        OperandARM32FlexImm(Func, Ty, Imm, RotateAmt);
  }

  void emit(const Cfg *Func) const override;
  using OperandARM32::dump;
  void dump(const Cfg *Func, Ostream &Str) const override;

  static bool classof(const Operand *Operand) {
    return Operand->getKind() == static_cast<OperandKind>(kFlexImm);
  }

  // Return true if the Immediate can fit in the ARM flexible operand.
  // Fills in the out-params RotateAmt and Immed_8 if Immediate fits.
  static bool canHoldImm(uint32_t Immediate, uint32_t *RotateAmt,
                         uint32_t *Immed_8);

  uint32_t getImm() const { return Imm; }
  uint32_t getRotateAmt() const { return RotateAmt; }

private:
  OperandARM32FlexImm(Cfg *Func, Type Ty, uint32_t Imm, uint32_t RotateAmt);
  ~OperandARM32FlexImm() override {}

  uint32_t Imm;
  uint32_t RotateAmt;
};

// Shifted register variant.
class OperandARM32FlexReg : public OperandARM32Flex {
  OperandARM32FlexReg() = delete;
  OperandARM32FlexReg(const OperandARM32FlexReg &) = delete;
  OperandARM32FlexReg &operator=(const OperandARM32FlexReg &) = delete;

public:
  // Register with immediate/reg shift amount and shift operation.
  static OperandARM32FlexReg *create(Cfg *Func, Type Ty, Variable *Reg,
                                     ShiftKind ShiftOp, Operand *ShiftAmt) {
    return new (Func->allocate<OperandARM32FlexReg>())
        OperandARM32FlexReg(Func, Ty, Reg, ShiftOp, ShiftAmt);
  }

  void emit(const Cfg *Func) const override;
  using OperandARM32::dump;
  void dump(const Cfg *Func, Ostream &Str) const override;

  static bool classof(const Operand *Operand) {
    return Operand->getKind() == static_cast<OperandKind>(kFlexReg);
  }

  Variable *getReg() const { return Reg; }
  ShiftKind getShiftOp() const { return ShiftOp; }
  // ShiftAmt can represent an immediate or a register.
  Operand *getShiftAmt() const { return ShiftAmt; }

private:
  OperandARM32FlexReg(Cfg *Func, Type Ty, Variable *Reg, ShiftKind ShiftOp,
                      Operand *ShiftAmt);
  ~OperandARM32FlexReg() override {}

  Variable *Reg;
  ShiftKind ShiftOp;
  Operand *ShiftAmt;
};

class InstARM32 : public InstTarget {
  InstARM32() = delete;
  InstARM32(const InstARM32 &) = delete;
  InstARM32 &operator=(const InstARM32 &) = delete;

public:
  enum InstKindARM32 {
    k__Start = Inst::Target,
    Mov,
    Movt,
    Movw,
    Mvn,
    Ret,
    Ldr
  };

  static const char *getWidthString(Type Ty);

  void dump(const Cfg *Func) const override;

protected:
  InstARM32(Cfg *Func, InstKindARM32 Kind, SizeT Maxsrcs, Variable *Dest)
      : InstTarget(Func, static_cast<InstKind>(Kind), Maxsrcs, Dest) {}
  ~InstARM32() override {}
  static bool isClassof(const Inst *Inst, InstKindARM32 MyKind) {
    return Inst->getKind() == static_cast<InstKind>(MyKind);
  }
};

void emitTwoAddr(const char *Opcode, const Inst *Inst, const Cfg *Func);

// TODO(jvoung): add condition codes if instruction can be predicated.

// Instructions of the form x := op(y).
template <InstARM32::InstKindARM32 K>
class InstARM32UnaryopGPR : public InstARM32 {
  InstARM32UnaryopGPR() = delete;
  InstARM32UnaryopGPR(const InstARM32UnaryopGPR &) = delete;
  InstARM32UnaryopGPR &operator=(const InstARM32UnaryopGPR &) = delete;

public:
  static InstARM32UnaryopGPR *create(Cfg *Func, Variable *Dest, Operand *Src) {
    return new (Func->allocate<InstARM32UnaryopGPR>())
        InstARM32UnaryopGPR(Func, Dest, Src);
  }
  void emit(const Cfg *Func) const override {
    if (!ALLOW_DUMP)
      return;
    Ostream &Str = Func->getContext()->getStrEmit();
    assert(getSrcSize() == 1);
    Str << "\t" << Opcode << "\t";
    getDest()->emit(Func);
    Str << ", ";
    getSrc(0)->emit(Func);
  }
  void emitIAS(const Cfg *Func) const override {
    (void)Func;
    llvm_unreachable("Not yet implemented");
  }
  void dump(const Cfg *Func) const override {
    if (!ALLOW_DUMP)
      return;
    Ostream &Str = Func->getContext()->getStrDump();
    dumpDest(Func);
    Str << " = " << Opcode << "." << getDest()->getType() << " ";
    dumpSources(Func);
  }
  static bool classof(const Inst *Inst) { return isClassof(Inst, K); }

private:
  InstARM32UnaryopGPR(Cfg *Func, Variable *Dest, Operand *Src)
      : InstARM32(Func, K, 1, Dest) {
    addSource(Src);
  }
  ~InstARM32UnaryopGPR() override {}
  static const char *Opcode;
};

// Instructions of the form x := x op y.
template <InstARM32::InstKindARM32 K>
class InstARM32TwoAddrGPR : public InstARM32 {
  InstARM32TwoAddrGPR() = delete;
  InstARM32TwoAddrGPR(const InstARM32TwoAddrGPR &) = delete;
  InstARM32TwoAddrGPR &operator=(const InstARM32TwoAddrGPR &) = delete;

public:
  // Dest must be a register.
  static InstARM32TwoAddrGPR *create(Cfg *Func, Variable *Dest, Operand *Src) {
    return new (Func->allocate<InstARM32TwoAddrGPR>())
        InstARM32TwoAddrGPR(Func, Dest, Src);
  }
  void emit(const Cfg *Func) const override {
    if (!ALLOW_DUMP)
      return;
    emitTwoAddr(Opcode, this, Func);
  }
  void emitIAS(const Cfg *Func) const override {
    (void)Func;
    llvm::report_fatal_error("Not yet implemented");
  }
  void dump(const Cfg *Func) const override {
    if (!ALLOW_DUMP)
      return;
    Ostream &Str = Func->getContext()->getStrDump();
    dumpDest(Func);
    Str << " = " << Opcode << "." << getDest()->getType() << " ";
    dumpSources(Func);
  }
  static bool classof(const Inst *Inst) { return isClassof(Inst, K); }

private:
  InstARM32TwoAddrGPR(Cfg *Func, Variable *Dest, Operand *Src)
      : InstARM32(Func, K, 2, Dest) {
    addSource(Dest);
    addSource(Src);
  }
  ~InstARM32TwoAddrGPR() override {}
  static const char *Opcode;
};

// Base class for assignment instructions.
// These can be tested for redundancy (and elided if redundant).
template <InstARM32::InstKindARM32 K>
class InstARM32Movlike : public InstARM32 {
  InstARM32Movlike() = delete;
  InstARM32Movlike(const InstARM32Movlike &) = delete;
  InstARM32Movlike &operator=(const InstARM32Movlike &) = delete;

public:
  static InstARM32Movlike *create(Cfg *Func, Variable *Dest, Operand *Source) {
    return new (Func->allocate<InstARM32Movlike>())
        InstARM32Movlike(Func, Dest, Source);
  }
  bool isRedundantAssign() const override {
    return checkForRedundantAssign(getDest(), getSrc(0));
  }
  bool isSimpleAssign() const override { return true; }
  void emit(const Cfg *Func) const override;
  void emitIAS(const Cfg *Func) const override;
  void dump(const Cfg *Func) const override {
    if (!ALLOW_DUMP)
      return;
    Ostream &Str = Func->getContext()->getStrDump();
    Str << Opcode << "." << getDest()->getType() << " ";
    dumpDest(Func);
    Str << ", ";
    dumpSources(Func);
  }
  static bool classof(const Inst *Inst) { return isClassof(Inst, K); }

private:
  InstARM32Movlike(Cfg *Func, Variable *Dest, Operand *Source)
      : InstARM32(Func, K, 1, Dest) {
    addSource(Source);
  }
  ~InstARM32Movlike() override {}

  static const char *Opcode;
};

// Move instruction (variable <- flex). This is more of a pseudo-inst.
// If var is a register, then we use "mov". If var is stack, then we use
// "str" to store to the stack.
typedef InstARM32Movlike<InstARM32::Mov> InstARM32Mov;
// MovT leaves the bottom bits alone so dest is also a source.
// This helps indicate that a previous MovW setting dest is not dead code.
typedef InstARM32TwoAddrGPR<InstARM32::Movt> InstARM32Movt;
typedef InstARM32UnaryopGPR<InstARM32::Movw> InstARM32Movw;
typedef InstARM32UnaryopGPR<InstARM32::Mvn> InstARM32Mvn;

// Load instruction.
class InstARM32Ldr : public InstARM32 {
  InstARM32Ldr() = delete;
  InstARM32Ldr(const InstARM32Ldr &) = delete;
  InstARM32Ldr &operator=(const InstARM32Ldr &) = delete;

public:
  // Dest must be a register.
  static InstARM32Ldr *create(Cfg *Func, Variable *Dest, OperandARM32Mem *Mem) {
    return new (Func->allocate<InstARM32Ldr>()) InstARM32Ldr(Func, Dest, Mem);
  }
  void emit(const Cfg *Func) const override;
  void emitIAS(const Cfg *Func) const override;
  void dump(const Cfg *Func) const override;
  static bool classof(const Inst *Inst) { return isClassof(Inst, Ldr); }

private:
  InstARM32Ldr(Cfg *Func, Variable *Dest, OperandARM32Mem *Mem);
  ~InstARM32Ldr() override {}
};

// Ret pseudo-instruction.  This is actually a "bx" instruction with
// an "lr" register operand, but epilogue lowering will search for a Ret
// instead of a generic "bx". This instruction also takes a Source
// operand (for non-void returning functions) for liveness analysis, though
// a FakeUse before the ret would do just as well.
class InstARM32Ret : public InstARM32 {
  InstARM32Ret() = delete;
  InstARM32Ret(const InstARM32Ret &) = delete;
  InstARM32Ret &operator=(const InstARM32Ret &) = delete;

public:
  static InstARM32Ret *create(Cfg *Func, Variable *LR,
                              Variable *Source = nullptr) {
    return new (Func->allocate<InstARM32Ret>()) InstARM32Ret(Func, LR, Source);
  }
  void emit(const Cfg *Func) const override;
  void emitIAS(const Cfg *Func) const override;
  void dump(const Cfg *Func) const override;
  static bool classof(const Inst *Inst) { return isClassof(Inst, Ret); }

private:
  InstARM32Ret(Cfg *Func, Variable *LR, Variable *Source);
  ~InstARM32Ret() override {}
};

// Declare partial template specializations of emit() methods that
// already have default implementations.  Without this, there is the
// possibility of ODR violations and link errors.

template <> void InstARM32Movw::emit(const Cfg *Func) const;
template <> void InstARM32Movt::emit(const Cfg *Func) const;

} // end of namespace Ice

#endif // SUBZERO_SRC_ICEINSTARM32_H
