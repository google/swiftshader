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

bool operator<(const RelocatableTuple &A, const RelocatableTuple &B) {
  if (A.Offset != B.Offset)
    return A.Offset < B.Offset;
  if (A.SuppressMangling != B.SuppressMangling)
    return A.SuppressMangling < B.SuppressMangling;
  return A.Name < B.Name;
}

bool operator<(const RegWeight &A, const RegWeight &B) {
  return A.getWeight() < B.getWeight();
}
bool operator<=(const RegWeight &A, const RegWeight &B) { return !(B < A); }
bool operator==(const RegWeight &A, const RegWeight &B) {
  return !(B < A) && !(A < B);
}

void LiveRange::addSegment(InstNumberT Start, InstNumberT End) {
#ifdef USE_SET
  RangeElementType Element(Start, End);
  RangeType::iterator Next = Range.lower_bound(Element);
  assert(Next == Range.upper_bound(Element)); // Element not already present

  // Beginning of code that merges contiguous segments.  TODO: change
  // "if(true)" to "if(false)" to see if this extra optimization code
  // gives any performance gain, or is just destabilizing.
  if (true) {
    RangeType::iterator FirstDelete = Next;
    RangeType::iterator Prev = Next;
    bool hasPrev = (Next != Range.begin());
    bool hasNext = (Next != Range.end());
    if (hasPrev)
      --Prev;
    // See if Element and Next should be joined.
    if (hasNext && End == Next->first) {
      Element.second = Next->second;
      ++Next;
    }
    // See if Prev and Element should be joined.
    if (hasPrev && Prev->second == Start) {
      Element.first = Prev->first;
      FirstDelete = Prev;
    }
    Range.erase(FirstDelete, Next);
  }
  // End of code that merges contiguous segments.

  Range.insert(Next, Element);
#else
  if (Range.empty()) {
    Range.push_back(RangeElementType(Start, End));
    return;
  }
  // Special case for faking in-arg liveness.
  if (End < Range.front().first) {
    assert(Start < 0);
    Range.push_front(RangeElementType(Start, End));
    return;
  }
  InstNumberT CurrentEnd = Range.back().second;
  assert(Start >= CurrentEnd);
  // Check for merge opportunity.
  if (Start == CurrentEnd) {
    Range.back().second = End;
    return;
  }
  Range.push_back(RangeElementType(Start, End));
#endif
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
bool LiveRange::overlaps(const LiveRange &Other) const {
  // Do a two-finger walk through the two sorted lists of segments.
  RangeType::const_iterator I1 = Range.begin(), I2 = Other.Range.begin();
  RangeType::const_iterator E1 = Range.end(), E2 = Other.Range.end();
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

bool LiveRange::overlaps(InstNumberT OtherBegin) const {
  bool Result = false;
  for (const RangeElementType &I : Range) {
    if (OtherBegin < I.first) {
      Result = false;
      break;
    }
    if (OtherBegin < I.second) {
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
// calculation.
bool LiveRange::containsValue(InstNumberT Value) const {
  for (const RangeElementType &I : Range) {
    if (I.first <= Value && Value <= I.second)
      return true;
  }
  return false;
}

IceString Variable::getName() const {
  if (!Name.empty())
    return Name;
  char buf[30];
  snprintf(buf, llvm::array_lengthof(buf), "__%u", getIndex());
  return buf;
}

Variable Variable::asType(Type Ty) {
  // Note: This returns a Variable, even if the "this" object is a
  // subclass of Variable.
  Variable V(kVariable, Ty, Number, Name);
  V.RegNum = RegNum;
  V.StackOffset = StackOffset;
  return V;
}

void VariableTracking::markUse(const Inst *Instr, const CfgNode *Node,
                               bool IsFromDef, bool IsImplicit) {
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
    SingleUseNode = NULL;
  }
}

void VariableTracking::markDef(const Inst *Instr, const CfgNode *Node) {
  // TODO(stichnot): If the definition occurs in the last instruction
  // of the block, consider not marking this as a separate use.  But
  // be careful not to omit all uses of the variable if markDef() and
  // markUse() both use this optimization.
  assert(Node);
  Definitions.push_back(Instr);
  const bool IsFromDef = true;
  const bool IsImplicit = false;
  markUse(Instr, Node, IsFromDef, IsImplicit);
  switch (MultiDef) {
  case MDS_Unknown:
    assert(SingleDefNode == NULL);
    MultiDef = MDS_SingleDef;
    SingleDefNode = Node;
    break;
  case MDS_SingleDef:
    assert(SingleDefNode);
    if (Node == SingleDefNode) {
      MultiDef = MDS_MultiDefSingleBlock;
    } else {
      MultiDef = MDS_MultiDefMultiBlock;
      SingleDefNode = NULL;
    }
    break;
  case MDS_MultiDefSingleBlock:
    assert(SingleDefNode);
    if (Node != SingleDefNode) {
      MultiDef = MDS_MultiDefMultiBlock;
      SingleDefNode = NULL;
    }
    break;
  case MDS_MultiDefMultiBlock:
    assert(SingleDefNode == NULL);
    break;
  }
}

const Inst *VariableTracking::getFirstDefinition() const {
  switch (MultiDef) {
  case MDS_Unknown:
  case MDS_MultiDefMultiBlock:
    return NULL;
  case MDS_SingleDef:
  case MDS_MultiDefSingleBlock:
    assert(!Definitions.empty());
    return Definitions[0];
  }
}

const Inst *VariableTracking::getSingleDefinition() const {
  switch (MultiDef) {
  case MDS_Unknown:
  case MDS_MultiDefMultiBlock:
  case MDS_MultiDefSingleBlock:
    return NULL;
  case MDS_SingleDef:
    assert(!Definitions.empty());
    return Definitions[0];
  }
}

void VariablesMetadata::init() {
  static TimerIdT IDvmetadata = GlobalContext::getTimerID("vmetadata");
  TimerMarker T(IDvmetadata, Func->getContext());
  Metadata.clear();
  Metadata.resize(Func->getNumVariables());

  // Mark implicit args as being used in the entry node.
  for (Variable *Var : Func->getImplicitArgs()) {
    const Inst *NoInst = NULL;
    const CfgNode *EntryNode = Func->getEntryNode();
    const bool IsFromDef = false;
    const bool IsImplicit = true;
    Metadata[Var->getIndex()].markUse(NoInst, EntryNode, IsFromDef, IsImplicit);
  }

  SizeT NumNodes = Func->getNumNodes();
  for (SizeT N = 0; N < NumNodes; ++N) {
    CfgNode *Node = Func->getNodes()[N];
    for (Inst *I : Node->getInsts()) {
      if (I->isDeleted())
        continue;
      if (InstFakeKill *Kill = llvm::dyn_cast<InstFakeKill>(I)) {
        // A FakeKill instruction indicates certain Variables (usually
        // physical scratch registers) are redefined, so we register
        // them as defs.
        for (SizeT SrcNum = 0; SrcNum < I->getSrcSize(); ++SrcNum) {
          Variable *Var = llvm::cast<Variable>(I->getSrc(SrcNum));
          SizeT VarNum = Var->getIndex();
          assert(VarNum < Metadata.size());
          Metadata[VarNum].markDef(Kill, Node);
        }
        continue; // no point in executing the rest
      }
      if (Variable *Dest = I->getDest()) {
        SizeT DestNum = Dest->getIndex();
        assert(DestNum < Metadata.size());
        Metadata[DestNum].markDef(I, Node);
      }
      for (SizeT SrcNum = 0; SrcNum < I->getSrcSize(); ++SrcNum) {
        Operand *Src = I->getSrc(SrcNum);
        SizeT NumVars = Src->getNumVars();
        for (SizeT J = 0; J < NumVars; ++J) {
          const Variable *Var = Src->getVar(J);
          SizeT VarNum = Var->getIndex();
          assert(VarNum < Metadata.size());
          const bool IsFromDef = false;
          const bool IsImplicit = false;
          Metadata[VarNum].markUse(I, Node, IsFromDef, IsImplicit);
        }
      }
    }
  }
}

bool VariablesMetadata::isMultiDef(const Variable *Var) const {
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
  if (!isTracked(Var))
    return NULL; // conservative answer
  SizeT VarNum = Var->getIndex();
  return Metadata[VarNum].getFirstDefinition();
}

const Inst *VariablesMetadata::getSingleDefinition(const Variable *Var) const {
  if (!isTracked(Var))
    return NULL; // conservative answer
  SizeT VarNum = Var->getIndex();
  return Metadata[VarNum].getSingleDefinition();
}

const InstDefList &
VariablesMetadata::getDefinitions(const Variable *Var) const {
  if (!isTracked(Var))
    return NoDefinitions;
  SizeT VarNum = Var->getIndex();
  return Metadata[VarNum].getDefinitions();
}

const CfgNode *VariablesMetadata::getLocalUseNode(const Variable *Var) const {
  if (!isTracked(Var))
    return NULL; // conservative answer
  SizeT VarNum = Var->getIndex();
  return Metadata[VarNum].getNode();
}

const InstDefList VariablesMetadata::NoDefinitions;

// ======================== dump routines ======================== //

void Variable::emit(const Cfg *Func) const {
  Func->getTarget()->emitVariable(this);
}

void Variable::dump(const Cfg *Func, Ostream &Str) const {
  if (Func == NULL) {
    Str << "%" << getName();
    return;
  }
  if (Func->getContext()->isVerbose(IceV_RegOrigins) ||
      (!hasReg() && !Func->getTarget()->hasComputedFrame()))
    Str << "%" << getName();
  if (hasReg()) {
    if (Func->getContext()->isVerbose(IceV_RegOrigins))
      Str << ":";
    Str << Func->getTarget()->getRegName(RegNum, getType());
  } else if (Func->getTarget()->hasComputedFrame()) {
    if (Func->getContext()->isVerbose(IceV_RegOrigins))
      Str << ":";
    Str << "[" << Func->getTarget()->getRegName(
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

void ConstantRelocatable::emit(GlobalContext *Ctx) const {
  Ostream &Str = Ctx->getStrEmit();
  if (SuppressMangling)
    Str << Name;
  else
    Str << Ctx->mangleName(Name);
  if (Offset) {
    if (Offset > 0)
      Str << "+";
    Str << Offset;
  }
}

void ConstantRelocatable::dump(const Cfg *, Ostream &Str) const {
  Str << "@" << Name;
  if (Offset)
    Str << "+" << Offset;
}

void LiveRange::dump(Ostream &Str) const {
  Str << "(weight=" << Weight << ") ";
  bool First = true;
  for (const RangeElementType &I : Range) {
    if (First)
      Str << ", ";
    First = false;
    Str << "[" << I.first << ":" << I.second << ")";
  }
}

Ostream &operator<<(Ostream &Str, const LiveRange &L) {
  L.dump(Str);
  return Str;
}

Ostream &operator<<(Ostream &Str, const RegWeight &W) {
  if (W.getWeight() == RegWeight::Inf)
    Str << "Inf";
  else
    Str << W.getWeight();
  return Str;
}

} // end of namespace Ice
