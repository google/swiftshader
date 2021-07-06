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

#if defined(_WIN64)
extern "C" void __chkstk();
#endif

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

bool shouldBePooled(const class ::Ice::Constant *C) {
  return ::Ice::X8664::TargetX8664::shouldBePooled(C);
}

::Ice::Type getPointerType() {
  return ::Ice::X8664::TargetX8664::getPointerType();
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
  {dflt,                                                                       \
   swapS,                                                                      \
   X8664::Traits::Cond::C1,                                                    \
   X8664::Traits::Cond::C2,                                                    \
   swapV,                                                                      \
   X8664::Traits::Cond::pred},
    FCMPX8664_TABLE
#undef X
};

const size_t TargetX8664Traits::TableFcmpSize = llvm::array_lengthof(TableFcmp);

const TargetX8664Traits::TableIcmp32Type TargetX8664Traits::TableIcmp32[] = {
#define X(val, C_32, C1_64, C2_64, C3_64) {X8664::Traits::Cond::C_32},
    ICMPX8664_TABLE
#undef X
};

const size_t TargetX8664Traits::TableIcmp32Size =
    llvm::array_lengthof(TableIcmp32);

const TargetX8664Traits::TableIcmp64Type TargetX8664Traits::TableIcmp64[] = {
#define X(val, C_32, C1_64, C2_64, C3_64)                                      \
  {X8664::Traits::Cond::C1_64, X8664::Traits::Cond::C2_64,                     \
   X8664::Traits::Cond::C3_64},
    ICMPX8664_TABLE
#undef X
};

const size_t TargetX8664Traits::TableIcmp64Size =
    llvm::array_lengthof(TableIcmp64);

const TargetX8664Traits::TableTypeX8664AttributesType
    TargetX8664Traits::TableTypeX8664Attributes[] = {
#define X(tag, elty, cvt, sdss, pdps, spsd, int_, unpack, pack, width, fld)    \
  {IceType_##elty},
        ICETYPEX8664_TABLE
#undef X
};

const size_t TargetX8664Traits::TableTypeX8664AttributesSize =
    llvm::array_lengthof(TableTypeX8664Attributes);

const uint32_t TargetX8664Traits::X86_STACK_ALIGNMENT_BYTES = 16;
const char *TargetX8664Traits::TargetName = "X8664";

template <>
std::array<SmallBitVector, RCX86_NUM>
    TargetX86Base<X8664::Traits>::TypeToRegisterSet = {{}};

template <>
std::array<SmallBitVector, RCX86_NUM>
    TargetX86Base<X8664::Traits>::TypeToRegisterSetUnfiltered = {{}};

template <>
std::array<SmallBitVector,
           TargetX86Base<X8664::Traits>::Traits::RegisterSet::Reg_NUM>
    TargetX86Base<X8664::Traits>::RegisterAliases = {{}};

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
  _add(rsp, Adjustment);
}

void TargetX8664::_mov_sp(Operand *NewValue) {
  assert(NewValue->getType() == IceType_i32);

  Variable *esp = getPhysicalRegister(Traits::RegisterSet::Reg_esp);
  Variable *rsp =
      getPhysicalRegister(Traits::RegisterSet::Reg_rsp, IceType_i64);

  _redefined(Context.insert<InstFakeDef>(esp, rsp));
  _redefined(_mov(esp, NewValue));
  _redefined(Context.insert<InstFakeDef>(rsp, esp));
}

void TargetX8664::_link_bp() {
  Variable *rsp =
      getPhysicalRegister(Traits::RegisterSet::Reg_rsp, Traits::WordType);
  Variable *rbp =
      getPhysicalRegister(Traits::RegisterSet::Reg_rbp, Traits::WordType);

  _push(rbp);
  _mov(rbp, rsp);

  // Keep ebp live for late-stage liveness analysis (e.g. asm-verbose mode).
  Context.insert<InstFakeUse>(rbp);
}

void TargetX8664::_unlink_bp() {
  Variable *rsp =
      getPhysicalRegister(Traits::RegisterSet::Reg_rsp, IceType_i64);
  Variable *rbp =
      getPhysicalRegister(Traits::RegisterSet::Reg_rbp, IceType_i64);

  // For late-stage liveness analysis (e.g. asm-verbose mode), adding a fake
  // use of rsp before the assignment of rsp=rbp keeps previous rsp
  // adjustments from being dead-code eliminated.
  Context.insert<InstFakeUse>(rsp);
  _mov(rsp, rbp);
  _pop(rbp);
}

void TargetX8664::_push_reg(RegNumT RegNum) {
  if (Traits::isXmm(RegNum)) {
    Variable *reg = getPhysicalRegister(RegNum, IceType_v4f32);
    Variable *rsp =
        getPhysicalRegister(Traits::RegisterSet::Reg_rsp, Traits::WordType);
    auto *address =
        Traits::X86OperandMem::create(Func, reg->getType(), rsp, nullptr);
    _sub_sp(
        Ctx->getConstantInt32(16)); // TODO(capn): accumulate all the offsets
                                    // and adjust the stack pointer once.
    _storep(reg, address);
  } else {
    _push(getPhysicalRegister(RegNum, Traits::WordType));
  }
}

void TargetX8664::_pop_reg(RegNumT RegNum) {
  if (Traits::isXmm(RegNum)) {
    Variable *reg = getPhysicalRegister(RegNum, IceType_v4f32);
    Variable *rsp =
        getPhysicalRegister(Traits::RegisterSet::Reg_rsp, Traits::WordType);
    auto *address =
        Traits::X86OperandMem::create(Func, reg->getType(), rsp, nullptr);
    _movp(reg, address);
    _add_sp(
        Ctx->getConstantInt32(16)); // TODO(capn): accumulate all the offsets
                                    // and adjust the stack pointer once.
  } else {
    _pop(getPhysicalRegister(RegNum, Traits::WordType));
  }
}

void TargetX8664::_sub_sp(Operand *Adjustment) {
  Variable *rsp =
      getPhysicalRegister(Traits::RegisterSet::Reg_rsp, Traits::WordType);

  _sub(rsp, Adjustment);

  // Add a fake use of the stack pointer, to prevent the stack pointer adustment
  // from being dead-code eliminated in a function that doesn't return.
  Context.insert<InstFakeUse>(rsp);
}

void TargetX8664::lowerIndirectJump(Variable *JumpTarget) {
  if (JumpTarget->getType() != IceType_i64) {
    Variable *T = makeReg(IceType_i64);
    _movzx(T, JumpTarget);
    JumpTarget = T;
  }

  _jmp(JumpTarget);
}

Inst *TargetX8664::emitCallToTarget(Operand *CallTarget, Variable *ReturnReg,
                                    size_t NumVariadicFpArgs) {
  Inst *NewCall = nullptr;

  if (CallTarget->getType() == IceType_i64) {
    // x86-64 does not support 64-bit direct calls, so write the value to a
    // register and make an indirect call for Constant call targets.
    RegNumT TargetReg = {};

    // System V: force r11 when calling a variadic function so that rax isn't
    // used, since rax stores the number of FP args (see NumVariadicFpArgs
    // usage below).
#if !defined(_WIN64)
    if (NumVariadicFpArgs > 0)
      TargetReg = Traits::RegisterSet::Reg_r11;
#endif

    if (llvm::isa<Constant>(CallTarget)) {
      Variable *T = makeReg(IceType_i64, TargetReg);
      _mov(T, CallTarget);
      CallTarget = T;
    } else if (llvm::isa<Variable>(CallTarget)) {
      Operand *T = legalizeToReg(CallTarget, TargetReg);
      CallTarget = T;
    }
  }

  // System V: store number of FP args in RAX for variadic calls
#if !defined(_WIN64)
  if (NumVariadicFpArgs > 0) {
    // Store number of FP args (stored in XMM registers) in RAX for variadic
    // calls
    auto *NumFpArgs = Ctx->getConstantInt64(NumVariadicFpArgs);
    Variable *NumFpArgsReg =
        legalizeToReg(NumFpArgs, Traits::RegisterSet::Reg_rax);
    Context.insert<InstFakeUse>(NumFpArgsReg);
  }
#endif

  NewCall = Context.insert<Traits::Insts::Call>(ReturnReg, CallTarget);

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

void TargetX8664::emitStackProbe(size_t StackSizeBytes) {
#if defined(_WIN64)
  // Mirroring the behavior of MSVC here, which emits a _chkstk when locals are
  // >= 4KB, rather than the 8KB claimed by the docs.
  if (StackSizeBytes >= 4096) {
    // __chkstk on Win64 probes the stack up to RSP - EAX, but does not clobber
    // RSP, so we don't need to save and restore it.

    Variable *EAX = makeReg(IceType_i32, Traits::RegisterSet::Reg_eax);
    _mov(EAX, Ctx->getConstantInt32(StackSizeBytes));

    auto *CallTarget =
        Ctx->getConstantInt64(reinterpret_cast<int64_t>(&__chkstk));
    Operand *CallTargetReg =
        legalizeToReg(CallTarget, Traits::RegisterSet::Reg_r11);
    emitCallToTarget(CallTargetReg, nullptr);
  }
#endif
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
#define X(tag, reverse, str) static const int _table1_##tag = InstIcmp::tag;
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
#define X(tag, reverse, str)                                                   \
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
#define X(tag, elty, cvt, sdss, pdps, spsd, int_, unpack, pack, width, fld)    \
  _tmp_##tag,
  ICETYPEX8664_TABLE
#undef X
      _num
};
// Define a set of constants based on high-level table entries.
#define X(tag, sizeLog2, align, elts, elty, str, rcstr)                        \
  static const int _table1_##tag = IceType_##tag;
ICETYPE_TABLE
#undef X
// Define a set of constants based on low-level table entries, and ensure the
// table entry keys are consistent.
#define X(tag, elty, cvt, sdss, pdps, spsd, int_, unpack, pack, width, fld)    \
  static const int _table2_##tag = _tmp_##tag;                                 \
  static_assert(_table1_##tag == _table2_##tag,                                \
                "Inconsistency between ICETYPEX8664_TABLE and ICETYPE_TABLE");
ICETYPEX8664_TABLE
#undef X
// Repeat the static asserts with respect to the high-level table entries in
// case the high-level table has extra entries.
#define X(tag, sizeLog2, align, elts, elty, str, rcstr)                        \
  static_assert(_table1_##tag == _table2_##tag,                                \
                "Inconsistency between ICETYPEX8664_TABLE and ICETYPE_TABLE");
ICETYPE_TABLE
#undef X
} // end of namespace dummy3
} // end of anonymous namespace

} // end of namespace X8664
} // end of namespace Ice
