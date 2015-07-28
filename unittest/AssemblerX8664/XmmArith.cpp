//===- subzero/unittest/AssemblerX8664/XmmArith.cpp -----------------------===//
//
//                        The Subzero Code Generator
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include "AssemblerX8664/TestUtil.h"

namespace Ice {
namespace X8664 {
namespace Test {
namespace {

TEST_F(AssemblerX8664Test, ArithSS) {
#define TestArithSSXmmXmm(FloatSize, Src, Value0, Dst, Value1, Inst, Op)       \
  do {                                                                         \
    static_assert(FloatSize == 32 || FloatSize == 64,                          \
                  "Invalid fp size " #FloatSize);                              \
    static constexpr char TestString[] =                                       \
        "(" #FloatSize ", " #Src ", " #Value0 ", " #Dst ", " #Value1           \
        ", " #Inst ", " #Op ")";                                               \
    static constexpr bool IsDouble = FloatSize == 64;                          \
    using Type = std::conditional<IsDouble, double, float>::type;              \
    const uint32_t T0 = allocateQword();                                       \
    const Type V0 = Value0;                                                    \
    const uint32_t T1 = allocateQword();                                       \
    const Type V1 = Value1;                                                    \
                                                                               \
    __ movss(IceType_f##FloatSize, Encoded_Xmm_##Dst(), dwordAddress(T0));     \
    __ movss(IceType_f##FloatSize, Encoded_Xmm_##Src(), dwordAddress(T1));     \
    __ Inst(IceType_f##FloatSize, Encoded_Xmm_##Dst(), Encoded_Xmm_##Src());   \
                                                                               \
    AssembledTest test = assemble();                                           \
    if (IsDouble) {                                                            \
      test.setQwordTo(T0, static_cast<double>(V0));                            \
      test.setQwordTo(T1, static_cast<double>(V1));                            \
    } else {                                                                   \
      test.setDwordTo(T0, static_cast<float>(V0));                             \
      test.setDwordTo(T1, static_cast<float>(V1));                             \
    }                                                                          \
                                                                               \
    test.run();                                                                \
                                                                               \
    ASSERT_DOUBLE_EQ(V0 Op V1, test.Dst<Type>()) << TestString;                \
    reset();                                                                   \
  } while (0)

#define TestArithSSXmmAddr(FloatSize, Value0, Dst, Value1, Inst, Op)           \
  do {                                                                         \
    static_assert(FloatSize == 32 || FloatSize == 64,                          \
                  "Invalid fp size " #FloatSize);                              \
    static constexpr char TestString[] =                                       \
        "(" #FloatSize ", Addr, " #Value0 ", " #Dst ", " #Value1 ", " #Inst    \
        ", " #Op ")";                                                          \
    static constexpr bool IsDouble = FloatSize == 64;                          \
    using Type = std::conditional<IsDouble, double, float>::type;              \
    const uint32_t T0 = allocateQword();                                       \
    const Type V0 = Value0;                                                    \
    const uint32_t T1 = allocateQword();                                       \
    const Type V1 = Value1;                                                    \
                                                                               \
    __ movss(IceType_f##FloatSize, Encoded_Xmm_##Dst(), dwordAddress(T0));     \
    __ Inst(IceType_f##FloatSize, Encoded_Xmm_##Dst(), dwordAddress(T1));      \
                                                                               \
    AssembledTest test = assemble();                                           \
    if (IsDouble) {                                                            \
      test.setQwordTo(T0, static_cast<double>(V0));                            \
      test.setQwordTo(T1, static_cast<double>(V1));                            \
    } else {                                                                   \
      test.setDwordTo(T0, static_cast<float>(V0));                             \
      test.setDwordTo(T1, static_cast<float>(V1));                             \
    }                                                                          \
                                                                               \
    test.run();                                                                \
                                                                               \
    ASSERT_DOUBLE_EQ(V0 Op V1, test.Dst<Type>()) << TestString;                \
    reset();                                                                   \
  } while (0)

#define TestArithSS(FloatSize, Src, Dst0, Dst1)                                \
  do {                                                                         \
    TestArithSSXmmXmm(FloatSize, Src, 1.0, Dst0, 10.0, addss, +);              \
    TestArithSSXmmAddr(FloatSize, 2.0, Dst1, 20.0, addss, +);                  \
    TestArithSSXmmXmm(FloatSize, Src, 3.0, Dst0, 30.0, subss, -);              \
    TestArithSSXmmAddr(FloatSize, 4.0, Dst1, 40.0, subss, -);                  \
    TestArithSSXmmXmm(FloatSize, Src, 5.0, Dst0, 50.0, mulss, *);              \
    TestArithSSXmmAddr(FloatSize, 6.0, Dst1, 60.0, mulss, *);                  \
    TestArithSSXmmXmm(FloatSize, Src, 7.0, Dst0, 70.0, divss, / );             \
    TestArithSSXmmAddr(FloatSize, 8.0, Dst1, 80.0, divss, / );                 \
  } while (0)

#define TestImpl(Src, Dst0, Dst1)                                              \
  do {                                                                         \
    TestArithSS(32, Src, Dst0, Dst1);                                          \
    TestArithSS(64, Src, Dst0, Dst1);                                          \
  } while (0)

  TestImpl(xmm0, xmm1, xmm2);
  TestImpl(xmm1, xmm2, xmm3);
  TestImpl(xmm2, xmm3, xmm4);
  TestImpl(xmm3, xmm4, xmm5);
  TestImpl(xmm4, xmm5, xmm6);
  TestImpl(xmm5, xmm6, xmm7);
  TestImpl(xmm6, xmm7, xmm8);
  TestImpl(xmm7, xmm8, xmm9);
  TestImpl(xmm8, xmm9, xmm10);
  TestImpl(xmm9, xmm10, xmm11);
  TestImpl(xmm10, xmm11, xmm12);
  TestImpl(xmm11, xmm12, xmm13);
  TestImpl(xmm12, xmm13, xmm14);
  TestImpl(xmm13, xmm14, xmm15);
  TestImpl(xmm14, xmm15, xmm0);
  TestImpl(xmm15, xmm0, xmm1);

#undef TestImpl
#undef TestArithSS
#undef TestArithSSXmmAddr
#undef TestArithSSXmmXmm
}

TEST_F(AssemblerX8664Test, PArith) {
#define TestPArithXmmXmm(Dst, Value0, Src, Value1, Inst, Op, Type, Size)       \
  do {                                                                         \
    static constexpr char TestString[] =                                       \
        "(" #Dst ", " #Value0 ", " #Src ", " #Value1 ", " #Inst ", " #Op       \
        ", " #Type ", " #Size ")";                                             \
    const uint32_t T0 = allocateDqword();                                      \
    const Dqword V0 Value0;                                                    \
                                                                               \
    const uint32_t T1 = allocateDqword();                                      \
    const Dqword V1 Value1;                                                    \
                                                                               \
    __ movups(Encoded_Xmm_##Dst(), dwordAddress(T0));                          \
    __ movups(Encoded_Xmm_##Src(), dwordAddress(T1));                          \
    __ Inst(IceType_i##Size, Encoded_Xmm_##Dst(), Encoded_Xmm_##Src());        \
                                                                               \
    AssembledTest test = assemble();                                           \
    test.setDqwordTo(T0, V0);                                                  \
    test.setDqwordTo(T1, V1);                                                  \
    test.run();                                                                \
                                                                               \
    ASSERT_EQ(packedAs<Type##Size##_t>(V0) Op V1, test.Dst<Dqword>())          \
        << TestString;                                                         \
    reset();                                                                   \
  } while (0)

#define TestPArithXmmAddr(Dst, Value0, Value1, Inst, Op, Type, Size)           \
  do {                                                                         \
    static constexpr char TestString[] =                                       \
        "(" #Dst ", " #Value0 ", Addr, " #Value1 ", " #Inst ", " #Op           \
        ", " #Type ", " #Size ")";                                             \
    const uint32_t T0 = allocateDqword();                                      \
    const Dqword V0 Value0;                                                    \
                                                                               \
    const uint32_t T1 = allocateDqword();                                      \
    const Dqword V1 Value1;                                                    \
                                                                               \
    __ movups(Encoded_Xmm_##Dst(), dwordAddress(T0));                          \
    __ Inst(IceType_i##Size, Encoded_Xmm_##Dst(), dwordAddress(T1));           \
                                                                               \
    AssembledTest test = assemble();                                           \
    test.setDqwordTo(T0, V0);                                                  \
    test.setDqwordTo(T1, V1);                                                  \
    test.run();                                                                \
                                                                               \
    ASSERT_EQ(packedAs<Type##Size##_t>(V0) Op V1, test.Dst<Dqword>())          \
        << TestString;                                                         \
    reset();                                                                   \
  } while (0)

#define TestPArithXmmImm(Dst, Value0, Imm, Inst, Op, Type, Size)               \
  do {                                                                         \
    static constexpr char TestString[] =                                       \
        "(" #Dst ", " #Value0 ", " #Imm ", " #Inst ", " #Op ", " #Type         \
        ", " #Size ")";                                                        \
    const uint32_t T0 = allocateDqword();                                      \
    const Dqword V0 Value0;                                                    \
                                                                               \
    __ movups(Encoded_Xmm_##Dst(), dwordAddress(T0));                          \
    __ Inst(IceType_i##Size, Encoded_Xmm_##Dst(), Immediate(Imm));             \
                                                                               \
    AssembledTest test = assemble();                                           \
    test.setDqwordTo(T0, V0);                                                  \
    test.run();                                                                \
                                                                               \
    ASSERT_EQ(packedAs<Type##Size##_t>(V0) Op Imm, test.Dst<Dqword>())         \
        << TestString;                                                         \
    reset();                                                                   \
  } while (0)

#define TestPAndnXmmXmm(Dst, Value0, Src, Value1, Type, Size)                  \
  do {                                                                         \
    static constexpr char TestString[] =                                       \
        "(" #Dst ", " #Value0 ", " #Src ", " #Value1 ", pandn, " #Type         \
        ", " #Size ")";                                                        \
    const uint32_t T0 = allocateDqword();                                      \
    const Dqword V0 Value0;                                                    \
                                                                               \
    const uint32_t T1 = allocateDqword();                                      \
    const Dqword V1 Value1;                                                    \
                                                                               \
    __ movups(Encoded_Xmm_##Dst(), dwordAddress(T0));                          \
    __ movups(Encoded_Xmm_##Src(), dwordAddress(T1));                          \
    __ pandn(IceType_i##Size, Encoded_Xmm_##Dst(), Encoded_Xmm_##Src());       \
                                                                               \
    AssembledTest test = assemble();                                           \
    test.setDqwordTo(T0, V0);                                                  \
    test.setDqwordTo(T1, V1);                                                  \
    test.run();                                                                \
                                                                               \
    ASSERT_EQ(~(packedAs<Type##Size##_t>(V0)) & V1, test.Dst<Dqword>())        \
        << TestString;                                                         \
    reset();                                                                   \
  } while (0)

#define TestPAndnXmmAddr(Dst, Value0, Value1, Type, Size)                      \
  do {                                                                         \
    static constexpr char TestString[] =                                       \
        "(" #Dst ", " #Value0 ", Addr, " #Value1 ", pandn, " #Type ", " #Size  \
        ")";                                                                   \
    const uint32_t T0 = allocateDqword();                                      \
    const Dqword V0 Value0;                                                    \
                                                                               \
    const uint32_t T1 = allocateDqword();                                      \
    const Dqword V1 Value1;                                                    \
                                                                               \
    __ movups(Encoded_Xmm_##Dst(), dwordAddress(T0));                          \
    __ pandn(IceType_i##Size, Encoded_Xmm_##Dst(), dwordAddress(T1));          \
                                                                               \
    AssembledTest test = assemble();                                           \
    test.setDqwordTo(T0, V0);                                                  \
    test.setDqwordTo(T1, V1);                                                  \
    test.run();                                                                \
                                                                               \
    ASSERT_EQ((~packedAs<Type##Size##_t>(V0)) & V1, test.Dst<Dqword>())        \
        << TestString;                                                         \
    reset();                                                                   \
  } while (0)

#define TestPArithSize(Dst, Src, Size)                                         \
  do {                                                                         \
    static_assert(Size == 8 || Size == 16 || Size == 32, "Invalid size.");     \
    if (Size != 8) {                                                           \
      TestPArithXmmXmm(                                                        \
          Dst,                                                                 \
          (uint64_t(0x8040201008040201ull), uint64_t(0x8080404002020101ull)),  \
          Src, (uint64_t(3u), uint64_t(0u)), psra, >>, int, Size);             \
      TestPArithXmmAddr(Dst, (uint64_t(0x8040201008040201ull),                 \
                              uint64_t(0x8080404002020101ull)),                \
                        (uint64_t(3u), uint64_t(0u)), psra, >>, int, Size);    \
      TestPArithXmmImm(Dst, (uint64_t(0x8040201008040201ull),                  \
                             uint64_t(0x8080404002020101ull)),                 \
                       3u, psra, >>, int, Size);                               \
      TestPArithXmmXmm(                                                        \
          Dst,                                                                 \
          (uint64_t(0x8040201008040201ull), uint64_t(0x8080404002020101ull)),  \
          Src, (uint64_t(3u), uint64_t(0u)), psrl, >>, uint, Size);            \
      TestPArithXmmAddr(Dst, (uint64_t(0x8040201008040201ull),                 \
                              uint64_t(0x8080404002020101ull)),                \
                        (uint64_t(3u), uint64_t(0u)), psrl, >>, uint, Size);   \
      TestPArithXmmImm(Dst, (uint64_t(0x8040201008040201ull),                  \
                             uint64_t(0x8080404002020101ull)),                 \
                       3u, psrl, >>, uint, Size);                              \
      TestPArithXmmXmm(                                                        \
          Dst,                                                                 \
          (uint64_t(0x8040201008040201ull), uint64_t(0x8080404002020101ull)),  \
          Src, (uint64_t(3u), uint64_t(0u)), psll, <<, uint, Size);            \
      TestPArithXmmAddr(Dst, (uint64_t(0x8040201008040201ull),                 \
                              uint64_t(0x8080404002020101ull)),                \
                        (uint64_t(3u), uint64_t(0u)), psll, <<, uint, Size);   \
      TestPArithXmmImm(Dst, (uint64_t(0x8040201008040201ull),                  \
                             uint64_t(0x8080404002020101ull)),                 \
                       3u, psll, <<, uint, Size);                              \
                                                                               \
      TestPArithXmmXmm(Dst, (uint64_t(0x8040201008040201ull),                  \
                             uint64_t(0x8080404002020101ull)),                 \
                       Src, (uint64_t(0xFFFFFFFF00000000ull),                  \
                             uint64_t(0x0123456789ABCDEull)),                  \
                       pmull, *, int, Size);                                   \
      TestPArithXmmAddr(                                                       \
          Dst,                                                                 \
          (uint64_t(0x8040201008040201ull), uint64_t(0x8080404002020101ull)),  \
          (uint64_t(0xFFFFFFFF00000000ull), uint64_t(0x0123456789ABCDEull)),   \
          pmull, *, int, Size);                                                \
      if (Size != 16) {                                                        \
        TestPArithXmmXmm(Dst, (uint64_t(0x8040201008040201ull),                \
                               uint64_t(0x8080404002020101ull)),               \
                         Src, (uint64_t(0xFFFFFFFF00000000ull),                \
                               uint64_t(0x0123456789ABCDEull)),                \
                         pmuludq, *, uint, Size);                              \
        TestPArithXmmAddr(                                                     \
            Dst, (uint64_t(0x8040201008040201ull),                             \
                  uint64_t(0x8080404002020101ull)),                            \
            (uint64_t(0xFFFFFFFF00000000ull), uint64_t(0x0123456789ABCDEull)), \
            pmuludq, *, uint, Size);                                           \
      }                                                                        \
    }                                                                          \
    TestPArithXmmXmm(Dst, (uint64_t(0x8040201008040201ull),                    \
                           uint64_t(0x8080404002020101ull)),                   \
                     Src, (uint64_t(0xFFFFFFFF00000000ull),                    \
                           uint64_t(0x0123456789ABCDEull)),                    \
                     padd, +, int, Size);                                      \
    TestPArithXmmAddr(                                                         \
        Dst,                                                                   \
        (uint64_t(0x8040201008040201ull), uint64_t(0x8080404002020101ull)),    \
        (uint64_t(0xFFFFFFFF00000000ull), uint64_t(0x0123456789ABCDEull)),     \
        padd, +, int, Size);                                                   \
    TestPArithXmmXmm(Dst, (uint64_t(0x8040201008040201ull),                    \
                           uint64_t(0x8080404002020101ull)),                   \
                     Src, (uint64_t(0xFFFFFFFF00000000ull),                    \
                           uint64_t(0x0123456789ABCDEull)),                    \
                     psub, -, int, Size);                                      \
    TestPArithXmmAddr(                                                         \
        Dst,                                                                   \
        (uint64_t(0x8040201008040201ull), uint64_t(0x8080404002020101ull)),    \
        (uint64_t(0xFFFFFFFF00000000ull), uint64_t(0x0123456789ABCDEull)),     \
        psub, -, int, Size);                                                   \
    TestPArithXmmXmm(Dst, (uint64_t(0x8040201008040201ull),                    \
                           uint64_t(0x8080404002020101ull)),                   \
                     Src, (uint64_t(0xFFFFFFFF00000000ull),                    \
                           uint64_t(0x0123456789ABCDEull)),                    \
                     pand, &, int, Size);                                      \
    TestPArithXmmAddr(                                                         \
        Dst,                                                                   \
        (uint64_t(0x8040201008040201ull), uint64_t(0x8080404002020101ull)),    \
        (uint64_t(0xFFFFFFFF00000000ull), uint64_t(0x0123456789ABCDEull)),     \
        pand, &, int, Size);                                                   \
                                                                               \
    TestPAndnXmmXmm(Dst, (uint64_t(0x8040201008040201ull),                     \
                          uint64_t(0x8080404002020101ull)),                    \
                    Src, (uint64_t(0xFFFFFFFF00000000ull),                     \
                          uint64_t(0x0123456789ABCDEull)),                     \
                    int, Size);                                                \
    TestPAndnXmmAddr(                                                          \
        Dst,                                                                   \
        (uint64_t(0x8040201008040201ull), uint64_t(0x8080404002020101ull)),    \
        (uint64_t(0xFFFFFFFF00000000ull), uint64_t(0x0123456789ABCDEull)),     \
        int, Size);                                                            \
                                                                               \
    TestPArithXmmXmm(Dst, (uint64_t(0x8040201008040201ull),                    \
                           uint64_t(0x8080404002020101ull)),                   \
                     Src, (uint64_t(0xFFFFFFFF00000000ull),                    \
                           uint64_t(0x0123456789ABCDEull)),                    \
                     por, |, int, Size);                                       \
    TestPArithXmmAddr(                                                         \
        Dst,                                                                   \
        (uint64_t(0x8040201008040201ull), uint64_t(0x8080404002020101ull)),    \
        (uint64_t(0xFFFFFFFF00000000ull), uint64_t(0x0123456789ABCDEull)),     \
        por, |, int, Size);                                                    \
    TestPArithXmmXmm(Dst, (uint64_t(0x8040201008040201ull),                    \
                           uint64_t(0x8080404002020101ull)),                   \
                     Src, (uint64_t(0xFFFFFFFF00000000ull),                    \
                           uint64_t(0x0123456789ABCDEull)),                    \
                     pxor, ^, int, Size);                                      \
    TestPArithXmmAddr(                                                         \
        Dst,                                                                   \
        (uint64_t(0x8040201008040201ull), uint64_t(0x8080404002020101ull)),    \
        (uint64_t(0xFFFFFFFF00000000ull), uint64_t(0x0123456789ABCDEull)),     \
        pxor, ^, int, Size);                                                   \
  } while (0)

#define TestPArith(Src, Dst)                                                   \
  do {                                                                         \
    TestPArithSize(Src, Dst, 8);                                               \
    TestPArithSize(Src, Dst, 16);                                              \
    TestPArithSize(Src, Dst, 32);                                              \
  } while (0)

  TestPArith(xmm0, xmm1);
  TestPArith(xmm1, xmm2);
  TestPArith(xmm2, xmm3);
  TestPArith(xmm3, xmm4);
  TestPArith(xmm4, xmm5);
  TestPArith(xmm5, xmm6);
  TestPArith(xmm6, xmm7);
  TestPArith(xmm7, xmm8);
  TestPArith(xmm8, xmm9);
  TestPArith(xmm9, xmm10);
  TestPArith(xmm10, xmm11);
  TestPArith(xmm11, xmm12);
  TestPArith(xmm12, xmm13);
  TestPArith(xmm13, xmm14);
  TestPArith(xmm14, xmm15);
  TestPArith(xmm15, xmm0);

#undef TestPArith
#undef TestPArithSize
#undef TestPAndnXmmAddr
#undef TestPAndnXmmXmm
#undef TestPArithXmmImm
#undef TestPArithXmmAddr
#undef TestPArithXmmXmm
}

TEST_F(AssemblerX8664Test, ArithPS) {
#define TestArithPSXmmXmm(Dst, Value0, Src, Value1, Inst, Op, Type)            \
  do {                                                                         \
    static constexpr char TestString[] =                                       \
        "(" #Dst ", " #Value0 ", " #Src ", " #Value1 ", " #Inst ", " #Op       \
        ", " #Type ")";                                                        \
    const uint32_t T0 = allocateDqword();                                      \
    const Dqword V0 Value0;                                                    \
    const uint32_t T1 = allocateDqword();                                      \
    const Dqword V1 Value1;                                                    \
                                                                               \
    __ movups(Encoded_Xmm_##Dst(), dwordAddress(T0));                          \
    __ movups(Encoded_Xmm_##Src(), dwordAddress(T1));                          \
    __ Inst(IceType_f32, Encoded_Xmm_##Dst(), Encoded_Xmm_##Src());            \
                                                                               \
    AssembledTest test = assemble();                                           \
    test.setDqwordTo(T0, V0);                                                  \
    test.setDqwordTo(T1, V1);                                                  \
    test.run();                                                                \
                                                                               \
    ASSERT_EQ(packedAs<Type>(V0) Op V1, test.Dst<Dqword>()) << TestString;     \
                                                                               \
    reset();                                                                   \
  } while (0)

#define TestArithPSXmmXmmUntyped(Dst, Value0, Src, Value1, Inst, Op, Type)     \
  do {                                                                         \
    static constexpr char TestString[] =                                       \
        "(" #Dst ", " #Value0 ", " #Src ", " #Value1 ", " #Inst ", " #Op       \
        ", " #Type ")";                                                        \
    const uint32_t T0 = allocateDqword();                                      \
    const Dqword V0 Value0;                                                    \
    const uint32_t T1 = allocateDqword();                                      \
    const Dqword V1 Value1;                                                    \
                                                                               \
    __ movups(Encoded_Xmm_##Dst(), dwordAddress(T0));                          \
    __ movups(Encoded_Xmm_##Src(), dwordAddress(T1));                          \
    __ Inst(Encoded_Xmm_##Dst(), Encoded_Xmm_##Src());                         \
                                                                               \
    AssembledTest test = assemble();                                           \
    test.setDqwordTo(T0, V0);                                                  \
    test.setDqwordTo(T1, V1);                                                  \
    test.run();                                                                \
                                                                               \
    ASSERT_EQ(packedAs<Type>(V0) Op V1, test.Dst<Dqword>()) << TestString;     \
                                                                               \
    reset();                                                                   \
  } while (0)

#define TestArithPSXmmAddrUntyped(Dst, Value0, Value1, Inst, Op, Type)         \
  do {                                                                         \
    static constexpr char TestString[] =                                       \
        "(" #Dst ", " #Value0 ", Addr, " #Value1 ", " #Inst ", " #Op           \
        ", " #Type ")";                                                        \
    const uint32_t T0 = allocateDqword();                                      \
    const Dqword V0 Value0;                                                    \
    const uint32_t T1 = allocateDqword();                                      \
    const Dqword V1 Value1;                                                    \
                                                                               \
    __ movups(Encoded_Xmm_##Dst(), dwordAddress(T0));                          \
    __ Inst(Encoded_Xmm_##Dst(), dwordAddress(T1));                            \
                                                                               \
    AssembledTest test = assemble();                                           \
    test.setDqwordTo(T0, V0);                                                  \
    test.setDqwordTo(T1, V1);                                                  \
    test.run();                                                                \
                                                                               \
    ASSERT_EQ(packedAs<Type>(V0) Op V1, test.Dst<Dqword>()) << TestString;     \
                                                                               \
    reset();                                                                   \
  } while (0)

#define TestMinMaxPS(Dst, Value0, Src, Value1, Inst, Type)                     \
  do {                                                                         \
    static constexpr char TestString[] =                                       \
        "(" #Dst ", " #Value0 ", " #Src ", " #Value1 ", " #Inst ", " #Type     \
        ")";                                                                   \
    const uint32_t T0 = allocateDqword();                                      \
    const Dqword V0 Value0;                                                    \
    const uint32_t T1 = allocateDqword();                                      \
    const Dqword V1 Value1;                                                    \
                                                                               \
    __ movups(Encoded_Xmm_##Dst(), dwordAddress(T0));                          \
    __ movups(Encoded_Xmm_##Src(), dwordAddress(T1));                          \
    __ Inst(Encoded_Xmm_##Dst(), Encoded_Xmm_##Src());                         \
                                                                               \
    AssembledTest test = assemble();                                           \
    test.setDqwordTo(T0, V0);                                                  \
    test.setDqwordTo(T1, V1);                                                  \
    test.run();                                                                \
                                                                               \
    ASSERT_EQ(packedAs<Type>(V0).Inst(V1), test.Dst<Dqword>()) << TestString;  \
                                                                               \
    reset();                                                                   \
  } while (0)

#define TestArithPSXmmAddr(Dst, Value0, Value1, Inst, Op, Type)                \
  do {                                                                         \
    static constexpr char TestString[] =                                       \
        "(" #Dst ", " #Value0 ", Addr, " #Value1 ", " #Inst ", " #Op           \
        ", " #Type ")";                                                        \
    const uint32_t T0 = allocateDqword();                                      \
    const Dqword V0 Value0;                                                    \
    const uint32_t T1 = allocateDqword();                                      \
    const Dqword V1 Value1;                                                    \
                                                                               \
    __ movups(Encoded_Xmm_##Dst(), dwordAddress(T0));                          \
    __ Inst(IceType_f32, Encoded_Xmm_##Dst(), dwordAddress(T1));               \
                                                                               \
    AssembledTest test = assemble();                                           \
    test.setDqwordTo(T0, V0);                                                  \
    test.setDqwordTo(T1, V1);                                                  \
    test.run();                                                                \
                                                                               \
    ASSERT_EQ(packedAs<Type>(V0) Op V1, test.Dst<Dqword>()) << TestString;     \
                                                                               \
    reset();                                                                   \
  } while (0)

#define TestArithPS(Dst, Src)                                                  \
  do {                                                                         \
    TestArithPSXmmXmm(Dst, (1.0, 100.0, -1000.0, 20.0), Src,                   \
                      (0.55, 0.43, 0.23, 1.21), addps, +, float);              \
    TestArithPSXmmAddr(Dst, (1.0, 100.0, -1000.0, 20.0),                       \
                       (0.55, 0.43, 0.23, 1.21), addps, +, float);             \
    TestArithPSXmmXmm(Dst, (1.0, 100.0, -1000.0, 20.0), Src,                   \
                      (0.55, 0.43, 0.23, 1.21), subps, -, float);              \
    TestArithPSXmmAddr(Dst, (1.0, 100.0, -1000.0, 20.0),                       \
                       (0.55, 0.43, 0.23, 1.21), subps, -, float);             \
    TestArithPSXmmXmm(Dst, (1.0, 100.0, -1000.0, 20.0), Src,                   \
                      (0.55, 0.43, 0.23, 1.21), mulps, *, float);              \
    TestArithPSXmmAddr(Dst, (1.0, 100.0, -1000.0, 20.0),                       \
                       (0.55, 0.43, 0.23, 1.21), mulps, *, float);             \
    TestArithPSXmmXmm(Dst, (1.0, 100.0, -1000.0, 20.0), Src,                   \
                      (0.55, 0.43, 0.23, 1.21), divps, /, float);              \
    TestArithPSXmmAddr(Dst, (1.0, 100.0, -1000.0, 20.0),                       \
                       (0.55, 0.43, 0.23, 1.21), divps, /, float);             \
    TestArithPSXmmXmmUntyped(Dst, (1.0, 100.0, -1000.0, 20.0), Src,            \
                             (0.55, 0.43, 0.23, 1.21), andps, &, float);       \
    TestArithPSXmmAddrUntyped(Dst, (1.0, 100.0, -1000.0, 20.0),                \
                              (0.55, 0.43, 0.23, 1.21), andps, &, float);      \
    TestArithPSXmmXmmUntyped(Dst, (1.0, -1000.0), Src, (0.55, 1.21), andpd, &, \
                             double);                                          \
    TestArithPSXmmAddrUntyped(Dst, (1.0, -1000.0), (0.55, 1.21), andpd, &,     \
                              double);                                         \
    TestArithPSXmmXmmUntyped(Dst, (1.0, 100.0, -1000.0, 20.0), Src,            \
                             (0.55, 0.43, 0.23, 1.21), orps, |, float);        \
    TestArithPSXmmXmmUntyped(Dst, (1.0, -1000.0), Src, (0.55, 1.21), orpd, |,  \
                             double);                                          \
    TestMinMaxPS(Dst, (1.0, 100.0, -1000.0, 20.0), Src,                        \
                 (0.55, 0.43, 0.23, 1.21), minps, float);                      \
    TestMinMaxPS(Dst, (1.0, 100.0, -1000.0, 20.0), Src,                        \
                 (0.55, 0.43, 0.23, 1.21), maxps, float);                      \
    TestMinMaxPS(Dst, (1.0, -1000.0), Src, (0.55, 1.21), minpd, double);       \
    TestMinMaxPS(Dst, (1.0, -1000.0), Src, (0.55, 1.21), maxpd, double);       \
    TestArithPSXmmXmmUntyped(Dst, (1.0, 100.0, -1000.0, 20.0), Src,            \
                             (0.55, 0.43, 0.23, 1.21), xorps, ^, float);       \
    TestArithPSXmmAddrUntyped(Dst, (1.0, 100.0, -1000.0, 20.0),                \
                              (0.55, 0.43, 0.23, 1.21), xorps, ^, float);      \
    TestArithPSXmmXmmUntyped(Dst, (1.0, -1000.0), Src, (0.55, 1.21), xorpd, ^, \
                             double);                                          \
    TestArithPSXmmAddrUntyped(Dst, (1.0, -1000.0), (0.55, 1.21), xorpd, ^,     \
                              double);                                         \
  } while (0)

  TestArithPS(xmm0, xmm1);
  TestArithPS(xmm1, xmm2);
  TestArithPS(xmm2, xmm3);
  TestArithPS(xmm3, xmm4);
  TestArithPS(xmm4, xmm5);
  TestArithPS(xmm5, xmm6);
  TestArithPS(xmm6, xmm7);
  TestArithPS(xmm7, xmm8);
  TestArithPS(xmm8, xmm9);
  TestArithPS(xmm9, xmm10);
  TestArithPS(xmm10, xmm11);
  TestArithPS(xmm11, xmm12);
  TestArithPS(xmm12, xmm13);
  TestArithPS(xmm13, xmm14);
  TestArithPS(xmm14, xmm15);
  TestArithPS(xmm15, xmm0);

#undef TestArithPs
#undef TestMinMaxPS
#undef TestArithPSXmmXmmUntyped
#undef TestArithPSXmmAddr
#undef TestArithPSXmmXmm
}

TEST_F(AssemblerX8664Test, Blending) {
  using f32 = float;
  using i8 = uint8_t;

#define TestBlendingXmmXmm(Dst, Value0, Src, Value1, M /*ask*/, Inst, Type)    \
  do {                                                                         \
    static constexpr char TestString[] =                                       \
        "(" #Dst ", " #Value0 ", " #Src ", " #Value1 ", " #M ", " #Inst        \
        ", " #Type ")";                                                        \
    const uint32_t T0 = allocateDqword();                                      \
    const Dqword V0 Value0;                                                    \
    const uint32_t T1 = allocateDqword();                                      \
    const Dqword V1 Value1;                                                    \
    const uint32_t Mask = allocateDqword();                                    \
    const Dqword MaskValue M;                                                  \
                                                                               \
    __ movups(Encoded_Xmm_xmm0(), dwordAddress(Mask));                         \
    __ movups(Encoded_Xmm_##Dst(), dwordAddress(T0));                          \
    __ movups(Encoded_Xmm_##Src(), dwordAddress(T1));                          \
    __ Inst(IceType_##Type, Encoded_Xmm_##Dst(), Encoded_Xmm_##Src());         \
                                                                               \
    AssembledTest test = assemble();                                           \
    test.setDqwordTo(T0, V0);                                                  \
    test.setDqwordTo(T1, V1);                                                  \
    test.setDqwordTo(Mask, MaskValue);                                         \
    test.run();                                                                \
                                                                               \
    ASSERT_EQ(packedAs<Type>(V0).blendWith(V1, MaskValue), test.Dst<Dqword>()) \
        << TestString;                                                         \
    reset();                                                                   \
  } while (0)

#define TestBlendingXmmAddr(Dst, Value0, Value1, M /*ask*/, Inst, Type)        \
  do {                                                                         \
    static constexpr char TestString[] =                                       \
        "(" #Dst ", " #Value0 ", Addr, " #Value1 ", " #M ", " #Inst ", " #Type \
        ")";                                                                   \
    const uint32_t T0 = allocateDqword();                                      \
    const Dqword V0 Value0;                                                    \
    const uint32_t T1 = allocateDqword();                                      \
    const Dqword V1 Value1;                                                    \
    const uint32_t Mask = allocateDqword();                                    \
    const Dqword MaskValue M;                                                  \
                                                                               \
    __ movups(Encoded_Xmm_xmm0(), dwordAddress(Mask));                         \
    __ movups(Encoded_Xmm_##Dst(), dwordAddress(T0));                          \
    __ Inst(IceType_##Type, Encoded_Xmm_##Dst(), dwordAddress(T1));            \
                                                                               \
    AssembledTest test = assemble();                                           \
    test.setDqwordTo(T0, V0);                                                  \
    test.setDqwordTo(T1, V1);                                                  \
    test.setDqwordTo(Mask, MaskValue);                                         \
    test.run();                                                                \
                                                                               \
    ASSERT_EQ(packedAs<Type>(V0).blendWith(V1, MaskValue), test.Dst<Dqword>()) \
        << TestString;                                                         \
    reset();                                                                   \
  } while (0)

#define TestBlending(Src, Dst)                                                 \
  do {                                                                         \
    TestBlendingXmmXmm(                                                        \
        Dst, (1.0, 2.0, 1.0, 2.0), Src, (-1.0, -2.0, -1.0, -2.0),              \
        (uint64_t(0x8000000000000000ull), uint64_t(0x0000000080000000ull)),    \
        blendvps, f32);                                                        \
    TestBlendingXmmAddr(                                                       \
        Dst, (1.0, 2.0, 1.0, 2.0), (-1.0, -2.0, -1.0, -2.0),                   \
        (uint64_t(0x8000000000000000ull), uint64_t(0x0000000080000000ull)),    \
        blendvps, f32);                                                        \
    TestBlendingXmmXmm(                                                        \
        Dst,                                                                   \
        (uint64_t(0xFFFFFFFFFFFFFFFFull), uint64_t(0xBBBBBBBBBBBBBBBBull)),    \
        Src,                                                                   \
        (uint64_t(0xAAAAAAAAAAAAAAAAull), uint64_t(0xEEEEEEEEEEEEEEEEull)),    \
        (uint64_t(0x8000000000000080ull), uint64_t(0x8080808000000000ull)),    \
        pblendvb, i8);                                                         \
    TestBlendingXmmAddr(                                                       \
        Dst,                                                                   \
        (uint64_t(0xFFFFFFFFFFFFFFFFull), uint64_t(0xBBBBBBBBBBBBBBBBull)),    \
        (uint64_t(0xAAAAAAAAAAAAAAAAull), uint64_t(0xEEEEEEEEEEEEEEEEull)),    \
        (uint64_t(0x8000000000000080ull), uint64_t(0x8080808000000000ull)),    \
        pblendvb, i8);                                                         \
  } while (0)

  /* xmm0 is taken. It is the implicit mask . */
  TestBlending(xmm1, xmm2);
  TestBlending(xmm2, xmm3);
  TestBlending(xmm3, xmm4);
  TestBlending(xmm4, xmm5);
  TestBlending(xmm5, xmm6);
  TestBlending(xmm6, xmm7);
  TestBlending(xmm7, xmm8);
  TestBlending(xmm8, xmm9);
  TestBlending(xmm9, xmm10);
  TestBlending(xmm10, xmm11);
  TestBlending(xmm11, xmm12);
  TestBlending(xmm12, xmm13);
  TestBlending(xmm13, xmm14);
  TestBlending(xmm14, xmm15);
  TestBlending(xmm15, xmm1);

#undef TestBlending
#undef TestBlendingXmmAddr
#undef TestBlendingXmmXmm
}

TEST_F(AssemblerX8664Test, Cmpps) {
#define TestCmppsXmmXmm(Dst, Src, C, Op)                                       \
  do {                                                                         \
    static constexpr char TestString[] =                                       \
        "(" #Src ", " #Dst ", " #C ", " #Op ")";                               \
    const uint32_t T0 = allocateDqword();                                      \
    const Dqword V0(-1.0, 1.0, 3.14, 1024.5);                                  \
    const uint32_t T1 = allocateDqword();                                      \
    const Dqword V1(-1.0, 1.0, 3.14, 1024.5);                                  \
                                                                               \
    __ movups(Encoded_Xmm_##Dst(), dwordAddress(T0));                          \
    __ movups(Encoded_Xmm_##Src(), dwordAddress(T1));                          \
    __ cmpps(Encoded_Xmm_##Dst(), Encoded_Xmm_##Src(), Cond::Cmpps_##C);       \
                                                                               \
    AssembledTest test = assemble();                                           \
    test.setDqwordTo(T0, V0);                                                  \
    test.setDqwordTo(T1, V1);                                                  \
    test.run();                                                                \
                                                                               \
    ASSERT_EQ(packedAs<float>(V0) Op V1, test.Dst<Dqword>()) << TestString;    \
    ;                                                                          \
    reset();                                                                   \
  } while (0)

#define TestCmppsXmmAddr(Dst, C, Op)                                           \
  do {                                                                         \
    static constexpr char TestString[] = "(" #Dst ", Addr, " #C ", " #Op ")";  \
    const uint32_t T0 = allocateDqword();                                      \
    const Dqword V0(-1.0, 1.0, 3.14, 1024.5);                                  \
    const uint32_t T1 = allocateDqword();                                      \
    const Dqword V1(-1.0, 1.0, 3.14, 1024.5);                                  \
                                                                               \
    __ movups(Encoded_Xmm_##Dst(), dwordAddress(T0));                          \
    __ cmpps(Encoded_Xmm_##Dst(), dwordAddress(T1), Cond::Cmpps_##C);          \
                                                                               \
    AssembledTest test = assemble();                                           \
    test.setDqwordTo(T0, V0);                                                  \
    test.setDqwordTo(T1, V1);                                                  \
    test.run();                                                                \
                                                                               \
    ASSERT_EQ(packedAs<float>(V0) Op V1, test.Dst<Dqword>()) << TestString;    \
    ;                                                                          \
    reset();                                                                   \
  } while (0)

#define TestCmppsOrdUnordXmmXmm(Dst, Src, C)                                   \
  do {                                                                         \
    static constexpr char TestString[] = "(" #Src ", " #Dst ", " #C ")";       \
    const uint32_t T0 = allocateDqword();                                      \
    const Dqword V0(1.0, 1.0, std::numeric_limits<float>::quiet_NaN(),         \
                    std::numeric_limits<float>::quiet_NaN());                  \
    const uint32_t T1 = allocateDqword();                                      \
    const Dqword V1(1.0, std::numeric_limits<float>::quiet_NaN(), 1.0,         \
                    std::numeric_limits<float>::quiet_NaN());                  \
                                                                               \
    __ movups(Encoded_Xmm_##Dst(), dwordAddress(T0));                          \
    __ movups(Encoded_Xmm_##Src(), dwordAddress(T1));                          \
    __ cmpps(Encoded_Xmm_##Dst(), Encoded_Xmm_##Src(), Cond::Cmpps_##C);       \
                                                                               \
    AssembledTest test = assemble();                                           \
    test.setDqwordTo(T0, V0);                                                  \
    test.setDqwordTo(T1, V1);                                                  \
    test.run();                                                                \
                                                                               \
    ASSERT_EQ(packedAs<float>(V0).C(V1), test.Dst<Dqword>()) << TestString;    \
    ;                                                                          \
    reset();                                                                   \
  } while (0)

#define TestCmppsOrdUnordXmmAddr(Dst, C)                                       \
  do {                                                                         \
    static constexpr char TestString[] = "(" #Dst ", " #C ")";                 \
    const uint32_t T0 = allocateDqword();                                      \
    const Dqword V0(1.0, 1.0, std::numeric_limits<float>::quiet_NaN(),         \
                    std::numeric_limits<float>::quiet_NaN());                  \
    const uint32_t T1 = allocateDqword();                                      \
    const Dqword V1(1.0, std::numeric_limits<float>::quiet_NaN(), 1.0,         \
                    std::numeric_limits<float>::quiet_NaN());                  \
                                                                               \
    __ movups(Encoded_Xmm_##Dst(), dwordAddress(T0));                          \
    __ cmpps(Encoded_Xmm_##Dst(), dwordAddress(T1), Cond::Cmpps_##C);          \
                                                                               \
    AssembledTest test = assemble();                                           \
    test.setDqwordTo(T0, V0);                                                  \
    test.setDqwordTo(T1, V1);                                                  \
    test.run();                                                                \
                                                                               \
    ASSERT_EQ(packedAs<float>(V0).C(V1), test.Dst<Dqword>()) << TestString;    \
    ;                                                                          \
    reset();                                                                   \
  } while (0)

#define TestCmpps(Dst, Src)                                                    \
  do {                                                                         \
    TestCmppsXmmXmm(Dst, Src, eq, == );                                        \
    TestCmppsXmmAddr(Dst, eq, == );                                            \
    TestCmppsXmmXmm(Dst, Src, eq, == );                                        \
    TestCmppsXmmAddr(Dst, eq, == );                                            \
    TestCmppsXmmXmm(Dst, Src, eq, == );                                        \
    TestCmppsXmmAddr(Dst, eq, == );                                            \
    TestCmppsOrdUnordXmmXmm(Dst, Src, unord);                                  \
    TestCmppsOrdUnordXmmAddr(Dst, unord);                                      \
    TestCmppsXmmXmm(Dst, Src, eq, == );                                        \
    TestCmppsXmmAddr(Dst, eq, == );                                            \
    TestCmppsXmmXmm(Dst, Src, eq, == );                                        \
    TestCmppsXmmAddr(Dst, eq, == );                                            \
    TestCmppsXmmXmm(Dst, Src, eq, == );                                        \
    TestCmppsXmmAddr(Dst, eq, == );                                            \
    TestCmppsOrdUnordXmmXmm(Dst, Src, unord);                                  \
    TestCmppsOrdUnordXmmAddr(Dst, unord);                                      \
  } while (0)

  TestCmpps(xmm0, xmm1);
  TestCmpps(xmm1, xmm2);
  TestCmpps(xmm2, xmm3);
  TestCmpps(xmm3, xmm4);
  TestCmpps(xmm4, xmm5);
  TestCmpps(xmm5, xmm6);
  TestCmpps(xmm6, xmm7);
  TestCmpps(xmm7, xmm8);
  TestCmpps(xmm8, xmm9);
  TestCmpps(xmm9, xmm10);
  TestCmpps(xmm10, xmm11);
  TestCmpps(xmm11, xmm12);
  TestCmpps(xmm12, xmm13);
  TestCmpps(xmm13, xmm14);
  TestCmpps(xmm14, xmm15);
  TestCmpps(xmm15, xmm0);

#undef TestCmpps
#undef TestCmppsOrdUnordXmmAddr
#undef TestCmppsOrdUnordXmmXmm
#undef TestCmppsXmmAddr
#undef TestCmppsXmmXmm
}

TEST_F(AssemblerX8664Test, Sqrtps_Rsqrtps_Reciprocalps_Sqrtpd) {
#define TestImplSingle(Dst, Inst, Expect)                                      \
  do {                                                                         \
    static constexpr char TestString[] = "(" #Dst ", " #Inst ")";              \
    const uint32_t T0 = allocateDqword();                                      \
    const Dqword V0(1.0, 4.0, 20.0, 3.14);                                     \
                                                                               \
    __ movups(Encoded_Xmm_##Dst(), dwordAddress(T0));                          \
    __ Inst(Encoded_Xmm_##Dst());                                              \
                                                                               \
    AssembledTest test = assemble();                                           \
    test.setDqwordTo(T0, V0);                                                  \
    test.run();                                                                \
    ASSERT_EQ(Dqword Expect, test.Dst<Dqword>()) << TestString;                \
    reset();                                                                   \
  } while (0)

#define TestImpl(Dst)                                                          \
  do {                                                                         \
    TestImplSingle(Dst, sqrtps, (uint64_t(0x400000003F800000ull),              \
                                 uint64_t(0x3FE2D10B408F1BBDull)));            \
    TestImplSingle(Dst, rsqrtps, (uint64_t(0x3EFFF0003F7FF000ull),             \
                                  uint64_t(0x3F1078003E64F000ull)));           \
    TestImplSingle(Dst, reciprocalps, (uint64_t(0x3E7FF0003F7FF000ull),        \
                                       uint64_t(0x3EA310003D4CC000ull)));      \
                                                                               \
    TestImplSingle(Dst, sqrtpd, (uint64_t(0x4036A09E9365F5F3ull),              \
                                 uint64_t(0x401C42FAE40282A8ull)));            \
  } while (0)

  TestImpl(xmm0);
  TestImpl(xmm1);
  TestImpl(xmm2);
  TestImpl(xmm3);
  TestImpl(xmm4);
  TestImpl(xmm5);
  TestImpl(xmm6);
  TestImpl(xmm7);
  TestImpl(xmm8);
  TestImpl(xmm9);
  TestImpl(xmm10);
  TestImpl(xmm11);
  TestImpl(xmm12);
  TestImpl(xmm13);
  TestImpl(xmm14);
  TestImpl(xmm15);

#undef TestImpl
#undef TestImplSingle
}

TEST_F(AssemblerX8664Test, Unpck) {
  const Dqword V0(uint64_t(0xAAAAAAAABBBBBBBBull),
                  uint64_t(0xCCCCCCCCDDDDDDDDull));
  const Dqword V1(uint64_t(0xEEEEEEEEFFFFFFFFull),
                  uint64_t(0x9999999988888888ull));

  const Dqword unpcklpsExpected(uint64_t(0xFFFFFFFFBBBBBBBBull),
                                uint64_t(0xEEEEEEEEAAAAAAAAull));
  const Dqword unpcklpdExpected(uint64_t(0xAAAAAAAABBBBBBBBull),
                                uint64_t(0xEEEEEEEEFFFFFFFFull));
  const Dqword unpckhpsExpected(uint64_t(0x88888888DDDDDDDDull),
                                uint64_t(0x99999999CCCCCCCCull));
  const Dqword unpckhpdExpected(uint64_t(0xCCCCCCCCDDDDDDDDull),
                                uint64_t(0x9999999988888888ull));

#define TestImplSingle(Dst, Src, Inst)                                         \
  do {                                                                         \
    static constexpr char TestString[] = "(" #Dst ", " #Src ", " #Inst ")";    \
    const uint32_t T0 = allocateDqword();                                      \
    const uint32_t T1 = allocateDqword();                                      \
                                                                               \
    __ movups(Encoded_Xmm_##Dst(), dwordAddress(T0));                          \
    __ movups(Encoded_Xmm_##Src(), dwordAddress(T1));                          \
    __ Inst(Encoded_Xmm_##Dst(), Encoded_Xmm_##Src());                         \
                                                                               \
    AssembledTest test = assemble();                                           \
    test.setDqwordTo(T0, V0);                                                  \
    test.setDqwordTo(T1, V1);                                                  \
    test.run();                                                                \
                                                                               \
    ASSERT_EQ(Inst##Expected, test.Dst<Dqword>()) << TestString;               \
    reset();                                                                   \
  } while (0)

#define TestImpl(Dst, Src)                                                     \
  do {                                                                         \
    TestImplSingle(Dst, Src, unpcklps);                                        \
    TestImplSingle(Dst, Src, unpcklpd);                                        \
    TestImplSingle(Dst, Src, unpckhps);                                        \
    TestImplSingle(Dst, Src, unpckhpd);                                        \
  } while (0)

  TestImpl(xmm0, xmm1);
  TestImpl(xmm1, xmm2);
  TestImpl(xmm2, xmm3);
  TestImpl(xmm3, xmm4);
  TestImpl(xmm4, xmm5);
  TestImpl(xmm5, xmm6);
  TestImpl(xmm6, xmm7);
  TestImpl(xmm7, xmm8);
  TestImpl(xmm8, xmm9);
  TestImpl(xmm9, xmm10);
  TestImpl(xmm10, xmm11);
  TestImpl(xmm11, xmm12);
  TestImpl(xmm12, xmm13);
  TestImpl(xmm13, xmm14);
  TestImpl(xmm14, xmm15);
  TestImpl(xmm15, xmm0);

#undef TestImpl
#undef TestImplSingle
}

TEST_F(AssemblerX8664Test, Shufp) {
  const Dqword V0(uint64_t(0x1111111122222222ull),
                  uint64_t(0x5555555577777777ull));
  const Dqword V1(uint64_t(0xAAAAAAAABBBBBBBBull),
                  uint64_t(0xCCCCCCCCDDDDDDDDull));

  const uint8_t pshufdImm = 0x63;
  const Dqword pshufdExpected(uint64_t(0xBBBBBBBBCCCCCCCCull),
                              uint64_t(0xAAAAAAAADDDDDDDDull));

  const uint8_t shufpsImm = 0xf9;
  const Dqword shufpsExpected(uint64_t(0x7777777711111111ull),
                              uint64_t(0xCCCCCCCCCCCCCCCCull));

#define TestImplSingleXmmXmm(Dst, Src, Inst)                                   \
  do {                                                                         \
    static constexpr char TestString[] = "(" #Dst ", " #Src ", " #Inst ")";    \
    const uint32_t T0 = allocateDqword();                                      \
    const uint32_t T1 = allocateDqword();                                      \
                                                                               \
    __ movups(Encoded_Xmm_##Dst(), dwordAddress(T0));                          \
    __ movups(Encoded_Xmm_##Src(), dwordAddress(T1));                          \
    __ Inst(IceType_f32, Encoded_Xmm_##Dst(), Encoded_Xmm_##Src(),             \
            Immediate(Inst##Imm));                                             \
                                                                               \
    AssembledTest test = assemble();                                           \
    test.setDqwordTo(T0, V0);                                                  \
    test.setDqwordTo(T1, V1);                                                  \
    test.run();                                                                \
                                                                               \
    ASSERT_EQ(Inst##Expected, test.Dst<Dqword>()) << TestString;               \
    reset();                                                                   \
  } while (0)

#define TestImplSingleXmmAddr(Dst, Inst)                                       \
  do {                                                                         \
    static constexpr char TestString[] = "(" #Dst ", Addr, " #Inst ")";        \
    const uint32_t T0 = allocateDqword();                                      \
    const uint32_t T1 = allocateDqword();                                      \
                                                                               \
    __ movups(Encoded_Xmm_##Dst(), dwordAddress(T0));                          \
    __ Inst(IceType_f32, Encoded_Xmm_##Dst(), dwordAddress(T1),                \
            Immediate(Inst##Imm));                                             \
                                                                               \
    AssembledTest test = assemble();                                           \
    test.setDqwordTo(T0, V0);                                                  \
    test.setDqwordTo(T1, V1);                                                  \
    test.run();                                                                \
                                                                               \
    ASSERT_EQ(Inst##Expected, test.Dst<Dqword>()) << TestString;               \
    reset();                                                                   \
  } while (0)

#define TestImplSingleXmmXmmUntyped(Dst, Src, Inst)                            \
  do {                                                                         \
    static constexpr char TestString[] =                                       \
        "(" #Dst ", " #Src ", " #Inst ", Untyped)";                            \
    const uint32_t T0 = allocateDqword();                                      \
    const uint32_t T1 = allocateDqword();                                      \
                                                                               \
    __ movups(Encoded_Xmm_##Dst(), dwordAddress(T0));                          \
    __ movups(Encoded_Xmm_##Src(), dwordAddress(T1));                          \
    __ Inst(Encoded_Xmm_##Dst(), Encoded_Xmm_##Src(), Immediate(Inst##Imm));   \
                                                                               \
    AssembledTest test = assemble();                                           \
    test.setDqwordTo(T0, V0);                                                  \
    test.setDqwordTo(T1, V1);                                                  \
    test.run();                                                                \
                                                                               \
    ASSERT_EQ(Inst##UntypedExpected, test.Dst<Dqword>()) << TestString;        \
    reset();                                                                   \
  } while (0)

#define TestImpl(Dst, Src)                                                     \
  do {                                                                         \
    TestImplSingleXmmXmm(Dst, Src, pshufd);                                    \
    TestImplSingleXmmAddr(Dst, pshufd);                                        \
    TestImplSingleXmmXmm(Dst, Src, shufps);                                    \
    TestImplSingleXmmAddr(Dst, shufps);                                        \
  } while (0)

  TestImpl(xmm0, xmm1);
  TestImpl(xmm1, xmm2);
  TestImpl(xmm2, xmm3);
  TestImpl(xmm3, xmm4);
  TestImpl(xmm4, xmm5);
  TestImpl(xmm5, xmm6);
  TestImpl(xmm6, xmm7);
  TestImpl(xmm7, xmm8);
  TestImpl(xmm8, xmm9);
  TestImpl(xmm9, xmm10);
  TestImpl(xmm10, xmm11);
  TestImpl(xmm11, xmm12);
  TestImpl(xmm12, xmm13);
  TestImpl(xmm13, xmm14);
  TestImpl(xmm14, xmm15);
  TestImpl(xmm15, xmm0);

#undef TestImpl
#undef TestImplSingleXmmXmmUntyped
#undef TestImplSingleXmmAddr
#undef TestImplSingleXmmXmm
}

TEST_F(AssemblerX8664Test, Cvt) {
  const Dqword dq2ps32DstValue(-1.0f, -1.0f, -1.0f, -1.0f);
  const Dqword dq2ps32SrcValue(-5, 3, 100, 200);
  const Dqword dq2ps32Expected(-5.0f, 3.0f, 100.0, 200.0);

  const Dqword dq2ps64DstValue(0.0f, 0.0f, -1.0f, -1.0f);
  const Dqword dq2ps64SrcValue(-5, 3, 100, 200);
  const Dqword dq2ps64Expected(-5.0f, 3.0f, 100.0, 200.0);

  const Dqword tps2dq32DstValue(-1.0f, -1.0f, -1.0f, -1.0f);
  const Dqword tps2dq32SrcValue(-5.0f, 3.0f, 100.0, 200.0);
  const Dqword tps2dq32Expected(-5, 3, 100, 200);

  const Dqword tps2dq64DstValue(-1.0f, -1.0f, -1.0f, -1.0f);
  const Dqword tps2dq64SrcValue(-5.0f, 3.0f, 100.0, 200.0);
  const Dqword tps2dq64Expected(-5, 3, 100, 200);

  const Dqword si2ss32DstValue(-1.0f, -1.0f, -1.0f, -1.0f);
  const int32_t si2ss32SrcValue = 5;
  const Dqword si2ss32Expected(5.0f, -1.0f, -1.0f, -1.0f);

  const Dqword si2ss64DstValue(-1.0, -1.0);
  const int32_t si2ss64SrcValue = 5;
  const Dqword si2ss64Expected(5.0, -1.0);

  const int32_t tss2si32DstValue = 0xF00F0FF0;
  const Dqword tss2si32SrcValue(-5.0f, -1.0f, -1.0f, -1.0f);
  const int32_t tss2si32Expected = -5;

  const int32_t tss2si64DstValue = 0xF00F0FF0;
  const Dqword tss2si64SrcValue(-5.0, -1.0);
  const int32_t tss2si64Expected = -5;

  const Dqword float2float32DstValue(-1.0, -1.0);
  const Dqword float2float32SrcValue(-5.0, 3, 100, 200);
  const Dqword float2float32Expected(-5.0, -1.0);

  const Dqword float2float64DstValue(-1.0, -1.0, -1.0, -1.0);
  const Dqword float2float64SrcValue(-5.0, 3.0);
  const Dqword float2float64Expected(-5.0, -1.0, -1.0, -1.0);

#define TestImplPXmmXmm(Dst, Src, Inst, Size)                                  \
  do {                                                                         \
    static constexpr char TestString[] =                                       \
        "(" #Dst ", " #Src ", cvt" #Inst ", f" #Size ")";                      \
    const uint32_t T0 = allocateDqword();                                      \
    const uint32_t T1 = allocateDqword();                                      \
                                                                               \
    __ movups(Encoded_Xmm_##Dst(), dwordAddress(T0));                          \
    __ movups(Encoded_Xmm_##Src(), dwordAddress(T1));                          \
    __ cvt##Inst(IceType_f##Size, Encoded_Xmm_##Dst(), Encoded_Xmm_##Src());   \
                                                                               \
    AssembledTest test = assemble();                                           \
    test.setDqwordTo(T0, Inst##Size##DstValue);                                \
    test.setDqwordTo(T1, Inst##Size##SrcValue);                                \
    test.run();                                                                \
                                                                               \
    ASSERT_EQ(Inst##Size##Expected, test.Dst<Dqword>()) << TestString;         \
    reset();                                                                   \
  } while (0)

#define TestImplSXmmReg(Dst, GPR, Inst, Size)                                  \
  do {                                                                         \
    static constexpr char TestString[] =                                       \
        "(" #Dst ", " #GPR ", cvt" #Inst ", f" #Size ")";                      \
    const uint32_t T0 = allocateDqword();                                      \
                                                                               \
    __ movups(Encoded_Xmm_##Dst(), dwordAddress(T0));                          \
    __ mov(IceType_i32, Encoded_GPR_##GPR(), Immediate(Inst##Size##SrcValue)); \
    __ cvt##Inst(IceType_f##Size, Encoded_Xmm_##Dst(), Encoded_GPR_##GPR());   \
                                                                               \
    AssembledTest test = assemble();                                           \
    test.setDqwordTo(T0, Inst##Size##DstValue);                                \
    test.run();                                                                \
                                                                               \
    ASSERT_EQ(Inst##Size##Expected, test.Dst<Dqword>()) << TestString;         \
    reset();                                                                   \
  } while (0)

#define TestImplSRegXmm(GPR, Src, Inst, Size)                                  \
  do {                                                                         \
    static constexpr char TestString[] =                                       \
        "(" #GPR ", " #Src ", cvt" #Inst ", f" #Size ")";                      \
    const uint32_t T0 = allocateDqword();                                      \
                                                                               \
    __ mov(IceType_i32, Encoded_GPR_##GPR(), Immediate(Inst##Size##DstValue)); \
    __ movups(Encoded_Xmm_##Src(), dwordAddress(T0));                          \
    __ cvt##Inst(IceType_f##Size, Encoded_GPR_##GPR(), Encoded_Xmm_##Src());   \
                                                                               \
    AssembledTest test = assemble();                                           \
    test.setDqwordTo(T0, Inst##Size##SrcValue);                                \
    test.run();                                                                \
                                                                               \
    ASSERT_EQ(static_cast<uint32_t>(Inst##Size##Expected), test.GPR())         \
        << TestString;                                                         \
    reset();                                                                   \
  } while (0)

#define TestImplPXmmAddr(Dst, Inst, Size)                                      \
  do {                                                                         \
    static constexpr char TestString[] =                                       \
        "(" #Dst ", Addr, cvt" #Inst ", f" #Size ")";                          \
    const uint32_t T0 = allocateDqword();                                      \
    const uint32_t T1 = allocateDqword();                                      \
                                                                               \
    __ movups(Encoded_Xmm_##Dst(), dwordAddress(T0));                          \
    __ cvt##Inst(IceType_f##Size, Encoded_Xmm_##Dst(), dwordAddress(T1));      \
                                                                               \
    AssembledTest test = assemble();                                           \
    test.setDqwordTo(T0, Inst##Size##DstValue);                                \
    test.setDqwordTo(T1, Inst##Size##SrcValue);                                \
    test.run();                                                                \
                                                                               \
    ASSERT_EQ(Inst##Size##Expected, test.Dst<Dqword>()) << TestString;         \
    reset();                                                                   \
  } while (0)

#define TestImplSXmmAddr(Dst, Inst, Size)                                      \
  do {                                                                         \
    static constexpr char TestString[] =                                       \
        "(" #Dst ", Addr, cvt" #Inst ", f" #Size ")";                          \
    const uint32_t T0 = allocateDqword();                                      \
    const uint32_t T1 = allocateDword();                                       \
                                                                               \
    __ movups(Encoded_Xmm_##Dst(), dwordAddress(T0));                          \
    __ cvt##Inst(IceType_f##Size, Encoded_Xmm_##Dst(), dwordAddress(T1));      \
                                                                               \
    AssembledTest test = assemble();                                           \
    test.setDqwordTo(T0, Inst##Size##DstValue);                                \
    test.setDwordTo(T1, Inst##Size##SrcValue);                                 \
    test.run();                                                                \
                                                                               \
    ASSERT_EQ(Inst##Size##Expected, test.Dst<Dqword>()) << TestString;         \
    reset();                                                                   \
  } while (0)

#define TestImplSRegAddr(GPR, Inst, Size)                                      \
  do {                                                                         \
    static constexpr char TestString[] =                                       \
        "(" #GPR ", Addr, cvt" #Inst ", f" #Size ")";                          \
    const uint32_t T0 = allocateDqword();                                      \
                                                                               \
    __ mov(IceType_i32, Encoded_GPR_##GPR(), Immediate(Inst##Size##DstValue)); \
    __ cvt##Inst(IceType_f##Size, Encoded_GPR_##GPR(), dwordAddress(T0));      \
                                                                               \
    AssembledTest test = assemble();                                           \
    test.setDqwordTo(T0, Inst##Size##SrcValue);                                \
    test.run();                                                                \
                                                                               \
    ASSERT_EQ(static_cast<uint32_t>(Inst##Size##Expected), test.GPR())         \
        << TestString;                                                         \
    reset();                                                                   \
  } while (0)

#define TestImplSize(Dst, Src, GPR, Size)                                      \
  do {                                                                         \
    TestImplPXmmXmm(Dst, Src, dq2ps, Size);                                    \
    TestImplPXmmAddr(Src, dq2ps, Size);                                        \
    TestImplPXmmXmm(Dst, Src, tps2dq, Size);                                   \
    TestImplPXmmAddr(Src, tps2dq, Size);                                       \
    TestImplSXmmReg(Dst, GPR, si2ss, Size);                                    \
    TestImplSXmmAddr(Dst, si2ss, Size);                                        \
    TestImplSRegXmm(GPR, Src, tss2si, Size);                                   \
    TestImplSRegAddr(GPR, tss2si, Size);                                       \
    TestImplPXmmXmm(Dst, Src, float2float, Size);                              \
    TestImplPXmmAddr(Src, float2float, Size);                                  \
  } while (0)

#define TestImpl(Dst, Src, GPR)                                                \
  do {                                                                         \
    TestImplSize(Dst, Src, GPR, 32);                                           \
    TestImplSize(Dst, Src, GPR, 64);                                           \
  } while (0)

  TestImpl(xmm0, xmm1, r1);
  TestImpl(xmm1, xmm2, r2);
  TestImpl(xmm2, xmm3, r3);
  TestImpl(xmm3, xmm4, r4);
  TestImpl(xmm4, xmm5, r5);
  TestImpl(xmm5, xmm6, r6);
  TestImpl(xmm6, xmm7, r7);
  TestImpl(xmm7, xmm8, r8);
  TestImpl(xmm8, xmm9, r10);
  TestImpl(xmm9, xmm10, r11);
  TestImpl(xmm10, xmm11, r12);
  TestImpl(xmm11, xmm12, r13);
  TestImpl(xmm12, xmm13, r14);
  TestImpl(xmm13, xmm14, r15);
  TestImpl(xmm14, xmm15, r1);
  TestImpl(xmm15, xmm0, r2);

#undef TestImpl
#undef TestImplSize
#undef TestImplSRegAddr
#undef TestImplSXmmAddr
#undef TestImplPXmmAddr
#undef TestImplSRegXmm
#undef TestImplSXmmReg
#undef TestImplPXmmXmm
}

TEST_F(AssemblerX8664Test, Ucomiss) {
  static constexpr float qnan32 = std::numeric_limits<float>::quiet_NaN();
  static constexpr double qnan64 = std::numeric_limits<float>::quiet_NaN();

  Dqword test32DstValue(0.0, qnan32, qnan32, qnan32);
  Dqword test32SrcValue(0.0, qnan32, qnan32, qnan32);

  Dqword test64DstValue(0.0, qnan64);
  Dqword test64SrcValue(0.0, qnan64);

#define TestImplXmmXmm(Dst, Value0, Src, Value1, Size, CompType, BParity,      \
                       BOther)                                                 \
  do {                                                                         \
    static constexpr char NearBranch = AssemblerX8664::kNearJump;              \
    static constexpr char TestString[] =                                       \
        "(" #Dst ", " #Value0 ", " #Src ", " #Value1 ", " #Size ", " #CompType \
        ", " #BParity ", " #BOther ")";                                        \
    const uint32_t T0 = allocateDqword();                                      \
    test##Size##DstValue.F##Size[0] = Value0;                                  \
    const uint32_t T1 = allocateDqword();                                      \
    test##Size##SrcValue.F##Size[0] = Value1;                                  \
    const uint32_t ImmIfTrue = 0xBEEF;                                         \
    const uint32_t ImmIfFalse = 0xC0FFE;                                       \
                                                                               \
    __ movups(Encoded_Xmm_##Dst(), dwordAddress(T0));                          \
    __ movups(Encoded_Xmm_##Src(), dwordAddress(T1));                          \
    __ mov(IceType_i32, GPRRegister::Encoded_Reg_eax, Immediate(ImmIfFalse));  \
    __ ucomiss(IceType_f##Size, Encoded_Xmm_##Dst(), Encoded_Xmm_##Src());     \
    Label Done;                                                                \
    __ j(Cond::Br_##BParity, &Done, NearBranch);                               \
    __ j(Cond::Br_##BOther, &Done, NearBranch);                                \
    __ mov(IceType_i32, GPRRegister::Encoded_Reg_eax, Immediate(ImmIfTrue));   \
    __ bind(&Done);                                                            \
                                                                               \
    AssembledTest test = assemble();                                           \
    test.setDqwordTo(T0, test##Size##DstValue);                                \
    test.setDqwordTo(T1, test##Size##SrcValue);                                \
    test.run();                                                                \
                                                                               \
    ASSERT_EQ(ImmIfTrue, test.eax()) << TestString;                            \
    reset();                                                                   \
  } while (0)

#define TestImplXmmAddr(Dst, Value0, Value1, Size, CompType, BParity, BOther)  \
  do {                                                                         \
    static constexpr char NearBranch = AssemblerX8664::kNearJump;              \
    static constexpr char TestString[] =                                       \
        "(" #Dst ", " #Value0 ", Addr, " #Value1 ", " #Size ", " #CompType     \
        ", " #BParity ", " #BOther ")";                                        \
    const uint32_t T0 = allocateDqword();                                      \
    test##Size##DstValue.F##Size[0] = Value0;                                  \
    const uint32_t T1 = allocateDqword();                                      \
    test##Size##SrcValue.F##Size[0] = Value1;                                  \
    const uint32_t ImmIfTrue = 0xBEEF;                                         \
    const uint32_t ImmIfFalse = 0xC0FFE;                                       \
                                                                               \
    __ movups(Encoded_Xmm_##Dst(), dwordAddress(T0));                          \
    __ mov(IceType_i32, GPRRegister::Encoded_Reg_eax, Immediate(ImmIfFalse));  \
    __ ucomiss(IceType_f##Size, Encoded_Xmm_##Dst(), dwordAddress(T1));        \
    Label Done;                                                                \
    __ j(Cond::Br_##BParity, &Done, NearBranch);                               \
    __ j(Cond::Br_##BOther, &Done, NearBranch);                                \
    __ mov(IceType_i32, GPRRegister::Encoded_Reg_eax, Immediate(ImmIfTrue));   \
    __ bind(&Done);                                                            \
                                                                               \
    AssembledTest test = assemble();                                           \
    test.setDqwordTo(T0, test##Size##DstValue);                                \
    test.setDqwordTo(T1, test##Size##SrcValue);                                \
    test.run();                                                                \
                                                                               \
    ASSERT_EQ(ImmIfTrue, test.eax()) << TestString;                            \
    reset();                                                                   \
  } while (0)

#define TestImplCond(Dst, Value0, Src, Value1, Size, CompType, BParity,        \
                     BOther)                                                   \
  do {                                                                         \
    TestImplXmmXmm(Dst, Value0, Src, Value1, Size, CompType, BParity, BOther); \
    TestImplXmmAddr(Dst, Value0, Value1, Size, CompType, BParity, BOther);     \
  } while (0)

#define TestImplSize(Dst, Src, Size)                                           \
  do {                                                                         \
    TestImplCond(Dst, 1.0, Src, 1.0, Size, isEq, p, ne);                       \
    TestImplCond(Dst, 1.0, Src, 2.0, Size, isNe, p, e);                        \
    TestImplCond(Dst, 1.0, Src, 2.0, Size, isLe, p, a);                        \
    TestImplCond(Dst, 1.0, Src, 1.0, Size, isLe, p, a);                        \
    TestImplCond(Dst, 1.0, Src, 2.0, Size, isLt, p, ae);                       \
    TestImplCond(Dst, 2.0, Src, 1.0, Size, isGe, p, b);                        \
    TestImplCond(Dst, 1.0, Src, 1.0, Size, isGe, p, b);                        \
    TestImplCond(Dst, 2.0, Src, 1.0, Size, isGt, p, be);                       \
    TestImplCond(Dst, qnan##Size, Src, 1.0, Size, isUnord, np, o);             \
    TestImplCond(Dst, 1.0, Src, qnan##Size, Size, isUnord, np, s);             \
    TestImplCond(Dst, qnan##Size, Src, qnan##Size, Size, isUnord, np, s);      \
  } while (0)

#define TestImpl(Dst, Src)                                                     \
  do {                                                                         \
    TestImplSize(Dst, Src, 32);                                                \
    TestImplSize(Dst, Src, 64);                                                \
  } while (0)

  TestImpl(xmm0, xmm1);
  TestImpl(xmm1, xmm2);
  TestImpl(xmm2, xmm3);
  TestImpl(xmm3, xmm4);
  TestImpl(xmm4, xmm5);
  TestImpl(xmm5, xmm6);
  TestImpl(xmm6, xmm7);
  TestImpl(xmm7, xmm8);
  TestImpl(xmm8, xmm9);
  TestImpl(xmm9, xmm10);
  TestImpl(xmm10, xmm11);
  TestImpl(xmm11, xmm12);
  TestImpl(xmm12, xmm13);
  TestImpl(xmm13, xmm14);
  TestImpl(xmm14, xmm15);
  TestImpl(xmm15, xmm0);

#undef TestImpl
#undef TestImplSize
#undef TestImplCond
#undef TestImplXmmAddr
#undef TestImplXmmXmm
}

TEST_F(AssemblerX8664Test, Sqrtss) {
  Dqword test32SrcValue(-100.0, -100.0, -100.0, -100.0);
  Dqword test32DstValue(-1.0, -1.0, -1.0, -1.0);

  Dqword test64SrcValue(-100.0, -100.0);
  Dqword test64DstValue(-1.0, -1.0);

#define TestSqrtssXmmXmm(Dst, Src, Value1, Result, Size)                       \
  do {                                                                         \
    static constexpr char TestString[] =                                       \
        "(" #Dst ", " #Src ", " #Value1 ", " #Result ", " #Size ")";           \
    const uint32_t T0 = allocateDqword();                                      \
    test##Size##SrcValue.F##Size[0] = Value1;                                  \
    const uint32_t T1 = allocateDqword();                                      \
                                                                               \
    __ movups(Encoded_Xmm_##Src(), dwordAddress(T0));                          \
    __ movups(Encoded_Xmm_##Dst(), dwordAddress(T1));                          \
    __ sqrtss(IceType_f##Size, Encoded_Xmm_##Dst(), Encoded_Xmm_##Src());      \
                                                                               \
    AssembledTest test = assemble();                                           \
    test.setDqwordTo(T0, test##Size##SrcValue);                                \
    test.setDqwordTo(T1, test##Size##DstValue);                                \
    test.run();                                                                \
                                                                               \
    Dqword Expected = test##Size##DstValue;                                    \
    Expected.F##Size[0] = Result;                                              \
    ASSERT_EQ(Expected, test.Dst<Dqword>()) << TestString;                     \
    reset();                                                                   \
  } while (0)

#define TestSqrtssXmmAddr(Dst, Value1, Result, Size)                           \
  do {                                                                         \
    static constexpr char TestString[] =                                       \
        "(" #Dst ", Addr, " #Value1 ", " #Result ", " #Size ")";               \
    const uint32_t T0 = allocateDqword();                                      \
    test##Size##SrcValue.F##Size[0] = Value1;                                  \
    const uint32_t T1 = allocateDqword();                                      \
                                                                               \
    __ movups(Encoded_Xmm_##Dst(), dwordAddress(T1));                          \
    __ sqrtss(IceType_f##Size, Encoded_Xmm_##Dst(), dwordAddress(T0));         \
                                                                               \
    AssembledTest test = assemble();                                           \
    test.setDqwordTo(T0, test##Size##SrcValue);                                \
    test.setDqwordTo(T1, test##Size##DstValue);                                \
    test.run();                                                                \
                                                                               \
    Dqword Expected = test##Size##DstValue;                                    \
    Expected.F##Size[0] = Result;                                              \
    ASSERT_EQ(Expected, test.Dst<Dqword>()) << TestString;                     \
    reset();                                                                   \
  } while (0)

#define TestSqrtssSize(Dst, Src, Size)                                         \
  do {                                                                         \
    TestSqrtssXmmXmm(Dst, Src, 4.0, 2.0, Size);                                \
    TestSqrtssXmmAddr(Dst, 4.0, 2.0, Size);                                    \
    TestSqrtssXmmXmm(Dst, Src, 9.0, 3.0, Size);                                \
    TestSqrtssXmmAddr(Dst, 9.0, 3.0, Size);                                    \
    TestSqrtssXmmXmm(Dst, Src, 100.0, 10.0, Size);                             \
    TestSqrtssXmmAddr(Dst, 100.0, 10.0, Size);                                 \
  } while (0)

#define TestSqrtss(Dst, Src)                                                   \
  do {                                                                         \
    TestSqrtssSize(Dst, Src, 32);                                              \
    TestSqrtssSize(Dst, Src, 64);                                              \
  } while (0)

  TestSqrtss(xmm0, xmm1);
  TestSqrtss(xmm1, xmm2);
  TestSqrtss(xmm2, xmm3);
  TestSqrtss(xmm3, xmm4);
  TestSqrtss(xmm4, xmm5);
  TestSqrtss(xmm5, xmm6);
  TestSqrtss(xmm6, xmm7);
  TestSqrtss(xmm7, xmm8);
  TestSqrtss(xmm8, xmm9);
  TestSqrtss(xmm9, xmm10);
  TestSqrtss(xmm10, xmm11);
  TestSqrtss(xmm11, xmm12);
  TestSqrtss(xmm12, xmm13);
  TestSqrtss(xmm13, xmm14);
  TestSqrtss(xmm14, xmm15);
  TestSqrtss(xmm15, xmm0);

#undef TestSqrtss
#undef TestSqrtssSize
#undef TestSqrtssXmmAddr
#undef TestSqrtssXmmXmm
}

TEST_F(AssemblerX8664Test, Insertps) {
#define TestInsertpsXmmXmmImm(Dst, Value0, Src, Value1, Imm, Expected)         \
  do {                                                                         \
    static constexpr char TestString[] =                                       \
        "(" #Dst ", " #Value0 ", " #Src ", " #Value1 ", " #Imm ", " #Expected  \
        ")";                                                                   \
    const uint32_t T0 = allocateDqword();                                      \
    const Dqword V0 Value0;                                                    \
    const uint32_t T1 = allocateDqword();                                      \
    const Dqword V1 Value1;                                                    \
                                                                               \
    __ movups(Encoded_Xmm_##Dst(), dwordAddress(T0));                          \
    __ movups(Encoded_Xmm_##Src(), dwordAddress(T1));                          \
    __ insertps(IceType_v4f32, Encoded_Xmm_##Dst(), Encoded_Xmm_##Src(),       \
                Immediate(Imm));                                               \
                                                                               \
    AssembledTest test = assemble();                                           \
    test.setDqwordTo(T0, V0);                                                  \
    test.setDqwordTo(T1, V1);                                                  \
    test.run();                                                                \
                                                                               \
    ASSERT_EQ(Dqword Expected, test.Dst<Dqword>()) << TestString;              \
    reset();                                                                   \
  } while (0)

#define TestInsertpsXmmAddrImm(Dst, Value0, Value1, Imm, Expected)             \
  do {                                                                         \
    static constexpr char TestString[] =                                       \
        "(" #Dst ", " #Value0 ", Addr, " #Value1 ", " #Imm ", " #Expected ")"; \
    const uint32_t T0 = allocateDqword();                                      \
    const Dqword V0 Value0;                                                    \
    const uint32_t T1 = allocateDqword();                                      \
    const Dqword V1 Value1;                                                    \
                                                                               \
    __ movups(Encoded_Xmm_##Dst(), dwordAddress(T0));                          \
    __ insertps(IceType_v4f32, Encoded_Xmm_##Dst(), dwordAddress(T1),          \
                Immediate(Imm));                                               \
                                                                               \
    AssembledTest test = assemble();                                           \
    test.setDqwordTo(T0, V0);                                                  \
    test.setDqwordTo(T1, V1);                                                  \
    test.run();                                                                \
                                                                               \
    ASSERT_EQ(Dqword Expected, test.Dst<Dqword>()) << TestString;              \
    reset();                                                                   \
  } while (0)

#define TestInsertps(Dst, Src)                                                 \
  do {                                                                         \
    TestInsertpsXmmXmmImm(                                                     \
        Dst, (uint64_t(-1), uint64_t(-1)), Src,                                \
        (uint64_t(0xAAAAAAAABBBBBBBBull), uint64_t(0xCCCCCCCCDDDDDDDDull)),    \
        0x99,                                                                  \
        (uint64_t(0xDDDDDDDD00000000ull), uint64_t(0x00000000FFFFFFFFull)));   \
    TestInsertpsXmmAddrImm(                                                    \
        Dst, (uint64_t(-1), uint64_t(-1)),                                     \
        (uint64_t(0xAAAAAAAABBBBBBBBull), uint64_t(0xCCCCCCCCDDDDDDDDull)),    \
        0x99,                                                                  \
        (uint64_t(0xBBBBBBBB00000000ull), uint64_t(0x00000000FFFFFFFFull)));   \
    TestInsertpsXmmXmmImm(                                                     \
        Dst, (uint64_t(-1), uint64_t(-1)), Src,                                \
        (uint64_t(0xAAAAAAAABBBBBBBBull), uint64_t(0xCCCCCCCCDDDDDDDDull)),    \
        0x9D,                                                                  \
        (uint64_t(0xDDDDDDDD00000000ull), uint64_t(0x0000000000000000ull)));   \
    TestInsertpsXmmAddrImm(                                                    \
        Dst, (uint64_t(-1), uint64_t(-1)),                                     \
        (uint64_t(0xAAAAAAAABBBBBBBBull), uint64_t(0xCCCCCCCCDDDDDDDDull)),    \
        0x9D,                                                                  \
        (uint64_t(0xBBBBBBBB00000000ull), uint64_t(0x0000000000000000ull)));   \
  } while (0)

  TestInsertps(xmm0, xmm1);
  TestInsertps(xmm1, xmm2);
  TestInsertps(xmm2, xmm3);
  TestInsertps(xmm3, xmm4);
  TestInsertps(xmm4, xmm5);
  TestInsertps(xmm5, xmm6);
  TestInsertps(xmm6, xmm7);
  TestInsertps(xmm7, xmm8);
  TestInsertps(xmm8, xmm9);
  TestInsertps(xmm9, xmm10);
  TestInsertps(xmm10, xmm11);
  TestInsertps(xmm11, xmm12);
  TestInsertps(xmm12, xmm13);
  TestInsertps(xmm13, xmm14);
  TestInsertps(xmm14, xmm15);
  TestInsertps(xmm15, xmm0);

#undef TestInsertps
#undef TestInsertpsXmmXmmAddr
#undef TestInsertpsXmmXmmImm
}

TEST_F(AssemblerX8664Test, Pinsr) {
  static constexpr uint8_t Mask32 = 0x03;
  static constexpr uint8_t Mask16 = 0x07;
  static constexpr uint8_t Mask8 = 0x0F;

#define TestPinsrXmmGPRImm(Dst, Value0, GPR, Value1, Imm, Size)                \
  do {                                                                         \
    static constexpr char TestString[] =                                       \
        "(" #Dst ", " #Value0 ", " #GPR ", " #Value1 ", " #Imm ", " #Size ")"; \
    const uint32_t T0 = allocateDqword();                                      \
    const Dqword V0 Value0;                                                    \
                                                                               \
    __ movups(Encoded_Xmm_##Dst(), dwordAddress(T0));                          \
    __ mov(IceType_i32, Encoded_GPR_##GPR(), Immediate(Value1));               \
    __ pinsr(IceType_i##Size, Encoded_Xmm_##Dst(), Encoded_GPR_##GPR(),        \
             Immediate(Imm));                                                  \
                                                                               \
    AssembledTest test = assemble();                                           \
    test.setDqwordTo(T0, V0);                                                  \
    test.run();                                                                \
                                                                               \
    constexpr uint8_t sel = (Imm)&Mask##Size;                                  \
    Dqword Expected = V0;                                                      \
    Expected.U##Size[sel] = Value1;                                            \
    ASSERT_EQ(Expected, test.Dst<Dqword>()) << TestString;                     \
    reset();                                                                   \
  } while (0)

#define TestPinsrXmmAddrImm(Dst, Value0, Value1, Imm, Size)                    \
  do {                                                                         \
    static constexpr char TestString[] =                                       \
        "(" #Dst ", " #Value0 ", Addr, " #Value1 ", " #Imm ", " #Size ")";     \
    const uint32_t T0 = allocateDqword();                                      \
    const Dqword V0 Value0;                                                    \
    const uint32_t T1 = allocateDword();                                       \
    const uint32_t V1 = Value1;                                                \
                                                                               \
    __ movups(Encoded_Xmm_##Dst(), dwordAddress(T0));                          \
    __ pinsr(IceType_i##Size, Encoded_Xmm_##Dst(), dwordAddress(T1),           \
             Immediate(Imm));                                                  \
                                                                               \
    AssembledTest test = assemble();                                           \
    test.setDqwordTo(T0, V0);                                                  \
    test.setDwordTo(T1, V1);                                                   \
    test.run();                                                                \
                                                                               \
    constexpr uint8_t sel = (Imm)&Mask##Size;                                  \
    Dqword Expected = V0;                                                      \
    Expected.U##Size[sel] = Value1;                                            \
    ASSERT_EQ(Expected, test.Dst<Dqword>()) << TestString;                     \
    reset();                                                                   \
  } while (0)

#define TestPinsrSize(Dst, GPR, Value1, Imm, Size)                             \
  do {                                                                         \
    TestPinsrXmmGPRImm(Dst, (uint64_t(0xAAAAAAAABBBBBBBBull),                  \
                             uint64_t(0xFFFFFFFFDDDDDDDDull)),                 \
                       GPR, Value1, Imm, Size);                                \
    TestPinsrXmmAddrImm(Dst, (uint64_t(0xAAAAAAAABBBBBBBBull),                 \
                              uint64_t(0xFFFFFFFFDDDDDDDDull)),                \
                        Value1, Imm, Size);                                    \
  } while (0)

#define TestPinsr(Src, Dst)                                                    \
  do {                                                                         \
    TestPinsrSize(Src, Dst, 0xEE, 0x03, 8);                                    \
    TestPinsrSize(Src, Dst, 0xFFEE, 0x03, 16);                                 \
    TestPinsrSize(Src, Dst, 0xC0FFEE, 0x03, 32);                               \
  } while (0)

  TestPinsr(xmm0, r1);
  TestPinsr(xmm1, r2);
  TestPinsr(xmm2, r3);
  TestPinsr(xmm3, r4);
  TestPinsr(xmm4, r5);
  TestPinsr(xmm5, r6);
  TestPinsr(xmm6, r7);
  TestPinsr(xmm7, r8);
  TestPinsr(xmm8, r10);
  TestPinsr(xmm9, r11);
  TestPinsr(xmm10, r12);
  TestPinsr(xmm11, r13);
  TestPinsr(xmm12, r14);
  TestPinsr(xmm13, r15);
  TestPinsr(xmm14, r1);
  TestPinsr(xmm15, r2);

#undef TestPinsr
#undef TestPinsrSize
#undef TestPinsrXmmAddrImm
#undef TestPinsrXmmGPRImm
}

TEST_F(AssemblerX8664Test, Pextr) {
  static constexpr uint8_t Mask32 = 0x03;
  static constexpr uint8_t Mask16 = 0x07;
  static constexpr uint8_t Mask8 = 0x0F;

#define TestPextrGPRXmmImm(GPR, Src, Value1, Imm, Size)                        \
  do {                                                                         \
    static constexpr char TestString[] =                                       \
        "(" #GPR ", " #Src ", " #Value1 ", " #Imm ", " #Size ")";              \
    const uint32_t T0 = allocateDqword();                                      \
    const Dqword V0 Value1;                                                    \
                                                                               \
    __ movups(Encoded_Xmm_##Src(), dwordAddress(T0));                          \
    __ pextr(IceType_i##Size, Encoded_GPR_##GPR(), Encoded_Xmm_##Src(),        \
             Immediate(Imm));                                                  \
                                                                               \
    AssembledTest test = assemble();                                           \
    test.setDqwordTo(T0, V0);                                                  \
    test.run();                                                                \
                                                                               \
    constexpr uint8_t sel = (Imm)&Mask##Size;                                  \
    ASSERT_EQ(V0.U##Size[sel], test.GPR()) << TestString;                      \
    reset();                                                                   \
  } while (0)

#define TestPextrSize(GPR, Src, Value1, Imm, Size)                             \
  do {                                                                         \
    TestPextrGPRXmmImm(GPR, Src, (uint64_t(0xAAAAAAAABBBBBBBBull),             \
                                  uint64_t(0xFFFFFFFFDDDDDDDDull)),            \
                       Imm, Size);                                             \
  } while (0)

#define TestPextr(Src, Dst)                                                    \
  do {                                                                         \
    TestPextrSize(Src, Dst, 0xEE, 0x03, 8);                                    \
    TestPextrSize(Src, Dst, 0xFFEE, 0x03, 16);                                 \
    TestPextrSize(Src, Dst, 0xC0FFEE, 0x03, 32);                               \
  } while (0)

  TestPextr(r1, xmm0);
  TestPextr(r2, xmm1);
  TestPextr(r3, xmm2);
  TestPextr(r4, xmm3);
  TestPextr(r5, xmm4);
  TestPextr(r6, xmm5);
  TestPextr(r7, xmm6);
  TestPextr(r8, xmm7);
  TestPextr(r10, xmm8);
  TestPextr(r11, xmm9);
  TestPextr(r12, xmm10);
  TestPextr(r13, xmm11);
  TestPextr(r14, xmm12);
  TestPextr(r15, xmm13);
  TestPextr(r1, xmm14);
  TestPextr(r2, xmm15);

#undef TestPextr
#undef TestPextrSize
#undef TestPextrXmmGPRImm
}

TEST_F(AssemblerX8664Test, Pcmpeq_Pcmpgt) {
#define TestPcmpXmmXmm(Dst, Value0, Src, Value1, Size, Inst, Op)               \
  do {                                                                         \
    static constexpr char TestString[] =                                       \
        "(" #Dst ", " #Value0 ", " #Src ", " #Value1 ", " #Size ", " #Op ")";  \
    const uint32_t T0 = allocateDqword();                                      \
    const Dqword V0 Value0;                                                    \
    const uint32_t T1 = allocateDqword();                                      \
    const Dqword V1 Value1;                                                    \
                                                                               \
    __ movups(Encoded_Xmm_##Dst(), dwordAddress(T0));                          \
    __ movups(Encoded_Xmm_##Src(), dwordAddress(T1));                          \
    __ Inst(IceType_i##Size, Encoded_Xmm_##Dst(), Encoded_Xmm_##Src());        \
                                                                               \
    AssembledTest test = assemble();                                           \
    test.setDqwordTo(T0, V0);                                                  \
    test.setDqwordTo(T1, V1);                                                  \
    test.run();                                                                \
                                                                               \
    Dqword Expected(uint64_t(0), uint64_t(0));                                 \
    static constexpr uint8_t ArraySize =                                       \
        sizeof(Dqword) / sizeof(uint##Size##_t);                               \
    for (uint8_t i = 0; i < ArraySize; ++i) {                                  \
      Expected.I##Size[i] = (V1.I##Size[i] Op V0.I##Size[i]) ? -1 : 0;         \
    }                                                                          \
    ASSERT_EQ(Expected, test.Dst<Dqword>()) << TestString;                     \
    reset();                                                                   \
  } while (0)

#define TestPcmpXmmAddr(Dst, Value0, Value1, Size, Inst, Op)                   \
  do {                                                                         \
    static constexpr char TestString[] =                                       \
        "(" #Dst ", " #Value0 ", Addr, " #Value1 ", " #Size ", " #Op ")";      \
    const uint32_t T0 = allocateDqword();                                      \
    const Dqword V0 Value0;                                                    \
    const uint32_t T1 = allocateDqword();                                      \
    const Dqword V1 Value1;                                                    \
                                                                               \
    __ movups(Encoded_Xmm_##Dst(), dwordAddress(T0));                          \
    __ Inst(IceType_i##Size, Encoded_Xmm_##Dst(), dwordAddress(T1));           \
                                                                               \
    AssembledTest test = assemble();                                           \
    test.setDqwordTo(T0, V0);                                                  \
    test.setDqwordTo(T1, V1);                                                  \
    test.run();                                                                \
                                                                               \
    Dqword Expected(uint64_t(0), uint64_t(0));                                 \
    static constexpr uint8_t ArraySize =                                       \
        sizeof(Dqword) / sizeof(uint##Size##_t);                               \
    for (uint8_t i = 0; i < ArraySize; ++i) {                                  \
      Expected.I##Size[i] = (V1.I##Size[i] Op V0.I##Size[i]) ? -1 : 0;         \
    }                                                                          \
    ASSERT_EQ(Expected, test.Dst<Dqword>()) << TestString;                     \
    reset();                                                                   \
  } while (0)

#define TestPcmpValues(Dst, Value0, Src, Value1, Size)                         \
  do {                                                                         \
    TestPcmpXmmXmm(Dst, Value0, Src, Value1, Size, pcmpeq, == );               \
    TestPcmpXmmAddr(Dst, Value0, Value1, Size, pcmpeq, == );                   \
    TestPcmpXmmXmm(Dst, Value0, Src, Value1, Size, pcmpgt, < );                \
    TestPcmpXmmAddr(Dst, Value0, Value1, Size, pcmpgt, < );                    \
  } while (0)

#define TestPcmpSize(Dst, Src, Size)                                           \
  do {                                                                         \
    TestPcmpValues(Dst, (uint64_t(0x8888888888888888ull),                      \
                         uint64_t(0x0000000000000000ull)),                     \
                   Src, (uint64_t(0x0000008800008800ull),                      \
                         uint64_t(0xFFFFFFFFFFFFFFFFull)),                     \
                   Size);                                                      \
    TestPcmpValues(Dst, (uint64_t(0x123567ABAB55DE01ull),                      \
                         uint64_t(0x12345abcde12345Aull)),                     \
                   Src, (uint64_t(0x0000008800008800ull),                      \
                         uint64_t(0xAABBCCDD1234321Aull)),                     \
                   Size);                                                      \
  } while (0)

#define TestPcmp(Dst, Src)                                                     \
  do {                                                                         \
    TestPcmpSize(xmm0, xmm1, 8);                                               \
    TestPcmpSize(xmm0, xmm1, 16);                                              \
    TestPcmpSize(xmm0, xmm1, 32);                                              \
  } while (0)

  TestPcmp(xmm0, xmm1);
  TestPcmp(xmm1, xmm2);
  TestPcmp(xmm2, xmm3);
  TestPcmp(xmm3, xmm4);
  TestPcmp(xmm4, xmm5);
  TestPcmp(xmm5, xmm6);
  TestPcmp(xmm6, xmm7);
  TestPcmp(xmm7, xmm8);
  TestPcmp(xmm8, xmm9);
  TestPcmp(xmm9, xmm10);
  TestPcmp(xmm10, xmm11);
  TestPcmp(xmm11, xmm12);
  TestPcmp(xmm12, xmm13);
  TestPcmp(xmm13, xmm14);
  TestPcmp(xmm14, xmm15);
  TestPcmp(xmm15, xmm0);

#undef TestPcmp
#undef TestPcmpSize
#undef TestPcmpValues
#undef TestPcmpXmmAddr
#undef TestPcmpXmmXmm
}

TEST_F(AssemblerX8664Test, Roundsd) {
#define TestRoundsdXmmXmm(Dst, Src, Mode, Input, RN)                           \
  do {                                                                         \
    static constexpr char TestString[] =                                       \
        "(" #Dst ", " #Src ", " #Mode ", " #Input ", " #RN ")";                \
    const uint32_t T0 = allocateDqword();                                      \
    const Dqword V0(-3.0, -3.0);                                               \
    const uint32_t T1 = allocateDqword();                                      \
    const Dqword V1(double(Input), -123.4);                                    \
                                                                               \
    __ movups(Encoded_Xmm_##Dst(), dwordAddress(T0));                          \
    __ movups(Encoded_Xmm_##Src(), dwordAddress(T1));                          \
    __ roundsd(Encoded_Xmm_##Dst(), Encoded_Xmm_##Src(),                       \
               AssemblerX8664::k##Mode);                                       \
                                                                               \
    AssembledTest test = assemble();                                           \
    test.setDqwordTo(T0, V0);                                                  \
    test.setDqwordTo(T1, V1);                                                  \
    test.run();                                                                \
                                                                               \
    const Dqword Expected(double(RN), -3.0);                                   \
    EXPECT_EQ(Expected, test.Dst<Dqword>()) << TestString;                     \
    reset();                                                                   \
  } while (0)

#define TestRoundsd(Dst, Src)                                                  \
  do {                                                                         \
    TestRoundsdXmmXmm(Dst, Src, RoundToNearest, 5.51, 6);                      \
    TestRoundsdXmmXmm(Dst, Src, RoundToNearest, 5.49, 5);                      \
    TestRoundsdXmmXmm(Dst, Src, RoundDown, 5.51, 5);                           \
    TestRoundsdXmmXmm(Dst, Src, RoundUp, 5.49, 6);                             \
    TestRoundsdXmmXmm(Dst, Src, RoundToZero, 5.49, 5);                         \
    TestRoundsdXmmXmm(Dst, Src, RoundToZero, 5.51, 5);                         \
  } while (0)

  TestRoundsd(xmm0, xmm1);
  TestRoundsd(xmm1, xmm2);
  TestRoundsd(xmm2, xmm3);
  TestRoundsd(xmm3, xmm4);
  TestRoundsd(xmm4, xmm5);
  TestRoundsd(xmm5, xmm6);
  TestRoundsd(xmm6, xmm7);
  TestRoundsd(xmm7, xmm8);
  TestRoundsd(xmm8, xmm9);
  TestRoundsd(xmm9, xmm10);
  TestRoundsd(xmm10, xmm11);
  TestRoundsd(xmm11, xmm12);
  TestRoundsd(xmm12, xmm13);
  TestRoundsd(xmm13, xmm14);
  TestRoundsd(xmm14, xmm15);
  TestRoundsd(xmm15, xmm0);

#undef TestRoundsd
#undef TestRoundsdXmmXmm
}

TEST_F(AssemblerX8664Test, Set1ps) {
#define TestImpl(Xmm, Src, Imm)                                                \
  do {                                                                         \
    __ set1ps(Encoded_Xmm_##Xmm(), Encoded_GPR_##Src(), Immediate(Imm));       \
                                                                               \
    AssembledTest test = assemble();                                           \
    test.run();                                                                \
                                                                               \
    const Dqword Expected((uint64_t(Imm) << 32) | uint32_t(Imm),               \
                          (uint64_t(Imm) << 32) | uint32_t(Imm));              \
    ASSERT_EQ(Expected, test.Xmm<Dqword>())                                    \
        << "(" #Xmm ", " #Src ", " #Imm ")";                                   \
    reset();                                                                   \
  } while (0)

  TestImpl(xmm0, r1, 1);
  TestImpl(xmm1, r2, 12);
  TestImpl(xmm2, r3, 22);
  TestImpl(xmm3, r4, 54);
  TestImpl(xmm4, r5, 80);
  TestImpl(xmm5, r6, 32);
  TestImpl(xmm6, r7, 55);
  TestImpl(xmm7, r8, 44);
  TestImpl(xmm8, r10, 10);
  TestImpl(xmm9, r11, 155);
  TestImpl(xmm10, r12, 165);
  TestImpl(xmm11, r13, 170);
  TestImpl(xmm12, r14, 200);
  TestImpl(xmm13, r15, 124);
  TestImpl(xmm14, r1, 101);
  TestImpl(xmm15, r2, 166);

#undef TestImpl
}

} // end of anonymous namespace
} // end of namespace Test
} // end of namespace X8664
} // end of namespace Ice
