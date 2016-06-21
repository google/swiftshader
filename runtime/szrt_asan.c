//===- subzero/runtime/szrt_asan.c - AddressSanitizer Runtime -----*- C -*-===//
//
//                        The Subzero Code Generator
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief Provides the AddressSanitizer runtime.
///
/// Exposes functions for initializing the shadow memory region and managing it
/// on loads, stores, and allocations.
///
//===----------------------------------------------------------------------===//

#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

static __thread int behind_malloc = 0;

// TODO(tlively): Define and implement this library
void __asan_init(void) {
  if (behind_malloc == 0)
    printf("set up shadow memory here\n");
}

void __asan_check(void *addr, int size) {
  if (behind_malloc == 0)
    printf("check %d bytes at %p\n", size, addr);
}

void *__asan_malloc(size_t size) {
  if (behind_malloc == 0)
    printf("malloc() called with size %d\n", size);
  ++behind_malloc;
  void *ret = malloc(size);
  --behind_malloc;
  assert(behind_malloc >= 0);
  return ret;
}

void __asan_free(void *ptr) {
  if (behind_malloc == 0)
    printf("free() called on %p\n", ptr);
  ++behind_malloc;
  free(ptr);
  --behind_malloc;
  assert(behind_malloc >= 0);
}

void __asan_alloca(void *ptr, int size) {
  if (behind_malloc == 0)
    printf("alloca of %d bytes at %p\n", size, ptr);
}

void __asan_unalloca(void *ptr, int size) {
  if (behind_malloc == 0)
    printf("unalloca of %d bytes as %p\n", size, ptr);
}
