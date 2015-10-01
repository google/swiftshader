//===- subzero/src/IceCfgNode.cpp - Basic block (node) implementation -----===//
//
//                        The Subzero Code Generator
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file implements the CfgNode class, including the complexities of
/// instruction insertion and in-edge calculation.
///
//===----------------------------------------------------------------------===//

#include "IceCfgNode.h"

#include "IceAssembler.h"
#include "IceCfg.h"
#include "IceGlobalInits.h"
#include "IceInst.h"
#include "IceInstVarIter.h"
#include "IceLiveness.h"
#include "IceOperand.h"
#include "IceTargetLowering.h"

namespace Ice {

CfgNode::CfgNode(Cfg *Func, SizeT LabelNumber)
    : Func(Func), Number(LabelNumber), LabelNumber(LabelNumber) {}

// Returns the name the node was created with. If no name was given, it
// synthesizes a (hopefully) unique name.
IceString CfgNode::getName() const {
  if (NameIndex >= 0)
    return Func->getIdentifierName(NameIndex);
  return "__" + std::to_string(LabelNumber);
}

// Adds an instruction to either the Phi list or the regular instruction list.
// Validates that all Phis are added before all regular instructions.
void CfgNode::appendInst(Inst *Inst) {
  ++InstCountEstimate;
  if (InstPhi *Phi = llvm::dyn_cast<InstPhi>(Inst)) {
    if (!Insts.empty()) {
      Func->setError("Phi instruction added to the middle of a block");
      return;
    }
    Phis.push_back(Phi);
  } else {
    Insts.push_back(Inst);
  }
}

// Renumbers the non-deleted instructions in the node. This needs to be done in
// preparation for live range analysis. The instruction numbers in a block must
// be monotonically increasing. The range of instruction numbers in a block,
// from lowest to highest, must not overlap with the range of any other block.
void CfgNode::renumberInstructions() {
  InstNumberT FirstNumber = Func->getNextInstNumber();
  for (Inst &I : Phis)
    I.renumber(Func);
  for (Inst &I : Insts)
    I.renumber(Func);
  InstCountEstimate = Func->getNextInstNumber() - FirstNumber;
}

// When a node is created, the OutEdges are immediately known, but the InEdges
// have to be built up incrementally. After the CFG has been constructed, the
// computePredecessors() pass finalizes it by creating the InEdges list.
void CfgNode::computePredecessors() {
  for (CfgNode *Succ : OutEdges)
    Succ->InEdges.push_back(this);
}

void CfgNode::computeSuccessors() {
  OutEdges = Insts.rbegin()->getTerminatorEdges();
}

// Validate each Phi instruction in the node with respect to control flow. For
// every phi argument, its label must appear in the predecessor list. For each
// predecessor, there must be a phi argument with that label. We don't check
// that phi arguments with the same label have the same value.
void CfgNode::validatePhis() {
  for (Inst &Instr : Phis) {
    auto *Phi = llvm::cast<InstPhi>(&Instr);
    // We do a simple O(N^2) algorithm to check for consistency. Even so, it
    // shows up as only about 0.2% of the total translation time. But if
    // necessary, we could improve the complexity by using a hash table to
    // count how many times each node is referenced in the Phi instruction, and
    // how many times each node is referenced in the incoming edge list, and
    // compare the two for equality.
    for (SizeT i = 0; i < Phi->getSrcSize(); ++i) {
      CfgNode *Label = Phi->getLabel(i);
      bool Found = false;
      for (CfgNode *InNode : getInEdges()) {
        if (InNode == Label) {
          Found = true;
          break;
        }
      }
      if (!Found)
        llvm::report_fatal_error("Phi error: label for bad incoming edge");
    }
    for (CfgNode *InNode : getInEdges()) {
      bool Found = false;
      for (SizeT i = 0; i < Phi->getSrcSize(); ++i) {
        CfgNode *Label = Phi->getLabel(i);
        if (InNode == Label) {
          Found = true;
          break;
        }
      }
      if (!Found)
        llvm::report_fatal_error("Phi error: missing label for incoming edge");
    }
  }
}

// This does part 1 of Phi lowering, by creating a new dest variable for each
// Phi instruction, replacing the Phi instruction's dest with that variable,
// and adding an explicit assignment of the old dest to the new dest. For
// example,
//   a=phi(...)
// changes to
//   "a_phi=phi(...); a=a_phi".
//
// This is in preparation for part 2 which deletes the Phi instructions and
// appends assignment instructions to predecessor blocks. Note that this
// transformation preserves SSA form.
void CfgNode::placePhiLoads() {
  for (Inst &I : Phis) {
    auto Phi = llvm::dyn_cast<InstPhi>(&I);
    Insts.insert(Insts.begin(), Phi->lower(Func));
  }
}

// This does part 2 of Phi lowering. For each Phi instruction at each out-edge,
// create a corresponding assignment instruction, and add all the assignments
// near the end of this block. They need to be added before any branch
// instruction, and also if the block ends with a compare instruction followed
// by a branch instruction that we may want to fuse, it's better to insert the
// new assignments before the compare instruction. The
// tryOptimizedCmpxchgCmpBr() method assumes this ordering of instructions.
//
// Note that this transformation takes the Phi dest variables out of SSA form,
// as there may be assignments to the dest variable in multiple blocks.
void CfgNode::placePhiStores() {
  // Find the insertion point.
  InstList::iterator InsertionPoint = Insts.end();
  // Every block must end in a terminator instruction, and therefore must have
  // at least one instruction, so it's valid to decrement InsertionPoint (but
  // assert just in case).
  assert(InsertionPoint != Insts.begin());
  --InsertionPoint;
  // Confirm that InsertionPoint is a terminator instruction. Calling
  // getTerminatorEdges() on a non-terminator instruction will cause an
  // llvm_unreachable().
  (void)InsertionPoint->getTerminatorEdges();
  // SafeInsertionPoint is always immediately before the terminator
  // instruction. If the block ends in a compare and conditional branch, it's
  // better to place the Phi store before the compare so as not to interfere
  // with compare/branch fusing. However, if the compare instruction's dest
  // operand is the same as the new assignment statement's source operand, this
  // can't be done due to data dependences, so we need to fall back to the
  // SafeInsertionPoint. To illustrate:
  //   ; <label>:95
  //   %97 = load i8* %96, align 1
  //   %98 = icmp ne i8 %97, 0
  //   br i1 %98, label %99, label %2132
  //   ; <label>:99
  //   %100 = phi i8 [ %97, %95 ], [ %110, %108 ]
  //   %101 = phi i1 [ %98, %95 ], [ %111, %108 ]
  // would be Phi-lowered as:
  //   ; <label>:95
  //   %97 = load i8* %96, align 1
  //   %100_phi = %97 ; can be at InsertionPoint
  //   %98 = icmp ne i8 %97, 0
  //   %101_phi = %98 ; must be at SafeInsertionPoint
  //   br i1 %98, label %99, label %2132
  //   ; <label>:99
  //   %100 = %100_phi
  //   %101 = %101_phi
  //
  // TODO(stichnot): It may be possible to bypass this whole SafeInsertionPoint
  // mechanism. If a source basic block ends in a conditional branch:
  //   labelSource:
  //   ...
  //   br i1 %foo, label %labelTrue, label %labelFalse
  // and a branch target has a Phi involving the branch operand:
  //   labelTrue:
  //   %bar = phi i1 [ %foo, %labelSource ], ...
  // then we actually know the constant i1 value of the Phi operand:
  //   labelTrue:
  //   %bar = phi i1 [ true, %labelSource ], ...
  // It seems that this optimization should be done by clang or opt, but we
  // could also do it here.
  InstList::iterator SafeInsertionPoint = InsertionPoint;
  // Keep track of the dest variable of a compare instruction, so that we
  // insert the new instruction at the SafeInsertionPoint if the compare's dest
  // matches the Phi-lowered assignment's source.
  Variable *CmpInstDest = nullptr;
  // If the current insertion point is at a conditional branch instruction, and
  // the previous instruction is a compare instruction, then we move the
  // insertion point before the compare instruction so as not to interfere with
  // compare/branch fusing.
  if (InstBr *Branch = llvm::dyn_cast<InstBr>(InsertionPoint)) {
    if (!Branch->isUnconditional()) {
      if (InsertionPoint != Insts.begin()) {
        --InsertionPoint;
        if (llvm::isa<InstIcmp>(InsertionPoint) ||
            llvm::isa<InstFcmp>(InsertionPoint)) {
          CmpInstDest = InsertionPoint->getDest();
        } else {
          ++InsertionPoint;
        }
      }
    }
  }

  // Consider every out-edge.
  for (CfgNode *Succ : OutEdges) {
    // Consider every Phi instruction at the out-edge.
    for (Inst &I : Succ->Phis) {
      auto Phi = llvm::dyn_cast<InstPhi>(&I);
      Operand *Operand = Phi->getOperandForTarget(this);
      assert(Operand);
      Variable *Dest = I.getDest();
      assert(Dest);
      InstAssign *NewInst = InstAssign::create(Func, Dest, Operand);
      if (CmpInstDest == Operand)
        Insts.insert(SafeInsertionPoint, NewInst);
      else
        Insts.insert(InsertionPoint, NewInst);
    }
  }
}

// Deletes the phi instructions after the loads and stores are placed.
void CfgNode::deletePhis() {
  for (Inst &I : Phis)
    I.setDeleted();
}

// Splits the edge from Pred to this node by creating a new node and hooking up
// the in and out edges appropriately. (The EdgeIndex parameter is only used to
// make the new node's name unique when there are multiple edges between the
// same pair of nodes.) The new node's instruction list is initialized to the
// empty list, with no terminator instruction. There must not be multiple edges
// from Pred to this node so all Inst::getTerminatorEdges implementations must
// not contain duplicates.
CfgNode *CfgNode::splitIncomingEdge(CfgNode *Pred, SizeT EdgeIndex) {
  CfgNode *NewNode = Func->makeNode();
  // Depth is the minimum as it works if both are the same, but if one is
  // outside the loop and the other is inside, the new node should be placed
  // outside and not be executed multiple times within the loop.
  NewNode->setLoopNestDepth(
      std::min(getLoopNestDepth(), Pred->getLoopNestDepth()));
  if (BuildDefs::dump())
    NewNode->setName("split_" + Pred->getName() + "_" + getName() + "_" +
                     std::to_string(EdgeIndex));
  // The new node is added to the end of the node list, and will later need to
  // be sorted into a reasonable topological order.
  NewNode->setNeedsPlacement(true);
  // Repoint Pred's out-edge.
  bool Found = false;
  for (CfgNode *&I : Pred->OutEdges) {
    if (I == this) {
      I = NewNode;
      NewNode->InEdges.push_back(Pred);
      Found = true;
      break;
    }
  }
  assert(Found);
  (void)Found;
  // Repoint this node's in-edge.
  Found = false;
  for (CfgNode *&I : InEdges) {
    if (I == Pred) {
      I = NewNode;
      NewNode->OutEdges.push_back(this);
      Found = true;
      break;
    }
  }
  assert(Found);
  (void)Found;
  // Repoint all suitable branch instructions' target and return.
  Found = false;
  for (Inst &I : Pred->getInsts())
    if (!I.isDeleted() && I.repointEdges(this, NewNode))
      Found = true;
  assert(Found);
  (void)Found;
  return NewNode;
}

namespace {

// Helper function used by advancedPhiLowering().
bool sameVarOrReg(const Variable *Var, const Operand *Opnd) {
  if (Var == Opnd)
    return true;
  if (const auto Var2 = llvm::dyn_cast<Variable>(Opnd)) {
    if (Var->hasReg() && Var->getRegNum() == Var2->getRegNum())
      return true;
  }
  return false;
}

} // end of anonymous namespace

// This the "advanced" version of Phi lowering for a basic block, in contrast
// to the simple version that lowers through assignments involving temporaries.
//
// All Phi instructions in a basic block are conceptually executed in parallel.
// However, if we lower Phis early and commit to a sequential ordering, we may
// end up creating unnecessary interferences which lead to worse register
// allocation. Delaying Phi scheduling until after register allocation can help
// unless there are no free registers for shuffling registers or stack slots
// and spilling becomes necessary.
//
// The advanced Phi lowering starts by finding a topological sort of the Phi
// instructions, where "A=B" comes before "B=C" due to the anti-dependence on
// B. Preexisting register assignments are considered in the topological sort.
// If a topological sort is not possible due to a cycle, the cycle is broken by
// introducing a non-parallel temporary. For example, a cycle arising from a
// permutation like "A=B;B=C;C=A" can become "T=A;A=B;B=C;C=T". All else being
// equal, prefer to schedule assignments with register-allocated Src operands
// earlier, in case that register becomes free afterwards, and prefer to
// schedule assignments with register-allocated Dest variables later, to keep
// that register free for longer.
//
// Once the ordering is determined, the Cfg edge is split and the assignment
// list is lowered by the target lowering layer. Since the assignment lowering
// may create new infinite-weight temporaries, a follow-on register allocation
// pass will be needed. To prepare for this, liveness (including live range
// calculation) of the split nodes needs to be calculated, and liveness of the
// original node need to be updated to "undo" the effects of the phi
// assignments.

// The specific placement of the new node within the Cfg node list is deferred
// until later, including after empty node contraction.
//
// After phi assignments are lowered across all blocks, another register
// allocation pass is run, focusing only on pre-colored and infinite-weight
// variables, similar to Om1 register allocation (except without the need to
// specially compute these variables' live ranges, since they have already been
// precisely calculated). The register allocator in this mode needs the ability
// to forcibly spill and reload registers in case none are naturally available.
void CfgNode::advancedPhiLowering() {
  if (getPhis().empty())
    return;

  // Count the number of non-deleted Phi instructions.
  struct PhiDesc {
    InstPhi *Phi;
    Variable *Dest;
    Operand *Src;
    bool Processed;
    size_t NumPred; // number of entries whose Src is this Dest
    int32_t Weight; // preference for topological order
  };
  llvm::SmallVector<PhiDesc, 32> Desc(getPhis().size());

  size_t NumPhis = 0;
  for (Inst &I : Phis) {
    auto *Inst = llvm::dyn_cast<InstPhi>(&I);
    if (!Inst->isDeleted()) {
      Variable *Dest = Inst->getDest();
      Desc[NumPhis].Phi = Inst;
      Desc[NumPhis].Dest = Dest;
      ++NumPhis;
      // Undo the effect of the phi instruction on this node's live-in set by
      // marking the phi dest variable as live on entry.
      SizeT VarNum = Func->getLiveness()->getLiveIndex(Dest->getIndex());
      assert(!Func->getLiveness()->getLiveIn(this)[VarNum]);
      Func->getLiveness()->getLiveIn(this)[VarNum] = true;
      Inst->setDeleted();
    }
  }
  if (NumPhis == 0)
    return;

  SizeT InEdgeIndex = 0;
  for (CfgNode *Pred : InEdges) {
    CfgNode *Split = splitIncomingEdge(Pred, InEdgeIndex++);
    SizeT Remaining = NumPhis;

    // First pass computes Src and initializes NumPred.
    for (size_t I = 0; I < NumPhis; ++I) {
      Variable *Dest = Desc[I].Dest;
      Operand *Src = Desc[I].Phi->getOperandForTarget(Pred);
      Desc[I].Src = Src;
      Desc[I].Processed = false;
      Desc[I].NumPred = 0;
      // Cherry-pick any trivial assignments, so that they don't contribute to
      // the running complexity of the topological sort.
      if (sameVarOrReg(Dest, Src)) {
        Desc[I].Processed = true;
        --Remaining;
        if (Dest != Src)
          // If Dest and Src are syntactically the same, don't bother adding
          // the assignment, because in all respects it would be redundant, and
          // if Dest/Src are on the stack, the target lowering may naively
          // decide to lower it using a temporary register.
          Split->appendInst(InstAssign::create(Func, Dest, Src));
      }
    }
    // Second pass computes NumPred by comparing every pair of Phi
    // instructions.
    for (size_t I = 0; I < NumPhis; ++I) {
      if (Desc[I].Processed)
        continue;
      const Variable *Dest = Desc[I].Dest;
      for (size_t J = 0; J < NumPhis; ++J) {
        if (Desc[J].Processed)
          continue;
        if (I != J) {
          // There shouldn't be two Phis with the same Dest variable or
          // register.
          assert(!sameVarOrReg(Dest, Desc[J].Dest));
        }
        const Operand *Src = Desc[J].Src;
        if (sameVarOrReg(Dest, Src))
          ++Desc[I].NumPred;
      }
    }

    // Another pass to compute initial Weight values.

    // Always pick NumPred=0 over NumPred>0.
    constexpr int32_t WeightNoPreds = 4;
    // Prefer Src as a register because the register might free up.
    constexpr int32_t WeightSrcIsReg = 2;
    // Prefer Dest not as a register because the register stays free longer.
    constexpr int32_t WeightDestNotReg = 1;

    for (size_t I = 0; I < NumPhis; ++I) {
      if (Desc[I].Processed)
        continue;
      int32_t Weight = 0;
      if (Desc[I].NumPred == 0)
        Weight += WeightNoPreds;
      if (auto Var = llvm::dyn_cast<Variable>(Desc[I].Src))
        if (Var->hasReg())
          Weight += WeightSrcIsReg;
      if (!Desc[I].Dest->hasReg())
        Weight += WeightDestNotReg;
      Desc[I].Weight = Weight;
    }

    // Repeatedly choose and process the best candidate in the topological
    // sort, until no candidates remain. This implementation is O(N^2) where N
    // is the number of Phi instructions, but with a small constant factor
    // compared to a likely implementation of O(N) topological sort.
    for (; Remaining; --Remaining) {
      size_t BestIndex = 0;
      int32_t BestWeight = -1;
      // Find the best candidate.
      for (size_t I = 0; I < NumPhis; ++I) {
        if (Desc[I].Processed)
          continue;
        int32_t Weight = 0;
        Weight = Desc[I].Weight;
        if (Weight > BestWeight) {
          BestIndex = I;
          BestWeight = Weight;
        }
      }
      assert(BestWeight >= 0);
      assert(Desc[BestIndex].NumPred <= 1);
      Variable *Dest = Desc[BestIndex].Dest;
      Operand *Src = Desc[BestIndex].Src;
      assert(!sameVarOrReg(Dest, Src));
      // Break a cycle by introducing a temporary.
      if (Desc[BestIndex].NumPred) {
        bool Found = false;
        // If the target instruction "A=B" is part of a cycle, find the "X=A"
        // assignment in the cycle because it will have to be rewritten as
        // "X=tmp".
        for (size_t J = 0; !Found && J < NumPhis; ++J) {
          if (Desc[J].Processed)
            continue;
          Operand *OtherSrc = Desc[J].Src;
          if (Desc[J].NumPred && sameVarOrReg(Dest, OtherSrc)) {
            SizeT VarNum = Func->getNumVariables();
            Variable *Tmp = Func->makeVariable(OtherSrc->getType());
            if (BuildDefs::dump())
              Tmp->setName(Func, "__split_" + std::to_string(VarNum));
            Split->appendInst(InstAssign::create(Func, Tmp, OtherSrc));
            Desc[J].Src = Tmp;
            Found = true;
          }
        }
        assert(Found);
      }
      // Now that a cycle (if any) has been broken, create the actual
      // assignment.
      Split->appendInst(InstAssign::create(Func, Dest, Src));
      // Update NumPred for all Phi assignments using this Phi's Src as their
      // Dest variable. Also update Weight if NumPred dropped from 1 to 0.
      if (auto Var = llvm::dyn_cast<Variable>(Src)) {
        for (size_t I = 0; I < NumPhis; ++I) {
          if (Desc[I].Processed)
            continue;
          if (sameVarOrReg(Var, Desc[I].Dest)) {
            if (--Desc[I].NumPred == 0)
              Desc[I].Weight += WeightNoPreds;
          }
        }
      }
      Desc[BestIndex].Processed = true;
    }
    Split->appendInst(InstBr::create(Func, this));

    Split->genCode();
    Func->getVMetadata()->addNode(Split);
  }
}

// Does address mode optimization. Pass each instruction to the TargetLowering
// object. If it returns a new instruction (representing the optimized address
// mode), then insert the new instruction and delete the old.
void CfgNode::doAddressOpt() {
  TargetLowering *Target = Func->getTarget();
  LoweringContext &Context = Target->getContext();
  Context.init(this);
  while (!Context.atEnd()) {
    Target->doAddressOpt();
  }
}

void CfgNode::doNopInsertion(RandomNumberGenerator &RNG) {
  TargetLowering *Target = Func->getTarget();
  LoweringContext &Context = Target->getContext();
  Context.init(this);
  Context.setInsertPoint(Context.getCur());
  // Do not insert nop in bundle locked instructions.
  bool PauseNopInsertion = false;
  while (!Context.atEnd()) {
    if (llvm::isa<InstBundleLock>(Context.getCur())) {
      PauseNopInsertion = true;
    } else if (llvm::isa<InstBundleUnlock>(Context.getCur())) {
      PauseNopInsertion = false;
    }
    if (!PauseNopInsertion)
      Target->doNopInsertion(RNG);
    // Ensure Cur=Next, so that the nops are inserted before the current
    // instruction rather than after.
    Context.advanceCur();
    Context.advanceNext();
  }
}

// Drives the target lowering. Passes the current instruction and the next
// non-deleted instruction for target lowering.
void CfgNode::genCode() {
  TargetLowering *Target = Func->getTarget();
  LoweringContext &Context = Target->getContext();
  // Lower the regular instructions.
  Context.init(this);
  Target->initNodeForLowering(this);
  while (!Context.atEnd()) {
    InstList::iterator Orig = Context.getCur();
    if (llvm::isa<InstRet>(*Orig))
      setHasReturn();
    Target->lower();
    // Ensure target lowering actually moved the cursor.
    assert(Context.getCur() != Orig);
  }
  // Do preliminary lowering of the Phi instructions.
  Target->prelowerPhis();
}

void CfgNode::livenessLightweight() {
  SizeT NumVars = Func->getNumVariables();
  LivenessBV Live(NumVars);
  // Process regular instructions in reverse order.
  for (Inst &I : reverse_range(Insts)) {
    if (I.isDeleted())
      continue;
    I.livenessLightweight(Func, Live);
  }
  for (Inst &I : Phis) {
    if (I.isDeleted())
      continue;
    I.livenessLightweight(Func, Live);
  }
}

// Performs liveness analysis on the block. Returns true if the incoming
// liveness changed from before, false if it stayed the same. (If it changes,
// the node's predecessors need to be processed again.)
bool CfgNode::liveness(Liveness *Liveness) {
  SizeT NumVars = Liveness->getNumVarsInNode(this);
  LivenessBV Live(NumVars);
  LiveBeginEndMap *LiveBegin = nullptr;
  LiveBeginEndMap *LiveEnd = nullptr;
  // Mark the beginning and ending of each variable's live range with the
  // sentinel instruction number 0.
  if (Liveness->getMode() == Liveness_Intervals) {
    LiveBegin = Liveness->getLiveBegin(this);
    LiveEnd = Liveness->getLiveEnd(this);
    LiveBegin->clear();
    LiveEnd->clear();
    // Guess that the number of live ranges beginning is roughly the number of
    // instructions, and same for live ranges ending.
    LiveBegin->reserve(getInstCountEstimate());
    LiveEnd->reserve(getInstCountEstimate());
  }
  // Initialize Live to be the union of all successors' LiveIn.
  for (CfgNode *Succ : OutEdges) {
    Live |= Liveness->getLiveIn(Succ);
    // Mark corresponding argument of phis in successor as live.
    for (Inst &I : Succ->Phis) {
      if (I.isDeleted())
        continue;
      auto Phi = llvm::dyn_cast<InstPhi>(&I);
      Phi->livenessPhiOperand(Live, this, Liveness);
    }
  }
  Liveness->getLiveOut(this) = Live;

  // Process regular instructions in reverse order.
  for (Inst &I : reverse_range(Insts)) {
    if (I.isDeleted())
      continue;
    I.liveness(I.getNumber(), Live, Liveness, LiveBegin, LiveEnd);
  }
  // Process phis in forward order so that we can override the instruction
  // number to be that of the earliest phi instruction in the block.
  SizeT NumNonDeadPhis = 0;
  InstNumberT FirstPhiNumber = Inst::NumberSentinel;
  for (Inst &I : Phis) {
    if (I.isDeleted())
      continue;
    if (FirstPhiNumber == Inst::NumberSentinel)
      FirstPhiNumber = I.getNumber();
    if (I.liveness(FirstPhiNumber, Live, Liveness, LiveBegin, LiveEnd))
      ++NumNonDeadPhis;
  }

  // When using the sparse representation, after traversing the instructions in
  // the block, the Live bitvector should only contain set bits for global
  // variables upon block entry. We validate this by shrinking the Live vector
  // and then testing it against the pre-shrunk version. (The shrinking is
  // required, but the validation is not.)
  LivenessBV LiveOrig = Live;
  Live.resize(Liveness->getNumGlobalVars());
  if (Live != LiveOrig) {
    if (BuildDefs::dump()) {
      // This is a fatal liveness consistency error. Print some diagnostics and
      // abort.
      Ostream &Str = Func->getContext()->getStrDump();
      Func->resetCurrentNode();
      Str << "LiveOrig-Live =";
      for (SizeT i = Live.size(); i < LiveOrig.size(); ++i) {
        if (LiveOrig.test(i)) {
          Str << " ";
          Liveness->getVariable(i, this)->dump(Func);
        }
      }
      Str << "\n";
    }
    llvm::report_fatal_error("Fatal inconsistency in liveness analysis");
  }

  bool Changed = false;
  LivenessBV &LiveIn = Liveness->getLiveIn(this);
  // Add in current LiveIn
  Live |= LiveIn;
  // Check result, set LiveIn=Live
  SizeT &PrevNumNonDeadPhis = Liveness->getNumNonDeadPhis(this);
  bool LiveInChanged = (Live != LiveIn);
  Changed = (NumNonDeadPhis != PrevNumNonDeadPhis || LiveInChanged);
  if (LiveInChanged)
    LiveIn = Live;
  PrevNumNonDeadPhis = NumNonDeadPhis;
  return Changed;
}

// Validate the integrity of the live ranges in this block.  If there are any
// errors, it prints details and returns false.  On success, it returns true.
bool CfgNode::livenessValidateIntervals(Liveness *Liveness) {
  if (!BuildDefs::asserts())
    return true;

  // Verify there are no duplicates.
  auto ComparePair =
      [](const LiveBeginEndMapEntry &A, const LiveBeginEndMapEntry &B) {
        return A.first == B.first;
      };
  LiveBeginEndMap &MapBegin = *Liveness->getLiveBegin(this);
  LiveBeginEndMap &MapEnd = *Liveness->getLiveEnd(this);
  if (std::adjacent_find(MapBegin.begin(), MapBegin.end(), ComparePair) ==
          MapBegin.end() &&
      std::adjacent_find(MapEnd.begin(), MapEnd.end(), ComparePair) ==
          MapEnd.end())
    return true;

  // There is definitely a liveness error.  All paths from here return false.
  if (!BuildDefs::dump())
    return false;

  // Print all the errors.
  if (BuildDefs::dump()) {
    GlobalContext *Ctx = Func->getContext();
    OstreamLocker L(Ctx);
    Ostream &Str = Ctx->getStrDump();
    if (Func->isVerbose()) {
      Str << "Live range errors in the following block:\n";
      dump(Func);
    }
    for (auto Start = MapBegin.begin();
         (Start = std::adjacent_find(Start, MapBegin.end(), ComparePair)) !=
         MapBegin.end();
         ++Start) {
      auto Next = Start + 1;
      Str << "Duplicate LR begin, block " << getName() << ", instructions "
          << Start->second << " & " << Next->second << ", variable "
          << Liveness->getVariable(Start->first, this)->getName(Func) << "\n";
    }
    for (auto Start = MapEnd.begin();
         (Start = std::adjacent_find(Start, MapEnd.end(), ComparePair)) !=
         MapEnd.end();
         ++Start) {
      auto Next = Start + 1;
      Str << "Duplicate LR end,   block " << getName() << ", instructions "
          << Start->second << " & " << Next->second << ", variable "
          << Liveness->getVariable(Start->first, this)->getName(Func) << "\n";
    }
  }

  return false;
}

// Once basic liveness is complete, compute actual live ranges. It is assumed
// that within a single basic block, a live range begins at most once and ends
// at most once. This is certainly true for pure SSA form. It is also true once
// phis are lowered, since each assignment to the phi-based temporary is in a
// different basic block, and there is a single read that ends the live in the
// basic block that contained the actual phi instruction.
void CfgNode::livenessAddIntervals(Liveness *Liveness, InstNumberT FirstInstNum,
                                   InstNumberT LastInstNum) {
  TimerMarker T1(TimerStack::TT_liveRange, Func);

  SizeT NumVars = Liveness->getNumVarsInNode(this);
  LivenessBV &LiveIn = Liveness->getLiveIn(this);
  LivenessBV &LiveOut = Liveness->getLiveOut(this);
  LiveBeginEndMap &MapBegin = *Liveness->getLiveBegin(this);
  LiveBeginEndMap &MapEnd = *Liveness->getLiveEnd(this);
  std::sort(MapBegin.begin(), MapBegin.end());
  std::sort(MapEnd.begin(), MapEnd.end());

  if (!livenessValidateIntervals(Liveness)) {
    llvm::report_fatal_error("livenessAddIntervals: Liveness error");
    return;
  }

  LivenessBV LiveInAndOut = LiveIn;
  LiveInAndOut &= LiveOut;

  // Iterate in parallel across the sorted MapBegin[] and MapEnd[].
  auto IBB = MapBegin.begin(), IEB = MapEnd.begin();
  auto IBE = MapBegin.end(), IEE = MapEnd.end();
  while (IBB != IBE || IEB != IEE) {
    SizeT i1 = IBB == IBE ? NumVars : IBB->first;
    SizeT i2 = IEB == IEE ? NumVars : IEB->first;
    SizeT i = std::min(i1, i2);
    // i1 is the Variable number of the next MapBegin entry, and i2 is the
    // Variable number of the next MapEnd entry. If i1==i2, then the Variable's
    // live range begins and ends in this block. If i1<i2, then i1's live range
    // begins at instruction IBB->second and extends through the end of the
    // block. If i1>i2, then i2's live range begins at the first instruction of
    // the block and ends at IEB->second. In any case, we choose the lesser of
    // i1 and i2 and proceed accordingly.
    InstNumberT LB = i == i1 ? IBB->second : FirstInstNum;
    InstNumberT LE = i == i2 ? IEB->second : LastInstNum + 1;

    Variable *Var = Liveness->getVariable(i, this);
    if (LB > LE) {
      Var->addLiveRange(FirstInstNum, LE);
      Var->addLiveRange(LB, LastInstNum + 1);
      // Assert that Var is a global variable by checking that its liveness
      // index is less than the number of globals. This ensures that the
      // LiveInAndOut[] access is valid.
      assert(i < Liveness->getNumGlobalVars());
      LiveInAndOut[i] = false;
    } else {
      Var->addLiveRange(LB, LE);
    }
    if (i == i1)
      ++IBB;
    if (i == i2)
      ++IEB;
  }
  // Process the variables that are live across the entire block.
  for (int i = LiveInAndOut.find_first(); i != -1;
       i = LiveInAndOut.find_next(i)) {
    Variable *Var = Liveness->getVariable(i, this);
    if (Liveness->getRangeMask(Var->getIndex()))
      Var->addLiveRange(FirstInstNum, LastInstNum + 1);
  }
}

// If this node contains only deleted instructions, and ends in an
// unconditional branch, contract the node by repointing all its in-edges to
// its successor.
void CfgNode::contractIfEmpty() {
  if (InEdges.empty())
    return;
  Inst *Branch = nullptr;
  for (Inst &I : Insts) {
    if (I.isDeleted())
      continue;
    if (I.isUnconditionalBranch())
      Branch = &I;
    else if (!I.isRedundantAssign())
      return;
  }
  assert(OutEdges.size() == 1);
  // Don't try to delete a self-loop.
  if (OutEdges[0] == this)
    return;

  Branch->setDeleted();
  CfgNode *Successor = OutEdges.front();
  // Repoint all this node's in-edges to this node's successor, unless this
  // node's successor is actually itself (in which case the statement
  // "OutEdges.front()->InEdges.push_back(Pred)" could invalidate the iterator
  // over this->InEdges).
  if (Successor != this) {
    for (CfgNode *Pred : InEdges) {
      for (CfgNode *&I : Pred->OutEdges) {
        if (I == this) {
          I = Successor;
          Successor->InEdges.push_back(Pred);
        }
      }
      for (Inst &I : Pred->getInsts()) {
        if (!I.isDeleted())
          I.repointEdges(this, Successor);
      }
    }

    // Remove the in-edge to the successor to allow node reordering to make
    // better decisions. For example it's more helpful to place a node after a
    // reachable predecessor than an unreachable one (like the one we just
    // contracted).
    Successor->InEdges.erase(
        std::find(Successor->InEdges.begin(), Successor->InEdges.end(), this));
  }
  InEdges.clear();
}

void CfgNode::doBranchOpt(const CfgNode *NextNode) {
  TargetLowering *Target = Func->getTarget();
  // Find the first opportunity for branch optimization (which will be the last
  // instruction in the block) and stop. This is sufficient unless there is
  // some target lowering where we have the possibility of multiple
  // optimizations per block. Take care with switch lowering as there are
  // multiple unconditional branches and only the last can be deleted.
  for (Inst &I : reverse_range(Insts)) {
    if (!I.isDeleted()) {
      Target->doBranchOpt(&I, NextNode);
      return;
    }
  }
}

// ======================== Dump routines ======================== //

namespace {

// Helper functions for emit().

void emitRegisterUsage(Ostream &Str, const Cfg *Func, const CfgNode *Node,
                       bool IsLiveIn, CfgVector<SizeT> &LiveRegCount) {
  if (!BuildDefs::dump())
    return;
  Liveness *Liveness = Func->getLiveness();
  const LivenessBV *Live;
  if (IsLiveIn) {
    Live = &Liveness->getLiveIn(Node);
    Str << "\t\t\t\t# LiveIn=";
  } else {
    Live = &Liveness->getLiveOut(Node);
    Str << "\t\t\t\t# LiveOut=";
  }
  if (!Live->empty()) {
    CfgVector<Variable *> LiveRegs;
    for (SizeT i = 0; i < Live->size(); ++i) {
      if ((*Live)[i]) {
        Variable *Var = Liveness->getVariable(i, Node);
        if (Var->hasReg()) {
          if (IsLiveIn)
            ++LiveRegCount[Var->getRegNum()];
          LiveRegs.push_back(Var);
        }
      }
    }
    // Sort the variables by regnum so they are always printed in a familiar
    // order.
    std::sort(LiveRegs.begin(), LiveRegs.end(),
              [](const Variable *V1, const Variable *V2) {
                return V1->getRegNum() < V2->getRegNum();
              });
    bool First = true;
    for (Variable *Var : LiveRegs) {
      if (!First)
        Str << ",";
      First = false;
      Var->emit(Func);
    }
  }
  Str << "\n";
}

void emitLiveRangesEnded(Ostream &Str, const Cfg *Func, const Inst *Instr,
                         CfgVector<SizeT> &LiveRegCount) {
  if (!BuildDefs::dump())
    return;
  bool First = true;
  Variable *Dest = Instr->getDest();
  // Normally we increment the live count for the dest register. But we
  // shouldn't if the instruction's IsDestRedefined flag is set, because this
  // means that the target lowering created this instruction as a non-SSA
  // assignment; i.e., a different, previous instruction started the dest
  // variable's live range.
  if (!Instr->isDestRedefined() && Dest && Dest->hasReg())
    ++LiveRegCount[Dest->getRegNum()];
  FOREACH_VAR_IN_INST(Var, *Instr) {
    bool ShouldReport = Instr->isLastUse(Var);
    if (ShouldReport && Var->hasReg()) {
      // Don't report end of live range until the live count reaches 0.
      SizeT NewCount = --LiveRegCount[Var->getRegNum()];
      if (NewCount)
        ShouldReport = false;
    }
    if (ShouldReport) {
      if (First)
        Str << " \t# END=";
      else
        Str << ",";
      Var->emit(Func);
      First = false;
    }
  }
}

void updateStats(Cfg *Func, const Inst *I) {
  if (!BuildDefs::dump())
    return;
  // Update emitted instruction count, plus fill/spill count for Variable
  // operands without a physical register.
  if (uint32_t Count = I->getEmitInstCount()) {
    Func->getContext()->statsUpdateEmitted(Count);
    if (Variable *Dest = I->getDest()) {
      if (!Dest->hasReg())
        Func->getContext()->statsUpdateFills();
    }
    for (SizeT S = 0; S < I->getSrcSize(); ++S) {
      if (Variable *Src = llvm::dyn_cast<Variable>(I->getSrc(S))) {
        if (!Src->hasReg())
          Func->getContext()->statsUpdateSpills();
      }
    }
  }
}

} // end of anonymous namespace

void CfgNode::emit(Cfg *Func) const {
  if (!BuildDefs::dump())
    return;
  Func->setCurrentNode(this);
  Ostream &Str = Func->getContext()->getStrEmit();
  Liveness *Liveness = Func->getLiveness();
  const bool DecorateAsm =
      Liveness && Func->getContext()->getFlags().getDecorateAsm();
  Str << getAsmName() << ":\n";
  // LiveRegCount keeps track of the number of currently live variables that
  // each register is assigned to. Normally that would be only 0 or 1, but the
  // register allocator's AllowOverlap inference allows it to be greater than 1
  // for short periods.
  CfgVector<SizeT> LiveRegCount(Func->getTarget()->getNumRegisters());
  if (DecorateAsm) {
    constexpr bool IsLiveIn = true;
    emitRegisterUsage(Str, Func, this, IsLiveIn, LiveRegCount);
    if (getInEdges().size()) {
      Str << "\t\t\t\t# preds=";
      bool First = true;
      for (CfgNode *I : getInEdges()) {
        if (!First)
          Str << ",";
        First = false;
        Str << I->getAsmName();
      }
      Str << "\n";
    }
    if (getLoopNestDepth()) {
      Str << "\t\t\t\t# loop depth=" << getLoopNestDepth() << "\n";
    }
  }

  for (const Inst &I : Phis) {
    if (I.isDeleted())
      continue;
    // Emitting a Phi instruction should cause an error.
    I.emit(Func);
  }
  for (const Inst &I : Insts) {
    if (I.isDeleted())
      continue;
    if (I.isRedundantAssign()) {
      // Usually, redundant assignments end the live range of the src variable
      // and begin the live range of the dest variable, with no net effect on
      // the liveness of their register. However, if the register allocator
      // infers the AllowOverlap condition, then this may be a redundant
      // assignment that does not end the src variable's live range, in which
      // case the active variable count for that register needs to be bumped.
      // That normally would have happened as part of emitLiveRangesEnded(),
      // but that isn't called for redundant assignments.
      Variable *Dest = I.getDest();
      if (DecorateAsm && Dest->hasReg() && !I.isLastUse(I.getSrc(0)))
        ++LiveRegCount[Dest->getRegNum()];
      continue;
    }
    I.emit(Func);
    if (DecorateAsm)
      emitLiveRangesEnded(Str, Func, &I, LiveRegCount);
    Str << "\n";
    updateStats(Func, &I);
  }
  if (DecorateAsm) {
    constexpr bool IsLiveIn = false;
    emitRegisterUsage(Str, Func, this, IsLiveIn, LiveRegCount);
  }
}

// Helper class for emitIAS().
namespace {
class BundleEmitHelper {
  BundleEmitHelper() = delete;
  BundleEmitHelper(const BundleEmitHelper &) = delete;
  BundleEmitHelper &operator=(const BundleEmitHelper &) = delete;

public:
  BundleEmitHelper(Assembler *Asm, TargetLowering *Target,
                   const InstList &Insts)
      : Asm(Asm), Target(Target), End(Insts.end()), BundleLockStart(End),
        BundleSize(1 << Asm->getBundleAlignLog2Bytes()),
        BundleMaskLo(BundleSize - 1), BundleMaskHi(~BundleMaskLo) {}
  // Check whether we're currently within a bundle_lock region.
  bool isInBundleLockRegion() const { return BundleLockStart != End; }
  // Check whether the current bundle_lock region has the align_to_end option.
  bool isAlignToEnd() const {
    assert(isInBundleLockRegion());
    return llvm::cast<InstBundleLock>(getBundleLockStart())->getOption() ==
           InstBundleLock::Opt_AlignToEnd;
  }
  // Check whether the entire bundle_lock region falls within the same bundle.
  bool isSameBundle() const {
    assert(isInBundleLockRegion());
    return SizeSnapshotPre == SizeSnapshotPost ||
           (SizeSnapshotPre & BundleMaskHi) ==
               ((SizeSnapshotPost - 1) & BundleMaskHi);
  }
  // Get the bundle alignment of the first instruction of the bundle_lock
  // region.
  intptr_t getPreAlignment() const {
    assert(isInBundleLockRegion());
    return SizeSnapshotPre & BundleMaskLo;
  }
  // Get the bundle alignment of the first instruction past the bundle_lock
  // region.
  intptr_t getPostAlignment() const {
    assert(isInBundleLockRegion());
    return SizeSnapshotPost & BundleMaskLo;
  }
  // Get the iterator pointing to the bundle_lock instruction, e.g. to roll
  // back the instruction iteration to that point.
  InstList::const_iterator getBundleLockStart() const {
    assert(isInBundleLockRegion());
    return BundleLockStart;
  }
  // Set up bookkeeping when the bundle_lock instruction is first processed.
  void enterBundleLock(InstList::const_iterator I) {
    assert(!isInBundleLockRegion());
    BundleLockStart = I;
    SizeSnapshotPre = Asm->getBufferSize();
    Asm->setPreliminary(true);
    Target->snapshotEmitState();
    assert(isInBundleLockRegion());
  }
  // Update bookkeeping when the bundle_unlock instruction is processed.
  void enterBundleUnlock() {
    assert(isInBundleLockRegion());
    SizeSnapshotPost = Asm->getBufferSize();
  }
  // Update bookkeeping when we are completely finished with the bundle_lock
  // region.
  void leaveBundleLockRegion() { BundleLockStart = End; }
  // Check whether the instruction sequence fits within the current bundle, and
  // if not, add nop padding to the end of the current bundle.
  void padToNextBundle() {
    assert(isInBundleLockRegion());
    if (!isSameBundle()) {
      intptr_t PadToNextBundle = BundleSize - getPreAlignment();
      Asm->padWithNop(PadToNextBundle);
      SizeSnapshotPre += PadToNextBundle;
      SizeSnapshotPost += PadToNextBundle;
      assert((Asm->getBufferSize() & BundleMaskLo) == 0);
      assert(Asm->getBufferSize() == SizeSnapshotPre);
    }
  }
  // If align_to_end is specified, add padding such that the instruction
  // sequences ends precisely at a bundle boundary.
  void padForAlignToEnd() {
    assert(isInBundleLockRegion());
    if (isAlignToEnd()) {
      if (intptr_t Offset = getPostAlignment()) {
        Asm->padWithNop(BundleSize - Offset);
        SizeSnapshotPre = Asm->getBufferSize();
      }
    }
  }
  // Update bookkeeping when rolling back for the second pass.
  void rollback() {
    assert(isInBundleLockRegion());
    Asm->setBufferSize(SizeSnapshotPre);
    Asm->setPreliminary(false);
    Target->rollbackEmitState();
  }

private:
  Assembler *const Asm;
  TargetLowering *const Target;
  // End is a sentinel value such that BundleLockStart==End implies that we are
  // not in a bundle_lock region.
  const InstList::const_iterator End;
  InstList::const_iterator BundleLockStart;
  const intptr_t BundleSize;
  // Masking with BundleMaskLo identifies an address's bundle offset.
  const intptr_t BundleMaskLo;
  // Masking with BundleMaskHi identifies an address's bundle.
  const intptr_t BundleMaskHi;
  intptr_t SizeSnapshotPre = 0;
  intptr_t SizeSnapshotPost = 0;
};

} // end of anonymous namespace

void CfgNode::emitIAS(Cfg *Func) const {
  Func->setCurrentNode(this);
  Assembler *Asm = Func->getAssembler<>();
  // TODO(stichnot): When sandboxing, defer binding the node label until just
  // before the first instruction is emitted, to reduce the chance that a
  // padding nop is a branch target.
  Asm->bindCfgNodeLabel(getIndex());
  for (const Inst &I : Phis) {
    if (I.isDeleted())
      continue;
    // Emitting a Phi instruction should cause an error.
    I.emitIAS(Func);
  }

  // Do the simple emission if not sandboxed.
  if (!Func->getContext()->getFlags().getUseSandboxing()) {
    for (const Inst &I : Insts) {
      if (!I.isDeleted() && !I.isRedundantAssign()) {
        I.emitIAS(Func);
        updateStats(Func, &I);
      }
    }
    return;
  }

  // The remainder of the function handles emission with sandboxing. There are
  // explicit bundle_lock regions delimited by bundle_lock and bundle_unlock
  // instructions. All other instructions are treated as an implicit
  // one-instruction bundle_lock region. Emission is done twice for each
  // bundle_lock region. The first pass is a preliminary pass, after which we
  // can figure out what nop padding is needed, then roll back, and make the
  // final pass.
  //
  // Ideally, the first pass would be speculative and the second pass would
  // only be done if nop padding were needed, but the structure of the
  // integrated assembler makes it hard to roll back the state of label
  // bindings, label links, and relocation fixups. Instead, the first pass just
  // disables all mutation of that state.

  BundleEmitHelper Helper(Asm, Func->getTarget(), Insts);
  InstList::const_iterator End = Insts.end();
  // Retrying indicates that we had to roll back to the bundle_lock instruction
  // to apply padding before the bundle_lock sequence.
  bool Retrying = false;
  for (InstList::const_iterator I = Insts.begin(); I != End; ++I) {
    if (I->isDeleted() || I->isRedundantAssign())
      continue;

    if (llvm::isa<InstBundleLock>(I)) {
      // Set up the initial bundle_lock state. This should not happen while
      // retrying, because the retry rolls back to the instruction following
      // the bundle_lock instruction.
      assert(!Retrying);
      Helper.enterBundleLock(I);
      continue;
    }

    if (llvm::isa<InstBundleUnlock>(I)) {
      Helper.enterBundleUnlock();
      if (Retrying) {
        // Make sure all instructions are in the same bundle.
        assert(Helper.isSameBundle());
        // If align_to_end is specified, make sure the next instruction begins
        // the bundle.
        assert(!Helper.isAlignToEnd() || Helper.getPostAlignment() == 0);
        Helper.leaveBundleLockRegion();
        Retrying = false;
      } else {
        // This is the first pass, so roll back for the retry pass.
        Helper.rollback();
        // Pad to the next bundle if the instruction sequence crossed a bundle
        // boundary.
        Helper.padToNextBundle();
        // Insert additional padding to make AlignToEnd work.
        Helper.padForAlignToEnd();
        // Prepare for the retry pass after padding is done.
        Retrying = true;
        I = Helper.getBundleLockStart();
      }
      continue;
    }

    // I points to a non bundle_lock/bundle_unlock instruction.
    if (Helper.isInBundleLockRegion()) {
      I->emitIAS(Func);
      // Only update stats during the final pass.
      if (Retrying)
        updateStats(Func, I);
    } else {
      // Treat it as though there were an implicit bundle_lock and
      // bundle_unlock wrapping the instruction.
      Helper.enterBundleLock(I);
      I->emitIAS(Func);
      Helper.enterBundleUnlock();
      Helper.rollback();
      Helper.padToNextBundle();
      I->emitIAS(Func);
      updateStats(Func, I);
      Helper.leaveBundleLockRegion();
    }
  }

  // Don't allow bundle locking across basic blocks, to keep the backtracking
  // mechanism simple.
  assert(!Helper.isInBundleLockRegion());
  assert(!Retrying);
}

void CfgNode::dump(Cfg *Func) const {
  if (!BuildDefs::dump())
    return;
  Func->setCurrentNode(this);
  Ostream &Str = Func->getContext()->getStrDump();
  Liveness *Liveness = Func->getLiveness();
  if (Func->isVerbose(IceV_Instructions) || Func->isVerbose(IceV_Loop))
    Str << getName() << ":\n";
  // Dump the loop nest depth
  if (Func->isVerbose(IceV_Loop))
    Str << "    // LoopNestDepth = " << getLoopNestDepth() << "\n";
  // Dump list of predecessor nodes.
  if (Func->isVerbose(IceV_Preds) && !InEdges.empty()) {
    Str << "    // preds = ";
    bool First = true;
    for (CfgNode *I : InEdges) {
      if (!First)
        Str << ", ";
      First = false;
      Str << "%" << I->getName();
    }
    Str << "\n";
  }
  // Dump the live-in variables.
  LivenessBV LiveIn;
  if (Liveness)
    LiveIn = Liveness->getLiveIn(this);
  if (Func->isVerbose(IceV_Liveness) && !LiveIn.empty()) {
    Str << "    // LiveIn:";
    for (SizeT i = 0; i < LiveIn.size(); ++i) {
      if (LiveIn[i]) {
        Variable *Var = Liveness->getVariable(i, this);
        Str << " %" << Var->getName(Func);
        if (Func->isVerbose(IceV_RegOrigins) && Var->hasReg()) {
          Str << ":"
              << Func->getTarget()->getRegName(Var->getRegNum(),
                                               Var->getType());
        }
      }
    }
    Str << "\n";
  }
  // Dump each instruction.
  if (Func->isVerbose(IceV_Instructions)) {
    for (const Inst &I : Phis)
      I.dumpDecorated(Func);
    for (const Inst &I : Insts)
      I.dumpDecorated(Func);
  }
  // Dump the live-out variables.
  LivenessBV LiveOut;
  if (Liveness)
    LiveOut = Liveness->getLiveOut(this);
  if (Func->isVerbose(IceV_Liveness) && !LiveOut.empty()) {
    Str << "    // LiveOut:";
    for (SizeT i = 0; i < LiveOut.size(); ++i) {
      if (LiveOut[i]) {
        Variable *Var = Liveness->getVariable(i, this);
        Str << " %" << Var->getName(Func);
        if (Func->isVerbose(IceV_RegOrigins) && Var->hasReg()) {
          Str << ":"
              << Func->getTarget()->getRegName(Var->getRegNum(),
                                               Var->getType());
        }
      }
    }
    Str << "\n";
  }
  // Dump list of successor nodes.
  if (Func->isVerbose(IceV_Succs)) {
    Str << "    // succs = ";
    bool First = true;
    for (CfgNode *I : OutEdges) {
      if (!First)
        Str << ", ";
      First = false;
      Str << "%" << I->getName();
    }
    Str << "\n";
  }
}

void CfgNode::profileExecutionCount(VariableDeclaration *Var) {
  constexpr char RMW_I64[] = "llvm.nacl.atomic.rmw.i64";

  GlobalContext *Context = Func->getContext();

  bool BadIntrinsic = false;
  const Intrinsics::FullIntrinsicInfo *Info =
      Context->getIntrinsicsInfo().find(RMW_I64, BadIntrinsic);
  assert(!BadIntrinsic);
  assert(Info != nullptr);

  Operand *RMWI64Name = Context->getConstantExternSym(RMW_I64);
  constexpr RelocOffsetT Offset = 0;
  constexpr bool SuppressMangling = true;
  Constant *Counter =
      Context->getConstantSym(Offset, Var->getName(), SuppressMangling);
  Constant *AtomicRMWOp = Context->getConstantInt32(Intrinsics::AtomicAdd);
  Constant *One = Context->getConstantInt64(1);
  Constant *OrderAcquireRelease =
      Context->getConstantInt32(Intrinsics::MemoryOrderAcquireRelease);

  InstIntrinsicCall *Inst = InstIntrinsicCall::create(
      Func, 5, Func->makeVariable(IceType_i64), RMWI64Name, Info->Info);
  Inst->addArg(AtomicRMWOp);
  Inst->addArg(Counter);
  Inst->addArg(One);
  Inst->addArg(OrderAcquireRelease);
  Insts.push_front(Inst);
}

} // end of namespace Ice
