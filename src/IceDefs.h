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

namespace Ice {

class CfgNode;
class Constant;
class GlobalContext;
class Cfg;
class Inst;
class InstPhi;
class InstTarget;
class Operand;
class Variable;

// TODO: Switch over to LLVM's ADT container classes.
// http://llvm.org/docs/ProgrammersManual.html#picking-the-right-data-structure-for-a-task
typedef std::string IceString;
typedef std::list<Inst *> InstList;
typedef std::list<InstPhi *> PhiList;
typedef std::vector<Variable *> VarList;
typedef std::vector<CfgNode *> NodeList;

// SizeT is for holding small-ish limits like number of source
// operands in an instruction.  It is used instead of size_t (which
// may be 64-bits wide) when we want to save space.
typedef uint32_t SizeT;

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
  IceV_All = ~IceV_None
};
typedef uint32_t VerboseMask;

// The Ostream class wraps an output stream and a Cfg pointer, so
// that dump routines have access to the Cfg object and can print
// labels and variable names.

class Ostream {
public:
  Ostream(llvm::raw_ostream *Stream) : Stream(Stream) {}

  llvm::raw_ostream *Stream;

private:
  Ostream(const Ostream &) LLVM_DELETED_FUNCTION;
  Ostream &operator=(const Ostream &) LLVM_DELETED_FUNCTION;
};

template <typename T> inline Ostream &operator<<(Ostream &Str, const T &Val) {
  if (Str.Stream)
    (*Str.Stream) << Val;
  return Str;
}

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
