//===- subzero/src/IceOperand.cpp - High-level operand implementation -----===//
//
//                        The Subzero Code Generator
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements the Operand class and its target-independent
// subclasses, primarily for the methods of the Variable class.
//
//===----------------------------------------------------------------------===//

#include "IceCfg.h"
#include "IceCfgNode.h"
#include "IceInst.h"
#include "IceOperand.h"
#include "IceTargetLowering.h" // dumping stack/frame pointer register

namespace Ice {

bool operator==(const RelocatableTuple &A, const RelocatableTuple &B) {
  return A.Offset == B.Offset && A.Name == B.Name;
}

bool operator<(const RegWeight &A, const RegWeight &B) {
  return A.getWeight() < B.getWeight();
}
bool operator<=(const RegWeight &A, const RegWeight &B) { return !(B < A); }
bool operator==(const RegWeight &A, const RegWeight &B) {
  return !(B < A) && !(A < B);
}

void LiveRange::addSegment(InstNumberT Start, InstNumberT End) {
  if (!Range.empty()) {
    // Check for merge opportunity.
    InstNumberT CurrentEnd = Range.back().second;
    assert(Start >= CurrentEnd);
    if (Start == CurrentEnd) {
      Range.back().second = End;
      return;
    }
  }
  Range.push_back(RangeElementType(Start, End));
}

// Returns true if this live range ends before Other's live range
// starts.  This means that the highest instruction number in this
// live range is less than or equal to the lowest instruction number
// of the Other live range.
bool LiveRange::endsBefore(const LiveRange &Other) const {
  // Neither range should be empty, but let's be graceful.
  if (Range.empty() || Other.Range.empty())
    return true;
  InstNumberT MyEnd = (*Range.rbegin()).second;
  InstNumberT OtherStart = (*Other.Range.begin()).first;
  return MyEnd <= OtherStart;
}

// Returns true if there is any overlap between the two live ranges.
bool LiveRange::overlaps(const LiveRange &Other, bool UseTrimmed) const {
  // Do a two-finger walk through the two sorted lists of segments.
  auto I1 = (UseTrimmed ? TrimmedBegin : Range.begin()),
       I2 = (UseTrimmed ? Other.TrimmedBegin : Other.Range.begin());
  auto E1 = Range.end(), E2 = Other.Range.end();
  while (I1 != E1 && I2 != E2) {
    if (I1->second <= I2->first) {
      ++I1;
      continue;
    }
    if (I2->second <= I1->first) {
      ++I2;
      continue;
    }
    return true;
  }
  return false;
}

bool LiveRange::overlapsInst(InstNumberT OtherBegin, bool UseTrimmed) const {
  bool Result = false;
  for (auto I = (UseTrimmed ? TrimmedBegin : Range.begin()), E = Range.end();
       I != E; ++I) {
    if (OtherBegin < I->first) {
      Result = false;
      break;
    }
    if (OtherBegin < I->second) {
      Result = true;
      break;
    }
  }
#if 0
  // An equivalent but less inefficient implementation:
  LiveRange Temp;
  Temp.addSegment(OtherBegin, OtherBegin + 1);
  bool Validation = overlaps(Temp);
  assert(Result == Validation);
#endif
  return Result;
}

// Returns true if the live range contains the given instruction
// number.  This is only used for validating the live range
// calculation.  The IsDest argument indicates whether the Variable
// being tested is used in the Dest position (as opposed to a Src
// position).
bool LiveRange::containsValue(InstNumberT Value, bool IsDest) const {
  for (const RangeElementType &I : Range) {
    if (I.first <= Value &&
        (Value < I.second || (!IsDest && Value == I.second)))
      return true;
  }
  return false;
}

void LiveRange::trim(InstNumberT Lower) {
  while (TrimmedBegin != Range.end() && TrimmedBegin->second <= Lower)
    ++TrimmedBegin;
}

IceString Variable::getName(const Cfg *Func) const {
  if (Func && NameIndex >= 0)
    return Func->getIdentifierName(NameIndex);
  return "__" + std::to_string(getIndex());
}

Variable *Variable::asType(Type Ty) {
  // Note: This returns a Variable, even if the "this" object is a
  // subclass of Variable.
  if (!ALLOW_DUMP || getType() == Ty)
    return this;
  Variable *V = new (getCurrentCfgAllocator()->Allocate<Variable>())
      Variable(kVariable, Ty, Number);
  V->NameIndex = NameIndex;
  V->RegNum = RegNum;
  V->StackOffset = StackOffset;
  return V;
}

void VariableTracking::markUse(MetadataKind TrackingKind, const Inst *Instr,
                               const CfgNode *Node, bool IsFromDef,
                               bool IsImplicit) {
  (void)TrackingKind;
  if (MultiBlock == MBS_MultiBlock)
    return;
  // TODO(stichnot): If the use occurs as a source operand in the
  // first instruction of the block, and its definition is in this
  // block's only predecessor, we might consider not marking this as a
  // separate use.  This may also apply if it's the first instruction
  // of the block that actually uses a Variable.
  assert(Node);
  bool MakeMulti = false;
  if (IsImplicit)
    MakeMulti = true;
  // A phi source variable conservatively needs to be marked as
  // multi-block, even if its definition is in the same block.  This
  // is because there can be additional control flow before branching
  // back to this node, and the variable is live throughout those
  // nodes.
  if (!IsFromDef && Instr && llvm::isa<InstPhi>(Instr))
    MakeMulti = true;

  if (!MakeMulti) {
    switch (MultiBlock) {
    case MBS_Unknown:
      MultiBlock = MBS_SingleBlock;
      SingleUseNode = Node;
      break;
    case MBS_SingleBlock:
      if (SingleUseNode != Node)
        MakeMulti = true;
      break;
    case MBS_MultiBlock:
      break;
    }
  }

  if (MakeMulti) {
    MultiBlock = MBS_MultiBlock;
    SingleUseNode = nullptr;
  }
}

void VariableTracking::markDef(MetadataKind TrackingKind, const Inst *Instr,
                               const CfgNode *Node) {
  // TODO(stichnot): If the definition occurs in the last instruction
  // of the block, consider not marking this as a separate use.  But
  // be careful not to omit all uses of the variable if markDef() and
  // markUse() both use this optimization.
  assert(Node);
// Verify that instructions are added in increasing order.
#ifndef NDEBUG
  if (TrackingKind == VMK_All) {
    const Inst *LastInstruction =
        Definitions.empty() ? FirstOrSingleDefinition : Definitions.back();
    assert(LastInstruction == nullptr ||
           Instr->getNumber() >= LastInstruction->getNumber());
  }
#endif
  const bool IsFromDef = true;
  const bool IsImplicit = false;
  markUse(TrackingKind, Instr, Node, IsFromDef, IsImplicit);
  if (TrackingKind == VMK_Uses)
    return;
  if (FirstOrSingleDefinition == nullptr)
    FirstOrSingleDefinition = Instr;
  else if (TrackingKind == VMK_All)
    Definitions.push_back(Instr);
  switch (MultiDef) {
  case MDS_Unknown:
    assert(SingleDefNode == nullptr);
    MultiDef = MDS_SingleDef;
    SingleDefNode = Node;
    break;
  case MDS_SingleDef:
    assert(SingleDefNode);
    if (Node == SingleDefNode) {
      MultiDef = MDS_MultiDefSingleBlock;
    } else {
      MultiDef = MDS_MultiDefMultiBlock;
      SingleDefNode = nullptr;
    }
    break;
  case MDS_MultiDefSingleBlock:
    assert(SingleDefNode);
    if (Node != SingleDefNode) {
      MultiDef = MDS_MultiDefMultiBlock;
      SingleDefNode = nullptr;
    }
    break;
  case MDS_MultiDefMultiBlock:
    assert(SingleDefNode == nullptr);
    break;
  }
}

const Inst *VariableTracking::getFirstDefinition() const {
  switch (MultiDef) {
  case MDS_Unknown:
  case MDS_MultiDefMultiBlock:
    return nullptr;
  case MDS_SingleDef:
  case MDS_MultiDefSingleBlock:
    assert(FirstOrSingleDefinition);
    return FirstOrSingleDefinition;
  }
  return nullptr;
}

const Inst *VariableTracking::getSingleDefinition() const {
  switch (MultiDef) {
  case MDS_Unknown:
  case MDS_MultiDefMultiBlock:
  case MDS_MultiDefSingleBlock:
    return nullptr;
  case MDS_SingleDef:
    assert(FirstOrSingleDefinition);
    return FirstOrSingleDefinition;
  }
  return nullptr;
}

void VariablesMetadata::init(MetadataKind TrackingKind) {
  TimerMarker T(TimerStack::TT_vmetadata, Func);
  Kind = TrackingKind;
  Metadata.clear();
  Metadata.resize(Func->getNumVariables());

  // Mark implicit args as being used in the entry node.
  for (Variable *Var : Func->getImplicitArgs()) {
    const Inst *NoInst = nullptr;
    const CfgNode *EntryNode = Func->getEntryNode();
    const bool IsFromDef = false;
    const bool IsImplicit = true;
    Metadata[Var->getIndex()].markUse(Kind, NoInst, EntryNode, IsFromDef,
                                      IsImplicit);
  }

  for (CfgNode *Node : Func->getNodes())
    addNode(Node);
}

void VariablesMetadata::addNode(CfgNode *Node) {
  if (Func->getNumVariables() >= Metadata.size())
    Metadata.resize(Func->getNumVariables());

  for (Inst &I : Node->getPhis()) {
    if (I.isDeleted())
      continue;
    if (Variable *Dest = I.getDest()) {
      SizeT DestNum = Dest->getIndex();
      assert(DestNum < Metadata.size());
      Metadata[DestNum].markDef(Kind, &I, Node);
    }
    for (SizeT SrcNum = 0; SrcNum < I.getSrcSize(); ++SrcNum) {
      if (const Variable *Var = llvm::dyn_cast<Variable>(I.getSrc(SrcNum))) {
        SizeT VarNum = Var->getIndex();
        assert(VarNum < Metadata.size());
        const bool IsFromDef = false;
        const bool IsImplicit = false;
        Metadata[VarNum].markUse(Kind, &I, Node, IsFromDef, IsImplicit);
      }
    }
  }

  for (Inst &I : Node->getInsts()) {
    if (I.isDeleted())
      continue;
    // Note: The implicit definitions (and uses) from InstFakeKill are
    // deliberately ignored.
    if (Variable *Dest = I.getDest()) {
      SizeT DestNum = Dest->getIndex();
      assert(DestNum < Metadata.size());
      Metadata[DestNum].markDef(Kind, &I, Node);
    }
    for (SizeT SrcNum = 0; SrcNum < I.getSrcSize(); ++SrcNum) {
      Operand *Src = I.getSrc(SrcNum);
      SizeT NumVars = Src->getNumVars();
      for (SizeT J = 0; J < NumVars; ++J) {
        const Variable *Var = Src->getVar(J);
        SizeT VarNum = Var->getIndex();
        assert(VarNum < Metadata.size());
        const bool IsFromDef = false;
        const bool IsImplicit = false;
        Metadata[VarNum].markUse(Kind, &I, Node, IsFromDef, IsImplicit);
      }
    }
  }
}

bool VariablesMetadata::isMultiDef(const Variable *Var) const {
  assert(Kind != VMK_Uses);
  if (Var->getIsArg())
    return false;
  if (!isTracked(Var))
    return true; // conservative answer
  SizeT VarNum = Var->getIndex();
  // Conservatively return true if the state is unknown.
  return Metadata[VarNum].getMultiDef() != VariableTracking::MDS_SingleDef;
}

bool VariablesMetadata::isMultiBlock(const Variable *Var) const {
  if (Var->getIsArg())
    return true;
  if (!isTracked(Var))
    return true; // conservative answer
  SizeT VarNum = Var->getIndex();
  // Conservatively return true if the state is unknown.
  return Metadata[VarNum].getMultiBlock() != VariableTracking::MBS_SingleBlock;
}

const Inst *VariablesMetadata::getFirstDefinition(const Variable *Var) const {
  assert(Kind != VMK_Uses);
  if (!isTracked(Var))
    return nullptr; // conservative answer
  SizeT VarNum = Var->getIndex();
  return Metadata[VarNum].getFirstDefinition();
}

const Inst *VariablesMetadata::getSingleDefinition(const Variable *Var) const {
  assert(Kind != VMK_Uses);
  if (!isTracked(Var))
    return nullptr; // conservative answer
  SizeT VarNum = Var->getIndex();
  return Metadata[VarNum].getSingleDefinition();
}

const InstDefList &
VariablesMetadata::getLatterDefinitions(const Variable *Var) const {
  assert(Kind == VMK_All);
  if (!isTracked(Var))
    return NoDefinitions;
  SizeT VarNum = Var->getIndex();
  return Metadata[VarNum].getLatterDefinitions();
}

const CfgNode *VariablesMetadata::getLocalUseNode(const Variable *Var) const {
  if (!isTracked(Var))
    return nullptr; // conservative answer
  SizeT VarNum = Var->getIndex();
  return Metadata[VarNum].getNode();
}

const InstDefList VariablesMetadata::NoDefinitions;

// ======================== dump routines ======================== //

void Variable::emit(const Cfg *Func) const {
  if (ALLOW_DUMP)
    Func->getTarget()->emitVariable(this);
}

void Variable::dump(const Cfg *Func, Ostream &Str) const {
  if (!ALLOW_DUMP)
    return;
  if (Func == nullptr) {
    Str << "%" << getName(Func);
    return;
  }
  if (Func->isVerbose(IceV_RegOrigins) ||
      (!hasReg() && !Func->getTarget()->hasComputedFrame()))
    Str << "%" << getName(Func);
  if (hasReg()) {
    if (Func->isVerbose(IceV_RegOrigins))
      Str << ":";
    Str << Func->getTarget()->getRegName(RegNum, getType());
  } else if (Func->getTarget()->hasComputedFrame()) {
    if (Func->isVerbose(IceV_RegOrigins))
      Str << ":";
    Str << "["
        << Func->getTarget()->getRegName(
               Func->getTarget()->getFrameOrStackReg(), IceType_i32);
    int32_t Offset = getStackOffset();
    if (Offset) {
      if (Offset > 0)
        Str << "+";
      Str << Offset;
    }
    Str << "]";
  }
}

template <> void ConstantInteger32::emit(TargetLowering *Target) const {
  Target->emit(this);
}

template <> void ConstantInteger64::emit(TargetLowering *Target) const {
  Target->emit(this);
}

template <> void ConstantFloat::emit(TargetLowering *Target) const {
  Target->emit(this);
}

template <> void ConstantDouble::emit(TargetLowering *Target) const {
  Target->emit(this);
}

void ConstantRelocatable::emit(TargetLowering *Target) const {
  Target->emit(this);
}

void ConstantRelocatable::emitWithoutPrefix(TargetLowering *Target) const {
  Target->emitWithoutPrefix(this);
}

void ConstantRelocatable::dump(const Cfg *Func, Ostream &Str) const {
  if (!ALLOW_DUMP)
    return;
  Str << "@";
  if (Func && !SuppressMangling) {
    Str << Func->getContext()->mangleName(Name);
  } else {
    Str << Name;
  }
  if (Offset)
    Str << "+" << Offset;
}

void ConstantUndef::emit(TargetLowering *Target) const { Target->emit(this); }

void LiveRange::dump(Ostream &Str) const {
  if (!ALLOW_DUMP)
    return;
  Str << "(weight=" << Weight << ") ";
  bool First = true;
  for (const RangeElementType &I : Range) {
    if (!First)
      Str << ", ";
    First = false;
    Str << "[" << I.first << ":" << I.second << ")";
  }
}

Ostream &operator<<(Ostream &Str, const LiveRange &L) {
  if (!ALLOW_DUMP)
    return Str;
  L.dump(Str);
  return Str;
}

Ostream &operator<<(Ostream &Str, const RegWeight &W) {
  if (!ALLOW_DUMP)
    return Str;
  if (W.getWeight() == RegWeight::Inf)
    Str << "Inf";
  else
    Str << W.getWeight();
  return Str;
}

} // end of namespace Ice
