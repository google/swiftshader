//===- subzero/src/IceTargetLoweringX8632Traits.h - x86-32 traits -*- C++ -*-=//
//
//                        The Subzero Code Generator
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief Declares the X8632 Target Lowering Traits.
///
//===----------------------------------------------------------------------===//

#ifndef SUBZERO_SRC_ICETARGETLOWERINGX8632TRAITS_H
#define SUBZERO_SRC_ICETARGETLOWERINGX8632TRAITS_H

#include "IceAssembler.h"
#include "IceConditionCodesX86.h"
#include "IceDefs.h"
#include "IceInst.h"
#include "IceOperand.h"
#include "IceRegistersX8632.h"
#include "IceTargetLowering.h"
#include "IceTargetLoweringX8632.def"
#include "IceTargetLoweringX86RegClass.h"

#include <array>

namespace Ice {
namespace X8632 {
using namespace ::Ice::X86;

struct Insts;
class TargetX8632;
class AssemblerX8632;
class TargetX8632;

struct TargetX8632Traits {
  using RegisterSet = ::Ice::RegX8632;
  using GPRRegister = RegisterSet::GPRRegister;
  using ByteRegister = RegisterSet::ByteRegister;
  using XmmRegister = RegisterSet::XmmRegister;
  using X87STRegister = RegisterSet::X87STRegister;

  //----------------------------------------------------------------------------
  //     __      ______  __     __  ______  ______  __  __   __  ______
  //    /\ \    /\  __ \/\ \  _ \ \/\  ___\/\  == \/\ \/\ "-.\ \/\  ___\
  //    \ \ \___\ \ \/\ \ \ \/ ".\ \ \  __\\ \  __<\ \ \ \ \-.  \ \ \__ \
  //     \ \_____\ \_____\ \__/".~\_\ \_____\ \_\ \_\ \_\ \_\\"\_\ \_____\
  //      \/_____/\/_____/\/_/   \/_/\/_____/\/_/ /_/\/_/\/_/ \/_/\/_____/
  //
  //----------------------------------------------------------------------------
  static const char *TargetName;
  static constexpr Type WordType = IceType_i32;

  static const char *getRegName(RegNumT RegNum) {
    static const char *const RegNames[RegisterSet::Reg_NUM] = {
#define X(val, encode, name, base, scratch, preserved, stackptr, frameptr,     \
          isGPR, is64, is32, is16, is8, isXmm, is64To8, is32To8, is16To8,      \
          isTrunc8Rcvr, isAhRcvr, aliases)                                     \
  name,
        REGX8632_TABLE
#undef X
    };
    RegNum.assertIsValid();
    return RegNames[RegNum];
  }

  static GPRRegister getEncodedGPR(RegNumT RegNum) {
    static const GPRRegister GPRRegs[RegisterSet::Reg_NUM] = {
#define X(val, encode, name, base, scratch, preserved, stackptr, frameptr,     \
          isGPR, is64, is32, is16, is8, isXmm, is64To8, is32To8, is16To8,      \
          isTrunc8Rcvr, isAhRcvr, aliases)                                     \
  GPRRegister(isGPR ? encode : GPRRegister::Encoded_Not_GPR),
        REGX8632_TABLE
#undef X
    };
    RegNum.assertIsValid();
    assert(GPRRegs[RegNum] != GPRRegister::Encoded_Not_GPR);
    return GPRRegs[RegNum];
  }

  static ByteRegister getEncodedByteReg(RegNumT RegNum) {
    static const ByteRegister ByteRegs[RegisterSet::Reg_NUM] = {
#define X(val, encode, name, base, scratch, preserved, stackptr, frameptr,     \
          isGPR, is64, is32, is16, is8, isXmm, is64To8, is32To8, is16To8,      \
          isTrunc8Rcvr, isAhRcvr, aliases)                                     \
  ByteRegister(is8 ? encode : ByteRegister::Encoded_Not_ByteReg),
        REGX8632_TABLE
#undef X
    };
    RegNum.assertIsValid();
    assert(ByteRegs[RegNum] != ByteRegister::Encoded_Not_ByteReg);
    return ByteRegs[RegNum];
  }

  static bool isXmm(RegNumT RegNum) {
    static const bool IsXmm[RegisterSet::Reg_NUM] = {
#define X(val, encode, name, base, scratch, preserved, stackptr, frameptr,     \
          isGPR, is64, is32, is16, is8, isXmm, is64To8, is32To8, is16To8,      \
          isTrunc8Rcvr, isAhRcvr, aliases)                                     \
  isXmm,
        REGX8632_TABLE
#undef X
    };
    return IsXmm[RegNum];
  }

  static XmmRegister getEncodedXmm(RegNumT RegNum) {
    static const XmmRegister XmmRegs[RegisterSet::Reg_NUM] = {
#define X(val, encode, name, base, scratch, preserved, stackptr, frameptr,     \
          isGPR, is64, is32, is16, is8, isXmm, is64To8, is32To8, is16To8,      \
          isTrunc8Rcvr, isAhRcvr, aliases)                                     \
  XmmRegister(isXmm ? encode : XmmRegister::Encoded_Not_Xmm),
        REGX8632_TABLE
#undef X
    };
    RegNum.assertIsValid();
    assert(XmmRegs[RegNum] != XmmRegister::Encoded_Not_Xmm);
    return XmmRegs[RegNum];
  }

  static uint32_t getEncoding(RegNumT RegNum) {
    static const uint32_t Encoding[RegisterSet::Reg_NUM] = {
#define X(val, encode, name, base, scratch, preserved, stackptr, frameptr,     \
          isGPR, is64, is32, is16, is8, isXmm, is64To8, is32To8, is16To8,      \
          isTrunc8Rcvr, isAhRcvr, aliases)                                     \
  encode,
        REGX8632_TABLE
#undef X
    };
    RegNum.assertIsValid();
    return Encoding[RegNum];
  }

  static RegNumT getBaseReg(RegNumT RegNum) {
    static const RegNumT BaseRegs[RegisterSet::Reg_NUM] = {
#define X(val, encode, name, base, scratch, preserved, stackptr, frameptr,     \
          isGPR, is64, is32, is16, is8, isXmm, is64To8, is32To8, is16To8,      \
          isTrunc8Rcvr, isAhRcvr, aliases)                                     \
  RegisterSet::base,
        REGX8632_TABLE
#undef X
    };
    RegNum.assertIsValid();
    return BaseRegs[RegNum];
  }

private:
  static RegisterSet::AllRegisters getFirstGprForType(Type Ty) {
    switch (Ty) {
    default:
      llvm_unreachable("Invalid type for GPR.");
    case IceType_i1:
    case IceType_i8:
      return RegisterSet::Reg_al;
    case IceType_i16:
      return RegisterSet::Reg_ax;
    case IceType_i32:
      return RegisterSet::Reg_eax;
    }
  }

public:
  // Return a register in RegNum's alias set that is suitable for Ty.
  static RegNumT getGprForType(Type Ty, RegNumT RegNum) {
    assert(RegNum.hasValue());

    if (!isScalarIntegerType(Ty)) {
      return RegNum;
    }

    // [abcd]h registers are not convertible to their ?l, ?x, and e?x versions.
    switch (RegNum) {
    default:
      break;
    case RegisterSet::Reg_ah:
    case RegisterSet::Reg_bh:
    case RegisterSet::Reg_ch:
    case RegisterSet::Reg_dh:
      assert(isByteSizedType(Ty));
      return RegNum;
    }

    const RegisterSet::AllRegisters FirstGprForType = getFirstGprForType(Ty);

    switch (RegNum) {
    default:
      llvm::report_fatal_error("Unknown register.");
#define X(val, encode, name, base, scratch, preserved, stackptr, frameptr,     \
          isGPR, is64, is32, is16, is8, isXmm, is64To8, is32To8, is16To8,      \
          isTrunc8Rcvr, isAhRcvr, aliases)                                     \
  case RegisterSet::val: {                                                     \
    if (!isGPR)                                                                \
      return RegisterSet::val;                                                 \
    assert((is32) || (is16) || (is8) ||                                        \
           getBaseReg(RegisterSet::val) == RegisterSet::Reg_esp);              \
    constexpr RegisterSet::AllRegisters FirstGprWithRegNumSize =               \
        (((is32) || RegisterSet::val == RegisterSet::Reg_esp)                  \
             ? RegisterSet::Reg_eax                                            \
             : (((is16) || RegisterSet::val == RegisterSet::Reg_sp)            \
                    ? RegisterSet::Reg_ax                                      \
                    : RegisterSet::Reg_al));                                   \
    const RegNumT NewRegNum =                                                  \
        RegNumT::fixme(RegNum - FirstGprWithRegNumSize + FirstGprForType);     \
    assert(getBaseReg(RegNum) == getBaseReg(NewRegNum) &&                      \
           "Error involving " #val);                                           \
    return NewRegNum;                                                          \
  }
      REGX8632_TABLE
#undef X
    }
  }

private:
  /// SizeOf is used to obtain the size of an initializer list as a constexpr
  /// expression. This is only needed until our C++ library is updated to
  /// C++ 14 -- which defines constexpr members to std::initializer_list.
  class SizeOf {
    SizeOf(const SizeOf &) = delete;
    SizeOf &operator=(const SizeOf &) = delete;

  public:
    constexpr SizeOf() : Size(0) {}
    template <typename... T>
    explicit constexpr SizeOf(T...) : Size(__length<T...>::value) {}
    constexpr SizeT size() const { return Size; }

  private:
    template <typename T, typename... U> struct __length {
      static constexpr std::size_t value = 1 + __length<U...>::value;
    };

    template <typename T> struct __length<T> {
      static constexpr std::size_t value = 1;
    };

    const std::size_t Size;
  };

public:
  static void initRegisterSet(
      const ::Ice::ClFlags & /*Flags*/,
      std::array<SmallBitVector, RCX86_NUM> *TypeToRegisterSet,
      std::array<SmallBitVector, RegisterSet::Reg_NUM> *RegisterAliases) {
    SmallBitVector IntegerRegistersI32(RegisterSet::Reg_NUM);
    SmallBitVector IntegerRegistersI16(RegisterSet::Reg_NUM);
    SmallBitVector IntegerRegistersI8(RegisterSet::Reg_NUM);
    SmallBitVector FloatRegisters(RegisterSet::Reg_NUM);
    SmallBitVector VectorRegisters(RegisterSet::Reg_NUM);
    SmallBitVector Trunc64To8Registers(RegisterSet::Reg_NUM);
    SmallBitVector Trunc32To8Registers(RegisterSet::Reg_NUM);
    SmallBitVector Trunc16To8Registers(RegisterSet::Reg_NUM);
    SmallBitVector Trunc8RcvrRegisters(RegisterSet::Reg_NUM);
    SmallBitVector AhRcvrRegisters(RegisterSet::Reg_NUM);
    SmallBitVector InvalidRegisters(RegisterSet::Reg_NUM);

    static constexpr struct {
      uint16_t Val;
      unsigned Is64 : 1;
      unsigned Is32 : 1;
      unsigned Is16 : 1;
      unsigned Is8 : 1;
      unsigned IsXmm : 1;
      unsigned Is64To8 : 1;
      unsigned Is32To8 : 1;
      unsigned Is16To8 : 1;
      unsigned IsTrunc8Rcvr : 1;
      unsigned IsAhRcvr : 1;
#define NUM_ALIASES_BITS 2
      SizeT NumAliases : (NUM_ALIASES_BITS + 1);
      uint16_t Aliases[1 << NUM_ALIASES_BITS];
#undef NUM_ALIASES_BITS
    } X8632RegTable[RegisterSet::Reg_NUM] = {
#define X(val, encode, name, base, scratch, preserved, stackptr, frameptr,     \
          isGPR, is64, is32, is16, is8, isXmm, is64To8, is32To8, is16To8,      \
          isTrunc8Rcvr, isAhRcvr, aliases)                                     \
  {                                                                            \
      RegisterSet::val,                                                        \
      is64,                                                                    \
      is32,                                                                    \
      is16,                                                                    \
      is8,                                                                     \
      isXmm,                                                                   \
      is64To8,                                                                 \
      is32To8,                                                                 \
      is16To8,                                                                 \
      isTrunc8Rcvr,                                                            \
      isAhRcvr,                                                                \
      (SizeOf aliases).size(),                                                 \
      aliases,                                                                 \
  },
        REGX8632_TABLE
#undef X
    };

    for (SizeT ii = 0; ii < llvm::array_lengthof(X8632RegTable); ++ii) {
      const auto &Entry = X8632RegTable[ii];
      (IntegerRegistersI32)[Entry.Val] = Entry.Is32;
      (IntegerRegistersI16)[Entry.Val] = Entry.Is16;
      (IntegerRegistersI8)[Entry.Val] = Entry.Is8;
      (FloatRegisters)[Entry.Val] = Entry.IsXmm;
      (VectorRegisters)[Entry.Val] = Entry.IsXmm;
      (Trunc64To8Registers)[Entry.Val] = Entry.Is64To8;
      (Trunc32To8Registers)[Entry.Val] = Entry.Is32To8;
      (Trunc16To8Registers)[Entry.Val] = Entry.Is16To8;
      (Trunc8RcvrRegisters)[Entry.Val] = Entry.IsTrunc8Rcvr;
      (AhRcvrRegisters)[Entry.Val] = Entry.IsAhRcvr;
      (*RegisterAliases)[Entry.Val].resize(RegisterSet::Reg_NUM);
      for (SizeT J = 0; J < Entry.NumAliases; J++) {
        SizeT Alias = Entry.Aliases[J];
        assert(!(*RegisterAliases)[Entry.Val][Alias] && "Duplicate alias");
        (*RegisterAliases)[Entry.Val].set(Alias);
      }
      (*RegisterAliases)[Entry.Val].set(Entry.Val);
    }

    (*TypeToRegisterSet)[RC_void] = InvalidRegisters;
    (*TypeToRegisterSet)[RC_i1] = IntegerRegistersI8;
    (*TypeToRegisterSet)[RC_i8] = IntegerRegistersI8;
    (*TypeToRegisterSet)[RC_i16] = IntegerRegistersI16;
    (*TypeToRegisterSet)[RC_i32] = IntegerRegistersI32;
    (*TypeToRegisterSet)[RC_i64] = InvalidRegisters;
    (*TypeToRegisterSet)[RC_f32] = FloatRegisters;
    (*TypeToRegisterSet)[RC_f64] = FloatRegisters;
    (*TypeToRegisterSet)[RC_v4i1] = VectorRegisters;
    (*TypeToRegisterSet)[RC_v8i1] = VectorRegisters;
    (*TypeToRegisterSet)[RC_v16i1] = VectorRegisters;
    (*TypeToRegisterSet)[RC_v16i8] = VectorRegisters;
    (*TypeToRegisterSet)[RC_v8i16] = VectorRegisters;
    (*TypeToRegisterSet)[RC_v4i32] = VectorRegisters;
    (*TypeToRegisterSet)[RC_v4f32] = VectorRegisters;
    (*TypeToRegisterSet)[RCX86_Is64To8] = Trunc64To8Registers;
    (*TypeToRegisterSet)[RCX86_Is32To8] = Trunc32To8Registers;
    (*TypeToRegisterSet)[RCX86_Is16To8] = Trunc16To8Registers;
    (*TypeToRegisterSet)[RCX86_IsTrunc8Rcvr] = Trunc8RcvrRegisters;
    (*TypeToRegisterSet)[RCX86_IsAhRcvr] = AhRcvrRegisters;
  }

  static SmallBitVector getRegisterSet(const ::Ice::ClFlags & /*Flags*/,
                                       TargetLowering::RegSetMask Include,
                                       TargetLowering::RegSetMask Exclude) {
    SmallBitVector Registers(RegisterSet::Reg_NUM);

#define X(val, encode, name, base, scratch, preserved, stackptr, frameptr,     \
          isGPR, is64, is32, is16, is8, isXmm, is64To8, is32To8, is16To8,      \
          isTrunc8Rcvr, isAhRcvr, aliases)                                     \
  if (scratch && (Include & ::Ice::TargetLowering::RegSet_CallerSave))         \
    Registers[RegisterSet::val] = true;                                        \
  if (preserved && (Include & ::Ice::TargetLowering::RegSet_CalleeSave))       \
    Registers[RegisterSet::val] = true;                                        \
  if (stackptr && (Include & ::Ice::TargetLowering::RegSet_StackPointer))      \
    Registers[RegisterSet::val] = true;                                        \
  if (frameptr && (Include & ::Ice::TargetLowering::RegSet_FramePointer))      \
    Registers[RegisterSet::val] = true;                                        \
  if (scratch && (Exclude & ::Ice::TargetLowering::RegSet_CallerSave))         \
    Registers[RegisterSet::val] = false;                                       \
  if (preserved && (Exclude & ::Ice::TargetLowering::RegSet_CalleeSave))       \
    Registers[RegisterSet::val] = false;                                       \
  if (stackptr && (Exclude & ::Ice::TargetLowering::RegSet_StackPointer))      \
    Registers[RegisterSet::val] = false;                                       \
  if (frameptr && (Exclude & ::Ice::TargetLowering::RegSet_FramePointer))      \
    Registers[RegisterSet::val] = false;

    REGX8632_TABLE

#undef X

    return Registers;
  }

  static RegNumT getRaxOrDie() {
    llvm::report_fatal_error("no rax in non-64-bit mode.");
  }

  static RegNumT getRdxOrDie() {
    llvm::report_fatal_error("no rdx in non-64-bit mode.");
  }

  // x86-32 calling convention:
  //
  // * The first four arguments of vector type, regardless of their position
  // relative to the other arguments in the argument list, are placed in
  // registers xmm0 - xmm3.
  //
  // This intends to match the section "IA-32 Function Calling Convention" of
  // the document "OS X ABI Function Call Guide" by Apple.

  /// The maximum number of arguments to pass in XMM registers
  static constexpr uint32_t X86_MAX_XMM_ARGS = 4;
  /// The maximum number of arguments to pass in GPR registers
  static constexpr uint32_t X86_MAX_GPR_ARGS = 0;
  /// Whether scalar floating point arguments are passed in XMM registers
  static constexpr bool X86_PASS_SCALAR_FP_IN_XMM = false;
  /// Get the register for a given argument slot in the XMM registers.
  static RegNumT getRegisterForXmmArgNum(uint32_t ArgNum) {
    // TODO(sehr): Change to use the CCArg technique used in ARM32.
    static_assert(RegisterSet::Reg_xmm0 + 1 == RegisterSet::Reg_xmm1,
                  "Inconsistency between XMM register numbers and ordinals");
    if (ArgNum >= X86_MAX_XMM_ARGS) {
      return RegNumT();
    }
    return RegNumT::fixme(RegisterSet::Reg_xmm0 + ArgNum);
  }
  /// Get the register for a given argument slot in the GPRs.
  static RegNumT getRegisterForGprArgNum(Type Ty, uint32_t ArgNum) {
    assert(Ty == IceType_i64 || Ty == IceType_i32);
    (void)Ty;
    (void)ArgNum;
    return RegNumT();
  }
  // Given the absolute argument position and argument position by type, return
  // the register index to assign it to.
  static SizeT getArgIndex(SizeT argPos, SizeT argPosByType) {
    (void)argPos;
    return argPosByType;
  };

  /// The number of bits in a byte
  static constexpr uint32_t X86_CHAR_BIT = 8;
  /// Stack alignment. This is defined in IceTargetLoweringX8632.cpp because it
  /// is used as an argument to std::max(), and the default std::less<T> has an
  /// operator(T const&, T const&) which requires this member to have an
  /// address.
  static const uint32_t X86_STACK_ALIGNMENT_BYTES;
  /// Size of the return address on the stack
  static constexpr uint32_t X86_RET_IP_SIZE_BYTES = 4;
  /// The number of different NOP instructions
  static constexpr uint32_t X86_NUM_NOP_VARIANTS = 5;

  /// \name Limits for unrolling memory intrinsics.
  /// @{
  static constexpr uint32_t MEMCPY_UNROLL_LIMIT = 8;
  static constexpr uint32_t MEMMOVE_UNROLL_LIMIT = 8;
  static constexpr uint32_t MEMSET_UNROLL_LIMIT = 8;
  /// @}

  /// Value is in bytes. Return Value adjusted to the next highest multiple of
  /// the stack alignment.
  static uint32_t applyStackAlignment(uint32_t Value) {
    return Utils::applyAlignment(Value, X86_STACK_ALIGNMENT_BYTES);
  }

  /// Return the type which the elements of the vector have in the X86
  /// representation of the vector.
  static Type getInVectorElementType(Type Ty) {
    assert(isVectorType(Ty));
    assert(static_cast<size_t>(Ty) < TableTypeX8632AttributesSize);
    return TableTypeX8632Attributes[Ty].InVectorElementType;
  }

  // Note: The following data structures are defined in
  // IceTargetLoweringX8632.cpp.

  /// The following table summarizes the logic for lowering the fcmp
  /// instruction. There is one table entry for each of the 16 conditions.
  ///
  /// The first four columns describe the case when the operands are floating
  /// point scalar values. A comment in lowerFcmp() describes the lowering
  /// template. In the most general case, there is a compare followed by two
  /// conditional branches, because some fcmp conditions don't map to a single
  /// x86 conditional branch. However, in many cases it is possible to swap the
  /// operands in the comparison and have a single conditional branch. Since
  /// it's quite tedious to validate the table by hand, good execution tests are
  /// helpful.
  ///
  /// The last two columns describe the case when the operands are vectors of
  /// floating point values. For most fcmp conditions, there is a clear mapping
  /// to a single x86 cmpps instruction variant. Some fcmp conditions require
  /// special code to handle and these are marked in the table with a
  /// Cmpps_Invalid predicate.
  /// {@
  static const struct TableFcmpType {
    uint32_t Default;
    bool SwapScalarOperands;
    CondX86::BrCond C1, C2;
    bool SwapVectorOperands;
    CondX86::CmppsCond Predicate;
  } TableFcmp[];
  static const size_t TableFcmpSize;
  /// @}

  /// The following table summarizes the logic for lowering the icmp instruction
  /// for i32 and narrower types. Each icmp condition has a clear mapping to an
  /// x86 conditional branch instruction.
  /// {@
  static const struct TableIcmp32Type {
    CondX86::BrCond Mapping;
  } TableIcmp32[];
  static const size_t TableIcmp32Size;
  /// @}

  /// The following table summarizes the logic for lowering the icmp instruction
  /// for the i64 type. For Eq and Ne, two separate 32-bit comparisons and
  /// conditional branches are needed. For the other conditions, three separate
  /// conditional branches are needed.
  /// {@
  static const struct TableIcmp64Type {
    CondX86::BrCond C1, C2, C3;
  } TableIcmp64[];
  static const size_t TableIcmp64Size;
  /// @}

  static CondX86::BrCond getIcmp32Mapping(InstIcmp::ICond Cond) {
    assert(static_cast<size_t>(Cond) < TableIcmp32Size);
    return TableIcmp32[Cond].Mapping;
  }

  static const struct TableTypeX8632AttributesType {
    Type InVectorElementType;
  } TableTypeX8632Attributes[];
  static const size_t TableTypeX8632AttributesSize;
};

} // end of namespace X8632
} // end of namespace Ice

#endif // SUBZERO_SRC_ICETARGETLOWERINGX8632TRAITS_H
