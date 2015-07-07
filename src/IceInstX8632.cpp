//===- subzero/src/IceInstX8632.cpp - X86-32 instruction implementation ---===//
//
//                        The Subzero Code Generator
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file defines X8632 specific data related to X8632 Instructions and
/// Instruction traits. These are declared in the IceTargetLoweringX8632Traits.h
/// header file.
///
/// This file also defines X8632 operand specific methods (dump and emit.)
///
//===----------------------------------------------------------------------===//
#include "IceInstX8632.h"

#include "IceAssemblerX8632.h"
#include "IceCfg.h"
#include "IceCfgNode.h"
#include "IceConditionCodesX8632.h"
#include "IceInst.h"
#include "IceRegistersX8632.h"
#include "IceTargetLoweringX8632.h"
#include "IceOperand.h"

namespace Ice {

namespace X86Internal {

const MachineTraits<TargetX8632>::InstBrAttributesType
    MachineTraits<TargetX8632>::InstBrAttributes[] = {
#define X(tag, encode, opp, dump, emit)                                        \
  { X8632::Traits::Cond::opp, dump, emit }                                     \
  ,
        ICEINSTX8632BR_TABLE
#undef X
};

const MachineTraits<TargetX8632>::InstCmppsAttributesType
    MachineTraits<TargetX8632>::InstCmppsAttributes[] = {
#define X(tag, emit)                                                           \
  { emit }                                                                     \
  ,
        ICEINSTX8632CMPPS_TABLE
#undef X
};

const MachineTraits<TargetX8632>::TypeAttributesType
    MachineTraits<TargetX8632>::TypeAttributes[] = {
#define X(tag, elementty, cvt, sdss, pack, width, fld)                         \
  { cvt, sdss, pack, width, fld }                                              \
  ,
        ICETYPEX8632_TABLE
#undef X
};

const char *MachineTraits<TargetX8632>::InstSegmentRegNames[] = {
#define X(val, name, prefix) name,
    SEG_REGX8632_TABLE
#undef X
};

uint8_t MachineTraits<TargetX8632>::InstSegmentPrefixes[] = {
#define X(val, name, prefix) prefix,
    SEG_REGX8632_TABLE
#undef X
};

void MachineTraits<TargetX8632>::X86Operand::dump(const Cfg *,
                                                  Ostream &Str) const {
  if (BuildDefs::dump())
    Str << "<OperandX8632>";
}

MachineTraits<TargetX8632>::X86OperandMem::X86OperandMem(
    Cfg *Func, Type Ty, Variable *Base, Constant *Offset, Variable *Index,
    uint16_t Shift, SegmentRegisters SegmentReg)
    : X86Operand(kMem, Ty), Base(Base), Offset(Offset), Index(Index),
      Shift(Shift), SegmentReg(SegmentReg), Randomized(false) {
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

void MachineTraits<TargetX8632>::X86OperandMem::emit(const Cfg *Func) const {
  if (!BuildDefs::dump())
    return;
  Ostream &Str = Func->getContext()->getStrEmit();
  if (SegmentReg != DefaultSegment) {
    assert(SegmentReg >= 0 && SegmentReg < SegReg_NUM);
    Str << "%" << X8632::Traits::InstSegmentRegNames[SegmentReg] << ":";
  }
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

void MachineTraits<TargetX8632>::X86OperandMem::dump(const Cfg *Func,
                                                     Ostream &Str) const {
  if (!BuildDefs::dump())
    return;
  if (SegmentReg != DefaultSegment) {
    assert(SegmentReg >= 0 && SegmentReg < SegReg_NUM);
    Str << X8632::Traits::InstSegmentRegNames[SegmentReg] << ":";
  }
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

void MachineTraits<TargetX8632>::X86OperandMem::emitSegmentOverride(
    MachineTraits<TargetX8632>::Assembler *Asm) const {
  if (SegmentReg != DefaultSegment) {
    assert(SegmentReg >= 0 && SegmentReg < SegReg_NUM);
    Asm->emitSegmentOverride(X8632::Traits::InstSegmentPrefixes[SegmentReg]);
  }
}

MachineTraits<TargetX8632>::Address
MachineTraits<TargetX8632>::X86OperandMem::toAsmAddress(
    MachineTraits<TargetX8632>::Assembler *Asm) const {
  int32_t Disp = 0;
  AssemblerFixup *Fixup = nullptr;
  // Determine the offset (is it relocatable?)
  if (getOffset()) {
    if (const auto CI = llvm::dyn_cast<ConstantInteger32>(getOffset())) {
      Disp = static_cast<int32_t>(CI->getValue());
    } else if (const auto CR =
                   llvm::dyn_cast<ConstantRelocatable>(getOffset())) {
      Disp = CR->getOffset();
      Fixup = Asm->createFixup(llvm::ELF::R_386_32, CR);
    } else {
      llvm_unreachable("Unexpected offset type");
    }
  }

  // Now convert to the various possible forms.
  if (getBase() && getIndex()) {
    return X8632::Traits::Address(
        RegX8632::getEncodedGPR(getBase()->getRegNum()),
        RegX8632::getEncodedGPR(getIndex()->getRegNum()),
        X8632::Traits::ScaleFactor(getShift()), Disp);
  } else if (getBase()) {
    return X8632::Traits::Address(
        RegX8632::getEncodedGPR(getBase()->getRegNum()), Disp);
  } else if (getIndex()) {
    return X8632::Traits::Address(
        RegX8632::getEncodedGPR(getIndex()->getRegNum()),
        X8632::Traits::ScaleFactor(getShift()), Disp);
  } else if (Fixup) {
    return X8632::Traits::Address::Absolute(Disp, Fixup);
  } else {
    return X8632::Traits::Address::Absolute(Disp);
  }
}

MachineTraits<TargetX8632>::Address
MachineTraits<TargetX8632>::VariableSplit::toAsmAddress(const Cfg *Func) const {
  assert(!Var->hasReg());
  const ::Ice::TargetLowering *Target = Func->getTarget();
  int32_t Offset =
      Var->getStackOffset() + Target->getStackAdjustment() + getOffset();
  return X8632::Traits::Address(
      RegX8632::getEncodedGPR(Target->getFrameOrStackReg()), Offset);
}

void MachineTraits<TargetX8632>::VariableSplit::emit(const Cfg *Func) const {
  if (!BuildDefs::dump())
    return;
  Ostream &Str = Func->getContext()->getStrEmit();
  assert(!Var->hasReg());
  // The following is copied/adapted from TargetX8632::emitVariable().
  const ::Ice::TargetLowering *Target = Func->getTarget();
  const Type Ty = IceType_i32;
  int32_t Offset =
      Var->getStackOffset() + Target->getStackAdjustment() + getOffset();
  if (Offset)
    Str << Offset;
  Str << "(%" << Target->getRegName(Target->getFrameOrStackReg(), Ty) << ")";
}

void MachineTraits<TargetX8632>::VariableSplit::dump(const Cfg *Func,
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

X86INSTS_DEFINE_STATIC_DATA(TargetX8632);
