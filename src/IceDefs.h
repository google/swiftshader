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

#include <stdint.h> // TODO: <cstdint> with C++11

#include <cassert>
#include <cstdio>     // snprintf
#include <functional> // std::less
#include <list>
#include <map>
#include <set>
#include <string>
#include <vector>

#include "llvm/ADT/BitVector.h"
#include "llvm/ADT/SmallBitVector.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/Compiler.h" // LLVM_STATIC_ASSERT
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/Timer.h"

// Roll our own static_assert<> in the absence of C++11.  TODO: change
// to static_assert<> with C++11.
template <bool> struct staticAssert;
template <> struct staticAssert<true> {}; // only true is defined
#define STATIC_ASSERT(x) staticAssert<(x)>()

namespace Ice {

class Cfg;
class CfgNode;
class Constant;
class GlobalContext;
class Inst;
class InstPhi;
class InstTarget;
class LiveRange;
class Liveness;
class Operand;
class TargetLowering;
class Variable;

// TODO: Switch over to LLVM's ADT container classes.
// http://llvm.org/docs/ProgrammersManual.html#picking-the-right-data-structure-for-a-task
typedef std::string IceString;
typedef std::list<Inst *> InstList;
typedef std::list<InstPhi *> PhiList;
typedef std::vector<Variable *> VarList;
typedef std::vector<Operand *> OperandList;
typedef std::vector<CfgNode *> NodeList;
typedef std::vector<Constant *> ConstantList;

// SizeT is for holding small-ish limits like number of source
// operands in an instruction.  It is used instead of size_t (which
// may be 64-bits wide) when we want to save space.
typedef uint32_t SizeT;

// InstNumberT is for holding an instruction number.  Instruction
// numbers are used for representing Variable live ranges.
typedef int32_t InstNumberT;

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

enum VerboseItem {
  IceV_None = 0,
  IceV_Instructions = 1 << 0,
  IceV_Deleted = 1 << 1,
  IceV_InstNumbers = 1 << 2,
  IceV_Preds = 1 << 3,
  IceV_Succs = 1 << 4,
  IceV_Liveness = 1 << 5,
  IceV_RegManager = 1 << 6,
  IceV_RegOrigins = 1 << 7,
  IceV_LinearScan = 1 << 8,
  IceV_Frame = 1 << 9,
  IceV_Timing = 1 << 10,
  IceV_AddrOpt = 1 << 11,
  IceV_All = ~IceV_None
};
typedef uint32_t VerboseMask;

typedef llvm::raw_ostream Ostream;

// TODO: Implement in terms of std::chrono after switching to C++11.
class Timer {
public:
  Timer() : Start(llvm::TimeRecord::getCurrentTime(false)) {}
  uint64_t getElapsedNs() const { return getElapsedSec() * 1000 * 1000 * 1000; }
  uint64_t getElapsedUs() const { return getElapsedSec() * 1000 * 1000; }
  uint64_t getElapsedMs() const { return getElapsedSec() * 1000; }
  double getElapsedSec() const {
    llvm::TimeRecord End = llvm::TimeRecord::getCurrentTime(false);
    return End.getWallTime() - Start.getWallTime();
  }
  void printElapsedUs(GlobalContext *Ctx, const IceString &Tag) const;

private:
  const llvm::TimeRecord Start;
  Timer(const Timer &) LLVM_DELETED_FUNCTION;
  Timer &operator=(const Timer &) LLVM_DELETED_FUNCTION;
};

} // end of namespace Ice

#endif // SUBZERO_SRC_ICEDEFS_H
