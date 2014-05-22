#include <stdint.h>
#include "test_arith.def"

#define X(inst, op, isdiv)                                                     \
  bool test##inst(bool a, bool b);                                             \
  uint8_t test##inst(uint8_t a, uint8_t b);                                    \
  uint16_t test##inst(uint16_t a, uint16_t b);                                 \
  uint32_t test##inst(uint32_t a, uint32_t b);                                 \
  uint64_t test##inst(uint64_t a, uint64_t b);
UINTOP_TABLE
#undef X

#define X(inst, op, isdiv)                                                     \
  bool test##inst(bool a, bool b);                                             \
  int8_t test##inst(int8_t a, int8_t b);                                       \
  int16_t test##inst(int16_t a, int16_t b);                                    \
  int32_t test##inst(int32_t a, int32_t b);                                    \
  int64_t test##inst(int64_t a, int64_t b);
SINTOP_TABLE
#undef X

float myFrem(float a, float b);
double myFrem(double a, double b);

#define X(inst, op, func)                                                      \
  float test##inst(float a, float b);                                          \
  double test##inst(double a, double b);
FPOP_TABLE
#undef X
