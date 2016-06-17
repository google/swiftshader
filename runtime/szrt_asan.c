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

#include <stddef.h>
#include <stdlib.h>

// TODO(tlively): Define and implement this library
void __asan_init(void) {
  printf("Set up shadow memory here\n");
  return;
}

void __asan_check(void *addr, int size) {
  printf("Check %d bytes at %p\n", size, addr);
  return;
}

void *__asan_malloc(size_t size) {
  printf("malloc() called with size %d\n", size);
  return malloc(size);
}

void __asan_free(void *ptr) {
  printf("free() called on %p\n", ptr);
  free(ptr);
}
