#include "test_icmp.def"

#define X(cmp, op)                                                             \
  bool icmp##cmp(uint8_t a, uint8_t b);                                        \
  bool icmp##cmp(uint16_t a, uint16_t b);                                      \
  bool icmp##cmp(uint32_t a, uint32_t b);                                      \
  bool icmp##cmp(uint64_t a, uint64_t b);
ICMP_U_TABLE
#undef X

#define X(cmp, op)                                                             \
  bool icmp##cmp(int8_t a, int8_t b);                                          \
  bool icmp##cmp(int16_t a, int16_t b);                                        \
  bool icmp##cmp(int32_t a, int32_t b);                                        \
  bool icmp##cmp(int64_t a, int64_t b);
ICMP_S_TABLE
#undef X
