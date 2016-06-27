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
#include <errno.h>
#include <limits.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>

#define RZ_SIZE (32)
#define SHADOW_SCALE_LOG2 (3)
#define SHADOW_SCALE ((size_t)1 << SHADOW_SCALE_LOG2)

// Assuming 48 bit address space on 64 bit systems
#define SHADOW_LENGTH_64 (1u << (48 - SHADOW_SCALE_LOG2))
#define SHADOW_LENGTH_32 (1u << (32 - SHADOW_SCALE_LOG2))
#define IS_32_BIT (sizeof(void *) == 4)

#define SHADOW_OFFSET(p) ((uintptr_t)(p) % SHADOW_SCALE)
#define IS_SHADOW_ALIGNED(p) (SHADOW_OFFSET(p) == 0)

#define MEM2SHADOW(p) (((uintptr_t)(p) >> SHADOW_SCALE_LOG2) + shadow_offset)
#define SHADOW2MEM(p)                                                          \
  ((uintptr_t)((char *)(p)-shadow_offset) << SHADOW_SCALE_LOG2)

#define POISON_VAL (-1)

static char *shadow_offset = NULL;

void __asan_init(void);
void __asan_check(char *, int);
void *__asan_malloc(size_t);
void __asan_free(char *);
void __asan_poison(char *, int);
void __asan_unpoison(char *, int);

void __asan_init(void) {
  // ensure the redzones are large enough to hold metadata
  assert(RZ_SIZE >= sizeof(void *) && RZ_SIZE >= sizeof(size_t));
  assert(shadow_offset == NULL);
  size_t length = (IS_32_BIT) ? SHADOW_LENGTH_32 : SHADOW_LENGTH_64;
  int prot = PROT_READ | PROT_WRITE;
  int flags = MAP_PRIVATE | MAP_ANONYMOUS;
  int fd = -1;
  off_t offset = 0;
  shadow_offset = mmap((void *)length, length, prot, flags, fd, offset);
  if (shadow_offset == NULL)
    fprintf(stderr, "unable to allocate shadow memory\n");
  else
    printf("set up shadow memory at %p\n", shadow_offset);
  if (mprotect(MEM2SHADOW(shadow_offset), length >> SHADOW_SCALE_LOG2,
               PROT_NONE))
    fprintf(stderr, "could not protect bad region\n");
  else
    printf("protected bad region\n");
}

void __asan_check(char *ptr, int size) {
  printf("check %d bytes at %p\n", size, ptr);
  char *end = ptr + size;
  for (; ptr < end; ++ptr) {
    char shadow = *(char *)MEM2SHADOW(ptr);
    printf("checking %p with shadow %d\n", ptr, shadow);
    assert(shadow == 0 || (shadow > 0 && SHADOW_OFFSET(ptr) <= shadow));
  }
}

void *__asan_malloc(size_t size) {
  printf("malloc() called with size %d\n", size);
  size_t padding =
      (IS_SHADOW_ALIGNED(size)) ? 0 : SHADOW_SCALE - SHADOW_OFFSET(size);
  size_t rz_left_size = RZ_SIZE;
  size_t rz_right_size = RZ_SIZE + padding;
  void *rz_left;
  int err = posix_memalign(&rz_left, SHADOW_SCALE,
                           rz_left_size + size + rz_right_size);
  if (err != 0) {
    assert(err == ENOMEM);
    return NULL;
  }
  void *ret = rz_left + rz_left_size;
  void *rz_right = ret + size;
  __asan_poison(rz_left, rz_left_size);
  __asan_poison(rz_right, rz_right_size);
  // record size and location data so we can find it again
  *(void **)rz_left = rz_right;
  *(size_t *)rz_right = rz_right_size;
  assert((uintptr_t)ret % 8 == 0);
  return ret;
}

void __asan_free(char *ptr) {
  printf("free() called on %p\n", ptr);
  void *rz_left = ptr - RZ_SIZE;
  void *rz_right = *(void **)rz_left;
  size_t rz_right_size = *(size_t *)rz_right;
  __asan_unpoison(rz_left, RZ_SIZE);
  __asan_unpoison(rz_right, rz_right_size);
  free(rz_left);
}

void __asan_poison(char *ptr, int size) {
  char *end = ptr + size;
  assert(IS_SHADOW_ALIGNED(end));
  // redzones should be no greater than RZ_SIZE + RZ_SIZE-1 for alignment
  assert(size < 2 * RZ_SIZE);
  printf("poison %d bytes at %p: %p - %p\n", size, ptr, MEM2SHADOW(ptr),
         MEM2SHADOW(end));
  size_t offset = SHADOW_OFFSET(ptr);
  *(char *)MEM2SHADOW(ptr) = (offset == 0) ? POISON_VAL : offset;
  ptr += SHADOW_OFFSET(size);
  assert(IS_SHADOW_ALIGNED(ptr));
  for (; ptr != end; ptr += SHADOW_SCALE) {
    *(char *)MEM2SHADOW(ptr) = POISON_VAL;
  }
}

void __asan_unpoison(char *ptr, int size) {
  char *end = ptr + size;
  assert(IS_SHADOW_ALIGNED(end));
  assert(size < 2 * RZ_SIZE);
  printf("unpoison %d bytes at %p: %p - %p\n", size, ptr, MEM2SHADOW(ptr),
         MEM2SHADOW(end));
  *(char *)MEM2SHADOW(ptr) = 0;
  ptr += SHADOW_OFFSET(size);
  assert(IS_SHADOW_ALIGNED(ptr));
  for (; ptr != end; ptr += SHADOW_SCALE) {
    *(char *)MEM2SHADOW(ptr) = 0;
  }
}
