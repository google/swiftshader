//===- subzero/src/IceTargetLoweringX8632.cpp - x86-32 lowering -----------===//
//
//                        The Subzero Code Generator
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements the TargetLoweringX8632 class, which
// consists almost entirely of the lowering sequence for each
// high-level instruction.  It also implements
// TargetX8632Fast::postLower() which does the simplest possible
// register allocation for the "fast" target.
//
//===----------------------------------------------------------------------===//

#include "IceDefs.h"
#include "IceCfg.h"
#include "IceCfgNode.h"
#include "IceInstX8632.h"
#include "IceOperand.h"
#include "IceTargetLoweringX8632.def"
#include "IceTargetLoweringX8632.h"

namespace Ice {

namespace {

// The following table summarizes the logic for lowering the fcmp
// instruction.  There is one table entry for each of the 16 conditions.
//
// The first four columns describe the case when the operands are
// floating point scalar values.  A comment in lowerFcmp() describes the
// lowering template.  In the most general case, there is a compare
// followed by two conditional branches, because some fcmp conditions
// don't map to a single x86 conditional branch.  However, in many cases
// it is possible to swap the operands in the comparison and have a
// single conditional branch.  Since it's quite tedious to validate the
// table by hand, good execution tests are helpful.
//
// The last two columns describe the case when the operands are vectors
// of floating point values.  For most fcmp conditions, there is a clear
// mapping to a single x86 cmpps instruction variant.  Some fcmp
// conditions require special code to handle and these are marked in the
// table with a Cmpps_Invalid predicate.
const struct TableFcmp_ {
  uint32_t Default;
  bool SwapScalarOperands;
  InstX8632::BrCond C1, C2;
  bool SwapVectorOperands;
  InstX8632Cmpps::CmppsCond Predicate;
} TableFcmp[] = {
#define X(val, dflt, swapS, C1, C2, swapV, pred)                               \
  {                                                                            \
    dflt, swapS, InstX8632Br::C1, InstX8632Br::C2, swapV, InstX8632Cmpps::pred \
  }                                                                            \
  ,
      FCMPX8632_TABLE
#undef X
};
const size_t TableFcmpSize = llvm::array_lengthof(TableFcmp);

// The following table summarizes the logic for lowering the icmp instruction
// for i32 and narrower types.  Each icmp condition has a clear mapping to an
// x86 conditional branch instruction.

const struct TableIcmp32_ {
  InstX8632::BrCond Mapping;
} TableIcmp32[] = {
#define X(val, C_32, C1_64, C2_64, C3_64)                                      \
  { InstX8632Br::C_32 }                                                        \
  ,
    ICMPX8632_TABLE
#undef X
  };
const size_t TableIcmp32Size = llvm::array_lengthof(TableIcmp32);

// The following table summarizes the logic for lowering the icmp instruction
// for the i64 type.  For Eq and Ne, two separate 32-bit comparisons and
// conditional branches are needed.  For the other conditions, three separate
// conditional branches are needed.
const struct TableIcmp64_ {
  InstX8632::BrCond C1, C2, C3;
} TableIcmp64[] = {
#define X(val, C_32, C1_64, C2_64, C3_64)                                      \
  { InstX8632Br::C1_64, InstX8632Br::C2_64, InstX8632Br::C3_64 }               \
  ,
    ICMPX8632_TABLE
#undef X
  };
const size_t TableIcmp64Size = llvm::array_lengthof(TableIcmp64);

InstX8632::BrCond getIcmp32Mapping(InstIcmp::ICond Cond) {
  size_t Index = static_cast<size_t>(Cond);
  assert(Index < TableIcmp32Size);
  return TableIcmp32[Index].Mapping;
}

const struct TableTypeX8632Attributes_ {
  Type InVectorElementType;
} TableTypeX8632Attributes[] = {
#define X(tag, elementty, cvt, sdss, pack, width)                              \
  { elementty }                                                                \
  ,
    ICETYPEX8632_TABLE
#undef X
  };
const size_t TableTypeX8632AttributesSize =
    llvm::array_lengthof(TableTypeX8632Attributes);

// Return the type which the elements of the vector have in the X86
// representation of the vector.
Type getInVectorElementType(Type Ty) {
  assert(isVectorType(Ty));
  size_t Index = static_cast<size_t>(Ty);
  assert(Index < TableTypeX8632AttributesSize);
  return TableTypeX8632Attributes[Ty].InVectorElementType;
}

// The maximum number of arguments to pass in XMM registers
const unsigned X86_MAX_XMM_ARGS = 4;
// The number of bits in a byte
const unsigned X86_CHAR_BIT = 8;

// Return a string representation of the type that is suitable for use
// in an identifier.
IceString typeIdentString(const Type Ty) {
  IceString Str;
  llvm::raw_string_ostream BaseOS(Str);
  if (isVectorType(Ty)) {
    BaseOS << "v" << typeNumElements(Ty) << typeElementType(Ty);
  } else {
    BaseOS << Ty;
  }
  return BaseOS.str();
}

// In some cases, there are x-macros tables for both high-level and
// low-level instructions/operands that use the same enum key value.
// The tables are kept separate to maintain a proper separation
// between abstraction layers.  There is a risk that the tables
// could get out of sync if enum values are reordered or if entries
// are added or deleted.  This dummy function uses static_assert to
// ensure everything is kept in sync.
void __attribute__((unused)) xMacroIntegrityCheck() {
  // Validate the enum values in FCMPX8632_TABLE.
  {
    // Define a temporary set of enum values based on low-level
    // table entries.
    enum _tmp_enum {
#define X(val, dflt, swapS, C1, C2, swapV, pred) _tmp_##val,
      FCMPX8632_TABLE
#undef X
          _num
    };
// Define a set of constants based on high-level table entries.
#define X(tag, str) static const int _table1_##tag = InstFcmp::tag;
    ICEINSTFCMP_TABLE;
#undef X
// Define a set of constants based on low-level table entries,
// and ensure the table entry keys are consistent.
#define X(val, dflt, swapS, C1, C2, swapV, pred)                               \
  static const int _table2_##val = _tmp_##val;                                 \
  STATIC_ASSERT(_table1_##val == _table2_##val);
    FCMPX8632_TABLE;
#undef X
// Repeat the static asserts with respect to the high-level
// table entries in case the high-level table has extra entries.
#define X(tag, str) STATIC_ASSERT(_table1_##tag == _table2_##tag);
    ICEINSTFCMP_TABLE;
#undef X
  }

  // Validate the enum values in ICMPX8632_TABLE.
  {
    // Define a temporary set of enum values based on low-level
    // table entries.
    enum _tmp_enum {
#define X(val, C_32, C1_64, C2_64, C3_64) _tmp_##val,
      ICMPX8632_TABLE
#undef X
          _num
    };
// Define a set of constants based on high-level table entries.
#define X(tag, str) static const int _table1_##tag = InstIcmp::tag;
    ICEINSTICMP_TABLE;
#undef X
// Define a set of constants based on low-level table entries,
// and ensure the table entry keys are consistent.
#define X(val, C_32, C1_64, C2_64, C3_64)                                      \
  static const int _table2_##val = _tmp_##val;                                 \
  STATIC_ASSERT(_table1_##val == _table2_##val);
    ICMPX8632_TABLE;
#undef X
// Repeat the static asserts with respect to the high-level
// table entries in case the high-level table has extra entries.
#define X(tag, str) STATIC_ASSERT(_table1_##tag == _table2_##tag);
    ICEINSTICMP_TABLE;
#undef X
  }

  // Validate the enum values in ICETYPEX8632_TABLE.
  {
    // Define a temporary set of enum values based on low-level
    // table entries.
    enum _tmp_enum {
#define X(tag, elementty, cvt, sdss, pack, width) _tmp_##tag,
      ICETYPEX8632_TABLE
#undef X
          _num
    };
// Define a set of constants based on high-level table entries.
#define X(tag, size, align, elts, elty, str)                                   \
  static const int _table1_##tag = tag;
    ICETYPE_TABLE;
#undef X
// Define a set of constants based on low-level table entries,
// and ensure the table entry keys are consistent.
#define X(tag, elementty, cvt, sdss, pack, width)                              \
  static const int _table2_##tag = _tmp_##tag;                                 \
  STATIC_ASSERT(_table1_##tag == _table2_##tag);
    ICETYPEX8632_TABLE;
#undef X
// Repeat the static asserts with respect to the high-level
// table entries in case the high-level table has extra entries.
#define X(tag, size, align, elts, elty, str)                                   \
  STATIC_ASSERT(_table1_##tag == _table2_##tag);
    ICETYPE_TABLE;
#undef X
  }
}

} // end of anonymous namespace

TargetX8632::TargetX8632(Cfg *Func)
    : TargetLowering(Func), IsEbpBasedFrame(false), FrameSizeLocals(0),
      LocalsSizeBytes(0), NextLabelNumber(0), ComputedLiveRanges(false),
      PhysicalRegisters(VarList(Reg_NUM)) {
  // TODO: Don't initialize IntegerRegisters and friends every time.
  // Instead, initialize in some sort of static initializer for the
  // class.
  llvm::SmallBitVector IntegerRegisters(Reg_NUM);
  llvm::SmallBitVector IntegerRegistersI8(Reg_NUM);
  llvm::SmallBitVector FloatRegisters(Reg_NUM);
  llvm::SmallBitVector VectorRegisters(Reg_NUM);
  llvm::SmallBitVector InvalidRegisters(Reg_NUM);
  ScratchRegs.resize(Reg_NUM);
#define X(val, init, name, name16, name8, scratch, preserved, stackptr,        \
          frameptr, isI8, isInt, isFP)                                         \
  IntegerRegisters[val] = isInt;                                               \
  IntegerRegistersI8[val] = isI8;                                              \
  FloatRegisters[val] = isFP;                                                  \
  VectorRegisters[val] = isFP;                                                 \
  ScratchRegs[val] = scratch;
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

void TargetX8632::translateO2() {
  GlobalContext *Context = Func->getContext();

  // Lower Phi instructions.
  Timer T_placePhiLoads;
  Func->placePhiLoads();
  if (Func->hasError())
    return;
  T_placePhiLoads.printElapsedUs(Context, "placePhiLoads()");
  Timer T_placePhiStores;
  Func->placePhiStores();
  if (Func->hasError())
    return;
  T_placePhiStores.printElapsedUs(Context, "placePhiStores()");
  Timer T_deletePhis;
  Func->deletePhis();
  if (Func->hasError())
    return;
  T_deletePhis.printElapsedUs(Context, "deletePhis()");
  Func->dump("After Phi lowering");

  // Address mode optimization.
  Timer T_doAddressOpt;
  Func->doAddressOpt();
  T_doAddressOpt.printElapsedUs(Context, "doAddressOpt()");

  // Argument lowering
  Timer T_argLowering;
  Func->doArgLowering();
  T_argLowering.printElapsedUs(Context, "lowerArguments()");

  // Target lowering.  This requires liveness analysis for some parts
  // of the lowering decisions, such as compare/branch fusing.  If
  // non-lightweight liveness analysis is used, the instructions need
  // to be renumbered first.  TODO: This renumbering should only be
  // necessary if we're actually calculating live intervals, which we
  // only do for register allocation.
  Timer T_renumber1;
  Func->renumberInstructions();
  if (Func->hasError())
    return;
  T_renumber1.printElapsedUs(Context, "renumberInstructions()");

  // TODO: It should be sufficient to use the fastest liveness
  // calculation, i.e. livenessLightweight().  However, for some
  // reason that slows down the rest of the translation.  Investigate.
  Timer T_liveness1;
  Func->liveness(Liveness_Basic);
  if (Func->hasError())
    return;
  T_liveness1.printElapsedUs(Context, "liveness()");
  Func->dump("After x86 address mode opt");

  Timer T_genCode;
  Func->genCode();
  if (Func->hasError())
    return;
  T_genCode.printElapsedUs(Context, "genCode()");

  // Register allocation.  This requires instruction renumbering and
  // full liveness analysis.
  Timer T_renumber2;
  Func->renumberInstructions();
  if (Func->hasError())
    return;
  T_renumber2.printElapsedUs(Context, "renumberInstructions()");
  Timer T_liveness2;
  Func->liveness(Liveness_Intervals);
  if (Func->hasError())
    return;
  T_liveness2.printElapsedUs(Context, "liveness()");
  // Validate the live range computations.  Do it outside the timing
  // code.  TODO: Put this under a flag.
  bool ValidLiveness = Func->validateLiveness();
  assert(ValidLiveness);
  (void)ValidLiveness; // used only in assert()
  ComputedLiveRanges = true;
  // The post-codegen dump is done here, after liveness analysis and
  // associated cleanup, to make the dump cleaner and more useful.
  Func->dump("After initial x8632 codegen");
  Timer T_regAlloc;
  regAlloc();
  if (Func->hasError())
    return;
  T_regAlloc.printElapsedUs(Context, "regAlloc()");
  Func->dump("After linear scan regalloc");

  // Stack frame mapping.
  Timer T_genFrame;
  Func->genFrame();
  if (Func->hasError())
    return;
  T_genFrame.printElapsedUs(Context, "genFrame()");
  Func->dump("After stack frame mapping");
}

void TargetX8632::translateOm1() {
  GlobalContext *Context = Func->getContext();
  Timer T_placePhiLoads;
  Func->placePhiLoads();
  if (Func->hasError())
    return;
  T_placePhiLoads.printElapsedUs(Context, "placePhiLoads()");
  Timer T_placePhiStores;
  Func->placePhiStores();
  if (Func->hasError())
    return;
  T_placePhiStores.printElapsedUs(Context, "placePhiStores()");
  Timer T_deletePhis;
  Func->deletePhis();
  if (Func->hasError())
    return;
  T_deletePhis.printElapsedUs(Context, "deletePhis()");
  Func->dump("After Phi lowering");

  Timer T_argLowering;
  Func->doArgLowering();
  T_argLowering.printElapsedUs(Context, "lowerArguments()");

  Timer T_genCode;
  Func->genCode();
  if (Func->hasError())
    return;
  T_genCode.printElapsedUs(Context, "genCode()");
  Func->dump("After initial x8632 codegen");

  Timer T_genFrame;
  Func->genFrame();
  if (Func->hasError())
    return;
  T_genFrame.printElapsedUs(Context, "genFrame()");
  Func->dump("After stack frame mapping");
}

IceString TargetX8632::RegNames[] = {
#define X(val, init, name, name16, name8, scratch, preserved, stackptr,        \
          frameptr, isI8, isInt, isFP)                                         \
  name,
  REGX8632_TABLE
#undef X
};

Variable *TargetX8632::getPhysicalRegister(SizeT RegNum) {
  assert(RegNum < PhysicalRegisters.size());
  Variable *Reg = PhysicalRegisters[RegNum];
  if (Reg == NULL) {
    CfgNode *Node = NULL; // NULL means multi-block lifetime
    Reg = Func->makeVariable(IceType_i32, Node);
    Reg->setRegNum(RegNum);
    PhysicalRegisters[RegNum] = Reg;
  }
  return Reg;
}

IceString TargetX8632::getRegName(SizeT RegNum, Type Ty) const {
  assert(RegNum < Reg_NUM);
  static IceString RegNames8[] = {
#define X(val, init, name, name16, name8, scratch, preserved, stackptr,        \
          frameptr, isI8, isInt, isFP)                                         \
  name8,
    REGX8632_TABLE
#undef X
  };
  static IceString RegNames16[] = {
#define X(val, init, name, name16, name8, scratch, preserved, stackptr,        \
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

void TargetX8632::emitVariable(const Variable *Var, const Cfg *Func) const {
  Ostream &Str = Ctx->getStrEmit();
  assert(Var->getLocalUseNode() == NULL ||
         Var->getLocalUseNode() == Func->getCurrentNode());
  if (Var->hasReg()) {
    Str << getRegName(Var->getRegNum(), Var->getType());
    return;
  }
  Str << InstX8632::getWidthString(Var->getType());
  Str << " [" << getRegName(getFrameOrStackReg(), IceType_i32);
  int32_t Offset = Var->getStackOffset();
  if (!hasFramePointer())
    Offset += getStackAdjustment();
  if (Offset) {
    if (Offset > 0)
      Str << "+";
    Str << Offset;
  }
  Str << "]";
}

void TargetX8632::lowerArguments() {
  VarList &Args = Func->getArgs();
  // The first four arguments of vector type, regardless of their
  // position relative to the other arguments in the argument list, are
  // passed in registers xmm0 - xmm3.
  unsigned NumXmmArgs = 0;

  Context.init(Func->getEntryNode());
  Context.setInsertPoint(Context.getCur());

  for (SizeT I = 0, E = Args.size(); I < E && NumXmmArgs < X86_MAX_XMM_ARGS;
       ++I) {
    Variable *Arg = Args[I];
    Type Ty = Arg->getType();
    if (!isVectorType(Ty))
      continue;
    // Replace Arg in the argument list with the home register.  Then
    // generate an instruction in the prolog to copy the home register
    // to the assigned location of Arg.
    int32_t RegNum = Reg_xmm0 + NumXmmArgs;
    ++NumXmmArgs;
    IceString Name = "home_reg:" + Arg->getName();
    const CfgNode *DefNode = NULL;
    Variable *RegisterArg = Func->makeVariable(Ty, DefNode, Name);
    RegisterArg->setRegNum(RegNum);
    RegisterArg->setIsArg(Func);
    Arg->setIsArg(Func, false);

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
void TargetX8632::finishArgumentLowering(Variable *Arg, Variable *FramePtr,
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
  Arg->setStackOffset(BasicFrameOffset + InArgsSizeBytes);
  InArgsSizeBytes += typeWidthInBytesOnStack(Ty);
  if (Arg->hasReg()) {
    assert(Ty != IceType_i64);
    OperandX8632Mem *Mem = OperandX8632Mem::create(
        Func, Ty, FramePtr,
        Ctx->getConstantInt(IceType_i32, Arg->getStackOffset()));
    if (isVectorType(Arg->getType())) {
      _movp(Arg, Mem);
    } else {
      _mov(Arg, Mem);
    }
  }
}

Type TargetX8632::stackSlotType() { return IceType_i32; }

void TargetX8632::addProlog(CfgNode *Node) {
  // If SimpleCoalescing is false, each variable without a register
  // gets its own unique stack slot, which leads to large stack
  // frames.  If SimpleCoalescing is true, then each "global" variable
  // without a register gets its own slot, but "local" variable slots
  // are reused across basic blocks.  E.g., if A and B are local to
  // block 1 and C is local to block 2, then C may share a slot with A
  // or B.
  const bool SimpleCoalescing = true;
  size_t InArgsSizeBytes = 0;
  size_t RetIpSizeBytes = 4;
  size_t PreservedRegsSizeBytes = 0;
  LocalsSizeBytes = 0;
  Context.init(Node);
  Context.setInsertPoint(Context.getCur());

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

  llvm::SmallBitVector CalleeSaves =
      getRegisterSet(RegSet_CalleeSave, RegSet_None);

  size_t GlobalsSize = 0;
  std::vector<size_t> LocalsSize(Func->getNumNodes());

  // Prepass.  Compute RegsUsed, PreservedRegsSizeBytes, and
  // LocalsSizeBytes.
  RegsUsed = llvm::SmallBitVector(CalleeSaves.size());
  const VarList &Variables = Func->getVariables();
  const VarList &Args = Func->getArgs();
  for (VarList::const_iterator I = Variables.begin(), E = Variables.end();
       I != E; ++I) {
    Variable *Var = *I;
    if (Var->hasReg()) {
      RegsUsed[Var->getRegNum()] = true;
      continue;
    }
    // An argument either does not need a stack slot (if passed in a
    // register) or already has one (if passed on the stack).
    if (Var->getIsArg())
      continue;
    // An unreferenced variable doesn't need a stack slot.
    if (ComputedLiveRanges && Var->getLiveRange().isEmpty())
      continue;
    // A spill slot linked to a variable with a stack slot should reuse
    // that stack slot.
    if (Var->getWeight() == RegWeight::Zero && Var->getRegisterOverlap()) {
      if (Variable *Linked = Var->getPreferredRegister()) {
        if (!Linked->hasReg())
          continue;
      }
    }
    size_t Increment = typeWidthInBytesOnStack(Var->getType());
    if (SimpleCoalescing) {
      if (Var->isMultiblockLife()) {
        GlobalsSize += Increment;
      } else {
        SizeT NodeIndex = Var->getLocalUseNode()->getIndex();
        LocalsSize[NodeIndex] += Increment;
        if (LocalsSize[NodeIndex] > LocalsSizeBytes)
          LocalsSizeBytes = LocalsSize[NodeIndex];
      }
    } else {
      LocalsSizeBytes += Increment;
    }
  }
  LocalsSizeBytes += GlobalsSize;

  // Add push instructions for preserved registers.
  for (SizeT i = 0; i < CalleeSaves.size(); ++i) {
    if (CalleeSaves[i] && RegsUsed[i]) {
      PreservedRegsSizeBytes += 4;
      const bool SuppressStackAdjustment = true;
      _push(getPhysicalRegister(i), SuppressStackAdjustment);
    }
  }

  // Generate "push ebp; mov ebp, esp"
  if (IsEbpBasedFrame) {
    assert((RegsUsed & getRegisterSet(RegSet_FramePointer, RegSet_None))
               .count() == 0);
    PreservedRegsSizeBytes += 4;
    Variable *ebp = getPhysicalRegister(Reg_ebp);
    Variable *esp = getPhysicalRegister(Reg_esp);
    const bool SuppressStackAdjustment = true;
    _push(ebp, SuppressStackAdjustment);
    _mov(ebp, esp);
  }

  // Generate "sub esp, LocalsSizeBytes"
  if (LocalsSizeBytes)
    _sub(getPhysicalRegister(Reg_esp),
         Ctx->getConstantInt(IceType_i32, LocalsSizeBytes));

  resetStackAdjustment();

  // Fill in stack offsets for stack args, and copy args into registers
  // for those that were register-allocated.  Args are pushed right to
  // left, so Arg[0] is closest to the stack/frame pointer.
  Variable *FramePtr = getPhysicalRegister(getFrameOrStackReg());
  size_t BasicFrameOffset = PreservedRegsSizeBytes + RetIpSizeBytes;
  if (!IsEbpBasedFrame)
    BasicFrameOffset += LocalsSizeBytes;

  unsigned NumXmmArgs = 0;
  for (SizeT i = 0; i < Args.size(); ++i) {
    Variable *Arg = Args[i];
    // Skip arguments passed in registers.
    if (isVectorType(Arg->getType()) && NumXmmArgs < X86_MAX_XMM_ARGS) {
      ++NumXmmArgs;
      continue;
    }
    finishArgumentLowering(Arg, FramePtr, BasicFrameOffset, InArgsSizeBytes);
  }

  // Fill in stack offsets for locals.
  size_t TotalGlobalsSize = GlobalsSize;
  GlobalsSize = 0;
  LocalsSize.assign(LocalsSize.size(), 0);
  size_t NextStackOffset = 0;
  for (VarList::const_iterator I = Variables.begin(), E = Variables.end();
       I != E; ++I) {
    Variable *Var = *I;
    if (Var->hasReg()) {
      RegsUsed[Var->getRegNum()] = true;
      continue;
    }
    if (Var->getIsArg())
      continue;
    if (ComputedLiveRanges && Var->getLiveRange().isEmpty())
      continue;
    if (Var->getWeight() == RegWeight::Zero && Var->getRegisterOverlap()) {
      if (Variable *Linked = Var->getPreferredRegister()) {
        if (!Linked->hasReg()) {
          // TODO: Make sure Linked has already been assigned a stack
          // slot.
          Var->setStackOffset(Linked->getStackOffset());
          continue;
        }
      }
    }
    size_t Increment = typeWidthInBytesOnStack(Var->getType());
    if (SimpleCoalescing) {
      if (Var->isMultiblockLife()) {
        GlobalsSize += Increment;
        NextStackOffset = GlobalsSize;
      } else {
        SizeT NodeIndex = Var->getLocalUseNode()->getIndex();
        LocalsSize[NodeIndex] += Increment;
        NextStackOffset = TotalGlobalsSize + LocalsSize[NodeIndex];
      }
    } else {
      NextStackOffset += Increment;
    }
    if (IsEbpBasedFrame)
      Var->setStackOffset(-NextStackOffset);
    else
      Var->setStackOffset(LocalsSizeBytes - NextStackOffset);
  }
  this->FrameSizeLocals = NextStackOffset;
  this->HasComputedFrame = true;

  if (Func->getContext()->isVerbose(IceV_Frame)) {
    Func->getContext()->getStrDump() << "LocalsSizeBytes=" << LocalsSizeBytes
                                     << "\n"
                                     << "InArgsSizeBytes=" << InArgsSizeBytes
                                     << "\n"
                                     << "PreservedRegsSizeBytes="
                                     << PreservedRegsSizeBytes << "\n";
  }
}

void TargetX8632::addEpilog(CfgNode *Node) {
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

  Variable *esp = getPhysicalRegister(Reg_esp);
  if (IsEbpBasedFrame) {
    Variable *ebp = getPhysicalRegister(Reg_ebp);
    _mov(esp, ebp);
    _pop(ebp);
  } else {
    // add esp, LocalsSizeBytes
    if (LocalsSizeBytes)
      _add(esp, Ctx->getConstantInt(IceType_i32, LocalsSizeBytes));
  }

  // Add pop instructions for preserved registers.
  llvm::SmallBitVector CalleeSaves =
      getRegisterSet(RegSet_CalleeSave, RegSet_None);
  for (SizeT i = 0; i < CalleeSaves.size(); ++i) {
    SizeT j = CalleeSaves.size() - i - 1;
    if (j == Reg_ebp && IsEbpBasedFrame)
      continue;
    if (CalleeSaves[j] && RegsUsed[j]) {
      _pop(getPhysicalRegister(j));
    }
  }
}

template <typename T> struct PoolTypeConverter {};

template <> struct PoolTypeConverter<float> {
  typedef float PrimitiveFpType;
  typedef uint32_t PrimitiveIntType;
  typedef ConstantFloat IceType;
  static const Type Ty = IceType_f32;
  static const char *TypeName;
  static const char *AsmTag;
  static const char *PrintfString;
};
const char *PoolTypeConverter<float>::TypeName = "float";
const char *PoolTypeConverter<float>::AsmTag = ".long";
const char *PoolTypeConverter<float>::PrintfString = "0x%x";

template <> struct PoolTypeConverter<double> {
  typedef double PrimitiveFpType;
  typedef uint64_t PrimitiveIntType;
  typedef ConstantDouble IceType;
  static const Type Ty = IceType_f64;
  static const char *TypeName;
  static const char *AsmTag;
  static const char *PrintfString;
};
const char *PoolTypeConverter<double>::TypeName = "double";
const char *PoolTypeConverter<double>::AsmTag = ".quad";
const char *PoolTypeConverter<double>::PrintfString = "0x%llx";

template <typename T> void TargetX8632::emitConstantPool() const {
  Ostream &Str = Ctx->getStrEmit();
  Type Ty = T::Ty;
  SizeT Align = typeAlignInBytes(Ty);
  ConstantList Pool = Ctx->getConstantPool(Ty);

  Str << "\t.section\t.rodata.cst" << Align << ",\"aM\",@progbits," << Align
      << "\n";
  Str << "\t.align\t" << Align << "\n";
  for (ConstantList::const_iterator I = Pool.begin(), E = Pool.end(); I != E;
       ++I) {
    typename T::IceType *Const = llvm::cast<typename T::IceType>(*I);
    typename T::PrimitiveFpType Value = Const->getValue();
    // Use memcpy() to copy bits from Value into RawValue in a way
    // that avoids breaking strict-aliasing rules.
    typename T::PrimitiveIntType RawValue;
    memcpy(&RawValue, &Value, sizeof(Value));
    char buf[30];
    int CharsPrinted =
        snprintf(buf, llvm::array_lengthof(buf), T::PrintfString, RawValue);
    assert(CharsPrinted >= 0 &&
           (size_t)CharsPrinted < llvm::array_lengthof(buf));
    (void)CharsPrinted; // avoid warnings if asserts are disabled
    Str << "L$" << Ty << "$" << Const->getPoolEntryID() << ":\n";
    Str << "\t" << T::AsmTag << "\t" << buf << "\t# " << T::TypeName << " "
        << Value << "\n";
  }
}

void TargetX8632::emitConstants() const {
  emitConstantPool<PoolTypeConverter<float> >();
  emitConstantPool<PoolTypeConverter<double> >();

  // No need to emit constants from the int pool since (for x86) they
  // are embedded as immediates in the instructions.
}

void TargetX8632::split64(Variable *Var) {
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
  assert(Hi == NULL);
  Lo = Func->makeVariable(IceType_i32, Context.getNode(),
                          Var->getName() + "__lo");
  Hi = Func->makeVariable(IceType_i32, Context.getNode(),
                          Var->getName() + "__hi");
  Var->setLoHi(Lo, Hi);
  if (Var->getIsArg()) {
    Lo->setIsArg(Func);
    Hi->setIsArg(Func);
  }
}

Operand *TargetX8632::loOperand(Operand *Operand) {
  assert(Operand->getType() == IceType_i64);
  if (Operand->getType() != IceType_i64)
    return Operand;
  if (Variable *Var = llvm::dyn_cast<Variable>(Operand)) {
    split64(Var);
    return Var->getLo();
  }
  if (ConstantInteger *Const = llvm::dyn_cast<ConstantInteger>(Operand)) {
    uint64_t Mask = (1ull << 32) - 1;
    return Ctx->getConstantInt(IceType_i32, Const->getValue() & Mask);
  }
  if (OperandX8632Mem *Mem = llvm::dyn_cast<OperandX8632Mem>(Operand)) {
    return OperandX8632Mem::create(Func, IceType_i32, Mem->getBase(),
                                   Mem->getOffset(), Mem->getIndex(),
                                   Mem->getShift(), Mem->getSegmentRegister());
  }
  llvm_unreachable("Unsupported operand type");
  return NULL;
}

Operand *TargetX8632::hiOperand(Operand *Operand) {
  assert(Operand->getType() == IceType_i64);
  if (Operand->getType() != IceType_i64)
    return Operand;
  if (Variable *Var = llvm::dyn_cast<Variable>(Operand)) {
    split64(Var);
    return Var->getHi();
  }
  if (ConstantInteger *Const = llvm::dyn_cast<ConstantInteger>(Operand)) {
    return Ctx->getConstantInt(IceType_i32, Const->getValue() >> 32);
  }
  if (OperandX8632Mem *Mem = llvm::dyn_cast<OperandX8632Mem>(Operand)) {
    Constant *Offset = Mem->getOffset();
    if (Offset == NULL)
      Offset = Ctx->getConstantInt(IceType_i32, 4);
    else if (ConstantInteger *IntOffset =
                 llvm::dyn_cast<ConstantInteger>(Offset)) {
      Offset = Ctx->getConstantInt(IceType_i32, 4 + IntOffset->getValue());
    } else if (ConstantRelocatable *SymOffset =
                   llvm::dyn_cast<ConstantRelocatable>(Offset)) {
      Offset = Ctx->getConstantSym(IceType_i32, 4 + SymOffset->getOffset(),
                                   SymOffset->getName());
    }
    return OperandX8632Mem::create(Func, IceType_i32, Mem->getBase(), Offset,
                                   Mem->getIndex(), Mem->getShift(),
                                   Mem->getSegmentRegister());
  }
  llvm_unreachable("Unsupported operand type");
  return NULL;
}

llvm::SmallBitVector TargetX8632::getRegisterSet(RegSetMask Include,
                                                 RegSetMask Exclude) const {
  llvm::SmallBitVector Registers(Reg_NUM);

#define X(val, init, name, name16, name8, scratch, preserved, stackptr,        \
          frameptr, isI8, isInt, isFP)                                         \
  if (scratch && (Include & RegSet_CallerSave))                                \
    Registers[val] = true;                                                     \
  if (preserved && (Include & RegSet_CalleeSave))                              \
    Registers[val] = true;                                                     \
  if (stackptr && (Include & RegSet_StackPointer))                             \
    Registers[val] = true;                                                     \
  if (frameptr && (Include & RegSet_FramePointer))                             \
    Registers[val] = true;                                                     \
  if (scratch && (Exclude & RegSet_CallerSave))                                \
    Registers[val] = false;                                                    \
  if (preserved && (Exclude & RegSet_CalleeSave))                              \
    Registers[val] = false;                                                    \
  if (stackptr && (Exclude & RegSet_StackPointer))                             \
    Registers[val] = false;                                                    \
  if (frameptr && (Exclude & RegSet_FramePointer))                             \
    Registers[val] = false;

  REGX8632_TABLE

#undef X

  return Registers;
}

void TargetX8632::lowerAlloca(const InstAlloca *Inst) {
  IsEbpBasedFrame = true;
  // TODO(sehr,stichnot): align allocated memory, keep stack aligned, minimize
  // the number of adjustments of esp, etc.
  Variable *esp = getPhysicalRegister(Reg_esp);
  Operand *TotalSize = legalize(Inst->getSizeInBytes());
  Variable *Dest = Inst->getDest();
  _sub(esp, TotalSize);
  _mov(Dest, esp);
}

void TargetX8632::lowerArithmetic(const InstArithmetic *Inst) {
  Variable *Dest = Inst->getDest();
  Operand *Src0 = legalize(Inst->getSrc(0));
  Operand *Src1 = legalize(Inst->getSrc(1));
  if (Dest->getType() == IceType_i64) {
    Variable *DestLo = llvm::cast<Variable>(loOperand(Dest));
    Variable *DestHi = llvm::cast<Variable>(hiOperand(Dest));
    Operand *Src0Lo = loOperand(Src0);
    Operand *Src0Hi = hiOperand(Src0);
    Operand *Src1Lo = loOperand(Src1);
    Operand *Src1Hi = hiOperand(Src1);
    Variable *T_Lo = NULL, *T_Hi = NULL;
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
      Variable *T_1 = NULL, *T_2 = NULL, *T_3 = NULL;
      Variable *T_4Lo = makeReg(IceType_i32, Reg_eax);
      Variable *T_4Hi = makeReg(IceType_i32, Reg_edx);
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
      _mov(T_1, Src0Hi);
      _imul(T_1, Src1Lo);
      _mov(T_2, Src1Hi);
      _imul(T_2, Src0Lo);
      _mov(T_3, Src0Lo, Reg_eax);
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
      Variable *T_1 = NULL, *T_2 = NULL, *T_3 = NULL;
      Constant *BitTest = Ctx->getConstantInt(IceType_i32, 0x20);
      Constant *Zero = Ctx->getConstantZero(IceType_i32);
      InstX8632Label *Label = InstX8632Label::create(Func, this);
      _mov(T_1, Src1Lo, Reg_ecx);
      _mov(T_2, Src0Lo);
      _mov(T_3, Src0Hi);
      _shld(T_3, T_2, T_1);
      _shl(T_2, T_1);
      _test(T_1, BitTest);
      _br(InstX8632Br::Br_e, Label);
      // Because of the intra-block control flow, we need to fake a use
      // of T_3 to prevent its earlier definition from being dead-code
      // eliminated in the presence of its later definition.
      Context.insert(InstFakeUse::create(Func, T_3));
      _mov(T_3, T_2);
      _mov(T_2, Zero);
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
      Variable *T_1 = NULL, *T_2 = NULL, *T_3 = NULL;
      Constant *BitTest = Ctx->getConstantInt(IceType_i32, 0x20);
      Constant *Zero = Ctx->getConstantZero(IceType_i32);
      InstX8632Label *Label = InstX8632Label::create(Func, this);
      _mov(T_1, Src1Lo, Reg_ecx);
      _mov(T_2, Src0Lo);
      _mov(T_3, Src0Hi);
      _shrd(T_2, T_3, T_1);
      _shr(T_3, T_1);
      _test(T_1, BitTest);
      _br(InstX8632Br::Br_e, Label);
      // Because of the intra-block control flow, we need to fake a use
      // of T_3 to prevent its earlier definition from being dead-code
      // eliminated in the presence of its later definition.
      Context.insert(InstFakeUse::create(Func, T_2));
      _mov(T_2, T_3);
      _mov(T_3, Zero);
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
      Variable *T_1 = NULL, *T_2 = NULL, *T_3 = NULL;
      Constant *BitTest = Ctx->getConstantInt(IceType_i32, 0x20);
      Constant *SignExtend = Ctx->getConstantInt(IceType_i32, 0x1f);
      InstX8632Label *Label = InstX8632Label::create(Func, this);
      _mov(T_1, Src1Lo, Reg_ecx);
      _mov(T_2, Src0Lo);
      _mov(T_3, Src0Hi);
      _shrd(T_2, T_3, T_1);
      _sar(T_3, T_1);
      _test(T_1, BitTest);
      _br(InstX8632Br::Br_e, Label);
      // Because of the intra-block control flow, we need to fake a use
      // of T_3 to prevent its earlier definition from being dead-code
      // eliminated in the presence of its later definition.
      Context.insert(InstFakeUse::create(Func, T_2));
      _mov(T_2, T_3);
      _sar(T_3, SignExtend);
      Context.insert(Label);
      _mov(DestLo, T_2);
      _mov(DestHi, T_3);
    } break;
    case InstArithmetic::Udiv: {
      const SizeT MaxSrcs = 2;
      InstCall *Call = makeHelperCall("__udivdi3", Dest, MaxSrcs);
      Call->addArg(Inst->getSrc(0));
      Call->addArg(Inst->getSrc(1));
      lowerCall(Call);
    } break;
    case InstArithmetic::Sdiv: {
      const SizeT MaxSrcs = 2;
      InstCall *Call = makeHelperCall("__divdi3", Dest, MaxSrcs);
      Call->addArg(Inst->getSrc(0));
      Call->addArg(Inst->getSrc(1));
      lowerCall(Call);
    } break;
    case InstArithmetic::Urem: {
      const SizeT MaxSrcs = 2;
      InstCall *Call = makeHelperCall("__umoddi3", Dest, MaxSrcs);
      Call->addArg(Inst->getSrc(0));
      Call->addArg(Inst->getSrc(1));
      lowerCall(Call);
    } break;
    case InstArithmetic::Srem: {
      const SizeT MaxSrcs = 2;
      InstCall *Call = makeHelperCall("__moddi3", Dest, MaxSrcs);
      Call->addArg(Inst->getSrc(0));
      Call->addArg(Inst->getSrc(1));
      lowerCall(Call);
    } break;
    case InstArithmetic::Fadd:
    case InstArithmetic::Fsub:
    case InstArithmetic::Fmul:
    case InstArithmetic::Fdiv:
    case InstArithmetic::Frem:
      llvm_unreachable("FP instruction with i64 type");
      break;
    }
  } else if (isVectorType(Dest->getType())) {
    // TODO: Trap on integer divide and integer modulo by zero.
    // See: https://code.google.com/p/nativeclient/issues/detail?id=3899
    //
    // TODO(wala): ALIGNHACK: All vector arithmetic is currently done in
    // registers.  This is a workaround of the fact that there is no
    // support for aligning stack operands.  Once there is support,
    // remove LEGAL_HACK.
#define LEGAL_HACK(s) legalizeToVar((s))
    switch (Inst->getOp()) {
    case InstArithmetic::_num:
      llvm_unreachable("Unknown arithmetic operator");
      break;
    case InstArithmetic::Add: {
      Variable *T = makeReg(Dest->getType());
      _movp(T, Src0);
      _padd(T, LEGAL_HACK(Src1));
      _movp(Dest, T);
    } break;
    case InstArithmetic::And: {
      Variable *T = makeReg(Dest->getType());
      _movp(T, Src0);
      _pand(T, LEGAL_HACK(Src1));
      _movp(Dest, T);
    } break;
    case InstArithmetic::Or: {
      Variable *T = makeReg(Dest->getType());
      _movp(T, Src0);
      _por(T, LEGAL_HACK(Src1));
      _movp(Dest, T);
    } break;
    case InstArithmetic::Xor: {
      Variable *T = makeReg(Dest->getType());
      _movp(T, Src0);
      _pxor(T, LEGAL_HACK(Src1));
      _movp(Dest, T);
    } break;
    case InstArithmetic::Sub: {
      Variable *T = makeReg(Dest->getType());
      _movp(T, Src0);
      _psub(T, LEGAL_HACK(Src1));
      _movp(Dest, T);
    } break;
    case InstArithmetic::Mul: {
      if (Dest->getType() == IceType_v4i32) {
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
        //
        // TODO(wala): SSE4.1 has pmulld.

        // Mask that directs pshufd to create a vector with entries
        // Src[1, 0, 3, 0]
        const unsigned Constant1030 = 0x31;
        Constant *Mask1030 = Ctx->getConstantInt(IceType_i8, Constant1030);
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
        // TODO(wala): ALIGHNHACK: Replace Src0R with Src0 and Src1R
        // with Src1 after stack operand alignment support is
        // implemented.
        Variable *Src0R = LEGAL_HACK(Src0);
        Variable *Src1R = LEGAL_HACK(Src1);
        _pshufd(T2, Src0R, Mask1030);
        _pshufd(T3, Src1R, Mask1030);
        _pmuludq(T1, Src1R);
        _pmuludq(T2, T3);
        _shufps(T1, T2, Ctx->getConstantInt(IceType_i8, Mask0202));
        _pshufd(T4, T1, Ctx->getConstantInt(IceType_i8, Mask0213));
        _movp(Dest, T4);
      } else if (Dest->getType() == IceType_v8i16) {
        Variable *T = makeReg(IceType_v8i16);
        _movp(T, Src0);
        _pmullw(T, legalizeToVar(Src1));
        _movp(Dest, T);
      } else {
        assert(Dest->getType() == IceType_v16i8);
        // Sz_mul_v16i8
        const IceString Helper = "Sz_mul_v16i8";
        const SizeT MaxSrcs = 2;
        InstCall *Call = makeHelperCall(Helper, Dest, MaxSrcs);
        Call->addArg(Src0);
        Call->addArg(Src1);
        lowerCall(Call);
      }
    } break;
    case InstArithmetic::Shl: {
      // Sz_shl_v4i32, Sz_shl_v8i16, Sz_shl_v16i8
      const IceString Helper = "Sz_shl_" + typeIdentString(Dest->getType());
      const SizeT MaxSrcs = 2;
      InstCall *Call = makeHelperCall(Helper, Dest, MaxSrcs);
      Call->addArg(Src0);
      Call->addArg(Src1);
      lowerCall(Call);
    } break;
    case InstArithmetic::Lshr: {
      // Sz_lshr_v4i32, Sz_lshr_v8i16, Sz_lshr_v16i8
      const IceString Helper = "Sz_lshr_" + typeIdentString(Dest->getType());
      const SizeT MaxSrcs = 2;
      InstCall *Call = makeHelperCall(Helper, Dest, MaxSrcs);
      Call->addArg(Src0);
      Call->addArg(Src1);
      lowerCall(Call);
    } break;
    case InstArithmetic::Ashr: {
      // Sz_ashr_v4i32, Sz_ashr_v8i16, Sz_ashr_v16i8
      const IceString Helper = "Sz_ashr_" + typeIdentString(Dest->getType());
      const SizeT MaxSrcs = 2;
      InstCall *Call = makeHelperCall(Helper, Dest, MaxSrcs);
      Call->addArg(Src0);
      Call->addArg(Src1);
      lowerCall(Call);
    } break;
    case InstArithmetic::Udiv: {
      // Sz_udiv_v4i32, Sz_udiv_v8i16, Sz_udiv_v16i8
      const IceString Helper = "Sz_udiv_" + typeIdentString(Dest->getType());
      const SizeT MaxSrcs = 2;
      InstCall *Call = makeHelperCall(Helper, Dest, MaxSrcs);
      Call->addArg(Src0);
      Call->addArg(Src1);
      lowerCall(Call);
    } break;
    case InstArithmetic::Sdiv: {
      // Sz_sdiv_v4i32, Sz_sdiv_v8i16, Sz_sdiv_v16i8
      const IceString Helper = "Sz_sdiv_" + typeIdentString(Dest->getType());
      const SizeT MaxSrcs = 2;
      InstCall *Call = makeHelperCall(Helper, Dest, MaxSrcs);
      Call->addArg(Src0);
      Call->addArg(Src1);
      lowerCall(Call);
    } break;
    case InstArithmetic::Urem: {
      // Sz_urem_v4i32, Sz_urem_v8i16, Sz_urem_v16i8
      const IceString Helper = "Sz_urem_" + typeIdentString(Dest->getType());
      const SizeT MaxSrcs = 2;
      InstCall *Call = makeHelperCall(Helper, Dest, MaxSrcs);
      Call->addArg(Src0);
      Call->addArg(Src1);
      lowerCall(Call);
    } break;
    case InstArithmetic::Srem: {
      // Sz_srem_v4i32, Sz_srem_v8i16, Sz_srem_v16i8
      const IceString Helper = "Sz_srem_" + typeIdentString(Dest->getType());
      const SizeT MaxSrcs = 2;
      InstCall *Call = makeHelperCall(Helper, Dest, MaxSrcs);
      Call->addArg(Src0);
      Call->addArg(Src1);
      lowerCall(Call);
    } break;
    case InstArithmetic::Fadd: {
      Variable *T = makeReg(Dest->getType());
      _movp(T, Src0);
      _addps(T, LEGAL_HACK(Src1));
      _movp(Dest, T);
    } break;
    case InstArithmetic::Fsub: {
      Variable *T = makeReg(Dest->getType());
      _movp(T, Src0);
      _subps(T, LEGAL_HACK(Src1));
      _movp(Dest, T);
    } break;
    case InstArithmetic::Fmul: {
      Variable *T = makeReg(Dest->getType());
      _movp(T, Src0);
      _mulps(T, LEGAL_HACK(Src1));
      _movp(Dest, T);
    } break;
    case InstArithmetic::Fdiv: {
      Variable *T = makeReg(Dest->getType());
      _movp(T, Src0);
      _divps(T, LEGAL_HACK(Src1));
      _movp(Dest, T);
    } break;
    case InstArithmetic::Frem: {
      const SizeT MaxSrcs = 2;
      InstCall *Call = makeHelperCall("Sz_frem_v4f32", Dest, MaxSrcs);
      Call->addArg(Src0);
      Call->addArg(Src1);
      lowerCall(Call);
    } break;
    }
#undef LEGAL_HACK
  } else { // Dest->getType() is non-i64 scalar
    Variable *T_edx = NULL;
    Variable *T = NULL;
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
      // TODO: Optimize for llvm::isa<Constant>(Src1)
      // TODO: Strength-reduce multiplications by a constant,
      // particularly -1 and powers of 2.  Advanced: use lea to
      // multiply by 3, 5, 9.
      //
      // The 8-bit version of imul only allows the form "imul r/m8"
      // where T must be in eax.
      if (Dest->getType() == IceType_i8)
        _mov(T, Src0, Reg_eax);
      else
        _mov(T, Src0);
      _imul(T, Src1);
      _mov(Dest, T);
      break;
    case InstArithmetic::Shl:
      _mov(T, Src0);
      if (!llvm::isa<Constant>(Src1))
        Src1 = legalizeToVar(Src1, false, Reg_ecx);
      _shl(T, Src1);
      _mov(Dest, T);
      break;
    case InstArithmetic::Lshr:
      _mov(T, Src0);
      if (!llvm::isa<Constant>(Src1))
        Src1 = legalizeToVar(Src1, false, Reg_ecx);
      _shr(T, Src1);
      _mov(Dest, T);
      break;
    case InstArithmetic::Ashr:
      _mov(T, Src0);
      if (!llvm::isa<Constant>(Src1))
        Src1 = legalizeToVar(Src1, false, Reg_ecx);
      _sar(T, Src1);
      _mov(Dest, T);
      break;
    case InstArithmetic::Udiv:
      // div and idiv are the few arithmetic operators that do not allow
      // immediates as the operand.
      Src1 = legalize(Src1, Legal_Reg | Legal_Mem);
      if (Dest->getType() == IceType_i8) {
        Variable *T_ah = NULL;
        Constant *Zero = Ctx->getConstantZero(IceType_i8);
        _mov(T, Src0, Reg_eax);
        _mov(T_ah, Zero, Reg_ah);
        _div(T, Src1, T_ah);
        _mov(Dest, T);
      } else {
        Constant *Zero = Ctx->getConstantZero(IceType_i32);
        _mov(T, Src0, Reg_eax);
        _mov(T_edx, Zero, Reg_edx);
        _div(T, Src1, T_edx);
        _mov(Dest, T);
      }
      break;
    case InstArithmetic::Sdiv:
      Src1 = legalize(Src1, Legal_Reg | Legal_Mem);
      T_edx = makeReg(IceType_i32, Reg_edx);
      _mov(T, Src0, Reg_eax);
      _cdq(T_edx, T);
      _idiv(T, Src1, T_edx);
      _mov(Dest, T);
      break;
    case InstArithmetic::Urem:
      Src1 = legalize(Src1, Legal_Reg | Legal_Mem);
      if (Dest->getType() == IceType_i8) {
        Variable *T_ah = NULL;
        Constant *Zero = Ctx->getConstantZero(IceType_i8);
        _mov(T, Src0, Reg_eax);
        _mov(T_ah, Zero, Reg_ah);
        _div(T_ah, Src1, T);
        _mov(Dest, T_ah);
      } else {
        Constant *Zero = Ctx->getConstantZero(IceType_i32);
        _mov(T_edx, Zero, Reg_edx);
        _mov(T, Src0, Reg_eax);
        _div(T_edx, Src1, T);
        _mov(Dest, T_edx);
      }
      break;
    case InstArithmetic::Srem:
      Src1 = legalize(Src1, Legal_Reg | Legal_Mem);
      T_edx = makeReg(IceType_i32, Reg_edx);
      _mov(T, Src0, Reg_eax);
      _cdq(T_edx, T);
      _idiv(T_edx, Src1, T);
      _mov(Dest, T_edx);
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
      InstCall *Call =
          makeHelperCall(Ty == IceType_f32 ? "fmodf" : "fmod", Dest, MaxSrcs);
      Call->addArg(Src0);
      Call->addArg(Src1);
      return lowerCall(Call);
    } break;
    }
  }
}

void TargetX8632::lowerAssign(const InstAssign *Inst) {
  Variable *Dest = Inst->getDest();
  Operand *Src0 = Inst->getSrc(0);
  assert(Dest->getType() == Src0->getType());
  if (Dest->getType() == IceType_i64) {
    Src0 = legalize(Src0);
    Operand *Src0Lo = loOperand(Src0);
    Operand *Src0Hi = hiOperand(Src0);
    Variable *DestLo = llvm::cast<Variable>(loOperand(Dest));
    Variable *DestHi = llvm::cast<Variable>(hiOperand(Dest));
    Variable *T_Lo = NULL, *T_Hi = NULL;
    _mov(T_Lo, Src0Lo);
    _mov(DestLo, T_Lo);
    _mov(T_Hi, Src0Hi);
    _mov(DestHi, T_Hi);
  } else {
    const bool AllowOverlap = true;
    // RI is either a physical register or an immediate.
    Operand *RI = legalize(Src0, Legal_Reg | Legal_Imm, AllowOverlap);
    if (isVectorType(Dest->getType()))
      _movp(Dest, RI);
    else
      _mov(Dest, RI);
  }
}

void TargetX8632::lowerBr(const InstBr *Inst) {
  if (Inst->isUnconditional()) {
    _br(Inst->getTargetUnconditional());
  } else {
    Operand *Src0 = legalize(Inst->getCondition());
    Constant *Zero = Ctx->getConstantZero(IceType_i32);
    _cmp(Src0, Zero);
    _br(InstX8632Br::Br_ne, Inst->getTargetTrue(), Inst->getTargetFalse());
  }
}

void TargetX8632::lowerCall(const InstCall *Instr) {
  // Classify each argument operand according to the location where the
  // argument is passed.
  OperandList XmmArgs;
  OperandList StackArgs;
  for (SizeT i = 0, NumArgs = Instr->getNumArgs(); i < NumArgs; ++i) {
    Operand *Arg = Instr->getArg(i);
    if (isVectorType(Arg->getType()) && XmmArgs.size() < X86_MAX_XMM_ARGS) {
      XmmArgs.push_back(Arg);
    } else {
      StackArgs.push_back(Arg);
    }
  }
  // For stack arguments, generate a sequence of push instructions,
  // pushing right to left, keeping track of stack offsets in case a
  // push involves a stack operand and we are using an esp-based frame.
  uint32_t StackOffset = 0;
  // TODO: Consolidate the stack adjustment for function calls by
  // reserving enough space for the arguments only once.
  //
  // TODO: If for some reason the call instruction gets dead-code
  // eliminated after lowering, we would need to ensure that the
  // pre-call push instructions and the post-call esp adjustment get
  // eliminated as well.
  for (OperandList::reverse_iterator I = StackArgs.rbegin(),
           E = StackArgs.rend(); I != E; ++I) {
    Operand *Arg = legalize(*I);
    if (Arg->getType() == IceType_i64) {
      _push(hiOperand(Arg));
      _push(loOperand(Arg));
    } else if (Arg->getType() == IceType_f64 || isVectorType(Arg->getType())) {
      // If the Arg turns out to be a memory operand, more than one push
      // instruction is required.  This ends up being somewhat clumsy in
      // the current IR, so we use a workaround.  Force the operand into
      // a (xmm) register, and then push the register.  An xmm register
      // push is actually not possible in x86, but the Push instruction
      // emitter handles this by decrementing the stack pointer and
      // directly writing the xmm register value.
      _push(legalize(Arg, Legal_Reg));
    } else {
      // Otherwise PNaCl requires parameter types to be at least 32-bits.
      assert(Arg->getType() == IceType_f32 || Arg->getType() == IceType_i32);
      _push(Arg);
    }
    StackOffset += typeWidthInBytesOnStack(Arg->getType());
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
    Variable *Reg = legalizeToVar(XmmArgs[i], false, Reg_xmm0 + i);
    // Generate a FakeUse of register arguments so that they do not get
    // dead code eliminated as a result of the FakeKill of scratch
    // registers after the call.
    Context.insert(InstFakeUse::create(Func, Reg));
  }
  // Generate the call instruction.  Assign its result to a temporary
  // with high register allocation weight.
  Variable *Dest = Instr->getDest();
  // ReturnReg doubles as ReturnRegLo as necessary.
  Variable *ReturnReg = NULL;
  Variable *ReturnRegHi = NULL;
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
      ReturnReg = makeReg(Dest->getType(), Reg_eax);
      break;
    case IceType_i64:
      ReturnReg = makeReg(IceType_i32, Reg_eax);
      ReturnRegHi = makeReg(IceType_i32, Reg_edx);
      break;
    case IceType_f32:
    case IceType_f64:
      // Leave ReturnReg==ReturnRegHi==NULL, and capture the result with
      // the fstp instruction.
      break;
    case IceType_v4i1:
    case IceType_v8i1:
    case IceType_v16i1:
    case IceType_v16i8:
    case IceType_v8i16:
    case IceType_v4i32:
    case IceType_v4f32:
      ReturnReg = makeReg(Dest->getType(), Reg_xmm0);
      break;
    }
  }
  // TODO(stichnot): LEAHACK: remove Legal_All (and use default) once
  // a proper emitter is used.
  Operand *CallTarget = legalize(Instr->getCallTarget(), Legal_All);
  Inst *NewCall = InstX8632Call::create(Func, ReturnReg, CallTarget);
  Context.insert(NewCall);
  if (ReturnRegHi)
    Context.insert(InstFakeDef::create(Func, ReturnRegHi));

  // Add the appropriate offset to esp.
  if (StackOffset) {
    Variable *esp = Func->getTarget()->getPhysicalRegister(Reg_esp);
    _add(esp, Ctx->getConstantInt(IceType_i32, StackOffset));
  }

  // Insert a register-kill pseudo instruction.
  VarList KilledRegs;
  for (SizeT i = 0; i < ScratchRegs.size(); ++i) {
    if (ScratchRegs[i])
      KilledRegs.push_back(Func->getTarget()->getPhysicalRegister(i));
  }
  Context.insert(InstFakeKill::create(Func, KilledRegs, NewCall));

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
      DestLo->setPreferredRegister(ReturnReg, false);
      DestHi->setPreferredRegister(ReturnRegHi, false);
      _mov(DestLo, ReturnReg);
      _mov(DestHi, ReturnRegHi);
    } else {
      assert(Dest->getType() == IceType_i32 || Dest->getType() == IceType_i16 ||
             Dest->getType() == IceType_i8 || Dest->getType() == IceType_i1 ||
             isVectorType(Dest->getType()));
      Dest->setPreferredRegister(ReturnReg, false);
      if (isVectorType(Dest->getType())) {
        _movp(Dest, ReturnReg);
      } else {
        _mov(Dest, ReturnReg);
      }
    }
  } else if (Dest->getType() == IceType_f32 || Dest->getType() == IceType_f64) {
    // Special treatment for an FP function which returns its result in
    // st(0).
    _fstp(Dest);
    // If Dest ends up being a physical xmm register, the fstp emit code
    // will route st(0) through a temporary stack slot.
  }
}

void TargetX8632::lowerCast(const InstCast *Inst) {
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
    if (Dest->getType() == IceType_i64) {
      // t1=movsx src; t2=t1; t2=sar t2, 31; dst.lo=t1; dst.hi=t2
      Variable *DestLo = llvm::cast<Variable>(loOperand(Dest));
      Variable *DestHi = llvm::cast<Variable>(hiOperand(Dest));
      Variable *T_Lo = makeReg(DestLo->getType());
      if (Src0RM->getType() == IceType_i32)
        _mov(T_Lo, Src0RM);
      else
        _movsx(T_Lo, Src0RM);
      _mov(DestLo, T_Lo);
      Variable *T_Hi = NULL;
      Constant *Shift = Ctx->getConstantInt(IceType_i32, 31);
      _mov(T_Hi, T_Lo);
      _sar(T_Hi, Shift);
      _mov(DestHi, T_Hi);
    } else if (isVectorType(Dest->getType())) {
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
            X86_CHAR_BIT * typeWidthInBytes(typeElementType(DestTy)) - 1;
        Constant *ShiftConstant = Ctx->getConstantInt(IceType_i8, ShiftAmount);
        Variable *T = makeReg(DestTy);
        _movp(T, Src0RM);
        _psll(T, ShiftConstant);
        _psra(T, ShiftConstant);
        _movp(Dest, T);
      }
    } else {
      // TODO: Sign-extend an i1 via "shl reg, 31; sar reg, 31", and
      // also copy to the high operand of a 64-bit variable.
      // t1 = movsx src; dst = t1
      Variable *T = makeReg(Dest->getType());
      _movsx(T, Src0RM);
      _mov(Dest, T);
    }
    break;
  }
  case InstCast::Zext: {
    Operand *Src0RM = legalize(Inst->getSrc(0), Legal_Reg | Legal_Mem);
    if (Dest->getType() == IceType_i64) {
      // t1=movzx src; dst.lo=t1; dst.hi=0
      Constant *Zero = Ctx->getConstantZero(IceType_i32);
      Variable *DestLo = llvm::cast<Variable>(loOperand(Dest));
      Variable *DestHi = llvm::cast<Variable>(hiOperand(Dest));
      Variable *Tmp = makeReg(DestLo->getType());
      if (Src0RM->getType() == IceType_i32)
        _mov(Tmp, Src0RM);
      else
        _movzx(Tmp, Src0RM);
      _mov(DestLo, Tmp);
      _mov(DestHi, Zero);
    } else if (Src0RM->getType() == IceType_i1) {
      // t = Src0RM; t &= 1; Dest = t
      Operand *One = Ctx->getConstantInt(IceType_i32, 1);
      Variable *T = makeReg(IceType_i32);
      _movzx(T, Src0RM);
      _and(T, One);
      _mov(Dest, T);
    } else if (isVectorType(Dest->getType())) {
      // onemask = materialize(1,1,...); dest = onemask & src
      Type DestTy = Dest->getType();
      Variable *OneMask = makeVectorOfOnes(DestTy);
      Variable *T = makeReg(DestTy);
      _movp(T, Src0RM);
      _pand(T, OneMask);
      _movp(Dest, T);
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
      Variable *T = NULL;
      _mov(T, Src0RM);
      _mov(Dest, T);
    }
    break;
  }
  case InstCast::Fptrunc:
  case InstCast::Fpext: {
    Operand *Src0RM = legalize(Inst->getSrc(0), Legal_Reg | Legal_Mem);
    // t1 = cvt Src0RM; Dest = t1
    Variable *T = makeReg(Dest->getType());
    _cvt(T, Src0RM);
    _mov(Dest, T);
    break;
  }
  case InstCast::Fptosi:
    if (isVectorType(Dest->getType())) {
      assert(Dest->getType() == IceType_v4i32 &&
             Inst->getSrc(0)->getType() == IceType_v4f32);
      Operand *Src0RM = legalize(Inst->getSrc(0), Legal_Reg | Legal_Mem);
      Variable *T = makeReg(Dest->getType());
      _cvt(T, Src0RM);
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
      InstCall *Call = makeHelperCall(
          SrcType == IceType_f32 ? "cvtftosi64" : "cvtdtosi64", Dest, MaxSrcs);
      // TODO: Call the correct compiler-rt helper function.
      Call->addArg(Inst->getSrc(0));
      lowerCall(Call);
    } else {
      Operand *Src0RM = legalize(Inst->getSrc(0), Legal_Reg | Legal_Mem);
      // t1.i32 = cvt Src0RM; t2.dest_type = t1; Dest = t2.dest_type
      Variable *T_1 = makeReg(IceType_i32);
      Variable *T_2 = makeReg(Dest->getType());
      _cvt(T_1, Src0RM);
      _mov(T_2, T_1); // T_1 and T_2 may have different integer types
      _mov(Dest, T_2);
      T_2->setPreferredRegister(T_1, true);
    }
    break;
  case InstCast::Fptoui:
    if (isVectorType(Dest->getType())) {
      assert(Dest->getType() == IceType_v4i32 &&
             Inst->getSrc(0)->getType() == IceType_v4f32);
      const SizeT MaxSrcs = 1;
      InstCall *Call = makeHelperCall("Sz_fptoui_v4f32", Dest, MaxSrcs);
      Call->addArg(Inst->getSrc(0));
      lowerCall(Call);
    } else if (Dest->getType() == IceType_i64 ||
               Dest->getType() == IceType_i32) {
      // Use a helper for both x86-32 and x86-64.
      split64(Dest);
      const SizeT MaxSrcs = 1;
      Type DestType = Dest->getType();
      Type SrcType = Inst->getSrc(0)->getType();
      IceString DstSubstring = (DestType == IceType_i64 ? "64" : "32");
      IceString SrcSubstring = (SrcType == IceType_f32 ? "f" : "d");
      // Possibilities are cvtftoui32, cvtdtoui32, cvtftoui64, cvtdtoui64
      IceString TargetString = "cvt" + SrcSubstring + "toui" + DstSubstring;
      // TODO: Call the correct compiler-rt helper function.
      InstCall *Call = makeHelperCall(TargetString, Dest, MaxSrcs);
      Call->addArg(Inst->getSrc(0));
      lowerCall(Call);
      return;
    } else {
      Operand *Src0RM = legalize(Inst->getSrc(0), Legal_Reg | Legal_Mem);
      // t1.i32 = cvt Src0RM; t2.dest_type = t1; Dest = t2.dest_type
      Variable *T_1 = makeReg(IceType_i32);
      Variable *T_2 = makeReg(Dest->getType());
      _cvt(T_1, Src0RM);
      _mov(T_2, T_1); // T_1 and T_2 may have different integer types
      _mov(Dest, T_2);
      T_2->setPreferredRegister(T_1, true);
    }
    break;
  case InstCast::Sitofp:
    if (isVectorType(Dest->getType())) {
      assert(Dest->getType() == IceType_v4f32 &&
             Inst->getSrc(0)->getType() == IceType_v4i32);
      Operand *Src0RM = legalize(Inst->getSrc(0), Legal_Reg | Legal_Mem);
      Variable *T = makeReg(Dest->getType());
      _cvt(T, Src0RM);
      _movp(Dest, T);
    } else if (Inst->getSrc(0)->getType() == IceType_i64) {
      // Use a helper for x86-32.
      const SizeT MaxSrcs = 1;
      Type DestType = Dest->getType();
      InstCall *Call = makeHelperCall(
          DestType == IceType_f32 ? "cvtsi64tof" : "cvtsi64tod", Dest, MaxSrcs);
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
      _cvt(T_2, T_1);
      _mov(Dest, T_2);
    }
    break;
  case InstCast::Uitofp: {
    Operand *Src0 = Inst->getSrc(0);
    if (isVectorType(Src0->getType())) {
      assert(Dest->getType() == IceType_v4f32 &&
             Src0->getType() == IceType_v4i32);
      const SizeT MaxSrcs = 1;
      InstCall *Call = makeHelperCall("Sz_uitofp_v4i32", Dest, MaxSrcs);
      Call->addArg(Src0);
      lowerCall(Call);
    } else if (Src0->getType() == IceType_i64 ||
               Src0->getType() == IceType_i32) {
      // Use a helper for x86-32 and x86-64.  Also use a helper for
      // i32 on x86-32.
      const SizeT MaxSrcs = 1;
      Type DestType = Dest->getType();
      IceString SrcSubstring = (Src0->getType() == IceType_i64 ? "64" : "32");
      IceString DstSubstring = (DestType == IceType_f32 ? "f" : "d");
      // Possibilities are cvtui32tof, cvtui32tod, cvtui64tof, cvtui64tod
      IceString TargetString = "cvtui" + SrcSubstring + "to" + DstSubstring;
      // TODO: Call the correct compiler-rt helper function.
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
      _cvt(T_2, T_1);
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
      InstCall *Call = makeHelperCall("Sz_bitcast_v8i1_to_i8", Dest, 1);
      Call->addArg(Src0);
      lowerCall(Call);
    } break;
    case IceType_i16: {
      assert(Src0->getType() == IceType_v16i1);
      InstCall *Call = makeHelperCall("Sz_bitcast_v16i1_to_i16", Dest, 1);
      Call->addArg(Src0);
      lowerCall(Call);
    } break;
    case IceType_i32:
    case IceType_f32: {
      Operand *Src0RM = legalize(Src0, Legal_Reg | Legal_Mem);
      Type DestType = Dest->getType();
      Type SrcType = Src0RM->getType();
      assert((DestType == IceType_i32 && SrcType == IceType_f32) ||
             (DestType == IceType_f32 && SrcType == IceType_i32));
      // a.i32 = bitcast b.f32 ==>
      //   t.f32 = b.f32
      //   s.f32 = spill t.f32
      //   a.i32 = s.f32
      Variable *T = NULL;
      // TODO: Should be able to force a spill setup by calling legalize() with
      // Legal_Mem and not Legal_Reg or Legal_Imm.
      Variable *Spill = Func->makeVariable(SrcType, Context.getNode());
      Spill->setWeight(RegWeight::Zero);
      Spill->setPreferredRegister(Dest, true);
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
      Variable *Spill = Func->makeVariable(IceType_f64, Context.getNode());
      Spill->setWeight(RegWeight::Zero);
      Spill->setPreferredRegister(llvm::dyn_cast<Variable>(Src0RM), true);
      _movq(Spill, Src0RM);

      Variable *DestLo = llvm::cast<Variable>(loOperand(Dest));
      Variable *DestHi = llvm::cast<Variable>(hiOperand(Dest));
      Variable *T_Lo = makeReg(IceType_i32);
      Variable *T_Hi = makeReg(IceType_i32);
      VariableSplit *SpillLo =
          VariableSplit::create(Func, Spill, VariableSplit::Low);
      VariableSplit *SpillHi =
          VariableSplit::create(Func, Spill, VariableSplit::High);

      _mov(T_Lo, SpillLo);
      _mov(DestLo, T_Lo);
      _mov(T_Hi, SpillHi);
      _mov(DestHi, T_Hi);
    } break;
    case IceType_f64: {
      Src0 = legalize(Src0);
      assert(Src0->getType() == IceType_i64);
      // a.f64 = bitcast b.i64 ==>
      //   t_lo.i32 = b_lo.i32
      //   FakeDef(s.f64)
      //   lo(s.f64) = t_lo.i32
      //   t_hi.i32 = b_hi.i32
      //   hi(s.f64) = t_hi.i32
      //   a.f64 = s.f64
      Variable *Spill = Func->makeVariable(IceType_f64, Context.getNode());
      Spill->setWeight(RegWeight::Zero);
      Spill->setPreferredRegister(Dest, true);

      Variable *T_Lo = NULL, *T_Hi = NULL;
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
      InstCall *Call = makeHelperCall("Sz_bitcast_i8_to_v8i1", Dest, 1);
      Variable *Src0AsI32 = Func->makeVariable(stackSlotType(),
                                               Context.getNode());
      // Arguments to functions are required to be at least 32 bits wide.
      lowerCast(InstCast::create(Func, InstCast::Zext, Src0AsI32, Src0));
      Call->addArg(Src0AsI32);
      lowerCall(Call);
    } break;
    case IceType_v16i1: {
      assert(Src0->getType() == IceType_i16);
      InstCall *Call = makeHelperCall("Sz_bitcast_i16_to_v16i1", Dest, 1);
      Variable *Src0AsI32 = Func->makeVariable(stackSlotType(),
                                               Context.getNode());
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

void TargetX8632::lowerExtractElement(const InstExtractElement *Inst) {
  Operand *SourceVectOperand = Inst->getSrc(0);
  ConstantInteger *ElementIndex =
      llvm::dyn_cast<ConstantInteger>(Inst->getSrc(1));
  // Only constant indices are allowed in PNaCl IR.
  assert(ElementIndex);

  unsigned Index = ElementIndex->getValue();
  Type Ty = SourceVectOperand->getType();
  Type ElementTy = typeElementType(Ty);
  Type InVectorElementTy = getInVectorElementType(Ty);
  Variable *ExtractedElement = makeReg(InVectorElementTy);

  // TODO(wala): Determine the best lowering sequences for each type.
  if (Ty == IceType_v4i32 || Ty == IceType_v4f32 || Ty == IceType_v4i1) {
    // Lower extractelement operations where the element is 32 bits
    // wide with pshufd.
    // TODO(wala): SSE4.1 has extractps and pextrd
    //
    // ALIGNHACK: Force vector operands to registers in instructions that
    // require aligned memory operands until support for stack alignment
    // is implemented.
#define ALIGN_HACK(Vect) legalizeToVar((Vect))
    Variable *T = NULL;
    if (Index) {
      // The shuffle only needs to occur if the element to be extracted
      // is not at the lowest index.
      Constant *Mask = Ctx->getConstantInt(IceType_i8, Index);
      T = makeReg(Ty);
      _pshufd(T, ALIGN_HACK(SourceVectOperand), Mask);
    } else {
      T = legalizeToVar(SourceVectOperand);
    }

    if (InVectorElementTy == IceType_i32) {
      _movd(ExtractedElement, T);
    } else { // Ty == Icetype_f32
      // TODO(wala): _movss is only used here because _mov does not
      // allow a vector source and a scalar destination.  _mov should be
      // able to be used here.
      // _movss is a binary instruction, so the FakeDef is needed to
      // keep the live range analysis consistent.
      Context.insert(InstFakeDef::create(Func, ExtractedElement));
      _movss(ExtractedElement, T);
    }
#undef ALIGN_HACK
  } else if (Ty == IceType_v8i16 || Ty == IceType_v8i1) {
    Constant *Mask = Ctx->getConstantInt(IceType_i8, Index);
    _pextrw(ExtractedElement, legalizeToVar(SourceVectOperand), Mask);
  } else {
    assert(Ty == IceType_v16i8 || Ty == IceType_v16i1);
    // Spill the value to a stack slot and do the extraction in memory.
    // TODO(wala): SSE4.1 has pextrb.
    //
    // TODO(wala): use legalize(SourceVectOperand, Legal_Mem) when
    // support for legalizing to mem is implemented.
    Variable *Slot = Func->makeVariable(Ty, Context.getNode());
    Slot->setWeight(RegWeight::Zero);
    _movp(Slot, legalizeToVar(SourceVectOperand));

    // Compute the location of the element in memory.
    unsigned Offset = Index * typeWidthInBytes(InVectorElementTy);
    OperandX8632Mem *Loc =
        getMemoryOperandForStackSlot(InVectorElementTy, Slot, Offset);
    _mov(ExtractedElement, Loc);
  }

  if (ElementTy == IceType_i1) {
    // Truncate extracted integers to i1s if necessary.
    Variable *T = makeReg(IceType_i1);
    InstCast *Cast =
        InstCast::create(Func, InstCast::Trunc, T, ExtractedElement);
    lowerCast(Cast);
    ExtractedElement = T;
  }

  // Copy the element to the destination.
  Variable *Dest = Inst->getDest();
  _mov(Dest, ExtractedElement);
}

void TargetX8632::lowerFcmp(const InstFcmp *Inst) {
  Operand *Src0 = Inst->getSrc(0);
  Operand *Src1 = Inst->getSrc(1);
  Variable *Dest = Inst->getDest();

  if (isVectorType(Dest->getType())) {
    InstFcmp::FCond Condition = Inst->getCondition();
    size_t Index = static_cast<size_t>(Condition);
    assert(Index < TableFcmpSize);

    if (TableFcmp[Index].SwapVectorOperands) {
      Operand *T = Src0;
      Src0 = Src1;
      Src1 = T;
    }

    Variable *T = NULL;

    // ALIGNHACK: Without support for stack alignment, both operands to
    // cmpps need to be forced into registers.  Once support for stack
    // alignment is implemented, remove LEGAL_HACK.
#define LEGAL_HACK(Vect) legalizeToVar((Vect))
    switch (Condition) {
    default: {
      InstX8632Cmpps::CmppsCond Predicate = TableFcmp[Index].Predicate;
      assert(Predicate != InstX8632Cmpps::Cmpps_Invalid);
      T = makeReg(Src0->getType());
      _movp(T, Src0);
      _cmpps(T, LEGAL_HACK(Src1), Predicate);
    } break;
    case InstFcmp::False:
      T = makeVectorOfZeros(Src0->getType());
      break;
    case InstFcmp::One: {
      // Check both unequal and ordered.
      T = makeReg(Src0->getType());
      Variable *T2 = makeReg(Src0->getType());
      Src1 = LEGAL_HACK(Src1);
      _movp(T, Src0);
      _cmpps(T, Src1, InstX8632Cmpps::Cmpps_neq);
      _movp(T2, Src0);
      _cmpps(T2, Src1, InstX8632Cmpps::Cmpps_ord);
      _pand(T, T2);
    } break;
    case InstFcmp::Ueq: {
      // Check both equal or unordered.
      T = makeReg(Src0->getType());
      Variable *T2 = makeReg(Src0->getType());
      Src1 = LEGAL_HACK(Src1);
      _movp(T, Src0);
      _cmpps(T, Src1, InstX8632Cmpps::Cmpps_eq);
      _movp(T2, Src0);
      _cmpps(T2, Src1, InstX8632Cmpps::Cmpps_unord);
      _por(T, T2);
    } break;
    case InstFcmp::True:
      T = makeVectorOfMinusOnes(IceType_v4i32);
      break;
    }
#undef LEGAL_HACK

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
  InstFcmp::FCond Condition = Inst->getCondition();
  size_t Index = static_cast<size_t>(Condition);
  assert(Index < TableFcmpSize);
  if (TableFcmp[Index].SwapScalarOperands) {
    Operand *Tmp = Src0;
    Src0 = Src1;
    Src1 = Tmp;
  }
  bool HasC1 = (TableFcmp[Index].C1 != InstX8632Br::Br_None);
  bool HasC2 = (TableFcmp[Index].C2 != InstX8632Br::Br_None);
  if (HasC1) {
    Src0 = legalize(Src0);
    Operand *Src1RM = legalize(Src1, Legal_Reg | Legal_Mem);
    Variable *T = NULL;
    _mov(T, Src0);
    _ucomiss(T, Src1RM);
  }
  Constant *Default =
      Ctx->getConstantInt(IceType_i32, TableFcmp[Index].Default);
  _mov(Dest, Default);
  if (HasC1) {
    InstX8632Label *Label = InstX8632Label::create(Func, this);
    _br(TableFcmp[Index].C1, Label);
    if (HasC2) {
      _br(TableFcmp[Index].C2, Label);
    }
    Context.insert(InstFakeUse::create(Func, Dest));
    Constant *NonDefault =
        Ctx->getConstantInt(IceType_i32, !TableFcmp[Index].Default);
    _mov(Dest, NonDefault);
    Context.insert(Label);
  }
}

void TargetX8632::lowerIcmp(const InstIcmp *Inst) {
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
      Variable *NewSrc0 = Func->makeVariable(NewTy, Context.getNode());
      Variable *NewSrc1 = Func->makeVariable(NewTy, Context.getNode());
      lowerCast(InstCast::create(Func, InstCast::Sext, NewSrc0, Src0));
      lowerCast(InstCast::create(Func, InstCast::Sext, NewSrc1, Src1));
      Src0 = NewSrc0;
      Src1 = NewSrc1;
      Ty = NewTy;
    }

    InstIcmp::ICond Condition = Inst->getCondition();

    // SSE2 only has signed comparison operations.  Transform unsigned
    // inputs in a manner that allows for the use of signed comparison
    // operations by flipping the high order bits.
    if (Condition == InstIcmp::Ugt || Condition == InstIcmp::Uge ||
        Condition == InstIcmp::Ult || Condition == InstIcmp::Ule) {
      Variable *T0 = makeReg(Ty);
      Variable *T1 = makeReg(Ty);
      Variable *HighOrderBits = makeVectorOfHighOrderBits(Ty);
      _movp(T0, Src0);
      _pxor(T0, HighOrderBits);
      _movp(T1, Src1);
      _pxor(T1, HighOrderBits);
      Src0 = T0;
      Src1 = T1;
    }

    // TODO: ALIGNHACK: Both operands to compare instructions need to be
    // in registers until stack alignment support is implemented.  Once
    // there is support for stack alignment, LEGAL_HACK can be removed.
#define LEGAL_HACK(Vect) legalizeToVar((Vect))
    Variable *T = makeReg(Ty);
    switch (Condition) {
    default:
      llvm_unreachable("unexpected condition");
      break;
    case InstIcmp::Eq: {
      _movp(T, Src0);
      _pcmpeq(T, LEGAL_HACK(Src1));
    } break;
    case InstIcmp::Ne: {
      _movp(T, Src0);
      _pcmpeq(T, LEGAL_HACK(Src1));
      Variable *MinusOne = makeVectorOfMinusOnes(Ty);
      _pxor(T, MinusOne);
    } break;
    case InstIcmp::Ugt:
    case InstIcmp::Sgt: {
      _movp(T, Src0);
      _pcmpgt(T, LEGAL_HACK(Src1));
    } break;
    case InstIcmp::Uge:
    case InstIcmp::Sge: {
      // !(Src1 > Src0)
      _movp(T, Src1);
      _pcmpgt(T, LEGAL_HACK(Src0));
      Variable *MinusOne = makeVectorOfMinusOnes(Ty);
      _pxor(T, MinusOne);
    } break;
    case InstIcmp::Ult:
    case InstIcmp::Slt: {
      _movp(T, Src1);
      _pcmpgt(T, LEGAL_HACK(Src0));
    } break;
    case InstIcmp::Ule:
    case InstIcmp::Sle: {
      // !(Src0 > Src1)
      _movp(T, Src0);
      _pcmpgt(T, LEGAL_HACK(Src1));
      Variable *MinusOne = makeVectorOfMinusOnes(Ty);
      _pxor(T, MinusOne);
    } break;
    }
#undef LEGAL_HACK

    _movp(Dest, T);
    eliminateNextVectorSextInstruction(Dest);
    return;
  }

  // If Src1 is an immediate, or known to be a physical register, we can
  // allow Src0 to be a memory operand.  Otherwise, Src0 must be copied into
  // a physical register.  (Actually, either Src0 or Src1 can be chosen for
  // the physical register, but unfortunately we have to commit to one or
  // the other before register allocation.)
  bool IsSrc1ImmOrReg = false;
  if (llvm::isa<Constant>(Src1)) {
    IsSrc1ImmOrReg = true;
  } else if (Variable *Var = llvm::dyn_cast<Variable>(Src1)) {
    if (Var->hasReg())
      IsSrc1ImmOrReg = true;
  }

  // Try to fuse a compare immediately followed by a conditional branch.  This
  // is possible when the compare dest and the branch source operands are the
  // same, and are their only uses.  TODO: implement this optimization for i64.
  if (InstBr *NextBr = llvm::dyn_cast_or_null<InstBr>(Context.getNextInst())) {
    if (Src0->getType() != IceType_i64 && !NextBr->isUnconditional() &&
        Dest == NextBr->getSrc(0) && NextBr->isLastUse(Dest)) {
      Operand *Src0New =
          legalize(Src0, IsSrc1ImmOrReg ? Legal_All : Legal_Reg, true);
      _cmp(Src0New, Src1);
      _br(getIcmp32Mapping(Inst->getCondition()), NextBr->getTargetTrue(),
          NextBr->getTargetFalse());
      // Skip over the following branch instruction.
      NextBr->setDeleted();
      Context.advanceNext();
      return;
    }
  }

  // a=icmp cond, b, c ==> cmp b,c; a=1; br cond,L1; FakeUse(a); a=0; L1:
  Constant *Zero = Ctx->getConstantZero(IceType_i32);
  Constant *One = Ctx->getConstantInt(IceType_i32, 1);
  if (Src0->getType() == IceType_i64) {
    InstIcmp::ICond Condition = Inst->getCondition();
    size_t Index = static_cast<size_t>(Condition);
    assert(Index < TableIcmp64Size);
    Operand *Src1LoRI = legalize(loOperand(Src1), Legal_Reg | Legal_Imm);
    Operand *Src1HiRI = legalize(hiOperand(Src1), Legal_Reg | Legal_Imm);
    if (Condition == InstIcmp::Eq || Condition == InstIcmp::Ne) {
      InstX8632Label *Label = InstX8632Label::create(Func, this);
      _mov(Dest, (Condition == InstIcmp::Eq ? Zero : One));
      _cmp(loOperand(Src0), Src1LoRI);
      _br(InstX8632Br::Br_ne, Label);
      _cmp(hiOperand(Src0), Src1HiRI);
      _br(InstX8632Br::Br_ne, Label);
      Context.insert(InstFakeUse::create(Func, Dest));
      _mov(Dest, (Condition == InstIcmp::Eq ? One : Zero));
      Context.insert(Label);
    } else {
      InstX8632Label *LabelFalse = InstX8632Label::create(Func, this);
      InstX8632Label *LabelTrue = InstX8632Label::create(Func, this);
      _mov(Dest, One);
      _cmp(hiOperand(Src0), Src1HiRI);
      _br(TableIcmp64[Index].C1, LabelTrue);
      _br(TableIcmp64[Index].C2, LabelFalse);
      _cmp(loOperand(Src0), Src1LoRI);
      _br(TableIcmp64[Index].C3, LabelTrue);
      Context.insert(LabelFalse);
      Context.insert(InstFakeUse::create(Func, Dest));
      _mov(Dest, Zero);
      Context.insert(LabelTrue);
    }
    return;
  }

  // cmp b, c
  Operand *Src0New =
      legalize(Src0, IsSrc1ImmOrReg ? Legal_All : Legal_Reg, true);
  InstX8632Label *Label = InstX8632Label::create(Func, this);
  _cmp(Src0New, Src1);
  _mov(Dest, One);
  _br(getIcmp32Mapping(Inst->getCondition()), Label);
  Context.insert(InstFakeUse::create(Func, Dest));
  _mov(Dest, Zero);
  Context.insert(Label);
}

void TargetX8632::lowerInsertElement(const InstInsertElement *Inst) {
  Operand *SourceVectOperand = Inst->getSrc(0);
  Operand *ElementToInsert = Inst->getSrc(1);
  ConstantInteger *ElementIndex =
      llvm::dyn_cast<ConstantInteger>(Inst->getSrc(2));
  // Only constant indices are allowed in PNaCl IR.
  assert(ElementIndex);
  unsigned Index = ElementIndex->getValue();
  assert(Index < typeNumElements(SourceVectOperand->getType()));

  Type Ty = SourceVectOperand->getType();
  Type ElementTy = typeElementType(Ty);
  Type InVectorElementTy = getInVectorElementType(Ty);

  if (ElementTy == IceType_i1) {
    // Expand the element to the appropriate size for it to be inserted
    // in the vector.
    Variable *Expanded =
        Func->makeVariable(InVectorElementTy, Context.getNode());
    InstCast *Cast =
        InstCast::create(Func, InstCast::Zext, Expanded, ElementToInsert);
    lowerCast(Cast);
    ElementToInsert = Expanded;
  }

  if (Ty == IceType_v4i32 || Ty == IceType_v4f32 || Ty == IceType_v4i1) {
    // Lower insertelement with 32-bit wide elements using shufps or
    // movss.
    // TODO(wala): SSE4.1 has pinsrd and insertps.
    Variable *Element = NULL;
    if (InVectorElementTy == IceType_f32) {
      // Element will be in an XMM register since it is floating point.
      Element = legalizeToVar(ElementToInsert);
    } else {
      // Copy an integer to an XMM register.
      Operand *T = legalize(ElementToInsert, Legal_Reg | Legal_Mem);
      Element = makeReg(Ty);
      _movd(Element, T);
    }

    if (Index == 0) {
      Variable *T = makeReg(Ty);
      _movp(T, SourceVectOperand);
      _movss(T, Element);
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
    // insertelement into index 1 (result is stored in Element):
    //   Element := Element[0, 0] SourceVectOperand[0, 0]
    //   Element := Element[3, 0] SourceVectOperand[2, 3]
    //
    // insertelement into index 2 (result is stored in T):
    //   T := SourceVectOperand
    //   Element := Element[0, 0] T[0, 3]
    //   T := T[0, 1] Element[0, 3]
    //
    // insertelement into index 3 (result is stored in T):
    //   T := SourceVectOperand
    //   Element := Element[0, 0] T[0, 2]
    //   T := T[0, 1] Element[3, 0]
    const unsigned char Mask1[3] = {0, 192, 128};
    const unsigned char Mask2[3] = {227, 196, 52};

    Constant *Mask1Constant = Ctx->getConstantInt(IceType_i8, Mask1[Index - 1]);
    Constant *Mask2Constant = Ctx->getConstantInt(IceType_i8, Mask2[Index - 1]);

    // ALIGNHACK: Force vector operands to registers in instructions that
    // require aligned memory operands until support for stack alignment
    // is implemented.
#define ALIGN_HACK(Vect) legalizeToVar((Vect))
    if (Index == 1) {
      SourceVectOperand = ALIGN_HACK(SourceVectOperand);
      _shufps(Element, SourceVectOperand, Mask1Constant);
      _shufps(Element, SourceVectOperand, Mask2Constant);
      _movp(Inst->getDest(), Element);
    } else {
      Variable *T = makeReg(Ty);
      _movp(T, SourceVectOperand);
      _shufps(Element, T, Mask1Constant);
      _shufps(T, Element, Mask2Constant);
      _movp(Inst->getDest(), T);
    }
#undef ALIGN_HACK
  } else if (Ty == IceType_v8i16 || Ty == IceType_v8i1) {
    Operand *Element = legalize(ElementToInsert, Legal_Mem | Legal_Reg);
    Variable *T = makeReg(Ty);
    _movp(T, SourceVectOperand);
    _pinsrw(T, Element, Ctx->getConstantInt(IceType_i8, Index));
    _movp(Inst->getDest(), T);
  } else {
    assert(Ty == IceType_v16i8 || Ty == IceType_v16i1);
    // Spill the value to a stack slot and perform the insertion in
    // memory.
    // TODO(wala): SSE4.1 has pinsrb.
    //
    // TODO(wala): use legalize(SourceVectOperand, Legal_Mem) when
    // support for legalizing to mem is implemented.
    Variable *Slot = Func->makeVariable(Ty, Context.getNode());
    Slot->setWeight(RegWeight::Zero);
    _movp(Slot, legalizeToVar(SourceVectOperand));

    // Compute the location of the position to insert in memory.
    unsigned Offset = Index * typeWidthInBytes(InVectorElementTy);
    OperandX8632Mem *Loc =
        getMemoryOperandForStackSlot(InVectorElementTy, Slot, Offset);
    _store(legalizeToVar(ElementToInsert), Loc);

    Variable *T = makeReg(Ty);
    _movp(T, Slot);
    _movp(Inst->getDest(), T);
  }
}

void TargetX8632::lowerIntrinsicCall(const InstIntrinsicCall *Instr) {
  switch (Instr->getIntrinsicInfo().ID) {
  case Intrinsics::AtomicCmpxchg: {
    if (!Intrinsics::VerifyMemoryOrder(
             llvm::cast<ConstantInteger>(Instr->getArg(3))->getValue())) {
      Func->setError("Unexpected memory ordering (success) for AtomicCmpxchg");
      return;
    }
    if (!Intrinsics::VerifyMemoryOrder(
             llvm::cast<ConstantInteger>(Instr->getArg(4))->getValue())) {
      Func->setError("Unexpected memory ordering (failure) for AtomicCmpxchg");
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
    if (!Intrinsics::VerifyMemoryOrder(
             llvm::cast<ConstantInteger>(Instr->getArg(0))->getValue())) {
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
    if (ConstantInteger *CI = llvm::dyn_cast<ConstantInteger>(ByteSize)) {
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
        Result = Ctx->getConstantInt(IceType_i32, 1);
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
    if (!Intrinsics::VerifyMemoryOrder(
             llvm::cast<ConstantInteger>(Instr->getArg(1))->getValue())) {
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
      OperandX8632Mem *Addr = FormMemoryOperand(Instr->getArg(0), IceType_f64);
      _movq(T, Addr);
      // Then cast the bits back out of the XMM register to the i64 Dest.
      InstCast *Cast = InstCast::create(Func, InstCast::Bitcast, Dest, T);
      lowerCast(Cast);
      // Make sure that the atomic load isn't elided.
      Context.insert(InstFakeUse::create(Func, Dest->getLo()));
      Context.insert(InstFakeUse::create(Func, Dest->getHi()));
      return;
    }
    InstLoad *Load = InstLoad::create(Func, Dest, Instr->getArg(0));
    lowerLoad(Load);
    // Make sure the atomic load isn't elided.
    Context.insert(InstFakeUse::create(Func, Dest));
    return;
  }
  case Intrinsics::AtomicRMW:
    if (!Intrinsics::VerifyMemoryOrder(
             llvm::cast<ConstantInteger>(Instr->getArg(3))->getValue())) {
      Func->setError("Unexpected memory ordering for AtomicRMW");
      return;
    }
    lowerAtomicRMW(Instr->getDest(),
                   static_cast<uint32_t>(llvm::cast<ConstantInteger>(
                       Instr->getArg(0))->getValue()),
                   Instr->getArg(1), Instr->getArg(2));
    return;
  case Intrinsics::AtomicStore: {
    if (!Intrinsics::VerifyMemoryOrder(
             llvm::cast<ConstantInteger>(Instr->getArg(2))->getValue())) {
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
      OperandX8632Mem *Addr = FormMemoryOperand(Ptr, IceType_f64);
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
      Constant *Eight = Ctx->getConstantInt(IceType_i16, 8);
      Variable *T = NULL;
      _mov(T, Val);
      _rol(T, Eight);
      _mov(Dest, T);
    }
    return;
  }
  case Intrinsics::Ctpop: {
    Variable *Dest = Instr->getDest();
    Operand *Val = Instr->getArg(0);
    InstCall *Call = makeHelperCall(Val->getType() == IceType_i64 ?
        "__popcountdi2" : "__popcountsi2", Dest, 1);
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
    Operand *SecondVal = NULL;
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
    Operand *SecondVal = NULL;
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
  case Intrinsics::Longjmp: {
    InstCall *Call = makeHelperCall("longjmp", NULL, 2);
    Call->addArg(Instr->getArg(0));
    Call->addArg(Instr->getArg(1));
    lowerCall(Call);
    return;
  }
  case Intrinsics::Memcpy: {
    // In the future, we could potentially emit an inline memcpy/memset, etc.
    // for intrinsic calls w/ a known length.
    InstCall *Call = makeHelperCall("memcpy", NULL, 3);
    Call->addArg(Instr->getArg(0));
    Call->addArg(Instr->getArg(1));
    Call->addArg(Instr->getArg(2));
    lowerCall(Call);
    return;
  }
  case Intrinsics::Memmove: {
    InstCall *Call = makeHelperCall("memmove", NULL, 3);
    Call->addArg(Instr->getArg(0));
    Call->addArg(Instr->getArg(1));
    Call->addArg(Instr->getArg(2));
    lowerCall(Call);
    return;
  }
  case Intrinsics::Memset: {
    // The value operand needs to be extended to a stack slot size
    // because "push" only works for a specific operand size.
    Operand *ValOp = Instr->getArg(1);
    assert(ValOp->getType() == IceType_i8);
    Variable *ValExt = Func->makeVariable(stackSlotType(), Context.getNode());
    lowerCast(InstCast::create(Func, InstCast::Zext, ValExt, ValOp));
    InstCall *Call = makeHelperCall("memset", NULL, 3);
    Call->addArg(Instr->getArg(0));
    Call->addArg(ValExt);
    Call->addArg(Instr->getArg(2));
    lowerCall(Call);
    return;
  }
  case Intrinsics::NaClReadTP: {
    Constant *Zero = Ctx->getConstantZero(IceType_i32);
    Operand *Src = OperandX8632Mem::create(Func, IceType_i32, NULL, Zero, NULL,
                                           0, OperandX8632Mem::SegReg_GS);
    Variable *Dest = Instr->getDest();
    Variable *T = NULL;
    _mov(T, Src);
    _mov(Dest, T);
    return;
  }
  case Intrinsics::Setjmp: {
    InstCall *Call = makeHelperCall("setjmp", Instr->getDest(), 1);
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
    Variable *esp = Func->getTarget()->getPhysicalRegister(Reg_esp);
    Variable *Dest = Instr->getDest();
    _mov(Dest, esp);
    return;
  }
  case Intrinsics::Stackrestore: {
    Variable *esp = Func->getTarget()->getPhysicalRegister(Reg_esp);
    _mov(esp, Instr->getArg(0));
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

void TargetX8632::lowerAtomicCmpxchg(Variable *DestPrev, Operand *Ptr,
                                     Operand *Expected, Operand *Desired) {
  if (Expected->getType() == IceType_i64) {
    // Reserve the pre-colored registers first, before adding any more
    // infinite-weight variables from FormMemoryOperand's legalization.
    Variable *T_edx = makeReg(IceType_i32, Reg_edx);
    Variable *T_eax = makeReg(IceType_i32, Reg_eax);
    Variable *T_ecx = makeReg(IceType_i32, Reg_ecx);
    Variable *T_ebx = makeReg(IceType_i32, Reg_ebx);
    _mov(T_eax, loOperand(Expected));
    _mov(T_edx, hiOperand(Expected));
    _mov(T_ebx, loOperand(Desired));
    _mov(T_ecx, hiOperand(Desired));
    OperandX8632Mem *Addr = FormMemoryOperand(Ptr, Expected->getType());
    const bool Locked = true;
    _cmpxchg8b(Addr, T_edx, T_eax, T_ecx, T_ebx, Locked);
    Variable *DestLo = llvm::cast<Variable>(loOperand(DestPrev));
    Variable *DestHi = llvm::cast<Variable>(hiOperand(DestPrev));
    _mov(DestLo, T_eax);
    _mov(DestHi, T_edx);
    return;
  }
  Variable *T_eax = makeReg(Expected->getType(), Reg_eax);
  _mov(T_eax, Expected);
  OperandX8632Mem *Addr = FormMemoryOperand(Ptr, Expected->getType());
  Variable *DesiredReg = legalizeToVar(Desired);
  const bool Locked = true;
  _cmpxchg(Addr, T_eax, DesiredReg, Locked);
  _mov(DestPrev, T_eax);
}

bool TargetX8632::tryOptimizedCmpxchgCmpBr(Variable *Dest, Operand *PtrToMem,
                                           Operand *Expected,
                                           Operand *Desired) {
  if (Ctx->getOptLevel() == Opt_m1)
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
          lowerAssign(PhiAssign);
          PhiAssign->setDeleted();
          Context.advanceNext();
        }
        _br(InstX8632::Br_e, NextBr->getTargetTrue(), NextBr->getTargetFalse());
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

void TargetX8632::lowerAtomicRMW(Variable *Dest, uint32_t Operation,
                                 Operand *Ptr, Operand *Val) {
  bool NeedsCmpxchg = false;
  LowerBinOp Op_Lo = NULL;
  LowerBinOp Op_Hi = NULL;
  switch (Operation) {
  default:
    Func->setError("Unknown AtomicRMW operation");
    return;
  case Intrinsics::AtomicAdd: {
    if (Dest->getType() == IceType_i64) {
      // All the fall-through paths must set this to true, but use this
      // for asserting.
      NeedsCmpxchg = true;
      Op_Lo = &TargetX8632::_add;
      Op_Hi = &TargetX8632::_adc;
      break;
    }
    OperandX8632Mem *Addr = FormMemoryOperand(Ptr, Dest->getType());
    const bool Locked = true;
    Variable *T = NULL;
    _mov(T, Val);
    _xadd(Addr, T, Locked);
    _mov(Dest, T);
    return;
  }
  case Intrinsics::AtomicSub: {
    if (Dest->getType() == IceType_i64) {
      NeedsCmpxchg = true;
      Op_Lo = &TargetX8632::_sub;
      Op_Hi = &TargetX8632::_sbb;
      break;
    }
    OperandX8632Mem *Addr = FormMemoryOperand(Ptr, Dest->getType());
    const bool Locked = true;
    Variable *T = NULL;
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
    Op_Lo = &TargetX8632::_or;
    Op_Hi = &TargetX8632::_or;
    break;
  case Intrinsics::AtomicAnd:
    NeedsCmpxchg = true;
    Op_Lo = &TargetX8632::_and;
    Op_Hi = &TargetX8632::_and;
    break;
  case Intrinsics::AtomicXor:
    NeedsCmpxchg = true;
    Op_Lo = &TargetX8632::_xor;
    Op_Hi = &TargetX8632::_xor;
    break;
  case Intrinsics::AtomicExchange:
    if (Dest->getType() == IceType_i64) {
      NeedsCmpxchg = true;
      // NeedsCmpxchg, but no real Op_Lo/Op_Hi need to be done. The values
      // just need to be moved to the ecx and ebx registers.
      Op_Lo = NULL;
      Op_Hi = NULL;
      break;
    }
    OperandX8632Mem *Addr = FormMemoryOperand(Ptr, Dest->getType());
    Variable *T = NULL;
    _mov(T, Val);
    _xchg(Addr, T);
    _mov(Dest, T);
    return;
  }
  // Otherwise, we need a cmpxchg loop.
  assert(NeedsCmpxchg);
  expandAtomicRMWAsCmpxchg(Op_Lo, Op_Hi, Dest, Ptr, Val);
}

void TargetX8632::expandAtomicRMWAsCmpxchg(LowerBinOp Op_Lo, LowerBinOp Op_Hi,
                                           Variable *Dest, Operand *Ptr,
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
  // If Op_{Lo,Hi} are NULL, then just copy the value.
  Val = legalize(Val);
  Type Ty = Val->getType();
  if (Ty == IceType_i64) {
    Variable *T_edx = makeReg(IceType_i32, Reg_edx);
    Variable *T_eax = makeReg(IceType_i32, Reg_eax);
    OperandX8632Mem *Addr = FormMemoryOperand(Ptr, Ty);
    _mov(T_eax, loOperand(Addr));
    _mov(T_edx, hiOperand(Addr));
    Variable *T_ecx = makeReg(IceType_i32, Reg_ecx);
    Variable *T_ebx = makeReg(IceType_i32, Reg_ebx);
    InstX8632Label *Label = InstX8632Label::create(Func, this);
    const bool IsXchg8b = Op_Lo == NULL && Op_Hi == NULL;
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
    _br(InstX8632Br::Br_ne, Label);
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
    // The address base is also reused in the loop.
    Context.insert(InstFakeUse::create(Func, Addr->getBase()));
    Variable *DestLo = llvm::cast<Variable>(loOperand(Dest));
    Variable *DestHi = llvm::cast<Variable>(hiOperand(Dest));
    _mov(DestLo, T_eax);
    _mov(DestHi, T_edx);
    return;
  }
  OperandX8632Mem *Addr = FormMemoryOperand(Ptr, Ty);
  Variable *T_eax = makeReg(Ty, Reg_eax);
  _mov(T_eax, Addr);
  InstX8632Label *Label = InstX8632Label::create(Func, this);
  Context.insert(Label);
  // We want to pick a different register for T than Eax, so don't use
  // _mov(T == NULL, T_eax).
  Variable *T = makeReg(Ty);
  _mov(T, T_eax);
  (this->*Op_Lo)(T, Val);
  const bool Locked = true;
  _cmpxchg(Addr, T_eax, T, Locked);
  _br(InstX8632Br::Br_ne, Label);
  // If Val is a variable, model the extended live range of Val through
  // the end of the loop, since it will be re-used by the loop.
  if (Variable *ValVar = llvm::dyn_cast<Variable>(Val)) {
    Context.insert(InstFakeUse::create(Func, ValVar));
  }
  // The address base is also reused in the loop.
  Context.insert(InstFakeUse::create(Func, Addr->getBase()));
  _mov(Dest, T_eax);
}

// Lowers count {trailing, leading} zeros intrinsic.
//
// We could do constant folding here, but that should have
// been done by the front-end/middle-end optimizations.
void TargetX8632::lowerCountZeros(bool Cttz, Type Ty, Variable *Dest,
                                  Operand *FirstVal, Operand *SecondVal) {
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
  Constant *ThirtyTwo = Ctx->getConstantInt(IceType_i32, 32);
  Constant *ThirtyOne = Ctx->getConstantInt(IceType_i32, 31);
  if (Cttz) {
    _mov(T_Dest, ThirtyTwo);
  } else {
    Constant *SixtyThree = Ctx->getConstantInt(IceType_i32, 63);
    _mov(T_Dest, SixtyThree);
  }
  _cmov(T_Dest, T, InstX8632::Br_ne);
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
  _cmov(T_Dest2, T_Dest, InstX8632::Br_e);
  _mov(DestLo, T_Dest2);
  _mov(DestHi, Ctx->getConstantZero(IceType_i32));
}

namespace {

bool isAdd(const Inst *Inst) {
  if (const InstArithmetic *Arith =
          llvm::dyn_cast_or_null<const InstArithmetic>(Inst)) {
    return (Arith->getOp() == InstArithmetic::Add);
  }
  return false;
}

void computeAddressOpt(Variable *&Base, Variable *&Index, uint16_t &Shift,
                       int32_t &Offset) {
  (void)Offset; // TODO: pattern-match for non-zero offsets.
  if (Base == NULL)
    return;
  // If the Base has more than one use or is live across multiple
  // blocks, then don't go further.  Alternatively (?), never consider
  // a transformation that would change a variable that is currently
  // *not* live across basic block boundaries into one that *is*.
  if (Base->isMultiblockLife() /* || Base->getUseCount() > 1*/)
    return;

  while (true) {
    // Base is Base=Var ==>
    //   set Base=Var
    const Inst *BaseInst = Base->getDefinition();
    Operand *BaseOperand0 = BaseInst ? BaseInst->getSrc(0) : NULL;
    Variable *BaseVariable0 = llvm::dyn_cast_or_null<Variable>(BaseOperand0);
    // TODO: Helper function for all instances of assignment
    // transitivity.
    if (BaseInst && llvm::isa<InstAssign>(BaseInst) && BaseVariable0 &&
        // TODO: ensure BaseVariable0 stays single-BB
        true) {
      Base = BaseVariable0;
      continue;
    }

    // Index is Index=Var ==>
    //   set Index=Var

    // Index==NULL && Base is Base=Var1+Var2 ==>
    //   set Base=Var1, Index=Var2, Shift=0
    Operand *BaseOperand1 =
        BaseInst && BaseInst->getSrcSize() >= 2 ? BaseInst->getSrc(1) : NULL;
    Variable *BaseVariable1 = llvm::dyn_cast_or_null<Variable>(BaseOperand1);
    if (Index == NULL && isAdd(BaseInst) && BaseVariable0 && BaseVariable1 &&
        // TODO: ensure BaseVariable0 and BaseVariable1 stay single-BB
        true) {
      Base = BaseVariable0;
      Index = BaseVariable1;
      Shift = 0; // should already have been 0
      continue;
    }

    // Index is Index=Var*Const && log2(Const)+Shift<=3 ==>
    //   Index=Var, Shift+=log2(Const)
    const Inst *IndexInst = Index ? Index->getDefinition() : NULL;
    if (const InstArithmetic *ArithInst =
            llvm::dyn_cast_or_null<InstArithmetic>(IndexInst)) {
      Operand *IndexOperand0 = ArithInst->getSrc(0);
      Variable *IndexVariable0 = llvm::dyn_cast<Variable>(IndexOperand0);
      Operand *IndexOperand1 = ArithInst->getSrc(1);
      ConstantInteger *IndexConstant1 =
          llvm::dyn_cast<ConstantInteger>(IndexOperand1);
      if (ArithInst->getOp() == InstArithmetic::Mul && IndexVariable0 &&
          IndexOperand1->getType() == IceType_i32 && IndexConstant1) {
        uint64_t Mult = IndexConstant1->getValue();
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
          LogMult = 4;
          break;
        }
        if (Shift + LogMult <= 3) {
          Index = IndexVariable0;
          Shift += LogMult;
          continue;
        }
      }
    }

    // Index is Index=Var<<Const && Const+Shift<=3 ==>
    //   Index=Var, Shift+=Const

    // Index is Index=Const*Var && log2(Const)+Shift<=3 ==>
    //   Index=Var, Shift+=log2(Const)

    // Index && Shift==0 && Base is Base=Var*Const && log2(Const)+Shift<=3 ==>
    //   swap(Index,Base)
    // Similar for Base=Const*Var and Base=Var<<Const

    // Base is Base=Var+Const ==>
    //   set Base=Var, Offset+=Const

    // Base is Base=Const+Var ==>
    //   set Base=Var, Offset+=Const

    // Base is Base=Var-Const ==>
    //   set Base=Var, Offset-=Const

    // Index is Index=Var+Const ==>
    //   set Index=Var, Offset+=(Const<<Shift)

    // Index is Index=Const+Var ==>
    //   set Index=Var, Offset+=(Const<<Shift)

    // Index is Index=Var-Const ==>
    //   set Index=Var, Offset-=(Const<<Shift)

    // TODO: consider overflow issues with respect to Offset.
    // TODO: handle symbolic constants.
    break;
  }
}

} // anonymous namespace

void TargetX8632::lowerLoad(const InstLoad *Inst) {
  // A Load instruction can be treated the same as an Assign
  // instruction, after the source operand is transformed into an
  // OperandX8632Mem operand.  Note that the address mode
  // optimization already creates an OperandX8632Mem operand, so it
  // doesn't need another level of transformation.
  Type Ty = Inst->getDest()->getType();
  Operand *Src0 = FormMemoryOperand(Inst->getSourceAddress(), Ty);

  // Fuse this load with a subsequent Arithmetic instruction in the
  // following situations:
  //   a=[mem]; c=b+a ==> c=b+[mem] if last use of a and a not in b
  //   a=[mem]; c=a+b ==> c=b+[mem] if commutative and above is true
  //
  // TODO: Clean up and test thoroughly.
  // (E.g., if there is an mfence-all make sure the load ends up on the
  // same side of the fence).
  //
  // TODO: Why limit to Arithmetic instructions?  This could probably be
  // applied to most any instruction type.  Look at all source operands
  // in the following instruction, and if there is one instance of the
  // load instruction's dest variable, and that instruction ends that
  // variable's live range, then make the substitution.  Deal with
  // commutativity optimization in the arithmetic instruction lowering.
  InstArithmetic *NewArith = NULL;
  if (InstArithmetic *Arith =
          llvm::dyn_cast_or_null<InstArithmetic>(Context.getNextInst())) {
    Variable *DestLoad = Inst->getDest();
    Variable *Src0Arith = llvm::dyn_cast<Variable>(Arith->getSrc(0));
    Variable *Src1Arith = llvm::dyn_cast<Variable>(Arith->getSrc(1));
    if (Src1Arith == DestLoad && Arith->isLastUse(Src1Arith) &&
        DestLoad != Src0Arith) {
      NewArith = InstArithmetic::create(Func, Arith->getOp(), Arith->getDest(),
                                        Arith->getSrc(0), Src0);
    } else if (Src0Arith == DestLoad && Arith->isCommutative() &&
               Arith->isLastUse(Src0Arith) && DestLoad != Src1Arith) {
      NewArith = InstArithmetic::create(Func, Arith->getOp(), Arith->getDest(),
                                        Arith->getSrc(1), Src0);
    }
    if (NewArith) {
      Arith->setDeleted();
      Context.advanceNext();
      lowerArithmetic(NewArith);
      return;
    }
  }

  InstAssign *Assign = InstAssign::create(Func, Inst->getDest(), Src0);
  lowerAssign(Assign);
}

void TargetX8632::doAddressOptLoad() {
  Inst *Inst = *Context.getCur();
  Variable *Dest = Inst->getDest();
  Operand *Addr = Inst->getSrc(0);
  Variable *Index = NULL;
  uint16_t Shift = 0;
  int32_t Offset = 0; // TODO: make Constant
  // Vanilla ICE load instructions should not use the segment registers,
  // and computeAddressOpt only works at the level of Variables and Constants,
  // not other OperandX8632Mem, so there should be no mention of segment
  // registers there either.
  const OperandX8632Mem::SegmentRegisters SegmentReg =
      OperandX8632Mem::DefaultSegment;
  Variable *Base = llvm::dyn_cast<Variable>(Addr);
  computeAddressOpt(Base, Index, Shift, Offset);
  if (Base && Addr != Base) {
    Constant *OffsetOp = Ctx->getConstantInt(IceType_i32, Offset);
    Addr = OperandX8632Mem::create(Func, Dest->getType(), Base, OffsetOp, Index,
                                   Shift, SegmentReg);
    Inst->setDeleted();
    Context.insert(InstLoad::create(Func, Dest, Addr));
  }
}

void TargetX8632::lowerPhi(const InstPhi * /*Inst*/) {
  Func->setError("Phi found in regular instruction list");
}

void TargetX8632::lowerRet(const InstRet *Inst) {
  Variable *Reg = NULL;
  if (Inst->hasRetValue()) {
    Operand *Src0 = legalize(Inst->getRetValue());
    if (Src0->getType() == IceType_i64) {
      Variable *eax = legalizeToVar(loOperand(Src0), false, Reg_eax);
      Variable *edx = legalizeToVar(hiOperand(Src0), false, Reg_edx);
      Reg = eax;
      Context.insert(InstFakeUse::create(Func, edx));
    } else if (Src0->getType() == IceType_f32 ||
               Src0->getType() == IceType_f64) {
      _fld(Src0);
    } else if (isVectorType(Src0->getType())) {
      Reg = legalizeToVar(Src0, false, Reg_xmm0);
    } else {
      _mov(Reg, Src0, Reg_eax);
    }
  }
  _ret(Reg);
  // Add a fake use of esp to make sure esp stays alive for the entire
  // function.  Otherwise post-call esp adjustments get dead-code
  // eliminated.  TODO: Are there more places where the fake use
  // should be inserted?  E.g. "void f(int n){while(1) g(n);}" may not
  // have a ret instruction.
  Variable *esp = Func->getTarget()->getPhysicalRegister(Reg_esp);
  Context.insert(InstFakeUse::create(Func, esp));
}

void TargetX8632::lowerSelect(const InstSelect *Inst) {
  Variable *Dest = Inst->getDest();
  Operand *SrcT = Inst->getTrueOperand();
  Operand *SrcF = Inst->getFalseOperand();
  Operand *Condition = Inst->getCondition();

  if (isVectorType(Dest->getType())) {
    // a=d?b:c ==> d=sext(d); a=(b&d)|(c&~d)
    // TODO(wala): SSE4.1 has blendvps and pblendvb.  SSE4.1 also has
    // blendps and pblendw for constant condition operands.
    Type SrcTy = SrcT->getType();
    Variable *T = makeReg(SrcTy);
    Variable *T2 = makeReg(SrcTy);
    // Sign extend the condition operand if applicable.
    if (SrcTy == IceType_v4f32) {
      // The sext operation takes only integer arguments.
      Variable *T3 = Func->makeVariable(IceType_v4i32, Context.getNode());
      lowerCast(InstCast::create(Func, InstCast::Sext, T3, Condition));
      _movp(T, T3);
    } else if (typeElementType(SrcTy) != IceType_i1) {
      lowerCast(InstCast::create(Func, InstCast::Sext, T, Condition));
    } else {
      _movp(T, Condition);
    }
    // ALIGNHACK: Until stack alignment support is implemented, the
    // bitwise vector instructions need to have both operands in
    // registers.  Once there is support for stack alignment, LEGAL_HACK
    // can be removed.
#define LEGAL_HACK(Vect) legalizeToVar((Vect))
    _movp(T2, T);
    _pand(T, LEGAL_HACK(SrcT));
    _pandn(T2, LEGAL_HACK(SrcF));
    _por(T, T2);
    _movp(Dest, T);
#undef LEGAL_HACK

    return;
  }

  // a=d?b:c ==> cmp d,0; a=b; jne L1; FakeUse(a); a=c; L1:
  Operand *ConditionRMI = legalize(Condition);
  Constant *Zero = Ctx->getConstantZero(IceType_i32);
  InstX8632Label *Label = InstX8632Label::create(Func, this);

  if (Dest->getType() == IceType_i64) {
    Variable *DestLo = llvm::cast<Variable>(loOperand(Dest));
    Variable *DestHi = llvm::cast<Variable>(hiOperand(Dest));
    Operand *SrcLoRI = legalize(loOperand(SrcT), Legal_Reg | Legal_Imm, true);
    Operand *SrcHiRI = legalize(hiOperand(SrcT), Legal_Reg | Legal_Imm, true);
    _cmp(ConditionRMI, Zero);
    _mov(DestLo, SrcLoRI);
    _mov(DestHi, SrcHiRI);
    _br(InstX8632Br::Br_ne, Label);
    Context.insert(InstFakeUse::create(Func, DestLo));
    Context.insert(InstFakeUse::create(Func, DestHi));
    Operand *SrcFLo = loOperand(SrcF);
    Operand *SrcFHi = hiOperand(SrcF);
    SrcLoRI = legalize(SrcFLo, Legal_Reg | Legal_Imm, true);
    SrcHiRI = legalize(SrcFHi, Legal_Reg | Legal_Imm, true);
    _mov(DestLo, SrcLoRI);
    _mov(DestHi, SrcHiRI);
  } else {
    _cmp(ConditionRMI, Zero);
    SrcT = legalize(SrcT, Legal_Reg | Legal_Imm, true);
    _mov(Dest, SrcT);
    _br(InstX8632Br::Br_ne, Label);
    Context.insert(InstFakeUse::create(Func, Dest));
    SrcF = legalize(SrcF, Legal_Reg | Legal_Imm, true);
    _mov(Dest, SrcF);
  }

  Context.insert(Label);
}

void TargetX8632::lowerStore(const InstStore *Inst) {
  Operand *Value = Inst->getData();
  Operand *Addr = Inst->getAddr();
  OperandX8632Mem *NewAddr = FormMemoryOperand(Addr, Value->getType());

  if (NewAddr->getType() == IceType_i64) {
    Value = legalize(Value);
    Operand *ValueHi = legalize(hiOperand(Value), Legal_Reg | Legal_Imm, true);
    Operand *ValueLo = legalize(loOperand(Value), Legal_Reg | Legal_Imm, true);
    _store(ValueHi, llvm::cast<OperandX8632Mem>(hiOperand(NewAddr)));
    _store(ValueLo, llvm::cast<OperandX8632Mem>(loOperand(NewAddr)));
  } else {
    Value = legalize(Value, Legal_Reg | Legal_Imm, true);
    _store(Value, NewAddr);
  }
}

void TargetX8632::doAddressOptStore() {
  InstStore *Inst = llvm::cast<InstStore>(*Context.getCur());
  Operand *Data = Inst->getData();
  Operand *Addr = Inst->getAddr();
  Variable *Index = NULL;
  uint16_t Shift = 0;
  int32_t Offset = 0; // TODO: make Constant
  Variable *Base = llvm::dyn_cast<Variable>(Addr);
  // Vanilla ICE store instructions should not use the segment registers,
  // and computeAddressOpt only works at the level of Variables and Constants,
  // not other OperandX8632Mem, so there should be no mention of segment
  // registers there either.
  const OperandX8632Mem::SegmentRegisters SegmentReg =
      OperandX8632Mem::DefaultSegment;
  computeAddressOpt(Base, Index, Shift, Offset);
  if (Base && Addr != Base) {
    Constant *OffsetOp = Ctx->getConstantInt(IceType_i32, Offset);
    Addr = OperandX8632Mem::create(Func, Data->getType(), Base, OffsetOp, Index,
                                   Shift, SegmentReg);
    Inst->setDeleted();
    Context.insert(InstStore::create(Func, Data, Addr));
  }
}

void TargetX8632::lowerSwitch(const InstSwitch *Inst) {
  // This implements the most naive possible lowering.
  // cmp a,val[0]; jeq label[0]; cmp a,val[1]; jeq label[1]; ... jmp default
  Operand *Src0 = Inst->getComparison();
  SizeT NumCases = Inst->getNumCases();
  // OK, we'll be slightly less naive by forcing Src into a physical
  // register if there are 2 or more uses.
  if (NumCases >= 2)
    Src0 = legalizeToVar(Src0, true);
  else
    Src0 = legalize(Src0, Legal_All, true);
  for (SizeT I = 0; I < NumCases; ++I) {
    Operand *Value = Ctx->getConstantInt(IceType_i32, Inst->getValue(I));
    _cmp(Src0, Value);
    _br(InstX8632Br::Br_e, Inst->getLabel(I));
  }

  _br(Inst->getLabelDefault());
}

// The following pattern occurs often in lowered C and C++ code:
//
//   %cmp     = fcmp/icmp pred <n x ty> %src0, %src1
//   %cmp.ext = sext <n x i1> %cmp to <n x ty>
//
// We can eliminate the sext operation by copying the result of pcmpeqd,
// pcmpgtd, or cmpps (which produce sign extended results) to the result
// of the sext operation.
void
TargetX8632::eliminateNextVectorSextInstruction(Variable *SignExtendedResult) {
  if (InstCast *NextCast =
          llvm::dyn_cast_or_null<InstCast>(Context.getNextInst())) {
    if (NextCast->getCastKind() == InstCast::Sext &&
        NextCast->getSrc(0) == SignExtendedResult) {
      _movp(NextCast->getDest(), legalizeToVar(SignExtendedResult));
      // Skip over the instruction.
      NextCast->setDeleted();
      Context.advanceNext();
    }
  }
}

void TargetX8632::lowerUnreachable(const InstUnreachable * /*Inst*/) {
  const SizeT MaxSrcs = 0;
  Variable *Dest = NULL;
  InstCall *Call = makeHelperCall("ice_unreachable", Dest, MaxSrcs);
  lowerCall(Call);
}

// There is no support for loading or emitting vector constants, so the
// vector values returned from makeVectorOfZeros, makeVectorOfOnes,
// etc. are initialized with register operations.
//
// TODO(wala): Add limited support for vector constants so that
// complex initialization in registers is unnecessary.

Variable *TargetX8632::makeVectorOfZeros(Type Ty, int32_t RegNum) {
  Variable *Reg = makeReg(Ty, RegNum);
  // Insert a FakeDef, since otherwise the live range of Reg might
  // be overestimated.
  Context.insert(InstFakeDef::create(Func, Reg));
  _pxor(Reg, Reg);
  return Reg;
}

Variable *TargetX8632::makeVectorOfMinusOnes(Type Ty, int32_t RegNum) {
  Variable *MinusOnes = makeReg(Ty, RegNum);
  // Insert a FakeDef so the live range of MinusOnes is not overestimated.
  Context.insert(InstFakeDef::create(Func, MinusOnes));
  _pcmpeq(MinusOnes, MinusOnes);
  return MinusOnes;
}

Variable *TargetX8632::makeVectorOfOnes(Type Ty, int32_t RegNum) {
  Variable *Dest = makeVectorOfZeros(Ty, RegNum);
  Variable *MinusOne = makeVectorOfMinusOnes(Ty);
  _psub(Dest, MinusOne);
  return Dest;
}

Variable *TargetX8632::makeVectorOfHighOrderBits(Type Ty, int32_t RegNum) {
  assert(Ty == IceType_v4i32 || Ty == IceType_v4f32 || Ty == IceType_v8i16 ||
         Ty == IceType_v16i8);
  if (Ty == IceType_v4f32 || Ty == IceType_v4i32 || Ty == IceType_v8i16) {
    Variable *Reg = makeVectorOfOnes(Ty, RegNum);
    SizeT Shift = typeWidthInBytes(typeElementType(Ty)) * X86_CHAR_BIT - 1;
    _psll(Reg, Ctx->getConstantInt(IceType_i8, Shift));
    return Reg;
  } else {
    // SSE has no left shift operation for vectors of 8 bit integers.
    const uint32_t HIGH_ORDER_BITS_MASK = 0x80808080;
    Constant *ConstantMask =
        Ctx->getConstantInt(IceType_i32, HIGH_ORDER_BITS_MASK);
    Variable *Reg = makeReg(Ty, RegNum);
    _movd(Reg, legalize(ConstantMask, Legal_Reg | Legal_Mem));
    _pshufd(Reg, Reg, Ctx->getConstantZero(IceType_i8));
    return Reg;
  }
}

OperandX8632Mem *TargetX8632::getMemoryOperandForStackSlot(Type Ty,
                                                           Variable *Slot,
                                                           uint32_t Offset) {
  // Ensure that Loc is a stack slot.
  assert(Slot->getWeight() == RegWeight::Zero);
  assert(Slot->getRegNum() == Variable::NoRegister);
  // Compute the location of Loc in memory.
  // TODO(wala,stichnot): lea should not be required.  The address of
  // the stack slot is known at compile time (although not until after
  // addProlog()).
  const Type PointerType = IceType_i32;
  Variable *Loc = makeReg(PointerType);
  _lea(Loc, Slot);
  Constant *ConstantOffset = Ctx->getConstantInt(IceType_i32, Offset);
  return OperandX8632Mem::create(Func, Ty, Loc, ConstantOffset);
}

// Helper for legalize() to emit the right code to lower an operand to a
// register of the appropriate type.
Variable *TargetX8632::copyToReg(Operand *Src, int32_t RegNum) {
  Type Ty = Src->getType();
  Variable *Reg = makeReg(Ty, RegNum);
  if (isVectorType(Ty)) {
    _movp(Reg, Src);
  } else {
    _mov(Reg, Src);
  }
  return Reg;
}

Operand *TargetX8632::legalize(Operand *From, LegalMask Allowed,
                               bool AllowOverlap, int32_t RegNum) {
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
  if (OperandX8632Mem *Mem = llvm::dyn_cast<OperandX8632Mem>(From)) {
    // Before doing anything with a Mem operand, we need to ensure
    // that the Base and Index components are in physical registers.
    Variable *Base = Mem->getBase();
    Variable *Index = Mem->getIndex();
    Variable *RegBase = NULL;
    Variable *RegIndex = NULL;
    if (Base) {
      RegBase = legalizeToVar(Base, true);
    }
    if (Index) {
      RegIndex = legalizeToVar(Index, true);
    }
    if (Base != RegBase || Index != RegIndex) {
      From = OperandX8632Mem::create(
          Func, Mem->getType(), RegBase, Mem->getOffset(), RegIndex,
          Mem->getShift(), Mem->getSegmentRegister());
    }

    if (!(Allowed & Legal_Mem)) {
      From = copyToReg(From, RegNum);
    }
    return From;
  }
  if (llvm::isa<Constant>(From)) {
    if (llvm::isa<ConstantUndef>(From)) {
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
      if (isVectorType(From->getType()))
        return makeVectorOfZeros(From->getType());
      From = Ctx->getConstantZero(From->getType());
    }
    // There should be no constants of vector type (other than undef).
    assert(!isVectorType(From->getType()));
    bool NeedsReg = false;
    if (!(Allowed & Legal_Imm))
      // Immediate specifically not allowed
      NeedsReg = true;
    // TODO(stichnot): LEAHACK: remove Legal_Reloc once a proper
    // emitter is used.
    if (!(Allowed & Legal_Reloc) && llvm::isa<ConstantRelocatable>(From))
      // Relocatable specifically not allowed
      NeedsReg = true;
    if (!(Allowed & Legal_Mem) &&
        (From->getType() == IceType_f32 || From->getType() == IceType_f64))
      // On x86, FP constants are lowered to mem operands.
      NeedsReg = true;
    if (NeedsReg) {
      From = copyToReg(From, RegNum);
    }
    return From;
  }
  if (Variable *Var = llvm::dyn_cast<Variable>(From)) {
    // Check if the variable is guaranteed a physical register.  This
    // can happen either when the variable is pre-colored or when it is
    // assigned infinite weight.
    bool MustHaveRegister =
        (Var->hasReg() || Var->getWeight() == RegWeight::Inf);
    // We need a new physical register for the operand if:
    //   Mem is not allowed and Var isn't guaranteed a physical
    //   register, or
    //   RegNum is required and Var->getRegNum() doesn't match.
    if ((!(Allowed & Legal_Mem) && !MustHaveRegister) ||
        (RegNum != Variable::NoRegister && RegNum != Var->getRegNum())) {
      Variable *Reg = copyToReg(From, RegNum);
      if (RegNum == Variable::NoRegister) {
        Reg->setPreferredRegister(Var, AllowOverlap);
      }
      From = Reg;
    }
    return From;
  }
  llvm_unreachable("Unhandled operand kind in legalize()");
  return From;
}

// Provide a trivial wrapper to legalize() for this common usage.
Variable *TargetX8632::legalizeToVar(Operand *From, bool AllowOverlap,
                                     int32_t RegNum) {
  return llvm::cast<Variable>(legalize(From, Legal_Reg, AllowOverlap, RegNum));
}

OperandX8632Mem *TargetX8632::FormMemoryOperand(Operand *Operand, Type Ty) {
  OperandX8632Mem *Mem = llvm::dyn_cast<OperandX8632Mem>(Operand);
  // It may be the case that address mode optimization already creates
  // an OperandX8632Mem, so in that case it wouldn't need another level
  // of transformation.
  if (!Mem) {
    Variable *Base = llvm::dyn_cast<Variable>(Operand);
    Constant *Offset = llvm::dyn_cast<Constant>(Operand);
    assert(Base || Offset);
    Mem = OperandX8632Mem::create(Func, Ty, Base, Offset);
  }
  return llvm::cast<OperandX8632Mem>(legalize(Mem));
}

Variable *TargetX8632::makeReg(Type Type, int32_t RegNum) {
  // There aren't any 64-bit integer registers for x86-32.
  assert(Type != IceType_i64);
  Variable *Reg = Func->makeVariable(Type, Context.getNode());
  if (RegNum == Variable::NoRegister)
    Reg->setWeightInfinite();
  else
    Reg->setRegNum(RegNum);
  return Reg;
}

void TargetX8632::postLower() {
  if (Ctx->getOptLevel() != Opt_m1)
    return;
  // TODO: Avoid recomputing WhiteList every instruction.
  RegSetMask RegInclude = RegSet_All;
  RegSetMask RegExclude = RegSet_None;
  if (hasFramePointer())
    RegExclude |= RegSet_FramePointer;
  llvm::SmallBitVector WhiteList = getRegisterSet(RegInclude, RegExclude);
  // Make one pass to black-list pre-colored registers.  TODO: If
  // there was some prior register allocation pass that made register
  // assignments, those registers need to be black-listed here as
  // well.
  for (InstList::iterator I = Context.getCur(), E = Context.getEnd(); I != E;
       ++I) {
    const Inst *Inst = *I;
    if (Inst->isDeleted())
      continue;
    if (llvm::isa<InstFakeKill>(Inst))
      continue;
    for (SizeT SrcNum = 0; SrcNum < Inst->getSrcSize(); ++SrcNum) {
      Operand *Src = Inst->getSrc(SrcNum);
      SizeT NumVars = Src->getNumVars();
      for (SizeT J = 0; J < NumVars; ++J) {
        const Variable *Var = Src->getVar(J);
        if (!Var->hasReg())
          continue;
        WhiteList[Var->getRegNum()] = false;
      }
    }
  }
  // The second pass colors infinite-weight variables.
  llvm::SmallBitVector AvailableRegisters = WhiteList;
  for (InstList::iterator I = Context.getCur(), E = Context.getEnd(); I != E;
       ++I) {
    const Inst *Inst = *I;
    if (Inst->isDeleted())
      continue;
    for (SizeT SrcNum = 0; SrcNum < Inst->getSrcSize(); ++SrcNum) {
      Operand *Src = Inst->getSrc(SrcNum);
      SizeT NumVars = Src->getNumVars();
      for (SizeT J = 0; J < NumVars; ++J) {
        Variable *Var = Src->getVar(J);
        if (Var->hasReg())
          continue;
        if (!Var->getWeight().isInf())
          continue;
        llvm::SmallBitVector AvailableTypedRegisters =
            AvailableRegisters & getRegisterSetForType(Var->getType());
        if (!AvailableTypedRegisters.any()) {
          // This is a hack in case we run out of physical registers
          // due to an excessive number of "push" instructions from
          // lowering a call.
          AvailableRegisters = WhiteList;
          AvailableTypedRegisters =
              AvailableRegisters & getRegisterSetForType(Var->getType());
        }
        assert(AvailableTypedRegisters.any());
        int32_t RegNum = AvailableTypedRegisters.find_first();
        Var->setRegNum(RegNum);
        AvailableRegisters[RegNum] = false;
      }
    }
  }
}

template <> void ConstantInteger::emit(GlobalContext *Ctx) const {
  Ostream &Str = Ctx->getStrEmit();
  Str << getValue();
}

template <> void ConstantFloat::emit(GlobalContext *Ctx) const {
  Ostream &Str = Ctx->getStrEmit();
  // It would be better to prefix with ".L$" instead of "L$", but
  // llvm-mc doesn't parse "dword ptr [.L$foo]".
  Str << "dword ptr [L$" << IceType_f32 << "$" << getPoolEntryID() << "]";
}

template <> void ConstantDouble::emit(GlobalContext *Ctx) const {
  Ostream &Str = Ctx->getStrEmit();
  Str << "qword ptr [L$" << IceType_f64 << "$" << getPoolEntryID() << "]";
}

TargetGlobalInitX8632::TargetGlobalInitX8632(GlobalContext *Ctx)
    : TargetGlobalInitLowering(Ctx) {}

namespace {
char hexdigit(unsigned X) { return X < 10 ? '0' + X : 'A' + X - 10; }
}

void TargetGlobalInitX8632::lower(const IceString &Name, SizeT Align,
                                  bool IsInternal, bool IsConst,
                                  bool IsZeroInitializer, SizeT Size,
                                  const char *Data, bool DisableTranslation) {
  if (Ctx->isVerbose()) {
    // TODO: Consider moving the dump output into the driver to be
    // reused for all targets.
    Ostream &Str = Ctx->getStrDump();
    Str << "@" << Name << " = " << (IsInternal ? "internal" : "external");
    Str << (IsConst ? " constant" : " global");
    Str << " [" << Size << " x i8] ";
    if (IsZeroInitializer) {
      Str << "zeroinitializer";
    } else {
      Str << "c\"";
      // Code taken from PrintEscapedString() in AsmWriter.cpp.  Keep
      // the strings in the same format as the .ll file for practical
      // diffing.
      for (uint64_t i = 0; i < Size; ++i) {
        unsigned char C = Data[i];
        if (isprint(C) && C != '\\' && C != '"')
          Str << C;
        else
          Str << '\\' << hexdigit(C >> 4) << hexdigit(C & 0x0F);
      }
      Str << "\"";
    }
    Str << ", align " << Align << "\n";
  }

  if (DisableTranslation)
    return;

  Ostream &Str = Ctx->getStrEmit();
  // constant:
  //   .section .rodata,"a",@progbits
  //   .align ALIGN
  //   .byte ...
  //   .size NAME, SIZE

  // non-constant:
  //   .data
  //   .align ALIGN
  //   .byte ...
  //   .size NAME, SIZE

  // zeroinitializer (constant):
  //   (.section or .data as above)
  //   .align ALIGN
  //   .zero SIZE
  //   .size NAME, SIZE

  // zeroinitializer (non-constant):
  //   (.section or .data as above)
  //   .comm NAME, SIZE, ALIGN
  //   .local NAME

  IceString MangledName = Ctx->mangleName(Name);
  // Start a new section.
  if (IsConst) {
    Str << "\t.section\t.rodata,\"a\",@progbits\n";
  } else {
    Str << "\t.type\t" << MangledName << ",@object\n";
    Str << "\t.data\n";
  }
  if (IsZeroInitializer) {
    if (IsConst) {
      Str << "\t.align\t" << Align << "\n";
      Str << MangledName << ":\n";
      Str << "\t.zero\t" << Size << "\n";
      Str << "\t.size\t" << MangledName << ", " << Size << "\n";
    } else {
      // TODO(stichnot): Put the appropriate non-constant
      // zeroinitializers in a .bss section to reduce object size.
      Str << "\t.comm\t" << MangledName << ", " << Size << ", " << Align
          << "\n";
    }
  } else {
    Str << "\t.align\t" << Align << "\n";
    Str << MangledName << ":\n";
    for (SizeT i = 0; i < Size; ++i) {
      Str << "\t.byte\t" << (((unsigned)Data[i]) & 0xff) << "\n";
    }
    Str << "\t.size\t" << MangledName << ", " << Size << "\n";
  }
  Str << "\t" << (IsInternal ? ".local" : ".global") << "\t" << MangledName
      << "\n";
}

} // end of namespace Ice
