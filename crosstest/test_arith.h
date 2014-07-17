#include <stdint.h>
#include "test_arith.def"

// Vector types
typedef int32_t v4si32 __attribute__((vector_size(16)));
typedef uint32_t v4ui32 __attribute__((vector_size(16)));
typedef int16_t v8si16 __attribute__((vector_size(16)));
typedef uint16_t v8ui16 __attribute__((vector_size(16)));
typedef int8_t v16si8 __attribute__((vector_size(16)));
typedef uint8_t v16ui8 __attribute__((vector_size(16)));
typedef float v4f32 __attribute__((vector_size(16)));

#define X(inst, op, isdiv)                                                     \
  bool test##inst(bool a, bool b);                                             \
  uint8_t test##inst(uint8_t a, uint8_t b);                                    \
  uint16_t test##inst(uint16_t a, uint16_t b);                                 \
  uint32_t test##inst(uint32_t a, uint32_t b);                                 \
  uint64_t test##inst(uint64_t a, uint64_t b);                                 \
  v4ui32 test##inst(v4ui32 a, v4ui32 b);                                       \
  v8ui16 test##inst(v8ui16 a, v8ui16 b);                                       \
  v16ui8 test##inst(v16ui8 a, v16ui8 b);
UINTOP_TABLE
#undef X

#define X(inst, op, isdiv)                                                     \
  bool test##inst(bool a, bool b);                                             \
  int8_t test##inst(int8_t a, int8_t b);                                       \
  int16_t test##inst(int16_t a, int16_t b);                                    \
  int32_t test##inst(int32_t a, int32_t b);                                    \
  int64_t test##inst(int64_t a, int64_t b);                                    \
  v4si32 test##inst(v4si32 a, v4si32 b);                                       \
  v8si16 test##inst(v8si16 a, v8si16 b);                                       \
  v16si8 test##inst(v16si8 a, v16si8 b);
SINTOP_TABLE
#undef X

float myFrem(float a, float b);
double myFrem(double a, double b);
v4f32 myFrem(v4f32 a, v4f32 b);

#define X(inst, op, func)                                                      \
  float test##inst(float a, float b);                                          \
  double test##inst(double a, double b);                                       \
  v4f32 test##inst(v4f32 a, v4f32 b);
FPOP_TABLE
#undef X

float mySqrt(float a);
double mySqrt(double a);
// mySqrt for v4f32 is currently unsupported.
