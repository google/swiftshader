//===- subzero/src/IceInstX8632.cpp - X86-32 instruction implementation ---===//
//
//                        The Subzero Code Generator
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements the InstX8632 and OperandX8632 classes,
// primarily the constructors and the dump()/emit() methods.
//
//===----------------------------------------------------------------------===//

#include "assembler_ia32.h"
#include "IceCfg.h"
#include "IceCfgNode.h"
#include "IceConditionCodesX8632.h"
#include "IceInst.h"
#include "IceInstX8632.h"
#include "IceRegistersX8632.h"
#include "IceTargetLoweringX8632.h"
#include "IceOperand.h"

namespace Ice {

namespace {

const struct InstX8632BrAttributes_ {
  CondX86::BrCond Opposite;
  const char *DisplayString;
  const char *EmitString;
} InstX8632BrAttributes[] = {
#define X(tag, encode, opp, dump, emit)                                        \
  { CondX86::opp, dump, emit }                                                 \
  ,
    ICEINSTX8632BR_TABLE
#undef X
  };

const struct InstX8632CmppsAttributes_ {
  const char *EmitString;
} InstX8632CmppsAttributes[] = {
#define X(tag, emit)                                                           \
  { emit }                                                                     \
  ,
    ICEINSTX8632CMPPS_TABLE
#undef X
  };

const struct TypeX8632Attributes_ {
  const char *CvtString;   // i (integer), s (single FP), d (double FP)
  const char *SdSsString;  // ss, sd, or <blank>
  const char *PackString;  // b, w, d, or <blank>
  const char *WidthString; // {byte,word,dword,qword} ptr
} TypeX8632Attributes[] = {
#define X(tag, elementty, cvt, sdss, pack, width)                              \
  { cvt, "" sdss, pack, width }                                                \
  ,
    ICETYPEX8632_TABLE
#undef X
  };

const char *InstX8632SegmentRegNames[] = {
#define X(val, name, prefix) name,
    SEG_REGX8632_TABLE
#undef X
};

uint8_t InstX8632SegmentPrefixes[] = {
#define X(val, name, prefix) prefix,
    SEG_REGX8632_TABLE
#undef X
};

} // end of anonymous namespace

const char *InstX8632::getWidthString(Type Ty) {
  return TypeX8632Attributes[Ty].WidthString;
}

OperandX8632Mem::OperandX8632Mem(Cfg *Func, Type Ty, Variable *Base,
                                 Constant *Offset, Variable *Index,
                                 uint16_t Shift, SegmentRegisters SegmentReg)
    : OperandX8632(kMem, Ty), Base(Base), Offset(Offset), Index(Index),
      Shift(Shift), SegmentReg(SegmentReg) {
  assert(Shift <= 3);
  Vars = NULL;
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

InstX8632AdjustStack::InstX8632AdjustStack(Cfg *Func, SizeT Amount,
                                           Variable *Esp)
    : InstX8632(Func, InstX8632::Adjuststack, 1, Esp), Amount(Amount) {
  addSource(Esp);
}

InstX8632Mul::InstX8632Mul(Cfg *Func, Variable *Dest, Variable *Source1,
                           Operand *Source2)
    : InstX8632(Func, InstX8632::Mul, 2, Dest) {
  addSource(Source1);
  addSource(Source2);
}

InstX8632Shld::InstX8632Shld(Cfg *Func, Variable *Dest, Variable *Source1,
                             Variable *Source2)
    : InstX8632(Func, InstX8632::Shld, 3, Dest) {
  addSource(Dest);
  addSource(Source1);
  addSource(Source2);
}

InstX8632Shrd::InstX8632Shrd(Cfg *Func, Variable *Dest, Variable *Source1,
                             Variable *Source2)
    : InstX8632(Func, InstX8632::Shrd, 3, Dest) {
  addSource(Dest);
  addSource(Source1);
  addSource(Source2);
}

InstX8632Label::InstX8632Label(Cfg *Func, TargetX8632 *Target)
    : InstX8632(Func, InstX8632::Label, 0, NULL),
      Number(Target->makeNextLabelNumber()) {}

IceString InstX8632Label::getName(const Cfg *Func) const {
  char buf[30];
  snprintf(buf, llvm::array_lengthof(buf), "%u", Number);
  return ".L" + Func->getFunctionName() + "$local$__" + buf;
}

InstX8632Br::InstX8632Br(Cfg *Func, const CfgNode *TargetTrue,
                         const CfgNode *TargetFalse,
                         const InstX8632Label *Label, CondX86::BrCond Condition)
    : InstX8632(Func, InstX8632::Br, 0, NULL), Condition(Condition),
      TargetTrue(TargetTrue), TargetFalse(TargetFalse), Label(Label) {}

bool InstX8632Br::optimizeBranch(const CfgNode *NextNode) {
  // If there is no next block, then there can be no fallthrough to
  // optimize.
  if (NextNode == NULL)
    return false;
  // Intra-block conditional branches can't be optimized.
  if (Label)
    return false;
  // If there is no fallthrough node, such as a non-default case label
  // for a switch instruction, then there is no opportunity to
  // optimize.
  if (getTargetFalse() == NULL)
    return false;

  // Unconditional branch to the next node can be removed.
  if (Condition == CondX86::Br_None && getTargetFalse() == NextNode) {
    assert(getTargetTrue() == NULL);
    setDeleted();
    return true;
  }
  // If the fallthrough is to the next node, set fallthrough to NULL
  // to indicate.
  if (getTargetFalse() == NextNode) {
    TargetFalse = NULL;
    return true;
  }
  // If TargetTrue is the next node, and TargetFalse is non-NULL
  // (which was already tested above), then invert the branch
  // condition, swap the targets, and set new fallthrough to NULL.
  if (getTargetTrue() == NextNode) {
    assert(Condition != CondX86::Br_None);
    Condition = InstX8632BrAttributes[Condition].Opposite;
    TargetTrue = getTargetFalse();
    TargetFalse = NULL;
    return true;
  }
  return false;
}

InstX8632Call::InstX8632Call(Cfg *Func, Variable *Dest, Operand *CallTarget)
    : InstX8632(Func, InstX8632::Call, 1, Dest) {
  HasSideEffects = true;
  addSource(CallTarget);
}

InstX8632Cmov::InstX8632Cmov(Cfg *Func, Variable *Dest, Operand *Source,
                             CondX86::BrCond Condition)
    : InstX8632(Func, InstX8632::Cmov, 2, Dest), Condition(Condition) {
  // The final result is either the original Dest, or Source, so mark
  // both as sources.
  addSource(Dest);
  addSource(Source);
}

InstX8632Cmpps::InstX8632Cmpps(Cfg *Func, Variable *Dest, Operand *Source,
                               CondX86::CmppsCond Condition)
    : InstX8632(Func, InstX8632::Cmpps, 2, Dest), Condition(Condition) {
  addSource(Dest);
  addSource(Source);
}

InstX8632Cmpxchg::InstX8632Cmpxchg(Cfg *Func, Operand *DestOrAddr,
                                   Variable *Eax, Variable *Desired,
                                   bool Locked)
    : InstX8632Lockable(Func, InstX8632::Cmpxchg, 3,
                        llvm::dyn_cast<Variable>(DestOrAddr), Locked) {
  assert(Eax->getRegNum() == RegX8632::Reg_eax);
  addSource(DestOrAddr);
  addSource(Eax);
  addSource(Desired);
}

InstX8632Cmpxchg8b::InstX8632Cmpxchg8b(Cfg *Func, OperandX8632Mem *Addr,
                                       Variable *Edx, Variable *Eax,
                                       Variable *Ecx, Variable *Ebx,
                                       bool Locked)
    : InstX8632Lockable(Func, InstX8632::Cmpxchg, 5, NULL, Locked) {
  assert(Edx->getRegNum() == RegX8632::Reg_edx);
  assert(Eax->getRegNum() == RegX8632::Reg_eax);
  assert(Ecx->getRegNum() == RegX8632::Reg_ecx);
  assert(Ebx->getRegNum() == RegX8632::Reg_ebx);
  addSource(Addr);
  addSource(Edx);
  addSource(Eax);
  addSource(Ecx);
  addSource(Ebx);
}

InstX8632Cvt::InstX8632Cvt(Cfg *Func, Variable *Dest, Operand *Source,
                           CvtVariant Variant)
    : InstX8632(Func, InstX8632::Cvt, 1, Dest), Variant(Variant) {
  addSource(Source);
}

InstX8632Icmp::InstX8632Icmp(Cfg *Func, Operand *Src0, Operand *Src1)
    : InstX8632(Func, InstX8632::Icmp, 2, NULL) {
  addSource(Src0);
  addSource(Src1);
}

InstX8632Ucomiss::InstX8632Ucomiss(Cfg *Func, Operand *Src0, Operand *Src1)
    : InstX8632(Func, InstX8632::Ucomiss, 2, NULL) {
  addSource(Src0);
  addSource(Src1);
}

InstX8632UD2::InstX8632UD2(Cfg *Func)
    : InstX8632(Func, InstX8632::UD2, 0, NULL) {}

InstX8632Test::InstX8632Test(Cfg *Func, Operand *Src1, Operand *Src2)
    : InstX8632(Func, InstX8632::Test, 2, NULL) {
  addSource(Src1);
  addSource(Src2);
}

InstX8632Mfence::InstX8632Mfence(Cfg *Func)
    : InstX8632(Func, InstX8632::Mfence, 0, NULL) {
  HasSideEffects = true;
}

InstX8632Store::InstX8632Store(Cfg *Func, Operand *Value, OperandX8632 *Mem)
    : InstX8632(Func, InstX8632::Store, 2, NULL) {
  addSource(Value);
  addSource(Mem);
}

InstX8632StoreP::InstX8632StoreP(Cfg *Func, Variable *Value,
                                 OperandX8632Mem *Mem)
    : InstX8632(Func, InstX8632::StoreP, 2, NULL) {
  addSource(Value);
  addSource(Mem);
}

InstX8632StoreQ::InstX8632StoreQ(Cfg *Func, Variable *Value,
                                 OperandX8632Mem *Mem)
    : InstX8632(Func, InstX8632::StoreQ, 2, NULL) {
  addSource(Value);
  addSource(Mem);
}

InstX8632Movsx::InstX8632Movsx(Cfg *Func, Variable *Dest, Operand *Source)
    : InstX8632(Func, InstX8632::Movsx, 1, Dest) {
  addSource(Source);
}

InstX8632Movzx::InstX8632Movzx(Cfg *Func, Variable *Dest, Operand *Source)
    : InstX8632(Func, InstX8632::Movzx, 1, Dest) {
  addSource(Source);
}

InstX8632Nop::InstX8632Nop(Cfg *Func, InstX8632Nop::NopVariant Variant)
    : InstX8632(Func, InstX8632::Nop, 0, NULL), Variant(Variant) {}

InstX8632Fld::InstX8632Fld(Cfg *Func, Operand *Src)
    : InstX8632(Func, InstX8632::Fld, 1, NULL) {
  addSource(Src);
}

InstX8632Fstp::InstX8632Fstp(Cfg *Func, Variable *Dest)
    : InstX8632(Func, InstX8632::Fstp, 0, Dest) {}

InstX8632Pop::InstX8632Pop(Cfg *Func, Variable *Dest)
    : InstX8632(Func, InstX8632::Pop, 0, Dest) {}

InstX8632Push::InstX8632Push(Cfg *Func, Variable *Source)
    : InstX8632(Func, InstX8632::Push, 1, NULL) {
  addSource(Source);
}

InstX8632Ret::InstX8632Ret(Cfg *Func, Variable *Source)
    : InstX8632(Func, InstX8632::Ret, Source ? 1 : 0, NULL) {
  if (Source)
    addSource(Source);
}

InstX8632Xadd::InstX8632Xadd(Cfg *Func, Operand *Dest, Variable *Source,
                             bool Locked)
    : InstX8632Lockable(Func, InstX8632::Xadd, 2,
                        llvm::dyn_cast<Variable>(Dest), Locked) {
  addSource(Dest);
  addSource(Source);
}

InstX8632Xchg::InstX8632Xchg(Cfg *Func, Operand *Dest, Variable *Source)
    : InstX8632(Func, InstX8632::Xchg, 2, llvm::dyn_cast<Variable>(Dest)) {
  addSource(Dest);
  addSource(Source);
}

// ======================== Dump routines ======================== //

namespace {

void emitIASBytes(const Cfg *Func, const x86::AssemblerX86 *Asm,
                  intptr_t StartPosition) {
  GlobalContext *Ctx = Func->getContext();
  Ostream &Str = Ctx->getStrEmit();
  intptr_t EndPosition = Asm->GetPosition();
  intptr_t LastFixupLoc = -1;
  AssemblerFixup *LastFixup = NULL;
  if (Asm->GetLatestFixup()) {
    LastFixup = Asm->GetLatestFixup();
    LastFixupLoc = LastFixup->position();
  }
  if (LastFixupLoc < StartPosition) {
    // The fixup doesn't apply to this current block.
    for (intptr_t i = StartPosition; i < EndPosition; ++i) {
      Str << "\t.byte 0x";
      Str.write_hex(Asm->LoadBuffer<uint8_t>(i));
      Str << "\n";
    }
    return;
  }
  const intptr_t FixupSize = 4;
  assert(LastFixupLoc + FixupSize <= EndPosition);
  // The fixup does apply to this current block.
  for (intptr_t i = StartPosition; i < LastFixupLoc; ++i) {
    Str << "\t.byte 0x";
    Str.write_hex(Asm->LoadBuffer<uint8_t>(i));
    Str << "\n";
  }
  Str << "\t.long ";
  const ConstantRelocatable *Reloc = LastFixup->value();
  if (Reloc->getSuppressMangling())
    Str << Reloc->getName();
  else
    Str << Ctx->mangleName(Reloc->getName());
  if (LastFixup->value()->getOffset()) {
    Str << " + " << LastFixup->value()->getOffset();
  }
  Str << "\n";
  for (intptr_t i = LastFixupLoc + FixupSize; i < EndPosition; ++i) {
    Str << "\t.byte 0x";
    Str.write_hex(Asm->LoadBuffer<uint8_t>(i));
    Str << "\n";
  }
}

} // end of anonymous namespace

void InstX8632::dump(const Cfg *Func) const {
  Ostream &Str = Func->getContext()->getStrDump();
  Str << "[X8632] ";
  Inst::dump(Func);
}

void InstX8632Label::emit(const Cfg *Func) const {
  Ostream &Str = Func->getContext()->getStrEmit();
  Str << getName(Func) << ":\n";
}

void InstX8632Label::dump(const Cfg *Func) const {
  Ostream &Str = Func->getContext()->getStrDump();
  Str << getName(Func) << ":";
}

void InstX8632Br::emit(const Cfg *Func) const {
  Ostream &Str = Func->getContext()->getStrEmit();
  Str << "\t";

  if (Condition == CondX86::Br_None) {
    Str << "jmp";
  } else {
    Str << InstX8632BrAttributes[Condition].EmitString;
  }

  if (Label) {
    Str << "\t" << Label->getName(Func) << "\n";
  } else {
    if (Condition == CondX86::Br_None) {
      Str << "\t" << getTargetFalse()->getAsmName() << "\n";
    } else {
      Str << "\t" << getTargetTrue()->getAsmName() << "\n";
      if (getTargetFalse()) {
        Str << "\tjmp\t" << getTargetFalse()->getAsmName() << "\n";
      }
    }
  }
}

void InstX8632Br::dump(const Cfg *Func) const {
  Ostream &Str = Func->getContext()->getStrDump();
  Str << "br ";

  if (Condition == CondX86::Br_None) {
    Str << "label %"
        << (Label ? Label->getName(Func) : getTargetFalse()->getName());
    return;
  }

  Str << InstX8632BrAttributes[Condition].DisplayString;
  if (Label) {
    Str << ", label %" << Label->getName(Func);
  } else {
    Str << ", label %" << getTargetTrue()->getName();
    if (getTargetFalse()) {
      Str << ", label %" << getTargetFalse()->getName();
    }
  }
}

void InstX8632Call::emit(const Cfg *Func) const {
  Ostream &Str = Func->getContext()->getStrEmit();
  assert(getSrcSize() == 1);
  Str << "\tcall\t";
  getCallTarget()->emit(Func);
  Str << "\n";
  Func->getTarget()->resetStackAdjustment();
}

void InstX8632Call::dump(const Cfg *Func) const {
  Ostream &Str = Func->getContext()->getStrDump();
  if (getDest()) {
    dumpDest(Func);
    Str << " = ";
  }
  Str << "call ";
  getCallTarget()->dump(Func);
}

// The ShiftHack parameter is used to emit "cl" instead of "ecx" for
// shift instructions, in order to be syntactically valid.  The
// Opcode parameter needs to be char* and not IceString because of
// template issues.
void emitTwoAddress(const char *Opcode, const Inst *Inst, const Cfg *Func,
                    bool ShiftHack) {
  Ostream &Str = Func->getContext()->getStrEmit();
  assert(Inst->getSrcSize() == 2);
  assert(Inst->getDest() == Inst->getSrc(0));
  Str << "\t" << Opcode << "\t";
  Inst->getDest()->emit(Func);
  Str << ", ";
  bool EmittedSrc1 = false;
  if (ShiftHack) {
    Variable *ShiftReg = llvm::dyn_cast<Variable>(Inst->getSrc(1));
    if (ShiftReg && ShiftReg->getRegNum() == RegX8632::Reg_ecx) {
      Str << "cl";
      EmittedSrc1 = true;
    }
  }
  if (!EmittedSrc1)
    Inst->getSrc(1)->emit(Func);
  Str << "\n";
}

void emitIASOpTyGPR(const Cfg *Func, Type Ty, const Operand *Op,
                    const x86::AssemblerX86::GPREmitterOneOp &Emitter) {
  x86::AssemblerX86 *Asm = Func->getAssembler<x86::AssemblerX86>();
  intptr_t StartPosition = Asm->GetPosition();
  if (const auto Var = llvm::dyn_cast<Variable>(Op)) {
    if (Var->hasReg()) {
      // We cheat a little and use GPRRegister even for byte operations.
      RegX8632::GPRRegister VarReg =
          RegX8632::getEncodedByteRegOrGPR(Ty, Var->getRegNum());
      (Asm->*(Emitter.Reg))(Ty, VarReg);
    } else {
      x86::Address StackAddr(static_cast<TargetX8632 *>(Func->getTarget())
                                 ->stackVarToAsmOperand(Var));
      (Asm->*(Emitter.Addr))(Ty, StackAddr);
    }
  } else if (const auto Mem = llvm::dyn_cast<OperandX8632Mem>(Op)) {
    Mem->emitSegmentOverride(Asm);
    (Asm->*(Emitter.Addr))(Ty, Mem->toAsmAddress(Asm));
  } else {
    llvm_unreachable("Unexpected operand type");
  }
  emitIASBytes(Func, Asm, StartPosition);
}

void emitIASRegOpTyGPR(const Cfg *Func, Type Ty, const Variable *Var,
                       const Operand *Src,
                       const x86::AssemblerX86::GPREmitterRegOp &Emitter) {
  x86::AssemblerX86 *Asm = Func->getAssembler<x86::AssemblerX86>();
  intptr_t StartPosition = Asm->GetPosition();
  assert(Var->hasReg());
  // We cheat a little and use GPRRegister even for byte operations.
  RegX8632::GPRRegister VarReg =
      RegX8632::getEncodedByteRegOrGPR(Ty, Var->getRegNum());
  if (const auto SrcVar = llvm::dyn_cast<Variable>(Src)) {
    if (SrcVar->hasReg()) {
      RegX8632::GPRRegister SrcReg =
          RegX8632::getEncodedByteRegOrGPR(Ty, SrcVar->getRegNum());
      (Asm->*(Emitter.GPRGPR))(Ty, VarReg, SrcReg);
    } else {
      x86::Address SrcStackAddr = static_cast<TargetX8632 *>(Func->getTarget())
                                      ->stackVarToAsmOperand(SrcVar);
      (Asm->*(Emitter.GPRAddr))(Ty, VarReg, SrcStackAddr);
    }
  } else if (const auto Mem = llvm::dyn_cast<OperandX8632Mem>(Src)) {
    Mem->emitSegmentOverride(Asm);
    (Asm->*(Emitter.GPRAddr))(Ty, VarReg, Mem->toAsmAddress(Asm));
  } else if (const auto Imm = llvm::dyn_cast<ConstantInteger32>(Src)) {
    (Asm->*(Emitter.GPRImm))(Ty, VarReg, x86::Immediate(Imm->getValue()));
  } else if (const auto Reloc = llvm::dyn_cast<ConstantRelocatable>(Src)) {
    AssemblerFixup *Fixup =
        x86::DisplacementRelocation::create(Asm, FK_Abs_4, Reloc);
    (Asm->*(Emitter.GPRImm))(Ty, VarReg, x86::Immediate(Fixup));
  } else if (const auto Split = llvm::dyn_cast<VariableSplit>(Src)) {
    x86::Address SrcAddr = Split->toAsmAddress(Func);
    (Asm->*(Emitter.GPRAddr))(Ty, VarReg, SrcAddr);
  } else {
    llvm_unreachable("Unexpected operand type");
  }
  emitIASBytes(Func, Asm, StartPosition);
}

void emitIASAddrOpTyGPR(const Cfg *Func, Type Ty, const x86::Address &Addr,
                        const Operand *Src,
                        const x86::AssemblerX86::GPREmitterAddrOp &Emitter) {
  x86::AssemblerX86 *Asm = Func->getAssembler<x86::AssemblerX86>();
  intptr_t StartPosition = Asm->GetPosition();
  // Src can only be Reg or Immediate.
  if (const auto SrcVar = llvm::dyn_cast<Variable>(Src)) {
    assert(SrcVar->hasReg());
    RegX8632::GPRRegister SrcReg =
        RegX8632::getEncodedByteRegOrGPR(Ty, SrcVar->getRegNum());
    (Asm->*(Emitter.AddrGPR))(Ty, Addr, SrcReg);
  } else if (const auto Imm = llvm::dyn_cast<ConstantInteger32>(Src)) {
    (Asm->*(Emitter.AddrImm))(Ty, Addr, x86::Immediate(Imm->getValue()));
  } else if (const auto Reloc = llvm::dyn_cast<ConstantRelocatable>(Src)) {
    AssemblerFixup *Fixup =
        x86::DisplacementRelocation::create(Asm, FK_Abs_4, Reloc);
    (Asm->*(Emitter.AddrImm))(Ty, Addr, x86::Immediate(Fixup));
  } else {
    llvm_unreachable("Unexpected operand type");
  }
  emitIASBytes(Func, Asm, StartPosition);
}

void emitIASGPRShift(const Cfg *Func, Type Ty, const Variable *Var,
                     const Operand *Src,
                     const x86::AssemblerX86::GPREmitterShiftOp &Emitter) {
  x86::AssemblerX86 *Asm = Func->getAssembler<x86::AssemblerX86>();
  intptr_t StartPosition = Asm->GetPosition();
  // Technically, the Dest Var can be mem as well, but we only use Reg.
  // We can extend this to check Dest if we decide to use that form.
  assert(Var->hasReg());
  // We cheat a little and use GPRRegister even for byte operations.
  RegX8632::GPRRegister VarReg =
      RegX8632::getEncodedByteRegOrGPR(Ty, Var->getRegNum());
  // Src must be reg == ECX or an Imm8.
  // This is asserted by the assembler.
  if (const auto SrcVar = llvm::dyn_cast<Variable>(Src)) {
    assert(SrcVar->hasReg());
    RegX8632::GPRRegister SrcReg =
        RegX8632::getEncodedByteRegOrGPR(Ty, SrcVar->getRegNum());
    (Asm->*(Emitter.GPRGPR))(Ty, VarReg, SrcReg);
  } else if (const auto Imm = llvm::dyn_cast<ConstantInteger32>(Src)) {
    (Asm->*(Emitter.GPRImm))(Ty, VarReg, x86::Immediate(Imm->getValue()));
  } else {
    llvm_unreachable("Unexpected operand type");
  }
  emitIASBytes(Func, Asm, StartPosition);
}

void emitIASXmmShift(const Cfg *Func, Type Ty, const Variable *Var,
                     const Operand *Src,
                     const x86::AssemblerX86::XmmEmitterShiftOp &Emitter) {
  x86::AssemblerX86 *Asm = Func->getAssembler<x86::AssemblerX86>();
  intptr_t StartPosition = Asm->GetPosition();
  assert(Var->hasReg());
  RegX8632::XmmRegister VarReg = RegX8632::getEncodedXmm(Var->getRegNum());
  if (const auto SrcVar = llvm::dyn_cast<Variable>(Src)) {
    if (SrcVar->hasReg()) {
      RegX8632::XmmRegister SrcReg =
          RegX8632::getEncodedXmm(SrcVar->getRegNum());
      (Asm->*(Emitter.XmmXmm))(Ty, VarReg, SrcReg);
    } else {
      x86::Address SrcStackAddr = static_cast<TargetX8632 *>(Func->getTarget())
                                      ->stackVarToAsmOperand(SrcVar);
      (Asm->*(Emitter.XmmAddr))(Ty, VarReg, SrcStackAddr);
    }
  } else if (const auto Mem = llvm::dyn_cast<OperandX8632Mem>(Src)) {
    assert(Mem->getSegmentRegister() == OperandX8632Mem::DefaultSegment);
    (Asm->*(Emitter.XmmAddr))(Ty, VarReg, Mem->toAsmAddress(Asm));
  } else if (const auto Imm = llvm::dyn_cast<ConstantInteger32>(Src)) {
    (Asm->*(Emitter.XmmImm))(Ty, VarReg, x86::Immediate(Imm->getValue()));
  } else {
    llvm_unreachable("Unexpected operand type");
  }
  emitIASBytes(Func, Asm, StartPosition);
}

void emitIASRegOpTyXMM(const Cfg *Func, Type Ty, const Variable *Var,
                       const Operand *Src,
                       const x86::AssemblerX86::XmmEmitterRegOp &Emitter) {
  x86::AssemblerX86 *Asm = Func->getAssembler<x86::AssemblerX86>();
  intptr_t StartPosition = Asm->GetPosition();
  assert(Var->hasReg());
  RegX8632::XmmRegister VarReg = RegX8632::getEncodedXmm(Var->getRegNum());
  if (const auto SrcVar = llvm::dyn_cast<Variable>(Src)) {
    if (SrcVar->hasReg()) {
      RegX8632::XmmRegister SrcReg =
          RegX8632::getEncodedXmm(SrcVar->getRegNum());
      (Asm->*(Emitter.XmmXmm))(Ty, VarReg, SrcReg);
    } else {
      x86::Address SrcStackAddr = static_cast<TargetX8632 *>(Func->getTarget())
                                      ->stackVarToAsmOperand(SrcVar);
      (Asm->*(Emitter.XmmAddr))(Ty, VarReg, SrcStackAddr);
    }
  } else if (const auto Mem = llvm::dyn_cast<OperandX8632Mem>(Src)) {
    assert(Mem->getSegmentRegister() == OperandX8632Mem::DefaultSegment);
    (Asm->*(Emitter.XmmAddr))(Ty, VarReg, Mem->toAsmAddress(Asm));
  } else if (const auto Imm = llvm::dyn_cast<Constant>(Src)) {
    (Asm->*(Emitter.XmmAddr))(
        Ty, VarReg, x86::Address::ofConstPool(Func->getContext(), Asm, Imm));
  } else {
    llvm_unreachable("Unexpected operand type");
  }
  emitIASBytes(Func, Asm, StartPosition);
}

template <typename DReg_t, typename SReg_t, DReg_t (*destEnc)(int32_t),
          SReg_t (*srcEnc)(int32_t)>
void emitIASCastRegOp(
    const Cfg *Func, Type DispatchTy, const Variable *Dest, const Operand *Src,
    const x86::AssemblerX86::CastEmitterRegOp<DReg_t, SReg_t> Emitter) {
  x86::AssemblerX86 *Asm = Func->getAssembler<x86::AssemblerX86>();
  intptr_t StartPosition = Asm->GetPosition();
  assert(Dest->hasReg());
  DReg_t DestReg = destEnc(Dest->getRegNum());
  if (const auto SrcVar = llvm::dyn_cast<Variable>(Src)) {
    if (SrcVar->hasReg()) {
      SReg_t SrcReg = srcEnc(SrcVar->getRegNum());
      (Asm->*(Emitter.RegReg))(DispatchTy, DestReg, SrcReg);
    } else {
      x86::Address SrcStackAddr = static_cast<TargetX8632 *>(Func->getTarget())
                                      ->stackVarToAsmOperand(SrcVar);
      (Asm->*(Emitter.RegAddr))(DispatchTy, DestReg, SrcStackAddr);
    }
  } else if (const auto Mem = llvm::dyn_cast<OperandX8632Mem>(Src)) {
    Mem->emitSegmentOverride(Asm);
    (Asm->*(Emitter.RegAddr))(DispatchTy, DestReg, Mem->toAsmAddress(Asm));
  } else {
    llvm_unreachable("Unexpected operand type");
  }
  emitIASBytes(Func, Asm, StartPosition);
}

void emitIASMovlikeXMM(const Cfg *Func, const Variable *Dest,
                       const Operand *Src,
                       const x86::AssemblerX86::XmmEmitterMovOps Emitter) {
  x86::AssemblerX86 *Asm = Func->getAssembler<x86::AssemblerX86>();
  intptr_t StartPosition = Asm->GetPosition();
  if (Dest->hasReg()) {
    RegX8632::XmmRegister DestReg = RegX8632::getEncodedXmm(Dest->getRegNum());
    if (const auto SrcVar = llvm::dyn_cast<Variable>(Src)) {
      if (SrcVar->hasReg()) {
        (Asm->*(Emitter.XmmXmm))(DestReg,
                                 RegX8632::getEncodedXmm(SrcVar->getRegNum()));
      } else {
        x86::Address StackAddr(static_cast<TargetX8632 *>(Func->getTarget())
                                   ->stackVarToAsmOperand(SrcVar));
        (Asm->*(Emitter.XmmAddr))(DestReg, StackAddr);
      }
    } else if (const auto SrcMem = llvm::dyn_cast<OperandX8632Mem>(Src)) {
      assert(SrcMem->getSegmentRegister() == OperandX8632Mem::DefaultSegment);
      (Asm->*(Emitter.XmmAddr))(DestReg, SrcMem->toAsmAddress(Asm));
    } else {
      llvm_unreachable("Unexpected operand type");
    }
  } else {
    x86::Address StackAddr(static_cast<TargetX8632 *>(Func->getTarget())
                               ->stackVarToAsmOperand(Dest));
    // Src must be a register in this case.
    const Variable *SrcVar = llvm::cast<Variable>(Src);
    assert(SrcVar->hasReg());
    (Asm->*(Emitter.AddrXmm))(StackAddr,
                              RegX8632::getEncodedXmm(SrcVar->getRegNum()));
  }
  emitIASBytes(Func, Asm, StartPosition);
}

bool checkForRedundantAssign(const Variable *Dest, const Operand *Source) {
  const Variable *Src = llvm::dyn_cast<const Variable>(Source);
  if (Src == NULL)
    return false;
  if (Dest->hasReg() && Dest->getRegNum() == Src->getRegNum()) {
    // TODO: On x86-64, instructions like "mov eax, eax" are used to
    // clear the upper 32 bits of rax.  We need to recognize and
    // preserve these.
    return true;
  }
  if (!Dest->hasReg() && !Src->hasReg() &&
      Dest->getStackOffset() == Src->getStackOffset())
    return true;
  return false;
}

// In-place ops
template <> const char *InstX8632Bswap::Opcode = "bswap";
template <> const char *InstX8632Neg::Opcode = "neg";
// Unary ops
template <> const char *InstX8632Bsf::Opcode = "bsf";
template <> const char *InstX8632Bsr::Opcode = "bsr";
template <> const char *InstX8632Lea::Opcode = "lea";
template <> const char *InstX8632Movd::Opcode = "movd";
template <> const char *InstX8632Sqrtss::Opcode = "sqrtss";
template <> const char *InstX8632Cbwdq::Opcode = "cbw/cwd/cdq";
// Mov-like ops
template <> const char *InstX8632Mov::Opcode = "mov";
template <> const char *InstX8632Movp::Opcode = "movups";
template <> const char *InstX8632Movq::Opcode = "movq";
// Binary ops
template <> const char *InstX8632Add::Opcode = "add";
template <> const char *InstX8632Addps::Opcode = "addps";
template <> const char *InstX8632Adc::Opcode = "adc";
template <> const char *InstX8632Addss::Opcode = "addss";
template <> const char *InstX8632Padd::Opcode = "padd";
template <> const char *InstX8632Sub::Opcode = "sub";
template <> const char *InstX8632Subps::Opcode = "subps";
template <> const char *InstX8632Subss::Opcode = "subss";
template <> const char *InstX8632Sbb::Opcode = "sbb";
template <> const char *InstX8632Psub::Opcode = "psub";
template <> const char *InstX8632And::Opcode = "and";
template <> const char *InstX8632Pand::Opcode = "pand";
template <> const char *InstX8632Pandn::Opcode = "pandn";
template <> const char *InstX8632Or::Opcode = "or";
template <> const char *InstX8632Por::Opcode = "por";
template <> const char *InstX8632Xor::Opcode = "xor";
template <> const char *InstX8632Pxor::Opcode = "pxor";
template <> const char *InstX8632Imul::Opcode = "imul";
template <> const char *InstX8632Mulps::Opcode = "mulps";
template <> const char *InstX8632Mulss::Opcode = "mulss";
template <> const char *InstX8632Pmull::Opcode = "pmull";
template <> const char *InstX8632Pmuludq::Opcode = "pmuludq";
template <> const char *InstX8632Div::Opcode = "div";
template <> const char *InstX8632Divps::Opcode = "divps";
template <> const char *InstX8632Idiv::Opcode = "idiv";
template <> const char *InstX8632Divss::Opcode = "divss";
template <> const char *InstX8632Rol::Opcode = "rol";
template <> const char *InstX8632Shl::Opcode = "shl";
template <> const char *InstX8632Psll::Opcode = "psll";
template <> const char *InstX8632Shr::Opcode = "shr";
template <> const char *InstX8632Sar::Opcode = "sar";
template <> const char *InstX8632Psra::Opcode = "psra";
template <> const char *InstX8632Pcmpeq::Opcode = "pcmpeq";
template <> const char *InstX8632Pcmpgt::Opcode = "pcmpgt";
template <> const char *InstX8632MovssRegs::Opcode = "movss";
// Ternary ops
template <> const char *InstX8632Insertps::Opcode = "insertps";
template <> const char *InstX8632Shufps::Opcode = "shufps";
template <> const char *InstX8632Pinsr::Opcode = "pinsr";
template <> const char *InstX8632Blendvps::Opcode = "blendvps";
template <> const char *InstX8632Pblendvb::Opcode = "pblendvb";
// Three address ops
template <> const char *InstX8632Pextr::Opcode = "pextr";
template <> const char *InstX8632Pshufd::Opcode = "pshufd";

// Inplace GPR ops
template <>
const x86::AssemblerX86::GPREmitterOneOp InstX8632Bswap::Emitter = {
    &x86::AssemblerX86::bswap, NULL /* only a reg form exists */};
template <>
const x86::AssemblerX86::GPREmitterOneOp InstX8632Neg::Emitter = {
    &x86::AssemblerX86::neg, &x86::AssemblerX86::neg};

// Unary GPR ops
template <>
const x86::AssemblerX86::GPREmitterRegOp InstX8632Bsf::Emitter = {
    &x86::AssemblerX86::bsf, &x86::AssemblerX86::bsf, NULL};
template <>
const x86::AssemblerX86::GPREmitterRegOp InstX8632Bsr::Emitter = {
    &x86::AssemblerX86::bsr, &x86::AssemblerX86::bsr, NULL};
template <>
const x86::AssemblerX86::GPREmitterRegOp InstX8632Lea::Emitter = {
    /* reg/reg and reg/imm are illegal */ NULL, &x86::AssemblerX86::lea, NULL};

// Unary XMM ops
template <>
const x86::AssemblerX86::XmmEmitterRegOp InstX8632Sqrtss::Emitter = {
  &x86::AssemblerX86::sqrtss, &x86::AssemblerX86::sqrtss
};

// Binary GPR ops
template <>
const x86::AssemblerX86::GPREmitterRegOp InstX8632Add::Emitter = {
    &x86::AssemblerX86::add, &x86::AssemblerX86::add, &x86::AssemblerX86::add};
template <>
const x86::AssemblerX86::GPREmitterRegOp InstX8632Adc::Emitter = {
    &x86::AssemblerX86::adc, &x86::AssemblerX86::adc, &x86::AssemblerX86::adc};
template <>
const x86::AssemblerX86::GPREmitterRegOp InstX8632And::Emitter = {
    &x86::AssemblerX86::And, &x86::AssemblerX86::And, &x86::AssemblerX86::And};
template <>
const x86::AssemblerX86::GPREmitterRegOp InstX8632Or::Emitter = {
    &x86::AssemblerX86::Or, &x86::AssemblerX86::Or, &x86::AssemblerX86::Or};
template <>
const x86::AssemblerX86::GPREmitterRegOp InstX8632Sbb::Emitter = {
    &x86::AssemblerX86::sbb, &x86::AssemblerX86::sbb, &x86::AssemblerX86::sbb};
template <>
const x86::AssemblerX86::GPREmitterRegOp InstX8632Sub::Emitter = {
    &x86::AssemblerX86::sub, &x86::AssemblerX86::sub, &x86::AssemblerX86::sub};
template <>
const x86::AssemblerX86::GPREmitterRegOp InstX8632Xor::Emitter = {
    &x86::AssemblerX86::Xor, &x86::AssemblerX86::Xor, &x86::AssemblerX86::Xor};

// Binary Shift GPR ops
template <>
const x86::AssemblerX86::GPREmitterShiftOp InstX8632Rol::Emitter = {
    &x86::AssemblerX86::rol, &x86::AssemblerX86::rol};
template <>
const x86::AssemblerX86::GPREmitterShiftOp InstX8632Sar::Emitter = {
    &x86::AssemblerX86::sar, &x86::AssemblerX86::sar};
template <>
const x86::AssemblerX86::GPREmitterShiftOp InstX8632Shl::Emitter = {
    &x86::AssemblerX86::shl, &x86::AssemblerX86::shl};
template <>
const x86::AssemblerX86::GPREmitterShiftOp InstX8632Shr::Emitter = {
    &x86::AssemblerX86::shr, &x86::AssemblerX86::shr};

// Binary XMM ops
template <>
const x86::AssemblerX86::XmmEmitterRegOp InstX8632Addss::Emitter = {
  &x86::AssemblerX86::addss, &x86::AssemblerX86::addss
};
template <>
const x86::AssemblerX86::XmmEmitterRegOp InstX8632Addps::Emitter = {
  &x86::AssemblerX86::addps, &x86::AssemblerX86::addps
};
template <>
const x86::AssemblerX86::XmmEmitterRegOp InstX8632Divss::Emitter = {
  &x86::AssemblerX86::divss, &x86::AssemblerX86::divss
};
template <>
const x86::AssemblerX86::XmmEmitterRegOp InstX8632Divps::Emitter = {
  &x86::AssemblerX86::divps, &x86::AssemblerX86::divps
};
template <>
const x86::AssemblerX86::XmmEmitterRegOp InstX8632Mulss::Emitter = {
  &x86::AssemblerX86::mulss, &x86::AssemblerX86::mulss
};
template <>
const x86::AssemblerX86::XmmEmitterRegOp InstX8632Mulps::Emitter = {
  &x86::AssemblerX86::mulps, &x86::AssemblerX86::mulps
};
template <>
const x86::AssemblerX86::XmmEmitterRegOp InstX8632Padd::Emitter = {
  &x86::AssemblerX86::padd, &x86::AssemblerX86::padd
};
template <>
const x86::AssemblerX86::XmmEmitterRegOp InstX8632Pand::Emitter = {
  &x86::AssemblerX86::pand, &x86::AssemblerX86::pand
};
template <>
const x86::AssemblerX86::XmmEmitterRegOp InstX8632Pandn::Emitter = {
  &x86::AssemblerX86::pandn, &x86::AssemblerX86::pandn
};
template <>
const x86::AssemblerX86::XmmEmitterRegOp InstX8632Pcmpeq::Emitter = {
  &x86::AssemblerX86::pcmpeq, &x86::AssemblerX86::pcmpeq
};
template <>
const x86::AssemblerX86::XmmEmitterRegOp InstX8632Pcmpgt::Emitter = {
  &x86::AssemblerX86::pcmpgt, &x86::AssemblerX86::pcmpgt
};
template <>
const x86::AssemblerX86::XmmEmitterRegOp InstX8632Pmull::Emitter = {
  &x86::AssemblerX86::pmull, &x86::AssemblerX86::pmull
};
template <>
const x86::AssemblerX86::XmmEmitterRegOp InstX8632Pmuludq::Emitter = {
  &x86::AssemblerX86::pmuludq, &x86::AssemblerX86::pmuludq
};
template <>
const x86::AssemblerX86::XmmEmitterRegOp InstX8632Por::Emitter = {
  &x86::AssemblerX86::por, &x86::AssemblerX86::por
};
template <>
const x86::AssemblerX86::XmmEmitterRegOp InstX8632Psub::Emitter = {
  &x86::AssemblerX86::psub, &x86::AssemblerX86::psub
};
template <>
const x86::AssemblerX86::XmmEmitterRegOp InstX8632Pxor::Emitter = {
  &x86::AssemblerX86::pxor, &x86::AssemblerX86::pxor
};
template <>
const x86::AssemblerX86::XmmEmitterRegOp InstX8632Subss::Emitter = {
  &x86::AssemblerX86::subss, &x86::AssemblerX86::subss
};
template <>
const x86::AssemblerX86::XmmEmitterRegOp InstX8632Subps::Emitter = {
  &x86::AssemblerX86::subps, &x86::AssemblerX86::subps
};

// Binary XMM Shift ops
template <>
const x86::AssemblerX86::XmmEmitterShiftOp InstX8632Psll::Emitter = {
    &x86::AssemblerX86::psll, &x86::AssemblerX86::psll,
    &x86::AssemblerX86::psll};
template <>
const x86::AssemblerX86::XmmEmitterShiftOp InstX8632Psra::Emitter = {
    &x86::AssemblerX86::psra, &x86::AssemblerX86::psra,
    &x86::AssemblerX86::psra};

template <> void InstX8632Sqrtss::emit(const Cfg *Func) const {
  Ostream &Str = Func->getContext()->getStrEmit();
  assert(getSrcSize() == 1);
  Type Ty = getSrc(0)->getType();
  assert(isScalarFloatingType(Ty));
  Str << "\tsqrt" << TypeX8632Attributes[Ty].SdSsString << "\t";
  getDest()->emit(Func);
  Str << ", ";
  getSrc(0)->emit(Func);
  Str << "\n";
}

template <> void InstX8632Addss::emit(const Cfg *Func) const {
  char buf[30];
  snprintf(buf, llvm::array_lengthof(buf), "add%s",
           TypeX8632Attributes[getDest()->getType()].SdSsString);
  emitTwoAddress(buf, this, Func);
}

template <> void InstX8632Padd::emit(const Cfg *Func) const {
  char buf[30];
  snprintf(buf, llvm::array_lengthof(buf), "padd%s",
           TypeX8632Attributes[getDest()->getType()].PackString);
  emitTwoAddress(buf, this, Func);
}

template <> void InstX8632Pmull::emit(const Cfg *Func) const {
  char buf[30];
  bool TypesAreValid = getDest()->getType() == IceType_v4i32 ||
                       getDest()->getType() == IceType_v8i16;
  bool InstructionSetIsValid =
      getDest()->getType() == IceType_v8i16 ||
      static_cast<TargetX8632 *>(Func->getTarget())->getInstructionSet() >=
          TargetX8632::SSE4_1;
  (void)TypesAreValid;
  (void)InstructionSetIsValid;
  assert(TypesAreValid);
  assert(InstructionSetIsValid);
  snprintf(buf, llvm::array_lengthof(buf), "pmull%s",
           TypeX8632Attributes[getDest()->getType()].PackString);
  emitTwoAddress(buf, this, Func);
}

template <> void InstX8632Pmull::emitIAS(const Cfg *Func) const {
  Type Ty = getDest()->getType();
  bool TypesAreValid = Ty == IceType_v4i32 || Ty == IceType_v8i16;
  bool InstructionSetIsValid =
      Ty == IceType_v8i16 ||
      static_cast<TargetX8632 *>(Func->getTarget())->getInstructionSet() >=
          TargetX8632::SSE4_1;
  (void)TypesAreValid;
  (void)InstructionSetIsValid;
  assert(TypesAreValid);
  assert(InstructionSetIsValid);
  assert(getSrcSize() == 2);
  Type ElementTy = typeElementType(Ty);
  emitIASRegOpTyXMM(Func, ElementTy, getDest(), getSrc(1), Emitter);
}

template <> void InstX8632Subss::emit(const Cfg *Func) const {
  char buf[30];
  snprintf(buf, llvm::array_lengthof(buf), "sub%s",
           TypeX8632Attributes[getDest()->getType()].SdSsString);
  emitTwoAddress(buf, this, Func);
}

template <> void InstX8632Psub::emit(const Cfg *Func) const {
  char buf[30];
  snprintf(buf, llvm::array_lengthof(buf), "psub%s",
           TypeX8632Attributes[getDest()->getType()].PackString);
  emitTwoAddress(buf, this, Func);
}

template <> void InstX8632Mulss::emit(const Cfg *Func) const {
  char buf[30];
  snprintf(buf, llvm::array_lengthof(buf), "mul%s",
           TypeX8632Attributes[getDest()->getType()].SdSsString);
  emitTwoAddress(buf, this, Func);
}

template <> void InstX8632Pmuludq::emit(const Cfg *Func) const {
  assert(getSrc(0)->getType() == IceType_v4i32 &&
         getSrc(1)->getType() == IceType_v4i32);
  emitTwoAddress(Opcode, this, Func);
}

template <> void InstX8632Divss::emit(const Cfg *Func) const {
  char buf[30];
  snprintf(buf, llvm::array_lengthof(buf), "div%s",
           TypeX8632Attributes[getDest()->getType()].SdSsString);
  emitTwoAddress(buf, this, Func);
}

template <> void InstX8632Div::emit(const Cfg *Func) const {
  Ostream &Str = Func->getContext()->getStrEmit();
  assert(getSrcSize() == 3);
  Str << "\t" << Opcode << "\t";
  getSrc(1)->emit(Func);
  Str << "\n";
}

template <> void InstX8632Div::emitIAS(const Cfg *Func) const {
  assert(getSrcSize() == 3);
  const Operand *Src = getSrc(1);
  Type Ty = Src->getType();
  const static x86::AssemblerX86::GPREmitterOneOp Emitter = {
      &x86::AssemblerX86::div, &x86::AssemblerX86::div};
  emitIASOpTyGPR(Func, Ty, Src, Emitter);
}

template <> void InstX8632Idiv::emit(const Cfg *Func) const {
  Ostream &Str = Func->getContext()->getStrEmit();
  assert(getSrcSize() == 3);
  Str << "\t" << Opcode << "\t";
  getSrc(1)->emit(Func);
  Str << "\n";
}

template <> void InstX8632Idiv::emitIAS(const Cfg *Func) const {
  assert(getSrcSize() == 3);
  const Operand *Src = getSrc(1);
  Type Ty = Src->getType();
  const static x86::AssemblerX86::GPREmitterOneOp Emitter = {
      &x86::AssemblerX86::idiv, &x86::AssemblerX86::idiv};
  emitIASOpTyGPR(Func, Ty, Src, Emitter);
}

namespace {

// pblendvb and blendvps take xmm0 as a final implicit argument.
void emitVariableBlendInst(const char *Opcode, const Inst *Inst,
                           const Cfg *Func) {
  Ostream &Str = Func->getContext()->getStrEmit();
  assert(Inst->getSrcSize() == 3);
  assert(llvm::cast<Variable>(Inst->getSrc(2))->getRegNum() ==
         RegX8632::Reg_xmm0);
  Str << "\t" << Opcode << "\t";
  Inst->getDest()->emit(Func);
  Str << ", ";
  Inst->getSrc(1)->emit(Func);
  Str << "\n";
}

void
emitIASVariableBlendInst(const Inst *Inst, const Cfg *Func,
                         const x86::AssemblerX86::XmmEmitterRegOp &Emitter) {
  assert(Inst->getSrcSize() == 3);
  assert(llvm::cast<Variable>(Inst->getSrc(2))->getRegNum() ==
         RegX8632::Reg_xmm0);
  const Variable *Dest = Inst->getDest();
  const Operand *Src = Inst->getSrc(1);
  emitIASRegOpTyXMM(Func, Dest->getType(), Dest, Src, Emitter);
}

} // end anonymous namespace

template <> void InstX8632Blendvps::emit(const Cfg *Func) const {
  assert(static_cast<TargetX8632 *>(Func->getTarget())->getInstructionSet() >=
         TargetX8632::SSE4_1);
  emitVariableBlendInst(Opcode, this, Func);
}

template <> void InstX8632Blendvps::emitIAS(const Cfg *Func) const {
  assert(static_cast<TargetX8632 *>(Func->getTarget())->getInstructionSet() >=
         TargetX8632::SSE4_1);
  static const x86::AssemblerX86::XmmEmitterRegOp Emitter = {
      &x86::AssemblerX86::blendvps, &x86::AssemblerX86::blendvps};
  emitIASVariableBlendInst(this, Func, Emitter);
}

template <> void InstX8632Pblendvb::emit(const Cfg *Func) const {
  assert(static_cast<TargetX8632 *>(Func->getTarget())->getInstructionSet() >=
         TargetX8632::SSE4_1);
  emitVariableBlendInst(Opcode, this, Func);
}

template <> void InstX8632Pblendvb::emitIAS(const Cfg *Func) const {
  assert(static_cast<TargetX8632 *>(Func->getTarget())->getInstructionSet() >=
         TargetX8632::SSE4_1);
  static const x86::AssemblerX86::XmmEmitterRegOp Emitter = {
      &x86::AssemblerX86::pblendvb, &x86::AssemblerX86::pblendvb};
  emitIASVariableBlendInst(this, Func, Emitter);
}

template <> void InstX8632Imul::emit(const Cfg *Func) const {
  Ostream &Str = Func->getContext()->getStrEmit();
  assert(getSrcSize() == 2);
  if (isByteSizedArithType(getDest()->getType())) {
    // The 8-bit version of imul only allows the form "imul r/m8".
    Variable *Src0 = llvm::dyn_cast<Variable>(getSrc(0));
    (void)Src0;
    assert(Src0 && Src0->getRegNum() == RegX8632::Reg_eax);
    Str << "\timul\t";
    getSrc(1)->emit(Func);
    Str << "\n";
  } else if (llvm::isa<Constant>(getSrc(1))) {
    Str << "\timul\t";
    getDest()->emit(Func);
    Str << ", ";
    getSrc(0)->emit(Func);
    Str << ", ";
    getSrc(1)->emit(Func);
    Str << "\n";
  } else {
    emitTwoAddress("imul", this, Func);
  }
}

template <> void InstX8632Imul::emitIAS(const Cfg *Func) const {
  assert(getSrcSize() == 2);
  const Variable *Var = getDest();
  Type Ty = Var->getType();
  const Operand *Src = getSrc(1);
  if (isByteSizedArithType(Ty)) {
    // The 8-bit version of imul only allows the form "imul r/m8".
    Variable *Src0 = llvm::dyn_cast<Variable>(getSrc(0));
    (void)Src0;
    assert(Src0 && Src0->getRegNum() == RegX8632::Reg_eax);
    const x86::AssemblerX86::GPREmitterOneOp Emitter = {
        &x86::AssemblerX86::imul, &x86::AssemblerX86::imul};
    emitIASOpTyGPR(Func, Ty, getSrc(1), Emitter);
  } else {
    // We only use imul as a two-address instruction even though
    // there is a 3 operand version when one of the operands is a constant.
    assert(Var == getSrc(0));
    const x86::AssemblerX86::GPREmitterRegOp Emitter = {
        &x86::AssemblerX86::imul, &x86::AssemblerX86::imul,
        &x86::AssemblerX86::imul};
    emitIASRegOpTyGPR(Func, Ty, Var, Src, Emitter);
  }
}

template <> void InstX8632Cbwdq::emit(const Cfg *Func) const {
  Ostream &Str = Func->getContext()->getStrEmit();
  assert(getSrcSize() == 1);
  Operand *Src0 = getSrc(0);
  assert(llvm::isa<Variable>(Src0));
  assert(llvm::cast<Variable>(Src0)->getRegNum() == RegX8632::Reg_eax);
  switch (Src0->getType()) {
  default:
    llvm_unreachable("unexpected source type!");
    break;
  case IceType_i8:
    assert(getDest()->getRegNum() == RegX8632::Reg_eax);
    Str << "\tcbw\n";
    break;
  case IceType_i16:
    assert(getDest()->getRegNum() == RegX8632::Reg_edx);
    Str << "\tcwd\n";
    break;
  case IceType_i32:
    assert(getDest()->getRegNum() == RegX8632::Reg_edx);
    Str << "\tcdq\n";
    break;
  }
}

template <> void InstX8632Cbwdq::emitIAS(const Cfg *Func) const {
  x86::AssemblerX86 *Asm = Func->getAssembler<x86::AssemblerX86>();
  intptr_t StartPosition = Asm->GetPosition();
  assert(getSrcSize() == 1);
  Operand *Src0 = getSrc(0);
  assert(llvm::isa<Variable>(Src0));
  assert(llvm::cast<Variable>(Src0)->getRegNum() == RegX8632::Reg_eax);
  switch (Src0->getType()) {
  default:
    llvm_unreachable("unexpected source type!");
    break;
  case IceType_i8:
    assert(getDest()->getRegNum() == RegX8632::Reg_eax);
    Asm->cbw();
    break;
  case IceType_i16:
    assert(getDest()->getRegNum() == RegX8632::Reg_edx);
    Asm->cwd();
    break;
  case IceType_i32:
    assert(getDest()->getRegNum() == RegX8632::Reg_edx);
    Asm->cdq();
    break;
  }
  emitIASBytes(Func, Asm, StartPosition);
}

void InstX8632Mul::emit(const Cfg *Func) const {
  Ostream &Str = Func->getContext()->getStrEmit();
  assert(getSrcSize() == 2);
  assert(llvm::isa<Variable>(getSrc(0)));
  assert(llvm::dyn_cast<Variable>(getSrc(0))->getRegNum() == RegX8632::Reg_eax);
  assert(getDest()->getRegNum() == RegX8632::Reg_eax); // TODO: allow edx?
  Str << "\tmul\t";
  getSrc(1)->emit(Func);
  Str << "\n";
}

void InstX8632Mul::emitIAS(const Cfg *Func) const {
  assert(getSrcSize() == 2);
  assert(llvm::isa<Variable>(getSrc(0)));
  assert(llvm::dyn_cast<Variable>(getSrc(0))->getRegNum() == RegX8632::Reg_eax);
  assert(getDest()->getRegNum() == RegX8632::Reg_eax); // TODO: allow edx?
  const Operand *Src = getSrc(1);
  Type Ty = Src->getType();
  const static x86::AssemblerX86::GPREmitterOneOp Emitter = {
      &x86::AssemblerX86::mul, &x86::AssemblerX86::mul};
  emitIASOpTyGPR(Func, Ty, Src, Emitter);
}

void InstX8632Mul::dump(const Cfg *Func) const {
  Ostream &Str = Func->getContext()->getStrDump();
  dumpDest(Func);
  Str << " = mul." << getDest()->getType() << " ";
  dumpSources(Func);
}

void InstX8632Shld::emit(const Cfg *Func) const {
  Ostream &Str = Func->getContext()->getStrEmit();
  assert(getSrcSize() == 3);
  assert(getDest() == getSrc(0));
  Str << "\tshld\t";
  getDest()->emit(Func);
  Str << ", ";
  getSrc(1)->emit(Func);
  Str << ", ";
  if (Variable *ShiftReg = llvm::dyn_cast<Variable>(getSrc(2))) {
    (void)ShiftReg;
    assert(ShiftReg->getRegNum() == RegX8632::Reg_ecx);
    Str << "cl";
  } else {
    getSrc(2)->emit(Func);
  }
  Str << "\n";
}

void InstX8632Shld::dump(const Cfg *Func) const {
  Ostream &Str = Func->getContext()->getStrDump();
  dumpDest(Func);
  Str << " = shld." << getDest()->getType() << " ";
  dumpSources(Func);
}

void InstX8632Shrd::emit(const Cfg *Func) const {
  Ostream &Str = Func->getContext()->getStrEmit();
  assert(getSrcSize() == 3);
  assert(getDest() == getSrc(0));
  Str << "\tshrd\t";
  getDest()->emit(Func);
  Str << ", ";
  getSrc(1)->emit(Func);
  Str << ", ";
  if (Variable *ShiftReg = llvm::dyn_cast<Variable>(getSrc(2))) {
    (void)ShiftReg;
    assert(ShiftReg->getRegNum() == RegX8632::Reg_ecx);
    Str << "cl";
  } else {
    getSrc(2)->emit(Func);
  }
  Str << "\n";
}

void InstX8632Shrd::dump(const Cfg *Func) const {
  Ostream &Str = Func->getContext()->getStrDump();
  dumpDest(Func);
  Str << " = shrd." << getDest()->getType() << " ";
  dumpSources(Func);
}

void InstX8632Cmov::emit(const Cfg *Func) const {
  Ostream &Str = Func->getContext()->getStrEmit();
  Str << "\t";
  assert(Condition != CondX86::Br_None);
  assert(getDest()->hasReg());
  Str << "cmov" << InstX8632BrAttributes[Condition].DisplayString << "\t";
  getDest()->emit(Func);
  Str << ", ";
  getSrc(1)->emit(Func);
  Str << "\n";
}

void InstX8632Cmov::emitIAS(const Cfg *Func) const {
  assert(Condition != CondX86::Br_None);
  assert(getDest()->hasReg());
  assert(getSrcSize() == 2);
  // Only need the reg/reg form now.
  const Variable *Src = llvm::cast<Variable>(getSrc(1));
  assert(Src->hasReg());
  assert(Src->getType() == IceType_i32);
  x86::AssemblerX86 *Asm = Func->getAssembler<x86::AssemblerX86>();
  intptr_t StartPosition = Asm->GetPosition();
  Asm->cmov(Condition, RegX8632::getEncodedGPR(getDest()->getRegNum()),
            RegX8632::getEncodedGPR(Src->getRegNum()));
  emitIASBytes(Func, Asm, StartPosition);
}

void InstX8632Cmov::dump(const Cfg *Func) const {
  Ostream &Str = Func->getContext()->getStrDump();
  Str << "cmov" << InstX8632BrAttributes[Condition].DisplayString << ".";
  Str << getDest()->getType() << " ";
  dumpDest(Func);
  Str << ", ";
  dumpSources(Func);
}

void InstX8632Cmpps::emit(const Cfg *Func) const {
  Ostream &Str = Func->getContext()->getStrEmit();
  assert(getSrcSize() == 2);
  assert(Condition < CondX86::Cmpps_Invalid);
  Str << "\t";
  Str << "cmp" << InstX8632CmppsAttributes[Condition].EmitString << "ps"
      << "\t";
  getDest()->emit(Func);
  Str << ", ";
  getSrc(1)->emit(Func);
  Str << "\n";
}

void InstX8632Cmpps::emitIAS(const Cfg *Func) const {
  x86::AssemblerX86 *Asm = Func->getAssembler<x86::AssemblerX86>();
  intptr_t StartPosition = Asm->GetPosition();
  assert(getSrcSize() == 2);
  assert(Condition < CondX86::Cmpps_Invalid);
  // Assuming there isn't any load folding for cmpps, and vector constants
  // are not allowed in PNaCl.
  assert(llvm::isa<Variable>(getSrc(1)));
  const Variable *SrcVar = llvm::cast<Variable>(getSrc(1));
  if (SrcVar->hasReg()) {
    Asm->cmpps(RegX8632::getEncodedXmm(getDest()->getRegNum()),
               RegX8632::getEncodedXmm(SrcVar->getRegNum()), Condition);
  } else {
    x86::Address SrcStackAddr = static_cast<TargetX8632 *>(Func->getTarget())
                                    ->stackVarToAsmOperand(SrcVar);
    Asm->cmpps(RegX8632::getEncodedXmm(getDest()->getRegNum()), SrcStackAddr,
               Condition);
  }
  emitIASBytes(Func, Asm, StartPosition);
}

void InstX8632Cmpps::dump(const Cfg *Func) const {
  Ostream &Str = Func->getContext()->getStrDump();
  assert(Condition < CondX86::Cmpps_Invalid);
  dumpDest(Func);
  Str << " = cmp" << InstX8632CmppsAttributes[Condition].EmitString << "ps"
      << "\t";
  dumpSources(Func);
}

void InstX8632Cmpxchg::emit(const Cfg *Func) const {
  Ostream &Str = Func->getContext()->getStrEmit();
  assert(getSrcSize() == 3);
  if (Locked) {
    Str << "\tlock";
  }
  Str << "\tcmpxchg\t";
  getSrc(0)->emit(Func);
  Str << ", ";
  getSrc(2)->emit(Func);
  Str << "\n";
}

void InstX8632Cmpxchg::emitIAS(const Cfg *Func) const {
  assert(getSrcSize() == 3);
  x86::AssemblerX86 *Asm = Func->getAssembler<x86::AssemblerX86>();
  intptr_t StartPosition = Asm->GetPosition();
  Type Ty = getSrc(0)->getType();
  const OperandX8632Mem *Mem = llvm::cast<OperandX8632Mem>(getSrc(0));
  assert(Mem->getSegmentRegister() == OperandX8632Mem::DefaultSegment);
  const x86::Address Addr = Mem->toAsmAddress(Asm);
  const Variable *VarReg = llvm::cast<Variable>(getSrc(2));
  assert(VarReg->hasReg());
  const RegX8632::GPRRegister Reg =
      RegX8632::getEncodedGPR(VarReg->getRegNum());
  if (Locked) {
    Asm->LockCmpxchg(Ty, Addr, Reg);
  } else {
    Asm->cmpxchg(Ty, Addr, Reg);
  }
  emitIASBytes(Func, Asm, StartPosition);
}

void InstX8632Cmpxchg::dump(const Cfg *Func) const {
  Ostream &Str = Func->getContext()->getStrDump();
  if (Locked) {
    Str << "lock ";
  }
  Str << "cmpxchg." << getSrc(0)->getType() << " ";
  dumpSources(Func);
}

void InstX8632Cmpxchg8b::emit(const Cfg *Func) const {
  Ostream &Str = Func->getContext()->getStrEmit();
  assert(getSrcSize() == 5);
  if (Locked) {
    Str << "\tlock";
  }
  Str << "\tcmpxchg8b\t";
  getSrc(0)->emit(Func);
  Str << "\n";
}

void InstX8632Cmpxchg8b::emitIAS(const Cfg *Func) const {
  assert(getSrcSize() == 5);
  x86::AssemblerX86 *Asm = Func->getAssembler<x86::AssemblerX86>();
  intptr_t StartPosition = Asm->GetPosition();
  const OperandX8632Mem *Mem = llvm::cast<OperandX8632Mem>(getSrc(0));
  assert(Mem->getSegmentRegister() == OperandX8632Mem::DefaultSegment);
  const x86::Address Addr = Mem->toAsmAddress(Asm);
  if (Locked) {
    Asm->lock();
  }
  Asm->cmpxchg8b(Addr);
  emitIASBytes(Func, Asm, StartPosition);
}

void InstX8632Cmpxchg8b::dump(const Cfg *Func) const {
  Ostream &Str = Func->getContext()->getStrDump();
  if (Locked) {
    Str << "lock ";
  }
  Str << "cmpxchg8b ";
  dumpSources(Func);
}

void InstX8632Cvt::emit(const Cfg *Func) const {
  Ostream &Str = Func->getContext()->getStrEmit();
  assert(getSrcSize() == 1);
  Str << "\tcvt";
  if (isTruncating())
    Str << "t";
  Str << TypeX8632Attributes[getSrc(0)->getType()].CvtString << "2"
      << TypeX8632Attributes[getDest()->getType()].CvtString << "\t";
  getDest()->emit(Func);
  Str << ", ";
  getSrc(0)->emit(Func);
  Str << "\n";
}

void InstX8632Cvt::emitIAS(const Cfg *Func) const {
  assert(getSrcSize() == 1);
  const Variable *Dest = getDest();
  const Operand *Src = getSrc(0);
  Type DestTy = Dest->getType();
  Type SrcTy = Src->getType();
  switch (Variant) {
  case Si2ss: {
    assert(isScalarIntegerType(SrcTy));
    assert(typeWidthInBytes(SrcTy) <= 4);
    assert(isScalarFloatingType(DestTy));
    static const x86::AssemblerX86::CastEmitterRegOp<
        RegX8632::XmmRegister, RegX8632::GPRRegister> Emitter = {
        &x86::AssemblerX86::cvtsi2ss, &x86::AssemblerX86::cvtsi2ss};
    emitIASCastRegOp<RegX8632::XmmRegister, RegX8632::GPRRegister,
                     RegX8632::getEncodedXmm, RegX8632::getEncodedGPR>(
        Func, DestTy, Dest, Src, Emitter);
    return;
  }
  case Tss2si: {
    assert(isScalarFloatingType(SrcTy));
    assert(isScalarIntegerType(DestTy));
    assert(typeWidthInBytes(DestTy) <= 4);
    static const x86::AssemblerX86::CastEmitterRegOp<
        RegX8632::GPRRegister, RegX8632::XmmRegister> Emitter = {
        &x86::AssemblerX86::cvttss2si, &x86::AssemblerX86::cvttss2si};
    emitIASCastRegOp<RegX8632::GPRRegister, RegX8632::XmmRegister,
                     RegX8632::getEncodedGPR, RegX8632::getEncodedXmm>(
        Func, SrcTy, Dest, Src, Emitter);
    return;
  }
  case Float2float: {
    assert(isScalarFloatingType(SrcTy));
    assert(isScalarFloatingType(DestTy));
    assert(DestTy != SrcTy);
    static const x86::AssemblerX86::XmmEmitterRegOp Emitter = {
        &x86::AssemblerX86::cvtfloat2float, &x86::AssemblerX86::cvtfloat2float};
    emitIASRegOpTyXMM(Func, SrcTy, Dest, Src, Emitter);
    return;
  }
  case Dq2ps: {
    assert(isVectorIntegerType(SrcTy));
    assert(isVectorFloatingType(DestTy));
    static const x86::AssemblerX86::XmmEmitterRegOp Emitter = {
        &x86::AssemblerX86::cvtdq2ps, &x86::AssemblerX86::cvtdq2ps};
    emitIASRegOpTyXMM(Func, DestTy, Dest, Src, Emitter);
    return;
  }
  case Tps2dq: {
    assert(isVectorFloatingType(SrcTy));
    assert(isVectorIntegerType(DestTy));
    static const x86::AssemblerX86::XmmEmitterRegOp Emitter = {
        &x86::AssemblerX86::cvttps2dq, &x86::AssemblerX86::cvttps2dq};
    emitIASRegOpTyXMM(Func, DestTy, Dest, Src, Emitter);
    return;
  }
  }
}

void InstX8632Cvt::dump(const Cfg *Func) const {
  Ostream &Str = Func->getContext()->getStrDump();
  dumpDest(Func);
  Str << " = cvt";
  if (isTruncating())
    Str << "t";
  Str << TypeX8632Attributes[getSrc(0)->getType()].CvtString << "2"
      << TypeX8632Attributes[getDest()->getType()].CvtString << " ";
  dumpSources(Func);
}

void InstX8632Icmp::emit(const Cfg *Func) const {
  Ostream &Str = Func->getContext()->getStrEmit();
  assert(getSrcSize() == 2);
  Str << "\tcmp\t";
  getSrc(0)->emit(Func);
  Str << ", ";
  getSrc(1)->emit(Func);
  Str << "\n";
}

void InstX8632Icmp::emitIAS(const Cfg *Func) const {
  assert(getSrcSize() == 2);
  const Operand *Src0 = getSrc(0);
  const Operand *Src1 = getSrc(1);
  Type Ty = Src0->getType();
  static const x86::AssemblerX86::GPREmitterRegOp RegEmitter = {
    &x86::AssemblerX86::cmp, &x86::AssemblerX86::cmp, &x86::AssemblerX86::cmp
  };
  static const x86::AssemblerX86::GPREmitterAddrOp AddrEmitter = {
    &x86::AssemblerX86::cmp, &x86::AssemblerX86::cmp
  };
  if (const Variable *SrcVar0 = llvm::dyn_cast<Variable>(Src0)) {
    if (SrcVar0->hasReg()) {
      emitIASRegOpTyGPR(Func, Ty, SrcVar0, Src1, RegEmitter);
    } else {
      x86::Address StackAddr(static_cast<TargetX8632 *>(Func->getTarget())
                                 ->stackVarToAsmOperand(SrcVar0));
      emitIASAddrOpTyGPR(Func, Ty, StackAddr, Src1, AddrEmitter);
    }
  } else if (const OperandX8632Mem *SrcMem0 =
                 llvm::dyn_cast<OperandX8632Mem>(Src0)) {
    x86::AssemblerX86 *Asm = Func->getAssembler<x86::AssemblerX86>();
    SrcMem0->emitSegmentOverride(Asm);
    emitIASAddrOpTyGPR(Func, Ty, SrcMem0->toAsmAddress(Asm), Src1, AddrEmitter);
  }
}

void InstX8632Icmp::dump(const Cfg *Func) const {
  Ostream &Str = Func->getContext()->getStrDump();
  Str << "cmp." << getSrc(0)->getType() << " ";
  dumpSources(Func);
}

void InstX8632Ucomiss::emit(const Cfg *Func) const {
  Ostream &Str = Func->getContext()->getStrEmit();
  assert(getSrcSize() == 2);
  Str << "\tucomi" << TypeX8632Attributes[getSrc(0)->getType()].SdSsString
      << "\t";
  getSrc(0)->emit(Func);
  Str << ", ";
  getSrc(1)->emit(Func);
  Str << "\n";
}

void InstX8632Ucomiss::emitIAS(const Cfg *Func) const {
  assert(getSrcSize() == 2);
  // Currently src0 is always a variable by convention, to avoid having
  // two memory operands.
  assert(llvm::isa<Variable>(getSrc(0)));
  const Variable *Src0 = llvm::cast<Variable>(getSrc(0));
  Type Ty = Src0->getType();
  const static x86::AssemblerX86::XmmEmitterRegOp Emitter = {
    &x86::AssemblerX86::ucomiss, &x86::AssemblerX86::ucomiss
  };
  emitIASRegOpTyXMM(Func, Ty, Src0, getSrc(1), Emitter);
}

void InstX8632Ucomiss::dump(const Cfg *Func) const {
  Ostream &Str = Func->getContext()->getStrDump();
  Str << "ucomiss." << getSrc(0)->getType() << " ";
  dumpSources(Func);
}

void InstX8632UD2::emit(const Cfg *Func) const {
  Ostream &Str = Func->getContext()->getStrEmit();
  assert(getSrcSize() == 0);
  Str << "\tud2\n";
}

void InstX8632UD2::emitIAS(const Cfg *Func) const {
  x86::AssemblerX86 *Asm = Func->getAssembler<x86::AssemblerX86>();
  intptr_t StartPosition = Asm->GetPosition();
  Asm->ud2();
  emitIASBytes(Func, Asm, StartPosition);
}

void InstX8632UD2::dump(const Cfg *Func) const {
  Ostream &Str = Func->getContext()->getStrDump();
  Str << "ud2\n";
}

void InstX8632Test::emit(const Cfg *Func) const {
  Ostream &Str = Func->getContext()->getStrEmit();
  assert(getSrcSize() == 2);
  Str << "\ttest\t";
  getSrc(0)->emit(Func);
  Str << ", ";
  getSrc(1)->emit(Func);
  Str << "\n";
}

void InstX8632Test::emitIAS(const Cfg *Func) const {
  assert(getSrcSize() == 2);
  const Operand *Src0 = getSrc(0);
  const Operand *Src1 = getSrc(1);
  Type Ty = Src0->getType();
  // The Reg/Addr form of test is not encodeable.
  static const x86::AssemblerX86::GPREmitterRegOp RegEmitter = {
    &x86::AssemblerX86::test, NULL, &x86::AssemblerX86::test
  };
  static const x86::AssemblerX86::GPREmitterAddrOp AddrEmitter = {
    &x86::AssemblerX86::test, &x86::AssemblerX86::test
  };
  if (const Variable *SrcVar0 = llvm::dyn_cast<Variable>(Src0)) {
    if (SrcVar0->hasReg()) {
      emitIASRegOpTyGPR(Func, Ty, SrcVar0, Src1, RegEmitter);
    } else {
      llvm_unreachable("Nothing actually generates this so it's untested");
      x86::Address StackAddr(static_cast<TargetX8632 *>(Func->getTarget())
                                 ->stackVarToAsmOperand(SrcVar0));
      emitIASAddrOpTyGPR(Func, Ty, StackAddr, Src1, AddrEmitter);
    }
  } else if (const OperandX8632Mem *SrcMem0 =
                 llvm::dyn_cast<OperandX8632Mem>(Src0)) {
    llvm_unreachable("Nothing actually generates this so it's untested");
    x86::AssemblerX86 *Asm = Func->getAssembler<x86::AssemblerX86>();
    SrcMem0->emitSegmentOverride(Asm);
    emitIASAddrOpTyGPR(Func, Ty, SrcMem0->toAsmAddress(Asm), Src1, AddrEmitter);
  }
}

void InstX8632Test::dump(const Cfg *Func) const {
  Ostream &Str = Func->getContext()->getStrDump();
  Str << "test." << getSrc(0)->getType() << " ";
  dumpSources(Func);
}

void InstX8632Mfence::emit(const Cfg *Func) const {
  Ostream &Str = Func->getContext()->getStrEmit();
  assert(getSrcSize() == 0);
  Str << "\tmfence\n";
}

void InstX8632Mfence::emitIAS(const Cfg *Func) const {
  x86::AssemblerX86 *Asm = Func->getAssembler<x86::AssemblerX86>();
  intptr_t StartPosition = Asm->GetPosition();
  Asm->mfence();
  emitIASBytes(Func, Asm, StartPosition);
}

void InstX8632Mfence::dump(const Cfg *Func) const {
  Ostream &Str = Func->getContext()->getStrDump();
  Str << "mfence\n";
}

void InstX8632Store::emit(const Cfg *Func) const {
  Ostream &Str = Func->getContext()->getStrEmit();
  assert(getSrcSize() == 2);
  Str << "\tmov" << TypeX8632Attributes[getSrc(0)->getType()].SdSsString
      << "\t";
  getSrc(1)->emit(Func);
  Str << ", ";
  getSrc(0)->emit(Func);
  Str << "\n";
}

void InstX8632Store::dump(const Cfg *Func) const {
  Ostream &Str = Func->getContext()->getStrDump();
  Str << "mov." << getSrc(0)->getType() << " ";
  getSrc(1)->dump(Func);
  Str << ", ";
  getSrc(0)->dump(Func);
}

void InstX8632StoreP::emit(const Cfg *Func) const {
  Ostream &Str = Func->getContext()->getStrEmit();
  assert(getSrcSize() == 2);
  Str << "\tmovups\t";
  getSrc(1)->emit(Func);
  Str << ", ";
  getSrc(0)->emit(Func);
  Str << "\n";
}

void InstX8632StoreP::emitIAS(const Cfg *Func) const {
  x86::AssemblerX86 *Asm = Func->getAssembler<x86::AssemblerX86>();
  intptr_t StartPosition = Asm->GetPosition();
  assert(getSrcSize() == 2);
  const Variable *Src = llvm::cast<Variable>(getSrc(0));
  const OperandX8632Mem *DestMem = llvm::cast<OperandX8632Mem>(getSrc(1));
  assert(DestMem->getSegmentRegister() == OperandX8632Mem::DefaultSegment);
  assert(Src->hasReg());
  Asm->movups(DestMem->toAsmAddress(Asm),
              RegX8632::getEncodedXmm(Src->getRegNum()));
  emitIASBytes(Func, Asm, StartPosition);
}

void InstX8632StoreP::dump(const Cfg *Func) const {
  Ostream &Str = Func->getContext()->getStrDump();
  Str << "storep." << getSrc(0)->getType() << " ";
  getSrc(1)->dump(Func);
  Str << ", ";
  getSrc(0)->dump(Func);
}

void InstX8632StoreQ::emit(const Cfg *Func) const {
  Ostream &Str = Func->getContext()->getStrEmit();
  assert(getSrcSize() == 2);
  assert(getSrc(1)->getType() == IceType_i64 ||
         getSrc(1)->getType() == IceType_f64);
  Str << "\tmovq\t";
  getSrc(1)->emit(Func);
  Str << ", ";
  getSrc(0)->emit(Func);
  Str << "\n";
}

void InstX8632StoreQ::emitIAS(const Cfg *Func) const {
  x86::AssemblerX86 *Asm = Func->getAssembler<x86::AssemblerX86>();
  intptr_t StartPosition = Asm->GetPosition();
  assert(getSrcSize() == 2);
  const Variable *Src = llvm::cast<Variable>(getSrc(0));
  const OperandX8632Mem *DestMem = llvm::cast<OperandX8632Mem>(getSrc(1));
  assert(DestMem->getSegmentRegister() == OperandX8632Mem::DefaultSegment);
  assert(Src->hasReg());
  Asm->movq(DestMem->toAsmAddress(Asm),
            RegX8632::getEncodedXmm(Src->getRegNum()));
  emitIASBytes(Func, Asm, StartPosition);
}

void InstX8632StoreQ::dump(const Cfg *Func) const {
  Ostream &Str = Func->getContext()->getStrDump();
  Str << "storeq." << getSrc(0)->getType() << " ";
  getSrc(1)->dump(Func);
  Str << ", ";
  getSrc(0)->dump(Func);
}

template <> void InstX8632Lea::emit(const Cfg *Func) const {
  Ostream &Str = Func->getContext()->getStrEmit();
  assert(getSrcSize() == 1);
  assert(getDest()->hasReg());
  Str << "\tlea\t";
  getDest()->emit(Func);
  Str << ", ";
  Operand *Src0 = getSrc(0);
  if (Variable *VSrc0 = llvm::dyn_cast<Variable>(Src0)) {
    Type Ty = VSrc0->getType();
    // lea on x86-32 doesn't accept mem128 operands, so cast VSrc0 to an
    // acceptable type.
    VSrc0->asType(isVectorType(Ty) ? IceType_i32 : Ty).emit(Func);
  } else {
    Src0->emit(Func);
  }
  Str << "\n";
}

template <> void InstX8632Mov::emit(const Cfg *Func) const {
  Ostream &Str = Func->getContext()->getStrEmit();
  assert(getSrcSize() == 1);
  Operand *Src = getSrc(0);
  // The llvm-mc assembler using Intel syntax has a bug in which "mov
  // reg, RelocatableConstant" does not generate the right instruction
  // with a relocation.  To work around, we emit "lea reg,
  // RelocatableConstant".  Also, the lowering and legalization is
  // changed to allow relocatable constants only in Assign and Call
  // instructions or in Mem operands.  TODO(stichnot): remove LEAHACK
  // once a proper emitter is used.
  //
  // In addition, llvm-mc doesn't like "lea eax, bp" or "lea eax, Sp"
  // or "lea eax, flags" etc., when the relocatable constant name is a
  // reserved word.  The hack-on-top-of-hack is to temporarily drop
  // into AT&T syntax for this lea instruction.
  bool UseLeaHack = llvm::isa<ConstantRelocatable>(Src);
  if (UseLeaHack) {
    Str << ".att_syntax\n";
    Str << "\tleal";
  } else {
    Str << "\tmov" << TypeX8632Attributes[getDest()->getType()].SdSsString;
  }
  Str << "\t";
  // For an integer truncation operation, src is wider than dest.
  // Ideally, we use a mov instruction whose data width matches the
  // narrower dest.  This is a problem if e.g. src is a register like
  // esi or si where there is no 8-bit version of the register.  To be
  // safe, we instead widen the dest to match src.  This works even
  // for stack-allocated dest variables because typeWidthOnStack()
  // pads to a 4-byte boundary even if only a lower portion is used.
  // TODO: This assert disallows usages such as copying a floating point
  // value between a vector and a scalar (which movss is used for).
  // Clean this up.
  assert(Func->getTarget()->typeWidthInBytesOnStack(getDest()->getType()) ==
         Func->getTarget()->typeWidthInBytesOnStack(Src->getType()));
  if (UseLeaHack) {
    Src->emit(Func);
    Str << ", %";
    getDest()->emit(Func);
    Str << "\n";
    Str << ".intel_syntax\n";
  } else {
    getDest()->asType(Src->getType()).emit(Func);
    Str << ", ";
    Src->emit(Func);
    Str << "\n";
  }
}

template <> void InstX8632Mov::emitIAS(const Cfg *Func) const {
  assert(getSrcSize() == 1);
  const Variable *Dest = getDest();
  const Operand *Src = getSrc(0);
  Type DestTy = Dest->getType();
  Type SrcTy = Src->getType();
  // Mov can be used for GPRs or XMM registers. Also, the type does not
  // necessarily match (Mov can be used for bitcasts). However, when
  // the type does not match, one of the operands must be a register.
  // Thus, the strategy is to find out if Src or Dest are a register,
  // then use that register's type to decide on which emitter set to use.
  // The emitter set will include reg-reg movs, but that case should
  // be unused when the types don't match.
  static const x86::AssemblerX86::XmmEmitterRegOp XmmRegEmitter = {
      &x86::AssemblerX86::movss, &x86::AssemblerX86::movss};
  static const x86::AssemblerX86::GPREmitterRegOp GPRRegEmitter = {
      &x86::AssemblerX86::mov, &x86::AssemblerX86::mov,
      &x86::AssemblerX86::mov};
  static const x86::AssemblerX86::GPREmitterAddrOp GPRAddrEmitter = {
      &x86::AssemblerX86::mov, &x86::AssemblerX86::mov};
  // For an integer truncation operation, src is wider than dest.
  // Ideally, we use a mov instruction whose data width matches the
  // narrower dest.  This is a problem if e.g. src is a register like
  // esi or si where there is no 8-bit version of the register.  To be
  // safe, we instead widen the dest to match src.  This works even
  // for stack-allocated dest variables because typeWidthOnStack()
  // pads to a 4-byte boundary even if only a lower portion is used.
  // TODO: This assert disallows usages such as copying a floating point
  // value between a vector and a scalar (which movss is used for).
  // Clean this up.
  assert(Func->getTarget()->typeWidthInBytesOnStack(getDest()->getType()) ==
         Func->getTarget()->typeWidthInBytesOnStack(Src->getType()));
  if (Dest->hasReg()) {
    if (isScalarFloatingType(DestTy)) {
      emitIASRegOpTyXMM(Func, DestTy, Dest, Src, XmmRegEmitter);
      return;
    } else {
      assert(isScalarIntegerType(DestTy));
      // Widen DestTy for truncation (see above note). We should only do this
      // when both Src and Dest are integer types.
      if (isScalarIntegerType(SrcTy)) {
        DestTy = SrcTy;
      }
      emitIASRegOpTyGPR(Func, DestTy, Dest, Src, GPRRegEmitter);
      return;
    }
  } else {
    // Dest must be Stack and Src *could* be a register. Use Src's type
    // to decide on the emitters.
    x86::Address StackAddr(static_cast<TargetX8632 *>(Func->getTarget())
                               ->stackVarToAsmOperand(Dest));
    if (isScalarFloatingType(SrcTy)) {
      // Src must be a register.
      const Variable *SrcVar = llvm::cast<Variable>(Src);
      assert(SrcVar->hasReg());
      x86::AssemblerX86 *Asm = Func->getAssembler<x86::AssemblerX86>();
      intptr_t StartPosition = Asm->GetPosition();
      Asm->movss(SrcTy, StackAddr,
                 RegX8632::getEncodedXmm(SrcVar->getRegNum()));
      emitIASBytes(Func, Asm, StartPosition);
      return;
    } else {
      // Src can be a register or immediate.
      assert(isScalarIntegerType(SrcTy));
      emitIASAddrOpTyGPR(Func, SrcTy, StackAddr, Src, GPRAddrEmitter);
      return;
    }
    return;
  }
}

template <> void InstX8632Movd::emitIAS(const Cfg *Func) const {
  x86::AssemblerX86 *Asm = Func->getAssembler<x86::AssemblerX86>();
  intptr_t StartPosition = Asm->GetPosition();
  assert(getSrcSize() == 1);
  const Variable *Dest = getDest();
  const Variable *Src = llvm::cast<Variable>(getSrc(0));
  // For insert/extract element (one of Src/Dest is an Xmm vector and
  // the other is an int type).
  if (Src->getType() == IceType_i32) {
    assert(isVectorType(Dest->getType()));
    assert(Dest->hasReg());
    RegX8632::XmmRegister DestReg = RegX8632::getEncodedXmm(Dest->getRegNum());
    if (Src->hasReg()) {
      Asm->movd(DestReg, RegX8632::getEncodedGPR(Src->getRegNum()));
    } else {
      x86::Address StackAddr(static_cast<TargetX8632 *>(Func->getTarget())
                                 ->stackVarToAsmOperand(Src));
      Asm->movd(DestReg, StackAddr);
    }
  } else {
    assert(isVectorType(Src->getType()));
    assert(Src->hasReg());
    assert(Dest->getType() == IceType_i32);
    RegX8632::XmmRegister SrcReg = RegX8632::getEncodedXmm(Src->getRegNum());
    if (Dest->hasReg()) {
      Asm->movd(RegX8632::getEncodedGPR(Dest->getRegNum()), SrcReg);
    } else {
      x86::Address StackAddr(static_cast<TargetX8632 *>(Func->getTarget())
                                 ->stackVarToAsmOperand(Dest));
      Asm->movd(StackAddr, SrcReg);
    }
  }
  emitIASBytes(Func, Asm, StartPosition);
}

template <> void InstX8632Movp::emit(const Cfg *Func) const {
  // TODO(wala,stichnot): movups works with all vector operands, but
  // there exist other instructions (movaps, movdqa, movdqu) that may
  // perform better, depending on the data type and alignment of the
  // operands.
  Ostream &Str = Func->getContext()->getStrEmit();
  assert(getSrcSize() == 1);
  Str << "\tmovups\t";
  getDest()->emit(Func);
  Str << ", ";
  getSrc(0)->emit(Func);
  Str << "\n";
}

template <> void InstX8632Movp::emitIAS(const Cfg *Func) const {
  assert(getSrcSize() == 1);
  assert(isVectorType(getDest()->getType()));
  const Variable *Dest = getDest();
  const Operand *Src = getSrc(0);
  const static x86::AssemblerX86::XmmEmitterMovOps Emitter = {
    &x86::AssemblerX86::movups, &x86::AssemblerX86::movups,
    &x86::AssemblerX86::movups
  };
  emitIASMovlikeXMM(Func, Dest, Src, Emitter);
}

template <> void InstX8632Movq::emit(const Cfg *Func) const {
  Ostream &Str = Func->getContext()->getStrEmit();
  assert(getSrcSize() == 1);
  assert(getDest()->getType() == IceType_i64 ||
         getDest()->getType() == IceType_f64);
  Str << "\tmovq\t";
  getDest()->emit(Func);
  Str << ", ";
  getSrc(0)->emit(Func);
  Str << "\n";
}

template <> void InstX8632Movq::emitIAS(const Cfg *Func) const {
  assert(getSrcSize() == 1);
  assert(getDest()->getType() == IceType_i64 ||
         getDest()->getType() == IceType_f64);
  const Variable *Dest = getDest();
  const Operand *Src = getSrc(0);
  const static x86::AssemblerX86::XmmEmitterMovOps Emitter = {
    &x86::AssemblerX86::movq, &x86::AssemblerX86::movq, &x86::AssemblerX86::movq
  };
  emitIASMovlikeXMM(Func, Dest, Src, Emitter);
}

template <> void InstX8632MovssRegs::emitIAS(const Cfg *Func) const {
  // This is Binop variant is only intended to be used for reg-reg moves
  // where part of the Dest register is untouched.
  assert(getSrcSize() == 2);
  const Variable *Dest = getDest();
  assert(Dest == getSrc(0));
  const Variable *Src = llvm::cast<Variable>(getSrc(1));
  assert(Dest->hasReg() && Src->hasReg());
  x86::AssemblerX86 *Asm = Func->getAssembler<x86::AssemblerX86>();
  intptr_t StartPosition = Asm->GetPosition();
  Asm->movss(IceType_f32, RegX8632::getEncodedXmm(Dest->getRegNum()),
             RegX8632::getEncodedXmm(Src->getRegNum()));
  emitIASBytes(Func, Asm, StartPosition);
}

void InstX8632Movsx::emit(const Cfg *Func) const {
  Ostream &Str = Func->getContext()->getStrEmit();
  assert(getSrcSize() == 1);
  Str << "\tmovsx\t";
  getDest()->emit(Func);
  Str << ", ";
  getSrc(0)->emit(Func);
  Str << "\n";
}

void InstX8632Movsx::dump(const Cfg *Func) const {
  Ostream &Str = Func->getContext()->getStrDump();
  Str << "movsx." << getDest()->getType() << "." << getSrc(0)->getType();
  Str << " ";
  dumpDest(Func);
  Str << ", ";
  dumpSources(Func);
}

void InstX8632Movzx::emit(const Cfg *Func) const {
  Ostream &Str = Func->getContext()->getStrEmit();
  assert(getSrcSize() == 1);
  Str << "\tmovzx\t";
  getDest()->emit(Func);
  Str << ", ";
  getSrc(0)->emit(Func);
  Str << "\n";
}

void InstX8632Movzx::dump(const Cfg *Func) const {
  Ostream &Str = Func->getContext()->getStrDump();
  Str << "movzx." << getDest()->getType() << "." << getSrc(0)->getType();
  Str << " ";
  dumpDest(Func);
  Str << ", ";
  dumpSources(Func);
}

void InstX8632Nop::emit(const Cfg *Func) const {
  Ostream &Str = Func->getContext()->getStrEmit();
  // TODO: Emit the right code for each variant.
  Str << "\tnop\t# variant = " << Variant << "\n";
}

void InstX8632Nop::emitIAS(const Cfg *Func) const {
  x86::AssemblerX86 *Asm = Func->getAssembler<x86::AssemblerX86>();
  intptr_t StartPosition = Asm->GetPosition();
  // TODO: Emit the right code for the variant.
  Asm->nop();
  emitIASBytes(Func, Asm, StartPosition);
}

void InstX8632Nop::dump(const Cfg *Func) const {
  Ostream &Str = Func->getContext()->getStrDump();
  Str << "nop (variant = " << Variant << ")";
}

void InstX8632Fld::emit(const Cfg *Func) const {
  Ostream &Str = Func->getContext()->getStrEmit();
  assert(getSrcSize() == 1);
  Type Ty = getSrc(0)->getType();
  Variable *Var = llvm::dyn_cast<Variable>(getSrc(0));
  if (Var && Var->hasReg()) {
    // This is a physical xmm register, so we need to spill it to a
    // temporary stack slot.
    SizeT Width = typeWidthInBytes(Ty);
    Str << "\tsub\tesp, " << Width << "\n";
    Str << "\tmov" << TypeX8632Attributes[Ty].SdSsString << "\t"
        << TypeX8632Attributes[Ty].WidthString << " [esp], ";
    Var->emit(Func);
    Str << "\n";
    Str << "\tfld\t" << TypeX8632Attributes[Ty].WidthString << " [esp]\n";
    Str << "\tadd\tesp, " << Width << "\n";
    return;
  }
  Str << "\tfld\t";
  getSrc(0)->emit(Func);
  Str << "\n";
}

void InstX8632Fld::emitIAS(const Cfg *Func) const {
  x86::AssemblerX86 *Asm = Func->getAssembler<x86::AssemblerX86>();
  intptr_t StartPosition = Asm->GetPosition();
  assert(getSrcSize() == 1);
  const Operand *Src = getSrc(0);
  Type Ty = Src->getType();
  if (const auto Var = llvm::dyn_cast<Variable>(Src)) {
    if (Var->hasReg()) {
      // This is a physical xmm register, so we need to spill it to a
      // temporary stack slot.
      x86::Immediate Width(typeWidthInBytes(Ty));
      Asm->sub(IceType_i32, RegX8632::Encoded_Reg_esp, Width);
      x86::Address StackSlot = x86::Address(RegX8632::Encoded_Reg_esp, 0);
      Asm->movss(Ty, StackSlot, RegX8632::getEncodedXmm(Var->getRegNum()));
      Asm->fld(Ty, StackSlot);
      Asm->add(IceType_i32, RegX8632::Encoded_Reg_esp, Width);
    } else {
      x86::Address StackAddr(static_cast<TargetX8632 *>(Func->getTarget())
                                 ->stackVarToAsmOperand(Var));
      Asm->fld(Ty, StackAddr);
    }
  } else if (const auto Mem = llvm::dyn_cast<OperandX8632Mem>(Src)) {
    assert(Mem->getSegmentRegister() == OperandX8632Mem::DefaultSegment);
    Asm->fld(Ty, Mem->toAsmAddress(Asm));
  } else if (const auto Imm = llvm::dyn_cast<Constant>(Src)) {
    Asm->fld(Ty, x86::Address::ofConstPool(Func->getContext(), Asm, Imm));
  } else {
    llvm_unreachable("Unexpected operand type");
  }
  emitIASBytes(Func, Asm, StartPosition);
}

void InstX8632Fld::dump(const Cfg *Func) const {
  Ostream &Str = Func->getContext()->getStrDump();
  Str << "fld." << getSrc(0)->getType() << " ";
  dumpSources(Func);
}

void InstX8632Fstp::emit(const Cfg *Func) const {
  Ostream &Str = Func->getContext()->getStrEmit();
  assert(getSrcSize() == 0);
  // TODO(jvoung,stichnot): Utilize this by setting Dest to NULL to
  // "partially" delete the fstp if the Dest is unused.
  // Even if Dest is unused, the fstp should be kept for the SideEffects
  // of popping the stack.
  if (getDest() == NULL) {
    Str << "\tfstp\tst(0)\n";
    return;
  }
  if (!getDest()->hasReg()) {
    Str << "\tfstp\t";
    getDest()->emit(Func);
    Str << "\n";
    return;
  }
  // Dest is a physical (xmm) register, so st(0) needs to go through
  // memory.  Hack this by creating a temporary stack slot, spilling
  // st(0) there, loading it into the xmm register, and deallocating
  // the stack slot.
  Type Ty = getDest()->getType();
  size_t Width = typeWidthInBytes(Ty);
  Str << "\tsub\tesp, " << Width << "\n";
  Str << "\tfstp\t" << TypeX8632Attributes[Ty].WidthString << " [esp]\n";
  Str << "\tmov" << TypeX8632Attributes[Ty].SdSsString << "\t";
  getDest()->emit(Func);
  Str << ", " << TypeX8632Attributes[Ty].WidthString << " [esp]\n";
  Str << "\tadd\tesp, " << Width << "\n";
}

void InstX8632Fstp::emitIAS(const Cfg *Func) const {
  x86::AssemblerX86 *Asm = Func->getAssembler<x86::AssemblerX86>();
  intptr_t StartPosition = Asm->GetPosition();
  assert(getSrcSize() == 0);
  const Variable *Dest = getDest();
  // TODO(jvoung,stichnot): Utilize this by setting Dest to NULL to
  // "partially" delete the fstp if the Dest is unused.
  // Even if Dest is unused, the fstp should be kept for the SideEffects
  // of popping the stack.
  if (Dest == NULL) {
    Asm->fstp(RegX8632::getEncodedSTReg(0));
    emitIASBytes(Func, Asm, StartPosition);
    return;
  }
  Type Ty = Dest->getType();
  if (!Dest->hasReg()) {
    x86::Address StackAddr(static_cast<TargetX8632 *>(Func->getTarget())
                               ->stackVarToAsmOperand(Dest));
    Asm->fstp(Ty, StackAddr);
  } else {
    // Dest is a physical (xmm) register, so st(0) needs to go through
    // memory.  Hack this by creating a temporary stack slot, spilling
    // st(0) there, loading it into the xmm register, and deallocating
    // the stack slot.
    x86::Immediate Width(typeWidthInBytes(Ty));
    Asm->sub(IceType_i32, RegX8632::Encoded_Reg_esp, Width);
    x86::Address StackSlot = x86::Address(RegX8632::Encoded_Reg_esp, 0);
    Asm->fstp(Ty, StackSlot);
    Asm->movss(Ty, RegX8632::getEncodedXmm(Dest->getRegNum()), StackSlot);
    Asm->add(IceType_i32, RegX8632::Encoded_Reg_esp, Width);
  }
  emitIASBytes(Func, Asm, StartPosition);
}

void InstX8632Fstp::dump(const Cfg *Func) const {
  Ostream &Str = Func->getContext()->getStrDump();
  dumpDest(Func);
  Str << " = fstp." << getDest()->getType() << ", st(0)";
  Str << "\n";
}

template <> void InstX8632Pcmpeq::emit(const Cfg *Func) const {
  char buf[30];
  snprintf(buf, llvm::array_lengthof(buf), "pcmpeq%s",
           TypeX8632Attributes[getDest()->getType()].PackString);
  emitTwoAddress(buf, this, Func);
}

template <> void InstX8632Pcmpgt::emit(const Cfg *Func) const {
  char buf[30];
  snprintf(buf, llvm::array_lengthof(buf), "pcmpgt%s",
           TypeX8632Attributes[getDest()->getType()].PackString);
  emitTwoAddress(buf, this, Func);
}

template <> void InstX8632Pextr::emit(const Cfg *Func) const {
  Ostream &Str = Func->getContext()->getStrEmit();
  assert(getSrcSize() == 2);
  // pextrb and pextrd are SSE4.1 instructions.
  assert(getSrc(0)->getType() == IceType_v8i16 ||
         getSrc(0)->getType() == IceType_v8i1 ||
         static_cast<TargetX8632 *>(Func->getTarget())->getInstructionSet()
             >= TargetX8632::SSE4_1);
  Str << "\t" << Opcode
      << TypeX8632Attributes[getSrc(0)->getType()].PackString << "\t";
  Variable *Dest = getDest();
  // pextrw must take a register dest.
  assert(Dest->getType() != IceType_i16 || Dest->hasReg());
  Dest->asType(IceType_i32).emit(Func);
  Str << ", ";
  getSrc(0)->emit(Func);
  Str << ", ";
  getSrc(1)->emit(Func);
  Str << "\n";
}

template <> void InstX8632Pinsr::emit(const Cfg *Func) const {
  Ostream &Str = Func->getContext()->getStrEmit();
  assert(getSrcSize() == 3);
  // pinsrb and pinsrd are SSE4.1 instructions.
  assert(getDest()->getType() == IceType_v8i16 ||
         getDest()->getType() == IceType_v8i1 ||
         static_cast<TargetX8632 *>(Func->getTarget())->getInstructionSet()
             >= TargetX8632::SSE4_1);
  Str << "\t" << Opcode
      << TypeX8632Attributes[getDest()->getType()].PackString << "\t";
  getDest()->emit(Func);
  Str << ", ";
  Operand *Src1 = getSrc(1);
  if (Variable *VSrc1 = llvm::dyn_cast<Variable>(Src1)) {
    // If src1 is a register, it should always be r32.
    if (VSrc1->hasReg()) {
      VSrc1->asType(IceType_i32).emit(Func);
    } else {
      VSrc1->emit(Func);
    }
  } else {
    Src1->emit(Func);
  }
  Str << ", ";
  getSrc(2)->emit(Func);
  Str << "\n";
}

void InstX8632Pop::emit(const Cfg *Func) const {
  Ostream &Str = Func->getContext()->getStrEmit();
  assert(getSrcSize() == 0);
  Str << "\tpop\t";
  getDest()->emit(Func);
  Str << "\n";
}

void InstX8632Pop::emitIAS(const Cfg *Func) const {
  assert(getSrcSize() == 0);
  x86::AssemblerX86 *Asm = Func->getAssembler<x86::AssemblerX86>();
  intptr_t StartPosition = Asm->GetPosition();
  if (getDest()->hasReg()) {
    Asm->popl(RegX8632::getEncodedGPR(getDest()->getRegNum()));
  } else {
    Asm->popl(static_cast<TargetX8632 *>(Func->getTarget())
                  ->stackVarToAsmOperand(getDest()));
  }
  emitIASBytes(Func, Asm, StartPosition);
}

void InstX8632Pop::dump(const Cfg *Func) const {
  Ostream &Str = Func->getContext()->getStrDump();
  dumpDest(Func);
  Str << " = pop." << getDest()->getType() << " ";
}

void InstX8632AdjustStack::emit(const Cfg *Func) const {
  Ostream &Str = Func->getContext()->getStrEmit();
  Str << "\tsub\tesp, " << Amount << "\n";
  Func->getTarget()->updateStackAdjustment(Amount);
}

void InstX8632AdjustStack::emitIAS(const Cfg *Func) const {
  x86::AssemblerX86 *Asm = Func->getAssembler<x86::AssemblerX86>();
  intptr_t StartPosition = Asm->GetPosition();
  Asm->sub(IceType_i32, RegX8632::Encoded_Reg_esp, x86::Immediate(Amount));
  emitIASBytes(Func, Asm, StartPosition);
  Func->getTarget()->updateStackAdjustment(Amount);
}

void InstX8632AdjustStack::dump(const Cfg *Func) const {
  Ostream &Str = Func->getContext()->getStrDump();
  Str << "esp = sub.i32 esp, " << Amount;
}

void InstX8632Push::emit(const Cfg *Func) const {
  Ostream &Str = Func->getContext()->getStrEmit();
  assert(getSrcSize() == 1);
  // Push is currently only used for saving GPRs.
  Variable *Var = llvm::cast<Variable>(getSrc(0));
  assert(Var->hasReg());
  Str << "\tpush\t";
  Var->emit(Func);
  Str << "\n";
}

void InstX8632Push::emitIAS(const Cfg *Func) const {
  assert(getSrcSize() == 1);
  // Push is currently only used for saving GPRs.
  Variable *Var = llvm::cast<Variable>(getSrc(0));
  assert(Var->hasReg());
  x86::AssemblerX86 *Asm = Func->getAssembler<x86::AssemblerX86>();
  intptr_t StartPosition = Asm->GetPosition();
  Asm->pushl(RegX8632::getEncodedGPR(Var->getRegNum()));
  emitIASBytes(Func, Asm, StartPosition);
}

void InstX8632Push::dump(const Cfg *Func) const {
  Ostream &Str = Func->getContext()->getStrDump();
  Str << "push." << getSrc(0)->getType() << " ";
  dumpSources(Func);
}

template <> void InstX8632Psll::emit(const Cfg *Func) const {
  assert(getDest()->getType() == IceType_v8i16 ||
         getDest()->getType() == IceType_v8i1 ||
         getDest()->getType() == IceType_v4i32 ||
         getDest()->getType() == IceType_v4i1);
  char buf[30];
  snprintf(buf, llvm::array_lengthof(buf), "psll%s",
           TypeX8632Attributes[getDest()->getType()].PackString);
  emitTwoAddress(buf, this, Func);
}

template <> void InstX8632Psra::emit(const Cfg *Func) const {
  assert(getDest()->getType() == IceType_v8i16 ||
         getDest()->getType() == IceType_v8i1 ||
         getDest()->getType() == IceType_v4i32 ||
         getDest()->getType() == IceType_v4i1);
  char buf[30];
  snprintf(buf, llvm::array_lengthof(buf), "psra%s",
           TypeX8632Attributes[getDest()->getType()].PackString);
  emitTwoAddress(buf, this, Func);
}

void InstX8632Ret::emit(const Cfg *Func) const {
  Ostream &Str = Func->getContext()->getStrEmit();
  Str << "\tret\n";
}

void InstX8632Ret::emitIAS(const Cfg *Func) const {
  x86::AssemblerX86 *Asm = Func->getAssembler<x86::AssemblerX86>();
  intptr_t StartPosition = Asm->GetPosition();
  Asm->ret();
  emitIASBytes(Func, Asm, StartPosition);
}

void InstX8632Ret::dump(const Cfg *Func) const {
  Ostream &Str = Func->getContext()->getStrDump();
  Type Ty = (getSrcSize() == 0 ? IceType_void : getSrc(0)->getType());
  Str << "ret." << Ty << " ";
  dumpSources(Func);
}

void InstX8632Xadd::emit(const Cfg *Func) const {
  Ostream &Str = Func->getContext()->getStrEmit();
  if (Locked) {
    Str << "\tlock";
  }
  Str << "\txadd\t";
  getSrc(0)->emit(Func);
  Str << ", ";
  getSrc(1)->emit(Func);
  Str << "\n";
}

void InstX8632Xadd::emitIAS(const Cfg *Func) const {
  assert(getSrcSize() == 2);
  x86::AssemblerX86 *Asm = Func->getAssembler<x86::AssemblerX86>();
  intptr_t StartPosition = Asm->GetPosition();
  Type Ty = getSrc(0)->getType();
  const OperandX8632Mem *Mem = llvm::cast<OperandX8632Mem>(getSrc(0));
  assert(Mem->getSegmentRegister() == OperandX8632Mem::DefaultSegment);
  const x86::Address Addr = Mem->toAsmAddress(Asm);
  const Variable *VarReg = llvm::cast<Variable>(getSrc(1));
  assert(VarReg->hasReg());
  const RegX8632::GPRRegister Reg =
      RegX8632::getEncodedGPR(VarReg->getRegNum());
  if (Locked) {
    Asm->lock();
  }
  Asm->xadd(Ty, Addr, Reg);
  emitIASBytes(Func, Asm, StartPosition);
}

void InstX8632Xadd::dump(const Cfg *Func) const {
  Ostream &Str = Func->getContext()->getStrDump();
  if (Locked) {
    Str << "lock ";
  }
  Type Ty = getSrc(0)->getType();
  Str << "xadd." << Ty << " ";
  dumpSources(Func);
}

void InstX8632Xchg::emit(const Cfg *Func) const {
  Ostream &Str = Func->getContext()->getStrEmit();
  Str << "\txchg\t";
  getSrc(0)->emit(Func);
  Str << ", ";
  getSrc(1)->emit(Func);
  Str << "\n";
}

void InstX8632Xchg::emitIAS(const Cfg *Func) const {
  assert(getSrcSize() == 2);
  x86::AssemblerX86 *Asm = Func->getAssembler<x86::AssemblerX86>();
  intptr_t StartPosition = Asm->GetPosition();
  Type Ty = getSrc(0)->getType();
  const OperandX8632Mem *Mem = llvm::cast<OperandX8632Mem>(getSrc(0));
  assert(Mem->getSegmentRegister() == OperandX8632Mem::DefaultSegment);
  const x86::Address Addr = Mem->toAsmAddress(Asm);
  const Variable *VarReg = llvm::cast<Variable>(getSrc(1));
  assert(VarReg->hasReg());
  const RegX8632::GPRRegister Reg =
      RegX8632::getEncodedGPR(VarReg->getRegNum());
  Asm->xchg(Ty, Addr, Reg);
  emitIASBytes(Func, Asm, StartPosition);
}

void InstX8632Xchg::dump(const Cfg *Func) const {
  Ostream &Str = Func->getContext()->getStrDump();
  Type Ty = getSrc(0)->getType();
  Str << "xchg." << Ty << " ";
  dumpSources(Func);
}

void OperandX8632Mem::emit(const Cfg *Func) const {
  Ostream &Str = Func->getContext()->getStrEmit();
  Str << TypeX8632Attributes[getType()].WidthString << " ";
  if (SegmentReg != DefaultSegment) {
    assert(SegmentReg >= 0 && SegmentReg < SegReg_NUM);
    Str << InstX8632SegmentRegNames[SegmentReg] << ":";
  }
  // TODO: The following is an almost verbatim paste of dump().
  bool Dumped = false;
  Str << "[";
  if (Base) {
    Base->emit(Func);
    Dumped = true;
  }
  if (Index) {
    assert(Base);
    Str << "+";
    if (Shift > 0)
      Str << (1u << Shift) << "*";
    Index->emit(Func);
    Dumped = true;
  }
  // Pretty-print the Offset.
  bool OffsetIsZero = false;
  bool OffsetIsNegative = false;
  if (Offset == NULL) {
    OffsetIsZero = true;
  } else if (ConstantInteger32 *CI =
                 llvm::dyn_cast<ConstantInteger32>(Offset)) {
    OffsetIsZero = (CI->getValue() == 0);
    OffsetIsNegative = (static_cast<int32_t>(CI->getValue()) < 0);
  } else {
    assert(llvm::isa<ConstantRelocatable>(Offset));
  }
  if (Dumped) {
    if (!OffsetIsZero) {     // Suppress if Offset is known to be 0
      if (!OffsetIsNegative) // Suppress if Offset is known to be negative
        Str << "+";
      Offset->emit(Func);
    }
  } else {
    // There is only the offset.
    Offset->emit(Func);
  }
  Str << "]";
}

void OperandX8632Mem::dump(const Cfg *Func, Ostream &Str) const {
  if (SegmentReg != DefaultSegment) {
    assert(SegmentReg >= 0 && SegmentReg < SegReg_NUM);
    Str << InstX8632SegmentRegNames[SegmentReg] << ":";
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
  if (Offset == NULL) {
    OffsetIsZero = true;
  } else if (ConstantInteger32 *CI =
                 llvm::dyn_cast<ConstantInteger32>(Offset)) {
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

void OperandX8632Mem::emitSegmentOverride(x86::AssemblerX86 *Asm) const {
  if (SegmentReg != DefaultSegment) {
    assert(SegmentReg >= 0 && SegmentReg < SegReg_NUM);
    Asm->EmitSegmentOverride(InstX8632SegmentPrefixes[SegmentReg]);
  }
}

x86::Address OperandX8632Mem::toAsmAddress(Assembler *Asm) const {
  int32_t Disp = 0;
  AssemblerFixup *Fixup = NULL;
  // Determine the offset (is it relocatable?)
  if (getOffset()) {
    if (ConstantInteger32 *CI =
            llvm::dyn_cast<ConstantInteger32>(getOffset())) {
      Disp = static_cast<int32_t>(CI->getValue());
    } else if (ConstantRelocatable *CR =
                   llvm::dyn_cast<ConstantRelocatable>(getOffset())) {
      Fixup = x86::DisplacementRelocation::create(Asm, FK_Abs_4, CR);
    } else {
      llvm_unreachable("Unexpected offset type");
    }
  }

  // Now convert to the various possible forms.
  if (getBase() && getIndex()) {
    return x86::Address(RegX8632::getEncodedGPR(getBase()->getRegNum()),
                        RegX8632::getEncodedGPR(getIndex()->getRegNum()),
                        x86::ScaleFactor(getShift()), Disp);
  } else if (getBase()) {
    return x86::Address(RegX8632::getEncodedGPR(getBase()->getRegNum()), Disp);
  } else if (getIndex()) {
    return x86::Address(RegX8632::getEncodedGPR(getIndex()->getRegNum()),
                        x86::ScaleFactor(getShift()), Disp);
  } else if (Fixup) {
    // The fixup itself has an offset, so Disp should still be 0.
    assert(Disp == 0);
    return x86::Address::Absolute(Fixup);
  } else {
    return x86::Address::Absolute(Disp);
  }
}

x86::Address VariableSplit::toAsmAddress(const Cfg *Func) const {
  assert(!Var->hasReg());
  const TargetLowering *Target = Func->getTarget();
  int32_t Offset =
      Var->getStackOffset() + Target->getStackAdjustment() + getOffset();
  return x86::Address(RegX8632::getEncodedGPR(Target->getFrameOrStackReg()),
                      Offset);
}

void VariableSplit::emit(const Cfg *Func) const {
  Ostream &Str = Func->getContext()->getStrEmit();
  assert(!Var->hasReg());
  // The following is copied/adapted from TargetX8632::emitVariable().
  const TargetLowering *Target = Func->getTarget();
  const Type Ty = IceType_i32;
  Str << TypeX8632Attributes[Ty].WidthString << " ["
      << Target->getRegName(Target->getFrameOrStackReg(), Ty);
  int32_t Offset =
      Var->getStackOffset() + Target->getStackAdjustment() + getOffset();
  if (Offset) {
    if (Offset > 0)
      Str << "+";
    Str << Offset;
  }
  Str << "]";
}

void VariableSplit::dump(const Cfg *Func, Ostream &Str) const {
  switch (Part) {
  case Low:
    Str << "low";
    break;
  case High:
    Str << "high";
    break;
  default:
    Str << "???";
    break;
  }
  Str << "(";
  if (Func)
    Var->dump(Func);
  else
    Var->dump(Str);
  Str << ")";
}

} // end of namespace Ice
