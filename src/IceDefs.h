//===- subzero/src/IceDefs.h - Common Subzero declarations ------*- C++ -*-===//
//
//                        The Subzero Code Generator
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file declares various useful types and classes that have widespread use
/// across Subzero. Every Subzero source file is expected to include IceDefs.h.
///
//===----------------------------------------------------------------------===//

#ifndef SUBZERO_SRC_ICEDEFS_H
#define SUBZERO_SRC_ICEDEFS_H

#include "IceBuildDefs.h" // TODO(stichnot): move into individual files
#include "IceTLS.h"

#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/BitVector.h"
#include "llvm/ADT/ilist.h"
#include "llvm/ADT/ilist_node.h"
#include "llvm/ADT/iterator_range.h"
#include "llvm/ADT/SmallBitVector.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/Support/Allocator.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/ELF.h"
#include "llvm/Support/raw_ostream.h"

#include <cassert>
#include <cstdint>
#include <cstdio>     // snprintf
#include <functional> // std::less
#include <limits>
#include <list>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <system_error>
#include <unordered_map>
#include <vector>

namespace Ice {

class Assembler;
class Cfg;
class CfgNode;
class Constant;
class ELFObjectWriter;
class ELFStreamer;
class FunctionDeclaration;
class GlobalContext;
class GlobalDeclaration;
class Inst;
class InstAssign;
class InstJumpTable;
class InstPhi;
class InstSwitch;
class InstTarget;
class LiveRange;
class Liveness;
class Operand;
class TargetDataLowering;
class TargetLowering;
class Variable;
class VariableDeclaration;
class VariablesMetadata;

template <size_t SlabSize = 1024 * 1024>
using ArenaAllocator =
    llvm::BumpPtrAllocatorImpl<llvm::MallocAllocator, SlabSize>;

ArenaAllocator<> *getCurrentCfgAllocator();

template <typename T> struct CfgLocalAllocator {
  using value_type = T;
  using pointer = T *;
  using const_pointer = const T *;
  using reference = T &;
  using const_reference = const T &;
  using size_type = std::size_t;
  CfgLocalAllocator() = default;
  template <class U> CfgLocalAllocator(const CfgLocalAllocator<U> &) {}
  pointer allocate(size_type Num) {
    return getCurrentCfgAllocator()->Allocate<T>(Num);
  }
  void deallocate(pointer, size_type) {}
  template <class U> struct rebind { typedef CfgLocalAllocator<U> other; };
  void construct(pointer P, const T &Val) {
    new (static_cast<void *>(P)) T(Val);
  }
  void destroy(pointer P) { P->~T(); }
};
template <typename T, typename U>
inline bool operator==(const CfgLocalAllocator<T> &,
                       const CfgLocalAllocator<U> &) {
  return true;
}
template <typename T, typename U>
inline bool operator!=(const CfgLocalAllocator<T> &,
                       const CfgLocalAllocator<U> &) {
  return false;
}

// makeUnique should be used when memory is expected to be allocated from the
// heap (as opposed to allocated from some Allocator.) It is intended to be
// used instead of new.
//
// The expected usage is as follows
//
// class MyClass {
// public:
//   static std::unique_ptr<MyClass> create(<ctor_args>) {
//     return makeUnique<MyClass>(<ctor_args>);
//   }
//
// private:
//   ENABLE_MAKE_UNIQUE;
//
//   MyClass(<ctor_args>) ...
// }
//
// ENABLE_MAKE_UNIQUE is a trick that is necessary if MyClass' ctor is private.
// Private ctors are highly encouraged when you're writing a class that you'd
// like to have allocated with makeUnique as it would prevent users from
// declaring stack allocated variables.
namespace Internal {
struct MakeUniqueEnabler {
  template <class T, class... Args>
  static std::unique_ptr<T> create(Args &&... TheArgs) {
    std::unique_ptr<T> Unique(new T(std::forward<Args>(TheArgs)...));
    return Unique;
  }
};
} // end of namespace Internal

template <class T, class... Args>
static std::unique_ptr<T> makeUnique(Args &&... TheArgs) {
  return ::Ice::Internal::MakeUniqueEnabler::create<T>(
      std::forward<Args>(TheArgs)...);
}

#define ENABLE_MAKE_UNIQUE friend struct ::Ice::Internal::MakeUniqueEnabler

using IceString = std::string;
using InstList = llvm::ilist<Inst>;
// Ideally PhiList would be llvm::ilist<InstPhi>, and similar for AssignList,
// but this runs into issues with SFINAE.
using PhiList = InstList;
using AssignList = InstList;

// Standard library containers with CfgLocalAllocator.
template <typename T> using CfgVector = std::vector<T, CfgLocalAllocator<T>>;
template <typename T> using CfgList = std::list<T, CfgLocalAllocator<T>>;

// Containers that are arena-allocated from the Cfg's allocator.
using OperandList = CfgVector<Operand *>;
using VarList = CfgVector<Variable *>;
using NodeList = CfgVector<CfgNode *>;

// Contains that use the default (global) allocator.
using ConstantList = std::vector<Constant *>;
using FunctionDeclarationList = std::vector<FunctionDeclaration *>;
using VariableDeclarationList = std::vector<VariableDeclaration *>;

/// SizeT is for holding small-ish limits like number of source operands in an
/// instruction. It is used instead of size_t (which may be 64-bits wide) when
/// we want to save space.
using SizeT = uint32_t;

/// InstNumberT is for holding an instruction number. Instruction numbers are
/// used for representing Variable live ranges.
using InstNumberT = int32_t;

/// A LiveBeginEndMapEntry maps a Variable::Number value to an Inst::Number
/// value, giving the instruction number that begins or ends a variable's live
/// range.
using LiveBeginEndMapEntry = std::pair<SizeT, InstNumberT>;
using LiveBeginEndMap = CfgVector<LiveBeginEndMapEntry>;
using LivenessBV = llvm::BitVector;

using TimerStackIdT = uint32_t;
using TimerIdT = uint32_t;

/// Use alignas(MaxCacheLineSize) to isolate variables/fields that might be
/// contended while multithreading. Assumes the maximum cache line size is 64.
enum { MaxCacheLineSize = 64 };
// Use ICE_CACHELINE_BOUNDARY to force the next field in a declaration
// list to be aligned to the next cache line.
// Note: zero is added to work around the following GCC 4.8 bug (fixed in 4.9):
//       https://gcc.gnu.org/bugzilla/show_bug.cgi?id=55382
#define ICE_CACHELINE_BOUNDARY                                                 \
  __attribute__((aligned(MaxCacheLineSize + 0))) int : 0

/// PNaCl is ILP32, so theoretically we should only need 32-bit offsets.
using RelocOffsetT = int32_t;
enum { RelocAddrSize = 4 };

enum LivenessMode {
  /// Basic version of live-range-end calculation. Marks the last uses of
  /// variables based on dataflow analysis. Records the set of live-in and
  /// live-out variables for each block. Identifies and deletes dead
  /// instructions (primarily stores).
  Liveness_Basic,

  /// In addition to Liveness_Basic, also calculate the complete live range for
  /// each variable in a form suitable for interference calculation and register
  /// allocation.
  Liveness_Intervals
};

enum RegAllocKind {
  RAK_Unknown,
  RAK_Global, /// full, global register allocation
  RAK_Phi,    /// infinite-weight Variables with active spilling/filling
  RAK_InfOnly /// allocation only for infinite-weight Variables
};

enum VerboseItem {
  IceV_None = 0,
  IceV_Instructions = 1 << 0,
  IceV_Deleted = 1 << 1,
  IceV_InstNumbers = 1 << 2,
  IceV_Preds = 1 << 3,
  IceV_Succs = 1 << 4,
  IceV_Liveness = 1 << 5,
  IceV_RegOrigins = 1 << 6,
  IceV_LinearScan = 1 << 7,
  IceV_Frame = 1 << 8,
  IceV_AddrOpt = 1 << 9,
  IceV_Random = 1 << 10,
  IceV_Folding = 1 << 11,
  IceV_RMW = 1 << 12,
  IceV_Loop = 1 << 13,
  IceV_All = ~IceV_None,
  IceV_Most = IceV_All & ~IceV_LinearScan
};
using VerboseMask = uint32_t;

enum FileType {
  FT_Elf, /// ELF .o file
  FT_Asm, /// Assembly .s file
  FT_Iasm /// "Integrated assembler" .byte-style .s file
};

using Ostream = llvm::raw_ostream;
using Fdstream = llvm::raw_fd_ostream;

using GlobalLockType = std::mutex;

enum ErrorCodes { EC_None = 0, EC_Args, EC_Bitcode, EC_Translation };

/// Wrapper around std::error_code for allowing multiple errors to be folded
/// into one. The current implementation keeps track of the first error, which
/// is likely to be the most useful one, and this could be extended to e.g.
/// collect a vector of errors.
class ErrorCode : public std::error_code {
  ErrorCode(const ErrorCode &) = delete;
  ErrorCode &operator=(const ErrorCode &) = delete;

public:
  ErrorCode() = default;
  void assign(ErrorCodes Code) {
    if (!HasError) {
      HasError = true;
      std::error_code::assign(Code, std::generic_category());
    }
  }
  void assign(int Code) { assign(static_cast<ErrorCodes>(Code)); }

private:
  bool HasError = false;
};

/// Reverse range adaptors written in terms of llvm::make_range().
template <typename T>
llvm::iterator_range<typename T::const_reverse_iterator>
reverse_range(const T &Container) {
  return llvm::make_range(Container.rbegin(), Container.rend());
}
template <typename T>
llvm::iterator_range<typename T::reverse_iterator> reverse_range(T &Container) {
  return llvm::make_range(Container.rbegin(), Container.rend());
}

/// Options for pooling and randomization of immediates.
enum RandomizeAndPoolImmediatesEnum { RPI_None, RPI_Randomize, RPI_Pool };

/// Salts for Random number generator for different randomization passes.
enum RandomizationPassesEnum {
  RPE_BasicBlockReordering,
  RPE_ConstantBlinding,
  RPE_FunctionReordering,
  RPE_GlobalVariableReordering,
  RPE_NopInsertion,
  RPE_PooledConstantReordering,
  RPE_RegAllocRandomization,
  RPE_num
};

} // end of namespace Ice

#endif // SUBZERO_SRC_ICEDEFS_H
