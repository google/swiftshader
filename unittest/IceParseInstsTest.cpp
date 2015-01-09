//===- unittest/IceParseInstsTest.cpp - test instruction errors -----------===//
//
//                        The Subzero Code Generator
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "llvm/ADT/STLExtras.h"
#include "llvm/Bitcode/NaCl/NaClBitcodeParser.h"
#include "llvm/Bitcode/NaCl/NaClLLVMBitCodes.h"

#include "BitcodeMunge.h"

#include "gtest/gtest.h"

using namespace llvm;

namespace {

static const uint64_t Terminator = 0x5768798008978675LL;

/// Test how we report a call arg that refers to nonexistent call argument
TEST(IceParseInstsTest, NonexistentCallArg) {
  const uint64_t BitcodeRecords[] = {
    1, naclbitc::BLK_CODE_ENTER, naclbitc::MODULE_BLOCK_ID, 2, Terminator,
    1, naclbitc::BLK_CODE_ENTER, naclbitc::TYPE_BLOCK_ID_NEW, 2, Terminator,
    3, naclbitc::TYPE_CODE_NUMENTRY, 3, Terminator,
    3, naclbitc::TYPE_CODE_INTEGER, 32, Terminator,
    3, naclbitc::TYPE_CODE_VOID, Terminator,
    3, naclbitc::TYPE_CODE_FUNCTION, 0, 1, 0, 0, Terminator,
    0, naclbitc::BLK_CODE_EXIT, Terminator,
    3, naclbitc::MODULE_CODE_FUNCTION, 2, 0, 1, 0, Terminator,
    3, naclbitc::MODULE_CODE_FUNCTION, 2, 0, 0, 0, Terminator,
    1, naclbitc::BLK_CODE_ENTER, naclbitc::FUNCTION_BLOCK_ID, 2, Terminator,
    3, naclbitc::FUNC_CODE_DECLAREBLOCKS, 1, Terminator,
    // Note: 100 is a bad value index in next line.
    3, naclbitc::FUNC_CODE_INST_CALL, 0, 4, 2, 100, Terminator,
    3, naclbitc::FUNC_CODE_INST_RET, Terminator,
    0, naclbitc::BLK_CODE_EXIT, Terminator,
    0, naclbitc::BLK_CODE_EXIT, Terminator
  };


  // Show bitcode objdump for BitcodeRecords.
  NaClObjDumpMunger DumpMunger(BitcodeRecords,
                               array_lengthof(BitcodeRecords), Terminator);
  EXPECT_FALSE(DumpMunger.runTestForAssembly("Nonexistent call arg"));
  EXPECT_EQ(
      "module {  // BlockID = 8\n"
      "  types {  // BlockID = 17\n"
      "    count 3;\n"
      "    @t0 = i32;\n"
      "    @t1 = void;\n"
      "    @t2 = void (i32, i32);\n"
      "  }\n"
      "  declare external void @f0(i32, i32);\n"
      "  define external void @f1(i32, i32);\n"
      "  function void @f1(i32 %p0, i32 %p1) {  // BlockID = 12\n"
      "    blocks 1;\n"
      "  %b0:\n"
      "    call void @f0(i32 %p0, i32 @f0);\n"
      "Error(66:4): Invalid relative value id: 100 (Must be <= 4)\n"
      "    ret void;\n"
      "  }\n"
      "}\n",
      DumpMunger.getTestResults());

  // Show that we get appropriate error when parsing in Subzero.
  IceTest::SubzeroBitcodeMunger Munger(
      BitcodeRecords, array_lengthof(BitcodeRecords), Terminator);
  EXPECT_FALSE(Munger.runTest("Nonexistent call arg"));
  EXPECT_EQ(
      "Error: (66:4) Invalid function record: <34 0 4 2 100>\n",
      Munger.getTestResults());
}

} // end of anonymous namespace
