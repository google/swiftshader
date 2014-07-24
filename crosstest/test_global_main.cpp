/* crosstest.py --test=test_global.cpp \
   --driver=test_global_main.cpp --prefix=Subzero_ --output=test_global */

#include <stdint.h>
#include <cstdlib>
#include <iostream>

#include "test_global.h"
namespace Subzero_ {
#include "test_global.h"
}

int main(int argc, char **argv) {
  size_t TotalTests = 0;
  size_t Passes = 0;
  size_t Failures = 0;

  const uint8_t *SzArray, *LlcArray;
  size_t SzArrayLen, LlcArrayLen;

  size_t NumArrays = getNumArrays();
  for (size_t i = 0; i < NumArrays; ++i) {
    LlcArrayLen = -1;
    SzArrayLen = -2;
    LlcArray = getArray(i, LlcArrayLen);
    SzArray = Subzero_::getArray(i, SzArrayLen);
    ++TotalTests;
    if (LlcArrayLen == SzArrayLen) {
      ++Passes;
    } else {
      std::cout << i << ":LlcArrayLen=" << LlcArrayLen
                << ", SzArrayLen=" << SzArrayLen << std::endl;
      ++Failures;
    }

    for (size_t i = 0; i < LlcArrayLen; ++i) {
      ++TotalTests;
      if (LlcArray[i] == SzArray[i]) {
        ++Passes;
      } else {
        ++Failures;
        std::cout << i << ":LlcArray[" << i << "] = " << (int)LlcArray[i]
                  << ", SzArray[" << i << "] = " << (int)SzArray[i]
                  << std::endl;
      }
    }
  }

  std::cout << "TotalTests=" << TotalTests << " Passes=" << Passes
            << " Failures=" << Failures << "\n";
  return Failures;
}
