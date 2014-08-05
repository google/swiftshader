//===- subzero/crosstest/test_global.cpp - Global variable access tests ---===//
//
//                        The Subzero Code Generator
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// Implementation for crosstesting global variable access operations.
//
//===----------------------------------------------------------------------===//

#include <stdint.h>
#include <cstdlib>

#include "test_global.h"

// Partially initialized array
int ArrayInitPartial[10] = { 60, 70, 80, 90, 100 };
int ArrayInitFull[] = { 10, 20, 30, 40, 50 };
const int ArrayConst[] = { -10, -20, -30 };
static double ArrayDouble[10] = { 0.5, 1.5, 2.5, 3.5 };

#if 0
#define ARRAY(a)                                                               \
  { (uint8_t *)(a), sizeof(a) }

struct {
  uint8_t *ArrayAddress;
  size_t ArraySizeInBytes;
} Arrays[] = {
  ARRAY(ArrayInitPartial),
  ARRAY(ArrayInitFull),
  ARRAY(ArrayConst),
  ARRAY(ArrayDouble),
};
size_t NumArraysElements = sizeof(Arrays) / sizeof(*Arrays);
#endif // 0

size_t getNumArrays() {
  return 4;
  // return NumArraysElements;
}

const uint8_t *getArray(size_t WhichArray, size_t &Len) {
  // Using a switch statement instead of a table lookup because such a
  // table is represented as a kind of initializer that Subzero
  // doesn't yet support.  Specifically, the table becomes constant
  // aggregate data, and it contains relocations.  TODO(stichnot):
  // switch over to the cleaner table-based method when global
  // initializers are fully implemented.
  switch (WhichArray) {
  default:
    Len = -1;
    return NULL;
  case 0:
    Len = sizeof(ArrayInitPartial);
    return (uint8_t *)&ArrayInitPartial;
  case 1:
    Len = sizeof(ArrayInitFull);
    return (uint8_t *)&ArrayInitFull;
  case 2:
    Len = sizeof(ArrayConst);
    return (uint8_t *)&ArrayConst;
  case 3:
    Len = sizeof(ArrayDouble);
    return (uint8_t *)&ArrayDouble;
  }
#if 0
  if (WhichArray >= NumArraysElements) {
    Len = -1;
    return NULL;
  }
  Len = Arrays[WhichArray].ArraySizeInBytes;
  return Arrays[WhichArray].ArrayAddress;
#endif // 0
}
