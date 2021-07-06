//===- subzero/src/IceTargetLoweringX8632.cpp - x86-32 lowering -----------===//
//
//                        The Subzero Code Generator
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief Implements the TargetLoweringX8632 class, which consists almost
/// entirely of the lowering sequence for each high-level instruction.
///
//===----------------------------------------------------------------------===//

#include "IceTargetLoweringX8632.h"

#include "IceTargetLoweringX8632Traits.h"

#if defined(_WIN32)
extern "C" void _chkstk();
#endif

namespace X8632 {
std::unique_ptr<::Ice::TargetLowering> createTargetLowering(::Ice::Cfg *Func) {
  return ::Ice::X8632::TargetX8632::create(Func);
}

std::unique_ptr<::Ice::TargetDataLowering>
createTargetDataLowering(::Ice::GlobalContext *Ctx) {
  return ::Ice::X8632::TargetDataX86<::Ice::X8632::TargetX8632Traits>::create(
      Ctx);
}

std::unique_ptr<::Ice::TargetHeaderLowering>
createTargetHeaderLowering(::Ice::GlobalContext *Ctx) {
  return ::Ice::X8632::TargetHeaderX86::create(Ctx);
}

void staticInit(::Ice::GlobalContext *Ctx) {
  ::Ice::X8632::TargetX8632::staticInit(Ctx);
}

bool shouldBePooled(const class ::Ice::Constant *C) {
  return ::Ice::X8632::TargetX8632::shouldBePooled(C);
}

::Ice::Type getPointerType() {
  return ::Ice::X8632::TargetX8632::getPointerType();
}

} // end of namespace X8632

namespace Ice {
namespace X8632 {

//------------------------------------------------------------------------------
//      ______   ______     ______     __     ______   ______
//     /\__  _\ /\  == \   /\  __ \   /\ \   /\__  _\ /\  ___\
//     \/_/\ \/ \ \  __<   \ \  __ \  \ \ \  \/_/\ \/ \ \___  \
//        \ \_\  \ \_\ \_\  \ \_\ \_\  \ \_\    \ \_\  \/\_____\
//         \/_/   \/_/ /_/   \/_/\/_/   \/_/     \/_/   \/_____/
//
//------------------------------------------------------------------------------
const TargetX8632Traits::TableFcmpType TargetX8632Traits::TableFcmp[] = {
#define X(val, dflt, swapS, C1, C2, swapV, pred)                               \
  {dflt,                                                                       \
   swapS,                                                                      \
   X8632::Traits::Cond::C1,                                                    \
   X8632::Traits::Cond::C2,                                                    \
   swapV,                                                                      \
   X8632::Traits::Cond::pred},
    FCMPX8632_TABLE
#undef X
};

const size_t TargetX8632Traits::TableFcmpSize = llvm::array_lengthof(TableFcmp);

const TargetX8632Traits::TableIcmp32Type TargetX8632Traits::TableIcmp32[] = {
#define X(val, C_32, C1_64, C2_64, C3_64) {X8632::Traits::Cond::C_32},
    ICMPX8632_TABLE
#undef X
};

const size_t TargetX8632Traits::TableIcmp32Size =
    llvm::array_lengthof(TableIcmp32);

const TargetX8632Traits::TableIcmp64Type TargetX8632Traits::TableIcmp64[] = {
#define X(val, C_32, C1_64, C2_64, C3_64)                                      \
  {X8632::Traits::Cond::C1_64, X8632::Traits::Cond::C2_64,                     \
   X8632::Traits::Cond::C3_64},
    ICMPX8632_TABLE
#undef X
};

const size_t TargetX8632Traits::TableIcmp64Size =
    llvm::array_lengthof(TableIcmp64);

const TargetX8632Traits::TableTypeX8632AttributesType
    TargetX8632Traits::TableTypeX8632Attributes[] = {
#define X(tag, elty, cvt, sdss, pdps, spsd, int_, unpack, pack, width, fld)    \
  {IceType_##elty},
        ICETYPEX8632_TABLE
#undef X
};

const size_t TargetX8632Traits::TableTypeX8632AttributesSize =
    llvm::array_lengthof(TableTypeX8632Attributes);

#if defined(_WIN32)
// Windows 32-bit only guarantees 4 byte stack alignment
const uint32_t TargetX8632Traits::X86_STACK_ALIGNMENT_BYTES = 4;
#else
const uint32_t TargetX8632Traits::X86_STACK_ALIGNMENT_BYTES = 16;
#endif
const char *TargetX8632Traits::TargetName = "X8632";

template <>
std::array<SmallBitVector, RCX86_NUM>
    TargetX86Base<X8632::Traits>::TypeToRegisterSet = {{}};

template <>
std::array<SmallBitVector, RCX86_NUM>
    TargetX86Base<X8632::Traits>::TypeToRegisterSetUnfiltered = {{}};

template <>
std::array<SmallBitVector,
           TargetX86Base<X8632::Traits>::Traits::RegisterSet::Reg_NUM>
    TargetX86Base<X8632::Traits>::RegisterAliases = {{}};

//------------------------------------------------------------------------------
//     __      ______  __     __  ______  ______  __  __   __  ______
//    /\ \    /\  __ \/\ \  _ \ \/\  ___\/\  == \/\ \/\ "-.\ \/\  ___\
//    \ \ \___\ \ \/\ \ \ \/ ".\ \ \  __\\ \  __<\ \ \ \ \-.  \ \ \__ \
//     \ \_____\ \_____\ \__/".~\_\ \_____\ \_\ \_\ \_\ \_\\"\_\ \_____\
//      \/_____/\/_____/\/_/   \/_/\/_____/\/_/ /_/\/_/\/_/ \/_/\/_____/
//
//------------------------------------------------------------------------------
void TargetX8632::_add_sp(Operand *Adjustment) {
  Variable *esp = getPhysicalRegister(Traits::RegisterSet::Reg_esp);
  _add(esp, Adjustment);
}

void TargetX8632::_mov_sp(Operand *NewValue) {
  Variable *esp = getPhysicalRegister(Traits::RegisterSet::Reg_esp);
  _redefined(_mov(esp, NewValue));
}

void TargetX8632::_sub_sp(Operand *Adjustment) {
  Variable *esp = getPhysicalRegister(Traits::RegisterSet::Reg_esp);
  _sub(esp, Adjustment);
  // Add a fake use of the stack pointer, to prevent the stack pointer adustment
  // from being dead-code eliminated in a function that doesn't return.
  Context.insert<InstFakeUse>(esp);
}

void TargetX8632::_link_bp() {
  Variable *ebp = getPhysicalRegister(Traits::RegisterSet::Reg_ebp);
  Variable *esp = getPhysicalRegister(Traits::RegisterSet::Reg_esp);
  _push(ebp);
  _mov(ebp, esp);
  // Keep ebp live for late-stage liveness analysis (e.g. asm-verbose mode).
  Context.insert<InstFakeUse>(ebp);
}

void TargetX8632::_unlink_bp() {
  Variable *esp = getPhysicalRegister(Traits::RegisterSet::Reg_esp);
  Variable *ebp = getPhysicalRegister(Traits::RegisterSet::Reg_ebp);
  // For late-stage liveness analysis (e.g. asm-verbose mode), adding a fake
  // use of esp before the assignment of esp=ebp keeps previous esp
  // adjustments from being dead-code eliminated.
  Context.insert<InstFakeUse>(esp);
  _mov(esp, ebp);
  _pop(ebp);
}

void TargetX8632::_push_reg(RegNumT RegNum) {
  _push(getPhysicalRegister(RegNum, Traits::WordType));
}

void TargetX8632::_pop_reg(RegNumT RegNum) {
  _pop(getPhysicalRegister(RegNum, Traits::WordType));
}

void TargetX8632::lowerIndirectJump(Variable *JumpTarget) { _jmp(JumpTarget); }

Inst *TargetX8632::emitCallToTarget(Operand *CallTarget, Variable *ReturnReg,
                                    size_t NumVariadicFpArgs) {
  (void)NumVariadicFpArgs;
  // Note that NumVariadicFpArgs is only used for System V x86-64 variadic
  // calls, because floating point arguments are passed via vector registers,
  // whereas for x86-32, all args are passed via the stack.

  return Context.insert<Traits::Insts::Call>(ReturnReg, CallTarget);
}

Variable *TargetX8632::moveReturnValueToRegister(Operand *Value,
                                                 Type ReturnType) {
  if (isVectorType(ReturnType)) {
    return legalizeToReg(Value, Traits::RegisterSet::Reg_xmm0);
  } else if (isScalarFloatingType(ReturnType)) {
    _fld(Value);
    return nullptr;
  } else {
    assert(ReturnType == IceType_i32 || ReturnType == IceType_i64);
    if (ReturnType == IceType_i64) {
      Variable *eax =
          legalizeToReg(loOperand(Value), Traits::RegisterSet::Reg_eax);
      Variable *edx =
          legalizeToReg(hiOperand(Value), Traits::RegisterSet::Reg_edx);
      Context.insert<InstFakeUse>(edx);
      return eax;
    } else {
      Variable *Reg = nullptr;
      _mov(Reg, Value, Traits::RegisterSet::Reg_eax);
      return Reg;
    }
  }
}

void TargetX8632::emitStackProbe(size_t StackSizeBytes) {
#if defined(_WIN32)
  if (StackSizeBytes >= 4096) {
    // _chkstk on Win32 is actually __alloca_probe, which adjusts ESP by the
    // stack amount specified in EAX, so we save ESP in ECX, and restore them
    // both after the call.

    Variable *EAX = makeReg(IceType_i32, Traits::RegisterSet::Reg_eax);
    Variable *ESP = makeReg(IceType_i32, Traits::RegisterSet::Reg_esp);
    Variable *ECX = makeReg(IceType_i32, Traits::RegisterSet::Reg_ecx);

    _push_reg(ECX->getRegNum());
    _mov(ECX, ESP);

    _mov(EAX, Ctx->getConstantInt32(StackSizeBytes));

    auto *CallTarget =
        Ctx->getConstantInt32(reinterpret_cast<int32_t>(&_chkstk));
    emitCallToTarget(CallTarget, nullptr);

    _mov(ESP, ECX);
    _pop_reg(ECX->getRegNum());
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
// Validate the enum values in FCMPX8632_TABLE.
namespace dummy1 {
// Define a temporary set of enum values based on low-level table entries.
enum _tmp_enum {
#define X(val, dflt, swapS, C1, C2, swapV, pred) _tmp_##val,
  FCMPX8632_TABLE
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
      "Inconsistency between FCMPX8632_TABLE and ICEINSTFCMP_TABLE");
FCMPX8632_TABLE
#undef X
// Repeat the static asserts with respect to the high-level table entries in
// case the high-level table has extra entries.
#define X(tag, str)                                                            \
  static_assert(                                                               \
      _table1_##tag == _table2_##tag,                                          \
      "Inconsistency between FCMPX8632_TABLE and ICEINSTFCMP_TABLE");
ICEINSTFCMP_TABLE
#undef X
} // end of namespace dummy1

// Validate the enum values in ICMPX8632_TABLE.
namespace dummy2 {
// Define a temporary set of enum values based on low-level table entries.
enum _tmp_enum {
#define X(val, C_32, C1_64, C2_64, C3_64) _tmp_##val,
  ICMPX8632_TABLE
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
      "Inconsistency between ICMPX8632_TABLE and ICEINSTICMP_TABLE");
ICMPX8632_TABLE
#undef X
// Repeat the static asserts with respect to the high-level table entries in
// case the high-level table has extra entries.
#define X(tag, reverse, str)                                                   \
  static_assert(                                                               \
      _table1_##tag == _table2_##tag,                                          \
      "Inconsistency between ICMPX8632_TABLE and ICEINSTICMP_TABLE");
ICEINSTICMP_TABLE
#undef X
} // end of namespace dummy2

// Validate the enum values in ICETYPEX8632_TABLE.
namespace dummy3 {
// Define a temporary set of enum values based on low-level table entries.
enum _tmp_enum {
#define X(tag, elty, cvt, sdss, pdps, spsd, int_, unpack, pack, width, fld)    \
  _tmp_##tag,
  ICETYPEX8632_TABLE
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
                "Inconsistency between ICETYPEX8632_TABLE and ICETYPE_TABLE");
ICETYPEX8632_TABLE
#undef X
// Repeat the static asserts with respect to the high-level table entries in
// case the high-level table has extra entries.
#define X(tag, sizeLog2, align, elts, elty, str, rcstr)                        \
  static_assert(_table1_##tag == _table2_##tag,                                \
                "Inconsistency between ICETYPEX8632_TABLE and ICETYPE_TABLE");
ICETYPE_TABLE
#undef X
} // end of namespace dummy3
} // end of anonymous namespace

} // end of namespace X8632
} // end of namespace Ice
