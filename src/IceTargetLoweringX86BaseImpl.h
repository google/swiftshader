//===- subzero/src/IceTargetLoweringX86BaseImpl.h - x86 lowering -*- C++ -*-==//
//
//                        The Subzero Code Generator
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements the TargetLoweringX86Base class, which
// consists almost entirely of the lowering sequence for each
// high-level instruction.
//
//===----------------------------------------------------------------------===//

#ifndef SUBZERO_SRC_ICETARGETLOWERINGX86BASEIMPL_H
#define SUBZERO_SRC_ICETARGETLOWERINGX86BASEIMPL_H

#include "IceCfg.h"
#include "IceCfgNode.h"
#include "IceClFlags.h"
#include "IceDefs.h"
#include "IceELFObjectWriter.h"
#include "IceGlobalInits.h"
#include "IceInstX8632.h"
#include "IceLiveness.h"
#include "IceOperand.h"
#include "IceRegistersX8632.h"
#include "IceTargetLoweringX8632.def"
#include "IceTargetLoweringX8632.h"
#include "IceUtils.h"
#include "llvm/Support/MathExtras.h"

namespace Ice {
namespace X86Internal {

// A helper class to ease the settings of RandomizationPoolingPause
// to disable constant blinding or pooling for some translation phases.
class BoolFlagSaver {
  BoolFlagSaver() = delete;
  BoolFlagSaver(const BoolFlagSaver &) = delete;
  BoolFlagSaver &operator=(const BoolFlagSaver &) = delete;

public:
  BoolFlagSaver(bool &F, bool NewValue) : OldValue(F), Flag(F) { F = NewValue; }
  ~BoolFlagSaver() { Flag = OldValue; }

private:
  const bool OldValue;
  bool &Flag;
};

template <class MachineTraits> class BoolFoldingEntry {
  BoolFoldingEntry(const BoolFoldingEntry &) = delete;

public:
  BoolFoldingEntry() = default;
  explicit BoolFoldingEntry(Inst *I);
  BoolFoldingEntry &operator=(const BoolFoldingEntry &) = default;
  // Instr is the instruction producing the i1-type variable of interest.
  Inst *Instr = nullptr;
  // IsComplex is the cached result of BoolFolding::hasComplexLowering(Instr).
  bool IsComplex = false;
  // IsLiveOut is initialized conservatively to true, and is set to false when
  // we encounter an instruction that ends Var's live range.  We disable the
  // folding optimization when Var is live beyond this basic block.  Note that
  // if liveness analysis is not performed (e.g. in Om1 mode), IsLiveOut will
  // always be true and the folding optimization will never be performed.
  bool IsLiveOut = true;
  // NumUses counts the number of times Var is used as a source operand in the
  // basic block.  If IsComplex is true and there is more than one use of Var,
  // then the folding optimization is disabled for Var.
  uint32_t NumUses = 0;
};

template <class MachineTraits> class BoolFolding {
public:
  enum BoolFoldingProducerKind {
    PK_None,
    PK_Icmp32,
    PK_Icmp64,
    PK_Fcmp,
    PK_Trunc
  };

  // Currently the actual enum values are not used (other than CK_None), but we
  // go
  // ahead and produce them anyway for symmetry with the
  // BoolFoldingProducerKind.
  enum BoolFoldingConsumerKind { CK_None, CK_Br, CK_Select, CK_Sext, CK_Zext };

private:
  BoolFolding(const BoolFolding &) = delete;
  BoolFolding &operator=(const BoolFolding &) = delete;

public:
  BoolFolding() = default;
  static BoolFoldingProducerKind getProducerKind(const Inst *Instr);
  static BoolFoldingConsumerKind getConsumerKind(const Inst *Instr);
  static bool hasComplexLowering(const Inst *Instr);
  void init(CfgNode *Node);
  const Inst *getProducerFor(const Operand *Opnd) const;
  void dump(const Cfg *Func) const;

private:
  // Returns true if Producers contains a valid entry for the given VarNum.
  bool containsValid(SizeT VarNum) const {
    auto Element = Producers.find(VarNum);
    return Element != Producers.end() && Element->second.Instr != nullptr;
  }
  void setInvalid(SizeT VarNum) { Producers[VarNum].Instr = nullptr; }
  // Producers maps Variable::Number to a BoolFoldingEntry.
  std::unordered_map<SizeT, BoolFoldingEntry<MachineTraits>> Producers;
};

template <class MachineTraits>
BoolFoldingEntry<MachineTraits>::BoolFoldingEntry(Inst *I)
    : Instr(I), IsComplex(BoolFolding<MachineTraits>::hasComplexLowering(I)) {}

template <class MachineTraits>
typename BoolFolding<MachineTraits>::BoolFoldingProducerKind
BoolFolding<MachineTraits>::getProducerKind(const Inst *Instr) {
  if (llvm::isa<InstIcmp>(Instr)) {
    if (Instr->getSrc(0)->getType() != IceType_i64)
      return PK_Icmp32;
    return PK_None; // TODO(stichnot): actually PK_Icmp64;
  }
  return PK_None; // TODO(stichnot): remove this

  if (llvm::isa<InstFcmp>(Instr))
    return PK_Fcmp;
  if (auto *Cast = llvm::dyn_cast<InstCast>(Instr)) {
    switch (Cast->getCastKind()) {
    default:
      return PK_None;
    case InstCast::Trunc:
      return PK_Trunc;
    }
  }
  return PK_None;
}

template <class MachineTraits>
typename BoolFolding<MachineTraits>::BoolFoldingConsumerKind
BoolFolding<MachineTraits>::getConsumerKind(const Inst *Instr) {
  if (llvm::isa<InstBr>(Instr))
    return CK_Br;
  if (llvm::isa<InstSelect>(Instr))
    return CK_Select;
  return CK_None; // TODO(stichnot): remove this

  if (auto *Cast = llvm::dyn_cast<InstCast>(Instr)) {
    switch (Cast->getCastKind()) {
    default:
      return CK_None;
    case InstCast::Sext:
      return CK_Sext;
    case InstCast::Zext:
      return CK_Zext;
    }
  }
  return CK_None;
}

// Returns true if the producing instruction has a "complex" lowering
// sequence.  This generally means that its lowering sequence requires
// more than one conditional branch, namely 64-bit integer compares
// and some floating-point compares.  When this is true, and there is
// more than one consumer, we prefer to disable the folding
// optimization because it minimizes branches.
template <class MachineTraits>
bool BoolFolding<MachineTraits>::hasComplexLowering(const Inst *Instr) {
  switch (getProducerKind(Instr)) {
  default:
    return false;
  case PK_Icmp64:
    return true;
  case PK_Fcmp:
    return MachineTraits::TableFcmp[llvm::cast<InstFcmp>(Instr)->getCondition()]
               .C2 != CondX86::Br_None;
  }
}

template <class MachineTraits>
void BoolFolding<MachineTraits>::init(CfgNode *Node) {
  Producers.clear();
  for (Inst &Instr : Node->getInsts()) {
    // Check whether Instr is a valid producer.
    Variable *Var = Instr.getDest();
    if (!Instr.isDeleted() // only consider non-deleted instructions
        && Var             // only instructions with an actual dest var
        && Var->getType() == IceType_i1          // only bool-type dest vars
        && getProducerKind(&Instr) != PK_None) { // white-listed instructions
      Producers[Var->getIndex()] = BoolFoldingEntry<MachineTraits>(&Instr);
    }
    // Check each src variable against the map.
    for (SizeT I = 0; I < Instr.getSrcSize(); ++I) {
      Operand *Src = Instr.getSrc(I);
      SizeT NumVars = Src->getNumVars();
      for (SizeT J = 0; J < NumVars; ++J) {
        const Variable *Var = Src->getVar(J);
        SizeT VarNum = Var->getIndex();
        if (containsValid(VarNum)) {
          if (I != 0 // All valid consumers use Var as the first source operand
              || getConsumerKind(&Instr) == CK_None // must be white-listed
              || (Producers[VarNum].IsComplex && // complex can't be multi-use
                  Producers[VarNum].NumUses > 0)) {
            setInvalid(VarNum);
            continue;
          }
          ++Producers[VarNum].NumUses;
          if (Instr.isLastUse(Var)) {
            Producers[VarNum].IsLiveOut = false;
          }
        }
      }
    }
  }
  for (auto &I : Producers) {
    // Ignore entries previously marked invalid.
    if (I.second.Instr == nullptr)
      continue;
    // Disable the producer if its dest may be live beyond this block.
    if (I.second.IsLiveOut) {
      setInvalid(I.first);
      continue;
    }
    // Mark as "dead" rather than outright deleting.  This is so that
    // other peephole style optimizations during or before lowering
    // have access to this instruction in undeleted form.  See for
    // example tryOptimizedCmpxchgCmpBr().
    I.second.Instr->setDead();
  }
}

template <class MachineTraits>
const Inst *
BoolFolding<MachineTraits>::getProducerFor(const Operand *Opnd) const {
  auto *Var = llvm::dyn_cast<const Variable>(Opnd);
  if (Var == nullptr)
    return nullptr;
  SizeT VarNum = Var->getIndex();
  auto Element = Producers.find(VarNum);
  if (Element == Producers.end())
    return nullptr;
  return Element->second.Instr;
}

template <class MachineTraits>
void BoolFolding<MachineTraits>::dump(const Cfg *Func) const {
  if (!BuildDefs::dump() || !Func->isVerbose(IceV_Folding))
    return;
  OstreamLocker L(Func->getContext());
  Ostream &Str = Func->getContext()->getStrDump();
  for (auto &I : Producers) {
    if (I.second.Instr == nullptr)
      continue;
    Str << "Found foldable producer:\n  ";
    I.second.Instr->dump(Func);
    Str << "\n";
  }
}

template <class Machine>
void TargetX86Base<Machine>::initNodeForLowering(CfgNode *Node) {
  FoldingInfo.init(Node);
  FoldingInfo.dump(Func);
}

template <class Machine>
TargetX86Base<Machine>::TargetX86Base(Cfg *Func)
    : Machine(Func) {
  static_assert(
      (Traits::InstructionSet::End - Traits::InstructionSet::Begin) ==
          (TargetInstructionSet::X86InstructionSet_End -
           TargetInstructionSet::X86InstructionSet_Begin),
      "Traits::InstructionSet range different from TargetInstructionSet");
  if (Func->getContext()->getFlags().getTargetInstructionSet() !=
      TargetInstructionSet::BaseInstructionSet) {
    InstructionSet = static_cast<typename Traits::InstructionSet>(
        (Func->getContext()->getFlags().getTargetInstructionSet() -
         TargetInstructionSet::X86InstructionSet_Begin) +
        Traits::InstructionSet::Begin);
  }
  // TODO: Don't initialize IntegerRegisters and friends every time.
  // Instead, initialize in some sort of static initializer for the
  // class.
  llvm::SmallBitVector IntegerRegisters(RegX8632::Reg_NUM);
  llvm::SmallBitVector IntegerRegistersI8(RegX8632::Reg_NUM);
  llvm::SmallBitVector FloatRegisters(RegX8632::Reg_NUM);
  llvm::SmallBitVector VectorRegisters(RegX8632::Reg_NUM);
  llvm::SmallBitVector InvalidRegisters(RegX8632::Reg_NUM);
  ScratchRegs.resize(RegX8632::Reg_NUM);
#define X(val, encode, name, name16, name8, scratch, preserved, stackptr,      \
          frameptr, isI8, isInt, isFP)                                         \
  IntegerRegisters[RegX8632::val] = isInt;                                     \
  IntegerRegistersI8[RegX8632::val] = isI8;                                    \
  FloatRegisters[RegX8632::val] = isFP;                                        \
  VectorRegisters[RegX8632::val] = isFP;                                       \
  ScratchRegs[RegX8632::val] = scratch;
  REGX8632_TABLE;
#undef X
  TypeToRegisterSet[IceType_void] = InvalidRegisters;
  TypeToRegisterSet[IceType_i1] = IntegerRegistersI8;
  TypeToRegisterSet[IceType_i8] = IntegerRegistersI8;
  TypeToRegisterSet[IceType_i16] = IntegerRegisters;
  TypeToRegisterSet[IceType_i32] = IntegerRegisters;
  TypeToRegisterSet[IceType_i64] = IntegerRegisters;
  TypeToRegisterSet[IceType_f32] = FloatRegisters;
  TypeToRegisterSet[IceType_f64] = FloatRegisters;
  TypeToRegisterSet[IceType_v4i1] = VectorRegisters;
  TypeToRegisterSet[IceType_v8i1] = VectorRegisters;
  TypeToRegisterSet[IceType_v16i1] = VectorRegisters;
  TypeToRegisterSet[IceType_v16i8] = VectorRegisters;
  TypeToRegisterSet[IceType_v8i16] = VectorRegisters;
  TypeToRegisterSet[IceType_v4i32] = VectorRegisters;
  TypeToRegisterSet[IceType_v4f32] = VectorRegisters;
}

template <class Machine> void TargetX86Base<Machine>::translateO2() {
  TimerMarker T(TimerStack::TT_O2, Func);

  if (!Ctx->getFlags().getPhiEdgeSplit()) {
    // Lower Phi instructions.
    Func->placePhiLoads();
    if (Func->hasError())
      return;
    Func->placePhiStores();
    if (Func->hasError())
      return;
    Func->deletePhis();
    if (Func->hasError())
      return;
    Func->dump("After Phi lowering");
  }

  // Address mode optimization.
  Func->getVMetadata()->init(VMK_SingleDefs);
  Func->doAddressOpt();

  // Find read-modify-write opportunities.  Do this after address mode
  // optimization so that doAddressOpt() doesn't need to be applied to RMW
  // instructions as well.
  findRMW();
  Func->dump("After RMW transform");

  // Argument lowering
  Func->doArgLowering();

  // Target lowering.  This requires liveness analysis for some parts
  // of the lowering decisions, such as compare/branch fusing.  If
  // non-lightweight liveness analysis is used, the instructions need
  // to be renumbered first.  TODO: This renumbering should only be
  // necessary if we're actually calculating live intervals, which we
  // only do for register allocation.
  Func->renumberInstructions();
  if (Func->hasError())
    return;

  // TODO: It should be sufficient to use the fastest liveness
  // calculation, i.e. livenessLightweight().  However, for some
  // reason that slows down the rest of the translation.  Investigate.
  Func->liveness(Liveness_Basic);
  if (Func->hasError())
    return;
  Func->dump("After x86 address mode opt");

  // Disable constant blinding or pooling for load optimization.
  {
    BoolFlagSaver B(RandomizationPoolingPaused, true);
    doLoadOpt();
  }
  Func->genCode();
  if (Func->hasError())
    return;
  Func->dump("After x86 codegen");

  // Register allocation.  This requires instruction renumbering and
  // full liveness analysis.
  Func->renumberInstructions();
  if (Func->hasError())
    return;
  Func->liveness(Liveness_Intervals);
  if (Func->hasError())
    return;
  // Validate the live range computations.  The expensive validation
  // call is deliberately only made when assertions are enabled.
  assert(Func->validateLiveness());
  // The post-codegen dump is done here, after liveness analysis and
  // associated cleanup, to make the dump cleaner and more useful.
  Func->dump("After initial x8632 codegen");
  Func->getVMetadata()->init(VMK_All);
  regAlloc(RAK_Global);
  if (Func->hasError())
    return;
  Func->dump("After linear scan regalloc");

  if (Ctx->getFlags().getPhiEdgeSplit()) {
    // We need to pause constant blinding or pooling during advanced
    // phi lowering, unless the lowering assignment has a physical
    // register for the dest Variable.
    {
      BoolFlagSaver B(RandomizationPoolingPaused, true);
      Func->advancedPhiLowering();
    }
    Func->dump("After advanced Phi lowering");
  }

  // Stack frame mapping.
  Func->genFrame();
  if (Func->hasError())
    return;
  Func->dump("After stack frame mapping");

  Func->contractEmptyNodes();
  Func->reorderNodes();

  // Branch optimization.  This needs to be done just before code
  // emission.  In particular, no transformations that insert or
  // reorder CfgNodes should be done after branch optimization.  We go
  // ahead and do it before nop insertion to reduce the amount of work
  // needed for searching for opportunities.
  Func->doBranchOpt();
  Func->dump("After branch optimization");

  // Nop insertion
  if (Ctx->getFlags().shouldDoNopInsertion()) {
    Func->doNopInsertion();
  }
}

template <class Machine> void TargetX86Base<Machine>::translateOm1() {
  TimerMarker T(TimerStack::TT_Om1, Func);

  Func->placePhiLoads();
  if (Func->hasError())
    return;
  Func->placePhiStores();
  if (Func->hasError())
    return;
  Func->deletePhis();
  if (Func->hasError())
    return;
  Func->dump("After Phi lowering");

  Func->doArgLowering();

  Func->genCode();
  if (Func->hasError())
    return;
  Func->dump("After initial x8632 codegen");

  regAlloc(RAK_InfOnly);
  if (Func->hasError())
    return;
  Func->dump("After regalloc of infinite-weight variables");

  Func->genFrame();
  if (Func->hasError())
    return;
  Func->dump("After stack frame mapping");

  // Nop insertion
  if (Ctx->getFlags().shouldDoNopInsertion()) {
    Func->doNopInsertion();
  }
}

bool canRMW(const InstArithmetic *Arith) {
  Type Ty = Arith->getDest()->getType();
  // X86 vector instructions write to a register and have no RMW
  // option.
  if (isVectorType(Ty))
    return false;
  bool isI64 = Ty == IceType_i64;

  switch (Arith->getOp()) {
  // Not handled for lack of simple lowering:
  //   shift on i64
  //   mul, udiv, urem, sdiv, srem, frem
  // Not handled for lack of RMW instructions:
  //   fadd, fsub, fmul, fdiv (also vector types)
  default:
    return false;
  case InstArithmetic::Add:
  case InstArithmetic::Sub:
  case InstArithmetic::And:
  case InstArithmetic::Or:
  case InstArithmetic::Xor:
    return true;
  case InstArithmetic::Shl:
  case InstArithmetic::Lshr:
  case InstArithmetic::Ashr:
    return false; // TODO(stichnot): implement
    return !isI64;
  }
}

bool isSameMemAddressOperand(const Operand *A, const Operand *B) {
  if (A == B)
    return true;
  if (auto *MemA = llvm::dyn_cast<OperandX8632Mem>(A)) {
    if (auto *MemB = llvm::dyn_cast<OperandX8632Mem>(B)) {
      return MemA->getBase() == MemB->getBase() &&
             MemA->getOffset() == MemB->getOffset() &&
             MemA->getIndex() == MemB->getIndex() &&
             MemA->getShift() == MemB->getShift() &&
             MemA->getSegmentRegister() == MemB->getSegmentRegister();
    }
  }
  return false;
}

template <class Machine> void TargetX86Base<Machine>::findRMW() {
  Func->dump("Before RMW");
  OstreamLocker L(Func->getContext());
  Ostream &Str = Func->getContext()->getStrDump();
  for (CfgNode *Node : Func->getNodes()) {
    // Walk through the instructions, considering each sequence of 3
    // instructions, and look for the particular RMW pattern.  Note that this
    // search can be "broken" (false negatives) if there are intervening deleted
    // instructions, or intervening instructions that could be safely moved out
    // of the way to reveal an RMW pattern.
    auto E = Node->getInsts().end();
    auto I1 = E, I2 = E, I3 = Node->getInsts().begin();
    for (; I3 != E; I1 = I2, I2 = I3, ++I3) {
      // Make I3 skip over deleted instructions.
      while (I3 != E && I3->isDeleted())
        ++I3;
      if (I1 == E || I2 == E || I3 == E)
        continue;
      assert(!I1->isDeleted());
      assert(!I2->isDeleted());
      assert(!I3->isDeleted());
      if (auto *Load = llvm::dyn_cast<InstLoad>(I1)) {
        if (auto *Arith = llvm::dyn_cast<InstArithmetic>(I2)) {
          if (auto *Store = llvm::dyn_cast<InstStore>(I3)) {
            // Look for:
            //   a = Load addr
            //   b = <op> a, other
            //   Store b, addr
            // Change to:
            //   a = Load addr
            //   b = <op> a, other
            //   x = FakeDef
            //   RMW <op>, addr, other, x
            //   b = Store b, addr, x
            // Note that inferTwoAddress() makes sure setDestNonKillable() gets
            // called on the updated Store instruction, to avoid liveness
            // problems later.
            //
            // With this transformation, the Store instruction acquires a Dest
            // variable and is now subject to dead code elimination if there are
            // no more uses of "b".  Variable "x" is a beacon for determining
            // whether the Store instruction gets dead-code eliminated.  If the
            // Store instruction is eliminated, then it must be the case that
            // the RMW instruction ends x's live range, and therefore the RMW
            // instruction will be retained and later lowered.  On the other
            // hand, if the RMW instruction does not end x's live range, then
            // the Store instruction must still be present, and therefore the
            // RMW instruction is ignored during lowering because it is
            // redundant with the Store instruction.
            //
            // Note that if "a" has further uses, the RMW transformation may
            // still trigger, resulting in two loads and one store, which is
            // worse than the original one load and one store.  However, this is
            // probably rare, and caching probably keeps it just as fast.
            if (!isSameMemAddressOperand(Load->getSourceAddress(),
                                         Store->getAddr()))
              continue;
            Operand *ArithSrcFromLoad = Arith->getSrc(0);
            Operand *ArithSrcOther = Arith->getSrc(1);
            if (ArithSrcFromLoad != Load->getDest()) {
              if (!Arith->isCommutative() || ArithSrcOther != Load->getDest())
                continue;
              std::swap(ArithSrcFromLoad, ArithSrcOther);
            }
            if (Arith->getDest() != Store->getData())
              continue;
            if (!canRMW(Arith))
              continue;
            if (Func->isVerbose(IceV_RMW)) {
              Str << "Found RMW in " << Func->getFunctionName() << ":\n  ";
              Load->dump(Func);
              Str << "\n  ";
              Arith->dump(Func);
              Str << "\n  ";
              Store->dump(Func);
              Str << "\n";
            }
            Variable *Beacon = Func->template makeVariable(IceType_i32);
            Beacon->setWeight(0);
            Store->setRmwBeacon(Beacon);
            InstFakeDef *BeaconDef = InstFakeDef::create(Func, Beacon);
            Node->getInsts().insert(I3, BeaconDef);
            InstX8632FakeRMW *RMW = InstX8632FakeRMW::create(
                Func, ArithSrcOther, Store->getAddr(), Beacon, Arith->getOp());
            Node->getInsts().insert(I3, RMW);
          }
        }
      }
    }
  }
}

// Converts a ConstantInteger32 operand into its constant value, or
// MemoryOrderInvalid if the operand is not a ConstantInteger32.
uint64_t getConstantMemoryOrder(Operand *Opnd) {
  if (auto Integer = llvm::dyn_cast<ConstantInteger32>(Opnd))
    return Integer->getValue();
  return Intrinsics::MemoryOrderInvalid;
}

// Determines whether the dest of a Load instruction can be folded
// into one of the src operands of a 2-operand instruction.  This is
// true as long as the load dest matches exactly one of the binary
// instruction's src operands.  Replaces Src0 or Src1 with LoadSrc if
// the answer is true.
bool canFoldLoadIntoBinaryInst(Operand *LoadSrc, Variable *LoadDest,
                               Operand *&Src0, Operand *&Src1) {
  if (Src0 == LoadDest && Src1 != LoadDest) {
    Src0 = LoadSrc;
    return true;
  }
  if (Src0 != LoadDest && Src1 == LoadDest) {
    Src1 = LoadSrc;
    return true;
  }
  return false;
}

template <class Machine> void TargetX86Base<Machine>::doLoadOpt() {
  for (CfgNode *Node : Func->getNodes()) {
    Context.init(Node);
    while (!Context.atEnd()) {
      Variable *LoadDest = nullptr;
      Operand *LoadSrc = nullptr;
      Inst *CurInst = Context.getCur();
      Inst *Next = Context.getNextInst();
      // Determine whether the current instruction is a Load
      // instruction or equivalent.
      if (auto *Load = llvm::dyn_cast<InstLoad>(CurInst)) {
        // An InstLoad always qualifies.
        LoadDest = Load->getDest();
        const bool DoLegalize = false;
        LoadSrc = formMemoryOperand(Load->getSourceAddress(),
                                    LoadDest->getType(), DoLegalize);
      } else if (auto *Intrin = llvm::dyn_cast<InstIntrinsicCall>(CurInst)) {
        // An AtomicLoad intrinsic qualifies as long as it has a valid
        // memory ordering, and can be implemented in a single
        // instruction (i.e., not i64).
        Intrinsics::IntrinsicID ID = Intrin->getIntrinsicInfo().ID;
        if (ID == Intrinsics::AtomicLoad &&
            Intrin->getDest()->getType() != IceType_i64 &&
            Intrinsics::isMemoryOrderValid(
                ID, getConstantMemoryOrder(Intrin->getArg(1)))) {
          LoadDest = Intrin->getDest();
          const bool DoLegalize = false;
          LoadSrc = formMemoryOperand(Intrin->getArg(0), LoadDest->getType(),
                                      DoLegalize);
        }
      }
      // A Load instruction can be folded into the following
      // instruction only if the following instruction ends the Load's
      // Dest variable's live range.
      if (LoadDest && Next && Next->isLastUse(LoadDest)) {
        assert(LoadSrc);
        Inst *NewInst = nullptr;
        if (auto *Arith = llvm::dyn_cast<InstArithmetic>(Next)) {
          Operand *Src0 = Arith->getSrc(0);
          Operand *Src1 = Arith->getSrc(1);
          if (canFoldLoadIntoBinaryInst(LoadSrc, LoadDest, Src0, Src1)) {
            NewInst = InstArithmetic::create(Func, Arith->getOp(),
                                             Arith->getDest(), Src0, Src1);
          }
        } else if (auto *Icmp = llvm::dyn_cast<InstIcmp>(Next)) {
          Operand *Src0 = Icmp->getSrc(0);
          Operand *Src1 = Icmp->getSrc(1);
          if (canFoldLoadIntoBinaryInst(LoadSrc, LoadDest, Src0, Src1)) {
            NewInst = InstIcmp::create(Func, Icmp->getCondition(),
                                       Icmp->getDest(), Src0, Src1);
          }
        } else if (auto *Fcmp = llvm::dyn_cast<InstFcmp>(Next)) {
          Operand *Src0 = Fcmp->getSrc(0);
          Operand *Src1 = Fcmp->getSrc(1);
          if (canFoldLoadIntoBinaryInst(LoadSrc, LoadDest, Src0, Src1)) {
            NewInst = InstFcmp::create(Func, Fcmp->getCondition(),
                                       Fcmp->getDest(), Src0, Src1);
          }
        } else if (auto *Select = llvm::dyn_cast<InstSelect>(Next)) {
          Operand *Src0 = Select->getTrueOperand();
          Operand *Src1 = Select->getFalseOperand();
          if (canFoldLoadIntoBinaryInst(LoadSrc, LoadDest, Src0, Src1)) {
            NewInst = InstSelect::create(Func, Select->getDest(),
                                         Select->getCondition(), Src0, Src1);
          }
        } else if (auto *Cast = llvm::dyn_cast<InstCast>(Next)) {
          // The load dest can always be folded into a Cast
          // instruction.
          Variable *Src0 = llvm::dyn_cast<Variable>(Cast->getSrc(0));
          if (Src0 == LoadDest) {
            NewInst = InstCast::create(Func, Cast->getCastKind(),
                                       Cast->getDest(), LoadSrc);
          }
        }
        if (NewInst) {
          CurInst->setDeleted();
          Next->setDeleted();
          Context.insert(NewInst);
          // Update NewInst->LiveRangesEnded so that target lowering
          // may benefit.  Also update NewInst->HasSideEffects.
          NewInst->spliceLivenessInfo(Next, CurInst);
        }
      }
      Context.advanceCur();
      Context.advanceNext();
    }
  }
  Func->dump("After load optimization");
}

template <class Machine>
bool TargetX86Base<Machine>::doBranchOpt(Inst *I, const CfgNode *NextNode) {
  if (InstX8632Br *Br = llvm::dyn_cast<InstX8632Br>(I)) {
    return Br->optimizeBranch(NextNode);
  }
  return false;
}

template <class Machine>
IceString TargetX86Base<Machine>::RegNames[] = {
#define X(val, encode, name, name16, name8, scratch, preserved, stackptr,      \
          frameptr, isI8, isInt, isFP)                                         \
  name,
    REGX8632_TABLE
#undef X
};

template <class Machine>
Variable *TargetX86Base<Machine>::getPhysicalRegister(SizeT RegNum, Type Ty) {
  if (Ty == IceType_void)
    Ty = IceType_i32;
  if (PhysicalRegisters[Ty].empty())
    PhysicalRegisters[Ty].resize(RegX8632::Reg_NUM);
  assert(RegNum < PhysicalRegisters[Ty].size());
  Variable *Reg = PhysicalRegisters[Ty][RegNum];
  if (Reg == nullptr) {
    Reg = Func->template makeVariable(Ty);
    Reg->setRegNum(RegNum);
    PhysicalRegisters[Ty][RegNum] = Reg;
    // Specially mark esp as an "argument" so that it is considered
    // live upon function entry.
    if (RegNum == RegX8632::Reg_esp) {
      Func->addImplicitArg(Reg);
      Reg->setIgnoreLiveness();
    }
  }
  return Reg;
}

template <class Machine>
IceString TargetX86Base<Machine>::getRegName(SizeT RegNum, Type Ty) const {
  assert(RegNum < RegX8632::Reg_NUM);
  static IceString RegNames8[] = {
#define X(val, encode, name, name16, name8, scratch, preserved, stackptr,      \
          frameptr, isI8, isInt, isFP)                                         \
  name8,
      REGX8632_TABLE
#undef X
  };
  static IceString RegNames16[] = {
#define X(val, encode, name, name16, name8, scratch, preserved, stackptr,      \
          frameptr, isI8, isInt, isFP)                                         \
  name16,
      REGX8632_TABLE
#undef X
  };
  switch (Ty) {
  case IceType_i1:
  case IceType_i8:
    return RegNames8[RegNum];
  case IceType_i16:
    return RegNames16[RegNum];
  default:
    return RegNames[RegNum];
  }
}

template <class Machine>
void TargetX86Base<Machine>::emitVariable(const Variable *Var) const {
  Ostream &Str = Ctx->getStrEmit();
  if (Var->hasReg()) {
    Str << "%" << getRegName(Var->getRegNum(), Var->getType());
    return;
  }
  if (Var->getWeight().isInf()) {
    llvm_unreachable("Infinite-weight Variable has no register assigned");
  }
  int32_t Offset = Var->getStackOffset();
  if (!hasFramePointer())
    Offset += getStackAdjustment();
  if (Offset)
    Str << Offset;
  const Type FrameSPTy = IceType_i32;
  Str << "(%" << getRegName(getFrameOrStackReg(), FrameSPTy) << ")";
}

template <class Machine>
X8632::Address
TargetX86Base<Machine>::stackVarToAsmOperand(const Variable *Var) const {
  if (Var->hasReg())
    llvm_unreachable("Stack Variable has a register assigned");
  if (Var->getWeight().isInf()) {
    llvm_unreachable("Infinite-weight Variable has no register assigned");
  }
  int32_t Offset = Var->getStackOffset();
  if (!hasFramePointer())
    Offset += getStackAdjustment();
  return X8632::Address(RegX8632::getEncodedGPR(getFrameOrStackReg()), Offset);
}

template <class Machine> void TargetX86Base<Machine>::lowerArguments() {
  VarList &Args = Func->getArgs();
  // The first four arguments of vector type, regardless of their
  // position relative to the other arguments in the argument list, are
  // passed in registers xmm0 - xmm3.
  unsigned NumXmmArgs = 0;

  Context.init(Func->getEntryNode());
  Context.setInsertPoint(Context.getCur());

  for (SizeT I = 0, E = Args.size();
       I < E && NumXmmArgs < Traits::X86_MAX_XMM_ARGS; ++I) {
    Variable *Arg = Args[I];
    Type Ty = Arg->getType();
    if (!isVectorType(Ty))
      continue;
    // Replace Arg in the argument list with the home register.  Then
    // generate an instruction in the prolog to copy the home register
    // to the assigned location of Arg.
    int32_t RegNum = RegX8632::Reg_xmm0 + NumXmmArgs;
    ++NumXmmArgs;
    Variable *RegisterArg = Func->template makeVariable(Ty);
    if (BuildDefs::dump())
      RegisterArg->setName(Func, "home_reg:" + Arg->getName(Func));
    RegisterArg->setRegNum(RegNum);
    RegisterArg->setIsArg();
    Arg->setIsArg(false);

    Args[I] = RegisterArg;
    Context.insert(InstAssign::create(Func, Arg, RegisterArg));
  }
}

// Helper function for addProlog().
//
// This assumes Arg is an argument passed on the stack.  This sets the
// frame offset for Arg and updates InArgsSizeBytes according to Arg's
// width.  For an I64 arg that has been split into Lo and Hi components,
// it calls itself recursively on the components, taking care to handle
// Lo first because of the little-endian architecture.  Lastly, this
// function generates an instruction to copy Arg into its assigned
// register if applicable.
template <class Machine>
void TargetX86Base<Machine>::finishArgumentLowering(Variable *Arg,
                                                    Variable *FramePtr,
                                                    size_t BasicFrameOffset,
                                                    size_t &InArgsSizeBytes) {
  Variable *Lo = Arg->getLo();
  Variable *Hi = Arg->getHi();
  Type Ty = Arg->getType();
  if (Lo && Hi && Ty == IceType_i64) {
    assert(Lo->getType() != IceType_i64); // don't want infinite recursion
    assert(Hi->getType() != IceType_i64); // don't want infinite recursion
    finishArgumentLowering(Lo, FramePtr, BasicFrameOffset, InArgsSizeBytes);
    finishArgumentLowering(Hi, FramePtr, BasicFrameOffset, InArgsSizeBytes);
    return;
  }
  if (isVectorType(Ty)) {
    InArgsSizeBytes = Traits::applyStackAlignment(InArgsSizeBytes);
  }
  Arg->setStackOffset(BasicFrameOffset + InArgsSizeBytes);
  InArgsSizeBytes += typeWidthInBytesOnStack(Ty);
  if (Arg->hasReg()) {
    assert(Ty != IceType_i64);
    OperandX8632Mem *Mem = OperandX8632Mem::create(
        Func, Ty, FramePtr, Ctx->getConstantInt32(Arg->getStackOffset()));
    if (isVectorType(Arg->getType())) {
      _movp(Arg, Mem);
    } else {
      _mov(Arg, Mem);
    }
    // This argument-copying instruction uses an explicit
    // OperandX8632Mem operand instead of a Variable, so its
    // fill-from-stack operation has to be tracked separately for
    // statistics.
    Ctx->statsUpdateFills();
  }
}

template <class Machine> Type TargetX86Base<Machine>::stackSlotType() {
  return IceType_i32;
}

template <class Machine> void TargetX86Base<Machine>::addProlog(CfgNode *Node) {
  // Stack frame layout:
  //
  // +------------------------+
  // | 1. return address      |
  // +------------------------+
  // | 2. preserved registers |
  // +------------------------+
  // | 3. padding             |
  // +------------------------+
  // | 4. global spill area   |
  // +------------------------+
  // | 5. padding             |
  // +------------------------+
  // | 6. local spill area    |
  // +------------------------+
  // | 7. padding             |
  // +------------------------+
  // | 8. allocas             |
  // +------------------------+
  //
  // The following variables record the size in bytes of the given areas:
  //  * X86_RET_IP_SIZE_BYTES:  area 1
  //  * PreservedRegsSizeBytes: area 2
  //  * SpillAreaPaddingBytes:  area 3
  //  * GlobalsSize:            area 4
  //  * GlobalsAndSubsequentPaddingSize: areas 4 - 5
  //  * LocalsSpillAreaSize:    area 6
  //  * SpillAreaSizeBytes:     areas 3 - 7

  // Determine stack frame offsets for each Variable without a
  // register assignment.  This can be done as one variable per stack
  // slot.  Or, do coalescing by running the register allocator again
  // with an infinite set of registers (as a side effect, this gives
  // variables a second chance at physical register assignment).
  //
  // A middle ground approach is to leverage sparsity and allocate one
  // block of space on the frame for globals (variables with
  // multi-block lifetime), and one block to share for locals
  // (single-block lifetime).

  Context.init(Node);
  Context.setInsertPoint(Context.getCur());

  llvm::SmallBitVector CalleeSaves =
      getRegisterSet(RegSet_CalleeSave, RegSet_None);
  RegsUsed = llvm::SmallBitVector(CalleeSaves.size());
  VarList SortedSpilledVariables, VariablesLinkedToSpillSlots;
  size_t GlobalsSize = 0;
  // If there is a separate locals area, this represents that area.
  // Otherwise it counts any variable not counted by GlobalsSize.
  SpillAreaSizeBytes = 0;
  // If there is a separate locals area, this specifies the alignment
  // for it.
  uint32_t LocalsSlotsAlignmentBytes = 0;
  // The entire spill locations area gets aligned to largest natural
  // alignment of the variables that have a spill slot.
  uint32_t SpillAreaAlignmentBytes = 0;
  // A spill slot linked to a variable with a stack slot should reuse
  // that stack slot.
  std::function<bool(Variable *)> TargetVarHook =
      [&VariablesLinkedToSpillSlots](Variable *Var) {
        if (SpillVariable *SpillVar = llvm::dyn_cast<SpillVariable>(Var)) {
          assert(Var->getWeight().isZero());
          if (SpillVar->getLinkedTo() && !SpillVar->getLinkedTo()->hasReg()) {
            VariablesLinkedToSpillSlots.push_back(Var);
            return true;
          }
        }
        return false;
      };

  // Compute the list of spilled variables and bounds for GlobalsSize, etc.
  getVarStackSlotParams(SortedSpilledVariables, RegsUsed, &GlobalsSize,
                        &SpillAreaSizeBytes, &SpillAreaAlignmentBytes,
                        &LocalsSlotsAlignmentBytes, TargetVarHook);
  uint32_t LocalsSpillAreaSize = SpillAreaSizeBytes;
  SpillAreaSizeBytes += GlobalsSize;

  // Add push instructions for preserved registers.
  uint32_t NumCallee = 0;
  size_t PreservedRegsSizeBytes = 0;
  for (SizeT i = 0; i < CalleeSaves.size(); ++i) {
    if (CalleeSaves[i] && RegsUsed[i]) {
      ++NumCallee;
      PreservedRegsSizeBytes += 4;
      _push(getPhysicalRegister(i));
    }
  }
  Ctx->statsUpdateRegistersSaved(NumCallee);

  // Generate "push ebp; mov ebp, esp"
  if (IsEbpBasedFrame) {
    assert((RegsUsed & getRegisterSet(RegSet_FramePointer, RegSet_None))
               .count() == 0);
    PreservedRegsSizeBytes += 4;
    Variable *ebp = getPhysicalRegister(RegX8632::Reg_ebp);
    Variable *esp = getPhysicalRegister(RegX8632::Reg_esp);
    _push(ebp);
    _mov(ebp, esp);
    // Keep ebp live for late-stage liveness analysis
    // (e.g. asm-verbose mode).
    Context.insert(InstFakeUse::create(Func, ebp));
  }

  // Align the variables area. SpillAreaPaddingBytes is the size of
  // the region after the preserved registers and before the spill areas.
  // LocalsSlotsPaddingBytes is the amount of padding between the globals
  // and locals area if they are separate.
  assert(SpillAreaAlignmentBytes <= Traits::X86_STACK_ALIGNMENT_BYTES);
  assert(LocalsSlotsAlignmentBytes <= SpillAreaAlignmentBytes);
  uint32_t SpillAreaPaddingBytes = 0;
  uint32_t LocalsSlotsPaddingBytes = 0;
  alignStackSpillAreas(Traits::X86_RET_IP_SIZE_BYTES + PreservedRegsSizeBytes,
                       SpillAreaAlignmentBytes, GlobalsSize,
                       LocalsSlotsAlignmentBytes, &SpillAreaPaddingBytes,
                       &LocalsSlotsPaddingBytes);
  SpillAreaSizeBytes += SpillAreaPaddingBytes + LocalsSlotsPaddingBytes;
  uint32_t GlobalsAndSubsequentPaddingSize =
      GlobalsSize + LocalsSlotsPaddingBytes;

  // Align esp if necessary.
  if (NeedsStackAlignment) {
    uint32_t StackOffset =
        Traits::X86_RET_IP_SIZE_BYTES + PreservedRegsSizeBytes;
    uint32_t StackSize =
        Traits::applyStackAlignment(StackOffset + SpillAreaSizeBytes);
    SpillAreaSizeBytes = StackSize - StackOffset;
  }

  // Generate "sub esp, SpillAreaSizeBytes"
  if (SpillAreaSizeBytes)
    _sub(getPhysicalRegister(RegX8632::Reg_esp),
         Ctx->getConstantInt32(SpillAreaSizeBytes));
  Ctx->statsUpdateFrameBytes(SpillAreaSizeBytes);

  resetStackAdjustment();

  // Fill in stack offsets for stack args, and copy args into registers
  // for those that were register-allocated.  Args are pushed right to
  // left, so Arg[0] is closest to the stack/frame pointer.
  Variable *FramePtr = getPhysicalRegister(getFrameOrStackReg());
  size_t BasicFrameOffset =
      PreservedRegsSizeBytes + Traits::X86_RET_IP_SIZE_BYTES;
  if (!IsEbpBasedFrame)
    BasicFrameOffset += SpillAreaSizeBytes;

  const VarList &Args = Func->getArgs();
  size_t InArgsSizeBytes = 0;
  unsigned NumXmmArgs = 0;
  for (Variable *Arg : Args) {
    // Skip arguments passed in registers.
    if (isVectorType(Arg->getType()) && NumXmmArgs < Traits::X86_MAX_XMM_ARGS) {
      ++NumXmmArgs;
      continue;
    }
    finishArgumentLowering(Arg, FramePtr, BasicFrameOffset, InArgsSizeBytes);
  }

  // Fill in stack offsets for locals.
  assignVarStackSlots(SortedSpilledVariables, SpillAreaPaddingBytes,
                      SpillAreaSizeBytes, GlobalsAndSubsequentPaddingSize,
                      IsEbpBasedFrame);
  // Assign stack offsets to variables that have been linked to spilled
  // variables.
  for (Variable *Var : VariablesLinkedToSpillSlots) {
    Variable *Linked = (llvm::cast<SpillVariable>(Var))->getLinkedTo();
    Var->setStackOffset(Linked->getStackOffset());
  }
  this->HasComputedFrame = true;

  if (BuildDefs::dump() && Func->isVerbose(IceV_Frame)) {
    OstreamLocker L(Func->getContext());
    Ostream &Str = Func->getContext()->getStrDump();

    Str << "Stack layout:\n";
    uint32_t EspAdjustmentPaddingSize =
        SpillAreaSizeBytes - LocalsSpillAreaSize -
        GlobalsAndSubsequentPaddingSize - SpillAreaPaddingBytes;
    Str << " in-args = " << InArgsSizeBytes << " bytes\n"
        << " return address = " << Traits::X86_RET_IP_SIZE_BYTES << " bytes\n"
        << " preserved registers = " << PreservedRegsSizeBytes << " bytes\n"
        << " spill area padding = " << SpillAreaPaddingBytes << " bytes\n"
        << " globals spill area = " << GlobalsSize << " bytes\n"
        << " globals-locals spill areas intermediate padding = "
        << GlobalsAndSubsequentPaddingSize - GlobalsSize << " bytes\n"
        << " locals spill area = " << LocalsSpillAreaSize << " bytes\n"
        << " esp alignment padding = " << EspAdjustmentPaddingSize
        << " bytes\n";

    Str << "Stack details:\n"
        << " esp adjustment = " << SpillAreaSizeBytes << " bytes\n"
        << " spill area alignment = " << SpillAreaAlignmentBytes << " bytes\n"
        << " locals spill area alignment = " << LocalsSlotsAlignmentBytes
        << " bytes\n"
        << " is ebp based = " << IsEbpBasedFrame << "\n";
  }
}

template <class Machine> void TargetX86Base<Machine>::addEpilog(CfgNode *Node) {
  InstList &Insts = Node->getInsts();
  InstList::reverse_iterator RI, E;
  for (RI = Insts.rbegin(), E = Insts.rend(); RI != E; ++RI) {
    if (llvm::isa<InstX8632Ret>(*RI))
      break;
  }
  if (RI == E)
    return;

  // Convert the reverse_iterator position into its corresponding
  // (forward) iterator position.
  InstList::iterator InsertPoint = RI.base();
  --InsertPoint;
  Context.init(Node);
  Context.setInsertPoint(InsertPoint);

  Variable *esp = getPhysicalRegister(RegX8632::Reg_esp);
  if (IsEbpBasedFrame) {
    Variable *ebp = getPhysicalRegister(RegX8632::Reg_ebp);
    // For late-stage liveness analysis (e.g. asm-verbose mode),
    // adding a fake use of esp before the assignment of esp=ebp keeps
    // previous esp adjustments from being dead-code eliminated.
    Context.insert(InstFakeUse::create(Func, esp));
    _mov(esp, ebp);
    _pop(ebp);
  } else {
    // add esp, SpillAreaSizeBytes
    if (SpillAreaSizeBytes)
      _add(esp, Ctx->getConstantInt32(SpillAreaSizeBytes));
  }

  // Add pop instructions for preserved registers.
  llvm::SmallBitVector CalleeSaves =
      getRegisterSet(RegSet_CalleeSave, RegSet_None);
  for (SizeT i = 0; i < CalleeSaves.size(); ++i) {
    SizeT j = CalleeSaves.size() - i - 1;
    if (j == RegX8632::Reg_ebp && IsEbpBasedFrame)
      continue;
    if (CalleeSaves[j] && RegsUsed[j]) {
      _pop(getPhysicalRegister(j));
    }
  }

  if (!Ctx->getFlags().getUseSandboxing())
    return;
  // Change the original ret instruction into a sandboxed return sequence.
  // t:ecx = pop
  // bundle_lock
  // and t, ~31
  // jmp *t
  // bundle_unlock
  // FakeUse <original_ret_operand>
  const SizeT BundleSize =
      1 << Func->template getAssembler<>()->getBundleAlignLog2Bytes();
  Variable *T_ecx = makeReg(IceType_i32, RegX8632::Reg_ecx);
  _pop(T_ecx);
  _bundle_lock();
  _and(T_ecx, Ctx->getConstantInt32(~(BundleSize - 1)));
  _jmp(T_ecx);
  _bundle_unlock();
  if (RI->getSrcSize()) {
    Variable *RetValue = llvm::cast<Variable>(RI->getSrc(0));
    Context.insert(InstFakeUse::create(Func, RetValue));
  }
  RI->setDeleted();
}

template <class Machine> void TargetX86Base<Machine>::split64(Variable *Var) {
  switch (Var->getType()) {
  default:
    return;
  case IceType_i64:
  // TODO: Only consider F64 if we need to push each half when
  // passing as an argument to a function call.  Note that each half
  // is still typed as I32.
  case IceType_f64:
    break;
  }
  Variable *Lo = Var->getLo();
  Variable *Hi = Var->getHi();
  if (Lo) {
    assert(Hi);
    return;
  }
  assert(Hi == nullptr);
  Lo = Func->template makeVariable(IceType_i32);
  Hi = Func->template makeVariable(IceType_i32);
  if (BuildDefs::dump()) {
    Lo->setName(Func, Var->getName(Func) + "__lo");
    Hi->setName(Func, Var->getName(Func) + "__hi");
  }
  Var->setLoHi(Lo, Hi);
  if (Var->getIsArg()) {
    Lo->setIsArg();
    Hi->setIsArg();
  }
}

template <class Machine>
Operand *TargetX86Base<Machine>::loOperand(Operand *Operand) {
  assert(Operand->getType() == IceType_i64 ||
         Operand->getType() == IceType_f64);
  if (Operand->getType() != IceType_i64 && Operand->getType() != IceType_f64)
    return Operand;
  if (Variable *Var = llvm::dyn_cast<Variable>(Operand)) {
    split64(Var);
    return Var->getLo();
  }
  if (ConstantInteger64 *Const = llvm::dyn_cast<ConstantInteger64>(Operand)) {
    ConstantInteger32 *ConstInt = llvm::dyn_cast<ConstantInteger32>(
        Ctx->getConstantInt32(static_cast<int32_t>(Const->getValue())));
    return legalize(ConstInt);
  }
  if (OperandX8632Mem *Mem = llvm::dyn_cast<OperandX8632Mem>(Operand)) {
    OperandX8632Mem *MemOperand = OperandX8632Mem::create(
        Func, IceType_i32, Mem->getBase(), Mem->getOffset(), Mem->getIndex(),
        Mem->getShift(), Mem->getSegmentRegister());
    // Test if we should randomize or pool the offset, if so randomize it or
    // pool it then create mem operand with the blinded/pooled constant.
    // Otherwise, return the mem operand as ordinary mem operand.
    return legalize(MemOperand);
  }
  llvm_unreachable("Unsupported operand type");
  return nullptr;
}

template <class Machine>
Operand *TargetX86Base<Machine>::hiOperand(Operand *Operand) {
  assert(Operand->getType() == IceType_i64 ||
         Operand->getType() == IceType_f64);
  if (Operand->getType() != IceType_i64 && Operand->getType() != IceType_f64)
    return Operand;
  if (Variable *Var = llvm::dyn_cast<Variable>(Operand)) {
    split64(Var);
    return Var->getHi();
  }
  if (ConstantInteger64 *Const = llvm::dyn_cast<ConstantInteger64>(Operand)) {
    ConstantInteger32 *ConstInt = llvm::dyn_cast<ConstantInteger32>(
        Ctx->getConstantInt32(static_cast<int32_t>(Const->getValue() >> 32)));
    // check if we need to blind/pool the constant
    return legalize(ConstInt);
  }
  if (OperandX8632Mem *Mem = llvm::dyn_cast<OperandX8632Mem>(Operand)) {
    Constant *Offset = Mem->getOffset();
    if (Offset == nullptr) {
      Offset = Ctx->getConstantInt32(4);
    } else if (ConstantInteger32 *IntOffset =
                   llvm::dyn_cast<ConstantInteger32>(Offset)) {
      Offset = Ctx->getConstantInt32(4 + IntOffset->getValue());
    } else if (ConstantRelocatable *SymOffset =
                   llvm::dyn_cast<ConstantRelocatable>(Offset)) {
      assert(!Utils::WouldOverflowAdd(SymOffset->getOffset(), 4));
      Offset =
          Ctx->getConstantSym(4 + SymOffset->getOffset(), SymOffset->getName(),
                              SymOffset->getSuppressMangling());
    }
    OperandX8632Mem *MemOperand = OperandX8632Mem::create(
        Func, IceType_i32, Mem->getBase(), Offset, Mem->getIndex(),
        Mem->getShift(), Mem->getSegmentRegister());
    // Test if the Offset is an eligible i32 constants for randomization and
    // pooling. Blind/pool it if it is. Otherwise return as oridinary mem
    // operand.
    return legalize(MemOperand);
  }
  llvm_unreachable("Unsupported operand type");
  return nullptr;
}

template <class Machine>
llvm::SmallBitVector
TargetX86Base<Machine>::getRegisterSet(RegSetMask Include,
                                       RegSetMask Exclude) const {
  llvm::SmallBitVector Registers(RegX8632::Reg_NUM);

#define X(val, encode, name, name16, name8, scratch, preserved, stackptr,      \
          frameptr, isI8, isInt, isFP)                                         \
  if (scratch && (Include & RegSet_CallerSave))                                \
    Registers[RegX8632::val] = true;                                           \
  if (preserved && (Include & RegSet_CalleeSave))                              \
    Registers[RegX8632::val] = true;                                           \
  if (stackptr && (Include & RegSet_StackPointer))                             \
    Registers[RegX8632::val] = true;                                           \
  if (frameptr && (Include & RegSet_FramePointer))                             \
    Registers[RegX8632::val] = true;                                           \
  if (scratch && (Exclude & RegSet_CallerSave))                                \
    Registers[RegX8632::val] = false;                                          \
  if (preserved && (Exclude & RegSet_CalleeSave))                              \
    Registers[RegX8632::val] = false;                                          \
  if (stackptr && (Exclude & RegSet_StackPointer))                             \
    Registers[RegX8632::val] = false;                                          \
  if (frameptr && (Exclude & RegSet_FramePointer))                             \
    Registers[RegX8632::val] = false;

  REGX8632_TABLE

#undef X

  return Registers;
}

template <class Machine>
void TargetX86Base<Machine>::lowerAlloca(const InstAlloca *Inst) {
  IsEbpBasedFrame = true;
  // Conservatively require the stack to be aligned.  Some stack
  // adjustment operations implemented below assume that the stack is
  // aligned before the alloca.  All the alloca code ensures that the
  // stack alignment is preserved after the alloca.  The stack alignment
  // restriction can be relaxed in some cases.
  NeedsStackAlignment = true;

  // TODO(stichnot): minimize the number of adjustments of esp, etc.
  Variable *esp = getPhysicalRegister(RegX8632::Reg_esp);
  Operand *TotalSize = legalize(Inst->getSizeInBytes());
  Variable *Dest = Inst->getDest();
  uint32_t AlignmentParam = Inst->getAlignInBytes();
  // For default align=0, set it to the real value 1, to avoid any
  // bit-manipulation problems below.
  AlignmentParam = std::max(AlignmentParam, 1u);

  // LLVM enforces power of 2 alignment.
  assert(llvm::isPowerOf2_32(AlignmentParam));
  assert(llvm::isPowerOf2_32(Traits::X86_STACK_ALIGNMENT_BYTES));

  uint32_t Alignment =
      std::max(AlignmentParam, Traits::X86_STACK_ALIGNMENT_BYTES);
  if (Alignment > Traits::X86_STACK_ALIGNMENT_BYTES) {
    _and(esp, Ctx->getConstantInt32(-Alignment));
  }
  if (const auto *ConstantTotalSize =
          llvm::dyn_cast<ConstantInteger32>(TotalSize)) {
    uint32_t Value = ConstantTotalSize->getValue();
    Value = Utils::applyAlignment(Value, Alignment);
    _sub(esp, Ctx->getConstantInt32(Value));
  } else {
    // Non-constant sizes need to be adjusted to the next highest
    // multiple of the required alignment at runtime.
    Variable *T = makeReg(IceType_i32);
    _mov(T, TotalSize);
    _add(T, Ctx->getConstantInt32(Alignment - 1));
    _and(T, Ctx->getConstantInt32(-Alignment));
    _sub(esp, T);
  }
  _mov(Dest, esp);
}

// Strength-reduce scalar integer multiplication by a constant (for
// i32 or narrower) for certain constants.  The lea instruction can be
// used to multiply by 3, 5, or 9, and the lsh instruction can be used
// to multiply by powers of 2.  These can be combined such that
// e.g. multiplying by 100 can be done as 2 lea-based multiplies by 5,
// combined with left-shifting by 2.
template <class Machine>
bool TargetX86Base<Machine>::optimizeScalarMul(Variable *Dest, Operand *Src0,
                                               int32_t Src1) {
  // Disable this optimization for Om1 and O0, just to keep things
  // simple there.
  if (Ctx->getFlags().getOptLevel() < Opt_1)
    return false;
  Type Ty = Dest->getType();
  Variable *T = nullptr;
  if (Src1 == -1) {
    _mov(T, Src0);
    _neg(T);
    _mov(Dest, T);
    return true;
  }
  if (Src1 == 0) {
    _mov(Dest, Ctx->getConstantZero(Ty));
    return true;
  }
  if (Src1 == 1) {
    _mov(T, Src0);
    _mov(Dest, T);
    return true;
  }
  // Don't bother with the edge case where Src1 == MININT.
  if (Src1 == -Src1)
    return false;
  const bool Src1IsNegative = Src1 < 0;
  if (Src1IsNegative)
    Src1 = -Src1;
  uint32_t Count9 = 0;
  uint32_t Count5 = 0;
  uint32_t Count3 = 0;
  uint32_t Count2 = 0;
  uint32_t CountOps = 0;
  while (Src1 > 1) {
    if (Src1 % 9 == 0) {
      ++CountOps;
      ++Count9;
      Src1 /= 9;
    } else if (Src1 % 5 == 0) {
      ++CountOps;
      ++Count5;
      Src1 /= 5;
    } else if (Src1 % 3 == 0) {
      ++CountOps;
      ++Count3;
      Src1 /= 3;
    } else if (Src1 % 2 == 0) {
      if (Count2 == 0)
        ++CountOps;
      ++Count2;
      Src1 /= 2;
    } else {
      return false;
    }
  }
  // Lea optimization only works for i16 and i32 types, not i8.
  if (Ty != IceType_i16 && Ty != IceType_i32 && (Count3 || Count5 || Count9))
    return false;
  // Limit the number of lea/shl operations for a single multiply, to
  // a somewhat arbitrary choice of 3.
  const uint32_t MaxOpsForOptimizedMul = 3;
  if (CountOps > MaxOpsForOptimizedMul)
    return false;
  _mov(T, Src0);
  Constant *Zero = Ctx->getConstantZero(IceType_i32);
  for (uint32_t i = 0; i < Count9; ++i) {
    const uint16_t Shift = 3; // log2(9-1)
    _lea(T, OperandX8632Mem::create(Func, IceType_void, T, Zero, T, Shift));
    _set_dest_nonkillable();
  }
  for (uint32_t i = 0; i < Count5; ++i) {
    const uint16_t Shift = 2; // log2(5-1)
    _lea(T, OperandX8632Mem::create(Func, IceType_void, T, Zero, T, Shift));
    _set_dest_nonkillable();
  }
  for (uint32_t i = 0; i < Count3; ++i) {
    const uint16_t Shift = 1; // log2(3-1)
    _lea(T, OperandX8632Mem::create(Func, IceType_void, T, Zero, T, Shift));
    _set_dest_nonkillable();
  }
  if (Count2) {
    _shl(T, Ctx->getConstantInt(Ty, Count2));
  }
  if (Src1IsNegative)
    _neg(T);
  _mov(Dest, T);
  return true;
}

template <class Machine>
void TargetX86Base<Machine>::lowerArithmetic(const InstArithmetic *Inst) {
  Variable *Dest = Inst->getDest();
  Operand *Src0 = legalize(Inst->getSrc(0));
  Operand *Src1 = legalize(Inst->getSrc(1));
  if (Inst->isCommutative()) {
    if (!llvm::isa<Variable>(Src0) && llvm::isa<Variable>(Src1))
      std::swap(Src0, Src1);
    if (llvm::isa<Constant>(Src0) && !llvm::isa<Constant>(Src1))
      std::swap(Src0, Src1);
  }
  if (Dest->getType() == IceType_i64) {
    // These helper-call-involved instructions are lowered in this
    // separate switch. This is because loOperand() and hiOperand()
    // may insert redundant instructions for constant blinding and
    // pooling. Such redundant instructions will fail liveness analysis
    // under -Om1 setting. And, actually these arguments do not need
    // to be processed with loOperand() and hiOperand() to be used.
    switch (Inst->getOp()) {
    case InstArithmetic::Udiv: {
      const SizeT MaxSrcs = 2;
      InstCall *Call = makeHelperCall(H_udiv_i64, Dest, MaxSrcs);
      Call->addArg(Inst->getSrc(0));
      Call->addArg(Inst->getSrc(1));
      lowerCall(Call);
      return;
    }
    case InstArithmetic::Sdiv: {
      const SizeT MaxSrcs = 2;
      InstCall *Call = makeHelperCall(H_sdiv_i64, Dest, MaxSrcs);
      Call->addArg(Inst->getSrc(0));
      Call->addArg(Inst->getSrc(1));
      lowerCall(Call);
      return;
    }
    case InstArithmetic::Urem: {
      const SizeT MaxSrcs = 2;
      InstCall *Call = makeHelperCall(H_urem_i64, Dest, MaxSrcs);
      Call->addArg(Inst->getSrc(0));
      Call->addArg(Inst->getSrc(1));
      lowerCall(Call);
      return;
    }
    case InstArithmetic::Srem: {
      const SizeT MaxSrcs = 2;
      InstCall *Call = makeHelperCall(H_srem_i64, Dest, MaxSrcs);
      Call->addArg(Inst->getSrc(0));
      Call->addArg(Inst->getSrc(1));
      lowerCall(Call);
      return;
    }
    default:
      break;
    }

    Variable *DestLo = llvm::cast<Variable>(loOperand(Dest));
    Variable *DestHi = llvm::cast<Variable>(hiOperand(Dest));
    Operand *Src0Lo = loOperand(Src0);
    Operand *Src0Hi = hiOperand(Src0);
    Operand *Src1Lo = loOperand(Src1);
    Operand *Src1Hi = hiOperand(Src1);
    Variable *T_Lo = nullptr, *T_Hi = nullptr;
    switch (Inst->getOp()) {
    case InstArithmetic::_num:
      llvm_unreachable("Unknown arithmetic operator");
      break;
    case InstArithmetic::Add:
      _mov(T_Lo, Src0Lo);
      _add(T_Lo, Src1Lo);
      _mov(DestLo, T_Lo);
      _mov(T_Hi, Src0Hi);
      _adc(T_Hi, Src1Hi);
      _mov(DestHi, T_Hi);
      break;
    case InstArithmetic::And:
      _mov(T_Lo, Src0Lo);
      _and(T_Lo, Src1Lo);
      _mov(DestLo, T_Lo);
      _mov(T_Hi, Src0Hi);
      _and(T_Hi, Src1Hi);
      _mov(DestHi, T_Hi);
      break;
    case InstArithmetic::Or:
      _mov(T_Lo, Src0Lo);
      _or(T_Lo, Src1Lo);
      _mov(DestLo, T_Lo);
      _mov(T_Hi, Src0Hi);
      _or(T_Hi, Src1Hi);
      _mov(DestHi, T_Hi);
      break;
    case InstArithmetic::Xor:
      _mov(T_Lo, Src0Lo);
      _xor(T_Lo, Src1Lo);
      _mov(DestLo, T_Lo);
      _mov(T_Hi, Src0Hi);
      _xor(T_Hi, Src1Hi);
      _mov(DestHi, T_Hi);
      break;
    case InstArithmetic::Sub:
      _mov(T_Lo, Src0Lo);
      _sub(T_Lo, Src1Lo);
      _mov(DestLo, T_Lo);
      _mov(T_Hi, Src0Hi);
      _sbb(T_Hi, Src1Hi);
      _mov(DestHi, T_Hi);
      break;
    case InstArithmetic::Mul: {
      Variable *T_1 = nullptr, *T_2 = nullptr, *T_3 = nullptr;
      Variable *T_4Lo = makeReg(IceType_i32, RegX8632::Reg_eax);
      Variable *T_4Hi = makeReg(IceType_i32, RegX8632::Reg_edx);
      // gcc does the following:
      // a=b*c ==>
      //   t1 = b.hi; t1 *=(imul) c.lo
      //   t2 = c.hi; t2 *=(imul) b.lo
      //   t3:eax = b.lo
      //   t4.hi:edx,t4.lo:eax = t3:eax *(mul) c.lo
      //   a.lo = t4.lo
      //   t4.hi += t1
      //   t4.hi += t2
      //   a.hi = t4.hi
      // The mul instruction cannot take an immediate operand.
      Src1Lo = legalize(Src1Lo, Legal_Reg | Legal_Mem);
      _mov(T_1, Src0Hi);
      _imul(T_1, Src1Lo);
      _mov(T_2, Src1Hi);
      _imul(T_2, Src0Lo);
      _mov(T_3, Src0Lo, RegX8632::Reg_eax);
      _mul(T_4Lo, T_3, Src1Lo);
      // The mul instruction produces two dest variables, edx:eax.  We
      // create a fake definition of edx to account for this.
      Context.insert(InstFakeDef::create(Func, T_4Hi, T_4Lo));
      _mov(DestLo, T_4Lo);
      _add(T_4Hi, T_1);
      _add(T_4Hi, T_2);
      _mov(DestHi, T_4Hi);
    } break;
    case InstArithmetic::Shl: {
      // TODO: Refactor the similarities between Shl, Lshr, and Ashr.
      // gcc does the following:
      // a=b<<c ==>
      //   t1:ecx = c.lo & 0xff
      //   t2 = b.lo
      //   t3 = b.hi
      //   t3 = shld t3, t2, t1
      //   t2 = shl t2, t1
      //   test t1, 0x20
      //   je L1
      //   use(t3)
      //   t3 = t2
      //   t2 = 0
      // L1:
      //   a.lo = t2
      //   a.hi = t3
      Variable *T_1 = nullptr, *T_2 = nullptr, *T_3 = nullptr;
      Constant *BitTest = Ctx->getConstantInt32(0x20);
      Constant *Zero = Ctx->getConstantZero(IceType_i32);
      InstX8632Label *Label = InstX8632Label::create(Func, this);
      _mov(T_1, Src1Lo, RegX8632::Reg_ecx);
      _mov(T_2, Src0Lo);
      _mov(T_3, Src0Hi);
      _shld(T_3, T_2, T_1);
      _shl(T_2, T_1);
      _test(T_1, BitTest);
      _br(CondX86::Br_e, Label);
      // T_2 and T_3 are being assigned again because of the
      // intra-block control flow, so we need the _mov_nonkillable
      // variant to avoid liveness problems.
      _mov_nonkillable(T_3, T_2);
      _mov_nonkillable(T_2, Zero);
      Context.insert(Label);
      _mov(DestLo, T_2);
      _mov(DestHi, T_3);
    } break;
    case InstArithmetic::Lshr: {
      // a=b>>c (unsigned) ==>
      //   t1:ecx = c.lo & 0xff
      //   t2 = b.lo
      //   t3 = b.hi
      //   t2 = shrd t2, t3, t1
      //   t3 = shr t3, t1
      //   test t1, 0x20
      //   je L1
      //   use(t2)
      //   t2 = t3
      //   t3 = 0
      // L1:
      //   a.lo = t2
      //   a.hi = t3
      Variable *T_1 = nullptr, *T_2 = nullptr, *T_3 = nullptr;
      Constant *BitTest = Ctx->getConstantInt32(0x20);
      Constant *Zero = Ctx->getConstantZero(IceType_i32);
      InstX8632Label *Label = InstX8632Label::create(Func, this);
      _mov(T_1, Src1Lo, RegX8632::Reg_ecx);
      _mov(T_2, Src0Lo);
      _mov(T_3, Src0Hi);
      _shrd(T_2, T_3, T_1);
      _shr(T_3, T_1);
      _test(T_1, BitTest);
      _br(CondX86::Br_e, Label);
      // T_2 and T_3 are being assigned again because of the
      // intra-block control flow, so we need the _mov_nonkillable
      // variant to avoid liveness problems.
      _mov_nonkillable(T_2, T_3);
      _mov_nonkillable(T_3, Zero);
      Context.insert(Label);
      _mov(DestLo, T_2);
      _mov(DestHi, T_3);
    } break;
    case InstArithmetic::Ashr: {
      // a=b>>c (signed) ==>
      //   t1:ecx = c.lo & 0xff
      //   t2 = b.lo
      //   t3 = b.hi
      //   t2 = shrd t2, t3, t1
      //   t3 = sar t3, t1
      //   test t1, 0x20
      //   je L1
      //   use(t2)
      //   t2 = t3
      //   t3 = sar t3, 0x1f
      // L1:
      //   a.lo = t2
      //   a.hi = t3
      Variable *T_1 = nullptr, *T_2 = nullptr, *T_3 = nullptr;
      Constant *BitTest = Ctx->getConstantInt32(0x20);
      Constant *SignExtend = Ctx->getConstantInt32(0x1f);
      InstX8632Label *Label = InstX8632Label::create(Func, this);
      _mov(T_1, Src1Lo, RegX8632::Reg_ecx);
      _mov(T_2, Src0Lo);
      _mov(T_3, Src0Hi);
      _shrd(T_2, T_3, T_1);
      _sar(T_3, T_1);
      _test(T_1, BitTest);
      _br(CondX86::Br_e, Label);
      // T_2 and T_3 are being assigned again because of the
      // intra-block control flow, so T_2 needs the _mov_nonkillable
      // variant to avoid liveness problems.  T_3 doesn't need special
      // treatment because it is reassigned via _sar instead of _mov.
      _mov_nonkillable(T_2, T_3);
      _sar(T_3, SignExtend);
      Context.insert(Label);
      _mov(DestLo, T_2);
      _mov(DestHi, T_3);
    } break;
    case InstArithmetic::Fadd:
    case InstArithmetic::Fsub:
    case InstArithmetic::Fmul:
    case InstArithmetic::Fdiv:
    case InstArithmetic::Frem:
      llvm_unreachable("FP instruction with i64 type");
      break;
    case InstArithmetic::Udiv:
    case InstArithmetic::Sdiv:
    case InstArithmetic::Urem:
    case InstArithmetic::Srem:
      llvm_unreachable("Call-helper-involved instruction for i64 type \
                       should have already been handled before");
      break;
    }
    return;
  }
  if (isVectorType(Dest->getType())) {
    // TODO: Trap on integer divide and integer modulo by zero.
    // See: https://code.google.com/p/nativeclient/issues/detail?id=3899
    if (llvm::isa<OperandX8632Mem>(Src1))
      Src1 = legalizeToVar(Src1);
    switch (Inst->getOp()) {
    case InstArithmetic::_num:
      llvm_unreachable("Unknown arithmetic operator");
      break;
    case InstArithmetic::Add: {
      Variable *T = makeReg(Dest->getType());
      _movp(T, Src0);
      _padd(T, Src1);
      _movp(Dest, T);
    } break;
    case InstArithmetic::And: {
      Variable *T = makeReg(Dest->getType());
      _movp(T, Src0);
      _pand(T, Src1);
      _movp(Dest, T);
    } break;
    case InstArithmetic::Or: {
      Variable *T = makeReg(Dest->getType());
      _movp(T, Src0);
      _por(T, Src1);
      _movp(Dest, T);
    } break;
    case InstArithmetic::Xor: {
      Variable *T = makeReg(Dest->getType());
      _movp(T, Src0);
      _pxor(T, Src1);
      _movp(Dest, T);
    } break;
    case InstArithmetic::Sub: {
      Variable *T = makeReg(Dest->getType());
      _movp(T, Src0);
      _psub(T, Src1);
      _movp(Dest, T);
    } break;
    case InstArithmetic::Mul: {
      bool TypesAreValidForPmull =
          Dest->getType() == IceType_v4i32 || Dest->getType() == IceType_v8i16;
      bool InstructionSetIsValidForPmull =
          Dest->getType() == IceType_v8i16 || InstructionSet >= Machine::SSE4_1;
      if (TypesAreValidForPmull && InstructionSetIsValidForPmull) {
        Variable *T = makeReg(Dest->getType());
        _movp(T, Src0);
        _pmull(T, Src1);
        _movp(Dest, T);
      } else if (Dest->getType() == IceType_v4i32) {
        // Lowering sequence:
        // Note: The mask arguments have index 0 on the left.
        //
        // movups  T1, Src0
        // pshufd  T2, Src0, {1,0,3,0}
        // pshufd  T3, Src1, {1,0,3,0}
        // # T1 = {Src0[0] * Src1[0], Src0[2] * Src1[2]}
        // pmuludq T1, Src1
        // # T2 = {Src0[1] * Src1[1], Src0[3] * Src1[3]}
        // pmuludq T2, T3
        // # T1 = {lo(T1[0]), lo(T1[2]), lo(T2[0]), lo(T2[2])}
        // shufps  T1, T2, {0,2,0,2}
        // pshufd  T4, T1, {0,2,1,3}
        // movups  Dest, T4

        // Mask that directs pshufd to create a vector with entries
        // Src[1, 0, 3, 0]
        const unsigned Constant1030 = 0x31;
        Constant *Mask1030 = Ctx->getConstantInt32(Constant1030);
        // Mask that directs shufps to create a vector with entries
        // Dest[0, 2], Src[0, 2]
        const unsigned Mask0202 = 0x88;
        // Mask that directs pshufd to create a vector with entries
        // Src[0, 2, 1, 3]
        const unsigned Mask0213 = 0xd8;
        Variable *T1 = makeReg(IceType_v4i32);
        Variable *T2 = makeReg(IceType_v4i32);
        Variable *T3 = makeReg(IceType_v4i32);
        Variable *T4 = makeReg(IceType_v4i32);
        _movp(T1, Src0);
        _pshufd(T2, Src0, Mask1030);
        _pshufd(T3, Src1, Mask1030);
        _pmuludq(T1, Src1);
        _pmuludq(T2, T3);
        _shufps(T1, T2, Ctx->getConstantInt32(Mask0202));
        _pshufd(T4, T1, Ctx->getConstantInt32(Mask0213));
        _movp(Dest, T4);
      } else {
        assert(Dest->getType() == IceType_v16i8);
        scalarizeArithmetic(Inst->getOp(), Dest, Src0, Src1);
      }
    } break;
    case InstArithmetic::Shl:
    case InstArithmetic::Lshr:
    case InstArithmetic::Ashr:
    case InstArithmetic::Udiv:
    case InstArithmetic::Urem:
    case InstArithmetic::Sdiv:
    case InstArithmetic::Srem:
      scalarizeArithmetic(Inst->getOp(), Dest, Src0, Src1);
      break;
    case InstArithmetic::Fadd: {
      Variable *T = makeReg(Dest->getType());
      _movp(T, Src0);
      _addps(T, Src1);
      _movp(Dest, T);
    } break;
    case InstArithmetic::Fsub: {
      Variable *T = makeReg(Dest->getType());
      _movp(T, Src0);
      _subps(T, Src1);
      _movp(Dest, T);
    } break;
    case InstArithmetic::Fmul: {
      Variable *T = makeReg(Dest->getType());
      _movp(T, Src0);
      _mulps(T, Src1);
      _movp(Dest, T);
    } break;
    case InstArithmetic::Fdiv: {
      Variable *T = makeReg(Dest->getType());
      _movp(T, Src0);
      _divps(T, Src1);
      _movp(Dest, T);
    } break;
    case InstArithmetic::Frem:
      scalarizeArithmetic(Inst->getOp(), Dest, Src0, Src1);
      break;
    }
    return;
  }
  Variable *T_edx = nullptr;
  Variable *T = nullptr;
  switch (Inst->getOp()) {
  case InstArithmetic::_num:
    llvm_unreachable("Unknown arithmetic operator");
    break;
  case InstArithmetic::Add:
    _mov(T, Src0);
    _add(T, Src1);
    _mov(Dest, T);
    break;
  case InstArithmetic::And:
    _mov(T, Src0);
    _and(T, Src1);
    _mov(Dest, T);
    break;
  case InstArithmetic::Or:
    _mov(T, Src0);
    _or(T, Src1);
    _mov(Dest, T);
    break;
  case InstArithmetic::Xor:
    _mov(T, Src0);
    _xor(T, Src1);
    _mov(Dest, T);
    break;
  case InstArithmetic::Sub:
    _mov(T, Src0);
    _sub(T, Src1);
    _mov(Dest, T);
    break;
  case InstArithmetic::Mul:
    if (auto *C = llvm::dyn_cast<ConstantInteger32>(Src1)) {
      if (optimizeScalarMul(Dest, Src0, C->getValue()))
        return;
    }
    // The 8-bit version of imul only allows the form "imul r/m8"
    // where T must be in eax.
    if (isByteSizedArithType(Dest->getType())) {
      _mov(T, Src0, RegX8632::Reg_eax);
      Src1 = legalize(Src1, Legal_Reg | Legal_Mem);
    } else {
      _mov(T, Src0);
    }
    _imul(T, Src1);
    _mov(Dest, T);
    break;
  case InstArithmetic::Shl:
    _mov(T, Src0);
    if (!llvm::isa<Constant>(Src1))
      Src1 = legalizeToVar(Src1, RegX8632::Reg_ecx);
    _shl(T, Src1);
    _mov(Dest, T);
    break;
  case InstArithmetic::Lshr:
    _mov(T, Src0);
    if (!llvm::isa<Constant>(Src1))
      Src1 = legalizeToVar(Src1, RegX8632::Reg_ecx);
    _shr(T, Src1);
    _mov(Dest, T);
    break;
  case InstArithmetic::Ashr:
    _mov(T, Src0);
    if (!llvm::isa<Constant>(Src1))
      Src1 = legalizeToVar(Src1, RegX8632::Reg_ecx);
    _sar(T, Src1);
    _mov(Dest, T);
    break;
  case InstArithmetic::Udiv:
    // div and idiv are the few arithmetic operators that do not allow
    // immediates as the operand.
    Src1 = legalize(Src1, Legal_Reg | Legal_Mem);
    if (isByteSizedArithType(Dest->getType())) {
      Variable *T_ah = nullptr;
      Constant *Zero = Ctx->getConstantZero(IceType_i8);
      _mov(T, Src0, RegX8632::Reg_eax);
      _mov(T_ah, Zero, RegX8632::Reg_ah);
      _div(T, Src1, T_ah);
      _mov(Dest, T);
    } else {
      Constant *Zero = Ctx->getConstantZero(IceType_i32);
      _mov(T, Src0, RegX8632::Reg_eax);
      _mov(T_edx, Zero, RegX8632::Reg_edx);
      _div(T, Src1, T_edx);
      _mov(Dest, T);
    }
    break;
  case InstArithmetic::Sdiv:
    // TODO(stichnot): Enable this after doing better performance
    // and cross testing.
    if (false && Ctx->getFlags().getOptLevel() >= Opt_1) {
      // Optimize division by constant power of 2, but not for Om1
      // or O0, just to keep things simple there.
      if (auto *C = llvm::dyn_cast<ConstantInteger32>(Src1)) {
        int32_t Divisor = C->getValue();
        uint32_t UDivisor = static_cast<uint32_t>(Divisor);
        if (Divisor > 0 && llvm::isPowerOf2_32(UDivisor)) {
          uint32_t LogDiv = llvm::Log2_32(UDivisor);
          Type Ty = Dest->getType();
          // LLVM does the following for dest=src/(1<<log):
          //   t=src
          //   sar t,typewidth-1 // -1 if src is negative, 0 if not
          //   shr t,typewidth-log
          //   add t,src
          //   sar t,log
          //   dest=t
          uint32_t TypeWidth = Traits::X86_CHAR_BIT * typeWidthInBytes(Ty);
          _mov(T, Src0);
          // If for some reason we are dividing by 1, just treat it
          // like an assignment.
          if (LogDiv > 0) {
            // The initial sar is unnecessary when dividing by 2.
            if (LogDiv > 1)
              _sar(T, Ctx->getConstantInt(Ty, TypeWidth - 1));
            _shr(T, Ctx->getConstantInt(Ty, TypeWidth - LogDiv));
            _add(T, Src0);
            _sar(T, Ctx->getConstantInt(Ty, LogDiv));
          }
          _mov(Dest, T);
          return;
        }
      }
    }
    Src1 = legalize(Src1, Legal_Reg | Legal_Mem);
    if (isByteSizedArithType(Dest->getType())) {
      _mov(T, Src0, RegX8632::Reg_eax);
      _cbwdq(T, T);
      _idiv(T, Src1, T);
      _mov(Dest, T);
    } else {
      T_edx = makeReg(IceType_i32, RegX8632::Reg_edx);
      _mov(T, Src0, RegX8632::Reg_eax);
      _cbwdq(T_edx, T);
      _idiv(T, Src1, T_edx);
      _mov(Dest, T);
    }
    break;
  case InstArithmetic::Urem:
    Src1 = legalize(Src1, Legal_Reg | Legal_Mem);
    if (isByteSizedArithType(Dest->getType())) {
      Variable *T_ah = nullptr;
      Constant *Zero = Ctx->getConstantZero(IceType_i8);
      _mov(T, Src0, RegX8632::Reg_eax);
      _mov(T_ah, Zero, RegX8632::Reg_ah);
      _div(T_ah, Src1, T);
      _mov(Dest, T_ah);
    } else {
      Constant *Zero = Ctx->getConstantZero(IceType_i32);
      _mov(T_edx, Zero, RegX8632::Reg_edx);
      _mov(T, Src0, RegX8632::Reg_eax);
      _div(T_edx, Src1, T);
      _mov(Dest, T_edx);
    }
    break;
  case InstArithmetic::Srem:
    // TODO(stichnot): Enable this after doing better performance
    // and cross testing.
    if (false && Ctx->getFlags().getOptLevel() >= Opt_1) {
      // Optimize mod by constant power of 2, but not for Om1 or O0,
      // just to keep things simple there.
      if (auto *C = llvm::dyn_cast<ConstantInteger32>(Src1)) {
        int32_t Divisor = C->getValue();
        uint32_t UDivisor = static_cast<uint32_t>(Divisor);
        if (Divisor > 0 && llvm::isPowerOf2_32(UDivisor)) {
          uint32_t LogDiv = llvm::Log2_32(UDivisor);
          Type Ty = Dest->getType();
          // LLVM does the following for dest=src%(1<<log):
          //   t=src
          //   sar t,typewidth-1 // -1 if src is negative, 0 if not
          //   shr t,typewidth-log
          //   add t,src
          //   and t, -(1<<log)
          //   sub t,src
          //   neg t
          //   dest=t
          uint32_t TypeWidth = Traits::X86_CHAR_BIT * typeWidthInBytes(Ty);
          // If for some reason we are dividing by 1, just assign 0.
          if (LogDiv == 0) {
            _mov(Dest, Ctx->getConstantZero(Ty));
            return;
          }
          _mov(T, Src0);
          // The initial sar is unnecessary when dividing by 2.
          if (LogDiv > 1)
            _sar(T, Ctx->getConstantInt(Ty, TypeWidth - 1));
          _shr(T, Ctx->getConstantInt(Ty, TypeWidth - LogDiv));
          _add(T, Src0);
          _and(T, Ctx->getConstantInt(Ty, -(1 << LogDiv)));
          _sub(T, Src0);
          _neg(T);
          _mov(Dest, T);
          return;
        }
      }
    }
    Src1 = legalize(Src1, Legal_Reg | Legal_Mem);
    if (isByteSizedArithType(Dest->getType())) {
      Variable *T_ah = makeReg(IceType_i8, RegX8632::Reg_ah);
      _mov(T, Src0, RegX8632::Reg_eax);
      _cbwdq(T, T);
      Context.insert(InstFakeDef::create(Func, T_ah));
      _idiv(T_ah, Src1, T);
      _mov(Dest, T_ah);
    } else {
      T_edx = makeReg(IceType_i32, RegX8632::Reg_edx);
      _mov(T, Src0, RegX8632::Reg_eax);
      _cbwdq(T_edx, T);
      _idiv(T_edx, Src1, T);
      _mov(Dest, T_edx);
    }
    break;
  case InstArithmetic::Fadd:
    _mov(T, Src0);
    _addss(T, Src1);
    _mov(Dest, T);
    break;
  case InstArithmetic::Fsub:
    _mov(T, Src0);
    _subss(T, Src1);
    _mov(Dest, T);
    break;
  case InstArithmetic::Fmul:
    _mov(T, Src0);
    _mulss(T, Src1);
    _mov(Dest, T);
    break;
  case InstArithmetic::Fdiv:
    _mov(T, Src0);
    _divss(T, Src1);
    _mov(Dest, T);
    break;
  case InstArithmetic::Frem: {
    const SizeT MaxSrcs = 2;
    Type Ty = Dest->getType();
    InstCall *Call = makeHelperCall(
        isFloat32Asserting32Or64(Ty) ? H_frem_f32 : H_frem_f64, Dest, MaxSrcs);
    Call->addArg(Src0);
    Call->addArg(Src1);
    return lowerCall(Call);
  }
  }
}

template <class Machine>
void TargetX86Base<Machine>::lowerAssign(const InstAssign *Inst) {
  Variable *Dest = Inst->getDest();
  Operand *Src0 = Inst->getSrc(0);
  assert(Dest->getType() == Src0->getType());
  if (Dest->getType() == IceType_i64) {
    Src0 = legalize(Src0);
    Operand *Src0Lo = loOperand(Src0);
    Operand *Src0Hi = hiOperand(Src0);
    Variable *DestLo = llvm::cast<Variable>(loOperand(Dest));
    Variable *DestHi = llvm::cast<Variable>(hiOperand(Dest));
    Variable *T_Lo = nullptr, *T_Hi = nullptr;
    _mov(T_Lo, Src0Lo);
    _mov(DestLo, T_Lo);
    _mov(T_Hi, Src0Hi);
    _mov(DestHi, T_Hi);
  } else {
    Operand *RI;
    if (Dest->hasReg()) {
      // If Dest already has a physical register, then legalize the
      // Src operand into a Variable with the same register
      // assignment.  This is mostly a workaround for advanced phi
      // lowering's ad-hoc register allocation which assumes no
      // register allocation is needed when at least one of the
      // operands is non-memory.

      // If we have a physical register for the dest variable, we can
      // enable our constant blinding or pooling again. Note this is
      // only for advancedPhiLowering(), the flag flip should leave
      // no other side effect.
      {
        BoolFlagSaver B(RandomizationPoolingPaused, false);
        RI = legalize(Src0, Legal_Reg, Dest->getRegNum());
      }
    } else {
      // If Dest could be a stack operand, then RI must be a physical
      // register or a scalar integer immediate.
      RI = legalize(Src0, Legal_Reg | Legal_Imm);
    }
    if (isVectorType(Dest->getType()))
      _movp(Dest, RI);
    else
      _mov(Dest, RI);
  }
}

template <class Machine>
void TargetX86Base<Machine>::lowerBr(const InstBr *Inst) {
  if (Inst->isUnconditional()) {
    _br(Inst->getTargetUnconditional());
    return;
  }
  Operand *Cond = Inst->getCondition();

  // Handle folding opportunities.
  if (const class Inst *Producer = FoldingInfo.getProducerFor(Cond)) {
    assert(Producer->isDeleted());
    switch (BoolFolding::getProducerKind(Producer)) {
    default:
      break;
    case BoolFolding::PK_Icmp32: {
      // TODO(stichnot): Refactor similarities between this block and
      // the corresponding code in lowerIcmp().
      auto *Cmp = llvm::dyn_cast<InstIcmp>(Producer);
      Operand *Src0 = Producer->getSrc(0);
      Operand *Src1 = legalize(Producer->getSrc(1));
      Operand *Src0RM = legalizeSrc0ForCmp(Src0, Src1);
      _cmp(Src0RM, Src1);
      _br(Traits::getIcmp32Mapping(Cmp->getCondition()), Inst->getTargetTrue(),
          Inst->getTargetFalse());
      return;
    }
    }
  }

  Operand *Src0 = legalize(Cond, Legal_Reg | Legal_Mem);
  Constant *Zero = Ctx->getConstantZero(IceType_i32);
  _cmp(Src0, Zero);
  _br(CondX86::Br_ne, Inst->getTargetTrue(), Inst->getTargetFalse());
}

template <class Machine>
void TargetX86Base<Machine>::lowerCall(const InstCall *Instr) {
  // x86-32 calling convention:
  //
  // * At the point before the call, the stack must be aligned to 16
  // bytes.
  //
  // * The first four arguments of vector type, regardless of their
  // position relative to the other arguments in the argument list, are
  // placed in registers xmm0 - xmm3.
  //
  // * Other arguments are pushed onto the stack in right-to-left order,
  // such that the left-most argument ends up on the top of the stack at
  // the lowest memory address.
  //
  // * Stack arguments of vector type are aligned to start at the next
  // highest multiple of 16 bytes.  Other stack arguments are aligned to
  // 4 bytes.
  //
  // This intends to match the section "IA-32 Function Calling
  // Convention" of the document "OS X ABI Function Call Guide" by
  // Apple.
  NeedsStackAlignment = true;

  typedef std::vector<Operand *> OperandList;
  OperandList XmmArgs;
  OperandList StackArgs, StackArgLocations;
  uint32_t ParameterAreaSizeBytes = 0;

  // Classify each argument operand according to the location where the
  // argument is passed.
  for (SizeT i = 0, NumArgs = Instr->getNumArgs(); i < NumArgs; ++i) {
    Operand *Arg = Instr->getArg(i);
    Type Ty = Arg->getType();
    // The PNaCl ABI requires the width of arguments to be at least 32 bits.
    assert(typeWidthInBytes(Ty) >= 4);
    if (isVectorType(Ty) && XmmArgs.size() < Traits::X86_MAX_XMM_ARGS) {
      XmmArgs.push_back(Arg);
    } else {
      StackArgs.push_back(Arg);
      if (isVectorType(Arg->getType())) {
        ParameterAreaSizeBytes =
            Traits::applyStackAlignment(ParameterAreaSizeBytes);
      }
      Variable *esp = Func->getTarget()->getPhysicalRegister(RegX8632::Reg_esp);
      Constant *Loc = Ctx->getConstantInt32(ParameterAreaSizeBytes);
      StackArgLocations.push_back(OperandX8632Mem::create(Func, Ty, esp, Loc));
      ParameterAreaSizeBytes += typeWidthInBytesOnStack(Arg->getType());
    }
  }

  // Adjust the parameter area so that the stack is aligned.  It is
  // assumed that the stack is already aligned at the start of the
  // calling sequence.
  ParameterAreaSizeBytes = Traits::applyStackAlignment(ParameterAreaSizeBytes);

  // Subtract the appropriate amount for the argument area.  This also
  // takes care of setting the stack adjustment during emission.
  //
  // TODO: If for some reason the call instruction gets dead-code
  // eliminated after lowering, we would need to ensure that the
  // pre-call and the post-call esp adjustment get eliminated as well.
  if (ParameterAreaSizeBytes) {
    _adjust_stack(ParameterAreaSizeBytes);
  }

  // Copy arguments that are passed on the stack to the appropriate
  // stack locations.
  for (SizeT i = 0, e = StackArgs.size(); i < e; ++i) {
    lowerStore(InstStore::create(Func, StackArgs[i], StackArgLocations[i]));
  }

  // Copy arguments to be passed in registers to the appropriate
  // registers.
  // TODO: Investigate the impact of lowering arguments passed in
  // registers after lowering stack arguments as opposed to the other
  // way around.  Lowering register arguments after stack arguments may
  // reduce register pressure.  On the other hand, lowering register
  // arguments first (before stack arguments) may result in more compact
  // code, as the memory operand displacements may end up being smaller
  // before any stack adjustment is done.
  for (SizeT i = 0, NumXmmArgs = XmmArgs.size(); i < NumXmmArgs; ++i) {
    Variable *Reg = legalizeToVar(XmmArgs[i], RegX8632::Reg_xmm0 + i);
    // Generate a FakeUse of register arguments so that they do not get
    // dead code eliminated as a result of the FakeKill of scratch
    // registers after the call.
    Context.insert(InstFakeUse::create(Func, Reg));
  }
  // Generate the call instruction.  Assign its result to a temporary
  // with high register allocation weight.
  Variable *Dest = Instr->getDest();
  // ReturnReg doubles as ReturnRegLo as necessary.
  Variable *ReturnReg = nullptr;
  Variable *ReturnRegHi = nullptr;
  if (Dest) {
    switch (Dest->getType()) {
    case IceType_NUM:
      llvm_unreachable("Invalid Call dest type");
      break;
    case IceType_void:
      break;
    case IceType_i1:
    case IceType_i8:
    case IceType_i16:
    case IceType_i32:
      ReturnReg = makeReg(Dest->getType(), RegX8632::Reg_eax);
      break;
    case IceType_i64:
      ReturnReg = makeReg(IceType_i32, RegX8632::Reg_eax);
      ReturnRegHi = makeReg(IceType_i32, RegX8632::Reg_edx);
      break;
    case IceType_f32:
    case IceType_f64:
      // Leave ReturnReg==ReturnRegHi==nullptr, and capture the result with
      // the fstp instruction.
      break;
    case IceType_v4i1:
    case IceType_v8i1:
    case IceType_v16i1:
    case IceType_v16i8:
    case IceType_v8i16:
    case IceType_v4i32:
    case IceType_v4f32:
      ReturnReg = makeReg(Dest->getType(), RegX8632::Reg_xmm0);
      break;
    }
  }
  Operand *CallTarget = legalize(Instr->getCallTarget());
  const bool NeedSandboxing = Ctx->getFlags().getUseSandboxing();
  if (NeedSandboxing) {
    if (llvm::isa<Constant>(CallTarget)) {
      _bundle_lock(InstBundleLock::Opt_AlignToEnd);
    } else {
      Variable *CallTargetVar = nullptr;
      _mov(CallTargetVar, CallTarget);
      _bundle_lock(InstBundleLock::Opt_AlignToEnd);
      const SizeT BundleSize =
          1 << Func->template getAssembler<>()->getBundleAlignLog2Bytes();
      _and(CallTargetVar, Ctx->getConstantInt32(~(BundleSize - 1)));
      CallTarget = CallTargetVar;
    }
  }
  Inst *NewCall = InstX8632Call::create(Func, ReturnReg, CallTarget);
  Context.insert(NewCall);
  if (NeedSandboxing)
    _bundle_unlock();
  if (ReturnRegHi)
    Context.insert(InstFakeDef::create(Func, ReturnRegHi));

  // Add the appropriate offset to esp.  The call instruction takes care
  // of resetting the stack offset during emission.
  if (ParameterAreaSizeBytes) {
    Variable *esp = Func->getTarget()->getPhysicalRegister(RegX8632::Reg_esp);
    _add(esp, Ctx->getConstantInt32(ParameterAreaSizeBytes));
  }

  // Insert a register-kill pseudo instruction.
  Context.insert(InstFakeKill::create(Func, NewCall));

  // Generate a FakeUse to keep the call live if necessary.
  if (Instr->hasSideEffects() && ReturnReg) {
    Inst *FakeUse = InstFakeUse::create(Func, ReturnReg);
    Context.insert(FakeUse);
  }

  if (!Dest)
    return;

  // Assign the result of the call to Dest.
  if (ReturnReg) {
    if (ReturnRegHi) {
      assert(Dest->getType() == IceType_i64);
      split64(Dest);
      Variable *DestLo = Dest->getLo();
      Variable *DestHi = Dest->getHi();
      _mov(DestLo, ReturnReg);
      _mov(DestHi, ReturnRegHi);
    } else {
      assert(Dest->getType() == IceType_i32 || Dest->getType() == IceType_i16 ||
             Dest->getType() == IceType_i8 || Dest->getType() == IceType_i1 ||
             isVectorType(Dest->getType()));
      if (isVectorType(Dest->getType())) {
        _movp(Dest, ReturnReg);
      } else {
        _mov(Dest, ReturnReg);
      }
    }
  } else if (isScalarFloatingType(Dest->getType())) {
    // Special treatment for an FP function which returns its result in
    // st(0).
    // If Dest ends up being a physical xmm register, the fstp emit code
    // will route st(0) through a temporary stack slot.
    _fstp(Dest);
    // Create a fake use of Dest in case it actually isn't used,
    // because st(0) still needs to be popped.
    Context.insert(InstFakeUse::create(Func, Dest));
  }
}

template <class Machine>
void TargetX86Base<Machine>::lowerCast(const InstCast *Inst) {
  // a = cast(b) ==> t=cast(b); a=t; (link t->b, link a->t, no overlap)
  InstCast::OpKind CastKind = Inst->getCastKind();
  Variable *Dest = Inst->getDest();
  switch (CastKind) {
  default:
    Func->setError("Cast type not supported");
    return;
  case InstCast::Sext: {
    // Src0RM is the source operand legalized to physical register or memory,
    // but not immediate, since the relevant x86 native instructions don't
    // allow an immediate operand.  If the operand is an immediate, we could
    // consider computing the strength-reduced result at translation time,
    // but we're unlikely to see something like that in the bitcode that
    // the optimizer wouldn't have already taken care of.
    Operand *Src0RM = legalize(Inst->getSrc(0), Legal_Reg | Legal_Mem);
    if (isVectorType(Dest->getType())) {
      Type DestTy = Dest->getType();
      if (DestTy == IceType_v16i8) {
        // onemask = materialize(1,1,...); dst = (src & onemask) > 0
        Variable *OneMask = makeVectorOfOnes(Dest->getType());
        Variable *T = makeReg(DestTy);
        _movp(T, Src0RM);
        _pand(T, OneMask);
        Variable *Zeros = makeVectorOfZeros(Dest->getType());
        _pcmpgt(T, Zeros);
        _movp(Dest, T);
      } else {
        // width = width(elty) - 1; dest = (src << width) >> width
        SizeT ShiftAmount =
            Traits::X86_CHAR_BIT * typeWidthInBytes(typeElementType(DestTy)) -
            1;
        Constant *ShiftConstant = Ctx->getConstantInt8(ShiftAmount);
        Variable *T = makeReg(DestTy);
        _movp(T, Src0RM);
        _psll(T, ShiftConstant);
        _psra(T, ShiftConstant);
        _movp(Dest, T);
      }
    } else if (Dest->getType() == IceType_i64) {
      // t1=movsx src; t2=t1; t2=sar t2, 31; dst.lo=t1; dst.hi=t2
      Constant *Shift = Ctx->getConstantInt32(31);
      Variable *DestLo = llvm::cast<Variable>(loOperand(Dest));
      Variable *DestHi = llvm::cast<Variable>(hiOperand(Dest));
      Variable *T_Lo = makeReg(DestLo->getType());
      if (Src0RM->getType() == IceType_i32) {
        _mov(T_Lo, Src0RM);
      } else if (Src0RM->getType() == IceType_i1) {
        _movzx(T_Lo, Src0RM);
        _shl(T_Lo, Shift);
        _sar(T_Lo, Shift);
      } else {
        _movsx(T_Lo, Src0RM);
      }
      _mov(DestLo, T_Lo);
      Variable *T_Hi = nullptr;
      _mov(T_Hi, T_Lo);
      if (Src0RM->getType() != IceType_i1)
        // For i1, the sar instruction is already done above.
        _sar(T_Hi, Shift);
      _mov(DestHi, T_Hi);
    } else if (Src0RM->getType() == IceType_i1) {
      // t1 = src
      // shl t1, dst_bitwidth - 1
      // sar t1, dst_bitwidth - 1
      // dst = t1
      size_t DestBits =
          Traits::X86_CHAR_BIT * typeWidthInBytes(Dest->getType());
      Constant *ShiftAmount = Ctx->getConstantInt32(DestBits - 1);
      Variable *T = makeReg(Dest->getType());
      if (typeWidthInBytes(Dest->getType()) <=
          typeWidthInBytes(Src0RM->getType())) {
        _mov(T, Src0RM);
      } else {
        // Widen the source using movsx or movzx.  (It doesn't matter
        // which one, since the following shl/sar overwrite the bits.)
        _movzx(T, Src0RM);
      }
      _shl(T, ShiftAmount);
      _sar(T, ShiftAmount);
      _mov(Dest, T);
    } else {
      // t1 = movsx src; dst = t1
      Variable *T = makeReg(Dest->getType());
      _movsx(T, Src0RM);
      _mov(Dest, T);
    }
    break;
  }
  case InstCast::Zext: {
    Operand *Src0RM = legalize(Inst->getSrc(0), Legal_Reg | Legal_Mem);
    if (isVectorType(Dest->getType())) {
      // onemask = materialize(1,1,...); dest = onemask & src
      Type DestTy = Dest->getType();
      Variable *OneMask = makeVectorOfOnes(DestTy);
      Variable *T = makeReg(DestTy);
      _movp(T, Src0RM);
      _pand(T, OneMask);
      _movp(Dest, T);
    } else if (Dest->getType() == IceType_i64) {
      // t1=movzx src; dst.lo=t1; dst.hi=0
      Constant *Zero = Ctx->getConstantZero(IceType_i32);
      Variable *DestLo = llvm::cast<Variable>(loOperand(Dest));
      Variable *DestHi = llvm::cast<Variable>(hiOperand(Dest));
      Variable *Tmp = makeReg(DestLo->getType());
      if (Src0RM->getType() == IceType_i32) {
        _mov(Tmp, Src0RM);
      } else {
        _movzx(Tmp, Src0RM);
      }
      if (Src0RM->getType() == IceType_i1) {
        Constant *One = Ctx->getConstantInt32(1);
        _and(Tmp, One);
      }
      _mov(DestLo, Tmp);
      _mov(DestHi, Zero);
    } else if (Src0RM->getType() == IceType_i1) {
      // t = Src0RM; t &= 1; Dest = t
      Constant *One = Ctx->getConstantInt32(1);
      Type DestTy = Dest->getType();
      Variable *T;
      if (DestTy == IceType_i8) {
        T = makeReg(DestTy);
        _mov(T, Src0RM);
      } else {
        // Use 32-bit for both 16-bit and 32-bit, since 32-bit ops are shorter.
        T = makeReg(IceType_i32);
        _movzx(T, Src0RM);
      }
      _and(T, One);
      _mov(Dest, T);
    } else {
      // t1 = movzx src; dst = t1
      Variable *T = makeReg(Dest->getType());
      _movzx(T, Src0RM);
      _mov(Dest, T);
    }
    break;
  }
  case InstCast::Trunc: {
    if (isVectorType(Dest->getType())) {
      // onemask = materialize(1,1,...); dst = src & onemask
      Operand *Src0RM = legalize(Inst->getSrc(0), Legal_Reg | Legal_Mem);
      Type Src0Ty = Src0RM->getType();
      Variable *OneMask = makeVectorOfOnes(Src0Ty);
      Variable *T = makeReg(Dest->getType());
      _movp(T, Src0RM);
      _pand(T, OneMask);
      _movp(Dest, T);
    } else {
      Operand *Src0 = Inst->getSrc(0);
      if (Src0->getType() == IceType_i64)
        Src0 = loOperand(Src0);
      Operand *Src0RM = legalize(Src0, Legal_Reg | Legal_Mem);
      // t1 = trunc Src0RM; Dest = t1
      Variable *T = nullptr;
      _mov(T, Src0RM);
      if (Dest->getType() == IceType_i1)
        _and(T, Ctx->getConstantInt1(1));
      _mov(Dest, T);
    }
    break;
  }
  case InstCast::Fptrunc:
  case InstCast::Fpext: {
    Operand *Src0RM = legalize(Inst->getSrc(0), Legal_Reg | Legal_Mem);
    // t1 = cvt Src0RM; Dest = t1
    Variable *T = makeReg(Dest->getType());
    _cvt(T, Src0RM, InstX8632Cvt::Float2float);
    _mov(Dest, T);
    break;
  }
  case InstCast::Fptosi:
    if (isVectorType(Dest->getType())) {
      assert(Dest->getType() == IceType_v4i32 &&
             Inst->getSrc(0)->getType() == IceType_v4f32);
      Operand *Src0RM = legalize(Inst->getSrc(0), Legal_Reg | Legal_Mem);
      if (llvm::isa<OperandX8632Mem>(Src0RM))
        Src0RM = legalizeToVar(Src0RM);
      Variable *T = makeReg(Dest->getType());
      _cvt(T, Src0RM, InstX8632Cvt::Tps2dq);
      _movp(Dest, T);
    } else if (Dest->getType() == IceType_i64) {
      // Use a helper for converting floating-point values to 64-bit
      // integers.  SSE2 appears to have no way to convert from xmm
      // registers to something like the edx:eax register pair, and
      // gcc and clang both want to use x87 instructions complete with
      // temporary manipulation of the status word.  This helper is
      // not needed for x86-64.
      split64(Dest);
      const SizeT MaxSrcs = 1;
      Type SrcType = Inst->getSrc(0)->getType();
      InstCall *Call =
          makeHelperCall(isFloat32Asserting32Or64(SrcType) ? H_fptosi_f32_i64
                                                           : H_fptosi_f64_i64,
                         Dest, MaxSrcs);
      Call->addArg(Inst->getSrc(0));
      lowerCall(Call);
    } else {
      Operand *Src0RM = legalize(Inst->getSrc(0), Legal_Reg | Legal_Mem);
      // t1.i32 = cvt Src0RM; t2.dest_type = t1; Dest = t2.dest_type
      Variable *T_1 = makeReg(IceType_i32);
      Variable *T_2 = makeReg(Dest->getType());
      _cvt(T_1, Src0RM, InstX8632Cvt::Tss2si);
      _mov(T_2, T_1); // T_1 and T_2 may have different integer types
      if (Dest->getType() == IceType_i1)
        _and(T_2, Ctx->getConstantInt1(1));
      _mov(Dest, T_2);
    }
    break;
  case InstCast::Fptoui:
    if (isVectorType(Dest->getType())) {
      assert(Dest->getType() == IceType_v4i32 &&
             Inst->getSrc(0)->getType() == IceType_v4f32);
      const SizeT MaxSrcs = 1;
      InstCall *Call = makeHelperCall(H_fptoui_4xi32_f32, Dest, MaxSrcs);
      Call->addArg(Inst->getSrc(0));
      lowerCall(Call);
    } else if (Dest->getType() == IceType_i64 ||
               Dest->getType() == IceType_i32) {
      // Use a helper for both x86-32 and x86-64.
      split64(Dest);
      const SizeT MaxSrcs = 1;
      Type DestType = Dest->getType();
      Type SrcType = Inst->getSrc(0)->getType();
      IceString TargetString;
      if (isInt32Asserting32Or64(DestType)) {
        TargetString = isFloat32Asserting32Or64(SrcType) ? H_fptoui_f32_i32
                                                         : H_fptoui_f64_i32;
      } else {
        TargetString = isFloat32Asserting32Or64(SrcType) ? H_fptoui_f32_i64
                                                         : H_fptoui_f64_i64;
      }
      InstCall *Call = makeHelperCall(TargetString, Dest, MaxSrcs);
      Call->addArg(Inst->getSrc(0));
      lowerCall(Call);
      return;
    } else {
      Operand *Src0RM = legalize(Inst->getSrc(0), Legal_Reg | Legal_Mem);
      // t1.i32 = cvt Src0RM; t2.dest_type = t1; Dest = t2.dest_type
      Variable *T_1 = makeReg(IceType_i32);
      Variable *T_2 = makeReg(Dest->getType());
      _cvt(T_1, Src0RM, InstX8632Cvt::Tss2si);
      _mov(T_2, T_1); // T_1 and T_2 may have different integer types
      if (Dest->getType() == IceType_i1)
        _and(T_2, Ctx->getConstantInt1(1));
      _mov(Dest, T_2);
    }
    break;
  case InstCast::Sitofp:
    if (isVectorType(Dest->getType())) {
      assert(Dest->getType() == IceType_v4f32 &&
             Inst->getSrc(0)->getType() == IceType_v4i32);
      Operand *Src0RM = legalize(Inst->getSrc(0), Legal_Reg | Legal_Mem);
      if (llvm::isa<OperandX8632Mem>(Src0RM))
        Src0RM = legalizeToVar(Src0RM);
      Variable *T = makeReg(Dest->getType());
      _cvt(T, Src0RM, InstX8632Cvt::Dq2ps);
      _movp(Dest, T);
    } else if (Inst->getSrc(0)->getType() == IceType_i64) {
      // Use a helper for x86-32.
      const SizeT MaxSrcs = 1;
      Type DestType = Dest->getType();
      InstCall *Call =
          makeHelperCall(isFloat32Asserting32Or64(DestType) ? H_sitofp_i64_f32
                                                            : H_sitofp_i64_f64,
                         Dest, MaxSrcs);
      // TODO: Call the correct compiler-rt helper function.
      Call->addArg(Inst->getSrc(0));
      lowerCall(Call);
      return;
    } else {
      Operand *Src0RM = legalize(Inst->getSrc(0), Legal_Reg | Legal_Mem);
      // Sign-extend the operand.
      // t1.i32 = movsx Src0RM; t2 = Cvt t1.i32; Dest = t2
      Variable *T_1 = makeReg(IceType_i32);
      Variable *T_2 = makeReg(Dest->getType());
      if (Src0RM->getType() == IceType_i32)
        _mov(T_1, Src0RM);
      else
        _movsx(T_1, Src0RM);
      _cvt(T_2, T_1, InstX8632Cvt::Si2ss);
      _mov(Dest, T_2);
    }
    break;
  case InstCast::Uitofp: {
    Operand *Src0 = Inst->getSrc(0);
    if (isVectorType(Src0->getType())) {
      assert(Dest->getType() == IceType_v4f32 &&
             Src0->getType() == IceType_v4i32);
      const SizeT MaxSrcs = 1;
      InstCall *Call = makeHelperCall(H_uitofp_4xi32_4xf32, Dest, MaxSrcs);
      Call->addArg(Src0);
      lowerCall(Call);
    } else if (Src0->getType() == IceType_i64 ||
               Src0->getType() == IceType_i32) {
      // Use a helper for x86-32 and x86-64.  Also use a helper for
      // i32 on x86-32.
      const SizeT MaxSrcs = 1;
      Type DestType = Dest->getType();
      IceString TargetString;
      if (isInt32Asserting32Or64(Src0->getType())) {
        TargetString = isFloat32Asserting32Or64(DestType) ? H_uitofp_i32_f32
                                                          : H_uitofp_i32_f64;
      } else {
        TargetString = isFloat32Asserting32Or64(DestType) ? H_uitofp_i64_f32
                                                          : H_uitofp_i64_f64;
      }
      InstCall *Call = makeHelperCall(TargetString, Dest, MaxSrcs);
      Call->addArg(Src0);
      lowerCall(Call);
      return;
    } else {
      Operand *Src0RM = legalize(Src0, Legal_Reg | Legal_Mem);
      // Zero-extend the operand.
      // t1.i32 = movzx Src0RM; t2 = Cvt t1.i32; Dest = t2
      Variable *T_1 = makeReg(IceType_i32);
      Variable *T_2 = makeReg(Dest->getType());
      if (Src0RM->getType() == IceType_i32)
        _mov(T_1, Src0RM);
      else
        _movzx(T_1, Src0RM);
      _cvt(T_2, T_1, InstX8632Cvt::Si2ss);
      _mov(Dest, T_2);
    }
    break;
  }
  case InstCast::Bitcast: {
    Operand *Src0 = Inst->getSrc(0);
    if (Dest->getType() == Src0->getType()) {
      InstAssign *Assign = InstAssign::create(Func, Dest, Src0);
      lowerAssign(Assign);
      return;
    }
    switch (Dest->getType()) {
    default:
      llvm_unreachable("Unexpected Bitcast dest type");
    case IceType_i8: {
      assert(Src0->getType() == IceType_v8i1);
      InstCall *Call = makeHelperCall(H_bitcast_8xi1_i8, Dest, 1);
      Call->addArg(Src0);
      lowerCall(Call);
    } break;
    case IceType_i16: {
      assert(Src0->getType() == IceType_v16i1);
      InstCall *Call = makeHelperCall(H_bitcast_16xi1_i16, Dest, 1);
      Call->addArg(Src0);
      lowerCall(Call);
    } break;
    case IceType_i32:
    case IceType_f32: {
      Operand *Src0RM = legalize(Src0, Legal_Reg | Legal_Mem);
      Type DestType = Dest->getType();
      Type SrcType = Src0RM->getType();
      (void)DestType;
      assert((DestType == IceType_i32 && SrcType == IceType_f32) ||
             (DestType == IceType_f32 && SrcType == IceType_i32));
      // a.i32 = bitcast b.f32 ==>
      //   t.f32 = b.f32
      //   s.f32 = spill t.f32
      //   a.i32 = s.f32
      Variable *T = nullptr;
      // TODO: Should be able to force a spill setup by calling legalize() with
      // Legal_Mem and not Legal_Reg or Legal_Imm.
      SpillVariable *SpillVar =
          Func->template makeVariable<SpillVariable>(SrcType);
      SpillVar->setLinkedTo(Dest);
      Variable *Spill = SpillVar;
      Spill->setWeight(RegWeight::Zero);
      _mov(T, Src0RM);
      _mov(Spill, T);
      _mov(Dest, Spill);
    } break;
    case IceType_i64: {
      Operand *Src0RM = legalize(Src0, Legal_Reg | Legal_Mem);
      assert(Src0RM->getType() == IceType_f64);
      // a.i64 = bitcast b.f64 ==>
      //   s.f64 = spill b.f64
      //   t_lo.i32 = lo(s.f64)
      //   a_lo.i32 = t_lo.i32
      //   t_hi.i32 = hi(s.f64)
      //   a_hi.i32 = t_hi.i32
      Operand *SpillLo, *SpillHi;
      if (auto *Src0Var = llvm::dyn_cast<Variable>(Src0RM)) {
        SpillVariable *SpillVar =
            Func->template makeVariable<SpillVariable>(IceType_f64);
        SpillVar->setLinkedTo(Src0Var);
        Variable *Spill = SpillVar;
        Spill->setWeight(RegWeight::Zero);
        _movq(Spill, Src0RM);
        SpillLo = VariableSplit::create(Func, Spill, VariableSplit::Low);
        SpillHi = VariableSplit::create(Func, Spill, VariableSplit::High);
      } else {
        SpillLo = loOperand(Src0RM);
        SpillHi = hiOperand(Src0RM);
      }

      Variable *DestLo = llvm::cast<Variable>(loOperand(Dest));
      Variable *DestHi = llvm::cast<Variable>(hiOperand(Dest));
      Variable *T_Lo = makeReg(IceType_i32);
      Variable *T_Hi = makeReg(IceType_i32);

      _mov(T_Lo, SpillLo);
      _mov(DestLo, T_Lo);
      _mov(T_Hi, SpillHi);
      _mov(DestHi, T_Hi);
    } break;
    case IceType_f64: {
      Src0 = legalize(Src0);
      assert(Src0->getType() == IceType_i64);
      if (llvm::isa<OperandX8632Mem>(Src0)) {
        Variable *T = Func->template makeVariable(Dest->getType());
        _movq(T, Src0);
        _movq(Dest, T);
        break;
      }
      // a.f64 = bitcast b.i64 ==>
      //   t_lo.i32 = b_lo.i32
      //   FakeDef(s.f64)
      //   lo(s.f64) = t_lo.i32
      //   t_hi.i32 = b_hi.i32
      //   hi(s.f64) = t_hi.i32
      //   a.f64 = s.f64
      SpillVariable *SpillVar =
          Func->template makeVariable<SpillVariable>(IceType_f64);
      SpillVar->setLinkedTo(Dest);
      Variable *Spill = SpillVar;
      Spill->setWeight(RegWeight::Zero);

      Variable *T_Lo = nullptr, *T_Hi = nullptr;
      VariableSplit *SpillLo =
          VariableSplit::create(Func, Spill, VariableSplit::Low);
      VariableSplit *SpillHi =
          VariableSplit::create(Func, Spill, VariableSplit::High);
      _mov(T_Lo, loOperand(Src0));
      // Technically, the Spill is defined after the _store happens, but
      // SpillLo is considered a "use" of Spill so define Spill before it
      // is used.
      Context.insert(InstFakeDef::create(Func, Spill));
      _store(T_Lo, SpillLo);
      _mov(T_Hi, hiOperand(Src0));
      _store(T_Hi, SpillHi);
      _movq(Dest, Spill);
    } break;
    case IceType_v8i1: {
      assert(Src0->getType() == IceType_i8);
      InstCall *Call = makeHelperCall(H_bitcast_i8_8xi1, Dest, 1);
      Variable *Src0AsI32 = Func->template makeVariable(stackSlotType());
      // Arguments to functions are required to be at least 32 bits wide.
      lowerCast(InstCast::create(Func, InstCast::Zext, Src0AsI32, Src0));
      Call->addArg(Src0AsI32);
      lowerCall(Call);
    } break;
    case IceType_v16i1: {
      assert(Src0->getType() == IceType_i16);
      InstCall *Call = makeHelperCall(H_bitcast_i16_16xi1, Dest, 1);
      Variable *Src0AsI32 = Func->template makeVariable(stackSlotType());
      // Arguments to functions are required to be at least 32 bits wide.
      lowerCast(InstCast::create(Func, InstCast::Zext, Src0AsI32, Src0));
      Call->addArg(Src0AsI32);
      lowerCall(Call);
    } break;
    case IceType_v8i16:
    case IceType_v16i8:
    case IceType_v4i32:
    case IceType_v4f32: {
      _movp(Dest, legalizeToVar(Src0));
    } break;
    }
    break;
  }
  }
}

template <class Machine>
void TargetX86Base<Machine>::lowerExtractElement(
    const InstExtractElement *Inst) {
  Operand *SourceVectNotLegalized = Inst->getSrc(0);
  ConstantInteger32 *ElementIndex =
      llvm::dyn_cast<ConstantInteger32>(Inst->getSrc(1));
  // Only constant indices are allowed in PNaCl IR.
  assert(ElementIndex);

  unsigned Index = ElementIndex->getValue();
  Type Ty = SourceVectNotLegalized->getType();
  Type ElementTy = typeElementType(Ty);
  Type InVectorElementTy = Traits::getInVectorElementType(Ty);
  Variable *ExtractedElementR = makeReg(InVectorElementTy);

  // TODO(wala): Determine the best lowering sequences for each type.
  bool CanUsePextr = Ty == IceType_v8i16 || Ty == IceType_v8i1 ||
                     InstructionSet >= Machine::SSE4_1;
  if (CanUsePextr && Ty != IceType_v4f32) {
    // Use pextrb, pextrw, or pextrd.
    Constant *Mask = Ctx->getConstantInt32(Index);
    Variable *SourceVectR = legalizeToVar(SourceVectNotLegalized);
    _pextr(ExtractedElementR, SourceVectR, Mask);
  } else if (Ty == IceType_v4i32 || Ty == IceType_v4f32 || Ty == IceType_v4i1) {
    // Use pshufd and movd/movss.
    Variable *T = nullptr;
    if (Index) {
      // The shuffle only needs to occur if the element to be extracted
      // is not at the lowest index.
      Constant *Mask = Ctx->getConstantInt32(Index);
      T = makeReg(Ty);
      _pshufd(T, legalize(SourceVectNotLegalized, Legal_Reg | Legal_Mem), Mask);
    } else {
      T = legalizeToVar(SourceVectNotLegalized);
    }

    if (InVectorElementTy == IceType_i32) {
      _movd(ExtractedElementR, T);
    } else { // Ty == IceType_f32
      // TODO(wala): _movss is only used here because _mov does not
      // allow a vector source and a scalar destination.  _mov should be
      // able to be used here.
      // _movss is a binary instruction, so the FakeDef is needed to
      // keep the live range analysis consistent.
      Context.insert(InstFakeDef::create(Func, ExtractedElementR));
      _movss(ExtractedElementR, T);
    }
  } else {
    assert(Ty == IceType_v16i8 || Ty == IceType_v16i1);
    // Spill the value to a stack slot and do the extraction in memory.
    //
    // TODO(wala): use legalize(SourceVectNotLegalized, Legal_Mem) when
    // support for legalizing to mem is implemented.
    Variable *Slot = Func->template makeVariable(Ty);
    Slot->setWeight(RegWeight::Zero);
    _movp(Slot, legalizeToVar(SourceVectNotLegalized));

    // Compute the location of the element in memory.
    unsigned Offset = Index * typeWidthInBytes(InVectorElementTy);
    OperandX8632Mem *Loc =
        getMemoryOperandForStackSlot(InVectorElementTy, Slot, Offset);
    _mov(ExtractedElementR, Loc);
  }

  if (ElementTy == IceType_i1) {
    // Truncate extracted integers to i1s if necessary.
    Variable *T = makeReg(IceType_i1);
    InstCast *Cast =
        InstCast::create(Func, InstCast::Trunc, T, ExtractedElementR);
    lowerCast(Cast);
    ExtractedElementR = T;
  }

  // Copy the element to the destination.
  Variable *Dest = Inst->getDest();
  _mov(Dest, ExtractedElementR);
}

template <class Machine>
void TargetX86Base<Machine>::lowerFcmp(const InstFcmp *Inst) {
  Operand *Src0 = Inst->getSrc(0);
  Operand *Src1 = Inst->getSrc(1);
  Variable *Dest = Inst->getDest();

  if (isVectorType(Dest->getType())) {
    InstFcmp::FCond Condition = Inst->getCondition();
    size_t Index = static_cast<size_t>(Condition);
    assert(Index < Traits::TableFcmpSize);

    if (Traits::TableFcmp[Index].SwapVectorOperands) {
      Operand *T = Src0;
      Src0 = Src1;
      Src1 = T;
    }

    Variable *T = nullptr;

    if (Condition == InstFcmp::True) {
      // makeVectorOfOnes() requires an integer vector type.
      T = makeVectorOfMinusOnes(IceType_v4i32);
    } else if (Condition == InstFcmp::False) {
      T = makeVectorOfZeros(Dest->getType());
    } else {
      Operand *Src0RM = legalize(Src0, Legal_Reg | Legal_Mem);
      Operand *Src1RM = legalize(Src1, Legal_Reg | Legal_Mem);
      if (llvm::isa<OperandX8632Mem>(Src1RM))
        Src1RM = legalizeToVar(Src1RM);

      switch (Condition) {
      default: {
        CondX86::CmppsCond Predicate = Traits::TableFcmp[Index].Predicate;
        assert(Predicate != CondX86::Cmpps_Invalid);
        T = makeReg(Src0RM->getType());
        _movp(T, Src0RM);
        _cmpps(T, Src1RM, Predicate);
      } break;
      case InstFcmp::One: {
        // Check both unequal and ordered.
        T = makeReg(Src0RM->getType());
        Variable *T2 = makeReg(Src0RM->getType());
        _movp(T, Src0RM);
        _cmpps(T, Src1RM, CondX86::Cmpps_neq);
        _movp(T2, Src0RM);
        _cmpps(T2, Src1RM, CondX86::Cmpps_ord);
        _pand(T, T2);
      } break;
      case InstFcmp::Ueq: {
        // Check both equal or unordered.
        T = makeReg(Src0RM->getType());
        Variable *T2 = makeReg(Src0RM->getType());
        _movp(T, Src0RM);
        _cmpps(T, Src1RM, CondX86::Cmpps_eq);
        _movp(T2, Src0RM);
        _cmpps(T2, Src1RM, CondX86::Cmpps_unord);
        _por(T, T2);
      } break;
      }
    }

    _movp(Dest, T);
    eliminateNextVectorSextInstruction(Dest);
    return;
  }

  // Lowering a = fcmp cond, b, c
  //   ucomiss b, c       /* only if C1 != Br_None */
  //                      /* but swap b,c order if SwapOperands==true */
  //   mov a, <default>
  //   j<C1> label        /* only if C1 != Br_None */
  //   j<C2> label        /* only if C2 != Br_None */
  //   FakeUse(a)         /* only if C1 != Br_None */
  //   mov a, !<default>  /* only if C1 != Br_None */
  //   label:             /* only if C1 != Br_None */
  //
  // setcc lowering when C1 != Br_None && C2 == Br_None:
  //   ucomiss b, c       /* but swap b,c order if SwapOperands==true */
  //   setcc a, C1
  InstFcmp::FCond Condition = Inst->getCondition();
  size_t Index = static_cast<size_t>(Condition);
  assert(Index < Traits::TableFcmpSize);
  if (Traits::TableFcmp[Index].SwapScalarOperands)
    std::swap(Src0, Src1);
  bool HasC1 = (Traits::TableFcmp[Index].C1 != CondX86::Br_None);
  bool HasC2 = (Traits::TableFcmp[Index].C2 != CondX86::Br_None);
  if (HasC1) {
    Src0 = legalize(Src0);
    Operand *Src1RM = legalize(Src1, Legal_Reg | Legal_Mem);
    Variable *T = nullptr;
    _mov(T, Src0);
    _ucomiss(T, Src1RM);
    if (!HasC2) {
      assert(Traits::TableFcmp[Index].Default);
      _setcc(Dest, Traits::TableFcmp[Index].C1);
      return;
    }
  }
  Constant *Default = Ctx->getConstantInt32(Traits::TableFcmp[Index].Default);
  _mov(Dest, Default);
  if (HasC1) {
    InstX8632Label *Label = InstX8632Label::create(Func, this);
    _br(Traits::TableFcmp[Index].C1, Label);
    if (HasC2) {
      _br(Traits::TableFcmp[Index].C2, Label);
    }
    Constant *NonDefault =
        Ctx->getConstantInt32(!Traits::TableFcmp[Index].Default);
    _mov_nonkillable(Dest, NonDefault);
    Context.insert(Label);
  }
}

template <class Machine>
void TargetX86Base<Machine>::lowerIcmp(const InstIcmp *Inst) {
  Operand *Src0 = legalize(Inst->getSrc(0));
  Operand *Src1 = legalize(Inst->getSrc(1));
  Variable *Dest = Inst->getDest();

  if (isVectorType(Dest->getType())) {
    Type Ty = Src0->getType();
    // Promote i1 vectors to 128 bit integer vector types.
    if (typeElementType(Ty) == IceType_i1) {
      Type NewTy = IceType_NUM;
      switch (Ty) {
      default:
        llvm_unreachable("unexpected type");
        break;
      case IceType_v4i1:
        NewTy = IceType_v4i32;
        break;
      case IceType_v8i1:
        NewTy = IceType_v8i16;
        break;
      case IceType_v16i1:
        NewTy = IceType_v16i8;
        break;
      }
      Variable *NewSrc0 = Func->template makeVariable(NewTy);
      Variable *NewSrc1 = Func->template makeVariable(NewTy);
      lowerCast(InstCast::create(Func, InstCast::Sext, NewSrc0, Src0));
      lowerCast(InstCast::create(Func, InstCast::Sext, NewSrc1, Src1));
      Src0 = NewSrc0;
      Src1 = NewSrc1;
      Ty = NewTy;
    }

    InstIcmp::ICond Condition = Inst->getCondition();

    Operand *Src0RM = legalize(Src0, Legal_Reg | Legal_Mem);
    Operand *Src1RM = legalize(Src1, Legal_Reg | Legal_Mem);

    // SSE2 only has signed comparison operations.  Transform unsigned
    // inputs in a manner that allows for the use of signed comparison
    // operations by flipping the high order bits.
    if (Condition == InstIcmp::Ugt || Condition == InstIcmp::Uge ||
        Condition == InstIcmp::Ult || Condition == InstIcmp::Ule) {
      Variable *T0 = makeReg(Ty);
      Variable *T1 = makeReg(Ty);
      Variable *HighOrderBits = makeVectorOfHighOrderBits(Ty);
      _movp(T0, Src0RM);
      _pxor(T0, HighOrderBits);
      _movp(T1, Src1RM);
      _pxor(T1, HighOrderBits);
      Src0RM = T0;
      Src1RM = T1;
    }

    Variable *T = makeReg(Ty);
    switch (Condition) {
    default:
      llvm_unreachable("unexpected condition");
      break;
    case InstIcmp::Eq: {
      if (llvm::isa<OperandX8632Mem>(Src1RM))
        Src1RM = legalizeToVar(Src1RM);
      _movp(T, Src0RM);
      _pcmpeq(T, Src1RM);
    } break;
    case InstIcmp::Ne: {
      if (llvm::isa<OperandX8632Mem>(Src1RM))
        Src1RM = legalizeToVar(Src1RM);
      _movp(T, Src0RM);
      _pcmpeq(T, Src1RM);
      Variable *MinusOne = makeVectorOfMinusOnes(Ty);
      _pxor(T, MinusOne);
    } break;
    case InstIcmp::Ugt:
    case InstIcmp::Sgt: {
      if (llvm::isa<OperandX8632Mem>(Src1RM))
        Src1RM = legalizeToVar(Src1RM);
      _movp(T, Src0RM);
      _pcmpgt(T, Src1RM);
    } break;
    case InstIcmp::Uge:
    case InstIcmp::Sge: {
      // !(Src1RM > Src0RM)
      if (llvm::isa<OperandX8632Mem>(Src0RM))
        Src0RM = legalizeToVar(Src0RM);
      _movp(T, Src1RM);
      _pcmpgt(T, Src0RM);
      Variable *MinusOne = makeVectorOfMinusOnes(Ty);
      _pxor(T, MinusOne);
    } break;
    case InstIcmp::Ult:
    case InstIcmp::Slt: {
      if (llvm::isa<OperandX8632Mem>(Src0RM))
        Src0RM = legalizeToVar(Src0RM);
      _movp(T, Src1RM);
      _pcmpgt(T, Src0RM);
    } break;
    case InstIcmp::Ule:
    case InstIcmp::Sle: {
      // !(Src0RM > Src1RM)
      if (llvm::isa<OperandX8632Mem>(Src1RM))
        Src1RM = legalizeToVar(Src1RM);
      _movp(T, Src0RM);
      _pcmpgt(T, Src1RM);
      Variable *MinusOne = makeVectorOfMinusOnes(Ty);
      _pxor(T, MinusOne);
    } break;
    }

    _movp(Dest, T);
    eliminateNextVectorSextInstruction(Dest);
    return;
  }

  // a=icmp cond, b, c ==> cmp b,c; a=1; br cond,L1; FakeUse(a); a=0; L1:
  if (Src0->getType() == IceType_i64) {
    InstIcmp::ICond Condition = Inst->getCondition();
    size_t Index = static_cast<size_t>(Condition);
    assert(Index < Traits::TableIcmp64Size);
    Operand *Src0LoRM = legalize(loOperand(Src0), Legal_Reg | Legal_Mem);
    Operand *Src0HiRM = legalize(hiOperand(Src0), Legal_Reg | Legal_Mem);
    Operand *Src1LoRI = legalize(loOperand(Src1), Legal_Reg | Legal_Imm);
    Operand *Src1HiRI = legalize(hiOperand(Src1), Legal_Reg | Legal_Imm);
    Constant *Zero = Ctx->getConstantZero(IceType_i32);
    Constant *One = Ctx->getConstantInt32(1);
    InstX8632Label *LabelFalse = InstX8632Label::create(Func, this);
    InstX8632Label *LabelTrue = InstX8632Label::create(Func, this);
    _mov(Dest, One);
    _cmp(Src0HiRM, Src1HiRI);
    if (Traits::TableIcmp64[Index].C1 != CondX86::Br_None)
      _br(Traits::TableIcmp64[Index].C1, LabelTrue);
    if (Traits::TableIcmp64[Index].C2 != CondX86::Br_None)
      _br(Traits::TableIcmp64[Index].C2, LabelFalse);
    _cmp(Src0LoRM, Src1LoRI);
    _br(Traits::TableIcmp64[Index].C3, LabelTrue);
    Context.insert(LabelFalse);
    _mov_nonkillable(Dest, Zero);
    Context.insert(LabelTrue);
    return;
  }

  // cmp b, c
  Operand *Src0RM = legalizeSrc0ForCmp(Src0, Src1);
  _cmp(Src0RM, Src1);
  _setcc(Dest, Traits::getIcmp32Mapping(Inst->getCondition()));
}

template <class Machine>
void TargetX86Base<Machine>::lowerInsertElement(const InstInsertElement *Inst) {
  Operand *SourceVectNotLegalized = Inst->getSrc(0);
  Operand *ElementToInsertNotLegalized = Inst->getSrc(1);
  ConstantInteger32 *ElementIndex =
      llvm::dyn_cast<ConstantInteger32>(Inst->getSrc(2));
  // Only constant indices are allowed in PNaCl IR.
  assert(ElementIndex);
  unsigned Index = ElementIndex->getValue();
  assert(Index < typeNumElements(SourceVectNotLegalized->getType()));

  Type Ty = SourceVectNotLegalized->getType();
  Type ElementTy = typeElementType(Ty);
  Type InVectorElementTy = Traits::getInVectorElementType(Ty);

  if (ElementTy == IceType_i1) {
    // Expand the element to the appropriate size for it to be inserted
    // in the vector.
    Variable *Expanded = Func->template makeVariable(InVectorElementTy);
    InstCast *Cast = InstCast::create(Func, InstCast::Zext, Expanded,
                                      ElementToInsertNotLegalized);
    lowerCast(Cast);
    ElementToInsertNotLegalized = Expanded;
  }

  if (Ty == IceType_v8i16 || Ty == IceType_v8i1 ||
      InstructionSet >= Machine::SSE4_1) {
    // Use insertps, pinsrb, pinsrw, or pinsrd.
    Operand *ElementRM =
        legalize(ElementToInsertNotLegalized, Legal_Reg | Legal_Mem);
    Operand *SourceVectRM =
        legalize(SourceVectNotLegalized, Legal_Reg | Legal_Mem);
    Variable *T = makeReg(Ty);
    _movp(T, SourceVectRM);
    if (Ty == IceType_v4f32)
      _insertps(T, ElementRM, Ctx->getConstantInt32(Index << 4));
    else
      _pinsr(T, ElementRM, Ctx->getConstantInt32(Index));
    _movp(Inst->getDest(), T);
  } else if (Ty == IceType_v4i32 || Ty == IceType_v4f32 || Ty == IceType_v4i1) {
    // Use shufps or movss.
    Variable *ElementR = nullptr;
    Operand *SourceVectRM =
        legalize(SourceVectNotLegalized, Legal_Reg | Legal_Mem);

    if (InVectorElementTy == IceType_f32) {
      // ElementR will be in an XMM register since it is floating point.
      ElementR = legalizeToVar(ElementToInsertNotLegalized);
    } else {
      // Copy an integer to an XMM register.
      Operand *T = legalize(ElementToInsertNotLegalized, Legal_Reg | Legal_Mem);
      ElementR = makeReg(Ty);
      _movd(ElementR, T);
    }

    if (Index == 0) {
      Variable *T = makeReg(Ty);
      _movp(T, SourceVectRM);
      _movss(T, ElementR);
      _movp(Inst->getDest(), T);
      return;
    }

    // shufps treats the source and desination operands as vectors of
    // four doublewords.  The destination's two high doublewords are
    // selected from the source operand and the two low doublewords are
    // selected from the (original value of) the destination operand.
    // An insertelement operation can be effected with a sequence of two
    // shufps operations with appropriate masks.  In all cases below,
    // Element[0] is being inserted into SourceVectOperand.  Indices are
    // ordered from left to right.
    //
    // insertelement into index 1 (result is stored in ElementR):
    //   ElementR := ElementR[0, 0] SourceVectRM[0, 0]
    //   ElementR := ElementR[3, 0] SourceVectRM[2, 3]
    //
    // insertelement into index 2 (result is stored in T):
    //   T := SourceVectRM
    //   ElementR := ElementR[0, 0] T[0, 3]
    //   T := T[0, 1] ElementR[0, 3]
    //
    // insertelement into index 3 (result is stored in T):
    //   T := SourceVectRM
    //   ElementR := ElementR[0, 0] T[0, 2]
    //   T := T[0, 1] ElementR[3, 0]
    const unsigned char Mask1[3] = {0, 192, 128};
    const unsigned char Mask2[3] = {227, 196, 52};

    Constant *Mask1Constant = Ctx->getConstantInt32(Mask1[Index - 1]);
    Constant *Mask2Constant = Ctx->getConstantInt32(Mask2[Index - 1]);

    if (Index == 1) {
      _shufps(ElementR, SourceVectRM, Mask1Constant);
      _shufps(ElementR, SourceVectRM, Mask2Constant);
      _movp(Inst->getDest(), ElementR);
    } else {
      Variable *T = makeReg(Ty);
      _movp(T, SourceVectRM);
      _shufps(ElementR, T, Mask1Constant);
      _shufps(T, ElementR, Mask2Constant);
      _movp(Inst->getDest(), T);
    }
  } else {
    assert(Ty == IceType_v16i8 || Ty == IceType_v16i1);
    // Spill the value to a stack slot and perform the insertion in
    // memory.
    //
    // TODO(wala): use legalize(SourceVectNotLegalized, Legal_Mem) when
    // support for legalizing to mem is implemented.
    Variable *Slot = Func->template makeVariable(Ty);
    Slot->setWeight(RegWeight::Zero);
    _movp(Slot, legalizeToVar(SourceVectNotLegalized));

    // Compute the location of the position to insert in memory.
    unsigned Offset = Index * typeWidthInBytes(InVectorElementTy);
    OperandX8632Mem *Loc =
        getMemoryOperandForStackSlot(InVectorElementTy, Slot, Offset);
    _store(legalizeToVar(ElementToInsertNotLegalized), Loc);

    Variable *T = makeReg(Ty);
    _movp(T, Slot);
    _movp(Inst->getDest(), T);
  }
}

template <class Machine>
void TargetX86Base<Machine>::lowerIntrinsicCall(
    const InstIntrinsicCall *Instr) {
  switch (Intrinsics::IntrinsicID ID = Instr->getIntrinsicInfo().ID) {
  case Intrinsics::AtomicCmpxchg: {
    if (!Intrinsics::isMemoryOrderValid(
            ID, getConstantMemoryOrder(Instr->getArg(3)),
            getConstantMemoryOrder(Instr->getArg(4)))) {
      Func->setError("Unexpected memory ordering for AtomicCmpxchg");
      return;
    }
    Variable *DestPrev = Instr->getDest();
    Operand *PtrToMem = Instr->getArg(0);
    Operand *Expected = Instr->getArg(1);
    Operand *Desired = Instr->getArg(2);
    if (tryOptimizedCmpxchgCmpBr(DestPrev, PtrToMem, Expected, Desired))
      return;
    lowerAtomicCmpxchg(DestPrev, PtrToMem, Expected, Desired);
    return;
  }
  case Intrinsics::AtomicFence:
    if (!Intrinsics::isMemoryOrderValid(
            ID, getConstantMemoryOrder(Instr->getArg(0)))) {
      Func->setError("Unexpected memory ordering for AtomicFence");
      return;
    }
    _mfence();
    return;
  case Intrinsics::AtomicFenceAll:
    // NOTE: FenceAll should prevent and load/store from being moved
    // across the fence (both atomic and non-atomic). The InstX8632Mfence
    // instruction is currently marked coarsely as "HasSideEffects".
    _mfence();
    return;
  case Intrinsics::AtomicIsLockFree: {
    // X86 is always lock free for 8/16/32/64 bit accesses.
    // TODO(jvoung): Since the result is constant when given a constant
    // byte size, this opens up DCE opportunities.
    Operand *ByteSize = Instr->getArg(0);
    Variable *Dest = Instr->getDest();
    if (ConstantInteger32 *CI = llvm::dyn_cast<ConstantInteger32>(ByteSize)) {
      Constant *Result;
      switch (CI->getValue()) {
      default:
        // Some x86-64 processors support the cmpxchg16b intruction, which
        // can make 16-byte operations lock free (when used with the LOCK
        // prefix). However, that's not supported in 32-bit mode, so just
        // return 0 even for large sizes.
        Result = Ctx->getConstantZero(IceType_i32);
        break;
      case 1:
      case 2:
      case 4:
      case 8:
        Result = Ctx->getConstantInt32(1);
        break;
      }
      _mov(Dest, Result);
      return;
    }
    // The PNaCl ABI requires the byte size to be a compile-time constant.
    Func->setError("AtomicIsLockFree byte size should be compile-time const");
    return;
  }
  case Intrinsics::AtomicLoad: {
    // We require the memory address to be naturally aligned.
    // Given that is the case, then normal loads are atomic.
    if (!Intrinsics::isMemoryOrderValid(
            ID, getConstantMemoryOrder(Instr->getArg(1)))) {
      Func->setError("Unexpected memory ordering for AtomicLoad");
      return;
    }
    Variable *Dest = Instr->getDest();
    if (Dest->getType() == IceType_i64) {
      // Follow what GCC does and use a movq instead of what lowerLoad()
      // normally does (split the load into two).
      // Thus, this skips load/arithmetic op folding. Load/arithmetic folding
      // can't happen anyway, since this is x86-32 and integer arithmetic only
      // happens on 32-bit quantities.
      Variable *T = makeReg(IceType_f64);
      OperandX8632Mem *Addr = formMemoryOperand(Instr->getArg(0), IceType_f64);
      _movq(T, Addr);
      // Then cast the bits back out of the XMM register to the i64 Dest.
      InstCast *Cast = InstCast::create(Func, InstCast::Bitcast, Dest, T);
      lowerCast(Cast);
      // Make sure that the atomic load isn't elided when unused.
      Context.insert(InstFakeUse::create(Func, Dest->getLo()));
      Context.insert(InstFakeUse::create(Func, Dest->getHi()));
      return;
    }
    InstLoad *Load = InstLoad::create(Func, Dest, Instr->getArg(0));
    lowerLoad(Load);
    // Make sure the atomic load isn't elided when unused, by adding a FakeUse.
    // Since lowerLoad may fuse the load w/ an arithmetic instruction,
    // insert the FakeUse on the last-inserted instruction's dest.
    Context.insert(
        InstFakeUse::create(Func, Context.getLastInserted()->getDest()));
    return;
  }
  case Intrinsics::AtomicRMW:
    if (!Intrinsics::isMemoryOrderValid(
            ID, getConstantMemoryOrder(Instr->getArg(3)))) {
      Func->setError("Unexpected memory ordering for AtomicRMW");
      return;
    }
    lowerAtomicRMW(
        Instr->getDest(),
        static_cast<uint32_t>(
            llvm::cast<ConstantInteger32>(Instr->getArg(0))->getValue()),
        Instr->getArg(1), Instr->getArg(2));
    return;
  case Intrinsics::AtomicStore: {
    if (!Intrinsics::isMemoryOrderValid(
            ID, getConstantMemoryOrder(Instr->getArg(2)))) {
      Func->setError("Unexpected memory ordering for AtomicStore");
      return;
    }
    // We require the memory address to be naturally aligned.
    // Given that is the case, then normal stores are atomic.
    // Add a fence after the store to make it visible.
    Operand *Value = Instr->getArg(0);
    Operand *Ptr = Instr->getArg(1);
    if (Value->getType() == IceType_i64) {
      // Use a movq instead of what lowerStore() normally does
      // (split the store into two), following what GCC does.
      // Cast the bits from int -> to an xmm register first.
      Variable *T = makeReg(IceType_f64);
      InstCast *Cast = InstCast::create(Func, InstCast::Bitcast, T, Value);
      lowerCast(Cast);
      // Then store XMM w/ a movq.
      OperandX8632Mem *Addr = formMemoryOperand(Ptr, IceType_f64);
      _storeq(T, Addr);
      _mfence();
      return;
    }
    InstStore *Store = InstStore::create(Func, Value, Ptr);
    lowerStore(Store);
    _mfence();
    return;
  }
  case Intrinsics::Bswap: {
    Variable *Dest = Instr->getDest();
    Operand *Val = Instr->getArg(0);
    // In 32-bit mode, bswap only works on 32-bit arguments, and the
    // argument must be a register. Use rotate left for 16-bit bswap.
    if (Val->getType() == IceType_i64) {
      Variable *T_Lo = legalizeToVar(loOperand(Val));
      Variable *T_Hi = legalizeToVar(hiOperand(Val));
      Variable *DestLo = llvm::cast<Variable>(loOperand(Dest));
      Variable *DestHi = llvm::cast<Variable>(hiOperand(Dest));
      _bswap(T_Lo);
      _bswap(T_Hi);
      _mov(DestLo, T_Hi);
      _mov(DestHi, T_Lo);
    } else if (Val->getType() == IceType_i32) {
      Variable *T = legalizeToVar(Val);
      _bswap(T);
      _mov(Dest, T);
    } else {
      assert(Val->getType() == IceType_i16);
      Val = legalize(Val);
      Constant *Eight = Ctx->getConstantInt16(8);
      Variable *T = nullptr;
      _mov(T, Val);
      _rol(T, Eight);
      _mov(Dest, T);
    }
    return;
  }
  case Intrinsics::Ctpop: {
    Variable *Dest = Instr->getDest();
    Operand *Val = Instr->getArg(0);
    InstCall *Call = makeHelperCall(isInt32Asserting32Or64(Val->getType())
                                        ? H_call_ctpop_i32
                                        : H_call_ctpop_i64,
                                    Dest, 1);
    Call->addArg(Val);
    lowerCall(Call);
    // The popcount helpers always return 32-bit values, while the intrinsic's
    // signature matches the native POPCNT instruction and fills a 64-bit reg
    // (in 64-bit mode). Thus, clear the upper bits of the dest just in case
    // the user doesn't do that in the IR. If the user does that in the IR,
    // then this zero'ing instruction is dead and gets optimized out.
    if (Val->getType() == IceType_i64) {
      Variable *DestHi = llvm::cast<Variable>(hiOperand(Dest));
      Constant *Zero = Ctx->getConstantZero(IceType_i32);
      _mov(DestHi, Zero);
    }
    return;
  }
  case Intrinsics::Ctlz: {
    // The "is zero undef" parameter is ignored and we always return
    // a well-defined value.
    Operand *Val = legalize(Instr->getArg(0));
    Operand *FirstVal;
    Operand *SecondVal = nullptr;
    if (Val->getType() == IceType_i64) {
      FirstVal = loOperand(Val);
      SecondVal = hiOperand(Val);
    } else {
      FirstVal = Val;
    }
    const bool IsCttz = false;
    lowerCountZeros(IsCttz, Val->getType(), Instr->getDest(), FirstVal,
                    SecondVal);
    return;
  }
  case Intrinsics::Cttz: {
    // The "is zero undef" parameter is ignored and we always return
    // a well-defined value.
    Operand *Val = legalize(Instr->getArg(0));
    Operand *FirstVal;
    Operand *SecondVal = nullptr;
    if (Val->getType() == IceType_i64) {
      FirstVal = hiOperand(Val);
      SecondVal = loOperand(Val);
    } else {
      FirstVal = Val;
    }
    const bool IsCttz = true;
    lowerCountZeros(IsCttz, Val->getType(), Instr->getDest(), FirstVal,
                    SecondVal);
    return;
  }
  case Intrinsics::Fabs: {
    Operand *Src = legalize(Instr->getArg(0));
    Type Ty = Src->getType();
    Variable *Dest = Instr->getDest();
    Variable *T = makeVectorOfFabsMask(Ty);
    // The pand instruction operates on an m128 memory operand, so if
    // Src is an f32 or f64, we need to make sure it's in a register.
    if (isVectorType(Ty)) {
      if (llvm::isa<OperandX8632Mem>(Src))
        Src = legalizeToVar(Src);
    } else {
      Src = legalizeToVar(Src);
    }
    _pand(T, Src);
    if (isVectorType(Ty))
      _movp(Dest, T);
    else
      _mov(Dest, T);
    return;
  }
  case Intrinsics::Longjmp: {
    InstCall *Call = makeHelperCall(H_call_longjmp, nullptr, 2);
    Call->addArg(Instr->getArg(0));
    Call->addArg(Instr->getArg(1));
    lowerCall(Call);
    return;
  }
  case Intrinsics::Memcpy: {
    // In the future, we could potentially emit an inline memcpy/memset, etc.
    // for intrinsic calls w/ a known length.
    InstCall *Call = makeHelperCall(H_call_memcpy, nullptr, 3);
    Call->addArg(Instr->getArg(0));
    Call->addArg(Instr->getArg(1));
    Call->addArg(Instr->getArg(2));
    lowerCall(Call);
    return;
  }
  case Intrinsics::Memmove: {
    InstCall *Call = makeHelperCall(H_call_memmove, nullptr, 3);
    Call->addArg(Instr->getArg(0));
    Call->addArg(Instr->getArg(1));
    Call->addArg(Instr->getArg(2));
    lowerCall(Call);
    return;
  }
  case Intrinsics::Memset: {
    // The value operand needs to be extended to a stack slot size
    // because the PNaCl ABI requires arguments to be at least 32 bits
    // wide.
    Operand *ValOp = Instr->getArg(1);
    assert(ValOp->getType() == IceType_i8);
    Variable *ValExt = Func->template makeVariable(stackSlotType());
    lowerCast(InstCast::create(Func, InstCast::Zext, ValExt, ValOp));
    InstCall *Call = makeHelperCall(H_call_memset, nullptr, 3);
    Call->addArg(Instr->getArg(0));
    Call->addArg(ValExt);
    Call->addArg(Instr->getArg(2));
    lowerCall(Call);
    return;
  }
  case Intrinsics::NaClReadTP: {
    if (Ctx->getFlags().getUseSandboxing()) {
      Constant *Zero = Ctx->getConstantZero(IceType_i32);
      Operand *Src =
          OperandX8632Mem::create(Func, IceType_i32, nullptr, Zero, nullptr, 0,
                                  OperandX8632Mem::SegReg_GS);
      Variable *Dest = Instr->getDest();
      Variable *T = nullptr;
      _mov(T, Src);
      _mov(Dest, T);
    } else {
      InstCall *Call = makeHelperCall(H_call_read_tp, Instr->getDest(), 0);
      lowerCall(Call);
    }
    return;
  }
  case Intrinsics::Setjmp: {
    InstCall *Call = makeHelperCall(H_call_setjmp, Instr->getDest(), 1);
    Call->addArg(Instr->getArg(0));
    lowerCall(Call);
    return;
  }
  case Intrinsics::Sqrt: {
    Operand *Src = legalize(Instr->getArg(0));
    Variable *Dest = Instr->getDest();
    Variable *T = makeReg(Dest->getType());
    _sqrtss(T, Src);
    _mov(Dest, T);
    return;
  }
  case Intrinsics::Stacksave: {
    Variable *esp = Func->getTarget()->getPhysicalRegister(RegX8632::Reg_esp);
    Variable *Dest = Instr->getDest();
    _mov(Dest, esp);
    return;
  }
  case Intrinsics::Stackrestore: {
    Variable *esp = Func->getTarget()->getPhysicalRegister(RegX8632::Reg_esp);
    _mov_nonkillable(esp, Instr->getArg(0));
    return;
  }
  case Intrinsics::Trap:
    _ud2();
    return;
  case Intrinsics::UnknownIntrinsic:
    Func->setError("Should not be lowering UnknownIntrinsic");
    return;
  }
  return;
}

template <class Machine>
void TargetX86Base<Machine>::lowerAtomicCmpxchg(Variable *DestPrev,
                                                Operand *Ptr, Operand *Expected,
                                                Operand *Desired) {
  if (Expected->getType() == IceType_i64) {
    // Reserve the pre-colored registers first, before adding any more
    // infinite-weight variables from formMemoryOperand's legalization.
    Variable *T_edx = makeReg(IceType_i32, RegX8632::Reg_edx);
    Variable *T_eax = makeReg(IceType_i32, RegX8632::Reg_eax);
    Variable *T_ecx = makeReg(IceType_i32, RegX8632::Reg_ecx);
    Variable *T_ebx = makeReg(IceType_i32, RegX8632::Reg_ebx);
    _mov(T_eax, loOperand(Expected));
    _mov(T_edx, hiOperand(Expected));
    _mov(T_ebx, loOperand(Desired));
    _mov(T_ecx, hiOperand(Desired));
    OperandX8632Mem *Addr = formMemoryOperand(Ptr, Expected->getType());
    const bool Locked = true;
    _cmpxchg8b(Addr, T_edx, T_eax, T_ecx, T_ebx, Locked);
    Variable *DestLo = llvm::cast<Variable>(loOperand(DestPrev));
    Variable *DestHi = llvm::cast<Variable>(hiOperand(DestPrev));
    _mov(DestLo, T_eax);
    _mov(DestHi, T_edx);
    return;
  }
  Variable *T_eax = makeReg(Expected->getType(), RegX8632::Reg_eax);
  _mov(T_eax, Expected);
  OperandX8632Mem *Addr = formMemoryOperand(Ptr, Expected->getType());
  Variable *DesiredReg = legalizeToVar(Desired);
  const bool Locked = true;
  _cmpxchg(Addr, T_eax, DesiredReg, Locked);
  _mov(DestPrev, T_eax);
}

template <class Machine>
bool TargetX86Base<Machine>::tryOptimizedCmpxchgCmpBr(Variable *Dest,
                                                      Operand *PtrToMem,
                                                      Operand *Expected,
                                                      Operand *Desired) {
  if (Ctx->getFlags().getOptLevel() == Opt_m1)
    return false;
  // Peek ahead a few instructions and see how Dest is used.
  // It's very common to have:
  //
  // %x = call i32 @llvm.nacl.atomic.cmpxchg.i32(i32* ptr, i32 %expected, ...)
  // [%y_phi = ...] // list of phi stores
  // %p = icmp eq i32 %x, %expected
  // br i1 %p, label %l1, label %l2
  //
  // which we can optimize into:
  //
  // %x = <cmpxchg code>
  // [%y_phi = ...] // list of phi stores
  // br eq, %l1, %l2
  InstList::iterator I = Context.getCur();
  // I is currently the InstIntrinsicCall. Peek past that.
  // This assumes that the atomic cmpxchg has not been lowered yet,
  // so that the instructions seen in the scan from "Cur" is simple.
  assert(llvm::isa<InstIntrinsicCall>(*I));
  Inst *NextInst = Context.getNextInst(I);
  if (!NextInst)
    return false;
  // There might be phi assignments right before the compare+branch, since this
  // could be a backward branch for a loop. This placement of assignments is
  // determined by placePhiStores().
  std::vector<InstAssign *> PhiAssigns;
  while (InstAssign *PhiAssign = llvm::dyn_cast<InstAssign>(NextInst)) {
    if (PhiAssign->getDest() == Dest)
      return false;
    PhiAssigns.push_back(PhiAssign);
    NextInst = Context.getNextInst(I);
    if (!NextInst)
      return false;
  }
  if (InstIcmp *NextCmp = llvm::dyn_cast<InstIcmp>(NextInst)) {
    if (!(NextCmp->getCondition() == InstIcmp::Eq &&
          ((NextCmp->getSrc(0) == Dest && NextCmp->getSrc(1) == Expected) ||
           (NextCmp->getSrc(1) == Dest && NextCmp->getSrc(0) == Expected)))) {
      return false;
    }
    NextInst = Context.getNextInst(I);
    if (!NextInst)
      return false;
    if (InstBr *NextBr = llvm::dyn_cast<InstBr>(NextInst)) {
      if (!NextBr->isUnconditional() &&
          NextCmp->getDest() == NextBr->getCondition() &&
          NextBr->isLastUse(NextCmp->getDest())) {
        lowerAtomicCmpxchg(Dest, PtrToMem, Expected, Desired);
        for (size_t i = 0; i < PhiAssigns.size(); ++i) {
          // Lower the phi assignments now, before the branch (same placement
          // as before).
          InstAssign *PhiAssign = PhiAssigns[i];
          PhiAssign->setDeleted();
          lowerAssign(PhiAssign);
          Context.advanceNext();
        }
        _br(CondX86::Br_e, NextBr->getTargetTrue(), NextBr->getTargetFalse());
        // Skip over the old compare and branch, by deleting them.
        NextCmp->setDeleted();
        NextBr->setDeleted();
        Context.advanceNext();
        Context.advanceNext();
        return true;
      }
    }
  }
  return false;
}

template <class Machine>
void TargetX86Base<Machine>::lowerAtomicRMW(Variable *Dest, uint32_t Operation,
                                            Operand *Ptr, Operand *Val) {
  bool NeedsCmpxchg = false;
  LowerBinOp Op_Lo = nullptr;
  LowerBinOp Op_Hi = nullptr;
  switch (Operation) {
  default:
    Func->setError("Unknown AtomicRMW operation");
    return;
  case Intrinsics::AtomicAdd: {
    if (Dest->getType() == IceType_i64) {
      // All the fall-through paths must set this to true, but use this
      // for asserting.
      NeedsCmpxchg = true;
      Op_Lo = &TargetX86Base<Machine>::_add;
      Op_Hi = &TargetX86Base<Machine>::_adc;
      break;
    }
    OperandX8632Mem *Addr = formMemoryOperand(Ptr, Dest->getType());
    const bool Locked = true;
    Variable *T = nullptr;
    _mov(T, Val);
    _xadd(Addr, T, Locked);
    _mov(Dest, T);
    return;
  }
  case Intrinsics::AtomicSub: {
    if (Dest->getType() == IceType_i64) {
      NeedsCmpxchg = true;
      Op_Lo = &TargetX86Base<Machine>::_sub;
      Op_Hi = &TargetX86Base<Machine>::_sbb;
      break;
    }
    OperandX8632Mem *Addr = formMemoryOperand(Ptr, Dest->getType());
    const bool Locked = true;
    Variable *T = nullptr;
    _mov(T, Val);
    _neg(T);
    _xadd(Addr, T, Locked);
    _mov(Dest, T);
    return;
  }
  case Intrinsics::AtomicOr:
    // TODO(jvoung): If Dest is null or dead, then some of these
    // operations do not need an "exchange", but just a locked op.
    // That appears to be "worth" it for sub, or, and, and xor.
    // xadd is probably fine vs lock add for add, and xchg is fine
    // vs an atomic store.
    NeedsCmpxchg = true;
    Op_Lo = &TargetX86Base<Machine>::_or;
    Op_Hi = &TargetX86Base<Machine>::_or;
    break;
  case Intrinsics::AtomicAnd:
    NeedsCmpxchg = true;
    Op_Lo = &TargetX86Base<Machine>::_and;
    Op_Hi = &TargetX86Base<Machine>::_and;
    break;
  case Intrinsics::AtomicXor:
    NeedsCmpxchg = true;
    Op_Lo = &TargetX86Base<Machine>::_xor;
    Op_Hi = &TargetX86Base<Machine>::_xor;
    break;
  case Intrinsics::AtomicExchange:
    if (Dest->getType() == IceType_i64) {
      NeedsCmpxchg = true;
      // NeedsCmpxchg, but no real Op_Lo/Op_Hi need to be done. The values
      // just need to be moved to the ecx and ebx registers.
      Op_Lo = nullptr;
      Op_Hi = nullptr;
      break;
    }
    OperandX8632Mem *Addr = formMemoryOperand(Ptr, Dest->getType());
    Variable *T = nullptr;
    _mov(T, Val);
    _xchg(Addr, T);
    _mov(Dest, T);
    return;
  }
  // Otherwise, we need a cmpxchg loop.
  (void)NeedsCmpxchg;
  assert(NeedsCmpxchg);
  expandAtomicRMWAsCmpxchg(Op_Lo, Op_Hi, Dest, Ptr, Val);
}

template <class Machine>
void TargetX86Base<Machine>::expandAtomicRMWAsCmpxchg(LowerBinOp Op_Lo,
                                                      LowerBinOp Op_Hi,
                                                      Variable *Dest,
                                                      Operand *Ptr,
                                                      Operand *Val) {
  // Expand a more complex RMW operation as a cmpxchg loop:
  // For 64-bit:
  //   mov     eax, [ptr]
  //   mov     edx, [ptr + 4]
  // .LABEL:
  //   mov     ebx, eax
  //   <Op_Lo> ebx, <desired_adj_lo>
  //   mov     ecx, edx
  //   <Op_Hi> ecx, <desired_adj_hi>
  //   lock cmpxchg8b [ptr]
  //   jne     .LABEL
  //   mov     <dest_lo>, eax
  //   mov     <dest_lo>, edx
  //
  // For 32-bit:
  //   mov     eax, [ptr]
  // .LABEL:
  //   mov     <reg>, eax
  //   op      <reg>, [desired_adj]
  //   lock cmpxchg [ptr], <reg>
  //   jne     .LABEL
  //   mov     <dest>, eax
  //
  // If Op_{Lo,Hi} are nullptr, then just copy the value.
  Val = legalize(Val);
  Type Ty = Val->getType();
  if (Ty == IceType_i64) {
    Variable *T_edx = makeReg(IceType_i32, RegX8632::Reg_edx);
    Variable *T_eax = makeReg(IceType_i32, RegX8632::Reg_eax);
    OperandX8632Mem *Addr = formMemoryOperand(Ptr, Ty);
    _mov(T_eax, loOperand(Addr));
    _mov(T_edx, hiOperand(Addr));
    Variable *T_ecx = makeReg(IceType_i32, RegX8632::Reg_ecx);
    Variable *T_ebx = makeReg(IceType_i32, RegX8632::Reg_ebx);
    InstX8632Label *Label = InstX8632Label::create(Func, this);
    const bool IsXchg8b = Op_Lo == nullptr && Op_Hi == nullptr;
    if (!IsXchg8b) {
      Context.insert(Label);
      _mov(T_ebx, T_eax);
      (this->*Op_Lo)(T_ebx, loOperand(Val));
      _mov(T_ecx, T_edx);
      (this->*Op_Hi)(T_ecx, hiOperand(Val));
    } else {
      // This is for xchg, which doesn't need an actual Op_Lo/Op_Hi.
      // It just needs the Val loaded into ebx and ecx.
      // That can also be done before the loop.
      _mov(T_ebx, loOperand(Val));
      _mov(T_ecx, hiOperand(Val));
      Context.insert(Label);
    }
    const bool Locked = true;
    _cmpxchg8b(Addr, T_edx, T_eax, T_ecx, T_ebx, Locked);
    _br(CondX86::Br_ne, Label);
    if (!IsXchg8b) {
      // If Val is a variable, model the extended live range of Val through
      // the end of the loop, since it will be re-used by the loop.
      if (Variable *ValVar = llvm::dyn_cast<Variable>(Val)) {
        Variable *ValLo = llvm::cast<Variable>(loOperand(ValVar));
        Variable *ValHi = llvm::cast<Variable>(hiOperand(ValVar));
        Context.insert(InstFakeUse::create(Func, ValLo));
        Context.insert(InstFakeUse::create(Func, ValHi));
      }
    } else {
      // For xchg, the loop is slightly smaller and ebx/ecx are used.
      Context.insert(InstFakeUse::create(Func, T_ebx));
      Context.insert(InstFakeUse::create(Func, T_ecx));
    }
    // The address base (if any) is also reused in the loop.
    if (Variable *Base = Addr->getBase())
      Context.insert(InstFakeUse::create(Func, Base));
    Variable *DestLo = llvm::cast<Variable>(loOperand(Dest));
    Variable *DestHi = llvm::cast<Variable>(hiOperand(Dest));
    _mov(DestLo, T_eax);
    _mov(DestHi, T_edx);
    return;
  }
  OperandX8632Mem *Addr = formMemoryOperand(Ptr, Ty);
  Variable *T_eax = makeReg(Ty, RegX8632::Reg_eax);
  _mov(T_eax, Addr);
  InstX8632Label *Label = InstX8632Label::create(Func, this);
  Context.insert(Label);
  // We want to pick a different register for T than Eax, so don't use
  // _mov(T == nullptr, T_eax).
  Variable *T = makeReg(Ty);
  _mov(T, T_eax);
  (this->*Op_Lo)(T, Val);
  const bool Locked = true;
  _cmpxchg(Addr, T_eax, T, Locked);
  _br(CondX86::Br_ne, Label);
  // If Val is a variable, model the extended live range of Val through
  // the end of the loop, since it will be re-used by the loop.
  if (Variable *ValVar = llvm::dyn_cast<Variable>(Val)) {
    Context.insert(InstFakeUse::create(Func, ValVar));
  }
  // The address base (if any) is also reused in the loop.
  if (Variable *Base = Addr->getBase())
    Context.insert(InstFakeUse::create(Func, Base));
  _mov(Dest, T_eax);
}

// Lowers count {trailing, leading} zeros intrinsic.
//
// We could do constant folding here, but that should have
// been done by the front-end/middle-end optimizations.
template <class Machine>
void TargetX86Base<Machine>::lowerCountZeros(bool Cttz, Type Ty, Variable *Dest,
                                             Operand *FirstVal,
                                             Operand *SecondVal) {
  // TODO(jvoung): Determine if the user CPU supports LZCNT (BMI).
  // Then the instructions will handle the Val == 0 case much more simply
  // and won't require conversion from bit position to number of zeros.
  //
  // Otherwise:
  //   bsr IF_NOT_ZERO, Val
  //   mov T_DEST, 63
  //   cmovne T_DEST, IF_NOT_ZERO
  //   xor T_DEST, 31
  //   mov DEST, T_DEST
  //
  // NOTE: T_DEST must be a register because cmov requires its dest to be a
  // register. Also, bsf and bsr require their dest to be a register.
  //
  // The xor DEST, 31 converts a bit position to # of leading zeroes.
  // E.g., for 000... 00001100, bsr will say that the most significant bit
  // set is at position 3, while the number of leading zeros is 28. Xor is
  // like (31 - N) for N <= 31, and converts 63 to 32 (for the all-zeros case).
  //
  // Similar for 64-bit, but start w/ speculating that the upper 32 bits
  // are all zero, and compute the result for that case (checking the lower
  // 32 bits). Then actually compute the result for the upper bits and
  // cmov in the result from the lower computation if the earlier speculation
  // was correct.
  //
  // Cttz, is similar, but uses bsf instead, and doesn't require the xor
  // bit position conversion, and the speculation is reversed.
  assert(Ty == IceType_i32 || Ty == IceType_i64);
  Variable *T = makeReg(IceType_i32);
  Operand *FirstValRM = legalize(FirstVal, Legal_Mem | Legal_Reg);
  if (Cttz) {
    _bsf(T, FirstValRM);
  } else {
    _bsr(T, FirstValRM);
  }
  Variable *T_Dest = makeReg(IceType_i32);
  Constant *ThirtyTwo = Ctx->getConstantInt32(32);
  Constant *ThirtyOne = Ctx->getConstantInt32(31);
  if (Cttz) {
    _mov(T_Dest, ThirtyTwo);
  } else {
    Constant *SixtyThree = Ctx->getConstantInt32(63);
    _mov(T_Dest, SixtyThree);
  }
  _cmov(T_Dest, T, CondX86::Br_ne);
  if (!Cttz) {
    _xor(T_Dest, ThirtyOne);
  }
  if (Ty == IceType_i32) {
    _mov(Dest, T_Dest);
    return;
  }
  _add(T_Dest, ThirtyTwo);
  Variable *DestLo = llvm::cast<Variable>(loOperand(Dest));
  Variable *DestHi = llvm::cast<Variable>(hiOperand(Dest));
  // Will be using "test" on this, so we need a registerized variable.
  Variable *SecondVar = legalizeToVar(SecondVal);
  Variable *T_Dest2 = makeReg(IceType_i32);
  if (Cttz) {
    _bsf(T_Dest2, SecondVar);
  } else {
    _bsr(T_Dest2, SecondVar);
    _xor(T_Dest2, ThirtyOne);
  }
  _test(SecondVar, SecondVar);
  _cmov(T_Dest2, T_Dest, CondX86::Br_e);
  _mov(DestLo, T_Dest2);
  _mov(DestHi, Ctx->getConstantZero(IceType_i32));
}

bool isAdd(const Inst *Inst) {
  if (const InstArithmetic *Arith =
          llvm::dyn_cast_or_null<const InstArithmetic>(Inst)) {
    return (Arith->getOp() == InstArithmetic::Add);
  }
  return false;
}

void dumpAddressOpt(const Cfg *Func, const Variable *Base,
                    const Variable *Index, uint16_t Shift, int32_t Offset,
                    const Inst *Reason) {
  if (!BuildDefs::dump())
    return;
  if (!Func->isVerbose(IceV_AddrOpt))
    return;
  OstreamLocker L(Func->getContext());
  Ostream &Str = Func->getContext()->getStrDump();
  Str << "Instruction: ";
  Reason->dumpDecorated(Func);
  Str << "  results in Base=";
  if (Base)
    Base->dump(Func);
  else
    Str << "<null>";
  Str << ", Index=";
  if (Index)
    Index->dump(Func);
  else
    Str << "<null>";
  Str << ", Shift=" << Shift << ", Offset=" << Offset << "\n";
}

bool matchTransitiveAssign(const VariablesMetadata *VMetadata, Variable *&Var,
                           const Inst *&Reason) {
  // Var originates from Var=SrcVar ==>
  //   set Var:=SrcVar
  if (Var == nullptr)
    return false;
  if (const Inst *VarAssign = VMetadata->getSingleDefinition(Var)) {
    assert(!VMetadata->isMultiDef(Var));
    if (llvm::isa<InstAssign>(VarAssign)) {
      Operand *SrcOp = VarAssign->getSrc(0);
      assert(SrcOp);
      if (Variable *SrcVar = llvm::dyn_cast<Variable>(SrcOp)) {
        if (!VMetadata->isMultiDef(SrcVar) &&
            // TODO: ensure SrcVar stays single-BB
            true) {
          Var = SrcVar;
          Reason = VarAssign;
          return true;
        }
      }
    }
  }
  return false;
}

bool matchCombinedBaseIndex(const VariablesMetadata *VMetadata, Variable *&Base,
                            Variable *&Index, uint16_t &Shift,
                            const Inst *&Reason) {
  // Index==nullptr && Base is Base=Var1+Var2 ==>
  //   set Base=Var1, Index=Var2, Shift=0
  if (Base == nullptr)
    return false;
  if (Index != nullptr)
    return false;
  const Inst *BaseInst = VMetadata->getSingleDefinition(Base);
  if (BaseInst == nullptr)
    return false;
  assert(!VMetadata->isMultiDef(Base));
  if (BaseInst->getSrcSize() < 2)
    return false;
  if (Variable *Var1 = llvm::dyn_cast<Variable>(BaseInst->getSrc(0))) {
    if (VMetadata->isMultiDef(Var1))
      return false;
    if (Variable *Var2 = llvm::dyn_cast<Variable>(BaseInst->getSrc(1))) {
      if (VMetadata->isMultiDef(Var2))
        return false;
      if (isAdd(BaseInst) &&
          // TODO: ensure Var1 and Var2 stay single-BB
          true) {
        Base = Var1;
        Index = Var2;
        Shift = 0; // should already have been 0
        Reason = BaseInst;
        return true;
      }
    }
  }
  return false;
}

bool matchShiftedIndex(const VariablesMetadata *VMetadata, Variable *&Index,
                       uint16_t &Shift, const Inst *&Reason) {
  // Index is Index=Var*Const && log2(Const)+Shift<=3 ==>
  //   Index=Var, Shift+=log2(Const)
  if (Index == nullptr)
    return false;
  const Inst *IndexInst = VMetadata->getSingleDefinition(Index);
  if (IndexInst == nullptr)
    return false;
  assert(!VMetadata->isMultiDef(Index));
  if (IndexInst->getSrcSize() < 2)
    return false;
  if (const InstArithmetic *ArithInst =
          llvm::dyn_cast<InstArithmetic>(IndexInst)) {
    if (Variable *Var = llvm::dyn_cast<Variable>(ArithInst->getSrc(0))) {
      if (ConstantInteger32 *Const =
              llvm::dyn_cast<ConstantInteger32>(ArithInst->getSrc(1))) {
        if (ArithInst->getOp() == InstArithmetic::Mul &&
            !VMetadata->isMultiDef(Var) && Const->getType() == IceType_i32) {
          uint64_t Mult = Const->getValue();
          uint32_t LogMult;
          switch (Mult) {
          case 1:
            LogMult = 0;
            break;
          case 2:
            LogMult = 1;
            break;
          case 4:
            LogMult = 2;
            break;
          case 8:
            LogMult = 3;
            break;
          default:
            return false;
          }
          if (Shift + LogMult <= 3) {
            Index = Var;
            Shift += LogMult;
            Reason = IndexInst;
            return true;
          }
        }
      }
    }
  }
  return false;
}

bool matchOffsetBase(const VariablesMetadata *VMetadata, Variable *&Base,
                     int32_t &Offset, const Inst *&Reason) {
  // Base is Base=Var+Const || Base is Base=Const+Var ==>
  //   set Base=Var, Offset+=Const
  // Base is Base=Var-Const ==>
  //   set Base=Var, Offset-=Const
  if (Base == nullptr)
    return false;
  const Inst *BaseInst = VMetadata->getSingleDefinition(Base);
  if (BaseInst == nullptr)
    return false;
  assert(!VMetadata->isMultiDef(Base));
  if (const InstArithmetic *ArithInst =
          llvm::dyn_cast<const InstArithmetic>(BaseInst)) {
    if (ArithInst->getOp() != InstArithmetic::Add &&
        ArithInst->getOp() != InstArithmetic::Sub)
      return false;
    bool IsAdd = ArithInst->getOp() == InstArithmetic::Add;
    Variable *Var = nullptr;
    ConstantInteger32 *Const = nullptr;
    if (Variable *VariableOperand =
            llvm::dyn_cast<Variable>(ArithInst->getSrc(0))) {
      Var = VariableOperand;
      Const = llvm::dyn_cast<ConstantInteger32>(ArithInst->getSrc(1));
    } else if (IsAdd) {
      Const = llvm::dyn_cast<ConstantInteger32>(ArithInst->getSrc(0));
      Var = llvm::dyn_cast<Variable>(ArithInst->getSrc(1));
    }
    if (Var == nullptr || Const == nullptr || VMetadata->isMultiDef(Var))
      return false;
    int32_t MoreOffset = IsAdd ? Const->getValue() : -Const->getValue();
    if (Utils::WouldOverflowAdd(Offset, MoreOffset))
      return false;
    Base = Var;
    Offset += MoreOffset;
    Reason = BaseInst;
    return true;
  }
  return false;
}

void computeAddressOpt(Cfg *Func, const Inst *Instr, Variable *&Base,
                       Variable *&Index, uint16_t &Shift, int32_t &Offset) {
  Func->resetCurrentNode();
  if (Func->isVerbose(IceV_AddrOpt)) {
    OstreamLocker L(Func->getContext());
    Ostream &Str = Func->getContext()->getStrDump();
    Str << "\nStarting computeAddressOpt for instruction:\n  ";
    Instr->dumpDecorated(Func);
  }
  (void)Offset; // TODO: pattern-match for non-zero offsets.
  if (Base == nullptr)
    return;
  // If the Base has more than one use or is live across multiple
  // blocks, then don't go further.  Alternatively (?), never consider
  // a transformation that would change a variable that is currently
  // *not* live across basic block boundaries into one that *is*.
  if (Func->getVMetadata()->isMultiBlock(Base) /* || Base->getUseCount() > 1*/)
    return;

  const VariablesMetadata *VMetadata = Func->getVMetadata();
  bool Continue = true;
  while (Continue) {
    const Inst *Reason = nullptr;
    if (matchTransitiveAssign(VMetadata, Base, Reason) ||
        matchTransitiveAssign(VMetadata, Index, Reason) ||
        matchCombinedBaseIndex(VMetadata, Base, Index, Shift, Reason) ||
        matchShiftedIndex(VMetadata, Index, Shift, Reason) ||
        matchOffsetBase(VMetadata, Base, Offset, Reason)) {
      dumpAddressOpt(Func, Base, Index, Shift, Offset, Reason);
    } else {
      Continue = false;
    }

    // Index is Index=Var<<Const && Const+Shift<=3 ==>
    //   Index=Var, Shift+=Const

    // Index is Index=Const*Var && log2(Const)+Shift<=3 ==>
    //   Index=Var, Shift+=log2(Const)

    // Index && Shift==0 && Base is Base=Var*Const && log2(Const)+Shift<=3 ==>
    //   swap(Index,Base)
    // Similar for Base=Const*Var and Base=Var<<Const

    // Index is Index=Var+Const ==>
    //   set Index=Var, Offset+=(Const<<Shift)

    // Index is Index=Const+Var ==>
    //   set Index=Var, Offset+=(Const<<Shift)

    // Index is Index=Var-Const ==>
    //   set Index=Var, Offset-=(Const<<Shift)

    // TODO: consider overflow issues with respect to Offset.
    // TODO: handle symbolic constants.
  }
}

template <class Machine>
void TargetX86Base<Machine>::lowerLoad(const InstLoad *Load) {
  // A Load instruction can be treated the same as an Assign
  // instruction, after the source operand is transformed into an
  // OperandX8632Mem operand.  Note that the address mode
  // optimization already creates an OperandX8632Mem operand, so it
  // doesn't need another level of transformation.
  Variable *DestLoad = Load->getDest();
  Type Ty = DestLoad->getType();
  Operand *Src0 = formMemoryOperand(Load->getSourceAddress(), Ty);
  InstAssign *Assign = InstAssign::create(Func, DestLoad, Src0);
  lowerAssign(Assign);
}

template <class Machine> void TargetX86Base<Machine>::doAddressOptLoad() {
  Inst *Inst = Context.getCur();
  Variable *Dest = Inst->getDest();
  Operand *Addr = Inst->getSrc(0);
  Variable *Index = nullptr;
  uint16_t Shift = 0;
  int32_t Offset = 0; // TODO: make Constant
  // Vanilla ICE load instructions should not use the segment registers,
  // and computeAddressOpt only works at the level of Variables and Constants,
  // not other OperandX8632Mem, so there should be no mention of segment
  // registers there either.
  const OperandX8632Mem::SegmentRegisters SegmentReg =
      OperandX8632Mem::DefaultSegment;
  Variable *Base = llvm::dyn_cast<Variable>(Addr);
  computeAddressOpt(Func, Inst, Base, Index, Shift, Offset);
  if (Base && Addr != Base) {
    Inst->setDeleted();
    Constant *OffsetOp = Ctx->getConstantInt32(Offset);
    Addr = OperandX8632Mem::create(Func, Dest->getType(), Base, OffsetOp, Index,
                                   Shift, SegmentReg);
    Context.insert(InstLoad::create(Func, Dest, Addr));
  }
}

template <class Machine>
void TargetX86Base<Machine>::randomlyInsertNop(float Probability) {
  RandomNumberGeneratorWrapper RNG(Ctx->getRNG());
  if (RNG.getTrueWithProbability(Probability)) {
    _nop(RNG(Traits::X86_NUM_NOP_VARIANTS));
  }
}

template <class Machine>
void TargetX86Base<Machine>::lowerPhi(const InstPhi * /*Inst*/) {
  Func->setError("Phi found in regular instruction list");
}

template <class Machine>
void TargetX86Base<Machine>::lowerRet(const InstRet *Inst) {
  Variable *Reg = nullptr;
  if (Inst->hasRetValue()) {
    Operand *Src0 = legalize(Inst->getRetValue());
    if (Src0->getType() == IceType_i64) {
      Variable *eax = legalizeToVar(loOperand(Src0), RegX8632::Reg_eax);
      Variable *edx = legalizeToVar(hiOperand(Src0), RegX8632::Reg_edx);
      Reg = eax;
      Context.insert(InstFakeUse::create(Func, edx));
    } else if (isScalarFloatingType(Src0->getType())) {
      _fld(Src0);
    } else if (isVectorType(Src0->getType())) {
      Reg = legalizeToVar(Src0, RegX8632::Reg_xmm0);
    } else {
      _mov(Reg, Src0, RegX8632::Reg_eax);
    }
  }
  // Add a ret instruction even if sandboxing is enabled, because
  // addEpilog explicitly looks for a ret instruction as a marker for
  // where to insert the frame removal instructions.
  _ret(Reg);
  // Add a fake use of esp to make sure esp stays alive for the entire
  // function.  Otherwise post-call esp adjustments get dead-code
  // eliminated.  TODO: Are there more places where the fake use
  // should be inserted?  E.g. "void f(int n){while(1) g(n);}" may not
  // have a ret instruction.
  Variable *esp = Func->getTarget()->getPhysicalRegister(RegX8632::Reg_esp);
  Context.insert(InstFakeUse::create(Func, esp));
}

template <class Machine>
void TargetX86Base<Machine>::lowerSelect(const InstSelect *Inst) {
  Variable *Dest = Inst->getDest();
  Type DestTy = Dest->getType();
  Operand *SrcT = Inst->getTrueOperand();
  Operand *SrcF = Inst->getFalseOperand();
  Operand *Condition = Inst->getCondition();

  if (isVectorType(DestTy)) {
    Type SrcTy = SrcT->getType();
    Variable *T = makeReg(SrcTy);
    Operand *SrcTRM = legalize(SrcT, Legal_Reg | Legal_Mem);
    Operand *SrcFRM = legalize(SrcF, Legal_Reg | Legal_Mem);
    if (InstructionSet >= Machine::SSE4_1) {
      // TODO(wala): If the condition operand is a constant, use blendps
      // or pblendw.
      //
      // Use blendvps or pblendvb to implement select.
      if (SrcTy == IceType_v4i1 || SrcTy == IceType_v4i32 ||
          SrcTy == IceType_v4f32) {
        Operand *ConditionRM = legalize(Condition, Legal_Reg | Legal_Mem);
        Variable *xmm0 = makeReg(IceType_v4i32, RegX8632::Reg_xmm0);
        _movp(xmm0, ConditionRM);
        _psll(xmm0, Ctx->getConstantInt8(31));
        _movp(T, SrcFRM);
        _blendvps(T, SrcTRM, xmm0);
        _movp(Dest, T);
      } else {
        assert(typeNumElements(SrcTy) == 8 || typeNumElements(SrcTy) == 16);
        Type SignExtTy = Condition->getType() == IceType_v8i1 ? IceType_v8i16
                                                              : IceType_v16i8;
        Variable *xmm0 = makeReg(SignExtTy, RegX8632::Reg_xmm0);
        lowerCast(InstCast::create(Func, InstCast::Sext, xmm0, Condition));
        _movp(T, SrcFRM);
        _pblendvb(T, SrcTRM, xmm0);
        _movp(Dest, T);
      }
      return;
    }
    // Lower select without Machine::SSE4.1:
    // a=d?b:c ==>
    //   if elementtype(d) != i1:
    //      d=sext(d);
    //   a=(b&d)|(c&~d);
    Variable *T2 = makeReg(SrcTy);
    // Sign extend the condition operand if applicable.
    if (SrcTy == IceType_v4f32) {
      // The sext operation takes only integer arguments.
      Variable *T3 = Func->template makeVariable(IceType_v4i32);
      lowerCast(InstCast::create(Func, InstCast::Sext, T3, Condition));
      _movp(T, T3);
    } else if (typeElementType(SrcTy) != IceType_i1) {
      lowerCast(InstCast::create(Func, InstCast::Sext, T, Condition));
    } else {
      Operand *ConditionRM = legalize(Condition, Legal_Reg | Legal_Mem);
      _movp(T, ConditionRM);
    }
    _movp(T2, T);
    _pand(T, SrcTRM);
    _pandn(T2, SrcFRM);
    _por(T, T2);
    _movp(Dest, T);

    return;
  }

  CondX86::BrCond Cond = CondX86::Br_ne;
  Operand *CmpOpnd0 = nullptr;
  Operand *CmpOpnd1 = nullptr;
  // Handle folding opportunities.
  if (const class Inst *Producer = FoldingInfo.getProducerFor(Condition)) {
    assert(Producer->isDeleted());
    switch (BoolFolding::getProducerKind(Producer)) {
    default:
      break;
    case BoolFolding::PK_Icmp32: {
      auto *Cmp = llvm::dyn_cast<InstIcmp>(Producer);
      Cond = Traits::getIcmp32Mapping(Cmp->getCondition());
      CmpOpnd1 = legalize(Producer->getSrc(1));
      CmpOpnd0 = legalizeSrc0ForCmp(Producer->getSrc(0), CmpOpnd1);
    } break;
    }
  }
  if (CmpOpnd0 == nullptr) {
    CmpOpnd0 = legalize(Condition, Legal_Reg | Legal_Mem);
    CmpOpnd1 = Ctx->getConstantZero(IceType_i32);
  }
  assert(CmpOpnd0);
  assert(CmpOpnd1);

  _cmp(CmpOpnd0, CmpOpnd1);
  if (typeWidthInBytes(DestTy) == 1 || isFloatingType(DestTy)) {
    // The cmov instruction doesn't allow 8-bit or FP operands, so
    // we need explicit control flow.
    // d=cmp e,f; a=d?b:c ==> cmp e,f; a=b; jne L1; a=c; L1:
    InstX8632Label *Label = InstX8632Label::create(Func, this);
    SrcT = legalize(SrcT, Legal_Reg | Legal_Imm);
    _mov(Dest, SrcT);
    _br(Cond, Label);
    SrcF = legalize(SrcF, Legal_Reg | Legal_Imm);
    _mov_nonkillable(Dest, SrcF);
    Context.insert(Label);
    return;
  }
  // mov t, SrcF; cmov_cond t, SrcT; mov dest, t
  // But if SrcT is immediate, we might be able to do better, as
  // the cmov instruction doesn't allow an immediate operand:
  // mov t, SrcT; cmov_!cond t, SrcF; mov dest, t
  if (llvm::isa<Constant>(SrcT) && !llvm::isa<Constant>(SrcF)) {
    std::swap(SrcT, SrcF);
    Cond = InstX8632::getOppositeCondition(Cond);
  }
  if (DestTy == IceType_i64) {
    // Set the low portion.
    Variable *DestLo = llvm::cast<Variable>(loOperand(Dest));
    Variable *TLo = nullptr;
    Operand *SrcFLo = legalize(loOperand(SrcF));
    _mov(TLo, SrcFLo);
    Operand *SrcTLo = legalize(loOperand(SrcT), Legal_Reg | Legal_Mem);
    _cmov(TLo, SrcTLo, Cond);
    _mov(DestLo, TLo);
    // Set the high portion.
    Variable *DestHi = llvm::cast<Variable>(hiOperand(Dest));
    Variable *THi = nullptr;
    Operand *SrcFHi = legalize(hiOperand(SrcF));
    _mov(THi, SrcFHi);
    Operand *SrcTHi = legalize(hiOperand(SrcT), Legal_Reg | Legal_Mem);
    _cmov(THi, SrcTHi, Cond);
    _mov(DestHi, THi);
    return;
  }

  assert(DestTy == IceType_i16 || DestTy == IceType_i32);
  Variable *T = nullptr;
  SrcF = legalize(SrcF);
  _mov(T, SrcF);
  SrcT = legalize(SrcT, Legal_Reg | Legal_Mem);
  _cmov(T, SrcT, Cond);
  _mov(Dest, T);
}

template <class Machine>
void TargetX86Base<Machine>::lowerStore(const InstStore *Inst) {
  Operand *Value = Inst->getData();
  Operand *Addr = Inst->getAddr();
  OperandX8632Mem *NewAddr = formMemoryOperand(Addr, Value->getType());
  Type Ty = NewAddr->getType();

  if (Ty == IceType_i64) {
    Value = legalize(Value);
    Operand *ValueHi = legalize(hiOperand(Value), Legal_Reg | Legal_Imm);
    Operand *ValueLo = legalize(loOperand(Value), Legal_Reg | Legal_Imm);
    _store(ValueHi, llvm::cast<OperandX8632Mem>(hiOperand(NewAddr)));
    _store(ValueLo, llvm::cast<OperandX8632Mem>(loOperand(NewAddr)));
  } else if (isVectorType(Ty)) {
    _storep(legalizeToVar(Value), NewAddr);
  } else {
    Value = legalize(Value, Legal_Reg | Legal_Imm);
    _store(Value, NewAddr);
  }
}

template <class Machine> void TargetX86Base<Machine>::doAddressOptStore() {
  InstStore *Inst = llvm::cast<InstStore>(Context.getCur());
  Operand *Data = Inst->getData();
  Operand *Addr = Inst->getAddr();
  Variable *Index = nullptr;
  uint16_t Shift = 0;
  int32_t Offset = 0; // TODO: make Constant
  Variable *Base = llvm::dyn_cast<Variable>(Addr);
  // Vanilla ICE store instructions should not use the segment registers,
  // and computeAddressOpt only works at the level of Variables and Constants,
  // not other OperandX8632Mem, so there should be no mention of segment
  // registers there either.
  const OperandX8632Mem::SegmentRegisters SegmentReg =
      OperandX8632Mem::DefaultSegment;
  computeAddressOpt(Func, Inst, Base, Index, Shift, Offset);
  if (Base && Addr != Base) {
    Inst->setDeleted();
    Constant *OffsetOp = Ctx->getConstantInt32(Offset);
    Addr = OperandX8632Mem::create(Func, Data->getType(), Base, OffsetOp, Index,
                                   Shift, SegmentReg);
    InstStore *NewStore = InstStore::create(Func, Data, Addr);
    if (Inst->getDest())
      NewStore->setRmwBeacon(Inst->getRmwBeacon());
    Context.insert(NewStore);
  }
}

template <class Machine>
void TargetX86Base<Machine>::lowerSwitch(const InstSwitch *Inst) {
  // This implements the most naive possible lowering.
  // cmp a,val[0]; jeq label[0]; cmp a,val[1]; jeq label[1]; ... jmp default
  Operand *Src0 = Inst->getComparison();
  SizeT NumCases = Inst->getNumCases();
  if (Src0->getType() == IceType_i64) {
    Src0 = legalize(Src0); // get Base/Index into physical registers
    Operand *Src0Lo = loOperand(Src0);
    Operand *Src0Hi = hiOperand(Src0);
    if (NumCases >= 2) {
      Src0Lo = legalizeToVar(Src0Lo);
      Src0Hi = legalizeToVar(Src0Hi);
    } else {
      Src0Lo = legalize(Src0Lo, Legal_Reg | Legal_Mem);
      Src0Hi = legalize(Src0Hi, Legal_Reg | Legal_Mem);
    }
    for (SizeT I = 0; I < NumCases; ++I) {
      Constant *ValueLo = Ctx->getConstantInt32(Inst->getValue(I));
      Constant *ValueHi = Ctx->getConstantInt32(Inst->getValue(I) >> 32);
      InstX8632Label *Label = InstX8632Label::create(Func, this);
      _cmp(Src0Lo, ValueLo);
      _br(CondX86::Br_ne, Label);
      _cmp(Src0Hi, ValueHi);
      _br(CondX86::Br_e, Inst->getLabel(I));
      Context.insert(Label);
    }
    _br(Inst->getLabelDefault());
    return;
  }
  // OK, we'll be slightly less naive by forcing Src into a physical
  // register if there are 2 or more uses.
  if (NumCases >= 2)
    Src0 = legalizeToVar(Src0);
  else
    Src0 = legalize(Src0, Legal_Reg | Legal_Mem);
  for (SizeT I = 0; I < NumCases; ++I) {
    Constant *Value = Ctx->getConstantInt32(Inst->getValue(I));
    _cmp(Src0, Value);
    _br(CondX86::Br_e, Inst->getLabel(I));
  }

  _br(Inst->getLabelDefault());
}

template <class Machine>
void TargetX86Base<Machine>::scalarizeArithmetic(InstArithmetic::OpKind Kind,
                                                 Variable *Dest, Operand *Src0,
                                                 Operand *Src1) {
  assert(isVectorType(Dest->getType()));
  Type Ty = Dest->getType();
  Type ElementTy = typeElementType(Ty);
  SizeT NumElements = typeNumElements(Ty);

  Operand *T = Ctx->getConstantUndef(Ty);
  for (SizeT I = 0; I < NumElements; ++I) {
    Constant *Index = Ctx->getConstantInt32(I);

    // Extract the next two inputs.
    Variable *Op0 = Func->template makeVariable(ElementTy);
    lowerExtractElement(InstExtractElement::create(Func, Op0, Src0, Index));
    Variable *Op1 = Func->template makeVariable(ElementTy);
    lowerExtractElement(InstExtractElement::create(Func, Op1, Src1, Index));

    // Perform the arithmetic as a scalar operation.
    Variable *Res = Func->template makeVariable(ElementTy);
    lowerArithmetic(InstArithmetic::create(Func, Kind, Res, Op0, Op1));

    // Insert the result into position.
    Variable *DestT = Func->template makeVariable(Ty);
    lowerInsertElement(InstInsertElement::create(Func, DestT, T, Res, Index));
    T = DestT;
  }

  lowerAssign(InstAssign::create(Func, Dest, T));
}

// The following pattern occurs often in lowered C and C++ code:
//
//   %cmp     = fcmp/icmp pred <n x ty> %src0, %src1
//   %cmp.ext = sext <n x i1> %cmp to <n x ty>
//
// We can eliminate the sext operation by copying the result of pcmpeqd,
// pcmpgtd, or cmpps (which produce sign extended results) to the result
// of the sext operation.
template <class Machine>
void TargetX86Base<Machine>::eliminateNextVectorSextInstruction(
    Variable *SignExtendedResult) {
  if (InstCast *NextCast =
          llvm::dyn_cast_or_null<InstCast>(Context.getNextInst())) {
    if (NextCast->getCastKind() == InstCast::Sext &&
        NextCast->getSrc(0) == SignExtendedResult) {
      NextCast->setDeleted();
      _movp(NextCast->getDest(), legalizeToVar(SignExtendedResult));
      // Skip over the instruction.
      Context.advanceNext();
    }
  }
}

template <class Machine>
void TargetX86Base<Machine>::lowerUnreachable(
    const InstUnreachable * /*Inst*/) {
  _ud2();
}

template <class Machine>
void TargetX86Base<Machine>::lowerRMW(const InstX8632FakeRMW *RMW) {
  // If the beacon variable's live range does not end in this
  // instruction, then it must end in the modified Store instruction
  // that follows.  This means that the original Store instruction is
  // still there, either because the value being stored is used beyond
  // the Store instruction, or because dead code elimination did not
  // happen.  In either case, we cancel RMW lowering (and the caller
  // deletes the RMW instruction).
  if (!RMW->isLastUse(RMW->getBeacon()))
    return;
  Operand *Src = RMW->getData();
  Type Ty = Src->getType();
  OperandX8632Mem *Addr = formMemoryOperand(RMW->getAddr(), Ty);
  if (Ty == IceType_i64) {
    Operand *SrcLo = legalize(loOperand(Src), Legal_Reg | Legal_Imm);
    Operand *SrcHi = legalize(hiOperand(Src), Legal_Reg | Legal_Imm);
    OperandX8632Mem *AddrLo = llvm::cast<OperandX8632Mem>(loOperand(Addr));
    OperandX8632Mem *AddrHi = llvm::cast<OperandX8632Mem>(hiOperand(Addr));
    switch (RMW->getOp()) {
    default:
      // TODO(stichnot): Implement other arithmetic operators.
      break;
    case InstArithmetic::Add:
      _add_rmw(AddrLo, SrcLo);
      _adc_rmw(AddrHi, SrcHi);
      return;
    case InstArithmetic::Sub:
      _sub_rmw(AddrLo, SrcLo);
      _sbb_rmw(AddrHi, SrcHi);
      return;
    case InstArithmetic::And:
      _and_rmw(AddrLo, SrcLo);
      _and_rmw(AddrHi, SrcHi);
      return;
    case InstArithmetic::Or:
      _or_rmw(AddrLo, SrcLo);
      _or_rmw(AddrHi, SrcHi);
      return;
    case InstArithmetic::Xor:
      _xor_rmw(AddrLo, SrcLo);
      _xor_rmw(AddrHi, SrcHi);
      return;
    }
  } else {
    // i8, i16, i32
    switch (RMW->getOp()) {
    default:
      // TODO(stichnot): Implement other arithmetic operators.
      break;
    case InstArithmetic::Add:
      Src = legalize(Src, Legal_Reg | Legal_Imm);
      _add_rmw(Addr, Src);
      return;
    case InstArithmetic::Sub:
      Src = legalize(Src, Legal_Reg | Legal_Imm);
      _sub_rmw(Addr, Src);
      return;
    case InstArithmetic::And:
      Src = legalize(Src, Legal_Reg | Legal_Imm);
      _and_rmw(Addr, Src);
      return;
    case InstArithmetic::Or:
      Src = legalize(Src, Legal_Reg | Legal_Imm);
      _or_rmw(Addr, Src);
      return;
    case InstArithmetic::Xor:
      Src = legalize(Src, Legal_Reg | Legal_Imm);
      _xor_rmw(Addr, Src);
      return;
    }
  }
  llvm::report_fatal_error("Couldn't lower RMW instruction");
}

template <class Machine>
void TargetX86Base<Machine>::lowerOther(const Inst *Instr) {
  if (const auto *RMW = llvm::dyn_cast<InstX8632FakeRMW>(Instr)) {
    lowerRMW(RMW);
  } else {
    TargetLowering::lowerOther(Instr);
  }
}

// Turn an i64 Phi instruction into a pair of i32 Phi instructions, to
// preserve integrity of liveness analysis.  Undef values are also
// turned into zeroes, since loOperand() and hiOperand() don't expect
// Undef input.
template <class Machine> void TargetX86Base<Machine>::prelowerPhis() {
  // Pause constant blinding or pooling, blinding or pooling will be done later
  // during phi lowering assignments
  BoolFlagSaver B(RandomizationPoolingPaused, true);

  CfgNode *Node = Context.getNode();
  for (Inst &I : Node->getPhis()) {
    auto Phi = llvm::dyn_cast<InstPhi>(&I);
    if (Phi->isDeleted())
      continue;
    Variable *Dest = Phi->getDest();
    if (Dest->getType() == IceType_i64) {
      Variable *DestLo = llvm::cast<Variable>(loOperand(Dest));
      Variable *DestHi = llvm::cast<Variable>(hiOperand(Dest));
      InstPhi *PhiLo = InstPhi::create(Func, Phi->getSrcSize(), DestLo);
      InstPhi *PhiHi = InstPhi::create(Func, Phi->getSrcSize(), DestHi);
      for (SizeT I = 0; I < Phi->getSrcSize(); ++I) {
        Operand *Src = Phi->getSrc(I);
        CfgNode *Label = Phi->getLabel(I);
        if (llvm::isa<ConstantUndef>(Src))
          Src = Ctx->getConstantZero(Dest->getType());
        PhiLo->addArgument(loOperand(Src), Label);
        PhiHi->addArgument(hiOperand(Src), Label);
      }
      Node->getPhis().push_back(PhiLo);
      Node->getPhis().push_back(PhiHi);
      Phi->setDeleted();
    }
  }
}

bool isMemoryOperand(const Operand *Opnd) {
  if (const auto Var = llvm::dyn_cast<Variable>(Opnd))
    return !Var->hasReg();
  // We treat vector undef values the same as a memory operand,
  // because they do in fact need a register to materialize the vector
  // of zeroes into.
  if (llvm::isa<ConstantUndef>(Opnd))
    return isScalarFloatingType(Opnd->getType()) ||
           isVectorType(Opnd->getType());
  if (llvm::isa<Constant>(Opnd))
    return isScalarFloatingType(Opnd->getType());
  return true;
}

// Lower the pre-ordered list of assignments into mov instructions.
// Also has to do some ad-hoc register allocation as necessary.
template <class Machine>
void TargetX86Base<Machine>::lowerPhiAssignments(
    CfgNode *Node, const AssignList &Assignments) {
  // Check that this is a properly initialized shell of a node.
  assert(Node->getOutEdges().size() == 1);
  assert(Node->getInsts().empty());
  assert(Node->getPhis().empty());
  CfgNode *Succ = Node->getOutEdges().front();
  getContext().init(Node);
  // Register set setup similar to regAlloc().
  RegSetMask RegInclude = RegSet_All;
  RegSetMask RegExclude = RegSet_StackPointer;
  if (hasFramePointer())
    RegExclude |= RegSet_FramePointer;
  llvm::SmallBitVector Available = getRegisterSet(RegInclude, RegExclude);
  bool NeedsRegs = false;
  // Initialize the set of available registers to the set of what is
  // available (not live) at the beginning of the successor block,
  // minus all registers used as Dest operands in the Assignments.  To
  // do this, we start off assuming all registers are available, then
  // iterate through the Assignments and remove Dest registers.
  // During this iteration, we also determine whether we will actually
  // need any extra registers for memory-to-memory copies.  If so, we
  // do the actual work of removing the live-in registers from the
  // set.  TODO(stichnot): This work is being repeated for every split
  // edge to the successor, so consider updating LiveIn just once
  // after all the edges are split.
  for (const Inst &I : Assignments) {
    Variable *Dest = I.getDest();
    if (Dest->hasReg()) {
      Available[Dest->getRegNum()] = false;
    } else if (isMemoryOperand(I.getSrc(0))) {
      NeedsRegs = true; // Src and Dest are both in memory
    }
  }
  if (NeedsRegs) {
    LivenessBV &LiveIn = Func->getLiveness()->getLiveIn(Succ);
    for (int i = LiveIn.find_first(); i != -1; i = LiveIn.find_next(i)) {
      Variable *Var = Func->getLiveness()->getVariable(i, Succ);
      if (Var->hasReg())
        Available[Var->getRegNum()] = false;
    }
  }
  // Iterate backwards through the Assignments.  After lowering each
  // assignment, add Dest to the set of available registers, and
  // remove Src from the set of available registers.  Iteration is
  // done backwards to enable incremental updates of the available
  // register set, and the lowered instruction numbers may be out of
  // order, but that can be worked around by renumbering the block
  // afterwards if necessary.
  for (const Inst &I : reverse_range(Assignments)) {
    Context.rewind();
    auto Assign = llvm::dyn_cast<InstAssign>(&I);
    Variable *Dest = Assign->getDest();

    // If the source operand is ConstantUndef, do not legalize it.
    // In function test_split_undef_int_vec, the advanced phi
    // lowering process will find an assignment of undefined
    // vector. This vector, as the Src here, will crash if it
    // go through legalize(). legalize() will create new variable
    // with makeVectorOfZeros(), but this new variable will be
    // assigned a stack slot. This will fail the assertion in
    // IceInstX8632.cpp:789, as XmmEmitterRegOp() complain:
    // Var->hasReg() fails. Note this failure is irrelevant to
    // randomization or pooling of constants.
    // So, we do not call legalize() to add pool label for the
    // src operands of phi assignment instructions.
    // Instead, we manually add pool label for constant float and
    // constant double values here.
    // Note going through legalize() does not affect the testing
    // results of SPEC2K and xtests.
    Operand *Src = Assign->getSrc(0);
    if (!llvm::isa<ConstantUndef>(Assign->getSrc(0))) {
      Src = legalize(Src);
    }

    Variable *SrcVar = llvm::dyn_cast<Variable>(Src);
    // Use normal assignment lowering, except lower mem=mem specially
    // so we can register-allocate at the same time.
    if (!isMemoryOperand(Dest) || !isMemoryOperand(Src)) {
      lowerAssign(Assign);
    } else {
      assert(Dest->getType() == Src->getType());
      const llvm::SmallBitVector &RegsForType =
          getRegisterSetForType(Dest->getType());
      llvm::SmallBitVector AvailRegsForType = RegsForType & Available;
      Variable *SpillLoc = nullptr;
      Variable *Preg = nullptr;
      // TODO(stichnot): Opportunity for register randomization.
      int32_t RegNum = AvailRegsForType.find_first();
      bool IsVector = isVectorType(Dest->getType());
      bool NeedSpill = (RegNum == -1);
      if (NeedSpill) {
        // Pick some register to spill and update RegNum.
        // TODO(stichnot): Opportunity for register randomization.
        RegNum = RegsForType.find_first();
        Preg = getPhysicalRegister(RegNum, Dest->getType());
        SpillLoc = Func->template makeVariable(Dest->getType());
        // Create a fake def of the physical register to avoid
        // liveness inconsistency problems during late-stage liveness
        // analysis (e.g. asm-verbose mode).
        Context.insert(InstFakeDef::create(Func, Preg));
        if (IsVector)
          _movp(SpillLoc, Preg);
        else
          _mov(SpillLoc, Preg);
      }
      assert(RegNum >= 0);
      if (llvm::isa<ConstantUndef>(Src))
        // Materialize an actual constant instead of undef.  RegNum is
        // passed in for vector types because undef vectors are
        // lowered to vector register of zeroes.
        Src =
            legalize(Src, Legal_All, IsVector ? RegNum : Variable::NoRegister);
      Variable *Tmp = makeReg(Dest->getType(), RegNum);
      if (IsVector) {
        _movp(Tmp, Src);
        _movp(Dest, Tmp);
      } else {
        _mov(Tmp, Src);
        _mov(Dest, Tmp);
      }
      if (NeedSpill) {
        // Restore the spilled register.
        if (IsVector)
          _movp(Preg, SpillLoc);
        else
          _mov(Preg, SpillLoc);
        // Create a fake use of the physical register to keep it live
        // for late-stage liveness analysis (e.g. asm-verbose mode).
        Context.insert(InstFakeUse::create(Func, Preg));
      }
    }
    // Update register availability before moving to the previous
    // instruction on the Assignments list.
    if (Dest->hasReg())
      Available[Dest->getRegNum()] = true;
    if (SrcVar && SrcVar->hasReg())
      Available[SrcVar->getRegNum()] = false;
  }

  // Add the terminator branch instruction to the end.
  Context.setInsertPoint(Context.getEnd());
  _br(Succ);
}

// There is no support for loading or emitting vector constants, so the
// vector values returned from makeVectorOfZeros, makeVectorOfOnes,
// etc. are initialized with register operations.
//
// TODO(wala): Add limited support for vector constants so that
// complex initialization in registers is unnecessary.

template <class Machine>
Variable *TargetX86Base<Machine>::makeVectorOfZeros(Type Ty, int32_t RegNum) {
  Variable *Reg = makeReg(Ty, RegNum);
  // Insert a FakeDef, since otherwise the live range of Reg might
  // be overestimated.
  Context.insert(InstFakeDef::create(Func, Reg));
  _pxor(Reg, Reg);
  return Reg;
}

template <class Machine>
Variable *TargetX86Base<Machine>::makeVectorOfMinusOnes(Type Ty,
                                                        int32_t RegNum) {
  Variable *MinusOnes = makeReg(Ty, RegNum);
  // Insert a FakeDef so the live range of MinusOnes is not overestimated.
  Context.insert(InstFakeDef::create(Func, MinusOnes));
  _pcmpeq(MinusOnes, MinusOnes);
  return MinusOnes;
}

template <class Machine>
Variable *TargetX86Base<Machine>::makeVectorOfOnes(Type Ty, int32_t RegNum) {
  Variable *Dest = makeVectorOfZeros(Ty, RegNum);
  Variable *MinusOne = makeVectorOfMinusOnes(Ty);
  _psub(Dest, MinusOne);
  return Dest;
}

template <class Machine>
Variable *TargetX86Base<Machine>::makeVectorOfHighOrderBits(Type Ty,
                                                            int32_t RegNum) {
  assert(Ty == IceType_v4i32 || Ty == IceType_v4f32 || Ty == IceType_v8i16 ||
         Ty == IceType_v16i8);
  if (Ty == IceType_v4f32 || Ty == IceType_v4i32 || Ty == IceType_v8i16) {
    Variable *Reg = makeVectorOfOnes(Ty, RegNum);
    SizeT Shift =
        typeWidthInBytes(typeElementType(Ty)) * Traits::X86_CHAR_BIT - 1;
    _psll(Reg, Ctx->getConstantInt8(Shift));
    return Reg;
  } else {
    // SSE has no left shift operation for vectors of 8 bit integers.
    const uint32_t HIGH_ORDER_BITS_MASK = 0x80808080;
    Constant *ConstantMask = Ctx->getConstantInt32(HIGH_ORDER_BITS_MASK);
    Variable *Reg = makeReg(Ty, RegNum);
    _movd(Reg, legalize(ConstantMask, Legal_Reg | Legal_Mem));
    _pshufd(Reg, Reg, Ctx->getConstantZero(IceType_i8));
    return Reg;
  }
}

// Construct a mask in a register that can be and'ed with a
// floating-point value to mask off its sign bit.  The value will be
// <4 x 0x7fffffff> for f32 and v4f32, and <2 x 0x7fffffffffffffff>
// for f64.  Construct it as vector of ones logically right shifted
// one bit.  TODO(stichnot): Fix the wala TODO above, to represent
// vector constants in memory.
template <class Machine>
Variable *TargetX86Base<Machine>::makeVectorOfFabsMask(Type Ty,
                                                       int32_t RegNum) {
  Variable *Reg = makeVectorOfMinusOnes(Ty, RegNum);
  _psrl(Reg, Ctx->getConstantInt8(1));
  return Reg;
}

template <class Machine>
OperandX8632Mem *
TargetX86Base<Machine>::getMemoryOperandForStackSlot(Type Ty, Variable *Slot,
                                                     uint32_t Offset) {
  // Ensure that Loc is a stack slot.
  assert(Slot->getWeight().isZero());
  assert(Slot->getRegNum() == Variable::NoRegister);
  // Compute the location of Loc in memory.
  // TODO(wala,stichnot): lea should not be required.  The address of
  // the stack slot is known at compile time (although not until after
  // addProlog()).
  const Type PointerType = IceType_i32;
  Variable *Loc = makeReg(PointerType);
  _lea(Loc, Slot);
  Constant *ConstantOffset = Ctx->getConstantInt32(Offset);
  return OperandX8632Mem::create(Func, Ty, Loc, ConstantOffset);
}

// Helper for legalize() to emit the right code to lower an operand to a
// register of the appropriate type.
template <class Machine>
Variable *TargetX86Base<Machine>::copyToReg(Operand *Src, int32_t RegNum) {
  Type Ty = Src->getType();
  Variable *Reg = makeReg(Ty, RegNum);
  if (isVectorType(Ty)) {
    _movp(Reg, Src);
  } else {
    _mov(Reg, Src);
  }
  return Reg;
}

template <class Machine>
Operand *TargetX86Base<Machine>::legalize(Operand *From, LegalMask Allowed,
                                          int32_t RegNum) {
  Type Ty = From->getType();
  // Assert that a physical register is allowed.  To date, all calls
  // to legalize() allow a physical register.  If a physical register
  // needs to be explicitly disallowed, then new code will need to be
  // written to force a spill.
  assert(Allowed & Legal_Reg);
  // If we're asking for a specific physical register, make sure we're
  // not allowing any other operand kinds.  (This could be future
  // work, e.g. allow the shl shift amount to be either an immediate
  // or in ecx.)
  assert(RegNum == Variable::NoRegister || Allowed == Legal_Reg);

  if (auto Mem = llvm::dyn_cast<OperandX8632Mem>(From)) {
    // Before doing anything with a Mem operand, we need to ensure
    // that the Base and Index components are in physical registers.
    Variable *Base = Mem->getBase();
    Variable *Index = Mem->getIndex();
    Variable *RegBase = nullptr;
    Variable *RegIndex = nullptr;
    if (Base) {
      RegBase = legalizeToVar(Base);
    }
    if (Index) {
      RegIndex = legalizeToVar(Index);
    }
    if (Base != RegBase || Index != RegIndex) {
      Mem =
          OperandX8632Mem::create(Func, Ty, RegBase, Mem->getOffset(), RegIndex,
                                  Mem->getShift(), Mem->getSegmentRegister());
    }

    // For all Memory Operands, we do randomization/pooling here
    From = randomizeOrPoolImmediate(Mem);

    if (!(Allowed & Legal_Mem)) {
      From = copyToReg(From, RegNum);
    }
    return From;
  }
  if (auto *Const = llvm::dyn_cast<Constant>(From)) {
    if (llvm::isa<ConstantUndef>(Const)) {
      // Lower undefs to zero.  Another option is to lower undefs to an
      // uninitialized register; however, using an uninitialized register
      // results in less predictable code.
      //
      // If in the future the implementation is changed to lower undef
      // values to uninitialized registers, a FakeDef will be needed:
      //     Context.insert(InstFakeDef::create(Func, Reg));
      // This is in order to ensure that the live range of Reg is not
      // overestimated.  If the constant being lowered is a 64 bit value,
      // then the result should be split and the lo and hi components will
      // need to go in uninitialized registers.
      if (isVectorType(Ty))
        return makeVectorOfZeros(Ty, RegNum);
      Const = Ctx->getConstantZero(Ty);
      From = Const;
    }
    // There should be no constants of vector type (other than undef).
    assert(!isVectorType(Ty));

    // If the operand is an 32 bit constant integer, we should check
    // whether we need to randomize it or pool it.
    if (ConstantInteger32 *C = llvm::dyn_cast<ConstantInteger32>(Const)) {
      Operand *NewConst = randomizeOrPoolImmediate(C, RegNum);
      if (NewConst != Const) {
        return NewConst;
      }
    }

    // Convert a scalar floating point constant into an explicit
    // memory operand.
    if (isScalarFloatingType(Ty)) {
      Variable *Base = nullptr;
      std::string Buffer;
      llvm::raw_string_ostream StrBuf(Buffer);
      llvm::cast<Constant>(From)->emitPoolLabel(StrBuf);
      llvm::cast<Constant>(From)->setShouldBePooled(true);
      Constant *Offset = Ctx->getConstantSym(0, StrBuf.str(), true);
      From = OperandX8632Mem::create(Func, Ty, Base, Offset);
    }
    bool NeedsReg = false;
    if (!(Allowed & Legal_Imm) && !isScalarFloatingType(Ty))
      // Immediate specifically not allowed
      NeedsReg = true;
    if (!(Allowed & Legal_Mem) && isScalarFloatingType(Ty))
      // On x86, FP constants are lowered to mem operands.
      NeedsReg = true;
    if (NeedsReg) {
      From = copyToReg(From, RegNum);
    }
    return From;
  }
  if (auto Var = llvm::dyn_cast<Variable>(From)) {
    // Check if the variable is guaranteed a physical register.  This
    // can happen either when the variable is pre-colored or when it is
    // assigned infinite weight.
    bool MustHaveRegister = (Var->hasReg() || Var->getWeight().isInf());
    // We need a new physical register for the operand if:
    //   Mem is not allowed and Var isn't guaranteed a physical
    //   register, or
    //   RegNum is required and Var->getRegNum() doesn't match.
    if ((!(Allowed & Legal_Mem) && !MustHaveRegister) ||
        (RegNum != Variable::NoRegister && RegNum != Var->getRegNum())) {
      From = copyToReg(From, RegNum);
    }
    return From;
  }
  llvm_unreachable("Unhandled operand kind in legalize()");
  return From;
}

// Provide a trivial wrapper to legalize() for this common usage.
template <class Machine>
Variable *TargetX86Base<Machine>::legalizeToVar(Operand *From, int32_t RegNum) {
  return llvm::cast<Variable>(legalize(From, Legal_Reg, RegNum));
}

// For the cmp instruction, if Src1 is an immediate, or known to be a
// physical register, we can allow Src0 to be a memory operand.
// Otherwise, Src0 must be copied into a physical register.
// (Actually, either Src0 or Src1 can be chosen for the physical
// register, but unfortunately we have to commit to one or the other
// before register allocation.)
template <class Machine>
Operand *TargetX86Base<Machine>::legalizeSrc0ForCmp(Operand *Src0,
                                                    Operand *Src1) {
  bool IsSrc1ImmOrReg = false;
  if (llvm::isa<Constant>(Src1)) {
    IsSrc1ImmOrReg = true;
  } else if (Variable *Var = llvm::dyn_cast<Variable>(Src1)) {
    if (Var->hasReg())
      IsSrc1ImmOrReg = true;
  }
  return legalize(Src0, IsSrc1ImmOrReg ? (Legal_Reg | Legal_Mem) : Legal_Reg);
}

template <class Machine>
OperandX8632Mem *TargetX86Base<Machine>::formMemoryOperand(Operand *Opnd,
                                                           Type Ty,
                                                           bool DoLegalize) {
  OperandX8632Mem *Mem = llvm::dyn_cast<OperandX8632Mem>(Opnd);
  // It may be the case that address mode optimization already creates
  // an OperandX8632Mem, so in that case it wouldn't need another level
  // of transformation.
  if (!Mem) {
    Variable *Base = llvm::dyn_cast<Variable>(Opnd);
    Constant *Offset = llvm::dyn_cast<Constant>(Opnd);
    assert(Base || Offset);
    if (Offset) {
      // During memory operand building, we do not blind or pool
      // the constant offset, we will work on the whole memory
      // operand later as one entity later, this save one instruction.
      // By turning blinding and pooling off, we guarantee
      // legalize(Offset) will return a constant*.
      {
        BoolFlagSaver B(RandomizationPoolingPaused, true);

        Offset = llvm::cast<Constant>(legalize(Offset));
      }

      assert(llvm::isa<ConstantInteger32>(Offset) ||
             llvm::isa<ConstantRelocatable>(Offset));
    }
    Mem = OperandX8632Mem::create(Func, Ty, Base, Offset);
  }
  // Do legalization, which contains randomization/pooling
  // or do randomization/pooling.
  return llvm::cast<OperandX8632Mem>(
      DoLegalize ? legalize(Mem) : randomizeOrPoolImmediate(Mem));
}

template <class Machine>
Variable *TargetX86Base<Machine>::makeReg(Type Type, int32_t RegNum) {
  // There aren't any 64-bit integer registers for x86-32.
  assert(Type != IceType_i64);
  Variable *Reg = Func->template makeVariable(Type);
  if (RegNum == Variable::NoRegister)
    Reg->setWeightInfinite();
  else
    Reg->setRegNum(RegNum);
  return Reg;
}

template <class Machine> void TargetX86Base<Machine>::postLower() {
  if (Ctx->getFlags().getOptLevel() == Opt_m1)
    return;
  inferTwoAddress();
}

template <class Machine>
void TargetX86Base<Machine>::makeRandomRegisterPermutation(
    llvm::SmallVectorImpl<int32_t> &Permutation,
    const llvm::SmallBitVector &ExcludeRegisters) const {
  // TODO(stichnot): Declaring Permutation this way loses type/size
  // information.  Fix this in conjunction with the caller-side TODO.
  assert(Permutation.size() >= RegX8632::Reg_NUM);
  // Expected upper bound on the number of registers in a single
  // equivalence class.  For x86-32, this would comprise the 8 XMM
  // registers.  This is for performance, not correctness.
  static const unsigned MaxEquivalenceClassSize = 8;
  typedef llvm::SmallVector<int32_t, MaxEquivalenceClassSize> RegisterList;
  typedef std::map<uint32_t, RegisterList> EquivalenceClassMap;
  EquivalenceClassMap EquivalenceClasses;
  SizeT NumShuffled = 0, NumPreserved = 0;

// Build up the equivalence classes of registers by looking at the
// register properties as well as whether the registers should be
// explicitly excluded from shuffling.
#define X(val, encode, name, name16, name8, scratch, preserved, stackptr,      \
          frameptr, isI8, isInt, isFP)                                         \
  if (ExcludeRegisters[RegX8632::val]) {                                       \
    /* val stays the same in the resulting permutation. */                     \
    Permutation[RegX8632::val] = RegX8632::val;                                \
    ++NumPreserved;                                                            \
  } else {                                                                     \
    const uint32_t Index = (scratch << 0) | (preserved << 1) | (isI8 << 2) |   \
                           (isInt << 3) | (isFP << 4);                         \
    /* val is assigned to an equivalence class based on its properties. */     \
    EquivalenceClasses[Index].push_back(RegX8632::val);                        \
  }
  REGX8632_TABLE
#undef X

  RandomNumberGeneratorWrapper RNG(Ctx->getRNG());

  // Shuffle the resulting equivalence classes.
  for (auto I : EquivalenceClasses) {
    const RegisterList &List = I.second;
    RegisterList Shuffled(List);
    RandomShuffle(Shuffled.begin(), Shuffled.end(), RNG);
    for (size_t SI = 0, SE = Shuffled.size(); SI < SE; ++SI) {
      Permutation[List[SI]] = Shuffled[SI];
      ++NumShuffled;
    }
  }

  assert(NumShuffled + NumPreserved == RegX8632::Reg_NUM);

  if (Func->isVerbose(IceV_Random)) {
    OstreamLocker L(Func->getContext());
    Ostream &Str = Func->getContext()->getStrDump();
    Str << "Register equivalence classes:\n";
    for (auto I : EquivalenceClasses) {
      Str << "{";
      const RegisterList &List = I.second;
      bool First = true;
      for (int32_t Register : List) {
        if (!First)
          Str << " ";
        First = false;
        Str << getRegName(Register, IceType_i32);
      }
      Str << "}\n";
    }
  }
}

template <class Machine>
void TargetX86Base<Machine>::emit(const ConstantInteger32 *C) const {
  if (!BuildDefs::dump())
    return;
  Ostream &Str = Ctx->getStrEmit();
  Str << getConstantPrefix() << C->getValue();
}

template <class Machine>
void TargetX86Base<Machine>::emit(const ConstantInteger64 *) const {
  llvm::report_fatal_error("Not expecting to emit 64-bit integers");
}

template <class Machine>
void TargetX86Base<Machine>::emit(const ConstantFloat *C) const {
  if (!BuildDefs::dump())
    return;
  Ostream &Str = Ctx->getStrEmit();
  C->emitPoolLabel(Str);
}

template <class Machine>
void TargetX86Base<Machine>::emit(const ConstantDouble *C) const {
  if (!BuildDefs::dump())
    return;
  Ostream &Str = Ctx->getStrEmit();
  C->emitPoolLabel(Str);
}

template <class Machine>
void TargetX86Base<Machine>::emit(const ConstantUndef *) const {
  llvm::report_fatal_error("undef value encountered by emitter.");
}

// Randomize or pool an Immediate.
template <class Machine>
Operand *TargetX86Base<Machine>::randomizeOrPoolImmediate(Constant *Immediate,
                                                          int32_t RegNum) {
  assert(llvm::isa<ConstantInteger32>(Immediate) ||
         llvm::isa<ConstantRelocatable>(Immediate));
  if (Ctx->getFlags().getRandomizeAndPoolImmediatesOption() == RPI_None ||
      RandomizationPoolingPaused == true) {
    // Immediates randomization/pooling off or paused
    return Immediate;
  }
  if (Immediate->shouldBeRandomizedOrPooled(Ctx)) {
    Ctx->statsUpdateRPImms();
    if (Ctx->getFlags().getRandomizeAndPoolImmediatesOption() ==
        RPI_Randomize) {
      // blind the constant
      // FROM:
      //  imm
      // TO:
      //  insert: mov imm+cookie, Reg
      //  insert: lea -cookie[Reg], Reg
      //  => Reg
      // If we have already assigned a phy register, we must come from
      // andvancedPhiLowering()=>lowerAssign(). In this case we should reuse
      // the assigned register as this assignment is that start of its use-def
      // chain. So we add RegNum argument here.
      // Note we use 'lea' instruction instead of 'xor' to avoid affecting
      // the flags.
      Variable *Reg = makeReg(IceType_i32, RegNum);
      ConstantInteger32 *Integer = llvm::cast<ConstantInteger32>(Immediate);
      uint32_t Value = Integer->getValue();
      uint32_t Cookie = Ctx->getRandomizationCookie();
      _mov(Reg, Ctx->getConstantInt(IceType_i32, Cookie + Value));
      Constant *Offset = Ctx->getConstantInt(IceType_i32, 0 - Cookie);
      _lea(Reg,
           OperandX8632Mem::create(Func, IceType_i32, Reg, Offset, nullptr, 0));
      // make sure liveness analysis won't kill this variable, otherwise a
      // liveness
      // assertion will be triggered.
      _set_dest_nonkillable();
      if (Immediate->getType() != IceType_i32) {
        Variable *TruncReg = makeReg(Immediate->getType(), RegNum);
        _mov(TruncReg, Reg);
        return TruncReg;
      }
      return Reg;
    }
    if (Ctx->getFlags().getRandomizeAndPoolImmediatesOption() == RPI_Pool) {
      // pool the constant
      // FROM:
      //  imm
      // TO:
      //  insert: mov $label, Reg
      //  => Reg
      assert(Ctx->getFlags().getRandomizeAndPoolImmediatesOption() == RPI_Pool);
      Immediate->setShouldBePooled(true);
      // if we have already assigned a phy register, we must come from
      // andvancedPhiLowering()=>lowerAssign(). In this case we should reuse
      // the assigned register as this assignment is that start of its use-def
      // chain. So we add RegNum argument here.
      Variable *Reg = makeReg(Immediate->getType(), RegNum);
      IceString Label;
      llvm::raw_string_ostream Label_stream(Label);
      Immediate->emitPoolLabel(Label_stream);
      const RelocOffsetT Offset = 0;
      const bool SuppressMangling = true;
      Constant *Symbol =
          Ctx->getConstantSym(Offset, Label_stream.str(), SuppressMangling);
      OperandX8632Mem *MemOperand =
          OperandX8632Mem::create(Func, Immediate->getType(), nullptr, Symbol);
      _mov(Reg, MemOperand);
      return Reg;
    }
    assert("Unsupported -randomize-pool-immediates option" && false);
  }
  // the constant Immediate is not eligible for blinding/pooling
  return Immediate;
}

template <class Machine>
OperandX8632Mem *
TargetX86Base<Machine>::randomizeOrPoolImmediate(OperandX8632Mem *MemOperand,
                                                 int32_t RegNum) {
  assert(MemOperand);
  if (Ctx->getFlags().getRandomizeAndPoolImmediatesOption() == RPI_None ||
      RandomizationPoolingPaused == true) {
    // immediates randomization/pooling is turned off
    return MemOperand;
  }

  // If this memory operand is already a randommized one, we do
  // not randomize it again.
  if (MemOperand->getRandomized())
    return MemOperand;

  if (Constant *C = llvm::dyn_cast_or_null<Constant>(MemOperand->getOffset())) {
    if (C->shouldBeRandomizedOrPooled(Ctx)) {
      // The offset of this mem operand should be blinded or pooled
      Ctx->statsUpdateRPImms();
      if (Ctx->getFlags().getRandomizeAndPoolImmediatesOption() ==
          RPI_Randomize) {
        // blind the constant offset
        // FROM:
        //  offset[base, index, shift]
        // TO:
        //  insert: lea offset+cookie[base], RegTemp
        //  => -cookie[RegTemp, index, shift]
        uint32_t Value =
            llvm::dyn_cast<ConstantInteger32>(MemOperand->getOffset())
                ->getValue();
        uint32_t Cookie = Ctx->getRandomizationCookie();
        Constant *Mask1 = Ctx->getConstantInt(
            MemOperand->getOffset()->getType(), Cookie + Value);
        Constant *Mask2 =
            Ctx->getConstantInt(MemOperand->getOffset()->getType(), 0 - Cookie);

        OperandX8632Mem *TempMemOperand = OperandX8632Mem::create(
            Func, MemOperand->getType(), MemOperand->getBase(), Mask1);
        // If we have already assigned a physical register, we must come from
        // advancedPhiLowering()=>lowerAssign(). In this case we should reuse
        // the assigned register as this assignment is that start of its use-def
        // chain. So we add RegNum argument here.
        Variable *RegTemp = makeReg(MemOperand->getOffset()->getType(), RegNum);
        _lea(RegTemp, TempMemOperand);
        // As source operand doesn't use the dstreg, we don't need to add
        // _set_dest_nonkillable().
        // But if we use the same Dest Reg, that is, with RegNum
        // assigned, we should add this _set_dest_nonkillable()
        if (RegNum != Variable::NoRegister)
          _set_dest_nonkillable();

        OperandX8632Mem *NewMemOperand = OperandX8632Mem::create(
            Func, MemOperand->getType(), RegTemp, Mask2, MemOperand->getIndex(),
            MemOperand->getShift(), MemOperand->getSegmentRegister());

        // Label this memory operand as randomize, so we won't randomize it
        // again in case we call legalize() mutiple times on this memory
        // operand.
        NewMemOperand->setRandomized(true);
        return NewMemOperand;
      }
      if (Ctx->getFlags().getRandomizeAndPoolImmediatesOption() == RPI_Pool) {
        // pool the constant offset
        // FROM:
        //  offset[base, index, shift]
        // TO:
        //  insert: mov $label, RegTemp
        //  insert: lea [base, RegTemp], RegTemp
        //  =>[RegTemp, index, shift]
        assert(Ctx->getFlags().getRandomizeAndPoolImmediatesOption() ==
               RPI_Pool);
        // Memory operand should never exist as source operands in phi
        // lowering assignments, so there is no need to reuse any registers
        // here. For phi lowering, we should not ask for new physical
        // registers in general.
        // However, if we do meet Memory Operand during phi lowering, we
        // should not blind or pool the immediates for now.
        if (RegNum != Variable::NoRegister)
          return MemOperand;
        Variable *RegTemp = makeReg(IceType_i32);
        IceString Label;
        llvm::raw_string_ostream Label_stream(Label);
        MemOperand->getOffset()->emitPoolLabel(Label_stream);
        MemOperand->getOffset()->setShouldBePooled(true);
        const RelocOffsetT SymOffset = 0;
        bool SuppressMangling = true;
        Constant *Symbol = Ctx->getConstantSym(SymOffset, Label_stream.str(),
                                               SuppressMangling);
        OperandX8632Mem *SymbolOperand = OperandX8632Mem::create(
            Func, MemOperand->getOffset()->getType(), nullptr, Symbol);
        _mov(RegTemp, SymbolOperand);
        // If we have a base variable here, we should add the lea instruction
        // to add the value of the base variable to RegTemp. If there is no
        // base variable, we won't need this lea instruction.
        if (MemOperand->getBase()) {
          OperandX8632Mem *CalculateOperand = OperandX8632Mem::create(
              Func, MemOperand->getType(), MemOperand->getBase(), nullptr,
              RegTemp, 0, MemOperand->getSegmentRegister());
          _lea(RegTemp, CalculateOperand);
          _set_dest_nonkillable();
        }
        OperandX8632Mem *NewMemOperand = OperandX8632Mem::create(
            Func, MemOperand->getType(), RegTemp, nullptr,
            MemOperand->getIndex(), MemOperand->getShift(),
            MemOperand->getSegmentRegister());
        return NewMemOperand;
      }
      assert("Unsupported -randomize-pool-immediates option" && false);
    }
  }
  // the offset is not eligible for blinding or pooling, return the original
  // mem operand
  return MemOperand;
}

} // end of namespace X86Internal
} // end of namespace Ice

#endif // SUBZERO_SRC_ICETARGETLOWERINGX86BASEIMPL_H
