//===- subzero/src/IceRegAlloc.cpp - Linear-scan implementation -----------===//
//
//                        The Subzero Code Generator
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file implements the LinearScan class, which performs the linear-scan
/// register allocation after liveness analysis has been performed.
///
//===----------------------------------------------------------------------===//

#include "IceRegAlloc.h"

#include "IceCfg.h"
#include "IceCfgNode.h"
#include "IceInst.h"
#include "IceOperand.h"
#include "IceTargetLowering.h"

namespace Ice {

namespace {

// Returns true if Var has any definitions within Item's live range.
// TODO(stichnot): Consider trimming the Definitions list similar to how the
// live ranges are trimmed, since all the overlapsDefs() tests are whether some
// variable's definitions overlap Cur, and trimming is with respect Cur.start.
// Initial tests show no measurable performance difference, so we'll keep the
// code simple for now.
bool overlapsDefs(const Cfg *Func, const Variable *Item, const Variable *Var) {
  constexpr bool UseTrimmed = true;
  VariablesMetadata *VMetadata = Func->getVMetadata();
  if (const Inst *FirstDef = VMetadata->getFirstDefinition(Var))
    if (Item->getLiveRange().overlapsInst(FirstDef->getNumber(), UseTrimmed))
      return true;
  const InstDefList &Defs = VMetadata->getLatterDefinitions(Var);
  for (size_t i = 0; i < Defs.size(); ++i) {
    if (Item->getLiveRange().overlapsInst(Defs[i]->getNumber(), UseTrimmed))
      return true;
  }
  return false;
}

void dumpDisableOverlap(const Cfg *Func, const Variable *Var,
                        const char *Reason) {
  if (!BuildDefs::dump())
    return;
  if (Func->isVerbose(IceV_LinearScan)) {
    VariablesMetadata *VMetadata = Func->getVMetadata();
    Ostream &Str = Func->getContext()->getStrDump();
    Str << "Disabling Overlap due to " << Reason << " " << *Var
        << " LIVE=" << Var->getLiveRange() << " Defs=";
    if (const Inst *FirstDef = VMetadata->getFirstDefinition(Var))
      Str << FirstDef->getNumber();
    const InstDefList &Defs = VMetadata->getLatterDefinitions(Var);
    for (size_t i = 0; i < Defs.size(); ++i) {
      Str << "," << Defs[i]->getNumber();
    }
    Str << "\n";
  }
}

void dumpLiveRange(const Variable *Var, const Cfg *Func) {
  if (!BuildDefs::dump())
    return;
  Ostream &Str = Func->getContext()->getStrDump();
  char buf[30];
  snprintf(buf, llvm::array_lengthof(buf), "%2d", Var->getRegNumTmp());
  Str << "R=" << buf << "  V=";
  Var->dump(Func);
  Str << "  Range=" << Var->getLiveRange();
}

} // end of anonymous namespace

LinearScan::LinearScan(Cfg *Func)
    : Func(Func), Ctx(Func->getContext()),
      Verbose(BuildDefs::dump() && Func->isVerbose(IceV_LinearScan)) {}

// Prepare for full register allocation of all variables.  We depend on
// liveness analysis to have calculated live ranges.
void LinearScan::initForGlobal() {
  TimerMarker T(TimerStack::TT_initUnhandled, Func);
  FindPreference = true;
  // For full register allocation, normally we want to enable FindOverlap
  // (meaning we look for opportunities for two overlapping live ranges to
  // safely share the same register).  However, we disable it for phi-lowering
  // register allocation since no overlap opportunities should be available and
  // it's more expensive to look for opportunities.
  FindOverlap = (Kind != RAK_Phi);
  const VarList &Vars = Func->getVariables();
  Unhandled.reserve(Vars.size());
  UnhandledPrecolored.reserve(Vars.size());
  // Gather the live ranges of all variables and add them to the Unhandled set.
  for (Variable *Var : Vars) {
    // Explicitly don't consider zero-weight variables, which are meant to be
    // spill slots.
    if (Var->getWeight().isZero())
      continue;
    // Don't bother if the variable has a null live range, which means it was
    // never referenced.
    if (Var->getLiveRange().isEmpty())
      continue;
    Var->untrimLiveRange();
    Unhandled.push_back(Var);
    if (Var->hasReg()) {
      Var->setRegNumTmp(Var->getRegNum());
      Var->setLiveRangeInfiniteWeight();
      UnhandledPrecolored.push_back(Var);
    }
  }

  // Build the (ordered) list of FakeKill instruction numbers.
  Kills.clear();
  // Phi lowering should not be creating new call instructions, so there should
  // be no infinite-weight not-yet-colored live ranges that span a call
  // instruction, hence no need to construct the Kills list.
  if (Kind == RAK_Phi)
    return;
  for (CfgNode *Node : Func->getNodes()) {
    for (Inst &I : Node->getInsts()) {
      if (auto Kill = llvm::dyn_cast<InstFakeKill>(&I)) {
        if (!Kill->isDeleted() && !Kill->getLinked()->isDeleted())
          Kills.push_back(I.getNumber());
      }
    }
  }
}

// Prepare for very simple register allocation of only infinite-weight
// Variables while respecting pre-colored Variables. Some properties we take
// advantage of:
//
// * Live ranges of interest consist of a single segment.
//
// * Live ranges of interest never span a call instruction.
//
// * Phi instructions are not considered because either phis have already been
//   lowered, or they don't contain any pre-colored or infinite-weight
//   Variables.
//
// * We don't need to renumber instructions before computing live ranges
//   because all the high-level ICE instructions are deleted prior to lowering,
//   and the low-level instructions are added in monotonically increasing order.
//
// * There are no opportunities for register preference or allowing overlap.
//
// Some properties we aren't (yet) taking advantage of:
//
// * Because live ranges are a single segment, the Inactive set will always be
//   empty, and the live range trimming operation is unnecessary.
//
// * Calculating overlap of single-segment live ranges could be optimized a
//   bit.
void LinearScan::initForInfOnly() {
  TimerMarker T(TimerStack::TT_initUnhandled, Func);
  FindPreference = false;
  FindOverlap = false;
  SizeT NumVars = 0;
  const VarList &Vars = Func->getVariables();

  // Iterate across all instructions and record the begin and end of the live
  // range for each variable that is pre-colored or infinite weight.
  std::vector<InstNumberT> LRBegin(Vars.size(), Inst::NumberSentinel);
  std::vector<InstNumberT> LREnd(Vars.size(), Inst::NumberSentinel);
  for (CfgNode *Node : Func->getNodes()) {
    for (Inst &Inst : Node->getInsts()) {
      if (Inst.isDeleted())
        continue;
      if (const Variable *Var = Inst.getDest()) {
        if (!Var->getIgnoreLiveness() &&
            (Var->hasReg() || Var->getWeight().isInf())) {
          if (LRBegin[Var->getIndex()] == Inst::NumberSentinel) {
            LRBegin[Var->getIndex()] = Inst.getNumber();
            ++NumVars;
          }
        }
      }
      for (SizeT I = 0; I < Inst.getSrcSize(); ++I) {
        Operand *Src = Inst.getSrc(I);
        SizeT NumVars = Src->getNumVars();
        for (SizeT J = 0; J < NumVars; ++J) {
          const Variable *Var = Src->getVar(J);
          if (Var->getIgnoreLiveness())
            continue;
          if (Var->hasReg() || Var->getWeight().isInf())
            LREnd[Var->getIndex()] = Inst.getNumber();
        }
      }
    }
  }

  Unhandled.reserve(NumVars);
  UnhandledPrecolored.reserve(NumVars);
  for (SizeT i = 0; i < Vars.size(); ++i) {
    Variable *Var = Vars[i];
    if (LRBegin[i] != Inst::NumberSentinel) {
      assert(LREnd[i] != Inst::NumberSentinel);
      Unhandled.push_back(Var);
      Var->resetLiveRange();
      constexpr uint32_t WeightDelta = 1;
      Var->addLiveRange(LRBegin[i], LREnd[i], WeightDelta);
      Var->untrimLiveRange();
      if (Var->hasReg()) {
        Var->setRegNumTmp(Var->getRegNum());
        Var->setLiveRangeInfiniteWeight();
        UnhandledPrecolored.push_back(Var);
      }
      --NumVars;
    }
  }
  // This isn't actually a fatal condition, but it would be nice to know if we
  // somehow pre-calculated Unhandled's size wrong.
  assert(NumVars == 0);

  // Don't build up the list of Kills because we know that no infinite-weight
  // Variable has a live range spanning a call.
  Kills.clear();
}

void LinearScan::init(RegAllocKind Kind) {
  this->Kind = Kind;
  Unhandled.clear();
  UnhandledPrecolored.clear();
  Handled.clear();
  Inactive.clear();
  Active.clear();

  switch (Kind) {
  case RAK_Unknown:
    llvm::report_fatal_error("Invalid RAK_Unknown");
    break;
  case RAK_Global:
  case RAK_Phi:
    initForGlobal();
    break;
  case RAK_InfOnly:
    initForInfOnly();
    break;
  }

  auto CompareRanges = [](const Variable *L, const Variable *R) {
    InstNumberT Lstart = L->getLiveRange().getStart();
    InstNumberT Rstart = R->getLiveRange().getStart();
    if (Lstart == Rstart)
      return L->getIndex() < R->getIndex();
    return Lstart < Rstart;
  };
  // Do a reverse sort so that erasing elements (from the end) is fast.
  std::sort(Unhandled.rbegin(), Unhandled.rend(), CompareRanges);
  std::sort(UnhandledPrecolored.rbegin(), UnhandledPrecolored.rend(),
            CompareRanges);

  Handled.reserve(Unhandled.size());
  Inactive.reserve(Unhandled.size());
  Active.reserve(Unhandled.size());
}

// This is called when Cur must be allocated a register but no registers are
// available across Cur's live range.  To handle this, we find a register that
// is not explicitly used during Cur's live range, spill that register to a
// stack location right before Cur's live range begins, and fill (reload) the
// register from the stack location right after Cur's live range ends.
void LinearScan::addSpillFill(IterationState &Iter) {
  // Identify the actual instructions that begin and end Iter.Cur's live range.
  // Iterate through Iter.Cur's node's instruction list until we find the actual
  // instructions with instruction numbers corresponding to Iter.Cur's recorded
  // live range endpoints.  This sounds inefficient but shouldn't be a problem
  // in practice because:
  // (1) This function is almost never called in practice.
  // (2) Since this register over-subscription problem happens only for
  //     phi-lowered instructions, the number of instructions in the node is
  //     proportional to the number of phi instructions in the original node,
  //     which is never very large in practice.
  // (3) We still have to iterate through all instructions of Iter.Cur's live
  //     range to find all explicitly used registers (though the live range is
  //     usually only 2-3 instructions), so the main cost that could be avoided
  //     would be finding the instruction that begin's Iter.Cur's live range.
  assert(!Iter.Cur->getLiveRange().isEmpty());
  InstNumberT Start = Iter.Cur->getLiveRange().getStart();
  InstNumberT End = Iter.Cur->getLiveRange().getEnd();
  CfgNode *Node = Func->getVMetadata()->getLocalUseNode(Iter.Cur);
  assert(Node);
  InstList &Insts = Node->getInsts();
  InstList::iterator SpillPoint = Insts.end();
  InstList::iterator FillPoint = Insts.end();
  // Stop searching after we have found both the SpillPoint and the FillPoint.
  for (auto I = Insts.begin(), E = Insts.end();
       I != E && (SpillPoint == E || FillPoint == E); ++I) {
    if (I->getNumber() == Start)
      SpillPoint = I;
    if (I->getNumber() == End)
      FillPoint = I;
    if (SpillPoint != E) {
      // Remove from RegMask any physical registers referenced during Cur's live
      // range.  Start looking after SpillPoint gets set, i.e. once Cur's live
      // range begins.
      for (SizeT i = 0; i < I->getSrcSize(); ++i) {
        Operand *Src = I->getSrc(i);
        SizeT NumVars = Src->getNumVars();
        for (SizeT j = 0; j < NumVars; ++j) {
          const Variable *Var = Src->getVar(j);
          if (Var->hasRegTmp())
            Iter.RegMask[Var->getRegNumTmp()] = false;
        }
      }
    }
  }
  assert(SpillPoint != Insts.end());
  assert(FillPoint != Insts.end());
  ++FillPoint;
  // TODO(stichnot): Randomize instead of find_first().
  int32_t RegNum = Iter.RegMask.find_first();
  assert(RegNum != -1);
  Iter.Cur->setRegNumTmp(RegNum);
  TargetLowering *Target = Func->getTarget();
  Variable *Preg = Target->getPhysicalRegister(RegNum, Iter.Cur->getType());
  // TODO(stichnot): Add SpillLoc to VariablesMetadata tracking so that SpillLoc
  // is correctly identified as !isMultiBlock(), reducing stack frame size.
  Variable *SpillLoc = Func->makeVariable(Iter.Cur->getType());
  // Add "reg=FakeDef;spill=reg" before SpillPoint
  Target->lowerInst(Node, SpillPoint, InstFakeDef::create(Func, Preg));
  Target->lowerInst(Node, SpillPoint, InstAssign::create(Func, SpillLoc, Preg));
  // add "reg=spill;FakeUse(reg)" before FillPoint
  Target->lowerInst(Node, FillPoint, InstAssign::create(Func, Preg, SpillLoc));
  Target->lowerInst(Node, FillPoint, InstFakeUse::create(Func, Preg));
}

void LinearScan::handleActiveRangeExpiredOrInactive(const Variable *Cur) {
  for (SizeT I = Active.size(); I > 0; --I) {
    const SizeT Index = I - 1;
    Variable *Item = Active[Index];
    Item->trimLiveRange(Cur->getLiveRange().getStart());
    bool Moved = false;
    if (Item->rangeEndsBefore(Cur)) {
      // Move Item from Active to Handled list.
      dumpLiveRangeTrace("Expiring     ", Cur);
      moveItem(Active, Index, Handled);
      Moved = true;
    } else if (!Item->rangeOverlapsStart(Cur)) {
      // Move Item from Active to Inactive list.
      dumpLiveRangeTrace("Inactivating ", Cur);
      moveItem(Active, Index, Inactive);
      Moved = true;
    }
    if (Moved) {
      // Decrement Item from RegUses[].
      assert(Item->hasRegTmp());
      int32_t RegNum = Item->getRegNumTmp();
      --RegUses[RegNum];
      assert(RegUses[RegNum] >= 0);
    }
  }
}

void LinearScan::handleInactiveRangeExpiredOrReactivated(const Variable *Cur) {
  for (SizeT I = Inactive.size(); I > 0; --I) {
    const SizeT Index = I - 1;
    Variable *Item = Inactive[Index];
    Item->trimLiveRange(Cur->getLiveRange().getStart());
    if (Item->rangeEndsBefore(Cur)) {
      // Move Item from Inactive to Handled list.
      dumpLiveRangeTrace("Expiring     ", Cur);
      moveItem(Inactive, Index, Handled);
    } else if (Item->rangeOverlapsStart(Cur)) {
      // Move Item from Inactive to Active list.
      dumpLiveRangeTrace("Reactivating ", Cur);
      moveItem(Inactive, Index, Active);
      // Increment Item in RegUses[].
      assert(Item->hasRegTmp());
      int32_t RegNum = Item->getRegNumTmp();
      assert(RegUses[RegNum] >= 0);
      ++RegUses[RegNum];
    }
  }
}

// Infer register preference and allowable overlap. Only form a preference when
// the current Variable has an unambiguous "first" definition. The preference
// is some source Variable of the defining instruction that either is assigned
// a register that is currently free, or that is assigned a register that is
// not free but overlap is allowed. Overlap is allowed when the Variable under
// consideration is single-definition, and its definition is a simple
// assignment - i.e., the register gets copied/aliased but is never modified.
// Furthermore, overlap is only allowed when preferred Variable definition
// instructions do not appear within the current Variable's live range.
void LinearScan::findRegisterPreference(IterationState &Iter) {
  Iter.Prefer = nullptr;
  Iter.PreferReg = Variable::NoRegister;
  Iter.AllowOverlap = false;

  if (FindPreference) {
    VariablesMetadata *VMetadata = Func->getVMetadata();
    if (const Inst *DefInst = VMetadata->getFirstDefinition(Iter.Cur)) {
      assert(DefInst->getDest() == Iter.Cur);
      bool IsAssign = DefInst->isSimpleAssign();
      bool IsSingleDef = !VMetadata->isMultiDef(Iter.Cur);
      for (SizeT i = 0; i < DefInst->getSrcSize(); ++i) {
        // TODO(stichnot): Iterate through the actual Variables of the
        // instruction, not just the source operands. This could capture Load
        // instructions, including address mode optimization, for Prefer (but
        // not for AllowOverlap).
        if (Variable *SrcVar = llvm::dyn_cast<Variable>(DefInst->getSrc(i))) {
          int32_t SrcReg = SrcVar->getRegNumTmp();
          // Only consider source variables that have (so far) been assigned a
          // register. That register must be one in the RegMask set, e.g.
          // don't try to prefer the stack pointer as a result of the stacksave
          // intrinsic.
          if (SrcVar->hasRegTmp() && Iter.RegMask[SrcReg]) {
            if (FindOverlap && !Iter.Free[SrcReg]) {
              // Don't bother trying to enable AllowOverlap if the register is
              // already free.
              Iter.AllowOverlap = IsSingleDef && IsAssign &&
                                  !overlapsDefs(Func, Iter.Cur, SrcVar);
            }
            if (Iter.AllowOverlap || Iter.Free[SrcReg]) {
              Iter.Prefer = SrcVar;
              Iter.PreferReg = SrcReg;
            }
          }
        }
      }
      if (Verbose && Iter.Prefer) {
        Ostream &Str = Ctx->getStrDump();
        Str << "Initial Iter.Prefer=";
        Iter.Prefer->dump(Func);
        Str << " R=" << Iter.PreferReg
            << " LIVE=" << Iter.Prefer->getLiveRange()
            << " Overlap=" << Iter.AllowOverlap << "\n";
      }
    }
  }
}

// Remove registers from the Free[] list where an Inactive range overlaps with
// the current range.
void LinearScan::filterFreeWithInactiveRanges(IterationState &Iter) {
  for (const Variable *Item : Inactive) {
    if (Item->rangeOverlaps(Iter.Cur)) {
      int32_t RegNum = Item->getRegNumTmp();
      // Don't assert(Free[RegNum]) because in theory (though probably never in
      // practice) there could be two inactive variables that were marked with
      // AllowOverlap.
      Iter.Free[RegNum] = false;
      // Disable AllowOverlap if an Inactive variable, which is not Prefer,
      // shares Prefer's register, and has a definition within Cur's live
      // range.
      if (Iter.AllowOverlap && Item != Iter.Prefer &&
          RegNum == Iter.PreferReg && overlapsDefs(Func, Iter.Cur, Item)) {
        Iter.AllowOverlap = false;
        dumpDisableOverlap(Func, Item, "Inactive");
      }
    }
  }
}

// Remove registers from the Free[] list where an Unhandled pre-colored range
// overlaps with the current range, and set those registers to infinite weight
// so that they aren't candidates for eviction.  Cur->rangeEndsBefore(Item) is
// an early exit check that turns a guaranteed O(N^2) algorithm into expected
// linear complexity.
void LinearScan::filterFreeWithPrecoloredRanges(IterationState &Iter) {
  for (Variable *Item : reverse_range(UnhandledPrecolored)) {
    assert(Item->hasReg());
    if (Iter.Cur->rangeEndsBefore(Item))
      break;
    if (Item->rangeOverlaps(Iter.Cur)) {
      int32_t ItemReg = Item->getRegNum(); // Note: not getRegNumTmp()
      Iter.Weights[ItemReg].setWeight(RegWeight::Inf);
      Iter.Free[ItemReg] = false;
      Iter.PrecoloredUnhandledMask[ItemReg] = true;
      // Disable Iter.AllowOverlap if the preferred register is one of these
      // pre-colored unhandled overlapping ranges.
      if (Iter.AllowOverlap && ItemReg == Iter.PreferReg) {
        Iter.AllowOverlap = false;
        dumpDisableOverlap(Func, Item, "PrecoloredUnhandled");
      }
    }
  }
}

void LinearScan::allocatePrecoloredRegister(Variable *Cur) {
  int32_t RegNum = Cur->getRegNum();
  // RegNumTmp should have already been set above.
  assert(Cur->getRegNumTmp() == RegNum);
  dumpLiveRangeTrace("Precoloring  ", Cur);
  Active.push_back(Cur);
  assert(RegUses[RegNum] >= 0);
  ++RegUses[RegNum];
  assert(!UnhandledPrecolored.empty());
  assert(UnhandledPrecolored.back() == Cur);
  UnhandledPrecolored.pop_back();
}

void LinearScan::allocatePreferredRegister(IterationState &Iter) {
  Iter.Cur->setRegNumTmp(Iter.PreferReg);
  dumpLiveRangeTrace("Preferring   ", Iter.Cur);
  assert(RegUses[Iter.PreferReg] >= 0);
  ++RegUses[Iter.PreferReg];
  Active.push_back(Iter.Cur);
}

void LinearScan::allocateFreeRegister(IterationState &Iter) {
  int32_t RegNum = Iter.Free.find_first();
  Iter.Cur->setRegNumTmp(RegNum);
  dumpLiveRangeTrace("Allocating   ", Iter.Cur);
  assert(RegUses[RegNum] >= 0);
  ++RegUses[RegNum];
  Active.push_back(Iter.Cur);
}

void LinearScan::handleNoFreeRegisters(IterationState &Iter) {
  // Check Active ranges.
  for (const Variable *Item : Active) {
    assert(Item->rangeOverlaps(Iter.Cur));
    int32_t RegNum = Item->getRegNumTmp();
    assert(Item->hasRegTmp());
    Iter.Weights[RegNum].addWeight(Item->getLiveRange().getWeight());
  }
  // Same as above, but check Inactive ranges instead of Active.
  for (const Variable *Item : Inactive) {
    int32_t RegNum = Item->getRegNumTmp();
    assert(Item->hasRegTmp());
    if (Item->rangeOverlaps(Iter.Cur))
      Iter.Weights[RegNum].addWeight(Item->getLiveRange().getWeight());
  }

  // All the weights are now calculated. Find the register with smallest
  // weight.
  int32_t MinWeightIndex = Iter.RegMask.find_first();
  // MinWeightIndex must be valid because of the initial RegMask.any() test.
  assert(MinWeightIndex >= 0);
  for (SizeT i = MinWeightIndex + 1; i < Iter.Weights.size(); ++i) {
    if (Iter.RegMask[i] && Iter.Weights[i] < Iter.Weights[MinWeightIndex])
      MinWeightIndex = i;
  }

  if (Iter.Cur->getLiveRange().getWeight() <= Iter.Weights[MinWeightIndex]) {
    // Cur doesn't have priority over any other live ranges, so don't allocate
    // any register to it, and move it to the Handled state.
    Handled.push_back(Iter.Cur);
    if (Iter.Cur->getLiveRange().getWeight().isInf()) {
      if (Kind == RAK_Phi)
        addSpillFill(Iter);
      else
        Func->setError("Unable to find a physical register for an "
                       "infinite-weight live range");
    }
  } else {
    // Evict all live ranges in Active that register number MinWeightIndex is
    // assigned to.
    for (SizeT I = Active.size(); I > 0; --I) {
      const SizeT Index = I - 1;
      Variable *Item = Active[Index];
      if (Item->getRegNumTmp() == MinWeightIndex) {
        dumpLiveRangeTrace("Evicting     ", Item);
        --RegUses[MinWeightIndex];
        assert(RegUses[MinWeightIndex] >= 0);
        Item->setRegNumTmp(Variable::NoRegister);
        moveItem(Active, Index, Handled);
      }
    }
    // Do the same for Inactive.
    for (SizeT I = Inactive.size(); I > 0; --I) {
      const SizeT Index = I - 1;
      Variable *Item = Inactive[Index];
      // Note: The Item->rangeOverlaps(Cur) clause is not part of the
      // description of AssignMemLoc() in the original paper.  But there
      // doesn't seem to be any need to evict an inactive live range that
      // doesn't overlap with the live range currently being considered. It's
      // especially bad if we would end up evicting an infinite-weight but
      // currently-inactive live range. The most common situation for this
      // would be a scratch register kill set for call instructions.
      if (Item->getRegNumTmp() == MinWeightIndex &&
          Item->rangeOverlaps(Iter.Cur)) {
        dumpLiveRangeTrace("Evicting     ", Item);
        Item->setRegNumTmp(Variable::NoRegister);
        moveItem(Inactive, Index, Handled);
      }
    }
    // Assign the register to Cur.
    Iter.Cur->setRegNumTmp(MinWeightIndex);
    assert(RegUses[MinWeightIndex] >= 0);
    ++RegUses[MinWeightIndex];
    Active.push_back(Iter.Cur);
    dumpLiveRangeTrace("Allocating   ", Iter.Cur);
  }
}

void LinearScan::assignFinalRegisters(
    const llvm::SmallBitVector &RegMaskFull,
    const llvm::SmallBitVector &PreDefinedRegisters, bool Randomized) {
  const size_t NumRegisters = RegMaskFull.size();
  llvm::SmallVector<int32_t, REGS_SIZE> Permutation(NumRegisters);
  if (Randomized) {
    // Create a random number generator for regalloc randomization. Merge
    // function's sequence and Kind value as the Salt. Because regAlloc() is
    // called twice under O2, the second time with RAK_Phi, we check
    // Kind == RAK_Phi to determine the lowest-order bit to make sure the Salt
    // is different.
    uint64_t Salt =
        (Func->getSequenceNumber() << 1) ^ (Kind == RAK_Phi ? 0u : 1u);
    Func->getTarget()->makeRandomRegisterPermutation(
        Permutation, PreDefinedRegisters | ~RegMaskFull, Salt);
  }

  // Finish up by setting RegNum = RegNumTmp (or a random permutation thereof)
  // for each Variable.
  for (Variable *Item : Handled) {
    int32_t RegNum = Item->getRegNumTmp();
    int32_t AssignedRegNum = RegNum;

    if (Randomized && Item->hasRegTmp() && !Item->hasReg()) {
      AssignedRegNum = Permutation[RegNum];
    }
    if (Verbose) {
      Ostream &Str = Ctx->getStrDump();
      if (!Item->hasRegTmp()) {
        Str << "Not assigning ";
        Item->dump(Func);
        Str << "\n";
      } else {
        Str << (AssignedRegNum == Item->getRegNum() ? "Reassigning "
                                                    : "Assigning ")
            << Func->getTarget()->getRegName(AssignedRegNum, IceType_i32)
            << "(r" << AssignedRegNum << ") to ";
        Item->dump(Func);
        Str << "\n";
      }
    }
    Item->setRegNum(AssignedRegNum);
  }
}

// Implements the linear-scan algorithm. Based on "Linear Scan Register
// Allocation in the Context of SSA Form and Register Constraints" by Hanspeter
// Mössenböck and Michael Pfeiffer,
// ftp://ftp.ssw.uni-linz.ac.at/pub/Papers/Moe02.PDF. This implementation is
// modified to take affinity into account and allow two interfering variables
// to share the same register in certain cases.
//
// Requires running Cfg::liveness(Liveness_Intervals) in preparation. Results
// are assigned to Variable::RegNum for each Variable.
void LinearScan::scan(const llvm::SmallBitVector &RegMaskFull,
                      bool Randomized) {
  TimerMarker T(TimerStack::TT_linearScan, Func);
  assert(RegMaskFull.any()); // Sanity check
  if (Verbose)
    Ctx->lockStr();
  Func->resetCurrentNode();
  const size_t NumRegisters = RegMaskFull.size();
  llvm::SmallBitVector PreDefinedRegisters(NumRegisters);
  if (Randomized) {
    for (Variable *Var : UnhandledPrecolored) {
      PreDefinedRegisters[Var->getRegNum()] = true;
    }
  }

  // Build a LiveRange representing the Kills list.
  LiveRange KillsRange(Kills);
  KillsRange.untrim();

  // Reset the register use count
  RegUses.resize(NumRegisters);
  std::fill(RegUses.begin(), RegUses.end(), 0);

  // Unhandled is already set to all ranges in increasing order of start
  // points.
  assert(Active.empty());
  assert(Inactive.empty());
  assert(Handled.empty());
  const TargetLowering::RegSetMask RegsInclude =
      TargetLowering::RegSet_CallerSave;
  const TargetLowering::RegSetMask RegsExclude = TargetLowering::RegSet_None;
  const llvm::SmallBitVector KillsMask =
      Func->getTarget()->getRegisterSet(RegsInclude, RegsExclude);

  // Allocate memory once outside the loop
  IterationState Iter;
  Iter.Weights.reserve(NumRegisters);
  Iter.PrecoloredUnhandledMask.reserve(NumRegisters);

  while (!Unhandled.empty()) {
    Iter.Cur = Unhandled.back();
    Unhandled.pop_back();
    dumpLiveRangeTrace("\nConsidering  ", Iter.Cur);
    Iter.RegMask =
        RegMaskFull &
        Func->getTarget()->getRegisterSetForType(Iter.Cur->getType());
    KillsRange.trim(Iter.Cur->getLiveRange().getStart());

    // Check for pre-colored ranges. If Cur is pre-colored, it definitely gets
    // that register. Previously processed live ranges would have avoided that
    // register due to it being pre-colored. Future processed live ranges won't
    // evict that register because the live range has infinite weight.
    if (Iter.Cur->hasReg()) {
      allocatePrecoloredRegister(Iter.Cur);
      continue;
    }

    handleActiveRangeExpiredOrInactive(Iter.Cur);
    handleInactiveRangeExpiredOrReactivated(Iter.Cur);

    // Calculate available registers into Free[].
    Iter.Free = Iter.RegMask;
    for (SizeT i = 0; i < Iter.RegMask.size(); ++i) {
      if (RegUses[i] > 0)
        Iter.Free[i] = false;
    }

    findRegisterPreference(Iter);
    filterFreeWithInactiveRanges(Iter);

    // Disable AllowOverlap if an Active variable, which is not Prefer, shares
    // Prefer's register, and has a definition within Cur's live range.
    if (Iter.AllowOverlap) {
      for (const Variable *Item : Active) {
        int32_t RegNum = Item->getRegNumTmp();
        if (Item != Iter.Prefer && RegNum == Iter.PreferReg &&
            overlapsDefs(Func, Iter.Cur, Item)) {
          Iter.AllowOverlap = false;
          dumpDisableOverlap(Func, Item, "Active");
        }
      }
    }

    Iter.Weights.resize(Iter.RegMask.size());
    std::fill(Iter.Weights.begin(), Iter.Weights.end(), RegWeight());

    Iter.PrecoloredUnhandledMask.resize(Iter.RegMask.size());
    Iter.PrecoloredUnhandledMask.reset();

    filterFreeWithPrecoloredRanges(Iter);

    // Remove scratch registers from the Free[] list, and mark their Weights[]
    // as infinite, if KillsRange overlaps Cur's live range.
    constexpr bool UseTrimmed = true;
    if (Iter.Cur->getLiveRange().overlaps(KillsRange, UseTrimmed)) {
      Iter.Free.reset(KillsMask);
      for (int i = KillsMask.find_first(); i != -1;
           i = KillsMask.find_next(i)) {
        Iter.Weights[i].setWeight(RegWeight::Inf);
        if (Iter.PreferReg == i)
          Iter.AllowOverlap = false;
      }
    }

    // Print info about physical register availability.
    if (Verbose) {
      Ostream &Str = Ctx->getStrDump();
      for (SizeT i = 0; i < Iter.RegMask.size(); ++i) {
        if (Iter.RegMask[i]) {
          Str << Func->getTarget()->getRegName(i, IceType_i32)
              << "(U=" << RegUses[i] << ",F=" << Iter.Free[i]
              << ",P=" << Iter.PrecoloredUnhandledMask[i] << ") ";
        }
      }
      Str << "\n";
    }

    if (Iter.Prefer && (Iter.AllowOverlap || Iter.Free[Iter.PreferReg])) {
      // First choice: a preferred register that is either free or is allowed
      // to overlap with its linked variable.
      allocatePreferredRegister(Iter);
    } else if (Iter.Free.any()) {
      // Second choice: any free register.
      allocateFreeRegister(Iter);
    } else {
      // Fallback: there are no free registers, so we look for the
      // lowest-weight register and see if Cur has higher weight.
      handleNoFreeRegisters(Iter);
    }
    dump(Func);
  }

  // Move anything Active or Inactive to Handled for easier handling.
  Handled.insert(Handled.end(), Active.begin(), Active.end());
  Active.clear();
  Handled.insert(Handled.end(), Inactive.begin(), Inactive.end());
  Inactive.clear();
  dump(Func);

  assignFinalRegisters(RegMaskFull, PreDefinedRegisters, Randomized);

  // TODO: Consider running register allocation one more time, with infinite
  // registers, for two reasons. First, evicted live ranges get a second chance
  // for a register. Second, it allows coalescing of stack slots. If there is
  // no time budget for the second register allocation run, each unallocated
  // variable just gets its own slot.
  //
  // Another idea for coalescing stack slots is to initialize the Unhandled
  // list with just the unallocated variables, saving time but not offering
  // second-chance opportunities.

  if (Verbose)
    Ctx->unlockStr();
}

// ======================== Dump routines ======================== //

void LinearScan::dumpLiveRangeTrace(const char *Label, const Variable *Item) {
  if (!BuildDefs::dump())
    return;

  if (Verbose) {
    Ostream &Str = Ctx->getStrDump();
    Str << Label;
    dumpLiveRange(Item, Func);
    Str << "\n";
  }
}

void LinearScan::dump(Cfg *Func) const {
  if (!BuildDefs::dump())
    return;
  if (!Func->isVerbose(IceV_LinearScan))
    return;
  Ostream &Str = Func->getContext()->getStrDump();
  Func->resetCurrentNode();
  Str << "**** Current regalloc state:\n";
  Str << "++++++ Handled:\n";
  for (const Variable *Item : Handled) {
    dumpLiveRange(Item, Func);
    Str << "\n";
  }
  Str << "++++++ Unhandled:\n";
  for (const Variable *Item : reverse_range(Unhandled)) {
    dumpLiveRange(Item, Func);
    Str << "\n";
  }
  Str << "++++++ Active:\n";
  for (const Variable *Item : Active) {
    dumpLiveRange(Item, Func);
    Str << "\n";
  }
  Str << "++++++ Inactive:\n";
  for (const Variable *Item : Inactive) {
    dumpLiveRange(Item, Func);
    Str << "\n";
  }
}

} // end of namespace Ice
