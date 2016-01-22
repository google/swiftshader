//===- subzero/src/IceTargetLoweringX8664.cpp - x86-64 lowering -----------===//
//
//                        The Subzero Code Generator
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief Implements the TargetLoweringX8664 class, which consists almost
/// entirely of the lowering sequence for each high-level instruction.
///
//===----------------------------------------------------------------------===//
#include "IceTargetLoweringX8664.h"

#include "IceDefs.h"
#include "IceTargetLoweringX8664Traits.h"

namespace X8664 {
std::unique_ptr<::Ice::TargetLowering> createTargetLowering(::Ice::Cfg *Func) {
  return ::Ice::X8664::TargetX8664::create(Func);
}

std::unique_ptr<::Ice::TargetDataLowering>
createTargetDataLowering(::Ice::GlobalContext *Ctx) {
  return ::Ice::X8664::TargetDataX86<::Ice::X8664::TargetX8664Traits>::create(
      Ctx);
}

std::unique_ptr<::Ice::TargetHeaderLowering>
createTargetHeaderLowering(::Ice::GlobalContext *Ctx) {
  return ::Ice::X8664::TargetHeaderX86::create(Ctx);
}

void staticInit(::Ice::GlobalContext *Ctx) {
  ::Ice::X8664::TargetX8664::staticInit(Ctx);
}
} // end of namespace X8664

namespace Ice {
namespace X8664 {

//------------------------------------------------------------------------------
//      ______   ______     ______     __     ______   ______
//     /\__  _\ /\  == \   /\  __ \   /\ \   /\__  _\ /\  ___\
//     \/_/\ \/ \ \  __<   \ \  __ \  \ \ \  \/_/\ \/ \ \___  \
//        \ \_\  \ \_\ \_\  \ \_\ \_\  \ \_\    \ \_\  \/\_____\
//         \/_/   \/_/ /_/   \/_/\/_/   \/_/     \/_/   \/_____/
//
//------------------------------------------------------------------------------
const TargetX8664Traits::TableFcmpType TargetX8664Traits::TableFcmp[] = {
#define X(val, dflt, swapS, C1, C2, swapV, pred)                               \
  {                                                                            \
    dflt, swapS, X8664::Traits::Cond::C1, X8664::Traits::Cond::C2, swapV,      \
        X8664::Traits::Cond::pred                                              \
  }                                                                            \
  ,
    FCMPX8664_TABLE
#undef X
};

const size_t TargetX8664Traits::TableFcmpSize = llvm::array_lengthof(TableFcmp);

const TargetX8664Traits::TableIcmp32Type TargetX8664Traits::TableIcmp32[] = {
#define X(val, C_32, C1_64, C2_64, C3_64)                                      \
  { X8664::Traits::Cond::C_32 }                                                \
  ,
    ICMPX8664_TABLE
#undef X
};

const size_t TargetX8664Traits::TableIcmp32Size =
    llvm::array_lengthof(TableIcmp32);

const TargetX8664Traits::TableIcmp64Type TargetX8664Traits::TableIcmp64[] = {
#define X(val, C_32, C1_64, C2_64, C3_64)                                      \
  {                                                                            \
    X8664::Traits::Cond::C1_64, X8664::Traits::Cond::C2_64,                    \
        X8664::Traits::Cond::C3_64                                             \
  }                                                                            \
  ,
    ICMPX8664_TABLE
#undef X
};

const size_t TargetX8664Traits::TableIcmp64Size =
    llvm::array_lengthof(TableIcmp64);

const TargetX8664Traits::TableTypeX8664AttributesType
    TargetX8664Traits::TableTypeX8664Attributes[] = {
#define X(tag, elementty, cvt, sdss, pdps, spsd, pack, width, fld)             \
  { IceType_##elementty }                                                      \
  ,
        ICETYPEX8664_TABLE
#undef X
};

const size_t TargetX8664Traits::TableTypeX8664AttributesSize =
    llvm::array_lengthof(TableTypeX8664Attributes);

const uint32_t TargetX8664Traits::X86_STACK_ALIGNMENT_BYTES = 16;
const char *TargetX8664Traits::TargetName = "X8664";

template <>
std::array<llvm::SmallBitVector, RCX86_NUM>
    TargetX86Base<X8664::Traits>::TypeToRegisterSet = {{}};

template <>
std::array<llvm::SmallBitVector,
           TargetX86Base<X8664::Traits>::Traits::RegisterSet::Reg_NUM>
    TargetX86Base<X8664::Traits>::RegisterAliases = {{}};

template <>
FixupKind TargetX86Base<X8664::Traits>::PcRelFixup =
    TargetX86Base<X8664::Traits>::Traits::FK_PcRel;

template <>
FixupKind TargetX86Base<X8664::Traits>::AbsFixup =
    TargetX86Base<X8664::Traits>::Traits::FK_Abs;

//------------------------------------------------------------------------------
//     __      ______  __     __  ______  ______  __  __   __  ______
//    /\ \    /\  __ \/\ \  _ \ \/\  ___\/\  == \/\ \/\ "-.\ \/\  ___\
//    \ \ \___\ \ \/\ \ \ \/ ".\ \ \  __\\ \  __<\ \ \ \ \-.  \ \ \__ \
//     \ \_____\ \_____\ \__/".~\_\ \_____\ \_\ \_\ \_\ \_\\"\_\ \_____\
//      \/_____/\/_____/\/_/   \/_/\/_____/\/_/ /_/\/_/\/_/ \/_/\/_____/
//
//------------------------------------------------------------------------------
void TargetX8664::_add_sp(Operand *Adjustment) {
  Variable *rsp =
      getPhysicalRegister(Traits::RegisterSet::Reg_rsp, IceType_i64);
  if (!NeedSandboxing) {
    _add(rsp, Adjustment);
    return;
  }

  Variable *esp =
      getPhysicalRegister(Traits::RegisterSet::Reg_esp, IceType_i32);
  Variable *r15 =
      getPhysicalRegister(Traits::RegisterSet::Reg_r15, IceType_i64);

  // When incrementing rsp, NaCl sandboxing requires the following sequence
  //
  // .bundle_start
  // add Adjustment, %esp
  // add %r15, %rsp
  // .bundle_end
  //
  // In Subzero, even though rsp and esp alias each other, defining one does not
  // define the other. Therefore, we must emit
  //
  // .bundle_start
  // %esp = fake-def %rsp
  // add Adjustment, %esp
  // %rsp = fake-def %esp
  // add %r15, %rsp
  // .bundle_end
  //
  // The fake-defs ensure that the
  //
  // add Adjustment, %esp
  //
  // instruction is not DCE'd.
  AutoBundle _(this);
  _redefined(Context.insert<InstFakeDef>(esp, rsp));
  _add(esp, Adjustment);
  _redefined(Context.insert<InstFakeDef>(rsp, esp));
  _add(rsp, r15);
}

void TargetX8664::_mov_sp(Operand *NewValue) {
  assert(NewValue->getType() == IceType_i32);

  Variable *esp = getPhysicalRegister(Traits::RegisterSet::Reg_esp);
  Variable *rsp =
      getPhysicalRegister(Traits::RegisterSet::Reg_rsp, IceType_i64);

  AutoBundle _(this);

  _redefined(Context.insert<InstFakeDef>(esp, rsp));
  _redefined(_mov(esp, NewValue));
  _redefined(Context.insert<InstFakeDef>(rsp, esp));

  if (!NeedSandboxing) {
    return;
  }

  Variable *r15 =
      getPhysicalRegister(Traits::RegisterSet::Reg_r15, IceType_i64);
  _add(rsp, r15);
}

void TargetX8664::_push_rbp() {
  assert(NeedSandboxing);

  Constant *_0 = Ctx->getConstantZero(IceType_i32);
  Variable *ebp =
      getPhysicalRegister(Traits::RegisterSet::Reg_ebp, IceType_i32);
  Variable *rsp =
      getPhysicalRegister(Traits::RegisterSet::Reg_rsp, IceType_i64);
  auto *TopOfStack = llvm::cast<X86OperandMem>(
      legalize(X86OperandMem::create(Func, IceType_i32, rsp, _0),
               Legal_Reg | Legal_Mem));

  // Emits a sequence:
  //
  //   .bundle_start
  //   push 0
  //   mov %ebp, %(rsp)
  //   .bundle_end
  //
  // to avoid leaking the upper 32-bits (i.e., the sandbox address.)
  AutoBundle _(this);
  _push(_0);
  Context.insert<typename Traits::Insts::Store>(ebp, TopOfStack);
}

void TargetX8664::_link_bp() {
  Variable *esp =
      getPhysicalRegister(Traits::RegisterSet::Reg_esp, IceType_i32);
  Variable *rsp =
      getPhysicalRegister(Traits::RegisterSet::Reg_rsp, Traits::WordType);
  Variable *ebp =
      getPhysicalRegister(Traits::RegisterSet::Reg_ebp, IceType_i32);
  Variable *rbp =
      getPhysicalRegister(Traits::RegisterSet::Reg_rbp, Traits::WordType);
  Variable *r15 =
      getPhysicalRegister(Traits::RegisterSet::Reg_r15, Traits::WordType);

  if (!NeedSandboxing) {
    _push(rbp);
    _mov(rbp, rsp);
  } else {
    _push_rbp();

    AutoBundle _(this);
    _redefined(Context.insert<InstFakeDef>(ebp, rbp));
    _redefined(Context.insert<InstFakeDef>(esp, rsp));
    _mov(ebp, esp);
    _redefined(Context.insert<InstFakeDef>(rsp, esp));
    _add(rbp, r15);
  }
  // Keep ebp live for late-stage liveness analysis (e.g. asm-verbose mode).
  Context.insert<InstFakeUse>(rbp);
}

void TargetX8664::_unlink_bp() {
  Variable *rsp =
      getPhysicalRegister(Traits::RegisterSet::Reg_rsp, IceType_i64);
  Variable *rbp =
      getPhysicalRegister(Traits::RegisterSet::Reg_rbp, IceType_i64);
  Variable *ebp =
      getPhysicalRegister(Traits::RegisterSet::Reg_ebp, IceType_i32);
  // For late-stage liveness analysis (e.g. asm-verbose mode), adding a fake
  // use of rsp before the assignment of rsp=rbp keeps previous rsp
  // adjustments from being dead-code eliminated.
  Context.insert<InstFakeUse>(rsp);
  if (!NeedSandboxing) {
    _mov(rsp, rbp);
    _pop(rbp);
  } else {
    _mov_sp(ebp);

    Variable *r15 =
        getPhysicalRegister(Traits::RegisterSet::Reg_r15, IceType_i64);
    Variable *rcx =
        getPhysicalRegister(Traits::RegisterSet::Reg_rcx, IceType_i64);
    Variable *ecx =
        getPhysicalRegister(Traits::RegisterSet::Reg_ecx, IceType_i32);

    _pop(rcx);
    Context.insert<InstFakeDef>(ecx, rcx);
    AutoBundle _(this);
    _mov(ebp, ecx);

    _redefined(Context.insert<InstFakeDef>(rbp, ebp));
    _add(rbp, r15);
  }
}

void TargetX8664::_push_reg(Variable *Reg) {
  Variable *rbp =
      getPhysicalRegister(Traits::RegisterSet::Reg_rbp, Traits::WordType);
  if (Reg != rbp || !NeedSandboxing) {
    _push(Reg);
  } else {
    _push_rbp();
  }
}

void TargetX8664::emitGetIP(CfgNode *Node) {
  // No IP base register is needed on X86-64.
  (void)Node;
}

Traits::X86OperandMem *TargetX8664::_sandbox_mem_reference(X86OperandMem *Mem) {
  // In x86_64-nacl, all memory references are relative to %r15 (i.e., %rzp.)
  // NaCl sandboxing also requires that any registers that are not %rsp and
  // %rbp to be 'truncated' to 32-bit before memory access.
  if (SandboxingType == ST_None) {
    return Mem;
  }

  if (SandboxingType == ST_Nonsfi) {
    llvm::report_fatal_error(
        "_sandbox_mem_reference not implemented for nonsfi");
  }

  Variable *Base = Mem->getBase();
  Variable *Index = Mem->getIndex();
  uint16_t Shift = 0;
  Variable *ZeroReg =
      getPhysicalRegister(Traits::RegisterSet::Reg_r15, IceType_i64);
  Constant *Offset = Mem->getOffset();
  Variable *T = nullptr;

  if (Mem->getIsRebased()) {
    // If Mem.IsRebased, then we don't need to update Mem to contain a reference
    // to a valid base register (%r15, %rsp, or %rbp), but we still need to
    // truncate Mem.Index (if any) to 32-bit.
    assert(ZeroReg == Base || Base->isRematerializable());
    T = makeReg(IceType_i32);
    _mov(T, Index);
    Shift = Mem->getShift();
  } else {
    if (Base != nullptr) {
      if (Base->isRematerializable()) {
        ZeroReg = Base;
      } else {
        T = Base;
      }
    }

    if (Index != nullptr) {
      assert(!Index->isRematerializable());
      if (T != nullptr) {
        llvm::report_fatal_error("memory reference contains base and index.");
      }
      T = Index;
      Shift = Mem->getShift();
    }
  }

  // NeedsLea is a flags indicating whether Mem needs to be materialized to a
  // GPR prior to being used. A LEA is needed if Mem.Offset is a constant
  // relocatable, or if Mem.Offset is negative. In both these cases, the LEA is
  // needed to ensure the sandboxed memory operand will only use the lower
  // 32-bits of T+Offset.
  bool NeedsLea = false;
  if (const auto *Offset = Mem->getOffset()) {
    if (llvm::isa<ConstantRelocatable>(Offset)) {
      NeedsLea = true;
    } else if (const auto *Imm = llvm::cast<ConstantInteger32>(Offset)) {
      NeedsLea = Imm->getValue() < 0;
    }
  }

  int32_t RegNum = Variable::NoRegister;
  int32_t RegNum32 = Variable::NoRegister;
  if (T != nullptr) {
    if (T->hasReg()) {
      RegNum = Traits::getGprForType(IceType_i64, T->getRegNum());
      RegNum32 = Traits::getGprForType(IceType_i32, RegNum);
      switch (RegNum) {
      case Traits::RegisterSet::Reg_rsp:
      case Traits::RegisterSet::Reg_rbp:
        // Memory operands referencing rsp/rbp do not need to be sandboxed.
        return Mem;
      }
    }

    switch (T->getType()) {
    default:
    case IceType_i64:
      // Even though "default:" would also catch T.Type == IceType_i64, an
      // explicit 'case IceType_i64' shows that memory operands are always
      // supposed to be 32-bits.
      llvm::report_fatal_error("Mem pointer should be 32-bit.");
    case IceType_i32: {
      Variable *T64 = makeReg(IceType_i64, RegNum);
      auto *Movzx = _movzx(T64, T);
      if (!NeedsLea) {
        // This movzx is only needed when Mem does not need to be lea'd into a
        // temporary. If an lea is going to be emitted, then eliding this movzx
        // is safe because the emitted lea will write a 32-bit result --
        // implicitly zero-extended to 64-bit.
        Movzx->setMustKeep();
      }
      T = T64;
    } break;
    }
  }

  if (NeedsLea) {
    Variable *NewT = makeReg(IceType_i32, RegNum32);
    Variable *Base = T;
    Variable *Index = T;
    static constexpr bool NotRebased = false;
    if (Shift == 0) {
      Index = nullptr;
    } else {
      Base = nullptr;
    }
    _lea(NewT, Traits::X86OperandMem::create(
                   Func, Mem->getType(), Base, Offset, Index, Shift,
                   Traits::X86OperandMem::DefaultSegment, NotRebased));

    T = makeReg(IceType_i64, RegNum);
    _movzx(T, NewT);
    Shift = 0;
    Offset = nullptr;
  }

  static constexpr bool IsRebased = true;
  return Traits::X86OperandMem::create(
      Func, Mem->getType(), ZeroReg, Offset, T, Shift,
      Traits::X86OperandMem::DefaultSegment, IsRebased);
}

void TargetX8664::_sub_sp(Operand *Adjustment) {
  Variable *rsp =
      getPhysicalRegister(Traits::RegisterSet::Reg_rsp, Traits::WordType);
  if (!NeedSandboxing) {
    _sub(rsp, Adjustment);
    return;
  }

  Variable *esp =
      getPhysicalRegister(Traits::RegisterSet::Reg_esp, IceType_i32);
  Variable *r15 =
      getPhysicalRegister(Traits::RegisterSet::Reg_r15, IceType_i64);

  // .bundle_start
  // sub Adjustment, %esp
  // add %r15, %rsp
  // .bundle_end
  AutoBundle _(this);
  _redefined(Context.insert<InstFakeDef>(esp, rsp));
  _sub(esp, Adjustment);
  _redefined(Context.insert<InstFakeDef>(rsp, esp));
  _add(rsp, r15);
}

void TargetX8664::initRebasePtr() {
  switch (SandboxingType) {
  case ST_Nonsfi:
    // Probably no implementation is needed, but error to be safe for now.
    llvm::report_fatal_error(
        "initRebasePtr() is not yet implemented on x32-nonsfi.");
  case ST_NaCl:
    RebasePtr = getPhysicalRegister(Traits::RegisterSet::Reg_r15, IceType_i64);
    break;
  case ST_None:
    // nothing.
    break;
  }
}

void TargetX8664::initSandbox() {
  assert(SandboxingType == ST_NaCl);
  Context.init(Func->getEntryNode());
  Context.setInsertPoint(Context.getCur());
  Variable *r15 =
      getPhysicalRegister(Traits::RegisterSet::Reg_r15, IceType_i64);
  Context.insert<InstFakeDef>(r15);
  Context.insert<InstFakeUse>(r15);
}

namespace {
bool isRematerializable(const Variable *Var) {
  return Var != nullptr && Var->isRematerializable();
}
} // end of anonymous namespace

bool TargetX8664::legalizeOptAddrForSandbox(OptAddr *Addr) {
  if (SandboxingType == ST_Nonsfi) {
    llvm::report_fatal_error("Nonsfi not yet implemented for x8664.");
  }

  if (isRematerializable(Addr->Base)) {
    if (Addr->Index == RebasePtr) {
      Addr->Index = nullptr;
      Addr->Shift = 0;
    }
    return true;
  }

  if (isRematerializable(Addr->Index)) {
    if (Addr->Base == RebasePtr) {
      Addr->Base = nullptr;
    }
    return true;
  }

  assert(Addr->Base != RebasePtr && Addr->Index != RebasePtr);

  if (Addr->Base == nullptr) {
    return true;
  }

  if (Addr->Index == nullptr) {
    return true;
  }

  return false;
}

void TargetX8664::lowerIndirectJump(Variable *JumpTarget) {
  std::unique_ptr<AutoBundle> Bundler;

  if (!NeedSandboxing) {
    Variable *T = makeReg(IceType_i64);
    _movzx(T, JumpTarget);
    JumpTarget = T;
  } else {
    Variable *T = makeReg(IceType_i32);
    Variable *T64 = makeReg(IceType_i64);
    Variable *r15 =
        getPhysicalRegister(Traits::RegisterSet::Reg_r15, IceType_i64);

    _mov(T, JumpTarget);
    Bundler = makeUnique<AutoBundle>(this);
    const SizeT BundleSize =
        1 << Func->getAssembler<>()->getBundleAlignLog2Bytes();
    _and(T, Ctx->getConstantInt32(~(BundleSize - 1)));
    _movzx(T64, T);
    _add(T64, r15);
    JumpTarget = T64;
  }

  _jmp(JumpTarget);
}

Inst *TargetX8664::emitCallToTarget(Operand *CallTarget, Variable *ReturnReg) {
  Inst *NewCall = nullptr;
  auto *CallTargetR = llvm::dyn_cast<Variable>(CallTarget);
  if (NeedSandboxing) {
    InstX86Label *ReturnAddress = InstX86Label::create(Func, this);
    ReturnAddress->setIsReturnLocation(true);
    constexpr bool SuppressMangling = true;
    /* AutoBundle scoping */ {
      std::unique_ptr<AutoBundle> Bundler;
      if (CallTargetR == nullptr) {
        Bundler = makeUnique<AutoBundle>(this, InstBundleLock::Opt_PadToEnd);
        _push(Ctx->getConstantSym(0, ReturnAddress->getName(Func),
                                  SuppressMangling));
      } else {
        Variable *T = makeReg(IceType_i32);
        Variable *T64 = makeReg(IceType_i64);
        Variable *r15 =
            getPhysicalRegister(Traits::RegisterSet::Reg_r15, IceType_i64);

        _mov(T, CallTargetR);
        Bundler = makeUnique<AutoBundle>(this, InstBundleLock::Opt_PadToEnd);
        _push(Ctx->getConstantSym(0, ReturnAddress->getName(Func),
                                  SuppressMangling));
        const SizeT BundleSize =
            1 << Func->getAssembler<>()->getBundleAlignLog2Bytes();
        _and(T, Ctx->getConstantInt32(~(BundleSize - 1)));
        _movzx(T64, T);
        _add(T64, r15);
        CallTarget = T64;
      }

      NewCall = Context.insert<Traits::Insts::Jmp>(CallTarget);
    }
    if (ReturnReg != nullptr) {
      Context.insert<InstFakeDef>(ReturnReg);
    }

    Context.insert(ReturnAddress);
  } else {
    if (CallTargetR != nullptr) {
      // x86-64 in Subzero is ILP32. Therefore, CallTarget is i32, but the
      // emitted call needs a i64 register (for textual asm.)
      Variable *T = makeReg(IceType_i64);
      _movzx(T, CallTargetR);
      CallTarget = T;
    }
    NewCall = Context.insert<Traits::Insts::Call>(ReturnReg, CallTarget);
  }
  return NewCall;
}

Variable *TargetX8664::moveReturnValueToRegister(Operand *Value,
                                                 Type ReturnType) {
  if (isVectorType(ReturnType) || isScalarFloatingType(ReturnType)) {
    return legalizeToReg(Value, Traits::RegisterSet::Reg_xmm0);
  } else {
    assert(ReturnType == IceType_i32 || ReturnType == IceType_i64);
    Variable *Reg = nullptr;
    _mov(Reg, Value,
         Traits::getGprForType(ReturnType, Traits::RegisterSet::Reg_rax));
    return Reg;
  }
}

void TargetX8664::emitSandboxedReturn() {
  Variable *T_rcx = makeReg(IceType_i64, Traits::RegisterSet::Reg_rcx);
  Variable *T_ecx = makeReg(IceType_i32, Traits::RegisterSet::Reg_ecx);
  _pop(T_rcx);
  _mov(T_ecx, T_rcx);
  // lowerIndirectJump(T_ecx);
  Variable *r15 =
      getPhysicalRegister(Traits::RegisterSet::Reg_r15, IceType_i64);

  /* AutoBundle scoping */ {
    AutoBundle _(this);
    const SizeT BundleSize =
        1 << Func->getAssembler<>()->getBundleAlignLog2Bytes();
    _and(T_ecx, Ctx->getConstantInt32(~(BundleSize - 1)));
    Context.insert<InstFakeDef>(T_rcx, T_ecx);
    _add(T_rcx, r15);

    _jmp(T_rcx);
  }
}

// In some cases, there are x-macros tables for both high-level and low-level
// instructions/operands that use the same enum key value. The tables are kept
// separate to maintain a proper separation between abstraction layers. There
// is a risk that the tables could get out of sync if enum values are reordered
// or if entries are added or deleted. The following dummy namespaces use
// static_asserts to ensure everything is kept in sync.

namespace {
// Validate the enum values in FCMPX8664_TABLE.
namespace dummy1 {
// Define a temporary set of enum values based on low-level table entries.
enum _tmp_enum {
#define X(val, dflt, swapS, C1, C2, swapV, pred) _tmp_##val,
  FCMPX8664_TABLE
#undef X
      _num
};
// Define a set of constants based on high-level table entries.
#define X(tag, str) static const int _table1_##tag = InstFcmp::tag;
ICEINSTFCMP_TABLE
#undef X
// Define a set of constants based on low-level table entries, and ensure the
// table entry keys are consistent.
#define X(val, dflt, swapS, C1, C2, swapV, pred)                               \
  static const int _table2_##val = _tmp_##val;                                 \
  static_assert(                                                               \
      _table1_##val == _table2_##val,                                          \
      "Inconsistency between FCMPX8664_TABLE and ICEINSTFCMP_TABLE");
FCMPX8664_TABLE
#undef X
// Repeat the static asserts with respect to the high-level table entries in
// case the high-level table has extra entries.
#define X(tag, str)                                                            \
  static_assert(                                                               \
      _table1_##tag == _table2_##tag,                                          \
      "Inconsistency between FCMPX8664_TABLE and ICEINSTFCMP_TABLE");
ICEINSTFCMP_TABLE
#undef X
} // end of namespace dummy1

// Validate the enum values in ICMPX8664_TABLE.
namespace dummy2 {
// Define a temporary set of enum values based on low-level table entries.
enum _tmp_enum {
#define X(val, C_32, C1_64, C2_64, C3_64) _tmp_##val,
  ICMPX8664_TABLE
#undef X
      _num
};
// Define a set of constants based on high-level table entries.
#define X(tag, str) static const int _table1_##tag = InstIcmp::tag;
ICEINSTICMP_TABLE
#undef X
// Define a set of constants based on low-level table entries, and ensure the
// table entry keys are consistent.
#define X(val, C_32, C1_64, C2_64, C3_64)                                      \
  static const int _table2_##val = _tmp_##val;                                 \
  static_assert(                                                               \
      _table1_##val == _table2_##val,                                          \
      "Inconsistency between ICMPX8664_TABLE and ICEINSTICMP_TABLE");
ICMPX8664_TABLE
#undef X
// Repeat the static asserts with respect to the high-level table entries in
// case the high-level table has extra entries.
#define X(tag, str)                                                            \
  static_assert(                                                               \
      _table1_##tag == _table2_##tag,                                          \
      "Inconsistency between ICMPX8664_TABLE and ICEINSTICMP_TABLE");
ICEINSTICMP_TABLE
#undef X
} // end of namespace dummy2

// Validate the enum values in ICETYPEX8664_TABLE.
namespace dummy3 {
// Define a temporary set of enum values based on low-level table entries.
enum _tmp_enum {
#define X(tag, elementty, cvt, sdss, pdps, spsd, pack, width, fld) _tmp_##tag,
  ICETYPEX8664_TABLE
#undef X
      _num
};
// Define a set of constants based on high-level table entries.
#define X(tag, sizeLog2, align, elts, elty, str)                               \
  static const int _table1_##tag = IceType_##tag;
ICETYPE_TABLE
#undef X
// Define a set of constants based on low-level table entries, and ensure the
// table entry keys are consistent.
#define X(tag, elementty, cvt, sdss, pdps, spsd, pack, width, fld)             \
  static const int _table2_##tag = _tmp_##tag;                                 \
  static_assert(_table1_##tag == _table2_##tag,                                \
                "Inconsistency between ICETYPEX8664_TABLE and ICETYPE_TABLE");
ICETYPEX8664_TABLE
#undef X
// Repeat the static asserts with respect to the high-level table entries in
// case the high-level table has extra entries.
#define X(tag, sizeLog2, align, elts, elty, str)                               \
  static_assert(_table1_##tag == _table2_##tag,                                \
                "Inconsistency between ICETYPEX8664_TABLE and ICETYPE_TABLE");
ICETYPE_TABLE
#undef X
} // end of namespace dummy3
} // end of anonymous namespace

} // end of namespace X8664
} // end of namespace Ice
