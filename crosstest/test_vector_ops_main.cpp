/* crosstest.py --test=test_vector_ops.ll  --driver=test_vector_ops_main.cpp \
   --prefix=Subzero_ --output=test_vector_ops */

#include <stdint.h>
#include <cstring>
#include <sstream>
#include <iostream>
#include <limits>
#include <utility>
#include <vector>
#include <stdlib.h>

#include "test_vector_ops.def"

// typedefs of native C++ SIMD vector types
#define X(ty, elty, castty) typedef elty ty __attribute__((vector_size(16)));
VECTOR_TYPE_TABLE
#undef X

// i1 vector types are not native C++ SIMD vector types. Instead, they
// are expanded by the test code into native 128 bit SIMD vector types
// with the appropriate number of elements. Representing the types in
// VectorOps<> requires a unique name for each type which this
// declaration provides.
#define X(ty, expandedty, num_elements)                                        \
  class ty;
I1_VECTOR_TYPE_TABLE
#undef X

template <typename T> struct VectorOps;

#define DECLARE_VECTOR_OPS(TYNAME, TY, ELTY, CASTTY, NUM_ELEMENTS)             \
  template <> struct VectorOps<TYNAME> {                                       \
    typedef TY Ty;                                                             \
    typedef ELTY ElementTy;                                                    \
    typedef CASTTY CastTy;                                                     \
    static TY (*insertelement)(TY, CASTTY, int32_t);                           \
    static TY (*Subzero_insertelement)(TY, CASTTY, int32_t);                   \
    static CASTTY (*extractelement)(TY, int32_t);                              \
    static CASTTY (*Subzero_extractelement)(TY, int32_t);                      \
    static size_t NumElements;                                                 \
    static const char *TypeName;                                               \
  };                                                                           \
  extern "C" TY insertelement_##TYNAME(TY, CASTTY, int32_t);                   \
  extern "C" TY Subzero_insertelement_##TYNAME(TY, CASTTY, int32_t);           \
  extern "C" CASTTY extractelement_##TYNAME(TY, int32_t);                      \
  extern "C" CASTTY Subzero_extractelement_##TYNAME(TY, int32_t);              \
  size_t VectorOps<TYNAME>::NumElements = NUM_ELEMENTS;                        \
  TY (*VectorOps<TYNAME>::insertelement)(TY, CASTTY, int32_t) =                \
      &insertelement_##TYNAME;                                                 \
  TY (*VectorOps<TYNAME>::Subzero_insertelement)(TY, CASTTY, int32_t) =        \
      &Subzero_insertelement_##TYNAME;                                         \
  CASTTY (*VectorOps<TYNAME>::extractelement)(TY, int32_t) =                   \
      &extractelement_##TYNAME;                                                \
  CASTTY (*VectorOps<TYNAME>::Subzero_extractelement)(TY, int32_t) =           \
      &Subzero_extractelement_##TYNAME;                                        \
  const char *VectorOps<TYNAME>::TypeName = #TYNAME;

#define X(ty, elty, castty)                                                    \
  DECLARE_VECTOR_OPS(ty, ty, elty, castty, (sizeof(ty) / sizeof(elty)))
VECTOR_TYPE_TABLE
#undef X

#define X(ty, expandedty, num_elements)                                        \
  DECLARE_VECTOR_OPS(ty, expandedty, bool, int64_t, num_elements)
I1_VECTOR_TYPE_TABLE
#undef X

template <typename T>
std::string vectAsString(const typename VectorOps<T>::Ty Vect) {
  std::ostringstream OS;
  for (size_t I = 0; I < VectorOps<T>::NumElements; ++I) {
    if (I > 0)
      OS << " ";
    OS << (typename VectorOps<T>::CastTy)Vect[I];
  }
  return OS.str();
}

template <typename T>
typename VectorOps<T>::Ty *getTestVectors(size_t &NumTestVectors) {
  typedef typename VectorOps<T>::Ty Ty;
  typedef typename VectorOps<T>::ElementTy ElementTy;

  Ty Zero;
  memset(&Zero, 0, sizeof(Zero));
  Ty Incr;
  // Note: The casts in the next two initializations are necessary,
  // since ElementTy isn't necessarily the type that the value is stored
  // in the vector.
  for (int I = 0; I < VectorOps<T>::NumElements; ++I)
    Incr[I] = (ElementTy)I;
  Ty Decr;
  for (int I = 0; I < VectorOps<T>::NumElements; ++I)
    Decr[I] = (ElementTy)-I;
  Ty Min;
  for (int I = 0; I < VectorOps<T>::NumElements; ++I)
    Min[I] = std::numeric_limits<ElementTy>::min();
  Ty Max;
  for (int I = 0; I < VectorOps<T>::NumElements; ++I)
    Max[I] = std::numeric_limits<ElementTy>::max();
  Ty TestVectors[] = {Zero, Incr, Decr, Min, Max};

  NumTestVectors = sizeof(TestVectors) / sizeof(Ty);

  const size_t VECTOR_ALIGNMENT = 16;
  void *Dest;
  if (posix_memalign(&Dest, VECTOR_ALIGNMENT, sizeof(TestVectors))) {
    std::cerr << "memory allocation error" << std::endl;
    abort();
  }

  memcpy(Dest, TestVectors, sizeof(TestVectors));

  return static_cast<Ty *>(Dest);
}

template <typename T>
void testInsertElement(size_t &TotalTests, size_t &Passes, size_t &Failures) {
  typedef typename VectorOps<T>::Ty Ty;
  typedef typename VectorOps<T>::ElementTy ElementTy;

  size_t NumTestVectors;
  Ty *TestVectors = getTestVectors<T>(NumTestVectors);

  ElementTy TestElements[] = {0, 1, std::numeric_limits<ElementTy>::min(),
                              std::numeric_limits<ElementTy>::max()};
  const size_t NumTestElements = sizeof(TestElements) / sizeof(ElementTy);

  for (size_t VI = 0; VI < NumTestVectors; ++VI) {
    Ty Vect = TestVectors[VI];
    for (size_t EI = 0; EI < NumTestElements; ++EI) {
      ElementTy Elt = TestElements[EI];
      for (size_t I = 0; I < VectorOps<T>::NumElements; ++I) {
        Ty ResultLlc = VectorOps<T>::insertelement(Vect, Elt, I);
        Ty ResultSz = VectorOps<T>::Subzero_insertelement(Vect, Elt, I);
        ++TotalTests;
        if (!memcmp(&ResultLlc, &ResultSz, sizeof(ResultLlc))) {
          ++Passes;
        } else {
          ++Failures;
          std::cout << "insertelement<" << VectorOps<T>::TypeName << ">(Vect=";
          std::cout << vectAsString<T>(Vect)
                    << ", Element=" << (typename VectorOps<T>::CastTy)Elt
                    << ", Pos=" << I << ")" << std::endl;
          std::cout << "llc=" << vectAsString<T>(ResultLlc) << std::endl;
          std::cout << "sz =" << vectAsString<T>(ResultSz) << std::endl;
        }
      }
    }
  }

  free(TestVectors);
}

template <typename T>
void testExtractElement(size_t &TotalTests, size_t &Passes, size_t &Failures) {
  typedef typename VectorOps<T>::Ty Ty;
  typedef typename VectorOps<T>::ElementTy ElementTy;
  typedef typename VectorOps<T>::CastTy CastTy;

  size_t NumTestVectors;
  Ty *TestVectors = getTestVectors<T>(NumTestVectors);

  for (size_t VI = 0; VI < NumTestVectors; ++VI) {
    Ty Vect = TestVectors[VI];
    for (size_t I = 0; I < VectorOps<T>::NumElements; ++I) {
      CastTy ResultLlc = VectorOps<T>::extractelement(Vect, I);
      CastTy ResultSz = VectorOps<T>::Subzero_extractelement(Vect, I);
      ++TotalTests;
      if (!memcmp(&ResultLlc, &ResultSz, sizeof(ResultLlc))) {
        ++Passes;
      } else {
        ++Failures;
        std::cout << "extractelement<" << VectorOps<T>::TypeName << ">(Vect=";
        std::cout << vectAsString<T>(Vect) << ", Pos=" << I << ")" << std::endl;
        std::cout << "llc=" << ResultLlc << std::endl;
        std::cout << "sz =" << ResultSz << std::endl;
      }
    }
  }

  free(TestVectors);
}

int main(int argc, char *argv[]) {
  size_t TotalTests = 0;
  size_t Passes = 0;
  size_t Failures = 0;

  testInsertElement<v4i1>(TotalTests, Passes, Failures);
  testInsertElement<v8i1>(TotalTests, Passes, Failures);
  testInsertElement<v16i1>(TotalTests, Passes, Failures);
  testInsertElement<v16si8>(TotalTests, Passes, Failures);
  testInsertElement<v16ui8>(TotalTests, Passes, Failures);
  testInsertElement<v8si16>(TotalTests, Passes, Failures);
  testInsertElement<v8ui16>(TotalTests, Passes, Failures);
  testInsertElement<v4si32>(TotalTests, Passes, Failures);
  testInsertElement<v4ui32>(TotalTests, Passes, Failures);
  testInsertElement<v4f32>(TotalTests, Passes, Failures);

  testExtractElement<v4i1>(TotalTests, Passes, Failures);
  testExtractElement<v8i1>(TotalTests, Passes, Failures);
  testExtractElement<v16i1>(TotalTests, Passes, Failures);
  testExtractElement<v16si8>(TotalTests, Passes, Failures);
  testExtractElement<v16ui8>(TotalTests, Passes, Failures);
  testExtractElement<v8si16>(TotalTests, Passes, Failures);
  testExtractElement<v8ui16>(TotalTests, Passes, Failures);
  testExtractElement<v4si32>(TotalTests, Passes, Failures);
  testExtractElement<v4ui32>(TotalTests, Passes, Failures);
  testExtractElement<v4f32>(TotalTests, Passes, Failures);

  std::cout << "TotalTests=" << TotalTests << " Passes=" << Passes
            << " Failures=" << Failures << "\n";

  return Failures;
}

extern "C" {

void ice_unreachable(void) {
  std::cerr << "\"unreachable\" instruction encountered" << std::endl;
  abort();
}
}
