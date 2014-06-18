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
#include "IceIntrinsics.h"
#include "IceLiveness.h"
#include "IceOperand.h"

#include <utility>

namespace Ice {

namespace {

const struct IceIntrinsicsEntry_ {
  Intrinsics::FullIntrinsicInfo Info;
  const char *IntrinsicName;
} IceIntrinsicsTable[] = {
#define AtomicCmpxchgInit(Overload, NameSuffix)                                \
  {                                                                            \
    {                                                                          \
      { Intrinsics::AtomicCmpxchg, true },                                     \
      { Overload, IceType_i32, Overload, Overload, IceType_i32, IceType_i32 }, \
      6                                                                        \
    }                                                                          \
    , "nacl.atomic.cmpxchg." NameSuffix                                        \
  }
    AtomicCmpxchgInit(IceType_i8, "i8"),
    AtomicCmpxchgInit(IceType_i16, "i16"),
    AtomicCmpxchgInit(IceType_i32, "i32"),
    AtomicCmpxchgInit(IceType_i64, "i64"),
#undef AtomicCmpxchgInit
    { { { Intrinsics::AtomicFence, true }, { IceType_void, IceType_i32 }, 2 },
      "nacl.atomic.fence" },
    { { { Intrinsics::AtomicFenceAll, true }, { IceType_void }, 1 },
      "nacl.atomic.fence.all" },
    { { { Intrinsics::AtomicIsLockFree, true },
        { IceType_i1, IceType_i32, IceType_i32 }, 3 },
      "nacl.atomic.is.lock.free" },

#define AtomicLoadInit(Overload, NameSuffix)                                   \
  {                                                                            \
    {                                                                          \
      { Intrinsics::AtomicLoad, true }                                         \
      , { Overload, IceType_i32, IceType_i32 }, 3                              \
    }                                                                          \
    , "nacl.atomic.load." NameSuffix                                           \
  }
    AtomicLoadInit(IceType_i8, "i8"),
    AtomicLoadInit(IceType_i16, "i16"),
    AtomicLoadInit(IceType_i32, "i32"),
    AtomicLoadInit(IceType_i64, "i64"),
#undef AtomicLoadInit

#define AtomicRMWInit(Overload, NameSuffix)                                    \
  {                                                                            \
    {                                                                          \
      { Intrinsics::AtomicRMW, true }                                          \
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
      { Intrinsics::AtomicStore, true }                                        \
      , { IceType_void, Overload, IceType_i32, IceType_i32 }, 5                \
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
      { Intrinsics::Bswap, false }                                             \
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
      { Intrinsics::Ctlz, false }                                              \
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
      { Intrinsics::Ctpop, false }                                             \
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
      { Intrinsics::Cttz, false }                                              \
      , { Overload, Overload, IceType_i1 }, 3                                  \
    }                                                                          \
    , "cttz." NameSuffix                                                       \
  }
    CttzInit(IceType_i32, "i32"),
    CttzInit(IceType_i64, "i64"),
#undef CttzInit
    { { { Intrinsics::Longjmp, true },
        { IceType_void, IceType_i32, IceType_i32 }, 3 },
      "nacl.longjmp" },
    { { { Intrinsics::Memcpy, true }, { IceType_void, IceType_i32, IceType_i32,
                                        IceType_i32,  IceType_i32, IceType_i1 },
        6 },
      "memcpy.p0i8.p0i8.i32" },
    { { { Intrinsics::Memmove, true },
        { IceType_void, IceType_i32, IceType_i32,
          IceType_i32,  IceType_i32, IceType_i1 },
        6 },
      "memmove.p0i8.p0i8.i32" },
    { { { Intrinsics::Memset, true }, { IceType_void, IceType_i32, IceType_i8,
                                        IceType_i32,  IceType_i32, IceType_i1 },
        6 },
      "memset.p0i8.i32" },
    { { { Intrinsics::NaClReadTP, false }, { IceType_i32 }, 1 },
      "nacl.read.tp" },
    { { { Intrinsics::Setjmp, true }, { IceType_i32, IceType_i32 }, 2 },
      "nacl.setjmp" },

#define SqrtInit(Overload, NameSuffix)                                         \
  {                                                                            \
    {                                                                          \
      { Intrinsics::Sqrt, false }                                              \
      , { Overload, Overload }, 2                                              \
    }                                                                          \
    , "sqrt." NameSuffix                                                       \
  }
    SqrtInit(IceType_f32, "f32"),
    SqrtInit(IceType_f64, "f64"),
#undef SqrtInit
    { { { Intrinsics::Stacksave, true }, { IceType_i32 }, 1 }, "stacksave" },
    { { { Intrinsics::Stackrestore, true }, { IceType_void, IceType_i32 }, 2 },
      "stackrestore" },
    { { { Intrinsics::Trap, true }, { IceType_void }, 1 }, "trap" }
  };
const size_t IceIntrinsicsTableSize = llvm::array_lengthof(IceIntrinsicsTable);

} // end of namespace

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

} // end of namespace Ice
