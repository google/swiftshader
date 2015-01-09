//===- BitcodeMunge.h - Subzero Bitcode Munger ------------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// Test harness for testing malformed bitcode files in Subzero. Uses NaCl's
// bitcode munger to do this.
//
//===----------------------------------------------------------------------===//

#ifndef SUBZERO_UNITTEST_BITCODEMUNGE_H
#define SUBZERO_UNITTEST_BITCODEMUNGE_H

#include "llvm/Bitcode/NaCl/NaClBitcodeMunge.h"

namespace IceTest {

// Class to run tests on Subzero's bitcode parser. Runs a Subzero
// translation, using (possibly) edited bitcode record values.  For
// more details on how to represent the input arrays, see
// NaClBitcodeMunge.h.
class SubzeroBitcodeMunger : public llvm::NaClBitcodeMunger {
public:
  SubzeroBitcodeMunger(const uint64_t Records[], size_t RecordSize,
                       uint64_t RecordTerminator)
      : llvm::NaClBitcodeMunger(Records, RecordSize, RecordTerminator) {}

  /// Runs PNaClTranslator to translate bitcode records (with defined
  /// record Munges), and puts output into DumpResults. Returns true
  /// if parse is successful.
  bool runTest(const char* TestName, const uint64_t Munges[],
               size_t MungeSize);

  /// Same as above, but without any edits.
  bool runTest(const char* TestName) {
    uint64_t NoMunges[] = {0};
    return runTest(TestName, NoMunges, 0);
  }
};

} // end of namespace IceTest

#endif // SUBZERO_UNITTEST_BITCODEMUNGE_H
