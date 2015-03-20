//===- subzero/src/IceRNG.cpp - PRNG implementation -----------------------===//
//
//                        The Subzero Code Generator
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements the random number generator.
//
//===----------------------------------------------------------------------===//

#include <time.h>

#include "IceRNG.h"

namespace Ice {

namespace {
const unsigned MAX = 2147483647;
} // end of anonymous namespace

// TODO(wala,stichnot): Switch to RNG implementation from LLVM or C++11.
//
// TODO(wala,stichnot): Make it possible to replay the RNG sequence in a
// subsequent run, for reproducing a bug.  Print the seed in a comment
// in the asm output.  Embed the seed in the binary via metadata that an
// attacker can't introspect.
RandomNumberGenerator::RandomNumberGenerator(uint64_t Seed, llvm::StringRef)
    : State(Seed) {}

uint64_t RandomNumberGenerator::next(uint64_t Max) {
  // Lewis, Goodman, and Miller (1969)
  State = (16807 * State) % MAX;
  return State % Max;
}

bool RandomNumberGeneratorWrapper::getTrueWithProbability(float Probability) {
  return RNG.next(MAX) < Probability * MAX;
}

} // end of namespace Ice
