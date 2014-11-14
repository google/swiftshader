//===- subzero/src/IceRegAlloc.h - Linear-scan reg. allocation --*- C++ -*-===//
//
//                        The Subzero Code Generator
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares the LinearScan data structure used during
// linear-scan register allocation, which holds the various work
// queues for the linear-scan algorithm.
//
//===----------------------------------------------------------------------===//

#ifndef SUBZERO_SRC_ICEREGALLOC_H
#define SUBZERO_SRC_ICEREGALLOC_H

#include "IceDefs.h"
#include "IceTypes.h"

namespace Ice {

class LinearScan {
  LinearScan(const LinearScan &) = delete;
  LinearScan &operator=(const LinearScan &) = delete;

public:
  LinearScan(Cfg *Func) : Func(Func) {}
  void initForGlobalAlloc();
  void scan(const llvm::SmallBitVector &RegMask);
  void dump(Cfg *Func) const;

private:
  Cfg *const Func;
  typedef std::vector<Variable *> OrderedRanges;
  typedef std::list<Variable *> UnorderedRanges;
  OrderedRanges Unhandled;
  // UnhandledPrecolored is a subset of Unhandled, specially collected
  // for faster processing.
  OrderedRanges UnhandledPrecolored;
  UnorderedRanges Active, Inactive, Handled;
  std::vector<InstNumberT> Kills;
};

} // end of namespace Ice

#endif // SUBZERO_SRC_ICEREGALLOC_H
