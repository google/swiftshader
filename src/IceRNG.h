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

namespace Ice {

class RandomNumberGenerator {
public:
  RandomNumberGenerator(llvm::StringRef Salt);
  uint64_t next(uint64_t Max);

private:
  uint64_t State;
};

} // end of namespace Ice

#endif // SUBZERO_SRC_ICERNG_H
