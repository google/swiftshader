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

// OperandARM32 extends the Operand hierarchy.
// TODO(jvoung): Add the OperandARM32Mem and OperandARM32Flex.
class OperandARM32 : public Operand {
  OperandARM32() = delete;
  OperandARM32(const OperandARM32 &) = delete;
  OperandARM32 &operator=(const OperandARM32 &) = delete;

public:
  enum OperandKindARM32 { k__Start = Operand::kTarget };

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
// TODO(jvoung): Fill out more.
class OperandARM32Mem : public OperandARM32 {
  OperandARM32Mem() = delete;
  OperandARM32Mem(const OperandARM32Mem &) = delete;
  OperandARM32Mem &operator=(const OperandARM32Mem &) = delete;

public:
  // Return true if a load/store instruction for an element of type Ty
  // can encode the Offset directly in the immediate field of the 32-bit
  // ARM instruction. For some types, if the load is Sign extending, then
  // the range is reduced.
  static bool canHoldOffset(Type Ty, bool SignExt, int32_t Offset);
};

class InstARM32 : public InstTarget {
  InstARM32() = delete;
  InstARM32(const InstARM32 &) = delete;
  InstARM32 &operator=(const InstARM32 &) = delete;

public:
  enum InstKindARM32 { k__Start = Inst::Target, Ret };

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

} // end of namespace Ice

#endif // SUBZERO_SRC_ICEINSTARM32_H
