//===- subzero/src/IceTargetLoweringX8632Traits.h - x86-32 traits -*- C++ -*-=//
//
//                        The Subzero Code Generator
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines the X8632 Target Lowering Traits.
//
//===----------------------------------------------------------------------===//

#ifndef SUBZERO_SRC_ICETARGETLOWERINGX8632TRAITS_H
#define SUBZERO_SRC_ICETARGETLOWERINGX8632TRAITS_H

#include "IceAssembler.h"
#include "IceConditionCodesX8632.h"
#include "IceDefs.h"
#include "IceInst.h"
#include "IceInstX8632.def"
#include "IceRegistersX8632.h"
#include "IceTargetLoweringX8632.def"

namespace Ice {

class TargetX8632;

namespace X86Internal {

template <class Machine> struct MachineTraits;

template <> struct MachineTraits<TargetX8632> {
  //----------------------------------------------------------------------------
  //     ______  ______  __    __
  //    /\  __ \/\  ___\/\ "-./  \
  //    \ \  __ \ \___  \ \ \-./\ \
  //     \ \_\ \_\/\_____\ \_\ \ \_\
  //      \/_/\/_/\/_____/\/_/  \/_/
  //
  //----------------------------------------------------------------------------
  enum ScaleFactor { TIMES_1 = 0, TIMES_2 = 1, TIMES_4 = 2, TIMES_8 = 3 };

  using GPRRegister = ::Ice::RegX8632::GPRRegister;
  using XmmRegister = ::Ice::RegX8632::XmmRegister;
  using ByteRegister = ::Ice::RegX8632::ByteRegister;
  using X87STRegister = ::Ice::RegX8632::X87STRegister;

  using Cond = ::Ice::CondX86;

  using RegisterSet = ::Ice::RegX8632;
  static const GPRRegister Encoded_Reg_Accumulator = RegX8632::Encoded_Reg_eax;
  static const GPRRegister Encoded_Reg_Counter = RegX8632::Encoded_Reg_ecx;
  static const FixupKind PcRelFixup = llvm::ELF::R_386_PC32;

  class Operand {
  public:
    Operand(const Operand &other)
        : length_(other.length_), fixup_(other.fixup_) {
      memmove(&encoding_[0], &other.encoding_[0], other.length_);
    }

    Operand &operator=(const Operand &other) {
      length_ = other.length_;
      fixup_ = other.fixup_;
      memmove(&encoding_[0], &other.encoding_[0], other.length_);
      return *this;
    }

    uint8_t mod() const { return (encoding_at(0) >> 6) & 3; }

    GPRRegister rm() const {
      return static_cast<GPRRegister>(encoding_at(0) & 7);
    }

    ScaleFactor scale() const {
      return static_cast<ScaleFactor>((encoding_at(1) >> 6) & 3);
    }

    GPRRegister index() const {
      return static_cast<GPRRegister>((encoding_at(1) >> 3) & 7);
    }

    GPRRegister base() const {
      return static_cast<GPRRegister>(encoding_at(1) & 7);
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
    Operand() : length_(0), fixup_(nullptr) {} // Needed by subclass Address.

    void SetModRM(int mod, GPRRegister rm) {
      assert((mod & ~3) == 0);
      encoding_[0] = (mod << 6) | rm;
      length_ = 1;
    }

    void SetSIB(ScaleFactor scale, GPRRegister index, GPRRegister base) {
      assert(length_ == 1);
      assert((scale & ~3) == 0);
      encoding_[1] = (scale << 6) | (index << 3) | base;
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
    uint8_t length_;
    uint8_t encoding_[6];
    AssemblerFixup *fixup_;

    explicit Operand(GPRRegister reg) : fixup_(nullptr) { SetModRM(3, reg); }

    // Get the operand encoding byte at the given index.
    uint8_t encoding_at(intptr_t index) const {
      assert(index >= 0 && index < length_);
      return encoding_[index];
    }

    // Returns whether or not this operand is really the given register in
    // disguise. Used from the assembler to generate better encodings.
    bool IsRegister(GPRRegister reg) const {
      return ((encoding_[0] & 0xF8) ==
              0xC0) // Addressing mode is register only.
             &&
             ((encoding_[0] & 0x07) == reg); // Register codes match.
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

    Address(GPRRegister base, int32_t disp) {
      if (disp == 0 && base != RegX8632::Encoded_Reg_ebp) {
        SetModRM(0, base);
        if (base == RegX8632::Encoded_Reg_esp)
          SetSIB(TIMES_1, RegX8632::Encoded_Reg_esp, base);
      } else if (Utils::IsInt(8, disp)) {
        SetModRM(1, base);
        if (base == RegX8632::Encoded_Reg_esp)
          SetSIB(TIMES_1, RegX8632::Encoded_Reg_esp, base);
        SetDisp8(disp);
      } else {
        SetModRM(2, base);
        if (base == RegX8632::Encoded_Reg_esp)
          SetSIB(TIMES_1, RegX8632::Encoded_Reg_esp, base);
        SetDisp32(disp);
      }
    }

    Address(GPRRegister index, ScaleFactor scale, int32_t disp) {
      assert(index != RegX8632::Encoded_Reg_esp); // Illegal addressing mode.
      SetModRM(0, RegX8632::Encoded_Reg_esp);
      SetSIB(scale, index, RegX8632::Encoded_Reg_ebp);
      SetDisp32(disp);
    }

    Address(GPRRegister base, GPRRegister index, ScaleFactor scale,
            int32_t disp) {
      assert(index != RegX8632::Encoded_Reg_esp); // Illegal addressing mode.
      if (disp == 0 && base != RegX8632::Encoded_Reg_ebp) {
        SetModRM(0, RegX8632::Encoded_Reg_esp);
        SetSIB(scale, index, base);
      } else if (Utils::IsInt(8, disp)) {
        SetModRM(1, RegX8632::Encoded_Reg_esp);
        SetSIB(scale, index, base);
        SetDisp8(disp);
      } else {
        SetModRM(2, RegX8632::Encoded_Reg_esp);
        SetSIB(scale, index, base);
        SetDisp32(disp);
      }
    }

    // AbsoluteTag is a special tag used by clients to create an absolute
    // Address.
    enum AbsoluteTag { ABSOLUTE };

    Address(AbsoluteTag, const uintptr_t Addr) {
      SetModRM(0, RegX8632::Encoded_Reg_ebp);
      SetDisp32(Addr);
    }

    // TODO(jpp): remove this.
    static Address Absolute(const uintptr_t Addr) {
      return Address(ABSOLUTE, Addr);
    }

    Address(AbsoluteTag, RelocOffsetT Offset, AssemblerFixup *Fixup) {
      SetModRM(0, RegX8632::Encoded_Reg_ebp);
      // Use the Offset in the displacement for now. If we decide to process
      // fixups later, we'll need to patch up the emitted displacement.
      SetDisp32(Offset);
      SetFixup(Fixup);
    }

    // TODO(jpp): remove this.
    static Address Absolute(RelocOffsetT Offset, AssemblerFixup *Fixup) {
      return Address(ABSOLUTE, Offset, Fixup);
    }

    static Address ofConstPool(Assembler *Asm, const Constant *Imm) {
      AssemblerFixup *Fixup = Asm->createFixup(llvm::ELF::R_386_32, Imm);
      const RelocOffsetT Offset = 0;
      return Address(ABSOLUTE, Offset, Fixup);
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

  // The maximum number of arguments to pass in XMM registers
  static const uint32_t X86_MAX_XMM_ARGS = 4;
  // The number of bits in a byte
  static const uint32_t X86_CHAR_BIT = 8;
  // Stack alignment. This is defined in IceTargetLoweringX8632.cpp because it
  // is used as an argument to std::max(), and the default std::less<T> has an
  // operator(T const&, T const&) which requires this member to have an address.
  static const uint32_t X86_STACK_ALIGNMENT_BYTES;
  // Size of the return address on the stack
  static const uint32_t X86_RET_IP_SIZE_BYTES = 4;
  // The number of different NOP instructions
  static const uint32_t X86_NUM_NOP_VARIANTS = 5;

  // Value is in bytes. Return Value adjusted to the next highest multiple
  // of the stack alignment.
  static uint32_t applyStackAlignment(uint32_t Value) {
    return Utils::applyAlignment(Value, X86_STACK_ALIGNMENT_BYTES);
  }

  // Return the type which the elements of the vector have in the X86
  // representation of the vector.
  static Type getInVectorElementType(Type Ty) {
    assert(isVectorType(Ty));
    size_t Index = static_cast<size_t>(Ty);
    (void)Index;
    assert(Index < TableTypeX8632AttributesSize);
    return TableTypeX8632Attributes[Ty].InVectorElementType;
  }

  // Note: The following data structures are defined in
  // IceTargetLoweringX8632.cpp.

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
  static const struct TableFcmpType {
    uint32_t Default;
    bool SwapScalarOperands;
    CondX86::BrCond C1, C2;
    bool SwapVectorOperands;
    CondX86::CmppsCond Predicate;
  } TableFcmp[];
  static const size_t TableFcmpSize;

  // The following table summarizes the logic for lowering the icmp instruction
  // for i32 and narrower types.  Each icmp condition has a clear mapping to an
  // x86 conditional branch instruction.

  static const struct TableIcmp32Type {
    CondX86::BrCond Mapping;
  } TableIcmp32[];
  static const size_t TableIcmp32Size;

  // The following table summarizes the logic for lowering the icmp instruction
  // for the i64 type.  For Eq and Ne, two separate 32-bit comparisons and
  // conditional branches are needed.  For the other conditions, three separate
  // conditional branches are needed.
  static const struct TableIcmp64Type {
    CondX86::BrCond C1, C2, C3;
  } TableIcmp64[];
  static const size_t TableIcmp64Size;

  static CondX86::BrCond getIcmp32Mapping(InstIcmp::ICond Cond) {
    size_t Index = static_cast<size_t>(Cond);
    assert(Index < TableIcmp32Size);
    return TableIcmp32[Index].Mapping;
  }

  static const struct TableTypeX8632AttributesType {
    Type InVectorElementType;
  } TableTypeX8632Attributes[];
  static const size_t TableTypeX8632AttributesSize;
};

} // end of namespace X86Internal

namespace X8632 {
using Traits = ::Ice::X86Internal::MachineTraits<TargetX8632>;
} // end of namespace X8632

} // end of namespace Ice

#endif // SUBZERO_SRC_ICETARGETLOWERINGX8632TRAITS_H
