/*
 * Simple sanity test of memcpy, memmove, and memset intrinsics.
 * (fixed length buffers, variable length buffers, etc.)
 */

#include <stdint.h> /* cstdint requires -std=c++0x or higher */
#include <cstdlib>
#include <cstring>

#include "mem_intrin.h"

typedef int elem_t;

/*
 * Reset buf to the sequence of bytes: n, n+1, n+2 ... length - 1
 */
static void __attribute__((noinline)) reset_buf(uint8_t *buf,
                                                uint8_t init,
                                                size_t length) {
  size_t i;
  size_t v = init;
  for (i = 0; i < length; ++i)
    buf[i] = v++;
}

/* Do a fletcher-16 checksum so that the order of the values matter.
 * (Not doing a fletcher-32 checksum, since we are working with
 * smaller buffers, whose total won't approach 2**16).
 */
static int __attribute__((noinline)) fletcher_checksum(uint8_t *buf,
                                                       size_t length) {
  size_t i;
  int sum = 0;
  int sum_of_sums = 0;
  const int kModulus = 255;
  for (i = 0; i < length; ++i) {
    sum = (sum + buf[i]) % kModulus;
    sum_of_sums = (sum_of_sums + sum) % kModulus;
  }
  return (sum_of_sums << 8) | sum;
}

#define NWORDS 32
#define BYTE_LENGTH (NWORDS * sizeof(elem_t))

int memcpy_test_fixed_len(uint8_t init) {
  elem_t buf[NWORDS];
  elem_t buf2[NWORDS];
  reset_buf((uint8_t *)buf, init, BYTE_LENGTH);
  memcpy((void *)buf2, (void *)buf, BYTE_LENGTH);
  return fletcher_checksum((uint8_t *)buf2, BYTE_LENGTH);
}

int memmove_test_fixed_len(uint8_t init) {
  elem_t buf[NWORDS];
  reset_buf((uint8_t *)buf, init, BYTE_LENGTH);
  memmove((void *)(buf + 4), (void *)buf, BYTE_LENGTH - (4 * sizeof(elem_t)));
  return fletcher_checksum((uint8_t *)buf + 4, BYTE_LENGTH - 4);
}

int memset_test_fixed_len(uint8_t init) {
  elem_t buf[NWORDS];
  memset((void *)buf, init, BYTE_LENGTH);
  return fletcher_checksum((uint8_t *)buf, BYTE_LENGTH);
}

int memcpy_test(uint8_t *buf, uint8_t *buf2, uint8_t init, size_t length) {
  reset_buf(buf, init, length);
  memcpy((void *)buf2, (void *)buf, length);
  return fletcher_checksum(buf2, length);
}

int memmove_test(uint8_t *buf, uint8_t *buf2, uint8_t init, size_t length) {
  int sum1;
  int sum2;
  const int overlap_bytes = 4 * sizeof(elem_t);
  if (length <= overlap_bytes)
    return 0;
  uint8_t *overlap_buf = buf + overlap_bytes;
  size_t reduced_length = length - overlap_bytes;
  reset_buf(buf, init, length);

  /* Test w/ overlap. */
  memmove((void *)overlap_buf, (void *)buf, reduced_length);
  sum1 = fletcher_checksum(overlap_buf, reduced_length);
  /* Test w/out overlap. */
  memmove((void *)buf2, (void *)buf, length);
  sum2 = fletcher_checksum(buf2, length);
  return sum1 + sum2;
}

int memset_test(uint8_t *buf, uint8_t *buf2, uint8_t init, size_t length) {
  memset((void *)buf, init, length);
  memset((void *)buf2, init + 4, length);
  return fletcher_checksum(buf, length) + fletcher_checksum(buf2, length);
}
