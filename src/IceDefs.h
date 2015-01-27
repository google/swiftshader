//===- subzero/src/IceDefs.h - Common Subzero declaraions -------*- C++ -*-===//
//
//                        The Subzero Code Generator
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares various useful types and classes that have
// widespread use across Subzero.  Every Subzero source file is
// expected to include IceDefs.h.
//
//===----------------------------------------------------------------------===//

#ifndef SUBZERO_SRC_ICEDEFS_H
#define SUBZERO_SRC_ICEDEFS_H

#include <cassert>
#include <cstdint>
#include <cstdio>     // snprintf
#include <functional> // std::less
#include <limits>
#include <list>
#include <map>
#include <mutex>
#include <string>
#include <system_error>
#include <vector>
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

#include "IceTLS.h"

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
class InstPhi;
class InstTarget;
class LiveRange;
class Liveness;
class Operand;
class TargetGlobalLowering;
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
  CfgLocalAllocator() = default;
  template <class U> CfgLocalAllocator(const CfgLocalAllocator<U> &) {}
  T *allocate(std::size_t Num) {
    return getCurrentCfgAllocator()->Allocate<T>(Num);
  }
  void deallocate(T *, std::size_t) {}
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

typedef std::string IceString;
typedef llvm::ilist<Inst> InstList;
// Ideally PhiList would be llvm::ilist<InstPhi>, and similar for
// AssignList, but this runs into issues with SFINAE.
typedef InstList PhiList;
typedef InstList AssignList;
// VarList and NodeList are arena-allocated from the Cfg's allocator.
typedef std::vector<Variable *, CfgLocalAllocator<Variable *>> VarList;
typedef std::vector<CfgNode *, CfgLocalAllocator<CfgNode *>> NodeList;
typedef std::vector<Constant *> ConstantList;

// SizeT is for holding small-ish limits like number of source
// operands in an instruction.  It is used instead of size_t (which
// may be 64-bits wide) when we want to save space.
typedef uint32_t SizeT;

// InstNumberT is for holding an instruction number.  Instruction
// numbers are used for representing Variable live ranges.
typedef int32_t InstNumberT;

// A LiveBeginEndMapEntry maps a Variable::Number value to an
// Inst::Number value, giving the instruction number that begins or
// ends a variable's live range.
typedef std::pair<SizeT, InstNumberT> LiveBeginEndMapEntry;
typedef std::vector<LiveBeginEndMapEntry,
                    CfgLocalAllocator<LiveBeginEndMapEntry>> LiveBeginEndMap;
typedef llvm::BitVector LivenessBV;

typedef uint32_t TimerStackIdT;
typedef uint32_t TimerIdT;

// Use alignas(MaxCacheLineSize) to isolate variables/fields that
// might be contended while multithreading.  Assumes the maximum cache
// line size is 64.
enum {
  MaxCacheLineSize = 64
};
// Use ICE_CACHELINE_BOUNDARY to force the next field in a declaration
// list to be aligned to the next cache line.
#define ICE_CACHELINE_BOUNDARY                                                 \
  alignas(MaxCacheLineSize) struct {}

// PNaCl is ILP32, so theoretically we should only need 32-bit offsets.
typedef int32_t RelocOffsetT;
enum { RelocAddrSize = 4 };

enum LivenessMode {
  // Basic version of live-range-end calculation.  Marks the last uses
  // of variables based on dataflow analysis.  Records the set of
  // live-in and live-out variables for each block.  Identifies and
  // deletes dead instructions (primarily stores).
  Liveness_Basic,

  // In addition to Liveness_Basic, also calculate the complete
  // live range for each variable in a form suitable for interference
  // calculation and register allocation.
  Liveness_Intervals
};

enum RegAllocKind {
  RAK_Global, // full, global register allocation
  RAK_InfOnly // allocation only for infinite-weight Variables
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
  IceV_All = ~IceV_None,
  IceV_Most = IceV_All & ~IceV_LinearScan
};
typedef uint32_t VerboseMask;

typedef llvm::raw_ostream Ostream;
typedef llvm::raw_fd_ostream Fdstream;

typedef std::mutex GlobalLockType;

enum ErrorCodes {
  EC_None = 0,
  EC_Args,
  EC_Bitcode,
  EC_Translation
};

// Wrapper around std::error_code for allowing multiple errors to be
// folded into one.  The current implementation keeps track of the
// first error, which is likely to be the most useful one, and this
// could be extended to e.g. collect a vector of errors.
class ErrorCode : public std::error_code {
  ErrorCode(const ErrorCode &) = delete;
  ErrorCode &operator=(const ErrorCode &) = delete;

public:
  ErrorCode() : HasError(false) {}
  void assign(ErrorCodes Code) {
    if (!HasError) {
      HasError = true;
      std::error_code::assign(Code, std::generic_category());
    }
  }
  void assign(int Code) { assign(static_cast<ErrorCodes>(Code)); }

private:
  bool HasError;
};

// Reverse range adaptors written in terms of llvm::make_range().
template <typename T>
llvm::iterator_range<typename T::const_reverse_iterator>
reverse_range(const T &Container) {
  return llvm::make_range(Container.rbegin(), Container.rend());
}
template <typename T>
llvm::iterator_range<typename T::reverse_iterator> reverse_range(T &Container) {
  return llvm::make_range(Container.rbegin(), Container.rend());
}

} // end of namespace Ice

#endif // SUBZERO_SRC_ICEDEFS_H
