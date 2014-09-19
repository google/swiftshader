//===- subzero/src/IceIntrinsics.cpp - Functions related to intrinsics ----===//
//
//                        The Subzero Code Generator
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements the Intrinsics utilities for matching and
// then dispatching by name.
//
//===----------------------------------------------------------------------===//

#include "IceCfg.h"
#include "IceCfgNode.h"
#include "IceInst.h"
#include "IceIntrinsics.h"
#include "IceLiveness.h"
#include "IceOperand.h"

#include <utility>

namespace Ice {

namespace {

void __attribute__((unused)) xIntrinsicInfoSizeCheck() {
  STATIC_ASSERT(sizeof(Intrinsics::IntrinsicInfo) == 4);
}

#define INTRIN(ID, SE, RT) { Intrinsics::ID, Intrinsics::SE, Intrinsics::RT }

// Build list of intrinsics with their attributes and expected prototypes.
// List is sorted alphabetically.
const struct IceIntrinsicsEntry_ {
  Intrinsics::FullIntrinsicInfo Info;
  const char *IntrinsicName;
} IceIntrinsicsTable[] = {

#define AtomicCmpxchgInit(Overload, NameSuffix)                                \
  {                                                                            \
    {                                                                          \
      INTRIN(AtomicCmpxchg, SideEffects_T, ReturnsTwice_F),                    \
      { Overload, IceType_i32, Overload, Overload, IceType_i32, IceType_i32 }, \
      6 },                                                                     \
    "nacl.atomic.cmpxchg." NameSuffix                                          \
  }
    AtomicCmpxchgInit(IceType_i8, "i8"),
    AtomicCmpxchgInit(IceType_i16, "i16"),
    AtomicCmpxchgInit(IceType_i32, "i32"),
    AtomicCmpxchgInit(IceType_i64, "i64"),
#undef AtomicCmpxchgInit

    { { INTRIN(AtomicFence, SideEffects_T, ReturnsTwice_F),
        { IceType_void, IceType_i32 }, 2 },
      "nacl.atomic.fence" },
    { { INTRIN(AtomicFenceAll, SideEffects_T, ReturnsTwice_F),
        { IceType_void }, 1 },
      "nacl.atomic.fence.all" },
    { { INTRIN(AtomicIsLockFree, SideEffects_F, ReturnsTwice_F),
        { IceType_i1, IceType_i32, IceType_i32 }, 3 },
      "nacl.atomic.is.lock.free" },

#define AtomicLoadInit(Overload, NameSuffix)                                   \
  {                                                                            \
    {                                                                          \
      INTRIN(AtomicLoad, SideEffects_T, ReturnsTwice_F),                       \
      { Overload, IceType_i32, IceType_i32 }, 3 },                             \
    "nacl.atomic.load." NameSuffix                                             \
  }
    AtomicLoadInit(IceType_i8, "i8"),
    AtomicLoadInit(IceType_i16, "i16"),
    AtomicLoadInit(IceType_i32, "i32"),
    AtomicLoadInit(IceType_i64, "i64"),
#undef AtomicLoadInit

#define AtomicRMWInit(Overload, NameSuffix)                                    \
  {                                                                            \
    {                                                                          \
      INTRIN(AtomicRMW, SideEffects_T, ReturnsTwice_F)                         \
      , { Overload, IceType_i32, IceType_i32, Overload, IceType_i32 }, 5       \
    }                                                                          \
    , "nacl.atomic.rmw." NameSuffix                                            \
  }
    AtomicRMWInit(IceType_i8, "i8"),
    AtomicRMWInit(IceType_i16, "i16"),
    AtomicRMWInit(IceType_i32, "i32"),
    AtomicRMWInit(IceType_i64, "i64"),
#undef AtomicRMWInit

#define AtomicStoreInit(Overload, NameSuffix)                                  \
  {                                                                            \
    {                                                                          \
      INTRIN(AtomicStore, SideEffects_T, ReturnsTwice_F)                       \
      , { IceType_void, Overload, IceType_i32, IceType_i32 }, 4                \
    }                                                                          \
    , "nacl.atomic.store." NameSuffix                                          \
  }
    AtomicStoreInit(IceType_i8, "i8"),
    AtomicStoreInit(IceType_i16, "i16"),
    AtomicStoreInit(IceType_i32, "i32"),
    AtomicStoreInit(IceType_i64, "i64"),
#undef AtomicStoreInit

#define BswapInit(Overload, NameSuffix)                                        \
  {                                                                            \
    {                                                                          \
      INTRIN(Bswap, SideEffects_F, ReturnsTwice_F)                             \
      , { Overload, Overload }, 2                                              \
    }                                                                          \
    , "bswap." NameSuffix                                                      \
  }
    BswapInit(IceType_i16, "i16"),
    BswapInit(IceType_i32, "i32"),
    BswapInit(IceType_i64, "i64"),
#undef BswapInit

#define CtlzInit(Overload, NameSuffix)                                         \
  {                                                                            \
    {                                                                          \
      INTRIN(Ctlz, SideEffects_F, ReturnsTwice_F)                              \
      , { Overload, Overload, IceType_i1 }, 3                                  \
    }                                                                          \
    , "ctlz." NameSuffix                                                       \
  }
    CtlzInit(IceType_i32, "i32"),
    CtlzInit(IceType_i64, "i64"),
#undef CtlzInit

#define CtpopInit(Overload, NameSuffix)                                        \
  {                                                                            \
    {                                                                          \
      INTRIN(Ctpop, SideEffects_F, ReturnsTwice_F)                             \
      , { Overload, Overload }, 2                                              \
    }                                                                          \
    , "ctpop." NameSuffix                                                      \
  }
    CtpopInit(IceType_i32, "i32"),
    CtpopInit(IceType_i64, "i64"),
#undef CtpopInit

#define CttzInit(Overload, NameSuffix)                                         \
  {                                                                            \
    {                                                                          \
      INTRIN(Cttz, SideEffects_F, ReturnsTwice_F)                              \
      , { Overload, Overload, IceType_i1 }, 3                                  \
    }                                                                          \
    , "cttz." NameSuffix                                                       \
  }
    CttzInit(IceType_i32, "i32"),
    CttzInit(IceType_i64, "i64"),
#undef CttzInit

    { { INTRIN(Longjmp, SideEffects_T, ReturnsTwice_F),
        { IceType_void, IceType_i32, IceType_i32 }, 3 },
      "nacl.longjmp" },
    { { INTRIN(Memcpy, SideEffects_T, ReturnsTwice_F),
        { IceType_void, IceType_i32, IceType_i32, IceType_i32, IceType_i32,
          IceType_i1},
        6 },
      "memcpy.p0i8.p0i8.i32" },
    { { INTRIN(Memmove, SideEffects_T, ReturnsTwice_F),
        { IceType_void, IceType_i32, IceType_i32, IceType_i32, IceType_i32,
          IceType_i1 },
        6 },
      "memmove.p0i8.p0i8.i32" },
    { { INTRIN(Memset, SideEffects_T, ReturnsTwice_F),
        { IceType_void, IceType_i32, IceType_i8, IceType_i32, IceType_i32,
          IceType_i1 },
        6 },
      "memset.p0i8.i32" },
    { { INTRIN(NaClReadTP, SideEffects_F, ReturnsTwice_F),
        { IceType_i32 }, 1 },
      "nacl.read.tp" },
    { { INTRIN(Setjmp, SideEffects_T, ReturnsTwice_T),
        { IceType_i32, IceType_i32 }, 2 },
      "nacl.setjmp" },

#define SqrtInit(Overload, NameSuffix)                                         \
  {                                                                            \
    {                                                                          \
      INTRIN(Sqrt, SideEffects_F, ReturnsTwice_F),                             \
      { Overload, Overload }, 2 },                                             \
      "sqrt." NameSuffix                                                       \
  }
    SqrtInit(IceType_f32, "f32"),
    SqrtInit(IceType_f64, "f64"),
#undef SqrtInit

    { { INTRIN(Stacksave, SideEffects_T, ReturnsTwice_F),
        { IceType_i32 }, 1 },
      "stacksave" },
    { { INTRIN(Stackrestore, SideEffects_T, ReturnsTwice_F),
        { IceType_void, IceType_i32 }, 2 },
      "stackrestore" },
    { { INTRIN(Trap, SideEffects_T, ReturnsTwice_F),
        { IceType_void }, 1 },
      "trap" }
};
const size_t IceIntrinsicsTableSize = llvm::array_lengthof(IceIntrinsicsTable);

#undef INTRIN

} // end of anonymous namespace

Intrinsics::Intrinsics() {
  for (size_t I = 0; I < IceIntrinsicsTableSize; ++I) {
    const struct IceIntrinsicsEntry_ &Entry = IceIntrinsicsTable[I];
    assert(Entry.Info.NumTypes <= kMaxIntrinsicParameters);
    map.insert(std::make_pair(IceString(Entry.IntrinsicName), Entry.Info));
  }
}

Intrinsics::~Intrinsics() {}

const Intrinsics::FullIntrinsicInfo *
Intrinsics::find(const IceString &Name) const {
  IntrinsicMap::const_iterator it = map.find(Name);
  if (it == map.end())
    return NULL;
  return &it->second;
}

bool Intrinsics::VerifyMemoryOrder(uint64_t Order) {
  // There is only one memory ordering for atomics allowed right now.
  return Order == Intrinsics::MemoryOrderSequentiallyConsistent;
}

Intrinsics::ValidateCallValue
Intrinsics::FullIntrinsicInfo::validateCall(const Ice::InstCall *Call,
                                            SizeT &ArgIndex) const {
  assert(NumTypes >= 1);
  Variable *Result = Call->getDest();
  if (Result == NULL) {
    if (Signature[0] != Ice::IceType_void)
      return Intrinsics::BadReturnType;
  } else if (Signature[0] != Result->getType()) {
    return Intrinsics::BadReturnType;
  }
  if (Call->getNumArgs() + 1 != NumTypes) {
    return Intrinsics::WrongNumOfArgs;
  }
  for (size_t i = 1; i < NumTypes; ++i) {
    if (Call->getArg(i - 1)->getType() != Signature[i]) {
      ArgIndex = i;
      return Intrinsics::WrongCallArgType;
    }
  }
  return Intrinsics::IsValidCall;
}

Type Intrinsics::FullIntrinsicInfo::getArgType(SizeT Index) const {
  assert(NumTypes > 1);
  assert(Index + 1 < NumTypes);
  return Signature[Index + 1];
}

} // end of namespace Ice
