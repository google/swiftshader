//===- subzero/crosstest/test_sync_atomic_main.cpp - Driver for tests -----===//
//
//                        The Subzero Code Generator
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// Driver for cross testing atomic intrinsics, via the sync builtins.
//
//===----------------------------------------------------------------------===//

/* crosstest.py --test=test_sync_atomic.cpp --crosstest-bitcode=0 \
   --driver=test_sync_atomic_main.cpp --prefix=Subzero_ \
   --output=test_sync_atomic */

#include <pthread.h>
#include <stdint.h>

#include <cerrno>
#include <climits>
#include <cstdlib>
#include <cstring>
#include <iostream>

// Include test_sync_atomic.h twice - once normally, and once within the
// Subzero_ namespace, corresponding to the llc and Subzero translated
// object files, respectively.
#include "test_sync_atomic.h"
namespace Subzero_ {
#include "test_sync_atomic.h"
}

volatile uint64_t Values[] = {
    0,                    1,                    0x7e,
    0x7f,                 0x80,                 0x81,
    0xfe,                 0xff,                 0x7ffe,
    0x7fff,               0x8000,               0x8001,
    0xfffe,               0xffff,
    0x007fffff /*Max subnormal + */,
    0x00800000 /*Min+ */, 0x7f7fffff /*Max+ */,
    0x7f800000 /*+Inf*/,  0xff800000 /*-Inf*/,
    0x7fa00000 /*SNaN*/,  0x7fc00000 /*QNaN*/,
    0x7ffffffe,           0x7fffffff,           0x80000000,
    0x80000001,           0xfffffffe,           0xffffffff,
    0x100000000ll,        0x100000001ll,
    0x000fffffffffffffll /*Max subnormal + */,
    0x0010000000000000ll /*Min+ */,
    0x7fefffffffffffffll /*Max+ */,
    0x7ff0000000000000ll /*+Inf*/,
    0xfff0000000000000ll /*-Inf*/,
    0x7ff0000000000001ll /*SNaN*/,
    0x7ff8000000000000ll /*QNaN*/,
    0x7ffffffffffffffell, 0x7fffffffffffffffll, 0x8000000000000000ll,
    0x8000000000000001ll, 0xfffffffffffffffell, 0xffffffffffffffffll };

const static size_t NumValues = sizeof(Values) / sizeof(*Values);

struct {
  volatile uint8_t l8;
  volatile uint16_t l16;
  volatile uint32_t l32;
  volatile uint64_t l64;
} AtomicLocs;

template <typename Type>
void testAtomicRMW(volatile Type *AtomicLoc,
                   size_t &TotalTests, size_t &Passes, size_t &Failures) {
  typedef Type (*FuncType)(bool, volatile Type*, Type);
  static struct {
    const char *Name;
    FuncType FuncLlc;
    FuncType FuncSz;
  } Funcs[] = {
#define X(inst)                                                             \
  {                                                                         \
    STR(inst), test_##inst, Subzero_::test_##inst                           \
  },                                                                        \
  {                                                                         \
    STR(inst) "_alloca", test_alloca_##inst, Subzero_::test_alloca_##inst   \
  },                                                                        \
  {                                                                         \
    STR(inst) "_const", test_const_##inst, Subzero_::test_const_##inst      \
  },
      RMWOP_TABLE
#undef X
  };
  const static size_t NumFuncs = sizeof(Funcs) / sizeof(*Funcs);

  for (size_t f = 0; f < NumFuncs; ++f) {
    for (size_t i = 0; i < NumValues; ++i) {
      Type Value1 = static_cast<Type>(Values[i]);
      for (size_t j = 0; j < NumValues; ++j) {
        Type Value2 = static_cast<Type>(Values[j]);
        for (size_t k = 0; k < 2; ++k) {
          bool fetch_first = k;
          ++TotalTests;
          *AtomicLoc = Value1;
          Type ResultSz1 = Funcs[f].FuncSz(
              fetch_first, AtomicLoc, Value2);
          Type ResultSz2 = *AtomicLoc;
          *AtomicLoc = Value1;
          Type ResultLlc1 = Funcs[f].FuncLlc(
              fetch_first, AtomicLoc, Value2);
          Type ResultLlc2 = *AtomicLoc;
          if (ResultSz1 == ResultLlc1 && ResultSz2 == ResultLlc2) {
            ++Passes;
          } else {
            ++Failures;
            std::cout << "test_" << Funcs[f].Name
                      << (CHAR_BIT * sizeof(Type)) << "("
                      << static_cast<uint64_t>(Value1) << ", "
                      << static_cast<uint64_t>(Value2)
                      << "): sz1=" << static_cast<uint64_t>(ResultSz1)
                      << " llc1=" << static_cast<uint64_t>(ResultLlc1)
                      << " sz2=" << static_cast<uint64_t>(ResultSz2)
                      << " llc2=" << static_cast<uint64_t>(ResultLlc2)
                      << "\n";
          }
        }
      }
    }
  }
}

template <typename Type>
void testValCompareAndSwap(volatile Type *AtomicLoc, size_t &TotalTests,
                           size_t &Passes, size_t &Failures) {
  for (size_t i = 0; i < NumValues; ++i) {
    Type Value1 = static_cast<Type>(Values[i]);
    for (size_t j = 0; j < NumValues; ++j) {
      Type Value2 = static_cast<Type>(Values[j]);
      for (size_t f = 0; f < 2; ++f) {
        bool flip = f;
        ++TotalTests;
        *AtomicLoc = Value1;
        Type ResultSz1 = Subzero_::test_val_cmp_swap(
            AtomicLoc, flip ? Value2 : Value1, Value2);
        Type ResultSz2 = *AtomicLoc;
        *AtomicLoc = Value1;
        Type ResultLlc1 = test_val_cmp_swap(
            AtomicLoc, flip ? Value2 : Value1, Value2);
        Type ResultLlc2 = *AtomicLoc;
        if (ResultSz1 == ResultLlc1 && ResultSz2 == ResultLlc2) {
          ++Passes;
        } else {
          ++Failures;
          std::cout << "test_val_cmp_swap" << (CHAR_BIT * sizeof(Type)) << "("
                    << static_cast<uint64_t>(Value1) << ", "
                    << static_cast<uint64_t>(Value2)
                    << "): sz1=" << static_cast<uint64_t>(ResultSz1)
                    << " llc1=" << static_cast<uint64_t>(ResultLlc1)
                    << " sz2=" << static_cast<uint64_t>(ResultSz2)
                    << " llc2=" << static_cast<uint64_t>(ResultLlc2)
                    << "\n";
        }
      }
    }
  }
}

template <typename Type>
struct ThreadData {
  Type (*FuncPtr)(bool, volatile Type*, Type);
  bool Fetch;
  volatile Type *Ptr;
  Type Adjustment;
};

template <typename Type>
void *threadWrapper(void *Data) {
  const size_t NumReps = 8000;
  ThreadData<Type> *TData = reinterpret_cast<ThreadData<Type>*>(Data);
  for (size_t i = 0; i < NumReps; ++i) {
    (void)TData->FuncPtr(TData->Fetch, TData->Ptr, TData->Adjustment);
  }
  return NULL;
}

template <typename Type>
void testAtomicRMWThreads(volatile Type *AtomicLoc, size_t &TotalTests,
                          size_t &Passes, size_t &Failures) {
  typedef Type (*FuncType)(bool, volatile Type*, Type);
  static struct {
    const char *Name;
    FuncType FuncLlc;
    FuncType FuncSz;
  } Funcs[] = {
#define X(inst)                                                             \
  {                                                                         \
    STR(inst), test_##inst, Subzero_::test_##inst                           \
  },                                                                        \
  {                                                                         \
    STR(inst) "_alloca", test_alloca_##inst, Subzero_::test_alloca_##inst   \
  },
      RMWOP_TABLE
#undef X
  };
  const static size_t NumFuncs = sizeof(Funcs) / sizeof(*Funcs);

  // Just test a few values, otherwise it takes a *really* long time.
  volatile uint64_t ValuesSubset[] = { 1, 0x7e, 0x000fffffffffffffffll };
  const size_t NumValuesSubset = sizeof(ValuesSubset) / sizeof(*ValuesSubset);

  for (size_t f = 0; f < NumFuncs; ++f) {
    for (size_t i = 0; i < NumValuesSubset; ++i) {
      Type Value1 = static_cast<Type>(ValuesSubset[i]);
      for (size_t j = 0; j < NumValuesSubset; ++j) {
        Type Value2 = static_cast<Type>(ValuesSubset[j]);
        bool fetch_first = true;
        ThreadData<Type> TDataSz = {
          Funcs[f].FuncSz, fetch_first, AtomicLoc, Value2 };
        ThreadData<Type> TDataLlc = {
          Funcs[f].FuncLlc, fetch_first, AtomicLoc, Value2 };
        ++TotalTests;
        const size_t NumThreads = 4;
        pthread_t t[NumThreads];

        // Try N threads w/ just Llc.
        *AtomicLoc = Value1;
        for (size_t m = 0; m < NumThreads; ++m) {
          pthread_create(&t[m], NULL, &threadWrapper<Type>,
                         reinterpret_cast<void *>(&TDataLlc));
        }
        for (size_t m = 0; m < NumThreads; ++m) {
          pthread_join(t[m], NULL);
        }
        Type ResultLlc = *AtomicLoc;

        // Try N threads w/ both Sz and Llc.
        *AtomicLoc = Value1;
        for (size_t m = 0; m < NumThreads; ++m) {
          if (pthread_create(&t[m], NULL, &threadWrapper<Type>,
                             m % 2 == 0
                             ? reinterpret_cast<void *>(&TDataLlc)
                             : reinterpret_cast<void *>(&TDataSz)) != 0) {
            ++Failures;
            std::cout << "pthread_create failed w/ " << strerror(errno) << "\n";
            abort();
          }
        }
        for (size_t m = 0; m < NumThreads; ++m) {
          if (pthread_join(t[m], NULL) != 0) {
            ++Failures;
            std::cout << "pthread_join failed w/ " << strerror(errno) << "\n";
            abort();
          }
        }
        Type ResultMixed = *AtomicLoc;

        if (ResultLlc == ResultMixed) {
          ++Passes;
        } else {
          ++Failures;
          std::cout << "test_with_threads_" << Funcs[f].Name
                    << (8 * sizeof(Type)) << "("
                    << static_cast<uint64_t>(Value1) << ", "
                    << static_cast<uint64_t>(Value2)
                    << "): llc=" << static_cast<uint64_t>(ResultLlc)
                    << " mixed=" << static_cast<uint64_t>(ResultMixed)
                    << "\n";
        }
      }
    }
  }
}

int main(int argc, char **argv) {
  size_t TotalTests = 0;
  size_t Passes = 0;
  size_t Failures = 0;

  testAtomicRMW<uint8_t>(&AtomicLocs.l8, TotalTests, Passes, Failures);
  testAtomicRMW<uint16_t>(&AtomicLocs.l16, TotalTests, Passes, Failures);
  testAtomicRMW<uint32_t>(&AtomicLocs.l32, TotalTests, Passes, Failures);
  testAtomicRMW<uint64_t>(&AtomicLocs.l64, TotalTests, Passes, Failures);
  testValCompareAndSwap<uint8_t>(
      &AtomicLocs.l8, TotalTests, Passes, Failures);
  testValCompareAndSwap<uint16_t>(
      &AtomicLocs.l16, TotalTests, Passes, Failures);
  testValCompareAndSwap<uint32_t>(
      &AtomicLocs.l32, TotalTests, Passes, Failures);
  testValCompareAndSwap<uint64_t>(
      &AtomicLocs.l64, TotalTests, Passes, Failures);
  testAtomicRMWThreads<uint8_t>(
      &AtomicLocs.l8, TotalTests, Passes, Failures);
  testAtomicRMWThreads<uint16_t>(
      &AtomicLocs.l16, TotalTests, Passes, Failures);
  testAtomicRMWThreads<uint32_t>(
      &AtomicLocs.l32, TotalTests, Passes, Failures);
  testAtomicRMWThreads<uint64_t>(
      &AtomicLocs.l64, TotalTests, Passes, Failures);

  std::cout << "TotalTests=" << TotalTests << " Passes=" << Passes
            << " Failures=" << Failures << "\n";
  return Failures;
}
