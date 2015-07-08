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
/// This file declares the InstMIPS32 and OperandMIPS32 classes and
/// their subclasses.  This represents the machine instructions and
/// operands used for MIPS32 code selection.
///
//===----------------------------------------------------------------------===//

#ifndef SUBZERO_SRC_ICEINSTMIPS32_H
#define SUBZERO_SRC_ICEINSTMIPS32_H

#include "IceDefs.h"
#include "IceInst.h"
#include "IceInstMIPS32.def"
#include "IceOperand.h"

namespace Ice {

class TargetMIPS32;

/// Base class for Mips instructions.
class InstMIPS32 : public InstTarget {
  InstMIPS32() = delete;
  InstMIPS32(const InstMIPS32 &) = delete;
  InstMIPS32 &operator=(const InstMIPS32 &) = delete;

public:
  enum InstKindMIPS32 { k__Start = Inst::Target, Ret };

  static const char *getWidthString(Type Ty);

  void dump(const Cfg *Func) const override;

protected:
  InstMIPS32(Cfg *Func, InstKindMIPS32 Kind, SizeT Maxsrcs, Variable *Dest)
      : InstTarget(Func, static_cast<InstKind>(Kind), Maxsrcs, Dest) {}
  ~InstMIPS32() override {}
  static bool isClassof(const Inst *Inst, InstKindMIPS32 MyKind) {
    return Inst->getKind() == static_cast<InstKind>(MyKind);
  }
};

/// Ret pseudo-instruction.  This is actually a "jr" instruction with
/// an "ra" register operand, but epilogue lowering will search for a Ret
/// instead of a generic "jr". This instruction also takes a Source
/// operand (for non-void returning functions) for liveness analysis, though
/// a FakeUse before the ret would do just as well.
/// TODO(reed kotler): This needs was take from the ARM port and needs to be
/// scrubbed in the future.
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
  ~InstMIPS32Ret() override {}
};

} // end of namespace Ice

#endif // SUBZERO_SRC_ICEINSTMIPS32_H
