//===- subzero/runtime/szrt.c - Subzero runtime source ----------*- C++ -*-===//
//
//                        The Subzero Code Generator
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements the runtime helper routines that are needed by
// Subzero.  This needs to be compiled by some non-Subzero compiler.
//
//===----------------------------------------------------------------------===//

#include <stdint.h>
#include <stdlib.h>

void ice_unreachable(void) {
  abort();
}

uint32_t cvtftoui32(float value) {
  return (uint32_t) value;
}

uint32_t cvtdtoui32(double value) {
  return (uint32_t) value;
}

int64_t cvtftosi64(float value) {
  return (int64_t) value;
}

int64_t cvtdtosi64(double value) {
  return (int64_t) value;
}

uint64_t cvtftoui64(float value) {
  return (uint64_t) value;
}

uint64_t cvtdtoui64(double value) {
  return (uint64_t) value;
}

float cvtui32tof(uint32_t value) {
  return (float) value;
}

float cvtsi64tof(int64_t value) {
  return (float) value;
}

float cvtui64tof(uint64_t value) {
  return (float) value;
}

double cvtui32tod(uint32_t value) {
  return (double) value;
}

double cvtsi64tod(int64_t value) {
  return (double) value;
}

double cvtui64tod(uint64_t value) {
  return (double) value;
}

/* TODO(stichnot):
   Sz_bitcast_v8i1_to_i8
   Sz_bitcast_v16i1_to_i16
   Sz_bitcast_i8_to_v8i1
   Sz_bitcast_i16_to_v16i1
*/
