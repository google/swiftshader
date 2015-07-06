//===- subzero/src/IceRegAlloc.h - Linear-scan reg. allocation --*- C++ -*-===//
//
//                        The Subzero Code Generator
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file declares the LinearScan data structure used during
/// linear-scan register allocation, which holds the various work
/// queues for the linear-scan algorithm.
///
//===----------------------------------------------------------------------===//

#ifndef SUBZERO_SRC_ICEREGALLOC_H
#define SUBZERO_SRC_ICEREGALLOC_H

#include "IceDefs.h"
#include "IceTypes.h"

namespace Ice {

class LinearScan {
  LinearScan() = delete;
  LinearScan(const LinearScan &) = delete;
  LinearScan &operator=(const LinearScan &) = delete;

public:
  explicit LinearScan(Cfg *Func) : Func(Func) {}
  void init(RegAllocKind Kind);
  void scan(const llvm::SmallBitVector &RegMask, bool Randomized);
  void dump(Cfg *Func) const;

private:
  typedef std::vector<Variable *> OrderedRanges;
  typedef std::vector<Variable *> UnorderedRanges;

  void initForGlobal();
  void initForInfOnly();
  /// Move an item from the From set to the To set.  From[Index] is
  /// pushed onto the end of To[], then the item is efficiently removed
  /// from From[] by effectively swapping it with the last item in
  /// From[] and then popping it from the back.  As such, the caller is
  /// best off iterating over From[] in reverse order to avoid the need
  /// for special handling of the iterator.
  void moveItem(UnorderedRanges &From, SizeT Index, UnorderedRanges &To) {
    To.push_back(From[Index]);
    From[Index] = From.back();
    From.pop_back();
  }

  Cfg *const Func;
  OrderedRanges Unhandled;
  /// UnhandledPrecolored is a subset of Unhandled, specially collected
  /// for faster processing.
  OrderedRanges UnhandledPrecolored;
  UnorderedRanges Active, Inactive, Handled;
  std::vector<InstNumberT> Kills;
  bool FindPreference = false;
  bool FindOverlap = false;
  // TODO(stichnot): We're not really using FindOverlap yet, but we
  // may want a flavor of register allocation where FindPreference is
  // useful but we didn't want to initialize VMetadata with VMK_All
  // and therefore we can't safely allow overlap.
};

} // end of namespace Ice

#endif // SUBZERO_SRC_ICEREGALLOC_H
