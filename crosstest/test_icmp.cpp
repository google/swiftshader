// This aims to test the icmp bitcode instruction across all PNaCl
// primitive integer types.

#include <stdint.h>

#include "test_icmp.h"

#define X(cmp, op)                                                             \
  bool icmp##cmp(uint8_t a, uint8_t b) { return a op b; }                      \
  bool icmp##cmp(uint16_t a, uint16_t b) { return a op b; }                    \
  bool icmp##cmp(uint32_t a, uint32_t b) { return a op b; }                    \
  bool icmp##cmp(uint64_t a, uint64_t b) { return a op b; }
ICMP_U_TABLE
#undef X
#define X(cmp, op)                                                             \
  bool icmp##cmp(int8_t a, int8_t b) { return a op b; }                        \
  bool icmp##cmp(int16_t a, int16_t b) { return a op b; }                      \
  bool icmp##cmp(int32_t a, int32_t b) { return a op b; }                      \
  bool icmp##cmp(int64_t a, int64_t b) { return a op b; }
ICMP_S_TABLE
#undef X
