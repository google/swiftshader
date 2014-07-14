/* crosstest.py --test=mem_intrin.cpp --driver=mem_intrin_main.cpp \
   --prefix=Subzero_ --output=mem_intrin */

#include <stdint.h> /* cstdint requires -std=c++0x or higher */
#include <cstdio>

#include "mem_intrin.h"
namespace Subzero_ {
#include "mem_intrin.h"
}

#define XSTR(s) STR(s)
#define STR(s) #s

void testFixedLen(size_t &TotalTests, size_t &Passes, size_t &Failures) {
#define do_test_fixed(test_func)                                        \
  for (uint8_t init_val = 0; init_val < 100; ++init_val) {              \
    ++TotalTests;                                                       \
    int llc_result = test_func(init_val);                               \
    int sz_result = Subzero_::test_func(init_val);                      \
    if (llc_result == sz_result) {                                      \
      ++Passes;                                                         \
    } else {                                                            \
      ++Failures;                                                       \
      printf("Failure (%s): init_val=%d, llc=%d, sz=%d\n",              \
             STR(test_func), init_val, llc_result, sz_result);          \
    }                                                                   \
  }

  do_test_fixed(memcpy_test_fixed_len)
  do_test_fixed(memmove_test_fixed_len)
  do_test_fixed(memset_test_fixed_len)
#undef do_test_fixed
}

void testVariableLen(size_t &TotalTests, size_t &Passes, size_t &Failures) {
  uint8_t buf[256];
  uint8_t buf2[256];
#define do_test_variable(test_func)                                     \
  for (size_t len = 4; len < 128; ++len) {                              \
    for (uint8_t init_val = 0; init_val < 100; ++init_val) {            \
      ++TotalTests;                                                     \
      int llc_result = test_func(buf, buf2, init_val, len);             \
      int sz_result = Subzero_::test_func(buf, buf2, init_val, len);    \
      if (llc_result == sz_result) {                                    \
        ++Passes;                                                       \
      } else {                                                          \
        ++Failures;                                                     \
        printf("Failure (%s): init_val=%d, len=%d, llc=%d, sz=%d\n",    \
               STR(test_func), init_val, len, llc_result, sz_result);   \
      }                                                                 \
    }                                                                   \
  }

  do_test_variable(memcpy_test)
  do_test_variable(memmove_test)
  do_test_variable(memset_test)
#undef do_test_variable
}

int main(int argc, char **argv) {
  unsigned TotalTests = 0;
  unsigned Passes = 0;
  unsigned Failures = 0;
  testFixedLen(TotalTests, Passes, Failures);
  testVariableLen(TotalTests, Passes, Failures);
  printf("TotalTests=%u Passes=%u Failures=%u\n", TotalTests, Passes, Failures);
  return Failures;
}
