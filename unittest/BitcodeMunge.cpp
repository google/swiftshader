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

bool IceTest::SubzeroBitcodeMunger::runTest(const char *TestName,
                                            const uint64_t Munges[],
                                            size_t MungeSize) {
  const bool AddHeader = true;
  setupTest(TestName, Munges, MungeSize, AddHeader);

  Ice::ClFlags Flags;
  Flags.setAllowErrorRecovery(true);
  Flags.setGenerateUnitTestMessages(true);
  Flags.setOutFileType(Ice::FT_Iasm);
  Ice::GlobalContext Ctx(DumpStream, DumpStream, nullptr,
                         Ice::IceV_Instructions, Ice::Target_X8632, Ice::Opt_m1,
                         "", Flags);
  Ice::PNaClTranslator Translator(&Ctx);
  Translator.translateBuffer(TestName, MungedInput.get());

  cleanupTest();
  return Translator.getErrorStatus().value() == 0;
}

} // end of namespace IceTest
