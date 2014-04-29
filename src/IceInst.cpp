//===- subzero/src/IceInst.cpp - High-level instruction implementation ----===//
//
//                        The Subzero Code Generator
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements the Inst class, primarily the various
// subclass constructors and dump routines.
//
//===----------------------------------------------------------------------===//

#include "IceCfg.h"
#include "IceCfgNode.h"
#include "IceInst.h"
#include "IceOperand.h"

namespace Ice {

namespace {

// Using non-anonymous struct so that array_lengthof works.
const struct _InstArithmeticAttributes {
  const char *DisplayString;
  bool IsCommutative;
} InstArithmeticAttributes[] = {
#define X(tag, str, commutative)                                               \
  { str, commutative }                                                         \
  ,
    ICEINSTARITHMETIC_TABLE
#undef X
  };
const size_t InstArithmeticAttributesSize =
    llvm::array_lengthof(InstArithmeticAttributes);

// Using non-anonymous struct so that array_lengthof works.
const struct _InstCastAttributes {
  const char *DisplayString;
} InstCastAttributes[] = {
#define X(tag, str)                                                            \
  { str }                                                                      \
  ,
    ICEINSTCAST_TABLE
#undef X
  };
const size_t InstCastAttributesSize = llvm::array_lengthof(InstCastAttributes);

// Using non-anonymous struct so that array_lengthof works.
const struct _InstFcmpAttributes {
  const char *DisplayString;
} InstFcmpAttributes[] = {
#define X(tag, str)                                                            \
  { str }                                                                      \
  ,
    ICEINSTFCMP_TABLE
#undef X
  };
const size_t InstFcmpAttributesSize = llvm::array_lengthof(InstFcmpAttributes);

// Using non-anonymous struct so that array_lengthof works.
const struct _InstIcmpAttributes {
  const char *DisplayString;
} InstIcmpAttributes[] = {
#define X(tag, str)                                                            \
  { str }                                                                      \
  ,
    ICEINSTICMP_TABLE
#undef X
  };
const size_t InstIcmpAttributesSize = llvm::array_lengthof(InstIcmpAttributes);

} // end of anonymous namespace

Inst::Inst(Cfg *Func, InstKind Kind, SizeT MaxSrcs, Variable *Dest)
    : Kind(Kind), Number(Func->newInstNumber()), Deleted(false),
      HasSideEffects(false), Dest(Dest), MaxSrcs(MaxSrcs), NumSrcs(0),
      Srcs(Func->allocateArrayOf<Operand *>(MaxSrcs)) {}

void Inst::updateVars(CfgNode *Node) {
  if (Dest)
    Dest->setDefinition(this, Node);

  SizeT VarIndex = 0;
  for (SizeT I = 0; I < getSrcSize(); ++I) {
    Operand *Src = getSrc(I);
    SizeT NumVars = Src->getNumVars();
    for (SizeT J = 0; J < NumVars; ++J, ++VarIndex) {
      Variable *Var = Src->getVar(J);
      Var->setUse(this, Node);
    }
  }
}

InstAlloca::InstAlloca(Cfg *Func, Operand *ByteCount, uint32_t AlignInBytes,
                       Variable *Dest)
    : Inst(Func, Inst::Alloca, 1, Dest), AlignInBytes(AlignInBytes) {
  // Verify AlignInBytes is 0 or a power of 2.
  assert(AlignInBytes == 0 || llvm::isPowerOf2_32(AlignInBytes));
  addSource(ByteCount);
}

InstArithmetic::InstArithmetic(Cfg *Func, OpKind Op, Variable *Dest,
                               Operand *Source1, Operand *Source2)
    : Inst(Func, Inst::Arithmetic, 2, Dest), Op(Op) {
  addSource(Source1);
  addSource(Source2);
}

bool InstArithmetic::isCommutative() const {
  return InstArithmeticAttributes[getOp()].IsCommutative;
}

InstAssign::InstAssign(Cfg *Func, Variable *Dest, Operand *Source)
    : Inst(Func, Inst::Assign, 1, Dest) {
  addSource(Source);
}

// If TargetTrue==TargetFalse, we turn it into an unconditional
// branch.  This ensures that, along with the 'switch' instruction
// semantics, there is at most one edge from one node to another.
InstBr::InstBr(Cfg *Func, Operand *Source, CfgNode *TargetTrue,
               CfgNode *TargetFalse)
    : Inst(Func, Inst::Br, 1, NULL), TargetFalse(TargetFalse),
      TargetTrue(TargetTrue) {
  if (TargetTrue == TargetFalse) {
    TargetTrue = NULL; // turn into unconditional version
  } else {
    addSource(Source);
  }
}

InstBr::InstBr(Cfg *Func, CfgNode *Target)
    : Inst(Func, Inst::Br, 0, NULL), TargetFalse(Target), TargetTrue(NULL) {}

NodeList InstBr::getTerminatorEdges() const {
  NodeList OutEdges;
  OutEdges.push_back(TargetFalse);
  if (TargetTrue)
    OutEdges.push_back(TargetTrue);
  return OutEdges;
}

InstCast::InstCast(Cfg *Func, OpKind CastKind, Variable *Dest, Operand *Source)
    : Inst(Func, Inst::Cast, 1, Dest), CastKind(CastKind) {
  addSource(Source);
}

InstFcmp::InstFcmp(Cfg *Func, FCond Condition, Variable *Dest, Operand *Source1,
                   Operand *Source2)
    : Inst(Func, Inst::Fcmp, 2, Dest), Condition(Condition) {
  addSource(Source1);
  addSource(Source2);
}

InstIcmp::InstIcmp(Cfg *Func, ICond Condition, Variable *Dest, Operand *Source1,
                   Operand *Source2)
    : Inst(Func, Inst::Icmp, 2, Dest), Condition(Condition) {
  addSource(Source1);
  addSource(Source2);
}

InstLoad::InstLoad(Cfg *Func, Variable *Dest, Operand *SourceAddr)
    : Inst(Func, Inst::Load, 1, Dest) {
  addSource(SourceAddr);
}

InstPhi::InstPhi(Cfg *Func, SizeT MaxSrcs, Variable *Dest)
    : Inst(Func, Phi, MaxSrcs, Dest) {
  Labels = Func->allocateArrayOf<CfgNode *>(MaxSrcs);
}

// TODO: A Switch instruction (and maybe others) can add duplicate
// edges.  We may want to de-dup Phis and validate consistency (i.e.,
// the source operands are the same for duplicate edges), though it
// seems the current lowering code is OK with this situation.
void InstPhi::addArgument(Operand *Source, CfgNode *Label) {
  Labels[getSrcSize()] = Label;
  addSource(Source);
}

InstRet::InstRet(Cfg *Func, Operand *RetValue)
    : Inst(Func, Ret, RetValue ? 1 : 0, NULL) {
  if (RetValue)
    addSource(RetValue);
}

InstSelect::InstSelect(Cfg *Func, Variable *Dest, Operand *Condition,
                       Operand *SourceTrue, Operand *SourceFalse)
    : Inst(Func, Inst::Select, 3, Dest) {
  assert(Condition->getType() == IceType_i1);
  addSource(Condition);
  addSource(SourceTrue);
  addSource(SourceFalse);
}

InstStore::InstStore(Cfg *Func, Operand *Data, Operand *Addr)
    : Inst(Func, Inst::Store, 2, NULL) {
  addSource(Data);
  addSource(Addr);
}

InstSwitch::InstSwitch(Cfg *Func, SizeT NumCases, Operand *Source,
                       CfgNode *LabelDefault)
    : Inst(Func, Inst::Switch, 1, NULL), LabelDefault(LabelDefault),
      NumCases(NumCases) {
  addSource(Source);
  Values = Func->allocateArrayOf<uint64_t>(NumCases);
  Labels = Func->allocateArrayOf<CfgNode *>(NumCases);
  // Initialize in case buggy code doesn't set all entries
  for (SizeT I = 0; I < NumCases; ++I) {
    Values[I] = 0;
    Labels[I] = NULL;
  }
}

void InstSwitch::addBranch(SizeT CaseIndex, uint64_t Value, CfgNode *Label) {
  assert(CaseIndex < NumCases);
  Values[CaseIndex] = Value;
  Labels[CaseIndex] = Label;
}

NodeList InstSwitch::getTerminatorEdges() const {
  NodeList OutEdges;
  OutEdges.push_back(LabelDefault);
  for (SizeT I = 0; I < NumCases; ++I) {
    OutEdges.push_back(Labels[I]);
  }
  return OutEdges;
}

InstUnreachable::InstUnreachable(Cfg *Func)
    : Inst(Func, Inst::Unreachable, 0, NULL) {}

// ======================== Dump routines ======================== //

void Inst::dumpDecorated(const Cfg *Func) const {
  Ostream &Str = Func->getContext()->getStrDump();
  if (!Func->getContext()->isVerbose(IceV_Deleted) && isDeleted())
    return;
  if (Func->getContext()->isVerbose(IceV_InstNumbers)) {
    char buf[30];
    int32_t Number = getNumber();
    if (Number < 0)
      snprintf(buf, llvm::array_lengthof(buf), "[XXX]");
    else
      snprintf(buf, llvm::array_lengthof(buf), "[%3d]", Number);
    Str << buf;
  }
  Str << "  ";
  if (isDeleted())
    Str << "  //";
  dump(Func);
  Str << "\n";
}

void Inst::dump(const Cfg *Func) const {
  Ostream &Str = Func->getContext()->getStrDump();
  dumpDest(Func);
  Str << " =~ ";
  dumpSources(Func);
}

void Inst::dumpSources(const Cfg *Func) const {
  Ostream &Str = Func->getContext()->getStrDump();
  for (SizeT I = 0; I < getSrcSize(); ++I) {
    if (I > 0)
      Str << ", ";
    getSrc(I)->dump(Func);
  }
}

void Inst::dumpDest(const Cfg *Func) const {
  if (getDest())
    getDest()->dump(Func);
}

void InstAlloca::dump(const Cfg *Func) const {
  Ostream &Str = Func->getContext()->getStrDump();
  dumpDest(Func);
  Str << " = alloca i8, i32 ";
  getSizeInBytes()->dump(Func);
  Str << ", align " << getAlignInBytes();
}

void InstArithmetic::dump(const Cfg *Func) const {
  Ostream &Str = Func->getContext()->getStrDump();
  dumpDest(Func);
  Str << " = " << InstArithmeticAttributes[getOp()].DisplayString << " "
      << getDest()->getType() << " ";
  dumpSources(Func);
}

void InstAssign::dump(const Cfg *Func) const {
  Ostream &Str = Func->getContext()->getStrDump();
  dumpDest(Func);
  Str << " = " << getDest()->getType() << " ";
  dumpSources(Func);
}

void InstBr::dump(const Cfg *Func) const {
  Ostream &Str = Func->getContext()->getStrDump();
  dumpDest(Func);
  Str << "br ";
  if (!isUnconditional()) {
    Str << "i1 ";
    getCondition()->dump(Func);
    Str << ", label %" << getTargetTrue()->getName() << ", ";
  }
  Str << "label %" << getTargetFalse()->getName();
}

void InstCall::dump(const Cfg *Func) const {
  Ostream &Str = Func->getContext()->getStrDump();
  if (getDest()) {
    dumpDest(Func);
    Str << " = ";
  }
  Str << "call ";
  if (getDest())
    Str << getDest()->getType();
  else
    Str << "void";
  Str << " ";
  getCallTarget()->dump(Func);
  Str << "(";
  for (SizeT I = 0; I < getNumArgs(); ++I) {
    if (I > 0)
      Str << ", ";
    Str << getArg(I)->getType() << " ";
    getArg(I)->dump(Func);
  }
  Str << ")";
}

void InstCast::dump(const Cfg *Func) const {
  Ostream &Str = Func->getContext()->getStrDump();
  dumpDest(Func);
  Str << " = " << InstCastAttributes[getCastKind()].DisplayString << " "
      << getSrc(0)->getType() << " ";
  dumpSources(Func);
  Str << " to " << getDest()->getType();
}

void InstIcmp::dump(const Cfg *Func) const {
  Ostream &Str = Func->getContext()->getStrDump();
  dumpDest(Func);
  Str << " = icmp " << InstIcmpAttributes[getCondition()].DisplayString << " "
      << getSrc(0)->getType() << " ";
  dumpSources(Func);
}

void InstFcmp::dump(const Cfg *Func) const {
  Ostream &Str = Func->getContext()->getStrDump();
  dumpDest(Func);
  Str << " = fcmp " << InstFcmpAttributes[getCondition()].DisplayString << " "
      << getSrc(0)->getType() << " ";
  dumpSources(Func);
}

void InstLoad::dump(const Cfg *Func) const {
  Ostream &Str = Func->getContext()->getStrDump();
  dumpDest(Func);
  Type Ty = getDest()->getType();
  Str << " = load " << Ty << "* ";
  dumpSources(Func);
  Str << ", align " << typeAlignInBytes(Ty);
}

void InstStore::dump(const Cfg *Func) const {
  Ostream &Str = Func->getContext()->getStrDump();
  Type Ty = getData()->getType();
  Str << "store " << Ty << " ";
  getData()->dump(Func);
  Str << ", " << Ty << "* ";
  getAddr()->dump(Func);
  Str << ", align " << typeAlignInBytes(Ty);
}

void InstSwitch::dump(const Cfg *Func) const {
  Ostream &Str = Func->getContext()->getStrDump();
  Type Ty = getComparison()->getType();
  Str << "switch " << Ty << " ";
  getSrc(0)->dump(Func);
  Str << ", label %" << getLabelDefault()->getName() << " [\n";
  for (SizeT I = 0; I < getNumCases(); ++I) {
    Str << "    " << Ty << " " << getValue(I) << ", label %"
        << getLabel(I)->getName() << "\n";
  }
  Str << "  ]";
}

void InstPhi::dump(const Cfg *Func) const {
  Ostream &Str = Func->getContext()->getStrDump();
  dumpDest(Func);
  Str << " = phi " << getDest()->getType() << " ";
  for (SizeT I = 0; I < getSrcSize(); ++I) {
    if (I > 0)
      Str << ", ";
    Str << "[ ";
    getSrc(I)->dump(Func);
    Str << ", %" << Labels[I]->getName() << " ]";
  }
}

void InstRet::dump(const Cfg *Func) const {
  Ostream &Str = Func->getContext()->getStrDump();
  Type Ty = hasRetValue() ? getSrc(0)->getType() : IceType_void;
  Str << "ret " << Ty;
  if (hasRetValue()) {
    Str << " ";
    dumpSources(Func);
  }
}

void InstSelect::dump(const Cfg *Func) const {
  Ostream &Str = Func->getContext()->getStrDump();
  dumpDest(Func);
  Operand *Condition = getCondition();
  Operand *TrueOp = getTrueOperand();
  Operand *FalseOp = getFalseOperand();
  Str << " = select " << Condition->getType() << " ";
  Condition->dump(Func);
  Str << ", " << TrueOp->getType() << " ";
  TrueOp->dump(Func);
  Str << ", " << FalseOp->getType() << " ";
  FalseOp->dump(Func);
}

void InstUnreachable::dump(const Cfg *Func) const {
  Ostream &Str = Func->getContext()->getStrDump();
  Str << "unreachable";
}

} // end of namespace Ice
