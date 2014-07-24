//===- subzero/crosstest/test_fcmp_main.cpp - Driver for tests ------------===//
//
//                        The Subzero Code Generator
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// Driver for cross testing the fcmp bitcode instruction
//
//===----------------------------------------------------------------------===//

/* crosstest.py --test=test_fcmp.pnacl.ll --driver=test_fcmp_main.cpp \
   --prefix=Subzero_ --output=test_fcmp */

#include <cassert>
#include <cfloat>
#include <cmath>
#include <cstring>
#include <iostream>

#include "vectors.h"
#include "test_fcmp.def"

#define X(cmp)                                                                 \
  extern "C" bool fcmp##cmp##Float(float a, float b);                          \
  extern "C" bool fcmp##cmp##Double(double a, double b);                       \
  extern "C" v4si32 fcmp##cmp##Vector(v4f32 a, v4f32 b);                       \
  extern "C" bool Subzero_fcmp##cmp##Float(float a, float b);                  \
  extern "C" bool Subzero_fcmp##cmp##Double(double a, double b);               \
  extern "C" v4si32 Subzero_fcmp##cmp##Vector(v4f32 a, v4f32 b);
FCMP_TABLE;
#undef X

volatile double *Values;
size_t NumValues;

void initializeValues() {
  static const double NegInf = -1.0 / 0.0;
  static const double Zero = 0.0;
  static const double Ten = 10.0;
  static const double PosInf = 1.0 / 0.0;
  static const double Nan = 0.0 / 0.0;
  static const double NegNan = -0.0 / 0.0;
  assert(std::fpclassify(NegInf) == FP_INFINITE);
  assert(std::fpclassify(PosInf) == FP_INFINITE);
  assert(std::fpclassify(Nan) == FP_NAN);
  assert(std::fpclassify(NegNan) == FP_NAN);
  assert(NegInf < Zero);
  assert(NegInf < PosInf);
  assert(Zero < PosInf);
  static volatile double InitValues[] = {NegInf,  -Zero, Zero,    DBL_MIN,
                                         FLT_MIN, Ten,   FLT_MAX, DBL_MAX,
                                         PosInf,  Nan,   NegNan};
  NumValues = sizeof(InitValues) / sizeof(*InitValues);
  Values = InitValues;
}

void testsScalar(size_t &TotalTests, size_t &Passes, size_t &Failures) {
  typedef bool (*FuncTypeFloat)(float, float);
  typedef bool (*FuncTypeDouble)(double, double);
  static struct {
    const char *Name;
    FuncTypeFloat FuncFloatSz;
    FuncTypeFloat FuncFloatLlc;
    FuncTypeDouble FuncDoubleSz;
    FuncTypeDouble FuncDoubleLlc;
  } Funcs[] = {
#define X(cmp)                                                                 \
  {                                                                            \
    "fcmp" STR(cmp), Subzero_fcmp##cmp##Float, fcmp##cmp##Float,               \
        Subzero_fcmp##cmp##Double, fcmp##cmp##Double                           \
  }                                                                            \
  ,
      FCMP_TABLE
#undef X
    };
  const static size_t NumFuncs = sizeof(Funcs) / sizeof(*Funcs);

  bool ResultSz, ResultLlc;

  assert(Values && NumValues);

  for (size_t f = 0; f < NumFuncs; ++f) {
    for (size_t i = 0; i < NumValues; ++i) {
      for (size_t j = 0; j < NumValues; ++j) {
        ++TotalTests;
        float Value1Float = Values[i];
        float Value2Float = Values[j];
        ResultSz = Funcs[f].FuncFloatSz(Value1Float, Value2Float);
        ResultLlc = Funcs[f].FuncFloatLlc(Value1Float, Value2Float);
        if (ResultSz == ResultLlc) {
          ++Passes;
        } else {
          ++Failures;
          std::cout << Funcs[f].Name << "Float(" << Value1Float << ", "
                    << Value2Float << "): sz=" << ResultSz
                    << " llc=" << ResultLlc << "\n";
        }
        ++TotalTests;
        double Value1Double = Values[i];
        double Value2Double = Values[j];
        ResultSz = Funcs[f].FuncDoubleSz(Value1Double, Value2Double);
        ResultLlc = Funcs[f].FuncDoubleLlc(Value1Double, Value2Double);
        if (ResultSz == ResultLlc) {
          ++Passes;
        } else {
          ++Failures;
          std::cout << Funcs[f].Name << "Double(" << Value1Double << ", "
                    << Value2Double << "): sz=" << ResultSz
                    << " llc=" << ResultLlc << "\n";
        }
      }
    }
  }
}

void testsVector(size_t &TotalTests, size_t &Passes, size_t &Failures) {
  typedef v4si32 (*FuncTypeVector)(v4f32, v4f32);
  static struct {
    const char *Name;
    FuncTypeVector FuncVectorSz;
    FuncTypeVector FuncVectorLlc;
  } Funcs[] = {
#define X(cmp)                                                                 \
  { "fcmp" STR(cmp), Subzero_fcmp##cmp##Vector, fcmp##cmp##Vector }            \
  ,
        FCMP_TABLE
#undef X
    };
  const static size_t NumFuncs = sizeof(Funcs) / sizeof(*Funcs);
  const static size_t NumElementsInType = 4;
  const static size_t MaxTestsPerFunc = 100000;

  assert(Values && NumValues);

  for (size_t f = 0; f < NumFuncs; ++f) {
    PRNG Index;
    for (size_t i = 0; i < MaxTestsPerFunc; ++i) {
      v4f32 Value1, Value2;
      for (size_t j = 0; j < NumElementsInType; ++j) {
        Value1[j] = Values[Index() % NumValues];
        Value2[j] = Values[Index() % NumValues];
      }
      ++TotalTests;
      v4si32 ResultSz, ResultLlc;
      ResultSz = Funcs[f].FuncVectorSz(Value1, Value2);
      ResultLlc = Funcs[f].FuncVectorLlc(Value1, Value2);
      if (!memcmp(&ResultSz, &ResultLlc, sizeof(ResultSz))) {
        ++Passes;
      } else {
        ++Failures;
        std::cout << Funcs[f].Name << "Vector(" << vectAsString<v4f32>(Value1)
                  << ", " << vectAsString<v4f32>(Value2)
                  << "): sz=" << vectAsString<v4si32>(ResultSz)
                  << " llc=" << vectAsString<v4si32>(ResultLlc) << "\n";
      }
    }
  }
}

int main(int argc, char **argv) {
  size_t TotalTests = 0;
  size_t Passes = 0;
  size_t Failures = 0;

  initializeValues();

  testsScalar(TotalTests, Passes, Failures);
  testsVector(TotalTests, Passes, Failures);

  std::cout << "TotalTests=" << TotalTests << " Passes=" << Passes
            << " Failures=" << Failures << "\n";
  return Failures;
}
