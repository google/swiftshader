//===- subzero/unittest/IceAssemblerX8632.cpp - X8632 Assembler tests -----===//
//
//                        The Subzero Code Generator
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "IceAssemblerX8632.h"

#include "IceDefs.h"

#include "gtest/gtest.h"

#include <cstring>
#include <errno.h>
#include <iostream>
#include <memory>
#include <sys/mman.h>
#include <type_traits>

namespace Ice {
namespace X8632 {
namespace {

class AssemblerX8632TestBase : public ::testing::Test {
protected:
  using Address = AssemblerX8632::Traits::Address;
  using Cond = AssemblerX8632::Traits::Cond;
  using GPRRegister = AssemblerX8632::Traits::GPRRegister;
  using XmmRegister = AssemblerX8632::Traits::XmmRegister;
  using X87STRegister = AssemblerX8632::Traits::X87STRegister;

  AssemblerX8632TestBase() { reset(); }

  void reset() { Assembler.reset(new AssemblerX8632()); }

  AssemblerX8632 *assembler() const { return Assembler.get(); }

  size_t codeBytesSize() const { return Assembler->getBufferView().size(); }

  const uint8_t *codeBytes() const {
    return static_cast<const uint8_t *>(
        static_cast<const void *>(Assembler->getBufferView().data()));
  }

private:
  std::unique_ptr<AssemblerX8632> Assembler;
};

// __ is a helper macro. It allows test cases to emit X8632 assembly
// instructions with
//
//   __ mov(GPRRegister::Reg_Eax, 1);
//   __ ret();
//
// and so on. The idea of having this was "stolen" from dart's unit tests.
#define __ (this->assembler())->

// AssemblerX8632LowLevelTest verify that the "basic" instructions the tests
// rely on are encoded correctly. Therefore, instead of executing the assembled
// code, these tests will verify that the assembled bytes are sane.
class AssemblerX8632LowLevelTest : public AssemblerX8632TestBase {
protected:
  // verifyBytes is a template helper that takes a Buffer, and a variable number
  // of bytes. As the name indicates, it is used to verify the bytes for an
  // instruction encoding.
  template <int N, int I> static void verifyBytes(const uint8_t *) {
    static_assert(I == N, "Invalid template instantiation.");
  }

  template <int N, int I = 0, typename... Args>
  static void verifyBytes(const uint8_t *Buffer, uint8_t Byte,
                          Args... OtherBytes) {
    static_assert(I < N, "Invalid template instantiation.");
    EXPECT_EQ(Byte, Buffer[I]) << "Byte " << (I + 1) << " of " << N;
    verifyBytes<N, I + 1>(Buffer, OtherBytes...);
    assert(Buffer[I] == Byte);
  }
};

TEST_F(AssemblerX8632LowLevelTest, Ret) {
  __ ret();

  constexpr size_t ByteCount = 1;
  ASSERT_EQ(ByteCount, codeBytesSize());

  verifyBytes<ByteCount>(codeBytes(), 0xc3);
}

TEST_F(AssemblerX8632LowLevelTest, CallImm4) {
  __ call(Immediate(4));

  constexpr size_t ByteCount = 5;
  ASSERT_EQ(ByteCount, codeBytesSize());

  verifyBytes<ByteCount>(codeBytes(), 0xe8, 0x00, 0x00, 0x00, 0x00);
}

TEST_F(AssemblerX8632LowLevelTest, PopRegs) {
  __ popl(GPRRegister::Encoded_Reg_eax);
  __ popl(GPRRegister::Encoded_Reg_ebx);
  __ popl(GPRRegister::Encoded_Reg_ecx);
  __ popl(GPRRegister::Encoded_Reg_edx);
  __ popl(GPRRegister::Encoded_Reg_edi);
  __ popl(GPRRegister::Encoded_Reg_esi);
  __ popl(GPRRegister::Encoded_Reg_ebp);

  constexpr size_t ByteCount = 7;
  ASSERT_EQ(ByteCount, codeBytesSize());

  constexpr uint8_t PopOpcode = 0x58;
  verifyBytes<ByteCount>(codeBytes(), PopOpcode | GPRRegister::Encoded_Reg_eax,
                         PopOpcode | GPRRegister::Encoded_Reg_ebx,
                         PopOpcode | GPRRegister::Encoded_Reg_ecx,
                         PopOpcode | GPRRegister::Encoded_Reg_edx,
                         PopOpcode | GPRRegister::Encoded_Reg_edi,
                         PopOpcode | GPRRegister::Encoded_Reg_esi,
                         PopOpcode | GPRRegister::Encoded_Reg_ebp);
}

TEST_F(AssemblerX8632LowLevelTest, PushRegs) {
  __ pushl(GPRRegister::Encoded_Reg_eax);
  __ pushl(GPRRegister::Encoded_Reg_ebx);
  __ pushl(GPRRegister::Encoded_Reg_ecx);
  __ pushl(GPRRegister::Encoded_Reg_edx);
  __ pushl(GPRRegister::Encoded_Reg_edi);
  __ pushl(GPRRegister::Encoded_Reg_esi);
  __ pushl(GPRRegister::Encoded_Reg_ebp);

  constexpr size_t ByteCount = 7;
  ASSERT_EQ(ByteCount, codeBytesSize());

  constexpr uint8_t PushOpcode = 0x50;
  verifyBytes<ByteCount>(codeBytes(), PushOpcode | GPRRegister::Encoded_Reg_eax,
                         PushOpcode | GPRRegister::Encoded_Reg_ebx,
                         PushOpcode | GPRRegister::Encoded_Reg_ecx,
                         PushOpcode | GPRRegister::Encoded_Reg_edx,
                         PushOpcode | GPRRegister::Encoded_Reg_edi,
                         PushOpcode | GPRRegister::Encoded_Reg_esi,
                         PushOpcode | GPRRegister::Encoded_Reg_ebp);
}

TEST_F(AssemblerX8632LowLevelTest, MovRegisterZero) {
  __ mov(IceType_i32, GPRRegister::Encoded_Reg_eax, Immediate(0x00));
  __ mov(IceType_i32, GPRRegister::Encoded_Reg_ebx, Immediate(0x00));
  __ mov(IceType_i32, GPRRegister::Encoded_Reg_ecx, Immediate(0x00));
  __ mov(IceType_i32, GPRRegister::Encoded_Reg_edx, Immediate(0x00));
  __ mov(IceType_i32, GPRRegister::Encoded_Reg_edi, Immediate(0x00));
  __ mov(IceType_i32, GPRRegister::Encoded_Reg_esi, Immediate(0x00));

  constexpr size_t MovReg32BitImmBytes = 5;
  constexpr size_t ByteCount = 6 * MovReg32BitImmBytes;
  ASSERT_EQ(ByteCount, codeBytesSize());

  constexpr uint8_t MovOpcode = 0xb8;
  verifyBytes<ByteCount>(
      codeBytes(), MovOpcode | GPRRegister::Encoded_Reg_eax, 0x00, 0x00, 0x00,
      0x00, MovOpcode | GPRRegister::Encoded_Reg_ebx, 0x00, 0x00, 0x00, 0x00,
      MovOpcode | GPRRegister::Encoded_Reg_ecx, 0x00, 0x00, 0x00, 0x00,
      MovOpcode | GPRRegister::Encoded_Reg_edx, 0x00, 0x00, 0x00, 0x00,
      MovOpcode | GPRRegister::Encoded_Reg_edi, 0x00, 0x00, 0x00, 0x00,
      MovOpcode | GPRRegister::Encoded_Reg_esi, 0x00, 0x00, 0x00, 0x00);
}

TEST_F(AssemblerX8632LowLevelTest, CmpRegReg) {
  __ cmp(IceType_i32, GPRRegister::Encoded_Reg_eax,
         GPRRegister::Encoded_Reg_ebx);
  __ cmp(IceType_i32, GPRRegister::Encoded_Reg_ebx,
         GPRRegister::Encoded_Reg_ecx);
  __ cmp(IceType_i32, GPRRegister::Encoded_Reg_ecx,
         GPRRegister::Encoded_Reg_edx);
  __ cmp(IceType_i32, GPRRegister::Encoded_Reg_edx,
         GPRRegister::Encoded_Reg_edi);
  __ cmp(IceType_i32, GPRRegister::Encoded_Reg_edi,
         GPRRegister::Encoded_Reg_esi);
  __ cmp(IceType_i32, GPRRegister::Encoded_Reg_esi,
         GPRRegister::Encoded_Reg_eax);

  const size_t CmpRegRegBytes = 2;
  const size_t ByteCount = 6 * CmpRegRegBytes;
  ASSERT_EQ(ByteCount, codeBytesSize());

  constexpr size_t CmpOpcode = 0x3b;
  constexpr size_t ModRm = 0xC0 /* Register Addressing */;
  verifyBytes<ByteCount>(
      codeBytes(), CmpOpcode, ModRm | (GPRRegister::Encoded_Reg_eax << 3) |
                                  GPRRegister::Encoded_Reg_ebx,
      CmpOpcode, ModRm | (GPRRegister::Encoded_Reg_ebx << 3) |
                     GPRRegister::Encoded_Reg_ecx,
      CmpOpcode, ModRm | (GPRRegister::Encoded_Reg_ecx << 3) |
                     GPRRegister::Encoded_Reg_edx,
      CmpOpcode, ModRm | (GPRRegister::Encoded_Reg_edx << 3) |
                     GPRRegister::Encoded_Reg_edi,
      CmpOpcode, ModRm | (GPRRegister::Encoded_Reg_edi << 3) |
                     GPRRegister::Encoded_Reg_esi,
      CmpOpcode, ModRm | (GPRRegister::Encoded_Reg_esi << 3) |
                     GPRRegister::Encoded_Reg_eax);
}

// After these tests we should have a sane environment; we know the following
// work:
//
//  (*) zeroing eax, ebx, ecx, edx, edi, and esi;
//  (*) call $4 instruction (used for ip materialization);
//  (*) register push and pop;
//  (*) cmp reg, reg; and
//  (*) returning from functions.
//
// We can now dive into testing each emitting method in AssemblerX8632. Each
// test will emit some instructions for performing the test. The assembled
// instructions will operate in a "safe" environment. All x86-32 registers are
// spilled to the program stack, and the registers are then zeroed out, with the
// exception of %esp and %ebp.
//
// The jitted code and the unittest code will share the same stack. Therefore,
// test harnesses need to ensure it does not leave anything it pushed on the
// stack.
//
// %ebp is initialized with a pointer for rIP-based addressing. This pointer is
// used for position-independent access to a scratchpad area for use in tests.
// This mechanism is used because the test framework needs to generate addresses
// that work on both x86-32 and x86-64 hosts, but are encodable using our x86-32
// assembler. This is made possible because the encoding for
//
//    pushq %rax (x86-64 only)
//
// is the same as the one for
//
//    pushl %eax (x86-32 only; not encodable in x86-64)
//
// Likewise, the encodings for
//
//    movl offset(%ebp), %reg (32-bit only)
//    movl <src>, offset(%ebp) (32-bit only)
//
// and
//
//    movl offset(%rbp), %reg (64-bit only)
//    movl <src>, offset(%rbp) (64-bit only)
//
// are also the same.
//
// We use a call instruction in order to generate a natural sized address on the
// stack. Said address is then removed from the stack with a pop %rBP, which can
// then be used to address memory safely in either x86-32 or x86-64, as long as
// the test code does not perform any arithmetic operation that writes to %rBP.
// This PC materialization technique is very common in x86-32 PIC.
//
// %rBP is used to provide the tests with a scratchpad area that can safely and
// portably be written to and read from. This scratchpad area is also used to
// store the "final" values in eax, ebx, ecx, edx, esi, and edi, allowing the
// harnesses access to 6 "return values" instead of the usual single return
// value supported by C++.
//
// The jitted code will look like the following:
//
// test:
//       push %eax
//       push %ebx
//       push %ecx
//       push %edx
//       push %edi
//       push %esi
//       push %ebp
//       call test$materialize_ip
// test$materialize_ip:                           <<------- %eBP will point here
//       pop  %ebp
//       mov  $0, %eax
//       mov  $0, %ebx
//       mov  $0, %ecx
//       mov  $0, %edx
//       mov  $0, %edi
//       mov  $0, %esi
//
//       << test code goes here >>
//
//       mov %eax, { 0 + $ScratchpadOffset}(%ebp)
//       mov %ebx, { 4 + $ScratchpadOffset}(%ebp)
//       mov %ecx, { 8 + $ScratchpadOffset}(%ebp)
//       mov %edx, {12 + $ScratchpadOffset}(%ebp)
//       mov %edi, {16 + $ScratchpadOffset}(%ebp)
//       mov %esi, {20 + $ScratchpadOffset}(%ebp)
//
//       pop %ebp
//       pop %esi
//       pop %edi
//       pop %edx
//       pop %ecx
//       pop %ebx
//       pop %eax
//       ret
//
//      << ... >>
//
// scratchpad:                              <<------- accessed via $Offset(%ebp)
//
//      << test scratch area >>
//
// TODO(jpp): test the
//
//    mov %reg, $Offset(%ebp)
//
// encodings using the low level assembler test ensuring that the register
// values can be written to the scratchpad area.
class AssemblerX8632Test : public AssemblerX8632TestBase {
protected:
  AssemblerX8632Test() { reset(); }

  void reset() {
    AssemblerX8632TestBase::reset();

    NeedsEpilogue = true;
    // 6 dwords are allocated for saving the GPR state after the jitted code
    // runs.
    NumAllocatedDwords = 6;
    addPrologue();
  }

  // AssembledBuffer is a wrapper around a PROT_EXEC mmap'ed buffer. This buffer
  // contains both the test code as well as prologue/epilogue, and the
  // scratchpad area that tests may use -- all tests use this scratchpad area
  // for storing the processor's registers after the tests executed. This class
  // also exposes helper methods for reading the register state after test
  // execution, as well as for reading the scratchpad area.
  class AssembledBuffer {
    AssembledBuffer() = delete;
    AssembledBuffer(const AssembledBuffer &) = delete;
    AssembledBuffer &operator=(const AssembledBuffer &) = delete;

  public:
    static constexpr uint32_t MaximumCodeSize = 1 << 20;
    static constexpr uint32_t EaxSlot = 0;
    static constexpr uint32_t EbxSlot = 1;
    static constexpr uint32_t EcxSlot = 2;
    static constexpr uint32_t EdxSlot = 3;
    static constexpr uint32_t EdiSlot = 4;
    static constexpr uint32_t EsiSlot = 5;

    AssembledBuffer(const uint8_t *Data, const size_t MySize,
                    const size_t ExtraStorageDwords)
        : Size(MaximumCodeSize + 4 * ExtraStorageDwords) {
      // MaxCodeSize is needed because EXPECT_LT needs a symbol with a name --
      // probably a compiler bug?
      uint32_t MaxCodeSize = MaximumCodeSize;
      EXPECT_LT(MySize, MaxCodeSize);
      assert(MySize < MaximumCodeSize);
      ExecutableData = mmap(nullptr, Size, PROT_WRITE | PROT_READ | PROT_EXEC,
                            MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
      EXPECT_NE(MAP_FAILED, ExecutableData) << strerror(errno);
      assert(MAP_FAILED != ExecutableData);
      std::memcpy(ExecutableData, Data, MySize);
    }

    // We allow AssembledBuffer to be moved so that we can return objects of
    // this type.
    AssembledBuffer(AssembledBuffer &&Buffer)
        : ExecutableData(Buffer.ExecutableData), Size(Buffer.Size) {
      Buffer.ExecutableData = nullptr;
      Buffer.Size = 0;
    }

    AssembledBuffer &operator=(AssembledBuffer &&Buffer) {
      ExecutableData = Buffer.ExecutableData;
      Buffer.ExecutableData = nullptr;
      Size = Buffer.Size;
      Buffer.Size = 0;
      return *this;
    }

    ~AssembledBuffer() {
      if (ExecutableData != nullptr) {
        munmap(ExecutableData, Size);
        ExecutableData = nullptr;
      }
    }

    void run() const { reinterpret_cast<void (*)()>(ExecutableData)(); }

    uint32_t eax() const { return contentsOfDword(AssembledBuffer::EaxSlot); }

    uint32_t ebx() const { return contentsOfDword(AssembledBuffer::EbxSlot); }

    uint32_t ecx() const { return contentsOfDword(AssembledBuffer::EcxSlot); }

    uint32_t edx() const { return contentsOfDword(AssembledBuffer::EdxSlot); }

    uint32_t edi() const { return contentsOfDword(AssembledBuffer::EdiSlot); }

    uint32_t esi() const { return contentsOfDword(AssembledBuffer::EsiSlot); }

    // contentsOfDword is used for reading the values in the scratchpad area.
    // Valid arguments are the dword ids returned by
    // AssemblerX8632Test::allocateDword() -- other inputs are considered
    // invalid, and are not guaranteed to work if the implementation changes.
    uint32_t contentsOfDword(uint32_t Dword) const {
      return *reinterpret_cast<uint32_t *>(
                 static_cast<uint8_t *>(ExecutableData) + dwordOffset(Dword));
    }

  private:
    static uint32_t dwordOffset(uint32_t Index) {
      return MaximumCodeSize + (Index * 4);
    }

    void *ExecutableData = nullptr;
    size_t Size;
  };

  // assemble created an AssembledBuffer with the jitted code. The first time
  // assemble is executed it will add the epilogue to the jitted code (which is
  // the reason why this method is not const qualified.
  AssembledBuffer assemble() {
    if (NeedsEpilogue) {
      addEpilogue();
    }

    NeedsEpilogue = false;
    return AssembledBuffer(codeBytes(), codeBytesSize(), NumAllocatedDwords);
  }

  // Allocates a new dword slot in the test's scratchpad area.
  uint32_t allocateDword() { return NumAllocatedDwords++; }

  Address dwordAddress(uint32_t Dword) {
    return Address(GPRRegister::Encoded_Reg_ebp, dwordDisp(Dword));
  }

private:
  // e??SlotAddress returns an AssemblerX8632::Traits::Address that can be used
  // by the test cases to encode an address operand for accessing the slot for
  // the specified register. These are all private for, when jitting the test
  // code, tests should not tamper with these values. Besides, during the test
  // execution these slots' contents are undefined and should not be accessed.
  Address eaxSlotAddress() { return dwordAddress(AssembledBuffer::EaxSlot); }
  Address ebxSlotAddress() { return dwordAddress(AssembledBuffer::EbxSlot); }
  Address ecxSlotAddress() { return dwordAddress(AssembledBuffer::EcxSlot); }
  Address edxSlotAddress() { return dwordAddress(AssembledBuffer::EdxSlot); }
  Address ediSlotAddress() { return dwordAddress(AssembledBuffer::EdiSlot); }
  Address esiSlotAddress() { return dwordAddress(AssembledBuffer::EsiSlot); }

  // Returns the displacement that should be used when accessing the specified
  // Dword in the scratchpad area. It needs to adjust for the initial
  // instructions that are emitted before the call that materializes the IP
  // register.
  uint32_t dwordDisp(uint32_t Dword) const {
    EXPECT_LT(Dword, NumAllocatedDwords);
    assert(Dword < NumAllocatedDwords);
    static constexpr uint8_t PushBytes = 1;
    static constexpr uint8_t CallImmBytes = 5;
    return AssembledBuffer::MaximumCodeSize + (Dword * 4) -
           (7 * PushBytes + CallImmBytes);
  }

  void addPrologue() {
    __ pushl(GPRRegister::Encoded_Reg_eax);
    __ pushl(GPRRegister::Encoded_Reg_ebx);
    __ pushl(GPRRegister::Encoded_Reg_ecx);
    __ pushl(GPRRegister::Encoded_Reg_edx);
    __ pushl(GPRRegister::Encoded_Reg_edi);
    __ pushl(GPRRegister::Encoded_Reg_esi);
    __ pushl(GPRRegister::Encoded_Reg_ebp);

    __ call(Immediate(4));
    __ popl(GPRRegister::Encoded_Reg_ebp);
    __ mov(IceType_i32, GPRRegister::Encoded_Reg_eax, Immediate(0x00));
    __ mov(IceType_i32, GPRRegister::Encoded_Reg_ebx, Immediate(0x00));
    __ mov(IceType_i32, GPRRegister::Encoded_Reg_ecx, Immediate(0x00));
    __ mov(IceType_i32, GPRRegister::Encoded_Reg_edx, Immediate(0x00));
    __ mov(IceType_i32, GPRRegister::Encoded_Reg_edi, Immediate(0x00));
    __ mov(IceType_i32, GPRRegister::Encoded_Reg_esi, Immediate(0x00));
  }

  void addEpilogue() {
    __ mov(IceType_i32, eaxSlotAddress(), GPRRegister::Encoded_Reg_eax);
    __ mov(IceType_i32, ebxSlotAddress(), GPRRegister::Encoded_Reg_ebx);
    __ mov(IceType_i32, ecxSlotAddress(), GPRRegister::Encoded_Reg_ecx);
    __ mov(IceType_i32, edxSlotAddress(), GPRRegister::Encoded_Reg_edx);
    __ mov(IceType_i32, ediSlotAddress(), GPRRegister::Encoded_Reg_edi);
    __ mov(IceType_i32, esiSlotAddress(), GPRRegister::Encoded_Reg_esi);

    __ popl(GPRRegister::Encoded_Reg_ebp);
    __ popl(GPRRegister::Encoded_Reg_esi);
    __ popl(GPRRegister::Encoded_Reg_edi);
    __ popl(GPRRegister::Encoded_Reg_edx);
    __ popl(GPRRegister::Encoded_Reg_ecx);
    __ popl(GPRRegister::Encoded_Reg_ebx);
    __ popl(GPRRegister::Encoded_Reg_eax);

    __ ret();
  }

  bool NeedsEpilogue;
  uint32_t NumAllocatedDwords;
};

TEST_F(AssemblerX8632Test, MovRegImm) {
  constexpr uint32_t ExpectedEax = 0x000000FFul;
  constexpr uint32_t ExpectedEbx = 0x0000FF00ul;
  constexpr uint32_t ExpectedEcx = 0x00FF0000ul;
  constexpr uint32_t ExpectedEdx = 0xFF000000ul;
  constexpr uint32_t ExpectedEdi = 0x6AAA0006ul;
  constexpr uint32_t ExpectedEsi = 0x6000AAA6ul;

  __ mov(IceType_i32, GPRRegister::Encoded_Reg_eax, Immediate(ExpectedEax));
  __ mov(IceType_i32, GPRRegister::Encoded_Reg_ebx, Immediate(ExpectedEbx));
  __ mov(IceType_i32, GPRRegister::Encoded_Reg_ecx, Immediate(ExpectedEcx));
  __ mov(IceType_i32, GPRRegister::Encoded_Reg_edx, Immediate(ExpectedEdx));
  __ mov(IceType_i32, GPRRegister::Encoded_Reg_edi, Immediate(ExpectedEdi));
  __ mov(IceType_i32, GPRRegister::Encoded_Reg_esi, Immediate(ExpectedEsi));

  AssembledBuffer test = assemble();
  test.run();
  EXPECT_EQ(ExpectedEax, test.eax());
  EXPECT_EQ(ExpectedEbx, test.ebx());
  EXPECT_EQ(ExpectedEcx, test.ecx());
  EXPECT_EQ(ExpectedEdx, test.edx());
  EXPECT_EQ(ExpectedEdi, test.edi());
  EXPECT_EQ(ExpectedEsi, test.esi());
}

TEST_F(AssemblerX8632Test, MovMemImm) {
  const uint32_t T0 = allocateDword();
  constexpr uint32_t ExpectedT0 = 0x00111100ul;
  const uint32_t T1 = allocateDword();
  constexpr uint32_t ExpectedT1 = 0x00222200ul;
  const uint32_t T2 = allocateDword();
  constexpr uint32_t ExpectedT2 = 0x03333000ul;
  const uint32_t T3 = allocateDword();
  constexpr uint32_t ExpectedT3 = 0x00444400ul;

  __ mov(IceType_i32, dwordAddress(T0), Immediate(ExpectedT0));
  __ mov(IceType_i32, dwordAddress(T1), Immediate(ExpectedT1));
  __ mov(IceType_i32, dwordAddress(T2), Immediate(ExpectedT2));
  __ mov(IceType_i32, dwordAddress(T3), Immediate(ExpectedT3));

  AssembledBuffer test = assemble();
  test.run();
  EXPECT_EQ(0ul, test.eax());
  EXPECT_EQ(0ul, test.ebx());
  EXPECT_EQ(0ul, test.ecx());
  EXPECT_EQ(0ul, test.edx());
  EXPECT_EQ(0ul, test.edi());
  EXPECT_EQ(0ul, test.esi());
  EXPECT_EQ(ExpectedT0, test.contentsOfDword(T0));
  EXPECT_EQ(ExpectedT1, test.contentsOfDword(T1));
  EXPECT_EQ(ExpectedT2, test.contentsOfDword(T2));
  EXPECT_EQ(ExpectedT3, test.contentsOfDword(T3));
}

TEST_F(AssemblerX8632Test, MovMemReg) {
  const uint32_t T0 = allocateDword();
  constexpr uint32_t ExpectedT0 = 0x00111100ul;
  const uint32_t T1 = allocateDword();
  constexpr uint32_t ExpectedT1 = 0x00222200ul;
  const uint32_t T2 = allocateDword();
  constexpr uint32_t ExpectedT2 = 0x00333300ul;
  const uint32_t T3 = allocateDword();
  constexpr uint32_t ExpectedT3 = 0x00444400ul;
  const uint32_t T4 = allocateDword();
  constexpr uint32_t ExpectedT4 = 0x00555500ul;
  const uint32_t T5 = allocateDword();
  constexpr uint32_t ExpectedT5 = 0x00666600ul;

  __ mov(IceType_i32, GPRRegister::Encoded_Reg_eax, Immediate(ExpectedT0));
  __ mov(IceType_i32, dwordAddress(T0), GPRRegister::Encoded_Reg_eax);
  __ mov(IceType_i32, GPRRegister::Encoded_Reg_ebx, Immediate(ExpectedT1));
  __ mov(IceType_i32, dwordAddress(T1), GPRRegister::Encoded_Reg_ebx);
  __ mov(IceType_i32, GPRRegister::Encoded_Reg_ecx, Immediate(ExpectedT2));
  __ mov(IceType_i32, dwordAddress(T2), GPRRegister::Encoded_Reg_ecx);
  __ mov(IceType_i32, GPRRegister::Encoded_Reg_edx, Immediate(ExpectedT3));
  __ mov(IceType_i32, dwordAddress(T3), GPRRegister::Encoded_Reg_edx);
  __ mov(IceType_i32, GPRRegister::Encoded_Reg_edi, Immediate(ExpectedT4));
  __ mov(IceType_i32, dwordAddress(T4), GPRRegister::Encoded_Reg_edi);
  __ mov(IceType_i32, GPRRegister::Encoded_Reg_esi, Immediate(ExpectedT5));
  __ mov(IceType_i32, dwordAddress(T5), GPRRegister::Encoded_Reg_esi);

  AssembledBuffer test = assemble();
  test.run();
  EXPECT_EQ(ExpectedT0, test.contentsOfDword(T0));
  EXPECT_EQ(ExpectedT1, test.contentsOfDword(T1));
  EXPECT_EQ(ExpectedT2, test.contentsOfDword(T2));
  EXPECT_EQ(ExpectedT3, test.contentsOfDword(T3));
  EXPECT_EQ(ExpectedT4, test.contentsOfDword(T4));
  EXPECT_EQ(ExpectedT5, test.contentsOfDword(T5));
}

TEST_F(AssemblerX8632Test, MovRegReg) {
  __ mov(IceType_i32, GPRRegister::Encoded_Reg_eax, Immediate(0x20));
  __ mov(IceType_i32, GPRRegister::Encoded_Reg_ebx,
         GPRRegister::Encoded_Reg_eax);
  __ mov(IceType_i32, GPRRegister::Encoded_Reg_ecx,
         GPRRegister::Encoded_Reg_ebx);
  __ mov(IceType_i32, GPRRegister::Encoded_Reg_edx,
         GPRRegister::Encoded_Reg_ecx);
  __ mov(IceType_i32, GPRRegister::Encoded_Reg_edi,
         GPRRegister::Encoded_Reg_edx);
  __ mov(IceType_i32, GPRRegister::Encoded_Reg_esi,
         GPRRegister::Encoded_Reg_edi);

  __ mov(IceType_i32, GPRRegister::Encoded_Reg_esi, Immediate(0x55000000ul));
  __ mov(IceType_i32, GPRRegister::Encoded_Reg_eax,
         GPRRegister::Encoded_Reg_esi);

  AssembledBuffer test = assemble();
  test.run();
  EXPECT_EQ(0x55000000ul, test.eax());
  EXPECT_EQ(0x20ul, test.ebx());
  EXPECT_EQ(0x20ul, test.ecx());
  EXPECT_EQ(0x20ul, test.edx());
  EXPECT_EQ(0x20ul, test.edi());
  EXPECT_EQ(0x55000000ul, test.esi());
}

TEST_F(AssemblerX8632Test, MovRegMem) {
  const uint32_t T0 = allocateDword();
  constexpr uint32_t ExpectedT0 = 0x00111100ul;
  const uint32_t T1 = allocateDword();
  constexpr uint32_t ExpectedT1 = 0x00222200ul;
  const uint32_t T2 = allocateDword();
  constexpr uint32_t ExpectedT2 = 0x00333300ul;
  const uint32_t T3 = allocateDword();
  constexpr uint32_t ExpectedT3 = 0x00444400ul;
  const uint32_t T4 = allocateDword();
  constexpr uint32_t ExpectedT4 = 0x00555500ul;
  const uint32_t T5 = allocateDword();
  constexpr uint32_t ExpectedT5 = 0x00666600ul;

  __ mov(IceType_i32, dwordAddress(T0), Immediate(ExpectedT0));
  __ mov(IceType_i32, GPRRegister::Encoded_Reg_eax, dwordAddress(T0));

  __ mov(IceType_i32, dwordAddress(T1), Immediate(ExpectedT1));
  __ mov(IceType_i32, GPRRegister::Encoded_Reg_ebx, dwordAddress(T1));

  __ mov(IceType_i32, dwordAddress(T2), Immediate(ExpectedT2));
  __ mov(IceType_i32, GPRRegister::Encoded_Reg_ecx, dwordAddress(T2));

  __ mov(IceType_i32, dwordAddress(T3), Immediate(ExpectedT3));
  __ mov(IceType_i32, GPRRegister::Encoded_Reg_edx, dwordAddress(T3));

  __ mov(IceType_i32, dwordAddress(T4), Immediate(ExpectedT4));
  __ mov(IceType_i32, GPRRegister::Encoded_Reg_edi, dwordAddress(T4));

  __ mov(IceType_i32, dwordAddress(T5), Immediate(ExpectedT5));
  __ mov(IceType_i32, GPRRegister::Encoded_Reg_esi, dwordAddress(T5));

  AssembledBuffer test = assemble();
  test.run();
  EXPECT_EQ(ExpectedT0, test.eax());
  EXPECT_EQ(ExpectedT1, test.ebx());
  EXPECT_EQ(ExpectedT2, test.ecx());
  EXPECT_EQ(ExpectedT3, test.edx());
  EXPECT_EQ(ExpectedT4, test.edi());
  EXPECT_EQ(ExpectedT5, test.esi());
}

TEST_F(AssemblerX8632Test, J) {
#define TestJ(C, Near, Src0, Value0, Src1, Value1, Dest)                       \
  do {                                                                         \
    const bool NearJmp = std::strcmp(#Near, "Near") == 0;                      \
    Label ShouldBeTaken;                                                       \
    __ mov(IceType_i32, GPRRegister::Encoded_Reg_##Src0, Immediate(Value0));   \
    __ mov(IceType_i32, GPRRegister::Encoded_Reg_##Src1, Immediate(Value1));   \
    __ mov(IceType_i32, GPRRegister::Encoded_Reg_##Dest, Immediate(0xBEEF));   \
    __ cmp(IceType_i32, GPRRegister::Encoded_Reg_##Src0,                       \
           GPRRegister::Encoded_Reg_##Src1);                                   \
    __ j(Cond::Br_##C, &ShouldBeTaken, NearJmp);                               \
    __ mov(IceType_i32, GPRRegister::Encoded_Reg_##Dest, Immediate(0xC0FFEE)); \
    __ bind(&ShouldBeTaken);                                                   \
    AssembledBuffer test = assemble();                                         \
    test.run();                                                                \
    EXPECT_EQ(Value0, test.Src0()) << "Br_" #C ", " #Near;                     \
    EXPECT_EQ(Value1, test.Src1()) << "Br_" #C ", " #Near;                     \
    EXPECT_EQ(0xBEEFul, test.Dest()) << "Br_" #C ", " #Near;                   \
    reset();                                                                   \
  } while (0)

  TestJ(o, Near, eax, 0x80000000ul, ebx, 0x1ul, ecx);
  TestJ(o, Far, ebx, 0x80000000ul, ecx, 0x1ul, edx);
  TestJ(no, Near, ecx, 0x1ul, edx, 0x1ul, edi);
  TestJ(no, Far, edx, 0x1ul, edi, 0x1ul, esi);
  TestJ(b, Near, edi, 0x1ul, esi, 0x80000000ul, eax);
  TestJ(b, Far, esi, 0x1ul, eax, 0x80000000ul, ebx);
  TestJ(ae, Near, eax, 0x80000000ul, ebx, 0x1ul, ecx);
  TestJ(ae, Far, ebx, 0x80000000ul, ecx, 0x1ul, edx);
  TestJ(e, Near, ecx, 0x80000000ul, edx, 0x80000000ul, edi);
  TestJ(e, Far, edx, 0x80000000ul, edi, 0x80000000ul, esi);
  TestJ(ne, Near, edi, 0x80000000ul, esi, 0x1ul, eax);
  TestJ(ne, Far, esi, 0x80000000ul, eax, 0x1ul, ebx);
  TestJ(be, Near, eax, 0x1ul, ebx, 0x80000000ul, ecx);
  TestJ(be, Far, ebx, 0x1ul, ecx, 0x80000000ul, edx);
  TestJ(a, Near, ecx, 0x80000000ul, edx, 0x1ul, edi);
  TestJ(a, Far, edx, 0x80000000ul, edi, 0x1ul, esi);
  TestJ(s, Near, edi, 0x1ul, esi, 0x80000000ul, eax);
  TestJ(s, Far, esi, 0x1ul, eax, 0x80000000ul, ebx);
  TestJ(ns, Near, eax, 0x80000000ul, ebx, 0x1ul, ecx);
  TestJ(ns, Far, ebx, 0x80000000ul, ecx, 0x1ul, edx);
  TestJ(p, Near, ecx, 0x80000000ul, edx, 0x1ul, edi);
  TestJ(p, Far, edx, 0x80000000ul, edi, 0x1ul, esi);
  TestJ(np, Near, edi, 0x1ul, esi, 0x80000000ul, eax);
  TestJ(np, Far, esi, 0x1ul, eax, 0x80000000ul, ebx);
  TestJ(l, Near, eax, 0x80000000ul, ebx, 0x1ul, ecx);
  TestJ(l, Far, ebx, 0x80000000ul, ecx, 0x1ul, edx);
  TestJ(ge, Near, ecx, 0x1ul, edx, 0x80000000ul, edi);
  TestJ(ge, Far, edx, 0x1ul, edi, 0x80000000ul, esi);
  TestJ(le, Near, edi, 0x80000000ul, esi, 0x1ul, eax);
  TestJ(le, Far, esi, 0x80000000ul, eax, 0x1ul, ebx);
  TestJ(g, Near, eax, 0x1ul, ebx, 0x80000000ul, ecx);
  TestJ(g, Far, ebx, 0x1ul, ecx, 0x80000000ul, edx);

#undef TestJ
}

#undef __

} // end of anonymous namespace
} // end of namespace X8632
} // end of namespace Ice
