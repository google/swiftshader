/* crosstest.py --test=test_fcmp.pnacl.ll --driver=test_fcmp_main.cpp \
   --prefix=Subzero_ --output=test_fcmp */

#include <cassert>
#include <cfloat>
#include <cmath>
#include <iostream>

#include "test_fcmp.def"

#define X(cmp)                                                                 \
  extern "C" bool fcmp##cmp##Float(float a, float b);                          \
  extern "C" bool fcmp##cmp##Double(double a, double b);                       \
  extern "C" bool Subzero_fcmp##cmp##Float(float a, float b);                  \
  extern "C" bool Subzero_fcmp##cmp##Double(double a, double b);
FCMP_TABLE;
#undef X

int main(int argc, char **argv) {
  static const double NegInf = -1.0 / 0.0;
  static const double Zero = 0.0;
  static const double Ten = 10.0;
  static const double PosInf = 1.0 / 0.0;
  static const double Nan = 0.0 / 0.0;
  assert(std::fpclassify(NegInf) == FP_INFINITE);
  assert(std::fpclassify(PosInf) == FP_INFINITE);
  assert(std::fpclassify(Nan) == FP_NAN);
  assert(NegInf < Zero);
  assert(NegInf < PosInf);
  assert(Zero < PosInf);

  volatile double Values[] = { NegInf,  Zero,    DBL_MIN, FLT_MIN, Ten,
                               FLT_MAX, DBL_MAX, PosInf,  Nan, };
  const static size_t NumValues = sizeof(Values) / sizeof(*Values);

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

  size_t TotalTests = 0;
  size_t Passes = 0;
  size_t Failures = 0;

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
                    << " llc=" << ResultLlc << std::endl;
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
                    << " llc=" << ResultLlc << std::endl;
        }
      }
    }
  }

  std::cout << "TotalTests=" << TotalTests << " Passes=" << Passes
            << " Failures=" << Failures << "\n";
  return Failures;
}
