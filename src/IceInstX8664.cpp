//===- subzero/src/IceInstX8664.cpp - X86-64 instruction implementation ---===//
//
//                        The Subzero Code Generator
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file defines X8664 specific data related to X8664 Instructions and
/// Instruction traits. These are declared in the IceTargetLoweringX8664Traits.h
/// header file.
///
/// This file also defines X8664 operand specific methods (dump and emit.)
///
//===----------------------------------------------------------------------===//
#include "IceInstX8664.h"

#include "IceAssemblerX8664.h"
#include "IceCfg.h"
#include "IceCfgNode.h"
#include "IceConditionCodesX8664.h"
#include "IceInst.h"
#include "IceRegistersX8664.h"
#include "IceTargetLoweringX8664.h"
#include "IceOperand.h"

namespace Ice {

namespace X86Internal {

const MachineTraits<TargetX8664>::InstBrAttributesType
    MachineTraits<TargetX8664>::InstBrAttributes[] = {
#define X(tag, encode, opp, dump, emit)                                        \
  { X8664::Traits::Cond::opp, dump, emit }                                     \
  ,
        ICEINSTX8664BR_TABLE
#undef X
};

const MachineTraits<TargetX8664>::InstCmppsAttributesType
    MachineTraits<TargetX8664>::InstCmppsAttributes[] = {
#define X(tag, emit)                                                           \
  { emit }                                                                     \
  ,
        ICEINSTX8664CMPPS_TABLE
#undef X
};

const MachineTraits<TargetX8664>::TypeAttributesType
    MachineTraits<TargetX8664>::TypeAttributes[] = {
#define X(tag, elementty, cvt, sdss, pack, width, fld)                         \
  { cvt, sdss, pack, width, fld }                                              \
  ,
        ICETYPEX8664_TABLE
#undef X
};

void MachineTraits<TargetX8664>::X86Operand::dump(const Cfg *,
                                                  Ostream &Str) const {
  if (BuildDefs::dump())
    Str << "<OperandX8664>";
}

MachineTraits<TargetX8664>::X86OperandMem::X86OperandMem(Cfg *Func, Type Ty,
                                                         Variable *Base,
                                                         Constant *Offset,
                                                         Variable *Index,
                                                         uint16_t Shift)
    : X86Operand(kMem, Ty), Base(Base), Offset(Offset), Index(Index),
      Shift(Shift) {
  assert(Shift <= 3);
  Vars = nullptr;
  NumVars = 0;
  if (Base)
    ++NumVars;
  if (Index)
    ++NumVars;
  if (NumVars) {
    Vars = Func->allocateArrayOf<Variable *>(NumVars);
    SizeT I = 0;
    if (Base)
      Vars[I++] = Base;
    if (Index)
      Vars[I++] = Index;
    assert(I == NumVars);
  }
}

void MachineTraits<TargetX8664>::X86OperandMem::emit(const Cfg *Func) const {
  if (!BuildDefs::dump())
    return;
  Ostream &Str = Func->getContext()->getStrEmit();
  // Emit as Offset(Base,Index,1<<Shift).
  // Offset is emitted without the leading '$'.
  // Omit the (Base,Index,1<<Shift) part if Base==nullptr.
  if (!Offset) {
    // No offset, emit nothing.
  } else if (const auto CI = llvm::dyn_cast<ConstantInteger32>(Offset)) {
    if (Base == nullptr || CI->getValue())
      // Emit a non-zero offset without a leading '$'.
      Str << CI->getValue();
  } else if (const auto CR = llvm::dyn_cast<ConstantRelocatable>(Offset)) {
    CR->emitWithoutPrefix(Func->getTarget());
  } else {
    llvm_unreachable("Invalid offset type for x86 mem operand");
  }

  if (Base) {
    Str << "(";
    Base->emit(Func);
    if (Index) {
      Str << ",";
      Index->emit(Func);
      if (Shift)
        Str << "," << (1u << Shift);
    }
    Str << ")";
  }
}

void MachineTraits<TargetX8664>::X86OperandMem::dump(const Cfg *Func,
                                                     Ostream &Str) const {
  if (!BuildDefs::dump())
    return;
  bool Dumped = false;
  Str << "[";
  if (Base) {
    if (Func)
      Base->dump(Func);
    else
      Base->dump(Str);
    Dumped = true;
  }
  if (Index) {
    assert(Base);
    Str << "+";
    if (Shift > 0)
      Str << (1u << Shift) << "*";
    if (Func)
      Index->dump(Func);
    else
      Index->dump(Str);
    Dumped = true;
  }
  // Pretty-print the Offset.
  bool OffsetIsZero = false;
  bool OffsetIsNegative = false;
  if (!Offset) {
    OffsetIsZero = true;
  } else if (const auto CI = llvm::dyn_cast<ConstantInteger32>(Offset)) {
    OffsetIsZero = (CI->getValue() == 0);
    OffsetIsNegative = (static_cast<int32_t>(CI->getValue()) < 0);
  } else {
    assert(llvm::isa<ConstantRelocatable>(Offset));
  }
  if (Dumped) {
    if (!OffsetIsZero) {     // Suppress if Offset is known to be 0
      if (!OffsetIsNegative) // Suppress if Offset is known to be negative
        Str << "+";
      Offset->dump(Func, Str);
    }
  } else {
    // There is only the offset.
    Offset->dump(Func, Str);
  }
  Str << "]";
}

MachineTraits<TargetX8664>::Address
MachineTraits<TargetX8664>::X86OperandMem::toAsmAddress(
    MachineTraits<TargetX8664>::Assembler *Asm) const {
  int32_t Disp = 0;
  AssemblerFixup *Fixup = nullptr;
  // Determine the offset (is it relocatable?)
  if (getOffset()) {
    if (const auto CI = llvm::dyn_cast<ConstantInteger32>(getOffset())) {
      Disp = static_cast<int32_t>(CI->getValue());
    } else if (const auto CR =
                   llvm::dyn_cast<ConstantRelocatable>(getOffset())) {
      Disp = CR->getOffset() - 4;
      Fixup = Asm->createFixup(PcRelFixup, CR);
    } else {
      llvm_unreachable("Unexpected offset type");
    }
  }

  // Now convert to the various possible forms.
  if (getBase() && getIndex()) {
    return X8664::Traits::Address(
        RegX8664::getEncodedGPR(getBase()->getRegNum()),
        RegX8664::getEncodedGPR(getIndex()->getRegNum()),
        X8664::Traits::ScaleFactor(getShift()), Disp);
  } else if (getBase()) {
    return X8664::Traits::Address(
        RegX8664::getEncodedGPR(getBase()->getRegNum()), Disp);
  } else if (getIndex()) {
    return X8664::Traits::Address(
        RegX8664::getEncodedGPR(getIndex()->getRegNum()),
        X8664::Traits::ScaleFactor(getShift()), Disp);
  } else if (Fixup) {
    return X8664::Traits::Address::Absolute(Disp, Fixup);
  } else {
    return X8664::Traits::Address::Absolute(Disp);
  }
}

MachineTraits<TargetX8664>::Address
MachineTraits<TargetX8664>::VariableSplit::toAsmAddress(const Cfg *Func) const {
  assert(!Var->hasReg());
  const ::Ice::TargetLowering *Target = Func->getTarget();
  int32_t Offset =
      Var->getStackOffset() + Target->getStackAdjustment() + getOffset();
  return X8664::Traits::Address(
      RegX8664::getEncodedGPR(Target->getFrameOrStackReg()), Offset);
}

void MachineTraits<TargetX8664>::VariableSplit::emit(const Cfg *Func) const {
  if (!BuildDefs::dump())
    return;
  Ostream &Str = Func->getContext()->getStrEmit();
  assert(!Var->hasReg());
  // The following is copied/adapted from TargetX8664::emitVariable().
  const ::Ice::TargetLowering *Target = Func->getTarget();
  const Type Ty = IceType_i32;
  int32_t Offset =
      Var->getStackOffset() + Target->getStackAdjustment() + getOffset();
  if (Offset)
    Str << Offset;
  Str << "(%" << Target->getRegName(Target->getFrameOrStackReg(), Ty) << ")";
}

void MachineTraits<TargetX8664>::VariableSplit::dump(const Cfg *Func,
                                                     Ostream &Str) const {
  if (!BuildDefs::dump())
    return;
  switch (Part) {
  case Low:
    Str << "low";
    break;
  case High:
    Str << "high";
    break;
  }
  Str << "(";
  if (Func)
    Var->dump(Func);
  else
    Var->dump(Str);
  Str << ")";
}

} // namespace X86Internal
} // end of namespace Ice

X86INSTS_DEFINE_STATIC_DATA(TargetX8664);
