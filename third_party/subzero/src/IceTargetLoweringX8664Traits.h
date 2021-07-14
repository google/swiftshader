//===- subzero/src/IceTargetLoweringX8664Traits.h - x86-64 traits -*- C++ -*-=//
//
//                        The Subzero Code Generator
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief Declares the X8664 Target Lowering Traits.
///
//===----------------------------------------------------------------------===//

#ifndef SUBZERO_SRC_ICETARGETLOWERINGX8664TRAITS_H
#define SUBZERO_SRC_ICETARGETLOWERINGX8664TRAITS_H

#include "IceAssembler.h"
#include "IceConditionCodesX86.h"
#include "IceDefs.h"
#include "IceInst.h"
#include "IceOperand.h"
#include "IceRegistersX8664.h"
#include "IceTargetLowering.h"
#include "IceTargetLoweringX8664.def"
#include "IceTargetLoweringX86RegClass.h"

#include <array>
#include <initializer_list>

namespace Ice {
namespace X8664 {
using namespace ::Ice::X86;

class AssemblerX8664;
struct Insts;
class TargetX8664;
class TargetX8664;

struct TargetX8664Traits {
  using RegisterSet = ::Ice::RegX8664;
  using GPRRegister = RegisterSet::GPRRegister;
  using ByteRegister = RegisterSet::ByteRegister;
  using XmmRegister = RegisterSet::XmmRegister;

  //----------------------------------------------------------------------------
  //     __      ______  __     __  ______  ______  __  __   __  ______
  //    /\ \    /\  __ \/\ \  _ \ \/\  ___\/\  == \/\ \/\ "-.\ \/\  ___\
  //    \ \ \___\ \ \/\ \ \ \/ ".\ \ \  __\\ \  __<\ \ \ \ \-.  \ \ \__ \
  //     \ \_____\ \_____\ \__/".~\_\ \_____\ \_\ \_\ \_\ \_\\"\_\ \_____\
  //      \/_____/\/_____/\/_/   \/_/\/_____/\/_/ /_/\/_/\/_/ \/_/\/_____/
  //
  //----------------------------------------------------------------------------
  static const char *TargetName;
  static constexpr Type WordType = IceType_i64;

  static const char *getRegName(RegNumT RegNum) {
    static const char *const RegNames[RegisterSet::Reg_NUM] = {
#define X(val, encode, name, base, scratch, preserved, stackptr, frameptr,     \
          sboxres, isGPR, is64, is32, is16, is8, isXmm, is64To8, is32To8,      \
          is16To8, isTrunc8Rcvr, isAhRcvr, aliases)                            \
  name,
        REGX8664_TABLE
#undef X
    };
    RegNum.assertIsValid();
    return RegNames[RegNum];
  }

  static GPRRegister getEncodedGPR(RegNumT RegNum) {
    static const GPRRegister GPRRegs[RegisterSet::Reg_NUM] = {
#define X(val, encode, name, base, scratch, preserved, stackptr, frameptr,     \
          sboxres, isGPR, is64, is32, is16, is8, isXmm, is64To8, is32To8,      \
          is16To8, isTrunc8Rcvr, isAhRcvr, aliases)                            \
  GPRRegister(isGPR ? encode : GPRRegister::Encoded_Not_GPR),
        REGX8664_TABLE
#undef X
    };
    RegNum.assertIsValid();
    assert(GPRRegs[RegNum] != GPRRegister::Encoded_Not_GPR);
    return GPRRegs[RegNum];
  }

  static ByteRegister getEncodedByteReg(RegNumT RegNum) {
    static const ByteRegister ByteRegs[RegisterSet::Reg_NUM] = {
#define X(val, encode, name, base, scratch, preserved, stackptr, frameptr,     \
          sboxres, isGPR, is64, is32, is16, is8, isXmm, is64To8, is32To8,      \
          is16To8, isTrunc8Rcvr, isAhRcvr, aliases)                            \
  ByteRegister(is8 ? encode : ByteRegister::Encoded_Not_ByteReg),
        REGX8664_TABLE
#undef X
    };
    RegNum.assertIsValid();
    assert(ByteRegs[RegNum] != ByteRegister::Encoded_Not_ByteReg);
    return ByteRegs[RegNum];
  }

  static bool isXmm(RegNumT RegNum) {
    static const bool IsXmm[RegisterSet::Reg_NUM] = {
#define X(val, encode, name, base, scratch, preserved, stackptr, frameptr,     \
          sboxres, isGPR, is64, is32, is16, is8, isXmm, is64To8, is32To8,      \
          is16To8, isTrunc8Rcvr, isAhRcvr, aliases)                            \
  isXmm,
        REGX8664_TABLE
#undef X
    };
    return IsXmm[RegNum];
  }

  static XmmRegister getEncodedXmm(RegNumT RegNum) {
    static const XmmRegister XmmRegs[RegisterSet::Reg_NUM] = {
#define X(val, encode, name, base, scratch, preserved, stackptr, frameptr,     \
          sboxres, isGPR, is64, is32, is16, is8, isXmm, is64To8, is32To8,      \
          is16To8, isTrunc8Rcvr, isAhRcvr, aliases)                            \
  XmmRegister(isXmm ? encode : XmmRegister::Encoded_Not_Xmm),
        REGX8664_TABLE
#undef X
    };
    RegNum.assertIsValid();
    assert(XmmRegs[RegNum] != XmmRegister::Encoded_Not_Xmm);
    return XmmRegs[RegNum];
  }

  static uint32_t getEncoding(RegNumT RegNum) {
    static const uint32_t Encoding[RegisterSet::Reg_NUM] = {
#define X(val, encode, name, base, scratch, preserved, stackptr, frameptr,     \
          sboxres, isGPR, is64, is32, is16, is8, isXmm, is64To8, is32To8,      \
          is16To8, isTrunc8Rcvr, isAhRcvr, aliases)                            \
  encode,
        REGX8664_TABLE
#undef X
    };
    RegNum.assertIsValid();
    return Encoding[RegNum];
  }

  static inline RegNumT getBaseReg(RegNumT RegNum) {
    static const RegNumT BaseRegs[RegisterSet::Reg_NUM] = {
#define X(val, encode, name, base, scratch, preserved, stackptr, frameptr,     \
          sboxres, isGPR, is64, is32, is16, is8, isXmm, is64To8, is32To8,      \
          is16To8, isTrunc8Rcvr, isAhRcvr, aliases)                            \
  RegisterSet::base,
        REGX8664_TABLE
#undef X
    };
    RegNum.assertIsValid();
    return BaseRegs[RegNum];
  }

private:
  static RegNumT getFirstGprForType(Type Ty) {
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
    case IceType_i64:
      return RegisterSet::Reg_rax;
    }
  }

public:
  static RegNumT getGprForType(Type Ty, RegNumT RegNum) {
    assert(RegNum.hasValue());

    if (!isScalarIntegerType(Ty)) {
      return RegNum;
    }

    assert(Ty == IceType_i1 || Ty == IceType_i8 || Ty == IceType_i16 ||
           Ty == IceType_i32 || Ty == IceType_i64);

    if (RegNum == RegisterSet::Reg_ah) {
      assert(Ty == IceType_i8);
      return RegNum;
    }

    assert(RegNum != RegisterSet::Reg_bh);
    assert(RegNum != RegisterSet::Reg_ch);
    assert(RegNum != RegisterSet::Reg_dh);

    const RegNumT FirstGprForType = getFirstGprForType(Ty);

    switch (RegNum) {
    default:
      llvm::report_fatal_error("Unknown register.");
#define X(val, encode, name, base, scratch, preserved, stackptr, frameptr,     \
          sboxres, isGPR, is64, is32, is16, is8, isXmm, is64To8, is32To8,      \
          is16To8, isTrunc8Rcvr, isAhRcvr, aliases)                            \
  case RegisterSet::val: {                                                     \
    if (!isGPR)                                                                \
      return RegisterSet::val;                                                 \
    assert((is64) || (is32) || (is16) || (is8) ||                              \
           getBaseReg(RegisterSet::val) == RegisterSet::Reg_rsp);              \
    constexpr RegisterSet::AllRegisters FirstGprWithRegNumSize =               \
        ((is64) || RegisterSet::val == RegisterSet::Reg_rsp)                   \
            ? RegisterSet::Reg_rax                                             \
            : (((is32) || RegisterSet::val == RegisterSet::Reg_esp)            \
                   ? RegisterSet::Reg_eax                                      \
                   : (((is16) || RegisterSet::val == RegisterSet::Reg_sp)      \
                          ? RegisterSet::Reg_ax                                \
                          : RegisterSet::Reg_al));                             \
    const auto NewRegNum =                                                     \
        RegNumT::fixme(RegNum - FirstGprWithRegNumSize + FirstGprForType);     \
    assert(getBaseReg(RegNum) == getBaseReg(NewRegNum) &&                      \
           "Error involving " #val);                                           \
    return NewRegNum;                                                          \
  }
      REGX8664_TABLE
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
    explicit constexpr SizeOf(T...) : Size(length<T...>::value) {}
    constexpr SizeT size() const { return Size; }

  private:
    template <typename T, typename... U> struct length {
      static constexpr std::size_t value = 1 + length<U...>::value;
    };

    template <typename T> struct length<T> {
      static constexpr std::size_t value = 1;
    };

    const std::size_t Size;
  };

public:
  static void initRegisterSet(
      const ::Ice::ClFlags &Flags,
      std::array<SmallBitVector, RCX86_NUM> *TypeToRegisterSet,
      std::array<SmallBitVector, RegisterSet::Reg_NUM> *RegisterAliases) {
    SmallBitVector IntegerRegistersI64(RegisterSet::Reg_NUM);
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
      unsigned IsReservedWhenSandboxing : 1;
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
    } X8664RegTable[RegisterSet::Reg_NUM] = {
#define X(val, encode, name, base, scratch, preserved, stackptr, frameptr,     \
          sboxres, isGPR, is64, is32, is16, is8, isXmm, is64To8, is32To8,      \
          is16To8, isTrunc8Rcvr, isAhRcvr, aliases)                            \
  {                                                                            \
      RegisterSet::val,                                                        \
      sboxres,                                                                 \
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
        REGX8664_TABLE
#undef X
    };

    for (SizeT ii = 0; ii < llvm::array_lengthof(X8664RegTable); ++ii) {
      const auto &Entry = X8664RegTable[ii];
      // Even though the register is disabled for register allocation, it might
      // still be used by the Target Lowering (e.g., base pointer), so the
      // register alias table still needs to be defined.
      (*RegisterAliases)[Entry.Val].resize(RegisterSet::Reg_NUM);
      for (Ice::SizeT J = 0; J < Entry.NumAliases; ++J) {
        SizeT Alias = Entry.Aliases[J];
        assert(!(*RegisterAliases)[Entry.Val][Alias] && "Duplicate alias");
        (*RegisterAliases)[Entry.Val].set(Alias);
      }

      (*RegisterAliases)[Entry.Val].set(Entry.Val);

      (IntegerRegistersI64)[Entry.Val] = Entry.Is64;
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
    }

    (*TypeToRegisterSet)[RC_void] = InvalidRegisters;
    (*TypeToRegisterSet)[RC_i1] = IntegerRegistersI8;
    (*TypeToRegisterSet)[RC_i8] = IntegerRegistersI8;
    (*TypeToRegisterSet)[RC_i16] = IntegerRegistersI16;
    (*TypeToRegisterSet)[RC_i32] = IntegerRegistersI32;
    (*TypeToRegisterSet)[RC_i64] = IntegerRegistersI64;
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

  static SmallBitVector getRegisterSet(const ::Ice::ClFlags &Flags,
                                       TargetLowering::RegSetMask Include,
                                       TargetLowering::RegSetMask Exclude) {
    SmallBitVector Registers(RegisterSet::Reg_NUM);

#define X(val, encode, name, base, scratch, preserved, stackptr, frameptr,     \
          sboxres, isGPR, is64, is32, is16, is8, isXmm, is64To8, is32To8,      \
          is16To8, isTrunc8Rcvr, isAhRcvr, aliases)                            \
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

    REGX8664_TABLE

#undef X

    return Registers;
  }

  static RegNumT getRaxOrDie() { return RegisterSet::Reg_rax; }

  static RegNumT getRdxOrDie() { return RegisterSet::Reg_rdx; }

#if defined(_WIN64)
  // Microsoft x86-64 calling convention:
  //
  // * The first four arguments of vector/fp type, regardless of their
  // position relative to the other arguments in the argument list, are placed
  // in registers %xmm0 - %xmm3.
  //
  // * The first four arguments of integer types, regardless of their position
  // relative to the other arguments in the argument list, are placed in
  // registers %rcx, %rdx, %r8, and %r9.

  /// The maximum number of arguments to pass in XMM registers
  static constexpr uint32_t X86_MAX_XMM_ARGS = 4;
  /// The maximum number of arguments to pass in GPR registers
  static constexpr uint32_t X86_MAX_GPR_ARGS = 4;
  static RegNumT getRegisterForGprArgNum(Type Ty, uint32_t ArgNum) {
    if (ArgNum >= X86_MAX_GPR_ARGS) {
      return RegNumT();
    }
    static const RegisterSet::AllRegisters GprForArgNum[] = {
        RegisterSet::Reg_rcx,
        RegisterSet::Reg_rdx,
        RegisterSet::Reg_r8,
        RegisterSet::Reg_r9,
    };
    static_assert(llvm::array_lengthof(GprForArgNum) == X86_MAX_GPR_ARGS,
                  "Mismatch between MAX_GPR_ARGS and GprForArgNum.");
    assert(Ty == IceType_i64 || Ty == IceType_i32);
    return getGprForType(Ty, GprForArgNum[ArgNum]);
  }
  // Given the absolute argument position and argument position by type, return
  // the register index to assign it to.
  static SizeT getArgIndex(SizeT argPos, SizeT argPosByType) {
    // Microsoft x64 ABI: register is selected by arg position (e.g. 1st int as
    // 2nd param goes into 2nd int reg)
    (void)argPosByType;
    return argPos;
  };

#else
  // System V x86-64 calling convention:
  //
  // * The first eight arguments of vector/fp type, regardless of their
  // position relative to the other arguments in the argument list, are placed
  // in registers %xmm0 - %xmm7.
  //
  // * The first six arguments of integer types, regardless of their position
  // relative to the other arguments in the argument list, are placed in
  // registers %rdi, %rsi, %rdx, %rcx, %r8, and %r9.
  //
  // This intends to match the section "Function Calling Sequence" of the
  // document "System V Application Binary Interface."

  /// The maximum number of arguments to pass in XMM registers
  static constexpr uint32_t X86_MAX_XMM_ARGS = 8;
  /// The maximum number of arguments to pass in GPR registers
  static constexpr uint32_t X86_MAX_GPR_ARGS = 6;
  /// Get the register for a given argument slot in the GPRs.
  static RegNumT getRegisterForGprArgNum(Type Ty, uint32_t ArgNum) {
    if (ArgNum >= X86_MAX_GPR_ARGS) {
      return RegNumT();
    }
    static const RegisterSet::AllRegisters GprForArgNum[] = {
        RegisterSet::Reg_rdi, RegisterSet::Reg_rsi, RegisterSet::Reg_rdx,
        RegisterSet::Reg_rcx, RegisterSet::Reg_r8,  RegisterSet::Reg_r9,
    };
    static_assert(llvm::array_lengthof(GprForArgNum) == X86_MAX_GPR_ARGS,
                  "Mismatch between MAX_GPR_ARGS and GprForArgNum.");
    assert(Ty == IceType_i64 || Ty == IceType_i32);
    return getGprForType(Ty, GprForArgNum[ArgNum]);
  }
  // Given the absolute argument position and argument position by type, return
  // the register index to assign it to.
  static SizeT getArgIndex(SizeT argPos, SizeT argPosByType) {
    (void)argPos;
    return argPosByType;
  }
#endif

  /// Whether scalar floating point arguments are passed in XMM registers
  static constexpr bool X86_PASS_SCALAR_FP_IN_XMM = true;
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

  /// The number of bits in a byte
  static constexpr uint32_t X86_CHAR_BIT = 8;
  /// Stack alignment. This is defined in IceTargetLoweringX8664.cpp because it
  /// is used as an argument to std::max(), and the default std::less<T> has an
  /// operator(T const&, T const&) which requires this member to have an
  /// address.
  static const uint32_t X86_STACK_ALIGNMENT_BYTES;
  /// Size of the return address on the stack
  static constexpr uint32_t X86_RET_IP_SIZE_BYTES = 8;
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
    assert(Ty < TableTypeX8664AttributesSize);
    return TableTypeX8664Attributes[Ty].InVectorElementType;
  }

  // Note: The following data structures are defined in
  // IceTargetLoweringX8664.cpp.

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
    assert(Cond < TableIcmp32Size);
    return TableIcmp32[Cond].Mapping;
  }

  static const struct TableTypeX8664AttributesType {
    Type InVectorElementType;
  } TableTypeX8664Attributes[];
  static const size_t TableTypeX8664AttributesSize;
};

} // end of namespace X8664
} // end of namespace Ice

#endif // SUBZERO_SRC_ICETARGETLOWERINGX8664TRAITS_H
