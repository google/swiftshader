/* crosstest.py --test=test_icmp.cpp --driver=test_icmp_main.cpp \
   --prefix=Subzero_ --output=test_icmp */

#include <stdint.h>
#include <iostream>

// Include test_icmp.h twice - once normally, and once within the
// Subzero_ namespace, corresponding to the llc and Subzero translated
// object files, respectively.
#include "test_icmp.h"
namespace Subzero_ {
#include "test_icmp.h"
}

volatile unsigned Values[] = { 0x0,        0x1,        0x7ffffffe, 0x7fffffff,
                               0x80000000, 0x80000001, 0xfffffffe, 0xffffffff,
                               0x7e,       0x7f,       0x80,       0x81,
                               0xfe,       0xff,       0x100,      0x101,
                               0x7ffe,     0x7fff,     0x8000,     0x8001,
                               0xfffe,     0xffff,     0x10000,    0x10001, };
const static size_t NumValues = sizeof(Values) / sizeof(*Values);

template <typename TypeUnsigned, typename TypeSigned>
void testsInt(size_t &TotalTests, size_t &Passes, size_t &Failures) {
  typedef bool (*FuncTypeUnsigned)(TypeUnsigned, TypeUnsigned);
  typedef bool (*FuncTypeSigned)(TypeSigned, TypeSigned);
  static struct {
    const char *Name;
    FuncTypeUnsigned FuncLlc;
    FuncTypeUnsigned FuncSz;
  } Funcs[] = {
#define X(cmp, op)                                                             \
  {                                                                            \
    STR(inst), (FuncTypeUnsigned)icmp##cmp,                                    \
        (FuncTypeUnsigned)Subzero_::icmp##cmp                                  \
  }                                                                            \
  ,
      ICMP_U_TABLE
#undef X
#define X(cmp, op)                                                             \
  {                                                                            \
    STR(inst), (FuncTypeUnsigned)(FuncTypeSigned)icmp##cmp,                    \
        (FuncTypeUnsigned)(FuncTypeSigned)Subzero_::icmp##cmp                  \
  }                                                                            \
  ,
          ICMP_S_TABLE
#undef X
    };
  const static size_t NumFuncs = sizeof(Funcs) / sizeof(*Funcs);

  if (sizeof(TypeUnsigned) <= sizeof(uint32_t)) {
    // This is the "normal" version of the loop nest, for 32-bit or
    // narrower types.
    for (size_t f = 0; f < NumFuncs; ++f) {
      for (size_t i = 0; i < NumValues; ++i) {
        for (size_t j = 0; j < NumValues; ++j) {
          TypeUnsigned Value1 = Values[i];
          TypeUnsigned Value2 = Values[j];
          ++TotalTests;
          bool ResultSz = Funcs[f].FuncSz(Value1, Value2);
          bool ResultLlc = Funcs[f].FuncLlc(Value1, Value2);
          if (ResultSz == ResultLlc) {
            ++Passes;
          } else {
            ++Failures;
            std::cout << "icmp" << Funcs[f].Name << (8 * sizeof(TypeUnsigned))
                      << "(" << Value1 << ", " << Value2 << "): sz=" << ResultSz
                      << " llc=" << ResultLlc << std::endl;
          }
        }
      }
    }
  } else {
    // This is the 64-bit version.  Test values are synthesized from
    // the 32-bit values in Values[].
    for (size_t f = 0; f < NumFuncs; ++f) {
      for (size_t iLo = 0; iLo < NumValues; ++iLo) {
        for (size_t iHi = 0; iHi < NumValues; ++iHi) {
          for (size_t jLo = 0; jLo < NumValues; ++jLo) {
            for (size_t jHi = 0; jHi < NumValues; ++jHi) {
              TypeUnsigned Value1 =
                  (((TypeUnsigned)Values[iHi]) << 32) + Values[iLo];
              TypeUnsigned Value2 =
                  (((TypeUnsigned)Values[jHi]) << 32) + Values[jLo];
              ++TotalTests;
              bool ResultSz = Funcs[f].FuncSz(Value1, Value2);
              bool ResultLlc = Funcs[f].FuncLlc(Value1, Value2);
              if (ResultSz == ResultLlc) {
                ++Passes;
              } else {
                ++Failures;
                std::cout << "icmp" << Funcs[f].Name
                          << (8 * sizeof(TypeUnsigned)) << "(" << Value1 << ", "
                          << Value2 << "): sz=" << ResultSz
                          << " llc=" << ResultLlc << std::endl;
              }
            }
          }
        }
      }
    }
  }
}

int main(int argc, char **argv) {
  size_t TotalTests = 0;
  size_t Passes = 0;
  size_t Failures = 0;

  testsInt<uint8_t, int8_t>(TotalTests, Passes, Failures);
  testsInt<uint16_t, int16_t>(TotalTests, Passes, Failures);
  testsInt<uint32_t, int32_t>(TotalTests, Passes, Failures);
  testsInt<uint64_t, int64_t>(TotalTests, Passes, Failures);

  std::cout << "TotalTests=" << TotalTests << " Passes=" << Passes
            << " Failures=" << Failures << "\n";
  return Failures;
}
