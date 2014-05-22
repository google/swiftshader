/* crosstest.py --test=test_arith.cpp --test=test_arith_frem.ll \
   --driver=test_arith_main.cpp --prefix=Subzero_ --output=test_arith */

#include <stdint.h>

#include <cfloat>
#include <cstring> // memcmp
#include <iostream>

// Include test_arith.h twice - once normally, and once within the
// Subzero_ namespace, corresponding to the llc and Subzero translated
// object files, respectively.
#include "test_arith.h"
namespace Subzero_ {
#include "test_arith.h"
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
  typedef TypeUnsigned (*FuncTypeUnsigned)(TypeUnsigned, TypeUnsigned);
  typedef TypeSigned (*FuncTypeSigned)(TypeSigned, TypeSigned);
  static struct {
    const char *Name;
    FuncTypeUnsigned FuncLlc;
    FuncTypeUnsigned FuncSz;
    bool ExcludeDivExceptions; // for divide related tests
  } Funcs[] = {
#define X(inst, op, isdiv)                                                     \
  {                                                                            \
    STR(inst), (FuncTypeUnsigned)test##inst,                                   \
        (FuncTypeUnsigned)Subzero_::test##inst, isdiv                          \
  }                                                                            \
  ,
      UINTOP_TABLE
#undef X
#define X(inst, op, isdiv)                                                     \
  {                                                                            \
    STR(inst), (FuncTypeUnsigned)(FuncTypeSigned)test##inst,                   \
        (FuncTypeUnsigned)(FuncTypeSigned)Subzero_::test##inst, isdiv          \
  }                                                                            \
  ,
          SINTOP_TABLE
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
          // Avoid HW divide-by-zero exception.
          if (Funcs[f].ExcludeDivExceptions && Value2 == 0)
            continue;
          // Avoid HW overflow exception (on x86-32).  TODO: adjust
          // for other architectures.
          if (Funcs[f].ExcludeDivExceptions && Value1 == 0x80000000 &&
              Value2 == 0xffffffff)
            continue;
          ++TotalTests;
          TypeUnsigned ResultSz = Funcs[f].FuncSz(Value1, Value2);
          TypeUnsigned ResultLlc = Funcs[f].FuncLlc(Value1, Value2);
          if (ResultSz == ResultLlc) {
            ++Passes;
          } else {
            ++Failures;
            std::cout << "test" << Funcs[f].Name << (8 * sizeof(TypeUnsigned))
                      << "(" << Value1 << ", " << Value2
                      << "): sz=" << (unsigned)ResultSz
                      << " llc=" << (unsigned)ResultLlc << std::endl;
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
              // Avoid HW divide-by-zero exception.
              if (Funcs[f].ExcludeDivExceptions && Value2 == 0)
                continue;
              ++TotalTests;
              TypeUnsigned ResultSz = Funcs[f].FuncSz(Value1, Value2);
              TypeUnsigned ResultLlc = Funcs[f].FuncLlc(Value1, Value2);
              if (ResultSz == ResultLlc) {
                ++Passes;
              } else {
                ++Failures;
                std::cout << "test" << Funcs[f].Name
                          << (8 * sizeof(TypeUnsigned)) << "(" << Value1 << ", "
                          << Value2 << "): sz=" << (unsigned)ResultSz
                          << " llc=" << (unsigned)ResultLlc << std::endl;
              }
            }
          }
        }
      }
    }
  }
}

template <typename Type>
void testsFp(size_t &TotalTests, size_t &Passes, size_t &Failures) {
  static const Type NegInf = -1.0 / 0.0;
  static const Type PosInf = 1.0 / 0.0;
  static const Type Nan = 0.0 / 0.0;
  volatile Type Values[] = {
    0,                    1,                    0x7e,
    0x7f,                 0x80,                 0x81,
    0xfe,                 0xff,                 0x7ffe,
    0x7fff,               0x8000,               0x8001,
    0xfffe,               0xffff,               0x7ffffffe,
    0x7fffffff,           0x80000000,           0x80000001,
    0xfffffffe,           0xffffffff,           0x100000000ll,
    0x100000001ll,        0x7ffffffffffffffell, 0x7fffffffffffffffll,
    0x8000000000000000ll, 0x8000000000000001ll, 0xfffffffffffffffell,
    0xffffffffffffffffll, NegInf,               PosInf,
    Nan,                  FLT_MIN,              FLT_MAX,
    DBL_MIN,              DBL_MAX
  };
  const static size_t NumValues = sizeof(Values) / sizeof(*Values);
  typedef Type (*FuncType)(Type, Type);
  static struct {
    const char *Name;
    FuncType FuncLlc;
    FuncType FuncSz;
  } Funcs[] = {
#define X(inst, op, func)                                                      \
  { STR(inst), (FuncType)test##inst, (FuncType)Subzero_::test##inst }          \
  ,
      FPOP_TABLE
#undef X
    };
  const static size_t NumFuncs = sizeof(Funcs) / sizeof(*Funcs);

  for (size_t f = 0; f < NumFuncs; ++f) {
    for (size_t i = 0; i < NumValues; ++i) {
      for (size_t j = 0; j < NumValues; ++j) {
        Type Value1 = Values[i];
        Type Value2 = Values[j];
        ++TotalTests;
        Type ResultSz = Funcs[f].FuncSz(Value1, Value2);
        Type ResultLlc = Funcs[f].FuncLlc(Value1, Value2);
        // Compare results using memcmp() in case they are both NaN.
        if (!memcmp(&ResultSz, &ResultLlc, sizeof(Type))) {
          ++Passes;
        } else {
          ++Failures;
          std::cout << std::fixed << "test" << Funcs[f].Name
                    << (8 * sizeof(Type)) << "(" << Value1 << ", " << Value2
                    << "): sz=" << ResultSz << " llc=" << ResultLlc
                    << std::endl;
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
  testsFp<float>(TotalTests, Passes, Failures);
  testsFp<double>(TotalTests, Passes, Failures);

  std::cout << "TotalTests=" << TotalTests << " Passes=" << Passes
            << " Failures=" << Failures << "\n";
  return Failures;
}
