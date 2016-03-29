//===- subzero/src/IceInst.cpp - High-level instruction implementation ----===//
//
//                        The Subzero Code Generator
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief Implements the Inst class, primarily the various subclass
/// constructors and dump routines.
///
//===----------------------------------------------------------------------===//

#include "IceInst.h"

#include "IceCfg.h"
#include "IceCfgNode.h"
#include "IceInstVarIter.h"
#include "IceLiveness.h"
#include "IceOperand.h"
#include "IceTargetLowering.h"

namespace Ice {

namespace {

// Using non-anonymous struct so that array_lengthof works.
const struct InstArithmeticAttributes_ {
  const char *DisplayString;
  bool IsCommutative;
} InstArithmeticAttributes[] = {
#define X(tag, str, commutative)                                               \
  { str, commutative }                                                         \
  ,
    ICEINSTARITHMETIC_TABLE
#undef X
};

// Using non-anonymous struct so that array_lengthof works.
const struct InstCastAttributes_ {
  const char *DisplayString;
} InstCastAttributes[] = {
#define X(tag, str)                                                            \
  { str }                                                                      \
  ,
    ICEINSTCAST_TABLE
#undef X
};

// Using non-anonymous struct so that array_lengthof works.
const struct InstFcmpAttributes_ {
  const char *DisplayString;
} InstFcmpAttributes[] = {
#define X(tag, str)                                                            \
  { str }                                                                      \
  ,
    ICEINSTFCMP_TABLE
#undef X
};

// Using non-anonymous struct so that array_lengthof works.
const struct InstIcmpAttributes_ {
  const char *DisplayString;
} InstIcmpAttributes[] = {
#define X(tag, str)                                                            \
  { str }                                                                      \
  ,
    ICEINSTICMP_TABLE
#undef X
};

} // end of anonymous namespace

Inst::Inst(Cfg *Func, InstKind Kind, SizeT MaxSrcs, Variable *Dest)
    : Kind(Kind), Number(Func->newInstNumber()), Dest(Dest), MaxSrcs(MaxSrcs),
      Srcs(Func->allocateArrayOf<Operand *>(MaxSrcs)), LiveRangesEnded(0) {}

const char *Inst::getInstName() const {
  if (!BuildDefs::dump())
    return "???";

  switch (Kind) {
#define X(InstrKind, name)                                                     \
  case InstrKind:                                                              \
    return name
    X(Unreachable, "unreachable");
    X(Alloca, "alloca");
    X(Arithmetic, "arithmetic");
    X(Br, "br");
    X(Call, "call");
    X(Cast, "cast");
    X(ExtractElement, "extractelement");
    X(Fcmp, "fcmp");
    X(Icmp, "icmp");
    X(IntrinsicCall, "intrinsiccall");
    X(InsertElement, "insertelement");
    X(Load, "load");
    X(Phi, "phi");
    X(Ret, "ret");
    X(Select, "select");
    X(Store, "store");
    X(Switch, "switch");
    X(Assign, "assign");
    X(BundleLock, "bundlelock");
    X(BundleUnlock, "bundleunlock");
    X(FakeDef, "fakedef");
    X(FakeUse, "fakeuse");
    X(FakeKill, "fakekill");
    X(JumpTable, "jumptable");
#undef X
  default:
    assert(Kind >= Target);
    return "target";
  }
}

// Assign the instruction a new number.
void Inst::renumber(Cfg *Func) {
  Number = isDeleted() ? NumberDeleted : Func->newInstNumber();
}

// Delete the instruction if its tentative Dead flag is still set after
// liveness analysis.
void Inst::deleteIfDead() {
  if (Dead)
    setDeleted();
}

// If Src is a Variable, it returns true if this instruction ends Src's live
// range. Otherwise, returns false.
bool Inst::isLastUse(const Operand *TestSrc) const {
  if (LiveRangesEnded == 0)
    return false; // early-exit optimization
  if (auto *TestVar = llvm::dyn_cast<const Variable>(TestSrc)) {
    LREndedBits Mask = LiveRangesEnded;
    FOREACH_VAR_IN_INST(Var, *this) {
      if (Var == TestVar) {
        // We've found where the variable is used in the instruction.
        return Mask & 1;
      }
      Mask >>= 1;
      if (Mask == 0)
        return false; // another early-exit optimization
    }
  }
  return false;
}

// Given an instruction like:
//   a = b + c + [x,y] + e
// which was created from OrigInst:
//   a = b + c + d + e
// with SpliceAssn spliced in:
//   d = [x,y]
//
// Reconstruct the LiveRangesEnded bitmask in this instruction by combining the
// LiveRangesEnded values of OrigInst and SpliceAssn. If operands d and [x,y]
// contain a different number of variables, then the bitmask position for e may
// be different in OrigInst and the current instruction, requiring extra shifts
// and masks in the computation. In the example above, OrigInst has variable e
// in bit position 3, whereas the current instruction has e in bit position 4
// because [x,y] consumes 2 bitmask slots while d only consumed 1.
//
// Additionally, set HasSideEffects if either OrigInst or SpliceAssn have
// HasSideEffects set.
void Inst::spliceLivenessInfo(Inst *OrigInst, Inst *SpliceAssn) {
  HasSideEffects |= OrigInst->HasSideEffects;
  HasSideEffects |= SpliceAssn->HasSideEffects;
  // Find the bitmask index of SpliceAssn's dest within OrigInst.
  Variable *SpliceDest = SpliceAssn->getDest();
  SizeT Index = 0;
  for (SizeT I = 0; I < OrigInst->getSrcSize(); ++I) {
    Operand *Src = OrigInst->getSrc(I);
    if (Src == SpliceDest) {
      LREndedBits LeftMask = OrigInst->LiveRangesEnded & ((1 << Index) - 1);
      LREndedBits RightMask = OrigInst->LiveRangesEnded >> (Index + 1);
      LiveRangesEnded = LeftMask | (SpliceAssn->LiveRangesEnded << Index) |
                        (RightMask << (Index + getSrc(I)->getNumVars()));
      return;
    }
    Index += getSrc(I)->getNumVars();
  }
  llvm::report_fatal_error("Failed to find splice operand");
}

void Inst::livenessLightweight(Cfg *Func, LivenessBV &Live) {
  assert(!isDeleted());
  resetLastUses();
  VariablesMetadata *VMetadata = Func->getVMetadata();
  FOREACH_VAR_IN_INST(Var, *this) {
    if (VMetadata->isMultiBlock(Var))
      continue;
    SizeT Index = Var->getIndex();
    if (Live[Index])
      continue;
    Live[Index] = true;
    setLastUse(IndexOfVarInInst(Var));
  }
}

bool Inst::liveness(InstNumberT InstNumber, LivenessBV &Live,
                    Liveness *Liveness, LiveBeginEndMap *LiveBegin,
                    LiveBeginEndMap *LiveEnd) {
  assert(!isDeleted());

  Dead = false;
  if (Dest) {
    SizeT VarNum = Liveness->getLiveIndex(Dest->getIndex());
    if (Live[VarNum]) {
      if (!isDestRedefined()) {
        Live[VarNum] = false;
        if (LiveBegin && Liveness->getRangeMask(Dest->getIndex())) {
          LiveBegin->push_back(std::make_pair(VarNum, InstNumber));
        }
      }
    } else {
      if (!hasSideEffects())
        Dead = true;
    }
  }
  if (Dead)
    return false;
  // Phi arguments only get added to Live in the predecessor node, but we still
  // need to update LiveRangesEnded.
  bool IsPhi = llvm::isa<InstPhi>(this);
  resetLastUses();
  FOREACH_VAR_IN_INST(Var, *this) {
    SizeT VarNum = Liveness->getLiveIndex(Var->getIndex());
    if (!Live[VarNum]) {
      setLastUse(IndexOfVarInInst(Var));
      if (!IsPhi) {
        Live[VarNum] = true;
        // For a variable in SSA form, its live range can end at most once in a
        // basic block. However, after lowering to two-address instructions, we
        // end up with sequences like "t=b;t+=c;a=t" where t's live range
        // begins and ends twice. ICE only allows a variable to have a single
        // liveness interval in a basic block (except for blocks where a
        // variable is live-in and live-out but there is a gap in the middle).
        // Therefore, this lowered sequence needs to represent a single
        // conservative live range for t. Since the instructions are being
        // traversed backwards, we make sure LiveEnd is only set once by
        // setting it only when LiveEnd[VarNum]==0 (sentinel value). Note that
        // it's OK to set LiveBegin multiple times because of the backwards
        // traversal.
        if (LiveEnd && Liveness->getRangeMask(Var->getIndex())) {
          // Ideally, we would verify that VarNum wasn't already added in this
          // block, but this can't be done very efficiently with LiveEnd as a
          // vector. Instead, livenessPostprocess() verifies this after the
          // vector has been sorted.
          LiveEnd->push_back(std::make_pair(VarNum, InstNumber));
        }
      }
    }
  }
  return true;
}

InstAlloca::InstAlloca(Cfg *Func, Variable *Dest, Operand *ByteCount,
                       uint32_t AlignInBytes)
    : InstHighLevel(Func, Inst::Alloca, 1, Dest), AlignInBytes(AlignInBytes) {
  // Verify AlignInBytes is 0 or a power of 2.
  assert(AlignInBytes == 0 || llvm::isPowerOf2_32(AlignInBytes));
  addSource(ByteCount);
}

InstArithmetic::InstArithmetic(Cfg *Func, OpKind Op, Variable *Dest,
                               Operand *Source1, Operand *Source2)
    : InstHighLevel(Func, Inst::Arithmetic, 2, Dest), Op(Op) {
  addSource(Source1);
  addSource(Source2);
}

const char *InstArithmetic::getInstName() const {
  if (!BuildDefs::dump())
    return "???";

  return InstArithmeticAttributes[getOp()].DisplayString;
}

const char *InstArithmetic::getOpName(OpKind Op) {
  return Op < InstArithmetic::_num ? InstArithmeticAttributes[Op].DisplayString
                                   : "???";
}

bool InstArithmetic::isCommutative() const {
  return InstArithmeticAttributes[getOp()].IsCommutative;
}

InstAssign::InstAssign(Cfg *Func, Variable *Dest, Operand *Source)
    : InstHighLevel(Func, Inst::Assign, 1, Dest) {
  addSource(Source);
}

bool InstAssign::isVarAssign() const { return llvm::isa<Variable>(getSrc(0)); }

// If TargetTrue==TargetFalse, we turn it into an unconditional branch. This
// ensures that, along with the 'switch' instruction semantics, there is at
// most one edge from one node to another.
InstBr::InstBr(Cfg *Func, Operand *Source, CfgNode *TargetTrue_,
               CfgNode *TargetFalse_)
    : InstHighLevel(Func, Inst::Br, 1, nullptr), TargetFalse(TargetFalse_),
      TargetTrue(TargetTrue_) {
  if (TargetTrue == TargetFalse) {
    TargetTrue = nullptr; // turn into unconditional version
  } else {
    addSource(Source);
  }
}

InstBr::InstBr(Cfg *Func, CfgNode *Target)
    : InstHighLevel(Func, Inst::Br, 0, nullptr), TargetFalse(Target),
      TargetTrue(nullptr) {}

NodeList InstBr::getTerminatorEdges() const {
  NodeList OutEdges;
  OutEdges.reserve(TargetTrue ? 2 : 1);
  OutEdges.push_back(TargetFalse);
  if (TargetTrue)
    OutEdges.push_back(TargetTrue);
  return OutEdges;
}

bool InstBr::repointEdges(CfgNode *OldNode, CfgNode *NewNode) {
  bool Found = false;
  if (TargetFalse == OldNode) {
    TargetFalse = NewNode;
    Found = true;
  }
  if (TargetTrue == OldNode) {
    TargetTrue = NewNode;
    Found = true;
  }
  return Found;
}

InstCast::InstCast(Cfg *Func, OpKind CastKind, Variable *Dest, Operand *Source)
    : InstHighLevel(Func, Inst::Cast, 1, Dest), CastKind(CastKind) {
  addSource(Source);
}

InstExtractElement::InstExtractElement(Cfg *Func, Variable *Dest,
                                       Operand *Source1, Operand *Source2)
    : InstHighLevel(Func, Inst::ExtractElement, 2, Dest) {
  addSource(Source1);
  addSource(Source2);
}

InstFcmp::InstFcmp(Cfg *Func, FCond Condition, Variable *Dest, Operand *Source1,
                   Operand *Source2)
    : InstHighLevel(Func, Inst::Fcmp, 2, Dest), Condition(Condition) {
  addSource(Source1);
  addSource(Source2);
}

InstIcmp::InstIcmp(Cfg *Func, ICond Condition, Variable *Dest, Operand *Source1,
                   Operand *Source2)
    : InstHighLevel(Func, Inst::Icmp, 2, Dest), Condition(Condition) {
  addSource(Source1);
  addSource(Source2);
}

InstInsertElement::InstInsertElement(Cfg *Func, Variable *Dest,
                                     Operand *Source1, Operand *Source2,
                                     Operand *Source3)
    : InstHighLevel(Func, Inst::InsertElement, 3, Dest) {
  addSource(Source1);
  addSource(Source2);
  addSource(Source3);
}

InstLoad::InstLoad(Cfg *Func, Variable *Dest, Operand *SourceAddr)
    : InstHighLevel(Func, Inst::Load, 1, Dest) {
  addSource(SourceAddr);
}

InstPhi::InstPhi(Cfg *Func, SizeT MaxSrcs, Variable *Dest)
    : InstHighLevel(Func, Phi, MaxSrcs, Dest) {
  Labels = Func->allocateArrayOf<CfgNode *>(MaxSrcs);
}

// TODO: A Switch instruction (and maybe others) can add duplicate edges. We
// may want to de-dup Phis and validate consistency (i.e., the source operands
// are the same for duplicate edges), though it seems the current lowering code
// is OK with this situation.
void InstPhi::addArgument(Operand *Source, CfgNode *Label) {
  Labels[getSrcSize()] = Label;
  addSource(Source);
}

// Find the source operand corresponding to the incoming edge for the given
// node. TODO: This uses a linear-time search, which could be improved if it
// becomes a problem.
Operand *InstPhi::getOperandForTarget(CfgNode *Target) const {
  for (SizeT I = 0; I < getSrcSize(); ++I) {
    if (Labels[I] == Target)
      return getSrc(I);
  }
  llvm_unreachable("Phi target not found");
  return nullptr;
}

// Updates liveness for a particular operand based on the given predecessor
// edge. Doesn't mark the operand as live if the Phi instruction is dead or
// deleted.
void InstPhi::livenessPhiOperand(LivenessBV &Live, CfgNode *Target,
                                 Liveness *Liveness) {
  if (isDeleted() || Dead)
    return;
  for (SizeT I = 0; I < getSrcSize(); ++I) {
    if (Labels[I] == Target) {
      if (auto *Var = llvm::dyn_cast<Variable>(getSrc(I))) {
        SizeT SrcIndex = Liveness->getLiveIndex(Var->getIndex());
        if (!Live[SrcIndex]) {
          setLastUse(I);
          Live[SrcIndex] = true;
        }
      }
      return;
    }
  }
  llvm_unreachable("Phi operand not found for specified target node");
}

// Change "a=phi(...)" to "a_phi=phi(...)" and return a new instruction
// "a=a_phi".
Inst *InstPhi::lower(Cfg *Func) {
  Variable *Dest = getDest();
  assert(Dest);
  Variable *NewSrc = Func->makeVariable(Dest->getType());
  if (BuildDefs::dump())
    NewSrc->setName(Func, Dest->getName(Func) + "_phi");
  if (auto *NewSrc64On32 = llvm::dyn_cast<Variable64On32>(NewSrc))
    NewSrc64On32->initHiLo(Func);
  this->Dest = NewSrc;
  return InstAssign::create(Func, Dest, NewSrc);
}

InstRet::InstRet(Cfg *Func, Operand *RetValue)
    : InstHighLevel(Func, Ret, RetValue ? 1 : 0, nullptr) {
  if (RetValue)
    addSource(RetValue);
}

InstSelect::InstSelect(Cfg *Func, Variable *Dest, Operand *Condition,
                       Operand *SourceTrue, Operand *SourceFalse)
    : InstHighLevel(Func, Inst::Select, 3, Dest) {
  assert(typeElementType(Condition->getType()) == IceType_i1);
  addSource(Condition);
  addSource(SourceTrue);
  addSource(SourceFalse);
}

InstStore::InstStore(Cfg *Func, Operand *Data, Operand *Addr)
    : InstHighLevel(Func, Inst::Store, 3, nullptr) {
  addSource(Data);
  addSource(Addr);
  // The 3rd operand is a dummy placeholder for the RMW beacon.
  addSource(Data);
}

Variable *InstStore::getRmwBeacon() const {
  return llvm::dyn_cast<Variable>(getSrc(2));
}

void InstStore::setRmwBeacon(Variable *Beacon) {
  Dest = llvm::dyn_cast<Variable>(getData());
  Srcs[2] = Beacon;
}

InstSwitch::InstSwitch(Cfg *Func, SizeT NumCases, Operand *Source,
                       CfgNode *LabelDefault)
    : InstHighLevel(Func, Inst::Switch, 1, nullptr), LabelDefault(LabelDefault),
      NumCases(NumCases) {
  addSource(Source);
  Values = Func->allocateArrayOf<uint64_t>(NumCases);
  Labels = Func->allocateArrayOf<CfgNode *>(NumCases);
  // Initialize in case buggy code doesn't set all entries
  for (SizeT I = 0; I < NumCases; ++I) {
    Values[I] = 0;
    Labels[I] = nullptr;
  }
}

void InstSwitch::addBranch(SizeT CaseIndex, uint64_t Value, CfgNode *Label) {
  assert(CaseIndex < NumCases);
  Values[CaseIndex] = Value;
  Labels[CaseIndex] = Label;
}

NodeList InstSwitch::getTerminatorEdges() const {
  NodeList OutEdges;
  OutEdges.reserve(NumCases + 1);
  OutEdges.push_back(LabelDefault);
  for (SizeT I = 0; I < NumCases; ++I) {
    OutEdges.push_back(Labels[I]);
  }
  std::sort(OutEdges.begin(), OutEdges.end(),
            [](const CfgNode *x, const CfgNode *y) {
              return x->getIndex() < y->getIndex();
            });
  auto Last = std::unique(OutEdges.begin(), OutEdges.end());
  OutEdges.erase(Last, OutEdges.end());
  return OutEdges;
}

bool InstSwitch::repointEdges(CfgNode *OldNode, CfgNode *NewNode) {
  bool Found = false;
  if (LabelDefault == OldNode) {
    LabelDefault = NewNode;
    Found = true;
  }
  for (SizeT I = 0; I < NumCases; ++I) {
    if (Labels[I] == OldNode) {
      Labels[I] = NewNode;
      Found = true;
    }
  }
  return Found;
}

InstUnreachable::InstUnreachable(Cfg *Func)
    : InstHighLevel(Func, Inst::Unreachable, 0, nullptr) {}

InstBundleLock::InstBundleLock(Cfg *Func, InstBundleLock::Option BundleOption)
    : InstHighLevel(Func, Inst::BundleLock, 0, nullptr),
      BundleOption(BundleOption) {}

InstBundleUnlock::InstBundleUnlock(Cfg *Func)
    : InstHighLevel(Func, Inst::BundleUnlock, 0, nullptr) {}

InstFakeDef::InstFakeDef(Cfg *Func, Variable *Dest, Variable *Src)
    : InstHighLevel(Func, Inst::FakeDef, Src ? 1 : 0, Dest) {
  assert(Dest);
  if (Src)
    addSource(Src);
}

InstFakeUse::InstFakeUse(Cfg *Func, Variable *Src, uint32_t Weight)
    : InstHighLevel(Func, Inst::FakeUse, Weight, nullptr) {
  assert(Src);
  for (uint32_t i = 0; i < Weight; ++i)
    addSource(Src);
}

InstFakeKill::InstFakeKill(Cfg *Func, const Inst *Linked)
    : InstHighLevel(Func, Inst::FakeKill, 0, nullptr), Linked(Linked) {}

InstJumpTable::InstJumpTable(Cfg *Func, SizeT NumTargets, CfgNode *Default)
    : InstHighLevel(Func, Inst::JumpTable, 1, nullptr),
      Id(Func->getTarget()->makeNextJumpTableNumber()), NumTargets(NumTargets) {
  Targets = Func->allocateArrayOf<CfgNode *>(NumTargets);
  for (SizeT I = 0; I < NumTargets; ++I)
    Targets[I] = Default;
}

bool InstJumpTable::repointEdges(CfgNode *OldNode, CfgNode *NewNode) {
  bool Found = false;
  for (SizeT I = 0; I < NumTargets; ++I) {
    if (Targets[I] == OldNode) {
      Targets[I] = NewNode;
      Found = true;
    }
  }
  return Found;
}

Type InstCall::getReturnType() const {
  if (Dest == nullptr)
    return IceType_void;
  return Dest->getType();
}

// ======================== Dump routines ======================== //

void Inst::dumpDecorated(const Cfg *Func) const {
  if (!BuildDefs::dump())
    return;
  Ostream &Str = Func->getContext()->getStrDump();
  if (!Func->isVerbose(IceV_Deleted) && (isDeleted() || isRedundantAssign()))
    return;
  if (Func->isVerbose(IceV_InstNumbers)) {
    char buf[30];
    InstNumberT Number = getNumber();
    if (Number == NumberDeleted)
      snprintf(buf, llvm::array_lengthof(buf), "[XXX]");
    else
      snprintf(buf, llvm::array_lengthof(buf), "[%3d]", Number);
    Str << buf;
  }
  Str << "  ";
  if (isDeleted())
    Str << "  //";
  dump(Func);
  dumpExtras(Func);
  Str << "\n";
}

void Inst::dump(const Cfg *Func) const {
  if (!BuildDefs::dump())
    return;
  Ostream &Str = Func->getContext()->getStrDump();
  dumpDest(Func);
  Str << " =~ " << getInstName() << " ";
  dumpSources(Func);
}

void Inst::dumpExtras(const Cfg *Func) const {
  if (!BuildDefs::dump())
    return;
  Ostream &Str = Func->getContext()->getStrDump();
  bool First = true;
  // Print "LIVEEND={a,b,c}" for all source operands whose live ranges are
  // known to end at this instruction.
  if (Func->isVerbose(IceV_Liveness)) {
    FOREACH_VAR_IN_INST(Var, *this) {
      if (isLastUse(Var)) {
        if (First)
          Str << " // LIVEEND={";
        else
          Str << ",";
        Var->dump(Func);
        First = false;
      }
    }
    if (!First)
      Str << "}";
  }
}

void Inst::dumpSources(const Cfg *Func) const {
  if (!BuildDefs::dump())
    return;
  Ostream &Str = Func->getContext()->getStrDump();
  for (SizeT I = 0; I < getSrcSize(); ++I) {
    if (I > 0)
      Str << ", ";
    getSrc(I)->dump(Func);
  }
}

void Inst::emitSources(const Cfg *Func) const {
  if (!BuildDefs::dump())
    return;
  Ostream &Str = Func->getContext()->getStrEmit();
  for (SizeT I = 0; I < getSrcSize(); ++I) {
    if (I > 0)
      Str << ", ";
    getSrc(I)->emit(Func);
  }
}

void Inst::dumpDest(const Cfg *Func) const {
  if (!BuildDefs::dump())
    return;
  if (getDest())
    getDest()->dump(Func);
}

void InstAlloca::dump(const Cfg *Func) const {
  if (!BuildDefs::dump())
    return;
  Ostream &Str = Func->getContext()->getStrDump();
  dumpDest(Func);
  Str << " = alloca i8, i32 ";
  getSizeInBytes()->dump(Func);
  if (getAlignInBytes())
    Str << ", align " << getAlignInBytes();
}

void InstArithmetic::dump(const Cfg *Func) const {
  if (!BuildDefs::dump())
    return;
  Ostream &Str = Func->getContext()->getStrDump();
  dumpDest(Func);
  Str << " = " << getInstName() << " " << getDest()->getType() << " ";
  dumpSources(Func);
}

void InstAssign::dump(const Cfg *Func) const {
  if (!BuildDefs::dump())
    return;
  Ostream &Str = Func->getContext()->getStrDump();
  dumpDest(Func);
  Str << " = " << getDest()->getType() << " ";
  dumpSources(Func);
}

void InstBr::dump(const Cfg *Func) const {
  if (!BuildDefs::dump())
    return;
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
  if (!BuildDefs::dump())
    return;
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

const char *InstCast::getCastName(InstCast::OpKind Kind) {
  if (Kind < InstCast::OpKind::_num)
    return InstCastAttributes[Kind].DisplayString;
  llvm_unreachable("Invalid InstCast::OpKind");
  return "???";
}

void InstCast::dump(const Cfg *Func) const {
  if (!BuildDefs::dump())
    return;
  Ostream &Str = Func->getContext()->getStrDump();
  dumpDest(Func);
  Str << " = " << getCastName(getCastKind()) << " " << getSrc(0)->getType()
      << " ";
  dumpSources(Func);
  Str << " to " << getDest()->getType();
}

void InstIcmp::dump(const Cfg *Func) const {
  if (!BuildDefs::dump())
    return;
  Ostream &Str = Func->getContext()->getStrDump();
  dumpDest(Func);
  Str << " = icmp " << InstIcmpAttributes[getCondition()].DisplayString << " "
      << getSrc(0)->getType() << " ";
  dumpSources(Func);
}

void InstExtractElement::dump(const Cfg *Func) const {
  if (!BuildDefs::dump())
    return;
  Ostream &Str = Func->getContext()->getStrDump();
  dumpDest(Func);
  Str << " = extractelement ";
  Str << getSrc(0)->getType() << " ";
  getSrc(0)->dump(Func);
  Str << ", ";
  Str << getSrc(1)->getType() << " ";
  getSrc(1)->dump(Func);
}

void InstInsertElement::dump(const Cfg *Func) const {
  if (!BuildDefs::dump())
    return;
  Ostream &Str = Func->getContext()->getStrDump();
  dumpDest(Func);
  Str << " = insertelement ";
  Str << getSrc(0)->getType() << " ";
  getSrc(0)->dump(Func);
  Str << ", ";
  Str << getSrc(1)->getType() << " ";
  getSrc(1)->dump(Func);
  Str << ", ";
  Str << getSrc(2)->getType() << " ";
  getSrc(2)->dump(Func);
}

void InstFcmp::dump(const Cfg *Func) const {
  if (!BuildDefs::dump())
    return;
  Ostream &Str = Func->getContext()->getStrDump();
  dumpDest(Func);
  Str << " = fcmp " << InstFcmpAttributes[getCondition()].DisplayString << " "
      << getSrc(0)->getType() << " ";
  dumpSources(Func);
}

void InstLoad::dump(const Cfg *Func) const {
  if (!BuildDefs::dump())
    return;
  Ostream &Str = Func->getContext()->getStrDump();
  dumpDest(Func);
  Type Ty = getDest()->getType();
  Str << " = load " << Ty << ", " << Ty << "* ";
  dumpSources(Func);
  Str << ", align " << typeAlignInBytes(Ty);
}

void InstStore::dump(const Cfg *Func) const {
  if (!BuildDefs::dump())
    return;
  Ostream &Str = Func->getContext()->getStrDump();
  Type Ty = getData()->getType();
  dumpDest(Func);
  if (Dest)
    Str << " = ";
  Str << "store " << Ty << " ";
  getData()->dump(Func);
  Str << ", " << Ty << "* ";
  getAddr()->dump(Func);
  Str << ", align " << typeAlignInBytes(Ty);
  if (getRmwBeacon()) {
    Str << ", beacon ";
    getRmwBeacon()->dump(Func);
  }
}

void InstSwitch::dump(const Cfg *Func) const {
  if (!BuildDefs::dump())
    return;
  Ostream &Str = Func->getContext()->getStrDump();
  Type Ty = getComparison()->getType();
  Str << "switch " << Ty << " ";
  getSrc(0)->dump(Func);
  Str << ", label %" << getLabelDefault()->getName() << " [\n";
  for (SizeT I = 0; I < getNumCases(); ++I) {
    Str << "    " << Ty << " " << static_cast<int64_t>(getValue(I))
        << ", label %" << getLabel(I)->getName() << "\n";
  }
  Str << "  ]";
}

void InstPhi::dump(const Cfg *Func) const {
  if (!BuildDefs::dump())
    return;
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
  if (!BuildDefs::dump())
    return;
  Ostream &Str = Func->getContext()->getStrDump();
  Type Ty = hasRetValue() ? getRetValue()->getType() : IceType_void;
  Str << "ret " << Ty;
  if (hasRetValue()) {
    Str << " ";
    dumpSources(Func);
  }
}

void InstSelect::dump(const Cfg *Func) const {
  if (!BuildDefs::dump())
    return;
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
  if (!BuildDefs::dump())
    return;
  Ostream &Str = Func->getContext()->getStrDump();
  Str << "unreachable";
}

void InstBundleLock::emit(const Cfg *Func) const {
  if (!BuildDefs::dump())
    return;
  Ostream &Str = Func->getContext()->getStrEmit();
  Str << "\t.bundle_lock";
  switch (BundleOption) {
  case Opt_None:
    break;
  case Opt_AlignToEnd:
    Str << "\t"
           "align_to_end";
    break;
  case Opt_PadToEnd:
    Str << "\t"
           "align_to_end /* pad_to_end */";
    break;
  }
  Str << "\n";
}

void InstBundleLock::dump(const Cfg *Func) const {
  if (!BuildDefs::dump())
    return;
  Ostream &Str = Func->getContext()->getStrDump();
  Str << "bundle_lock";
  switch (BundleOption) {
  case Opt_None:
    break;
  case Opt_AlignToEnd:
    Str << " align_to_end";
    break;
  case Opt_PadToEnd:
    Str << " pad_to_end";
    break;
  }
}

void InstBundleUnlock::emit(const Cfg *Func) const {
  if (!BuildDefs::dump())
    return;
  Ostream &Str = Func->getContext()->getStrEmit();
  Str << "\t.bundle_unlock";
  Str << "\n";
}

void InstBundleUnlock::dump(const Cfg *Func) const {
  if (!BuildDefs::dump())
    return;
  Ostream &Str = Func->getContext()->getStrDump();
  Str << "bundle_unlock";
}

void InstFakeDef::emit(const Cfg *Func) const {
  if (!BuildDefs::dump())
    return;
  // Go ahead and "emit" these for now, since they are relatively rare.
  Ostream &Str = Func->getContext()->getStrEmit();
  Str << "\t# ";
  getDest()->emit(Func);
  Str << " = def.pseudo";
  if (getSrcSize() > 0)
    Str << " ";
  emitSources(Func);
  Str << "\n";
}

void InstFakeDef::dump(const Cfg *Func) const {
  if (!BuildDefs::dump())
    return;
  Ostream &Str = Func->getContext()->getStrDump();
  dumpDest(Func);
  Str << " = def.pseudo ";
  dumpSources(Func);
}

void InstFakeUse::emit(const Cfg *Func) const { (void)Func; }

void InstFakeUse::dump(const Cfg *Func) const {
  if (!BuildDefs::dump())
    return;
  Ostream &Str = Func->getContext()->getStrDump();
  Str << "use.pseudo ";
  dumpSources(Func);
}

void InstFakeKill::emit(const Cfg *Func) const { (void)Func; }

void InstFakeKill::dump(const Cfg *Func) const {
  if (!BuildDefs::dump())
    return;
  Ostream &Str = Func->getContext()->getStrDump();
  if (Linked->isDeleted())
    Str << "// ";
  Str << "kill.pseudo scratch_regs";
}

void InstJumpTable::dump(const Cfg *Func) const {
  if (!BuildDefs::dump())
    return;
  Ostream &Str = Func->getContext()->getStrDump();
  Str << "jump table [";
  for (SizeT I = 0; I < NumTargets; ++I)
    Str << "\n    " << Targets[I]->getName();
  Str << "\n  ]";
}

void InstTarget::dump(const Cfg *Func) const {
  if (!BuildDefs::dump())
    return;
  Ostream &Str = Func->getContext()->getStrDump();
  Str << "[TARGET] ";
  Inst::dump(Func);
}

bool checkForRedundantAssign(const Variable *Dest, const Operand *Source) {
  const auto *SrcVar = llvm::dyn_cast<const Variable>(Source);
  if (!SrcVar)
    return false;
  if (Dest->hasReg() && Dest->getRegNum() == SrcVar->getRegNum()) {
    // TODO: On x86-64, instructions like "mov eax, eax" are used to clear the
    // upper 32 bits of rax. We need to recognize and preserve these.
    return true;
  }
  if (!Dest->hasReg() && !SrcVar->hasReg() &&
      Dest->getStackOffset() == SrcVar->getStackOffset())
    return true;
  return false;
}

} // end of namespace Ice
