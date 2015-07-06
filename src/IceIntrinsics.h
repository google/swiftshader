//===- subzero/src/IceIntrinsics.h - List of Ice Intrinsics -----*- C++ -*-===//
//
//                        The Subzero Code Generator
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file declares the kinds of intrinsics supported by PNaCl.
///
//===----------------------------------------------------------------------===//

#ifndef SUBZERO_SRC_ICEINTRINSICS_H
#define SUBZERO_SRC_ICEINTRINSICS_H

#include "IceDefs.h"
#include "IceTypes.h"

namespace Ice {

class InstCall;

static const size_t kMaxIntrinsicParameters = 6;

class Intrinsics {
  Intrinsics(const Intrinsics &) = delete;
  Intrinsics &operator=(const Intrinsics &) = delete;

public:
  Intrinsics();
  ~Intrinsics();

  /// Some intrinsics allow overloading by type. This enum collapses all
  /// overloads into a single ID, but the type can still be recovered by the
  /// type of the intrinsic function call's return value and parameters.
  enum IntrinsicID {
    UnknownIntrinsic = 0,
    // Arbitrary (alphabetical) order.
    AtomicCmpxchg,
    AtomicFence,
    AtomicFenceAll,
    AtomicIsLockFree,
    AtomicLoad,
    AtomicRMW,
    AtomicStore,
    Bswap,
    Ctlz,
    Ctpop,
    Cttz,
    Fabs,
    Longjmp,
    Memcpy,
    Memmove,
    Memset,
    NaClReadTP,
    Setjmp,
    Sqrt,
    Stacksave,
    Stackrestore,
    Trap
  };

  /// Operations that can be represented by the AtomicRMW
  /// intrinsic.
  ///
  /// Do not reorder these values: their order offers forward
  /// compatibility of bitcode targeted to PNaCl.
  enum AtomicRMWOperation {
    AtomicInvalid = 0, // Invalid, keep first.
    AtomicAdd,
    AtomicSub,
    AtomicOr,
    AtomicAnd,
    AtomicXor,
    AtomicExchange,
    AtomicNum // Invalid, keep last.
  };

  /// Memory orderings supported by PNaCl IR.
  ///
  /// Do not reorder these values: their order offers forward
  /// compatibility of bitcode targeted to PNaCl.
  enum MemoryOrder {
    MemoryOrderInvalid = 0, // Invalid, keep first.
    MemoryOrderRelaxed,
    MemoryOrderConsume,
    MemoryOrderAcquire,
    MemoryOrderRelease,
    MemoryOrderAcquireRelease,
    MemoryOrderSequentiallyConsistent,
    MemoryOrderNum // Invalid, keep last.
  };

  /// Verify memory ordering rules for atomic intrinsics.  For
  /// AtomicCmpxchg, Order is the "success" ordering and OrderOther is
  /// the "failure" ordering.  Returns true if valid, false if invalid.
  // TODO(stichnot,kschimpf): Perform memory order validation in the
  // bitcode reader/parser, allowing LLVM and Subzero to share.  See
  // https://code.google.com/p/nativeclient/issues/detail?id=4126 .
  static bool isMemoryOrderValid(IntrinsicID ID, uint64_t Order,
                                 uint64_t OrderOther = MemoryOrderInvalid);

  enum SideEffects { SideEffects_F = 0, SideEffects_T = 1 };

  enum ReturnsTwice { ReturnsTwice_F = 0, ReturnsTwice_T = 1 };

  /// Basic attributes related to each intrinsic, that are relevant to
  /// code generation. Perhaps the attributes representation can be shared
  /// with general function calls, but PNaCl currently strips all
  /// attributes from functions.
  struct IntrinsicInfo {
    enum IntrinsicID ID : 30;
    enum SideEffects HasSideEffects : 1;
    enum ReturnsTwice ReturnsTwice : 1;
  };

  /// The types of validation values for FullIntrinsicInfo.validateCall.
  enum ValidateCallValue {
    IsValidCall,      /// Valid use of instrinsic call.
    BadReturnType,    /// Return type invalid for intrinsic.
    WrongNumOfArgs,   /// Wrong number of arguments for intrinsic.
    WrongCallArgType, /// Argument of wrong type.
  };

  /// The complete set of information about an intrinsic.
  struct FullIntrinsicInfo {
    struct IntrinsicInfo Info; /// Information that CodeGen would care about.

    // Sanity check during parsing.
    Type Signature[kMaxIntrinsicParameters];
    uint8_t NumTypes;

    /// Validates that type signature of call matches intrinsic.
    /// If WrongArgumentType is returned, ArgIndex is set to corresponding
    /// argument index.
    ValidateCallValue validateCall(const Ice::InstCall *Call,
                                   SizeT &ArgIndex) const;

    /// Returns the return type of the intrinsic.
    Type getReturnType() const {
      assert(NumTypes > 1);
      return Signature[0];
    }

    /// Returns number of arguments expected.
    SizeT getNumArgs() const {
      assert(NumTypes > 1);
      return NumTypes - 1;
    }

    /// Returns type of Index-th argument.
    Type getArgType(SizeT Index) const;
  };

  /// Find the information about a given intrinsic, based on function name.  If
  /// the function name does not have the common "llvm." prefix, nullptr is
  /// returned and Error is set to false.  Otherwise, tries to find a reference
  /// to a FullIntrinsicInfo entry (valid for the lifetime of the map).  If
  /// found, sets Error to false and returns the reference.  If not found, sets
  /// Error to true and returns nullptr (indicating an unknown "llvm.foo"
  /// intrinsic).
  const FullIntrinsicInfo *find(const IceString &Name, bool &Error) const;

private:
  // TODO(jvoung): May want to switch to something like LLVM's StringMap.
  typedef std::map<IceString, FullIntrinsicInfo> IntrinsicMap;
  IntrinsicMap Map;
};

} // end of namespace Ice

#endif // SUBZERO_SRC_ICEINTRINSICS_H
