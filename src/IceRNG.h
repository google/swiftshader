//===- subzero/src/IceRNG.h - Random number generator -----------*- C++ -*-===//
//
//                        The Subzero Code Generator
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares a random number generator.
//
//===----------------------------------------------------------------------===//

#ifndef SUBZERO_SRC_ICERNG_H
#define SUBZERO_SRC_ICERNG_H

#include <stdint.h>
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/Compiler.h"

namespace Ice {

class RandomNumberGenerator {
public:
  RandomNumberGenerator(llvm::StringRef Salt);
  uint64_t next(uint64_t Max);

private:
  RandomNumberGenerator(const RandomNumberGenerator &) LLVM_DELETED_FUNCTION;
  RandomNumberGenerator &
  operator=(const RandomNumberGenerator &) LLVM_DELETED_FUNCTION;

  uint64_t State;
};

// This class adds additional random number generator utilities. The
// reason for the wrapper class is that we want to keep the
// RandomNumberGenerator interface identical to LLVM's.
class RandomNumberGeneratorWrapper {
public:
  uint64_t next(uint64_t Max) { return RNG.next(Max); }
  bool getTrueWithProbability(float Probability);
  RandomNumberGeneratorWrapper(RandomNumberGenerator &RNG) : RNG(RNG) {}

private:
  RandomNumberGeneratorWrapper(const RandomNumberGeneratorWrapper &)
      LLVM_DELETED_FUNCTION;
  RandomNumberGeneratorWrapper &
  operator=(const RandomNumberGeneratorWrapper &) LLVM_DELETED_FUNCTION;

  RandomNumberGenerator &RNG;
};

} // end of namespace Ice

#endif // SUBZERO_SRC_ICERNG_H
