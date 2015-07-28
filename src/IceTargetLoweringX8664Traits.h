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
/// This file declares the X8664 Target Lowering Traits.
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

namespace Ice {

class TargetX8664;

namespace X8664 {
class AssemblerX8664;
} // end of namespace X8664

namespace X86Internal {

template <class Machine> struct Insts;
template <class Machine> struct MachineTraits;

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
  using XmmRegister = ::Ice::RegX8664::XmmRegister;
  using ByteRegister = ::Ice::RegX8664::ByteRegister;

  using Cond = ::Ice::CondX8664;

  using RegisterSet = ::Ice::RegX8664;
  static const GPRRegister Encoded_Reg_Accumulator = RegX8664::Encoded_Reg_eax;
  static const GPRRegister Encoded_Reg_Counter = RegX8664::Encoded_Reg_ecx;
  static const FixupKind PcRelFixup = llvm::ELF::R_386_PC32; // TODO(jpp): ???

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

    Address(GPRRegister base, int32_t disp) {
      if (disp == 0 && (base & 7) != RegX8664::Encoded_Reg_ebp) {
        SetModRM(0, base);
        if ((base & 7) == RegX8664::Encoded_Reg_esp)
          SetSIB(TIMES_1, RegX8664::Encoded_Reg_esp, base);
      } else if (Utils::IsInt(8, disp)) {
        SetModRM(1, base);
        if ((base & 7) == RegX8664::Encoded_Reg_esp)
          SetSIB(TIMES_1, RegX8664::Encoded_Reg_esp, base);
        SetDisp8(disp);
      } else {
        SetModRM(2, base);
        if ((base & 7) == RegX8664::Encoded_Reg_esp)
          SetSIB(TIMES_1, RegX8664::Encoded_Reg_esp, base);
        SetDisp32(disp);
      }
    }

    Address(GPRRegister index, ScaleFactor scale, int32_t disp) {
      assert(index != RegX8664::Encoded_Reg_esp); // Illegal addressing mode.
      SetModRM(0, RegX8664::Encoded_Reg_esp);
      SetSIB(scale, index, RegX8664::Encoded_Reg_ebp);
      SetDisp32(disp);
    }

    Address(GPRRegister base, GPRRegister index, ScaleFactor scale,
            int32_t disp) {
      assert(index != RegX8664::Encoded_Reg_esp); // Illegal addressing mode.
      if (disp == 0 && (base & 7) != RegX8664::Encoded_Reg_ebp) {
        SetModRM(0, RegX8664::Encoded_Reg_esp);
        SetSIB(scale, index, base);
      } else if (Utils::IsInt(8, disp)) {
        SetModRM(1, RegX8664::Encoded_Reg_esp);
        SetSIB(scale, index, base);
        SetDisp8(disp);
      } else {
        SetModRM(2, RegX8664::Encoded_Reg_esp);
        SetSIB(scale, index, base);
        SetDisp32(disp);
      }
    }

    // PcRelTag is a special tag for requesting rip-relative addressing in
    // X86-64.
    // TODO(jpp): this is bogus. remove.
    enum AbsoluteTag { ABSOLUTE };

    Address(AbsoluteTag, const uintptr_t Addr) {
      SetModRM(0, RegX8664::Encoded_Reg_ebp);
      SetDisp32(Addr);
    }

    // TODO(jpp): remove this.
    static Address Absolute(const uintptr_t Addr) {
      return Address(ABSOLUTE, Addr);
    }

    Address(AbsoluteTag, RelocOffsetT Offset, AssemblerFixup *Fixup) {
      SetModRM(0, RegX8664::Encoded_Reg_ebp);
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
      // TODO(jpp): ???
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
  using Assembler = X8664::AssemblerX8664;
};

} // end of namespace X86Internal

namespace X8664 {
using Traits = ::Ice::X86Internal::MachineTraits<TargetX8664>;
} // end of namespace X8664

} // end of namespace Ice

#endif // SUBZERO_SRC_ICETARGETLOWERINGX8664TRAITS_H
