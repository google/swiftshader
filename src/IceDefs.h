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
#include <set>
#include <string>
#include <vector>
#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/BitVector.h"
#include "llvm/ADT/SmallBitVector.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/raw_ostream.h"

namespace Ice {

class Cfg;
class CfgNode;
class Constant;
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
class TargetLowering;
class Variable;
class VariableDeclaration;
class VariablesMetadata;

// TODO: Switch over to LLVM's ADT container classes.
// http://llvm.org/docs/ProgrammersManual.html#picking-the-right-data-structure-for-a-task
typedef std::string IceString;
typedef std::list<Inst *> InstList;
typedef std::list<InstAssign *> AssignList;
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

// A LiveBeginEndMapEntry maps a Variable::Number value to an
// Inst::Number value, giving the instruction number that begins or
// ends a variable's live range.
typedef std::pair<SizeT, InstNumberT> LiveBeginEndMapEntry;
typedef std::vector<LiveBeginEndMapEntry> LiveBeginEndMap;
typedef llvm::BitVector LivenessBV;

typedef uint32_t TimerStackIdT;
typedef uint32_t TimerIdT;

// PNaCl is ILP32, so theoretically we should only need 32-bit offsets.
typedef int32_t RelocOffsetT;

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
  IceV_AddrOpt = 1 << 10,
  IceV_All = ~IceV_None,
  IceV_Most = IceV_All & ~IceV_LinearScan
};
typedef uint32_t VerboseMask;

typedef llvm::raw_ostream Ostream;

} // end of namespace Ice

#endif // SUBZERO_SRC_ICEDEFS_H
