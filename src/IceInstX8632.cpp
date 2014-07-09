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

#include "IceCfg.h"
#include "IceCfgNode.h"
#include "IceInst.h"
#include "IceInstX8632.h"
#include "IceTargetLoweringX8632.h"
#include "IceOperand.h"

namespace Ice {

namespace {

const struct InstX8632BrAttributes_ {
  const char *DisplayString;
  const char *EmitString;
} InstX8632BrAttributes[] = {
#define X(tag, dump, emit)                                                     \
  { dump, emit }                                                               \
  ,
    ICEINSTX8632BR_TABLE
#undef X
  };
const size_t InstX8632BrAttributesSize =
    llvm::array_lengthof(InstX8632BrAttributes);

const struct TypeX8632Attributes_ {
  const char *CvtString;   // i (integer), s (single FP), d (double FP)
  const char *SdSsString;  // ss, sd, or <blank>
  const char *WidthString; // {byte,word,dword,qword} ptr
} TypeX8632Attributes[] = {
#define X(tag, cvt, sdss, width)                                               \
  { cvt, "" sdss, width }                                                      \
  ,
    ICETYPEX8632_TABLE
#undef X
  };
const size_t TypeX8632AttributesSize =
    llvm::array_lengthof(TypeX8632Attributes);

const char *InstX8632SegmentRegNames[] = {
#define X(val, name)                                                           \
  name,
    SEG_REGX8632_TABLE
#undef X
};
const size_t InstX8632SegmentRegNamesSize =
    llvm::array_lengthof(InstX8632SegmentRegNames);

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
  return ".L" + Func->getFunctionName() + "$__" + buf;
}

InstX8632Br::InstX8632Br(Cfg *Func, CfgNode *TargetTrue, CfgNode *TargetFalse,
                         InstX8632Label *Label, InstX8632Br::BrCond Condition)
    : InstX8632(Func, InstX8632::Br, 0, NULL), Condition(Condition),
      TargetTrue(TargetTrue), TargetFalse(TargetFalse), Label(Label) {}

InstX8632Call::InstX8632Call(Cfg *Func, Variable *Dest, Operand *CallTarget)
    : InstX8632(Func, InstX8632::Call, 1, Dest) {
  HasSideEffects = true;
  addSource(CallTarget);
}

InstX8632Cdq::InstX8632Cdq(Cfg *Func, Variable *Dest, Operand *Source)
    : InstX8632(Func, InstX8632::Cdq, 1, Dest) {
  assert(Dest->getRegNum() == TargetX8632::Reg_edx);
  assert(llvm::isa<Variable>(Source));
  assert(llvm::dyn_cast<Variable>(Source)->getRegNum() == TargetX8632::Reg_eax);
  addSource(Source);
}

InstX8632Cvt::InstX8632Cvt(Cfg *Func, Variable *Dest, Operand *Source)
    : InstX8632(Func, InstX8632::Cvt, 1, Dest) {
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

InstX8632Mov::InstX8632Mov(Cfg *Func, Variable *Dest, Operand *Source)
    : InstX8632(Func, InstX8632::Mov, 1, Dest) {
  addSource(Source);
}

InstX8632Movp::InstX8632Movp(Cfg *Func, Variable *Dest, Operand *Source)
    : InstX8632(Func, InstX8632::Movp, 1, Dest) {
  addSource(Source);
}

InstX8632StoreQ::InstX8632StoreQ(Cfg *Func, Operand *Value, OperandX8632 *Mem)
    : InstX8632(Func, InstX8632::StoreQ, 2, NULL) {
  addSource(Value);
  addSource(Mem);
}

InstX8632Movq::InstX8632Movq(Cfg *Func, Variable *Dest, Operand *Source)
    : InstX8632(Func, InstX8632::Movq, 1, Dest) {
  addSource(Source);
}

InstX8632Movsx::InstX8632Movsx(Cfg *Func, Variable *Dest, Operand *Source)
    : InstX8632(Func, InstX8632::Movsx, 1, Dest) {
  addSource(Source);
}

InstX8632Movzx::InstX8632Movzx(Cfg *Func, Variable *Dest, Operand *Source)
    : InstX8632(Func, InstX8632::Movzx, 1, Dest) {
  addSource(Source);
}

InstX8632Fld::InstX8632Fld(Cfg *Func, Operand *Src)
    : InstX8632(Func, InstX8632::Fld, 1, NULL) {
  addSource(Src);
}

InstX8632Fstp::InstX8632Fstp(Cfg *Func, Variable *Dest)
    : InstX8632(Func, InstX8632::Fstp, 0, Dest) {}

InstX8632Pop::InstX8632Pop(Cfg *Func, Variable *Dest)
    : InstX8632(Func, InstX8632::Pop, 0, Dest) {}

InstX8632Push::InstX8632Push(Cfg *Func, Operand *Source,
                             bool SuppressStackAdjustment)
    : InstX8632(Func, InstX8632::Push, 1, NULL),
      SuppressStackAdjustment(SuppressStackAdjustment) {
  addSource(Source);
}

bool InstX8632Mov::isRedundantAssign() const {
  // TODO(stichnot): The isRedundantAssign() implementations for
  // InstX8632Mov, InstX8632Movp, and InstX8632Movq are
  // identical. Consolidate them.
  Variable *Src = llvm::dyn_cast<Variable>(getSrc(0));
  if (Src == NULL)
    return false;
  if (getDest()->hasReg() && getDest()->getRegNum() == Src->getRegNum()) {
    // TODO: On x86-64, instructions like "mov eax, eax" are used to
    // clear the upper 32 bits of rax.  We need to recognize and
    // preserve these.
    return true;
  }
  if (!getDest()->hasReg() && !Src->hasReg() &&
      Dest->getStackOffset() == Src->getStackOffset())
    return true;
  return false;
}

bool InstX8632Movp::isRedundantAssign() const {
  Variable *Src = llvm::dyn_cast<Variable>(getSrc(0));
  if (Src == NULL)
    return false;
  if (getDest()->hasReg() && getDest()->getRegNum() == Src->getRegNum()) {
    return true;
  }
  if (!getDest()->hasReg() && !Src->hasReg() &&
      Dest->getStackOffset() == Src->getStackOffset())
    return true;
  return false;
}

bool InstX8632Movq::isRedundantAssign() const {
  Variable *Src = llvm::dyn_cast<Variable>(getSrc(0));
  if (Src == NULL)
    return false;
  if (getDest()->hasReg() && getDest()->getRegNum() == Src->getRegNum()) {
    return true;
  }
  if (!getDest()->hasReg() && !Src->hasReg() &&
      Dest->getStackOffset() == Src->getStackOffset())
    return true;
  return false;
}

InstX8632Sqrtss::InstX8632Sqrtss(Cfg *Func, Variable *Dest, Operand *Source)
    : InstX8632(Func, InstX8632::Sqrtss, 1, Dest) {
  addSource(Source);
}

InstX8632Ret::InstX8632Ret(Cfg *Func, Variable *Source)
    : InstX8632(Func, InstX8632::Ret, Source ? 1 : 0, NULL) {
  if (Source)
    addSource(Source);
}

InstX8632Xadd::InstX8632Xadd(Cfg *Func, Operand *Dest, Variable *Source,
                             bool Locked)
    : InstX8632(Func, InstX8632::Xadd, 2, llvm::dyn_cast<Variable>(Dest)),
      Locked(Locked) {
  HasSideEffects = Locked;
  addSource(Dest);
  addSource(Source);
}

// ======================== Dump routines ======================== //

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

  if (Condition == Br_None) {
    Str << "jmp";
  } else {
    Str << InstX8632BrAttributes[Condition].EmitString;
  }

  if (Label) {
    Str << "\t" << Label->getName(Func) << "\n";
  } else {
    if (Condition == Br_None) {
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

  if (Condition == Br_None) {
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
    if (ShiftReg && ShiftReg->getRegNum() == TargetX8632::Reg_ecx) {
      Str << "cl";
      EmittedSrc1 = true;
    }
  }
  if (!EmittedSrc1)
    Inst->getSrc(1)->emit(Func);
  Str << "\n";
}

template <> const char *InstX8632Add::Opcode = "add";
template <> const char *InstX8632Adc::Opcode = "adc";
template <> const char *InstX8632Addss::Opcode = "addss";
template <> const char *InstX8632Sub::Opcode = "sub";
template <> const char *InstX8632Subss::Opcode = "subss";
template <> const char *InstX8632Sbb::Opcode = "sbb";
template <> const char *InstX8632And::Opcode = "and";
template <> const char *InstX8632Or::Opcode = "or";
template <> const char *InstX8632Xor::Opcode = "xor";
template <> const char *InstX8632Pxor::Opcode = "pxor";
template <> const char *InstX8632Imul::Opcode = "imul";
template <> const char *InstX8632Mulss::Opcode = "mulss";
template <> const char *InstX8632Div::Opcode = "div";
template <> const char *InstX8632Idiv::Opcode = "idiv";
template <> const char *InstX8632Divss::Opcode = "divss";
template <> const char *InstX8632Shl::Opcode = "shl";
template <> const char *InstX8632Shr::Opcode = "shr";
template <> const char *InstX8632Sar::Opcode = "sar";

template <> void InstX8632Addss::emit(const Cfg *Func) const {
  char buf[30];
  snprintf(buf, llvm::array_lengthof(buf), "add%s",
           TypeX8632Attributes[getDest()->getType()].SdSsString);
  emitTwoAddress(buf, this, Func);
}

template <> void InstX8632Subss::emit(const Cfg *Func) const {
  char buf[30];
  snprintf(buf, llvm::array_lengthof(buf), "sub%s",
           TypeX8632Attributes[getDest()->getType()].SdSsString);
  emitTwoAddress(buf, this, Func);
}

template <> void InstX8632Mulss::emit(const Cfg *Func) const {
  char buf[30];
  snprintf(buf, llvm::array_lengthof(buf), "mul%s",
           TypeX8632Attributes[getDest()->getType()].SdSsString);
  emitTwoAddress(buf, this, Func);
}

template <> void InstX8632Divss::emit(const Cfg *Func) const {
  char buf[30];
  snprintf(buf, llvm::array_lengthof(buf), "div%s",
           TypeX8632Attributes[getDest()->getType()].SdSsString);
  emitTwoAddress(buf, this, Func);
}

template <> void InstX8632Imul::emit(const Cfg *Func) const {
  Ostream &Str = Func->getContext()->getStrEmit();
  assert(getSrcSize() == 2);
  if (getDest()->getType() == IceType_i8) {
    // The 8-bit version of imul only allows the form "imul r/m8".
    Variable *Src0 = llvm::dyn_cast<Variable>(getSrc(0));
    assert(Src0 && Src0->getRegNum() == TargetX8632::Reg_eax);
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

void InstX8632Mul::emit(const Cfg *Func) const {
  Ostream &Str = Func->getContext()->getStrEmit();
  assert(getSrcSize() == 2);
  assert(llvm::isa<Variable>(getSrc(0)));
  assert(llvm::dyn_cast<Variable>(getSrc(0))->getRegNum() ==
         TargetX8632::Reg_eax);
  assert(getDest()->getRegNum() == TargetX8632::Reg_eax); // TODO: allow edx?
  Str << "\tmul\t";
  getSrc(1)->emit(Func);
  Str << "\n";
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
    assert(ShiftReg->getRegNum() == TargetX8632::Reg_ecx);
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
    assert(ShiftReg->getRegNum() == TargetX8632::Reg_ecx);
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

void InstX8632Cdq::emit(const Cfg *Func) const {
  Ostream &Str = Func->getContext()->getStrEmit();
  assert(getSrcSize() == 1);
  Str << "\tcdq\n";
}

void InstX8632Cdq::dump(const Cfg *Func) const {
  Ostream &Str = Func->getContext()->getStrDump();
  dumpDest(Func);
  Str << " = cdq." << getSrc(0)->getType() << " ";
  dumpSources(Func);
}

void InstX8632Cvt::emit(const Cfg *Func) const {
  Ostream &Str = Func->getContext()->getStrEmit();
  assert(getSrcSize() == 1);
  Str << "\tcvts" << TypeX8632Attributes[getSrc(0)->getType()].CvtString << "2s"
      << TypeX8632Attributes[getDest()->getType()].CvtString << "\t";
  getDest()->emit(Func);
  Str << ", ";
  getSrc(0)->emit(Func);
  Str << "\n";
}

void InstX8632Cvt::dump(const Cfg *Func) const {
  Ostream &Str = Func->getContext()->getStrDump();
  dumpDest(Func);
  Str << " = cvts" << TypeX8632Attributes[getSrc(0)->getType()].CvtString
      << "2s" << TypeX8632Attributes[getDest()->getType()].CvtString << " ";
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

void InstX8632StoreQ::dump(const Cfg *Func) const {
  Ostream &Str = Func->getContext()->getStrDump();
  Str << "storeq." << getSrc(0)->getType() << " ";
  getSrc(1)->dump(Func);
  Str << ", ";
  getSrc(0)->dump(Func);
}

void InstX8632Mov::emit(const Cfg *Func) const {
  Ostream &Str = Func->getContext()->getStrEmit();
  assert(getSrcSize() == 1);
  Operand *Src = getSrc(0);
  // The llvm-mc assembler using Intel syntax has a bug in which "mov
  // reg, RelocatableConstant" does not generate the right instruction
  // with a relocation.  To work around, we emit "lea reg,
  // [RelocatableConstant]".  Also, the lowering and legalization is
  // changed to allow relocatable constants only in Assign and Call
  // instructions or in Mem operands.  TODO(stichnot): remove LEAHACK
  // once a proper emitter is used.
  bool UseLeaHack = llvm::isa<ConstantRelocatable>(Src);
  Str << "\t";
  if (UseLeaHack)
    Str << "lea";
  else
    Str << "mov" << TypeX8632Attributes[getDest()->getType()].SdSsString;
  Str << "\t";
  // For an integer truncation operation, src is wider than dest.
  // Ideally, we use a mov instruction whose data width matches the
  // narrower dest.  This is a problem if e.g. src is a register like
  // esi or si where there is no 8-bit version of the register.  To be
  // safe, we instead widen the dest to match src.  This works even
  // for stack-allocated dest variables because typeWidthOnStack()
  // pads to a 4-byte boundary even if only a lower portion is used.
  assert(Func->getTarget()->typeWidthInBytesOnStack(getDest()->getType()) ==
         Func->getTarget()->typeWidthInBytesOnStack(Src->getType()));
  getDest()->asType(Src->getType()).emit(Func);
  Str << ", ";
  Src->emit(Func);
  Str << "\n";
}

void InstX8632Mov::dump(const Cfg *Func) const {
  Ostream &Str = Func->getContext()->getStrDump();
  Str << "mov." << getDest()->getType() << " ";
  dumpDest(Func);
  Str << ", ";
  dumpSources(Func);
}

void InstX8632Movp::emit(const Cfg *Func) const {
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

void InstX8632Movp::dump(const Cfg *Func) const {
  Ostream &Str = Func->getContext()->getStrDump();
  Str << "movups." << getDest()->getType() << " ";
  dumpDest(Func);
  Str << ", ";
  dumpSources(Func);
}

void InstX8632Movq::emit(const Cfg *Func) const {
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

void InstX8632Movq::dump(const Cfg *Func) const {
  Ostream &Str = Func->getContext()->getStrDump();
  Str << "movq." << getDest()->getType() << " ";
  dumpDest(Func);
  Str << ", ";
  dumpSources(Func);
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

void InstX8632Fld::dump(const Cfg *Func) const {
  Ostream &Str = Func->getContext()->getStrDump();
  Str << "fld." << getSrc(0)->getType() << " ";
  dumpSources(Func);
}

void InstX8632Fstp::emit(const Cfg *Func) const {
  Ostream &Str = Func->getContext()->getStrEmit();
  assert(getSrcSize() == 0);
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

void InstX8632Fstp::dump(const Cfg *Func) const {
  Ostream &Str = Func->getContext()->getStrDump();
  dumpDest(Func);
  Str << " = fstp." << getDest()->getType() << ", st(0)";
  Str << "\n";
}

void InstX8632Pop::emit(const Cfg *Func) const {
  Ostream &Str = Func->getContext()->getStrEmit();
  assert(getSrcSize() == 0);
  Str << "\tpop\t";
  getDest()->emit(Func);
  Str << "\n";
}

void InstX8632Pop::dump(const Cfg *Func) const {
  Ostream &Str = Func->getContext()->getStrDump();
  dumpDest(Func);
  Str << " = pop." << getDest()->getType() << " ";
}

void InstX8632Push::emit(const Cfg *Func) const {
  Ostream &Str = Func->getContext()->getStrEmit();
  assert(getSrcSize() == 1);
  Type Ty = getSrc(0)->getType();
  Variable *Var = llvm::dyn_cast<Variable>(getSrc(0));
  if ((isVectorType(Ty) || Ty == IceType_f32 || Ty == IceType_f64) && Var &&
      Var->hasReg()) {
    // The xmm registers can't be directly pushed, so we fake it by
    // decrementing esp and then storing to [esp].
    Str << "\tsub\tesp, " << typeWidthInBytes(Ty) << "\n";
    if (!SuppressStackAdjustment)
      Func->getTarget()->updateStackAdjustment(typeWidthInBytes(Ty));
    if (isVectorType(Ty)) {
      Str << "\tmovups\txmmword ptr [esp], ";
    } else {
      Str << "\tmov" << TypeX8632Attributes[Ty].SdSsString << "\t"
          << TypeX8632Attributes[Ty].WidthString << " [esp], ";
    }
    getSrc(0)->emit(Func);
    Str << "\n";
  } else if (Ty == IceType_f64 && (!Var || !Var->hasReg())) {
    // A double on the stack has to be pushed as two halves.  Push the
    // upper half followed by the lower half for little-endian.  TODO:
    // implement.
    llvm_unreachable("Missing support for pushing doubles from memory");
  } else {
    Str << "\tpush\t";
    getSrc(0)->emit(Func);
    Str << "\n";
    if (!SuppressStackAdjustment)
      Func->getTarget()->updateStackAdjustment(4);
  }
}

void InstX8632Push::dump(const Cfg *Func) const {
  Ostream &Str = Func->getContext()->getStrDump();
  Str << "push." << getSrc(0)->getType() << " ";
  dumpSources(Func);
}

void InstX8632Ret::emit(const Cfg *Func) const {
  Ostream &Str = Func->getContext()->getStrEmit();
  Str << "\tret\n";
}

void InstX8632Ret::dump(const Cfg *Func) const {
  Ostream &Str = Func->getContext()->getStrDump();
  Type Ty = (getSrcSize() == 0 ? IceType_void : getSrc(0)->getType());
  Str << "ret." << Ty << " ";
  dumpSources(Func);
}

void InstX8632Sqrtss::emit(const Cfg *Func) const {
  Ostream &Str = Func->getContext()->getStrEmit();
  assert(getSrcSize() == 1);
  Type Ty = getSrc(0)->getType();
  assert(Ty == IceType_f32 || Ty == IceType_f64);
  Str << "\tsqrt" << TypeX8632Attributes[Ty].SdSsString << "\t";
  getDest()->emit(Func);
  Str << ", ";
  getSrc(0)->emit(Func);
  Str << "\n";
}

void InstX8632Sqrtss::dump(const Cfg *Func) const {
  Ostream &Str = Func->getContext()->getStrDump();
  dumpDest(Func);
  Str << " = sqrt." << getDest()->getType() << " ";
  dumpSources(Func);
}

void InstX8632Xadd::emit(const Cfg *Func) const {
  Ostream &Str = Func->getContext()->getStrEmit();
  if (Locked) {
    Str << "\tlock xadd ";
  } else {
    Str << "\txadd\t";
  }
  getSrc(0)->emit(Func);
  Str << ", ";
  getSrc(1)->emit(Func);
  Str << "\n";
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

void OperandX8632::dump(const Cfg *Func) const {
  Ostream &Str = Func->getContext()->getStrDump();
  Str << "<OperandX8632>";
}

void OperandX8632Mem::emit(const Cfg *Func) const {
  Ostream &Str = Func->getContext()->getStrEmit();
  Str << TypeX8632Attributes[getType()].WidthString << " ";
  if (SegmentReg != DefaultSegment) {
    assert(SegmentReg >= 0 &&
           static_cast<size_t>(SegmentReg) < InstX8632SegmentRegNamesSize);
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
  } else if (ConstantInteger *CI = llvm::dyn_cast<ConstantInteger>(Offset)) {
    OffsetIsZero = (CI->getValue() == 0);
    OffsetIsNegative = (static_cast<int64_t>(CI->getValue()) < 0);
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

void OperandX8632Mem::dump(const Cfg *Func) const {
  Ostream &Str = Func->getContext()->getStrDump();
  if (SegmentReg != DefaultSegment) {
    assert(SegmentReg >= 0 &&
           static_cast<size_t>(SegmentReg) < InstX8632SegmentRegNamesSize);
    Str << InstX8632SegmentRegNames[SegmentReg] << ":";
  }
  bool Dumped = false;
  Str << "[";
  if (Base) {
    Base->dump(Func);
    Dumped = true;
  }
  if (Index) {
    assert(Base);
    Str << "+";
    if (Shift > 0)
      Str << (1u << Shift) << "*";
    Index->dump(Func);
    Dumped = true;
  }
  // Pretty-print the Offset.
  bool OffsetIsZero = false;
  bool OffsetIsNegative = false;
  if (Offset == NULL) {
    OffsetIsZero = true;
  } else if (ConstantInteger *CI = llvm::dyn_cast<ConstantInteger>(Offset)) {
    OffsetIsZero = (CI->getValue() == 0);
    OffsetIsNegative = (static_cast<int64_t>(CI->getValue()) < 0);
  }
  if (Dumped) {
    if (!OffsetIsZero) {     // Suppress if Offset is known to be 0
      if (!OffsetIsNegative) // Suppress if Offset is known to be negative
        Str << "+";
      Offset->dump(Func);
    }
  } else {
    // There is only the offset.
    Offset->dump(Func);
  }
  Str << "]";
}

void VariableSplit::emit(const Cfg *Func) const {
  Ostream &Str = Func->getContext()->getStrEmit();
  assert(Var->getLocalUseNode() == NULL ||
         Var->getLocalUseNode() == Func->getCurrentNode());
  assert(!Var->hasReg());
  // The following is copied/adapted from TargetX8632::emitVariable().
  const TargetLowering *Target = Func->getTarget();
  const Type Ty = IceType_i32;
  Str << TypeX8632Attributes[Ty].WidthString << " ["
      << Target->getRegName(Target->getFrameOrStackReg(), Ty);
  int32_t Offset = Var->getStackOffset() + Target->getStackAdjustment();
  if (Part == High)
    Offset += 4;
  if (Offset) {
    if (Offset > 0)
      Str << "+";
    Str << Offset;
  }
  Str << "]";
}

void VariableSplit::dump(const Cfg *Func) const {
  Ostream &Str = Func->getContext()->getStrDump();
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
  Var->dump(Func);
  Str << ")";
}

} // end of namespace Ice
