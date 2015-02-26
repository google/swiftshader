//===- subzero/runtime/szrt.c - Subzero runtime source ----------*- C++ -*-===//
//
//                        The Subzero Code Generator
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements wrappers for particular bitcode instructions
// that are too uncommon and complex for a particular target to bother
// implementing directly in Subzero target lowering.  This needs to be
// compiled by some non-Subzero compiler.
//
//===----------------------------------------------------------------------===//

#include <stdint.h>
#include <stdlib.h>

// TODO(stichnot): The various NaN cross tests try to map Subzero's
// undefined behavior to the same as llc's undefined behavior, as
// observed by the cross tests.  This will have to be kept up to date
// with any future changes to llc, and may also have to be different
// for different targets.  It would be better to find a more
// appropriate set of llc options when building the Subzero runtime.
//
// We test for NaN using "value==value" instead of using isnan(value)
// to avoid an external dependency on fpclassify().

uint32_t cvtftoui32(float value) {
  if (value == value) // NaNaN
    return (uint32_t)value;
  return 0x80000000;
}

uint32_t cvtdtoui32(double value) {
  if (value == value) // NaNaN
    return (uint32_t)value;
  return 0x80000000;
}

int64_t cvtftosi64(float value) { return (int64_t)value; }

int64_t cvtdtosi64(double value) { return (int64_t)value; }

uint64_t cvtftoui64(float value) { return (uint64_t)value; }

uint64_t cvtdtoui64(double value) { return (uint64_t)value; }

float cvtui32tof(uint32_t value) { return (float)value; }

float cvtsi64tof(int64_t value) { return (float)value; }

float cvtui64tof(uint64_t value) { return (float)value; }

double cvtui32tod(uint32_t value) { return (double)value; }

double cvtsi64tod(int64_t value) { return (double)value; }

double cvtui64tod(uint64_t value) { return (double)value; }

/* TODO(stichnot):
   Sz_bitcast_v8i1_to_i8
   Sz_bitcast_v16i1_to_i16
   Sz_bitcast_i8_to_v8i1
   Sz_bitcast_i16_to_v16i1
*/
