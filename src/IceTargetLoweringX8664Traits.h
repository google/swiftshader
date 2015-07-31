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
#include "IceTargetLoweringX8664.def"

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
  enum InstructionSet {
    Begin,
    // SSE2 is the PNaCl baseline instruction set.
    SSE2 = Begin,
    SSE4_1,
    End
  };

  static const char *TargetName;

  static IceString getRegName(SizeT RegNum, Type Ty) {
    assert(RegNum < RegisterSet::Reg_NUM);
    static const struct {
      const char *const Name8;
      const char *const Name16;
      const char *const Name /*32*/;
      const char *const Name64;
    } RegNames[] = {
#define X(val, encode, name64, name32, name16, name8, scratch, preserved,      \
          stackptr, frameptr, isInt, isFP)                                     \
  { name8, name16, name32, name64 }                                            \
  ,
        REGX8664_TABLE
#undef X
    };

    switch (Ty) {
    case IceType_i1:
    case IceType_i8:
      return RegNames[RegNum].Name8;
    case IceType_i16:
      return RegNames[RegNum].Name16;
    case IceType_i64:
      return RegNames[RegNum].Name64;
    default:
      return RegNames[RegNum].Name;
    }
  }

  static void initRegisterSet(llvm::SmallBitVector *IntegerRegisters,
                              llvm::SmallBitVector *IntegerRegistersI8,
                              llvm::SmallBitVector *FloatRegisters,
                              llvm::SmallBitVector *VectorRegisters,
                              llvm::SmallBitVector *ScratchRegs) {
#define X(val, encode, name64, name32, name16, name8, scratch, preserved,      \
          stackptr, frameptr, isInt, isFP)                                     \
  (*IntegerRegisters)[RegisterSet::val] = isInt;                               \
  (*IntegerRegistersI8)[RegisterSet::val] = 1;                                 \
  (*FloatRegisters)[RegisterSet::val] = isFP;                                  \
  (*VectorRegisters)[RegisterSet::val] = isFP;                                 \
  (*ScratchRegs)[RegisterSet::val] = scratch;
    REGX8664_TABLE;
#undef X
  }

  static llvm::SmallBitVector
  getRegisterSet(TargetLowering::RegSetMask Include,
                 TargetLowering::RegSetMask Exclude) {
    llvm::SmallBitVector Registers(RegisterSet::Reg_NUM);

#define X(val, encode, name64, name32, name16, name8, scratch, preserved,      \
          stackptr, frameptr, isInt, isFP)                                     \
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
                                const llvm::SmallBitVector &ExcludeRegisters) {
    // TODO(stichnot): Declaring Permutation this way loses type/size
    // information.  Fix this in conjunction with the caller-side TODO.
    assert(Permutation.size() >= RegisterSet::Reg_NUM);
    // Expected upper bound on the number of registers in a single equivalence
    // class.  For x86-64, this would comprise the 16 XMM registers.  This is
    // for performance, not correctness.
    static const unsigned MaxEquivalenceClassSize = 8;
    typedef llvm::SmallVector<int32_t, MaxEquivalenceClassSize> RegisterList;
    typedef std::map<uint32_t, RegisterList> EquivalenceClassMap;
    EquivalenceClassMap EquivalenceClasses;
    SizeT NumShuffled = 0, NumPreserved = 0;

// Build up the equivalence classes of registers by looking at the register
// properties as well as whether the registers should be explicitly excluded
// from shuffling.
#define X(val, encode, name64, name32, name16, name8, scratch, preserved,      \
          stackptr, frameptr, isInt, isFP)                                     \
  if (ExcludeRegisters[RegisterSet::val]) {                                    \
    /* val stays the same in the resulting permutation. */                     \
    Permutation[RegisterSet::val] = RegisterSet::val;                          \
    ++NumPreserved;                                                            \
  } else {                                                                     \
    const uint32_t Index = (scratch << 0) | (preserved << 1) |                 \
                           (/*isI8=*/1 << 2) | (isInt << 3) | (isFP << 4);     \
    /* val is assigned to an equivalence class based on its properties. */     \
    EquivalenceClasses[Index].push_back(RegisterSet::val);                     \
  }
    REGX8664_TABLE
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
          Str << getRegName(Register, IceType_i32);
        }
        Str << "}\n";
      }
    }
  }

  /// The maximum number of arguments to pass in XMM registers
  static const uint32_t X86_MAX_XMM_ARGS = 4;
  /// The number of bits in a byte
  static const uint32_t X86_CHAR_BIT = 8;
  /// Stack alignment. This is defined in IceTargetLoweringX8664.cpp because it
  /// is used as an argument to std::max(), and the default std::less<T> has an
  /// operator(T const&, T const&) which requires this member to have an
  /// address.
  static const uint32_t X86_STACK_ALIGNMENT_BYTES;
  /// Size of the return address on the stack
  static const uint32_t X86_RET_IP_SIZE_BYTES = 4;
  /// The number of different NOP instructions
  static const uint32_t X86_NUM_NOP_VARIANTS = 5;

  /// Value is in bytes. Return Value adjusted to the next highest multiple
  /// of the stack alignment.
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
  /// point scalar values.  A comment in lowerFcmp() describes the lowering
  /// template.  In the most general case, there is a compare followed by two
  /// conditional branches, because some fcmp conditions don't map to a single
  /// x86 conditional branch.  However, in many cases it is possible to swap the
  /// operands in the comparison and have a single conditional branch.  Since
  /// it's quite tedious to validate the table by hand, good execution tests are
  /// helpful.
  ///
  /// The last two columns describe the case when the operands are vectors of
  /// floating point values.  For most fcmp conditions, there is a clear mapping
  /// to a single x86 cmpps instruction variant.  Some fcmp conditions require
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
  /// for i32 and narrower types.  Each icmp condition has a clear mapping to an
  /// x86 conditional branch instruction.
  /// {@
  static const struct TableIcmp32Type { Cond::BrCond Mapping; } TableIcmp32[];
  static const size_t TableIcmp32Size;
  /// @}

  /// The following table summarizes the logic for lowering the icmp instruction
  /// for the i64 type.  For Eq and Ne, two separate 32-bit comparisons and
  /// conditional branches are needed.  For the other conditions, three separate
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

  /// X86Operand extends the Operand hierarchy.  Its subclasses are
  /// X86OperandMem and VariableSplit.
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
    Address toAsmAddress(Assembler *Asm) const;

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
  /// locations (Low and High).  This is needed for some cases of the Bitcast
  /// instruction.  Since it's not possible for integer registers to access the
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

  /// SpillVariable decorates a Variable by linking it to another Variable.
  /// When stack frame offsets are computed, the SpillVariable is given a
  /// distinct stack slot only if its linked Variable has a register.  If the
  /// linked Variable has a stack slot, then the Variable and SpillVariable
  /// share that slot.
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
