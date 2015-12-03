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
#include "IceConditionCodesX8664.h"
#include "IceDefs.h"
#include "IceInst.h"
#include "IceInstX8664.def"
#include "IceOperand.h"
#include "IceRegistersX8664.h"
#include "IceTargetLowering.h"
#include "IceTargetLoweringX8664.def"
#include "IceTargetLoweringX86RegClass.h"

#include <array>

namespace Ice {

class TargetX8664;

namespace X8664 {
class AssemblerX8664;
} // end of namespace X8664

namespace X86Internal {

template <class Machine> struct Insts;
template <class Machine> struct MachineTraits;
template <class Machine> class TargetX86Base;

template <> struct MachineTraits<TargetX8664> {
  //----------------------------------------------------------------------------
  //     ______  ______  __    __
  //    /\  __ \/\  ___\/\ "-./  \
  //    \ \  __ \ \___  \ \ \-./\ \
  //     \ \_\ \_\/\_____\ \_\ \ \_\
  //      \/_/\/_/\/_____/\/_/  \/_/
  //
  //----------------------------------------------------------------------------
  static constexpr bool Is64Bit = true;
  static constexpr bool HasPopa = false;
  static constexpr bool HasPusha = false;
  static constexpr bool UsesX87 = false;
  static constexpr ::Ice::RegX8664::GPRRegister Last8BitGPR =
      ::Ice::RegX8664::GPRRegister::Encoded_Reg_r15d;

  enum ScaleFactor { TIMES_1 = 0, TIMES_2 = 1, TIMES_4 = 2, TIMES_8 = 3 };

  using GPRRegister = ::Ice::RegX8664::GPRRegister;
  using ByteRegister = ::Ice::RegX8664::ByteRegister;
  using XmmRegister = ::Ice::RegX8664::XmmRegister;

  using Cond = ::Ice::CondX8664;

  using RegisterSet = ::Ice::RegX8664;
  static const GPRRegister Encoded_Reg_Accumulator = RegX8664::Encoded_Reg_eax;
  static const GPRRegister Encoded_Reg_Counter = RegX8664::Encoded_Reg_ecx;
  static const FixupKind PcRelFixup = llvm::ELF::R_X86_64_PC32;
  static const FixupKind RelFixup = llvm::ELF::R_X86_64_32S;

  class Operand {
  public:
    enum RexBits {
      RexNone = 0x00,
      RexBase = 0x40,
      RexW = RexBase | (1 << 3),
      RexR = RexBase | (1 << 2),
      RexX = RexBase | (1 << 1),
      RexB = RexBase | (1 << 0),
    };

    Operand(const Operand &other)
        : fixup_(other.fixup_), rex_(other.rex_), length_(other.length_) {
      memmove(&encoding_[0], &other.encoding_[0], other.length_);
    }

    Operand &operator=(const Operand &other) {
      length_ = other.length_;
      fixup_ = other.fixup_;
      rex_ = other.rex_;
      memmove(&encoding_[0], &other.encoding_[0], other.length_);
      return *this;
    }

    uint8_t mod() const { return (encoding_at(0) >> 6) & 3; }

    uint8_t rexX() const { return (rex_ & RexX) != RexX ? RexNone : RexX; }
    uint8_t rexB() const { return (rex_ & RexB) != RexB ? RexNone : RexB; }

    GPRRegister rm() const {
      return static_cast<GPRRegister>((rexB() != 0 ? 0x08 : 0) |
                                      (encoding_at(0) & 7));
    }

    ScaleFactor scale() const {
      return static_cast<ScaleFactor>((encoding_at(1) >> 6) & 3);
    }

    GPRRegister index() const {
      return static_cast<GPRRegister>((rexX() != 0 ? 0x08 : 0) |
                                      ((encoding_at(1) >> 3) & 7));
    }

    GPRRegister base() const {
      return static_cast<GPRRegister>((rexB() != 0 ? 0x08 : 0) |
                                      (encoding_at(1) & 7));
    }

    int8_t disp8() const {
      assert(length_ >= 2);
      return static_cast<int8_t>(encoding_[length_ - 1]);
    }

    int32_t disp32() const {
      assert(length_ >= 5);
      return bit_copy<int32_t>(encoding_[length_ - 4]);
    }

    AssemblerFixup *fixup() const { return fixup_; }

  protected:
    Operand() : fixup_(nullptr), length_(0) {} // Needed by subclass Address.

    void SetModRM(int mod, GPRRegister rm) {
      assert((mod & ~3) == 0);
      encoding_[0] = (mod << 6) | (rm & 0x07);
      rex_ = (rm & 0x08) ? RexB : RexNone;
      length_ = 1;
    }

    void SetSIB(ScaleFactor scale, GPRRegister index, GPRRegister base) {
      assert(length_ == 1);
      assert((scale & ~3) == 0);
      encoding_[1] = (scale << 6) | ((index & 0x07) << 3) | (base & 0x07);
      rex_ =
          ((base & 0x08) ? RexB : RexNone) | ((index & 0x08) ? RexX : RexNone);
      length_ = 2;
    }

    void SetDisp8(int8_t disp) {
      assert(length_ == 1 || length_ == 2);
      encoding_[length_++] = static_cast<uint8_t>(disp);
    }

    void SetDisp32(int32_t disp) {
      assert(length_ == 1 || length_ == 2);
      intptr_t disp_size = sizeof(disp);
      memmove(&encoding_[length_], &disp, disp_size);
      length_ += disp_size;
    }

    void SetFixup(AssemblerFixup *fixup) { fixup_ = fixup; }

  private:
    AssemblerFixup *fixup_;
    uint8_t rex_ = 0;
    uint8_t encoding_[6];
    uint8_t length_;

    explicit Operand(GPRRegister reg) : fixup_(nullptr) { SetModRM(3, reg); }

    /// Get the operand encoding byte at the given index.
    uint8_t encoding_at(intptr_t index) const {
      assert(index >= 0 && index < length_);
      return encoding_[index];
    }

    /// Returns whether or not this operand is really the given register in
    /// disguise. Used from the assembler to generate better encodings.
    bool IsRegister(GPRRegister reg) const {
      return ((encoding_[0] & 0xF8) ==
              0xC0) // Addressing mode is register only.
             &&
             (rm() == reg); // Register codes match.
    }

    template <class> friend class AssemblerX86Base;
  };

  class Address : public Operand {
    Address() = delete;

  public:
    Address(const Address &other) : Operand(other) {}

    Address &operator=(const Address &other) {
      Operand::operator=(other);
      return *this;
    }

    Address(GPRRegister Base, int32_t Disp, AssemblerFixup *Fixup) {
      if (Fixup == nullptr && Disp == 0 &&
          (Base & 7) != RegX8664::Encoded_Reg_ebp) {
        SetModRM(0, Base);
        if ((Base & 7) == RegX8664::Encoded_Reg_esp)
          SetSIB(TIMES_1, RegX8664::Encoded_Reg_esp, Base);
      } else if (Fixup == nullptr && Utils::IsInt(8, Disp)) {
        SetModRM(1, Base);
        if ((Base & 7) == RegX8664::Encoded_Reg_esp)
          SetSIB(TIMES_1, RegX8664::Encoded_Reg_esp, Base);
        SetDisp8(Disp);
      } else {
        SetModRM(2, Base);
        if ((Base & 7) == RegX8664::Encoded_Reg_esp)
          SetSIB(TIMES_1, RegX8664::Encoded_Reg_esp, Base);
        SetDisp32(Disp);
        if (Fixup)
          SetFixup(Fixup);
      }
    }

    Address(GPRRegister Index, ScaleFactor Scale, int32_t Disp,
            AssemblerFixup *Fixup) {
      assert(Index != RegX8664::Encoded_Reg_esp); // Illegal addressing mode.
      SetModRM(0, RegX8664::Encoded_Reg_esp);
      SetSIB(Scale, Index, RegX8664::Encoded_Reg_ebp);
      SetDisp32(Disp);
      if (Fixup)
        SetFixup(Fixup);
    }

    Address(GPRRegister Base, GPRRegister Index, ScaleFactor Scale,
            int32_t Disp, AssemblerFixup *Fixup) {
      assert(Index != RegX8664::Encoded_Reg_esp); // Illegal addressing mode.
      if (Fixup == nullptr && Disp == 0 &&
          (Base & 7) != RegX8664::Encoded_Reg_ebp) {
        SetModRM(0, RegX8664::Encoded_Reg_esp);
        SetSIB(Scale, Index, Base);
      } else if (Fixup == nullptr && Utils::IsInt(8, Disp)) {
        SetModRM(1, RegX8664::Encoded_Reg_esp);
        SetSIB(Scale, Index, Base);
        SetDisp8(Disp);
      } else {
        SetModRM(2, RegX8664::Encoded_Reg_esp);
        SetSIB(Scale, Index, Base);
        SetDisp32(Disp);
        if (Fixup)
          SetFixup(Fixup);
      }
    }

    /// Generate a RIP-relative address expression on x86-64.
    Address(RelocOffsetT Offset, AssemblerFixup *Fixup) {
      SetModRM(0, RegX8664::Encoded_Reg_ebp);
      // Use the Offset in the displacement for now. If we decide to process
      // fixups later, we'll need to patch up the emitted displacement.
      SetDisp32(Offset);
      if (Fixup)
        SetFixup(Fixup);
    }

    static Address ofConstPool(Assembler *Asm, const Constant *Imm) {
      // TODO(jpp): ???
      AssemblerFixup *Fixup = Asm->createFixup(RelFixup, Imm);
      const RelocOffsetT Offset = 4;
      return Address(Offset, Fixup);
    }
  };

  //----------------------------------------------------------------------------
  //     __      ______  __     __  ______  ______  __  __   __  ______
  //    /\ \    /\  __ \/\ \  _ \ \/\  ___\/\  == \/\ \/\ "-.\ \/\  ___\
  //    \ \ \___\ \ \/\ \ \ \/ ".\ \ \  __\\ \  __<\ \ \ \ \-.  \ \ \__ \
  //     \ \_____\ \_____\ \__/".~\_\ \_____\ \_\ \_\ \_\ \_\\"\_\ \_____\
  //      \/_____/\/_____/\/_/   \/_/\/_____/\/_/ /_/\/_/\/_/ \/_/\/_____/
  //
  //----------------------------------------------------------------------------
  enum InstructionSet {
    Begin,
    // SSE2 is the PNaCl baseline instruction set.
    SSE2 = Begin,
    SSE4_1,
    End
  };

  static const char *TargetName;
  static constexpr Type WordType = IceType_i64;

  static IceString getRegName(int32_t RegNum) {
    static const char *const RegNames[] = {
#define X(val, encode, name, base, scratch, preserved, stackptr, frameptr,     \
          isGPR, is64, is32, is16, is8, isXmm, is64To8, is32To8, is16To8,      \
          isTrunc8Rcvr, isAhRcvr, aliases)                                     \
  name,
        REGX8664_TABLE
#undef X
    };
    assert(RegNum >= 0);
    assert(RegNum < RegisterSet::Reg_NUM);
    return RegNames[RegNum];
  }

  static GPRRegister getEncodedGPR(int32_t RegNum) {
    static const GPRRegister GPRRegs[] = {
#define X(val, encode, name, base, scratch, preserved, stackptr, frameptr,     \
          isGPR, is64, is32, is16, is8, isXmm, is64To8, is32To8, is16To8,      \
          isTrunc8Rcvr, isAhRcvr, aliases)                                     \
  GPRRegister(isGPR ? encode : GPRRegister::Encoded_Not_GPR),
        REGX8664_TABLE
#undef X
    };
    assert(RegNum >= 0);
    assert(RegNum < RegisterSet::Reg_NUM);
    assert(GPRRegs[RegNum] != GPRRegister::Encoded_Not_GPR);
    return GPRRegs[RegNum];
  }

  static ByteRegister getEncodedByteReg(int32_t RegNum) {
    static const ByteRegister ByteRegs[] = {
#define X(val, encode, name, base, scratch, preserved, stackptr, frameptr,     \
          isGPR, is64, is32, is16, is8, isXmm, is64To8, is32To8, is16To8,      \
          isTrunc8Rcvr, isAhRcvr, aliases)                                     \
  ByteRegister(is8 ? encode : ByteRegister::Encoded_Not_ByteReg),
        REGX8664_TABLE
#undef X
    };
    assert(RegNum >= 0);
    assert(RegNum < RegisterSet::Reg_NUM);
    assert(ByteRegs[RegNum] != ByteRegister::Encoded_Not_ByteReg);
    return ByteRegs[RegNum];
  }

  static XmmRegister getEncodedXmm(int32_t RegNum) {
    static const XmmRegister XmmRegs[] = {
#define X(val, encode, name, base, scratch, preserved, stackptr, frameptr,     \
          isGPR, is64, is32, is16, is8, isXmm, is64To8, is32To8, is16To8,      \
          isTrunc8Rcvr, isAhRcvr, aliases)                                     \
  XmmRegister(isXmm ? encode : XmmRegister::Encoded_Not_Xmm),
        REGX8664_TABLE
#undef X
    };
    assert(RegNum >= 0);
    assert(RegNum < RegisterSet::Reg_NUM);
    assert(XmmRegs[RegNum] != XmmRegister::Encoded_Not_Xmm);
    return XmmRegs[RegNum];
  }

  static uint32_t getEncoding(int32_t RegNum) {
    static const uint32_t Encoding[] = {
#define X(val, encode, name, base, scratch, preserved, stackptr, frameptr,     \
          isGPR, is64, is32, is16, is8, isXmm, is64To8, is32To8, is16To8,      \
          isTrunc8Rcvr, isAhRcvr, aliases)                                     \
  encode,
        REGX8664_TABLE
#undef X
    };
    assert(RegNum >= 0);
    assert(RegNum < RegisterSet::Reg_NUM);
    return Encoding[RegNum];
  }

  static inline int32_t getBaseReg(int32_t RegNum) {
    static const int32_t BaseRegs[] = {
#define X(val, encode, name, base, scratch, preserved, stackptr, frameptr,     \
          isGPR, is64, is32, is16, is8, isXmm, is64To8, is32To8, is16To8,      \
          isTrunc8Rcvr, isAhRcvr, aliases)                                     \
  RegisterSet::base,
        REGX8664_TABLE
#undef X
    };
    assert(RegNum >= 0);
    assert(RegNum < RegisterSet::Reg_NUM);
    return BaseRegs[RegNum];
  }

  static int32_t getGprForType(Type, int32_t RegNum) { return RegNum; }

  static void initRegisterSet(
      std::array<llvm::SmallBitVector, RCX86_NUM> *TypeToRegisterSet,
      std::array<llvm::SmallBitVector, RegisterSet::Reg_NUM> *RegisterAliases,
      llvm::SmallBitVector *ScratchRegs) {
    llvm::SmallBitVector IntegerRegistersI64(RegisterSet::Reg_NUM);
    llvm::SmallBitVector IntegerRegistersI32(RegisterSet::Reg_NUM);
    llvm::SmallBitVector IntegerRegistersI16(RegisterSet::Reg_NUM);
    llvm::SmallBitVector IntegerRegistersI8(RegisterSet::Reg_NUM);
    llvm::SmallBitVector FloatRegisters(RegisterSet::Reg_NUM);
    llvm::SmallBitVector VectorRegisters(RegisterSet::Reg_NUM);
    llvm::SmallBitVector Trunc64To8Registers(RegisterSet::Reg_NUM);
    llvm::SmallBitVector Trunc32To8Registers(RegisterSet::Reg_NUM);
    llvm::SmallBitVector Trunc16To8Registers(RegisterSet::Reg_NUM);
    llvm::SmallBitVector Trunc8RcvrRegisters(RegisterSet::Reg_NUM);
    llvm::SmallBitVector AhRcvrRegisters(RegisterSet::Reg_NUM);
    llvm::SmallBitVector InvalidRegisters(RegisterSet::Reg_NUM);
    ScratchRegs->resize(RegisterSet::Reg_NUM);

#define X(val, encode, name, base, scratch, preserved, stackptr, frameptr,     \
          isGPR, is64, is32, is16, is8, isXmm, is64To8, is32To8, is16To8,      \
          isTrunc8Rcvr, isAhRcvr, aliases)                                     \
  (IntegerRegistersI64)[RegisterSet::val] = is64;                              \
  (IntegerRegistersI32)[RegisterSet::val] = is32;                              \
  (IntegerRegistersI16)[RegisterSet::val] = is16;                              \
  (IntegerRegistersI8)[RegisterSet::val] = is8;                                \
  (FloatRegisters)[RegisterSet::val] = isXmm;                                  \
  (VectorRegisters)[RegisterSet::val] = isXmm;                                 \
  (Trunc64To8Registers)[RegisterSet::val] = is64To8;                           \
  (Trunc32To8Registers)[RegisterSet::val] = is32To8;                           \
  (Trunc16To8Registers)[RegisterSet::val] = is16To8;                           \
  (Trunc8RcvrRegisters)[RegisterSet::val] = isTrunc8Rcvr;                      \
  (AhRcvrRegisters)[RegisterSet::val] = isAhRcvr;                              \
  (*RegisterAliases)[RegisterSet::val].resize(RegisterSet::Reg_NUM);           \
  for (SizeT RegAlias : aliases) {                                             \
    assert(!(*RegisterAliases)[RegisterSet::val][RegAlias] &&                  \
           "Duplicate alias for " #val);                                       \
    (*RegisterAliases)[RegisterSet::val].set(RegAlias);                        \
  }                                                                            \
  (*RegisterAliases)[RegisterSet::val].set(RegisterSet::val);                  \
  (*ScratchRegs)[RegisterSet::val] = scratch;
    REGX8664_TABLE;
#undef X

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

  static llvm::SmallBitVector
  getRegisterSet(TargetLowering::RegSetMask Include,
                 TargetLowering::RegSetMask Exclude) {
    llvm::SmallBitVector Registers(RegisterSet::Reg_NUM);

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

    REGX8664_TABLE

#undef X

    return Registers;
  }

  static void
  makeRandomRegisterPermutation(GlobalContext *Ctx, Cfg *Func,
                                llvm::SmallVectorImpl<int32_t> &Permutation,
                                const llvm::SmallBitVector &ExcludeRegisters,
                                uint64_t Salt) {
    // TODO(stichnot): Declaring Permutation this way loses type/size
    // information. Fix this in conjunction with the caller-side TODO.
    assert(Permutation.size() >= RegisterSet::Reg_NUM);
    // Expected upper bound on the number of registers in a single equivalence
    // class.  For x86-64, this would comprise the 16 XMM registers. This is
    // for performance, not correctness.
    static const unsigned MaxEquivalenceClassSize = 8;
    using RegisterList = llvm::SmallVector<int32_t, MaxEquivalenceClassSize>;
    using EquivalenceClassMap = std::map<uint32_t, RegisterList>;
    EquivalenceClassMap EquivalenceClasses;
    SizeT NumShuffled = 0, NumPreserved = 0;

// Build up the equivalence classes of registers by looking at the register
// properties as well as whether the registers should be explicitly excluded
// from shuffling.
#define X(val, encode, name, base, scratch, preserved, stackptr, frameptr,     \
          isGPR, is64, is32, is16, is8, isXmm, is64To8, is32To8, is16To8,      \
          isTrunc8Rcvr, isAhRcvr, aliases)                                     \
  if (ExcludeRegisters[RegisterSet::val]) {                                    \
    /* val stays the same in the resulting permutation. */                     \
    Permutation[RegisterSet::val] = RegisterSet::val;                          \
    ++NumPreserved;                                                            \
  } else {                                                                     \
    uint32_t AttrKey = 0;                                                      \
    uint32_t Index = 0;                                                        \
    /* Combine relevant attributes into an equivalence class key. */           \
    Index |= (scratch << (AttrKey++));                                         \
    Index |= (preserved << (AttrKey++));                                       \
    Index |= (is8 << (AttrKey++));                                             \
    Index |= (is16 << (AttrKey++));                                            \
    Index |= (is32 << (AttrKey++));                                            \
    Index |= (is64 << (AttrKey++));                                            \
    Index |= (isXmm << (AttrKey++));                                           \
    Index |= (is16To8 << (AttrKey++));                                         \
    Index |= (is32To8 << (AttrKey++));                                         \
    Index |= (is64To8 << (AttrKey++));                                         \
    Index |= (isTrunc8Rcvr << (AttrKey++));                                    \
    /* val is assigned to an equivalence class based on its properties. */     \
    EquivalenceClasses[Index].push_back(RegisterSet::val);                     \
  }
    REGX8664_TABLE
#undef X

    // Create a random number generator for regalloc randomization.
    RandomNumberGenerator RNG(Ctx->getFlags().getRandomSeed(),
                              RPE_RegAllocRandomization, Salt);
    RandomNumberGeneratorWrapper RNGW(RNG);

    // Shuffle the resulting equivalence classes.
    for (auto I : EquivalenceClasses) {
      const RegisterList &List = I.second;
      RegisterList Shuffled(List);
      RandomShuffle(Shuffled.begin(), Shuffled.end(), RNGW);
      for (size_t SI = 0, SE = Shuffled.size(); SI < SE; ++SI) {
        Permutation[List[SI]] = Shuffled[SI];
        ++NumShuffled;
      }
    }

    assert(NumShuffled + NumPreserved == RegisterSet::Reg_NUM);

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
          Str << getRegName(Register);
        }
        Str << "}\n";
      }
    }
  }

  /// The maximum number of arguments to pass in XMM registers
  static const uint32_t X86_MAX_XMM_ARGS = 8;
  /// The maximum number of arguments to pass in GPR registers
  static const uint32_t X86_MAX_GPR_ARGS = 6;
  /// The number of bits in a byte
  static const uint32_t X86_CHAR_BIT = 8;
  /// Stack alignment. This is defined in IceTargetLoweringX8664.cpp because it
  /// is used as an argument to std::max(), and the default std::less<T> has an
  /// operator(T const&, T const&) which requires this member to have an
  /// address.
  static const uint32_t X86_STACK_ALIGNMENT_BYTES;
  /// Size of the return address on the stack
  static const uint32_t X86_RET_IP_SIZE_BYTES = 8;
  /// The number of different NOP instructions
  static const uint32_t X86_NUM_NOP_VARIANTS = 5;

  /// \name Limits for unrolling memory intrinsics.
  /// @{
  static constexpr uint32_t MEMCPY_UNROLL_LIMIT = 8;
  static constexpr uint32_t MEMMOVE_UNROLL_LIMIT = 8;
  static constexpr uint32_t MEMSET_UNROLL_LIMIT = 16;
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
    size_t Index = static_cast<size_t>(Ty);
    (void)Index;
    assert(Index < TableTypeX8664AttributesSize);
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
    Cond::BrCond C1, C2;
    bool SwapVectorOperands;
    Cond::CmppsCond Predicate;
  } TableFcmp[];
  static const size_t TableFcmpSize;
  /// @}

  /// The following table summarizes the logic for lowering the icmp instruction
  /// for i32 and narrower types. Each icmp condition has a clear mapping to an
  /// x86 conditional branch instruction.
  /// {@
  static const struct TableIcmp32Type { Cond::BrCond Mapping; } TableIcmp32[];
  static const size_t TableIcmp32Size;
  /// @}

  /// The following table summarizes the logic for lowering the icmp instruction
  /// for the i64 type. For Eq and Ne, two separate 32-bit comparisons and
  /// conditional branches are needed. For the other conditions, three separate
  /// conditional branches are needed.
  /// {@
  static const struct TableIcmp64Type {
    Cond::BrCond C1, C2, C3;
  } TableIcmp64[];
  static const size_t TableIcmp64Size;
  /// @}

  static Cond::BrCond getIcmp32Mapping(InstIcmp::ICond Cond) {
    size_t Index = static_cast<size_t>(Cond);
    assert(Index < TableIcmp32Size);
    return TableIcmp32[Index].Mapping;
  }

  static const struct TableTypeX8664AttributesType {
    Type InVectorElementType;
  } TableTypeX8664Attributes[];
  static const size_t TableTypeX8664AttributesSize;

  //----------------------------------------------------------------------------
  //      __  __   __  ______  ______
  //    /\ \/\ "-.\ \/\  ___\/\__  _\
  //    \ \ \ \ \-.  \ \___  \/_/\ \/
  //     \ \_\ \_\\"\_\/\_____\ \ \_\
  //      \/_/\/_/ \/_/\/_____/  \/_/
  //
  //----------------------------------------------------------------------------
  using Insts = ::Ice::X86Internal::Insts<TargetX8664>;

  using TargetLowering = ::Ice::X86Internal::TargetX86Base<TargetX8664>;
  using Assembler = X8664::AssemblerX8664;

  /// X86Operand extends the Operand hierarchy. Its subclasses are X86OperandMem
  /// and VariableSplit.
  class X86Operand : public ::Ice::Operand {
    X86Operand() = delete;
    X86Operand(const X86Operand &) = delete;
    X86Operand &operator=(const X86Operand &) = delete;

  public:
    enum OperandKindX8664 { k__Start = ::Ice::Operand::kTarget, kMem, kSplit };
    using ::Ice::Operand::dump;

    void dump(const Cfg *, Ostream &Str) const override;

  protected:
    X86Operand(OperandKindX8664 Kind, Type Ty)
        : Operand(static_cast<::Ice::Operand::OperandKind>(Kind), Ty) {}
  };

  /// X86OperandMem represents the m64 addressing mode, with optional base and
  /// index registers, a constant offset, and a fixed shift value for the index
  /// register.
  class X86OperandMem : public X86Operand {
    X86OperandMem() = delete;
    X86OperandMem(const X86OperandMem &) = delete;
    X86OperandMem &operator=(const X86OperandMem &) = delete;

  public:
    enum SegmentRegisters { DefaultSegment = -1, SegReg_NUM };
    static X86OperandMem *
    create(Cfg *Func, Type Ty, Variable *Base, Constant *Offset,
           Variable *Index = nullptr, uint16_t Shift = 0,
           SegmentRegisters SegmentRegister = DefaultSegment) {
      assert(SegmentRegister == DefaultSegment);
      (void)SegmentRegister;
      return new (Func->allocate<X86OperandMem>())
          X86OperandMem(Func, Ty, Base, Offset, Index, Shift);
    }
    Variable *getBase() const { return Base; }
    Constant *getOffset() const { return Offset; }
    Variable *getIndex() const { return Index; }
    uint16_t getShift() const { return Shift; }
    SegmentRegisters getSegmentRegister() const { return DefaultSegment; }
    void emitSegmentOverride(Assembler *) const {}
    Address toAsmAddress(Assembler *Asm,
                         const Ice::TargetLowering *Target) const;

    void emit(const Cfg *Func) const override;
    using X86Operand::dump;
    void dump(const Cfg *Func, Ostream &Str) const override;

    static bool classof(const Operand *Operand) {
      return Operand->getKind() == static_cast<OperandKind>(kMem);
    }

    void setRandomized(bool R) { Randomized = R; }

    bool getRandomized() const { return Randomized; }

  private:
    X86OperandMem(Cfg *Func, Type Ty, Variable *Base, Constant *Offset,
                  Variable *Index, uint16_t Shift);

    Variable *Base;
    Constant *Offset;
    Variable *Index;
    uint16_t Shift;
    /// A flag to show if this memory operand is a randomized one. Randomized
    /// memory operands are generated in
    /// TargetX86Base::randomizeOrPoolImmediate()
    bool Randomized = false;
  };

  /// VariableSplit is a way to treat an f64 memory location as a pair of i32
  /// locations (Low and High). This is needed for some cases of the Bitcast
  /// instruction. Since it's not possible for integer registers to access the
  /// XMM registers and vice versa, the lowering forces the f64 to be spilled to
  /// the stack and then accesses through the VariableSplit.
  // TODO(jpp): remove references to VariableSplit from IceInstX86Base as 64bit
  // targets can natively handle these.
  class VariableSplit : public X86Operand {
    VariableSplit() = delete;
    VariableSplit(const VariableSplit &) = delete;
    VariableSplit &operator=(const VariableSplit &) = delete;

  public:
    enum Portion { Low, High };
    static VariableSplit *create(Cfg *Func, Variable *Var, Portion Part) {
      return new (Func->allocate<VariableSplit>())
          VariableSplit(Func, Var, Part);
    }
    int32_t getOffset() const { return Part == High ? 4 : 0; }

    Address toAsmAddress(const Cfg *Func) const;
    void emit(const Cfg *Func) const override;
    using X86Operand::dump;
    void dump(const Cfg *Func, Ostream &Str) const override;

    static bool classof(const Operand *Operand) {
      return Operand->getKind() == static_cast<OperandKind>(kSplit);
    }

  private:
    VariableSplit(Cfg *Func, Variable *Var, Portion Part)
        : X86Operand(kSplit, IceType_i32), Var(Var), Part(Part) {
      assert(Var->getType() == IceType_f64);
      Vars = Func->allocateArrayOf<Variable *>(1);
      Vars[0] = Var;
      NumVars = 1;
    }

    Variable *Var;
    Portion Part;
  };

  /// SpillVariable decorates a Variable by linking it to another Variable. When
  /// stack frame offsets are computed, the SpillVariable is given a distinct
  /// stack slot only if its linked Variable has a register. If the linked
  /// Variable has a stack slot, then the Variable and SpillVariable share that
  /// slot.
  class SpillVariable : public Variable {
    SpillVariable() = delete;
    SpillVariable(const SpillVariable &) = delete;
    SpillVariable &operator=(const SpillVariable &) = delete;

  public:
    static SpillVariable *create(Cfg *Func, Type Ty, SizeT Index) {
      return new (Func->allocate<SpillVariable>()) SpillVariable(Ty, Index);
    }
    const static OperandKind SpillVariableKind =
        static_cast<OperandKind>(kVariable_Target);
    static bool classof(const Operand *Operand) {
      return Operand->getKind() == SpillVariableKind;
    }
    void setLinkedTo(Variable *Var) { LinkedTo = Var; }
    Variable *getLinkedTo() const { return LinkedTo; }
    // Inherit dump() and emit() from Variable.

  private:
    SpillVariable(Type Ty, SizeT Index)
        : Variable(SpillVariableKind, Ty, Index), LinkedTo(nullptr) {}
    Variable *LinkedTo;
  };

  // Note: The following data structures are defined in IceInstX8664.cpp.

  static const struct InstBrAttributesType {
    Cond::BrCond Opposite;
    const char *DisplayString;
    const char *EmitString;
  } InstBrAttributes[];

  static const struct InstCmppsAttributesType {
    const char *EmitString;
  } InstCmppsAttributes[];

  static const struct TypeAttributesType {
    const char *CvtString;   // i (integer), s (single FP), d (double FP)
    const char *SdSsString;  // ss, sd, or <blank>
    const char *PackString;  // b, w, d, or <blank>
    const char *WidthString; // b, w, l, q, or <blank>
    const char *FldString;   // s, l, or <blank>
  } TypeAttributes[];
};

} // end of namespace X86Internal

namespace X8664 {
using Traits = ::Ice::X86Internal::MachineTraits<TargetX8664>;
} // end of namespace X8664

} // end of namespace Ice

#endif // SUBZERO_SRC_ICETARGETLOWERINGX8664TRAITS_H
