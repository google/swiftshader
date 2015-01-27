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

// Note: alignment stored as 0 or log2(Alignment)+1.
uint64_t getEncAlignPower(unsigned Power) {
  return Power + 1;
}
uint64_t getEncAlignZero() { return 0; }

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

/// Test how we recognize alignments in alloca instructions.
TEST(IceParseInstsTests, AllocaAlignment) {
  const uint64_t BitcodeRecords[] = {
    1, naclbitc::BLK_CODE_ENTER, naclbitc::MODULE_BLOCK_ID, 2, Terminator,
    1,   naclbitc::BLK_CODE_ENTER, naclbitc::TYPE_BLOCK_ID_NEW, 2, Terminator,
    3,     naclbitc::TYPE_CODE_NUMENTRY, 4, Terminator,
    3,     naclbitc::TYPE_CODE_INTEGER, 32, Terminator,
    3,     naclbitc::TYPE_CODE_VOID, Terminator,
    3,     naclbitc::TYPE_CODE_FUNCTION, 0, 1, 0, Terminator,
    3,     naclbitc::TYPE_CODE_INTEGER, 8, Terminator,
    0,   naclbitc::BLK_CODE_EXIT, Terminator,
    3,   naclbitc::MODULE_CODE_FUNCTION, 2, 0, 0, 0, Terminator,
    1,   naclbitc::BLK_CODE_ENTER, naclbitc::FUNCTION_BLOCK_ID, 2, Terminator,
    3,     naclbitc::FUNC_CODE_DECLAREBLOCKS, 1, Terminator,
    3,     naclbitc::FUNC_CODE_INST_ALLOCA, 1, getEncAlignPower(0), Terminator,
    3,     naclbitc::FUNC_CODE_INST_RET, Terminator,
    0,   naclbitc::BLK_CODE_EXIT, Terminator,
    0, naclbitc::BLK_CODE_EXIT, Terminator};

  const uint64_t ReplaceIndex = 11; // index for FUNC_CODE_INST_ALLOCA

  // Show text when alignment is 1.
  NaClObjDumpMunger DumpMunger(BitcodeRecords, array_lengthof(BitcodeRecords),
                               Terminator);
  EXPECT_TRUE(DumpMunger.runTestForAssembly("Good alloca alignment 1"));
  EXPECT_EQ("module {  // BlockID = 8\n"
            "  types {  // BlockID = 17\n"
            "    count 4;\n"
            "    @t0 = i32;\n"
            "    @t1 = void;\n"
            "    @t2 = void (i32);\n"
            "    @t3 = i8;\n"
            "  }\n"
            "  define external void @f0(i32);\n"
            "  function void @f0(i32 %p0) {  // BlockID = 12\n"
            "    blocks 1;\n"
            "  %b0:\n"
            "    %v0 = alloca i8, i32 %p0, align 1;\n"
            "    ret void;\n"
            "  }\n"
            "}\n",
            DumpMunger.getTestResults());

  // Show that we can handle alignment of 1.
  IceTest::SubzeroBitcodeMunger Munger(
      BitcodeRecords, array_lengthof(BitcodeRecords), Terminator);
  EXPECT_TRUE(Munger.runTest("Good alloca alignment 1"));

  // Show what happens when changing alignment to 0.
  const uint64_t Align0[] = {
      ReplaceIndex, NaClBitcodeMunger::Replace,
      3, naclbitc::FUNC_CODE_INST_ALLOCA, 1, getEncAlignZero(), Terminator,
  };
  EXPECT_TRUE(Munger.runTest("Good alloca alignment 0", Align0,
                             array_lengthof(Align0)));
  EXPECT_TRUE(DumpMunger.runTestForAssembly("Good alloca alignment 0", Align0,
                                            array_lengthof(Align0)));
  EXPECT_EQ("    %v0 = alloca i8, i32 %p0, align 0;\n",
            DumpMunger.getLinesWithSubstring("alloca"));

  // Show what happens when changing alignment to 2**30.
  const uint64_t Align30[] = {
      ReplaceIndex, NaClBitcodeMunger::Replace,
      3, naclbitc::FUNC_CODE_INST_ALLOCA, 1, getEncAlignPower(30), Terminator,
  };
  EXPECT_FALSE(Munger.runTest("Bad alloca alignment 30", Align30,
                              array_lengthof(Align30)));
  EXPECT_EQ("Error: (62:4) Invalid function record: <19 1 31>\n",
            Munger.getTestResults());

  EXPECT_FALSE(DumpMunger.runTestForAssembly("Bad alloca alignment 30", Align30,
                                             array_lengthof(Align30)));
  EXPECT_EQ("    %v0 = alloca i8, i32 %p0, align 0;\n",
            DumpMunger.getLinesWithSubstring("alloca"));
  EXPECT_EQ(
      "Error(62:4): Alignment can't be greater than 2**29. Found: 2**30\n",
      DumpMunger.getLinesWithSubstring("Error"));

  // Show what happens when changing alignment to 2**29.
  const uint64_t Align29[] = {
      ReplaceIndex, NaClBitcodeMunger::Replace,
      3, naclbitc::FUNC_CODE_INST_ALLOCA, 1, getEncAlignPower(29), Terminator,
  };
  EXPECT_TRUE(Munger.runTest("Good alloca alignment 29", Align29,
                             array_lengthof(Align29)));
  EXPECT_TRUE(DumpMunger.runTestForAssembly("Good alloca alignment 29", Align29,
                                            array_lengthof(Align29)));
  EXPECT_EQ("    %v0 = alloca i8, i32 %p0, align 536870912;\n",
            DumpMunger.getLinesWithSubstring("alloca"));
}

// Test how we recognize alignments in load i32 instructions.
TEST(IceParseInstsTests, LoadI32Alignment) {
  const uint64_t BitcodeRecords[] = {
    1, naclbitc::BLK_CODE_ENTER, naclbitc::MODULE_BLOCK_ID, 2, Terminator,
    1,   naclbitc::BLK_CODE_ENTER, naclbitc::TYPE_BLOCK_ID_NEW, 2, Terminator,
    3,     naclbitc::TYPE_CODE_NUMENTRY, 2, Terminator,
    3,     naclbitc::TYPE_CODE_INTEGER, 32, Terminator,
    3,     naclbitc::TYPE_CODE_FUNCTION, 0, 0, 0, Terminator,
    0,   naclbitc::BLK_CODE_EXIT, Terminator,
    3,   naclbitc::MODULE_CODE_FUNCTION, 1, 0, 0, 0, Terminator,
    1,   naclbitc::BLK_CODE_ENTER, naclbitc::FUNCTION_BLOCK_ID, 2, Terminator,
    3,     naclbitc::FUNC_CODE_DECLAREBLOCKS, 1, Terminator,
    3,     naclbitc::FUNC_CODE_INST_LOAD, 1, getEncAlignPower(0), 0, Terminator,
    3,     naclbitc::FUNC_CODE_INST_RET, 1, Terminator,
    0,   naclbitc::BLK_CODE_EXIT, Terminator,
    0, naclbitc::BLK_CODE_EXIT, Terminator};

  const uint64_t ReplaceIndex = 9; // index for FUNC_CODE_INST_LOAD

  // Show text when alignment is 1.
  NaClObjDumpMunger DumpMunger(BitcodeRecords, array_lengthof(BitcodeRecords),
                               Terminator);
  EXPECT_TRUE(DumpMunger.runTestForAssembly("Good load i32 alignment 1"));
  EXPECT_EQ("module {  // BlockID = 8\n"
            "  types {  // BlockID = 17\n"
            "    count 2;\n"
            "    @t0 = i32;\n"
            "    @t1 = i32 (i32);\n"
            "  }\n"
            "  define external i32 @f0(i32);\n"
            "  function i32 @f0(i32 %p0) {  // BlockID = 12\n"
            "    blocks 1;\n"
            "  %b0:\n"
            "    %v0 = load i32* %p0, align 1;\n"
            "    ret i32 %v0;\n"
            "  }\n"
            "}\n",
            DumpMunger.getTestResults());
  IceTest::SubzeroBitcodeMunger Munger(
      BitcodeRecords, array_lengthof(BitcodeRecords), Terminator);
  EXPECT_TRUE(Munger.runTest("Good load i32 alignment 1"));

  // Show what happens when changing alignment to 0.
  const uint64_t Align0[] = {
      ReplaceIndex, NaClBitcodeMunger::Replace,
      3, naclbitc::FUNC_CODE_INST_LOAD, 1, getEncAlignZero(), 0, Terminator,
  };
  EXPECT_FALSE(Munger.runTest("Bad load i32 alignment 0", Align0,
                              array_lengthof(Align0)));
  EXPECT_EQ("Error: (58:4) Invalid function record: <20 1 0 0>\n",
            Munger.getTestResults());
  EXPECT_FALSE(DumpMunger.runTestForAssembly("Bad load i32 alignment 0", Align0,
                                             array_lengthof(Align0)));
  EXPECT_EQ("    %v0 = load i32* %p0, align 0;\n"
            "Error(58:4): load: Illegal alignment for i32. Expects: 1\n",
            DumpMunger.getLinesWithSubstring("load"));

  // Show what happens when changing alignment to 4.
  const uint64_t Align4[] = {
      ReplaceIndex, NaClBitcodeMunger::Replace,
      3, naclbitc::FUNC_CODE_INST_LOAD, 1, getEncAlignPower(2), 0, Terminator,
  };
  EXPECT_FALSE(Munger.runTest("Bad load i32 alignment 4", Align4,
                              array_lengthof(Align4)));
  EXPECT_EQ("Error: (58:4) Invalid function record: <20 1 3 0>\n",
            Munger.getTestResults());
  EXPECT_FALSE(DumpMunger.runTestForAssembly("Bad load i32 alignment 4", Align4,
                                             array_lengthof(Align4)));
  EXPECT_EQ("    %v0 = load i32* %p0, align 4;\n"
            "Error(58:4): load: Illegal alignment for i32. Expects: 1\n",
            DumpMunger.getLinesWithSubstring("load"));

  // Show what happens when changing alignment to 2**29.
  const uint64_t Align29[] = {
      ReplaceIndex, NaClBitcodeMunger::Replace,
      3, naclbitc::FUNC_CODE_INST_LOAD, 1, getEncAlignPower(29), 0, Terminator,
  };
  EXPECT_FALSE(Munger.runTest("Bad load i32 alignment 29", Align29,
                              array_lengthof(Align29)));
  EXPECT_EQ("Error: (58:4) Invalid function record: <20 1 30 0>\n",
            Munger.getTestResults());
  EXPECT_FALSE(DumpMunger.runTestForAssembly("Bad load i32 alignment 29",
                                             Align29, array_lengthof(Align29)));
  EXPECT_EQ("    %v0 = load i32* %p0, align 536870912;\n"
            "Error(58:4): load: Illegal alignment for i32. Expects: 1\n",
            DumpMunger.getLinesWithSubstring("load"));

  // Show what happens when changing alignment to 2**30.
  const uint64_t Align30[] = {
      ReplaceIndex, NaClBitcodeMunger::Replace,
      3, naclbitc::FUNC_CODE_INST_LOAD, 1, getEncAlignPower(30), 0, Terminator,
  };
  EXPECT_FALSE(Munger.runTest("Bad load i32 alignment 30", Align30,
                              array_lengthof(Align30)));
  EXPECT_EQ("Error: (58:4) Invalid function record: <20 1 31 0>\n",
            Munger.getTestResults());
  EXPECT_FALSE(DumpMunger.runTestForAssembly("Bad load i32 alignment 30",
                                             Align30, array_lengthof(Align30)));
  EXPECT_EQ("    %v0 = load i32* %p0, align 0;\n"
            "Error(58:4): load: Illegal alignment for i32. Expects: 1\n",
            DumpMunger.getLinesWithSubstring("load"));
}

// Test how we recognize alignments in load float instructions.
TEST(IceParseInstsTests, LoadFloatAlignment) {
  const uint64_t BitcodeRecords[] = {
    1, naclbitc::BLK_CODE_ENTER, naclbitc::MODULE_BLOCK_ID, 2, Terminator,
    1,   naclbitc::BLK_CODE_ENTER, naclbitc::TYPE_BLOCK_ID_NEW, 2, Terminator,
    3,     naclbitc::TYPE_CODE_NUMENTRY, 3, Terminator,
    3,     naclbitc::TYPE_CODE_FLOAT, Terminator,
    3,     naclbitc::TYPE_CODE_INTEGER, 32, Terminator,
    3,     naclbitc::TYPE_CODE_FUNCTION, 0, 0, 1, Terminator,
    0,   naclbitc::BLK_CODE_EXIT, Terminator,
    3,   naclbitc::MODULE_CODE_FUNCTION, 2, 0, 0, 0, Terminator,
    1,   naclbitc::BLK_CODE_ENTER, naclbitc::FUNCTION_BLOCK_ID, 2, Terminator,
    3,     naclbitc::FUNC_CODE_DECLAREBLOCKS, 1, Terminator,
    3,     naclbitc::FUNC_CODE_INST_LOAD, 1, getEncAlignPower(0), 0, Terminator,
    3,     naclbitc::FUNC_CODE_INST_RET, 1, Terminator,
    0,   naclbitc::BLK_CODE_EXIT, Terminator,
    0, naclbitc::BLK_CODE_EXIT, Terminator};

  const uint64_t ReplaceIndex = 10; // index for FUNC_CODE_INST_LOAD

  // Show text when alignment is 1.
  NaClObjDumpMunger DumpMunger(BitcodeRecords, array_lengthof(BitcodeRecords),
                               Terminator);
  EXPECT_TRUE(DumpMunger.runTestForAssembly("Good load float alignment 1"));
  EXPECT_EQ("module {  // BlockID = 8\n"
            "  types {  // BlockID = 17\n"
            "    count 3;\n"
            "    @t0 = float;\n"
            "    @t1 = i32;\n"
            "    @t2 = float (i32);\n"
            "  }\n"
            "  define external float @f0(i32);\n"
            "  function float @f0(i32 %p0) {  // BlockID = 12\n"
            "    blocks 1;\n"
            "  %b0:\n"
            "    %v0 = load float* %p0, align 1;\n"
            "    ret float %v0;\n"
            "  }\n"
            "}\n",
            DumpMunger.getTestResults());
  IceTest::SubzeroBitcodeMunger Munger(
      BitcodeRecords, array_lengthof(BitcodeRecords), Terminator);
  EXPECT_TRUE(Munger.runTest("Good load float alignment 1"));

  // Show what happens when changing alignment to 0.
  const uint64_t Align0[] = {
      ReplaceIndex, NaClBitcodeMunger::Replace,
      3, naclbitc::FUNC_CODE_INST_LOAD, 1, getEncAlignZero(), 0, Terminator,
  };
  EXPECT_FALSE(Munger.runTest("Bad load float alignment 0", Align0,
                              array_lengthof(Align0)));
  EXPECT_EQ("Error: (58:4) Invalid function record: <20 1 0 0>\n",
            Munger.getTestResults());
  EXPECT_FALSE(DumpMunger.runTestForAssembly("Bad load float alignment 0",
                                             Align0, array_lengthof(Align0)));
  EXPECT_EQ(
      "    %v0 = load float* %p0, align 0;\n"
            "Error(58:4): load: Illegal alignment for float. Expects: 1 or 4\n",
            DumpMunger.getLinesWithSubstring("load"));

  // Show what happens when changing alignment to 4.
  const uint64_t Align4[] = {
      ReplaceIndex, NaClBitcodeMunger::Replace,
      3, naclbitc::FUNC_CODE_INST_LOAD, 1, getEncAlignPower(2), 0, Terminator,
  };
  EXPECT_TRUE(Munger.runTest("Good load float alignment 4", Align4,
                             array_lengthof(Align4)));
  EXPECT_TRUE(DumpMunger.runTestForAssembly("Good load float alignment 4",
                                            Align4, array_lengthof(Align4)));
  EXPECT_EQ("    %v0 = load float* %p0, align 4;\n",
            DumpMunger.getLinesWithSubstring("load"));

  const uint64_t Align29[] = {
      ReplaceIndex, NaClBitcodeMunger::Replace,
      3, naclbitc::FUNC_CODE_INST_LOAD, 1, getEncAlignPower(29), 0, Terminator,
  };
  EXPECT_FALSE(Munger.runTest("Bad load float alignment 29", Align29,
                              array_lengthof(Align29)));
  EXPECT_EQ("Error: (58:4) Invalid function record: <20 1 30 0>\n",
            Munger.getTestResults());
  EXPECT_FALSE(DumpMunger.runTestForAssembly("Bad load float alignment 29",
                                             Align29, array_lengthof(Align29)));
  EXPECT_EQ(
      "    %v0 = load float* %p0, align 536870912;\n"
            "Error(58:4): load: Illegal alignment for float. Expects: 1 or 4\n",
            DumpMunger.getLinesWithSubstring("load"));

  // Show what happens when changing alignment to 2**30.
  const uint64_t Align30[] = {
      ReplaceIndex, NaClBitcodeMunger::Replace,
      3, naclbitc::FUNC_CODE_INST_LOAD, 1, getEncAlignPower(30), 0, Terminator,
  };
  EXPECT_FALSE(Munger.runTest("Bad load float alignment 30", Align30,
                              array_lengthof(Align30)));
  EXPECT_EQ("Error: (58:4) Invalid function record: <20 1 31 0>\n",
            Munger.getTestResults());
  EXPECT_FALSE(DumpMunger.runTestForAssembly("Bad load float alignment 30",
                                             Align30, array_lengthof(Align30)));
  EXPECT_EQ(
      "    %v0 = load float* %p0, align 0;\n"
            "Error(58:4): load: Illegal alignment for float. Expects: 1 or 4\n",
            DumpMunger.getLinesWithSubstring("load"));
}

// Test how we recognize alignments in store instructions.
TEST(NaClParseInstsTests, StoreAlignment) {
  const uint64_t BitcodeRecords[] = {
  1, naclbitc::BLK_CODE_ENTER, naclbitc::MODULE_BLOCK_ID, 2, Terminator,
  1,   naclbitc::BLK_CODE_ENTER, naclbitc::TYPE_BLOCK_ID_NEW, 2, Terminator,
  3,     naclbitc::TYPE_CODE_NUMENTRY, 3, Terminator,
  3,     naclbitc::TYPE_CODE_FLOAT, Terminator,
  3,     naclbitc::TYPE_CODE_INTEGER, 32, Terminator,
  3,     naclbitc::TYPE_CODE_FUNCTION, 0, 0, 1, 0, Terminator,
  0,   naclbitc::BLK_CODE_EXIT, Terminator,
  3,   naclbitc::MODULE_CODE_FUNCTION, 2, 0, 0, 0, Terminator,
  1,   naclbitc::BLK_CODE_ENTER, naclbitc::FUNCTION_BLOCK_ID, 2, Terminator,
  3,     naclbitc::FUNC_CODE_DECLAREBLOCKS, 1, Terminator,
  3,     naclbitc::FUNC_CODE_INST_STORE, 2, 1, getEncAlignPower(0), Terminator,
  3,     naclbitc::FUNC_CODE_INST_RET, 1, Terminator,
  0,   naclbitc::BLK_CODE_EXIT, Terminator,
  0, naclbitc::BLK_CODE_EXIT, Terminator};

  const uint64_t ReplaceIndex = 10; // index for FUNC_CODE_INST_STORE

  // Show text when alignment is 1.
  NaClObjDumpMunger DumpMunger(BitcodeRecords, array_lengthof(BitcodeRecords),
                               Terminator);
  EXPECT_TRUE(DumpMunger.runTestForAssembly("Good Store Alignment 1"));
  EXPECT_EQ("module {  // BlockID = 8\n"
            "  types {  // BlockID = 17\n"
            "    count 3;\n"
            "    @t0 = float;\n"
            "    @t1 = i32;\n"
            "    @t2 = float (i32, float);\n"
            "  }\n"
            "  define external float @f0(i32, float);\n"
            "  function float @f0(i32 %p0, float %p1) {  // BlockID = 12\n"
            "    blocks 1;\n"
            "  %b0:\n"
            "    store float %p1, float* %p0, align 1;\n"
            "    ret float %p1;\n"
            "  }\n"
            "}\n",
            DumpMunger.getTestResults());
  IceTest::SubzeroBitcodeMunger Munger(
      BitcodeRecords, array_lengthof(BitcodeRecords), Terminator);
  EXPECT_TRUE(Munger.runTest("Good store alignment"));

  // Show what happens when changing alignment to 0.
  const uint64_t Align0[] = {
      ReplaceIndex, NaClBitcodeMunger::Replace,
      3, naclbitc::FUNC_CODE_INST_STORE, 2, 1, getEncAlignZero(), Terminator,
  };
  EXPECT_FALSE(
      Munger.runTest("Bad store alignment 0", Align0, array_lengthof(Align0)));
  EXPECT_EQ("Error: (62:4) Invalid function record: <24 2 1 0>\n",
            Munger.getTestResults());
  EXPECT_FALSE(DumpMunger.runTestForAssembly("Bad store alignment 0", Align0,
                                             array_lengthof(Align0)));
  EXPECT_EQ(
      "    store float %p1, float* %p0, align 0;\n"
      "Error(62:4): store: Illegal alignment for float. Expects: 1 or 4\n",
      DumpMunger.getLinesWithSubstring("store"));

  // Show what happens when changing alignment to 4.
  const uint64_t Align4[] = {
      ReplaceIndex, NaClBitcodeMunger::Replace,
      3, naclbitc::FUNC_CODE_INST_STORE, 2, 1, getEncAlignPower(2), Terminator,
  };
  EXPECT_TRUE(
      Munger.runTest("Bad store alignment 4", Align4, array_lengthof(Align4)));
  EXPECT_TRUE(DumpMunger.runTestForAssembly("Good store alignment 4", Align4,
                                            array_lengthof(Align4)));

  // Show what happens when changing alignment to 8.
  const uint64_t Align8[] = {
      ReplaceIndex, NaClBitcodeMunger::Replace,
      3, naclbitc::FUNC_CODE_INST_STORE, 2, 1, getEncAlignPower(3), Terminator,
  };
  EXPECT_FALSE(
      Munger.runTest("Bad store alignment 8", Align8, array_lengthof(Align8)));
  EXPECT_EQ("Error: (62:4) Invalid function record: <24 2 1 4>\n",
            Munger.getTestResults());
  EXPECT_FALSE(DumpMunger.runTestForAssembly("Bad store alignment 8", Align8,
                                             array_lengthof(Align8)));
  EXPECT_EQ(
      "    store float %p1, float* %p0, align 8;\n"
      "Error(62:4): store: Illegal alignment for float. Expects: 1 or 4\n",
      DumpMunger.getLinesWithSubstring("store"));

  // Show what happens when changing alignment to 2**29.
  const uint64_t Align29[] = {
      ReplaceIndex, NaClBitcodeMunger::Replace,
      3, naclbitc::FUNC_CODE_INST_STORE, 2, 1, getEncAlignPower(29), Terminator,
  };
  EXPECT_FALSE(Munger.runTest("Bad store alignment 29", Align29,
                              array_lengthof(Align29)));
  EXPECT_EQ("Error: (62:4) Invalid function record: <24 2 1 30>\n",
            Munger.getTestResults());
  EXPECT_FALSE(DumpMunger.runTestForAssembly("Bad store alignment 29", Align29,
                                             array_lengthof(Align29)));
  EXPECT_EQ(
      "    store float %p1, float* %p0, align 536870912;\n"
      "Error(62:4): store: Illegal alignment for float. Expects: 1 or 4\n",
      DumpMunger.getLinesWithSubstring("store"));

  const uint64_t Align30[] = {
      ReplaceIndex, NaClBitcodeMunger::Replace,
      // Note: alignment stored as 0 or log2(Alignment)+1.
      3, naclbitc::FUNC_CODE_INST_STORE, 2, 1, getEncAlignPower(30), Terminator,
  };
  EXPECT_FALSE(Munger.runTest("Bad store alignment 30", Align30,
                              array_lengthof(Align30)));
  EXPECT_EQ("Error: (62:4) Invalid function record: <24 2 1 31>\n",
            Munger.getTestResults());
  EXPECT_FALSE(DumpMunger.runTestForAssembly("Bad Store alignment 30", Align30,
                                             array_lengthof(Align30)));
  EXPECT_EQ(
      "    store float %p1, float* %p0, align 0;\n"
      "Error(62:4): store: Illegal alignment for float. Expects: 1 or 4\n",
      DumpMunger.getLinesWithSubstring("store"));
}

} // end of anonymous namespace
