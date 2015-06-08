//===- BitcodeMunge.cpp - Subzero Bitcode Munger ----------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// Test harness for testing malformed bitcode files in Subzero.
//
//===----------------------------------------------------------------------===//

#include "BitcodeMunge.h"
#include "IceCfg.h"
#include "IceClFlags.h"
#include "PNaClTranslator.h"
#include "IceTypes.h"

namespace IceTest {

void IceTest::SubzeroBitcodeMunger::resetFlags() {
  Ice::ClFlags::resetClFlags(Flags);
  resetMungeFlags();
}

void IceTest::SubzeroBitcodeMunger::resetMungeFlags() {
  Flags.setAllowErrorRecovery(true);
  Flags.setGenerateUnitTestMessages(true);
  Flags.setOptLevel(Ice::Opt_m1);
  Flags.setOutFileType(Ice::FT_Iasm);
  Flags.setTargetArch(Ice::Target_X8632);
  Flags.setVerbose(Ice::IceV_Instructions);
}

bool IceTest::SubzeroBitcodeMunger::runTest(const uint64_t Munges[],
                                            size_t MungeSize) {
  const bool AddHeader = true;
  setupTest(Munges, MungeSize, AddHeader);
  Ice::GlobalContext Ctx(DumpStream, DumpStream, DumpStream, nullptr, Flags);
  Ice::PNaClTranslator Translator(&Ctx);
  const char *BufferName = "Test";
  Translator.translateBuffer(BufferName, MungedInput.get());

  cleanupTest();
  return Translator.getErrorStatus().value() == 0;
}

} // end of namespace IceTest
